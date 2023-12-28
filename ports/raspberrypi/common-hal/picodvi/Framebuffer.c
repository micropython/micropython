/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Scott Shawcroft for Adafruit Industries
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

#include "bindings/picodvi/Framebuffer.h"

#include "py/gc.h"
#include "py/runtime.h"
#include "shared-bindings/time/__init__.h"
#include "common-hal/pwmio/PWMOut.h"
#include "common-hal/rp2pio/StateMachine.h"
#include "supervisor/port.h"

#include "src/common/pico_stdlib/include/pico/stdlib.h"
#include "src/rp2040/hardware_structs/include/hardware/structs/mpu.h"
#include "src/rp2_common/cmsis/stub/CMSIS/Device/RaspberryPi/RP2040/Include/RP2040.h"
#include "src/rp2_common/hardware_pwm/include/hardware/pwm.h"
#include "src/rp2_common/hardware_vreg/include/hardware/vreg.h"
#include "src/rp2_common/pico_multicore/include/pico/multicore.h"

#include "lib/PicoDVI/software/libdvi/tmds_encode.h"

picodvi_framebuffer_obj_t *active_picodvi = NULL;

STATIC PIO pio_instances[2] = {pio0, pio1};

static void __not_in_flash_func(core1_main)(void) {
    // The MPU is reset before this starts.

    picodvi_framebuffer_obj_t *self = active_picodvi;
    dvi_register_irqs_this_core(&self->dvi, DMA_IRQ_1);

    while (queue_is_empty(&self->dvi.q_colour_valid)) {
        __wfe();
    }
    dvi_start(&self->dvi);

    // Turn off flash access. After this, it will hard fault. Better than messing
    // up CIRCUITPY.
    MPU->CTRL = MPU_CTRL_PRIVDEFENA_Msk | MPU_CTRL_ENABLE_Msk;
    MPU->RNR = 6; // 7 is used by pico-sdk stack protection.
    MPU->RBAR = XIP_MAIN_BASE | MPU_RBAR_VALID_Msk;
    MPU->RASR = MPU_RASR_XN_Msk | // Set execute never and everything else is restricted.
        MPU_RASR_ENABLE_Msk |
        (0x1b << MPU_RASR_SIZE_Pos);         // Size is 0x10000000 which masks up to SRAM region.
    MPU->RNR = 7;

    uint y = 0;
    while (1) {
        uint32_t *scanbuf;
        queue_remove_blocking_u32(&self->dvi.q_colour_valid, &scanbuf);

        uint32_t *tmdsbuf;
        queue_remove_blocking_u32(&self->dvi.q_tmds_free, &tmdsbuf);
        // Check to see if the tmds memory has moved and replace this tmdsbuf
        // the corresponding on at a new location.
        size_t old_fb = tmdsbuf[self->tmdsbuf_size - 1];
        if (old_fb != (uint32_t)self->framebuffer) {
            size_t index = ((uint32_t)(tmdsbuf - old_fb)) / self->tmdsbuf_size;
            // Check our index and hang if it is out of range. Hang is ok since this is core 1.
            // Better than writing the wrong memory that is shared with CP.
            while (index >= DVI_N_TMDS_BUFFERS) {
            }
            tmdsbuf = self->framebuffer + self->framebuffer_len + (self->tmdsbuf_size * index);
            tmdsbuf[self->tmdsbuf_size - 1] = (uint32_t)self->framebuffer;
        }
        uint pixwidth = self->dvi.timing->h_active_pixels;
        uint words_per_channel = pixwidth / DVI_SYMBOLS_PER_WORD;
        if (self->color_depth == 8) {
            tmds_encode_data_channel_8bpp(scanbuf, tmdsbuf + 0 * words_per_channel, pixwidth / 2, DVI_8BPP_BLUE_MSB,  DVI_8BPP_BLUE_LSB);
            tmds_encode_data_channel_8bpp(scanbuf, tmdsbuf + 1 * words_per_channel, pixwidth / 2, DVI_8BPP_GREEN_MSB, DVI_8BPP_GREEN_LSB);
            tmds_encode_data_channel_8bpp(scanbuf, tmdsbuf + 2 * words_per_channel, pixwidth / 2, DVI_8BPP_RED_MSB,   DVI_8BPP_RED_LSB);
        } else if (self->color_depth == 16) {
            tmds_encode_data_channel_16bpp(scanbuf, tmdsbuf + 0 * words_per_channel, pixwidth / 2, DVI_16BPP_BLUE_MSB,  DVI_16BPP_BLUE_LSB);
            tmds_encode_data_channel_16bpp(scanbuf, tmdsbuf + 1 * words_per_channel, pixwidth / 2, DVI_16BPP_GREEN_MSB, DVI_16BPP_GREEN_LSB);
            tmds_encode_data_channel_16bpp(scanbuf, tmdsbuf + 2 * words_per_channel, pixwidth / 2, DVI_16BPP_RED_MSB,   DVI_16BPP_RED_LSB);
        } else if (self->color_depth == 1) {
            tmds_encode_1bpp(scanbuf, tmdsbuf, pixwidth);
        } else if (self->color_depth == 2) {
            tmds_encode_2bpp(scanbuf, tmdsbuf, pixwidth);
        }
        queue_add_blocking_u32(&self->dvi.q_tmds_valid, &tmdsbuf);

        queue_add_blocking_u32(&self->dvi.q_colour_free, &scanbuf);
        ++y;
        if (y == self->dvi.timing->v_active_lines) {
            y = 0;
        }
    }
    __builtin_unreachable();
}

