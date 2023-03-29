from .Pins import *
from .MotorControl import *
from .Data import *
from .S3 import *
from CircuitOS import SingleLED, RGB_LED


motors = MotorControl((
	(Pins.MOTOR_FL_A, Pins.MOTOR_FL_B),
	(Pins.MOTOR_FR_A, Pins.MOTOR_FR_B),
	(Pins.MOTOR_BL_A, Pins.MOTOR_BL_B),
	(Pins.MOTOR_BR_A, Pins.MOTOR_BR_B)
))

frontlights = SingleLED(Pins.LED_FRONT)
backlights = SingleLED(Pins.LED_BACK)
underlights = RGB_LED((Pins.LED_R, Pins.LED_G, Pins.LED_B))

s3 = S3Interface()

def begin():
	s3.begin()
