# SB_MOD_SR04_t1604

Firmware for the SR04 ultrasonic rangefinder sensor module using SensorBus.

This is based around the ATtiny1604 microcontroller.

The code makes use of all three of the ATtiny1604's timers.

- TCA - used by the sensor code (`SB_sr04lib_avrmod`) to time the echoes from pings.
- TCB - used by the SensorBus library (`SBlib_avrmod`) for signal timeouts.
- RTC/PIT - creates an interrupt roughly once per second to tell the module to perform a measurement (ie, a ping).

## NOTES

Min reliable: 3860 = c.2cm
Max: c.6500 = 560cm

SB_working & Session 1:

- #define SB_START_TRANSMISSION_PAUSE 500
- #define SB_BIT_PAUSE 50
- #define SB_BYTE_PAUSE 50
- CLK_STROBE_TIME = 10000

SB_working_updated_Python & Session 2:

- With updated Python that can read variable length messages.

BUT: Usually crashes after a number of cycles, so don't know what that's about.
