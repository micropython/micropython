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

#include "obj.h"
#include "runtime.h"
#include "mphalport.h"
#include "callbackdeal.h"
#include "gccollect.h"

#if MICROPY_QPY_MODULE_NET

#include "helios_debug.h"
#include "helios_nw.h"
#include "helios_dev.h"
#include "helios_sim.h"
#include "helios_datacall.h"
#include "helios_atcmd.h"

#define QPY_NET_LOG(msg, ...)      custom_log(modnet, msg, ##__VA_ARGS__)

extern int _get_current_simid(void);
/*=============================================================================*/
/* FUNCTION: qpy_net_set_mode                                                  */
/*=============================================================================*/
/*!@brief: set network mode
 *	
 * @mode	[in] 	network mode
 * @roaming	[in] 	enable or disable roaming
 *
 * @return:
 *     0	-	success
 *    -1	-	error
 */
/*=============================================================================*/
static mp_obj_t qpy_net_set_configuration(size_t n_args, const mp_obj_t *args)
{
	int mode;
	int roaming;
	int ret = 0;
	Helios_NwConfigInfoStruct config_info;
    memset(&config_info, 0, sizeof(Helios_NwConfigInfoStruct));

	mode = mp_obj_get_int(args[0]);
	QPY_NET_LOG("[network] set config, mode=%d\r\n", mode);
    
    int cur_simid = _get_current_simid();
    
	if (n_args == 2)
	{
		roaming = mp_obj_get_int(args[1]);
		if((roaming != 0) && (roaming != 1))
		{
			QPY_NET_LOG("[network] invalid roaming value, roaming=%d\r\n", roaming);
			return mp_obj_new_int(-1);
		}
		config_info.roaming_switch = roaming;
		QPY_NET_LOG("[network] set config, roaming=%d\r\n", roaming);
	}
	if (n_args ==3) 
	{
		cur_simid = mp_obj_get_int(args[2]);
	}
	if ((cur_simid != 0) && (cur_simid != 1))
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, simId should be in [0~1]."));
	}
	config_info.net_mode = mode;

	MP_THREAD_GIL_EXIT();
	ret = Helios_Nw_SetConfiguration(cur_simid, &config_info);
	MP_THREAD_GIL_ENTER();
	return mp_obj_new_int(ret); 
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qpy_net_set_configuration_obj, 1, 3, qpy_net_set_configuration);


/*=============================================================================*/
/* FUNCTION: qpy_net_get_mode                                                  */
/*=============================================================================*/
/*!@brief: get network mode
 *
 * @return:
 *     returns net mode on success.
 *    -1	-	error
 */
/*=============================================================================*/
static mp_obj_t qpy_net_get_configuration(size_t n_args, const mp_obj_t *args)
{
	int ret = 0;
	Helios_NwConfigInfoStruct config_info;
    memset(&config_info, 0, sizeof(Helios_NwConfigInfoStruct));

    int cur_simid = _get_current_simid();
	if (n_args ==1) 
	{
		cur_simid = mp_obj_get_int(args[0]);
	}
	if ((cur_simid != 0) && (cur_simid != 1))
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, simId should be in [0~1]."));
	}
	MP_THREAD_GIL_EXIT();
	ret = Helios_Nw_GetConfiguration(cur_simid, &config_info);
	MP_THREAD_GIL_ENTER();
	if(ret == 0)
	{
		mp_obj_t tuple[2] = {mp_obj_new_int(config_info.net_mode), mp_obj_new_bool(config_info.roaming_switch)};
		return mp_obj_new_tuple(2, tuple);
	}
	return mp_obj_new_int(-1);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qpy_net_get_configuration_obj, 0, 1, qpy_net_get_configuration);


/*=============================================================================*/
/* FUNCTION: qpy_net_get_csq                                                  */
/*=============================================================================*/
/*!@brief: get CSQ signal strength
 *
 * @return:
 *     returns CSQ on success
 *    -1	-	error
 */
/*=============================================================================*/
static mp_obj_t qpy_net_get_csq(size_t n_args, const mp_obj_t *args)
{
	int ret = 0;
	int csq = -1;
    uint8_t status = 0;
    int cur_simid = _get_current_simid();
	if (n_args ==1) 
	{
		cur_simid = mp_obj_get_int(args[0]);
	}
	if ((cur_simid != 0) && (cur_simid != 1))
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, simId should be in [0~1]."));
	}
	MP_THREAD_GIL_EXIT();
    ret = Helios_SIM_GetCardStatus(cur_simid, (Helios_SIM_Status_e *)&status);
	MP_THREAD_GIL_ENTER();
    if (ret == 0)
    {
        if (status != 1)
        {
            return mp_obj_new_int(99);
        }
    }
    else
    {
        return mp_obj_new_int(-1);
    }
	MP_THREAD_GIL_EXIT();
	csq = Helios_Nw_GetCSQ(cur_simid);
	MP_THREAD_GIL_ENTER();
	if(csq != -1)
	{
		return mp_obj_new_int(csq);
	}

	return mp_obj_new_int(-1);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qpy_net_get_csq_obj, 0, 1, qpy_net_get_csq);

/*=============================================================================*/
/* FUNCTION: qpy_net_get_nitz_time                                             */
/*=============================================================================*/
/*!@brief: get nitz time
 *
 * @return:
 *     returns nitz time on success
 *    -1	-	error
 */
/*=============================================================================*/
static mp_obj_t qpy_net_get_nitz_time(void)
{
	int ret = 0; 
	Helios_NwNITZTimeInfoStruct info;
    memset(&info, 0, sizeof(Helios_NwNITZTimeInfoStruct));
	MP_THREAD_GIL_EXIT();
	ret = Helios_Nw_GetNITZTime(&info);
	MP_THREAD_GIL_ENTER();
	if (ret == 0)
	{
		QPY_NET_LOG("nitz_time:%s\r\n",info.nitz_time);
		mp_obj_t tuple[3] = {
			mp_obj_new_str(info.nitz_time, strlen(info.nitz_time)), 
			mp_obj_new_int_from_ull(info.abs_time), 
			mp_obj_new_int(info.leap_sec)};

        return mp_obj_new_tuple(3, tuple);
	}

	return mp_obj_new_int(-1);
}
static MP_DEFINE_CONST_FUN_OBJ_0(qpy_net_get_nitz_time_obj, qpy_net_get_nitz_time);


/*=============================================================================*/
/* FUNCTION: qpy_net_get_operator_name                                         */
/*=============================================================================*/
/*!@brief: get operator name
 *
 * @return:
 *     returns operator name on success
 *    -1	-	error
 */
/*=============================================================================*/
static mp_obj_t qpy_net_get_operator_name(size_t n_args, const mp_obj_t *args)
{
	int ret = 0; 
	Helios_NwOperatorInfoStruct info;
	Helios_NwRegisterStatusInfoStruct reg_info;
    memset(&info, 0, sizeof(Helios_NwOperatorInfoStruct));
    memset(&reg_info, 0, sizeof(Helios_NwRegisterStatusInfoStruct));
    
    int cur_simid = _get_current_simid();
	if (n_args ==1) 
	{
		cur_simid = mp_obj_get_int(args[0]);
	}
	if ((cur_simid != 0) && (cur_simid != 1))
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, simId should be in [0~1]."));
	}
	MP_THREAD_GIL_EXIT();
	ret = Helios_Nw_GetRegisterStatus(cur_simid, &reg_info);
	MP_THREAD_GIL_ENTER();

	if (ret == 0)
	{
		if ((reg_info.data_reg.status != 1) && (reg_info.data_reg.status != 5))
		{
			QPY_NET_LOG("nw is not registered\r\n");
			return mp_obj_new_int(-1);
		}
	}
	else
	{
		QPY_NET_LOG("get nw register status failed.\r\n");
		return mp_obj_new_int(-1);
	}

	MP_THREAD_GIL_EXIT();
	ret = Helios_Nw_GetOperatorName(cur_simid, &info);
	MP_THREAD_GIL_ENTER();

	if (ret == 0)
	{
		mp_obj_t tuple[4] = {
			mp_obj_new_str(info.long_name, strlen(info.long_name)), 
			mp_obj_new_str(info.short_name, strlen(info.short_name)), 
			mp_obj_new_str(info.mcc, strlen(info.mcc)),
			mp_obj_new_str(info.mnc, strlen(info.mnc))};
			
		return mp_obj_new_tuple(4, tuple);
	}
	
	return mp_obj_new_int(-1);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qpy_net_get_operator_name_obj, 0, 1, qpy_net_get_operator_name);


/*=============================================================================*/
/* FUNCTION: qpy_net_get_selection                                             */
/*=============================================================================*/
/*!@brief: get network selection
 *
 * @return:
 *     returns (nw_selection,mcc, mnc, act) on success
 *    -1	-	error
 */
/*=============================================================================*/
static mp_obj_t qpy_net_get_selection(size_t n_args, const mp_obj_t *args)
{
	int ret = 0;
	Helios_NwSelectionInfoStruct info;
    memset(&info, 0, sizeof(Helios_NwSelectionInfoStruct));

    int cur_simid = _get_current_simid();
	if (n_args ==1) 
	{
		cur_simid = mp_obj_get_int(args[0]);
	}
	if ((cur_simid != 0) && (cur_simid != 1))
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, simId should be in [0~1]."));
	}
	MP_THREAD_GIL_EXIT();
	ret = Helios_Nw_GetSelection(cur_simid, &info);
	MP_THREAD_GIL_ENTER();
	if (ret == 0)
	{
		mp_obj_t tuple[4] = {
			mp_obj_new_int(info.nw_selection_mode), 
			mp_obj_new_str(info.mcc, strlen(info.mcc)), 
			mp_obj_new_str(info.mnc, strlen(info.mnc)), 
			mp_obj_new_int(info.act)};
			
		return mp_obj_new_tuple(4, tuple);
	}
	return mp_obj_new_int(-1);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qpy_net_get_selection_obj, 0, 1, qpy_net_get_selection);


/*=============================================================================*/
/* FUNCTION: qpy_net_get_reg_status                                            */
/*=============================================================================*/
/*!@brief: get information about network registration
 *
 * @return:
 *     returns information about network registration on success
 *    -1	-	error
 */
/*=============================================================================*/
static mp_obj_t qpy_net_get_reg_status(size_t n_args, const mp_obj_t *args)
{
	int ret = 0;
	Helios_NwRegisterStatusInfoStruct info;
    memset(&info, 0, sizeof(Helios_NwRegisterStatusInfoStruct));

    int cur_simid = _get_current_simid();
	if (n_args ==1) 
	{
		cur_simid = mp_obj_get_int(args[0]);
	}
	if ((cur_simid != 0) && (cur_simid != 1))
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, simId should be in [0~1]."));
	}
	MP_THREAD_GIL_EXIT();
	ret = Helios_Nw_GetRegisterStatus(cur_simid, &info);
	MP_THREAD_GIL_ENTER();
	if (ret == 0)
	{
		mp_obj_t voice_list[6] = {
			mp_obj_new_int(info.voice_reg.status), 
			mp_obj_new_int(info.voice_reg.lac), 
			mp_obj_new_int(info.voice_reg.cid), 
			mp_obj_new_int(info.voice_reg.act), 
			mp_obj_new_int(info.voice_reg.reject_cause), 
			mp_obj_new_int(info.voice_reg.psc)
			};
		
		mp_obj_t data_list[6] = {
			mp_obj_new_int(info.data_reg.status),
			mp_obj_new_int(info.data_reg.lac), 
			mp_obj_new_int(info.data_reg.cid), 
			mp_obj_new_int(info.data_reg.act), 
			mp_obj_new_int(info.data_reg.reject_cause), 
			mp_obj_new_int(info.data_reg.psc)
			};
		
		mp_obj_t tuple[2] = {
			mp_obj_new_list(6,voice_list), 
			mp_obj_new_list(6,data_list)};
		return mp_obj_new_tuple(2, tuple);
	}
	return mp_obj_new_int(-1);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qpy_net_get_reg_status_obj, 0, 1, qpy_net_get_reg_status);


/*=============================================================================*/
/* FUNCTION: qpy_net_get_signal_strength                                       */
/*=============================================================================*/
/*!@brief: get signal strength
 *	
 * @return:
 *     returns information about signal strength on success
 *    -1	-	error
 */
