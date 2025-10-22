#ifndef __SMD_SBLIB_4809__
#define __SMD_SBLIB_4809__

#ifndef __AVR_ATmega4809__
#define __AVR_ATmega4809__
#endif

// Ensure we have io/sfr/pindefs loaded
#ifndef   _AVR_IO_H_
#include  <avr/io.h>
#endif

#include <avr/interrupt.h>
#include <util/delay.h>

#include "app_defines.h"
#include "smd_avr0_serial.h"
#include "SBlib_defines.h"

extern SMD_AVR0_Serial serial;

void sbSendMessage(uint8_t* msgBuf, uint8_t msgLen);
void strobeDataLine();
void sbSetDefaults();

class SensorBusModule {
public:
	SensorBusModule(volatile PORT_t* port,
		uint8_t datPin_pm, uint8_t clkPin_pm,
		volatile uint8_t* datCtrl);
	void init();
	void sendMessage(uint8_t* msgBuf, uint8_t msgLen);

protected:
	volatile PORT_t* _port;
	uint8_t _datPin;
	uint8_t _clkPin;
	volatile uint8_t* _datCtrl;

};

#endif