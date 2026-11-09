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
#include "supervisor/background_callback.h"
#include "supervisor/port.h"

#include "pico/stdlib.h"

// This is from: https://github.com/raspberrypi/pico-examples-rp2350/blob/a1/hstx/dvi_out_hstx_encoder/dvi_out_hstx_encoder.c

#include "hardware/dma.h"
#include "hardware/structs/bus_ctrl.h"
#include "hardware/structs/hstx_ctrl.h"
#include "hardware/structs/hstx_fifo.h"

// ----------------------------------------------------------------------------
// HSTX command lists

#define VSYNC_LEN 6
#define VACTIVE_LEN 9

static uint32_t vblank_line640_vsync_off[VSYNC_LEN] = {
    HSTX_CMD_RAW_REPEAT | MODE_640_H_FRONT_PORCH,
    SYNC_V1_H1,
    HSTX_CMD_RAW_REPEAT | MODE_640_H_SYNC_WIDTH,
    SYNC_V1_H0,
    HSTX_CMD_RAW_REPEAT | (MODE_640_H_BACK_PORCH + MODE_640_H_ACTIVE_PIXELS),
    SYNC_V1_H1
};

static uint32_t vblank_line640_vsync_on[VSYNC_LEN] = {
    HSTX_CMD_RAW_REPEAT | MODE_640_H_FRONT_PORCH,
    SYNC_V0_H1,
    HSTX_CMD_RAW_REPEAT | MODE_640_H_SYNC_WIDTH,
    SYNC_V0_H0,
    HSTX_CMD_RAW_REPEAT | (MODE_640_H_BACK_PORCH + MODE_640_H_ACTIVE_PIXELS),
    SYNC_V0_H1
};

static uint32_t vactive_line640[VACTIVE_LEN] = {
    HSTX_CMD_RAW_REPEAT | MODE_640_H_FRONT_PORCH,
    SYNC_V1_H1,
    HSTX_CMD_NOP,
    HSTX_CMD_RAW_REPEAT | MODE_640_H_SYNC_WIDTH,
    SYNC_V1_H0,
    HSTX_CMD_NOP,
    HSTX_CMD_RAW_REPEAT | MODE_640_H_BACK_PORCH,
    SYNC_V1_H1,
    HSTX_CMD_TMDS | MODE_640_H_ACTIVE_PIXELS
};

static uint32_t vblank_line720_vsync_off[VSYNC_LEN] = {
    HSTX_CMD_RAW_REPEAT | MODE_720_H_FRONT_PORCH,
    SYNC_V1_H1,
    HSTX_CMD_RAW_REPEAT | MODE_720_H_SYNC_WIDTH,
    SYNC_V1_H0,
    HSTX_CMD_RAW_REPEAT | (MODE_720_H_BACK_PORCH + MODE_720_H_ACTIVE_PIXELS),
    SYNC_V1_H1
};

static uint32_t vblank_line720_vsync_on[VSYNC_LEN] = {
    HSTX_CMD_RAW_REPEAT | MODE_720_H_FRONT_PORCH,
    SYNC_V0_H1,
    HSTX_CMD_RAW_REPEAT | MODE_720_H_SYNC_WIDTH,
    SYNC_V0_H0,
    HSTX_CMD_RAW_REPEAT | (MODE_720_H_BACK_PORCH + MODE_720_H_ACTIVE_PIXELS),
    SYNC_V0_H1
};

static uint32_t vactive_line720[VACTIVE_LEN] = {
    HSTX_CMD_RAW_REPEAT | MODE_720_H_FRONT_PORCH,
    SYNC_V1_H1,
    HSTX_CMD_NOP,
    HSTX_CMD_RAW_REPEAT | MODE_720_H_SYNC_WIDTH,
    SYNC_V1_H0,
    HSTX_CMD_NOP,
    HSTX_CMD_RAW_REPEAT | MODE_720_H_BACK_PORCH,
    SYNC_V1_H1,
    HSTX_CMD_TMDS | MODE_720_H_ACTIVE_PIXELS
};

picodvi_framebuffer_obj_t *active_picodvi = NULL;

