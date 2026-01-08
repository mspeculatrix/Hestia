#include <avr/io.h>


#ifndef __APP_FUNCTIONS_H__
#define __APP_FUNCTIONS_H__

/**
 * @brief Get a message from the designated module and put it in the buffer.
 * @param uint8_t module - ID of module (0-7)
 * @param uint8_t* msgBuf - somewhere to put the message.
 * @return bool Whether msg was successfully received.
 */
bool getMessage(uint8_t module, uint8_t* msgBuf) {
	bool received = false;

	return received;
}


#endif
