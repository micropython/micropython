# WRL-13990 Board Support

The Sparkfun `WRL-13990` is a very basic, no thrills, `nRF52832-QFAA` based board. It can be used as a DIL socket and eases the development of Bluetooth Low Energy or other ARM Cortex-M4 applications.

## Board Pins
An overview of all available pins is in the table below. Most of the pins can be used for PWM, UART, SPI, I2C, etc. However some pins are special like the ADC and NFC provision. Please refer to the datasheet for additional information.

- When using NFC pins take special care about configuration. See note in [datasheet](http://infocenter.nordicsemi.com/pdf/nRF52832_PS_v1.4.pdf), page 17..18. At reset the pins are set to NFC mode!
- When UART hardware handshaking is required, select the GPIO's you like and set them accordingly in the `mpconfigboard.h`.


BOARD    | CPU | REMARK
---------|-----|---------------------------------------
         | P0  | Reserved 32kHz XTAL
         | P1  | Reserved 32kHz XTAL
A0       | P2  | Alternatively ADC0_IN0
A1       | P3  | Alternatively ADC0_IN1
A2       | P4  | Alternatively ADC0_IN2
A3       | P5  | Alternatively ADC0_IN3
BTN      | P6  | Button (Low Active)
LED      | P7  | Blue LED (High Active)
         | P8  |
NFC1     | P9  | See datasheet
NFC2     | P10 | See datasheet
         | P11 |
         | P12 |
         | P13 |
         | P14 |
         | P15 |
         | P16 |
         | P17 |
         | P18 |
         | P19 |
         | P20 |
NRESET   | P21 | Can be used as GPIO, see datasheet
         | P22 |
SPI0_MOSI| P23 |
SPI0_MISO| P24 |
SPI0_SCK | P25 |
RXD      | P26 | Receive serial data (baudrate 115200)
TXD      | P27 | Transmit serial data
A4       | P28 | Alternatively ADC0_IN4
A5       | P29 | Alternatively ADC0_IN5
A6       | P30 | Alternatively ADC0_IN6
A7       | P31 | Alternatively ADC0_IN7
