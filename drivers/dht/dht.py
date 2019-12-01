# DHT11/DHT22 driver for MicroPython on ESP8266
# MIT license; Copyright (c) 2016 Damien P. George

ENABLE_HIGH = 1

try:
    from esp import dht_readinto
except:
    from pyb import dht_readinto

from machine import Pin
from utime import sleep_ms

class DHTBase:
    def __init__(self, pin, delay=250, flags=0):
        self.pin = pin
        self.buf = bytearray(5)
        self.delay = delay
        self.flags = flags
        pin.init(mode=Pin.OPEN_DRAIN, value=1)
        if delay < 250:
            sleep_ms(250 - delay)

    def measure(self):
        buf = self.buf
        if self.delay > 0:
            sleep_ms(self.delay)
        dht_readinto(self.pin, self.flags, buf)
        if (buf[0] + buf[1] + buf[2] + buf[3]) & 0xff != buf[4]:
            raise Exception("checksum error")

class DHT11(DHTBase):
    def humidity(self):
        return self.buf[0]

    def temperature(self):
        return self.buf[2]

class DHT22(DHTBase):
    def humidity(self):
        return (self.buf[0] << 8 | self.buf[1]) * 0.1

    def temperature(self):
        t = ((self.buf[2] & 0x7f) << 8 | self.buf[3]) * 0.1
        if self.buf[2] & 0x80:
            t = -t
        return t
