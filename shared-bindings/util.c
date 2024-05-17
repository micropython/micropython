// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/runtime.h"

#include "shared-bindings/util.h"

// If so, deinit() has already been called on the object, so complain.
void raise_deinited_error(void) {
    mp_raise_ValueError(MP_ERROR_TEXT("Object has been deinitialized and can no longer be used. Create a new object."));
}

void properties_print_helper(const mp_print_t *print, mp_obj_t self_in, const mp_arg_t *properties, size_t n_properties) {
    const mp_obj_type_t *type = mp_obj_get_type(self_in);
    mp_printf(print, "%q(", type->name);
    for (size_t i = 0; i < n_properties; i++) {
        if (i > 0) {
            mp_print_str(print, ", ");
        }
        mp_printf(print, "%q=", properties[i].qst);
        mp_obj_print_helper(print, mp_load_attr(self_in, properties[i].qst), PRINT_REPR);
    }
    mp_print_str(print, ")");
}

void properties_construct_helper(mp_obj_t self_in, const mp_arg_t *args, const mp_arg_val_t *vals, size_t n_properties) {
    for (size_t i = 0; i < n_properties; i++) {
        if (vals[i].u_obj != NULL) {
            mp_store_attr(self_in, args[i].qst, vals[i].u_obj);
        }
    }
}
