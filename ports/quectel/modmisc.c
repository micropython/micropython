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
#include <string.h>
#include "mpconfigport.h"

#include "py/compile.h"
#include "py/runtime.h"
#include "py/repl.h"
#include "py/mperrno.h"
#include "py/obj.h"  // Pawn 2020-12-19 Add replEnable

#if MICROPY_QPY_MODULE_MISC

#include "helios_dev.h"
#include "modmisc.h"

#if MICROPY_QPY_MISC_PWM
#include "helios_gpio.h"
#endif

int repl_protect_enable=0;
#if MICROPY_PY_REPL_PASSWORD_PROTECT
extern int repl_protect_password_enable;
extern char g_repl_protect_pswd[64+1];
#endif

static mp_obj_t qpy_misc_set_replEnable(size_t n_args, const mp_obj_t *args)
{
	int flag;
 	
	flag = mp_obj_get_int(args[0]);
#if !MICROPY_PY_REPL_PASSWORD_PROTECT
	if (flag != 0 && flag != 1)
	{
		return mp_obj_new_int(-1);
	}
#else
    if (flag != 0 && flag != 1 && flag != 2)
	{
		return mp_obj_new_int(-1);
	}

    if (flag == 2) {
        if (n_args > 1) {
            return mp_obj_new_int(-1);
        } else {
            if ((strlen(g_repl_protect_pswd) > 0) && (repl_protect_enable == 1)) {
                return mp_obj_new_int(4); //repl-protection by password
            } else if ((strlen(g_repl_protect_pswd) <= 0) && (repl_protect_enable == 1)) {
                return mp_obj_new_int(3); //repl refuse
            } else if ((strlen(g_repl_protect_pswd) > 0) && (repl_protect_enable == 0)) {
                return mp_obj_new_int(2); //repl enable but The password has already been set
            } else {
                return mp_obj_new_int(1); //repl enable
            }
        }
    }
    
	mp_buffer_info_t bufinfo = {0};
    if (strlen(g_repl_protect_pswd) > 0)
    {
        if (n_args > 1)
        {
            mp_get_buffer_raise(args[1], &bufinfo, MP_BUFFER_READ);
            if ((bufinfo.len > 0) && (strcmp(g_repl_protect_pswd, bufinfo.buf) == 0))
            {
                repl_protect_enable = flag;
            }
            else
            {
                return mp_obj_new_int(-1);
            }
        }
        else
        {
            return mp_obj_new_int(-1);
        }
    }
    else
#endif
    {
	    repl_protect_enable = flag;
    }
  
	return mp_obj_new_int(0);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qpy_misc_set_replEnable_obj, 1, 2, qpy_misc_set_replEnable);

