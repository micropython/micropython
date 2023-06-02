from .Pins import *
from CircuitOS import PCA95XX, InputShift, InputExpander, Piezo, Display
from machine import Pin, I2C, Signal, SPI
import st7789

spi = SPI(1, baudrate=27000000, polarity=0, phase=0)

piezo = Piezo(Pins.BUZZ_PIN)
i2c = I2C(sda=Pin(Pins.I2C_SDA, mode=Pin.OUT, pull=Pin.PULL_UP), scl=Pin(Pins.I2C_SCL, mode=Pin.OUT, pull=Pin.PULL_UP),
		  freq=100000)
expander = PCA95XX(i2c)

if expander.begin():
	buttons = InputExpander(expander)
	for btn in dir(Buttons):
		attr = getattr(Buttons, btn)
		if type(attr) != int:
			continue

		buttons.register_button(attr)


	class BBBacklight:
		def __init__(self):
			expander.pin_mode(Pins.BL_PIN_I2C, Pin.OUT)
			self.off()

		def on(self):
			expander.pin_write(Pins.BL_PIN_I2C, True)

		def off(self):
			expander.pin_write(Pins.BL_PIN_I2C, False)


	backlight = BBBacklight()

else:
	del expander
	blPin = Pin(Pins.BL_PIN_IO, mode=Pin.OUT, value=True)
	backlight = Signal(blPin)
	backlight.off()
	buttons = InputShift(Pins.SHIFT_DATA, Pins.SHIFT_CLOCK, Pins.SHIFT_LOAD, Pins.SHIFT_COUNT)

tft = st7789.ST7789(spi, 128, 128, dc=Pin(Pins.TFT_DC, Pin.OUT),
					color_order=st7789.BGR, inversion=False, rotation=2)
display = Display(tft, 128, 128)


def begin(panel_type: bool):
	global tft, display
	tft.init()

	if panel_type:
		tft.offset(0, 32)  # red screen with matte ribbon (GREENTAB128)
	else:
		pass  # orange PCB (GREENTAB3)
	# TODO - TFT with red with shiny ribbon and yellow headers NOT WORKING (also GREENTAB3 in C++ firmware)
	print("Panel type: " + str(panel_type))

	backlight.on()

	buttons.scan()
