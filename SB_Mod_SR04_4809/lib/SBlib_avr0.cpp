#include "SBlib_avr0.h"

/***** CLASS METHODS *****/
// SensorBusModule::SensorBusModule(volatile PORT_t* port,
// 	uint8_t datPin_pm, uint8_t clkPin_pm,
// 	volatile uint8_t* datCtrl)
// 	: _port(port), _datPin(datPin_pm), _clkPin(clkPin_pm), _datCtrl(datCtrl) {
// 	init();
// }

// void SensorBusModule::init() {
// 	// Initial configuration
// 	_port->DIRCLR = _datPin | _clkPin; // set as inputs to start with
// 	// base address of PIN0CTRL, then offset to correct pin
// 	volatile uint8_t* pinCtrlBase = &(_port->PIN0CTRL);
// 	pinCtrlBase[*_datCtrl] = PORT_PULLUPEN_bm | PORT_ISC_FALLING_gc;
// }

// // This will be called by methods specific to the instance.
// void SensorBusModule::sendMessage(uint8_t* msgBuf, uint8_t msgLen) {

// }

void sbSetDefaults() {
	// SB_PORT.OUTSET = SB_CLK | SB_DAT | SB_ACT; // all lines to high
	SB_PORT.DIRCLR = SB_CLK | SB_DAT | SB_ACT; // all lines to inputs
	// Enable interrupts on data line
	SB_PORT.SB_DAT_CTRL = PORT_PULLUPEN_bm | PORT_ISC_FALLING_gc;
}

uint8_t sbReceiveByte() {

}

void sbSendMessage(uint8_t* msgBuf, uint8_t msgLen) {
	// Assumes all signals start as inputs
	// Disable interrupts on DAT
	SB_PORT.SB_DAT_CTRL &= ~PORT_ISC_gm;  			// Clear current ISC bits
	SB_PORT.SB_DAT_CTRL |= PORT_ISC_INTDISABLE_gc;  // Disable interrupts

	bool bus_inactive = waitForState(&SB_PORT, SB_ACT, HIGH, 20000, SB_MAX_TO_LOOP_COUNT);

	if (bus_inactive) {
		SB_PORT.OUTCLR = SB_DAT | SB_ACT;				// Set both low
		SB_PORT.DIRSET = SB_DAT | SB_ACT;				// Set both to outputs
		bool clk_strobed = waitForState(&SB_PORT, SB_CLK, LOW, 0xFFFF, 255);
		if (clk_strobed) {
			// serial.writeln("strobe");
			SB_PORT.OUTSET = SB_DAT;				// Take high
			_delay_us(SB_START_TRANSMISSION_PAUSE);
			SB_PORT.OUTSET = SB_CLK;				// Take high
			SB_PORT.DIRSET = SB_CLK;						// & output

			// EXCHANGE LOOP
			for (uint8_t i = 0; i < msgLen; i++) {
				for (uint8_t bit = 0; bit < 8; bit++) {
					if (msgBuf[i] & (1 << bit)) {
						SB_PORT.OUTSET = SB_DAT;
					} else {
						SB_PORT.OUTCLR = SB_DAT;
					}
					_delay_us(SB_BIT_PAUSE);
					SB_PORT.OUTCLR = SB_CLK;
					_delay_us(SB_BIT_PAUSE);
					SB_PORT.OUTSET = SB_CLK;
					// _delay_us(SB_BIT_PAUSE);
				}
				_delay_us(SB_BYTE_PAUSE);
			}
		} else {
			serial.writeln("* No clk strobe");
		}
	} else {
		serial.writeln("* Timed out waiting for bus to be inactive");
	}

	// RESET TO DEFAULTS
	sbSetDefaults();
}

void timeoutCounterInit() {
	TCB0.CTRLA = 0;	// Disable timer before configuration
	TCB0.CTRLA = TCB_CLKSEL_CLKDIV2_gc;   // Select clock source
	TCB0.CTRLB = TCB_CNTMODE_INT_gc;      // periodic interrupt mode but interrupts disabled
	TCB0.INTFLAGS = TCB_CAPT_bm;	// Clear any pending interrupt flag
}

void timeoutCounterStart(uint16_t timeoutValue) {
	TCB0.CNT = 0;     // Reset count
	TCB0.CCMP = timeoutValue;			// Load rollover value
	TCB0.CTRLA |= TCB_ENABLE_bm; 	// Enable the timer
}

void timeoutCounterStop() {
	TCB0.INTFLAGS = TCB_CAPT_bm;
	TCB0.CTRLA &= ~TCB_ENABLE_bm;
}

bool waitForState(volatile PORT_t* port, uint8_t pin, uint8_t state, uint16_t timeoutTicks, uint8_t max_loops) {
	bool stateAchieved = false;
	bool max_loop_limit = false;
	uint8_t loop_count = 0;
	timeoutCounterStart(timeoutTicks);
	while (!stateAchieved && !max_loop_limit) {
		bool timed_out = false;
		while (!stateAchieved && !timed_out) {
			uint8_t pinState = port->IN & pin;
			if ((state == HIGH && pinState) || (state == LOW && !pinState)) {
				stateAchieved = true;
			} else if (TCB0.INTFLAGS & TCB_CAPT_bm) {	// overflow happened
				TCB0.INTFLAGS = TCB_CAPT_bm;			// clear flag
				timed_out = true;
			}
		}
		if (!stateAchieved) {
			if (loop_count >= max_loops) {
				max_loop_limit = true;
			} else {
				loop_count++;
			}
		}
	}
	timeoutCounterStop();
	return stateAchieved;
}