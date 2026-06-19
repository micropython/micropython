/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Infineon Technologies AG
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

// This file is never compiled standalone, it's included directly from
// extmod/machine_uart.c via MICROPY_PY_MACHINE_UART_INCLUDEFILE.

// std includes
#include <string.h>

// MTB / PDL includes
#include "cybsp.h"
#include "cy_scb_uart.h"
#include "cy_scb_common.h"
#include "cy_sysclk.h"

// MicroPython includes
#include "py/ringbuf.h"
#include "py/mphal.h"
#include "py/mperrno.h"
#include "py/nlr.h"
#include "py/stream.h"

// Port-specific includes
#include "genhdr/pins_af.h"
#include "modmachine.h"
#include "mphalport.h"
#include "machine_scb.h"
#include "sys_int.h"

/******************************************************************************/
// Defaults and constants

#define uart_assert_raise_val(msg, ret)   if (ret != CY_SCB_UART_SUCCESS) { \
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT(msg), ret); \
}

// Flow control macros not part of PSOC Edge PDL
#define UART_FLOW_CONTROL_NONE    (0)
#define UART_FLOW_CONTROL_RTS     (1)
#define UART_FLOW_CONTROL_CTS     (2)

// Class-level constants exposed to Python
#define MICROPY_PY_MACHINE_UART_CLASS_CONSTANTS \
    { MP_ROM_QSTR(MP_QSTR_IRQ_RXIDLE), MP_ROM_INT(CY_SCB_RX_INTR_NOT_EMPTY) }, \
    { MP_ROM_QSTR(MP_QSTR_IRQ_BREAK), MP_ROM_INT(CY_SCB_RX_INTR_UART_BREAK_DETECT) }, \
    { MP_ROM_QSTR(MP_QSTR_IRQ_TXIDLE), MP_ROM_INT(CY_SCB_TX_INTR_UART_DONE) }, \
    { MP_ROM_QSTR(MP_QSTR_RTS), MP_ROM_INT(UART_FLOW_CONTROL_RTS) }, \
    { MP_ROM_QSTR(MP_QSTR_CTS), MP_ROM_INT(UART_FLOW_CONTROL_CTS) }, \

/******************************************************************************/

typedef struct _machine_uart_obj_t {
    mp_obj_base_t base;
    int id;                         // id matches the SCB id.
    machine_scb_obj_t *scb_obj;
    mp_hal_pin_obj_t tx;
    mp_hal_pin_obj_t rx;
    mp_hal_pin_obj_t rts;
    mp_hal_pin_obj_t cts;
    uint32_t baudrate;
    uint32_t flow;
    uint8_t bits;
    uint8_t parity;
    uint8_t stop;
    uint32_t timeout_ms;
    uint32_t timeout_char_ms;
    cy_stc_scb_uart_context_t ctx;
    ringbuf_t rx_ringbuf;
    #if MICROPY_PY_MACHINE_UART_IRQ
    uint32_t mp_irq_trigger;
    uint32_t mp_irq_flags;
    mp_irq_obj_t *mp_irq_obj;
    #endif
} machine_uart_obj_t;

static machine_uart_obj_t *mp_machine_uart_obj_get(uint8_t id) {
    for (uint8_t i = 0; i < MICROPY_PY_MACHINE_UART_NUM_ENTRIES; i++) {
        if (MP_STATE_PORT(machine_uart_obj[i]) != NULL) {
            if (MP_STATE_PORT(machine_uart_obj[i])->id == id) {
                return MP_STATE_PORT(machine_uart_obj[i]);
            }
        }
    }
    return NULL;
}

static machine_uart_obj_t *mp_machine_uart_obj_alloc(void) {
    machine_uart_obj_t *self = NULL;
    for (uint8_t i = 0; i < MICROPY_PY_MACHINE_UART_NUM_ENTRIES; i++) {
        if (MP_STATE_PORT(machine_uart_obj[i]) == NULL) {
            self = mp_obj_malloc(machine_uart_obj_t, &machine_uart_type);
            MP_STATE_PORT(machine_uart_obj[i]) = self;
            break;
        }
    }

    if (self == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("all available UART instances are allocated"));
    }

    return self;
}

static void mp_machine_uart_obj_free(machine_uart_obj_t *self) {
    for (uint8_t i = 0; i < MICROPY_PY_MACHINE_UART_NUM_ENTRIES; i++) {
        if (MP_STATE_PORT(machine_uart_obj[i]) == self) {
            MP_STATE_PORT(machine_uart_obj[i]) = NULL;
            break;
        }
    }
}

