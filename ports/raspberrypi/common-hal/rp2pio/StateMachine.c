/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
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

#include "bindings/rp2pio/StateMachine.h"

#include "common-hal/microcontroller/__init__.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"

#include "src/rp2040/hardware_regs/include/hardware/platform_defs.h"
#include "src/rp2_common/hardware_clocks/include/hardware/clocks.h"
#include "src/rp2_common/hardware_dma/include/hardware/dma.h"
#include "src/rp2_common/hardware_pio/include/hardware/pio_instructions.h"
#include "src/rp2040/hardware_structs/include/hardware/structs/iobank0.h"
#include "src/rp2_common/hardware_irq/include/hardware/irq.h"

#include "lib/utils/interrupt_char.h"
#include "py/obj.h"
#include "py/objproperty.h"
#include "py/runtime.h"

// Count how many state machines are using each pin.
STATIC uint8_t _pin_reference_count[TOTAL_GPIO_COUNT];
STATIC uint32_t _current_program_id[NUM_PIOS][NUM_PIO_STATE_MACHINES];
STATIC uint8_t _current_program_offset[NUM_PIOS][NUM_PIO_STATE_MACHINES];
STATIC uint8_t _current_program_len[NUM_PIOS][NUM_PIO_STATE_MACHINES];
STATIC bool _never_reset[NUM_PIOS][NUM_PIO_STATE_MACHINES];

STATIC uint32_t _current_pins[NUM_PIOS];
STATIC uint32_t _current_sm_pins[NUM_PIOS][NUM_PIO_STATE_MACHINES];

STATIC PIO pio_instances[2] = {pio0, pio1};
typedef void (*interrupt_handler_type)(void *);
STATIC interrupt_handler_type _interrupt_handler[NUM_PIOS][NUM_PIO_STATE_MACHINES];
STATIC void *_interrupt_arg[NUM_PIOS][NUM_PIO_STATE_MACHINES];

STATIC void rp2pio_statemachine_interrupt_handler(void);

static void rp2pio_statemachine_set_pull(uint32_t pull_pin_up, uint32_t pull_pin_down, uint32_t pins_we_use) {
    for (int i = 0; i < TOTAL_GPIO_COUNT; i++) {
        bool used = pins_we_use & (1 << i);
        if (used) {
            bool pull_up = pull_pin_up & (1 << i);
            bool pull_down = pull_pin_down & (1 << i);
            gpio_set_pulls(i, pull_up, pull_down);
        }
    }
}

void _reset_statemachine(PIO pio, uint8_t sm, bool leave_pins) {
    uint8_t pio_index = pio_get_index(pio);
    uint32_t program_id = _current_program_id[pio_index][sm];
    if (program_id == 0) {
        return;
    }
    _current_program_id[pio_index][sm] = 0;
    bool program_in_use = false;
    for (size_t i = 0; i < NUM_PIO_STATE_MACHINES; i++) {
        if (_current_program_id[pio_index][i] == program_id) {
            program_in_use = true;
            break;
        }
    }
    if (!program_in_use) {
        uint8_t offset = _current_program_offset[pio_index][sm];
        pio_program_t program_struct = {
            .length = _current_program_len[pio_index][sm]
        };
        pio_remove_program(pio, &program_struct, offset);
    }

    uint32_t pins = _current_sm_pins[pio_index][sm];
    for (size_t pin_number = 0; pin_number < TOTAL_GPIO_COUNT; pin_number++) {
        if ((pins & (1 << pin_number)) == 0) {
            continue;
        }
        _pin_reference_count[pin_number]--;
        if (_pin_reference_count[pin_number] == 0) {
            if (!leave_pins) {
                reset_pin_number(pin_number);
            }
            _current_pins[pio_index] &= ~(1 << pin_number);
        }
    }
    _current_sm_pins[pio_index][sm] = 0;
    pio->inte0 &= ~((PIO_IRQ0_INTF_SM0_RXNEMPTY_BITS | PIO_IRQ0_INTF_SM0_TXNFULL_BITS | PIO_IRQ0_INTF_SM0_BITS) << sm);
    pio_sm_unclaim(pio, sm);
}

void reset_rp2pio_statemachine(void) {
    for (size_t i = 0; i < NUM_PIOS; i++) {
        PIO pio = pio_instances[i];
        for (size_t j = 0; j < NUM_PIO_STATE_MACHINES; j++) {
            if (_never_reset[i][j]) {
                continue;
            }
            _reset_statemachine(pio, j, false);
        }
    }
    for (uint8_t irq = PIO0_IRQ_0; irq <= PIO1_IRQ_1; irq++) {
        irq_handler_t int_handler = irq_get_exclusive_handler(irq);
        if (int_handler > 0) {
            irq_set_enabled(irq, false);
            irq_remove_handler(irq,int_handler);
        }
    }
}

