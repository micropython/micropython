from machine import SPI, Pin, Signal
from CircuitOS import InputShift, EncoderInput
from CircuitOS.Matrix import *
from .Pins import *
import uasyncio

buttons = InputShift(pin_data=Pins.INP_DATA, pin_clock=Pins.INP_SCK, pin_load=Pins.INP_PL, count=1)
encoders = EncoderInput([(Pins.ENC_L1, Pins.ENC_L2), (Pins.ENC_R1, Pins.ENC_R2)])

def begin():
    buttons.scan()


async def loop():
    uasyncio.create_task(buttons.loop())
    uasyncio.create_task(encoders.loop())
    while (True):
        await uasyncio.sleep_ms(0)

# to start loop run:
# uasyncio.run(loop())
