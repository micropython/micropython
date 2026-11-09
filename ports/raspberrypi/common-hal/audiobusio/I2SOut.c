// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>
#include <string.h>

#include "mpconfigport.h"

#include "py/gc.h"
#include "py/mperrno.h"
#include "py/runtime.h"
#include "common-hal/audiobusio/I2SOut.h"
#include "shared-bindings/audiobusio/I2SOut.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-module/audiocore/__init__.h"
#include "bindings/rp2pio/StateMachine.h"

const uint16_t i2s_program[] = {

/* From i2s.pio:

.program i2s
.side_set 2

; Load the next set of samples
                    ;        /--- LRCLK
                    ;        |/-- BCLK
                    ;        ||
    pull noblock      side 0b11 ; Loads OSR with the next FIFO value or X
    mov x osr         side 0b11 ; Save the new value in case we need it again
    set y 14          side 0b11
bitloop1:
    out pins 1        side 0b10 [2] ; Right channel first
    jmp y-- bitloop1  side 0b11 [2]
    out pins 1        side 0b00 [2]
    set y 14          side 0b01 [2]
bitloop0:
    out pins 1        side 0b00 [2] ; Then left channel
    jmp y-- bitloop0  side 0b01 [2]
    out pins 1        side 0b10 [2]
*/
    // Above assembled with pioasm.
    0x9880, //  0: pull   noblock         side 3
    0xb827, //  1: mov    x, osr          side 3
    0xf84e, //  2: set    y, 14           side 3
    0x7201, //  3: out    pins, 1         side 2 [2]
    0x1a83, //  4: jmp    y--, 3          side 3 [2]
    0x6201, //  5: out    pins, 1         side 0 [2]
    0xea4e, //  6: set    y, 14           side 1 [2]
    0x6201, //  7: out    pins, 1         side 0 [2]
    0x0a87, //  8: jmp    y--, 7          side 1 [2]
    0x7201, //  9: out    pins, 1         side 2 [2]
};


const uint16_t i2s_program_left_justified[] = {
/* From i2s_left.pio:

.program i2s
.side_set 2

; Load the next set of samples
                     ;        /--- LRCLK
                     ;        |/-- BCLK
                     ;        ||
    pull noblock      side 0b01 ; Loads OSR with the next FIFO value or X
    mov x osr         side 0b01 ; Save the new value in case we need it again
    set y 14          side 0b01
bitloop1:
    out pins 1        side 0b10 [2] ; Right channel first
    jmp y-- bitloop1  side 0b11 [2]
    out pins 1        side 0b10 [2]
    set y 14          side 0b11 [2]
bitloop0:
    out pins 1        side 0b00 [2] ; Then left channel
    jmp y-- bitloop0  side 0b01 [2]
    out pins 1        side 0b00 [2]
*/
    // Above assembled with pioasm.
    0x8880, //  0: pull   noblock         side 1
    0xa827, //  1: mov    x, osr          side 1
    0xe84e, //  2: set    y, 14           side 1
    0x7201, //  3: out    pins, 1         side 2 [2]
    0x1a83, //  4: jmp    y--, 3          side 3 [2]
    0x7201, //  5: out    pins, 1         side 2 [2]
    0xfa4e, //  6: set    y, 14           side 3 [2]
    0x6201, //  7: out    pins, 1         side 0 [2]
    0x0a87, //  8: jmp    y--, 7          side 1 [2]
    0x6201, //  9: out    pins, 1         side 0 [2]
};

// Another version of i2s_program with the LRCLC and BCLK pin swapped
const uint16_t i2s_program_swap[] = {
/* From i2s_swap.pio:

.program i2s
.side_set 2

; Load the next set of samples
                    ;        /--- BCLK
                    ;        |/-- LRCLK
                    ;        ||
    pull noblock      side 0b11 ; Loads OSR with the next FIFO value or X
    mov x osr         side 0b11 ; Save the new value in case we need it again
    set y 14          side 0b11
bitloop1:
    out pins 1        side 0b01 [2] ; Right channel first
    jmp y-- bitloop1  side 0b11 [2]
    out pins 1        side 0b00 [2]
    set y 14          side 0b10 [2]
bitloop0:
    out pins 1        side 0b00 [2] ; Then left channel
    jmp y-- bitloop0  side 0b10 [2]
    out pins 1        side 0b01 [2]
*/
    // Above assembled with pioasm.
    0x9880, //  0: pull   noblock         side 3
    0xb827, //  1: mov    x, osr          side 3
    0xf84e, //  2: set    y, 14           side 3
    0x6a01, //  3: out    pins, 1         side 1 [2]
    0x1a83, //  4: jmp    y--, 3          side 3 [2]
    0x6201, //  5: out    pins, 1         side 0 [2]
    0xf24e, //  6: set    y, 14           side 2 [2]
    0x6201, //  7: out    pins, 1         side 0 [2]
    0x1287, //  8: jmp    y--, 7          side 2 [2]
    0x6a01, //  9: out    pins, 1         side 1 [2]
};

