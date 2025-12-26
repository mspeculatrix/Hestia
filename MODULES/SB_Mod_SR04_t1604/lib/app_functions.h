#ifndef __APP_FUNCTIONS_H__
#define __APP+FUNCTIONS_H__


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

/* PIT timer functions

The Periodic Interrupt Timer (PIT) is part of the RTC block.

These functions assume the default 32.768 kHz ULP oscillator for the RTC.

*/
void PIT_init() {
	// Wait for all RTC registers to be synchronized
	while (RTC.STATUS > 0);

	// Select internal 32.768 kHz oscillator
	RTC.CLKSEL = RTC_CLKSEL_INT32K_gc;

	// Wait for synchronization
	while (RTC.STATUS > 0);

	// Enable RTC with prescaler (required for PIT to work)
	RTC.CTRLA = RTC_PRESCALER_DIV1_gc | RTC_RTCEN_bm;

	// Wait for synchronization
	while (RTC.STATUS > 0);

	// Wait for PIT registers to be ready
	while (RTC.PITSTATUS > 0);

	// Enable PIT interrupt BEFORE enabling PIT
	RTC.PITINTCTRL = RTC_PI_bm;

	// Configure PIT period AND enable it
	RTC.PITCTRLA = RTC_PERIOD_CYC32768_gc | RTC_PITEN_bm;
}

void PIT_enable() {
	while (RTC.PITSTATUS > 0); // Important and easy to overlook !
	RTC.PITCTRLA |= RTC_PITEN_bm;
}

void PIT_disable() {
	while (RTC.PITSTATUS > 0);
	RTC.PITCTRLA &= ~RTC_PITEN_bm;
}


#endif
