from ST7735 import TFT
from machine import SPI, Pin, Signal, I2C
from CircuitOS import InputExpander, Display, PanelST7735, PCA95XX
from .Pins import *


spi: SPI = SPI(1, baudrate=27000000, polarity=0, phase=0, sck=Pin(Pins.SPI_SCK), mosi=Pin(Pins.SPI_MOSI), miso=Pin(Pins.SPI_MISO))
tft = TFT(spi, aDC=Pins.TFT_DC, aReset=Pins.TFT_RST, aCS=Pins.TFT_RST)

blPin = Pin(Pins.BL, mode=Pin.OUT, value=True)
backlight = Signal(blPin, invert=True)

i2c = I2C(0, sda=Pin(Pins.I2C_SDA, Pin.OUT), scl=Pin(Pins.I2C_SCL, Pin.OUT))
expander = PCA95XX(i2c)
buttons = InputExpander(expander)
for btn in dir(Buttons):
	attr = getattr(Buttons, btn)
	if type(attr) != int:
		continue
	buttons.register_button(attr)

display = Display(PanelST7735(tft), 160, 128)


def begin():
	tft.initr()
	tft.rotation(1)
	tft.rgb(False)

	display.fill(display.Color.Black)
	display.commit()

	backlight.on()

	buttons.scan()
