from machine import I2C
import time


class IS31FL3731:
    ISSI_REG_CONFIG = 0x00
    ISSI_REG_CONFIG_PICTUREMODE = 0x00
    ISSI_REG_CONFIG_AUTOPLAYMODE = 0x08
    ISSI_REG_CONFIG_AUDIOPLAYMODE = 0x18

    ISSI_CONF_PICTUREMODE = 0x00
    ISSI_CONF_AUTOFRAMEMODE = 0x04
    ISSI_CONF_AUDIOMODE = 0x08

    ISSI_REG_PICTUREFRAME = 0x01

    ISSI_REG_SHUTDOWN = 0x0A
    ISSI_REG_AUDIOSYNC = 0x06

    ISSI_COMMANDREGISTER = 0xFD
    ISSI_BANK_FUNCTIONREG = 0x0B

    def __init__(self, i2c, addr=0x74):
        self.i2c = i2c
        self.addr = addr
        self.width = 16
        self.height = 9
        self.brightness = 255
        self.current_state = [[(0, 0, 0, 0)] * self.height for _ in range(self.width)]

    def init(self):
        if not self.addr in self.i2c.scan():
            print("I2C com error")
            return

        self.write_register8(self.ISSI_BANK_FUNCTIONREG, self.ISSI_REG_SHUTDOWN, 0x00)
        time.sleep_ms(10)
        self.write_register8(self.ISSI_BANK_FUNCTIONREG, self.ISSI_REG_SHUTDOWN, 0x01)
        self.write_register8(self.ISSI_BANK_FUNCTIONREG, self.ISSI_REG_CONFIG, self.ISSI_REG_CONFIG_PICTUREMODE)
        self.write_register8(self.ISSI_BANK_FUNCTIONREG, self.ISSI_REG_PICTUREFRAME, 0)
        self.select_bank(0)

        for i in range(2):
            self.i2c.writeto(self.addr, bytearray([0x24 + i * 77] + [0] * 77))

        for i in range(8):
            for j in range(18):
                self.write_register8(i, j, 0xff)

        self.audio_sync(False)

    def push(self, data):
        parts = []
        current = {"offset": -1, "size": 0}

        for i in range(144):
            y = i // self.width
            x = i - y * self.width

            if data[x][y] != self.current_state[x][y]:
                if current["offset"] == -1:
                    current["offset"] = i
                current["size"] += 1
            elif current["offset"] != -1:
                parts.append(current)
                current = {"offset": -1, "size": 0}

        if current["offset"] != -1:
            parts.append(current)

        more = []
        for part in parts:
            size = part["size"]
            if size >= 70:
                part["size"] //= 2
                more.append({"offset": part["offset"] + part["size"], "size": size - part["size"]})
        parts.extend(more)

        self.select_bank(0)
        for part in parts:
            buf = bytearray([0x24 + part["offset"]])
            for i in range(part["offset"], part["offset"] + part["size"]):
                y = i // self.width
                x = i - y * self.width

                pix = data[x][y]
                fval = sum(pix[:3]) / (3.0 * 255.0) * (float(pix[3]) / 255.0)
                fval *= (float(self.brightness) / 255.0)
                fval = fval ** 2
                buf.append(min(int(round(fval * 255.0)), 255))

            self.i2c.writeto(self.addr, buf)

            self.current_state = data

        def audio_sync(self, sync):
            if sync:
                self.write_register8(self.ISSI_BANK_FUNCTIONREG, self.ISSI_REG_AUDIOSYNC, 0x1)
            else:
                self.write_register8(self.ISSI_BANK_FUNCTIONREG, self.ISSI_REG_AUDIOSYNC, 0x0)

        def write_register8(self, bank, reg, data):
            self.select_bank(bank)
            self.i2c.writeto(self.addr, bytearray([reg, data]))

        def read_register8(self, bank, reg):
            self.select_bank(bank)
            self.i2c.writeto(self.addr, bytearray([reg]))
            return self.i2c.readfrom(self.addr, 1)[0]

        def select_bank(self, bank):
            self.i2c.writeto(self.addr, bytearray([self.ISSI_COMMANDREGISTER, bank]))

        def set_brightness(self, brightness):
            self.brightness = brightness

            inverse = [[tuple(255 - comp if idx < 3 else comp for idx, comp in enumerate(pixel))
                        for pixel in row] for row in self.current_state]

            self.current_state = inverse
            self.push(self.current_state)
