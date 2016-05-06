# 1-Wire driver for MicroPython on ESP8266
# MIT license; Copyright (c) 2016 Damien P. George

import _rcswitch as _rc

class RCswitchError(Exception):
    pass

class RCswitch:
    def __init__(self, pin):
        self.pin = pin
        self.pin.init(pin.OPEN_DRAIN, pin.PULL_NONE)

##3847937
    def send(self,value):
        return _rc.send(self.pin,value)

    def readbit(self):
        return _rc.readbit(self.pin)

    def writebit(self, value):
        return _rc.writebit(self.pin, value)
