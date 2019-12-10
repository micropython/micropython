# DHT11/DHT22 driver for MicroPython on ESP8266
# MIT license; Copyright (c) 2016 Damien P. George

try:
    from esp import dht_readinto
except:
    from pyb import dht_readinto

from machine import Pin
from utime import sleep_ms

class DHTBase:
    def __init__(self, pin, delay_ms = 250, start_ms = 18):
        self.pin = pin
        self.buf = bytearray(5)
        self.delay_ms = delay_ms
        self.start_ms = start_ms
        self.pin.init(mode=Pin.OPEN_DRAIN)
        self.pin.on()

    def start(self):
        self.pin.off()
        return self.start_ms

    def receive(self):
        buf = self.buf
        dht_readinto(self.pin, buf)
        if (buf[0] + buf[1] + buf[2] + buf[3]) & 0xff != buf[4]:
            raise Exception("checksum error")

    def measure(self):
        if self.delay_ms > 0:
            self.pin.on()
            sleep_ms(self.delay_ms)
        self.start()
        sleep_ms(self.start_ms)
        self.receive()

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
