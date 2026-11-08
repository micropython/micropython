// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tod Kurt
//
// SPDX-License-Identifier: MIT
//
// MCP4822 dual-channel 12-bit SPI DAC audio output.
// Uses PIO + DMA for non-blocking audio playback, mirroring audiobusio.I2SOut.

#include <stdint.h>
#include <string.h>

#include "mpconfigport.h"

#include "py/gc.h"
#include "py/mperrno.h"
#include "py/runtime.h"
#include "common-hal/mcp4822/MCP4822.h"
#include "shared-bindings/mcp4822/MCP4822.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-module/audiocore/__init__.h"
#include "bindings/rp2pio/StateMachine.h"

// ─────────────────────────────────────────────────────────────────────────────
// PIO program for MCP4822 SPI DAC
// ─────────────────────────────────────────────────────────────────────────────
//
// Pin assignment:
//   OUT pin (1)       = MOSI  — serial data out
//   SET pins (N)      = MOSI through CS — for CS control & command-bit injection
//   SIDE-SET pin (1)  = SCK   — serial clock
//
// SET PINS bit mapping (bit0=MOSI, ..., bit N=CS):
//   0 = CS low,  MOSI low    1 = CS low,  MOSI high
//   (1 << cs_bit_pos) = CS high, MOSI low
//
// SIDE-SET (1 pin, SCK): side 0 = SCK low, side 1 = SCK high
//
// MCP4822 16-bit command word:
//   [15]    channel (0=A, 1=B)   [14] don't care   [13] gain (1=1x, 0=2x)
//   [12]    output enable (1)    [11:0] 12-bit data
//
// DMA feeds unsigned 16-bit audio samples.  RP2040 narrow-write replication
// fills both halves of the 32-bit PIO FIFO entry with the same value,
// giving mono→stereo for free.
//
// The PIO pulls 32 bits, then sends two SPI transactions:
//   Channel A: cmd nibble, then all 16 sample bits from upper half-word
//   Channel B: cmd nibble, then all 16 sample bits from lower half-word
// The MCP4822 captures exactly 16 bits per CS frame (4 cmd + 12 data),
// so only the top 12 of the 16 sample bits become DAC data.  The bottom
// 4 sample bits clock out harmlessly after the DAC has latched.
// This gives correct 16-bit → 12-bit scaling (effectively sample >> 4).
//
// PIO instruction encoding with .side_set 1 (no opt):
//   [15:13] opcode   [12] side-set   [11:8] delay   [7:0] operands
//
// Total: 26 instructions, 86 PIO clocks per audio sample.
// ─────────────────────────────────────────────────────────────────────────────

static const uint16_t mcp4822_pio_program[] = {
    //                                   side SCK
    //  0: pull noblock       side 0   ; Get 32 bits or keep X if FIFO empty
    0x8080,
    //  1: mov x, osr         side 0   ; Save for pull-noblock fallback
    0xA027,

    // ── Channel A: command nibble 0b0011 (1x gain) ────────────────────────
    //  2: set pins, 0        side 0   ; CS low, MOSI=0 (bit15=0: channel A)
    0xE000,
    //  3: nop                side 1   ; SCK high — latch bit 15
    0xB042,
    //  4: set pins, 0        side 0   ; MOSI=0 (bit14=0: don't care)
    0xE000,
    //  5: nop                side 1   ; SCK high
    0xB042,
    //  6: set pins, 1        side 0   ; MOSI=1 (bit13=1: gain 1x)  [patched for 2x]
    0xE001,
    //  7: nop                side 1   ; SCK high
    0xB042,
    //  8: set pins, 1        side 0   ; MOSI=1 (bit12=1: output active)
    0xE001,
    //  9: nop                side 1   ; SCK high
    0xB042,
    // 10: set y, 15          side 0   ; Loop counter: 16 sample bits
    0xE04F,
    // 11: out pins, 1        side 0   ; Data bit → MOSI; SCK low  (bitloopA)
    0x6001,
    // 12: jmp y--, 11        side 1   ; SCK high, loop back
    0x108B,
    // 13: set pins, 4        side 0   ; CS high — DAC A latches
    0xE004,

    // ── Channel B: command nibble 0b1011 (1x gain) ────────────────────────
    // 14: set pins, 1        side 0   ; CS low, MOSI=1 (bit15=1: channel B)
    0xE001,
    // 15: nop                side 1   ; SCK high
    0xB042,
    // 16: set pins, 0        side 0   ; MOSI=0 (bit14=0)
    0xE000,
    // 17: nop                side 1   ; SCK high
    0xB042,
    // 18: set pins, 1        side 0   ; MOSI=1 (bit13=1: gain 1x)  [patched for 2x]
    0xE001,
    // 19: nop                side 1   ; SCK high
    0xB042,
    // 20: set pins, 1        side 0   ; MOSI=1 (bit12=1: output active)
    0xE001,
    // 21: nop                side 1   ; SCK high
    0xB042,
    // 22: set y, 15          side 0   ; Loop counter: 16 sample bits
    0xE04F,
    // 23: out pins, 1        side 0   ; Data bit → MOSI; SCK low  (bitloopB)
    0x6001,
    // 24: jmp y--, 23        side 1   ; SCK high, loop back
    0x1097,
    // 25: set pins, 4        side 0   ; CS high — DAC B latches
    0xE004,
};

