# HEAD_NODE

A SensorBus Node to control the head unit for the Hestia robot.

Connects to a head_servo module.

## PIN ASSIGNMENTS

| MCU pin | BOARD pin | Description                |
|:-------:|:---------:|----------------------------|
| PD0     | 2         | ACT pin SERVO module       |
| PD1-PD7 | 3-9       | ACT pins for other modules |
| PE0     | 11        | Common CLK pin             |
| PE1     | 12        | Common DAT pin             |
| UPDI    | 23        | Programming                |
| PA0     | 26        | TX                         |
| PA1     | 27        | RX                         |
