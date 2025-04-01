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

#include "py/mperrno.h"
#include "py/mphal.h"

#if MICROPY_HW_ENABLE_OSPI

#include "ospi_ext.h"
#include "ospi_flash.h"
#include "ospi_drv.h"
#include "ospi_xip_user.h"
#include "pinconf.h"

#define WAIT_SR_TIMEOUT     (1000000)

// Generic SPI flash commands.
#define CMD_SPI_WREN (0x06)

// This is the maximum number of bytes that can be written to SPI flash at once.
#define PAGE_SIZE (256)

// All OSP0/OSPI1 pins use the same alternate function.
#define OSPI_PIN_FUNCTION PINMUX_ALTERNATE_FUNCTION_1

typedef struct _ospi_flash_t {
    const ospi_pin_settings_t *pin;
    const ospi_flash_settings_t *set;
    ospi_flash_cfg_t cfg;
    bool xip_active;
} ospi_flash_t;

static ospi_flash_t global_flash;

/******************************************************************************/
// Generic SPI-flash helper functions.

static void ospi_flash_wren_spi(ospi_flash_t *self) {
    uint8_t buf[1] = {CMD_SPI_WREN};
    ospi_spi_transfer(&self->cfg, 1, buf, buf);
}

static int ospi_flash_read_cmd(ospi_flash_t *self, uint32_t cmd, uint8_t cmd_dummy_cycles, size_t len, uint8_t *dest) {
    int ret = 0;
    if (self->set->inst_len == OSPI_INST_L_8bit) {
        ospi_setup_read_ext(&self->cfg, self->set->rxds, self->set->inst_len, OSPI_ADDR_L_0bit, OSPI_DATA_L_8bit, len, cmd_dummy_cycles);
        ret = ospi_recv_blocking_8bit_data(&self->cfg, cmd, dest);
    } else {
        uint16_t dest16 = 0;
        ospi_setup_read_ext(&self->cfg, self->set->rxds, self->set->inst_len, OSPI_ADDR_L_32bit, OSPI_DATA_L_16bit, len, cmd_dummy_cycles);
        ospi_push(&self->cfg, cmd);
        ret = ospi_recv_blocking_16bit_data(&self->cfg, 0 /* addr */, &dest16);
        *dest = dest16;
    }
    return ret;
}

static int ospi_flash_write_cmd_addr(ospi_flash_t *self, uint32_t cmd, uint32_t addr_len, uint32_t addr) {
    if (self->set->inst_len == OSPI_INST_L_8bit) {
        ospi_setup_write_ext(&self->cfg, false, self->set->inst_len, addr_len, OSPI_DATA_L_8bit);
    } else {
        ospi_setup_write_ext(&self->cfg, self->set->rxds, self->set->inst_len, addr_len, OSPI_DATA_L_16bit);
    }
    if (addr_len == OSPI_ADDR_L_0bit) {
        ospi_send_blocking(&self->cfg, cmd);
    } else {
        ospi_push(&self->cfg, cmd);
        ospi_send_blocking(&self->cfg, addr);
    }
    return 0;
}

static int ospi_flash_write_cmd(ospi_flash_t *self, uint32_t cmd) {
    return ospi_flash_write_cmd_addr(self, cmd, OSPI_ADDR_L_0bit, 0);
}

static uint32_t ospi_flash_read_id_spi(ospi_flash_t *self) {
    uint8_t buf[4] = {0x9f, 0, 0, 0};
    ospi_spi_transfer(&self->cfg, 4, buf, buf);
    return buf[1] | buf[2] << 8 | buf[3] << 16;
}

static int ospi_flash_wait_sr(ospi_flash_t *self, uint8_t mask, uint8_t val, uint32_t timeout) {
    do {
        uint8_t sr = 0;
        int ret = ospi_flash_read_cmd(self, self->set->read_sr, self->set->read_sr_dummy_cycles, 1, &sr);
        if (ret != 0) {
            return ret;
        }
        if ((sr & mask) == val) {
            return 0; // success
        }
    } while (timeout--);

    return -MP_ETIMEDOUT;
}

