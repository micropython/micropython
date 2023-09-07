/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Scott Shawcroft for Adafruit Industries LLC
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

#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/busio/UART.h"

#include "components/driver/include/driver/uart.h"

#include "mpconfigport.h"
#include "shared/readline/readline.h"
#include "shared/runtime/interrupt_char.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "py/runtime.h"
#include "py/stream.h"
#include "supervisor/port.h"
#include "supervisor/shared/translate/translate.h"
#include "supervisor/shared/tick.h"

static uint8_t never_reset_uart_mask = 0;

static void uart_event_task(void *param) {
    busio_uart_obj_t *self = param;
    uart_event_t event;
    while (true) {
        if (xQueueReceive(self->event_queue, &event, portMAX_DELAY)) {
            switch (event.type) {
                case UART_BREAK:
                case UART_PATTERN_DET:
                    // When the console uart receives CTRL+C or BREAK, wake the main task and schedule a keyboard interrupt
                    if (self->is_console) {
                        port_wake_main_task();
                        if (mp_interrupt_char == CHAR_CTRL_C) {
                            uart_flush(self->uart_num);
                            mp_sched_keyboard_interrupt();
                        }
                    }
                    break;
                case UART_DATA:
                    // When the console uart receives any key, wake the main task
                    if (self->is_console) {
                        port_wake_main_task();
                    }
                    break;
                default:
                    break;
            }
        }
    }
}

void uart_reset(void) {
    for (uart_port_t num = 0; num < UART_NUM_MAX; num++) {
        #ifdef CONFIG_ESP_CONSOLE_UART_NUM
        // Do not reset the UART used by the IDF for logging.
        if (num == CONFIG_ESP_CONSOLE_UART_NUM) {
            continue;
        }
        #endif
        if (uart_is_driver_installed(num) && !(never_reset_uart_mask & (1 << num))) {
            uart_driver_delete(num);
        }
    }
}

void common_hal_busio_uart_never_reset(busio_uart_obj_t *self) {
    common_hal_never_reset_pin(self->rx_pin);
    common_hal_never_reset_pin(self->tx_pin);
    common_hal_never_reset_pin(self->rts_pin);
    common_hal_never_reset_pin(self->cts_pin);
    never_reset_uart_mask |= 1 << self->uart_num;
}

