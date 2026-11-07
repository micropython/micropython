// machine_uart_port.c - machine.UART for GD32F4xx
// Included by extmod/machine_uart.c via MICROPY_PY_MACHINE_UART_INCLUDEFILE.

#include <string.h>

#include "py/runtime.h"
#include "py/stream.h"
#include "py/mperrno.h"
#include "py/mphal.h"

#include "gd32f4xx.h"

#if MICROPY_PY_MACHINE_UART_IRQ
#include "shared/runtime/mpirq.h"
#endif

#define UART_RX_BUFFER_SIZE  (256)

// ---- simple SPSC ring buffer ----
typedef struct _uart_ringbuf_t {
    uint8_t *buf;
    size_t size;
    volatile size_t iget;
    volatile size_t iput;
} uart_ringbuf_t;

static inline void uart_ringbuf_init(uart_ringbuf_t *r, uint8_t *buf, size_t size) {
    r->buf = buf;
    r->size = size;
    r->iget = 0;
    r->iput = 0;
}

static inline size_t uart_ringbuf_avail(uart_ringbuf_t *r) {
    return (r->iput + r->size - r->iget) % r->size;
}

static inline void uart_ringbuf_put(uart_ringbuf_t *r, uint8_t v) {
    size_t next = (r->iput + 1) % r->size;
    if (next == r->iget) {
        return;   // full, drop
    }
    r->buf[r->iput] = v;
    r->iput = next;
}

static inline int uart_ringbuf_get(uart_ringbuf_t *r) {
    if (r->iget == r->iput) {
        return -1;
    }
    int v = r->buf[r->iget];
    r->iget = (r->iget + 1) % r->size;
    return v;
}

// ---- object ----
typedef struct _machine_uart_obj_t {
    mp_obj_base_t base;
    uint32_t usart;
    uint32_t rcu;
    uint8_t  idx;
    uint32_t baudrate;
    uint32_t bits;
    uint32_t parity;
    uint32_t stop;
    bool     tx_enabled;
    bool     rx_enabled;
    uint16_t timeout;          // ms; 0 = block forever
    uart_ringbuf_t read_buffer;
    uint8_t  read_buffer_data[UART_RX_BUFFER_SIZE];
    #if MICROPY_PY_MACHINE_UART_IRQ
    mp_irq_obj_t *irq_obj;               // NULL until UART.irq() is called
    #endif
} machine_uart_obj_t;

// ---- instances ----
typedef struct {
    uint32_t usart;
    uint32_t rcu;
} uart_hw_t;

static const uart_hw_t uart_hw[] = {
    { USART0, RCU_USART0 },
    { USART1, RCU_USART1 },
    { USART2, RCU_USART2 },
    { UART3,  RCU_UART3  },
    { UART4,  RCU_UART4  },
    { USART5, RCU_USART5 },
};

#define NUM_UART MP_ARRAY_SIZE(uart_hw)

static machine_uart_obj_t machine_uart_obj[NUM_UART];

// forward decl
static void machine_uart_irq_handler(uint8_t idx);

// ---- per-instance IRQ handlers ----
void USART0_IRQHandler(void) { machine_uart_irq_handler(0); }
void USART1_IRQHandler(void) { machine_uart_irq_handler(1); }
void USART2_IRQHandler(void) { machine_uart_irq_handler(2); }
void UART3_IRQHandler(void)  { machine_uart_irq_handler(3); }
void UART4_IRQHandler(void)  { machine_uart_irq_handler(4); }
void USART5_IRQHandler(void) { machine_uart_irq_handler(5); }

static const IRQn_Type uart_irqn[] = {
    USART0_IRQn, USART1_IRQn, USART2_IRQn, UART3_IRQn, UART4_IRQn, USART5_IRQn,
};

static void machine_uart_irq_handler(uint8_t idx) {
    machine_uart_obj_t *self = &machine_uart_obj[idx];
    while (usart_flag_get(self->usart, USART_FLAG_RBNE) != RESET) {
        uint8_t data = (uint8_t)usart_data_receive(self->usart);
        uart_ringbuf_put(&self->read_buffer, data);
    }
    #if MICROPY_PY_MACHINE_UART_IRQ
    // Wake any Python-level handler installed with UART.irq().
    if (self->irq_obj != NULL) {
        mp_irq_handler(self->irq_obj);
    }
    #endif
}

// ---- extmod hooks: sendbreak / readchar / writechar / irq ----

#if MICROPY_PY_MACHINE_UART_SENDBREAK
// Hold TX low for longer than one character frame; the receiver sees a break.
static void mp_machine_uart_sendbreak(machine_uart_obj_t *self) {
    // Estimate one frame: start + data + parity + stop bits at the current baud.
    uint32_t bits_per_frame = 1 + self->bits
        + ((self->parity == USART_PM_NONE) ? 0 : 1)
        + ((self->stop == USART_STB_2BIT) ? 2 : 1);
    uint32_t frame_us = (uint32_t)((uint64_t)bits_per_frame * 1000000u / self->baudrate);
    // Twelve bit-times is comfortably longer than any single frame.
    uint32_t break_us = frame_us * 12u;

    usart_data_transmit(self->usart, 0x00);
    while (usart_flag_get(self->usart, USART_FLAG_TBE) == RESET) {
        ;
    }
    mp_hal_delay_us(break_us);
}
#endif

