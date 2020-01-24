/**
  ******************************************************************************
  * @file    BlueNRG1_it.c 
  * @author  VMA RF Application Team
  * @version V1.0.0
  * @date    September-2015
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "BlueNRG1_it.h"
#include "BlueNRG1_conf.h"
#include "ble_const.h" 
#include "bluenrg1_stack.h"
#include "SDK_EVAL_Com.h"
#include "clock.h"

/** @addtogroup BlueNRG1_StdPeriph_Examples
  * @{
  */

/** @addtogroup GPIO_Examples
  * @{
  */ 

/** @addtogroup GPIO_IOToggle
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/**
 * @brief  SPI IRQ mode defines
 */
#define SEND_START       0
#define SEND_READ_REG    1
#define SEND_WRITE_REG   2
#define SEND_WRITE_DATA  3

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t buffer_spi_tail = 0;
volatile uint8_t buffer_spi_used = 0;

/**
 * @brief  SPI flag set at end of transaction
 */
extern FlagStatus spi_eor;
extern FlagStatus spi_eot;

extern volatile  uint16_t bytes_to_receive;
extern volatile  uint16_t bytes_to_send;

extern uint8_t input_buffer_spi[128];
extern uint8_t output_buffer_spi[128];

extern volatile  uint8_t inbuffer_idx;
extern volatile  uint8_t outbuffer_idx;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M0 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles SVCall exception.
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles PendSV_Handler exception.
  */
//void PendSV_Handler(void)
//{
//}

/**
  * @brief  This function handles SysTick Handler.
  */
void SysTick_Handler(void)
{
  SysCount_Handler();
}

/**
  * @brief  This function handles GPIO interrupt request.
  * @param  None
  * @retval None
  */
void GPIO_Handler(void)
{
	__nop();
}
//

void SPI_Handler(void)
{
	/* Check if SPI RX interrupt event occured */
  if(SPI_GetITStatus(SPI_IT_RX) == SET) 
	{
		SPI_ClearITPendingBit(SPI_IT_RX);
		input_buffer_spi[inbuffer_idx++] = SPI_ReceiveData();

				if(bytes_to_receive == inbuffer_idx)
				{
					inbuffer_idx = 0;
					spi_eor = RESET;
				}
  }
	if(SPI_GetITStatus(SPI_IT_TX) == SET)
	{
		SPI_ClearITPendingBit(SPI_IT_TX);
		SPI_SendData(output_buffer_spi[outbuffer_idx++]);
		if(bytes_to_send == outbuffer_idx)
			{
				SPI_ITConfig(SPI_IT_TX , DISABLE);
				outbuffer_idx = 0;
				spi_eot = RESET;
			}
	}
}
//

/******************************************************************************/
/*                 BlueNRG-1 Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (system_bluenrg1.c).                                               */
/******************************************************************************/
/**
* @brief  This function handles UART interrupt request.
* @param  None
* @retval None
*/
void UART_Handler(void)
{  
  SdkEvalComIOUartIrqHandler();
}

void Blue_Handler(void)
{
   //Call RAL_Isr
   RAL_Isr();
}

/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2015 STMicroelectronics *****END OF FILE****/
