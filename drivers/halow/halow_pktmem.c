/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 OpenMV LLC.
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
 * Packet memory for the Morse Micro 802.11ah driver.
 *
 * This is the same design as the SDK's heap-backed mmpktmem, with one change:
 * the reserved command pools are taken from the driver's memory pool at init
 * rather than living in .bss, so that the driver claims no static RAM.  The
 * pools exist so that command traffic to and from the transceiver keeps working
 * when the pool is too fragmented to satisfy a data allocation.
 */

#include "py/mphal.h"

#if MICROPY_PY_NETWORK_HALOW

#include <stdatomic.h>
#include <string.h>

#include "mmhal_wlan.h"
#include "mmosal.h"
#include "mmpkt.h"
#include "mmpkt_list.h"

#include "halow_osal.h"

// Reserved buffers for commands to the transceiver.  Commands are small and
// there are never many outstanding at once.
#define HALOW_TX_COMMAND_BLOCK_SIZE (352)
#define HALOW_TX_COMMAND_N_BLOCKS   (2)

// Reserved buffers for command responses, which arrive in full sized packets.
#define HALOW_RX_COMMAND_BLOCK_SIZE (MMHAL_WLAN_MMPKT_RX_MAX_SIZE)
#define HALOW_RX_COMMAND_N_BLOCKS   (2)

// Upper bounds on concurrently allocated data packets, which come straight from
// the driver's pool.  These are what actually bound the driver's memory use.
//
// The transmit bound is also the transmit window: morselib stops accepting
// packets at HALOW_TX_PAUSE_THRESHOLD, and a sender then waits for the queue to
// drain rather than filling it.  Measured on an N6 at MCS7/8MHz, raising this
// from 16 to 32 took throughput from ~2.4 to ~5.5 Mbit/s and removed the stalls
// entirely; 48 gained nothing further.  Blocks are counted rather than pooled,
// so a larger bound costs nothing until the traffic uses it.
#ifndef MICROPY_HW_HALOW_TX_BLOCKS
#define MICROPY_HW_HALOW_TX_BLOCKS  (32)
#endif
#ifndef MICROPY_HW_HALOW_RX_BLOCKS
#define MICROPY_HW_HALOW_RX_BLOCKS  (16)
#endif

// Transmission is paused when all but one of the TX allocations are in use, and
// resumed once it drops back below the second threshold.
#define HALOW_TX_PAUSE_THRESHOLD    (MICROPY_HW_HALOW_TX_BLOCKS - 1)
#define HALOW_TX_UNPAUSE_THRESHOLD  (MICROPY_HW_HALOW_TX_BLOCKS - 2)

#if MICROPY_HW_HALOW_TX_BLOCKS < 3 || MICROPY_HW_HALOW_RX_BLOCKS < 2
#error "halow: the packet pools are too small to flow control"
#endif

typedef struct _halow_pktmem_t {
    // Data packets in flight, counted rather than pooled.
    volatile atomic_int_least32_t tx_data_allocated;
    volatile atomic_uint_fast8_t tx_data_paused;
    volatile atomic_int_least32_t rx_data_allocated;

    // Reserved command buffers, and the backing memory they were carved from.
    struct mmpkt_list tx_command_free_list;
    struct mmpkt_list rx_command_free_list;
    uint8_t *tx_command_pool;
    uint8_t *rx_command_pool;

    mmhal_wlan_pktmem_tx_flow_control_cb_t tx_flow_control_cb;
} halow_pktmem_t;

static halow_pktmem_t pktmem;

// Carve a reserved pool into blocks and put them all on a free list.  Returns
// NULL if the driver's pool could not supply the memory, in which case the
// corresponding allocations simply fall back to ordinary pool allocations.
static uint8_t *halow_pool_init(struct mmpkt_list *list, size_t block_size, size_t n_blocks) {
    uint8_t *pool = halow_osal_malloc(block_size * n_blocks);
    if (pool == NULL) {
        return NULL;
    }
    for (size_t i = 0; i < n_blocks; i++) {
        mmpkt_list_append(list, (struct mmpkt *)(pool + block_size * i));
    }
    return pool;
}

