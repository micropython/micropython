from micropython import const


class Pins:
	BL: int = const(0)
	BATT: int = const(39)
	CHARGE: int = const(16)

	TFT_SCK: int = const(22)
	TFT_MOSI: int = const(21)
	TFT_DC: int = const(5)
	TFT_RST: int = const(17)

	I2C_SDA: int = const(18)
	I2C_SCL: int = const(19)

	JTAG1: int = const(12)
	JTAG2: int = const(13)
	JTAG3: int = const(14)
	JTAG4: int = const(15)


class Buttons:
	Up: int = const(0)
	Down: int = const(1)
	Left: int = const(2)
	Right: int = const(3)
	A: int = const(4)
	B: int = const(5)
	Menu: int = const(6)

	# Maps Buttons [0-6] to their respective GPIO pins
	Pins: [int] = const([
		const(26),
		const(25),
		const(33),
		const(27),
		const(32),
		const(35),
		const(34)
	])
