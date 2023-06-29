/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023-2024 bilibili:fvstory
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

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_rgb.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"

#include "py/runtime.h"
#include "py/mphal.h"
#include "py/mperrno.h"
#include "py/obj.h"
#include "py/objstr.h"
#include "py/objarray.h"
#include "py/stream.h"
#include "py/binary.h"

#include "modesp32s3.h"
#include "mpconfigport.h"
#include "mphalport.h"

#define DEBUG_printf(...)  mp_printf(&mp_plat_print, __VA_ARGS__);

// The I8080 lcd structure
typedef struct mp_esp32s3_i80lcd_obj_t {
    mp_obj_base_t base;

    uint16_t width;
    uint16_t height;
    mp_obj_t rd;
    mp_obj_t flushdown_callback;
    esp_lcd_i80_bus_handle_t i80_bus;
    esp_lcd_i80_bus_config_t bus_config;
    esp_lcd_panel_io_handle_t io_handle;
    esp_lcd_panel_io_i80_config_t io_config;

} mp_esp32s3_i80lcd_obj_t;

const mp_obj_type_t mp_esp32s3_i80lcd_type;
void esp32s3_hw_i80lcd_deinit(mp_esp32s3_i80lcd_obj_t *i80lcd);
// *********************************************************************************

// The RGB lcd structure
// The RGB just flush the color once. If we want to use the psram as gram, must to
// modify the main.c and the mpconfigboard to make the espidf use the psram with 4MB
// space remaining for gram's usage. And call the special heap malloc methead for MP.
typedef struct mp_esp32s3_rgblcd_obj_t {
    mp_obj_base_t base;

    mp_obj_t flushdown_callback;
    mp_obj_t bk;
    #ifdef MICROPY_ESP_ENABLE_PSRAM_OCT
    void *buf1;
    void *buf2;
    void *buf3;
    #endif
    esp_lcd_panel_handle_t panel_handle;
    esp_lcd_rgb_panel_config_t panel_config;

} mp_esp32s3_rgblcd_obj_t;

const mp_obj_type_t mp_esp32s3_rgblcd_type;
void esp32s3_hw_rgblcd_deinit(mp_esp32s3_rgblcd_obj_t *rgblcd);
// *********************************************************************************

// TODO The CAM structure
typedef struct mp_esp32s3_cam_obj_t {
    mp_obj_base_t base;
    // TODO

} mp_esp32s3_cam_obj_t;

const mp_obj_type_t mp_esp32s3_cam_type;
void esp32s3_hw_cam_deinit(mp_esp32s3_cam_obj_t *cam);
// *********************************************************************************

// The data structure for the esp32s3_lcd_cam_singleton.**************************
// esp32s3 has the only one lcd_cam to use
typedef struct _esp32s3_lcd_cam_obj_t {
    mp_obj_base_t base;

    mp_esp32s3_i80lcd_obj_t *i80lcd_obj;
    mp_esp32s3_rgblcd_obj_t *rgblcd_obj;
    // TODO CAM
    mp_esp32s3_cam_obj_t *cam_obj;
    enum {
        MP_HW_LCD_CAM_STATE_NONE,
        MP_HW_LCD_CAM_STATE_I80INIT,
        MP_HW_LCD_CAM_STATE_RGBINIT,
        MP_HW_LCD_CAM_STATE_CAMINIT,
    } state;
} esp32s3_lcd_cam_obj_t;

const mp_obj_type_t esp32s3_lcd_cam_type;
// *********************************************************************************

static esp32s3_lcd_cam_obj_t *_get_lcd_cam_singleton() {
    return MP_STATE_PORT(esp32s3_lcd_cam_singleton);
}

// The LCD_CAM deinit
void mp_esp32s3_lcd_cam_global_deinit(void) {
    // Disable, deallocate
    esp32s3_lcd_cam_obj_t *lcd_cam = _get_lcd_cam_singleton();
    if (lcd_cam->state == MP_HW_LCD_CAM_STATE_I80INIT) {
        mp_esp32s3_i80lcd_obj_t *i80lcd = lcd_cam->i80lcd_obj;
        esp32s3_hw_i80lcd_deinit(i80lcd);
    } else if (lcd_cam->state == MP_HW_LCD_CAM_STATE_RGBINIT) {
        mp_esp32s3_rgblcd_obj_t *rgblcd = lcd_cam->rgblcd_obj;
        esp32s3_hw_rgblcd_deinit(rgblcd);
    } else if (lcd_cam->state == MP_HW_LCD_CAM_STATE_CAMINIT) {
        mp_esp32s3_cam_obj_t *cam = lcd_cam->cam_obj;
        esp32s3_hw_cam_deinit(cam);
    }
    lcd_cam->state = MP_HW_LCD_CAM_STATE_NONE;
}
// ***************************************************************************************************

// I80 LCD
// The i80lcd call_back
STATIC bool mp_esp32s3_i80lcd_flushdown_callback(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata, void *user_ctx) {
    mp_esp32s3_i80lcd_obj_t *self = (mp_esp32s3_i80lcd_obj_t *)user_ctx;

    if (self->flushdown_callback != mp_const_none) {
        mp_sched_schedule(self->flushdown_callback, self);
    }
    return false;
}

STATIC void mp_esp32s3_i80lcd_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    mp_esp32s3_i80lcd_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "<I8080 data=%p, dc=%p, write=%p, read=%p, cs=%p, width=%u, height=%u, cmd_bits=%u, param_bits=%u, queue_size=%u>",
        self->bus_config.data_gpio_nums,
        self->bus_config.dc_gpio_num,
        self->bus_config.wr_gpio_num,
        self->rd,
        self->io_config.cs_gpio_num,
        self->width,
        self->height,
        self->io_config.lcd_cmd_bits,
        self->io_config.lcd_param_bits,
        self->io_config.trans_queue_depth);
}

