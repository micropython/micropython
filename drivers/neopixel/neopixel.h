#ifndef MICROPY_INCLUDED_DRIVERS_NEOPIXEL_NEOPIXEL_H
#define MICROPY_INCLUDED_DRIVERS_NEOPIXEL_NEOPIXEL_H

#include "py/obj.h"

// To be provided by the port
uint32_t mp_hal_delay_ns_calc_neopixel(uint32_t ns, bool is_low_cycle);

MP_DECLARE_CONST_FUN_OBJ_3(neopixel_write_obj);

#endif // MICROPY_INCLUDED_DRIVERS_NEOPIXEL_NEOPIXEL_H
