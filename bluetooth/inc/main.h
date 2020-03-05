#ifndef __MAIN__
#define __MAIN__

#include "BlueNRG1_conf.h"

#define SPITX_TRACE 1 //выводим в UART все что передается в SPI
#define BLERX_TRACE 1 //выводим в UART все что принято по радио
#define SPIRX_TRACE 1 
#define BLETX_TRACE 1 //- аналогично


#if CLIENT
#define SPI_IN_Pin	GPIO_Pin_3
#define SPI_OUT_Pin	GPIO_Pin_2
#define SPI_CS_Pin	GPIO_Pin_11
#define SPI_CLK_Pin	GPIO_Pin_8

#define BLE_IRQ  GPIO_Pin_6

#define MANAGEMENT_FLAGS 1
#else
#define SPI_IN_Pin	GPIO_Pin_3
#define SPI_OUT_Pin	GPIO_Pin_2
#define SPI_CS_Pin	GPIO_Pin_1
#define SPI_CLK_Pin	GPIO_Pin_0

#define BLE_IRQ GPIO_Pin_6

#define MANAGEMENT_FLAGS 0
#endif

#endif
