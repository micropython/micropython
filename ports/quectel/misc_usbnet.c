/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
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

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "mpconfigport.h"
#include "py/obj.h"
#include "py/compile.h"
#include "py/runtime.h"

#if MICROPY_QPY_MISC_USBNET

#include "helios_usbnet.h"
#include "helios_debug.h"

#define HELIOS_MODUSB_LOG(msg, ...) custom_log("modusbnet", msg, ##__VA_ARGS__)

STATIC mp_obj_t misc_usbnet_set_type(mp_obj_t type_in)
{
	int type = mp_obj_get_int(type_in);
	if (type != HELIOS_USBNET_TYPE_ECM && type != HELIOS_USBNET_TYPE_RNDIS)
	{
		HELIOS_MODUSB_LOG("USBNET type must be Type_ECM or Type_RNDIS.");
		return mp_obj_new_int(-1);
	}
	int ret = Helios_USBNET_SetType(type);
	return mp_obj_new_int(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(misc_usbnet_set_type_obj, misc_usbnet_set_type);

STATIC mp_obj_t misc_usbnet_get_type(void)
{
	Helios_USBNET_Type_e type = 0;
	int ret = Helios_USBNET_GetType(&type);
	if (ret == 0)
	{
		ret = type;
	}
	return mp_obj_new_int(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(misc_usbnet_get_type_obj, misc_usbnet_get_type);

STATIC mp_obj_t misc_usbnet_get_status(void)
{
	uint8_t status = 0;
	int ret = Helios_USBNET_GetStatus(&status);
	if (ret == 0)
	{
		ret = status;
	}
	return mp_obj_new_int(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(misc_usbnet_get_status_obj, misc_usbnet_get_status);

STATIC mp_obj_t misc_usbnet_open(void)
{
	int ret = Helios_USBNET_Open();
	return mp_obj_new_int(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(misc_usbnet_open_obj, misc_usbnet_open);

STATIC mp_obj_t misc_usbnet_close(void)
{
	int ret = Helios_USBNET_Close();
	return mp_obj_new_int(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(misc_usbnet_close_obj, misc_usbnet_close);

#if defined(PLAT_Unisoc)
STATIC mp_obj_t misc_usbnet_get_nat(size_t n_args, const mp_obj_t *args)
{
	if (n_args == 2)
	{
		uint8_t sim_id = mp_obj_get_int(args[0]);
		uint8_t profile_id = mp_obj_get_int(args[1]);
		if (sim_id != 0)
		{
			HELIOS_MODUSB_LOG("invalid value, now simId only supports 0.");
            return mp_obj_new_int(-1);
		}
		if (profile_id < 1 || profile_id > 8)
		{
			HELIOS_MODUSB_LOG("invalid value, profileIdx should be in [1,8].");
            return mp_obj_new_int(-1);
		}
		int nat = Helios_USBNET_GetNat(sim_id, profile_id);
		return mp_obj_new_int(nat);
	}
	return mp_obj_new_int(-1);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(misc_usbnet_get_nat_obj, 2, 2, misc_usbnet_get_nat);

STATIC mp_obj_t misc_usbnet_set_nat(size_t n_args, const mp_obj_t *args)
{
	if (n_args == 3)
	{
		uint8_t sim_id = mp_obj_get_int(args[0]);
		uint8_t profile_id = mp_obj_get_int(args[1]);
		uint8_t nat = mp_obj_get_int(args[2]);
		if (sim_id != 0)
		{
			HELIOS_MODUSB_LOG("invalid value, now simId only supports 0.");
            return mp_obj_new_int(-1);
		}
		if (profile_id < 1 || profile_id > 8)
		{
			HELIOS_MODUSB_LOG("invalid value, profileIdx should be in [1,8].");
            return mp_obj_new_int(-1);
		}
		if (nat != 0 && nat != 1)
		{
			HELIOS_MODUSB_LOG("invalid value, Nat should be in [0,1].");
            return mp_obj_new_int(-1);
		}
		int ret = Helios_USBNET_SetNat(sim_id, profile_id, nat);
		return mp_obj_new_int(ret);
	}
	return mp_obj_new_int(-1);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(misc_usbnet_set_nat_obj, 3, 3, misc_usbnet_set_nat);
#endif

#if defined(PLAT_Unisoc_8850)
STATIC mp_obj_t misc_usbnet_set_MAC(mp_obj_t macinfo)
{
	mp_buffer_info_t m_macinfo;
	unsigned char mac[6];
    mp_get_buffer_raise(macinfo, &m_macinfo, MP_BUFFER_READ);
	if( m_macinfo.len != 6 ) {
		HELIOS_MODUSB_LOG("mac length error");
        return mp_obj_new_int(-1);
	}
    memcpy(mac, m_macinfo.buf, 6);
	
	int ret = Helios_USBNET_SetMAC(mac);
	
	return mp_obj_new_int(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(misc_usbnet_set_MAC_obj, misc_usbnet_set_MAC);

STATIC mp_obj_t misc_usbnet_set_product(mp_obj_t macinfo)
{
	mp_buffer_info_t m_product;
	char product[32];
    mp_get_buffer_raise(macinfo, &m_product, MP_BUFFER_READ);
	if( m_product.len > 16 ) {
		HELIOS_MODUSB_LOG("product length error");
        return mp_obj_new_int(-1);
	}
	memset(product, 0x00, sizeof(product));
    strncpy(product, m_product.buf, m_product.len);
	
	int ret = Helios_USBNET_SetProduct(product);
	
	return mp_obj_new_int(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(misc_usbnet_set_product_obj, misc_usbnet_set_product);

#endif

STATIC const mp_rom_map_elem_t misc_usbnet_locals_dict_table[] = {
	{MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_USBNET)},
	{MP_ROM_QSTR(MP_QSTR_set_worktype), MP_ROM_PTR(&misc_usbnet_set_type_obj)},
	{MP_ROM_QSTR(MP_QSTR_get_worktype), MP_ROM_PTR(&misc_usbnet_get_type_obj)},
	{MP_ROM_QSTR(MP_QSTR_get_status), MP_ROM_PTR(&misc_usbnet_get_status_obj)},
	{MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&misc_usbnet_open_obj)},
	{MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&misc_usbnet_close_obj)},
	{MP_ROM_QSTR(MP_QSTR_Type_ECM), MP_ROM_INT(HELIOS_USBNET_TYPE_ECM)},
	{MP_ROM_QSTR(MP_QSTR_Type_RNDIS), MP_ROM_INT(HELIOS_USBNET_TYPE_RNDIS)},
#if defined(PLAT_Unisoc)
	{MP_ROM_QSTR(MP_QSTR_getNat), MP_ROM_PTR(&misc_usbnet_get_nat_obj)},
	{MP_ROM_QSTR(MP_QSTR_setNat), MP_ROM_PTR(&misc_usbnet_set_nat_obj)},
#endif
#if defined(PLAT_Unisoc_8850)
	{MP_ROM_QSTR(MP_QSTR_setMAC), MP_ROM_PTR(&misc_usbnet_set_MAC_obj)},
	{MP_ROM_QSTR(MP_QSTR_setProduct), MP_ROM_PTR(&misc_usbnet_set_product_obj)},
#endif
};

STATIC MP_DEFINE_CONST_DICT(misc_usbnet_locals_dict, misc_usbnet_locals_dict_table);

const mp_obj_module_t misc_usbnet_module = {
	.base = {&mp_type_module},
	.globals = (mp_obj_dict_t *)&misc_usbnet_locals_dict,
};

#endif /* MICROPY_QPY_MISC_USBNET */
