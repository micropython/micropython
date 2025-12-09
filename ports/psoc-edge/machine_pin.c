#include "py/obj.h"
#include "extmod/modmachine.h"
#include "extmod/virtpin.h"

typedef struct _machine_pin_obj_t {
    mp_obj_base_t base;
    qstr name;
    uint8_t port;
    uint8_t pin;
}
machine_pin_obj_t;

static void machine_pin_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {

}

mp_obj_t mp_pin_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {

    return mp_const_none;
}

static mp_obj_t machine_pin_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {

    return mp_const_none;
}

static mp_uint_t pin_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {

    return 0;
}

static const mp_pin_p_t pin_pin_p = {
    .ioctl = pin_ioctl,
};

static const mp_rom_map_elem_t machine_pin_locals_dict_table[] = {
    // Instance methods

    // Const
};

static MP_DEFINE_CONST_DICT(machine_pin_locals_dict, machine_pin_locals_dict_table);


MP_DEFINE_CONST_OBJ_TYPE(
    machine_pin_type,
    MP_QSTR_Pin,
    MP_TYPE_FLAG_NONE,
    make_new, mp_pin_make_new,
    print, machine_pin_print,
    call, machine_pin_call,
    protocol, &pin_pin_p,
    locals_dict, &machine_pin_locals_dict
    );
