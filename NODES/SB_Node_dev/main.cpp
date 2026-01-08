/*
 * SB_Node_dev
 *
 * ATmega4809 firmware code for SensorBus node.
 */

#ifndef __AVR_ATmega4809__
#define __AVR_ATmega4809__
#endif

#ifndef F_CPU
#define F_CPU 20000000UL // 20 MHz unsigned long
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <SB_nodelib_ng.h>
#include <smd_ng_serial.h>

#include "lib/app_defines.h"
#include "lib/app_functions.h"

 /******************************************************************************
 *****     GLOBALS                                                         *****
 ******************************************************************************/

volatile int8_t commRequest = -1;	// flag set in ISR

uint8_t sbMsgOutBuf[MSG_BUF_LEN];	// Buffer for outgoing SB messages
uint8_t sbMsgInBuf[MSG_BUF_LEN];	// Buffer for incoming SB messages

// Using serial only for dev & debugging.
SMD_NG_Serial serial = SMD_NG_Serial(SERIAL_BAUDRATE,
	&PORTA, TX_PIN, RX_PIN);


// Interrupt service routine invoked when any DAT line is pulled low.
ISR(DAT_ISR_VECTOR) {
	uint8_t commRequest = DAT_PORT.INTFLAGS;
	DAT_PORT.INTFLAGS = commRequest; // clear flags
}

/******************************************************************************
***** MAIN                                                                *****
******************************************************************************/
int main(void) {

	//--------------------------------------------------------------------------
	//-----   SETUP                                                        -----
	//--------------------------------------------------------------------------

	CCP = CCP_IOREG_gc;     // Unlock protected registers
	CLKCTRL.MCLKCTRLB = 0;  // No prescaling, full main clock frequency

	// Configure DAT port pins
	DAT_PORT.DIRCLR = 0xFF;	// Set DAT port pins as inputs
	// Enable internal pull-ups and set Interrupt Sense Control (ISC)
	// to falling edge
	for (uint8_t i = 0; i < 8; i++) {
		// Accessing PINnCTRL as an array starting at PIN0CTRL
		*((uint8_t*)&DAT_PORT.PIN0CTRL + i) = PORT_PULLUPEN_bm | PORT_ISC_FALLING_gc;
	}

	serial.begin();
	serial.writeln("Node active");

	sei();
	/***************************************************************************
	****** MAIN LOOP                                                       *****
	***************************************************************************/

	while (1) {
		if (commRequest >= 0) {
			// A module is requesting comms
			cli();	// Disable interrupts while dealing with this.
			// commRequest's bits tells you which module(s) made the request
			// We should cycle through them and call responder functions as
			// appropriate.

			// The first step, however, will be to retrieve the message.
			// So we'll want a getMessage(module) function where we pass which
			// module we want to talk to.

			// The order of these if statements determines the priority we
			// place on the sensors.

			if (commRequest & (1 << MOD_SRO4)) {
				bool received = getMessage(MOD_SRO4, sbMsgInBuf);
				if (received) {
					// now, what do we do with it?
				}
			}

			// When done...
			commRequest = -1;	// reset
			sei();				// Re-enable interrupts
		}
	}

}
