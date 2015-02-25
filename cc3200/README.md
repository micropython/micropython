# Build Instructions for the CC3200

Currently the CC3200 port of Micro Python builds under Linux and OSX and not under Windows.

The tool chain required for the build can be found at <https://launchpad.net/gcc-arm-embedded>.

In order to download the image to the CC3200 you will need the CCS_Uniflash tool from TI, which at this 
moment is only available for Windows, so, you need Linux/OSX to build and Windows to flash the image.

## To build an image suitable for debugging:

In order to debug the port specific code, optimizations need to be disabled on the 
port file (check the Makefile for specific details). You can use CCS from TI.
Use the CC3200.ccxml file supplied with this distribution for the debuuger configuration. 
```bash
make BTARGET=application BTYPE=debug
```
## To build an image suitable to be flashed to the device:
```bash
make BTARGET=application BTYPE=release
```
## Building the bootloader
```bash
make BTARGET=bootloader BTYPE=release
```

## Flashing the CC3200
- Make sure that you have built both the *bootloader* and the *application* in **release** mode.
- Make sure the SOP2 jumper is in position.
- Open CCS_Uniflash and connect to the board (by default on port 22). 
- Format the serial flash (select 1MB size in case of the CC3200-LAUNCHXL, leave the rest unchecked).
- Mark the following files for erasing: `/cert/ca.pem`, `/cert/client.pm`, `/cert/private.key` and `/tmp/pac.bin`.
- Add a new file with the name of /sys/mcuimg.bin, and select the URL to point to cc3200\bootmgr\build\<BOARD_NAME>\bootloader.bin.
- Add another file with the name of /sys/factimg.bin, and select the URL to point to cc3200\build\<BOARD_NAME>\MCUIMG.BIN.
- Click "Program" to apply all changes.
- Flash the latest service pack (servicepack_1.0.0.1.2.bin) using the "Service Pack Update" button.
- Close CCS_Uniflash, remove the SOP2 jumper and reset the board.

## Playing with MicroPython and the CC3200:

Once the software is running, you have two options to access the MicroPython REPL:

- Through the UART. 
  **Connect to PORT 22, baud rate = 115200, parity = none, stop bits = 1**
- Through telnet. 
  * Connect to the network created by the board (as boots up in AP mode), **ssid = "micropy-wlan", key = "micropython"**
    * You can also reinitialize the WLAN in station mode and connect to another AP, or in AP mode but with a
      different ssid and/or key.
  * Use your favourite telnet client with the following settings: **host = 192.168.1.1, port = 23.**
  * Log in with **user = "micro" and password = "python"**

The board has a small file system of 64K located in the serial flash connected to the CC3200. SD cards are also supported, but
since the CC3200 LaunchXL doesn't come with an SD card socket installed, you will need to add one yourself. Any SD card breakout
board will do, as long as you connect it as described here: <http://processors.wiki.ti.com/index.php/CC32xx_SDHost_FatFS>

## Uploading scripts:

To upload your MicroPython scripts to the FTP server, open your FTP client of choice and connect to:
**ftp://192.168.1.1, user = "micro", password = "python"**

I have tested the FTP server with **FileZilla, FireFTP, FireFox, IE and Chrome,** other clients should work as well, but I am 
not 100% sure of it.

## Upgrading the firmware Over The Air:

OTA software updates can be performed through the FTP server. After building a new MCUIMG.BIN in release mode, upload it to:
`/SFLASH/SYS/MCUIMG.BIN` it will take around 8s (The TI simplelink file system is quite slow because every file is mirrored for
safety). You won't see the file being stored inside `/SFLASH/SYS/` because it's actually saved bypassing FatFS, but rest assured that
the file was successfully transferred, and it has been signed with a MD5 checksum to verify its integrity. 
Now, reset the MCU by pressing the switch on the board, or by typing:

```python
import pyb
pyb.hard_reset()
```

### Note regarding FileZilla:

Do not use the quick connect button, instead, open the site manager and create a new configuration. In the "General" tab make 
sure that encryption is set to: "Only use plain FTP (insecure)". In the Transfer Settings tab limit the max number of connections 
to one, otherwise FileZilla will try to open a second command connection when retrieving and saving files, and for simplicity and 
to reduce code size, only one command and one data connections are possible.