void mmhal_wlan_pktmem_init(struct mmhal_wlan_pktmem_init_args *args) {
    memset(&pktmem, 0, sizeof(pktmem));
    pktmem.tx_flow_control_cb = args->tx_flow_control_cb;

    pktmem.tx_command_pool = halow_pool_init(&pktmem.tx_command_free_list,
        HALOW_TX_COMMAND_BLOCK_SIZE, HALOW_TX_COMMAND_N_BLOCKS);
    pktmem.rx_command_pool = halow_pool_init(&pktmem.rx_command_free_list,
        HALOW_RX_COMMAND_BLOCK_SIZE, HALOW_RX_COMMAND_N_BLOCKS);
}

void mmhal_wlan_pktmem_deinit(void) {
    // Give anything still holding a packet a chance to let go of it before the
    // reserved pools go back to the driver's memory pool.  A pool that was
    // never allocated has nothing to wait for.
    bool drained = false;
    for (unsigned i = 0; i < 100; i++) {
        drained = pktmem.tx_data_allocated == 0 && pktmem.rx_data_allocated == 0 &&
            (pktmem.tx_command_pool == NULL ||
                pktmem.tx_command_free_list.len == HALOW_TX_COMMAND_N_BLOCKS) &&
            (pktmem.rx_command_pool == NULL ||
                pktmem.rx_command_free_list.len == HALOW_RX_COMMAND_N_BLOCKS);
        if (drained) {
            break;
        }
        mmosal_task_sleep(10);
    }

    if (drained) {
        halow_osal_free(pktmem.tx_command_pool);
        halow_osal_free(pktmem.rx_command_pool);
    }
    // Otherwise the pools are left where they are: something still holds a
    // packet carved out of them, and handing the memory back would let the next
    // allocation take it while that packet is still in use.  The whole heap is
    // released immediately after this, so nothing is really leaked.
    memset(&pktmem, 0, sizeof(pktmem));
}

/*******************************************************************************/
// Reserved command pools

static struct mmpkt *halow_pool_alloc(struct mmpkt_list *list, uint32_t block_size,
    const struct mmpkt_ops *ops, uint32_t space_at_start, uint32_t space_at_end,
    uint32_t metadata_length) {
    MMOSAL_TASK_ENTER_CRITICAL();
    struct mmpkt *buf = mmpkt_list_dequeue(list);
    MMOSAL_TASK_EXIT_CRITICAL();

    if (buf == NULL) {
        return NULL;
    }

    struct mmpkt *pkt = mmpkt_init_buf((uint8_t *)buf, block_size,
        space_at_start, space_at_end, metadata_length, ops);
    if (pkt == NULL) {
        // Too big for a reserved block; hand it back and let the caller retry
        // against the pool.
        ops->free_mmpkt(buf);
    }
    return pkt;
}

static void halow_tx_command_free(void *mmpkt) {
    MMOSAL_TASK_ENTER_CRITICAL();
    mmpkt_list_append(&pktmem.tx_command_free_list, (struct mmpkt *)mmpkt);
    MMOSAL_TASK_EXIT_CRITICAL();
}

static const struct mmpkt_ops halow_tx_command_ops = {
    .free_mmpkt = halow_tx_command_free,
};

static void halow_rx_command_free(void *mmpkt) {
    MMOSAL_TASK_ENTER_CRITICAL();
    mmpkt_list_append(&pktmem.rx_command_free_list, (struct mmpkt *)mmpkt);
    MMOSAL_TASK_EXIT_CRITICAL();
}

static const struct mmpkt_ops halow_rx_command_ops = {
    .free_mmpkt = halow_rx_command_free,
};

/*******************************************************************************/
// Data packets

static void halow_tx_data_free(void *mmpkt) {
    atomic_int_least32_t old_value = atomic_fetch_sub(&pktmem.tx_data_allocated, 1);
    MMOSAL_ASSERT(old_value > 0);
    mmosal_free(mmpkt);

    if (pktmem.tx_data_allocated < HALOW_TX_UNPAUSE_THRESHOLD) {
        if (atomic_exchange(&pktmem.tx_data_paused, 0)) {
            pktmem.tx_flow_control_cb();
        }
    }
}

static const struct mmpkt_ops halow_tx_data_ops = {
    .free_mmpkt = halow_tx_data_free,
};

