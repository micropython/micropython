/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Damien P. George
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

#include <string.h>

#include "py/mperrno.h"
#include "py/mphal.h"
#include "mpu.h"
#include "qspi.h"
#include "pin_static_af.h"

#if MICROPY_HW_ENABLE_QSPI || defined(MICROPY_HW_QSPIFLASH_SIZE_BITS_LOG2)

#define QSPI_MAP_ADDR (0x90000000)

#ifndef MICROPY_HW_QSPI_PRESCALER
#define MICROPY_HW_QSPI_PRESCALER       3  // F_CLK = F_AHB/3 (72MHz when CPU is 216MHz)
#endif

#ifndef MICROPY_HW_QSPI_SAMPLE_SHIFT
#define MICROPY_HW_QSPI_SAMPLE_SHIFT    1  // sample shift enabled
#endif

#ifndef MICROPY_HW_QSPI_TIMEOUT_COUNTER
#define MICROPY_HW_QSPI_TIMEOUT_COUNTER 0  // timeout counter disabled (see F7 errata)
#endif

#ifndef MICROPY_HW_QSPI_CS_HIGH_CYCLES
#define MICROPY_HW_QSPI_CS_HIGH_CYCLES  2  // nCS stays high for 2 cycles
#endif

#ifndef MICROPY_HW_QSPIFLASH_SIZE_BITS_LOG2
#endif

// Fast Read command in 32bit and 24bit addressing.
#define QSPI_FAST_READ_A4_CMD 0xec
#define QSPI_FAST_READ_A3_CMD 0xeb

// this formula computes the log2 of "m"
#define BITS_TO_LOG2(m) ((m) - 1) / (((m) - 1) % 255 + 1) / 255 % 255 * 8 + 7 - 86 / (((m) - 1) % 255 + 12)

#define MBytes (1024 * 1024)
static size_t qspi_memory_size_bytes = 0;

static inline void qspi_mpu_disable_all(void) {
    // Configure MPU to disable access to entire QSPI region, to prevent CPU
    // speculative execution from accessing this region and modifying QSPI registers.
    uint32_t irq_state = mpu_config_start();
    mpu_config_region(MPU_REGION_QSPI1, QSPI_MAP_ADDR, MPU_CONFIG_DISABLE(0x00, MPU_REGION_SIZE_256MB));
    mpu_config_end(irq_state);
}

#if 1

