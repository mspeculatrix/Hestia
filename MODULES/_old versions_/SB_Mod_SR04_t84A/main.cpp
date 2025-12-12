/*
 * LP_SB_SR04
 *
 * Ultrasonic rangefinder using the Sensor Bus
 *
 */

#define DEBUG true

#ifndef __AVR_ATtiny84A__
#define __AVR_ATtiny84A__
#endif

#ifndef F_CPU
#define F_CPU 1000000UL // define it now as 16 MHz unsigned long
#endif

#ifndef __OPTIMIZE__
#define __OPTIMIZE__
#endif

#define __DELAY_BACKWARD_COMPATIBLE__ // fix problem with _delay_ms()

#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "lib/defines.h"
#include "lib/functions.h"
#include "lib/smd_std_macros.h"
#include "lib/smd_std_typedefs.h"
#include "lib/smd_attiny_24_44_84A.h"

 /* GLOBALS */
volatile bool exchangeInit = false;
uint8_t outMsgBuf[MSG_BUF_LEN];
uint8_t inMsgBuf[MSG_BUF_LEN];

// -----  FUNCTIONS  -----------------------------------------------------------

void receiveMessage() {
	uint8_t msgLen = MSG_BUF_LEN;
	uint8_t bitIdx = 0;
	uint8_t byteIdx = 0;
	bool msgEnd = false;
	while (!msgEnd) {
		while (bit_is_set(PORTA, SB_CLK)) {} 	// Wait for SB_CLK LOW
		if (bit_is_set(PORTA, SB_DAT)) {		// Read bit
			inMsgBuf[byteIdx] |= (1 << bitIdx);
		} else {
			inMsgBuf[byteIdx] &= ~(1 << bitIdx);
		}
		bitIdx++;
		if (bitIdx == 8) {						// We've got a byte's worth
			if (byteIdx = 0) { 					// This is the first byte
				msgLen = inMsgBuf[byteIdx];
			}
			byteIdx++;
			if (byteIdx == msgLen) {
				msgEnd = true;
			} else {
				bitIdx = 0;
			}
		}
		while (bit_is_clear(PORTA, SB_CLK)) {} 	// Wait for SB_CLK HIGH
	}



	for (uint8_t idx = 0; idx < MSG_BUF_LEN; idx++) {
		for (uint8_t bit = 0; bit < 8; bit++) {

			while (bit_is_clear(PORTA, SB_CLK)) {}// Wait for SB_CLK HIGH
		}
	}
}

void sendMessage() {
	uint8_t msgIdx = 0;
	for (uint8_t idx = 0; idx < MSG_BUF_LEN; idx++) {
		for (uint8_t bit = 0; bit < 8; bit++) {
			uint8_t bitval = (outMsgBuf[idx] & (1 << bit)) >> bit;
			if (bitval == 1) {
				SETBIT(PORTA, SB_DAT);
			} else {
				CLEARBIT(PORTA, SB_DAT);
			}
			_delay_ms(BIT_PAUSE);
			CLEARBIT(PORTA, SB_CLK);
			_delay_ms(BIT_PAUSE);
			SETBIT(PORTA, SB_CLK);
		}
		_delay_ms(BYTE_PAUSE);
	}
}

void setReceiveMode() {
	DAT_INT_DISABLE;						// Disable SB_DAT interrupts
	SET_DAT_OUTPUT;
	CLEARBIT(PORTA, SB_DAT);
	_delay_ms(DAT_PULSE_LENGTH);
	SETBIT(PORTA, SB_DAT);
	SET_DAT_INPUT;
	// DDRA &= ~(1 << SB_CLK);				// Set /SB_CLK to INPUT
}

