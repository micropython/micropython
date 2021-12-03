/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
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

#include <string.h>

#include "py/binary.h"
#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "shared/runtime/mpirq.h"
#include "modrp2.h"

#include "hardware/clocks.h"
#include "hardware/irq.h"
#include "hardware/pio.h"

#define PIO_NUM(pio) ((pio) == pio0 ? 0 : 1)

typedef struct _rp2_pio_obj_t {
    mp_obj_base_t base;
    PIO pio;
    uint8_t irq;
} rp2_pio_obj_t;

typedef struct _rp2_pio_irq_obj_t {
    mp_irq_obj_t base;
    uint32_t flags;
    uint32_t trigger;
} rp2_pio_irq_obj_t;

typedef struct _rp2_state_machine_obj_t {
    mp_obj_base_t base;
    PIO pio;
    uint8_t irq;
    uint8_t sm; // 0-3
    uint8_t id; // 0-7
} rp2_state_machine_obj_t;

typedef struct _rp2_state_machine_irq_obj_t {
    mp_irq_obj_t base;
    uint8_t flags;
    uint8_t trigger;
} rp2_state_machine_irq_obj_t;

STATIC const rp2_state_machine_obj_t rp2_state_machine_obj[8];
STATIC uint8_t rp2_state_machine_initial_pc[8];

STATIC mp_obj_t rp2_state_machine_init_helper(const rp2_state_machine_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);

STATIC void pio_irq0(PIO pio) {
    uint32_t ints = pio->ints0;

    // Acknowledge SM0-3 IRQs if they are enabled on this IRQ0.
    pio->irq = ints >> 8;

    // Call handler if it is registered, for PIO irqs.
    rp2_pio_irq_obj_t *irq = MP_STATE_PORT(rp2_pio_irq_obj[PIO_NUM(pio)]);
    if (irq != NULL && (ints & irq->trigger)) {
        irq->flags = ints & irq->trigger;
        mp_irq_handler(&irq->base);
    }

    // Call handler if it is registered, for StateMachine irqs.
    for (size_t i = 0; i < 4; ++i) {
        rp2_state_machine_irq_obj_t *irq = MP_STATE_PORT(rp2_state_machine_irq_obj[PIO_NUM(pio) * 4 + i]);
        if (irq != NULL && ((ints >> (8 + i)) & irq->trigger)) {
            irq->flags = 1;
            mp_irq_handler(&irq->base);
        }
    }
}

STATIC void pio0_irq0(void) {
    pio_irq0(pio0);
}

STATIC void pio1_irq0(void) {
    pio_irq0(pio1);
}

void rp2_pio_init(void) {
    // Reset all PIO instruction memory.
    pio_clear_instruction_memory(pio0);
    pio_clear_instruction_memory(pio1);

    // Set up interrupts.
    memset(MP_STATE_PORT(rp2_pio_irq_obj), 0, sizeof(MP_STATE_PORT(rp2_pio_irq_obj)));
    memset(MP_STATE_PORT(rp2_state_machine_irq_obj), 0, sizeof(MP_STATE_PORT(rp2_state_machine_irq_obj)));
    irq_set_exclusive_handler(PIO0_IRQ_0, pio0_irq0);
    irq_set_exclusive_handler(PIO1_IRQ_0, pio1_irq0);
}

void rp2_pio_deinit(void) {
    // Disable and clear interrupts.
    irq_set_mask_enabled((1u << PIO0_IRQ_0) | (1u << PIO0_IRQ_1), false);
    irq_remove_handler(PIO0_IRQ_0, pio0_irq0);
    irq_remove_handler(PIO1_IRQ_0, pio1_irq0);
}

/******************************************************************************/
// Helper functions to manage asm_pio data structure.

#define ASM_PIO_CONFIG_DEFAULT { -1, 0, 0, 0 };

enum {
    PROG_DATA,
    PROG_OFFSET_PIO0,
    PROG_OFFSET_PIO1,
    PROG_EXECCTRL,
    PROG_SHIFTCTRL,
    PROG_OUT_PINS,
    PROG_SET_PINS,
    PROG_SIDESET_PINS,
    PROG_MAX_FIELDS,
};

typedef struct _asm_pio_config_t {
    int8_t base;
    uint8_t count;
    uint32_t pindirs;
    uint32_t pinvals;
} asm_pio_config_t;

STATIC void asm_pio_override_shiftctrl(mp_obj_t arg, uint32_t bits, uint32_t lsb, pio_sm_config *config) {
    if (arg != mp_const_none) {
        config->shiftctrl = (config->shiftctrl & ~bits) | (mp_obj_get_int(arg) << lsb);
    }
}

