# NOTE: Modified version to align with implemented I2C API in nrf port.
#
# Examples usage of SSD1306_SPI on pca10040
#
# from machine import Pin, SPI
# from ssd1306 import SSD1306_SPI
# spi = SPI(0, baudrate=40000000)
# dc = Pin.board.PA11
# res = Pin.board.PA12
# cs = Pin.board.PA13
# disp = SSD1306_SPI(128, 64, spi, dc, res, cs)
#
#
# Example usage of SSD1306_I2C on pca10040
#
# from machine import Pin, I2C
# from ssd1306_mod import SSD1306_I2C_Mod
# i2c = I2C(0, Pin.board.PA3, Pin.board.PA4)
# disp = SSD1306_I2C_Mod(128, 64, i2c)

from ssd1306 import SSD1306_I2C

SET_COL_ADDR        = const(0x21)
SET_PAGE_ADDR       = const(0x22)

class SSD1306_I2C_Mod(SSD1306_I2C):

    def show(self):
        x0 = 0
        x1 = self.width - 1
        if self.width == 64:
            # displays with width of 64 pixels are shifted by 32
            x0 += 32
            x1 += 32
        self.write_cmd(SET_COL_ADDR)
        self.write_cmd(x0)
        self.write_cmd(x1)
        self.write_cmd(SET_PAGE_ADDR)
        self.write_cmd(0)
        self.write_cmd(self.pages - 1)

        chunk_size = 254 # 255, excluding opcode.
        num_of_chunks = len(self.buffer) // chunk_size
        leftover = len(self.buffer) - (num_of_chunks * chunk_size)

        for i in range(0, num_of_chunks):
            self.write_data(self.buffer[chunk_size*i:chunk_size*(i+1)])
        if (leftover > 0):
            self.write_data(self.buffer[chunk_size * num_of_chunks:])


    def write_data(self, buf):
        buffer = bytearray([0x40]) + buf # Co=0, D/C#=1
        self.i2c.writeto(self.addr, buffer)
