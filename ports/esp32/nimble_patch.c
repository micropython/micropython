/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Mindhash B.V.
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

#include "py/mpconfig.h"

#if MICROPY_PY_BLUETOOTH

#include "nimble/ble.h"
#include "nimble/host/src/ble_hs_resolv_priv.h"

#if MYNEWT_VAL(BLE_HOST_BASED_PRIVACY)

// This is a link-time patch to correctly resolve peer RPA using a stored peer
// IRK if using host-based privacy. When a peer connects, if their address is a
// RPA we should try to resolve it using all known peer IRK's, and if we find a
// match we know it's identity address.
//
// This is implemented by `ble_hs_resolv_rpa_addr()`: It iterates over entries
// in `g_ble_hs_resolv_list`, looking for the first entry with an IRK that can
// resolve the given RPA, and once found it saves the RPA in the `rl_peer_rpa`
// field of the entry. Then any next time the RPA needs to be resolved the code
// simply searches `ble_hs_resolv_list_find` using the RPA (instead of doing a
// cryptographic operation with the IRK every time).
//
// However, it appears in IDFv5.5.4 (and other versions) this function is never
// called on connection, meaning the `rl_peer_rpa` field doesn't get updated and
// subsequently the RPA cannot be resolved properly. Instead another function is
// called, one that is tightly coupled to the BLE NVS store, which in we do not
// use in MicroPython because we want to use our own store callbacks.
//
// Thus we wrap the call to that function and call `ble_hs_resolv_rpa_addr()` in
// the wrapper to ensure RPA resolving works correctly, and update the address
// type if resolved successfully since a resolved address is always public.

void __real_ble_rpa_replace_peer_params_with_rl(uint8_t *, uint8_t *, struct ble_hs_resolv_entry **);

void __wrap_ble_rpa_replace_peer_params_with_rl(uint8_t *peer_addr, uint8_t *addr_type, struct ble_hs_resolv_entry **rl) {
    if (rl != NULL) {
        *rl = ble_hs_resolv_rpa_addr(peer_addr, *addr_type);
        if (*rl != NULL) {
            (*rl)->rl_addr_type = BLE_ADDR_PUBLIC;
        }
    }
    __real_ble_rpa_replace_peer_params_with_rl(peer_addr, addr_type, rl);
}
#endif
#endif
