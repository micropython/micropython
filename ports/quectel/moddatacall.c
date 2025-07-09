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

#include <stdlib.h>
#include <string.h>

#include "nlr.h"
#include "objlist.h"
#include "objstr.h"
#include "runtime.h"
#include "mperrno.h"

#if MICROPY_QPY_MODULE_DATACALL

#include "helios_debug.h"
#include "helios_datacall.h"
#include "callbackdeal.h"

#if MICROPY_QPY_MODULE_WANINFO
#include "netif.h"
#include "ip6_addr.h"
#endif

#define MOD_DATACALL_LOG(msg, ...)      custom_log(DataCall, msg, ##__VA_ARGS__)

extern int _get_current_simid(void);

static mp_obj_t qpy_datacall_set_pdp_context(size_t n_args, const mp_obj_t *args)
{
	int ret = 0;
	int profile_id = mp_obj_get_int(args[0]);
	int ip_type = mp_obj_get_int(args[1]);
	int auth_type = mp_obj_get_int(args[5]);
	
	mp_buffer_info_t apninfo = {0};
	mp_buffer_info_t usrinfo = {0};
	mp_buffer_info_t pwdinfo = {0};
	mp_get_buffer_raise(args[2], &apninfo, MP_BUFFER_READ);
	mp_get_buffer_raise(args[3], &usrinfo, MP_BUFFER_READ);
	mp_get_buffer_raise(args[4], &pwdinfo, MP_BUFFER_READ);

	int min_profile_id = (int)Helios_DataCall_GetProfileIdxMin();
	int max_profile_id = (int)Helios_DataCall_GetProfileIdxMax();
	
	if ((profile_id < min_profile_id) || (profile_id > max_profile_id))
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, profileID should be in [%d,%d]."), min_profile_id, max_profile_id);
	}
	if ((ip_type < 0) || (ip_type > HELIOS_PDP_TYPE_NUM - 1))
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, ipType should be in [0~%d]."), HELIOS_PDP_TYPE_NUM - 1);
	}
	if ((auth_type < 0) || (auth_type > HELIOS_AUTH_TYPE_NUM - 1))
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, authType should be in [0~%d]."), HELIOS_AUTH_TYPE_NUM - 1);
	}
	if (apninfo.len > HELIOS_APN_LEN_MAX)
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, the length of apn should be no more than [%d] bytes."), HELIOS_APN_LEN_MAX);
	}
	if (usrinfo.len > HELIOS_USR_LEN_MAX)
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, the length of username should be no more than [%d] bytes."), HELIOS_USR_LEN_MAX);
	}
	if (pwdinfo.len > HELIOS_PWD_LEN_MAX)
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, the length of password should be no more than [%d] bytes."), HELIOS_PWD_LEN_MAX);
	}
	if (usrinfo.len == 0 && pwdinfo.len == 0 && auth_type != 0) {
		auth_type = 0;
	}

	int cur_simid = _get_current_simid();
	if (n_args ==7) {
		cur_simid = mp_obj_get_int(args[6]);
	}
	if ((cur_simid != 0) && (cur_simid != 1))
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, simId should be in [0~1]."));
	}

	Helios_DataCallStartStruct pdp_context = {0};
	pdp_context.ip_type = ip_type;
	pdp_context.auth = auth_type;
	memcpy((void *)pdp_context.apn, apninfo.buf, apninfo.len);
	memcpy((void *)pdp_context.user, usrinfo.buf, usrinfo.len);
	memcpy((void *)pdp_context.pwd, pwdinfo.buf, pwdinfo.len);
	MP_THREAD_GIL_EXIT();
	#if MICROPY_QPY_MODULE_DSDS
	ret = Helios_DataCall_SetPDPContext(cur_simid,profile_id, &pdp_context);
	#else
	ret = Helios_DataCall_SetPDPContext(profile_id, &pdp_context);
	#endif
	MP_THREAD_GIL_ENTER();
	return mp_obj_new_int(ret);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qpy_datacall_set_pdp_context_obj, 6, 7, qpy_datacall_set_pdp_context);


