/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023-2024 Arduino SA
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
 * OpenAMP's remoteproc class.
 */
#ifndef MICROPY_INCLUDED_MODOPENAMP_REMOTEPROC_H
#define MICROPY_INCLUDED_MODOPENAMP_REMOTEPROC_H

#include "openamp/remoteproc.h"
#include "openamp/remoteproc_loader.h"

void *mp_openamp_remoteproc_store_alloc(void);
struct remoteproc *mp_openamp_remoteproc_init(struct remoteproc *rproc,
    const struct remoteproc_ops *ops, void *arg);
void *mp_openamp_remoteproc_mmap(struct remoteproc *rproc, metal_phys_addr_t *pa,
    metal_phys_addr_t *da, size_t size, unsigned int attribute,
    struct metal_io_region **io);
int mp_openamp_remoteproc_start(struct remoteproc *rproc);
int mp_openamp_remoteproc_stop(struct remoteproc *rproc);
int mp_openamp_remoteproc_config(struct remoteproc *rproc, void *data);
void mp_openamp_remoteproc_remove(struct remoteproc *rproc);
int mp_openamp_remoteproc_shutdown(struct remoteproc *rproc);

#endif // MICROPY_INCLUDED_MODOPENAMP_REMOTEPROC_H
