import framebuf
from machine import SPI, Pin
from st7789 import ST7789, color565
import st7789


class Panel:
	def __init__(self, width: int, height: int):
		self._width = width
		self._height = height

	def width(self):
		return self._width

	def height(self):
		return self._height

	def push(self, data: bytearray):
		pass

	def init(self):
		pass


class Display(framebuf.FrameBuffer):

	def __init__(self, panel: Panel):
		self.buffer = bytearray(panel.width() * panel.height() * 2)
		self.panel = panel
		self.width = panel.width()
		self.height = panel.height()
		super().__init__(self.buffer, panel.width(), panel.height(), framebuf.RGB565)

	def commit(self):
		self.panel.push(self.buffer)

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


class PanelST7735(Panel):
	def __init__(self, spi: SPI, dc: Pin, reset: Pin = None, cs: Pin = None, rotation: int = 0):
		super().__init__(160, 128)
		d = dict(spi=spi, width=self.height(), height=self.width(), dc=dc, rotation=rotation,
				 color_order=st7789.BGR, inversion=False)

		if cs is not None:
			d.update(cs=cs)
		if reset is not None:
			d.update(reset=reset)
		self.tft = ST7789(**d)

	def push(self, data: bytearray):
		self.tft.blit_buffer(data, 0, 0, self.width(), self.height())

	def init(self):
		self.tft.init()


class PanelILI9341(Panel):
	custom_init = [
		(b'\x01', 150),  # soft reset
		(b'\x11', 255),  # exit sleep
		(b'\xCB\x39\x2C\x00\x34\x02',),  # power control A
		(b'\xCF\x00\xC1\x30',),  # power control B
		(b'\xE8\x85\x00\x78',),  # driver timing control A
		(b'\xEA\x00\x00',),  # driver timing control B
		(b'\xED\x64\x03\x12\x81',),  # power on sequence control
		(b'\xF7\x20',),  # pump ratio control
		(b'\xC0\x23',),  # power control,VRH[5:0]
		(b'\xC1\x10',),  # Power control,SAP[2:0];BT[3:0]
		(b'\xC5\x3E\x28',),  # vcm control
		(b'\xC7\x86',),  # vcm control 2
		(b'\x3A\x55',),  # pixel format
		(b'\xB1\x00\x18',),  # frameration control,normal mode full colours
		(b'\xB6\x08\x82\x27',),  # display function control
		(b'\xF2\x00',),  # 3gamma function disable
		(b'\x26\x01',),  # gamma curve selected
		# set positive gamma correction
		(b'\xE0\x0F\x31\x2B\x0C\x0E\x08\x4E\xF1\x37\x07\x10\x03\x0E\x09\x00',),
		# set negative gamma correction
		(b'\xE1\x00\x0E\x14\x03\x11\x07\x31\xC1\x48\x08\x0F\x0C\x31\x36\x0F',),
		(b'\x29', 100),  # display on
	]

	custom_rotations = [
		(0x48, 240, 320, 0, 0),
		(0x28, 320, 240, 0, 0),
		(0x88, 240, 320, 0, 0),
		(0xe8, 320, 240, 0, 0),
	]

	def __init__(self, spi: SPI, dc: Pin, reset: Pin = None, cs: Pin = None, rotation: int = 0):
		super().__init__(320, 240)
		d = dict(spi=spi, height=self.width(), width=self.height(), dc=dc, custom_init=PanelILI9341.custom_init,
				 rotations=PanelILI9341.custom_rotations, rotation=rotation, color_order=st7789.RGB,
				 inversion=False, options=0, buffer_size=0)

		if cs is not None:
			d.update(cs=cs)
		if reset is not None:
			d.update(reset=reset)
		self.tft = ST7789(**d)

	def push(self, data: bytearray):
		self.tft.blit_buffer(data, 0, 0, self.width(), self.height())

	def init(self):
		self.tft.init()


class PanelST7789(Panel):
	def __init__(self, spi: SPI, dc: Pin, reset: Pin = None, cs: Pin = None, rotation: int = 0):
		super().__init__(320, 240)
		d = dict(spi=spi, height=self.width(), width=self.height(), dc=dc, rotation=rotation,
				 color_order=st7789.RGB, inversion=False)

		if cs is not None:
			d.update(cs=cs)
		if reset is not None:
			d.update(reset=reset)

		self.tft = ST7789(**d)

	def push(self, data: bytearray):
		self.tft.blit_buffer(data, 0, 0, self.width(), self.height())

	def init(self):
		self.tft.init()
