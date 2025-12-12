/*
 * SmartHCSR04.cpp
 *
 * Created: 11/27/2018 12:32:39 PM
 * Author : Steve
 */
#ifndef F_CPU
#define F_CPU 1000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdio.h>
#include <smd_avr_usi_i2clib.h>
#include <uproc/smd_attiny_24_44_84A.h>
 //#include <smd_avr_sensorbuslib.h>

#define TRIG_PIN PA1			// orange
#define ECHO_PIN PA2			// yellow
#define ALERT_PIN PA3			// goes low to alert hub
#define LCD_I2C_ADDR 0xC6
#define MAX_ECHO_TIME 23200		// 23200us is 400cm max range.
#define DIST_FACTOR 58

#define ALERT_PULSE_TIME 50		// us
#define ALERT_INTERVAL 500		// ms - minimum time between alert signals - blocking

#define DEFAULT_ALERT_DIST 15	// cm

void displayMsg(const char msg[], uint8_t x, uint8_t y) {
	USI_I2C_start(LCD_I2C_ADDR, I2C_WRITE_MODE);
	USI_I2C_sendByte(0);	// command register
	USI_I2C_sendByte(3);	// position cursor command
	USI_I2C_sendByte(y);	// x pos : first col
	USI_I2C_sendByte(x);	// y pos
	for (uint8_t i = 0; i < strlen(msg); i++) {
		USI_I2C_sendByte(char(msg[i]));
	}
	// clear rest of line with spaces
	//for(uint8_t i = strlen(msg); i < 12; i++) {
	//USI_I2C_sendByte(0x20);	// space
	//}
	USI_I2C_stop();
}

void displayClear() {
	USI_I2C_start(LCD_I2C_ADDR, I2C_WRITE_MODE);
	USI_I2C_sendByte(0);	// command register
	USI_I2C_sendByte(12);
	USI_I2C_stop();
}

void displayInit() {
	USI_I2C_start(LCD_I2C_ADDR, I2C_WRITE_MODE);
	USI_I2C_sendByte(0);	// command register
	USI_I2C_sendByte(4);	// cursor off
	USI_I2C_sendByte(12);	// clear screen, home cursor
	USI_I2C_sendByte(19);	// backlight on
	USI_I2C_stop();
}

uint16_t ping(void) {
	unsigned long elapsed = 0;
	uint16_t distance = 0;
	PORTA |= (1 << TRIG_PIN);				// set trigger pin high
	_delay_us(10);							// wait for min. 10us
	PORTA &= ~(1 << TRIG_PIN);				// take trigger pin low again
	while ((PINA & (1 << ECHO_PIN)) == 0);	// loop until echo pin goes high
	TCNT1 = 0;								// reset timer
	while (PINA & (1 << ECHO_PIN));			// loop until echo pin goes low again
	elapsed = TCNT1;						// read timer
	// at clock speed of 1MHz, elapsed will be number of us for echo time.
	// This should be well within range of 16-bit int (TCNT1 is 16-bit).
	// With higher clock speeds, elapsed is going to be much higher and
	// may need scaling
	elapsed = elapsed / (F_CPU / 1000000.0);	// to scale for clock speed, but here we're diving by 1 !
	if (elapsed <= MAX_ECHO_TIME) {
		distance = (uint16_t)(elapsed / DIST_FACTOR);
	}
	return distance;
}

// MAIN OBJECTS
//SensorBus ssb = SensorBus();

int main(void) {
	// ----------------------------------------------------------------------------------------------------
	// -----  SETUP                                                                                   -----
	// ----------------------------------------------------------------------------------------------------
	DDRA = 0;	// actually defaults to this, but let's be specific
	DDRA |= (1 << TRIG_PIN);		// Trigger pin as output
	DDRA |= (1 << ALERT_PIN);		// Alert pin as output
	PORTA &= ~(1 << TRIG_PIN);		// set trigger pin low
	PORTA |= (1 << ALERT_PIN);		// set alert pin high (it's active low)
	TCCR1B |= (1 << CS10);			// set prescaler to clk/1 (no prescaling)


	//ssb.init();
	//ssb.addChannel(PCINT3);
	USI_I2C_initialiseBus();
	displayInit();
	char display_buf[21];
	//displayMsg("Echo:",1,1);
	//displayMsg("us",13,1);
	displayMsg("Dist:", 1, 2);
	displayMsg("cm", 13, 2);
	uint16_t dist = 0;
	//bool alerted = FALSE;
	while (1) {
		dist = ping();
		if (dist < DEFAULT_ALERT_DIST) {
			PORTA &= ~(1 << ALERT_PIN);		// take alert pin low
			//_delay_us(ALERT_PULSE_TIME);	// make sure we get noticed
			//alerted = TRUE;
			_delay_ms(ALERT_INTERVAL);		// delay to avoid sending too many alerts
			PORTA |= (1 << ALERT_PIN);		// take it high again
		}
		//sprintf(display_buf, "%5u", echoTime);
		//displayMsg(display_buf,7,1);
		sprintf(display_buf, "%3i", dist);
		displayMsg(display_buf, 9, 2);
		_delay_ms(100);
	}
}
