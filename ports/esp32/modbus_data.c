#include "modmodbus.h"

#include "py/objarray.h"

#include "esp_log.h"
#include "uart.h"

#include <string.h>

#if ESP_IDF_VERSION_MINOR >= 3

static portMUX_TYPE modbus_spinlock = portMUX_INITIALIZER_UNLOCKED;
bool register_area_set = false;

/****************************
 *
 *     Register Area
 *
 *      Data area on the slave device, that is accessed via modbus
 *      from the master device
 *
 * **************************
 */

/* modbus.Register_Area(
    type=modbus.AREA_HOLDING / modbus.AREA_INPUT / modbus.AREA_COIL / modbus.AREA_DISCRETE,
    start_offset=int,
    size=int (number of bytes)
) */
STATIC mp_obj_t modbus_register_area_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *in_args) {
    mp_arg_check_num(n_args, n_kw, 3, 3, true);
    if (slave_running) {
        mp_raise_ValueError(MP_ERROR_TEXT("Slave already running. You must define registers before you start the slave."));
    }

    enum { ARG_type, ARG_start_offset, ARG_size };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_type, MP_ARG_INT | MP_ARG_REQUIRED, {.u_int = MB_PARAM_HOLDING}},
        { MP_QSTR_start_offset, MP_ARG_INT | MP_ARG_REQUIRED, {.u_int = 0}},
        { MP_QSTR_size, MP_ARG_INT | MP_ARG_REQUIRED, {.u_int = 0}},
    };

    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, in_args + n_args);

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, in_args, &kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if ((args[ARG_type].u_int < 0) || (args[ARG_type].u_int > 3)) {
        mp_raise_ValueError(MP_ERROR_TEXT("Type not allowed"));
    }

    if ((args[ARG_start_offset].u_int < 0)) {
        mp_raise_ValueError(MP_ERROR_TEXT("Negative Start Offset not allowed"));
    }

    if ((args[ARG_size].u_int < 0)) {
        mp_raise_ValueError(MP_ERROR_TEXT("Negative Size not allowed"));
    }

    if (!slave_set) {
        mp_raise_ValueError(MP_ERROR_TEXT("Modbus Slave must be initialized first"));
    }

    modbus_register_area_obj_t *self = m_new_obj(modbus_register_area_obj_t);
    self->base.type = &modbus_register_area_type;
    self->type = args[ARG_type].u_int;
    self->reg_start_offset = args[ARG_start_offset].u_int;
    self->reg_size = args[ARG_size].u_int;
    self->address = (void *)calloc(self->reg_size, 2);


    mb_register_area_descriptor_t reg_area;
    reg_area.type = self->type;
    reg_area.start_offset = self->reg_start_offset;
    if ((self->type == MB_PARAM_HOLDING) || (self->type == MB_PARAM_INPUT)) {
        reg_area.size = self->reg_size * 2;
    } else {
        reg_area.size = self->reg_size / 8 + 1;
    }
    reg_area.address = self->address;

    ESP_ERROR_CHECK(mbc_slave_set_descriptor(reg_area));
    register_area_set = true;
    return MP_OBJ_FROM_PTR(self);
};

STATIC void modbus_register_area_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    modbus_register_area_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "Register_Area(type=%s, start_offset=%u, size=%u, address=0x%p)",
        area_names[self->type], self->reg_start_offset, self->reg_size, self->address);
};


/* read(self, offset, length)
    returns registers from offset to offset+length as bytearray
*/
STATIC mp_obj_t modbus_register_area_read_raw(size_t n_args, const mp_obj_t *args) {
    mp_arg_check_num(n_args, 0, 3, 3, false);
    modbus_register_area_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    int16_t reg_start_addr = (int16_t)mp_obj_get_int(args[1]);
    int16_t reg_length = (int16_t)mp_obj_get_int(args[2]);

    raise_VauleError_if_outside_area(self->reg_start_offset, self->reg_size, reg_start_addr, reg_length);

    portENTER_CRITICAL(&modbus_spinlock);
    mp_obj_t data = mp_obj_new_bytearray(
        2 * reg_length,
        (void *)(self->address + 2 * (reg_start_addr - self->reg_start_offset))
        );
    portEXIT_CRITICAL(&modbus_spinlock);

    return data;
};
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(modbus_register_area_read_raw_obj, 3, modbus_register_area_read_raw);


