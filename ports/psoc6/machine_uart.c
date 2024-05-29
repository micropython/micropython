// mpy includes
#include "py/mphal.h"
#include "py/mperrno.h"
#include "py/ringbuf.h"
#include "py/runtime.h"
#include "shared/runtime/interrupt_char.h"
#include "shared/runtime/mpirq.h"

// MTB includes
#include "cybsp.h"
#include "cyhal.h"

// port specific includes
#include "modmachine.h"
#include "machine_pin_phy.h"
#include "mplogger.h"

#define UART_HWCONTROL_RTS  (1)
#define UART_HWCONTROL_CTS  (2)
#define MAX_UART             10 // TODO: Change as per bsp?
#define DEFAULT_UART_PIN    (MP_ROM_QSTR(MP_QSTR_NC))

// OR-ed IRQ flags which are allowed to be used by the user
#define MP_UART_ALLOWED_FLAGS (CYHAL_UART_IRQ_TX_EMPTY | CYHAL_UART_IRQ_TX_DONE | CYHAL_UART_IRQ_RX_FULL | CYHAL_UART_IRQ_RX_DONE)


#define uart_assert_raise_val(msg, ret)   if (ret != CY_RSLT_SUCCESS) { \
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT(msg), ret); \
}

typedef struct _machine_uart_obj_t {
    mp_obj_base_t base;
    cyhal_uart_t uart_obj;
    uint32_t init_flag;    // Flag to support reinitialisation of uart parameters
    uint32_t bits;
    uint32_t stop;
    uint32_t baudrate;
    cyhal_uart_parity_t parity;
    machine_pin_phy_obj_t *tx;
    machine_pin_phy_obj_t *rx;
    machine_pin_phy_obj_t *cts;
    machine_pin_phy_obj_t *rts;
    cyhal_uart_cfg_t cfg;
    uint16_t timeout; // only used by cyhal_uart_getc() ie, readchar()
    uint8_t flow;
    uint32_t rxbuf;
    bool interrupt;
    uint16_t mp_irq_trigger;            // user IRQ trigger mask
    uint16_t mp_irq_flags;              // user IRQ active IRQ flags
    mp_irq_obj_t *mp_irq_obj;           // user IRQ object
} machine_uart_obj_t;

#define MICROPY_PY_MACHINE_UART_CLASS_CONSTANTS \
    { MP_ROM_QSTR(MP_QSTR_RTS), MP_ROM_INT(UART_HWCONTROL_RTS) }, \
    { MP_ROM_QSTR(MP_QSTR_CTS), MP_ROM_INT(UART_HWCONTROL_CTS) }, \
    { MP_ROM_QSTR(MP_QSTR_TX_EMPTY), MP_ROM_INT(CYHAL_UART_IRQ_TX_EMPTY) }, \
    { MP_ROM_QSTR(MP_QSTR_TX_DONE), MP_ROM_INT(CYHAL_UART_IRQ_TX_DONE) }, \
    { MP_ROM_QSTR(MP_QSTR_RX_FULL), MP_ROM_INT(CYHAL_UART_IRQ_RX_FULL) }, \
    { MP_ROM_QSTR(MP_QSTR_RX_DONE), MP_ROM_INT(CYHAL_UART_IRQ_RX_DONE) }, \

machine_uart_obj_t *uart_obj[MAX_UART] = { NULL };

static const char *_parity_name[] = {"None", "0", "1"};


void uart_event_handler(void *handler_arg, cyhal_uart_event_t event) {
    machine_uart_obj_t *self = handler_arg;
    mp_irq_handler(self->mp_irq_obj);
}

void uart_irq_config(machine_uart_obj_t *self, bool enable) {
    if (self->mp_irq_trigger) {
        cyhal_uart_register_callback(&self->uart_obj, uart_event_handler, self);
        cyhal_uart_enable_event(&self->uart_obj, self->mp_irq_trigger, 7, enable);
    }
}

static mp_uint_t uart_irq_trigger(mp_obj_t self_in, mp_uint_t new_trigger) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uart_irq_config(self, false);
    self->mp_irq_trigger = new_trigger;
    uart_irq_config(self, true);
    return 0;
}

