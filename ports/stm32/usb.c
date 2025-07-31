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
#include "usbd_msc_interface.h"
#include "usbd_hid_interface.h"

#include "py/objstr.h"
#include "py/runtime.h"
#include "py/stream.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "shared/runtime/mpirq.h"
#include "bufhelper.h"
#include "storage.h"
#include "sdcard.h"
#include "usb.h"

#if MICROPY_HW_ENABLE_USB

// Work out which USB device to use as the main one (the one with the REPL)
#if !defined(MICROPY_HW_USB_MAIN_DEV)
#if defined(MICROPY_HW_USB_FS)
#define MICROPY_HW_USB_MAIN_DEV (USB_PHY_FS_ID)
#elif defined(MICROPY_HW_USB_HS)
#define MICROPY_HW_USB_MAIN_DEV (USB_PHY_HS_ID)
#else
#error Unable to determine proper MICROPY_HW_USB_MAIN_DEV to use
#endif
#endif

// Maximum number of endpoints (excluding EP0)
#if defined(STM32G0) || defined(STM32G4) || defined(STM32H5) || defined(STM32L0) || defined(STM32L1) || defined(STM32WB)
#define MAX_ENDPOINT(dev_id) (7)
#elif defined(STM32L4)
#define MAX_ENDPOINT(dev_id) (5)
#elif defined(STM32F4)
#define MAX_ENDPOINT(dev_id) ((dev_id) == USB_PHY_FS_ID ? 3 : 5)
#elif defined(STM32F7)
#define MAX_ENDPOINT(dev_id) ((dev_id) == USB_PHY_FS_ID ? 5 : 8)
#elif defined(STM32H7)
#define MAX_ENDPOINT(dev_id) (8)
#endif

// Constants for USB_VCP.irq trigger.
#define USBD_CDC_IRQ_RX (1)

static void pyb_usb_vcp_init0(void);

// this will be persistent across a soft-reset
mp_uint_t pyb_usb_flags = 0;

typedef struct _usb_device_t {
    uint32_t enabled;
    USBD_HandleTypeDef hUSBDDevice;
    usbd_cdc_msc_hid_state_t usbd_cdc_msc_hid_state;
    usbd_cdc_itf_t usbd_cdc_itf[MICROPY_HW_USB_CDC_NUM];
    #if MICROPY_HW_USB_HID
    usbd_hid_itf_t usbd_hid_itf;
    #endif
} usb_device_t;

usb_device_t usb_device = {0};
pyb_usb_storage_medium_t pyb_usb_storage_medium = PYB_USB_STORAGE_MEDIUM_NONE;

#if !MICROPY_HW_USB_IS_MULTI_OTG

// Units of FIFO size arrays below are 4x 16-bit words = 8 bytes
// There are 512x 16-bit words it total to use here (when using PCD_SNG_BUF)

static const uint8_t usbd_fifo_size_cdc1[USBD_PMA_NUM_FIFO] = {
    16, 16, 16, 16,         // EP0(out), EP0(in), MSC/HID(out), MSC/HID(in)
    0, 16, 16, 16,          // unused, CDC_CMD(in), CDC_DATA(out), CDC_DATA(in)
    0, 0, 0, 0, 0, 0, 0, 0, // 8x unused
};

#if MICROPY_HW_USB_CDC_NUM >= 2
static const uint8_t usbd_fifo_size_cdc2[USBD_PMA_NUM_FIFO] = {
    8, 8, 16, 16,           // EP0(out), EP0(in), MSC/HID(out), MSC/HID(in)
    0, 8, 16, 8,            // unused, CDC_CMD(in), CDC_DATA(out), CDC_DATA(in)
    0, 8, 16, 8,            // unused, CDC2_CMD(in), CDC2_DATA(out), CDC2_DATA(in)
    0, 0, 0, 0,             // 4x unused
};

// RX; EP0(in), MSC/HID, CDC_CMD, CDC_DATA, CDC2_CMD/HID, CDC2_DATA, HID
static const uint8_t usbd_fifo_size_cdc2_msc_hid[USBD_PMA_NUM_FIFO] = {
    8, 8, 16, 16,           // EP0(out), EP0(in), MSC/HID(out), MSC/HID(in)
    0, 8, 8, 8,             // unused, CDC_CMD(in), CDC_DATA(out), CDC_DATA(in)
    0, 8, 8, 8,             // unused, CDC2_CMD(in), CDC2_DATA(out), CDC2_DATA(in)
    8, 8,                   // HID(out), HID(in)
    0, 0,                   // 2x unused
};
#endif

#else

// Units of FIFO size arrays below are 4x 32-bit words = 16 bytes
// FS: there are 320x 32-bit words in total to use here
// HS: there are 1024x 32-bit words in total to use here

// RX; EP0(in), MSC/HID, CDC_CMD, CDC_DATA
static const uint8_t usbd_fifo_size_cdc1[] = {
    32, 8, 16, 8, 16, 0, 0, // FS: RX, EP0(in), 5x IN endpoints
    #if MICROPY_HW_USB_HS
    116, 8, 64, 4, 64, 0, 0, 0, 0, 0, // HS: RX, EP0(in), 8x IN endpoints
    #endif
};

