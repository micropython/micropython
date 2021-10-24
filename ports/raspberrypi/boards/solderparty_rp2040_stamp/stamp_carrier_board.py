from board import *
import busio


_SPI = None
_UART = None
_I2C = None


SCL = GP5
SDA = GP4
SCK = GP18
CIPO = GP16
MISO = GP16
COPI = GP19
MOSI = GP19
CS = GP17
TX = GP0
RX = GP1
LED = GP20
VOLTAGE_MONITOR = A0
BATTERY = A0


def SPI():
    global _SPI

    if not _SPI:
        _SPI = busio.SPI(SCK, COPI, CIPO)

    return _SPI


def UART():
    global _UART

    if not _UART:
        _UART = busio.UART(TX, RX)

    return _UART


def I2C():
    global _I2C

    if not _I2C:
        _I2C = busio.I2C(SCL, SDA)

    return _I2C
