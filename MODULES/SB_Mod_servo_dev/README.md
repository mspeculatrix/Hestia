# SB_SERVO_DEV

General SensorBus servo module for development & general mucking about.

Based around the ATtiny1604.

## ATtiny1604 Pinout

| FUNC  | MCU PIN | IC PIN | - | IC PIN | MCU PIN | FUNC   |
|:-----:|:-------:|:------:|:-:|:------:|:-------:|:------:|
| VCC   | VCC     | 1      | - | 14     | GND     | GND    |
|       | PA4     | 2      | - | 13     | PA3     | SB_CLK |
| SERVO | PA5     | 3      | - | 12     | PA2     | SB_ACT |
|       | PA6     | 4      | - | 11     | PA1     | SB_DAT |
|       | PA7     | 5      | - | 10     | PA0     | UPDI   |
| TX    | PB3     | 6      | - | 9      | PB0     |        |
| RX    | PB2     | 7      | - | 8      | PB1     |        |
