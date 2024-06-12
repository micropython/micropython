/*
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "mutex_extra.h"

// These functions are taken from lib/pico-sdk/src/common/pico_sync/mutex.c and modified
// so that they atomically obtain the mutex and disable interrupts.

uint32_t __time_critical_func(recursive_mutex_enter_blocking_and_disable_interrupts)(recursive_mutex_t * mtx) {
    lock_owner_id_t caller = lock_get_caller_owner_id();
    do {
        uint32_t save = spin_lock_blocking(mtx->core.spin_lock);
        if (mtx->owner == caller || !lock_is_owner_id_valid(mtx->owner)) {
            mtx->owner = caller;
            uint __unused total = ++mtx->enter_count;
            spin_unlock_unsafe(mtx->core.spin_lock);
            assert(total); // check for overflow
            return save;
        }
        lock_internal_spin_unlock_with_wait(&mtx->core, save);
    } while (true);
}

void __time_critical_func(recursive_mutex_exit_and_restore_interrupts)(recursive_mutex_t * mtx, uint32_t save) {
    spin_lock_unsafe_blocking(mtx->core.spin_lock);
    assert(lock_is_owner_id_valid(mtx->owner));
    assert(mtx->enter_count);
    if (!--mtx->enter_count) {
        mtx->owner = LOCK_INVALID_OWNER_ID;
    }
    lock_internal_spin_unlock_with_notify(&mtx->core, save);
}
