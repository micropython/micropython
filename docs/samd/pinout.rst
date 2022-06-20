.. _samd_pinout:

Pinout for the SAMD machine modules
===================================

The assignment of device functions to pins is very flexible. The same function may be used
at different pins. The representation of the assignment choices are given by a table,
which is a subset of the MCU's Pin MUX table and is specific to each board, as the
available pin set varies. The structure of the table is the same for each board, but
the set of rows is different.

Adafruit ItsyBitsy M0 Express pin assignment Table
--------------------------------------------------

|

=== ==== ============ ==== ==== ====== ====== ====== ======
Pin GPIO Pin name     IRQ  ADC  Serial Serial TCC/TC TCC/TC
=== ==== ============ ==== ==== ====== ====== ====== ======
  0 PA11           D0  11   19    0/3    2/3   1/1    0/3
  1 PA10           D1  10   18    0/2    2/2   1/0    0/2
  2 PA14           D2  14    -    2/2    4/2   3/0    0/4
  3 PA09           D3   9   17    0/1    2/1   0/1    1/3
  4 PA08           D4   -   16    0/0    2/0   0/0    1/2
  5 PA15           D5  15    -    2/3    4/3   3/1    0/5
  7 PA21           D7   5    -    5/3    3/3   7/1    0/7
  9 PA07           D9   7    7     -     0/3   1/1     -
 10 PA18          D10   2    -    1/2    3/2   3/0    0/2
 11 PA16          D11   0    -    1/0    3/0   2/0    0/6
 12 PA19          D12   3    -    1/3    3/3   3/1    0/3
 13 PA17          D13   1    -    1/1    3/1   2/1    0/7
 14 PA02           A0   2    0     -      -     -      -
 15 PB08           A1   8    2     -     4/0   4/0     -
 16 PB09           A2   9    3     -     4/1   4/1     -
 17 PA04           A3   4    4     -     0/0   0/0     -
 18 PA05           A4   5    5     -     0/1   0/1     -
 19 PB02           A5   2    -     -     5/0   6/0     -
 20 PA22          SDA   6    -    3/0    5/0   4/0    0/4
 21 PA23          SCL   7    -    3/1    5/1   4/1    0/5
 22 PB10         MOSI  10    -     -     4/2   5/0    0/4
 23 PA12         MISO  12    -    2/0    4/0   2/0    0/6
 24 PB11          SCK  11    -     -     4/3   5/1    0/5
 25 PA00  DOTSTAR_CLK   0    -     -     1/0   2/0     -
 26 PA01 DOTSTAR_DATA   1    -     -     1/1   2/1     -
 27 PB22   FLASH_MOSI   6    -     -     5/2   7/0     -
 28 PB03   FLASH_MISO   3    -     -     5/1   6/1     -
 29 PB23    FLASH_SCK   7    -     -     5/3   7/1     -
=== ==== ============ ==== ==== ====== ====== ====== ======


Description of the columns:

- Pin       The number that is expected at machine.Pin(n), if the pin is given
            as a number. This is NOT the GPIO number, but the board pin number, as
            given in the board specific definition file.
- GPIO      The GPIO number.  

- Pin Name  The name of a Pin which is expected argument to machine.Pin("name").
- IRQ       The IRQ number assigned to that GPIO, used internally by Pin.irq(). When
            using Pin.irq(), different pins must use different IRQs
- ADC       The ADC channel assigned to the pin. When using ADC, different pins must
            not use the same ADC channel.
- Serial    Two columns with optional Serial signal assignments. Both may be used.
            The cell content is device #/pad #. The pad # is the respective internal
            signal of that serial device. Details below.
- TCC/TC    Two colums with assignments of the TCC modules for PWM. 
            The cell content is device #/output #. For PWM, devices 0, 1, and 2
            are used. The TC device pair 3/4 is used for ticks_us().
  
