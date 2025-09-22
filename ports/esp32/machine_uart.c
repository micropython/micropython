/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2023 Damien P. George
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

#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_task.h"
#include "shared/runtime/mpirq.h"

#include "py/runtime.h"
#include "py/stream.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "uart.h"
#include "machine_timer.h"

#if SOC_UART_SUPPORT_XTAL_CLK
// Works independently of APB frequency, on ESP32C3, ESP32S3.
#define UART_SOURCE_CLK UART_SCLK_XTAL
#else
#define UART_SOURCE_CLK UART_SCLK_DEFAULT
#endif

#define UART_INV_TX UART_SIGNAL_TXD_INV
#define UART_INV_RX UART_SIGNAL_RXD_INV
#define UART_INV_RTS UART_SIGNAL_RTS_INV
#define UART_INV_CTS UART_SIGNAL_CTS_INV

#define UART_INV_MASK (UART_INV_TX | UART_INV_RX | UART_INV_RTS | UART_INV_CTS)
#define UART_IRQ_RX (1 << UART_DATA)
#define UART_IRQ_RXIDLE (0x1000)
#define UART_IRQ_BREAK (1 << UART_BREAK)
#define MP_UART_ALLOWED_FLAGS (UART_IRQ_RX | UART_IRQ_RXIDLE | UART_IRQ_BREAK)
#define RXIDLE_TIMER_MIN (machine_timer_freq_hz() * 5 / 10000) // 500us minimum rxidle time
#define UART_QUEUE_SIZE (3)

enum {
    RXIDLE_INACTIVE,
    RXIDLE_STANDBY,
    RXIDLE_ARMED,
    RXIDLE_ALERT,
};

// RXIDLE irq feature uses this machine.Timer id
#define RXIDLE_TIMER_IDX 0

typedef struct _machine_uart_obj_t {
    mp_obj_base_t base;
    uart_port_t uart_num;
    uart_hw_flowcontrol_t flowcontrol;
    uint8_t bits;
    uint8_t parity;
    uint8_t stop;
    gpio_num_t tx;
    gpio_num_t rx;
    gpio_num_t rts;
    gpio_num_t cts;
    uint16_t txbuf;
    uint16_t rxbuf;
    uint16_t timeout;       // timeout waiting for first char (in ms)
    uint16_t timeout_char;  // timeout waiting between chars (in ms)
    uint32_t invert;        // lines to invert
    TaskHandle_t uart_event_task;
    QueueHandle_t uart_queue;
    uint16_t mp_irq_trigger;   // user IRQ trigger mask
    uint16_t mp_irq_flags;     // user IRQ active IRQ flags
    mp_irq_obj_t *mp_irq_obj;  // user IRQ object
    machine_timer_obj_t *rxidle_timer;
    uint8_t rxidle_state;
    uint16_t rxidle_period;
} machine_uart_obj_t;

static const char *_parity_name[] = {"None", "1", "0"};

static bool uart_is_repl(uart_port_t uart_num) {
    #if MICROPY_HW_ENABLE_UART_REPL
    return uart_num == MICROPY_HW_UART_REPL;
    #else
    return false;
    #endif
}

/******************************************************************************/
// MicroPython bindings for UART

#define MICROPY_PY_MACHINE_UART_CLASS_CONSTANTS \
    { MP_ROM_QSTR(MP_QSTR_INV_TX), MP_ROM_INT(UART_INV_TX) }, \
    { MP_ROM_QSTR(MP_QSTR_INV_RX), MP_ROM_INT(UART_INV_RX) }, \
    { MP_ROM_QSTR(MP_QSTR_INV_RTS), MP_ROM_INT(UART_INV_RTS) }, \
    { MP_ROM_QSTR(MP_QSTR_INV_CTS), MP_ROM_INT(UART_INV_CTS) }, \
    { MP_ROM_QSTR(MP_QSTR_RTS), MP_ROM_INT(UART_HW_FLOWCTRL_RTS) }, \
    { MP_ROM_QSTR(MP_QSTR_CTS), MP_ROM_INT(UART_HW_FLOWCTRL_CTS) }, \
    { MP_ROM_QSTR(MP_QSTR_IRQ_RX), MP_ROM_INT(UART_IRQ_RX) }, \
    { MP_ROM_QSTR(MP_QSTR_IRQ_RXIDLE), MP_ROM_INT(UART_IRQ_RXIDLE) }, \
    { MP_ROM_QSTR(MP_QSTR_IRQ_BREAK), MP_ROM_INT(UART_IRQ_BREAK) }, \

