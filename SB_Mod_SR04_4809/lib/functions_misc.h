#ifndef __FUNCTIONS_MISC_H__
#define __FUNCTIONS_MISC_H__


#include <avr/io.h>
#include <util/delay.h>
#include "app_defines.h"
#include "smd_std_macros.h"


// Clear any buffer you like by writing zeros to it.
void clearBuf(char* buf, uint8_t len) {
	for (uint8_t i = 0; i < len; i++) {
		buf[i] = 0;
	}
}


#endif
