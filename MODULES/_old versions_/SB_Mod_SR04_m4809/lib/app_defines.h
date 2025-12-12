#ifndef __DEFINES_H__
#define __DEFINES_H__

#include <avr/io.h>

#define SERIAL_BAUDRATE 19200
#define MSG_BUF_LEN 16

#define NEWLINE 10
#define CR 13

#define SENSOR_PORT PORTC
#define ECHO      PIN0_bm
#define TRIGGER   PIN1_bm

// Pin allocations
#define LED_PORT PORTF
#define ACT_LED   PIN0_bm
#define ALERT_LED PIN1_bm

#define WAIT_UNTIL_HIGH(port, pin) do { } while (!(port.IN & (pin)))
#define WAIT_UNTIL_LOW(port, pin) do { } while (port.IN & (pin))

#endif
