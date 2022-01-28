/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018-2019 Damien P. George
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

#include <stdio.h>

#include "py/mperrno.h"
#include "py/mphal.h"
#include "dma.h"
#include "pin.h"
#include "pin_static_af.h"
#include "pendsv.h"
#include "sdio.h"

#if MICROPY_PY_NETWORK_CYW43

#define DEFAULT_MASK (0)

enum {
    SDMMC_IRQ_STATE_DONE,
    SDMMC_IRQ_STATE_CMD_DONE,
    SDMMC_IRQ_STATE_CMD_DATA_PENDING,
};

static volatile int sdmmc_irq_state;
static volatile uint32_t sdmmc_block_size_log2;
static volatile bool sdmmc_write;
static volatile bool sdmmc_dma;
static volatile uint32_t sdmmc_error;
static volatile uint8_t *sdmmc_buf_cur;
static volatile uint8_t *sdmmc_buf_top;

// The H7/F7/L4 have 2 SDMMC peripherals, but at the moment this driver only supports
// using one of them in a given build, selected by MICROPY_HW_SDIO_SDMMC.

#if MICROPY_HW_SDIO_SDMMC == 1
#define SDMMC                   SDMMC1
#define SDMMC_IRQn              SDMMC1_IRQn
#define SDMMC_IRQHandler        SDMMC1_IRQHandler
#define SDMMC_CLK_ENABLE()      __HAL_RCC_SDMMC1_CLK_ENABLE()
#define SDMMC_CLK_DISABLE()     __HAL_RCC_SDMMC1_CLK_DISABLE()
#define SDMMC_IS_CLK_DISABLED() __HAL_RCC_SDMMC1_IS_CLK_DISABLED()
#define STATIC_AF_SDMMC_CK      STATIC_AF_SDMMC1_CK
#define STATIC_AF_SDMMC_CMD     STATIC_AF_SDMMC1_CMD
#define STATIC_AF_SDMMC_D0      STATIC_AF_SDMMC1_D0
#define STATIC_AF_SDMMC_D1      STATIC_AF_SDMMC1_D1
#define STATIC_AF_SDMMC_D2      STATIC_AF_SDMMC1_D2
#define STATIC_AF_SDMMC_D3      STATIC_AF_SDMMC1_D3
#else
#if defined(STM32F7)
#error Due to DMA configuration, only SDMMC1 is currently supported on F7
#endif
#define SDMMC                   SDMMC2
#define SDMMC_IRQn              SDMMC2_IRQn
#define SDMMC_IRQHandler        SDMMC2_IRQHandler
#define SDMMC_CLK_ENABLE()      __HAL_RCC_SDMMC2_CLK_ENABLE()
#define SDMMC_CLK_DISABLE()     __HAL_RCC_SDMMC2_CLK_DISABLE()
#define SDMMC_IS_CLK_DISABLED() __HAL_RCC_SDMMC2_IS_CLK_DISABLED()
#define STATIC_AF_SDMMC_CK      STATIC_AF_SDMMC2_CK
#define STATIC_AF_SDMMC_CMD     STATIC_AF_SDMMC2_CMD
#define STATIC_AF_SDMMC_D0      STATIC_AF_SDMMC2_D0
#define STATIC_AF_SDMMC_D1      STATIC_AF_SDMMC2_D1
#define STATIC_AF_SDMMC_D2      STATIC_AF_SDMMC2_D2
#define STATIC_AF_SDMMC_D3      STATIC_AF_SDMMC2_D3
#endif

// If no custom SDIO pins defined, use the default ones
#ifndef MICROPY_HW_SDIO_CK
#define MICROPY_HW_SDIO_D0      (pin_C8)
#define MICROPY_HW_SDIO_D1      (pin_C9)
#define MICROPY_HW_SDIO_D2      (pin_C10)
#define MICROPY_HW_SDIO_D3      (pin_C11)
#define MICROPY_HW_SDIO_CK      (pin_C12)
#define MICROPY_HW_SDIO_CMD     (pin_D2)
#endif

