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
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "queue.h"
#include "usbd_cdc_if.h"
#include "sw_timers.h"
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
Queue q_from_spirx;
Queue q_from_usbrx;

uint8_t test_buf[MAX_STRING_LENGTH] = {0};

volatile bool exchanged = true;
volatile bool usb_transmit = true;
volatile bool while_lock = false;

uint8_t spi_transmit_buff[MAX_STRING_LENGTH];
uint8_t spi_receive_buff[MAX_STRING_LENGTH];

uint8_t usb_transmit_buff[MAX_STRING_LENGTH];
uint8_t usb_receive_buff[MAX_STRING_LENGTH];

struct timer t_resolver;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if(hspi->Instance == hspi1.Instance)
	{
		exchanged = true;
		queue_push(&q_from_spirx, spi_receive_buff, MAX_STRING_LENGTH);
		HAL_GPIO_WritePin(SPI_EN_GPIO_Port, SPI_EN_Pin, GPIO_PIN_SET);
	}
}
//

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM6)
	{
		t_OnDigitCompleteInterrupt();
	}
}
//

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == BLE_IRQ_Pin)
	{
		if(HAL_GPIO_ReadPin(BLE_IRQ_GPIO_Port, BLE_IRQ_Pin) == GPIO_PIN_SET && exchanged && !while_lock)
		{
			memset(spi_transmit_buff, 0, sizeof(spi_transmit_buff[0])*MAX_STRING_LENGTH);
			memset(spi_receive_buff, 0, sizeof(spi_receive_buff[0])*MAX_STRING_LENGTH);
			queue_get_front(&q_from_usbrx, spi_transmit_buff, 0,  queue_get_frontl(&q_from_usbrx));
			queue_pop(&q_from_usbrx);
			HAL_GPIO_WritePin(SPI_EN_GPIO_Port, SPI_EN_Pin, GPIO_PIN_RESET);
			exchanged = false;
			if(HAL_SPI_TransmitReceive_DMA(&hspi1, spi_transmit_buff, spi_receive_buff, MAX_STRING_LENGTH) != HAL_OK)
				{
					Error_Handler();
				}
		}
	}
}
//

/**
	*	@brief	Функция приема сообщения с CDC
	*	@note		см. функцию @ref CDC_Receive_FS
	*	@param	
	*	@retval	
**/
//
void Connection_RecieveCallback(uint8_t* buffer, uint32_t Len)
{
	if(Len > 0)
		queue_push(&q_from_usbrx, buffer, Len);
}
//


/**
	*	@brief	Функция приема сообщения с CDC
	*	@note		см. функцию @ref CDC_Transmit_FS
	*	@param	
	*	@retval	
**/
//
void Connection_TransmitCallback(void)
{
	//usb_transmit = true;
}


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
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  MX_USB_DEVICE_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */
	HAL_TIM_Base_Start_IT(&htim6);
	queue_init(&q_from_spirx);
	queue_init(&q_from_usbrx);
  HAL_GPIO_WritePin(BLE_Boot_GPIO_Port, BLE_Boot_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(BLE_nRST_GPIO_Port, BLE_nRST_Pin, GPIO_PIN_SET);
	HAL_Delay(1000);
	HAL_GPIO_WritePin(BLE_nRST_GPIO_Port, BLE_nRST_Pin, GPIO_PIN_RESET);
	HAL_Delay(500);
	HAL_GPIO_WritePin(BLE_nRST_GPIO_Port, BLE_nRST_Pin, GPIO_PIN_SET);
  /* USER CODE END 2 */
 
 

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	
	//заполнение тестового массива данных
	for(int i = 0; i < MAX_STRING_LENGTH; i++)
	{
		test_buf[i] = i;
	}
	
	Timer_set(&t_resolver, 500, NULL, true, false, true);
	
  while (1)
  {
		t_OnDigitCompleteContinuous();
		
		if(exchanged == true)
			{
				while_lock = true;
				if(!queue_isempty(&q_from_usbrx))
					{
						memset(spi_transmit_buff, 0, sizeof(spi_transmit_buff[0])*MAX_STRING_LENGTH);
						queue_get_front(&q_from_usbrx, spi_transmit_buff, 0,  MAX_STRING_LENGTH);
						queue_pop(&q_from_usbrx);
						HAL_GPIO_WritePin(SPI_EN_GPIO_Port, SPI_EN_Pin, GPIO_PIN_RESET);
						exchanged = false;
						if(HAL_SPI_TransmitReceive_DMA(&hspi1, spi_transmit_buff, spi_receive_buff, MAX_STRING_LENGTH) != HAL_OK)
							{
								Error_Handler();
							}
					}
				while_lock = false;
			}
		if(!queue_isempty(&q_from_spirx) /*&& usb_transmit == true*/)
			{
				memset(usb_transmit_buff, 0, MAX_STRING_LENGTH);
				queue_get_front(&q_from_spirx, usb_transmit_buff, 0,  MAX_STRING_LENGTH);
				queue_pop(&q_from_spirx);
				usb_transmit = false;
				usb_transmit_buff[MAX_STRING_LENGTH-1] = '\n';
				usb_transmit_buff[MAX_STRING_LENGTH-2] = '\r';
				usb_transmit_buff[MAX_STRING_LENGTH-3] = '#';
				CDC_Transmit_FS(usb_transmit_buff, MAX_STRING_LENGTH);
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 40;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_USB;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLLSAI1;
  PeriphClkInit.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_MSI;
  PeriphClkInit.PLLSAI1.PLLSAI1M = 1;
  PeriphClkInit.PLLSAI1.PLLSAI1N = 24;
  PeriphClkInit.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV7;
  PeriphClkInit.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_48M2CLK;
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
