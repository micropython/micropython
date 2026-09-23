// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

// This file contains all of the Python API definitions for the
// rp2pio.StateMachine class.

#include <string.h>

#include "shared-bindings/microcontroller/Pin.h"
#include "bindings/rp2pio/StateMachine.h"
#include "shared-bindings/util.h"

#include "shared/runtime/buffer_helper.h"
#include "shared/runtime/context_manager_helpers.h"
#include "shared/runtime/interrupt_char.h"
#include "py/binary.h"
#include "py/mperrno.h"
#include "py/objproperty.h"
#include "py/runtime.h"

//| import memorymap
//|
//| FifoType = Literal["auto", "txrx", "tx", "rx", "txput", "txget", "putget"]
//| """A type representing one of the strings ``"auto"``, ``"txrx"``, ``"tx"``, ``"rx"``, ``"txput"``, ``"txget"`` or ``"putget"``. These values are supported on RP2350. For type-checking only, not actually defined in CircuitPython."""
//| FifoType_piov0 = Literal["auto", "txrx", "tx", "rx"]
//| """A type representing one of the strings ``"auto"``, ``"txrx"``, ``"tx"``, or ``"rx"``. These values are supported on both RP2350 and RP2040. For type-checking only, not actually defined in CircuitPython."""
//| MovStatusType = Literal["txfifo", "rxfifo", "irq"]
//| """A type representing one of the strings ``"txfifo"``, ``"rxfifo"``, or ``"irq"``. These values are supported on RP2350. For type-checking only, not actually defined in CircuitPython."""
//| MovStatusType_piov0 = Literal["txfifo", "rxfifo"]
//| """A type representing one of the strings ``"txfifo"``, ``"rxfifo"``. These values are supported on RP2350 and RP2040. For type-checking only, not actually defined in CircuitPython."""
//|
//|
//| class StateMachine:
//|     """A single PIO StateMachine
//|
//|     The programmable I/O peripheral on the RP2 series of microcontrollers is
//|     unique. It is a collection of generic state machines that can be
//|     used for a variety of protocols. State machines may be independent or
//|     coordinated. Program memory and IRQs are shared between the state machines
//|     in a particular PIO instance. They are independent otherwise.
//|
//|     This class is designed to facilitate sharing of PIO resources. By default,
//|     it is assumed that the state machine is used on its own and can be placed
//|     in either PIO. State machines with the same program will be placed in the
//|     same PIO if possible."""
//|
//|     def __init__(
//|         self,
//|         program: ReadableBuffer,
//|         frequency: int,
//|         *,
//|         pio_version: int = 0,
//|         may_exec: Optional[ReadableBuffer] = None,
//|         init: Optional[ReadableBuffer] = None,
//|         first_out_pin: Optional[microcontroller.Pin] = None,
//|         out_pin_count: int = 1,
//|         initial_out_pin_state: int = 0,
//|         initial_out_pin_direction: int = 0xFFFFFFFF,
//|         first_in_pin: Optional[microcontroller.Pin] = None,
//|         in_pin_count: int = 1,
//|         pull_in_pin_up: int = 0,
//|         pull_in_pin_down: int = 0,
//|         first_set_pin: Optional[microcontroller.Pin] = None,
//|         set_pin_count: int = 1,
//|         initial_set_pin_state: int = 0,
//|         initial_set_pin_direction: int = 0x1F,
//|         first_sideset_pin: Optional[microcontroller.Pin] = None,
//|         sideset_pin_count: int = 1,
//|         sideset_pindirs: bool = False,
//|         initial_sideset_pin_state: int = 0,
//|         initial_sideset_pin_direction: int = 0x1F,
//|         sideset_enable: bool = False,
//|         jmp_pin: Optional[microcontroller.Pin] = None,
//|         jmp_pin_pull: Optional[digitalio.Pull] = None,
//|         exclusive_pin_use: bool = True,
//|         auto_pull: bool = False,
//|         pull_threshold: int = 32,
//|         out_shift_right: bool = True,
//|         wait_for_txstall: bool = True,
//|         auto_push: bool = False,
//|         push_threshold: int = 32,
//|         in_shift_right: bool = True,
//|         user_interruptible: bool = True,
//|         wrap_target: int = 0,
//|         wrap: int = -1,
//|         offset: int = -1,
//|         fifo_type: FifoType = "auto",
//|         mov_status_type: MovStatusType = "txfifo",
//|         mov_status_n: int = 0,
//|     ) -> None:
//|         """Construct a StateMachine object on the given pins with the given program.
//|
//|         The following parameters are usually supplied directly:
//|
//|         :param ReadableBuffer program: the program to run with the state machine
//|         :param int frequency: the target clock frequency of the state machine. Actual may be less. Use 0 for system clock speed.
//|         :param ReadableBuffer init: a program to run once at start up. This is run after program
//|              is started so instructions may be intermingled
//|         :param ReadableBuffer may_exec: Instructions that may be executed via `StateMachine.run` calls.
//|             Some elements of the `StateMachine`'s configuration are inferred from the instructions used;
//|             for instance, if there is no ``in`` or ``push`` instruction, then the `StateMachine` is configured without a receive FIFO.
//|             In this case, passing a ``may_exec`` program containing an ``in`` instruction such as ``in x``, a receive FIFO will be configured.
//|         :param ~microcontroller.Pin first_out_pin: the first pin to use with the OUT instruction
//|         :param int initial_out_pin_state: the initial output value for out pins starting at first_out_pin
//|         :param int initial_out_pin_direction: the initial output direction for out pins starting at first_out_pin
//|         :param ~microcontroller.Pin first_in_pin: the first pin to use with the IN instruction
//|         :param int pull_in_pin_up: a 1-bit in this mask sets pull up on the corresponding in pin
//|         :param int pull_in_pin_down: a 1-bit in this mask sets pull down on the corresponding in pin. Setting both pulls enables a "bus keep" function, i.e. a weak pull to whatever is current high/low state of GPIO.
//|         :param ~microcontroller.Pin first_set_pin: the first pin to use with the SET instruction
//|         :param int initial_set_pin_state: the initial output value for set pins starting at first_set_pin
//|         :param int initial_set_pin_direction: the initial output direction for set pins starting at first_set_pin
//|         :param ~microcontroller.Pin first_sideset_pin: the first pin to use with a side set
//|         :param int initial_sideset_pin_state: the initial output value for sideset pins starting at first_sideset_pin
//|         :param int initial_sideset_pin_direction: the initial output direction for sideset pins starting at first_sideset_pin
//|         :param bool sideset_enable: True when the top sideset bit is to enable. This should be used with the ".side_set # opt" directive
//|         :param ~microcontroller.Pin jmp_pin: the pin which determines the branch taken by JMP PIN instructions
//|         :param ~digitalio.Pull jmp_pin_pull: The pull value for the jmp pin, default is no pull.
//|         :param bool exclusive_pin_use: When True, do not share any pins with other state machines. Pins are never shared with other peripherals
//|         :param bool wait_for_txstall: When True, writing data out will block until the TX FIFO and OSR are empty
//|             and an instruction is stalled waiting for more data. When False, data writes won't
//|             wait for the OSR to empty (only the TX FIFO) so make sure you give enough time before
//|             deiniting or stopping the state machine.
//|         :param bool user_interruptible: When True (the default),
//|             `write()`, `readinto()`, and `write_readinto()` can be interrupted by a ctrl-C.
//|             This is useful when developing a PIO program: if there is an error in the program
//|             that causes an infinite loop, you will be able to interrupt the loop.
//|             However, if you are writing to a device that can get into a bad state if a read or write
//|             is interrupted, you may want to set this to False after your program has been vetted.
//|         :param int offset: A specific offset in the state machine's program memory where the program must be loaded.
//|             The default value, -1, allows the program to be loaded at any offset.
//|             This is appropriate for most programs.
//|
//|         The following parameters are usually set via assembler directives and passed using a ``**program.pio_kwargs`` argument but may also be specified directly:
//|
//|         :param int out_pin_count: the count of consecutive pins to use with OUT starting at first_out_pin
//|         :param int in_pin_count: the count of consecutive pins to use with IN starting at first_in_pin
//|         :param int set_pin_count: the count of consecutive pins to use with SET starting at first_set_pin
//|         :param int sideset_pin_count: the count of consecutive pins to use with a side set starting at first_sideset_pin. Does not include sideset enable
//|         :param bool sideset_pindirs: `True` to indicate that the side set values should be applied to the PINDIRs and not the PINs
//|         :param int pio_version: The version of the PIO peripheral required by the program. The constructor will raise an error if the actual hardware is not compatible with this program version.
//|         :param bool auto_push: When True, automatically save data from input shift register
//|              (ISR) into the rx FIFO when an IN instruction shifts more than push_threshold bits
//|         :param int push_threshold: Number of bits to shift before saving the ISR value to the RX FIFO
//|         :param bool in_shift_right: When True, data is shifted into the right side (LSB) of the
//|             ISR. It is shifted into the left (MSB) otherwise. NOTE! This impacts data alignment
//|             when the number of bytes is not a power of two (1, 2 or 4 bytes).
//|         :param bool auto_pull: When True, automatically load data from the tx FIFO into the
//|             output shift register (OSR) when an OUT instruction shifts more than pull_threshold bits
//|         :param int pull_threshold: Number of bits to shift before loading a new value into the OSR from the tx FIFO
//|         :param bool out_shift_right: When True, data is shifted out the right side (LSB) of the
//|             OSR. It is shifted out the left (MSB) otherwise. NOTE! This impacts data alignment
//|             when the number of bytes is not a power of two (1, 2 or 4 bytes).
//|         :param int wrap_target: The target instruction number of automatic wrap. Defaults to the first instruction of the program.
//|         :param int wrap: The instruction after which to wrap to the ``wrap``
//|             instruction. As a special case, -1 (the default) indicates the
//|             last instruction of the program.
//|         :param FifoType fifo_type: How the program accessess the FIFOs. PIO version 0 in the RP2040 only supports a subset of values, `FifoType_piov0`.
//|         :param MovStatusType mov_status_type: What condition the ``mov status`` instruction checks. PIO version 0 in the RP2040 only supports a subset of values, `MovStatusType_piov0`.
//|         :param MovStatusType mov_status_n: The FIFO depth or IRQ the ``mov status`` instruction checks for. For ``mov_status irq`` this includes the encoding of the ``next``/``prev`` selection bits.
//|         """
//|         ...
//|

