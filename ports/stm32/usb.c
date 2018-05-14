/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014, 2015 Damien P. George
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

#include <stdarg.h>
#include <string.h>

#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_cdc_msc_hid.h"
#include "usbd_cdc_interface.h"
#include "usbd_msc_storage.h"
#include "usbd_hid_interface.h"

#include "py/objstr.h"
#include "py/runtime.h"
#include "py/stream.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "bufhelper.h"
#include "usb.h"

#if MICROPY_HW_ENABLE_USB

// Work out which USB device to use as the main one (the one with the REPL)
#if !defined(MICROPY_HW_USB_MAIN_DEV)
#if defined(MICROPY_HW_USB_FS)
#define MICROPY_HW_USB_MAIN_DEV (USB_PHY_FS_ID)
#elif defined(MICROPY_HW_USB_HS) && defined(MICROPY_HW_USB_HS_IN_FS)
#define MICROPY_HW_USB_MAIN_DEV (USB_PHY_HS_ID)
#else
#error Unable to determine proper MICROPY_HW_USB_MAIN_DEV to use
#endif
#endif

// this will be persistent across a soft-reset
mp_uint_t pyb_usb_flags = 0;

typedef struct _usb_device_t {
    uint32_t enabled;
    USBD_HandleTypeDef hUSBDDevice;
    usbd_cdc_msc_hid_state_t usbd_cdc_msc_hid_state;
    usbd_cdc_itf_t usbd_cdc_itf;
    usbd_hid_itf_t usbd_hid_itf;
} usb_device_t;

usb_device_t usb_device;
pyb_usb_storage_medium_t pyb_usb_storage_medium = PYB_USB_STORAGE_MEDIUM_NONE;

// predefined hid mouse data
STATIC const mp_obj_str_t pyb_usb_hid_mouse_desc_obj = {
    {&mp_type_bytes},
    0, // hash not valid
    USBD_HID_MOUSE_REPORT_DESC_SIZE,
    USBD_HID_MOUSE_ReportDesc,
};
const mp_obj_tuple_t pyb_usb_hid_mouse_obj = {
    {&mp_type_tuple},
    5,
    {
        MP_OBJ_NEW_SMALL_INT(1), // subclass: boot
        MP_OBJ_NEW_SMALL_INT(2), // protocol: mouse
        MP_OBJ_NEW_SMALL_INT(USBD_HID_MOUSE_MAX_PACKET),
        MP_OBJ_NEW_SMALL_INT(8), // polling interval: 8ms
        (mp_obj_t)&pyb_usb_hid_mouse_desc_obj,
    },
};

// predefined hid keyboard data
STATIC const mp_obj_str_t pyb_usb_hid_keyboard_desc_obj = {
    {&mp_type_bytes},
    0, // hash not valid
    USBD_HID_KEYBOARD_REPORT_DESC_SIZE,
    USBD_HID_KEYBOARD_ReportDesc,
};
const mp_obj_tuple_t pyb_usb_hid_keyboard_obj = {
    {&mp_type_tuple},
    5,
    {
        MP_OBJ_NEW_SMALL_INT(1), // subclass: boot
        MP_OBJ_NEW_SMALL_INT(1), // protocol: keyboard
        MP_OBJ_NEW_SMALL_INT(USBD_HID_KEYBOARD_MAX_PACKET),
        MP_OBJ_NEW_SMALL_INT(8), // polling interval: 8ms
        (mp_obj_t)&pyb_usb_hid_keyboard_desc_obj,
    },
};

void pyb_usb_init0(void) {
    mp_hal_set_interrupt_char(-1);
    MP_STATE_PORT(pyb_hid_report_desc) = MP_OBJ_NULL;
}