static mp_obj_t qpy_datacall_get_pdp_context(size_t n_args, const mp_obj_t *args)
{	
	int ret = 0;
	int profile_id = mp_obj_get_int(args[0]);
	Helios_DataCallStartStruct pdp_context = {0};

	int min_profile_id = (int)Helios_DataCall_GetProfileIdxMin();
	int max_profile_id = (int)Helios_DataCall_GetProfileIdxMax();
	
	if ((profile_id < min_profile_id) || (profile_id > max_profile_id))
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, profileID should be in [%d,%d]."), min_profile_id, max_profile_id);
	}
	int cur_simid = _get_current_simid();
	if (n_args ==2) {
		cur_simid = mp_obj_get_int(args[1]);
	}
	if ((cur_simid != 0) && (cur_simid != 1))
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, simId should be in [0~1]."));
	}
	MP_THREAD_GIL_EXIT();
	#if MICROPY_QPY_MODULE_DSDS
	ret = Helios_DataCall_GetPDPContext(cur_simid,profile_id, &pdp_context);
	#else
	ret = Helios_DataCall_GetPDPContext(profile_id, &pdp_context);
	#endif
	MP_THREAD_GIL_ENTER();
	if (ret == 0)
	{
		mp_obj_t pdp_ctx[5] = 
		{
			mp_obj_new_int(pdp_context.ip_type),
			mp_obj_new_str(pdp_context.apn, strlen(pdp_context.apn)),
			mp_obj_new_str(pdp_context.user, strlen(pdp_context.user)),
			mp_obj_new_str(pdp_context.pwd, strlen(pdp_context.pwd)),
			mp_obj_new_int(pdp_context.auth)
		};
		return mp_obj_new_tuple(5, pdp_ctx);
	}
	
	return mp_obj_new_int(-1);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qpy_datacall_get_pdp_context_obj, 1, 2, qpy_datacall_get_pdp_context);

/*=============================================================================*/
/* FUNCTION: qpy_datacall_start                                                */
/*=============================================================================*/
/*!@brief 		: set APN and datacall.
 * @profile_idx	[in] 	profile_idx, 1~HELIOS_PROFILE_IDX_MAX
 * @ip_type		[in] 	0-IPV4, 1-IPV6, 2-IPV4 and IPV6
 * @apn_name	[in] 	anp name
 * @usr_name	[in] 	user name
 * @password	[in] 	password
 * @auth_type	[in] 	auth_type, 0-None, 1-PAP, 2-CHAP
 * @return		:
 *        -  0--successful
 *        - -1--error
 */
/*=============================================================================*/
static mp_obj_t qpy_datacall_start(size_t n_args, const mp_obj_t *args)
{
	int ret = 0;
	int profile_id = mp_obj_get_int(args[0]);
	int ip_type    = mp_obj_get_int(args[1]);
	int auth_type  = mp_obj_get_int(args[5]);
	
	mp_buffer_info_t apninfo = {0};
	mp_buffer_info_t usrinfo = {0};
	mp_buffer_info_t pwdinfo = {0};
	mp_get_buffer_raise(args[2], &apninfo, MP_BUFFER_READ);
	mp_get_buffer_raise(args[3], &usrinfo, MP_BUFFER_READ);
	mp_get_buffer_raise(args[4], &pwdinfo, MP_BUFFER_READ);

	int min_profile_id = (int)HELIOS_PROFILE_IDX_MIN;
	int max_profile_id = (int)HELIOS_PROFILE_IDX_MAX;
	
	if ((profile_id < min_profile_id) || (profile_id > max_profile_id))
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, profileID should be in [%d,%d]."), min_profile_id, max_profile_id);
	}
	if ((ip_type < 0) || (ip_type > HELIOS_PDP_TYPE_NUM - 1))
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, ipType should be in [0~%d]."), HELIOS_PDP_TYPE_NUM - 1);
	}
	if ((auth_type < 0) || (auth_type > HELIOS_AUTH_TYPE_NUM - 1))
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, authType should be in [0~%d]."), HELIOS_AUTH_TYPE_NUM - 1);
	}
	if (apninfo.len > HELIOS_APN_LEN_MAX)
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, the length of apn should be no more than [%d] bytes."), HELIOS_APN_LEN_MAX);
	}
	if (usrinfo.len > HELIOS_USR_LEN_MAX)
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, the length of username should be no more than [%d] bytes."), HELIOS_USR_LEN_MAX);
	}
	if (pwdinfo.len > HELIOS_PWD_LEN_MAX)
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, the length of password should be no more than [%d] bytes."), HELIOS_PWD_LEN_MAX);
	}

	MOD_DATACALL_LOG("[datacall] profile_idx=%d, ip_version=%d, auth_type=%d\r\n", profile_id, ip_type, auth_type);
	MOD_DATACALL_LOG("[datacall] anp_name=%s, usr_name=%s, password=%s\r\n", apninfo.buf, usrinfo.buf, pwdinfo.buf);
	
    Helios_DataCallStartStruct DataCallStartStruct = {0};
    DataCallStartStruct.ip_type = (int32_t)ip_type;
    DataCallStartStruct.auth = (int32_t)auth_type;
    snprintf(DataCallStartStruct.apn, sizeof(DataCallStartStruct.apn), "%s", (char *)apninfo.buf);
    snprintf(DataCallStartStruct.user, sizeof(DataCallStartStruct.user), "%s", (char *)usrinfo.buf);
    snprintf(DataCallStartStruct.pwd, sizeof(DataCallStartStruct.pwd), "%s", (char *)pwdinfo.buf);

    int cur_simid = _get_current_simid();
	if (n_args ==7) {
		cur_simid = mp_obj_get_int(args[6]);
	}
	if ((cur_simid != 0) && (cur_simid != 1))
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, simId should be in [0~1]."));
	}

    MP_THREAD_GIL_EXIT();
    ret = Helios_DataCall_Start(profile_id, cur_simid, &DataCallStartStruct);
    MP_THREAD_GIL_ENTER();
    
	return mp_obj_new_int(ret);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qpy_datacall_start_obj, 5, 7, qpy_datacall_start);


