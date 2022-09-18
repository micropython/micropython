/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Damien P. George
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

#include "py/runtime.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "usbd.h"
#include "mpconfigport.h"

#include "pico/unique_id.h"

#define DESC_STR_MAX (31)

// tusb.h is not available when running the string preprocessor
#ifndef NO_QSTR
#include "tusb.h"
#endif

#if CFG_TUD_MSC
#define USBD_DESC_DEFAULT_LEN (TUD_CONFIG_DESC_LEN + TUD_CDC_DESC_LEN + TUD_MSC_DESC_LEN)
#else
#define USBD_DESC_DEFAULT_LEN (TUD_CONFIG_DESC_LEN + TUD_CDC_DESC_LEN)
#endif
#define USBD_MAX_POWER_MA (250)

#define USBD_ITF_CDC (0) // needs 2 interfaces
#define USBD_ITF_MSC (2)
#if CFG_TUD_MSC
#define USBD_ITF_MAX (3)
#else
#define USBD_ITF_MAX (2)
#endif

#define USBD_CDC_EP_CMD (0x81)
#define USBD_CDC_EP_OUT (0x02)
#define USBD_CDC_EP_IN (0x82)
#define USBD_CDC_CMD_MAX_SIZE (8)
#define USBD_CDC_IN_OUT_MAX_SIZE (64)

#define EPNUM_MSC_OUT    (0x03)
#define EPNUM_MSC_IN     (0x83)

#define USBD_STR_0 (0x00)
#define USBD_STR_MANUF (0x01)
#define USBD_STR_PRODUCT (0x02)
#define USBD_STR_SERIAL (0x03)
#define USBD_STR_CDC (0x04)
#define USBD_STR_MSC (0x05)

// Note: descriptors returned from callbacks must exist long enough for transfer to complete

typedef struct _usb_device_descriptor_obj_t {
    mp_obj_base_t base;
} usb_device_descriptor_obj_t;

// singleton
STATIC const usb_device_descriptor_obj_t usb_device_descriptor_obj = {{&usbd_descriptor_type}};

STATIC mp_obj_t usb_device_descriptor_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 0, false);

    return (mp_obj_t)&usb_device_descriptor_obj;
}

const tusb_desc_device_t usbd_desc_device_default = {
    .bLength = sizeof(tusb_desc_device_t),
    .bDescriptorType = TUSB_DESC_DEVICE,
    .bcdUSB = 0x0200,
    .bDeviceClass = TUSB_CLASS_MISC,
    .bDeviceSubClass = MISC_SUBCLASS_COMMON,
    .bDeviceProtocol = MISC_PROTOCOL_IAD,
    .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,
    .idVendor = MICROPY_HW_USB_VID,
    .idProduct = MICROPY_HW_USB_PID,
    .bcdDevice = 0x0100,
    .iManufacturer = USBD_STR_MANUF,
    .iProduct = USBD_STR_PRODUCT,
    .iSerialNumber = USBD_STR_SERIAL,
    .bNumConfigurations = 1,
};
static tusb_desc_device_t usbd_desc_device;

