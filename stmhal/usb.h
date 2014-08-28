/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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

#define VCP_CHAR_NONE   (0)
#define VCP_CHAR_CTRL_A (1)
#define VCP_CHAR_CTRL_B (2)
#define VCP_CHAR_CTRL_C (3)
#define VCP_CHAR_CTRL_D (4)
#define VCP_CHAR_CTRL_E (5)

typedef enum {
    USB_DEVICE_MODE_CDC_MSC,
    USB_DEVICE_MODE_CDC_HID,
} usb_device_mode_t;

typedef enum {
    USB_STORAGE_MEDIUM_FLASH,
    USB_STORAGE_MEDIUM_SDCARD,
} usb_storage_medium_t;

const mp_obj_type_t pyb_usb_vcp_type;

void pyb_usb_init0(void);
void pyb_usb_dev_init(usb_device_mode_t mode, usb_storage_medium_t medium);
void pyb_usb_dev_stop(void);
bool usb_vcp_is_enabled(void);
bool usb_vcp_is_connected(void);
void usb_vcp_set_interrupt_char(int c);
int usb_vcp_recv_byte(uint8_t *c); // if a byte is available, return 1 and put the byte in *c, else return 0
void usb_vcp_send_strn(const char* str, int len);
void usb_vcp_send_strn_cooked(const char *str, int len);
void usb_hid_send_report(uint8_t *buf); // 4 bytes for mouse: ?, x, y, ?

void pyb_usb_host_init(void);
void pyb_usb_host_process(void);
uint pyb_usb_host_get_keyboard(void);