bool pyb_usb_dev_init(uint16_t vid, uint16_t pid, usb_device_mode_t mode, USBD_HID_ModeInfoTypeDef *hid_info) {
    bool high_speed = (mode & USBD_MODE_HIGH_SPEED) != 0;
    mode &= 0x7f;
    usb_device_t *usb_dev = &usb_device;
    if (!usb_dev->enabled) {
        // only init USB once in the device's power-lifetime

        // set up the USBD state
        USBD_HandleTypeDef *usbd = &usb_dev->hUSBDDevice;
        usbd->id = MICROPY_HW_USB_MAIN_DEV;
        usbd->dev_state  = USBD_STATE_DEFAULT;
        usbd->pDesc = (USBD_DescriptorsTypeDef*)&USBD_Descriptors;
        usbd->pClass = &USBD_CDC_MSC_HID;
        usb_dev->usbd_cdc_msc_hid_state.pdev = usbd;
        usb_dev->usbd_cdc_msc_hid_state.cdc = &usb_dev->usbd_cdc_itf.base;
        usb_dev->usbd_cdc_msc_hid_state.hid = &usb_dev->usbd_hid_itf.base;
        usbd->pClassData = &usb_dev->usbd_cdc_msc_hid_state;

        // configure the VID, PID and the USBD mode (interfaces it will expose)
        USBD_SetVIDPIDRelease(&usb_dev->usbd_cdc_msc_hid_state, vid, pid, 0x0200, mode == USBD_MODE_CDC);
        if (USBD_SelectMode(&usb_dev->usbd_cdc_msc_hid_state, mode, hid_info) != 0) {
            return false;
        }

        switch (pyb_usb_storage_medium) {
#if MICROPY_HW_HAS_SDCARD
            case PYB_USB_STORAGE_MEDIUM_SDCARD:
                USBD_MSC_RegisterStorage(&usb_dev->usbd_cdc_msc_hid_state, (USBD_StorageTypeDef*)&USBD_SDCARD_STORAGE_fops);
                break;
#endif
            default:
                USBD_MSC_RegisterStorage(&usb_dev->usbd_cdc_msc_hid_state, (USBD_StorageTypeDef*)&USBD_FLASH_STORAGE_fops);
                break;
        }

        // start the USB device
        USBD_LL_Init(usbd, high_speed);
        USBD_LL_Start(usbd);
        usb_dev->enabled = true;
    }

    return true;
}

void pyb_usb_dev_deinit(void) {
    usb_device_t *usb_dev = &usb_device;
    if (usb_dev->enabled) {
        USBD_Stop(&usb_dev->hUSBDDevice);
        usb_dev->enabled = false;
    }
}

bool usb_vcp_is_enabled(void) {
    return usb_device.enabled;
}

int usb_vcp_recv_byte(uint8_t *c) {
    return usbd_cdc_rx(&usb_device.usbd_cdc_itf, c, 1, 0);
}

void usb_vcp_send_strn(const char *str, int len) {
    if (usb_device.enabled) {
        usbd_cdc_tx_always(&usb_device.usbd_cdc_itf, (const uint8_t*)str, len);
    }
}

/******************************************************************************/
// MicroPython bindings for USB

/*
  Philosophy of USB driver and Python API: pyb.usb_mode(...) configures the USB
  on the board.  The USB itself is not an entity, rather the interfaces are, and
  can be accessed by creating objects, such as pyb.USB_VCP() and pyb.USB_HID().

  We have:

    pyb.usb_mode()          # return the current usb mode
    pyb.usb_mode(None)      # disable USB
    pyb.usb_mode('VCP')     # enable with VCP interface
    pyb.usb_mode('VCP+MSC') # enable with VCP and MSC interfaces
    pyb.usb_mode('VCP+HID') # enable with VCP and HID, defaulting to mouse protocol
    pyb.usb_mode('VCP+HID', vid=0xf055, pid=0x9800) # specify VID and PID
    pyb.usb_mode('VCP+HID', hid=pyb.hid_mouse)
    pyb.usb_mode('VCP+HID', hid=pyb.hid_keyboard)
    pyb.usb_mode('VCP+HID', pid=0x1234, hid=(subclass, protocol, max_packet_len, polling_interval, report_desc))

    vcp = pyb.USB_VCP() # get the VCP device for read/write
    hid = pyb.USB_HID() # get the HID device for write/poll

  Possible extensions:
    pyb.usb_mode('host', ...)
    pyb.usb_mode('OTG', ...)
    pyb.usb_mode(..., port=2) # for second USB port
*/

