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

#include "fsl_debug_console.h"
#include "app.h"
#include "gccollect.h"

#if defined(MICROPY_PY_LVGL) && MICROPY_PY_LVGL
#include "littlevgl_support.h"
#endif 

#include "led.h"
#include "pin.h"
#include "sdcard.h"

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

void REPLTask(void *param);
void CardDetectTask(void *param);
void SdcardInitTask(void *param);

static void SDCARD_DetectCallBack(bool isInserted, void *userData);

static volatile bool s_cardInserted     = false;
static volatile bool s_cardInsertStatus = false;

static TaskHandle_t CardDetectTaskHandle;
static TaskHandle_t SdcardInitTaskHandle;
static SemaphoreHandle_t s_fileAccessSemaphore = NULL;
static SemaphoreHandle_t s_CardDetectSemaphore = NULL;
extern sd_card_t g_sd;

static const sdmmchost_detect_card_t s_sdCardDetect = {
#ifndef BOARD_SD_DETECT_TYPE
    .cdType = kSDMMCHOST_DetectCardByGpioCD,
#else
    .cdType = BOARD_SD_DETECT_TYPE,
#endif
    .cdTimeOut_ms = (~0U),
    .cardInserted = SDCARD_DetectCallBack,
    .cardRemoved  = SDCARD_DetectCallBack,
};

static void mp_reset(void)
{
    //Set Stack top
    mp_stack_set_top(&__StackTop);
    //Set Stack limit 
    mp_stack_set_limit(STACK_SIZE);

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
}

static void SDCARD_DetectCallBack(bool isInserted, void *userData)
{
    s_cardInsertStatus = isInserted;
    xSemaphoreGiveFromISR(s_CardDetectSemaphore, NULL);
}

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

        /* set vfs_fat */
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


void CardDetectTask(void *param)
{
    bool mounted_sdcard = false;
    s_fileAccessSemaphore = xSemaphoreCreateBinary();
    s_CardDetectSemaphore = xSemaphoreCreateBinary();

    g_sd.host.base           = SD_HOST_BASEADDR;
    g_sd.host.sourceClock_Hz = SD_HOST_CLK_FREQ;
    g_sd.usrParam.cd         = &s_sdCardDetect;
     /* SD host init function */
    if (SD_HostInit(&g_sd) == kStatus_Success)
    {
        while (true)
        {
            /* take card detect semaphore */
            if (xSemaphoreTake(s_CardDetectSemaphore, portMAX_DELAY) == pdTRUE)
            {
                if (s_cardInserted != s_cardInsertStatus)
                {
                    s_cardInserted = s_cardInsertStatus;
                    if (!s_cardInserted)
                    {
                        /* power off card */
                        SD_PowerOffCard(g_sd.host.base, g_sd.usrParam.pwr);
                    }

                    if (s_cardInserted)
                    {
                        /* power on the card */
                        SD_PowerOnCard(g_sd.host.base, g_sd.usrParam.pwr);
                        mounted_sdcard = init_sdcard_fs();
                        if(mounted_sdcard)
                        {
                            mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_sd));
                            mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_sd_slash_lib));
                        }
                        /* make file system */
                        xSemaphoreGive(s_fileAccessSemaphore);
                    }
                }

                if (!s_cardInserted)
                {
                    printf("\r\nPlease insert a card into board.\r\n");
                }
            }
        }
    }
    else
    {
        printf("\r\nSD host init fail\r\n");
    }
    vTaskSuspend(NULL);
}

void SdcardInitTask(void *param)
{
    while(1)
    {
        /* mount the sdcard */
        if(xSemaphoreTake(s_fileAccessSemaphore, portMAX_DELAY) == pdTRUE)
        {
        
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
                if(!main_ret)
                {
                    printf("main.py exec error\r\n");
                }
            }
            BaseType_t task_status;
            task_status = xTaskCreate(REPLTask, "REPL", configMINIMAL_STACK_SIZE+400, NULL, 4, NULL);
            if(pdPASS != task_status)
            {
                printf("Failed to creat REPL task\r\n");
            }
        }
    }
}

int main(void) { 
    // BOARD init
    BOARD_InitHardware();

    pin_init0();
    led_init();
#if defined(MICROPY_PY_LVGL) && MICROPY_PY_LVGL
    lv_port_pre_init();
#endif
    /* initialize */
    mp_reset();

    BaseType_t status;
    status = xTaskCreate(CardDetectTask, "CardDetect", configMINIMAL_STACK_SIZE+400, NULL, 3, &CardDetectTaskHandle);
    if(pdPASS != status)
    {
        printf("Failed to creat CardDetect task\r\n");
    }
    status = xTaskCreate(SdcardInitTask, "SdcardInit", configMINIMAL_STACK_SIZE+400, NULL, 2, &SdcardInitTaskHandle);
    if(pdPASS != status)
    {
        printf("Failed to creat SdcardInit task\r\n");
    }
    vTaskStartScheduler();
    mp_deinit();
    return 0;
}

void REPLTask(void *param)
{
    /* Delete useless Task */
    vTaskDelete(CardDetectTaskHandle);
    vTaskDelete(SdcardInitTaskHandle);

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
soft_reset:
    for(;;)
    {
        if(pyexec_mode_kind == PYEXEC_MODE_FRIENDLY_REPL)
        {
            if(pyexec_friendly_repl() !=0)
                break;
        }
        else if (pyexec_mode_kind == PYEXEC_MODE_RAW_REPL)
        {
            if(pyexec_raw_repl() != 0)
                break;
        }
    }
    goto soft_reset_exit;
    #endif
    #else
    pyexec_frozen_module("frozentest.py");
    #endif

soft_reset_exit:
    printf("MPY: soft reboot\n");
    gc_sweep_all();
    mp_deinit();
    mp_reset();
    goto soft_reset;
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
    (void)func;
    printf("Assertion '%s' failed, at file %s:%d\n", expr, file, line);
    __fatal_error("Assertion failed");
}
#endif

void nlr_jump_fail(void *val) {
    PRINTF("FATAL: uncaught exception %p\n", val);
    mp_obj_print_exception(&mp_plat_print, MP_OBJ_FROM_PTR(val));
    __fatal_error("");
    while (1);
}

void vApplicationMallocFailedHook(void)
{
    for (;;)
    {
        printf("Malloc Failed \r\n");
        break;
    }
}

/*!
 * @brief Stack overflow hook.
 */
void vApplicationStackOverflowHook(TaskHandle_t xTask, signed char *pcTaskName)
{
    (void)pcTaskName;
    (void)xTask;

    for (;;)
    {
        printf("Stack OVerflow \r\n");
        break;
    }
}
