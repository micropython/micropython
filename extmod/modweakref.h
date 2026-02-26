#ifndef MICROPY_INCLUDED_EXTMOD_MODWEAKREF_H
#define MICROPY_INCLUDED_EXTMOD_MODWEAKREF_H

#include "py/obj.h"

struct _mp_obj_weakref_ref_t;
typedef struct _mp_obj_weakref_ref_t mp_obj_weakref_ref_t;

mp_obj_t weakref_ref_new(mp_obj_t obj);
void weakref_ref_set(mp_obj_weakref_ref_t *self, mp_obj_t obj);
mp_obj_t weakref_ref_get(const mp_obj_weakref_ref_t *self);

#endif // MICROPY_INCLUDED_EXTMOD_MODWEAKREF_H
