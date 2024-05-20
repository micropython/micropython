// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-module/aesio/__init__.h"

extern const mp_obj_type_t aesio_aes_type;

void common_hal_aesio_aes_construct(aesio_aes_obj_t *self,
    const uint8_t *key,
    uint32_t key_length,
    const uint8_t *iv,
    int mode,
    int counter);
void common_hal_aesio_aes_rekey(aesio_aes_obj_t *self,
    const uint8_t *key,
    uint32_t key_length,
    const uint8_t *iv);
void common_hal_aesio_aes_set_mode(aesio_aes_obj_t *self,
    int mode);
void common_hal_aesio_aes_encrypt(aesio_aes_obj_t *self,
    uint8_t *buffer,
    size_t len);
void common_hal_aesio_aes_decrypt(aesio_aes_obj_t *self,
    uint8_t *buffer,
    size_t len);
