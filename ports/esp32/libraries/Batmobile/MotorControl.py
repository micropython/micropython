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
		self.__pins = []
		self.__PWM = []
		self.__values = [0] * 4

		for i in range(4):
			pinA = Pin(pins[i][0], mode=Pin.OUT, value=1)
			pinB = Pin(pins[i][1], mode=Pin.OUT, value=1)
			self.__pins.append((pinA, pinB))

			pwm = PWM(self.__pins[i][0], freq=1000, duty=1023)
			self.__PWM.append(pwm)

	def set(self, motor: int, value: int):
		if motor < 0 or motor >= 4:
			return

		value = min(max(value, -100), 100)

		pins = self.__pins[motor]
		pwm = self.__PWM[motor]

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

	def setFR(self, value: int):
		self.set(Motor.FrontRight, value)

	def setFL(self, value: int):
		self.set(Motor.FrontLeft, value)

	def setBR(self, value: int):
		self.set(Motor.BackRight, value)

	def setBL(self, value: int):
		self.set(Motor.BackLeft, value)

	def getFR(self):
		return self.get(Motor.FrontRight)

	def getFL(self):
		return self.get(Motor.FrontLeft)

	def getBR(self):
		return self.get(Motor.BackRight)

	def getBL(self):
		return self.get(Motor.BackLeft)

	def setAll(self, val: int | [int] | (int, int, int, int)):
		if type(val) == int:
			self.setAll((val, val, val, val))
			return

		if type(val) == tuple or type(val) == list:
			if len(val) != 4:
				return

		for i in range(4):
			self.set(i, val[i])

	def getAll(self):
		return tuple(self.get(i) for i in range(4))

	def stopAll(self):
		self.setAll((0, 0, 0, 0))