// Another version of i2s_program_left_justified with the LRCLC and BCLK pin
// swapped.
const uint16_t i2s_program_left_justified_swap[] = {
/* From i2s_swap_left.pio:

.program i2s
.side_set 2

; Load the next set of samples
                    ;        /--- BCLK
                    ;        |/-- LRCLK
                    ;        ||
    pull noblock      side 0b10 ; Loads OSR with the next FIFO value or X
    mov x osr         side 0b10 ; Save the new value in case we need it again
    set y 14          side 0b10
bitloop1:
    out pins 1        side 0b01 [2] ; Right channel first
    jmp y-- bitloop1  side 0b11 [2]
    out pins 1        side 0b01 [2]
    set y 14          side 0b11 [2]
bitloop0:
    out pins 1        side 0b00 [2] ; Then left channel
    jmp y-- bitloop0  side 0b10 [2]
    out pins 1        side 0b00 [2]
*/
    // Above assembled with pioasm.
    0x9080, //  0: pull   noblock         side 2
    0xb027, //  1: mov    x, osr          side 2
    0xf04e, //  2: set    y, 14           side 2
    0x6a01, //  3: out    pins, 1         side 1 [2]
    0x1a83, //  4: jmp    y--, 3          side 3 [2]
    0x6a01, //  5: out    pins, 1         side 1 [2]
    0xfa4e, //  6: set    y, 14           side 3 [2]
    0x6201, //  7: out    pins, 1         side 0 [2]
    0x1287, //  8: jmp    y--, 7          side 2 [2]
    0x6201, //  9: out    pins, 1         side 0 [2]
};

// External-clock TX. BCLK and WS are inputs driven by something
// else, so the state machine side-sets nothing and waits on the two clocks
// instead. `wait gpio` encodes an absolute pin index, so unlike the internal-clock
// programs above this one cannot be a static table: it is assembled at
// construct time with the pin numbers patched in.
//
//     0: wait 0 gpio W
//     1: wait 1 gpio W          ; right channel starts (WS changes on a BCLK fall)
//        .wrap_target
//     2: pull noblock           ; refills OSR from X on underflow, as the internal-clock program does
//     3: mov x, osr
//     4: set y, 31
//     5: wait 1 gpio B
//     6: wait 0 gpio B          ; drive on the falling edge; receiver latches on rising
//     7: out pins, 1
//     8: jmp y--, 5
//        .wrap
//
// Left-justified moves the `out` ahead of the two waits, which drives the MSB
// on the same falling edge WS changed on. The first loop iteration's `wait 1`
// otherwise lands on the Philips delay bit, so no pre-roll is needed.
//
// One 32-bit FIFO word covers a whole 16-bit stereo frame; at 24/32 bits the
// frame is two words (right then left). Same layout the internal-clock programs use.
#define I2S_EXT_CLOCK_PROGRAM_LEN (9)
#define I2S_EXT_CLOCK_WRAP_TARGET (2)
#define I2S_EXT_CLOCK_WRAP (8)

static void build_i2sout_ext_clock_program(uint16_t *prog, uint8_t bclk, uint8_t ws, bool left_justified) {
    const uint16_t wait_0_bclk = 0x2000 | bclk;
    const uint16_t wait_1_bclk = 0x2080 | bclk;
    prog[0] = 0x2000 | ws;  // wait 0 gpio W
    prog[1] = 0x2080 | ws;  // wait 1 gpio W
    prog[2] = 0x8080;       // pull noblock
    prog[3] = 0xa027;       // mov x, osr
    prog[4] = 0xe05f;       // set y, 31
    if (left_justified) {
        prog[5] = 0x6001;   // out pins, 1
        prog[6] = wait_1_bclk;
        prog[7] = wait_0_bclk;
    } else {
        prog[5] = wait_1_bclk;
        prog[6] = wait_0_bclk;
        prog[7] = 0x6001;   // out pins, 1
    }
    prog[8] = 0x0080 | 5;   // jmp y--, 5
}

// `wait gpio` indices are relative to the PIO's GPIO base, which
// rp2pio_statemachine_construct picks the same way.
static uint8_t i2s_wait_gpio_index(const mcu_pin_obj_t *pin, uint8_t gpio_offset) {
    if (pin->number < gpio_offset) {
        mp_raise_ValueError(MP_ERROR_TEXT("Cannot use GPIO0..15 together with GPIO32..47"));
    }
    return pin->number - gpio_offset;
}

