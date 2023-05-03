
class MatrixOutput:
	def __init__(self, width: int, height: int, bpp: int = 1):
		self.width = width
		self.height = height
		self.bpp = bpp

	def push(self, data: bytearray):
		raise NotImplementedError

	def get_width(self):
		return self.width

	def get_height(self):
		return self.height

	def get_bpp(self):
		return self.bpp


class MatrixOutputBuffered(MatrixOutput):
	def __init__(self, output: MatrixOutput):
		super().__init__(output.get_width(), output.get_height(), output.get_bpp())
		self.output = output
		self.buffer = bytearray(self.width * self.height * self.bpp)

	def push(self, data: bytearray):
		self.buffer = bytearray(data)
		self.output.push(data)

	def get_buffer(self):
		return self.buffer


class MatrixOutputPart(MatrixOutput):
	def __init__(self, output: MatrixOutputBuffered, width: int, height: int):
		super().__init__(width, height, output.get_bpp())
		self.output = output

	def map(self, x: int, y: int) -> tuple:
		raise NotImplementedError

	def push(self, data: bytearray):
		width = self.get_width()
		height = self.get_height()
		bpp = self.output.bpp

		whole = self.output.get_buffer()

		for x in range(width):
			for y in range(height):
				mapped_x, mapped_y = self.map(x, y)
				pixel_offset = (x + y * width) * bpp
				target_offset = (mapped_x + mapped_y * self.output.get_width()) * bpp
				whole[target_offset: target_offset + bpp] = data[pixel_offset: pixel_offset + bpp]

		self.output.push(whole)