MP_REGISTER_ROOT_POINTER(struct _machine_uart_obj_t *machine_uart_obj[MICROPY_PY_MACHINE_UART_NUM_ENTRIES]);

#if MICROPY_PY_MACHINE_UART_IRQ
static void machine_uart_irq_rx_idle(machine_uart_obj_t *self);
static void machine_uart_irq_rx_break(machine_uart_obj_t *self);
static void machine_uart_irq_tx_idle(machine_uart_obj_t *self);
#endif

static void machine_uart_fill_rx_ring_buff(machine_uart_obj_t *self) {
    uint32_t available_rx_frames = Cy_SCB_UART_GetNumInRxFifo(self->scb_obj->scb);
    for (uint32_t i = 0; i < available_rx_frames; i++) {
        if (!ringbuf_put(&self->rx_ringbuf, (uint8_t)Cy_SCB_UART_Get(self->scb_obj->scb))) {
            /**
             * No overflow handling.
             * Just return and wait for next interrupt
             * to read the remaining data.
             */
            return;
        }
    }
}

static void machine_uart_scb_isr(mp_obj_t hw_uart_obj) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(hw_uart_obj);
    #if MICROPY_PY_MACHINE_UART_IRQ
    self->mp_irq_flags = 0; // Clear flags at the beginning of the ISR.
    #endif

    if (0UL != (CY_SCB_RX_INTR & Cy_SCB_GetInterruptCause(self->scb_obj->scb))) {
        if (0UL != (CY_SCB_RX_INTR_NOT_EMPTY & Cy_SCB_GetRxInterruptStatusMasked(self->scb_obj->scb))) {
            machine_uart_fill_rx_ring_buff(self);

            #if MICROPY_PY_MACHINE_UART_IRQ
            machine_uart_irq_rx_idle(self);
            #endif

            Cy_SCB_ClearRxInterrupt(self->scb_obj->scb, CY_SCB_RX_INTR_NOT_EMPTY);
        }

        #if MICROPY_PY_MACHINE_UART_IRQ
        if (0UL != (CY_SCB_RX_INTR_UART_BREAK_DETECT & Cy_SCB_GetRxInterruptStatusMasked(self->scb_obj->scb))) {
            machine_uart_irq_rx_break(self);
            Cy_SCB_ClearRxInterrupt(self->scb_obj->scb, CY_SCB_RX_INTR_UART_BREAK_DETECT);
        }
        #endif
    }

    #if MICROPY_PY_MACHINE_UART_IRQ
    if ((0UL != (CY_SCB_TX_INTR & Cy_SCB_GetInterruptCause(self->scb_obj->scb))) &&
        (0UL != (CY_SCB_TX_INTR_UART_DONE & Cy_SCB_GetTxInterruptStatusMasked(self->scb_obj->scb)))) {
        machine_uart_irq_tx_idle(self);
        Cy_SCB_ClearTxInterrupt(self->scb_obj->scb, CY_SCB_TX_INTR_UART_DONE);
    }
    #endif
}

void machine_uart_obj_make_or_reuse(machine_uart_obj_t **self_ptr, uint8_t id, bool *is_new) {
    /**
     * UART() constructor path:
     *
     * Create or reuse and object based on the id.
     * If the object for the given id already exists,
     * reuse it and reinit the hardware with the new params.
     * If the object is being created for the first time,
     * allocate it and the associated SCB object.
     */

    /* Use object if it already exists */
    (*self_ptr) = mp_machine_uart_obj_get(id);
    (*is_new) = false;

    if (*self_ptr == NULL) {
        /* Create a new object and allocate the scb instance if free.*/
        if (machine_scb_is_free(id)) {
            (*self_ptr) = mp_machine_uart_obj_alloc();
            (*self_ptr)->id = id;
            (*self_ptr)->scb_obj = machine_scb_obj_alloc(id, *self_ptr, machine_uart_scb_isr);
        } else {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("SCB %u is already in use by a machine.I2C or machine.SPI instance."), id);
        }
        (*is_new) = true;
    }
}

static void machine_uart_baudrate_set(machine_uart_obj_t *self) {
    /**
     * TODO: Review clock configuration and formula to calculate the divider value.
     *       Clock sources, path and proper divider configuration.
     * Given the clock peripheral = 100 MHz
     * Oversampling = 12 (fixed for now)
     * The formula to calculate the divider value is: divider = clock / (baudrate * oversample)
     */
    #define CLOCK_PERIPHERAL_FREQ_HZ (100000000UL)

    Cy_SysClk_PeriphAssignDivider(self->scb_obj->clk, CY_SYSCLK_DIV_16_BIT, 0UL);
    uint32_t divider = CLOCK_PERIPHERAL_FREQ_HZ / (self->baudrate * 12UL);
    Cy_SysClk_PeriphSetDivider(CY_SYSCLK_DIV_16_BIT, 0UL, divider);
    Cy_SysClk_PeriphEnableDivider(CY_SYSCLK_DIV_16_BIT, 0UL);
}