/* write(self, offset, length, data)
    writes registers from offset to offset+length.
    data (bytearray) must be at least 2 times as long as parameter length.
*/
STATIC mp_obj_t modbus_register_area_write_raw(size_t n_args, const mp_obj_t *args) {
    mp_arg_check_num(n_args, 0, 4, 4, false);
    modbus_register_area_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    int16_t reg_start_addr = (int16_t)mp_obj_get_int(args[1]);
    int16_t reg_length = (int16_t)mp_obj_get_int(args[2]);

    raise_VauleError_if_outside_area(self->reg_start_offset, self->reg_size, reg_start_addr, reg_length);

    if (!mp_obj_is_type(args[3], &mp_type_bytearray)) {
        mp_raise_TypeError(MP_ERROR_TEXT("Argument data must be of type bytearray"));
    }
    mp_obj_array_t *data = (mp_obj_array_t *)args[3];

    if (2 * data->len < reg_length) {
        mp_raise_ValueError(MP_ERROR_TEXT("Not enough data to write"));
    }

    portENTER_CRITICAL(&modbus_spinlock);
    memcpy((void *)(self->address + 2 * (reg_start_addr - self->reg_start_offset)), data->items, 2 * reg_length);
    portEXIT_CRITICAL(&modbus_spinlock);

    return mp_const_none;
};
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(modbus_register_area_write_raw_obj, 4, modbus_register_area_write_raw);


/* get_registers(self, register, length)
    returns registers from register to register+length as bytearray
*/
STATIC mp_obj_t modbus_register_area_read_registers(size_t n_args, const mp_obj_t *args) {
    mp_arg_check_num(n_args, 0, 3, 3, false);
    modbus_register_area_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    int16_t reg_start_addr = (int16_t)mp_obj_get_int(args[1]);
    int16_t reg_length = (int16_t)mp_obj_get_int(args[2]);

    raise_VauleError_if_outside_area(self->reg_start_offset, self->reg_size, reg_start_addr, reg_length);

    portENTER_CRITICAL(&modbus_spinlock);
    mp_obj_t data = mp_obj_new_list(0, NULL);
    for (uint16_t i = 0; i < reg_length; i++) {
        uint8_t *p = (uint8_t *)(self->address + 2 * (reg_start_addr - self->reg_start_offset) + 2 * i);
        mp_obj_t item = mp_obj_new_int((p[1] << 8) + p[0]);
        mp_obj_list_append(data, item);
    }
    portEXIT_CRITICAL(&modbus_spinlock);
    return data;
};
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(modbus_register_area_read_registers_obj, 3, modbus_register_area_read_registers);


/* set_registers(self, register, data)
    returns registers from register to register+length as bytearray
*/
STATIC mp_obj_t modbus_register_area_write_registers(size_t n_args, const mp_obj_t *args) {
    mp_arg_check_num(n_args, 0, 3, 3, false);
    modbus_register_area_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    if (!mp_obj_is_type(args[2], &mp_type_list)) {
        mp_raise_TypeError(MP_ERROR_TEXT("Argument data must be of type list"));
    }
    mp_obj_list_t *data = (mp_obj_list_t *)args[2];

    int16_t reg_start_addr = (int16_t)mp_obj_get_int(args[1]);
    int16_t reg_length = data->len / 2 + 1;

    raise_VauleError_if_outside_area(self->reg_start_offset, self->reg_size, reg_start_addr, reg_length);

    portENTER_CRITICAL(&modbus_spinlock);
    for (uint16_t i = 0; i < data->len; i++) {
        uint16_t x = (uint16_t)get_uint_checked(data->items[i], PARAM_TYPE_U16, true);
        memcpy((void *)(self->address + 2 * (reg_start_addr - self->reg_start_offset) + 2 * i), &x, 2);
    }
    portEXIT_CRITICAL(&modbus_spinlock);
    return mp_const_none;
};
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(modbus_register_area_write_registers_obj, 3, modbus_register_area_write_registers);


