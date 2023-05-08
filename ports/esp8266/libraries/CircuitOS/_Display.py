import ST7735
import framebuf
import array


class Display():

	def __init__(self, panel: ST7735.TFT, width: int, height: int):
		self.panel = panel
		self.width = width
		self.height = height

	def fill(self, c: int):
		self.panel.fill(c)

	def pixel(self, x: int, y: int, c: int):
		self.panel.pixel((x, y), c)

	def hline(self, x: int, y: int, w: int, c: int):
		self.panel.hline((x, y), w, c)

	def vline(self, x: int, y: int, h: int, c: int):
		self.panel.vline((x, y), h, c)

	def line(self, x1: int, y1: int, x2: int, y2: int, c: int):
		self.panel.line((x1, y1), (x2, y2), c)

	def rect(self, x: int, y: int, w: int, h: int, c: int, f: bool = False):
		if f:
			self.panel.fillrect((x, y), (w, h), c)
		else:
			self.panel.rect((x, y), (w, h), c)

	def ellipse(self, x: int, y: int, xr: int, yr: int, c: int, f: bool = False, m: int = None):
		# TODO - implement ellipse drawing, quadrant restriction with parameter m
		if xr != yr:
			return
		if f:
			self.panel.fillcircle((x, y), xr, c)
		else:
			self.panel.circle((x, y), xr, c)

	def poly(self, x: int, y: int, coords: array.array, c: int, f: bool = None):
		if (f):
			# TODO - implement polygon filling
			pass
		else:
			x1 = coords[0]
			y1 = coords[1]
			for i in range(1, len(coords)):
				x2 = coords[i * 2]
				y2 = coords[i * 2 + 1]
				self.panel.line((x1, y1), (x2, y2), c)
				x1 = x2
				y1 = y2

	def text(self, s: str, x: int, y: int, c: int = None):
		w = len(s) * 8
		h = 8
		buf = bytearray(w * h * 2)
		fbuf = framebuf.FrameBuffer(buf, w, h, framebuf.RGB565)
		keyColor = (~c) & 0xFFFF
		fbuf.fill(keyColor)
		fbuf.text(s, 0, 0, c)
		self.blit(buf, x, y, w, h, keyColor)

	def scroll(self, xstep: int, ystep: int):
		# TODO - implement display scrolling if possible
		pass

	def blit(self, sprite: bytearray, x: int, y: int, w: int, h: int, key: int = -1, palette=None):
		for i in range(len(sprite) // 2):
			color = sprite[i * 2] << 8 | sprite[i * 2 + 1]
			if color != key:
				self.panel.pixel((x + i % w, y + i // w), color)

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