static inline uint8_t machine_uart_break_width(machine_uart_obj_t *self) {
    return 1 +  // Start bit
           self->bits +
           (self->parity != CY_SCB_UART_PARITY_NONE ? 1 : 0) +
           (self->stop == CY_SCB_UART_STOP_BITS_1 ? 1 : 2) +
           1; // Extra bit, make the frame longer
}

static void machine_uart_hw_init(machine_uart_obj_t *self) {
    cy_stc_scb_uart_config_t config =
    {
        .uartMode = CY_SCB_UART_STANDARD,
        .enableMultiProcessorMode = false,
        .smartCardRetryOnNack = false,
        .irdaInvertRx = false,
        .irdaEnableLowPowerReceiver = false,

        .oversample = 12UL,

        .enableMsbFirst = false,
        .dataWidth = self->bits,
        .parity = self->parity,
        .stopBits = self->stop,
        .enableInputFilter = false,
        .breakWidth = machine_uart_break_width(self),
        .dropOnFrameError = false,
        .dropOnParityError = false,

        .receiverAddress = 0UL,
        .receiverAddressMask = 0UL,
        .acceptAddrInFifo = false,

        /**
         * TODO: Enable CTS/RTS based on user config.
         */
        .enableCts = false,
        .ctsPolarity = CY_SCB_UART_ACTIVE_LOW,
        .rtsRxFifoLevel = 0UL,
        .rtsPolarity = CY_SCB_UART_ACTIVE_LOW,

        .rxFifoTriggerLevel = 0UL,
        .rxFifoIntEnableMask = 0UL,
        .txFifoTriggerLevel = 0UL,
        .txFifoIntEnableMask = 0UL,
    };

    cy_en_scb_uart_status_t rslt = Cy_SCB_UART_Init(self->scb_obj->scb, &config, &self->ctx);
    uart_assert_raise_val("failed to initialize UART hardware, error code: %d", rslt);

    sys_int_init(&(self->scb_obj->irq));
    Cy_SCB_SetRxInterruptMask(self->scb_obj->scb, CY_SCB_RX_INTR_NOT_EMPTY);

    Cy_SCB_UART_Enable(self->scb_obj->scb);
}

static void machine_uart_hw_deinit(machine_uart_obj_t *self) {
    Cy_SCB_UART_Disable(self->scb_obj->scb, &self->ctx);
    sys_int_deinit(&self->scb_obj->irq);
}

static bool machine_uart_rx_wait(machine_uart_obj_t *self, uint32_t timeout_ms) {
    uint32_t timeout_time_ms = mp_hal_ticks_ms() + timeout_ms;

    while (ringbuf_avail(&self->rx_ringbuf) == 0) {
        if (mp_hal_ticks_ms() > timeout_time_ms) {
            return false;
        }
        mp_event_handle_nowait();
    }

    return true;
}

static bool machine_uart_tx_wait(machine_uart_obj_t *self, uint32_t timeout_ms) {
    uint32_t timeout_time_ms = mp_hal_ticks_ms() + timeout_ms;
    while ((Cy_SCB_UART_GetTxFifoStatus(self->scb_obj->scb) & CY_SCB_UART_TX_NOT_FULL) == 0) {
        if (mp_hal_ticks_ms() > timeout_time_ms) {
            return false;
        }
        mp_event_handle_nowait();
    }

    return true;
}

/******************************************************************************/
// MicroPython bindings

static void mp_machine_uart_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "UART(tx="MP_HAL_PIN_FMT ", "
        "rx="MP_HAL_PIN_FMT ", "
        "baudrate=%ld, "
        "bits=%u, "
        "parity=%u, "
        "stop=%u, ",
        mp_hal_pin_name(self->tx),
        mp_hal_pin_name(self->rx),
        self->baudrate,
        self->bits,
        self->parity,
        self->stop
        );

    if (self->rts != NULL) {
        mp_printf(print, "rts="MP_HAL_PIN_FMT ", ", mp_hal_pin_name(self->rts));
    }

    if (self->cts != NULL) {
        mp_printf(print, "cts="MP_HAL_PIN_FMT ", ", mp_hal_pin_name(self->cts));
    }

    mp_printf(print, "flow=%u, "
        "timeout=%ld, "
        "timeout_char=%ld, "
        "rxbuf=%d)",
        self->flow,
        self->timeout_ms,
        self->timeout_char_ms,
        self->rx_ringbuf.size
        );
}

