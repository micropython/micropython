/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Qualcomm Innovation Center, Inc.
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

// Driver for the UNO Q's 13x8 charlieplexed LED matrix (104 LEDs on STM32 port
// F lines PF0..PF10).  A pixel is lit by driving one line high and one low while
// the rest float; TIM17 scans one pixel per interrupt so persistence of vision
// shows the whole frame.  The frame is a MONO_VLSB buffer that the ledmatrix
// module wraps in a framebuf.FrameBuffer.

#include "py/runtime.h"
#include "py/objarray.h"
#include "py/mphal.h"
#include "timer.h"
#include "irq.h"

#define MATRIX_WIDTH  (13)
#define MATRIX_HEIGHT (8)
#define MATRIX_PIXELS (MATRIX_WIDTH * MATRIX_HEIGHT)
#define MATRIX_REFRESH_HZ (100)

// Masks for PF0..PF10.
#define PIN_BIT_MASK (0x7ff)        // ODR/BSRR: one bit per pin
#define PIN_FIELD_MASK (0x3fffff)   // MODER/OSPEEDR/PUPDR: two bits per pin

// Each framebuffer pixel (row-major) to its {high, low} PF line indices, packed
// as (high << 8) | low.
static const uint16_t pixel_pairs[MATRIX_PIXELS] = {
    0x0001, 0x0100, 0x0002, 0x0200, 0x0102, 0x0201, 0x0003, 0x0300, 0x0103, 0x0301,
    0x0203, 0x0302, 0x0004, 0x0400, 0x0104, 0x0401, 0x0204, 0x0402, 0x0304, 0x0403,
    0x0005, 0x0500, 0x0105, 0x0501, 0x0205, 0x0502, 0x0305, 0x0503, 0x0405, 0x0504,
    0x0006, 0x0600, 0x0106, 0x0601, 0x0206, 0x0602, 0x0306, 0x0603, 0x0406, 0x0604,
    0x0506, 0x0605, 0x0007, 0x0700, 0x0107, 0x0701, 0x0207, 0x0702, 0x0307, 0x0703,
    0x0407, 0x0704, 0x0507, 0x0705, 0x0607, 0x0706, 0x0008, 0x0800, 0x0108, 0x0801,
    0x0208, 0x0802, 0x0308, 0x0803, 0x0408, 0x0804, 0x0508, 0x0805, 0x0608, 0x0806,
    0x0708, 0x0807, 0x0009, 0x0900, 0x0109, 0x0901, 0x0209, 0x0902, 0x0309, 0x0903,
    0x0409, 0x0904, 0x0509, 0x0905, 0x0609, 0x0906, 0x0709, 0x0907, 0x0809, 0x0908,
    0x000a, 0x0a00, 0x010a, 0x0a01, 0x020a, 0x0a02, 0x030a, 0x0a03, 0x040a, 0x0a04,
    0x050a, 0x0a05, 0x060a, 0x0a06,
};

static uint8_t matrix_fb[MATRIX_WIDTH];     // MONO_VLSB: one byte per column
static uint8_t matrix_pixel;                // scan position
static bool matrix_running;

// Advance one pixel: float every line, then if the pixel is set drive its high
// line high and its low line low.  ODR is loaded via BSRR before MODER switches
// the pair to output, so the pins never glitch.
static inline void matrix_scan_tick(void) {
    GPIOF->MODER &= ~PIN_FIELD_MASK;
    uint32_t x = matrix_pixel % MATRIX_WIDTH;
    uint32_t y = matrix_pixel / MATRIX_WIDTH;
    if (matrix_fb[x] & (1 << y)) {
        uint16_t pair = pixel_pairs[matrix_pixel];
        uint32_t hi = pair >> 8;
        uint32_t lo = pair & 0xff;
        GPIOF->BSRR = (1 << hi) | (1 << (lo + 16));
        GPIOF->MODER |= (1 << (2 * hi)) | (1 << (2 * lo));
    }
    if (++matrix_pixel >= MATRIX_PIXELS) {
        matrix_pixel = 0;
    }
}

