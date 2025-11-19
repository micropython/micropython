/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 Analog Devices, Inc.
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

#include "zephyr_device.h"
#include "py/runtime.h"

#if defined(CONFIG_MICROPY_DYNAMIC_PINCTRL)

#include <zephyr/drivers/pinctrl.h>
extern pinctrl_soc_pin_t zephyr_pinctrl_get_pinctrl(const mp_obj_t obj);

/* Circumvent limitation of only having devices that are initialized */
typedef struct mp_zephyr_device_data_t_ {
    const struct device *dev;
    char *name;
    const char **labels;
    size_t label_cnt;
    struct pinctrl_dev_config *pinctrl_cfg;
} mp_zephyr_device_data_t;

#define PINCTRL_EXISTS_X_OR_Y(node, to_do, to_or) COND_CODE_1(DT_CAT(node, _PINCTRL_IDX_0_EXISTS), \
    (to_do), (to_or))

#define DT_NODELABEL_STRING_ARRAY_DECLARE(node) static const char *_##node##_labels[] = \
    DT_NODELABEL_STRING_ARRAY(node)

#define DT_NODELABEL_STRING_ARRAY_DECLARE_PINCTRL(node) \
    PINCTRL_EXISTS_X_OR_Y(node, DT_NODELABEL_STRING_ARRAY_DECLARE(node); , )

DT_FOREACH_STATUS_OKAY_NODE(DT_NODELABEL_STRING_ARRAY_DECLARE_PINCTRL)

#define DT_NODELABEL_STRING_ARRAY_NAME(node) _##node##_labels

#define PINCTRL_DT_DEV_CONFIG_ENTRY(node) \
    PINCTRL_EXISTS_X_OR_Y(node, PINCTRL_DT_DEV_CONFIG_GET(node), NULL)

#define PINCTRL_DT_DATA_DECLARE(node) \
    PINCTRL_EXISTS_X_OR_Y(node, PINCTRL_DT_DEV_CONFIG_DECLARE(node); , )

DT_FOREACH_STATUS_OKAY_NODE(PINCTRL_DT_DATA_DECLARE)

#define DEVICE_DATA_ENTRY(node) {                        \
        .dev = DEVICE_DT_GET(node),                      \
        .name = DEVICE_DT_NAME(node),                    \
        .labels = DT_NODELABEL_STRING_ARRAY_NAME(node),  \
        .label_cnt = DT_CAT(node, _NODELABEL_NUM),       \
        .pinctrl_cfg = PINCTRL_DT_DEV_CONFIG_ENTRY(node) \
},

#define DEVICE_DATA_ENTRY_PINCTRL(node) \
    PINCTRL_EXISTS_X_OR_Y(node, DEVICE_DATA_ENTRY(node), )

static const mp_zephyr_device_data_t all_devices[] =
{
    DT_FOREACH_STATUS_OKAY_NODE(DEVICE_DATA_ENTRY_PINCTRL)
};

static const mp_zephyr_device_data_t *device_find_by_node_label(const char *name) {
    for (int i = 0; i < ARRAY_SIZE(all_devices); i++)
    {
        for (int j = 0; j < all_devices[i].label_cnt; j++) {
            if (strcmp(name, all_devices[i].labels[j]) == 0) {
                return &(all_devices[i]);
            }
        }
    }
    return NULL;
}

static const mp_zephyr_device_data_t *device_find_by_name(const char *name) {
    for (int i = 0; i < ARRAY_SIZE(all_devices); i++)
    {
        if (strcmp(name, all_devices[i].name) == 0) {
            return &(all_devices[i]);
        }
    }
    return NULL;
}

#endif

