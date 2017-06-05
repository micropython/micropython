import pyb
from pyb import Pin
from onewire import OneWire

"""

# This example assumes the ground of your DS18S20 is connected to PD0, vcc is connected to PD6
# and the data pin is connected to PD2.

>>> gnd = Pin('PD0')
>>> gnd.init(Pin.OUT_PP)
>>> gnd.low()
        
>>> vcc = Pin('PD6')
>>> vcc.init(Pin.OUT_PP)
>>> vcc.high()

>>> d = DS18S20('PD2')

# if only one DS18S20 is attached to the bus, then you don't need to 
# pass a ROM to read_temp.
>>> result = d.read_temp()
>>> print(result)
20.25


# TODO: provide example of reading a specific ROM
# TODO: provide example of read_temps()
# (these don't currently work because OneWire.search() is broken)

"""

class DS18S20(object):

    def __init__(self, pin):
        self.ow = OneWire(pin)
        self.roms = self.ow.devices()

    def read_temps(self):
        """
        Read and return the temperatures of all attached DS18S20 devices.
        """
        temps = []
        for rom in self.roms:
            temps.append(self.read_temp(rom))
        return temps

    def read_temp(self, rom=None):
        """
        Read and return the temperature of one DS18S20 device.
        Pass the 8-byte bytearray with the ROM of the specific device you want to read.
        If only one DS18S20 device is attached to the bus you may omit the rom parameter.
        """
        ow = self.ow
        ow.reset()
        self.select_rom(rom)
        ow.write_byte(0x44)  # Convert Temp
        while True:
            if ow.read_bit():
                break
        ow.reset()
        self.select_rom(rom)
        ow.write_byte(0xbe)  # Read scratch
        data = ow.read_bytes(9)
        return self.convert_temp(data)

    def convert_temp(self, data):
        """
        Convert the raw temperature data into degrees celsius and return as a float.
        """
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
        
    def select_rom(self, rom):
        if rom:
            self.ow.select(rom)
        else:
            self.ow.skip_rom()
    
