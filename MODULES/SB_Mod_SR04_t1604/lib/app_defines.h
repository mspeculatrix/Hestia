#ifndef __DEFINES_H__
#define __DEFINES_H__

#include <avr/io.h>

#define SERIAL_BAUDRATE 19200
#define MSG_BUF_LEN 16

#define NEWLINE 10
#define CR 13

#define DEFAULT_LED_FLASH_DELAY 150
#define DEFAULT_LED_PULSE_DELAY 150
#define DEFAULT_LED_PULSE_CYCLES 3

// Pin allocations
#define SENSOR_PORT PORTA
#define ECHO      PIN5_bm
#define TRIGGER   PIN4_bm

#define LED_PORT PORTB
#define ALERT_LED PIN0_bm
#define ACT_LED   PIN1_bm

#define WAIT_UNTIL_HIGH(port, pin) do { } while (!(port.IN & (pin)))
#define WAIT_UNTIL_LOW(port, pin) do { } while (port.IN & (pin))

#endif