static mp_obj_t qpy_datacall_stop(size_t n_args, const mp_obj_t *args)
{	
	int ret = 0;
	int profile_id = mp_obj_get_int(args[0]);
	int ip_type = mp_obj_get_int(args[1]);
	int cur_simid = _get_current_simid();

	int min_profile_id = (int)HELIOS_PROFILE_IDX_MIN;
	int max_profile_id = (int)HELIOS_PROFILE_IDX_MAX;
	
	if ((profile_id < min_profile_id) || (profile_id > max_profile_id))
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, profileID should be in [%d,%d]."), min_profile_id, max_profile_id);
	}
	if ((ip_type < 0) || (ip_type > 2))
	{
		mp_raise_ValueError(MP_ERROR_TEXT("invalid value, ipTpye should be in [0,2]."));
	}
	if (n_args ==3) {
		cur_simid = mp_obj_get_int(args[2]);
	}
	if ((cur_simid != 0) && (cur_simid != 1))
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, simId should be in [0~1]."));
	}
	MP_THREAD_GIL_EXIT();
	ret = Helios_DataCall_Stop((int32_t)profile_id, cur_simid, (int32_t)ip_type);
	MP_THREAD_GIL_ENTER();
	return mp_obj_new_int(ret);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qpy_datacall_stop_obj, 2, 3, qpy_datacall_stop);


/*=============================================================================*/
/* FUNCTION: qpy_datacall_set_autoconnect                                      */
/*=============================================================================*/
/*!@brief 		: set auto connect
 * @profile_idx	[in] 	profile_idx, 1~7
 * @enable		[in] 	0-disable, 1-enable
 * @return		:
 *        -  0--successful
 *        - -1--error
 */
/*=============================================================================*/
static mp_obj_t qpy_datacall_set_autoconnect(size_t n_args, const mp_obj_t *args)
{	
	int ret = 0;
	int profile_id = mp_obj_get_int(args[0]);
	bool enable = mp_obj_is_true(args[1]);

	int min_profile_id = (int)HELIOS_PROFILE_IDX_MIN;
	int max_profile_id = (int)HELIOS_PROFILE_IDX_MAX;
	
	if ((profile_id < min_profile_id) || (profile_id > max_profile_id))
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, profileID should be in [%d,%d]."), min_profile_id, max_profile_id);
	}
	int cur_simid = _get_current_simid();
	if (n_args ==3) {
		cur_simid = mp_obj_get_int(args[2]);
	}
	if ((cur_simid != 0) && (cur_simid != 1))
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, simId should be in [0~1]."));
	}
	ret = Helios_DataCall_SetAutoConnect((int32_t)profile_id, cur_simid, enable);
	return mp_obj_new_int(ret);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qpy_datacall_set_autoconnect_obj, 1, 3, qpy_datacall_set_autoconnect);


/*=============================================================================*/
/* FUNCTION: qpy_datacall_set_asynmode                                         */
/*=============================================================================*/
/*!@brief 		: Set asynchronous mode to datacall
 * @mode		[in] 	0-disable, 1-enable
 * @return		:
 *        -  0--successful
 *        - -1--error
 */
/*=============================================================================*/
static mp_obj_t qpy_datacall_set_asynmode(mp_obj_t mode)
{	
	bool enable = mp_obj_is_true(mode);
    int cur_simid = _get_current_simid();
    Helios_DataCall_SetAsynMode((int32_t)Helios_DataCall_GetCurrentPDP(), cur_simid, enable);
	return mp_obj_new_int(0);
}
static MP_DEFINE_CONST_FUN_OBJ_1(qpy_datacall_set_asynmode_obj, qpy_datacall_set_asynmode);


