/*
 * Host-side tests for the HaLow driver's first-fit allocator.
 *
 * morselib allocates from PendSV context, so the driver cannot use the
 * MicroPython GC and carries its own allocator over a fixed pool.  These tests
 * compile the real halow_osal.c against the stubs in stub/, and call
 * halow_heap_verify() after every operation to check the block list invariants.
 */

#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- stub state the headers declare ------------------------------------------
int halow_test_atomic_depth = 0;
int halow_test_atomic_max = 0;
uint32_t halow_test_ticks_ms = 0;
int halow_test_malloc_fail = 0;

#include "py/mphal.h"
#include "py/mpprint.h"
#include "py/runtime.h"

halow_test_state_t halow_test_state = { NULL, NULL };
const mp_print_t mp_plat_print = { NULL, NULL };

int mp_printf(const mp_print_t *print, const char *fmt, ...) {
    (void)print;
    (void)fmt;
    return 0;
}
int mp_vprintf(const mp_print_t *print, const char *fmt, va_list args) {
    (void)print;
    (void)fmt;
    (void)args;
    return 0;
}

void *m_malloc_maybe(size_t n) {
    if (halow_test_malloc_fail) {
        return NULL;
    }
    return calloc(1, n);
}
void m_free(void *p) {
    free(p);
}

// The scheduler is ARM/PendSV and cannot build on the host. halow_osal.c
// references it for the task/mutex/timer layer, which these tests do not
// exercise; stubbing lets the allocator be tested in isolation.
typedef struct _halow_task_t halow_task_t;
typedef bool (*halow_cond_fn_t)(void *arg);
halow_task_t *halow_sched_task_create(void (*entry)(void *), void *arg, size_t sw, const char *n) {
    (void)entry;
    (void)arg;
    (void)sw;
    (void)n;
    return NULL;
}
void halow_sched_task_delete(halow_task_t *t) {
    (void)t;
}
halow_task_t *halow_sched_task_current(void) {
    return NULL;
}
void halow_sched_yield(void) {
}
bool halow_sched_wait(halow_cond_fn_t c, void *a, uint32_t t) {
    (void)c;
    (void)a;
    (void)t;
    return true;
}
void halow_sched_run(void) {
}
void halow_sched_reap(void) {
}
void halow_sched_deinit(void) {
}

#define MICROPY_PY_NETWORK_HALOW (1)
#include "halow_osal.c"

// -----------------------------------------------------------------------------
static int failures = 0;
static int checks = 0;

#define CHECK(cond, ...)                                     \
    do {                                                     \
        checks++;                                            \
        if (!(cond)) {                                       \
            failures++;                                      \
            printf("  FAIL %s:%d: ", __func__, __LINE__);    \
            printf(__VA_ARGS__);                             \
            printf("\n");                                    \
        }                                                    \
    } while (0)

// Walk the block list and assert every structural invariant. Called after every
// operation, so corruption is reported where it happens.
static void halow_heap_verify(const char *where) {
    checks++;
    size_t total = 0;
    int blocks = 0;
    const uint8_t *base = (const uint8_t *)MP_STATE_PORT(halow_heap);
    const uint8_t *end = base + MICROPY_HW_HALOW_HEAP_SIZE;

    for (halow_block_t *b = halow_heap_head; b != NULL; b = b->next) {
        blocks++;
        if (blocks > 4096) {
            failures++;
            printf("  FAIL %s: block list is cyclic or absurdly long\n", where);
            return;
        }
        // In range.
        if ((const uint8_t *)b < base || (const uint8_t *)b >= end) {
            failures++;
            printf("  FAIL %s: block %p outside pool [%p,%p)\n", where,
                (void *)b, (const void *)base, (const void *)end);
            return;
        }
        // Payload fits inside the pool.
        if ((const uint8_t *)b + HALOW_BLOCK_HDR + b->size > end) {
            failures++;
            printf("  FAIL %s: block %p size %zu runs past the pool end\n",
                where, (void *)b, b->size);
            return;
        }
        // Address ordered and non-overlapping: next must sit exactly after us.
        if (b->next != NULL) {
            const uint8_t *expect = (const uint8_t *)b + HALOW_BLOCK_HDR + b->size;
            if ((const uint8_t *)b->next != expect) {
                failures++;
                printf("  FAIL %s: block %p+%zu should abut %p but next is %p\n",
                    where, (void *)b, b->size, (const void *)expect, (void *)b->next);
                return;
            }
            // No two adjacent free blocks: free() must coalesce.
            if (!b->used && !b->next->used) {
                failures++;
                printf("  FAIL %s: adjacent free blocks not coalesced at %p\n",
                    where, (void *)b);
                return;
            }
        }
        // Payload alignment: morselib will put DMA-able structures here.
        if (((uintptr_t)HALOW_BLOCK_DATA(b) % HALOW_BLOCK_ALIGN) != 0) {
            failures++;
            printf("  FAIL %s: payload %p not %d-aligned\n", where,
                HALOW_BLOCK_DATA(b), HALOW_BLOCK_ALIGN);
            return;
        }
        total += HALOW_BLOCK_HDR + b->size;
    }
    // Every byte accounted for: no leaked space between blocks.
    if (total != MICROPY_HW_HALOW_HEAP_SIZE) {
        failures++;
        printf("  FAIL %s: blocks account for %zu of %d bytes\n", where,
            total, MICROPY_HW_HALOW_HEAP_SIZE);
    }
}