STATIC uint32_t _check_pins_free(const mcu_pin_obj_t *first_pin, uint8_t pin_count, bool exclusive_pin_use) {
    uint32_t pins_we_use = 0;
    if (first_pin != NULL) {
        for (size_t i = 0; i < pin_count; i++) {
            uint8_t pin_number = first_pin->number + i;
            if (pin_number >= TOTAL_GPIO_COUNT) {
                mp_raise_ValueError(translate("Pin count too large"));
            }
            const mcu_pin_obj_t *pin = mcu_pin_global_dict_table[pin_number].value;
            if (exclusive_pin_use || _pin_reference_count[pin_number] == 0) {
                assert_pin_free(pin);
            }
            pins_we_use |= 1 << pin_number;
        }
    }
    return pins_we_use;
}


bool rp2pio_statemachine_construct(rp2pio_statemachine_obj_t *self,
    const uint16_t *program, size_t program_len,
    size_t frequency,
    const uint16_t *init, size_t init_len,
    const mcu_pin_obj_t *first_out_pin, uint8_t out_pin_count,
    const mcu_pin_obj_t *first_in_pin, uint8_t in_pin_count,
    uint32_t pull_pin_up, uint32_t pull_pin_down,
    const mcu_pin_obj_t *first_set_pin, uint8_t set_pin_count,
    const mcu_pin_obj_t *first_sideset_pin, uint8_t sideset_pin_count,
    uint32_t initial_pin_state, uint32_t initial_pin_direction,
    uint32_t pins_we_use, bool tx_fifo, bool rx_fifo,
    bool auto_pull, uint8_t pull_threshold, bool out_shift_right,
    bool wait_for_txstall,
    bool auto_push, uint8_t push_threshold, bool in_shift_right,
    bool claim_pins) {
    // Create a program id that isn't the pointer so we can store it without storing the original object.
    uint32_t program_id = ~((uint32_t)program);

    // Next, find a PIO and state machine to use.
    size_t pio_index = NUM_PIOS;
    uint8_t program_offset = 32;
    pio_program_t program_struct = {
        .instructions = (uint16_t *)program,
        .length = program_len,
        .origin = -1
    };
    for (size_t i = 0; i < NUM_PIOS; i++) {
        PIO pio = pio_instances[i];
        uint8_t free_count = 0;
        for (size_t j = 0; j < NUM_PIO_STATE_MACHINES; j++) {
            if (_current_program_id[i][j] == program_id &&
                _current_program_len[i][j] == program_len) {
                program_offset = _current_program_offset[i][j];
            }
            int temp_claim = pio_claim_unused_sm(pio, false);
            if (temp_claim >= 0) {
                pio_sm_unclaim(pio, temp_claim);
                free_count++;
            }
        }
        if (free_count > 0 && (program_offset < 32 || pio_can_add_program(pio, &program_struct))) {
            pio_index = i;
            if (program_offset < 32) {
                break;
            }
        }
        // Reset program offset if we weren't able to find a free state machine
        // on that PIO. (We would have broken the loop otherwise.)
        program_offset = 32;
    }

    int state_machine = -1;
    if (pio_index < NUM_PIOS) {
        PIO pio = pio_instances[pio_index];
        for (size_t i = 0; i < NUM_PIOS; i++) {
            if (i == pio_index) {
                continue;
            }
            if ((_current_pins[i] & pins_we_use) != 0) {
                // Pin in use by another PIO already.
                return false;
            }
        }
        state_machine = pio_claim_unused_sm(pio, false);
    }
    if (pio_index == NUM_PIOS || state_machine < 0 || state_machine >= NUM_PIO_STATE_MACHINES) {
        return false;
    }

    self->pio = pio_instances[pio_index];
    self->state_machine = state_machine;
    if (program_offset == 32) {
        program_offset = pio_add_program(self->pio, &program_struct);
    }
    self->offset = program_offset;
    _current_program_id[pio_index][state_machine] = program_id;
    _current_program_len[pio_index][state_machine] = program_len;
    _current_program_offset[pio_index][state_machine] = program_offset;
    _current_sm_pins[pio_index][state_machine] = pins_we_use;
    _current_pins[pio_index] |= pins_we_use;

    pio_sm_set_pins_with_mask(self->pio, state_machine, initial_pin_state, pins_we_use);
    pio_sm_set_pindirs_with_mask(self->pio, state_machine, initial_pin_direction, pins_we_use);
    rp2pio_statemachine_set_pull(pull_pin_up, pull_pin_down, pins_we_use);
    self->initial_pin_state = initial_pin_state;
    self->initial_pin_direction = initial_pin_direction;
    self->pull_pin_up = pull_pin_up;
    self->pull_pin_down = pull_pin_down;

    for (size_t pin_number = 0; pin_number < TOTAL_GPIO_COUNT; pin_number++) {
        if ((pins_we_use & (1 << pin_number)) == 0) {
            continue;
        }
        _pin_reference_count[pin_number]++;
        const mcu_pin_obj_t *pin = mcu_pin_global_dict_table[pin_number].value;
        // Also claim the pin at the top level when we're the first to grab it.
        if (_pin_reference_count[pin_number] == 1) {
            if (claim_pins) {
                claim_pin(pin);
            }
            pio_gpio_init(self->pio, pin_number);
        }
    }

    pio_sm_config c = {0, 0, 0};

    if (frequency == 0) {
        frequency = clock_get_hz(clk_sys);
    }
    uint64_t frequency256 = ((uint64_t)clock_get_hz(clk_sys)) * 256;
    uint64_t div256 = frequency256 / frequency;
    if (frequency256 % div256 > 0) {
        div256 += 1;
    }
    self->actual_frequency = frequency256 / div256;
    sm_config_set_clkdiv_int_frac(&c, div256 / 256, div256 % 256);

    if (first_out_pin != NULL) {
        sm_config_set_out_pins(&c, first_out_pin->number, out_pin_count);
    }
    if (first_in_pin != NULL) {
        sm_config_set_in_pins(&c, first_in_pin->number);
    }
    if (first_set_pin != NULL) {
        sm_config_set_set_pins(&c, first_set_pin->number, set_pin_count);
    }
    if (first_sideset_pin != NULL) {
        sm_config_set_sideset(&c, sideset_pin_count, false /* optional */, false /* pin direction */);
        sm_config_set_sideset_pins(&c, first_sideset_pin->number);
    }
    sm_config_set_wrap(&c, program_offset, program_offset + program_len - 1);
    sm_config_set_in_shift(&c, in_shift_right, auto_push, push_threshold);
    sm_config_set_out_shift(&c, out_shift_right, auto_pull, pull_threshold);

    enum pio_fifo_join join = PIO_FIFO_JOIN_NONE;
    if (!rx_fifo) {
        join = PIO_FIFO_JOIN_TX;
    } else if (!tx_fifo) {
        join = PIO_FIFO_JOIN_RX;
    }
    if (rx_fifo) {
        self->rx_dreq = pio_get_dreq(self->pio, self->state_machine, false);
    }
    if (tx_fifo) {
        self->tx_dreq = pio_get_dreq(self->pio, self->state_machine, true);
    }
    self->in = rx_fifo;
    self->out = tx_fifo;
    self->out_shift_right = out_shift_right;
    self->in_shift_right = in_shift_right;
    self->wait_for_txstall = wait_for_txstall;

    self->init = init;
    self->init_len = init_len;

    sm_config_set_fifo_join(&c, join);
    self->sm_config = c;

    pio_sm_init(self->pio, self->state_machine, program_offset, &c);
    common_hal_rp2pio_statemachine_run(self, init, init_len);

    common_hal_rp2pio_statemachine_set_frequency(self, frequency);
    pio_sm_set_enabled(self->pio, self->state_machine, true);
    return true;
}