static mp_obj_t qpy_datacall_get_info(size_t n_args, const mp_obj_t *args)
{
	int ret = 0;
	int profile_id  = mp_obj_get_int(args[0]);
	int ip_type     = mp_obj_get_int(args[1]);
	char ip4_ip_addr[16] = {0};
	char ip4_pri_dns[16] = {0};
	char ip4_sec_dns[16] = {0};
	char ip6_ip_addr[64] = {0};
	char ip6_pri_dns[64] = {0};
	char ip6_sec_dns[64] = {0};
	Helios_DataCallInfoStruct info = {0};
	int min_profile_id = (int)HELIOS_PROFILE_IDX_MIN;
	int max_profile_id = (int)HELIOS_PROFILE_IDX_MAX;
	
	if ((profile_id < min_profile_id) || (profile_id > max_profile_id))
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, profileID should be in [%d,%d]."), min_profile_id, max_profile_id);
	}
	if ((ip_type < 0) || (ip_type > HELIOS_PDP_TYPE_NUM - 1))
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, ipType should be in [0~%d]."), HELIOS_PDP_TYPE_NUM - 1);
	}
	info.ip_version = ip_type;
	int cur_simid = _get_current_simid();
	if (n_args ==3) {
		cur_simid = mp_obj_get_int(args[2]);
	}
	if ((cur_simid != 0) && (cur_simid != 1))
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, simId should be in [0~1]."));
	}
	MP_THREAD_GIL_EXIT();
	ret = Helios_DataCall_GetInfo(profile_id, cur_simid, &info);
	MP_THREAD_GIL_ENTER();
	if (0 == ret)
	{
		inet_ntop(AF_INET, &info.v4.addr.ip, ip4_ip_addr, sizeof(ip4_ip_addr));
		inet_ntop(AF_INET, &info.v4.addr.pri_dns, ip4_pri_dns, sizeof(ip4_pri_dns));
		inet_ntop(AF_INET, &info.v4.addr.sec_dns, ip4_sec_dns, sizeof(ip4_sec_dns));
		inet_ntop(AF_INET6, &info.v6.addr.ip, ip6_ip_addr, sizeof(ip6_ip_addr));
		inet_ntop(AF_INET6, &info.v6.addr.pri_dns, ip6_pri_dns, sizeof(ip6_pri_dns));
		inet_ntop(AF_INET6, &info.v6.addr.sec_dns, ip6_sec_dns, sizeof(ip6_sec_dns));

		mp_obj_t ip4_list[5] = {
				mp_obj_new_int(info.v4.state),
				mp_obj_new_int(info.v4.reconnect),

				mp_obj_new_str(ip4_ip_addr, strlen(ip4_ip_addr)),
				mp_obj_new_str(ip4_pri_dns, strlen(ip4_pri_dns)),
				mp_obj_new_str(ip4_sec_dns, strlen(ip4_sec_dns)),
			};
		mp_obj_t ip6_list[5] = {
				mp_obj_new_int(info.v6.state),
				mp_obj_new_int(info.v6.reconnect),

				mp_obj_new_str(ip6_ip_addr, strlen(ip6_ip_addr)),
				mp_obj_new_str(ip6_pri_dns, strlen(ip6_pri_dns)),
				mp_obj_new_str(ip6_sec_dns, strlen(ip6_sec_dns)),
			};

		if (ip_type == 0)
		{
			mp_obj_t tuple[3] = {
				mp_obj_new_int(info.profile_idx),
				mp_obj_new_int(info.ip_version),
				mp_obj_new_list(5, ip4_list),
			};
			return mp_obj_new_tuple(3, tuple);
		}
		else if (ip_type == 1)
		{
			mp_obj_t tuple[3] = {
				mp_obj_new_int(info.profile_idx),
				mp_obj_new_int(info.ip_version),
				mp_obj_new_list(5, ip6_list),
			};
			return mp_obj_new_tuple(3, tuple);
		}
		else if (ip_type == 2)
		{
			mp_obj_t tuple[4] = {
				mp_obj_new_int(info.profile_idx),
				mp_obj_new_int(info.ip_version),
				mp_obj_new_list(5, ip4_list),
				mp_obj_new_list(5, ip6_list),
			};
		
			return mp_obj_new_tuple(4, tuple);
		}
	}
	return mp_obj_new_int(ret);
}

static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qpy_datacall_get_info_obj, 1, 3, qpy_datacall_get_info);



static c_callback_t * g_usr_callback = NULL;

