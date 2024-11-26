#pragma once

#include "i2c.h"
#include <stdbool.h>

typedef enum { NXQ1TXH5_STATUS_SUCCESS = 10, NXQ1TXH5_STATUS_ERROR } nxq1txh5_status_t;

typedef nxq1txh5_status_t (*NXQ1TXH5_I2C1_Transfer_fp)(uint8_t regAddress, uint16_t *data);

/**
 * @brief The one of I2C receive function templates
 * @param regAddress Register address to read from
 * @param data Pointer to the buffer where 2x8bit data will be saved
 * @return Status of the data transfer request operation
 */
nxq1txh5_status_t NXQ1TXH5_I2C1_Receive(uint8_t regAddress, uint16_t *data);

/**
 * @brief The one of I2C transmit function templates
 * @param regAddress Register address to write to
 * @param data Pointer to the buffer from which 2x8bit data is being sent
 * @return Status of the data transfer request operation
 */
nxq1txh5_status_t NXQ1TXH5_I2C1_Transmit(uint8_t regAddress, uint16_t *data);