static uint32_t mask_and_rotate(const mcu_pin_obj_t *first_pin, uint32_t bit_count, uint32_t value) {
    value = value & ((1 << bit_count) - 1);
    uint32_t shift = first_pin->number;
    return value << shift | value >> (32 - shift);
}

void common_hal_rp2pio_statemachine_construct(rp2pio_statemachine_obj_t *self,
    const uint16_t *program, size_t program_len,
    size_t frequency,
    const uint16_t *init, size_t init_len,
    const mcu_pin_obj_t *first_out_pin, uint8_t out_pin_count, uint32_t initial_out_pin_state, uint32_t initial_out_pin_direction,
    const mcu_pin_obj_t *first_in_pin, uint8_t in_pin_count,
    uint32_t pull_pin_up, uint32_t pull_pin_down,
    const mcu_pin_obj_t *first_set_pin, uint8_t set_pin_count, uint32_t initial_set_pin_state, uint32_t initial_set_pin_direction,
    const mcu_pin_obj_t *first_sideset_pin, uint8_t sideset_pin_count, uint32_t initial_sideset_pin_state, uint32_t initial_sideset_pin_direction,
    uint32_t wait_gpio_mask,
    bool exclusive_pin_use,
    bool auto_pull, uint8_t pull_threshold, bool out_shift_right,
    bool wait_for_txstall,
    bool auto_push, uint8_t push_threshold, bool in_shift_right) {

    // First, check that all pins are free OR already in use by any PIO if exclusive_pin_use is false.
    uint32_t pins_we_use = wait_gpio_mask;
    pins_we_use |= _check_pins_free(first_out_pin, out_pin_count, exclusive_pin_use);
    pins_we_use |= _check_pins_free(first_in_pin, in_pin_count, exclusive_pin_use);
    pins_we_use |= _check_pins_free(first_set_pin, set_pin_count, exclusive_pin_use);
    pins_we_use |= _check_pins_free(first_sideset_pin, sideset_pin_count, exclusive_pin_use);

    // Look through the program to see what we reference and make sure it was provided.
    bool tx_fifo = false;
    bool rx_fifo = false;
    bool in_loaded = false; // can be loaded in other ways besides the fifo
    bool out_loaded = false;
    bool in_used = false;
    bool out_used = false;
    for (size_t i = 0; i < program_len; i++) {
        uint16_t full_instruction = program[i];
        uint16_t instruction = full_instruction & 0xe000;
        if (instruction == 0x8000) {
            if ((full_instruction & 0xe080) == pio_instr_bits_push) {
                rx_fifo = true;
                in_loaded = true;
            } else { // pull otherwise.
                tx_fifo = true;
                out_loaded = true;
            }
        }
        if (instruction == pio_instr_bits_jmp) {
            uint16_t condition = (full_instruction & 0x00e0) >> 5;
            if (condition == 0x6) { // GPIO
                mp_raise_NotImplementedError_varg(translate("Instruction %d jumps on pin"), i);
            }
        }
        if (instruction == pio_instr_bits_wait) {
            uint16_t wait_source = (full_instruction & 0x0060) >> 5;
            uint16_t wait_index = full_instruction & 0x001f;
            if (wait_source == 0 && (pins_we_use & (1 << wait_index)) == 0) { // GPIO
                mp_raise_ValueError_varg(translate("Instruction %d uses extra pin"), i);
            }
            if (wait_source == 1) { // Input pin
                if (first_in_pin == NULL) {
                    mp_raise_ValueError_varg(translate("Missing first_in_pin. Instruction %d waits based on pin"), i);
                }
                if (wait_index > in_pin_count) {
                    mp_raise_ValueError_varg(translate("Instruction %d waits on input outside of count"), i);
                }
            }
        }
        if (instruction == pio_instr_bits_in) {
            uint16_t source = (full_instruction & 0x00e0) >> 5;
            uint16_t bit_count = full_instruction & 0x001f;
            if (source == 0) {
                if (first_in_pin == NULL) {
                    mp_raise_ValueError_varg(translate("Missing first_in_pin. Instruction %d shifts in from pin(s)"), i);
                }
                if (bit_count > in_pin_count) {
                    mp_raise_ValueError_varg(translate("Instruction %d shifts in more bits than pin count"), i);
                }
            }
            if (auto_push) {
                in_loaded = true;
                rx_fifo = true;
            }
            in_used = true;
        }
        if (instruction == pio_instr_bits_out) {
            uint16_t bit_count = full_instruction & 0x001f;
            uint16_t destination = (full_instruction & 0x00e0) >> 5;
            // Check for pins or pindirs destination.
            if (destination == 0x0 || destination == 0x4) {
                if (first_out_pin == NULL) {
                    mp_raise_ValueError_varg(translate("Missing first_out_pin. Instruction %d shifts out to pin(s)"), i);
                }
                if (bit_count > out_pin_count) {
                    mp_raise_ValueError_varg(translate("Instruction %d shifts out more bits than pin count"), i);
                }
            }
            if (auto_pull) {
                out_loaded = true;
                tx_fifo = true;
            }
            out_used = true;
        }
        if (instruction == pio_instr_bits_set) {
            uint16_t destination = (full_instruction & 0x00e0) >> 5;
            // Check for pins or pindirs destination.
            if ((destination == 0x00 || destination == 0x4) && first_set_pin == NULL) {
                mp_raise_ValueError_varg(translate("Missing first_set_pin. Instruction %d sets pin(s)"), i);
            }
        }
        if (instruction == pio_instr_bits_mov) {
            uint16_t source = full_instruction & 0x0007;
            uint16_t destination = (full_instruction & 0x00e0) >> 5;
            // Check for pins or pindirs destination.
            if (destination == 0x0 && first_out_pin == NULL) {
                mp_raise_ValueError_varg(translate("Missing first_out_pin. Instruction %d writes pin(s)"), i);
            }
            if (source == 0x0 && first_in_pin == NULL) {
                mp_raise_ValueError_varg(translate("Missing first_in_pin. Instruction %d reads pin(s)"), i);
            }
            if (destination == 0x6) {
                in_loaded = true;
            } else if (destination == 0x7) {
                out_loaded = true;
            }
        }
    }

    if (!in_loaded && in_used) {
        mp_raise_ValueError_varg(translate("Program does IN without loading ISR"));
    }
    if (!out_loaded && out_used) {
        mp_raise_ValueError_varg(translate("Program does OUT without loading OSR"));
    }

    uint32_t initial_pin_state = mask_and_rotate(first_out_pin, out_pin_count, initial_out_pin_state);
    uint32_t initial_pin_direction = mask_and_rotate(first_out_pin, out_pin_count, initial_out_pin_direction);
    initial_set_pin_state = mask_and_rotate(first_set_pin, set_pin_count, initial_set_pin_state);
    initial_set_pin_direction = mask_and_rotate(first_set_pin, set_pin_count, initial_set_pin_direction);
    uint32_t set_out_overlap = mask_and_rotate(first_out_pin, out_pin_count, 0xffffffff) &
        mask_and_rotate(first_set_pin, set_pin_count, 0xffffffff);
    // Check that OUT and SET settings agree because we don't have a way of picking one over the other.
    if ((initial_pin_state & set_out_overlap) != (initial_set_pin_state & set_out_overlap)) {
        mp_raise_ValueError(translate("Initial set pin state conflicts with initial out pin state"));
    }
    if ((initial_pin_direction & set_out_overlap) != (initial_set_pin_direction & set_out_overlap)) {
        mp_raise_ValueError(translate("Initial set pin direction conflicts with initial out pin direction"));
    }
    initial_pin_state |= initial_set_pin_state;
    initial_pin_direction |= initial_set_pin_direction;

    // Sideset overrides OUT or SET so we always use its values.
    uint32_t sideset_mask = mask_and_rotate(first_sideset_pin, sideset_pin_count, 0x1f);
    initial_pin_state = (initial_pin_state & ~sideset_mask) | mask_and_rotate(first_sideset_pin, sideset_pin_count, initial_sideset_pin_state);
    initial_pin_direction = (initial_pin_direction & ~sideset_mask) | mask_and_rotate(first_sideset_pin, sideset_pin_count, initial_sideset_pin_direction);

    // Deal with pull up/downs
    uint32_t pull_up = mask_and_rotate(first_in_pin, in_pin_count, pull_pin_up);
    uint32_t pull_down = mask_and_rotate(first_in_pin, in_pin_count, pull_pin_down);
    if (initial_pin_direction & (pull_up | pull_down)) {
        mp_raise_ValueError(translate("pull masks conflict with direction masks"));
    }
    bool ok = rp2pio_statemachine_construct(self,
        program, program_len,
        frequency,
        init, init_len,
        first_out_pin, out_pin_count,
        first_in_pin, in_pin_count,
        pull_up, pull_down,
        first_set_pin, set_pin_count,
        first_sideset_pin, sideset_pin_count,
        initial_pin_state, initial_pin_direction,
        pins_we_use, tx_fifo, rx_fifo,
        auto_pull, pull_threshold, out_shift_right,
        wait_for_txstall,
        auto_push, push_threshold, in_shift_right,
        true /* claim pins */);
    if (!ok) {
        mp_raise_RuntimeError(translate("All state machines in use"));
    }
}