static void datacall_callback(int32_t profile_idx, uint8_t sim_id, int32_t nw_status)
{

	MOD_DATACALL_LOG("[datacall] pdp = %d, nwsta = %d\r\n", profile_idx, nw_status);
	if (g_usr_callback)
	{
		st_CallBack_Datacall *Datacall_param = (st_CallBack_Datacall *)malloc(sizeof(st_CallBack_Datacall));
		if(NULL != Datacall_param) 
		{
			Datacall_param->profile_idx = profile_idx;															   
			Datacall_param->nw_status = nw_status;															  
			Datacall_param->sim_id = sim_id;  
			Datacall_param->callback = *g_usr_callback;

			qpy_send_msg_to_callback_deal_thread(CALLBACK_TYPE_ID_DATACALL, Datacall_param);
		}
	}
}


/*=============================================================================*/
/* FUNCTION: qpy_datacall_register_usr_callback                                */
/*=============================================================================*/
/*!@brief 		: register the callback function for user
 * @user_cb		[in] 	callback function
 * @return		:
 *        -  0--successful
 *        - -1--error
 */
/*=============================================================================*/
static mp_obj_t qpy_datacall_register_usr_callback(mp_obj_t callback)
{	
    static c_callback_t cb = {0};
    memset(&cb, 0, sizeof(c_callback_t));
	cb.arg = mp_obj_new_tuple(3, NULL);
	g_usr_callback = &cb;
	mp_sched_schedule_callback_register(g_usr_callback, callback);
	
    Helios_DataCallInitStruct DataCallInitStruct = {datacall_callback};
    int cur_simid = _get_current_simid();
    Helios_DataCall_Init((int32_t)Helios_DataCall_GetCurrentPDP(), cur_simid, &DataCallInitStruct);
	return mp_obj_new_int(0);
}
static MP_DEFINE_CONST_FUN_OBJ_1(qpy_register_usr_callback_obj, qpy_datacall_register_usr_callback);

static mp_obj_t qpy_datacall_get_pdp_range(void)
{	
	uint32_t min = Helios_DataCall_GetProfileIdxMin();
	uint32_t max = Helios_DataCall_GetProfileIdxMax();
	mp_obj_t tuple[2] = 
	{
		mp_obj_new_int(min),
		mp_obj_new_int(max)
	};
		
	return mp_obj_new_tuple(2, tuple);
}
static MP_DEFINE_CONST_FUN_OBJ_0(qpy_get_pdp_range_obj, qpy_datacall_get_pdp_range);

static mp_obj_t qpy_datacall_get_range(mp_obj_t kind)
{
    int para_kind = mp_obj_get_int(kind);

    if (para_kind == 0) //apn
    {
        return mp_obj_new_int(HELIOS_APN_LEN_MAX);
    }
    else if (para_kind == 1) //password
    {
        return mp_obj_new_int(HELIOS_PWD_LEN_MAX);
    }
    else if (para_kind == 2) //usrname
    {
        return mp_obj_new_int(HELIOS_USR_LEN_MAX);
    }
    else if (para_kind == 3) //pid
    {
        return mp_obj_new_int(HELIOS_PROFILE_IDX_MAX);
    }
    else if (para_kind == 4) //iptype
    {
        return mp_obj_new_int(HELIOS_PDP_TYPE_NUM);
    }
    else if (para_kind == 5) //authtype
    {
        return mp_obj_new_int(HELIOS_AUTH_TYPE_NUM);
    }
    else
    {
        return mp_obj_new_int(-1);
    }
}
static MP_DEFINE_CONST_FUN_OBJ_1(qpy_datacall_get_range_obj, qpy_datacall_get_range);

static mp_obj_t qpy_datacall_get_apn(size_t n_args, const mp_obj_t *args)
{
#if defined(PLAT_ASR) || defined(PLAT_Unisoc) || defined(PLAT_ASR_1803s) || defined(PLAT_ASR_1803sc)|| defined(PLAT_ASR_1606) || defined(PLAT_ASR_1609) \
    || defined(PLAT_Unisoc_8910_R05) || defined(PLAT_ASR_1602) || defined(PLAT_Unisoc_8910_R06)
    int sim_id = mp_obj_get_int(args[0]);
    int ret = 0;
    char apn[99+1] = {0};
	if (sim_id != 0)
	{
		mp_raise_ValueError(MP_ERROR_TEXT("invalid simid."));
	}
    if (n_args == 1)
    {
		MP_THREAD_GIL_EXIT();
        ret = Helios_DataCall_GetApn(2, sim_id, apn);
		MP_THREAD_GIL_ENTER();
    	if (ret == 0)
    	{
    		return mp_obj_new_str(apn, strlen(apn));
    	}
    	return mp_obj_new_int(-1);
    }
    else if (n_args == 2)
    {
        int pid = mp_obj_get_int(args[1]);
        MP_THREAD_GIL_EXIT();
        ret = Helios_DataCall_GetApn(3, sim_id, apn, pid);
		MP_THREAD_GIL_ENTER();
    	if (ret == 0)
    	{
    		return mp_obj_new_str(apn, strlen(apn));
    	}
    	return mp_obj_new_int(-1);
    }
    else
    {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid value, The number of parameters cannot be greater than 2."));
    }
#else
    mp_raise_ValueError(MP_ERROR_TEXT("NOT SUPPORT"));
#endif
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qpy_datacall_get_apn_obj, 1, 2, qpy_datacall_get_apn);

