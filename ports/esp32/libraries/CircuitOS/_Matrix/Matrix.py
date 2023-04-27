import framebuf
from .MatrixOutput import MatrixOutput
from .. import Display


class Matrix(framebuf.FrameBuffer):
	def __init__(self, output: MatrixOutput):
		self.output = output
		self.width = output.get_width()
		self.height = output.get_height()
		self.bpp = output.get_bpp()
		self.buffer = bytearray(self.width * self.height * self.bpp)

		fmt = framebuf.GS8
		if self.bpp == 2:
			fmt = framebuf.RGB565

		super().__init__(self.buffer, self.width, self.height, fmt)

	def commit(self):
		self.output.push(self.buffer)

	class Color(Display.Color):
		pass
