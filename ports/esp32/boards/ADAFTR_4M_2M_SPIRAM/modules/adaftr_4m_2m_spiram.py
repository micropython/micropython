# Adafruit ESP32-S3 Feather with 4MB Flash 2MB PSRAM MicroPython Helper Library

from micropython import const

# SPI
SPI_SCK = const(36)
SPI_MOSI = const(35)
SPI_MISO = const(37)

# UART
UART_RX = const(38)
UART_TX = const(39)

# I2C
I2C_SCL = const(4)
I2C_SDA = const(3)

# Board
POWER = const(21)
NEOPIXEL = const(33)
