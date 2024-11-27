/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "gpio.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "nxq1txx5.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
nxq1txx5_t *nxq = &(nxq1txx5_t){0};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{

    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_ADC1_Init();
    MX_USART1_UART_Init();
    MX_ADC2_Init();
    MX_TIM3_Init();
    MX_TIM4_Init();
    MX_I2C1_Init();
    /* USER CODE BEGIN 2 */
    IR_START;
    HAL_ADCEx_Calibration_Start(&ADC_LS);
    HAL_ADCEx_InjectedStart_IT(&ADC_LS);
    HAL_TIM_Base_Start(&ADC_LS_TIM); // Trigger the low speed ADC conversion

#pragma region NXQ_DRIVER_USAGE
    NXQ_STBY_OFF; // Not implemented by driver because of poor pin functionality (used once after power up)
    NXQ1TXX5_Link(nxq, NXQ1TXX5_I2C1_Receive, NXQ1TXX5_I2C1_Transmit); // Blocking transfer test
    NXQ1TXX5_Charge(nxq, NXQ1TXX5_CHARGE_OFF);
    HAL_Delay(5000);
    NXQ1TXX5_Charge(nxq, NXQ1TXX5_CHARGE_ON);

    /* Wait for the device to switch to charging mode.
     * Otherwise not all device registers are updated with valid values */
    while (true)
    {
        NXQ1TXX5_SystemStatus(nxq);
        if (nxq->operation == NXQ1TXX5_OPERATION_CHARGE)
        {
            /* NXQ1TXX5 enters to charge mode to check for
             * a receiver on the transmitter base.
             * So we should ensure that QI receiver is really
             * placed by requesting operation again after ~1000ms. */
            HAL_Delay(1000);
            NXQ1TXX5_SystemStatus(nxq);
            if (nxq->operation == NXQ1TXX5_OPERATION_CHARGE)
                break;
        }
    }
    NXQ1TXX5_Link(nxq, NXQ1TXX5_I2C1_Receive_IT, NXQ1TXX5_I2C1_Transmit_IT); // IT transfer test
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1)
    {

        static int taskStep = 0;
        if (nxq->process == NXQ1TXX5_PROCESS_IDLE) // Wait until the current task is finished
        {
            switch (taskStep)
            {
            case 0:
                NXQ1TXX5_Temperature(nxq);
                break;
            case 1:
                NXQ1TXX5_SupplyVoltage(nxq);
                break;
            case 2:
                NXQ1TXX5_NTCVoltage(nxq);
                taskStep = -1; // All tasks complete, reset counter
                break;
            }
            taskStep++;
        }
#pragma endregion NXQ_DRIVER_USAGE

        HAL_GPIO_TogglePin(RGB_R_GPIO_Port, RGB_R_Pin | RGB_G_Pin | RGB_B_Pin);
        HAL_Delay(1);
        HAL_GPIO_TogglePin(RGB_R_GPIO_Port, RGB_R_Pin | RGB_G_Pin | RGB_B_Pin);
        HAL_Delay(100);
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
    }
    /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        Error_Handler();
    }
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
    PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
        Error_Handler();
    }

    /** Enables the Clock Security System
     */
    HAL_RCC_EnableCSS();
}

/* USER CODE BEGIN 4 */
void HAL_ADCEx_InjectedConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    static float temperature;
    static float vin;
    static float irFront;
    static float irBack;

    temperature = ADC_TemperatureGet(HAL_ADCEx_InjectedGetValue(&ADC_LS, ADC_INJECTED_RANK_1));
    vin = ADC_VinGet(HAL_ADCEx_InjectedGetValue(&ADC_LS, ADC_INJECTED_RANK_2));
    irFront = ADC_LsbToVoltage(HAL_ADCEx_InjectedGetValue(&ADC_LS, ADC_INJECTED_RANK_3));
    irBack = ADC_LsbToVoltage(HAL_ADCEx_InjectedGetValue(&ADC_LS, ADC_INJECTED_RANK_4));

    HAL_ADCEx_InjectedStart_IT(&ADC_LS);
    // HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin); // FIXME: look at the ADC sampling freq
}

void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    NXQ1TXX5_Callback(nxq);
}

void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    NXQ1TXX5_Callback(nxq);
}
/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1) {}
    /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
