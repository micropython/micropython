/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Dryw Wade Clere at SparkFun Electronics
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

#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "pin_static_af.h"
#include "ltdc.h"

#if MICROPY_HW_ENABLE_LTDC

// The LTDC scans a frame buffer from memory out to a parallel RGB display.  Python code
// passes it any buffer (for example the bytearray behind a framebuf.FrameBuffer), which
// it then displays without copying.  The board sets the panel size, timing and pins.

#ifndef MICROPY_HW_LTDC_HSYNC_POL
#define MICROPY_HW_LTDC_HSYNC_POL (LTDC_HSPOLARITY_AL)
#endif
#ifndef MICROPY_HW_LTDC_VSYNC_POL
#define MICROPY_HW_LTDC_VSYNC_POL (LTDC_VSPOLARITY_AL)
#endif
#ifndef MICROPY_HW_LTDC_DE_POL
#define MICROPY_HW_LTDC_DE_POL (LTDC_DEPOLARITY_AL)
#endif
#ifndef MICROPY_HW_LTDC_PCLK_POL
#define MICROPY_HW_LTDC_PCLK_POL (LTDC_PCPOLARITY_IPC)
#endif

// Levels of the optional control pins when the display is on.
#ifndef MICROPY_HW_LTDC_PWR_ON
#define MICROPY_HW_LTDC_PWR_ON (1)
#endif
#ifndef MICROPY_HW_LTDC_DISP_ON
#define MICROPY_HW_LTDC_DISP_ON (1)
#endif
#ifndef MICROPY_HW_LTDC_BL_ON
#define MICROPY_HW_LTDC_BL_ON (1)
#endif

// Frame size in pixel clocks, including sync and porches.
#define LTDC_TOTAL_WIDTH (MICROPY_HW_LTDC_HSW + MICROPY_HW_LTDC_HBP + MICROPY_HW_LTDC_WIDTH + MICROPY_HW_LTDC_HFP)
#define LTDC_TOTAL_HEIGHT (MICROPY_HW_LTDC_VSW + MICROPY_HW_LTDC_VBP + MICROPY_HW_LTDC_HEIGHT + MICROPY_HW_LTDC_VFP)

// A reload waits at most a frame for vertical blanking.  Allow two, to cover the ms tick
// resolution and a pixel clock slower than requested.  This only catches a stopped LTDC.
#define LTDC_RELOAD_TIMEOUT_MS (2 * 1000ULL * LTDC_TOTAL_WIDTH * LTDC_TOTAL_HEIGHT / MICROPY_HW_LTDC_PCLK_HZ + 1)

typedef struct _ltdc_format_t {
    uint8_t ltdc_format;
    uint8_t bytes_per_pixel;
} ltdc_format_t;

// Indexed by the LTDC_FORMAT_xxx values below, which Python code sees as LTDC.xxx.
enum {
    LTDC_FORMAT_RGB565,
    LTDC_FORMAT_RGB888,
    LTDC_FORMAT_ARGB8888,
    LTDC_FORMAT_GS8,
    LTDC_FORMAT_NUM,
};

static const ltdc_format_t ltdc_formats[LTDC_FORMAT_NUM] = {
    [LTDC_FORMAT_RGB565] = { LTDC_PIXEL_FORMAT_RGB565, 2 },
    [LTDC_FORMAT_RGB888] = { LTDC_PIXEL_FORMAT_RGB888, 3 },
    [LTDC_FORMAT_ARGB8888] = { LTDC_PIXEL_FORMAT_ARGB8888, 4 },
    [LTDC_FORMAT_GS8] = { LTDC_PIXEL_FORMAT_L8, 1 },
};

typedef struct _stm_ltdc_obj_t {
    mp_obj_base_t base;
    LTDC_HandleTypeDef hltdc;
    uint8_t format;
    bool active;
    bool layer_enabled;
} stm_ltdc_obj_t;

static stm_ltdc_obj_t stm_ltdc_obj;

// The buffer being displayed and the one waiting for the next reload, so that neither
// is freed while the LTDC may still read it.
MP_REGISTER_ROOT_POINTER(mp_obj_t ltdc_buf[2]);