#if defined(PLAT_ASR) || defined(PLAT_ASR_1803s) || defined(PLAT_ASR_1803sc) || defined(PLAT_Unisoc_8910_R05) || defined(PLAT_Unisoc_8910_R06)
static mp_obj_t qpy_datacall_set_dns_server(size_t n_args, const mp_obj_t *args)
{
    int profile_id  = mp_obj_get_int(args[0]);
    int sim_id     = mp_obj_get_int(args[1]);

    mp_buffer_info_t new_pri_dns = {0};
    mp_buffer_info_t new_sec_dns = {0};
    mp_get_buffer_raise(args[2], &new_pri_dns, MP_BUFFER_READ);
    mp_get_buffer_raise(args[3], &new_sec_dns, MP_BUFFER_READ);

	int min_profile_id = (int)HELIOS_PROFILE_IDX_MIN;
	int max_profile_id = (int)HELIOS_PROFILE_IDX_MAX;
	
	if ((profile_id < min_profile_id) || (profile_id > max_profile_id))
	{
		mp_raise_msg_varg(&mp_type_ValueError, "invalid value, profileID should be in [%d,%d].", min_profile_id, max_profile_id);
	}

    char new_pri[128];
    char new_sec[128];
    memset(&new_pri, 0, sizeof(new_pri));
    memset(&new_sec, 0, sizeof(new_sec));

    memcpy(new_pri, new_pri_dns.buf, new_pri_dns.len);
    memcpy(new_sec, new_sec_dns.buf, new_sec_dns.len);

    int ret = 0;
	MP_THREAD_GIL_EXIT();
    ret = Helios_DataCall_SetDnsServer(profile_id, sim_id, new_pri, new_sec);
	MP_THREAD_GIL_ENTER();
    return mp_obj_new_int(ret);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qpy_datacall_set_dns_server_obj, 4, 4, qpy_datacall_set_dns_server);
#endif

static mp_obj_t qpy_datacall_get_date_speed(size_t n_args, const mp_obj_t *args)
{
	uint32_t rx = 0, tx = 0;
#if defined(PLAT_ASR_1803s) || defined(PLAT_ASR_1803sc)
	MP_THREAD_GIL_EXIT();
	Helios_DataCall_GetDataSpeed(&rx, &tx);
	MP_THREAD_GIL_ENTER();
#endif
	mp_obj_t date_speed[2] = {
		mp_obj_new_int_from_uint(rx),	//bps
		mp_obj_new_int_from_uint(tx),	//bps
	};
	return mp_obj_new_tuple(2,date_speed);	
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qpy_datacall_get_date_speed_obj, 0, 0, qpy_datacall_get_date_speed);

