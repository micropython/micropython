// ra_pin_prefix.c becomes the initial portion of the generated pins file.

#include <stdio.h>

#include "py/obj.h"
#include "py/mphal.h"
#include "pin.h"

#define PIN_AD(p_name, p_pin, ad_bit, ad_channel) \
    { \
        { &machine_pin_type }, \
        .name = MP_QSTR_##p_name, \
        .pin = p_pin, \
        .bit = ad_bit, \
        .channel = ad_channel \
    }

#define PIN(p_name, p_pin, p_ad) \
    { \
        { &machine_pin_type }, \
        .name = MP_QSTR_##p_name, \
        .pin = p_pin, \
        .ad = p_ad, \
    }
