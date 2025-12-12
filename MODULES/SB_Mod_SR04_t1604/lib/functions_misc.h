#ifndef __FUNCTIONS_MISC_H__
#define __FUNCTIONS_MISC_H__


#include <avr/io.h>
#include <util/delay.h>
#include "app_defines.h"
#include "smd_std_macros.h"

// PROTOTYPES
void clearBuf(char* buf, uint8_t len);
void flashLED(uint8_t led);
void pulseLED(uint8_t led);
void pulseLED(uint8_t led, uint8_t numPulses);


// Clear any buffer you like by writing zeros to it.
void clearBuf(char* buf, uint8_t len) {
	for (uint8_t i = 0; i < len; i++) {
		buf[i] = 0;
	}
}

void flashLED(uint8_t led) {
	LED_PORT.OUTSET = led;
	_delay_ms(DEFAULT_LED_FLASH_DELAY);
	LED_PORT.OUTCLR = led;
}

void pulseLED(uint8_t led) {
	pulseLED(led, DEFAULT_LED_PULSE_CYCLES);
}

void pulseLED(uint8_t led, uint8_t numPulses) {
	for (uint8_t i = 0; i < numPulses; i++) {
		LED_PORT.OUTSET = led;
		_delay_ms(DEFAULT_LED_PULSE_DELAY);
		LED_PORT.OUTCLR = led;
		_delay_ms(DEFAULT_LED_PULSE_DELAY);
	}
}

#endif