// RX; EP0(in), MSC/HID, CDC_CMD, CDC_DATA, HID
static const uint8_t usbd_fifo_size_cdc1_msc_hid[] = {
    32, 8, 16, 4, 12, 8, 0,
    #if MICROPY_HW_USB_HS
    116, 8, 64, 4, 56, 8, 0, 0, 0, 0,
    #endif
};

#if MICROPY_HW_USB_CDC_NUM >= 2
// RX; EP0(in), MSC/HID, CDC_CMD, CDC_DATA, CDC2_CMD, CDC2_DATA
static const uint8_t usbd_fifo_size_cdc2[] = {
    32, 8, 16, 4, 8, 4, 8,
    #if MICROPY_HW_USB_HS
    116, 8, 64, 2, 32, 2, 32, 0, 0, 0,
    #endif
};

// RX; EP0(in), MSC/HID, CDC_CMD, CDC_DATA, CDC2_CMD/HID, CDC2_DATA, HID
static const uint8_t usbd_fifo_size_cdc2_msc_hid[] = {
    0, 0, 0, 0, 0, 0, 0, // FS: can't support 2xVCP+MSC+HID
    #if MICROPY_HW_USB_HS
    102, 8, 64, 2, 32, 8, 32, 8, 0, 0,
    #endif
};
#endif

#if MICROPY_HW_USB_CDC_NUM >= 3
// RX; EP0(in), MSC/HID, CDC_CMD, CDC_DATA, CDC2_CMD, CDC2_DATA, CDC3_CMD, CDC3_DATA
static const uint8_t usbd_fifo_size_cdc3[] = {
    0, 0, 0, 0, 0, 0, 0, // FS: can't support 3x VCP mode
    #if MICROPY_HW_USB_HS
    82, 8, 64, 2, 32, 2, 32, 2, 32, 0,
    #endif
};

// RX; EP0(in), MSC/HID, CDC_CMD, CDC_DATA, CDC2_CMD/HID, CDC2_DATA, CDC3_CMD/HID, CDC3_DATA, HID
static const uint8_t usbd_fifo_size_cdc3_msc_hid[] = {
    0, 0, 0, 0, 0, 0, 0, // FS: can't support 3x VCP mode
    #if MICROPY_HW_USB_HS
    82, 8, 64, 2, 25, 8, 25, 8, 25, 8,
    #endif
};
#endif

#endif

#if MICROPY_HW_USB_HID
// predefined hid mouse data
static const mp_obj_str_t pyb_usb_hid_mouse_desc_obj = {
    {&mp_type_bytes},
    0, // hash not valid
    USBD_HID_MOUSE_REPORT_DESC_SIZE,
    USBD_HID_MOUSE_ReportDesc,
};
const mp_rom_obj_tuple_t pyb_usb_hid_mouse_obj = {
    {&mp_type_tuple},
    5,
    {
        MP_ROM_INT(1), // subclass: boot
        MP_ROM_INT(2), // protocol: mouse
        MP_ROM_INT(USBD_HID_MOUSE_MAX_PACKET),
        MP_ROM_INT(8), // polling interval: 8ms
        MP_ROM_PTR(&pyb_usb_hid_mouse_desc_obj),
    },
};

// predefined hid keyboard data
static const mp_obj_str_t pyb_usb_hid_keyboard_desc_obj = {
    {&mp_type_bytes},
    0, // hash not valid
    USBD_HID_KEYBOARD_REPORT_DESC_SIZE,
    USBD_HID_KEYBOARD_ReportDesc,
};
const mp_rom_obj_tuple_t pyb_usb_hid_keyboard_obj = {
    {&mp_type_tuple},
    5,
    {
        MP_ROM_INT(1), // subclass: boot
        MP_ROM_INT(1), // protocol: keyboard
        MP_ROM_INT(USBD_HID_KEYBOARD_MAX_PACKET),
        MP_ROM_INT(8), // polling interval: 8ms
        MP_ROM_PTR(&pyb_usb_hid_keyboard_desc_obj),
    },
};
#endif

void pyb_usb_init0(void) {
    for (int i = 0; i < MICROPY_HW_USB_CDC_NUM; ++i) {
        usb_device.usbd_cdc_itf[i].cdc_idx = i;
        usb_device.usbd_cdc_itf[i].attached_to_repl = false;
    }
    #if MICROPY_HW_USB_HID
    MP_STATE_PORT(pyb_hid_report_desc) = MP_OBJ_NULL;
    #endif

    pyb_usb_vcp_init0();
}

