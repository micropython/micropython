.. _samd_pinout:

Pinout for the SAMD machine modules
===================================

The assignment of device functions to pins is very flexible. The same function may be used
at different pins. The representation of the assignment choices are given by a table,
which is a subset of the MCU's Pin MUX table and is specific to each board, as the
available pin set varies. The structure of the table is the same for each board, but
the set of rows is different.

.. _samd21_pinout_table:

Adafruit ItsyBitsy M0 Express pin assignment table
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

- *Pin*     The number that is expected at machine.Pin(n), if the pin is given
            as a number. This is NOT the GPIO number, but the board pin number, as
            given in the board specific definition file.
- *GPIO*    The GPIO number.  

- *Pin Name*  The name of a Pin which is expected argument to machine.Pin("name").
- *IRQ*     The IRQ number assigned to that GPIO, used internally by Pin.irq(). When
            using Pin.irq(), different pins must use different IRQs
- *ADC*     The ADC channel assigned to the pin. When using ADC, different pins must
            not use the same ADC channel.
- *Serial*  Two columns with optional Serial signal assignments. Both may be used.
            The cell content is device #/pad #. The pad # is the respective internal
            signal of that serial device. Details below.
- *TCC/TC*  Two colums with assignments of the TCC modules for PWM. 
            The cell content is device #/output #. For PWM, devices 0, 1, and 2
            are used. The TC device pair 3/4 is used for ticks_us().
  
SAMD21 UART assignments
```````````````````````
The UART devices and signals must be chosen according to the following rules:

- The TX signal must be at a Pin with pad numbers 2 or 0, like Pin 1 with serial
  device 0 or 2.
- The RX pin may be assigned to one of the other pads.

Examples for Adafruit ItsyBitsy M0 Express:

- uart 0 at pins 0/1  This is the default UART at the RX/TX labeled pins
- uart 1 at pins 12/10
- uart 2 at pins 0/1
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
- I2C 3 at the pins SDA/SCL This is the default I2C device at the SDA/SCl labeled pin
- I2C 4 at the pins A1/A2
- I2C 5 at the pins SDA/SCL,

or other combinations.

SAMD21 SPI assignments
``````````````````````
The I2C devices and signals must be chosen according to the following rules:

- The following pad number pairs are suitable for MOSI/SCK: 0/1, 2/3, 3/1, and 0/3.
- The MISO signal must be at a Pin with a different pad number than MOSI or SCK.

Examples for Adafruit ItsyBitsy M0 Express:

- SPI 0 at pins 0/4/1
- SPI 1 at pins 11/12/13
- SPI 2 at pins 0/4/1
- SPI 3 at pins 11/12/13
- SPI 4 at Pin MOSI/MISO/SCK This is the default SPI device at the MOSI/MISO/SCK labeled pins.

or other combinations.


SAMD21 PWM assignments
``````````````````````

The TCC/TC device numbers 0, 1 and 2 can be used for PWM. Device 0 has four
channels, device 1 and 2 have two channels. So in total 3 different PWM
frequencies can be used, and 8 different duty cycle values.

The DAC output for the Adafruit ItsyBitsy M0 Express board is avalable at the pin A0.

|
|

.. _samd51_pinout_table:

Adafruit ItsyBitsy M4 Express pin assignment table
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

- *Pin*     The number that is expected at machine.Pin(n), if the pin is given
            as a number. This is NOT the GPIO number, but the board pin number, as
            given in the board specific definition file.
- *GPIO*    The GPIO number.
- *Pin Name* The name of a Pin which is expected argument to machine.Pin("name").
- *IRQ*     The IRQ number assigned to that GPIO, used internally by Pin.irq(). When
            using Pin.irq(), different pins must use different IRQs
- *ADC*     The ADC0/1 channel assigned to the pin. When using ADC, different pins must
            not use the same ADC device and channel.
- *Serial*  Two columns with optional Serial signal assignments. Both may be used.
            The cell content is device #/pad #. The pad # is the respective internal
            signal of that serial device. Details below.
- *TC*      These device are currently not assigned to Pin. the TC device pair 0/1
            is used for ticks_us().
- *PWM*     Two colums with assignments of the TCC modules for PWM
            The cell content is device #/output #. Details below.

SAMD51 UART assignments
```````````````````````
The UART devices and signals must be chosen according to the following rules:

