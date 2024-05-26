// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017, 2018 Scott Shawcroft for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2020 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-module/sdcardio/SDCard.h"

extern const mp_obj_type_t sdcardio_SDCard_type;

void common_hal_sdcardio_sdcard_construct(sdcardio_sdcard_obj_t *self, busio_spi_obj_t *spi, const mcu_pin_obj_t *cs, int baudrate);
void common_hal_sdcardio_sdcard_deinit(sdcardio_sdcard_obj_t *self);
void common_hal_sdcardio_sdcard_check_for_deinit(sdcardio_sdcard_obj_t *self);
int common_hal_sdcardio_sdcard_get_blockcount(sdcardio_sdcard_obj_t *self);
int common_hal_sdcardio_sdcard_readblocks(sdcardio_sdcard_obj_t *self, uint32_t start_block, mp_buffer_info_t *buf);
int common_hal_sdcardio_sdcard_sync(sdcardio_sdcard_obj_t *self);
int common_hal_sdcardio_sdcard_writeblocks(sdcardio_sdcard_obj_t *self, uint32_t start_block, mp_buffer_info_t *buf);

// Used by native vfs blockdev.
mp_uint_t sdcardio_sdcard_readblocks(mp_obj_t self_in, uint8_t *buf, uint32_t start_block, uint32_t buflen);
mp_uint_t sdcardio_sdcard_writeblocks(mp_obj_t self_in, uint8_t *buf, uint32_t start_block, uint32_t buflen);
bool sdcardio_sdcard_ioctl(mp_obj_t self_in, size_t cmd, size_t arg, mp_int_t *out_value);
