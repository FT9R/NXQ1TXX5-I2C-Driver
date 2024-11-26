#pragma once

#include "ifc/stm32/nxq1txh5_ifc.h"

/* Reference data types */
typedef enum {
    NXQ1TXH5_ERROR_NONE,
    NXQ1TXH5_ERROR_ARGUMENT = 20,
    NXQ1TXH5_ERROR_I2C_RX,
    NXQ1TXH5_ERROR_I2C_TX
} nxq1txh5_error_t;

typedef enum {
    NXQ1TXH5_OPERATION_UNDEFINED,
    NXQ1TXH5_OPERATION_IDLE = 30,
    NXQ1TXH5_OPERATION_CHARGE,
    NXQ1TXH5_OPERATION_SHUT
} nxq1txh5_operation_t;

typedef enum {
    NXQ1TXH5_STATE_UNDEFINED,
    NXQ1TXH5_STATE_IDLE = 40,
    NXQ1TXH5_STATE_LINK,
    NXQ1TXH5_STATE_CHARGE_SWITCH,
    NXQ1TXH5_STATE_SYSTEM_STATUS,
    NXQ1TXH5_STATE_TEMPERATURE,
    NXQ1TXH5_STATE_SUPPLY_VOLTAGE,
    NXQ1TXH5_STATE_NTC_VOLTAGE
} nxq1txh5_state_t;

typedef enum { NXQ1TXH5_CHARGE_ON = 50, NXQ1TXH5_CHARGE_OFF } nxq1txh5_charge_t;

/* Write registers */
typedef struct
{
    uint16_t disable : 1;
    uint16_t reserved : 15;
} nxq1txh5_charge_ctrl_reg_t;

/* Read registers */
typedef struct
{
    uint16_t supplyStatus : 1;
    uint16_t reserved : 9;
    uint16_t status : 5;
    uint16_t reserved1 : 1;
} nxq1txh5_system_status_reg_t;

typedef struct
{
    uint16_t supplyLevel : 10;
    uint16_t reserved : 6;
} nxq1txh5_supply_status_reg_t;

typedef struct
{
    uint16_t temperatureLevel : 9;
    uint16_t reserved : 7;
} nxq1txh5_temperature_status_reg_t;

typedef struct
{
    uint16_t ntcLevel : 10;
    uint16_t reserved : 6;
} nxq1txh5_ntc_status_reg_t;

/* Common register */
typedef union
{
    nxq1txh5_charge_ctrl_reg_t chargeCtrl;
    nxq1txh5_system_status_reg_t systemStatus;
    nxq1txh5_supply_status_reg_t supplyStatus;
    nxq1txh5_temperature_status_reg_t temperatureStatus;
    nxq1txh5_ntc_status_reg_t ntcStatus;
    uint16_t raw;
} nxq1txh5_reg_t;

/* Device handle */
typedef struct
{
    struct
    {
        NXQ1TXH5_I2C1_Transfer_fp receive;
        NXQ1TXH5_I2C1_Transfer_fp transmit;
    } Interface;

    nxq1txh5_state_t state;
    nxq1txh5_error_t error;

    nxq1txh5_operation_t operation;
    bool supplyStatus; // 1 - Vdd level is OK, 0 - Vdd level is < 1.2V
    float supplyVoltage;
    int16_t temperature;
    float ntcVoltage;
} nxq1txh5_t;

/**
 * @brief Links user defined platform functions to the target device
 * @param handle Pointer to the target device handle
 * @param fpReceive Pointer to the platform receive function
 * @param fpTransmit Pointer to the platform transmit function
 */
void NXQ1TXH5_Link(nxq1txh5_t *handle, NXQ1TXH5_I2C1_Transfer_fp fpReceive, NXQ1TXH5_I2C1_Transfer_fp fpTransmit);

/**
 * @brief Sets the QI charger into ON/OFF state
 * @param handle Pointer to the target device handle
 * @param charge New state for QI charger
 */
void NXQ1TXH5_Charge(nxq1txh5_t *handle, nxq1txh5_charge_t charge);

/**
 * @brief Updates handle's `supplyStatus` and `operation` fields
 * @param handle Pointer to the target device handle
 * @note This function should be called twice with interval ~500ms to ensure that device operation is steady-state
 */
void NXQ1TXH5_SystemStatus(nxq1txh5_t *handle);

/**
 * @brief Updates handle's `temperature` field
 * @param handle Pointer to the target device handle
 */
void NXQ1TXH5_Temperature(nxq1txh5_t *handle);

/**
 * @brief Updates handle's `supplyVoltage` field
 * @param handle Pointer to the target device handle
 */
void NXQ1TXH5_SupplyVoltage(nxq1txh5_t *handle);

/**
 * @brief Updates handle's `ntcVoltage` field
 * @param handle Pointer to the target device handle
 * @note Internal NXQ's ADC reference is 1.5V, so retrieved value can't excess this value
 */
void NXQ1TXH5_NTCVoltage(nxq1txh5_t *handle);