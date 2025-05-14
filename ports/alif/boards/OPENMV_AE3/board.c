/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 OpenMV LLC.
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

#include "py/mphal.h"
#include "ospi_ext.h"
#include "ospi_flash.h"
#include "se_services.h"

#define OMV_BOOT_MAGIC_ADDR     (0x200FFFFCU)
#define OMV_BOOT_MAGIC_VALUE    (0xB00710ADU)

#if CORE_M55_HP
#define NPU_IRQ_NUMBER          NPU_HP_IRQ_IRQn
#define NPU_BASE_ADDRESS        (void *)NPU_HP_BASE
#else
#define NPU_IRQ_NUMBER          NPU_HE_IRQ_IRQn
#define NPU_BASE_ADDRESS        (void *)NPU_HE_BASE
#endif

typedef struct {
    volatile uint32_t ID;       // 0x0
    volatile uint32_t STATUS;   // 0x4
    volatile uint32_t CMD;      // 0x8
    volatile uint32_t RESET;    // 0xC
} npu_regs_t;

#define NPU ((npu_regs_t *)NPU_BASE_ADDRESS)

const ospi_pin_settings_t ospi_pin_settings = {
    .peripheral_number = 0,
    .pin_reset = pin_FLASH_RESET,
    .pin_cs = pin_FLASH_CS,
    .pin_clk_p = pin_FLASH_SCLK_P,
    .pin_clk_n = pin_FLASH_SCLK_N,
    .pin_rwds = pin_FLASH_DQSM,
    .pin_d0 = pin_FLASH_D0,
    .pin_d1 = pin_FLASH_D1,
    .pin_d2 = pin_FLASH_D2,
    .pin_d3 = pin_FLASH_D3,
    .pin_d4 = pin_FLASH_D4,
    .pin_d5 = pin_FLASH_D5,
    .pin_d6 = pin_FLASH_D6,
    .pin_d7 = pin_FLASH_D7,
};

const ospi_flash_settings_t ospi_flash_settings[] = {
    {
        .jedec_id = 0x3980c2,
        .freq_hz = 100000000,
        .read_dummy_cycles = 10,
        OSPI_FLASH_SETTINGS_MX25,
    },
    {
        .jedec_id = 0x195b9d,
        .freq_hz = 100000000,
        .read_dummy_cycles = 9,
        OSPI_FLASH_SETTINGS_IS25,
    },
    {
        .jedec_id = 0x17bb6b,
        .freq_hz = 100000000,
        .read_dummy_cycles = 7,
        OSPI_FLASH_SETTINGS_EM,
    },
};
const size_t ospi_flash_settings_len = 3;

void board_startup(void) {
    // Switch the USB multiplexer to use the Alif USB port.
    mp_hal_pin_output(pin_USB_D_SEL);
    mp_hal_pin_high(pin_USB_D_SEL);
}

void board_enter_bootloader(void) {
    *((uint32_t *)OMV_BOOT_MAGIC_ADDR) = OMV_BOOT_MAGIC_VALUE;
    NVIC_SystemReset();
}