#if MICROPY_PY_MACHINE_UART_READCHAR_WRITECHAR
// Read one character, blocking honouring the instance timeout.  Returns -1 on
// timeout, matching the extmod convention.
static mp_int_t mp_machine_uart_readchar(machine_uart_obj_t *self) {
    if (self->timeout == 0) {
        int c;
        while ((c = uart_ringbuf_get(&self->read_buffer)) < 0) {
            ;
        }
        return c;
    }
    uint32_t start = mp_hal_ticks_ms();
    for (;;) {
        int c = uart_ringbuf_get(&self->read_buffer);
        if (c >= 0) {
            return c;
        }
        if ((uint32_t)(mp_hal_ticks_ms() - start) >= self->timeout) {
            return -1;
        }
    }
}

static void mp_machine_uart_writechar(machine_uart_obj_t *self, uint16_t data) {
    while (usart_flag_get(self->usart, USART_FLAG_TBE) == RESET) {
        ;
    }
    usart_data_transmit(self->usart, (uint8_t)data);
}
#endif

#if MICROPY_PY_MACHINE_UART_IRQ

// Triggers the user may request.  RX-any is handled by the RXNE interrupt;
// the others have no hardware support on this port, so they are masked out and
// UART.irq() rejects them with a ValueError rather than silently ignoring.
#define UART_IRQ_ALLOWED_TRIGGERS (1u << 0)   // RX data available

static mp_uint_t uart_irq_trigger(mp_obj_t self_in, mp_uint_t trigger) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_uint_t enabled = 0;
    if (trigger & UART_IRQ_ALLOWED_TRIGGERS) {
        usart_interrupt_enable(self->usart, USART_INT_RBNE);
        enabled |= UART_IRQ_ALLOWED_TRIGGERS;
    } else {
        usart_interrupt_disable(self->usart, USART_INT_RBNE);
    }
    return enabled;
}

static mp_uint_t uart_irq_info(mp_obj_t self_in, mp_uint_t info_type) {
    (void)self_in;
    if (info_type == MP_IRQ_INFO_FLAGS) {
        return UART_IRQ_ALLOWED_TRIGGERS;
    } else if (info_type == MP_IRQ_INFO_TRIGGERS) {
        return UART_IRQ_ALLOWED_TRIGGERS;
    }
    return 0;
}

static const mp_irq_methods_t uart_irq_methods = {
    .trigger = uart_irq_trigger,
    .info = uart_irq_info,
};

static mp_irq_obj_t *mp_machine_uart_irq(machine_uart_obj_t *self, bool any_args,
                                          mp_arg_val_t *args) {
    if (self->irq_obj == NULL) {
        self->irq_obj = mp_irq_new(&uart_irq_methods, MP_OBJ_FROM_PTR(self));
    }
    if (any_args) {
        mp_obj_t handler = args[MP_IRQ_ARG_INIT_handler].u_obj;
        if (handler != mp_const_none && !mp_obj_is_callable(handler)) {
            mp_raise_ValueError(MP_ERROR_TEXT("handler must be None or callable"));
        }
        mp_uint_t trigger = (mp_uint_t)args[MP_IRQ_ARG_INIT_trigger].u_int;
        if (trigger & ~UART_IRQ_ALLOWED_TRIGGERS) {
            mp_raise_ValueError(MP_ERROR_TEXT("unsupported trigger"));
        }
        mp_irq_init(self->irq_obj, &uart_irq_methods, MP_OBJ_FROM_PTR(self));
        self->irq_obj->handler = handler;
        self->irq_obj->ishard = false;
        uart_irq_trigger(MP_OBJ_FROM_PTR(self), trigger);
    }
    return self->irq_obj;
}
#endif

// ---- type methods ----
static void mp_machine_uart_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "UART(%u, baudrate=%u, bits=%u, parity=%u, stop=%u)",
        self->idx, (unsigned)self->baudrate, (unsigned)self->bits,
        (unsigned)self->parity, (unsigned)self->stop);
}

