// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "supervisor/background_callback.h"
#include "supervisor/board.h"
#include "supervisor/linker.h"
#include "supervisor/port.h"

#include "bindings/rp2pio/StateMachine.h"
#include "genhdr/mpversion.h"
#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/countio/Counter.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/rtc/__init__.h"

#if CIRCUITPY_AUDIOCORE
#include "audio_dma.h"
#endif

#if CIRCUITPY_PICODVI_AUDIOOUT
#include "common-hal/picodvi/AudioOut.h"
#endif

#if CIRCUITPY_SSL
#include "shared-module/ssl/__init__.h"
#include "psa/crypto.h"
#endif

#if CIRCUITPY_RP2_SHA256_HW
#include "rp2_sha256_psa.h"
#endif

#if CIRCUITPY_WIFI
#include "common-hal/wifi/__init__.h"
#endif

#include "common-hal/rtc/RTC.h"
#include "common-hal/busio/UART.h"

#if CIRCUITPY_SDIOIO
#include "common-hal/sdioio/SDCard.h"
#endif

#include "supervisor/shared/safe_mode.h"
#include "supervisor/shared/stack.h"
#include "supervisor/shared/tick.h"

#include "hardware/clocks.h"
#include "hardware/structs/psm.h"
#include "hardware/structs/scb.h"
#include "hardware/structs/watchdog.h"
#include "hardware/gpio.h"
#include "hardware/uart.h"
#include "hardware/sync.h"
#include "hardware/timer.h"
#if CIRCUITPY_CYW43
#include "py/mphal.h"
#include "pico/cyw43_arch.h"
#endif
#include "pico/time.h"
#include "pico/binary_info.h"

#include "pico/bootrom.h"
#include "hardware/watchdog.h"

#if PICO_RP2350
#include "RP2350.h" // CMSIS
#endif

#if CIRCUITPY_BOOT_BUTTON_NO_GPIO
#include "hardware/gpio.h"
#include "hardware/sync.h"
#include "hardware/structs/ioqspi.h"
#include "hardware/structs/sio.h"
#endif

#include "supervisor/shared/serial.h"

#include "tusb.h"
#include <cmsis_compiler.h>
#include "lib/tlsf/tlsf.h"

critical_section_t background_queue_lock;

// The SDK's panic() lives in flash, but core1 may run with flash access
// disabled by the MPU (usb_host and picodvi lock it out), where a flash call
// hard faults before anything is printed. -Wl,--wrap=panic routes every
// panic here instead: core0 keeps the SDK behavior, core1 halts from RAM.
// Verified by tools/check_core1_flash_calls.py.
void __wrap_panic(const char *fmt, ...) __attribute__((noreturn, format(printf, 1, 2)));
void panic_core0(const char *fmt, va_list args) __attribute__((noreturn, format(printf, 1, 0)));

// Flash-resident; only ever called from core0 (see __wrap_panic). Mirrors
// the SDK implementation. noinline keeps the flash-calling code out of the
// RAM-resident wrapper below.
__attribute__((noinline)) void panic_core0(const char *fmt, va_list args) {
    puts("\n*** PANIC ***\n");
    if (fmt) {
        vprintf(fmt, args);
        puts("");
    }
    _exit(1);
}

void __not_in_flash_func(__wrap_panic)(const char *fmt, ...) {
    if (get_core_num() == 0) {
        va_list args;
        va_start(args, fmt);
        panic_core0(fmt, args);
    }
    // Flash may be MPU-locked on this core and stdio is core0-only, so the
    // message is unprintable here. Halt in RAM: a debugger stops at the
    // breakpoint, otherwise spin.
    __breakpoint();
    while (1) {
    }
}

extern volatile bool mp_msc_enabled;

static void _tick_callback(uint alarm_num);

static void _binary_info(void) {
    // Binary info readable with `picotool`.
    bi_decl(bi_program_name("CircuitPython"));
    bi_decl(bi_program_version_string(MICROPY_GIT_TAG));
    bi_decl(bi_program_build_date_string(MICROPY_BUILD_DATE));
    bi_decl(bi_program_url("https://circuitpython.org"));

    bi_decl(bi_program_build_attribute("BOARD=" CIRCUITPY_BOARD_ID));
    // TODO: Add build attribute for debug builds. Needs newer CircuitPython with CIRCUITPY_DEBUG.
}

