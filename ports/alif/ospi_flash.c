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

int ospi_flash_issi_octal_switch(ospi_flash_t *self) {
    // Switch SPI flash to Octal DDR mode.
    const uint8_t cmd_wrvol = 0x81;
    const uint8_t issi_mode_octal_ddr_dqs = 0xe7;
    ospi_flash_wren_spi(self);
    uint8_t buf[5] = {cmd_wrvol, 0, 0, 0, issi_mode_octal_ddr_dqs};
    ospi_spi_transfer(&self->cfg, sizeof(buf), buf, buf);
    self->cfg.ddr_en = 1;
    return 0;
}

/******************************************************************************/
// Functions specific to MX flash chips.

int ospi_flash_mx_octal_switch(ospi_flash_t *self) {
    // Switch SPI flash to Octal SDR or DDR mode (SOPI or DOPI) by writing to CR2.
    const uint8_t cmd_wrcr2 = 0x72;
    const uint8_t mx_mode_enable_sopi = 0x01;
    const uint8_t mx_mode_enable_dopi = 0x02;
    uint8_t mx_mode;
    if (self->set->octal_mode == OSPI_FLASH_OCTAL_MODE_DDD) {
        mx_mode = mx_mode_enable_dopi;
    } else {
        mx_mode = mx_mode_enable_sopi;
    }
    ospi_flash_wren_spi(self);
    uint8_t buf[6] = {cmd_wrcr2, 0, 0, 0, 0, mx_mode};
    ospi_spi_transfer(&self->cfg, sizeof(buf), buf, buf);
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
    const ospi_flash_settings_t *set = &ospi_flash_settings;
    self->pin = pin;
    self->set = set;

    uint32_t pad_ctrl = PADCTRL_OUTPUT_DRIVE_STRENGTH_12MA | PADCTRL_SLEW_RATE_FAST | PADCTRL_READ_ENABLE;

    pinconf_set(pin->pin_cs->port, pin->pin_cs->pin, OSPI_PIN_FUNCTION, PADCTRL_OUTPUT_DRIVE_STRENGTH_12MA);
    pinconf_set(pin->pin_clk->port, pin->pin_clk->pin, OSPI_PIN_FUNCTION, PADCTRL_OUTPUT_DRIVE_STRENGTH_12MA | PADCTRL_SLEW_RATE_FAST);
    if (pin->pin_rwds != NULL) {
        pinconf_set(pin->pin_rwds->port, pin->pin_rwds->pin, OSPI_PIN_FUNCTION, pad_ctrl);
        if (pin->pin_rwds->port == PORT_10 && pin->pin_rwds->pin == PIN_7) {
            // Alif: P5_6 is needed to support proper alt function selection of P10_7.
            pinconf_set(PORT_5, PIN_6, OSPI_PIN_FUNCTION, pad_ctrl);
        }
    }
    pinconf_set(pin->pin_d0->port, pin->pin_d0->pin, OSPI_PIN_FUNCTION, pad_ctrl);
    pinconf_set(pin->pin_d1->port, pin->pin_d1->pin, OSPI_PIN_FUNCTION, pad_ctrl);
    pinconf_set(pin->pin_d2->port, pin->pin_d2->pin, OSPI_PIN_FUNCTION, pad_ctrl);
    pinconf_set(pin->pin_d3->port, pin->pin_d3->pin, OSPI_PIN_FUNCTION, pad_ctrl);
    if (pin->pin_d4 != NULL) {
        pinconf_set(pin->pin_d4->port, pin->pin_d4->pin, OSPI_PIN_FUNCTION, pad_ctrl);
        pinconf_set(pin->pin_d5->port, pin->pin_d5->pin, OSPI_PIN_FUNCTION, pad_ctrl);
        pinconf_set(pin->pin_d6->port, pin->pin_d6->pin, OSPI_PIN_FUNCTION, pad_ctrl);
        pinconf_set(pin->pin_d7->port, pin->pin_d7->pin, OSPI_PIN_FUNCTION, pad_ctrl);
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
    self->cfg.ospi_clock = set->freq_hz;
    self->cfg.ddr_en = 0;
    self->cfg.wait_cycles = 0; // used only for ospi_xip_exit
    ospi_init(&self->cfg);

    // Check the device ID before attempting to switch to octal mode (if needed).
    if (ospi_flash_read_id_spi(self) != set->jedec_id) {
        return -1;
    }

    // Switch to octal mode if needed.
    if (set->octal_switch != NULL) {
        set->octal_switch(self);

        // Check the device ID after switching mode.
        if (ospi_flash_read_id(self) != set->jedec_id) {
            return -1;
        }
    }

    return 0;
}

/******************************************************************************/
// Top-level read/erase/write functions.

int ospi_flash_erase_sector(uint32_t addr) {
    ospi_flash_t *self = &global_flash;

    ospi_flash_write_cmd(self, self->set->write_en);
    int ret = ospi_flash_wait_wel1(self);
    if (ret < 0) {
        return ret;
    }

    ospi_flash_write_cmd_addr(self, self->set->erase_command, OSPI_ADDR_L_32bit, addr);

    return ospi_flash_wait_wip0(self);
}

int ospi_flash_read(uint32_t addr, uint32_t len, uint8_t *dest) {
    // OSPI FIFO is limited to 256 bytes.  Need DMA to get a longer read.
    ospi_flash_t *self = &global_flash;

    while (len) {
        uint32_t l = len / 4;
        if (l > 256) {
            l = 256;
        }
        ospi_setup_read_ext(&self->cfg, self->set->rxds, self->set->inst_len, OSPI_ADDR_L_32bit, OSPI_DATA_L_32bit, l, self->set->read_dummy_cycles);
        ospi_push(&self->cfg, self->set->read_command);
        ospi_recv_blocking_32bit_data(&self->cfg, addr, (uint32_t *)dest);
        addr += l * 4;
        len -= l * 4;
        dest += l * 4;
    }

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
    for (; len; len -= 4) {
        ospi_push(&self->cfg, __ROR(*src32++, 16));
    }

    ospi_writel((&self->cfg), ser, self->cfg.ser);
    while ((ospi_readl((&self->cfg), sr) & (SR_TF_EMPTY | SR_BUSY)) != SR_TF_EMPTY) {
    }

    return ospi_flash_wait_wip0(self);
}

int ospi_flash_write(uint32_t addr, uint32_t len, const uint8_t *src) {
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
    return ret;
}

#endif // MICROPY_HW_ENABLE_OSPI