#if CIRCUITPY_PICODVI_AUDIOOUT
#include "common-hal/picodvi/AudioOut.h"
#else
static inline uint32_t *picodvi_audioout_next_frame(void) {
    return active_picodvi->dma_commands;
}
static inline void picodvi_audioout_frame_done(void) {
}
static inline void picodvi_audioout_framebuffer_deinit(picodvi_framebuffer_obj_t *self) {
}
#endif

// Incremented once per completed frame by the DMA frame-boundary IRQ, so refresh() can block
// until the next frame starts (vblank sync). A full-framebuffer repaint that begins right after a
// frame boundary then races ahead of scanout, avoiding tearing without a second framebuffer.
static volatile uint32_t framebuffer_frame_count = 0;

static void __not_in_flash_func(dma_irq_handler)(void) {
    if (active_picodvi == NULL) {
        return;
    }
    uint ch_num = active_picodvi->dma_pixel_channel;
    dma_hw->intr = 1u << ch_num;

    // Set the read_addr back to the start and trigger the first transfer (which
    // will trigger the pixel channel).
    dma_channel_hw_t *ch = &dma_hw->ch[active_picodvi->dma_command_channel];
    ch->al3_read_addr_trig = (uintptr_t)picodvi_audioout_next_frame();
    framebuffer_frame_count++;    // frame boundary: scanout has wrapped back to the top
    picodvi_audioout_frame_done();
}

