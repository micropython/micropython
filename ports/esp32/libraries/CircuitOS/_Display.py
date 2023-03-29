import framebuf
import ST7735


class Display(framebuf.FrameBuffer):

	def __init__(self, tft: ST7735.TFT, width: int, height: int):
		self.buffer = bytearray(width * height * 2)
		self.tft = tft
		self.width = width
		self.height = height
		super().__init__(self.buffer, width, height, framebuf.RGB565)

	def commit(self):
		self.tft.image(0, 0, self.width, self.height, self.buffer)

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