/*=============================================================================*/
static mp_obj_t qpy_net_get_signal_strength(size_t n_args, const mp_obj_t *args)
{
	int ret = 0;
	Helios_NwSignalStrengthInfoStruct info;
    memset(&info, 0, sizeof(Helios_NwSignalStrengthInfoStruct));
	int cur_simid = _get_current_simid();
	if (n_args ==2) 
	{
		cur_simid = mp_obj_get_int(args[1]);
	}
	if ((cur_simid != 0) && (cur_simid != 1))
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, simId should be in [0~1]."));
	}
	MP_THREAD_GIL_EXIT();
	ret = Helios_Nw_GetSignalStrength(cur_simid, &info);
	MP_THREAD_GIL_ENTER();

	if (ret == 0)
	{
		mp_obj_t gw_list[4] = {
			mp_obj_new_int(info.gw_signal_strength.rssi), 
			mp_obj_new_int(info.gw_signal_strength.bit_error_rate), 
			mp_obj_new_int(info.gw_signal_strength.rscp), 
			mp_obj_new_int(info.gw_signal_strength.ecno)};

    #if !defined (PLAT_RDA) //defined(PLAT_ASR_1803s) || defined(PLAT_ASR_1803sc) || defined(PLAT_Qualcomm) || defined(PLAT_Unisoc) || defined(PLAT_ASR)
        if (n_args > 0)
        {
            int flag = mp_obj_get_int(args[0]);
            if (flag == 1)
            {
        		mp_obj_t lte_list[5] = {
        			mp_obj_new_int(info.lte_signal_strength.rssi), 
        			mp_obj_new_int(info.lte_signal_strength.rsrp), 
        			mp_obj_new_int(info.lte_signal_strength.rsrq), 
        			mp_obj_new_int(info.lte_signal_strength.cqi),
        			mp_obj_new_int(info.lte_signal_strength.sinr)};

                mp_obj_t tuple[2] = {
        			mp_obj_new_list(4,gw_list), 
        			mp_obj_new_list(5,lte_list)};
                    
                return mp_obj_new_tuple(2, tuple);
            }
        }
    #endif
        mp_obj_t lte_list[4] = {
			mp_obj_new_int(info.lte_signal_strength.rssi), 
			mp_obj_new_int(info.lte_signal_strength.rsrp), 
			mp_obj_new_int(info.lte_signal_strength.rsrq), 
			mp_obj_new_int(info.lte_signal_strength.cqi)};

        mp_obj_t tuple[2] = {
			mp_obj_new_list(4,gw_list), 
			mp_obj_new_list(4,lte_list)};
            
        return mp_obj_new_tuple(2, tuple);
	}
	return mp_obj_new_int(-1);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qpy_net_get_signal_strength_obj, 0, 2, qpy_net_get_signal_strength);


/*=============================================================================*/
/* FUNCTION: qpy_net_get_mnc                                                   */
/*=============================================================================*/
/*!@brief: get mnc
 *	
 * @return:
 *     returns mnc on success
 *    -1	-	error
 */
/*=============================================================================*/
static mp_obj_t qpy_net_get_mnc(size_t n_args, const mp_obj_t *args)
{
	int ret = 0;
	int i = 0;
   	mp_obj_t mnc_list = mp_obj_new_list(0, NULL);
	Helios_NwCellInfoStruct info;
    memset(&info, 0, sizeof(Helios_NwCellInfoStruct));
    int cur_simid = _get_current_simid();
	if (n_args ==1) 
	{
		cur_simid = mp_obj_get_int(args[0]);
	}
	if ((cur_simid != 0) && (cur_simid != 1))
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, simId should be in [0~1]."));
	}
	MP_THREAD_GIL_EXIT();
	ret = Helios_Nw_GetCellInfo(cur_simid, &info);
    MP_THREAD_GIL_ENTER();
    
    if (ret == 0)
    {
		if (info.gsm_info_num > 0)
		{
			for (i=0; i<info.gsm_info_num; i++)
			{					
				mp_obj_list_append(mnc_list, mp_obj_new_int(info.gsm_info[i].mnc));
			}
		}

		if (info.umts_info_num > 0)
		{
			for (i=0; i<info.umts_info_num; i++)
			{					
				mp_obj_list_append(mnc_list, mp_obj_new_int(info.umts_info[i].mnc));
			}
		}

		if (info.lte_info_num > 0)
		{
			for (i=0; i<info.lte_info_num; i++)
			{					
				mp_obj_list_append(mnc_list, mp_obj_new_int(info.lte_info[i].mnc));
			}
		}

		return mnc_list;
	}

	return mp_obj_new_int(-1);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qpy_net_get_mnc_obj, 0, 1, qpy_net_get_mnc);

static mp_obj_t qpy_net_get_mnc_servingcell(size_t n_args, const mp_obj_t *args)
{
	int ret = 0;
	int i = 0;
	Helios_NwCellInfoStruct info;
    memset(&info, 0, sizeof(Helios_NwCellInfoStruct));
    int cur_simid = _get_current_simid();
	if (n_args ==1) 
	{
		cur_simid = mp_obj_get_int(args[0]);
	}
	if ((cur_simid != 0) && (cur_simid != 1))
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, simId should be in [0~1]."));
	}
	MP_THREAD_GIL_EXIT();
	ret = Helios_Nw_GetCellInfo(cur_simid, &info);
    MP_THREAD_GIL_ENTER();
    
    if (ret == 0)
    {
		if (info.gsm_info_num > 0)
		{
			for (i=0; i<info.gsm_info_num; i++)
			{					
				if (info.gsm_info[i].flag == 0) //serving cell
                {
                    return mp_obj_new_int(info.gsm_info[i].mnc);
                }            
			}
		}

		if (info.umts_info_num > 0)
		{
			for (i=0; i<info.umts_info_num; i++)
			{					
				if (info.umts_info[i].flag == 0) //serving cell
                {
                    return mp_obj_new_int(info.umts_info[i].mnc);
                } 
			}
		}

		if (info.lte_info_num > 0)
		{
			for (i=0; i<info.lte_info_num; i++)
			{					
				if (info.lte_info[i].flag == 0) //serving cell
                {
                    return mp_obj_new_int(info.lte_info[i].mnc);
                }
			}
		}
	}

	return mp_obj_new_int(-1);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qpy_net_get_mnc_servingcell_obj, 0, 1, qpy_net_get_mnc_servingcell);


/*=============================================================================*/
/* FUNCTION: qpy_net_get_mcc                                                   */
/*=============================================================================*/
/*!@brief: get mcc
 *	
 * @return:
 *     returns mnc on success
 *    -1	-	error
 */
/*=============================================================================*/
static mp_obj_t qpy_net_get_mcc(size_t n_args, const mp_obj_t *args)
{
	int ret = 0;
	int i = 0;
   	mp_obj_t mcc_list = mp_obj_new_list(0, NULL);
	Helios_NwCellInfoStruct info;
    memset(&info, 0, sizeof(Helios_NwCellInfoStruct));
    int cur_simid = _get_current_simid();
	if (n_args ==1) 
	{
		cur_simid = mp_obj_get_int(args[0]);
	}
	if ((cur_simid != 0) && (cur_simid != 1))
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, simId should be in [0~1]."));
	}
	MP_THREAD_GIL_EXIT();
	ret = Helios_Nw_GetCellInfo(cur_simid, &info);
    MP_THREAD_GIL_ENTER();
    
    if (ret == 0)
    {
		if (info.gsm_info_num > 0)
		{
			for (i=0; i<info.gsm_info_num; i++)
			{					
				mp_obj_list_append(mcc_list, mp_obj_new_int(info.gsm_info[i].mcc));
			}
		}

		if (info.umts_info_num > 0)
		{
			for (i=0; i<info.umts_info_num; i++)
			{					
				mp_obj_list_append(mcc_list, mp_obj_new_int(info.umts_info[i].mcc));
			}
		}

		if (info.lte_info_num > 0)
		{
			for (i=0; i<info.lte_info_num; i++)
			{					
				mp_obj_list_append(mcc_list, mp_obj_new_int(info.lte_info[i].mcc));
			}
		}

		return mcc_list;
	}

	return mp_obj_new_int(-1);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qpy_net_get_mcc_obj, 0, 1, qpy_net_get_mcc);

static mp_obj_t qpy_net_get_mcc_servingcell(size_t n_args, const mp_obj_t *args)
{
	int ret = 0;
	int i = 0;
	Helios_NwCellInfoStruct info;
    memset(&info, 0, sizeof(Helios_NwCellInfoStruct));
    int cur_simid = _get_current_simid();
	if (n_args ==1) 
	{
		cur_simid = mp_obj_get_int(args[0]);
	}
	if ((cur_simid != 0) && (cur_simid != 1))
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, simId should be in [0~1]."));
	}
	MP_THREAD_GIL_EXIT();
	ret = Helios_Nw_GetCellInfo(cur_simid, &info);
    MP_THREAD_GIL_ENTER();
    
    if (ret == 0)
    {
		if (info.gsm_info_num > 0)
		{
			for (i=0; i<info.gsm_info_num; i++)
			{					
				if (info.gsm_info[i].flag == 0) //serving cell
                {
                    return mp_obj_new_int(info.gsm_info[i].mcc);
                }
			}
		}

		if (info.umts_info_num > 0)
		{
			for (i=0; i<info.umts_info_num; i++)
			{					
				if (info.umts_info[i].flag == 0) //serving cell
                {
                    return mp_obj_new_int(info.umts_info[i].mcc);
                }
			}
		}

		if (info.lte_info_num > 0)
		{
			for (i=0; i<info.lte_info_num; i++)
			{					
				if (info.lte_info[i].flag == 0) //serving cell
                {
                    return mp_obj_new_int(info.lte_info[i].mcc);
                }
			}
		}
	}

	return mp_obj_new_int(-1);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qpy_net_get_mcc_servingcell_obj, 0, 1, qpy_net_get_mcc_servingcell);


/*=============================================================================*/
/* FUNCTION: qpy_net_get_lac                                                   */
/*=============================================================================*/
/*!@brief: get lac
 *	
 * @return:
 *     returns lac on success
 *    -1	-	error
 */
/*=============================================================================*/
static mp_obj_t qpy_net_get_lac(size_t n_args, const mp_obj_t *args)
{
	int ret = 0;
	int i = 0;
   	mp_obj_t lac_list = mp_obj_new_list(0, NULL);
	Helios_NwCellInfoStruct info;
    memset(&info, 0, sizeof(Helios_NwCellInfoStruct));
    int cur_simid = _get_current_simid();
	if (n_args ==1) 
	{
		cur_simid = mp_obj_get_int(args[0]);
	}
	if ((cur_simid != 0) && (cur_simid != 1))
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, simId should be in [0~1]."));
	}
	MP_THREAD_GIL_EXIT();
	ret = Helios_Nw_GetCellInfo(cur_simid, &info);
    MP_THREAD_GIL_ENTER();
    
    if (ret == 0)
    {
		if (info.gsm_info_num > 0)
		{
			for (i=0; i<info.gsm_info_num; i++)
			{					
				mp_obj_list_append(lac_list, mp_obj_new_int(info.gsm_info[i].lac));
			}
		}
		
		if (info.umts_info_num > 0)
		{
			for (i=0; i<info.umts_info_num; i++)
			{					
				mp_obj_list_append(lac_list, mp_obj_new_int(info.umts_info[i].lac));
			}
		}
		
		if (info.lte_info_num > 0)
		{
			for (i=0; i<info.lte_info_num; i++)
			{					
				mp_obj_list_append(lac_list, mp_obj_new_int(info.lte_info[i].tac));
			}
		}

		return lac_list;
	}

	return mp_obj_new_int(-1);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qpy_net_get_lac_obj, 0, 1, qpy_net_get_lac);