void common_hal_rp2pio_statemachine_restart(rp2pio_statemachine_obj_t *self) {
    pio_sm_restart(self->pio, self->state_machine);

    uint8_t pio_index = pio_get_index(self->pio);
    uint32_t pins_we_use = _current_sm_pins[pio_index][self->state_machine];
    pio_sm_set_pins_with_mask(self->pio, self->state_machine, self->initial_pin_state, pins_we_use);
    pio_sm_set_pindirs_with_mask(self->pio, self->state_machine, self->initial_pin_direction, pins_we_use);
    rp2pio_statemachine_set_pull(self->pull_pin_up, self->pull_pin_down, pins_we_use);
    common_hal_rp2pio_statemachine_run(self, self->init, self->init_len);
    pio_sm_set_enabled(self->pio, self->state_machine, true);
}

void common_hal_rp2pio_statemachine_stop(rp2pio_statemachine_obj_t *self) {
    pio_sm_set_enabled(self->pio, self->state_machine, false);
}

void common_hal_rp2pio_statemachine_run(rp2pio_statemachine_obj_t *self, const uint16_t *instructions, size_t len) {
    for (size_t i = 0; i < len; i++) {
        pio_sm_exec(self->pio, self->state_machine, instructions[i]);
    }
}

uint32_t common_hal_rp2pio_statemachine_get_frequency(rp2pio_statemachine_obj_t *self) {
    return self->actual_frequency;
}

