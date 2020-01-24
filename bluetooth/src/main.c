
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
#if CLIENT
#include "Throughput_client_config.h"
#else
#include "Throughput_server_config.h"
#endif

#include "queue.h"
#include "messaging.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define BLE_THROUGHPUT_VERSION_STRING "1.0.0" 
#define BUFF_SIZE                   	(10)
#define MAX_QLENGTH 128
/* Private macro -------------------------------------------------------------*/
extern uint16_t connection_handle;
/* Private variables ---------------------------------------------------------*/
struct timer t_updater;

uint8_t test_buff[7] = {'h', 'e', 'l', 'l', 'o', '\r', '\n'};
uint8_t input_buffer_spi[MAX_QLENGTH];
uint8_t output_buffer_spi[MAX_QLENGTH];

FlagStatus spi_eor = SET;
FlagStatus spi_eot = SET;

volatile uint16_t bytes_to_receive = MAX_QLENGTH;
volatile uint16_t bytes_to_send = MAX_QLENGTH;

volatile uint8_t inbuffer_idx = 0;
volatile uint8_t outbuffer_idx = 0;

/* Private function prototypes -----------------------------------------------*/
void InitiateSPISend(uint8_t* buffer, uint8_t len);
void InitiateSPIRec(void);

_Bool processSPI();
_Bool processAttributes(Queue rx, Queue tx);
void send_test(void);
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
	
	Timer_Set(&t_updater, CLOCK_SECOND/1000);
	
	printf("Rx and Tx queues initialized\r\n");
	
	SPI_ITConfig(SPI_IT_RX, ENABLE);
	
  while(1) {
    /* BlueNRG-1 stack tick */
    BTLE_StackTick();
		
		/* Application tick */
    APP_Tick(send_test);
		
		InitiateSPISend(test_buff, 7);
		InitiateSPIRec();
  }
  
} /* end main() */
//

void InitiateSPISend(uint8_t* buffer, uint8_t len)
{
	SPI_ITConfig(SPI_IT_TX, ENABLE);
	uint8_t i = 0;
	while(i < len)
		{
			output_buffer_spi[i] = buffer[i];
			i++;
		}
}
//

void InitiateSPIRec(void)
{
	spi_eor = SET;
}
//

void send_test(void)
{
//	if(Timer_Expired(&t_updater)){
//		APP_UpdateTX(test_buff, sizeof(test_buff[0])*7);
//		Timer_Restart(&t_updater);
//	}
}
//

_Bool processAttributes(Queue rx, Queue tx)
{
	
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
	
  /* Enable SPI and GPIO clocks */
  SysCtrl_PeripheralClockCmd(CLOCK_PERIPH_GPIO | CLOCK_PERIPH_SPI, ENABLE);   
  
  /* Configure SPI pins */
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;		//OUT
  GPIO_InitStructure.GPIO_Mode = Serial0_Mode;
  GPIO_InitStructure.GPIO_Pull = ENABLE;
  GPIO_InitStructure.GPIO_HighPwr = DISABLE;
  GPIO_Init(&GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; 	//IN
  GPIO_InitStructure.GPIO_Mode = Serial0_Mode;
  GPIO_Init(&GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; 	//CLOCK
  GPIO_InitStructure.GPIO_Mode = Serial0_Mode;
  GPIO_Init(&GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; 	//NSS
  GPIO_InitStructure.GPIO_Mode = Serial0_Mode;
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
  SPI_TxFifoInterruptLevelConfig(SPI_FIFO_LEV_8);
  SPI_RxFifoInterruptLevelConfig(SPI_FIFO_LEV_8);
	
  /* Enable SPI functionality */
  SPI_Cmd(ENABLE);
	
  /* Enable the DMA Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = SPI_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = LOW_PRIORITY;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure); 
	NVIC_EnableIRQ(SPI_IRQn);
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