void sdio_init(uint32_t irq_pri) {
    // configure IO pins
    mp_hal_pin_config_alt_static(MICROPY_HW_SDIO_D0, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_UP, STATIC_AF_SDMMC_D0);
    mp_hal_pin_config_alt_static(MICROPY_HW_SDIO_D1, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_UP, STATIC_AF_SDMMC_D1);
    mp_hal_pin_config_alt_static(MICROPY_HW_SDIO_D2, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_UP, STATIC_AF_SDMMC_D2);
    mp_hal_pin_config_alt_static(MICROPY_HW_SDIO_D3, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_UP, STATIC_AF_SDMMC_D3);
    mp_hal_pin_config_alt_static(MICROPY_HW_SDIO_CK, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, STATIC_AF_SDMMC_CK);
    mp_hal_pin_config_alt_static(MICROPY_HW_SDIO_CMD, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_UP, STATIC_AF_SDMMC_CMD);

    SDMMC_CLK_ENABLE(); // enable SDIO peripheral

    SDMMC_TypeDef *SDIO = SDMMC;
    #if defined(STM32F7)
    SDIO->CLKCR = SDMMC_CLKCR_HWFC_EN | SDMMC_CLKCR_PWRSAV | (120 - 2); // 1-bit, 400kHz
    #else
    SDIO->CLKCR = SDMMC_CLKCR_HWFC_EN | SDMMC_CLKCR_PWRSAV | (120 / 2); // 1-bit, 400kHz
    #endif
    mp_hal_delay_us(10);
    SDIO->POWER = 3; // the card is clocked
    mp_hal_delay_us(10);
    SDIO->DCTRL = SDMMC_DCTRL_RWMOD; // RWMOD is SDIO_CK
    #if defined(STM32F7)
    SDIO->CLKCR |= SDMMC_CLKCR_CLKEN;
    #endif
    mp_hal_delay_us(10);

    #if defined(STM32F7)
    __HAL_RCC_DMA2_CLK_ENABLE(); // enable DMA2 peripheral
    #endif

    NVIC_SetPriority(SDMMC_IRQn, irq_pri);

    SDIO->MASK = 0;
    HAL_NVIC_EnableIRQ(SDMMC_IRQn);
}

void sdio_deinit(void) {
    SDMMC_CLK_DISABLE();
}

void sdio_reenable(void) {
    if (SDMMC_IS_CLK_DISABLED()) {
        SDMMC_CLK_ENABLE(); // enable SDIO peripheral
        sdio_enable_high_speed_4bit();
    }
}

void sdio_enable_irq(bool enable) {
    if (enable) {
        SDMMC->MASK |= SDMMC_MASK_SDIOITIE;
    } else {
        SDMMC->MASK &= ~SDMMC_MASK_SDIOITIE;
    }
}

void sdio_enable_high_speed_4bit(void) {
    SDMMC_TypeDef *SDIO = SDMMC;
    SDIO->POWER = 0; // power off
    mp_hal_delay_us(10);
    #if defined(STM32F7)
    SDIO->CLKCR = SDMMC_CLKCR_HWFC_EN | SDMMC_CLKCR_WIDBUS_0 | SDMMC_CLKCR_BYPASS /*| SDMMC_CLKCR_PWRSAV*/; // 4-bit, 48MHz
    #else
    SDIO->CLKCR = SDMMC_CLKCR_HWFC_EN | SDMMC_CLKCR_WIDBUS_0; // 4-bit, 48MHz
    #endif
    mp_hal_delay_us(10);
    SDIO->POWER = 3; // the card is clocked
    mp_hal_delay_us(10);
    SDIO->DCTRL = SDMMC_DCTRL_SDIOEN | SDMMC_DCTRL_RWMOD; // SDIOEN, RWMOD is SDIO_CK
    #if defined(STM32F7)
    SDIO->CLKCR |= SDMMC_CLKCR_CLKEN;
    #endif
    SDIO->MASK = DEFAULT_MASK;
    mp_hal_delay_us(10);
}

