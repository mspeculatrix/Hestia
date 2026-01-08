/*
 * SB_Mod_SR04_t1604
 *
 * ATmega1604 firmware code for an SR04 ultrasonic rangefinder module with a
 * SensorBus interface.
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

#include <smd_ng_serial.h>
#include <SB_lib_defines.h>
#include <SB_sr04lib_ng.h>

#include "lib/app_defines.h"
#include "lib/app_functions.h"


 /******************************************************************************
 *****     GLOBALS                                                         *****
 ******************************************************************************/

volatile bool commRequest = false;	// flag set in ISR
volatile bool performPing = false;	// flag set in ISR

uint8_t sbMsgOutBuf[SensorBus::MSG_BUF_LEN];	// For outgoing SB messages
uint8_t sbMsgInBuf[SensorBus::MSG_BUF_LEN];		// For incoming SB messages

// Using serial only for dev & debugging.
SMD_NG_Serial serial = SMD_NG_Serial(SERIAL_BAUDRATE,
	&PORTB, TX_PIN, RX_PIN);

// SR04 sensor object
SB_SR04 sr04 = SB_SR04(&SENSOR_PORT, TRIGGER, ECHO,
	&SB_PORT, SB_CLK, SB_ACT, SB_DAT, &SB_PORT.PIN2CTRL);

/* **** ISRs ***** */

// Interrupt service routine required by SensorBusModule class.
// Invoked when /DAT is pulled low.
ISR(PORTA_PORT_vect) {
	if (SB_PORT.INTFLAGS & SB_DAT) {		// Check if /DAT triggered
		SB_PORT.INTFLAGS = SB_DAT;			// Clear interrupt flag
		commRequest = true;					// Set event flag
	}
}

// PIT timer ISR called ~every 1 second to initiate ping
ISR(RTC_PIT_vect) {
	RTC.PITINTFLAGS = RTC_PI_bm; 	// Clears the interrupt register flag
	performPing = true;				// Set event flag
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
	LED_PORT.DIRSET = ALERT_LED | ACT_LED;	// Set LED GPIOs as outputs
	LED_PORT.OUTCLR = ALERT_LED | ACT_LED;	// Set to off

	// Set up non-changing parts of outgoing message
	sbMsgOutBuf[0] = MSG_BUF_LEN;			// Max message length
	sbMsgOutBuf[1] = SBMSG_USONIC_DATA_US;	// Message type

	SENSOR_PORT.OUTCLR = TRIGGER; 			// Default to low
	SENSOR_PORT.DIRSET = TRIGGER;			// Set as output
	SENSOR_PORT.DIRCLR = ECHO;				// Set as input

	serial.begin();

	pulseLED(ACT_LED);						// Show we're alive
	pulseLED(ALERT_LED);

	PIT_init();								// Config timer for regular pings
	sei();
	PIT_enable();

	serial.writeln("SB_Mod_SR04 running");	// Just for dev/debugging

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
			uint16_t dist = sr04.ping();
			serial.writeln((int)dist);
			performPing = false;
			// sbMsgOutBuf[0] = 4;
			// sbMsgOutBuf[2] = (uint8_t)(dist & 0x00FF);	// low byte
			// sbMsgOutBuf[3] = (uint8_t)(dist >> 8);		// high byte
			// sbMod.sendMessage(sbMsgOutBuf);
			LED_PORT.OUTCLR = ACT_LED;
		}

	}
}