void board_early_init(void) {
    // Set default run profile
    run_profile_t run_profile = {
        .dcdc_mode = DCDC_MODE_PWM,
        .dcdc_voltage = DCDC_VOUT_0825,
        // CLK_SRC_LFRC or CLK_SRC_LFXO
        .aon_clk_src = CLK_SRC_LFXO,
        // CLK_SRC_HFRC, CLK_SRC_HFXO or CLK_SRC_PLL
        .run_clk_src = CLK_SRC_PLL,
        #if CORE_M55_HP
        .cpu_clk_freq = CLOCK_FREQUENCY_400MHZ,
        #else
        .cpu_clk_freq = CLOCK_FREQUENCY_160MHZ,
        #endif
        .scaled_clk_freq = SCALED_FREQ_XO_HIGH_DIV_38_4_MHZ,
        // AON, modem aon, SSE-700 AON, modem, SYSTOP, DEBUG, SE
        .power_domains = PD_VBAT_AON_MASK | PD_SSE700_AON_MASK | PD_SYST_MASK |
            PD_DBSS_MASK | PD_SESS_MASK | PD_SRAMS_MASK | PD_SRAM_CTRL_AON_MASK,
        // Add all memories
        .memory_blocks = SERAM_MASK | SRAM0_MASK | SRAM1_MASK | MRAM_MASK | BACKUP4K_MASK |
            SRAM6A_MASK | SRAM6B_MASK | SRAM7_1_MASK | SRAM7_2_MASK | SRAM7_3_MASK |
            SRAM8_MASK | SRAM9_MASK | FWRAM_MASK,
        .phy_pwr_gating = LDO_PHY_MASK | USB_PHY_MASK | MIPI_TX_DPHY_MASK | MIPI_RX_DPHY_MASK |
            MIPI_PLL_DPHY_MASK,
        .vdd_ioflex_3V3 = IOFLEX_LEVEL_3V3,
    };

    if (se_services_set_run_profile(&run_profile)) {
        MICROPY_BOARD_FATAL_ERROR("se_services_set_run_profile");
    }

    // Set default off profile
    off_profile_t off_profile = {
        .dcdc_mode = DCDC_MODE_PWM,
        .dcdc_voltage = DCDC_VOUT_0825,
        // CLK_SRC_LFRC or CLK_SRC_LFXO
        .aon_clk_src = CLK_SRC_LFXO,
        // CLK_SRC_HFRC, CLK_SRC_HFXO or CLK_SRC_PLL
        .stby_clk_src = CLK_SRC_HFRC,
        .stby_clk_freq = SCALED_FREQ_RC_STDBY_76_8_MHZ,
        // Disable all power domains.
        .power_domains = 0,
        // Add all memories
        .memory_blocks = SERAM_MASK | SRAM0_MASK | SRAM1_MASK | MRAM_MASK | BACKUP4K_MASK |
            SRAM6A_MASK | SRAM6B_MASK | SRAM7_1_MASK | SRAM7_2_MASK | SRAM7_3_MASK |
            SRAM8_MASK | SRAM9_MASK | FWRAM_MASK,
        .phy_pwr_gating = LDO_PHY_MASK | USB_PHY_MASK | MIPI_TX_DPHY_MASK | MIPI_RX_DPHY_MASK |
            MIPI_PLL_DPHY_MASK,
        .vdd_ioflex_3V3 = IOFLEX_LEVEL_3V3,
        .vtor_address = SCB->VTOR,
        .vtor_address_ns = SCB->VTOR,
        .ewic_cfg = EWIC_RTC_A,
        .wakeup_events = WE_LPRTC,
    };

    if (se_services_set_off_profile(&off_profile)) {
        MICROPY_BOARD_FATAL_ERROR("se_services_set_off_profile");
    }

    // Select PLL for PD4 memory.
    if (se_services_select_pll_source(PLL_SOURCE_PLL, PLL_TARGET_PD4_SRAM)) {
        MICROPY_BOARD_FATAL_ERROR("se_services_select_pll_source");
    }
}

MP_WEAK void board_enter_stop(void) {
    // Disable NPU interrupt
    NVIC_DisableIRQ(NPU_IRQ_NUMBER);
    NVIC_ClearPendingIRQ(NPU_IRQ_NUMBER);

    // Soft-reset NPU
    NPU->RESET = 0x00000000;

    // Wait until reset
    uint32_t data = 0;
    do {
        // Poll channel0 status registers
        data = NPU->STATUS;
    } while (data);

    // Set default value, enables off for clocks and power.
    NPU->CMD = 0x0000000C;
}

MP_WEAK void board_enter_standby(void) {

}

MP_WEAK void board_exit_standby(void) {

}