static inline void qspi_mpu_enable_mapped() {
    // Configure MPU to allow access to only the valid part of external SPI flash.
    // The memory accesses to the mapped QSPI are faster if the MPU is not used
    // unprivileged or the background region is disabled, the MPU issues a fault.
    uint32_t irq_state = mpu_config_start();

    if (qspi_memory_size_bytes > (128 * MBytes)) {
        mpu_config_region(MPU_REGION_QSPI1, QSPI_MAP_ADDR, MPU_CONFIG_DISABLE(0xFF, MPU_REGION_SIZE_256MB));
    } else if (qspi_memory_size_bytes > (64 * MBytes)) {
        mpu_config_region(MPU_REGION_QSPI1, QSPI_MAP_ADDR, MPU_CONFIG_DISABLE(0x0F, MPU_REGION_SIZE_256MB));
    } else if (qspi_memory_size_bytes > (32 * MBytes)) {
        mpu_config_region(MPU_REGION_QSPI1, QSPI_MAP_ADDR, MPU_CONFIG_DISABLE(0x03, MPU_REGION_SIZE_256MB));
    } else if (qspi_memory_size_bytes > (16 * MBytes)) {
        mpu_config_region(MPU_REGION_QSPI1, QSPI_MAP_ADDR, MPU_CONFIG_DISABLE(0x01, MPU_REGION_SIZE_256MB));
    } else if (qspi_memory_size_bytes > (8 * MBytes)) {
        mpu_config_region(MPU_REGION_QSPI1, QSPI_MAP_ADDR, MPU_CONFIG_DISABLE(0x01, MPU_REGION_SIZE_256MB));
        mpu_config_region(MPU_REGION_QSPI2, QSPI_MAP_ADDR, MPU_CONFIG_DISABLE(0x0F, MPU_REGION_SIZE_32MB));
    } else if (qspi_memory_size_bytes > (4 * MBytes)) {
        mpu_config_region(MPU_REGION_QSPI1, QSPI_MAP_ADDR, MPU_CONFIG_DISABLE(0x01, MPU_REGION_SIZE_256MB));
        mpu_config_region(MPU_REGION_QSPI2, QSPI_MAP_ADDR, MPU_CONFIG_DISABLE(0x03, MPU_REGION_SIZE_32MB));
    } else if (qspi_memory_size_bytes > (2 * MBytes)) {
        mpu_config_region(MPU_REGION_QSPI1, QSPI_MAP_ADDR, MPU_CONFIG_DISABLE(0x01, MPU_REGION_SIZE_256MB));
        mpu_config_region(MPU_REGION_QSPI2, QSPI_MAP_ADDR, MPU_CONFIG_DISABLE(0x01, MPU_REGION_SIZE_32MB));
    } else if (qspi_memory_size_bytes > (1 * MBytes)) {
        mpu_config_region(MPU_REGION_QSPI1, QSPI_MAP_ADDR, MPU_CONFIG_DISABLE(0x01, MPU_REGION_SIZE_256MB));
        mpu_config_region(MPU_REGION_QSPI2, QSPI_MAP_ADDR, MPU_CONFIG_DISABLE(0x0F, MPU_REGION_SIZE_32MB));
        mpu_config_region(MPU_REGION_QSPI3, QSPI_MAP_ADDR, MPU_CONFIG_DISABLE(0x01, MPU_REGION_SIZE_16MB));
    } else {
        mpu_config_region(MPU_REGION_QSPI1, QSPI_MAP_ADDR, MPU_CONFIG_DISABLE(0x01, MPU_REGION_SIZE_256MB));
        mpu_config_region(MPU_REGION_QSPI2, QSPI_MAP_ADDR, MPU_CONFIG_DISABLE(0x01, MPU_REGION_SIZE_32MB));
        mpu_config_region(MPU_REGION_QSPI3, QSPI_MAP_ADDR, MPU_CONFIG_DISABLE(0x03, MPU_REGION_SIZE_4MB));
    }
    mpu_config_end(irq_state);
}

#else

// This variant of the function is harder to read, but 76 bytes smaller.

static inline void qspi_mpu_enable_mapped() {
    // Configure MPU to allow access to only the valid part of external SPI flash.
    // The memory accesses to the mapped QSPI are faster if the MPU is not used
    // for the memory-mapped region, so 3 MPU regions are used to disable access
    // to everything except the valid address space, using holes in the bottom
    // of the regions and nesting them.
    // Note: Disabling a subregion (by setting its corresponding SRD bit to 1)
    // means another region overlapping the disabled range matches instead.  If no
    // other enabled region overlaps the disabled subregion, and the access is
    // unprivileged or the background region is disabled, the MPU issues a fault.
    uint32_t irq_state = mpu_config_start();

    static const uint8_t region_definitions[][7] = {
        // Each row per MB region total size, specifying region srd and size for MPU_REGION_QSPI1, 2 and 3.
        {128, 0xFF, MPU_REGION_SIZE_256MB, 0, 0, 0, 0},
        { 64, 0x0F, MPU_REGION_SIZE_256MB, 0, 0, 0, 0},
        { 32, 0x03, MPU_REGION_SIZE_256MB, 0, 0, 0, 0},
        { 16, 0x01, MPU_REGION_SIZE_256MB, 0, 0, 0, 0},
        {  8, 0x01, MPU_REGION_SIZE_256MB,
           0x0F, MPU_REGION_SIZE_32MB, 0, 0},
        {  4, 0x01, MPU_REGION_SIZE_256MB,
           0x03, MPU_REGION_SIZE_32MB, 0, 0},
        {  2, 0x01, MPU_REGION_SIZE_256MB,
           0x01, MPU_REGION_SIZE_32MB, 0, 0},
        {  1, 0x01, MPU_REGION_SIZE_256MB,
           0x0F, MPU_REGION_SIZE_32MB,
           0x01, MPU_REGION_SIZE_16MB},
        {  0, 0x01, MPU_REGION_SIZE_256MB,
           0x01, MPU_REGION_SIZE_32MB,
           0x03, MPU_REGION_SIZE_4MB},
    };
    size_t qspi_memory_size_mbytes = qspi_memory_size_bytes / 1024 / 1024;

    for (uint8_t i = 0; i < 9; ++i) {
        if (qspi_memory_size_mbytes > region_definitions[i][0]) {
            uint32_t attr_size_1 = MPU_CONFIG_DISABLE(region_definitions[i][1], region_definitions[i][2]);
            mpu_config_region(MPU_REGION_QSPI1, QSPI_MAP_ADDR, attr_size_1);
            if (region_definitions[i][3] > 0) {
                uint32_t attr_size_2 = MPU_CONFIG_DISABLE(region_definitions[i][3], region_definitions[i][4]);
                mpu_config_region(MPU_REGION_QSPI2, QSPI_MAP_ADDR, attr_size_2);
            }
            if (region_definitions[i][5] > 0) {
                uint32_t attr_size_3 = MPU_CONFIG_DISABLE(region_definitions[i][5], region_definitions[i][6]);
                mpu_config_region(MPU_REGION_QSPI3, QSPI_MAP_ADDR, attr_size_3);
            }
            break;
        }
    }
    mpu_config_end(irq_state);
}

