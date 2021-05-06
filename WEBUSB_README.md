<!--
SPDX-FileCopyrightText: 2014 MicroPython & CircuitPython contributors (https://github.com/adafruit/circuitpython/graphs/contributors)

SPDX-License-Identifier: MIT
-->

# WebUSB Serial Support

To date, this has only been tested on one port (esp32s2), on one board (espressif_kaluga_1).

## What it does

If you have ever used CircuitPython on a platform with a graphical LCD display, you have probably
already seen multiple "consoles" in use (although the LCD console is "output only").

New compile-time option CIRCUITPY_USB_VENDOR enables an additional "console" that can be used in
parallel with the original (CDC) serial console.

Web pages that support the WebUSB standard can connect to the "vendor" interface and activate
this WebUSB serial console at any time.

You can type into either console, and CircuitPython output is sent to all active consoles.

One example of a web page you can use to test drive this feature can be found at:

https://adafruit.github.io/Adafruit_TinyUSB_Arduino/examples/webusb-serial/index.html

## How to enable

Update your platform's mpconfigboard.mk file to enable and disable specific types of USB interfaces.

CIRCUITPY_USB_HID = xxx
CIRCUITPY_USB_MIDI = xxx
CIRCUITPY_USB_VENDOR = xxx

On at least some of the hardware platforms, the maximum number of USB endpoints is fixed.
For example, on the ESP32S2, you must pick only one of the above 3 interfaces to be enabled.

Original espressif_kaluga_1 mpconfigboard.mk settings:

CIRCUITPY_USB_HID = 1
CIRCUITPY_USB_MIDI = 0
CIRCUITPY_USB_VENDOR = 0

Settings to enable WebUSB instead:

CIRCUITPY_USB_HID = 0
CIRCUITPY_USB_MIDI = 0
CIRCUITPY_USB_VENDOR = 1

Notice that to enable VENDOR on ESP32-S2, we had to give up HID. There may be platforms that can have both, or even all three.

## Implementation Notes

CircuitPython uses the tinyusb library.

The tinyusb library already has support for WebUSB serial.
The tinyusb examples already include a "WebUSB serial" example.

    Sidenote - The use of the term "vendor" instead of "WebUSB" was done to match tinyusb.

Basically, this feature was ported into CircuitPython by pulling code snippets out of the
tinyusb example, and putting them where they best belonged in the CircuitPython codebase.

### TODO: This needs to be reworked for dynamic USB descriptors.
