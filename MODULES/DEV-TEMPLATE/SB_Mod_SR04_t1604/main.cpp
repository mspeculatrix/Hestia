/*
 * SB_Mod_SR04_t1604
 *
 * ATtiny1604 firmware code for an SR04 ultrasonic rangefinder module with a
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

volatile bool performPing = false;	// flag set in ISR
// We're setting up the Periodic Interrupt Timer (PIT) to produce an interrupt
// every 0.5sec. We don't want to fire a ping that frequently so we use a
// counter to decide how often to ping.
volatile uint8_t pitTickCounter = 0;
// Following is the number of half-seconds we count before firing off a ping.
const uint8_t PIT_TICK_COUNT_THRESHOLD = 5; // 3 = 1.5second period

// Using serial only for dev & debugging.
SMD_NG_Serial serial = SMD_NG_Serial(SERIAL_BAUDRATE,
	&PORTB, TX_PIN, RX_PIN);

// SR04 sensor object
SB_SR04 sr04 = SB_SR04(&SENSOR_PORT, TRIGGER, ECHO,
	&SB_PORT, SB_CLK, SB_ACT, SB_DAT, &SB_DATPORT, &SB_DATPORT.SB_DAT_CTRL);

/* **** ISRs ***** */

ISR(SB_DAT_ISR_VEC) {
	sr04.commRequestRcvd = SB_DATPORT.INTFLAGS;	// set to bits triggering interrupt(s)
	SB_DATPORT.INTFLAGS = sr04.commRequestRcvd;	// clear flags
}

// PIT timer ISR called ~every 0.5 second to initiate ping
ISR(RTC_PIT_vect) {
	pitTickCounter++;
	RTC.PITINTFLAGS = RTC_PI_bm; 		// Clears the interrupt register flag
	if (pitTickCounter == PIT_TICK_COUNT_THRESHOLD) {
		performPing = true;				// Set event flag
		pitTickCounter = 0;
	}
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
	sr04.sendMsgBuf[0] = 4;
	sr04.sendMsgBuf[1] = SBMSG_USONIC_DATA_US;	// Message type

	SENSOR_PORT.OUTCLR = TRIGGER; 			// Default to low
	SENSOR_PORT.DIRSET = TRIGGER;			// Set as output
	SENSOR_PORT.DIRCLR = ECHO;				// Set as input

	serial.begin();

	pulseLED(ACT_LED);						// Show we're alive
	pulseLED(ALERT_LED);

	PIT_init();		// Config timer for regular pings
	sei();
	PIT_enable();

	serial.writeln("SB_Mod_SR04 running");	// Just for dev/debugging

	/***************************************************************************
	****** MAIN LOOP                                                       *****
	***************************************************************************/

	while (1) {

		if (sr04.commRequestRcvd >= 0) {
			cli();
			PIT_disable();	// Disable pings
			SensorBus::err_code err = sr04.recvMessage(SB_DAT);
			serial.write("<< ");
			if (err == ERR_NONE) {
				//sr04.printMsg(sr04.recvMsgBuf);
			} else {
				serial.writeln(sr04.errMsg(err));
			}
			sr04.commRequestRcvd = -1;
			PIT_restart();	// Re-enable pings
			SB_DATPORT.INTFLAGS = SB_DAT;
			sei();
		}

		if (performPing) {
			cli();
			PIT_disable();	// Disable ping timer
			LED_PORT.OUTSET = ACT_LED;
			performPing = false;
			uint16_t dist = sr04.ping();	// Perform the ping
			//serial.write((int)dist);
			sr04.sendMsgBuf[2] = (uint8_t)(dist & 0x00FF);	// low byte
			sr04.sendMsgBuf[3] = (uint8_t)(dist >> 8);		// high byte
			err_code err = sr04.sendMessage();	// TRANSMIT
			if (err > 0) {
				serial.writeln(sr04.errMsg(err));
			}
			LED_PORT.OUTCLR = ACT_LED;
			// SB_DATPORT.INTFLAGS = SB_DAT;
			sr04.commRequestRcvd = -1;
			PIT_restart();
			sei();
		}
	}
}
