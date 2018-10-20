# MakerDiary NRF52840 MDK

Refer to https://github.com/makerdiary/nrf52840-mdk or
https://wiki.makerdiary.com/nrf52840-mdk/ for more details about the device.

Notably, CircuitPython does not currently support QSPI external flash on NRF
devices, so neither does this port - the 64Mb flash device is not used for
anything. Also, don't confuse this with the 64MiB drive that shows up on your
computer - it's actually part of the MSC driver provided by the DAPLink
debugger, and is inaccessible at all from Python land (this drive is where you
can copy `firmware.hex` if you'd prefer to flash that way as opposed to with
`pyocd`. You'll still have access to 256KB of the onboard flash, however, for
storing your Python files, cat pictures, or whatever.

It's also interesting to note that all three LEDs and the "user button" on this
device are wired through sinks, not sources, so flip your boolean expectations
when dealing with `digitalio.DigitalInOut` on this device - `my_led.value =
True` turns the LED off!  Likewise, the user button will read `False` when
pressed.

## Installing CircuitPython submodules

Before you can build, you will need to run the following commands once, which
will install the submodules that are part of the CircuitPython ecosystem, and
build the `mpy-cross` tool:

```
$ cd circuitpython
$ git submodule update --init
$ make -C mpy-cross
```

You then need to download the SD and Nordic SDK files via:

> This script relies on `wget`, which must be available from the command line.

```
$ cd ports/nrf
$ ./drivers/bluetooth/download_ble_stack.sh
```

## Note about bootloaders

While most Adafruit devices come with (or can easily be flashed with) an
Adafruit-provided bootloader (supporting niceties like UF2 flashing), this
board comes with DAPLink which (apparently?) handles everything from debugging
to programming the device, as well as the boot sequence. What's particularly
awesome about this board is that there is no physical interaction with the board
required to flash new code (read: CircuitPython builds) - the device is _always_
listening for new firmware uploads (via `pyocd-flashtool`), even if userspace
code is running.

## Building and Flashing CircuitPython

You'll need to have [pyocd](https://github.com/mbedmicro/pyOCD) installed as
appropriate for your system.

```sh
make BOARD=makerdiary_nrf52840_mdk FLASHER=pyocd SD=s140 flash
```

This should give you the following (or very similar) output, and you will see
a DFU blinky pattern on one of the board LEDs:

```
$ make BOARD=makerdiary_nrf52840_mdk FLASHER=pyocd SD=s140 flash
Use make V=1, make V=2 or set BUILD_VERBOSE similarly in your environment to increase build verbosity.
pyocd-flashtool -t nrf52 build-makerdiary_nrf52840_mdk-s140/firmware.hex --sector_erase
INFO:root:DAP SWD MODE initialised
INFO:root:ROM table #0 @ 0xe00ff000 cidr=b105100d pidr=2002c4008
INFO:root:[0]<e000e000:SCS-M4 cidr=b105e00d, pidr=4000bb00c, class=14>
WARNING:root:Invalid coresight component, cidr=0x0
INFO:root:[1]<e0001000: cidr=0, pidr=0, component invalid>
INFO:root:[2]<e0002000:FPB cidr=b105e00d, pidr=4002bb003, class=14>
WARNING:root:Invalid coresight component, cidr=0x1010101
INFO:root:[3]<e0000000: cidr=1010101, pidr=101010101010101, component invalid>
WARNING:root:Invalid coresight component, cidr=0x0
INFO:root:[4]<e0040000: cidr=0, pidr=0, component invalid>
INFO:root:[5]<e0041000:ETM-M4 cidr=b105900d, pidr=4000bb925, class=9, devtype=13, devid=0>
INFO:root:CPU core is Cortex-M4
INFO:root:FPU present
INFO:root:6 hardware breakpoints, 4 literal comparators
INFO:root:4 hardware watchpoints
[====================] 100%
INFO:root:Programmed 237568 bytes (58 pages) at 14.28 kB/s
#pyocd-tool -t nrf52 erase 0xFF000
pyocd-tool -t nrf52 write32 0xFF000 0x00000001
WARNING:root:Invalid coresight component, cidr=0x0
WARNING:root:Invalid coresight component, cidr=0x1010101
WARNING:root:Invalid coresight component, cidr=0x0
pyocd-tool -t nrf52 reset
WARNING:root:Invalid coresight component, cidr=0x0
WARNING:root:Invalid coresight component, cidr=0x1010101
WARNING:root:Invalid coresight component, cidr=0x0
Resetting target
```

Alternatively (and untested by me), it's apparently possible to copy
`firmware.hex` to the MSC device provided by DAPLink and flash that way. Refer
to [the upstream
documentation](https://wiki.makerdiary.com/nrf52840-mdk/getting-started/#drag-n-drop-programming)
for details.