- The TX signal must be at a Pin with pad numbers 0, like Pin 1 with serial
  device 3.
- The RX pin may be assigned to one of the other pads.

Examples for Adafruit ItsyBitsy 4 Express:

- uart 0 at pins A4/A1
- uart 1 at pins 1/0   This is the default UART at the RX/TX labeled pins
- uart 2 at pins SCL/SDA  This is the default I2C device at the SDA/SCl labeled pin
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

- SPI 1 at Pin MOSI/MISO/SCK  This is the default SPI device at the MOSI/MISO/SCK labeled pins.
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

The DAC outputs for the Adafruit ItsyBitsy M4 Express board are avalable at the pins A0 and A1.

|
|

Adafruit Feather M4 Express pin assignment table
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
 27 PB03     NEOPIXEL   9  15    -     -     5/1    6/1    -     - 
 28 PB11     FLASH_CS  12   -    -     -     4/3    5/1   0/5   1/1
 29 PB10    FLASH_SCK  10   -    -     -     4/2    5/0   0/4   1/0
 30 PA08   FLASH_MOSI   -   8    2    0/0    2/1    0/0   0/0   1/4
 31 PA09   FLASH_MISO   9   9    3    0/1    2/0    0/1   0/1   1/5
 32 PA10     FLASH_WP  10  10    -    0/2    2/2    1/0   0/2   1/6
 33 PA11   FLASH_HOLD  11  11    -    0/3    2/3    1/1   0/3   1/7
=== ==== ============ ==== ==== ==== ====== ====== ===== ===== =====

For the definition of the table columns see the explanation at the table for
Adafruit ItsyBitsy M4 Express :ref:`samd51_pinout_table`.

The default devices at the board are:

- UART 5 at pins 0/1, labeled RX/TX
- I2C 2 at pins 21/20, labeled SDA/SCL
- SPI 1 at pins 22/23/24, labeled MOSI, MISO and SCK
- DAC output on pins 14 and 15, labeled A0 and A1

|
|

SEEED XIAO pin assignment table
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

For the definition of the table columns see the explanation at the table for
Adafruit ItsyBitsy M0 Express :ref:`samd21_pinout_table`.

The default devices at the board are:

- UART 4 at pins 7/6, labeled A6_D6/A7_D7
- I2C 2 at pins 4/5, labeled A4_D4/A5_D5
- SPI 0 at pins 10/9/8, labeled A10_D10, A9_D9 and A8_D8
- DAC output on pin 0, labeled A0_D0

|
|

Adafruit Feather M0 Express pin assignment table
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
 24 PB10         MOSI  10   -     -     4/2    5/0    0/4
 25 PA12         MISO  12   -    2/0    4/0    2/0    0/6
 26 PB11          SCK  11   -     -     4/3    5/1    0/5
 27 PA06     NEOPIXEL   6   6     -     0/2    1/0     - 
 28 PA13     FLASH_CS  13   -    2/1    4/1    2/0    0/7
=== ==== ============ ==== ==== ====== ====== ====== ======

For the definition of the table columns see the explanation at the table for
Adafruit ItsyBitsy M0 Express :ref:`samd21_pinout_table`.

The default devices at the board are:

- UART 5 at pins 21/22, labeled RX/TX
- I2C 3 at pins 22/22, labeled SDA/SCL
- SPI 4 at pins 22/23/24, labeled MOSI, MISO and SCK
- DAC output on pin 14, labeled A0

|
|

