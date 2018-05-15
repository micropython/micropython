# The MIT License (MIT)
#
# Copyright (c) 2018 Dan Halbert for Adafruit Industries
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

import struct

"""
HID specific descriptors
========================

* Author(s): Dan Halbert
"""

from adafruit_usb_descriptor import hid

REPORT_IDS = {
    "KEYBOARD" : 1,
    "MOUSE" : 2,
    "CONSUMER" : 3,
    "SYS_CONTROL" : 4,
    "GAMEPAD" : 5,
    "DIGITIZER" : 6,
    }

# Byte count for each kind of report. Length does not include report ID in first byte.
REPORT_LENGTHS = {
    "KEYBOARD" : 8,
    "MOUSE" : 4,
    "CONSUMER" : 2,
    "SYS_CONTROL" : 1,
    "GAMEPAD" : 6,
    "DIGITIZER" : 5,
    }

KEYBOARD_WITH_ID = hid.ReportDescriptor(
    description="KEYBOARD",
    report_descriptor=bytes([
        # Regular keyboard
        0x05, 0x01,                 # Usage Page (Generic Desktop)
        0x09, 0x06,                 # Usage (Keyboard)
        0xA1, 0x01,                 # Collection (Application)
        0x85, REPORT_IDS["KEYBOARD"], #   Report ID (1)
        0x05, 0x07,                 #   Usage Page (Keyboard)
        0x19, 224,                  #   Usage Minimum (224)
        0x29, 231,                  #   Usage Maximum (231)
        0x15, 0x00,                 #   Logical Minimum (0)
        0x25, 0x01,                 #   Logical Maximum (1)
        0x75, 0x01,                 #   Report Size (1)
        0x95, 0x08,                 #   Report Count (8)
        0x81, 0x02,                 #   Input (Data, Variable, Absolute)
        0x81, 0x01,                 #   Input (Constant)
        0x19, 0x00,                 #   Usage Minimum (0)
        0x29, 101,                  #   Usage Maximum (101)
        0x15, 0x00,                 #   Logical Minimum (0)
        0x25, 101,                  #   Logical Maximum (101)
        0x75, 0x08,                 #   Report Size (8)
        0x95, 0x06,                 #   Report Count (6)
        0x81, 0x00,                 #   Input (Data, Array)
        0x05, 0x08,                 #   Usage Page (LED)
        0x19, 0x01,                 #   Usage Minimum (1)
        0x29, 0x05,                 #   Usage Maximum (5)
        0x15, 0x00,                 #   Logical Minimum (0)
        0x25, 0x01,                 #   Logical Maximum (1)
        0x75, 0x01,                 #   Report Size (1)
        0x95, 0x05,                 #   Report Count (5)
        0x91, 0x02,                 #   Output (Data, Variable, Absolute)
        0x95, 0x03,                 #   Report Count (3)
        0x91, 0x01,                 #   Output (Constant)
        0xC0,                       # End Collection
    ]))

MOUSE_WITH_ID = hid.ReportDescriptor(
    description="MOUSE",
    report_descriptor=bytes([
        # Regular mouse
        0x05, 0x01,        # Usage Page (Generic Desktop)
        0x09, 0x02,        # Usage (Mouse)
        0xA1, 0x01,        # Collection (Application)
        0x09, 0x01,        #   Usage (Pointer)
        0xA1, 0x00,        #   Collection (Physical)
        0x85, REPORT_IDS["MOUSE"], # Report ID (n)
        0x05, 0x09,        #     Usage Page (Button)
        0x19, 0x01,        #     Usage Minimum (0x01)
        0x29, 0x05,        #     Usage Maximum (0x05)
        0x15, 0x00,        #     Logical Minimum (0)
        0x25, 0x01,        #     Logical Maximum (1)
        0x95, 0x05,        #     Report Count (5)
        0x75, 0x01,        #     Report Size (1)
        0x81, 0x02,        #     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
        0x95, 0x01,        #     Report Count (1)
        0x75, 0x03,        #     Report Size (3)
        0x81, 0x01,        #     Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
        0x05, 0x01,        #     Usage Page (Generic Desktop Ctrls)
        0x09, 0x30,        #     Usage (X)
        0x09, 0x31,        #     Usage (Y)
        0x15, 0x81,        #     Logical Minimum (-127)
        0x25, 0x7F,        #     Logical Maximum (127)
        0x75, 0x08,        #     Report Size (8)
        0x95, 0x02,        #     Report Count (2)
        0x81, 0x06,        #     Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
        0x09, 0x38,        #     Usage (Wheel)
        0x15, 0x81,        #     Logical Minimum (-127)
        0x25, 0x7F,        #     Logical Maximum (127)
        0x75, 0x08,        #     Report Size (8)
        0x95, 0x01,        #     Report Count (1)
        0x81, 0x06,        #     Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
        0xC0,              #   End Collection
        0xC0,              # End Collection
    ]))

CONSUMER_WITH_ID = hid.ReportDescriptor(
    description="CONSUMER",
    report_descriptor=bytes([
        # Consumer ("multimedia") keys
        0x05, 0x0C,        # Usage Page (Consumer)
        0x09, 0x01,        # Usage (Consumer Control)
        0xA1, 0x01,        # Collection (Application)
        0x85, REPORT_IDS["CONSUMER"], # Report ID (n)
        0x75, 0x10,        #   Report Size (16)
        0x95, 0x01,        #   Report Count (1)
        0x15, 0x01,        #   Logical Minimum (1)
        0x26, 0x8C, 0x02,  #   Logical Maximum (652)
        0x19, 0x01,        #   Usage Minimum (Consumer Control)
        0x2A, 0x8C, 0x02,  #   Usage Maximum (AC Send)
        0x81, 0x00,        #   Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
        0xC0,              # End Collection
    ]))

