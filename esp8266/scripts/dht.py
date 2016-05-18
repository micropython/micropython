# DHT Sensor driver for MicroPython on ESP8266
# MIT license; Copyright (c) 2016 Seong-Woo Kim, Damien P. George

import _dht

class DHTError(Exception):
    pass

class DHT:

    def __init__(self, pin):
        self.pin = pin
        self.pin.init(pin.OUT)

    def read(self):
        _dht.read(self.pin)
        humidity = self.readhumi()
        temperature = self.readtemp()
        return (humidity, temperature)

    def readhumi(self):
        return _dht.readhumi()

    def readtemp(self):
        return _dht.readtemp()
