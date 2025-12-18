/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024-2025 Damien P. George
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

// This XSPI driver is currently configured to run in 1-line (SPI) mode.
// It uses the mp_qspi_proto_t QSPI protocol and translates quad-commands
// into 1-line commands.

#include <string.h>
#include "py/mperrno.h"
#include "py/mphal.h"
#include "xspi.h"

#if defined(MICROPY_HW_XSPIFLASH_SIZE_BITS_LOG2)

#ifndef MICROPY_HW_XSPI_PRESCALER
#define MICROPY_HW_XSPI_PRESCALER (4) // F_CLK = F_AHB/4
#endif

#ifndef MICROPY_HW_XSPI_CS_HIGH_CYCLES
#define MICROPY_HW_XSPI_CS_HIGH_CYCLES (2) // nCS stays high for 4 cycles
#endif

// Currently hard-coded to use XSPI2 instance.
#define XSPIx (XSPI2)

// For XSPI2, PN0 through PN12.
#define XSPI2_AF (9)

typedef struct _xspi_flash_t {
    XSPI_TypeDef *xspi;
    uintptr_t xip_base;
} xspi_flash_t;

const xspi_flash_t xspi_flash1 = {
    .xspi = XSPI1,
    .xip_base = 0x90000000,
};

const xspi_flash_t xspi_flash2 = {
    .xspi = XSPI2,
    .xip_base = 0x70000000,
};

static bool xspi_dtr_enabled = false;

#ifdef pyb_pin_FLASH_RESET
// Can't rely on SysTick being available, so use a busy loop for delays.
// The timing here is approximate and assumes a CPU frequency of 800MHz.
static void xspi_delay_us(unsigned int us) {
    while (us--) {
        for (unsigned int i = 0; i < 800; ++i) {
            __NOP();
        }
    }
}
#endif

static inline void mp_hal_pin_config_alt_speed(mp_hal_pin_obj_t pin, uint32_t pull, uint32_t alt, uint32_t speed) {
    mp_hal_pin_config(pin, MP_HAL_PIN_MODE_ALT, pull, alt);
    mp_hal_pin_config_speed(pin, speed);
}

static int xspi_read_111_ext(uint8_t cmd, bool addr_enabled, uint32_t addr, size_t len, uint8_t *dest);
static int xspi_write_111_ext(uint8_t cmd, bool addr_enabled, uint32_t addr, size_t len, const uint8_t *src);
static int xspi_read_888_dtr_ext(uint16_t cmd, bool addr_enabled, uint32_t addr, uint32_t num_dummy, size_t len, uint8_t *dest);
static int xspi_write_888_dtr_ext(uint16_t cmd, bool addr_enabled, uint32_t addr, size_t len, const uint8_t *src);
static void xspi_memory_map_111(void);
static void xspi_memory_map_888(void);
static void xspi_memory_map_exit(void);

