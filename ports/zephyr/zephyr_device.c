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
#include "extmod/modmachine.h"
#include "modmachine.h"

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

    #if defined(CONFIG_MICROPY_DYNAMIC_PINCTRL)
    const mp_zephyr_device_data_t *device;

    if (dev == NULL) {
        device = device_find_by_node_label(dev_name);
        if (device == NULL) {
            device = device_find_by_name(dev_name);
        }
        if (device != NULL) {
            if (device->dev == NULL || device->pinctrl_cfg == NULL) {
                #if MICROPY_ERROR_REPORTING <= MICROPY_ERROR_REPORTING_TERSE
                mp_raise_ValueError(MP_ERROR_TEXT("device is invalid"));
                #else
                mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("device %s is invalid"), dev_name);
                #endif
            }
            dev = device->dev;
        }
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

const mp_zephyr_device_data_t *zephyr_device_find_data_name(mp_obj_t name) {
    const char *dev_name = mp_obj_str_get_str(name);
    const mp_zephyr_device_data_t *device;

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

    return device;
}

const mp_zephyr_device_data_t *zephyr_device_find_data_dev(const struct device *dev) {
    for (int i = 0; i < ARRAY_SIZE(all_devices); i++)
    {
        if (all_devices[i].dev == dev) {
            return &(all_devices[i]);
        }
    }

    mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("unexpected critical failure"));

    return 0;
}

int zephyr_device_apply_pinctrl(const mp_zephyr_device_data_t *data, size_t pin_cnt, ...) {
    pinctrl_soc_pin_t *pinctrl;
    struct pinctrl_state *pinctrl_states;
    va_list args;
    pinctrl_soc_pin_t pinctrl_buffer[32];
    int ret = 0;
    size_t real_pin_cnt = 0;

    va_start(args, pin_cnt);

    for (int i = 0; i < pin_cnt; i++) {
        mp_obj_t pin_obj = va_arg(args, mp_obj_t);
        if (pin_obj == mp_const_none) {
            continue;
        }
        if (!mp_obj_is_type(pin_obj, &machine_pin_type)) {
            mp_raise_ValueError(MP_ERROR_TEXT("argument is not a Pin"));
        }
        machine_pin_obj_t *pin = MP_OBJ_TO_PTR(pin_obj);
        if (!pin->is_pinctrl) {
            mp_raise_ValueError(MP_ERROR_TEXT("Pin is not a pinctrl Pin"));
        }
        pinctrl_buffer[real_pin_cnt] = pin->pinctrl_data.pinctrl;
        real_pin_cnt++;
    }

    va_end(args);

    if (device_is_ready(data->dev)) {
        ret = device_deinit(data->dev);
    }

    if (ret == -ENOTSUP) {
        mp_raise_msg(&mp_type_RuntimeError,
            MP_ERROR_TEXT("device is already initialized and doesn't support de-initialization"));
    }
    if (ret < 0) {
        mp_raise_OSError(-ret);
    }

    pinctrl = m_malloc(real_pin_cnt * sizeof(pinctrl_soc_pin_t));
    pinctrl_states = m_malloc(data->pinctrl_cfg->state_cnt * sizeof(struct pinctrl_state));

    for (int i = 0; i < real_pin_cnt; i++) {
        pinctrl[i] = pinctrl_buffer[i];
    }

    for (int i = 0; i < data->pinctrl_cfg->state_cnt; i++) {
        pinctrl_states[i].pins = pinctrl;
        pinctrl_states[i].pin_cnt = real_pin_cnt;
        pinctrl_states[i].id = data->pinctrl_cfg->states[i].id;
    }

    ret = pinctrl_update_states(data->pinctrl_cfg, pinctrl_states, data->pinctrl_cfg->state_cnt);
    if (ret < 0) {
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("couldn't apply pinctrl"));
    }

    ret = device_init(data->dev);
    if (ret < 0) {
        mp_raise_OSError(-ret);
    }

    return ret;
}

#endif
