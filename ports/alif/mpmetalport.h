/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 OpenMV LLC.
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
 * libmetal alif port.
 */
#ifndef MICROPY_INCLUDED_ALIF_MPMETALPORT_H
#define MICROPY_INCLUDED_ALIF_MPMETALPORT_H

#include <stdlib.h>
#include "py/mphal.h"
#include "py/runtime.h"
#include "mpu.h"

#define METAL_HAVE_STDATOMIC_H      0
#define METAL_HAVE_FUTEX_H          0

#define METAL_MAX_DEVICE_REGIONS    2

// Set to 1 to enable log output.
#define METAL_LOG_HANDLER_ENABLE    0

#define metal_cpu_yield()

// Shared memory config
#define METAL_SHM_NAME              "OPENAMP_SHM"
// Note 1K must be reserved at the start of the openamp
// shared memory region, for the shared resource table.
#define METAL_RSC_ADDR              ((void *)_openamp_shm_region_start)
#define METAL_RSC_SIZE              (1024)

#define METAL_SHM_ADDR              ((metal_phys_addr_t)(_openamp_shm_region_start + METAL_RSC_SIZE))
#define METAL_SHM_SIZE              ((size_t)(_openamp_shm_region_end - _openamp_shm_region_start - METAL_RSC_SIZE))

#define METAL_MPU_REGION_ID         (MP_MPU_REGION_OPENAMP)
#define METAL_MPU_REGION_BASE       ((uint32_t)_openamp_shm_region_start)
#define METAL_MPU_REGION_SIZE       (0x00010000U)

extern const char _openamp_shm_region_start[];
extern const char _openamp_shm_region_end[];

int metal_rproc_notify(void *priv, uint32_t id);
extern void openamp_remoteproc_notified(mp_sched_node_t *node);

static inline int __metal_sleep_usec(unsigned int usec) {
    mp_hal_delay_us(usec);
    return 0;
}

static inline void metal_generic_default_poll(void) {
    mp_event_handle_nowait();
    __WFI();
}

#endif // MICROPY_INCLUDED_ALIF_METAL_PORT_H
