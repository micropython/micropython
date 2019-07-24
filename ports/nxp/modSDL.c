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

#include "py/runtime.h"
#include "py/mphal.h"
#include "py/obj.h"
#include "py/binary.h"

#include "lib/utils/pyexec.h"
#include "driver/include/common.h"

#include "littlevgl_support.h"

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "task.h"

#if LV_HOR_RES != 480
#error "LV_HOR_RES must be set to 480!"
#endif

#if LV_VER_RES != 272
#error "LV_VER_RES must be set to 272!"
#endif

#if LV_COLOR_DEPTH != 16
#error "LV_COLOR_DEPTH must be set to 16!"
#endif

TimerHandle_t xTimer;

void InitLcd(void);
void FlushDisplay(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t *color_p);
void DispTask(void *param);


void vTimerCallback(TimerHandle_t pxTimer)
{
    lv_tick_inc(2);
    lv_task_handler();
}

void InitLcd(void)
{
    DEMO_InitLcd();
}

STATIC mp_obj_t mp_init_SDL(void)
{
    BaseType_t stat;
    stat = xTaskCreate(DispTask, "Display", configMINIMAL_STACK_SIZE+400, NULL, 4, NULL);
    if (pdPASS != stat)
    {
        printf("Failed to create display task\r\n");
    }
    vTaskDelay(5);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mp_init_SDL_obj, mp_init_SDL);

void DispTask(void *param)
{
    InitLcd();
    DEMO_InitTouch();
    xTimer = xTimerCreate("lvgl_timer",5,pdTRUE,NULL,vTimerCallback);
    if(xTimer == NULL || xTimerStart(xTimer,0) != pdPASS)
    {
        printf("Failed creating or starting LVGL timer!\r\n");
    }
    vTaskSuspend(NULL);
}

/* 
 * Flush a buffer to the display.
 */
void display_flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t *color_p)
{
    DEMO_FlushDisplay(x1, y1, x2, y2, color_p);
}
DEFINE_PTR_OBJ(display_flush);

void display_fill(int32_t x1, int32_t y1, int32_t x2, int32_t y2, lv_color_t *color_p)
{
    printf("fill display \r\n");
}
DEFINE_PTR_OBJ(display_fill);

void display_map(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t *color_p)
{
    printf("map display\r\n");
}
DEFINE_PTR_OBJ(display_map);


void touch_read(lv_indev_data_t * data)
{
    DEMO_ReadTouch(data);
}
DEFINE_PTR_OBJ(touch_read);

STATIC const mp_rom_map_elem_t SDL_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_SDL) },
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&mp_init_SDL_obj) },
    { MP_ROM_QSTR(MP_QSTR_display_flush), MP_ROM_PTR(&PTR_OBJ(display_flush)) },
    { MP_ROM_QSTR(MP_QSTR_display_fill), MP_ROM_PTR(&PTR_OBJ(display_fill)) },
    { MP_ROM_QSTR(MP_QSTR_display_map), MP_ROM_PTR(&PTR_OBJ(display_map)) },
    { MP_ROM_QSTR(MP_QSTR_touch_read), MP_ROM_PTR(&PTR_OBJ(touch_read)) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_SDL_globals, SDL_globals_table);


const mp_obj_module_t mp_module_SDL = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_SDL_globals,
};