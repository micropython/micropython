# SparkFun MicroMod nRF52840 Processor

Featuring the nRF52840 SoC from Nordic Semiconductor, the [SparkFun MicroMod nRF52840 Processor](https://www.sparkfun.com/products/16984) offers a powerful combination of ARM Cortex-M4 CPU and 2.4 GHz Bluetooth transceiver in the MicroMod form-factor with the M.2 MicroMod connector to allow you to plug in a compatible MicroMod Carrier Board with any number of peripherals.

The MicroMod nRF52840 Processor features the same Raytac MDBT50Q-P1M found on our [Pro nRF52840 Mini](https://www.sparkfun.com/products/15025). This module includes an integrated trace antenna, fits the IC to an FCC-approved footprint along with including decoupling and timing mechanisms that would need to be designed into a circuit using the bare nRF52840 IC. The Bluetooth transceiver included on the nRF52840 boasts a BT 5.1 stack and supports Bluetooth 5, Bluetooth mesh, IEEE 802.15.4 (Zigbee & Thread) and 2.4Ghz RF wireless protocols (including Nordic's proprietary RF protocol) allowing you to pick which option works best for your application.

We've also routed two I<sup>2</sup>C buses, 2 SPI buses, eleven GPIO, dedicated digital, analog, PWM & PDM pins along with multiple serial UARTS to cover nearly all of your peripheral needs.

## CircuitPython Pin Defs

CircuitPython pin definitions, while similar to other boards represent a slight departure from just the typical `A` and `D` pin definitions. The majority of general pins are labeled as `G` (or alternatively, `BUS`,) as the MicroMod system they build on uses those names to specify pins that may not be specficially analog or digital.

This can be somewhat confusing, especially around the analog pins. Here's a quick pin-map:

MicroMod Pin # | ATP Pin Label | Pin Definition | Additional Definitions | Pin/Port Reference | Notes
:--------------|:--------------|:--------------|:-----------------------|:-------------------|:------
8 | G11 | | | (Not Connected) |
10 | D0 | D0 | | P0_27 |
11 | BOOT | BOOT |  | P0_07 |
12 | SDA | I2C_SDA | SDA | P0_08 |
13 | RTS1 | UART_RTS1 |  | P1_02 |
14 | SCL | I2C_SCL | SCL | P0_11 |
15 | CTS1 | UART_CTS1 |  | P1_09 |
16 | /I2C INT | I2C_INT |  |P0_15|
17 | TX | UART_TX1 | TX | P1_03 |
18 | D1 | D1 |  | P1_08 |
19 | RX | UART_RX1 | RX | P1_10 |
20 | RX2 | UART_RX2 | | P1_05 |
22 | TX2 | UART_TX2 | | P1_07 |
32 | PWM0 | PWM0 |  |P0_06|
34 | A0 | A0 |  | P0_04 | Attached to AIN2
38 | A1 | A1 |  | P0_05 | Attached to AIN3
40 | G0 | G0 | BUS0 | P0_29 | Attached to AIN5
42 | G1 | G1 | BUS1 | P0_03 | Attached to AIN1
44 | G2 | G2 | BUS2 | P1_13 |
46 | G3 | G3 | BUS3 | P1_12 |
47 | PWM1 | PWM1 |  |P0_16|
48 | G4 | G4 | BUS4 | P1_11 |
49 | BATT_VIN | BATT_VIN3 | | P0_30 | Attached to AIN6, will be battery voltage / 3.
50 | PDM_CLK | PDM_CLK | | P0_25 |
51 | SDA1 | I2C_SDA1 | | P1_01 |
52 | PDM_DATA | PDM_DATA | | P0_26 |
53 | SCL1 | I2C_SCL1 | | P0_24 |
55 | /CS | SPI_CS | | P0_20 |
57 | SCK | SPI_SCK | LED_CLK | P0_28 | Attached to AIN4
59 | COPI | SPI_COPI | SPI_MOSI, LED_DAT | P0_31 | Attached to AIN7
60 | SCK1 | SDIO_SCK | SPI_SCK1 |  |
61 | CIPO | SPI_CIPO | SPI_MISO | P0_02 |
62 | COPI1 | SDIO_CMD | SPI_COPI1 |  |
63 | G10 | G10 |  | P0_10 | Attached to NFC2
64 | CIPO1 | SDIO_DATA0 | SPI_CIPO1 |  |
65 | G9 | G9 |  | P0_09 | Attached to NFC1
66 | DAT1 | SDIO_DATA1 |  |  |
67 | G8 | G8 | | P1_14 |
68 | DAT2 | SDIO_DATA2 | |  |
69 | G7 | G7 | BUS7 | P1_04 |
70 | CS1 | SDIO_DATA3 | SPI_CS1 |  |
71 | G6 | G6 | BUS6 | P1_06 |
73 | G5 | G5 | BUS5 | P0_15 |

## Peripheral Naming

The SparkFun MicroMod spec uses a zero-based peripheral numbering scheme. The 0th peripheral is the default and the "0" is omitted from the peripheral name.  For example, the first I2C peripheral is named `I2C` (instead of `I2C0`) and the second I2C peripheral is named `I2C1`. Note: MicroMod `UART`  is not present in the edge connector pinout because the primary debug serial port (i.e.`UART0`) is exposed as a virtual serial port over USB. As a result, the first UART peripheral in the edge connector pinout is `UART1` and the second UART peripheral is `UART2`.

For more details, see https://www.sparkfun.com/micromod#tech-specs.


## Bootloader Notes

The MicroMod nRF52840 Processor needs to have the [Adafruit nRF52 UF2 bootloader](https://github.com/adafruit/Adafruit_nRF52_Bootloader/releases/latest) flashed on it.

## Hardware Reference

The MicroMod nRF52840 Processor hardware layout is open source:

* [Schematic](https://cdn.sparkfun.com/assets/f/0/9/9/e/MicroMod_Processor_Board-nRF52840.pdf)
* [Eagle Files](https://cdn.sparkfun.com/assets/3/0/5/d/a/MicroMod_Processor_Board-nRF52840.zip)
* [Hookup Guide](https://learn.sparkfun.com/tutorials/micromod-nrf52840-processor-hookup-guide)
