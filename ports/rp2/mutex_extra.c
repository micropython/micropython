/*
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "mutex_extra.h"

// These functions are taken from lib/pico-sdk/src/common/pico_sync/mutex.c and modified
// so that they atomically obtain the mutex and disable interrupts.

uint32_t __time_critical_func(mutex_enter_blocking_and_disable_interrupts)(mutex_t * mtx) {
    lock_owner_id_t caller = lock_get_caller_owner_id();
    do {
        uint32_t save = spin_lock_blocking(mtx->core.spin_lock);
        if (!lock_is_owner_id_valid(mtx->owner)) {
            mtx->owner = caller;
            spin_unlock_unsafe(mtx->core.spin_lock);
            return save;
        }
        lock_internal_spin_unlock_with_wait(&mtx->core, save);
    } while (true);
}

void __time_critical_func(mutex_exit_and_restore_interrupts)(mutex_t * mtx, uint32_t save) {
    spin_lock_unsafe_blocking(mtx->core.spin_lock);
    assert(lock_is_owner_id_valid(mtx->owner));
    mtx->owner = LOCK_INVALID_OWNER_ID;
    lock_internal_spin_unlock_with_notify(&mtx->core, save);
}
