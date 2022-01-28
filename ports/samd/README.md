Port of MicroPython to Microchip SAMD MCUs
==========================================

Supports SAMD21 and SAMD51.

## Features:

### REPL

- REPL over USB VCP
- REPL over USART using board specified USART pins (initialised on startup).
  - The USART Pins are board specific, defined in `boards/$(BOARD)/mpconfigboard.h`,
    and set at compile time. See the table below. At this stage, the USART cannot be
    moved to different pins unless `mpconfigboard.h` is edited and the port recompiled.
  - Two USART functions are accessible through MicroPython:
    - `uart_init()`. The 'C' function behind this function is what initialises the
      USART on startup. Calling this function in MicroPython resets any other peripheral
      operating on these pins and reconnects the USART SERCOM to the designated pins.
    - `uart_deinit()`. This simply 'disconnects' the SERCOM from the pins. The USART
      remains operating over USB VCP to maintain access to the REPL.

### Boards

| Board                                                        | USART                                                       | LFS1 Flash size | Tested |
| ------------------------------------------------------------ | ----------------------------------------------------------- | --------------- | ------ |
| ADAFRUIT_FEATHER_M0_EXPRESS                                  | Tx=PA10=SERCOM0/PAD[2], Rx=PA11=SERCOM0/PAD[3]              | 64k             | No     |
| ADAFRUIT_ITSYBITSY_M4_EXPRESS                                | Tx=TX_D1=PA17=SERCOM3/PAD[0],  Rx=RX_D0=PA16=SERCOM3/PAD[1] | 128k            | No     |
| ADAFRUIT_TRINKET_M0                                          | Tx=D4=PA06=SERCOM0/PAD[2], Rx=D3=PA07=SERCOM0/PAD[3]        | 64k             | No     |
| MINISAM_M4                                                   | Tx=TX_D1=PA17=SERCOM3/PAD[0], Rx=RX_D0=PA16=SERCOM3/PAD[1]  | 128k            | No     |
| SAMD21_XPLAINED_PRO                                          | Tx=PA10=SERCOM0/PAD[2], Rx=PA11=SERCOM0/PAD[3]              | 64k             | No     |
| SEEED_WIO_TERMINAL                                           | Tx=BCM14=PB27=SERCOM2/PAD[0], Rx=BCM15=PB26=SERCOM2/PAD[1]  | 128k            | Yes    |
| SEEED_XIAO                                                   | Tx=A6=PB8=SERCOM4/PAD[0], Rx=A7=PB9=SERCOM4/PAD[1]          | 64k             | Yes    |

Note: all USARTs are set to: async, 8 bit, 1 stop bit, no parity, 115200 bps.

### Modules

- Internal modules and functions:

`>>>help('modules')`
`__main__          micropython       uheapq            ustruct`
`_boot             samd              uio               usys`
`_uasyncio         uarray            ujson             utime`
`builtins          uasyncio          uos               uzlib`
`gc                ubinascii         urandom`
`machine           uctypes           ure`
`Plus any modules on the filesystem`

#### Flash

- Internal Flash Block Device `samd.Flash()` initialised with littlefs1 in frozen module '`_boot.py`'.

- **No external SPI Flash driver** (ToDo).

