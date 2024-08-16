// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "esp_intr_alloc.h"
#include "esp_lcd_panel_interface.h"
#include "esp_lcd_panel_rgb.h"
#include "esp_pm.h"
#include "esp_private/gdma.h"
#include "hal/dma_types.h"
#include "hal/lcd_hal.h"
#include "hal/lcd_ll.h"
#include "soc/lcd_periph.h"

#include "esp_log.h"
#define TAG "LCD"

#include "esp_rom_sys.h"

#include "bindings/espidf/__init__.h"
#include "py/objarray.h"
#include "shared-bindings/dotclockframebuffer/DotClockFramebuffer.h"
#include "common-hal/dotclockframebuffer/DotClockFramebuffer.h"
#include "common-hal/espidf/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "py/runtime.h"
#include "driver/gpio.h"
#include "esp_rom_gpio.h"
#include "hal/lcd_ll.h"
#include "hal/gpio_hal.h"
#include "soc/lcd_cam_struct.h"
#include "esp_heap_caps.h"

// should be from rom/cache.h but it wasn't working
int Cache_WriteBack_Addr(uint32_t addr, uint32_t size);

#define LCD_RGB_ISR_IRAM_SAFE (1)
#define LCD_RGB_INTR_ALLOC_FLAGS     (ESP_INTR_FLAG_IRAM | ESP_INTR_FLAG_INTRDISABLED)

#define common_hal_mcu_pin_number_maybe(x) ((x) ? common_hal_mcu_pin_number((x)) : -1)

static void claim_and_record(const mcu_pin_obj_t *pin, uint64_t *used_pins_mask) {
    if (pin) {
        int number = common_hal_mcu_pin_number(pin);
        *used_pins_mask |= (UINT64_C(1) << number);
        claim_pin_number(number);
        never_reset_pin_number(number);
    }
}

static int valid_pin(const mcu_pin_obj_t *pin, qstr name) {
    int result = common_hal_mcu_pin_number(pin);
    if (result == NO_PIN) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("Invalid %q pin"), name);
    }
    return result;
}

