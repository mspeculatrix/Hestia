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
#include "lib/app_functions.h"
#include "lib/SBlib_defines.h"
#include "lib/SBlib_avrmod.h"
#include "lib/SB_sr04lib_avrmod.h"


 /******************************************************************************
 *****     GLOBALS                                                         *****
 ******************************************************************************/

 // Using serial only for dev & debugging.
SMD_AVRMod_Serial serial = SMD_AVRMod_Serial(SERIAL_BAUDRATE,
	&PORTB, TX_PIN, RX_PIN);

volatile bool commRequest = false;
volatile bool performPing = false;
uint8_t sbMsgOutBuf[MSG_BUF_LEN];	// Buffer for outgoing SB messages
uint8_t sbMsgInBuf[MSG_BUF_LEN];	// Buffer for incoming SB messages

SensorBusModule sbMod = SensorBusModule(&SB_PORT, SB_DAT, SB_CLK, SB_ACT,
	&SB_PORT.PIN2CTRL, MSG_BUF_LEN);

// *** ISRs ***
// Interrupt service routine required by SensorBusModule class.
// Invoked when /DAT is pulled low.
ISR(PORTA_PORT_vect) {
	if (SB_PORT.INTFLAGS & SB_DAT) {		// Check if /DAT triggered
		SB_PORT.INTFLAGS = SB_DAT;		// Clear interrupt flag
		commRequest = true;				// Set event flag
	}
}

// ISR called ~every 1 second to initiate ping
ISR(RTC_PIT_vect) {
	// Clear interrupt flag
	RTC.PITINTFLAGS = RTC_PI_bm; // Writing this bit clears the flag
	performPing = true;
}


/*******************************************************************************
*****   MAIN                                                               *****
*******************************************************************************/
int main(void) {

	//--------------------------------------------------------------------------
	//-----   SETUP                                                        -----
	//--------------------------------------------------------------------------

	CCP = CCP_IOREG_gc;     		// Unlock protected registers
	CLKCTRL.MCLKCTRLB = 0;  		// No prescaling, full main clock frequency

	// Setup GPIOs
	LED_PORT.DIRSET = ALERT_LED | ACT_LED;	// Set as outputs
	LED_PORT.OUTCLR = ALERT_LED | ACT_LED;	// Set to off

	// Set up non-changing parts of outgoing message
	sbMsgOutBuf[0] = MSG_BUF_LEN;			// Max message length
	sbMsgOutBuf[1] = SBMSG_USONIC_DATA_US;

	SENSOR_PORT.OUTCLR = TRIGGER; 			// Default to low
	SENSOR_PORT.DIRSET = TRIGGER;			// Output
	SENSOR_PORT.DIRCLR = ECHO;				// Input

	serial.begin();
	sbMod.init();

	enableSensorTimer(); 		// For timing echoes

	pulseLED(ACT_LED);
	pulseLED(ALERT_LED);

	PIT_init();					// Config timer for regular pings
	sei();
	PIT_enable();

	serial.writeln("SB_Mod_SR04 running");

	/***************************************************************************
	****** MAIN LOOP                                                       *****
	***************************************************************************/

	while (1) {

		// if (commRequest) {
		//	PIT_disable();	// Disable pings
		// 	commRequest = false;
		// 	sbMod.setReceiveMode(); // go into receive mode
		// 	sbMod.strobeClk();

		// 	// uint8_t msgLen = 0;
		// 	// read a byte & set msgLen
		// 	// loop for remaining bytes
		// 	// decide what to do with message

		//	PIT_enable();	// Re-enable pings
		// }

		if (performPing) {
			LED_PORT.OUTSET = ACT_LED;
			uint16_t dist = ping();
			serial.writeln((int)dist);
			performPing = false;
			LED_PORT.OUTCLR = ACT_LED;
		}
		// sbMsgOutBuf[0] = 4;
		// sbMsgOutBuf[2] = (uint8_t)(dist & 0x00FF);	// low byte
		// sbMsgOutBuf[3] = (uint8_t)(dist >> 8);		// high byte
		// sbMod.sendMessage(sbMsgOutBuf);

	}
}