static mp_obj_t qpy_misc_set_repl_password(size_t n_args, const mp_obj_t *args)
{
#if MICROPY_PY_REPL_PASSWORD_PROTECT
    mp_buffer_info_t bufinfo[2] = {0};
 	mp_get_buffer_raise(args[0], &bufinfo[0], MP_BUFFER_READ);

    if (strlen(g_repl_protect_pswd) > 0) /*repl-protection by password*/
    {
        if (n_args < 2)
            return mp_obj_new_int(-1);

        mp_get_buffer_raise(args[1], &bufinfo[1], MP_BUFFER_READ);
        if (bufinfo[1].len > 0)
        {
            if (strcmp(g_repl_protect_pswd, bufinfo[1].buf) == 0)
            {
                memset(g_repl_protect_pswd, 0, sizeof(g_repl_protect_pswd));
                if (bufinfo[0].len > 0)
                {
                    repl_protect_password_enable = 1;
                    strncpy(g_repl_protect_pswd, bufinfo[0].buf, bufinfo[0].len);
                }
                else
                {
                    repl_protect_password_enable = 0;
                }
            }
            else
            {
                return mp_obj_new_int(-1);
            }
        }
        else
        {
            return mp_obj_new_int(-1);
        }
    }
    else
    {
        memset(g_repl_protect_pswd, 0, sizeof(g_repl_protect_pswd));
        if (bufinfo[0].len > 0)
        {
            repl_protect_password_enable = 1;
            strncpy(g_repl_protect_pswd, bufinfo[0].buf, bufinfo[0].len);
        }
        else
        {
            repl_protect_password_enable = 0;
        }
    }

#endif    
	return mp_obj_new_int(0);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(qpy_misc_set_repl_password_obj, 1, 2, qpy_misc_set_repl_password);

#if !defined(PLAT_ASR_1803s)
#if defined(PLAT_ASR)
static mp_obj_t qpy_misc_IncCoreVoltage(void)
{
    if(0 != Helios_Dev_IncreaseCoreVoltage()) {
        mp_obj_new_int(-1);
    }
	return mp_obj_new_int(0);
}
static MP_DEFINE_CONST_FUN_OBJ_0(qpy_misc_IncCoreVoltage_obj, qpy_misc_IncCoreVoltage);
#endif

#endif

#if MICROPY_QPY_MISC_CFG_NETLIGHT  //20220620  add net light nv  @jimmy
static mp_obj_t net_light_enable(size_t n_args,const mp_obj_t *args) 
{   
    int ret = 0;  
    if(n_args == 1)
    {       
        int value = mp_obj_get_int(args[0]);
        if((value == 0) || (value == 1))
        {
    	    ret = Helios_netlight_enable(value);
        }
        else
        {
            ret = -1;
        }
    }
    else
    {
        ret = Helios_netlight_state_get();
    } 
    return mp_obj_new_int(ret);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(net_light_enable_obj, 0, 1, net_light_enable);
#endif

static const mp_rom_map_elem_t misc_module_globals_table[] = {

    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_misc) },
#if MICROPY_QPY_MISC_POWER
    { MP_ROM_QSTR(MP_QSTR_Power), MP_ROM_PTR(&misc_power_type) },
#endif
#if MICROPY_QPY_MISC_ADC
    { MP_ROM_QSTR(MP_QSTR_ADC), MP_ROM_PTR(&misc_adc_type) },
#endif
#if MICROPY_QPY_MISC_POWERKEY
    { MP_ROM_QSTR(MP_QSTR_PowerKey), MP_ROM_PTR(&misc_powerkey_type) },
#endif
#if MICROPY_QPY_MISC_TEMPERATURE
    { MP_ROM_QSTR(MP_QSTR_Temperature), MP_ROM_PTR(&machine_temperature_type) },
#endif
#if MICROPY_QPY_MISC_PWM
    { MP_ROM_QSTR(MP_QSTR_PWM), MP_ROM_PTR(&misc_pwm_type) },
    { MP_ROM_QSTR(MP_QSTR_PWM_V2), MP_ROM_PTR(&misc_pwm_v2_type) },
#endif
#if MICROPY_QPY_MISC_USB
    { MP_ROM_QSTR(MP_QSTR_USB), MP_ROM_PTR(&misc_usb_type) },
#endif
#if MICROPY_QPY_MISC_USBNET
    { MP_ROM_QSTR(MP_QSTR_USBNET), MP_ROM_PTR(&misc_usbnet_module) },
#endif
    { MP_ROM_QSTR(MP_QSTR_replEnable), MP_ROM_PTR(&qpy_misc_set_replEnable_obj) },
    { MP_ROM_QSTR(MP_QSTR_replUpdatePassswd), MP_ROM_PTR(&qpy_misc_set_repl_password_obj) },
#if defined(PLAT_ASR)
    { MP_ROM_QSTR(MP_QSTR_IncCoreVoltage), MP_ROM_PTR(&qpy_misc_IncCoreVoltage_obj)},
#endif
#if MICROPY_QPY_MISC_CFG_NETLIGHT
    { MP_ROM_QSTR(MP_QSTR_net_light), MP_ROM_PTR(&net_light_enable_obj) }, 
#endif        
};
static MP_DEFINE_CONST_DICT(misc_module_globals, misc_module_globals_table);

const mp_obj_module_t mp_module_misc = {
	.base = { &mp_type_module },
	.globals = (mp_obj_dict_t *)&misc_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_misc, mp_module_misc);
#endif /* MICROPY_QPY_MODULE_MISC */

