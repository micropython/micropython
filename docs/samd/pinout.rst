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

=== ==== ============ ==== ==== ====== ====== ====== ======
Pin GPIO Pin name     IRQ  ADC  Serial Serial TCC/TC TCC/TC
=== ==== ============ ==== ==== ====== ====== ====== ======
  2 PA02           A0   2   0     -       -     -      -
 40 PB08           A1   8   2     -      4/0   4/0     -
 41 PB09           A2   9   3     -      4/1   4/1     -
  4 PA04           A3   4   4     -      0/0   0/0     -
  5 PA05           A4   5   5     -      0/1   0/1     -
 34 PB02           A5   2  10     -      5/0   6/0     -
 11 PA11           D0  11  19    0/3     2/3   1/1    0/3
 10 PA10           D1  10  18    0/2     2/2   1/0    0/2
 14 PA14           D2  14   -    2/2     4/2   3/0    0/4
  9 PA09           D3   9  17    0/1     2/1   0/1    1/3
  8 PA08           D4   -  16    0/0     2/0   0/0    1/2
 15 PA15           D5  15   -    2/3     4/3   3/1    0/5
 21 PA21           D7   5   -    5/3     3/3   7/1    0/7
  7 PA07           D9   7   7     -      0/3   1/1     -
 18 PA18          D10   2   -    1/2     3/2   3/0    0/2
 16 PA16          D11   0   -    1/0     3/0   2/0    0/6
 19 PA19          D12   3   -    1/3     3/3   3/1    0/3
 17 PA17          D13   1   -    1/1     3/1   2/1    0/7
  0 PA00  DOTSTAR_CLK   0   -     -      1/0   2/0     -
  1 PA01 DOTSTAR_DATA   1   -     -      1/1   2/1     -
 27 PA27     FLASH_CS  15   -     -       -     -      -
 35 PB03   FLASH_MISO   3  11     -      5/1   6/1     -
 54 PB22   FLASH_MOSI   6   -     -      5/2   7/0     -
 55 PB23    FLASH_SCK   7   -     -      5/3   7/1     -
 12 PA12         MISO  12   -    2/0     4/0   2/0    0/6
 42 PB10         MOSI  10   -     -      4/2   5/0    0/4
 43 PB11          SCK  11   -     -      4/3   5/1    0/5
 23 PA23          SCL   7   -    3/1     5/1   4/1    0/5
 22 PA22          SDA   6   -    3/0     5/0   4/0    0/4
 30 PA30        SWCLK  10   -     -      1/2   1/0     -
 31 PA31        SWDIO  11   -     -      1/3   1/1     -
 24 PA24       USB_DM  12   -    3/2     5/2   5/0    1/2
 25 PA25       USB_DP  13   -    3/3     5/3   5/1    1/3
  3 PA03                3   1     -       -     -      -
  6 PA06                6   6     -      0/2   1/0     -
 13 PA13               13   -    2/1     4/1   2/0    0/7
 20 PA20                4   -    5/2     3/2   7/0    0/4
 28 PA28                8   -     -       -     -      -
=== ==== ============ ==== ==== ====== ====== ====== ======


Description of the columns:

- *Pin* - The number that is expected at ``machine.Pin(n)``, if the pin is given
  as a number.
- *GPIO* - The GPIO name, which can be used as argument to ``machine.Pin("name")``.
- *Pin Name* - The boards name, which can be used as argument to ``machine.Pin("name")``.
- *IRQ* - The IRQ number assigned to that GPIO, used internally by ``Pin.irq()``. When
  using ``Pin.irq()``, different pins must use different IRQs
- *ADC* - The ADC channel assigned to the pin. When using ADC, different pins must
  not use the same ADC channel.
- *Serial* - Two columns with optional Serial signal assignments. Both may be used.
  The cell content is device #/pad #. The pad # is the respective internal
  signal of that serial device. Details below.
- *TCC/TC* - Two columns with assignments of the TCC modules for PWM.
  The cell content is device #/output #. For PWM, devices 0, 1, and 2
  are used. The TC device pair 3/4 is used for ``ticks_us()``.