SAMD21 UART assignments
```````````````````````
The UART devices and signals must be chosen according to the following rules:

- The TX signal must be at a Pin with pad numbers 2 or 0, like Pin 1 with serial
  device 0 or 2.
- The RX pin may be assigned to one of the other pads.

Examples for Adafruit ItsyBitsy M0 Express:

- uart 0 at pins 0/1
- uart 1 at pins 12/10
- uart 2 at pins 0/1,
- uart 3 at pins 11/13
- uart 4 at pins 2/5
- uart 5 at pins SCL/SDA

or other combinations.

SAMD21 I2C assignments
``````````````````````
The I2C devices and signals must be chosen according to the following rules:

- The SDA signal must be at a Pin with pad numbers 0.
- The SCL signal must be at a Pin with pad numbers 1.

Examples for Adafruit ItsyBitsy M0 Express:

- I2C 0 at Pin A3/A4
- I2C 1 at pins 11/13
- I2C 2 at the pins 4/3
- I2C 3 at the pins SDA/SCL
- I2C 4 at the pins A1/A2
- I2C 5 at the pins SDA/SCL,

or other combinations.

SAMD21 SPI assignments
``````````````````````
The I2C devices and signals must be chosen according to the following rules:

- The following pad number pairs are suitable for MOSI/SCK: 0/1, 2/3, 3/1, and 0/3.
- The MISO signal must be at a Pin with a different pad number than MOSI or SCK.

Examples for Adafruit ItsyBitsy M0 Express:

- SPI 4 at Pin MOSI/MISO/SCK
- SPI 3 at pins 11/12/13
- SPI 1 at pins 11/12/13
- SPI 0 at pins 0/4/1
- SPI 2 at pins 0/4/1

or other combinations.


SAMD21 PWM assignments
``````````````````````

The TCC/TC device numbers 0, 1 and 2 can be used for PWM. Device 0 has four
channels, device 1 and 2 have two channels. So in total 3 different PWM
frequencies can be used, and 8 different duty cycle values.

|
|

Adafruit ItsyBitsy M4 Express pin assignment Table
--------------------------------------------------

|

=== ==== ============ ==== ==== ==== ====== ====== ===== ===== =====
Pin GPIO Pin name     IRQ  ADC  ADC  Serial Serial  TC    PWM   PWM
=== ==== ============ ==== ==== ==== ====== ====== ===== ===== =====
  0 PA16           D0  0     -    -   1/0    3/1    2/0   1/0   0/4
  1 PA17           D1  1     -    -   1/1    3/0    2/1   1/1   0/5
  2 PA07           D2  7     7    -    -     0/3    1/1    -     - 
  3 PB22           D3 22     -    -   1/2    5/2    7/0    -     - 
  4 PA14           D4 14     -    -   2/2    4/2    3/0   2/0   1/2
  5 PA15           D5 15     -    -   2/3    4/3    3/1   2/1   1/3
  7 PA18           D7  2     -    -   1/2    3/2    3/0   1/2   0/6
  9 PA19           D9  3     -    -   1/3    3/3    3/1   1/3   0/7
 10 PA20          D10  4     -    -   5/2    3/2    7/0   1/4   0/0
 11 PA21          D11  5     -    -   5/3    3/3    7/1   1/5   0/1
 12 PA23          D12  7     -    -   3/1    5/0    4/1   1/7   0/3
 13 PA22          D13  6     -    -   3/0    5/1    4/0   1/6   0/2
 14 PA02           A0  2     0    -    -      -      -     -     - 
 15 PA05           A1  5     5    -    -     0/1    0/1    -     - 
 16 PB08           A2  8     2    0    -     4/0    4/0    -     - 
 17 PB09           A3  9     3    1    -     4/1    4/1    -     - 
 18 PA04           A4  4     4    -    -     0/0    0/0    -     - 
 19 PA06           A5  6     6    -    -     0/2    1/0    -     - 
 20 PA12          SDA 12     -    -   2/0    4/1    2/0   0/6   1/2
 21 PA13          SCL 13     -    -   2/1    4/0    2/1   0/7   1/3
 22 PA00         MOSI  0     -    -    -     1/0    2/0    -     - 
 23 PB23         MISO  7     -    -   1/3    5/3    7/1    -     - 
 24 PA01          SCK  1     -    -    -     1/1    2/1    -     - 
 25 PB02  DOTSTAR_CLK  2    14    -    -     5/0    6/0   2/2    - 
 26 PB03 DOTSTAR_DATA  9    15    -    -     5/1    6/1    -     - 
 27 PB11     FLASH_CS 12     -    -    -     4/3    5/1   0/5   1/1
 28 PB10    FLASH_SCK 10     -    -    -     4/2    5/0   0/4   1/0
 29 PA08   FLASH_MOSI  -     8    2   0/0    2/1    0/0   0/0   1/4
 30 PA09   FLASH_MISO  9     9    3   0/1    2/0    0/1   0/1   1/5
 31 PA10     FLASH_WP 10    10    -   0/2    2/2    1/0   0/2   1/6
 32 PA11   FLASH_HOLD 11    11    -   0/3    2/3    1/1   0/3   1/7
=== ==== ============ ==== ==== ==== ====== ====== ===== ===== =====


Description of the columns:

- Pin       The number that is expected at machine.Pin(n), if the pin is given
            as a number. This is NOT the GPIO number, but the board pin number, as
            given in the board specific definition file.
- GPIO      The GPIO number.
- Pin Name  The name of a Pin which is expected argument to machine.Pin("name").
- IRQ       The IRQ number assigned to that GPIO, used internally by Pin.irq(). When
            using Pin.irq(), different pins must use different IRQs
- ADC       The ADC0/1 channel assigned to the pin. When using ADC, different pins must
            not use the same ADC device and channel.
- Serial    Two columns with optional Serial signal assignments. Both may be used.
            The cell content is device #/pad #. The pad # is the respective internal
            signal of that serial device. Details below.
- TC        These device are currently not assigned to Pin. the TC device pair 0/1
            is used for ticks_us().
- PWM       Two colums with assignments of the TCC modules for PWM
            The cell content is device #/output #. Details below.

SAMD51 UART assignments
```````````````````````
The UART devices and signals must be chosen according to the following rules:

