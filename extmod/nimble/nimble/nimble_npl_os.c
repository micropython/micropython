/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018-2019 Damien P. George
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

#include <stdio.h>
#include "py/mphal.h"
#include "py/runtime.h"
#include "nimble/ble.h"
#include "nimble/nimble_npl.h"
#include "extmod/nimble/hal/hal_uart.h"

#include "extmod/modbluetooth.h"
#include "extmod/nimble/modbluetooth_nimble.h"

#define DEBUG_OS_printf(...) // printf(__VA_ARGS__)
#define DEBUG_MALLOC_printf(...) // printf(__VA_ARGS__)
#define DEBUG_EVENT_printf(...) // printf(__VA_ARGS__)
#define DEBUG_MUTEX_printf(...) // printf(__VA_ARGS__)
#define DEBUG_SEM_printf(...) // printf(__VA_ARGS__)
#define DEBUG_CALLOUT_printf(...) // printf(__VA_ARGS__)
#define DEBUG_TIME_printf(...) // printf(__VA_ARGS__)
#define DEBUG_CRIT_printf(...) // printf(__VA_ARGS__)

bool ble_npl_os_started(void) {
    DEBUG_OS_printf("ble_npl_os_started\n");
    return true;
}

void *ble_npl_get_current_task_id(void) {
    DEBUG_OS_printf("ble_npl_get_current_task_id\n");
    return NULL;
}

/******************************************************************************/
// malloc

// Maintain a linked list of heap memory that we've passed to Nimble,
// discoverable via the bluetooth_nimble_memory root pointer.

typedef struct _mp_bluetooth_nimble_malloc_t {
    struct _mp_bluetooth_nimble_malloc_t *prev;
    struct _mp_bluetooth_nimble_malloc_t *next;
    size_t size;
    uint8_t data[];
} mp_bluetooth_nimble_malloc_t;

// TODO: This is duplicated from mbedtls. Perhaps make this a generic feature?
STATIC void *m_malloc_bluetooth(size_t size) {
    size += sizeof(mp_bluetooth_nimble_malloc_t);
    mp_bluetooth_nimble_malloc_t *alloc = m_malloc0(size);
    alloc->size = size;
    alloc->next = MP_STATE_PORT(bluetooth_nimble_memory);
    if (alloc->next) {
        alloc->next->prev = alloc;
    }
    MP_STATE_PORT(bluetooth_nimble_memory) = alloc;
    return alloc->data;
}

STATIC mp_bluetooth_nimble_malloc_t* get_nimble_malloc(void *ptr) {
    return (mp_bluetooth_nimble_malloc_t*)((uintptr_t)ptr - sizeof(mp_bluetooth_nimble_malloc_t));
}

STATIC void m_free_bluetooth(void *ptr) {
    mp_bluetooth_nimble_malloc_t *alloc = get_nimble_malloc(ptr);
    if (alloc->next) {
        alloc->next->prev = alloc->prev;
    }
    if (alloc->prev) {
        alloc->prev->next = alloc->next;
    } else {
        MP_STATE_PORT(bluetooth_nimble_memory) = NULL;
    }
    m_free(alloc
    #if MICROPY_MALLOC_USES_ALLOCATED_SIZE
           , alloc->size
    #endif
    );
}

// Check if a nimble ptr is tracked.
// If it isn't, that means that it's from a previous soft-reset cycle.
STATIC bool is_valid_nimble_malloc(void *ptr) {
    DEBUG_MALLOC_printf("NIMBLE is_valid_nimble_malloc(%p)\n", ptr);
    mp_bluetooth_nimble_malloc_t *alloc = MP_STATE_PORT(bluetooth_nimble_memory);
    while (alloc) {
        DEBUG_MALLOC_printf("NIMBLE   checking: %p\n", alloc->data);
        if (alloc->data == ptr) {
            return true;
        }
        alloc = alloc->next;
    }
    return false;
}