SAMD21 UART assignments
```````````````````````
The UART devices and signals must be chosen according to the following rules:

- The TX signal must be at a Pin with pad numbers 2 or 0, like Pin D1 with serial
  device 0 or 2.
- The RX pin may be assigned to one of the other pads.

Examples for Adafruit ItsyBitsy M0 Express:

- uart 0 at pins D0/D1  This is the default UART at the RX/TX labelled pins
- uart 1 at pins D12/D10
- uart 2 at pins D0/D1
- uart 3 at pins D11/D13
- uart 4 at pins D2/D5
- uart 5 at pins SCL/SDA

or other combinations. For hardware flow control, tx must be at pad 0, rx at pad 1,
rts at pad 2 and cts at pad 3. This applies for instance to
UART 3 or UART 1 at the pins D13/D11/D10/D12 for rx/tx/rts/cts.

SAMD21 I2C assignments
``````````````````````
The I2C devices and signals must be chosen according to the following rules:

- The SDA signal must be at a Pin with pad numbers 0.
- The SCL signal must be at a Pin with pad numbers 1.

Examples for Adafruit ItsyBitsy M0 Express:

- I2C 0 at Pin A3/A4
- I2C 1 at pins D11/D13
- I2C 2 at the pins D4/D3
- I2C 3 at the pins SDA/SCL This is the default I2C device at the SDA/SCl labelled pin
- I2C 4 at the pins A1/A2
- I2C 5 at the pins SDA/SCL,

or other combinations.

SAMD21 SPI assignments
``````````````````````
The SPI devices and signals must be chosen according to the following rules:

- The following pad number pairs are suitable for MOSI/SCK: 0/1, 2/3, 3/1, and 0/3.
- The MISO signal must be at a Pin with a different pad number than MOSI or SCK.

Examples for Adafruit ItsyBitsy M0 Express:

- SPI 0 at pins D0/D4/D1
- SPI 1 at pins D11/D12/D13
- SPI 2 at pins D0/D4/D1
- SPI 3 at pins D11/D12/D13
- SPI 4 at Pin MOSI/MISO/SCK This is the default SPI device at the MOSI/MISO/SCK labelled pins.

or other combinations.


SAMD21 PWM assignments
``````````````````````

The TCC/TC device numbers 0, 1 and 2 can be used for PWM. Device 0 has four
channels, device 1 and 2 have two channels. So in total 3 different PWM
frequencies can be used, and 8 different duty cycle values.

The DAC output for the Adafruit ItsyBitsy M0 Express board is available at the pin A0.

.. _samd51_pinout_table:

Adafruit ItsyBitsy M4 Express pin assignment table
--------------------------------------------------

=== ==== ============ ==== ==== ==== ====== ====== ===== ===== =====
Pin GPIO Pin name     IRQ  ADC  ADC  Serial Serial  TC    PWM   PWM
=== ==== ============ ==== ==== ==== ====== ====== ===== ===== =====
  2 PA02           A0   2    0    -     -      -     -     -     -
  5 PA05           A1   5    5    -     -     0/1   0/1    -     -
 40 PB08           A2   8    2    0     -     4/0   4/0    -     -
 41 PB09           A3   9    3    1     -     4/1   4/1    -     -
  4 PA04           A4   4    4    -     -     0/0   0/0    -     -
  6 PA06           A5   6    6    -     -     0/2   1/0    -     -
 16 PA16           D0   0    -    -    1/0    3/1   2/0   1/0   0/4
 17 PA17           D1   1    -    -    1/1    3/0   2/1   1/1   0/5
  7 PA07           D2   7    7    -     -     0/3   1/1    -     -
 54 PB22           D3  22    -    -    1/2    5/2   7/0    -     -
 14 PA14           D4  14    -    -    2/2    4/2   3/0   2/0   1/2
 15 PA15           D5  15    -    -    2/3    4/3   3/1   2/1   1/3
 18 PA18           D7   2    -    -    1/2    3/2   3/0   1/2   0/6
 19 PA19           D9   3    -    -    1/3    3/3   3/1   1/3   0/7
 20 PA20          D10   4    -    -    5/2    3/2   7/0   1/4   0/0
 21 PA21          D11   5    -    -    5/3    3/3   7/1   1/5   0/1
 23 PA23          D12   7    -    -    3/1    5/0   4/1   1/7   0/3
 22 PA22          D13   6    -    -    3/0    5/1   4/0   1/6   0/2
 34 PB02  DOTSTAR_CLK   2   14    -     -     5/0   6/0   2/2    -
 35 PB03 DOTSTAR_DATA   9   15    -     -     5/1   6/1    -     -
 55 PB23         MISO   7    -    -    1/3    5/3   7/1    -     -
  0 PA00         MOSI   0    -    -     -     1/0   2/0    -     -
 43 PB11      QSPI_CS  12    -    -     -     4/3   5/1   0/5   1/1
  8 PA08      QSPI_D0   -    8    2    0/0    2/1   0/0   0/0   1/4
  9 PA09      QSPI_D1   9    9    3    0/1    2/0   0/1   0/1   1/5
 10 PA10      QSPI_D2  10   10    -    0/2    2/2   1/0   0/2   1/6
 11 PA11      QSPI_D3  11   11    -    0/3    2/3   1/1   0/3   1/7
 42 PB10     QSPI_SCK  10    -    -     -     4/2   5/0   0/4   1/0
  1 PA01          SCK   1    -    -     -     1/1   2/1    -     -
 13 PA13          SCL  13    -    -    2/1    4/0   2/1   0/7   1/3
 12 PA12          SDA  12    -    -    2/0    4/1   2/0   0/6   1/2
 30 PA30        SWCLK  14    -    -    7/2    1/2   6/0   2/0    -
 31 PA31        SWDIO  15    -    -    7/3    1/3   6/1   2/1    -
 24 PA24       USB_DM   8    -    -    3/2    5/2   5/0   2/2    -
 25 PA25       USB_DP   9    -    -    3/3    5/3   5/1    -     -
  3 PA03            -   3   10    -     -      -     -     -     -
 27 PA27            -  11    -    -     -      -     -     -     -
=== ==== ============ ==== ==== ==== ====== ====== ===== ===== =====


Description of the columns:

- *Pin* - The number that is expected at ``machine.Pin(n)``, if the pin is given
  as a number.
- *GPIO* - The GPIO name, which can be used as argument to ``machine.Pin("name")``.
- *Pin Name* - The boards name, which can be used as argument to ``machine.Pin("name")``.
- *IRQ* - The IRQ number assigned to that GPIO, used internally by ``Pin.irq()``. When
  using ``Pin.irq()``, different pins must use different IRQs
- *ADC* - The ADC0/1 channel assigned to the pin. When using ADC, different pins must
  not use the same ADC device and channel.
- *Serial* - Two columns with optional Serial signal assignments. Both may be used.
  The cell content is device #/pad #. The pad # is the respective internal
  signal of that serial device. Details below.
- *TC* - These device are currently not assigned to Pin. the TC device pair 0/1
  is used for ``ticks_us()``.
- *PWM* - Two columns with assignments of the TCC modules for PWM
  The cell content is device #/output #. Details below.

SAMD51 UART assignments
```````````````````````
The UART devices and signals must be chosen according to the following rules:

- The TX signal must be at a Pin with pad numbers 0, like Pin D1 with serial
  device 3.
- The RX pin may be assigned to one of the other pads.

Examples for Adafruit ItsyBitsy 4 Express:

- uart 0 at pins A4/A1
- uart 1 at pins D1/D0   This is the default UART at the RX/TX labelled pins
- uart 2 at pins SCL/SDA  This is the default I2C device at the SDA/SCl labelled pin
- uart 3 at pins D0/D1
- uart 4 at pins SDA/SCL
- uart 5 at pins D12/D13

or other combinations. For hardware flow control, tx must be at pad 0, rx at pad 1,
rts at pad 2 and cts at pad 3. This applies for instance to
UART 5 at the pins D12/D13/D10/D11 for rx/tx/rts/cts.

SAMD51 I2C assignments
``````````````````````
The I2C devices and signals must be chosen according to the following rules:

- The SDA signal must be at a Pin with pad numbers 0.
- The SCL signal must be at a Pin with pad numbers 1.

Examples for Adafruit ItsyBitsy M0 Express:

- I2C 0 at pins A3/A4
- I2C 1 at pins D0/D1
- I2C 2 at the pins SDA/SCL
- I2C 3 at the pins D1/D0
- I2C 4 at the pins A2/A3
- I2C 5 at the pins D12/D13

or other combinations.

SAMD51 SPI assignments
``````````````````````
The SPI devices and signals must be chosen according to the following rules:

- The following pad number pairs are suitable for MOSI/SCK: 0/1 and 3/1.
- The MISO signal must be at a Pin with a different pad number than MOSI or SCK.

Examples for Adafruit ItsyBitsy M0 Express:

- SPI 1 at Pin MOSI/MISO/SCK  This is the default SPI device at the MOSI/MISO/SCK labelled pins.
- SPI 3 at pins D13/D11/D12
- SPI 5 at pins D12/D3/D13

or other combinations.


SAMD51 PWM assignments
``````````````````````

The TCC/PWM device numbers 0 through 4 can be used for PWM. Device 0 has six
channels, device 1 has four channels, device 2 has three channels and devices
3 and 4 have two channels. So in total up to 5 different PWM frequencies
can be used, and up to 17 different duty cycle values. Note that these numbers
do not apply to every board.

The DAC outputs for the Adafruit ItsyBitsy M4 Express board are available at the pins A0 and A1.

Adafruit Feather M4 Express pin assignment table
------------------------------------------------

