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
#include <stdlib.h>
#include "py/obj.h"
#include <string.h>
#include "py/runtime.h"
#include "mphalport.h"
#include "gccollect.h"


#if MICROPY_QPY_MODULE_FOTA

#include "helios_fota.h"
#include "helios_power.h"
#include "helios_debug.h"
#include "callbackdeal.h"
#include "shared/runtime/pyexec.h"


#define MOD_FOTA_LOG(msg, ...)      custom_log(FOTA, msg, ##__VA_ARGS__)

typedef struct _fota_obj_t {
	mp_obj_base_t base;
	int ctx;
}fota_obj_t;

const mp_obj_type_t mp_fota_type;
static fota_obj_t *fota_self_obj = NULL;

static char *temp_server_address1 = NULL;

extern void Helios_Fota_SslConfig(char *rootCA, char *clientCert, char *clientKey);

#if defined(PLAT_Qualcomm) || defined (PLAT_Unisoc) || defined(PLAT_ASR_1803s) || defined(PLAT_EIGEN) 
extern int Helios_Fota_Download_Cancel(void);
extern int Helios_Fota_Reset_Disable(int flag);
extern int Helios_Fota_APN_Set(int ip_type, char *apn, char *user, char *pass);
#endif

static mp_obj_t fota_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
	if (fota_self_obj == NULL)
    {
    	fota_self_obj = mp_obj_malloc_with_finaliser(fota_obj_t, &mp_fota_type);
    }
	fota_obj_t *self = fota_self_obj;
	
	self->base.type = &mp_fota_type;
	self->ctx = Helios_Fota_Init();

#if defined (PLAT_ASR) || defined (PLAT_Unisoc) || defined(PLAT_Qualcomm) || defined(PLAT_ASR_1803s) || defined(PLAT_EIGEN)
	const byte *key = NULL;
	const byte *cert = NULL;
	const byte *root_cert = NULL;
#if defined(PLAT_Qualcomm) || defined (PLAT_Unisoc) || defined(PLAT_ASR_1803s) || defined(PLAT_EIGEN)
	int reset_disable = 0; 
