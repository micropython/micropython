/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Damien P. George
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

// This OCTOSPI driver is currently configured to run in 1-line (SPI) mode.
// It uses the mp_qspi_proto_t QSPI protocol and translates quad-commands
// into 1-line commands.

#include "py/mperrno.h"
#include "py/mphal.h"
#include "octospi.h"
#include "pin_static_af.h"

#if defined(MICROPY_HW_OSPIFLASH_SIZE_BITS_LOG2)

#ifndef MICROPY_HW_OSPI_PRESCALER
#define MICROPY_HW_OSPI_PRESCALER (3) // F_CLK = F_AHB/3
#endif

#ifndef MICROPY_HW_OSPI_CS_HIGH_CYCLES
#define MICROPY_HW_OSPI_CS_HIGH_CYCLES (2) // nCS stays high for 2 cycles
#endif

void octospi_init(void) {
    // Configure OCTOSPI pins (allows 1, 2, 4 or 8 line configuration).
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_OSPIFLASH_CS, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_OCTOSPI1_NCS);
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_OSPIFLASH_SCK, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_OCTOSPI1_CLK);
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_OSPIFLASH_IO0, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_OCTOSPI1_IO0);
    #if defined(MICROPY_HW_OSPIFLASH_IO1)
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_OSPIFLASH_IO1, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_OCTOSPI1_IO1);
    #if defined(MICROPY_HW_OSPIFLASH_IO2)
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_OSPIFLASH_IO2, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_OCTOSPI1_IO2);
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_OSPIFLASH_IO3, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_OCTOSPI1_IO3);
    #if defined(MICROPY_HW_OSPIFLASH_IO4)
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_OSPIFLASH_IO4, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_OCTOSPI1_IO4);
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_OSPIFLASH_IO5, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_OCTOSPI1_IO5);
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_OSPIFLASH_IO6, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_OCTOSPI1_IO6);
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_OSPIFLASH_IO7, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_OCTOSPI1_IO7);
    #endif
    #endif
    #endif

    // Reset and turn on the OCTOSPI peripheral.
    __HAL_RCC_OSPI1_CLK_ENABLE();
    __HAL_RCC_OSPI1_FORCE_RESET();
    __HAL_RCC_OSPI1_RELEASE_RESET();

    // Configure the OCTOSPI peripheral.

    OCTOSPI1->CR =
        3 << OCTOSPI_CR_FTHRES_Pos // 4 bytes must be available to read/write
            | 0 << OCTOSPI_CR_MSEL_Pos // FLASH 0 selected
            | 0 << OCTOSPI_CR_DMM_Pos // dual-memory mode disabled
    ;

    OCTOSPI1->DCR1 =
        (MICROPY_HW_OSPIFLASH_SIZE_BITS_LOG2 - 3 - 1) << OCTOSPI_DCR1_DEVSIZE_Pos
            | (MICROPY_HW_OSPI_CS_HIGH_CYCLES - 1) << OCTOSPI_DCR1_CSHT_Pos
            | 0 << OCTOSPI_DCR1_CKMODE_Pos // CLK idles at low state
    ;

    OCTOSPI1->DCR2 =
        (MICROPY_HW_OSPI_PRESCALER - 1) << OCTOSPI_DCR2_PRESCALER_Pos
    ;

    OCTOSPI1->DCR3 = 0;
    OCTOSPI1->DCR4 = 0;

    // Enable the OCTOSPI peripheral.
    OCTOSPI1->CR |= OCTOSPI_CR_EN;
}

static int octospi_ioctl(void *self_in, uint32_t cmd) {
    (void)self_in;
    switch (cmd) {
        case MP_QSPI_IOCTL_INIT:
            octospi_init();
            break;
        case MP_QSPI_IOCTL_BUS_ACQUIRE:
            // Abort any ongoing transfer if peripheral is busy.
            if (OCTOSPI1->SR & OCTOSPI_SR_BUSY) {
                OCTOSPI1->CR |= OCTOSPI_CR_ABORT;
                while (OCTOSPI1->CR & OCTOSPI_CR_ABORT) {
                }
            }
            break;
        case MP_QSPI_IOCTL_BUS_RELEASE:
            break;
    }
    return 0; // success
}

