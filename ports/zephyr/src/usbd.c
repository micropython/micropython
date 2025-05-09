/*
* This file is part of the MicroPython project, http://micropython.org/
*
* The MIT License (MIT)
*
* Copyright (c) 2024-2025 MASSDRIVER EI (massdriver.space)
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
#include <stdint.h>

#include <zephyr/device.h>
#include <zephyr/usb/usbd.h>
#include <zephyr/usb/bos.h>

#if defined(CONFIG_USB_DEVICE_STACK_NEXT)

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(mp_usbd);

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
USBD_DESC_PRODUCT_DEFINE(mp_product, "Micropython on Zephyr RTOS");
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

static void mp_fix_code_triple(struct usbd_context *uds_ctx,
    const enum usbd_speed speed) {
    /* Always use class code information from Interface Descriptors */
    if (IS_ENABLED(CONFIG_USBD_CDC_ACM_CLASS) ||
        IS_ENABLED(CONFIG_USBD_CDC_ECM_CLASS) ||
        IS_ENABLED(CONFIG_USBD_CDC_NCM_CLASS) ||
        IS_ENABLED(CONFIG_USBD_AUDIO2_CLASS)) {
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

struct usbd_context *mp_usbd_init_device(usbd_msg_cb_t msg_cb) {
    int err;

    err = usbd_add_descriptor(&mp_usbd, &mp_lang);
    if (err) {
        LOG_ERR("Failed to initialize language descriptor (%d)", err);
        return NULL;
    }

    err = usbd_add_descriptor(&mp_usbd, &mp_mfr);
    if (err) {
        LOG_ERR("Failed to initialize manufacturer descriptor (%d)", err);
        return NULL;
    }

    err = usbd_add_descriptor(&mp_usbd, &mp_product);
    if (err) {
        LOG_ERR("Failed to initialize product descriptor (%d)", err);
        return NULL;
    }

    err = usbd_add_descriptor(&mp_usbd, &mp_sn);
    if (err) {
        LOG_ERR("Failed to initialize SN descriptor (%d)", err);
        return NULL;
    }

    if (usbd_caps_speed(&mp_usbd) == USBD_SPEED_HS) {
        err = usbd_add_configuration(&mp_usbd, USBD_SPEED_HS,
            &mp_hs_config);
        if (err) {
            LOG_ERR("Failed to add High-Speed configuration");
            return NULL;
        }

        err = usbd_register_all_classes(&mp_usbd, USBD_SPEED_HS, 1, blocklist);
        if (err) {
            LOG_ERR("Failed to add register classes");
            return NULL;
        }

        mp_fix_code_triple(&mp_usbd, USBD_SPEED_HS);
    }

    err = usbd_add_configuration(&mp_usbd, USBD_SPEED_FS,
        &mp_fs_config);
    if (err) {
        LOG_ERR("Failed to add Full-Speed configuration");
        return NULL;
    }

    err = usbd_register_all_classes(&mp_usbd, USBD_SPEED_FS, 1, blocklist);
    if (err) {
        LOG_ERR("Failed to add register classes");
        return NULL;
    }

    mp_fix_code_triple(&mp_usbd, USBD_SPEED_FS);

    if (msg_cb != NULL) {
        err = usbd_msg_register_cb(&mp_usbd, msg_cb);
        if (err) {
            LOG_ERR("Failed to register message callback");
            return NULL;
        }
    }

    err = usbd_init(&mp_usbd);
    if (err) {
        LOG_ERR("Failed to initialize device support");
        return NULL;
    }

    return &mp_usbd;
}

static struct usbd_context *mp_usbd_context;

int mp_usbd_init(void) {
    int err;

    mp_usbd_context = mp_usbd_init_device(NULL);
    if (mp_usbd_context == NULL) {
        return -ENODEV;
    }

    err = usbd_enable(mp_usbd_context);
    if (err) {
        return err;
    }

    return 0;
}

#endif // defined(CONFIG_USB_DEVICE_STACK_NEXT)