void SDMMC_IRQHandler(void) {
    if (SDMMC->STA & SDMMC_STA_CMDREND) {
        SDMMC->ICR = SDMMC_ICR_CMDRENDC;
        uint32_t r1 = SDMMC->RESP1;
        if (SDMMC->RESPCMD == 53 && r1 & 0x800) {
            printf("bad RESP1: %lu %lx\n", SDMMC->RESPCMD, r1);
            sdmmc_error = 0xffffffff;
            SDMMC->MASK &= SDMMC_MASK_SDIOITIE;
            sdmmc_irq_state = SDMMC_IRQ_STATE_DONE;
            return;
        }
        #if defined(STM32H7)
        if (!sdmmc_dma) {
            while (sdmmc_buf_cur < sdmmc_buf_top && (SDMMC->STA & SDMMC_STA_DPSMACT) && !(SDMMC->STA & SDMMC_STA_RXFIFOE)) {
                *(uint32_t *)sdmmc_buf_cur = SDMMC->FIFO;
                sdmmc_buf_cur += 4;
            }
        }
        #endif
        if (sdmmc_buf_cur >= sdmmc_buf_top) {
            // data transfer finished, so we are done
            SDMMC->MASK &= SDMMC_MASK_SDIOITIE;
            sdmmc_irq_state = SDMMC_IRQ_STATE_DONE;
            return;
        }
        if (sdmmc_write) {
            SDMMC->DCTRL =
                SDMMC_DCTRL_SDIOEN
                | SDMMC_DCTRL_RWMOD
                | sdmmc_block_size_log2 << SDMMC_DCTRL_DBLOCKSIZE_Pos
                #if defined(STM32F7)
                | (sdmmc_dma << SDMMC_DCTRL_DMAEN_Pos)
                #endif
                | (!sdmmc_write) << SDMMC_DCTRL_DTDIR_Pos
                    | SDMMC_DCTRL_DTEN
            ;
            if (!sdmmc_dma) {
                SDMMC->MASK |= SDMMC_MASK_TXFIFOHEIE;
            }
        }
        sdmmc_irq_state = SDMMC_IRQ_STATE_CMD_DONE;
    } else if (SDMMC->STA & SDMMC_STA_DATAEND) {
        // data transfer complete
        // note: it's possible to get DATAEND before CMDREND
        SDMMC->ICR = SDMMC_ICR_DATAENDC;
        #if defined(STM32F7)
        // check if there is some remaining data in RXFIFO
        if (!sdmmc_dma) {
            while (SDMMC->STA & SDMMC_STA_RXDAVL) {
                *(uint32_t *)sdmmc_buf_cur = SDMMC->FIFO;
                sdmmc_buf_cur += 4;
            }
        }
        #endif
        if (sdmmc_irq_state == SDMMC_IRQ_STATE_CMD_DONE) {
            // command and data finished, so we are done
            SDMMC->MASK &= SDMMC_MASK_SDIOITIE;
            sdmmc_irq_state = SDMMC_IRQ_STATE_DONE;
        }
    } else if (SDMMC->STA & SDMMC_STA_TXFIFOHE) {
        if (!sdmmc_dma && sdmmc_write) {
            // write up to 8 words to fifo
            for (size_t i = 8; i && sdmmc_buf_cur < sdmmc_buf_top; --i) {
                SDMMC->FIFO = *(uint32_t *)sdmmc_buf_cur;
                sdmmc_buf_cur += 4;
            }
            if (sdmmc_buf_cur >= sdmmc_buf_top) {
                // finished, disable IRQ
                SDMMC->MASK &= ~SDMMC_MASK_TXFIFOHEIE;
            }
        }
    } else if (SDMMC->STA & SDMMC_STA_RXFIFOHF) {
        if (!sdmmc_dma && !sdmmc_write) {
            // read up to 8 words from fifo
            for (size_t i = 8; i && sdmmc_buf_cur < sdmmc_buf_top; --i) {
                *(uint32_t *)sdmmc_buf_cur = SDMMC->FIFO;
                sdmmc_buf_cur += 4;
            }
        }
    } else if (SDMMC->STA & SDMMC_STA_SDIOIT) {
        SDMMC->MASK &= ~SDMMC_MASK_SDIOITIE;
        SDMMC->ICR = SDMMC_ICR_SDIOITC;

        #if MICROPY_PY_NETWORK_CYW43
        extern void (*cyw43_poll)(void);
        if (cyw43_poll) {
            pendsv_schedule_dispatch(PENDSV_DISPATCH_CYW43, cyw43_poll);
        }
        #endif
    } else if (SDMMC->STA & 0x3f) {
        // an error
        sdmmc_error = SDMMC->STA;
        SDMMC->ICR = SDMMC_STATIC_FLAGS;
        SDMMC->MASK &= SDMMC_MASK_SDIOITIE;
        sdmmc_irq_state = SDMMC_IRQ_STATE_DONE;
    }
}

