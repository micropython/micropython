#ifndef __LVMP_DRV_COMMON_H
#define __LVMP_DRV_COMMON_H

#include <string.h>
#include "py/obj.h"
#include "py/runtime.h"
#include "py/binary.h"

//////////////////////////////////////////////////////////////////////////////
// A read-only buffer that contains a C pointer
// Used to communicate function pointers to lvgl Micropython bindings
//

typedef struct mp_ptr_t
{
    mp_obj_base_t base;
    void *ptr;
} mp_ptr_t;

STATIC mp_int_t mp_ptr_get_buffer(mp_obj_t self_in, mp_buffer_info_t *bufinfo, mp_uint_t flags)
{
    mp_ptr_t *self = MP_OBJ_TO_PTR(self_in);

    if (flags & MP_BUFFER_WRITE) {
        // read-only ptr
        return 1;
    }

    bufinfo->buf = &self->ptr;
    bufinfo->len = sizeof(self->ptr);
    bufinfo->typecode = BYTEARRAY_TYPECODE;
    return 0;
}

#define PTR_OBJ(ptr_global) ptr_global ## _obj

#define DEFINE_PTR_OBJ_TYPE(ptr_obj_type, ptr_type_qstr)\
STATIC const mp_obj_type_t ptr_obj_type = {\
    { &mp_type_type },\
    .name = ptr_type_qstr,\
    .buffer_p = { .get_buffer = mp_ptr_get_buffer }\
}

#define DEFINE_PTR_OBJ(ptr_global)\
DEFINE_PTR_OBJ_TYPE(ptr_global ## _type, MP_QSTR_ ## ptr_global);\
STATIC const mp_ptr_t PTR_OBJ(ptr_global) = {\
    { &ptr_global ## _type },\
    &ptr_global\
}

#define NEW_PTR_OBJ(name, value)\
({\
    DEFINE_PTR_OBJ_TYPE(ptr_obj_type, MP_QSTR_ ## name);\
    mp_ptr_t *ptr_obj = m_new_obj(mp_ptr_t);\
    ptr_obj->base.type = &ptr_obj_type;\
    ptr_obj->ptr = value;\
    MP_OBJ_FROM_PTR(ptr_obj);\
})

STATIC inline void *PTR_FROM_OBJ(mp_obj_t obj)
{
    void *result;
    if (!obj) return NULL;
    mp_buffer_info_t buffer_info;
    mp_get_buffer_raise(obj, &buffer_info, MP_BUFFER_READ);
    if (buffer_info.len != sizeof(result) || buffer_info.typecode != BYTEARRAY_TYPECODE){
        nlr_raise(
            mp_obj_new_exception_msg_varg(
                &mp_type_SyntaxError, "Cannot convert %s to pointer!", mp_obj_get_type_str(obj)));
    }
    memcpy(&result, buffer_info.buf, sizeof(result));
    return result;
}

#endif // __LVMP_DRV_COMMON_H
