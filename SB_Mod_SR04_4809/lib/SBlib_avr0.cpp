#include "SBlib_avr0.h"

/***** CLASS METHODS *****/
SensorBusModule::SensorBusModule(volatile PORT_t* port,
	uint8_t datPin_pm, uint8_t clkPin_pm,
	volatile uint8_t* datCtrl)
	: _port(port), _datPin(datPin_pm), _clkPin(clkPin_pm), _datCtrl(datCtrl) {
	init();
}

void SensorBusModule::init() {
	// Initial configuration
	_port->DIRCLR = _datPin | _clkPin; // set as inputs to start with
	// base address of PIN0CTRL, then offset to correct pin
	volatile uint8_t* pinCtrlBase = &(_port->PIN0CTRL);
	pinCtrlBase[*_datCtrl] = PORT_PULLUPEN_bm | PORT_ISC_FALLING_gc;
}

// This will be called by methods specific to the instance.
void SensorBusModule::sendMessage(uint8_t* msgBuf, uint8_t msgLen) {

}

/*
|    | CONTROLLER                      | MODULE |
|---:|---------------------------------|-|
|  1 |                                 | Check if `SB_CLK` LOW. If so, wait |
|  2 |                                 | Disable `SB_DAT` interrupts        |
|  3 |                                 | Set `SB_DAT` to OUTPUT, HIGH       |
|  4 |                                 | Pulse `SB_DAT` LOW                 |
|  5 | If not busy:                    | Set `SB_DAT` to INPUT              |
|  6 | Disable all `SB_DAT` interrupts | |
|  7 | Identify device, if found:      | Wait for `SB_DAT` to pulse LOW     |
|  8 | Set `SB_DAT` OUTPUT, HIGH       | |
|  9 | Pulse `SB_DAT` LOW              | |
| 10 | Set `SB_DAT` to INPUT           | Set `SB_DAT` OUTPUT, HIGH          |
| 11 |                                 | Set `SB_CLK` OUTPUT, HIGH          |
| 12 | <-- message exchange -->        | <-- message exchange -->           |
| 13 | <-- reset to default -->        | <-- reset to default -->           |
*/

void sbSetDefaults() {
	SB_PORT.OUTSET = SB_CLK | SB_DAT; // both lines to high
	SB_PORT.DIRCLR = SB_CLK | SB_DAT; // both lines to inputs
	// Enable interrupts on data line
	SB_PORT.SB_DAT_CTRL = PORT_PULLUPEN_bm | PORT_ISC_FALLING_gc;
}

void strobeDataLine() {
	SB_PORT.OUTSET = SB_DAT;						// Set to high
	SB_PORT.DIRSET = SB_DAT;						// Set DAT to output
	SB_PORT.OUTCLR = SB_DAT;						// Pulse DAT low
	_delay_us(SB_PULSE_LENGTH);
	// SB_PORT.OUTSET = SB_DAT;						// Set to high
	// SB_PORT.DIRCLR = SB_DAT;						// Set DAT to input
}

void sbSendMessage(uint8_t* msgBuf, uint8_t msgLen) {
	// while (!(SB_PORT.IN & SB_CLK));					// if CLK low, wait.

	// Disable interrupts on DAT
	SB_PORT.SB_DAT_CTRL &= ~PORT_ISC_gm;  			// Clear current ISC bits
	SB_PORT.SB_DAT_CTRL |= PORT_ISC_INTDISABLE_gc;  // Disable interrupts
	SB_PORT.OUTSET = SB_DAT;						// Set to high
	SB_PORT.DIRSET = SB_DAT;						// Set DAT to output
	SB_PORT.OUTCLR = SB_DAT;						// Take DAT low
	while (SB_PORT.IN & SB_CLK); // Wait for CLK to go low
	// strobeDataLine();
	SB_PORT.OUTSET = SB_DAT;
	//while (SB_PORT.IN & SB_DAT) {} 					// wait for DAT to go low
	SB_PORT.DIRSET = SB_CLK;				// Set to output
	SB_PORT.OUTSET = SB_CLK;				// Set to high

	// MESSAGE EXCHANGE
	/*

|    | SENDER                               | RECEIVER                      |
|---:|--------------------------------------|-------------------------------|
|  1 | <-- START_TRANSMISSION_PAUSE -->     | |
|  2 | <-- START EXCHANGE LOOP -->          | |
|  3 | For each Byte:                       | |
|  4 | - For each bit in byte (8 times)     | |
|  5 |   -- Set bit value on `SB_DAT`       | |
|  6 |   -- BIT_PAUSE                       | Wait for `SB_CLK` to go LOW   |
|  7 |   -- Take `SB_CLK` LOW               | Read bit                      |
|  8 |   -- BIT_PAUSE                       | Wait for `SB_CLK` to go HIGH  |
|  9 |   -- Take `SB_CLK` HIGH              | Save byte                     |
| 10 | - BYTE_PAUSE                         | |
| 11 | <-- END LOOP -->                     | |

	*/
	// _delay_us(START_TRANSMISSION_PAUSE);
	// // ECHANGE LOOP
	// for (uint8_t i = 0; i < msgLen; i++) {
	// 	for (uint8_t bit = 0; bit < 8; bit++) {
	// 		if (msgBuf[i] & (1 << bit)) {
	// 			SB_PORT.OUTSET = SB_DAT;
	// 		} else {
	// 			SB_PORT.OUTCLR = SB_DAT;
	// 		}
	// 		_delay_us(BIT_PAUSE);
	// 		SB_PORT.OUTCLR = SB_CLK;
	// 		_delay_us(BIT_PAUSE);
	// 		SB_PORT.OUTSET = SB_CLK;
	// 	}
	// 	_delay_us(BYTE_PAUSE);
	// }

	// RESET TO DEFAULTS
	sbSetDefaults();
}