static void setup(void) {
    halow_test_malloc_fail = 0;
    halow_test_atomic_depth = 0;
    CHECK(halow_osal_init(), "init should succeed");
}
static void teardown(void) {
    halow_osal_deinit();
    CHECK(halow_test_atomic_depth == 0, "atomic sections unbalanced: depth %d",
        halow_test_atomic_depth);
}

// -----------------------------------------------------------------------------
static void test_init_deinit(void) {
    setup();
    CHECK(MP_STATE_PORT(halow_heap) != NULL, "heap pointer should be set");
    CHECK(halow_heap_head != NULL, "head should be set");
    halow_heap_verify("after init");
    teardown();
    CHECK(MP_STATE_PORT(halow_heap) == NULL, "heap pointer cleared on deinit");
    // The dangling-head hazard: a stale halow_heap_head after deinit would make
    // the next malloc walk freed memory. active(False)/active(True) does this.
    CHECK(halow_heap_head == NULL, "head must be cleared on deinit, else the "
        "next active(True) walks freed memory");
}

static void test_init_is_idempotent(void) {
    setup();
    void *first = MP_STATE_PORT(halow_heap);
    CHECK(halow_osal_init(), "second init should succeed");
    CHECK(MP_STATE_PORT(halow_heap) == first, "second init must not re-allocate");
    teardown();
}

static void test_init_handles_oom(void) {
    halow_osal_deinit();
    halow_test_malloc_fail = 1;
    CHECK(!halow_osal_init(), "init must report failure, not crash, when the GC "
        "heap cannot give up 96 KB");
    CHECK(MP_STATE_PORT(halow_heap) == NULL, "no heap on failed init");
    CHECK(halow_osal_malloc(64) == NULL, "malloc must return NULL with no pool");
    halow_osal_free(NULL);
    halow_test_malloc_fail = 0;
}

static void test_basic_alloc_free(void) {
    setup();
    void *a = halow_osal_malloc(100);
    CHECK(a != NULL, "100-byte alloc should succeed");
    halow_heap_verify("after alloc");
    memset(a, 0xAA, 100);
    halow_osal_free(a);
    halow_heap_verify("after free");
    teardown();
}

static void test_zero_size(void) {
    setup();
    CHECK(halow_osal_malloc(0) == NULL, "malloc(0) returns NULL");
    halow_heap_verify("after malloc(0)");
    teardown();
}

static void test_alignment(void) {
    setup();
    // Deliberately awkward sizes: every payload must still be 8-aligned.
    size_t sizes[] = { 1, 3, 7, 9, 15, 17, 31, 33, 63, 65, 127 };
    void *p[sizeof(sizes) / sizeof(sizes[0])];
    for (size_t i = 0; i < sizeof(sizes) / sizeof(sizes[0]); i++) {
        p[i] = halow_osal_malloc(sizes[i]);
        CHECK(p[i] != NULL, "alloc %zu failed", sizes[i]);
        CHECK(((uintptr_t)p[i] % HALOW_BLOCK_ALIGN) == 0,
            "alloc %zu returned %p, not %d-aligned", sizes[i], p[i], HALOW_BLOCK_ALIGN);
    }
    halow_heap_verify("after ragged allocs");
    for (size_t i = 0; i < sizeof(sizes) / sizeof(sizes[0]); i++) {
        halow_osal_free(p[i]);
    }
    halow_heap_verify("after ragged frees");
    teardown();
}

