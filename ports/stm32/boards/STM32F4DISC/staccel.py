"""
Driver for accelerometer on STM32F4 Discover board.

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

from micropython import const
from pyb import Pin
from pyb import SPI

READWRITE_CMD = const(0x80) 
MULTIPLEBYTE_CMD = const(0x40)
WHO_AM_I_ADDR = const(0x0f)
OUT_X_ADDR = const(0x29)
OUT_Y_ADDR = const(0x2b)
OUT_Z_ADDR = const(0x2d)
OUT_T_ADDR = const(0x0c)

LIS302DL_WHO_AM_I_VAL = const(0x3b)
LIS302DL_CTRL_REG1_ADDR = const(0x20)
# Configuration for 100Hz sampling rate, +-2g range
LIS302DL_CONF = const(0b01000111)

LIS3DSH_WHO_AM_I_VAL = const(0x3f)
LIS3DSH_CTRL_REG4_ADDR = const(0x20)
LIS3DSH_CTRL_REG5_ADDR = const(0x24)
# Configuration for 100Hz sampling rate, +-2g range
LIS3DSH_CTRL_REG4_CONF = const(0b01100111)
LIS3DSH_CTRL_REG5_CONF = const(0b00000000)

class STAccel:
    def __init__(self):
        self.cs_pin = Pin('PE3', Pin.OUT_PP, Pin.PULL_NONE)
        self.cs_pin.high()
        self.spi = SPI(1, SPI.MASTER, baudrate=328125, polarity=0, phase=1, bits=8)

        self.who_am_i = self.read_id()

        if self.who_am_i == LIS302DL_WHO_AM_I_VAL:
            self.write_bytes(LIS302DL_CTRL_REG1_ADDR, bytearray([LIS302DL_CONF]))
            self.sensitivity = 18
        elif self.who_am_i == LIS3DSH_WHO_AM_I_VAL:
            self.write_bytes(LIS3DSH_CTRL_REG4_ADDR, bytearray([LIS3DSH_CTRL_REG4_CONF]))
            self.write_bytes(LIS3DSH_CTRL_REG5_ADDR, bytearray([LIS3DSH_CTRL_REG5_CONF]))
            self.sensitivity = 0.06 * 256
        else:
            raise Exception('LIS302DL or LIS3DSH accelerometer not present')

    def convert_raw_to_g(self, x):
        if x & 0x80:
            x = x - 256
        return x * self.sensitivity / 1000

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
        return self.read_bytes(WHO_AM_I_ADDR, 1)[0]

    def x(self):
        return self.convert_raw_to_g(self.read_bytes(OUT_X_ADDR, 1)[0])

    def y(self):
        return self.convert_raw_to_g(self.read_bytes(OUT_Y_ADDR, 1)[0])

    def z(self):
        return self.convert_raw_to_g(self.read_bytes(OUT_Z_ADDR, 1)[0])

    def xyz(self):
        return (self.x(), self.y(), self.z())