=== ==== ============ ==== ==== ==== ====== ====== ===== ===== =====
Pin GPIO Pin name     IRQ  ADC  ADC  Serial Serial  TC    PWM   PWM
=== ==== ============ ==== ==== ==== ====== ====== ===== ===== =====
  2 PA02           A0   2    0    -     -      -     -     -      -
  5 PA05           A1   5    5    -     -     0/1   0/1    -      -
 40 PB08           A2   8    2    0     -     4/0   4/0    -      -
 41 PB09           A3   9    3    1     -     4/1   4/1    -      -
  4 PA04           A4   4    4    -     -     0/0   0/0    -      -
 38 PB06           A5   6    -    8     -      -     -     -      -
 49 PB17           D0   1    -    -    5/1     -    6/1   3/1    0/5
 48 PB16           D1   0    -    -    5/0     -    6/0   3/0    0/4
 14 PA14           D4  14    -    -    2/2    4/2   3/0   2/0    1/2
 16 PA16           D5   0    -    -    1/0    3/1   2/0   1/0    0/4
 18 PA18           D6   2    -    -    1/2    3/2   3/0   1/2    0/6
 19 PA19           D9   3    -    -    1/3    3/3   3/1   1/3    0/7
  3 PA03         AREF   3   10    -     -      -     -     -      -
 20 PA20          D10   4    -    -    5/2    3/2   7/0   1/4    0/0
 21 PA21          D11   5    -    -    5/3    3/3   7/1   1/5    0/1
 22 PA22          D12   6    -    -    3/0    5/1   4/0   1/6    0/2
 23 PA23          D13   7    -    -    3/1    5/0   4/1   1/7    0/3
 54 PB22         MISO  22    -    -    1/2    5/2   7/0    -      -
 55 PB23         MOSI   7    -    -    1/3    5/3   7/1    -      -
 35 PB03     NEOPIXEL   9   15    -     -     5/1   6/1    -      -
 43 PB11      QSPI_CS  12    -    -     -     4/3   5/1   0/5   1/1
  8 PA08      QSPI_D0   -    8    2    0/0    2/1   0/0   0/0   1/4
  9 PA09      QSPI_D1   9    9    3    0/1    2/0   0/1   0/1   1/5
 10 PA10      QSPI_D2  10   10    -    0/2    2/2   1/0   0/2   1/6
 11 PA11      QSPI_D3  11   11    -    0/3    2/3   1/1   0/3   1/7
 42 PB10     QSPI_SCK  10    -    -     -     4/2   5/0   0/4   1/0
 17 PA17          SCK   1    -    -    1/1    3/0   2/1   1/1    0/5
 13 PA13          SCL  13    -    -    2/1    4/0   2/1   0/7    1/3
 12 PA12          SDA  12    -    -    2/0    4/1   2/0   0/6    1/2
 30 PA30        SWCLK  14    -    -    7/2    1/2   6/0   2/0     -
 31 PA31        SWDIO  15    -    -    7/3    1/3   6/1   2/1     -
 24 PA24       USB_DM   8    -    -    3/2    5/2   5/0   2/2     -
 25 PA25       USB_DP   9    -    -    3/3    5/3   5/1    -      -
 33 PB01         VDIV   1   13    -     -     5/3   7/1    -      -
  0 PA00            -   0    -    -     -     1/0   2/0    -      -
  1 PA01            -   1    -    -     -     1/1   2/1    -      -
  6 PA06            -   6    6    -     -     0/2   1/0    -      -
  7 PA07            -   7    7    -     -     0/3   1/1    -      -
 15 PA15            -  15    -    -    2/3    4/3   3/1   2/1    1/3
 27 PA27            -  11    -    -     -      -     -     -      -
 32 PB00            -   9   12    -     -     5/2   7/0    -      -
 34 PB02            -   2   14    -     -     5/0   6/0   2/2     -
 36 PB04            -   4    -    6     -      -     -     -      -
 37 PB05            -   5    -    7     -      -     -     -      -
 39 PB07            -   7    -    9     -      -     -     -      -
 44 PB12            -  12    -    -    4/0     -    4/0   3/0    0/0
 45 PB13            -  13    -    -    4/1     -    4/1   3/1    0/1
 46 PB14            -  14    -    -    4/2     -    5/0   4/0    0/2
 47 PB15            -  15    -    -    4/3     -    5/1   4/1    0/3
 62 PB30            -  14    -    -    7/0    5/1   0/0   4/0    0/6
 63 PB31            -  15    -    -    7/1    5/0   0/1   4/1    0/7
=== ==== ============ ==== ==== ==== ====== ====== ===== ===== =====

For the definition of the table columns see the explanation at the table for
Adafruit ItsyBitsy M4 Express :ref:`samd51_pinout_table`.

The default devices at the board are:

- UART 5 at pins D0/D1, labelled RX/TX
- I2C 2 at pins PA12/PA13, labelled SDA/SCL
- SPI 1 at pins PA23/PA22/PA17, labelled MOSI, MISO and SCK
- DAC output on pins PA02 and PA05, labelled A0 and A1

Adafruit Metro M4 Airlift pin assignment table
----------------------------------------------

=== ==== ============ ==== ==== ==== ====== ====== ===== ===== =====
Pin GPIO Pin name     IRQ  ADC  ADC  Serial Serial  TC    PWM   PWM
=== ==== ============ ==== ==== ==== ====== ====== ===== ===== =====
  2 PA02           A0  2     0    -     -      -     -     -     -
  5 PA05           A1  5     5    -     -     0/1   0/1    -     -
  6 PA06           A2  6     6    -     -     0/2   1/0    -     -
 32 PB00           A3  9    12    -     -     5/2   7/0    -     -
 40 PB08           A4  8     2    0     -     4/0   4/0    -     -
 41 PB09           A5  9     3    1     -     4/1   4/1    -     -
 23 PA23           D0  7     -    -    3/1    5/0   4/1   1/7   0/3
 22 PA22           D1  6     -    -    3/0    5/1   4/0   1/6   0/2
 49 PB17           D2  1     -    -    5/1     -    6/1   3/1   0/5
 48 PB16           D3  0     -    -    5/0     -    6/0   3/0   0/4
 45 PB13           D4 13     -    -    4/1     -    4/1   3/1   0/1
 46 PB14           D5 14     -    -    4/2     -    5/0   4/0   0/2
 47 PB15           D6 15     -    -    4/3     -    5/1   4/1   0/3
 44 PB12           D7 12     -    -    4/0     -    4/0   3/0   0/0
 21 PA21           D8  5     -    -    5/3    3/3   7/1   1/5   0/1
 20 PA20           D9  4     -    -    5/2    3/2   7/0   1/4   0/0
  3 PA03         AREF  3    10    -     -      -     -     -     -
 18 PA18          D10  2     -    -    1/2    3/2   3/0   1/2   0/6
 19 PA19          D11  3     -    -    1/3    3/3   3/1   1/3   0/7
 16 PA16          D13  0     -    -    1/0    3/1   2/0   1/0   0/4
 36 PB04     ESP_BUSY  4     -    6     -      -     -     -     -
 15 PA15       ESP_CS 15     -    -    2/3    4/3   3/1   2/1   1/3
 33 PB01    ESP_GPIO0  1    13    -     -     5/3   7/1    -     -
 37 PB05    ESP_RESET  5     -    7     -      -     -     -     -
 55 PB23      ESP_RTS  7     -    -    1/3    5/3   7/1    -     -
  7 PA07       ESP_RX  7     7    -     -     0/3   1/1    -     -
  4 PA04       ESP_TX  4     4    -     -     0/0   0/0    -     -
 43 PB11     FLASH_CS 12     -    -     -     4/3   5/1   0/5   1/1
 11 PA11   FLASH_HOLD 11    11    -    0/3    2/3   1/1   0/3   1/7
  9 PA09   FLASH_MISO  9     9    3    0/1    2/0   0/1   0/1   1/5
  8 PA08   FLASH_MOSI  -     8    2    0/0    2/1   0/0   0/0   1/4
 42 PB10    FLASH_SCK 10     -    -     -     4/2   5/0   0/4   1/0
 10 PA10     FLASH_WP 10    10    -    0/2    2/2   1/0   0/2   1/6
 14 PA14         MISO 14     -    -    2/2    4/2   3/0   2/0   1/2
 12 PA12         MOSI 12     -    -    2/0    4/1   2/0   0/6   1/2
 54 PB22     NEOPIXEL 22     -    -    1/2    5/2   7/0    -     -
 38 PB06        RXLED  6     -    8     -      -     -     -     -
 13 PA13          SCK 13     -    -    2/1    4/0   2/1   0/7   1/3
 35 PB03          SCL  9    15    -     -     5/1   6/1    -     -
 34 PB02          SDA  2    14    -     -     5/0   6/0   2/2    -
 30 PA30        SWCLK 14     -    -    7/2    1/2   6/0   2/0    -
 31 PA31        SWDIO 15     -    -    7/3    1/3   6/1   2/1    -
 62 PB30          SWO 14     -    -    7/0    5/1   0/0   4/0   0/6
 39 PB07        TXLED  7     -    9     -      -     -     -     -
 24 PA24       USB_DM  8     -    -    3/2    5/2   5/0   2/2    -
 25 PA25       USB_DP  9     -    -    3/3    5/3   5/1    -     -
 17 PA17   USB_HOSTEN  1     -    -    1/1    3/0   2/1   1/1   0/5
  0 PA00            -  0     -    -     -     1/0   2/0    -     -
  1 PA01            -  1     -    -     -     1/1   2/1    -     -
 27 PA27            - 11     -    -     -      -     -     -     -
 63 PB31            - 15     -    -    7/1    5/0   0/1   4/1   0/7
