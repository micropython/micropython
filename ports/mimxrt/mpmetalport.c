/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 Arduino SA
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
 * libmetal mimxrt port.
 */

#include "py/mperrno.h"
#include "py/mphal.h"

#include "fsl_common.h"
#include "fsl_mu.h"
#include CPU_HEADER_H

#include "metal/sys.h"
#include "metal/utilities.h"
#include "metal/device.h"

struct metal_state _metal;
static mp_sched_node_t rproc_notify_node;

#define IRQ_PRI_MU  NVIC_EncodePriority(NVIC_PRIORITYGROUP_4, 10, 0)

int metal_sys_init(const struct metal_init_params *params) {
    metal_unused(params);

    // Init MU.
    MU_Init(MUA);

    // Configure and enable MU IRQs.
    MU_ClearStatusFlags(MUA, kMU_GenInt0Flag);
    NVIC_SetPriority(MUA_IRQn, IRQ_PRI_MU);
    NVIC_EnableIRQ(MUA_IRQn);
    MU_EnableInterrupts(MUA, kMU_GenInt0InterruptEnable);

    #ifndef VIRTIO_USE_DCACHE
    // If cache management is not enabled, configure the MPU to disable caching
    // for the entire shared memory region.
    ARM_MPU_Disable();
    MPU->RBAR = ARM_MPU_RBAR(10, METAL_MPU_REGION_BASE);
    // Normal type, not shareable, non-cacheable
    MPU->RASR = ARM_MPU_RASR(0, ARM_MPU_AP_FULL, 1, 0, 0, 0, 0, METAL_MPU_REGION_SIZE);
    ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk | MPU_CTRL_HFNMIENA_Msk);
    #endif

    metal_bus_register(&metal_generic_bus);
    return 0;
}

void metal_sys_finish(void) {
    NVIC_DisableIRQ(MUA_IRQn);
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
    MU_TriggerInterrupts(MUA, kMU_GenInt0InterruptTrigger);
    return 0;
}

void MUA_IRQHandler(void) {
    if (MU_GetStatusFlags(MUA) & kMU_GenInt0Flag) {
        MU_ClearStatusFlags(MUA, kMU_GenInt0Flag);
        mp_sched_schedule_node(&rproc_notify_node, openamp_remoteproc_notified);
    }
    __DSB();
}
