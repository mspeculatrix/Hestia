# SB_NODE_DEV

An experimental sensor node for playing around with and developing SensorBus code.

This version is based around the ATmega4809A. I could foresee others being based on the STM32.

This is designed to work with up to 8 modules whose DAT lines are connected
to Port D.

## PIN ASSIGNMENTS

| MCU pin | DIP pin | Description          |
|:-------:|:-------:|----------------------|
| PD0-PD7 | 2-9     | ACT pins for modules |
| PE0     | 11      | Common CLK pin       |
| PE1     | 12      | Common DAT pin       |
| UPDI    | 23      | Programming          |
| PA0     | 26      | TX                   |
| PA1     | 27      | RX                   |
