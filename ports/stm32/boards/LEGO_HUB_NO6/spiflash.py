# MicroPython driver for SPI flash
# MIT license; Copyright (c) 2022 Damien P. George

from micropython import const

_PAGE_SIZE = const(256)  # maximum bytes writable in one SPI transfer
_CMD_WRITE = const(0x02)
_CMD_READ = const(0x03)
_CMD_RDSR = const(0x05)
_CMD_WREN = const(0x06)
_CMD_WRITE_32 = const(0x12)
_CMD_READ_32 = const(0x13)
_CMD_SEC_ERASE = const(0x20)
_CMD_SEC_ERASE_32 = const(0x21)
_CMD_JEDEC_ID = const(0x9F)


class SPIFlash:
    def __init__(self, spi, cs):
        self.spi = spi
        self.cs = cs
        self.id = self._get_id()
        # flash chip on Hub No. 6 uses 32-bit addressing
        _32_bit = self.id == b"\xef\x40\x19"
        self._READ = _CMD_READ_32 if _32_bit else _CMD_READ
        self._WRITE = _CMD_WRITE_32 if _32_bit else _CMD_WRITE
        self._ERASE = _CMD_SEC_ERASE_32 if _32_bit else _CMD_SEC_ERASE

    def _get_id(self):
        self.cs(0)
        self.spi.write(bytearray([_CMD_JEDEC_ID]))
        buf = self.spi.read(3)
        self.cs(1)
        return buf

    def _wait_wel1(self):
        # wait WEL=1
        self.cs(0)
        self.spi.write(bytearray([_CMD_RDSR]))
        buf = bytearray(1)
        while True:
            self.spi.readinto(buf)
            if buf[0] & 2:
                break
        self.cs(1)

    def _wait_wip0(self):
        # wait WIP=0
        self.cs(0)
        self.spi.write(bytearray([_CMD_RDSR]))
        buf = bytearray(1)
        while True:
            self.spi.readinto(buf)
            if not (buf[0] & 1):
                break
        self.cs(1)

    def _flash_modify(self, cmd, addr, buf):
        self.cs(0)
        self.spi.write(bytearray([_CMD_WREN]))
        self.cs(1)
        self._wait_wel1()
        self.cs(0)
        self.spi.write(bytearray([cmd, addr >> 24, addr >> 16, addr >> 8, addr]))
        if buf:
            self.spi.write(buf)
        self.cs(1)
        self._wait_wip0()

    def erase_block(self, addr):
        self._flash_modify(self._ERASE, addr, None)

    def readinto(self, addr, buf):
        self.cs(0)
        self.spi.write(bytearray([self._READ, addr >> 16, addr >> 8, addr]))
        self.spi.readinto(buf)
        self.cs(1)

    def write(self, addr, buf):
        offset = addr & (_PAGE_SIZE - 1)
        remain = len(buf)
        buf = memoryview(buf)
        buf_offset = 0
        while remain:
            l = min(_PAGE_SIZE - offset, remain)
            self._flash_modify(self._WRITE, addr, buf[buf_offset : buf_offset + l])
            remain -= l
            addr += l
            buf_offset += l
            offset = 0
