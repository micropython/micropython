#define MICROPY_ENABLE_DYNRUNTIME (1)
#define MICROPY_PY_URANDOM (1)
#define MICROPY_PY_URANDOM_EXTRA_FUNCS (1)

#include "py/dynruntime.h"

// Dynamic native modules don't support a data section so these must go in the BSS
uint32_t yasmarang_pad, yasmarang_n, yasmarang_d;
uint8_t yasmarang_dat;

#include "extmod/modurandom.c"

mp_obj_t mpy_init(mp_obj_fun_bc_t *self, size_t n_args, size_t n_kw, mp_obj_t *args) {
    MP_DYNRUNTIME_INIT_ENTRY

    yasmarang_pad = 0xeda4baba;
    yasmarang_n = 69;
    yasmarang_d = 233;

    mp_store_global(MP_QSTR___name__, MP_OBJ_NEW_QSTR(MP_QSTR_urandom));
    mp_store_global(MP_QSTR_getrandbits, MP_OBJ_FROM_PTR(&mod_urandom_getrandbits_obj));
    mp_store_global(MP_QSTR_seed, MP_OBJ_FROM_PTR(&mod_urandom_seed_obj));
    #if MICROPY_PY_URANDOM_EXTRA_FUNCS
    mp_store_global(MP_QSTR_randrange, MP_OBJ_FROM_PTR(&mod_urandom_randrange_obj));
    mp_store_global(MP_QSTR_randint, MP_OBJ_FROM_PTR(&mod_urandom_randint_obj));
    mp_store_global(MP_QSTR_choice, MP_OBJ_FROM_PTR(&mod_urandom_choice_obj));
    #if MICROPY_PY_BUILTINS_FLOAT
    mp_store_global(MP_QSTR_random, MP_OBJ_FROM_PTR(&mod_urandom_random_obj));
    mp_store_global(MP_QSTR_uniform, MP_OBJ_FROM_PTR(&mod_urandom_uniform_obj));
    #endif
    #endif

    MP_DYNRUNTIME_INIT_EXIT
}
