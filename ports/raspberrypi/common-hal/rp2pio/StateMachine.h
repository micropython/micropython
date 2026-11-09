// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#include "common-hal/microcontroller/Pin.h"
#include "common-hal/memorymap/AddressRange.h"
#include "hardware/pio.h"

// Shared PIO allocator declarations (rp2pio_statemachine_find_pio). Kept in a
// separate, mp-free header so external C++ drivers can include them too.
#include "pio_alloc.h"

// pio_pinmask_t can hold ANY pin masks, so it is used before selection of gpiobase
#if NUM_BANK0_GPIOS > 32
typedef struct { uint64_t value;
} pio_pinmask_t;
typedef uint64_t pio_pinmask_value_t;
#define PIO_PINMASK_C(c) UINT64_C(c)
#define PIO_PINMASK_BIT (64)
#define PIO_PINMASK(i) (UINT64_C(1) << (i))
#define PIO_PINMASK_PRINT(p) mp_printf(&mp_plat_print, \
    "%s:%d: %s = %08x %08x\n", \
    __FILE__, __LINE__, #p, \
    (uint32_t)(PIO_PINMASK_VALUE(p) >> 32), \
    (uint32_t)PIO_PINMASK_VALUE(p));
#define PIO_PINMASK_ALL PIO_PINMASK_FROM_VALUE(~UINT64_C(0))
#else
typedef struct { uint32_t value;
} pio_pinmask_t;
typedef uint32_t pio_pinmask_value_t;
#define PIO_PINMASK_C(c) UINT32_C(c)
#define PIO_PINMASK_BIT (32)
#define PIO_PINMASK(i) (UINT32_C(1) << (i))
#define PIO_PINMASK_PRINT(p) mp_printf(&mp_plat_print, "%s:%d: %s = %08x\n", \
    __FILE__, __LINE__, #p, \
    (uint32_t)(PIO_PINMASK_VALUE(p)));
#define PIO_PINMASK_ALL PIO_PINMASK_FROM_VALUE(~UINT32_C(0))
#endif
#define PIO_PINMASK_VALUE(p) ((p).value)
#define PIO_PINMASK_FROM_VALUE(v) ((pio_pinmask_t) {(v)})
#define PIO_PINMASK_FROM_PIN(i) ((pio_pinmask_t) {(PIO_PINMASK(i))})
#define PIO_PINMASK_NONE PIO_PINMASK_FROM_VALUE(0)
#define PIO_PINMASK_SET(p, i) ((p).value |= PIO_PINMASK(i))
#define PIO_PINMASK_CLEAR(p, i) ((p).value &= ~PIO_PINMASK(i))
#define PIO_PINMASK_IS_SET(p, i) (((p).value & PIO_PINMASK(i)) != 0)
#define PIO_PINMASK_BINOP(op, p, q) PIO_PINMASK_FROM_VALUE((p).value op(q).value)
#define PIO_PINMASK_BINOP_ASSIGN(op, p, q) ((p).value op(q).value)
#define PIO_PINMASK_EQUAL(p, q) ((p).value == (q).value)
#define PIO_PINMASK_AND(p, q) PIO_PINMASK_BINOP(&, (p), (q))
#define PIO_PINMASK_AND_NOT(p, q) PIO_PINMASK_BINOP(&~, (p), (q))
#define PIO_PINMASK_OR(p, q) PIO_PINMASK_BINOP(|, (p), (q))
#define PIO_PINMASK_OR3(p, q, r) PIO_PINMASK_OR((p), PIO_PINMASK_OR((q), (r)))
#define PIO_PINMASK_INTERSECT(p, q) PIO_PINMASK_BINOP_ASSIGN( &=, (p), (q))
#define PIO_PINMASK_DIFFERENCE(p, q) PIO_PINMASK_BINOP_ASSIGN( &= ~, (p), (q))
#define PIO_PINMASK_MERGE(p, q) PIO_PINMASK_BINOP_ASSIGN( |=, (p), (q))

// pio peripheral registers only work 32 bits at a time and depend on the selection of base
// (0 only on RP2040 & RP2350A; 0 or 16 on RP2350B)
typedef struct { uint32_t value32;
} pio_pinmask32_t;
#define PIO_PINMASK32(i) (1u << (i))
#define PIO_PINMASK32_C(c) UINT32_C(c)
#define PIO_PINMASK32_NONE PIO_PINMASK32_FROM_VALUE(0)
#define PIO_PINMASK32_ALL PIO_PINMASK32_FROM_VALUE(~UINT32_C(0))
#define PIO_PINMASK32_BASE(i, base) PIO_PINMASK32((i) - (base))
#define PIO_PINMASK32_VALUE(p) ((p).value32)
#define PIO_PINMASK32_FROM_VALUE(v) ((pio_pinmask32_t) {(v)})
#define PIO_PINMASK32_SET(p, i) ((p).value32 |= PIO_PINMASK32_VALUE(i))
#define PIO_PINMASK32_CLEAR(p, i) ((p).value32 &= ~PIO_PINMASK32_VALUE(i))
#define PIO_PINMASK32_IS_SET(p, i) (((p).value32 & PIO_PINMASK32_VALUE(i)) != 0)
#define PIO_PINMASK32_BINOP(op, p, q) PIO_PINMASK32_FROM_VALUE((p).value32 op(q).value32)
#define PIO_PINMASK32_AND(p, q) PIO_PINMASK32_BINOP(&, (p), (q))
#define PIO_PINMASK32_AND_NOT(p, q) PIO_PINMASK32_BINOP(&~, (p), (q))
#define PIO_PINMASK32_OR(p, q) PIO_PINMASK32_BINOP(|, (p), (q))
#define PIO_PINMASK32_OR3(p, q, r) PIO_PINMASK32_OR((p), PIO_PINMASK32_OR((q), (r)))
#define PIO_PINMASK32_INTERSECT(p, q) PIO_PINMASK32_BINOP( &=, (p), (q))
#define PIO_PINMASK32_DIFFERENCE(p, q) PIO_PINMASK32_BINOP( &= ~, (p), (q))
#define PIO_PINMASK32_MERGE(p, q) PIO_PINMASK32_BINOP( |=, (p), (q))
#define PIO_PINMASK32_FROM_PINMASK_WITH_OFFSET(p, gpio_offset) PIO_PINMASK32_FROM_VALUE(PIO_PINMASK_VALUE((p)) >> (gpio_offset))
#define PIO_PINMASK_FROM_PINMASK32_WITH_OFFSET(p, gpio_offset) PIO_PINMASK_FROM_VALUE(PIO_PINMASK32_VALUE((p)) << (gpio_offset))

