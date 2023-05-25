from ST7735 import TFT
from machine import SPI, Pin, Signal, I2C
from CircuitOS import InputGPIO, Display, PanelST7735
from .Pins import *

spiTFT: SPI = SPI(1, baudrate=27000000, polarity=0, phase=0, sck=Pin(Pins.TFT_SCK), mosi=Pin(Pins.TFT_MOSI))
panel = PanelST7735(spiTFT, dc=Pin(Pins.TFT_DC, Pin.OUT), reset=Pin(Pins.TFT_RST, Pin.OUT), cs=Pin(Pins.JTAG1, Pin.OUT),
					rotation=1)
display = Display(panel)
blPin = Pin(Pins.BL, mode=Pin.OUT, value=True)
backlight = Signal(blPin, invert=True)

buttons = InputGPIO(Buttons.Pins, inverted=True)

i2c = I2C(0, sda=Pin(Pins.I2C_SDA), scl=Pin(Pins.I2C_SCL))


def begin():
	panel.init()

	display.fill(display.Color.Black)
	display.commit()

	backlight.on()

	buttons.scan()