static void uart_timer_callback(machine_timer_obj_t *timer) {
    // The UART object is referred here by the callback field.
    machine_uart_obj_t *self = (machine_uart_obj_t *)timer->callback;
    if (self->rxidle_state == RXIDLE_ALERT) {
        // At the first call, just switch the state
        self->rxidle_state = RXIDLE_ARMED;
    } else if (self->rxidle_state == RXIDLE_ARMED) {
        // At the second call, run the irq callback and stop the timer
        self->rxidle_state = RXIDLE_STANDBY;
        self->mp_irq_flags = UART_IRQ_RXIDLE;
        mp_irq_handler(self->mp_irq_obj);
        mp_hal_wake_main_task_from_isr();
        machine_timer_disable(self->rxidle_timer);
    }
}

static void uart_event_task(void *self_in) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uart_event_t event;
    for (;;) {
        // Waiting for an UART event.
        if (xQueueReceive(self->uart_queue, (void *)&event, (TickType_t)portMAX_DELAY)) {
            uint16_t mp_irq_flags = 0;
            switch (event.type) {
                // Event of UART receiving data
                case UART_DATA:
                    if (self->mp_irq_trigger & UART_IRQ_RXIDLE) {
                        if (self->rxidle_state != RXIDLE_INACTIVE) {
                            if (self->rxidle_state == RXIDLE_STANDBY) {
                                machine_timer_enable(self->rxidle_timer);
                            }
                        }
                        self->rxidle_state = RXIDLE_ALERT;
                    }
                    mp_irq_flags |= UART_IRQ_RX;
                    break;
                case UART_BREAK:
                    mp_irq_flags |= UART_IRQ_BREAK;
                    break;
                default:
                    break;
            }
            // Check the flags to see if the user handler should be called
            if (self->mp_irq_trigger & mp_irq_flags) {
                self->mp_irq_flags = mp_irq_flags;
                mp_irq_handler(self->mp_irq_obj);
                mp_hal_wake_main_task_from_isr();
            }
        }
    }
}

static inline void uart_event_task_create(machine_uart_obj_t *self) {
    if (xTaskCreatePinnedToCore(uart_event_task, "uart_event_task", 2048, self,
        ESP_TASKD_EVENT_PRIO, (TaskHandle_t *)&self->uart_event_task, MP_TASK_COREID) != pdPASS) {
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("failed to create UART event task"));
    }
}

static void mp_machine_uart_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint32_t baudrate;
    check_esp_err(uart_get_baudrate(self->uart_num, &baudrate));
    mp_printf(print, "UART(%u, baudrate=%u, bits=%u, parity=%s, stop=%u, tx=%d, rx=%d, rts=%d, cts=%d, txbuf=%u, rxbuf=%u, timeout=%u, timeout_char=%u, irq=%d",
        self->uart_num, baudrate, self->bits, _parity_name[self->parity],
        self->stop, self->tx, self->rx, self->rts, self->cts, self->txbuf, self->rxbuf, self->timeout, self->timeout_char, self->mp_irq_trigger);
    if (self->invert) {
        mp_printf(print, ", invert=");
        uint32_t invert_mask = self->invert;
        if (invert_mask & UART_INV_TX) {
            mp_printf(print, "INV_TX");
            invert_mask &= ~UART_INV_TX;
            if (invert_mask) {
                mp_printf(print, "|");
            }
        }
        if (invert_mask & UART_INV_RX) {
            mp_printf(print, "INV_RX");
            invert_mask &= ~UART_INV_RX;
            if (invert_mask) {
                mp_printf(print, "|");
            }
        }
        if (invert_mask & UART_INV_RTS) {
            mp_printf(print, "INV_RTS");
            invert_mask &= ~UART_INV_RTS;
            if (invert_mask) {
                mp_printf(print, "|");
            }
        }
        if (invert_mask & UART_INV_CTS) {
            mp_printf(print, "INV_CTS");
        }
    }
    if (self->flowcontrol) {
        mp_printf(print, ", flow=");
        uint32_t flow_mask = self->flowcontrol;
        if (flow_mask & UART_HW_FLOWCTRL_RTS) {
            mp_printf(print, "RTS");
            flow_mask &= ~UART_HW_FLOWCTRL_RTS;
            if (flow_mask) {
                mp_printf(print, "|");
            }
        }
        if (flow_mask & UART_HW_FLOWCTRL_CTS) {
            mp_printf(print, "CTS");
        }
    }
    mp_printf(print, ")");
}

