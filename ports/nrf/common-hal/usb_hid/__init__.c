/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 hathach for Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "py/obj.h"
#include "py/mphal.h"
#include "py/runtime.h"

#include "common-hal/usb_hid/Device.h"
#include "shared-bindings/usb_hid/Device.h"
#include "tusb.h"

#define USB_HID_REPORT_LENGTH_KEYBOARD      8
#define USB_HID_REPORT_LENGTH_MOUSE         4
#define USB_HID_REPORT_LENGTH_CONSUMER      2
#define USB_HID_REPORT_LENGTH_SYS_CONTROL   1
#define USB_HID_REPORT_LENGTH_GAMEPAD       6
#define USB_HID_REPORT_LENGTH_DIGITIZER     5

#if USB_HID_DEVICE_KEYBOARD
static uint8_t keyboard_report_buffer[USB_HID_REPORT_LENGTH_KEYBOARD];
#endif

#if USB_HID_DEVICE_MOUSE
static uint8_t mouse_report_buffer[USB_HID_REPORT_LENGTH_MOUSE];
#endif

#if USB_HID_DEVICE_CONSUMER
static uint8_t consumer_report_buffer[USB_HID_REPORT_LENGTH_CONSUMER];
#endif

#if USB_HID_DEVICE_SYS_CONTROL
static uint8_t sys_control_report_buffer[USB_HID_REPORT_LENGTH_SYS_CONTROL];
#endif

#if USB_HID_DEVICE_GAMEPAD
static uint8_t gamepad_report_buffer[USB_HID_REPORT_LENGTH_GAMEPAD];
#endif

#if USB_HID_DEVICE_DIGITIZER
static uint8_t digitizer_report_buffer[USB_HID_REPORT_LENGTH_DIGITIZER];
#endif

usb_hid_device_obj_t usb_hid_devices[] = {
#if USB_HID_DEVICE_KEYBOARD
    {
        .base          = { .type = &usb_hid_device_type } ,
        .report_buffer = keyboard_report_buffer           ,
        .report_id     = USB_HID_REPORT_ID_KEYBOARD       ,
        .report_length = USB_HID_REPORT_LENGTH_KEYBOARD   ,
        .usage_page    = HID_USAGE_PAGE_DESKTOP           ,
        .usage         = HID_USAGE_DESKTOP_KEYBOARD       ,
    },
#endif

#if USB_HID_DEVICE_MOUSE
    {
        .base          = { .type = &usb_hid_device_type } ,
        .report_buffer = mouse_report_buffer              ,
        .report_id     = USB_HID_REPORT_ID_MOUSE          ,
        .report_length = USB_HID_REPORT_LENGTH_MOUSE      ,
        .usage_page    = HID_USAGE_PAGE_DESKTOP           ,
        .usage         = HID_USAGE_DESKTOP_MOUSE          ,
    },
#endif

#if USB_HID_DEVICE_CONSUMER
    {
        .base          = { .type = &usb_hid_device_type } ,
        .report_buffer = consumer_report_buffer           ,
        .report_id     = USB_HID_REPORT_ID_CONSUMER       ,
        .report_length = USB_HID_REPORT_LENGTH_CONSUMER   ,
        .usage_page    = HID_USAGE_PAGE_CONSUMER          ,
        .usage         = HID_USAGE_CONSUMER_CONTROL       ,
    },
#endif

#if USB_HID_DEVICE_SYS_CONTROL
    {
        .base          = { .type = &usb_hid_device_type }  ,
        .report_buffer = sys_control_report_buffer         ,
        .report_id     = USB_HID_REPORT_ID_SYS_CONTROL     ,
        .report_length = USB_HID_REPORT_LENGTH_SYS_CONTROL ,
        .usage_page    = HID_USAGE_PAGE_DESKTOP            ,
        .usage         = HID_USAGE_DESKTOP_SYSTEM_CONTROL  ,
    },
#endif

#if USB_HID_DEVICE_GAMEPAD
    {
        .base          = { .type = &usb_hid_device_type } ,
        .report_buffer = gamepad_report_buffer            ,
        .report_id     = USB_HID_REPORT_ID_GAMEPAD        ,
        .report_length = USB_HID_REPORT_LENGTH_GAMEPAD    ,
        .usage_page    = HID_USAGE_PAGE_DESKTOP           ,
        .usage         = HID_USAGE_DESKTOP_GAMEPAD        ,
    },
#endif

#if USB_HID_DEVICE_DIGITIZER
    {
        .base          = { .type = &usb_hid_device_type } ,
        .report_buffer = digitizer_report_buffer          ,
        .report_id     = USB_HID_REPORT_ID_DIGITIZER      ,
        .report_length = USB_HID_REPORT_LENGTH_DIGITIZER  ,
        .usage_page    = 0x0D                             ,
        .usage         = 0x02                             ,
    },
#endif
};


mp_obj_tuple_t common_hal_usb_hid_devices = {
    .base = {
        .type = &mp_type_tuple,
    },
    .len = USB_HID_NUM_DEVICES,
    .items = {
#if USB_HID_NUM_DEVICES >= 1
        (mp_obj_t) &usb_hid_devices[0],
#endif
#if USB_HID_NUM_DEVICES >= 2
        (mp_obj_t) &usb_hid_devices[1],
#endif
#if USB_HID_NUM_DEVICES >= 3
        (mp_obj_t) &usb_hid_devices[2],
#endif
#if USB_HID_NUM_DEVICES >= 4
        (mp_obj_t) &usb_hid_devices[3],
#endif
#if USB_HID_NUM_DEVICES >= 5
        (mp_obj_t) &usb_hid_devices[4],
#endif
#if USB_HID_NUM_DEVICES >= 6
        (mp_obj_t) &usb_hid_devices[5],
#endif
    }
};
