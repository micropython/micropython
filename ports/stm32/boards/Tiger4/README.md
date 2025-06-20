# STM32F7406NE_Customboard

This repository contains the MicroPython board definition files for a custom STM32F746NE board. The firmware can be built and flashed to your board for MicroPython development.

## Pin Configuration
Pin definitions are located in `pins.csv`.

## Building Firmware
Ensure you have the necessary dependencies and tools installed. You can find detailed instructions on setting up your build environment in the MicroPython documentation:
Getting Started:
https://docs.micropython.org/en/latest/develop/gettingstarted.html

Clone the MicroPython repository and navigate to the board directory:
```
cd micropython/ports/stm32/boards
git clone https://github.com/Sebastian2513/STM32F746NE_Customboard.git
cd ..
make BOARD=STM32F746NE_Customboard
```

## Programming the Board
There are several options for flashing the firmware on to the STM32F746NE board:

* DFU Mode: To put the board in DFU mode, connect BOOT0 to the appropriate pin.
* STM32CubeProgrammer: Recommended tool for flashing the firmware.

Once Flash it should boot up MicroPython on the REPL:
```
MicroPython v1.24.0-preview.90.g75350f9c8.dirty on 2024-08-19; Tiger 4 with STM32F746NE
Type "help()" for more information.
>>> 
```
## IDEs for Development
* Thonny: Recommended IDE for testing and running MicroPython scripts.
* PyCharm: The MicroPython plugin is available, but may not be fully up to date.

---
## Pins configuration
#### Exposed Port Pins

* IO_Alarm: PC13
* IO_Alarm2: PI8
* IO_Analog_0: PB0
* IO_Analog_1: PB1
* IO_Analog_2: PC2
* IO_Analog_3: PC3
* IO_USB_ID: PB12
* IO_USB_VBUS: PB13
* IO_USB_DP: PB14
* IO_USB_DM: PB15
* OSC25MHz_IN: PH0
* OSC25MHz_OUT: PH1
* OSC32MHz_IN: PC14
* OSC32MHz_OUT: PC15
* IO_PD11: PD11
* IO_L01_3: PA3
* IO_L01_6: PA6
* IO_L02_0: PA8
* IO_L03_1-IO_L03_7: PI9-PI15
* IO_L04_7: PC7
* IO_L05_0-IO_L05_4: PC8-PC12
* IO_L06_0-IO_L06_7: PJ0-PJ6
* IO_L07_2: PD2
* IO_L07_3: PD3
* IO_L07_7: PD7
* IO_L08_0-IO_L08_7: PI0-PI7  (IO_L08_1[PI1] is configured to SD_SW)
* IO_L09_4-IO_L09_7: PB12-PB15
* IO_L14_1: PH9
* IO_L14_2: PH10
* IO_L14_6: PH14
* IO_L14_7: PH15
* IO_L15_2: PG10

#### opt. HW Type Selection - _Needs a resistor to operate_
* Type0: PE3
* Type1: PE4
* Type2: PE5
* Type3: PE6

## Peripherals

#### LED(Red) - _Needs a resistor to operate_

* LED_R: PG6

#### QSPI Flash W25Q64FV (8MB)

* QSPI_BK1_NCS: PB6
* QSPI_CLK: PB2
* QSPI_BK1_IO0: PF8
* QSPI_BK1_IO1: PD12
* QSPI_BK1_IO2: PE2
* QSPI_BK1_IO3: PD13

#### SPI Flash W25Q64FV (8MB)

* SPI1_CS: PA4
* SPI1_SCK: PA5
* SPI1_MISO: PB4
* SPI1_MOSI: PB5

#### MicroSD Slot - _there's no card detect pin_

* SDMMC1_D0: PC8
* SDMMC1_D1: PC9
* SDMMC1_D2: PC10
* SDMMC1_D3: PC11
* SDMMC1_CK: PC12
* SDMMC1_CMD: PD2
* SD_SW: PI1 (dummy, no switch at this pin(needs to pull low manually))

#### USB(Adapter)

* USB_ID: PA10
* USB_DM: PA11
* USB_DP: PA12

#### Ethernet RMII mode

* ETHPWR: PG7 (Pull low to provide power to the Ethernet IC)
* ETH_MDC: PC1
* ETH_MDIO: PA2
* ETH_REF_CLK: PA1
* ETH_CRS_DV: PA7
* ETH_RXD0: PC4
* ETH_RXD1: PC5
* ETH_TX_EN: PG11
* ETH_TXD0: PG13
* ETH_TXD1: PG14

#### EEPROMs (For Mac Address)

* IO_I2C2_SCL: PH4
* IO_I2C2_SDA: PH5

#### SDRAM W9864G6KH-6I

* SDRAMPWR,PG3 (Pull low to provide power to the SDRAM)
* SDRAM_SDCKE0,PH2
* SDRAM_SDNE0,PH3
* SDRAM_SDCLK,PG8
* SDRAM_SDNCAS,PG15
* SDRAM_SDNRAS,PF11
* SDRAM_SDNWE,PC0
* SDRAM_BA0,PG4
* SDRAM_BA1,PG5
* SDRAM_NBL0,PE0
* SDRAM_NBL1,PE1
* SDRAM_A0,PF0
* SDRAM_A1,PF1
* SDRAM_A2,PF2
* SDRAM_A3,PF3
* SDRAM_A4,PF4
* SDRAM_A5,PF5
* SDRAM_A6,PF12
* SDRAM_A7,PF13
* SDRAM_A8,PF14
* SDRAM_A9,PF15
* SDRAM_A10,PG0
* SDRAM_A11,PG1
* SDRAM_D0,PD14
* SDRAM_D1,PD15
* SDRAM_D2,PD0
* SDRAM_D3,PD1
* SDRAM_D4,PE7
* SDRAM_D5,PE8
* SDRAM_D6,PE9
* SDRAM_D7,PE10
* SDRAM_D8,PE11
* SDRAM_D9,PE12
* SDRAM_D10,PE13
* SDRAM_D11,PE14
* SDRAM_D12,PE15
* SDRAM_D13,PD8
* SDRAM_D14,PD9
* SDRAM_D15,PD10

---

### Bus Pinouts

#### Dummy SD Card detect pin

| SD_SW | ETHPWR | SDRAMPWR |
|-------|--------|----------|
|  PC3  |   PG7  |   PG3    |

#### UART

|       | UART1 | UART2 | UART6 | UART7 |
|-------|-------|-------|-------|-------|
|  TX   |  PA9  |  PD5  |  PC6  |  PF6  | 
|  RX   |  PB7  |  PD6  |  PC7  |  PF7  |

#### I2C (I2C3_SCL pin is the same as SPI5_MISO)

|       | I2C3  |  I2C4 |
|-------|-------|-------|
|  SCL  |  PH7  |  PH11 |
|  SDA  |  PH8  |  PH12 |

#### SPI

|       | SPI1  | SPI5  | 
|-------|-------|-------|
|  CS   |  PA4  |  PF10 |
|  SCK  |  PA5  |  PH6  |
|  MISO |  PB4  |  PH7  |
|  MOSI |  PB5  |  PF9  |

#### CAN 

|    | CAN1 |
|----|------|
| TX | PH13 |
| RX | PI9  |
