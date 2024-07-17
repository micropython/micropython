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
#include "ospi_flash.h"
#include "ospi_drv.h"
#include "pinconf.h"

#define CMD_RDSR        (0x05)
#define CMD_WREN        (0x06)
#define CMD_SEC_ERASE_32ADDR (0x21) // 4kiB sector erase with 32-bit address
#define CMD_WRVOL       (0x81)
#define CMD_RD_DEVID    (0x9f)

#define WAIT_SR_TIMEOUT (1000000)

// maximum bytes we can write in one SPI transfer
// limited by 256 byte FIFO buffer (can't go up to 256)
// need to use DMA to make this 256
#define PAGE_SIZE       (128)

#define ISSI_MODE_OCTAL_DDR_DQS (0xe7)

// All OSPI1 pins use the same alternate function.
#define OSPI1_PIN_FUNCTION PINMUX_ALTERNATE_FUNCTION_1

typedef struct _mp_spiflash_t {
    ospi_flash_cfg_t cfg;
} mp_spiflash_t;

static mp_spiflash_t global_flash;

// Alif version of this function can overwrite the destination buffer.
static void ospi_recv_blocking2(ospi_flash_cfg_t *ospi_cfg, uint32_t command, uint8_t *buffer) {
    uint32_t val;

    ospi_writel(ospi_cfg, data_reg, command);
    ospi_writel(ospi_cfg, ser, ospi_cfg->ser);

    ospi_cfg->rx_cnt = 0;

    while (ospi_cfg->rx_cnt < ospi_cfg->rx_req) {
        unsigned int timeout = 100000;
        while (ospi_readl(ospi_cfg, rxflr) == 0) {
            if (--timeout == 0) {
                return;
            }
        }
        val = ospi_readl(ospi_cfg, data_reg);
        *buffer++ = (uint8_t)val;
        ospi_cfg->rx_cnt++;
    }
}

static int mp_spiflash_read_cmd(mp_spiflash_t *self, uint8_t cmd, size_t len, uint8_t *dest) {
    ospi_setup_read(&self->cfg, 0, len, 8);
    ospi_recv_blocking2(&self->cfg, cmd, dest);
    return 0;
}

static int mp_spiflash_write_cmd(mp_spiflash_t *self, uint8_t cmd) {
    ospi_setup_write(&self->cfg, 0);
    ospi_send_blocking(&self->cfg, cmd);
    return 0;
}

static int mp_spiflash_wait_sr(mp_spiflash_t *self, uint8_t mask, uint8_t val, uint32_t timeout) {
    do {
        uint8_t sr;
        int ret = mp_spiflash_read_cmd(self, CMD_RDSR, 1, &sr);
        if (ret != 0) {
            return ret;
        }
        if ((sr & mask) == val) {
            return 0; // success
        }
    } while (timeout--);

    return -MP_ETIMEDOUT;
}

static int mp_spiflash_wait_wel1(mp_spiflash_t *self) {
    return mp_spiflash_wait_sr(self, 2, 2, WAIT_SR_TIMEOUT);
}

static int mp_spiflash_wait_wip0(mp_spiflash_t *self) {
    return mp_spiflash_wait_sr(self, 1, 0, WAIT_SR_TIMEOUT);
}

static uint32_t ospi_flash_read_id(mp_spiflash_t *self) {
    uint8_t buf[8];
    ospi_setup_read(&self->cfg, 0, 3, ospi_flash_settings.read_id_dummy_cycles);
    ospi_recv_blocking2(&self->cfg, CMD_RD_DEVID, buf);
    return buf[0] | buf[1] << 8 | buf[2] << 16;
}

static void ospi_flash_write_reg_sdr(mp_spiflash_t *self, uint8_t cmd, uint8_t addr, uint8_t value) {
    mp_spiflash_write_cmd(self, CMD_WREN);
    ospi_setup_write_sdr(&self->cfg, 6);
    ospi_push(&self->cfg, cmd);
    ospi_push(&self->cfg, 0x00);
    ospi_push(&self->cfg, 0x00);
    ospi_push(&self->cfg, addr);
    ospi_send_blocking(&self->cfg, value);
}

