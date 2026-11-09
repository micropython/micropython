// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "bindings/rp2pio/StateMachine.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/microcontroller/Processor.h"
#include "shared-bindings/usb_host/Port.h"
#include "supervisor/port_heap.h"
#include "supervisor/shared/serial.h"
#include "supervisor/shared/settings.h"
#include "supervisor/usb.h"

#include "pico/time.h"
#include "hardware/structs/mpu.h"
#if PICO_RP2040
#include "RP2040.h" // (cmsis)
#endif
#if PICO_RP2350
#include "RP2350.h" // (cmsis)
#endif
#include "hardware/dma.h"
#include "pico/multicore.h"

#include "py/runtime.h"

#include "tusb.h"

#include "lib/Pico-PIO-USB/src/pio_usb.h"
#include "lib/Pico-PIO-USB/src/pio_usb_configuration.h"


usb_host_port_obj_t usb_host_instance;

volatile bool _core1_ready = false;

#if PIO_USB_HOST_ISOCHRONOUS
// The isochronous ring from settings.toml. Like the port, it is never freed.
static uint8_t *_iso_settings_buffer = NULL;
static uint32_t _iso_settings_size = 0;
// A ring claimed for the length of one stream, which replaces the one above
// until it is released.
static uint8_t *_iso_claimed_buffer = NULL;

// The ring size the host controller accepts: a power of two in [2048, 65536].
static uint32_t _iso_ring_size(size_t size) {
    uint32_t rounded = 2048;
    while (rounded < size && rounded < 65536) {
        rounded <<= 1;
    }
    return rounded;
}

// Hand the host controller another ring. Core 1 may still be in the frame
// where the endpoint stopped streaming into the old ring, so let it finish
// first. False while an endpoint streams into the ring.
static bool _iso_set_ring(uint8_t *buffer, uint32_t size) {
    busy_wait_us(2000);
    return pio_usb_host_set_iso_ring(buffer, size);
}
#endif

// Core1 posts TinyUSB events while core0 may hold the queue FIFO mutex. The
// contended wait path calls best_effort_wfe_or_timeout() and time_us_64(),
// which normally live in flash, but core1 must not touch flash (see the MPU
// setup in core1_main below). These RAM-resident replacements are linked in
// with --wrap on USB host builds only (see the port Makefile). Issue #11116.
//
uint64_t __wrap_time_us_64(void);
bool __wrap_best_effort_wfe_or_timeout(absolute_time_t timeout_timestamp);

// The same hi/lo/hi re-read loop as the SDK's timer_time_us_64, to defeat
// rollover between the two 32-bit halves.
uint64_t __not_in_flash_func(__wrap_time_us_64)(void) {
    uint32_t hi = timer_hw->timerawh;
    uint32_t lo;
    do {
        lo = timer_hw->timerawl;
        uint32_t next_hi = timer_hw->timerawh;
        if (hi == next_hi) {
            break;
        }
        hi = next_hi;
    } while (true);
    return ((uint64_t)hi << 32) | lo;
}

// Mirrors the SDK's own PICO_TIME_DEFAULT_ALARM_POOL_DISABLED fallback
// (a poll of the clock with no __wfe): "best effort" explicitly permits
// returning early, and every SDK caller re-checks in a loop. Polling honors
// the timeout exactly and avoids dragging the alarm pool machinery into RAM.
bool __not_in_flash_func(__wrap_best_effort_wfe_or_timeout)(absolute_time_t timeout_timestamp) {
    tight_loop_contents();
    return __wrap_time_us_64() >= to_us_since_boot(timeout_timestamp);
}

static void __not_in_flash_func(core1_main)(void) {
    // The MPU is reset before this starts.
    SysTick->LOAD = (uint32_t)((common_hal_mcu_processor_get_frequency() / 1000) - 1UL);
    SysTick->VAL = 0UL;   /* Load the SysTick Counter Value */
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |  // Processor clock.
        SysTick_CTRL_ENABLE_Msk;

    // Turn off flash access. After this, it will hard fault. Better than messing
    // up CIRCUITPY.
    #if __CORTEX_M == 0
    MPU->CTRL = MPU_CTRL_PRIVDEFENA_Msk | MPU_CTRL_ENABLE_Msk;
    MPU->RNR = 6; // 7 is used by pico-sdk stack protection.
    MPU->RBAR = XIP_BASE | MPU_RBAR_VALID_Msk;
    MPU->RASR = MPU_RASR_XN_Msk | // Set execute never and everything else is restricted.
        MPU_RASR_ENABLE_Msk |
        (0x1b << MPU_RASR_SIZE_Pos);         // Size is 0x10000000 which masks up to SRAM region.
    MPU->RNR = 7;
    #endif
    #if __CORTEX_M == 33
    MPU->CTRL = MPU_CTRL_PRIVDEFENA_Msk | MPU_CTRL_ENABLE_Msk;
    MPU->RNR = 6; // 7 is used by pico-sdk stack protection.
    MPU->RBAR = XIP_BASE | MPU_RBAR_XN_Msk;
    MPU->RLAR = XIP_SRAM_BASE | MPU_RLAR_EN_Msk;
    #endif

    _core1_ready = true;

    while (true) {
        pio_usb_host_frame();
        if (tuh_task_event_ready()) {
            // Queue the tinyusb background task.
            usb_background_schedule();
        }
        // Wait for systick to reload.
        while ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == 0) {
        }
    }
}

