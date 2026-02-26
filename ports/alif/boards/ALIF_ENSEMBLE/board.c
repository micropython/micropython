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

const ospi_pin_settings_t ospi_pin_settings = {
    .peripheral_number = 1,
    .pin_reset = pin_OSPI1_RESET,
    .pin_cs = pin_OSPI1_CS,
    .pin_clk_p = pin_OSPI1_SCLK,
    .pin_clk_n = NULL,
    .pin_rwds = pin_OSPI1_RXDS,
    .pin_d0 = pin_OSPI1_D0,
    .pin_d1 = pin_OSPI1_D1,
    .pin_d2 = pin_OSPI1_D2,
    .pin_d3 = pin_OSPI1_D3,
    .pin_d4 = pin_OSPI1_D4,
    .pin_d5 = pin_OSPI1_D5,
    .pin_d6 = pin_OSPI1_D6,
    .pin_d7 = pin_OSPI1_D7,
};

const ospi_flash_settings_t ospi_flash_settings[] = {
    {
        .jedec_id = 0x1a5b9d,
        .freq_hz = 100000000,
        .read_dummy_cycles = 9,
        OSPI_FLASH_SETTINGS_IS25,
    },
};
const size_t ospi_flash_settings_len = 1;

void board_early_init(void) {
    off_profile_t off_profile = {
        .dcdc_mode = DCDC_MODE_PWM,
        .dcdc_voltage = DCDC_VOUT_0825,
        // CLK_SRC_LFRC or CLK_SRC_LFXO
        .aon_clk_src = CLK_SRC_LFXO,
        // CLK_SRC_HFRC, CLK_SRC_HFXO or CLK_SRC_PLL
        .stby_clk_src = CLK_SRC_HFRC,
        .stby_clk_freq = SCALED_FREQ_RC_STDBY_76_8_MHZ,
        // Disable all power domains except AON.
        .power_domains = PD_VBAT_AON_MASK,
        // Keep SERAM, MRAM and backup SRAM on.
        // (SRAM0 also needs to stay on because it's used for .bss.sram0 which is zerod
        // by the runtime before the run profile is configured.)
        .memory_blocks = SERAM_MASK | SRAM0_MASK | MRAM_MASK | BACKUP4K_MASK,
        // Gate the clocks of IP blocks.
        .ip_clock_gating = 0x3ffb,
        // Gate PHY power (saves 0.5uA).
        .phy_pwr_gating = LDO_PHY_MASK | USB_PHY_MASK | MIPI_TX_DPHY_MASK | MIPI_RX_DPHY_MASK |
            MIPI_PLL_DPHY_MASK,
        .vdd_ioflex_3V3 = IOFLEX_LEVEL_3V3,
        .vtor_address = SCB->VTOR,
        .vtor_address_ns = SCB->VTOR,
        // Configure wake-up sources.
        .ewic_cfg = EWIC_VBAT_GPIO | EWIC_VBAT_TIMER | EWIC_RTC_A,
        .wakeup_events = WE_LPGPIO7 | WE_LPGPIO6 | WE_LPGPIO5 | WE_LPGPIO4 | WE_LPTIMER0 | WE_LPRTC,
    };

    if (se_services_set_off_profile(&off_profile)) {
        MICROPY_BOARD_FATAL_ERROR("se_services_set_off_profile");
    }

    // Configure the joystick buttons as an input with pull-up enabled.
    mp_hal_pin_config(pin_JOY_LEFT, MP_HAL_PIN_MODE_INPUT, MP_HAL_PIN_PULL_UP, 0, 0, 0, true);
    mp_hal_pin_config(pin_JOY_RIGHT, MP_HAL_PIN_MODE_INPUT, MP_HAL_PIN_PULL_UP, 0, 0, 0, true);
    mp_hal_pin_config(pin_JOY_DOWN, MP_HAL_PIN_MODE_INPUT, MP_HAL_PIN_PULL_UP, 0, 0, 0, true);
    mp_hal_pin_config(pin_JOY_UP, MP_HAL_PIN_MODE_INPUT, MP_HAL_PIN_PULL_UP, 0, 0, 0, true);
    mp_hal_pin_config(pin_JOY_SW, MP_HAL_PIN_MODE_INPUT, MP_HAL_PIN_PULL_UP, 0, 0, 0, true);
}
