#include "main.h"
#include "comm.h"
#include "queue.h"


extern Queue q_from_tx;

/**
	*	@brief	Функция приема сообщения с CDC
	*	@note		см. функцию @ref CDC_Receive_FS
	*	@param	
	*	@retval	
**/
//
void Connection_RecieveCallback(uint8_t* buffer, uint32_t Len)
{
	//HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_13);
	queue_push(&q_from_tx, buffer, Len);
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
	
}
//
