from machine import Pin, I2C
from time import sleep_ms
from .Pins import *
import ustruct


class Nuvoton:
	ADDR = const(0x43)
	BYTE_ID = const(0x00)

	BYTE_EVT_NUM = const(0x12)
	BYTE_EVT_GET = const(0x13)

	NUM_BTN = const(9)
	NUM_ENC = const(7)
	NUM_POT = const(3)

	def __init__(self, i2c: I2C):
		self.i2c = i2c
		self.pin_reset = Pin(Pins.NUVO_RESET, Pin.OUT)

		self._on_press = [None] * self.NUM_BTN
		self._on_release = [None] * self.NUM_BTN
		self._on_encoder = [None] * self.NUM_ENC
		self._on_encoder_left = [None] * self.NUM_ENC
		self._on_encoder_right = [None] * self.NUM_ENC
		self._on_slider = [None] * self.NUM_POT

	def begin(self):
		self.reset()
		sleep_ms(500)

		try:
			identified = self.identify()
		except OSError:
			return False

		return identified

	def identify(self):
		data = self.write_read(self.BYTE_ID, 1)
		return data[0] == self.ADDR

	def reset(self):
		self.pin_reset.off()
		sleep_ms(50)
		self.pin_reset.on()

	def write(self, data: [] | int):
		if type(data) is int:
			data = [data]
		self.i2c.writeto(self.ADDR, bytes(data))

	def read(self, nbytes: int):
		return self.i2c.readfrom(self.ADDR, nbytes)

	def write_read(self, data: [] | int, nbytes: int):
		self.write(data)
		sleep_ms(1)
		return self.read(nbytes)

	def get_num_events(self):
		data = self.write_read(self.BYTE_EVT_NUM, 2)
		return data[1]

	class Device:
		Button = const(0)
		Encoder = const(1)
		Slider = const(2)

	class Event:
		def __init__(self, device: int, id: int, val: int):
			self.device = device
			self.id = id
			self.val = val

	def get_events(self, count: int):
		if count == 0:
			return []

		try:
			self.write([self.BYTE_EVT_GET, count])
		except OSError:
			pass

		sleep_ms(1)

		data = self.read(2 * count + 1)

		events = []
		for i in range(count):
			device_id = data[1 + i * 2]
			value = data[1 + i * 2 + 1]

			id = device_id & 0x0f
			device = device_id >> 4

			if device == self.Device.Encoder:
				value = ustruct.unpack('b', bytes([value]))[0]

			event = self.Event(device, id, value)
			events.append(event)

		return events

	def scan(self):
		count = self.get_num_events()
		events = self.get_events(count)

		for evt in events:
			if evt.device == self.Device.Button and evt.id < self.NUM_BTN:
				if evt.val == 1:
					if self._on_press[evt.id] is not None:
						self._on_press[evt.id]()
				else:
					if self._on_release[evt.id] is not None:
						self._on_release[evt.id]()

			elif evt.device == self.Device.Encoder and evt.id < self.NUM_ENC:
				if self._on_encoder[evt.id] is not None:
					self._on_encoder[evt.id](evt.val)

				if evt.val < 0:
					for _ in range(abs(evt.val)):
						if self._on_encoder_left[evt.id] is None:
							break
						self._on_encoder_left[evt.id]()

				if evt.val > 0:
					for _ in range(abs(evt.val)):
						if self._on_encoder_right[evt.id] is None:
							break
						self._on_encoder_right[evt.id]()

			elif evt.device == self.Device.Slider and evt.id < self.NUM_POT:
				if self._on_slider[evt.id] is not None:
					self._on_slider[evt.id](evt.val)

	def on_press(self, i: int, callback: callable):
		if i >= self.NUM_BTN:
			return
		self._on_press[i] = callback

	def on_release(self, i: int, callback: callable):
		if i >= self.NUM_BTN:
			return
		self._on_release[i] = callback

	def on_enc_move(self, i: int, callback: callable):
		if i >= self.NUM_ENC:
			return
		self._on_encoder[i] = callback

	def on_enc_move_left(self, i: int, callback: callable):
		if i >= self.NUM_ENC:
			return
		self._on_encoder_left[i] = callback

	def on_enc_move_right(self, i: int, callback: callable):
		if i >= self.NUM_ENC:
			return
		self._on_encoder_right[i] = callback

	def on_slider_move(self, i: int, callback: callable):
		if i >= self.NUM_POT:
			return
		self._on_slider[i] = callback
