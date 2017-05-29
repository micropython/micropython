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
* RTC
* Some BLE support including _REPL over BLE_

Note that this port is still a work-in-progress and some modules are not fully feature complete.

## Tested hardware
* nRF51
* nRF52
  * [PCA10040](http://infocenter.nordicsemi.com/index.jsp?topic=%2Fcom.nordic.infocenter.nrf52%2Fdita%2Fnrf52%2Fdevelopment%2Fnrf52_dev_kit.html) 
  * [D52Q](https://www.dynastream.com/components/d52)
  * [Adafruit Feather nRF52](https://www.adafruit.com/product/3406)

## Build steps

Example is for the Adafruit Feather nRF52:

```
> sudo apt-get install build-essential libffi-dev pkg-config gcc-arm-none-eabi git python python-pip
> git clone https://github.com/adafruit/Adafruit_nRF52_Arduino.git
> cd Adafruit_nRF52_Arduino/tools/nrfutil-0.5.2/
> sudo pip install -r requirements.txt
> sudo python setup.py install
> cd ../../..
> git clone https://github.com/tralamazza/micropython.git nrf5_no_sdk
> cd nrf5_no_sdk/
> git submodule update --init
> make -C mpy-cross
> cd nrf5/
> make BOARD=feather52
> make BOARD=feather52 dfu-gen
> make BOARD=feather52 dfu-flash
```