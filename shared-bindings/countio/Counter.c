
#include <stdint.h>

#include "shared/runtime/context_manager_helpers.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "py/runtime0.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/countio/Counter.h"
#include "shared-bindings/countio/Edge.h"
#include "shared-bindings/util.h"

//| class Counter:
//|     """Count the number of rising- and/or falling-edge transitions on a given pin.
//|     """
//|
//|     def __init__(self, pin: microcontroller.Pin, *, edge: Edge = Edge.FALL, pull: Optional[digitalio.Pull]) -> None:
//|         """Create a Counter object associated with the given pin that counts
//|         rising- and/or falling-edge transitions. At least one of ``rise`` and ``fall`` must be True.
//|         The default is to count only falling edges, and is for historical backward compatibility.
//|
//|         :param ~microcontroller.Pin pin: pin to monitor
//|         :param Edge: which edge transitions to count
//|         :param digitalio.Pull: enable a pull-up or pull-down if not None
//|
//|
//|         For example::
//|
//|             import board
//|             import countio
//|
//|             # Count rising edges only.
//|             pin_counter = countio.Counter(board.D1, edge=Edge.RISE)
//|             # Reset the count after 100 counts.
//|             while True:
//|                 if pin_counter.count >= 100:
//|                     pin_counter.reset()
//|                 print(pin_counter.count)
//|         """
//|         ...
//|
STATIC mp_obj_t countio_counter_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_pin, ARG_edge, ARG_pull };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_pin,  MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_edge, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_FROM_PTR(&edge_FALL_obj) } },
        { MP_QSTR_pull, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const mcu_pin_obj_t *pin = validate_obj_is_free_pin(args[ARG_pin].u_obj);
    const countio_edge_t edge = validate_edge(args[ARG_edge].u_obj, MP_QSTR_edge);
    const digitalio_pull_t pull = validate_pull(args[ARG_pull].u_obj, MP_QSTR_pull);
    // Make long-lived because some implementations use a pointer to the object as interrupt-handler data.
    countio_counter_obj_t *self = m_new_ll_obj(countio_counter_obj_t);
    self->base.type = &countio_counter_type;

    common_hal_countio_counter_construct(self, pin, edge, pull);

    return MP_OBJ_FROM_PTR(self);
}

//|     def deinit(self) -> None:
//|         """Deinitializes the Counter and releases any hardware resources for reuse."""
//|
STATIC mp_obj_t countio_counter_deinit(mp_obj_t self_in) {
    countio_counter_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_countio_counter_deinit(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(countio_counter_deinit_obj, countio_counter_deinit);

STATIC void check_for_deinit(countio_counter_obj_t *self) {
    if (common_hal_countio_counter_deinited(self)) {
        raise_deinited_error();
    }
}

//|     def __enter__(self) -> Counter:
//|         """No-op used by Context Managers."""
//|
//  Provided by context manager helper.

//|     def __exit__(self) -> None:
//|         """Automatically deinitializes the hardware when exiting a context. See
//|            :ref:`lifetime-and-contextmanagers` for more info."""
//|
STATIC mp_obj_t countio_counter_obj___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    common_hal_countio_counter_deinit(args[0]);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(countio_counter___exit___obj, 4, 4, countio_counter_obj___exit__);


//|     count: int
//|     """The current count in terms of pulses."""
//|
STATIC mp_obj_t countio_counter_obj_get_count(mp_obj_t self_in) {
    countio_counter_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    return mp_obj_new_int(common_hal_countio_counter_get_count(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(countio_counter_get_count_obj, countio_counter_obj_get_count);

STATIC mp_obj_t countio_counter_obj_set_count(mp_obj_t self_in, mp_obj_t new_count) {
    countio_counter_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    common_hal_countio_counter_set_count(self, mp_obj_get_int(new_count));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(countio_counter_set_count_obj, countio_counter_obj_set_count);

const mp_obj_property_t countio_counter_count_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&countio_counter_get_count_obj,
              (mp_obj_t)&countio_counter_set_count_obj,
              MP_ROM_NONE},
};

//|     def reset(self) -> None:
//|         """Resets the count back to 0."""
//|
STATIC mp_obj_t countio_counter_reset(mp_obj_t self_in) {
    countio_counter_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    common_hal_countio_counter_set_count(self, 0);
    return mp_const_none;
}


MP_DEFINE_CONST_FUN_OBJ_1(countio_counter_reset_obj, countio_counter_reset);

STATIC const mp_rom_map_elem_t countio_counter_locals_dict_table[] = {
    // Methods
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&countio_counter_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&countio_counter___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR_count), MP_ROM_PTR(&countio_counter_count_obj) },
    { MP_ROM_QSTR(MP_QSTR_reset), MP_ROM_PTR(&countio_counter_reset_obj) },
};
STATIC MP_DEFINE_CONST_DICT(countio_counter_locals_dict, countio_counter_locals_dict_table);

const mp_obj_type_t countio_counter_type = {
    { &mp_type_type },
    .name = MP_QSTR_Counter,
    .make_new = countio_counter_make_new,
    .locals_dict = (mp_obj_dict_t *)&countio_counter_locals_dict,
};