static mp_obj_t qpy_net_get_lac_servingcell(size_t n_args, const mp_obj_t *args)
{
	int ret = 0;
	int i = 0;
	Helios_NwCellInfoStruct info;
    memset(&info, 0, sizeof(Helios_NwCellInfoStruct));
    int cur_simid = _get_current_simid();
	if (n_args ==1) 
	{
		cur_simid = mp_obj_get_int(args[0]);
	}
	if ((cur_simid != 0) && (cur_simid != 1))
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, simId should be in [0~1]."));
	}
	MP_THREAD_GIL_EXIT();
	ret = Helios_Nw_GetCellInfo(cur_simid, &info);
    MP_THREAD_GIL_ENTER();
    
    if (ret == 0)
    {
		if (info.gsm_info_num > 0)
		{
			for (i=0; i<info.gsm_info_num; i++)
			{
				if (info.gsm_info[i].flag == 0) //serving cell
                {
                    return mp_obj_new_int(info.gsm_info[i].lac);
                }
			}
		}
		
		if (info.umts_info_num > 0)
		{
			for (i=0; i<info.umts_info_num; i++)
			{
				if (info.umts_info[i].flag == 0) //serving cell
                {
                    return mp_obj_new_int(info.umts_info[i].lac);
                }
			}
		}
		
		if (info.lte_info_num > 0)
		{
			for (i=0; i<info.lte_info_num; i++)
			{
				if (info.lte_info[i].flag == 0) //serving cell
                {
                    return mp_obj_new_int(info.lte_info[i].tac);
                }
			}
		}
	}

	return mp_obj_new_int(-1);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qpy_net_get_lac_servingcell_obj, 0, 1, qpy_net_get_lac_servingcell);

/*=============================================================================*/
/* FUNCTION: qpy_net_get_cid                                                   */
/*=============================================================================*/
/*!@brief: get cid
 *	
 * @return:
 *     returns cid on success
 *    -1	-	error
 */
/*=============================================================================*/
static mp_obj_t qpy_net_get_cid(size_t n_args, const mp_obj_t *args)
{
	int ret = 0;
	int i = 0;
   	mp_obj_t cid_list = mp_obj_new_list(0, NULL);
	Helios_NwCellInfoStruct info;
    memset(&info, 0, sizeof(Helios_NwCellInfoStruct));
    int cur_simid = _get_current_simid();
	if (n_args ==1) 
	{
		cur_simid = mp_obj_get_int(args[0]);
	}
	if ((cur_simid != 0) && (cur_simid != 1))
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, simId should be in [0~1]."));
	}
	MP_THREAD_GIL_EXIT();
	ret = Helios_Nw_GetCellInfo(cur_simid, &info);
    MP_THREAD_GIL_ENTER();
    
    if (ret == 0)
    {
		if (info.gsm_info_num > 0)
		{
			for (i=0; i<info.gsm_info_num; i++)
			{					
				mp_obj_list_append(cid_list, mp_obj_new_int(info.gsm_info[i].cid));
			}
		}
		
		if (info.umts_info_num > 0)
		{
			for (i=0; i<info.umts_info_num; i++)
			{					
				mp_obj_list_append(cid_list, mp_obj_new_int(info.umts_info[i].cid));
			}
		}
		
		if (info.lte_info_num > 0)
		{
			for (i=0; i<info.lte_info_num; i++)
			{					
				mp_obj_list_append(cid_list, mp_obj_new_int(info.lte_info[i].cid));
			}
		}

		return cid_list;
	}

	return mp_obj_new_int(-1);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qpy_net_get_cid_obj, 0, 1, qpy_net_get_cid);

static mp_obj_t qpy_net_get_cid_servingcell(size_t n_args, const mp_obj_t *args)
{
	int ret = 0;
	int i = 0;
	Helios_NwCellInfoStruct info;
    memset(&info, 0, sizeof(Helios_NwCellInfoStruct));
    int cur_simid = _get_current_simid();
	if (n_args ==1) 
	{
		cur_simid = mp_obj_get_int(args[0]);
	}
	if ((cur_simid != 0) && (cur_simid != 1))
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, simId should be in [0~1]."));
	}
	MP_THREAD_GIL_EXIT();
	ret = Helios_Nw_GetCellInfo(cur_simid, &info);
    MP_THREAD_GIL_ENTER();
    
    if (ret == 0)
    {
		if (info.gsm_info_num > 0)
		{
			for (i=0; i<info.gsm_info_num; i++)
			{
				if (info.gsm_info[i].flag == 0) //serving cell
                {
                    return mp_obj_new_int(info.gsm_info[i].cid);
                }
			}
		}
		
		if (info.umts_info_num > 0)
		{
			for (i=0; i<info.umts_info_num; i++)
			{
				if (info.umts_info[i].flag == 0) //serving cell
                {
                    return mp_obj_new_int(info.umts_info[i].cid);
                }
			}
		}
		
		if (info.lte_info_num > 0)
		{
			for (i=0; i<info.lte_info_num; i++)
			{
				if (info.lte_info[i].flag == 0) //serving cell
                {
                    return mp_obj_new_int(info.lte_info[i].cid);
                }
			}
		}
	}

	return mp_obj_new_int(-1);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qpy_net_get_cid_servingcell_obj, 0, 1, qpy_net_get_cid_servingcell);

#if defined(PLAT_Qualcomm) || defined(PLAT_ASR) || MICROPY_QPY_MODULE_NET_BAND
static mp_obj_t qpy_net_set_band(size_t n_args, const mp_obj_t *args)
{
	int ret = 0;
	int net_type = mp_obj_get_int(args[0]);
	int gsm_band = mp_obj_get_int(args[1]);

	if ((net_type < 0) || (net_type > 3))
	{
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, net_type should be in [0,%d]."), HELIOS_BAND_TYPE_MAX_NUM);
	}
	if ((gsm_band < 0) || (gsm_band > 0x0f))
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, gsm_band should be in [0,%x]."), HELIOS_BAND_GSM_MAXVALUE);
	}

	size_t len = 0;
    mp_obj_t *elem = NULL;
    mp_obj_get_array(args[2], &len, &elem);
	QPY_NET_LOG("[helios_nw_band]elem num : %d\r\n", len);
	if (len != 4)
	{
		mp_raise_ValueError("invalid value, band tuple should be 4 uint32 elements.");
	}
	
	uint32_t band_hh = mp_obj_get_int_truncated(elem[0]);
	uint32_t band_hl = mp_obj_get_int_truncated(elem[1]);
	uint32_t band_lh = mp_obj_get_int_truncated(elem[2]);
	uint32_t band_ll = mp_obj_get_int_truncated(elem[3]);
	QPY_NET_LOG("[helios_nw_band]band_hh=%x, band_hl=%x, band_lh=%x, band_ll=%x\r\n", band_hh, band_hl, band_lh, band_ll);
	
	Helios_NwBandStruct band_info;
    memset(&band_info, 0, sizeof(Helios_NwBandStruct));
	band_info.band_type = net_type;
	band_info.band_gsm  = gsm_band;
	band_info.band_low  = (((uint64_t)band_lh) << 32) | band_ll;
	band_info.band_hign = (((uint64_t)band_hh) << 32) | band_hl;
	QPY_NET_LOG("[helios_nw_band]bandh=%llx, bandl=%llx\r\n", band_info.band_hign, band_info.band_low);

    int cur_simid = _get_current_simid();
	MP_THREAD_GIL_EXIT();
	ret = Helios_Nw_SetBand(cur_simid, &band_info);
	MP_THREAD_GIL_ENTER();
	return ((ret == 0) ? mp_obj_new_int(0) : mp_obj_new_int(ret));
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qpy_net_set_band_obj, 3, 3, qpy_net_set_band);

static mp_obj_t qpy_net_get_band(mp_obj_t net_type)
{
	int band_type = mp_obj_get_int(net_type);
	if ((band_type < 0) || (band_type > 3))
	{
		mp_raise_ValueError(MP_ERROR_TEXT("invalid value, net_type should be in [0,3]."));
	}

	int ret = 0;
	Helios_NwBandStruct band_info;
    memset(&band_info, 0, sizeof(Helios_NwBandStruct));
	band_info.band_type = band_type;
    int cur_simid = _get_current_simid();
	MP_THREAD_GIL_EXIT();
	ret = Helios_Nw_GetBand(cur_simid, &band_info);
	MP_THREAD_GIL_ENTER();
	if (ret == 0)
	{
		char str[64] = {0};
		if (band_type == HELIOS_GSM_BAND)
		{
			snprintf(str, 64, "0x%x", band_info.band_gsm);
		}
		else
		{
			if (band_info.band_hign == 0)
			{
#if MICROPY_QPY_MODULE_NET_BAND
				uint32_t band_lh = band_info.band_low >> 32;
    			uint32_t band_ll = band_info.band_low & 0xFFFFFFFF;
				QPY_NET_LOG("[helios_nw_band]band_hign=%lx, band_low=%lx\r\n", band_lh, band_ll);
				if (band_lh ==0) {
					snprintf(str, 64, "0x%lx", band_ll);
				} else {
					snprintf(str, 64, "0x%lx%08lx", band_lh, band_ll);
				}
#else
				snprintf(str, 64, "0x%llx", band_info.band_low);
#endif
			}
			else
			{
				snprintf(str, 64, "0x%llx%016llx", band_info.band_hign, band_info.band_low);
			}
		}
		return mp_obj_new_str(str, strlen(str));
	}
	return mp_obj_new_int(ret);
}
static MP_DEFINE_CONST_FUN_OBJ_1(qpy_net_get_band_obj, qpy_net_get_band);
#endif

#if defined(PLAT_ASR) || MICROPY_QPY_MODULE_NET_BAND
static mp_obj_t qpy_net_band_restore(void)
{
	int ret = 0;
	int cur_simid = _get_current_simid();
    MP_THREAD_GIL_EXIT();
	ret = Helios_Nw_Band_Restore(cur_simid);
    MP_THREAD_GIL_ENTER();

	return mp_obj_new_int(ret);
}
static MP_DEFINE_CONST_FUN_OBJ_0(qpy_net_band_restore_obj, qpy_net_band_restore);
#endif
/*=============================================================================*/
/* FUNCTION: qpy_net_get_cell_info                                             */
/*=============================================================================*/
/*!@brief: get cell informations 
 *	
 * @return:
 *     returns cell informations on success
 *    -1	-	error
 */
/*=============================================================================*/
static mp_obj_t qpy_net_get_cell_info(size_t n_args, const mp_obj_t *args)
{
	int i = 0;
	int ret = 0;

	mp_obj_t list_gsm  = mp_obj_new_list(0, NULL);
	mp_obj_t list_umts = mp_obj_new_list(0, NULL);
	mp_obj_t list_lte  = mp_obj_new_list(0, NULL);
	Helios_NwCellInfoStruct info;
    memset(&info, 0, sizeof(Helios_NwCellInfoStruct));

    int cur_simid = _get_current_simid();
#if defined(PLAT_RDA) || defined(PLAT_EIGEN) || defined(PLAT_EIGEN_718) || defined(PLAT_SONY_ALT1350)
    int flag_args = -1;
#else
	if (n_args ==1) 
	{
		cur_simid = mp_obj_get_int(args[0]);
	}
	if ((cur_simid != 0) && (cur_simid != 1))
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, simId should be in [0~1]."));
	}