static int ospi_flash_wait_wel1(ospi_flash_t *self) {
    return ospi_flash_wait_sr(self, 2, 2, WAIT_SR_TIMEOUT);
}

static int ospi_flash_wait_wip0(ospi_flash_t *self) {
    return ospi_flash_wait_sr(self, 1, 0, WAIT_SR_TIMEOUT);
}

static uint32_t ospi_flash_read_id(ospi_flash_t *self) {
    uint8_t buf[4] = {0};
    if (self->set->inst_len == OSPI_INST_L_8bit) {
        ospi_setup_read_ext(&self->cfg, self->set->rxds, self->set->inst_len, OSPI_ADDR_L_0bit, OSPI_DATA_L_8bit, 3, self->set->read_id_dummy_cycles);
        ospi_recv_blocking_8bit_data(&self->cfg, self->set->read_id, buf);
    } else {
        ospi_setup_read_ext(&self->cfg, self->set->rxds, self->set->inst_len, OSPI_ADDR_L_32bit, OSPI_DATA_L_16bit, 4, self->set->read_id_dummy_cycles);
        ospi_push(&self->cfg, self->set->read_id);
        // Read 8-bit values because data is in SDR mode for read id.
        ospi_recv_blocking_8bit_data(&self->cfg, 0, buf);
    }
    return buf[0] | buf[1] << 8 | buf[2] << 16;
}

/******************************************************************************/
// Functions specific to ISSI flash chips.

int ospi_flash_issi_init(ospi_flash_t *self) {
    const uint8_t cmd_wrvol = 0x81;

    // Configure dummy cycles.
    ospi_flash_wren_spi(self);
    uint8_t buf0[5] = {cmd_wrvol, 0, 0, 1, self->set->read_dummy_cycles};
    ospi_spi_transfer(&self->cfg, sizeof(buf0), buf0, buf0);

    // Switch SPI flash to Octal DDR mode.
    const uint8_t issi_mode_octal_ddr_dqs = 0xe7;
    ospi_flash_wren_spi(self);
    uint8_t buf1[5] = {cmd_wrvol, 0, 0, 0, issi_mode_octal_ddr_dqs};
    ospi_spi_transfer(&self->cfg, sizeof(buf1), buf1, buf1);
    self->cfg.ddr_en = 1;
    return 0;
}

/******************************************************************************/
// Functions specific to MX flash chips.

int ospi_flash_mx_init(ospi_flash_t *self) {
    const uint8_t cmd_wrcr2 = 0x72;
    const uint8_t mx_mode_enable_sopi = 0x01;
    const uint8_t mx_mode_enable_dopi = 0x02;
    uint8_t mx_mode;
    if (self->set->octal_mode == OSPI_FLASH_OCTAL_MODE_DDD) {
        mx_mode = mx_mode_enable_dopi;
    } else {
        mx_mode = mx_mode_enable_sopi;
    }

    // Configure dummy cycles.
    uint8_t ddc_value = 0;
    const uint8_t ospi_flash_mx_ddc[][2] = {
        {20, 0}, {18, 1}, {16, 2}, {14, 3}, {12, 4}, {10, 5}, {8, 6}, {6, 7}
    };
    for (size_t i = 0; i < MP_ARRAY_SIZE(ospi_flash_mx_ddc); i++) {
        if (self->set->read_dummy_cycles == ospi_flash_mx_ddc[i][0]) {
            ddc_value = ospi_flash_mx_ddc[i][1];
            break;
        }
    }

    ospi_flash_wren_spi(self);
    uint8_t buf0[6] = {cmd_wrcr2, 0, 0, 3, 0, ddc_value};
    ospi_spi_transfer(&self->cfg, sizeof(buf0), buf0, buf0);

    // Switch SPI flash to Octal SDR or DDR mode.
    ospi_flash_wren_spi(self);
    uint8_t buf1[6] = {cmd_wrcr2, 0, 0, 0, 0, mx_mode};
    ospi_spi_transfer(&self->cfg, sizeof(buf1), buf1, buf1);
    if (self->set->octal_mode == OSPI_FLASH_OCTAL_MODE_DDD) {
        self->cfg.ddr_en = 1;
    } else {
        self->cfg.ddr_en = 0;
    }
    return 0;
}

