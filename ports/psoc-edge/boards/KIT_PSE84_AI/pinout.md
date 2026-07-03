## Pin Headers

| Pin   | (Pin.board.name) | PWM      | SPI        | I2C       | UART       | PDM       | Remarks                                        |
|-------|------------------|----------|------------|-----------|------------|-----------|------------------------------------------------|
| P9_0  | -                | PWM7     | SPI1_SEL0  | -         | UART1_RTS  | PDM5_CLK  | * MicroPython does not support HW select line. |
| P9_1  | -                | PWM0     | SPI1_MISO  | -         | UART1_CTS  | PDM5_DATA |                                                |
| P9_2  | -                | PWM279   | SPI1_MOSI  | I2C1_SDA  | UART1_TX   | PDM4_CLK  |                                                |
| P9_3  | -                | PWM256   | SPI1_CLK   | I2C1_SCL  | UART1_RX   | PDM4_DATA |                                                |
| P11_1 | -                | -        | -          | -         | -          | -         | * Only input (GPIO).                           |
| P16_0 | -                | PWM0/256 | SPI10_CLK  | I2C10_SCL | UART10_RX  | -         |                                                |
| P16_1 | -                | PWM1/257 | SPI10_MOSI | I2C10_SDA | UART10_TX  | -         |                                                |
| P16_2 | -                | PWM2/258 | SPI10_MISO | -         | UART10_CTS | -         |                                                |
| P16_3 | -                | PWM3/259 | SPI10_SEL0 | -         | UART10_RTS | -         | * MicroPython does not support HW select line. |
| P16_4 | -                | PWM4/260 | SPI10_SEL1 | -         | -          | -         |                                                |
| P16_5 | -                | PWM5/261 | SPI5_MISO  | -         | UART5_CTS  | -         |                                                |
| P16_6 | -                | PWM6/278 | SPI5_SEL0  | -         | UART5_RTS  | -         |                                                |
| P16_7 | -                | PWM7/279 | SPI5_SEL1  | -         | -          | -         |                                                |
| P17_0 | I2C_SCL_3V3      | -        | SPI5_CLK   | I2C5_SCL  | UART5_RX   | -         | * Pull Up Resistor (4.7 K)                     |
| P17_1 | I2C_SDA_3V3      | -        | SPI5_MOSI  | I2C5_SDA  | UART5_TX   | -         | * Pull Up Resistor (4.7 K)                     |
| P17_2 | -                | -        | SPI11_CLK  | I2C11_SCL | UART11_RX  | -         |                                                |
| P17_3 | -                | -        | SPI11_MOSI | I2C11_SDA | UART11_TX  | -         |                                                |
| P17_4 | -                | -        | -          | -         | -          | -         |                                                |
| P17_5 | -                | -        | SPI11_MISO | -         | UART11_CTS | -         |                                                |
| P17_7 | -                | -        | SPI11_SEL1 | -         | -          | -         |                                                |

### Note
When a pin provides signals for certain (multi line) bus protocol, but the exposed
pins do not provide all the required signals for that instantiation, these are omitted.
Only signals which allow a functional instantiation on a peripheral are listed.

### Notes to add to the pinout diagram

* Logic Level 3.3V on P16, P17.
* Logic Level 1.8V on the rest of the ports.

## Hirose Connector