extern uint32_t _ld_dtcm_bss_start;
extern uint32_t _ld_dtcm_bss_size;
extern uint32_t _ld_dtcm_data_destination;
extern uint32_t _ld_dtcm_data_size;
extern uint32_t _ld_dtcm_data_flash_copy;
extern uint32_t _ld_itcm_destination;
extern uint32_t _ld_itcm_size;
extern uint32_t _ld_itcm_flash_copy;

static tlsf_t _heap = NULL;
static tlsf_t _psram_heap = NULL;
static size_t _psram_size = 0;

#ifdef CIRCUITPY_PSRAM_CHIP_SELECT

#include "hardware/regs/qmi.h"
#include "hardware/regs/xip.h"
#include "hardware/structs/qmi.h"
#include "hardware/structs/xip_ctrl.h"

static void __no_inline_not_in_flash_func(setup_psram)(void) {
    // Read the system clock and the chip select pin before QMI goes into
    // direct mode. Both are in flash, and flash reads fault in direct mode.
    uint32_t sys_clk_khz = clock_get_hz(clk_sys) / 1000;
    const uint8_t cs_pin = CIRCUITPY_PSRAM_CHIP_SELECT->number;
    gpio_set_function(cs_pin, GPIO_FUNC_XIP_CS1);
    _psram_size = 0;
    common_hal_mcu_disable_interrupts();
    // Try and read the PSRAM ID via direct_csr.
    qmi_hw->direct_csr = 30 << QMI_DIRECT_CSR_CLKDIV_LSB |
        QMI_DIRECT_CSR_EN_BITS;
    // Need to poll for the cooldown on the last XIP transfer to expire
    // (via direct-mode BUSY flag) before it is safe to perform the first
    // direct-mode operation
    while ((qmi_hw->direct_csr & QMI_DIRECT_CSR_BUSY_BITS) != 0) {
    }

    // Exit out of QMI in case we've inited already
    qmi_hw->direct_csr |= QMI_DIRECT_CSR_ASSERT_CS1N_BITS;
    // Transmit as quad.
    qmi_hw->direct_tx = QMI_DIRECT_TX_OE_BITS |
        QMI_DIRECT_TX_IWIDTH_VALUE_Q << QMI_DIRECT_TX_IWIDTH_LSB |
        0xf5;
    while ((qmi_hw->direct_csr & QMI_DIRECT_CSR_BUSY_BITS) != 0) {
    }
    (void)qmi_hw->direct_rx;
    qmi_hw->direct_csr &= ~(QMI_DIRECT_CSR_ASSERT_CS1N_BITS);

    // Read the id
    qmi_hw->direct_csr |= QMI_DIRECT_CSR_ASSERT_CS1N_BITS;
    uint8_t kgd = 0;
    uint8_t eid = 0;
    for (size_t i = 0; i < 7; i++) {
        if (i == 0) {
            qmi_hw->direct_tx = 0x9f;
        } else {
            qmi_hw->direct_tx = 0xff;
        }
        while ((qmi_hw->direct_csr & QMI_DIRECT_CSR_TXEMPTY_BITS) == 0) {
        }
        while ((qmi_hw->direct_csr & QMI_DIRECT_CSR_BUSY_BITS) != 0) {
        }
        if (i == 5) {
            kgd = qmi_hw->direct_rx;
        } else if (i == 6) {
            eid = qmi_hw->direct_rx;
        } else {
            (void)qmi_hw->direct_rx;
        }
    }
    // Disable direct csr.
    qmi_hw->direct_csr &= ~(QMI_DIRECT_CSR_ASSERT_CS1N_BITS | QMI_DIRECT_CSR_EN_BITS);

    if (kgd != 0x5D) {
        common_hal_mcu_enable_interrupts();
        reset_pin_number(cs_pin);
        return;
    }

    // Enable quad mode.
    qmi_hw->direct_csr = 30 << QMI_DIRECT_CSR_CLKDIV_LSB |
        QMI_DIRECT_CSR_EN_BITS;
    // Need to poll for the cooldown on the last XIP transfer to expire
    // (via direct-mode BUSY flag) before it is safe to perform the first
    // direct-mode operation
    while ((qmi_hw->direct_csr & QMI_DIRECT_CSR_BUSY_BITS) != 0) {
    }

    // RESETEN, RESET and quad enable
    for (uint8_t i = 0; i < 3; i++) {
        qmi_hw->direct_csr |= QMI_DIRECT_CSR_ASSERT_CS1N_BITS;
        if (i == 0) {
            qmi_hw->direct_tx = 0x66;
        } else if (i == 1) {
            qmi_hw->direct_tx = 0x99;
        } else {
            qmi_hw->direct_tx = 0x35;
        }
        while ((qmi_hw->direct_csr & QMI_DIRECT_CSR_BUSY_BITS) != 0) {
        }
        qmi_hw->direct_csr &= ~(QMI_DIRECT_CSR_ASSERT_CS1N_BITS);
        for (size_t j = 0; j < 20; j++) {
            asm ("nop");
        }
        (void)qmi_hw->direct_rx;
    }
    // Disable direct csr.
    qmi_hw->direct_csr &= ~(QMI_DIRECT_CSR_ASSERT_CS1N_BITS | QMI_DIRECT_CSR_EN_BITS);

    mcu_processor_update_psram_timing(sys_clk_khz);
    qmi_hw->m[1].rfmt = (QMI_M0_RFMT_PREFIX_WIDTH_VALUE_Q << QMI_M0_RFMT_PREFIX_WIDTH_LSB |
            QMI_M0_RFMT_ADDR_WIDTH_VALUE_Q << QMI_M0_RFMT_ADDR_WIDTH_LSB |
            QMI_M0_RFMT_SUFFIX_WIDTH_VALUE_Q << QMI_M0_RFMT_SUFFIX_WIDTH_LSB |
            QMI_M0_RFMT_DUMMY_WIDTH_VALUE_Q << QMI_M0_RFMT_DUMMY_WIDTH_LSB |
            QMI_M0_RFMT_DUMMY_LEN_VALUE_24 << QMI_M0_RFMT_DUMMY_LEN_LSB |
            QMI_M0_RFMT_DATA_WIDTH_VALUE_Q << QMI_M0_RFMT_DATA_WIDTH_LSB |
            QMI_M0_RFMT_PREFIX_LEN_VALUE_8 << QMI_M0_RFMT_PREFIX_LEN_LSB |
            QMI_M0_RFMT_SUFFIX_LEN_VALUE_NONE << QMI_M0_RFMT_SUFFIX_LEN_LSB);
    qmi_hw->m[1].rcmd = 0xeb << QMI_M0_RCMD_PREFIX_LSB |
        0 << QMI_M0_RCMD_SUFFIX_LSB;
    qmi_hw->m[1].wfmt = (QMI_M0_WFMT_PREFIX_WIDTH_VALUE_Q << QMI_M0_WFMT_PREFIX_WIDTH_LSB |
            QMI_M0_WFMT_ADDR_WIDTH_VALUE_Q << QMI_M0_WFMT_ADDR_WIDTH_LSB |
            QMI_M0_WFMT_SUFFIX_WIDTH_VALUE_Q << QMI_M0_WFMT_SUFFIX_WIDTH_LSB |
            QMI_M0_WFMT_DUMMY_WIDTH_VALUE_Q << QMI_M0_WFMT_DUMMY_WIDTH_LSB |
            QMI_M0_WFMT_DUMMY_LEN_VALUE_NONE << QMI_M0_WFMT_DUMMY_LEN_LSB |
            QMI_M0_WFMT_DATA_WIDTH_VALUE_Q << QMI_M0_WFMT_DATA_WIDTH_LSB |
            QMI_M0_WFMT_PREFIX_LEN_VALUE_8 << QMI_M0_WFMT_PREFIX_LEN_LSB |
            QMI_M0_WFMT_SUFFIX_LEN_VALUE_NONE << QMI_M0_WFMT_SUFFIX_LEN_LSB);
    qmi_hw->m[1].wcmd = 0x38 << QMI_M0_WCMD_PREFIX_LSB |
        0 << QMI_M0_WCMD_SUFFIX_LSB;

    common_hal_mcu_enable_interrupts();

    _psram_size = 1024 * 1024; // 1 MiB
    uint8_t size_id = eid >> 5;
    if (eid == 0x26 || size_id == 2) {
        _psram_size *= 8;
    } else if (size_id == 0) {
        _psram_size *= 2;
    } else if (size_id == 1) {
        _psram_size *= 4;
    }

    // Mark that we can write to PSRAM.
    xip_ctrl_hw->ctrl |= XIP_CTRL_WRITABLE_M1_BITS;

    // Test write to the PSRAM.
    volatile uint32_t *psram_nocache = (volatile uint32_t *)0x15000000;
    psram_nocache[0] = 0x12345678;
    volatile uint32_t readback = psram_nocache[0];
    if (readback != 0x12345678) {
        _psram_size = 0;
        return;
    }
}
#endif

