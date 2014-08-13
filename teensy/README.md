# Build Instructions for Teensy 3.1

Currently the Teensy 3.1 port of Micro Python builds under Linux and not under Windows.

The tool chain required for the build can be found at <https://launchpad.net/gcc-arm-embedded>.

Downlaod the current Linux *.tar.bz2 file. Instructions regarding unpacking the file and moving it to the correct location 
as well as adding the extracted folders to the enviroment variable can be found at 
<http://eliaselectronics.com/stm32f4-tutorials/setting-up-the-stm32f4-arm-development-toolchain/>

## Previous Instructions Shown Below (no longer valid)

This assumes that you have TeensyDuino installed and set the ARDUINO environment
variable pointing to the where Arduino with TeensyDuino is installed.

```
cd teensy
ARDUINO=~/arduino-1.0.5 make
```

To upload micropython to the Teensy 3.1.

Press the Program button on the Teensy 3.1
```
make upload
```

Currently, the python prompt is through the USB serial interface.
