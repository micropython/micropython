// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright 2019 Sony Semiconductor Solutions Corporation
//
// SPDX-License-Identifier: MIT

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include <arch/chip/scu.h>
#include <arch/chip/adc.h>

#include "py/runtime.h"

#include "shared-bindings/analogio/AnalogIn.h"
#include "shared-bindings/microcontroller/Pin.h"

typedef struct {
    const char *devpath;
    const mcu_pin_obj_t *pin;
    int fd;
} analogin_dev_t;

static analogin_dev_t analogin_dev[] = {
    {"/dev/lpadc0", &pin_LPADC0, -1},
    {"/dev/lpadc1", &pin_LPADC1, -1},
    {"/dev/lpadc2", &pin_LPADC2, -1},
    {"/dev/lpadc3", &pin_LPADC3, -1},
    {"/dev/hpadc0", &pin_HPADC0, -1},
    {"/dev/hpadc1", &pin_HPADC1, -1},
};

void common_hal_analogio_analogin_construct(analogio_analogin_obj_t *self, const mcu_pin_obj_t *pin) {
    if (!pin->analog) {
        raise_ValueError_invalid_pin();
    }

    self->number = -1;

    for (int i = 0; i < MP_ARRAY_SIZE(analogin_dev); i++) {
        if (pin->number == analogin_dev[i].pin->number) {
            self->number = i;
            break;
        }
    }

    if (self->number < 0) {
        raise_ValueError_invalid_pin();
    }

    if (analogin_dev[self->number].fd < 0) {
        analogin_dev[self->number].fd = open(analogin_dev[self->number].devpath, O_RDONLY);
        if (analogin_dev[self->number].fd < 0) {
            raise_ValueError_invalid_pin();
        }
    }

    // SCU FIFO overwrite
    ioctl(analogin_dev[self->number].fd, SCUIOC_SETFIFOMODE, 1);

    // ADC FIFO size
    ioctl(analogin_dev[self->number].fd, ANIOC_CXD56_FIFOSIZE, 2);

    // start ADC
    ioctl(analogin_dev[self->number].fd, ANIOC_CXD56_START, 0);

    self->pin = pin;
}

void common_hal_analogio_analogin_deinit(analogio_analogin_obj_t *self) {
    if (common_hal_analogio_analogin_deinited(self)) {
        return;
    }

    // stop ADC
    ioctl(analogin_dev[self->number].fd, ANIOC_CXD56_STOP, 0);
    close(analogin_dev[self->number].fd);
    analogin_dev[self->number].fd = -1;

    self->pin = NULL;
}

bool common_hal_analogio_analogin_deinited(analogio_analogin_obj_t *self) {
    return analogin_dev[self->number].fd < 0;
}

uint16_t common_hal_analogio_analogin_get_value(analogio_analogin_obj_t *self) {
    int16_t value = 0;

    read(analogin_dev[self->number].fd, &value, sizeof(value));

    return (uint16_t)32768 + (uint16_t)value;
}

// Reference voltage is a fixed value which is depending on the board.
// e.g.)
// - Reference Voltage of A2 and A3 pins on Main Board is 0.7V.
// - Reference Voltage of A0 ~ A5 pins on External Interface board is 5.0V
float common_hal_analogio_analogin_get_reference_voltage(analogio_analogin_obj_t *self) {
    float voltage;

    if (self->number == 2 || self->number == 3) {
        voltage = 0.0f;
    } else {
        voltage = 5.0f;
    }

    return voltage;
}

void analogin_reset(void) {
    for (int i = 0; i < MP_ARRAY_SIZE(analogin_dev); i++) {
        if (analogin_dev[i].fd >= 0) {
            // stop ADC
            ioctl(analogin_dev[i].fd, ANIOC_CXD56_STOP, 0);
            close(analogin_dev[i].fd);
            analogin_dev[i].fd = -1;
        }
    }
}
