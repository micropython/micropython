from CircuitOS import MatrixOutputPart, MatrixOutputBuffered


class MatrixBig(MatrixOutputPart):
	def __init__(self, output: MatrixOutputBuffered):
		super().__init__(output, 8, 9)

	def map(self, x, y):
		return self.get_width() - x - 1, y


class MatrixLeft(MatrixOutputPart):
	def __init__(self, output: MatrixOutputBuffered):
		super().__init__(output, 3, 8)

	def map(self, x, y):
		return 15 - y, self.get_width() - x - 1


class MatrixRight(MatrixOutputPart):
	def __init__(self, output: MatrixOutputBuffered):
		super().__init__(output, 3, 8)

	def map(self, x, y):
		return y + 8, x + 3


class MatrixMid(MatrixOutputPart):
	def __init__(self, output: MatrixOutputBuffered):
		super().__init__(output, 12, 2)

	def map(self, x, y):
		if x > 7:
			return x + y * 4, 6
		else:
			return 8 + x, 8 - y
