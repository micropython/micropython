/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 * Copyright (c) 2019 Glenn Ruben Bakke
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
 *
 * This file is part of the TinyUSB stack.
 */

#include "py/mphal.h"

#if MICROPY_HW_ENABLE_USBDEV && MICROPY_HW_USB_CDC

#include "nrfx.h"
#include "nrfx_power.h"
#include "nrfx_uart.h"
#include "py/ringbuf.h"
#include "py/stream.h"
#include "py/runtime.h"
#include "shared/runtime/interrupt_char.h"
#include "shared/tinyusb/mp_usbd.h"
#include "shared/tinyusb/mp_usbd_cdc.h"
#include "extmod/misc.h"
#include <hal/nrf_ficr.h>

#ifdef BLUETOOTH_SD
#include "nrf_sdm.h"
#include "nrf_soc.h"
#include "ble_drv.h"
#endif

extern void tusb_hal_nrf_power_event(uint32_t event);

static void board_init(void) {
    // Config clock source.
#ifndef BLUETOOTH_SD
    NRF_CLOCK->LFCLKSRC = (uint32_t)((CLOCK_LFCLKSRC_SRC_Xtal << CLOCK_LFCLKSRC_SRC_Pos) & CLOCK_LFCLKSRC_SRC_Msk);
    NRF_CLOCK->TASKS_LFCLKSTART = 1UL;
#endif

    // Priorities 0, 1, 4 (nRF52) are reserved for SoftDevice
    // 2 is highest for application
    NRFX_IRQ_PRIORITY_SET(USBD_IRQn, 2);

    // USB power may already be ready at this time -> no event generated
    // We need to invoke the handler based on the status initially
    uint32_t usb_reg;

#ifdef BLUETOOTH_SD
    uint8_t sd_en = false;
    sd_softdevice_is_enabled(&sd_en);

    if (sd_en) {
        sd_power_usbdetected_enable(true);
        sd_power_usbpwrrdy_enable(true);
        sd_power_usbremoved_enable(true);

        sd_power_usbregstatus_get(&usb_reg);
    } else
#endif
    {
        // Power module init
        const nrfx_power_config_t pwr_cfg = { 0 };
        nrfx_power_init(&pwr_cfg);

        // Register tusb function as USB power handler
        const nrfx_power_usbevt_config_t config = { .handler = (nrfx_power_usb_event_handler_t) tusb_hal_nrf_power_event };
        nrfx_power_usbevt_init(&config);

        nrfx_power_usbevt_enable();

        usb_reg = NRF_POWER->USBREGSTATUS;
    }

    if (usb_reg & POWER_USBREGSTATUS_VBUSDETECT_Msk) {
        tusb_hal_nrf_power_event(NRFX_POWER_USB_EVT_DETECTED);
    }

#ifndef BLUETOOTH_SD
    if (usb_reg & POWER_USBREGSTATUS_OUTPUTRDY_Msk) {
        tusb_hal_nrf_power_event(NRFX_POWER_USB_EVT_READY);
    }
#endif
}

void mp_usbd_port_get_serial_number(char *serial_buf) {
    uint32_t deviceid[2];
	deviceid[0] = nrf_ficr_deviceid_get(NRF_FICR, 0);
	deviceid[1] = nrf_ficr_deviceid_get(NRF_FICR, 1);
    MP_STATIC_ASSERT(sizeof(deviceid) * 2 <= MICROPY_HW_USB_DESC_STR_MAX);
    mp_usbd_hex_str(serial_buf, (uint8_t *)deviceid, sizeof(deviceid));
}

int usb_cdc_init(void)
{
    static bool initialized = false;
    if (!initialized) {
        #if BLUETOOTH_SD
        // Initialize the clock and BLE stack.
        ble_drv_stack_enable();
        #endif
        board_init();
        initialized = true;
    }

    mp_usbd_init();
    return 0;
}

#ifdef BLUETOOTH_SD
// process SOC event from SD
void usb_cdc_sd_event_handler(uint32_t soc_evt) {
    /*------------- usb power event handler -------------*/
    int32_t usbevt = (soc_evt == NRF_EVT_POWER_USB_DETECTED   ) ? NRFX_POWER_USB_EVT_DETECTED:
                     (soc_evt == NRF_EVT_POWER_USB_POWER_READY) ? NRFX_POWER_USB_EVT_READY   :
                     (soc_evt == NRF_EVT_POWER_USB_REMOVED    ) ? NRFX_POWER_USB_EVT_REMOVED : -1;

    if (usbevt >= 0) {
        tusb_hal_nrf_power_event(usbevt);
    }
}
#endif

void USBD_IRQHandler(void) {
    tud_int_handler(0);
}

#endif // MICROPY_HW_USB_CDC