#define DEFAULT_UART_BAUDRATE     (115200)
#define DEFAULT_UART_BITS         (8)
#define DEFAULT_UART_STOP         (1)
#define DEFAULT_UART_RXBUF_SIZE   (256)

enum {
    ARG_tx, ARG_rx,
    ARG_baudrate, ARG_bits, ARG_parity, ARG_stop,
    ARG_rts, ARG_cts, ARG_flow,
    ARG_timeout, ARG_timeout_char, ARG_rxbuf
};

static const mp_arg_t allowed_args[] = {
    { MP_QSTR_tx,           MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    { MP_QSTR_rx,           MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    { MP_QSTR_baudrate,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_UART_BAUDRATE} },
    { MP_QSTR_bits,         MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_UART_BITS} },
    { MP_QSTR_parity,       MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    { MP_QSTR_stop,         MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_UART_STOP} },
    { MP_QSTR_rts,          MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    { MP_QSTR_cts,          MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    { MP_QSTR_flow,         MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = UART_FLOW_CONTROL_NONE} },
    { MP_QSTR_timeout,      MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_timeout_char, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_rxbuf,        MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_UART_RXBUF_SIZE} },
};

/**
 * Core init implementation. Accepts a pointer-to-pointer for self so it can
 * allocate a new object when *self_ptr is NULL (constructor path). When called
 * from init() the object is already allocated so *self_ptr is non-NULL.
 */
