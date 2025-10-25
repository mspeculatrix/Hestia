# SENSOR BUS

Each smart sensor or subsystem will be attached to a common serial bus.

This should not be a UART-based thing because we don't want UART buffers on a device filling up with stuff not intended for that device. Instead we'll bit-bang our own protocol.

The communication doesn't need to be async because we'll only be pushing data in one direction at a time.

This version uses three signals:

- SB_CLK - shared by all modules
- SB_ACT - shared by all modules
- SB_DAT - unique line from module to controller

## MESSAGE FORMAT

Messages will consist of bytes:

0. MSG_DATA_LEN - how many bytes of data in total
1. MSG_TYPE - set param, alert, data
2. Data
3. ...

## SIGNALS

Default states (pullups on all lines):

| SIGNAL  | CONTROLLER | MODULE       |           |
|---------|------------|--------------|-----------|
| SB_ACT  | INPUT, int | INPUT        | Shared    |
| SB_CLK  | INPUT      | INPUT        | Shared    |
| SB_DAT  | INPUT      | INPUT, int   | Dedicated |

All lines have pullups.

**NB:** The shared lines are set to inputs by default so that we don't have multiple devices driving the lines high.

## MESSAGE: CONTROLLER → MODULE

| CONTROLLER                     | MODULE |
|--------------------------------|-|
| Disable `SB_DAT` interrupts    | |
| Set `SB_CLK` to OUTPUT, HIGH   | |
| Set `SB_DAT` to OUTPUT, HIGH   | |
| Pulse `SB_DAT` LOW             | |
| Set `SB_DAT` to INPUT          | Go into receive mode:    |
| Wait for LOW pulse on `SB_DAT` | Disable `SB_DAT` interrupts |
|                                | Set `SB_DAT` to OUTPUT, HIGH |
|                                | Pulse `SB_DAT` LOW |
|                                | Set `SB_DAT` to INPUT |
| <-- message exchange -->       | <-- message exchange -->   |
| <-- reset to default -->       | <-- reset to default -->   |
| Set `SB_DAT` to INPUT          | |
| Set `SB_CLK` to INPUT          | |
| Enable `SB_DAT` interrupts     | |

## MESSAGE: MODULE → CONTROLLER

|    | CONTROLLER                      | MODULE |
|---:|---------------------------------|-|
|  1 |                                 | Disable `SB_DAT` interrupts        |
|  2 |                                 | Check if `SB_ACT` LOW. If so, wait |
|  4 |                                 | Set `SB_DAT` to OUTPUT, LOW        |
|    |                                 | Set `SB_ACT` to OUTPUT |
|  4 |                                 | Set `SB_ACT` LOW                 |
|| Could be a moment here where another sensor fires ||
|| Somewhere in the following, we could have the module ||
|| switch DAT to input & watch for a strobe ||
|  5 | If not busy:                    | |
|  6 | Disable `SB_ACT` interrupt      | |
|    |       | |
|  7 | Identify device, if found:      | Wait for `SB_CLK` to pulse LOW     |
|  8 | Set `SB_CLK` OUTPUT, HIGH       | |
|  9 | Pulse `SB_CLK` LOW              | |
| 10 | Set `SB_CLK` to INPUT           | Set `SB_DAT` HIGH          |
| 11 |                                 | Set `SB_CLK` OUTPUT, HIGH          |
| 12 | <-- message exchange -->        | <-- message exchange -->           |
| 13 | <-- reset to default -->        | Set `SB_ACT` HIGH then INPUT       |

Step 1 - the client checking if `SB_CLK` is low - is just a quick and dirty check to ensure the bus isn't in use.

Step 7 - this needs a timeout on the client's part. If it doesn't see an `SB_DAT` pulse within a given time it starts again from step 1. Makes a fixed number of overall attempts before giving up. That number might depend on the urgency of the message.

## MESSAGE EXCHANGE

Starts with sender setting `SB_DAT` and `SB_CLK` to HIGH.

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
