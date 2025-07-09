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

#include "stdio.h"
#include "stdlib.h"
#include "obj.h"
#include "runtime.h"
#include "mphalport.h"

#if MICROPY_QPY_MODULE_SIM

#include "helios_debug.h"
#include "helios_sim.h"

#if MICROPY_QPY_MODULE_ESIM_SIM_SWITCH
#include "rt_lpa.h"
#include "rt_port_at.h"
#include "rt_utils.h"

#define MAX_EID_HEX_LEN  16
#define MAX_EID_LEN      33
#define THE_MAX_CARD_NUM 20
#endif

#define QPY_MODSIM_LOG(msg, ...)      custom_log("SIM", msg, ##__VA_ARGS__)


static void toUpperCase(char *str) 
{
	while (*str != '\0') {
		if(*str >='a' && *str <= 'z') {
			*str -= 32;
		}
		str++;
	}
}

int _get_current_simid(void)
{
    int cur_simid = 0;
    
#if defined(PLAT_ASR) ||defined(PLAT_ASR_1606) || defined(PLAT_ASR_1609) || defined(PLAT_Unisoc_8850_R02) || defined(PLAT_EIGEN)  || defined(PLAT_ASR_1602) || defined(PLAT_EIGEN_718)

	uint8_t sim_id = 0;
	MP_THREAD_GIL_EXIT();
	Helios_SIM_GetCurrentSimid(&sim_id);
	MP_THREAD_GIL_ENTER();
	cur_simid = sim_id;
#elif defined(PLAT_Unisoc) || defined(PLAT_Unisoc_8850)
	cur_simid = 0;
#endif

	if(cur_simid < 0)
	{
		cur_simid = 0;
	}

    return cur_simid;
}
/*=============================================================================*/
/* FUNCTION: qpy_sim_get_imsi                                                  */
/*=============================================================================*/
/*!@brief: Get the IMSI of the SIM card
 *	
 * @return:
 *     if get successfully, return the IMSI
 *    -1	-	error
 */
/*=============================================================================*/
static mp_obj_t qpy_sim_get_imsi(size_t n_args, const mp_obj_t *args)
{
	int cur_simid = 0;
	char imsi_str[HELIOS_SIM_IMSI_LEN+1] = {0};
    uint8_t status = 0;
    int ret = 0;
	if ( n_args > 0  )
	{
		cur_simid = mp_obj_get_int(args[0]);
	}else{
		cur_simid = _get_current_simid();
	}
	if ((cur_simid != 0) && (cur_simid != 1))
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, simId should be in [0~1]."));
	}
	MP_THREAD_GIL_EXIT();
    Helios_SIM_GetCardStatus(cur_simid, (Helios_SIM_Status_e *)&status);
	MP_THREAD_GIL_ENTER();
    if (ret == 0)
    {
        if (status != 1)
        {
            return mp_obj_new_int(-1);
        }
    }
    else
    {
        return mp_obj_new_int(-1);
    }
    MP_THREAD_GIL_EXIT();
    ret = Helios_SIM_GetIMSI(cur_simid, (void *)imsi_str, sizeof(imsi_str));
	MP_THREAD_GIL_ENTER();
	if (ret == 0)
	{
		return mp_obj_new_str(imsi_str, strlen(imsi_str));
	}
	
	return mp_obj_new_int(-1);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qpy_sim_get_imsi_obj, 0, 1, qpy_sim_get_imsi);


/*=============================================================================*/
/* FUNCTION: qpy_sim_get_iccid                                                 */
/*=============================================================================*/
/*!@brief: Get the ICCID of the SIM card
 *	
 * @return:
 *     if get successfully, return the ICCID
 *    -1	-	error
 */
/*=============================================================================*/
static mp_obj_t qpy_sim_get_iccid(size_t n_args, const mp_obj_t *args)
{
	char iccid_str[HELIOS_SIM_ICCID_LEN+1] = {0};
	uint8_t status = 0;
	int ret = 0;
	int cur_simid = 0;
	if ( n_args > 0  )
	{
		cur_simid = mp_obj_get_int(args[0]);
	}else{
		cur_simid = _get_current_simid();
	}
	if ((cur_simid != 0) && (cur_simid != 1))
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, simId should be in [0~1]."));
	}
    MP_THREAD_GIL_EXIT();
    Helios_SIM_GetCardStatus(cur_simid, (Helios_SIM_Status_e *)&status);
	MP_THREAD_GIL_ENTER();
    if (ret == 0)
    {
        if (status != 1)
        {
            return mp_obj_new_int(-1);
        }
    }
    else
    {
        return mp_obj_new_int(-1);
    }
	MP_THREAD_GIL_EXIT();
    ret = Helios_SIM_GetICCID(cur_simid, (void *)iccid_str, sizeof(iccid_str));
	MP_THREAD_GIL_ENTER();
    if (ret == 0)
    {
		toUpperCase(iccid_str);
        return mp_obj_new_str(iccid_str, strlen(iccid_str));
    }
    
	return mp_obj_new_int(-1);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qpy_sim_get_iccid_obj, 0, 1, qpy_sim_get_iccid);


/*=============================================================================*/
/* FUNCTION: qpy_sim_get_phonenumber                                           */
/*=============================================================================*/
/*!@brief: Get the phone number of the SIM card
 *	
 * @return:
 *     if get successfully, return the phone number
 *    -1	-	error
 */