/* get_bits(self, start_bit, length)
    returns bits from start_bit to start_bit+length as bytearray
*/
STATIC mp_obj_t modbus_register_area_read_bits(size_t n_args, const mp_obj_t *args) {
    mp_arg_check_num(n_args, 0, 3, 3, false);
    modbus_register_area_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    if (mp_obj_get_int(args[1]) < self->reg_start_offset) {
        mp_raise_ValueError(MP_ERROR_TEXT("Reading outside Register Area is not allowed"));
    }

    if (mp_obj_get_int(args[2]) < 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("Negative Length not allowed"));
    }

    int16_t offset = (int16_t)mp_obj_get_int(args[1]);
    int16_t length = (int16_t)mp_obj_get_int(args[2]);

    if ((offset + length > self->reg_start_offset + self->reg_size * 16)) {
        mp_raise_ValueError(MP_ERROR_TEXT("Reading outside Register Area is not allowed"));
    }

    uint16_t first_reg = (offset - self->reg_start_offset) / 8;
    uint16_t last_reg = (offset - self->reg_start_offset + length) / 8;

    uint16_t first_bit = (offset - self->reg_start_offset) % 8;
    uint16_t last_bit = (offset - self->reg_start_offset + length) - 8 * first_reg;

    portENTER_CRITICAL(&modbus_spinlock);
    mp_obj_t data = mp_obj_new_list(0, NULL);
    for (uint16_t i = first_reg; i <= last_reg; i++) {
        uint8_t p = *(uint8_t*)(self->address + i);
        for (uint8_t j = first_bit; j < MIN(last_bit, 8); j++) {
            mp_obj_t item = mp_obj_new_bool((p & (1 << j)) > 0);
            mp_obj_list_append(data, item);
        }
        first_bit = 0;
        last_bit -= 8;
    }
    portEXIT_CRITICAL(&modbus_spinlock);
    return data;
};
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(modbus_register_area_read_bits_obj, 3, modbus_register_area_read_bits);


/* set_bits(self, start_bit, data)
    returns registers from start_bit to start_bit+length as bytearray
*/
STATIC mp_obj_t modbus_register_area_write_bits(size_t n_args, const mp_obj_t *args) {
    mp_arg_check_num(n_args, 0, 3, 3, false);
    modbus_register_area_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    if (mp_obj_get_int(args[1]) < self->reg_start_offset) {
        mp_raise_ValueError(MP_ERROR_TEXT("Writing outside Register Area is not allowed"));
    }

    if (!mp_obj_is_type(args[2], &mp_type_list)) {
        mp_raise_TypeError(MP_ERROR_TEXT("Argument data must be of type list"));
    }
    mp_obj_list_t *data = (mp_obj_list_t *)args[2];
    int16_t offset = (int16_t)mp_obj_get_int(args[1]);
    int16_t length = data->len;

    uint16_t first_reg = (offset - self->reg_start_offset) / 8;
    uint16_t last_reg = (offset - self->reg_start_offset + length) / 8;

    uint16_t first_bit = (offset - self->reg_start_offset) % 8;

    portENTER_CRITICAL(&modbus_spinlock);
    int16_t i = 0;
    int16_t offs = 0;
    uint16_t p[last_reg - first_reg + 1];
    memcpy(p, (void *)(self->address + first_reg), 2 * (last_reg - first_reg + 1));

    while (i < length) {
        bool bit = mp_obj_get_int(data->items[i]) > 0;

        if (bit) {
            p[offs] |= 1 << (first_bit + i - 16 * offs);
        } else {
            p[offs] &= 0xFFFF - (1 << (first_bit + i - 16 * offs));
        }
        i += 1;
        if (first_bit + i - 16 * offs >= 16) {
            first_bit = 0;
            offs += 1;
        }
    }
    memcpy((void *)(self->address + first_reg), p, 2 * (last_reg - first_reg + 1));
    portEXIT_CRITICAL(&modbus_spinlock);
    return mp_const_none;
};
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(modbus_register_area_write_bits_obj, 3, modbus_register_area_write_bits);


