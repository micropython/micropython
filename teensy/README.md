Build Instructions for Teensy 3.1

This assumes that you have TeensyDuino installed and set the ARUINO environment
variable pointing to the where Arduino with TeensyDuino is installed.

```
cd teensy
ARDUINO=~/arduino-1.0.5 make
```

To build the loader

```
cd teensy/loader
make
```

To upload micropython to the Teensy 3.1.

Press the Program button on the Teensy 3.1
```
make upload
```

Currently, the python prompt is through the USB serial interface.

The LED will blink (100 msec on/100 msec off) while waiting for the USB Serial
device to be configured, and will blink (200 msec on/200 msec off) while
sitting at the readline prompt.

Currently, there is no I/O support configured (no GPIO, ADC, etc).