/*=============================================================================*/
static mp_obj_t qpy_sim_get_phonenumber(size_t n_args, const mp_obj_t *args)
{
	char phone_number[HELIOS_SIM_PHONENUM_LEN+1] = {0};
	int ret = 0;
	int cur_simid = 0;
	if ( n_args > 0  )
	{
		cur_simid = mp_obj_get_int(args[0]);
	}else{
		cur_simid = _get_current_simid();
	}
	if ((cur_simid != 0) && (cur_simid != 1))
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, simId should be in [0~1]."));
	}
    MP_THREAD_GIL_EXIT();
	ret = Helios_SIM_GetPhoneNumber(cur_simid, (void *)phone_number, sizeof(phone_number));
	MP_THREAD_GIL_ENTER();
	if (ret == 0)
	{
		return mp_obj_new_str(phone_number, strlen(phone_number));
	}

	return mp_obj_new_int(-1);
}

#if !defined (PLAT_RDA)
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qpy_sim_get_phonenumber_obj, 0, 1, qpy_sim_get_phonenumber);
#endif
/*=============================================================================*/
/* FUNCTION: qpy_sim_enable_pin                                                */
/*=============================================================================*/
/*!@brief: enable SIM card PIN code verification, and restart will take effect
 *	
 * @pin		[in] 	PIN code
 * @return:
 *     0	-	success
 *    -1	-	error
 */
/*=============================================================================*/
#ifndef MICROPY_QPY_MODULE_NO_SIMPIN
static mp_obj_t qpy_sim_enable_pin(size_t n_args, const mp_obj_t *args)
{
	Helios_SIMPinInfoStruct info = {0};
	mp_buffer_info_t bufinfo = {0};
	int cur_simid = 0;
	if ( n_args > 1  )
	{
		cur_simid = mp_obj_get_int(args[1]);
	}else{
		cur_simid = _get_current_simid();
	}
	if ((cur_simid != 0) && (cur_simid != 1))
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, simId should be in [0~1]."));
	}
	mp_get_buffer_raise(args[0], &bufinfo, MP_BUFFER_READ);
    if (bufinfo.len > HELIOS_SIM_PIN_LEN_MAX)
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, the length of pin should be no more than [%d] bytes."), HELIOS_SIM_PIN_LEN_MAX);
	}
	
	strncpy((char *)info.pin, (const char *)bufinfo.buf, bufinfo.len);

    
	MP_THREAD_GIL_EXIT();
	int ret = Helios_SIM_PINEnable(cur_simid, &info);
	MP_THREAD_GIL_ENTER();
	if (ret == 0)
	{
		return mp_obj_new_int(ret);
	}
	
	return mp_obj_new_int(-1);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qpy_sim_enable_pin_obj, 1, 2, qpy_sim_enable_pin);


/*=============================================================================*/
/* FUNCTION: qpy_sim_disable_pin                                               */
/*=============================================================================*/
/*!@brief: disable SIM card PIN code verification
 *	
 * @pin		[in] 	PIN code
 * @return:
 *     0	-	success
 *    -1	-	error
 */
/*=============================================================================*/
static mp_obj_t qpy_sim_disable_pin(size_t n_args, const mp_obj_t *args)
{
	Helios_SIMPinInfoStruct info = {0};
	mp_buffer_info_t bufinfo = {0};
	int cur_simid = 0;
	if ( n_args > 1  )
	{
		cur_simid = mp_obj_get_int(args[1]);
	}else{
		cur_simid = _get_current_simid();
	}
	
	mp_get_buffer_raise(args[0], &bufinfo, MP_BUFFER_READ);
	if (bufinfo.len > HELIOS_SIM_PIN_LEN_MAX)
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, the length of pin should be no more than [%d] bytes."), HELIOS_SIM_PIN_LEN_MAX);
	}
	if ((cur_simid != 0) && (cur_simid != 1))
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, simId should be in [0~1]."));
	}
	strncpy((char *)info.pin, (const char *)bufinfo.buf, bufinfo.len);
    
	MP_THREAD_GIL_EXIT();
	int ret = Helios_SIM_PINDisable(cur_simid, &info);
	MP_THREAD_GIL_ENTER();
	if (ret == 0)
	{
		return mp_obj_new_int(ret);
	}
	
	return mp_obj_new_int(-1);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qpy_sim_disable_pin_obj, 1, 2, qpy_sim_disable_pin);


/*=============================================================================*/
/* FUNCTION: qpy_sim_verify_pin                                                */
/*=============================================================================*/
/*!@brief: when the SIM state is requested PIN/PIN2, enter the PIN/PIN2 code to verify
 *	
 * @pin		[in] 	PIN code
 * @return:
 *     0	-	success
 *    -1	-	error
 */
/*=============================================================================*/
static mp_obj_t qpy_sim_verify_pin(size_t n_args, const mp_obj_t *args)
{
	Helios_SIMPinInfoStruct info = {0};
	mp_buffer_info_t bufinfo = {0};
 	int cur_simid = 0;
	if ( n_args > 1  )
	{
		cur_simid = mp_obj_get_int(args[1]);
	}else{
		cur_simid = _get_current_simid();
	}
	mp_get_buffer_raise(args[0], &bufinfo, MP_BUFFER_READ);

	if (bufinfo.len > HELIOS_SIM_PIN_LEN_MAX)
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, the length of pin should be no more than [%d] bytes."), HELIOS_SIM_PIN_LEN_MAX);
	}
	if ((cur_simid != 0) && (cur_simid != 1))
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, simId should be in [0~1]."));
	}
	strncpy((char *)info.pin, (const char *)bufinfo.buf, bufinfo.len);

	MP_THREAD_GIL_EXIT();
	int ret = Helios_SIM_PINVerify(cur_simid, &info);
	MP_THREAD_GIL_ENTER();
	if (ret == 0)
	{
		return mp_obj_new_int(ret);
	}
	
	return mp_obj_new_int(-1);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qpy_sim_verify_pin_obj, 1, 2,qpy_sim_verify_pin);

