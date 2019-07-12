# QiMing STM32F103RCTx

MicroPython board definition files for the QiMingXinXin STM32F103RCTx dev board.

**Brand:** QiMingXinXin (unlabeled)

**Markings:** IMT 103RC v1.2

![board](docs/STM32F103RCT6_front.jpg)

![board](docs/STM32F103RCT6_back.jpg)

### Build and deploy the firmware:

* Clone the board definitions to your [MicroPython](https://github.com/micropython/micropython)
  `ports/stm32f1/boards` folder.

```
cd micropython/ports/stm32f1/boards
git clone https://github.com/lonphy/QiMing_F103RC.git
```

* Disconnect the board from USB
* Set BOOT0 jumper to ON (B0->3V3)
* Connect the board via USB

```
cd micropython/ports/stm32
make BOARD=QiMing_F103RC
make BOARD=QiMing_F103RC deploy
```

* Disconnect the board from USB
* Set BOOT0 jumper to OFF (B0->GND)
* Connect the board via USB

```
$ screen /dev/ttyACM0
```

### Specifications:

* STM32F103RCT6 ARM Cortex M3 / 72MHz, 256 KB Flash, 64KB SRAM
* Winbond W25Q16 16Mbit SPI Flash
* Atmel 24C02
* RTC battery CR1220
* NRF24L01 socket
* ESP8266-01/S1 socket
* DHT11 18B20 socket
* HS0038 socket
* Buzzer
* BLE socket
* TFT-LCD socket
* JTAG/SWD 20Pin socket


### Links:

* [STM32F103RC on st.com](https://www.st.com/content/st_com/en/products/microcontrollers-microprocessors/stm32-32-bit-arm-cortex-mcus/stm32-mainstream-mcus/stm32f1-series/stm32f103/stm32f103rc.html)
* [STM32F103RC datasheet](https://www.st.com/resource/en/datasheet/stm32f103rc.pdf)
