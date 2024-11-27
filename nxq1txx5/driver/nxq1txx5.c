#include "nxq1txx5.h"

#define SYSTEM_STATUS_REGISTER      0x00
#define SUPPLY_STATUS_REGISTER      0x02
#define TEMPERATURE_STATUS_REGISTER 0x03
#define NTC_STATUS_REGISTER         0x07
#define ON_OFF_REGISTER             0x10

#define BYTE_SWAP(REG) (((REG) >> 8) & 0x00ff) | (((REG) << 8) & 0xff00);

static void Transfer(nxq1txx5_t *handle, NXQ1TXX5_I2C1_Transfer_fp fpTransfer, uint8_t regAddress);

void NXQ1TXX5_Link(nxq1txx5_t *handle, NXQ1TXX5_I2C1_Transfer_fp fpReceive, NXQ1TXX5_I2C1_Transfer_fp fpTransmit)
{
    if (handle == NULL)
        return;

    /* Check any existing errors */
    if (handle->error != NXQ1TXX5_ERROR_NONE)
        return;

    /* Ensure that previous process is completed */
    /* Force idle process in case of the first call */
    if (handle->process == NXQ1TXX5_PROCESS_UNDEFINED)
        handle->process = NXQ1TXX5_PROCESS_IDLE;
    if (handle->process == NXQ1TXX5_PROCESS_IDLE)
    {
        /* Start current process */
        handle->process = NXQ1TXX5_PROCESS_LINK;

        /* Check interface functions */
        if (fpReceive == NULL || fpTransmit == NULL)
        {
            handle->error = NXQ1TXX5_ERROR_ARGUMENT;
            return;
        }

        /* Link platform functions to the device */
        handle->Interface.receive = fpReceive;
        handle->Interface.transmit = fpTransmit;

        /* Finish current process */
        handle->process = NXQ1TXX5_PROCESS_IDLE;
    }
}

void NXQ1TXX5_Charge(nxq1txx5_t *handle, nxq1txx5_charge_t charge)
{
    if (handle == NULL)
        return;

    /* Check any existing errors */
    if (handle->error != NXQ1TXX5_ERROR_NONE)
        return;

    /* Ensure that previous process is completed */
    if (handle->process == NXQ1TXX5_PROCESS_IDLE)
    {
        /* Start current process */
        handle->process = NXQ1TXX5_PROCESS_CHARGE_SWITCH;

        /* Write to register */
        handle->Interface.reg.raw = 0;
        switch (charge)
        {
        case NXQ1TXX5_CHARGE_ON:
            handle->Interface.reg.chargeCtrl.disable = 0;
            break;

        case NXQ1TXX5_CHARGE_OFF:
            handle->Interface.reg.chargeCtrl.disable = 1;
            break;
        }
        Transfer(handle, handle->Interface.transmit, ON_OFF_REGISTER);
        if (handle->error != NXQ1TXX5_ERROR_NONE)
            return;
    }

    /* Continue execution only if transfer is complete */
    if ((handle->process != NXQ1TXX5_PROCESS_CHARGE_SWITCH) || (handle->Interface.status != NXQ1TXX5_IFC_STATUS_IDLE))
        return;

    /* Finish current process */
    handle->process = NXQ1TXX5_PROCESS_IDLE;
}

void NXQ1TXX5_SystemStatus(nxq1txx5_t *handle)
{
    if (handle == NULL)
        return;

    /* Check any existing errors */
    if (handle->error != NXQ1TXX5_ERROR_NONE)
        return;

    /* Ensure that previous process is completed */
    if (handle->process == NXQ1TXX5_PROCESS_IDLE)
    {
        /* Start current process */
        handle->process = NXQ1TXX5_PROCESS_SYSTEM_STATUS;

        /* Read from register */
        handle->Interface.reg.raw = 0;
        handle->supplyStatus = false;
        handle->operation = NXQ1TXX5_OPERATION_UNDEFINED;
        Transfer(handle, handle->Interface.receive, SYSTEM_STATUS_REGISTER);
        if (handle->error != NXQ1TXX5_ERROR_NONE)
            return;
    }

    /* Continue execution only if transfer is complete */
    if ((handle->process != NXQ1TXX5_PROCESS_SYSTEM_STATUS) || (handle->Interface.status != NXQ1TXX5_IFC_STATUS_IDLE))
        return;

    /* Update handle fields */
    handle->supplyStatus = handle->Interface.reg.systemStatus.supplyStatus == 1 ? true : false;
    switch (handle->Interface.reg.systemStatus.status)
    {
    case 0b00000:
        handle->operation = NXQ1TXX5_OPERATION_SHUT;
        break;

    case 0b10000:
        handle->operation = NXQ1TXX5_OPERATION_CHARGE;
        break;

    default:
        handle->operation = NXQ1TXX5_OPERATION_UNDEFINED;
        break;
    }

    /* Finish current process */
    handle->process = NXQ1TXX5_PROCESS_IDLE;
}