static mp_uint_t uart_irq_info(mp_obj_t self_in, mp_uint_t info_type) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (info_type == MP_IRQ_INFO_FLAGS) {
        return self->mp_irq_flags;
    } else if (info_type == MP_IRQ_INFO_TRIGGERS) {
        return self->mp_irq_trigger;
    }
    return 0;
}

const mp_irq_methods_t uart_irq_methods = {
    .trigger = uart_irq_trigger,
    .info = uart_irq_info,
};

static void uart_init(machine_uart_obj_t *machine_uart_obj) {
    uint32_t actualbaud;

    // // Initialize the UART configuration structure
    machine_uart_obj->cfg.data_bits = machine_uart_obj->bits;
    machine_uart_obj->cfg.stop_bits = machine_uart_obj->stop;
    machine_uart_obj->cfg.parity = machine_uart_obj->parity;
    machine_uart_obj->cfg.rx_buffer = NULL;
    machine_uart_obj->cfg.rx_buffer_size = 0;

    if (machine_uart_obj->init_flag == 0) {
        // Initialize the UART Block
        cy_rslt_t result = cyhal_uart_init(&machine_uart_obj->uart_obj, machine_uart_obj->tx->addr, machine_uart_obj->rx->addr, machine_uart_obj->cts->addr,
            machine_uart_obj->rts->addr, NULL, &(machine_uart_obj->cfg));
        uart_assert_raise_val("UART initialisation failed with return code %lx !", result);
    }

    // Set the baud rate
    cy_rslt_t result = cyhal_uart_set_baud(&machine_uart_obj->uart_obj, machine_uart_obj->baudrate, &actualbaud);
    uart_assert_raise_val("UART baudrate failed with return code %lx !", result);
    machine_uart_obj->baudrate = actualbaud;

    // reconfigure the uart config structure . If it's a reinitialise
    if (machine_uart_obj->init_flag != 0) {
        result = cyhal_uart_configure(&machine_uart_obj->uart_obj, &(machine_uart_obj->cfg));
        uart_assert_raise_val("UART configuration failed with return code %lx !", result);
    }

    machine_uart_obj->init_flag = machine_uart_obj->init_flag + 1;
}

static inline machine_uart_obj_t *uart_obj_alloc() {
    for (uint8_t i = 0; i < MAX_UART; i++)
    {
        if (uart_obj[i] == NULL) {
            uart_obj[i] = mp_obj_malloc(machine_uart_obj_t, &machine_uart_type);
            return uart_obj[i];
        }
    }
    return NULL;
}

static inline void uart_obj_free(machine_uart_obj_t *uart_obj_ptr) {
    for (uint8_t i = 0; i < MAX_UART; i++)
    {
        if (uart_obj[i] == uart_obj_ptr) {
            uart_obj[i] = NULL;
        }
    }
}

static inline void uart_tx_alloc(machine_uart_obj_t *uart_obj, mp_obj_t pin_name) {
    machine_pin_phy_obj_t *tx = pin_phy_realloc(pin_name, PIN_PHY_FUNC_UART);
    if (tx == NULL) {
        size_t slen;
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("UART pin (%s) not found !"), mp_obj_str_get_data(pin_name, &slen));
    }
    uart_obj->tx = tx;
}

static inline void uart_tx_free(machine_uart_obj_t *uart_obj) {
    pin_phy_free(uart_obj->tx);
}

static inline void uart_rx_alloc(machine_uart_obj_t *uart_obj, mp_obj_t pin_name) {
    machine_pin_phy_obj_t *rx = pin_phy_realloc(pin_name, PIN_PHY_FUNC_UART);

    if (rx == NULL) {
        size_t slen;
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("UART pin (%s) not found !"), mp_obj_str_get_data(pin_name, &slen));
    }

    uart_obj->rx = rx;
}

static inline void uart_rx_free(machine_uart_obj_t *uart_obj) {
    pin_phy_free(uart_obj->rx);
}