int pyb_usb_dev_detect(void) {
    if (usb_device.enabled) {
        return usb_device.hUSBDDevice.id;
    }

    #if MICROPY_HW_USB_FS && MICROPY_HW_USB_HS
    // Try to auto-detect which USB is connected by reading DP/DM pins
    for (int i = 0; i < 2; ++i) {
        mp_hal_pin_obj_t dp = i == 0 ? pyb_pin_USB_DP : pyb_pin_USB_HS_DP;
        mp_hal_pin_obj_t dm = i == 0 ? pyb_pin_USB_DM : pyb_pin_USB_HS_DM;
        mp_hal_pin_config(dp, MP_HAL_PIN_MODE_INPUT, MP_HAL_PIN_PULL_UP, 0);
        mp_hal_pin_config(dm, MP_HAL_PIN_MODE_INPUT, MP_HAL_PIN_PULL_UP, 0);
        int state = mp_hal_pin_read(dp) == 0 && mp_hal_pin_read(dm) == 0;
        mp_hal_pin_config(dp, MP_HAL_PIN_MODE_INPUT, MP_HAL_PIN_PULL_NONE, 0);
        mp_hal_pin_config(dm, MP_HAL_PIN_MODE_INPUT, MP_HAL_PIN_PULL_NONE, 0);
        if (state) {
            // DP and DM pins are actively held low so assume USB is connected
            return i == 0 ? USB_PHY_FS_ID : USB_PHY_HS_ID;
        }
    }
    #endif

    return MICROPY_HW_USB_MAIN_DEV;
}

bool pyb_usb_dev_init(int dev_id, uint16_t vid, uint16_t pid, uint8_t mode, size_t msc_n, const void *msc_unit, USBD_HID_ModeInfoTypeDef *hid_info) {
    usb_device_t *usb_dev = &usb_device;
    if (!usb_dev->enabled) {
        // only init USB once in the device's power-lifetime

        // set up the USBD state
        USBD_HandleTypeDef *usbd = &usb_dev->hUSBDDevice;
        usbd->id = dev_id;
        usbd->dev_state = USBD_STATE_DEFAULT;
        usbd->pDesc = (USBD_DescriptorsTypeDef *)&USBD_Descriptors;
        usbd->pClass = &USBD_CDC_MSC_HID;
        usb_dev->usbd_cdc_msc_hid_state.pdev = usbd;
        for (int i = 0; i < MICROPY_HW_USB_CDC_NUM; ++i) {
            usb_dev->usbd_cdc_msc_hid_state.cdc[i] = &usb_dev->usbd_cdc_itf[i].base;
        }
        #if MICROPY_HW_USB_HID
        usb_dev->usbd_cdc_msc_hid_state.hid = &usb_dev->usbd_hid_itf.base;
        #endif
        usbd->pClassData = &usb_dev->usbd_cdc_msc_hid_state;

        // configure the VID, PID and the USBD mode (interfaces it will expose)
        int cdc_only = (mode & USBD_MODE_IFACE_MASK) == USBD_MODE_CDC;
        USBD_SetVIDPIDRelease(&usb_dev->usbd_cdc_msc_hid_state, vid, pid, 0x0200, cdc_only);
        if (USBD_SelectMode(&usb_dev->usbd_cdc_msc_hid_state, mode, hid_info, MAX_ENDPOINT(dev_id)) != 0) {
            return false;
        }

        #if MICROPY_HW_USB_MSC
        // Configure the MSC interface
        const void *msc_unit_default[1];
        if (msc_n == 0) {
            msc_n = 1;
            msc_unit = msc_unit_default;
            switch (pyb_usb_storage_medium) {
                #if MICROPY_HW_ENABLE_SDCARD
                case PYB_USB_STORAGE_MEDIUM_SDCARD:
                    msc_unit_default[0] = &pyb_sdcard_type;
                    break;
                #endif
                default:
                    msc_unit_default[0] = &pyb_flash_type;
                    break;
            }
        }
        usbd_msc_init_lu(msc_n, msc_unit);
        USBD_MSC_RegisterStorage(&usb_dev->usbd_cdc_msc_hid_state, (USBD_StorageTypeDef *)&usbd_msc_fops);
        #endif

        const uint8_t *fifo_size = usbd_fifo_size_cdc1;
        #if MICROPY_HW_USB_IS_MULTI_OTG
        if ((mode & USBD_MODE_MSC_HID) == USBD_MODE_MSC_HID) {
            fifo_size = usbd_fifo_size_cdc1_msc_hid;
        }
        #endif
        #if MICROPY_HW_USB_CDC_NUM >= 3
        if (mode & USBD_MODE_IFACE_CDC(2)) {
            if ((mode & USBD_MODE_MSC_HID) == USBD_MODE_MSC_HID) {
                fifo_size = usbd_fifo_size_cdc3_msc_hid;
            } else {
                fifo_size = usbd_fifo_size_cdc3;
            }
        } else
        #endif
        #if MICROPY_HW_USB_CDC_NUM >= 2
        if (mode & USBD_MODE_IFACE_CDC(1)) {
            if ((mode & USBD_MODE_MSC_HID) == USBD_MODE_MSC_HID) {
                fifo_size = usbd_fifo_size_cdc2_msc_hid;
            } else {
                fifo_size = usbd_fifo_size_cdc2;
            }
        }
        #endif

        // start the USB device
        USBD_LL_Init(usbd, (mode & USBD_MODE_HIGH_SPEED) != 0, fifo_size);
        USBD_LL_Start(usbd);
        usb_dev->enabled = true;
    }

    return true;
}

void pyb_usb_dev_deinit(void) {
    usb_device_t *usb_dev = &usb_device;
    if (usb_dev->enabled) {
        USBD_Stop(&usb_dev->hUSBDDevice);
        USBD_DeInit(&usb_dev->hUSBDDevice);
        usb_dev->enabled = false;
    }
}

