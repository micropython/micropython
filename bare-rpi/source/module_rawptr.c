#include "py/mpconfig.h"
#include "py/nlr.h"
#include "py/misc.h"
#include "py/qstr.h"
#include "py/obj.h"
#include "py/runtime.h"

#include <stdio.h>

typedef struct _rawptr_t 
{
    mp_obj_base_t base;
    void* address;
} rawptr_t;

extern const mp_obj_type_t rawptr_type;

mp_obj_t rawptr_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) 
{
    // test if number of arguments is exactly one (?) - on error: raise exception
    mp_arg_check_num(n_args, n_kw, 1, 1, true);
    // I want an object of my class, right?
    rawptr_t *self = m_new_obj(rawptr_t);
    // set the type
    self->base.type = &rawptr_type;
    // set the parameter
    self->address = (void*) mp_obj_get_int(args[0]);
    return MP_OBJ_FROM_PTR(self);
}


STATIC void rawptr_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) 
{
    rawptr_t *self = MP_OBJ_TO_PTR(self_in);
    printf ("RawPtr-Type: { .address = 0x%lu }\n", self->address );
}

STATIC mp_obj_t rawptr_increment(mp_obj_t self_in) 
{
    rawptr_t *self = MP_OBJ_TO_PTR(self_in);
    self->address++;
    return mp_const_none;
}

STATIC mp_obj_t rawptr_decrement(mp_obj_t self_in)
{
    rawptr_t* self = MP_OBJ_TO_PTR ( self_in );
    self->address--;
    return mp_const_none;
}

STATIC mp_obj_t rawptr_offset ( mp_obj_t self_in, mp_obj_t off )
{
    rawptr_t* self = MP_OBJ_TO_PTR ( self_in );
    int offset = mp_obj_get_int( off );
    self->address = (void*) ((uint32_t) self->address) + ((int) offset);
    return mp_const_none;
}

STATIC mp_obj_t rawptr_setAddr ( mp_obj_t self_in, mp_obj_t addr )
{
    rawptr_t* self = MP_OBJ_TO_PTR ( self_in );
    self->address = (void*) mp_obj_get_int( addr );
    return mp_const_none;
}

STATIC mp_obj_t rawptr_getAddr ( mp_obj_t self_in )
{
    rawptr_t* self = MP_OBJ_TO_PTR ( self_in );
    return mp_obj_new_int( (uint32_t) self->address ); 
}

STATIC mp_obj_t rawptr_read ( mp_obj_t self_in )
{
    rawptr_t* self = MP_OBJ_TO_PTR ( self_in );
    uint32_t val = *((uint32_t*) self->address);
    return mp_obj_new_int( val ); 
}

STATIC mp_obj_t rawptr_write ( mp_obj_t self_in, mp_obj_t val )
{
    rawptr_t* self = MP_OBJ_TO_PTR ( self_in );
    uint32_t v = mp_obj_get_int( val );
    *((uint32_t*)self->address) = v;
    printf ("WRITE TO 0x%8p -> value: 0x%lx\n", self->address, v );
    return mp_const_none; 
}

MP_DEFINE_CONST_FUN_OBJ_1(rawptr_increment_obj, rawptr_increment);
MP_DEFINE_CONST_FUN_OBJ_1(rawptr_decrement_obj, rawptr_decrement);
MP_DEFINE_CONST_FUN_OBJ_2(rawptr_setAddr_obj, rawptr_setAddr);
MP_DEFINE_CONST_FUN_OBJ_2(rawptr_offset_obj, rawptr_offset);
MP_DEFINE_CONST_FUN_OBJ_1(rawptr_getAddr_obj, rawptr_getAddr);
MP_DEFINE_CONST_FUN_OBJ_1(rawptr_read_obj, rawptr_read);
MP_DEFINE_CONST_FUN_OBJ_2(rawptr_write_obj, rawptr_write);


STATIC const mp_rom_map_elem_t rawptr_locals_dict_table[] = 
{
    { MP_ROM_QSTR(MP_QSTR_inc), MP_ROM_PTR(&rawptr_increment_obj) },
    { MP_ROM_QSTR(MP_QSTR_dec), MP_ROM_PTR(&rawptr_decrement_obj) },
    { MP_ROM_QSTR(MP_QSTR_offset), MP_ROM_PTR(&rawptr_offset_obj) },
    { MP_ROM_QSTR(MP_QSTR_setAddr), MP_ROM_PTR(&rawptr_setAddr_obj) },
    { MP_ROM_QSTR(MP_QSTR_getAddr), MP_ROM_PTR(&rawptr_getAddr_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&rawptr_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&rawptr_write_obj) },
};
STATIC MP_DEFINE_CONST_DICT ( rawptr_locals_dict, rawptr_locals_dict_table );

const mp_obj_type_t rawptr_type = 
{
    { &mp_type_type },
    .name = MP_QSTR_RawPtr,
    .print = rawptr_print,
    .make_new = rawptr_new,
    .locals_dict = (mp_obj_dict_t*)&rawptr_locals_dict,
};

STATIC const mp_map_elem_t cmodule_globals_table[] = 
{
    { MP_OBJ_NEW_QSTR(MP_QSTR_RawPtr), (mp_obj_t)&rawptr_type }
};

STATIC MP_DEFINE_CONST_DICT ( mp_cmodule_globals, cmodule_globals_table );

const mp_obj_module_t cmodule = 
{
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_cmodule_globals,
};