static void halow_rx_data_free(void *mmpkt) {
    if (mmpkt != NULL) {
        atomic_fetch_sub(&pktmem.rx_data_allocated, 1);
        mmosal_free(mmpkt);
    }
}

static const struct mmpkt_ops halow_rx_data_ops = {
    .free_mmpkt = halow_rx_data_free,
};

// mmpkt_alloc_on_heap() rounds each of these up to a multiple of four, which
// wraps to zero near UINT32_MAX and returns a block smaller than the caller
// writes into.  Bounded per field, not on the total, which is morselib's call.
static bool halow_pkt_size_ok(uint32_t space_at_start, uint32_t space_at_end,
    uint32_t metadata_length, uint32_t max) {
    return space_at_start <= max && space_at_end <= max && metadata_length <= max;
}

struct mmpkt *mmhal_wlan_alloc_mmpkt_for_tx(uint8_t pkt_class, uint32_t space_at_start,
    uint32_t space_at_end, uint32_t metadata_length) {
    if (!halow_pkt_size_ok(space_at_start, space_at_end, metadata_length,
        MMHAL_WLAN_MMPKT_TX_MAX_SIZE)) {
        return NULL;
    }
    // Commands come out of their reserved pool where possible, so that control
    // traffic keeps flowing even when the data path has taken everything else.
    if (pkt_class == MMHAL_WLAN_PKT_COMMAND) {
        struct mmpkt *pkt = halow_pool_alloc(&pktmem.tx_command_free_list,
            HALOW_TX_COMMAND_BLOCK_SIZE, &halow_tx_command_ops,
            space_at_start, space_at_end, metadata_length);
        if (pkt != NULL) {
            return pkt;
        }
    }

    if (atomic_fetch_add(&pktmem.tx_data_allocated, 1) >= MICROPY_HW_HALOW_TX_BLOCKS) {
        atomic_fetch_sub(&pktmem.tx_data_allocated, 1);
        return NULL;
    }

    struct mmpkt *pkt = mmpkt_alloc_on_heap(space_at_start, space_at_end, metadata_length);
    if (pkt == NULL) {
        atomic_fetch_sub(&pktmem.tx_data_allocated, 1);
        return NULL;
    }
    pkt->ops = &halow_tx_data_ops;

    if (pktmem.tx_data_allocated > HALOW_TX_PAUSE_THRESHOLD) {
        if (!atomic_exchange(&pktmem.tx_data_paused, 1)) {
            pktmem.tx_flow_control_cb();
        }
    }
    return pkt;
}

struct mmpkt *mmhal_wlan_alloc_mmpkt_for_rx(uint8_t pkt_class, uint32_t capacity,
    uint32_t metadata_length) {
    if (!halow_pkt_size_ok(0, capacity, metadata_length, MMHAL_WLAN_MMPKT_RX_MAX_SIZE)) {
        return NULL;
    }
    if (pkt_class == MMHAL_WLAN_PKT_COMMAND) {
        struct mmpkt *pkt = halow_pool_alloc(&pktmem.rx_command_free_list,
            HALOW_RX_COMMAND_BLOCK_SIZE, &halow_rx_command_ops, 0, capacity, metadata_length);
        if (pkt == NULL) {
            pkt = mmpkt_alloc_on_heap(0, capacity, metadata_length);
        }
        return pkt;
    }

    if (atomic_fetch_add(&pktmem.rx_data_allocated, 1) >= MICROPY_HW_HALOW_RX_BLOCKS) {
        atomic_fetch_sub(&pktmem.rx_data_allocated, 1);
        return NULL;
    }

    struct mmpkt *pkt = mmpkt_alloc_on_heap(0, capacity, metadata_length);
    if (pkt == NULL) {
        atomic_fetch_sub(&pktmem.rx_data_allocated, 1);
        return NULL;
    }
    pkt->ops = &halow_rx_data_ops;
    return pkt;
}

enum mmwlan_tx_flow_control_state mmhal_wlan_pktmem_tx_flow_control_state(void) {
    return pktmem.tx_data_paused ? MMWLAN_TX_PAUSED : MMWLAN_TX_READY;
}

#endif // MICROPY_PY_NETWORK_HALOW