void i2sout_reset(void) {
}

// Caller validates that pins are free.
void common_hal_audiobusio_i2sout_construct(audiobusio_i2sout_obj_t *self,
    const mcu_pin_obj_t *bit_clock, const mcu_pin_obj_t *word_select,
    const mcu_pin_obj_t *data, const mcu_pin_obj_t *main_clock, bool left_justified,
    bool external_clock) {
    // Ensure the object starts in its deinit state, so its finaliser does
    // nothing if this raises before the state machine is constructed.
    common_hal_rp2pio_statemachine_mark_deinit(&self->state_machine);

    if (main_clock != NULL) {
        mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("%q"), MP_QSTR_main_clock);
    }
    const mcu_pin_obj_t *sideset_pin = NULL;
    const uint16_t *program = NULL;
    size_t program_len = 0;
    uint16_t ext_clock_program[I2S_EXT_CLOCK_PROGRAM_LEN];
    pio_pinmask_t wait_gpio_mask = PIO_PINMASK_NONE;

    self->external_clock = external_clock;

    if (external_clock) {
        // In external clock mode the clocks are `wait gpio` targets, so they
        // need not be sequential GPIOs.
        uint8_t gpio_offset = 0;
        #if NUM_BANK0_GPIOS > 32
        if (bit_clock->number >= 32 || word_select->number >= 32 || data->number >= 32) {
            gpio_offset = 16;
        }
        #endif
        build_i2sout_ext_clock_program(ext_clock_program,
            i2s_wait_gpio_index(bit_clock, gpio_offset),
            i2s_wait_gpio_index(word_select, gpio_offset),
            left_justified);
        program = ext_clock_program;
        program_len = I2S_EXT_CLOCK_PROGRAM_LEN;
        wait_gpio_mask = PIO_PINMASK_OR(PIO_PINMASK_FROM_PIN(bit_clock->number),
            PIO_PINMASK_FROM_PIN(word_select->number));
    } else if (bit_clock->number == word_select->number - 1) {
        sideset_pin = bit_clock;

        if (left_justified) {
            program_len = MP_ARRAY_SIZE(i2s_program_left_justified);
            program = i2s_program_left_justified;
        } else {
            program_len = MP_ARRAY_SIZE(i2s_program);
            program = i2s_program;
        }

    } else if (bit_clock->number == word_select->number + 1) {
        sideset_pin = word_select;

        if (left_justified) {
            program_len = MP_ARRAY_SIZE(i2s_program_left_justified_swap);
            program = i2s_program_left_justified_swap;
        } else {
            program_len = MP_ARRAY_SIZE(i2s_program_swap);
            program = i2s_program_swap;
        }

    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("Bit clock and word select must be sequential GPIO pins"));
    }

    // Use the state machine to manage pins.
    common_hal_rp2pio_statemachine_construct(
        &self->state_machine,
        program, program_len,
        // Clock at 44.1 khz to warm the DAC up. In external clock mode the SM
        // is driven by the waits, not the clock divider, so run it at sysclk.
        external_clock ? 0 : 44100 * 32 * 6,
        NULL, 0, // init
        NULL, 0, // may_exec
        data, 1, PIO_PINMASK32_NONE, PIO_PINMASK32_ALL, // out pin
        NULL, 0, // in pins
        PIO_PINMASK32_NONE, PIO_PINMASK32_NONE, // in pulls
        NULL, 0, PIO_PINMASK32_NONE, PIO_PINMASK32_FROM_VALUE(0x1f), // set pins
        sideset_pin, sideset_pin == NULL ? 0 : 2, false, PIO_PINMASK32_NONE, PIO_PINMASK32_FROM_VALUE(0x1f), // sideset pins
        false, // No sideset enable
        NULL, PULL_NONE, // jump pin
        wait_gpio_mask, // wait gpio pins
        // The clocks are shared through wait_gpio_mask, which _check_gpio_mask_free
        // already allows to be shared; the data pin stays exclusively ours.
        true, // exclusive pin use
        false, 32, false, // shift out left to start with MSB
        false, // Wait for txstall
        false, 32, false, // in settings
        false, // Not user-interruptible.
        external_clock ? I2S_EXT_CLOCK_WRAP_TARGET : 0,
        external_clock ? I2S_EXT_CLOCK_WRAP : -1, // wrap settings
        PIO_ANY_OFFSET,
        PIO_FIFO_TYPE_DEFAULT,
        PIO_MOV_STATUS_DEFAULT,
        PIO_MOV_N_DEFAULT
        );

    self->playing = false;
    audio_dma_init(&self->dma);
}

