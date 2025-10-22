#ifndef __FUNCTIONS_H__
#define __FUNCTIONS_H__


#include <avr/io.h>
#include <util/delay.h>
#include "defines.h"
#include "smd_std_macros.h"

// Prototypes
void flashActive(uint8_t);
void flashAlert(uint8_t);
void flashLED(volatile uint8_t*, uint8_t, uint8_t, uint8_t);
void setPin(volatile uint8_t*, uint8_t, uint8_t);

// void clearBuf(char* buf, uint8_t len) {
// 	for (uint8_t i = 0; i < len; i++) {
// 		buf[i] = 0;
// 	}
// }

void flashActive(uint8_t times) {
	flashLED(&PORTB, ACT_LED, times, 100);
}

void flashAlert(uint8_t times) {
	flashLED(&PORTA, ALERT_LED, times, 100);
}

void flashLED(volatile uint8_t* portreg, uint8_t led, uint8_t times, uint8_t duration = 100) {
	for (uint8_t i = 0; i < times; i++) {
		setPin(portreg, led, HIGH);
		_delay_ms(duration / 2);
		setPin(portreg, led, LOW);
		_delay_ms(duration / 2);
	}
}

void setPin(volatile uint8_t* portreg, uint8_t pin, uint8_t hilo) {
	if (hilo == 1) {
		*portreg |= (1 << pin);
	} else {
		*portreg &= ~(1 << pin);
	}
}


#endif