#endif	
    MP_THREAD_GIL_EXIT();
	ret = Helios_Nw_GetCellInfo(cur_simid, &info);
    MP_THREAD_GIL_ENTER();
	
    if (ret == 0)
    {
		if (info.gsm_info_num > 0)
		{
			for (i=0; i<info.gsm_info_num; i++)
			{
				mp_obj_t gsm[8] = {
					mp_obj_new_int(info.gsm_info[i].flag), 
					mp_obj_new_int(info.gsm_info[i].cid),
					mp_obj_new_int(info.gsm_info[i].mcc),
					mp_obj_new_int(info.gsm_info[i].mnc),
					mp_obj_new_int(info.gsm_info[i].lac),
					mp_obj_new_int(info.gsm_info[i].arfcn),
					mp_obj_new_int(info.gsm_info[i].bsic), 
					//Pawn.zhou Edit 2020/12/18
					mp_obj_new_int(info.gsm_info[i].rssi)};

				mp_obj_list_append(list_gsm, mp_obj_new_tuple(8, gsm));
			}			
		}
	
		if (info.umts_info_num > 0)
		{
			for (i=0; i<info.umts_info_num; i++)
			{
				mp_obj_t umts[9] = {
						mp_obj_new_int(info.umts_info[i].flag), 
						mp_obj_new_int(info.umts_info[i].cid),
						mp_obj_new_int(info.umts_info[i].lcid),
						mp_obj_new_int(info.umts_info[i].mcc),
						mp_obj_new_int(info.umts_info[i].mnc),
						mp_obj_new_int(info.umts_info[i].lac),
						mp_obj_new_int(info.umts_info[i].uarfcn),
						mp_obj_new_int(info.umts_info[i].psc),
						//Pawn.zhou Edit 2020/12/18
						mp_obj_new_int(info.umts_info[i].rssi)};
				
				mp_obj_list_append(list_umts, mp_obj_new_tuple(9, umts));
			}
		}
	
		if (info.lte_info_num > 0)
		{
		#if defined(PLAT_RDA) || defined(PLAT_EIGEN) || defined(PLAT_EIGEN_718) || defined(PLAT_SONY_ALT1350)
            if (n_args > 0)
            {
                flag_args = mp_obj_get_int(args[0]);
                    
                if (flag_args == 1)
                {
                    for (i=0; i<info.lte_info_num; i++)
			        {
                        mp_obj_t lte[10] = {
        					mp_obj_new_int(info.lte_info[i].flag),
        					mp_obj_new_int(info.lte_info[i].cid),
        					mp_obj_new_int(info.lte_info[i].mcc),
        					mp_obj_new_int(info.lte_info[i].mnc),
        					mp_obj_new_int(info.lte_info[i].pci),
        					mp_obj_new_int(info.lte_info[i].tac),
        					mp_obj_new_int(info.lte_info[i].earfcn),
        					mp_obj_new_int(info.lte_info[i].rssi),
        					mp_obj_new_int(info.lte_info[i].rsrq),
        					mp_obj_new_int(info.lte_info[i].sinr)};

        				mp_obj_list_append(list_lte, mp_obj_new_tuple(10, lte));
                    }
                    
                    mp_obj_t list[3] = {list_gsm, list_umts, list_lte};
		            return mp_obj_new_tuple(3, list);
                }
                else if (flag_args == 0) 
                {
                    //
                }
                else
                {
                    return mp_obj_new_int(-1);
                }
            }
        #endif
			for (i=0; i<info.lte_info_num; i++)
			{
				//Pawn.zhou Edit 2020/12/18
				mp_obj_t lte[9] = {
					mp_obj_new_int(info.lte_info[i].flag),
					mp_obj_new_int(info.lte_info[i].cid),
					mp_obj_new_int(info.lte_info[i].mcc),
					mp_obj_new_int(info.lte_info[i].mnc),
					mp_obj_new_int(info.lte_info[i].pci),
					mp_obj_new_int(info.lte_info[i].tac),
					mp_obj_new_int(info.lte_info[i].earfcn),
					mp_obj_new_int(info.lte_info[i].rssi),
					mp_obj_new_int(info.lte_info[i].rsrq)};

				mp_obj_list_append(list_lte, mp_obj_new_tuple(9, lte));
			}
		}
		mp_obj_t list[3] = {list_gsm, list_umts, list_lte};

		return mp_obj_new_tuple(3, list);
    }
	return mp_obj_new_int(-1);
}

static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qpy_net_get_cell_info_obj, 0, 1, qpy_net_get_cell_info);




static mp_obj_t qpy_net_get_cell_info_v2(size_t n_args, const mp_obj_t *args)
{
	int i = 0;
	int ret = 0;
#if	defined(PLAT_Qualcomm) || defined(PLAT_ASR) || defined(PLAT_ASR_1602) || defined(PLAT_ASR_1606) || defined(PLAT_ASR_1609) || defined(PLAT_ASR_1803s) || defined(PLAT_ASR_1803sc) || defined(PLAT_EIGEN) || defined(PLAT_EIGEN_718)
	int mcc = 0;
	int mnc = 0;
	char para[5]={0};
#endif
	mp_obj_t list_gsm  = mp_obj_new_list(0, NULL);
	mp_obj_t list_umts = mp_obj_new_list(0, NULL);
	mp_obj_t list_lte  = mp_obj_new_list(0, NULL);
	Helios_NwCellInfoStruct info;
    memset(&info, 0, sizeof(Helios_NwCellInfoStruct));

    int cur_simid = _get_current_simid();
	if (n_args ==1) 
	{
		cur_simid = mp_obj_get_int(args[0]);
	}
	if ((cur_simid != 0) && (cur_simid != 1))
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, simId should be in [0~1]."));
	}
    MP_THREAD_GIL_EXIT();
	ret = Helios_Nw_GetCellInfo(cur_simid, &info);
    MP_THREAD_GIL_ENTER();
    
    if (ret == 0)
    {
		if (info.gsm_info_num > 0)
		{
			for (i=0; i<info.gsm_info_num; i++)
			{
			#if	defined(PLAT_Qualcomm) || defined(PLAT_ASR) || defined(PLAT_ASR_1602) || defined(PLAT_ASR_1606) || defined(PLAT_ASR_1609) || defined(PLAT_ASR_1803s) || defined(PLAT_ASR_1803sc) || defined(PLAT_EIGEN) || defined(PLAT_EIGEN_718)		
				memset(para, 0, 5);
				sprintf(para, "%x", info.gsm_info[i].mcc);
				sscanf(para, "%d", &mcc);
				memset(para, 0, 5);
                sprintf(para, "%x", info.gsm_info[i].mnc);
				sscanf(para, "%d", &mnc);
			#endif
				mp_obj_t gsm[8] = {
					mp_obj_new_int(info.gsm_info[i].flag), 
					mp_obj_new_int(info.gsm_info[i].cid),
			#if	defined(PLAT_Qualcomm) || defined(PLAT_ASR) || defined(PLAT_ASR_1602) || defined(PLAT_ASR_1606) || defined(PLAT_ASR_1609) || defined(PLAT_ASR_1803s) || defined(PLAT_ASR_1803sc) || defined(PLAT_EIGEN) || defined(PLAT_EIGEN_718)
					mp_obj_new_int(mcc),
					mp_obj_new_int(mnc),
			#else
					mp_obj_new_int(info.gsm_info[i].mcc),
					mp_obj_new_int(info.gsm_info[i].mnc),
			#endif
					mp_obj_new_int(info.gsm_info[i].lac),
					mp_obj_new_int(info.gsm_info[i].arfcn),
					mp_obj_new_int(info.gsm_info[i].bsic), 
					//Pawn.zhou Edit 2020/12/18
					mp_obj_new_int(info.gsm_info[i].rssi)};

				mp_obj_list_append(list_gsm, mp_obj_new_tuple(8, gsm));
			}			
		}
	
		if (info.umts_info_num > 0)
		{
			for (i=0; i<info.umts_info_num; i++)
			{
			#if	defined(PLAT_Qualcomm) || defined(PLAT_ASR) || defined(PLAT_ASR_1602) || defined(PLAT_ASR_1606) || defined(PLAT_ASR_1609) || defined(PLAT_ASR_1803s) || defined(PLAT_ASR_1803sc) || defined(PLAT_EIGEN) || defined(PLAT_EIGEN_718)		
				memset(para, 0, 5);
				sprintf(para, "%x", info.umts_info[i].mcc);
				sscanf(para, "%d", &mcc);
				memset(para, 0, 5);
                sprintf(para, "%x", info.umts_info[i].mnc);
				sscanf(para, "%d", &mnc);
			#endif
				mp_obj_t umts[9] = {
					mp_obj_new_int(info.umts_info[i].flag), 
					mp_obj_new_int(info.umts_info[i].cid),
					mp_obj_new_int(info.umts_info[i].lcid),
			#if	defined(PLAT_Qualcomm) || defined(PLAT_ASR) || defined(PLAT_ASR_1602) || defined(PLAT_ASR_1606) || defined(PLAT_ASR_1609) || defined(PLAT_ASR_1803s) || defined(PLAT_ASR_1803sc) || defined(PLAT_EIGEN) || defined(PLAT_EIGEN_718)
					mp_obj_new_int(mcc),
					mp_obj_new_int(mnc),
			#else
					mp_obj_new_int(info.umts_info[i].mcc),
					mp_obj_new_int(info.umts_info[i].mnc),
			#endif
					mp_obj_new_int(info.umts_info[i].lac),
					mp_obj_new_int(info.umts_info[i].uarfcn),
					mp_obj_new_int(info.umts_info[i].psc),
					//Pawn.zhou Edit 2020/12/18
					mp_obj_new_int(info.umts_info[i].rssi)};
				
				mp_obj_list_append(list_umts, mp_obj_new_tuple(9, umts));
			}
		}
	
		if (info.lte_info_num > 0)
		{
			for (i=0; i<info.lte_info_num; i++)
			{
			#if	defined(PLAT_Qualcomm) || defined(PLAT_ASR) || defined(PLAT_ASR_1602) || defined(PLAT_ASR_1606) || defined(PLAT_ASR_1609) || defined(PLAT_ASR_1803s) || defined(PLAT_ASR_1803sc) || defined(PLAT_EIGEN) || defined(PLAT_EIGEN_718)		
				memset(para, 0, 5);
				sprintf(para, "%x", info.lte_info[i].mcc);
				sscanf(para, "%d", &mcc);
				memset(para, 0, 5);
                sprintf(para, "%x", info.lte_info[i].mnc);
				sscanf(para, "%d", &mnc);
			#endif
				mp_obj_t lte[9] = {
					mp_obj_new_int(info.lte_info[i].flag),
					mp_obj_new_int(info.lte_info[i].cid),
			#if	defined(PLAT_Qualcomm) || defined(PLAT_ASR) || defined(PLAT_ASR_1602) || defined(PLAT_ASR_1606) || defined(PLAT_ASR_1609) || defined(PLAT_ASR_1803s) || defined(PLAT_ASR_1803sc) || defined(PLAT_EIGEN) || defined(PLAT_EIGEN_718)
					mp_obj_new_int(mcc),
					mp_obj_new_int(mnc),
			#else
					mp_obj_new_int(info.lte_info[i].mcc),
					mp_obj_new_int(info.lte_info[i].mnc),
			#endif
					mp_obj_new_int(info.lte_info[i].pci),
					mp_obj_new_int(info.lte_info[i].tac),
					mp_obj_new_int(info.lte_info[i].earfcn),
					mp_obj_new_int(info.lte_info[i].rssi),
					mp_obj_new_int(info.lte_info[i].rsrq)};

				mp_obj_list_append(list_lte, mp_obj_new_tuple(9, lte));
			}
		}
		mp_obj_t list[3] = {list_gsm, list_umts, list_lte};

		return mp_obj_new_tuple(3, list);
    }
	return mp_obj_new_int(-1);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qpy_net_get_cell_info_v2_obj, 0, 1, qpy_net_get_cell_info_v2);

/*=============================================================================*/
/* FUNCTION: qpy_net_get_modem_fun                                             */
/*=============================================================================*/
/*!@brief: get the current mode of the SIM
 *	
 * @return:
 *     returns mode on success
 *    -1	-	error
 */
/*=============================================================================*/
static mp_obj_t qpy_net_get_modem_fun(size_t n_args, const mp_obj_t *args)
{

	int ret = 0;
	Helios_DevModemFunction current_fun = 0;
	#if MICROPY_QPY_MODULE_DSDS
	int cur_simid = _get_current_simid();
	if (n_args ==1) 
	{
		cur_simid = mp_obj_get_int(args[0]);
	}
	if ((cur_simid != 0) && (cur_simid != 1))
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, simId should be in [0~1]."));
	}
	MP_THREAD_GIL_EXIT();
	ret = Helios_Dev_GetModemFunction(&current_fun, cur_simid);
	MP_THREAD_GIL_ENTER();
	#else
	MP_THREAD_GIL_EXIT();
	ret = Helios_Dev_GetModemFunction(&current_fun,0);
	MP_THREAD_GIL_ENTER();
	#endif
	if (ret == 0)
	{
		return mp_obj_new_int(current_fun);
	}

	return mp_obj_new_int(-1);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qpy_net_get_modem_fun_obj, 0, 1, qpy_net_get_modem_fun);


