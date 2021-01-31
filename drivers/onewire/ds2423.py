# DS12423x dual counter driver for MicroPython.
# MIT license; Copyright (c) 2021 Christofer "Buff" Andersson

from micropython import const

_DS2423_READ_MEMORY_COMMAND = const(0xa5)
_DS2423_COUNTER_A = const(0xc0)
_DS2423_COUNTER_B = const(0xe0)


class DS2423(object):

    def __init__(self, onewire):
        self.ow = onewire

    def scan(self):
        return [rom for rom in self.ow.scan() if rom[0] == 29]

    def begin(self, adress):
        '''
        - Takes device adress, use .scan()
        - ie.  'counter.begin(bytearray(b'\x1d\x6c\xec\x0c\x00\x00\x00\x94'))'
        or counter.begin(rom[0]) from scan function
        '''
        self.rom = adress

    def isbusy(self):
        return not self.ow.readbit()

    def get_count(self, counter):
        buf = [None] * 42
        self.ow.select_rom(self.rom)
        self.ow.writebyte(_DS2423_READ_MEMORY_COMMAND)
        self.ow.writebyte(_DS2423_COUNTER_A if counter ==
                          "DS2423_COUNTER_A" else _DS2423_COUNTER_B)
        self.ow.writebyte(0x01)
        self.ow.readinto(buf)
        self.ow.reset()
        count = int(buf[35])
        for i in range(34, 31, -1):
            count = (count << 8) + buf[i]
        # ################
        # TODO CRC16 Check
        # ################
        # crcHi = buffer[41]
        # crcLo = buffer[40]
        return count
