#include <stdio.h>
#include <stdint.h>

#include <stm32f4xx_hal.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "modstm.h"

STATIC uint32_t get_read_addr(mp_obj_t addr_o, uint align) {
    uint32_t addr = mp_obj_get_int(addr_o) & 0x7fffffff;
    if (addr < 0x10000000) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "cannot read from address %08x", addr));
    }
    if ((addr & (align - 1)) != 0) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "address %08x is not aligned to %d bytes", addr, align));
    }
    return addr;
}

STATIC uint32_t get_write_addr(mp_obj_t addr_o, uint align) {
    uint32_t addr = mp_obj_get_int(addr_o) & 0x7fffffff;
    if (addr < 0x10000000) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "cannot write to address %08x", addr));
    }
    if ((addr & (align - 1)) != 0) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "address %08x is not aligned to %d bytes", addr, align));
    }
    return addr;
}

STATIC mp_obj_t stm_read8(mp_obj_t addr) {
    uint32_t a = get_read_addr(addr, 1);
    uint32_t v = *(uint8_t*)a;
    return mp_obj_new_int(v);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(stm_read8_obj, stm_read8);

STATIC mp_obj_t stm_read16(mp_obj_t addr) {
    uint32_t a = get_read_addr(addr, 2);
    uint32_t v = *(uint16_t*)a;
    return mp_obj_new_int(v);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(stm_read16_obj, stm_read16);

STATIC mp_obj_t stm_read32(mp_obj_t addr) {
    uint32_t a = get_read_addr(addr, 4);
    uint32_t v = *(uint32_t*)a;
    return mp_obj_new_int(v);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(stm_read32_obj, stm_read32);

STATIC mp_obj_t stm_write8(mp_obj_t addr, mp_obj_t val) {
    uint32_t a = get_write_addr(addr, 1);
    uint32_t v = mp_obj_get_int(val);
    *(uint8_t*)a = v;
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_2(stm_write8_obj, stm_write8);

STATIC mp_obj_t stm_write16(mp_obj_t addr, mp_obj_t val) {
    uint32_t a = get_write_addr(addr, 2);
    uint32_t v = mp_obj_get_int(val);
    *(uint16_t*)a = v;
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_2(stm_write16_obj, stm_write16);

STATIC mp_obj_t stm_write32(mp_obj_t addr, mp_obj_t val) {
    uint32_t a = get_write_addr(addr, 4);
    uint32_t v = mp_obj_get_int(val);
    *(uint32_t*)a = v;
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_2(stm_write32_obj, stm_write32);

STATIC const mp_map_elem_t stm_module_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_stm) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_read8), (mp_obj_t)&stm_read8_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_read16), (mp_obj_t)&stm_read16_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_read32), (mp_obj_t)&stm_read32_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_write8), (mp_obj_t)&stm_write8_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_write16), (mp_obj_t)&stm_write16_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_write32), (mp_obj_t)&stm_write32_obj },

    { MP_OBJ_NEW_QSTR(MP_QSTR_GPIOA), MP_OBJ_NEW_SMALL_INT(GPIOA_BASE) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_GPIOB), MP_OBJ_NEW_SMALL_INT(GPIOB_BASE) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_GPIOC), MP_OBJ_NEW_SMALL_INT(GPIOC_BASE) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_GPIOD), MP_OBJ_NEW_SMALL_INT(GPIOD_BASE) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_GPIO_IDR), MP_OBJ_NEW_SMALL_INT(0x10) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_GPIO_BSRRL), MP_OBJ_NEW_SMALL_INT(0x18) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_GPIO_BSRRH), MP_OBJ_NEW_SMALL_INT(0x1a) },
};

STATIC const mp_obj_dict_t stm_module_globals = {
    .base = {&mp_type_dict},
    .map = {
        .all_keys_are_qstrs = 1,
        .table_is_fixed_array = 1,
        .used = sizeof(stm_module_globals_table) / sizeof(mp_map_elem_t),
        .alloc = sizeof(stm_module_globals_table) / sizeof(mp_map_elem_t),
        .table = (mp_map_elem_t*)stm_module_globals_table,
    },
};

const mp_obj_module_t stm_module = {
    .base = { &mp_type_module },
    .name = MP_QSTR_stm,
    .globals = (mp_obj_dict_t*)&stm_module_globals,
};
