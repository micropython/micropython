import framebuf
from . import MatrixOutput
from .. import Display


class Matrix(framebuf.FrameBuffer):
    def __init__(self, output: MatrixOutput, rotation: int = 0):
        self.output = output
        self.rotation = rotation
        self.width = output.get_width()
        self.height = output.get_height()
        self.bpp = output.get_bpp()
        self.buffer = bytearray(self.width * self.height * self.bpp)

        fmt = framebuf.GS8
        if self.bpp == 2:
            fmt = framebuf.RGB565

        super().__init__(self.buffer, self.width, self.height, fmt)

    def commit(self):
        rotated_buffer = self.rotate_buffer(self.buffer)
        self.output.push(rotated_buffer)

    def rotate_buffer(self, buffer: bytearray):
        if self.rotation == 0:
            return buffer
        elif self.rotation == 1:
            return self.rotate_90(buffer)
        elif self.rotation == 2:
            return self.rotate_180(buffer)
        elif self.rotation == 3:
            return self.rotate_270(buffer)
        else:
            raise ValueError("Rotation value should be between 0 and 3")

    def rotate_90(self, buffer):
        rotated = bytearray(len(buffer))
        for y in range(self.height):
            for x in range(self.width):
                src_idx = (y * self.width + x) * self.bpp
                dst_idx = ((self.width - x - 1) * self.height + y) * self.bpp
                rotated[dst_idx:dst_idx + self.bpp] = buffer[src_idx:src_idx + self.bpp]
        return rotated

    def rotate_180(self, buffer):
        rotated = bytearray(len(buffer))
        for y in range(self.height):
            for x in range(self.width):
                src_idx = (y * self.width + x) * self.bpp
                dst_idx = ((self.height - y - 1) * self.width + (self.width - x - 1)) * self.bpp
                rotated[dst_idx:dst_idx + self.bpp] = buffer[src_idx:src_idx + self.bpp]
        return rotated

    def rotate_270(self, buffer):
        rotated = bytearray(len(buffer))
        for y in range(self.height):
            for x in range(self.width):
                src_idx = (y * self.width + x) * self.bpp
                dst_idx = (x * self.height + (self.height - y - 1)) * self.bpp
                rotated[dst_idx:dst_idx + self.bpp] = buffer[src_idx:src_idx + self.bpp]
        return rotated

    class Color(Display.Color):
        pass