bool common_hal_picodvi_framebuffer_preflight(
    mp_uint_t width, mp_uint_t height,
    mp_uint_t color_depth) {

    // These modes don't duplicate pixels so we can do sub-byte colors. They
    // take too much ram for more than 8bit color though.
    bool full_resolution = color_depth == 1 || color_depth == 2 || color_depth == 4 || color_depth == 8;
    // These modes rely on the memory transfer to duplicate values across bytes.
    bool doubled = color_depth == 8 || color_depth == 16 || color_depth == 32;

    // for each supported resolution, check the color depth is supported
    if (width == 640 && height == 480) {
        return full_resolution;
    }
    if (width == 320 && height == 240) {
        return doubled;
    }
    if (width == 160 && height == 120) {
        return doubled;
    }

    if (width == 720 && height == 400) {
        return full_resolution;
    }

    if (width == 360 && height == 200) {
        return doubled;
    }

    if (width == 180 && height == 100) {
        return doubled;
    }
    return false;
}

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

    if (!common_hal_picodvi_framebuffer_preflight(width, height, color_depth)) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("Invalid %q and %q"), MP_QSTR_width, MP_QSTR_height);
    }

    self->dvi_audio = NULL;
    self->audioout = MP_OBJ_NULL;
    self->dma_command_channel = -1;
    self->dma_pixel_channel = -1;

    if (width % 160 == 0) {
        self->output_width = 640;
    } else {
        self->output_width = 720;
    }
    size_t output_scaling = self->output_width / width;

    size_t all_allocated = 0;
    int8_t pins[8] = {
        clk_dp->number, clk_dn->number,
        red_dp->number, red_dn->number,
        green_dp->number, green_dn->number,
        blue_dp->number, blue_dn->number
    };
    qstr pin_names[8] = {
        MP_QSTR_clk_dp, MP_QSTR_clk_dn,
        MP_QSTR_red_dp, MP_QSTR_red_dn,
        MP_QSTR_green_dp, MP_QSTR_green_dn,
        MP_QSTR_blue_dp, MP_QSTR_blue_dn
    };
    for (size_t i = 0; i < 8; i++) {
        if (!(12 <= pins[i] && pins[i] <= 19)) {
            raise_ValueError_invalid_pin_name(pin_names[i]);
        }
        pins[i] -= 12;
        size_t mask = 1 << pins[i];
        if ((all_allocated & mask) != 0) {
            raise_ValueError_invalid_pin_name(pin_names[i]);
        }
        all_allocated |= mask;
    }

    self->width = width;
    self->height = height;
    self->color_depth = color_depth;
    // Pitch is number of 32-bit words per line. We round up pitch_bytes to the nearest word
    // so that each scanline begins on a natural 32-bit word boundary.
    size_t pitch_bytes = (self->width * color_depth) / 8;
    self->pitch = (pitch_bytes + sizeof(uint32_t) - 1) / sizeof(uint32_t);
    size_t framebuffer_size = self->pitch * self->height;

    // We check that allocations aren't in PSRAM because we haven't added XIP
    // streaming support.
    self->framebuffer = (uint32_t *)port_malloc(framebuffer_size * sizeof(uint32_t), true);
    if (self->framebuffer == NULL || ((size_t)self->framebuffer & 0xf0000000) == 0x10000000) {
        common_hal_picodvi_framebuffer_deinit(self);
        m_malloc_fail(framebuffer_size * sizeof(uint32_t));
        return;
    }
    memset(self->framebuffer, 0, framebuffer_size * sizeof(uint32_t));

    // We compute all DMA transfers needed for a single frame. This ensure we don't have any super
    // quick interrupts that we need to respond to. Each transfer takes two words, trans_count and
    // read_addr. Active pixel lines need two transfers due to different read addresses. When pixel
    // doubling, then we must also set transfer size.
    size_t dma_command_size = 2;
    if (output_scaling > 1) {
        dma_command_size = 4;
    }

    if (self->output_width == 640) {
        self->dma_commands_len = (MODE_640_V_FRONT_PORCH + MODE_640_V_SYNC_WIDTH + MODE_640_V_BACK_PORCH + 2 * MODE_640_V_ACTIVE_LINES + 1) * dma_command_size;
    } else {
        self->dma_commands_len = (MODE_720_V_FRONT_PORCH + MODE_720_V_SYNC_WIDTH + MODE_720_V_BACK_PORCH + 2 * MODE_720_V_ACTIVE_LINES + 1) * dma_command_size;
    }
    self->dma_commands = (uint32_t *)port_malloc(self->dma_commands_len * sizeof(uint32_t), true);
    if (self->dma_commands == NULL || ((size_t)self->dma_commands & 0xf0000000) == 0x10000000) {
        common_hal_picodvi_framebuffer_deinit(self);
        m_malloc_fail(self->dma_commands_len * sizeof(uint32_t));
        return;
    }

    // The command channel and the pixel channel form a pipeline that feeds combined HSTX
    // commands and pixel data to the HSTX FIFO. The command channel reads a pre-computed
    // list of control/status words from the dma_commands buffer and writes them to the
    // pixel channel's control/status registers. Under control of the command channel, the
    // pixel channel smears/swizzles pixel data from the framebuffer and combines
    // it with HSTX commands, forwarding the combined stream to the HSTX FIFO.

    self->dma_pixel_channel = dma_claim_unused_channel(false);
    self->dma_command_channel = dma_claim_unused_channel(false);
    if (self->dma_pixel_channel < 0 || self->dma_command_channel < 0) {
        common_hal_picodvi_framebuffer_deinit(self);
        mp_raise_RuntimeError(MP_ERROR_TEXT("Internal resource(s) in use"));
        return;
    }

    size_t command_word = 0;
    size_t frontporch_start;
    if (self->output_width == 640) {
        frontporch_start = MODE_640_V_TOTAL_LINES - MODE_640_V_FRONT_PORCH;
    } else {
        frontporch_start = MODE_720_V_TOTAL_LINES - MODE_720_V_FRONT_PORCH;
    }
    size_t frontporch_end = frontporch_start;
    if (self->output_width == 640) {
        frontporch_end += MODE_640_V_FRONT_PORCH;
    } else {
        frontporch_end += MODE_720_V_FRONT_PORCH;
    }
    size_t vsync_start = 0;
    size_t vsync_end = vsync_start;
    if (self->output_width == 640) {
        vsync_end += MODE_640_V_SYNC_WIDTH;
    } else {
        vsync_end += MODE_720_V_SYNC_WIDTH;
    }
    size_t backporch_start = vsync_end;
    size_t backporch_end = backporch_start;
    if (self->output_width == 640) {
        backporch_end += MODE_640_V_BACK_PORCH;
    } else {
        backporch_end += MODE_720_V_BACK_PORCH;
    }
    size_t active_start = backporch_end;

    uint32_t dma_ctrl = self->dma_command_channel << DMA_CH0_CTRL_TRIG_CHAIN_TO_LSB |
        DREQ_HSTX << DMA_CH0_CTRL_TRIG_TREQ_SEL_LSB |
        DMA_CH0_CTRL_TRIG_IRQ_QUIET_BITS |
        DMA_CH0_CTRL_TRIG_INCR_READ_BITS |
        DMA_CH0_CTRL_TRIG_EN_BITS;
    uint32_t dma_pixel_ctrl;
    if (output_scaling > 1) {
        // We do color_depth size transfers when pixel doubling. The memory bus will
        // duplicate the bytes read to produce 32 bits for the HSTX.
        if (color_depth == 32) {
            dma_pixel_ctrl = dma_ctrl | DMA_SIZE_32 << DMA_CH0_CTRL_TRIG_DATA_SIZE_LSB;
        } else if (color_depth == 16) {
            dma_pixel_ctrl = dma_ctrl | DMA_SIZE_16 << DMA_CH0_CTRL_TRIG_DATA_SIZE_LSB;
        } else {
            dma_pixel_ctrl = dma_ctrl | DMA_SIZE_8 << DMA_CH0_CTRL_TRIG_DATA_SIZE_LSB;
        }
    } else {
        dma_pixel_ctrl = dma_ctrl | DMA_SIZE_32 << DMA_CH0_CTRL_TRIG_DATA_SIZE_LSB;
    }
    if (self->color_depth == 16) {
        dma_pixel_ctrl |= DMA_CH0_CTRL_TRIG_BSWAP_BITS;
    }
    dma_ctrl |= DMA_SIZE_32 << DMA_CH0_CTRL_TRIG_DATA_SIZE_LSB;

    uint32_t dma_write_addr = (uint32_t)&hstx_fifo_hw->fifo;
    // Write ctrl and write_addr once when not pixel doubling because they don't
    // change. (write_addr doesn't change when pixel doubling either but we need
    // to rewrite it because it is after the ctrl register.)
    if (output_scaling == 1) {
        dma_channel_hw_addr(self->dma_pixel_channel)->al1_ctrl = dma_ctrl;
        dma_channel_hw_addr(self->dma_pixel_channel)->al1_write_addr = dma_write_addr;
    }

    uint32_t *vblank_line_vsync_on = self->output_width == 640 ?  vblank_line640_vsync_on : vblank_line720_vsync_on;
    uint32_t *vblank_line_vsync_off = self->output_width == 640 ?  vblank_line640_vsync_off : vblank_line720_vsync_off;
    uint32_t *vactive_line = self->output_width == 640 ?  vactive_line640 : vactive_line720;

    size_t mode_v_total_lines;
    if (self->output_width == 640) {
        mode_v_total_lines = MODE_640_V_TOTAL_LINES;
    } else {
        mode_v_total_lines = MODE_720_V_TOTAL_LINES;
    }

    for (size_t v_scanline = 0; v_scanline < mode_v_total_lines; v_scanline++) {
        if (output_scaling > 1) {
            self->dma_commands[command_word++] = dma_ctrl;
            self->dma_commands[command_word++] = dma_write_addr;
        }
        if (vsync_start <= v_scanline && v_scanline < vsync_end) {
            self->dma_commands[command_word++] = VSYNC_LEN;
            self->dma_commands[command_word++] = (uintptr_t)vblank_line_vsync_on;
        } else if (backporch_start <= v_scanline && v_scanline < backporch_end) {
            self->dma_commands[command_word++] = VSYNC_LEN;
            self->dma_commands[command_word++] = (uintptr_t)vblank_line_vsync_off;
        } else if (frontporch_start <= v_scanline && v_scanline < frontporch_end) {
            self->dma_commands[command_word++] = VSYNC_LEN;
            self->dma_commands[command_word++] = (uintptr_t)vblank_line_vsync_off;
        } else {
            self->dma_commands[command_word++] = VACTIVE_LEN;
            self->dma_commands[command_word++] = (uintptr_t)vactive_line;
            size_t row = v_scanline - active_start;
            size_t transfer_count = self->pitch;
            if (output_scaling > 1) {
                self->dma_commands[command_word++] = dma_pixel_ctrl;
                self->dma_commands[command_word++] = dma_write_addr;
                row /= output_scaling;
                // When pixel scaling, we do one transfer per pixel and it gets
                // mirrored into the rest of the word.
                transfer_count = self->width;
            }
            self->dma_commands[command_word++] = transfer_count;
            uint32_t *row_start = &self->framebuffer[row * self->pitch];
            self->dma_commands[command_word++] = (uintptr_t)row_start;
        }
    }
    // Last command is NULL which will trigger an IRQ.
    if (output_scaling > 1) {
        self->dma_commands[command_word++] = DMA_CH0_CTRL_TRIG_IRQ_QUIET_BITS |
            DMA_CH0_CTRL_TRIG_EN_BITS;
        self->dma_commands[command_word++] = 0;
    }
    self->dma_commands[command_word++] = 0;
    self->dma_commands[command_word++] = 0;

    if (color_depth == 32) {
        // Configure HSTX's TMDS encoder for RGB888
        hstx_ctrl_hw->expand_tmds =
            7 << HSTX_CTRL_EXPAND_TMDS_L2_NBITS_LSB |
                16 << HSTX_CTRL_EXPAND_TMDS_L2_ROT_LSB |
                7 << HSTX_CTRL_EXPAND_TMDS_L1_NBITS_LSB |
                8 << HSTX_CTRL_EXPAND_TMDS_L1_ROT_LSB |
                7 << HSTX_CTRL_EXPAND_TMDS_L0_NBITS_LSB |
                0 << HSTX_CTRL_EXPAND_TMDS_L0_ROT_LSB;
    } else if (color_depth == 16) {
        // Configure HSTX's TMDS encoder for RGB565
        hstx_ctrl_hw->expand_tmds =
            4 << HSTX_CTRL_EXPAND_TMDS_L2_NBITS_LSB |
                0 << HSTX_CTRL_EXPAND_TMDS_L2_ROT_LSB |
                5 << HSTX_CTRL_EXPAND_TMDS_L1_NBITS_LSB |
                27 << HSTX_CTRL_EXPAND_TMDS_L1_ROT_LSB |
                4 << HSTX_CTRL_EXPAND_TMDS_L0_NBITS_LSB |
                21 << HSTX_CTRL_EXPAND_TMDS_L0_ROT_LSB;
    } else if (color_depth == 8) {
        // Configure HSTX's TMDS encoder for RGB332
        hstx_ctrl_hw->expand_tmds =
            2 << HSTX_CTRL_EXPAND_TMDS_L2_NBITS_LSB |
                0 << HSTX_CTRL_EXPAND_TMDS_L2_ROT_LSB |
                2 << HSTX_CTRL_EXPAND_TMDS_L1_NBITS_LSB |
                29 << HSTX_CTRL_EXPAND_TMDS_L1_ROT_LSB |
                1 << HSTX_CTRL_EXPAND_TMDS_L0_NBITS_LSB |
                26 << HSTX_CTRL_EXPAND_TMDS_L0_ROT_LSB;
    } else if (color_depth == 4) {
        // Configure HSTX's TMDS encoder for RGBD
        hstx_ctrl_hw->expand_tmds =
            0 << HSTX_CTRL_EXPAND_TMDS_L2_NBITS_LSB |
                28 << HSTX_CTRL_EXPAND_TMDS_L2_ROT_LSB |
                0 << HSTX_CTRL_EXPAND_TMDS_L1_NBITS_LSB |
                27 << HSTX_CTRL_EXPAND_TMDS_L1_ROT_LSB |
                0 << HSTX_CTRL_EXPAND_TMDS_L0_NBITS_LSB |
                26 << HSTX_CTRL_EXPAND_TMDS_L0_ROT_LSB;
    } else {
        // Grayscale
        uint8_t rot = 24 + color_depth;
        hstx_ctrl_hw->expand_tmds =
            (color_depth - 1) << HSTX_CTRL_EXPAND_TMDS_L2_NBITS_LSB |
                rot << HSTX_CTRL_EXPAND_TMDS_L2_ROT_LSB |
                    (color_depth - 1) << HSTX_CTRL_EXPAND_TMDS_L1_NBITS_LSB |
                rot << HSTX_CTRL_EXPAND_TMDS_L1_ROT_LSB |
                    (color_depth - 1) << HSTX_CTRL_EXPAND_TMDS_L0_NBITS_LSB |
                rot << HSTX_CTRL_EXPAND_TMDS_L0_ROT_LSB;
    }
    size_t pixels_per_word;
    if (output_scaling == 1) {
        pixels_per_word = 32 / color_depth;
    } else {
        pixels_per_word = 1;
    }

    size_t shifts_before_empty = (pixels_per_word % 32);
    if (output_scaling > 1) {
        shifts_before_empty *= output_scaling;
    }

    size_t shift_amount = color_depth % 32;

    // Pixels come in 32 bits at a time. color_depth dictates the number
    // of pixels per word. Control symbols (RAW) are an entire 32-bit word.
    hstx_ctrl_hw->expand_shift =
        shifts_before_empty << HSTX_CTRL_EXPAND_SHIFT_ENC_N_SHIFTS_LSB |
            shift_amount << HSTX_CTRL_EXPAND_SHIFT_ENC_SHIFT_LSB |
            1 << HSTX_CTRL_EXPAND_SHIFT_RAW_N_SHIFTS_LSB |
            0 << HSTX_CTRL_EXPAND_SHIFT_RAW_SHIFT_LSB;

    // Serial output config: clock period of 5 cycles, pop from command
    // expander every 5 cycles, shift the output shiftreg by 2 every cycle.
    hstx_ctrl_hw->csr = 0;
    hstx_ctrl_hw->csr =
        HSTX_CTRL_CSR_EXPAND_EN_BITS |
        5u << HSTX_CTRL_CSR_CLKDIV_LSB |
            5u << HSTX_CTRL_CSR_N_SHIFTS_LSB |
            2u << HSTX_CTRL_CSR_SHIFT_LSB |
            HSTX_CTRL_CSR_EN_BITS;

    // Note we are leaving the HSTX clock at the SDK default of 125 MHz; since
    // we shift out two bits per HSTX clock cycle, this gives us an output of
    // 250 Mbps, which is very close to the bit clock for 480p 60Hz (252 MHz).
    // If we want the exact rate then we'll have to reconfigure PLLs.

    // Setup the data to pin mapping. `pins` is a pair of pins in a standard
    // order: clock, red, green and blue. We don't actually care they are next
    // to one another but they'll work better that way.
    for (size_t i = 0; i < 8; i++) {
        uint lane = i / 2;
        size_t invert = i % 2 == 1 ? HSTX_CTRL_BIT0_INV_BITS : 0;
        uint32_t lane_data_sel_bits;
        // Clock
        if (lane == 0) {
            lane_data_sel_bits = HSTX_CTRL_BIT0_CLK_BITS;
        } else {
            // Output even bits during first half of each HSTX cycle, and odd bits
            // during second half. The shifter advances by two bits each cycle.
            lane -= 1;
            lane_data_sel_bits =
                (lane * 10) << HSTX_CTRL_BIT0_SEL_P_LSB |
                        (lane * 10 + 1) << HSTX_CTRL_BIT0_SEL_N_LSB;
        }
        hstx_ctrl_hw->bit[pins[i]] = lane_data_sel_bits | invert;
    }

    for (int i = 12; i <= 19; ++i) {
        gpio_set_function(i, 0); // HSTX
    }

    dma_channel_config c;
    c = dma_channel_get_default_config(self->dma_command_channel);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_32);
    channel_config_set_read_increment(&c, true);
    channel_config_set_write_increment(&c, true);
    // This wraps the transfer back to the start of the write address.
    size_t wrap = 3; // 8 bytes because we write two DMA registers.
    volatile uint32_t *write_addr = &dma_hw->ch[self->dma_pixel_channel].al3_transfer_count;
    if (output_scaling > 1) {
        wrap = 4; // 16 bytes because we write all four DMA registers.
        write_addr = &dma_hw->ch[self->dma_pixel_channel].al3_ctrl;
    }
    channel_config_set_ring(&c, true, wrap);
    // No chain because we use an interrupt to reload this channel instead of a
    // third channel.
    dma_channel_configure(
        self->dma_command_channel,
        &c,
        write_addr,
        self->dma_commands,
        (1 << wrap) / sizeof(uint32_t),
        false
        );

    dma_hw->ints1 = (1u << self->dma_pixel_channel);
    dma_hw->inte1 = (1u << self->dma_pixel_channel);
    irq_set_exclusive_handler(DMA_IRQ_1, dma_irq_handler);
    irq_set_enabled(DMA_IRQ_1, true);
    irq_set_priority(DMA_IRQ_1, PICO_HIGHEST_IRQ_PRIORITY);

    bus_ctrl_hw->priority = BUSCTRL_BUS_PRIORITY_DMA_W_BITS | BUSCTRL_BUS_PRIORITY_DMA_R_BITS;

    // For the output.
    self->framebuffer_len = framebuffer_size;

    active_picodvi = self;

    common_hal_picodvi_framebuffer_refresh(self);
    dma_irq_handler();
}