void NXQ1TXX5_Temperature(nxq1txx5_t *handle)
{
    int16_t temperature = 0;

    if (handle == NULL)
        return;

    /* Check any existing errors */
    if (handle->error != NXQ1TXX5_ERROR_NONE)
        return;

    /* Ensure that previous process is completed */
    if (handle->process == NXQ1TXX5_PROCESS_IDLE)
    {
        /* Start current process */
        handle->process = NXQ1TXX5_PROCESS_TEMPERATURE;

        /* Read from register */
        handle->Interface.reg.raw = 0;
        handle->temperature = 0;
        Transfer(handle, handle->Interface.receive, TEMPERATURE_STATUS_REGISTER);
        if (handle->error != NXQ1TXX5_ERROR_NONE)
            return;
    }

    /* Continue execution only if transfer is complete */
    if ((handle->process != NXQ1TXX5_PROCESS_TEMPERATURE) || (handle->Interface.status != NXQ1TXX5_IFC_STATUS_IDLE))
        return;

    /* Calculate the temperature from 2-complement code */
    if (handle->Interface.reg.temperatureStatus.temperatureLevel & 0x100)
        temperature = handle->Interface.reg.raw | 0xfe00; // Negative value, sign-extend
    else
        temperature = handle->Interface.reg.raw; // Positive value

    /* Update handle fields */
    if (temperature != -256) // Skip when device is not in charge mode
        handle->temperature = temperature;

    /* Finish current process */
    handle->process = NXQ1TXX5_PROCESS_IDLE;
}

void NXQ1TXX5_SupplyVoltage(nxq1txx5_t *handle)
{
    float supplyVoltage = 0;

    if (handle == NULL)
        return;

    /* Check any existing errors */
    if (handle->error != NXQ1TXX5_ERROR_NONE)
        return;

    /* Ensure that previous process is completed */
    if (handle->process == NXQ1TXX5_PROCESS_IDLE)
    {
        /* Start current process */
        handle->process = NXQ1TXX5_PROCESS_SUPPLY_VOLTAGE;

        /* Read from register */
        handle->Interface.reg.raw = 0;
        handle->supplyVoltage = 0;
        Transfer(handle, handle->Interface.receive, SUPPLY_STATUS_REGISTER);
        if (handle->error != NXQ1TXX5_ERROR_NONE)
            return;
    }

    /* Continue execution only if transfer is complete */
    if ((handle->process != NXQ1TXX5_PROCESS_SUPPLY_VOLTAGE) || (handle->Interface.status != NXQ1TXX5_IFC_STATUS_IDLE))
        return;

    /* Calculate the voltage */
    supplyVoltage = ((float) handle->Interface.reg.supplyStatus.supplyLevel * 6) / 0x3ff;

    /* Update handle fields */
    if (supplyVoltage > 0) // Skip when device is not in charge mode
        handle->supplyVoltage = supplyVoltage;

    /* Finish current process */
    handle->process = NXQ1TXX5_PROCESS_IDLE;
}

