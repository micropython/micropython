"""
Driver for STMicroelectronics L3GD20 gyro for MicroPython.

This driver assumes a SPI device and a chip select (CS) pin given
to the constructor.

The following example assumes that the CS is connected to the "PC1" pin
and spi bus 5 is used.

>>> from pyb import Pin
>>> cs = Pin('PC1', Pin.OUT_PP, Pin.PULL_NONE)
>>> cs.high()
>>> from pyb import SPI
>>> spi_bus_nr = 5
>>> spi = SPI(spi_bus_nr, SPI.MASTER, baudrate=328125,
              polarity=1, phase=1, bits=8)

>>> from stm_l3gd20 import L3GD20
>>> dev = L3GD20(cs, spi)

Reading angular velocity:

>>> wx = d.omega_x()
>>> print(wx)
875.0

See:
    STM32Cube_FW_F4_V1.3.0/Drivers/BSP/Components/l3gd20/l3gd20.h
    STM32Cube_FW_F4_V1.3.0/Drivers/BSP/Components/l3gd20/l3gd20.c
"""
from pyb import SPI

#
#    General SPI
#
L3GD20_READWRITE_CMD = const(0x80)
L3GD20_MULTIPLEBYTE_CMD = const(0x40)
#
# Constants for the SPI device.
#
L3GD20_WHO_AM_I_ADDR = const(0x0F)  # device identification register
L3GD20_CTRL_REG1_ADDR = const(0x20)  # Control register 1
L3GD20_CTRL_REG2_ADDR = const(0x21)  # Control register 2
L3GD20_CTRL_REG3_ADDR = const(0x22)  # Control register 3
L3GD20_CTRL_REG4_ADDR = const(0x23)  # Control register 4
L3GD20_CTRL_REG5_ADDR = const(0x24)  # Control register 5
L3GD20_REFERENCE_REG_ADDR = const(0x25)  # Reference register
L3GD20_OUT_TEMP_ADDR = const(0x26)  # Out temp register
L3GD20_STATUS_REG_ADDR = const(0x27)  # Status register
L3GD20_OUT_X_L_ADDR = const(0x28)  # Output Register X
L3GD20_OUT_X_H_ADDR = const(0x29)  # Output Register X
L3GD20_OUT_Y_L_ADDR = const(0x2A)  # Output Register Y
L3GD20_OUT_Y_H_ADDR = const(0x2B)  # Output Register Y
L3GD20_OUT_Z_L_ADDR = const(0x2C)  # Output Register Z
L3GD20_OUT_Z_H_ADDR = const(0x2D)  # Output Register Z
L3GD20_FIFO_CTRL_REG_ADDR = const(0x2E)  # Fifo control Register
L3GD20_FIFO_SRC_REG_ADDR = const(0x2F)  # Fifo src Register
L3GD20_INT1_CFG_ADDR = const(0x30)  # Interrupt 1 configuration
L3GD20_INT1_SRC_ADDR = const(0x31)  # Interrupt 1 source
L3GD20_INT1_TSH_XH_ADDR = const(0x32)  # Interrupt 1 Threshold X
L3GD20_INT1_TSH_XL_ADDR = const(0x33)  # Interrupt 1 Threshold X
L3GD20_INT1_TSH_YH_ADDR = const(0x34)  # Interrupt 1 Threshold Y
L3GD20_INT1_TSH_YL_ADDR = const(0x35)  # Interrupt 1 Threshold Y
L3GD20_INT1_TSH_ZH_ADDR = const(0x36)  # Interrupt 1 Threshold Z
L3GD20_INT1_TSH_ZL_ADDR = const(0x37)  # Interrupt 1 Threshold Z
L3GD20_INT1_DURATION_ADDR = const(0x38)  # Interrupt 1 DURATION
L3GD20_I_AM_L3GD20 = const(0xD4)
L3GD20_I_AM_L3GD20_TR = const(0xD5)
L3GD20_MODE_POWERDOWN = const(0x00)
L3GD20_MODE_ACTIVE = const(0x08)
L3GD20_OUTPUT_DATARATE_1 = const(0x00)
L3GD20_OUTPUT_DATARATE_2 = const(0x40)
L3GD20_OUTPUT_DATARATE_3 = const(0x80)
L3GD20_OUTPUT_DATARATE_4 = const(0xC0)
L3GD20_X_ENABLE = const(0x02)
L3GD20_Y_ENABLE = const(0x01)
L3GD20_Z_ENABLE = const(0x04)
L3GD20_AXES_ENABLE = const(0x07)
L3GD20_AXES_DISABLE = const(0x00)
L3GD20_BANDWIDTH_1 = const(0x00)
L3GD20_BANDWIDTH_2 = const(0x10)
L3GD20_BANDWIDTH_3 = const(0x20)
L3GD20_BANDWIDTH_4 = const(0x30)
L3GD20_FULLSCALE_250 = const(0x00)
L3GD20_FULLSCALE_500 = const(0x10)
L3GD20_FULLSCALE_2000 = const(0x20)
L3GD20_FULLSCALE_SELECTION = const(0x30)
# Gyroscope sensitivity with 250 dps full scale [DPS/LSB]
L3GD20_SENSITIVITY_250DPS = 8.75
# Gyroscope sensitivity with 500 dps full scale [DPS/LSB]
L3GD20_SENSITIVITY_500DPS = 17.50
# Gyroscope sensitivity with 2000 dps full scale [DPS/LSB]
L3GD20_SENSITIVITY_2000DPS = 70.00
L3GD20_BDU_CONTINOUS = const(0x00)
L3GD20_BDU_SINGLE = const(0x80)
L3GD20_SIM_4WIRE = const(0x00)
L3GD20_SIM_3WIRE = const(0x01)
L3GD20_BLE_LSB = const(0x00)
L3GD20_BLE_MSB = const(0x40)
L3GD20_FIFO_ENABLE = const(0x40)
L3GD20_FIFO_DISABLE = const(0x00)
L3GD20_HIGHPASSFILTER_DISABLE = const(0x00)
L3GD20_HIGHPASSFILTER_ENABLE = const(0x10)
L3GD20_INT1 = const(0x00)
L3GD20_INT2 = const(0x01)
L3GD20_INT1INTERRUPT_DISABLE = const(0x00)
L3GD20_INT1INTERRUPT_ENABLE = const(0x80)
L3GD20_INT2INTERRUPT_DISABLE = const(0x00)
L3GD20_INT2INTERRUPT_ENABLE = const(0x08)
L3GD20_INT1INTERRUPT_LOW_EDGE = const(0x20)
L3GD20_INT1INTERRUPT_HIGH_EDGE = const(0x00)
L3GD20_INT1_SEL_0 = const(0x00)
L3GD20_INT1_SEL_1 = const(0x04)
L3GD20_INT1_SEL_2 = const(0x08)
L3GD20_INT1_SEL_3 = const(0x0C)
L3GD20_INT2_SEL_0 = const(0x00)
L3GD20_INT2_SEL_1 = const(0x01)
L3GD20_INT2_SEL_2 = const(0x02)
L3GD20_INT2_SEL_3 = const(0x03)
L3GD20_BOOT_NORMALMODE = const(0x00)
L3GD20_BOOT_REBOOTMEMORY = const(0x80)
L3GD20_HPM_NORMAL_MODE_RES = const(0x00)
L3GD20_HPM_REF_SIGNAL = const(0x10)
L3GD20_HPM_NORMAL_MODE = const(0x20)
L3GD20_HPM_AUTORESET_INT = const(0x30)
L3GD20_HPFCF_0 = const(0x00)
L3GD20_HPFCF_1 = const(0x01)
L3GD20_HPFCF_2 = const(0x02)
L3GD20_HPFCF_3 = const(0x03)
L3GD20_HPFCF_4 = const(0x04)
L3GD20_HPFCF_5 = const(0x05)
L3GD20_HPFCF_6 = const(0x06)
L3GD20_HPFCF_7 = const(0x07)
L3GD20_HPFCF_8 = const(0x08)
L3GD20_HPFCF_9 = const(0x09)
#
# Default configuration:
# Output data rate  190 Hz
# Bandwidth/Cut off  50 Hz
L3GD20_CTRL_REG1_VAL = const(L3GD20_OUTPUT_DATARATE_1 |
                             L3GD20_BANDWIDTH_1 |
                             L3GD20_MODE_ACTIVE |
                             L3GD20_X_ENABLE |
                             L3GD20_Y_ENABLE |
                             L3GD20_Z_ENABLE)
