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


import time

import RPi.GPIO as GPIO

# Pin assignments
# Don't use following header pins:
#    8, 10      (BCM 14, 15)    - serial
#    19, 21, 23 (BCM 9, 10, 11) - SPI
#    27, 28     (BCM 0, 1)      - I2C
SB_DAT: list[int] = [19]  # header pins [35]
SB_CLK: int = 26  # header pin 37
SB_ACT: int = 13  # header pin 33

current_client: int = 0  # This will be the actual BCM pin number


def interrupt_handler(channel) -> None:
	global current_client
	for dat in SB_DAT:
		if GPIO.input(dat) == GPIO.LOW:
			current_client = dat


def disable_interrupts() -> None:
	GPIO.remove_event_detect(SB_ACT)


def enable_interrupts() -> None:
	GPIO.add_event_detect(
		# SB_ACT, GPIO.FALLING, callback=interrupt_handler, bouncetime=10
		SB_ACT,
		GPIO.FALLING,
		callback=interrupt_handler,
	)


def receive_message(dat: int) -> list[int]:
	recvdMessage: list[int] = [0, 0, 0, 0]
	wait_for_signal_state(SB_CLK, GPIO.HIGH)
	wait_for_signal_state(dat, GPIO.HIGH)
	# Receive loop
	byteIdx: int = 0
	# The following doesn't work with any message other that 4
	# bytes long. Need to read the first byte!!
	while byteIdx < 4:
		bitIdx: int = 0
		while bitIdx < 8:
			# Wait for SB_CLK LOW
			wait_for_signal_state(SB_CLK, GPIO.LOW)
			# Could try:
			# GPIO.wait_for_edge(SB_CLK, GPIO.FALLING, timeout=5000)
			# print('clk low')
			# Read bit
			if GPIO.input(dat) == GPIO.HIGH:
				recvdMessage[byteIdx] |= 1 << bitIdx
			# Wait for SB_CLK HIGH
			wait_for_signal_state(SB_CLK, GPIO.HIGH)
			# print('clk high')
			bitIdx += 1
		byteIdx += 1
	return recvdMessage


def set_receive_mode(dat: int) -> None:
	# print(f'= R: {dat}')
	# Assuming all lines are in their default state as INPUTs
	disable_interrupts()  # Disable interrupts
	# Acknowledge with CLK strobe
	GPIO.setup(SB_CLK, GPIO.OUT)
	GPIO.output(SB_CLK, GPIO.LOW)
	time.sleep(1 / 10000)
	GPIO.output(SB_CLK, GPIO.HIGH)
	GPIO.setup(SB_CLK, GPIO.IN)


def set_SB_default_state() -> None:
	global current_client
	# print('= Setting default state')
	for dat in SB_DAT:  # Set all pins as inputs
		GPIO.setup(dat, GPIO.IN)
	GPIO.setup(SB_CLK, GPIO.IN)
	GPIO.setup(SB_ACT, GPIO.IN)
	current_client = 0
	enable_interrupts()


def wait_for_signal_state(signal: int, awaitedState: int) -> None:
	unacceptableState: int = GPIO.HIGH
	if awaitedState == GPIO.HIGH:
		unacceptableState = GPIO.LOW
	while GPIO.input(signal) == unacceptableState:
		pass  # might want to put a tiny delay in here


################################################################################
#####  MAIN                                                                #####
################################################################################
def main() -> None:
	global current_client
	GPIO.setmode(GPIO.BCM)  # use board numbering
	GPIO.setwarnings(True)
	set_SB_default_state()

	runloop: bool = True
	print('= Running')
	# =====  MAIN LOOP  ========================================================
	while runloop:
		if current_client > 0:
			# okay folks, we have a live one
			set_receive_mode(current_client)
			recvd_msg: list[int] = receive_message(current_client)
			print(recvd_msg)
			# set_SB_default_state()  # also resets current_client
			current_client = 0
			enable_interrupts()


if __name__ == '__main__':
	main()