static int octospi_write_cmd_data(void *self_in, uint8_t cmd, size_t len, uint32_t data) {
    (void)self_in;

    OCTOSPI1->FCR = OCTOSPI_FCR_CTCF; // clear TC flag

    OCTOSPI1->CR = (OCTOSPI1->CR & ~OCTOSPI_CR_FMODE_Msk) | 0 << OCTOSPI_CR_FMODE_Pos; // indirect write mode

    if (len == 0) {
        OCTOSPI1->CCR =
            0 << OCTOSPI_CCR_DDTR_Pos // DD mode disabled
                | 0 << OCTOSPI_CCR_SIOO_Pos // send instruction every transaction
                | 0 << OCTOSPI_CCR_DMODE_Pos // no data
                | 0 << OCTOSPI_CCR_ABMODE_Pos // no alternate byte
                | 0 << OCTOSPI_CCR_ADMODE_Pos // no address
                | 1 << OCTOSPI_CCR_IMODE_Pos // instruction on 1 line
        ;
        OCTOSPI1->TCR = 0 << OCTOSPI_TCR_DCYC_Pos; // 0 dummy cycles

        // This triggers the start of the operation.
        OCTOSPI1->IR = cmd << OCTOSPI_IR_INSTRUCTION_Pos; // write opcode
    } else {
        OCTOSPI1->DLR = len - 1;
        OCTOSPI1->CCR =
            0 << OCTOSPI_CCR_DDTR_Pos // DD mode disabled
                | 0 << OCTOSPI_CCR_SIOO_Pos // send instruction every transaction
                | 1 << OCTOSPI_CCR_DMODE_Pos // data on 1 line
                | 0 << OCTOSPI_CCR_ABMODE_Pos // no alternate byte
                | 0 << OCTOSPI_CCR_ADMODE_Pos // no address
                | 1 << OCTOSPI_CCR_IMODE_Pos // instruction on 1 line
        ;
        OCTOSPI1->TCR = 0 << OCTOSPI_TCR_DCYC_Pos; // 0 dummy cycles
        OCTOSPI1->IR = cmd << OCTOSPI_IR_INSTRUCTION_Pos; // write opcode

        // Wait for at least 1 free byte location in the FIFO.
        while (!(OCTOSPI1->SR & OCTOSPI_SR_FTF)) {
        }

        // This triggers the start of the operation.
        // This assumes len==2
        *(uint16_t *)&OCTOSPI1->DR = data;
    }

    // Wait for write to finish
    while (!(OCTOSPI1->SR & OCTOSPI_SR_TCF)) {
        if (OCTOSPI1->SR & OCTOSPI_SR_TEF) {
            return -MP_EIO;
        }
    }

    OCTOSPI1->FCR = OCTOSPI_FCR_CTCF; // clear TC flag

    return 0;
}

