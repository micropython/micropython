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

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "py/builtin.h"
#include "py/compile.h"
#include "py/runtime.h"
#include "py/repl.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "py/objmodule.h"
#include "py/stackctrl.h"
#include "py/mphal.h"
#include "shared/runtime/pyexec.h"
#include "shared/readline/readline.h"
#include "shared/runtime/gchelper.h"
#include <stdlib.h>
#include "helios.h"
#include "helios_os.h"
#include "helios_debug.h"
#include "mphalport.h"
#include "callbackdeal.h"
#if MICROPY_KBD_EXCEPTION
#include "shared/runtime/interrupt_char.h"
#endif

#if CONFIG_MBEDTLS
#include "mbedtls_init.h"
#endif


Helios_Thread_t ql_micropython_task_ref;


#define MP_TASK_STACK_SIZE      (MP_QPY_TASK_STACK_SIZE)
#define MP_TASK_STACK_LEN       (MP_TASK_STACK_SIZE/sizeof(uint32_t))

#if (defined(PLAT_ASR) || defined(PLAT_Unisoc) || defined(PLAT_ASR_1803s) || defined(PLAT_ASR_1803sc) || defined(PLAT_Qualcomm) \
    || defined(PLAT_ASR_1606) || defined(PLAT_ASR_1609) || defined(PLAT_Unisoc_8910_R05) || defined(PLAT_Unisoc_8910_R06) || defined(PLAT_ASR_1602)) //support
#define QPY_ASSERT_SUPPORT 1
#endif

void nlr_jump_fail(void *val) {
#if QPY_ASSERT_SUPPORT
    Helios_Assert(__func__, __FILE__, __LINE__, "");
#endif
    while(1);
}

void NORETURN __fatal_error(const char *msg) {
#if QPY_ASSERT_SUPPORT
    Helios_Assert(msg, __FILE__, __LINE__, "");
#endif
    while(1);
}

#ifndef NDEBUG
#if !defined(PLAT_Qualcomm)
void MP_WEAK __assert_func(const char *file, int line, const char *func, const char *expr) {
#if QPY_ASSERT_SUPPORT
    Helios_Assert(expr, file, line, "");
#else
    printf("Assertion '%s' failed, at file %s:%d\n", expr, file, line);
#endif
    __fatal_error("Assertion failed");
}

#else
void __assert_fail(const char *__message,
                                            const char *__file, int __line,
                                            const char *__function) {
#if QPY_ASSERT_SUPPORT
   Helios_Assert(__message, __file, __line, "");
#else
   printf("Assertion '%s' failed, at file %s:%d\n", __message, __file, __line);
#endif
   __fatal_error("Assertion failed");
}
#endif

#endif

static char *stack_top;
#if MICROPY_ENABLE_GC
#if defined(PLAT_ECR6600)
static char __attribute__((__section__(".data"))) heap[MICROPY_GC_HEAP_SIZE];
#elif defined(PLAT_SONY_ALT1350)
static char __attribute__((__section__("gpm1_working_data"))) heap[MICROPY_GC_HEAP_SIZE];
#else
static char heap[MICROPY_GC_HEAP_SIZE];
#endif
#endif

extern pyexec_mode_kind_t pyexec_mode_kind;

#if MICROPY_PY_KBD_EXCEPTION
MAINPY_RUNNING_FLAG_DEF
MAINPY_INTERRUPT_BY_KBD_FLAG_DEF
SET_MAINPY_RUNNING_TIMER_DEF
#endif

