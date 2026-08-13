/*
 * QEMU tests for the HaLow cooperative scheduler.
 *
 * halow_sched.c cannot be tested on the host: its core is a naked function that
 * saves r4-r11 and d8-d15 by hand and builds a fake initial frame for the first
 * switch.  mps3-an547 is a Cortex-M55 built with the same float ABI as the
 * target, so the register file, FPU banking and exception model all match.
 *
 * Not covered: SPI, IRQ wiring, morselib, or timing.
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "py/mphal.h"
#include "py/runtime.h"
#include "halow_osal.h"
#include "halow_sched.h"

void qemu_puts(const char *s);
void qemu_exit(int code);

volatile uint32_t halow_test_ticks = 0;
volatile uint32_t halow_test_event_waits = 0;

// --- tiny allocator ----------------------------------------------------------
// The real first-fit allocator has its own 233-check host suite; this harness is
// about the context switch, so back halow_osal_malloc() with a bump allocator
// and keep the two concerns separate.
#define POOL_SIZE (32 * 1024)
static uint8_t pool[POOL_SIZE] __attribute__((aligned(8)));
static size_t pool_used;
static int pool_frees;

void *halow_osal_malloc(size_t size) {
    size = (size + 7u) & ~(size_t)7u;
    if (size == 0 || size > POOL_SIZE - pool_used) {
        return NULL;
    }
    void *p = &pool[pool_used];
    pool_used += size;
    return p;
}

void halow_osal_free(void *ptr) {
    if (ptr != NULL) {
        pool_frees++;   // bump allocator: count it, don't reclaim
    }
}

static void pool_reset(void) {
    pool_used = 0;
    pool_frees = 0;
}

// --- test plumbing -----------------------------------------------------------
static int failures;
static int checks;

void print_num(uint32_t v) {
    char buf[12];
    int i = 11;
    buf[i--] = '\0';
    if (v == 0) {
        buf[i--] = '0';
    }
    while (v > 0 && i >= 0) {
        buf[i--] = '0' + (v % 10);
        v /= 10;
    }
    qemu_puts(&buf[i + 1]);
}

static void check(bool cond, const char *msg) {
    checks++;
    if (!cond) {
        failures++;
        qemu_puts("  FAIL: ");
        qemu_puts(msg);
        qemu_puts("\n");
    }
}

// --- 1. a task runs and retires ----------------------------------------------
static volatile int ran_count;
static void task_runs_once(void *arg) {
    (void)arg;
    ran_count++;
}

static void test_task_runs_and_is_reaped(void) {
    pool_reset();
    halow_sched_deinit();
    ran_count = 0;
    halow_task_t *t = halow_sched_task_create(task_runs_once, NULL, 256, "once");
    check(t != NULL, "task_create returned NULL");
    halow_sched_run();
    check(ran_count == 1, "task did not run exactly once");
    // It retired via the trampoline, so run() should have reaped its stack.
    check(pool_frees == 1, "dead task's stack was not freed by reap");
    halow_sched_run();
    check(ran_count == 1, "a reaped task ran again");
}

// --- 2. core registers survive a switch --------------------------------------
// The whole point of the naked asm. Load r4-r11 with a known pattern, yield to
// the scheduler and back, and prove every one came back intact. r4-r11 are the
// callee-saved set the ABI says must survive a call.
static volatile int reg_result;
static void task_core_regs(void *arg) {
    (void)arg;
    uint32_t out[8];
    __asm volatile (
        "mov  r4,  #0x11        \n"
        "mov  r5,  #0x22        \n"
        "mov  r6,  #0x33        \n"
        "mov  r7,  #0x44        \n"
        "mov  r8,  #0x55        \n"
        "mov  r9,  #0x66        \n"
        "mov  r10, #0x77        \n"
        "mov  r11, #0x88        \n"
        :
        :
        : "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11"
        );
    halow_sched_yield();
    __asm volatile (
        "str  r4,  [%0, #0]     \n"
        "str  r5,  [%0, #4]     \n"
        "str  r6,  [%0, #8]     \n"
        "str  r7,  [%0, #12]    \n"
        "str  r8,  [%0, #16]    \n"
        "str  r9,  [%0, #20]    \n"
        "str  r10, [%0, #24]    \n"
        "str  r11, [%0, #28]    \n"
        :
        : "r" (out)
        : "memory"
        );
    reg_result = (out[0] == 0x11 && out[1] == 0x22 && out[2] == 0x33 && out[3] == 0x44
        && out[4] == 0x55 && out[5] == 0x66 && out[6] == 0x77 && out[7] == 0x88);
}

static void test_core_registers_survive(void) {
    pool_reset();
    halow_sched_deinit();
    reg_result = -1;
    halow_task_t *t = halow_sched_task_create(task_core_regs, NULL, 256, "regs");
    check(t != NULL, "task_create returned NULL");
    halow_sched_run();   // runs up to the yield
    halow_sched_run();   // resumes and finishes
    check(reg_result == 1, "r4-r11 were NOT preserved across a context switch");
}

// --- 3. FPU registers survive a switch ---------------------------------------
// d8-d15 are the callee-saved half of the VFP file and are saved by the vpush in
// halow_context_switch(). If that vpush/vpop pair is wrong -- or if the FPU was
// never enabled -- this is where it shows.
#if defined(__ARM_FP)
static volatile int fpu_result;
static void task_fpu_regs(void *arg) {
    (void)arg;
    double out[8];
    const double in[8] = { 1.5, 2.5, 3.5, 4.5, 5.5, 6.5, 7.5, 8.5 };
    __asm volatile ("vldm %0, {d8-d15}" : : "r" (in) : "d8", "d9", "d10", "d11",
        "d12", "d13", "d14", "d15");
    halow_sched_yield();
    __asm volatile ("vstm %0, {d8-d15}" : : "r" (out) : "memory");
    int ok = 1;
    for (int i = 0; i < 8; i++) {
        if (out[i] != in[i]) {
            ok = 0;
        }
    }
    fpu_result = ok;
}

static void test_fpu_registers_survive(void) {
    pool_reset();
    halow_sched_deinit();
    fpu_result = -1;
    halow_task_t *t = halow_sched_task_create(task_fpu_regs, NULL, 512, "fpu");
    check(t != NULL, "task_create returned NULL");
    halow_sched_run();
    halow_sched_run();
    check(fpu_result == 1, "d8-d15 were NOT preserved across a context switch");
}
#endif

// --- 4. tasks run in creation order ------------------------------------------
static volatile int order_idx;
static volatile int order_seen[3];
static void task_order_a(void *arg) {
    order_seen[order_idx++] = (int)(uintptr_t)arg;
}

static void test_tasks_run_in_creation_order(void) {
    pool_reset();
    halow_sched_deinit();
    order_idx = 0;
    for (uintptr_t i = 1; i <= 3; i++) {
        check(halow_sched_task_create(task_order_a, (void *)i, 256, "ord") != NULL,
            "task_create returned NULL");
    }
    halow_sched_run();
    check(order_idx == 3, "not all three tasks ran");
    check(order_seen[0] == 1 && order_seen[1] == 2 && order_seen[2] == 3,
        "tasks did not run in creation order");
}

// --- 5. the argument actually reaches the task -------------------------------
static volatile uintptr_t arg_seen;
static void task_arg(void *arg) {
    arg_seen = (uintptr_t)arg;
}

static void test_argument_is_passed(void) {
    pool_reset();
    halow_sched_deinit();
    arg_seen = 0;
    halow_sched_task_create(task_arg, (void *)0xDEADBEEF, 256, "arg");
    halow_sched_run();
    check(arg_seen == 0xDEADBEEF, "task argument did not survive the trampoline");
}

// --- 6. wait: satisfied, and timed out ---------------------------------------
static volatile int cond_calls;
static bool cond_true(void *arg) {
    (void)arg;
    cond_calls++;
    return true;
}
static bool cond_false(void *arg) {
    (void)arg;
    cond_calls++;
    return false;
}

static void test_wait_returns_true_when_satisfied(void) {
    halow_sched_deinit();
    cond_calls = 0;
    check(halow_sched_wait(cond_true, NULL, 100), "wait should succeed immediately");
    check(cond_calls == 1, "a satisfied condition should be evaluated once");
}

static void test_wait_times_out(void) {
    halow_sched_deinit();
    cond_calls = 0;
    halow_test_ticks = 0;
    uint32_t before = halow_test_event_waits;
    check(!halow_sched_wait(cond_false, NULL, 5), "wait should time out and return false");
    check(halow_test_event_waits > before, "a wait that times out should have slept, not spun");
}

static void test_wait_zero_timeout_polls_once(void) {
    halow_sched_deinit();
    cond_calls = 0;
    check(!halow_sched_wait(cond_false, NULL, 0), "zero timeout should return false");
    check(cond_calls == 1, "zero timeout should evaluate the condition exactly once");
}

// --- 7. re-entrancy: run() from inside a task is a no-op ----------------------
static volatile int reentrant_depth;
static volatile int reentrant_ran;
static void task_reentrant(void *arg) {
    (void)arg;
    reentrant_ran++;
    reentrant_depth++;
    halow_sched_run();   // must not recurse into this task again
    reentrant_depth--;
}

static void test_run_is_not_reentrant(void) {
    pool_reset();
    halow_sched_deinit();
    reentrant_ran = 0;
    reentrant_depth = 0;
    halow_sched_task_create(task_reentrant, NULL, 512, "reent");
    halow_sched_run();
    check(reentrant_ran == 1, "re-entrant run() re-entered the task");
}

// --- 8. stack painting reads back sanely -------------------------------------
// Geometry worth stating, because it is not obvious: the first context switch
// POPS the 26-word initial frame, so a task starts with sp at the very top of
// its stack. The top 26 words are therefore already unpainted before the task
// runs, and only usage deeper than that eats into the fill. A task with a
// shallow frame legitimately leaves the watermark unchanged -- so this test
// deliberately burns far more than that, or it would prove nothing.
static volatile size_t stack_free_seen;
static volatile size_t stack_free_before;
static halow_task_t *stack_task;

static void task_deep_stack_user(void *arg) {
    (void)arg;
    // 96 words, comfortably past the 26-word initial frame. volatile so the
    // compiler cannot elide it or hoist it into registers.
    volatile uint32_t scratch[96];
    for (int i = 0; i < 96; i++) {
        scratch[i] = (uint32_t)(i + 1);
    }
    uint32_t sum = 0;
    for (int i = 0; i < 96; i++) {
        sum += scratch[i];
    }
    (void)sum;
    stack_free_seen = halow_sched_stack_free_words(stack_task);
}

static void test_stack_watermark(void) {
    pool_reset();
    halow_sched_deinit();
    const size_t words = 256;
    stack_free_seen = 0;
    stack_task = halow_sched_task_create(task_deep_stack_user, NULL, words, "wm");
    check(stack_task != NULL, "task_create returned NULL");

    // Before it runs, exactly the initial context frame is unpainted.
    stack_free_before = halow_sched_stack_free_words(stack_task);
    check(stack_free_before > 0 && stack_free_before < words,
        "fresh stack watermark should be inside (0, stack_words)");
    check(words - stack_free_before < 64,
        "more than 64 words unpainted before the task ran: the initial frame is "
        "larger than expected, or the fill is not covering the whole stack");

    halow_sched_run();

    qemu_puts("\n    [words=");
    print_num((uint32_t)words);
    qemu_puts(" before=");
    print_num((uint32_t)stack_free_before);
    qemu_puts(" after=");
    print_num((uint32_t)stack_free_seen);
    qemu_puts(" drop=");
    print_num((uint32_t)(stack_free_before - stack_free_seen));
    qemu_puts("] ");
    check(stack_free_seen > 0, "watermark read as zero: the task overflowed its "
        "stack, or the fill is not working");
    check(stack_free_seen < stack_free_before,
        "watermark did not drop after a task burned 96 words of stack: the fill "
        "or the scan is not measuring real usage");

    // Deep stack usage is (stack_words - free), NOT (before - after). A task
    // starts with sp at the very top, so the first HALOW_CONTEXT_WORDS it uses
    // land in the initial frame's region, which was already unpainted -- those
    // words are real usage that never shows up as a drop from `before`.
    // Measured here: before=230, after=156, so the drop reads 74 while the task
    // actually used 100 words (96 array + 4 of frame). The total is the honest
    // number, and it is the one worth reading on the bench.
    size_t used = words - stack_free_seen;
    check(used >= 96, "total stack used is less than the 96 words the task "
        "demonstrably wrote");
    check(used < words, "task used its entire stack: treat as an overflow");
}

// --- 9. deinit clears state --------------------------------------------------
static void test_deinit_clears_state(void) {
    pool_reset();
    halow_sched_deinit();
    halow_sched_task_create(task_runs_once, NULL, 256, "d");
    halow_sched_deinit();
    check(halow_sched_task_current() == NULL, "current task not cleared by deinit");
    ran_count = 0;
    halow_sched_run();
    check(ran_count == 0, "a task survived deinit and ran");
}

// --- 10. task capacity is bounded, not corrupting -----------------------------
static void test_task_capacity(void) {
    pool_reset();
    halow_sched_deinit();
    int created = 0;
    for (int i = 0; i < 16; i++) {
        if (halow_sched_task_create(task_runs_once, NULL, 128, "cap") != NULL) {
            created++;
        }
    }
    check(created > 0, "no tasks could be created at all");
    check(created <= 8, "task table grew past its bound");
    check(halow_sched_task_create(task_runs_once, NULL, 128, "over") == NULL,
        "creating past capacity must return NULL, not overflow the table");
}

// -----------------------------------------------------------------------------
struct test { const char *name;
              void (*fn)(void);
};


// --- 12. one servicer at a time ----------------------------------------------
// The transceiver is serviced from two places: the poll dispatched from PendSV,
// and MicroPython context, where a wait or the transmit path runs the driver
// directly. PendSV preempts MicroPython, so without the claim the poll can start
// a bus transaction on top of one already in flight.
static void test_claim_is_exclusive(void) {
    pool_reset();
    halow_sched_deinit();
    check(halow_sched_claim(), "first claim was refused");
    halow_sched_release();
    check(halow_sched_claim(), "claim was not released");
    halow_sched_release();
}

// Nesting has to be allowed, or a wait reached from inside a claimed region is
// refused the scheduler that is the only thing able to satisfy it.  Both of
// these run in thread context, so they are the same owner.
static void test_claim_nests_within_a_context(void) {
    pool_reset();
    halow_sched_deinit();
    check(halow_sched_claim(), "outer claim was refused");
    check(halow_sched_claim(), "a nested claim from the same context was refused");
    halow_sched_release();
    check(halow_sched_claim(), "the claim was dropped by the inner release");
    halow_sched_release();
    halow_sched_release();
    check(halow_sched_claim(), "the claim was not released by the outermost release");
    halow_sched_release();
}

static void test_deinit_releases_the_claim(void) {
    pool_reset();
    check(halow_sched_claim(), "claim was refused before deinit");
    halow_sched_deinit();
    check(halow_sched_claim(), "deinit left the claim held");
    halow_sched_release();
}

// --- 13. a task that will not yield cannot hold the pass -----------------------
// This is the regression test for a hang seen on hardware: a transceiver that
// stopped answering on the bus put a driver task into an unbounded retry loop,
// and because the scheduler is cooperative the poll never returned from PendSV.
// USB, the network and the main loop all stopped with it.
//
// The task below models that loop: it spins forever, and like morselib's retry
// path it reaches a bus operation every time round. It must not be able to keep
// the CPU once the pass is out of time.
static volatile uint32_t spinner_laps;

// Far more laps than the budget can allow, so reaching it means the budget never
// arrived.  The task gives up there rather than spinning: a broken scheduler
// should fail this suite, not hang it.
#define SPINNER_LAP_LIMIT (10000)

static void task_spins_forever(void *arg) {
    (void)arg;
    while (spinner_laps < SPINNER_LAP_LIMIT) {
        spinner_laps++;
        // Stands in for mmhal_wlan_spi_cs_assert(): the point in the bus path
        // where a task that has overrun its turn gives one up.
        halow_test_ticks++;
        if (halow_sched_over_budget()) {
            halow_sched_yield();
        }
    }
}

static void test_runaway_task_cannot_starve_the_system(void) {
    pool_reset();
    halow_sched_deinit();
    halow_test_ticks = 0;
    spinner_laps = 0;
    halow_sched_task_create(task_spins_forever, NULL, 512, "spin");

    // Returning at all is the assertion: before the budget existed this call
    // never came back.
    halow_sched_run();
    check(spinner_laps > 0, "the runaway task never ran");
    check(spinner_laps < SPINNER_LAP_LIMIT, "the task was never denied a turn");

    // And it is still there, still runnable, having been denied a turn rather
    // than killed -- so the driver keeps working if the bus recovers.
    uint32_t laps_after_first_pass = spinner_laps;
    halow_sched_run();
    check(spinner_laps > laps_after_first_pass, "the runaway task was not resumed");
}

// The pass loop runs between task switches, where halow_task_cur is NULL.  The
// public predicate answers for the running task, so using it there made the
// check dead code and every pass ran to the full count regardless of the clock.
static volatile uint32_t slow_task_turns;

static void task_burns_a_whole_turn(void *arg) {
    (void)arg;
    for (;;) {
        slow_task_turns++;
        halow_test_ticks += 100;     // one turn, far past the budget
        halow_sched_yield();
    }
}

static void test_pass_loop_stops_when_out_of_time(void) {
    pool_reset();
    halow_sched_deinit();
    halow_test_ticks = 0;
    slow_task_turns = 0;
    halow_sched_task_create(task_burns_a_whole_turn, NULL, 512, "slow");

    halow_sched_run();
    // Each turn overruns the budget, so the pass loop has to stop after the
    // first one rather than running out its full count.
    check(slow_task_turns == 1, "the pass loop kept going after its time was up");
}

// A wait must never be unbounded, whatever was asked for: morselib joins its
// tasks with MMOSAL_WAIT_FOREVER, and a task wedged on a dead bus would take
// the firmware with it.
static int forever_release_after;

static bool cond_false_until_released(void *arg) {
    (void)arg;
    cond_calls++;
    return halow_test_ticks >= (uint32_t)forever_release_after;
}

static void test_wait_forever_outlives_the_cap(void) {
    // MMOSAL_WAIT_FOREVER is a promise: morselib's SDIO lock path asserts if
    // the wait returns false, so it must survive far past the teardown cap.
    pool_reset();
    halow_sched_deinit();
    halow_test_ticks = 0;
    cond_calls = 0;
    forever_release_after = 60000;   // six times the cap
    check(halow_sched_wait(cond_false_until_released, NULL, 0xFFFFFFFFu),
        "an infinite wait gave up instead of waiting");
    check(halow_test_ticks >= 60000, "the wait was satisfied early");
}

static void test_wait_forever_is_capped_in_teardown(void) {
    // Deinit on a dead bus has to complete: the same wait, with teardown in
    // progress, gives up after the cap.
    pool_reset();
    halow_sched_deinit();
    halow_test_ticks = 0;
    cond_calls = 0;
    halow_sched_teardown = true;
    check(!halow_sched_wait(cond_false, NULL, 0xFFFFFFFFu),
        "a teardown wait should give up");
    halow_sched_teardown = false;
    check(halow_test_ticks > 0, "the capped wait did not actually wait");
}

static void test_budget_is_not_spent_when_idle(void) {
    // A pass that does no work must not report itself over budget, or every
    // bus operation would yield and nothing would ever make progress.
    pool_reset();
    halow_sched_deinit();
    halow_test_ticks = 0;
    halow_sched_run();
    check(!halow_sched_over_budget(), "a fresh pass started out of time");

    // Outside a pass the deadline is whatever the last one left behind, which
    // is always in the past.  The transceiver is driven straight from
    // MicroPython during boot, and answering true there would yield into the
    // scheduler from the middle of a transfer nothing is waiting on.
    halow_test_ticks += 10000;
    check(!halow_sched_over_budget(), "a stale deadline leaked outside a pass");
}

int main(void) {
    static const struct test tests[] = {
        { "task runs and is reaped", test_task_runs_and_is_reaped },
        { "core registers survive switch", test_core_registers_survive },
        #if defined(__ARM_FP)
        { "FPU d8-d15 survive switch", test_fpu_registers_survive },
        #endif
        { "tasks run in creation order", test_tasks_run_in_creation_order },
        { "argument reaches the task", test_argument_is_passed },
        { "wait succeeds when satisfied", test_wait_returns_true_when_satisfied },
        { "wait times out", test_wait_times_out },
        { "wait(0) polls once", test_wait_zero_timeout_polls_once },
        { "run() is not re-entrant", test_run_is_not_reentrant },
        { "stack watermark", test_stack_watermark },
        { "deinit clears state", test_deinit_clears_state },
        { "task capacity is bounded", test_task_capacity },
        { "one servicer at a time", test_claim_is_exclusive },
        { "claims nest within a context", test_claim_nests_within_a_context },
        { "deinit releases the claim", test_deinit_releases_the_claim },
        { "runaway task cannot starve", test_runaway_task_cannot_starve_the_system },
        { "budget intact when idle", test_budget_is_not_spent_when_idle },
        { "pass loop stops when out of time", test_pass_loop_stops_when_out_of_time },
        { "an infinite wait outlives the cap", test_wait_forever_outlives_the_cap },
        { "an infinite wait is capped in teardown", test_wait_forever_is_capped_in_teardown },
    };

    qemu_puts("halow_sched on Cortex-M55 (qemu mps3-an547)\n");
    #if defined(__ARM_FP)
    qemu_puts("hard float: FPU context save IS exercised\n");
    #else
    qemu_puts("soft float: FPU context save NOT exercised\n");
    #endif
    qemu_puts("\n");

    for (unsigned i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        int before = failures;
        qemu_puts("  ");
        qemu_puts(tests[i].name);
        tests[i].fn();
        qemu_puts(failures == before ? " ... ok\n" : " ... FAILED\n");
    }

    qemu_puts("\n");
    print_num((uint32_t)checks);
    qemu_puts(" checks, ");
    print_num((uint32_t)failures);
    qemu_puts(" failures\n");
    return failures != 0;
}