# Normal Mode
# Highpass cut-off 0.018Hz
L3GD20_CTRL_REG2_VAL = const(L3GD20_HPM_NORMAL_MODE_RES | L3GD20_HPFCF_9)
# No interrupts
L3GD20_CTRL_REG3_VAL = const(L3GD20_INT1INTERRUPT_DISABLE |
                             L3GD20_INT2INTERRUPT_DISABLE)
# Continous block update
# Little endinan data
# Full scale is 250 dps (degrees per second)
# Use 4 wire SPI serial interface mode
L3GD20_CTRL_REG4_VAL = const(L3GD20_BDU_CONTINOUS |
                             L3GD20_BLE_LSB |
                             L3GD20_FULLSCALE_250 |
                             L3GD20_SIM_4WIRE)
# Normal Mode
# FIFO is enabled
# No Highpass
# Int1/int2:00
#
L3GD20_CTRL_REG5_VAL = const(L3GD20_BOOT_NORMALMODE |
                             L3GD20_FIFO_ENABLE |
                             L3GD20_HIGHPASSFILTER_DISABLE |
                             L3GD20_INT1_SEL_0 |
                             L3GD20_INT2_SEL_0)


class L3GD20:
    #
    # Debug
    #
    DEFAULT_CONF = [
        (L3GD20_CTRL_REG1_ADDR, L3GD20_CTRL_REG1_VAL),
        (L3GD20_CTRL_REG2_ADDR, L3GD20_CTRL_REG2_VAL),
        (L3GD20_CTRL_REG3_ADDR, L3GD20_CTRL_REG3_VAL),
        (L3GD20_CTRL_REG4_ADDR, L3GD20_CTRL_REG4_VAL),
        (L3GD20_CTRL_REG5_ADDR, L3GD20_CTRL_REG5_VAL)]
    DEBUG = False

    def __init__(self, cs_pin, ser_dev):
        """
        Create a L3GD20 device.
        """
        if not isinstance(ser_dev, SPI):
            raise ValueError("Only SPI devices are supported as ser_dev")
        self._conf = {}
        self._cs_pin = cs_pin
        self._ser = ser_dev
        who_am_i = self._read_id()
        if who_am_i == L3GD20_I_AM_L3GD20:
            for addr, val in self.DEFAULT_CONF:
                self._conf[addr] = val
                self._write_bytes(addr, bytearray([val, ]))
                if self.DEBUG:
                    print("Set 0x%02x to 0x%02x" % (addr, val))
        else:
            raise Exception('L3GD20 gyro not present')
        self._update_dps_fs()

    def _update_dps_fs(self):
        conv = {
            L3GD20_FULLSCALE_250: L3GD20_SENSITIVITY_250DPS,
            L3GD20_FULLSCALE_500: L3GD20_SENSITIVITY_500DPS,
            L3GD20_FULLSCALE_2000: L3GD20_SENSITIVITY_2000DPS,
            L3GD20_FULLSCALE_SELECTION: L3GD20_SENSITIVITY_2000DPS}
        entry = self._conf[L3GD20_CTRL_REG4_ADDR] & L3GD20_FULLSCALE_SELECTION
        self._sensitivity = conv[entry]

    def _convert_raw_to_dps(self, x):
        # Convert integer values read from the device to a
        # degree per second value
        x = x[1]*256+x[0]
        if x & 0x8000:
            x = x - 65536
        return x * self._sensitivity

    def _read_bytes(self, addr, nbytes):
        addr |= L3GD20_READWRITE_CMD
        if nbytes > 1:
            addr |= L3GD20_MULTIPLEBYTE_CMD
        self._cs_pin.low()
        self._ser.send(addr)
        if self.DEBUG:
            print("SPI read addr: ", addr)
        buf = self._ser.recv(nbytes)
        self._cs_pin.high()
        if self.DEBUG:
            print("SPI read data", buf)
        return buf

    def _write_bytes(self, addr, buf):
        """
        Write byte to a certain address.
        SIDE EFFECT: If data is written to
        L3GD20_CTRL_REG4_ADDR the conversion factor is recalculated
        based on the register value.
        """
        if len(buf) > 1:
            addr |= L3GD20_MULTIPLEBYTE_CMD
        self._cs_pin.low()
        self._ser.send(addr)
        if self.DEBUG:
            print("SPI write addr: ", addr)
        for b in buf:
            self._ser.send(b)
            if self.DEBUG:
                print("SPI write data: ", b)
        self._cs_pin.high()
        if addr == L3GD20_CTRL_REG4_ADDR:
            self._update_dps_fs()

    def _read_id(self):
        return self._read_bytes(L3GD20_WHO_AM_I_ADDR, 1)[0]

    def omega_x(self):
        """
        Get angular velocity around x axis in degree per second.
        """
        return self._convert_raw_to_dps(self._read_bytes(L3GD20_OUT_X_L_ADDR,
                                        2))

    def omega_y(self):
        """
        Get angular velocity around y axis.in degree per second.
        """
        return self._convert_raw_to_dps(self._read_bytes(L3GD20_OUT_Y_L_ADDR,
                                        2))

    def omega_z(self):
        """
        Get angular velocity around z axis in degree per second.
        """
        return self._convert_raw_to_dps(self._read_bytes(L3GD20_OUT_Z_L_ADDR,
                                        2))

    def omega_xyz(self):
        """
        Get tuple of all angular velocities in degree per second.
        """
        return (self.omega_x(), self.omega_y(), self.omega_z())

    def temperature(self):
        """
        Get temperatur of device as integer degree celsius.
        """
        val = self._read_bytes(L3GD20_OUT_TEMP_ADDR, 1)[0]
        if val & 0x80:
            val -= 256
        return val
