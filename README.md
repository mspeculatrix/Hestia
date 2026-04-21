# HESTIA

Home robot project. A work in progress.

Hestia uses a distributed computing architecture. This consists of:

- **The Mainframe**: sounds impressive but it's probably going to be a Raspberry Pi or something similar. This does high-level stuff. It talks to...
- **Nodes**: These are microcontroller-based boards that perform specific functions, such as motor control, sensing etc. They talk to the Mainframe but also may also connect to several modules, most likely via SensorBus.
- **Modules**: These are semi-autonomous boards, each performing a single task, such as sensing, and communicating important information to (and receiving commands from) Nodes.

## CURRENT NODES

No specific Nodes have been designed so far. I've using an ATmega4809-based dev board for experimenting.

## CURRENT MODULES

- **sr04-1**: Ultrasonic rangefinder using the SensorBus HC-SR04 module board. Based on the ATtiny1604.
- **head_servo**: Uses the SensorBus servo module board to control two servos for pan & tilt operations on Hestia's sensor head. Based on the ATtiny1604.

## GIT BRANCHES

**Main**: Everything in this branch should be functional, but not necessarily complete. Basically, I use this branch as a 'safe' fallback if things go tits-up in the `dev` branch.

**dev**: This is where most of the work takes place. It always has the most up-to-date version of the project, but nothing is guaranteed to work. If you want to follow the project, this is where you should hang out.

**experimental**: For just trying stuff out, particularly stuff that I think is likely to fail. Anything in this branch should be considered temporary at best.