=== ==== ============ ==== ==== ==== ====== ====== ===== ===== =====

For the definition of the table columns see the explanation at the table
for Adafruit ItsyBitsy M4 Express :ref:`samd51_pinout_table`.

The default devices at the board are:

- UART 3 at pins PA23/PA22, labelled D0/D1 resp. RX/TX
- I2C 5 at pins PB02/PB03, labelled SDA/SCL
- SPI 4 at pins PA12/PA14/PA13, labelled MOSI, MISO and SCK
- DAC output on pins PA02 and PA05, labelled A0 and A1

SEEED XIAO pin assignment table
-------------------------------

=== ==== ============ ==== ==== ====== ====== ====== ======
Pin GPIO Pin name     IRQ  ADC  Serial Serial TCC/TC TCC/TC
=== ==== ============ ==== ==== ====== ====== ====== ======
  2 PA02        A0_D0   2    0     -      -     -      -
  4 PA04        A1_D1   4    4     -     0/0   0/0     -
 10 PA10        A2_D2  10   18    0/2    2/2   1/0    0/2
 11 PA11        A3_D3  11   19    0/3    2/3   1/1    0/3
  8 PA08        A4_D4   -   16    0/0    2/0   0/0    1/2
  9 PA09        A5_D5   9   17    0/1    2/1   0/1    1/3
 40 PB08        A6_D6   8    2     -     4/0   4/0     -
 41 PB09        A7_D7   9    3     -     4/1   4/1     -
  7 PA07        A8_D8   7    7     -     0/3   1/1     -
  5 PA05        A9_D9   5    5     -     0/1   0/1     -
  6 PA06      A10_D10   6    6     -     0/2   1/0     -
 18 PA18       RX_LED   2    -    1/2    3/2   3/0    0/2
 30 PA30        SWCLK  10    -     -     1/2   1/0     -
 31 PA31        SWDIO  11    -     -     1/3   1/1     -
 19 PA19       TX_LED   3    -    1/3    3/3   3/1    0/3
 24 PA24       USB_DM  12    -    3/2    5/2   5/0    1/2
 25 PA25       USB_DP  13    -    3/3    5/3   5/1    1/3
 17 PA17     USER_LED   1    -    1/1    3/1   2/1    0/7
  0 PA00            -   0    -     -     1/0   2/0     -
  1 PA01            -   1    -     -     1/1   2/1     -
  3 PA03            -   3    1     -      -     -      -
 12 PA12            -  12    -    2/0    4/0   2/0    0/6
 13 PA13            -  13    -    2/1    4/1   2/0    0/7
 14 PA14            -  14    -    2/2    4/2   3/0    0/4
 15 PA15            -  15    -    2/3    4/3   3/1    0/5
 16 PA16            -   0    -    1/0    3/0   2/0    0/6
 20 PA20            -   4    -    5/2    3/2   7/0    0/4
 21 PA21            -   5    -    5/3    3/3   7/1    0/7
 22 PA22            -   6    -    3/0    5/0   4/0    0/4
 23 PA23            -   7    -    3/1    5/1   4/1    0/5
 27 PA27            -  15    -     -      -     -      -
 28 PA28            -   8    -     -      -     -      -
 34 PB02            -   2   10     -     5/0   6/0     -
 35 PB03            -   3   11     -     5/1   6/1     -
 42 PB10            -  10    -     -     4/2   5/0    0/4
 43 PB11            -  11    -     -     4/3   5/1    0/5
 54 PB22            -   6    -     -     5/2   7/0     -
 55 PB23            -   7    -     -     5/3   7/1     -
=== ==== ============ ==== ==== ====== ====== ====== ======

For the definition of the table columns see the explanation at the table for
Adafruit ItsyBitsy M0 Express :ref:`samd21_pinout_table`.

The default devices at the board are:

- UART 4 at pins PB08/PB09, labelled A6_D6/A7_D7
- I2C 2 at pins PA08/PA09, labelled A4_D4/A5_D5
- SPI 0 at pins PA06/PA05/PA07, labelled A10_D10, A9_D9 and A8_D8
- DAC output on pin PA02, labelled A0_D0

Adafruit Feather M0 Express pin assignment table
------------------------------------------------

=== ==== ============ ==== ==== ====== ====== ====== ======
Pin GPIO Pin name     IRQ  ADC  Serial Serial TCC/TC TCC/TC
=== ==== ============ ==== ==== ====== ====== ====== ======
  2 PA02           A0   2    0     -      -     -      -
 40 PB08           A1   8    2     -     4/0   4/0     -
 41 PB09           A2   9    3     -     4/1   4/1     -
  4 PA04           A3   4    4     -     0/0   0/0     -
  5 PA05           A4   5    5     -     0/1   0/1     -
 34 PB02           A5   2   10     -     5/0   6/0     -
 11 PA11           D0  11   19    0/3    2/3   1/1    0/3
 10 PA10           D1  10   18    0/2    2/2   1/0    0/2
 14 PA14           D2  14    -    2/2    4/2   3/0    0/4
  9 PA09           D3   9   17    0/1    2/1   0/1    1/3
  8 PA08           D4   -   16    0/0    2/0   0/0    1/2
 15 PA15           D5  15    -    2/3    4/3   3/1    0/5
 20 PA20           D6   4    -    5/2    3/2   7/0    0/4
 21 PA21           D7   5    -    5/3    3/3   7/1    0/7
  7 PA07           D9   7    7     -     0/3   1/1     -
 55 PB23           RX   7    -     -     5/3   7/1     -
 54 PB22           TX   6    -     -     5/2   7/0     -
 18 PA18          D10   2    -    1/2    3/2   3/0    0/2
 16 PA16          D11   0    -    1/0    3/0   2/0    0/6
 19 PA19          D12   3    -    1/3    3/3   3/1    0/3
 17 PA17          D13   1    -    1/1    3/1   2/1    0/7
 13 PA13     FLASH_CS  13    -    2/1    4/1   2/0    0/7
 35 PB03       LED_RX   3   11     -     5/1   6/1     -
 27 PA27       LED_TX  15    -     -      -     -      -
 12 PA12         MISO  12    -    2/0    4/0   2/0    0/6
 42 PB10         MOSI  10    -     -     4/2   5/0    0/4
  6 PA06     NEOPIXEL   6    6     -     0/2   1/0     -
 43 PB11          SCK  11    -     -     4/3   5/1    0/5
 23 PA23          SCL   7    -    3/1    5/1   4/1    0/5
 22 PA22          SDA   6    -    3/0    5/0   4/0    0/4
 30 PA30        SWCLK  10    -     -     1/2   1/0     -
 31 PA31        SWDIO  11    -     -     1/3   1/1     -
 24 PA24       USB_DM  12    -    3/2    5/2   5/0    1/2
 25 PA25       USB_DP  13    -    3/3    5/3   5/1    1/3
  0 PA00            -   0    -     -     1/0   2/0     -
  1 PA01            -   1    -     -     1/1   2/1     -
  3 PA03            -   3    1     -      -     -      -
 28 PA28            -   8    -     -      -     -      -
