from machine import Pin, I2C
from CircuitOS import InputShift, Encoder, IS31FL3731, MatrixOutputCharlie, Matrix, ShiftOutput, Slider
from .MatrixMap import *
from .MatrixRGB import *
from .Pins import *

buttons = InputShift(pin_data=Pins.INP_DATA, pin_clock=Pins.INP_SCK, pin_load=Pins.INP_PL)

enc_left = Encoder(Pins.ENC_L1, Pins.ENC_L2)
enc_right = Encoder(Pins.ENC_R1, Pins.ENC_R2)

slider_left = Slider(Pins.POT_L, min=220, max=720, ema_a=0.01, reverse=True)
slider_right = Slider(Pins.POT_R, min=220, max=720, ema_a=0.01, reverse=True)


i2c = I2C(0, sda=Pin(Pins.I2C_SDA), scl=Pin(Pins.I2C_SCL))
charlie = IS31FL3731(i2c, 0x74)
matrix_out = MatrixOutputCharlie(charlie)
matrix_buf = MatrixOutputBuffered(matrix_out)

matrix_track = Matrix(TrackMatrixOutput(matrix_buf))
matrix_cursor = Matrix(CursorMatrixOutput(matrix_buf))
matrix_sliders = Matrix(SlidersMatrixOutput(matrix_buf))


shift_out = ShiftOutput(Pins.RGB_CLK, Pins.RGB_DATA)
rgb_out = MatrixOutputRGB(shift_out, RGB_map)
rgb_buf = MatrixOutputBuffered(rgb_out)

rgb_track = Matrix(TrackRGBOutput(rgb_buf))
rgb_slot = Matrix(SlotRGBOutput(rgb_buf))


def begin():
    charlie.init()

    matrix_track.fill(0)
    matrix_cursor.fill(0)
    matrix_sliders.fill(0)
    matrix_track.commit()
    matrix_cursor.commit()
    matrix_sliders.commit()

    rgb_track.fill(0)
    rgb_slot.fill(0)
    rgb_track.commit()
    rgb_slot.commit()

    buttons.scan()
    enc_left.scan()
    enc_right.scan()
