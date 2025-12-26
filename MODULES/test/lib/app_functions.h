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

/* PIT timer functions

The Periodic Interrupt Timer (PIT) is part of the RTC block.

These functions assume the default 32.768 kHz ULP oscillator for the RTC.

`RTC.PITCTRLB` selects both the period and enables the PIT. For a 32.768 kHz clock:
  - `CYC32768` → about 1 second.
`RTC.PITINTCTRL` controls the PIT interrupt; `RTC.PI_bm` enables it.

*/
void PIT_init() {
	// Ensure RTC clock is running on 32.768 kHz ULP (usually default).
	// If not, you must enable it in CLKCTRL / MCLK as per datasheet.

	// Set RTC clock source to 32.768kHz ULP (usually default, but explicit)
	CCP = CCP_IOREG_gc;  // Unlock protected register
	CLKCTRL.OSC32KCTRLA |= (1 << 0);  // Bit 0 = ENABLE (0x01)
	while (!(CLKCTRL.MCLKSTATUS & (1 << 5)));  // Wait for XOSC32KS bit 5

	// Then set RTC clock source
	while (RTC.STATUS > 0);
	RTC.CLKSEL = RTC_CLKSEL_INT32K_gc;



	// Disable PIT first
	RTC.PITCTRLA = 0;

	// PITCTRLA: PERIOD = CYC32768 (bits 6:3 = 0b1110), PITEN=0 (bit 0)
	RTC.PITCTRLA = RTC_PERIOD_CYC32768_gc;  // ~1.024s period

	// Enable PIT interrupt
	RTC.PITINTCTRL = RTC_PI_bm;
}

void PIT_enable() {
	// Keep setting, just (re)enable PIT and interrupt
	RTC.PITCTRLA |= RTC_PITEN_bm;
	// RTC.PITINTCTRL |= RTC_PI_bm;
}

void PIT_disable() {
	// Disable interrupt and PIT
	// RTC.PITINTCTRL &= ~RTC_PI_bm;
	RTC.PITCTRLA &= ~RTC_PITEN_bm;
}


#endif