static void _turn_off_dma(int channel) {
    if (channel < 0) {
        return;
    }
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

    // Stop frame restarts before aborting either channel. An already pending
    // frame IRQ must not repoint the command DMA during teardown.
    uint32_t irq_state = save_and_disable_interrupts();
    active_picodvi = NULL;
    if (self->dma_pixel_channel >= 0) {
        dma_channel_set_irq1_enabled(self->dma_pixel_channel, false);
        dma_hw->ints1 = 1u << self->dma_pixel_channel;
    }
    restore_interrupts(irq_state);

    for (int i = 12; i <= 19; ++i) {
        reset_pin_number(i);
    }

    _turn_off_dma(self->dma_pixel_channel);
    _turn_off_dma(self->dma_command_channel);
    self->dma_pixel_channel = -1;
    self->dma_command_channel = -1;

    picodvi_audioout_framebuffer_deinit(self);

    port_free(self->framebuffer);
    self->framebuffer = NULL;

    port_free(self->dma_commands);
    self->dma_commands = NULL;

    self->base.type = &mp_type_NoneType;
}

bool common_hal_picodvi_framebuffer_deinited(picodvi_framebuffer_obj_t *self) {
    return self->framebuffer == NULL;
}

void common_hal_picodvi_framebuffer_refresh(picodvi_framebuffer_obj_t *self) {
}

