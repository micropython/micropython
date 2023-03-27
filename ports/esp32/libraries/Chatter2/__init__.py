from ST7735 import TFT, TFTColor
from machine import SPI, Pin, Signal
from CircuitOS import InputShift, Piezo
from .Pins import *
import framebuf

spiLora: SPI = SPI(1, baudrate=27000000, polarity=0, phase=0, sck=Pin(Pins.LORA_SCK), mosi=Pin(Pins.LORA_MOSI), miso=Pin(Pins.LORA_MISO))

spiTFT: SPI = SPI(2, baudrate=27000000, polarity=0, phase=0, sck=Pin(Pins.TFT_SCK), mosi=Pin(Pins.TFT_MOSI))
tft = TFT(spiTFT, aDC=Pins.TFT_DC, aReset=Pins.TFT_RST, aCS=Pins.TFT_CS)

blPin = Pin(Pins.BL, mode=Pin.OUT, value=True)
backlight = Signal(blPin, invert=True)

buttons = InputShift(dataPin=Pins.INP_DATA, clockPin=Pins.INP_SCK, loadPin=Pins.INP_PL, numShifts=2)

piezo = Piezo(Pins.BUZZ)

framebuffer = bytearray(160 * 128 * 2)
frame = framebuf.FrameBuffer(framebuffer, 160, 128, framebuf.RGB565)


class Color:
	Black = 0
	Red = TFTColor(0xFF, 0x00, 0x00)
	Maroon = TFTColor(0x80, 0x00, 0x00)
	Green = TFTColor(0x00, 0xFF, 0x00)
	Forest = TFTColor(0x00, 0x80, 0x80)
	Blue = TFTColor(0x00, 0x00, 0xFF)
	Navy = TFTColor(0x00, 0x00, 0x80)
	Cyan = TFTColor(0x00, 0xFF, 0xFF)
	Yellow = TFTColor(0xFF, 0xFF, 0x00)
	Purple = TFTColor(0xFF, 0x00, 0xFF)
	White = TFTColor(0xFF, 0xFF, 0xFF)
	Gray = TFTColor(0x80, 0x80, 0x80)


def begin():
	tft = TFT(spiTFT, aDC=Pins.TFT_DC, aReset=Pins.TFT_RST, aCS=Pins.TFT_CS)
	tft.initr()
	tft.rotation(1)
	tft.rgb(False)

	backlight.on()

	buttons.scan()


def pushFrame():
	tft.image(0, 0, 160, 128, framebuffer)