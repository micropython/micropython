from machine import Pin, PWM
from micropython import const


class Servo:
	Rotate = const(0)
	Extend = const(1)
	Pinch = const(2)
	Lift = const(3)


class ServoControl:
	Limits = [
		(120, 380),
		(240, 400),
		(120, 180),
		(160, 300)
	]

	StartPos = [512, 400, 1023, 0]

	def __init__(self, pins: tuple):
		self.pins = []
		self.state = [0]*4

		for i in range(4):
			pwm = PWM(Pin(pins[i]), freq=200)
			self.pins.append(pwm)

		self.center()

	def set(self, motor: int, pos: int):
		self.state[motor] = pos
		self._send(motor, pos)

	def get(self, motor: int) -> int:
		return self.state[motor]

	def center(self):
		for i in range(4):
			self.set(i, self.StartPos[i])

	def _send(self, motor: int, pos: int):
		value = self._map(motor, pos)
		self.pins[motor].duty(value)

	def _map(self, motor: int, value: int) -> int:
		limits = self.Limits[motor]
		return int((value / 1023) * (limits[1] - limits[0]) + limits[0])
