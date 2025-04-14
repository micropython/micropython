"""
    Communicate with devices on the i2c bus (i2c / Qwiic connector)
    0x72 Sparkfun 16x2 LCD https://www.sparkfun.com/products/16397
    0x36 MAX17048 fuel gauge https://cdn.sparkfun.com/assets/b/b/2/c/b/MAX17048.pdf
    Author: Wolf Paulus wolf@paulus.com
    MIT license; Copyright (c) 2022 wolfpaulus.com
"""
from micropython import const
from machine import Pin, I2C

_I2C_SDA = const(21)
_I2C_SCL = const(22)
_FREQ = const(400000)


def write_bytes(address: int, ba: bytearray) -> None:
    """Sends a bytearray to the given device address
    :param address:  I2C address of the device to write to
    :param ba: bytes to write
    :return: None
    """
    _i2c.writeto(address, ba)


def write_byte(address: int, data: int) -> None:
    """Sends a single byte to the device
    :param address:  I2C address of the device to write to
    :param data: the data will be cast to a byte and then send.
    :return: None
    """
    ba = bytearray(1)
    ba[0] = data
    _i2c.writeto(address, ba)


def write_cmd(address: int, command: int, value: int) -> None:
    """Sends two bytes, e.g. a command and a parameter
    :param address:  I2C address of the device to write to
    :param command: The "command" or register
    :param value: The byte to write to the I2C bus
    :return: None
    """
    ba = bytearray(2)
    ba[0] = command
    ba[1] = value
    write_bytes(address, ba)


def write_block(address: int, command: int, values: [int]) -> None:
    """Sends a command byte and a data list
    :param address:  I2C address of the device to write to
    :param command: The "command" or register
    :param values: a list of ints (cast to bytes) to write on the I2C bus.
    :return: None
    """
    ba = bytearray(len(values) + 1)
    ba[0] = command
    for i in range(len(values)):
        ba[i + 1] = values[i]
    write_bytes(address, ba)


def scan() -> []:
    """
    Scan the i2c bus for devices
    :return: list of device ids
    """
    return _i2c.scan()


def is_device_connected(address: int) -> bool:
    """
    :param address: The I2C address of the device to to look for
    :return: True, if the device is on the bus
    """
    devices = scan()
    if address in devices:
        try:
            write_byte(address, 0x0)
            return True
        except Exception as ee:
            print("Error connecting to Device: %X, %s" % (address, ee))
            pass
    return False


def read16(address: int, register: int) -> int:
    """
    :param address: The I2C address of the device to to look for
    :param register: int
    :return: register value
    """
    buffer = _i2c.readfrom_mem(address, register, 2)
    return buffer[0] << 8 | buffer[1]


_i2c = I2C(0, sda=Pin(_I2C_SDA), scl=Pin(_I2C_SCL), freq=_FREQ)
