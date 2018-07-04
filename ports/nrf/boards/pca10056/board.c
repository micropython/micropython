/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
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

#include <string.h>
#include <stdbool.h>

#include "nrfx.h"
#include "nrfx_power.h"
#include "boards/board.h"

#include "tick.h"
#include "tusb.h"

void board_init(void) {

    // Init USB
    NRF_CLOCK->LFCLKSRC = (uint32_t)((CLOCK_LFCLKSRC_SRC_Xtal << CLOCK_LFCLKSRC_SRC_Pos) & CLOCK_LFCLKSRC_SRC_Msk);
    NRF_CLOCK->TASKS_LFCLKSTART = 1UL;

#ifdef SOFTDEVICE_PRESENT
    // TODO support Softdevice config
#else
    // Softdevice is not present, init power module and register tusb power event function
    // for vusb detect, ready, removed
    extern void tusb_hal_nrf_power_event(uint32_t event);

    // Power module init
    const nrfx_power_config_t pwr_cfg = { 0 };
    nrfx_power_init(&pwr_cfg);

    // USB Power detection
    const nrfx_power_usbevt_config_t config = { .handler = (nrfx_power_usb_event_handler_t) tusb_hal_nrf_power_event };
    nrfx_power_usbevt_init(&config);

    nrfx_power_usbevt_enable();
#endif

    tusb_init();
}

bool board_requests_safe_mode(void) {
  return false;
}

void reset_board(void) {

}


//--------------------------------------------------------------------+
// tinyusb callbacks
//--------------------------------------------------------------------+
void tud_mount_cb(uint8_t rhport) {
    (void) rhport;
}

void tud_umount_cb(uint8_t rhport) {
    (void) rhport;
}

uint32_t tusb_hal_millis(void)
{
    uint64_t ms;
    uint32_t us;
    current_tick(&ms, &us);
    return (uint32_t) ms;
}