static void mp_machine_uart_init_helper(machine_uart_obj_t *self, size_t n_args,
                                        const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum {
        ARG_baudrate, ARG_bits, ARG_parity, ARG_stop, ARG_tx, ARG_rx,
        ARG_rxbuf, ARG_timeout, ARG_flow,
    };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_baudrate, MP_ARG_INT, {.u_int = 9600} },
        { MP_QSTR_bits,     MP_ARG_INT, {.u_int = 8} },
        { MP_QSTR_parity,   MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_stop,     MP_ARG_INT, {.u_int = 1} },
        { MP_QSTR_tx,       MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_rx,       MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_rxbuf,    MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_timeout,  MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_flow,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    self->baudrate = args[ARG_baudrate].u_int;
    self->bits = args[ARG_bits].u_int;
    self->stop = args[ARG_stop].u_int;
    self->parity = (args[ARG_parity].u_obj == mp_const_none)
        ? 0 : mp_obj_get_int(args[ARG_parity].u_obj);
    self->timeout = (args[ARG_timeout].u_int < 0) ? 0 : (uint16_t)args[ARG_timeout].u_int;
    self->tx_enabled = (args[ARG_tx].u_obj != mp_const_none);
    self->rx_enabled = (args[ARG_rx].u_obj != mp_const_none);

    uart_ringbuf_init(&self->read_buffer, self->read_buffer_data, UART_RX_BUFFER_SIZE);

    rcu_periph_clock_enable(self->rcu);
    usart_deinit(self->usart);
    usart_baudrate_set(self->usart, self->baudrate);
    usart_word_length_set(self->usart, (self->bits == 9) ? USART_WL_9BIT : USART_WL_8BIT);
    usart_stop_bit_set(self->usart, (self->stop == 2) ? USART_STB_2BIT : USART_STB_1BIT);
    usart_parity_config(self->usart, USART_PM_NONE);
    usart_hardware_flow_rts_config(self->usart, USART_RTS_DISABLE);
    usart_hardware_flow_cts_config(self->usart, USART_CTS_DISABLE);
    usart_receive_config(self->usart, USART_RECEIVE_ENABLE);
    usart_transmit_config(self->usart, USART_TRANSMIT_ENABLE);
    usart_enable(self->usart);

    // enable RX interrupt
    usart_interrupt_enable(self->usart, USART_INT_RBNE);
    NVIC_SetPriority(uart_irqn[self->idx], 2);
    NVIC_EnableIRQ(uart_irqn[self->idx]);
}

static mp_obj_t mp_machine_uart_make_new(const mp_obj_type_t *type, size_t n_args,
                                          size_t n_kw, const mp_obj_t *all_args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);
    mp_int_t uart_id = mp_obj_get_int(all_args[0]);
    if (uart_id < 0 || (mp_uint_t)uart_id >= NUM_UART) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid UART"));
    }

    machine_uart_obj_t *self = &machine_uart_obj[uart_id];
    self->base.type = &machine_uart_type;
    self->usart = uart_hw[uart_id].usart;
    self->rcu = uart_hw[uart_id].rcu;
    self->idx = (uint8_t)uart_id;

    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, all_args + n_args);
    mp_machine_uart_init_helper(self, n_args - 1, all_args + 1, &kw_args);

    return MP_OBJ_FROM_PTR(self);
}

static void mp_machine_uart_deinit(machine_uart_obj_t *self) {
    NVIC_DisableIRQ(uart_irqn[self->idx]);
    usart_interrupt_disable(self->usart, USART_INT_RBNE);
    usart_disable(self->usart);
    usart_deinit(self->usart);
    rcu_periph_clock_disable(self->rcu);
}

static mp_int_t mp_machine_uart_any(machine_uart_obj_t *self) {
    return (mp_int_t)uart_ringbuf_avail(&self->read_buffer);
}

static bool mp_machine_uart_txdone(machine_uart_obj_t *self) {
    return usart_flag_get(self->usart, USART_FLAG_TBE) != RESET;
}

static mp_uint_t mp_machine_uart_read(mp_obj_t self_in, void *buf_in, mp_uint_t size, int *errcode) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (!self->rx_enabled) {
        *errcode = MP_EPERM;
        return MP_STREAM_ERROR;
    }

    uint8_t *dest = buf_in;
    mp_uint_t n_read = 0;

    while (n_read < size) {
        int c = uart_ringbuf_get(&self->read_buffer);
        if (c >= 0) {
            dest[n_read++] = (uint8_t)c;
            continue;
        }
        // buffer empty: wait for a byte
        mp_uint_t start = mp_hal_ticks_ms();
        for (;;) {
            c = uart_ringbuf_get(&self->read_buffer);
            if (c >= 0) {
                break;
            }
            if (self->timeout && (mp_hal_ticks_ms() - start) > self->timeout) {
                if (n_read == 0) {
                    *errcode = MP_EAGAIN;
                    return MP_STREAM_ERROR;
                }
                return n_read;
            }
        }
        dest[n_read++] = (uint8_t)c;
    }
    return n_read;
}

static mp_uint_t mp_machine_uart_write(mp_obj_t self_in, const void *buf_in, mp_uint_t size, int *errcode) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (!self->tx_enabled) {
        *errcode = MP_EPERM;
        return MP_STREAM_ERROR;
    }
    const uint8_t *src = buf_in;
    for (mp_uint_t i = 0; i < size; i++) {
        while (usart_flag_get(self->usart, USART_FLAG_TBE) == RESET) {
            ;
        }
        usart_data_transmit(self->usart, src[i]);
    }
    return size;
}

static mp_uint_t mp_machine_uart_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    switch (request) {
        case MP_STREAM_FLUSH:
            while (usart_flag_get(self->usart, USART_FLAG_TC) == RESET) {
                ;
            }
            return 0;
        case MP_STREAM_GET_DATA_OPTS:
            return 0;
        default:
            *errcode = MP_EINVAL;
            return MP_STREAM_ERROR;
    }
    (void)self;
}
