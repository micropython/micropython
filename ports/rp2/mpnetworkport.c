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
#include "lwip/netif.h"
#include "lwip/timeouts.h"

// Poll lwIP every 64ms by default
#define LWIP_TICK_RATE_MS 64

// Soft timer for running lwIP in the background.
static soft_timer_entry_t mp_network_soft_timer;

// Callback for change of netif state
NETIF_DECLARE_EXT_CALLBACK(netif_callback)

#if MICROPY_PY_NETWORK_CYW43
#include "lib/cyw43-driver/src/cyw43.h"
#include "lib/cyw43-driver/src/cyw43_stats.h"

#if !defined(__riscv)
#if PICO_RP2040
#include "RP2040.h" // cmsis, for NVIC_SetPriority and PendSV_IRQn
#elif PICO_RP2350
#include "RP2350.h" // cmsis, for NVIC_SetPriority and PendSV_IRQn
#else
#error Unknown processor
#endif
#endif

#define CYW43_IRQ_LEVEL GPIO_IRQ_LEVEL_HIGH
#define CYW43_SHARED_IRQ_HANDLER_PRIORITY PICO_SHARED_IRQ_HANDLER_HIGHEST_ORDER_PRIORITY


// The Pico SDK only lets us set GPIO wake on the current running CPU, but the
// hardware doesn't have this limit. We need to always enable/disable the pin
// interrupt on CPU0, regardless of which CPU runs PendSV and
// cyw43_post_poll_hook(). See feature request at https://github.com/raspberrypi/pico-sdk/issues/2354
static void gpio_set_cpu0_host_wake_irq_enabled(bool enable) {
    // This is a re-implementation of gpio_set_irq_enabled() and _gpio_set_irq_enabled()
    // from the pico-sdk, but with the core, gpio, and event type hardcoded to shrink
    // code size.
    io_bank0_irq_ctrl_hw_t *irq_ctrl_base = &io_bank0_hw->proc0_irq_ctrl;
    uint32_t gpio = CYW43_PIN_WL_HOST_WAKE;
    uint32_t events = CYW43_IRQ_LEVEL;
    io_rw_32 *en_reg = &irq_ctrl_base->inte[gpio / 8];
    events <<= 4 * (gpio % 8);
    if (enable) {
        hw_set_bits(en_reg, events);
    } else {
        hw_clear_bits(en_reg, events);
    }
}

// GPIO IRQ always runs on CPU0
static void gpio_irq_handler(void) {
    uint32_t events = gpio_get_irq_event_mask(CYW43_PIN_WL_HOST_WAKE);
    if (events & CYW43_IRQ_LEVEL) {
        // As we use a level interrupt (and can't use an edge interrupt
        // as CYW43_PIN_WL_HOST_WAKE is also a SPI data pin), we need to disable
        // the interrupt to stop it re-triggering until after PendSV run
        // cyw43_poll(). It is re-enabled in cyw43_post_poll_hook(), implemented
        // below.
        gpio_set_cpu0_host_wake_irq_enabled(false);
        pendsv_schedule_dispatch(PENDSV_DISPATCH_CYW43, cyw43_poll);
        __sev();
        CYW43_STAT_INC(IRQ_COUNT);
    }
}

void cyw43_irq_init(void) {
    gpio_add_raw_irq_handler_with_order_priority(CYW43_PIN_WL_HOST_WAKE, gpio_irq_handler, CYW43_SHARED_IRQ_HANDLER_PRIORITY);
    irq_set_enabled(IO_IRQ_BANK0, true);
}

// This hook will run on whichever CPU serviced the PendSV interrupt
void cyw43_post_poll_hook(void) {
    gpio_set_cpu0_host_wake_irq_enabled(true);
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

    // Only keep the timer running if any TCP sockets are active, or any netif is up
    struct netif *netif;
    extern void *tcp_active_pcbs;
    bool keep_running = (tcp_active_pcbs != NULL);
    if (!keep_running) {
        NETIF_FOREACH(netif) {
            if (netif->flags & NETIF_FLAG_LINK_UP) {
                keep_running = true;
                break;
            }
        }
    }

    // Periodic timer will re-queue as soon as this handler exits,
    // one shot timer will not
    mp_network_soft_timer.mode = keep_running ? SOFT_TIMER_MODE_PERIODIC : SOFT_TIMER_MODE_ONE_SHOT;
}

static void mp_network_netif_status_cb(struct netif *netif, netif_nsc_reason_t reason, const netif_ext_callback_args_t *args);

void mod_network_lwip_init(void) {
    soft_timer_static_init(
        &mp_network_soft_timer,
        SOFT_TIMER_MODE_ONE_SHOT,
        LWIP_TICK_RATE_MS,
        mp_network_soft_timer_callback
        );

    if (netif_callback.callback_fn == NULL) {
        netif_add_ext_callback(&netif_callback, mp_network_netif_status_cb);
    }
}

static void mp_network_netif_status_cb(struct netif *netif, netif_nsc_reason_t reason, const netif_ext_callback_args_t *args) {
    // Start the network soft timer any time an interface comes up, unless
    // it's already running
    if (reason == LWIP_NSC_LINK_CHANGED && args->link_changed.state
        && mp_network_soft_timer.mode == SOFT_TIMER_MODE_ONE_SHOT) {
        mp_network_soft_timer.mode = SOFT_TIMER_MODE_PERIODIC;
        soft_timer_reinsert(&mp_network_soft_timer, LWIP_TICK_RATE_MS);
    }
}

#endif // MICROPY_PY_LWIP