/*=============================================================================*/
/* FUNCTION: qpy_sim_change_pin                                                */
/*=============================================================================*/
/*!@brief: After enabling SIM card PIN verification, change the SIM card PIN
 *	
 * @old_pin		[in] 	old PIN
 * @new_pin		[in] 	new PIN
 * @return:
 *     0	-	success
 *    -1	-	error
 */
/*=============================================================================*/

static mp_obj_t qpy_sim_change_pin(size_t n_args, const mp_obj_t *args)
{
	Helios_SIMChangePinInfoStruct info = {0};
	mp_buffer_info_t bufinfo[2] = {0};
	int cur_simid = 0;
	if ( n_args > 2  )
	{
		cur_simid = mp_obj_get_int(args[2]);
	}else{
		cur_simid = _get_current_simid();
	}
	mp_get_buffer_raise(args[0], &bufinfo[0], MP_BUFFER_READ);
 	mp_get_buffer_raise(args[1], &bufinfo[1], MP_BUFFER_READ);

	if ((bufinfo[0].len > HELIOS_SIM_PIN_LEN_MAX) || (bufinfo[1].len > HELIOS_SIM_PIN_LEN_MAX))
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, the length of pin should be no more than [%d] bytes."), HELIOS_SIM_PIN_LEN_MAX);
	}
	if ((cur_simid != 0) && (cur_simid != 1))
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, simId should be in [0~1]."));
	}
 	strncpy((char *)info.old_pin, (const char *)bufinfo[0].buf, bufinfo[0].len);
 	strncpy((char *)info.new_pin, (const char *)bufinfo[1].buf, bufinfo[1].len);

	MP_THREAD_GIL_EXIT();
	int ret = Helios_SIM_PINChange(cur_simid, &info);
	MP_THREAD_GIL_ENTER();
	if (ret == 0)
	{
		return mp_obj_new_int(ret);
	}
	
	return mp_obj_new_int(-1);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qpy_sim_change_pin_obj, 2,3,qpy_sim_change_pin);


/*=============================================================================*/
/* FUNCTION: qpy_sim_unblock_pin                                               */
/*=============================================================================*/
/*!@brief: When the SIM card status is requested PUK/PUK2 after multiple incorrect input
 *		   of PIN/PIN2 code, input PUK/PUK2 code and a new PIN/PIN2 code to unlock
 *	
 * @puk			[in] 	PUK/PUK2
 * @new_pin		[in] 	new PIN/PIN2
 * @return:
 *     0	-	success
 *    -1	-	error
 */
/*=============================================================================*/
static mp_obj_t qpy_sim_unblock_pin(size_t n_args, const mp_obj_t *args)
{
	Helios_SIMUnlockPinInfoStruct info = {0};
	mp_buffer_info_t bufinfo[2] = {0};
	
	int cur_simid = 0;
	if ( n_args > 2  )
	{
		cur_simid = mp_obj_get_int(args[2]);
	}else{
		cur_simid = _get_current_simid();

	}
	mp_get_buffer_raise(args[0], &bufinfo[0], MP_BUFFER_READ);


	mp_get_buffer_raise(args[1], &bufinfo[1], MP_BUFFER_READ);

    
    if ((bufinfo[0].len > HELIOS_SIM_PIN_LEN_MAX) || (bufinfo[1].len > HELIOS_SIM_PIN_LEN_MAX))
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, the length of pin or puk should be no more than [%d] bytes."), HELIOS_SIM_PIN_LEN_MAX);
	}
	if ((cur_simid != 0) && (cur_simid != 1))
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, simId should be in [0~1]."));
	}
 	strncpy((char *)info.puk, (const char *)bufinfo[0].buf, bufinfo[0].len);
 	strncpy((char *)info.new_pin, (const char *)bufinfo[1].buf, bufinfo[1].len);

	MP_THREAD_GIL_EXIT();
	int ret = Helios_SIM_PINUnlock(cur_simid, &info);
	MP_THREAD_GIL_ENTER();
	if (ret == 0)
	{
		return mp_obj_new_int(ret);
	}
	
	return mp_obj_new_int(-1);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qpy_sim_unblock_pin_obj, 2, 3, qpy_sim_unblock_pin);
#endif

#if MICROPY_QPY_MODULE_PIN_REMATTEMPTS

static mp_obj_t qpy_sim_pin_remain_attempts(size_t n_args, const mp_obj_t *args)
{
	Helios_SIMRemainPinAttempts info = {0};
	
	uint8_t cur_simid = _get_current_simid();
	MP_THREAD_GIL_EXIT();
	int ret = Helios_SIM_PINRemainAttempts(cur_simid, &info);
	MP_THREAD_GIL_ENTER();
	if (ret == 0)
	{
		if (n_args > 0)
		{
			int flag = mp_obj_get_int(args[0]);
			if (flag == 0)
			{
				mp_obj_t tuple[2] = {
					mp_obj_new_int(info.pin_remain_attempts), 
					mp_obj_new_int(info.puk_remain_attempts)
				};
				return mp_obj_new_tuple(2, tuple);
			}
			else if (flag == 1)
			{
				mp_obj_t tuple[4] = {
					mp_obj_new_int(info.pin_remain_attempts), 
					mp_obj_new_int(info.puk_remain_attempts),
					mp_obj_new_int(info.pin2_remain_attempts), 
					mp_obj_new_int(info.puk2_remain_attempts)
				};
				return mp_obj_new_tuple(4, tuple);
			}
			else
			{
				mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, flag should be in [0,1]."));
			}
		}
		else
		{
			mp_obj_t tuple[2] = {
				mp_obj_new_int(info.pin_remain_attempts), 
				mp_obj_new_int(info.puk_remain_attempts)
				};
			return mp_obj_new_tuple(2, tuple);
		}
	}
	return mp_obj_new_int(-1);

}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qpy_sim_pin_remain_attempts_obj, 0, 1, qpy_sim_pin_remain_attempts);

