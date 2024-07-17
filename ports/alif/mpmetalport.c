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
#include "hwsem.h"

#include "py/mperrno.h"
#include "py/mphal.h"

#include "metal/sys.h"
#include "metal/utilities.h"
#include "metal/device.h"

struct metal_state _metal;
static mp_sched_node_t rproc_notify_node;

int metal_sys_init(const struct metal_init_params *params) {
    metal_unused(params);

    // Reset the hardware semaphore.
    hwsem_reset(METAL_HSEM_DEVICE);
    #if MICROPY_PY_OPENAMP_HOST
    hwsem_reset(METAL_HSEM_REMOTE);
    #endif

    // Enable the hardware semaphore IRQ.
    NVIC_ClearPendingIRQ(METAL_HSEM_IRQn);
    NVIC_SetPriority(METAL_HSEM_IRQn, IRQ_PRI_HWSEM);
    NVIC_EnableIRQ(METAL_HSEM_IRQn);

    // If cache management is not enabled, configure the MPU to disable
    // caching for the entire Open-AMP shared memory region.
    #ifndef VIRTIO_USE_DCACHE
    ARM_MPU_Disable();
    // NOTE: The startup code uses the first 4 attributes.
    #define MEMATTR_IDX_NORMAL_NON_CACHEABLE      4
    ARM_MPU_SetMemAttr(MEMATTR_IDX_NORMAL_NON_CACHEABLE, ARM_MPU_ATTR(
        ARM_MPU_ATTR_NON_CACHEABLE,
        ARM_MPU_ATTR_NON_CACHEABLE));
    MPU->RNR = METAL_MPU_REGION_ID;
    MPU->RBAR = ARM_MPU_RBAR(METAL_MPU_REGION_BASE, ARM_MPU_SH_NON, 0, 1, 0); // RO-0, NP-1, XN-0
    MPU->RLAR = ARM_MPU_RLAR(METAL_MPU_REGION_BASE + METAL_MPU_REGION_SIZE - 1, MEMATTR_IDX_NORMAL_NON_CACHEABLE);
    ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk | MPU_CTRL_HFNMIENA_Msk);
    #endif

    metal_bus_register(&metal_generic_bus);

    return 0;
}

void metal_sys_finish(void) {
    NVIC_DisableIRQ(METAL_HSEM_IRQn);
    NVIC_ClearPendingIRQ(METAL_HSEM_IRQn);
    hwsem_reset(METAL_HSEM_DEVICE);
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
    // Release the HW semaphore to notify the other core.
    hwsem_release(METAL_HSEM_REMOTE, HWSEM_MASTERID);
    return 0;
}

void METAL_HSEM_IRQ_HANDLER(void) {
    // Schedule the node only if the other core released the Semaphore.
    if (METAL_HSEM_DEVICE->HWSEM_REL_REG == 0) {
        mp_sched_schedule_node(&rproc_notify_node, openamp_remoteproc_notified);
    }
    hwsem_request(METAL_HSEM_DEVICE, METAL_HSEM_REMOTE_ID);
}
