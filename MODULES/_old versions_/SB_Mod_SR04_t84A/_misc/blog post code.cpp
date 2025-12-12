/*
* HC-SR04.cpp
*/
#ifndef F_CPU
#define F_CPU 1000000UL
#endif

#include <avr/io.h>;
#include <util/delay.h>;

#define TRIG_PIN PA1
#define ECHO_PIN PA2
#define MAX_ECHO_TIME 23200 // 23200us is 400cm max range.
#define DIST_FACTOR 58

uint16_t ping(void) {
	unsigned long elapsed = 0;
	PORTA |= (1 << TRIG_PIN);       // set trigger pin high
	_delay_us(10);                  // wait for minimum of 10us
	PORTA &= ~(1 << TRIG_PIN);      // take trigger pin low again
	while ((PINA & (1 << ECHO_PIN)) == 0); // loop until echo pin goes high
	TCNT1 = 0;                      // reset timer
	while (PINA & (1 << ECHO_PIN)); // loop until echo pin goes low again
	elapsed = TCNT1; // read timer
	elapsed = elapsed / (F_CPU / 1000000.0); // to scale for clock speed
	if (elapsed > MAX_ECHO_TIME) elapsed = 0;
	return (uint16_t)elapsed;
}

int main(void) {
	// -----------------
	// ----- SETUP -----
	// -----------------
	DDRA = 0; // defaults to this, but let's be specific. Also sets Echo pin as input.
	DDRA = (1 << TRIG_PIN);       // set as output
	PORTA &= ~(1 << TRIG_PIN);    // set trigger pin low
	TCCR1B |= (1 << CS10);  // set prescaler to clk/1 (no prescaling)

	uint16_t echoTime = 0;

	// ---------------------
	// ----- MAIN LOOP -----
	// ---------------------
	while (1) {
		echoTime = ping();
		_delay_ms(100);
	}
}