bool usb_vcp_is_enabled(void) {
    return usb_device.enabled;
}

int usb_vcp_recv_byte(uint8_t *c) {
    return usbd_cdc_rx(&usb_device.usbd_cdc_itf[0], c, 1, 0);
}

void usb_vcp_send_strn(const char *str, int len) {
    if (usb_device.enabled) {
        usbd_cdc_tx_always(&usb_device.usbd_cdc_itf[0], (const uint8_t *)str, len);
    }
}

usbd_cdc_itf_t *usb_vcp_get(int idx) {
    return &usb_device.usbd_cdc_itf[idx];
}

#if MICROPY_HW_USB_HID

// return hid interface if hid is configured, NULL otherwise
usbd_hid_itf_t *usbd_hid_get(void) {
    #if defined(USE_HOST_MODE)
    return NULL;
    #else
    uint8_t usb_mode = USBD_GetMode(&usb_device.usbd_cdc_msc_hid_state) & USBD_MODE_IFACE_MASK;
    if (usb_mode == USBD_MODE_HID || usb_mode == USBD_MODE_CDC_HID || usb_mode == USBD_MODE_MSC_HID) {
        return &usb_device.usbd_hid_itf;
    } else {
        return NULL;
    }
    #endif
}

#endif

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

typedef struct _pyb_usb_mode_table_t {
    uint8_t usbd_mode;
    uint16_t qst;
    const char *deprecated_str;
    uint16_t default_pid;
} pyb_usb_mode_table_t;

// These are all the modes supported by USBD_SelectMode.
// Note: there are some names (eg CDC, VCP+VCP) which are supported for backwards compatibility.
static const pyb_usb_mode_table_t pyb_usb_mode_table[] = {
    { USBD_MODE_CDC, MP_QSTR_VCP, "CDC", MICROPY_HW_USB_PID_CDC },
    { USBD_MODE_MSC, MP_QSTR_MSC, NULL, MICROPY_HW_USB_PID_MSC },
    { USBD_MODE_CDC_MSC, MP_QSTR_VCP_plus_MSC, "CDC+MSC", MICROPY_HW_USB_PID_CDC_MSC },
    { USBD_MODE_CDC_HID, MP_QSTR_VCP_plus_HID, "CDC+HID", MICROPY_HW_USB_PID_CDC_HID },
    { USBD_MODE_CDC_MSC_HID, MP_QSTR_VCP_plus_MSC_plus_HID, NULL, MICROPY_HW_USB_PID_CDC_MSC_HID },

    #if MICROPY_HW_USB_CDC_NUM >= 2
    { USBD_MODE_CDC2, MP_QSTR_2xVCP, "VCP+VCP", MICROPY_HW_USB_PID_CDC2 },
    { USBD_MODE_CDC2_MSC, MP_QSTR_2xVCP_plus_MSC, "VCP+VCP+MSC", MICROPY_HW_USB_PID_CDC2_MSC },
    { USBD_MODE_CDC2_MSC_HID, MP_QSTR_2xVCP_plus_MSC_plus_HID, NULL, MICROPY_HW_USB_PID_CDC2_MSC_HID },
    #endif

    #if MICROPY_HW_USB_CDC_NUM >= 3
    { USBD_MODE_CDC3, MP_QSTR_3xVCP, NULL, MICROPY_HW_USB_PID_CDC3 },
    { USBD_MODE_CDC3_MSC, MP_QSTR_3xVCP_plus_MSC, NULL, MICROPY_HW_USB_PID_CDC3_MSC },
    { USBD_MODE_CDC3_MSC_HID, MP_QSTR_3xVCP_plus_MSC_plus_HID, NULL, MICROPY_HW_USB_PID_CDC3_MSC_HID },
    #endif
};