STATIC void asm_pio_get_pins(const char *type, mp_obj_t prog_pins, mp_obj_t arg_base, asm_pio_config_t *config) {
    if (prog_pins != mp_const_none) {
        // The PIO program specified pins for initialisation on out/set/sideset.
        if (mp_obj_is_integer(prog_pins)) {
            // A single pin specified, set its dir and value.
            config->count = 1;
            mp_int_t value = mp_obj_get_int(prog_pins);
            config->pindirs = value >> 1;
            config->pinvals = value & 1;
        } else {
            // An array of pins specified, set their dirs and values.
            size_t count;
            mp_obj_t *items;
            mp_obj_get_array(prog_pins, &count, &items);
            config->count = count;
            for (size_t i = 0; i < config->count; ++i) {
                mp_int_t value = mp_obj_get_int(items[i]);
                config->pindirs |= (value >> 1) << i;
                config->pinvals |= (value & 1) << i;
            }
        }
    }

    if (arg_base != mp_const_none) {
        // The instantiation of the PIO program specified a base pin.
        config->base = mp_hal_get_pin_obj(arg_base);
    }
}

STATIC void asm_pio_init_gpio(PIO pio, uint32_t sm, asm_pio_config_t *config) {
    uint32_t pinmask = ((1 << config->count) - 1) << config->base;
    pio_sm_set_pins_with_mask(pio, sm, config->pinvals << config->base, pinmask);
    pio_sm_set_pindirs_with_mask(pio, sm, config->pindirs << config->base, pinmask);
    for (size_t i = 0; i < config->count; ++i) {
        gpio_set_function(config->base + i, pio == pio0 ? GPIO_FUNC_PIO0 : GPIO_FUNC_PIO1);
    }
}

/******************************************************************************/
// PIO object

STATIC const mp_irq_methods_t rp2_pio_irq_methods;

STATIC rp2_pio_obj_t rp2_pio_obj[] = {
    { { &rp2_pio_type }, pio0, PIO0_IRQ_0 },
    { { &rp2_pio_type }, pio1, PIO1_IRQ_0 },
};

STATIC void rp2_pio_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    rp2_pio_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "PIO(%u)", self->pio == pio0 ? 0 : 1);
}

// constructor(id)
STATIC mp_obj_t rp2_pio_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, false);

    // Get the PIO object.
    int pio_id = mp_obj_get_int(args[0]);
    if (!(0 <= pio_id && pio_id < MP_ARRAY_SIZE(rp2_pio_obj))) {
        mp_raise_ValueError("invalid PIO");
    }
    const rp2_pio_obj_t *self = &rp2_pio_obj[pio_id];

    // Return the PIO object.
    return MP_OBJ_FROM_PTR(self);
}

