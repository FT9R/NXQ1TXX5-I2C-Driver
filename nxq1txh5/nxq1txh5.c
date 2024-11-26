#include "nxq1txh5.h"

#define SYSTEM_STATUS_REGISTER      0x00
#define SUPPLY_STATUS_REGISTER      0x02
#define TEMPERATURE_STATUS_REGISTER 0x03
#define NTC_STATUS_REGISTER         0x07
#define ON_OFF_REGISTER             0x10

#define BYTE_SWAP(REG) (((REG) >> 8) & 0x00ff) | (((REG) << 8) & 0xff00);

static void Transfer(nxq1txh5_t *handle, NXQ1TXH5_I2C1_Transfer_fp fpTransfer, uint8_t regAddress, nxq1txh5_reg_t *reg);

void NXQ1TXH5_Link(nxq1txh5_t *handle, NXQ1TXH5_I2C1_Transfer_fp fpReceive, NXQ1TXH5_I2C1_Transfer_fp fpTransmit)
{
    if (handle == NULL)
        return;

    /* Check any existing errors */
    if (handle->error != NXQ1TXH5_ERROR_NONE)
        return;

    /* Set operational state */
    handle->state = NXQ1TXH5_STATE_LINK;

    if (fpReceive == NULL || fpTransmit == NULL)
    {
        handle->error = NXQ1TXH5_ERROR_ARGUMENT;
        return;
    }

    /* Link platform functions to the device */
    handle->Interface.receive = fpReceive;
    handle->Interface.transmit = fpTransmit;

    /* Set operational state */
    handle->state = NXQ1TXH5_STATE_IDLE;
}

void NXQ1TXH5_Charge(nxq1txh5_t *handle, nxq1txh5_charge_t charge)
{
    nxq1txh5_reg_t reg = {0};

    if (handle == NULL)
        return;

    /* Check any existing errors */
    if (handle->error != NXQ1TXH5_ERROR_NONE)
        return;

    /* Set operational state */
    handle->state = NXQ1TXH5_STATE_CHARGE_SWITCH;

    /* Write to register */
    switch (charge)
    {
    case NXQ1TXH5_CHARGE_ON:
        reg.chargeCtrl.disable = 0;
        break;

    case NXQ1TXH5_CHARGE_OFF:
        reg.chargeCtrl.disable = 1;
        break;
    }
    Transfer(handle, handle->Interface.transmit, ON_OFF_REGISTER, &reg);
    if (handle->error != NXQ1TXH5_ERROR_NONE)
        return;

    /* Set operational state */
    handle->state = NXQ1TXH5_STATE_IDLE;
}

void NXQ1TXH5_SystemStatus(nxq1txh5_t *handle)
{
    nxq1txh5_reg_t reg = {0};

    if (handle == NULL)
        return;

    /* Check any existing errors */
    if (handle->error != NXQ1TXH5_ERROR_NONE)
        return;

    /* Set operational state */
    handle->state = NXQ1TXH5_STATE_SYSTEM_STATUS;

    /* Read from register */
    Transfer(handle, handle->Interface.receive, SYSTEM_STATUS_REGISTER, &reg);
    if (handle->error != NXQ1TXH5_ERROR_NONE)
        return;

    /* Update handle fields */
    handle->supplyStatus = reg.systemStatus.supplyStatus == 1 ? true : false;
    switch (reg.systemStatus.status)
    {
    case 0b00000:
        handle->operation = NXQ1TXH5_OPERATION_SHUT;
        break;

    case 0b10000:
        handle->operation = NXQ1TXH5_OPERATION_CHARGE;
        break;

    default:
        handle->operation = NXQ1TXH5_OPERATION_UNDEFINED;
        break;
    }

    /* Set operational state */
    handle->state = NXQ1TXH5_STATE_IDLE;
}

