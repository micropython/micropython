import framebuf

import ILI9341
import ST7735
from st7789 import ST7789

class Panel:
	def push(self, data: bytearray, width, height):
		pass


class Display(framebuf.FrameBuffer):

	def __init__(self, panel: Panel, width: int, height: int):
		self.buffer = bytearray(width * height * 2)
		self.panel = panel
		self.width = width
		self.height = height
		super().__init__(self.buffer, width, height, framebuf.RGB565)

	def commit(self):
		self.panel.push(self.buffer, self.width, self.height)

	class Color:

		def __new__(cls, red: int, green: int, blue: int):
			return ST7735.TFTColor(red, green, blue)

		Black = 0
		Red = ST7735.TFTColor(0xFF, 0x00, 0x00)
		Maroon = ST7735.TFTColor(0x80, 0x00, 0x00)
		Green = ST7735.TFTColor(0x00, 0xFF, 0x00)
		Forest = ST7735.TFTColor(0x00, 0x80, 0x80)
		Blue = ST7735.TFTColor(0x00, 0x00, 0xFF)
		Navy = ST7735.TFTColor(0x00, 0x00, 0x80)
		Cyan = ST7735.TFTColor(0x00, 0xFF, 0xFF)
		Yellow = ST7735.TFTColor(0xFF, 0xFF, 0x00)
		Purple = ST7735.TFTColor(0xFF, 0x00, 0xFF)
		White = ST7735.TFTColor(0xFF, 0xFF, 0xFF)
		Gray = ST7735.TFTColor(0x80, 0x80, 0x80)


class PanelST7735(Panel):
	def __init__(self, tft: ST7735.TFT):
		self.tft = tft

	def push(self, data: bytearray, width, height):
		self.tft.image(0, 0, width, height, data)


class PanelILI9341(Panel):
	def __init__(self, tft: ILI9341.TFT):
		self.tft = tft

	def push(self, data: bytearray, width, height):
		self.tft.draw_sprite(data, 0, 0, width, height)


class PanelST7789(Panel):
	def __init__(self, tft: ST7789):
		self.tft = tft

	def push(self, data: bytearray, width, height):
		self.tft.blit_buffer(data, 0, 0, width, height)
