"""
    fg - Fuel Gauge
    Author: Wolf Paulus wolf@paulus.com
    MIT license; Copyright (c) 2022 wolfpaulus.com
"""
from micropython import const
import pybox_i2c as i2c

DEFAULT_ADDRESS = const(0x36)
_MAX17048_VCELL = const(0x02)
_MAX17048_SOC = const(0x04)


def is_connected(address=DEFAULT_ADDRESS):
    """
    Determine if a device is connected to the system.
    :return: True if the device is connected, otherwise False.
    :rtype: bool
    """
    return i2c.is_device_connected(address)


def voltage(address=DEFAULT_ADDRESS) -> float:
    """Current Voltage"""
    raw = i2c.read16(address, _MAX17048_VCELL)
    return raw * 78.125 / 1_000_000


def remaining(address=DEFAULT_ADDRESS) -> int:
    """remaining capacity in percentage"""
    raw = i2c.read16(address, _MAX17048_SOC)
    return raw // 256
