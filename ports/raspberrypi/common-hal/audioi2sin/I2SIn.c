// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>
#include <string.h>

#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/runtime.h"
#include "shared/runtime/interrupt_char.h"
#include "common-hal/audioi2sin/I2SIn.h"
#include "shared-bindings/audioi2sin/I2SIn.h"
#include "shared-bindings/audiocore/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-module/audioi2sin/__init__.h"
#include "bindings/rp2pio/StateMachine.h"
#include "supervisor/port.h"

#include "hardware/dma.h"

#if CIRCUITPY_AUDIOI2SIN

// 16-bit programs sample a stereo frame of 16+16 = 32 bits and rely on
// auto-push at 32 to deliver one packed (right<<16 | left) word per frame.
// 32-bit programs sample 32 bits per channel and produce two FIFO words per
// frame (right first, then left). Each bit takes 6 PIO clocks (a [2]-delay
// pair). 24-bit recordings reuse the 32-bit programs because most 24-bit
// MEMS mics (SPH0645LM4H, INMP441, ICS-43434) transmit their 24 valid bits
// left-justified inside a 32-bit slot.
//
// `in pins 1` runs on a cycle where side-set drives BCLK high. The attached
// device updates the data line on the BCLK falling edge, so by the rising edge it
// has settled and is safe to sample. Sampling at BCLK low (the previous,
// incorrect arrangement) catches the data mid-transition and the result is
// effectively noise.
#define PIO_CLOCKS_PER_BIT (6)

// Internal-clock RX, regular pin order (BCLK = WS - 1), Philips alignment.
static const uint16_t i2sin_program[] = {
    0xb842, //  0: nop                    side 3
            //     .wrap_target
    0xf94e, //  1: set    y, 14           side 3 [1]
    0xb242, //  2: nop                    side 2 [2]
    0x5801, //  3: in     pins, 1         side 3
    0x1982, //  4: jmp    y--, 2          side 3 [1]
    0xa242, //  5: nop                    side 0 [2]
    0x4801, //  6: in     pins, 1         side 1
    0xe94e, //  7: set    y, 14           side 1 [1]
    0xa242, //  8: nop                    side 0 [2]
    0x4801, //  9: in     pins, 1         side 1
    0x0988, // 10: jmp    y--, 8          side 1 [1]
    0xb242, // 11: nop                    side 2 [2]
    0x5801, // 12: in     pins, 1         side 3
            //     .wrap
};

// Internal-clock RX, regular pin order, left-justified.
static const uint16_t i2sin_program_left_justified[] = {
    0xa842, //  0: nop                    side 1
            //     .wrap_target
    0xe94e, //  1: set    y, 14           side 1 [1]
    0xb242, //  2: nop                    side 2 [2]
    0x5801, //  3: in     pins, 1         side 3
    0x1982, //  4: jmp    y--, 2          side 3 [1]
    0xb242, //  5: nop                    side 2 [2]
    0x5801, //  6: in     pins, 1         side 3
    0xf94e, //  7: set    y, 14           side 3 [1]
    0xa242, //  8: nop                    side 0 [2]
    0x4801, //  9: in     pins, 1         side 1
    0x0988, // 10: jmp    y--, 8          side 1 [1]
    0xa242, // 11: nop                    side 0 [2]
    0x4801, // 12: in     pins, 1         side 1
            //     .wrap
};

// Internal-clock RX, swapped pin order (BCLK = WS + 1), Philips alignment.
static const uint16_t i2sin_program_swap[] = {
    0xb842, //  0: nop                    side 3
            //     .wrap_target
    0xf94e, //  1: set    y, 14           side 3 [1]
    0xaa42, //  2: nop                    side 1 [2]
    0x5801, //  3: in     pins, 1         side 3
    0x1982, //  4: jmp    y--, 2          side 3 [1]
    0xa242, //  5: nop                    side 0 [2]
    0x5001, //  6: in     pins, 1         side 2
    0xf14e, //  7: set    y, 14           side 2 [1]
    0xa242, //  8: nop                    side 0 [2]
    0x5001, //  9: in     pins, 1         side 2
    0x1188, // 10: jmp    y--, 8          side 2 [1]
    0xaa42, // 11: nop                    side 1 [2]
    0x5801, // 12: in     pins, 1         side 3
            //     .wrap
};

// Internal-clock RX, swapped pin order, left-justified.
static const uint16_t i2sin_program_left_justified_swap[] = {
    0xb042, //  0: nop                    side 2
            //     .wrap_target
    0xf14e, //  1: set    y, 14           side 2 [1]
    0xaa42, //  2: nop                    side 1 [2]
    0x5801, //  3: in     pins, 1         side 3
    0x1982, //  4: jmp    y--, 2          side 3 [1]
    0xaa42, //  5: nop                    side 1 [2]
    0x5801, //  6: in     pins, 1         side 3
    0xf94e, //  7: set    y, 14           side 3 [1]
    0xa242, //  8: nop                    side 0 [2]
    0x5001, //  9: in     pins, 1         side 2
    0x1188, // 10: jmp    y--, 8          side 2 [1]
    0xa242, // 11: nop                    side 0 [2]
    0x5001, // 12: in     pins, 1         side 2
            //     .wrap
};