void common_hal_busio_uart_construct(busio_uart_obj_t *self,
    const mcu_pin_obj_t *tx, const mcu_pin_obj_t *rx,
    const mcu_pin_obj_t *rts, const mcu_pin_obj_t *cts,
    const mcu_pin_obj_t *rs485_dir, bool rs485_invert,
    uint32_t baudrate, uint8_t bits, busio_uart_parity_t parity, uint8_t stop,
    mp_float_t timeout, uint16_t receiver_buffer_size, byte *receiver_buffer,
    bool sigint_enabled) {

    mp_arg_validate_int_max(bits, 8, MP_QSTR_bytes);

    bool have_tx = tx != NULL;
    bool have_rx = rx != NULL;
    bool have_rts = rts != NULL;
    bool have_cts = cts != NULL;

    uart_config_t uart_config = {0};
    bool have_rs485_dir = rs485_dir != NULL;

    // shared-bindings checks that TX and RX are not both None, so we don't need to check here.

    // Filter for sane settings for RS485
    if (have_rs485_dir) {
        if (have_rts || have_cts) {
            mp_raise_ValueError(translate("Cannot specify RTS or CTS in RS485 mode"));
        }
    } else if (rs485_invert) {
        mp_raise_ValueError(translate("RS485 inversion specified when not in RS485 mode"));
    }

    self->timeout_ms = timeout * 1000;

    self->uart_num = UART_NUM_MAX;
    for (uart_port_t num = 0; num < UART_NUM_MAX; num++) {
        if (!uart_is_driver_installed(num)) {
            self->uart_num = num;
        }
    }
    if (self->uart_num == UART_NUM_MAX) {
        mp_raise_ValueError(translate("All UART peripherals are in use"));
    }

    uart_mode_t mode = UART_MODE_UART;
    uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
    if (have_rs485_dir) {
        mode = UART_MODE_RS485_HALF_DUPLEX;
        if (!rs485_invert) {
            // This one is not in the set
            uart_set_line_inverse(self->uart_num, UART_SIGNAL_DTR_INV);
        }
    } else if (have_rts && have_cts) {
        uart_config.flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS;
    } else if (have_rts) {
        uart_config.flow_ctrl = UART_HW_FLOWCTRL_RTS;
    } else if (have_rts) {
        uart_config.flow_ctrl = UART_HW_FLOWCTRL_CTS;
    }

    if (receiver_buffer_size <= UART_FIFO_LEN) {
        receiver_buffer_size = UART_FIFO_LEN + 8;
    }

    uart_config.rx_flow_ctrl_thresh = UART_FIFO_LEN - 8;
    // Install the driver before we change the settings.
    if (uart_driver_install(self->uart_num, receiver_buffer_size, 0, 20, &self->event_queue, 0) != ESP_OK ||
        uart_set_mode(self->uart_num, mode) != ESP_OK) {
        mp_raise_RuntimeError(translate("UART init"));
    }

    // On the console uart, enable pattern detection to look for CTRL+C
    #if CIRCUITPY_CONSOLE_UART
    if (rx == CIRCUITPY_CONSOLE_UART_RX) {
        self->is_console = true;
        uart_enable_pattern_det_baud_intr(self->uart_num, CHAR_CTRL_C, 1, 1, 0, 0);
    }
    #endif

    // Start a task to listen for uart events
    xTaskCreatePinnedToCore(
        uart_event_task,
        "uart_event_task",
        configMINIMAL_STACK_SIZE + 512,
        self,
        CONFIG_PTHREAD_TASK_PRIO_DEFAULT,
        &self->event_task,
        xPortGetCoreID());
    // uart_set_hw_flow_ctrl(self->uart_num, uart_config.flow_control, uart_config.rx_flow_ctrl_thresh);

    // Set baud rate
    // common_hal_busio_uart_set_baudrate(self, baudrate);
    uart_config.baud_rate = baudrate;

    uart_config.data_bits = UART_DATA_8_BITS;
    switch (bits) {
        // Shared bindings prevents data < 7 bits.
        // case 5:
        //     uart_config.data_bits = UART_DATA_5_BITS;
        //     break;
        // case 6:
        //     uart_config.data_bits = UART_DATA_6_BITS;
        //     break;
        case 7:
            uart_config.data_bits = UART_DATA_7_BITS;
            break;
        case 8:
            uart_config.data_bits = UART_DATA_8_BITS;
            break;
        default:
            // Won't hit this because shared-bindings limits to 7-9 bits. We error on 9 above.
            break;
    }
    // uart_set_word_length(self->uart_num, uart_config.data_bits);

    uart_config.parity = UART_PARITY_DISABLE;
    switch (parity) {
        case BUSIO_UART_PARITY_NONE:
            uart_config.parity = UART_PARITY_DISABLE;
            break;
        case BUSIO_UART_PARITY_EVEN:
            uart_config.parity = UART_PARITY_EVEN;
            break;
        case BUSIO_UART_PARITY_ODD:
            uart_config.parity = UART_PARITY_ODD;
            break;
        default:
            // Won't reach here because the input is an enum that is completely handled.
            break;
    }
    // uart_set_parity(self->uart_num, uart_config.parity);

    // Stop is 1 or 2 always.
    uart_config.stop_bits = UART_STOP_BITS_1;
    if (stop == 2) {
        uart_config.stop_bits = UART_STOP_BITS_2;
    }
    // uart_set_stop_bits(self->uart_num, stop_bits);
    uart_config.source_clk = UART_SCLK_APB; // guessing here...

    // config all in one?
    if (uart_param_config(self->uart_num, &uart_config) != ESP_OK) {
        mp_raise_RuntimeError(translate("UART init"));
    }

    self->tx_pin = NULL;
    self->rx_pin = NULL;
    self->rts_pin = NULL;
    self->cts_pin = NULL;
    int tx_num = -1;
    int rx_num = -1;
    int rts_num = -1;
    int cts_num = -1;
    if (have_tx) {
        claim_pin(tx);
        self->tx_pin = tx;
        tx_num = tx->number;
    }

    if (have_rx) {
        claim_pin(rx);
        self->rx_pin = rx;
        rx_num = rx->number;
    }

    if (have_rts) {
        claim_pin(rts);
        self->rts_pin = rts;
        rts_num = rts->number;
    }

    if (have_cts) {
        claim_pin(cts);
        self->cts_pin = cts;
        cts_num = cts->number;
    }

    if (have_rs485_dir) {
        claim_pin(rs485_dir);
        // RTS is used for RS485 direction.
        self->rts_pin = rs485_dir;
        rts_num = rs485_dir->number;
    }
    if (uart_set_pin(self->uart_num, tx_num, rx_num, rts_num, cts_num) != ESP_OK) {
        raise_ValueError_invalid_pins();
    }
}

bool common_hal_busio_uart_deinited(busio_uart_obj_t *self) {
    return self->rx_pin == NULL && self->tx_pin == NULL;
}

