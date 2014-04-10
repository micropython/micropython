Build Instructions for Teensy 3.1

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