// 32-bit-per-channel variants: identical to the 16-bit programs above except
// the loop counter is set to 30 (so each `bitloop` runs 31 in's, plus one
// outside the loop = 32 in's per channel).
static const uint16_t i2sin_program_32[] = {
    0xb842, //  0: nop                    side 3
            //     .wrap_target
    0xf95e, //  1: set    y, 30           side 3 [1]
    0xb242, //  2: nop                    side 2 [2]
    0x5801, //  3: in     pins, 1         side 3
    0x1982, //  4: jmp    y--, 2          side 3 [1]
    0xa242, //  5: nop                    side 0 [2]
    0x4801, //  6: in     pins, 1         side 1
    0xe95e, //  7: set    y, 30           side 1 [1]
    0xa242, //  8: nop                    side 0 [2]
    0x4801, //  9: in     pins, 1         side 1
    0x0988, // 10: jmp    y--, 8          side 1 [1]
    0xb242, // 11: nop                    side 2 [2]
    0x5801, // 12: in     pins, 1         side 3
            //     .wrap
};

static const uint16_t i2sin_program_left_justified_32[] = {
    0xa842, //  0: nop                    side 1
            //     .wrap_target
    0xe95e, //  1: set    y, 30           side 1 [1]
    0xb242, //  2: nop                    side 2 [2]
    0x5801, //  3: in     pins, 1         side 3
    0x1982, //  4: jmp    y--, 2          side 3 [1]
    0xb242, //  5: nop                    side 2 [2]
    0x5801, //  6: in     pins, 1         side 3
    0xf95e, //  7: set    y, 30           side 3 [1]
    0xa242, //  8: nop                    side 0 [2]
    0x4801, //  9: in     pins, 1         side 1
    0x0988, // 10: jmp    y--, 8          side 1 [1]
    0xa242, // 11: nop                    side 0 [2]
    0x4801, // 12: in     pins, 1         side 1
            //     .wrap
};

static const uint16_t i2sin_program_swap_32[] = {
    0xb842, //  0: nop                    side 3
            //     .wrap_target
    0xf95e, //  1: set    y, 30           side 3 [1]
    0xaa42, //  2: nop                    side 1 [2]
    0x5801, //  3: in     pins, 1         side 3
    0x1982, //  4: jmp    y--, 2          side 3 [1]
    0xa242, //  5: nop                    side 0 [2]
    0x5001, //  6: in     pins, 1         side 2
    0xf15e, //  7: set    y, 30           side 2 [1]
    0xa242, //  8: nop                    side 0 [2]
    0x5001, //  9: in     pins, 1         side 2
    0x1188, // 10: jmp    y--, 8          side 2 [1]
    0xaa42, // 11: nop                    side 1 [2]
    0x5801, // 12: in     pins, 1         side 3
            //     .wrap
};

static const uint16_t i2sin_program_left_justified_swap_32[] = {
    0xb042, //  0: nop                    side 2
            //     .wrap_target
    0xf15e, //  1: set    y, 30           side 2 [1]
    0xaa42, //  2: nop                    side 1 [2]
    0x5801, //  3: in     pins, 1         side 3
    0x1982, //  4: jmp    y--, 2          side 3 [1]
    0xaa42, //  5: nop                    side 1 [2]
    0x5801, //  6: in     pins, 1         side 3
    0xf95e, //  7: set    y, 30           side 3 [1]
    0xa242, //  8: nop                    side 0 [2]
    0x5001, //  9: in     pins, 1         side 2
    0x1188, // 10: jmp    y--, 8          side 2 [1]
    0xa242, // 11: nop                    side 0 [2]
    0x5001, // 12: in     pins, 1         side 2
            //     .wrap
};

// External-clock RX. BCLK and WS are inputs driven by something
// else -- another I2S object or the codec -- so the state machine side-sets
// nothing and waits on the two clocks instead. `wait gpio` encodes an absolute
// pin index, so unlike the internal-clock programs above this one is assembled at
// construct time with the pin numbers patched in.
//
//     0: wait 0 gpio W          ; resync: find a WS rising edge, i.e. the start
//     1: wait 1 gpio W          ;   of the RIGHT channel (matches word order)
//        .wrap_target
//     2: set y, 31
//     3: wait 0 gpio B          ; data changes here
//     4: wait 1 gpio B          ; ...and is settled here
//     5: in pins, 1
//     6: jmp y--, 3
//        .wrap
//
// `set y, 31` + `jmp y--` runs the loop exactly 32 times, so with auto-push at
// 32 and shift-left this pushes one 32-bit word per 32 BCLK, MSB first --
// identical to what the internal-clock programs produce, so record_to_buffer and
// fill_buffer need no changes. One template covers every bit_depth: at 16 bits
// a 32-BCLK frame is one push (right<<16 | left), at 24/32 a 64-BCLK frame is
// two pushes (right then left).
//
// The WS resync lands on the first data bit, so the program above is
// the `left_justified` alignment: data starts on the WS edge and no instruction
// is spent on a delay bit. The default (Philips) alignment is that program plus
// one more BCLK of skew, the other of the two possible alignments.
//
// Free-running after the initial sync: the external frame must be exactly
// 2 x bits_per_channel BCLKs, the same assumption internal clock mode already bakes
// in. If sync is lost it stays lost.
#define I2SIN_EXT_CLOCK_MAX_PROGRAM_LEN (8)
// The bit loop is the last 5 instructions; everything before it is one-shot sync.
#define I2SIN_EXT_CLOCK_WRAP_TARGET(len) ((int)(len) - 5)

