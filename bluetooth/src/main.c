
/******************** (C) COPYRIGHT 2018 STMicroelectronics ********************
* File Name          : BLE_Throughput_main.c
* Author             : RF Application Team
* Version            : 1.1.0
* Date               : 09-February-2016
* Description        : Code demostrating the BLE Throughput applications
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/**
 * @file BLE_Throughput_main.c
 * @brief This is a Throughput demo that shows how to implement a simple throughput test  between two BlueNRG-1,2 devices.
 * Two throughput test options are available: 
 * unidirectional (server notification to client side); 
 * bidirectional (server notification to client side and write without response from client to server side).
 * 
**/
    
/** @addtogroup BlueNRG1_demonstrations_applications
 * BlueNRG-1 throughput demo \see BLE_Throughput_main.c for documentation.
 *
 *@{
 */

/** @} */
/** \cond DOXYGEN_SHOULD_SKIP_THIS
 */
/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "BlueNRG1_it.h"
#include "BlueNRG1_conf.h"
#include "ble_const.h" 
#include "bluenrg1_stack.h"
#include "gp_timer.h"
#include "sleep.h"
#include "app_state.h"
#include "throughput.h"
#include "SDK_EVAL_Config.h"
#include "queue.h"

#if CLIENT
#include "Throughput_client_config.h"
#define SPI_IN_Pin	GPIO_Pin_3
#define SPI_OUT_Pin	GPIO_Pin_2
#define SPI_CS_Pin	GPIO_Pin_11
#define SPI_CLK_Pin	GPIO_Pin_8

const uint32_t BLE_IRQ = GPIO_Pin_6;
#else
#include "Throughput_server_config.h"
#define SPI_IN_Pin	GPIO_Pin_3
#define SPI_OUT_Pin	GPIO_Pin_2
#define SPI_CS_Pin	GPIO_Pin_1
#define SPI_CLK_Pin	GPIO_Pin_0

const uint32_t BLE_IRQ = GPIO_Pin_6;
#endif

#include "queue.h"
#include "messaging.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define BLE_THROUGHPUT_VERSION_STRING "1.0.0" 
#define BUFF_SIZE                   	(10)
/* Private macro -------------------------------------------------------------*/
extern uint16_t connection_handle;
/* Private variables ---------------------------------------------------------*/
struct timer t_updater;
Queue q_ble_rx;
Queue q_spi_rx;

uint8_t input_buffer_spi[MAX_STRING_LENGTH] = {0};
uint8_t output_buffer_spi[MAX_STRING_LENGTH] = {0};
uint8_t output_buffer_ble[MAX_STRING_LENGTH] = {0};

uint8_t test_buffer[MAX_STRING_LENGTH] = {0};

uint8_t bluenrg_stack = 0;

FlagStatus gpio_toggle = RESET;

FlagStatus spi_eor = SET;
FlagStatus spi_eot = SET;

FlagStatus spi_receive_enabled = RESET;
FlagStatus spi_transmit_enabled = RESET;

volatile uint16_t bytes_to_receive = MAX_STRING_LENGTH;
volatile uint16_t bytes_to_send = MAX_STRING_LENGTH;

volatile uint8_t inbuffer_idx = 0;
volatile uint8_t outbuffer_idx = 0;

/* Private function prototypes -----------------------------------------------*/
void InitiateSPISend(uint8_t* buffer, uint8_t len);
void InitiateSPIRec(void);

_Bool processSPI(void);
_Bool processAttributes(Queue rx, Queue tx);
void Exchange(void);
void Exchange_v2(void);
void SPI_Slave_Configuration(void);
/* Private functions ---------------------------------------------------------*/
uint32_t Timer_Elapsed(struct timer *t)
{
	uint32_t ct = Clock_Time() ;
	if ( ct > t->start )
		return Clock_Time() - t->start;
	else 
		return 0xFFFFFFFF - t->start + ct;
}
//

