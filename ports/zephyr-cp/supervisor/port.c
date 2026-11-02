// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/port.h"

#include "mpconfigboard.h"
#include "adaboot/update_mode.h"
#include "supervisor/shared/tick.h"

#if CIRCUITPY_AUDIOBUSIO_I2SOUT
#include "common-hal/audiobusio/I2SOut.h"
#endif
#if CIRCUITPY_NEOPIXEL_WRITE
#include "common-hal/neopixel_write/__init__.h"
#endif

#if CIRCUITPY_RTC
#include "shared-bindings/rtc/__init__.h"
#endif

#include <stdlib.h>

#include <zephyr/autoconf.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/reboot.h>
#include <zephyr/sys/util.h>

#if defined(CONFIG_ARCH_POSIX)
#include <limits.h>
#include <stdio.h>
#include <fcntl.h>

#include "cmdline.h"
#include "nsi_host_trampolines.h"
#include "posix_board_if.h"
#include "posix_native_task.h"
#endif

#include "lib/tlsf/tlsf.h"
#include <zephyr/device.h>
#include <zephyr/kernel.h>

#if defined(CONFIG_IMG_MANAGER)
#include <zephyr/dfu/mcuboot.h>
#endif

#if defined(CONFIG_SOC_FAMILY_RPI_PICO)
#include <hardware/structs/psm.h>
#include <hardware/structs/watchdog.h>
#endif

#if defined(CONFIG_TRACING_PERFETTO) && defined(CONFIG_BOARD_NATIVE_SIM)
#include "perfetto_encoder.h"
#include <zephyr/sys/mem_stats.h>
#define CIRCUITPY_PERFETTO_TRACK_GROUP_UUID 0x3000ULL
#define CIRCUITPY_PERFETTO_VM_HEAP_USED_UUID 0x3001ULL
#define CIRCUITPY_PERFETTO_OUTER_HEAP_USED_UUID 0x3002ULL
#endif

static tlsf_t heap;
static size_t tlsf_heap_used = 0;

// Auto generated in pins.c
extern const struct device *const rams[];
extern const uint32_t *const ram_bounds[];
extern const size_t circuitpy_max_ram_size;

static pool_t pools[CIRCUITPY_RAM_DEVICE_COUNT];
static uint8_t valid_pool_count = 0;
static bool zephyr_malloc_active = false;
static void *zephyr_malloc_top = NULL;
static void *zephyr_malloc_bottom = NULL;

static K_EVENT_DEFINE(main_needed);

static struct k_timer tick_timer;

#if defined(CONFIG_ARCH_POSIX)
// Number of VM runs before exiting.
// <= 0 means run forever.
// INT32_MAX means option was not provided.
static int32_t native_sim_port_resets = INT32_MAX;
static uint32_t native_sim_reset_port_count = 0;

// Path to a file used to preserve retained memory across the execv reboot, or
// NULL if disabled. Set with --retained-memory=<path> (see
// cp_retained_save/restore()).
static const char *native_sim_retained_memory;

typedef struct {
    uint32_t saved_word;
    uint32_t port_reset_count;
} cp_retained_data_t;

static struct args_struct_t native_sim_port_args[] = {
    {
        .option = "port-resets",
        .name = "count",
        .type = 'i',
        .dest = &native_sim_port_resets,
        .descript = "Exit native_sim after this many port_reset() calls. "
            "Example: --port-resets=2"
    },
    {
        .option = "retained-memory",
        .name = "path",
        .type = 's',
        .dest = (void *)&native_sim_retained_memory,
        .descript = "File used to preserve some state"
            " across the process re-exec reboot. "
            "Example: --retained-memory=/tmp/cp_retained.bin"
    },
    ARG_TABLE_ENDMARKER
};

static void native_sim_register_cmdline_opts(void) {
    native_add_command_line_opts(native_sim_port_args);
}

NATIVE_TASK(native_sim_register_cmdline_opts, PRE_BOOT_1, 0);
#endif

#if defined(CONFIG_TRACING_PERFETTO) && defined(CONFIG_BOARD_NATIVE_SIM)
static bool perfetto_circuitpython_tracks_emitted;

