# MicroPython Port To The Nordic Semiconductor nRF Series

This is a port of MicroPython to the Nordic Semiconductor nRF series of chips.

## Supported Features

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
  * ubluepy: Bluetooth LE module for MicroPython
  * 1 non-connectable advertiser while in connection

## Tested Hardware

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

Prerequisite steps for building the nrf port:

    git clone <URL>.git micropython
    cd micropython
    git submodule update --init
    make -C mpy-cross

By default, the PCA10040 (nrf52832) is used as compile target. To build and flash issue the following command inside the ports/nrf/ folder:

    make
    make flash

Alternatively the target board could be defined:

     make BOARD=pca10040
     make flash
     
## Compile and Flash with Bluetooth Stack

First prepare the bluetooth folder by downloading Bluetooth LE stacks and headers:

     ./drivers/bluetooth/download_ble_stack.sh

If the Bluetooth stacks has been downloaded, compile the target with the following command:

    make BOARD=pca10040 SD=s132

The **make sd** will trigger a flash of the bluetooth stack before that application is flashed. Note that **make sd** will perform a full erase of the chip, which could cause 3rd party bootloaders to also be wiped.

    make BOARD=pca10040 SD=s132 sd

Note: further tuning of features to include in bluetooth or even setting up the device to use REPL over Bluetooth can be configured in the `bluetooth_conf.h`.

## Target Boards and Make Flags

Target Board (BOARD) | Bluetooth Stack (SD)    | Bluetooth Support      | Flash Util
---------------------|-------------------------|------------------------|-------------------------------
microbit             | s110                    | Peripheral             | [PyOCD](#pyocdopenocd-targets)
pca10000             | s110                    | Peripheral             | [Segger](#segger-targets)
pca10001             | s110                    | Peripheral             | [Segger](#segger-targets)
pca10028             | s110                    | Peripheral             | [Segger](#segger-targets)
pca10031             | s110                    | Peripheral             | [Segger](#segger-targets)
pca10040             | s132                    | Peripheral and Central | [Segger](#segger-targets)
feather52            | s132                    | Peripheral and Central | [UART DFU](#dfu-targets)
arduino_primo        | s132                    | Peripheral and Central | [PyOCD](#pyocdopenocd-targets)
pca10056             |                         |                        | [Segger](#segger-targets)

## Segger Targets

Install the necessary tools to flash and debug using Segger:

[JLink Download](https://www.segger.com/downloads/jlink#)

[nrfjprog linux-32bit Download](https://www.nordicsemi.com/eng/nordic/download_resource/52615/16/95882111/97746)

[nrfjprog linux-64bit Download](https://www.nordicsemi.com/eng/nordic/download_resource/51386/21/77886419/94917)

[nrfjprog osx Download](https://www.nordicsemi.com/eng/nordic/download_resource/53402/12/97293750/99977)

[nrfjprog win32 Download](https://www.nordicsemi.com/eng/nordic/download_resource/33444/40/22191727/53210)

note: On Linux it might be required to link SEGGER's `libjlinkarm.so` inside nrfjprog's folder.

## PyOCD/OpenOCD Targets

Install the necessary tools to flash and debug using OpenOCD:

    sudo apt-get install openocd
    sudo pip install pyOCD

## DFU Targets

    sudo apt-get install build-essential libffi-dev pkg-config gcc-arm-none-eabi git python python-pip
    git clone https://github.com/adafruit/Adafruit_nRF52_Arduino.git
    cd Adafruit_nRF52_Arduino/tools/nrfutil-0.5.2/
    sudo pip install -r requirements.txt
    sudo python setup.py install
  
**make flash** and **make sd** will not work with DFU targets. Hence, **dfu-gen** and **dfu-flash** must be used instead.
* dfu-gen: Generates a Firmware zip to be used by the DFU flash application.
* dfu-flash: Triggers the DFU flash application to upload the firmware from the generated Firmware zip file. 

Example on how to generate and flash feather52 target:

    make BOARD=feather52 SD=s132
    make BOARD=feather52 SD=s132 dfu-gen
    make BOARD=feather52 SD=s132 dfu-flash
    
## Bluetooth LE REPL

The port also implements a BLE REPL driver. This feature is disabled by default, as it will deactivate the UART REPL when activated. As some of the nRF devices only have one UART, using the BLE REPL free's the UART instance such that it can be used as a general UART peripheral not bound to REPL.

The configuration can be enabled by editing the `bluetooth_conf.h` and set `MICROPY_PY_BLE_NUS` to 1.

When enabled you have different options to test it:
* [NUS Console for Linux](https://github.com/tralamazza/nus_console) (recommended)
* [WebBluetooth REPL](https://glennrub.github.io/webbluetooth/micropython/repl/) (experimental)

Other:
* nRF UART application for IPhone/Android

WebBluetooth mode can also be configured by editing `bluetooth_conf.h` and set `BLUETOOTH_WEBBLUETOOTH_REPL` to 1. This will alternate advertisement between Eddystone URL and regular connectable advertisement. The Eddystone URL will point the phone or PC to download [WebBluetooth REPL](https://glennrub.github.io/webbluetooth/micropython/repl/) (experimental), which subsequently can be used to connect to the Bluetooth REPL from the PC or Phone browser.
