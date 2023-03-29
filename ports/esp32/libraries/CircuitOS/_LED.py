from machine import Pin, PWM


class SingleLED:
	def __init__(self, pin: int, reverse_logic=False):
		self.reverse = reverse_logic
		self.__pin = Pin(pin)
		self.__pwm = PWM(self.__pin, freq=1000)
		self.set(0)

	def set(self, value: int):
		value = max(0, min(100, value))
		self.__value = value

		scaled = int((value / 100) ** 2 * 1023)
		if self.reverse:
			scaled = 1023 - scaled

		self.__pwm.duty(scaled)

	def get(self):
		return self.__value


class RGB_LED:
	def __init__(self, pins: (int, int, int), reverse_logic=False):
		self.__channels = (
			SingleLED(pins[0], reverse_logic),
			SingleLED(pins[1], reverse_logic),
			SingleLED(pins[2], reverse_logic)
		)

	def setR(self, value: int):
		self.__channels[0].set(value)

	def setG(self, value: int):
		self.__channels[1].set(value)

	def setB(self, value: int):
		self.__channels[2].set(value)

	def getR(self):
		return self.__channels[0].get()

	def getG(self):
		return self.__channels[1].get()

	def getB(self):
		return self.__channels[2].get()

	def set(self, red: int, green: int, blue: int):
		self.setR(red)
		self.setG(green)
		self.setB(blue)

	def get(self):
		return self.getR(), self.getG(), self.getB()
