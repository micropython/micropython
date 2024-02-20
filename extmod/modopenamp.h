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
 * OpenAMP's Python module.
 */
#ifndef MICROPY_INCLUDED_MODOPENAMP_H
#define MICROPY_INCLUDED_MODOPENAMP_H

// Include a port config file if one is defined.
#ifdef MICROPY_PY_OPENAMP_CONFIG_FILE
#include MICROPY_PY_OPENAMP_CONFIG_FILE
#endif

// Use the default resource table layout and initialization code.
// Note ports and boards can override the default table and provide
// a custom one in openamp_rsc_table_t and openamp_rsc_table_init()
#ifndef MICROPY_PY_OPENAMP_RSC_TABLE_ENABLE
#define MICROPY_PY_OPENAMP_RSC_TABLE_ENABLE   (1)
#endif

// This enables a trace buffer that can be used by remote processor for
// writing trace logs. This is enabled by default to increase the default
// resource table's compatibility with common OpenAMP examples.
#ifndef MICROPY_PY_OPENAMP_TRACE_BUF_ENABLE
#define MICROPY_PY_OPENAMP_TRACE_BUF_ENABLE (1)
#endif

// For ports that don't define a custom image store, this enables a generic
// VFS-based image store that supports loading elf files from storage.
#ifndef MICROPY_PY_OPENAMP_REMOTEPROC_STORE_ENABLE
#define MICROPY_PY_OPENAMP_REMOTEPROC_STORE_ENABLE (1)
#endif

// Enable or disable support for loading elf files. This option saves
// around 7KBs when disabled.
#ifndef MICROPY_PY_OPENAMP_REMOTEPROC_ELFLD_ENABLE
#define MICROPY_PY_OPENAMP_REMOTEPROC_ELFLD_ENABLE (1)
#endif

// The resource table is used for sharing the configuration of the virtio
// device, vrings and other resources, between the host and remote cores.
// The layout and address the table structure must match the one used in
// the remote processor's firmware.
#if MICROPY_PY_OPENAMP_RSC_TABLE_ENABLE
typedef struct openamp_rsc_table {
    unsigned int version;
    unsigned int num;
    unsigned int reserved[2];
    #if MICROPY_PY_OPENAMP_TRACE_BUF_ENABLE
    unsigned int offset[2];
    #else
    unsigned int offset[1];
    #endif
    struct fw_rsc_vdev vdev;
    struct fw_rsc_vdev_vring vring0;
    struct fw_rsc_vdev_vring vring1;
    #if MICROPY_PY_OPENAMP_TRACE_BUF_ENABLE
    struct fw_rsc_trace trace;
    #endif
} openamp_rsc_table_t;
#endif // MICROPY_PY_OPENAMP_RSC_TABLE_ENABLE

// Performs low-level initialization of OpenAMP, such as initializing libmetal,
// the shared resource table, shared memory regions, vrings and virtio device.
void openamp_init(void);

// Ports should run this callback in scheduler context, when
// the remote processor notifies the host of pending messages.
void openamp_remoteproc_notified(mp_sched_node_t *node);

#endif // MICROPY_INCLUDED_MODOPENAMP_H
