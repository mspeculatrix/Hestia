/*
 * SB_Node_dev
 *
 * ATmega4809 firmware code for a generic SensorBus node. Use as a template.
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
	serial.writeln("Node active");

	sei();						// Enable interrupts
	/***************************************************************************
	****** MAIN LOOP                                                       *****
	***************************************************************************/

	while (1) {

		// SEND MESSAGE
		// Condition for sending message
		// cli();
		// Load relevant values into node.sendMsgBuf
		// err_code err = node.sendMessage(MOD_0);
		// Deal with any errors
		// Finally...
		// SB_DATPORT.INTFLAGS = 0xFF;
		// sei();

		// DEAL WITH RECEIVED MESSAGE
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