static void _port_heap_init(void) {
    uint32_t *heap_bottom = port_heap_get_bottom();
    uint32_t *heap_top = port_heap_get_top();
    size_t size = (heap_top - heap_bottom) * sizeof(uint32_t);
    _heap = tlsf_create_with_pool(heap_bottom, size, size);
    if (_psram_size > 0) {
        _psram_heap = tlsf_create_with_pool((void *)0x11000000, _psram_size, _psram_size);
    }
}

void port_heap_init(void) {
    // We call _port_heap_init from port_init to initialize the heap early.
}

void *port_malloc(size_t size, bool dma_capable) {
    // Prefer internal RAM for everything: it is much faster than PSRAM
    // (data there skips the external bus and the shared XIP cache). PSRAM, when
    // present, serves as spillover capacity for allocations that don't need DMA.
    common_hal_mcu_disable_interrupts();
    void *block = tlsf_malloc(_heap, size);
    common_hal_mcu_enable_interrupts();
    if (block == NULL && !dma_capable && _psram_size > 0) {
        common_hal_mcu_disable_interrupts();
        block = tlsf_malloc(_psram_heap, size);
        common_hal_mcu_enable_interrupts();
    }
    return block;
}

void port_free(void *ptr) {
    common_hal_mcu_disable_interrupts();
    if (((size_t)ptr) < SRAM_BASE) {
        tlsf_free(_psram_heap, ptr);
    } else {
        tlsf_free(_heap, ptr);
    }
    common_hal_mcu_enable_interrupts();
}