void NXQ1TXH5_Temperature(nxq1txh5_t *handle)
{
    nxq1txh5_reg_t reg = {0};
    int16_t temperature = 0;

    if (handle == NULL)
        return;

    /* Check any existing errors */
    if (handle->error != NXQ1TXH5_ERROR_NONE)
        return;

    /* Set operational state */
    handle->state = NXQ1TXH5_STATE_TEMPERATURE;

    /* Read from register */
    Transfer(handle, handle->Interface.receive, TEMPERATURE_STATUS_REGISTER, &reg);
    if (handle->error != NXQ1TXH5_ERROR_NONE)
        return;

    /* Calculate the temperature from 2-complement code */
    if (reg.temperatureStatus.temperatureLevel & 0x100)
        temperature = reg.raw | 0xfe00; // Negative value, sign-extend
    else
        temperature = reg.raw; // Positive value

    /* Update handle fields */
    if (temperature != -256) // Skip when device is not in charge mode
        handle->temperature = temperature;

    /* Set operational state */
    handle->state = NXQ1TXH5_STATE_IDLE;
}

void NXQ1TXH5_SupplyVoltage(nxq1txh5_t *handle)
{
    nxq1txh5_reg_t reg = {0};
    float supplyVoltage = 0;

    if (handle == NULL)
        return;

    /* Check any existing errors */
    if (handle->error != NXQ1TXH5_ERROR_NONE)
        return;

    /* Set operational state */
    handle->state = NXQ1TXH5_STATE_SUPPLY_VOLTAGE;

    /* Read from register */
    Transfer(handle, handle->Interface.receive, SUPPLY_STATUS_REGISTER, &reg);
    if (handle->error != NXQ1TXH5_ERROR_NONE)
        return;

    /* Calculate the voltage */
    supplyVoltage = ((float) reg.supplyStatus.supplyLevel * 6) / 0x3ff;

    /* Update handle fields */
    if (supplyVoltage > 0) // Skip when device is not in charge mode
        handle->supplyVoltage = supplyVoltage;

    /* Set operational state */
    handle->state = NXQ1TXH5_STATE_IDLE;
}

void NXQ1TXH5_NTCVoltage(nxq1txh5_t *handle)
{
    nxq1txh5_reg_t reg = {0};
    float ntcVoltage = 0;

    if (handle == NULL)
        return;

    /* Check any existing errors */
    if (handle->error != NXQ1TXH5_ERROR_NONE)
        return;

    /* Set operational state */
    handle->state = NXQ1TXH5_STATE_NTC_VOLTAGE;

    /* Read from register */
    Transfer(handle, handle->Interface.receive, NTC_STATUS_REGISTER, &reg);
    if (handle->error != NXQ1TXH5_ERROR_NONE)
        return;

    /* Calculate the voltage */
    ntcVoltage = ((float) reg.supplyStatus.supplyLevel * 1.5) / 0x3ff;

    /* Update handle fields */
    if (ntcVoltage > 0) // Skip when device is not in charge mode
        handle->ntcVoltage = ntcVoltage;

    /* Set operational state */
    handle->state = NXQ1TXH5_STATE_IDLE;
}

/**
 * @section Private func definitions
 */
static void Transfer(nxq1txh5_t *handle, NXQ1TXH5_I2C1_Transfer_fp fpTransfer, uint8_t regAddress, nxq1txh5_reg_t *reg)
{
    uint16_t data = 0;

    if ((handle == NULL) || (fpTransfer == NULL) || (reg == NULL))
        return;

    /* Check any existing errors */
    if (handle->error != NXQ1TXH5_ERROR_NONE)
        return;

    /* Check the transfer function to be used and begin the transmission */
    if (fpTransfer == handle->Interface.receive)
    {
        /* Receive data */
        if (fpTransfer(regAddress, &data) != NXQ1TXH5_STATUS_SUCCESS)
            handle->error = NXQ1TXH5_ERROR_I2C_RX;

        /* Prepare data to match the register byte order */
        reg->raw = BYTE_SWAP(data);
    }
    else if (fpTransfer == handle->Interface.transmit)
    {
        /* Prepare data to match the register byte order */
        data = BYTE_SWAP(reg->raw);

        /* Transmit data */
        if (fpTransfer(regAddress, &data) != NXQ1TXH5_STATUS_SUCCESS)
            handle->error = NXQ1TXH5_ERROR_I2C_TX;
    }
}