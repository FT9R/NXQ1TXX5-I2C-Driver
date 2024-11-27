#include "nxq1txx5_ifc.h"

// #define NXQ1TXX5_SLAVE_ADDRESS (uint8_t) 0x0e // Not documented
#define NXQ1TXX5_SLAVE_ADDRESS (uint8_t) 0x34

#define TIMEOUT_RX 1000
#define TIMEOUT_TX 1000

nxq1txx5_ifc_status_t NXQ1TXX5_I2C1_Receive(uint8_t regAddress, uint16_t *data)
{
    if (data == NULL)
        return NXQ1TXX5_IFC_STATUS_ERROR_ARG;

    return HAL_I2C_Mem_Read(&hi2c1, NXQ1TXX5_SLAVE_ADDRESS << 1, regAddress, I2C_MEMADD_SIZE_8BIT, (uint8_t *) data, 2,
                            TIMEOUT_RX) == HAL_OK
               ? NXQ1TXX5_IFC_STATUS_IDLE
               : NXQ1TXX5_IFC_STATUS_ERROR_RX;
}

nxq1txx5_ifc_status_t NXQ1TXX5_I2C1_Transmit(uint8_t regAddress, uint16_t *data)
{
    if (data == NULL)
        return NXQ1TXX5_IFC_STATUS_ERROR_ARG;

    return HAL_I2C_Mem_Write(&hi2c1, NXQ1TXX5_SLAVE_ADDRESS << 1, regAddress, I2C_MEMADD_SIZE_8BIT, (uint8_t *) data, 2,
                             TIMEOUT_TX) == HAL_OK
               ? NXQ1TXX5_IFC_STATUS_IDLE
               : NXQ1TXX5_IFC_STATUS_ERROR_TX;
}

nxq1txx5_ifc_status_t NXQ1TXX5_I2C1_Receive_IT(uint8_t regAddress, uint16_t *data)
{
    if (data == NULL)
        return NXQ1TXX5_IFC_STATUS_ERROR_ARG;

    return HAL_I2C_Mem_Read_IT(&hi2c1, NXQ1TXX5_SLAVE_ADDRESS << 1, regAddress, I2C_MEMADD_SIZE_8BIT, (uint8_t *) data,
                               2) == HAL_OK
               ? NXQ1TXX5_IFC_STATUS_BUSY_RX
               : NXQ1TXX5_IFC_STATUS_ERROR_RX;
}

nxq1txx5_ifc_status_t NXQ1TXX5_I2C1_Transmit_IT(uint8_t regAddress, uint16_t *data)
{
    if (data == NULL)
        return NXQ1TXX5_IFC_STATUS_ERROR_ARG;

    return HAL_I2C_Mem_Write_IT(&hi2c1, NXQ1TXX5_SLAVE_ADDRESS << 1, regAddress, I2C_MEMADD_SIZE_8BIT, (uint8_t *) data,
                                2) == HAL_OK
               ? NXQ1TXX5_IFC_STATUS_BUSY_TX
               : NXQ1TXX5_IFC_STATUS_ERROR_TX;
}

nxq1txx5_ifc_status_t NXQ1TXX5_I2C1_Receive_DMA(uint8_t regAddress, uint16_t *data)
{
    if (data == NULL)
        return NXQ1TXX5_IFC_STATUS_ERROR_ARG;

    return HAL_I2C_Mem_Read_DMA(&hi2c1, NXQ1TXX5_SLAVE_ADDRESS << 1, regAddress, I2C_MEMADD_SIZE_8BIT, (uint8_t *) data,
                                2) == HAL_OK
               ? NXQ1TXX5_IFC_STATUS_BUSY_RX
               : NXQ1TXX5_IFC_STATUS_ERROR_RX;
}

nxq1txx5_ifc_status_t NXQ1TXX5_I2C1_Transmit_DMA(uint8_t regAddress, uint16_t *data)
{
    if (data == NULL)
        return NXQ1TXX5_IFC_STATUS_ERROR_ARG;

    return HAL_I2C_Mem_Write_DMA(&hi2c1, NXQ1TXX5_SLAVE_ADDRESS << 1, regAddress, I2C_MEMADD_SIZE_8BIT,
                                 (uint8_t *) data, 2) == HAL_OK
               ? NXQ1TXX5_IFC_STATUS_BUSY_TX
               : NXQ1TXX5_IFC_STATUS_ERROR_TX;
}