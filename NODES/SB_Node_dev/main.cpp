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

	uint32_t debug_count = 0;
	node.sendMsgBuf[0] = 6;			// DUMMY DATA - for testing
	node.sendMsgBuf[1] = 30;
	node.sendMsgBuf[2] = 44;
	node.sendMsgBuf[3] = 55;
	node.sendMsgBuf[4] = 66;
	node.sendMsgBuf[5] = 255;


	while (1) {

		// DUMMY ROUTINE - for testing the send functionality
		debug_count++;
		if (debug_count == 0x005F0000) {
			debug_count = 0;
			cli();
			// node.printBuf(node.sendMsgBuf);
			err_code err = node.sendMessage(MOD_SRO4);
			if (err > 0) {
				serial.writeln(node.errMsg(err));
			}
			SB_DATPORT.INTFLAGS = 0xFF;
			sei();
		}


		if (node.commRequestRcvd >= 0) {
			// A module is requesting comms
			cli();	// Disable interrupts while dealing with this.

			// commRequestRcvd's bits tells you which module(s) made the request
			// We should cycle through them and call responder functions as
			// appropriate.

			// The order of these if statements determines the priority we
			// place on the sensors.

			if (node.commRequestRcvd & (MOD_SRO4)) {
				SensorBus::err_code err = node.recvMessage(MOD_SRO4);
				serial.write("<< ");
				// __builtin_ctz(value) turns an eight-bit value with 1 bit set
				// to its bit position value - eg, 00001000 becomes 3.
				// It does this by counting trailing zeros (hence ctz)
				serial.write(__builtin_ctz(node.commRequestRcvd));
				if (err == ERR_NONE) {
					uint16_t data = node.recvMsgBuf[2];
					data |= node.recvMsgBuf[3] << 8;
					serial.write(" ");
					serial.write((int)data);
					serial.writeln(" ");
				} else {
					serial.write(" err: "); serial.write(node.errMsg(err));
					serial.write(" : "); serial.write(SB_DATPORT.DIR);
					serial.write(" : "); serial.writeln(SB_DATPORT.IN);
				}
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
