from ST7735 import TFT
from machine import SPI, I2C, Pin
from CircuitOS import Display, PanelST7735
from .Pins import *
from .Nuvoton import *
from .LED import *
from .Motors import *
from .WheelInput import *

i2c = I2C(0, sda=Pin(Pins.I2C_SDA), scl=Pin(Pins.I2C_SCL))
nuvo = Nuvoton(i2c)

motors = Motors(nuvo)
led = LED(nuvo)
buttons = WheelInput(nuvo)

spiTFT: SPI = SPI(1, baudrate=27000000, polarity=0, phase=0, sck=Pin(Pins.TFT_SCK), mosi=Pin(Pins.TFT_MOSI))
tft = TFT(spiTFT, aDC=Pins.TFT_DC, aReset=Pins.TFT_RST, aCS=Pins.TFT_CS)

display = Display(PanelST7735(tft), 160, 128)


class WheelBacklight:
    def on(self):
        led.set_backlight(True)

    def off(self):
        led.set_backlight(False)


backlight = WheelBacklight()


def begin():
    tft.initr()
    tft.rotation(1)
    tft.rgb(False)

    display.fill(display.Color.Black)
    display.commit()

    nuvo.begin()
    backlight.on()

    buttons.scan()