static void perfetto_emit_outer_heap_stats(void) {
    if (!perfetto_start()) {
        return;
    }
    size_t total = tlsf_heap_used;
    #if defined(CONFIG_COMMON_LIBC_MALLOC) && defined(CONFIG_SYS_HEAP_RUNTIME_STATS)
    extern int malloc_runtime_stats_get(struct sys_memory_stats *stats);
    struct sys_memory_stats stats;
    if (malloc_runtime_stats_get(&stats) == 0) {
        total += stats.allocated_bytes;
    }
    #endif
    perfetto_emit_counter(CIRCUITPY_PERFETTO_OUTER_HEAP_USED_UUID, (int64_t)total);
    Z_SPIN_DELAY(1);
}

static void perfetto_emit_circuitpython_tracks(void) {
    if (perfetto_circuitpython_tracks_emitted) {
        return;
    }
    if (!perfetto_start()) {
        return;
    }
    perfetto_emit_track_descriptor(CIRCUITPY_PERFETTO_TRACK_GROUP_UUID,
        perfetto_get_process_uuid(),
        "CircuitPython");
    perfetto_emit_counter_track_descriptor(CIRCUITPY_PERFETTO_VM_HEAP_USED_UUID,
        CIRCUITPY_PERFETTO_TRACK_GROUP_UUID,
        "VM Heap Used",
        PERFETTO_COUNTER_UNIT_BYTES);
    perfetto_emit_counter_track_descriptor(CIRCUITPY_PERFETTO_OUTER_HEAP_USED_UUID,
        CIRCUITPY_PERFETTO_TRACK_GROUP_UUID,
        "Outer Heap Used",
        PERFETTO_COUNTER_UNIT_BYTES);
    perfetto_circuitpython_tracks_emitted = true;
}
#else
static inline void perfetto_emit_outer_heap_stats(void) {
}

static inline void perfetto_emit_circuitpython_tracks(void) {
}
#endif

static void _tick_function(struct k_timer *timer_id) {
    supervisor_tick();
}

// Save and retrieve a word from memory that is preserved over reset. Used for safe mode.
static __noinit uint32_t cp_saved_word;

void port_set_saved_word(uint32_t value) {
    cp_saved_word = value;
}

uint32_t port_get_saved_word(void) {
    return cp_saved_word;
}

// Save and restore retained memory across the native_sim/bsim reboot. Opt in
// with --retained-memory=<path>.
#if defined(CONFIG_ARCH_POSIX)
static void cp_retained_save(void) {
    const char *path = native_sim_retained_memory;
    if (path == NULL || path[0] == '\0') {
        return;
    }
    int fd = nsi_host_open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        return;
    }
    cp_retained_data_t data = {
        .saved_word = cp_saved_word,
        .port_reset_count = native_sim_reset_port_count,
    };
    (void)nsi_host_write(fd, &data, sizeof(data));
    (void)nsi_host_close(fd);
}

static void cp_retained_restore(void) {
    const char *path = native_sim_retained_memory;
    if (path == NULL || path[0] == '\0') {
        return;
    }
    int fd = nsi_host_open(path, O_RDONLY, 0 /* unused */);
    if (fd < 0) {
        return; // First boot: no save file yet.
    }
    cp_retained_data_t data = { 0 };
    (void)nsi_host_read(fd, &data, sizeof(data));
    (void)nsi_host_close(fd);
    cp_saved_word = data.saved_word;
    native_sim_reset_port_count = data.port_reset_count;
}
#endif

safe_mode_t port_init(void) {
    #if defined(CONFIG_ARCH_POSIX)
    cp_retained_restore();
    #endif

    // We run CircuitPython at the lowest priority (just higher than idle.)
    // This allows networking and USB to preempt us.
    k_thread_priority_set(k_current_get(), CONFIG_NUM_PREEMPT_PRIORITIES - 1);

    #if defined(CONFIG_MCUMGR_GRP_OS_BOOTLOADER_INFO_HOOK)
    // Called from here, not SYS_INIT: this file is an archive member that
    // nothing references, so the linker never pulls it in unless another
    // always-linked translation unit calls into it.
    adaboot_info_init();
    #endif

    #if defined(CONFIG_IMG_MANAGER)
    // Confirm the running image so MCUboot treats it as permanent instead of
    // reverting to the other slot on the next boot. Reaching here means the
    // image booted far enough to run the port, which is the usual recovery
    // criterion for a test (TEST_AND_CONFIRM) OTA upgrade.
    if (!boot_is_img_confirmed()) {
        (void)boot_write_img_confirmed();
    }
    #endif

    k_timer_init(&tick_timer, _tick_function, NULL);
    perfetto_emit_circuitpython_tracks();
    return SAFE_MODE_NONE;
}

