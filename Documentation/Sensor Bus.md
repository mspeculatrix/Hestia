# SENSOR BUS

Each smart sensor or subsystem will be attached to a common serial bus.

This should not be a UART-based thing because we don't want UART buffers on a device filling up with stuff not intended for that device. Instead we'll bit-bang our own protocol.

The communication doesn't need to be async because we'll only be pushing data in one direction at a time.

## MESSAGE FORMAT

Messages will consist of:

0. MSG_DATA_LEN - how many bytes of data
1. MSG_TYPE - set param, alert, data
2. Data
3. ...

## SIGNALS

| SIGNAL  | Controlled by | HUB                | CLIENT          |
|---------|:-------------:|--------------------|-----------------|
| SB_CLK  | bidirectional | def: INPUT, pullup | def: INPUT      |
| /SB_INT | clients       | INPUT, int, pullup | OUTPUT, diode   |
| /SB_ACT | hub           | OUTPUT, pullup     | INPUT           |
| SB_DAT  | bidirectional | def: INPUT, int    | def: INPUT, int |

First three signals are all shared by all clients.

SB_DAT is a dedicated line from each subsystem to hub.

**NB:** The shared lines are set to inputs by default so that we don't have multiple devices driving the lines high. The `/SB_INT`  and `/SB_ACT` lines will each need one global pullup.

## MESSAGE: HUB → CLIENT

| HUB                           | CLIENT |
|-------------------------------|-|
| Set `/SB_ACT` OUPUT, LOW      | |
| Disable `/SB_INT` interrupts  | |
| Set `SB_CLK` to OUTPUT, HIGH  | |
| Set `SB_DAT` to OUTPUT, HIGH | |
| Pulse `SB_DAT` LOW           | |
|                               | Go into receive mode:    |
| Wait for pulse on `/SB_INT`   | Disable `SB_DAT` interrupts |
|                               | Pulse `/SB_INT` LOW          |
| <-- message exchange -->      | <-- message exchange -->   |
| <-- reset to default -->      | <-- reset to default -->   |
| Enable `/SB_INT` interrupts   | |

New version:

| HUB                            | CLIENT |
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

## MESSAGE: CLIENT → HUB

| HUB                          | CLIENT |
|------------------------------|-|
|                              | Check `/SB_ACT`: proceed if HIGH |
|                              | Set `SB_CLK` to OUTPUT, HIGH     |
|                              | Set `SB_DAT` to OUTPUT, LOW      |
|                              | Pulse `/SB_INT`                  |
| Disable `/SB_INT` interrupts | |
| Identify device              | (Wait for `/SB_ACT` to go LOW)   |
| If found, take `/SB_ACT` LOW | |
| Wait for `SB_DAT` to go HIGH | Set `SB_DAT` HIGH                |
|                              | Brief pause                      |
| <-- message exchange -->     | <-- message exchange -->         |
| <-- reset to default -->     | <-- reset to default -->         |

New version:

|    | HUB                             | CLIENT |
|---:|---------------------------------|-|
|  1 |                                 | Check if `SB_CLK` LOW. If so, wait |
|  2 |                                 | Disable `SB_DAT` interrupts        |
|  3 |                                 | Set `SB_DAT` to OUTPUT, HIGH       |
|  4 |                                 | Pulse `SB_DAT` LOW                 |
|  5 | If not busy:                    | Set `SB_DAT` to INPUT              |
|  6 | Disable all `SB_DAT` interrupts | |
|  7 | Identify device, if found:      | Wait for `SB_DAT` to pulse LOW     |
|  8 | Set `SB_DAT` OUTPUT, HIGH       | |
|  9 | Pulse `SB_DAT` LOW              | |
| 10 | Set `SB_DAT` to INPUT           | Set `SB_DAT` OUTPUT, HIGH          |
| 11 |                                 | Set `SB_CLK` OUTPUT, HIGH          |
| 12 | <-- message exchange -->        | <-- message exchange -->           |
| 13 | <-- reset to default -->        | <-- reset to default -->           |

Step 1 - the client checking if `SB_CLK` is low - is just a quick and dirty check to ensure the bus isn't in use.

Step 7 - this needs a timeout on the client's part. If it doesn't see an `SB_DAT` pulse within a given time it starts again from step 1. Makes a fixed number of overall attempts before giving up. That number might depend on the urgency of the message.

## MESSAGE EXCHANGE

Starts with sender setting `SB_DAT` and `SB_CLK` to HIGH.

| SENDER                               | RECEIVER                      |
|--------------------------------------|-------------------------------|
|                                      | Watch for `SB_CLK` to go LOW  |
| Set `SB_CLK` LOW while `SB_DAT` HIGH | |
|                                      | Watch for `SB_DAT` to go LOW  |
|                                      | Check `SB_CLK` is still LOW   |
| START_TRANSMISSION_PAUSE             | |
| Set `SB_DAT` LOW while `SB_CLK` LOW  | |
| Take `SB_DAT` & `SB_CLK` HIGH        | Watch for `SB_DAT` to go HIGH |
| <-- START EXCHANGE LOOP -->          | |
| For each Byte:                       | If `/SB_ACT` HIGH, done       |
| - For each bit in byte:              | |
|   -- Set bit value on `SB_DAT`       | |
|   -- BIT_PAUSE                       | Wait for `SB_CLK` to go LOW   |
|   -- Take `SB_CLK` LOW               | Read bit |
|   -- BIT_PAUSE                       | Wait for SB_CLK to go HIGH    |
|   -- Take `SB_CLK` HIGH              | |
| - BYTE_PAUSE                         | |
| <-- END LOOP -->                     | |
| Set `/SB_ACT` HIGH                   | |

New version:

|    | SENDER                               | RECEIVER                      |
|---:|--------------------------------------|-------------------------------|
|  1 | <-- START_TRANSMISSION_PAUSE -->     | |
|  2 | <-- START EXCHANGE LOOP -->          | |
|  3 | For each Byte:                       | |
|  4 | - For each bit in byte (8 times)     | |
|  5 |   -- Set bit value on `SB_DAT`       | |
|  6 |   -- BIT_PAUSE                       | Wait for `SB_CLK` to go LOW   |
|  7 |   -- Take `SB_CLK` LOW               | Read bit                      |
|  8 |   -- BIT_PAUSE                       | Wait for SB_CLK to go HIGH    |
|  9 |   -- Take `SB_CLK` HIGH              | Save byte                     |
| 10 | - BYTE_PAUSE                         | |
| 11 | <-- END LOOP -->                     | |

The first byte sent gives the length of the message (not including that byte).
