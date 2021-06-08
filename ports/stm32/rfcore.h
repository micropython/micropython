/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Damien P. George
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
#ifndef MICROPY_INCLUDED_STM32_RFCORE_H
#define MICROPY_INCLUDED_STM32_RFCORE_H

#include <stdint.h>

void rfcore_init(void);

void rfcore_ble_init(void);
void rfcore_ble_hci_cmd(size_t len, const uint8_t *src);
void rfcore_ble_check_msg(int (*cb)(void *, const uint8_t *, size_t), void *env);
void rfcore_ble_set_txpower(uint8_t level);

void rfcore_start_flash_erase(void);
void rfcore_end_flash_erase(void);

MP_DECLARE_CONST_FUN_OBJ_0(rfcore_status_obj);
MP_DECLARE_CONST_FUN_OBJ_1(rfcore_fw_version_obj);
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(rfcore_sys_hci_obj);

#endif // MICROPY_INCLUDED_STM32_RFCORE_H
