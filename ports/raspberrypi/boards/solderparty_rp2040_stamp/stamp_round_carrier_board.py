from board import *
import busio


_SPI = None
_UART = None
_I2C = None


D0 = GP0
SDA = D0
D1 = GP1
SCL = D1
D8 = GP8
CIPO = D8
MISO = D8
D9 = GP9
CS = D9
D10 = GP10
SCK = D10
D11 = GP11
COPI = D11
MOSI = D11
D14 = GP14
D15 = GP15
D16 = GP16
TX = D16
D17 = GP17
RX = D17
D26 = GP26
A0 = D26
D27 = GP27
A1 = D27
D28 = GP28
A2 = D28
D29 = GP29
A3 = D29
D24 = GP24
NEOPIXEL = D24
D25 = GP25
LED = D25


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