STATIC mp_obj_t mp_esp32s3_i80lcd_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum {
        ARG_data,
        ARG_command,
        ARG_write,
        ARG_read,
        ARG_cs,
        ARG_pclk,
        ARG_width,
        ARG_height,
        ARG_swap_color_bytes,
        ARG_cmd_bits,
        ARG_param_bits,
        ARG_queue_size,
        ARG_flushdown_callback
    };
    const mp_arg_t make_new_args[] = {
        { MP_QSTR_data,              MP_ARG_OBJ | MP_ARG_KW_ONLY | MP_ARG_REQUIRED          },
        { MP_QSTR_command,           MP_ARG_OBJ | MP_ARG_KW_ONLY | MP_ARG_REQUIRED          },
        { MP_QSTR_write,             MP_ARG_OBJ | MP_ARG_KW_ONLY | MP_ARG_REQUIRED          },
        { MP_QSTR_read,              MP_ARG_OBJ | MP_ARG_KW_ONLY,  {.u_obj = MP_OBJ_NULL  } },
        { MP_QSTR_cs,                MP_ARG_OBJ | MP_ARG_KW_ONLY,  {.u_obj = MP_OBJ_NULL  } },
        { MP_QSTR_pclk,              MP_ARG_INT | MP_ARG_KW_ONLY,  {.u_int = 10000000     } },
        { MP_QSTR_width,             MP_ARG_INT | MP_ARG_KW_ONLY,  {.u_int = 480          } },
        { MP_QSTR_height,            MP_ARG_INT | MP_ARG_KW_ONLY,  {.u_int = 320          } },
        { MP_QSTR_swap_color_bytes,  MP_ARG_BOOL | MP_ARG_KW_ONLY, {.u_bool = true        } },
        { MP_QSTR_cmd_bits,          MP_ARG_INT | MP_ARG_KW_ONLY,  {.u_int = 8            } },
        { MP_QSTR_param_bits,        MP_ARG_INT | MP_ARG_KW_ONLY,  {.u_int = 8            } },
        { MP_QSTR_queue_size,        MP_ARG_INT | MP_ARG_KW_ONLY,  {.u_int = 10           } },
        { MP_QSTR_flushdown_callback, MP_ARG_OBJ | MP_ARG_KW_ONLY,  {.u_obj = mp_const_none} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(make_new_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(make_new_args), make_new_args, args);
    // Detect the LCD_CAM singleton
    esp32s3_lcd_cam_obj_t *lcd_cam = MP_STATE_PORT(esp32s3_lcd_cam_singleton);
    if (lcd_cam != NULL) {
        mp_esp32s3_lcd_cam_global_deinit();
    } else {
        lcd_cam = m_new_obj(esp32s3_lcd_cam_obj_t);
        lcd_cam->base.type = &esp32s3_lcd_cam_type;
        // Set the global singleton pointer for the esp32s3 lcd_cam protocol.
        MP_STATE_PORT(esp32s3_lcd_cam_singleton) = lcd_cam;
    }
    // Then make new I8080
    mp_esp32s3_i80lcd_obj_t *self = m_new_obj(mp_esp32s3_i80lcd_obj_t);
    lcd_cam->i80lcd_obj = self;
    self->base.type = &mp_esp32s3_i80lcd_type;
    self->flushdown_callback = args[ARG_flushdown_callback].u_obj;
    self->width = args[ARG_width].u_int;
    self->height = args[ARG_height].u_int;

    // Then use the espidf esp_lcd to construct i80
    // bus_handle
    self->i80_bus = NULL;
    self->bus_config.clk_src = LCD_CLK_SRC_DEFAULT;
    self->bus_config.dc_gpio_num = mp_hal_get_pin_obj(args[ARG_command].u_obj);
    self->bus_config.wr_gpio_num = mp_hal_get_pin_obj(args[ARG_write].u_obj);
    // data bus
    mp_obj_tuple_t *t = MP_OBJ_TO_PTR(args[ARG_data].u_obj);
    for (size_t i = 0; i < t->len; i++)
    {
        self->bus_config.data_gpio_nums[i] = mp_hal_get_pin_obj(t->items[i]);
    }
    self->bus_config.bus_width = t->len;
    self->bus_config.max_transfer_bytes = self->width * self->height * sizeof(uint16_t);
    self->bus_config.psram_trans_align = 64;
    self->bus_config.sram_trans_align = 4;
    check_esp_err(esp_lcd_new_i80_bus(&self->bus_config, &self->i80_bus));
    // io_handle
    self->io_handle = NULL;
    if (args[ARG_cs].u_obj != MP_OBJ_NULL) {
        self->io_config.cs_gpio_num = mp_hal_get_pin_obj(args[ARG_cs].u_obj);
    } else {
        self->io_config.cs_gpio_num = -1;
    }
    self->io_config.pclk_hz = args[ARG_pclk].u_int;
    self->io_config.trans_queue_depth = args[ARG_queue_size].u_int;
    // dc_levels
    self->io_config.dc_levels.dc_idle_level = 0;
    self->io_config.dc_levels.dc_cmd_level = 0;
    self->io_config.dc_levels.dc_dummy_level = 0;
    self->io_config.dc_levels.dc_data_level = 1;
    self->io_config.flags.swap_color_bytes = args[ARG_swap_color_bytes].u_bool;
    // the LCD_CAM isr call back
    self->io_config.on_color_trans_done = mp_esp32s3_i80lcd_flushdown_callback;
    self->io_config.user_ctx = &self;
    self->io_config.lcd_cmd_bits = args[ARG_cmd_bits].u_int;
    self->io_config.lcd_param_bits = args[ARG_param_bits].u_int;
    check_esp_err(esp_lcd_new_panel_io_i80(self->i80_bus, &self->io_config, &self->io_handle));

    self->rd = args[ARG_read].u_obj;
    if (self->rd != MP_OBJ_NULL) {
        mp_hal_pin_obj_t rd_pin = mp_hal_get_pin_obj(self->rd);
        mp_hal_pin_output(rd_pin);
        mp_hal_pin_write(rd_pin, 1);
    }
    DEBUG_printf("i80_init_success\n");
    // record the LCD_CAM state
    lcd_cam->state = MP_HW_LCD_CAM_STATE_I80INIT;
    return MP_OBJ_FROM_PTR(self);
}

void esp32s3_hw_i80lcd_deinit(mp_esp32s3_i80lcd_obj_t *i80lcd) {
    check_esp_err(esp_lcd_panel_io_del(i80lcd->io_handle));
    check_esp_err(esp_lcd_del_i80_bus(i80lcd->i80_bus));
}

STATIC mp_obj_t mp_esp32s3_i80lcd_deinit(mp_obj_t _i80lcd) {
    esp32s3_lcd_cam_obj_t *lcd_cam = _get_lcd_cam_singleton();
    mp_esp32s3_i80lcd_obj_t *self = MP_OBJ_TO_PTR(_i80lcd);

    esp32s3_hw_i80lcd_deinit(self);
    m_del_obj(mp_esp32s3_i80lcd_obj_t, self);
    lcd_cam->state = MP_HW_LCD_CAM_STATE_NONE;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_esp32s3_i80lcd_deinit_obj, mp_esp32s3_i80lcd_deinit);

STATIC mp_obj_t mp_esp32s3_i80lcd_tx_param(size_t n_args, const mp_obj_t *args_in) {
    mp_esp32s3_i80lcd_obj_t *self = MP_OBJ_TO_PTR(args_in[0]);
    int cmd = mp_obj_get_int(args_in[1]);
    if (n_args == 3) {
        mp_buffer_info_t bufinfo;
        mp_get_buffer_raise(args_in[2], &bufinfo, MP_BUFFER_READ);
        const void *param = bufinfo.buf;
        check_esp_err(esp_lcd_panel_io_tx_param(self->io_handle, cmd, param, bufinfo.len));
    } else {
        check_esp_err(esp_lcd_panel_io_tx_param(self->io_handle, cmd, NULL, 0));
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_esp32s3_i80lcd_tx_param_obj, 2, 3, mp_esp32s3_i80lcd_tx_param);

STATIC mp_obj_t mp_esp32s3_i80lcd_tx_color(size_t n_args, const mp_obj_t *args_in) {
    mp_esp32s3_i80lcd_obj_t *self = MP_OBJ_TO_PTR(args_in[0]);

    int cmd = mp_obj_get_int(args_in[1]);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args_in[2], &bufinfo, MP_BUFFER_READ);
    const void *color_data = bufinfo.buf;
    check_esp_err(esp_lcd_panel_io_tx_color(self->io_handle, cmd, color_data, bufinfo.len));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_esp32s3_i80lcd_tx_color_obj, 3, 3, mp_esp32s3_i80lcd_tx_color);

STATIC const mp_rom_map_elem_t mp_esp32s3_i80lcd_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_tx_param),    MP_ROM_PTR(&mp_esp32s3_i80lcd_tx_param_obj)   },
    { MP_ROM_QSTR(MP_QSTR_tx_color),    MP_ROM_PTR(&mp_esp32s3_i80lcd_tx_color_obj)   },
    { MP_ROM_QSTR(MP_QSTR_deinit),      MP_ROM_PTR(&mp_esp32s3_i80lcd_deinit_obj)     },
    { MP_ROM_QSTR(MP_QSTR___del__),     MP_ROM_PTR(&mp_esp32s3_i80lcd_deinit_obj)     },
};
STATIC MP_DEFINE_CONST_DICT(mp_esp32s3_i80lcd_locals_dict, mp_esp32s3_i80lcd_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    mp_esp32s3_i80lcd_type,
    MP_QSTR_I80,
    MP_TYPE_FLAG_NONE,
    make_new, mp_esp32s3_i80lcd_make_new,
    print, mp_esp32s3_i80lcd_print,
    locals_dict, &mp_esp32s3_i80lcd_locals_dict
    );