static void mp_machine_uart_init_helper(machine_uart_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_baudrate, ARG_bits, ARG_parity, ARG_stop, ARG_tx, ARG_rx, ARG_rts, ARG_cts, ARG_txbuf, ARG_rxbuf, ARG_timeout, ARG_timeout_char, ARG_invert, ARG_flow };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_baudrate, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_bits, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_parity, MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_stop, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_tx, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_rx, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_rts, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_cts, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_txbuf, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_rxbuf, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_timeout_char, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_invert, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_flow, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // If UART is being freshly initialised then restore object defaults
    if (!uart_is_driver_installed(self->uart_num)) {
        self->bits = 8;
        self->parity = 0;
        self->stop = 1;
        self->rts = UART_PIN_NO_CHANGE;
        self->cts = UART_PIN_NO_CHANGE;
        self->txbuf = 256;
        self->rxbuf = 256; // IDF minimum
        self->timeout = 0;
        self->timeout_char = 0;
        self->invert = 0;
        self->flowcontrol = 0;
        self->uart_event_task = NULL;
        self->uart_queue = NULL;
        self->rxidle_state = RXIDLE_INACTIVE;

        // Set the MicroPython default UART pins. These may be overwritten with
        // caller-provided pins, below
        switch (self->uart_num) {
            case UART_NUM_0:
                self->rx = UART_PIN_NO_CHANGE; // GPIO 3
                self->tx = UART_PIN_NO_CHANGE; // GPIO 1
                break;
            case UART_NUM_1:
                self->rx = 9;
                self->tx = 10;
                break;
            #if SOC_UART_HP_NUM > 2
            case UART_NUM_2:
                self->rx = 16;
                self->tx = 17;
                break;
            #endif
            #if SOC_UART_LP_NUM >= 1
            case LP_UART_NUM_0:
                self->rx = 4;
                self->tx = 5;
                break;
            #endif
            case UART_NUM_MAX:
                assert(0); // Range is checked in mp_machine_uart_make_new, value should be unreachable
        }
    } else {
        // wait for all data to be transmitted before changing settings
        uart_wait_tx_done(self->uart_num, pdMS_TO_TICKS(1000));
    }

    // Default driver parameters, should correspond to values set above
    uart_config_t uartcfg = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 0,
        .source_clk = UART_SOURCE_CLK,
    };


    if ((args[ARG_txbuf].u_int >= 0 && args[ARG_txbuf].u_int != self->txbuf) || (args[ARG_rxbuf].u_int >= 0 && args[ARG_rxbuf].u_int != self->rxbuf)) {
        if (uart_is_repl(self->uart_num)) {
            mp_raise_ValueError(MP_ERROR_TEXT("REPL UART buffer size is fixed"));
        }

        if (args[ARG_txbuf].u_int >= 0) {
            self->txbuf = args[ARG_txbuf].u_int;
        }
        if (args[ARG_rxbuf].u_int >= 0) {
            self->rxbuf = args[ARG_rxbuf].u_int;
        }

        // must reinitialise driver to change the tx/rx buffer size
        if (uart_is_driver_installed(self->uart_num)) {
            // Update uartcfg with the current uart parameters
            uint32_t baudrate;
            check_esp_err(uart_get_baudrate(self->uart_num, &baudrate));
            uartcfg.baud_rate = baudrate;
            check_esp_err(uart_get_word_length(self->uart_num, &uartcfg.data_bits));
            check_esp_err(uart_get_parity(self->uart_num, &uartcfg.parity));
            check_esp_err(uart_get_stop_bits(self->uart_num, &uartcfg.stop_bits));
            // De-initialise the driver
            mp_machine_uart_deinit(self);
        }
    }

    if (!uart_is_repl(self->uart_num) && !uart_is_driver_installed(self->uart_num)) {
        // install the driver if needed - could be uninstalled for multiple reasons:
        // 1. First time initialising this UART
        // 2. Has been temporarily de-initialised to change the buffer size
        // 3. Python code called .deinit() and now .init()
        check_esp_err(uart_param_config(self->uart_num, &uartcfg));
        check_esp_err(uart_driver_install(self->uart_num, self->rxbuf, self->txbuf, UART_QUEUE_SIZE, &self->uart_queue, 0));
        if (self->mp_irq_obj != NULL && self->mp_irq_obj->handler != mp_const_none) {
            uart_event_task_create(self);
        }
    }

    // set baudrate
    uint32_t baudrate = 115200;
    if (args[ARG_baudrate].u_int > 0) {
        check_esp_err(uart_set_baudrate(self->uart_num, args[ARG_baudrate].u_int));
    }
    check_esp_err(uart_get_baudrate(self->uart_num, &baudrate));

    if (args[ARG_tx].u_obj != MP_OBJ_NULL) {
        self->tx = machine_pin_get_id(args[ARG_tx].u_obj);
    }

    if (args[ARG_rx].u_obj != MP_OBJ_NULL) {
        self->rx = machine_pin_get_id(args[ARG_rx].u_obj);
    }

    if (args[ARG_rts].u_obj != MP_OBJ_NULL) {
        self->rts = machine_pin_get_id(args[ARG_rts].u_obj);
    }

    if (args[ARG_cts].u_obj != MP_OBJ_NULL) {
        self->cts = machine_pin_get_id(args[ARG_cts].u_obj);
    }
    check_esp_err(uart_set_pin(self->uart_num, self->tx, self->rx, self->rts, self->cts));

    // set data bits
    switch (args[ARG_bits].u_int) {
        case 0:
            break;
        case 5:
            check_esp_err(uart_set_word_length(self->uart_num, UART_DATA_5_BITS));
            self->bits = 5;
            break;
        case 6:
            check_esp_err(uart_set_word_length(self->uart_num, UART_DATA_6_BITS));
            self->bits = 6;
            break;
        case 7:
            check_esp_err(uart_set_word_length(self->uart_num, UART_DATA_7_BITS));
            self->bits = 7;
            break;
        case 8:
            check_esp_err(uart_set_word_length(self->uart_num, UART_DATA_8_BITS));
            self->bits = 8;
            break;
        default:
            mp_raise_ValueError(MP_ERROR_TEXT("invalid data bits"));
            break;
    }

    // set parity
    if (args[ARG_parity].u_obj != MP_OBJ_NULL) {
        if (args[ARG_parity].u_obj == mp_const_none) {
            check_esp_err(uart_set_parity(self->uart_num, UART_PARITY_DISABLE));
            self->parity = 0;
        } else {
            mp_int_t parity = mp_obj_get_int(args[ARG_parity].u_obj);
            if (parity & 1) {
                check_esp_err(uart_set_parity(self->uart_num, UART_PARITY_ODD));
                self->parity = 1;
            } else {
                check_esp_err(uart_set_parity(self->uart_num, UART_PARITY_EVEN));
                self->parity = 2;
            }
        }
    }

    // set stop bits
    switch (args[ARG_stop].u_int) {
        // FIXME: ESP32 also supports 1.5 stop bits
        case 0:
            break;
        case 1:
            check_esp_err(uart_set_stop_bits(self->uart_num, UART_STOP_BITS_1));
            self->stop = 1;
            break;
        case 2:
            check_esp_err(uart_set_stop_bits(self->uart_num, UART_STOP_BITS_2));
            self->stop = 2;
            break;
        default:
            mp_raise_ValueError(MP_ERROR_TEXT("invalid stop bits"));
            break;
    }

    // set timeout
    if (args[ARG_timeout].u_int != -1) {
        self->timeout = args[ARG_timeout].u_int;
    }

    // set timeout_char
    if (args[ARG_timeout_char].u_int != -1) {
        self->timeout_char = args[ARG_timeout_char].u_int;
    }
    // make sure it is at least as long as a whole character (12 bits here)
    uint32_t char_time_ms = 12000 / baudrate + 1;
    uint32_t rx_timeout = self->timeout_char / char_time_ms;
    if (rx_timeout < 1) {
        check_esp_err(uart_set_rx_full_threshold(self->uart_num, 1));
        check_esp_err(uart_set_rx_timeout(self->uart_num, 1));
    } else {
        check_esp_err(uart_set_rx_timeout(self->uart_num, rx_timeout));
    }

    // set line inversion
    if (args[ARG_invert].u_int != -1) {
        if (args[ARG_invert].u_int & ~UART_INV_MASK) {
            mp_raise_ValueError(MP_ERROR_TEXT("invalid inversion mask"));
        }
        self->invert = args[ARG_invert].u_int;
    }
    check_esp_err(uart_set_line_inverse(self->uart_num, self->invert));

    // set hardware flow control
    if (args[ARG_flow].u_int != -1) {
        if (args[ARG_flow].u_int & ~UART_HW_FLOWCTRL_CTS_RTS) {
            mp_raise_ValueError(MP_ERROR_TEXT("invalid flow control mask"));
        }
        self->flowcontrol = args[ARG_flow].u_int;
    }
    uint8_t uart_fifo_len = UART_HW_FIFO_LEN(self->uart_num);
    check_esp_err(uart_set_hw_flow_ctrl(self->uart_num, self->flowcontrol, uart_fifo_len - uart_fifo_len / 4));

    // discard any input from previous configuration
    check_esp_err(uart_flush_input(self->uart_num));
}

