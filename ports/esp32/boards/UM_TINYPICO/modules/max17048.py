# Basic MAX17048 library for OMGS3 and other Unexpected Maker products
# MIT license; Copyright (c) 2024 Seon Rozenblum - Unexpected Maker
#
# Project home:
#   https://unexpectedmaker.com

from machine import I2C


class MAX17048:
    _MAX17048_ADDRESS = 0x36

    _VCELL_REGISTER = 0x02
    _SOC_REGISTER = 0x04
    _MODE_REGISTER = 0x06
    _VERSION_REGISTER = 0x08
    _HIBRT_REGISTER = 0x0A
    _CONFIG_REGISTER = 0x0C
    _COMMAND_REGISTER = 0xFE

    def __init__(self, i2c, address=_MAX17048_ADDRESS):
        self.i2c = i2c
        self.address = address

    def _read_register(self, register, num_bytes):
        result = self.i2c.readfrom_mem(self.address, register, num_bytes)
        return int.from_bytes(result, "big")

    def _write_register(self, register, value, num_bytes):
        data = value.to_bytes(num_bytes, "big")
        self.i2c.writeto_mem(self.address, register, data)

    @property
    def cell_voltage(self):
        """The voltage of the connected cell in Volts."""
        raw_voltage = self._read_register(self._VCELL_REGISTER, 2)
        voltage = (raw_voltage >> 4) * 0.00125
        return voltage

    @property
    def state_of_charge(self):
        """The state of charge of the battery in percentage."""
        raw_soc = self._read_register(self._SOC_REGISTER, 2)
        return raw_soc / 256

    @property
    def version(self):
        """The chip version."""
        return self._read_register(self._VERSION_REGISTER, 2)

    @property
    def hibernate(self):
        """True if the chip is in hibernate mode, False otherwise."""
        hib = self._read_register(self._HIBRT_REGISTER, 2)
        return (hib & 0x4000) != 0

    @hibernate.setter
    def hibernate(self, value):
        config = self._read_register(self._CONFIG_REGISTER, 2)
        if value:
            config |= 0x8000  # Set the sleep bit
        else:
            config &= ~0x8000  # Clear the sleep bit
        self._write_register(self._CONFIG_REGISTER, config, 2)

    def quick_start(self):
        """Perform a quick start to reset the SOC calculation in the chip."""
        self._write_register(self._MODE_REGISTER, 0x4000, 2)

    def reset(self):
        """Reset the chip."""
        self._write_register(self._COMMAND_REGISTER, 0x5400, 2)
