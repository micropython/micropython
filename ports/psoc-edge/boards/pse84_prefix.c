#include "py/obj.h"
#include "py/mphal.h"
#include "machine_pin.h"
#include "machine_pin_af.h"
#include "cy_device_headers_ns.h"

#define AF(af_name_idx, af_fn, af_unit, af_signal, af_ptr) \
    { \
        .idx = HSIOM_SEL_##af_name_idx, \
        .fn = MACHINE_PIN_AF_FN_##af_fn, \
        .unit = (af_unit), \
        .signal = MACHINE_PIN_AF_SIGNAL_##af_fn##_##af_signal, \
        .periph = (af_ptr) \
    }

#define PIN(p_port, p_pin, p_af) \
    { \
        { &machine_pin_type }, \
        .name = MP_QSTR_P##p_port##_##p_pin, \
        .port = (p_port), \
        .pin = (p_pin), \
        .af_num = (sizeof(p_af) / sizeof(machine_pin_af_obj_t)), \
        .af = (p_af) \
    }
