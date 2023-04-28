from CircuitOS import Matrix, IS31FL3731, MatrixOutputCharlie, InputGPIO, SingleLED
from machine import I2C, Pin
from .Pins import Pins

i2c = I2C(0, sda=Pin(Pins.I2C_SDA), scl=Pin(Pins.I2C_SCL))
charlie = IS31FL3731(i2c)
matrix_out = MatrixOutputCharlie(charlie)
matrix = Matrix(matrix_out, rotation=2)

buttons = InputGPIO([Pins.BTN], inverted=True)

led = SingleLED(Pins.LED)


def begin():
	charlie.init()

	matrix.fill(0)
	matrix.commit()

	led.set(0)

	buttons.scan()
