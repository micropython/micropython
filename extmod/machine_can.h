#ifndef MICROPY_INCLUDED_EXTMOD_MACHINE_CAN_H
#define MICROPY_INCLUDED_EXTMOD_MACHINE_CAN_H

#include "py/obj.h"

// machine.CAN support APIs that are called from port-level C code

// Return the 0-based index of the CAN peripheral based on the name or the
// (1-based) number.
//
// Raises an exception if the identifier is invalid, doesn't exist, or is reserved.
mp_uint_t machine_can_get_index(mp_obj_t identifier);

void machine_can_deinit_all(void);

#endif // MICROPY_INCLUDED_EXTMOD_MACHINE_CAN_H