void *port_realloc(void *ptr, size_t size, bool dma_capable) {
    if (ptr == NULL) {
        // Fresh allocation: same internal-first policy as port_malloc.
        return port_malloc(size, dma_capable);
    }
    if (_psram_size > 0 && ((size_t)ptr) < SRAM_BASE) {
        common_hal_mcu_disable_interrupts();
        void *block = tlsf_realloc(_psram_heap, ptr, size);
        common_hal_mcu_enable_interrupts();
        if (block) {
            return block;
        }
    }
    common_hal_mcu_disable_interrupts();
    void *new_ptr = tlsf_realloc(_heap, ptr, size);
    common_hal_mcu_enable_interrupts();
    return new_ptr;
}

#if !CIRCUITPY_ALL_MEMORY_DMA_CAPABLE
bool port_buffer_is_dma_capable(const void *ptr, size_t len) {
    // For RP2350, DMA can only access SRAM, not PSRAM
    // PSRAM addresses are below SRAM_BASE
    return ptr != NULL && ((size_t)ptr) >= SRAM_BASE;
}
#endif

static bool max_size_walker(void *ptr, size_t size, int used, void *user) {
    size_t *max_size = (size_t *)user;
    if (!used && *max_size < size) {
        *max_size = size;
    }
    return true;
}

size_t port_heap_get_largest_free_size(void) {
    size_t max_size = 0;
    tlsf_walk_pool(tlsf_get_pool(_heap), max_size_walker, &max_size);
    max_size = tlsf_fit_size(_heap, max_size);
    if (_psram_heap != NULL) {
        tlsf_walk_pool(tlsf_get_pool(_psram_heap), max_size_walker, &max_size);
        max_size = tlsf_fit_size(_psram_heap, max_size);
    }
    return max_size;
}

