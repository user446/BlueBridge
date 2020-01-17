#include "messaging.h"

_Bool make_a_message(const uint8_t command[COMMAND_PROPERTY_LEN], uint8_t* message, uint8_t len, uint8_t result[RESULT_MAX_LEN])
{
	_Bool ret = FALSE;
	if(COMMAND_PROPERTY_LEN + len > RESULT_MAX_LEN)
		return ret;
	else ret = TRUE;
	
	for(int i = 0; i < COMMAND_PROPERTY_LEN-1; i++)
		result[i] = command[i];
	
	for(int i = 0; i < len; i++)
		result[i+COMMAND_PROPERTY_LEN-1] = message[i];
	
	return ret;
}
//
