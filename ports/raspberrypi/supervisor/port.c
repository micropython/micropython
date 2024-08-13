// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <string.h>
#include <stdlib.h>

#include "supervisor/background_callback.h"
#include "supervisor/board.h"
#include "supervisor/port.h"

#include "bindings/rp2pio/StateMachine.h"
#include "genhdr/mpversion.h"
#include "shared-bindings/busio/I2C.h"
#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/countio/Counter.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/rtc/__init__.h"

#if CIRCUITPY_AUDIOCORE
#include "audio_dma.h"
#endif

#if CIRCUITPY_SSL
#include "shared-module/ssl/__init__.h"
#endif

#if CIRCUITPY_WIFI
#include "common-hal/wifi/__init__.h"
#endif

#include "common-hal/rtc/RTC.h"
#include "common-hal/busio/UART.h"

#include "supervisor/shared/safe_mode.h"
#include "supervisor/shared/stack.h"
#include "supervisor/shared/tick.h"

#include "src/rp2040/hardware_structs/include/hardware/structs/watchdog.h"
#include "src/rp2_common/hardware_gpio/include/hardware/gpio.h"
#include "src/rp2_common/hardware_uart/include/hardware/uart.h"
#include "src/rp2_common/hardware_sync/include/hardware/sync.h"
#include "src/rp2_common/hardware_timer/include/hardware/timer.h"
#if CIRCUITPY_CYW43
#include "py/mphal.h"
#include "pico/cyw43_arch.h"
#endif
#include "src/common/pico_time/include/pico/time.h"
#include "src/common/pico_binary_info/include/pico/binary_info.h"

#include "pico/bootrom.h"
#include "hardware/watchdog.h"

#include "supervisor/shared/serial.h"

#include "tusb.h"
#include <cmsis_compiler.h>

critical_section_t background_queue_lock;

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

#ifdef CIRCUITPY_PSRAM_CHIP_SELECT

#include "lib/tlsf/tlsf.h"

#include "src/rp2350/hardware_regs/include/hardware/regs/qmi.h"
#include "src/rp2350/hardware_regs/include/hardware/regs/xip.h"
#include "src/rp2350/hardware_structs/include/hardware/structs/qmi.h"
#include "src/rp2350/hardware_structs/include/hardware/structs/xip_ctrl.h"

static tlsf_t _heap = NULL;
static pool_t _ram_pool = NULL;
static pool_t _psram_pool = NULL;
static size_t _psram_size = 0;

static void __no_inline_not_in_flash_func(setup_psram)(void) {
    gpio_set_function(CIRCUITPY_PSRAM_CHIP_SELECT->number, GPIO_FUNC_XIP_CS1);
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
        reset_pin_number(CIRCUITPY_PSRAM_CHIP_SELECT->number);
        return;
    }
    never_reset_pin_number(CIRCUITPY_PSRAM_CHIP_SELECT->number);

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

    qmi_hw->m[1].timing =
        QMI_M0_TIMING_PAGEBREAK_VALUE_1024 << QMI_M0_TIMING_PAGEBREAK_LSB | // Break between pages.
            3 << QMI_M0_TIMING_SELECT_HOLD_LSB | // Delay releasing CS for 3 extra system cycles.
            1 << QMI_M0_TIMING_COOLDOWN_LSB |
            1 << QMI_M0_TIMING_RXDELAY_LSB |
            16 << QMI_M0_TIMING_MAX_SELECT_LSB | // In units of 64 system clock cycles. PSRAM says 8us max. 8 / 0.00752 / 64 = 16.62
            7 << QMI_M0_TIMING_MIN_DESELECT_LSB | // In units of system clock cycles. PSRAM says 50ns.50 / 7.52 = 6.64
            2 << QMI_M0_TIMING_CLKDIV_LSB;
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

void port_heap_init(void) {
    uint32_t *heap_bottom = port_heap_get_bottom();
    uint32_t *heap_top = port_heap_get_top();
    size_t size = (heap_top - heap_bottom) * sizeof(uint32_t);
    _heap = tlsf_create_with_pool(heap_bottom, size, 64 * 1024 * 1024);
    _ram_pool = tlsf_get_pool(_heap);
    if (_psram_size > 0) {
        _psram_pool = tlsf_add_pool(_heap, (void *)0x11000004, _psram_size - 4);
    }
}