static void machine_uart_init_impl(machine_uart_obj_t **self_ptr, int uart_id, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    /* -- TX / RX pins -- */
    uint8_t pin_num = 2U;
    #define MAX_MACHINE_UART_OBJ_AF_PINS 4
    mp_hal_pin_af_config_t uart_pins_af_config[MAX_MACHINE_UART_OBJ_AF_PINS] = {
        /* TX */ MP_HAL_PIN_AF_CONF_INIT_GPIO_SIGNAL(CY_GPIO_DM_STRONG_IN_OFF, 1, MACHINE_PIN_AF_SIGNAL_UART_TX),
        /* RX */ MP_HAL_PIN_AF_CONF_INIT_GPIO_SIGNAL(CY_GPIO_DM_HIGHZ, 0, MACHINE_PIN_AF_SIGNAL_UART_RX),
    };

    if (args[ARG_tx].u_obj != mp_const_none) {
        mp_hal_pin_obj_t tx_pin = mp_hal_get_pin_obj(args[ARG_tx].u_obj);
        MP_HAL_PIN_AF_CONF_SET_PIN_AF(uart_pins_af_config[0], tx_pin);
    }

    if (args[ARG_rx].u_obj != mp_const_none) {
        mp_hal_pin_obj_t rx_pin = mp_hal_get_pin_obj(args[ARG_rx].u_obj);
        MP_HAL_PIN_AF_CONF_SET_PIN_AF(uart_pins_af_config[1], rx_pin);
    }

    /* -- Flow control pins -- */
    uint32_t flow = (uint32_t)args[ARG_flow].u_int;

    /** TODO: Currently throw error if flow is not NONE.
     * This needs to be implemented
     * {
     */
    if (flow != UART_FLOW_CONTROL_NONE) {
        mp_raise_ValueError(MP_ERROR_TEXT("flow control not supported yet. Keep flow=0"));
    }
    /** } */

    #define PIN_AF_CONFIG_INDEX_NONE (0xFF)
    uint8_t pin_af_conf_rts_index = PIN_AF_CONFIG_INDEX_NONE;
    if (flow & UART_FLOW_CONTROL_RTS) {
        pin_af_conf_rts_index = pin_num;
        pin_num++;
        uart_pins_af_config[pin_af_conf_rts_index] = MP_HAL_PIN_AF_CONF_INIT_GPIO_SIGNAL(CY_GPIO_DM_STRONG_IN_OFF, 1, MACHINE_PIN_AF_SIGNAL_UART_RTS);
        if (args[ARG_rts].u_obj != mp_const_none) {
            mp_hal_pin_obj_t rts_pin = mp_hal_get_pin_obj(args[ARG_rts].u_obj);
            MP_HAL_PIN_AF_CONF_SET_PIN_AF(uart_pins_af_config[pin_af_conf_rts_index], rts_pin);
        }
    }

    uint8_t pin_af_conf_cts_index = PIN_AF_CONFIG_INDEX_NONE;
    if (flow & UART_FLOW_CONTROL_CTS) {
        pin_af_conf_cts_index = pin_num;
        pin_num++;
        uart_pins_af_config[pin_af_conf_cts_index] = MP_HAL_PIN_AF_CONF_INIT_GPIO_SIGNAL(CY_GPIO_DM_HIGHZ, 0, MACHINE_PIN_AF_SIGNAL_UART_CTS);
        if (args[ARG_cts].u_obj != mp_const_none) {
            mp_hal_pin_obj_t cts_pin = mp_hal_get_pin_obj(args[ARG_cts].u_obj);
            MP_HAL_PIN_AF_CONF_SET_PIN_AF(uart_pins_af_config[pin_af_conf_cts_index], cts_pin);
        }
    }

    /* -- Resolve ID - pin match -- */
    uint8_t fn_unit = uart_id;
    mp_hal_periph_pins_af_resolve_fn_unit(uart_pins_af_config, pin_num, MACHINE_PIN_AF_FN_UART, (machine_pin_af_unit_t *)&fn_unit);

    /* -- Resolve not provided AF pins -- */
    mp_hal_periph_pins_af_resolve_pin_af(uart_pins_af_config, pin_num, (machine_pin_af_unit_t)fn_unit);

    /* -- Baudrate -- */
    uint32_t baudrate = (uint32_t)args[ARG_baudrate].u_int;
    if (baudrate == 0U) {
        mp_raise_ValueError(MP_ERROR_TEXT("baudrate must be non-zero"));
    }

    /* -- Data bits -- */
    uint8_t bits = args[ARG_bits].u_int;
    /**
     * TODO: Add support for 7 (or below <8 bits) and 9 bits.
     */
    if (bits != 8) {
        mp_raise_ValueError(MP_ERROR_TEXT("bits must be 8"));
    }

    /* -- Parity -- */
    mp_obj_t parity_arg = args[ARG_parity].u_obj;
    uint8_t parity;
    if (parity_arg == mp_const_none) {
        parity = (uint8_t)CY_SCB_UART_PARITY_NONE;
    } else {
        int p = mp_obj_get_int(parity_arg);
        if (p == 0) {
            parity = (uint8_t)CY_SCB_UART_PARITY_EVEN;
        } else if (p == 1) {
            parity = (uint8_t)CY_SCB_UART_PARITY_ODD;
        } else {
            mp_raise_ValueError(MP_ERROR_TEXT("parity must be None, 0 (even) or 1 (odd)"));
        }
    }

    /* -- Stop bits -- */
    int stop_arg = args[ARG_stop].u_int;
    uint8_t stop;
    if (stop_arg == 1) {
        stop = (uint8_t)CY_SCB_UART_STOP_BITS_1;
    } else if (stop_arg == 2) {
        stop = (uint8_t)CY_SCB_UART_STOP_BITS_2;
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("stop bits must be 1 or 2"));
    }

    /* -- Timeouts -- */
    uint32_t timeout_ms = args[ARG_timeout].u_int;
    if (timeout_ms < 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("timeout must be non-negative"));
    }

    uint32_t timeout_char_ms = args[ARG_timeout_char].u_int;
    if (timeout_char_ms < 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("timeout_char must be non-negative"));
    }
    /* Make sure timeout_char is at least as long as a whole character (13 bits to be safe). */
    uint32_t min_timeout_char = 13000 / baudrate + 1;
    if (timeout_char_ms < min_timeout_char) {
        timeout_char_ms = min_timeout_char;
    }

    machine_uart_obj_t *self = *self_ptr;

    /* -- Object allocation -- */
    bool is_new;
    bool is_make_obj_required = (*self_ptr == NULL) ? true : false;
    if (is_make_obj_required) {
        machine_uart_obj_make_or_reuse(self_ptr, fn_unit, &is_new);
        self = *self_ptr;
    }

    /* -- Reinitialization reset -- */
    /* For reused UART() object or init() path */
    if (!is_new) {
        machine_uart_hw_deinit(self);
        m_del(uint8_t, self->rx_ringbuf.buf, self->rx_ringbuf.size);
    }

    /* -- UART params init -- */
    self->tx = uart_pins_af_config[0].pin;
    self->rx = uart_pins_af_config[1].pin;
    self->rts = (pin_af_conf_rts_index != PIN_AF_CONFIG_INDEX_NONE) ? uart_pins_af_config[pin_af_conf_rts_index].pin : MP_OBJ_NULL;
    self->cts = (pin_af_conf_cts_index != PIN_AF_CONFIG_INDEX_NONE) ? uart_pins_af_config[pin_af_conf_cts_index].pin : MP_OBJ_NULL;
    self->flow = flow;
    self->baudrate = baudrate;
    self->bits = bits;
    self->parity = parity;
    self->stop = stop;
    self->timeout_ms = timeout_ms;
    self->timeout_char_ms = timeout_char_ms;

    /* -- IRQ init -- */
    #if MICROPY_PY_MACHINE_UART_IRQ
    self->mp_irq_obj = NULL;
    self->mp_irq_trigger = 0;
    self->mp_irq_flags = 0;
    #endif

    /* -- Initialise hardware -- */
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        ringbuf_alloc(&self->rx_ringbuf, args[ARG_rxbuf].u_int);
        mp_hal_periph_pins_af_init(uart_pins_af_config, pin_num);
        machine_uart_baudrate_set(self);
        machine_uart_hw_init(self);
        nlr_pop();
    } else {
        // Ensure partially-initialized instances are fully released on init failure.
        machine_uart_hw_deinit(self);
        m_del(uint8_t, self->rx_ringbuf.buf, self->rx_ringbuf.size);
        nlr_raise(nlr.ret_val);
    }
}

