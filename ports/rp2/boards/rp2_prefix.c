#include <stdint.h>
#include "py/obj.h"
#include "py/mphal.h"
#include "machine_pin.h"

#define AF(af_idx, af_fn, af_unit)                      \
    {                                                   \
        .base = { &machine_pin_af_type },               \
        .name = MP_QSTR_##af_fn,                        \
        .idx = (af_idx),                                \
        .fn = GPIO_FUNC_##af_fn,                        \
        .unit = (af_unit),                              \
    }

#if MICROPY_HW_PIN_EXT_COUNT
#define PIN(_id, _name, _is_ext, _af_num, _af_list)     \
    {                                                   \
        .base = { &machine_pin_type },                  \
        .name = MP_QSTR_##_name,                        \
        .id = (uint8_t)(_id),                           \
        .is_ext = (_is_ext),                            \
        .is_output = false,                             \
        .last_output_value = 0,                         \
        .af_num = (_af_num),                            \
        .af = (_af_list),                               \
    }
#else
#define PIN(_id, _name, _is_ext, _af_num, _af_list)     \
    {                                                   \
        .base = { &machine_pin_type },                  \
        .name = MP_QSTR_##_name,                        \
        .id = (uint8_t)(_id),                           \
        .af_num = (_af_num),                            \
        .af = (_af_list),                               \
    }
#endif
