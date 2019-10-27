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

#if defined(MICROPY_HW_QSPIFLASH_SIZE_BITS_LOG2)

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

static inline void qspi_mpu_disable_all(void) {
    // Configure MPU to disable access to entire QSPI region, to prevent CPU
    // speculative execution from accessing this region and modifying QSPI registers.
    uint32_t irq_state = mpu_config_start();
    mpu_config_region(MPU_REGION_QSPI1, QSPI_MAP_ADDR, MPU_CONFIG_DISABLE(0x00, MPU_REGION_SIZE_256MB));
    mpu_config_end(irq_state);
}

static inline void qspi_mpu_enable_mapped(void) {
    // Configure MPU to allow access to only the valid part of external SPI flash.
    // The memory accesses to the mapped QSPI are faster if the MPU is not used
    // for the memory-mapped region, so 3 MPU regions are used to disable access
    // to everything except the valid address space, using holes in the bottom
    // of the regions and nesting them.
    // At the moment this is hard-coded to 2MiB of QSPI address space.
    uint32_t irq_state = mpu_config_start();
    mpu_config_region(MPU_REGION_QSPI1, QSPI_MAP_ADDR, MPU_CONFIG_DISABLE(0x01, MPU_REGION_SIZE_256MB));
    mpu_config_region(MPU_REGION_QSPI2, QSPI_MAP_ADDR, MPU_CONFIG_DISABLE(0x0f, MPU_REGION_SIZE_32MB));
    mpu_config_region(MPU_REGION_QSPI3, QSPI_MAP_ADDR, MPU_CONFIG_DISABLE(0x01, MPU_REGION_SIZE_16MB));
    mpu_config_end(irq_state);
}

void qspi_init(void) {
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

    QUADSPI->DCR =
        (MICROPY_HW_QSPIFLASH_SIZE_BITS_LOG2 - 3 - 1) << QUADSPI_DCR_FSIZE_Pos
        | (MICROPY_HW_QSPI_CS_HIGH_CYCLES - 1) << QUADSPI_DCR_CSHT_Pos
        | 0 << QUADSPI_DCR_CKMODE_Pos // CLK idles at low state
        ;
}

void qspi_memory_map(void) {
    // Enable memory-mapped mode

    QUADSPI->ABR = 0; // disable continuous read mode
    QUADSPI->CCR =
        0 << QUADSPI_CCR_DDRM_Pos // DDR mode disabled
        | 0 << QUADSPI_CCR_SIOO_Pos // send instruction every transaction
        | 3 << QUADSPI_CCR_FMODE_Pos // memory-mapped mode
        | 3 << QUADSPI_CCR_DMODE_Pos // data on 4 lines
        | 4 << QUADSPI_CCR_DCYC_Pos // 4 dummy cycles
        | 0 << QUADSPI_CCR_ABSIZE_Pos // 8-bit alternate byte
        | 3 << QUADSPI_CCR_ABMODE_Pos // alternate byte on 4 lines
        | 2 << QUADSPI_CCR_ADSIZE_Pos // 24-bit address size
        | 3 << QUADSPI_CCR_ADMODE_Pos // address on 4 lines
        | 1 << QUADSPI_CCR_IMODE_Pos // instruction on 1 line
        | 0xeb << QUADSPI_CCR_INSTRUCTION_Pos // quad read opcode
        ;

    qspi_mpu_enable_mapped();
}

STATIC int qspi_ioctl(void *self_in, uint32_t cmd) {
    (void)self_in;
    switch (cmd) {
        case MP_QSPI_IOCTL_INIT:
            qspi_init();
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
    }
    return 0; // success
}

STATIC void qspi_write_cmd_data(void *self_in, uint8_t cmd, size_t len, uint32_t data) {
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

        // This assumes len==2
        *(uint16_t*)&QUADSPI->DR = data;
    }

    // Wait for write to finish
    while (!(QUADSPI->SR & QUADSPI_SR_TCF)) {
    }

    QUADSPI->FCR = QUADSPI_FCR_CTCF; // clear TC flag
}

