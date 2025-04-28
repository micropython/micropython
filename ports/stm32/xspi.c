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

static inline void mp_hal_pin_config_alt_speed(mp_hal_pin_obj_t pin, uint32_t pull, uint32_t alt, uint32_t speed) {
    // TODO use mp_hal_pin_config_alt_static_speed
    mp_hal_pin_config(pin, MP_HAL_PIN_MODE_ALT, pull, alt);
    mp_hal_pin_config_speed(pin, speed);
}

static int xspi_read_111(uint8_t cmd, size_t len, uint8_t *dest);
static void xspi_memory_map_111(void);
#if 0
static void xspi_memory_map_888(void);
#endif

void xspi_init(void) {
    #if !MICROPY_HW_RUNS_FROM_EXT_FLASH
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

    LL_RCC_SetXSPIClockSource(LL_RCC_XSPI2_CLKSOURCE_HCLK);

    __HAL_RCC_XSPIM_CLK_ENABLE();
    __HAL_RCC_XSPI1_CLK_ENABLE();
    __HAL_RCC_XSPI2_CLK_ENABLE();

    // XSPI power enable.
    __HAL_RCC_PWR_CLK_ENABLE();
    HAL_PWREx_EnableVddIO3();                     
    HAL_PWREx_ConfigVddIORange(PWR_VDDIO3, PWR_VDDIO_RANGE_1V8);

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
        //10 << XSPI_DCR3_CSBOUND_Pos // transaction boundary at 1024
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

    //////////////////////////

    #if 0
    for (int i = 0; i < 4; ++i) {
        printf("xspim %x %x\n", i, (int)((volatile uint32_t *)XSPIM)[i]);
    }
    for (int i = 0; i < 0x22c/4; ++i) {
        if (((volatile uint32_t *)XSPI1)[i]) {
            printf("xspi1 %x %x\n", i, (int)((volatile uint32_t *)XSPI1)[i]);
        }
    }
    for (int i = 0; i < 0x22c/4; ++i) {
        if (((volatile uint32_t *)XSPIx)[i]) {
            printf("xspi2 %x %x\n", i, (int)((volatile uint32_t *)XSPIx)[i]);
        }
    }
    #endif
                
    //////////////////////////

    uint8_t buf[4];
    xspi_read_111(0x9f, 3, buf);
    //mp_printf(&mp_plat_print, "xspi flash init id=%x:%x:%x\n", buf[0], buf[1], buf[2]);

    if (1) {
        xspi_memory_map_111();
    }
    #endif
}

uint32_t xspi_get_xip_base(const xspi_flash_t *self) {
    return self->xip_base;
}

bool xspi_is_valid_addr(const xspi_flash_t *self, uint32_t addr) {
    return self->xip_base <= addr && addr < self->xip_base + 256 * 1024 * 1024;
}

static int xspi_read_111(uint8_t cmd, size_t len, uint8_t *dest) {
    //printf("read_111 cmd=%02x %d\n", cmd, len);
    //mp_hal_delay_ms(10);

    //XSPIx->FCR = XSPI_FCR_CTCF; // clear TC flag

    XSPIx->CR = (XSPIx->CR & ~XSPI_CR_FMODE_Msk) | 1 << XSPI_CR_FMODE_Pos; // indirect read mode
    XSPIx->CCR =
        0 << XSPI_CCR_DDTR_Pos // data DTR disabled
            | 1 << XSPI_CCR_DMODE_Pos // data on 1 line
            | 0 << XSPI_CCR_ABMODE_Pos // no alternate byte
            | 0 << XSPI_CCR_ADMODE_Pos // no address
            | 1 << XSPI_CCR_IMODE_Pos // instruction on 1 line
    ;
    XSPIx->TCR = 0 << XSPI_TCR_DCYC_Pos; // 0 dummy cycles
    XSPIx->DLR = len - 1; // number of bytes to read
    XSPIx->IR = cmd; // read opcode (triggers the start of the transaction)

    // Read in data 1 byte at a time.
    while (len) {
        while (!((XSPIx->SR >> XSPI_SR_FLEVEL_Pos) & 0x3f)) {
            if (XSPIx->SR & XSPI_SR_TEF) {
                return -MP_EIO;
            }
        }
        *dest++ = *(volatile uint8_t *)&XSPIx->DR;
        //printf("read %02x\n", dest[-1]);
        //mp_hal_delay_ms(100);
        --len;
    }

    XSPIx->FCR = XSPI_FCR_CTCF; // clear TC flag

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

    // TODO: enable MPU?
}

