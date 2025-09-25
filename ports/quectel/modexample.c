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
#include "py/compile.h"
#include "py/runtime0.h"
#include "py/nlr.h"
#include "py/objlist.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/stream.h"
#include "shared/runtime/pyexec.h"
#include "shared/runtime/interrupt_char.h"


#if MICROPY_QPY_MODULE_EXAMPLE

static mp_obj_t example_exec(const mp_obj_t arg0)
{
	int ret = 0;
	
	mp_buffer_info_t bufinfo;
	char fname[128] = {0};
	char path[128] = {0};
	mp_get_buffer_raise(arg0, &bufinfo, MP_BUFFER_READ);

	memcpy(path, bufinfo.buf, bufinfo.len);
	
	if(bufinfo.buf != NULL)
	{
		// Pawn 2021-01-18 for JIRA STASR3601-2428 begin
		if (path[0] != '/')
		{
			snprintf(fname, sizeof(fname), "/%s", (char *)bufinfo.buf);
		}
		else
		{
			snprintf(fname, sizeof(fname), "%s", (char *)bufinfo.buf);
		}
		
		ret = pyexec_file_if_exists(fname);
	}
	if ( ret == -1 )
	{
		mp_raise_msg_varg(&mp_type_OSError, MP_ERROR_TEXT("File path error or not exist: [%s]"), (char *)bufinfo.buf);
	}
	#if MICROPY_PY_KBD_EXCEPTION
	else if(ret == RET_KBD_INTERRUPT)
	{
        pyexec_system_exit = PYEXEC_FORCED_EXIT;
        MAINPY_INTERRUPT_BY_KBD_FLAG_SET();
        mp_raise_msg_varg(&mp_type_SystemExit, MP_ERROR_TEXT("CTRL_C Interrupt"));
	}
	#endif
	#if MICROPY_PY_SOFT_RESET
	else if((ret & PYEXEC_SOFTRESET) == PYEXEC_SOFTRESET)
	{
        pyexec_system_exit = PYEXEC_FORCED_EXIT;
        mp_raise_msg_varg(&mp_type_SystemExit, MP_ERROR_TEXT("SoftReset"));
	}
	#endif
	// Pawn 2021-01-18 for JIRA STASR3601-2428 end
	return mp_const_none;
}



static MP_DEFINE_CONST_FUN_OBJ_1(example_exec_obj, example_exec);


static mp_obj_t example_initialize()
{
    static int initialized = 0;
    if (!initialized) {
        initialized = 1;
    }
    return mp_const_none;
}

static MP_DEFINE_CONST_FUN_OBJ_0(example_initialize_obj, example_initialize);

static const mp_rom_map_elem_t mp_module_example_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_example) },
    { MP_ROM_QSTR(MP_QSTR___init__), MP_ROM_PTR(&example_initialize_obj) },
    { MP_ROM_QSTR(MP_QSTR_exec), MP_ROM_PTR(&example_exec_obj) },

};


static MP_DEFINE_CONST_DICT(mp_module_example_globals, mp_module_example_globals_table);


const mp_obj_module_t example_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_example_globals,
};

MP_REGISTER_MODULE(MP_QSTR_example, example_module);
#endif /* MICROPY_QPY_MODULE_EXAMPLE */
