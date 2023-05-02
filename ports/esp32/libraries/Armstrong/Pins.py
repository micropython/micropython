from micropython import const


class Pins:
	I2C_SDA: int = const(33)
	I2C_SCL: int = const(32)

	ENC_1A = const(34)
	ENC_1B = const(39)
	ENC_2A = const(16)
	ENC_2B = const(4)
	ENC_3A = const(19)
	ENC_3B = const(18)
	ENC_4A = const(5)
	ENC_4B = const(17)


	LED_R = const(23)
	LED_G = const(22)
	LED_B = const(21)

	SERVO_1 = const(0)
	SERVO_2 = const(2)
	SERVO_3 = const(12)
	SERVO_4 = const(13)


	SHIFTOUT_CLOCK = const(15)
	SHIFTOUT_DATA = const(25)

	SHIFTIN_DATA = const(14)
	SHIFTIN_CLOCK = const(27)
	SHIFTIN_LOAD = const(26)


class Buttons:
	Slot_1 = const(0)
	Slot_2 = const(1)
	Slot_3 = const(2)
	Slot_4 = const(3)
	PlayPause = const(4)
