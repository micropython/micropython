#ifndef MICROPY_INCLUDED_SHARED_MODULE_VECTORIO_INIT_H
#define MICROPY_INCLUDED_SHARED_MODULE_VECTORIO_INIT_H

#include "py/obj.h"

typedef void event_function(mp_obj_t obj);

typedef struct {
    mp_obj_t obj;
    event_function *event;
} vectorio_event_t;


#endif
