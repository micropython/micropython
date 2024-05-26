// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2019 Artur Pacholec
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdint.h>

#define MICROPY_PY_SYS_PLATFORM                     "NXP IMXRT10XX"
#define SPI_FLASH_MAX_BAUDRATE 24000000

extern uint8_t _ld_filesystem_start;
extern uint8_t _ld_filesystem_end;
extern uint8_t _ld_default_stack_size;

#define CIRCUITPY_DEFAULT_STACK_SIZE                ((uint32_t)&_ld_default_stack_size)
#define MICROPY_PY_BUILTINS_NOTIMPLEMENTED          (0)
#define MICROPY_PY_FUNCTION_ATTRS                   (0)
#define MICROPY_PY_REVERSE_SPECIAL_METHODS          (1)


#define CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_START_ADDR ((uint32_t)&_ld_filesystem_start)
#define CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_SIZE ((uint32_t)(&_ld_filesystem_end - &_ld_filesystem_start))

// Allocate 32 bytes at a time instead of the default 16 so that allocated buffers
// are aligned to cache lines.
#define MICROPY_BYTES_PER_GC_BLOCK              (32)

#include "py/circuitpy_mpconfig.h"

// TODO:
//    mp_obj_t playing_audio[AUDIO_DMA_CHANNEL_COUNT] as an MP_REGISTER_ROOT_POINTER.
