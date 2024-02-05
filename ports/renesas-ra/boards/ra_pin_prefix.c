// ra_pin_prefix.c becomes the initial portion of the generated pins file.

#include <stdio.h>

#include "py/obj.h"
#include "py/mphal.h"
#include "extmod/modmachine.h"
#include "pin.h"

#define PIN_ADC(p_name, p_pin, adc_bits, adc_channel) \
    { \
        { &machine_pin_type }, \
        .name = MP_QSTR_##p_name, \
        .pin = p_pin, \
        .bit = adc_bits, \
        .channel = adc_channel \
    }

#define PIN(p_name, p_pin, p_adc) \
    { \
        { &machine_pin_type }, \
        .name = MP_QSTR_##p_name, \
        .pin = p_pin, \
        .ad = p_adc, \
    }
