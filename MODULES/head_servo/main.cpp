/* HEAD_SERVO

Firmware for a servo control module for the Hestia sensor head.

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
#include <SB_servolib_t1604.h>

#include "lib/app_defines.h"

 /******************************************************************************
  *****     GLOBALS                                                         *****
  ******************************************************************************/

  // Using serial only for dev & debugging.
SMD_NG_Serial serial = SMD_NG_Serial(SERIAL_BAUDRATE,
	&PORTB, TX_PIN, RX_PIN);

SB_Servo::SB_Servo_t1604 servo = SB_Servo::SB_Servo_t1604(
	PAN_SERVO_MIN, PAN_SERVO_MAX, TILT_SERVO_MIN, TILT_SERVO_MAX,
	&SB_PORT, SB_CLK, SB_ACT, SB_DAT, &SB_DATPORT, &SB_DATPORT.SB_DAT_CTRL);

/* **** ISRs ***** */

ISR(SB_DAT_ISR_VEC) {
	servo.commRequestRcvd = SB_DATPORT.INTFLAGS; // gets bits triggering int(s)
	SB_DATPORT.INTFLAGS = servo.commRequestRcvd; // clear flags
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

	serial.begin();
	servo.begin();

	serial.writeln("head_servo running");	// Just for dev/debugging

	/***************************************************************************
	****** MAIN LOOP                                                       *****
	***************************************************************************/

	while (1) {
		if (servo.commRequestRcvd >= 0) {
			cli();
			SensorBus::err_code err = servo.recvMessage(SB_DAT);
			if (err == ERR_NONE) {
				serial.write("<< ");
				switch (servo.recvMsgBuf[1]) {
					case SBMSG_SET_PARAM:
						servo.setAngle(servo.recvMsgBuf[2], servo.recvMsgBuf[3]);
						servo.setAngle(servo.recvMsgBuf[4], servo.recvMsgBuf[5]);
						break;
				}
				servo.printMsg(servo.recvMsgBuf);
			} else {
				serial.writeln(servo.errMsg(err));
			}
			servo.commRequestRcvd = -1;
			SB_DATPORT.INTFLAGS = SB_DAT;
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
