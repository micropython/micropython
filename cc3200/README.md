MicroPython port to CC3200 WiFi SoC
===================================

This is a MicroPython port to Texas Instruments CC3200 WiFi SoC (ARM Cortex-M4
architecture). This port supports 2 boards: WiPy and TI CC3200-LAUNCHXL.

## Build Instructions for the CC3200

Currently the CC3200 port of MicroPython builds under Linux and OSX,
but not under Windows.

The toolchain required for the build can be found at
<https://launchpad.net/gcc-arm-embedded>.

In order to flash the image to the CC3200 you will need the
[cc3200tool](https://github.com/ALLTERCO/cc3200tool). An alternative is
to use CCS_Uniflash tool from TI, which works only under Windows, and all
support is provided by TI itself.

Building the bootloader:

```
make BTARGET=bootloader BTYPE=release BOARD=LAUNCHXL
```

Building the "release" image:

```
make BTARGET=application BTYPE=release BOARD=LAUNCHXL
```

To build an image suitable for debugging:

In order to debug the port specific code, optimizations need to be disabled on the
port file (check the Makefile for specific details). You can use CCS from TI.
Use the CC3200.ccxml file supplied with this distribution for the debuuger configuration.

```
make BTARGET=application BTYPE=debug BOARD=LAUNCHXL
```

## Flashing the CC3200-LAUNCHXL

Note that WiPy comes factory programmed with a default version of MicroPython,
it cannot be programmed via serial, and can be upgraded only with OTA (see
below).

- Make sure that you have built both the *bootloader* and the *application* in **release** mode.
- Make sure the SOP2 jumper is in position.
- Make sure you Linux system recognized the board and created `ttyUSB*`
  devices (see below for configuration of `ftdi_sio` driver).
- Run "make erase" and immediately press Reset button on the device.
- Wait few seconds.
- Run "make deploy" and immediately press Reset button on the device.
- You are recommended to install the latest vendor WiFi firmware
  servicepack from http://www.ti.com/tool/cc3200sdk. Download
  CC3200SDK-SERVICEPACK package, install it, and locate `ota_*.ucf`
  and `ota_*.ucf.signed.bin` files. Copy them to the port's directory
  and run "make servicepack", with immediate press of Reset button.
- Remove the SOP2 jumper and reset the board.

Flashing process using TI Uniflash:

- Open CCS_Uniflash and connect to the board (by default on port 22). 
- Format the serial flash (select 1MB size in case of the CC3200-LAUNCHXL, 2MB in case of the WiPy, leave the rest unchecked).
- Mark the following files for erasing: `/cert/ca.pem`, `/cert/client.pem`, `/cert/private.key` and `/tmp/pac.bin`.
- Add a new file with the name of /sys/mcuimg.bin, and select the URL to point to cc3200\bootmgr\build\<BOARD_NAME>\bootloader.bin.
- Add another file with the name of /sys/factimg.bin, and select the URL to point to cc3200\build\<BOARD_NAME>\mcuimg.bin.
- Click "Program" to apply all changes.
- Flash the latest service pack (servicepack_1.0.0.10.0.bin) using the "Service Pack Update" button.
- Close CCS_Uniflash, remove the SOP2 jumper and reset the board.

## Playing with MicroPython and the CC3200:

Once the software is running, you have two options to access the MicroPython REPL:

- Through telnet.
  * Connect to the network created by the board (as boots up in AP mode), **ssid = "wipy-wlan", key = "www.wipy.io"**.
    * You can also reinitialize the WLAN in station mode and connect to another AP, or in AP mode but with a
      different ssid and/or key.
  * Use your favourite telnet client with the following settings: **host = 192.168.1.1, port = 23.**
  * Log in with **user = "micro" and password = "python"**

- Through UART (serial).
  * This is enabled by default in the standard configuration, for UART0 (speed 115200).
  * For CC3200-LAUNCHXL, you will need to configure Linux `ftdi_sio` driver as described
    in the [blog post](http://www.achanceofbrainshowers.com/blog/tech/2014/8/19/cc3200-development-under-linux/).
    After that, connecting a board will create two `/dev/ttyUSB*` devices, a serial
    console is available on the 2nd one (usually `/dev/ttyUSB1`).
  * WiPy doesn't have onboard USB-UART converter, so you will need an external one,
    connected to GPIO01 (Tx) and GPIO02 (Rx).
  * Usage of UART port for REPL is controlled by MICROPY_STDIO_UART setting (and
    is done at the high level, using a suitable call to `os.dupterm()` function
    in boot.py, so you can override it at runtime regardless of MICROPY_STDIO_UART
    setting).

The board has a small file system of 192K (WiPy) or 64K (Launchpad) located in the serial flash connected to the CC3200. 
SD cards are also supported, you can connect any SD card and configure the pinout using the SD class API.

## Uploading scripts:

To upload your MicroPython scripts to the FTP server, open your FTP client of choice and connect to:
**ftp://192.168.1.1, user = "micro", password = "python"**

Tested FTP clients are: FileZilla, FireFTP, FireFox, IE and Chrome. Other
clients should work as well, but you may need to configure them to use a
single connection (this should be the default for any compliant FTP client).

## Upgrading the firmware Over The Air (OTA)

OTA software updates can be performed through the builtin FTP server. After
building a new `mcuimg.bin` in release mode, upload it to:
`/flash/sys/mcuimg.bin`. It will take around 6s (The TI SimpleLink file
system is quite slow because every file is mirrored for safety). You won't
see the file being stored inside `/flash/sys/` because it's actually saved
bypassing FatFS, but rest assured that the file was successfully transferred,
and it has been signed with a MD5 checksum to verify its integrity.
Now, reset the MCU by pressing the switch on the board, or by typing:

```python
import machine
machine.reset()
```

There's a script which automates this process from the host side:

- Make sure the board is running and connected to the same network as the computer.

```bash
make BTARGET=application BTYPE=release BOARD=LAUNCHXL WIPY_IP=192.168.1.1 WIPY_USER=micro WIPY_PWD=python deploy-ota
```

If `WIPY_IP`, `WIPY_USER` or `WIPY_PWD` are omitted the default values (the ones shown above) will be used.


## Notes and known issues

## Regarding old revisions of the CC3200-LAUNCHXL

First silicon (pre-release) revisions of the CC3200 had issues with the ram blocks, and MicroPython cannot run
there. Make sure to use a **v4.1 (or higher) LAUNCHXL board** when trying this port, otherwise it won't work.

### Note regarding FileZilla

Do not use the quick connect button, instead, open the site manager and create a new configuration. In the "General" tab make 
sure that encryption is set to: "Only use plain FTP (insecure)". In the Transfer Settings tab limit the max number of connections 
to one, otherwise FileZilla will try to open a second command connection when retrieving and saving files, and for simplicity and 
to reduce code size, only one command and one data connections are possible.
