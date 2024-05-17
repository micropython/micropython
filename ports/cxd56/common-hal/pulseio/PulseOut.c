// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright 2019 Sony Semiconductor Solutions Corporation
//
// SPDX-License-Identifier: MIT

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

static bool pulseout_timer_handler(uint32_t *next_interval_us, void *arg) {
    uint8_t pwm_num = (uint8_t)(int)arg;
    pulse_index++;

    if (pulse_index >= pulse_length) {
        return false;
    }

    *next_interval_us = pulse_buffer[pulse_index];

    if (pulse_index % 2 == 0) {
        pwmout_start(pwm_num);
    } else {
        pwmout_stop(pwm_num);
    }

    return true;
}

void common_hal_pulseio_pulseout_construct(pulseio_pulseout_obj_t *self,
    const mcu_pin_obj_t *pin,
    uint32_t frequency,
    uint16_t duty_cycle) {

    pwmout_result_t result = common_hal_pwmio_pwmout_construct(
        &self->pwmout, pin, duty_cycle, frequency, false);

    // This will raise an exception and not return if needed.
    common_hal_pwmio_pwmout_raise_error(result);

    if (pulse_fd < 0) {
        pulse_fd = open("/dev/timer0", O_RDONLY);
    }

    if (pulse_fd < 0) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("All timers in use"));
    }

    self->pwm_num = self->pwmout.number;
}

void common_hal_pulseio_pulseout_deinit(pulseio_pulseout_obj_t *self) {
    if (common_hal_pulseio_pulseout_deinited(self)) {
        return;
    }

    ioctl(pulse_fd, TCIOC_STOP, 0);
    close(pulse_fd);
    pulse_fd = -1;

    common_hal_pwmio_pwmout_deinit(&self->pwmout);

    pulse_buffer = NULL;
}

bool common_hal_pulseio_pulseout_deinited(pulseio_pulseout_obj_t *self) {
    return pulse_fd < 0;
}

void common_hal_pulseio_pulseout_send(pulseio_pulseout_obj_t *self, uint16_t *pulses, uint16_t len) {
    if (pulse_buffer != NULL) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Another send is already active"));
    }

    struct timer_sethandler_s sethandler;

    pulse_buffer = pulses;
    pulse_index = 0;
    pulse_length = len;

    unsigned long timeout = pulse_buffer[0];

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
