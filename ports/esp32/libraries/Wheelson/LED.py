from micropython import const
from .Nuvoton import Nuvoton


class LED:

	BYTE_BL_SET = const(0x20)
	BYTE_BL_GET = const(0x21)
	BYTE_HEAD_SET = const(0x22)
	BYTE_HEAD_GET = const(0x23)
	BYTE_RGB_SET = const(0x24)
	BYTE_RGB_GET = const(0x25)

	class Color:
		Off = const(0)
		Red = const(1)
		Green = const(2)
		Yellow = const(3)
		Blue = const(4)
		Magenta = const(5)
		Cyan = const(6)
		White = const(7)

	def __init__(self, nuvo: Nuvoton):
		self.nuvo = nuvo

	def set_backlight(self, backlight: bool):
		self.nuvo.write([self.BYTE_BL_SET, backlight])

	def get_backlight(self):
		data = self.nuvo.write_read(self.BYTE_BL_GET, 1)
		return bool(data[0])

	def set_headlight(self, headlight: bool):
		self.nuvo.write([self.BYTE_HEAD_SET, headlight])

	def get_headlight(self):
		data = self.nuvo.write_read(self.BYTE_HEAD_GET, 1)
		return data[0] & 0xFF

	def set_rgb(self, colour: int):
		self.nuvo.write([self.BYTE_RGB_SET, colour])

	def get_rgb(self):
		data = self.nuvo.write_read(self.BYTE_RGB_GET, 1)
		color = data[0] & 0xFF
		return color