int sdio_transfer(uint32_t cmd, uint32_t arg, uint32_t *resp) {
    #if defined(STM32F7)
    // Wait for any outstanding TX to complete
    while (SDMMC->STA & SDMMC_STA_TXACT) {
    }
    #endif

    #if defined(STM32F7)
    DMA2_Stream3->CR = 0; // ensure DMA is reset
    #endif
    SDMMC->ICR = SDMMC_STATIC_FLAGS; // clear interrupts
    SDMMC->ARG = arg;
    SDMMC->CMD = cmd | SDMMC_CMD_WAITRESP_0 | SDMMC_CMD_CPSMEN;

    sdmmc_irq_state = SDMMC_IRQ_STATE_CMD_DATA_PENDING;
    sdmmc_error = 0;
    sdmmc_buf_cur = NULL;
    sdmmc_buf_top = NULL;
    SDMMC->MASK = (SDMMC->MASK & SDMMC_MASK_SDIOITIE) | SDMMC_MASK_CMDRENDIE | 0x3f;

    uint32_t start = mp_hal_ticks_ms();
    for (;;) {
        __WFI();
        if (sdmmc_irq_state == SDMMC_IRQ_STATE_DONE) {
            break;
        }
        if (mp_hal_ticks_ms() - start > 1000) {
            SDMMC->MASK = DEFAULT_MASK;
            printf("sdio_transfer timeout STA=0x%08x\n", (uint)SDMMC->STA);
            return -MP_ETIMEDOUT;
        }
    }

    SDMMC->MASK &= SDMMC_MASK_SDIOITIE;

    if (sdmmc_error == SDMMC_STA_CCRCFAIL && cmd == 5) {
        // Errata: CMD CRC error is incorrectly generated for CMD 5
        return 0;
    } else if (sdmmc_error) {
        return -(0x1000000 | sdmmc_error);
    }

    uint32_t rcmd = SDMMC->RESPCMD;
    if (rcmd != cmd) {
        printf("sdio_transfer: cmd=%lu rcmd=%lu\n", cmd, rcmd);
        return -MP_EIO;
    }
    if (resp != NULL) {
        *resp = SDMMC->RESP1;
    }
    return 0;
}

