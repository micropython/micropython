from .Pins import *
from CircuitOS import PCA95XX, InputShift, InputExpander, PanelST7735
from machine import Pin, I2C, Signal, SPI
from ST7735 import TFT

spi = SPI(1, baudrate=27000000, polarity=0, phase=0)
tft = TFT(spi, Pins.TFT_DC, -1, -1)

i2c = I2C(sda=Pin(Pins.I2C_SDA), scl=Pin(Pins.I2C_SCL))
expander = PCA95XX(i2c)

if expander.begin():
    expander.pin_mode(Pins.BL_PIN_I2C, Pin.OUT)
    expander.pin_write(Pins.BL_PIN_I2C, True)
    buttons = InputExpander(expander)

else:
    blPin = Pin(Pins.BL_PIN_IO, mode=Pin.OUT, value=True)
    backlight = Signal(blPin)
    buttons = InputShift(Pins.SHIFT_DATA, Pins.SHIFT_CLOCK, Pins.SHIFT_LOAD, Pins.SHIFT_COUNT)


def begin(panel_type: bool):
    tft.rgb(False)
    print("panel type: " + str(panel_type))
    if panel_type:
        tft.initg((2, 3))
        tft.rotation(2) #narančasti ekran
    else:
        tft.initb()


    tft.fill(TFT.RED)
    backlight.on()

    buttons.scan()