STATIC mp_obj_t pyb_usb_mode(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_mode, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_vid, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = USBD_VID} },
        { MP_QSTR_pid, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_hid, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = (mp_obj_t)&pyb_usb_hid_mouse_obj} },
        #if USBD_SUPPORT_HS_MODE
        { MP_QSTR_high_speed, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
        #endif
    };

    // fetch the current usb mode -> pyb.usb_mode()
    if (n_args == 0) {
    #if defined(USE_HOST_MODE)
        return MP_OBJ_NEW_QSTR(MP_QSTR_host);
    #else
        uint8_t mode = USBD_GetMode(&usb_device.usbd_cdc_msc_hid_state);
        switch (mode) {
            case USBD_MODE_CDC:
                return MP_OBJ_NEW_QSTR(MP_QSTR_VCP);
            case USBD_MODE_MSC:
                return MP_OBJ_NEW_QSTR(MP_QSTR_MSC);
            case USBD_MODE_HID:
                return MP_OBJ_NEW_QSTR(MP_QSTR_HID);
            case USBD_MODE_CDC_MSC:
                return MP_OBJ_NEW_QSTR(MP_QSTR_VCP_plus_MSC);
            case USBD_MODE_CDC_HID:
                return MP_OBJ_NEW_QSTR(MP_QSTR_VCP_plus_HID);
            case USBD_MODE_MSC_HID:
                return MP_OBJ_NEW_QSTR(MP_QSTR_MSC_plus_HID);
            default:
                return mp_const_none;
        }
    #endif
    }

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // record the fact that the usb has been explicitly configured
    pyb_usb_flags |= PYB_USB_FLAG_USB_MODE_CALLED;

    // check if user wants to disable the USB
    if (args[0].u_obj == mp_const_none) {
        // disable usb
        pyb_usb_dev_deinit();
        return mp_const_none;
    }

    // get mode string
    const char *mode_str = mp_obj_str_get_str(args[0].u_obj);

#if defined(USE_HOST_MODE)

    // hardware configured for USB host mode

    if (strcmp(mode_str, "host") == 0) {
        pyb_usb_host_init();
    } else {
        goto bad_mode;
    }

#else

    // hardware configured for USB device mode

    // get the VID, PID and USB mode
    // note: PID=-1 means select PID based on mode
    // note: we support CDC as a synonym for VCP for backward compatibility
    uint16_t vid = args[1].u_int;
    uint16_t pid = args[2].u_int;
    usb_device_mode_t mode;
    if (strcmp(mode_str, "CDC+MSC") == 0 || strcmp(mode_str, "VCP+MSC") == 0) {
        if (args[2].u_int == -1) {
            pid = USBD_PID_CDC_MSC;
        }
        mode = USBD_MODE_CDC_MSC;
    } else if (strcmp(mode_str, "CDC+HID") == 0 || strcmp(mode_str, "VCP+HID") == 0) {
        if (args[2].u_int == -1) {
            pid = USBD_PID_CDC_HID;
        }
        mode = USBD_MODE_CDC_HID;
    } else if (strcmp(mode_str, "CDC") == 0 || strcmp(mode_str, "VCP") == 0) {
        if (args[2].u_int == -1) {
            pid = USBD_PID_CDC;
        }
        mode = USBD_MODE_CDC;
    } else if (strcmp(mode_str, "MSC") == 0) {
        if (args[2].u_int == -1) {
            pid = USBD_PID_MSC;
        }
        mode = USBD_MODE_MSC;
    } else {
        goto bad_mode;
    }

    // get hid info if user selected such a mode
    USBD_HID_ModeInfoTypeDef hid_info;
    if (mode & USBD_MODE_HID) {
        mp_obj_t *items;
        mp_obj_get_array_fixed_n(args[3].u_obj, 5, &items);
        hid_info.subclass = mp_obj_get_int(items[0]);
        hid_info.protocol = mp_obj_get_int(items[1]);
        hid_info.max_packet_len = mp_obj_get_int(items[2]);
        hid_info.polling_interval = mp_obj_get_int(items[3]);
        mp_buffer_info_t bufinfo;
        mp_get_buffer_raise(items[4], &bufinfo, MP_BUFFER_READ);
        hid_info.report_desc = bufinfo.buf;
        hid_info.report_desc_len = bufinfo.len;

        // need to keep a copy of this so report_desc does not get GC'd
        MP_STATE_PORT(pyb_hid_report_desc) = items[4];
    }

    #if USBD_SUPPORT_HS_MODE
    if (args[4].u_bool) {
        mode |= USBD_MODE_HIGH_SPEED;
    }
    #endif

    // init the USB device
    if (!pyb_usb_dev_init(vid, pid, mode, &hid_info)) {
        goto bad_mode;
    }