void *port_malloc(size_t size, bool dma_capable) {
    void *block = tlsf_malloc(_heap, size);
    return block;
}

void port_free(void *ptr) {
    tlsf_free(_heap, ptr);
}

void *port_realloc(void *ptr, size_t size) {
    return tlsf_realloc(_heap, ptr, size);
}

static void max_size_walker(void *ptr, size_t size, int used, void *user) {
    size_t *max_size = (size_t *)user;
    if (!used && *max_size < size) {
        *max_size = size;
    }
}

size_t port_heap_get_largest_free_size(void) {
    size_t max_size = 0;
    tlsf_walk_pool(_ram_pool, max_size_walker, &max_size);
    if (_psram_pool != NULL) {
        tlsf_walk_pool(_psram_pool, max_size_walker, &max_size);
    }
    // IDF does this. Not sure why.
    return tlsf_fit_size(_heap, max_size);
}
#endif

safe_mode_t port_init(void) {
    _binary_info();
    // Set brown out.

    // Load from the XIP memory space that doesn't cache. That way we don't
    // evict anything else. The code we're loading is linked to the RAM address
    // anyway.
    #ifdef PICO_RP2040
    size_t nocache = 0x03000000;
    #endif
    #ifdef PICO_RP2350
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

    #if CIRCUITPY_CYW43
    never_reset_pin_number(23);
    never_reset_pin_number(24);
    never_reset_pin_number(25);
    never_reset_pin_number(29);
    #endif

    // Reset everything into a known state before board_init.
    reset_port();

    serial_early_init();

    #ifdef CIRCUITPY_PSRAM_CHIP_SELECT
    setup_psram();
    #endif

    // Initialize RTC
    #if CIRCUITPY_RTC
    common_hal_rtc_init();
    #endif

    // For the tick.
    hardware_alarm_claim(0);
    hardware_alarm_set_callback(0, _tick_callback);

    // Check brownout.

    #if CIRCUITPY_CYW43
    // A small number of samples of pico w need an additional delay before
    // initializing the cyw43 chip. Delays inside cyw43_arch_init_with_country
    // are intended to meet the power on timing requirements, but apparently
    // are inadequate. We'll back off this long delay based on future testing.
    mp_hal_delay_ms(1000);
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
    #if CIRCUITPY_BUSIO
    reset_i2c();
    reset_spi();
    reset_uart();
    #endif

    #if CIRCUITPY_COUNTIO
    reset_countio();
    #endif

    #if CIRCUITPY_RP2PIO
    reset_rp2pio_statemachine();
    #endif

    #if CIRCUITPY_RTC
    rtc_reset();
    #endif

    #if CIRCUITPY_AUDIOCORE
    audio_dma_reset();
    #endif

    #if CIRCUITPY_SSL
    ssl_reset();
    #endif

    #if CIRCUITPY_WATCHDOG
    watchdog_reset();
    #endif

    #if CIRCUITPY_WIFI
    wifi_reset();
    #endif

    reset_all_pins();
}

void reset_to_bootloader(void) {
    reset_usb_boot(0, 0);
    while (true) {
    }
}

void reset_cpu(void) {
    watchdog_reboot(0, SRAM_END, 0);
    watchdog_start_tick(12);

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
void port_set_saved_word(uint32_t value) {
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
    uint64_t microseconds = time_us_64();
    if (subticks != NULL) {
        *subticks = (uint8_t)(((microseconds % 1000000) % 977) / 31);
    }
    return 1024 * (microseconds / 1000000) + (microseconds % 1000000) / 977;
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
    common_hal_mcu_disable_interrupts();
    #if CIRCUITPY_USB_HOST
    if (!background_callback_pending() && !tud_task_event_ready() && !tuh_task_event_ready() && !_woken_up) {
    #else
    if (!background_callback_pending() && !tud_task_event_ready() && !_woken_up) {
        #endif
        __DSB();
        __WFI();
    }
    common_hal_mcu_enable_interrupts();
}

/**
 * \brief Default interrupt handler for unused IRQs.
 */
extern void isr_hardfault(void); // provide a prototype to avoid a missing-prototypes diagnostic
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

void port_yield() {
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
