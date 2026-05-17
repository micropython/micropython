// Include the main ADC driver, so mboot can use adc_config() and adc_config_and_read_u16().
#include "py/obj.h"
#if MICROPY_PY_MACHINE_ADC
#include "../machine_adc.c"
#endif
