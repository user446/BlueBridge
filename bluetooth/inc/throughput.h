
#ifndef _THROUGHPUT_H_
#define _THROUGHPUT_H_

#define NUM_PACKETS 500 
#define CONVERSION_NUM 4

#include <stdio.h>



uint8_t THROUGHPUT_DeviceInit(void);

void APP_Tick( void (*fptr_while_connected)(void));

_Bool APP_UpdateTX(uint8_t *sendbuf, uint8_t size);


#endif // _THROUGHPUT_H_
