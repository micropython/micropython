from micropython import const


class Pins:
	BL: int = const(21)
	BATT: int = const(35)
	# CHARGE: int = const(16)

	SPI_MOSI: int = const(23)
	SPI_MISO: int = const(19)
	SPI_SCK: int = const(18)

	TFT_DC: int = const(0)
	TFT_RST: int = const(2)
	TFT_CS: int = const(4)

	SD_CS: int = const(5)

	I2C_SDA: int = const(14)
	I2C_SCL: int = const(27)

	RGB: int = const(12)


class Buttons:
	Num_1: int = const(0)
	Num_2: int = const(1)
	Num_3: int = const(2)
	Num_4: int = const(3)
	Num_5: int = const(4)
	Num_6: int = const(5)
	Num_7: int = const(6)
	Num_8: int = const(7)
	Num_9: int = const(8)
	Num_0: int = const(10)
	Num_Ast: int = const(9)
	Num_Hash: int = const(11)

	Home: int = const(14)
	Power: int = const(13)
	Alt_L: int = const(15)
	Alt_R: int = const(12)
