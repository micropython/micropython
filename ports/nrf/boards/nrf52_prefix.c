// nrf52_prefix.c becomes the initial portion of the generated pins file.

#include <stdio.h>

#include "py/obj.h"
#include "py/mphal.h"
#include "nrf_pin.h"

#define PIN(p_name, p_port, p_pin, p_adc_channel) \
    { \
        { &mcu_pin_type }, \
        .name = MP_QSTR_##p_name, \
        .port = (p_port), \
        .pin = (p_pin), \
        .adc_channel = (p_adc_channel), \
    }
