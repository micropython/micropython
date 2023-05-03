from machine import Pin


class ShiftOutput:
	def __init__(self, pin_clk: int, pins_data: [int]|int):
		if type(pins_data) is int:
			pins_data = [pins_data]

		self.pin_clk = Pin(pin_clk, Pin.OUT)
		self.pins_data = [Pin(data, Pin.OUT) for data in pins_data]

		self.size = len(pins_data)
		self.state = [[False]*8 for _ in range(self.size)]

	def set(self, pin: int, state: bool):
		index, pin = divmod(pin, 8)
		if index >= self.size or pin >= 8:
			return
		self.state[index][pin] = state
		self.send(self.state)

	def set_all(self, state: bool):
		for shift in self.state:
			for i in range(8):
				shift[i] = state
		self.send(self.state)

	def send(self, state):
		self.pin_clk.value(0)

		for i in range(8):
			for j in range(self.size):
				self.pins_data[j].value(state[j][7 - i])
			self.pin_clk.value(1)
			self.pin_clk.value(0)

	def get(self, pin: int):
		index, pin = divmod(pin, 8)
		if index >= self.size or pin >= 8:
			return False
		return self.state[index][pin]