#endif
/*=============================================================================*/
/* FUNCTION: sim_get_card_status                                               */
/*=============================================================================*/
/*!@brief: Get the status of the SIM card.
 *
 * @return :
 *        - 0		SIM was removed.
 *	      - 1		SIM is ready.
 *	      - 2		Expecting the universal PIN./SIM is locked, waiting for a CHV1 password.
 *	      - 3		Expecting code to unblock the universal PIN./SIM is blocked, CHV1 unblocking password is required.
 *	      - 4		SIM is locked due to a SIM/USIM personalization check failure.
 *	      - 5		SIM is blocked due to an incorrect PCK; an MEP unblocking password is required.
 *	      - 6		Expecting key for hidden phone book entries.
 *	      - 7		Expecting code to unblock the hidden key.
 *	      - 8		SIM is locked; waiting for a CHV2 password.
 *	      - 9		SIM is blocked; CHV2 unblocking password is required.
 *	      - 10		SIM is locked due to a network personalization check failure.
 *	      - 11		SIM is blocked due to an incorrect NCK; an MEP unblocking password is required.
 *	      - 12		
SIM is locked due to a network subset personalization check failure.
 *	      - 13		SIM is blocked due to an incorrect NSCK; an MEP unblocking password is required.
 *	      - 14		SIM is locked due to a service provider personalization check failure.
 *	      - 15		SIM is blocked due to an incorrect SPCK; an MEP unblocking password is required.
 *	      - 16		SIM is locked due to a corporate personalization check failure.
 *	      - 17		SIM is blocked due to an incorrect CCK; an MEP unblocking password is required.
 *	      - 18		SIM is being initialized; waiting for completion.
 *	      - 19		Use of CHV1/CHV2/universal PIN/code to unblock the CHV1/code to unblock the CHV2/code to unblock the universal PIN/ is blocked.
 *	      - 20		Unuseful status.
 *		  - 21		Unknow status.
 */
 /*=============================================================================*/

static mp_obj_t qpy_sim_get_card_status(size_t n_args, const mp_obj_t *args)
{
	Helios_SIM_Status_e status = 0;
    int ret = 0;
    int cur_simid = 0 ;
	if ( n_args > 0 )
	{
		cur_simid = (uint8_t)mp_obj_get_int(args[0]);
	}else{
		cur_simid = _get_current_simid();
	}
	if ((cur_simid != 0) && (cur_simid != 1))
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, simId should be in [0~1]."));
	}
    MP_THREAD_GIL_EXIT();
    ret = Helios_SIM_GetCardStatus(cur_simid, &status);
	MP_THREAD_GIL_ENTER();
	if (ret == 0)
	{
		return mp_obj_new_int(status);
	}
	return mp_obj_new_int(-1);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qpy_sim_get_card_status_obj, 0, 1, qpy_sim_get_card_status);

#if MICROPY_QPY_MODULE_PHB
static mp_obj_t qpy_sim_get_phonebookstatus(void)
{
	uint8_t pb_ready_flag = 0;
	MP_THREAD_GIL_EXIT();
    Helios_SIM_GetPbReady(&pb_ready_flag);
	MP_THREAD_GIL_ENTER();
	return mp_obj_new_int(pb_ready_flag);
}
static MP_DEFINE_CONST_FUN_OBJ_0(qpy_sim_get_phonebookstatus_obj, qpy_sim_get_phonebookstatus);
/*=============================================================================*/
/* FUNCTION: qpy_sim_read_phonebook_record                                 */
/*=============================================================================*/
/*!@brief : Read the phone book.
 *
 * @args[1]		[in]	the storage position of the phone book.
 * @args[2]		[in]	start_index
 * @args[3]		[in]	end_index
 * @args[4]		[in]	username
 * @return :
 *     -1	-	error
 * If it reads successfully, the results are returned in the following format.
 * (record_number, [(index, username, phone_number), ... , (index, username, phone_number)])
 * For example:
 *	(2, [(1, 'zhangsan', '18122483511'), (2, 'lisi', '18122483542')])
 */
/*=============================================================================*/

