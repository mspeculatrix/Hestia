# SENSORBUS

Each smart sensor or subsystem will be attached to a common serial bus.

This should not be a UART-based thing because we don't want UART buffers on a device filling up with stuff not intended for that device. Instead we'll bit-bang our own protocol.

The communication doesn't need to be async because we'll only be pushing data in one direction at a time.

This version uses three signals:

- SB_CLK - shared by all modules
- SB_ACT - shared by all modules
- SB_DAT - unique line from module to controller

Header pinout
![Header](img/SensorBus_Header.png "Header")

```
 IDC HEADER               CABLE
 +-----------------+      1. GND
 | •1 GND   +5V 2• |      2. +5V
   •3 CLK   GND 4• |      3. CLK
   •5 DAT  /ACT 6• |      4. GND
 | •7 +5V   GND 8• |      5. DAT
 +-----------------+      6. /ACT
                          7. +5V
                          8. GND
```

## MESSAGE FORMAT

Messages will consist of bytes:

0. MSG_DATA_LEN - how many bytes of data in total, including this one
1. MSG_TYPE - set param, alert, data
2. Data
3. ...

## SIGNALS

Default states (pullups on all lines):

| SIGNAL  | NODE       | MODULE       |           |
|---------|------------|--------------|-----------|
| SB_ACT  | INPUT, int | INPUT        | Shared    |
| SB_CLK  | INPUT      | INPUT        | Shared    |
| SB_DAT  | INPUT      | INPUT, int   | Dedicated |

All lines have pullups.

**NB:** The shared lines are set to inputs by default so that we don't have multiple devices driving the lines high.

## SEND MESSAGE

| SENDER                           | RECEIVER                      |
|----------------------------------|-------------------------------|
| Wait for `SB_ACT` to be high     |                               |
| Disable `SB_DAT` interrupts      |                               |
| Set `SB_CLK` to OUTPUT, HIGH     |                               |
| Set `DAT` to OUTPUT, HIGH        |                               |
| Pulse `DAT` LOW                  | Go into receive mode:         |
| Set `DAT` to INPUT               | -Disable `SB_DAT` interrupts  |
| Wait for `DAT` to go LOW         | -Set `SB_ACT` OUTPUT, HIGH    |
|                                  | -Set `DAT` OUTPUT, HIGH       |
|                                  | -Take `SB_ACT` LOW            |
|                                  | -Strobe `DAT` LOW             |
|                                  | -Set `DAT` to INPUT           |
| <-- START_TRANSMISSION_PAUSE --> |                               |
| <-- START EXCHANGE LOOP -->      |                               |
| For each Byte:                   |                               |
| - For each bit in byte (8 times) |                               |
|   -- Set bit value on `SB_DAT`   |                               |
|   -- BIT_PAUSE                   | Wait for `SB_CLK` to go LOW   |
|   -- Take `SB_CLK` LOW           | Read bit                      |
|   -- BIT_PAUSE                   | Wait for `SB_CLK` to go HIGH  |
|   -- Take `SB_CLK` HIGH          | Save byte                     |
| - BYTE_PAUSE                     |                               |
| <-- END LOOP -->                 |                               |
| <-- reset to defaults -->        | <-- reset to defaults -->     |
| Set `SB_DAT` to INPUT            |                               |
| Set `SB_CLK` to INPUT            |                               |
| Enable `SB_DAT` interrupts       |                               |