- The TX signal must be at a Pin with pad numbers 0, like Pin 1 with serial
  device 3.
- The RX pin may be assigned to one of the other pads.

Examples for Adafruit ItsyBitsy 4 Express:

- uart 0 at pins A4/A0
- uart 2 at pins SCL/SDA
- uart 1 at pins 1/0
- uart 3 at pins 0/1
- uart 4 at pins SDA/SCL
- uart 5 at pins D12/D13

or other combinations.

SAMD51 I2C assignments
``````````````````````
The I2C devices and signals must be chosen according to the following rules:

- The SDA signal must be at a Pin with pad numbers 0.
- The SCL signal must be at a Pin with pad numbers 1.

Examples for Adafruit ItsyBitsy M0 Express:

- I2C 0 at pins A3/A4
- I2C 1 at pins 0/1
- I2C 2 at the pins SDA/SCL
- I2C 3 at the pins 1/0
- I2C 4 at the pins A2/A3
- I2C 5 at the pins 12/13

or other combinations.

SAMD51 SPI assignments
``````````````````````
The I2C devices and signals must be chosen according to the following rules:

- The following pad number pairs are suitable for MOSI/SCK: 0/1 abd 3/1.
- The MISO signal must be at a Pin with a different pad number than MOSI or SCK.

Examples for Adafruit ItsyBitsy M0 Express:

- SPI 1 at Pin MOSI/MISO/SCK
- SPI 3 at pins 13/11/12
- SPI 5 at pins 12/3/13

or other combinations.


SAMD51 PWM assignments
``````````````````````

The TCC/PWM device numbers 0 through 4 can be used for PWM. Device 0 has six
channels, device 1 has four channels, device 2 has three channels and devices
3 and 4 have two channels. So in total up to 5 different PWM frequencies
can be used, and up to 17 different duty cycle values. Note that these numbers
do not apply to every board.

|
|

Adafruit Feather M4 Express pin assignment Table
------------------------------------------------

|