static mp_obj_t mp_machine_uart_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // get uart id
    mp_int_t uart_num = mp_obj_get_int(args[0]);
    if (uart_num < 0 || uart_num >= UART_NUM_MAX) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("UART(%d) does not exist"), uart_num);
    }
    _Static_assert(UART_NUM_MAX == SOC_UART_NUM, "SOC and driver constant should match");

    machine_uart_obj_t *self = MP_STATE_PORT(machine_uart_objs)[uart_num];
    if (self == NULL) {
        // Allocate a new UART object
        self = mp_obj_malloc(machine_uart_obj_t, &machine_uart_type);
        self->uart_num = uart_num;
        // remaining fields are set from inside mp_machine_uart_init_helper()
    }

    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    mp_machine_uart_init_helper(self, n_args - 1, args + 1, &kw_args);

    // Make sure pins are connected.
    check_esp_err(uart_set_pin(self->uart_num, self->tx, self->rx, self->rts, self->cts));

    MP_STATE_PORT(machine_uart_objs)[uart_num] = self;

    return MP_OBJ_FROM_PTR(self);
}

static void mp_machine_uart_deinit(machine_uart_obj_t *self) {
    if (self->uart_event_task != NULL) {
        vTaskDelete(self->uart_event_task);
        self->uart_event_task = NULL;
    }
    if (self->rxidle_timer != NULL) {
        machine_timer_disable(self->rxidle_timer);
        if (self->rxidle_state > RXIDLE_STANDBY) {
            // Currently deinit(),init() sequence resumes any previously
            // configured irqs, and we currently also rely on this when changing
            // buffer size - so keep rxidle in standby if it was active
            self->rxidle_state = RXIDLE_STANDBY;
        }
    }
    // Only stop the ESP-IDF driver entirely if it's not the REPL,
    // (if it's the REPL then it shouldn't have any ISR configured anyway.)
    if (!uart_is_repl(self->uart_num)) {
        check_esp_err(uart_driver_delete(self->uart_num));
        self->uart_queue = NULL;
    }
}

