from CircuitOS import MatrixOutputPart, MatrixOutputBuffered
from Synthia.MatrixRGB import PixelMapping, PixelPin


class CursorMatrixOutput(MatrixOutputPart):
    def __init__(self, output: MatrixOutputBuffered):
        super().__init__(output, 16, 1)

    def map(self, x: int, y: int) -> tuple:
        return x, y + 5


class SlidersMatrixOutput(MatrixOutputPart):
    def __init__(self, output: MatrixOutputBuffered):
        super().__init__(output, 2, 8)

    def map(self, x: int, y: int) -> tuple:
        return 15 - y, 7 - x


class TrackMatrixOutput(MatrixOutputPart):
    def __init__(self, output: MatrixOutputBuffered):
        super().__init__(output, 16, 5)

    def map(self, x: int, y: int) -> tuple:
        return x, y


class TrackRGBOutput(MatrixOutputPart):
    def __init__(self, output):
        super().__init__(output, 1, 5)

    def map(self, x, y):
        return y, 0


class SlotRGBOutput(MatrixOutputPart):
    def __init__(self, output):
        super().__init__(output, 5, 1)

    def map(self, x, y):
        return x + 5, 0


RGB_map = [
    PixelMapping(PixelPin(0, 2), PixelPin(0, 1), PixelPin(0, 3)),
    PixelMapping(PixelPin(0, 7), PixelPin(0, 6), PixelPin(0, 0)),
    PixelMapping(PixelPin(0, 4), PixelPin(1, 0), PixelPin(0, 5)),
    PixelMapping(PixelPin(1, 2), PixelPin(1, 3), PixelPin(1, 1)),
    PixelMapping(PixelPin(1, 5), PixelPin(1, 6), PixelPin(1, 4)),
    PixelMapping(PixelPin(2, 5), PixelPin(2, 4), PixelPin(2, 6)),
    PixelMapping(PixelPin(2, 0), PixelPin(2, 7), PixelPin(2, 1)),
    PixelMapping(PixelPin(2, 3), PixelPin(2, 2), PixelPin(3, 4)),
    PixelMapping(PixelPin(3, 6), PixelPin(3, 5), PixelPin(3, 0)),
    PixelMapping(PixelPin(3, 2), PixelPin(3, 1), PixelPin(3, 3)),
]
