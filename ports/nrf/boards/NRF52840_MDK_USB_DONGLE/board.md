nRF52840 MDK USB Dongle
=======================

The *[nRF52840 MDK USB
Dongle](https://wiki.makerdiary.com/nrf52840-mdk-usb-dongle)* is a small,
low-cost development board in a USB dongle form-factor powered by an nRF52840
with 1MB flash and 256KB RAM.

This device is pre-installed with [Open
Bootloader](https://wiki.makerdiary.com/nrf52840-mdk-usb-dongle/programming/),
allowing DFU upgrades over USB using Nordic [nRF
Connect](https://www.nordicsemi.com/Software-and-tools/Development-Tools/nRF-Connect-for-desktop)
or [nrfutil](https://github.com/NordicSemiconductor/pc-nrfutil/). To support
Open Bootloader, the flash and memory layout must be adjusted slightly (details
[here](https://devzone.nordicsemi.com/nordic/short-range-guides/b/getting-started/posts/nrf52840-dongle-programming-tutorial))
from the typical nRF build; this board definition ensure the appropriate build
configuration is used for MicroPython.


Pinout
------

The [pinout
diagram](https://wiki.makerdiary.com/nrf52840-mdk-usb-dongle/#pinout-diagram)
provides an overview of the available pins and their capabilities. All pins are
available in MicroPython, using the pin numbers labelled in the diagram
(excluding the leading port number, *P0*).

The three LEDs are available either through the usual `Pin` mechanism - pins
22-24 - or by `board.LED(n)` where n can be 1, 2 or 3.


Build instructions
------------------

Follow the standard [nRF Port build instructions](../../README.md); but use
`nrf52840-mdk-usb-dongle` as the value for `BOARD`:

    make BOARD=nrf52840-mdk-usb-dongle

The build artifacts will be created in `build-nrf52840-mdk-usb-dongle`. Once
built, the target can be deployed to the device as described in
[nRFUtil targets](../../README.md#nrfutil-targets).

An alternative way to deploy to the device, is to open `firmware.hex` using
*nRF Connect* and select *Write*. Detailed instructions can be found on the
[developer
wiki](https://wiki.makerdiary.com/nrf52840-mdk-usb-dongle/programming/).
