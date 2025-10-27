/*
 * SB_Mod_SR04
 *
 * ATmega4809-P code for an SR04 ultrasonic rangefinder using a SensorBus
 * interface.
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


 /*******************************************************************************
 ***** GLOBALS                                                              *****
 *******************************************************************************/

 // Using serial only for dev & debugging.
SMD_AVR0_Serial serial = SMD_AVR0_Serial(SERIAL_BAUDRATE);

bool commRequest = false;
uint8_t sbMsgOutBuf[MSG_BUF_LEN];	// Buffer for outgoing SB messages
uint8_t sbMsgInBuf[MSG_BUF_LEN];	// Buffer for incoming SB messages

SensorBusModule sbMod = SensorBusModule(&PORTD, PIN0_bm, PIN1_bm, PIN2_bm, &PORTD.PIN0CTRL, MSG_BUF_LEN);

// Interrupt service routine - required by SensorBusModule class.
// Invoked when /DAT is pulled low.
ISR(PORTD_PORT_vect) {
	if (PORTD.INTFLAGS & PIN0_bm) {		// Check if /DAT triggered
		PORTD.INTFLAGS = PIN0_bm;		// Clear interrupt flag
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

	SENSOR_PORT.OUTCLR = TRIGGER; 	// Default to low
	SENSOR_PORT.DIRSET = TRIGGER;	// Output
	SENSOR_PORT.DIRCLR = ECHO;		// Input

	serial.begin();
	sbMod.init();

	// Set up non-changing parts of outgoing message
	sbMsgOutBuf[0] = MSG_BUF_LEN;		// Max message length
	sbMsgOutBuf[1] = SENSOR_DATA_US;

	enableSensorTimer();

	serial.writeln("Running");

	/***************************************************************************
	****** MAIN LOOP                                                       *****
	***************************************************************************/

	while (1) {

		// if (commRequest) {
		// 	commRequest = false;
		// 	sbMod.setReceiveMode(); // go into receive mode
		// 	sbMod.strobeClk();

		// 	// uint8_t msgLen = 0;
		// 	// read a byte & set msgLen
		// 	// loop for remaining bytes
		// 	// decide what to do with message
		// }

		// uint16_t dist = 0xAA55;
		uint16_t dist = ping();
		serial.writeln((int)dist);
		sbMsgOutBuf[0] = 4;
		sbMsgOutBuf[2] = (uint8_t)(dist & 0x00FF);	// low byte
		sbMsgOutBuf[3] = (uint8_t)(dist >> 8);		// high byte
		sbMod.sendMessage(sbMsgOutBuf);

		_delay_ms(1000);
	}
}