// device(bDeviceClass=None, bDeviceSubclass=None, bDeviceProtocol=None,
//    idVendor=None, idProduct=None, bcdDevice=None)
STATIC mp_obj_t usb_device_descriptor_device(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { 
        ARG_bDeviceClass, ARG_bDeviceSubClass, ARG_bDeviceProtocol,
        ARG_idVendor, ARG_idProduct, ARG_bcdDevice
    };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_bDeviceClass, MP_ARG_OBJ, {.u_obj = MP_ROM_NONE} },
        { MP_QSTR_bDeviceSubclass, MP_ARG_OBJ, {.u_obj = MP_ROM_NONE} },
        { MP_QSTR_bDeviceProtocol, MP_ARG_OBJ, {.u_obj = MP_ROM_NONE} },
        { MP_QSTR_idVendor, MP_ARG_OBJ, {.u_obj = MP_ROM_NONE} },
        { MP_QSTR_idProduct, MP_ARG_OBJ, {.u_obj = MP_ROM_NONE} },
        { MP_QSTR_bcdDevice, MP_ARG_OBJ, {.u_obj = MP_ROM_NONE} }
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (args[ARG_bDeviceClass].u_obj != mp_const_none) {
        usbd_desc_device.bDeviceClass = mp_obj_get_int(args[ARG_bDeviceClass].u_obj);
    }
    if (args[ARG_bDeviceSubClass].u_obj != mp_const_none) {
        usbd_desc_device.bDeviceSubClass = mp_obj_get_int(args[ARG_bDeviceSubClass].u_obj);
    }
    if (args[ARG_bDeviceProtocol].u_obj != mp_const_none) {
        usbd_desc_device.bDeviceProtocol = mp_obj_get_int(args[ARG_bDeviceProtocol].u_obj);
    }
    if (args[ARG_idVendor].u_obj != mp_const_none) {
        usbd_desc_device.idVendor = mp_obj_get_int(args[ARG_idVendor].u_obj);
    }
    if (args[ARG_idProduct].u_obj != mp_const_none) {
        usbd_desc_device.idProduct = mp_obj_get_int(args[ARG_idProduct].u_obj);
    }
    if (args[ARG_bDeviceClass].u_obj != mp_const_none) {
        usbd_desc_device.bDeviceClass = mp_obj_get_int(args[ARG_bDeviceClass].u_obj);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(usb_device_descriptor_device_obj, 1, usb_device_descriptor_device);

typedef struct {
    uint8_t cfg[512];
    int len;
    int itf_count;
    int in_count;
    int out_count;
    char names[MICROPY_HW_USB_MAX_DESCRIPTORS][DESC_STR_MAX];
    int names_len;
} desc_cfg_t;
static desc_cfg_t usbd_desc_cfg;

uint8_t usbd_desc_cfg_cdc[] = {TUD_CDC_DESCRIPTOR(USBD_ITF_CDC, USBD_STR_CDC, USBD_CDC_EP_CMD, USBD_CDC_CMD_MAX_SIZE, USBD_CDC_EP_OUT, USBD_CDC_EP_IN, USBD_CDC_IN_OUT_MAX_SIZE)};

#if CFG_TUD_MSC
uint8_t usbd_desc_cfg_msc[] = {TUD_MSC_DESCRIPTOR(USBD_ITF_MSC, 5, EPNUM_MSC_OUT, EPNUM_MSC_IN, 64)};
#endif

static int desc_cfg_namesfull(desc_cfg_t *cfg) {
    if (cfg->names_len >= MICROPY_HW_USB_MAX_DESCRIPTORS) {
        return -1;
    }
    return 0;
}

static int desc_cfg_addname(desc_cfg_t *cfg, char *name) {
    int len;
    char *spot;

    if (desc_cfg_namesfull(cfg)) {
        return -1;
    }
    len = strlen(name);
    if (len > DESC_STR_MAX-1) {
        len = DESC_STR_MAX-1;
    }
    spot = cfg->names[cfg->names_len];
    memcpy(spot, name, len);
    spot[len] = 0;
    cfg->names_len++;
    return cfg->names_len;
}

static int desc_cfg_adddesc(desc_cfg_t *cfg, uint8_t *desc, int len, char *name) {
    uint8_t *new_desc;
    uint8_t *line;
    tusb_desc_interface_t *desc_itf;
    tusb_desc_endpoint_t *desc_endpoint;
    int i;

    if (desc_cfg_namesfull(cfg)) {
        return -1;
    }

    // get the new spot
    new_desc = &(cfg->cfg[cfg->len]);
    // and copy the descriptor over
    memcpy(new_desc, desc, len);

    for (i=0; i<len; ) {
        line = &new_desc[i]; // get the new line
        if (line[1] == TUSB_DESC_INTERFACE) { // if it's an interface descriptor
            desc_itf = (tusb_desc_interface_t *) line;
            if (desc_itf->bAlternateSetting == 0) {
                desc_itf->bInterfaceNumber = cfg->itf_count;
                cfg->itf_count++;
                if (name) {
                    desc_itf->iInterface = desc_cfg_addname(cfg, name);
                    name = NULL; // only save name once
                }
            }
        } else if (line[1] == TUSB_DESC_ENDPOINT) {
            desc_endpoint = (tusb_desc_endpoint_t*) line;
             if (desc_endpoint->bEndpointAddress & 0x80) { // if it's in
                desc_endpoint->bEndpointAddress = 0x80 | cfg->in_count;
                cfg->in_count++;
            } else { // if it's out
                desc_endpoint->bEndpointAddress = cfg->out_count;
                cfg->out_count++;
            }
        }
        i += line[0]; // jump to the next "line"
        if (line[0] == 0) { // no infinite loops for me!
            return 0; // also, this means we didn't save it at all (desc_len not updated)
        }
    }
    cfg->len += len;

    return 0;
}


static void desc_cfg_init(desc_cfg_t *cfg) {
    tusb_desc_configuration_t const initial_cfg =
    {
        .bLength             = sizeof(tusb_desc_configuration_t),
        .bDescriptorType     = TUSB_DESC_CONFIGURATION,

        // Total Length & Interface Number will be updated later
        .wTotalLength        = 0,
        .bNumInterfaces      = 0,
        .bConfigurationValue = 1,
        .iConfiguration      = 0x00,
        .bmAttributes        = TU_BIT(7) | TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP,
        .bMaxPower           = TUSB_DESC_CONFIG_POWER_MA(USBD_MAX_POWER_MA)
    };

    // clear the config
    memset(cfg->cfg, 0, sizeof(cfg->cfg));
    // copy the initial contents
    memcpy(cfg->cfg, &initial_cfg, sizeof(tusb_desc_configuration_t));
    cfg->len = sizeof(tusb_desc_configuration_t);
    cfg->itf_count = 0;
    cfg->in_count = 1;
    cfg->out_count = 1;
    cfg->names_len = 0;
    desc_cfg_addname(cfg, ""); // language code
    desc_cfg_addname(cfg, "MicroPython");
    desc_cfg_addname(cfg, "Board in FS mode");
    desc_cfg_addname(cfg, ""); // this is the serial number, will be dynamic

    desc_cfg_adddesc(cfg, usbd_desc_cfg_cdc, sizeof(usbd_desc_cfg_cdc), "Board CDC");
    #if CFG_TUD_MSC
    desc_cfg_adddesc(cfg, usbd_desc_cfg_msc, sizeof(usbd_desc_cfg_msc), "Board MSC");
    #endif
}

static void desc_cfg_finalize(desc_cfg_t *cfg) {
    tusb_desc_configuration_t *conf;

    conf = (tusb_desc_configuration_t*) cfg->cfg;
    conf->wTotalLength = cfg->len; // update total length
    conf->bNumInterfaces = cfg->itf_count; // update interface count
}


const uint8_t *tud_descriptor_device_cb(void) {
    return (const uint8_t *)&usbd_desc_device;
}

const uint8_t *tud_descriptor_configuration_cb(uint8_t index) {
    (void)index;

    desc_cfg_finalize(&usbd_desc_cfg);
    return usbd_desc_cfg.cfg;
}

const uint16_t *tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
    static uint16_t desc_str[DESC_STR_MAX];

    uint8_t len;
    if (index == 0) {
        desc_str[1] = 0x0409; // supported language is English
        len = 1;
    } else {
        if (index > usbd_desc_cfg.names_len) {
            return NULL;
        }
        // check, if serial is requested
        if (index == USBD_STR_SERIAL) {
            pico_unique_board_id_t id;
            pico_get_unique_board_id(&id);
            // byte by byte conversion
            for (len = 0; len < 16; len += 2) {
                const char *hexdig = "0123456789abcdef";
                desc_str[1 + len] = hexdig[id.id[len >> 1] >> 4];
                desc_str[1 + len + 1] = hexdig[id.id[len >> 1] & 0x0f];
            }
        } else {
            const char *str = usbd_desc_cfg.names[index];
            for (len = 0; len < DESC_STR_MAX - 1 && str[len]; ++len) {
                desc_str[1 + len] = str[len];
            }
        }
    }

    // first byte is length (including header), second byte is string type
    desc_str[0] = (TUSB_DESC_STRING << 8) | (2 * len + 2);

    return desc_str;
}

