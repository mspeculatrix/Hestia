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

CLK_STROBE_DIV: int = 100000

SIG_AWAIT_TO = 50  # milliseconds

# ERROR CODES
ERR_SIGNAL_AWAIT_TO: int = 10
ERR_CLK_AWAIT_LOW_TO: int = 20
ERR_CLK_AWAIT_HIGH_TO: int = 21
ERR_DAT_AWAIT_HIGH_TO: int = 22

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


def receive_byte(dat: int) -> tuple[int, int]:
	error: int = 0
	recByte: int = 0
	bitIdx: int = 0
	while bitIdx < 8:
		# Wait for SB_CLK LOW
		err: int = wait_for_signal_state(SB_CLK, GPIO.LOW)
		if err > 0:
			error = ERR_CLK_AWAIT_LOW_TO
			break
		# Could try:
		# GPIO.wait_for_edge(SB_CLK, GPIO.FALLING, timeout=5000)
		# Read bit
		if GPIO.input(dat) == GPIO.HIGH:
			recByte |= 1 << bitIdx
		# Wait for SB_CLK HIGH
		err = wait_for_signal_state(SB_CLK, GPIO.HIGH)
		if err > 0:
			error = ERR_CLK_AWAIT_HIGH_TO
			break
		# print('clk high')
		bitIdx += 1
	return (recByte, error)


def receive_message(dat: int) -> tuple[list[int], int | None]:
	getByte: int = 0
	error: int | None = None
	recvdMessage: list[int] = []
	# print(f'R:{dat}')
	disable_interrupts()  # Disable interrupts
	# Acknowledge with CLK strobe
	GPIO.setup(SB_CLK, GPIO.OUT)
	GPIO.output(SB_CLK, GPIO.LOW)
	time.sleep(1 / CLK_STROBE_DIV)
	GPIO.output(SB_CLK, GPIO.HIGH)
	GPIO.setup(SB_CLK, GPIO.IN)
	err = wait_for_signal_state(SB_CLK, GPIO.HIGH)
	if err > 0:
		error = ERR_CLK_AWAIT_HIGH_TO
	else:
		err = wait_for_signal_state(dat, GPIO.HIGH)
		if err > 0:
			error = ERR_DAT_AWAIT_HIGH_TO
		else:
			# Get first byte to discover the message length
			getByte, err = receive_byte(dat)
			recvdMessage.append(getByte)
			if err > 0:
				error = err
			else:  # Receive loop for rest of message
				while len(recvdMessage) < recvdMessage[0]:
					getByte, err = receive_byte(dat)
					recvdMessage.append(getByte)
					if err > 0:
						error = err
						break
	enable_interrupts()
	return (recvdMessage, error)


def set_SB_default_state() -> None:
	global current_client
	# print('= Setting default state')
	for dat in SB_DAT:  # Set all pins as inputs
		GPIO.setup(dat, GPIO.IN)
	GPIO.setup(SB_CLK, GPIO.IN)
	GPIO.setup(SB_ACT, GPIO.IN)
	current_client = 0
	enable_interrupts()


def wait_for_signal_state(signal: int, awaitedState: int) -> int:
	error: int = 0
	start = time.perf_counter()
	done: bool = False
	while not done:
		if GPIO.input(signal) == awaitedState:
			done = True
		elif (time.perf_counter() - start) * 1000 >= SIG_AWAIT_TO:
			error = ERR_SIGNAL_AWAIT_TO
			done = True
	return error


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
			recvd_msg, error = receive_message(current_client)
			if error is None:
				print(recvd_msg)
			else:
				print(f'***ERROR: {error}')
			# set_SB_default_state()  # also resets current_client
			set_SB_default_state()


if __name__ == '__main__':
	main()