void xspi_init(void) {
    // Configure XSPI pins.
    mp_hal_pin_config_alt_speed(pyb_pin_XSPIM_P2_CS, MP_HAL_PIN_PULL_NONE, XSPI2_AF, MP_HAL_PIN_SPEED_VERY_HIGH);
    mp_hal_pin_config_alt_speed(pyb_pin_XSPIM_P2_SCK, MP_HAL_PIN_PULL_NONE, XSPI2_AF, MP_HAL_PIN_SPEED_VERY_HIGH);
    mp_hal_pin_config_alt_speed(pyb_pin_XSPIM_P2_DQS, MP_HAL_PIN_PULL_NONE, XSPI2_AF, MP_HAL_PIN_SPEED_VERY_HIGH);
    mp_hal_pin_config_alt_speed(pyb_pin_XSPIM_P2_IO0, MP_HAL_PIN_PULL_NONE, XSPI2_AF, MP_HAL_PIN_SPEED_VERY_HIGH);
    mp_hal_pin_config_alt_speed(pyb_pin_XSPIM_P2_IO1, MP_HAL_PIN_PULL_NONE, XSPI2_AF, MP_HAL_PIN_SPEED_VERY_HIGH);
    mp_hal_pin_config_alt_speed(pyb_pin_XSPIM_P2_IO2, MP_HAL_PIN_PULL_NONE, XSPI2_AF, MP_HAL_PIN_SPEED_VERY_HIGH);
    mp_hal_pin_config_alt_speed(pyb_pin_XSPIM_P2_IO3, MP_HAL_PIN_PULL_NONE, XSPI2_AF, MP_HAL_PIN_SPEED_VERY_HIGH);
    mp_hal_pin_config_alt_speed(pyb_pin_XSPIM_P2_IO4, MP_HAL_PIN_PULL_NONE, XSPI2_AF, MP_HAL_PIN_SPEED_VERY_HIGH);
    mp_hal_pin_config_alt_speed(pyb_pin_XSPIM_P2_IO5, MP_HAL_PIN_PULL_NONE, XSPI2_AF, MP_HAL_PIN_SPEED_VERY_HIGH);
    mp_hal_pin_config_alt_speed(pyb_pin_XSPIM_P2_IO6, MP_HAL_PIN_PULL_NONE, XSPI2_AF, MP_HAL_PIN_SPEED_VERY_HIGH);
    mp_hal_pin_config_alt_speed(pyb_pin_XSPIM_P2_IO7, MP_HAL_PIN_PULL_NONE, XSPI2_AF, MP_HAL_PIN_SPEED_VERY_HIGH);

    LL_AHB5_GRP1_ForceReset(LL_AHB5_GRP1_PERIPH_XSPIM);
    LL_AHB5_GRP1_ForceReset(LL_AHB5_GRP1_PERIPH_XSPI1);
    LL_AHB5_GRP1_ForceReset(LL_AHB5_GRP1_PERIPH_XSPI2);
    LL_AHB5_GRP1_ForceReset(LL_AHB5_GRP1_PERIPH_XSPI3);

    LL_AHB5_GRP1_ReleaseReset(LL_AHB5_GRP1_PERIPH_XSPIM);
    LL_AHB5_GRP1_ReleaseReset(LL_AHB5_GRP1_PERIPH_XSPI1);
    LL_AHB5_GRP1_ReleaseReset(LL_AHB5_GRP1_PERIPH_XSPI2);
    LL_AHB5_GRP1_ReleaseReset(LL_AHB5_GRP1_PERIPH_XSPI3);

    LL_RCC_SetXSPIClockSource(LL_RCC_XSPI1_CLKSOURCE_HCLK);
    LL_RCC_SetXSPIClockSource(LL_RCC_XSPI2_CLKSOURCE_HCLK);

    LL_AHB5_GRP1_EnableClock(LL_AHB5_GRP1_PERIPH_XSPIM);
    LL_AHB5_GRP1_EnableClock(LL_AHB5_GRP1_PERIPH_XSPI1);
    LL_AHB5_GRP1_EnableClock(LL_AHB5_GRP1_PERIPH_XSPI2);

    // Configure XSPIM in direct mode.
    XSPI1->CR &= ~XSPI_CR_EN;
    XSPI2->CR &= ~XSPI_CR_EN;
    XSPIM->CR = 0;

    // Configure the XSPIx peripheral.

    XSPIx->CR =
        3 << XSPI_CR_FTHRES_Pos // 4 byte must be available to read/write
            | 0 << XSPI_CR_MSEL_Pos // FLASH 0 selected
            | 0 << XSPI_CR_CSSEL_Pos // use NCS1 as chip select
            | 0 << XSPI_CR_DMM_Pos // dual-memory mode disabled
            | 1 << XSPI_CR_TCEN_Pos // time-out counter enabled
            | 0 << XSPI_CR_DMAEN_Pos // DMA disabled
            | 0 << XSPI_CR_ABORT_Pos // no abort request
            | 0 << XSPI_CR_EN_Pos // disabled
    ;

    XSPIx->DCR1 =
        1 << XSPI_DCR1_MTYP_Pos // Macronix mode
            | (MICROPY_HW_XSPIFLASH_SIZE_BITS_LOG2 - 3 - 1) << XSPI_DCR1_DEVSIZE_Pos
            | (MICROPY_HW_XSPI_CS_HIGH_CYCLES - 1) << XSPI_DCR1_CSHT_Pos
            | 0 << XSPI_DCR1_FRCK_Pos // CLK is not free running
            | 0 << XSPI_DCR1_CKMODE_Pos // CLK idles at low state
    ;

    XSPIx->DCR2 =
        0 << XSPI_DCR2_WRAPSIZE_Pos // separate wrap reads are not supported by the memory
            | (MICROPY_HW_XSPI_PRESCALER - 1) << XSPI_DCR2_PRESCALER_Pos
    ;

    XSPIx->DCR3 =
        0
        // 10 << XSPI_DCR3_CSBOUND_Pos // transaction boundary at 1024
    ;

    XSPIx->DCR4 =
        0 << XSPI_DCR4_REFRESH_Pos // refresh disabled (it's non-volatile memory)
    ;

    XSPIx->TCR = 0;

    // Enable the XSPI peripheral.
    XSPIx->CR |= XSPI_CR_EN;

    // XSPIM init
    XSPI1->CR &= ~(1 << XSPI_CR_EN_Pos);
    XSPI2->CR &= ~(1 << XSPI_CR_EN_Pos);
    XSPIM->CR = 0; // can also be (1 << 4) to pass through CS signal
    XSPIx->CR |= 1 << XSPI_CR_EN_Pos;

    #ifdef pyb_pin_FLASH_RESET
    // Reset SPI flash to make sure it's in a known state (SPI mode).
    mp_hal_pin_output(pyb_pin_FLASH_RESET);
    mp_hal_pin_low(pyb_pin_FLASH_RESET);
    xspi_delay_us(1000);
    mp_hal_pin_high(pyb_pin_FLASH_RESET);
    xspi_delay_us(10000);
    #endif

    // Enable memory-mapped mode.
    // Can select either SPI or DTR mode.
    if (1) {
        xspi_switch_to_dtr();
        xspi_memory_map_888();
    } else {
        xspi_memory_map_111();
    }
}