safe_mode_t port_init(void) {
    _binary_info();
    // Set brown out.

    // Load from the XIP memory space that doesn't cache. That way we don't
    // evict anything else. The code we're loading is linked to the RAM address
    // anyway.
    #if PICO_RP2040
    size_t nocache = 0x03000000;
    #endif
    #if PICO_RP2350
    size_t nocache = 0x04000000;
    #endif

    // Copy all of the "tightly coupled memory" code and data to run from RAM.
    // This lets us use the 16k cache for dynamically used data and code.
    // We must do this before we try and call any of its code or load the data.
    uint32_t *itcm_flash_copy = (uint32_t *)(((size_t)&_ld_itcm_flash_copy) | nocache);
    for (uint32_t i = 0; i < ((size_t)&_ld_itcm_size) / 4; i++) {
        (&_ld_itcm_destination)[i] = itcm_flash_copy[i];
    }

    // Copy all of the data to run from DTCM.
    uint32_t *dtcm_flash_copy = (uint32_t *)(((size_t)&_ld_dtcm_data_flash_copy) | nocache);
    for (uint32_t i = 0; i < ((size_t)&_ld_dtcm_data_size) / 4; i++) {
        (&_ld_dtcm_data_destination)[i] = dtcm_flash_copy[i];
    }

    // Clear DTCM bss.
    for (uint32_t i = 0; i < ((size_t)&_ld_dtcm_bss_size) / 4; i++) {
        (&_ld_dtcm_bss_start)[i] = 0;
    }

    // Set up the critical section to protect the background task queue.
    critical_section_init(&background_queue_lock);

    // Reset everything into a known state before board_init.
    reset_port();

    // Initialize RTC
    #if CIRCUITPY_RTC
    common_hal_rtc_init();
    #endif

    // Send-event-on-pend, so the WFE in port_idle_until_interrupt wakes on a
    // pending interrupt (as WFI did) in addition to waking on SEV — including
    // the SEV core 1 sends via port_wake_main_task() when it queues USB host
    // work.
    #if PICO_RP2040
    scb_hw->scr |= M0PLUS_SCR_SEVONPEND_BITS;
    #else
    scb_hw->scr |= M33_SCR_SEVONPEND_BITS;
    #endif

    // For the tick.
    hardware_alarm_claim(0);
    hardware_alarm_set_callback(0, _tick_callback);

    // RP2 port-specific early serial initialization for psram debug.
    // The RTC must already be initialized, otherwise the serial UART
    // will hang.
    serial_early_init();

    #ifdef CIRCUITPY_PSRAM_CHIP_SELECT
    setup_psram();
    #endif

    // Initialize heap early to allow for early allocation.
    _port_heap_init();

    // Check brownout.

    #if CIRCUITPY_CYW43
    // A small number of samples of pico w need an additional delay before
    // initializing the cyw43 chip. Delays inside cyw43_arch_init_with_country
    // are intended to meet the power on timing requirements, but apparently
    // are inadequate. We'll back off this long delay based on future testing.
    mp_hal_delay_ms(CIRCUITPY_CYW43_INIT_DELAY);

    // Change this as a placeholder as to how to init with country code.
    // Default country code is CYW43_COUNTRY_WORLDWIDE)
    if (cyw43_arch_init_with_country(PICO_CYW43_ARCH_DEFAULT_COUNTRY_CODE)) {
        serial_write("WiFi init failed\n");
    } else {
        cyw_ever_init = true;
    }
    #endif
    if (board_requests_safe_mode()) {
        return SAFE_MODE_USER;
    }

    return SAFE_MODE_NONE;
}

void reset_port(void) {
    #if CIRCUITPY_RTC
    rtc_reset();
    #endif

    #if CIRCUITPY_PICODVI_AUDIOOUT
    picodvi_audioout_reset();
    #endif

    #if CIRCUITPY_SSL
    ssl_reset();

    // For raspberrypi, we must free PSA crypto, because there are GC-heap objects
    // in the key slots.  We can't put this call in ssl_reset() because that's a
    // shared-module implementation. Unlike raspberrypi, espressif ESP-IDF inits PSA
    // once at boot and would never re-init it.
    // common_hal_ssl_sslcontext_construct() re-inits PSA on demand.
    // So for raspberrypi, we must call mbedtls_psa_crypto_free() explicitly.
    mbedtls_psa_crypto_free();
    #endif

    #if CIRCUITPY_RP2_SHA256_HW
    rp2_sha256_psa_reset();
    #endif

    #if CIRCUITPY_WATCHDOG
    watchdog_reset();
    #endif

    #if CIRCUITPY_WIFI
    wifi_reset();
    #endif
}

void reset_to_bootloader(void) {
    reset_usb_boot(0, 0);
    while (true) {
    }
}

