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
void PIT_init(void);
void PIT_enable(void);
void PIT_disable(void);
void PIT_restart(void);

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

/*            *****  PIT TIMER FUNCTIONS  *****

The Periodic Interrupt Timer (PIT) is part of the RTC block.
These functions assume the default 32.768 kHz ULP oscillator for the RTC.

*/

// Initialise the Periodic Interrupt Timer (PIT). This needs to be called
// once during the setup part of main(), before calling sei().
void PIT_init(void) {
	while (RTC.STATUS > 0);	// Ensure all RTC registers are synchronised
	RTC.CLKSEL = RTC_CLKSEL_INT32K_gc;	// Select internal 32.768kHz oscillator
	while (RTC.STATUS > 0);				// Wait for synchronisation
	// Enable RTC with prescaler (required for PIT to work)
	RTC.CTRLA = RTC_PRESCALER_DIV1_gc | RTC_RTCEN_bm;
	while (RTC.STATUS > 0);				// Wait for synchronisation
	while (RTC.PITSTATUS > 0);			// Wait for PIT registers to be ready
	// Enable PIT interrupt BEFORE enabling PIT
	RTC.PITINTCTRL = RTC_PI_bm;

	// Configure PIT period AND enable it. The following values are options to
	// determine the approximate intervals at which the interrupt fires
	// (other, smaller, values may be available):
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
void PIT_enable(void) {
	while (RTC.PITSTATUS > 0); 					// Wait for synchronisation
	RTC.PITCTRLA |= RTC_PITEN_bm;
}

// Disable the Periodic Interrupt Timer (PIT)
void PIT_disable(void) {
	RTC.PITINTCTRL &= ~RTC_PI_bm;				// Disable the interrupt first
	while (RTC.PITSTATUS & RTC_CTRLBUSY_bm);	// Wait for PIT synchronisation
	RTC.PITCTRLA &= ~RTC_PITEN_bm;	    		// Turn off the PIT
}

// Restart PIT after it has been disabled
void PIT_restart(void) {
	while (RTC.STATUS > 0 || RTC.PITSTATUS > 0);	// Wait for synchronisation
	// Reset the RTC Prescaler by toggling RTCEN
	// This forces the internal prescaler (and thus the PIT) back to 0
	uint8_t temp = RTC.CTRLA;
	RTC.CTRLA = temp & ~RTC_RTCEN_bm;
	while (RTC.STATUS > 0); // Wait for sync
	RTC.CTRLA = temp | RTC_RTCEN_bm;
	while (RTC.STATUS > 0); // Wait for sync

	RTC.PITINTCTRL = RTC_PI_bm;				// Re-enable PIT and its interrupt
	while (RTC.PITSTATUS > 0);				// Wait until ready
	RTC.PITCTRLA |= RTC_PITEN_bm;
}

/*
There is an easier and faster way of turning the PIT off and on again.

Create a global var:

volatile bool pitActive = false;

Functions:

void PIT_soft_disable() {
	pitActive = false;
}

void PIT_soft_restart() {
	// This doesn't reset the timer to 0, but it starts processing the
	// interrupts immediately.
	pitActive = true;
}

In the ISR, check if pitActive is true or false. Eg, the ISR we're using would
become:

ISR(RTC_PIT_vect) {
	RTC.PITINTFLAGS = RTC_PI_bm; 		// Clears the interrupt register flag
	if(pitActive) {
		pitTickCounter++;
		if (pitTickCounter == PIT_TICK_COUNT_THRESHOLD) {
			performPing = true;				// Set event flag
			pitTickCounter = 0;
		}
	} else {
		pitTickCounter = 0;
	}
}

This approach is faster because it avoids synchronisation delays. But it
doesn't reset the timer to 0. However, resetting pitTickCounter to 0 achieves
something of the same effect.

*/


#endif