static mp_obj_t qpy_sim_read_phonebook_record(size_t n_args, const mp_obj_t *args)
{
	uint8_t i = 0;
	int32_t storage = 0;
	int cur_simid = 0;
	Helios_SIMReadPhoneBookInfoStruct records = {0};
	mp_buffer_info_t nameinfo = {0};
	mp_obj_t list_records = mp_obj_new_list(0, NULL);

	if ( n_args > 4 ){
		cur_simid = mp_obj_get_int(args[4]);
	}else{
    	cur_simid = _get_current_simid();
	}
	storage = mp_obj_get_int(args[0]);
	records.start_index = mp_obj_get_int(args[1]);
	records.end_index   = mp_obj_get_int(args[2]);
	mp_get_buffer_raise(args[3], &nameinfo, MP_BUFFER_READ);

	if ((storage < 0) || (storage > 15))
	{
		mp_raise_ValueError(MP_ERROR_TEXT("invalid value, storage should be in (0,15)."));
	}
	if (records.end_index < records.start_index)
	{
		mp_raise_ValueError(MP_ERROR_TEXT("invalid value, end >= start."));
	}
	if ((records.end_index - records.start_index) > 20)
	{
		mp_raise_ValueError(MP_ERROR_TEXT("invalid value, end - start <= 20."));
	}
	if ((cur_simid != 0) && (cur_simid != 1))
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, simId should be in [0~1]."));
	}
	if (nameinfo.len > 30)
	{
		mp_raise_ValueError(MP_ERROR_TEXT("invalid value, the length of username should be no more than 30 bytes."));
	}
	records.user_name = (char *)nameinfo.buf;
    
	MP_THREAD_GIL_EXIT();
	int ret = Helios_SIM_ReadPhonebookRecord(cur_simid, storage, &records);
	MP_THREAD_GIL_ENTER();
	if (ret == 0)
	{
		for (i=0; i<records.phonebook.count; ++i)
		{
			mp_obj_t record_info[3] = 
			{
				mp_obj_new_int(records.phonebook.records[i].index),
				mp_obj_new_str(records.phonebook.records[i].user_name, strlen(records.phonebook.records[i].user_name)),
				mp_obj_new_str(records.phonebook.records[i].phone_num, strlen(records.phonebook.records[i].phone_num)),
			};
			mp_obj_list_append(list_records, mp_obj_new_tuple(3, record_info));
		}

		mp_obj_t tuple_records[2] = 
		{
			mp_obj_new_int(records.phonebook.count),
			list_records,
		};

		return mp_obj_new_tuple(2, tuple_records);
	}
	return mp_obj_new_int(-1);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qpy_sim_read_phonebook_record_obj, 3, 5, qpy_sim_read_phonebook_record);

/*=============================================================================*/
/* FUNCTION: qpy_sim_write_phonebook_record                                */
/*=============================================================================*/
/*!@brief 		: Write the information of phone number to the phone book.
 * @args[1]		[in]	the storage position of the phone book.
 * @args[2]		[in]	the record index in phone book.
 * @args[3]		[in]	username
 * @args[4]		[in]	Phone number, it can include '+'.
 * @return		:
 *        -  0--success
 *        - -1--error
 */
/*=============================================================================*/

static mp_obj_t qpy_sim_write_phonebook_record(size_t n_args, const mp_obj_t *args)
{
	int32_t storage = 0;
	mp_buffer_info_t bufinfo[2] = {0};
	Helios_SIMPhoneInfoStruct record = {0};
	int cur_simid = 0;
	if( n_args > 4 ){
		cur_simid =  mp_obj_get_int(args[4]);
	} else{
		cur_simid = _get_current_simid();
	}
	storage = mp_obj_get_int(args[0]);
	record.index = mp_obj_get_int(args[1]);
	mp_get_buffer_raise(args[2], &bufinfo[0], MP_BUFFER_READ);
	mp_get_buffer_raise(args[3], &bufinfo[1], MP_BUFFER_READ);
	
	if ((storage < 0) || (storage > 15))
	{
		mp_raise_ValueError(MP_ERROR_TEXT("invalid value, storage should be in (0,15)."));
	}
	if ((record.index < 1) || (record.index > 500))
	{
		mp_raise_ValueError(MP_ERROR_TEXT("invalid value, index should be in (1,500)."));
	}
	if (bufinfo[0].len > 30)
	{
		mp_raise_ValueError(MP_ERROR_TEXT("invalid value, the length of username should be no more than 30 bytes."));
	}
	if (bufinfo[1].len > 20)
	{
		mp_raise_ValueError(MP_ERROR_TEXT("invalid value, the length of phonenumber should be no more than 20 bytes."));
	}
	if ((cur_simid != 0) && (cur_simid != 1))
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, simId should be in [0~1]."));
	}
 	strncpy(record.user_name, bufinfo[0].buf, bufinfo[0].len);
 	strncpy(record.phone_num, bufinfo[1].buf, bufinfo[1].len);

   
	MP_THREAD_GIL_EXIT();
	int ret = Helios_SIM_WritePhonebookRecord(cur_simid, storage, &record);
	MP_THREAD_GIL_ENTER();
 	if (ret == 0)
 	{
		return mp_obj_new_int(0);
 	}
 	return mp_obj_new_int(-1);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qpy_sim_write_phonebook_record_obj, 3,5, qpy_sim_write_phonebook_record);

#endif


#if MICROPY_QPY_MODULE_SIMDET
static mp_obj_t qpy_sim_set_simdet(size_t n_args, const mp_obj_t *args)
{
	int ret = 0;
	int cur_simid = 0;

	int detenable = 0;
	int insertlevel = 0;
	if ( n_args > 2 ){
		cur_simid = mp_obj_get_int(args[2]);
	}else{
		cur_simid = _get_current_simid();
	}
	detenable = mp_obj_get_int(args[0]);
	insertlevel = mp_obj_get_int(args[1]);

	if (detenable != 0 && detenable != 1)
	{
        mp_raise_ValueError(MP_ERROR_TEXT("invalid value, detenable should be in (0,1)."));
	}
    if (insertlevel != 0 && insertlevel != 1)
	{
        mp_raise_ValueError(MP_ERROR_TEXT("invalid value, insertlevel should be in (0,1)."));
	}
	if ((cur_simid != 0) && (cur_simid != 1))
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, simId should be in [0~1]."));
	}
	MP_THREAD_GIL_EXIT();
    ret = Helios_SIM_SetSimDet(cur_simid, detenable, insertlevel);
	MP_THREAD_GIL_ENTER();
    if (ret == 0)
	{
		return mp_obj_new_int(ret);
	}
	
	return mp_obj_new_int(-1);
}