static void __not_in_flash_func(core1_scanline_callback)(void) {
    picodvi_framebuffer_obj_t *self = active_picodvi;
    uint32_t *next_scanline_buf;
    next_scanline_buf = self->framebuffer + (self->pitch * self->next_scanline);
    queue_add_blocking_u32(&self->dvi.q_colour_valid, &next_scanline_buf);

    // Remove any buffers that were sent back to us.
    while (queue_try_remove_u32(&self->dvi.q_colour_free, &next_scanline_buf)) {
    }
    self->next_scanline += 1;
    if (self->next_scanline >= self->height) {
        self->next_scanline = 0;
    }
}

extern uint8_t dvi_vertical_repeat;
extern bool dvi_monochrome_tmds;

void common_hal_picodvi_framebuffer_construct(picodvi_framebuffer_obj_t *self,
    mp_uint_t width, mp_uint_t height,
    const mcu_pin_obj_t *clk_dp, const mcu_pin_obj_t *clk_dn,
    const mcu_pin_obj_t *red_dp, const mcu_pin_obj_t *red_dn,
    const mcu_pin_obj_t *green_dp, const mcu_pin_obj_t *green_dn,
    const mcu_pin_obj_t *blue_dp, const mcu_pin_obj_t *blue_dn,
    mp_uint_t color_depth) {
    if (active_picodvi != NULL) {
        mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("%q in use"), MP_QSTR_picodvi);
    }

    bool color_framebuffer = color_depth >= 8;
    const struct dvi_timing *timing = NULL;
    if ((width == 640 && height == 480) ||
        (width == 320 && height == 240)) {
        timing = &dvi_timing_640x480p_60hz;
    } else if ((width == 800 && height == 480) ||
               (width == 400 && height == 240)) {
        timing = &dvi_timing_800x480p_60hz;
    } else {
        if (height != 480 && height != 240) {
            mp_raise_ValueError_varg(MP_ERROR_TEXT("Invalid %q"), MP_QSTR_height);
        }
        mp_raise_ValueError_varg(MP_ERROR_TEXT("Invalid %q"), MP_QSTR_width);
    }

    // If the width is > 400, then it must not be color frame buffer and vice
    // versa.
    if ((width > 400) == color_framebuffer) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("Invalid %q"), MP_QSTR_color_depth);
    }

    bool invert_diffpairs = clk_dn->number < clk_dp->number;
    int8_t other_pins[4];
    int8_t *a;
    int8_t *b;
    if (invert_diffpairs) {
        a = other_pins;
        b = self->pin_pair;
    } else {
        a = self->pin_pair;
        b = other_pins;
    }
    a[0] = clk_dp->number;
    a[1] = red_dp->number;
    a[2] = green_dp->number;
    a[3] = blue_dp->number;
    b[0] = clk_dn->number;
    b[1] = red_dn->number;
    b[2] = green_dn->number;
    b[3] = blue_dn->number;
    qstr pin_names[4] = {MP_QSTR_clk_dp, MP_QSTR_red_dp, MP_QSTR_green_dp, MP_QSTR_blue_dp};
    for (size_t i = 0; i < 4; i++) {
        if (other_pins[i] - self->pin_pair[i] != 1) {
            raise_ValueError_invalid_pin_name(pin_names[i]);
        }
    }

    uint8_t slice = pwm_gpio_to_slice_num(self->pin_pair[0]);


    pio_program_t program_struct = {
        .instructions = NULL,
        .length = 2,
        .origin = -1
    };
    size_t pio_index = NUM_PIOS;
    int free_state_machines[4]; // We may find all four free. We only use the first three.
    for (size_t i = 0; i < NUM_PIOS; i++) {
        PIO pio = pio_instances[i];
        uint8_t free_count = 0;
        for (size_t sm = 0; sm < NUM_PIO_STATE_MACHINES; sm++) {
            if (!pio_sm_is_claimed(pio, sm)) {
                free_state_machines[free_count] = sm;
                free_count++;
            }
        }
        if (free_count >= 3 && pio_can_add_program(pio, &program_struct)) {
            pio_index = i;
            break;
        }
    }

    if (pio_index == NUM_PIOS) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("All state machines in use"));
    }

    self->width = width;
    self->height = height;

    size_t tmds_bufs_per_scanline;
    size_t scanline_width = width;
    if (color_framebuffer) {
        dvi_vertical_repeat = 2;
        dvi_monochrome_tmds = false;
        tmds_bufs_per_scanline = 3;
        scanline_width *= 2;
    } else {
        dvi_vertical_repeat = 1;
        dvi_monochrome_tmds = true;
        // One tmds buffer is used for all three color outputs.
        tmds_bufs_per_scanline = 1;
    }
    self->pitch = (self->width * color_depth) / 8;
    // Align each row to words.
    if (self->pitch % sizeof(uint32_t) != 0) {
        self->pitch += sizeof(uint32_t) - (self->pitch % sizeof(uint32_t));
    }
    self->pitch /= sizeof(uint32_t);
    size_t framebuffer_size = self->pitch * self->height;
    self->tmdsbuf_size = tmds_bufs_per_scanline * scanline_width / DVI_SYMBOLS_PER_WORD + 1;
    size_t total_allocation_size = sizeof(uint32_t) * (framebuffer_size + DVI_N_TMDS_BUFFERS * self->tmdsbuf_size);
    self->framebuffer = (uint32_t *)port_malloc(total_allocation_size, true);
    if (self->framebuffer == NULL) {
        m_malloc_fail(total_allocation_size);
        return;
    }

    // Do the pwmio check last because it claims the pwm slice.
    if (!pwmio_claim_slice_ab_channels(slice)) {
        mp_raise_ValueError(MP_ERROR_TEXT("All timers for this pin are in use"));
    }
    self->pwm_slice = slice;

    pwmout_never_reset(self->pwm_slice, 0);
    pwmout_never_reset(self->pwm_slice, 1);

    for (size_t i = 0; i < 4; i++) {
        never_reset_pin_number(self->pin_pair[i]);
        never_reset_pin_number(self->pin_pair[i] + 1);
    }

    for (size_t i = 0; i < 3; i++) {
        rp2pio_statemachine_never_reset(pio_instances[pio_index], free_state_machines[i]);
    }

    // For the output.
    user_irq_claim(DMA_IRQ_1);
    self->framebuffer_len = framebuffer_size;
    self->color_depth = color_depth;

    self->dvi.timing = timing;
    self->dvi.ser_cfg.pio = pio_instances[pio_index];
    self->dvi.ser_cfg.sm_tmds[0] = free_state_machines[0];
    self->dvi.ser_cfg.sm_tmds[1] = free_state_machines[1];
    self->dvi.ser_cfg.sm_tmds[2] = free_state_machines[2];
    self->dvi.ser_cfg.pins_clk = self->pin_pair[0];
    self->dvi.ser_cfg.pins_tmds[0] = self->pin_pair[1];
    self->dvi.ser_cfg.pins_tmds[1] = self->pin_pair[2];
    self->dvi.ser_cfg.pins_tmds[2] = self->pin_pair[3];
    self->dvi.ser_cfg.invert_diffpairs = invert_diffpairs;
    self->dvi.scanline_callback = core1_scanline_callback;

    vreg_set_voltage(VREG_VOLTAGE_1_20);
    common_hal_time_delay_ms(10);
    set_sys_clock_khz(timing->bit_clk_khz, true); // Run at TMDS bit clock
    self->tmds_lock = next_striped_spin_lock_num();
    self->colour_lock = next_striped_spin_lock_num();
    dvi_init(&self->dvi, self->tmds_lock, self->colour_lock);

    // Load up the TMDS buffers.
    for (int i = 0; i < DVI_N_TMDS_BUFFERS; ++i) {
        uint32_t *tmdsbuf = self->framebuffer + (self->framebuffer_len + self->tmdsbuf_size * i);
        // Use the last word in the buffer to track its original root. That way
        // we can detect when framebuffer is moved.
        tmdsbuf[self->tmdsbuf_size - 1] = (uint32_t)self->framebuffer;
        queue_add_blocking_u32(&self->dvi.q_tmds_free, &tmdsbuf);
    }

    active_picodvi = self;

    // Core 1 will wait until it sees the first colour buffer, then start up the
    // DVI signalling.
    multicore_launch_core1(core1_main);

    self->next_scanline = 0;
    uint32_t *next_scanline_buf = self->framebuffer + (self->pitch * self->next_scanline);
    queue_add_blocking_u32(&self->dvi.q_colour_valid, &next_scanline_buf);
    self->next_scanline += 1;
    next_scanline_buf = self->framebuffer + (self->pitch * self->next_scanline);
    queue_add_blocking_u32(&self->dvi.q_colour_valid, &next_scanline_buf);
    self->next_scanline += 1;

    // Wait for the second core to run dvi_start because it is in flash. Once it is done,
    // it'll pull from this queue. Not waiting may lead to us reading flash when this core
    // doesn't want us to.
    while (queue_get_level(&self->dvi.q_colour_valid) == 2) {
    }
}