STATIC void qspi_write_cmd_addr_data(void *self_in, uint8_t cmd, uint32_t addr, size_t len, const uint8_t *src) {
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
            | 2 << QUADSPI_CCR_ADSIZE_Pos // 24-bit address size
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
            | 2 << QUADSPI_CCR_ADSIZE_Pos // 24-bit address size
            | 1 << QUADSPI_CCR_ADMODE_Pos // address on 1 line
            | 1 << QUADSPI_CCR_IMODE_Pos // instruction on 1 line
            | cmd << QUADSPI_CCR_INSTRUCTION_Pos // write opcode
            ;

        QUADSPI->AR = addr;

        // Write out the data 1 byte at a time
        while (len) {
            while (!(QUADSPI->SR & QUADSPI_SR_FTF)) {
            }
            *(volatile uint8_t*)&QUADSPI->DR = *src++;
            --len;
        }
    }

    // Wait for write to finish
    while (!(QUADSPI->SR & QUADSPI_SR_TCF)) {
    }

    QUADSPI->FCR = QUADSPI_FCR_CTCF; // clear TC flag
}

STATIC uint32_t qspi_read_cmd(void *self_in, uint8_t cmd, size_t len) {
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
    }

    QUADSPI->FCR = QUADSPI_FCR_CTCF; // clear TC flag

    // Read result
    return QUADSPI->DR;
}

STATIC void qspi_read_cmd_qaddr_qdata(void *self_in, uint8_t cmd, uint32_t addr, size_t len, uint8_t *dest) {
    (void)self_in;
    QUADSPI->FCR = QUADSPI_FCR_CTCF; // clear TC flag

    QUADSPI->DLR = len - 1; // number of bytes to read

    QUADSPI->CCR =
        0 << QUADSPI_CCR_DDRM_Pos // DDR mode disabled
        | 0 << QUADSPI_CCR_SIOO_Pos // send instruction every transaction
        | 1 << QUADSPI_CCR_FMODE_Pos // indirect read mode
        | 3 << QUADSPI_CCR_DMODE_Pos // data on 4 lines
        | 4 << QUADSPI_CCR_DCYC_Pos // 4 dummy cycles
        | 0 << QUADSPI_CCR_ABSIZE_Pos // 8-bit alternate byte
        | 3 << QUADSPI_CCR_ABMODE_Pos // alternate byte on 4 lines
        | 2 << QUADSPI_CCR_ADSIZE_Pos // 24-bit address size
        | 3 << QUADSPI_CCR_ADMODE_Pos // address on 4 lines
        | 1 << QUADSPI_CCR_IMODE_Pos // instruction on 1 line
        | cmd << QUADSPI_CCR_INSTRUCTION_Pos // quad read opcode
        ;

    QUADSPI->ABR = 0; // alternate byte: disable continuous read mode
    QUADSPI->AR = addr; // addres to read from

    // Read in the data 4 bytes at a time if dest is aligned
    if (((uintptr_t)dest & 3) == 0) {
        while (len >= 4) {
            while (!(QUADSPI->SR & QUADSPI_SR_FTF)) {
            }
            *(uint32_t*)dest = QUADSPI->DR;
            dest += 4;
            len -= 4;
        }
    }

    // Read in remaining data 1 byte at a time
    while (len) {
        while (!((QUADSPI->SR >> QUADSPI_SR_FLEVEL_Pos) & 0x3f)) {
        }
        *dest++ = *(volatile uint8_t*)&QUADSPI->DR;
        --len;
    }

    QUADSPI->FCR = QUADSPI_FCR_CTCF; // clear TC flag
}

const mp_qspi_proto_t qspi_proto = {
    .ioctl = qspi_ioctl,
    .write_cmd_data = qspi_write_cmd_data,
    .write_cmd_addr_data = qspi_write_cmd_addr_data,
    .read_cmd = qspi_read_cmd,
    .read_cmd_qaddr_qdata = qspi_read_cmd_qaddr_qdata,
};

#endif // defined(MICROPY_HW_QSPIFLASH_SIZE_BITS_LOG2)
