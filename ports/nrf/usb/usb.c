/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 hathach for Adafruit Industries
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

#include "nrfx.h"
#include "nrfx_power.h"
#include "tick.h"
#include "usb.h"
#include "lib/utils/interrupt_char.h"

#ifdef NRF52840_XXAA

//--------------------------------------------------------------------+
// STRING DESCRIPTORS
//--------------------------------------------------------------------+

// array of pointer to string descriptors
uint16_t const * const string_desc_arr [] =
{
    // 0 index is supported language = English
    TUD_DESC_STRCONV(0x0409),

    // Manufacturer
    TUD_DESC_STRCONV('A','d','a','f','r','u','i','t',' ','I','n','d','u','s','t','r','i','e','s'),

    // Product
    TUD_DESC_STRCONV('C','i','r','c','u','i','t','P','Y',' ','n','R','F','5','2'),

    // Serials TODO use chip ID
    TUD_DESC_STRCONV('1', '2', '3', '4', '5'),

    // CDC Interface
    TUD_DESC_STRCONV('B','l','u','e','f','r','u','i','t',' ','S','e','r','i','a','l'),

    // MSC Interface
    TUD_DESC_STRCONV('B','l','u','e','f','r','u','i','t',' ','S','t','o','r','a','g','e'),

    // Custom Interface
    TUD_DESC_STRCONV('B','l','u','e','f','r','u','i','t',' ','C','u','s','t','o','m')
};

// tud_desc_set is required by tinyusb stack
// since CFG_TUD_DESC_AUTO is enabled, we only need to set string_arr
tud_desc_set_t tud_desc_set =
{
    .device     = NULL,
    .config     = NULL,
    .string_arr = (uint8_t const **) string_desc_arr,
    .hid_report = NULL
};

void usb_init(void) {

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

//--------------------------------------------------------------------+
// tinyusb callbacks
//--------------------------------------------------------------------+
void tud_mount_cb(void) {
}

void tud_umount_cb(void) {
}

uint32_t tusb_hal_millis(void) {
    uint64_t ms;
    uint32_t us;
    current_tick(&ms, &us);
    return (uint32_t) ms;
}

#if MICROPY_KBD_EXCEPTION

/**
 * Callback invoked when received an "wanted" char.
 * @param itf           Interface index (for multiple cdc interfaces)
 * @param wanted_char   The wanted char (set previously)
 */
void tud_cdc_rx_wanted_cb(uint8_t itf, char wanted_char)
{
    (void) itf; // not used

    if (mp_interrupt_char == wanted_char) {
        tud_cdc_read_flush();    // flush read fifo
        mp_keyboard_interrupt();
    }
}

#endif

#endif // NRF52840_XXAA

