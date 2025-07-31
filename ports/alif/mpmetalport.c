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
 * libmetal Alif port.
 */

#include ALIF_CMSIS_H
#include "py/mperrno.h"
#include "py/mphal.h"

#include "metal/sys.h"
#include "metal/utilities.h"
#include "metal/device.h"

#include "se_services.h"

struct metal_state _metal;

static bool metal_active;
static mp_sched_node_t rproc_notify_node;

int metal_sys_init(const struct metal_init_params *params) {
    metal_unused(params);

    // If cache management is not enabled, configure the MPU to disable
    // caching for the entire Open-AMP shared memory region.
    #ifndef VIRTIO_USE_DCACHE
    ARM_MPU_Disable();
    MPU->RNR = METAL_MPU_REGION_ID;
    MPU->RBAR = ARM_MPU_RBAR(METAL_MPU_REGION_BASE, ARM_MPU_SH_NON, 0, 1, 1); // RO-0, NP-1, XN-1
    MPU->RLAR = ARM_MPU_RLAR(METAL_MPU_REGION_BASE + METAL_MPU_REGION_SIZE - 1, MP_MPU_ATTR_NORMAL_NON_CACHEABLE);
    ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk | MPU_CTRL_HFNMIENA_Msk);
    #endif

    metal_bus_register(&metal_generic_bus);
    metal_active = true;

    return 0;
}

void metal_sys_finish(void) {
    metal_active = false;
    metal_bus_unregister(&metal_generic_bus);
}

unsigned int sys_irq_save_disable(void) {
    return disable_irq();
}

void sys_irq_restore_enable(unsigned int state) {
    enable_irq(state);
}

void *metal_machine_io_mem_map(void *va, metal_phys_addr_t pa,
    size_t size, unsigned int flags) {
    metal_unused(pa);
    metal_unused(size);
    metal_unused(flags);
    return va;
}

void metal_machine_cache_flush(void *addr, unsigned int len) {
    SCB_CleanDCache_by_Addr(addr, len);
}

void metal_machine_cache_invalidate(void *addr, unsigned int len) {
    SCB_InvalidateDCache_by_Addr(addr, len);
}

int metal_rproc_notify(void *priv, uint32_t id) {
    // Notify the remote core.
    se_services_notify();
    return 0;
}

void metal_rproc_notified(void) {
    if (!metal_active) {
        return;
    }
    // The remote core notified this core.
    mp_sched_schedule_node(&rproc_notify_node, openamp_remoteproc_notified);
}