static uint8_t ospi_flash_mx_read_cr_helper(ospi_flash_t *self, uint16_t command, uint32_t addr) {
    // TODO: currently only works in DDR mode

    uint16_t buf[1] = {0};
    ospi_setup_read_ext(&self->cfg, self->set->rxds, OSPI_INST_L_16bit, OSPI_ADDR_L_32bit, OSPI_DATA_L_16bit, 1, 4);
    ospi_push(&self->cfg, command);
    ospi_recv_blocking_16bit_data(&self->cfg, addr, buf);
    return buf[0] & 0xff;
}

uint8_t ospi_flash_mx_read_cr(ospi_flash_t *self) {
    return ospi_flash_mx_read_cr_helper(self, 0x15ea, 0);
}

uint8_t ospi_flash_mx_read_cr2(ospi_flash_t *self, uint32_t addr) {
    return ospi_flash_mx_read_cr_helper(self, 0x718e, addr);
}

static int ospi_flash_mx_write_cr_helper(ospi_flash_t *self, uint16_t command, uint32_t addr, uint8_t value) {
    // TODO: currently only works in DDR mode

    // Enable writes so that the register can be modified.
    ospi_flash_write_cmd(self, self->set->write_en);
    int ret = ospi_flash_wait_wel1(self);
    if (ret < 0) {
        return ret;
    }

    // Do the write.
    ospi_setup_write_ext(&self->cfg, self->set->rxds, OSPI_INST_L_16bit, OSPI_ADDR_L_32bit, OSPI_DATA_L_16bit);
    ospi_push(&self->cfg, command);
    ospi_push(&self->cfg, addr);
    ospi_push(&self->cfg, value << 8); // in DDR mode, MSByte contains the register value to write
    ospi_writel((&self->cfg), ser, self->cfg.ser);
    while ((ospi_readl((&self->cfg), sr) & (SR_TF_EMPTY | SR_BUSY)) != SR_TF_EMPTY) {
    }

    // Wait for the write to finish.
    return ospi_flash_wait_wip0(self);
}

int ospi_flash_mx_write_cr(ospi_flash_t *self, uint8_t value) {
    return ospi_flash_mx_write_cr_helper(self, 0x01fe, 1, value);
}

int ospi_flash_mx_write_cr2(ospi_flash_t *self, uint32_t addr, uint8_t value) {
    return ospi_flash_mx_write_cr_helper(self, 0x728d, addr, value);
}

/******************************************************************************/
// SPI flash initialisation.