#endif

	if (n_args > 0 || n_kw > 0) 
	{
		//ssl args
		enum { ARG_key, ARG_cert, ARG_root_cert, ARG_reset_disable};
		
		static const mp_arg_t allowed_args[] = {
			{ MP_QSTR_key, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
			{ MP_QSTR_cert, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
			{ MP_QSTR_root_cert, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
			{ MP_QSTR_reset_disable, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
		};
			
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
		// parse args
    	mp_arg_val_t args_dest[MP_ARRAY_SIZE(allowed_args)];
    	mp_arg_parse_all(n_args, args, &kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args_dest);
		
	    if (args_dest[ARG_key].u_obj != mp_const_none)
		{
	        size_t key_len;
	        key = (const byte *)mp_obj_str_get_data(args_dest[ARG_key].u_obj, &key_len);
	    }
		
		if (args_dest[ARG_cert].u_obj != mp_const_none)
		{
			size_t cert_len;
			cert = (const byte *)mp_obj_str_get_data(args_dest[ARG_cert].u_obj, &cert_len);
		}
		
		if (args_dest[ARG_root_cert].u_obj != mp_const_none)
		{
			size_t root_cert_len;
			root_cert = (const byte *)mp_obj_str_get_data(args_dest[ARG_root_cert].u_obj, &root_cert_len);
		}
	#if defined(PLAT_Qualcomm) || defined (PLAT_Unisoc) || defined(PLAT_ASR_1803s) || defined(PLAT_EIGEN)
		if (args_dest[ARG_reset_disable].u_int != -1)
		{
			reset_disable = args_dest[ARG_reset_disable].u_int;
		}
	#endif
	}
#if defined(PLAT_Qualcomm) || defined (PLAT_Unisoc) || defined(PLAT_ASR_1803s) || defined(PLAT_EIGEN)
	Helios_Fota_Reset_Disable(reset_disable);
#endif
	Helios_Fota_SslConfig((char *)(root_cert), (char *)(cert), (char *)(key));
#endif
	return MP_OBJ_FROM_PTR(self);
}

static mp_obj_t fota_write(size_t n_args, const mp_obj_t *args)
{
	int ret;
	int file_size;
	
	fota_obj_t *self = MP_OBJ_TO_PTR(args[0]);

	if(n_args > 2)
	{
		file_size = mp_obj_get_int(args[2]);
	}
	else
	{
		MOD_FOTA_LOG("*** input param invalid \r\n***");
		return mp_obj_new_int(-1);
	}
	
	mp_buffer_info_t bufinfo;
	mp_get_buffer_raise(args[1], &bufinfo, MP_BUFFER_READ);

	MOD_FOTA_LOG(" buff len : %d   file_size : %d\r\n", bufinfo.len, file_size);

	MP_THREAD_GIL_EXIT();
	ret = Helios_Fota_PackageWrite(self->ctx, bufinfo.buf, bufinfo.len, (size_t)file_size);
	MP_THREAD_GIL_ENTER();

	if(ret)
	{
		MOD_FOTA_LOG("*** fota package write fail ***\r\n");
		return mp_obj_new_int(-1);
	}
	if(ret < 0)
	{
		MOD_FOTA_LOG("*** fota package file read fail ***\r\n");
		return mp_obj_new_int(-1);
	}
	
	return mp_obj_new_int(0);
}
#if !defined(PLAT_RDA)
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(fota_write_obj, 1, 3, fota_write);
#endif


static mp_obj_t fota_flush(const mp_obj_t arg0)
{
	int ret = 0; 
	fota_obj_t *self = MP_OBJ_TO_PTR(arg0);
	
	ret = Helios_Fota_PackageFlush(self->ctx);
	if(ret)
	{
		MOD_FOTA_LOG("*** fota package flush fail ***\r\n");
		return mp_obj_new_int(-1);
	}
	MOD_FOTA_LOG("fota package write done, verifing ...\r\n");
	
	return mp_obj_new_int(0);
}
#if !defined(PLAT_RDA)
static MP_DEFINE_CONST_FUN_OBJ_1(fota_flush_obj, fota_flush);
#endif
static mp_obj_t fota_verify(const mp_obj_t arg0)
{
	int ret = 0; 
	fota_obj_t *self = MP_OBJ_TO_PTR(arg0);
	MP_THREAD_GIL_EXIT();
	ret = Helios_Fota_PackageVerify(self->ctx);
	MP_THREAD_GIL_ENTER();

	if(ret)
	{
		MOD_FOTA_LOG("*** fota package verify fail ***\r\n");
		return mp_obj_new_int(-1);
	}
	MOD_FOTA_LOG("fota package verify done, will restart to update ...\r\n");
	
	return mp_obj_new_int(0);
}
#if !defined(PLAT_RDA)
static MP_DEFINE_CONST_FUN_OBJ_1(fota_verify_obj, fota_verify);
#endif
static c_callback_t *fota_callback = NULL;


static void mpFotaProgressCB(int sta, int progress)
{
    if(fota_callback != NULL){
        MP_THREAD_GIL_ENTER();
        GC_STACKTOP_SET();
		mp_obj_t fota_list[2] = { 											
				mp_obj_new_int(sta),												
				mp_obj_new_int(progress),											
			};	

	    mp_sched_schedule_ex(fota_callback, mp_obj_new_list(2, fota_list));
	    GC_STACKTOP_CLEAR();
	    MP_THREAD_GIL_EXIT();
    }
	
	if(sta == 1)
	{
		MOD_FOTA_LOG("fota test downloading (%d)%d ...\r\n", sta, progress);
	}
	else if(sta == 0)
	{
		MOD_FOTA_LOG("fota test downloading (%d)%d ...\r\n", sta, progress);
	}
	else if(sta == 2)
	{
		MOD_FOTA_LOG("fota test update flag setted, will restart to update ...\r\n");
	#if defined (PLAT_ASR)
		Helios_Power_Reset(1);
	#endif
	}
	else if(sta == -1)
	{
		MOD_FOTA_LOG("fota test download failed (%d)%d\r\n", sta, progress);
		MOD_FOTA_LOG("========== fota test end ==========\r\n");
	}
}

static mp_obj_t fota_get_url()
{
  return mp_obj_new_str(temp_server_address1, strlen(temp_server_address1));
}
#if defined(PLAT_SONY_ALT1350)
static MP_DEFINE_CONST_FUN_OBJ_0(fota_get_url_obj, fota_get_url);
#endif
static mp_obj_t fota_get_obj()
{
  return MP_OBJ_FROM_PTR(fota_self_obj);
}
#if defined(PLAT_SONY_ALT1350)
static MP_DEFINE_CONST_FUN_OBJ_0(fota_get_obj_obj, fota_get_obj);
#endif

static mp_obj_t fota_firmware_download(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
	int ret;
	char *server_address1 = NULL;
	char *server_address2 = NULL;

    enum {
        ARG_url1,
		ARG_url2,	
        ARG_callback,
    };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_url1,         MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_url2,         MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_callback,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };

    mp_arg_val_t args_parse[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args-1, args+1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args_parse);

    if (args_parse[ARG_url1].u_obj != mp_const_none) {
        server_address1 = (char *)(mp_obj_str_get_str(args_parse[ARG_url1].u_obj));
    }

    if (args_parse[ARG_url2].u_obj != mp_const_none) {
        server_address2 = (char *)(mp_obj_str_get_str(args_parse[ARG_url2].u_obj));
    }

    if (args_parse[ARG_callback].u_obj != mp_const_none) {
        static c_callback_t cb = {0};
        memset(&cb, 0, sizeof(c_callback_t));
    	fota_callback = &cb;
    	mp_sched_schedule_callback_register(fota_callback, args_parse[ARG_callback].u_obj);
    }


    MP_THREAD_GIL_EXIT();
	ret  = Helios_Fota_firmware_download(server_address1, server_address2, mpFotaProgressCB);
	MP_THREAD_GIL_ENTER();
#if !defined(PLAT_SONY_ALT1350)	
	if(ret)
	{
		MOD_FOTA_LOG("*** fota firmware download fail ***\r\n");
		return mp_obj_new_int(-1);
	}
	return mp_obj_new_int(0);
#else
	temp_server_address1 = server_address1;
	ret = pyexec_frozen_module("fota_BG950S.py", false);
	if(ret == 0)
	{
		MOD_FOTA_LOG("*** fota execute fail ***\r\n");
		return mp_obj_new_int(-1);
	}
	return mp_obj_new_int(0);
#endif
}

static MP_DEFINE_CONST_FUN_OBJ_KW(fota_firmware_download_obj, 1, fota_firmware_download);

#if defined(PLAT_Qualcomm)
static mp_obj_t fota_download_cancel(const mp_obj_t arg0)
{
	int ret = 0; 
	
	ret = Helios_Fota_Download_Cancel();
	if(ret)
	{
		MOD_FOTA_LOG("*** fota download cancel fail ***\r\n");
		return mp_obj_new_int(-1);
	}
	
	return mp_obj_new_int(0);
}

static MP_DEFINE_CONST_FUN_OBJ_1(fota_download_cancel_obj, fota_download_cancel);

static mp_obj_t fota_apn_set(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args)
{	
	int ret;
	int ip_type = 0;
	char *apn = NULL;
	char *user = NULL;
	char *password = NULL;

    enum {
		ARG_ip_type,
        ARG_apn,
		ARG_user,	
        ARG_password,
    };
    static const mp_arg_t allowed_args[] = {
		{ MP_QSTR_ip_type,           MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_fota_apn,          MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_fota_user,         MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_fota_password,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };

    mp_arg_val_t args_parse[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args-1, args+1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args_parse);

	if (args_parse[ARG_ip_type].u_int != -1)
	{
		ip_type = args_parse[ARG_ip_type].u_int;
	}

    if (args_parse[ARG_apn].u_obj != mp_const_none) {
        apn = (char *)(mp_obj_str_get_str(args_parse[ARG_apn].u_obj));
    }

    if (args_parse[ARG_user].u_obj != mp_const_none) {
        user = (char *)(mp_obj_str_get_str(args_parse[ARG_user].u_obj));
    }

	if (args_parse[ARG_password].u_obj != mp_const_none) {
        password = (char *)(mp_obj_str_get_str(args_parse[ARG_password].u_obj));
    }

	ret  = Helios_Fota_APN_Set(ip_type, apn, user, password);
	
	if(ret)
	{
		return mp_obj_new_int(-1);
	}

	return mp_obj_new_int(0);
}

static MP_DEFINE_CONST_FUN_OBJ_KW(fota_apn_set_obj, 1, fota_apn_set);

#endif

static mp_obj_t fota___del__(mp_obj_t self_in)
{
    fota_obj_t *self = MP_OBJ_TO_PTR(self_in);

	Helios_Fota_Deinit(self->ctx);
    fota_callback = NULL;
	fota_self_obj = NULL;

    return mp_obj_new_int(0);
}
static MP_DEFINE_CONST_FUN_OBJ_1(fota___del___obj, fota___del__);


static const mp_rom_map_elem_t fota_locals_dict_table[] = {
	{ MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&fota___del___obj) },
	{ MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_fota) },
#if defined(PLAT_SONY_ALT1350)
	{ MP_ROM_QSTR(MP_QSTR_fota_get_url), MP_ROM_PTR(&fota_get_url_obj) },
	{ MP_ROM_QSTR(MP_QSTR_fota_get_obj), MP_ROM_PTR(&fota_get_obj_obj) },
#endif
#if !defined(PLAT_RDA)
	{ MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&fota_write_obj) },
	{ MP_ROM_QSTR(MP_QSTR_flush), MP_ROM_PTR(&fota_flush_obj) },
	{ MP_ROM_QSTR(MP_QSTR_verify), MP_ROM_PTR(&fota_verify_obj) },
#endif
	{ MP_ROM_QSTR(MP_QSTR_httpDownload), MP_ROM_PTR(&fota_firmware_download_obj) },
#if defined(PLAT_Qualcomm)
	{ MP_ROM_QSTR(MP_QSTR_download_cancel), MP_ROM_PTR(&fota_download_cancel_obj) },
	{ MP_ROM_QSTR(MP_QSTR_apn_set), MP_ROM_PTR(&fota_apn_set_obj) },
#endif
};
static MP_DEFINE_CONST_DICT(fota_locals_dict, fota_locals_dict_table);


MP_DEFINE_CONST_OBJ_TYPE(
    mp_fota_type,
    MP_QSTR_fota,
    MP_TYPE_FLAG_NONE,
    make_new, fota_make_new,
    locals_dict, &fota_locals_dict
    );
MP_REGISTER_MODULE(MP_QSTR_fota, mp_fota_type);

#endif /* MICROPY_QPY_MODULE_FOTA */
