/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
<<<<<<<< HEAD:ports/raspberrypi/common-hal/wifi/Monitor.c
 * Copyright (c) 2021 microDev
========
 * Copyright (c) 2013, 2014 Damien P. George
>>>>>>>> v1.20.0:ports/samd/pendsv.c
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

<<<<<<<< HEAD:ports/raspberrypi/common-hal/wifi/Monitor.c
#include <string.h>

#include "py/mpstate.h"
#include "py/runtime.h"

#include "shared-bindings/wifi/Monitor.h"
#include "shared-bindings/wifi/Packet.h"


#define MONITOR_PAYLOAD_FCS_LEN     (4)
#define MONITOR_QUEUE_TIMEOUT_TICK  (0)

typedef struct {
} monitor_packet_t;

void common_hal_wifi_monitor_construct(wifi_monitor_obj_t *self, uint8_t channel, size_t queue) {
    mp_raise_NotImplementedError(translate("wifi.Monitor not available"));
}

bool common_hal_wifi_monitor_deinited(void) {
    return true;
}

void common_hal_wifi_monitor_deinit(wifi_monitor_obj_t *self) {
}

void common_hal_wifi_monitor_set_channel(wifi_monitor_obj_t *self, uint8_t channel) {
}

mp_obj_t common_hal_wifi_monitor_get_channel(wifi_monitor_obj_t *self) {
    return MP_OBJ_NEW_SMALL_INT(0);
}

mp_obj_t common_hal_wifi_monitor_get_queue(wifi_monitor_obj_t *self) {
    return mp_obj_new_int_from_uint(0);
}

mp_obj_t common_hal_wifi_monitor_get_lost(wifi_monitor_obj_t *self) {
    return mp_obj_new_int_from_uint(0);
}

mp_obj_t common_hal_wifi_monitor_get_queued(wifi_monitor_obj_t *self) {
    return mp_obj_new_int_from_uint(0);
}

mp_obj_t common_hal_wifi_monitor_get_packet(wifi_monitor_obj_t *self) {
    return mp_const_none;
}
========
#include <stdlib.h>

#include "py/runtime.h"
#include "shared/runtime/interrupt_char.h"
#include "sam.h"
#include "pendsv.h"


#if defined(PENDSV_DISPATCH_NUM_SLOTS)
uint32_t pendsv_dispatch_active;
pendsv_dispatch_t pendsv_dispatch_table[PENDSV_DISPATCH_NUM_SLOTS];
#endif

void pendsv_init(void) {
    #if defined(PENDSV_DISPATCH_NUM_SLOTS)
    pendsv_dispatch_active = false;
    #endif

    // set PendSV interrupt at lowest priority
    NVIC_SetPriority(PendSV_IRQn, IRQ_PRI_PENDSV);
}

#if defined(PENDSV_DISPATCH_NUM_SLOTS)
void pendsv_schedule_dispatch(size_t slot, pendsv_dispatch_t f) {
    pendsv_dispatch_table[slot] = f;
    pendsv_dispatch_active = true;
    SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
}

void pendsv_dispatch_handler(void) {
    for (size_t i = 0; i < PENDSV_DISPATCH_NUM_SLOTS; ++i) {
        if (pendsv_dispatch_table[i] != NULL) {
            pendsv_dispatch_t f = pendsv_dispatch_table[i];
            pendsv_dispatch_table[i] = NULL;
            f();
        }
    }
}

void PendSV_Handler(void) {
    if (pendsv_dispatch_active) {
        pendsv_dispatch_handler();
    }
}
#endif
>>>>>>>> v1.20.0:ports/samd/pendsv.c
