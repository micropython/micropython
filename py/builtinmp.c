#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>

#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "runtime.h"
#include "builtin.h"

// Various builtins specific to MicroPython runtime,
// living in micropython module

#if MICROPY_MEM_STATS
static mp_obj_t mem_total() {
    return MP_OBJ_NEW_SMALL_INT((machine_int_t)m_get_total_bytes_allocated());
}

static mp_obj_t mem_current() {
    return MP_OBJ_NEW_SMALL_INT((machine_int_t)m_get_current_bytes_allocated());
}

static mp_obj_t mem_peak() {
    return MP_OBJ_NEW_SMALL_INT((machine_int_t)m_get_peak_bytes_allocated());
}

MP_DEFINE_CONST_FUN_OBJ_0(mp_builtin_mem_total_obj, mem_total);
MP_DEFINE_CONST_FUN_OBJ_0(mp_builtin_mem_current_obj, mem_current);
MP_DEFINE_CONST_FUN_OBJ_0(mp_builtin_mem_peak_obj, mem_peak);
#endif

void mp_module_micropython_init(void) {
    mp_obj_t m_mp = mp_obj_new_module(MP_QSTR_micropython);
    rt_store_name(MP_QSTR_micropython, m_mp);

#if MICROPY_MEM_STATS
    rt_store_attr(m_mp, QSTR_FROM_STR_STATIC("mem_total"), (mp_obj_t)&mp_builtin_mem_total_obj);
    rt_store_attr(m_mp, QSTR_FROM_STR_STATIC("mem_current"), (mp_obj_t)&mp_builtin_mem_current_obj);
    rt_store_attr(m_mp, QSTR_FROM_STR_STATIC("mem_peak"), (mp_obj_t)&mp_builtin_mem_peak_obj);
#endif
}