static int octospi_write_cmd_addr_data(void *self_in, uint8_t cmd, uint32_t addr, size_t len, const uint8_t *src) {
    (void)self_in;

    uint8_t adsize = MICROPY_HW_SPI_ADDR_IS_32BIT(addr) ? 3 : 2;

    OCTOSPI1->FCR = OCTOSPI_FCR_CTCF; // clear TC flag

    OCTOSPI1->CR = (OCTOSPI1->CR & ~OCTOSPI_CR_FMODE_Msk) | 0 << OCTOSPI_CR_FMODE_Pos; // indirect write mode

    if (len == 0) {
        OCTOSPI1->CCR =
            0 << OCTOSPI_CCR_DDTR_Pos // DD mode disabled
                | 0 << OCTOSPI_CCR_SIOO_Pos // send instruction every transaction
                | 0 << OCTOSPI_CCR_DMODE_Pos // no data
                | 0 << OCTOSPI_CCR_ABMODE_Pos // no alternate byte
                | adsize << OCTOSPI_CCR_ADSIZE_Pos // 32/24-bit address size
                | 1 << OCTOSPI_CCR_ADMODE_Pos // address on 1 line
                | 1 << OCTOSPI_CCR_IMODE_Pos // instruction on 1 line
        ;
        OCTOSPI1->TCR = 0 << OCTOSPI_TCR_DCYC_Pos; // 0 dummy cycles
        OCTOSPI1->IR = cmd << OCTOSPI_IR_INSTRUCTION_Pos; // write opcode

        // This triggers the start of the operation.
        OCTOSPI1->AR = addr;
    } else {
        OCTOSPI1->DLR = len - 1;

        OCTOSPI1->CCR =
            0 << OCTOSPI_CCR_DDTR_Pos // DD mode disabled
                | 0 << OCTOSPI_CCR_SIOO_Pos // send instruction every transaction
                | 1 << OCTOSPI_CCR_DMODE_Pos // data on 1 line
                | 0 << OCTOSPI_CCR_ABMODE_Pos // no alternate byte
                | adsize << OCTOSPI_CCR_ADSIZE_Pos // 32/24-bit address size
                | 1 << OCTOSPI_CCR_ADMODE_Pos // address on 1 line
                | 1 << OCTOSPI_CCR_IMODE_Pos // instruction on 1 line
        ;
        OCTOSPI1->TCR = 0 << OCTOSPI_TCR_DCYC_Pos; // 0 dummy cycles
        OCTOSPI1->IR = cmd << OCTOSPI_IR_INSTRUCTION_Pos; // write opcode
        OCTOSPI1->AR = addr;

        // Write out the data 1 byte at a time
        // This triggers the start of the operation.
        while (len) {
            while (!(OCTOSPI1->SR & OCTOSPI_SR_FTF)) {
                if (OCTOSPI1->SR & OCTOSPI_SR_TEF) {
                    return -MP_EIO;
                }
            }
            *(volatile uint8_t *)&OCTOSPI1->DR = *src++;
            --len;
        }
    }

    // Wait for write to finish
    while (!(OCTOSPI1->SR & OCTOSPI_SR_TCF)) {
        if (OCTOSPI1->SR & OCTOSPI_SR_TEF) {
            return -MP_EIO;
        }
    }

    OCTOSPI1->FCR = OCTOSPI_FCR_CTCF; // clear TC flag

    return 0;
}

static int octospi_read_cmd(void *self_in, uint8_t cmd, size_t len, uint32_t *dest) {
    (void)self_in;

    OCTOSPI1->FCR = OCTOSPI_FCR_CTCF; // clear TC flag

    OCTOSPI1->DLR = len - 1; // number of bytes to read

    OCTOSPI1->CR = (OCTOSPI1->CR & ~OCTOSPI_CR_FMODE_Msk) | 1 << OCTOSPI_CR_FMODE_Pos; // indirect read mode

    OCTOSPI1->CCR =
        0 << OCTOSPI_CCR_DDTR_Pos // DD mode disabled
            | 0 << OCTOSPI_CCR_SIOO_Pos // send instruction every transaction
            | 1 << OCTOSPI_CCR_DMODE_Pos // data on 1 line
            | 0 << OCTOSPI_CCR_ABMODE_Pos // no alternate byte
            | 0 << OCTOSPI_CCR_ADMODE_Pos // no address
            | 1 << OCTOSPI_CCR_IMODE_Pos // instruction on 1 line
    ;

    OCTOSPI1->TCR = 0 << OCTOSPI_TCR_DCYC_Pos; // 0 dummy cycles

    // This triggers the start of the operation.
    OCTOSPI1->IR = cmd << OCTOSPI_IR_INSTRUCTION_Pos; // read opcode

    // Wait for read to finish
    while (!(OCTOSPI1->SR & OCTOSPI_SR_TCF)) {
        if (OCTOSPI1->SR & OCTOSPI_SR_TEF) {
            return -MP_EIO;
        }
    }

    OCTOSPI1->FCR = OCTOSPI_FCR_CTCF; // clear TC flag

    // Read result
    *dest = OCTOSPI1->DR;

    return 0;
}

