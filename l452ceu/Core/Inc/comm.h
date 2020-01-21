#ifndef __COMM_H
#define __COMM_H

#include <stdbool.h>

void Connection_RecieveCallback(uint8_t* buffer, uint32_t Len);
void Connection_TransmitCallback(void);

#endif