STATIC void _turn_off_dma(uint8_t channel) {
    dma_channel_config c = dma_channel_get_default_config(channel);
    channel_config_set_enable(&c, false);
    dma_channel_set_config(channel, &c, false /* trigger */);

    if (dma_channel_is_busy(channel)) {
        dma_channel_abort(channel);
    }
    dma_channel_set_irq1_enabled(channel, false);
    dma_channel_unclaim(channel);
}

void common_hal_picodvi_framebuffer_deinit(picodvi_framebuffer_obj_t *self) {
    if (common_hal_picodvi_framebuffer_deinited(self)) {
        return;
    }
    // Stop the other core and free resources.

    // Grab the locks before shutting down the other core so we don't leave the
    // locks locked.
    spin_lock_t *tmds_lock = spin_lock_instance(self->tmds_lock);
    spin_lock_t *colour_lock = spin_lock_instance(self->colour_lock);
    uint32_t tmds_save = spin_lock_blocking(tmds_lock);
    uint32_t colour_save = spin_lock_blocking(colour_lock);
    multicore_reset_core1();
    spin_unlock(colour_lock, colour_save);
    spin_unlock(tmds_lock, tmds_save);

    for (size_t i = 0; i < 4; i++) {
        reset_pin_number(self->pin_pair[i]);
        reset_pin_number(self->pin_pair[i] + 1);
    }

    for (int i = 0; i < N_TMDS_LANES; ++i) {
        // Turn off data first because it chains to the ctrl DMA.
        _turn_off_dma(self->dvi.dma_cfg[i].chan_data);
        _turn_off_dma(self->dvi.dma_cfg[i].chan_ctrl);
    }

    pwm_set_enabled(self->pwm_slice, false);
    pwmout_free(self->pwm_slice, 0);
    pwmout_free(self->pwm_slice, 1);

    pio_program_t program_struct = {
        .length = 2
    };
    PIO pio = self->dvi.ser_cfg.pio;
    for (size_t i = 0; i < 3; i++) {
        int sm = self->dvi.ser_cfg.sm_tmds[i];
        pio_sm_set_enabled(pio, sm, false);
        pio_sm_unclaim(pio, sm);
        rp2pio_statemachine_reset_ok(pio, sm);
    }
    pio_remove_program(pio, &program_struct, self->dvi.ser_cfg.prog_offs);

    if (user_irq_is_claimed(DMA_IRQ_1)) {
        user_irq_unclaim(DMA_IRQ_1);
    }

    active_picodvi = NULL;

    port_free(self->framebuffer);
    self->framebuffer = NULL;

    self->base.type = &mp_type_NoneType;
}