uint32_t xspi_get_xip_base(const xspi_flash_t *self) {
    return self->xip_base;
}

bool xspi_is_valid_addr(const xspi_flash_t *self, uint32_t addr) {
    return self->xip_base <= addr && addr < self->xip_base + 256 * 1024 * 1024;
}

static int xspi_read_111_ext(uint8_t cmd, bool addr_enabled, uint32_t addr, size_t len, uint8_t *dest) {
    uint32_t admode = addr_enabled ? 1 : 0;
    XSPIx->CR = (XSPIx->CR & ~XSPI_CR_FMODE_Msk) | 1 << XSPI_CR_FMODE_Pos; // indirect read mode
    XSPIx->CCR =
        1 << XSPI_CCR_DMODE_Pos // data on 1 line
            | 3 << XSPI_CCR_ADSIZE_Pos // 32-bit address size
            | admode << XSPI_CCR_ADMODE_Pos // address on 1 line, or disabled
            | 1 << XSPI_CCR_IMODE_Pos // instruction on 1 line
    ;
    XSPIx->TCR = 0 << XSPI_TCR_DCYC_Pos; // 0 dummy cycles
    XSPIx->DLR = len - 1; // number of bytes to read
    XSPIx->IR = cmd; // read opcode (triggers the start of the transaction if address disabled)
    if (addr_enabled) {
        XSPIx->AR = addr; // triggers the start of the transaction
    }

    #if 0 // untested code
    // Read in the data 4 bytes at a time if dest is aligned.
    if (((uintptr_t)dest & 3) == 0) {
        while (len >= 4) {
            while (!(XSPIx->SR & XSPI_SR_FTF)) {
                if (XSPIx->SR & XSPI_SR_TEF) {
                    return -MP_EIO;
                }
            }
            *(uint32_t *)dest = XSPIx->DR;
            dest += 4;
            len -= 4;
        }
    }
    #endif

    // Read in data 1 byte at a time.
    while (len--) {
        while (!((XSPIx->SR >> XSPI_SR_FLEVEL_Pos) & 0x3f)) {
            if (XSPIx->SR & XSPI_SR_TEF) {
                return -MP_EIO;
            }
        }
        *dest++ = *(volatile uint8_t *)&XSPIx->DR;
    }

    XSPIx->FCR = XSPI_FCR_CTCF; // clear TC flag

    return 0;
}

