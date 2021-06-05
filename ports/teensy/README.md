# Build Instructions for Teensy 3.1

Currently the Teensy 3.1 port of MicroPython builds under Linux and not under Windows.

The tool chain required for the build can be found at <https://launchpad.net/gcc-arm-embedded>.

Download the current Linux *.tar.bz2 file. Instructions regarding unpacking the file and moving it to the correct location
as well as adding the extracted folders to the environment variable can be found at
<http://eliaselectronics.com/stm32f4-tutorials/setting-up-the-stm32f4-arm-development-toolchain/>

In order to download the firmware image to the teensy, you'll need to use the
downloader included with TeensyDuino. The following assumes that you have
TeensyDuino installed and set the ARDUINO environment variable pointing to the
where Arduino with TeensyDuino is installed.

```bash
cd teensy
ARDUINO=~/arduino-1.0.5 make
```

To upload MicroPython to the Teensy 3.1.

Press the Program button on the Teensy 3.1
```bash
sudo ARDUINO=~/arduino-1.0.5/ make deploy
```

Currently, the Python prompt is through the USB serial interface, i.e.

```bash
minicom -D /dev/ttyACM0
```

## TIPS

### Install 49-teensy.rules into /etc/udev/rules.d
If you install the 49-teensy.rules file from http://www.pjrc.com/teensy/49-teensy.rules
into your ```/etc/udev/rules.d``` folder then you won't need to use sudo:
```bash
sudo cp ~/Downloads/49-teensy.rules /etc/udev/rules.d
sudo udevadm control --reload-rules
```
Unplug and replug the teensy board, and then you can use: ```ARDUINO=~/arduino-1.0.5/ make deploy```

### Create a GNUmakefile to hold your ARDUINO setting.
Create a file call GNUmakefile (note the lowercase m) in the teensy folder
with the following contents:
```make
$(info Executing GNUmakefile)

ARDUINO=${HOME}/arduino-1.0.5
$(info ARDUINO=${ARDUINO})

include Makefile
```
GNUmakefile is not checked into the source code control system, so it will
retain your settings when updating your source tree. You can also add
additional Makefile customizations this way.

### Tips for OSX

Set the ARDUINO environment variable to the location where Arduino with TeensyDuino is installed.
```bash
export ARDUINO=~/Downloads/Arduino.app/Contents/Java/
```

Search /dev/ for USB port name, which will be cu.usbmodem followed by a few numbers. The name of the port maybe different depending on the version of OSX.
To access the Python prompt type: 

```bash
screen <devicename> 115200
```