static size_t build_i2sin_ext_clock_program(uint16_t *prog, uint8_t bclk, uint8_t ws,
    bool left_justified) {
    const uint16_t wait_0_bclk = 0x2000 | bclk;
    const uint16_t wait_1_bclk = 0x2080 | bclk;
    size_t len = 0;
    prog[len++] = 0x2000 | ws;  // wait 0 gpio W
    prog[len++] = 0x2080 | ws;  // wait 1 gpio W
    if (!left_justified) {
        prog[len++] = wait_1_bclk;  // one more BCLK of skew
    }
    const size_t bitloop = len + 1;
    prog[len++] = 0xe05f;       // set y, 31
    prog[len++] = wait_0_bclk;
    prog[len++] = wait_1_bclk;
    prog[len++] = 0x4001;       // in pins, 1
    prog[len++] = 0x0080 | bitloop; // jmp y--, bitloop
    return len;
}

// `wait gpio` indices are relative to the PIO's GPIO base, which
// rp2pio_statemachine_construct picks the same way.
static uint8_t i2s_wait_gpio_index(const mcu_pin_obj_t *pin, uint8_t gpio_offset) {
    if (pin->number < gpio_offset) {
        mp_raise_ValueError(MP_ERROR_TEXT("Cannot use GPIO0..15 together with GPIO32..47"));
    }
    return pin->number - gpio_offset;
}