static int xspi_write_111_ext(uint8_t cmd, bool addr_enabled, uint32_t addr, size_t len, const uint8_t *src) {
    uint32_t dmode = len == 0 ? 0 : 1;
    uint32_t admode = addr_enabled ? 1 : 0;

    // Configure and start the transfer.
    // Transfer starts with IR write if no address or data, with AR write if no data,
    // otherwise with DR write.

    XSPIx->CR = (XSPIx->CR & ~XSPI_CR_FMODE_Msk) | 0 << XSPI_CR_FMODE_Pos; // indirect write mode
    XSPIx->CCR =
        dmode << XSPI_CCR_DMODE_Pos // data on 1 line, or disabled
            | 3 << XSPI_CCR_ADSIZE_Pos // 32-bit address size
            | admode << XSPI_CCR_ADMODE_Pos // address on 1 line, or disabled
            | 1 << XSPI_CCR_IMODE_Pos // instruction on 1 line
    ;
    XSPIx->TCR = 0 << XSPI_TCR_DCYC_Pos; // 0 dummy cycles
    if (len != 0) {
        XSPIx->DLR = len - 1;
    }
    XSPIx->IR = cmd; // write opcode
    if (addr_enabled) {
        XSPIx->AR = addr; // address
    }

    // Write out the data one byte at a time
    while (len--) {
        while (!(XSPIx->SR & XSPI_SR_FTF)) {
            if (XSPIx->SR & XSPI_SR_TEF) {
                return -MP_EIO;
            }
        }
        *(volatile uint8_t *)&XSPIx->DR = *src++;
    }

    // Wait for write to finish
    while (!(XSPIx->SR & XSPI_SR_TCF)) {
        if (XSPIx->SR & XSPI_SR_TEF) {
            return -MP_EIO;
        }
    }

    XSPIx->FCR = XSPI_FCR_CTCF; // clear TC flag

    // Wait for peripheral to return to idle.
    while (XSPIx->SR & XSPI_SR_BUSY) {
    }

    return 0;
}

static int xspi_read_888_dtr_ext(uint16_t cmd, bool addr_enabled, uint32_t addr, uint32_t num_dummy, size_t len, uint8_t *dest) {
    uint32_t admode = addr_enabled ? 4 : 0;

    // Configure and start the transfer.
    // Transfer starts with IR write if no address, otherwise with AR write.

    XSPIx->CR = (XSPIx->CR & ~XSPI_CR_FMODE_Msk) | 1 << XSPI_CR_FMODE_Pos; // indirect read mode
    XSPIx->CCR =
        1 << XSPI_CCR_DQSE_Pos // DQS enabled
            | 1 << XSPI_CCR_DDTR_Pos // data DTR enabled
            | 4 << XSPI_CCR_DMODE_Pos // data on 8 lines
            | 3 << XSPI_CCR_ADSIZE_Pos // 32-bit address size
            | 1 << XSPI_CCR_ADDTR_Pos // address DTR enabled
            | admode << XSPI_CCR_ADMODE_Pos // address on 8 lines, or disabled
            | 1 << XSPI_CCR_ISIZE_Pos // 16-bit instruction
            | 1 << XSPI_CCR_IDTR_Pos // instruction DTR enabled
            | 4 << XSPI_CCR_IMODE_Pos // instruction on 8 lines
    ;
    XSPIx->TCR = num_dummy << XSPI_TCR_DCYC_Pos; // N dummy cycles
    XSPIx->DLR = len - 1;
    XSPIx->IR = cmd; // read opcode
    if (addr_enabled) {
        XSPIx->AR = addr; // address
    }

    // Read in data 1 byte at a time.
    while (len--) {
        while (!((XSPIx->SR >> XSPI_SR_FLEVEL_Pos) & 0x3f)) {
            if (XSPIx->SR & XSPI_SR_TEF) {
                return -MP_EIO;
            }
        }
        *dest++ = *(volatile uint8_t *)&XSPIx->DR;
    }

    XSPIx->FCR = XSPI_FCR_CTCF; // clear TC flag

    return 0;
}

