#pragma once

#include "../ifc/stm32/nxq1txx5_ifc.h"

/* Reference data types */
typedef enum { NXQ1TXX5_ERROR_NONE, NXQ1TXX5_ERROR_ARGUMENT = 20, NXQ1TXX5_ERROR_IFC } nxq1txx5_error_t;

typedef enum {
    NXQ1TXX5_OPERATION_UNDEFINED,
    NXQ1TXX5_OPERATION_IDLE = 30,
    NXQ1TXX5_OPERATION_CHARGE,
    NXQ1TXX5_OPERATION_SHUT
} nxq1txx5_operation_t;

typedef enum {
    NXQ1TXX5_PROCESS_UNDEFINED,
    NXQ1TXX5_PROCESS_IDLE = 40,
    NXQ1TXX5_PROCESS_LINK,
    NXQ1TXX5_PROCESS_CHARGE_SWITCH,
    NXQ1TXX5_PROCESS_SYSTEM_STATUS,
    NXQ1TXX5_PROCESS_TEMPERATURE,
    NXQ1TXX5_PROCESS_SUPPLY_VOLTAGE,
    NXQ1TXX5_PROCESS_NTC_VOLTAGE
} nxq1txx5_process_t;

typedef enum { NXQ1TXX5_CHARGE_ON = 50, NXQ1TXX5_CHARGE_OFF } nxq1txx5_charge_t;

/* Write registers */
typedef struct
{
    uint16_t disable : 1;
    uint16_t reserved : 15;
} nxq1txx5_charge_ctrl_reg_t;

/* Read registers */
typedef struct
{
    uint16_t supplyStatus : 1;
    uint16_t reserved : 9;
    uint16_t status : 5;
    uint16_t reserved1 : 1;
} nxq1txx5_system_status_reg_t;

typedef struct
{
    uint16_t supplyLevel : 10;
    uint16_t reserved : 6;
} nxq1txx5_supply_status_reg_t;

typedef struct
{
    uint16_t temperatureLevel : 9;
    uint16_t reserved : 7;
} nxq1txx5_temperature_status_reg_t;

typedef struct
{
    uint16_t ntcLevel : 10;
    uint16_t reserved : 6;
} nxq1txx5_ntc_status_reg_t;

/* Common register */
typedef union
{
    nxq1txx5_charge_ctrl_reg_t chargeCtrl;
    nxq1txx5_system_status_reg_t systemStatus;
    nxq1txx5_supply_status_reg_t supplyStatus;
    nxq1txx5_temperature_status_reg_t temperatureStatus;
    nxq1txx5_ntc_status_reg_t ntcStatus;
    uint16_t raw;
} nxq1txx5_reg_t;

/* Device handle */
typedef struct
{
    struct
    {
        NXQ1TXX5_I2C1_Transfer_fp receive;
        NXQ1TXX5_I2C1_Transfer_fp transmit;
        nxq1txx5_ifc_status_t status;
        nxq1txx5_reg_t reg;
    } Interface;

    nxq1txx5_process_t process; // Current process, executed by driver
    nxq1txx5_error_t error;
    nxq1txx5_operation_t operation; // Device operation mode
    bool supplyStatus; // 1 - Vdd level is OK, 0 - Vdd level is < 1.2V
    float supplyVoltage;
    int16_t temperature;
    float ntcVoltage;
} nxq1txx5_t;

/**
 * @brief Links user defined platform functions to the target device
 * @param handle Pointer to the target device handle
 * @param fpReceive Pointer to the platform receive function
 * @param fpTransmit Pointer to the platform transmit function
 * @note This function has no effect if driver serves any process
 */
void NXQ1TXX5_Link(nxq1txx5_t *handle, NXQ1TXX5_I2C1_Transfer_fp fpReceive, NXQ1TXX5_I2C1_Transfer_fp fpTransmit);

/**
 * @brief Sets the QI charger into ON/OFF state
 * @param handle Pointer to the target device handle
 * @param charge New state for QI charger
 */
void NXQ1TXX5_Charge(nxq1txx5_t *handle, nxq1txx5_charge_t charge);

/**
 * @brief Updates handle's `supplyStatus` and `operation` fields
 * @param handle Pointer to the target device handle
 * @note This function should be called twice with interval ~1000ms to ensure that device operation is steady-state
 */
void NXQ1TXX5_SystemStatus(nxq1txx5_t *handle);

/**
 * @brief Updates handle's `temperature` field
 * @param handle Pointer to the target device handle
 */
void NXQ1TXX5_Temperature(nxq1txx5_t *handle);

/**
 * @brief Updates handle's `supplyVoltage` field
 * @param handle Pointer to the target device handle
 */
void NXQ1TXX5_SupplyVoltage(nxq1txx5_t *handle);

/**
 * @brief Updates handle's `ntcVoltage` field
 * @param handle Pointer to the target device handle
 * @note Internal NXQ's ADC reference is 1.5V, so retrieved value can't excess this value
 */
void NXQ1TXX5_NTCVoltage(nxq1txx5_t *handle);

/**
 * @brief Driver callback function to serve RX/TX transfers
 * @param handle Pointer to the target device handle
 * @note Put it inside RX and TX IRQ handlers or HAL callbacks
 */
void NXQ1TXX5_Callback(nxq1txx5_t *handle);