STATIC mp_obj_t modbus_register_area_read(size_t n_args, const mp_obj_t *args)  {
    mp_arg_check_num(n_args, 0, 3, 4, false);
    modbus_register_area_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    int16_t reg_start_addr = mp_obj_get_int(args[1]);
    uint8_t type = mp_obj_get_int(args[2]);

    uint8_t reg_length = 0;
    if (n_args == 4) {
        if (type == PARAM_TYPE_ASCII) {
            reg_length = mp_obj_get_int(args[3]);
        } else {
            mp_raise_ValueError("parameter length allowed only for TYPE_ASCII");
        }
    } else {
        if (type == PARAM_TYPE_ASCII) {
            mp_raise_ValueError("parameter length required for TYPE_ASCII");
        } else {
            reg_length = get_length_of_type_in_registers(type);
        }
    }

    raise_VauleError_if_outside_area(self->reg_start_offset, self->reg_size, reg_start_addr, reg_length);

    if ((type == PARAM_TYPE_U8) || (type == PARAM_TYPE_U16)) {
        uint16_t temp_data;

        portENTER_CRITICAL(&modbus_spinlock);
        memcpy(&temp_data, self->address + 2 * (reg_start_addr - self->reg_start_offset), 2);
        portEXIT_CRITICAL(&modbus_spinlock);

        if (type == PARAM_TYPE_U8) {
            return mp_obj_new_int(temp_data & 0xFF);
        }

        if (type == PARAM_TYPE_U16) {
            return mp_obj_new_int(temp_data);
        }
    }

    if (type == PARAM_TYPE_U32) {
        uint16_t temp_data[2];

        portENTER_CRITICAL(&modbus_spinlock);
        memcpy(temp_data, self->address + 2 * (reg_start_addr - self->reg_start_offset), 4);
        portEXIT_CRITICAL(&modbus_spinlock);

        return mp_obj_new_int_from_uint((temp_data[1] << 16) + temp_data[0]);
    }

    if (type == PARAM_TYPE_FLOAT) {
        float temp_data;

        portENTER_CRITICAL(&modbus_spinlock);
        memcpy(&temp_data, self->address + 2 * (reg_start_addr - self->reg_start_offset), 4);
        portEXIT_CRITICAL(&modbus_spinlock);

        return mp_obj_new_float(temp_data);
    }

    if (type == PARAM_TYPE_ASCII) {
        char temp_str[reg_length * 2];
        strncpy(temp_str, self->address + 2 * (reg_start_addr - self->reg_start_offset), reg_length * 2);
        return mp_obj_new_str(temp_str, strlen(temp_str));
    }

    mp_raise_ValueError("unknown type");
}
MP_DEFINE_CONST_FUN_OBJ_VAR(modbus_register_area_read_obj, 3, modbus_register_area_read);