| Pin   | (Pin.board.name) | ADC  | PWM      | SPI       | I2C      | UART      | PDM       | Remarks                                        |
|-------|------------------|------|----------|-----------|----------|-----------|-----------|------------------------------------------------|
| P13_0 | -                | -    | -        | -         | -        | -         | -         |                                                |
| P13_1 | -                | -    | PWM4/268 | SPI7_CLK  | I2C7_SCL | UART7_RX  | -         |                                                |
| P13_2 | -                | -    | -        | SPI7_MOSI | I2C7_SDA | UART7_TX  | PDM1_CLK  |                                                |
| P13_3 | -                | -    | PWM5/269 | SPI7_MISO | -        | UART7_CTS | PDM1_DATA |                                                |
| P13_4 | -                | -    | -        | SPI7_SEL0 | -        | UART7_RTS | -         | * MicroPython does not support HW select line. |
| P13_5 | -                | -    | -        | -         | -        | -         | -         |                                                |
| P13_6 | -                | -    | PWM6/270 | SPI7_SEL1 | -        | -         | -         |                                                |
| P13_7 | -                | -    | -        | -         | -        | -         | -         |                                                |
| P15_0 | ADC0             | ADC0 | PWM2/274 | SPI9_CLK  | I2C9_SCL | UART9_RX  | -         |                                                |
| P15_1 | ADC1             | ADC1 | -        | SPI9_MOSI | I2C9_SDA | UART9_TX  | -         |                                                |
| P15_2 | ADC2             | ADC2 | PWM3/275 | SPI9_MISO | -        | UART9_CTS | -         |                                                |
| P15_3 | ADC3             | ADC3 | -        | SPI9_SEL0 | -        | UART9_RTS | -         | * MicroPython does not support HW select line. |
| P15_4 | ADC4             | ADC4 | PWM4/276 | SPI9_SEL1 | -        | -         | -         |                                                |
| P15_5 | ADC5             | ADC5 | -        | -         | -        | -         | -         |                                                |
| P15_6 | ADC6             | ADC6 | PWM5/277 | -         | -        | -         | -         |                                                |
| P15_7 | ADC7             | ADC7 | -        | -         | -        | -         | -         |                                                |

## On-board Peripherals Without Accessible Pins

| Pin   | (Pin.board.name) | Peripheral                                      |
|-------|------------------|-------------------------------------------------|
| P3_0  | I3C_SCL          | Magnetic Sensor                                 |
| P3_1  | I3C_SDA          | Magnetic Sensor                                 |
| P6_4  | MAG_INT          | Magnetic Sensor                                 |
| P6_6  | IMU2_INT         | IMU Sensor                                      |
| P7_0  | USER_BUTTON      | User Button                                     |
| P7_7  | IMU1_INT         | IMU Sensor                                      |
| P8_0  | I2C_SCL_1V8      | Humidity Sensor, Pressure Sensor, IMU Sensor    |
| P8_1  | I2C_SDA_1V8      | Humidity Sensor, Pressure Sensor, IMU Sensor    |
| P8_5  | PDM_CLK          | Digital Microphones                             |
| P8_6  | PDM_DATA         | Digital Microphones                             |
| P10_5 | USER_LED2        | User LED2                                       |
| P10_7 | USER_LED1        | User LED1                                       |
| P11_3 | PRESS_SENS_INT   | Pressure Sensor                                 |
| P12_3 | I2S_TX_FYSYNC    | Audio Amplifier                                 |
| P14_0 | AMIC1_CTB_INP    | Analog Microphone 1                             |
| P14_1 | AMIC1_CTB_INN    | Analog Microphone 1                             |
| P14_2 | AMIC1_CTB_OUT    | Analog Microphone 1                             |
| P14_3 | AMIC2_CTB_OUT    | Analog Microphone 2                             |
| P14_4 | AMIC2_CTB_INP    | Analog Microphone 2                             |
| P14_5 | AMIC2_CTB_INN    | Analog Microphone 2                             |
| P14_6 | AMIC1_CTB_REF    | Analog Microphone 1                             |
| P14_7 | AMIC2_CTB_REF    | Analog Microphone 2                             |
| P20_3 | RADAR_INT        | Radar                                           |
| P20_4 | USER_LED_G       | RGB LED Green Channel                           |
| P20_5 | USER_LED_B       | RGB LED Blue Channel                            |
| P20_6 | USER_LED_R       | RGB LED Red Channel                             |
| P20_7 | RADAR_RESET      | Radar                                           |
| P21_1 | I2S_TX_SD        | Audio Amplifier                                 |
| P21_2 | I2S_TX_SCK       | Audio Amplifier                                 |
| P21_3 | I2S_TX_MCK       | Audio Amplifier                                 |
| P21_4 | RADAR_SPI_MISO   | Radar                                           |
| P21_5 | RADAR_SPI_MOSI   | Radar                                           |
| P21_6 | RADAR_SPI_CLK    | Radar                                           |
| P21_7 | RADAR_SPI_CS     | Radar                                           |
