from machine import SPI

spi = SPI(0, sck="P9_2", mosi="P9_0", miso="P9_1")
print(spi)