// Caller validates that pins are free.
void common_hal_audioi2sin_i2sin_construct(audioi2sin_i2sin_obj_t *self,
    const mcu_pin_obj_t *bit_clock, const mcu_pin_obj_t *word_select,
    const mcu_pin_obj_t *data, const mcu_pin_obj_t *main_clock,
    uint32_t sample_rate, uint8_t bit_depth, uint8_t output_bit_depth,
    bool mono, bool left_justified, bool samples_signed,
    bool external_clock) {
    // Ensure the object starts in its deinit state, so its finaliser does
    // nothing if this raises before the state machine is constructed.
    common_hal_rp2pio_statemachine_mark_deinit(&self->state_machine);

    if (main_clock != NULL) {
        mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("%q"), MP_QSTR_main_clock);
    }
    if (bit_depth != 16 && bit_depth != 24 && bit_depth != 32) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("%q must be 16, 24, or 32"), MP_QSTR_bit_depth);
    }

    // 24- and 32-bit recordings both clock 32 bits per channel; 24-bit MEMS
    // mics deliver their data left-justified inside that 32-bit slot.
    bool wide = (bit_depth != 16);
    uint32_t bits_per_channel = wide ? 32 : 16;
    uint32_t pio_clocks_per_frame = bits_per_channel * 2 * PIO_CLOCKS_PER_BIT;

    const mcu_pin_obj_t *sideset_pin = NULL;
    const uint16_t *program = NULL;
    size_t program_len = 0;
    uint16_t ext_clock_program[I2SIN_EXT_CLOCK_MAX_PROGRAM_LEN];
    pio_pinmask_t wait_gpio_mask = PIO_PINMASK_NONE;

    if (external_clock) {
        // In external clock mode the clocks are `wait gpio` targets, so they
        // need not be sequential GPIOs.
        uint8_t gpio_offset = 0;
        #if NUM_BANK0_GPIOS > 32
        if (bit_clock->number >= 32 || word_select->number >= 32 || data->number >= 32) {
            gpio_offset = 16;
        }
        #endif
        program_len = build_i2sin_ext_clock_program(ext_clock_program,
            i2s_wait_gpio_index(bit_clock, gpio_offset),
            i2s_wait_gpio_index(word_select, gpio_offset),
            left_justified);
        program = ext_clock_program;
        wait_gpio_mask = PIO_PINMASK_OR(PIO_PINMASK_FROM_PIN(bit_clock->number),
            PIO_PINMASK_FROM_PIN(word_select->number));
    } else if (bit_clock->number == word_select->number - 1) {
        sideset_pin = bit_clock;
        if (left_justified) {
            program = wide ? i2sin_program_left_justified_32 : i2sin_program_left_justified;
            program_len = wide ? MP_ARRAY_SIZE(i2sin_program_left_justified_32)
                               : MP_ARRAY_SIZE(i2sin_program_left_justified);
        } else {
            program = wide ? i2sin_program_32 : i2sin_program;
            program_len = wide ? MP_ARRAY_SIZE(i2sin_program_32)
                               : MP_ARRAY_SIZE(i2sin_program);
        }
    } else if (bit_clock->number == word_select->number + 1) {
        sideset_pin = word_select;
        if (left_justified) {
            program = wide ? i2sin_program_left_justified_swap_32 : i2sin_program_left_justified_swap;
            program_len = wide ? MP_ARRAY_SIZE(i2sin_program_left_justified_swap_32)
                               : MP_ARRAY_SIZE(i2sin_program_left_justified_swap);
        } else {
            program = wide ? i2sin_program_swap_32 : i2sin_program_swap;
            program_len = wide ? MP_ARRAY_SIZE(i2sin_program_swap_32)
                               : MP_ARRAY_SIZE(i2sin_program_swap);
        }
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("Bit clock and word select must be sequential GPIO pins"));
    }

    common_hal_rp2pio_statemachine_construct(
        &self->state_machine,
        program, program_len,
        // In external clock mode the SM is driven by the waits, not the clock
        // divider, so run it at sysclk.
        external_clock ? 0 : sample_rate *pio_clocks_per_frame,
        NULL, 0, // init
        NULL, 0, // may_exec
        NULL, 0, PIO_PINMASK32_NONE, PIO_PINMASK32_NONE, // out pin
        data, 1, // in pins
        PIO_PINMASK32_NONE, PIO_PINMASK32_NONE, // in pulls
        NULL, 0, PIO_PINMASK32_NONE, PIO_PINMASK32_FROM_VALUE(0x1f), // set pins
        sideset_pin, sideset_pin == NULL ? 0 : 2, false, PIO_PINMASK32_NONE, PIO_PINMASK32_FROM_VALUE(0x1f), // sideset pins
        false, // No sideset enable
        NULL, PULL_NONE, // jump pin
        wait_gpio_mask, // wait gpio pins
        // The clocks are shared through wait_gpio_mask, which _check_gpio_mask_free
        // already allows to be shared; the data pin stays exclusively ours.
        true, // exclusive pin use
        false, 32, false, // out settings (unused)
        false, // Wait for txstall
        true, 32, false, // in settings: auto-push at 32 bits, shift left (MSB first)
        false, // Not user-interruptible.
        external_clock ? I2SIN_EXT_CLOCK_WRAP_TARGET(program_len) : 1,
        external_clock ? (int)program_len - 1 : -1, // wrap settings
        PIO_ANY_OFFSET,
        PIO_FIFO_TYPE_DEFAULT,
        PIO_MOV_STATUS_DEFAULT,
        PIO_MOV_N_DEFAULT);

    // In external clock mode the SM runs at sysclk and the real rate is
    // whatever the outside world drives WS at, so `sample_rate` is a
    // declaration rather than a measurement. A mismatch shows up as the same
    // slow drift the underrun-pad / overflow-drop paths in fill_buffer absorb.
    self->sample_rate = external_clock
        ? sample_rate
        : common_hal_rp2pio_statemachine_get_frequency(&self->state_machine) / pio_clocks_per_frame;
    self->bit_depth = bit_depth;
    self->mono = mono;
    self->samples_signed = samples_signed;
    self->left_justified = left_justified;
    self->external_clock = external_clock;
    self->settled = false;
    self->ring = NULL;
    self->ring_size = 0;
    self->half_size = 0;
    self->read_pos = 0;
    self->dma_channel = -1;
    self->overflow = false;

    // Populate the audiosample base so I2SIn can feed the audio pipeline as a
    // streaming source. The output depth must be 8 or 16 to actually stream; we
    // still fill base here (so sample_rate/bits_per_sample/etc. read back) and
    // raise from reset_buffer if a 24/32-bit instance is played. The owned
    // conversion buffer is allocated lazily on first reset_buffer().
    uint8_t channel_count = mono ? 1 : 2;
    // Report the *nominal* requested rate (not the PIO-derived self->sample_rate,
    // which can be off by a fraction of a Hz). audiosample_must_match requires
    // exact equality against the consumer, which is configured for the nominal
    // rate; the tiny capture-clock difference shows up as the slow drift that the
    // underrun silence-pad / overflow-drop paths in fill_buffer absorb.
    self->base.sample_rate = sample_rate;
    self->base.bits_per_sample = output_bit_depth;
    self->base.channel_count = channel_count;
    self->base.samples_signed = samples_signed;
    self->base.single_buffer = false;
    self->base.max_buffer_length =
        2 * AUDIOI2SIN_STREAM_FRAMES * (output_bit_depth / 8) * channel_count;
    self->output_buffer = NULL;
    self->output_half_bytes = self->base.max_buffer_length / 2;
    self->output_index = 0;

    // Each PIO frame produces 4 bytes in the FIFO regardless of bit depth
    // (16-bit auto-pushes one packed stereo word, 24/32-bit pushes two
    // separate 32-bit words). One stereo frame is therefore either 4 or
    // 8 bytes; size the half-buffer for ~40 ms of audio so a slow consumer
    // (SD card flush etc.) can complete without overrunning.
    size_t bytes_per_frame = (bit_depth == 16) ? 4 : 8;
    size_t target = (size_t)self->sample_rate * bytes_per_frame * 40 / 1000;
    size_t half_size = (target > 4096) ? target : 4096;
    // Round up to a multiple of 8 so 24/32-bit pair reads never straddle
    // the half boundary.
    half_size = (half_size + 7u) & ~(size_t)7u;

    self->ring = (uint8_t *)port_malloc(2 * half_size, true);
    if (self->ring == NULL) {
        common_hal_rp2pio_statemachine_deinit(&self->state_machine);
        m_malloc_fail(2 * half_size);
    }
    self->half_size = half_size;
    self->ring_size = 2 * half_size;

    common_hal_rp2pio_statemachine_set_read_buffers_raw(&self->state_machine,
        NULL, 0,
        self->ring, half_size,
        self->ring + half_size, half_size);
    if (!common_hal_rp2pio_statemachine_background_read(&self->state_machine, 4, false)) {
        port_free(self->ring);
        self->ring = NULL;
        common_hal_rp2pio_statemachine_deinit(&self->state_machine);
        mp_raise_OSError(MP_EIO);
    }
    self->dma_channel = common_hal_rp2pio_statemachine_get_read_dma_channel(&self->state_machine);
}

