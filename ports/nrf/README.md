# CircuitPython Port To The Nordic Semiconductor nRF52 Series

This is a port of CircuitPython to the Nordic Semiconductor nRF52 series of chips.

> **NOTE**: There are board-specific READMEs that may be more up to date than the
  generic board-neutral documentation below.

## Flash

Some boards have UF2 bootloaders and can simply be flashed in the normal way, by copying
firmware.uf2 to the BOOT drive.

For some boards, you can use the `flash` target:

	make BOARD=pca10056 flash

## Segger Targets

Install the necessary tools to flash and debug using Segger:

[JLink Download](https://www.segger.com/downloads/jlink#)

[nrfjprog linux-32bit Download](https://www.nordicsemi.com/eng/nordic/download_resource/52615/16/95882111/97746)

[nrfjprog linux-64bit Download](https://www.nordicsemi.com/eng/nordic/download_resource/51386/21/77886419/94917)

[nrfjprog osx Download](https://www.nordicsemi.com/eng/nordic/download_resource/53402/12/97293750/99977)

[nrfjprog win32 Download](https://www.nordicsemi.com/eng/nordic/download_resource/33444/40/22191727/53210)

note: On Linux it might be required to link SEGGER's `libjlinkarm.so` inside nrfjprog's folder.

## DFU Targets

run follow command to install [adafruit-nrfutil](https://github.com/adafruit/Adafruit_nRF52_nrfutil) from PyPi

    $ pip3 install --user adafruit-nrfutil

**make flash** and **make sd** will not work with DFU targets. Hence, **dfu-gen** and **dfu-flash** must be used instead.
* dfu-gen: Generates a Firmware zip to be used by the DFU flash application.
* dfu-flash: Triggers the DFU flash application to upload the firmware from the generated Firmware zip file.


When enabled you have different options to test it:
* [NUS Console for Linux](https://github.com/tralamazza/nus_console) (recommended)
* [WebBluetooth REPL](https://glennrub.github.io/webbluetooth/micropython/repl/) (experimental)
