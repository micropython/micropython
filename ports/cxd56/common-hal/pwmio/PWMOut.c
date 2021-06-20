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

#include "py/runtime.h"

#include "shared-bindings/pwmio/PWMOut.h"

typedef struct {
    const char *devpath;
    const mcu_pin_obj_t *pin;
    int fd;
    bool reset;
} pwmout_dev_t;

STATIC pwmout_dev_t pwmout_dev[] = {
    {"/dev/pwm0", &pin_PWM0, -1, true},
    {"/dev/pwm1", &pin_PWM1, -1, true},
    {"/dev/pwm2", &pin_PWM2, -1, true},
    {"/dev/pwm3", &pin_PWM3, -1, true}
};

pwmout_result_t common_hal_pwmio_pwmout_construct(pwmio_pwmout_obj_t *self,
    const mcu_pin_obj_t *pin, uint16_t duty, uint32_t frequency,
    bool variable_frequency) {
    self->number = -1;

    for (int i = 0; i < MP_ARRAY_SIZE(pwmout_dev); i++) {
        if (pin->number == pwmout_dev[i].pin->number) {
            self->number = i;
            break;
        }
    }

    if (self->number < 0) {
        return PWMOUT_INVALID_PIN;
    }

    if (pwmout_dev[self->number].fd < 0) {
        pwmout_dev[self->number].fd = open(pwmout_dev[self->number].devpath, O_RDONLY);
        if (pwmout_dev[self->number].fd < 0) {
            return PWMOUT_INVALID_PIN;
        }
    }

    self->info.frequency = frequency;
    self->info.duty = duty;
    self->variable_frequency = variable_frequency;

    if (ioctl(pwmout_dev[self->number].fd, PWMIOC_SETCHARACTERISTICS, (unsigned long)((uintptr_t)&self->info)) < 0) {
        mp_raise_ValueError(translate("Invalid PWM frequency"));
    }
    ioctl(pwmout_dev[self->number].fd, PWMIOC_START, 0);

    claim_pin(pin);

    self->pin = pin;

    return PWMOUT_OK;
}

void common_hal_pwmio_pwmout_deinit(pwmio_pwmout_obj_t *self) {
    if (common_hal_pwmio_pwmout_deinited(self)) {
        return;
    }

    ioctl(pwmout_dev[self->number].fd, PWMIOC_STOP, 0);
    close(pwmout_dev[self->number].fd);
    pwmout_dev[self->number].fd = -1;

    reset_pin_number(self->pin->number);
    self->pin = NULL;
}

bool common_hal_pwmio_pwmout_deinited(pwmio_pwmout_obj_t *self) {
    return pwmout_dev[self->number].fd < 0;
}

void common_hal_pwmio_pwmout_set_duty_cycle(pwmio_pwmout_obj_t *self, uint16_t duty) {
    self->info.duty = duty;

    ioctl(pwmout_dev[self->number].fd, PWMIOC_SETCHARACTERISTICS, (unsigned long)((uintptr_t)&self->info));
}

uint16_t common_hal_pwmio_pwmout_get_duty_cycle(pwmio_pwmout_obj_t *self) {
    return self->info.duty;
}

void common_hal_pwmio_pwmout_set_frequency(pwmio_pwmout_obj_t *self, uint32_t frequency) {
    self->info.frequency = frequency;

    if (ioctl(pwmout_dev[self->number].fd, PWMIOC_SETCHARACTERISTICS, (unsigned long)((uintptr_t)&self->info)) < 0) {
        mp_raise_ValueError(translate("Invalid PWM frequency"));
    }
}

uint32_t common_hal_pwmio_pwmout_get_frequency(pwmio_pwmout_obj_t *self) {
    return self->info.frequency;
}

bool common_hal_pwmio_pwmout_get_variable_frequency(pwmio_pwmout_obj_t *self) {
    return self->variable_frequency;
}

void common_hal_pwmio_pwmout_never_reset(pwmio_pwmout_obj_t *self) {
    never_reset_pin_number(self->pin->number);

    pwmout_dev[self->number].reset = false;
}

void common_hal_pwmio_pwmout_reset_ok(pwmio_pwmout_obj_t *self) {
    pwmout_dev[self->number].reset = true;
}

void pwmout_reset(void) {
    for (int i = 0; i < MP_ARRAY_SIZE(pwmout_dev); i++) {
        if (pwmout_dev[i].fd >= 0 && pwmout_dev[i].reset) {
            ioctl(pwmout_dev[i].fd, PWMIOC_STOP, 0);
            close(pwmout_dev[i].fd);
            pwmout_dev[i].fd = -1;

            reset_pin_number(pwmout_dev[i].pin->number);
        }
    }
}

void pwmout_start(uint8_t pwm_num) {
    ioctl(pwmout_dev[pwm_num].fd, PWMIOC_START, 0);
}

void pwmout_stop(uint8_t pwm_num) {
    ioctl(pwmout_dev[pwm_num].fd, PWMIOC_STOP, 0);
}