void *nimble_malloc(size_t size) {
    DEBUG_MALLOC_printf("NIMBLE malloc(%u)\n", (uint)size);
    void* ptr = m_malloc_bluetooth(size);
    DEBUG_MALLOC_printf("  --> %p\n", ptr);
    return ptr;
}

// Only free if it's still a valid pointer.
void nimble_free(void *ptr) {
    DEBUG_MALLOC_printf("NIMBLE free(%p)\n", ptr);

    if (ptr) {
        // After a stack re-init, NimBLE has variables in BSS that might be
        // still pointing to old allocations from a previous init. We can't do
        // anything about this (e.g. ble_gatts_free_mem is private). But we
        // can identify that this is a non-null, invalid alloc because it
        // won't be in our list, so ignore it because it is effectively free'd
        // anyway (it's not referenced by anything the GC can find).
        if (is_valid_nimble_malloc(ptr)) {
            m_free_bluetooth(ptr);
        }
    }
}

// Only realloc if it's still a valid pointer. Otherwise just malloc.
void *nimble_realloc(void *ptr, size_t new_size) {
    DEBUG_MALLOC_printf("NIMBLE realloc(%p, %u)\n", ptr, (uint)new_size);

    if (!ptr) {
        return nimble_malloc(new_size);
    }

    assert(is_valid_nimble_malloc(ptr));

    // Existing alloc is big enough.
    mp_bluetooth_nimble_malloc_t *alloc = get_nimble_malloc(ptr);
    size_t old_size = alloc->size - sizeof(mp_bluetooth_nimble_malloc_t);
    if (old_size >= new_size) {
        return ptr;
    }

    // Allocate a new, larger region.
    void *ptr2 = m_malloc_bluetooth(new_size);

    // Copy old, smaller region into new region.
    memcpy(ptr2, ptr, old_size);
    m_free_bluetooth(ptr);

    DEBUG_MALLOC_printf("  --> %p\n", ptr2);

    return ptr2;
}

// No-op implementation (only used by NimBLE logging).
int nimble_sprintf(char *str, const char *fmt, ...) {
    str[0] = 0;
    return 0;
}

/******************************************************************************/
// EVENTQ

struct ble_npl_eventq *global_eventq = NULL;

// This must not be called recursively or concurrently with the UART handler.
void mp_bluetooth_nimble_os_eventq_run_all(void) {
    if (mp_bluetooth_nimble_ble_state == MP_BLUETOOTH_NIMBLE_BLE_STATE_OFF) {
        return;
    }

    // Keep running while there are pending events.
    while (true) {
        struct ble_npl_event *ev = NULL;

        os_sr_t sr;
        OS_ENTER_CRITICAL(sr);
        // Search all queues for an event.
        for (struct ble_npl_eventq *evq = global_eventq; evq != NULL; evq = evq->nextq) {
            ev = evq->head;
            if (ev) {
                // Remove this event from the queue.
                evq->head = ev->next;
                if (ev->next) {
                    ev->next->prev = NULL;
                    ev->next = NULL;
                }
                ev->prev = NULL;

                ev->pending = false;

                // Stop searching and execute this event.
                break;
            }
        }
        OS_EXIT_CRITICAL(sr);

        if (!ev) {
            break;
        }

        // Run the event handler.
        DEBUG_EVENT_printf("event_run(%p)\n", ev);
        ev->fn(ev);
        DEBUG_EVENT_printf("event_run(%p) done\n", ev);

        if (ev->pending) {
            // If this event has been re-enqueued while it was running, then
            // stop running further events. This prevents an infinite loop
            // where the reset event re-enqueues itself on HCI timeout.
            break;
        }
    }
}

void ble_npl_eventq_init(struct ble_npl_eventq *evq) {
    DEBUG_EVENT_printf("ble_npl_eventq_init(%p)\n", evq);
    os_sr_t sr;
    OS_ENTER_CRITICAL(sr);
    evq->head = NULL;
    struct ble_npl_eventq **evq2;
    for (evq2 = &global_eventq; *evq2 != NULL; evq2 = &(*evq2)->nextq) {
    }
    *evq2 = evq;
    evq->nextq = NULL;
    OS_EXIT_CRITICAL(sr);
}

