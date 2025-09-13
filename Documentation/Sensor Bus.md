# SENSOR BUS

Each smart sensor or subsystem will be attached to a common serial bus.

This should not be a UART-based thing because we don't want UART buffers on a device filling up with stuff not intended for that device. Instead we'll bit-bang our own protocol.

The communication doesn't need to be async because we'll only be pushing data in one direction at a time.

## MESSAGE FORMAT

Messages will consist of a fixed length. Bytes are:

1. MSG TYPE - set param, alert, data
2. Data
3. Data

## SIGNALS

| SIGNAL  | Controlled by | HUB                | CLIENT          |
|---------|:-------------:|--------------------|-----------------|
| SB_CLK  | bidirectional | def: INPUT         | def: INPUT      |
| /SB_INT | clients       | INPUT, int, pullup | OUTPUT, diode   |
| /SB_ACT | hub           | OUTPUT, pullup     | INPUT           |
| SB_DAT  | bidirectional | def: INPUT         | def: INPUT, int |

First three signals are all shared by all clients.

SB_DAT is a dedicated line from each subsystem to hub.

**NB:** The shared lines are set to inputs by default so that we don't have multiple devices driving the lines high. The `/SB_INT`  and `/SB_ACT` lines will each need one global pullup.

## MESSAGE: HUB → CLIENT

| HUB                           | CLIENT |
|-------------------------------|-|
| Set `/SB_ACT` OUPUT, LOW      | |
| Disable `/SB_INT` interrupts  | |
| Set `SB_CLK` to OUTPUT, HIGH  | |
| Set `/SB_DAT` to OUTPUT, HIGH | |
| Pulse `/SB_DAT` LOW           | |
|                               | Go into receive mode:    |
| Wait for pulse on `/SB_INT`   | Disable `/SB_DAT` interrupts |
|                               | Pulse `/SB_INT` LOW          |
| <-- message exchange -->      | <-- message exchange -->   |
| <-- reset to default -->      | <-- reset to default -->   |
| Enable `/SB_INT` interrupts   | |

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
