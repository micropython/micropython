from ST7735 import TFT
from machine import SPI, Pin, Signal, I2C
from CircuitOS import InputGPIO, Display, Piezo, RGB_LED
from .Pins import *
from i2c_bm8563 import *

spiTFT: SPI = SPI(1, baudrate=27000000, polarity=0, phase=0, sck=Pin(Pins.TFT_SCK), mosi=Pin(Pins.TFT_MOSI))
tft = TFT(spiTFT, aDC=Pins.TFT_DC, aReset=Pins.TFT_RST, aCS=0)

blPin = Pin(Pins.BL, mode=Pin.OUT, value=True)
backlight = Signal(blPin, invert=True)

buttons = InputGPIO(Buttons.Pins, inverted=True)

piezo = Piezo(Pins.BUZZ)
rgb = RGB_LED((Pins.LED_R, Pins.LED_G, Pins.LED_B), True)
display = Display(tft, 160, 128)

i2c = I2C(0, sda=Pin(Pins.I2C_SDA), scl=Pin(Pins.I2C_SCL))
rtc = rtc_BM8563(i2c)


def begin():
    tft.initr()
    tft.rotation(3)
    tft.rgb(False)

    display.fill(display.Color.Black)
    display.commit()

    backlight.on()

    buttons.scan()

    rtc.begin()