void common_hal_rp2pio_statemachine_set_frequency(rp2pio_statemachine_obj_t *self, uint32_t frequency) {
    if (frequency == 0) {
        frequency = clock_get_hz(clk_sys);
    }
    uint64_t frequency256 = ((uint64_t)clock_get_hz(clk_sys)) * 256;
    uint64_t div256 = frequency256 / frequency;
    if (frequency256 % div256 > 0) {
        div256 += 1;
    }
    // 0 is interpreted as 0x10000 so it's valid.
    if (div256 / 256 > 0x10000 || frequency > clock_get_hz(clk_sys)) {
        mp_raise_ValueError_varg(translate("%q out of range"), MP_QSTR_frequency);
    }
    self->actual_frequency = frequency256 / div256;

    pio_sm_set_clkdiv_int_frac(self->pio, self->state_machine, div256 / 256, div256 % 256);
    // Reset the clkdiv counter in case our new TOP is lower.
    pio_sm_clkdiv_restart(self->pio, self->state_machine);
}

void rp2pio_statemachine_deinit(rp2pio_statemachine_obj_t *self, bool leave_pins) {
    uint8_t sm = self->state_machine;
    uint8_t pio_index = pio_get_index(self->pio);
    common_hal_mcu_disable_interrupts();
    _interrupt_arg[pio_index][sm] = NULL;
    _interrupt_handler[pio_index][sm] = NULL;
    common_hal_mcu_enable_interrupts();
    _never_reset[pio_index][sm] = false;
    _reset_statemachine(self->pio, sm, leave_pins);
    self->state_machine = NUM_PIO_STATE_MACHINES;
}