static mp_obj_t mp_machine_uart_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, MP_OBJ_FUN_ARGS_MAX, true);

    /**
     * Only the constructor takes the id.
     * Its validation together with the rest of the arguments is
     * delegated to machine_uart_init_implt() which also allocates
     * the object self = NULL.
    */
    int uart_id = MACHINE_PIN_AF_UNIT_NONE;
    size_t init_n_args = n_args;
    const mp_obj_t *init_args = args;
    if (n_args > 0 && mp_obj_is_int(args[0])) {
        uart_id = mp_obj_get_int(args[0]);
        if (uart_id < 0 || uart_id >= MICROPY_PY_MACHINE_SCB_NUM_ENTRIES) {
            mp_raise_ValueError(MP_ERROR_TEXT("UART id out of range"));
        }
        init_n_args = n_args - 1;
        init_args = args + 1;
    } else if (n_args > 0) {
        mp_raise_TypeError(MP_ERROR_TEXT("UART id must be an integer"));
    }

    machine_uart_obj_t *self = NULL;
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    machine_uart_init_impl(&self, uart_id, init_n_args, init_args, &kw_args);
    return MP_OBJ_FROM_PTR(self);
}

static void mp_machine_uart_deinit(machine_uart_obj_t *self) {
    machine_uart_hw_deinit(self);
    m_del(uint8_t, self->rx_ringbuf.buf, self->rx_ringbuf.size);
    machine_scb_obj_free(self->scb_obj);
    mp_machine_uart_obj_free(self);
}

static void mp_machine_uart_init_helper(machine_uart_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    machine_uart_init_impl(&self, self->id, n_args, pos_args, kw_args);
}

static mp_int_t mp_machine_uart_any(machine_uart_obj_t *self) {
    return ringbuf_avail(&self->rx_ringbuf);
}

static bool mp_machine_uart_txdone(machine_uart_obj_t *self) {
    return Cy_SCB_UART_IsTxComplete(self->scb_obj->scb);
}

#if MICROPY_PY_MACHINE_UART_SENDBREAK
static void mp_machine_uart_sendbreak(machine_uart_obj_t *self) {
    Cy_SCB_UART_SendBreakBlocking(self->scb_obj->scb, machine_uart_break_width(self));
}
#endif

#if MICROPY_PY_MACHINE_UART_READCHAR_WRITECHAR
static mp_int_t mp_machine_uart_readchar(machine_uart_obj_t *self) {
    if (machine_uart_rx_wait(self, self->timeout_ms)) {
        return (uint8_t)ringbuf_get(&self->rx_ringbuf);
    }

    return MP_STREAM_ERROR;
}

static void mp_machine_uart_writechar(machine_uart_obj_t *self, uint16_t data) {
    if (machine_uart_tx_wait(self, self->timeout_ms)) {
        Cy_SCB_UART_Put(self->scb_obj->scb, (uint32_t)data);
    } else {
        mp_raise_OSError(MP_ETIMEDOUT);
    }
}
#endif