void common_hal_dotclockframebuffer_framebuffer_construct(dotclockframebuffer_framebuffer_obj_t *self,
    const mcu_pin_obj_t *de,
    const mcu_pin_obj_t *vsync,
    const mcu_pin_obj_t *hsync,
    const mcu_pin_obj_t *dclk,
    const mcu_pin_obj_t **red, uint8_t num_red,
    const mcu_pin_obj_t **green, uint8_t num_green,
    const mcu_pin_obj_t **blue, uint8_t num_blue,
    int frequency, int width, int height,
    int hsync_pulse_width, int hsync_back_porch, int hsync_front_porch, bool hsync_idle_low,
    int vsync_pulse_width, int vsync_back_porch, int vsync_front_porch, bool vsync_idle_low,
    bool de_idle_high, bool pclk_active_high, bool pclk_idle_high, int overscan_left) {

    if (num_red != 5 || num_green != 6 || num_blue != 5) {
        mp_raise_ValueError(MP_ERROR_TEXT("Must provide 5/6/5 RGB pins"));
    }

    claim_and_record(de, &self->used_pins_mask);
    claim_and_record(vsync, &self->used_pins_mask);
    claim_and_record(hsync, &self->used_pins_mask);
    claim_and_record(dclk, &self->used_pins_mask);

    for (size_t i = 0; i < num_red; i++) {
        claim_and_record(red[i], &self->used_pins_mask);
    }
    for (size_t i = 0; i < num_green; i++) {
        claim_and_record(green[i], &self->used_pins_mask);
    }
    for (size_t i = 0; i < num_blue; i++) {
        claim_and_record(blue[i], &self->used_pins_mask);
    }

    esp_lcd_rgb_panel_config_t *cfg = &self->panel_config;
    cfg->timings.pclk_hz = frequency;
    cfg->timings.h_res = (width + overscan_left + 15) / 16 * 16; // round up to multiple of 16
    cfg->timings.v_res = height;
    cfg->timings.hsync_pulse_width = hsync_pulse_width;
    cfg->timings.hsync_back_porch = hsync_back_porch;
    cfg->timings.hsync_front_porch = hsync_front_porch;
    cfg->timings.vsync_pulse_width = vsync_pulse_width;
    cfg->timings.vsync_back_porch = vsync_back_porch;
    cfg->timings.vsync_front_porch = vsync_front_porch;
    cfg->timings.flags.hsync_idle_low = hsync_idle_low;
    cfg->timings.flags.vsync_idle_low = hsync_idle_low;
    cfg->timings.flags.de_idle_high = de_idle_high;
    cfg->timings.flags.pclk_active_neg = !pclk_active_high;
    cfg->timings.flags.pclk_idle_high = pclk_idle_high;

    cfg->data_width = 16;
    cfg->sram_trans_align = 8;
    cfg->psram_trans_align = 64;
    cfg->hsync_gpio_num = valid_pin(hsync, MP_QSTR_hsync);
    cfg->vsync_gpio_num = valid_pin(vsync, MP_QSTR_vsync);
    cfg->de_gpio_num = valid_pin(de, MP_QSTR_de);
    cfg->pclk_gpio_num = valid_pin(dclk, MP_QSTR_dclk);
    cfg->clk_src = LCD_CLK_SRC_DEFAULT;

    cfg->data_gpio_nums[0] = valid_pin(blue[0], MP_QSTR_blue);
    cfg->data_gpio_nums[1] = valid_pin(blue[1], MP_QSTR_blue);
    cfg->data_gpio_nums[2] = valid_pin(blue[2], MP_QSTR_blue);
    cfg->data_gpio_nums[3] = valid_pin(blue[3], MP_QSTR_blue);
    cfg->data_gpio_nums[4] = valid_pin(blue[4], MP_QSTR_blue);

    cfg->data_gpio_nums[5] = valid_pin(green[0], MP_QSTR_green);
    cfg->data_gpio_nums[6] = valid_pin(green[1], MP_QSTR_green);
    cfg->data_gpio_nums[7] = valid_pin(green[2], MP_QSTR_green);
    cfg->data_gpio_nums[8] = valid_pin(green[3], MP_QSTR_green);
    cfg->data_gpio_nums[9] = valid_pin(green[4], MP_QSTR_green);
    cfg->data_gpio_nums[10] = valid_pin(green[5], MP_QSTR_green);

    cfg->data_gpio_nums[11] = valid_pin(red[0], MP_QSTR_red);
    cfg->data_gpio_nums[12] = valid_pin(red[1], MP_QSTR_red);
    cfg->data_gpio_nums[13] = valid_pin(red[2], MP_QSTR_red);
    cfg->data_gpio_nums[14] = valid_pin(red[3], MP_QSTR_red);
    cfg->data_gpio_nums[15] = valid_pin(red[4], MP_QSTR_red);

    cfg->disp_gpio_num = GPIO_NUM_NC;

    cfg->flags.disp_active_low = 0;
    cfg->flags.refresh_on_demand = 0;
    cfg->flags.fb_in_psram = 1; // allocate frame buffer in PSRAM

    esp_err_t ret = esp_lcd_new_rgb_panel(&self->panel_config, &self->panel_handle);
    CHECK_ESP_RESULT(ret);
    CHECK_ESP_RESULT(esp_lcd_panel_reset(self->panel_handle));
    CHECK_ESP_RESULT(esp_lcd_panel_init(self->panel_handle));

    uint16_t color = 0;
    CHECK_ESP_RESULT(self->panel_handle->draw_bitmap(self->panel_handle, 0, 0, 1, 1, &color));

    void *fb;
    CHECK_ESP_RESULT(esp_lcd_rgb_panel_get_frame_buffer(self->panel_handle, 1, &fb));

    self->frequency = frequency;
    self->width = width;
    self->row_stride = 2 * (cfg->timings.h_res);
    self->first_pixel_offset = 2 * overscan_left;
    self->refresh_rate = frequency / (cfg->timings.h_res + hsync_front_porch + hsync_back_porch) / (height + vsync_front_porch + vsync_back_porch);
    self->bufinfo.buf = (uint8_t *)fb;
    self->bufinfo.len = 2 * (cfg->timings.h_res * cfg->timings.v_res);
    self->bufinfo.typecode = 'H' | MP_OBJ_ARRAY_TYPECODE_FLAG_RW;

//  LCD_CAM.lcd_ctrl2.lcd_vsync_idle_pol = _vsync_polarity;
//  LCD_CAM.lcd_ctrl2.lcd_hsync_idle_pol = _hsync_polarity;

}


void common_hal_dotclockframebuffer_framebuffer_deinit(dotclockframebuffer_framebuffer_obj_t *self) {
    if (common_hal_dotclockframebuffer_framebuffer_deinitialized(self)) {
        return;
    }

    reset_pin_mask(self->used_pins_mask);
    self->used_pins_mask = 0;
    esp_lcd_panel_del(self->panel_handle);
}

bool common_hal_dotclockframebuffer_framebuffer_deinitialized(dotclockframebuffer_framebuffer_obj_t *self) {
    return self->used_pins_mask == 0;
}


mp_int_t common_hal_dotclockframebuffer_framebuffer_get_width(dotclockframebuffer_framebuffer_obj_t *self) {
    return self->width;
}

mp_int_t common_hal_dotclockframebuffer_framebuffer_get_height(dotclockframebuffer_framebuffer_obj_t *self) {
    return self->panel_config.timings.v_res;
}

mp_int_t common_hal_dotclockframebuffer_framebuffer_get_frequency(dotclockframebuffer_framebuffer_obj_t *self) {
    return self->frequency;
}

mp_int_t common_hal_dotclockframebuffer_framebuffer_get_row_stride(dotclockframebuffer_framebuffer_obj_t *self) {
    return self->row_stride;
}

mp_int_t common_hal_dotclockframebuffer_framebuffer_get_first_pixel_offset(dotclockframebuffer_framebuffer_obj_t *self) {
    return self->first_pixel_offset;
}

void common_hal_dotclockframebuffer_framebuffer_refresh(dotclockframebuffer_framebuffer_obj_t *self) {
    Cache_WriteBack_Addr((uint32_t)(self->bufinfo.buf), self->bufinfo.len);
}

mp_int_t common_hal_dotclockframebuffer_framebuffer_get_refresh_rate(dotclockframebuffer_framebuffer_obj_t *self) {
    return self->refresh_rate;
}