void common_hal_rp2pio_statemachine_deinit(rp2pio_statemachine_obj_t *self) {
    rp2pio_statemachine_deinit(self, false);
}

void common_hal_rp2pio_statemachine_never_reset(rp2pio_statemachine_obj_t *self) {
    uint8_t sm = self->state_machine;
    uint8_t pio_index = pio_get_index(self->pio);
    _never_reset[pio_index][sm] = true;
    // TODO: never reset all the pins
}

bool common_hal_rp2pio_statemachine_deinited(rp2pio_statemachine_obj_t *self) {
    return self->state_machine == NUM_PIO_STATE_MACHINES;
}

enum dma_channel_transfer_size _stride_to_dma_size(uint8_t stride) {
    switch (stride) {
        case 4:
            return DMA_SIZE_32;
        case 2:
            return DMA_SIZE_16;
        case 1:
        default:
            return DMA_SIZE_8;
    }
}

static bool _transfer(rp2pio_statemachine_obj_t *self,
    const uint8_t *data_out, size_t out_len, uint8_t out_stride_in_bytes,
    uint8_t *data_in, size_t in_len, uint8_t in_stride_in_bytes) {
    // This implementation is based on SPI but varies because the tx and rx buffers
    // may be different lengths and occur at different times or speeds.

    // Use DMA for large transfers if channels are available
    const size_t dma_min_size_threshold = 32;
    int chan_tx = -1;
    int chan_rx = -1;
    size_t len = MAX(out_len, in_len);
    bool tx = data_out != NULL;
    bool rx = data_in != NULL;
    if (len >= dma_min_size_threshold) {
        // Use DMA channels to service the two FIFOs
        if (tx) {
            chan_tx = dma_claim_unused_channel(false);
        }
        if (rx) {
            chan_rx = dma_claim_unused_channel(false);
        }
    }
    volatile uint8_t *tx_destination = NULL;
    const volatile uint8_t *rx_source = NULL;
    if (tx) {
        tx_destination = (volatile uint8_t *)&self->pio->txf[self->state_machine];
        if (!self->out_shift_right) {
            tx_destination += 4 - out_stride_in_bytes;
        }
    }
    if (rx) {
        rx_source = (const volatile uint8_t *)&self->pio->rxf[self->state_machine];
        if (self->in_shift_right) {
            rx_source += 4 - in_stride_in_bytes;
        }
    }
    uint32_t stall_mask = 1 << (PIO_FDEBUG_TXSTALL_LSB + self->state_machine);
    bool use_dma = (!rx || chan_rx >= 0) && (!tx || chan_tx >= 0);
    if (use_dma) {
        dma_channel_config c;
        uint32_t channel_mask = 0;
        if (tx) {
            c = dma_channel_get_default_config(chan_tx);
            channel_config_set_transfer_data_size(&c, _stride_to_dma_size(out_stride_in_bytes));
            channel_config_set_dreq(&c, self->tx_dreq);
            channel_config_set_read_increment(&c, true);
            channel_config_set_write_increment(&c, false);
            dma_channel_configure(chan_tx, &c,
                tx_destination,
                data_out,
                out_len / out_stride_in_bytes,
                false);
            channel_mask |= 1u << chan_tx;
        }
        if (rx) {
            c = dma_channel_get_default_config(chan_rx);
            channel_config_set_transfer_data_size(&c, _stride_to_dma_size(in_stride_in_bytes));
            channel_config_set_dreq(&c, self->rx_dreq);
            channel_config_set_read_increment(&c, false);
            channel_config_set_write_increment(&c, true);
            dma_channel_configure(chan_rx, &c,
                data_in,
                rx_source,
                in_len / in_stride_in_bytes,
                false);
            channel_mask |= 1u << chan_rx;
        }

        dma_start_channel_mask(channel_mask);
        while ((rx && dma_channel_is_busy(chan_rx)) ||
               (tx && dma_channel_is_busy(chan_tx))) {
            // TODO: We should idle here until we get a DMA interrupt or something else.
            RUN_BACKGROUND_TASKS;
        }
        // Clear the stall bit so we can detect when the state machine is done transmitting.
        self->pio->fdebug = stall_mask;
    }

    // If we have claimed only one channel successfully, we should release immediately. This also
    // releases the DMA after use_dma has been done.
    if (chan_rx >= 0) {
        dma_channel_unclaim(chan_rx);
    }
    if (chan_tx >= 0) {
        dma_channel_unclaim(chan_tx);
    }

    if (!use_dma) {
        // Use software for small transfers, or if couldn't claim two DMA channels
        size_t rx_remaining = in_len / in_stride_in_bytes;
        size_t tx_remaining = out_len / out_stride_in_bytes;

        while (rx_remaining || tx_remaining) {
            while (tx_remaining && !pio_sm_is_tx_fifo_full(self->pio, self->state_machine)) {
                if (out_stride_in_bytes == 1) {
                    *tx_destination = *data_out;
                } else if (out_stride_in_bytes == 2) {
                    *((uint16_t *)tx_destination) = *((uint16_t *)data_out);
                } else if (out_stride_in_bytes == 4) {
                    *((uint32_t *)tx_destination) = *((uint32_t *)data_out);
                }
                data_out += out_stride_in_bytes;
                --tx_remaining;
            }
            while (rx_remaining && !pio_sm_is_rx_fifo_empty(self->pio, self->state_machine)) {
                if (in_stride_in_bytes == 1) {
                    *data_in = (uint8_t)*rx_source;
                } else if (in_stride_in_bytes == 2) {
                    *((uint16_t *)data_in) = *((uint16_t *)rx_source);
                } else if (in_stride_in_bytes == 4) {
                    *((uint32_t *)data_in) = *((uint32_t *)rx_source);
                }
                data_in += in_stride_in_bytes;
                --rx_remaining;
            }
            RUN_BACKGROUND_TASKS;
        }
        // Clear the stall bit so we can detect when the state machine is done transmitting.
        self->pio->fdebug = stall_mask;
    }
    // Wait for the state machine to finish transmitting the data we've queued
    // up.
    if (tx) {
        while (!pio_sm_is_tx_fifo_empty(self->pio, self->state_machine) ||
               (self->wait_for_txstall && (self->pio->fdebug & stall_mask) == 0)) {
            RUN_BACKGROUND_TASKS;
        }
    }
    return true;
}