int main(void) 
 {
	uint8_t ret;
	
  /* System Init */
  SystemInit();
  
  /* Identify BlueNRG1 platform */
  SdkEvalIdentification();
  
  /* Init Clock */
  Clock_Init();

  /* Init the UART peripheral */
  SdkEvalComUartInit(UART_BAUDRATE);
	
	/* SPI initialization */
	SPI_Slave_Configuration();

  /* BlueNRG-1 stack init */
  ret = BlueNRG_Stack_Initialization(&BlueNRG_Stack_Init_params);
  if (ret != BLE_STATUS_SUCCESS) {
    printf("Error in BlueNRG_Stack_Initialization() 0x%02x\r\n", ret);
    while(1);
  }
  
  printf("BlueNRG-1 BLE Throughput Server Application (version: %s, %d,%d,%d)\r\n", BLE_THROUGHPUT_VERSION_STRING,MAX_ATT_MTU,PREPARE_WRITE_LIST_SIZE,MBLOCKS_COUNT);

  /* Init Throughput test */
  ret = THROUGHPUT_DeviceInit();
  if (ret != BLE_STATUS_SUCCESS) {
    printf("THROUGHPUT_DeviceInit()--> Failed 0x%02x\r\n", ret);
    while(1);
  }
  
  printf("BLE Stack Initialized\r\n");
	
	queue_init(&q_ble_rx);
	queue_init(&q_spi_rx);
	
	printf("Rx queues initialized\r\n");
	
	Timer_Set(&t_updater, CLOCK_SECOND/1000);
	bluenrg_stack = BLE_STACK_CONFIGURATION;
	
	GPIO_SetBits(BLE_IRQ);         
	
  while(1) {
    /* BlueNRG-1 stack tick */
    BTLE_StackTick();
		
		/* Application tick */
    APP_Tick(Exchange_v2);
  }
  
} /* end main() */
//

/**
  * @brief 	Функция переброса данных из SPI периферии в BLE
						Является более-менее рабочей, отправка/прием по SPI раздельны
						См. примеры в DK 3.1.0 - SPI DMA Master/Slave
  */
void Exchange(void)
{
	#if SERVER
	if(DMA_CH_SPI_RX->CCR_b.EN == RESET && queue_isempty(&q_spi_rx))
	{
		DMA_CH_SPI_RX->CCR_b.EN = SET;
//		spi_eot = RESET;
	}
	if(DMA_CH_SPI_TX->CCR_b.EN == RESET && queue_isempty(&q_spi_rx))
	{
		DMA_CH_SPI_TX->CCR_b.EN = SET;
//		spi_eot = RESET;
	}
	#else
//	if(DMA_CH_SPI_RX->CCR_b.EN == SET && queue_isempty(&q_spi_rx))
//	{
//		DMA_CH_SPI_RX->CCR_b.EN = RESET;
////		spi_eot = RESET;
//	}
//	if(DMA_CH_SPI_TX->CCR_b.EN == SET && queue_isempty(&q_spi_rx))
//	{
//		DMA_CH_SPI_TX->CCR_b.EN = RESET;
////		spi_eot = RESET;
//	}
	#endif
	
	if(Timer_Expired(&t_updater) && GPIO_ReadBit(BLE_IRQ) == Bit_SET)
	{
		GPIO_ResetBits(BLE_IRQ);
		Timer_Restart(&t_updater);
	}
	
	if(!queue_isempty(&q_spi_rx) && DMA_CH_SPI_RX->CCR_b.EN == RESET && spi_eor == SET)
	{
		spi_eor = RESET;
		memset(output_buffer_ble, 0, MAX_STRING_LENGTH);
		queue_get_front(&q_spi_rx, output_buffer_ble, 0, MAX_STRING_LENGTH);
		queue_pop(&q_spi_rx);
		#if CLIENT
		for(int i = 0; i < MAX_STRING_LENGTH; i++)
		{
			printf("%c", output_buffer_ble[i]);
		}
		printf("\r\n");
		#endif
		APP_UpdateTX(output_buffer_ble, MAX_STRING_LENGTH);
		
		DMA_CH_SPI_RX->CCR_b.EN = SET;
	}
	if(!queue_isempty(&q_ble_rx) && DMA_CH_SPI_TX->CCR_b.EN == RESET && spi_eot == SET)
	{
		spi_eot = RESET;
		memset(output_buffer_spi, 0, MAX_STRING_LENGTH);
		queue_get_front(&q_ble_rx, output_buffer_spi, 0, MAX_STRING_LENGTH);
		queue_pop(&q_ble_rx);
		#if CLIENT
//		for(int i = 0; i < MAX_STRING_LENGTH; i++)
//		{
//			printf("%c", output_buffer_spi[i]);
//		}
		#endif
		GPIO_SetBits(BLE_IRQ);
		DMA_CH_SPI_TX->CCR_b.EN = SET;
	}
}
//

