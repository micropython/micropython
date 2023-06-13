#### SoftSPI
from machine import SoftSPI

spi = SoftSPI(baudrate=100000, polarity=1, phase=0, sck="P9_2", mosi="P9_0", miso="P9_1")
print(spi)
spi.init(baudrate=200000)
print(spi)
