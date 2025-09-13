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
volatile bool hubRequest = false;
uint8_t outMsgBuf[MSG_BUF_LEN];
uint8_t inMsgBuf[MSG_BUF_LEN];

// -----  FUNCTIONS  -----------------------------------------------------------

void receiveMessage() {
	// **** THIS ISN'T FINISHED !!!! ****
	// Watch for SB_CLK to go LOW
	// Watch for SB_DAT to go LOW
	// Check SB_CLK is still LOW.
	// Start of message transaction
	for (uint8_t idx = 0; idx < MSG_BUF_LEN; idx++) {
		for (uint8_t bit = 0; bit < 8; bit++) {
			while (bit_is_clear(PORTA, SB_ACT)) {		// If HIGH, done.
				while (bit_is_set(PORTA, SB_CLK)) {} 	// Wait for SB_CLK LOW
				if (bit_is_set(PORTA, SB_DAT)) {			// Read bit
					inMsgBuf[idx] |= (1 << bit);
				} else {
					inMsgBuf[idx] &= ~(1 << bit);
				}
				while (bit_is_clear(PORTA, SB_CLK)) {}// Wait for SB_CLK HIGH
			}
		}
	}
}

void sendMessage() {
	// SB_DAT should already be HIGH at this point
	CLEARBIT(PORTA, SB_CLK);	// take low while DAT is high
	CLEARBIT(PORTA, SB_DAT);	// take LOW while CLK is low
	_delay_ms(START_TRANSMISSION_PAUSE);
	SETBIT(PORTA, SB_CLK);
	SETBIT(PORTA, SB_DAT);
	// Now in transmission mode
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
	DAT_INT_DISABLE;					// Disable SB_DAT interrupts
	// DDRA &= ~(1 << SB_CLK);				// Set /SB_CLK to INPUT
	setPin(&PORTA, SB_INT, LOW);		// Pulse /SB_INT
	_delay_ms(INT_PULSE_LENGTH);
	setPin(&PORTA, SB_INT, HIGH);
}

uint8_t setSendMode() {
	uint8_t error = 0;
	if (bit_is_set(PINA, SB_ACT)) { 		// SB_ACT is HIGH
		DDRA |= (1 << SB_CLK | 1 << SB_DAT); // Set to OUTPUTs
		setPin(&PORTA, SB_CLK, HIGH);
		setPin(&PORTA, SB_DAT, LOW);
		setPin(&PORTA, SB_INT, LOW);		// Pulse /SB_INT
		_delay_ms(INT_PULSE_LENGTH);
		setPin(&PORTA, SB_INT, HIGH);		// Pulse /SB_INT
		bool loop = true;
		uint8_t attempts = 0;
		while (loop) {						// Wait for /SB_ACT to go LOW
			if (bit_is_set(PINA, SB_ACT)) {
				_delay_ms(TIMEOUT_LOOP_DELAY);
				attempts++;
				if (attempts == MAX_CONNECTION_ATTEMPTS) {
					loop = false;
					error = ERR_MAX_ATTEMPTS;
				}
			} else {
				loop = false;
				// setPin(&PORTA, SB_DAT, HIGH);
				SETBIT(PORTA, SB_DAT);
				_delay_ms(STABILISE_DELAY); 			// let hub get ready
			}
		}
	} else {
		error = ERR_HUB_NOT_RESPONDING;
	}
	return error;
}

void setSBDefaultState() {
	// Set SB pins as inputs
	DDRA &= ~(1 << SB_ACT | 1 << SB_CLK | 1 << SB_DAT);
	DAT_INT_ENABLE;							// Enable interrupts on SB_DAT
}

// -----  Interrupt Service Routine  -------------------------------------------
ISR(PCINT0_vect) {
	// This responds to any PC interrupts 0-7. But as we're using only PCINT0
	// then we can be sure that's the pin on which the interrupt occurred.
	DAT_INT_DISABLE;						// Disable interrupts on SB_DAT
	hubRequest = true;						// Hub has pulsed SB_DAT low
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
	DDRA |= (1 << ALERT_LED | 1 << SB_INT);	// ALERT_LED, SB_INT to outputs
	DDRB |= (1 << ACT_LED | 1 << TRIGGER); 	// LED and TRIGGER to outputs
	setPin(&PORTA, SB_INT, HIGH);
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
		// if (hubRequest) {
		// 	hubRequest = false;
		// }
		for (uint8_t byte0 = 64; byte0 < 128; byte0++) {
			outMsgBuf[0] = byte0;
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