void ble_npl_eventq_put(struct ble_npl_eventq *evq, struct ble_npl_event *ev) {
    DEBUG_EVENT_printf("ble_npl_eventq_put(%p, %p (%p, %p))\n", evq, ev, ev->fn, ev->arg);
    os_sr_t sr;
    OS_ENTER_CRITICAL(sr);
    ev->next = NULL;
    ev->pending = true;
    if (evq->head == NULL) {
        // Empty list, make this the first item.
        evq->head = ev;
        ev->prev = NULL;
    } else {
        // Find the tail of this list.
        struct ble_npl_event *tail = evq->head;
        while (true) {
            if (tail == ev) {
                DEBUG_EVENT_printf("  --> already in queue\n");
                // Already in the list (e.g. a fragmented ACL will enqueue an
                // event to process it for each fragment).
                break;
            }
            if (tail->next == NULL) {
                // Found the end of the list, add this event as the tail.
                tail->next = ev;
                ev->prev = tail;
                break;
            }
            DEBUG_EVENT_printf("  --> %p\n", tail->next);
            tail = tail->next;
        }
    }
    OS_EXIT_CRITICAL(sr);
}

void ble_npl_event_init(struct ble_npl_event *ev, ble_npl_event_fn *fn, void *arg) {
    DEBUG_EVENT_printf("ble_npl_event_init(%p, %p, %p)\n", ev, fn, arg);
    ev->fn = fn;
    ev->arg = arg;
    ev->next = NULL;
    ev->pending = false;
}

void *ble_npl_event_get_arg(struct ble_npl_event *ev) {
    DEBUG_EVENT_printf("ble_npl_event_get_arg(%p) -> %p\n", ev, ev->arg);
    return ev->arg;
}

void ble_npl_event_set_arg(struct ble_npl_event *ev, void *arg) {
    DEBUG_EVENT_printf("ble_npl_event_set_arg(%p, %p)\n", ev, arg);
    ev->arg = arg;
}

/******************************************************************************/
// MUTEX

ble_npl_error_t ble_npl_mutex_init(struct ble_npl_mutex *mu) {
    DEBUG_MUTEX_printf("ble_npl_mutex_init(%p)\n", mu);
    mu->locked = 0;
    return BLE_NPL_OK;
}

ble_npl_error_t ble_npl_mutex_pend(struct ble_npl_mutex *mu, ble_npl_time_t timeout) {
    DEBUG_MUTEX_printf("ble_npl_mutex_pend(%p, %u) locked=%u\n", mu, (uint)timeout, (uint)mu->locked);

    // All NimBLE code is executed by the scheduler (and is therefore
    // implicitly mutexed) so this mutex implementation is a no-op.

    ++mu->locked;

    return BLE_NPL_OK;
}

ble_npl_error_t ble_npl_mutex_release(struct ble_npl_mutex *mu) {
    DEBUG_MUTEX_printf("ble_npl_mutex_release(%p) locked=%u\n", mu, (uint)mu->locked);
    assert(mu->locked > 0);

    --mu->locked;

    return BLE_NPL_OK;
}

/******************************************************************************/
// SEM

ble_npl_error_t ble_npl_sem_init(struct ble_npl_sem *sem, uint16_t tokens) {
    DEBUG_SEM_printf("ble_npl_sem_init(%p, %u)\n", sem, (uint)tokens);
    sem->count = tokens;
    return BLE_NPL_OK;
}

