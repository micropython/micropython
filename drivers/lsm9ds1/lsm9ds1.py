"""
The MIT License (MIT)

Copyright (c) 2013, 2014 Damien P. George

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.


LSM9DS1 - 9DOF inertial sensor of STMicro driver for MicroPython.
The sensor contains an accelerometer / gyroscope / magnetometer
Uses the internal FIFO to store up to 16 gyro/accel data, use the iter_accel_gyro generator to access it.

Example usage:

import time
from lsm9ds1 import LSM9DS1
from machine import Pin, I2C

lsm = LSM9DS1(I2C(1, scl=Pin(15), sda=Pin(14)))

while (True):
    #for g,a in lsm.iter_accel_gyro(): print(g,a)    # using fifo
    print('Accelerometer: x:{:>8.3f} y:{:>8.3f} z:{:>8.3f}'.format(*lsm.accel()))
    print('Magnetometer:  x:{:>8.3f} y:{:>8.3f} z:{:>8.3f}'.format(*lsm.magnet()))
    print('Gyroscope:     x:{:>8.3f} y:{:>8.3f} z:{:>8.3f}'.format(*lsm.gyro()))
    print("")
    time.sleep_ms(100)
"""
import array


_WHO_AM_I = const(0xF)
_CTRL_REG1_G = const(0x10)
_INT_GEN_SRC_G = const(0x14)
_OUT_TEMP = const(0x15)
_OUT_G = const(0x18)
_CTRL_REG4_G = const(0x1E)
_STATUS_REG = const(0x27)
_OUT_XL = const(0x28)
_FIFO_CTRL_REG = const(0x2E)
_FIFO_SRC = const(0x2F)
_OFFSET_REG_X_M = const(0x05)
_CTRL_REG1_M = const(0x20)
_OUT_M = const(0x28)
_SCALE_GYRO = const(((245, 0), (500, 1), (2000, 3)))
_SCALE_ACCEL = const(((2, 0), (4, 2), (8, 3), (16, 1)))