#endif

    return mp_const_none;

bad_mode:
    mp_raise_ValueError("bad USB mode");
}
MP_DEFINE_CONST_FUN_OBJ_KW(pyb_usb_mode_obj, 0, pyb_usb_mode);

/******************************************************************************/
// MicroPython bindings for USB VCP

/// \moduleref pyb
/// \class USB_VCP - USB virtual comm port
///
/// The USB_VCP class allows creation of an object representing the USB
/// virtual comm port.  It can be used to read and write data over USB to
/// the connected host.

typedef struct _pyb_usb_vcp_obj_t {
    mp_obj_base_t base;
    usbd_cdc_itf_t *cdc_itf;
} pyb_usb_vcp_obj_t;

STATIC const pyb_usb_vcp_obj_t pyb_usb_vcp_obj = {{&pyb_usb_vcp_type}, &usb_device.usbd_cdc_itf};

STATIC void pyb_usb_vcp_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    mp_print_str(print, "USB_VCP()");
}

/// \classmethod \constructor()
/// Create a new USB_VCP object.
STATIC mp_obj_t pyb_usb_vcp_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 0, 0, false);

    // TODO raise exception if USB is not configured for VCP

    // return the USB VCP object
    return (mp_obj_t)&pyb_usb_vcp_obj;
}

STATIC mp_obj_t pyb_usb_vcp_setinterrupt(mp_obj_t self_in, mp_obj_t int_chr_in) {
    mp_hal_set_interrupt_char(mp_obj_get_int(int_chr_in));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_usb_vcp_setinterrupt_obj, pyb_usb_vcp_setinterrupt);

STATIC mp_obj_t pyb_usb_vcp_isconnected(mp_obj_t self_in) {
    pyb_usb_vcp_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(usbd_cdc_is_connected(self->cdc_itf));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_usb_vcp_isconnected_obj, pyb_usb_vcp_isconnected);

// deprecated in favour of USB_VCP.isconnected
STATIC mp_obj_t pyb_have_cdc(void) {
    return pyb_usb_vcp_isconnected(MP_OBJ_FROM_PTR(&pyb_usb_vcp_obj));
}
MP_DEFINE_CONST_FUN_OBJ_0(pyb_have_cdc_obj, pyb_have_cdc);

/// \method any()
/// Return `True` if any characters waiting, else `False`.
STATIC mp_obj_t pyb_usb_vcp_any(mp_obj_t self_in) {
    pyb_usb_vcp_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (usbd_cdc_rx_num(self->cdc_itf) > 0) {
        return mp_const_true;
    } else {
        return mp_const_false;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_usb_vcp_any_obj, pyb_usb_vcp_any);

/// \method send(data, *, timeout=5000)
/// Send data over the USB VCP:
///
///   - `data` is the data to send (an integer to send, or a buffer object).
///   - `timeout` is the timeout in milliseconds to wait for the send.
///
/// Return value: number of bytes sent.
STATIC const mp_arg_t pyb_usb_vcp_send_args[] = {
    { MP_QSTR_data,    MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 5000} },
};
#define PYB_USB_VCP_SEND_NUM_ARGS MP_ARRAY_SIZE(pyb_usb_vcp_send_args)