static mp_obj_t qpy_datacall_get_address_info(size_t n_args, const mp_obj_t *args)
{
    int profile_id  = mp_obj_get_int(args[0]);
    int ip_type = 0;

    if (n_args == 2)
    {
        ip_type = mp_obj_get_int(args[1]);
    }

#if MICROPY_QPY_MODULE_WANINFO
    struct netif* netif_cfg = netif_get_by_cid(profile_id-1);

    if(netif_cfg)
    {
        static uint8_t g_mac[6] = {0};
        g_mac[0] = netif_cfg->hwaddr[0];
        g_mac[1] = netif_cfg->hwaddr[1];
        g_mac[2] = netif_cfg->hwaddr[2];
        g_mac[3] = netif_cfg->hwaddr[3];
        g_mac[4] = netif_cfg->hwaddr[4];
        g_mac[5] = netif_cfg->hwaddr[5];

        char mac[64] = {0};
        sprintf(mac, "%02X-%02X-%02X-%02X-%02X-%02X", g_mac[0], g_mac[1], g_mac[2], g_mac[3], g_mac[4], g_mac[5]);

        if (ip_type == 0)//IPV4
        {
            char mask[16] = {0};
            char gw[16] = {0};
            
            struct sockaddr_in local_v4;

            local_v4.sin_addr.s_addr = netif_cfg->netmask.addr;
            inet_ntop(AF_INET, &local_v4.sin_addr.s_addr, mask, sizeof(mask));

            local_v4.sin_addr.s_addr = netif_cfg->gw.addr;
            inet_ntop(AF_INET, &local_v4.sin_addr.s_addr, gw, sizeof(gw));

            mp_obj_t addrinfo_tuple[3] = {
                mp_obj_new_str(mac, strlen(mac)),
                mp_obj_new_str(mask, strlen(mask)),
                mp_obj_new_str(gw, strlen(gw))
            };

            return mp_obj_new_tuple(3,addrinfo_tuple);
        }
        else if (ip_type == 1)//IPV6
        {
            struct sockaddr_in6 local_v6;
            char ip6_gw[64] = {0};
            ip6_addr_t * ip6_addr_ptr = NULL;

            ip6_addr_ptr = &(netif_cfg->ip6_gw);
            memcpy(&local_v6.sin6_addr, ip6_addr_ptr, sizeof(ip6_addr_t));
            inet_ntop(AF_INET6, &local_v6.sin6_addr, ip6_gw, sizeof(ip6_gw));

            mp_obj_t addrinfo_tuple[2] = {
                mp_obj_new_str(mac, strlen(mac)),
                mp_obj_new_str(ip6_gw, strlen(ip6_gw))
            };

            return mp_obj_new_tuple(2,addrinfo_tuple);
        }
        else if (ip_type == 2)//IPV4&6
        {
            char mask[16] = {0};
            char gw[16] = {0};

            struct sockaddr_in local_v4;

            local_v4.sin_addr.s_addr = netif_cfg->netmask.addr;
            inet_ntop(AF_INET, &local_v4.sin_addr.s_addr, mask, sizeof(mask));

            local_v4.sin_addr.s_addr = netif_cfg->gw.addr;
            inet_ntop(AF_INET, &local_v4.sin_addr.s_addr, gw, sizeof(gw));

            mp_obj_t addrinfo_tuple[3] = {
                mp_obj_new_str(mac, strlen(mac)),
                mp_obj_new_str(mask, strlen(mask)),
                mp_obj_new_str(gw, strlen(gw))
            };

            char ip6_gw[64] = {0};
            struct sockaddr_in6 local_v6;
            ip6_addr_t * ip6_addr_ptr = NULL;

            ip6_addr_ptr = &(netif_cfg->ip6_gw);
            memcpy(&local_v6.sin6_addr, ip6_addr_ptr, sizeof(ip6_addr_t));
            inet_ntop(AF_INET6, &local_v6.sin6_addr, ip6_gw, sizeof(ip6_gw));

            mp_obj_t addr6info_tuple[2] = {
                mp_obj_new_str(mac, strlen(mac)),
                mp_obj_new_str(ip6_gw, strlen(ip6_gw))
            };

            mp_obj_t ipconfig_list[2] = {
                mp_obj_new_tuple(3, addrinfo_tuple),
                mp_obj_new_tuple(2, addr6info_tuple)
            };
            return mp_obj_new_list(2, ipconfig_list);
        }
    }
#else
    (void)profile_id;(void)ip_type;
#endif

    return mp_obj_new_int(-1);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qpy_datacall_get_address_info_obj, 1, 2, qpy_datacall_get_address_info);

static mp_obj_t qpy_datacall_get_sim_info(size_t n_args, const mp_obj_t *args)
{
#if MICROPY_QPY_MODULE_WANINFO
	int sim_id  = mp_obj_get_int(args[0]);
    Helios_DataCallSimUsed info;

    Helios_DataCall_GetSimUsed(sim_id, &info);
    
    mp_obj_t siminfo_tuple[4] = {
        mp_obj_new_int(info.recv_MBytes_sim),
        mp_obj_new_int(info.recv_Bytes_sim),
        mp_obj_new_int(info.used_MBytes_sim),
        mp_obj_new_int(info.used_Bytes_sim)
    };

    return mp_obj_new_tuple(4,siminfo_tuple);
#endif

    return mp_obj_new_int(-1);
}static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qpy_datacall_get_sim_info_obj, 1, 1, qpy_datacall_get_sim_info);

static mp_obj_t qpy_module_datacall_deinit(void)
{
	g_usr_callback = NULL;
    MOD_DATACALL_LOG("module datacall deinit.\r\n");
#if defined(PLAT_ASR) || defined(PLAT_Unisoc) || defined(PLAT_Qualcomm) || defined(PLAT_Unisoc_8910_R05) || defined(PLAT_Unisoc_8910_R06)
	Helios_DataCall_Deinit();
#endif
	return mp_obj_new_int(0);
}
static MP_DEFINE_CONST_FUN_OBJ_0(qpy_module_datacall_deinit_obj, qpy_module_datacall_deinit);