=== ==== ============ ==== ==== ==== ====== ====== ===== ===== =====
Pin GPIO Pin name     IRQ  ADC  ADC  Serial Serial  TC    PWM   PWM
=== ==== ============ ==== ==== ==== ====== ====== ===== ===== =====
  0 PB17           D0   1   -    -    5/1     -     6/1   3/1   0/5
  1 PB16           D1   0   -    -    5/0     -     6/0   3/0   0/4
  4 PA14           D4  14   -    -    2/2    4/2    3/0   2/0   1/2
  5 PA16           D5   0   -    -    1/0    3/1    2/0   1/0   0/4
  6 PA18           D6   2   -    -    1/2    3/2    3/0   1/2   0/6
  8 PB03           D8   9  15    -     -     5/1    6/1    -     - 
  9 PA19           D9   3   -    -    1/3    3/3    3/1   1/3   0/7
 10 PA20          D10   4   -    -    5/2    3/2    7/0   1/4   0/0
 11 PA21          D11   5   -    -    5/3    3/3    7/1   1/5   0/1
 12 PA22          D12   6   -    -    3/0    5/1    4/0   1/6   0/2
 13 PA23          D13   7   -    -    3/1    5/0    4/1   1/7   0/3
 14 PA02           A0   2   0    -     -      -      -     -     - 
 15 PA05           A1   5   5    -     -     0/1    0/1    -     - 
 16 PB08           A2   8   2    0     -     4/0    4/0    -     - 
 17 PB09           A3   9   3    1     -     4/1    4/1    -     - 
 18 PA04           A4   4   4    -     -     0/0    0/0    -     - 
 19 PB06           A5   6   -    8     -      -      -     -     - 
 20 PA13          SCL  13   -    -    2/1    4/0    2/1   0/7   1/3
 21 PA12          SDA  12   -    -    2/0    4/1    2/0   0/6   1/2
 22 PB23         MOSI   7   -    -    1/3    5/3    7/1    -     - 
 23 PB22         MISO  22   -    -    1/2    5/2    7/0    -     - 
 24 PA17          SCK   1   -    -    1/1    3/0    2/1   1/1   0/5
 25 PB01         VDIV   1  13    -     -     5/3    7/1    -     - 
 26 PA03         AREF   3  10    -     -      -      -     -     - 
 27 PB11     FLASH_CS  12   -    -     -     4/3    5/1   0/5   1/1
 28 PB10    FLASH_SCK  10   -    -     -     4/2    5/0   0/4   1/0
 29 PA08   FLASH_MOSI   -   8    2    0/0    2/1    0/0   0/0   1/4
 30 PA09   FLASH_MISO   9   9    3    0/1    2/0    0/1   0/1   1/5
 31 PA10     FLASH_WP  10  10    -    0/2    2/2    1/0   0/2   1/6
 32 PA11   FLASH_HOLD  11  11    -    0/3    2/3    1/1   0/3   1/7
=== ==== ============ ==== ==== ==== ====== ====== ===== ===== =====

|
|

SEEED XIAO pin assignment Table
-------------------------------

|

=== ==== ============ ==== ==== ====== ====== ====== ======
Pin GPIO Pin name     IRQ  ADC  Serial Serial TCC/TC TCC/TC
=== ==== ============ ==== ==== ====== ====== ====== ======
  0 PA02        A0_D0   2   0     -      -      -      - 
  1 PA04        A1_D1   4   4     -     0/0    0/0     - 
  2 PA10        A2_D2  10  18    0/2    2/2    1/0    0/2
  3 PA11        A3_D3  11  19    0/3    2/3    1/1    0/3
  4 PA08        A4_D4   -  16    0/0    2/0    0/0    1/2
  5 PA09        A5_D5   9  17    0/1    2/1    0/1    1/3
  6 PB08        A6_D6   8   2     -     4/0    4/0     - 
  7 PB09        A7_D7   9   3     -     4/1    4/1     - 
  8 PA07        A8_D8   7   7     -     0/3    1/1     - 
  9 PA05        A9_D9   5   5     -     0/1    0/1     - 
 10 PA06      A10_D10   6   6     -     0/2    1/0     - 
=== ==== ============ ==== ==== ====== ====== ====== ======

|
|

Adafruit Feather M0 Express pin assignment Table
------------------------------------------------

|

=== ==== ============ ==== ==== ====== ====== ====== ======
Pin GPIO Pin name     IRQ  ADC  Serial Serial TCC/TC TCC/TC
=== ==== ============ ==== ==== ====== ====== ====== ======
  0 PA11           D0  11  19    0/3    2/3    1/1    0/3
  1 PA10           D1  10  18    0/2    2/2    1/0    0/2
  2 PA14           D2  14   -    2/2    4/2    3/0    0/4
  3 PA09           D3   9  17    0/1    2/1    0/1    1/3
  4 PA08           D4   -  16    0/0    2/0    0/0    1/2
  5 PA15           D5  15   -    2/3    4/3    3/1    0/5
  6 PA20           D6   4   -    5/2    3/2    7/0    0/4
  7 PA21           D7   5   -    5/3    3/3    7/1    0/7
  8 PA06           D8   6   6     -     0/2    1/0     - 
  9 PA07           D9   7   7     -     0/3    1/1     - 
 10 PA18          D10   2   -    1/2    3/2    3/0    0/2
 11 PA16          D11   0   -    1/0    3/0    2/0    0/6
 12 PA19          D12   3   -    1/3    3/3    3/1    0/3
 13 PA17          D13   1   -    1/1    3/1    2/1    0/7
 14 PA02           A0   2   0     -      -      -      - 
 15 PB08           A1   8   2     -     4/0    4/0     - 
 16 PB09           A2   9   3     -     4/1    4/1     - 
 17 PA04           A3   4   4     -     0/0    0/0     - 
 18 PA05           A4   5   5     -     0/1    0/1     - 
 19 PB02           A5   2   -     -     5/0    6/0     - 
 20 PB22           TX   6   -     -     5/2    7/0     - 
 21 PB23           RX   7   -     -     5/3    7/1     - 
 22 PA23          SCL   7   -    3/1    5/1    4/1    0/5
 23 PA22          SDA   6   -    3/0    5/0    4/0    0/4
 24 PA06     NEOPIXEL   6   6     -     0/2    1/0     - 
 25 PA13     FLASH_CS  13   -    2/1    4/1    2/0    0/7