static inline void uart_cts_alloc(machine_uart_obj_t *uart_obj, mp_obj_t pin_name) {
    machine_pin_phy_obj_t *cts = pin_phy_realloc(pin_name, PIN_PHY_FUNC_UART);

    if (cts == NULL) {
        size_t slen;
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("UART pin (%s) not found !"), mp_obj_str_get_data(pin_name, &slen));
    }

    uart_obj->cts = cts;
}

static inline void uart_cts_free(machine_uart_obj_t *uart_obj) {
    pin_phy_free(uart_obj->cts);
}

static inline void uart_rts_alloc(machine_uart_obj_t *uart_obj, mp_obj_t pin_name) {
    machine_pin_phy_obj_t *rts = pin_phy_realloc(pin_name, PIN_PHY_FUNC_UART);
    if (rts == NULL) {
        size_t slen;
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("UART pin (%s) not found !"), mp_obj_str_get_data(pin_name, &slen));
    }

    uart_obj->rts = rts;
}

static inline void uart_rts_free(machine_uart_obj_t *uart_obj) {
    pin_phy_free(uart_obj->rts);
}

static void mp_machine_uart_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "UART(baudrate=%u, bits=%u, parity=%s, stop=%u, tx=%d, rx=%d, rts=%d, cts=%d, rxbuf=%u, timeout=%u",
        self->baudrate, self->bits, _parity_name[self->parity],
        self->stop, self->tx->addr, self->rx->addr, self->rts->addr, self->cts->addr, self->rxbuf, self->timeout);
    if (self->flow) {
        mp_printf(print, ", flow=");
        uint32_t flow_mask = self->flow;
        if (flow_mask & UART_HWCONTROL_RTS) {
            mp_printf(print, "RTS");
            flow_mask &= ~UART_HWCONTROL_RTS;
            if (flow_mask) {
                mp_printf(print, "|");
            }
        }
        if (flow_mask & UART_HWCONTROL_CTS) {
            mp_printf(print, "CTS");
        }
    }
    mp_printf(print, ")");
}

