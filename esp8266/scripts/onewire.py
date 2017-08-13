# 1-Wire driver for MicroPython on ESP8266
# MIT license; Copyright (c) 2016 Damien P. George

import _onewire as _ow

class OneWireError(Exception):
    pass

class OneWire:
    SEARCH_ROM = const(0xf0)
    MATCH_ROM = const(0x55)
    SKIP_ROM = const(0xcc)

    def __init__(self, pin):
        self.pin = pin
        self.pin.init(pin.OPEN_DRAIN)

    def reset(self):
        return _ow.reset(self.pin)

    def readbit(self):
        return _ow.readbit(self.pin)

    def readbyte(self):
        return _ow.readbyte(self.pin)

    def read(self, count):
        buf = bytearray(count)
        for i in range(count):
            buf[i] = _ow.readbyte(self.pin)
        return buf

    def writebit(self, value):
        return _ow.writebit(self.pin, value)

    def writebyte(self, value):
        return _ow.writebyte(self.pin, value)

    def write(self, buf):
        for b in buf:
            _ow.writebyte(self.pin, b)

    def select_rom(self, rom):
        self.reset()
        self.writebyte(MATCH_ROM)
        self.write(rom)

    def scan(self):
        devices = []
        diff = 65
        rom = False
        for i in range(0xff):
            rom, diff = self._search_rom(rom, diff)
            if rom:
                devices += [rom]
            if diff == 0:
                break
        return devices

    def _search_rom(self, l_rom, diff):
        if not self.reset():
            return None, 0
        self.writebyte(SEARCH_ROM)
        if not l_rom:
            l_rom = bytearray(8)
        rom = bytearray(8)
        next_diff = 0
        i = 64
        for byte in range(8):
            r_b = 0
            for bit in range(8):
                b = self.readbit()
                if self.readbit():
                    if b: # there are no devices or there is an error on the bus
                        return None, 0
                else:
                    if not b: # collision, two devices with different bit meaning
                        if diff > i or ((l_rom[byte] & (1 << bit)) and diff != i):
                            b = 1
                            next_diff = i
                self.writebit(b)
                if b:
                    r_b |= 1 << bit
                i -= 1
            rom[byte] = r_b
        return rom, next_diff

    def crc8(self, data):
        return _ow.crc8(data)

class DS18B20:
    CONVERT = const(0x44)
    RD_SCRATCH = const(0xbe)
    WR_SCRATCH = const(0x4e)

    def __init__(self, onewire):
        self.ow = onewire

    def scan(self):
        return [rom for rom in self.ow.scan() if rom[0] == 0x28]

    def convert_temp(self):
        if not self.ow.reset():
            raise OneWireError
        self.ow.writebyte(SKIP_ROM)
        self.ow.writebyte(CONVERT)

    def read_scratch(self, rom):
        if not self.ow.reset():
            raise OneWireError
        self.ow.select_rom(rom)
        self.ow.writebyte(RD_SCRATCH)
        buf = self.ow.read(9)
        if self.ow.crc8(buf):
            raise OneWireError
        return buf

    def write_scratch(self, rom, buf):
        if not self.ow.reset():
            raise OneWireError
        self.ow.select_rom(rom)
        self.ow.writebyte(WR_SCRATCH)
        self.ow.write(buf)

    def read_temp(self, rom):
        buf = self.read_scratch(rom)
        return (buf[1] << 8 | buf[0]) / 16
