/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 microDev
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

#include "common-hal/ps2io/Ps2.h"

#include "supervisor/port.h"
#include "shared-bindings/microcontroller/__init__.h"

#define STATE_IDLE 0
#define STATE_RECV 1
#define STATE_RECV_PARITY 2
#define STATE_RECV_STOP 3
#define STATE_RECV_ERR 10

#define ERROR_STARTBIT 0x01
#define ERROR_TIMEOUT 0x02
#define ERROR_PARITY 0x04
#define ERROR_STOPBIT 0x08
#define ERROR_BUFFER 0x10

#define ERROR_TX_CLKLO 0x100
#define ERROR_TX_CLKHI 0x200
#define ERROR_TX_ACKDATA 0x400
#define ERROR_TX_ACKCLK 0x800
#define ERROR_TX_RTS 0x1000
#define ERROR_TX_NORESP 0x2000

void ps2_reset(void) {
    gpio_uninstall_isr_service();
}

static void delay_us(uint32_t t) {
    common_hal_mcu_delay_us(t);
}

/* interrupt handling */

static void IRAM_ATTR ps2_interrupt_handler(void *self_in) {
    // Grab the current time first.
    uint64_t current_tick = port_get_raw_ticks(NULL);

    ps2io_ps2_obj_t *self = self_in;
    int data_bit = gpio_get_level(self->data_pin) ? 1 : 0;

    // test for timeout
    if (self->state != STATE_IDLE) {
        int64_t diff_ms = current_tick - self->last_raw_ticks;
        if (diff_ms > 1) { // a.k.a. > 1.001ms
            self->last_errors |= ERROR_TIMEOUT;
            self->state = STATE_IDLE;
        }
    }

    self->last_raw_ticks = current_tick;

    if (self->state == STATE_IDLE) {
        self->bits = 0;
        self->parity = false;
        self->bitcount = 0;
        self->state = STATE_RECV;
        if (data_bit) {
            // start bit should be 0
            self->last_errors |= ERROR_STARTBIT;
            self->state = STATE_RECV_ERR;
        } else {
            self->state = STATE_RECV;
        }

    } else if (self->state == STATE_RECV) {
        if (data_bit) {
            self->bits |= data_bit << self->bitcount;
            self->parity = !self->parity;
        }
        ++self->bitcount;
        if (self->bitcount >= 8) {
            self->state = STATE_RECV_PARITY;
        }

    } else if (self->state == STATE_RECV_PARITY) {
        ++self->bitcount;
        if (data_bit) {
            self->parity = !self->parity;
        }
        if (!self->parity) {
            self->last_errors |= ERROR_PARITY;
            self->state = STATE_RECV_ERR;
        } else {
            self->state = STATE_RECV_STOP;
        }

    } else if (self->state == STATE_RECV_STOP) {
        ++self->bitcount;
        if (!data_bit) {
            self->last_errors |= ERROR_STOPBIT;
        } else if (self->waiting_cmd_response) {
            self->cmd_response = self->bits;
            self->waiting_cmd_response = false;
        } else if (self->bufcount >= sizeof(self->buffer)) {
            self->last_errors |= ERROR_BUFFER;
        } else {
            self->buffer[self->bufposw] = self->bits;
            self->bufposw = (self->bufposw + 1) % sizeof(self->buffer);
            self->bufcount++;
        }
        self->state = STATE_IDLE;

    } else if (self->state == STATE_RECV_ERR) {
        // just count the bits until idle
        if (++self->bitcount >= 10) {
            self->state = STATE_IDLE;
        }
    }
}

static void enable_interrupt(ps2io_ps2_obj_t *self) {
    // turn on falling edge interrupt
    gpio_install_isr_service(ESP_INTR_FLAG_IRAM);
    gpio_set_intr_type(self->clk_pin, GPIO_INTR_NEGEDGE);
    gpio_isr_handler_add(self->clk_pin, ps2_interrupt_handler, (void *)self);
}

static void disable_interrupt(ps2io_ps2_obj_t *self) {
    // turn off fallling edge interrupt
    gpio_isr_handler_remove(self->clk_pin);
}

static void resume_interrupt(ps2io_ps2_obj_t *self) {
    self->state = STATE_IDLE;
    gpio_isr_handler_add(self->clk_pin, ps2_interrupt_handler, (void *)self);
}

/* gpio handling */

static void clk_hi(ps2io_ps2_obj_t *self) {
    // external pull-up
    gpio_set_direction(self->clk_pin, GPIO_MODE_INPUT);
}

static bool wait_clk_lo(ps2io_ps2_obj_t *self, uint32_t us) {
    clk_hi(self);
    delay_us(1);
    while (gpio_get_level(self->clk_pin) && us) {
        --us;
        delay_us(1);
    }
    return us;
}

static bool wait_clk_hi(ps2io_ps2_obj_t *self, uint32_t us) {
    clk_hi(self);
    delay_us(1);
    while (!gpio_get_level(self->clk_pin) && us) {
        --us;
        delay_us(1);
    }
    return us;
}

static void clk_lo(ps2io_ps2_obj_t *self) {
    gpio_set_direction(self->clk_pin, GPIO_MODE_OUTPUT);
    gpio_set_level(self->clk_pin, 0);
}

static void data_hi(ps2io_ps2_obj_t *self) {
    // external pull-up
    gpio_set_direction(self->data_pin, GPIO_MODE_INPUT);
}

static bool wait_data_lo(ps2io_ps2_obj_t *self, uint32_t us) {
    data_hi(self);
    delay_us(1);
    while (gpio_get_level(self->data_pin) && us) {
        --us;
        delay_us(1);
    }
    return us;
}

