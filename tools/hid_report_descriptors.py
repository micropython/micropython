# SPDX-FileCopyrightText: Copyright (c) 2018 Dan Halbert for Adafruit Industries
# SPDX-FileCopyrightText: 2014 MicroPython & CircuitPython contributors (https://github.com/adafruit/circuitpython/graphs/contributors)
#
# SPDX-License-Identifier: MIT

import struct

"""
HID specific descriptors
========================

* Author(s): Dan Halbert
"""

from collections import namedtuple

from adafruit_usb_descriptor import hid

# Information about each kind of device
# report_length does not include report ID in first byte, if present when sent.
DeviceData = namedtuple(
    "DeviceData", ("report_length", "out_report_length", "usage_page", "usage")
)
HID_DEVICE_DATA = {
    "KEYBOARD": DeviceData(
        report_length=8, out_report_length=1, usage_page=0x01, usage=0x06
    ),  # Generic Desktop, Keyboard
    "MOUSE": DeviceData(
        report_length=4, out_report_length=0, usage_page=0x01, usage=0x02
    ),  # Generic Desktop, Mouse
    "CONSUMER": DeviceData(
        report_length=2, out_report_length=0, usage_page=0x0C, usage=0x01
    ),  # Consumer, Consumer Control
    "SYS_CONTROL": DeviceData(
        report_length=1, out_report_length=0, usage_page=0x01, usage=0x80
    ),  # Generic Desktop, Sys Control
    "GAMEPAD": DeviceData(
        report_length=6, out_report_length=0, usage_page=0x01, usage=0x05
    ),  # Generic Desktop, Game Pad
    "DIGITIZER": DeviceData(
        report_length=5, out_report_length=0, usage_page=0x0D, usage=0x02
    ),  # Digitizers, Pen
    "XAC_COMPATIBLE_GAMEPAD": DeviceData(
        report_length=3, out_report_length=0, usage_page=0x01, usage=0x05
    ),  # Generic Desktop, Game Pad
    "RAW": DeviceData(
        report_length=64, out_report_length=0, usage_page=0xFFAF, usage=0xAF
    ),  # Vendor 0xFFAF "Adafruit", 0xAF
}

# Function to call for each kind of HID descriptor.
REPORT_DESCRIPTOR_FUNCTIONS = {
    "KEYBOARD": hid.ReportDescriptor.keyboard,
    "MOUSE": hid.ReportDescriptor.mouse,
    "CONSUMER": hid.ReportDescriptor.consumer_control,
    "SYS_CONTROL": hid.ReportDescriptor.sys_control,
    "GAMEPAD": hid.ReportDescriptor.gamepad,
    "DIGITIZER": hid.ReportDescriptor.digitizer,
    "XAC_COMPATIBLE_GAMEPAD": hid.ReportDescriptor.xac_compatible_gamepad,
    "RAW": hid.ReportDescriptor.raw,
}