// PIO.add_program(prog)
STATIC mp_obj_t rp2_pio_add_program(mp_obj_t self_in, mp_obj_t prog_in) {
    rp2_pio_obj_t *self = MP_OBJ_TO_PTR(self_in);

    // Get the program data.
    mp_obj_t *prog;
    mp_obj_get_array_fixed_n(prog_in, PROG_MAX_FIELDS, &prog);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(prog[PROG_DATA], &bufinfo, MP_BUFFER_READ);

    // Add the program data to the PIO instruction memory.
    struct pio_program pio_program = { bufinfo.buf, bufinfo.len / 2, -1 };
    if (!pio_can_add_program(self->pio, &pio_program)) {
        mp_raise_OSError(MP_ENOMEM);
    }
    uint offset = pio_add_program(self->pio, &pio_program);

    // Store the program offset in the program object.
    prog[PROG_OFFSET_PIO0 + PIO_NUM(self->pio)] = MP_OBJ_NEW_SMALL_INT(offset);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(rp2_pio_add_program_obj, rp2_pio_add_program);

// PIO.remove_program([prog])
STATIC mp_obj_t rp2_pio_remove_program(size_t n_args, const mp_obj_t *args) {
    rp2_pio_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    // Default to remove all programs.
    uint8_t length = 32;
    uint offset = 0;

    if (n_args > 1) {
        // Get specific program to remove.
        mp_obj_t *prog;
        mp_obj_get_array_fixed_n(args[1], PROG_MAX_FIELDS, &prog);
        mp_buffer_info_t bufinfo;
        mp_get_buffer_raise(prog[PROG_DATA], &bufinfo, MP_BUFFER_READ);
        length = bufinfo.len / 2;
        offset = mp_obj_get_int(prog[PROG_OFFSET_PIO0 + PIO_NUM(self->pio)]);
        if (offset < 0) {
            mp_raise_ValueError("prog not in instruction memory");
        }
        // Invalidate the program offset in the program object.
        prog[PROG_OFFSET_PIO0 + PIO_NUM(self->pio)] = MP_OBJ_NEW_SMALL_INT(-1);
    }

    // Remove the program from the instruction memory.
    struct pio_program pio_program = { NULL, length, -1 };
    pio_remove_program(self->pio, &pio_program, offset);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(rp2_pio_remove_program_obj, 1, 2, rp2_pio_remove_program);

// PIO.state_machine(id, prog, freq=-1, *, set=None)
STATIC mp_obj_t rp2_pio_state_machine(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    rp2_pio_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);

    // Get and verify the state machine id.
    mp_int_t sm_id = mp_obj_get_int(pos_args[1]);
    if (!(0 <= sm_id && sm_id < 4)) {
        mp_raise_ValueError("invalide state machine");
    }

    // Return the correct StateMachine object.
    const rp2_state_machine_obj_t *sm = &rp2_state_machine_obj[(self->pio == pio0 ? 0 : 4) + sm_id];

    if (n_args > 2 || kw_args->used > 0) {
        // Configuration arguments given so init this StateMachine.
        rp2_state_machine_init_helper(sm, n_args - 2, pos_args + 2, kw_args);
    }

    return MP_OBJ_FROM_PTR(sm);
}
MP_DEFINE_CONST_FUN_OBJ_KW(rp2_pio_state_machine_obj, 2, rp2_pio_state_machine);

// PIO.irq(handler=None, trigger=IRQ_SM0|IRQ_SM1|IRQ_SM2|IRQ_SM3, hard=False)
STATIC mp_obj_t rp2_pio_irq(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_handler, ARG_trigger, ARG_hard };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_handler, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_trigger, MP_ARG_INT, {.u_int = 0xf00} },
        { MP_QSTR_hard, MP_ARG_BOOL, {.u_bool = false} },
    };

    // Parse the arguments.
    rp2_pio_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Get the IRQ object.
    rp2_pio_irq_obj_t *irq = MP_STATE_PORT(rp2_pio_irq_obj[PIO_NUM(self->pio)]);

    // Allocate the IRQ object if it doesn't already exist.
    if (irq == NULL) {
        irq = m_new_obj(rp2_pio_irq_obj_t);
        irq->base.base.type = &mp_irq_type;
        irq->base.methods = (mp_irq_methods_t *)&rp2_pio_irq_methods;
        irq->base.parent = MP_OBJ_FROM_PTR(self);
        irq->base.handler = mp_const_none;
        irq->base.ishard = false;
        MP_STATE_PORT(rp2_pio_irq_obj[PIO_NUM(self->pio)]) = irq;
    }

    if (n_args > 1 || kw_args->used != 0) {
        // Configure IRQ.

        // Disable all IRQs while data is updated.
        irq_set_enabled(self->irq, false);

        // Update IRQ data.
        irq->base.handler = args[ARG_handler].u_obj;
        irq->base.ishard = args[ARG_hard].u_bool;
        irq->flags = 0;
        irq->trigger = args[ARG_trigger].u_int;

        // Enable IRQ if a handler is given.
        if (args[ARG_handler].u_obj != mp_const_none) {
            self->pio->inte0 = irq->trigger;
            irq_set_enabled(self->irq, true);
        }
    }

    return MP_OBJ_FROM_PTR(irq);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(rp2_pio_irq_obj, 1, rp2_pio_irq);

STATIC const mp_rom_map_elem_t rp2_pio_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_add_program), MP_ROM_PTR(&rp2_pio_add_program_obj) },
    { MP_ROM_QSTR(MP_QSTR_remove_program), MP_ROM_PTR(&rp2_pio_remove_program_obj) },
    { MP_ROM_QSTR(MP_QSTR_state_machine), MP_ROM_PTR(&rp2_pio_state_machine_obj) },
    { MP_ROM_QSTR(MP_QSTR_irq), MP_ROM_PTR(&rp2_pio_irq_obj) },

    { MP_ROM_QSTR(MP_QSTR_IN_LOW), MP_ROM_INT(0) },
    { MP_ROM_QSTR(MP_QSTR_IN_HIGH), MP_ROM_INT(1) },
    { MP_ROM_QSTR(MP_QSTR_OUT_LOW), MP_ROM_INT(2) },
    { MP_ROM_QSTR(MP_QSTR_OUT_HIGH), MP_ROM_INT(3) },

    { MP_ROM_QSTR(MP_QSTR_SHIFT_LEFT), MP_ROM_INT(0) },
    { MP_ROM_QSTR(MP_QSTR_SHIFT_RIGHT), MP_ROM_INT(1) },

    { MP_ROM_QSTR(MP_QSTR_JOIN_NONE), MP_ROM_INT(0) },
    { MP_ROM_QSTR(MP_QSTR_JOIN_TX), MP_ROM_INT(1) },
    { MP_ROM_QSTR(MP_QSTR_JOIN_RX), MP_ROM_INT(2) },

    { MP_ROM_QSTR(MP_QSTR_IRQ_SM0), MP_ROM_INT(0x100) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_SM1), MP_ROM_INT(0x200) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_SM2), MP_ROM_INT(0x400) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_SM3), MP_ROM_INT(0x800) },
};
STATIC MP_DEFINE_CONST_DICT(rp2_pio_locals_dict, rp2_pio_locals_dict_table);