/**
  * @brief 	Функция переброса данных из SPI периферии в BLE (вторая версия)
						Недоделана, попытка сделать одновременный прием-передачу по SPI с отправкой на BLE.
						См. примеры в DK 3.1.0 - SPI DMA Master/Slave
						Возможно было бы проще полностью переделать.
  */
void Exchange_v2(void)
{
	if(GPIO_ReadBit(SPI_CS_Pin) == Bit_SET)
	{
		GPIO_ResetBits(BLE_IRQ);
	}
	
//		if(!queue_isempty(&q_ble_rx) && !queue_isempty(&q_spi_rx))
//		{
//			memset(output_buffer_spi, 0, MAX_STRING_LENGTH);
//			queue_get_front(&q_ble_rx, output_buffer_spi, 0, MAX_STRING_LENGTH);
//			queue_pop(&q_ble_rx);
//			
//			for(int i = 0; i < MAX_STRING_LENGTH; i++)
//			{
//				printf("%c", output_buffer_spi[i]);
//			}
//			if(!queue_isempty(&q_spi_rx))
//			{
//				memset(output_buffer_ble, 0, MAX_STRING_LENGTH);
//				queue_get_front(&q_spi_rx, output_buffer_ble, 0, MAX_STRING_LENGTH);
//				queue_pop(&q_spi_rx);
//				APP_UpdateTX(output_buffer_ble, MAX_STRING_LENGTH);
//			}
//			GPIO_SetBits(BLE_IRQ);
//			
//			DMA_CH_SPI_TX->CMAR = (uint32_t)output_buffer_spi[0];
//			DMA_CH_SPI_TX->CNDTR = (uint32_t)MAX_STRING_LENGTH;
//			
//			DMA_CH_SPI_TX->CCR_b.EN = SET;
//			DMA_CH_SPI_RX->CCR_b.EN = SET;
//		}
		
		if(spi_transmit_enabled == SET)
		{
			if(!queue_isempty(&q_ble_rx))
			{
				memset(output_buffer_spi, 0, MAX_STRING_LENGTH);
				queue_get_front(&q_ble_rx, output_buffer_spi, 0, MAX_STRING_LENGTH);
				queue_pop(&q_ble_rx);
				
				printf("Notify received:");
				for(int i = 0; i < MAX_STRING_LENGTH; i++)
				{
					printf("%c", output_buffer_spi[i]);
				}
				printf("\r\n");
				GPIO_SetBits(BLE_IRQ);
				
				DMA_CH_SPI_TX->CMAR = (uint32_t)output_buffer_spi[0];
				DMA_CH_SPI_TX->CNDTR = (uint32_t)MAX_STRING_LENGTH;
				
				DMA_CH_SPI_TX->CCR_b.EN = SET;
				DMA_CH_SPI_RX->CCR_b.EN = SET;
			}
		}
		if(spi_receive_enabled == SET)
		{
			if(!queue_isempty(&q_spi_rx))
			{
				memset(output_buffer_ble, 0, MAX_STRING_LENGTH);
				queue_get_front(&q_spi_rx, output_buffer_ble, 0, MAX_STRING_LENGTH);
				queue_pop(&q_spi_rx);
				APP_UpdateTX(output_buffer_ble, MAX_STRING_LENGTH);
				printf("Notify sent:");
				for(int i = 0; i < MAX_STRING_LENGTH; i++)
				{
					printf("%c", output_buffer_ble[i]);
				}
				printf("\r\n");
				memset(input_buffer_spi, 0, MAX_STRING_LENGTH);
				DMA_CH_SPI_RX->CCR_b.EN = SET;
				DMA_CH_SPI_TX->CCR_b.EN = SET;
			}
		}
	else
	{
		if(DMA_CH_SPI_TX->CCR_b.EN == RESET && DMA_CH_SPI_RX->CCR_b.EN == RESET)
		{
			//memset(output_buffer_spi, 0, MAX_STRING_LENGTH);
			memset(input_buffer_spi, 0, MAX_STRING_LENGTH);
			DMA_CH_SPI_TX->CCR_b.EN = SET;
			DMA_CH_SPI_RX->CCR_b.EN = SET;
		}
	}
}
//

_Bool processAttributes(Queue rx, Queue tx)
{
	return true;
}
//


/**
  * @brief  SPI Slave initialization.
  * @param  None
  * @retval None
  */
