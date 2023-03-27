from ST7735 import TFT
from machine import SPI, Pin, Signal
from CircuitOS import InputShift, Piezo, Display
from .Pins import *


spiLora: SPI = SPI(1, baudrate=27000000, polarity=0, phase=0, sck=Pin(Pins.LORA_SCK), mosi=Pin(Pins.LORA_MOSI), miso=Pin(Pins.LORA_MISO))

spiTFT: SPI = SPI(2, baudrate=27000000, polarity=0, phase=0, sck=Pin(Pins.TFT_SCK), mosi=Pin(Pins.TFT_MOSI))
tft = TFT(spiTFT, aDC=Pins.TFT_DC, aReset=Pins.TFT_RST, aCS=Pins.TFT_CS)

blPin = Pin(Pins.BL, mode=Pin.OUT, value=True)
backlight = Signal(blPin, invert=True)

buttons = InputShift(dataPin=Pins.INP_DATA, clockPin=Pins.INP_SCK, loadPin=Pins.INP_PL, numShifts=2)

piezo = Piezo(Pins.BUZZ)

display = Display(tft, 160, 128)


def begin():
	tft = TFT(spiTFT, aDC=Pins.TFT_DC, aReset=Pins.TFT_RST, aCS=Pins.TFT_CS)
	tft.initr()
	tft.rotation(1)
	tft.rgb(False)

	display.fill(display.Color.Black)
	display.commit()

	backlight.on()

	buttons.scan()
