# MicroPython port to the NRF5

This is a port of MicroPython to the Nordic nRF5 series of chips. 

## Supported features

* UART
* SPI
* LEDs
* Pins
* ADC
* I2C
* PWM (nRF52 only)
* Temperature
* RTC (Real Time Counter. Low-Power counter)
* BLE support including:
  * Peripheral role on nrf51 targets
  * Central role and Peripheral role on nrf52 targets
   * _REPL over Bluetooth LE_ (optionally using WebBluetooth)
   * ubluepy: Bluetooth LE module for micropython

## Tested hardware

* nRF51
  * [micro:bit](http://microbit.org/)
  * PCA10000 (dongle)
  * PCA10001
  * PCA10028
  * PCA10031 (dongle)
* nRF52832
  * [PCA10040](http://infocenter.nordicsemi.com/index.jsp?topic=%2Fcom.nordic.infocenter.nrf52%2Fdita%2Fnrf52%2Fdevelopment%2Fnrf52_dev_kit.html) 
  * [Adafruit Feather nRF52](https://www.adafruit.com/product/3406)
  * [Thingy:52](http://www.nordicsemi.com/eng/Products/Nordic-Thingy-52)
  * [Arduino Primo](http://www.arduino.org/products/boards/arduino-primo)
* nRF52840
  * [PCA10056](http://www.nordicsemi.com/eng/Products/nRF52840-Preview-DK)

## Compile and Flash

Before any compilation of any targets some basic operatons needs to be done:

    git clone <URL>.git micropython
    cd micropython
    git submodule update --init
    make -C mpy-cross

By default PCA10040 (nrf52832) is used as compile target. To build issue the following command inside the nrf5/ folder:

    make
    make flash

Alternativly the target board could be defined:

     make BOARD=pca10040
     make flash
     
Available board target names:
* microbit
* feather52 
* pca10000
* pca10001
* pca10028
* pca10031
* pca10040
* pca10056

## Compile and Flash with Bluetooth Stack

First prepare the bluetooth folder by downloading Bluetooth LE stacks and headers:

     ./drivers/bluetooth/download_ble_stack.sh

If the Bluetooth stacks has been downloaded, compile the target with the following command:

    make BOARD=pca10040 SD=s132
    make sd

The **make sd** will trigger a flash of the bluetooth stack before that application is flashed. Note that **make sd** will perform a full erase of the chip, which could cause 3rd party bootloaders to also be wiped.

Note: further tuning of features to include in bluetooth or even setting up the device to use REPL over Bluetooth can be configured in the bluetooth_conf.h.

Board       | SD param    | Support
------------|-------------|----------
microbit    | s110        | Peripheral
pca10000    | s110        | Peripheral
pca10001    | s110        | Peripheral
pca10028    | s110        | Peripheral 
pca10031    | s110        | Peripheral
pca10040    | s132        | Peripheral and Central
feather52   | s132        | Peripheral and Central
pca10056    |             |

## Segger targets

Install the necessary tools to flash and debug using Segger:

[JLink](https://www.segger.com/downloads/jlink#)

[nrfjprog linux-32bit](https://www.nordicsemi.com/eng/nordic/download_resource/52615/16/95882111/97746)
[nrfjprog linux-64bit](https://www.nordicsemi.com/eng/nordic/download_resource/51386/21/77886419/94917)
[nrfjprog osx](https://www.nordicsemi.com/eng/nordic/download_resource/53402/12/97293750/99977)
[nrfjprog win32](https://www.nordicsemi.com/eng/nordic/download_resource/33444/40/22191727/53210)

Boards that would need JLink/nrfjprog:
* PCA10000
* PCA10001
* PCA10028
* PCA10031
* PCA10040
* PCA10056

## PyOCD/OpenOCD targets

Install the necessary tools to flash and debug using OpenOCD:

    sudo apt-get install openocd
    sudo pip install pyOCD

Boards that would need PyOCD:
* micro:bit

## DFU targets

    sudo apt-get install build-essential libffi-dev pkg-config gcc-arm-none-eabi git python python-pip
    git clone https://github.com/adafruit/Adafruit_nRF52_Arduino.git
    cd Adafruit_nRF52_Arduino/tools/nrfutil-0.5.2/
    sudo pip install -r requirements.txt
    sudo python setup.py install
  
**make flash** and **make sd** will not work with DFU targets. Hence, **dfu-gen** and **dfu-flash** must be used instead.
* dfu-gen: Generates a Firmware zip to be used by the DFU flash application.
* dfu-flash: Triggers the DFU flash application to upload the firmware from the generated Firmware zip file. 

Example on how to generate and flash feather52 target:

    make BOARD=feather52
    make BOARD=feather52 dfu-gen
    make BOARD=feather52 dfu-flash

Boards that would need DFU flash utilities:
* feather52 (Adafruit Feather nRF52)
