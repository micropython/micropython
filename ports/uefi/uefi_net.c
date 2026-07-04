/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Nicko van Someren
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

// Shared C plumbing for the UEFI `network`/`socket` stack: the async completion
// token/event waiter (the bridge to the port's WFE / asyncio / Ctrl-C machinery)
// and the service-binding child lifecycle. See README.md.

#include "py/runtime.h"
#include "py/mphal.h"
#include "uefi_port.h"
#include "efi.h"
#include "efi_net.h"

#define BS (mp_uefi_st->BootServices)

// Global DNS-server override (network.ipconfig(dns=...)). When set it takes
// precedence over the DHCP-provided resolver for getaddrinfo — the resolver the
// user picked, and the seam the test uses to point at a reachable DNS.
static UINT8 g_dns_override[4];
static int g_dns_override_set;

void net_set_dns_override(const UINT8 ip[4]) {
    g_dns_override[0] = ip[0];
    g_dns_override[1] = ip[1];
    g_dns_override[2] = ip[2];
    g_dns_override[3] = ip[3];
    g_dns_override_set = 1;
}

int net_get_dns_override(UINT8 out[4]) {
    if (!g_dns_override_set) {
        return 0;
    }
    out[0] = g_dns_override[0];
    out[1] = g_dns_override[1];
    out[2] = g_dns_override[2];
    out[3] = g_dns_override[3];
    return 1;
}

// The completion notify runs as a hard ISR (TPL_CALLBACK): it only flips the flag
// and nudges the idle loop — never touches the VM or Boot Services (the TPL rule).
static void EFIAPI net_op_notify(EFI_EVENT event, void *context) {
    (void)event;
    net_op_t *op = (net_op_t *)context;
    op->done = 1;
    mp_uefi_wake();
}

EFI_STATUS net_op_init(net_op_t *op) {
    op->done = 0;
    op->event = NULL;
    return mp_uefi_create_signal_event(net_op_notify, op, &op->event);
}

int net_op_wait(net_op_t *op, int timeout_ms) {
    uint64_t start = mp_hal_ticks_ms();
    while (!op->done) {
        // Process the interrupt char etc.; a pending Ctrl-C raises out of here.
        mp_handle_pending(true);
        if (op->done) {
            break;
        }
        if (timeout_ms >= 0) {
            uint64_t elapsed = mp_hal_ticks_ms() - start;
            if (elapsed >= (uint64_t)timeout_ms) {
                return -1;
            }
        }
        // Sleep in the port's WFE. The completion notify calls mp_uefi_wake() to
        // return us early; the 20 ms cap lets the firmware's network timer (which
        // drives the stack's state machines) run and keeps timeout granularity tight.
        mp_uefi_wfe(20);
    }
    return 0;
}

void net_op_close(net_op_t *op) {
    if (op->event != NULL) {
        mp_uefi_close_event(op->event);
        op->event = NULL;
    }
}

EFI_STATUS net_sb_create_child(EFI_HANDLE nic, const EFI_GUID *sb_guid, EFI_HANDLE *child_out) {
    EFI_SERVICE_BINDING_PROTOCOL *sb = NULL;
    EFI_STATUS st = BS->HandleProtocol(nic, (EFI_GUID *)sb_guid, (void **)&sb);
    if (EFI_ERROR(st)) {
        return st;
    }
    *child_out = NULL;
    return sb->CreateChild(sb, child_out);
}

EFI_STATUS net_sb_destroy_child(EFI_HANDLE nic, const EFI_GUID *sb_guid, EFI_HANDLE child) {
    EFI_SERVICE_BINDING_PROTOCOL *sb = NULL;
    EFI_STATUS st = BS->HandleProtocol(nic, (EFI_GUID *)sb_guid, (void **)&sb);
    if (EFI_ERROR(st)) {
        return st;
    }
    return sb->DestroyChild(sb, child);
}