Adafruit Trinket M0 pin assignment table
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

For the definition of the table columns see the explanation at the table for
Adafruit ItsyBitsy M0 Express :ref:`samd21_pinout_table`.

The default devices at the board are:

- UART 0 at pins 3/4, labeled D3/D4
- I2C 2 at pins 0/2, labeled D0/D2
- SPI 0 at pins 4/2/3, labeled D4, D2 and D0
- DAC output on pin 1, labeled D1

|
|

SAMD21 Xplained PRO pin assignment table
----------------------------------------

|

=== ==== ============ ==== ==== ====== ====== ====== ======
Pin GPIO Pin name     IRQ  ADC  Serial Serial TCC/TC TCC/TC
=== ==== ============ ==== ==== ====== ====== ====== ======
  0 PB00    EXT1_PIN3   0    -    -     5/2    7/0     - 
  1 PB01    EXT1_PIN4   1    -    -     5/3    7/1     - 
  2 PB06    EXT1_PIN5   6   14    -      -      -      - 
  3 PB07    EXT1_PIN6   7   15    -      -      -      - 
  4 PB02    EXT1_PIN7   2    -    -     5/0    6/0     - 
  5 PB03    EXT1_PIN8   3    -    -     5/1    6/1     - 
  6 PB04    EXT1_PIN9   4   12    -      -      -      - 
  7 PB05   EXT1_PIN10   5   13    -      -      -      - 
  8 PA08   EXT1_PIN11   -   16   0/0    2/0    0/0    1/2
  9 PA09   EXT1_PIN12   9   17   0/1    2/1    0/1    1/3
 10 PB09   EXT1_PIN13   9    3    -     4/1    4/1     - 
 11 PB08   EXT1_PIN14   8    2    -     4/0    4/0     - 
 12 PA05   EXT1_PIN15   5    5    -     0/1    0/1     - 
 13 PA06   EXT1_PIN16   6    6    -     0/2    1/0     - 
 14 PA04   EXT1_PIN17   4    4    -     0/0    0/0     - 
 15 PA07   EXT1_PIN18   7    7    -     0/3    1/1     - 
 16 PA10    EXT2_PIN3  10   18   0/2    2/2    1/0    0/2
 17 PA11    EXT2_PIN4  11   19   0/3    2/3    1/1    0/3
 18 PA20    EXT2_PIN5   4    -   5/2    3/2    7/0    0/4
 19 PA21    EXT2_PIN6   5    -   5/3    3/3    7/1    0/7
 20 PB12    EXT2_PIN7  12    -   4/0     -     4/0    0/6
 21 PB13    EXT2_PIN8  13    -   4/1     -     4/1    0/7
 22 PB14    EXT2_PIN9  14    -   4/2     -     5/0     - 
 23 PB15   EXT2_PIN10  15    -   4/3     -     5/1     - 
 26 PB11   EXT2_PIN13  11    -    -     4/3    5/1    0/5
 27 PB10   EXT2_PIN14  10    -    -     4/2    5/0    0/4
 28 PA17   EXT2_PIN15   1    -   1/1    3/1    2/1    0/7
 29 PA18   EXT2_PIN16   2    -   1/2    3/2    3/0    0/2
 30 PA16   EXT2_PIN17   0    -   1/0    3/0    2/0    0/6
 31 PA19   EXT2_PIN18   3    -   1/3    3/3    3/1    0/3
 32 PA02    EXT3_PIN3   2    0    -      -      -      - 
 33 PA03    EXT3_PIN4   3    1    -      -      -      - 
 35 PA15    EXT3_PIN6  15    -   2/3    4/3    3/1    0/5
 36 PA12    EXT3_PIN7  12    -   2/0    4/0    2/0    0/6
 37 PA13    EXT3_PIN8  13    -   2/1    4/1    2/0    0/7
 38 PA28    EXT3_PIN9   8    -    -      -      -      - 
 39 PA27   EXT3_PIN10  15    -    -      -      -      - 
 44 PB17   EXT3_PIN15   1    -   5/1     -     6/1    0/5
 45 PB22   EXT3_PIN16   6    -    -     5/2    7/0     - 
 46 PB16   EXT3_PIN17   9    -   5/0     -     6/0    0/4
 47 PB23   EXT3_PIN18   7    -    -     5/3    7/1     - 