STATIC mp_obj_t pyb_usb_vcp_send(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    // parse args
    pyb_usb_vcp_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_arg_val_t vals[PYB_USB_VCP_SEND_NUM_ARGS];
    mp_arg_parse_all(n_args - 1, args + 1, kw_args, PYB_USB_VCP_SEND_NUM_ARGS, pyb_usb_vcp_send_args, vals);

    // get the buffer to send from
    mp_buffer_info_t bufinfo;
    uint8_t data[1];
    pyb_buf_get_for_send(vals[0].u_obj, &bufinfo, data);

    // send the data
    int ret = usbd_cdc_tx(self->cdc_itf, bufinfo.buf, bufinfo.len, vals[1].u_int);

    return mp_obj_new_int(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_usb_vcp_send_obj, 1, pyb_usb_vcp_send);

/// \method recv(data, *, timeout=5000)
///
/// Receive data on the bus:
///
///   - `data` can be an integer, which is the number of bytes to receive,
///     or a mutable buffer, which will be filled with received bytes.
///   - `timeout` is the timeout in milliseconds to wait for the receive.
///
/// Return value: if `data` is an integer then a new buffer of the bytes received,
/// otherwise the number of bytes read into `data` is returned.
STATIC mp_obj_t pyb_usb_vcp_recv(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    // parse args
    pyb_usb_vcp_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_arg_val_t vals[PYB_USB_VCP_SEND_NUM_ARGS];
    mp_arg_parse_all(n_args - 1, args + 1, kw_args, PYB_USB_VCP_SEND_NUM_ARGS, pyb_usb_vcp_send_args, vals);

    // get the buffer to receive into
    vstr_t vstr;
    mp_obj_t o_ret = pyb_buf_get_for_recv(vals[0].u_obj, &vstr);

    // receive the data
    int ret = usbd_cdc_rx(self->cdc_itf, (uint8_t*)vstr.buf, vstr.len, vals[1].u_int);

    // return the received data
    if (o_ret != MP_OBJ_NULL) {
        return mp_obj_new_int(ret); // number of bytes read into given buffer
    } else {
        vstr.len = ret; // set actual number of bytes read
        return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr); // create a new buffer
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_usb_vcp_recv_obj, 1, pyb_usb_vcp_recv);

mp_obj_t pyb_usb_vcp___exit__(size_t n_args, const mp_obj_t *args) {
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_usb_vcp___exit___obj, 4, 4, pyb_usb_vcp___exit__);

STATIC const mp_rom_map_elem_t pyb_usb_vcp_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_setinterrupt), MP_ROM_PTR(&pyb_usb_vcp_setinterrupt_obj) },
    { MP_ROM_QSTR(MP_QSTR_isconnected), MP_ROM_PTR(&pyb_usb_vcp_isconnected_obj) },
    { MP_ROM_QSTR(MP_QSTR_any), MP_ROM_PTR(&pyb_usb_vcp_any_obj) },
    { MP_ROM_QSTR(MP_QSTR_send), MP_ROM_PTR(&pyb_usb_vcp_send_obj) },
    { MP_ROM_QSTR(MP_QSTR_recv), MP_ROM_PTR(&pyb_usb_vcp_recv_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mp_stream_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&mp_stream_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_readline), MP_ROM_PTR(&mp_stream_unbuffered_readline_obj)},
    { MP_ROM_QSTR(MP_QSTR_readlines), MP_ROM_PTR(&mp_stream_unbuffered_readlines_obj)},
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&mp_stream_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&mp_identity_obj) },
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&mp_identity_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&mp_identity_obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&pyb_usb_vcp___exit___obj) },
};

STATIC MP_DEFINE_CONST_DICT(pyb_usb_vcp_locals_dict, pyb_usb_vcp_locals_dict_table);

STATIC mp_uint_t pyb_usb_vcp_read(mp_obj_t self_in, void *buf, mp_uint_t size, int *errcode) {
    pyb_usb_vcp_obj_t *self = MP_OBJ_TO_PTR(self_in);
    int ret = usbd_cdc_rx(self->cdc_itf, (byte*)buf, size, 0);
    if (ret == 0) {
        // return EAGAIN error to indicate non-blocking
        *errcode = MP_EAGAIN;
        return MP_STREAM_ERROR;
    }
    return ret;
}

STATIC mp_uint_t pyb_usb_vcp_write(mp_obj_t self_in, const void *buf, mp_uint_t size, int *errcode) {
    pyb_usb_vcp_obj_t *self = MP_OBJ_TO_PTR(self_in);
    int ret = usbd_cdc_tx(self->cdc_itf, (const byte*)buf, size, 0);
    if (ret == 0) {
        // return EAGAIN error to indicate non-blocking
        *errcode = MP_EAGAIN;
        return MP_STREAM_ERROR;
    }
    return ret;
}