// **************************************************************************************************

// RGB_LCD*******************************************************************************************
STATIC void mp_esp32s3_rgblcd_print(const mp_print_t *print,
    mp_obj_t self_in,
    mp_print_kind_t kind) {
    (void)kind;
    mp_esp32s3_rgblcd_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(
        print,
        "<rgb pclk=%u, width=%u, height=%u>",
        self->panel_config.timings.pclk_hz,
        self->panel_config.timings.h_res,
        self->panel_config.timings.v_res
        );
}

mp_obj_t mp_esp32s3_rgblcd_make_new(const mp_obj_type_t *type,
    size_t n_args,
    size_t n_kw,
    const mp_obj_t *all_args) {
    enum {
        ARG_data,
        ARG_hsync,
        ARG_vsync,
        ARG_de,
        ARG_pclk_pin,
        ARG_hsync_pulse_width,
        ARG_hsync_back_porch,
        ARG_hsync_front_porch,
        ARG_vsync_pulse_width,
        ARG_vsync_back_porch,
        ARG_vsync_front_porch,
        ARG_disp,
        ARG_backlight,
        ARG_pclk,
        ARG_width,
        ARG_height,
        #ifdef MICROPY_ESP_ENABLE_PSRAM_OCT
        ARG_num_fbs,
        ARG_use_bounce_buffer,
        ARG_use_psram,
        ARG_refresh_on_demand,
        #endif
        ARG_flushdown_callback,
    };
    const mp_arg_t allowed_args[] = {
        { MP_QSTR_data,                MP_ARG_OBJ | MP_ARG_REQUIRED | MP_ARG_KW_ONLY, {.u_obj = MP_OBJ_NULL  } },
        { MP_QSTR_hsync,               MP_ARG_OBJ | MP_ARG_REQUIRED | MP_ARG_KW_ONLY, {.u_obj = MP_OBJ_NULL  } },
        { MP_QSTR_vsync,               MP_ARG_OBJ | MP_ARG_REQUIRED | MP_ARG_KW_ONLY, {.u_obj = MP_OBJ_NULL  } },
        { MP_QSTR_de,                  MP_ARG_OBJ | MP_ARG_REQUIRED | MP_ARG_KW_ONLY, {.u_obj = MP_OBJ_NULL  } },
        { MP_QSTR_pclk_pin,            MP_ARG_OBJ | MP_ARG_REQUIRED | MP_ARG_KW_ONLY, {.u_obj = MP_OBJ_NULL  } },
        { MP_QSTR_hsync_pulse_width,   MP_ARG_OBJ | MP_ARG_REQUIRED | MP_ARG_KW_ONLY, {.u_obj = MP_OBJ_NULL  } },
        { MP_QSTR_hsync_back_porch,    MP_ARG_OBJ | MP_ARG_REQUIRED | MP_ARG_KW_ONLY, {.u_obj = MP_OBJ_NULL  } },
        { MP_QSTR_hsync_front_porch,   MP_ARG_OBJ | MP_ARG_REQUIRED | MP_ARG_KW_ONLY, {.u_obj = MP_OBJ_NULL  } },
        { MP_QSTR_vsync_pulse_width,   MP_ARG_OBJ | MP_ARG_REQUIRED | MP_ARG_KW_ONLY, {.u_obj = MP_OBJ_NULL  } },
        { MP_QSTR_vsync_back_porch,    MP_ARG_OBJ | MP_ARG_REQUIRED | MP_ARG_KW_ONLY, {.u_obj = MP_OBJ_NULL  } },
        { MP_QSTR_vsync_front_porch,   MP_ARG_OBJ | MP_ARG_REQUIRED | MP_ARG_KW_ONLY, {.u_obj = MP_OBJ_NULL  } },
        { MP_QSTR_disp,                MP_ARG_OBJ | MP_ARG_KW_ONLY,                   {.u_obj = MP_OBJ_NULL  } },
        { MP_QSTR_backlight,           MP_ARG_OBJ | MP_ARG_KW_ONLY,                   {.u_obj = MP_OBJ_NULL  } },
        { MP_QSTR_pclk,                MP_ARG_INT | MP_ARG_KW_ONLY,                   {.u_int = 10000000     } },
        { MP_QSTR_width,               MP_ARG_INT | MP_ARG_KW_ONLY,                   {.u_int = 800          } },
        { MP_QSTR_height,              MP_ARG_INT | MP_ARG_KW_ONLY,                   {.u_int = 480          } },
        #ifdef MICROPY_ESP_ENABLE_PSRAM_OCT
        { MP_QSTR_num_fbs,             MP_ARG_INT | MP_ARG_KW_ONLY,                   {.u_int = 1            } },
        { MP_QSTR_use_bounce_buffer,   MP_ARG_BOOL | MP_ARG_KW_ONLY,                  {.u_bool = true        } },
        { MP_QSTR_use_psram,           MP_ARG_BOOL | MP_ARG_KW_ONLY,                  {.u_bool = true        } },
        { MP_QSTR_refresh_on_demand,   MP_ARG_BOOL | MP_ARG_KW_ONLY,                  {.u_bool = false       } },
        #endif
        { MP_QSTR_flushdown_callback,  MP_ARG_OBJ | MP_ARG_KW_ONLY,                   {.u_obj = mp_const_none} },

    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    // Detect the LCD_CAM singleton
    esp32s3_lcd_cam_obj_t *lcd_cam = MP_STATE_PORT(esp32s3_lcd_cam_singleton);
    if (lcd_cam != NULL) {
        mp_esp32s3_lcd_cam_global_deinit();
    } else {
        lcd_cam = m_new_obj(esp32s3_lcd_cam_obj_t);
        lcd_cam->base.type = &esp32s3_lcd_cam_type;
        // Set the global singleton pointer for the esp32s3 lcd_cam protocol.
        MP_STATE_PORT(esp32s3_lcd_cam_singleton) = lcd_cam;
    }

    // create new rgb_lcd
    mp_esp32s3_rgblcd_obj_t *self = m_new_obj(mp_esp32s3_rgblcd_obj_t);
    lcd_cam->rgblcd_obj = self;
    self->base.type = &mp_esp32s3_rgblcd_type;

    self->flushdown_callback = args[ARG_flushdown_callback].u_obj;
    // rgb panel_handle
    self->panel_handle = NULL;
    self->panel_config.data_width = 16;// RGB565
    self->panel_config.psram_trans_align = 64;
    #ifdef MICROPY_ESP_ENABLE_PSRAM_OCT
    if (args[ARG_num_fbs].u_int <= 3) {
        self->panel_config.num_fbs = args[ARG_num_fbs].u_int;
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid num_fbs, must <=3"));
    }
    if (args[ARG_use_bounce_buffer].u_bool) {
        self->panel_config.bounce_buffer_size_px = 10 * args[ARG_width].u_int;
    }
    #else
    self->panel_config.num_fbs = 0;
    #endif
    self->panel_config.clk_src = LCD_CLK_SRC_DEFAULT;
    if (args[ARG_disp].u_obj != MP_OBJ_NULL) {
        self->panel_config.disp_gpio_num = mp_hal_get_pin_obj(args[ARG_disp].u_obj);
    } else {
        self->panel_config.disp_gpio_num = -1;
    }
    self->panel_config.pclk_gpio_num = mp_hal_get_pin_obj(args[ARG_pclk_pin].u_obj);
    self->panel_config.vsync_gpio_num = mp_hal_get_pin_obj(args[ARG_vsync].u_obj);
    self->panel_config.hsync_gpio_num = mp_hal_get_pin_obj(args[ARG_hsync].u_obj);
    self->panel_config.de_gpio_num = mp_hal_get_pin_obj(args[ARG_de].u_obj);
    // data bus
    mp_obj_tuple_t *t = MP_OBJ_TO_PTR(args[ARG_data].u_obj);
    for (size_t i = 0; i < t->len; i++) {
        self->panel_config.data_gpio_nums[i] = mp_hal_get_pin_obj(t->items[i]);
    }
    // timings
    self->panel_config.timings.pclk_hz = args[ARG_pclk].u_int;
    self->panel_config.timings.h_res = args[ARG_width].u_int;
    self->panel_config.timings.v_res = args[ARG_height].u_int;
    self->panel_config.timings.hsync_pulse_width = args[ARG_hsync_pulse_width].u_int;
    self->panel_config.timings.hsync_back_porch = args[ARG_hsync_back_porch].u_int;
    self->panel_config.timings.hsync_front_porch = args[ARG_hsync_front_porch].u_int;
    self->panel_config.timings.vsync_pulse_width = args[ARG_vsync_pulse_width].u_int;
    self->panel_config.timings.vsync_back_porch = args[ARG_vsync_back_porch].u_int;
    self->panel_config.timings.vsync_front_porch = args[ARG_vsync_front_porch].u_int;
    self->panel_config.timings.flags.pclk_active_neg = 1;
    #ifdef MICROPY_ESP_ENABLE_PSRAM_OCT
    if (self->panel_config.num_fbs == 0) {
        self->panel_config.flags.no_fb = 1;
    }
    if (args[ARG_use_psram].u_bool) {
        self->panel_config.flags.fb_in_psram = 1;
    }
    if (args[ARG_refresh_on_demand].u_bool) {
        self->panel_config.flags.refresh_on_demand = 1;
    }
    #else
    self->panel_config.flags.no_fb = 1;
    self->panel_config.flags.fb_in_psram = 0;
    self->panel_config.flags.refresh_on_demand = 1;
    #endif

    check_esp_err(esp_lcd_new_rgb_panel(&self->panel_config, &self->panel_handle));
    check_esp_err(esp_lcd_panel_reset(self->panel_handle));
    check_esp_err(esp_lcd_panel_init(self->panel_handle));
    #ifdef MICROPY_ESP_ENABLE_PSRAM_OCT
    self->buf1 = NULL;
    self->buf2 = NULL;
    self->buf3 = NULL;

    if (self->panel_config.num_fbs == 1) {
        check_esp_err(esp_lcd_rgb_panel_get_frame_buffer(self->panel_handle, 1, &self->buf1));
    } else if (self->panel_config.num_fbs == 2) {
        check_esp_err(esp_lcd_rgb_panel_get_frame_buffer(self->panel_handle, 2, &self->buf1, &self->buf2));
    } else if (self->panel_config.num_fbs == 3) {
        check_esp_err(esp_lcd_rgb_panel_get_frame_buffer(self->panel_handle, 3, &self->buf1, &self->buf2, &self->buf3));
    }
    #endif
    // init backlight. Turn off LCD backlight
    self->bk = args[ARG_backlight].u_obj;
    if (self->bk != MP_OBJ_NULL) {
        mp_hal_pin_obj_t bk_pin = mp_hal_get_pin_obj(self->bk);
        mp_hal_pin_output(bk_pin);
        mp_hal_pin_write(bk_pin, 0);
    }
    DEBUG_printf("rgb_init_success\n");
    // record the LCD_CAM state
    lcd_cam->state = MP_HW_LCD_CAM_STATE_RGBINIT;
    DEBUG_printf("MP_HW_LCD_CAM_STATE_RGBINIT\n");
    return MP_OBJ_FROM_PTR(self);
}

void esp32s3_hw_rgblcd_deinit(mp_esp32s3_rgblcd_obj_t *rgblcd) {
    check_esp_err(esp_lcd_panel_del(rgblcd->panel_handle));
}

STATIC mp_obj_t mp_esp32s3_rgblcd_deinit(mp_obj_t _rgblcd) {
    esp32s3_lcd_cam_obj_t *lcd_cam = _get_lcd_cam_singleton();
    mp_esp32s3_rgblcd_obj_t *self = MP_OBJ_TO_PTR(_rgblcd);

    esp32s3_hw_rgblcd_deinit(self);
    m_del_obj(mp_esp32s3_rgblcd_obj_t, self);
    lcd_cam->state = MP_HW_LCD_CAM_STATE_NONE;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_esp32s3_rgblcd_deinit_obj, mp_esp32s3_rgblcd_deinit);

STATIC mp_obj_t mp_esp32s3_rgblcd_init(mp_obj_t _rgblcd) {
    mp_esp32s3_rgblcd_obj_t *self = _rgblcd;

    check_esp_err(esp_lcd_panel_init(self->panel_handle));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_esp32s3_rgblcd_init_obj, mp_esp32s3_rgblcd_init);

STATIC mp_obj_t mp_esp32s3_rgblcd_reset(mp_obj_t _rgblcd) {
    mp_esp32s3_rgblcd_obj_t *self = _rgblcd;

    check_esp_err(esp_lcd_panel_reset(self->panel_handle));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_esp32s3_rgblcd_reset_obj, mp_esp32s3_rgblcd_reset);

STATIC mp_obj_t mp_esp32s3_rgblcd_bitmap(size_t n_args, const mp_obj_t *args_in) {
    mp_esp32s3_rgblcd_obj_t *self = MP_OBJ_TO_PTR(args_in[0]);

    DEBUG_printf("bitmapping\n");
    int x_start = mp_obj_get_int(args_in[1]);
    int y_start = mp_obj_get_int(args_in[2]);
    int x_end = mp_obj_get_int(args_in[3]);
    int y_end = mp_obj_get_int(args_in[4]);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args_in[5], &bufinfo, MP_BUFFER_READ);
    const void *color_data = bufinfo.buf;
    check_esp_err(esp_lcd_panel_draw_bitmap(
        self->panel_handle,
        x_start,
        y_start,
        x_end + 1,
        y_end + 1,
        color_data
        ));

    if (self->flushdown_callback != mp_const_none) {
        mp_sched_schedule(self->flushdown_callback, self);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_esp32s3_rgblcd_bitmap_obj, 6, 6, mp_esp32s3_rgblcd_bitmap);

STATIC mp_obj_t mp_esp32s3_rgblcd_mirror(mp_obj_t self_in,
    mp_obj_t mirror_x_in,
    mp_obj_t mirror_y_in) {
    mp_esp32s3_rgblcd_obj_t *self = MP_OBJ_TO_PTR(self_in);

    check_esp_err(esp_lcd_panel_mirror(
        self->panel_handle,
        mp_obj_is_true(mirror_x_in),
        mp_obj_is_true(mirror_y_in)
        ));

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(mp_esp32s3_rgblcd_mirror_obj, mp_esp32s3_rgblcd_mirror);

STATIC mp_obj_t mp_esp32s3_rgblcd_swap_xy(mp_obj_t self_in, mp_obj_t swap_axes_in) {
    mp_esp32s3_rgblcd_obj_t *self = MP_OBJ_TO_PTR(self_in);

    check_esp_err(esp_lcd_panel_swap_xy(self->panel_handle, mp_obj_is_true(swap_axes_in)));

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mp_esp32s3_rgblcd_swap_xy_obj, mp_esp32s3_rgblcd_swap_xy);

STATIC mp_obj_t mp_esp32s3_rgblcd_set_gap(mp_obj_t self_in,
    mp_obj_t x_gap_in,
    mp_obj_t y_gap_in) {
    mp_esp32s3_rgblcd_obj_t *self = MP_OBJ_TO_PTR(self_in);

    check_esp_err(esp_lcd_panel_set_gap(
        self->panel_handle,
        mp_obj_get_int(x_gap_in),
        mp_obj_get_int(y_gap_in)
        ));

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(mp_esp32s3_rgblcd_set_gap_obj, mp_esp32s3_rgblcd_set_gap);

STATIC mp_obj_t mp_esp32s3_rgblcd_invert_color(mp_obj_t self_in, mp_obj_t invert_in) {
    mp_esp32s3_rgblcd_obj_t *self = MP_OBJ_TO_PTR(self_in);

    check_esp_err(esp_lcd_panel_invert_color(self->panel_handle, mp_obj_is_true(invert_in)));

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mp_esp32s3_rgblcd_invert_color_obj, mp_esp32s3_rgblcd_invert_color);

STATIC mp_obj_t mp_esp32s3_rgblcd_disp_on_off(mp_obj_t self_in, mp_obj_t on_off_in) {
    mp_esp32s3_rgblcd_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->panel_config.disp_gpio_num != -1) {
        check_esp_err(esp_lcd_panel_disp_on_off(self->panel_handle, mp_obj_is_true(on_off_in)));
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mp_esp32s3_rgblcd_disp_on_off_obj, mp_esp32s3_rgblcd_disp_on_off);

STATIC mp_obj_t mp_esp32s3_rgblcd_backlight_on(mp_obj_t self_in) {
    mp_esp32s3_rgblcd_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->bk != MP_OBJ_NULL) {
        mp_hal_pin_obj_t bk_pin = mp_hal_get_pin_obj(self->bk);
        mp_hal_pin_write(bk_pin, 1);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_esp32s3_rgblcd_backlight_on_obj, mp_esp32s3_rgblcd_backlight_on);

STATIC mp_obj_t mp_esp32s3_rgblcd_backlight_off(mp_obj_t self_in) {
    mp_esp32s3_rgblcd_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->bk != MP_OBJ_NULL) {
        mp_hal_pin_obj_t bk_pin = mp_hal_get_pin_obj(self->bk);
        mp_hal_pin_write(bk_pin, 0);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_esp32s3_rgblcd_backlight_off_obj, mp_esp32s3_rgblcd_backlight_off);

STATIC const mp_rom_map_elem_t mp_esp32s3_rgblcd_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit),        MP_ROM_PTR(&mp_esp32s3_rgblcd_deinit_obj)              },
    { MP_ROM_QSTR(MP_QSTR_reset),         MP_ROM_PTR(&mp_esp32s3_rgblcd_reset_obj)               },
    { MP_ROM_QSTR(MP_QSTR_init),          MP_ROM_PTR(&mp_esp32s3_rgblcd_init_obj)                },
    { MP_ROM_QSTR(MP_QSTR_bitmap),        MP_ROM_PTR(&mp_esp32s3_rgblcd_bitmap_obj)              },
    { MP_ROM_QSTR(MP_QSTR_mirror),        MP_ROM_PTR(&mp_esp32s3_rgblcd_mirror_obj)              },
    { MP_ROM_QSTR(MP_QSTR_swap_xy),       MP_ROM_PTR(&mp_esp32s3_rgblcd_swap_xy_obj)             },
    { MP_ROM_QSTR(MP_QSTR_set_gap),       MP_ROM_PTR(&mp_esp32s3_rgblcd_set_gap_obj)             },
    { MP_ROM_QSTR(MP_QSTR_invert_color),  MP_ROM_PTR(&mp_esp32s3_rgblcd_invert_color_obj)        },
    { MP_ROM_QSTR(MP_QSTR_disp_on_off),   MP_ROM_PTR(&mp_esp32s3_rgblcd_disp_on_off_obj)         },
    { MP_ROM_QSTR(MP_QSTR_backlight_on),  MP_ROM_PTR(&mp_esp32s3_rgblcd_backlight_on_obj)        },
    { MP_ROM_QSTR(MP_QSTR_backlight_off), MP_ROM_PTR(&mp_esp32s3_rgblcd_backlight_off_obj)       },
    { MP_ROM_QSTR(MP_QSTR___del__),       MP_ROM_PTR(&mp_esp32s3_rgblcd_deinit_obj)              },
};
STATIC MP_DEFINE_CONST_DICT(mp_esp32s3_rgblcd_locals_dict, mp_esp32s3_rgblcd_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    mp_esp32s3_rgblcd_type,
    MP_QSTR_RGB,
    MP_TYPE_FLAG_NONE,
    make_new, mp_esp32s3_rgblcd_make_new,
    print, mp_esp32s3_rgblcd_print,
    locals_dict, &mp_esp32s3_rgblcd_locals_dict
    );