static mp_obj_t qpy_sim_get_simdet(size_t n_args, const mp_obj_t *args)
{
	int ret = 0;
	int detenable = 99;
    int insertlevel = 99;
	int cur_simid  = 0;
	if ( n_args > 0 ){
		cur_simid = mp_obj_get_int(args[0]);
	}else{
		cur_simid = _get_current_simid();
	}
	if ((cur_simid != 0) && (cur_simid != 1))
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, simId should be in [0~1]."));
	}
	MP_THREAD_GIL_EXIT();
    ret = Helios_SIM_GetSimDet(cur_simid, &detenable, &insertlevel);
	MP_THREAD_GIL_ENTER();
    if (ret == 0)
	{
	    mp_obj_t tuple[2] = 
		{
			mp_obj_new_int(detenable),
			mp_obj_new_int(insertlevel),
		};
		return mp_obj_new_tuple(2, tuple);
	}
	
	return mp_obj_new_int(-1);
}

static c_callback_t *g_sim_user_callback;

static void qpy_sim_event_handler(uint8_t sim_id, unsigned int event_id, void *ctx)
{
	if(g_sim_user_callback)
	{
	    QPY_MODSIM_LOG("[SIM] callback start.\r\n");
	    GC_STACKTOP_SET();
	    mp_sched_schedule_ex(g_sim_user_callback, mp_obj_new_int(event_id));
	    GC_STACKTOP_CLEAR();
	    QPY_MODSIM_LOG("[SIM] callback end.\r\n");
	}
}

static mp_obj_t qpy_sim_add_event_handler(mp_obj_t handler)
{
    static c_callback_t cb = {0};
    memset(&cb, 0, sizeof(c_callback_t));
    g_sim_user_callback = &cb;
    mp_sched_schedule_callback_register(g_sim_user_callback, handler);
	MP_THREAD_GIL_EXIT();
	Helios_SIM_Add_Event_Handler(qpy_sim_event_handler);
	MP_THREAD_GIL_ENTER();
	return mp_obj_new_int(0);
}

static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qpy_sim_set_simdet_obj, 2, 3, qpy_sim_set_simdet);

static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qpy_sim_get_simdet_obj, 0, 1, qpy_sim_get_simdet);

static MP_DEFINE_CONST_FUN_OBJ_1(qpy_sim_add_event_handler_obj, qpy_sim_add_event_handler);
#endif

#if defined(PLAT_ASR)
static mp_obj_t qpy_sim_genericaccess(const mp_obj_t sim_id, const mp_obj_t cmd)
{
	Helios_SIMGenericAccesStruct info = {0};
    int simid = mp_obj_get_int(sim_id);
	mp_buffer_info_t bufinfo = {0};
 	mp_get_buffer_raise(cmd, &bufinfo, MP_BUFFER_READ);

	if (bufinfo.len <= 0)
	{
		mp_raise_ValueError(MP_ERROR_TEXT("invalid value, the length of cmd should be more than 0 bytes."));
	}

	if (simid != 0)
	{
		mp_raise_ValueError(MP_ERROR_TEXT("invalid simid."));
	}

 	strncpy((char *)info.cmd, (const char *)bufinfo.buf, bufinfo.len);
 	info.len = bufinfo.len;
	MP_THREAD_GIL_EXIT();
	int ret = Helios_SIM_GenericAccess(simid, &info);
	MP_THREAD_GIL_ENTER();
	if (ret == 0)
	{
	    mp_obj_t tuple[2] = 
		{
			mp_obj_new_int(strlen(info.resp)),
			mp_obj_new_str(info.resp, strlen(info.resp)),
		};
		return mp_obj_new_tuple(2, tuple);
		//return mp_obj_new_str(info.resp, strlen(info.resp));
	}
	return mp_obj_new_int(-1);
}
static MP_DEFINE_CONST_FUN_OBJ_2(qpy_sim_genericaccess_obj, qpy_sim_genericaccess);
#endif

#if MICROPY_QPY_MODULE_DUALSIM || MICROPY_QPY_MODULE_DSDS
static c_callback_t * g_switchsim_done_callback;

static void switch_sim_done_callback(uint8_t state)
{		
	if (g_switchsim_done_callback)
	{
	    GC_STACKTOP_SET();
		mp_sched_schedule_ex(g_switchsim_done_callback, mp_obj_new_int(state));
		GC_STACKTOP_CLEAR();
	}
}

static mp_obj_t qpy_sim_set_switchcard_cb(mp_obj_t switchsim_callback)
{
	static c_callback_t cb = {0};
    memset(&cb, 0, sizeof(c_callback_t));
	g_switchsim_done_callback = &cb;
	mp_sched_schedule_callback_register(g_switchsim_done_callback, switchsim_callback);
	
	return mp_obj_new_int(0);
}
static MP_DEFINE_CONST_FUN_OBJ_1(qpy_sim_set_switchcard_cb_obj, qpy_sim_set_switchcard_cb);

static mp_obj_t qpy_sim_switch_card(mp_obj_t sim_id)
{
    uint8_t simid = mp_obj_get_int(sim_id);
	if ((simid != 0) && (simid != 1))
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, simid should be in [0,1]."));
	}
