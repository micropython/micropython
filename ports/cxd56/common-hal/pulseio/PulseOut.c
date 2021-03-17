/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright 2019 Sony Semiconductor Solutions Corporation
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

#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <arch/chip/timer.h>

#include "py/runtime.h"

#include "shared-bindings/pulseio/PulseOut.h"

static uint16_t *pulse_buffer = NULL;
static uint16_t pulse_index = 0;
static uint16_t pulse_length;
static int pulse_fd = -1;

static bool pulseout_timer_handler(unsigned int *next_interval_us, void *arg) {
    uint8_t pwm_num = (uint8_t)(int)arg;
    pulse_index++;

    if (pulse_index >= pulse_length) {
        return false;
    }

    *next_interval_us = pulse_buffer[pulse_index] * 1000;

    if (pulse_index % 2 == 0) {
        pwmout_start(pwm_num);
    } else {
        pwmout_stop(pwm_num);
    }

    return true;
}

void common_hal_pulseio_pulseout_construct(pulseio_pulseout_obj_t *self,
    const pwmio_pwmout_obj_t *carrier,
    const mcu_pin_obj_t *pin,
    uint32_t frequency,
    uint16_t duty_cycle) {
    if (!carrier || pin || frequency) {
        mp_raise_NotImplementedError(translate("Port does not accept pins or frequency. Construct and pass a PWMOut Carrier instead"));
    }

    if (pulse_fd < 0) {
        pulse_fd = open("/dev/timer0", O_RDONLY);
    }

    if (pulse_fd < 0) {
        mp_raise_RuntimeError(translate("All timers in use"));
    }

    self->pwm_num = carrier->number;
}

void common_hal_pulseio_pulseout_deinit(pulseio_pulseout_obj_t *self) {
    if (common_hal_pulseio_pulseout_deinited(self)) {
        return;
    }

    ioctl(pulse_fd, TCIOC_STOP, 0);
    close(pulse_fd);
    pulse_fd = -1;

    pulse_buffer = NULL;
}

bool common_hal_pulseio_pulseout_deinited(pulseio_pulseout_obj_t *self) {
    return pulse_fd < 0;
}

void common_hal_pulseio_pulseout_send(pulseio_pulseout_obj_t *self, uint16_t *pulses, uint16_t len) {
    if (pulse_buffer != NULL) {
        mp_raise_RuntimeError(translate("Another send is already active"));
    }

    struct timer_sethandler_s sethandler;

    pulse_buffer = pulses;
    pulse_index = 0;
    pulse_length = len;

    unsigned long timeout = pulse_buffer[0] * 1000;

    ioctl(pulse_fd, TCIOC_SETTIMEOUT, timeout);

    sethandler.handler = pulseout_timer_handler;
    sethandler.arg = (void *)(int)self->pwm_num;

    ioctl(pulse_fd, TCIOC_SETHANDLER, (unsigned long)&sethandler);
    ioctl(pulse_fd, TCIOC_START, 0);

    while (pulse_index < len) {
        // Do other things while we wait. The interrupts will handle sending the
        // signal.
        RUN_BACKGROUND_TASKS;
    }

    pulse_buffer = NULL;
}

void pulseout_reset(void) {
    ioctl(pulse_fd, TCIOC_STOP, 0);
    close(pulse_fd);
    pulse_fd = -1;

    pulse_buffer = NULL;
}