// TODO: Provide a way around these checks in case someone wants to use the FIFO
// with manually run code.

bool common_hal_rp2pio_statemachine_write(rp2pio_statemachine_obj_t *self, const uint8_t *data, size_t len, uint8_t stride_in_bytes) {
    if (!self->out) {
        mp_raise_RuntimeError(translate("No out in program"));
    }
    return _transfer(self, data, len, stride_in_bytes, NULL, 0, 0);
}

bool common_hal_rp2pio_statemachine_readinto(rp2pio_statemachine_obj_t *self, uint8_t *data, size_t len, uint8_t stride_in_bytes) {
    if (!self->in) {
        mp_raise_RuntimeError(translate("No in in program"));
    }
    return _transfer(self, NULL, 0, 0, data, len, stride_in_bytes);
}

bool common_hal_rp2pio_statemachine_write_readinto(rp2pio_statemachine_obj_t *self,
    const uint8_t *data_out, size_t out_len, uint8_t out_stride_in_bytes,
    uint8_t *data_in, size_t in_len, uint8_t in_stride_in_bytes) {
    if (!self->in || !self->out) {
        mp_raise_RuntimeError(translate("No in or out in program"));
    }
    return _transfer(self, data_out, out_len, out_stride_in_bytes, data_in, in_len, in_stride_in_bytes);
}