// Set a power, enable or backlight pin to its on or off level.
#define LTDC_CONTROL_PIN(name, on) \
    mp_hal_pin_output(MICROPY_HW_LTDC_##name##_PIN); \
    mp_hal_pin_write(MICROPY_HW_LTDC_##name##_PIN, (on) ? MICROPY_HW_LTDC_##name##_ON : !MICROPY_HW_LTDC_##name##_ON)

static void ltdc_control_pins(bool on) {
    #if defined(MICROPY_HW_LTDC_PWR_PIN)
    LTDC_CONTROL_PIN(PWR, on);
    #endif
    #if defined(MICROPY_HW_LTDC_DISP_PIN)
    LTDC_CONTROL_PIN(DISP, on);
    #endif
    #if defined(MICROPY_HW_LTDC_BL_PIN)
    LTDC_CONTROL_PIN(BL, on);
    #endif
}

// Configure the board's MICROPY_HW_LTDC_<signal> pin for the LTDC's LCD_<signal>.
#define LTDC_PIN(signal) \
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_LTDC_##signal, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_HIGH, STATIC_AF_LCD_##signal)

static void ltdc_init_pins(void) {
    LTDC_PIN(CLK);
    LTDC_PIN(HSYNC);
    LTDC_PIN(VSYNC);
    #if defined(MICROPY_HW_LTDC_DE)
    LTDC_PIN(DE);
    #endif

    // Boards may leave out the low bits of each colour.
    #if defined(MICROPY_HW_LTDC_R0)
    LTDC_PIN(R0);
    LTDC_PIN(R1);
    #endif
    #if defined(MICROPY_HW_LTDC_R2)
    LTDC_PIN(R2);
    #endif
    LTDC_PIN(R3);
    LTDC_PIN(R4);
    LTDC_PIN(R5);
    LTDC_PIN(R6);
    LTDC_PIN(R7);

    #if defined(MICROPY_HW_LTDC_G0)
    LTDC_PIN(G0);
    LTDC_PIN(G1);
    #endif
    LTDC_PIN(G2);
    LTDC_PIN(G3);
    LTDC_PIN(G4);
    LTDC_PIN(G5);
    LTDC_PIN(G6);
    LTDC_PIN(G7);

    #if defined(MICROPY_HW_LTDC_B0)
    LTDC_PIN(B0);
    LTDC_PIN(B1);
    #endif
    #if defined(MICROPY_HW_LTDC_B2)
    LTDC_PIN(B2);
    #endif
    LTDC_PIN(B3);
    LTDC_PIN(B4);
    LTDC_PIN(B5);
    LTDC_PIN(B6);
    LTDC_PIN(B7);
}

static void ltdc_init_clock(void) {
    #if defined(STM32N6)
    // Derive the pixel clock from PLL1 through IC16.
    uint32_t div = (HAL_RCCEx_GetPLL1CLKFreq() + MICROPY_HW_LTDC_PCLK_HZ / 2) / MICROPY_HW_LTDC_PCLK_HZ;
    if (div < 1) {
        div = 1;
    } else if (div > 256) {
        div = 256;
    }
    LL_RCC_IC16_SetSource(LL_RCC_ICCLKSOURCE_PLL1);
    LL_RCC_IC16_SetDivider(div);
    LL_RCC_IC16_Enable();
    LL_RCC_SetLTDCClockSource(LL_RCC_LTDC_CLKSOURCE_IC16);
    #else
    #error LTDC pixel clock not implemented for this MCU
    #endif

    // Keep the clock on while the CPU sleeps, otherwise the display goes blank when idle.
    __HAL_RCC_LTDC_CLK_ENABLE();
    __HAL_RCC_LTDC_CLK_SLEEP_ENABLE();
    __HAL_RCC_LTDC_FORCE_RESET();
    __HAL_RCC_LTDC_RELEASE_RESET();
}

static void ltdc_init(stm_ltdc_obj_t *self) {
    ltdc_init_pins();
    ltdc_init_clock();

    LTDC_HandleTypeDef *h = &self->hltdc;
    h->Instance = LTDC;
    h->Init.HSPolarity = MICROPY_HW_LTDC_HSYNC_POL;
    h->Init.VSPolarity = MICROPY_HW_LTDC_VSYNC_POL;
    h->Init.DEPolarity = MICROPY_HW_LTDC_DE_POL;
    h->Init.PCPolarity = MICROPY_HW_LTDC_PCLK_POL;
    h->Init.HorizontalSync = MICROPY_HW_LTDC_HSW - 1;
    h->Init.AccumulatedHBP = MICROPY_HW_LTDC_HSW + MICROPY_HW_LTDC_HBP - 1;
    h->Init.AccumulatedActiveW = MICROPY_HW_LTDC_HSW + MICROPY_HW_LTDC_HBP + MICROPY_HW_LTDC_WIDTH - 1;
    h->Init.TotalWidth = LTDC_TOTAL_WIDTH - 1;
    h->Init.VerticalSync = MICROPY_HW_LTDC_VSW - 1;
    h->Init.AccumulatedVBP = MICROPY_HW_LTDC_VSW + MICROPY_HW_LTDC_VBP - 1;
    h->Init.AccumulatedActiveH = MICROPY_HW_LTDC_VSW + MICROPY_HW_LTDC_VBP + MICROPY_HW_LTDC_HEIGHT - 1;
    h->Init.TotalHeigh = LTDC_TOTAL_HEIGHT - 1;
    h->Init.Backcolor.Red = 0;
    h->Init.Backcolor.Green = 0;
    h->Init.Backcolor.Blue = 0;
    if (HAL_LTDC_Init(h) != HAL_OK) {
        mp_raise_OSError(MP_EIO);
    }

    // Until show() is called the display shows the black background colour.
    self->layer_enabled = false;
    self->active = true;
    ltdc_control_pins(true);
}

void ltdc_deinit(void) {
    stm_ltdc_obj_t *self = &stm_ltdc_obj;
    if (self->active) {
        ltdc_control_pins(false);
        HAL_LTDC_DeInit(&self->hltdc);
        __HAL_RCC_LTDC_CLK_DISABLE();
        self->active = false;
    }
    MP_STATE_PORT(ltdc_buf)[0] = MP_OBJ_NULL;
    MP_STATE_PORT(ltdc_buf)[1] = MP_OBJ_NULL;
}

static bool ltdc_reload_pending(void) {
    #if defined(LTDC_LxRCR_VBR)
    return LTDC_Layer1->RCR & LTDC_LxRCR_VBR;
    #else
    return LTDC->SRCR & LTDC_SRCR_VBR;
    #endif
}

static void ltdc_wait_reload(void) {
    mp_uint_t t0 = mp_hal_ticks_ms();
    while (ltdc_reload_pending()) {
        if (mp_hal_ticks_ms() - t0 > LTDC_RELOAD_TIMEOUT_MS) {
            mp_raise_OSError(MP_ETIMEDOUT);
        }
        mp_event_handle_nowait();
    }
}

// Apply the new settings at the next vertical blanking period, to avoid tearing.
static void ltdc_reload(stm_ltdc_obj_t *self, bool wait) {
    #if defined(LTDC_LxRCR_VBR)
    HAL_LTDC_ReloadLayer(&self->hltdc, LTDC_RELOAD_VERTICAL_BLANKING, LTDC_LAYER_1);
    #else
    HAL_LTDC_Reload(&self->hltdc, LTDC_RELOAD_VERTICAL_BLANKING);
    #endif
    if (wait) {
        ltdc_wait_reload();
        // The previous buffer is no longer read, so let it be collected.
        MP_STATE_PORT(ltdc_buf)[0] = MP_OBJ_NULL;
    }
}

static void ltdc_show(stm_ltdc_obj_t *self, mp_obj_t buf_in, bool wait) {
    mp_buffer_info_t bufinfo;
    const ltdc_format_t *fmt = &ltdc_formats[self->format];
    if (buf_in != mp_const_none) {
        mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_READ);
        if (bufinfo.len < MICROPY_HW_LTDC_WIDTH * MICROPY_HW_LTDC_HEIGHT * fmt->bytes_per_pixel) {
            mp_raise_ValueError(MP_ERROR_TEXT("buffer too small"));
        }
    }

    // The shadow registers must not be written again while a reload is pending, so
    // a previous show(wait=False) must finish before the address can change again.
    ltdc_wait_reload();
    MP_STATE_PORT(ltdc_buf)[0] = MP_STATE_PORT(ltdc_buf)[1];
    MP_STATE_PORT(ltdc_buf)[1] = buf_in;

    if (buf_in == mp_const_none) {
        if (self->layer_enabled) {
            // Turn off the layer, so the display shows the black background.
            LTDC_Layer1->CR = 0;
            self->layer_enabled = false;
            ltdc_reload(self, wait);
        }
        return;
    }

    // The LTDC reads memory directly, so write back anything still in the cache.
    MP_HAL_CLEAN_DCACHE(bufinfo.buf, bufinfo.len);

    if (!self->layer_enabled) {
        LTDC_LayerCfgTypeDef cfg = {
            .WindowX0 = 0,
            .WindowX1 = MICROPY_HW_LTDC_WIDTH,
            .WindowY0 = 0,
            .WindowY1 = MICROPY_HW_LTDC_HEIGHT,
            .PixelFormat = fmt->ltdc_format,
            .Alpha = 255,
            .Alpha0 = 0,
            .BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA,
            .BlendingFactor2 = LTDC_BLENDING_FACTOR2_CA,
            .FBStartAdress = (uint32_t)bufinfo.buf,
            .ImageWidth = MICROPY_HW_LTDC_WIDTH,
            .ImageHeight = MICROPY_HW_LTDC_HEIGHT,
        };
        if (self->format == LTDC_FORMAT_GS8) {
            // GS8 is the LTDC's L8 format with a greyscale lookup table.
            for (uint32_t i = 0; i < 256; ++i) {
                LTDC_Layer1->CLUTWR = i << 24 | i << 16 | i << 8 | i;
            }
        }
        if (HAL_LTDC_ConfigLayer_NoReload(&self->hltdc, &cfg, LTDC_LAYER_1) != HAL_OK) {
            mp_raise_OSError(MP_EIO);
        }
        if (self->format == LTDC_FORMAT_GS8) {
            // Shadow registers read back their active value, so a read-modify-write
            // (as in HAL_LTDC_EnableCLUT_NoReload) would clear the pending LEN.
            LTDC_Layer1->CR = LTDC_LxCR_LEN | LTDC_LxCR_CLUTEN;
        }
        self->layer_enabled = true;
    } else {
        HAL_LTDC_SetAddress_NoReload(&self->hltdc, (uint32_t)bufinfo.buf, LTDC_LAYER_1);
    }
    ltdc_reload(self, wait);
}

/******************************************************************************/
// MicroPython bindings

static void stm_ltdc_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    mp_printf(print, "LTDC(width=%u, height=%u)", MICROPY_HW_LTDC_WIDTH, MICROPY_HW_LTDC_HEIGHT);
}

