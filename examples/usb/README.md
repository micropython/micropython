## USBDevice Examples

These are examples of how to use the low-level `machine.USBDevice` class to create custom
USB device functionality.

Alternatively, there are [usb driver modules in
micropython-lib](https://github.com/micropython/micropython-lib/tree/master/micropython/usb#readme)
which provide a simpler interface and more built-in functionality.

In this directory:

* `usb_dfu_device.py` implements the USB Device Firmware Update protocol.
* `usb_simple_device.py` implements a custom USB protocol, with a matching host
  program to run on a PC.

See comments at the top of each Python file for more details.
