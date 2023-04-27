from CircuitOS import MatrixOutput
from CircuitOS.Devices.ShiftOutput import ShiftOutput


class PixelPin:
    def __init__(self, index: int, pin: int):
        self.index = index
        self.pin = pin


class PixelMapping:
    def __init__(self, pin_r: PixelPin, pin_g: PixelPin, pin_b: PixelPin):
        self.pin_r = pin_r
        self.pin_g = pin_g
        self.pin_b = pin_b


class MatrixOutputRGB(MatrixOutput):
    def __init__(self, shift: ShiftOutput, pixel_map: [PixelMapping]):
        super().__init__(10, 1, 2)
        self.output = shift
        self.map = pixel_map

    def init(self):
        self.output.set_all(True)

    def push(self, data):
        state = self.output.state

        for i in range(10):
            """ RGB888 format
            pixel = data[i * 3 : (i + 1) * 3]
            r, g, b = pixel[0], pixel[1], pixel[2]
            """

            # RGB565 format
            rgb565 = int.from_bytes(data[i * 2:i * 2 + 2], "big")
            r = ((rgb565 >> 11) & 0x1F) << 3
            g = ((rgb565 >> 5) & 0x3F) << 2
            b = (rgb565 & 0x1F) << 3

            pin_r = self.map[i].pin_r
            pin_g = self.map[i].pin_g
            pin_b = self.map[i].pin_b

            state[pin_r.index][pin_r.pin] = r < 128
            state[pin_g.index][pin_g.pin] = g < 128
            state[pin_b.index][pin_b.pin] = b < 128

        self.output.state = state
        self.output.send(state)
