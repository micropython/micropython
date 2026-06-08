# Pin Connections for HIL Test Setup Single Board

| Test Name         | Board 1 Pin   | Board 1 Pin   | Description                    |
|-------------------|---------------|---------------|--------------------------------|
| Pin               | P16_0         | P16_1         | Output to Input                |
|                   |               |               |                                |
| PWM               | P16_1         | P16_0         | PWM Output to IO Input         |
|                   |               |               |                                |
| UART              | P17_0         | P17_1         | UART RX to TX                  |
|                   | P16_5         | P16_6         | UART CTS to RTS                |

# Pin Connections for HIL Test Setup Between 2 Boards

| Test Name         | Board 1 Pin   | Board 2 Pin   | Description                    |
|-------------------|---------------|---------------|--------------------------------|
| I2C               | P17_0         | P17_0         | I2C SCL to SCL (with pullup)   |
|                   | P17_1         | P17_1         | I2C SDA to SDA (with pullup)   |
|                   | GND           | GND           | Common Ground                  |
|                   |               |               |                                |
| SPI               | P16_0         | P16_0         | SPI SCK to SCK                 |
|                   | P16_1         | P16_1         | SPI MOSI to MOSI               |
|                   | P16_2         | P16_2         | SPI MISO to MISO               |
|                   | P16_3         | P16_3         | SPI SSEL to CS out             |
|                   | GND           | GND           | Common Ground                  |

## Board-specific SPI pin sets for multi-board tests

| Board Type                | SCK  | MOSI | MISO | SSEL |
|--------------------------|------|------|------|------|
| KIT_PSE84_AI             | P16_0 | P16_1 | P16_2 | P16_3 |
| CY8CPROTO-062-4343W      | P9_2 | P9_0 | P9_1 | P9_3 |
| CY8CPROTO-063-BLE        | P9_2 | P9_0 | P9_1 | P9_3 |
| CY8CKIT-062S2-AI         | N/A  | N/A  | N/A  | N/A  |

Note: `CY8CKIT-062S2-AI` is currently not supported by `board_ext_hw/multi/spi_basic.py`
for SPI target mode on `P9_x`, so the test is skipped on that board.
