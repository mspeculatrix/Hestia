/*
 * SB_Node_dev
 *
 * ATmega4809 firmware code for a generic SensorBus node.
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

SB_Node node = SB_Node(&SB_PORT, SB_CLK, SB_ACT, &DAT_PORT);

// Using serial only for dev & debugging.
SMD_NG_Serial serial = SMD_NG_Serial(SERIAL_BAUDRATE,
	&PORTA, TX_PIN, RX_PIN);


// Interrupt service routine invoked when any DAT line is pulled low.
// It's possible more than one could happen at the same moment.
ISR(DAT_ISR_VECTOR) {
	commRequest = DAT_PORT.INTFLAGS;	// set to bits triggering interrupt(s)
	DAT_PORT.INTFLAGS = commRequest;	// clear flags
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
	serial.writeln("Node active");

	sei();						// Enable interrupts
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

			// The order of these if statements determines the priority we
			// place on the sensors.

			if (commRequest & (MOD_SRO4)) {
				// __builtin_ctz(value) turns an eight-bit value with 1 bit set
				// to its bit position value - eg, 00001000 becomes 3.
				// It does this by counting trailing zeros (hence ctz)
				serial.write(__builtin_ctz(commRequest));
				SensorBus::err_code err = node.recvMessage(MOD_SRO4);
				if (err == ERR_NONE) {
					serial.write(": ");
					for (uint8_t i = 0; i < node.recvMsg[0]; i++) {
						serial.write(node.recvMsg[i]);
						serial.write(" ");
					}
					serial.writeln(" ");
				} else {
					serial.write(" err: "); serial.write(node.errMsg(err));
					serial.write(" : "); serial.write(DAT_PORT.DIR);
					serial.write(" : "); serial.writeln(DAT_PORT.IN);
				}
			}

			// When done...
			commRequest = -1;	// reset
			// Clear the interrupt flag one last time to remove any
			// 'tail-end' triggers caused by the module releasing the DAT line.
			DAT_PORT.INTFLAGS = 0xFF;
			sei();				// Re-enable interrupts
		}
	}

}