const mp_obj_type_t rp2_pio_type = {
    { &mp_type_type },
    .name = MP_QSTR_PIO,
    .print = rp2_pio_print,
    .make_new = rp2_pio_make_new,
    .locals_dict = (mp_obj_dict_t *)&rp2_pio_locals_dict,
};

STATIC mp_uint_t rp2_pio_irq_trigger(mp_obj_t self_in, mp_uint_t new_trigger) {
    rp2_pio_obj_t *self = MP_OBJ_TO_PTR(self_in);
    rp2_pio_irq_obj_t *irq = MP_STATE_PORT(rp2_pio_irq_obj[PIO_NUM(self->pio)]);
    irq_set_enabled(self->irq, false);
    irq->flags = 0;
    irq->trigger = new_trigger;
    irq_set_enabled(self->irq, true);
    return 0;
}

STATIC mp_uint_t rp2_pio_irq_info(mp_obj_t self_in, mp_uint_t info_type) {
    rp2_pio_obj_t *self = MP_OBJ_TO_PTR(self_in);
    rp2_pio_irq_obj_t *irq = MP_STATE_PORT(rp2_pio_irq_obj[PIO_NUM(self->pio)]);
    if (info_type == MP_IRQ_INFO_FLAGS) {
        return irq->flags;
    } else if (info_type == MP_IRQ_INFO_TRIGGERS) {
        return irq->trigger;
    }
    return 0;
}

STATIC const mp_irq_methods_t rp2_pio_irq_methods = {
    .trigger = rp2_pio_irq_trigger,
    .info = rp2_pio_irq_info,
};

/******************************************************************************/
// StateMachine object

STATIC const mp_irq_methods_t rp2_state_machine_irq_methods;

STATIC const rp2_state_machine_obj_t rp2_state_machine_obj[] = {
    { { &rp2_state_machine_type }, pio0, PIO0_IRQ_0, 0, 0 },
    { { &rp2_state_machine_type }, pio0, PIO0_IRQ_0, 1, 1 },
    { { &rp2_state_machine_type }, pio0, PIO0_IRQ_0, 2, 2 },
    { { &rp2_state_machine_type }, pio0, PIO0_IRQ_0, 3, 3 },
    { { &rp2_state_machine_type }, pio1, PIO1_IRQ_0, 0, 4 },
    { { &rp2_state_machine_type }, pio1, PIO1_IRQ_0, 1, 5 },
    { { &rp2_state_machine_type }, pio1, PIO1_IRQ_0, 2, 6 },
    { { &rp2_state_machine_type }, pio1, PIO1_IRQ_0, 3, 7 },
};

STATIC void rp2_state_machine_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    rp2_state_machine_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "StateMachine(%u)", self->id);
}