#endif

void qspi_set_memory_size(size_t memory_size_bytes) {
    qspi_memory_size_bytes = memory_size_bytes;
    size_t QSPIFLASH_SIZE_BITS_LOG2 = BITS_TO_LOG2(qspi_memory_size_bytes * 8);
    QUADSPI->DCR =
        (QSPIFLASH_SIZE_BITS_LOG2 - 3 - 1) << QUADSPI_DCR_FSIZE_Pos
            | (MICROPY_HW_QSPI_CS_HIGH_CYCLES - 1) << QUADSPI_DCR_CSHT_Pos
            | 0 << QUADSPI_DCR_CKMODE_Pos // CLK idles at low state
    ;
}

void qspi_init(size_t memory_size_bytes) {
    qspi_memory_size_bytes = memory_size_bytes;

    qspi_mpu_disable_all();

    // Configure pins
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_QSPIFLASH_CS, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_QUADSPI_BK1_NCS);
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_QSPIFLASH_SCK, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_QUADSPI_CLK);
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_QSPIFLASH_IO0, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_QUADSPI_BK1_IO0);
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_QSPIFLASH_IO1, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_QUADSPI_BK1_IO1);
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_QSPIFLASH_IO2, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_QUADSPI_BK1_IO2);
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_QSPIFLASH_IO3, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_VERY_HIGH, STATIC_AF_QUADSPI_BK1_IO3);

    // Bring up the QSPI peripheral

    __HAL_RCC_QSPI_CLK_ENABLE();
    __HAL_RCC_QSPI_FORCE_RESET();
    __HAL_RCC_QSPI_RELEASE_RESET();

    QUADSPI->CR =
        (MICROPY_HW_QSPI_PRESCALER - 1) << QUADSPI_CR_PRESCALER_Pos
            | 3 << QUADSPI_CR_FTHRES_Pos // 4 bytes must be available to read/write
        #if defined(QUADSPI_CR_FSEL_Pos)
        | 0 << QUADSPI_CR_FSEL_Pos // FLASH 1 selected
        #endif
        #if defined(QUADSPI_CR_DFM_Pos)
        | 0 << QUADSPI_CR_DFM_Pos // dual-flash mode disabled
        #endif
        | MICROPY_HW_QSPI_SAMPLE_SHIFT << QUADSPI_CR_SSHIFT_Pos
            | MICROPY_HW_QSPI_TIMEOUT_COUNTER << QUADSPI_CR_TCEN_Pos
            | 1 << QUADSPI_CR_EN_Pos // enable the peripheral
    ;

    if (qspi_memory_size_bytes) {
        qspi_set_memory_size(qspi_memory_size_bytes);
    }
}

