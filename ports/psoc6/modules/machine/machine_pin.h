#ifndef MICROPY_INCLUDED_MACHINE_PIN_H
#define MICROPY_INCLUDED_MACHINE_PIN_H

#include <stdint.h>
#include "machine_pin_phy.h"

// Function Prototypes to support interaction between c<->py
int pin_find(mp_obj_t obj);
int pin_addr_by_name(mp_obj_t obj);

#endif // MICROPY_INCLUDED_MACHINE_PIN_H