int sdio_transfer_cmd53(bool write, uint32_t block_size, uint32_t arg, size_t len, uint8_t *buf) {
    #if defined(STM32F7)
    // Wait for any outstanding TX to complete
    while (SDMMC->STA & SDMMC_STA_TXACT) {
    }
    #endif

    // for SDIO_BYTE_MODE the SDIO chuck of data must be a single block of the length of buf
    int block_size_log2 = 0;
    if (block_size == 4) {
        block_size_log2 = 2;
    } else if (block_size == 8) {
        block_size_log2 = 3;
    } else if (block_size == 16) {
        block_size_log2 = 4;
    } else if (block_size == 32) {
        block_size_log2 = 5;
    } else if (block_size == 64) {
        block_size_log2 = 6;
    } else if (block_size == 128) {
        block_size_log2 = 7;
    } else if (block_size == 256) {
        block_size_log2 = 8;
    } else {
        printf("sdio_transfer_cmd53: invalid block_size %lu", block_size);
        return -MP_EINVAL;
    }

    bool dma = (len > 16);

    SDMMC->ICR = SDMMC_STATIC_FLAGS; // clear interrupts
    SDMMC->MASK &= SDMMC_MASK_SDIOITIE;

    SDMMC->DTIMER = 0x2000000; // about 700ms running at 48MHz
    SDMMC->DLEN = (len + block_size - 1) & ~(block_size - 1);

    #if defined(STM32F7)
    DMA2_Stream3->CR = 0;
    #endif

    if (dma) {
        // prepare DMA so it's ready when the DPSM starts its transfer

        #if defined(STM32F7)
        // enable DMA2 peripheral in case it was turned off by someone else
        RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
        #endif

        if (write) {
            // make sure cache is flushed to RAM so the DMA can read the correct data
            MP_HAL_CLEAN_DCACHE(buf, len);
        } else {
            // make sure cache is flushed and invalidated so when DMA updates the RAM
            // from reading the peripheral the CPU then reads the new data
            MP_HAL_CLEANINVALIDATE_DCACHE(buf, len);
        }

        #if defined(STM32F7)
        if ((uint32_t)buf & 3) {
            printf("sdio_transfer_cmd53: buf=%p is not aligned for DMA\n", buf);
            return -MP_EINVAL;
        }
        uint32_t dma_config =
            2 << DMA_SxCR_MSIZE_Pos // MSIZE word
                | 2 << DMA_SxCR_PSIZE_Pos // PSIZE word
                | write << DMA_SxCR_DIR_Pos // DIR mem-to-periph
                | 1 << DMA_SxCR_PFCTRL_Pos // PFCTRL periph is flow controller
        ;
        uint32_t dma_src = (uint32_t)buf;
        uint32_t dma_dest = (uint32_t)&SDMMC->FIFO;
        uint32_t dma_len = ((len + block_size - 1) & ~(block_size - 1)) / 4;
        dma_nohal_init(&dma_SDIO_0, dma_config);
        dma_nohal_start(&dma_SDIO_0, dma_src, dma_dest, dma_len);
        #else
        SDMMC->IDMABASE0 = (uint32_t)buf;
        SDMMC->IDMACTRL = SDMMC_IDMA_IDMAEN;
        #endif
    } else {
        #if defined(STM32H7)
        SDMMC->IDMACTRL = 0;
        #endif
    }

    // for reading, need to initialise the DPSM before starting the CPSM
    // so that the DPSM is ready to receive when the device sends data
    // (and in case we get a long-running unrelated IRQ here on the host just
    // after writing to CMD to initiate the command)
    if (!write) {
        SDMMC->DCTRL =
            SDMMC_DCTRL_SDIOEN
            | SDMMC_DCTRL_RWMOD
            | block_size_log2 << SDMMC_DCTRL_DBLOCKSIZE_Pos
            #if defined(STM32F7)
            | (dma << SDMMC_DCTRL_DMAEN_Pos)
            #endif
            | (!write) << SDMMC_DCTRL_DTDIR_Pos
                | SDMMC_DCTRL_DTEN
        ;
    }

    SDMMC->ARG = arg;
    SDMMC->CMD = 53 | SDMMC_CMD_WAITRESP_0 | SDMMC_CMD_CPSMEN;

    sdmmc_irq_state = SDMMC_IRQ_STATE_CMD_DATA_PENDING;
    sdmmc_block_size_log2 = block_size_log2;
    sdmmc_write = write;
    sdmmc_dma = dma;
    sdmmc_error = 0;
    sdmmc_buf_cur = (uint8_t *)buf;
    sdmmc_buf_top = (uint8_t *)buf + len;
    SDMMC->MASK = (SDMMC->MASK & SDMMC_MASK_SDIOITIE) | SDMMC_MASK_CMDRENDIE | SDMMC_MASK_DATAENDIE | SDMMC_MASK_RXFIFOHFIE | 0x3f;

    // wait to complete transfer
    uint32_t start = mp_hal_ticks_ms();
    for (;;) {
        __WFI();
        if (sdmmc_irq_state == SDMMC_IRQ_STATE_DONE) {
            break;
        }
        if (mp_hal_ticks_ms() - start > 200) {
            SDMMC->MASK &= SDMMC_MASK_SDIOITIE;
            #if defined(STM32F7)
            printf("sdio_transfer_cmd53: timeout wr=%d len=%u dma=%u buf_idx=%u STA=%08x SDMMC=%08x:%08x DMA=%08x:%08x:%08x RCC=%08x\n", write, (uint)len, (uint)dma, sdmmc_buf_cur - buf, (uint)SDMMC->STA, (uint)SDMMC->DCOUNT, (uint)SDMMC->FIFOCNT, (uint)DMA2->LISR, (uint)DMA2->HISR, (uint)DMA2_Stream3->NDTR, (uint)RCC->AHB1ENR);
            #else
            printf("sdio_transfer_cmd53: timeout wr=%d len=%u dma=%u buf_idx=%u STA=%08x SDMMC=%08x:%08x IDMA=%08x\n", write, (uint)len, (uint)dma, sdmmc_buf_cur - buf, (uint)SDMMC->STA, (uint)SDMMC->DCOUNT, (uint)SDMMC->DCTRL, (uint)SDMMC->IDMACTRL);
            #endif
            #if defined(STM32F7)
            if (sdmmc_dma) {
                dma_nohal_deinit(&dma_SDIO_0);
            }
            #endif
            return -MP_ETIMEDOUT;
        }
    }

    SDMMC->MASK &= SDMMC_MASK_SDIOITIE;

    if (sdmmc_error) {
        #if defined(STM32F7)
        printf("sdio_transfer_cmd53: error=%08lx wr=%d len=%u dma=%u buf_idx=%u STA=%08x SDMMC=%08x:%08x DMA=%08x:%08x:%08x RCC=%08x\n", sdmmc_error, write, (uint)len, (uint)dma, sdmmc_buf_cur - buf, (uint)SDMMC->STA, (uint)SDMMC->DCOUNT, (uint)SDMMC->FIFOCNT, (uint)DMA2->LISR, (uint)DMA2->HISR, (uint)DMA2_Stream3->NDTR, (uint)RCC->AHB1ENR);
        #else
        printf("sdio_transfer_cmd53: error=%08lx wr=%d len=%u dma=%u buf_idx=%u STA=%08x SDMMC=%08x:%08x IDMA=%08x\n", sdmmc_error, write, (uint)len, (uint)dma, sdmmc_buf_cur - buf, (uint)SDMMC->STA, (uint)SDMMC->DCOUNT, (uint)SDMMC->DCTRL, (uint)SDMMC->IDMACTRL);
        #endif
        #if defined(STM32F7)
        if (sdmmc_dma) {
            dma_nohal_deinit(&dma_SDIO_0);
        }
        #endif
        return -(0x1000000 | sdmmc_error);
    }

    if (!sdmmc_dma) {
        if (sdmmc_buf_cur != sdmmc_buf_top) {
            printf("sdio_transfer_cmd53: didn't transfer correct length: cur=%p top=%p\n", sdmmc_buf_cur, sdmmc_buf_top);
            return -MP_EIO;
        }
    } else {
        #if defined(STM32F7)
        dma_nohal_deinit(&dma_SDIO_0);
        #endif
    }

    return 0;
}

#endif
