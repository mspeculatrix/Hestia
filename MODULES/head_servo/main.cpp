/* HEAD_SERVO

Firmware for a servo control module for the Hestia sensor head.

NB: Serial comms is needed only for dev/debugging and can be removed
completely for deployment.

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
#include <SB_servolib_t1604.h>

#include "lib/app_defines.h"

/*******************************************************************************
*****     GLOBALS                                                          *****
*******************************************************************************/

// Using serial only for dev & debugging.
// SMD_NG_Serial serial = SMD_NG_Serial(SERIAL_BAUDRATE,
// 	&PORTB, TX_PIN, RX_PIN);

SB_Servo::SB_Servo_t1604 servo = SB_Servo::SB_Servo_t1604(
	PAN_SERVO_MIN, PAN_SERVO_MAX, TILT_SERVO_MIN, TILT_SERVO_MAX);

/* **** ISRs ***** */

ISR(PORTA_PORT_vect) {
	servo.getIntFlags();
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

	// serial.begin();
	servo.begin();

	servo.setLED(servo.LED_A, ON);
	_delay_ms(250);
	servo.setLED(servo.LED_B, ON);
	_delay_ms(500);
	servo.setLED(servo.LED_A, OFF);
	_delay_ms(250);
	servo.setLED(servo.LED_B, OFF);

	// serial.writeln("head_servo running");	// Just for dev/debugging

	/***************************************************************************
	****** MAIN LOOP                                                       *****
	***************************************************************************/
	while (1) {
		if (servo.commRequestRcvd >= 0) {
			cli();
			SensorBus::err_code err = servo.recvMessage();
			servo.setLED(servo.LED_A, ON);
			if (err == ERR_NONE) {
				servo.respondToMessage();
				servo.serial.write("<< ");
				servo.printMsg(servo.recvMsgBuf);
			} else {
				servo.serial.write(err);
				servo.serial.write(" : ");
				servo.serial.writeln(servo.errMsg(err));
			}
			servo.commRequestRcvd = -1;				// Reset
			servo.setLED(servo.LED_A, OFF);
			servo.resetIntFlag();
			sei();
		}

		// SENDING MESSAGES
		// cli();
		// err_code err = servo.sendMessage();
		// if (err > 0) {
		// serial.writeln(servo.errMsg(err));
		// }
		// Finally...
		// SB_DATPORT.INTFLAGS = 0xFF;
		// sei();

	}
}