// Clocks per sample: 2 (pull+mov) + 42 (chanA) + 42 (chanB) = 86
// Per channel: 8(4 cmd bits × 2 clks) + 1(set y) + 32(16 bits × 2 clks) + 1(cs high) = 42
#define MCP4822_CLOCKS_PER_SAMPLE 86

// MCP4822 gain bit (bit 13) position in the PIO program:
//   Instruction 6  = channel A gain bit
//   Instruction 18 = channel B gain bit
// 1x gain: set pins, 1 (0xE001) — bit 13 = 1
// 2x gain: set pins, 0 (0xE000) — bit 13 = 0
#define MCP4822_PIO_GAIN_INSTR_A 6
#define MCP4822_PIO_GAIN_INSTR_B 18
#define MCP4822_PIO_GAIN_1X 0xE001  // set pins, 1
#define MCP4822_PIO_GAIN_2X 0xE000  // set pins, 0

void mcp4822_reset(void) {
}

// Caller validates that pins are free.
void common_hal_mcp4822_mcp4822_construct(mcp4822_mcp4822_obj_t *self,
    const mcu_pin_obj_t *clock, const mcu_pin_obj_t *mosi,
    const mcu_pin_obj_t *cs, uint8_t gain) {
    // Ensure the object starts in its deinit state, so its finaliser does
    // nothing if this raises before the state machine is constructed.
    common_hal_rp2pio_statemachine_mark_deinit(&self->state_machine);

    // The SET pin group spans from MOSI to CS.
    // MOSI must have a lower GPIO number than CS, gap at most 4.
    if (cs->number <= mosi->number || (cs->number - mosi->number) > 4) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("Invalid %q and %q"), MP_QSTR_CS, MP_QSTR_MOSI);
    }

    uint8_t set_count = cs->number - mosi->number + 1;

    // Build a mutable copy of the PIO program and patch the gain bit
    uint16_t program[MP_ARRAY_SIZE(mcp4822_pio_program)];
    memcpy(program, mcp4822_pio_program, sizeof(mcp4822_pio_program));
    uint16_t gain_instr = (gain == 2) ? MCP4822_PIO_GAIN_2X : MCP4822_PIO_GAIN_1X;
    program[MCP4822_PIO_GAIN_INSTR_A] = gain_instr;
    program[MCP4822_PIO_GAIN_INSTR_B] = gain_instr;

    // Initial SET pin state: CS high (bit at CS position), others low
    uint32_t cs_bit_position = cs->number - mosi->number;
    pio_pinmask32_t initial_set_state = PIO_PINMASK32_FROM_VALUE(1u << cs_bit_position);
    pio_pinmask32_t initial_set_dir = PIO_PINMASK32_FROM_VALUE((1u << set_count) - 1);

    common_hal_rp2pio_statemachine_construct(
        &self->state_machine,
        program, MP_ARRAY_SIZE(mcp4822_pio_program),
        44100 * MCP4822_CLOCKS_PER_SAMPLE,          // Initial frequency; play() adjusts
        NULL, 0,                                     // No init program
        NULL, 0,                                     // No may_exec
        mosi, 1,                                     // OUT: MOSI, 1 pin
        PIO_PINMASK32_NONE, PIO_PINMASK32_ALL,      // OUT state=low, dir=output
        NULL, 0,                                     // IN: none
        PIO_PINMASK32_NONE, PIO_PINMASK32_NONE,     // IN pulls: none
        mosi, set_count,                             // SET: MOSI..CS
        initial_set_state, initial_set_dir,          // SET state (CS high), dir=output
        clock, 1, false,                             // SIDE-SET: SCK, 1 pin, not pindirs
        PIO_PINMASK32_NONE,                          // SIDE-SET state: SCK low
        PIO_PINMASK32_FROM_VALUE(0x1),               // SIDE-SET dir: output
        false,                                       // No sideset enable
        NULL, PULL_NONE,                             // No jump pin
        PIO_PINMASK_NONE,                            // No wait GPIO
        true,                                        // Exclusive pin use
        false, 32, false,                            // OUT shift: no autopull, 32-bit, shift left
        false,                                       // Don't wait for txstall
        false, 32, false,                            // IN shift (unused)
        false,                                       // Not user-interruptible
        0, -1,                                       // Wrap: whole program
        PIO_ANY_OFFSET,
        PIO_FIFO_TYPE_DEFAULT,
        PIO_MOV_STATUS_DEFAULT,
        PIO_MOV_N_DEFAULT
        );

    self->playing = false;
    audio_dma_init(&self->dma);
}