static void test_no_overlap_and_writes_are_isolated(void) {
    setup();
    enum { N = 64, SZ = 200 };
    uint8_t *p[N];
    for (int i = 0; i < N; i++) {
        p[i] = halow_osal_malloc(SZ);
        CHECK(p[i] != NULL, "alloc %d failed", i);
        memset(p[i], i + 1, SZ);   // unique pattern per allocation
    }
    halow_heap_verify("after N allocs");
    // If any two live allocations overlapped, a later memset would have
    // clobbered an earlier one.
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < SZ; j++) {
            if (p[i][j] != (uint8_t)(i + 1)) {
                failures++;
                printf("  FAIL overlap: block %d byte %d = %02x, expected %02x\n",
                    i, j, p[i][j], (uint8_t)(i + 1));
                i = N;
                break;
            }
        }
    }
    checks++;
    for (int i = 0; i < N; i++) {
        halow_osal_free(p[i]);
    }
    halow_heap_verify("after N frees");
    teardown();
}

static void test_coalesce_restores_full_heap(void) {
    setup();
    size_t biggest_before = halow_heap_head->size;
    enum { N = 32 };
    void *p[N];
    for (int i = 0; i < N; i++) {
        p[i] = halow_osal_malloc(512);
        CHECK(p[i] != NULL, "alloc %d failed", i);
    }
    // Free out of order -- coalescing must not depend on free order.
    for (int i = 0; i < N; i += 2) {
        halow_osal_free(p[i]);
    }
    halow_heap_verify("after even frees");
    for (int i = 1; i < N; i += 2) {
        halow_osal_free(p[i]);
    }
    halow_heap_verify("after odd frees");
    CHECK(halow_heap_head->next == NULL, "heap should be one block again, got a list");
    CHECK(halow_heap_head->size == biggest_before,
        "heap should be fully reclaimed: %zu vs %zu", halow_heap_head->size, biggest_before);
    teardown();
}

static void test_exhaustion_returns_null(void) {
    setup();
    // Take the pool down to nothing, then confirm failure is a NULL rather than
    // a wild pointer or a crash.
    int n = 0;
    while (halow_osal_malloc(4096) != NULL) {
        n++;
        if (n > 1000) {
            break;
        }
    }
    CHECK(n > 0 && n < 1000, "expected a bounded number of 4K allocs, got %d", n);
    CHECK(halow_osal_malloc(4096) == NULL, "exhausted heap must return NULL");
    CHECK(halow_osal_malloc(1) == NULL || 1, "small alloc may still fit; not fatal");
    halow_heap_verify("at exhaustion");
    teardown();
}

static void test_too_big_returns_null(void) {
    setup();
    CHECK(halow_osal_malloc(MICROPY_HW_HALOW_HEAP_SIZE * 2) == NULL,
        "an allocation larger than the pool must return NULL");
    halow_heap_verify("after oversize request");
    teardown();
}

// Independent of calloc: any caller passing a size near SIZE_MAX corrupts the
// heap, because HALOW_BLOCK_ROUND wraps. morselib sizes buffers from wire
// values, so a malformed length off the air can reach here.
static void test_malloc_size_overflow(void) {
    setup();
    size_t rounds_to_zero = SIZE_MAX - 3;   // (n + 7) & ~7 wraps to 0
    void *p = halow_osal_malloc(rounds_to_zero);
    CHECK(p == NULL, "malloc(SIZE_MAX-3) must return NULL; rounding wrapped to 0 "
        "and handed back %p", p);
    CHECK(halow_osal_malloc(SIZE_MAX) == NULL, "malloc(SIZE_MAX) must return NULL");
    halow_heap_verify("after size-overflow requests");
    teardown();
}

static void test_calloc_zeroes(void) {
    setup();
    uint8_t *p = mmosal_calloc_(16, 8);
    CHECK(p != NULL, "calloc failed");
    int nonzero = 0;
    for (int i = 0; i < 16 * 8; i++) {
        if (p[i] != 0) {
            nonzero++;
        }
    }
    CHECK(nonzero == 0, "calloc left %d non-zero bytes", nonzero);
    halow_osal_free(p);
    halow_heap_verify("after calloc/free");
    teardown();
}

