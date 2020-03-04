/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
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
#include "adc.h"
#include "dma.h"
#include "lptim.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include "queue.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum spi_transer_stages{
  spi_stage_idle = 0,
  spi_stage_accept_sending,
  spi_stage_data_receiving
} ESpiTransferStage;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
Queue q_from_spirx;
Queue q_from_usbrx;

ESpiTransferStage geSpiTransferStage = spi_stage_idle;

volatile bool exchanged = true;
volatile bool uart_received = true;
volatile bool while_lock = false;

uint8_t spi_transmit_buff[MAX_STRING_LENGTH];
uint8_t spi_receive_buff[MAX_STRING_LENGTH];

uint8_t usb_transmit_buff[MAX_STRING_LENGTH];
uint8_t usb_receive_buff[MAX_STRING_LENGTH];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi){

	if(hspi->Instance == hspi1.Instance){

    HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET);

    switch (geSpiTransferStage){

      case spi_stage_accept_sending:
        
        HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_RESET);
        
        memset(spi_receive_buff, 0, sizeof(spi_receive_buff[0])*MAX_STRING_LENGTH);
			  memset(spi_transmit_buff, 0, sizeof(spi_transmit_buff[0])*MAX_STRING_LENGTH);
        
        HAL_SPI_TransmitReceive_DMA(&hspi1, spi_transmit_buff, spi_receive_buff, MAX_STRING_LENGTH);
				// TODO: if (result != HAL_OK) Error_Handler();
				
        geSpiTransferStage = spi_stage_data_receiving;
      break;

      case spi_stage_data_receiving:
        queue_push(&q_from_spirx, spi_receive_buff, MAX_STRING_LENGTH);
        geSpiTransferStage = spi_stage_idle;
      break;
    }
	}
}
//

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){

	uart_received = true;
	queue_push(&q_from_usbrx, usb_receive_buff, MAX_STRING_LENGTH);
}
//

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){

	if(GPIO_Pin == BLE_IRQ_Pin){

		if(HAL_GPIO_ReadPin(BLE_IRQ_GPIO_Port, BLE_IRQ_Pin) == GPIO_PIN_SET){

      HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_RESET);
      
      spi_transmit_buff[0] = 0xAA;			
      HAL_SPI_TransmitReceive_DMA(&hspi1, spi_transmit_buff, spi_receive_buff, 1);			
		}
	}
}

//
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
  MX_LPTIM1_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
	printf( "Core=%d, %d MHz\r\n", SystemCoreClock, SystemCoreClock / 1000000);
	printf("APB1 = %d\r\n", HAL_RCC_GetPCLK1Freq());
	printf("APB2 = %d\r\n", HAL_RCC_GetPCLK2Freq());

  /* USER CODE END 2 */
 
 

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1){
    if( !queue_isempty(&q_from_usbrx) ){

      memset(spi_transmit_buff, 0, sizeof(spi_transmit_buff[0])*MAX_STRING_LENGTH);

      queue_get_front(&q_from_usbrx, spi_transmit_buff, 0,  MAX_STRING_LENGTH);
      queue_pop(&q_from_usbrx);

      HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_RESET);
      HAL_SPI_TransmitReceive_DMA(&hspi1, spi_transmit_buff, spi_receive_buff, MAX_STRING_LENGTH);
      // TODO:  if( result != HAL_OK) Error_Handler();
          
    }
			
		if( !queue_isempty(&q_from_spirx) ){

      memset(usb_transmit_buff, 0, MAX_STRING_LENGTH);

      queue_get_front(&q_from_spirx, usb_transmit_buff, 0,  MAX_STRING_LENGTH);
      queue_pop(&q_from_spirx);

      usb_transmit_buff[MAX_STRING_LENGTH-1] = '\n';
      usb_transmit_buff[MAX_STRING_LENGTH-2] = '\r';
      usb_transmit_buff[MAX_STRING_LENGTH-3] = '#';
        
      HAL_UART_Transmit_DMA(&huart1, usb_transmit_buff, MAX_STRING_LENGTH);
    }

		if(uart_received){
      
			memset(usb_receive_buff, 0, sizeof(usb_receive_buff[0])*MAX_STRING_LENGTH);
			HAL_UART_Receive_DMA(&huart1, usb_receive_buff, MAX_STRING_LENGTH);
			uart_received = false;
		}
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

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 3;
  RCC_OscInitStruct.PLL.PLLN = 8;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_LPTIM1
                              |RCC_PERIPHCLK_ADC;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.Lptim1ClockSelection = RCC_LPTIM1CLKSOURCE_LSI;
  PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_SYSCLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure the main internal regulator output voltage 
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
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
void assert_failed(char *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
