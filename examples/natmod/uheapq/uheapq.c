#define MICROPY_PY_UHEAPQ (1)

#include "py/dynruntime.h"

#include "extmod/moduheapq.c"

mp_obj_t mpy_init(mp_obj_fun_bc_t *self, size_t n_args, size_t n_kw, mp_obj_t *args) {
    MP_DYNRUNTIME_INIT_ENTRY

    mp_store_global(MP_QSTR___name__, MP_OBJ_NEW_QSTR(MP_QSTR_uheapq));
    mp_store_global(MP_QSTR_heappush, MP_OBJ_FROM_PTR(&mod_uheapq_heappush_obj));
    mp_store_global(MP_QSTR_heappop, MP_OBJ_FROM_PTR(&mod_uheapq_heappop_obj));
    mp_store_global(MP_QSTR_heapify, MP_OBJ_FROM_PTR(&mod_uheapq_heapify_obj));

    MP_DYNRUNTIME_INIT_EXIT
}
