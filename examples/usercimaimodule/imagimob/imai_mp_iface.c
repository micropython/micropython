// Include MicroPython API.
#include "py/runtime.h"

#include "py/mphal.h"
#include "model.h"

static mp_obj_t init(void){
    IMAI_init();
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(init_obj, init);

static mp_obj_t enqueue(const mp_obj_t data_in_obj){
    float data_in[IMAI_DATA_IN_COUNT];
    mp_obj_t *data_in_items;
    size_t len;
    mp_obj_get_array(data_in_obj, &len, &data_in_items);
    if (len != IMAI_DATA_IN_COUNT) {
        mp_raise_ValueError("data_in must be a list of IMAI_DATA_IN_COUNT floats");
    }
    for (int i = 0; i < IMAI_DATA_IN_COUNT; i++) {
        data_in[i] = mp_obj_float_get(data_in_items[i]);
    }
    int result = IMAI_enqueue(data_in);
    return MP_OBJ_NEW_SMALL_INT(result);
}
MP_DEFINE_CONST_FUN_OBJ_1(enqueue_obj, enqueue);

static mp_obj_t dequeue(mp_obj_t data_out_obj) {
    mp_buffer_info_t buf_info;
    mp_get_buffer(data_out_obj, &buf_info, MP_BUFFER_WRITE);
    float *data_out = (float *)buf_info.buf;
    int result = IMAI_dequeue(data_out);
    if (result == 0) {
        return MP_OBJ_NEW_SMALL_INT(result);
    } else if (result == -1) {
        return MP_OBJ_NEW_SMALL_INT(result);
    } else if (result == -2) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Internal memory allocation error"), result);
    }
    return MP_OBJ_NEW_SMALL_INT(result);
}
MP_DEFINE_CONST_FUN_OBJ_1(dequeue_obj, dequeue);

static const mp_rom_map_elem_t example_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_imai) },
    
    // imagimob fns
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&init_obj) },
    { MP_ROM_QSTR(MP_QSTR_enqueue), MP_ROM_PTR(&enqueue_obj) },
    { MP_ROM_QSTR(MP_QSTR_dequeue), MP_ROM_PTR(&dequeue_obj) },

    // constants
    { MP_ROM_QSTR(MP_QSTR_DATA_IN),                      MP_ROM_INT(IMAI_DATA_IN_COUNT) },
    { MP_ROM_QSTR(MP_QSTR_DATA_OUT),                     MP_ROM_INT(IMAI_DATA_OUT_COUNT) },
};
static MP_DEFINE_CONST_DICT(example_module_globals, example_module_globals_table);

// Define module object.
const mp_obj_module_t imai_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&example_module_globals,
};

// Register the module to make it available in Python.
MP_REGISTER_MODULE(MP_QSTR_imai, imai_module);