STATIC mp_obj_t modbus_register_area_write(size_t n_args, const mp_obj_t *args) {
    mp_arg_check_num(n_args, 0, 4, 4, false);
    modbus_register_area_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    int16_t start_addr = mp_obj_get_int(args[1]);
    uint8_t type = mp_obj_get_int(args[2]);

    uint8_t reg_length = 0;
    if (type != PARAM_TYPE_ASCII) {
        reg_length = get_length_of_type_in_registers(type);
        raise_VauleError_if_outside_area(self->reg_start_offset, self->reg_size, start_addr, reg_length);
    }

    if (type == PARAM_TYPE_FLOAT) {
        if (mp_obj_is_float(args[3])||mp_obj_is_int(args[3])) {
            float temp_data = mp_obj_get_float(args[3]);

            portENTER_CRITICAL(&modbus_spinlock);
            memcpy(self->address + 2 * (start_addr - self->reg_start_offset), &temp_data, 4);
            portEXIT_CRITICAL(&modbus_spinlock);

            return mp_const_none;
        }
        mp_raise_ValueError(MP_ERROR_TEXT("value could not be converted to float"));
    }

    if ((type == PARAM_TYPE_U8) || (type == PARAM_TYPE_U16)) {
        uint16_t temp_data = (uint16_t)get_uint_checked(args[3], type, true);
        portENTER_CRITICAL(&modbus_spinlock);
        memcpy(self->address + 2 * (start_addr - self->reg_start_offset), &temp_data, 2);
        portEXIT_CRITICAL(&modbus_spinlock);
        return mp_const_none;
    } else if (type == PARAM_TYPE_U32) {
        uint32_t temp_data = (uint32_t)get_uint_checked(args[3], type, true);
        portENTER_CRITICAL(&modbus_spinlock);
        memcpy(self->address + 2 * (start_addr - self->reg_start_offset), &temp_data, 4);
        portEXIT_CRITICAL(&modbus_spinlock);
        return mp_const_none;
    }
    
    if (type == PARAM_TYPE_ASCII) {
        char *txt = mp_obj_str_get_str(args[3]);
        uint16_t reg_length = strlen(txt) / 2 + 1;
        raise_VauleError_if_outside_area(self->reg_start_offset, self->reg_size, start_addr, reg_length);

        portENTER_CRITICAL(&modbus_spinlock);
        strcpy(self->address + 2 * (start_addr - self->reg_start_offset), txt);
        portEXIT_CRITICAL(&modbus_spinlock);
        return mp_const_none;
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR(modbus_register_area_write_obj, 4, modbus_register_area_write);


STATIC void modbus_register_area_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    modbus_register_area_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (dest[0] == MP_OBJ_NULL) {
        // Load
        if (attr == MP_QSTR_type) {
            dest[0] = mp_obj_new_int(self->type);
        } else if (attr == MP_QSTR_start_offset) {
            dest[0] = mp_obj_new_int(self->reg_start_offset);
        } else if (attr == MP_QSTR_size) {
            dest[0] = mp_obj_new_int(self->reg_size);
        } else if (attr == MP_QSTR_read_raw) {
            dest[0] = MP_OBJ_FROM_PTR(&modbus_register_area_read_raw_obj);
            dest[1] = self_in;
        } else if (attr == MP_QSTR_write_raw) {
            dest[0] = MP_OBJ_FROM_PTR(&modbus_register_area_write_raw_obj);
            dest[1] = self_in;
        } else if (attr == MP_QSTR_read_registers) {
            dest[0] = MP_OBJ_FROM_PTR(&modbus_register_area_read_registers_obj);
            dest[1] = self_in;
        } else if (attr == MP_QSTR_write_registers) {
            dest[0] = MP_OBJ_FROM_PTR(&modbus_register_area_write_registers_obj);
            dest[1] = self_in;
        } else if (attr == MP_QSTR_read_bits) {
            dest[0] = MP_OBJ_FROM_PTR(&modbus_register_area_read_bits_obj);
            dest[1] = self_in;
        } else if (attr == MP_QSTR_write_bits) {
            dest[0] = MP_OBJ_FROM_PTR(&modbus_register_area_write_bits_obj);
            dest[1] = self_in;
        } else if (attr == MP_QSTR_read) {
            dest[0] = MP_OBJ_FROM_PTR(&modbus_register_area_read_obj);
            dest[1] = self_in;
        } else if (attr == MP_QSTR_write) {
            dest[0] = MP_OBJ_FROM_PTR(&modbus_register_area_write_obj);
            dest[1] = self_in;
        }
    } else if (dest[1] != MP_OBJ_NULL) {
        // Store
        mp_raise_ValueError(MP_ERROR_TEXT("Parameter is Read Only"));
    }
};

/****************************
 *
 *     Parameter Descriptor
 *
 * **************************
 */

/* modbus.Parameter_Descriptor(
    key=str, unit=str, slave_addr=int,
    mb_param_type=INPUT/HOLDING/COIL/DISCRETE,
    reg_start=int, size=int (registers),
    data_type=FLOAT, UINT16, ...
    access=int, min=int, max=int, step=int
) */
STATIC mp_obj_t modbus_parameter_discriptor_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *in_args) {
    mp_arg_check_num(n_args, n_kw, 7, 11, true);
    if (master_running) {
        mp_raise_ValueError(MP_ERROR_TEXT("Master already running. You must define parameter descriptors before you start the master."));
    }

    enum { ARG_key, ARG_unit, ARG_slave_addr, ARG_mb_param_type, ARG_reg_start, ARG_size, ARG_data_type, ARG_access, ARG_min, ARG_max, ARG_step};
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_key,            MP_ARG_OBJ | MP_ARG_REQUIRED, {.u_obj = mp_const_none}},
        { MP_QSTR_unit,           MP_ARG_OBJ | MP_ARG_REQUIRED, {.u_obj = mp_const_none}},
        { MP_QSTR_slave_addr,     MP_ARG_INT | MP_ARG_REQUIRED, {.u_int = 0}},
        { MP_QSTR_mb_param_type,  MP_ARG_INT | MP_ARG_REQUIRED, {.u_int = 0}},
        { MP_QSTR_reg_start,      MP_ARG_INT | MP_ARG_REQUIRED, {.u_int = 0}},
        { MP_QSTR_size,           MP_ARG_INT | MP_ARG_REQUIRED, {.u_int = 0}},
        { MP_QSTR_data_type,      MP_ARG_INT,                   {.u_int = PARAM_TYPE_U16}},
        { MP_QSTR_access,         MP_ARG_INT,                   {.u_int = PAR_PERMS_READ_WRITE_TRIGGER }},
        { MP_QSTR_min,            MP_ARG_INT,                   {.u_int = 0 }},
        { MP_QSTR_max,            MP_ARG_INT,                   {.u_int = 0 }},
        { MP_QSTR_step,           MP_ARG_INT,                   {.u_int = 0 }},
    };

    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, in_args + n_args);

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, in_args, &kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (args[ARG_key].u_obj == mp_const_none) {
        mp_raise_ValueError(MP_ERROR_TEXT("key cannot be None"));
    }
    if (args[ARG_unit].u_obj == mp_const_none) {
        mp_raise_ValueError(MP_ERROR_TEXT("unit cannot be None"));
    }
    if (args[ARG_size].u_int > 12) {
        mp_raise_ValueError(MP_ERROR_TEXT("max parameter size is 12"));
    }

    if (!master_set) {
        mp_raise_ValueError(MP_ERROR_TEXT("Modbus Master must be initialized first"));
    }

    modbus_parameter_descriptor_obj_t *self = m_new_obj(modbus_parameter_descriptor_obj_t);
    self->base.type = &modbus_parameter_descriptor_type;
    self->cid = cid_counter++;
    self->param_key = args[ARG_key].u_obj;
    self->param_unit = args[ARG_unit].u_obj;

    // important for building request
    self->mb_slave_addr = args[ARG_slave_addr].u_int;

    // important for building request
    self->mb_param_type = args[ARG_mb_param_type].u_int;

    // important for building request
    self->mb_reg_start = args[ARG_reg_start].u_int;

    // important for building request
    self->mb_size = args[ARG_size].u_int;

    // could be used to write the result into an address. not used
    self->param_offset = NULL;

    // type is just for this api
    // it has nothing to do with the number of bytes being read from slave
    self->param_type = args[ARG_data_type].u_int;

    // relevant for string length
    self->param_size = self->mb_size*2;
    self->min = args[ARG_min].u_int;
    self->max = args[ARG_max].u_int;
    self->step = args[ARG_step].u_int;
    self->access = args[ARG_access].u_int;

    if (param_descriptions == NULL) {
        param_descriptions = mp_obj_new_list(0, NULL);
    }
    mp_obj_list_append(param_descriptions, self);

    return MP_OBJ_FROM_PTR(self);
};