=== ==== ============ ==== ==== ====== ====== ====== ======

For the definition of the table columns see the explanation at the table for
Adafruit ItsyBitsy M0 Express :ref:`samd21_pinout_table`.

The default devices at the board are:

- UART 5 at pins PB23/PB22, labelled RX/TX
- I2C 3 at pins PA22/PA23, labelled SDA/SCL
- SPI 4 at pins PA10/PA12/PA11, labelled MOSI, MISO and SCK
- DAC output on pin PA02, labelled A0

Adafruit Trinket M0 pin assignment table
------------------------------------------------

=== ==== ============ ==== ==== ====== ====== ====== ======
Pin GPIO Pin name     IRQ  ADC  Serial Serial TCC/TC TCC/TC
=== ==== ============ ==== ==== ====== ====== ====== ======
  8 PA08           D0   -   16    0/0    2/0   0/0    1/2
  2 PA02           D1   2    0     -      -     -      -
  9 PA09           D2   9   17    0/1    2/1   0/1    1/3
  7 PA07           D3   7    7     -     0/3   1/1     -
  6 PA06           D4   6    6     -     0/2   1/0     -
  1 PA01  DOTSTAR_CLK   1    -     -     1/1   2/1     -
  0 PA00 DOTSTAR_DATA   0    -     -     1/0   2/0     -
 10 PA10          LED  10   18    0/2    2/2   1/0    0/2
 30 PA30        SWCLK  10    -     -     1/2   1/0     -
 31 PA31        SWDIO  11    -     -     1/3   1/1     -
 24 PA24       USB_DM  12    -    3/2    5/2   5/0    1/2
 25 PA25       USB_DP  13    -    3/3    5/3   5/1    1/3
  3 PA03            -   3    1     -      -     -      -
  4 PA04            -   4    4     -     0/0   0/0     -
  5 PA05            -   5    5     -     0/1   0/1     -
 11 PA11            -  11   19    0/3    2/3   1/1    0/3
 14 PA14            -  14    -    2/2    4/2   3/0    0/4
 15 PA15            -  15    -    2/3    4/3   3/1    0/5
 16 PA16            -   0    -    1/0    3/0   2/0    0/6
 17 PA17            -   1    -    1/1    3/1   2/1    0/7
 18 PA18            -   2    -    1/2    3/2   3/0    0/2
 19 PA19            -   3    -    1/3    3/3   3/1    0/3
 22 PA22            -   6    -    3/0    5/0   4/0    0/4
 23 PA23            -   7    -    3/1    5/1   4/1    0/5
 27 PA27            -  15    -     -      -     -      -
 28 PA28            -   8    -     -      -     -      -
=== ==== ============ ==== ==== ====== ====== ====== ======

For the definition of the table columns see the explanation at the table for
Adafruit ItsyBitsy M0 Express :ref:`samd21_pinout_table`.

The default devices at the board are:

- UART 0 at pins PA07/PA06, labelled D3/D4
- I2C 2 at pins PA08/PA09, labelled D0/D2
- SPI 0 at pins PA06/PA09/PA08, labelled D4, D2 and D0
- DAC output on pin PA02, labelled D1

SAMD21 Xplained PRO pin assignment table
----------------------------------------

=== ==== ============ ==== ==== ====== ====== ====== ======
Pin GPIO Pin name     IRQ  ADC  Serial Serial TCC/TC TCC/TC
=== ==== ============ ==== ==== ====== ====== ====== ======
 32 PB00    EXT1_PIN3   0    8     -     5/2   7/0     -
 33 PB01    EXT1_PIN4   1    9     -     5/3   7/1     -
 38 PB06    EXT1_PIN5   6   14     -      -     -      -
 39 PB07    EXT1_PIN6   7   15     -      -     -      -
 34 PB02    EXT1_PIN7   2   10     -     5/0   6/0     -
 35 PB03    EXT1_PIN8   3   11     -     5/1   6/1     -
 36 PB04    EXT1_PIN9   4   12     -      -     -      -
 37 PB05   EXT1_PIN10   5   13     -      -     -      -
  8 PA08   EXT1_PIN11   -   16    0/0    2/0   0/0    1/2
  9 PA09   EXT1_PIN12   9   17    0/1    2/1   0/1    1/3
 41 PB09   EXT1_PIN13   9    3     -     4/1   4/1     -
 40 PB08   EXT1_PIN14   8    2     -     4/0   4/0     -
  5 PA05   EXT1_PIN15   5    5     -     0/1   0/1     -
  6 PA06   EXT1_PIN16   6    6     -     0/2   1/0     -
  4 PA04   EXT1_PIN17   4    4     -     0/0   0/0     -
  7 PA07   EXT1_PIN18   7    7     -     0/3   1/1     -
 10 PA10    EXT2_PIN3  10   18    0/2    2/2   1/0    0/2
 11 PA11    EXT2_PIN4  11   19    0/3    2/3   1/1    0/3
 20 PA20    EXT2_PIN5   4    -    5/2    3/2   7/0    0/4
 21 PA21    EXT2_PIN6   5    -    5/3    3/3   7/1    0/7
 44 PB12    EXT2_PIN7  12    -    4/0     -    4/0    0/6
 45 PB13    EXT2_PIN8  13    -    4/1     -    4/1    0/7
 46 PB14    EXT2_PIN9  14    -    4/2     -    5/0     -
 47 PB15   EXT2_PIN10  15    -    4/3     -    5/1     -
 43 PB11   EXT2_PIN13  11    -     -     4/3   5/1    0/5
 42 PB10   EXT2_PIN14  10    -     -     4/2   5/0    0/4
 17 PA17   EXT2_PIN15   1    -    1/1    3/1   2/1    0/7
 18 PA18   EXT2_PIN16   2    -    1/2    3/2   3/0    0/2
 16 PA16   EXT2_PIN17   0    -    1/0    3/0   2/0    0/6
 19 PA19   EXT2_PIN18   3    -    1/3    3/3   3/1    0/3
  2 PA02    EXT3_PIN3   2    0     -      -     -      -
  3 PA03    EXT3_PIN4   3    1     -      -     -      -
 15 PA15    EXT3_PIN6  15    -    2/3    4/3   3/1    0/5
 12 PA12    EXT3_PIN7  12    -    2/0    4/0   2/0    0/6
 13 PA13    EXT3_PIN8  13    -    2/1    4/1   2/0    0/7
 28 PA28    EXT3_PIN9   8    -     -      -     -      -
 27 PA27   EXT3_PIN10  15    -     -      -     -      -
 49 PB17   EXT3_PIN15   1    -    5/1     -    6/1    0/5
 54 PB22   EXT3_PIN16   6    -     -     5/2   7/0     -
 48 PB16   EXT3_PIN17   9    -    5/0     -    6/0    0/4
 55 PB23   EXT3_PIN18   7    -     -     5/3   7/1     -
 62 PB30          LED  14    -     -     5/0   0/0    1/2
 30 PA30        SWCLK  10    -     -     1/2   1/0     -
 31 PA31        SWDIO  11    -     -     1/3   1/1     -
 24 PA24       USB_DM  12    -    3/2    5/2   5/0    1/2
 25 PA25       USB_DP  13    -    3/3    5/3   5/1    1/3
  0 PA00            -   0    -     -     1/0   2/0     -
  1 PA01            -   1    -     -     1/1   2/1     -
 14 PA14            -  14    -    2/2    4/2   3/0    0/4
 22 PA22            -   6    -    3/0    5/0   4/0    0/4
 23 PA23            -   7    -    3/1    5/1   4/1    0/5
 63 PB31            -  15    -     -     5/1   0/1    1/3
