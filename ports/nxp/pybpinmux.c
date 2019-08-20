/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2019 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include "stdint.h"
#include "stdio.h"

#include "py/runtime.h"
#include "py/obj.h"
#include "fsl_iomuxc.h"
#include "pybpinmux.h"

const mp_obj_type_t board_ports_obj_type = {
    {&mp_type_type},
    .name = MP_QSTR_board_ports,
    .locals_dict = (mp_obj_dict_t*)&port_board_pinmux_locals_dict,
};

const port_obj_t* port_find_from_dict(const mp_obj_dict_t *dict_name, mp_obj_t port_name)
{
    const mp_map_t *dict_map = &(dict_name->map);
    mp_map_elem_t * elem = mp_map_lookup((mp_map_t *)dict_map, port_name, MP_MAP_LOOKUP);
    if(elem != NULL && elem->value != MP_OBJ_NULL )
    {
        return MP_OBJ_TO_PTR(elem->value);
    }
    return NULL;
}

const port_obj_t *port_find(mp_obj_t user_obj)
{
    const port_obj_t* port_obj;
    if(MP_OBJ_IS_TYPE(user_obj, &port_type))
    {
        port_obj = MP_OBJ_TO_PTR(user_obj);
    }
    else
    {
        port_obj = port_find_from_dict(&port_board_pinmux_locals_dict, user_obj);
    }
    if(port_obj != NULL)
        return port_obj;
    nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "Port(%s) doesn't exist", mp_obj_str_get_str(user_obj)));
}


STATIC mp_obj_t port_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    mp_arg_check_num(n_args, n_kw, 1, 1, false);
    const port_obj_t *port = port_find(args[0]);
    return MP_OBJ_FROM_PTR(port);
}

STATIC void port_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    port_obj_t* self = MP_OBJ_TO_PTR(self_in);

    mp_printf(print, "Port:%q, Pinmux number: %d.", self->port_name, self->pinmux_num);
}

STATIC mp_obj_t port_list_pinmux(mp_obj_t self_in)
{
    port_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint8_t i = 0;
    mp_obj_t pinmux_list = mp_obj_new_list(0, NULL);
    const pinmux_obj_t *pinmux = self->pinmux;

    for(i=0; i<self->pinmux_num; i++, pinmux++)
    {
        mp_obj_list_append(pinmux_list, MP_OBJ_FROM_PTR(pinmux));
    }
    return pinmux_list;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(list_pinmux_obj, port_list_pinmux);


STATIC const mp_rom_map_elem_t port_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_list_pinmux), MP_ROM_PTR(&list_pinmux_obj) },
};
STATIC MP_DEFINE_CONST_DICT(port_locals_dict, port_locals_dict_table);

const mp_obj_type_t port_type = {
    {&mp_type_type},
    .name = MP_QSTR_Port,
    .make_new = port_make_new,
    .print = port_print,
    .locals_dict = (mp_obj_dict_t*)&port_locals_dict,
};

/* pinmux class */

STATIC void pinmux_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t  kind)
{
    pinmux_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "%q", self->pinmux_name);
}

/* instance method set */
STATIC mp_obj_t pinmux_set(mp_obj_t self_in, mp_obj_t args)
{
    pinmux_obj_t *self = MP_OBJ_TO_PTR(self_in);
    
    uint32_t inputOnfield = mp_obj_get_int(args);
    IOMUXC_SetPinMux(self->muxRegister, self->muxMode, self->inputRegister, self->inputDaisy, self->configRegister, inputOnfield);
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(set_obj, pinmux_set);

/* instance method config */
STATIC mp_obj_t pinmux_config(mp_obj_t self_in, mp_obj_t args)
{
    pinmux_obj_t *self = MP_OBJ_TO_PTR(self_in);

    uint32_t configValue = mp_obj_get_int(args);
    IOMUXC_SetPinConfig(self->muxRegister, self->muxMode, self->inputRegister, self->inputDaisy, self->configRegister, configValue);
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(config_obj, pinmux_config);

STATIC mp_obj_t pinmux_name(mp_obj_t self_in)
{
    pinmux_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_QSTR(self->pinmux_name);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(name_obj, pinmux_name);


STATIC const mp_rom_map_elem_t pinmux_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_Set), MP_ROM_PTR(&set_obj) },
    { MP_ROM_QSTR(MP_QSTR_Config), MP_ROM_PTR(&config_obj) },
    { MP_ROM_QSTR(MP_QSTR_Name), MP_ROM_PTR(&name_obj) },
};

STATIC MP_DEFINE_CONST_DICT(pinmux_dict, pinmux_locals_dict_table);


/* define class pinmux */
const mp_obj_type_t pinmux_type = {
    {&mp_type_type},
    .name = MP_QSTR_pinmux,
    .print = pinmux_print,
    .locals_dict = (mp_obj_dict_t*)&pinmux_dict, 
};
