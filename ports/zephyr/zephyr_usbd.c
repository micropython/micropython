/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024-2025 MASSDRIVER EI (massdriver.space)
 * Copyright (c) 2026 Panoramix Labs
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

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <zephyr/kernel.h>
#include <zephyr/usb/usbd.h>
#include <zephyr/usb/class/usbd_uvc.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(zephyr_usbd);

#include "py/runtime.h"
#include "py/gc.h"
#include "py/mphal.h"
#include "py/mperrno.h"
#include "py/binary.h"
#include "zephyr_device.h"

#ifdef CONFIG_USB_DEVICE_STACK_NEXT

typedef struct _mp_obj_zephyr_usbd_t {
    mp_obj_base_t base;
    bool active;

    #if CONFIG_USBD_VIDEO_CLASS
    const struct device *uvc_dev;
    #endif
} mp_obj_zephyr_usbd_t;

// Helper to convert a (mp_obj_t)string to an (uint32_t)pixelformat
#define VIDEO_FOURCC_FROM_MP_OBJ(obj) \
    ({ \
        const char *_fourcc_str = mp_obj_str_get_str(obj); \
        strlen(_fourcc_str) < 3 ? 0x00000000 : VIDEO_FOURCC_FROM_STR(_fourcc_str); \
    })

static mp_obj_t zephyr_usbd_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    (void)type;
    (void)n_args;
    (void)n_kw;
    (void)args;

    if (MP_STATE_VM(usbd) != MP_OBJ_NULL) {
        return MP_STATE_VM(usbd);
    }

    mp_arg_check_num(n_args, n_kw, 0, 0, false);
    mp_obj_zephyr_usbd_t *self = mp_obj_malloc(mp_obj_zephyr_usbd_t, type);

    self->active = false;

    return MP_OBJ_FROM_PTR(self);
}

/* By default, do not register the USB DFU class DFU mode instance. */
static const char *const blocklist[] = {
    "dfu_dfu",
    NULL,
};

USBD_DEVICE_DEFINE(mp_usbd,
    DEVICE_DT_GET(DT_NODELABEL(zephyr_udc0)),
    CONFIG_MICROPY_USB_DEVICE_VID, CONFIG_MICROPY_USB_DEVICE_PID);

USBD_DESC_LANG_DEFINE(mp_lang);
USBD_DESC_MANUFACTURER_DEFINE(mp_mfr, "Zephyr Project");
USBD_DESC_PRODUCT_DEFINE(mp_product, "MicroPython on Zephyr RTOS");
USBD_DESC_SERIAL_NUMBER_DEFINE(mp_sn);

USBD_DESC_CONFIG_DEFINE(fs_cfg_desc, "FS Configuration");
USBD_DESC_CONFIG_DEFINE(hs_cfg_desc, "HS Configuration");

/* not self-powered, no remote wakeup  */
static const uint8_t attributes = 0;

/* Full speed configuration
* power = 250 * 2 mA = 500mA
*/
USBD_CONFIGURATION_DEFINE(mp_fs_config,
    attributes,
    250, &fs_cfg_desc);

/* High speed configuration */
USBD_CONFIGURATION_DEFINE(mp_hs_config,
    attributes,
    250, &hs_cfg_desc);

static void mp_fix_code_triple(struct usbd_context *uds_ctx, const enum usbd_speed speed) {
    /* Always use class code information from Interface Descriptors */
    if (IS_ENABLED(CONFIG_USBD_CDC_ACM_CLASS) ||
        IS_ENABLED(CONFIG_USBD_CDC_ECM_CLASS) ||
        IS_ENABLED(CONFIG_USBD_CDC_NCM_CLASS) ||
        IS_ENABLED(CONFIG_USBD_MIDI2_CLASS) ||
        IS_ENABLED(CONFIG_USBD_AUDIO2_CLASS) ||
        IS_ENABLED(CONFIG_USBD_VIDEO_CLASS)) {
        /*
         * Class with multiple interfaces have an Interface
         * Association Descriptor available, use an appropriate triple
         * to indicate it.
         */
        usbd_device_set_code_triple(uds_ctx, speed,
            USB_BCC_MISCELLANEOUS, 0x02, 0x01);
    } else {
        usbd_device_set_code_triple(uds_ctx, speed, 0, 0, 0);
    }
}

int mp_usbd_init(void) {
    int err;

    err = usbd_add_descriptor(&mp_usbd, &mp_lang);
    if (err) {
        LOG_ERR("Failed to initialize language descriptor (%d)", err);
        return err;
    }

    err = usbd_add_descriptor(&mp_usbd, &mp_mfr);
    if (err) {
        LOG_ERR("Failed to initialize manufacturer descriptor (%d)", err);
        return err;
    }

    err = usbd_add_descriptor(&mp_usbd, &mp_product);
    if (err) {
        LOG_ERR("Failed to initialize product descriptor (%d)", err);
        return err;
    }

    IF_ENABLED(CONFIG_HWINFO, (
        err = usbd_add_descriptor(&mp_usbd, &mp_sn);
        ))
    if (err) {
        LOG_ERR("Failed to initialize SN descriptor (%d)", err);
        return err;
    }

    if (usbd_caps_speed(&mp_usbd) == USBD_SPEED_HS) {
        err = usbd_add_configuration(&mp_usbd, USBD_SPEED_HS, &mp_hs_config);
        if (err) {
            LOG_ERR("Failed to add High-Speed configuration");
            return err;
        }

        err = usbd_register_all_classes(&mp_usbd, USBD_SPEED_HS, 1, blocklist);
        if (err) {
            LOG_ERR("Failed to add register classes");
            return err;
        }

        mp_fix_code_triple(&mp_usbd, USBD_SPEED_HS);
    }

    err = usbd_add_configuration(&mp_usbd, USBD_SPEED_FS, &mp_fs_config);
    if (err) {
        LOG_ERR("Failed to add Full-Speed configuration");
        return err;
    }

    err = usbd_register_all_classes(&mp_usbd, USBD_SPEED_FS, 1, blocklist);
    if (err) {
        LOG_ERR("Failed to add register classes");
        return err;
    }

    mp_fix_code_triple(&mp_usbd, USBD_SPEED_FS);

    #if !CONFIG_MICROPY_ZEPHYR_USBD_RUNTIME

    err = usbd_init(&mp_usbd);
    if (err) {
        LOG_ERR("Failed to initialize USB device support");
        return err;
    }

    err = usbd_enable(&mp_usbd);
    if (err) {
        LOG_ERR("Failed to enable USB device support");
        return err;
    }

    #endif // CONFIG_MICROPY_ZEPHYR_USBD_RUNTIME

    return 0;
}