static int one_of(qstr_short_t what, mp_obj_t arg, size_t n_options, const qstr_short_t options[], const int values[]) {
    for (size_t i = 0; i < n_options; i++) {
        mp_obj_t option_str = MP_OBJ_NEW_QSTR(options[i]);
        if (mp_obj_equal(arg, option_str)) {
            return values[i];
        }
    }
    mp_raise_ValueError_varg(MP_ERROR_TEXT("Invalid %q"), what);
}

static mp_obj_t rp2pio_statemachine_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    rp2pio_statemachine_obj_t *self = mp_obj_malloc(rp2pio_statemachine_obj_t, &rp2pio_statemachine_type);
    enum { ARG_program, ARG_frequency, ARG_init, ARG_pio_version, ARG_may_exec,
           ARG_first_out_pin, ARG_out_pin_count, ARG_initial_out_pin_state, ARG_initial_out_pin_direction,
           ARG_first_in_pin, ARG_in_pin_count,
           ARG_pull_in_pin_up, ARG_pull_in_pin_down,
           ARG_first_set_pin, ARG_set_pin_count, ARG_initial_set_pin_state, ARG_initial_set_pin_direction,
           ARG_first_sideset_pin, ARG_sideset_pin_count, ARG_sideset_pindirs,
           ARG_initial_sideset_pin_state, ARG_initial_sideset_pin_direction,
           ARG_sideset_enable,
           ARG_jmp_pin, ARG_jmp_pin_pull,
           ARG_exclusive_pin_use,
           ARG_auto_pull, ARG_pull_threshold, ARG_out_shift_right,
           ARG_wait_for_txstall,
           ARG_auto_push, ARG_push_threshold, ARG_in_shift_right,
           ARG_user_interruptible,
           ARG_wrap_target,
           ARG_wrap,
           ARG_offset,
           ARG_fifo_type,
           ARG_mov_status_type,
           ARG_mov_status_n, };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_program, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_frequency, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_init, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_pio_version, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_may_exec, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },

        { MP_QSTR_first_out_pin, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_out_pin_count, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 1} },
        { MP_QSTR_initial_out_pin_state, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_initial_out_pin_direction, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0xffffffff} },

        { MP_QSTR_first_in_pin, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_in_pin_count, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 1} },
        { MP_QSTR_pull_in_pin_up, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_pull_in_pin_down, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },

        { MP_QSTR_first_set_pin, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_set_pin_count, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 1} },
        { MP_QSTR_initial_set_pin_state, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_initial_set_pin_direction, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0x1f} },

        { MP_QSTR_first_sideset_pin, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_sideset_pin_count, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 1} },
        { MP_QSTR_sideset_pindirs, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_initial_sideset_pin_state, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_initial_sideset_pin_direction, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0x1f} },

        { MP_QSTR_sideset_enable, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },

        { MP_QSTR_jmp_pin, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_jmp_pin_pull, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },

        { MP_QSTR_exclusive_pin_use, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = true} },
        { MP_QSTR_auto_pull, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_pull_threshold, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 32} },
        { MP_QSTR_out_shift_right, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = true} },
        { MP_QSTR_wait_for_txstall, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = true} },
        { MP_QSTR_auto_push, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_push_threshold, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 32} },
        { MP_QSTR_in_shift_right, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = true} },
        { MP_QSTR_user_interruptible, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = true} },

        { MP_QSTR_wrap_target, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_wrap, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },

        { MP_QSTR_offset, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = PIO_ANY_OFFSET} },

        { MP_QSTR_fifo_type, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_ROM_QSTR(MP_QSTR_auto) } },
        { MP_QSTR_mov_status_type, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_ROM_QSTR(MP_QSTR_txfifo) } },
        { MP_QSTR_mov_status_n, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    (void)mp_arg_validate_int_max(args[ARG_pio_version].u_int, PICO_PIO_VERSION, MP_QSTR_out_pin_count);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_program].u_obj, &bufinfo, MP_BUFFER_READ);

    mp_buffer_info_t init_bufinfo;
    init_bufinfo.len = 0;
    mp_get_buffer(args[ARG_init].u_obj, &init_bufinfo, MP_BUFFER_READ);

    mp_buffer_info_t may_exec_bufinfo;
    may_exec_bufinfo.len = 0;
    mp_get_buffer(args[ARG_may_exec].u_obj, &may_exec_bufinfo, MP_BUFFER_READ);

    // We don't validate pin in use here because we may be ok sharing them within a PIO.
    const mcu_pin_obj_t *first_out_pin =
        validate_obj_is_pin_or_none(args[ARG_first_out_pin].u_obj, MP_QSTR_first_out_pin);
    mp_int_t out_pin_count = mp_arg_validate_int_min(args[ARG_out_pin_count].u_int, 1, MP_QSTR_out_pin_count);

    const mcu_pin_obj_t *first_in_pin =
        validate_obj_is_pin_or_none(args[ARG_first_in_pin].u_obj, MP_QSTR_first_in_pin);
    mp_int_t in_pin_count = mp_arg_validate_int_min(args[ARG_in_pin_count].u_int, 1, MP_QSTR_in_pin_count);

    const mcu_pin_obj_t *first_set_pin =
        validate_obj_is_pin_or_none(args[ARG_first_set_pin].u_obj, MP_QSTR_first_set_pin);
    mp_int_t set_pin_count = mp_arg_validate_int_range(args[ARG_set_pin_count].u_int, 1, 5, MP_QSTR_set_pin_count);

    const mcu_pin_obj_t *first_sideset_pin =
        validate_obj_is_pin_or_none(args[ARG_first_sideset_pin].u_obj, MP_QSTR_first_sideset_pin);
    mp_int_t sideset_pin_count =
        mp_arg_validate_int_range(args[ARG_sideset_pin_count].u_int, 1, 5, MP_QSTR_set_pin_count);

    const mcu_pin_obj_t *jmp_pin =
        validate_obj_is_pin_or_none(args[ARG_jmp_pin].u_obj, MP_QSTR_jmp_pin);
    digitalio_pull_t jmp_pin_pull = validate_pull(args[ARG_jmp_pin_pull].u_rom_obj, MP_QSTR_jmp_pull);

    mp_int_t pull_threshold =
        mp_arg_validate_int_range(args[ARG_pull_threshold].u_int, 1, 32, MP_QSTR_pull_threshold);
    mp_int_t push_threshold =
        mp_arg_validate_int_range(args[ARG_push_threshold].u_int, 1, 32, MP_QSTR_push_threshold);

    mp_arg_validate_length_range(bufinfo.len, 2, 64, MP_QSTR_program);
    if (bufinfo.len % 2 != 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("Program size invalid"));
    }

    mp_arg_validate_length_range(init_bufinfo.len, 0, 64, MP_QSTR_init);
    if (init_bufinfo.len % 2 != 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("Init program size invalid"));
    }

    int wrap = args[ARG_wrap].u_int;
    int wrap_target = args[ARG_wrap_target].u_int;

    const qstr_short_t fifo_alternatives[] = { MP_QSTR_auto, MP_QSTR_txrx, MP_QSTR_tx, MP_QSTR_rx,
                                               #if PICO_PIO_VERSION > 0
                                               MP_QSTR_txput, MP_QSTR_txget, MP_QSTR_putget
                                               #endif
    };
    const int fifo_values[] = { PIO_FIFO_JOIN_AUTO, PIO_FIFO_JOIN_NONE, PIO_FIFO_JOIN_TX, PIO_FIFO_JOIN_RX,
                                #if PICO_PIO_VERSION > 0
                                PIO_FIFO_JOIN_TXPUT, PIO_FIFO_JOIN_TXGET, PIO_FIFO_JOIN_PUTGET
                                #endif
    };
    MP_STATIC_ASSERT(MP_ARRAY_SIZE(fifo_alternatives) == MP_ARRAY_SIZE(fifo_values));

    int fifo_type = one_of(MP_QSTR_fifo_type, args[ARG_fifo_type].u_obj, MP_ARRAY_SIZE(fifo_alternatives), fifo_alternatives, fifo_values);

    const qstr_short_t mov_status_alternatives[] = { MP_QSTR_txfifo, MP_QSTR_rxfifo,
                                                     #if PICO_PIO_VERSION > 0
                                                     MP_QSTR_IRQ
                                                     #endif
    };
    const int mov_status_values[] = { STATUS_TX_LESSTHAN, STATUS_RX_LESSTHAN,
                                      #if PICO_PIO_VERSION > 0
                                      STATUS_IRQ_SET
                                      #endif
    };
    MP_STATIC_ASSERT(MP_ARRAY_SIZE(mov_status_alternatives) == MP_ARRAY_SIZE(mov_status_values));
    int mov_status_type = one_of(MP_QSTR_mov_status_type, args[ARG_mov_status_type].u_obj, MP_ARRAY_SIZE(mov_status_alternatives), mov_status_alternatives, mov_status_values);

    common_hal_rp2pio_statemachine_construct(self,
        bufinfo.buf, bufinfo.len / 2,
        args[ARG_frequency].u_int,
        init_bufinfo.buf, init_bufinfo.len / 2,
        may_exec_bufinfo.buf, may_exec_bufinfo.len / 2,
        first_out_pin, out_pin_count, PIO_PINMASK32_FROM_VALUE(args[ARG_initial_out_pin_state].u_int), PIO_PINMASK32_FROM_VALUE(args[ARG_initial_out_pin_direction].u_int),
        first_in_pin, in_pin_count, PIO_PINMASK32_FROM_VALUE(args[ARG_pull_in_pin_up].u_int), PIO_PINMASK32_FROM_VALUE(args[ARG_pull_in_pin_down].u_int),
        first_set_pin, set_pin_count, PIO_PINMASK32_FROM_VALUE(args[ARG_initial_set_pin_state].u_int), PIO_PINMASK32_FROM_VALUE(args[ARG_initial_set_pin_direction].u_int),
        first_sideset_pin, sideset_pin_count, args[ARG_sideset_pindirs].u_bool,
        PIO_PINMASK32_FROM_VALUE(args[ARG_initial_sideset_pin_state].u_int), PIO_PINMASK32_FROM_VALUE(args[ARG_initial_sideset_pin_direction].u_int),
        args[ARG_sideset_enable].u_bool,
        jmp_pin, jmp_pin_pull,
        PIO_PINMASK_FROM_VALUE(0), // wait_gpio_mask
        args[ARG_exclusive_pin_use].u_bool,
        args[ARG_auto_pull].u_bool, pull_threshold, args[ARG_out_shift_right].u_bool,
        args[ARG_wait_for_txstall].u_bool,
        args[ARG_auto_push].u_bool, push_threshold, args[ARG_in_shift_right].u_bool,
        args[ARG_user_interruptible].u_bool,
        wrap_target, wrap, args[ARG_offset].u_int,
        fifo_type,
        mov_status_type, args[ARG_mov_status_n].u_int
        );
    return MP_OBJ_FROM_PTR(self);
}

