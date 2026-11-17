/* app_defines.h for SB_Mod_servo_dev app */

#ifndef __APP_DEFINES_H__
#define __APP_DEFINES_H__

#include <avr/io.h>

#define SERIAL_BAUDRATE 57600
#define TX_PIN PIN2_bm
#define RX_PIN PIN3_bm

#define LED_PORT PORTA
#define LEDA_PIN  PIN4_bm
#define LEDB_PIN  PIN7_bm

// SensorBus ports, pins, ISR vector
#define SB_PORT 		PORTA
#define SB_CLK  		PIN3_bm
#define SB_ACT  		PIN2_bm
#define SB_DATPORT 		PORTA
#define SB_DAT  		PIN1_bm
#define SB_DAT_CTRL		PIN1CTRL
#define SB_DAT_ISR_VEC 	PORTA_PORT_vect

#define SERVO_A_MIN   313
#define SERVO_A_MAX   625
#define SERVO_B_MIN   313
#define SERVO_B_MAX   625

#endif