/*=============================================================================*/
/* FUNCTION: qpy_net_get_modem_fun                                             */
/*=============================================================================*/
/*!@brief: turn on or off flight mode
 *	
 * @modem_fun	[in] 	1 - turn off flight mode, 4 - turn on flight mode
 * @rst			[in] 	0 - effective immediately, 1 - restart to take effect
 *
 * @return:
 *     0	-	success
 *    -1	-	error
 */
/*=============================================================================*/
static mp_obj_t qpy_net_set_modem_fun(size_t n_args, const mp_obj_t *args)
{

	int modem_fun;
	int rst = 0;
	int ret = 0;
	
	if ( n_args > 1 )
	{
		rst = mp_obj_get_int(args[1]);
	}
	
	modem_fun = (uint8_t)mp_obj_get_int(args[0]);

	#if MICROPY_QPY_MODULE_DSDS
	int cur_simid = _get_current_simid();
	if (n_args ==3) 
	{
		cur_simid = mp_obj_get_int(args[2]);
	}
	if ((cur_simid != 0) && (cur_simid != 1))
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, simId should be in [0~1]."));
	}
	MP_THREAD_GIL_EXIT();
	ret = Helios_Dev_SetModemFunction(modem_fun, rst, cur_simid);
	MP_THREAD_GIL_ENTER();
	#else
	MP_THREAD_GIL_EXIT();
	ret = Helios_Dev_SetModemFunction(modem_fun, rst,0);
	MP_THREAD_GIL_ENTER();
	#endif
	if (ret == 0)
	{
		return mp_obj_new_int(0);
	}

	return mp_obj_new_int(-1);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qpy_net_set_modem_fun_obj, 1, 3, qpy_net_set_modem_fun);

static mp_obj_t qpy_net_set_apn(size_t n_args, const mp_obj_t *args)
{
#if defined(PLAT_Qualcomm) || defined(PLAT_ASR_1803s) || defined(PLAT_ASR_1803sc) || (defined(PLAT_Unisoc) && !defined(BOARD_EG915UEU_AB)) \
    || defined(PLAT_ASR) || defined(PLAT_ASR_1606) || defined(PLAT_ASR_1609) || defined(PLAT_Unisoc_8910_R05) || defined(PLAT_ASR_1602) || defined(PLAT_Unisoc_8910_R06)
    if (n_args == 2)
    {
        mp_buffer_info_t apninfo = {0};
        mp_get_buffer_raise(args[0], &apninfo, MP_BUFFER_READ);
        uint8_t net_simid = mp_obj_get_int(args[1]);
        int ret = 0;
        if (net_simid != 0)
        {
            mp_raise_ValueError(MP_ERROR_TEXT("invalid simid."));
        }
        if (apninfo.len > HELIOS_APN_LEN_MAX)
    	{
    		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, the length of apn should be no more than [%d] bytes."), HELIOS_APN_LEN_MAX);
    	}
		MP_THREAD_GIL_EXIT();
        ret = Helios_Nw_SetApn((char *)apninfo.buf, net_simid);
		MP_THREAD_GIL_ENTER();
    	if (ret == 0)
    	{
    		return mp_obj_new_int(0);
    	}
    	return mp_obj_new_int(-1);
    }
    else
#endif
    {
        if (n_args == 7)//pid/iptype/apn/usrname/pwd/authtype/simid
        {
            int8_t ret = 0;
            uint8_t net_simid = mp_obj_get_int(args[6]);
        	uint8_t profile_id = mp_obj_get_int(args[0]);
        	int ip_type = mp_obj_get_int(args[1]);
        	int auth_type = mp_obj_get_int(args[5]);
        	
        	mp_buffer_info_t apninfo = {0};
        	mp_buffer_info_t usrinfo = {0};
        	mp_buffer_info_t pwdinfo = {0};
        	mp_get_buffer_raise(args[2], &apninfo, MP_BUFFER_READ);
        	mp_get_buffer_raise(args[3], &usrinfo, MP_BUFFER_READ);
        	mp_get_buffer_raise(args[4], &pwdinfo, MP_BUFFER_READ);

        	int min_profile_id = (int)HELIOS_PROFILE_IDX_MIN;
        	int max_profile_id = (int)HELIOS_PROFILE_IDX_MAX;

            #if MICROPY_QPY_MODULE_DSDS
            if ((net_simid != 0) && (net_simid != 1))
			#else
			if (net_simid != 0)
			#endif
            {
                mp_raise_ValueError(MP_ERROR_TEXT("invalid simid."));
            }
        	if ((profile_id < min_profile_id) || (profile_id > max_profile_id))
        	{
        		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, profileIdx should be in [%d,%d]."), min_profile_id, max_profile_id);
        	}
        	if ((ip_type < 0) || (ip_type > HELIOS_PDP_TYPE_NUM - 1))
        	{
        		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, ipType should be in [0~%d]."), HELIOS_PDP_TYPE_NUM-1);
        	}
        	if ((auth_type < 0) || (auth_type > HELIOS_AUTH_TYPE_NUM - 1))
        	{
        		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid value, authType should be in [0~%d]."), HELIOS_AUTH_TYPE_NUM-1);
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

        	QPY_NET_LOG("[NW-PDNINFO] profile_idx=%d, ip_version=%d, auth_type=%d\r\n", profile_id, ip_type, auth_type);
        	QPY_NET_LOG("[NW-PDNINFO] anp_name=%s, usr_name=%s, password=%s\r\n", apninfo.buf, usrinfo.buf, pwdinfo.buf);
        	
            Helios_DataCallStartStruct pdpinfo_set;
            memset(&pdpinfo_set, 0, sizeof(Helios_DataCallStartStruct));
            
            pdpinfo_set.ip_type = (int32_t)ip_type;
            pdpinfo_set.auth = (int32_t)auth_type;
            snprintf(pdpinfo_set.apn, sizeof(pdpinfo_set.apn), "%s", (char *)apninfo.buf);
            snprintf(pdpinfo_set.user, sizeof(pdpinfo_set.user), "%s", (char *)usrinfo.buf);
            snprintf(pdpinfo_set.pwd, sizeof(pdpinfo_set.pwd), "%s", (char *)pwdinfo.buf);
            MP_THREAD_GIL_EXIT();
			#if MICROPY_QPY_MODULE_DSDS
            ret = Helios_DataCall_SetPDPContext(net_simid, profile_id, &pdpinfo_set);
			#else
            ret = Helios_DataCall_SetPDPContext(profile_id, &pdpinfo_set);
			#endif
            MP_THREAD_GIL_ENTER();
        	return mp_obj_new_int(ret);
        }
        else
        {
            mp_raise_ValueError(MP_ERROR_TEXT("Incorrect parameter number!"));
        }
    }
}

static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qpy_net_set_apn_obj, 2, 7, qpy_net_set_apn);

static mp_obj_t qpy_net_get_apn(size_t n_args, const mp_obj_t *args)
{
#if defined(PLAT_Qualcomm) || defined(PLAT_ASR_1803s) || defined(PLAT_ASR_1803sc) || (defined(PLAT_Unisoc) && !defined(BOARD_EG915UEU_AB)) \
    || defined(PLAT_ASR) || defined(PLAT_ASR_1606) || defined(PLAT_ASR_1609) || defined(PLAT_Unisoc_8910_R05) || defined(PLAT_ASR_1602) || defined(PLAT_Unisoc_8910_R06)
    if (n_args == 1)
    {
        uint8_t net_simid = mp_obj_get_int(args[0]);
        int ret = 0;
        char apn[99+1] = {0};
        if (net_simid != 0)
        {
            mp_raise_ValueError(MP_ERROR_TEXT("invalid simid."));
        }
		MP_THREAD_GIL_EXIT();
        ret = Helios_Nw_GetApn(apn, net_simid);
		MP_THREAD_GIL_ENTER();
    	if (ret == 0)
    	{
    		return mp_obj_new_str(apn, strlen(apn));
    	}
    	return mp_obj_new_int(-1);
    }
    else
#endif
    {
        if (n_args == 2) //pid/simid
        {
            uint8_t net_simid = mp_obj_get_int(args[1]);
            int ret = 0;
            uint8_t profile_id = mp_obj_get_int(args[0]);
            Helios_DataCallStartStruct pdpinfo_get;

			#if MICROPY_QPY_MODULE_DSDS
            if ((net_simid != 0) && (net_simid != 1))
			#else
			if (net_simid != 0)
			#endif
            {
                mp_raise_ValueError(MP_ERROR_TEXT("invalid simid."));
            }
            
            if ((profile_id < HELIOS_PROFILE_IDX_MIN) || (profile_id > HELIOS_PROFILE_IDX_MAX))
        	{
        		mp_raise_ValueError(MP_ERROR_TEXT("invalid profile_id."));
        	}
            memset(&pdpinfo_get, 0, sizeof(Helios_DataCallStartStruct));
			MP_THREAD_GIL_EXIT();
			#if MICROPY_QPY_MODULE_DSDS
            ret = Helios_DataCall_GetPDPContext(net_simid,profile_id, &pdpinfo_get);
			#else
            ret = Helios_DataCall_GetPDPContext(profile_id, &pdpinfo_get);
			#endif
			MP_THREAD_GIL_ENTER();
            if (ret != 0)
                return mp_obj_new_int(-1);
            
            mp_obj_t tuple[5] = {
    			mp_obj_new_int(pdpinfo_get.ip_type),
    			mp_obj_new_str(pdpinfo_get.apn, strlen(pdpinfo_get.apn)),
    			mp_obj_new_str(pdpinfo_get.user, strlen(pdpinfo_get.user)),
    			mp_obj_new_str(pdpinfo_get.pwd, strlen(pdpinfo_get.pwd)), 
    			mp_obj_new_int(pdpinfo_get.auth)};
    			
    		return mp_obj_new_tuple(5, tuple);
        }
        else
        {
            mp_raise_ValueError(MP_ERROR_TEXT("Incorrect parameter number!"));
        }
    }
}

static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qpy_net_get_apn_obj, 1, 2, qpy_net_get_apn);

#if MICROPY_QPY_MODULE_NET_LED
static mp_obj_t qpy_set_networkled_config( const mp_obj_t args)
{

	typedef struct network_led_control_info{
		int				i_gpio_num;			// GPIO_NO
		uint32_t		i_net_type;			// TCP  UDP 0-tcp 1-udp
		uint32_t		i_gpio_low_time;	// GPIO LOW times  ms
		uint32_t		i_gpio_hight_time;  // GPIO hight times ms
		uint32_t		i_loop_nums;		// cycle nums
	}network_led_control_info_t;
	
	int Helios_NW_Led_Config(network_led_control_info_t config);
	volatile network_led_control_info_t config;
	mp_obj_t *items = NULL;
	size_t len = 0;
	char *str = NULL;
	mp_obj_list_get(args, &len, &items);
	if ( len!= 5){
		return  mp_obj_new_int(-1);
	}
	config.i_gpio_num = (int)mp_obj_get_int(items[0]);
	config.i_net_type = (int)mp_obj_get_int(items[1]);
	config.i_gpio_low_time =(int) mp_obj_get_int(items[2]);
	config.i_gpio_hight_time = (int)mp_obj_get_int(items[3]);
	config.i_loop_nums =(int) mp_obj_get_int(items[4]);
	MP_THREAD_GIL_EXIT();
	Helios_NW_Led_Config(config);
	MP_THREAD_GIL_ENTER();
 	return mp_obj_new_int(0);

}
static MP_DEFINE_CONST_FUN_OBJ_1(qpy_set_networkled_config_obj, qpy_set_networkled_config);
#endif

static c_callback_t *g_net_user_callback = NULL;

