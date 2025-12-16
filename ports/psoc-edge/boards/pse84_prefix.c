#include "py/obj.h"
#include "py/mphal.h"
#include "machine_pin.h"

#define PIN(p_port, p_pin) \
    { \
        { &machine_pin_type }, \
        .name = MP_QSTR_P##p_port##_##p_pin, \
        .port = (p_port), \
        .pin = (p_pin), \
    }