// StateMachine.init(prog, freq=-1, *,
//     in_base=None, out_base=None, set_base=None, jmp_pin=None,
//     sideset_base=None, in_shiftdir=None, out_shiftdir=None,
//     push_thresh=None, pull_thresh=None,
// )
STATIC mp_obj_t rp2_state_machine_init_helper(const rp2_state_machine_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum {
        ARG_prog, ARG_freq,
        ARG_in_base, ARG_out_base, ARG_set_base, ARG_jmp_pin, ARG_sideset_base,
        ARG_in_shiftdir, ARG_out_shiftdir, ARG_push_thresh, ARG_pull_thresh
    };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_prog, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_freq, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_in_base, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_out_base, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_set_base, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_jmp_pin, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_sideset_base, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_in_shiftdir, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_out_shiftdir, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_push_thresh, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_pull_thresh, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    };

    // Parse the arguments.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Get the program.
    mp_obj_t *prog;
    mp_obj_get_array_fixed_n(args[ARG_prog].u_obj, PROG_MAX_FIELDS, &prog);

    // Get and the program offset, and load it into memory if it's not already there.
    mp_int_t offset = mp_obj_get_int(prog[PROG_OFFSET_PIO0 + PIO_NUM(self->pio)]);
    if (offset < 0) {
        rp2_pio_add_program(&rp2_pio_obj[PIO_NUM(self->pio)], args[ARG_prog].u_obj);
        offset = mp_obj_get_int(prog[PROG_OFFSET_PIO0 + PIO_NUM(self->pio)]);
        rp2_state_machine_initial_pc[self->id] = offset;
    }

    // Compute the clock divider.
    uint16_t clkdiv_int;
    uint8_t clkdiv_frac;
    if (args[ARG_freq].u_int < 0) {
        // Default: run at CPU frequency.
        clkdiv_int = 1;
        clkdiv_frac = 0;
    } else if (args[ARG_freq].u_int == 0) {
        // Special case of 0: set clkdiv to 0.
        clkdiv_int = 0;
        clkdiv_frac = 0;
    } else {
        // Frequency given in Hz, compute clkdiv from it.
        uint64_t div = (uint64_t)clock_get_hz(clk_sys) * 256ULL / (uint64_t)args[ARG_freq].u_int;
        if (!(div >= 1 * 256 && div <= 65536 * 256)) {
            mp_raise_ValueError(MP_ERROR_TEXT("freq out of range"));
        }
        clkdiv_int = div / 256;
        clkdiv_frac = div & 0xff;
    }

    // Disable and reset the state machine.
    pio_sm_init(self->pio, self->sm, offset, NULL);

    // Build the state machine config.
    pio_sm_config config = pio_get_default_sm_config();
    sm_config_set_clkdiv_int_frac(&config, clkdiv_int, clkdiv_frac);
    config.execctrl = mp_obj_get_int_truncated(prog[PROG_EXECCTRL]);
    config.shiftctrl = mp_obj_get_int_truncated(prog[PROG_SHIFTCTRL]);

    // Adjust wrap top/bottom to account for location of program in instruction memory.
    config.execctrl += (offset << PIO_SM0_EXECCTRL_WRAP_TOP_LSB)
        + (offset << PIO_SM0_EXECCTRL_WRAP_BOTTOM_LSB);

    // Configure in pin base, if needed.
    if (args[ARG_in_base].u_obj != mp_const_none) {
        sm_config_set_in_pins(&config, mp_hal_get_pin_obj(args[ARG_in_base].u_obj));
    }

    // Configure out pins, if needed.
    asm_pio_config_t out_config = ASM_PIO_CONFIG_DEFAULT;
    asm_pio_get_pins("out", prog[PROG_OUT_PINS], args[ARG_out_base].u_obj, &out_config);
    if (out_config.base >= 0) {
        sm_config_set_out_pins(&config, out_config.base, out_config.count);
    }

    // Configure set pin, if needed.
    asm_pio_config_t set_config = ASM_PIO_CONFIG_DEFAULT;
    asm_pio_get_pins("set", prog[PROG_SET_PINS], args[ARG_set_base].u_obj, &set_config);
    if (set_config.base >= 0) {
        sm_config_set_set_pins(&config, set_config.base, set_config.count);
    }

    // Configure jmp pin, if needed.
    if (args[ARG_jmp_pin].u_obj != mp_const_none) {
        sm_config_set_jmp_pin(&config, mp_hal_get_pin_obj(args[ARG_jmp_pin].u_obj));
    }

    // Configure sideset pin, if needed.
    asm_pio_config_t sideset_config = ASM_PIO_CONFIG_DEFAULT;
    asm_pio_get_pins("sideset", prog[PROG_SIDESET_PINS], args[ARG_sideset_base].u_obj, &sideset_config);
    if (sideset_config.base >= 0) {
        uint32_t count = sideset_config.count;
        if (config.execctrl & (1 << PIO_SM0_EXECCTRL_SIDE_EN_LSB)) {
            // When sideset is optional, count includes the option bit.
            ++count;
        }
        config.pinctrl |= count << PIO_SM0_PINCTRL_SIDESET_COUNT_LSB;
        sm_config_set_sideset_pins(&config, sideset_config.base);
    }

    // Override shift state if needed.
    asm_pio_override_shiftctrl(args[ARG_in_shiftdir].u_obj, PIO_SM0_SHIFTCTRL_IN_SHIFTDIR_BITS, PIO_SM0_SHIFTCTRL_IN_SHIFTDIR_LSB, &config);
    asm_pio_override_shiftctrl(args[ARG_out_shiftdir].u_obj, PIO_SM0_SHIFTCTRL_OUT_SHIFTDIR_BITS, PIO_SM0_SHIFTCTRL_OUT_SHIFTDIR_LSB, &config);
    asm_pio_override_shiftctrl(args[ARG_push_thresh].u_obj, PIO_SM0_SHIFTCTRL_PUSH_THRESH_BITS, PIO_SM0_SHIFTCTRL_PUSH_THRESH_LSB, &config);
    asm_pio_override_shiftctrl(args[ARG_pull_thresh].u_obj, PIO_SM0_SHIFTCTRL_PULL_THRESH_BITS, PIO_SM0_SHIFTCTRL_PULL_THRESH_LSB, &config);

    // Configure the state machine.
    pio_sm_set_config(self->pio, self->sm, &config);

    // Configure the GPIO.
    if (out_config.base >= 0) {
        asm_pio_init_gpio(self->pio, self->sm, &out_config);
    }
    if (set_config.base >= 0) {
        asm_pio_init_gpio(self->pio, self->sm, &set_config);
    }
    if (sideset_config.base >= 0) {
        asm_pio_init_gpio(self->pio, self->sm, &sideset_config);
    }

    return mp_const_none;
}