// Reset the microcontroller completely.
void reset_cpu(void) {
    #if defined(CONFIG_ARCH_POSIX)
    cp_retained_save();
    #endif

    #if defined(CONFIG_SOC_FAMILY_RPI_PICO)
    // On RP2 the Arm system reset leaves the USB controller running, so the
    // host never sees the board go away. Reset the whole chip through the
    // watchdog instead, as pico-sdk's watchdog_reboot() does. Zephyr's
    // hal_rpi_pico does not build watchdog.c, so write the registers here.
    hw_clear_bits(&watchdog_hw->ctrl, WATCHDOG_CTRL_ENABLE_BITS);
    watchdog_hw->scratch[4] = 0;
    hw_set_bits(&psm_hw->wdsel, PSM_WDSEL_BITS & ~(PSM_WDSEL_ROSC_BITS | PSM_WDSEL_XOSC_BITS));
    hw_set_bits(&watchdog_hw->ctrl, WATCHDOG_CTRL_TRIGGER_BITS);
    #endif

    // Try a warm reboot first. It won't return if it works but isn't always
    // implemented.
    sys_reboot(SYS_REBOOT_WARM);
    sys_reboot(SYS_REBOOT_COLD);
    printk("Failed to reboot. Looping.\n");
    while (true) {
    }
}

void reset_port(void) {
    #if CIRCUITPY_AUDIOBUSIO_I2SOUT
    i2sout_reset();
    #endif
    #if CIRCUITPY_NEOPIXEL_WRITE
    neopixel_write_reset();
    #endif

    #if CIRCUITPY_RTC
    rtc_reset();
    #endif

    #if defined(CONFIG_ARCH_POSIX)
    native_sim_reset_port_count++;
    if (native_sim_port_resets != INT32_MAX &&
        native_sim_port_resets > 0 &&
        native_sim_reset_port_count >= (uint32_t)(native_sim_port_resets + 1)) {
        printk("posix: exiting after %d port resets\n", native_sim_port_resets);
        posix_exit(0);
    }
    #endif
}

void reset_to_bootloader(void) {
    // Ask for UF2 mode so the bootloader shows its drive.
    adaboot_request_update_mode(true);
    reset_cpu();
}

void port_wake_main_task(void) {
    k_event_set(&main_needed, 1);
}

void port_wake_main_task_from_isr(void) {
    k_event_set(&main_needed, 1);
}

void port_task_yield(void) {
    k_yield();
}

void port_task_sleep_ms(uint32_t msecs) {
    k_msleep(msecs);
}

void port_boot_info(void) {
}

// Get stack limit address
uint32_t *port_stack_get_limit(void) {
    return (uint32_t *)k_current_get()->stack_info.start;
}

// Get stack top address
uint32_t *port_stack_get_top(void) {
    _thread_stack_info_t stack_info = k_current_get()->stack_info;

    uint32_t *top = (uint32_t *)(stack_info.start + stack_info.size - stack_info.delta);
    #if defined(CONFIG_ARCH_POSIX)
    // On hosted builds the thread stack is a pthread stack. pthread_getattr_np(),
    // which the POSIX arch uses to fix up stack_info, can report a size larger
    // than the real mapping (ASan intercepts it and inflates the size). The GC
    // scans up to the returned top, so clamp it to the end of the mapping that
    // contains the current stack pointer.
    //
    // Only ever clamp *downwards*: /proc/self/maps merges adjacent anonymous
    // mappings with the same flags, so the line containing our stack pointer
    // routinely covers several thread stacks at once (e.g. 0xf63c2000-0xf73c4000
    // for two 8MB stacks). Taking its end as the top made the GC scan megabytes
    // past this thread's stack, into the neighbouring thread's live stack -- and
    // segfault whenever the range had an unmapped hole (a guard page, or a stack
    // that has since been freed).
    volatile uint32_t stack_probe;
    uintptr_t sp = (uintptr_t)&stack_probe;
    FILE *maps = fopen("/proc/self/maps", "r");
    if (maps != NULL) {
        char line[256];
        unsigned long low, high;
        while (fgets(line, sizeof(line), maps) != NULL) {
            if (sscanf(line, "%lx-%lx", &low, &high) == 2 && low <= sp && sp < high) {
                if (high < (uintptr_t)top) {
                    top = (uint32_t *)high;
                }
                break;
            }
        }
        fclose(maps);
    }
    #endif
    return top;
}

