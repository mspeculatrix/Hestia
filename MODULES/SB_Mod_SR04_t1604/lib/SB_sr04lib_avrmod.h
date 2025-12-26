/*
 * Library for SR04 ultrasonic rangefinder
 * For use with Modern ATmega and ATtiny microcontrollers (0-, 1- and 2-Series).
*/

#ifndef __SMD_SR04LIB_AVRMOD__
#define __SMD_SR04LIB_AVRMOD__

// Ensure we have io/sfr/pindefs loaded
#ifndef   _AVR_IO_H_
#include  <avr/io.h>
#endif

#include <avr/interrupt.h>
#include <util/delay.h>
#include "app_defines.h"
#include "SBlib_defines.h"
#include "SBlib_avrmod.h"

#define MSG_BUF_LEN 16

#define MAX_ECHO_TIME 23200
#define DIST_FACTOR 100

void enableSensorTimer(void);
uint16_t ping(void);

#endif