#if 0
static void xspi_memory_map_888(void) {
    XSPIx->CCR =
        1 << XSPI_CCR_DQSE_Pos // DQS enabled
            | 1 << XSPI_CCR_DDTR_Pos // data DTR enabled
            | 4 << XSPI_CCR_DMODE_Pos // data on 8 lines
            | 0 << XSPI_CCR_ABSIZE_Pos // 8-bit alternate bytes
            | 0 << XSPI_CCR_ABDTR_Pos // alternate bytes DTR disabled
            | 0 << XSPI_CCR_ABMODE_Pos // no alternate bytes
            | 3 << XSPI_CCR_ADSIZE_Pos // 32-bit address
            | 1 << XSPI_CCR_ADDTR_Pos // address DTR enabled
            | 4 << XSPI_CCR_ADMODE_Pos // address on 8 lines
            | 1 << XSPI_CCR_ISIZE_Pos // 16-bit instruction
            | 1 << XSPI_CCR_IDTR_Pos // instruction DTR enabled
            | 4 << XSPI_CCR_IMODE_Pos // instruction on 8 lines
    ;

    XSPIx->TCR =
        0 << XSPI_TCR_SSHIFT_Pos // no sample shift
            //| 1 << XSPI_TCR_DHQC_Pos // 1/4 cycle hold enabled
            | 6 << XSPI_TCR_DCYC_Pos // 6 dummy cycles for reading (minimum, flash may insert more by holding DQS low)
    ;

    XSPIx->IR = 0xee11; // OCTA DTR read mode (8DTRD)

    XSPIx->LPTR = 1024; // timeout period in number of CLK cycles

    // Enable the XSPI peripheral in memory-mapped mode.
    XSPIx->CR = (XSPIx->CR & ~XSPI_CR_FMODE_Msk) | 3 << XSPI_CR_FMODE_Pos;  // memory-mapped mode

    // TODO: enable MPU?
}
#endif

#if 1
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
#endif