bool common_hal_rp2pio_statemachine_get_rxstall(rp2pio_statemachine_obj_t *self) {
    uint32_t stall_mask = 1 << (PIO_FDEBUG_RXSTALL_LSB + self->state_machine);
    return (self->pio->fdebug & stall_mask) != 0;
}

void common_hal_rp2pio_statemachine_clear_rxfifo(rp2pio_statemachine_obj_t *self) {
    uint8_t level = pio_sm_get_rx_fifo_level(self->pio, self->state_machine);
    uint32_t stall_mask = 1 << (PIO_FDEBUG_RXSTALL_LSB + self->state_machine);
    for (size_t i = 0; i < level; i++) {
        (void)self->pio->rxf[self->state_machine];
    }
    self->pio->fdebug = stall_mask;
}

size_t common_hal_rp2pio_statemachine_get_in_waiting(rp2pio_statemachine_obj_t *self) {
    uint8_t level = pio_sm_get_rx_fifo_level(self->pio, self->state_machine);
    return level;
}

void common_hal_rp2pio_statemachine_set_interrupt_handler(rp2pio_statemachine_obj_t *self, void (*handler)(void *), void *arg, int mask) {
    uint8_t pio_index = pio_get_index(self->pio);
    uint8_t sm = self->state_machine;

    common_hal_mcu_disable_interrupts();
    uint32_t inte = self->pio->inte0;
    inte &= ~((PIO_IRQ0_INTF_SM0_RXNEMPTY_BITS | PIO_IRQ0_INTF_SM0_TXNFULL_BITS | PIO_IRQ0_INTF_SM0_BITS) << sm);
    inte |= (mask << sm);
    self->pio->inte0 = inte;
    _interrupt_arg[pio_index][sm] = arg;
    _interrupt_handler[pio_index][sm] = handler;
    irq_set_exclusive_handler(PIO0_IRQ_0 + 2 * pio_index, rp2pio_statemachine_interrupt_handler);
    irq_set_enabled(PIO0_IRQ_0 + 2 * pio_index, true);
    common_hal_mcu_enable_interrupts();
}

STATIC void rp2pio_statemachine_interrupt_handler(void) {
    for (size_t pio_index = 0; pio_index < NUM_PIOS; pio_index++) {
        PIO pio = pio_instances[pio_index];
        for (size_t sm = 0; sm < NUM_PIO_STATE_MACHINES; sm++) {
            if (!_interrupt_handler[pio_index][sm]) {
                continue;
            }
            uint32_t intf = (PIO_IRQ0_INTF_SM0_RXNEMPTY_BITS | PIO_IRQ0_INTF_SM0_TXNFULL_BITS | PIO_IRQ0_INTF_SM0_BITS) << sm;
            if (pio->ints0 & intf) {
                _interrupt_handler[pio_index][sm](_interrupt_arg[pio_index][sm]);
            }
        }
    }
}

uint8_t rp2pio_statemachine_program_offset(rp2pio_statemachine_obj_t *self) {
    uint8_t pio_index = pio_get_index(self->pio);
    uint8_t sm = self->state_machine;
    return _current_program_offset[pio_index][sm];
}

void rp2pio_statemachine_set_wrap(rp2pio_statemachine_obj_t *self, uint wrap_target, uint wrap) {
    uint8_t sm = self->state_machine;
    uint8_t offset = rp2pio_statemachine_program_offset(self);

    pio_sm_set_wrap(self->pio, sm, offset + wrap_target, offset + wrap);
}
