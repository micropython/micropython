from machine import Pin


class Input:

	def __init__(self, numButtons: int):
		self.numButtons = numButtons

		self.__state: [] = [False] * numButtons
		self.__onPress: [] = [None] * numButtons
		self.__onRelease: [] = [None] * numButtons

		self._loopTask = None

		pass

	def state(self, i: int):
		if i >= self.numButtons: return False

		return self.__state[i]

	def onPress(self, i: int, callback):
		if i >= self.numButtons: return

		self.__onPress[i] = callback

	def onRelease(self, i: int, callback):
		if i >= self.numButtons: return

		self.__onRelease[i] = callback

	async def loop(self):
		while True:
			self.scan()

	def pressed(self, i: int):
		if i >= self.numButtons: return

		old = self.__state[i]
		self.__state[i] = True

		if old is not self.__state[i] and self.__onPress[i] is not None:
			self.__onPress[i]()

	def released(self, i: int):
		if i >= self.numButtons: return

		old = self.__state[i]
		self.__state[i] = False

		if old is not self.__state[i] and self.__onRelease[i] is not None:
			self.__onRelease[i]()

	def scan(self):
		pass


class InputShift(Input):

	def __init__(self, dataPin: int, clockPin: int, loadPin: int, numShifts: int = 1):
		self.numShifts = numShifts
		self.dataPin: Pin = Pin(dataPin, mode=Pin.IN)
		self.clockPin: Pin = Pin(clockPin, mode=Pin.OUT, value=1)
		self.loadPin: Pin = Pin(loadPin, mode=Pin.OUT, value=1)

		self.bitState = [0] * numShifts * 8

		super().__init__(numShifts * 8)

	def scan(self):
		self.clockPin.value(0)
		self.LH(self.loadPin)

		for i in range(self.numShifts * 8):
			self.bitState[self.numShifts * 8 - i - 1] = self.dataPin.value()
			self.HL(self.clockPin)

		for i in range(self.numShifts * 8):
			if self.bitState[i]:
				self.released(i)
			else:
				self.pressed(i)

	def HL(self, pin: Pin):
		pin.value(1)
		pin.value(0)

	def LH(self, pin: Pin):
		pin.value(0)
		pin.value(1)