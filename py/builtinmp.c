#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>

#include "misc.h"
#include "mpconfig.h"
#include "obj.h"
#include "builtin.h"

// Various builtins specific to MicroPython runtime,
// living in micropython module

#if MICROPY_MEM_STATS
static mp_obj_t mem_total() {
    return MP_OBJ_NEW_SMALL_INT(m_get_total_bytes_allocated());
}

static mp_obj_t mem_current() {
    return MP_OBJ_NEW_SMALL_INT(m_get_current_bytes_allocated());
}

static mp_obj_t mem_peak() {
    return MP_OBJ_NEW_SMALL_INT(m_get_peak_bytes_allocated());
}

MP_DEFINE_CONST_FUN_OBJ_0(mp_builtin_mem_total_obj, mem_total);
MP_DEFINE_CONST_FUN_OBJ_0(mp_builtin_mem_current_obj, mem_current);
MP_DEFINE_CONST_FUN_OBJ_0(mp_builtin_mem_peak_obj, mem_peak);
#endif
