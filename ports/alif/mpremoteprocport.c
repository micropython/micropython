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
 * modremoteproc alif port.
 */

#include <stdio.h>
#include <stdint.h>

#include "py/obj.h"
#include "py/runtime.h"

#include "metal/alloc.h"
#include "metal/errno.h"
#include "metal/io.h"
#include "metal/sys.h"
#include "metal/device.h"
#include "metal/utilities.h"
#include "extmod/modopenamp_remoteproc.h"
#include "se_services.h"

typedef struct mmap {
    uintptr_t base;
    uintptr_t limit;
} mmap_t;

static const mmap_t mmap_nocache[] = {
    { .base = ITCM_BASE, .limit = ITCM_BASE + ITCM_SIZE },
    { .base = DTCM_BASE, .limit = DTCM_BASE + ITCM_SIZE },
    { .base = MRAM_BASE, .limit = MRAM_BASE + MRAM_SIZE }
};

static bool is_cacheable(const void *p, size_t bytes) {
    uintptr_t base = (uintptr_t)p;
    if (bytes == 0) {
        return false;
    }
    uintptr_t limit = base + bytes;
    for (unsigned int i = 0; i < sizeof(mmap_nocache) / sizeof(mmap_nocache[0]); i++) {
        if (base >= mmap_nocache[i].base && limit < mmap_nocache[i].limit) {
            return false;
        }
    }
    return true;
}

struct remoteproc *mp_openamp_remoteproc_init(struct remoteproc *rproc,
    const struct remoteproc_ops *ops, void *arg) {
    metal_log(METAL_LOG_DEBUG, "rproc_init()\n");

    rproc->ops = ops;
    rproc->state = RPROC_OFFLINE;
    // Allocate the image store and save it in private data.
    rproc->priv = mp_openamp_remoteproc_store_alloc();

    // Reset the remote core.
    se_services_boot_reset_cpu(EXTSYS_1);
    return rproc;
}

void *mp_openamp_remoteproc_mmap(struct remoteproc *rproc, metal_phys_addr_t *pa,
    metal_phys_addr_t *da, size_t size, unsigned int attribute,
    struct metal_io_region **io) {
    metal_log(METAL_LOG_DEBUG, "rproc_mmap(): pa 0x%p da 0x%p io 0x%p size %u\n", *pa, *da, *io, size);

    struct remoteproc_mem *mem;
    metal_phys_addr_t lpa = *pa;
    metal_phys_addr_t lda = *da;

    if (lda == METAL_BAD_PHYS) {
        return NULL;
    }

    if (lpa == METAL_BAD_PHYS) {
        lpa = lda;
    }

    // Currently this port doesn't support loading firmware to flash,
    // only SD/SRAM images are supported. Check of load address is in
    // the flash region, and if so return NULL.
    if (lda >= MRAM_BASE && lda < (MRAM_BASE + MRAM_SIZE)) {
        return NULL;
    }

    mem = metal_allocate_memory(sizeof(*mem));
    if (!mem) {
        return NULL;
    }

    *io = metal_allocate_memory(sizeof(struct metal_io_region));
    if (!*io) {
        metal_free_memory(mem);
        return NULL;
    }

    remoteproc_init_mem(mem, NULL, lpa, lda, size, *io);

    metal_io_init(*io, (void *)mem->da, &mem->pa, size,
        sizeof(metal_phys_addr_t) << 3, attribute, NULL);

    remoteproc_add_mem(rproc, mem);
    *pa = lpa;
    *da = lda;
    return metal_io_phys_to_virt(*io, mem->pa);
}

int mp_openamp_remoteproc_start(struct remoteproc *rproc) {
    metal_log(METAL_LOG_DEBUG, "rproc_start()\n");

    // Flush cached areas for the remote core.
    struct metal_list *node;
    metal_list_for_each(&rproc->mems, node) {
        struct remoteproc_mem *mem;
        mem = metal_container_of(node, struct remoteproc_mem, node);
        if (is_cacheable((uint32_t *)mem->pa, mem->size)) {
            SCB_CleanDCache_by_Addr((uint32_t *)mem->pa, mem->size);
        }
    }

    se_services_boot_reset_cpu(EXTSYS_1);
    se_services_boot_cpu(EXTSYS_1, (uint32_t)rproc->bootaddr);  // GlobalToLocal
    return 0;
}

int mp_openamp_remoteproc_stop(struct remoteproc *rproc) {
    metal_log(METAL_LOG_DEBUG, "rproc_stop()\n");
    if (rproc->state == RPROC_RUNNING) {
        se_services_boot_reset_cpu(EXTSYS_1);
    }
    return 0;
}

int mp_openamp_remoteproc_config(struct remoteproc *rproc, void *data) {
    metal_log(METAL_LOG_DEBUG, "rproc_config()\n");
    (void)rproc;
    return 0;
}

void mp_openamp_remoteproc_remove(struct remoteproc *rproc) {
    metal_log(METAL_LOG_DEBUG, "rproc_remove()\n");
    (void)rproc;
}

int mp_openamp_remoteproc_shutdown(struct remoteproc *rproc) {
    metal_log(METAL_LOG_DEBUG, "rproc_shutdown()\n");
    if (rproc->state == RPROC_RUNNING) {
        se_services_boot_reset_cpu(EXTSYS_1);
    }
    return 0;
}
