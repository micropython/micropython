"""
Driver for accelerometer on STM32F4 Discover board.

Assumes it's a LIS302DL MEMS device.
Sets accelerometer range at +-2g.
Returns list containing X,Y,Z axis acceleration values in 'g' units (9.8m/s^2).

See:
    STM32Cube_FW_F4_V1.1.0/Drivers/BSP/Components/lis302dl/lis302dl.h
    STM32Cube_FW_F4_V1.1.0/Drivers/BSP/Components/lis302dl/lis302dl.c
    STM32Cube_FW_F4_V1.1.0/Drivers/BSP/STM32F4-Discovery/stm32f4_discovery.c
    STM32Cube_FW_F4_V1.1.0/Drivers/BSP/STM32F4-Discovery/stm32f4_discovery.h
    STM32Cube_FW_F4_V1.1.0/Drivers/BSP/STM32F4-Discovery/stm32f4_discovery_accelerometer.c
    STM32Cube_FW_F4_V1.1.0/Drivers/BSP/STM32F4-Discovery/stm32f4_discovery_accelerometer.h
    STM32Cube_FW_F4_V1.1.0/Projects/STM32F4-Discovery/Demonstrations/Src/main.c
"""

from pyb import Pin
from pyb import SPI

READWRITE_CMD = const(0x80) 
MULTIPLEBYTE_CMD = const(0x40)
LIS302DL_WHO_AM_I_ADDR = const(0x0f)
LIS302DL_WHO_AM_I_VAL = const(0x3b)
LIS302DL_CTRL_REG1_ADDR = const(0x20)
LIS302DL_OUT_X = const(0x29)
LIS302DL_OUT_Y = const(0x2b)
LIS302DL_OUT_Z = const(0x2d)
# Configuration for 100Hz sampling rate, +-2g range
LIS302DL_CONF = const(0b01000111)

def convert_raw_to_g(x):
    if x & 0x80:
        x = x - 256
    return x * 18 / 1000

class STAccel:
    def __init__(self):
        self.cs_pin = Pin('PE3', Pin.OUT_PP, Pin.PULL_NONE)
        self.cs_pin.high()
        self.spi = SPI(1, SPI.MASTER, baudrate=328125, polarity=0, phase=1, bits=8)
        self.write_bytes(LIS302DL_CTRL_REG1_ADDR, bytearray([LIS302DL_CONF]))
        if self.read_id() != LIS302DL_WHO_AM_I_VAL:
            raise Exception('LIS302DL accelerometer not present')

    def read_bytes(self, addr, nbytes):
        if nbytes > 1:
            addr |= READWRITE_CMD | MULTIPLEBYTE_CMD
        else:
            addr |= READWRITE_CMD
        self.cs_pin.low()
        self.spi.send(addr)
        #buf = self.spi.send_recv(bytearray(nbytes * [0])) # read data, MSB first
        buf = self.spi.recv(nbytes)
        self.cs_pin.high()
        return buf

    def write_bytes(self, addr, buf):
        if len(buf) > 1:
            addr |= MULTIPLEBYTE_CMD
        self.cs_pin.low()
        self.spi.send(addr)
        for b in buf:
            self.spi.send(b)
        self.cs_pin.high()

    def read_id(self):
        return self.read_bytes(LIS302DL_WHO_AM_I_ADDR, 1)[0]

    def x(self):
        return convert_raw_to_g(self.read_bytes(LIS302DL_OUT_X, 1)[0])

    def y(self):
        return convert_raw_to_g(self.read_bytes(LIS302DL_OUT_Y, 1)[0])

    def z(self):
        return convert_raw_to_g(self.read_bytes(LIS302DL_OUT_Z, 1)[0])

    def xyz(self):
        val = self.read_bytes(LIS302DL_OUT_X, 5)
        return [convert_raw_to_g(val[0]),
                convert_raw_to_g(val[2]),
                convert_raw_to_g(val[4])]
