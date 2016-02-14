"""
LPS25 - STMicro MEMS I2C pressure and temperature sensor driver for MicroPython

http://www.st.com/web/en/resource/technical/document/datasheet/DM00066332.pdf
see AN4450 application note for additional details of implementation

Example usage:
>>> from lps25 import LPS25
>>> lps = LPS25(I2C(1, I2C.MASTER), 0x5c)
>>> lps.read_pressure()
944.4485
>>> lps.read_temperature()
21.71667
"""

class LPS25:    
    LPS_WHO_AM_I = const(0xf)
    LPS_PRESSURE_OUT = const(0x28)
    LPS_TEMP_OUT = const(0x2b)

    def __init__(self, i2c, address):
        """ 
        for the sake of simplicity, this initalizes the chip to a
        recommended default (see AN4450): 
        1Hz interval, 4uA avg current consumption 
        """
        self.i2c = i2c
        self.address = address
        if self.read_id() != b'\xbd':
            raise OSError("No LPS25 device on address {}".format(address))
        i2c.mem_write(0x05, address, 0x10)
        i2c.mem_write(0xc1, address, 0x2e)
        i2c.mem_write(0x40, address, 0x21)
        i2c.mem_write(0x90, address, 0x20)
        
    def read_id(self):
        return self.i2c.mem_read(1, self.address, LPS_WHO_AM_I)
    
    def read_pressure(self):
        """ returns pressure in hPa """
        data = int.from_bytes(self.i2c.mem_read(3, self.address, LPS_PRESSURE_OUT | 0x80))
        if data & 0x80000000: 
            data = data - 0xffffffff
        return data / 4096
            
    def read_temperature(self):
        """ return temperature in degrees celsius """
        data = int.from_bytes(self.i2c.mem_read(2, self.address, LPS_TEMP_OUT | 0x80))
        if data & 0x8000: 
            data = data - 0xffff
        return 42.5 + data/480