#if MICROPY_QPY_MODULE_DUALSIM
	uint8_t old_simid = HELIOS_SIM_0;
	MP_THREAD_GIL_EXIT();
	Helios_SIM_GetCurrentSimid(&old_simid);
	MP_THREAD_GIL_ENTER();
	if (simid == old_simid)
	{
		return mp_obj_new_int(-1);
	}
#endif
	MP_THREAD_GIL_EXIT();
	int ret = Helios_SIM_SwitchCard(simid, switch_sim_done_callback);
	MP_THREAD_GIL_ENTER();
	return mp_obj_new_int(ret);
}
static MP_DEFINE_CONST_FUN_OBJ_1(qpy_sim_switch_card_obj, qpy_sim_switch_card);

static mp_obj_t qpy_sim_get_current_simid()
{
	uint8_t simid = HELIOS_SIM_0;
	MP_THREAD_GIL_EXIT();
	Helios_SIM_GetCurrentSimid(&simid);
	MP_THREAD_GIL_ENTER();
	return mp_obj_new_int(simid);
}
static MP_DEFINE_CONST_FUN_OBJ_0(qpy_sim_get_current_simid_obj, qpy_sim_get_current_simid);

#if defined(BOARD_EC800MCN_LE_VOLVGL) || defined(BOARD_EC800MCN_LE_CPE) || defined(BOARD_EG810MEU_LA_VOLVGL)
static mp_obj_t qpy_sim_is_insert(mp_obj_t sim_id)
{
    int simid = mp_obj_get_int(sim_id);
	if ((simid != 0) && (simid != 1))
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, simid should be in [0,1]."));
	}
	uint8_t insert_sta = 0;
	MP_THREAD_GIL_EXIT();
	Helios_SIM_GetInsertStatus(simid, &insert_sta);
	MP_THREAD_GIL_ENTER();
	return mp_obj_new_bool(insert_sta);
}
static MP_DEFINE_CONST_FUN_OBJ_1(qpy_sim_is_insert_obj, qpy_sim_is_insert);
#endif

#endif

static mp_obj_t qpy_module_sim_deinit(void)
{
    #if MICROPY_QPY_MODULE_DUALSIM || MICROPY_QPY_MODULE_DSDS
    g_switchsim_done_callback = NULL;
    #endif

	#if MICROPY_QPY_MODULE_SIMDET
    g_sim_user_callback = NULL;
	MP_THREAD_GIL_EXIT();
    Helios_SIM_Add_Event_Handler(NULL);
	MP_THREAD_GIL_ENTER();
    #endif
	
	return mp_obj_new_int(0);
}

static MP_DEFINE_CONST_FUN_OBJ_0(qpy_module_sim_deinit_obj, qpy_module_sim_deinit);

#if MICROPY_QPY_MODULE_ESIM_SIM_SWITCH
static mp_obj_t qpy_sim_get_esim_list(void)
{
	int ret = 0;
	u08 profile_num =0;
	profile_info_t card_info[THE_MAX_CARD_NUM] = {0};
	mp_obj_t list_profile = mp_obj_new_list(0, NULL);
	MP_THREAD_GIL_EXIT();
	ret = rt_lpa_get_profile_info(card_info, &profile_num, THE_MAX_CARD_NUM);
	MP_THREAD_GIL_ENTER();
	if (ret == 0)
    {
        int i = 0;
		for (i = 0; i < profile_num; i++)
		{
            mp_obj_t profile_info[3] = {
    			mp_obj_new_str(card_info[i].iccid, strlen(card_info[i].iccid)),
    			mp_obj_new_int(card_info[i].state),
    			mp_obj_new_int(card_info[i].class)};
    		mp_obj_list_append(list_profile, mp_obj_new_tuple(3, profile_info));
		}
    }
	mp_obj_t tuple[2] = 
	{
		mp_obj_new_int(profile_num),
		list_profile,
	};
	return mp_obj_new_tuple(2, tuple);
}
static MP_DEFINE_CONST_FUN_OBJ_0(qpy_sim_get_esim_list_obj, qpy_sim_get_esim_list);


static mp_obj_t qpy_sim_get_esim_eid(void)
{
	char eid_buf[MAX_EID_HEX_LEN] = {0};
	char eid[MAX_EID_LEN] = {0};
    int ret = -1;
	MP_THREAD_GIL_EXIT();
    ret = rt_lpa_get_eid((u08 *)eid_buf);
    rt_utils_bytes_to_hex((cpu08)eid_buf, sizeof(eid_buf), eid);
	MP_THREAD_GIL_ENTER();
	if (ret != 0)
	{
		memset(eid, 0, sizeof(eid));
	}
    return mp_obj_new_str(eid, strlen(eid));
}
static MP_DEFINE_CONST_FUN_OBJ_0(qpy_sim_get_esim_eid_obj, qpy_sim_get_esim_eid);

static mp_obj_t qpy_sim_enable_esim(const mp_obj_t iccid)
{
    int ret = -1;
    mp_buffer_info_t qpy_iccid = {0};
    mp_get_buffer_raise(iccid, &qpy_iccid, MP_BUFFER_READ);

	if (qpy_iccid.len != 20)
	{
		mp_raise_ValueError(MP_ERROR_TEXT("invalid value, the length of iccid should be 20 bytes."));
	}
	MP_THREAD_GIL_EXIT();
	ret = rt_lpa_enable_profile(qpy_iccid.buf);
	MP_THREAD_GIL_ENTER();
	return mp_obj_new_int(ret);
}
static MP_DEFINE_CONST_FUN_OBJ_1(qpy_sim_enable_esim_obj, qpy_sim_enable_esim);
#endif