uint64_t port_get_raw_ticks(uint8_t *subticks) {
    // Make sure time advances in the simulator.
    #if defined(CONFIG_ARCH_POSIX)
    k_busy_wait(100);
    #endif
    int64_t uptime = k_uptime_ticks() * 32768 / CONFIG_SYS_CLOCK_TICKS_PER_SEC;
    if (subticks != NULL) {
        *subticks = uptime % 32;
    }
    return uptime / 32;
}

// Enable 1/1024 second tick.
void port_enable_tick(void) {
    k_timer_start(&tick_timer, K_USEC(1000000 / 1024), K_USEC(1000000 / 1024));
}

// Disable 1/1024 second tick.
void port_disable_tick(void) {
    k_timer_stop(&tick_timer);
}

static k_timeout_t next_timeout;
static k_timepoint_t next_timepoint;

void port_interrupt_after_ticks(uint32_t ticks) {
    size_t zephyr_ticks = ticks * CONFIG_SYS_CLOCK_TICKS_PER_SEC / 1024;
    k_timeout_t maybe_next_timeout = K_TIMEOUT_ABS_TICKS(k_uptime_ticks() + zephyr_ticks);
    k_timepoint_t maybe_next_timepoint = sys_timepoint_calc(maybe_next_timeout);
    if (sys_timepoint_cmp(maybe_next_timepoint, next_timepoint) < 0) {
        next_timeout = maybe_next_timeout;
        next_timepoint = maybe_next_timepoint;
    }
}

void port_idle_until_interrupt(void) {
    k_event_wait(&main_needed, 0xffffffff, true, next_timeout);
    next_timeout = K_FOREVER;
    next_timepoint = sys_timepoint_calc(next_timeout);
}

// Zephyr doesn't maintain one multi-heap. So, make our own using TLSF.
void port_heap_init(void) {
    // Do a test malloc to determine if Zephyr has an outer heap that may
    // overlap with a memory region we've identified in ram_bounds. We'll
    // corrupt each other if we both use it.
    #ifdef CONFIG_COMMON_LIBC_MALLOC
    uint32_t *test_malloc = malloc(32);
    free(test_malloc); // Free right away so we don't forget. We don't actually write it anyway.
    zephyr_malloc_active = test_malloc != NULL;
    #endif

    for (size_t i = 0; i < CIRCUITPY_RAM_DEVICE_COUNT; i++) {
        uint32_t *heap_bottom = ram_bounds[2 * i];
        uint32_t *heap_top = ram_bounds[2 * i + 1];
        size_t size = (heap_top - heap_bottom) * sizeof(uint32_t);
        // The linker script may fill up a region we thought we could use at
        // build time. (The ram_bounds values are sometimes determined by the
        // linker.) So, we need to guard against regions that aren't actually
        // free.
        if (size < 1024) {
            printk("Skipping region because the linker filled it up.\n");
            continue;
        }
        #ifdef CONFIG_COMMON_LIBC_MALLOC
        // Skip a ram region if our test malloc is within it. We'll use Zephyr's
        // malloc to share that space with Zephyr.
        if (heap_bottom <= test_malloc && test_malloc < heap_top) {
            zephyr_malloc_top = heap_top;
            zephyr_malloc_bottom = heap_bottom;
            printk("Skipping region because Zephyr malloc is within bounds\n");
            pools[i] = NULL;
            continue;
        }
        #endif

        printk("Init heap at %p - %p with size %d\n", heap_bottom, heap_top, size);
        // If this crashes, then make sure you've enabled all of the Kconfig needed for the drivers.
        if (valid_pool_count == 0) {
            heap = tlsf_create_with_pool(heap_bottom, size, circuitpy_max_ram_size);
            if (heap == NULL) {
                // Can happen for a region the linker filled almost to the top,
                // which the build-time MINIMUM_RAM_SIZE filter cannot predict
                // because it only sees the devicetree size.
                printk("Heap creation failed at %p; trying the next region\n", heap_bottom);
                pools[i] = NULL;
                continue;
            }
            pools[i] = tlsf_get_pool(heap);
        } else {
            pools[i] = tlsf_add_pool(heap, heap_bottom + 1, size - sizeof(uint32_t));
        }
        valid_pool_count++;
    }
    perfetto_emit_outer_heap_stats();
    #if !DT_HAS_CHOSEN(zephyr_sram)
    #error "No SRAM!"
    #endif
}

