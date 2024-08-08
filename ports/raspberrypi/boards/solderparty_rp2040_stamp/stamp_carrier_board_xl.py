from board import *
import busio


_SPI = None
_UART = None
_I2C = None


SCL = GP5
SDA = GP4
SCK = GP22
CIPO = GP20
MISO = GP20
COPI = GP23
MOSI = GP23
CS = GP21
TX = GP0
RX = GP1
LED = GP3
VOLTAGE_MONITOR = A0
BATTERY = A0
USB_SWITCH = GP7
CARD_SCK = GP10
CARD_CIPO = GP8
CARD_MISO = GP8
CARD_COPI = GP11
CARD_MOSI = GP11
CARD_CS = GP9
CARD_DETECT = GP2


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
