/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Arduino SA
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
 * libmetal stm32 port.
 */

#include "py/mperrno.h"
#include "py/mphal.h"

#include "mpu.h"

#include "metal/sys.h"
#include "metal/utilities.h"
#include "metal/device.h"

struct metal_state _metal;
static mp_sched_node_t rproc_notify_node;

int metal_sys_init(const struct metal_init_params *params) {
    metal_unused(params);

    // Clear HSEM pending IRQ.
    HSEM_COMMON->ICR |= (uint32_t)__HAL_HSEM_SEMID_TO_MASK(METAL_HSEM_MASTER_ID);
    HAL_NVIC_ClearPendingIRQ(HSEM1_IRQn);

    // Enable and configure HSEM.
    __HAL_RCC_HSEM_CLK_ENABLE();
    NVIC_SetPriority(HSEM1_IRQn, IRQ_PRI_HSEM);
    HAL_NVIC_EnableIRQ(HSEM1_IRQn);
    HAL_HSEM_ActivateNotification(__HAL_HSEM_SEMID_TO_MASK(METAL_HSEM_MASTER_ID));

    #ifndef VIRTIO_USE_DCACHE
    // If cache management is not enabled, configure the MPU to disable caching
    // for the entire shared memory region.
    uint32_t irq_state = mpu_config_start();
    mpu_config_region(MPU_REGION_OPENAMP, METAL_MPU_REGION_BASE, MPU_CONFIG_SHARED_UNCACHED(METAL_MPU_REGION_SIZE));
    mpu_config_end(irq_state);
    #endif

    metal_bus_register(&metal_generic_bus);
    return 0;
}

void metal_sys_finish(void) {
    HAL_NVIC_DisableIRQ(HSEM1_IRQn);
    HAL_HSEM_DeactivateNotification(__HAL_HSEM_SEMID_TO_MASK(METAL_HSEM_MASTER_ID));
    __HAL_RCC_HSEM_CLK_DISABLE();
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
    HAL_HSEM_FastTake(METAL_HSEM_REMOTE_ID);
    HAL_HSEM_Release(METAL_HSEM_REMOTE_ID, 0);
    return 0;
}

void HSEM1_IRQHandler(void) {
    HAL_HSEM_IRQHandler();
    mp_sched_schedule_node(&rproc_notify_node, openamp_remoteproc_notified);
    HAL_HSEM_ActivateNotification(__HAL_HSEM_SEMID_TO_MASK(METAL_HSEM_MASTER_ID));
}