void machine_uart_deinit_all(void) {
    for (int i = 0; i < UART_NUM_MAX; i++) {
        machine_uart_obj_t *uart = MP_STATE_PORT(machine_uart_objs)[i];
        if (uart) {
            mp_machine_uart_deinit(uart);
            MP_STATE_PORT(machine_uart_objs)[i] = NULL;
        }
    }
}

static mp_int_t mp_machine_uart_any(machine_uart_obj_t *self) {
    size_t rxbufsize;
    check_esp_err(uart_get_buffered_data_len(self->uart_num, &rxbufsize));
    return rxbufsize;
}

static bool mp_machine_uart_txdone(machine_uart_obj_t *self) {
    return uart_wait_tx_done(self->uart_num, 0) == ESP_OK;
}

static void mp_machine_uart_sendbreak(machine_uart_obj_t *self) {
    // Calculate the length of the break, as 13 bits.
    uint32_t baudrate;
    check_esp_err(uart_get_baudrate(self->uart_num, &baudrate));
    uint32_t break_delay_us = 13000000 / baudrate;

    // Wait for any outstanding data to be transmitted.
    check_esp_err(uart_wait_tx_done(self->uart_num, pdMS_TO_TICKS(1000)));

    // Set the TX pin to output, pull it low, and wait for the break period.
    mp_hal_pin_output(self->tx);
    mp_hal_pin_write(self->tx, 0);
    mp_hal_delay_us(break_delay_us);

    // Restore original UART pin settings.
    check_esp_err(uart_set_pin(self->uart_num, self->tx, self->rx, self->rts, self->cts));
}