static mp_obj_t qpy_datacall_use_attach_apn(size_t n_args, const mp_obj_t *args)
{
#if defined(PLAT_ASR)
    int ret = 0;
    bool is_using_attach_apn = false;
    if (n_args == 1) {
        is_using_attach_apn = mp_obj_is_true(args[0]);
		MP_THREAD_GIL_EXIT();
        ret = Helios_DataCall_SetIsUseAttachApn(is_using_attach_apn ? 0 : 1);
		MP_THREAD_GIL_ENTER(); 
        return mp_obj_new_int(ret); 
    } else {
		MP_THREAD_GIL_EXIT();
        is_using_attach_apn = Helios_DataCall_GetIsUseAttachApn();
		MP_THREAD_GIL_ENTER();
        return mp_obj_new_bool(!is_using_attach_apn); 
    }
#else
	 mp_raise_ValueError(MP_ERROR_TEXT("NOT SUPPORT"));
#endif
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qpy_datacall_use_attach_apn_obj, 0, 1, qpy_datacall_use_attach_apn);


static const mp_rom_map_elem_t mp_module_datacall_globals_table[] = {
	{ MP_ROM_QSTR(MP_QSTR___name__),		MP_ROM_QSTR(MP_QSTR_dial) },
	{ MP_ROM_QSTR(MP_QSTR___qpy_module_deinit__),   MP_ROM_PTR(&qpy_module_datacall_deinit_obj) },
	{ MP_ROM_QSTR(MP_QSTR_setPDPContext),	MP_ROM_PTR(&qpy_datacall_set_pdp_context_obj) },
	{ MP_ROM_QSTR(MP_QSTR_getPDPContext),	MP_ROM_PTR(&qpy_datacall_get_pdp_context_obj) },
	{ MP_ROM_QSTR(MP_QSTR_start),			MP_ROM_PTR(&qpy_datacall_start_obj) },
	{ MP_ROM_QSTR(MP_QSTR_stop),			MP_ROM_PTR(&qpy_datacall_stop_obj) },
	{ MP_ROM_QSTR(MP_QSTR_setAutoConnect),	MP_ROM_PTR(&qpy_datacall_set_autoconnect_obj) },
	{ MP_ROM_QSTR(MP_QSTR_getInfo),			MP_ROM_PTR(&qpy_datacall_get_info_obj) },
	{ MP_ROM_QSTR(MP_QSTR_setCallback),		MP_ROM_PTR(&qpy_register_usr_callback_obj) },
	{ MP_ROM_QSTR(MP_QSTR_setAsynMode),		MP_ROM_PTR(&qpy_datacall_set_asynmode_obj) },
	{ MP_ROM_QSTR(MP_QSTR_getPdpRange),		MP_ROM_PTR(&qpy_get_pdp_range_obj) },
	{ MP_ROM_QSTR(MP_QSTR_getRange),		MP_ROM_PTR(&qpy_datacall_get_range_obj) },
	{ MP_ROM_QSTR(MP_QSTR_getApn), 	        MP_ROM_PTR(&qpy_datacall_get_apn_obj) },
	#if defined(PLAT_ASR) || defined(PLAT_ASR_1803s) || defined(PLAT_ASR_1803sc) || defined(PLAT_Unisoc_8910_R05) || defined(PLAT_Unisoc_8910_R06)
	{ MP_ROM_QSTR(MP_QSTR_setDnsserver),    MP_ROM_PTR(&qpy_datacall_set_dns_server_obj) },
	#endif
	{ MP_ROM_QSTR(MP_QSTR_useAttachApn),	MP_ROM_PTR(&qpy_datacall_use_attach_apn_obj) },
	{ MP_ROM_QSTR(MP_QSTR_getSpeed),    	MP_ROM_PTR(&qpy_datacall_get_date_speed_obj) },
	{ MP_ROM_QSTR(MP_QSTR_getAddressinfo),  MP_ROM_PTR(&qpy_datacall_get_address_info_obj) },
	{ MP_ROM_QSTR(MP_QSTR_getSiminfo),      MP_ROM_PTR(&qpy_datacall_get_sim_info_obj) },
};
static MP_DEFINE_CONST_DICT(mp_module_datacall_globals, mp_module_datacall_globals_table);


const mp_obj_module_t mp_module_dial = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_datacall_globals,
};

MP_REGISTER_MODULE(MP_QSTR_dial, mp_module_dial);
#endif /* MICROPY_QPY_MODULE_DATACALL */
