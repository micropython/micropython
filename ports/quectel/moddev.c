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
#include <string.h>
#include "obj.h"
#include "runtime.h"
#include "mphalport.h"

#if MICROPY_QPY_MODULE_MODEM

#include "helios_dev.h"


static mp_obj_t queclib_dev_product_id()
{
	char product_id_str[64] = {0};
	int ret = Helios_Dev_GetPID((void *)product_id_str, sizeof(product_id_str));
	if(ret == 0)
	{
		return mp_obj_new_str(product_id_str, strlen(product_id_str));
	}
	return mp_obj_new_int(-1);
}

static MP_DEFINE_CONST_FUN_OBJ_0(queclib_dev_product_id_obj, queclib_dev_product_id);

extern int _get_current_simid(void);

static mp_obj_t queclib_dev_serial_number(size_t n_args, const mp_obj_t *args)
{
#if !defined(PLAT_aic8800m40) && !defined(PLAT_ECR6600) && !defined(PLAT_SONY_ALT1350)
	int cur_simid = _get_current_simid();
	if (n_args ==1) {
		cur_simid = mp_obj_get_int(args[0]);
	}
	if ((cur_simid != 0) && (cur_simid != 1))
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, simId should be in [0~1]."));
	}
#endif
	char serial_number_str[64] = {0};
	#if MICROPY_QPY_MODULE_DSDS
	int ret = Helios_Dev_GetSN((void *)serial_number_str, sizeof(serial_number_str), cur_simid);
	#else
	int ret = Helios_Dev_GetSN((void *)serial_number_str, sizeof(serial_number_str),0);
	#endif
	if(ret == 0)
	{
		return mp_obj_new_str(serial_number_str, strlen(serial_number_str));
	}
	return mp_obj_new_int(-1);
}

static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(queclib_dev_serial_number_obj, 0, 1, queclib_dev_serial_number);

static mp_obj_t queclib_dev_model()
{
	char model_str[64] = {0};
	int ret = Helios_Dev_GetModel((void *)model_str, sizeof(model_str));
	if(ret == 0)
	{
		return mp_obj_new_str(model_str, strlen(model_str));
	}
	return mp_obj_new_int(-1);
}

static MP_DEFINE_CONST_FUN_OBJ_0(queclib_dev_model_obj, queclib_dev_model);


mp_obj_t queclib_dev_fw_version()
{
	char fw_version_str[64] = {0};
	int ret = Helios_Dev_GetFwVersion((void *)fw_version_str, sizeof(fw_version_str));
	if(ret == 0)
	{
		return mp_obj_new_str(fw_version_str, strlen(fw_version_str));
	}
	return mp_obj_new_int(-1);
}

static MP_DEFINE_CONST_FUN_OBJ_0(queclib_dev_fw_version_obj, queclib_dev_fw_version);


#if !defined(PLAT_aic8800m40) && !defined(PLAT_ECR6600)

static mp_obj_t queclib_dev_imei(size_t n_args, const mp_obj_t *args)
{
	#if !defined(PLAT_SONY_ALT1350)
	int cur_simid = _get_current_simid();
	#else
	int cur_simid = 0;
	#endif
	if (n_args ==1) {
		cur_simid = mp_obj_get_int(args[0]);
	}
	if ((cur_simid != 0) && (cur_simid != 1))
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, simId should be in [0~1]."));
	}
	//the imei length is 15-17 bytes
	char imei_str[19] = {0};
#if MICROPY_QPY_MODULE_DSDS
	int ret = Helios_Dev_GetIMEI((void *)imei_str, sizeof(imei_str), cur_simid);
#else
	#if MICROPY_QPY_MODULE_DUALIMEI
	cur_simid = (n_args ==1) ? cur_simid + 2 : 0;
	int ret = Helios_Dev_GetIMEI((void *)imei_str, sizeof(imei_str),cur_simid);
	#else
	int ret = Helios_Dev_GetIMEI((void *)imei_str, sizeof(imei_str),0);
	#endif
#endif
	if(ret == 0)
	{
		return mp_obj_new_str(imei_str, strlen(imei_str));
	}
	return mp_obj_new_int(-1);
}

static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(queclib_dev_imei_obj, 0, 1, queclib_dev_imei);
#endif

#if defined (PLAT_Qualcomm)
static mp_obj_t queclib_dev_backup()
{
    MP_THREAD_GIL_EXIT();
	int ret = Helios_Dev_Backup();
	MP_THREAD_GIL_ENTER();
	if( ret != 0 ) return mp_obj_new_int(-1);
	return mp_obj_new_int(0);
}

static MP_DEFINE_CONST_FUN_OBJ_0(queclib_dev_backup_obj, queclib_dev_backup);

static mp_obj_t queclib_dev_backup_status()
{
    Helios_DevBackupStatusStruct backup_status_info = {0};
    
	int ret = Helios_Dev_Backup_Status(&backup_status_info);
	if( ret == 0 )
    {
        mp_obj_t info[3] = 
		{
			mp_obj_new_int(backup_status_info.backup_valid),
			mp_obj_new_int(backup_status_info.backup_cnt),
			mp_obj_new_int(backup_status_info.restore_cnt),
		};
		return mp_obj_new_tuple(3, info);
    }
	return mp_obj_new_int(-1);
}

static MP_DEFINE_CONST_FUN_OBJ_0(queclib_dev_backup_status_obj, queclib_dev_backup_status);

static mp_obj_t set_usbmode(mp_obj_t usbmode) {
    int mode = mp_obj_get_int(usbmode);
	extern int Helios_Dev_SetUSBMode(int mode);
    return mp_obj_new_int(Helios_Dev_SetUSBMode(mode));
}