=== ==== ============ ==== ==== ====== ====== ====== ======

For the definition of the table columns see the explanation at the table for
Adafruit ItsyBitsy M0 Express :ref:`samd21_pinout_table`.

There are no pins labelled for default devices on this board. DAC output
is on pin PA02, labelled EXT3_PIN3

Minisam M4 pin assignment table
-------------------------------

=== ==== ============ ==== ==== ==== ====== ====== ===== ===== =====
Pin GPIO Pin name     IRQ  ADC  ADC  Serial Serial  TC    PWM   PWM
=== ==== ============ ==== ==== ==== ====== ====== ===== ===== =====
 16 PA16           D0   0    -    -    1/0    3/1   2/0   1/0   0/4
 17 PA17           D1   1    -    -    1/1    3/0   2/1   1/1   0/5
 19 PA19           D3   3    -    -    1/3    3/3   3/1   1/3   0/7
 20 PA20           D4   4    -    -    5/2    3/2   7/0   1/4   0/0
 21 PA21           D5   5    -    -    5/3    3/3   7/1   1/5   0/1
  2 PA02        A0_D9   2    0    -     -      -     -     -     -
 40 PB08       A1_D10   8    2    0     -     4/0   4/0    -     -
 41 PB09       A2_D11   9    3    1     -     4/1   4/1    -     -
  4 PA04       A3_D12   4    4    -     -     0/0   0/0    -     -
  5 PA05       A4_D13   5    5    -     -     0/1   0/1    -     -
  6 PA06           A5   6    6    -     -     0/2   1/0    -     -
  7 PA07        A6_D2   7    7    -     -     0/3   1/1    -     -
  3 PA03         AREF   3   10    -     -      -     -     -     -
  0 PA00       BUTTON   0    -    -     -     1/0   2/0    -     -
 34 PB02  DOTSTAR_CLK   2   14    -     -     5/0   6/0   2/2    -
 35 PB03 DOTSTAR_DATA   9   15    -     -     5/1   6/1    -     -
 15 PA15          LED  15    -    -    2/3    4/3   3/1   2/1   1/3
 55 PB23         MISO   7    -    -    1/3    5/3   7/1    -     -
 54 PB22         MOSI  22    -    -    1/2    5/2   7/0    -     -
 43 PB11      QSPI_CS  12    -    -     -     4/3   5/1   0/5   1/1
  8 PA08      QSPI_D0   -    8    2    0/0    2/1   0/0   0/0   1/4
  9 PA09      QSPI_D1   9    9    3    0/1    2/0   0/1   0/1   1/5
 10 PA10      QSPI_D2  10   10    -    0/2    2/2   1/0   0/2   1/6
 11 PA11      QSPI_D3  11   11    -    0/3    2/3   1/1   0/3   1/7
 42 PB10     QSPI_SCK  10    -    -     -     4/2   5/0   0/4   1/0
  1 PA01          SCK   1    -    -     -     1/1   2/1    -     -
 13 PA13          SCL  13    -    -    2/1    4/0   2/1   0/7   1/3
 12 PA12          SDA  12    -    -    2/0    4/1   2/0   0/6   1/2
 30 PA30        SWCLK  14    -    -    7/2    1/2   6/0   2/0    -
 31 PA31        SWDIO  15    -    -    7/3    1/3   6/1   2/1    -
 24 PA24       USB_DM   8    -    -    3/2    5/2   5/0   2/2    -
 25 PA25       USB_DP   9    -    -    3/3    5/3   5/1    -     -
 14 PA14            -  14    -    -    2/2    4/2   3/0   2/0   1/2
 18 PA18            -   2    -    -    1/2    3/2   3/0   1/2   0/6
 22 PA22            -   6    -    -    3/0    5/1   4/0   1/6   0/2
 23 PA23            -   7    -    -    3/1    5/0   4/1   1/7   0/3
 27 PA27            -  11    -    -     -      -     -     -     -
=== ==== ============ ==== ==== ==== ====== ====== ===== ===== =====

For the definition of the table columns see the explanation at the table for
Adafruit ItsyBitsy M4 Express :ref:`samd51_pinout_table`.

The default devices at the board are:

- UART 1 at pins PA16/PA17, labelled D0/D1
- I2C 2 at pins PA12/PA13, labelled SDA/SCL
- SPI 1 at pins PB22/PB23/PA01, labelled MOSI, MISO and SCK
- DAC output on pins PA02 and PA05, labelled A0_D9 and A4_D13

Seeed WIO Terminal pin assignment table
---------------------------------------

=== ==== ============ ==== ==== ==== ====== ====== ===== ===== =====
Pin GPIO Pin name     IRQ  ADC  ADC  Serial Serial  TC    PWM   PWM
=== ==== ============ ==== ==== ==== ====== ====== ===== ===== =====
 33 PB01           CS   1   13    -     -    5/3    7/1    -     -
 59 PB27           RX  13    -    -    2/1   4/0     -    1/3    -
 58 PB26           TX  12    -    -    2/0   4/1     -    1/2    -
 79 PC15   3V3_ENABLE  15    -    -    7/3   6/3     -    0/5   1/1
 78 PC14    5V_ENABLE  14    -    -    7/2   6/2     -    0/4   1/0
 40 PB08        A0_D0   8    2    0     -    4/0    4/0    -     -
 41 PB09        A1_D1   9    3    1     -    4/1    4/1    -     -
  7 PA07        A2_D2   7    7    -     -    0/3    1/1    -     -
 36 PB04        A3_D3   4    -    6     -     -      -     -     -
 37 PB05        A4_D4   5    -    7     -     -      -     -     -
 38 PB06        A5_D5   6    -    8     -     -      -     -     -
  4 PA04        A6_D6   4    4    -     -    0/0    0/0    -     -
 39 PB07        A7_D7   7    -    9     -     -      -     -     -
  6 PA06        A8_D8   6    6    -     -    0/2    1/0    -     -
 90 PC26     BUTTON_1  10    -    -     -     -      -     -     -
 91 PC27     BUTTON_2  11    -    -    1/0    -      -     -     -
 92 PC28     BUTTON_3  12    -    -    1/1    -      -     -     -
