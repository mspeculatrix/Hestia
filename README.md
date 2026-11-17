# HESTIA

A home robot project with the emphasis on sensors. The idea is that Hestia will autonomously patrol the home, keeping an eye on things. But it's really a way of playing with sensors.

I'm writing about this project in a series of articles for the Machina Speculatrix publication on Medium. The [Hestia home page](https://medium.com/machina-speculatrix/hestia-the-observant-robot-an-introduction-4c6f7eecf5d9) is open access although the articles themselves require a Medium subscription.

**BEWARE**: This is a work in (slow) progress. There are many parts of this repo that are incomplete and disfunctional.

## HARDWARE

The main chassis is a DOIT T300 tank kit. But most of the motion-related software would be usable with any two-motor design. The two 9V brushed motors are driven by an off-the-shelf dual H-bridge board based around the L298 controller IC.

## COMPUTE

Hestia uses a distributed computing architecture. This consists of:

- **The Mainframe**: sounds impressive but it's probably going to be a Raspberry Pi or something similar. This does high-level stuff. It talks to...
- **Nodes**: These are microcontroller-based boards that perform specific functions, such as motor control, sensing etc. They talk to the Mainframe but also may also connect to several modules, most likely via SensorBus.
- **Modules**: These are semi-autonomous boards, each performing a single task, such as sensing, and communicating important information to (and receiving commands from) Nodes.

## CURRENT NODES

So far we just have the motor controller board based on an RPi Pico. This has [its own repository](https://github.com/mspeculatrix/Hestia_Pico_Motor_Controller/tree/main).

## CURRENT MODULES

- **sr04-1**: Ultrasonic rangefinder using the SensorBus HC-SR04 module board. Based on the ATtiny1604.
- **head_servo**: Uses the SensorBus servo module board to control two servos for pan & tilt operations on Hestia's sensor head. Based on the ATtiny1604.

## GIT BRANCHES

**Main**: Everything in this branch should be functional, but not necessarily complete. Basically, I use this branch as a 'safe' fallback if things go tits-up in the `dev` branch.

**dev**: This is where most of the work takes place. It always has the most up-to-date version of the project, but nothing is guaranteed to work. If you want to follow the project, this is where you should hang out.

**experimental**: For just trying stuff out, particularly stuff that I think is likely to fail. Anything in this branch should be considered temporary at best.

## OTHER REPOS FOR THIS PROJECT

- [Hestia Webserver](https://github.com/mspeculatrix/Hestia_Webserver) - a command and control server designed to be run off-robot.
- [Hestia Pico Motor Controller](https://github.com/mspeculatrix/Hestia_Pico_Motor_Controller/tree/main) - an RPi Pico-based dual motor controller with serial and WebSocket interfaces.

## SUPPORT THIS PROJECT

If you find this project in any way interesting or useful, you could consider [buying me a coffee](https://ko-fi.com/zolachrome).
