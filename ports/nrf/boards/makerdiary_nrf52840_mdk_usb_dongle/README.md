# MakerDiary NRF52840 MDK USB Dongle

Refer to [The makerdiary Github repo](https://github.com/makerdiary/nrf52840-mdk-usb-dongle)
or [The nrf52840-mdk-usb-dongle wiki](https://wiki.makerdiary.com/nrf52840-mdk-usb-dongle/)
for more details about the device.

This is pretty much just the nRF52840 with a useful number of pins exposed for
your pleasure along with one RGB LED and an onboard antenna in a USB stick form
factor with room for headers on the sides.

Note that all three LEDs on this device are wired through sinks, not sources,
so flip your boolean expectations when dealing with `DigitalInOut` or `PWMOut`
on this device --
`led.value = True` or `led.duty_cycle = 0xffff` turns the LED off!

The onboard button is hard wired to the Reset pin so you cannot use it yourself.

## Installing CircuitPython submodules

Before you can build, you will need to run the following commands once, which
will install the submodules that are part of the CircuitPython ecosystem, and
build the `mpy-cross` tool:

```
$ cd circuitpython
$ git submodule update --init
$ make -C mpy-cross
```

## Note about bootloaders

While most Adafruit devices come with (or can easily be flashed with) an
Adafruit-provided bootloader (supporting niceties like UF2 flashing), this
board comes with one that supports DFU via nrfutil.  If you ever need to
restore the DFU bootloader via a SWD debugger, use
[the nRF52 open bootloader hex file](https://github.com/makerdiary/nrf52840-mdk-usb-dongle/tree/master/firmware/open_bootloader).

## Building and Flashing CircuitPython

```
$ cd ports/nrf
```

### Build CircuitPython for the MDK USB Dongle

```
make BOARD=makerdiary_nrf52840_mdk_usb_dongle SD=s140 V=1 -j4 hex
```

This should produce a `build-makerdiary_nrf52840_mdk_usb_dongle-s140/firmware.hex` file.

### Install nrfutil

You'll need to have [nrfutil](https://pypi.org/project/nrfutil/) installed as
appropriate for your system.
As of 2019-01, _nrfutil still requires Python 2.7_... ugh!

### Flash the nRF52 Radio Soft Device

Build a DFU package from the softdevice hex file and flash it:

```sh
nrfutil pkg generate --hw-version 52 --sd-req 0x00 --sd-id 0xAE --softdevice bluetooth/s140_nrf52_6.1.0/s140_nrf52_6.1.0_softdevice.hex dfu_sd140-6.1.0.zip
nrfutil dfu usb-serial -pkg dfu_sd140-6.1.0.zip -p /dev/tty.usbmodemABRACADBRA  # likely /dev/ttyACM0 on Linux
```

Note that the `--sd=id 0xAE` comes from the Nordic nRF52 manual for SoftDevice
6.1.0.  When the SoftDevice is changed, read the Nordic manual to find the
correct value and use it on all of the `nrfutil pkg generate` commands.

`/dev/tty.usbmodem*` is a macOS name.  On Linux it'll likely be `/dev/ttyACM*`.  On Windows probably a COM port.

### Flash CircuitPython

Build a DFU package from the hex application file and flash it:

```
nrfutil pkg generate --sd-req 0xAE --application build-makerdiary_nrf52840_mdk_usb_dongle-s140/firmware.hex --hw-version 52 --application-version 1 dfu_circuitpython.zip
nrfutil dfu usb-serial -pkg dfu_circuitpython.zip -p /dev/tty.usbmodemABRACADBRA
```

I'm not sure if `--application-version 1` is actually meaningful or required.

After this, your device should be up and running CircuitPython.  When it
resets, you'll see the CIRCUITPY USB filesystem and a new console usb modem
serial port show up.

```
Adafruit CircuitPython 4.0.0-alpha.5-139-g10ceb6716 on 2019-01-14; MakerDiary nRF52840 MDK USB Dongle with nRF52840
>>>
```

### TODO items

* Update the Makefile to do the above DFU .zip building and nrfutil flashing.
* Create a UF2 bootloader for this. It is already a USB stick form factor, it deserves to behave like one.
