#ifndef __APP_FUNCTIONS_H__
#define __APP_FUNCTIONS_H__


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

// Flash an LED once.
void flashLED(uint8_t led) {
	LED_PORT.OUTSET = led;
	_delay_ms(DEFAULT_LED_FLASH_DELAY);
	LED_PORT.OUTCLR = led;
}

// Flash a specified LED a default number of times.
// Wrapper to next function.
void pulseLED(uint8_t led) {
	pulseLED(led, DEFAULT_LED_PULSE_CYCLES);
}

// Flash a specified LED a given number of times.
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

// Initialise the Periodic Interrupt Timer (PIT). This needs to be called
// once during the setup part of main().
void PIT_init(void) {
	// Wait for all RTC registers to be synchronized
	while (RTC.STATUS > 0);
	// Select internal 32.768kHz oscillator
	RTC.CLKSEL = RTC_CLKSEL_INT32K_gc;
	while (RTC.STATUS > 0);					// Wait for synchronization

	// Enable RTC with prescaler (required for PIT to work)
	RTC.CTRLA = RTC_PRESCALER_DIV1_gc | RTC_RTCEN_bm;
	while (RTC.STATUS > 0);					// Wait for synchronization

	// Wait for PIT registers to be ready
	while (RTC.PITSTATUS > 0);

	// Enable PIT interrupt BEFORE enabling PIT
	RTC.PITINTCTRL = RTC_PI_bm;

	// Configure PIT period AND enable it. The following values determine the
	// approximate intervals at which the interrupt fires (other, smaller,
	// values may be available):
	//     RTC_PERIOD_CYC4096_gc 	- 125 ms
	//     RTC_PERIOD_CYC8192_gc	- 250 ms
	//     RTC_PERIOD_CYC16384_gc	- 500 ms
	//     RTC_PERIOD_CYC32768_gc 	- 1 sec
	// The ATtiny1604 can't go beyond this but some microcontrollers, such as
	// the ATmega4809, can use:
	//     RTC_PERIOD_CYC65536_gc	- 2 secs

	RTC.PITCTRLA = RTC_PERIOD_CYC16384_gc | RTC_PITEN_bm; // 0.5 sec interval
}

// Enable the Periodic Interrupt Timer (PIT). Call this AFTER sei().
void PIT_enable() {
	while (RTC.PITSTATUS > 0); // Important and easy to overlook !
	RTC.PITCTRLA |= RTC_PITEN_bm;
}

// Disable the Periodic Interrupt Timer (PIT)
void PIT_disable() {
	RTC.PITINTCTRL &= ~RTC_PI_bm;				// Disable the interrupt first
	while (RTC.PITSTATUS & RTC_CTRLBUSY_bm);	// Wait for PIT synchronization
	RTC.PITCTRLA &= ~RTC_PITEN_bm;	    		// Turn off the PIT
}

void PIT_restart() {
	// Wait for synchronization
	while (RTC.STATUS > 0 || RTC.PITSTATUS > 0);

	// Reset the RTC Prescaler by toggling RTCEN
	// This forces the internal prescaler (and thus the PIT) back to 0
	uint8_t temp = RTC.CTRLA;
	RTC.CTRLA = temp & ~RTC_RTCEN_bm;
	while (RTC.STATUS > 0); // Wait for sync
	RTC.CTRLA = temp | RTC_RTCEN_bm;
	while (RTC.STATUS > 0); // Wait for sync

	// Re-enable PIT and its interrupt
	RTC.PITINTCTRL = RTC_PI_bm;

	while (RTC.PITSTATUS > 0);
	RTC.PITCTRLA |= RTC_PITEN_bm;
}

#endif