STATIC mp_uint_t pyb_usb_vcp_ioctl(mp_obj_t self_in, mp_uint_t request, mp_uint_t arg, int *errcode) {
    mp_uint_t ret;
    pyb_usb_vcp_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (request == MP_STREAM_POLL) {
        mp_uint_t flags = arg;
        ret = 0;
        if ((flags & MP_STREAM_POLL_RD) && usbd_cdc_rx_num(self->cdc_itf) > 0) {
            ret |= MP_STREAM_POLL_RD;
        }
        if ((flags & MP_STREAM_POLL_WR) && usbd_cdc_tx_half_empty(self->cdc_itf)) {
            ret |= MP_STREAM_POLL_WR;
        }
    } else {
        *errcode = MP_EINVAL;
        ret = MP_STREAM_ERROR;
    }
    return ret;
}

STATIC const mp_stream_p_t pyb_usb_vcp_stream_p = {
    .read = pyb_usb_vcp_read,
    .write = pyb_usb_vcp_write,
    .ioctl = pyb_usb_vcp_ioctl,
};

const mp_obj_type_t pyb_usb_vcp_type = {
    { &mp_type_type },
    .name = MP_QSTR_USB_VCP,
    .print = pyb_usb_vcp_print,
    .make_new = pyb_usb_vcp_make_new,
    .getiter = mp_identity_getiter,
    .iternext = mp_stream_unbuffered_iter,
    .protocol = &pyb_usb_vcp_stream_p,
    .locals_dict = (mp_obj_dict_t*)&pyb_usb_vcp_locals_dict,
};

/******************************************************************************/
// MicroPython bindings for USB HID

typedef struct _pyb_usb_hid_obj_t {
    mp_obj_base_t base;
    usb_device_t *usb_dev;
} pyb_usb_hid_obj_t;

STATIC const pyb_usb_hid_obj_t pyb_usb_hid_obj = {{&pyb_usb_hid_type}, &usb_device};

STATIC mp_obj_t pyb_usb_hid_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 0, 0, false);

    // TODO raise exception if USB is not configured for HID

    // return the USB HID object
    return (mp_obj_t)&pyb_usb_hid_obj;
}

/// \method recv(data, *, timeout=5000)
///
/// Receive data on the bus:
///
///   - `data` can be an integer, which is the number of bytes to receive,
///     or a mutable buffer, which will be filled with received bytes.
///   - `timeout` is the timeout in milliseconds to wait for the receive.
///
/// Return value: if `data` is an integer then a new buffer of the bytes received,
/// otherwise the number of bytes read into `data` is returned.
STATIC mp_obj_t pyb_usb_hid_recv(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_data,    MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 5000} },
    };

    // parse args
    pyb_usb_hid_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_arg_val_t vals[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, vals);

    // get the buffer to receive into
    vstr_t vstr;
    mp_obj_t o_ret = pyb_buf_get_for_recv(vals[0].u_obj, &vstr);

    // receive the data
    int ret = usbd_hid_rx(&self->usb_dev->usbd_hid_itf, vstr.len, (uint8_t*)vstr.buf, vals[1].u_int);

    // return the received data
    if (o_ret != MP_OBJ_NULL) {
        return mp_obj_new_int(ret); // number of bytes read into given buffer
    } else {
        vstr.len = ret; // set actual number of bytes read
        return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr); // create a new buffer
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_usb_hid_recv_obj, 1, pyb_usb_hid_recv);

STATIC mp_obj_t pyb_usb_hid_send(mp_obj_t self_in, mp_obj_t report_in) {
    pyb_usb_hid_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_buffer_info_t bufinfo;
    byte temp_buf[8];
    // get the buffer to send from
    // we accept either a byte array, or a tuple/list of integers
    if (!mp_get_buffer(report_in, &bufinfo, MP_BUFFER_READ)) {
        mp_obj_t *items;
        mp_obj_get_array(report_in, &bufinfo.len, &items);
        if (bufinfo.len > sizeof(temp_buf)) {
            mp_raise_ValueError("tuple/list too large for HID report; use bytearray instead");
        }
        for (int i = 0; i < bufinfo.len; i++) {
            temp_buf[i] = mp_obj_get_int(items[i]);
        }
        bufinfo.buf = temp_buf;
    }

    // send the data
    if (USBD_OK == USBD_HID_SendReport(&self->usb_dev->usbd_hid_itf.base, bufinfo.buf, bufinfo.len)) {
        return mp_obj_new_int(bufinfo.len);
    } else {
        return mp_obj_new_int(0);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_usb_hid_send_obj, pyb_usb_hid_send);

// deprecated in favour of USB_HID.send
STATIC mp_obj_t pyb_hid_send_report(mp_obj_t arg) {
    return pyb_usb_hid_send(MP_OBJ_FROM_PTR(&pyb_usb_hid_obj), arg);
}
MP_DEFINE_CONST_FUN_OBJ_1(pyb_hid_send_report_obj, pyb_hid_send_report);

STATIC const mp_rom_map_elem_t pyb_usb_hid_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_send), MP_ROM_PTR(&pyb_usb_hid_send_obj) },
    { MP_ROM_QSTR(MP_QSTR_recv), MP_ROM_PTR(&pyb_usb_hid_recv_obj) },
};