// Called from isr_hardfault(), so this and everything it calls runs from RAM.
// Same register sequence as watchdog_reboot(0, 0, 0), which is in flash.
void __not_in_flash_func(reset_cpu)(void) {
    hw_clear_bits(&watchdog_hw->ctrl, WATCHDOG_CTRL_ENABLE_BITS);
    watchdog_hw->scratch[4] = 0;
    hw_set_bits(&psm_hw->wdsel, PSM_WDSEL_BITS & ~(PSM_WDSEL_ROSC_BITS | PSM_WDSEL_XOSC_BITS));
    hw_clear_bits(&watchdog_hw->ctrl,
        WATCHDOG_CTRL_PAUSE_DBG0_BITS | WATCHDOG_CTRL_PAUSE_DBG1_BITS | WATCHDOG_CTRL_PAUSE_JTAG_BITS);
    hw_set_bits(&watchdog_hw->ctrl, WATCHDOG_CTRL_TRIGGER_BITS);

    while (true) {
        __wfi();
    }
}

// From the linker script
extern uint32_t _ld_cp_dynamic_mem_start;
extern uint32_t _ld_cp_dynamic_mem_end;
uint32_t *port_stack_get_limit(void) {
    #pragma GCC diagnostic push

    #pragma GCC diagnostic ignored "-Warray-bounds"
    return port_stack_get_top() - (CIRCUITPY_DEFAULT_STACK_SIZE + CIRCUITPY_EXCEPTION_STACK_SIZE) / sizeof(uint32_t);
    #pragma GCC diagnostic pop
}

uint32_t *port_stack_get_top(void) {
    return &_ld_cp_dynamic_mem_end;
}

uint32_t *port_heap_get_bottom(void) {
    return &_ld_cp_dynamic_mem_start;
}

uint32_t *port_heap_get_top(void) {
    return port_stack_get_limit();
}

uint32_t __uninitialized_ram(saved_word);
void __not_in_flash_func(port_set_saved_word)(uint32_t value) {
    // Store in RAM because the watchdog scratch registers don't survive
    // resetting by pulling the RUN pin low.
    saved_word = value;
}

uint32_t port_get_saved_word(void) {
    return saved_word;
}

static volatile bool ticks_enabled;
static volatile bool _woken_up;

uint64_t port_get_raw_ticks(uint8_t *subticks) {
    int64_t all_subticks = time_us_64() * 512 / 15625;
    if (subticks != NULL) {
        *subticks = all_subticks % 32;
    }
    return all_subticks / 32;
}

static void _tick_callback(uint alarm_num) {
    if (ticks_enabled) {
        supervisor_tick();
        hardware_alarm_set_target(0, delayed_by_us(get_absolute_time(), 977));
    }
    _woken_up = true;
}

// Enable 1/1024 second tick.
void port_enable_tick(void) {
    ticks_enabled = true;
    hardware_alarm_set_target(0, delayed_by_us(get_absolute_time(), 977));
}

// Disable 1/1024 second tick.
void port_disable_tick(void) {
    // One additional _tick_callback may occur, but it will just return
    // whenever !ticks_enabled. Cancel is not called just in case
    // it could nuke a timeout set by port_interrupt_after_ticks.
    ticks_enabled = false;
}

// This is called by sleep, we ignore it when our ticks are enabled because
// they'll wake us up earlier. If we don't, we'll mess up ticks by overwriting
// the next RTC wake up time.
void port_interrupt_after_ticks(uint32_t ticks) {
    if (!ticks_enabled) {
        hardware_alarm_set_target(0, delayed_by_us(get_absolute_time(), ticks * 977));
    }
    _woken_up = false;
}

