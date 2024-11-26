#include "nxq1txh5_ifc.h"

// #define NXQ1TXH5_SLAVE_ADDRESS (uint8_t) 0x0e // Not documented
#define NXQ1TXH5_SLAVE_ADDRESS (uint8_t) 0x34

#define TIMEOUT_RX 1000
#define TIMEOUT_TX 1000

nxq1txh5_status_t NXQ1TXH5_I2C1_Receive(uint8_t regAddress, uint16_t *data)
{
    if (data == NULL)
        return NXQ1TXH5_STATUS_ERROR;

    return HAL_I2C_Mem_Read(&hi2c1, NXQ1TXH5_SLAVE_ADDRESS << 1, regAddress, I2C_MEMADD_SIZE_8BIT, (uint8_t *) data, 2,
                            TIMEOUT_RX) == HAL_OK
               ? NXQ1TXH5_STATUS_SUCCESS
               : NXQ1TXH5_STATUS_ERROR;
}

nxq1txh5_status_t NXQ1TXH5_I2C1_Transmit(uint8_t regAddress, uint16_t *data)
{
    if (data == NULL)
        return NXQ1TXH5_STATUS_ERROR;

    return HAL_I2C_Mem_Write(&hi2c1, NXQ1TXH5_SLAVE_ADDRESS << 1, regAddress, I2C_MEMADD_SIZE_8BIT, (uint8_t *) data, 2,
                             TIMEOUT_TX) == HAL_OK
               ? NXQ1TXH5_STATUS_SUCCESS
               : NXQ1TXH5_STATUS_ERROR;
}