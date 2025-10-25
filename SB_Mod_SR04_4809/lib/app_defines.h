#ifndef __DEFINES_H__
#define __DEFINES_H__

#include <avr/io.h>

// #ifndef __AVR_ATmega4809__
// #define __AVR_ATmega4809__
// #endif

#define SERIAL_BAUDRATE 19200
#define MSG_BUF_LEN 16

#define NEWLINE 10
#define CR 13

#define SB_PORT PORTD
#define SB_DAT PIN0_bm // Chip Enable (Active Low: /CE)
#define SB_CLK PIN1_bm // Output Enable (Active Low: /OE)
#define SB_ACT PIN2_bm // Output Enable (Active Low: /OE)
#define SB_PORT_INT_VEC PORTD_PORT_vect
#define SB_DAT_CTRL PIN0CTRL // only for non-class stuff
#define SB_PULSE_LENGTH 500 	// us

#define SENSOR_PORT PORTC
#define ECHO      PIN0_bm
#define TRIGGER   PIN1_bm

// Pin allocations
// #define ACT_LED   PB2
// #define ALERT_LED PA7

#define STABILISE_DELAY 50 			// ms to wait for other system to stabilise
#define TIMEOUT_LOOP_DELAY 5 		// ms pause in connection loops
#define MAX_INIT_ATTEMPTS 10
#define MAX_CONNECTION_ATTEMPTS 10
#define INIT_RETRY_DELAY 5

#define ERR_HUB_NOT_RESPONDING 1
#define ERR_MAX_ATTEMPTS 2

#define WAIT_UNTIL_HIGH(port, pin) do { } while (!(port.IN & (pin)))
#define WAIT_UNTIL_LOW(port, pin) do { } while (port.IN & (pin))

// #define SET_CLK_INPUT DDRA &= ~(1 << SB_CLK)
// #define SET_CLK_OUTPUT DDRA |= (1 << SB_CLK)

// #define SET_DAT_INPUT DDRA &= ~(1 << SB_DAT)
// #define SET_DAT_OUTPUT DDRA |= (1 << SB_DAT)

// #define DAT_INT_ENABLE PCMSK0 |= (1 << PCINT0)
// #define DAT_INT_DISABLE PCMSK0 &= ~(1 << PCINT0)

#endif