ble_npl_error_t ble_npl_sem_pend(struct ble_npl_sem *sem, ble_npl_time_t timeout) {
    DEBUG_SEM_printf("ble_npl_sem_pend(%p, %u) count=%u\n", sem, (uint)timeout, (uint)sem->count);

    // This is only called by NimBLE in ble_hs_hci_cmd_tx to synchronously
    // wait for an HCI ACK. The corresponding ble_npl_sem_release is called
    // directly by the UART rx handler (i.e. hal_uart_rx_cb in
    // extmod/nimble/hal/hal_uart.c). So this loop needs to run only the HCI
    // UART processing but not run any events.

    if (sem->count == 0) {
        uint32_t t0 = mp_hal_ticks_ms();
        while (sem->count == 0 && mp_hal_ticks_ms() - t0 < timeout) {
            if (sem->count != 0) {
                break;
            }

            mp_bluetooth_nimble_hci_uart_wfi();
        }

        if (sem->count == 0) {
            DEBUG_SEM_printf("ble_npl_sem_pend: semaphore timeout\n");
            return BLE_NPL_TIMEOUT;
        }

        DEBUG_SEM_printf("ble_npl_sem_pend: acquired in %u ms\n", (int)(mp_hal_ticks_ms() - t0));
    }
    sem->count -= 1;
    return BLE_NPL_OK;
}

ble_npl_error_t ble_npl_sem_release(struct ble_npl_sem *sem) {
    DEBUG_SEM_printf("ble_npl_sem_release(%p)\n", sem);
    sem->count += 1;
    return BLE_NPL_OK;
}

uint16_t ble_npl_sem_get_count(struct ble_npl_sem *sem) {
    DEBUG_SEM_printf("ble_npl_sem_get_count(%p)\n", sem);
    return sem->count;
}

/******************************************************************************/
// CALLOUT

static struct ble_npl_callout *global_callout = NULL;

void mp_bluetooth_nimble_os_callout_process(void) {
    os_sr_t sr;
    OS_ENTER_CRITICAL(sr);
    uint32_t tnow = mp_hal_ticks_ms();
    for (struct ble_npl_callout *c = global_callout; c != NULL; c = c->nextc) {
        if (!c->active) {
            continue;
        }
        if ((int32_t)(tnow - c->ticks) >= 0) {
            DEBUG_CALLOUT_printf("callout_run(%p) tnow=%u ticks=%u evq=%p\n", c, (uint)tnow, (uint)c->ticks, c->evq);
            c->active = false;
            if (c->evq) {
                // Enqueue this callout for execution in the event queue.
                ble_npl_eventq_put(c->evq, &c->ev);
            } else {
                // Execute this callout directly.
                OS_EXIT_CRITICAL(sr);
                c->ev.fn(&c->ev);
                OS_ENTER_CRITICAL(sr);
            }
            DEBUG_CALLOUT_printf("callout_run(%p) done\n", c);
        }
    }
    OS_EXIT_CRITICAL(sr);
}

void ble_npl_callout_init(struct ble_npl_callout *c, struct ble_npl_eventq *evq, ble_npl_event_fn *ev_cb, void *ev_arg) {
    DEBUG_CALLOUT_printf("ble_npl_callout_init(%p, %p, %p, %p)\n", c, evq, ev_cb, ev_arg);
    os_sr_t sr;
    OS_ENTER_CRITICAL(sr);
    c->active = false;
    c->ticks = 0;
    c->evq = evq;
    ble_npl_event_init(&c->ev, ev_cb, ev_arg);

    struct ble_npl_callout **c2;
    for (c2 = &global_callout; *c2 != NULL; c2 = &(*c2)->nextc) {
        if (c == *c2) {
            // callout already in linked list so don't link it in again
            OS_EXIT_CRITICAL(sr);
            return;
        }
    }
    *c2 = c;
    c->nextc = NULL;
    OS_EXIT_CRITICAL(sr);
}