=== ==== ============ ==== ==== ====== ====== ====== ======

|
|

Adafruit Trinket M0 pin assignment Table
------------------------------------------------

|

=== ==== ============ ==== ==== ====== ====== ====== ======
Pin GPIO Pin name     IRQ  ADC  Serial Serial TCC/TC TCC/TC
=== ==== ============ ==== ==== ====== ====== ====== ======
  0 PA08           D0   -   16   0/0    2/0    0/0    1/2
  1 PA02           D1   2    0    -      -      -      - 
  2 PA09           D2   9   17   0/1    2/1    0/1    1/3
  3 PA07           D3   7    7    -     0/3    1/1     - 
  4 PA06           D4   6    6    -     0/2    1/0     - 
  5 PA00 DOTSTAR_DATA   0    -    -     1/0    2/0     - 
  6 PA01  DOTSTAR_CLK   1    -    -     1/1    2/1     - 
=== ==== ============ ==== ==== ====== ====== ====== ======

|
|

SAMD21 Xplained PRO pin assignment table
----------------------------------------

|

=== ==== ============ ==== ==== ====== ====== ====== ======
Pin GPIO Pin name     IRQ  ADC  Serial Serial TCC/TC TCC/TC
=== ==== ============ ==== ==== ====== ====== ====== ======
  0 PB00    EXT1_PIN3   0   -     -     5/2    7/0     - 
  1 PB01    EXT1_PIN4   1   -     -     5/3    7/1     - 
  2 PB06    EXT1_PIN5   6  14     -      -      -      - 
  3 PB07    EXT1_PIN6   7  15     -      -      -      - 
  4 PB02    EXT1_PIN7   2   -     -     5/0    6/0     - 
  5 PB03    EXT1_PIN8   3   -     -     5/1    6/1     - 
  6 PB04    EXT1_PIN9   4  12     -      -      -      - 
  7 PB05   EXT1_PIN10   5  13     -      -      -      - 
  8 PA08   EXT1_PIN11   -  16    0/0    2/0    0/0    1/2
  9 PA09   EXT1_PIN12   9  17    0/1    2/1    0/1    1/3
 10 PB09   EXT1_PIN13   9   3     -     4/1    4/1     - 
 11 PB08   EXT1_PIN14   8   2     -     4/0    4/0     - 
 12 PA05   EXT1_PIN15   5   5     -     0/1    0/1     - 
 13 PA06   EXT1_PIN16   6   6     -     0/2    1/0     - 
 14 PA04   EXT1_PIN17   4   4     -     0/0    0/0     - 
 15 PA07   EXT1_PIN18   7   7     -     0/3    1/1     - 
 16 PA10    EXT2_PIN3  10  18    0/2    2/2    1/0    0/2
 17 PA11    EXT2_PIN4  11  19    0/3    2/3    1/1    0/3
 18 PA20    EXT2_PIN5   4   -    5/2    3/2    7/0    0/4
 19 PA21    EXT2_PIN6   5   -    5/3    3/3    7/1    0/7
 20 PB12    EXT2_PIN7  12   -    4/0     -     4/0    0/6
 21 PB13    EXT2_PIN8  13   -    4/1     -     4/1    0/7
 22 PB14    EXT2_PIN9  14   -    4/2     -     5/0     - 
 23 PB15   EXT2_PIN10  15   -    4/3     -     5/1     - 
 26 PB11   EXT2_PIN13  11   -     -     4/3    5/1    0/5
 27 PB10   EXT2_PIN14  10   -     -     4/2    5/0    0/4
 28 PA17   EXT2_PIN15   1   -    1/1    3/1    2/1    0/7
 29 PA18   EXT2_PIN16   2   -    1/2    3/2    3/0    0/2