static void qpy_net_event_handler(uint8_t sim_id, int32_t event_id, void *ctx)
{
	switch (event_id)
	{
		case HELIOS_NW_DATA_REG_STATUS_IND:
		{
			Helios_NwRegisterInfoStruct *nw_register_status = (Helios_NwRegisterInfoStruct *)ctx;

			if (g_net_user_callback)
			{
				QPY_NET_LOG("[net] callback start.\r\n");
				st_CallBack_Net *Net_param = malloc(sizeof(st_CallBack_Net));
				if(NULL != Net_param) 
				{																 
					Net_param->event_id = event_id;															   
					Net_param->status = nw_register_status->status;															  
					Net_param->lac = nw_register_status->lac;  
					Net_param->cid = nw_register_status->cid;  
					Net_param->act = nw_register_status->act;  
					Net_param->callback = *g_net_user_callback;
					QPY_NET_LOG("[net] callback start 1.\r\n");
					qpy_send_msg_to_callback_deal_thread(CALLBACK_TYPE_ID_NET, Net_param); 
				}
				QPY_NET_LOG("[net] callback end.\r\n");
			}
			break;
		}
	#if MICROPY_QPY_MODULE_JAMDET
		case HELIOS_NW_JAMMING_DETECT_IND:
		{
			/* ... */
			QPY_NET_LOG("[net]jamming event.\r\n", event_id);
			uint8_t jam_status = *(uint8_t *)ctx;
			if (g_net_user_callback)
			{
				MP_THREAD_GIL_ENTER();
				GC_STACKTOP_SET();
				mp_obj_t tuple[3] = 
    			{
    				mp_obj_new_int(event_id),
    				mp_obj_new_int(jam_status),
    				mp_obj_new_int(sim_id)
    			};
				mp_sched_schedule_ex(g_net_user_callback, mp_obj_new_tuple(3, tuple));
				GC_STACKTOP_CLEAR();
				MP_THREAD_GIL_EXIT();
			}
			break;
		}
	#endif
		case HELIOS_NW_VOICE_REG_STATUS_IND:
			/* ... */
			QPY_NET_LOG("[net] ind_flag = %x\r\n", event_id);
			break;
		case HELIOS_NW_NITZ_TIME_UPDATE_IND:
			/* ... */
			QPY_NET_LOG("[net] ind_flag = %x\r\n", event_id);
			break;
		case HELIOS_NW_SIGNAL_QUALITY_IND:
			/* ... */
			QPY_NET_LOG("[net] ind_flag = %x\r\n", event_id);
			break;
		default:
			QPY_NET_LOG("[net] event handler, ind=%x\r\n", event_id);
			break;
	}
}

/*=============================================================================*/
/* FUNCTION: qpy_net_add_event_handler                                         */
/*=============================================================================*/
/*!@brief: registered user callback function
 *	
 * @handler	[in] 	callback function
 *
 * @return:
 *     0	-	success
 *    -1	-	error
 */
/*=============================================================================*/
static mp_obj_t qpy_net_add_event_handler(mp_obj_t handler)
{
    static c_callback_t cb = {0};
    memset(&cb, 0, sizeof(c_callback_t));
	cb.arg = mp_obj_new_tuple(5, NULL);
	g_net_user_callback = &cb;
	mp_sched_schedule_callback_register(g_net_user_callback, handler);
	Helios_NwInitStruct info;
    memset(&info, 0, sizeof(Helios_NwInitStruct));

	info.user_cb = qpy_net_event_handler;
	Helios_Nw_Init(&info);
	return mp_obj_new_int(0);
}
static MP_DEFINE_CONST_FUN_OBJ_1(qpy_net_add_event_handler_obj, qpy_net_add_event_handler);

/*=============================================================================*/
/* FUNCTION: qpy_module_net_deinit                                         */
/*=============================================================================*/
/*!@brief: deinit net module
 *	
 *
 * @return:
 *     0	-	success
 */
/*=============================================================================*/
static mp_obj_t qpy_module_net_deinit(void)
{
	QPY_NET_LOG("module net deinit.\r\n");
#if defined(PLAT_ASR) || defined(PLAT_Unisoc) || defined(PLAT_Unisoc_8910_R05) || defined(PLAT_Unisoc_8910_R06)
	MP_THREAD_GIL_EXIT();
	Helios_Nw_Deinit();
	MP_THREAD_GIL_ENTER();
#endif
	g_net_user_callback = NULL;
	return mp_obj_new_int(0);
}
static MP_DEFINE_CONST_FUN_OBJ_0(qpy_module_net_deinit_obj, qpy_module_net_deinit);

#if MICROPY_QPY_MODULE_FTM_MODE
#include "helios_ftm.h"
#include "helios_os.h"
static mp_obj_t qpy_ftm_mode_switch(mp_obj_t mode)
{
	int ret = -1;
	uint8_t ftm_mode = (uint8_t)mp_obj_get_int(mode);
	if(ftm_mode > 1) return mp_obj_new_int(-1);

	MP_THREAD_GIL_EXIT();
	if(ftm_mode)
	{
		ret = Helios_Dev_SetModemFunction(!ftm_mode, 0, 0);
		Helios_sleep(1);
		ret = (!ret) ? helios_ftm_mode_switch(ftm_mode) : -1;
	}
	else
	{
		ret = helios_ftm_mode_switch(ftm_mode);
		Helios_sleep(1);
		ret = (!ret) ? Helios_Dev_SetModemFunction(!ftm_mode, 0, 0) : -1;
	}
	MP_THREAD_GIL_ENTER();	

	return mp_obj_new_int(ret);
}
static MP_DEFINE_CONST_FUN_OBJ_1(qpy_ftm_mode_switch_obj, qpy_ftm_mode_switch);

static mp_obj_t qpy_ftm_mode_start(mp_obj_t band, mp_obj_t channel, mp_obj_t power)
{
	int ret = -1;

	MP_THREAD_GIL_EXIT();
	ret = helios_ftm_test_start((uint16_t)mp_obj_get_int(band), (uint32_t)mp_obj_get_int(channel), (uint8_t)mp_obj_get_int(power));
	MP_THREAD_GIL_ENTER();	

	return mp_obj_new_int(ret);
}
static MP_DEFINE_CONST_FUN_OBJ_3(qpy_ftm_mode_start_obj, qpy_ftm_mode_start);

static mp_obj_t qpy_ftm_mode_stop(void)
{
	int ret = -1;
	MP_THREAD_GIL_EXIT();
	//uint16_t band, uint32_t tx_channel,uint8_t tx_power
	ret = helios_ftm_test_stop();
	MP_THREAD_GIL_ENTER();	

	return mp_obj_new_int(ret);
}
static MP_DEFINE_CONST_FUN_OBJ_0(qpy_ftm_mode_stop_obj, qpy_ftm_mode_stop);
#endif



#if MICROPY_QPY_MODULE_BACKCELL

static mp_obj_t qpy_add_black_cell(size_t n_args, const mp_obj_t *args)
{
	int8_t ret = -1;
	uint8_t rat =5;
	if (n_args == 2){
		rat = mp_obj_get_int(args[1]);
	}
	mp_buffer_info_t cell_info = {0};
	mp_get_buffer_raise(args[0], &cell_info, MP_BUFFER_READ);
	MP_THREAD_GIL_EXIT();
	ret = Helios_Nw_AddBlackCell(rat, (char *)cell_info.buf);
	MP_THREAD_GIL_ENTER();
	if (ret != 0){
		ret =-1;
	}
	return mp_obj_new_int(ret);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qpy_add_black_cell_obj, 1, 2, qpy_add_black_cell);

static mp_obj_t qpy_get_black_cell(void)
{
	int8_t ret = 0;
	int8_t i = 0;
	mp_obj_t list_gsm  = mp_obj_new_list(0, NULL);
	mp_obj_t list_lte  = mp_obj_new_list(0, NULL);
	Helios_NwBackCellInfo info;
	memset(&info, 0, sizeof(Helios_NwBackCellInfo));
	MP_THREAD_GIL_EXIT();
	ret = Helios_Nw_GetBlackCell(&info);
	MP_THREAD_GIL_ENTER();
	if (ret == 0){
		if (info.gsm_info_num > 0)
		{
			for (i=0; i<info.gsm_info_num; i++)
			{
				mp_obj_list_append(list_gsm, mp_obj_new_str(info.gsm_cell_info[i], strlen(info.gsm_cell_info[i])));
			}			
		}
		if (info.lte_info_num > 0)
		{
			for (i=0; i<info.lte_info_num; i++)
			{
				mp_obj_list_append(list_lte, mp_obj_new_str(info.lte_cell_info[i], strlen(info.lte_cell_info[i])));
			}
		}
		mp_obj_t list[2] = {list_gsm, list_lte};
		return mp_obj_new_tuple(2, list);
	}
	if (ret != 0){
		ret =-1;
	}
	return mp_obj_new_int(ret);
}
static MP_DEFINE_CONST_FUN_OBJ_0(qpy_get_black_cell_obj, qpy_get_black_cell);

static mp_obj_t qpy_get_black_cell_cfg(void)
{

	int8_t ret = 0;
	int blackcellCfg = 0;
	int blackcellCfg_num = 0;
	MP_THREAD_GIL_EXIT();
	ret = Helios_Nw_GetBlackCellCfg(&blackcellCfg,&blackcellCfg_num);
	MP_THREAD_GIL_ENTER();
	if (ret == 0)
	{
		mp_obj_t list[2] = {mp_obj_new_int(blackcellCfg), mp_obj_new_int(blackcellCfg_num)};
		return mp_obj_new_tuple(2, list);
	}
	if (ret != 0){
		ret =-1;
	}
	return mp_obj_new_int(ret);
}
static MP_DEFINE_CONST_FUN_OBJ_0(qpy_get_black_cell_cfg_obj, qpy_get_black_cell_cfg);


static mp_obj_t qpy_set_black_cell_cfg(size_t n_args, const mp_obj_t *args)
{
	int8_t ret = -1;
	uint8_t blackcellCfg = mp_obj_get_int(args[0]);
	if (blackcellCfg > 1)
	{
		mp_raise_ValueError(MP_ERROR_TEXT("invalid value, blackcellCfg should be in [0,1]."));
	}
	uint8_t blackcellCfg_num = mp_obj_get_int(args[1]);
	if (blackcellCfg_num == 0 || blackcellCfg_num > 8)
	{
			mp_raise_ValueError(MP_ERROR_TEXT("invalid value, blackcellCfg_num should be in [1,8]."));
	}
	MP_THREAD_GIL_EXIT();
	ret = Helios_Nw_SetBlackCellCfg(blackcellCfg, blackcellCfg_num);
	MP_THREAD_GIL_ENTER();
	if (ret != 0){
		ret =-1;
	}
	return mp_obj_new_int(ret);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qpy_set_black_cell_cfg_obj, 2, 2, qpy_set_black_cell_cfg);

static mp_obj_t qpy_delete_black_cell(size_t n_args, const mp_obj_t *args)
{
	int8_t ret = -1;
	if (n_args == 0)
	{
		ret = Helios_Nw_DeleteAllBlackCell();
	}
	else if (n_args == 1)
	{
		uint8_t rat = 5;
		mp_buffer_info_t cell_info = {0};
		mp_get_buffer_raise(args[0], &cell_info, MP_BUFFER_READ);
		MP_THREAD_GIL_EXIT();
		ret = Helios_Nw_DeleteBlackCell(rat, (char *)cell_info.buf);
		MP_THREAD_GIL_ENTER();
	}
	if (ret != 0){
		ret =-1;
	}
	return mp_obj_new_int(ret);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qpy_delete_black_cell_obj, 0, 1, qpy_delete_black_cell);
#endif


#if MICROPY_QPY_MODULE_DRX_TIMER
static mp_obj_t qpy_net_get_drxtm(void)
{
	uint8_t cur_simid = _get_current_simid();
	uint16_t drx_timer_value = 0;
	MP_THREAD_GIL_EXIT();
	if(0 != Helios_Nw_GetDrxtm(cur_simid, &drx_timer_value)) {
		MP_THREAD_GIL_ENTER();
		return mp_obj_new_int(-1);
	}
	MP_THREAD_GIL_ENTER();
	return mp_obj_new_int(drx_timer_value);
}
static MP_DEFINE_CONST_FUN_OBJ_0(qpy_net_get_drxtm_obj, qpy_net_get_drxtm);

static mp_obj_t qpy_net_set_drxtm(mp_obj_t drx_timer)
{
	uint8_t cur_simid = _get_current_simid();
	uint16_t drx_timer_value = (uint16_t)mp_obj_get_int(drx_timer);
	MP_THREAD_GIL_EXIT();
	if(0 != Helios_Nw_SetDrxtm(cur_simid, drx_timer_value)) {
		MP_THREAD_GIL_ENTER();
		return mp_obj_new_int(-1);
	}
	MP_THREAD_GIL_ENTER();
	return mp_obj_new_int(0);
}
static MP_DEFINE_CONST_FUN_OBJ_1(qpy_net_set_drxtm_obj, qpy_net_set_drxtm);
#endif