// ********************************************************************************************

// CAM
// TODO
STATIC void mp_esp32s3_cam_print(const mp_print_t *print,
    mp_obj_t self_in,
    mp_print_kind_t kind) {
    (void)kind;
    mp_esp32s3_cam_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(
        print,
        "<cam=%p>",
        self
        );
}

mp_obj_t mp_esp32s3_cam_make_new(const mp_obj_type_t *type,
    size_t n_args,
    size_t n_kw,
    const mp_obj_t *all_args) {
    enum {
        ARG_data,
        ARG_hsync,
        ARG_vsync,
        ARG_de,
        ARG_pclk_pin,
        ARG_hsync_pulse_width,
        ARG_hsync_back_porch,
        ARG_hsync_front_porch,
        ARG_vsync_pulse_width,
        ARG_vsync_back_porch,
        ARG_vsync_front_porch,
        ARG_disp,
        ARG_backlight,
        ARG_pclk,
        ARG_width,
        ARG_height,
        ARG_num_fbs,
        ARG_use_bounce_buffer,
        ARG_use_psram,
        ARG_refresh_on_demand,
        ARG_flushdown_callback,
    };
    const mp_arg_t allowed_args[] = {
        { MP_QSTR_data,                MP_ARG_OBJ | MP_ARG_REQUIRED | MP_ARG_KW_ONLY, {.u_obj = MP_OBJ_NULL  } },
        { MP_QSTR_hsync,               MP_ARG_OBJ | MP_ARG_REQUIRED | MP_ARG_KW_ONLY, {.u_obj = MP_OBJ_NULL  } },
        { MP_QSTR_vsync,               MP_ARG_OBJ | MP_ARG_REQUIRED | MP_ARG_KW_ONLY, {.u_obj = MP_OBJ_NULL  } },
        { MP_QSTR_de,                  MP_ARG_OBJ | MP_ARG_REQUIRED | MP_ARG_KW_ONLY, {.u_obj = MP_OBJ_NULL  } },
        { MP_QSTR_pclk_pin,            MP_ARG_OBJ | MP_ARG_REQUIRED | MP_ARG_KW_ONLY, {.u_obj = MP_OBJ_NULL  } },
        { MP_QSTR_hsync_pulse_width,   MP_ARG_OBJ | MP_ARG_REQUIRED | MP_ARG_KW_ONLY, {.u_obj = MP_OBJ_NULL  } },
        { MP_QSTR_hsync_back_porch,    MP_ARG_OBJ | MP_ARG_REQUIRED | MP_ARG_KW_ONLY, {.u_obj = MP_OBJ_NULL  } },
        { MP_QSTR_hsync_front_porch,   MP_ARG_OBJ | MP_ARG_REQUIRED | MP_ARG_KW_ONLY, {.u_obj = MP_OBJ_NULL  } },
        { MP_QSTR_vsync_pulse_width,   MP_ARG_OBJ | MP_ARG_REQUIRED | MP_ARG_KW_ONLY, {.u_obj = MP_OBJ_NULL  } },
        { MP_QSTR_vsync_back_porch,    MP_ARG_OBJ | MP_ARG_REQUIRED | MP_ARG_KW_ONLY, {.u_obj = MP_OBJ_NULL  } },
        { MP_QSTR_vsync_front_porch,   MP_ARG_OBJ | MP_ARG_REQUIRED | MP_ARG_KW_ONLY, {.u_obj = MP_OBJ_NULL  } },
        { MP_QSTR_disp,                MP_ARG_OBJ | MP_ARG_KW_ONLY,                   {.u_obj = MP_OBJ_NULL  } },
        { MP_QSTR_backlight,           MP_ARG_OBJ | MP_ARG_KW_ONLY,                   {.u_obj = MP_OBJ_NULL  } },
        { MP_QSTR_pclk,                MP_ARG_INT | MP_ARG_KW_ONLY,                   {.u_int = 12000000     } },
        { MP_QSTR_width,               MP_ARG_INT | MP_ARG_KW_ONLY,                   {.u_int = 800          } },
        { MP_QSTR_height,              MP_ARG_INT | MP_ARG_KW_ONLY,                   {.u_int = 480          } },
        { MP_QSTR_num_fbs,             MP_ARG_INT | MP_ARG_KW_ONLY,                   {.u_int = 0            } },
        { MP_QSTR_use_bounce_buffer,   MP_ARG_BOOL | MP_ARG_KW_ONLY,                  {.u_bool = false       } },
        { MP_QSTR_use_psram,           MP_ARG_BOOL | MP_ARG_KW_ONLY,                  {.u_bool = false       } },
        { MP_QSTR_refresh_on_demand,   MP_ARG_BOOL | MP_ARG_KW_ONLY,                  {.u_bool = true        } },
        { MP_QSTR_flushdown_callback,  MP_ARG_OBJ | MP_ARG_KW_ONLY,                   {.u_obj = mp_const_none} },

    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    // Detect the LCD_CAM singleton
    esp32s3_lcd_cam_obj_t *lcd_cam = MP_STATE_PORT(esp32s3_lcd_cam_singleton);
    if (lcd_cam != NULL) {
        mp_esp32s3_lcd_cam_global_deinit();
    } else {
        lcd_cam = m_new_obj(esp32s3_lcd_cam_obj_t);
        lcd_cam->base.type = &esp32s3_lcd_cam_type;
        // Set the global singleton pointer for the esp32s3 lcd_cam protocol.
        MP_STATE_PORT(esp32s3_lcd_cam_singleton) = lcd_cam;
    }
    // create new cam
    mp_esp32s3_cam_obj_t *self = m_new_obj(mp_esp32s3_cam_obj_t);
    lcd_cam->cam_obj = self;
    self->base.type = &mp_esp32s3_cam_type;
    DEBUG_printf("cam_init_success\n");
    // record the LCD_CAM state
    lcd_cam->state = MP_HW_LCD_CAM_STATE_CAMINIT;
    return MP_OBJ_FROM_PTR(self);
}

void esp32s3_hw_cam_deinit(mp_esp32s3_cam_obj_t *cam) {
    // check_esp_err(esp_lcd_panel_del(cam->cam_handle));
}

STATIC mp_obj_t mp_esp32s3_cam_deinit(mp_obj_t _cam) {
    esp32s3_lcd_cam_obj_t *lcd_cam = _get_lcd_cam_singleton();
    mp_esp32s3_cam_obj_t *self = MP_OBJ_TO_PTR(_cam);

    esp32s3_hw_cam_deinit(self);
    m_del_obj(mp_esp32s3_cam_obj_t, self);
    lcd_cam->state = MP_HW_LCD_CAM_STATE_NONE;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_esp32s3_cam_deinit_obj, mp_esp32s3_cam_deinit);

STATIC const mp_rom_map_elem_t mp_esp32s3_cam_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit),        MP_ROM_PTR(&mp_esp32s3_cam_deinit_obj)        },
    { MP_ROM_QSTR(MP_QSTR___del__),       MP_ROM_PTR(&mp_esp32s3_cam_deinit_obj)        },
};
STATIC MP_DEFINE_CONST_DICT(mp_esp32s3_cam_locals_dict, mp_esp32s3_cam_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    mp_esp32s3_cam_type,
    MP_QSTR_CAM,
    MP_TYPE_FLAG_NONE,
    make_new, mp_esp32s3_cam_make_new,
    print, mp_esp32s3_cam_print,
    locals_dict, &mp_esp32s3_cam_locals_dict
    );