static int xspi_write_888_dtr_ext(uint16_t cmd, bool addr_enabled, uint32_t addr, size_t len, const uint8_t *src) {
    uint32_t dmode = len == 0 ? 0 : 4;
    uint32_t admode = addr_enabled ? 4 : 0;

    // Configure and start the transfer.
    // Transfer starts with IR write if no address or data, with AR write if no data,
    // otherwise with DR write.

    XSPIx->CR = (XSPIx->CR & ~XSPI_CR_FMODE_Msk) | 0 << XSPI_CR_FMODE_Pos; // indirect write mode
    XSPIx->CCR =
        1 << XSPI_CCR_DDTR_Pos // data DTR enabled
            | dmode << XSPI_CCR_DMODE_Pos // data on 8 lines, or disabled
            | 3 << XSPI_CCR_ADSIZE_Pos // 32-bit address size
            | 1 << XSPI_CCR_ADDTR_Pos // address DTR enabled
            | admode << XSPI_CCR_ADMODE_Pos // address on 8 lines, or disabled
            | 1 << XSPI_CCR_ISIZE_Pos // 16-bit instruction
            | 1 << XSPI_CCR_IDTR_Pos // instruction DTR enabled
            | 4 << XSPI_CCR_IMODE_Pos // instruction on 8 lines
    ;
    XSPIx->TCR = 0 << XSPI_TCR_DCYC_Pos; // 0 dummy cycles
    if (len != 0) {
        XSPIx->DLR = len - 1;
    }
    XSPIx->IR = cmd; // write opcode
    if (addr_enabled) {
        XSPIx->AR = addr; // address
    }

    // Write out the data one byte at a time
    while (len--) {
        while (!(XSPIx->SR & XSPI_SR_FTF)) {
            if (XSPIx->SR & XSPI_SR_TEF) {
                return -MP_EIO;
            }
        }
        *(volatile uint8_t *)&XSPIx->DR = *src++;
    }

    // Wait for write to finish
    while (!(XSPIx->SR & XSPI_SR_TCF)) {
        if (XSPIx->SR & XSPI_SR_TEF) {
            return -MP_EIO;
        }
    }

    XSPIx->FCR = XSPI_FCR_CTCF; // clear TC flag

    // Wait for peripheral to return to idle.
    while (XSPIx->SR & XSPI_SR_BUSY) {
    }

    return 0;
}

static void xspi_memory_map_111(void) {
    XSPIx->CCR =
        1 << XSPI_CCR_DMODE_Pos // data on 1 line
            | 3 << XSPI_CCR_ADSIZE_Pos // 32-bit address
            | 1 << XSPI_CCR_ADMODE_Pos // address on 1 line
            | 1 << XSPI_CCR_IMODE_Pos // instruction on 1 line
    ;

    XSPIx->TCR = 0 << XSPI_TCR_DCYC_Pos; // no dummy cycles
    XSPIx->IR = 0x13; // READ4B
    XSPIx->LPTR = 1024; // timeout period in number of CLK cycles

    // Enable the XSPI peripheral in memory-mapped mode.
    XSPIx->CR = (XSPIx->CR & ~XSPI_CR_FMODE_Msk) | 3 << XSPI_CR_FMODE_Pos;
}