// LTDC(*, format=LTDC.RGB565)
static mp_obj_t stm_ltdc_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_format };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_format, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = LTDC_FORMAT_RGB565} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_int_t format = args[ARG_format].u_int;
    if (format < 0 || format >= LTDC_FORMAT_NUM) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid format"));
    }

    // There's one display, so this always returns the same object.
    stm_ltdc_obj_t *self = &stm_ltdc_obj;
    self->base.type = &stm_ltdc_type;
    ltdc_deinit();
    self->format = format;
    ltdc_init(self);
    return MP_OBJ_FROM_PTR(self);
}

// show(buf, *, wait=True)
static mp_obj_t stm_ltdc_show(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_buf, ARG_wait };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_buf, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_wait, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = true} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    stm_ltdc_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    if (!self->active) {
        mp_raise_OSError(MP_EPERM);
    }
    ltdc_show(self, args[ARG_buf].u_obj, args[ARG_wait].u_bool);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(stm_ltdc_show_obj, 2, stm_ltdc_show);

static mp_obj_t stm_ltdc_deinit(mp_obj_t self_in) {
    ltdc_deinit();
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(stm_ltdc_deinit_obj, stm_ltdc_deinit);

static void stm_ltdc_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    if (dest[0] != MP_OBJ_NULL) {
        return;
    }
    if (attr == MP_QSTR_width) {
        dest[0] = MP_OBJ_NEW_SMALL_INT(MICROPY_HW_LTDC_WIDTH);
    } else if (attr == MP_QSTR_height) {
        dest[0] = MP_OBJ_NEW_SMALL_INT(MICROPY_HW_LTDC_HEIGHT);
    } else {
        dest[1] = MP_OBJ_SENTINEL;
    }
}

static const mp_rom_map_elem_t stm_ltdc_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_show), MP_ROM_PTR(&stm_ltdc_show_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&stm_ltdc_deinit_obj) },

    { MP_ROM_QSTR(MP_QSTR_RGB565), MP_ROM_INT(LTDC_FORMAT_RGB565) },
    { MP_ROM_QSTR(MP_QSTR_RGB888), MP_ROM_INT(LTDC_FORMAT_RGB888) },
    { MP_ROM_QSTR(MP_QSTR_ARGB8888), MP_ROM_INT(LTDC_FORMAT_ARGB8888) },
    { MP_ROM_QSTR(MP_QSTR_GS8), MP_ROM_INT(LTDC_FORMAT_GS8) },
};
static MP_DEFINE_CONST_DICT(stm_ltdc_locals_dict, stm_ltdc_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    stm_ltdc_type,
    MP_QSTR_LTDC,
    MP_TYPE_FLAG_NONE,
    make_new, stm_ltdc_make_new,
    print, stm_ltdc_print,
    attr, stm_ltdc_attr,
    locals_dict, &stm_ltdc_locals_dict
    );

#endif // MICROPY_HW_ENABLE_LTDC