void common_hal_busio_uart_deinit(busio_uart_obj_t *self) {
    if (common_hal_busio_uart_deinited(self)) {
        return;
    }
    vTaskDelete(self->event_task);
    uart_driver_delete(self->uart_num);

    common_hal_reset_pin(self->rx_pin);
    common_hal_reset_pin(self->tx_pin);
    common_hal_reset_pin(self->rts_pin);
    common_hal_reset_pin(self->cts_pin);
    self->rx_pin = NULL;
    self->tx_pin = NULL;
    self->cts_pin = NULL;
    self->rts_pin = NULL;
}

// Read characters.
size_t common_hal_busio_uart_read(busio_uart_obj_t *self, uint8_t *data, size_t len, int *errcode) {
    if (self->rx_pin == NULL) {
        mp_raise_ValueError_varg(translate("No %q pin"), MP_QSTR_rx);
    }
    if (len == 0) {
        // Nothing to read.
        return 0;
    }

    size_t total_read = 0;
    uint64_t start_ticks = supervisor_ticks_ms64();

    // Busy-wait until timeout or until we've read enough chars.
    while (supervisor_ticks_ms64() - start_ticks <= self->timeout_ms) {
        // Read as many chars as we can right now, up to len.
        size_t num_read = uart_read_bytes(self->uart_num, data, len, 0);
        if (num_read < 0) {
            break;
        }

        // Advance pointer in data buffer, and decrease how many chars left to read.
        data += num_read;
        len -= num_read;
        total_read += num_read;
        if (len == 0) {
            // Don't need to read any more: data buf is full.
            break;
        }
        if (num_read > 0) {
            // Reset the timeout on every character read.
            start_ticks = supervisor_ticks_ms64();
        }
        RUN_BACKGROUND_TASKS;
        // Allow user to break out of a timeout with a KeyboardInterrupt.
        if (mp_hal_is_interrupted()) {
            break;
        }
        // If we are zero timeout, make sure we don't loop again (in the event
        // we read in under 1ms)
        if (self->timeout_ms == 0) {
            break;
        }
    }

    if (total_read == 0) {
        *errcode = EAGAIN;
        return MP_STREAM_ERROR;
    }

    return total_read;
}

// Write characters.
size_t common_hal_busio_uart_write(busio_uart_obj_t *self, const uint8_t *data, size_t len, int *errcode) {
    if (self->tx_pin == NULL) {
        mp_raise_ValueError_varg(translate("No %q pin"), MP_QSTR_tx);
    }

    size_t left_to_write = len;
    while (left_to_write > 0) {
        int count = uart_tx_chars(self->uart_num, (const char *)data, left_to_write);
        if (count < 0) {
            *errcode = MP_EAGAIN;
            return MP_STREAM_ERROR;
        }
        left_to_write -= count;
        data += count;
        RUN_BACKGROUND_TASKS;
    }
    while (uart_wait_tx_done(self->uart_num, 0) == ESP_ERR_TIMEOUT) {
        RUN_BACKGROUND_TASKS;
    }

    return len;
}

uint32_t common_hal_busio_uart_get_baudrate(busio_uart_obj_t *self) {
    uint32_t baudrate;
    uart_get_baudrate(self->uart_num, &baudrate);
    return baudrate;
}

void common_hal_busio_uart_set_baudrate(busio_uart_obj_t *self, uint32_t baudrate) {
    if (baudrate > UART_BITRATE_MAX ||
        uart_set_baudrate(self->uart_num, baudrate) != ESP_OK) {
        mp_arg_error_invalid(MP_QSTR_baudrate);
    }
}

mp_float_t common_hal_busio_uart_get_timeout(busio_uart_obj_t *self) {
    return (mp_float_t)(self->timeout_ms / 1000.0f);
}

void common_hal_busio_uart_set_timeout(busio_uart_obj_t *self, mp_float_t timeout) {
    self->timeout_ms = timeout * 1000;
}

uint32_t common_hal_busio_uart_rx_characters_available(busio_uart_obj_t *self) {
    size_t count;
    uart_get_buffered_data_len(self->uart_num, &count);
    return count;
}

void common_hal_busio_uart_clear_rx_buffer(busio_uart_obj_t *self) {
    uart_flush(self->uart_num);
}

// True if there are no characters still to be written.
bool common_hal_busio_uart_ready_to_tx(busio_uart_obj_t *self) {
    if (self->tx_pin == NULL) {
        return false;
    }
    return uart_wait_tx_done(self->uart_num, 0) != ESP_ERR_TIMEOUT;
}
