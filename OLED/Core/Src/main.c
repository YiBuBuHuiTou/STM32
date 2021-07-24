/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "data_code_tbl.h"
#include "oled.h"
#include "delay.h"
#include <stdlib.h>
#include <string.h>
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
uint8_t  first_flag = 1;
uint8_t err = 0;
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
  MX_I2C1_Init();
  MX_TIM7_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  	oled_init();
    oled_fill(0xFF);
    HAL_Delay(1000);
    oled_fill(0x00);
	for(unsigned char index = 0; index < 4; index++) {
		for (unsigned char page =0; page < 2; page++) {
			oled_SetPos(31 + 16 * index, page +2);
			HAL_I2C_Mem_Write(&hi2c1, I2C_SLAVE_OLED_ADDRESS, I2C_SLAVE_OLED_DATA, I2C_MEMADD_SIZE_8BIT, &cxdcln[index][page * 16], sizeof(cxdcln[0])/2, 1000);
		}
	}
	while (1) {
		HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
		HAL_GPIO_WritePin(ultrasonic_trig_GPIO_Port, ultrasonic_trig_Pin, GPIO_PIN_SET);
		delay_us(20);
		HAL_GPIO_WritePin(ultrasonic_trig_GPIO_Port, ultrasonic_trig_Pin, GPIO_PIN_RESET);
		first_flag = 1;
		while(HAL_GPIO_ReadPin(ultrasonic_echo_GPIO_Port, ultrasonic_echo_Pin) == GPIO_PIN_RESET){
			if (first_flag) {
				first_flag = 0;
				tim7_start();
			}
			if (time_diff() > 0x50000) {
				err = 1;
				break;
			}
		}
		tim7_stop();
		if(err) {
			break;
		}

		first_flag = 1;
		while(HAL_GPIO_ReadPin(ultrasonic_echo_GPIO_Port, ultrasonic_echo_Pin) == GPIO_PIN_SET) {
			if (first_flag) {
				first_flag = 0;
				tim7_start();
			}
			if (time_diff() > 0x50000) {
				err = 1;
				break;
			}
		}
		if(err) {
			tim7_stop();
			break;
		}

		uint32_t diff = time_diff();
		tim7_stop();
		uint32_t dis = 340 * diff / 20000;
     	for(uint8_t index = 0; index < 3; index ++) {
     		for( uint8_t page = 0; page < 2; page++) {
				oled_SetPos(16 * index, page);
				HAL_I2C_Mem_Write(&hi2c1, I2C_SLAVE_OLED_ADDRESS, I2C_SLAVE_OLED_DATA, I2C_MEMADD_SIZE_8BIT, &ceju[index][page * 16], sizeof(ceju[0])/2, 1000);
			}
		}

     	oled_clear(48, 127, 0, 1);
     	uint8_t buff[10] = {0};
     	uint8_t data[16] = {0};
     	itoa(dis, &buff[0], 10);
     	for(uint8_t index = 0; index < sizeof(buff); index++) {
     		if(buff[index] == '\0') {
     			break;
     		}
     		memcpy(data,num[buff[index]-48],sizeof(data));
     		for (uint8_t page = 0; page < 2; page++) {
     			oled_SetPos(48 + 8 * index, page);
				HAL_I2C_Mem_Write(&hi2c1, I2C_SLAVE_OLED_ADDRESS, I2C_SLAVE_OLED_DATA, I2C_MEMADD_SIZE_8BIT, &data[page * 8], sizeof(data)/2, 1000);
     		}

     	}
     	HAL_Delay(500);
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 15;
  RCC_OscInitStruct.PLL.PLLN = 216;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

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
while (1) {
}
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
