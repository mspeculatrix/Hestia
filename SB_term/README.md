# SB_term

A Python-based 'terminal' for communicating with Sensor Bus devices.

Runs on a Raspberry Pi.

For experimentation and development.

| BCM |   Name  | Physical | Name    | BCM |
|:---:|:-------:|:--------:|:-------:|:---:|
|     | --3V3-- |  1 -- 2  | --5V--  |     |
|   2 |   SDA 1 |  3 -- 4  | --5V--  |     |
|   3 |   SCL 1 |  5 -- 6  | --GND-- |     |
|   4 | GPIO  7 |  7 -- 8  | TxD     | 14  |
|     | --GND-- |  9 -- 10 | RxD     | 15  |
|  17 | GPIO 0  | 11 -- 12 | GPIO 1  | 18  |
|  27 | GPIO 2  | 13 -- 14 | --GND-- |     |
|  22 | GPIO 3  | 15 -- 16 | GPIO 4  | 23  |
|     | --3V3-- | 17 -- 18 | GPIO 5  | 24  |
|  10 |    MOSI | 19 -- 20 | --GND-- |     |
|   9 |    MISO | 21 -- 22 | GPIO 6  | 25  |
|  11 |    SCLK | 23 -- 24 | CE0     | 8   |
|     | --GND-- | 25 -- 26 | CE1     | 7   |
|   0 |   SDA 0 | 27 -- 28 | SCL 0   | 1   |
|   5 | GPIO 21 | 29 -- 30 | --GND-- |     |
|   6 | GPIO 22 | 31 -- 32 | GPIO 26 | 12  |
|  13 | GPIO 23 | 33 -- 34 | --GND-- |     |
|  19 | GPIO 24 | 35 -- 36 | GPIO 27 | 16  |
|  26 | GPIO 25 | 37 -- 38 | GPIO 28 | 20  |
|     | --GND-- | 39 -- 40 | GPIO 29 | 21  |
