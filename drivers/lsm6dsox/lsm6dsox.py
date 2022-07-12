"""
LSM6DSOX STMicro driver for MicroPython based on LSM9DS1:
Source repo: https://github.com/hoihu/projects/tree/master/raspi-hat

The MIT License (MIT)

Copyright (c) 2021 Damien P. George
Copyright (c) 2021-2022 Ibrahim Abdelkader <iabdalkader@openmv.io>

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

Basic example usage:

import time
from lsm6dsox import LSM6DSOX

from machine import Pin, SPI, I2C
# Init in I2C mode.
lsm = LSM6DSOX(I2C(0, scl=Pin(13), sda=Pin(12)))

# Or init in SPI mode.
#lsm = LSM6DSOX(SPI(5), cs_pin=Pin(10))

while (True):
    print('Accelerometer: x:{:>8.3f} y:{:>8.3f} z:{:>8.3f}'.format(*lsm.read_accel()))
    print('Gyroscope:     x:{:>8.3f} y:{:>8.3f} z:{:>8.3f}'.format(*lsm.read_gyro()))
    print("")
    time.sleep_ms(100)
"""

import array
from micropython import const


class LSM6DSOX:
    _CTRL3_C = const(0x12)
    _CTRL1_XL = const(0x10)
    _CTRL8_XL = const(0x17)
    _CTRL9_XL = const(0x18)

    _CTRL2_G = const(0x11)
    _CTRL7_G = const(0x16)

    _OUTX_L_G = const(0x22)
    _OUTX_L_XL = const(0x28)
    _MLC_STATUS = const(0x38)

    _DEFAULT_ADDR = const(0x6A)
    _WHO_AM_I_REG = const(0x0F)

    _FUNC_CFG_ACCESS = const(0x01)
    _FUNC_CFG_BANK_USER = const(0)
    _FUNC_CFG_BANK_HUB = const(1)
    _FUNC_CFG_BANK_EMBED = const(2)

    _MLC0_SRC = const(0x70)
    _MLC_INT1 = const(0x0D)
    _TAP_CFG0 = const(0x56)

    _EMB_FUNC_EN_A = const(0x04)
    _EMB_FUNC_EN_B = const(0x05)

    def __init__(
        self,
        bus,
        cs_pin=None,
        address=_DEFAULT_ADDR,
        gyro_odr=104,
        accel_odr=104,
        gyro_scale=2000,
        accel_scale=4,
        ucf=None,
    ):
        """Initalizes Gyro and Accelerator.
        accel_odr: (0, 1.6Hz, 3.33Hz, 6.66Hz, 12.5Hz, 26Hz, 52Hz, 104Hz, 208Hz, 416Hz, 888Hz)
        gyro_odr:  (0, 1.6Hz, 3.33Hz, 6.66Hz, 12.5Hz, 26Hz, 52Hz, 104Hz, 208Hz, 416Hz, 888Hz)
        gyro_scale:  (245dps, 500dps, 1000dps, 2000dps)
        accel_scale: (+/-2g, +/-4g, +/-8g, +-16g)
        ucf: MLC program to load.
        """
        self.bus = bus
        self.cs_pin = cs_pin
        self.address = address
        self._use_i2c = hasattr(self.bus, "readfrom_mem")

        if not self._use_i2c and cs_pin is None:
            raise ValueError("A CS pin must be provided in SPI mode")

        # check the id of the Accelerometer/Gyro
        if self.__read_reg(_WHO_AM_I_REG) != 108:
            raise OSError("No LSM6DS device was found at address 0x%x" % (self.address))

        # allocate scratch buffer for efficient conversions and memread op's
        self.scratch_int = array.array("h", [0, 0, 0])

        SCALE_GYRO = {250: 0, 500: 1, 1000: 2, 2000: 3}
        SCALE_ACCEL = {2: 0, 4: 2, 8: 3, 16: 1}
        # XL_HM_MODE = 0 by default. G_HM_MODE = 0 by default.
        ODR = {
            0: 0x00,
            1.6: 0x08,
            3.33: 0x09,
            6.66: 0x0A,
            12.5: 0x01,
            26: 0x02,
            52: 0x03,
            104: 0x04,
            208: 0x05,
            416: 0x06,
            888: 0x07,
        }

        gyro_odr = round(gyro_odr, 2)
        accel_odr = round(accel_odr, 2)

        # Sanity checks
        if not gyro_odr in ODR:
            raise ValueError("Invalid sampling rate: %d" % accel_odr)
        if not gyro_scale in SCALE_GYRO:
            raise ValueError("invalid gyro scaling: %d" % gyro_scale)
        if not accel_odr in ODR:
            raise ValueError("Invalid sampling rate: %d" % accel_odr)
        if not accel_scale in SCALE_ACCEL:
            raise ValueError("invalid accelerometer scaling: %d" % accel_scale)

        # Soft-reset the device.
        self.reset()

        # Load and configure MLC if UCF file is provided
        if ucf != None:
            self.load_mlc(ucf)

        # Set Gyroscope datarate and scale.
        # Note output from LPF2 second filtering stage is selected. See Figure 18.
        self.__write_reg(_CTRL1_XL, (ODR[accel_odr] << 4) | (SCALE_ACCEL[accel_scale] << 2) | 2)

        # Enable LPF2 and HPF fast-settling mode, ODR/4
        self.__write_reg(_CTRL8_XL, 0x09)

        # Set Gyroscope datarate and scale.
        self.__write_reg(_CTRL2_G, (ODR[gyro_odr] << 4) | (SCALE_GYRO[gyro_scale] << 2) | 0)

        self.gyro_scale = 32768 / gyro_scale
        self.accel_scale = 32768 / accel_scale

    def __read_reg(self, reg, size=1):
        if self._use_i2c:
            buf = self.bus.readfrom_mem(self.address, reg, size)
        else:
            try:
                self.cs_pin(0)
                self.bus.write(bytes([reg | 0x80]))
                buf = self.bus.read(size)
            finally:
                self.cs_pin(1)
        if size == 1:
            return int(buf[0])
        return [int(x) for x in buf]

    def __write_reg(self, reg, val):
        if self._use_i2c:
            self.bus.writeto_mem(self.address, reg, bytes([val]))
        else:
            try:
                self.cs_pin(0)
                self.bus.write(bytes([reg, val]))
            finally:
                self.cs_pin(1)

    def __read_reg_into(self, reg, buf):
        if self._use_i2c:
            self.bus.readfrom_mem_into(self.address, reg, buf)
        else:
            try:
                self.cs_pin(0)
                self.bus.write(bytes([reg | 0x80]))
                self.bus.readinto(buf)
            finally:
                self.cs_pin(1)

    def reset(self):
        self.__write_reg(_CTRL3_C, self.__read_reg(_CTRL3_C) | 0x1)
        for i in range(0, 10):
            if (self.__read_reg(_CTRL3_C) & 0x01) == 0:
                return
            time.sleep_ms(10)
        raise OSError("Failed to reset LSM6DS device.")

    def set_mem_bank(self, bank):
        cfg = self.__read_reg(_FUNC_CFG_ACCESS) & 0x3F
        self.__write_reg(_FUNC_CFG_ACCESS, cfg | (bank << 6))

    def set_embedded_functions(self, enable, emb_ab=None):
        self.set_mem_bank(_FUNC_CFG_BANK_EMBED)
        if enable:
            self.__write_reg(_EMB_FUNC_EN_A, emb_ab[0])
            self.__write_reg(_EMB_FUNC_EN_B, emb_ab[1])
        else:
            emb_a = self.__read_reg(_EMB_FUNC_EN_A)
            emb_b = self.__read_reg(_EMB_FUNC_EN_B)
            self.__write_reg(_EMB_FUNC_EN_A, (emb_a & 0xC7))
            self.__write_reg(_EMB_FUNC_EN_B, (emb_b & 0xE6))
            emb_ab = (emb_a, emb_b)

        self.set_mem_bank(_FUNC_CFG_BANK_USER)
        return emb_ab

    def load_mlc(self, ucf):
        # Load MLC config from file
        with open(ucf, "r") as ucf_file:
            for l in ucf_file:
                if l.startswith("Ac"):
                    v = [int(v, 16) for v in l.strip().split(" ")[1:3]]
                    self.__write_reg(v[0], v[1])

        emb_ab = self.set_embedded_functions(False)

        # Disable I3C interface
        self.__write_reg(_CTRL9_XL, self.__read_reg(_CTRL9_XL) | 0x01)

        # Enable Block Data Update
        self.__write_reg(_CTRL3_C, self.__read_reg(_CTRL3_C) | 0x40)

        # Route signals on interrupt pin 1
        self.set_mem_bank(_FUNC_CFG_BANK_EMBED)
        self.__write_reg(_MLC_INT1, self.__read_reg(_MLC_INT1) & 0x01)
        self.set_mem_bank(_FUNC_CFG_BANK_USER)

        # Configure interrupt pin mode
        self.__write_reg(_TAP_CFG0, self.__read_reg(_TAP_CFG0) | 0x41)

        self.set_embedded_functions(True, emb_ab)

    def read_mlc_output(self):
        buf = None
        if self.__read_reg(_MLC_STATUS) & 0x1:
            self.__read_reg(0x1A, size=12)
            self.set_mem_bank(_FUNC_CFG_BANK_EMBED)
            buf = self.__read_reg(_MLC0_SRC, 8)
            self.set_mem_bank(_FUNC_CFG_BANK_USER)
        return buf

    def read_gyro(self):
        """Returns gyroscope vector in degrees/sec."""
        mv = memoryview(self.scratch_int)
        f = self.gyro_scale
        self.__read_reg_into(_OUTX_L_G, mv)
        return (mv[0] / f, mv[1] / f, mv[2] / f)

    def read_accel(self):
        """Returns acceleration vector in gravity units (9.81m/s^2)."""
        mv = memoryview(self.scratch_int)
        f = self.accel_scale
        self.__read_reg_into(_OUTX_L_XL, mv)
        return (mv[0] / f, mv[1] / f, mv[2] / f)
