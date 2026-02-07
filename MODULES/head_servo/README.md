# HEAD_SERVOS

SensorBus servo module for controlling the pan and tilt servos on the sensor head.

The tilt sensor has only about a 90-degree range, so the constructor values reflect this.

SensorBus messages should have the following format:

| Byte | Value                             |
|:----:|-----------------------------------|
| 0    | 6                                 |
| 1    | SBMSG_SET_PARAM                   |
| 2    | SB_Servo::SB_SERVO_A - PAN servo  |
| 3    | Angle for Servo A - 0-180         |
| 2    | SB_Servo::SB_SERVO_B - TILT servo |
| 3    | Angle for Servo B - 0-180         |

## ATtiny1604 Pinout

| FUNC   | MCU PIN | IC PIN | - | IC PIN | MCU PIN | FUNC   |
|:------:|:-------:|:------:|:-:|:------:|:-------:|:------:|
| VCC    | VCC     | 1      | - | 14     | GND     | GND    |
|        | PA4     | 2      | - | 13     | PA3     | SERVO  |
| SB_CLK | PA5     | 3      | - | 12     | PA2     | SB_ACT |
|        | PA6     | 4      | - | 11     | PA1     | SB_DAT |
|        | PA7     | 5      | - | 10     | PA0     | UPDI   |
| TX     | PB3     | 6      | - | 9      | PB0     |        |
| RX     | PB2     | 7      | - | 8      | PB1     |        |
