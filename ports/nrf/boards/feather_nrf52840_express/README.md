# Setup

The `feather52840` board is currently based on the `PCA10056` development
board from Nordic Semiconductors, since commercial modules are not yet
available for the nRF52840.

The difference between the `pca10056` and `feather52840` board support
packages is that no bootloader is present on the `pca10056` (a HW debugger
like a Segger J-Link is required to flash firmware images), whereas the
`feather52840` package uses a serial bootloader, with a slightly different
flash layout to account for the bootloader's presence.

Both targets run on the same hardware and assume the same pinouts.

The `feather52840` board support package will be updated at a later date
to reflect any pin changes in the final Feather form-factor HW.

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
$ ./bluetooth/download_ble_stack.sh
```

## Installing the Serial Bootloader

The Adafruit nRF52840 Feather uses a serial bootloader that allows you to
update the core CircuitPython firmware and internal file system contents
using only a serial connection.

On empty devices, the serial bootloader will need to be flashed once using a
HW debugger such as a Segger J-Link before the serial updater (`adafruit-nrfutil`) can
be used.

### Install `nrfjprog`

Before you can install the bootloader, you will first need to install the
`nrfjprog` tool from Nordic Semiconductors for your operating system. The
binary files can be downloaded via the following links:

- [nRF5x toolset tar for Linux 32-bit v9.7.2](http://www.nordicsemi.com/eng/nordic/Products/nRF52832/nRF5x-Command-Line-Tools-Linux32/52619)
- [nRF5x toolset tar for Linux 64-bit v9.7.2](http://www.nordicsemi.com/eng/nordic/Products/nRF52832/nRF5x-Command-Line-Tools-Linux64/51388)
- [nRF5x toolset tar for OSX v9.7.2](http://www.nordicsemi.com/eng/nordic/Products/nRF52832/nRF5x-Command-Line-Tools-OSX/53406)
- [nRF5x toolset installer for Windows v9.7.2](http://www.nordicsemi.com/eng/nordic/Products/nRF52832/nRF5x-Command-Line-Tools-Win32/48768)

You will then need to add the `nrfjprog` folder to your system `PATH` variable
so that it is available from the command line. The exact process for this is
OS specific, but on a POSIX type system like OS X or Linux, you can
temporarily add the location to your `PATH` environment variables as follows:

```
$ export PATH=$PATH:YOURPATHHERE/nRF5x-Command-Line-Tools_9_7_2_OSX/nrfjprog/
```

You can test this by running the following command:

```
$ nrfjprog --version
nrfjprog version: 9.7.2
JLinkARM.dll version: 6.20f
```

### Flash the USB CDC Bootloader with 'nrfjprog'

> This operation only needs to be done once, and only on boards that don't
  already have the serial bootloader installed.

Firstly clone the [Adafruit_nRF52_Bootloader](https://github.com/adafruit/Adafruit_nRF52_Bootloader.git) and enter its directory

```
$ git clone https://github.com/adafruit/Adafruit_nRF52_Bootloader.git
$ cd Adafruit_nRF52_Bootloader
```

Once `nrfjprog` is installed and available in `PATH` you can flash your
board with the serial bootloader via the following command:

```
make BOARD=feather_nrf52840_express VERSION=latest flash
```

This should give you the following (or very similar) output, and you will see
a DFU blinky pattern on one of the board LEDs:

```
$ make BOARD=pca10056 VERSION=latest flash
Flashing: bin/pca10056/6.0.0r0/pca10056_bootloader_s140_6.0.0r0.hex
nrfjprog  --program bin/pca10056/6.0.0r0/pca10056_bootloader_s140_6.0.0r0.hex --chiperase -f nrf52 --reset
Parsing hex file.
Erasing user available code and UICR flash areas.
Applying system reset.
Checking that the area to write is not protected.
Programing device.
Applying system reset.
Run.
```

From this point onward, you can now use a simple serial port for firmware
updates.

Note: You can specify other version that are available in the directory `Adafruit_nRF52_Bootloader/bin/feather_nrf52840_express/` . The `VERSION=latest` will use the latest bootloader available.

### IMPORTANT: Disable Mass Storage on PCA10056 J-Link

The J-Link firmware on the PCA10056 implement USB Mass Storage, but this
causes a known conflict with reliable USB CDC serial port communication. In
order to use the serial bootloader, **you must disable MSD support on the
Segger J-Link**!

To disable mass storage support, run the `JLinkExe` (or equivalent) command,
and send `MSDDisable`. (You can re-enable MSD support via `MSDEnable`):

```
$ JLinkExe
SEGGER J-Link Commander V6.20f (Compiled Oct 13 2017 17:20:01)
DLL version V6.20f, compiled Oct 13 2017 17:19:52

Connecting to J-Link via USB...O.K.
Firmware: J-Link OB-SAM3U128-V2-NordicSemi compiled Jul 24 2017 17:30:12
Hardware version: V1.00
S/N: 683947110
VTref = 3.300V


Type "connect" to establish a target connection, '?' for help
J-Link>MSDDisable
Probe configured successfully.
J-Link>exit
```

## Building and Flashing CircuitPython

### Installing `adafruit-nrfutil`

run follow command to install [adafruit-nrfutil](https://github.com/adafruit/Adafruit_nRF52_nrfutil) from PyPi

```
$ pip3 install adafruit-nrfutil --user
```

### Flashing CircuitPython with USB CDC

With the serial bootloader present on your board, you first need to force your
board into DFU mode by holding down BUTTON1 and RESETTING the board (with
BUTTON1 still pressed as you come out of reset).

This will give you a **fast blinky DFU pattern** to indicate you are in DFU
mode.

You can **build and flash** a CircuitPython binary via the following command:

```
$ make V=1 SD=s140 SERIAL=/dev/tty.usbmodem1411 BOARD=feather52840 all dfu-gen dfu-flash
```

This should give you the following results:

```
$make V=1 BOARD=feather52840 SD=s140 SERIAL=/dev/tty.usbmodem1411 dfu-gen dfu-flash
nrfutil dfu genpkg --sd-req 0xFFFE --dev-type 0x0052 --application build-feather52840-s140/firmware.hex build-feather52840-s140/dfu-package.zip
Zip created at build-feather52840-s140/dfu-package.zip
nrfutil --verbose dfu serial --package build-feather52840-s140/dfu-package.zip -p /dev/ttyACM1 -b 115200 --singlebank
Upgrading target on /dev/ttyACM1 with DFU package /home/hathach/Dropbox/adafruit/circuitpython/ada_cp/ports/nrf/build-feather52840-s140/dfu-package.zip. Flow control is disabled, Single bank mode
Starting DFU upgrade of type 4, SoftDevice size: 0, bootloader size: 0, application size: 199840
Sending DFU start packet
Sending DFU init packet
Sending firmware file
#########################################################################################################################################################################################################################################################################################################################################################################################################
Activating new firmware

DFU upgrade took 8.50606513023s
Device programmed.
```

### Flashing CircuitPython with MSC UF2

uf2 file is generated last by `all` target

```
$ make V=1 SD=s140 SERIAL=/dev/tty.usbmodem1411 BOARD=feather52840 all
Create firmware.uf2
../../tools/uf2/utils/uf2conv.py -f 0xADA52840 -c -o "build-feather52840-s140/firmware.uf2" "build-feather52840-s140/firmware.hex"
Converting to uf2, output size: 392192, start address: 0x26000
Wrote 392192 bytes to build-feather52840-s140/firmware.uf2.
```

Simply drag and drop firmware.uf2 to the MSC, the nrf52840 will blink fast and reset after done.
