/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 NED KONZ <NED@METAMAGIX.TECH>
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

// This file is never compiled standalone, it's included directly from
// extmod/machine_adc.c via MICROPY_PY_MACHINE_ADC_INCLUDEFILE.

/*
 * The ADC peripheral and pinmux is configured in the board's .dts file. Make
 * sure that the ADC is enabled (status = "okay";).
 *
 * In addition to that, this driver requires an ADC channel specified in the
 * io-channels property of the zephyr,user node. This is usually done with a
 * devicetree overlay.
 *
 * Configuration of channels (settings like gain, reference, or acquisition time)
 * also needs to be specified in devicetree, in ADC controller child nodes. Also
 * the ADC resolution and oversampling setting (if used) need to be specified
 * there.
 *
 * Here is an excerpt from a devicetree overlay that configures an ADC
 * with one channel that would be referred to as ('adc', 0) in the constructor
 * of the ADC object:
 *
 *  / {
 *      zephyr,user {
 *          io-channels = <&adc 0>;
 *      };
 *  };
 *
 *  &adc {
 *      #address-cells = <1>;
 *      #size-cells = <0>;
 *
 *      channel@0 {
 *          reg = <0>;
 *          zephyr,gain = "ADC_GAIN_1_6";
 *          zephyr,reference = "ADC_REF_INTERNAL";
 *          zephyr,acquisition-time = <ADC_ACQ_TIME_DEFAULT>;
 *          zephyr,input-positive = <NRF_SAADC_AIN0>;
 *          zephyr,resolution = <12>;
 *      };
 *      // other channels here (1, 4, 5, 7)
 *  };
 *
 */

#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>
#include <zephyr/drivers/adc.h>
#include "py/mphal.h"
#include "py/mperrno.h"
#include "zephyr_device.h"

static uint16_t sample_buffer;

static struct adc_sequence adc_sequence = {
    .options = NULL, // No options set by default
    .channels = 0,
    .buffer = &sample_buffer,
    .buffer_size = sizeof(sample_buffer),   // bytes, not samples
    .resolution = 12, // Default resolution, can be changed later
    .oversampling = 0, // Default oversampling, can be changed later
    .calibrate = false, // Default to no calibration
};

#define USER_NODE DT_PATH(zephyr_user)

#define DT_SPEC_AND_COMMA(node_id, prop, idx) \
    ADC_DT_SPEC_GET_BY_IDX(node_id, idx),

// Data of ADC io-channels specified in devicetree.
static const struct adc_dt_spec adc_channels[] = {
    // We require that the user node has an io-channels property.
    #if DT_NODE_HAS_PROP(USER_NODE, io_channels)
    DT_FOREACH_PROP_ELEM(USER_NODE, io_channels,
        DT_SPEC_AND_COMMA)
    #endif
};

#define NUM_CHANNELS ARRAY_SIZE(adc_channels)

typedef struct _machine_adc_obj_t {
    mp_obj_base_t base;
    char const *name;   // name given by the user in the tuple
    const struct adc_dt_spec *spec; // ADC device channel specification
} machine_adc_obj_t;


static void mp_machine_adc_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_adc_obj_t *self = MP_OBJ_TO_PTR(self_in);

    mp_printf(print, "ADC(%s.%d)", self->name, self->spec->channel_id);
}

// constructor((adcblock, channel) | adc_obj, ...)
static mp_obj_t mp_machine_adc_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    machine_adc_obj_t *self = NULL;
    if (mp_obj_is_type(args[0], &machine_adc_type)) {
        // Already an ADC object, just return it
        self = MP_OBJ_TO_PTR(args[0]);
    } else if (mp_obj_is_type(args[0], &mp_type_tuple)) {
        // Get the wanted (adcblock, channel) values.
        mp_obj_t *items;
        mp_obj_get_array_fixed_n(args[0], 2, &items);
        const char *dev_name = mp_obj_str_get_str(items[0]);
        int channel_id = mp_obj_get_int(items[1]);
        const struct device *wanted_port = zephyr_device_find(items[0]);

        // Find the desired channel by device name and channel ID
        struct adc_dt_spec const *wanted_adc_channel = NULL;

        for (size_t i = 0U; i < ARRAY_SIZE(adc_channels); i++) {
            if (adc_channels[i].dev == wanted_port &&
                adc_channels[i].channel_id == channel_id) {
                wanted_adc_channel = adc_channels + i;
                break;
            }
        }

        if (!wanted_adc_channel) {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("ADC channel (%s, %d) not found"), dev_name, channel_id);
        }

        if (!adc_is_ready_dt(wanted_adc_channel)) {
            mp_raise_OSError(MP_EIO);
        }

        int err = adc_channel_setup_dt(wanted_adc_channel);
        if (err < 0) {
            mp_raise_OSError(MP_EINVAL);
        }

        self = mp_obj_malloc(machine_adc_obj_t, &machine_adc_type);
        self->spec = wanted_adc_channel;
        self->name = dev_name;
    } else {
        // Unknown type, raise a ValueError
        mp_raise_ValueError(MP_ERROR_TEXT("ADC must be initialized with a tuple of (adcblock, channel) or an ADC object"));
    }

    return MP_OBJ_FROM_PTR(self);
}

static mp_uint_t zephyr_adc_read(const struct adc_dt_spec *spec) {
    int err = adc_sequence_init_dt(spec, &adc_sequence);
    if (err < 0) {
        mp_raise_OSError(MP_EOPNOTSUPP);
    }

    err = adc_read_dt(spec, &adc_sequence);
    if (err < 0) {
        mp_raise_OSError(MP_EIO);
    }

    return (mp_uint_t)sample_buffer; // Return the read value as a 16-bit integer
}

static mp_int_t mp_machine_adc_read_u16(machine_adc_obj_t *self) {
    mp_uint_t raw = zephyr_adc_read(self->spec);
    // Scale raw reading to 16 bit value using a Taylor expansion (for 8 <= bits <= 16)
    mp_int_t bits = self->spec->resolution;
    mp_uint_t u16 = raw << (16 - bits) | raw >> (2 * bits - 16);
    return u16;
}


#if MICROPY_PY_MACHINE_ADC_READ_UV
static mp_int_t mp_machine_adc_read_uv(machine_adc_obj_t *self) {
    int32_t raw = (int32_t)zephyr_adc_read(self->spec);
    int err = adc_raw_to_millivolts_dt(self->spec, &raw);
    if (err < 0) {
        mp_raise_OSError(MP_EOPNOTSUPP);
    }
    return (mp_int_t)raw * 1000UL; // Convert to microvolts
}
#endif


#if MICROPY_PY_MACHINE_ADC_READ
static mp_int_t mp_machine_adc_read(machine_adc_obj_t *self) {
    mp_uint_t raw = zephyr_adc_read(self->spec);
    return (mp_int_t)raw;
}
#endif

#define MICROPY_PY_MACHINE_ADC_CLASS_CONSTANTS