bool common_hal_audioi2sin_i2sin_deinited(audioi2sin_i2sin_obj_t *self) {
    return common_hal_rp2pio_statemachine_deinited(&self->state_machine);
}

void common_hal_audioi2sin_i2sin_deinit(audioi2sin_i2sin_obj_t *self) {
    if (common_hal_audioi2sin_i2sin_deinited(self)) {
        return;
    }
    common_hal_rp2pio_statemachine_stop_background_read(&self->state_machine);
    common_hal_rp2pio_statemachine_deinit(&self->state_machine);
    if (self->ring != NULL) {
        port_free(self->ring);
        self->ring = NULL;
    }
    if (self->output_buffer != NULL) {
        port_free(self->output_buffer);
        self->output_buffer = NULL;
    }
    self->ring_size = 0;
    self->half_size = 0;
    self->dma_channel = -1;
    audiosample_mark_deinit(&self->base);
}

uint8_t common_hal_audioi2sin_i2sin_get_bit_depth(audioi2sin_i2sin_obj_t *self) {
    return self->bit_depth;
}

uint32_t common_hal_audioi2sin_i2sin_get_sample_rate(audioi2sin_i2sin_obj_t *self) {
    return self->sample_rate;
}

bool common_hal_audioi2sin_i2sin_get_samples_signed(audioi2sin_i2sin_obj_t *self) {
    return self->samples_signed;
}

// The flag latches when record_to_buffer or fill_buffer finds the DMA more than
// a half-buffer ahead of the read cursor and has to skip forward. Reading clears
// it so a streaming consumer can attribute each report to a known interval.
bool common_hal_audioi2sin_i2sin_get_overflow(audioi2sin_i2sin_obj_t *self) {
    bool overflow = self->overflow;
    self->overflow = false;
    return overflow;
}

// In 16-bit mode, each PIO frame produces a single 32-bit FIFO word with bits
// 31..16 = right channel and bits 15..0 = left channel (both MSB-first signed
// 16-bit). In 24/32-bit mode each frame produces two FIFO words: right first,
// then left, each MSB-first in the full 32 bits. For mono we keep the left
// channel; for stereo we emit (left, right) pairs.
//
// `output_buffer_length` is the requested number of samples to write (sample
// width = 2 bytes for bit_depth=16, 4 bytes for bit_depth=24 or 32). Returns
// the number actually written.
// Compute the byte offset inside `ring` that the DMA is currently writing
// (one past the last word it finished). Always lies in [0, ring_size).
static size_t i2sin_write_pos(audioi2sin_i2sin_obj_t *self) {
    uintptr_t addr = (uintptr_t)dma_channel_hw_addr(self->dma_channel)->write_addr;
    uintptr_t base = (uintptr_t)self->ring;
    if (addr < base || addr >= base + self->ring_size) {
        // The ISR retargets write_addr to the start of the next half right
        // after a transfer completes; it should always be in-range, but if
        // we observe it mid-update just report "no new data".
        return self->read_pos;
    }
    return (size_t)(addr - base);
}

// 24-bit non-left-justified data arrives in the low 24 bits of the FIFO word
// with the sign bit at bit 23 and bits 31..24 zero. To make it decode
// correctly as int32 (array typecode "i"), lift the sign bit to bit 31.
static inline uint32_t movesign24(uint32_t val) {
    return ((val & 0x800000u) << 8) | (val & 0x7fffffu);
}

