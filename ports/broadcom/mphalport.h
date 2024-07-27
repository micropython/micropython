// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Scott Shawcroft
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdint.h>
#include "py/obj.h"


#include "supervisor/shared/tick.h"

void mp_hal_delay_ms(mp_uint_t ms);
void mp_hal_delay_us(mp_uint_t us);

#define mp_hal_ticks_ms()       ((mp_uint_t)supervisor_ticks_ms32())

void mp_hal_set_interrupt_char(int c);
int mp_hal_stdin_rx_chr(void);
mp_uint_t mp_hal_stdout_tx_strn(const char *str, size_t len);

#ifdef MICROPY_HW_USBHOST
#include "usbkbd.h"

void usbkbd_setup();
#endif
