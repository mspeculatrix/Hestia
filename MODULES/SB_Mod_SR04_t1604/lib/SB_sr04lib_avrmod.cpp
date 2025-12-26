#include "SB_sr04lib_avrmod.h"

void enableSensorTimer(void) {
	// Configure TCA0 for normal (count‑up) mode, no PWM
	// The following just sets the default, but for the sake of completeness...
	TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_NORMAL_gc;
	// Optional: ensure no compare channels are enabled
	TCA0.SINGLE.CTRLD = 0;
	// Clear the counter and period registers
	TCA0.SINGLE.CNT = 0;      // reset counter
	TCA0.SINGLE.PER = 0xFFFF; // roll over at max 16‑bit value
	// Enable & clock div
	TCA0.SINGLE.CTRLA = TCA_SINGLE_ENABLE_bm | TCA_SINGLE_CLKSEL_DIV1_gc;
	// TCA0.SINGLE.CTRLA = TCA_SINGLE_ENABLE_bm;
}

uint16_t ping(void) {
	uint16_t distance = 0;
	SENSOR_PORT.OUTSET = TRIGGER;		// set trigger pin high
	_delay_us(10);						// wait for min. 10us
	SENSOR_PORT.OUTCLR = TRIGGER;		// take trigger pin low again
	WAIT_UNTIL_HIGH(SENSOR_PORT, ECHO);
	TCA0.SINGLE.CNT = 0;      			// reset counter
	WAIT_UNTIL_LOW(SENSOR_PORT, ECHO);
	unsigned long elapsed = TCA0.SINGLE.CNT / DIST_FACTOR;
	distance = (uint16_t)elapsed;
	// elapsed = (elapsed * 64) / (F_CPU / 1000000.0);	// to scale for clock speed & prescaler
	// //if (elapsed <= MAX_ECHO_TIME) {
	// distance = (uint16_t)(elapsed / DIST_FACTOR);
	// //}
	return distance;
}
