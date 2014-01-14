#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "mpqstr.h"
#include "obj.h"

typedef struct _mp_obj_int_t {
    mp_obj_base_t base;
} mp_obj_int_t;

static mp_obj_t int_make_new(mp_obj_t type_in, int n_args, const mp_obj_t *args) {
    switch (n_args) {
        case 0:
            return MP_OBJ_NEW_SMALL_INT(0);

        case 1:
            // TODO allow string as arg and parse it
            return MP_OBJ_NEW_SMALL_INT(mp_obj_get_int(args[0]));

        case 2:
            // TODO make args[0] and args[1] correct
            return MP_OBJ_NEW_SMALL_INT(mp_obj_get_int_base(args[0], args[1]));

        default:
            nlr_jump(mp_obj_new_exception_msg_1_arg(MP_QSTR_TypeError, "int takes at most 2 arguments, %d given", (void*)(machine_int_t)n_args));
    }
}

const mp_obj_type_t int_type = {
    { &mp_const_type },
    "int",
    .make_new = int_make_new,
};

mp_obj_t mp_obj_new_int(machine_int_t value) {
    return MP_OBJ_NEW_SMALL_INT(value);
}
