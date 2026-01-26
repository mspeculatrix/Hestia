/* app_defines.h for SB_Mod_servo_dev app */

#ifndef __APP_DEFINES_H__
#define __APP_DEFINES_H__

#include <avr/io.h>

#define SERIAL_BAUDRATE 57600
#define TX_PIN PIN2_bm
#define RX_PIN PIN3_bm

// Pin allocations
#define SERVO_PORT  PORTA
#define SERVO_PIN   PIN3_bm

#define LED_PORT PORTB
#define LED_PIN  PIN0_bm

// SensorBus ports, pins, ISR vector
#define SB_PORT 		PORTA
#define SB_CLK  		PIN5_bm
#define SB_ACT  		PIN2_bm
#define SB_DATPORT 		PORTA
#define SB_DAT  		PIN1_bm
#define SB_DAT_CTRL		PIN1CTRL
#define SB_DAT_ISR_VEC 	PORTA_PORT_vect

#endif
