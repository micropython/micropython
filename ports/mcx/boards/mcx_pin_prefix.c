// mcx_pin_prefix.c becomes the initial portion of the generated pins file.

#include <stdio.h>

#include "py/obj.h"
#include "py/mphal.h"

#include "extmod/modmachine.h"

/* Struct definitions */
#include "machine_pin.h"

#define AF(p_af_id, p_af_type, p_af_inst, p_af_attr) \
    { \
        .af_id = p_af_id,   \
        .af_type = MACHINE_PIN_AF_TYPE_##p_af_type, \
        .af_instance_id = p_af_inst, \
        .af_attribute = MACHINE_PIN_AF_ATTR_##p_af_attr, \
    }

#define ADC(p_adc_id, p_adc_side, p_adc_channel) \
    { \
        .adc_id = p_adc_id,      \
        .adc_side = p_adc_side,    \
        .adc_channel = p_adc_channel, \
    }

#define PIN(p_name, p_port, p_pin, p_af, p_adc) \
    { \
        { &machine_pin_type }, \
        .name = MP_QSTR_##p_name, \
        .port = p_port,    \
        .pin = p_pin,     \
        .af_count = sizeof(p_af) / sizeof(p_af[0]), \
        .af = p_af,      \
        .adc = p_adc,     \
    }
