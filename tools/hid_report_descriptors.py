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


def keyboard_hid_descriptor(report_id):
    data = HID_DEVICE_DATA["KEYBOARD"]
    return hid.ReportDescriptor(
        description="KEYBOARD",
        report_descriptor=bytes(
            # Regular keyboard
            (
                0x05,
                data.usage_page,  # Usage Page (Generic Desktop)
                0x09,
                data.usage,  # Usage (Keyboard)
                0xA1,
                0x01,  # Collection (Application)
            )
            + ((0x85, report_id) if report_id != 0 else ())
            + (
                0x05,
                0x07,  #   Usage Page (Keyboard)
                0x19,
                224,  #   Usage Minimum (224)
                0x29,
                231,  #   Usage Maximum (231)
                0x15,
                0x00,  #   Logical Minimum (0)
                0x25,
                0x01,  #   Logical Maximum (1)
                0x75,
                0x01,  #   Report Size (1)
                0x95,
                0x08,  #   Report Count (8)
                0x81,
                0x02,  #   Input (Data, Variable, Absolute)
                0x81,
                0x01,  #   Input (Constant)
                0x19,
                0x00,  #   Usage Minimum (0)
                0x29,
                0xDD,  #   Usage Maximum (221)
                0x15,
                0x00,  #   Logical Minimum (0)
                0x25,
                0xDD,  #   Logical Maximum (221)
                0x75,
                0x08,  #   Report Size (8)
                0x95,
                0x06,  #   Report Count (6)
                0x81,
                0x00,  #   Input (Data, Array)
                0x05,
                0x08,  #   Usage Page (LED)
                0x19,
                0x01,  #   Usage Minimum (1)
                0x29,
                0x05,  #   Usage Maximum (5)
                0x15,
                0x00,  #   Logical Minimum (0)
                0x25,
                0x01,  #   Logical Maximum (1)
                0x75,
                0x01,  #   Report Size (1)
                0x95,
                0x05,  #   Report Count (5)
                0x91,
                0x02,  #   Output (Data, Variable, Absolute)
                0x95,
                0x03,  #   Report Count (3)
                0x91,
                0x01,  #   Output (Constant)
                0xC0,  # End Collection
            )
        ),
    )


def mouse_hid_descriptor(report_id):
    data = HID_DEVICE_DATA["MOUSE"]
    return hid.ReportDescriptor(
        description="MOUSE",
        report_descriptor=bytes(
            # Regular mouse
            (
                0x05,
                data.usage_page,  # Usage Page (Generic Desktop)
                0x09,
                data.usage,  # Usage (Mouse)
                0xA1,
                0x01,  # Collection (Application)
                0x09,
                0x01,  #   Usage (Pointer)
                0xA1,
                0x00,  #   Collection (Physical)
            )
            + ((0x85, report_id) if report_id != 0 else ())
            + (
                0x05,
                0x09,  #     Usage Page (Button)
                0x19,
                0x01,  #     Usage Minimum (0x01)
                0x29,
                0x05,  #     Usage Maximum (0x05)
                0x15,
                0x00,  #     Logical Minimum (0)
                0x25,
                0x01,  #     Logical Maximum (1)
                0x95,
                0x05,  #     Report Count (5)
                0x75,
                0x01,  #     Report Size (1)
                0x81,
                0x02,  #     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
                0x95,
                0x01,  #     Report Count (1)
                0x75,
                0x03,  #     Report Size (3)
                0x81,
                0x01,  #     Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
                0x05,
                0x01,  #     Usage Page (Generic Desktop Ctrls)
                0x09,
                0x30,  #     Usage (X)
                0x09,
                0x31,  #     Usage (Y)
                0x15,
                0x81,  #     Logical Minimum (-127)
                0x25,
                0x7F,  #     Logical Maximum (127)
                0x75,
                0x08,  #     Report Size (8)
                0x95,
                0x02,  #     Report Count (2)
                0x81,
                0x06,  #     Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
                0x09,
                0x38,  #     Usage (Wheel)
                0x15,
                0x81,  #     Logical Minimum (-127)
                0x25,
                0x7F,  #     Logical Maximum (127)
                0x75,
                0x08,  #     Report Size (8)
                0x95,
                0x01,  #     Report Count (1)
                0x81,
                0x06,  #     Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
                0xC0,  #   End Collection
                0xC0,  # End Collection
            )
        ),
    )


