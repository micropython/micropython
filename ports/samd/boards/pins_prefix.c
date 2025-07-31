// pins_prefix.c becomes the initial portion of the generated pins file.

#include <stdio.h>

#include "py/obj.h"
#include "py/mphal.h"
#include "extmod/modmachine.h"
#include "pin_af.h"
#include "genhdr/pins.h"

#if defined(MCU_SAMD21)

#define PIN(p_name, p_eic, p_adc0, p_sercom1, p_sercom2, p_tcc1, p_tcc2) \
    {{&machine_pin_type}, PIN_##p_name, MP_QSTR_##p_name, p_eic, p_adc0, p_sercom1, p_sercom2, p_tcc1, p_tcc2 }

#elif defined(MCU_SAMD51)

#define PIN(p_name, p_eic, p_adc0, p_adc1, p_sercom1, p_sercom2, p_tc, p_tcc1, p_tcc2) \
    {{&machine_pin_type}, PIN_##p_name, MP_QSTR_##p_name, p_eic, p_adc0, p_adc1, p_sercom1, p_sercom2, p_tc, p_tcc1, p_tcc2 }

#endif