#if MICROPY_QPY_MODULE_JAMDET
static mp_obj_t qpy_jamdet_set_switch(size_t n_args, const mp_obj_t *args)
{
    uint8_t sim_id = 0;
	
    uint8_t opt = (uint8_t)mp_obj_get_int(args[0]);
    if ((opt != 0) && (opt != 1))
    {
        mp_raise_ValueError("invalid value, opt shuould be 0 or 1,");
    }

	if (n_args == 2)
	{
		sim_id = (uint8_t)mp_obj_get_int(args[1]);
		if ((sim_id != 0) && (sim_id != 1))
		{
			mp_raise_ValueError("invalid value, simId should be 0 or 1.");
		}
	}
	
    MP_THREAD_GIL_EXIT();
    int ret = Helios_Nw_SetJamdetSwitch(sim_id, opt);
    MP_THREAD_GIL_ENTER();
	return mp_obj_new_int(ret);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qpy_jamdet_set_switch_obj, 1, 2, qpy_jamdet_set_switch);

static mp_obj_t qpy_jamdet_get_switch(size_t n_args, const mp_obj_t *args)
{
    uint8_t sim_id = 0;
	if (n_args == 1)
	{
		sim_id = (uint8_t)mp_obj_get_int(args[0]);
		if ((sim_id != 0) && (sim_id != 1))
		{
			mp_raise_ValueError("invalid value, simId should be 0 or 1.");
		}
	}
	uint8_t opt = 0;
    MP_THREAD_GIL_EXIT();
    Helios_Nw_GetJamdetSwitch(sim_id, &opt);
    MP_THREAD_GIL_ENTER();
	
	return mp_obj_new_int(opt);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qpy_jamdet_get_switch_obj, 0, 1, qpy_jamdet_get_switch);

static mp_obj_t qpy_jamdet_set_configuration(size_t n_args, const mp_obj_t *args)
{
	uint8_t sim_id = 0;
	if (n_args == 8)
	{
		sim_id = (uint8_t)mp_obj_get_int(args[7]);
		if ((sim_id != 0) && (sim_id != 1))
		{
			mp_raise_ValueError("invalid value, simId should be 0 or 1.");
		}
	}

	Helios_NwJamDetConfigStruct cfg = {0};
	cfg.gsm_minch = mp_obj_get_int(args[0]);
	cfg.gsm_sinr  = mp_obj_get_int(args[1]);
	cfg.gsm_rssi  = mp_obj_get_int(args[2]);
	cfg.lte_rsrp  = mp_obj_get_int(args[3]);
	cfg.lte_rsrq  = mp_obj_get_int(args[4]);
	cfg.lte_rssi  = mp_obj_get_int(args[5]);
	cfg.shake_period = mp_obj_get_int(args[6]);

	if ((cfg.gsm_minch < 0) || (cfg.gsm_minch > 254))
	{
		mp_raise_ValueError("invalid value, gsm_minch should be in [0,254].");
	}
	if ((cfg.gsm_sinr < 0) || (cfg.gsm_sinr > 63))
	{
		mp_raise_ValueError("invalid value, gsm_sinr should be in [0,63].");
	}
	if ((cfg.gsm_rssi < -110) || (cfg.gsm_rssi > -50))
	{
		mp_raise_ValueError("invalid value, gsm_rssi should be in [-110,-50].");
	}
	if ((cfg.lte_rsrp < -140) || (cfg.lte_rsrp > -44))
	{
		mp_raise_ValueError("invalid value, lte_rsrp should be in [-140,-44].");
	}
	if ((cfg.lte_rsrq < -19) || (cfg.lte_rsrq > -3))
	{
		mp_raise_ValueError("invalid value, lte_rsrq should be in [-19,-3].");
	}
	if ((cfg.lte_rssi < -120) || (cfg.lte_rssi > -20))
	{
		mp_raise_ValueError("invalid value, lte_rssi should be in [-120,-20].");
	}
	if ((cfg.shake_period < 1) || (cfg.shake_period > 10))
	{
		mp_raise_ValueError("invalid value, shake_period should be in [1,10].");
	}

	MP_THREAD_GIL_EXIT();
    int ret = Helios_Nw_SetJamdetConfiguration(sim_id, &cfg);
    MP_THREAD_GIL_ENTER();
	
	return mp_obj_new_int(ret);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qpy_jamdet_set_config_obj, 7, 8, qpy_jamdet_set_configuration);

static mp_obj_t qpy_jamdet_get_configuration(size_t n_args, const mp_obj_t *args)
{
	uint8_t sim_id = 0;
	if (n_args == 1)
	{
		sim_id = (uint8_t)mp_obj_get_int(args[0]);
		if ((sim_id != 0) && (sim_id != 1))
		{
			mp_raise_ValueError("invalid value, simId should be 0 or 1.");
		}
	}
	
	Helios_NwJamDetConfigStruct cfg = {0};
	MP_THREAD_GIL_EXIT();
    Helios_Nw_GetJamdetConfiguration(sim_id, &cfg);
    MP_THREAD_GIL_ENTER();
	mp_obj_t info[7] = 
	{
		mp_obj_new_int(cfg.gsm_minch),
		mp_obj_new_int(cfg.gsm_sinr),
		mp_obj_new_int(cfg.gsm_rssi),
		mp_obj_new_int(cfg.lte_rsrp),
		mp_obj_new_int(cfg.lte_rsrq),
		mp_obj_new_int(cfg.lte_rssi),
		mp_obj_new_int(cfg.shake_period)
	};
	return mp_obj_new_tuple(7, info);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qpy_jamdet_get_config_obj, 0, 1, qpy_jamdet_get_configuration);

static mp_obj_t qpy_jamdet_get_stauts(size_t n_args, const mp_obj_t *args)
{
	uint8_t sim_id = 0;
	if (n_args == 1)
	{
		sim_id = (uint8_t)mp_obj_get_int(args[0]);
		if ((sim_id != 0) && (sim_id != 1))
		{
			mp_raise_ValueError("invalid value, simId should be 0 or 1.");
		}
	}

	uint8_t opt = 0;
    MP_THREAD_GIL_EXIT();
    Helios_Nw_GetJamdetSwitch(sim_id, &opt);
    MP_THREAD_GIL_ENTER();
	if (opt == 0)
	{
		mp_raise_ValueError("Please enable the jamdet function first.");
	}

	uint8_t status = 0;
	MP_THREAD_GIL_EXIT();
    int ret = Helios_Nw_GetJamdetStatus(sim_id, &status);
    MP_THREAD_GIL_ENTER();
	if (ret == 0)
	{
		return mp_obj_new_int(status);
	}
	return mp_obj_new_int(ret);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qpy_jamdet_get_stauts_obj, 0, 1, qpy_jamdet_get_stauts);

#endif

#if defined(MICROPY_QPY_MODULE_IMS_STATUS)
static mp_obj_t qpy_net_get_ims_register(size_t n_args, const mp_obj_t *args)
{
	uint8_t sim_id = _get_current_simid();;
	if (n_args == 1) {
		sim_id = mp_obj_get_int(args[0]);
	}
    if ((sim_id != 0) && (sim_id != 1))
    {
        mp_raise_ValueError("invalid value, simId should be 0 or 1.");
    }
	MP_THREAD_GIL_EXIT();
    int ret = Helios_Nw_ImsIsRegister(sim_id);
    MP_THREAD_GIL_ENTER();
    return mp_obj_new_int(ret);
}
static  MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qpy_net_get_ims_register_obj, 0, 1, qpy_net_get_ims_register);
#endif

#if MICROPY_QPY_MODULE_NET_EX

static mp_obj_t qpy_net_get_T3402(void)
{
    char resp[128] = {0};
	char para[64] = {0};
	MP_THREAD_GIL_EXIT();
	HELIOS_AT_RESP_STATUS_E ret = Helios_Atcmd_Send_Sync(0, "AT+QCFG=\"emmtimer\"\r\n", resp, sizeof(resp), NULL, 10);
	MP_THREAD_GIL_ENTER();
	if(ret == HELIOS_AT_RESP_OK) {
		Helios_Atcmd_Get_Para_Value_Str(resp, 1, 1, para, sizeof(para));
		if(strlen(para) != 0) {
			return mp_obj_new_str(para, strlen(para));
		}
	}
	return mp_obj_new_int(-1);
}
static MP_DEFINE_CONST_FUN_OBJ_0(qpy_net_get_T3402_obj, qpy_net_get_T3402);


static mp_obj_t qpy_net_get_T3412(void)
{
    char resp[128] = {0};
	char para[64] = {0};
	MP_THREAD_GIL_EXIT();
	HELIOS_AT_RESP_STATUS_E ret = Helios_Atcmd_Send_Sync(0, "AT+QCFG=\"emmtimer\"\r\n", resp, sizeof(resp), NULL, 10);
	MP_THREAD_GIL_ENTER();
	if(ret == HELIOS_AT_RESP_OK) {
		Helios_Atcmd_Get_Para_Value_Str(resp, 1, 2, para, sizeof(para));
		if(strlen(para) != 0) {
			return mp_obj_new_str(para, strlen(para));
		}
	}
	return mp_obj_new_int(-1);
}
static MP_DEFINE_CONST_FUN_OBJ_0(qpy_net_get_T3412_obj, qpy_net_get_T3412);


static mp_obj_t qpy_net_get_T3324(void)
{
    char resp[128] = {0};
	char para[64] = {0};
	char cmd[64] = {0};
	int n = 0;
	HELIOS_AT_RESP_STATUS_E ret = HELIOS_AT_RESP_OK;
	MP_THREAD_GIL_EXIT();
	ret = Helios_Atcmd_Send_Sync(0, "AT+CEREG?\r\n", resp, sizeof(resp), NULL, 10);
	MP_THREAD_GIL_ENTER();
	if(ret != HELIOS_AT_RESP_OK) {
		return mp_obj_new_int(-1);
	}
	Helios_Atcmd_Get_Para_Value_Str(resp, 1, 0, para, sizeof(para));
	if(strlen(para) != 0) {
		n = atoi(para);
	}
	memset(resp, 0 ,sizeof(resp));
	memset(para, 0 ,sizeof(para));
	if (n !=4) {
		MP_THREAD_GIL_EXIT();
		ret = Helios_Atcmd_Send_Sync(0, "AT+CEREG=4\r\n", NULL, 0, NULL, 10);
		MP_THREAD_GIL_ENTER();
		if(ret != HELIOS_AT_RESP_OK) {
			return mp_obj_new_int(-1);
		}
	}else {
		Helios_Atcmd_Get_Para_Value_Str(resp, 1, 7, para, sizeof(para));
		if(strlen(para) != 0) {
			return mp_obj_new_str(para, strlen(para));
		}
	}
	MP_THREAD_GIL_EXIT();
	Helios_Atcmd_Send_Sync(0, "AT+CEREG?\r\n", resp, sizeof(resp), NULL, 10);
	MP_THREAD_GIL_ENTER();
	Helios_Atcmd_Get_Para_Value_Str(resp, 1, 7, para, sizeof(para));
	snprintf(cmd, 64, "AT+CEREG=%d\r\n", n);
	MP_THREAD_GIL_EXIT();
	ret = Helios_Atcmd_Send_Sync(0, cmd, NULL, 0, NULL, 10);
	MP_THREAD_GIL_ENTER();
	if(ret != HELIOS_AT_RESP_OK) {
		return mp_obj_new_int(-1);
	}
	return mp_obj_new_str(para, strlen(para));

}
static MP_DEFINE_CONST_FUN_OBJ_0(qpy_net_get_T3324_obj, qpy_net_get_T3324);