=== ==== ============ ==== ==== ====== ====== ====== ======

For the definition of the table columns see the explanation at the table for
Adafruit ItsyBitsy M0 Express :ref:`samd21_pinout_table`.

There are no pins labeled for default devices on this board. DAC output 
is on pin 32, labeled EXT3_PIN3

|
|

Minisam M4 pin assignment table
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
 16 PB22         MOSI  22    -   -    1/2     5/2   7/0    -     - 
 17 PB23         MISO   7    -   -    1/3     5/3   7/1    -     - 
 18 PA01          SCK   1    -   -     -      1/1   2/1    -     - 
 19 PB03 DOTSTAR_DATA   9   15   -     -      5/1   6/1    -     - 
 20 PB02  DOTSTAR_CLK   2   14   -     -      5/0   6/0   2/2    - 
=== ==== ============ ==== ==== ==== ====== ====== ===== ===== =====

For the definition of the table columns see the explanation at the table for
Adafruit ItsyBitsy M4 Express :ref:`samd51_pinout_table`.

The default devices at the board are:

- UART 1 at pins 6/6, labeled D0/D1
- I2C 2 at pins 14/15, labeled SDA/SCL
- SPI 1 at pins 16/17/18, labeled MOSI, MISO and SCK
- DAC output on pins 0 and 4, labeled A0_D9 and A4_D13

|
|

Seeed WIO Terminal pin assignment table
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

For the definition of the table columns see the explanation at the table for
Adafruit ItsyBitsy M4 Express :ref:`samd51_pinout_table`.

There seems to be no default pin assignment for this board. 

|
|

Spakrfun SAMD51 Thing Plus pin assignment table
------------------------------------------------

|

=== ==== ============ ==== ==== ==== ====== ====== ===== ===== =====
Pin GPIO Pin name     IRQ  ADC  ADC  Serial Serial  TC    PWM   PWM
=== ==== ============ ==== ==== ==== ====== ====== ===== ===== =====
  0 PA13           D0  13    -    -    2/1    4/0   2/1   0/7   1/3
  1 PA12           D1  12    -    -    2/0    4/1   2/0   0/6   1/2
  2 PB23          RXD   7    -    -    1/3    5/3   7/1    -     - 
  3 PB22          TXD  22    -    -    1/2    5/2   7/0    -     - 
  4 PA06           D4   6    6    -     -     0/2   1/0    -     - 
  5 PA15           D5  15    -    -    2/3    4/3   3/1   2/1   1/3
  6 PA20           D6   4    -    -    5/2    3/2   7/0   1/4   0/0
  7 PA21           D7   5    -    -    5/3    3/3   7/1   1/5   0/1
  9 PA07           D9   7    7    -     -     0/3   1/1    -     - 
 10 PA18          D10   2    -    -    1/2    3/2   3/0   1/2   0/6
 11 PA16          D11   0    -    -    1/0    3/1   2/0   1/0   0/4
 12 PA19          D12   3    -    -    1/3    3/3   3/1   1/3   0/7
 13 PA17          D13   1    -    -    1/1    3/0   2/1   1/1   0/5
 14 PA02           A0   2    0    -     -      -     -     -     - 
 15 PB08           A1   8    2    0     -     4/0   4/0    -     - 
 16 PB09           A2   9    3    1     -     4/1   4/1    -     - 
 17 PA04           A3   4    4    -     -     0/0   0/0    -     - 
 18 PA05           A4   5    5    -     -     0/1   0/1    -     - 
 19 PB02           A5   2   14    -     -     5/0   6/0   2/2    - 
 20 PA22          SDA   6    -    -    3/0    5/1   4/0   1/6   0/2
 21 PA23          SCL   7    -    -    3/1    5/0   4/1   1/7   0/3
 22 PB12         MOSI  12    -    -    4/0     -    4/0   3/0   0/0
 23 PB11         MISO  12    -    -     -     4/3   5/1   0/5   1/1
 24 PB13          SCK  13    -    -    4/1     -    4/1   3/1   0/1
 25 PA08   FLASH_MOSI   -    8    2    0/0    2/1   0/0   0/0   1/4
 26 PA09    FLASH_SCK   9    9    3    0/1    2/0   0/1   0/1   1/5
 27 PA10     FLASH_CS  10   10    -    0/2    2/2   1/0   0/2   1/6
 28 PA11   FLASH_MISO  11   11    -    0/3    2/3   1/1   0/3   1/7
 29 PA30       SWDCLK  14    -    -    7/2    1/2   6/0   2/0    - 
 30 PA31        SWDIO  15    -    -    7/3    1/3   6/1   2/1    - 