static void mp_machine_uart_init_helper(machine_uart_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_baudrate, ARG_bits, ARG_parity, ARG_stop, ARG_tx, ARG_rx, ARG_timeout, ARG_rxbuf, ARG_flow, ARG_rts, ARG_cts };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_baudrate, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_bits, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_parity, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_INT(-1)} },
        { MP_QSTR_stop, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_tx, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_rx, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_rxbuf, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_flow, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_rts, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = DEFAULT_UART_PIN} },
        { MP_QSTR_cts, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = DEFAULT_UART_PIN} },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    bool enable_rts = 0;
    bool enable_cts = 0;


    // Set baudrate if configured.
    if (args[ARG_baudrate].u_int > 0) {
        self->baudrate = args[ARG_baudrate].u_int;
    } else {
        if (self->init_flag == 0) {
            mp_raise_TypeError(MP_ERROR_TEXT("baudrate must be provided"));
        }
    }

    // Set bits if configured.
    if (args[ARG_bits].u_int > 0) {
        self->bits = args[ARG_bits].u_int;
    } else {
        if (self->init_flag == 0) {
            mp_raise_TypeError(MP_ERROR_TEXT("bits must be provided"));
        }
    }

    // Set parity if configured.
    if (args[ARG_parity].u_obj != MP_OBJ_NEW_SMALL_INT(-1)) {
        if (args[ARG_parity].u_obj == mp_const_none) {
            self->parity = 0;
        } else if (mp_obj_get_int(args[ARG_parity].u_obj) & 1) {
            self->parity = 1;  // odd
        } else {
            self->parity = 2;  // even
        }
    }

    // Set stop bits if configured.
    if (args[ARG_stop].u_int > 0) {
        self->stop = (args[ARG_stop].u_int);
    } else {
        if (self->init_flag == 0) {
            mp_raise_TypeError(MP_ERROR_TEXT("stop bits must be provided"));
        }
    }

    // Set TX/RX pins if configured.
    if (args[ARG_tx].u_obj != mp_const_none) {
        uart_tx_alloc(self, args[ARG_tx].u_obj);
    } else {
        if (self->init_flag == 0) {
            mp_raise_TypeError(MP_ERROR_TEXT("tx pin must be provided"));
        }
    }

    if (args[ARG_rx].u_obj != mp_const_none) {
        uart_rx_alloc(self, args[ARG_rx].u_obj);
    } else {
        if (self->init_flag == 0) {
            mp_raise_TypeError(MP_ERROR_TEXT("rx pin must be provided"));
        }
    }

    // Set CTS/RTS pins if configured.
    if (args[ARG_rts].u_obj != mp_const_none) {
        uart_rts_alloc(self, args[ARG_rts].u_obj);
    }

    if (args[ARG_cts].u_obj != mp_const_none) {
        uart_cts_alloc(self, args[ARG_cts].u_obj);
    }

    // Set hardware flow control if configured.
    if (args[ARG_flow].u_int >= 1) {
        if (args[ARG_flow].u_int & ~(UART_HWCONTROL_CTS | UART_HWCONTROL_RTS)) {
            mp_raise_ValueError(MP_ERROR_TEXT("bad hardware flow control mask"));
        }
        self->flow = args[ARG_flow].u_int;
    }

    self->interrupt = 0;
    self->mp_irq_trigger = 0;
    self->mp_irq_obj = NULL;

    // initialise UART at cyhal level
    if (n_args > 0 || kw_args->used > 0) {
        uart_init(self);

        // Configure UART RX software buffer, which will extend the hardware RX FIFO buffer only for SW async mode.
        if (args[ARG_rxbuf].u_int > 0) {
            self->rxbuf = args[ARG_rxbuf].u_int;
            uint8_t rx_buf[self->rxbuf];
            cy_rslt_t result = cyhal_uart_config_software_buffer(&self->uart_obj, rx_buf, self->rxbuf);
            uart_assert_raise_val("Configuring the UART RX software buffer failed with return code %lx !", result);
        }

        if (self->flow & UART_HWCONTROL_CTS) {
            enable_rts = 1;
        }
        if (self->flow & UART_HWCONTROL_RTS) {
            enable_cts = 1;
        }
        cy_rslt_t result = cyhal_uart_enable_flow_control(&self->uart_obj, enable_cts, enable_rts);
        uart_assert_raise_val("Configuring the UART for flow control failed with return code %lx !", result);
    }
}

static mp_obj_t mp_machine_uart_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // Get UART bus.
    int uart_id = mp_obj_get_int(args[0]);
    if (uart_id < 0 || uart_id > MAX_UART) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("UART(%d) doesn't exist"), uart_id);
    }

    // Get Peripheralobject
    machine_uart_obj_t *self = uart_obj_alloc();
    self->init_flag = 0;
    // Initialise the UART peripheral.
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    return MP_OBJ_FROM_PTR(self);
}

static void mp_machine_uart_deinit(machine_uart_obj_t *self) {
    cyhal_uart_free(&(self->uart_obj));
    uart_tx_free(self);
    uart_rx_free(self);
    uart_cts_free(self);
    uart_rts_free(self);
    uart_obj_free(self);
}

static mp_int_t mp_machine_uart_any(machine_uart_obj_t *self) {
    return cyhal_uart_readable(&self->uart_obj);

}

static bool mp_machine_uart_txdone(machine_uart_obj_t *self) {
    return !(cyhal_uart_is_tx_active(&self->uart_obj));

}

// send a character
static void mp_machine_uart_writechar(machine_uart_obj_t *self, uint16_t data) {
    cy_rslt_t result = cyhal_uart_putc(&self->uart_obj, data);
    uart_assert_raise_val("Put character failed with return code %lx !", result);
}

// Get a character
static mp_int_t mp_machine_uart_readchar(machine_uart_obj_t *self) {
    uint8_t value;
    cy_rslt_t result = cyhal_uart_getc(&self->uart_obj, &value, self->timeout);
    if (result == CY_RSLT_SUCCESS) {
        return value;
    }
    return -1; // in case of timeout
}

