from ST7735 import TFT
from machine import SPI, Pin, Signal, I2C
from CircuitOS import InputGPIO, Display, PanelST7735
from .Pins import *
from .Modules.RGBLight import *
from .Modules.Flashlight import *
from .Modules.VibroFire import *
from .Modules.SolderPro import *

spiTFT: SPI = SPI(1, baudrate=27000000, polarity=0, phase=0, sck=Pin(Pins.TFT_SCK), mosi=Pin(Pins.TFT_MOSI))
tft = TFT(spiTFT, aDC=Pins.TFT_DC, aReset=Pins.TFT_RST, aCS=Pins.JTAG1)

blPin = Pin(Pins.BL, mode=Pin.OUT, value=True)
backlight = Signal(blPin, invert=True)

buttons = InputGPIO(Buttons.Pins, inverted=True)

display = Display(PanelST7735(tft), 160, 128)

i2c = I2C(0, sda=Pin(Pins.I2C_SDA), scl=Pin(Pins.I2C_SCL))



def begin():
	tft.initr()
	tft.rotation(1)
	tft.rgb(False)

	display.fill(display.Color.Black)
	display.commit()

	backlight.on()

	buttons.scan()
