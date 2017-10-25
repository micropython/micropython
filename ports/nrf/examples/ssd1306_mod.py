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

class SSD1306_I2C_Mod(SSD1306_I2C):

    def write_data(self, buf):
        buffer = bytearray([0x40]) + buf # Co=0, D/C#=1
        self.i2c.writeto(self.addr, buffer)
