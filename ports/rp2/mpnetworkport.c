/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Damien P. George
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

#include "py/runtime.h"
#include "py/mphal.h"
#include "pendsv.h"

#if MICROPY_PY_LWIP

#include "shared/runtime/softtimer.h"
#include "lwip/timeouts.h"

// Poll lwIP every 64ms by default
#define LWIP_TICK_RATE_MS 64

// Soft timer for running lwIP in the background.
static soft_timer_entry_t mp_network_soft_timer;

#if MICROPY_PY_NETWORK_CYW43
#include "lib/cyw43-driver/src/cyw43.h"
#include "lib/cyw43-driver/src/cyw43_stats.h"
#include "hardware/irq.h"
#include "RP2040.h" // cmsis, for NVIC_SetPriority and PendSV_IRQn

#define CYW43_IRQ_LEVEL GPIO_IRQ_LEVEL_HIGH
#define CYW43_SHARED_IRQ_HANDLER_PRIORITY PICO_SHARED_IRQ_HANDLER_HIGHEST_ORDER_PRIORITY

volatile int cyw43_has_pending = 0;

static void gpio_irq_handler(void) {
    uint32_t events = gpio_get_irq_event_mask(CYW43_PIN_WL_HOST_WAKE);
    if (events & CYW43_IRQ_LEVEL) {
        // As we use a high level interrupt, it will go off forever until it's serviced.
        // So disable the interrupt until this is done.  It's re-enabled again by
        // CYW43_POST_POLL_HOOK which is called at the end of cyw43_poll_func.
        gpio_set_irq_enabled(CYW43_PIN_WL_HOST_WAKE, CYW43_IRQ_LEVEL, false);
        cyw43_has_pending = 1;
        __sev();
        pendsv_schedule_dispatch(PENDSV_DISPATCH_CYW43, cyw43_poll);
        CYW43_STAT_INC(IRQ_COUNT);
    }
}

void cyw43_irq_init(void) {
    gpio_add_raw_irq_handler_with_order_priority(CYW43_PIN_WL_HOST_WAKE, gpio_irq_handler, CYW43_SHARED_IRQ_HANDLER_PRIORITY);
    irq_set_enabled(IO_IRQ_BANK0, true);
    NVIC_SetPriority(PendSV_IRQn, IRQ_PRI_PENDSV);
}

void cyw43_post_poll_hook(void) {
    cyw43_has_pending = 0;
    gpio_set_irq_enabled(CYW43_PIN_WL_HOST_WAKE, CYW43_IRQ_LEVEL, true);
}

#endif

#if MICROPY_PY_NETWORK_WIZNET5K
void wiznet5k_poll(void);
void wiznet5k_deinit(void);

void wiznet5k_try_poll(void) {
    pendsv_schedule_dispatch(PENDSV_DISPATCH_WIZNET, wiznet5k_poll);
}
#endif

u32_t sys_now(void) {
    // Used by LwIP
    return mp_hal_ticks_ms();
}

void lwip_lock_acquire(void) {
    // Prevent PendSV from running.
    pendsv_suspend();
}

void lwip_lock_release(void) {
    // Allow PendSV to run again.
    pendsv_resume();
}

// This is called by soft_timer and executes at PendSV level.
static void mp_network_soft_timer_callback(soft_timer_entry_t *self) {
    // Run the lwIP internal updates.
    sys_check_timeouts();

    #if MICROPY_PY_NETWORK_WIZNET5K
    wiznet5k_poll();
    #endif
}

void mod_network_lwip_init(void) {
    soft_timer_static_init(
        &mp_network_soft_timer,
        SOFT_TIMER_MODE_PERIODIC,
        LWIP_TICK_RATE_MS,
        mp_network_soft_timer_callback
        );

    soft_timer_reinsert(&mp_network_soft_timer, LWIP_TICK_RATE_MS);
}

#endif // MICROPY_PY_LWIP