#if MICROPY_PY_SOFT_RESET
static int vm_softreset_flag = 0;
#define SOFTRESET_FLAG_SET() vm_softreset_flag = 1;
#define SOFTRESET_FLAG_CLEAR() vm_softreset_flag = 0;
#define SOFTRESET_FLAG_TRUE() (1 == vm_softreset_flag)
#define SOFTRESET_FLAG_FALSE() (0 == vm_softreset_flag)
#endif
void quecpython_task(void *arg)
{
	int stack_dummy;
    Helios_Thread_t id = 0;
	void *stack_ptr = NULL;
    #if MICROPY_QPY_MODULE_POC && CONFIG_POC_BND_XIN
    helios_debug( "start qpy_poc_register_task"); 
    extern void qpy_poc_register_task();
    qpy_poc_register_task();
    #endif
    mp_mthread_sleep_deal_init();

#if CONFIG_MBEDTLS
    mbedtls_platform_setup(NULL);
#endif

	#if MICROPY_PY_THREAD
    id = Helios_Thread_GetID();
    ql_micropython_task_ref = id;
    stack_ptr = Helios_Thread_GetStaskPtr(id);
#if defined(PLAT_ECR6600) || defined(PLAT_aic8800m40)
    mp_thread_init(stack_ptr, MP_TASK_STACK_SIZE);   // unit: Word
#else
    mp_thread_init(stack_ptr, MP_TASK_STACK_LEN);
#endif
	#endif

	if(mp_hal_stdio_init()) return;

soft_reset:
	mp_stack_set_top((void *)&stack_dummy);
#if defined(PLAT_ECR6600) || defined(PLAT_aic8800m40)  // unit: Byte
    mp_stack_set_limit(MP_TASK_STACK_SIZE * sizeof(uint32_t) - 1024);
#else
    mp_stack_set_limit(MP_TASK_STACK_SIZE - 1024);
#endif
    stack_top = (char*)&stack_dummy;
    #if MICROPY_ENABLE_GC
    gc_init(heap, heap + sizeof(heap));
    #endif
    mp_init();
    #if MICROPY_ENABLE_CALLBACK_DEAL
	qpy_callback_deal_init();
    #endif
    readline_init0();
    
	// run boot-up scripts
#if defined(PLAT_RDA)
    pyexec_frozen_module("_boot_RDA.py");
#elif defined(PLAT_Qualcomm)
	pyexec_frozen_module("_boot_Qualcomm.py");
#elif defined(BOARD_EC800ECN_LC_WDF)
    pyexec_frozen_module("_boot_WDF.py");//EIGEN WDF CUNSTOMER BOOT WITH SINGEL FILE SYSTEM
#elif defined(PLAT_ECR6600) || defined(PLAT_aic8800m40)
    pyexec_frozen_module("_boot_WIFI.py");
#elif defined(BOARD_EC600GCN_LA_CDD)
    pyexec_frozen_module("_boot_dsds.py");
#elif defined(PLAT_SONY_ALT1350)
    pyexec_frozen_module("_boot_SONY.py", false);
#else
    pyexec_frozen_module("_boot.py", false);
#endif

    if (pyexec_mode_kind == PYEXEC_MODE_FRIENDLY_REPL
    #if MICROPY_PY_KBD_EXCEPTION
    && MAINPY_INTERRUPT_BY_KBD_FLAG_FALSE()
    #endif
    ) 
    {
        #if MICROPY_PY_SOFT_RESET
        SOFTRESET_FLAG_CLEAR();
        #endif
        
        #if MICROPY_PY_KBD_EXCEPTION
        MAINPY_RUNNING_FLAG_SET();
        #endif
        
        int ret = pyexec_file_if_exists("/usr/main.py");
        if (ret & PYEXEC_FORCED_EXIT) {
            goto soft_reset_exit;
        }
        
        #if MICROPY_PY_KBD_EXCEPTION
        MAINPY_RUNNING_FLAG_CLEAR();
        if(RET_KBD_INTERRUPT == ret)
        {
            MAINPY_INTERRUPT_BY_KBD_FLAG_SET();
        }
        #endif
        
        #if MICROPY_PY_SOFT_RESET
        if((PYEXEC_SOFTRESET & ret) == PYEXEC_SOFTRESET) {
            SOFTRESET_FLAG_SET();
        }
        #endif
    }
    else
    {
        #if MICROPY_PY_KBD_EXCEPTION
        MAINPY_INTERRUPT_BY_KBD_FLAG_CLEAR();
        #endif
        #if MICROPY_PY_SOFT_RESET
        SOFTRESET_FLAG_CLEAR();
        #endif
    }

    if(1
        #if MICROPY_PY_KBD_EXCEPTION
        && MAINPY_INTERRUPT_BY_KBD_FLAG_FALSE()
        #endif
        #if MICROPY_PY_SOFT_RESET
        && SOFTRESET_FLAG_FALSE()
        #endif
    )
    {
        #if MICROPY_PY_SOFT_RESET
        nlr_buf_t nlr;
        nlr.ret_val = NULL;
        if (nlr_push(&nlr) == 0) {
        #endif
        	for (;;) {
                if (pyexec_mode_kind == PYEXEC_MODE_RAW_REPL) {
                    if (pyexec_raw_repl() != 0) {
                        break;
                    }
                } else {
                    if (pyexec_friendly_repl() != 0) {
                        break;
                    }
                }
            }
        #if MICROPY_PY_SOFT_RESET
            nlr_pop();
        } else {
            mp_obj_print_exception(&mp_plat_print, MP_OBJ_FROM_PTR(nlr.ret_val));
        }
        #endif
    }

soft_reset_exit:

	#if MICROPY_PY_THREAD
    mp_thread_deinit();
    #endif

	gc_sweep_all();

    mp_hal_stdout_tx_str("MPY: soft reboot\r\n");
    #if MICROPY_ENABLE_CALLBACK_DEAL
    qpy_callback_deal_deinit();
    qpy_callback_para_link_free_all();
    #endif
	mp_deinit();
#if !defined(PLAT_RDA)
    fflush(stdout);
#endif
    goto soft_reset;
}

#if !MICROPY_VFS
mp_lexer_t *mp_lexer_new_from_file(qstr filename) {
    mp_raise_OSError(MP_ENOENT);
}

mp_import_stat_t mp_import_stat(const char *path) {
    (void)path;
    return MP_IMPORT_STAT_NO_EXIST;
}

mp_obj_t mp_builtin_open(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(mp_builtin_open_obj, 1, mp_builtin_open);

#endif

application_init(quecpython_task, "quecpython_task", (MP_TASK_STACK_SIZE)/1024, 0);