void *port_malloc(size_t size, bool dma_capable) {
    void *block = NULL;
    if (valid_pool_count > 0) {
        block = tlsf_malloc(heap, size);
    }
    if (block != NULL) {
        tlsf_heap_used += tlsf_block_size(block);
    }
    #ifdef CONFIG_COMMON_LIBC_MALLOC
    if (block == NULL) {
        block = malloc(size);
    }
    #endif
    if (block != NULL) {
        perfetto_emit_outer_heap_stats();
    }
    return block;
}

void port_free(void *ptr) {
    if (ptr == NULL) {
        return;
    }
    if (valid_pool_count > 0 && !(ptr >= zephyr_malloc_bottom && ptr < zephyr_malloc_top)) {
        tlsf_heap_used -= tlsf_block_size(ptr);
        tlsf_free(heap, ptr);
    } else {
        #ifdef CONFIG_COMMON_LIBC_MALLOC
        free(ptr);
        #endif
    }
    perfetto_emit_outer_heap_stats();
}

void *port_realloc(void *ptr, size_t size, bool dma_capable) {
    if (ptr == NULL) {
        return port_malloc(size, dma_capable);
    }
    if (valid_pool_count > 0 && !(ptr >= zephyr_malloc_bottom && ptr < zephyr_malloc_top)) {
        size_t old_size = tlsf_block_size(ptr);
        void *new_block = tlsf_realloc(heap, ptr, size);
        if (new_block != NULL) {
            tlsf_heap_used = tlsf_heap_used - old_size + tlsf_block_size(new_block);
            perfetto_emit_outer_heap_stats();
        }
        return new_block;
    }
    #ifdef CONFIG_COMMON_LIBC_MALLOC
    void *new_block = realloc(ptr, size);
    if (new_block != NULL) {
        perfetto_emit_outer_heap_stats();
    }
    return new_block;
    #endif
    return NULL;
}

static bool max_size_walker(void *ptr, size_t size, int used, void *user) {
    size_t *max_size = (size_t *)user;
    if (!used && *max_size < size) {
        *max_size = size;
    }
    return true;
}

// The larger of the TLSF and Zephyr malloc free space, because port_malloc()
// falls back to Zephyr's malloc when TLSF can't satisfy a request.
size_t port_heap_get_largest_free_size(void) {
    bool known = false;
    size_t largest = 0;
    if (valid_pool_count > 0) {
        size_t max_size = 0;
        for (size_t i = 0; i < CIRCUITPY_RAM_DEVICE_COUNT; i++) {
            if (pools[i] == NULL) {
                continue;
            }
            tlsf_walk_pool(pools[i], max_size_walker, &max_size);
        }
        // IDF does this. Not sure why.
        largest = tlsf_fit_size(heap, max_size);
        known = true;
    }
    #if defined(CONFIG_COMMON_LIBC_MALLOC) && defined(CONFIG_SYS_HEAP_RUNTIME_STATS)
    // Compute how much of Zephyr's malloc arena the VM heap may take: the
    // arena's total free bytes, minus:
    // - Alignment padding. sys_heap can add up to __alignof__(z_max_align_t)
    //   bytes to a malloc() request, so a request for all the free bytes
    //   would fail.
    // - CONFIG_CIRCUITPY_ZEPHYR_MALLOC_RESERVE, left for Zephyr's own
    //   allocations, such as for TLS connections and BLE pairing.
    //
    // This can be misleading when the arena is fragmented. sys_heap only
    // reports total free bytes, not the largest free block, so the result can
    // be larger than any single block malloc() can return. gc_try_add_heap()
    // then retries with smaller sizes.
    //
    // zephyr_malloc_active is set by port_heap_init() when Zephyr's malloc()
    // works at all.
    if (zephyr_malloc_active) {
        extern int malloc_runtime_stats_get(struct sys_memory_stats *stats);
        struct sys_memory_stats stats;
        if (malloc_runtime_stats_get(&stats) == 0) {
            size_t held_back = __alignof__(z_max_align_t) + CONFIG_CIRCUITPY_ZEPHYR_MALLOC_RESERVE;
            size_t arena_free = stats.free_bytes > held_back ? stats.free_bytes - held_back : 0;
            largest = MAX(largest, arena_free);
            known = true;
        }
    }
    #endif
    return known ? largest : 64 * 1024;
}

void assert_post_action(const char *file, unsigned int line) {
    // printk("Assertion failed at %s:%u\n", file, line);
    // Check that this is arm
    #if defined(__arm__)
    __asm__ ("bkpt");
    #endif
    while (1) {
        ;
    }
}