=== ==== ============ ==== ==== ==== ====== ====== ===== ===== =====

For the definition of the table columns see the explanation at the table for
Adafruit ItsyBitsy M4 Express :ref:`samd51_pinout_table`.

The default devices at the board are:

- UART 1 at pins 2/3, labeled RX/TX
- I2C 5 at pins 20/21, labeled SDA/SCL
- SPI 4 at pins 22/23/24, labeled MOSI, MISO and SCK
- DAC output on pins 14 and 18, labeled A0 and A4

|

Scripts for creating the pin assignment tables
----------------------------------------------

The tables shown above were created with small Python scripts running at the target board:

SAMD21 script::

  from samd import pininfo
  from machine import Pin

  def print_entry(e, txt):
      print(txt, end=": ")
      if e == 255:
          print(" - ", end="")
      else:
          print("%d/%d" % (e >> 4, e & 0x0f), end="")

  def get_pininfo(pin):
      p = Pin(pin)
      info = pininfo(p)

      print("%3d" % pin, end=" ")
      print(repr(p), end="")
      print(" %12s" % info[0], end="")
      print(" IRQ:%2s" % (info[1] if info[1] != 255 else "-"), end="")
      print(" ADC:%2s" % (info[2] if info[2] != 255 else "-"), end="")
      print_entry(info[3], " Serial1")
      print_entry(info[4], " Serial2")
      print_entry(info[5], " PWM1" if (info[5] >> 4) < 3 else "   TC")
      print_entry(info[6], " PWM2")
      print()

  def table(num=64):
      for i in range(num):
          try:
              get_pininfo(i)
          except:
              pass

  table()  


SAMD51 script::

  from samd import pininfo
  from machine import Pin

  def print_entry(e, txt):
      print(txt, end=":")
      if e == 255:
          print(" - ", end="")
      else:
          print("%d/%d" % (e >> 4, e & 0x0f), end="")

  def get_pininfo(pin):
      p = Pin(pin)
      info = pininfo(p)

      print("%3d" % pin, end=" ")
      print(repr(p), end="")
      print(" %12s" % info[0], end="")
      print(" IRQ:%2s" % (info[1] if info[1] != 255 else "-"), end="")
      print(" ADC0:%2s" % (info[2] if info[2] != 255 else "-"), end="")
      print(" ADC1:%2s" % (info[3] if info[3] != 255 else "-"), end="")
      print_entry(info[4], " Serial1")
      print_entry(info[5], " Serial2")
      print_entry(info[6], " TC")
      print_entry(info[7], " PWM1")
      print_entry(info[8], " PWM2")
      print()

  def table(num=128):
      for i in range(num):
          try:
              get_pininfo(i)
          except:
              pass

  table()
 