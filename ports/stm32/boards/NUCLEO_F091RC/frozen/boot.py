# st-flash write build-NUCLEO_F091RC/firmware.bin 0x8000000
from micropython import const
import pyb
import time

class Encoder(object):
    def __init__(self, pin_x, pin_y, reverse, scale):
        self.reverse = reverse
        self.scale = scale
        self.forward = True
        self.pin_x = pin_x
        self.pin_y = pin_y
        self._pos = 0
        self.x_interrupt = pyb.ExtInt(pin_x, pyb.ExtInt.IRQ_RISING_FALLING, pyb.Pin.PULL_NONE, self.x_callback)
        self.y_interrupt = pyb.ExtInt(pin_y, pyb.ExtInt.IRQ_RISING_FALLING, pyb.Pin.PULL_NONE, self.y_callback)

    def x_callback(self, line):
        self.forward = self.pin_x.value() ^ self.pin_y.value() ^ self.reverse
        self._pos += 1 if self.forward else -1

    def y_callback(self, line):
        self.forward = self.pin_x.value() ^ self.pin_y.value() ^ self.reverse ^ 1
        self._pos += 1 if self.forward else -1

    @property
    def position(self):
        return self._pos*self.scale


pin_a = pyb.Pin(pyb.Pin.cpu.A6, pyb.Pin.IN, pull=pyb.Pin.PULL_NONE)
pin_b = pyb.Pin(pyb.Pin.cpu.A7, pyb.Pin.IN, pull=pyb.Pin.PULL_NONE)

encoder = Encoder(pin_a,pin_b, 1, 5)

# hardware timer for encoder use, no need for interrupt with other solutions doing it in software
#pin_a = pyb.Pin(pyb.Pin.cpu.A6, pyb.Pin.AF_PP, pull=pyb.Pin.PULL_NONE, af=pyb.Pin.AF1_TIM3)
#pin_b = pyb.Pin(pyb.Pin.cpu.A7, pyb.Pin.AF_PP, pull=pyb.Pin.PULL_NONE, af=pyb.Pin.AF1_TIM3)
#enc_timer = pyb.Timer(3, prescaler=1, period=100000)
#enc_channel = enc_timer.channel(1, pyb.Timer.ENC_AB)

while True:
#	ticks = enc_timer.counter()
	ticks = encoder.position
	if ticks % 10 == 0:
		pyb.LED(1).on()
		time.sleep_ms(50)
		pyb.LED(1).off()
	print("Counter =", ticks, ticks//5)