static mp_obj_t pyb_usb_mode(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum {
        ARG_mode, ARG_port, ARG_vid, ARG_pid,
        #if MICROPY_HW_USB_MSC
        ARG_msc,
        #endif
        #if MICROPY_HW_USB_HID
        ARG_hid,
        #endif
        #if USBD_SUPPORT_HS_MODE
        ARG_high_speed
        #endif
    };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_mode, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_port, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_vid, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = MICROPY_HW_USB_VID} },
        { MP_QSTR_pid, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        #if MICROPY_HW_USB_MSC
        { MP_QSTR_msc, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_PTR(&mp_const_empty_tuple_obj)} },
        #endif
        #if MICROPY_HW_USB_HID
        { MP_QSTR_hid, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_PTR(&pyb_usb_hid_mouse_obj)} },
        #endif
        #if USBD_SUPPORT_HS_MODE
        { MP_QSTR_high_speed, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
        #endif
    };

    // fetch the current usb mode -> pyb.usb_mode()
    if (n_args == 0) {
        #if defined(USE_HOST_MODE)
        return MP_OBJ_NEW_QSTR(MP_QSTR_host);
        #else
        uint8_t mode = USBD_GetMode(&usb_device.usbd_cdc_msc_hid_state) & USBD_MODE_IFACE_MASK;
        for (size_t i = 0; i < MP_ARRAY_SIZE(pyb_usb_mode_table); ++i) {
            const pyb_usb_mode_table_t *m = &pyb_usb_mode_table[i];
            if (mode == m->usbd_mode) {
                return MP_OBJ_NEW_QSTR(m->qst);
            }
        }
        return mp_const_none;
        #endif
    }

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // record the fact that the usb has been explicitly configured
    pyb_usb_flags |= PYB_USB_FLAG_USB_MODE_CALLED;

    // check if user wants to disable the USB
    if (args[ARG_mode].u_obj == mp_const_none) {
        // disable usb
        pyb_usb_dev_deinit();
        return mp_const_none;
    }

    // get mode string
    const char *mode_str = mp_obj_str_get_str(args[ARG_mode].u_obj);

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
    uint16_t vid = args[ARG_vid].u_int;
    mp_int_t pid = args[ARG_pid].u_int;
    uint8_t mode = 0;
    for (size_t i = 0; i < MP_ARRAY_SIZE(pyb_usb_mode_table); ++i) {
        const pyb_usb_mode_table_t *m = &pyb_usb_mode_table[i];
        if (strcmp(mode_str, qstr_str(m->qst)) == 0
            || (m->deprecated_str != NULL && strcmp(mode_str, m->deprecated_str) == 0)) {
            if (pid == -1) {
                pid = m->default_pid;
            }
            mode = m->usbd_mode;
            break;
        }
    }
    if (mode == 0) {
        goto bad_mode;
    }

    // Get MSC logical units
    size_t msc_n = 0;
    const void *msc_unit[USBD_MSC_MAX_LUN];
    #if MICROPY_HW_USB_MSC
    if (mode & USBD_MODE_IFACE_MSC) {
        mp_obj_t *items;
        mp_obj_get_array(args[ARG_msc].u_obj, &msc_n, &items);
        if (msc_n > USBD_MSC_MAX_LUN) {
            mp_raise_ValueError(MP_ERROR_TEXT("too many logical units"));
        }
        for (size_t i = 0; i < msc_n; ++i) {
            const mp_obj_type_t *type = mp_obj_get_type(items[i]);
            if (type == &pyb_flash_type
                #if MICROPY_HW_ENABLE_SDCARD
                || type == &pyb_sdcard_type
                #endif
                #if MICROPY_HW_ENABLE_MMCARD
                || type == &pyb_mmcard_type
                #endif
                ) {
                msc_unit[i] = type;
            } else {
                mp_raise_ValueError(MP_ERROR_TEXT("unsupported logical unit"));
            }
        }
    }
    #endif

    // get hid info if user selected such a mode
    USBD_HID_ModeInfoTypeDef hid_info;
    #if MICROPY_HW_USB_HID
    if (mode & USBD_MODE_IFACE_HID) {
        mp_obj_t *items;
        mp_obj_get_array_fixed_n(args[ARG_hid].u_obj, 5, &items);
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
    #endif

    #if USBD_SUPPORT_HS_MODE
    if (args[ARG_high_speed].u_bool) {
        mode |= USBD_MODE_HIGH_SPEED;
    }
    #endif

    // Work out which port/peripheral to use, either user supplied or auto detect
    int dev_id = args[ARG_port].u_int;
    if (dev_id == -1) {
        dev_id = pyb_usb_dev_detect();
    }

    // init the USB device
    if (!pyb_usb_dev_init(dev_id, vid, pid, mode, msc_n, msc_unit, &hid_info)) {
        goto bad_mode;
    }

    #endif

    return mp_const_none;

bad_mode:
    mp_raise_ValueError(MP_ERROR_TEXT("bad USB mode"));
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

const pyb_usb_vcp_obj_t pyb_usb_vcp_obj[MICROPY_HW_USB_CDC_NUM] = {
    {{&pyb_usb_vcp_type}, &usb_device.usbd_cdc_itf[0]},
    #if MICROPY_HW_USB_CDC_NUM >= 2
    {{&pyb_usb_vcp_type}, &usb_device.usbd_cdc_itf[1]},
    #endif
    #if MICROPY_HW_USB_CDC_NUM >= 3
    {{&pyb_usb_vcp_type}, &usb_device.usbd_cdc_itf[2]},
    #endif
};

static bool pyb_usb_vcp_irq_scheduled[MICROPY_HW_USB_CDC_NUM];

static void pyb_usb_vcp_init0(void) {
    for (size_t i = 0; i < MICROPY_HW_USB_CDC_NUM; ++i) {
        MP_STATE_PORT(pyb_usb_vcp_irq)[i] = mp_const_none;
        pyb_usb_vcp_irq_scheduled[i] = false;
    }

    // Activate USB_VCP(0) on dupterm slot 1 for the REPL
    MP_STATE_VM(dupterm_objs[1]) = MP_OBJ_FROM_PTR(&pyb_usb_vcp_obj[0]);
    usb_vcp_attach_to_repl(&pyb_usb_vcp_obj[0], true);
}

static mp_obj_t pyb_usb_vcp_irq_run(mp_obj_t self_in) {
    pyb_usb_vcp_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint8_t idx = self->cdc_itf->cdc_idx;
    mp_obj_t callback = MP_STATE_PORT(pyb_usb_vcp_irq)[idx];
    pyb_usb_vcp_irq_scheduled[idx] = false;
    if (callback != mp_const_none && usbd_cdc_rx_num(self->cdc_itf)) {
        mp_call_function_1(callback, self_in);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(pyb_usb_vcp_irq_run_obj, pyb_usb_vcp_irq_run);

void usbd_cdc_rx_event_callback(usbd_cdc_itf_t *cdc) {
    uint8_t idx = cdc->cdc_idx;
    mp_obj_t self = MP_OBJ_FROM_PTR(&pyb_usb_vcp_obj[idx]);
    mp_obj_t callback = MP_STATE_PORT(pyb_usb_vcp_irq)[idx];
    if (callback != mp_const_none && !pyb_usb_vcp_irq_scheduled[idx]) {
        pyb_usb_vcp_irq_scheduled[idx] = mp_sched_schedule(MP_OBJ_FROM_PTR(&pyb_usb_vcp_irq_run_obj), self);
    }
}

static void pyb_usb_vcp_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    int id = ((pyb_usb_vcp_obj_t *)MP_OBJ_TO_PTR(self_in))->cdc_itf->cdc_idx;
    mp_printf(print, "USB_VCP(%u)", id);
}

void usb_vcp_attach_to_repl(const pyb_usb_vcp_obj_t *self, bool attached) {
    self->cdc_itf->attached_to_repl = attached;
    if (attached) {
        // Default behavior is non-blocking when attached to repl
        self->cdc_itf->flow &= ~USBD_CDC_FLOWCONTROL_CTS;
    } else {
        self->cdc_itf->flow |= USBD_CDC_FLOWCONTROL_CTS;
    }
}

/// \classmethod \constructor()
/// Create a new USB_VCP object.
static mp_obj_t pyb_usb_vcp_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 0, 1, false);

    // TODO raise exception if USB is not configured for VCP

    int id = (n_args == 0) ? 0 : mp_obj_get_int(args[0]);
    if (0 <= id && id < MICROPY_HW_USB_CDC_NUM) {
        return MP_OBJ_FROM_PTR(&pyb_usb_vcp_obj[id]);
    } else {
        mp_raise_ValueError(NULL);
    }
}