- Block Device size is set in `ports/samd/boards/$(BOARD)/mpconfigboard.h` :

  - SAMD21: (eg; SEEED_XIAO): 64k `0xFFFF`

   *  SAMD51: (eg; M4's): 128k `0x1FFFF`

#### Pins & LEDs

##### `machine.Pin()` class.

- GPIO methods & constants:

    value           IN              OUT             PULL_DOWN
    PULL_UP         high            init            low
    off             on              toggle

- Each board has its own pin numbering scheme, so please see the table below (the
  structure is defined in`boards/$(BOARD)/pins.c`) for pin numbers referenced
  (index) by 'Pin'. Eg; `SEEED_XIAO/pins.c`: `{{&machine_pin_type}, PIN_PA02}, // A0/D0`
  means MicroPython `Pin(0)` is SEEED_XIAO pin "A0/D0" on SAMD21G18A PortA, Pin2.

- Note: on the SEEED_XIAO, if the TX & TX pins are used by the `Pin()` class, the `Pin()`
  initialisation disconnects the pins from the SERCOM similar to the way
  `machine.uart_deinit()` does.

| MicroPython Pin() | SAMD51 Pin#/  ItsyBitsy_M4 | SAMD21 Pin#/  Feather_M0 | SAMD21 Pin#/  Xplained Pro          | SAMD21 Pin#/  Trinket_M0 | SAMD51 Pin#/ Minisam | SAMD21 Pin#/ SEEED_XIAO | SAMD51 Pin#/  SEEED_WIO_TERMINAL      |
| ----------------- | -------------------------- | ------------------------ | ----------------------------------- | ------------------------ | -------------------- | ----------------------- | ------------------------------------- |
| Pin(0)            | PA16/ RX_D0                | PA11/ D0                 | PB00/ PIN3_ADC(+) (ext1)            | PA08/ D0                 | PA02/ A0,D9          | PA02 / A0/D0            | PB08 / A0/D0                          |
| Pin(1)            | PA17/ TX_D1                | PA10/ D1                 | PB01/ PIN4_ADC(-) (ext1)            | PA02/ D1                 | PB08/ A1,D10         | PA04 / A1/D1            | PB09 / A1/D1                          |
| Pin(2)            | PA07/ D2                   | PA14/ D2                 | PB06/ PIN5_GPIO (ext1)              | PA09/ D2                 | PB09/ A2,D11         | PA10 / A2/D2            | PA07 / A2/D2                          |
| Pin(3)            | PB22/ D3                   | PA09/ D3/                | PB07/ PIN6_GPIO (ext1)              | PA07/ D3/ RxD            | PA04/ A3,D12         | PA11 / A3/D3            | PB04 / A3/D3                          |
| Pin(4)            | PA14/ D4                   | PA08/ D4/                | PB02/ PIN7_PWM(+) (ext1)            | PA06/ D4/ TxD            | PA05/ A4,D13         | PA08 / A4/D4            | PB05 / A4/D4                          |
| Pin(5)            | PA15/ D5                   | PA15/ D5                 | PB03/ PIN8_PWM(-) (ext1)            |                          | PA06/ A5             | PA09 / A5/D5            | PB06 / A5/D5                          |
| Pin(6)            | -1/ D6                     | PA20/ D6                 | PB04/ PIN9_IRQ/GPIO (ext1)          |                          | PA16/ RX_D0          | PB08 / A6/D6/TX         | PA04 / A6/D6                          |
| Pin(7)            | PA18/ D7                   | PA21/ D7                 | PB05/ PIN10_SPI_SS_B/GPIO (ext1)    |                          | PA17/ TX_D1          | PB09 / A7/D7/RX         | PB07 / A7/D7                          |
| Pin(8)            | -1/ D8                     | PA06/ D8/                | PA08/ PIN11_TWI_SDA (ext1)          |                          | PA07/ D2,A6          | PA07 / A8/D8            | PA06 / A8/D8                          |
| Pin(9)            | PA19/ D9                   | PA07/ D9/                | PA09/ PIN12_TWI_SCL (ext1)          |                          | PA19/ D3             | PA05 / A9/D9            | PD08 / SWITCH_X                       |
| Pin(10)           | PA20/ D10                  | PA18/ D10                | PB09/ PIN13_UART_RX (ext1)          |                          | PA20/ D4             | PA06 / A10/D10          | PD09 / SWITCH_Y                       |
| Pin(11)           | PA21/ D11                  | PA16/ D11                | PB08/ PIN14_UART_TX (ext1)          |                          | PA21/ D5             |                         | PD10 / SWITCH_Z                       |
| Pin(12)           | PA23/ D12                  | PA19/ D12                | PA05/ PIN15_SPI_SS_A (ext1)         |                          | PA00/ BUTTON         |                         | PD12 / SWITCH_B                       |
| Pin(13)           | PA22/ D13                  | PA17/ D13/               | PA06/ PIN16_SPI_MOSI (ext1)         |                          |                      |                         | PD20 / SWITCH_U                       |
| Pin(14)           | PA02/ A0                   | PA02/ A0                 | PA04/ PIN17_SPI_MISO (ext1)         |                          |                      |                         | PC26 / BUTTON_1                       |
| Pin(15)           | PA05/ A1                   | PB08/ A1                 | PA07/ PIN18_SPI_SCK (ext1)          |                          |                      |                         | PC27 / BUTTON_2                       |
| Pin(16)           | PB08/ A2                   | PB09/ A2                 | PA10/ PIN3_ADC(+) (ext2)            |                          |                      |                         | PC28 / BUTTON_3                       |
| Pin(17)           | PB09/ A3                   | PA04/ A3/                | PA11/ PIN4_ADC(-) (ext2)            |                          |                      |                         | PD11 / BUZZER_CTR                     |
| Pin(18)           | PA04/ A4                   | PA05/ A4/                | PA20/ PIN5_GPIO (ext2)              |                          |                      |                         | PC14/ 5V_OUTPUT_CTR- '1'= 5V on hdr   |
| Pin(19)           | PA06/ A5                   | PB02/ A5                 | PA21/ PIN6_GPIO (ext2)              |                          |                      |                         | PC15/ 3V3_OUTPUT_CTR- '0'= 3V3 on hdr |
| Pin(20)           |                            |                          | PB12/ PIN7_PWM(+) (ext2)            |                          |                      |                         |                                       |
| Pin(21)           |                            |                          | PB13/ PIN8_PWM(-) (ext2)            |                          |                      |                         |                                       |
| Pin(22)           |                            |                          | PB14/ PIN9_IRQ/GPIO (ext2)          |                          |                      |                         |                                       |
| Pin(23)           |                            |                          | PB15/ PIN10_SPI_SS_B/GPIO (ext2)    |                          |                      |                         |                                       |
| Pin(24)           |                            |                          | -1 / PIN11_TWI_SDA already defined  |                          |                      |                         |                                       |
| Pin(25)           |                            |                          | -1 / PIN12_TWI_SCL already defined  |                          |                      |                         |                                       |
| Pin(26)           |                            |                          | PB11/ PIN13_UART_RX (ext2)          |                          |                      |                         |                                       |
| Pin(27)           |                            |                          | PB10/ PIN14_UART_TX (ext2)          |                          |                      |                         |                                       |
| Pin(28)           |                            |                          | PA17/ PIN15_SPI_SS_A (ext2)         |                          |                      |                         |                                       |
| Pin(29)           |                            |                          | PA18/ PIN16_SPI_MOSI (ext2)         |                          |                      |                         |                                       |
| Pin(30)           |                            |                          | PA16/ PIN17_SPI_MISO (ext2)         |                          |                      |                         |                                       |
| Pin(31)           |                            |                          | PA19/ PIN18_SPI_SCK (ext2)          |                          |                      |                         |                                       |
| Pin(32)           |                            |                          | PA02/ PIN3_ADC(+) (ext3)            |                          |                      |                         |                                       |
| Pin(33)           |                            |                          | PA03/ PIN4_ADC(-) (ext3)            |                          |                      |                         |                                       |
| Pin(34)           |                            |                          | -1/ PIN5_GPIO already defined       |                          |                      |                         |                                       |
| Pin(35)           |                            |                          | PA15/ PIN6_GPIO; USER_BUTTON (ext3) |                          |                      |                         |                                       |
| Pin(36)           |                            |                          | PA12/ PIN7_PWM(+) (ext3)            |                          |                      |                         |                                       |
| Pin(37)           |                            |                          | PA13/ PIN8_PWM(-) (ext3)            |                          |                      |                         |                                       |
| Pin(38)           |                            |                          | PA28/ PIN9_IRQ/GPIO (ext3)          |                          |                      |                         |                                       |
| Pin(39)           |                            |                          | PA27/ PIN10_SPI_SS_B/GPIO (ext3)    |                          |                      |                         |                                       |
| Pin(40)           |                            |                          | -1/ PIN11_TWI_SDA already defined   |                          |                      |                         |                                       |
| Pin(41)           |                            |                          | -1/ PIN12_TWI_SCL already defined   |                          |                      |                         |                                       |
| Pin(42)           |                            |                          | -1/ PIN13_UART_RX already defined   |                          |                      |                         |                                       |
| Pin(43)           |                            |                          | -1/ PIN14_UART_TX already defined   |                          |                      |                         |                                       |
| Pin(44)           |                            |                          | PA15/ PIN6_GPIO; USER_BUTTON (ext3) |                          |                      |                         |                                       |
| Pin(45)           |                            |                          | PB22/ PIN16_SPI_MOSI (ext3)         |                          |                      |                         |                                       |
| Pin(46)           |                            |                          | PB16/ PIN17_SPI_MISO (ext3)         |                          |                      |                         |                                       |
| Pin(47)           |                            |                          | PB23/ PIN18_SPI_SCK (ext3)          |                          |                      |                         |                                       |

##### `machine.LED()` class.

- GPIO methods & constants:

`value           OUT             high            low`
`off             on              toggle`

- As above, please see `boards/$(BOARD)/pins.c` for pin numbers referenced by 'LED'.
Eg; `SEEED_XIAO/pins.c`: `{{&machine_led_type}, PIN_PA17}, // W13` means MicroPython
`LED(0)` is SEEED_XIAO LED "W13" connected to SAMD21G18A PortA, Pin17.

| MicroPython  LED() | SAMD51 Pin#/  ItsyBitsy_M4 | SAMD21 Pin#/  Feather_M0 | SAMD21 Pin#/  Xplained Pro | SAMD21 Pin#/  Trinket_M0 | SAMD51  Pin#/ Minisam | SAMD21  Pin#/ SEEED_XIAO | SAMD51  Pin#/ SEEED_WIO_TERMINAL |
| ------------------ | -------------------------- | ------------------------ | -------------------------- | ------------------------ | --------------------- | ------------------------ | -------------------------------- |
| LED(0)             | PA22/ D13/ user LED        | PA17/ D13/ user LED      | PB30/ USER_LED             | PA10/ USER_LED           | PA15/ LED             | PA17 / W13               | PA15 / USER_LED (Blue)           |
| LED(1)             |                            |                          |                            |                          |                       | PA18 / RX_LED            | PC05 / LCD_BACKLIGHT_CTR         |
| LED(2)             |                            |                          |                            |                          |                       | PA19 / TX_LED            |                                  |