static MP_DEFINE_CONST_FUN_OBJ_1(set_usbmode_obj, set_usbmode);

//forrest.liu@20220112 add for main uart change from AT to general uart
extern void Helios_Dev_Main_UART_Enable_Set(int onoff);
extern int Helios_Dev_Main_UART_Enable_Get(void);

static mp_obj_t main_uart_enable_set(mp_obj_t onoff) {
    int mode = mp_obj_get_int(onoff);
    Helios_Dev_Main_UART_Enable_Set(mode);
	return mp_obj_new_int(0);
}

static MP_DEFINE_CONST_FUN_OBJ_1(main_uart_enable_set_obj, main_uart_enable_set);

static mp_obj_t main_uart_enable_get() {
    int mode = Helios_Dev_Main_UART_Enable_Get();
	return mp_obj_new_int(mode);
}

static MP_DEFINE_CONST_FUN_OBJ_0(main_uart_enable_get_obj, main_uart_enable_get);

#endif

#if defined(PLAT_ECR6600) || defined(PLAT_aic8800m40)

static mp_obj_t queclib_dev_mac()
{
	char serial_number_str[64] = {0};
	int ret = Helios_Dev_GetMAC((void *)serial_number_str, sizeof(serial_number_str));
	sprintf(serial_number_str,"%02x:%02x:%02x:%02x:%02x:%02x",
	serial_number_str[0]&0xff,
	serial_number_str[1]&0xff,
	serial_number_str[2]&0xff,
	serial_number_str[3]&0xff,
	serial_number_str[4]&0xff,
	serial_number_str[5]&0xff);

	if(ret == 0)
	{
		return mp_obj_new_str(serial_number_str, strlen(serial_number_str));
	}

	return mp_obj_new_int(-1);
}

static MP_DEFINE_CONST_FUN_OBJ_0(queclib_dev_mac_obj, queclib_dev_mac);
#endif


#if defined(BOARD_EG915ULA_AC_USBUP) || defined(BOARD_EG915UCN_AC_USBUP) \
	|| defined(BOARD_EG915UEU_AC_USBUP) || defined(BOARD_EG915UEC_AC_USBUP)
int Helios_Dev_SetIMEI(void *buffer);

static mp_obj_t queclib_dev_imei_set(mp_obj_t imei) {
    char *imei_str = (char *)mp_obj_str_get_str(imei);
	MP_THREAD_GIL_EXIT();
    int ret = Helios_Dev_SetIMEI(imei_str);
	MP_THREAD_GIL_ENTER();
	if(ret != 0)
	{
		return mp_obj_new_int(-1);
	}
	return mp_obj_new_int(0);
}

static MP_DEFINE_CONST_FUN_OBJ_1(queclib_dev_imei_set_obj, queclib_dev_imei_set);
#endif

static const mp_rom_map_elem_t mp_module_modem_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_modem) },
#if !defined(PLAT_aic8800m40)
	{ MP_ROM_QSTR(MP_QSTR_getDevSN), MP_ROM_PTR(&queclib_dev_serial_number_obj) },
#endif
#if !defined(PLAT_aic8800m40) && !defined(PLAT_ECR6600)
    { MP_ROM_QSTR(MP_QSTR_getDevImei), MP_ROM_PTR(&queclib_dev_imei_obj) },
#endif
	{ MP_ROM_QSTR(MP_QSTR_getDevModel), MP_ROM_PTR(&queclib_dev_model_obj) },
	{ MP_ROM_QSTR(MP_QSTR_getDevFwVersion), MP_ROM_PTR(&queclib_dev_fw_version_obj) },
	{ MP_ROM_QSTR(MP_QSTR_getDevProductId), MP_ROM_PTR(&queclib_dev_product_id_obj) },

#if defined (PLAT_Qualcomm)
	{ MP_ROM_QSTR(MP_QSTR_backup), MP_ROM_PTR(&queclib_dev_backup_obj) },
	{ MP_ROM_QSTR(MP_QSTR_backupStatus), MP_ROM_PTR(&queclib_dev_backup_status_obj) },
	{ MP_ROM_QSTR(MP_QSTR_setUSBMode), MP_ROM_PTR(&set_usbmode_obj) },
	{ MP_ROM_QSTR(MP_QSTR_main_uart_enable_set), MP_ROM_PTR(&main_uart_enable_set_obj) },
	{ MP_ROM_QSTR(MP_QSTR_main_uart_enable_get), MP_ROM_PTR(&main_uart_enable_get_obj) },
#endif
#if defined(BOARD_EG915ULA_AC_USBUP) || defined(BOARD_EG915UCN_AC_USBUP) \
 	|| defined(BOARD_EG915UEU_AC_USBUP) || defined(BOARD_EG915UEC_AC_USBUP) 
	{ MP_ROM_QSTR(MP_QSTR_setDevImei), MP_ROM_PTR(&queclib_dev_imei_set_obj) },
#endif
#if defined(PLAT_ECR6600) || defined(PLAT_aic8800m40)
    { MP_ROM_QSTR(MP_QSTR_getDevMAC), MP_ROM_PTR(&queclib_dev_mac_obj) },
#endif
};
static MP_DEFINE_CONST_DICT(mp_module_modem_globals, mp_module_modem_globals_table);


const mp_obj_module_t mp_module_modem = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_modem_globals,
};

MP_REGISTER_MODULE(MP_QSTR_modem, mp_module_modem);
#endif /* MICROPY_PORT_BUILTIN_MODULES_MODEM */
