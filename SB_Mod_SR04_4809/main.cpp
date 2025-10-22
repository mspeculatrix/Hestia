/*

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

#include "lib/app_defines.h"
#include "lib/smd_avr0_serial.h"
#include "lib/functions_misc.h"
#include "lib/SBlib_defines.h"
#include "lib/SBlib_avr0.h"
#include "lib/SB_sr04lib_avr0.h"

#define MODULE_ID 0xAA

 /*******************************************************************************
 ***** GLOBALS                                                              *****
 *******************************************************************************/

SMD_AVR0_Serial serial = SMD_AVR0_Serial(SERIAL_BAUDRATE);
bool commRequest = false;
uint8_t sbMsgBuf[MSG_LEN];	// Buffer for SB messages
volatile uint8_t* datCtrl = &PORTD.PIN0CTRL;

// SensorBusModule sbMod = SensorBusModule(&SB_PORT, SB_DAT, SB_CLK, datCtrl);

// Interrupt service routine - invoked when /DAT is pulled low
ISR(SB_PORT_INT_VEC) {
	if (SB_PORT.INTFLAGS & SB_DAT) {		// Check if SB_DAT triggered
		SB_PORT.INTFLAGS = SB_DAT;		// Clear interrupt flag
		commRequest = true;				// Set event flag
	}
}

/*******************************************************************************
***** MAIN                                                               *****
*******************************************************************************/
int main(void) {

	//--------------------------------------------------------------------------
	//-----   SETUP                                                        -----
	//--------------------------------------------------------------------------

	CCP = CCP_IOREG_gc;     // Unlock protected registers
	CLKCTRL.MCLKCTRLB = 0;  // No prescaling, full main clock frequency

	// Configure non-existent pins with pullups (as in original code)
	PORTB.PIN0CTRL = PORT_PULLUPEN_bm;
	PORTB.PIN1CTRL = PORT_PULLUPEN_bm;
	PORTB.PIN2CTRL = PORT_PULLUPEN_bm;
	PORTB.PIN3CTRL = PORT_PULLUPEN_bm;
	PORTB.PIN4CTRL = PORT_PULLUPEN_bm;
	PORTB.PIN5CTRL = PORT_PULLUPEN_bm;
	PORTC.PIN6CTRL = PORT_PULLUPEN_bm;
	PORTC.PIN7CTRL = PORT_PULLUPEN_bm;

	SB_PORT.DIRCLR = SB_CLK | SB_DAT; // set as inputs to start with
	SENSOR_PORT.OUTCLR = TRIGGER; 	// Default to low
	SENSOR_PORT.DIRSET = TRIGGER;	// Output
	SENSOR_PORT.DIRCLR = ECHO;		// Input

	serial.begin();

	// Set up non-changing parts of message
	sbMsgBuf[0] = MSG_LEN;		// Message length
	sbMsgBuf[1] = SENSOR_DATA_US;

	char serialMsgBuf[MSG_BUF_LEN]; // For sending messages via serial
	uint8_t msgIdx = 0;
	bool msgRecvd = false;			// Do we have one yet?

	SB_PORT.SB_DAT_CTRL = PORT_PULLUPEN_bm | PORT_ISC_FALLING_gc;

	enableTimer();

	serial.writeln("Running");

	/***************************************************************************
	****** MAIN LOOP                                                       *****
	***************************************************************************/

	while (1) {
		// uint16_t dist = ping();
		uint16_t dist = 0xAA55;

		sbMsgBuf[2] = (uint8_t)(dist & 0x00FF); // low byte
		sbMsgBuf[3] = (uint8_t)(dist >> 8); 	// high byte
		sbSendMessage(sbMsgBuf, MSG_LEN);
		// SB_PORT.OUTSET = SB_DAT;						// Set to high
		// SB_PORT.DIRSET = SB_DAT;						// Set DAT to output
		// SB_PORT.OUTCLR = SB_DAT;						// Pulse DAT low
		// _delay_ms(SB_PULSE_LENGTH);
		// SB_PORT.OUTSET = SB_DAT;						// Set to high
		// SB_PORT.DIRCLR = SB_DAT;						// Set DAT to input

		_delay_ms(1000);
	}
}
