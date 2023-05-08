from micropython import const


class Pins:
	BUZZ_PIN = const(12)
	BL_PIN_I2C = const(8)
	BL_PIN_IO = const(15)

	I2C_SDA = const(4)
	I2C_SCL = const(5)
	SHIFT_DATA = const(5)
	SHIFT_CLOCK = const(4)
	SHIFT_LOAD = const(0)
	SHIFT_COUNT = const(1)  # 1 shift reg for inputs

	TFT_DC = const(2)

	SPI_SCK = const(14)
	SPI_MOSI = const(13)


class Buttons:
	Up = const(0)
	Down = const(1)
	Left = const(2)
	Right = const(3)
	A = const(5)
	B = const(4)
	C = const(6)