static mp_uint_t mp_machine_uart_read(mp_obj_t self_in, void *buf_in, mp_uint_t size, int *errcode) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (size == 0) {
        return 0;
    }

    /* wait for first char to become available */
    if (!machine_uart_rx_wait(self, self->timeout_ms)) {
        /* return EAGAIN error to indicate non-blocking (then read() method returns None) */
        *errcode = MP_EAGAIN;
        return MP_STREAM_ERROR;
    }

    uint32_t read_count = 0;
    uint32_t to_read;
    do {
        to_read = (size < ringbuf_avail(&self->rx_ringbuf)) ? size : ringbuf_avail(&self->rx_ringbuf);
        ringbuf_memcpy_get_internal(&self->rx_ringbuf, (uint8_t *)buf_in + read_count, to_read);
        read_count += to_read;
        size -= to_read;
    } while (size > 0 && machine_uart_rx_wait(self, self->timeout_char_ms));

    return read_count;
}

static mp_uint_t mp_machine_uart_write(mp_obj_t self_in, const void *buf_in, mp_uint_t size, int *errcode) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);

    /* wait to be able to write the first character. */
    if (!machine_uart_tx_wait(self, self->timeout_ms)) {
        /* EAGAIN causes write to return None */
        *errcode = MP_EAGAIN;
        return MP_STREAM_ERROR;
    }

    uint32_t write_count = 0;
    do {
        uint32_t written = Cy_SCB_UART_PutArray(self->scb_obj->scb, (void *)buf_in, size);
        buf_in = (const uint8_t *)buf_in + written;
        size -= written;
        write_count += written;
    } while (size > 0 && machine_uart_tx_wait(self, self->timeout_char_ms));

    return write_count;
}

static mp_uint_t mp_machine_uart_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_uint_t ret;
    if (request == MP_STREAM_POLL) {
        uintptr_t flags = arg;
        ret = 0;
        if ((flags & MP_STREAM_POLL_RD) && ringbuf_avail(&self->rx_ringbuf) > 0) {
            ret |= MP_STREAM_POLL_RD;
        }
        if ((flags & MP_STREAM_POLL_WR) && (Cy_SCB_UART_GetTxFifoStatus(self->scb_obj->scb) & CY_SCB_UART_TX_NOT_FULL)) {
            ret |= MP_STREAM_POLL_WR;
        }
    } else if (request == MP_STREAM_FLUSH) {
        /**
         * Estimate the time required to shift out all remaining bytes from the TX hardware pipeline.
         * Cy_SCB_UART_GetNumInTxFifo() returns the exact byte count in the TX FIFO but explicitly
         * excludes the TX shifter register (1 byte). Adding 1 accounts for that shifter. The sum is
         * multiplied by 13 bits per symbol (1 start + 8 data + 2 stop, 1 parity), and multiplied by 2
         * (for safety margin) and divided by the baudrate to get the duration in milliseconds.
         * uint32_t is sufficient: max FIFO is < 256 bytes, so the worst-case duration is well within
         * the uint32_t range even at the lowest supported baud rates.
         */
        uint32_t timeout = mp_hal_ticks_ms() + (1
            + Cy_SCB_UART_GetNumInTxFifo(self->scb_obj->scb)
            ) * 13000 * 2 / self->baudrate;
        do {
            if (mp_machine_uart_txdone(self)) {
                return 0;
            }
            mp_event_handle_nowait();
        } while (mp_hal_ticks_ms() < timeout);
        *errcode = MP_ETIMEDOUT;
        ret = MP_STREAM_ERROR;
    } else {
        *errcode = MP_EINVAL;
        ret = MP_STREAM_ERROR;
    }
    return ret;
}

void machine_uart_deinit_all() {
    for (uint8_t i = 0; i < MICROPY_PY_MACHINE_UART_NUM_ENTRIES; i++) {
        machine_uart_obj_t *self = MP_STATE_PORT(machine_uart_obj[i]);
        if (self != NULL) {
            mp_machine_uart_deinit(self);
        }
    }
}

#if MICROPY_PY_MACHINE_UART_IRQ

#define MP_UART_ALLOWED_FLAGS (CY_SCB_RX_INTR_NOT_EMPTY | \
    CY_SCB_RX_INTR_UART_BREAK_DETECT | \
    CY_SCB_TX_INTR_UART_DONE)

static void machine_uart_irq_rx_idle(machine_uart_obj_t *self) {
    /**
     * Use CY_SCB_RX_INTR_NOT_EMPTY as RX_IDLE combined with
     * checking if the RX FIFO is empty. Therefore this function
     * will be called after reading all the data in the
     * RX FIFO.
     */
    if ((self->mp_irq_obj != NULL) &&
        (self->mp_irq_obj->handler != NULL) &&
        (self->mp_irq_trigger & CY_SCB_RX_INTR_NOT_EMPTY) &&
        (Cy_SCB_UART_GetNumInRxFifo(self->scb_obj->scb) == 0)) {
        self->mp_irq_flags |= CY_SCB_RX_INTR_NOT_EMPTY;
        mp_irq_handler(self->mp_irq_obj);
    }
}