int ospi_flash_init(void) {
    mp_spiflash_t *self = &global_flash;

    uint32_t pad_ctrl = PADCTRL_OUTPUT_DRIVE_STRENGTH_12MA | PADCTRL_SLEW_RATE_FAST | PADCTRL_READ_ENABLE;

    pinconf_set(pin_OSPI1_CS->port, pin_OSPI1_CS->pin, OSPI1_PIN_FUNCTION, PADCTRL_OUTPUT_DRIVE_STRENGTH_12MA);
    pinconf_set(pin_OSPI1_SCLK->port, pin_OSPI1_SCLK->pin, OSPI1_PIN_FUNCTION, PADCTRL_OUTPUT_DRIVE_STRENGTH_12MA | PADCTRL_SLEW_RATE_FAST);
    pinconf_set(pin_OSPI1_D0->port, pin_OSPI1_D0->pin, OSPI1_PIN_FUNCTION, pad_ctrl);
    pinconf_set(pin_OSPI1_D1->port, pin_OSPI1_D1->pin, OSPI1_PIN_FUNCTION, pad_ctrl);
    pinconf_set(pin_OSPI1_D2->port, pin_OSPI1_D2->pin, OSPI1_PIN_FUNCTION, pad_ctrl);
    pinconf_set(pin_OSPI1_D3->port, pin_OSPI1_D3->pin, OSPI1_PIN_FUNCTION, pad_ctrl);
    #if defined(pin_OSPI1_D4)
    pinconf_set(pin_OSPI1_D4->port, pin_OSPI1_D4->pin, OSPI1_PIN_FUNCTION, pad_ctrl);
    pinconf_set(pin_OSPI1_D5->port, pin_OSPI1_D5->pin, OSPI1_PIN_FUNCTION, pad_ctrl);
    pinconf_set(pin_OSPI1_D6->port, pin_OSPI1_D6->pin, OSPI1_PIN_FUNCTION, pad_ctrl);
    pinconf_set(pin_OSPI1_D7->port, pin_OSPI1_D7->pin, OSPI1_PIN_FUNCTION, pad_ctrl);
    #endif
    #if defined(pin_OSPI1_RXDS)
    pinconf_set(pin_OSPI1_RXDS->port, pin_OSPI1_RXDS->pin, OSPI1_PIN_FUNCTION, pad_ctrl);
    #endif

    #if defined(pin_OSPI1_RXDS)
    if (pin_OSPI1_RXDS->port == PORT_10 && pin_OSPI1_RXDS->pin == PIN_7) {
        // Alif: P5_6 is needed to support proper alt function selection of P10_7.
        pinconf_set(PORT_5, PIN_6, OSPI1_PIN_FUNCTION, pad_ctrl);
    }
    #endif

    // Reset the SPI flash.
    mp_hal_pin_output(pin_OSPI1_RESET);
    mp_hal_pin_low(pin_OSPI1_RESET);
    mp_hal_delay_us(30);
    mp_hal_pin_high(pin_OSPI1_RESET);

    // Configure the OSPI peripheral.
    self->cfg.regs = (ssi_regs_t *)OSPI1_BASE;
    self->cfg.aes_regs = (aes_regs_t *)AES1_BASE;
    self->cfg.xip_base = (volatile void *)OSPI1_XIP_BASE;
    self->cfg.ser = 1;
    self->cfg.addrlen = 8; // 32-bit address length
    self->cfg.ospi_clock = ospi_flash_settings.freq_mhz;
    self->cfg.ddr_en = 0;
    self->cfg.wait_cycles = 0; // used only for ospi_xip_exit
    ospi_init(&self->cfg);

    if (ospi_flash_settings.is_oct && ospi_flash_settings.is_ddr) {
        // Switch SPI flash to Octal DDR mode.
        ospi_flash_write_reg_sdr(self, CMD_WRVOL, 0x00, ISSI_MODE_OCTAL_DDR_DQS);
        self->cfg.ddr_en = 1;
    }

    // Check the device ID.
    if (ospi_flash_read_id(self) != ospi_flash_settings.jedec_id) {
        return -1;
    }

    return 0;
}

int ospi_flash_erase_sector(uint32_t addr) {
    mp_spiflash_t *self = &global_flash;

    mp_spiflash_write_cmd(self, CMD_WREN);
    int ret = mp_spiflash_wait_wel1(self);
    if (ret < 0) {
        return ret;
    }

    ospi_setup_write(&self->cfg, 8 /* 32-bit addr len*/);
    ospi_push(&self->cfg, CMD_SEC_ERASE_32ADDR);
    ospi_send_blocking(&self->cfg, addr);

    return mp_spiflash_wait_wip0(self);
}

int ospi_flash_read(uint32_t addr, uint32_t len, uint8_t *dest) {
    // OSPI FIFO is limited to 256 bytes.  Need DMA to get a longer read.
    mp_spiflash_t *self = &global_flash;

    while (len) {
        uint32_t l = len;
        if (l > 256) {
            l = 256;
        }
        ospi_setup_read(&self->cfg, 8 /* 32-bit addr len*/, l, ospi_flash_settings.read_dummy_cycles);
        ospi_push(&self->cfg, ospi_flash_settings.read_command);
        ospi_recv_blocking2(&self->cfg, addr, dest);
        addr += l;
        len -= l;
        dest += l;
    }

    return 0;
}

static int ospi_flash_write_page(uint32_t addr, uint32_t len, const uint8_t *src) {
    mp_spiflash_t *self = &global_flash;

    mp_spiflash_write_cmd(self, CMD_WREN);
    int ret = mp_spiflash_wait_wel1(self);
    if (ret < 0) {
        return ret;
    }

    ospi_setup_write(&self->cfg, 8 /* 32-bit addr len*/);
    ospi_push(&self->cfg, ospi_flash_settings.write_command);
    ospi_push(&self->cfg, addr);
    while (--len) {
        ospi_push(&self->cfg, *src++);
    }
    ospi_send_blocking(&self->cfg, *src);

    return mp_spiflash_wait_wip0(self);
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
