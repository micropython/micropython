from machine import Pin, I2C
from time import sleep_ms
from .Pins import *


class Nuvoton:
	ADDR = 0x38
	BYTE_ID = 0x00
	BYTE_BATT = 0x50
	BYTE_SHUTDOWN = 0x51

	def __init__(self, i2c: I2C):
		self.i2c = i2c
		self.pin_reset = Pin(Pins.NUVO_RESET, Pin.OUT)

	def begin(self):
		self.reset()
		sleep_ms(500)

		try:
			self.read(self.BYTE_ID)
		except OSError:
			return False

		return self.identify()

	def identify(self):
		data = self.write_read(self.BYTE_ID, 1)
		return data[0] == self.ADDR

	def reset(self):
		self.pin_reset.off()
		sleep_ms(50)
		self.pin_reset.on()

	def get_battery(self):
		data = self.write_read(self.BYTE_BATT, 2)
		return data[0] << 8 | data[1]

	def shutdown(self):
		from Wheelson import motors, led
		from .LED import LED

		for i in range(4):
			motors.set(i, 0)

		led.set_backlight(False)
		led.set_headlight(0)
		led.set_rgb(LED.Color.Off)

		try:
			self.write(self.BYTE_SHUTDOWN)
		except OSError:
			pass

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
