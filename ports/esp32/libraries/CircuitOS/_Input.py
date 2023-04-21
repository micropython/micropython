from machine import Pin, Signal
from .Devices.PCA95XX import PCA95XX


class Input:

	def __init__(self, num_buttons: int):
		self.numButtons = num_buttons

		self.__state: [] = [False] * num_buttons
		self.__on_press: [] = [None] * num_buttons
		self.__on_release: [] = [None] * num_buttons

		pass

	def state(self, i: int):
		if i >= self.numButtons:
			return False

		return self.__state[i]

	def on_press(self, i: int, callback):
		if i >= self.numButtons:
			return

		self.__on_press[i] = callback

	def on_release(self, i: int, callback):
		if i >= self.numButtons:
			return

		self.__on_release[i] = callback

	async def loop(self):
		while True:
			self.scan()

	def pressed(self, i: int):
		if i >= self.numButtons:
			return

		old = self.__state[i]
		self.__state[i] = True

		if old is not self.__state[i] and self.__on_press[i] is not None:
			self.__on_press[i]()

	def released(self, i: int):
		if i >= self.numButtons:
			return

		old = self.__state[i]
		self.__state[i] = False

		if old is not self.__state[i] and self.__on_release[i] is not None:
			self.__on_release[i]()

	def scan(self):
		pass


class InputShift(Input):

	def __init__(self, pin_data: int, pin_clock: int, pin_load: int, count: int = 1):
		self.numShifts = count
		self.pin_data: Pin = Pin(pin_data, mode=Pin.IN)
		self.pin_clock: Pin = Pin(pin_clock, mode=Pin.OUT, value=1)
		self.pin_load: Pin = Pin(pin_load, mode=Pin.OUT, value=1)

		self.bitState = [0] * count * 8

		super().__init__(count * 8)

	def scan(self):
		self.pin_clock.value(0)
		self.LH(self.pin_load)

		for i in range(self.numShifts * 8):
			self.bitState[self.numShifts * 8 - i - 1] = self.pin_data.value()
			self.HL(self.pin_clock)

		for i in range(self.numShifts * 8):
			if self.bitState[i]:
				self.released(i)
			else:
				self.pressed(i)

	@staticmethod
	def HL(pin: Pin):
		pin.value(1)
		pin.value(0)

	@staticmethod
	def LH(pin: Pin):
		pin.value(0)
		pin.value(1)


class InputGPIO(Input):

	def __init__(self, pins: [int], inverted: bool = False):
		super().__init__(len(pins))

		self.pins = []
		self.signals = []

		for i in range(len(pins)):
			pin = Pin(pins[i], mode=Pin.IN)
			signal = Signal(pin, invert=inverted)

			self.pins.append(pin)
			self.signals.append(signal)

	def scan(self):
		state: [int] = []

		for i in range(len(self.signals)):
			signal = self.signals[i]
			state.append(signal.value())

		for i in range(len(state)):
			if state[i] == 1:
				self.pressed(i)
			else:
				self.released(i)


class InputExpander(Input):

	def __init__(self, expander: PCA95XX):
		super().__init__(16)

		self.expander = expander
		self.buttons = []

	def scan(self):
		state = self.expander.state_read()

		for i in range(len(self.buttons)):
			pin = self.buttons[i]

			if ((state >> pin) & 1) == 1:
				self.released(pin)
			else:
				self.pressed(pin)

	def register_button(self, pin: int):
		if pin >= 16:
			return

		self.expander.pin_mode(pin, Pin.IN)
		self.buttons.append(pin)

