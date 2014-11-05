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

#include "usbd_cdc_msc_hid0.h"

#define VCP_CHAR_NONE   (0)
#define VCP_CHAR_CTRL_A (1)
#define VCP_CHAR_CTRL_B (2)
#define VCP_CHAR_CTRL_C (3)
#define VCP_CHAR_CTRL_D (4)
#define VCP_CHAR_CTRL_E (5)

// Windows needs a different PID to distinguish different device configurations
#define USBD_PID_DEFAULT (0x9800)
#define USBD_PID_SECONDARY (0x9801)

typedef enum {
    USB_STORAGE_MEDIUM_NONE = 0,
    USB_STORAGE_MEDIUM_FLASH,
    USB_STORAGE_MEDIUM_SDCARD,
} usb_storage_medium_t;

extern struct _USBD_HandleTypeDef hUSBDDevice;
extern usb_storage_medium_t usb_storage_medium;
extern const struct _mp_obj_tuple_t pyb_usb_hid_mouse_obj;
extern const struct _mp_obj_tuple_t pyb_usb_hid_keyboard_obj;
extern const mp_obj_type_t pyb_usb_vcp_type;
extern const mp_obj_type_t pyb_usb_hid_type;
MP_DECLARE_CONST_FUN_OBJ(pyb_usb_mode_obj);

void pyb_usb_init0(void);
void pyb_usb_dev_init(uint16_t pid, usb_device_mode_t mode, USBD_HID_ModeInfoTypeDef *hid_info);
void pyb_usb_dev_deinit(void);
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
