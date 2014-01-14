#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "mpqstr.h"
#include "obj.h"
#include "objint.h"

// This dispatcher function is expected to be independent of the implementation
// of long int
static mp_obj_t int_make_new(mp_obj_t type_in, int n_args, const mp_obj_t *args) {
    switch (n_args) {
        case 0:
            return MP_OBJ_NEW_SMALL_INT(0);

        case 1:
            // TODO allow string as arg and parse it
            return mp_obj_new_int(mp_obj_get_int(args[0]));

        case 2:
            // TODO make args[0] and args[1] correct
            return MP_OBJ_NEW_SMALL_INT(mp_obj_get_int_base(args[1], args[0]));

        default:
            nlr_jump(mp_obj_new_exception_msg_1_arg(MP_QSTR_TypeError, "int takes at most 2 arguments, %d given", (void*)(machine_int_t)n_args));
    }
}

const mp_obj_type_t int_type = {
    { &mp_const_type },
    "int",
    .print = int_print,
    .make_new = int_make_new,
    .binary_op = int_binary_op,
};

#if MICROPY_LONGINT_IMPL == MICROPY_LONGINT_IMPL_NONE
// This is called only for non-SMALL_INT
void int_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in) {
}

// This is called only for non-SMALL_INT
mp_obj_t int_binary_op(int op, mp_obj_t lhs_in, mp_obj_t rhs_in) {
    assert(0);
    return mp_const_none;
}

// This is called only with strings whose value doesn't fit in SMALL_INT
mp_obj_t mp_obj_new_int_from_long_str(const char *s) {
    assert(0);
    return mp_const_none;
}

mp_obj_t mp_obj_new_int_from_uint(machine_uint_t value) {
    // SMALL_INT accepts only signed numbers, of one bit less size
    // then word size, which totals 2 bits less for unsigned numbers.
    if ((value & (WORD_MSBIT_HIGH | (WORD_MSBIT_HIGH >> 1))) == 0) {
        return MP_OBJ_NEW_SMALL_INT(value);
    }
    // TODO: Raise exception
    assert(0);
    return mp_const_none;
}

mp_obj_t mp_obj_new_int(machine_int_t value) {
    if (MP_OBJ_FITS_SMALL_INT(value)) {
        return MP_OBJ_NEW_SMALL_INT(value);
    }
    // TODO: Raise exception
    assert(0);
    return mp_const_none;
}
#endif
