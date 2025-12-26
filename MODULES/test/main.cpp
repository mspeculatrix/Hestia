/*
 * SB_Mod_SR04_t1604
 *
 * ATmega1604 code for an SR04 ultrasonic rangefinder using a SensorBus
 * interface.
 */

#ifndef __AVR_ATtiny1604__
#define __AVR_ATtiny1604__
#endif

#ifndef F_CPU
#define F_CPU 20000000UL // 20 MHz unsigned long
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "lib/smd_avrmod_serial.h"
#include "lib/app_defines.h"

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

	// **CRITICAL: Wait for PIT registers to be ready**
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

/******************************************************************************
*****     GLOBALS                                                         *****
******************************************************************************/

// Using serial only for dev & debugging.
// SMD_AVRMod_Serial serial = SMD_AVRMod_Serial(SERIAL_BAUDRATE,
// 	&PORTB, TX_PIN, RX_PIN);

volatile bool performPing = false;

// ISR called ~every 1 second to initiate ping
ISR(RTC_PIT_vect) {
	// Clear interrupt flag
	RTC.PITINTFLAGS = RTC_PI_bm;	// writing this bit clears the flag
	PORTB.OUTTGL = PIN0_bm;
	performPing = true;				// set flag
}

/*******************************************************************************
*****   MAIN                                                               *****
*******************************************************************************/

int main(void) {

	//--------------------------------------------------------------------------
	//-----   SETUP                                                        -----
	//--------------------------------------------------------------------------

	CCP = CCP_IOREG_gc;     // Unlock protected registers
	CLKCTRL.MCLKCTRLB = 0;  // No prescaling, full main clock frequency

	// serial.begin();

	PORTB.DIRSET = PIN0_bm | PIN1_bm;
	PORTB.OUTTGL = PIN0_bm;
	_delay_ms(250);
	PORTB.OUTTGL = PIN0_bm;
	_delay_ms(250);
	PORTB.OUTTGL = PIN0_bm;

	PIT_init();					// Config timer for regular pings
	sei();
	PIT_enable();

	// serial.writeln("test running");

	/***************************************************************************
	****** MAIN LOOP                                                       *****
	***************************************************************************/

	while (1) {

		if (performPing) {
			// uint16_t dist = ping();
			// serial.writeln((int)dist);
			PORTB.OUTTGL = PIN1_bm;  // Toggle ALERT LED
			// serial.writeln("hi");
			performPing = false;
		}
	}
}
