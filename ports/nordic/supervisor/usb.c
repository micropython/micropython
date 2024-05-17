// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 hathach for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "nrfx.h"
#include "nrfx_power.h"
#include "supervisor/usb.h"
#include "shared/runtime/interrupt_char.h"
#include "shared/readline/readline.h"
#include "lib/tinyusb/src/device/usbd.h"
#include "supervisor/background_callback.h"

#ifdef SOFTDEVICE_PRESENT
#include "nrf_sdm.h"
#include "nrf_soc.h"
#endif

// tinyusb function that handles power event (detected, ready, removed)
// We must call it within SD's SOC event handler, or set it as power event handler if SD is not enabled.
extern void tusb_hal_nrf_power_event(uint32_t event);

void init_usb_hardware(void) {

    // 2 is max priority (0, 1, and 4 are reserved for SD)
    // 5 is max priority that still allows calling SD functions such as
    // sd_softdevice_is_enabled
    NVIC_SetPriority(USBD_IRQn, 2);

    // USB power may already be ready at this time -> no event generated
    // We need to invoke the handler based on the status initially for the first call
    static bool first_call = true;
    uint32_t usb_reg;

    #ifdef SOFTDEVICE_PRESENT
    uint8_t sd_en = false;
    (void)sd_softdevice_is_enabled(&sd_en);

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
        const nrfx_power_usbevt_config_t config = { .handler = (nrfx_power_usb_event_handler_t)tusb_hal_nrf_power_event };
        nrfx_power_usbevt_init(&config);

        nrfx_power_usbevt_enable();

        usb_reg = NRF_POWER->USBREGSTATUS;
    }

    if (first_call) {
        first_call = false;
        if (usb_reg & POWER_USBREGSTATUS_VBUSDETECT_Msk) {
            tusb_hal_nrf_power_event(NRFX_POWER_USB_EVT_DETECTED);
        }

        if (usb_reg & POWER_USBREGSTATUS_OUTPUTRDY_Msk) {
            tusb_hal_nrf_power_event(NRFX_POWER_USB_EVT_READY);
        }
    }
}

extern void USBD_IRQHandler(void);
void USBD_IRQHandler(void) {
    usb_irq_handler(0);
}