void TIM17_IRQHandler(void) {
    if (TIM17->SR & TIM_SR_UIF) {
        TIM17->SR = ~TIM_SR_UIF;
        matrix_scan_tick();
    }
}

static void matrix_start(void) {
    if (matrix_running) {
        return;
    }

    // PF0..PF10: push-pull, very-high speed, no pull, starting floating (input).
    __HAL_RCC_GPIOF_CLK_ENABLE();
    GPIOF->OTYPER &= ~PIN_BIT_MASK;
    GPIOF->OSPEEDR |= PIN_FIELD_MASK;
    GPIOF->PUPDR &= ~PIN_FIELD_MASK;
    GPIOF->MODER &= ~PIN_FIELD_MASK;

    // TIM17 fires MATRIX_REFRESH_HZ * MATRIX_PIXELS times a second, one tick per
    // pixel.  Prescale to a 1MHz count so the period fits a 16-bit reload.
    __HAL_RCC_TIM17_CLK_ENABLE();
    uint32_t count_hz = 1000000;
    TIM17->PSC = (timer_get_source_freq(17) / count_hz) - 1;
    TIM17->ARR = (count_hz / (MATRIX_REFRESH_HZ * MATRIX_PIXELS)) - 1;
    TIM17->EGR = TIM_EGR_UG;     // load PSC/ARR
    TIM17->SR = 0;
    TIM17->DIER = TIM_DIER_UIE;
    TIM17->CR1 = TIM_CR1_CEN;

    HAL_NVIC_SetPriority(TIM17_IRQn, IRQ_PRI_TIMX, 0);
    HAL_NVIC_EnableIRQ(TIM17_IRQn);
    matrix_running = true;
}

static void matrix_stop(void) {
    if (!matrix_running) {
        return;
    }
    HAL_NVIC_DisableIRQ(TIM17_IRQn);
    TIM17->CR1 = 0;
    TIM17->DIER = 0;
    __HAL_RCC_TIM17_CLK_DISABLE();
    GPIOF->MODER &= ~PIN_FIELD_MASK;    // float all lines
    matrix_running = false;
}

static mp_obj_t ledmatrix_start(void) {
    matrix_start();
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_0(ledmatrix_start_obj, ledmatrix_start);

static mp_obj_t ledmatrix_stop(void) {
    matrix_stop();
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_0(ledmatrix_stop_obj, ledmatrix_stop);

// Writable view of the live scan buffer, for wrapping in a framebuf.FrameBuffer.
static mp_obj_t ledmatrix_buffer(void) {
    return mp_obj_new_memoryview('B' | MP_OBJ_ARRAY_TYPECODE_FLAG_RW, sizeof(matrix_fb), matrix_fb);
}
static MP_DEFINE_CONST_FUN_OBJ_0(ledmatrix_buffer_obj, ledmatrix_buffer);

static const mp_rom_map_elem_t ledmatrix_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR__ledmatrix) },
    { MP_ROM_QSTR(MP_QSTR_start), MP_ROM_PTR(&ledmatrix_start_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&ledmatrix_stop_obj) },
    { MP_ROM_QSTR(MP_QSTR_buffer), MP_ROM_PTR(&ledmatrix_buffer_obj) },
    { MP_ROM_QSTR(MP_QSTR_WIDTH), MP_ROM_INT(MATRIX_WIDTH) },
    { MP_ROM_QSTR(MP_QSTR_HEIGHT), MP_ROM_INT(MATRIX_HEIGHT) },
};
static MP_DEFINE_CONST_DICT(ledmatrix_module_globals, ledmatrix_module_globals_table);

const mp_obj_module_t ledmatrix_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&ledmatrix_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR__ledmatrix, ledmatrix_module);
