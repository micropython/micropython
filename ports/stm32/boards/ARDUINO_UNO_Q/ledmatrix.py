# MIT license; Copyright (c) 2026 Qualcomm Innovation Center, Inc.
#
# Driver for the Arduino UNO Q's 13x8 LED matrix.  LedMatrix is a
# framebuf.FrameBuffer over the live scan buffer owned by the _ledmatrix C
# module, so the usual drawing methods apply and edits appear without a flush.
#
#     from ledmatrix import LedMatrix
#     m = LedMatrix()
#     m.text("A", 3, 0, 1)

import framebuf
import _ledmatrix


class LedMatrix(framebuf.FrameBuffer):
    WIDTH = _ledmatrix.WIDTH
    HEIGHT = _ledmatrix.HEIGHT

    def __init__(self):
        self._buf = _ledmatrix.buffer()
        super().__init__(self._buf, self.WIDTH, self.HEIGHT, framebuf.MONO_VLSB)
        _ledmatrix.start()

    def deinit(self):
        _ledmatrix.stop()
