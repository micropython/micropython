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

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "py/compile.h"
#include "py/runtime.h"
#include "py/repl.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "py/stackctrl.h"
#include "lib/utils/pyexec.h"

#include "lib/oofatfs/ff.h"
#include "extmod/vfs.h"
#include "extmod/vfs_fat.h"


#if MICROPY_MIN_USE_IMXRT1064_MCU
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"
#include "app.h"
#include "system_MIMXRT1064.h"
#include "gccollect.h"
#include "littlevgl_support.h"
#endif

#include "led.h"
#include "pin.h"
#include "sdcard.h"

//static const char fresh_main_py[] = "# main.py -- put your code here!\r\n";

#if MICROPY_ENABLE_COMPILER
void do_str(const char *src, mp_parse_input_kind_t input_kind) {
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_lexer_t *lex = mp_lexer_new_from_str_len(MP_QSTR__lt_stdin_gt_, src, strlen(src), 0);
        qstr source_name = lex->source_name;
        mp_parse_tree_t parse_tree = mp_parse(lex, input_kind);
        mp_obj_t module_fun = mp_compile(&parse_tree, source_name, MP_EMIT_OPT_NONE, true);
        mp_call_function_0(module_fun);
        nlr_pop();
    } else {
        // uncaught exception
        mp_obj_print_exception(&mp_plat_print, (mp_obj_t)nlr.ret_val);
    }
}
#endif

STATIC bool init_sdcard_fs(void)
{
    bool first_part = true;
    fs_user_mount_t *vfs_fat;
    mp_vfs_mount_t *vfs;
    for (int part_num = 1; part_num <= 4; ++part_num)
    {
        vfs_fat = m_new_obj_maybe(fs_user_mount_t);
        vfs = m_new_obj_maybe(mp_vfs_mount_t);
        if(vfs == NULL || vfs_fat == NULL)
        {
            break;
        }
        vfs_fat->flags = FSUSER_FREE_OBJ;
        sdcard_init_vfs(vfs_fat, part_num);
        /* mount the partition */
        FRESULT res = f_mount(&(vfs_fat->fatfs));

        if(res != FR_OK)
        {
            m_del_obj(fs_user_mount_t, vfs_fat);
            m_del_obj(mp_vfs_mount_t, vfs);
        }
        else
        {
            if(first_part)
            {
                vfs->str = "/sd";
                vfs->len = 3;
            }
            else
            {
                if(part_num == 2)
                {
                    vfs->str = "/sd2";
                }
                else if (part_num == 3)
                {
                    vfs->str = "/sd3";
                }
                else 
                    vfs->str = "/sd4";
                vfs->len = 4;
            }
            vfs->obj = MP_OBJ_FROM_PTR(vfs_fat);
            vfs->next = NULL;
            for(mp_vfs_mount_t **m = &MP_STATE_VM(vfs_mount_table);;m= &(*m)->next)
            {
                if(*m == NULL)
                {
                    *m = vfs;
                    break;
                }
            }
            if(first_part)
            {
                MP_STATE_PORT(vfs_cur) = vfs;
            }
            first_part = false;
        }
    }
    if(first_part)
    {
        printf("MPY: can't mount SD card\r\n");
        return false;
    }
    else
    {
        return true;
    }
}

int main(void) {    
    // BOARD init
    BOARD_InitHardware();

    pin_init0();
    led_init();
    lv_port_pre_init();
    //Set Stack top
    mp_stack_set_top(&__StackTop);
    //Set Stack limit 
    mp_stack_set_limit((char*)&__StackTop - (char*)&__HeapLimit -256);

    #if MICROPY_ENABLE_GC
    gc_init((void *)&__HeapBase, (void *)&__HeapLimit);
    #endif

    #if MICROPY_ENABLE_PYSTACK
    static mp_obj_t pystack[384];
    mp_pystack_init(pystack, &pystack[384]);
    #endif 
    // MicroPython init
    mp_init();
    mp_obj_list_init(MP_OBJ_TO_PTR(mp_sys_path), 0);
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR_));
    mp_obj_list_init(MP_OBJ_TO_PTR(mp_sys_argv), 0);

    /* mount the sdcard */
    bool mounted_sdcard = false;
    if(sdcard_WaitCardInsert() == kStatus_Success)
    {
        mounted_sdcard = init_sdcard_fs();
    }
    if (mounted_sdcard)
    {
        mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_sd));
        mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_sd_slash_lib));
    }
    
    /* reset config variables; they should be set by boot.py */
    MP_STATE_PORT(pyb_config_main) = MP_OBJ_NULL;
    
    mp_import_stat_t stat = mp_import_stat("boot.py");
    if(stat == MP_IMPORT_STAT_FILE)
    {
        vstr_t *vstr = vstr_new(16);
        vstr_add_str(vstr, "boot.py");
        if(!pyexec_file(vstr_null_terminated_str(vstr)))
            printf("boot.py exec failed\r\n");
    }

    /* Run the main script from the current directory. */
    const char *main_py;
    main_py = "main.py";
    stat = mp_import_stat(main_py);
    if(stat == MP_IMPORT_STAT_FILE)
    {
        int main_ret = pyexec_file(main_py);
        if(main_ret & PYEXEC_FORCED_EXIT)
        {
            goto main_exit;
        }
        if(!main_ret)
        {
            printf("main.py exec error\r\n");
        }
    }
    #if MICROPY_ENABLE_COMPILER
    #if MICROPY_REPL_EVENT_DRIVEN
    pyexec_event_repl_init();
    for (;;) {
        int c = mp_hal_stdin_rx_chr();
        if (pyexec_event_repl_process_char(c)) {
            break;
        }
    }
    #else
    for(;;)
    {
        if(pyexec_friendly_repl() !=0)
            break;
    }
    #endif
    //do_str("print('hello world!', list(x+1 for x in range(10)), end='eol\\n')", MP_PARSE_SINGLE_INPUT);
    //do_str("for i in range(10):\r\n  print(i)", MP_PARSE_FILE_INPUT);
    #else
    pyexec_frozen_module("frozentest.py");
    #endif
main_exit:
    mp_deinit();
    return 0;
}

mp_obj_t mp_builtin_open(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(mp_builtin_open_obj, 1, mp_builtin_open);

void NORETURN __fatal_error(const char *msg) {
    PRINTF("FATAL ERROR HAPPENED\r\n");
    while (1);
}

#ifndef NDEBUG
void MP_WEAK __assert_func(const char *file, int line, const char *func, const char *expr) {
    printf("Assertion '%s' failed, at file %s:%d\n", expr, file, line);
    __fatal_error("Assertion failed");
}
#endif

void nlr_jump_fail(void *val) {
    PRINTF("FATAL: uncaught exception %p\n", val);
    __fatal_error("");
    while (1);
}
