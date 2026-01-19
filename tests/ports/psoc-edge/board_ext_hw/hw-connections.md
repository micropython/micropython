# Pin Connections for HIL Test Setup Between 2 Boards

| Test Name         | Board 1 Pin   | Board 2 Pin   | Description                    |
|-------------------|---------------|---------------|--------------------------------|
| Pin               | P16_0         | P16_1         | Output to Input                |
|                   | GND           | GND           | Common Ground                  |
|                   |               |               |                                |
| I2C               | P17_0         | P17_0         | I2C SCL to SCL (with pullup)   |
|                   | P17_1         | P17_1         | I2C SDA to SDA (with pullup)   |
|                   | GND           | GND           | Common Ground                  |
|                   |               |               |                                |