STATIC MP_DEFINE_CONST_DICT(pyb_usb_hid_locals_dict, pyb_usb_hid_locals_dict_table);

STATIC mp_uint_t pyb_usb_hid_ioctl(mp_obj_t self_in, mp_uint_t request, mp_uint_t arg, int *errcode) {
    pyb_usb_hid_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_uint_t ret;
    if (request == MP_STREAM_POLL) {
        mp_uint_t flags = arg;
        ret = 0;
        if ((flags & MP_STREAM_POLL_RD) && usbd_hid_rx_num(&self->usb_dev->usbd_hid_itf) > 0) {
            ret |= MP_STREAM_POLL_RD;
        }
        if ((flags & MP_STREAM_POLL_WR) && USBD_HID_CanSendReport(&self->usb_dev->usbd_hid_itf.base)) {
            ret |= MP_STREAM_POLL_WR;
        }
    } else {
        *errcode = MP_EINVAL;
        ret = MP_STREAM_ERROR;
    }
    return ret;
}

STATIC const mp_stream_p_t pyb_usb_hid_stream_p = {
    .ioctl = pyb_usb_hid_ioctl,
};

const mp_obj_type_t pyb_usb_hid_type = {
    { &mp_type_type },
    .name = MP_QSTR_USB_HID,
    .make_new = pyb_usb_hid_make_new,
    .protocol = &pyb_usb_hid_stream_p,
    .locals_dict = (mp_obj_dict_t*)&pyb_usb_hid_locals_dict,
};

/******************************************************************************/
// code for experimental USB OTG support

#ifdef USE_HOST_MODE

#include "led.h"
#include "usbh_core.h"
#include "usbh_usr.h"
#include "usbh_hid_core.h"
#include "usbh_hid_keybd.h"
#include "usbh_hid_mouse.h"

__ALIGN_BEGIN USBH_HOST USB_Host __ALIGN_END ;

static int host_is_enabled = 0;

void pyb_usb_host_init(void) {
    if (!host_is_enabled) {
        // only init USBH once in the device's power-lifetime
        /* Init Host Library */
        USBH_Init(&USB_OTG_Core, USB_OTG_FS_CORE_ID, &USB_Host, &HID_cb, &USR_Callbacks);
    }
    host_is_enabled = 1;
}

void pyb_usb_host_process(void) {
    USBH_Process(&USB_OTG_Core, &USB_Host);
}

uint8_t usb_keyboard_key = 0;

// TODO this is an ugly hack to get key presses
uint pyb_usb_host_get_keyboard(void) {
    uint key = usb_keyboard_key;
    usb_keyboard_key = 0;
    return key;
}

void USR_MOUSE_Init(void) {
    led_state(4, 1);
    USB_OTG_BSP_mDelay(100);
    led_state(4, 0);
}

void USR_MOUSE_ProcessData(HID_MOUSE_Data_TypeDef *data) {
    led_state(4, 1);
    USB_OTG_BSP_mDelay(50);
    led_state(4, 0);
}

void USR_KEYBRD_Init(void) {
    led_state(4, 1);
    USB_OTG_BSP_mDelay(100);
    led_state(4, 0);
}

void USR_KEYBRD_ProcessData(uint8_t pbuf) {
    led_state(4, 1);
    USB_OTG_BSP_mDelay(50);
    led_state(4, 0);
    usb_keyboard_key = pbuf;
}

#endif // USE_HOST_MODE

#endif // MICROPY_HW_ENABLE_USB
