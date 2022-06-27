/*
Github: junhuanchen
Copyright (c) 2018 Juwan
Licensed under the MIT license:
http://www.opensource.org/licenses/mit-license.php
*/

// refer: https://github.com/junhuanchen/esp-idf-software-serial

#include "sw_serial.h"

#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/nlr.h"	

#if MICROPY_PY_SERIAL

STATIC mp_obj_t serial_initialize() {
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(serial_initialize_obj, serial_initialize);

STATIC mp_obj_t serial_new(size_t n_args, const mp_obj_t *args) {
    if (n_args == 4) {
        mp_obj_t TxPin = args[0], RxPin = args[1], Inverse = args[2], buffSize = args[3];
        
        /*
        if (RxPin != XX) {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_Exception, 
                "RxPin(arg 0) %d ", mp_obj_get_int(RxPin)));
        }
        */
        
        SwSerial *self = sw_new(mp_obj_get_int(TxPin), mp_obj_get_int(RxPin), mp_obj_get_int(Inverse), mp_obj_get_int(buffSize));
        // printf("%d %d %d %d\n", mp_obj_get_int(TxPin), mp_obj_get_int(RxPin), mp_obj_get_int(Inverse), mp_obj_get_int(buffSize));
        if (self != NULL) {
            // sw_open(self, 115200);
            // sw_stop(self);
            return MP_OBJ_FROM_PTR(self);
        }
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(serial_new_obj, 0, 4, serial_new);

STATIC mp_obj_t serial_del(mp_obj_t self) {
    SwSerial *tmp = (SwSerial *)self;
    sw_del(tmp);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(serial_del_obj, serial_del);

STATIC mp_obj_t serial_write(mp_obj_t self, mp_obj_t data) {
    SwSerial *tmp = (SwSerial *)self;
    
    // printf("write %d \n", (uint8_t)mp_obj_get_int(data));
    
    sw_write(tmp, (uint8_t)mp_obj_get_int(data));
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(serial_write_obj, serial_write);

STATIC mp_obj_t serial_read(mp_obj_t self) {
    SwSerial *tmp = (SwSerial *)self;
    
    int res = sw_read(tmp);
    
    // printf("read %d \n", (uint8_t)res);
    
    return mp_obj_new_int(res);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(serial_read_obj, serial_read);

STATIC mp_obj_t serial_any(mp_obj_t self) {
    SwSerial *tmp = (SwSerial *)self;
    
    int res = sw_any(tmp);
    
    if (res > 0)
    {
        return mp_obj_new_int(res);
    }
    
    return mp_obj_new_int(0);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(serial_any_obj, serial_any);

STATIC mp_obj_t serial_stop(mp_obj_t self) {
    SwSerial *tmp = (SwSerial *)self;
    return mp_obj_new_int(sw_stop(tmp));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(serial_stop_obj, serial_stop);

STATIC mp_obj_t serial_open(mp_obj_t self, mp_obj_t baudRate) {
    SwSerial *tmp = (SwSerial *)self;
    esp_err_t res = sw_open(tmp, mp_obj_get_int(baudRate));
    return mp_obj_new_int(res);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(serial_open_obj, serial_open);

STATIC const mp_rom_map_elem_t serial_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_serial) },
    { MP_ROM_QSTR(MP_QSTR___init__), MP_ROM_PTR(&serial_initialize_obj) },

    { MP_ROM_QSTR(MP_QSTR_new), MP_ROM_PTR(&serial_new_obj) },
    { MP_ROM_QSTR(MP_QSTR_del), MP_ROM_PTR(&serial_del_obj) },
    { MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&serial_open_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&serial_stop_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&serial_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&serial_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_any), MP_ROM_PTR(&serial_any_obj) },
    
    { MP_ROM_QSTR(MP_QSTR_EOF), MP_ROM_INT((mp_int_t)SW_EOF)},
};

STATIC MP_DEFINE_CONST_DICT(serial_module_globals, serial_module_globals_table);

const mp_obj_module_t serial_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&serial_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_serial, serial_module);

#endif