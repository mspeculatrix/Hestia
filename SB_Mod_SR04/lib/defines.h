#ifndef __DEFINES_H__
#define __DEFINES_H__

// Pin allocations
#define ACT_LED   PB2
#define ALERT_LED PA7
#define TRIGGER   PB1
#define ECHO      PB0
#define SB_DAT    PA0
#define SB_CLK    PA1
#define SB_INT    PA3 // This line should be protected with a diode
#define SB_ACT    PA2

#define MSG_BUF_LEN 3

#define INT_PULSE_LENGTH 10 		// Interrupt pulse duration in ms
#define STABILISE_DELAY 50 			// ms to wait for other system to stabilise
#define TIMEOUT_LOOP_DELAY 5 		// ms pause in connection loops
#define MAX_CONNECTION_ATTEMPTS 10

// Transmission delays
#define START_TRANSMISSION_PAUSE 50
#define BIT_PAUSE 50
#define BYTE_PAUSE 50

#define ERR_HUB_NOT_RESPONDING 1
#define ERR_MAX_ATTEMPTS 2

#define DAT_INT_ENABLE PCMSK0 |= (1 << PCINT0)
#define DAT_INT_DISABLE PCMSK0 &= ~(1 << PCINT0)

#endif
