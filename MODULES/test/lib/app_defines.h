#ifndef __DEFINES_H__
#define __DEFINES_H__

#include <avr/io.h>

#define SERIAL_BAUDRATE 19200
#define TX_PIN PIN2_bm
#define RX_PIN PIN3_bm
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

// SensorBus pins
#define SB_PORT PORTA
#define SB_DAT PIN2_bm // Chip Enable (Active Low: /CE)
#define SB_CLK PIN3_bm // Output Enable (Active Low: /OE)
#define SB_ACT PIN1_bm // Output Enable (Active Low: /OE)
#define SB_PORT_INT_VEC PORTA_PORT_vect
#define SB_DAT_CTRL PIN2CTRL // only for non-class stuff

#define LED_PORT  PORTB
#define ALERT_LED PIN0_bm
#define ACT_LED   PIN1_bm

#define WAIT_UNTIL_HIGH(port, pin) do { } while (!(port.IN & (pin)))
#define WAIT_UNTIL_LOW(port, pin) do { } while (port.IN & (pin))

#endif