static void xspi_memory_map_888(void) {
    XSPIx->CCR =
        1 << XSPI_CCR_DQSE_Pos // DQS enabled
            | 1 << XSPI_CCR_DDTR_Pos // data DTR enabled
            | 4 << XSPI_CCR_DMODE_Pos // data on 8 lines
            | 3 << XSPI_CCR_ADSIZE_Pos // 32-bit address
            | 1 << XSPI_CCR_ADDTR_Pos // address DTR enabled
            | 4 << XSPI_CCR_ADMODE_Pos // address on 8 lines
            | 1 << XSPI_CCR_ISIZE_Pos // 16-bit instruction
            | 1 << XSPI_CCR_IDTR_Pos // instruction DTR enabled
            | 4 << XSPI_CCR_IMODE_Pos // instruction on 8 lines
    ;

    XSPIx->TCR = 20 << XSPI_TCR_DCYC_Pos; // 20 dummy cycles for reading (minimum, flash may insert more by holding DQS low)
    XSPIx->IR = 0xee11; // octal DTR read mode (8DTRD)
    XSPIx->LPTR = 1024; // timeout period in number of CLK cycles

    // Enable the XSPI peripheral in memory-mapped mode.
    XSPIx->CR = (XSPIx->CR & ~XSPI_CR_FMODE_Msk) | 3 << XSPI_CR_FMODE_Pos;
}

static void xspi_memory_map_exit(void) {
    // Abort any ongoing transfer if peripheral is busy.
    if (XSPIx->SR & XSPI_SR_BUSY) {
        XSPIx->CR |= XSPI_CR_ABORT;
        while (!(XSPIx->SR & XSPI_SR_TCF)) {
        }
        XSPIx->FCR = XSPI_FCR_CTCF; // clear TC flag
        while (XSPIx->SR & XSPI_SR_BUSY) {
        }
    }
    XSPIx->CR = (XSPIx->CR & ~XSPI_CR_FMODE_Msk) | 0 << XSPI_CR_FMODE_Pos; // indirect write mode
}

void xspi_switch_to_dtr(void) {
    uint8_t buf[4];

    // WREN.
    xspi_write_111_ext(0x06, false, 0, 0, NULL);

    // Wait WEL=1, with small timeout.
    for (unsigned int i = 0; i < 100; ++i) {
        xspi_read_111_ext(0x05, false, 0, 1, buf);
        if (buf[0] & 2) {
            break;
        }
    }

    // Switch to DOPI DTR mode.
    buf[0] = 2;
    xspi_write_111_ext(0x72, true, 0x00000000, 1, buf);

    xspi_dtr_enabled = true;
}

void xspi_switch_to_spi(void) {
    uint8_t buf[4];

    // WREN.
    xspi_write_888_dtr_ext(0x06f9, false, 0, 0, NULL);

    // Wait WEL=1, with small timeout.
    for (unsigned int i = 0; i < 100; ++i) {
        xspi_read_111_ext(0x05, false, 0, 1, buf);
        if (buf[0] & 2) {
            break;
        }
    }

    // Switch to SPI mode.
    buf[0] = 0;
    buf[1] = 0;
    xspi_write_888_dtr_ext(0x728d, true, 0x00000000, 2, buf);

    xspi_dtr_enabled = false;
}

static int xspi_ioctl(void *self_in, uint32_t cmd, uintptr_t arg) {
    xspi_flash_t *self = self_in;
    switch (cmd) {
        case MP_QSPI_IOCTL_INIT:
            // XSPI must be manually initialise by calling `xspi_init()` at boot.
            // Here, just determine if it's in SPI or DTR mode.
            xspi_dtr_enabled = XSPIx->IR == 0xee11;
            break;
        case MP_QSPI_IOCTL_BUS_ACQUIRE:
            xspi_memory_map_exit();
            break;
        case MP_QSPI_IOCTL_BUS_RELEASE:
            if (xspi_dtr_enabled) {
                xspi_memory_map_888();
            } else {
                xspi_memory_map_111();
            }
            break;
        case MP_QSPI_IOCTL_MEMORY_MODIFIED: {
            uintptr_t *addr_len = (uintptr_t *)arg;
            volatile void *addr = (volatile void *)(self->xip_base + addr_len[0]);
            size_t len = addr_len[1];
            SCB_InvalidateICache_by_Addr(addr, len);
            SCB_InvalidateDCache_by_Addr(addr, len);
            break;
        }
    }
    return 0; // success
}