// ********************************************************************************************

// LCD_CAM
STATIC void esp32s3_lcd_cam_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    esp32s3_lcd_cam_obj_t *self = self_in;
    mp_printf(print, "lcd_cam(%p; ", self);
}

STATIC mp_obj_t esp32s3_lcd_cam_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    esp32s3_lcd_cam_obj_t *self = MP_STATE_PORT(esp32s3_lcd_cam_singleton);
    if (self != NULL) {
        return self;
    }
    self = m_new_obj(esp32s3_lcd_cam_obj_t);
    self->base.type = &esp32s3_lcd_cam_type;
    // Set the global singleton pointer for the espnow protocol.
    MP_STATE_PORT(esp32s3_lcd_cam_singleton) = self;

    return self;
}

STATIC mp_obj_t esp32s3_lcd_cam_deinit(mp_obj_t self_in) {
    mp_esp32s3_lcd_cam_global_deinit();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32s3_lcd_cam_deinit_obj, esp32s3_lcd_cam_deinit);

STATIC const mp_rom_map_elem_t esp32s3_lcd_cam_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit),        MP_ROM_PTR(&esp32s3_lcd_cam_deinit_obj)        },
    { MP_ROM_QSTR(MP_QSTR___del__),       MP_ROM_PTR(&esp32s3_lcd_cam_deinit_obj)        },
};
STATIC MP_DEFINE_CONST_DICT(esp32s3_lcd_cam_locals_dict, esp32s3_lcd_cam_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    esp32s3_lcd_cam_type,
    MP_QSTR_LCD_CAM,
    MP_TYPE_FLAG_NONE,
    make_new, esp32s3_lcd_cam_make_new,
    print, esp32s3_lcd_cam_print,
    locals_dict, &esp32s3_lcd_cam_locals_dict
    );

// ************************************************************************************************
STATIC const mp_map_elem_t mp_module_esp32s3_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_esp32s3)  },
    { MP_ROM_QSTR(MP_QSTR_I80),      (mp_obj_t)&mp_esp32s3_i80lcd_type },
    { MP_ROM_QSTR(MP_QSTR_RGB),      (mp_obj_t)&mp_esp32s3_rgblcd_type },
    { MP_ROM_QSTR(MP_QSTR_CAM),      (mp_obj_t)&mp_esp32s3_cam_type    },
    { MP_ROM_QSTR(MP_QSTR_LCD_CAM),  (mp_obj_t)&esp32s3_lcd_cam_type   },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_esp32s3_globals, mp_module_esp32s3_globals_table);


const mp_obj_module_t mp_module_esp32s3 = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_esp32s3_globals,
};

MP_REGISTER_MODULE(MP_QSTR_esp32s3, mp_module_esp32s3);
MP_REGISTER_ROOT_POINTER(struct _esp32s3_lcd_cam_obj_t *esp32s3_lcd_cam_singleton);