uint8_t setSendMode() {
	uint8_t error = 0;
	uint8_t initAttempts = 0;
	while (error == 0) {
		initAttempts++;
		if (bit_is_set(PINA, SB_CLK)) { 			// clock is high so let's try
			DAT_INT_DISABLE;						// Disable DAT interrupts
			SET_DAT_OUTPUT;
			SETBIT(PORTA, SB_DAT);
			CLEARBIT(PORTA, SB_DAT);				// Strobe DAT line
			_delay_ms(DAT_PULSE_LENGTH);
			SETBIT(PORTA, SB_DAT);
			SET_DAT_INPUT;

			// Wait for `SB_DAT` to pulse LOW

			bool loop = true;
			uint8_t attempts = 0;
			while (loop) {
				if (bit_is_set(PINA, SB_DAT)) {
					_delay_ms(TIMEOUT_LOOP_DELAY);
					attempts++;
					if (attempts == MAX_CONNECTION_ATTEMPTS) {
						loop = false;
						error = ERR_MAX_ATTEMPTS;
					}
				} else {			// DAT has gone low
					loop = false;
					// setPin(&PORTA, SB_DAT, HIGH);
					SET_DAT_OUTPUT;
					SET_CLK_OUTPUT;
					SETBIT(PORTA, SB_DAT);
					SETBIT(PORTA, SB_CLK);
					_delay_ms(STABILISE_DELAY); 			// let hub get ready
				}
			}
		} else if (initAttempts < MAX_INIT_ATTEMPTS) {
			_delay_ms(INIT_RETRY_DELAY);
		} else {
			error = ERR_HUB_NOT_RESPONDING;
		}
	}
	return error;
}

void setSBDefaultState() {
	DDRA &= ~(1 << SB_CLK | 1 << SB_DAT);	// Set SB pins as inputs
	DAT_INT_ENABLE;							// Enable interrupts on SB_DAT
}

// -----  Interrupt Service Routine  -------------------------------------------
ISR(PCINT0_vect) {
	// This responds to any PC interrupts 0-7. But as we're using only PCINT0
	// then we can be sure that's the pin on which the interrupt occurred.
	DAT_INT_DISABLE;						// Disable interrupts on SB_DAT
	exchangeInit = true;					// Controller has pulsed SB_DAT low
	if (DEBUG) {
		setPin(&PORTB, ACT_LED, HIGH);			// Flash LED
		_delay_ms(50);
		setPin(&PORTB, ACT_LED, LOW);
	}
}

// =============================================================================
// =====   MAIN                                                            =====
// =============================================================================
int main(void) {

	/***************************************************************************
	*****   SETUP                                                          *****
	***************************************************************************/

	setSBDefaultState();		// Sets SB lines (Port A) to defaults (inputs)
	DDRA |= (1 << ALERT_LED);	// ALERT_LED, SB_INT to outputs
	DDRB |= (1 << ACT_LED | 1 << TRIGGER); 	// LED and TRIGGER to outputs
	setPin(&PORTB, ACT_LED, LOW);
	setPin(&PORTA, ALERT_LED, LOW);
	setPin(&PORTB, TRIGGER, LOW);

	// Set up interrupts
	GIMSK |= (1 << PCIE0);						// Enable PC interrupts 0-7
	DAT_INT_ENABLE;								// Enable interrupts on SB_DAT

	flashActive(5);
	flashAlert(5);

	sei();									// Turn interrupts on

	/***************************************************************************
	*****   MAIN LOOP                                                      *****
	***************************************************************************/


	while (1) {
		// if (exchangeInit) {
		// exchangeInit = false;
		// DAT_INT_ENABLE;
		// }
		for (uint8_t byte0 = 64; byte0 < 128; byte0++) {
			outMsgBuf[0] = MSG_BUF_LEN;
			for (uint8_t byte1 = 64; byte1 < 128; byte1++) {
				outMsgBuf[1] = byte1;
				for (uint8_t byte2 = 64; byte2 < 128; byte2++) {
					outMsgBuf[2] = byte2;
					setSendMode();
					sendMessage();
					setSBDefaultState();
					_delay_ms(1000);
					flashAlert(1);
				}
			}
		}
	}

}