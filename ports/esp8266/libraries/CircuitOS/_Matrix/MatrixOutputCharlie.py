from CircuitOS import IS31FL3731
from .MatrixOutput import MatrixOutput


class MatrixOutputCharlie(MatrixOutput):

    def __init__(self, is31: IS31FL3731):
        super().__init__(16, 9)
        self.charlie = is31

    def push(self, data: bytearray):
        self.charlie.push(data)
