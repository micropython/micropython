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
#include "nimble/nimble_npl.h"

#define DEBUG_OS_printf(...) //printf(__VA_ARGS__)
#define DEBUG_EVENT_printf(...) //printf(__VA_ARGS__)
#define DEBUG_MUTEX_printf(...) //printf(__VA_ARGS__)
#define DEBUG_SEM_printf(...) //printf(__VA_ARGS__)
#define DEBUG_CALLOUT_printf(...) //printf(__VA_ARGS__)
#define DEBUG_TIME_printf(...) //printf(__VA_ARGS__)
#define DEBUG_CRIT_printf(...) //printf(__VA_ARGS__)

bool ble_npl_os_started(void) {
    DEBUG_OS_printf("ble_npl_os_started\n");
    return true;
}

void *ble_npl_get_current_task_id(void) {
    DEBUG_OS_printf("ble_npl_get_current_task_id\n");
    return NULL;
}

/******************************************************************************/
// EVENTQ

struct ble_npl_eventq *global_eventq = NULL;

void os_eventq_run_all(void) {
    for (struct ble_npl_eventq *evq = global_eventq; evq != NULL; evq = evq->nextq) {
        while (evq->head != NULL) {
            struct ble_npl_event *ev = evq->head;
            evq->head = ev->next;
            if (ev->next) {
                ev->next->prev = NULL;
                ev->next = NULL;
            }
            ev->prev = NULL;
            DEBUG_EVENT_printf("event_run(%p)\n", ev);
            ev->fn(ev);
            DEBUG_EVENT_printf("event_run(%p) done\n", ev);
        }
    }
}

void ble_npl_eventq_init(struct ble_npl_eventq *evq) {
    DEBUG_EVENT_printf("ble_npl_eventq_init(%p)\n", evq);
    evq->head = NULL;
    struct ble_npl_eventq **evq2;
    for (evq2 = &global_eventq; *evq2 != NULL; evq2 = &(*evq2)->nextq) {
    }
    *evq2 = evq;
    evq->nextq = NULL;
}

void ble_npl_eventq_put(struct ble_npl_eventq *evq, struct ble_npl_event *ev) {
    DEBUG_EVENT_printf("ble_npl_eventq_put(%p, %p (%p, %p))\n", evq, ev, ev->fn, ev->arg);
    ev->next = NULL;
    if (evq->head == NULL) {
        evq->head = ev;
        ev->prev = NULL;
    } else {
        struct ble_npl_event *ev2 = evq->head;
        while (true) {
            if (ev2 == ev) {
                DEBUG_EVENT_printf("  --> already in queue\n");
                return;
            }
            if (ev2->next == NULL) {
                break;
            }
            DEBUG_EVENT_printf("  --> %p\n", ev2->next);
            ev2 = ev2->next;
        }
        ev2->next = ev;
        ev->prev = ev2;
    }
}

void ble_npl_event_init(struct ble_npl_event *ev, ble_npl_event_fn *fn, void *arg) {
    DEBUG_EVENT_printf("ble_npl_event_init(%p, %p, %p)\n", ev, fn, arg);
    ev->fn = fn;
    ev->arg = arg;
    ev->next = NULL;
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
    mu->locked = 1;
    return BLE_NPL_OK;
}

ble_npl_error_t ble_npl_mutex_release(struct ble_npl_mutex *mu) {
    DEBUG_MUTEX_printf("ble_npl_mutex_release(%p) locked=%u\n", mu, (uint)mu->locked);
    mu->locked = 0;
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
    if (sem->count == 0) {
        uint32_t t0 = mp_hal_ticks_ms();
        while (sem->count == 0 && mp_hal_ticks_ms() - t0 < timeout) {
            extern void nimble_uart_process(void);
            nimble_uart_process();
            if (sem->count != 0) {
                break;
            }
            __WFI();
        }
        if (sem->count == 0) {
            printf("timeout\n");
            return BLE_NPL_TIMEOUT;
        }
        DEBUG_SEM_printf("got response in %u ms\n", (int)(mp_hal_ticks_ms() - t0));
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

void os_callout_process(void) {
    uint32_t tnow = mp_hal_ticks_ms();
    for (struct ble_npl_callout *c = global_callout; c != NULL; c = c->nextc) {
        if (!c->active) {
            continue;
        }
        if ((int32_t)(tnow - c->ticks) >= 0) {
            DEBUG_CALLOUT_printf("callout_run(%p) tnow=%u ticks=%u evq=%p\n", c, (uint)tnow, (uint)c->ticks, c->evq);
            c->active = false;
            if (c->evq) {
                ble_npl_eventq_put(c->evq, &c->ev);
            } else {
                c->ev.fn(&c->ev);
            }
            DEBUG_CALLOUT_printf("callout_run(%p) done\n", c);
        }
    }
}

void ble_npl_callout_init(struct ble_npl_callout *c, struct ble_npl_eventq *evq, ble_npl_event_fn *ev_cb, void *ev_arg) {
    DEBUG_CALLOUT_printf("ble_npl_callout_init(%p, %p, %p, %p)\n", c, evq, ev_cb, ev_arg);
    c->active = false;
    c->ticks = 0;
    c->evq = evq;
    ble_npl_event_init(&c->ev, ev_cb, ev_arg);

    struct ble_npl_callout **c2;
    for (c2 = &global_callout; *c2 != NULL; c2 = &(*c2)->nextc) {
        if (c == *c2) {
            // callout already in linked list so don't link it in again
            return;
        }
    }
    *c2 = c;
    c->nextc = NULL;
}

ble_npl_error_t ble_npl_callout_reset(struct ble_npl_callout *c, ble_npl_time_t ticks) {
    DEBUG_CALLOUT_printf("ble_npl_callout_reset(%p, %u) tnow=%u\n", c, (uint)ticks, (uint)mp_hal_ticks_ms());
    c->active = true;
    c->ticks = ble_npl_time_get() + ticks;
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

uint32_t ble_npl_hw_enter_critical(void) {
    DEBUG_CRIT_printf("ble_npl_hw_enter_critical()\n");
    return raise_irq_pri(15);
}

void ble_npl_hw_exit_critical(uint32_t ctx) {
    DEBUG_CRIT_printf("ble_npl_hw_exit_critical(%u)\n", (uint)ctx);
    restore_irq_pri(ctx);
}