def consumer_hid_descriptor(report_id):
    data = HID_DEVICE_DATA["CONSUMER"]
    return hid.ReportDescriptor(
        description="CONSUMER",
        report_descriptor=bytes(
            # Consumer ("multimedia") keys
            (
                0x05,
                data.usage_page,  # Usage Page (Consumer)
                0x09,
                data.usage,  # Usage (Consumer Control)
                0xA1,
                0x01,  # Collection (Application)
            )
            + ((0x85, report_id) if report_id != 0 else ())
            + (
                0x75,
                0x10,  #   Report Size (16)
                0x95,
                0x01,  #   Report Count (1)
                0x15,
                0x01,  #   Logical Minimum (1)
                0x26,
                0x8C,
                0x02,  #   Logical Maximum (652)
                0x19,
                0x01,  #   Usage Minimum (Consumer Control)
                0x2A,
                0x8C,
                0x02,  #   Usage Maximum (AC Send)
                0x81,
                0x00,  #   Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
                0xC0,  # End Collection
            )
        ),
    )


def sys_control_hid_descriptor(report_id):
    data = HID_DEVICE_DATA["SYS_CONTROL"]
    return hid.ReportDescriptor(
        description="SYS_CONTROL",
        report_descriptor=bytes(
            # Power controls
            (
                0x05,
                data.usage_page,  # Usage Page (Generic Desktop Ctrls)
                0x09,
                data.usage,  # Usage (Sys Control)
                0xA1,
                0x01,  # Collection (Application)
            )
            + ((0x85, report_id) if report_id != 0 else ())
            + (
                0x75,
                0x02,  #   Report Size (2)
                0x95,
                0x01,  #   Report Count (1)
                0x15,
                0x01,  #   Logical Minimum (1)
                0x25,
                0x03,  #   Logical Maximum (3)
                0x09,
                0x82,  #   Usage (Sys Sleep)
                0x09,
                0x81,  #   Usage (Sys Power Down)
                0x09,
                0x83,  #   Usage (Sys Wake Up)
                0x81,
                0x60,  #   Input (Data,Array,Abs,No Wrap,Linear,No Preferred State,Null State)
                0x75,
                0x06,  #   Report Size (6)
                0x81,
                0x03,  #   Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
                0xC0,  # End Collection
            )
        ),
    )


def gamepad_hid_descriptor(report_id):
    data = HID_DEVICE_DATA["GAMEPAD"]
    return hid.ReportDescriptor(
        description="GAMEPAD",
        report_descriptor=bytes(
            # Gamepad with 16 buttons and two joysticks
            (
                0x05,
                data.usage_page,  # Usage Page (Generic Desktop Ctrls)
                0x09,
                data.usage,  # Usage (Game Pad)
                0xA1,
                0x01,  # Collection (Application)
            )
            + ((0x85, report_id) if report_id != 0 else ())
            + (
                0x05,
                0x09,  #   Usage Page (Button)
                0x19,
                0x01,  #   Usage Minimum (Button 1)
                0x29,
                0x10,  #   Usage Maximum (Button 16)
                0x15,
                0x00,  #   Logical Minimum (0)
                0x25,
                0x01,  #   Logical Maximum (1)
                0x75,
                0x01,  #   Report Size (1)
                0x95,
                0x10,  #   Report Count (16)
                0x81,
                0x02,  #   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
                0x05,
                0x01,  #   Usage Page (Generic Desktop Ctrls)
                0x15,
                0x81,  #   Logical Minimum (-127)
                0x25,
                0x7F,  #   Logical Maximum (127)
                0x09,
                0x30,  #   Usage (X)
                0x09,
                0x31,  #   Usage (Y)
                0x09,
                0x32,  #   Usage (Z)
                0x09,
                0x35,  #   Usage (Rz)
                0x75,
                0x08,  #   Report Size (8)
                0x95,
                0x04,  #   Report Count (4)
                0x81,
                0x02,  #   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
                0xC0,  # End Collection
            )
        ),
    )


