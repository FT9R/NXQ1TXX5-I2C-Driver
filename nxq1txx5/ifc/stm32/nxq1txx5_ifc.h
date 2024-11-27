#pragma once

#include "i2c.h"
#include <stdbool.h>

typedef enum {
    NXQ1TXX5_IFC_STATUS_UNDEFINED,
    NXQ1TXX5_IFC_STATUS_IDLE = 10,
    NXQ1TXX5_IFC_STATUS_BUSY_RX,
    NXQ1TXX5_IFC_STATUS_BUSY_TX,
    NXQ1TXX5_IFC_STATUS_ERROR_RX,
    NXQ1TXX5_IFC_STATUS_ERROR_TX,
    NXQ1TXX5_IFC_STATUS_ERROR_ARG
} nxq1txx5_ifc_status_t;

typedef nxq1txx5_ifc_status_t (*NXQ1TXX5_I2C1_Transfer_fp)(uint8_t regAddress, uint16_t *data);

/**
 * @brief The one of I2C blocking receive function templates
 * @param regAddress Register address to read from
 * @param data Pointer to the buffer where 2x8bit data will be saved
 * @return Status of the data transfer request operation
 */
nxq1txx5_ifc_status_t NXQ1TXX5_I2C1_Receive(uint8_t regAddress, uint16_t *data);

/**
 * @brief The one of I2C blocking transmit function templates
 * @param regAddress Register address to write to
 * @param data Pointer to the buffer from which 2x8bit data is being sent
 * @return Status of the data transfer request operation
 */
nxq1txx5_ifc_status_t NXQ1TXX5_I2C1_Transmit(uint8_t regAddress, uint16_t *data);

/**
 * @brief The one of I2C IT receive function templates
 * @param regAddress Register address to read from
 * @param data Pointer to the buffer where 2x8bit data will be saved
 * @return Status of the data transfer request operation
 */
nxq1txx5_ifc_status_t NXQ1TXX5_I2C1_Receive_IT(uint8_t regAddress, uint16_t *data);

/**
 * @brief The one of I2C IT transmit function templates
 * @param regAddress Register address to write to
 * @param data Pointer to the buffer from which 2x8bit data is being sent
 * @return Status of the data transfer request operation
 */
nxq1txx5_ifc_status_t NXQ1TXX5_I2C1_Transmit_IT(uint8_t regAddress, uint16_t *data);

/**
 * @brief The one of I2C DMA receive function templates
 * @param regAddress Register address to read from
 * @param data Pointer to the buffer where 2x8bit data will be saved
 * @return Status of the data transfer request operation
 */
nxq1txx5_ifc_status_t NXQ1TXX5_I2C1_Receive_DMA(uint8_t regAddress, uint16_t *data);

/**
 * @brief The one of I2C DMA transmit function templates
 * @param regAddress Register address to write to
 * @param data Pointer to the buffer from which 2x8bit data is being sent
 * @return Status of the data transfer request operation
 */
nxq1txx5_ifc_status_t NXQ1TXX5_I2C1_Transmit_DMA(uint8_t regAddress, uint16_t *data);