void qspi_memory_map() {
    // Enable memory-mapped mode
    uint8_t cmd = QSPI_FAST_READ_A3_CMD;
    uint8_t adsize = 2;
    if (qspi_memory_size_bytes > (16 * MBytes)) {
        // Flash chips over 16MB require 32bit addressing.
        cmd = QSPI_FAST_READ_A4_CMD;
        adsize = 3;
    }

    QUADSPI->ABR = 0; // disable continuous read mode

    QUADSPI->CCR =
        0 << QUADSPI_CCR_DDRM_Pos // DDR mode disabled
            | 0 << QUADSPI_CCR_SIOO_Pos // send instruction every transaction
            | 3 << QUADSPI_CCR_FMODE_Pos // memory-mapped mode
            | 3 << QUADSPI_CCR_DMODE_Pos // data on 4 lines
            | 4 << QUADSPI_CCR_DCYC_Pos // 4 dummy cycles
            | 0 << QUADSPI_CCR_ABSIZE_Pos // 8-bit alternate byte
            | 3 << QUADSPI_CCR_ABMODE_Pos // alternate byte on 4 lines
            | adsize << QUADSPI_CCR_ADSIZE_Pos
            | 3 << QUADSPI_CCR_ADMODE_Pos // address on 4 lines
            | 1 << QUADSPI_CCR_IMODE_Pos // instruction on 1 line
            | cmd << QUADSPI_CCR_INSTRUCTION_Pos
    ;

    qspi_mpu_enable_mapped();
}

STATIC int qspi_ioctl(void *self_in, uint32_t cmd, uint32_t arg) {
    (void)self_in;
    switch (cmd) {
        case MP_QSPI_IOCTL_INIT:
            qspi_init(0);
            break;
        case MP_QSPI_IOCTL_BUS_ACQUIRE:
            // Disable memory-mapped region during bus access
            qspi_mpu_disable_all();
            // Abort any ongoing transfer if peripheral is busy
            if (QUADSPI->SR & QUADSPI_SR_BUSY) {
                QUADSPI->CR |= QUADSPI_CR_ABORT;
                while (QUADSPI->CR & QUADSPI_CR_ABORT) {
                }
            }
            break;
        case MP_QSPI_IOCTL_BUS_RELEASE:
            // Switch to memory-map mode when bus is idle
            qspi_memory_map();
            break;
        case MP_QSPI_IOCTL_FLASH_SIZE:
            if (arg > 0) {
                qspi_set_memory_size(arg);
            }
            return qspi_memory_size_bytes;
    }
    return 0; // success
}

STATIC int qspi_write_cmd_data(void *self_in, uint8_t cmd, size_t len, uint32_t data) {
    (void)self_in;

    QUADSPI->FCR = QUADSPI_FCR_CTCF; // clear TC flag

    if (len == 0) {
        QUADSPI->CCR =
            0 << QUADSPI_CCR_DDRM_Pos // DDR mode disabled
                | 0 << QUADSPI_CCR_SIOO_Pos // send instruction every transaction
                | 0 << QUADSPI_CCR_FMODE_Pos // indirect write mode
                | 0 << QUADSPI_CCR_DMODE_Pos // no data
                | 0 << QUADSPI_CCR_DCYC_Pos // 0 dummy cycles
                | 0 << QUADSPI_CCR_ABMODE_Pos // no alternate byte
                | 0 << QUADSPI_CCR_ADMODE_Pos // no address
                | 1 << QUADSPI_CCR_IMODE_Pos // instruction on 1 line
                | cmd << QUADSPI_CCR_INSTRUCTION_Pos // write opcode
        ;
    } else {
        QUADSPI->DLR = len - 1;

        QUADSPI->CCR =
            0 << QUADSPI_CCR_DDRM_Pos // DDR mode disabled
                | 0 << QUADSPI_CCR_SIOO_Pos // send instruction every transaction
                | 0 << QUADSPI_CCR_FMODE_Pos // indirect write mode
                | 1 << QUADSPI_CCR_DMODE_Pos // data on 1 line
                | 0 << QUADSPI_CCR_DCYC_Pos // 0 dummy cycles
                | 0 << QUADSPI_CCR_ABMODE_Pos // no alternate byte
                | 0 << QUADSPI_CCR_ADMODE_Pos // no address
                | 1 << QUADSPI_CCR_IMODE_Pos // instruction on 1 line
                | cmd << QUADSPI_CCR_INSTRUCTION_Pos // write opcode
        ;

        // Wait for at least 1 free byte location in the FIFO.
        while (!(QUADSPI->SR & QUADSPI_SR_FTF)) {
        }

        if (len == 1) {
            *(uint8_t *)&QUADSPI->DR = data;
        } else {
            // This assumes len==2
            *(uint16_t *)&QUADSPI->DR = data;
        }
    }

    // Wait for write to finish
    while (!(QUADSPI->SR & QUADSPI_SR_TCF)) {
        if (QUADSPI->SR & QUADSPI_SR_TEF) {
            return -MP_EIO;
        }
    }

    QUADSPI->FCR = QUADSPI_FCR_CTCF; // clear TC flag

    return 0;
}