static mp_obj_t zephyr_usbd_active(size_t n_args, const mp_obj_t *args) {
    mp_obj_zephyr_usbd_t *self = MP_OBJ_TO_PTR(args[0]);
    int ret;

    if (n_args == 1) {
        return self->active ? mp_const_true : mp_const_false;
    }

    if (mp_obj_is_true(args[1])) {
        ret = usbd_init(&mp_usbd);
        if (ret) {
            mp_raise_OSError(-ret);
        }

        ret = usbd_enable(&mp_usbd);
        if (ret) {
            mp_raise_OSError(-ret);
        }

        self->active = true;
    } else {
        ret = usbd_disable(&mp_usbd);
        if (ret) {
            mp_raise_OSError(-ret);
        }

        ret = usbd_shutdown(&mp_usbd);
        if (ret) {
            mp_raise_OSError(-ret);
        }

        self->active = false;
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(zephyr_usbd_active_obj, 1, 2, zephyr_usbd_active);

#if CONFIG_USBD_VIDEO_CLASS
static mp_obj_t zephyr_usbd_config_uvc(mp_obj_t uvc_source_obj, mp_obj_t uvc_formats_obj) {
    const struct device *uvc_dev = DEVICE_DT_GET(DT_NODELABEL(uvc0));
    const struct device *src_dev = zephyr_device_find(uvc_source_obj);
    if (src_dev == NULL) {
        mp_raise_ValueError("No device by that name");
    }
    if (!DEVICE_API_IS(video, src_dev)) {
        mp_raise_ValueError("Not a video device");
    }
    uvc_device_init(uvc_dev, src_dev);

    size_t uvc_formats_len;
    mp_obj_t *uvc_formats_items;
    mp_obj_get_array(uvc_formats_obj, &uvc_formats_len, &uvc_formats_items);

    for (size_t i = 0; i < uvc_formats_len; i++) {
        mp_obj_t *fmt_items;
        mp_obj_get_array_fixed_n(uvc_formats_items[i], 3, &fmt_items);
        struct video_format fmt = {
            .pixelformat = VIDEO_FOURCC_FROM_MP_OBJ(fmt_items[0]),
            .width = mp_obj_get_int(fmt_items[1]),
            .height = mp_obj_get_int(fmt_items[2]),
        };
        video_estimate_fmt_size(&fmt);
        uvc_device_add_format(uvc_dev, &fmt);
    }

    uvc_device_enable(uvc_dev);

    return mp_const_none;
}
#endif

static mp_obj_t zephyr_usbd_config(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_id, ARG_uvc, ARG_uvc_source, ARG_uvc_formats };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id, MP_ARG_REQUIRED | MP_ARG_OBJ, },
        { MP_QSTR_uvc, MP_ARG_KW_ONLY | MP_ARG_BOOL, { .u_bool = mp_const_false } },
        { MP_QSTR_uvc_source, MP_ARG_KW_ONLY | MP_ARG_OBJ, { .u_obj = mp_const_none } },
        { MP_QSTR_uvc_formats, MP_ARG_KW_ONLY | MP_ARG_OBJ, { .u_obj = mp_const_none } },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    #if CONFIG_USBD_VIDEO_CLASS
    if (args[ARG_uvc].u_bool) {
        zephyr_usbd_config_uvc(args[ARG_uvc_source].u_obj, args[ARG_uvc_formats].u_obj);
    }
    #endif

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(zephyr_usbd_config_obj, 1, zephyr_usbd_config);

static const mp_rom_map_elem_t zephyr_usbd_locals_table[] = {
    { MP_ROM_QSTR(MP_QSTR_config), MP_ROM_PTR(&zephyr_usbd_config_obj) },
    { MP_ROM_QSTR(MP_QSTR_active), MP_ROM_PTR(&zephyr_usbd_active_obj) },
};
static MP_DEFINE_CONST_DICT(zephyr_usbd_locals_dict, zephyr_usbd_locals_table);

MP_DEFINE_CONST_OBJ_TYPE(
    zephyr_usbd_type,
    MP_QSTR_USBDevice,
    MP_TYPE_FLAG_NONE,
    make_new, zephyr_usbd_make_new,
    locals_dict, &zephyr_usbd_locals_dict
    );

MP_REGISTER_ROOT_POINTER(mp_obj_t usbd);

#endif // CONFIG_USB_DEVICE_STACK_NEXT