static mp_obj_t qpy_net_get_TeDRX(void)
{
	char resp[128] = {0};
	char para[64] = {0};
	MP_THREAD_GIL_EXIT();
	HELIOS_AT_RESP_STATUS_E ret = Helios_Atcmd_Send_Sync(0, "AT+CEDRXRDP\r\n", resp, sizeof(resp), NULL, 10);
	MP_THREAD_GIL_ENTER();
	if(ret == HELIOS_AT_RESP_OK) {
		Helios_Atcmd_Get_Para_Value_Str(resp, 1, 1, para, sizeof(para));
		return mp_obj_new_str(para, strlen(para));
	}
	return mp_obj_new_int(-1);
}
static MP_DEFINE_CONST_FUN_OBJ_0(qpy_net_get_TeDRX_obj, qpy_net_get_TeDRX);

static mp_obj_t qpy_net_get_TPTW(void)
{
	char resp[128] = {0};
	char para[64] = {0};
	MP_THREAD_GIL_EXIT();
	HELIOS_AT_RESP_STATUS_E ret = Helios_Atcmd_Send_Sync(0, "AT+CEDRXRDP\r\n", resp, sizeof(resp), NULL, 10);
	MP_THREAD_GIL_ENTER();
	if(ret == HELIOS_AT_RESP_OK) {
		Helios_Atcmd_Get_Para_Value_Str(resp, 1, 3, para, sizeof(para));
		return mp_obj_new_str(para, strlen(para));
	}
	return mp_obj_new_int(-1);
}
static MP_DEFINE_CONST_FUN_OBJ_0(qpy_net_get_TPTW_obj, qpy_net_get_TPTW);

static mp_obj_t qpy_net_get_qRxlevMin(void)
{
	char resp[256] = {0};
	char para[64] = {0};
	MP_THREAD_GIL_EXIT();
	HELIOS_AT_RESP_STATUS_E ret = Helios_Atcmd_Send_Sync(0, "AT+QCFG=\"sibinfo\"\r\n", resp, sizeof(resp), NULL, 10);
	MP_THREAD_GIL_ENTER();
	if(ret == HELIOS_AT_RESP_OK) {
		Helios_Atcmd_Get_Para_Value_Str(resp, 1, 3, para, sizeof(para));
		if(strlen(para) != 0) {
			return mp_obj_new_str(para, strlen(para));
		}
	}
	return mp_obj_new_int(-1);
}
static MP_DEFINE_CONST_FUN_OBJ_0(qpy_net_get_qRxlevMin_obj, qpy_net_get_qRxlevMin);

static mp_obj_t qpy_net_get_reject_cause(void)
{
	char resp[128] = {0};
	char para[64] = {0};
	MP_THREAD_GIL_EXIT();
	HELIOS_AT_RESP_STATUS_E ret = Helios_Atcmd_Send_Sync(0, "AT+QCFG=\"emmcause\"\r\n", resp, sizeof(resp), NULL, 10);
	MP_THREAD_GIL_ENTER();
	if(ret == HELIOS_AT_RESP_OK) {
		Helios_Atcmd_Get_Para_Value_Str(resp, 1, 1, para, sizeof(para));
		return mp_obj_new_str(para, strlen(para));
	}
	return mp_obj_new_int(-1);
}
static MP_DEFINE_CONST_FUN_OBJ_0(qpy_net_get_reject_cause_obj, qpy_net_get_reject_cause);
#endif


static const mp_rom_map_elem_t net_module_globals_table[] = {
	{ MP_OBJ_NEW_QSTR(MP_QSTR___name__), 	MP_ROM_QSTR(MP_QSTR_net) 					},
	{ MP_ROM_QSTR(MP_QSTR___qpy_module_deinit__),   MP_ROM_PTR(&qpy_module_net_deinit_obj) },
	{ MP_ROM_QSTR(MP_QSTR_csqQueryPoll), 	MP_ROM_PTR(&qpy_net_get_csq_obj) 			},
	{ MP_ROM_QSTR(MP_QSTR_getState), 		MP_ROM_PTR(&qpy_net_get_reg_status_obj) 	},
	{ MP_ROM_QSTR(MP_QSTR_getConfig), 		MP_ROM_PTR(&qpy_net_get_configuration_obj) 	},
    { MP_ROM_QSTR(MP_QSTR_setConfig), 		MP_ROM_PTR(&qpy_net_set_configuration_obj) 	},
	{ MP_ROM_QSTR(MP_QSTR_nitzTime), 		MP_ROM_PTR(&qpy_net_get_nitz_time_obj) 		},
	{ MP_ROM_QSTR(MP_QSTR_operatorName), 	MP_ROM_PTR(&qpy_net_get_operator_name_obj) 	},
	//{ MP_ROM_QSTR(MP_QSTR_setNetMode), 		MP_ROM_PTR(&qpy_net_set_selection_obj) 		},	
	{ MP_ROM_QSTR(MP_QSTR_getNetMode), 		MP_ROM_PTR(&qpy_net_get_selection_obj) 		},	
	{ MP_ROM_QSTR(MP_QSTR_getSignal), 		MP_ROM_PTR(&qpy_net_get_signal_strength_obj)},
	{ MP_ROM_QSTR(MP_QSTR_getCellInfo), 	MP_ROM_PTR(&qpy_net_get_cell_info_obj) 		},
	{ MP_ROM_QSTR(MP_QSTR_getCellInfos), 	MP_ROM_PTR(&qpy_net_get_cell_info_v2_obj) 	},

	{ MP_ROM_QSTR(MP_QSTR_getCi), 			MP_ROM_PTR(&qpy_net_get_cid_obj) 			},
	{ MP_ROM_QSTR(MP_QSTR_getLac), 			MP_ROM_PTR(&qpy_net_get_lac_obj) 			},
	{ MP_ROM_QSTR(MP_QSTR_getMnc), 			MP_ROM_PTR(&qpy_net_get_mnc_obj) 			},
	{ MP_ROM_QSTR(MP_QSTR_getMcc), 			MP_ROM_PTR(&qpy_net_get_mcc_obj) 			},
	{ MP_ROM_QSTR(MP_QSTR_setModemFun), 	MP_ROM_PTR(&qpy_net_set_modem_fun_obj) 		},
	{ MP_ROM_QSTR(MP_QSTR_getModemFun), 	MP_ROM_PTR(&qpy_net_get_modem_fun_obj) 		},
	{ MP_ROM_QSTR(MP_QSTR_setCallback), 	MP_ROM_PTR(&qpy_net_add_event_handler_obj) 	},

    { MP_ROM_QSTR(MP_QSTR_setApn), 	MP_ROM_PTR(&qpy_net_set_apn_obj) 	},
    { MP_ROM_QSTR(MP_QSTR_getApn), 	MP_ROM_PTR(&qpy_net_get_apn_obj) 	},
	#if MICROPY_QPY_MODULE_NET_LED
	{ MP_ROM_QSTR(MP_QSTR_set_networkled_config), 	MP_ROM_PTR(&qpy_set_networkled_config_obj) 	},
	#endif
    { MP_ROM_QSTR(MP_QSTR_getServingCi),    MP_ROM_PTR(&qpy_net_get_cid_servingcell_obj) },
	{ MP_ROM_QSTR(MP_QSTR_getServingLac), 	MP_ROM_PTR(&qpy_net_get_lac_servingcell_obj) },
	{ MP_ROM_QSTR(MP_QSTR_getServingMnc), 	MP_ROM_PTR(&qpy_net_get_mnc_servingcell_obj) },
	{ MP_ROM_QSTR(MP_QSTR_getServingMcc), 	MP_ROM_PTR(&qpy_net_get_mcc_servingcell_obj) },
#if defined(PLAT_Qualcomm) || defined(PLAT_ASR)|| MICROPY_QPY_MODULE_NET_BAND
	{ MP_ROM_QSTR(MP_QSTR_setBand), 		MP_ROM_PTR(&qpy_net_set_band_obj) },
	{ MP_ROM_QSTR(MP_QSTR_getBand), 		MP_ROM_PTR(&qpy_net_get_band_obj) },
#endif
#if defined(PLAT_ASR) || MICROPY_QPY_MODULE_NET_BAND
    { MP_ROM_QSTR(MP_QSTR_bandRst),         MP_ROM_PTR(&qpy_net_band_restore_obj) },
#endif

#if MICROPY_QPY_MODULE_FTM_MODE
	{ MP_ROM_QSTR(MP_QSTR_ftmModeSwitch),         MP_ROM_PTR(&qpy_ftm_mode_switch_obj) },
	{ MP_ROM_QSTR(MP_QSTR_ftmTestStart),         MP_ROM_PTR(&qpy_ftm_mode_start_obj) },
	{ MP_ROM_QSTR(MP_QSTR_ftmTestStop),         MP_ROM_PTR(&qpy_ftm_mode_stop_obj) },
#endif

#if MICROPY_QPY_MODULE_BACKCELL
	{ MP_ROM_QSTR(MP_QSTR_addBlackCell),         MP_ROM_PTR(&qpy_add_black_cell_obj) },
	{ MP_ROM_QSTR(MP_QSTR_getBlackCell),         MP_ROM_PTR(&qpy_get_black_cell_obj) },
	{ MP_ROM_QSTR(MP_QSTR_getBlackCellCfg),         MP_ROM_PTR(&qpy_get_black_cell_cfg_obj) },
	{ MP_ROM_QSTR(MP_QSTR_setBlackCellCfg),         MP_ROM_PTR(&qpy_set_black_cell_cfg_obj) },
	{ MP_ROM_QSTR(MP_QSTR_deleteBlackCell),         MP_ROM_PTR(&qpy_delete_black_cell_obj) },
#endif

#if MICROPY_QPY_MODULE_DRX_TIMER
    { MP_ROM_QSTR(MP_QSTR_setDrxTm), 	MP_ROM_PTR(&qpy_net_set_drxtm_obj) 	},
    { MP_ROM_QSTR(MP_QSTR_getDrxTm), 	MP_ROM_PTR(&qpy_net_get_drxtm_obj) 	},
#endif

#if MICROPY_QPY_MODULE_JAMDET
    { MP_ROM_QSTR(MP_QSTR_setJamdetSwitch), 	MP_ROM_PTR(&qpy_jamdet_set_switch_obj) 	},
    { MP_ROM_QSTR(MP_QSTR_getJamdetSwitch), 	MP_ROM_PTR(&qpy_jamdet_get_switch_obj) 	},
	{ MP_ROM_QSTR(MP_QSTR_setJamdetParam), 		MP_ROM_PTR(&qpy_jamdet_set_config_obj) 	},
	{ MP_ROM_QSTR(MP_QSTR_getJamdetParam), 		MP_ROM_PTR(&qpy_jamdet_get_config_obj) 	},
	{ MP_ROM_QSTR(MP_QSTR_getJamdetStatus), 	MP_ROM_PTR(&qpy_jamdet_get_stauts_obj) 	},
#endif

#if defined(MICROPY_QPY_MODULE_IMS_STATUS)
    { MP_ROM_QSTR(MP_QSTR_getImsRegister),     MP_ROM_PTR(&qpy_net_get_ims_register_obj) },
#endif

#if MICROPY_QPY_MODULE_NET_EX
	{ MP_ROM_QSTR(MP_QSTR_getT3402),     		 MP_ROM_PTR(&qpy_net_get_T3402_obj) },
	{ MP_ROM_QSTR(MP_QSTR_getT3412),     		 MP_ROM_PTR(&qpy_net_get_T3412_obj) },
	{ MP_ROM_QSTR(MP_QSTR_getT3324),     		 MP_ROM_PTR(&qpy_net_get_T3324_obj) },
	{ MP_ROM_QSTR(MP_QSTR_getTeDRX),     		 MP_ROM_PTR(&qpy_net_get_TeDRX_obj) },
	{ MP_ROM_QSTR(MP_QSTR_getTPTW),     	 	 MP_ROM_PTR(&qpy_net_get_TPTW_obj) },
	{ MP_ROM_QSTR(MP_QSTR_getqRxlevMin),     	 MP_ROM_PTR(&qpy_net_get_qRxlevMin_obj) },
	{ MP_ROM_QSTR(MP_QSTR_getRejectCause),     	 MP_ROM_PTR(&qpy_net_get_reject_cause_obj) },
#endif
};

static MP_DEFINE_CONST_DICT(net_module_globals, net_module_globals_table);
const mp_obj_module_t mp_module_net = {
	.base = { &mp_type_module },
	.globals = (mp_obj_dict_t *)&net_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_net, mp_module_net);
#endif /* MICROPY_QPY_MODULE_NET */