int ospi_flash_init(void) {
    ospi_flash_t *self = &global_flash;

    const ospi_pin_settings_t *pin = &ospi_pin_settings;
    const ospi_flash_settings_t *set = NULL;

    self->pin = pin;

    unsigned int unit = pin->peripheral_number;
    mp_hal_pin_config(pin->pin_cs, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE,
        MP_HAL_PIN_SPEED_LOW, MP_HAL_PIN_DRIVE_12MA, MP_HAL_PIN_ALT(OSPI_SS0, unit), false);
    mp_hal_pin_config(pin->pin_clk_p, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE,
        MP_HAL_PIN_SPEED_HIGH, MP_HAL_PIN_DRIVE_12MA, MP_HAL_PIN_ALT(OSPI_SCLK, unit), false);
    if (pin->pin_clk_n != NULL) {
        mp_hal_pin_config(pin->pin_clk_n, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE,
            MP_HAL_PIN_SPEED_HIGH, MP_HAL_PIN_DRIVE_12MA, MP_HAL_PIN_ALT(OSPI_SCLKN, unit), false);
    }
    if (pin->pin_rwds != NULL) {
        mp_hal_pin_config(pin->pin_rwds, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE,
            MP_HAL_PIN_SPEED_HIGH, MP_HAL_PIN_DRIVE_12MA, MP_HAL_PIN_ALT(OSPI_RXDS, unit), true);
        if (pin->pin_rwds->port == PORT_10 && pin->pin_rwds->pin == PIN_7) {
            // Alif: P5_6 is needed to support proper alt function selection of P10_7.
            mp_hal_pin_config(pin_P5_6, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE,
                MP_HAL_PIN_SPEED_HIGH, MP_HAL_PIN_DRIVE_12MA, MP_HAL_PIN_ALT(OSPI_RXDS, unit), true);
        }
    }
    mp_hal_pin_config(pin->pin_d0, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE,
        MP_HAL_PIN_SPEED_HIGH, MP_HAL_PIN_DRIVE_12MA, MP_HAL_PIN_ALT(OSPI_D0, unit), true);
    mp_hal_pin_config(pin->pin_d1, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE,
        MP_HAL_PIN_SPEED_HIGH, MP_HAL_PIN_DRIVE_12MA, MP_HAL_PIN_ALT(OSPI_D1, unit), true);
    mp_hal_pin_config(pin->pin_d2, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_UP,
        MP_HAL_PIN_SPEED_HIGH, MP_HAL_PIN_DRIVE_12MA, MP_HAL_PIN_ALT(OSPI_D2, unit), true);
    mp_hal_pin_config(pin->pin_d3, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE,
        MP_HAL_PIN_SPEED_HIGH, MP_HAL_PIN_DRIVE_12MA, MP_HAL_PIN_ALT(OSPI_D3, unit), true);
    if (pin->pin_d4 != NULL) {
        mp_hal_pin_config(pin->pin_d4, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE,
            MP_HAL_PIN_SPEED_HIGH, MP_HAL_PIN_DRIVE_12MA, MP_HAL_PIN_ALT(OSPI_D4, unit), true);
        mp_hal_pin_config(pin->pin_d5, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE,
            MP_HAL_PIN_SPEED_HIGH, MP_HAL_PIN_DRIVE_12MA, MP_HAL_PIN_ALT(OSPI_D5, unit), true);
        mp_hal_pin_config(pin->pin_d6, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE,
            MP_HAL_PIN_SPEED_HIGH, MP_HAL_PIN_DRIVE_12MA, MP_HAL_PIN_ALT(OSPI_D6, unit), true);
        mp_hal_pin_config(pin->pin_d7, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE,
            MP_HAL_PIN_SPEED_HIGH, MP_HAL_PIN_DRIVE_12MA, MP_HAL_PIN_ALT(OSPI_D7, unit), true);
    }

    // Reset the SPI flash.
    mp_hal_pin_output(pin->pin_reset);
    mp_hal_pin_low(pin->pin_reset);
    mp_hal_delay_us(100);
    mp_hal_pin_high(pin->pin_reset);
    mp_hal_delay_us(1000);

    // Configure the OSPI peripheral.
    if (pin->peripheral_number == 0) {
        self->cfg.regs = (ssi_regs_t *)OSPI0_BASE;
        self->cfg.aes_regs = (aes_regs_t *)AES0_BASE;
        self->cfg.xip_base = (volatile void *)OSPI0_XIP_BASE;
    } else {
        self->cfg.regs = (ssi_regs_t *)OSPI1_BASE;
        self->cfg.aes_regs = (aes_regs_t *)AES1_BASE;
        self->cfg.xip_base = (volatile void *)OSPI1_XIP_BASE;
    }
    self->cfg.ser = 1; // enable slave select
    self->cfg.addrlen = 8; // 32-bit address length
    self->cfg.ddr_en = 0;
    self->cfg.wait_cycles = 0; // used only for ospi_xip_exit
    self->cfg.ospi_clock = 100000;  // use 100KHz for detection.
    ospi_init(&self->cfg);

    // Read the device ID.
    uint32_t jedec_id = ospi_flash_read_id_spi(self);

    // Auto-detect the flash.
    for (size_t i = 0; i < ospi_flash_settings_len; i++) {
        set = &ospi_flash_settings[i];
        if (jedec_id == set->jedec_id) {
            self->set = set;
            break;
        }
    }

    if (self->set == NULL) {
        // Flash part is not supported.
        return -1;
    }

    // Switch to the higher frequency.
    self->cfg.ospi_clock = set->freq_hz;
    ospi_init(&self->cfg);

    // Switch to octal mode if needed.
    if (set->flash_init != NULL) {
        set->flash_init(self);

        // Check the device ID after switching mode.
        if (ospi_flash_read_id(self) != set->jedec_id) {
            return -1;
        }
    }

    // Enter XIP mode.
    ospi_flash_xip_enter(self);

    return 0;
}

