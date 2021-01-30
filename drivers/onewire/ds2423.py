# DS12423x dual counter driver for MicroPython.
# MIT license; Copyright (c) 2021 Christofer Buff Andersson""

from micropython import const

DS2423_READ_MEMORY_COMMAND = const(0xa5)
DS2423_COUNTER_A = const(0xc0)
DS2423_COUNTER_B = const(0xe0)


class DS2423(object):

    def __init__(self, onewire):
        self.ow = onewire

    def scan(self):
        '''
        - Prints devices on bus
        - Returns list of rom's, all 1wire devices no bus
        '''
        roms = []
        i = 0
        roms = self.ow.scan()
        print("DS2423 Counters found on bus: {}".format(len(roms)))
        for rom in roms:
            print("Device id {} {}".format(i, rom))
            i += 1
        return roms

    def begin(self, adress):
        '''
        - Takes device adress, use .scan()
        - ie.  'counter.begin(bytearray(b'\x1d\x6c\xec\x0c\x00\x00\x00\x94'))'
        '''
        self.rom = adress

    def isbusy(self):
        """
        - Checks wether one of the DS2423 devices on the bus is busy
        performing a temperature conversion
        """
        return not self.ow.readbit()

    def get_count(self, counter):
        '''
        - Read counter value from ROM and updates it
        - Counter is called with literal "DS2423_COUNTER_A" or "DS2423_COUNTER_B"
        '''
        buffer = [None] * 42
        self.ow.select_rom(self.rom)
        self.ow.writebyte(DS2423_READ_MEMORY_COMMAND)
        self.ow.writebyte(DS2423_COUNTER_A if counter == "DS2423_COUNTER_A" else DS2423_COUNTER_B)
        self.ow.writebyte(0x01)
        self.ow.readinto(buffer)
        self.ow.reset()
        count = int(buffer[35])
        for i in range(34, 31, -1):
            count = (count << 8) + buffer[i]
        # ################
        # TODO CRC16 Check
        # ################
        # crcHi = buffer[41]
        # crcLo = buffer[40]
        return count
