import pyb
from pyb import Pin
import onewire

class DS18S20(object):

    def __init__(self, pin):
        self.ow = onewire.OneWire(pin)
        self.roms = self.ow.devices()

    def read_temps(self):
        temps = []
        for rom in self.roms:
            temps.append(self.read_temp(rom))
        return temps
    
    def read_temp(self, rom):
        ow = self.ow
        ow.reset()
        ow.select(rom)
        ow.write(bytearray([0x44]))  # Convert Temp
        while True:
            if ow.read_bit():
                break
        ow.reset()
        ow.select(rom)
        ow.write(bytearray([0xbe]))  # Read scratch
        data = ow.read(9)
        return self.convert_temp(data)

    def convert_temp(self, data):
        temp_lsb = data[0]
        temp_msb = data[1]
        if temp_msb != 0:
            # convert negative number
            temp_read = temp_lsb >> 1 | 0x80  # truncate bit 0 by shifting, fill high bit with 1.
            temp_read = -((~temp_read + 1) & 0xff) # now convert from two's complement
        else:
            temp_read = temp_lsb >> 1  # truncate bit 0 by shifting
        count_remain = data[6]
        count_per_c = data[7]
        temp = temp_read - 0.25 + (count_per_c - count_remain) / count_per_c
        return temp
        

d = DS18S20('PD0')
print(d.read_temps())