STATIC int qspi_write_cmd_addr_data(void *self_in, uint8_t cmd, uint32_t addr, size_t len, const uint8_t *src) {
    (void)self_in;

    uint8_t adsize = MICROPY_HW_SPI_ADDR_IS_32BIT(addr) ? 3 : 2;

    QUADSPI->FCR = QUADSPI_FCR_CTCF; // clear TC flag

    if (len == 0) {
        QUADSPI->CCR =
            0 << QUADSPI_CCR_DDRM_Pos // DDR mode disabled
                | 0 << QUADSPI_CCR_SIOO_Pos // send instruction every transaction
                | 0 << QUADSPI_CCR_FMODE_Pos // indirect write mode
                | 0 << QUADSPI_CCR_DMODE_Pos // no data
                | 0 << QUADSPI_CCR_DCYC_Pos // 0 dummy cycles
                | 0 << QUADSPI_CCR_ABMODE_Pos // no alternate byte
                | adsize << QUADSPI_CCR_ADSIZE_Pos // 32/24-bit address size
                | 1 << QUADSPI_CCR_ADMODE_Pos // address on 1 line
                | 1 << QUADSPI_CCR_IMODE_Pos // instruction on 1 line
                | cmd << QUADSPI_CCR_INSTRUCTION_Pos // write opcode
        ;

        QUADSPI->AR = addr;
    } else {
        QUADSPI->DLR = len - 1;

        QUADSPI->CCR =
            0 << QUADSPI_CCR_DDRM_Pos // DDR mode disabled
                | 0 << QUADSPI_CCR_SIOO_Pos // send instruction every transaction
                | 0 << QUADSPI_CCR_FMODE_Pos // indirect write mode
                | 1 << QUADSPI_CCR_DMODE_Pos // data on 1 line
                | 0 << QUADSPI_CCR_DCYC_Pos // 0 dummy cycles
                | 0 << QUADSPI_CCR_ABMODE_Pos // no alternate byte
                | adsize << QUADSPI_CCR_ADSIZE_Pos // 32/24-bit address size
                | 1 << QUADSPI_CCR_ADMODE_Pos // address on 1 line
                | 1 << QUADSPI_CCR_IMODE_Pos // instruction on 1 line
                | cmd << QUADSPI_CCR_INSTRUCTION_Pos // write opcode
        ;

        QUADSPI->AR = addr;

        // Write out the data 1 byte at a time
        while (len) {
            while (!(QUADSPI->SR & QUADSPI_SR_FTF)) {
                if (QUADSPI->SR & QUADSPI_SR_TEF) {
                    return -MP_EIO;
                }
            }
            *(volatile uint8_t *)&QUADSPI->DR = *src++;
            --len;
        }
    }

    // Wait for write to finish
    while (!(QUADSPI->SR & QUADSPI_SR_TCF)) {
        if (QUADSPI->SR & QUADSPI_SR_TEF) {
            return -MP_EIO;
        }
    }

    QUADSPI->FCR = QUADSPI_FCR_CTCF; // clear TC flag

    return 0;
}

STATIC int qspi_read_cmd(void *self_in, uint8_t cmd, size_t len, uint32_t *dest) {
    (void)self_in;

    QUADSPI->FCR = QUADSPI_FCR_CTCF; // clear TC flag

    QUADSPI->DLR = len - 1; // number of bytes to read

    QUADSPI->CCR =
        0 << QUADSPI_CCR_DDRM_Pos // DDR mode disabled
            | 0 << QUADSPI_CCR_SIOO_Pos // send instruction every transaction
            | 1 << QUADSPI_CCR_FMODE_Pos // indirect read mode
            | 1 << QUADSPI_CCR_DMODE_Pos // data on 1 line
            | 0 << QUADSPI_CCR_DCYC_Pos // 0 dummy cycles
            | 0 << QUADSPI_CCR_ABMODE_Pos // no alternate byte
            | 0 << QUADSPI_CCR_ADMODE_Pos // no address
            | 1 << QUADSPI_CCR_IMODE_Pos // instruction on 1 line
            | cmd << QUADSPI_CCR_INSTRUCTION_Pos // read opcode
    ;

    // Wait for read to finish
    while (!(QUADSPI->SR & QUADSPI_SR_TCF)) {
        if (QUADSPI->SR & QUADSPI_SR_TEF) {
            return -MP_EIO;
        }
    }

    QUADSPI->FCR = QUADSPI_FCR_CTCF; // clear TC flag

    // Read result
    *dest = QUADSPI->DR;

    return 0;
}