// Sign-extend a raw FIFO sample to a canonical int32 value. `raw` holds the
// sample as delivered by the PIO program for the given input bit depth and
// alignment. The returned int32 represents the same magnitude with the sign
// bit at bit 31.
static inline int32_t i2sin_normalize_signed(uint32_t raw, uint8_t in_depth,
    bool left_justified) {
    if (in_depth == 32) {
        return (int32_t)raw;
    }
    if (in_depth == 24) {
        if (left_justified) {
            // value in bits 31..8, sign at 31; arithmetic shift right 8
            return (int32_t)raw >> 8;
        }
        // value in bits 23..0, sign at 23
        uint32_t sign_bit = 0x800000u;
        return (int32_t)((raw ^ sign_bit) - sign_bit);
    }
    // 16-bit: low 16 bits, sign at 15
    return (int16_t)(raw & 0xffffu);
}

// Normalize `raw` (input-depth bits, just-read FIFO sample) for this port's wire
// format, then hand off to the shared converter to rescale `in_depth` ->
// `out_depth` and write it to `buffer` at sample index `idx`. The depth
// conversion + unsigned-WAV flip + container store live in
// `shared_audioi2sin_write_converted` (shared with other ports); see that helper
// for the upscale/downscale semantics.
//
// For signed 24-bit output, the int32 slot holds the sign-extended value
// (range -2^23 .. 2^23-1) — unlike the default `output_bit_depth=bit_depth=24`
// path which uses `movesign24`, the converted path returns proper two's
// complement so the result decodes correctly as int32.
static inline void i2sin_write_converted(void *buffer, uint32_t idx,
    uint32_t raw, uint8_t in_depth, uint8_t out_depth,
    bool samples_signed, bool left_justified) {
    int32_t s = i2sin_normalize_signed(raw, in_depth, left_justified);
    shared_audioi2sin_write_converted(buffer, idx, s, in_depth, out_depth, samples_signed);
}

uint32_t common_hal_audioi2sin_i2sin_record_to_buffer(audioi2sin_i2sin_obj_t *self,
    void *buffer, uint32_t output_buffer_length) {
    uint32_t output_count = 0;
    const size_t ring_size = self->ring_size;
    const size_t half_size = self->half_size;

    // I2S delivers signed PCM. When the caller asked for unsigned samples,
    // flip the sign bit per sample (XOR with 0x8000 for 16-bit, 0x800000 for
    // 24-bit data in a 32-bit slot, 0x80000000 for 32-bit), matching the WAV
    // convention. The default (no-conversion) path applies the flip to the
    // raw FIFO word before splitting into channels; the conversion path
    // applies the flip per output sample at output bit width.
    const bool convert = self->base.bits_per_sample != self->bit_depth;
    const uint32_t flip16 = (!convert && !self->samples_signed) ? 0x80008000u : 0u;
    const uint32_t flip32 = (!convert && !self->samples_signed)
        ? (self->bit_depth == 24 ? 0x800000u : 0x80000000u)
        : 0u;
    const bool fix_sign24 = !convert
        && self->bit_depth == 24
        && self->samples_signed
        && !self->left_justified;
    const uint8_t in_depth = self->bit_depth;
    const uint8_t out_depth = self->base.bits_per_sample;
    const bool left_justified = self->left_justified;
    const bool samples_signed = self->samples_signed;

    if (self->bit_depth == 16) {
        // 16-bit mode auto-pushes one stereo frame per FIFO word. The DMA has
        // been streaming since construct time, so the ring already contains
        // settled data; drop the first 4 bytes once to discard a single
        // pre-record frame (matches the prior synchronous behaviour).
        uint16_t *output = convert ? NULL : (uint16_t *)buffer;
        while (output_count < output_buffer_length) {
            size_t write_pos = i2sin_write_pos(self);
            size_t avail = (write_pos + ring_size - self->read_pos) % ring_size;
            if (avail > half_size) {
                // DMA has filled more than one half ahead of us -- we lost
                // data. Snap to just behind the DMA on a 4-byte boundary.
                self->overflow = true;
                self->read_pos = write_pos & ~(size_t)3u;
                avail = 0;
            }
            if (!self->settled && avail >= 4) {
                self->read_pos = (self->read_pos + 4) % ring_size;
                avail -= 4;
                self->settled = true;
            }
            if (avail < 4) {
                RUN_BACKGROUND_TASKS;
                if (mp_hal_is_interrupted()) {
                    return output_count;
                }
                continue;
            }
            while (avail >= 4 && output_count < output_buffer_length) {
                uint32_t frame = *(volatile uint32_t *)(self->ring + self->read_pos) ^ flip16;
                uint16_t left = (uint16_t)(frame & 0xffff);
                uint16_t right = (uint16_t)(frame >> 16);
                if (!convert) {
                    if (self->mono) {
                        output[output_count++] = left;
                    } else {
                        output[output_count++] = left;
                        if (output_count >= output_buffer_length) {
                            self->read_pos = (self->read_pos + 4) % ring_size;
                            avail -= 4;
                            break;
                        }
                        output[output_count++] = right;
                    }
                } else {
                    i2sin_write_converted(buffer, output_count++, left,
                        in_depth, out_depth, samples_signed, left_justified);
                    if (!self->mono) {
                        if (output_count >= output_buffer_length) {
                            self->read_pos = (self->read_pos + 4) % ring_size;
                            avail -= 4;
                            break;
                        }
                        i2sin_write_converted(buffer, output_count++, right,
                            in_depth, out_depth, samples_signed, left_justified);
                    }
                }
                self->read_pos = (self->read_pos + 4) % ring_size;
                avail -= 4;
            }
        }
    } else {
        // 24-/32-bit mode emits two FIFO pushes per stereo frame (right then
        // left). The state machine was started at instruction 0 by the
        // constructor, so the very first push is the right channel and
        // alternation is preserved as long as we never touch the program
        // counter and always read an even number of words. half_size is a
        // multiple of 8, so reading 8-byte pairs stays aligned across the
        // ring wrap.
        uint32_t *output = convert ? NULL : (uint32_t *)buffer;
        while (output_count < output_buffer_length) {
            size_t write_pos = i2sin_write_pos(self);
            size_t avail = (write_pos + ring_size - self->read_pos) % ring_size;
            if (avail > half_size) {
                // Overflow: snap to a frame-aligned position one half behind
                // the DMA's current half so we resume on the right channel.
                self->overflow = true;
                size_t cur_half = (write_pos < half_size) ? 0 : half_size;
                self->read_pos = (cur_half + half_size) % ring_size;
                self->settled = false;
                avail = (write_pos + ring_size - self->read_pos) % ring_size;
            }
            if (!self->settled && avail >= 8) {
                self->read_pos = (self->read_pos + 8) % ring_size;
                avail -= 8;
                self->settled = true;
            }
            if (avail < 8) {
                RUN_BACKGROUND_TASKS;
                if (mp_hal_is_interrupted()) {
                    return output_count;
                }
                continue;
            }
            while (avail >= 8 && output_count < output_buffer_length) {
                uint32_t right = *(volatile uint32_t *)(self->ring + self->read_pos) ^ flip32;
                size_t next_pos = (self->read_pos + 4) % ring_size;
                uint32_t left = *(volatile uint32_t *)(self->ring + next_pos) ^ flip32;
                if (fix_sign24) {
                    right = movesign24(right);
                    left = movesign24(left);
                }
                if (!convert) {
                    if (self->mono) {
                        output[output_count++] = left;
                    } else {
                        output[output_count++] = left;
                        if (output_count >= output_buffer_length) {
                            self->read_pos = (self->read_pos + 8) % ring_size;
                            avail -= 8;
                            break;
                        }
                        output[output_count++] = right;
                    }
                } else {
                    i2sin_write_converted(buffer, output_count++, left,
                        in_depth, out_depth, samples_signed, left_justified);
                    if (!self->mono) {
                        if (output_count >= output_buffer_length) {
                            self->read_pos = (self->read_pos + 8) % ring_size;
                            avail -= 8;
                            break;
                        }
                        i2sin_write_converted(buffer, output_count++, right,
                            in_depth, out_depth, samples_signed, left_justified);
                    }
                }
                self->read_pos = (self->read_pos + 8) % ring_size;
                avail -= 8;
            }
        }
    }

    return output_count;
}

