# DS18x20 temperature sensor driver for MicroPython.
# MIT license; Copyright (c) 2016 Damien P. George

_CONVERT = const(0x44)
_RD_SCRATCH = const(0xbe)
_WR_SCRATCH = const(0x4e)

class DS18X20:
    def __init__(self, onewire):
        self.ow = onewire
        self.buf = bytearray(9)

    def scan(self):
        return [rom for rom in self.ow.scan() if rom[0] == 0x10 or rom[0] == 0x28]

    def convert_temp(self):
        self.ow.reset(True)
        self.ow.writebyte(self.ow.SKIP_ROM)
        self.ow.writebyte(_CONVERT)

    def read_scratch(self, rom):
        self.ow.reset(True)
        self.ow.select_rom(rom)
        self.ow.writebyte(_RD_SCRATCH)
        self.ow.readinto(self.buf)
        if self.ow.crc8(self.buf):
            raise Exception('CRC error')
        return self.buf

    def write_scratch(self, rom, buf):
        self.ow.reset(True)
        self.ow.select_rom(rom)
        self.ow.writebyte(_WR_SCRATCH)
        self.ow.write(buf)

    def read_temp(self, rom):
        buf = self.read_scratch(rom)
        if rom[0] == 0x10:
            if buf[1]:
                t = buf[0] >> 1 | 0x80
                t = -((~t + 1) & 0xff)
            else:
                t = buf[0] >> 1
            return t - 0.25 + (buf[7] - buf[6]) / buf[7]
        else:
            return (buf[1] << 8 | buf[0]) / 16