// StateMachine(id, ...)
STATIC mp_obj_t rp2_state_machine_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // Get the StateMachine object.
    mp_int_t sm_id = mp_obj_get_int(args[0]);
    if (!(0 <= sm_id && sm_id < MP_ARRAY_SIZE(rp2_state_machine_obj))) {
        mp_raise_ValueError("invalid StateMachine");
    }
    const rp2_state_machine_obj_t *self = &rp2_state_machine_obj[sm_id];

    if (n_args > 1 || n_kw > 0) {
        // Configuration arguments given so init this StateMachine.
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        rp2_state_machine_init_helper(self, n_args - 1, args + 1, &kw_args);
    }

    // Return the StateMachine object.
    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t rp2_state_machine_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    return rp2_state_machine_init_helper(MP_OBJ_TO_PTR(args[0]), n_args - 1, args + 1, kw_args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(rp2_state_machine_init_obj, 1, rp2_state_machine_init);

// StateMachine.active([value])
STATIC mp_obj_t rp2_state_machine_active(size_t n_args, const mp_obj_t *args) {
    rp2_state_machine_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (n_args > 1) {
        pio_sm_set_enabled(self->pio, self->sm, mp_obj_is_true(args[1]));
    }
    return mp_obj_new_bool((self->pio->ctrl >> self->sm) & 1);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(rp2_state_machine_active_obj, 1, 2, rp2_state_machine_active);

// StateMachine.restart()
STATIC mp_obj_t rp2_state_machine_restart(mp_obj_t self_in) {
    rp2_state_machine_obj_t *self = MP_OBJ_TO_PTR(self_in);
    pio_sm_restart(self->pio, self->sm);
    pio_sm_exec(self->pio, self->sm, pio_encode_jmp(rp2_state_machine_initial_pc[self->id]));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(rp2_state_machine_restart_obj, rp2_state_machine_restart);

// StateMachine.exec(instr)
STATIC mp_obj_t rp2_state_machine_exec(mp_obj_t self_in, mp_obj_t instr_in) {
    rp2_state_machine_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_t rp2_module = mp_import_name(MP_QSTR_rp2, mp_const_none, MP_OBJ_NEW_SMALL_INT(0));
    mp_obj_t asm_pio_encode = mp_load_attr(rp2_module, MP_QSTR_asm_pio_encode);
    uint32_t sideset_count = self->pio->sm[self->sm].pinctrl >> PIO_SM0_PINCTRL_SIDESET_COUNT_LSB;
    uint8_t sideset_opt = !!(self->pio->sm[self->sm].execctrl & (1 << PIO_SM0_EXECCTRL_SIDE_EN_LSB));
    mp_obj_t args[3];
    args[0] = instr_in;
    args[1] = MP_OBJ_NEW_SMALL_INT(sideset_count);
    args[2] = MP_OBJ_NEW_SMALL_INT(sideset_opt);
    mp_obj_t encoded_obj = mp_call_function_n_kw(asm_pio_encode, 3, 0, args);
    mp_int_t encoded = mp_obj_get_int(encoded_obj);
    pio_sm_exec(self->pio, self->sm, encoded);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(rp2_state_machine_exec_obj, rp2_state_machine_exec);

// StateMachine.get(buf=None, shift=0)
STATIC mp_obj_t rp2_state_machine_get(size_t n_args, const mp_obj_t *args) {
    rp2_state_machine_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_buffer_info_t bufinfo;
    bufinfo.buf = NULL;
    uint32_t shift = 0;
    if (n_args > 1) {
        if (args[1] != mp_const_none) {
            mp_get_buffer_raise(args[1], &bufinfo, MP_BUFFER_WRITE);
            if (bufinfo.typecode == BYTEARRAY_TYPECODE) {
                bufinfo.typecode = 'b';
            } else {
                bufinfo.typecode |= 0x20; // make lowercase to support upper and lower
            }
            if (bufinfo.len == 0) { // edge case: buffer of zero length supplied
                return args[1];
            }
        }
        if (n_args > 2) {
            shift = mp_obj_get_int(args[2]);
        }
    }
    uint8_t *dest = bufinfo.buf;
    const uint8_t *dest_top = dest + bufinfo.len;
    for (;;) {
        while (pio_sm_is_rx_fifo_empty(self->pio, self->sm)) {
            // This delay must be fast.
            mp_handle_pending(true);
            MICROPY_HW_USBDEV_TASK_HOOK
        }
        uint32_t value = pio_sm_get(self->pio, self->sm) >> shift;
        if (dest == NULL) {
            return mp_obj_new_int_from_uint(value);
        }
        if (bufinfo.typecode == 'b') {
            *(uint8_t *)dest = value;
            dest += sizeof(uint8_t);
        } else if (bufinfo.typecode == 'h') {
            *(uint16_t *)dest = value;
            dest += sizeof(uint16_t);
        } else if (bufinfo.typecode == 'i') {
            *(uint32_t *)dest = value;
            dest += sizeof(uint32_t);
        } else {
            mp_raise_ValueError("unsupported buffer type");
        }
        if (dest >= dest_top) {
            return args[1];
        }
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(rp2_state_machine_get_obj, 1, 3, rp2_state_machine_get);

// StateMachine.put(value, shift=0)
STATIC mp_obj_t rp2_state_machine_put(size_t n_args, const mp_obj_t *args) {
    rp2_state_machine_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    uint32_t shift = 0;
    if (n_args > 2) {
        shift = mp_obj_get_int(args[2]);
    }
    uint32_t data;
    mp_buffer_info_t bufinfo;
    if (!mp_get_buffer(args[1], &bufinfo, MP_BUFFER_READ)) {
        data = mp_obj_get_int_truncated(args[1]);
        bufinfo.buf = &data;
        bufinfo.len = sizeof(uint32_t);
        bufinfo.typecode = 'I';
    }
    const uint8_t *src = bufinfo.buf;
    const uint8_t *src_top = src + bufinfo.len;
    while (src < src_top) {
        uint32_t value;
        if (bufinfo.typecode == 'B' || bufinfo.typecode == BYTEARRAY_TYPECODE) {
            value = *(uint8_t *)src;
            src += sizeof(uint8_t);
        } else if (bufinfo.typecode == 'H') {
            value = *(uint16_t *)src;
            src += sizeof(uint16_t);
        } else if (bufinfo.typecode == 'I') {
            value = *(uint32_t *)src;
            src += sizeof(uint32_t);
        } else {
            mp_raise_ValueError("unsupported buffer type");
        }
        while (pio_sm_is_tx_fifo_full(self->pio, self->sm)) {
            // This delay must be fast.
            mp_handle_pending(true);
            MICROPY_HW_USBDEV_TASK_HOOK
        }
        pio_sm_put(self->pio, self->sm, value << shift);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(rp2_state_machine_put_obj, 2, 3, rp2_state_machine_put);

// StateMachine.rx_fifo()
STATIC mp_obj_t rp2_state_machine_rx_fifo(mp_obj_t self_in) {
    rp2_state_machine_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(pio_sm_get_rx_fifo_level(self->pio, self->sm));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(rp2_state_machine_rx_fifo_obj, rp2_state_machine_rx_fifo);

// StateMachine.tx_fifo()
STATIC mp_obj_t rp2_state_machine_tx_fifo(mp_obj_t self_in) {
    rp2_state_machine_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(pio_sm_get_tx_fifo_level(self->pio, self->sm));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(rp2_state_machine_tx_fifo_obj, rp2_state_machine_tx_fifo);

// StateMachine.irq(handler=None, trigger=0|1, hard=False)
STATIC mp_obj_t rp2_state_machine_irq(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_handler, ARG_trigger, ARG_hard };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_handler, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_trigger, MP_ARG_INT, {.u_int = 1} },
        { MP_QSTR_hard, MP_ARG_BOOL, {.u_bool = false} },
    };

    // Parse the arguments.
    rp2_state_machine_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Get the IRQ object.
    rp2_state_machine_irq_obj_t *irq = MP_STATE_PORT(rp2_state_machine_irq_obj[self->id]);

    // Allocate the IRQ object if it doesn't already exist.
    if (irq == NULL) {
        irq = m_new_obj(rp2_state_machine_irq_obj_t);
        irq->base.base.type = &mp_irq_type;
        irq->base.methods = (mp_irq_methods_t *)&rp2_state_machine_irq_methods;
        irq->base.parent = MP_OBJ_FROM_PTR(self);
        irq->base.handler = mp_const_none;
        irq->base.ishard = false;
        MP_STATE_PORT(rp2_state_machine_irq_obj[self->id]) = irq;
    }

    if (n_args > 1 || kw_args->used != 0) {
        // Configure IRQ.

        // Disable all IRQs while data is updated.
        irq_set_enabled(self->irq, false);

        // Update IRQ data.
        irq->base.handler = args[ARG_handler].u_obj;
        irq->base.ishard = args[ARG_hard].u_bool;
        irq->flags = 0;
        irq->trigger = args[ARG_trigger].u_int;

        // Enable IRQ if a handler is given.
        if (args[ARG_handler].u_obj == mp_const_none) {
            self->pio->inte0 &= ~(1 << (8 + self->sm));
        } else {
            self->pio->inte0 |= 1 << (8 + self->sm);
        }

        if (self->pio->inte0) {
            irq_set_enabled(self->irq, true);
        }
    }

    return MP_OBJ_FROM_PTR(irq);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(rp2_state_machine_irq_obj, 1, rp2_state_machine_irq);

STATIC const mp_rom_map_elem_t rp2_state_machine_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&rp2_state_machine_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_active), MP_ROM_PTR(&rp2_state_machine_active_obj) },
    { MP_ROM_QSTR(MP_QSTR_restart), MP_ROM_PTR(&rp2_state_machine_restart_obj) },
    { MP_ROM_QSTR(MP_QSTR_exec), MP_ROM_PTR(&rp2_state_machine_exec_obj) },
    { MP_ROM_QSTR(MP_QSTR_get), MP_ROM_PTR(&rp2_state_machine_get_obj) },
    { MP_ROM_QSTR(MP_QSTR_put), MP_ROM_PTR(&rp2_state_machine_put_obj) },
    { MP_ROM_QSTR(MP_QSTR_rx_fifo), MP_ROM_PTR(&rp2_state_machine_rx_fifo_obj) },
    { MP_ROM_QSTR(MP_QSTR_tx_fifo), MP_ROM_PTR(&rp2_state_machine_tx_fifo_obj) },
    { MP_ROM_QSTR(MP_QSTR_irq), MP_ROM_PTR(&rp2_state_machine_irq_obj) },
};
STATIC MP_DEFINE_CONST_DICT(rp2_state_machine_locals_dict, rp2_state_machine_locals_dict_table);

const mp_obj_type_t rp2_state_machine_type = {
    { &mp_type_type },
    .name = MP_QSTR_StateMachine,
    .print = rp2_state_machine_print,
    .make_new = rp2_state_machine_make_new,
    .locals_dict = (mp_obj_dict_t *)&rp2_state_machine_locals_dict,
};

STATIC mp_uint_t rp2_state_machine_irq_trigger(mp_obj_t self_in, mp_uint_t new_trigger) {
    rp2_state_machine_obj_t *self = MP_OBJ_TO_PTR(self_in);
    rp2_state_machine_irq_obj_t *irq = MP_STATE_PORT(rp2_state_machine_irq_obj[PIO_NUM(self->pio)]);
    irq_set_enabled(self->irq, false);
    irq->flags = 0;
    irq->trigger = new_trigger;
    irq_set_enabled(self->irq, true);
    return 0;
}

STATIC mp_uint_t rp2_state_machine_irq_info(mp_obj_t self_in, mp_uint_t info_type) {
    rp2_state_machine_obj_t *self = MP_OBJ_TO_PTR(self_in);
    rp2_state_machine_irq_obj_t *irq = MP_STATE_PORT(rp2_state_machine_irq_obj[PIO_NUM(self->pio)]);
    if (info_type == MP_IRQ_INFO_FLAGS) {
        return irq->flags;
    } else if (info_type == MP_IRQ_INFO_TRIGGERS) {
        return irq->trigger;
    }
    return 0;
}

STATIC const mp_irq_methods_t rp2_state_machine_irq_methods = {
    .trigger = rp2_state_machine_irq_trigger,
    .info = rp2_state_machine_irq_info,
};