// Write `count` silence samples at output depth starting at sample index `idx`.
// For signed PCM silence is 0; for unsigned (WAV) it is mid-scale.
static void i2sin_fill_silence(void *buffer, uint32_t idx, uint32_t count,
    uint8_t out_depth, bool samples_signed) {
    if (out_depth == 8) {
        uint8_t v = samples_signed ? 0 : 0x80u;
        memset((uint8_t *)buffer + idx, v, count);
    } else { // 16-bit (the only other streamable width)
        uint16_t v = samples_signed ? 0 : 0x8000u;
        uint16_t *p = (uint16_t *)buffer + idx;
        for (uint32_t i = 0; i < count; i++) {
            p[i] = v;
        }
    }
}

// Non-blocking fill: convert up to `frames` frames currently available in the
// DMA ring into `buffer` (output depth, interleaved), padding the remainder with
// silence on underrun rather than spinning. Used by get_buffer() from the output
// backend's refill interrupt, so it must never block. `out_depth` is always 8 or
// 16 here (reset_buffer rejects 24/32). Reuses the same per-sample conversion as
// record_to_buffer via i2sin_write_converted.
void common_hal_audioi2sin_i2sin_fill_buffer(audioi2sin_i2sin_obj_t *self,
    uint8_t *buffer, uint32_t frames) {
    const size_t ring_size = self->ring_size;
    const size_t half_size = self->half_size;
    const uint8_t in_depth = self->bit_depth;
    const uint8_t out_depth = self->base.bits_per_sample;
    const bool samples_signed = self->samples_signed;
    const bool left_justified = self->left_justified;
    const bool stereo = !self->mono;
    const uint8_t channel_count = stereo ? 2 : 1;
    const size_t frame_bytes = (in_depth == 16) ? 4 : 8;
    const uint32_t total_samples = frames * channel_count;

    uint32_t produced = 0;
    while (produced < total_samples) {
        size_t write_pos = i2sin_write_pos(self);
        size_t avail = (write_pos + ring_size - self->read_pos) % ring_size;
        if (avail > half_size) {
            // The consumer is slower than the mic and the ring is filling up
            // (the overflow case record_to_buffer also handles). Drop back to
            // just behind the DMA so latency stays bounded; the discarded audio
            // shows up as a brief discontinuity, not a hang.
            self->overflow = true;
            if (in_depth == 16) {
                self->read_pos = write_pos & ~(size_t)3u;
            } else {
                size_t cur_half = (write_pos < half_size) ? 0 : half_size;
                self->read_pos = (cur_half + half_size) % ring_size;
                self->settled = false;
            }
            avail = (write_pos + ring_size - self->read_pos) % ring_size;
        }
        if (!self->settled && avail >= frame_bytes) {
            // Drop one frame so playback starts on a settled sample and (for
            // 24/32-bit) on the right-channel word the program emits first.
            self->read_pos = (self->read_pos + frame_bytes) % ring_size;
            avail -= frame_bytes;
            self->settled = true;
        }
        if (avail < frame_bytes) {
            break; // underrun: pad the rest with silence below
        }
        uint32_t left, right;
        if (in_depth == 16) {
            uint32_t fr = *(volatile uint32_t *)(self->ring + self->read_pos);
            left = fr & 0xffffu;
            right = fr >> 16;
        } else {
            right = *(volatile uint32_t *)(self->ring + self->read_pos);
            size_t next_pos = (self->read_pos + 4) % ring_size;
            left = *(volatile uint32_t *)(self->ring + next_pos);
        }
        i2sin_write_converted(buffer, produced++, left,
            in_depth, out_depth, samples_signed, left_justified);
        if (stereo && produced < total_samples) {
            i2sin_write_converted(buffer, produced++, right,
                in_depth, out_depth, samples_signed, left_justified);
        }
        self->read_pos = (self->read_pos + frame_bytes) % ring_size;
    }
    if (produced < total_samples) {
        i2sin_fill_silence(buffer, produced, total_samples - produced,
            out_depth, samples_signed);
    }
}