static void machine_uart_irq_rx_break(machine_uart_obj_t *self) {
    if ((self->mp_irq_obj != NULL) &&
        (self->mp_irq_obj->handler != NULL) &&
        (self->mp_irq_trigger & CY_SCB_RX_INTR_UART_BREAK_DETECT)) {
        self->mp_irq_flags |= CY_SCB_RX_INTR_UART_BREAK_DETECT;
        mp_irq_handler(self->mp_irq_obj);
    }
}

static void machine_uart_irq_tx_idle(machine_uart_obj_t *self) {
    if ((self->mp_irq_obj != NULL) &&
        (self->mp_irq_obj->handler != NULL) &&
        (self->mp_irq_trigger & CY_SCB_TX_INTR_UART_DONE)) {
        self->mp_irq_flags |= CY_SCB_TX_INTR_UART_DONE;
        mp_irq_handler(self->mp_irq_obj);
    }
}

static void machine_uart_irq_scb_config(machine_uart_obj_t *self, bool enable) {
    if (enable) {
        /**
         * CY_SCB_RX_INTR_NOT_EMPTY is always enabled.
         */
        if (self->mp_irq_trigger & CY_SCB_RX_INTR_UART_BREAK_DETECT) {
            /**
             * Clear any stale sticky status before unmasking to prevent a
             * spurious ISR from a break that occurred before this irq() call.
             */
            Cy_SCB_ClearRxInterrupt(self->scb_obj->scb, CY_SCB_RX_INTR_UART_BREAK_DETECT);
            Cy_SCB_SetRxInterruptMask(self->scb_obj->scb, CY_SCB_RX_INTR_UART_BREAK_DETECT | CY_SCB_RX_INTR_NOT_EMPTY);
        }
        if (self->mp_irq_trigger & CY_SCB_TX_INTR_UART_DONE) {
            /**
             * Clear any stale sticky status before unmasking to prevent a
             * spurious ISR from a TX done that occurred before this irq() call.
             */
            Cy_SCB_ClearTxInterrupt(self->scb_obj->scb, CY_SCB_TX_INTR_UART_DONE);
            Cy_SCB_SetTxInterruptMask(self->scb_obj->scb, CY_SCB_TX_INTR_UART_DONE);
        }
    } else {
        /**
         * If disabling, we want to keep the CY_SCB_RX_INTR_NOT_EMPTY
         * enabled to be able to fill the RX ring buffer and avoid data loss.
         */
        Cy_SCB_SetRxInterruptMask(self->scb_obj->scb, CY_SCB_RX_INTR_NOT_EMPTY);
        Cy_SCB_SetTxInterruptMask(self->scb_obj->scb, 0);
    }
}

static mp_uint_t uart_irq_trigger(mp_obj_t self_in, mp_uint_t new_trigger) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    machine_uart_irq_scb_config(self, false);
    self->mp_irq_trigger = new_trigger;
    machine_uart_irq_scb_config(self, true);
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

static mp_irq_obj_t *mp_machine_uart_irq(machine_uart_obj_t *self, bool any_args, mp_arg_val_t *args) {
    if (self->mp_irq_obj == NULL) {
        self->mp_irq_trigger = 0;
        self->mp_irq_obj = mp_irq_new(&uart_irq_methods, MP_OBJ_FROM_PTR(self));
    }

    if (any_args) {
        /* Check the handler */
        mp_obj_t handler = args[MP_IRQ_ARG_INIT_handler].u_obj;
        if (handler != mp_const_none && !mp_obj_is_callable(handler)) {
            mp_raise_ValueError(MP_ERROR_TEXT("handler must be None or callable"));
        }

        /* Check the trigger */
        mp_uint_t trigger = args[MP_IRQ_ARG_INIT_trigger].u_int;
        mp_uint_t not_supported = trigger & ~MP_UART_ALLOWED_FLAGS;
        if (trigger != 0 && not_supported) {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("trigger 0x%08x unsupported"), not_supported);
        }

        /* Reconfigure user IRQs */
        machine_uart_irq_scb_config(self, false);
        self->mp_irq_obj->handler = handler;
        self->mp_irq_obj->ishard = args[MP_IRQ_ARG_INIT_hard].u_bool;
        self->mp_irq_trigger = trigger;
        machine_uart_irq_scb_config(self, true);
    }

    return self->mp_irq_obj;
}
#endif