// As of 0.6.1, the PIO resource requirement is 1 PIO with 3 state machines &
// 32 instructions. Since there are only 32 instructions in a state machine, it should
// be impossible to have an allocated state machine but 32 instruction slots available;
// go ahead and check for it anyway.
//
// Since we require ALL state machines to be available, it's not possible for the GPIO
// ranges to mismatch on rp2350b
static size_t get_usb_pio(void) {
    size_t pio_index = rp2pio_statemachine_find_pio(32, NUM_PIO_STATE_MACHINES);
    if (pio_index == NUM_PIOS) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("All state machines in use"));
    }
    return pio_index;
}


usb_host_port_obj_t *common_hal_usb_host_port_construct(const mcu_pin_obj_t *dp, const mcu_pin_obj_t *dm) {
    if ((dp->number + 1 != dm->number)
        && (dp->number - 1 != dm->number)) {
        raise_ValueError_invalid_pins();
    }
    usb_host_port_obj_t *self = &usb_host_instance;

    // Return the singleton if given the same pins.
    if (self->dp != NULL) {
        if (self->dp != dp || self->dm != dm) {
            mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("%q in use"), MP_QSTR_usb_host);
        }
        return self;
    }

    assert_pin_free(dp);
    assert_pin_free(dm);

    pio_usb_configuration_t pio_cfg = PIO_USB_DEFAULT_CONFIG;
    pio_cfg.skip_alarm_pool = true;
    pio_cfg.pin_dp = dp->number;
    if (dp->number - 1 == dm->number) {
        pio_cfg.pinout = PIO_USB_PINOUT_DMDP;
    }
    pio_cfg.pio_tx_num = get_usb_pio();
    pio_cfg.pio_rx_num = pio_cfg.pio_tx_num;
    int dma_ch = dma_claim_unused_channel(false);
    if (dma_ch < 0) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("All dma channels in use"));
    }
    pio_cfg.tx_ch = dma_ch;

    self->base.type = &usb_host_port_type;
    self->dp = dp;
    self->dm = dm;
    claim_pin(dp);
    claim_pin(dm);

    // Unclaim everything so that the library can.
    dma_channel_unclaim(pio_cfg.tx_ch);

    #if PIO_USB_HOST_ISOCHRONOUS
    // Ring storage for isochronous IN endpoints (UVC cameras, UAC audio),
    // opt-in via settings.toml. Set  before core 1 launches so it never
    // observes a half-set ring.
    mp_int_t iso_size = 0;
    if (settings_get_int("CIRCUITPY_USB_HOST_ISO_BUFFER_SIZE", &iso_size) == SETTINGS_OK
        && iso_size > 0) {
        uint32_t rounded = _iso_ring_size(iso_size);
        // dma_capable=true forces the internal-SRAM heap. Never PSRAM: PSRAM
        // is inside the XIP region that core 1's MPU blocks below, and the
        // ring is written from core 1. Like the port itself, the buffer is
        // never freed; it survives soft reload.
        uint8_t *iso_buf = port_malloc(rounded, true);
        if (iso_buf != NULL) {
            pio_usb_host_set_iso_ring(iso_buf, rounded);
            _iso_settings_buffer = iso_buf;
            _iso_settings_size = rounded;
        } else {
            mp_raise_RuntimeError_varg(MP_ERROR_TEXT("Failed to allocate %q buffer"),
                MP_QSTR_USB_HOST_ISO_BUFFER);
        }
    }
    #endif

    // Core 1 will run the SOF interrupt directly.
    _core1_ready = false;
    multicore_launch_core1(core1_main);
    while (!_core1_ready) {
    }

    tuh_configure(TUH_OPT_RHPORT, TUH_CFGID_RPI_PIO_USB_CONFIGURATION, &pio_cfg);
    tuh_init(TUH_OPT_RHPORT);

    return self;
}

bool common_hal_usb_host_port_claim_iso_buffer(size_t size) {
    #if PIO_USB_HOST_ISOCHRONOUS
    // A ring left claimed because it was still streaming may be free by now.
    common_hal_usb_host_port_release_iso_buffer();
    if (_iso_claimed_buffer != NULL) {
        return false;
    }
    uint32_t rounded = _iso_ring_size(size);
    // Internal SRAM only, for the same reason as the settings.toml ring above.
    uint8_t *buffer = port_malloc(rounded, true);
    if (buffer == NULL) {
        m_malloc_fail(rounded);
    }
    if (!_iso_set_ring(buffer, rounded)) {
        port_free(buffer);
        return false;
    }
    _iso_claimed_buffer = buffer;
    return true;
    #else
    (void)size;
    return false;
    #endif
}

void common_hal_usb_host_port_release_iso_buffer(void) {
    #if PIO_USB_HOST_ISOCHRONOUS
    if (_iso_claimed_buffer == NULL) {
        return;
    }
    // When an endpoint still streams into the ring, core 1 keeps writing it, so
    // it can't be freed. It stays claimed and the next claim tries again.
    if (_iso_set_ring(_iso_settings_buffer, _iso_settings_size)) {
        port_free(_iso_claimed_buffer);
        _iso_claimed_buffer = NULL;
    }
    #endif
}

// Not used, but we must define to put this hook into SRAM
void __not_in_flash_func(tuh_event_hook_cb)(uint8_t rhport, uint32_t eventid, bool in_isr) {
    (void)rhport;
    (void)eventid;
    (void)in_isr;
}