static int octospi_read_cmd_qaddr_qdata(void *self_in, uint8_t cmd, uint32_t addr, size_t len, uint8_t *dest) {
    (void)self_in;

    #if defined(MICROPY_HW_OSPIFLASH_IO1) && !defined(MICROPY_HW_OSPIFLASH_IO2) && !defined(MICROPY_HW_OSPIFLASH_IO4)

    // Use 2-line address, 2-line data.

    uint32_t adsize = MICROPY_HW_SPI_ADDR_IS_32BIT(addr) ? 3 : 2;
    uint32_t dmode = 2; // data on 2-lines
    uint32_t admode = 2; // address on 2-lines
    uint32_t dcyc = 4; // 4 dummy cycles

    if (cmd == 0xeb || cmd == 0xec) {
        // Convert to 2-line command.
        cmd = MICROPY_HW_SPI_ADDR_IS_32BIT(addr) ? 0xbc : 0xbb;
    }

    #else

    // Fallback to use 1-line address, 1-line data.

    uint32_t adsize = MICROPY_HW_SPI_ADDR_IS_32BIT(addr) ? 3 : 2;
    uint32_t dmode = 1; // data on 1-line
    uint32_t admode = 1; // address on 1-line
    uint32_t dcyc = 0; // 0 dummy cycles

    if (cmd == 0xeb || cmd == 0xec) {
        // Convert to 1-line command.
        cmd = MICROPY_HW_SPI_ADDR_IS_32BIT(addr) ? 0x13 : 0x03;
    }

    #endif

    OCTOSPI1->FCR = OCTOSPI_FCR_CTCF; // clear TC flag

    OCTOSPI1->DLR = len - 1; // number of bytes to read
    OCTOSPI1->CR = (OCTOSPI1->CR & ~OCTOSPI_CR_FMODE_Msk) | 1 << OCTOSPI_CR_FMODE_Pos; // indirect read mode
    OCTOSPI1->CCR =
        0 << OCTOSPI_CCR_DDTR_Pos // DD mode disabled
            | 0 << OCTOSPI_CCR_SIOO_Pos // send instruction every transaction
            | dmode << OCTOSPI_CCR_DMODE_Pos // data on n lines
            | 0 << OCTOSPI_CCR_ABSIZE_Pos // 8-bit alternate byte
            | 0 << OCTOSPI_CCR_ABMODE_Pos // no alternate byte
            | adsize << OCTOSPI_CCR_ADSIZE_Pos // 32 or 24-bit address size
            | admode << OCTOSPI_CCR_ADMODE_Pos // address on n lines
            | 1 << OCTOSPI_CCR_IMODE_Pos // instruction on 1 line
    ;
    OCTOSPI1->TCR = dcyc << OCTOSPI_TCR_DCYC_Pos; // n dummy cycles
    OCTOSPI1->IR = cmd << OCTOSPI_IR_INSTRUCTION_Pos; // quad read opcode

    // This triggers the start of the operation.
    OCTOSPI1->AR = addr; // address to read from

    // Read in the data 4 bytes at a time if dest is aligned
    if (((uintptr_t)dest & 3) == 0) {
        while (len >= 4) {
            while (!(OCTOSPI1->SR & OCTOSPI_SR_FTF)) {
                if (OCTOSPI1->SR & OCTOSPI_SR_TEF) {
                    return -MP_EIO;
                }
            }
            *(uint32_t *)dest = OCTOSPI1->DR;
            dest += 4;
            len -= 4;
        }
    }

    // Read in remaining data 1 byte at a time
    while (len) {
        while (!((OCTOSPI1->SR >> OCTOSPI_SR_FLEVEL_Pos) & 0x3f)) {
            if (OCTOSPI1->SR & OCTOSPI_SR_TEF) {
                return -MP_EIO;
            }
        }
        *dest++ = *(volatile uint8_t *)&OCTOSPI1->DR;
        --len;
    }

    OCTOSPI1->FCR = OCTOSPI_FCR_CTCF; // clear TC flag

    return 0;
}

const mp_qspi_proto_t octospi_proto = {
    .ioctl = octospi_ioctl,
    .write_cmd_data = octospi_write_cmd_data,
    .write_cmd_addr_data = octospi_write_cmd_addr_data,
    .read_cmd = octospi_read_cmd,
    .read_cmd_qaddr_qdata = octospi_read_cmd_qaddr_qdata,
};

#endif // defined(MICROPY_HW_OSPIFLASH_SIZE_BITS_LOG2)