// init(*, flow=-1)
static mp_obj_t pyb_usb_vcp_init(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_flow };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_flow, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    pyb_usb_vcp_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // flow control
    if (args[ARG_flow].u_int != -1) {
        self->cdc_itf->flow = args[ARG_flow].u_int;
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(pyb_usb_vcp_init_obj, 1, pyb_usb_vcp_init);

static mp_obj_t pyb_usb_vcp_setinterrupt(mp_obj_t self_in, mp_obj_t int_chr_in) {
    mp_hal_set_interrupt_char(mp_obj_get_int(int_chr_in));
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(pyb_usb_vcp_setinterrupt_obj, pyb_usb_vcp_setinterrupt);

static mp_obj_t pyb_usb_vcp_isconnected(mp_obj_t self_in) {
    pyb_usb_vcp_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(usbd_cdc_is_connected(self->cdc_itf));
}
static MP_DEFINE_CONST_FUN_OBJ_1(pyb_usb_vcp_isconnected_obj, pyb_usb_vcp_isconnected);

// deprecated in favour of USB_VCP.isconnected
static mp_obj_t pyb_have_cdc(void) {
    return pyb_usb_vcp_isconnected(MP_OBJ_FROM_PTR(&pyb_usb_vcp_obj[0]));
}
MP_DEFINE_CONST_FUN_OBJ_0(pyb_have_cdc_obj, pyb_have_cdc);

/// \method any()
/// Return `True` if any characters waiting, else `False`.
static mp_obj_t pyb_usb_vcp_any(mp_obj_t self_in) {
    pyb_usb_vcp_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (usbd_cdc_rx_num(self->cdc_itf) > 0) {
        return mp_const_true;
    } else {
        return mp_const_false;
    }
}
static MP_DEFINE_CONST_FUN_OBJ_1(pyb_usb_vcp_any_obj, pyb_usb_vcp_any);

/// \method send(data, *, timeout=5000)
/// Send data over the USB VCP:
///
///   - `data` is the data to send (an integer to send, or a buffer object).
///   - `timeout` is the timeout in milliseconds to wait for the send.
///
/// Return value: number of bytes sent.
static const mp_arg_t pyb_usb_vcp_send_args[] = {
    { MP_QSTR_data,    MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 5000} },
};
#define PYB_USB_VCP_SEND_NUM_ARGS MP_ARRAY_SIZE(pyb_usb_vcp_send_args)

static mp_obj_t pyb_usb_vcp_send(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
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
static MP_DEFINE_CONST_FUN_OBJ_KW(pyb_usb_vcp_send_obj, 1, pyb_usb_vcp_send);

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
static mp_obj_t pyb_usb_vcp_recv(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    // parse args
    pyb_usb_vcp_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_arg_val_t vals[PYB_USB_VCP_SEND_NUM_ARGS];
    mp_arg_parse_all(n_args - 1, args + 1, kw_args, PYB_USB_VCP_SEND_NUM_ARGS, pyb_usb_vcp_send_args, vals);

    // get the buffer to receive into
    vstr_t vstr;
    mp_obj_t o_ret = pyb_buf_get_for_recv(vals[0].u_obj, &vstr);

    // receive the data
    int ret = usbd_cdc_rx(self->cdc_itf, (uint8_t *)vstr.buf, vstr.len, vals[1].u_int);

    // return the received data
    if (o_ret != MP_OBJ_NULL) {
        return mp_obj_new_int(ret); // number of bytes read into given buffer
    } else {
        vstr.len = ret; // set actual number of bytes read
        return mp_obj_new_bytes_from_vstr(&vstr); // create a new buffer
    }
}
static MP_DEFINE_CONST_FUN_OBJ_KW(pyb_usb_vcp_recv_obj, 1, pyb_usb_vcp_recv);

// irq(handler=None, trigger=IRQ_RX, hard=False)
static mp_obj_t pyb_usb_vcp_irq(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_handler, ARG_trigger, ARG_hard };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_handler, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_trigger, MP_ARG_INT, {.u_int = USBD_CDC_IRQ_RX} },
        { MP_QSTR_hard, MP_ARG_BOOL, {.u_bool = false} },
    };
    pyb_usb_vcp_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (n_args > 1 || kw_args->used != 0) {
        // Check the handler.
        mp_obj_t handler = args[MP_IRQ_ARG_INIT_handler].u_obj;
        if (handler != mp_const_none && !mp_obj_is_callable(handler)) {
            mp_raise_ValueError(MP_ERROR_TEXT("handler must be None or callable"));
        }

        // Check the trigger.
        mp_uint_t trigger = args[MP_IRQ_ARG_INIT_trigger].u_int;
        if (trigger == 0) {
            handler = mp_const_none;
        } else if (trigger != USBD_CDC_IRQ_RX) {
            mp_raise_ValueError(MP_ERROR_TEXT("unsupported trigger"));
        }

        // Check hard/soft.
        if (args[MP_IRQ_ARG_INIT_hard].u_bool) {
            mp_raise_ValueError(MP_ERROR_TEXT("hard unsupported"));
        }

        // Reconfigure the IRQ.
        MP_STATE_PORT(pyb_usb_vcp_irq)[self->cdc_itf->cdc_idx] = handler;
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(pyb_usb_vcp_irq_obj, 1, pyb_usb_vcp_irq);

static const mp_rom_map_elem_t pyb_usb_vcp_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&pyb_usb_vcp_init_obj) },
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
    { MP_ROM_QSTR(MP_QSTR_irq), MP_ROM_PTR(&pyb_usb_vcp_irq_obj) },
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&mp_identity_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&mp_identity_obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&mp_stream___exit___obj) },

    // class constants
    { MP_ROM_QSTR(MP_QSTR_RTS), MP_ROM_INT(USBD_CDC_FLOWCONTROL_RTS) },
    { MP_ROM_QSTR(MP_QSTR_CTS), MP_ROM_INT(USBD_CDC_FLOWCONTROL_CTS) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_RX), MP_ROM_INT(USBD_CDC_IRQ_RX) },
};

