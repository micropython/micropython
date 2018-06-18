# st-flash write build-NUCLEO_F091RC/firmware.bin 0x8000000


from micropython import const
import pyb
import time

pyb.LED(1).on()