void common_hal_picodvi_framebuffer_wait_for_vblank(picodvi_framebuffer_obj_t *self) {
    // Block until the next frame boundary (vblank). A full-framebuffer repaint issued right after
    // this returns starts at the top of scanout and stays ahead of the beam, so it does not tear
    // mid-screen. Bounded by a timeout so a stopped or absent DVI signal can never hang the caller.
    if (self != active_picodvi) {
        return;
    }
    uint32_t start = framebuffer_frame_count;
    uint64_t timeout = common_hal_time_monotonic_ms() + 30;    // ~2 frames @ 60 Hz
    while (framebuffer_frame_count == start) {
        RUN_BACKGROUND_TASKS;      // don't starve USB host / other tasks during the ~<16 ms wait
        if (common_hal_time_monotonic_ms() >= timeout) {
            break;
        }
    }
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

int common_hal_picodvi_framebuffer_get_native_frames_per_second(picodvi_framebuffer_obj_t *self) {
    return self->output_width == 640 ? 60 : 70;
}

bool common_hal_picodvi_framebuffer_get_grayscale(picodvi_framebuffer_obj_t *self) {
    return self->color_depth < 4;
}

mp_int_t common_hal_picodvi_framebuffer_get_buffer(mp_obj_t self_in, mp_buffer_info_t *bufinfo, mp_uint_t flags) {
    picodvi_framebuffer_obj_t *self = (picodvi_framebuffer_obj_t *)self_in;
    bufinfo->buf = self->framebuffer;
    if (self->color_depth == 32) {
        bufinfo->typecode = 'I';
    } else if (self->color_depth == 16) {
        bufinfo->typecode = 'H';
    } else {
        bufinfo->typecode = 'B';
    }
    bufinfo->len = self->framebuffer_len * sizeof(uint32_t);
    return 0;
}

int common_hal_picodvi_framebuffer_get_row_stride(picodvi_framebuffer_obj_t *self) {
    // Pitch is in words but row stride is expected as bytes.
    return self->pitch * sizeof(uint32_t);
}
