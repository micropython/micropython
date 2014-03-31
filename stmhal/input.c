#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "readline.h"
#include "usb.h"

STATIC mp_obj_t mp_builtin_input(uint n_args, const mp_obj_t *args) {
    if (n_args == 1) {
        mp_obj_print(args[0], PRINT_REPR);
    }
    vstr_t line;
    vstr_init(&line, 16);
    int ret = readline(&line, "");
    if (line.len == 0 && ret == VCP_CHAR_CTRL_D) {
        nlr_jump(mp_obj_new_exception(&mp_type_EOFError));
    }
    mp_obj_t o = mp_obj_new_str((const byte*)line.buf, line.len, false);
    vstr_clear(&line);
    return o;
}

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_builtin_input_obj, 0, 1, mp_builtin_input);
