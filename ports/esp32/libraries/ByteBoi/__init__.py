from machine import SPI, Pin, Signal, I2C
from CircuitOS import Display, PCA95XX
from .Pins import *

i2c = I2C(0, sda=Pin(Pins.I2C_SDA), scl=Pin(Pins.I2C_SCL))

expander = PCA95XX(i2c)

variant = 1
if expander.begin():
	variant = 0

init_pins(variant)
print(f"Variant: {variant}")

spi: SPI = SPI(1, baudrate=16000000, polarity=0, phase=0, sck=Pin(Pins.SPI_SCK), mosi=Pin(Pins.SPI_MOSI),
			   miso=Pin(Pins.SPI_MISO))

if variant == 0:
	from CircuitOS import PanelILI9341, InputExpander

	panel = PanelILI9341(spi, dc=Pin(Pins.TFT_DC, Pin.OUT), reset=Pin(Pins.TFT_RST, Pin.OUT),
						 cs=Pin(Pins.TFT_CS, Pin.OUT), rotation=1)
	panel.init()
	buttons = InputExpander(expander)
	for btn in dir(Buttons):
		attr = getattr(Buttons, btn)
		if type(attr) != int:
			continue

		buttons.register_button(attr)


	class BBBacklight:
		def __init__(self):
			expander.pin_mode(Pins.BL, Pin.OUT)

		def on(self):
			expander.pin_write(Pins.BL, False)

		def off(self):
			expander.pin_write(Pins.BL, True)


	backlight = BBBacklight()

else:  # variant == 1
	from CircuitOS import PanelST7789, InputShift

	spiTFT: SPI = SPI(2, baudrate=16000000, polarity=1, phase=1, sck=Pin(Pins.TFT_SCK), mosi=Pin(Pins.TFT_MOSI))
	panel = PanelST7789(spiTFT, dc=Pin(Pins.TFT_DC, Pin.OUT), reset=Pin(Pins.TFT_RST, Pin.OUT), rotation=1)
	panel.init()

	blPin = Pin(Pins.BL, mode=Pin.OUT, value=True)
	backlight = Signal(blPin, invert=True)

	buttons = InputShift(Pins.SHIFT_DAT, Pins.SHIFT_CLK, Pins.SHIFT_PL)


display = Display(panel)


def begin():
	display.fill(display.Color.Black)
	display.commit()

	backlight.on()

	buttons.scan()
