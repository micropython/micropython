from CircuitOS import InputShift, Encoder, RGB_LED, Encoders, ShiftOutput
from .Pins import *
from .Servos import ServoControl

buttons = InputShift(Pins.SHIFTIN_DATA, Pins.SHIFTIN_CLOCK, Pins.SHIFTIN_LOAD)

enc_A = Encoder(Pins.ENC_3A, Pins.ENC_3B)
enc_B = Encoder(Pins.ENC_4A, Pins.ENC_4B)
enc_C = Encoder(Pins.ENC_1A, Pins.ENC_1B)
enc_D = Encoder(Pins.ENC_2A, Pins.ENC_2B)
encoders = Encoders([enc_A, enc_B, enc_C, enc_D])

rgb = RGB_LED((Pins.LED_R, Pins.LED_G, Pins.LED_B), reverse_logic=True)

leds = ShiftOutput(Pins.SHIFTOUT_CLOCK, Pins.SHIFTOUT_DATA)
class LED:
	Slot_1 = const(1)
	Slot_2 = const(2)
	Slot_3 = const(3)
	Slot_4 = const(4)
	PlayPause = const(0)


servos = ServoControl((Pins.SERVO_1, Pins.SERVO_2, Pins.SERVO_3, Pins.SERVO_4))


def begin():
	leds.set_all(False)
	rgb.set(0, 0, 0)
	servos.center()
	buttons.scan()
	encoders.scan()
