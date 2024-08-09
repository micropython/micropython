#include "pico/stdlib.h"

#ifndef MICROPY_INCLUDED_RP2_MACHINE_PSRAM_H
#define MICROPY_INCLUDED_RP2_MACHINE_PSRAM_H

#define PSRAM_LOCATION _u(0x11000000)

extern void psram_set_qmi_timing();
extern size_t psram_init(uint cs_pin);

#endif