uintptr_t ospi_flash_get_xip_base(void) {
    ospi_flash_t *self = &global_flash;
    return (uintptr_t)self->cfg.xip_base;
}

int ospi_flash_xip_enter(ospi_flash_t *self) {
    if (!self->xip_active) {
        uint32_t irq_state = disable_irq();
        self->xip_active = true;
        ospi_xip_enter_ext(&self->cfg, self->set->inst_len, self->set->xip_data_len,
            self->set->read_command, self->set->read_command, self->set->read_dummy_cycles);
        enable_irq(irq_state);
    }
    return 0;
}

int ospi_flash_xip_exit(ospi_flash_t *self) {
    if (self->xip_active) {
        uint32_t irq_state = disable_irq();
        ospi_xip_exit_ext(&self->cfg, self->set->inst_len, self->set->read_command, self->set->read_command);
        self->xip_active = false;
        enable_irq(irq_state);
    }
    return 0;
}

int ospi_flash_xip_restore(ospi_flash_t *self) {
    if (self->xip_active) {
        ospi_xip_restore_ext(&self->cfg, self->set->xip_data_len);
    }
    return 0;
}

/******************************************************************************/
// Top-level read/erase/write functions.

int ospi_flash_erase_sector(uint32_t addr) {
    ospi_flash_t *self = &global_flash;

    ospi_flash_write_cmd(self, self->set->write_en);
    int ret = ospi_flash_wait_wel1(self);
    if (ret == 0) {
        ospi_flash_write_cmd_addr(self, self->set->erase_command, OSPI_ADDR_L_32bit, addr);
        ret = ospi_flash_wait_wip0(self);
    }

    ospi_flash_xip_restore(self);

    return ret;
}

int ospi_flash_read(uint32_t addr, uint32_t len, uint8_t *dest) {
    ospi_flash_t *self = &global_flash;
    // Perform an XIP read (memcpy)
    memcpy(dest, (void *)self->cfg.xip_base + addr, len);
    return 0;
}

static int ospi_flash_write_page(uint32_t addr, uint32_t len, const uint8_t *src) {
    ospi_flash_t *self = &global_flash;

    ospi_flash_write_cmd(self, self->set->write_en);
    int ret = ospi_flash_wait_wel1(self);
    if (ret < 0) {
        return ret;
    }

    ospi_setup_write_ext(&self->cfg, self->set->rxds, self->set->inst_len, OSPI_ADDR_L_32bit, OSPI_DATA_L_32bit);
    ospi_push(&self->cfg, self->set->write_command);
    ospi_push(&self->cfg, addr);

    const uint32_t *src32 = (const uint32_t *)src;
    if (self->set->bswap16) {
        // MX flashes swap 16-bit words when read in 8D-8D-8D.
        for (; len; len -= 4) {
            ospi_push(&self->cfg, __ROR(*src32++, 16));
        }
    } else {
        // For the rest of the flashes, we just correct the endianness.
        for (; len; len -= 4) {
            ospi_push(&self->cfg, __REV(*src32++));
        }
    }

    ospi_writel((&self->cfg), ser, self->cfg.ser);
    while ((ospi_readl((&self->cfg), sr) & (SR_TF_EMPTY | SR_BUSY)) != SR_TF_EMPTY) {
    }

    return ospi_flash_wait_wip0(self);
}

int ospi_flash_write(uint32_t addr, uint32_t len, const uint8_t *src) {
    ospi_flash_t *self = &global_flash;

    int ret = 0;
    uint32_t offset = addr & (PAGE_SIZE - 1);

    while (len) {
        size_t rest = PAGE_SIZE - offset;
        if (rest > len) {
            rest = len;
        }
        ret = ospi_flash_write_page(addr, rest, src);
        if (ret != 0) {
            break;
        }
        len -= rest;
        addr += rest;
        src += rest;
        offset = 0;
    }

    ospi_flash_xip_restore(self);

    return ret;
}

#endif // MICROPY_HW_ENABLE_OSPI
