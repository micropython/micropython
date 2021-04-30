/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017-2018 Scott Shawcroft for Adafruit Industries
 * Copyright (c) 2019 Elvis Pfutzenreuter <epxx@epxx.co>
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

#include <stdint.h>

#include "atmel_start_pins.h"
#include "hal/include/hal_gpio.h"

#include "background.h"
#include "eic_handler.h"
#include "mpconfigport.h"
#include "py/gc.h"
#include "py/runtime.h"
#include "samd/external_interrupts.h"
#include "samd/pins.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/ps2io/Ps2.h"
#include "supervisor/port.h"
#include "supervisor/shared/translate.h"

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

static void ps2_set_config(ps2io_ps2_obj_t *self) {
    uint32_t sense_setting = EIC_CONFIG_SENSE0_FALL_Val;
    set_eic_handler(self->channel, EIC_HANDLER_PS2);
    turn_on_eic_channel(self->channel, sense_setting);
}

static void disable_interrupt(ps2io_ps2_obj_t *self) {
    uint32_t mask = 1 << self->channel;
    EIC->INTENCLR.reg = mask << EIC_INTENSET_EXTINT_Pos;
}

static void resume_interrupt(ps2io_ps2_obj_t *self) {
    disable_interrupt(self);

    self->state = STATE_IDLE;
    gpio_set_pin_function(self->clk_pin, GPIO_PIN_FUNCTION_A);
    uint32_t mask = 1 << self->channel;
    EIC->INTFLAG.reg = mask << EIC_INTFLAG_EXTINT_Pos;
    EIC->INTENSET.reg = mask << EIC_INTENSET_EXTINT_Pos;

    ps2_set_config(self);
}

static void clk_hi(ps2io_ps2_obj_t *self) {
    // External pull-up
    // Must set pull after setting direction.
    gpio_set_pin_direction(self->clk_pin, GPIO_DIRECTION_IN);
    gpio_set_pin_pull_mode(self->clk_pin, GPIO_PULL_OFF);
}

static bool wait_clk_lo(ps2io_ps2_obj_t *self, uint32_t us) {
    clk_hi(self);
    common_hal_mcu_delay_us(1);
    while (gpio_get_pin_level(self->clk_pin) && us) {
        --us;
        common_hal_mcu_delay_us(1);
    }
    return us;
}

static bool wait_clk_hi(ps2io_ps2_obj_t *self, uint32_t us) {
    clk_hi(self);
    common_hal_mcu_delay_us(1);
    while (!gpio_get_pin_level(self->clk_pin) && us) {
        --us;
        common_hal_mcu_delay_us(1);
    }
    return us;
}

static void clk_lo(ps2io_ps2_obj_t *self) {
    gpio_set_pin_pull_mode(self->clk_pin, GPIO_PULL_OFF);
    gpio_set_pin_direction(self->clk_pin, GPIO_DIRECTION_OUT);
    gpio_set_pin_level(self->clk_pin, 0);
}

static void data_hi(ps2io_ps2_obj_t *self) {
    // External pull-up
    gpio_set_pin_direction(self->data_pin, GPIO_DIRECTION_IN);
    gpio_set_pin_pull_mode(self->data_pin, GPIO_PULL_OFF);
}

static bool wait_data_lo(ps2io_ps2_obj_t *self, uint32_t us) {
    data_hi(self);
    common_hal_mcu_delay_us(1);
    while (gpio_get_pin_level(self->data_pin) && us) {
        --us;
        common_hal_mcu_delay_us(1);
    }
    return us;
}

static bool wait_data_hi(ps2io_ps2_obj_t *self, uint32_t us) {
    data_hi(self);
    common_hal_mcu_delay_us(1);
    while (!gpio_get_pin_level(self->data_pin) && us) {
        --us;
        common_hal_mcu_delay_us(1);
    }
    return us;
}

static void data_lo(ps2io_ps2_obj_t *self) {
    gpio_set_pin_pull_mode(self->data_pin, GPIO_PULL_OFF);
    gpio_set_pin_direction(self->data_pin, GPIO_DIRECTION_OUT);
    gpio_set_pin_level(self->data_pin, 0);
}

static void idle(ps2io_ps2_obj_t *self) {
    clk_hi(self);
    data_hi(self);
}

static void inhibit(ps2io_ps2_obj_t *self) {
    clk_lo(self);
    data_hi(self);
}

static void delay_us(uint32_t t) {
    common_hal_mcu_delay_us(t);
}

void ps2_interrupt_handler(uint8_t channel) {
    // Grab the current time first.
    uint64_t current_tick = port_get_raw_ticks(NULL);

    ps2io_ps2_obj_t *self = get_eic_channel_data(channel);
    int data_bit = gpio_get_pin_level(self->data_pin) ? 1 : 0;

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

void common_hal_ps2io_ps2_construct(ps2io_ps2_obj_t *self,
    const mcu_pin_obj_t *data_pin, const mcu_pin_obj_t *clk_pin) {
    if (!clk_pin->has_extint) {
        mp_raise_RuntimeError(translate("No hardware support on clk pin"));
    }
    if (eic_get_enable() && !eic_channel_free(clk_pin->extint_channel)) {
        mp_raise_RuntimeError(translate("EXTINT channel already in use"));
    }

    clk_hi(self);
    data_hi(self);

    self->channel = clk_pin->extint_channel;
    self->clk_pin = clk_pin->number;
    self->data_pin = data_pin->number;
    self->state = STATE_IDLE;
    self->bufcount = 0;
    self->bufposr = 0;
    self->bufposw = 0;
    self->waiting_cmd_response = false;

    set_eic_channel_data(clk_pin->extint_channel, (void *)self);

    // Check to see if the EIC is enabled and start it up if its not.'
    if (eic_get_enable() == 0) {
        turn_on_external_interrupt_controller();
    }

    gpio_set_pin_function(clk_pin->number, GPIO_PIN_FUNCTION_A);
    gpio_set_pin_function(data_pin->number, GPIO_PIN_FUNCTION_A);

    turn_on_cpu_interrupt(self->channel);

    claim_pin(clk_pin);
    claim_pin(data_pin);

    // Set config will enable the EIC.
    ps2_set_config(self);
}

bool common_hal_ps2io_ps2_deinited(ps2io_ps2_obj_t *self) {
    return self->clk_pin == NO_PIN;
}

void common_hal_ps2io_ps2_deinit(ps2io_ps2_obj_t *self) {
    if (common_hal_ps2io_ps2_deinited(self)) {
        return;
    }
    set_eic_handler(self->channel, EIC_HANDLER_NO_INTERRUPT);
    turn_off_eic_channel(self->channel);
    reset_pin_number(self->clk_pin);
    reset_pin_number(self->data_pin);
    self->clk_pin = NO_PIN;
    self->data_pin = NO_PIN;
}

uint16_t common_hal_ps2io_ps2_get_len(ps2io_ps2_obj_t *self) {
    return self->bufcount;
}

bool common_hal_ps2io_ps2_get_paused(ps2io_ps2_obj_t *self) {
    uint32_t mask = 1 << self->channel;
    return (EIC->INTENSET.reg & (mask << EIC_INTENSET_EXTINT_Pos)) == 0;
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
