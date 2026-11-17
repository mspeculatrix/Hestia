import RPi.GPIO as gpio  # type: ignore


def main() -> None:
	print(gpio.VERSION)


if __name__ == '__main__':
	main()
