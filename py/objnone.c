#include <stdlib.h>
#include <stdint.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "obj.h"

typedef struct _mp_obj_none_t {
    mp_obj_base_t base;
} mp_obj_none_t;

void none_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in) {
    print(env, "None");
}

const mp_obj_type_t none_type = {
    { &mp_const_type },
    "NoneType",
    .print = none_print,
    .methods = {{NULL, NULL},},
};

static const mp_obj_none_t none_obj = {{&none_type}};
const mp_obj_t mp_const_none = (mp_obj_t)&none_obj;

// the stop-iteration object just needs to be something unique
// it's not the StopIteration exception
static const mp_obj_none_t stop_it_obj = {{&none_type}};
const mp_obj_t mp_const_stop_iteration = (mp_obj_t)&stop_it_obj;
