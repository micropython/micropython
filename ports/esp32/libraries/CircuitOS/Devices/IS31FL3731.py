from machine import I2C
from time import sleep_ms

from micropython import const


class IS31FL3731:
    ISSI_REG_CONFIG = const(0x00)
    ISSI_REG_CONFIG_PICTUREMODE = const(0x00)
    ISSI_REG_CONFIG_AUTOPLAYMODE = const(0x08)
    ISSI_REG_CONFIG_AUDIOPLAYMODE = const(0x18)

    ISSI_CONF_PICTUREMODE = const(0x00)
    ISSI_CONF_AUTOFRAMEMODE = const(0x04)
    ISSI_CONF_AUDIOMODE = const(0x08)

    ISSI_REG_PICTUREFRAME = const(0x01)

    ISSI_REG_SHUTDOWN = const(0x0A)
    ISSI_REG_AUDIOSYNC = const(0x06)

    ISSI_COMMANDREGISTER = const(0xFD)
    ISSI_BANK_FUNCTIONREG = const(0x0B)

    def __init__(self, i2c: I2C, addr: int = 0x74):
        self.i2c = i2c
        self.addr = addr

    def init(self):
        self.write_register(self.ISSI_BANK_FUNCTIONREG, self.ISSI_REG_SHUTDOWN, 0x00)

        sleep_ms(10)

        self.write_register(self.ISSI_BANK_FUNCTIONREG, self.ISSI_REG_SHUTDOWN, 0x01)
        self.write_register(self.ISSI_BANK_FUNCTIONREG, self.ISSI_REG_CONFIG, self.ISSI_REG_CONFIG_PICTUREMODE)
        self.write_register(self.ISSI_BANK_FUNCTIONREG, self.ISSI_REG_PICTUREFRAME, 0)

        self.select_bank(0)
        for i in range(2):
            self.write([0x24 + i * 77] + [0] * 77)

        for i in range(8):
            for j in range(18):
                self.write_register(i, j, 0xff)

        self.audio_sync(False)

    def push(self, data):
        self.select_bank(0)
        for i in range(0, 144, 64):  # Divide the data into 64-byte chunks
            chunk = data[i:i+64]
            self.write([0x24 + i] + list(chunk))

        # self.currentState = data

    def audio_sync(self, sync):
        if sync:
            self.write_register(self.ISSI_BANK_FUNCTIONREG, self.ISSI_REG_AUDIOSYNC, 0x1)
        else:
            self.write_register(self.ISSI_BANK_FUNCTIONREG, self.ISSI_REG_AUDIOSYNC, 0x0)

    def write_register(self, bank, reg, data):
        self.select_bank(bank)
        self.write([reg, data])

    def read_register(self, bank, reg):
        self.select_bank(bank)
        return self.write_read(reg, 1)[0]

    def select_bank(self, bank):
        self.write([self.ISSI_COMMANDREGISTER, bank])

    def write(self, data: [] | int):
        if type(data) is int:
            data = [data]

        self.i2c.writeto(self.addr, bytes(data))

    def read(self, nbytes: int):
        return self.i2c.readfrom(self.addr, nbytes)

    def write_read(self, data: [] | int, nbytes: int):
        self.write(data)
        sleep_ms(1)
        return self.read(nbytes)
