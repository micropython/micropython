from st7789 import ST7789, color565
import framebuf
import array


class Display():

	def __init__(self, panel: ST7789, width: int, height: int):
		self.panel = panel
		self.width = width
		self.height = height

	def fill(self, c: int):
		self.panel.fill(c)

	def pixel(self, x: int, y: int, c: int):
		self.panel.pixel(x, y, c)

	def hline(self, x: int, y: int, w: int, c: int):
		self.panel.hline(x, y, w, c)

	def vline(self, x: int, y: int, h: int, c: int):
		self.panel.vline(x, y, h, c)

	def line(self, x1: int, y1: int, x2: int, y2: int, c: int):
		self.panel.line(x1, y1, x2, y2, c)

	def rect(self, x: int, y: int, w: int, h: int, c: int, f: bool = False):
		if f:
			self.panel.fill_rect(x, y, w, h, c)
		else:
			self.panel.rect(x, y, w, h, c)

	def ellipse(self, x: int, y: int, xr: int, yr: int, c: int, f: bool = False, m: int = None):
		# TODO - implement ellipse drawing, quadrant restriction with parameter m
		if xr != yr:
			return
		if f:
			self.panel.fill_circle(x, y, xr, c)
		else:
			self.panel.circle(x, y, xr, c)

	def poly(self, x: int, y: int, coords: array.array, c: int, f: bool = None):
		tuples = [(coords[i], coords[i + 1]) for i in range(0, len(coords), 2)]

		if (f):
			self.panel.fill_polygon(tuples, x, y, c)
			pass
		else:
			self.panel.polygon(tuples, x, y, c, 0, 0, 0)

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
		self.panel.blit_img(sprite, x, y, w, h, key)

	class Color:

		def __new__(cls, red: int, green: int, blue: int):
			return color565(red, green, blue)

		Black = 0
		Red = color565(0xFF, 0x00, 0x00)
		Maroon = color565(0x80, 0x00, 0x00)
		Green = color565(0x00, 0xFF, 0x00)
		Forest = color565(0x00, 0x80, 0x80)
		Blue = color565(0x00, 0x00, 0xFF)
		Navy = color565(0x00, 0x00, 0x80)
		Cyan = color565(0x00, 0xFF, 0xFF)
		Yellow = color565(0xFF, 0xFF, 0x00)
		Purple = color565(0xFF, 0x00, 0xFF)
		White = color565(0xFF, 0xFF, 0xFF)
		Gray = color565(0x80, 0x80, 0x80)