void common_hal_audioi2sin_i2sin_reset_buffer(audioi2sin_i2sin_obj_t *self,
    bool single_channel_output, uint8_t channel) {
    (void)single_channel_output;
    (void)channel;
    // The audio pipeline only carries 8- or 16-bit samples. 24/32-bit modes can
    // still record() but cannot stream; fail clearly the first time playback is
    // set up rather than emitting garbage.
    if (self->base.bits_per_sample != 8 && self->base.bits_per_sample != 16) {
        mp_raise_ValueError_varg(
            MP_ERROR_TEXT("%q must be 8 or 16"), MP_QSTR_output_bit_depth);
    }
    if (self->output_buffer == NULL) {
        // The output backend's DMA reads this buffer directly, so it must be
        // DMA-capable like the input ring.
        self->output_buffer = (uint8_t *)port_malloc(self->base.max_buffer_length, true);
        if (self->output_buffer == NULL) {
            m_malloc_fail(self->base.max_buffer_length);
        }
    }
    self->output_index = 0;
    // An external-clock SM free-runs a 32-BCLK counter from the WS edge it
    // synced to at construct time, so anything that disturbs the frame leaves
    // it locked to the wrong half-frame or the wrong bit for good. Re-exec the
    // program here so playback always begins from a fresh WS sync. An
    // internal-clock SM generates its own clocks and has nothing to sync to,
    // so leave it alone.
    if (self->external_clock) {
        common_hal_rp2pio_statemachine_restart(&self->state_machine);
        // restart() clears the shift counters but not the RX FIFO, and the
        // words still sitting in it were captured with the old alignment.
        pio_sm_clear_fifos(self->state_machine.pio, self->state_machine.state_machine);
    }
    // Resync to live audio: snap the read cursor just behind the DMA write head
    // (frame-aligned) and re-settle so playback begins on fresh samples.
    size_t write_pos = i2sin_write_pos(self);
    self->read_pos = (self->bit_depth == 16)
        ? (write_pos & ~(size_t)3u)
        : (write_pos & ~(size_t)7u);
    self->settled = false;
    self->overflow = false;
}

audioio_get_buffer_result_t common_hal_audioi2sin_i2sin_get_buffer(
    audioi2sin_i2sin_obj_t *self, bool single_channel_output, uint8_t channel,
    uint8_t **buffer, uint32_t *buffer_length) {
    uint32_t half = self->output_half_bytes;
    uint8_t *out = self->output_buffer + half * self->output_index;
    self->output_index = 1 - self->output_index;

    uint32_t bytes_per_sample = self->base.bits_per_sample / 8;
    uint32_t frames = half / (bytes_per_sample * self->base.channel_count);
    common_hal_audioi2sin_i2sin_fill_buffer(self, out, frames);

    if (single_channel_output) {
        out += (channel % self->base.channel_count) * bytes_per_sample;
    }
    *buffer = out;
    *buffer_length = half;
    // A live mic is an infinite stream; never report DONE or the backend stops.
    return GET_BUFFER_MORE_DATA;
}

#endif // CIRCUITPY_AUDIOI2SIN