ble_npl_error_t ble_npl_callout_reset(struct ble_npl_callout *c, ble_npl_time_t ticks) {
    DEBUG_CALLOUT_printf("ble_npl_callout_reset(%p, %u) tnow=%u\n", c, (uint)ticks, (uint)mp_hal_ticks_ms());
    os_sr_t sr;
    OS_ENTER_CRITICAL(sr);
    c->active = true;
    c->ticks = ble_npl_time_get() + ticks;
    OS_EXIT_CRITICAL(sr);
    return BLE_NPL_OK;
}

void ble_npl_callout_stop(struct ble_npl_callout *c) {
    DEBUG_CALLOUT_printf("ble_npl_callout_stop(%p)\n", c);
    c->active = false;
}

bool ble_npl_callout_is_active(struct ble_npl_callout *c) {
    DEBUG_CALLOUT_printf("ble_npl_callout_is_active(%p)\n", c);
    return c->active;
}

ble_npl_time_t ble_npl_callout_get_ticks(struct ble_npl_callout *c) {
    DEBUG_CALLOUT_printf("ble_npl_callout_get_ticks(%p)\n", c);
    return c->ticks;
}

ble_npl_time_t ble_npl_callout_remaining_ticks(struct ble_npl_callout *c, ble_npl_time_t now) {
    DEBUG_CALLOUT_printf("ble_npl_callout_remaining_ticks(%p, %u)\n", c, (uint)now);
    if (c->ticks > now) {
        return c->ticks - now;
    } else {
        return 0;
    }
}

void *ble_npl_callout_get_arg(struct ble_npl_callout *c) {
    DEBUG_CALLOUT_printf("ble_npl_callout_get_arg(%p)\n", c);
    return ble_npl_event_get_arg(&c->ev);
}

void ble_npl_callout_set_arg(struct ble_npl_callout *c, void *arg) {
    DEBUG_CALLOUT_printf("ble_npl_callout_set_arg(%p, %p)\n", c, arg);
    ble_npl_event_set_arg(&c->ev, arg);
}

/******************************************************************************/
// TIME

uint32_t ble_npl_time_get(void) {
    DEBUG_TIME_printf("ble_npl_time_get -> %u\n", (uint)mp_hal_ticks_ms());
    return mp_hal_ticks_ms();
}

ble_npl_error_t ble_npl_time_ms_to_ticks(uint32_t ms, ble_npl_time_t *out_ticks) {
    DEBUG_TIME_printf("ble_npl_time_ms_to_ticks(%u)\n", (uint)ms);
    *out_ticks = ms;
    return BLE_NPL_OK;
}

ble_npl_time_t ble_npl_time_ms_to_ticks32(uint32_t ms) {
    DEBUG_TIME_printf("ble_npl_time_ms_to_ticks32(%u)\n", (uint)ms);
    return ms;
}

uint32_t ble_npl_time_ticks_to_ms32(ble_npl_time_t ticks) {
    DEBUG_TIME_printf("ble_npl_time_ticks_to_ms32(%u)\n", (uint)ticks);
    return ticks;
}

void ble_npl_time_delay(ble_npl_time_t ticks) {
    mp_hal_delay_ms(ticks + 1);
}

/******************************************************************************/
// CRITICAL

// This is used anywhere NimBLE modifies global data structures.

// Currently all NimBLE code is invoked by the scheduler so there should be no
// races, so on STM32 MICROPY_PY_BLUETOOTH_ENTER/MICROPY_PY_BLUETOOTH_EXIT are
// no-ops. However, in the future we may wish to make HCI UART processing
// happen asynchronously (e.g. on RX IRQ), so the port can implement these
// macros accordingly.

uint32_t ble_npl_hw_enter_critical(void) {
    DEBUG_CRIT_printf("ble_npl_hw_enter_critical()\n");
    MICROPY_PY_BLUETOOTH_ENTER
    return atomic_state;
}

void ble_npl_hw_exit_critical(uint32_t atomic_state) {
    MICROPY_PY_BLUETOOTH_EXIT
    DEBUG_CRIT_printf("ble_npl_hw_exit_critical(%u)\n", (uint)atomic_state);
}