static int xspi_ioctl(void *self_in, uint32_t cmd, uintptr_t arg) {
    xspi_flash_t *self = self_in;
    switch (cmd) {
        case MP_QSPI_IOCTL_INIT:
            xspi_init();
            break;
        case MP_QSPI_IOCTL_BUS_ACQUIRE:
            xspi_memory_map_exit();
            break;
        case MP_QSPI_IOCTL_BUS_RELEASE:
            xspi_memory_map_111();
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

static int xspi_write_helper(void *self_in, uint8_t cmd, bool addr_enable, uint32_t addr, size_t len, const uint8_t *src) {
    (void)self_in;

    //printf("write cmd=%02x addr=%x len=%d\n", cmd, (int)addr, len);
    //mp_hal_delay_ms(10);

    uint8_t adsize = 0;
    uint8_t admode = 0;
    if (addr_enable) {
        adsize = MICROPY_HW_SPI_ADDR_IS_32BIT(addr) ? 3 : 2;
        admode = 1;
    }

    XSPIx->CR = (XSPIx->CR & ~XSPI_CR_FMODE_Msk) | 0 << XSPI_CR_FMODE_Pos; // indirect write mode

    if (len == 0) {
        XSPIx->CCR =
            adsize << XSPI_CCR_ADSIZE_Pos // 32/24-bit address size
                | admode << XSPI_CCR_ADMODE_Pos // address on 1 line, or disabled
                | 1 << XSPI_CCR_IMODE_Pos // instruction on 1 line
        ;
        XSPIx->TCR = 0 << XSPI_TCR_DCYC_Pos; // 0 dummy cycles
        XSPIx->IR = cmd; // write opcode (triggers the start of the transaction if address disabled)
        if (addr_enable) {
            // This triggers the start of the operation.
            XSPIx->AR = addr;
        }
    } else {
        XSPIx->CCR =
            1 << XSPI_CCR_DMODE_Pos // data on 1 line
                | adsize << XSPI_CCR_ADSIZE_Pos // 32/24-bit address size
                | admode << XSPI_CCR_ADMODE_Pos // address on 1 line
                | 1 << XSPI_CCR_IMODE_Pos // instruction on 1 line
        ;
        XSPIx->TCR = 0 << XSPI_TCR_DCYC_Pos; // 0 dummy cycles
        XSPIx->DLR = len - 1;
        XSPIx->IR = cmd; // write opcode
        XSPIx->AR = addr; // address

        // Write out the data 1 byte at a time
        // This triggers the start of the operation.
        while (len) {
            while (!(XSPIx->SR & XSPI_SR_FTF)) {
                if (XSPIx->SR & XSPI_SR_TEF) {
                    return -MP_EIO;
                }
            }
            *(volatile uint8_t *)&XSPIx->DR = *src++;
            --len;
        }
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

static int xspi_write_cmd_data(void *self_in, uint8_t cmd, size_t len, uint32_t data) {
    return xspi_write_helper(self_in, cmd, false, 0, len, (const uint8_t *)&data);
}

static int xspi_write_cmd_addr_data(void *self_in, uint8_t cmd, uint32_t addr, size_t len, const uint8_t *src) {
    return xspi_write_helper(self_in, cmd, true, addr, len, src);
}

static int xspi_read_cmd(void *self_in, uint8_t cmd, size_t len, uint32_t *dest) {
    (void)self_in;
    return xspi_read_111(cmd, len, (uint8_t *)dest);

    /*
    XSPIx->DLR = len - 1; // number of bytes to read
    XSPIx->CR = (XSPIx->CR & ~XSPI_CR_FMODE_Msk) | 1 << XSPI_CR_FMODE_Pos; // indirect read mode
    XSPIx->CCR =
        1 << XSPI_CCR_DMODE_Pos // data on 1 line
            | 1 << XSPI_CCR_IMODE_Pos // instruction on 1 line
    ;
    XSPIx->TCR = 0 << XSPI_TCR_DCYC_Pos; // 0 dummy cycles
    XSPIx->IR = cmd; // read opcode (triggers the start of the transaction)

    // Wait for read to finish.
    while (!(XSPIx->SR & XSPI_SR_TCF)) {
        if (XSPIx->SR & XSPI_SR_TEF) {
            return -MP_EIO;
        }
    }

    XSPIx->FCR = XSPI_FCR_CTCF; // clear TC flag

    // Read result
    *dest = XSPIx->DR;
    */

    return 0;
}

static int xspi_read_cmd_qaddr_qdata(void *self_in, uint8_t cmd, uint32_t addr, uint8_t num_dummy, size_t len, uint8_t *dest) {
    (void)self_in;

    //printf("read q cmd=%02x addr=%x len=%d\n", cmd, (int)addr, len);
    //mp_hal_delay_ms(10);

    // Use 1-line address, 1-line data.

    uint32_t adsize = MICROPY_HW_SPI_ADDR_IS_32BIT(addr) ? 3 : 2;
    uint32_t dmode = 1; // data on 1-line
    uint32_t admode = 1; // address on 1-line
    uint32_t dcyc = 0; // 0 dummy cycles

    if (cmd == 0xeb || cmd == 0xec) {
        // Convert to 1-line command.
        cmd = MICROPY_HW_SPI_ADDR_IS_32BIT(addr) ? 0x13 : 0x03;
    }

    XSPIx->CR = (XSPIx->CR & ~XSPI_CR_FMODE_Msk) | 1 << XSPI_CR_FMODE_Pos; // indirect read mode
    XSPIx->CCR =
        dmode << XSPI_CCR_DMODE_Pos // data on n lines
            | adsize << XSPI_CCR_ADSIZE_Pos // 32 or 24-bit address size
            | admode << XSPI_CCR_ADMODE_Pos // address on n lines
            | 1 << XSPI_CCR_IMODE_Pos // instruction on 1 line
    ;
    XSPIx->TCR = dcyc << XSPI_TCR_DCYC_Pos; // n dummy cycles
    XSPIx->DLR = len - 1; // number of bytes to read
    XSPIx->IR = cmd; // read opcode
    XSPIx->AR = addr; // address to read from (triggers the start of the transaction)

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

    // Read in remaining data 1 byte at a time
    while (len) {
        while (!((XSPIx->SR >> XSPI_SR_FLEVEL_Pos) & 0x3f)) {
            if (XSPIx->SR & XSPI_SR_TEF) {
                return -MP_EIO;
            }
        }
        *dest++ = *(volatile uint8_t *)&XSPIx->DR;
        --len;
    }

    XSPIx->FCR = XSPI_FCR_CTCF; // clear TC flag

    // Wait for peripheral to return to idle.
    while (XSPIx->SR & XSPI_SR_BUSY) {
    }

    return 0;
}

static int xspi_read(void *self_in, uint32_t addr, size_t len, uint8_t *dest) {
    xspi_flash_t *self = self_in;
    memcpy(dest, (const void *)(self->xip_base + addr), len);
    return 0;
}

const mp_qspi_proto_t xspi_proto = {
    .ioctl = xspi_ioctl,
    .write_cmd_data = xspi_write_cmd_data,
    .write_cmd_addr_data = xspi_write_cmd_addr_data,
    .read_cmd = xspi_read_cmd,
    .read_cmd_qaddr_qdata = xspi_read_cmd_qaddr_qdata,
    .read = xspi_read,
};

#if 0

#define CMD_WRITE       (0x02)
#define CMD_RDSR        (0x05)
#define CMD_WREN        (0x06)
#define CMD_WRITE_32    (0x12)
#define CMD_SEC_ERASE   (0x20)
#define CMD_SEC_ERASE_32 (0x21)

#define WAIT_SR_TIMEOUT (1000000)

#define PAGE_SIZE (256) // maximum bytes we can write in one SPI transfer
#define SECTOR_SIZE (4096)

static void xspi_flash_acquire_bus(xspi_flash_t *self) {
    xspi_memory_map_exit();
}

static void xspi_flash_release_bus(xspi_flash_t *self) {
    xspi_memory_map_111();
}

static void xspi_flash_notify_modified(xspi_flash_t *self, uint32_t addr, uint32_t len) {
    SCB_InvalidateICache_by_Addr((void *)(self->xip_base + addr), len);
    SCB_InvalidateDCache_by_Addr((void *)(self->xip_base + addr), len);
}

static int xspi_flash_write_cmd(xspi_flash_t *self, uint8_t cmd) {
    return xspi_write_helper(self, cmd, false, 0, 0, NULL);
}

static int xspi_flash_wait_sr(xspi_flash_t *self, uint8_t mask, uint8_t val, uint32_t timeout) {
    do {
        uint32_t sr;
        int ret = xspi_read_cmd(self, CMD_RDSR, 1, &sr);
        if (ret != 0) {
            return ret;
        }
        if ((sr & mask) == val) {
            return 0; // success
        }
    } while (timeout--);

    return -MP_ETIMEDOUT;
}

static int xspi_flash_wait_wel1(xspi_flash_t *self) {
    return xspi_flash_wait_sr(self, 2, 2, WAIT_SR_TIMEOUT);
}

static int xspi_flash_wait_wip0(xspi_flash_t *self) {
    return xspi_flash_wait_sr(self, 1, 0, WAIT_SR_TIMEOUT);
}

static inline void xspi_flash_deepsleep_internal(xspi_flash_t *self, int value) {
    xspi_flash_write_cmd(self, value ? 0xb9 : 0xab); // sleep/wake
}

int xspi_flash_init(xspi_flash_t *self) {
    xspi_init();

    xspi_flash_acquire_bus(self);

    // Ensure SPI flash is out of sleep mode
    xspi_flash_deepsleep_internal(self, 0);

    xspi_flash_release_bus(self);

    return 0;
}

void xspi_flash_deepsleep(xspi_flash_t *self, int value) {
    if (value) {
        xspi_flash_acquire_bus(self);
    }
    xspi_flash_deepsleep_internal(self, value);
    if (!value) {
        xspi_flash_release_bus(self);
    }
}

static int xspi_flash_erase_block_internal(xspi_flash_t *self, uint32_t addr) {
    int ret = 0;
    // enable writes
    ret = xspi_flash_write_cmd(self, CMD_WREN);
    if (ret != 0) {
        return ret;
    }

    // wait WEL=1
    ret = xspi_flash_wait_wel1(self);
    if (ret != 0) {
        return ret;
    }

    // erase the sector
    uint8_t cmd = MICROPY_HW_SPI_ADDR_IS_32BIT(addr) ? CMD_SEC_ERASE_32 : CMD_SEC_ERASE;
    ret = xspi_write_helper(self, cmd, true, addr, 0, NULL);
    if (ret != 0) {
        return ret;
    }

    // wait WIP=0
    return xspi_flash_wait_wip0(self);
}

static int xspi_flash_write_page(xspi_flash_t *self, uint32_t addr, size_t len, const uint8_t *src) {
    int ret = 0;
    // enable writes
    ret = xspi_flash_write_cmd(self, CMD_WREN);
    if (ret != 0) {
        return ret;
    }

    // wait WEL=1
    ret = xspi_flash_wait_wel1(self);
    if (ret != 0) {
        return ret;
    }

    // write the page
    uint8_t cmd = MICROPY_HW_SPI_ADDR_IS_32BIT(addr) ? CMD_WRITE_32 : CMD_WRITE;
    ret = xspi_write_helper(self, cmd, true, addr, len, src);
    if (ret != 0) {
        return ret;
    }

    // wait WIP=0
    return xspi_flash_wait_wip0(self);
}

/******************************************************************************/
// Interface functions that go direct to the SPI flash device

int xspi_flash_erase_block(xspi_flash_t *self, uint32_t addr) {
    xspi_flash_acquire_bus(self);
    int ret = xspi_flash_erase_block_internal(self, addr);
    xspi_flash_notify_modified(self, addr, SECTOR_SIZE);
    xspi_flash_release_bus(self);
    return ret;
}

int xspi_flash_read(xspi_flash_t *self, uint32_t addr, size_t len, uint8_t *dest) {
    memcpy(dest, (const void *)(self->xip_base + addr), len);
    return 0;
}

int xspi_flash_write(xspi_flash_t *self, uint32_t addr, size_t len, const uint8_t *src) {
    uint32_t orig_addr = addr;
    uint32_t orig_len = len;
    xspi_flash_acquire_bus(self);
    int ret = 0;
    uint32_t offset = addr & (PAGE_SIZE - 1);
    while (len) {
        size_t rest = PAGE_SIZE - offset;
        if (rest > len) {
            rest = len;
        }
        ret = xspi_flash_write_page(self, addr, rest, src);
        if (ret != 0) {
            break;
        }
        len -= rest;
        addr += rest;
        src += rest;
        offset = 0;
    }
    xspi_flash_notify_modified(self, orig_addr, orig_len);
    xspi_flash_release_bus(self);
    return ret;
}

#endif

#endif // defined(MICROPY_HW_XSPIFLASH_SIZE_BITS_LOG2)
