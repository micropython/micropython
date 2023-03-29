from machine import Pin, PWM
from micropython import const


class Motor:
	FrontLeft = const(0)
	FrontRight = const(1)
	BackLeft = const(2)
	BackRight = const(3)


class MotorControl:

	def __init__(self, pins: ((int, int), (int, int), (int, int), (int, int))):
		"""
		:param pins: Pin pairs for the motors. Order: FrontLeft, FrontRight, BackLeft, BackRight.
		"""
		self.pins = []
		self.PWM = []
		self.__values = [0] * 4

		for i in range(4):
			pinA = Pin(pins[i][0], mode=Pin.OUT, value=1)
			pinB = Pin(pins[i][1], mode=Pin.OUT, value=1)
			self.pins.append((pinA, pinB))

			pwm = PWM(self.pins[i][0], freq=1000, duty=1023)
			self.PWM.append(pwm)

	def set(self, motor: int, value: int):
		if motor < 0 or motor >= 4:
			return

		value = min(max(value, -100), 100)

		pins = self.pins[motor]
		pwm = self.PWM[motor]

		if value == 0:
			pwm.duty(1023)
			pins[0].value(1)
			pins[1].value(1)
			self.__values[motor] = 0
			return

		self.__values[motor] = value

		reverse = value < 0
		if motor in [Motor.FrontRight, Motor.BackRight]:
			reverse = not reverse

		value = abs(value)
		duty = int((value * 1023) / 100)

		if reverse:
			pins[1].value(1)
			duty = 1023 - duty
		else:
			pins[1].value(0)

		pwm.duty(duty)

	def get(self, motor: int):
		if motor < 0 or motor >= 4:
			return 0

		return self.__values[motor]

	def set_FR(self, value: int):
		self.set(Motor.FrontRight, value)

	def set_FL(self, value: int):
		self.set(Motor.FrontLeft, value)

	def set_BR(self, value: int):
		self.set(Motor.BackRight, value)

	def set_BL(self, value: int):
		self.set(Motor.BackLeft, value)

	def get_FR(self):
		return self.get(Motor.FrontRight)

	def get_FL(self):
		return self.get(Motor.FrontLeft)

	def get_BR(self):
		return self.get(Motor.BackRight)

	def get_BL(self):
		return self.get(Motor.BackLeft)

	def set_all(self, val: int | [int] | (int, int, int, int)):
		if type(val) == int:
			self.set_all((val, val, val, val))
			return

		if type(val) == tuple or type(val) == list:
			if len(val) != 4:
				return

		for i in range(4):
			self.set(i, val[i])

	def get_all(self):
		return tuple(self.get(i) for i in range(4))

	def stop_all(self):
		self.set_all((0, 0, 0, 0))