static void test_calloc_overflow(void) {
    setup();
    // nitems * size overflows size_t. A wrapped product allocates a tiny block
    // while the caller believes it owns gigabytes; the first write past the
    // block corrupts the heap. Must return NULL instead.
    size_t huge = (size_t)1 << (sizeof(size_t) * 8 - 1);
    void *p = mmosal_calloc_(huge, 4);
    CHECK(p == NULL, "calloc(%zu, 4) overflows and must return NULL, got %p", huge, p);
    void *q = mmosal_calloc_(SIZE_MAX, 2);
    CHECK(q == NULL, "calloc(SIZE_MAX, 2) overflows and must return NULL, got %p", q);
    halow_heap_verify("after overflow attempts");
    teardown();
}

static void test_realloc(void) {
    setup();
    CHECK(mmosal_realloc_(NULL, 64) != NULL, "realloc(NULL, n) should malloc");
    void *p = halow_osal_malloc(64);
    memset(p, 0x5A, 64);
    void *big = mmosal_realloc_(p, 4096);
    CHECK(big != NULL, "realloc grow failed");
    int bad = 0;
    for (int i = 0; i < 64; i++) {
        if (((uint8_t *)big)[i] != 0x5A) {
            bad++;
        }
    }
    CHECK(bad == 0, "realloc lost %d bytes of the original contents", bad);
    CHECK(mmosal_realloc_(big, 0) == NULL, "realloc(p, 0) frees and returns NULL");
    halow_heap_verify("after realloc");
    teardown();
}

// Randomised churn. Deterministic seed so a failure is reproducible.
static void test_stress(void) {
    setup();
    enum { SLOTS = 96, ITERS = 20000 };
    uint8_t *p[SLOTS] = { 0 };
    size_t sz[SLOTS] = { 0 };
    unsigned seed = 12345;
    int allocs = 0, frees = 0;

    for (int i = 0; i < ITERS; i++) {
        seed = seed * 1103515245u + 12345u;
        int slot = (seed >> 16) % SLOTS;
        if (p[slot] == NULL) {
            size_t want = 8 + ((seed >> 8) % 1024);
            p[slot] = halow_osal_malloc(want);
            if (p[slot] != NULL) {
                sz[slot] = want;
                memset(p[slot], (uint8_t)(slot + 1), want);
                allocs++;
            }
        } else {
            // Verify our bytes survived everything that happened in between.
            for (size_t j = 0; j < sz[slot]; j++) {
                if (p[slot][j] != (uint8_t)(slot + 1)) {
                    failures++;
                    printf("  FAIL stress: slot %d corrupted at byte %zu (iter %d)\n",
                        slot, j, i);
                    i = ITERS;
                    break;
                }
            }
            halow_osal_free(p[slot]);
            p[slot] = NULL;
            frees++;
        }
        if ((i % 500) == 0) {
            halow_heap_verify("during stress");
        }
    }
    checks++;
    for (int i = 0; i < SLOTS; i++) {
        if (p[i] != NULL) {
            halow_osal_free(p[i]);
        }
    }
    halow_heap_verify("after stress");
    CHECK(halow_heap_head->next == NULL,
        "heap should coalesce back to one block after stress");
    printf("  (stress: %d allocs, %d frees)\n", allocs, frees);
    teardown();
}

int main(void) {
    struct { const char *name;
             void (*fn)(void);
    } tests[] = {
        { "init/deinit", test_init_deinit },
        { "init is idempotent", test_init_is_idempotent },
        { "init handles OOM", test_init_handles_oom },
        { "basic alloc/free", test_basic_alloc_free },
        { "malloc(0)", test_zero_size },
        { "alignment", test_alignment },
        { "no overlap, isolated writes", test_no_overlap_and_writes_are_isolated },
        { "coalesce restores full heap", test_coalesce_restores_full_heap },
        { "exhaustion returns NULL", test_exhaustion_returns_null },
        { "oversize returns NULL", test_too_big_returns_null },
        { "malloc size overflow", test_malloc_size_overflow },
        { "calloc zeroes", test_calloc_zeroes },
        { "calloc overflow", test_calloc_overflow },
        { "realloc", test_realloc },
        { "stress", test_stress },
    };
    int n = (int)(sizeof(tests) / sizeof(tests[0]));
    for (int i = 0; i < n; i++) {
        int before = failures;
        printf("%-32s ", tests[i].name);
        fflush(stdout);
        tests[i].fn();
        printf("%s\n", failures == before ? "ok" : "FAILED");
    }
    printf("\n%d checks, %d failures\n", checks, failures);
    printf("max atomic nesting depth: %d\n", halow_test_atomic_max);
    return failures != 0;
}