STATIC int qspi_read_cmd_addr_data(void *self_in, uint8_t cmd, uint32_t addr, size_t len, uint8_t *dest, uint8_t mode) {
    (void)self_in;
    uint8_t adsize = MICROPY_HW_SPI_ADDR_IS_32BIT(addr) ? 3 : 2;

    uint32_t dmode = 0;
    uint32_t admode = 0;
    uint32_t dcyc = 0;
    uint32_t abmode = 0;

    if (mode == MP_QSPI_TRANSFER_CMD_QADDR_QDATA) {
        dmode = 3; // 4 data lines used
        admode = 3; // 4 address lines used
        dcyc = 4; // 4 dummy cycles (2  bytes)
        abmode = 3; // alternate-byte bytes sent on 4 lines
    } else if (mode == MP_QSPI_TRANSFER_CMD_ADDR_DATA) {
        dmode = 1; // 1 data lines used
        admode = 1; // 1 address lines used
        dcyc = 8; // 8 dummy cycles (1 byte)
        abmode = 0; // No alternate-byte bytes sent
    } else {
        return -1;
    }

    QUADSPI->FCR = QUADSPI_FCR_CTCF; // clear TC flag

    QUADSPI->DLR = len - 1; // number of bytes to read

    QUADSPI->CCR =
        0 << QUADSPI_CCR_DDRM_Pos // DDR mode disabled
            | 0 << QUADSPI_CCR_SIOO_Pos // send instruction every transaction
            | 1 << QUADSPI_CCR_FMODE_Pos // indirect read mode
            | dmode << QUADSPI_CCR_DMODE_Pos // data lines
            | dcyc << QUADSPI_CCR_DCYC_Pos // dummy cycles
            | 0 << QUADSPI_CCR_ABSIZE_Pos // 8-bit alternate byte
            | abmode << QUADSPI_CCR_ABMODE_Pos // alternate byte count / lines
            | adsize << QUADSPI_CCR_ADSIZE_Pos // 32 or 24-bit address size
            | admode << QUADSPI_CCR_ADMODE_Pos // address lines
            | 1 << QUADSPI_CCR_IMODE_Pos // instruction on 1 line
            | cmd << QUADSPI_CCR_INSTRUCTION_Pos // quad read opcode
    ;

    QUADSPI->ABR = 0; // alternate byte: disable continuous read mode
    QUADSPI->AR = addr; // address to read from

    #if defined(STM32H7)
    // Workaround for SR getting set immediately after setting the address.
    if (QUADSPI->SR & 0x01) {
        QUADSPI->FCR |= QUADSPI_FCR_CTEF;
        QUADSPI->AR = addr; // address to read from
    }
    #endif

    // Read in the data 4 bytes at a time if dest is aligned
    if (((uintptr_t)dest & 3) == 0) {
        while (len >= 4) {
            while (!(QUADSPI->SR & QUADSPI_SR_FTF)) {
                if (QUADSPI->SR & QUADSPI_SR_TEF) {
                    return -MP_EIO;
                }
            }
            *(uint32_t *)dest = QUADSPI->DR;
            dest += 4;
            len -= 4;
        }
    }

    // Read in remaining data 1 byte at a time
    while (len) {
        while (!((QUADSPI->SR >> QUADSPI_SR_FLEVEL_Pos) & 0x3f)) {
            if (QUADSPI->SR & QUADSPI_SR_TEF) {
                return -MP_EIO;
            }
        }
        *dest++ = *(volatile uint8_t *)&QUADSPI->DR;
        --len;
    }

    QUADSPI->FCR = QUADSPI_FCR_CTCF; // clear TC flag

    return 0;
}

const mp_qspi_proto_t qspi_proto = {
    .ioctl = qspi_ioctl,
    .write_cmd_data = qspi_write_cmd_data,
    .write_cmd_addr_data = qspi_write_cmd_addr_data,
    .read_cmd = qspi_read_cmd,
    .read_cmd_addr_data = qspi_read_cmd_addr_data,
};

#endif // defined(MICROPY_HW_QSPIFLASH_SIZE_BITS_LOG2)