void NXQ1TXX5_NTCVoltage(nxq1txx5_t *handle)
{
    float ntcVoltage = 0;

    if (handle == NULL)
        return;

    /* Check any existing errors */
    if (handle->error != NXQ1TXX5_ERROR_NONE)
        return;

    /* Ensure that previous process is completed */
    if (handle->process == NXQ1TXX5_PROCESS_IDLE)
    {
        /* Start current process */
        handle->process = NXQ1TXX5_PROCESS_NTC_VOLTAGE;

        /* Read from register */
        handle->Interface.reg.raw = 0;
        handle->ntcVoltage = 0;
        Transfer(handle, handle->Interface.receive, NTC_STATUS_REGISTER);
        if (handle->error != NXQ1TXX5_ERROR_NONE)
            return;
    }

    /* Continue execution only if transfer is complete */
    if ((handle->process != NXQ1TXX5_PROCESS_NTC_VOLTAGE) || (handle->Interface.status != NXQ1TXX5_IFC_STATUS_IDLE))
        return;

    /* Calculate the voltage */
    ntcVoltage = ((float) handle->Interface.reg.supplyStatus.supplyLevel * 1.5) / 0x3ff;

    /* Update handle fields */
    if (ntcVoltage > 0) // Skip when device is not in charge mode
        handle->ntcVoltage = ntcVoltage;

    /* Finish current process */
    handle->process = NXQ1TXX5_PROCESS_IDLE;
}

void NXQ1TXX5_Callback(nxq1txx5_t *handle)
{
    if (handle->error == NXQ1TXX5_ERROR_NONE)
    {
        if ((handle->Interface.status == NXQ1TXX5_IFC_STATUS_BUSY_RX) ||
            (handle->Interface.status == NXQ1TXX5_IFC_STATUS_BUSY_TX))
        {
            /* Prepare data to match the register byte order */
            if (handle->Interface.status == NXQ1TXX5_IFC_STATUS_BUSY_RX)
                handle->Interface.reg.raw = BYTE_SWAP(handle->Interface.reg.raw);

            /* Transfer is completed, continue the current process */
            handle->Interface.status = NXQ1TXX5_IFC_STATUS_IDLE;
            switch (handle->process)
            {
            case NXQ1TXX5_PROCESS_CHARGE_SWITCH:
                NXQ1TXX5_Charge(handle, NULL);
                break;

            case NXQ1TXX5_PROCESS_SYSTEM_STATUS:
                NXQ1TXX5_SystemStatus(handle);
                break;

            case NXQ1TXX5_PROCESS_TEMPERATURE:
                NXQ1TXX5_Temperature(handle);
                break;

            case NXQ1TXX5_PROCESS_SUPPLY_VOLTAGE:
                NXQ1TXX5_SupplyVoltage(handle);
                break;

            case NXQ1TXX5_PROCESS_NTC_VOLTAGE:
                NXQ1TXX5_NTCVoltage(handle);
                break;
            }
        }
    }
}

/**
 * @section Private func definitions
 */
static void Transfer(nxq1txx5_t *handle, NXQ1TXX5_I2C1_Transfer_fp fpTransfer, uint8_t regAddress)
{
    nxq1txx5_ifc_status_t *status;

    if ((handle == NULL) || (fpTransfer == NULL))
        return;

    /* Check any existing errors */
    if (handle->error != NXQ1TXX5_ERROR_NONE)
        return;

    /* Check tranfer function */
    if (fpTransfer == handle->Interface.receive) {}
    else if (fpTransfer == handle->Interface.transmit)
    {
        /* Prepare data to match the register byte order */
        handle->Interface.reg.raw = BYTE_SWAP(handle->Interface.reg.raw);
    }
    else
    {
        handle->error = NXQ1TXX5_ERROR_ARGUMENT;
        return;
    }

    /* Start data transfer */
    status = &handle->Interface.status;
    *status = fpTransfer(regAddress, &handle->Interface.reg.raw);
    if ((*status != NXQ1TXX5_IFC_STATUS_IDLE) && (*status != NXQ1TXX5_IFC_STATUS_BUSY_RX) &&
        (*status != NXQ1TXX5_IFC_STATUS_BUSY_TX))
    {
        handle->error = NXQ1TXX5_ERROR_IFC;
        return;
    }

    /* Prepare data to match the register byte order,
     * if blocking mechanism has been used */
    if (*status == NXQ1TXX5_IFC_STATUS_IDLE)
        handle->Interface.reg.raw = BYTE_SWAP(handle->Interface.reg.raw);
}