static MP_DEFINE_CONST_DICT(pyb_usb_vcp_locals_dict, pyb_usb_vcp_locals_dict_table);

static mp_uint_t pyb_usb_vcp_read(mp_obj_t self_in, void *buf, mp_uint_t size, int *errcode) {
    pyb_usb_vcp_obj_t *self = MP_OBJ_TO_PTR(self_in);
    int ret = usbd_cdc_rx(self->cdc_itf, (byte *)buf, size, 0);
    if (ret == 0) {
        // return EAGAIN error to indicate non-blocking
        *errcode = MP_EAGAIN;
        return MP_STREAM_ERROR;
    }
    return ret;
}

static mp_uint_t pyb_usb_vcp_write(mp_obj_t self_in, const void *buf, mp_uint_t size, int *errcode) {
    pyb_usb_vcp_obj_t *self = MP_OBJ_TO_PTR(self_in);
    int ret = usbd_cdc_tx_flow(self->cdc_itf, (const byte *)buf, size);
    if (ret == 0) {
        // return EAGAIN error to indicate non-blocking
        *errcode = MP_EAGAIN;
        return MP_STREAM_ERROR;
    }
    return ret;
}

static mp_uint_t pyb_usb_vcp_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    mp_uint_t ret;
    pyb_usb_vcp_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (request == MP_STREAM_POLL) {
        uintptr_t flags = arg;
        ret = 0;
        if ((flags & MP_STREAM_POLL_RD) && usbd_cdc_rx_num(self->cdc_itf) > 0) {
            ret |= MP_STREAM_POLL_RD;
        }
        if ((flags & MP_STREAM_POLL_WR) && usbd_cdc_tx_half_empty(self->cdc_itf)) {
            ret |= MP_STREAM_POLL_WR;
        }
    } else if (request == MP_STREAM_CLOSE) {
        ret = 0;
    } else {
        *errcode = MP_EINVAL;
        ret = MP_STREAM_ERROR;
    }
    return ret;
}