class LSM9DS1:
    def __init__(self, i2c, address_gyro=0x6B, address_magnet=0x1E):
        self.i2c = i2c
        self.address_gyro = address_gyro
        self.address_magnet = address_magnet
        # check id's of accelerometer/gyro and magnetometer
        if (self.magent_id() != b"=") or (self.gyro_id() != b"h"):
            raise OSError(
                "Invalid LSM9DS1 device, using address {}/{}".format(address_gyro, address_magnet)
            )
        # allocate scratch buffer for efficient conversions and memread op's
        self.scratch = array.array("B", [0, 0, 0, 0, 0, 0])
        self.scratch_int = array.array("h", [0, 0, 0])
        self.init_gyro_accel()
        self.init_magnetometer()

    def init_gyro_accel(self, sample_rate=6, scale_gyro=0, scale_accel=0):
        """Initalizes Gyro and Accelerator.
        sample rate: 0-6 (off, 14.9Hz, 59.5Hz, 119Hz, 238Hz, 476Hz, 952Hz)
        scale_gyro: 0-2 (245dps, 500dps, 2000dps )
        scale_accel: 0-3 (+/-2g, +/-4g, +/-8g, +-16g)
        """
        assert sample_rate <= 6, "invalid sampling rate: %d" % sample_rate
        assert scale_gyro <= 2, "invalid gyro scaling: %d" % scale_gyro
        assert scale_accel <= 3, "invalid accelerometer scaling: %d" % scale_accel

        i2c = self.i2c
        addr = self.address_gyro
        mv = memoryview(self.scratch)
        # angular control registers 1-3 / Orientation
        mv[0] = ((sample_rate & 0x07) << 5) | ((_SCALE_GYRO[scale_gyro][1] & 0x3) << 3)
        mv[1:4] = b"\x00\x00\x00"
        i2c.writeto_mem(addr, _CTRL_REG1_G, mv[:5])
        # ctrl4 - enable x,y,z, outputs, no irq latching, no 4D
        # ctrl5 - enable all axes, no decimation
        # ctrl6 - set scaling and sample rate of accel
        # ctrl7,8 - leave at default values
        # ctrl9 - FIFO enabled
        mv[0] = mv[1] = 0x38
        mv[2] = ((sample_rate & 7) << 5) | ((_SCALE_ACCEL[scale_accel][1] & 0x3) << 3)
        mv[3] = 0x00
        mv[4] = 0x4
        mv[5] = 0x2
        i2c.writeto_mem(addr, _CTRL_REG4_G, mv[:6])

        # fifo: use continous mode (overwrite old data if overflow)
        i2c.writeto_mem(addr, _FIFO_CTRL_REG, b"\x00")
        i2c.writeto_mem(addr, _FIFO_CTRL_REG, b"\xc0")

        self.scale_gyro = 32768 / _SCALE_GYRO[scale_gyro][0]
        self.scale_accel = 32768 / _SCALE_ACCEL[scale_accel][0]

    def init_magnetometer(self, sample_rate=7, scale_magnet=0):
        """
        sample rates = 0-7 (0.625, 1.25, 2.5, 5, 10, 20, 40, 80Hz)
        scaling = 0-3 (+/-4, +/-8, +/-12, +/-16 Gauss)
        """
        assert sample_rate < 8, "invalid sample rate: %d (0-7)" % sample_rate
        assert scale_magnet < 4, "invalid scaling: %d (0-3)" % scale_magnet
        i2c = self.i2c
        addr = self.address_magnet
        mv = memoryview(self.scratch)
        mv[0] = 0x40 | (sample_rate << 2)  # ctrl1: high performance mode
        mv[1] = scale_magnet << 5  # ctrl2: scale, normal mode, no reset
        mv[2] = 0x00  # ctrl3: continous conversion, no low power, I2C
        mv[3] = 0x08  # ctrl4: high performance z-axis
        mv[4] = 0x00  # ctr5: no fast read, no block update
        i2c.writeto_mem(addr, _CTRL_REG1_M, mv[:5])
        self.scale_factor_magnet = 32768 / ((scale_magnet + 1) * 4)

    def calibrate_magnet(self, offset):
        """
        offset is a magnet vecor that will be substracted by the magnetometer
        for each measurement. It is written to the magnetometer's offset register
        """
        offset = [int(i * self.scale_factor_magnet) for i in offset]
        mv = memoryview(self.scratch)
        mv[0] = offset[0] & 0xFF
        mv[1] = offset[0] >> 8
        mv[2] = offset[1] & 0xFF
        mv[3] = offset[1] >> 8
        mv[4] = offset[2] & 0xFF
        mv[5] = offset[2] >> 8
        self.i2c.writeto_mem(self.address_magnet, _OFFSET_REG_X_M, mv[:6])

    def gyro_id(self):
        return self.i2c.readfrom_mem(self.address_gyro, _WHO_AM_I, 1)

    def magent_id(self):
        return self.i2c.readfrom_mem(self.address_magnet, _WHO_AM_I, 1)

    def magnet(self):
        """Returns magnetometer vector in gauss.
        raw_values: if True, the non-scaled adc values are returned
        """
        mv = memoryview(self.scratch_int)
        f = self.scale_factor_magnet
        self.i2c.readfrom_mem_into(self.address_magnet, _OUT_M | 0x80, mv)
        return (mv[0] / f, mv[1] / f, mv[2] / f)

    def gyro(self):
        """Returns gyroscope vector in degrees/sec."""
        mv = memoryview(self.scratch_int)
        f = self.scale_gyro
        self.i2c.readfrom_mem_into(self.address_gyro, _OUT_G | 0x80, mv)
        return (mv[0] / f, mv[1] / f, mv[2] / f)

    def accel(self):
        """Returns acceleration vector in gravity units (9.81m/s^2)."""
        mv = memoryview(self.scratch_int)
        f = self.scale_accel
        self.i2c.readfrom_mem_into(self.address_gyro, _OUT_XL | 0x80, mv)
        return (mv[0] / f, mv[1] / f, mv[2] / f)

    def iter_accel_gyro(self):
        """A generator that returns tuples of (gyro,accelerometer) data from the fifo."""
        while True:
            fifo_state = int.from_bytes(
                self.i2c.readfrom_mem(self.address_gyro, _FIFO_SRC, 1), "big"
            )
            if fifo_state & 0x3F:
                # print("Available samples=%d" % (fifo_state & 0x1f))
                yield self.gyro(), self.accel()
            else:
                break
