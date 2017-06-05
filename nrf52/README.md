# nRF52 Port

WIP

## requirements

- GNU make
- ARM GCC for embedded development
- JLink Segger command line tools
- Download and unzip the Nordic SDK 11 under `nordic/`


## quickstart

Plug your board and type:

    $> make flash-all


## debugging

    $> make gdbserver

in another terminal

    $> make gdb

in yet another terminal (for the RTT messages)

    $> telnet 127.0.0.1 19021


## TODO

- BLE peripheral
	- Advertisement
	- Manage services/characteristics
	- Notifications/Indications
- HAL
	- UART
	- TWI
	- SPI
	- ADC
	- Flash
	- GPIO
	- I2S