// These commands may be passed to this function.
#define CMD_WREN        (0x06)
#define CMD_RSTEN       (0x66)
#define CMD_RESET       (0x99)
#define CMD_SLEEP       (0xb9)
#define CMD_AWAKE       (0xab)
static int xspi_write_cmd_data(void *self_in, uint8_t cmd, size_t len, uint32_t data) {
    if (xspi_dtr_enabled) {
        uint16_t cmd16 = 0;
        if (cmd == CMD_WREN) {
            cmd16 = 0x06f9;
        } else if (cmd == CMD_SLEEP) {
            cmd16 = 0xb946;
        } else if (cmd == CMD_AWAKE) {
            cmd16 = 0xab54;
        }
        return xspi_write_888_dtr_ext(cmd16, false, 0, len, (const uint8_t *)&data);
    }
    return xspi_write_111_ext(cmd, false, 0, len, (const uint8_t *)&data);
}

// These commands may be passed to this function.
#define CMD_WRITE       (0x02)
#define CMD_WRITE_32    (0x12)
#define CMD_SEC_ERASE   (0x20)
#define CMD_SEC_ERASE_32 (0x21)
static int xspi_write_cmd_addr_data(void *self_in, uint8_t cmd, uint32_t addr, size_t len, const uint8_t *src) {
    // Convert 24-bit address commands to 32-bit address commands.
    if (cmd == CMD_WRITE) {
        cmd = CMD_WRITE_32;
    } else if (cmd == CMD_SEC_ERASE) {
        cmd = CMD_SEC_ERASE_32;
    }
    if (xspi_dtr_enabled) {
        uint16_t cmd16 = 0;
        if (cmd == CMD_WRITE_32) {
            cmd16 = 0x12ed;
        } else if (cmd == CMD_SEC_ERASE_32) {
            cmd16 = 0x21de;
        }
        return xspi_write_888_dtr_ext(cmd16, true, addr, len, src);
    }
    return xspi_write_111_ext(cmd, true, addr, len, src);
}

// These commands may be passed to this function.
#define CMD_RDSR        (0x05)
#define CMD_RD_DEVID    (0x9f)
static int xspi_read_cmd(void *self_in, uint8_t cmd, size_t len, uint32_t *dest) {
    (void)self_in;
    if (xspi_dtr_enabled) {
        uint16_t cmd16 = 0;
        uint32_t num_dummy = 0;
        if (cmd == CMD_RDSR) {
            cmd16 = 0x05fa;
            num_dummy = 4;
            len = 2;
        } else if (cmd == CMD_RD_DEVID) {
            // TODO this doesn't really work, because result is in STR format.
            cmd16 = 0x9f60;
            num_dummy = 4;
        }
        return xspi_read_888_dtr_ext(cmd16, true, 0, num_dummy, len, (uint8_t *)dest);
    }
    return xspi_read_111_ext(cmd, false, 0, len, (uint8_t *)dest);
}

static int xspi_direct_read(void *self_in, uint32_t addr, size_t len, uint8_t *dest) {
    xspi_flash_t *self = self_in;
    memcpy(dest, (const void *)(self->xip_base + addr), len);
    return 0;
}

const mp_qspi_proto_t xspi_proto = {
    .ioctl = xspi_ioctl,
    .write_cmd_data = xspi_write_cmd_data,
    .write_cmd_addr_data = xspi_write_cmd_addr_data,
    .read_cmd = xspi_read_cmd,
    .read_cmd_qaddr_qdata = NULL, // unused because .direct_read is set below, and caching is disabled
    .direct_read = xspi_direct_read,
};

#endif // defined(MICROPY_HW_XSPIFLASH_SIZE_BITS_LOG2)