107 PD11       BUZZER   6    -    -    7/3   6/3     -    0/4    -
 47 PB15       GPCLK0  15    -    -    4/3    -     5/1   4/1   0/3
 44 PB12       GPCLK1  12    -    -    4/0    -     4/0   3/0   0/0
 45 PB13       GPCLK2  13    -    -    4/1    -     4/1   3/1   0/1
 48 PB16     I2C_BCLK   0    -    -    5/0    -     6/0   3/0   0/4
 20 PA20    I2S_LRCLK   4    -    -    5/2   3/2    7/0   1/4   0/0
 21 PA21     I2S_SDIN   5    -    -    5/3   3/3    7/1   1/5   0/1
 22 PA22    I2S_SDOUT   6    -    -    3/0   5/1    4/0   1/6   0/2
 50 PB18     LCD_MISO   2    -    -    5/2   7/2     -    1/0    -
 51 PB19     LCD_MOSI   3    -    -    5/3   7/3     -    1/1    -
 52 PB20      LCD_SCK   4    -    -    3/0   7/1     -    1/2    -
 53 PB21       LCD_CS   5    -    -    3/1   7/0     -    1/3    -
 70 PC06      LCD_D/C   6    -    -    6/2    -      -     -     -
 71 PC07    LCD_RESET   9    -    -    6/3    -      -     -     -
 74 PC10       LCD_XL  10    -    -    6/2   7/2     -    0/0   1/4
 76 PC12       LCD_XR  12    -    -    7/0   6/1     -    0/2   1/6
 77 PC13       LCD_YD  13    -    -    7/1   6/0     -    0/3   1/7
 75 PC11       LCD_YU  11    -    -    6/3   7/3     -    0/1   1/5
 15 PA15     LED_BLUE  15    -    -    2/3   4/3    3/1   2/1   1/3
 69 PC05      LED_LCD   5    -    -    6/1    -      -     -     -
 94 PC30          MIC  14    -   12     -     -      -     -     -
 32 PB00         MISO   9   12    -     -    5/2    7/0    -     -
 34 PB02         MOSI   2   14    -     -    5/0    6/0   2/2    -
 35 PB03          SCK   9   15    -     -    5/1    6/1    -     -
 12 PA12         SCL0  12    -    -    2/0   4/1    2/0   0/6   1/2
 13 PA13         SDA0  13    -    -    2/1   4/0    2/1   0/7   1/3
 16 PA16         SCL1   0    -    -    1/0   3/1    2/0   1/0   0/4
 17 PA17         SDA1   1    -    -    1/1   3/0    2/1   1/1   0/5
117 PD21       SD_DET  11    -    -    1/3   3/3     -    1/1    -
 83 PC19        SD_CS   3    -    -    6/3   0/3     -    0/3    -
 82 PC18      SD_MISO   2    -    -    6/2   0/2     -    0/2    -
 43 PB11      QSPI_CS  12    -    -     -     4/3   5/1   0/5   1/1
  8 PA08      QSPI_D0   -    8    2    0/0    2/1   0/0   0/0   1/4
  9 PA09      QSPI_D1   9    9    3    0/1    2/0   0/1   0/1   1/5
 10 PA10      QSPI_D2  10   10    -    0/2    2/2   1/0   0/2   1/6
 11 PA11      QSPI_D3  11   11    -    0/3    2/3   1/1   0/3   1/7
 42 PB10     QSPI_SCK  10    -    -     -     4/2   5/0   0/4   1/0
 80 PC16      SD_MOSI   0    -    -    6/0   0/1     -    0/0    -
 81 PC17       SD_SCK   1    -    -    6/1   0/0     -    0/1    -
 30 PA30        SWCLK  14    -    -    7/2   1/2    6/0   2/0    -
 31 PA31        SWDIO  15    -    -    7/3   1/3    6/1   2/1    -
108 PD12     SWITCH_B   7    -    -     -     -      -    0/5    -
116 PD20     SWITCH_U  10    -    -    1/2   3/2     -    1/0    -
104 PD08     SWITCH_X   3    -    -    7/0   6/1     -    0/1    -
105 PD09     SWITCH_Y   4    -    -    7/1   6/0     -    0/2    -
106 PD10     SWITCH_Z   5    -    -    7/2   6/2     -    0/3    -
 24 PA24       USB_DM   8    -    -    3/2   5/2    5/0   2/2    -
 25 PA25       USB_DP   9    -    -    3/3   5/3    5/1    -     -
  0 PA00            -   0    -    -     -    1/0    2/0    -     -
  1 PA01            -   1    -    -     -    1/1    2/1    -     -
  2 PA02            -   2    0    -     -     -      -     -     -
  3 PA03            -   3   10    -     -     -      -     -     -
  5 PA05            -   5    5    -     -    0/1    0/1    -     -
 14 PA14            -  14    -    -    2/2   4/2    3/0   2/0   1/2
 18 PA18            -   2    -    -    1/2   3/2    3/0   1/2   0/6
 19 PA19            -   3    -    -    1/3   3/3    3/1   1/3   0/7
 23 PA23            -   7    -    -    3/1   5/0    4/1   1/7   0/3
 27 PA27            -  11    -    -     -     -      -     -     -
 46 PB14            -  14    -    -    4/2    -     5/0   4/0   0/2
 49 PB17            -   1    -    -    5/1    -     6/1   3/1   0/5
 54 PB22            -  22    -    -    1/2   5/2    7/0    -     -
 55 PB23            -   7    -    -    1/3   5/3    7/1    -     -
 56 PB24            -   8    -    -    0/0   2/1     -     -     -
 57 PB25            -   9    -    -    0/1   2/0     -     -     -
 60 PB28            -  14    -    -    2/2   4/2     -    1/4    -
 61 PB29            -  15    -    -    2/3   4/3     -    1/5    -
 62 PB30            -  14    -    -    7/0   5/1    0/0   4/0   0/6
 63 PB31            -  15    -    -    7/1   5/0    0/1   4/1   0/7
 64 PC00            -   0    -   10     -     -      -     -     -
 65 PC01            -   1    -   11     -     -      -     -     -
 66 PC02            -   2    -    4     -     -      -     -     -
 67 PC03            -   3    -    5     -     -      -     -     -
 68 PC04            -   4    -    -    6/0    -      -    0/0    -
 84 PC20            -   4    -    -     -     -      -    0/4    -
 85 PC21            -   5    -    -     -     -      -    0/5    -
 86 PC22            -   6    -    -    1/0   3/1     -    0/5    -
 87 PC23            -   7    -    -    1/1   3/0     -    0/7    -
 88 PC24            -   8    -    -    0/2   2/2     -     -     -
 89 PC25            -   9    -    -    0/3   2/3     -     -     -
 95 PC31            -  15    -   13     -     -      -     -     -
 96 PD00            -   0    -   14     -     -      -     -     -
 97 PD01            -   1    -   15     -     -      -     -     -
=== ==== ============ ==== ==== ==== ====== ====== ===== ===== =====

For the definition of the table columns see the explanation at the table for
Adafruit ItsyBitsy M4 Express :ref:`samd51_pinout_table`.