#if MICROPY_QPY_MODULE_ESIM
extern const struct _mp_obj_module_t mp_module_esim;
#endif

static const mp_rom_map_elem_t mp_module_sim_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), 		MP_ROM_QSTR(MP_QSTR_sim) 			},
    
#if MICROPY_QPY_MODULE_ESIM
    { MP_ROM_QSTR(MP_QSTR_esim), MP_ROM_PTR(&mp_module_esim) },
#endif

    { MP_ROM_QSTR(MP_QSTR_getImsi), 		MP_ROM_PTR(&qpy_sim_get_imsi_obj) 		},
	{ MP_ROM_QSTR(MP_QSTR_getIccid), 		MP_ROM_PTR(&qpy_sim_get_iccid_obj) 		},

#ifndef MICROPY_QPY_MODULE_NO_SIMPIN
	{ MP_ROM_QSTR(MP_QSTR_verifyPin), 		MP_ROM_PTR(&qpy_sim_verify_pin_obj) 	},
	{ MP_ROM_QSTR(MP_QSTR_changePin), 		MP_ROM_PTR(&qpy_sim_change_pin_obj) 	},
	{ MP_ROM_QSTR(MP_QSTR_unblockPin), 		MP_ROM_PTR(&qpy_sim_unblock_pin_obj) 	},

	{ MP_ROM_QSTR(MP_QSTR_enablePin), 		MP_ROM_PTR(&qpy_sim_enable_pin_obj) 	},
	{ MP_ROM_QSTR(MP_QSTR_disablePin), 		MP_ROM_PTR(&qpy_sim_disable_pin_obj) 	},
#endif

	{ MP_ROM_QSTR(MP_QSTR_getStatus), 		MP_ROM_PTR(&qpy_sim_get_card_status_obj)},
#if MICROPY_QPY_MODULE_PHB
    { MP_ROM_QSTR(MP_QSTR_getPhonebookStatus),       MP_ROM_PTR(&qpy_sim_get_phonebookstatus_obj) },
	{ MP_ROM_QSTR(MP_QSTR_readPhonebook),   MP_ROM_PTR(&qpy_sim_read_phonebook_record_obj) },
	{ MP_ROM_QSTR(MP_QSTR_writePhonebook),  MP_ROM_PTR(&qpy_sim_write_phonebook_record_obj) },
#endif
#if defined (PLAT_ASR)
	{ MP_ROM_QSTR(MP_QSTR_genericAccess),  MP_ROM_PTR(&qpy_sim_genericaccess_obj) },
#endif
#if MICROPY_QPY_MODULE_DUALSIM || MICROPY_QPY_MODULE_DSDS
	{ MP_ROM_QSTR(MP_QSTR_switchCard),      MP_ROM_PTR(&qpy_sim_switch_card_obj) },
	{ MP_ROM_QSTR(MP_QSTR_setSwitchcardCallback),      MP_ROM_PTR(&qpy_sim_set_switchcard_cb_obj) },
	{ MP_ROM_QSTR(MP_QSTR_getCurSimid),     MP_ROM_PTR(&qpy_sim_get_current_simid_obj) },
#if defined(BOARD_EC800MCN_LE_VOLVGL) || defined(BOARD_EC800MCN_LE_CPE) || defined(BOARD_EG810MEU_LA_VOLVGL)
	{ MP_ROM_QSTR(MP_QSTR_isInsert),     MP_ROM_PTR(&qpy_sim_is_insert_obj) },
#endif
#endif
#if MICROPY_QPY_MODULE_SIMDET
    { MP_ROM_QSTR(MP_QSTR_setSimDet),       MP_ROM_PTR(&qpy_sim_set_simdet_obj) },
	{ MP_ROM_QSTR(MP_QSTR_getSimDet),       MP_ROM_PTR(&qpy_sim_get_simdet_obj) },
	{ MP_ROM_QSTR(MP_QSTR_setCallback),     MP_ROM_PTR(&qpy_sim_add_event_handler_obj) },
#endif

    { MP_ROM_QSTR(MP_QSTR___qpy_module_deinit__),   MP_ROM_PTR(&qpy_module_sim_deinit_obj) },

#if !defined (PLAT_RDA)
	{ MP_ROM_QSTR(MP_QSTR_getPhoneNumber), 	MP_ROM_PTR(&qpy_sim_get_phonenumber_obj)},
#endif
#if MICROPY_QPY_MODULE_PIN_REMATTEMPTS
	{ MP_ROM_QSTR(MP_QSTR_getPinRemAttempts),	MP_ROM_PTR(&qpy_sim_pin_remain_attempts_obj) 	},
#endif
#if MICROPY_QPY_MODULE_ESIM_SIM_SWITCH
	{ MP_ROM_QSTR(MP_QSTR_eSimGetList),	MP_ROM_PTR(&qpy_sim_get_esim_list_obj) 	},
	{ MP_ROM_QSTR(MP_QSTR_eSimGetEid),	MP_ROM_PTR(&qpy_sim_get_esim_eid_obj) 	},
	{ MP_ROM_QSTR(MP_QSTR_eSimSwitch),	MP_ROM_PTR(&qpy_sim_enable_esim_obj) 	},
#endif
};
static MP_DEFINE_CONST_DICT(mp_module_sim_globals, mp_module_sim_globals_table);


const mp_obj_module_t mp_module_sim = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_sim_globals,
};

MP_REGISTER_MODULE(MP_QSTR_sim, mp_module_sim);
#endif


