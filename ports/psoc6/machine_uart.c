// mpy includes
#include "py/mphal.h"
#include "py/mperrno.h"
#include "py/ringbuf.h"
#include "py/runtime.h"

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

#define uart_assert_raise_val(msg, ret)   if (ret != CY_RSLT_SUCCESS) { \
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT(msg), ret); \
}

typedef struct _machine_uart_obj_t {
    mp_obj_base_t base;
    cyhal_uart_t uart_obj;
    uint32_t bits;
    uint32_t stop;
    uint32_t baudrate;
    cyhal_uart_parity_t parity;
    machine_pin_phy_obj_t *tx;
    machine_pin_phy_obj_t *rx;
    machine_pin_phy_obj_t *cts;
    machine_pin_phy_obj_t *rts;
    uint16_t timeout;
    cyhal_uart_cfg_t cfg;
} machine_uart_obj_t;


 #define MICROPY_PY_MACHINE_UART_CLASS_CONSTANTS
//     { MP_ROM_QSTR(MP_QSTR_CTS), MP_ROM_INT(UART_HWCONTROL_CTS) },
//     { MP_ROM_QSTR(MP_QSTR_RTS), MP_ROM_INT(UART_HWCONTROL_RTS) },

machine_uart_obj_t *uart_obj[MAX_UART] = { NULL };

static void uart_init(machine_uart_obj_t *machine_uart_obj) {
    uint32_t actualbaud;

    // // Initialize the UART configuration structure
    machine_uart_obj->cfg.data_bits = machine_uart_obj->bits;
    machine_uart_obj->cfg.stop_bits = machine_uart_obj->stop;
    machine_uart_obj->cfg.parity = machine_uart_obj->parity;
    machine_uart_obj->cfg.rx_buffer = NULL;
    machine_uart_obj->cfg.rx_buffer_size = 0;


    // Initialize the UART Block
    cy_rslt_t result = cyhal_uart_init(&machine_uart_obj->uart_obj, machine_uart_obj->tx->addr, machine_uart_obj->rx->addr, machine_uart_obj->cts->addr,
        machine_uart_obj->rts->addr, NULL, &(machine_uart_obj->cfg));

    uart_assert_raise_val("UART initialisation failed with return code %lx !", result);

    // Set the baud rate
    result = cyhal_uart_set_baud(&machine_uart_obj->uart_obj, machine_uart_obj->baudrate, &actualbaud);

    uart_assert_raise_val("UART baudrate failed with return code %lx !", result);

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
    machine_pin_phy_obj_t *tx = pin_phy_realloc(pin_name, PIN_PHY_FUNC_NONE);
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
    machine_pin_phy_obj_t *rx = pin_phy_realloc(pin_name, PIN_PHY_FUNC_NONE);

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
    machine_pin_phy_obj_t *cts = pin_phy_realloc(pin_name, PIN_PHY_FUNC_NONE);

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
    machine_pin_phy_obj_t *rts = pin_phy_realloc(pin_name, PIN_PHY_FUNC_NONE);

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
    // machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "UART");
}

static void mp_machine_uart_init_helper(machine_uart_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_baudrate, ARG_bits, ARG_parity, ARG_stop, ARG_tx, ARG_rx, ARG_timeout, ARG_rxbuf, ARG_txbuf, ARG_rts, ARG_cts };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_baudrate, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_bits, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_parity, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_INT(-1)} },
        { MP_QSTR_stop, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_tx, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_rx, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_rxbuf, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_txbuf, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_rts, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_cts, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    self->baudrate = CYHAL_UART_DEFAULT_BAUD;
    // Set baudrate if configured.
    if (args[ARG_baudrate].u_int > 0) {
        self->baudrate = args[ARG_baudrate].u_int;
    }

    self->bits = 8;
    // Set bits if configured.
    if (args[ARG_bits].u_int > 0) {
        self->bits = args[ARG_bits].u_int;
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

    self->stop = 0;
    // Set stop bits if configured.
    if (args[ARG_stop].u_int > 0) {
        self->stop = (args[ARG_stop].u_int);
    }

    // Set TX/RX pins if configured.
    if (args[ARG_tx].u_obj != mp_const_none) {
        uart_tx_alloc(self, args[ARG_tx].u_obj);
    }

    if (args[ARG_rx].u_obj != mp_const_none) {
        uart_rx_alloc(self, args[ARG_rx].u_obj);
    }

    // Set CTS/RTS pins if configured.
    if (args[ARG_rts].u_obj != mp_const_none) {
        uart_rts_alloc(self, args[ARG_rts].u_obj);
    }

    if (args[ARG_cts].u_obj != mp_const_none) {
        uart_cts_alloc(self, args[ARG_cts].u_obj);
    }

    // initialise UART at cyhal level
    if (n_args > 0 || kw_args->used > 0) {
        uart_init(self);
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

    // Initialise the UART peripheral.
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    mp_machine_uart_init_helper(self, n_args - 1, args + 1, &kw_args);

    return MP_OBJ_FROM_PTR(self);
}

static void mp_machine_uart_deinit(machine_uart_obj_t *self) {
    cyhal_uart_free(&self->uart_obj);
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
    return cyhal_uart_is_tx_active(&self->uart_obj);

}


static mp_uint_t mp_machine_uart_read(mp_obj_t self_in, void *buf_in, mp_uint_t size, int *errcode) {
    // machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    // mp_uint_t timeout = self->timeout;
    // uint8_t *dest = buf_in;
    return 0;
}

static mp_uint_t mp_machine_uart_write(mp_obj_t self_in, const void *buf_in, mp_uint_t size, int *errcode) {
    // machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return 0;
}

static mp_uint_t mp_machine_uart_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    // machine_uart_obj_t *self = self_in;
    return 0;
}