// Configure the timer used for IRQ_RXIDLE
static void uart_irq_configure_timer(machine_uart_obj_t *self, mp_uint_t trigger) {

    self->rxidle_state = RXIDLE_INACTIVE;

    if (trigger & UART_IRQ_RXIDLE) {
        // The RXIDLE event is always a soft IRQ.
        self->mp_irq_obj->ishard = false;
        uint32_t baudrate;
        uart_get_baudrate(self->uart_num, &baudrate);
        mp_int_t period = machine_timer_freq_hz() * 20 / baudrate + 1;
        if (period < RXIDLE_TIMER_MIN) {
            period = RXIDLE_TIMER_MIN;
        }
        self->rxidle_period = period;
        self->rxidle_timer->period = period;
        self->rxidle_timer->handler = uart_timer_callback;
        // The Python callback is not used. So use this
        // data field to hold a reference to the UART object.
        self->rxidle_timer->callback = self;
        self->rxidle_timer->repeat = true;
        self->rxidle_state = RXIDLE_STANDBY;
    }
}

static mp_uint_t uart_irq_trigger(mp_obj_t self_in, mp_uint_t new_trigger) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);

    uart_irq_configure_timer(self, new_trigger);
    self->mp_irq_trigger = new_trigger;
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

static const mp_irq_methods_t uart_irq_methods = {
    .trigger = uart_irq_trigger,
    .info = uart_irq_info,
};

