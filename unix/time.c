#include <string.h>
#include <time.h>

#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "runtime.h"

static mp_obj_t mod_time_time() {
    return mp_obj_new_int((machine_int_t)time(NULL));
}
static MP_DEFINE_CONST_FUN_OBJ_0(mod_time_time_obj, mod_time_time);

// Note: this is deprecated since CPy3.3, but pystone still uses it.
static mp_obj_t mod_time_clock() {
    return mp_obj_new_int((machine_int_t)clock());
}
static MP_DEFINE_CONST_FUN_OBJ_0(mod_time_clock_obj, mod_time_clock);

void time_init() {
    mp_obj_t m = mp_obj_new_module(QSTR_FROM_STR_STATIC("time"));
    rt_store_attr(m, QSTR_FROM_STR_STATIC("time"), (mp_obj_t)&mod_time_time_obj);
    rt_store_attr(m, QSTR_FROM_STR_STATIC("clock"), (mp_obj_t)&mod_time_clock_obj);
}
