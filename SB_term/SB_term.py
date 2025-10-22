#!/usr/bin/env python3

# SB_term.py
#
# Sensor Bus terminal.
#
# For the Raspberry Pi.
#
# Resources:
# - https://roboticsbackend.com/raspberry-pi-gpio-interrupts-tutorial/
# - https://sourceforge.net/p/raspberry-gpio-python/wiki/BasicUsage/
#


import RPi.GPIO as GPIO

# Pin assignments
# Don't use following header pins:
#    8, 10      (BCM 14, 15)    - serial
#    19, 21, 23 (BCM 9, 10, 11) - SPI
#    27, 28     (BCM 0, 1)      - I2C
SB_DAT: list[int] = [19]  # header pins [35]
SB_CLK: int = 26  # header pin 37
# SB_INT: int = 27  # header pin 13
# SB_ACT: int = 22  # header pin 15

current_client: int = 0  # This will be the actual BCM pin number


def interrupt_handler(channel) -> None:
	print('Interrupt!')
	global current_client
	for dat in SB_DAT:
		if GPIO.input(dat) == GPIO.LOW:
			print(f'- dat low: {dat}')
			current_client = dat


def disable_interrupts() -> None:
	for pin in SB_DAT:
		GPIO.remove_event_detect(pin)


def enable_interrupts() -> None:
	for pin in SB_DAT:
		GPIO.add_event_detect(
			pin, GPIO.FALLING, callback=interrupt_handler, bouncetime=10
		)


def receive_message(dat: int) -> list[int]:
	recvdMessage: list[int] = [0, 0, 0]
	wait_for_signal_state(SB_CLK, GPIO.LOW)  # Wait for SB_CLK LOW
	wait_for_signal_state(dat, GPIO.LOW)  # Watch for SB_DAT to go LOW
	# Check SB_CLK is still LOW.
	if GPIO.input(SB_CLK) == GPIO.HIGH:
		# This is presumably a problem, but what are we going to do about it?
		print('Well this is not right')
	# Receive loop
	byteIdx: int = 0
	while byteIdx < 3:
		bitIdx: int = 0
		while bitIdx < 8:
			# Wait for SB_CLK LOW
			wait_for_signal_state(SB_CLK, GPIO.LOW)
			# Read bit
			# bitval: int = GPIO.input(dat)
			if GPIO.input(dat) == GPIO.HIGH:
				recvdMessage[byteIdx] |= 1 << bitIdx
			# Wait for SB_CLK HIGH
			wait_for_signal_state(SB_CLK, GPIO.HIGH)
			bitIdx += 1
		byteIdx += 1
	return recvdMessage


def wait_for_signal_state(signal: int, awaitedState: int) -> None:
	unacceptableState: int = GPIO.HIGH
	if awaitedState == GPIO.HIGH:
		unacceptableState = GPIO.LOW
	while GPIO.input(signal) == unacceptableState:
		pass  # might want to put a tiny delay in here


def set_receive_mode(dat: int) -> None:
	print(f'- receive mode - client: {dat}')
	# Assuming all lines are in their default state as INPUTs
	disable_interrupts()  # Disable interrupts
	print('- waiting for SB_DAT to go high')
	# Wait for SB_DAT to go HIGH
	while GPIO.input(dat) == 0:
		pass  # obviously need to do a timeout here
	print('- SB_DAT high')
	# Acknowledge
	# GPIO.setup(dat, GPIO.OUT)
	# GPIO.output(SB_DAT[dat], GPIO.LOW)
	# time.sleep(10 / 1000000)
	# GPIO.output(SB_DAT[dat], GPIO.HIGH)
	# GPIO.setup(dat, GPIO.IN)


def set_SB_default_state() -> None:
	global current_client
	print('- setting default state')
	for dat in SB_DAT:  # Set all pins as inputs
		GPIO.setup(dat, GPIO.IN)
	GPIO.setup(SB_CLK, GPIO.IN)
	current_client = 0
	enable_interrupts()


def main() -> None:
	global current_client
	GPIO.setmode(GPIO.BCM)  # use board numbering
	GPIO.setwarnings(True)
	set_SB_default_state()

	runloop: bool = True
	print('Running')
	# =====  MAIN LOOP  ========================================================
	while runloop:
		if current_client > 0:
			# okay folks, we have a live one
			set_receive_mode(current_client)
			# recvd_msg: list[int] = receive_message(current_client)
			# print(recvd_msg)
			set_SB_default_state()  # also resets current_client


if __name__ == '__main__':
	main()
