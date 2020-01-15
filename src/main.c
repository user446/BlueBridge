
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
#include "Throughput_config.h"
#include "queue.h"
#include "messaging.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define BLE_THROUGHPUT_VERSION_STRING "1.0.0" 
#define BUFF_SIZE                   	(10)
/* Private macro -------------------------------------------------------------*/
extern uint16_t connection_handle;
/* Private variables ---------------------------------------------------------*/
uint8_t spi_buffer_tx[BUFF_SIZE];
uint8_t spi_buffer_rx[BUFF_SIZE];
uint8_t test_buff[] = {'h', 'e', 'l', 'l', 'o'};

extern uint8_t buffer_spi_tail;
extern volatile uint8_t buffer_spi_used;

Queue q_msg_from_rxATT;
Queue q_msg_to_txATT;

/* Private function prototypes -----------------------------------------------*/
_Bool processSPI(Queue rx, Queue tx);
_Bool processAttributes(Queue rx, Queue tx);
void send_test(void);

void SPI_Slave_Configuration(void);
void DMASpi_Sending(uint32_t buffer, uint32_t size);
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
	//SPI_Slave_Configuration();

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
  
  printf("BLE Stack Initialized \n");
	
	queue_init(&q_msg_from_rxATT);
	queue_init(&q_msg_to_txATT);
	
	printf("Rx and Tx queues initialized \n");
	
  while(1) {
    /* BlueNRG-1 stack tick */
    BTLE_StackTick();
		
		/* Application tick */
    APP_Tick(send_test);
  }
  
} /* end main() */
//

void send_test(void)
{
	APP_UpdateTX(test_buff, sizeof(test_buff[0])*5);
}
//

_Bool processAttributes(Queue rx, Queue tx)
{
	
}
//

/**
* @brief  Process command code
* @param  None
* @retval None
*/
_Bool processSPI(Queue rx, Queue tx)
{
		unsigned short tx_string[MAX_LENGTH];
		unsigned short tx_str_len = queue_get_frontl(&q_msg_from_rxATT);
		queue_get_front(&q_msg_from_rxATT, tx_string, 0, tx_str_len);
		
    /* Prepare buffer to sent through SPI DMA TX channel */
    DMASpi_Sending((uint32_t)&tx_string[0], tx_str_len*sizeof(tx_string[0]));
		
		queue_pop(&q_msg_from_rxATT);
    
    while(buffer_spi_used==0);    
	
		//rx->digits =  spi_buffer_rx[buffer_spi_tail];
		queue_push(&q_msg_to_txATT, (unsigned short*)spi_buffer_rx, buffer_spi_tail);
		//memcpy(rx[nucleo_receive_cnt++].digits, spi_buffer_rx, buffer_spi_tail);
    
    buffer_spi_tail = (buffer_spi_tail+1)%SPI_BUFF_SIZE;    
    buffer_spi_used--;
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
  
  /* Enable SPI and GPIO clocks */
  SysCtrl_PeripheralClockCmd(CLOCK_PERIPH_GPIO | CLOCK_PERIPH_SPI, ENABLE);   
  
  /* Configure SPI pins */
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = Serial0_Mode;
  GPIO_InitStructure.GPIO_Pull = ENABLE;
  GPIO_InitStructure.GPIO_HighPwr = DISABLE;
  GPIO_Init(&GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = Serial0_Mode;
  GPIO_Init(&GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = Serial0_Mode;
  GPIO_Init(&GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = Serial0_Mode;
  GPIO_Init(&GPIO_InitStructure);
  
  /* Configure SPI in master mode */
  SPI_StructInit(&SPI_InitStructure);
  SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b ;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_BaudRate = SPI_BAUDRATE;
  SPI_Init(&SPI_InitStructure);

  /* Clear RX and TX FIFO */
  SPI_ClearTXFIFO();
  SPI_ClearRXFIFO();
  
  
  /* Configure DMA peripheral */
  SysCtrl_PeripheralClockCmd(CLOCK_PERIPH_DMA, ENABLE);
  
  DMA_InitType DMA_InitStructure;
  /* Configure DMA SPI TX channel */
  DMA_InitStructure.DMA_PeripheralBaseAddr = SPI_DR_BASE_ADDR;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)spi_buffer_tx;  
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize = (uint32_t)SPI_BUFF_SIZE;  
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;    
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  
  DMA_Init(DMA_CH_SPI_TX, &DMA_InitStructure);
    
  /* Configure DMA SPI RX channel */
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)spi_buffer_rx;  
  DMA_InitStructure.DMA_BufferSize = (uint32_t)SPI_BUFF_SIZE;  
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_Init(DMA_CH_SPI_RX, &DMA_InitStructure);
  DMA_Cmd(DMA_CH_SPI_RX, ENABLE);
  
  /* Enable SPI_TX/SPI_RX DMA requests */
  SPI_DMACmd(SPI_DMAReq_Tx | SPI_DMAReq_Rx, ENABLE);
  
    
  /* Enable SPI functionality */
  SPI_Cmd(ENABLE);
  
}

void DMASpi_Sending(uint32_t buffer, uint32_t size) 
{
  /* DMA_CH_SPI_TX reset */
  DMA_CH_SPI_TX->CCR_b.EN = RESET;
  
  /* Rearm the DMA_CH_SPI_TX */
  DMA_CH_SPI_TX->CMAR = buffer;
  DMA_CH_SPI_TX->CNDTR = size;
  
  /* DMA_CH_SPI_TX enable */
  DMA_CH_SPI_TX->CCR_b.EN = SET;
  
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