//|     def deinit(self) -> None:
//|         """Turn off the state machine and release its resources."""
//|         ...
//|
static mp_obj_t rp2pio_statemachine_obj_deinit(mp_obj_t self_in) {
    rp2pio_statemachine_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_rp2pio_statemachine_deinit(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(rp2pio_statemachine_deinit_obj, rp2pio_statemachine_obj_deinit);

//|     def __enter__(self) -> StateMachine:
//|         """No-op used by Context Managers.
//|         Provided by context manager helper."""
//|         ...
//|

//|     def __exit__(self) -> None:
//|         """Automatically deinitializes the hardware when exiting a context. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
//|


static void check_for_deinit(rp2pio_statemachine_obj_t *self) {
    if (common_hal_rp2pio_statemachine_deinited(self)) {
        raise_deinited_error();
    }
}

//|     def restart(self) -> None:
//|         """Resets this state machine, runs any init and enables the clock."""
//|         ...
//|
// TODO: "and any others given. They must share an underlying PIO. An exception will be raised otherwise.""
static mp_obj_t rp2pio_statemachine_restart(mp_obj_t self_obj) {
    rp2pio_statemachine_obj_t *self = MP_OBJ_TO_PTR(self_obj);
    check_for_deinit(self);

    common_hal_rp2pio_statemachine_restart(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(rp2pio_statemachine_restart_obj, rp2pio_statemachine_restart);


//|     def run(self, instructions: ReadableBuffer) -> None:
//|         """Runs all given instructions. They will likely be interleaved with
//|         in-memory instructions. Make sure this doesn't wait for input!
//|
//|         This can be used to output internal state to the RX FIFO and then
//|         read with `readinto`."""
//|         ...
//|
static mp_obj_t rp2pio_statemachine_run(mp_obj_t self_obj, mp_obj_t instruction_obj) {
    rp2pio_statemachine_obj_t *self = MP_OBJ_TO_PTR(self_obj);
    check_for_deinit(self);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(instruction_obj, &bufinfo, MP_BUFFER_READ);

    if (bufinfo.len % 2 != 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("Program size invalid"));
    }
    common_hal_rp2pio_statemachine_run(self, bufinfo.buf, (size_t)bufinfo.len / 2);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(rp2pio_statemachine_run_obj, rp2pio_statemachine_run);

//|     def stop(self) -> None:
//|         """Stops the state machine clock. Use `restart` to enable it."""
//|         ...
//|
static mp_obj_t rp2pio_statemachine_stop(mp_obj_t self_obj) {
    rp2pio_statemachine_obj_t *self = MP_OBJ_TO_PTR(self_obj);
    check_for_deinit(self);

    common_hal_rp2pio_statemachine_stop(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(rp2pio_statemachine_stop_obj, rp2pio_statemachine_stop);

//|     def write(
//|         self,
//|         buffer: ReadableBuffer,
//|         *,
//|         start: int = 0,
//|         end: Optional[int] = None,
//|         swap: bool = False,
//|     ) -> None:
//|         """Write the data contained in ``buffer`` to the state machine. If the buffer is empty, nothing happens.
//|
//|         Writes to the FIFO will match the input buffer's element size. For example, bytearray elements
//|         will perform 8 bit writes to the PIO FIFO. The RP2040's memory bus will duplicate the value into
//|         the other byte positions. So, pulling more data in the PIO assembly will read the duplicated values.
//|
//|         To perform 16 or 32 bits writes into the FIFO use an `array.array` with a type code of the desired
//|         size.
//|
//|         :param ~circuitpython_typing.ReadableBuffer buffer: Write out the data in this buffer
//|         :param int start: Start of the slice of ``buffer`` to write out: ``buffer[start:end]``
//|         :param int end: End of the slice; this index is not included. Defaults to ``len(buffer)``
//|         :param bool swap: For 2- and 4-byte elements, swap (reverse) the byte order"""
//|         ...
//|
static mp_obj_t rp2pio_statemachine_write(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_buffer, ARG_start, ARG_end, ARG_swap };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_buffer,     MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_start,      MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_end,        MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = INT_MAX} },
        { MP_QSTR_swap,       MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
    };
    rp2pio_statemachine_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_for_deinit(self);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_buffer].u_obj, &bufinfo, MP_BUFFER_READ);
    int stride_in_bytes = mp_binary_get_size('@', bufinfo.typecode, NULL);
    if (stride_in_bytes > 4) {
        mp_raise_ValueError(MP_ERROR_TEXT("Buffer elements must be 4 bytes long or less"));
    }
    int32_t start = args[ARG_start].u_int;
    size_t length = bufinfo.len / stride_in_bytes;
    // Normalize in element size units, not bytes.
    normalize_buffer_bounds(&start, args[ARG_end].u_int, &length);

    // Treat start and length in terms of bytes from now on.
    start *= stride_in_bytes;
    length *= stride_in_bytes;

    if (length == 0) {
        return mp_const_none;
    }

    bool ok = common_hal_rp2pio_statemachine_write(self, ((uint8_t *)bufinfo.buf) + start, length, stride_in_bytes, args[ARG_swap].u_bool);
    if (mp_hal_is_interrupted()) {
        return mp_const_none;
    }
    if (!ok) {
        mp_raise_OSError(MP_EIO);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(rp2pio_statemachine_write_obj, 2, rp2pio_statemachine_write);

//|     def background_write(
//|         self,
//|         once: Optional[ReadableBuffer] = None,
//|         *,
//|         loop: Optional[ReadableBuffer] = None,
//|         loop2: Optional[ReadableBuffer] = None,
//|         swap: bool = False,
//|     ) -> None:
//|         """Write data to the TX fifo in the background, with optional looping.
//|
//|         First, if any previous ``once`` or ``loop`` buffer has not been started, this function blocks until they have been started.
//|         This means that any ``once`` or ``loop`` buffer will be written at least once.
//|         Then the ``once`` and/or ``loop`` buffers are queued. and the function returns.
//|         The ``once`` buffer (if specified) will be written just once.
//|         Finally, the ``loop`` and/or ``loop2`` buffer (if specified) will continue being looped indefinitely.  If both ``loop`` and ``loop2`` are specified, they will alternate.
//|
//|         Writes to the FIFO will match the input buffer's element size. For example, bytearray elements
//|         will perform 8 bit writes to the PIO FIFO. The RP2040's memory bus will duplicate the value into
//|         the other byte positions. So, pulling more data in the PIO assembly will read the duplicated values.
//|
//|         To perform 16 or 32 bits writes into the FIFO use an `array.array` with a type code of the desired
//|         size, or use `memoryview.cast` to change the interpretation of an
//|         existing buffer.  To send just part of a larger buffer, slice a `memoryview`
//|         of it.
//|
//|         If a buffer is modified while it is being written out, the updated
//|         values will be used. However, because of interactions between CPU
//|         writes, DMA and the PIO FIFO are complex, it is difficult to predict
//|         the result of modifying multiple values. Instead, alternate between
//|         a pair of buffers.
//|
//|         Having both a ``once`` and a ``loop`` parameter is to support a special case in PWM generation
//|         where a change in duty cycle requires a special transitional buffer to be used exactly once. Most
//|         use cases will probably only use one of ``once`` or ``loop``.
//|
//|         Having neither ``once`` nor ``loop`` terminates an existing
//|         background looping write after exactly a whole loop. This is in contrast to
//|         `stop_background_write`, which interrupts an ongoing DMA operation.
//|
//|         :param ~Optional[circuitpython_typing.ReadableBuffer] once: Data to be written once
//|         :param ~Optional[circuitpython_typing.ReadableBuffer] loop: Data to be written repeatedly
//|         :param ~Optional[circuitpython_typing.ReadableBuffer] loop2: Data to be written repeatedly
//|         :param bool swap: For 2- and 4-byte elements, swap (reverse) the byte order
//|         """
//|         ...
//|

static void fill_buf_info(sm_buf_info *info, mp_obj_t obj, size_t *stride_in_bytes, mp_uint_t direction) {
    if (obj != mp_const_none) {
        info->obj = obj;
        mp_get_buffer_raise(obj, &info->info, direction);
        size_t stride = mp_binary_get_size('@', info->info.typecode, NULL);
        if (stride > 4) {
            mp_raise_ValueError(MP_ERROR_TEXT("Buffer elements must be 4 bytes long or less"));
        }
        if (*stride_in_bytes && stride != *stride_in_bytes) {
            mp_raise_ValueError(MP_ERROR_TEXT("Mismatched data size"));
        }
        *stride_in_bytes = stride;
    } else {
        memset(info, 0, sizeof(*info));
    }
}

static mp_obj_t rp2pio_statemachine_background_write(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_once, ARG_loop, ARG_loop2, ARG_swap };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_once,     MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_loop,     MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = mp_const_none} },
        { MP_QSTR_loop2,    MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = mp_const_none} },
        { MP_QSTR_swap,     MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
    };
    rp2pio_statemachine_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_for_deinit(self);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    size_t stride_in_bytes = 0;

    fill_buf_info(&self->once_write_buf_info, args[ARG_once].u_obj, &stride_in_bytes, MP_BUFFER_READ);
    fill_buf_info(&self->loop_write_buf_info, args[ARG_loop].u_obj, &stride_in_bytes, MP_BUFFER_READ);
    fill_buf_info(&self->loop2_write_buf_info, args[ARG_loop2].u_obj, &stride_in_bytes, MP_BUFFER_READ);

    if (!stride_in_bytes) {
        return mp_const_none;
    }

    bool ok = common_hal_rp2pio_statemachine_background_write(self, stride_in_bytes, args[ARG_swap].u_bool);

    if (mp_hal_is_interrupted()) {
        return mp_const_none;
    }
    if (!ok) {
        mp_raise_OSError(MP_EIO);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(rp2pio_statemachine_background_write_obj, 1, rp2pio_statemachine_background_write);

//|     def stop_background_write(self) -> None:
//|         """Immediately stop a background write, if one is in progress.  Any
//|         DMA in progress is halted, but items already in the TX FIFO are not
//|         affected."""
//|
static mp_obj_t rp2pio_statemachine_obj_stop_background_write(mp_obj_t self_in) {
    rp2pio_statemachine_obj_t *self = MP_OBJ_TO_PTR(self_in);
    bool ok = common_hal_rp2pio_statemachine_stop_background_write(self);
    if (mp_hal_is_interrupted()) {
        return mp_const_none;
    }
    if (!ok) {
        mp_raise_OSError(MP_EIO);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(rp2pio_statemachine_stop_background_write_obj, rp2pio_statemachine_obj_stop_background_write);


//|     writing: bool
//|     """Returns True if a background write is in progress"""
static mp_obj_t rp2pio_statemachine_obj_get_writing(mp_obj_t self_in) {
    rp2pio_statemachine_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(common_hal_rp2pio_statemachine_get_writing(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(rp2pio_statemachine_get_writing_obj, rp2pio_statemachine_obj_get_writing);

MP_PROPERTY_GETTER(rp2pio_statemachine_writing_obj,
    (mp_obj_t)&rp2pio_statemachine_get_writing_obj);

//|     pending_write: int
//|     pending: int
//|     """Returns the number of pending buffers for background writing.
//|
//|     If the number is 0, then a `StateMachine.background_write` call will not block.
//|     Note that `pending` is a deprecated alias for `pending_write` and will be removed
//|     in a future version of CircuitPython."""
//|


static mp_obj_t rp2pio_statemachine_obj_get_pending_write(mp_obj_t self_in) {
    rp2pio_statemachine_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int(common_hal_rp2pio_statemachine_get_pending_write(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(rp2pio_statemachine_get_pending_write_obj, rp2pio_statemachine_obj_get_pending_write);

MP_PROPERTY_GETTER(rp2pio_statemachine_pending_obj,
    (mp_obj_t)&rp2pio_statemachine_get_pending_write_obj);

MP_PROPERTY_GETTER(rp2pio_statemachine_pending_write_obj,
    (mp_obj_t)&rp2pio_statemachine_get_pending_write_obj);


// =================================================================================================================================

//|     def background_read(
//|         self,
//|         once: Optional[WriteableBuffer] = None,
//|         *,
//|         loop: Optional[WriteableBuffer] = None,
//|         loop2: Optional[WriteableBuffer] = None,
//|         swap: bool = False,
//|     ) -> None:
//|         """Read data from the RX fifo in the background, with optional looping.
//|
//|         First, if any previous ``once`` or ``loop`` buffer has not been started, this function blocks until they have been started.
//|         This means that any ``once`` or ``loop`` buffer will be read at least once.
//|         Then the ``once`` and/or ``loop`` buffers are queued. and the function returns.
//|         The ``once`` buffer (if specified) will be read just once.
//|         Finally, the ``loop`` and/or ``loop2`` buffer (if specified) will continue being read indefinitely.  If both ``loop`` and ``loop2`` are specified, they will alternate.
//|
//|         Reads from the FIFO will match the input buffer's element size. For example, bytearray elements
//|         will perform 8 bit reads from the PIO FIFO. The RP2040's memory bus will duplicate the value into
//|         the other byte positions. So, pulling more data in the PIO assembly will read the duplicated values.
//|
//|         To perform 16 or 32 bits reads from the FIFO use an `array.array` with a type code of the desired
//|         size, or use `memoryview.cast` to change the interpretation of an
//|         existing buffer.  To receive just part of a larger buffer, slice a `memoryview`
//|         of it.
//|
//|         Most use cases will probably only use one of ``once`` or ``loop``.
//|
//|         Having neither ``once`` nor ``loop`` terminates an existing
//|         background looping read after exactly a whole loop. This is in contrast to
//|         `stop_background_read`, which interrupts an ongoing DMA operation.
//|
//|         :param ~Optional[circuitpython_typing.WriteableBuffer] once: Data to be read once
//|         :param ~Optional[circuitpython_typing.WriteableBuffer] loop: Data to be read repeatedly
//|         :param ~Optional[circuitpython_typing.WriteableBuffer] loop2: Data to be read repeatedly
//|         :param bool swap: For 2- and 4-byte elements, swap (reverse) the byte order
//|         """
//|         ...
//|


static mp_obj_t rp2pio_statemachine_background_read(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_once, ARG_loop, ARG_loop2, ARG_swap };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_once,     MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_loop,     MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = mp_const_none} },
        { MP_QSTR_loop2,    MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = mp_const_none} },
        { MP_QSTR_swap,     MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
    };
    rp2pio_statemachine_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_for_deinit(self);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    size_t stride_in_bytes = 0;

    fill_buf_info(&self->once_read_buf_info, args[ARG_once].u_obj, &stride_in_bytes, MP_BUFFER_WRITE);
    fill_buf_info(&self->loop_read_buf_info, args[ARG_loop].u_obj, &stride_in_bytes, MP_BUFFER_WRITE);
    fill_buf_info(&self->loop2_read_buf_info, args[ARG_loop2].u_obj, &stride_in_bytes, MP_BUFFER_WRITE);

    if (!stride_in_bytes) {
        return mp_const_none;
    }

    bool ok = common_hal_rp2pio_statemachine_background_read(self, stride_in_bytes, args[ARG_swap].u_bool);

    if (mp_hal_is_interrupted()) {
        return mp_const_none;
    }
    if (!ok) {
        mp_raise_OSError(MP_EIO);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(rp2pio_statemachine_background_read_obj, 1, rp2pio_statemachine_background_read);

//|     def stop_background_read(self) -> None:
//|         """Immediately stop a background read, if one is in progress.  Any
//|         DMA in progress is halted, but items already in the RX FIFO are not
//|         affected."""
//|
static mp_obj_t rp2pio_statemachine_obj_stop_background_read(mp_obj_t self_in) {
    rp2pio_statemachine_obj_t *self = MP_OBJ_TO_PTR(self_in);
    bool ok = common_hal_rp2pio_statemachine_stop_background_read(self);
    if (mp_hal_is_interrupted()) {
        return mp_const_none;
    }
    if (!ok) {
        mp_raise_OSError(MP_EIO);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(rp2pio_statemachine_stop_background_read_obj, rp2pio_statemachine_obj_stop_background_read);

//|     reading: bool
//|     """Returns True if a background read is in progress"""
static mp_obj_t rp2pio_statemachine_obj_get_reading(mp_obj_t self_in) {
    rp2pio_statemachine_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(common_hal_rp2pio_statemachine_get_reading(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(rp2pio_statemachine_get_reading_obj, rp2pio_statemachine_obj_get_reading);

MP_PROPERTY_GETTER(rp2pio_statemachine_reading_obj,
    (mp_obj_t)&rp2pio_statemachine_get_reading_obj);

//|     pending_read: int
//|     """Returns the number of pending buffers for background reading.
//|
//|     If the number is 0, then a `StateMachine.background_read` call will not block."""
//|
static mp_obj_t rp2pio_statemachine_obj_get_pending_read(mp_obj_t self_in) {
    rp2pio_statemachine_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int(common_hal_rp2pio_statemachine_get_pending_read(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(rp2pio_statemachine_get_pending_read_obj, rp2pio_statemachine_obj_get_pending_read);

MP_PROPERTY_GETTER(rp2pio_statemachine_pending_read_obj,
    (mp_obj_t)&rp2pio_statemachine_get_pending_read_obj);


// =================================================================================================================================

//|     def readinto(
//|         self,
//|         buffer: WriteableBuffer,
//|         *,
//|         start: int = 0,
//|         end: Optional[int] = None,
//|         swap: bool = False,
//|     ) -> None:
//|         """Read into ``buffer``. If the number of bytes to read is 0, nothing happens. The buffer
//|         includes any data added to the fifo even if it was added before this was called.
//|
//|         Reads from the FIFO will match the input buffer's element size. For example, bytearray elements
//|         will perform 8 bit reads from the PIO FIFO. The alignment within the 32 bit value depends on
//|         ``in_shift_right``. When ``in_shift_right`` is True, the upper N bits will be read. The lower
//|         bits will be read when ``in_shift_right`` is False.
//|
//|         To perform 16 or 32 bits writes into the FIFO use an `array.array` with a type code of the desired
//|         size.
//|
//|         :param ~circuitpython_typing.WriteableBuffer buffer: Read data into this buffer
//|         :param int start: Start of the slice of ``buffer`` to read into: ``buffer[start:end]``
//|         :param int end: End of the slice; this index is not included. Defaults to ``len(buffer)``
//|         :param bool swap: For 2- and 4-byte elements, swap (reverse) the byte order"""
//|         ...
//|

static mp_obj_t rp2pio_statemachine_readinto(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_buffer, ARG_start, ARG_end, ARG_swap };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_buffer,     MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_start,      MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_end,        MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = INT_MAX} },
        { MP_QSTR_swap,       MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
    };
    rp2pio_statemachine_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_for_deinit(self);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_buffer].u_obj, &bufinfo, MP_BUFFER_WRITE);
    int stride_in_bytes = mp_binary_get_size('@', bufinfo.typecode, NULL);
    if (stride_in_bytes > 4) {
        mp_raise_ValueError(MP_ERROR_TEXT("Buffer elements must be 4 bytes long or less"));
    }
    int32_t start = args[ARG_start].u_int;
    size_t length = bufinfo.len / stride_in_bytes;
    normalize_buffer_bounds(&start, args[ARG_end].u_int, &length);

    // Treat start and length in terms of bytes from now on.
    start *= stride_in_bytes;
    length *= stride_in_bytes;
    if (length == 0) {
        return mp_const_none;
    }

    bool ok = common_hal_rp2pio_statemachine_readinto(self, ((uint8_t *)bufinfo.buf) + start, length, stride_in_bytes, args[ARG_swap].u_bool);
    if (!ok) {
        mp_raise_OSError(MP_EIO);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(rp2pio_statemachine_readinto_obj, 2, rp2pio_statemachine_readinto);

//|     def write_readinto(
//|         self,
//|         buffer_out: ReadableBuffer,
//|         buffer_in: WriteableBuffer,
//|         *,
//|         out_start: int = 0,
//|         out_end: Optional[int] = None,
//|         in_start: int = 0,
//|         in_end: Optional[int] = None,
//|     ) -> None:
//|         """Write out the data in ``buffer_out`` while simultaneously reading data into ``buffer_in``.
//|         The lengths of the slices defined by ``buffer_out[out_start:out_end]`` and ``buffer_in[in_start:in_end]``
//|         may be different. The function will return once both are filled.
//|         If buffer slice lengths are both 0, nothing happens.
//|
//|         Data transfers to and from the FIFOs will match the corresponding buffer's element size. See
//|         `write` and `readinto` for details.
//|
//|         To perform 16 or 32 bits writes into the FIFO use an `array.array` with a type code of the desired
//|         size.
//|
//|         :param ~circuitpython_typing.ReadableBuffer buffer_out: Write out the data in this buffer
//|         :param ~circuitpython_typing.WriteableBuffer buffer_in: Read data into this buffer
//|         :param int out_start: Start of the slice of buffer_out to write out: ``buffer_out[out_start:out_end]``
//|         :param int out_end: End of the slice; this index is not included. Defaults to ``len(buffer_out)``
//|         :param int in_start: Start of the slice of ``buffer_in`` to read into: ``buffer_in[in_start:in_end]``
//|         :param int in_end: End of the slice; this index is not included. Defaults to ``len(buffer_in)``
//|         :param bool swap_out: For 2- and 4-byte elements, swap (reverse) the byte order for the buffer being transmitted (written)
//|         :param bool swap_in: For 2- and 4-rx elements, swap (reverse) the byte order for the buffer being received (read)
//|         """
//|         ...
//|

static mp_obj_t rp2pio_statemachine_write_readinto(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_buffer_out, ARG_buffer_in, ARG_out_start, ARG_out_end, ARG_in_start, ARG_in_end, ARG_swap_out, ARG_swap_in };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_buffer_out,    MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_buffer_in,     MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_out_start,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_out_end,       MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = INT_MAX} },
        { MP_QSTR_in_start,      MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_in_end,        MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = INT_MAX} },
        { MP_QSTR_swap_out,      MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_swap_in,       MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
    };
    rp2pio_statemachine_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_for_deinit(self);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_buffer_info_t buf_out_info;
    mp_get_buffer_raise(args[ARG_buffer_out].u_obj, &buf_out_info, MP_BUFFER_READ);
    int out_stride_in_bytes = mp_binary_get_size('@', buf_out_info.typecode, NULL);
    if (out_stride_in_bytes > 4) {
        mp_raise_ValueError(MP_ERROR_TEXT("Out-buffer elements must be <= 4 bytes long"));
    }
    int32_t out_start = args[ARG_out_start].u_int;
    size_t out_length = buf_out_info.len / out_stride_in_bytes;
    normalize_buffer_bounds(&out_start, args[ARG_out_end].u_int, &out_length);

    mp_buffer_info_t buf_in_info;
    mp_get_buffer_raise(args[ARG_buffer_in].u_obj, &buf_in_info, MP_BUFFER_WRITE);
    int in_stride_in_bytes = mp_binary_get_size('@', buf_in_info.typecode, NULL);
    if (in_stride_in_bytes > 4) {
        mp_raise_ValueError(MP_ERROR_TEXT("In-buffer elements must be <= 4 bytes long"));
    }
    int32_t in_start = args[ARG_in_start].u_int;
    size_t in_length = buf_in_info.len / in_stride_in_bytes;
    normalize_buffer_bounds(&in_start, args[ARG_in_end].u_int, &in_length);

    // Treat start and length in terms of bytes from now on.
    out_start *= out_stride_in_bytes;
    out_length *= out_stride_in_bytes;
    in_start *= in_stride_in_bytes;
    in_length *= in_stride_in_bytes;

    if (out_length == 0 && in_length == 0) {
        return mp_const_none;
    }

    bool ok = common_hal_rp2pio_statemachine_write_readinto(self,
        ((uint8_t *)buf_out_info.buf) + out_start,
        out_length,
        out_stride_in_bytes,
        ((uint8_t *)buf_in_info.buf) + in_start,
        in_length,
        in_stride_in_bytes, args[ARG_swap_out].u_bool, args[ARG_swap_in].u_bool);
    if (!ok) {
        mp_raise_OSError(MP_EIO);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(rp2pio_statemachine_write_readinto_obj, 2, rp2pio_statemachine_write_readinto);

//|     def clear_rxfifo(self) -> None:
//|         """Clears any unread bytes in the rxfifo."""
//|         ...
//|
static mp_obj_t rp2pio_statemachine_obj_clear_rxfifo(mp_obj_t self_in) {
    rp2pio_statemachine_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_rp2pio_statemachine_clear_rxfifo(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(rp2pio_statemachine_clear_rxfifo_obj, rp2pio_statemachine_obj_clear_rxfifo);

//|     def clear_txstall(self) -> None:
//|         """Clears the txstall flag."""
//|         ...
//|
static mp_obj_t rp2pio_statemachine_obj_clear_txstall(mp_obj_t self_in) {
    rp2pio_statemachine_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_rp2pio_statemachine_clear_txstall(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(rp2pio_statemachine_clear_txstall_obj, rp2pio_statemachine_obj_clear_txstall);


//|     frequency: int
//|     """The actual state machine frequency. This may not match the frequency requested
//|     due to internal limitations."""

static mp_obj_t rp2pio_statemachine_obj_get_frequency(mp_obj_t self_in) {
    rp2pio_statemachine_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_rp2pio_statemachine_get_frequency(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(rp2pio_statemachine_get_frequency_obj, rp2pio_statemachine_obj_get_frequency);

static mp_obj_t rp2pio_statemachine_obj_set_frequency(mp_obj_t self_in, mp_obj_t frequency) {
    rp2pio_statemachine_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    common_hal_rp2pio_statemachine_set_frequency(self, mp_obj_get_int(frequency));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(rp2pio_statemachine_set_frequency_obj, rp2pio_statemachine_obj_set_frequency);

MP_PROPERTY_GETSET(rp2pio_statemachine_frequency_obj,
    (mp_obj_t)&rp2pio_statemachine_get_frequency_obj,
    (mp_obj_t)&rp2pio_statemachine_set_frequency_obj);

//|     txstall: bool
//|     """True when the state machine has stalled due to a full TX FIFO since the last
//|        `clear_txstall` call."""

static mp_obj_t rp2pio_statemachine_obj_get_txstall(mp_obj_t self_in) {
    rp2pio_statemachine_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_rp2pio_statemachine_get_txstall(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(rp2pio_statemachine_get_txstall_obj, rp2pio_statemachine_obj_get_txstall);

MP_PROPERTY_GETTER(rp2pio_statemachine_txstall_obj,
    (mp_obj_t)&rp2pio_statemachine_get_txstall_obj);

//|     rxstall: bool
//|     """True when the state machine has stalled due to a full RX FIFO since the last
//|        `clear_rxfifo` call."""

static mp_obj_t rp2pio_statemachine_obj_get_rxstall(mp_obj_t self_in) {
    rp2pio_statemachine_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_rp2pio_statemachine_get_rxstall(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(rp2pio_statemachine_get_rxstall_obj, rp2pio_statemachine_obj_get_rxstall);

MP_PROPERTY_GETTER(rp2pio_statemachine_rxstall_obj,
    (mp_obj_t)&rp2pio_statemachine_get_rxstall_obj);

//|     in_waiting: int
//|     """The number of words available to readinto"""
//|

static mp_obj_t rp2pio_statemachine_obj_get_in_waiting(mp_obj_t self_in) {
    rp2pio_statemachine_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_rp2pio_statemachine_get_in_waiting(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(rp2pio_statemachine_get_in_waiting_obj, rp2pio_statemachine_obj_get_in_waiting);

MP_PROPERTY_GETTER(rp2pio_statemachine_in_waiting_obj,
    (mp_obj_t)&rp2pio_statemachine_get_in_waiting_obj);

//|     offset: int
//|     """The instruction offset where the program was actually loaded"""
//|

static mp_obj_t rp2pio_statemachine_obj_get_offset(mp_obj_t self_in) {
    rp2pio_statemachine_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_rp2pio_statemachine_get_offset(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(rp2pio_statemachine_get_offset_obj, rp2pio_statemachine_obj_get_offset);

MP_PROPERTY_GETTER(rp2pio_statemachine_offset_obj,
    (mp_obj_t)&rp2pio_statemachine_get_offset_obj);

//|     pc: int
//|     """The current program counter of the state machine"""
//|

static mp_obj_t rp2pio_statemachine_obj_get_pc(mp_obj_t self_in) {
    rp2pio_statemachine_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_rp2pio_statemachine_get_pc(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(rp2pio_statemachine_get_pc_obj, rp2pio_statemachine_obj_get_pc);

MP_PROPERTY_GETTER(rp2pio_statemachine_pc_obj,
    (mp_obj_t)&rp2pio_statemachine_get_pc_obj);

//|     rxfifo: memorymap.AddressRange
//|     """Access the state machine's rxfifo directly
//|
//|     If the state machine's fifo mode is ``txput`` then accessing this object
//|     reads values stored by the ``mov rxfifo[], isr`` PIO instruction, and the
//|     result of modifying it is undefined.
//|
//|     If the state machine's fifo mode is ``txget`` then modifying this object
//|     writes values accessed by the ``mov osr, rxfifo[]`` PIO instruction, and
//|     the result of accessing it is undefined.
//|
//|     If this state machine's mode is something else, then the property's value is `None`.
//|
//|     Note: Since the ``txput`` and ``txget`` fifo mode does not exist on RP2040, this property will always be `None`."""
//|

static mp_obj_t rp2pio_statemachine_obj_get_rxfifo(mp_obj_t self_in) {
    rp2pio_statemachine_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return common_hal_rp2pio_statemachine_get_rxfifo(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(rp2pio_statemachine_get_rxfifo_obj, rp2pio_statemachine_obj_get_rxfifo);

MP_PROPERTY_GETTER(rp2pio_statemachine_rxfifo_obj,
    (mp_obj_t)&rp2pio_statemachine_get_rxfifo_obj);


//|     last_read: array.array
//|     """Returns the buffer most recently filled by background reads.
//|
//|     This property is self-clearing -- once read, subsequent reads
//|     will return a zero-length buffer until the background read buffer
//|     changes or restarts.
//|     """
static mp_obj_t rp2pio_statemachine_obj_get_last_read(mp_obj_t self_in) {
    rp2pio_statemachine_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return common_hal_rp2pio_statemachine_get_last_read(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(rp2pio_statemachine_get_last_read_obj, rp2pio_statemachine_obj_get_last_read);

MP_PROPERTY_GETTER(rp2pio_statemachine_last_read_obj,
    (mp_obj_t)&rp2pio_statemachine_get_last_read_obj);


//|     last_write: array.array
//|     """Returns the buffer most recently emptied by background writes.
//|
//|     This property is self-clearing -- once read, subsequent reads
//|     will return a zero-length buffer until the background write buffer
//|     changes or restarts.
//|     """
//|
//|
static mp_obj_t rp2pio_statemachine_obj_get_last_write(mp_obj_t self_in) {
    rp2pio_statemachine_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return common_hal_rp2pio_statemachine_get_last_write(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(rp2pio_statemachine_get_last_write_obj, rp2pio_statemachine_obj_get_last_write);

MP_PROPERTY_GETTER(rp2pio_statemachine_last_write_obj,
    (mp_obj_t)&rp2pio_statemachine_get_last_write_obj);

static const mp_rom_map_elem_t rp2pio_statemachine_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&rp2pio_statemachine_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&rp2pio_statemachine_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&default___exit___obj) },

    { MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&rp2pio_statemachine_stop_obj) },
    { MP_ROM_QSTR(MP_QSTR_restart), MP_ROM_PTR(&rp2pio_statemachine_restart_obj) },
    { MP_ROM_QSTR(MP_QSTR_run), MP_ROM_PTR(&rp2pio_statemachine_run_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear_rxfifo), MP_ROM_PTR(&rp2pio_statemachine_clear_rxfifo_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear_txstall), MP_ROM_PTR(&rp2pio_statemachine_clear_txstall_obj) },

    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&rp2pio_statemachine_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&rp2pio_statemachine_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_write_readinto), MP_ROM_PTR(&rp2pio_statemachine_write_readinto_obj) },

    { MP_ROM_QSTR(MP_QSTR_background_write), MP_ROM_PTR(&rp2pio_statemachine_background_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop_background_write), MP_ROM_PTR(&rp2pio_statemachine_stop_background_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_writing), MP_ROM_PTR(&rp2pio_statemachine_writing_obj) },
    { MP_ROM_QSTR(MP_QSTR_pending), MP_ROM_PTR(&rp2pio_statemachine_pending_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_pending_write), MP_ROM_PTR(&rp2pio_statemachine_pending_write_obj) },

    { MP_ROM_QSTR(MP_QSTR_background_read), MP_ROM_PTR(&rp2pio_statemachine_background_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop_background_read), MP_ROM_PTR(&rp2pio_statemachine_stop_background_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_reading), MP_ROM_PTR(&rp2pio_statemachine_reading_obj) },
    { MP_ROM_QSTR(MP_QSTR_pending_read), MP_ROM_PTR(&rp2pio_statemachine_pending_read_obj) },

    { MP_ROM_QSTR(MP_QSTR_frequency), MP_ROM_PTR(&rp2pio_statemachine_frequency_obj) },
    { MP_ROM_QSTR(MP_QSTR_rxstall), MP_ROM_PTR(&rp2pio_statemachine_rxstall_obj) },
    { MP_ROM_QSTR(MP_QSTR_txstall), MP_ROM_PTR(&rp2pio_statemachine_txstall_obj) },
    { MP_ROM_QSTR(MP_QSTR_in_waiting), MP_ROM_PTR(&rp2pio_statemachine_in_waiting_obj) },

    { MP_ROM_QSTR(MP_QSTR_offset), MP_ROM_PTR(&rp2pio_statemachine_offset_obj) },
    { MP_ROM_QSTR(MP_QSTR_pc), MP_ROM_PTR(&rp2pio_statemachine_pc_obj) },

    { MP_ROM_QSTR(MP_QSTR_rxfifo), MP_ROM_PTR(&rp2pio_statemachine_rxfifo_obj) },

    { MP_ROM_QSTR(MP_QSTR_last_read), MP_ROM_PTR(&rp2pio_statemachine_last_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_last_write), MP_ROM_PTR(&rp2pio_statemachine_last_write_obj) },

};
static MP_DEFINE_CONST_DICT(rp2pio_statemachine_locals_dict, rp2pio_statemachine_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    rp2pio_statemachine_type,
    MP_QSTR_StateMachine,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, rp2pio_statemachine_make_new,
    locals_dict, &rp2pio_statemachine_locals_dict
    );