def digitizer_hid_descriptor(report_id):
    data = HID_DEVICE_DATA["DIGITIZER"]
    return hid.ReportDescriptor(
        description="DIGITIZER",
        report_descriptor=bytes(
            # Digitizer (used as an absolute pointer)
            (
                0x05,
                data.usage_page,  # Usage Page (Digitizers)
                0x09,
                data.usage,  # Usage (Pen)
                0xA1,
                0x01,  # Collection (Application)
            )
            + ((0x85, report_id) if report_id != 0 else ())
            + (
                0x09,
                0x01,  #   Usage (Stylus)
                0xA1,
                0x00,  #   Collection (Physical)
                0x09,
                0x32,  #     Usage (In-Range)
                0x09,
                0x42,  #     Usage (Tip Switch)
                0x09,
                0x44,  #     Usage (Barrel Switch)
                0x09,
                0x45,  #     Usage (Eraser Switch)
                0x15,
                0x00,  #     Logical Minimum (0)
                0x25,
                0x01,  #     Logical Maximum (1)
                0x75,
                0x01,  #     Report Size (1)
                0x95,
                0x04,  #     Report Count (4)
                0x81,
                0x02,  #     Input (Data,Var,Abs)
                0x75,
                0x04,  #     Report Size (4) -- Filler
                0x95,
                0x01,  #     Report Count (1) -- Filler
                0x81,
                0x01,  #     Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
                0x05,
                0x01,  #     Usage Page (Generic Desktop Ctrls)
                0x15,
                0x00,  #     Logical Minimum (0)
                0x26,
                0xFF,
                0x7F,  #     Logical Maximum (32767)
                0x09,
                0x30,  #     Usage (X)
                0x09,
                0x31,  #     Usage (Y)
                0x75,
                0x10,  #     Report Size (16)
                0x95,
                0x02,  #     Report Count (2)
                0x81,
                0x02,  #     Input (Data,Var,Abs)
                0xC0,  #   End Collection
                0xC0,  # End Collection
            )
        ),
    )


def xac_compatible_gamepad_hid_descriptor(report_id):
    data = HID_DEVICE_DATA["XAC_COMPATIBLE_GAMEPAD"]
    return hid.ReportDescriptor(
        description="XAC",
        report_descriptor=bytes(
            # This descriptor mimics the simple joystick from PDP that the XBox likes
            (
                0x05,
                data.usage_page,  #  Usage Page (Desktop),
                0x09,
                data.usage,  #  Usage (Gamepad),
                0xA1,
                0x01,  #  Collection (Application),
            )
            + ((0x85, report_id) if report_id != 0 else ())
            + (
                0x15,
                0x00,  #      Logical Minimum (0),
                0x25,
                0x01,  #      Logical Maximum (1),
                0x35,
                0x00,  #      Physical Minimum (0),
                0x45,
                0x01,  #      Physical Maximum (1),
                0x75,
                0x01,  #      Report Size (1),
                0x95,
                0x08,  #      Report Count (8),
                0x05,
                0x09,  #      Usage Page (Button),
                0x19,
                0x01,  #      Usage Minimum (01h),
                0x29,
                0x08,  #      Usage Maximum (08h),
                0x81,
                0x02,  #      Input (Variable),
                0x05,
                0x01,  #      Usage Page (Desktop),
                0x26,
                0xFF,
                0x00,  #      Logical Maximum (255),
                0x46,
                0xFF,
                0x00,  #      Physical Maximum (255),
                0x09,
                0x30,  #      Usage (X),
                0x09,
                0x31,  #      Usage (Y),
                0x75,
                0x08,  #      Report Size (8),
                0x95,
                0x02,  #      Report Count (2),
                0x81,
                0x02,  #      Input (Variable),
                0xC0,  #  End Collection
            )
        ),
    )


def raw_hid_descriptor(report_id):
    if report_id != 0:
        raise ValueError("raw hid must not have a report id")
    data = HID_DEVICE_DATA["RAW"]
    return hid.ReportDescriptor(
        description="RAW",
        report_descriptor=bytes(
            # Provide vendor-defined
            # This is a two-byte page value.
            (
                0x06,
                data.usage_page & 0xFF,
                (data.usage_page >> 8) & 0xFF,  #  Usage Page (Vendor 0xFFAF "Adafruit"),
                0x09,
                data.usage,  #  Usage (AF),
                0xA1,
                0x01,  #  Collection (Application),
                0x75,
                0x08,  #      Report Size (8),
                0x15,
                0x00,  #      Logical Minimum (0),
                0x26,
                0xFF,
                0x00,  #      Logical Maximum (255),
                0x95,
                0x08,  #      Report Count (8),
                0x09,
                0x01,  #      Usage(xxx)
                0x81,
                0x02,  #      Input (Variable)
                0x95,
                0x08,  #      Report Count (8),
                0x09,
                0x02,  #      Usage(xxx)
                0x91,
                0x02,  #      Input (Variable)
                0xC0,  #  End Collection
            )
        ),
    )


# Function to call for each kind of HID descriptor.
REPORT_DESCRIPTOR_FUNCTIONS = {
    "KEYBOARD": keyboard_hid_descriptor,
    "MOUSE": mouse_hid_descriptor,
    "CONSUMER": consumer_hid_descriptor,
    "SYS_CONTROL": sys_control_hid_descriptor,
    "GAMEPAD": gamepad_hid_descriptor,
    "DIGITIZER": digitizer_hid_descriptor,
    "XAC_COMPATIBLE_GAMEPAD": xac_compatible_gamepad_hid_descriptor,
    "RAW": raw_hid_descriptor,
}