bool common_hal_mcp4822_mcp4822_deinited(mcp4822_mcp4822_obj_t *self) {
    return common_hal_rp2pio_statemachine_deinited(&self->state_machine);
}

void common_hal_mcp4822_mcp4822_deinit(mcp4822_mcp4822_obj_t *self) {
    if (common_hal_mcp4822_mcp4822_deinited(self)) {
        return;
    }
    if (common_hal_mcp4822_mcp4822_get_playing(self)) {
        common_hal_mcp4822_mcp4822_stop(self);
    }
    common_hal_rp2pio_statemachine_deinit(&self->state_machine);
    audio_dma_deinit(&self->dma);
}

void common_hal_mcp4822_mcp4822_play(mcp4822_mcp4822_obj_t *self,
    mp_obj_t sample, bool loop) {

    if (common_hal_mcp4822_mcp4822_get_playing(self)) {
        common_hal_mcp4822_mcp4822_stop(self);
    }

    audiosample_check(sample);

    uint8_t bits_per_sample = audiosample_get_bits_per_sample(sample);
    if (bits_per_sample < 16) {
        bits_per_sample = 16;
    }

    uint32_t sample_rate = audiosample_get_sample_rate(sample);
    uint8_t channel_count = audiosample_get_channel_count(sample);
    if (channel_count > 2) {
        mp_raise_ValueError(MP_ERROR_TEXT("Too many channels in sample."));
    }

    // PIO clock = sample_rate × clocks_per_sample
    common_hal_rp2pio_statemachine_set_frequency(
        &self->state_machine,
        (uint32_t)sample_rate * MCP4822_CLOCKS_PER_SAMPLE);
    common_hal_rp2pio_statemachine_restart(&self->state_machine);

    audio_dma_result result = audio_dma_setup_playback(
        &self->dma,
        sample,
        loop,
        false,              // single_channel_output
        0,                  // audio_channel
        false,              // output_signed = false (unsigned for MCP4822)
        bits_per_sample,    // output_resolution
        (uint32_t)&self->state_machine.pio->txf[self->state_machine.state_machine],
        self->state_machine.tx_dreq,
        false);             // swap_channel

    if (result == AUDIO_DMA_DMA_BUSY) {
        common_hal_mcp4822_mcp4822_stop(self);
        mp_raise_RuntimeError(MP_ERROR_TEXT("No DMA channel found"));
    } else if (result == AUDIO_DMA_MEMORY_ERROR) {
        common_hal_mcp4822_mcp4822_stop(self);
        mp_raise_RuntimeError(MP_ERROR_TEXT("Unable to allocate buffers for signed conversion"));
    } else if (result == AUDIO_DMA_SOURCE_ERROR) {
        common_hal_mcp4822_mcp4822_stop(self);
        mp_raise_RuntimeError(MP_ERROR_TEXT("Audio source error"));
    }

    self->playing = true;
}

void common_hal_mcp4822_mcp4822_pause(mcp4822_mcp4822_obj_t *self) {
    audio_dma_pause(&self->dma);
}

void common_hal_mcp4822_mcp4822_resume(mcp4822_mcp4822_obj_t *self) {
    audio_dma_resume(&self->dma);
}

bool common_hal_mcp4822_mcp4822_get_paused(mcp4822_mcp4822_obj_t *self) {
    return audio_dma_get_paused(&self->dma);
}

void common_hal_mcp4822_mcp4822_stop(mcp4822_mcp4822_obj_t *self) {
    audio_dma_stop(&self->dma);
    common_hal_rp2pio_statemachine_stop(&self->state_machine);
    self->playing = false;
}

bool common_hal_mcp4822_mcp4822_get_playing(mcp4822_mcp4822_obj_t *self) {
    bool playing = audio_dma_get_playing(&self->dma);
    if (!playing && self->playing) {
        common_hal_mcp4822_mcp4822_stop(self);
    }
    return playing;
}
