#include "modmodbus.h"

#include "py/objarray.h"

#include "esp_log.h"
#include "uart.h"

#include <string.h>

#if ESP_IDF_VERSION_MINOR >= 3


STATIC mp_obj_t modbus_get_param_descriptors() {
    if (param_descriptions == NULL) {
        return mp_const_none;
    }
    return param_descriptions;
}
MP_DEFINE_CONST_FUN_OBJ_0(modbus_get_param_descriptors_obj, modbus_get_param_descriptors);


/**
 * @brief Get the length of a type in registers
 * Returns the length of a data type (eg PARAM_TYPE_U8).
 * Returns 255 for PARAM_TYPE_ASCII
 *
 * @param type
 */
uint8_t get_length_of_type_in_registers(uint8_t type) {
    switch (type) {
        case PARAM_TYPE_U8:
        case PARAM_TYPE_U16:
            return 1;
        case PARAM_TYPE_U32:
        case PARAM_TYPE_FLOAT:
            return 2;
        case PARAM_TYPE_ASCII:
            return 255;
        default:
            return 0;
    }
}


STATIC const mp_rom_map_elem_t mp_module_modbus_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_modbus) },

    { MP_ROM_QSTR(MP_QSTR_HOLDING), MP_ROM_INT(MB_PARAM_HOLDING) },
    { MP_ROM_QSTR(MP_QSTR_INPUT), MP_ROM_INT(MB_PARAM_INPUT) },
    { MP_ROM_QSTR(MP_QSTR_COIL), MP_ROM_INT(MB_PARAM_COIL) },
    { MP_ROM_QSTR(MP_QSTR_DISCRETE), MP_ROM_INT(MB_PARAM_DISCRETE) },

    { MP_ROM_QSTR(MP_QSTR_TYPE_U8), MP_ROM_INT(PARAM_TYPE_U8) },
    { MP_ROM_QSTR(MP_QSTR_TYPE_U16), MP_ROM_INT(PARAM_TYPE_U16) },
    { MP_ROM_QSTR(MP_QSTR_TYPE_U32), MP_ROM_INT(PARAM_TYPE_U32) },
    { MP_ROM_QSTR(MP_QSTR_TYPE_FLOAT), MP_ROM_INT(PARAM_TYPE_FLOAT) },
    { MP_ROM_QSTR(MP_QSTR_TYPE_ASCII), MP_ROM_INT(PARAM_TYPE_ASCII) },

    { MP_ROM_QSTR(MP_QSTR_SERIAL_MODE_RTU), MP_ROM_INT(MB_MODE_RTU) },
    { MP_ROM_QSTR(MP_QSTR_SERIAL_MODE_ASCII), MP_ROM_INT(MB_MODE_ASCII) },
    { MP_ROM_QSTR(MP_QSTR_SERIAL_MODE_TCP), MP_ROM_INT(MB_MODE_TCP) },
    { MP_ROM_QSTR(MP_QSTR_SERIAL_MODE_UDP), MP_ROM_INT(MB_MODE_UDP) },

    { MP_ROM_QSTR(MP_QSTR_PARITY_NONE), MP_ROM_INT(UART_PARITY_DISABLE) },
    { MP_ROM_QSTR(MP_QSTR_PARITY_EVEN), MP_ROM_INT(UART_PARITY_EVEN) },
    { MP_ROM_QSTR(MP_QSTR_PARITY_ODD), MP_ROM_INT(UART_PARITY_ODD) },

    { MP_ROM_QSTR(MP_QSTR_EVENT_HOLDING_REG_WR), MP_ROM_INT(MB_EVENT_HOLDING_REG_WR) },
    { MP_ROM_QSTR(MP_QSTR_EVENT_HOLDING_REG_RD), MP_ROM_INT(MB_EVENT_HOLDING_REG_RD) },
    { MP_ROM_QSTR(MP_QSTR_EVENT_INPUT_REG_RD), MP_ROM_INT(MB_EVENT_INPUT_REG_RD) },
    { MP_ROM_QSTR(MP_QSTR_EVENT_COILS_REG_WR), MP_ROM_INT(MB_EVENT_COILS_WR) },
    { MP_ROM_QSTR(MP_QSTR_EVENT_COILS_REG_RD), MP_ROM_INT(MB_EVENT_COILS_RD) },
    { MP_ROM_QSTR(MP_QSTR_EVENT_DISCRETE_REG_RD), MP_ROM_INT(MB_EVENT_DISCRETE_RD) },

    { MP_ROM_QSTR(MP_QSTR_Register_Area), MP_ROM_PTR(&modbus_register_area_type) },
    { MP_ROM_QSTR(MP_QSTR_Parameter_Descriptor), MP_ROM_PTR(&modbus_parameter_descriptor_type) },
    { MP_ROM_QSTR(MP_QSTR_Serial_Slave), MP_ROM_PTR(&modbus_serial_slave_type) },
    { MP_ROM_QSTR(MP_QSTR_Serial_Master), MP_ROM_PTR(&modbus_serial_master_type) },
    { MP_ROM_QSTR(MP_QSTR_TCP_Slave), MP_ROM_PTR(&modbus_tcp_slave_type) },
    { MP_ROM_QSTR(MP_QSTR_TCP_Master), MP_ROM_PTR(&modbus_tcp_master_type) },

    { MP_ROM_QSTR(MP_QSTR_get_parameter_descriptors), MP_ROM_PTR(&modbus_get_param_descriptors_obj) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_modbus_globals, mp_module_modbus_globals_table);

const mp_obj_module_t mp_module_modbus = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_modbus_globals,
};

MP_REGISTER_MODULE(MP_QSTR_modbus, mp_module_modbus, ESP_IDF_VERSION_MINOR >= 3);
#endif
