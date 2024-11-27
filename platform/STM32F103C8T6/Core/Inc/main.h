/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "arm_math.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define ADC_LS_TIM htim4
#define ADC_HS_TIM htim3
#define ADC_HS hadc1
#define ADC_LS hadc2
#define RGB_R_Pin GPIO_PIN_0
#define RGB_R_GPIO_Port GPIOA
#define RGB_G_Pin GPIO_PIN_1
#define RGB_G_GPIO_Port GPIOA
#define RGB_B_Pin GPIO_PIN_2
#define RGB_B_GPIO_Port GPIOA
#define TEMP_Pin GPIO_PIN_3
#define TEMP_GPIO_Port GPIOA
#define QI_CURRENT_Pin GPIO_PIN_4
#define QI_CURRENT_GPIO_Port GPIOA
#define QI_VOLTAGE_Pin GPIO_PIN_5
#define QI_VOLTAGE_GPIO_Port GPIOA
#define VIN_Pin GPIO_PIN_6
#define VIN_GPIO_Port GPIOA
#define IR_FRONT_Pin GPIO_PIN_7
#define IR_FRONT_GPIO_Port GPIOA
#define IR_PU_Pin GPIO_PIN_0
#define IR_PU_GPIO_Port GPIOB
#define IR_BACK_Pin GPIO_PIN_1
#define IR_BACK_GPIO_Port GPIOB
#define IR_EN_Pin GPIO_PIN_2
#define IR_EN_GPIO_Port GPIOB
#define QI_STBY_Pin GPIO_PIN_14
#define QI_STBY_GPIO_Port GPIOB
#define LED_Pin GPIO_PIN_12
#define LED_GPIO_Port GPIOA
#define SOLENOID_Pin GPIO_PIN_3
#define SOLENOID_GPIO_Port GPIOB
#define USART1_DE_Pin GPIO_PIN_5
#define USART1_DE_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
/* IR sensor control */
#define IR_START                                                       \
    do                                                                 \
    {                                                                  \
        HAL_GPIO_WritePin(IR_PU_GPIO_Port, IR_PU_Pin, GPIO_PIN_SET);   \
        HAL_GPIO_WritePin(IR_EN_GPIO_Port, IR_EN_Pin, GPIO_PIN_RESET); \
    }                                                                  \
    while (0)
#define IR_STOP                                                        \
    do                                                                 \
    {                                                                  \
        HAL_GPIO_WritePin(IR_PU_GPIO_Port, IR_PU_Pin, GPIO_PIN_RESET); \
        HAL_GPIO_WritePin(IR_EN_GPIO_Port, IR_EN_Pin, GPIO_PIN_SET);   \
    }                                                                  \
    while (0)

/* NXQ standby control */
#define NXQ_STBY_OFF HAL_GPIO_WritePin(QI_STBY_GPIO_Port, QI_STBY_Pin, GPIO_PIN_RESET)
#define NXQ_STBY_ON  HAL_GPIO_WritePin(QI_STBY_GPIO_Port, QI_STBY_Pin, GPIO_PIN_SET)
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
