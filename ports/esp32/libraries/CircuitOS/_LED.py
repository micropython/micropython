from machine import Pin, PWM


class SingleLED:
	def __init__(self, pin: int, reverse_logic=False):
		self.reverse = reverse_logic
		self.pin = Pin(pin)
		self.pwm = PWM(self.pin, freq=1000)

		self.value = 0
		self.set(0)

	def set(self, value: int):
		value = max(0, min(100, value))
		self.value = value

		scaled = int((value / 100) ** 2 * 1023)
		if self.reverse:
			scaled = 1023 - scaled

		self.pwm.duty(scaled)

	def get(self):
		return self.value


class RGB_LED:
	def __init__(self, pins: (int, int, int), reverse_logic=False):
		self.__channels = (
			SingleLED(pins[0], reverse_logic),
			SingleLED(pins[1], reverse_logic),
			SingleLED(pins[2], reverse_logic)
		)

	def set_r(self, value: int):
		self.__channels[0].set(value)

	def set_g(self, value: int):
		self.__channels[1].set(value)

	def set_b(self, value: int):
		self.__channels[2].set(value)

	def get_r(self):
		return self.__channels[0].get()

	def get_g(self):
		return self.__channels[1].get()

	def get_b(self):
		return self.__channels[2].get()

	def set(self, red: int, green: int, blue: int):
		self.set_r(red)
		self.set_g(green)
		self.set_b(blue)

	def get(self):
		return self.get_r(), self.get_g(), self.get_b()
