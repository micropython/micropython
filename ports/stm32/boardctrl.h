/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Damien P. George
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
#ifndef MICROPY_INCLUDED_STM32_BOARDCTRL_H
#define MICROPY_INCLUDED_STM32_BOARDCTRL_H

#include "py/mpconfig.h"

// Additional entries for use with pendsv_schedule_dispatch.
#ifndef MICROPY_BOARD_PENDSV_ENTRIES
#define MICROPY_BOARD_PENDSV_ENTRIES
#endif

#ifndef MICROPY_BOARD_STARTUP
#define MICROPY_BOARD_STARTUP powerctrl_check_enter_bootloader
#endif

#ifndef MICROPY_BOARD_EARLY_INIT
#define MICROPY_BOARD_EARLY_INIT()
#endif

#ifndef MICROPY_BOARD_BEFORE_SOFT_RESET_LOOP
#define MICROPY_BOARD_BEFORE_SOFT_RESET_LOOP boardctrl_before_soft_reset_loop
#endif

#ifndef MICROPY_BOARD_TOP_SOFT_RESET_LOOP
#define MICROPY_BOARD_TOP_SOFT_RESET_LOOP boardctrl_top_soft_reset_loop
#endif

#ifndef MICROPY_BOARD_RUN_BOOT_PY
#define MICROPY_BOARD_RUN_BOOT_PY boardctrl_run_boot_py
#endif

#ifndef MICROPY_BOARD_RUN_MAIN_PY
#define MICROPY_BOARD_RUN_MAIN_PY boardctrl_run_main_py
#endif

#ifndef MICROPY_BOARD_START_SOFT_RESET
#define MICROPY_BOARD_START_SOFT_RESET boardctrl_start_soft_reset
#endif

#ifndef MICROPY_BOARD_END_SOFT_RESET
#define MICROPY_BOARD_END_SOFT_RESET boardctrl_end_soft_reset
#endif

// Called when USBD CDC data is available.
// Default function defined in usbd_cdc_interface.h.
#ifndef MICROPY_BOARD_USBD_CDC_RX_EVENT
#define MICROPY_BOARD_USBD_CDC_RX_EVENT usbd_cdc_rx_event_callback
#endif

// Called to poll Bluetooth HCI now.
// Default function defined in mpbthciport.h.
#ifndef MICROPY_BOARD_BT_HCI_POLL_NOW
#define MICROPY_BOARD_BT_HCI_POLL_NOW mp_bluetooth_hci_poll_now_default
#endif

// Called to poll Bluetooth HCI after the given timeout.
// Default function defined in mpbthciport.h.
#ifndef MICROPY_BOARD_BT_HCI_POLL_IN_MS
#define MICROPY_BOARD_BT_HCI_POLL_IN_MS mp_bluetooth_hci_poll_in_ms_default
#endif

// Constants to return from boardctrl_run_boot_py, boardctrl_run_main_py.
enum {
    BOARDCTRL_CONTINUE,
    BOARDCTRL_GOTO_SOFT_RESET_EXIT,
};

// Constants for boardctrl_state_t.reset_mode.
enum {
    BOARDCTRL_RESET_MODE_NORMAL = 1,
    BOARDCTRL_RESET_MODE_SAFE_MODE = 2,
    BOARDCTRL_RESET_MODE_FACTORY_FILESYSTEM = 3,
    BOARDCTRL_RESET_MODE_BOOTLOADER = 4,
};

typedef struct _boardctrl_state_t {
    uint8_t reset_mode;
    bool log_soft_reset;
} boardctrl_state_t;

void boardctrl_before_soft_reset_loop(boardctrl_state_t *state);
void boardctrl_top_soft_reset_loop(boardctrl_state_t *state);
int boardctrl_run_boot_py(boardctrl_state_t *state);
int boardctrl_run_main_py(boardctrl_state_t *state);
void boardctrl_start_soft_reset(boardctrl_state_t *state);
void boardctrl_end_soft_reset(boardctrl_state_t *state);

#endif // MICROPY_INCLUDED_STM32_BOARDCTRL_H