STATIC void modbus_parameter_discriptor_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    modbus_parameter_descriptor_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "Parameter_Descriptor(cid=%d, key=%s, unit=%s, type=%s, reg_start=%u, size=%u, data_type=%s)",
        self->cid, mp_obj_str_get_str(self->param_key), mp_obj_str_get_str(self->param_unit), area_names[self->mb_param_type], self->mb_reg_start, self->mb_size, data_type_names[self->param_type]);
};


// reads a register
STATIC mp_obj_t modbus_parameter_descriptor_read(mp_obj_t self_in) {
    if (!master_running) {
        mp_raise_ValueError(MP_ERROR_TEXT("Modbus Master must be running"));
    }
    modbus_parameter_descriptor_obj_t *self = self_in;
    const mb_parameter_descriptor_t *param_descriptor = NULL;

    
    uint8_t type = 0;

    esp_err_t err = mbc_master_get_cid_info(self->cid, &param_descriptor);

    if ((err != ESP_ERR_NOT_FOUND) && (param_descriptor != NULL)) {
        if ((self->mb_param_type == MB_PARAM_HOLDING) || (self->mb_param_type == MB_PARAM_INPUT)) {
            uint16_t temp_data[self->mb_size];
            memset(temp_data, 0, self->mb_size*2);
            err = mbc_master_get_parameter(param_descriptor->cid, (char *)param_descriptor->param_key, (uint8_t *)temp_data, &type);

            if (err == ESP_OK) {
            
                if (self->param_type == PARAM_TYPE_U8) {
                    return mp_obj_new_int(temp_data[0] & 0xFF);
                } else if (self->param_type == PARAM_TYPE_U16) {
                    return mp_obj_new_int(temp_data[0]);
                } else if (self->param_type == PARAM_TYPE_U32) {
                    return mp_obj_new_int((temp_data[1] << 16) + temp_data[0]);
                } else if (self->param_type == PARAM_TYPE_FLOAT) {
                    float temp_data2 = 0;
                    memcpy(&temp_data2, temp_data, 4);
                    return mp_obj_new_float(temp_data2);
                } else if (self->param_type == PARAM_TYPE_ASCII) {
                    char temp_str[self->mb_size * 2];    // two chars per register
                    strncpy(temp_str, (char *)temp_data, self->mb_size * 2);
                    return mp_obj_new_str(temp_str, MIN(strlen(temp_str), self->mb_size * 2));
                }
            }
        }else{
            // mb_param_type is COIL or DISCRETE -> return bits
            uint8_t num_of_bytes_exp = self->mb_size / 8 + 2;
            uint8_t temp_data[num_of_bytes_exp];
            memset(temp_data, 0, num_of_bytes_exp);

            err = mbc_master_get_parameter(param_descriptor->cid, (char *)param_descriptor->param_key, (uint8_t *)temp_data, &type);

            mp_obj_t list = mp_obj_new_list(0, NULL);
            uint8_t byte_mask = 0;
            uint8_t mask = 0;
            uint8_t first_bit = self->mb_reg_start % 8;
            uint8_t total_bits_processed = 0;
            for (uint8_t i = 0; i < num_of_bytes_exp; i++) {  // for each byte
                for (uint8_t j = first_bit; j < 8; j++) {   // for each bit
                    mask = 1 << j;
                    mp_obj_list_append(list, mp_obj_new_bool((temp_data[i] & mask) > 0));
                    total_bits_processed++;
                    if (total_bits_processed == self->mb_size){
                        return list;
                    }
                }
                first_bit = 0;
            }
            return list;
        }

        ESP_LOGE("modbus", "Characteristic #%d (%s) read fail, err = 0x%x (%s).",
            param_descriptor->cid,
            (char *)param_descriptor->param_key,
            (int)err,
            (char *)esp_err_to_name(err));
        
    } else {
        ESP_LOGE("modbus", "Could not get information for characteristic %d.", param_descriptor->cid);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(modbus_parameter_descriptor_read_obj, modbus_parameter_descriptor_read);

// writes a register
STATIC mp_obj_t modbus_parameter_descriptor_write(mp_obj_t self_in, mp_obj_t data) {
    if (!master_running) {
        mp_raise_ValueError(MP_ERROR_TEXT("Modbus Master must be running"));
    }
    modbus_parameter_descriptor_obj_t *self = self_in;
    const mb_parameter_descriptor_t *param_descriptor = NULL;

    uint16_t temp_data[self->mb_size];
    if ((self->param_type == PARAM_TYPE_U8) || (self->param_type == PARAM_TYPE_U16)) {
        temp_data[0] = (uint16_t)get_uint_checked(data, self->param_type, true);
    } else if (self->param_type == PARAM_TYPE_U32) {
        uint32_t temp_data2 = get_uint_checked(data, self->param_type, true);
        temp_data[1] = (temp_data2 >> 16) & 0xFFFF;
        temp_data[0] = temp_data2 & 0xFFFF;
    } else if (self->param_type == PARAM_TYPE_FLOAT) {
        float temp_data2 = mp_obj_get_float(data);
        memcpy(temp_data, &temp_data2, 4);
    } else if (self->param_type == PARAM_TYPE_ASCII) {
        char *temp_str = mp_obj_str_get_str(data);
        if (strlen(temp_str) > self->mb_size * 2) {
            mp_raise_ValueError("string too long");
        }
        strncpy(temp_data, temp_str, self->mb_size * 2);
    }

    uint8_t type = 0;

    esp_err_t err = mbc_master_get_cid_info(self->cid, &param_descriptor);

    if ((err != ESP_ERR_NOT_FOUND) && (param_descriptor != NULL)) {
        err = mbc_master_set_parameter(param_descriptor->cid, (char *)param_descriptor->param_key, (uint8_t *)temp_data, &type);

        if (err == ESP_OK) {
            return mp_const_none;
        } else {
            ESP_LOGE("modbus", "Characteristic #%d (%s) write fail, err = 0x%x (%s).",
                param_descriptor->cid,
                (char *)param_descriptor->param_key,
                (int)err,
                (char *)esp_err_to_name(err));
        }
    } else {
        ESP_LOGE("modbus", "Could not get information for characteristic %d.", param_descriptor->cid);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(modbus_parameter_descriptor_write_obj, modbus_parameter_descriptor_write);


STATIC void modbus_parameter_discriptor_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    modbus_parameter_descriptor_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (dest[0] == MP_OBJ_NULL) {
        // Load
        if (attr == MP_QSTR_cid) {
            dest[0] = mp_obj_new_int(self->cid);
        } else if (attr == MP_QSTR_param_key) {
            dest[0] = self->param_key;
        } else if (attr == MP_QSTR_param_unit) {
            dest[0] = self->param_unit;
        } else if (attr == MP_QSTR_slave_addr) {
            dest[0] = mp_obj_new_int(self->mb_slave_addr);
        } else if (attr == MP_QSTR_mb_param_type) {
            dest[0] = mp_obj_new_int(self->mb_param_type);
        } else if (attr == MP_QSTR_reg_start) {
            dest[0] = mp_obj_new_int(self->mb_reg_start);
        } else if (attr == MP_QSTR_size) {
            dest[0] = mp_obj_new_int(self->mb_size);
        } else if (attr == MP_QSTR_param_type) {
            dest[0] = mp_obj_new_int(self->param_type);
        } else if (attr == MP_QSTR_min) {
            dest[0] = mp_obj_new_int(self->min);
        } else if (attr == MP_QSTR_max) {
            dest[0] = mp_obj_new_int(self->max);
        } else if (attr == MP_QSTR_step) {
            dest[0] = mp_obj_new_int(self->step);
        } else if (attr == MP_QSTR_read) {
            dest[0] = MP_OBJ_FROM_PTR(&modbus_parameter_descriptor_read_obj);
            dest[1] = self_in;
        } else if (attr == MP_QSTR_write) {
            dest[0] = MP_OBJ_FROM_PTR(&modbus_parameter_descriptor_write_obj);
            dest[1] = self_in;
        }
    } else if (dest[1] != MP_OBJ_NULL) {
        // Store
        mp_raise_ValueError(MP_ERROR_TEXT("Parameter is Read Only"));
    }
};

/**
 * @brief raises a ValueError, if the given var does not fit inside the register area
 * all params are registers, not bytes!
 * @param area_start_register
 * @param area_length
 * @param var_start_register
 * @param var_length
 */
void raise_VauleError_if_outside_area(int16_t area_start_register, int16_t area_length,
    int16_t var_start_register, int16_t var_length) {
    char errmsg[120];

    if (var_length < 0) {
        mp_raise_ValueError("negative length is not allowed");
    }

    if (var_start_register < 0) {
        mp_raise_ValueError("negative register address is not allowed");
    }

    if (var_start_register < area_start_register) {
        snprintf(
            errmsg, 120,
            "register area starts at %d, trying to access it at %d",
            area_start_register, var_start_register
            );
        mp_raise_ValueError(errmsg);
    }

    if (var_start_register + var_length > area_start_register + area_length) {
        snprintf(
            errmsg, 120,
            "register area ends at %d, trying to access it at %d",
            area_start_register + area_length, var_start_register + var_length
            );
        mp_raise_ValueError(errmsg);
    }
}

uint32_t get_uint_checked(mp_obj_t data, uint8_t type, bool raise_exception) {
    if (!mp_obj_is_int(data)){
        if (raise_exception) {
            mp_raise_ValueError("value could not be converted to unsigned integer");
        }
    }
    int32_t value = mp_obj_get_int(data);
    
    bool out_of_range = false;

    out_of_range |= value < 0;
    out_of_range |= (type == PARAM_TYPE_U8) && (value > 0xFF);
    out_of_range |= (type == PARAM_TYPE_U16) && (value > 0xFFFF);
    out_of_range |= (type == PARAM_TYPE_U32) && (value > 0xFFFFFFFF);

    if (out_of_range & raise_exception) {
        mp_raise_ValueError("value out of range");
    }

    return (uint32_t)value;
}

const mp_obj_type_t modbus_register_area_type = {
    { &mp_type_type },
    .name = MP_QSTR_Register_Area,
    .print = modbus_register_area_print,
    .make_new = modbus_register_area_make_new,
    .attr = modbus_register_area_attr,
};

const mp_obj_type_t modbus_parameter_descriptor_type = {
    { &mp_type_type },
    .name = MP_QSTR_Parameter_Descriptor,
    .print = modbus_parameter_discriptor_print,
    .make_new = modbus_parameter_discriptor_make_new,
    .attr = modbus_parameter_discriptor_attr,
};
#endif
