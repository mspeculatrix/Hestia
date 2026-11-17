/*
 * head_node
 *
 * Firmware for Hestia's head control node.
 *
 */

#ifndef __AVR_ATmega4809__
#define __AVR_ATmega4809__
#endif

#ifndef F_CPU
#define F_CPU 20000000UL // 20 MHz unsigned long
#endif

#include <stdlib.h>
#include <stdint.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <SB_nodelib_ng.h>
#include <SB_servolib_t1604.h>
#include <smd_ng_serial.h>
#include "lib/app_defines.h"

 /******************************************************************************
 *****     GLOBALS                                                         *****
 ******************************************************************************/

SB_Node node = SB_Node(&SB_PORT, SB_CLK, SB_ACT, &SB_DATPORT);

// Using serial only for dev & debugging.
SMD_NG_Serial serial = SMD_NG_Serial(SERIAL_BAUDRATE,
	&PORTA, TX_PIN, RX_PIN);


// Interrupt service routine invoked when any DAT line is pulled low.
// It's possible more than one could happen at the same moment.
ISR(SB_DAT_ISR_VEC) {
	node.commRequestRcvd = SB_DATPORT.INTFLAGS;	// set to bits triggering interrupt(s)
	SB_DATPORT.INTFLAGS = node.commRequestRcvd;	// clear flags
}

/*******************************************************************************
*****     FUNCTIONS                                                        *****
*******************************************************************************/

// Uses XORing to get a random number state. Faster than using libc rand()
uint8_t get_rng_state(void) {
	static uint8_t rng_state = 42;  // Arbitrary non-zero start
	rng_state ^= (rng_state << 3);
	rng_state ^= (rng_state >> 5);
	rng_state ^= (rng_state << 7);
	return rng_state;
}

// Get random unsigned int in range 0-180
uint8_t get_random_unsigned_180(void) {
	uint8_t rng_state = get_rng_state();
	return (rng_state * 180u) >> 8;  // Scale 0-255 → 0-179
}

// Get random signed int -128 to 127
int8_t get_random_signed_180(void) {
	uint8_t rng_state = get_rng_state();
	return (int8_t)rng_state;  // 0-255 → -128 to 127 (sign-extended)
}

// Scale an unsigned number in range 0-90 to 0-180
int8_t scale_unsigned_90_to_180(uint8_t input) {
	return (input * 2u);
}

// Scale a signed number in range -45 - 45 to unsigned 0-180
uint8_t scale_signed_90_to_180(int8_t input) {
	return ((uint16_t)(input + 45) * 2u);
}

void printMsg(uint8_t* buf) {
	serial.write(">> ");
	for (uint8_t i = 0; i < buf[0]; i++) {
		serial.write(buf[i]);
		serial.write(" ");
	}
}

/*******************************************************************************
***** MAIN                                                                 *****
*******************************************************************************/
int main(void) {

	//--------------------------------------------------------------------------
	//-----   SETUP                                                        -----
	//--------------------------------------------------------------------------
	cli();						// Disable interrupts while getting setup
	CCP = CCP_IOREG_gc;			// Unlock protected registers
	CLKCTRL.MCLKCTRLB = 0;		// No prescaling, full main clock frequency

	serial.begin();
	serial.writeln("Head node active");

	sei();						// Enable interrupts
	/***************************************************************************
	****** MAIN LOOP                                                       *****
	***************************************************************************/

	uint32_t debug_count = 0;
	node.sendMsgBuf[0] = 4;			// DUMMY DATA - for testing
	node.sendMsgBuf[1] = SBMSG_SET_PARAM;

	while (1) {

		// Send random instructions to servos
		debug_count++;
		if (debug_count == 0x002F0000) {
			cli();
			debug_count = 0;
			node.sendMsgBuf[2] = get_random_unsigned_180();
			node.sendMsgBuf[3] = get_random_unsigned_180();
			printMsg(node.sendMsgBuf);
			err_code err = node.sendMessage(MOD_SERVO);
			serial.writeln(node.errMsg(err));
			SB_DATPORT.INTFLAGS = 0xFF;
			sei();
		}

		// INCOMING MESSAGES FROM MODULES
		if (node.commRequestRcvd >= 0) {
			// A module is requesting comms
			cli();	// Disable interrupts while dealing with this.

			// Get the incoming message
			SensorBus::err_code err = node.recvMessage(node.commRequestRcvd);

			if (err == ERR_NONE) {
				uint8_t device = 0;
				// Following test checks that one and only one bit is set in
				// node.commRequestRcvd
				if ((node.commRequestRcvd > 0)
					&& ((node.commRequestRcvd & (node.commRequestRcvd - 1)) == 0)) {
					// __builtin_ctz(value) turns an eight-bit value with 1 bit set
					// to its bit position value - eg, 00001000 becomes 3.
					// It does this by counting trailing zeros (hence ctz)
					device = __builtin_ctz(node.commRequestRcvd);
					serial.write("<< ");
					serial.write(device);
					serial.write(": ");
					for (uint8_t i = 0; i < node.recvMsgBuf[0]; i++) {
						serial.write(" ");
						serial.write(node.recvMsgBuf[i]);
					}
					serial.writeln(" ");
				} else {
					// multiple trigger error
				}
			} else {
				//serial.writeln(err);
			}

			// When done...
			node.commRequestRcvd = -1;	// reset
			// Clear the interrupt flag one last time to remove any
			// 'tail-end' triggers caused by the module releasing the DAT line
			SB_DATPORT.INTFLAGS = 0xFF;
			sei();						// Re-enable interrupts
		}
	}

}