static mp_irq_obj_t *mp_machine_uart_irq(machine_uart_obj_t *self, bool any_args, mp_arg_val_t *args) {
    if (uart_is_repl(self->uart_num)) {
        mp_raise_ValueError(MP_ERROR_TEXT("REPL UART does not support IRQs"));
    }

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

        // Check the trigger
        mp_uint_t trigger = args[MP_IRQ_ARG_INIT_trigger].u_int;
        mp_uint_t not_supported = trigger & ~MP_UART_ALLOWED_FLAGS;
        if (trigger != 0 && not_supported) {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("trigger 0x%04x unsupported"), not_supported);
        }

        self->mp_irq_obj->handler = handler;
        if (args[MP_IRQ_ARG_INIT_hard].u_bool) {
            mp_raise_ValueError(MP_ERROR_TEXT("hard IRQ is not supported"));
        }
        self->mp_irq_obj->ishard = false;
        self->mp_irq_trigger = trigger;
        self->rxidle_timer = machine_timer_create(RXIDLE_TIMER_IDX);
        uart_irq_configure_timer(self, trigger);

        // Start a task for handling events
        if (handler != mp_const_none && self->uart_event_task == NULL && self->uart_queue != NULL) {
            uart_event_task_create(self);
        } else if (handler == mp_const_none && self->uart_event_task != NULL) {
            vTaskDelete(self->uart_event_task);
            self->uart_event_task = NULL;
        }
    }

    return self->mp_irq_obj;
}

static mp_uint_t mp_machine_uart_read(mp_obj_t self_in, void *buf_in, mp_uint_t size, int *errcode) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);

    // make sure we want at least 1 char
    if (size == 0) {
        return 0;
    }

    TickType_t time_to_wait = self->timeout > 0 ? pdMS_TO_TICKS(self->timeout) : 0;

    bool release_gil = (self->timeout + self->timeout_char) > 0;
    if (release_gil) {
        MP_THREAD_GIL_EXIT();
    }

    int bytes_read = uart_read_bytes(self->uart_num, buf_in, 1, time_to_wait);
    if (size > 1 && bytes_read != 0) {
        time_to_wait = self->timeout_char > 0 ? pdMS_TO_TICKS(self->timeout_char) : 0;
        bytes_read += uart_read_bytes(self->uart_num, buf_in + 1, size - 1, time_to_wait);
    }

    if (release_gil) {
        MP_THREAD_GIL_ENTER();
    }

    if (bytes_read <= 0) {
        *errcode = MP_EAGAIN;
        return MP_STREAM_ERROR;
    }

    return bytes_read;
}

static mp_uint_t mp_machine_uart_write(mp_obj_t self_in, const void *buf_in, mp_uint_t size, int *errcode) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);

    int bytes_written = uart_write_bytes(self->uart_num, buf_in, size);

    if (bytes_written < 0) {
        *errcode = MP_EAGAIN;
        return MP_STREAM_ERROR;
    }

    // return number of bytes written
    return bytes_written;
}

static mp_uint_t mp_machine_uart_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    machine_uart_obj_t *self = self_in;
    mp_uint_t ret;
    if (request == MP_STREAM_POLL) {
        mp_uint_t flags = arg;
        ret = 0;
        size_t rxbufsize;
        check_esp_err(uart_get_buffered_data_len(self->uart_num, &rxbufsize));
        if ((flags & MP_STREAM_POLL_RD) && rxbufsize > 0) {
            ret |= MP_STREAM_POLL_RD;
        }
        if ((flags & MP_STREAM_POLL_WR) && 1) { // FIXME: uart_tx_any_room(self->uart_num)
            ret |= MP_STREAM_POLL_WR;
        }
    } else if (request == MP_STREAM_FLUSH) {
        // The timeout is estimated using the buffer size and the baudrate.
        // Take the worst case assumptions at 13 bit symbol size times 2.
        uint32_t baudrate;
        check_esp_err(uart_get_baudrate(self->uart_num, &baudrate));
        uint32_t timeout = (3 + self->txbuf) * 13000 * 2 / baudrate;
        if (uart_wait_tx_done(self->uart_num, timeout) == ESP_OK) {
            ret = 0;
        } else {
            *errcode = MP_ETIMEDOUT;
            ret = MP_STREAM_ERROR;
        }
    } else {
        *errcode = MP_EINVAL;
        ret = MP_STREAM_ERROR;
    }
    return ret;
}

MP_REGISTER_ROOT_POINTER(void *machine_uart_objs[SOC_UART_NUM]);
