from machine import SPI, Pin, Signal
from CircuitOS import InputShift, Piezo, Display, PanelST7735
from .Pins import *


spiLora: SPI = SPI(1, baudrate=27000000, polarity=0, phase=0, sck=Pin(Pins.LORA_SCK), mosi=Pin(Pins.LORA_MOSI), miso=Pin(Pins.LORA_MISO))

spiTFT: SPI = SPI(2, baudrate=27000000, polarity=0, phase=0, sck=Pin(Pins.TFT_SCK), mosi=Pin(Pins.TFT_MOSI))
panel = PanelST7735(spiTFT, dc=Pin(Pins.TFT_DC, Pin.OUT), reset=Pin(Pins.TFT_RST, Pin.OUT), cs=Pin(Pins.TFT_CS, Pin.OUT), rotation=1)
display = Display(panel)
blPin = Pin(Pins.BL, mode=Pin.OUT, value=True)
backlight = Signal(blPin, invert=True)

buttons = InputShift(pin_data=Pins.INP_DATA, pin_clock=Pins.INP_SCK, pin_load=Pins.INP_PL, count=2)

piezo = Piezo(Pins.BUZZ)



def begin():
	panel.init()

	display.fill(display.Color.Black)
	display.commit()

	backlight.on()

	buttons.scan()