enum { PIO_ANY_OFFSET = -1 };
enum { PIO_FIFO_JOIN_AUTO = -1, PIO_FIFO_TYPE_DEFAULT = PIO_FIFO_JOIN_AUTO };
enum { PIO_MOV_STATUS_DEFAULT = STATUS_TX_LESSTHAN };
enum { PIO_MOV_N_DEFAULT = 0 };

typedef struct sm_buf_info {
    mp_obj_t obj;
    mp_buffer_info_t info;
} sm_buf_info;

#define RP2PIO_STATEMACHINE_N_BUFS 3

typedef struct {
    mp_obj_base_t base;
    pio_pinmask32_t pins; // Bitmask of what pins this state machine uses.
    int state_machine;
    PIO pio;
    const uint16_t *init;
    size_t init_len;
    pio_pinmask_t pins_we_own; // Subset of pins that no other state machine had already claimed.
    pio_pinmask_t initial_pin_state;
    pio_pinmask_t initial_pin_direction;
    pio_pinmask_t pull_pin_up;
    pio_pinmask_t pull_pin_down;
    uint tx_dreq;
    uint rx_dreq;
    uint32_t actual_frequency;
    pio_sm_config sm_config;
    bool in;
    bool out;
    bool wait_for_txstall;
    bool out_shift_right;
    bool in_shift_right;
    bool user_interruptible;
    #if NUM_BANK0_GPIOS > 32
    uint8_t pio_gpio_offset;
    #endif
    uint8_t offset;
    uint8_t fifo_depth;  // Either 4 if FIFOs are not joined, or 8 if they are.

    // dma-related items
    volatile int pending_buffers_write;
    volatile int pending_buffers_read;
    int write_buf_index, read_buf_index;
    sm_buf_info write_buf[RP2PIO_STATEMACHINE_N_BUFS];
    sm_buf_info read_buf[RP2PIO_STATEMACHINE_N_BUFS];

    sm_buf_info once_read_buf_info, loop_read_buf_info, loop2_read_buf_info;
    sm_buf_info current_read_buf, next_read_buf_1, next_read_buf_2, next_read_buf_3;
    sm_buf_info once_write_buf_info, loop_write_buf_info, loop2_write_buf_info;
    sm_buf_info current_write_buf, next_write_buf_1, next_write_buf_2, next_write_buf_3;

    bool switched_write_buffers, switched_read_buffers;

    int background_stride_in_bytes;
    bool dma_completed_write, byteswap;
    bool dma_completed_read;
    #if PICO_PIO_VERSION > 0
    memorymap_addressrange_obj_t rxfifo_obj;
    #endif
} rp2pio_statemachine_obj_t;

void reset_rp2pio_statemachine(void);

// Minimal internal version that only fails on pin error (not in use) or full PIO.
bool rp2pio_statemachine_construct(rp2pio_statemachine_obj_t *self,
    const uint16_t *program, size_t program_len,
    size_t frequency,
    const uint16_t *init, size_t init_len,
    const mcu_pin_obj_t *first_out_pin, uint8_t out_pin_count,
    const mcu_pin_obj_t *first_in_pin, uint8_t in_pin_count,
    pio_pinmask_t pull_pin_up, pio_pinmask_t pull_pin_down,
    const mcu_pin_obj_t *first_set_pin, uint8_t set_pin_count,
    const mcu_pin_obj_t *first_sideset_pin, uint8_t sideset_pin_count, bool sideset_pindirs,
    pio_pinmask_t initial_pin_state, pio_pinmask_t initial_pin_direction,
    const mcu_pin_obj_t *jmp_pin,
    pio_pinmask_t pins_we_use, bool tx_fifo, bool rx_fifo,
    bool auto_pull, uint8_t pull_threshold, bool out_shift_right,
    bool wait_for_txstall,
    bool auto_push, uint8_t push_threshold, bool in_shift_right,
    bool claim_pins,
    bool interruptible,
    bool sideset_enable,
    int wrap_target, int wrap, int offset,
    int fifo_type,
    int mov_status_type, int mov_status_n
    );

uint8_t rp2pio_statemachine_program_offset(rp2pio_statemachine_obj_t *self);

void rp2pio_statemachine_deinit(rp2pio_statemachine_obj_t *self, bool leave_pins);
void rp2pio_statemachine_dma_complete_write(rp2pio_statemachine_obj_t *self, int channel);
void rp2pio_statemachine_dma_complete_read(rp2pio_statemachine_obj_t *self, int channel);

extern const mp_obj_type_t rp2pio_statemachine_type;
