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

CLK_STROBE_DURATION = 0.0001
DAT_STROBE_DURATION = 0.00001
BIT_PAUSE_DURATION = 0.00001
SEND_PAUSE_DURATION = 0.001

SIG_AWAIT_TO = 50  # timeout in milliseconds

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
	time.sleep(CLK_STROBE_DURATION)
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


def send_message(sensor: int, bytes: list[int]) -> int:
	disable_interrupts()
	GPIO.setup(SB_ACT, GPIO.OUT)
	GPIO.setup(sensor, GPIO.OUT)
	GPIO.output(SB_ACT, GPIO.LOW)
	GPIO.output(sensor, GPIO.LOW)  # strobe DAT line
	time.sleep(DAT_STROBE_DURATION)
	GPIO.output(sensor, GPIO.HIGH)
	err: int = wait_for_signal_state(SB_CLK, GPIO.LOW)  # wait for CLK strobe
	if err == 0:
		GPIO.setup(SB_CLK, GPIO.OUT)
		GPIO.output(SB_CLK, GPIO.HIGH)
		# Need a short pause here
		time.sleep(SEND_PAUSE_DURATION)
		for byte in bytes:
			for i in range(0, 8):
				if byte & (1 << i):
					GPIO.output(sensor, GPIO.HIGH)
				else:
					GPIO.output(sensor, GPIO.LOW)
				time.sleep(BIT_PAUSE_DURATION)  #  bit pause
				GPIO.output(SB_CLK, GPIO.LOW)
				time.sleep(BIT_PAUSE_DURATION)  #  bit pause
				GPIO.output(SB_CLK, GPIO.HIGH)
	set_SB_default_state()
	return err


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
	print('SB_term running')
	counter: int = 0
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

		counter += 1
		if counter == 6500000:
			print('sending')
			counter = 0
			msg: list[int] = [4, 50, 170, 255]
			send_message(SB_DAT[0], msg)


if __name__ == '__main__':
	main()
