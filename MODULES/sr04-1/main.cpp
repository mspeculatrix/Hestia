/**
* sr04 - 1
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

#include <SB_lib_defines.h>
#include <SB_sr04lib_ng.h>

/*******************************************************************************
*****     GLOBALS                                                          *****
*******************************************************************************/

volatile bool performPing = false;	// flag set in ISR
// We're setting up the Periodic Interrupt Timer (PIT) to produce an interrupt
// every 0.5sec. We don't want to fire a ping that frequently so we use a
// counter to decide how often to ping.
volatile uint8_t pitTickCounter = 0;
// Following is the number of half-seconds we count before firing off a ping.
const uint8_t PIT_TICK_COUNT_THRESHOLD = 5; // 3 = 1.5second period

// SR04 sensor object
SB_SR04 sr04 = SB_SR04();

/* **** ISRs ***** */

ISR(PORTA_PORT_vect) {
	sr04.getIntFlags();
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

	sr04.begin();

	// Set up non-changing parts of outgoing message
	sr04.sendMsgBuf[0] = 4;
	sr04.sendMsgBuf[1] = SBMSG_USONIC_DATA_US;	// Message type

	sr04.pulseLED(sr04.ACT_LED);						// Show we're alive
	sr04.pulseLED(sr04.ALERT_LED);

	sr04.PIT_init();		// Config timer for regular pings
	sei();
	sr04.PIT_enable();

	sr04.serial.writeln("SB_Mod_SR04 running");	// Just for dev/debugging

	/***************************************************************************
	****** MAIN LOOP                                                       *****
	***************************************************************************/

	while (1) {

		if (sr04.commRequestRcvd >= 0) {
			cli();
			sr04.PIT_disable();	// Disable pings
			SensorBus::err_code err = sr04.recvMessage();
			sr04.serial.write("<< ");
			if (err == ERR_NONE) {
				//sr04.printMsg(sr04.recvMsgBuf);
			} else {
				sr04.serial.writeln(sr04.errMsg(err));
			}
			sr04.commRequestRcvd = -1;
			sr04.PIT_restart();	// Re-enable pings
			sr04.resetIntFlag();
			sei();
		}

		if (performPing) {
			cli();
			sr04.PIT_disable();	// Disable ping timer
			sr04.setLED(sr04.ACT_LED, ON);
			performPing = false;
			uint16_t dist = sr04.ping();	// Perform the ping
			//serial.write((int)dist);
			sr04.sendMsgBuf[2] = (uint8_t)(dist & 0x00FF);	// low byte
			sr04.sendMsgBuf[3] = (uint8_t)(dist >> 8);		// high byte
			err_code err = sr04.sendMessage();	// TRANSMIT
			if (err > 0) {
				sr04.serial.writeln(sr04.errMsg(err));
			}
			sr04.setLED(sr04.ACT_LED, OFF);
			// SB_DATPORT.INTFLAGS = SB_DAT;
			sr04.commRequestRcvd = -1;
			sr04.PIT_restart();
			sei();
		}
	}
}