void usbd_descriptor_reset(void) {
    usbd_desc_device = usbd_desc_device_default;
    desc_cfg_init(&usbd_desc_cfg);
}

STATIC const mp_rom_map_elem_t usb_device_descriptors_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_device), (mp_obj_t)&usb_device_descriptor_device_obj},

    { MP_ROM_QSTR(MP_QSTR_CLASS_UNSPECIFIED), MP_ROM_INT(TUSB_CLASS_UNSPECIFIED)},
    { MP_ROM_QSTR(MP_QSTR_CLASS_AUDIO), MP_ROM_INT(TUSB_CLASS_AUDIO)},
    { MP_ROM_QSTR(MP_QSTR_CLASS_CDC), MP_ROM_INT(TUSB_CLASS_CDC)},
    { MP_ROM_QSTR(MP_QSTR_CLASS_HID), MP_ROM_INT(TUSB_CLASS_HID)},
    { MP_ROM_QSTR(MP_QSTR_CLASS_PHYSICAL), MP_ROM_INT(TUSB_CLASS_PHYSICAL)},
    { MP_ROM_QSTR(MP_QSTR_CLASS_IMAGE), MP_ROM_INT(TUSB_CLASS_IMAGE)},
    { MP_ROM_QSTR(MP_QSTR_CLASS_PRINTER), MP_ROM_INT(TUSB_CLASS_PRINTER)},
    { MP_ROM_QSTR(MP_QSTR_CLASS_MSC), MP_ROM_INT(TUSB_CLASS_MSC)},
    { MP_ROM_QSTR(MP_QSTR_CLASS_HUB), MP_ROM_INT(TUSB_CLASS_HUB)},
    { MP_ROM_QSTR(MP_QSTR_CLASS_CDC_DATA), MP_ROM_INT(TUSB_CLASS_CDC_DATA)},
    { MP_ROM_QSTR(MP_QSTR_CLASS_SMART_CARD), MP_ROM_INT(TUSB_CLASS_SMART_CARD)},
    { MP_ROM_QSTR(MP_QSTR_CLASS_CONTENT_SECURITY), MP_ROM_INT(TUSB_CLASS_CONTENT_SECURITY)},
    { MP_ROM_QSTR(MP_QSTR_CLASS_VIDEO), MP_ROM_INT(TUSB_CLASS_VIDEO)},
    { MP_ROM_QSTR(MP_QSTR_CLASS_PERSONAL_HEALTHCARE), MP_ROM_INT(TUSB_CLASS_PERSONAL_HEALTHCARE)},
    { MP_ROM_QSTR(MP_QSTR_CLASS_AUDIO_VIDEO), MP_ROM_INT(TUSB_CLASS_AUDIO_VIDEO)},
    { MP_ROM_QSTR(MP_QSTR_CLASS_DIAGNOSTIC), MP_ROM_INT(TUSB_CLASS_DIAGNOSTIC)},
    { MP_ROM_QSTR(MP_QSTR_CLASS_WIRELESS_CONTROLLER), MP_ROM_INT(TUSB_CLASS_WIRELESS_CONTROLLER)},
    { MP_ROM_QSTR(MP_QSTR_CLASS_MISC), MP_ROM_INT(TUSB_CLASS_MISC)},
    { MP_ROM_QSTR(MP_QSTR_CLASS_APPLICATION_SPECIFIC), MP_ROM_INT(TUSB_CLASS_APPLICATION_SPECIFIC)},
    { MP_ROM_QSTR(MP_QSTR_CLASS_VENDOR_SPECIFIC), MP_ROM_INT(TUSB_CLASS_VENDOR_SPECIFIC)},
};
STATIC MP_DEFINE_CONST_DICT(usb_device_descriptors_locals_dict, usb_device_descriptors_locals_dict_table);

const mp_obj_type_t usbd_descriptor_type = {
    { &mp_type_type },
    .name = MP_QSTR_Descriptors,
    .make_new = usb_device_descriptor_make_new,
    .locals_dict = (mp_obj_t)&usb_device_descriptors_locals_dict,
};