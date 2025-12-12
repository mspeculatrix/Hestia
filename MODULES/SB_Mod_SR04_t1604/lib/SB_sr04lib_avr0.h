/* Library for SR04 ultrasonic rangefinder */

#ifndef __SMD_SR04LIB__
#define __SMD_SR04LIB__

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
#include "SBlib_defines.h"
#include "SBlib_avr0.h"

#define MSG_BUF_LEN 16

#define MAX_ECHO_TIME 23200
#define DIST_FACTOR 100

void enableSensorTimer(void);
uint16_t ping(void);

#endif