=== ==== ============ ==== ==== ====== ====== ====== ======

|
|

Minisam M4 pin assignment Table
-------------------------------

|

=== ==== ============ ==== ==== ==== ====== ====== ===== ===== =====
Pin GPIO Pin name     IRQ  ADC  ADC  Serial Serial  TC    PWM   PWM
=== ==== ============ ==== ==== ==== ====== ====== ===== ===== =====
  0 PA02        A0_D9   2    0   -     -       -     -     -     - 
  1 PB08       A1_D10   8    2   0     -      4/0   4/0    -     - 
  2 PB09       A2_D11   9    3   1     -      4/1   4/1    -     - 
  3 PA04       A3_D12   4    4   -     -      0/0   0/0    -     - 
  4 PA05       A4_D13   5    5   -     -      0/1   0/1    -     - 
  5 PA06           A5   6    6   -     -      0/2   1/0    -     - 
  6 PA16           D0   0    -   -    1/0     3/1   2/0   1/0   0/4
  7 PA17           D1   1    -   -    1/1     3/0   2/1   1/1   0/5
  8 PA07        A6_D2   7    7   -     -      0/3   1/1    -     - 
  9 PA19           D3   3    -   -    1/3     3/3   3/1   1/3   0/7
 10 PA20           D4   4    -   -    5/2     3/2   7/0   1/4   0/0
 11 PA21           D5   5    -   -    5/3     3/3   7/1   1/5   0/1
 12 PA00       BUTTON   0    -   -     -      1/0   2/0    -     - 
 13 PA03         AREF   3   10   -     -       -     -     -     - 
 14 PA12          SDA  12    -   -    2/0     4/1   2/0   0/6   1/2
 15 PA13          SCL  13    -   -    2/1     4/0   2/1   0/7   1/3
 16 PB03 DOTSTAR_DATA   9   15   -     -      5/1   6/1    -     - 
 17 PB02  DOTSTAR_CLK   2   14   -     -      5/0   6/0   2/2    - 
=== ==== ============ ==== ==== ==== ====== ====== ===== ===== =====

|
|

Seeed WIO Terminal pin assignment Table
---------------------------------------

|

=== ==== ============ ==== ==== ==== ====== ====== ===== ===== =====
Pin GPIO Pin name     IRQ  ADC  ADC  Serial Serial  TC    PWM   PWM
=== ==== ============ ==== ==== ==== ====== ====== ===== ===== =====
  0 PB08        A0_D0   8    2    0    -     4/0    4/0    -     - 
  1 PB09        A1_D1   9    3    1    -     4/1    4/1    -     - 
  2 PA07        A2_D2   7    7    -    -     0/3    1/1    -     - 
  3 PB04        A3_D3   4    -    6    -      -      -     -     - 
  4 PB05        A4_D4   5    -    7    -      -      -     -     - 
  5 PB06        A5_D5   6    -    8    -      -      -     -     - 
  6 PA04        A6_D6   4    4    -    -     0/0    0/0    -     - 
  7 PB07        A7_D7   7    -    9    -      -      -     -     - 
  8 PA06        A8_D8   6    6    -    -     0/2    1/0    -     - 
  9 PD08     SWITCH_X   3    -    -   7/0    6/1     -    0/1    - 
 10 PD09     SWITCH_Y   4    -    -   7/1    6/0     -    0/2    - 
 11 PD10     SWITCH_Z   5    -    -   7/2    6/2     -    0/3    - 
 12 PD12     SWITCH_B   7    -    -    -      -      -    0/5    - 
 13 PD20     SWITCH_U  10    -    -   1/2    3/2     -    1/0    - 
 14 PC26     BUTTON_1  10    -    -    -      -      -     -     - 
 15 PC27     BUTTON_2  11    -    -   1/0     -      -     -     - 
 16 PC28     BUTTON_3  12    -    -   1/1     -      -     -     - 
 17 PD11   BUZZER_CTR   6    -    -   7/3    6/3     -    0/4    - 
 18 PC14    5V_ENABLE  14    -    -   7/2    6/2     -    0/4   1/0
 19 PC15   3V3_ENABLE  15    -    -   7/3    6/3     -    0/5   1/1
=== ==== ============ ==== ==== ==== ====== ====== ===== ===== =====