SYS_CONTROL_WITH_ID = hid.ReportDescriptor(
    description="SYS_CONTROL",
    report_descriptor=bytes([
        # Power controls
        0x05, 0x01,        # Usage Page (Generic Desktop Ctrls)
        0x09, 0x80,        # Usage (Sys Control)
        0xA1, 0x01,        # Collection (Application)
        0x85, REPORT_IDS["SYS_CONTROL"], # Report ID (n)
        0x75, 0x02,        #   Report Size (2)
        0x95, 0x01,        #   Report Count (1)
        0x15, 0x01,        #   Logical Minimum (1)
        0x25, 0x03,        #   Logical Maximum (3)
        0x09, 0x82,        #   Usage (Sys Sleep)
        0x09, 0x81,        #   Usage (Sys Power Down)
        0x09, 0x83,        #   Usage (Sys Wake Up)
        0x81, 0x60,        #   Input (Data,Array,Abs,No Wrap,Linear,No Preferred State,Null State)
        0x75, 0x06,        #   Report Size (6)
        0x81, 0x03,        #   Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
        0xC0,              # End Collection
    ]))

GAMEPAD_WITH_ID = hid.ReportDescriptor(
    description="GAMEPAD",
    report_descriptor=bytes([
        # Gamepad with 16 buttons and two joysticks
        0x05, 0x01,        # Usage Page (Generic Desktop Ctrls)
        0x09, 0x05,        # Usage (Game Pad)
        0xA1, 0x01,        # Collection (Application)
        0x85, REPORT_IDS["GAMEPAD"], # Report ID (n)
        0x05, 0x09,        #   Usage Page (Button)
        0x19, 0x01,        #   Usage Minimum (Button 1)
        0x29, 0x10,        #   Usage Maximum (Button 16)
        0x15, 0x00,        #   Logical Minimum (0)
        0x25, 0x01,        #   Logical Maximum (1)
        0x75, 0x01,        #   Report Size (1)
        0x95, 0x10,        #   Report Count (16)
        0x81, 0x02,        #   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
        0x05, 0x01,        #   Usage Page (Generic Desktop Ctrls)
        0x15, 0x81,        #   Logical Minimum (-127)
        0x25, 0x7F,        #   Logical Maximum (127)
        0x09, 0x30,        #   Usage (X)
        0x09, 0x31,        #   Usage (Y)
        0x09, 0x32,        #   Usage (Z)
        0x09, 0x35,        #   Usage (Rz)
        0x75, 0x08,        #   Report Size (8)
        0x95, 0x04,        #   Report Count (4)
        0x81, 0x02,        #   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
        0xC0,              # End Collection
    ]))

DIGITIZER_WITH_ID = hid.ReportDescriptor(
    description="DIGITIZER",
    report_descriptor=bytes([
        # Digitizer (used as an absolute pointer)
        0x05, 0x0D,        # Usage Page (Digitizers)
        0x09, 0x02,        # Usage (Pen)
        0xA1, 0x01,        # Collection (Application)
        0x85, REPORT_IDS["DIGITIZER"], # Report ID (n)
        0x09, 0x01,        #   Usage (Stylus)
        0xA1, 0x00,        #   Collection (Physical)
        0x09, 0x32,        #     Usage (In-Range)
        0x09, 0x42,        #     Usage (Tip Switch)
        0x09, 0x44,        #     Usage (Barrel Switch)
        0x09, 0x45,        #     Usage (Eraser Switch)
        0x15, 0x00,        #     Logical Minimum (0)
        0x25, 0x01,        #     Logical Maximum (1)
        0x75, 0x01,        #     Report Size (1)
        0x95, 0x04,        #     Report Count (4)
        0x81, 0x02,        #     Input (Data,Var,Abs)
        0x75, 0x04,        #     Report Size (4) -- Filler
        0x95, 0x01,        #     Report Count (1) -- Filler
        0x81, 0x01,        #     Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
        0x05, 0x01,        #     Usage Page (Generic Desktop Ctrls)
        0x15, 0x00,        #     Logical Minimum (0)
        0x26, 0xff, 0x7f,  #     Logical Maximum (32767)
        0x09, 0x30,        #     Usage (X)
        0x09, 0x31,        #     Usage (Y)
        0x75, 0x10,        #     Report Size (16)
        0x95, 0x02,        #     Report Count (2)
        0x81, 0x02,        #     Input (Data,Var,Abs)
        0xC0,              #   End Collection
        0xC0,              # End Collection
    ]))

# Byte count for each kind of report. Length does not include report ID in first byte.
REPORT_DESCRIPTORS = {
    "KEYBOARD" : KEYBOARD_WITH_ID,
    "MOUSE" : MOUSE_WITH_ID,
    "CONSUMER" : CONSUMER_WITH_ID,
    "SYS_CONTROL" : SYS_CONTROL_WITH_ID,
    "GAMEPAD" : GAMEPAD_WITH_ID,
    "DIGITIZER" : DIGITIZER_WITH_ID,
    }