// Before sending break all UART TX interrupt sources are disabled
// The state of UART TX interrupt sources is restored before function returns.
// Blocks until break is completed. Only call this function when UART TX FIFO and shifter are empty.
static void mp_machine_uart_sendbreak(machine_uart_obj_t *self) {
    uint32_t breakwidth = 4;  // Width of break condition. Valid range is the TX data width (4 to 16 bits). How to get that?
    Cy_SCB_UART_SendBreakBlocking(self->uart_obj.base, breakwidth);
}

static mp_irq_obj_t *mp_machine_uart_irq(machine_uart_obj_t *self, bool any_args, mp_arg_val_t *args) {
    if (self->mp_irq_obj == NULL) {
        self->mp_irq_trigger = 0;
        self->mp_irq_obj = mp_irq_new(&uart_irq_methods, MP_OBJ_FROM_PTR(self));
    }
    if (any_args) {
        // Check the handler
        mp_obj_t handler = args[MP_IRQ_ARG_INIT_handler].u_obj;
        if (handler != mp_const_none && !mp_obj_is_callable(handler)) {
            mp_raise_ValueError(MP_ERROR_TEXT("handler must be None or callable"));
        }

        // Get the trigger
        mp_uint_t trigger = args[MP_IRQ_ARG_INIT_trigger].u_int;
        mp_uint_t not_supported = trigger & ~MP_UART_ALLOWED_FLAGS;
        if (trigger != 0 && not_supported) {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("trigger 0x%08x unsupported"), not_supported);
        }
        // Reconfigure user IRQs
        uart_irq_config(self, false);
        self->mp_irq_obj->handler = handler;
        self->mp_irq_obj->ishard = args[MP_IRQ_ARG_INIT_hard].u_bool;
        self->mp_irq_trigger = trigger;
        uart_irq_config(self, true);
    }
    self->interrupt = 1;
    return self->mp_irq_obj;
}

static mp_uint_t mp_machine_uart_read(mp_obj_t self_in, void *buf_in, mp_uint_t size, int *errcode) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    void *dest = buf_in;
    size_t rx_length = size;
    if (self->interrupt == 1) {
        cy_rslt_t result = cyhal_uart_read_async(&self->uart_obj, dest, rx_length);
        if (result != CY_RSLT_SUCCESS) {
            *errcode = MP_EAGAIN;
            return MP_STREAM_ERROR;
        }
    } else {
        cyhal_uart_read(&self->uart_obj, dest, &rx_length);
    }
    if (rx_length <= 0) {
        *errcode = MP_EAGAIN;
        return MP_STREAM_ERROR;
    }
    return rx_length;
}


static mp_uint_t mp_machine_uart_write(mp_obj_t self_in, const void *buf_in, mp_uint_t size, int *errcode) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    size_t tx_length = size;
    if (self->interrupt == 1) {
        cyhal_uart_write_async(&self->uart_obj, (void *)buf_in, tx_length);
    } else {
        cyhal_uart_write(&self->uart_obj, (void *)buf_in, &tx_length);
    }
    if (tx_length < 0) {
        *errcode = MP_EAGAIN;
        return MP_STREAM_ERROR;
    }
    return tx_length;
}

static mp_uint_t mp_machine_uart_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    machine_uart_obj_t *self = self_in;
    mp_uint_t ret;
    if (request == MP_STREAM_POLL) {           // Need to understand the select
        mp_uint_t flags = arg;
        ret = 0;
        if ((flags & MP_STREAM_POLL_RD) && cyhal_uart_readable(&self->uart_obj)) {
            ret |= MP_STREAM_POLL_RD;
        }
        if ((flags & MP_STREAM_POLL_WR) && cyhal_uart_writable(&self->uart_obj)) {
            ret |= MP_STREAM_POLL_WR;
        }
    } else if (request == MP_STREAM_FLUSH) {
        // Since uart.write() waits up to the last byte, uart.flush() always succeeds.
        ret = 0;
    } else {
        *errcode = MP_EINVAL;
        ret = MP_STREAM_ERROR;
    }
    return ret;
}