bool common_hal_audiobusio_i2sout_deinited(audiobusio_i2sout_obj_t *self) {
    return common_hal_rp2pio_statemachine_deinited(&self->state_machine);
}

void common_hal_audiobusio_i2sout_deinit(audiobusio_i2sout_obj_t *self) {
    if (common_hal_audiobusio_i2sout_deinited(self)) {
        return;
    }

    if (common_hal_audiobusio_i2sout_get_playing(self)) {
        common_hal_audiobusio_i2sout_stop(self);
    }

    common_hal_rp2pio_statemachine_deinit(&self->state_machine);

    audio_dma_deinit(&self->dma);
}

void common_hal_audiobusio_i2sout_play(audiobusio_i2sout_obj_t *self,
    mp_obj_t sample, bool loop) {
    if (common_hal_audiobusio_i2sout_get_playing(self)) {
        common_hal_audiobusio_i2sout_stop(self);
    }

    audiosample_check(sample);

    uint8_t bits_per_sample = audiosample_get_bits_per_sample(sample);
    // Make sure we transmit a minimum of 16 bits.
    // TODO: Maybe we need an intermediate object to upsample instead. This is
    // only needed for some I2S devices that expect at least 8.
    if (bits_per_sample < 16) {
        bits_per_sample = 16;
    }
    // We always output stereo so output twice as many bits.
    uint16_t bits_per_sample_output = bits_per_sample * 2;
    size_t clocks_per_bit = 6;
    uint32_t frequency = bits_per_sample_output * audiosample_get_sample_rate(sample);
    uint8_t channel_count = audiosample_get_channel_count(sample);
    if (channel_count > 2) {
        mp_raise_ValueError(MP_ERROR_TEXT("Too many channels in sample."));
    }

    // An external clock can't be retimed: the sample rate has to match whatever
    // the outside world is running WS at, or the pitch is wrong. The restart
    // still matters -- it re-execs the program at its offset, so every play()
    // re-syncs to WS.
    if (!self->external_clock) {
        common_hal_rp2pio_statemachine_set_frequency(&self->state_machine, clocks_per_bit * frequency);
    }
    common_hal_rp2pio_statemachine_restart(&self->state_machine);

    // On the RP2040, output registers are always written with a 32-bit write.
    // If the write is 8 or 16 bits wide, the data will be replicated in upper bytes.
    // See section 2.1.4 Narrow IO Register Writes in the RP2040 datasheet.
    // This means that identical 16-bit audio data will be written in both halves of the incoming PIO
    // FIFO register. Thus we get mono-to-stereo conversion for the I2S output for free.
    audio_dma_result result = audio_dma_setup_playback(
        &self->dma,
        sample,
        loop,
        false, // single channel
        0, // audio channel
        true,  // output signed
        bits_per_sample,
        (uint32_t)&self->state_machine.pio->txf[self->state_machine.state_machine],  // output register
        self->state_machine.tx_dreq, // data request line
        false); // swap channel

    if (result == AUDIO_DMA_DMA_BUSY) {
        common_hal_audiobusio_i2sout_stop(self);
        mp_raise_RuntimeError(MP_ERROR_TEXT("No DMA channel found"));
    } else if (result == AUDIO_DMA_MEMORY_ERROR) {
        common_hal_audiobusio_i2sout_stop(self);
        mp_raise_RuntimeError(MP_ERROR_TEXT("Unable to allocate buffers for signed conversion"));
    } else if (result == AUDIO_DMA_SOURCE_ERROR) {
        common_hal_audiobusio_i2sout_stop(self);
        mp_raise_RuntimeError(MP_ERROR_TEXT("Audio source error"));
    }

    self->playing = true;
}

void common_hal_audiobusio_i2sout_pause(audiobusio_i2sout_obj_t *self) {
    audio_dma_pause(&self->dma);
}

void common_hal_audiobusio_i2sout_resume(audiobusio_i2sout_obj_t *self) {
    // Maybe: Clear any overrun/underrun errors

    audio_dma_resume(&self->dma);
}

bool common_hal_audiobusio_i2sout_get_paused(audiobusio_i2sout_obj_t *self) {
    return audio_dma_get_paused(&self->dma);
}

void common_hal_audiobusio_i2sout_stop(audiobusio_i2sout_obj_t *self) {
    audio_dma_stop(&self->dma);

    common_hal_rp2pio_statemachine_stop(&self->state_machine);

    self->playing = false;
}

bool common_hal_audiobusio_i2sout_get_playing(audiobusio_i2sout_obj_t *self) {
    bool playing = audio_dma_get_playing(&self->dma);
    if (!playing && self->playing) {
        common_hal_audiobusio_i2sout_stop(self);
    }
    return playing;
}