static const mp_stream_p_t pyb_usb_vcp_stream_p = {
    .read = pyb_usb_vcp_read,
    .write = pyb_usb_vcp_write,
    .ioctl = pyb_usb_vcp_ioctl,
};

MP_DEFINE_CONST_OBJ_TYPE(
    pyb_usb_vcp_type,
    MP_QSTR_USB_VCP,
    MP_TYPE_FLAG_ITER_IS_STREAM,
    make_new, pyb_usb_vcp_make_new,
    print, pyb_usb_vcp_print,
    protocol, &pyb_usb_vcp_stream_p,
    locals_dict, &pyb_usb_vcp_locals_dict
    );

/******************************************************************************/
// MicroPython bindings for USB HID

#if MICROPY_HW_USB_HID

typedef struct _pyb_usb_hid_obj_t {
    mp_obj_base_t base;
    usb_device_t *usb_dev;
} pyb_usb_hid_obj_t;

static const pyb_usb_hid_obj_t pyb_usb_hid_obj = {{&pyb_usb_hid_type}, &usb_device};

static mp_obj_t pyb_usb_hid_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 0, 0, false);

    // TODO raise exception if USB is not configured for HID

    // return the USB HID object
    return MP_OBJ_FROM_PTR(&pyb_usb_hid_obj);
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
static mp_obj_t pyb_usb_hid_recv(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
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
    int ret = usbd_hid_rx(&self->usb_dev->usbd_hid_itf, vstr.len, (uint8_t *)vstr.buf, vals[1].u_int);

    if (ret < 0) {
        // error, just return 0/empty bytes
        ret = 0;
    }

    // return the received data
    if (o_ret != MP_OBJ_NULL) {
        return mp_obj_new_int(ret); // number of bytes read into given buffer
    } else {
        vstr.len = ret; // set actual number of bytes read
        return mp_obj_new_bytes_from_vstr(&vstr); // create a new buffer
    }
}
static MP_DEFINE_CONST_FUN_OBJ_KW(pyb_usb_hid_recv_obj, 1, pyb_usb_hid_recv);

static mp_obj_t pyb_usb_hid_send(mp_obj_t self_in, mp_obj_t report_in) {
    pyb_usb_hid_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_buffer_info_t bufinfo;
    byte temp_buf[8];
    // get the buffer to send from
    // we accept either a byte array, or a tuple/list of integers
    if (!mp_get_buffer(report_in, &bufinfo, MP_BUFFER_READ)) {
        mp_obj_t *items;
        mp_obj_get_array(report_in, &bufinfo.len, &items);
        if (bufinfo.len > sizeof(temp_buf)) {
            mp_raise_ValueError(MP_ERROR_TEXT("tuple/list too large for HID report; use bytearray instead"));
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
static MP_DEFINE_CONST_FUN_OBJ_2(pyb_usb_hid_send_obj, pyb_usb_hid_send);

// deprecated in favour of USB_HID.send
static mp_obj_t pyb_hid_send_report(mp_obj_t arg) {
    return pyb_usb_hid_send(MP_OBJ_FROM_PTR(&pyb_usb_hid_obj), arg);
}
MP_DEFINE_CONST_FUN_OBJ_1(pyb_hid_send_report_obj, pyb_hid_send_report);

static const mp_rom_map_elem_t pyb_usb_hid_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_send), MP_ROM_PTR(&pyb_usb_hid_send_obj) },
    { MP_ROM_QSTR(MP_QSTR_recv), MP_ROM_PTR(&pyb_usb_hid_recv_obj) },
};

static MP_DEFINE_CONST_DICT(pyb_usb_hid_locals_dict, pyb_usb_hid_locals_dict_table);

static mp_uint_t pyb_usb_hid_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    pyb_usb_hid_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_uint_t ret;
    if (request == MP_STREAM_POLL) {
        uintptr_t flags = arg;
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

static const mp_stream_p_t pyb_usb_hid_stream_p = {
    .ioctl = pyb_usb_hid_ioctl,
};

MP_DEFINE_CONST_OBJ_TYPE(
    pyb_usb_hid_type,
    MP_QSTR_USB_HID,
    MP_TYPE_FLAG_NONE,
    make_new, pyb_usb_hid_make_new,
    protocol, &pyb_usb_hid_stream_p,
    locals_dict, &pyb_usb_hid_locals_dict
    );

#endif // MICROPY_HW_USB_HID

/******************************************************************************/
// code for experimental USB OTG support

#ifdef USE_HOST_MODE

#include "led.h"
#include "usbh_core.h"
#include "usbh_usr.h"
#include "usbh_hid_core.h"
#include "usbh_hid_keybd.h"
#include "usbh_hid_mouse.h"

__ALIGN_BEGIN USBH_HOST USB_Host __ALIGN_END;

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

#if MICROPY_HW_USB_HID
MP_REGISTER_ROOT_POINTER(mp_obj_t pyb_hid_report_desc);
#endif
MP_REGISTER_ROOT_POINTER(mp_obj_t pyb_usb_vcp_irq[MICROPY_HW_USB_CDC_NUM]);

#endif // MICROPY_HW_ENABLE_USB