void SPI_Slave_Configuration(void)
{
  SPI_InitType SPI_InitStructure;
  GPIO_InitType GPIO_InitStructure;
  NVIC_InitType NVIC_InitStructure;
	DMA_InitType DMA_InitStructure;
	
  /* Enable SPI and GPIO clocks */
  SysCtrl_PeripheralClockCmd(CLOCK_PERIPH_GPIO | CLOCK_PERIPH_SPI, ENABLE);   
  
  /* Configure SPI pins */
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = SPI_OUT_Pin;		//OUT
  GPIO_InitStructure.GPIO_Mode = Serial0_Mode;
  GPIO_InitStructure.GPIO_Pull = ENABLE;
  GPIO_InitStructure.GPIO_HighPwr = DISABLE;
  GPIO_Init(&GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = SPI_IN_Pin;		//IN
  GPIO_InitStructure.GPIO_Mode = Serial0_Mode;
  GPIO_Init(&GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = SPI_CLK_Pin; 	//CLOCK
  GPIO_InitStructure.GPIO_Mode = Serial0_Mode;
  GPIO_Init(&GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = SPI_CS_Pin; 	//NSS
  GPIO_InitStructure.GPIO_Mode = Serial0_Mode;
  GPIO_Init(&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = BLE_IRQ;			//External output
	GPIO_InitStructure.GPIO_Mode = GPIO_Output;
  GPIO_InitStructure.GPIO_Pull = DISABLE;
  GPIO_InitStructure.GPIO_HighPwr = ENABLE;
  GPIO_Init(&GPIO_InitStructure);
  
  /* Configure SPI in master mode */
  SPI_StructInit(&SPI_InitStructure);
  SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b ;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_Init(&SPI_InitStructure);

  /* Clear RX and TX FIFO */
  SPI_ClearTXFIFO();
  SPI_ClearRXFIFO();
	
	SPI_EndianFormatReception(SPI_ENDIAN_MSByte_MSBit);
	SPI_EndianFormatTransmission(SPI_ENDIAN_MSByte_MSBit);
	
  /* Configure the level of FIFO for interrupt */
//  SPI_TxFifoInterruptLevelConfig(SPI_FIFO_LEV_1);
//  SPI_RxFifoInterruptLevelConfig(SPI_FIFO_LEV_1);
	
  /* Enable the DMA Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = SPI_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = LOW_PRIORITY;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure); 

	SysCtrl_PeripheralClockCmd(CLOCK_PERIPH_DMA, ENABLE);

  /* Configure DMA SPI TX channel */
  DMA_InitStructure.DMA_PeripheralBaseAddr = SPI_DR_BASE_ADDR;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)output_buffer_spi;  
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize = (uint32_t)MAX_STRING_LENGTH;  
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;    
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  
  DMA_Init(DMA_CH_SPI_TX, &DMA_InitStructure);
	DMA_Cmd(DMA_CH_SPI_TX, ENABLE);
    
  /* Configure DMA SPI RX channel */
	DMA_InitStructure.DMA_PeripheralBaseAddr = SPI_DR_BASE_ADDR;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)input_buffer_spi;  
  DMA_InitStructure.DMA_BufferSize = (uint32_t)MAX_STRING_LENGTH;  
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_Init(DMA_CH_SPI_RX, &DMA_InitStructure);
  DMA_Cmd(DMA_CH_SPI_RX, ENABLE);
  
  /* Enable SPI_TX/SPI_RX DMA requests */
  SPI_DMACmd(SPI_DMAReq_Tx | SPI_DMAReq_Rx, ENABLE);
  
  /* Enable DMA_CH_UART_TX Transfer Complete interrupt */
  DMA_FlagConfig(DMA_CH_SPI_TX, DMA_FLAG_TC, ENABLE);
	DMA_FlagConfig(DMA_CH_SPI_RX, DMA_FLAG_TC, ENABLE);
//	DMA_CH_SPI_TX->CCR_b.EN = RESET;
//	DMA_CH_SPI_RX->CCR_b.EN = RESET;
  
  /* Enable the USARTx Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = DMA_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = HIGH_PRIORITY;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);  
	
	
  /* Enable SPI functionality */
  SPI_Cmd(ENABLE);
}
//

#ifdef  USE_FULL_ASSERT

/**
* @brief  Reports the name of the source file and the source line number
*         where the assert_param error has occurred.
* @param  file: pointer to the source file name
* @param  line: assert_param error line source number
*/
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
  ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  
  /* Infinite loop */
  while (1)
  {
  }
}

#endif

/******************* (C) COPYRIGHT 2015 STMicroelectronics *****END OF FILE****/
/** \endcond
 */