static bool wait_data_hi(ps2io_ps2_obj_t *self, uint32_t us) {
    data_hi(self);
    delay_us(1);
    while (!gpio_get_level(self->data_pin) && us) {
        --us;
        delay_us(1);
    }
    return us;
}

static void data_lo(ps2io_ps2_obj_t *self) {
    gpio_set_direction(self->data_pin, GPIO_MODE_OUTPUT);
    gpio_set_level(self->data_pin, 0);
}

static void idle(ps2io_ps2_obj_t *self) {
    clk_hi(self);
    data_hi(self);
}

static void inhibit(ps2io_ps2_obj_t *self) {
    clk_lo(self);
    data_hi(self);
}

/* ps2io module functions */

void common_hal_ps2io_ps2_construct(ps2io_ps2_obj_t *self,
    const mcu_pin_obj_t *data_pin, const mcu_pin_obj_t *clk_pin) {
    self->clk_pin = (gpio_num_t)clk_pin->number;
    self->data_pin = (gpio_num_t)data_pin->number;
    self->state = STATE_IDLE;
    self->bufcount = 0;
    self->bufposr = 0;
    self->bufposw = 0;
    self->waiting_cmd_response = false;

    idle(self);
    enable_interrupt(self);

    claim_pin(clk_pin);
    claim_pin(data_pin);
}

bool common_hal_ps2io_ps2_deinited(ps2io_ps2_obj_t *self) {
    return self->clk_pin == GPIO_NUM_MAX;
}

void common_hal_ps2io_ps2_deinit(ps2io_ps2_obj_t *self) {
    if (common_hal_ps2io_ps2_deinited(self)) {
        return;
    }
    gpio_uninstall_isr_service();
    reset_pin_number(self->clk_pin);
    reset_pin_number(self->data_pin);
    self->clk_pin = GPIO_NUM_MAX;
    self->data_pin = GPIO_NUM_MAX;
}

uint16_t common_hal_ps2io_ps2_get_len(ps2io_ps2_obj_t *self) {
    return self->bufcount;
}

int16_t common_hal_ps2io_ps2_popleft(ps2io_ps2_obj_t *self) {
    common_hal_mcu_disable_interrupts();
    if (self->bufcount <= 0) {
        common_hal_mcu_enable_interrupts();
        return -1;
    }
    uint8_t b = self->buffer[self->bufposr];
    self->bufposr = (self->bufposr + 1) % sizeof(self->buffer);
    self->bufcount -= 1;
    common_hal_mcu_enable_interrupts();
    return b;
}

uint16_t common_hal_ps2io_ps2_clear_errors(ps2io_ps2_obj_t *self) {
    common_hal_mcu_disable_interrupts();
    uint16_t errors = self->last_errors;
    self->last_errors = 0;
    common_hal_mcu_enable_interrupts();
    return errors;
}

// Based upon TMK implementation of PS/2 protocol
// https://github.com/tmk/tmk_keyboard/blob/master/tmk_core/protocol/ps2_interrupt.c

int16_t common_hal_ps2io_ps2_sendcmd(ps2io_ps2_obj_t *self, uint8_t b) {
    disable_interrupt(self);

    /* terminate a transmission if we have */
    inhibit(self);
    delay_us(100);

    /* RTS and start bit */
    data_lo(self);
    clk_hi(self);
    if (!wait_clk_lo(self, 10000)) {
        self->last_errors |= ERROR_TX_RTS;
        goto ERROR;
    }

    bool parity = true;
    for (uint8_t i = 0; i < 8; i++) {
        delay_us(15);
        if (b & (1 << i)) {
            parity = !parity;
            data_hi(self);
        } else {
            data_lo(self);
        }
        if (!wait_clk_hi(self, 50)) {
            self->last_errors |= ERROR_TX_CLKHI;
            goto ERROR;
        }
        if (!wait_clk_lo(self, 50)) {
            self->last_errors |= ERROR_TX_CLKLO;
            goto ERROR;
        }
    }

    /* Parity bit */
    delay_us(15);
    if (parity) {
        data_hi(self);
    } else {
        data_lo(self);
    }
    if (!wait_clk_hi(self, 50)) {
        self->last_errors |= ERROR_TX_CLKHI;
        goto ERROR;
    }
    if (!wait_clk_lo(self, 50)) {
        self->last_errors |= ERROR_TX_CLKLO;
        goto ERROR;
    }

    /* Stop bit */
    delay_us(15);
    data_hi(self);

    /* Ack */
    if (!wait_data_lo(self, 50)) {
        self->last_errors |= ERROR_TX_ACKDATA;
        goto ERROR;
    }
    if (!wait_clk_lo(self, 50)) {
        self->last_errors |= ERROR_TX_ACKCLK;
        goto ERROR;
    }

    /* wait for idle state */
    if (!wait_clk_hi(self, 50)) {
        self->last_errors |= ERROR_TX_ACKCLK;
        goto ERROR;
    }
    if (!wait_data_hi(self, 50)) {
        self->last_errors |= ERROR_TX_ACKDATA;
        goto ERROR;
    }

    /* Wait for response byte */
    self->waiting_cmd_response = true;
    idle(self);
    resume_interrupt(self);

    for (int i = 0; i < 25; ++i) {
        delay_us(1000);
        common_hal_mcu_disable_interrupts();
        bool has_response = !self->waiting_cmd_response;
        uint8_t response = self->cmd_response;
        common_hal_mcu_enable_interrupts();

        if (has_response) {
            return response;
        }
    }

    /* No response */
    common_hal_mcu_disable_interrupts();
    self->waiting_cmd_response = false;
    self->last_errors |= ERROR_TX_NORESP;
    common_hal_mcu_enable_interrupts();
    return -1;

    /* Other errors */
ERROR:
    idle(self);
    resume_interrupt(self);
    return -1;
}