bool common_hal_picodvi_framebuffer_deinited(picodvi_framebuffer_obj_t *self) {
    return self->framebuffer == NULL;
}

void common_hal_picodvi_framebuffer_refresh(picodvi_framebuffer_obj_t *self) {
}

int common_hal_picodvi_framebuffer_get_width(picodvi_framebuffer_obj_t *self) {
    return self->width;
}

int common_hal_picodvi_framebuffer_get_height(picodvi_framebuffer_obj_t *self) {
    return self->height;
}

int common_hal_picodvi_framebuffer_get_color_depth(picodvi_framebuffer_obj_t *self) {
    return self->color_depth;
}

mp_int_t common_hal_picodvi_framebuffer_get_buffer(mp_obj_t self_in, mp_buffer_info_t *bufinfo, mp_uint_t flags) {
    picodvi_framebuffer_obj_t *self = (picodvi_framebuffer_obj_t *)self_in;
    bufinfo->buf = self->framebuffer;
    char typecode = 'B';
    if (self->color_depth == 16) {
        typecode = 'H';
    }
    bufinfo->typecode = typecode;
    bufinfo->len = self->framebuffer_len * sizeof(uint32_t);
    return 0;
}

int common_hal_picodvi_framebuffer_get_row_stride(picodvi_framebuffer_obj_t *self) {
    // Pitch is in words but row stride is expected as bytes.
    return self->pitch * sizeof(uint32_t);
}