Default pin assignments:
- UART 2 at pins PB27 and PB26, labelled RX and TX
- I2C 4 at pins PA12 and PA13, labelled SCL0 and SDA0
- I2C 3 at pins PA16 and PA17, labelled SCL1 and SDA1
- SPI 5 at pins PB00, PB02 and PB03, labelle MISO, MOSI and SCK

There seems to be no default pin assignment for this board.

Sparkfun SAMD51 Thing Plus pin assignment table
------------------------------------------------

=== ==== ============ ==== ==== ==== ====== ====== ===== ===== =====
Pin GPIO Pin name     IRQ  ADC  ADC  Serial Serial  TC    PWM   PWM
=== ==== ============ ==== ==== ==== ====== ====== ===== ===== =====
  2 PA02           A0   2    0    -     -      -     -     -     -
 40 PB08           A1   8    2    0     -     4/0   4/0    -     -
 41 PB09           A2   9    3    1     -     4/1   4/1    -     -
  4 PA04           A3   4    4    -     -     0/0   0/0    -     -
  5 PA05           A4   5    5    -     -     0/1   0/1    -     -
 34 PB02           A5   2   14    -     -     5/0   6/0   2/2    -
 13 PA13           D0  13    -    -    2/1    4/0   2/1   0/7   1/3
 12 PA12           D1  12    -    -    2/0    4/1   2/0   0/6   1/2
  6 PA06           D4   6    6    -     -     0/2   1/0    -     -
 15 PA15           D5  15    -    -    2/3    4/3   3/1   2/1   1/3
 20 PA20           D6   4    -    -    5/2    3/2   7/0   1/4   0/0
 21 PA21           D7   5    -    -    5/3    3/3   7/1   1/5   0/1
  7 PA07           D9   7    7    -     -     0/3   1/1    -     -
 18 PA18          D10   2    -    -    1/2    3/2   3/0   1/2   0/6
 16 PA16          D11   0    -    -    1/0    3/1   2/0   1/0   0/4
 19 PA19          D12   3    -    -    1/3    3/3   3/1   1/3   0/7
 17 PA17          D13   1    -    -    1/1    3/0   2/1   1/1   0/5
 10 PA10     FLASH_CS  10   10    -    0/2    2/2   1/0   0/2   1/6
 11 PA11   FLASH_MISO  11   11    -    0/3    2/3   1/1   0/3   1/7
  8 PA08   FLASH_MOSI   -    8    2    0/0    2/1   0/0   0/0   1/4
  9 PA09    FLASH_SCK   9    9    3    0/1    2/0   0/1   0/1   1/5
 43 PB11         MISO  12    -    -     -     4/3   5/1   0/5   1/1
 44 PB12         MOSI  12    -    -    4/0     -    4/0   3/0   0/0
 55 PB23          RXD   7    -    -    1/3    5/3   7/1    -     -
 35 PB03        RXLED   9   15    -     -     5/1   6/1    -     -
 45 PB13          SCK  13    -    -    4/1     -    4/1   3/1   0/1
 23 PA23          SCL   7    -    -    3/1    5/0   4/1   1/7   0/3
 22 PA22          SDA   6    -    -    3/0    5/1   4/0   1/6   0/2
 30 PA30        SWCLK  14    -    -    7/2    1/2   6/0   2/0    -
 31 PA31        SWDIO  15    -    -    7/3    1/3   6/1   2/1    -
 54 PB22          TXD  22    -    -    1/2    5/2   7/0    -     -
 27 PA27        TXLED  11    -    -     -      -     -     -     -
 24 PA24       USB_DM   8    -    -    3/2    5/2   5/0   2/2    -
 25 PA25       USB_DP   9    -    -    3/3    5/3   5/1    -     -
  0 PA00            -   0    -    -     -     1/0   2/0    -     -
  1 PA01            -   1    -    -     -     1/1   2/1    -     -
  3 PA03            -   3   10    -     -      -     -     -     -
 14 PA14            -  14    -    -    2/2    4/2   3/0   2/0   1/2
 32 PB00            -   9   12    -     -     5/2   7/0    -     -
 33 PB01            -   1   13    -     -     5/3   7/1    -     -
 36 PB04            -   4    -    6     -      -     -     -     -
 37 PB05            -   5    -    7     -      -     -     -     -
 38 PB06            -   6    -    8     -      -     -     -     -
 39 PB07            -   7    -    9     -      -     -     -     -
 42 PB10            -  10    -    -     -     4/2   5/0   0/4   1/0
 46 PB14            -  14    -    -    4/2     -    5/0   4/0   0/2
 47 PB15            -  15    -    -    4/3     -    5/1   4/1   0/3
 48 PB16            -   0    -    -    5/0     -    6/0   3/0   0/4
 49 PB17            -   1    -    -    5/1     -    6/1   3/1   0/5
 62 PB30            -  14    -    -    7/0    5/1   0/0   4/0   0/6
 63 PB31            -  15    -    -    7/1    5/0   0/1   4/1   0/7
=== ==== ============ ==== ==== ==== ====== ====== ===== ===== =====

For the definition of the table columns see the explanation at the table for
Adafruit ItsyBitsy M4 Express :ref:`samd51_pinout_table`.

The default devices at the board are:

- UART 2 at pins PA13/PA12, labelled RXD/TXD
- I2C 5 at pins PA22/PA23, labelled SDA/SCL
- SPI 4 at pins PB12/PB11/PB13, labelled MOSI, MISO and SCK
- DAC output on pins PA02 and PA05, labelled A0 and A4

Scripts for creating the pin assignment tables
----------------------------------------------

The tables shown above were created with small a Python script running on the target board::

  from samd import pininfo
  from machine import Pin
  import os

  def print_item(e, txt):
      print(txt, end=": ")
      if e == 255:
          print(" - ", end="")
      else:
          print("%d/%d" % (e >> 4, e & 0x0f), end="")

  def print_pininfo(pin_id, name, info):

      print("%3d" % pin_id, end=" ")
      print("%4s %12s" % (info[0], name), end="")
      print(" IRQ:%2s" % (info[1] if info[1] != 255 else "-"), end="")
      print(" ADC0:%2s" % (info[2] if info[2] != 255 else "-"), end="")
      if len(info) == 7:
          print_item(info[3], " Serial1")
          print_item(info[4], " Serial2")
          print_item(info[5], " PWM1" if (info[5] >> 4) < 3 else "   TC")
          print_item(info[6], " PWM2")
      else:
          print(" ADC1:%2s" % (info[3] if info[3] != 255 else "-"), end="")
          print_item(info[4], " Serial1")
          print_item(info[5], " Serial2")
          print_item(info[6], " TC")
          print_item(info[7], " PWM1")
          print_item(info[8], " PWM2")
      print()

  def tblkey(i):
      name = i[1]
      if name != "":
          if len(name) < 3:
              return " " + name
          else:
              return name
      else:
          return "zzzzzzz%03d" % i[0]

  def table(num=127, sort=True):
      pintbl = []
      inv_bd = {v: k for k, v in Pin.board.__dict__.items()}
      for i in range(num):
          try:
              p = Pin(i)
              pi = pininfo(p)
              if p in inv_bd.keys():
                  name = inv_bd[p]
              else:
                  name = ""
              pintbl.append((i, name, pininfo(i)))
          except:
              pass
              # print("not defined")

      if sort:
          pintbl.sort(key=tblkey)
      for item in pintbl:
          print_pininfo(item[0], item[1], item[2])

  table()