const struct device *zephyr_device_find(mp_obj_t name) {
    const char *dev_name = mp_obj_str_get_str(name);
    const struct device *dev = device_get_binding(dev_name);

    #ifdef CONFIG_DEVICE_DT_METADATA
    if (dev == NULL) {
        dev = device_get_by_dt_nodelabel(dev_name);
    }
    #endif

    if (dev == NULL) {
        #if MICROPY_ERROR_REPORTING <= MICROPY_ERROR_REPORTING_TERSE
        mp_raise_ValueError(MP_ERROR_TEXT("device not found"));
        #else
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("device %s not found"), dev_name);
        #endif
    }

    return dev;
}

#if defined(CONFIG_MICROPY_DYNAMIC_PINCTRL)

const struct device *zephyr_device_setup(mp_obj_t obj) {
    mp_obj_t *args;
    mp_obj_t *pinctrls;
    const mp_zephyr_device_data_t *device;
    size_t pinctrl_count;
    const char *dev_name;
    pinctrl_soc_pin_t *pinctrl;
    struct pinctrl_state *pinctrl_states;
    int ret = 0;

    if (mp_obj_is_str(obj)) {
        return zephyr_device_find(obj);
    }
    if (mp_obj_get_int(mp_obj_len(obj)) != 2) {
        mp_raise_ValueError(MP_ERROR_TEXT("Invalid device object"));
    }

    mp_obj_get_array_fixed_n(obj, 2, &args);

    dev_name = mp_obj_str_get_str(args[0]);
    device = device_find_by_node_label(dev_name);

    if (device == NULL) {
        device = device_find_by_name(dev_name);
    }

    if (device == NULL) {
        #if MICROPY_ERROR_REPORTING <= MICROPY_ERROR_REPORTING_TERSE
        mp_raise_ValueError(MP_ERROR_TEXT("device not found"));
        #else
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("device %s not found"), dev_name);
        #endif
    } else if (device->dev == NULL || device->pinctrl_cfg == NULL) {
        #if MICROPY_ERROR_REPORTING <= MICROPY_ERROR_REPORTING_TERSE
        mp_raise_ValueError(MP_ERROR_TEXT("device is invalid"));
        #else
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("device %s is invalid"), dev_name);
        #endif
    }

    if (device_is_ready(device->dev)) {
        ret = device_deinit(device->dev);
    }

    if (ret == -ENOTSUP) {
        mp_raise_msg(&mp_type_RuntimeError,
            MP_ERROR_TEXT("Device is already initialized and doesn't support de-initialization"));
    }
    if (ret < 0) {
        mp_raise_OSError(-ret);
    }

    if (mp_obj_get_int(mp_obj_len(args[1])) < 1) {
        mp_raise_ValueError(MP_ERROR_TEXT("No Pinctrl objects"));
    }
    mp_obj_get_array(args[1], &pinctrl_count, &pinctrls);

    /* should get cleaned by gc properly when pinctrl dont need it anymore? Otherwise, leaks */
    pinctrl = m_malloc(pinctrl_count * sizeof(pinctrl_soc_pin_t));
    pinctrl_states = m_malloc(device->pinctrl_cfg->state_cnt * sizeof(struct pinctrl_state));
    for (int i = 0; i < pinctrl_count; i++) {
        pinctrl[i] = zephyr_pinctrl_get_pinctrl(pinctrls[i]);
    }
    for (int i = 0; i < device->pinctrl_cfg->state_cnt; i++) {
        pinctrl_states[i].pins = pinctrl;
        pinctrl_states[i].pin_cnt = pinctrl_count;
        pinctrl_states[i].id = device->pinctrl_cfg->states[i].id;
    }
    ret = pinctrl_update_states(device->pinctrl_cfg, pinctrl_states, device->pinctrl_cfg->state_cnt);
    if (ret < 0) {
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("Couldn't apply pinctrl"));
    }

    ret = device_init(device->dev);
    if (ret < 0) {
        mp_raise_OSError(-ret);
    }

    return device->dev;
}

#else

const struct device *zephyr_device_setup(mp_obj_t obj) {
    return zephyr_device_find(obj);
}

#endif
