# SparkFun MicroMod nRF52840 Processor

Featuring the nRF52840 SoC from Nordic Semiconductor, the [SparkFun MicroMod nRF52840 Processor](https://www.sparkfun.com/products/16984) offers a powerful combination of ARM Cortex-M4 CPU and 2.4 GHz Bluetooth transceiver in the MicroMod form-factor with the M.2 MicroMod connector to allow you to plug in a compatible MicroMod Carrier Board with any number of peripherals.

The MicroMod nRF52840 Processor features the same Raytac MDBT50Q-P1M found on our [Pro nRF52840 Mini](https://www.sparkfun.com/products/15025). This module includes an integrated trace antenna, fits the IC to an FCC-approved footprint along with including decoupling and timing mechanisms that would need to be designed into a circuit using the bare nRF52840 IC. The Bluetooth transceiver included on the nRF52840 boasts a BT 5.1 stack and supports Bluetooth 5, Bluetooth mesh, IEEE 802.15.4 (Zigbee & Thread) and 2.4Ghz RF wireless protocols (including Nordic's proprietary RF protocol) allowing you to pick which option works best for your application.

We've also routed two I<sup>2</sup>C buses, 2 SPI buses, eleven GPIO, dedicated digital, analog, PWM & PDM pins along with multiple serial UARTS to cover nearly all of your peripheral needs.

## CircuitPython Pin Defs

CircuitPython pin definitions, while simialr to other boards represent a slight departure from just the typical `A` and `D` pin definitions. The majority of general pins are labled as `G` (or alternatively, `BUS`,) as the MicroMod system they build on uses those names to specify pins that may not be specficially analog or digital.

This can be somewhat confusing, especially around the analog pins. Here's a quick pin-map. This pin map will use the label either on the [SparkFun MicroMod ATP Carrier Board](https://www.sparkfun.com/products/16885), or the pin name on the [graphical datasheet](https://cdn.sparkfun.com/assets/learn_tutorials/1/4/0/1/MicroMod_nRF52840_v1.0_Graphical_Datasheet.pdf). Some of the aditional aliases are just names to make naming consistent (e.g.: RTS/CTS), but they also can refer to additional functionality a pin may have (e.g.: NFC pins)

MicroMod Pin # | ATP Pin Label | Pin Definition | Additional Definitons | Pin/Port Reference | Notes
:--------------|:--------------|:--------------|:-----------------------|:-------------------|:------
8 | G11 | | | (Not Connected) |
10 | D0 | D0 | | P0_27 |
11 | BOOT | BOOT | BUTTON1 | P0_07 |
12 | SDA | SDA | | P0_08 |
13 | RTS1 | RTS | RTS1 | P1_02 |
14 | SCL | SCL | | P0_11 |
15 | CTS1 | CTS | CTS1 | P1_09 |
16 | /I2C INT | I2C_INT | P0_15 |
17 | TX | TX | TX1 | P1_03 |
18 | D1 | D1 | CAM_TRIG | P1_08 |
19 | RX | RX | RX1 | P1_10 |
20 | RX2 | RX2 | | P1_05 |
22 | TX2 | TX2 | | P1_07 |
32 | PWM0 | PWM0 | P0_06 |
34 | A0 | A0 | ADC0 | P0_04 | Attached to AIN2
38 | A1 | A1 | ADC1 | P0_05 | Attached to AIN3
40 | G0 | G0 | BUS0 | P0_29 | Attached to AIN5
42 | G1 | G1 | BUS1 | P0_03 | Attached to AIN1
44 | G2 | G2 | BUS2 | P1_13 |
46 | G3 | G3 | BUS3 | P1_12 |
47 | PWM1 | PWM1 | P0_16 |
48 | G4 | G4 | BUS4 | P1_11 |
49 | BATT_VIN | BATT_VIN3 | | P0_30 | Attached to AIN6, will be battery voltage / 3. |
50 | PDM_CLK | PDM_CLK | | P0_25 |
51 | SDA1 | SDA1 | | P1_01 |
52 | PDM_DATA | PDM_DATA | | P0_26 |
53 | SCL1 | SCL1 | | P0_24 |
55 | /CS | CS | | P0_20 |
57 | SCK | SCK | | P0_28 | Attached to AIN4
59 | COPI | COPI | MOSI | P0_31 | Attached to AIN7
61 | CIPO | CIPO | MISO | P0_02 |
63 | G10 | G10 | NFC2, ADC_DP, CAM_VSYNC | P0_10 | Attached to NFC2
65 | G9 | G9 | NFC1, ADC_DM, CAM_HSYNC | P0_09 | Attached to NFC1
67 | G8 | G8 | | P1_14 |
69 | G7 | G7 | BUS7 | P1_04 |
71 | G6 | G6 | BUS6 | P1_06 |
73 | G5 | G5 | BUS5 | P0_15 |

## Peripheral Naming

CircuitPython attempts to stay in line with the naming of the serial peripheral naming in the MicroMod system. The bare UART pins are also named <pin>1. The UART 2 pins are named <pin>2. However, the I2C names on MicroMod are <I2C pin> and <I2C pin>1. Perhaps this will change in the future, but as of [Interface v1](https://cdn.sparkfun.com/assets/learn_tutorials/1/2/0/6/SparkFun_MicroMod_Interface_v1.0_-_Pin_Descriptions.pdf), it may lead to some confusion.


## Bootloader Notes

The MicroMod nRF52840 Processor needs to have the [Adafruit nRF52 UF2 bootloader](https://github.com/adafruit/Adafruit_nRF52_Bootloader/pull/194) flashed on it. [[TODO: LINK TO BUILD]]

## Hardware Reference

The MicroMod nRF52840 Processor hardware layout is open source:

* [Schematic](https://cdn.sparkfun.com/assets/f/0/9/9/e/MicroMod_Processor_Board-nRF52840.pdf)
* [Eagle Files](https://cdn.sparkfun.com/assets/3/0/5/d/a/MicroMod_Processor_Board-nRF52840.zip)
* [Hookup Guide](https://learn.sparkfun.com/tutorials/micromod-nrf52840-processor-hookup-guide)
