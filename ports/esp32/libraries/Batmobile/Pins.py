from micropython import const


class Pins:
	MOTOR_FL_A = const(21)
	MOTOR_FL_B = const(22)
	MOTOR_FR_A = const(23)
	MOTOR_FR_B = const(25)
	MOTOR_BL_A = const(18)
	MOTOR_BL_B = const(19)
	MOTOR_BR_A = const(33)
	MOTOR_BR_B = const(32)

	S3_MOSI = const(17)
	S3_MISO = const(16)
	S3_CLK = const(5)
	S3_CS = const(4)
	S3_RST = const(13)
	S3_INT = const(34)

	LED_R = const(12)
	LED_G = const(15)
	LED_B = const(2)

	LED_FRONT = const(14)
	LED_BACK = const(27)

	SPKR_OUT = const(26)
	SPKR_EN = const(0)

	BATT = const(39)
	CHARGE = const(36)