void port_idle_until_interrupt(void) {
    #if PICO_RP2040
    common_hal_mcu_disable_interrupts();
    #if CIRCUITPY_USB_HOST
    if (!background_callback_pending() && !tud_task_event_ready() && !tuh_task_event_ready() && !_woken_up) {
    #else
    if (!background_callback_pending() && !tud_task_event_ready() && !_woken_up) {
        #endif
        __DSB();
        // WFE, not WFI: the event register is sticky, so a SEV from core 1
        // (port_wake_main_task, e.g. a USB host event) that lands between the
        // checks above and here still terminates the wait. Pending interrupts
        // wake it too, via SEVONPEND (set in port_init).
        __wfe();
    }
    common_hal_mcu_enable_interrupts();
    #else
    // because we use interrupt priority, don't use
    // common_hal_mcu_disable_interrupts (because an interrupt masked by
    // BASEPRI will not occur)
    uint32_t state = save_and_disable_interrupts();

    // Ensure BASEPRI is at 0...
    uint32_t oldBasePri = __get_BASEPRI();
    __set_BASEPRI(0);
    __isb();
    #if CIRCUITPY_USB_HOST
    if (!background_callback_pending() && !tud_task_event_ready() && !tuh_task_event_ready() && !_woken_up) {
    #else
    if (!background_callback_pending() && !tud_task_event_ready() && !_woken_up) {
        #endif
        __DSB();
        // WFE, not WFI: see the RP2040 branch above.
        __wfe();
    }

    // and restore basepri before reenabling interrupts
    __set_BASEPRI(oldBasePri);
    __isb();

    restore_interrupts(state);
    #endif
}

// Called whenever a background callback is queued, including from core 1
// (which runs the PIO-USB host and posts its events here). SEV is broadcast
// to both cores and latches in the event register, so it reliably ends the
// WFE in port_idle_until_interrupt even if it arrives before the WFE starts.
// Without this, core 0 sleeps through host events for the remainder of a
// time.sleep(): device removal processing and enumeration stall until the
// next unrelated wakeup.
// PLACE_IN_ITCM: core 1 runs with flash execute-never, so this must live in
// RAM like its background_callback_add_core caller (__sev inlines to a bare
// instruction).
void PLACE_IN_ITCM(port_wake_main_task)(void) {
    __sev();
}

/**
 * \brief Default interrupt handler for unused IRQs.
 */
extern MP_NORETURN void isr_hardfault(void); // provide a prototype to avoid a missing-prototypes diagnostic
__attribute__((used)) void __not_in_flash_func(isr_hardfault)(void) {
    // Only safe mode from core 0 which is running CircuitPython. Core 1 faulting
    // should not be fatal to CP. (Fingers crossed.)
    if (get_core_num() == 0) {
        reset_into_safe_mode(SAFE_MODE_HARD_FAULT);
    }
    while (true) {
        asm ("nop;");
    }
}

void port_task_yield(void) {
    #if CIRCUITPY_CYW43
    cyw43_arch_poll();
    #endif
}

void port_boot_info(void) {
    #if CIRCUITPY_CYW43
    mp_printf(&mp_plat_print, "MAC");
    for (int i = 0; i < 6; i++) {
        mp_printf(&mp_plat_print, ":%02X", cyw43_state.mac[i]);
    }
    mp_printf(&mp_plat_print, "\n");
    #endif
}

#if CIRCUITPY_BOOT_BUTTON_NO_GPIO
bool __no_inline_not_in_flash_func(port_boot_button_pressed)(void) {
    // Sense the state of the boot button. Because this function
    // disables flash, it cannot be safely called once the second
    // core has been started. When the BOOTSEL button is sensed as
    // pressed, return is delayed until the button is released and
    // a delay has passed in order to debounce the button.
    const uint32_t CS_PIN_INDEX = 1;
    #if PICO_RP2040
    const uint32_t CS_BIT = 1u << 1;
    #else
    const uint32_t CS_BIT = SIO_GPIO_HI_IN_QSPI_CSN_BITS;
    #endif
    uint32_t int_state = save_and_disable_interrupts();
    // Wait for any outstanding XIP activity to finish. Flash
    // must be quiescent before disabling the chip select. Since
    // there's no XIP busy indication we can test, we delay a
    // generous 5 ms to allow any XIP activity to finish.
    busy_wait_us(5000);
    // Float the flash chip select pin. The line will HI-Z due to
    // the external 10K pull-up resistor.
    hw_write_masked(&ioqspi_hw->io[CS_PIN_INDEX].ctrl,
        GPIO_OVERRIDE_LOW << IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_LSB,
            IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_BITS);
    // Delay 100 us to allow the CS line to stabilize. If BOOTSEL is
    // pressed, the line will be pulled low by the button and its
    // 1K external resistor to ground.
    busy_wait_us(100);
    bool button_pressed = !(sio_hw->gpio_hi_in & CS_BIT);
    // Wait for the button to be released.
    if (button_pressed) {
        while (!(sio_hw->gpio_hi_in & CS_BIT)) {
            tight_loop_contents();
        }
        // Wait for 50 ms to debounce the button.
        busy_wait_us(50000);
    }
    // Restore the flash chip select pin to its original state.
    hw_write_masked(&ioqspi_hw->io[CS_PIN_INDEX].ctrl,
        GPIO_OVERRIDE_NORMAL << IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_LSB,
            IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_BITS);
    // Delay 5 ms to allow the flash chip to re-enable and for the
    // flash CS pin to stabilize.
    busy_wait_us(5000);
    // Restore the interrupt state.
    restore_interrupts(int_state);
    return button_pressed;
}
#endif
