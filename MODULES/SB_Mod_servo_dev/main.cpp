/* SB_Mod_servo_dev

 ATtiny1604 firmware code for a servo control board with a SensorBus interface.

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
#include <SB_servolib_ng.h>

#include "lib/app_defines.h"

 /******************************************************************************
  *****     GLOBALS                                                         *****
  ******************************************************************************/

  // Using serial only for dev & debugging.
SMD_NG_Serial serial = SMD_NG_Serial(SERIAL_BAUDRATE,
	&PORTB, TX_PIN, RX_PIN);

SB_Servo servo = SB_Servo(&SERVO_PORT, SERVO_PIN, &SB_PORT, SB_CLK, SB_ACT,
	SB_DAT, &SB_DATPORT, &SB_DATPORT.SB_DAT_CTRL);

/* **** ISRs ***** */

ISR(SB_DAT_ISR_VEC) {
	servo.commRequestRcvd = SB_DATPORT.INTFLAGS;	// set to bits triggering interrupt(s)
	SB_DATPORT.INTFLAGS = servo.commRequestRcvd;	// clear flags
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

	SERVO_PORT.DIRSET = SERVO_PIN; 	// Set as output, always and forever
	LED_PORT.DIRSET = LED_PIN;		// -- ditto --
	LED_PORT.OUTCLR = LED_PIN;		// Startup OFF

	serial.begin();

	serial.writeln("SB_Mod_servo_dev running");	// Just for dev/debugging

	/***************************************************************************
	****** MAIN LOOP                                                       *****
	***************************************************************************/

	while (1) {
		if (servo.commRequestRcvd >= 0) {
			cli();
			SensorBus::err_code err = servo.recvMessage(SB_DAT);
			serial.writeln("<< ");
			if (err == ERR_NONE) {
				servo.printMsg(servo.recvMsgBuf);
			} else {
				serial.writeln(servo.errMsg(err));
			}
			servo.commRequestRcvd = -1;
			SB_DATPORT.INTFLAGS = SB_DAT;
			sei();
		}
	}
}