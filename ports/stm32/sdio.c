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
#include "genhdr/pins.h"
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

void sdio_init(uint32_t irq_pri) {
    // configure IO pins
    mp_hal_pin_config(pin_C8, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_UP, 12);
    mp_hal_pin_config(pin_C9, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_UP, 12);
    mp_hal_pin_config(pin_C10, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_UP, 12);
    mp_hal_pin_config(pin_C11, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_UP, 12);
    mp_hal_pin_config(pin_C12, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, 12); // CLK doesn't need pull-up
    mp_hal_pin_config(pin_D2, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_UP, 12);

    __HAL_RCC_SDMMC1_CLK_ENABLE(); // enable SDIO peripheral

    SDMMC_TypeDef *SDIO = SDMMC1;
    SDIO->CLKCR = SDMMC_CLKCR_HWFC_EN | SDMMC_CLKCR_PWRSAV | 118; // 1-bit, 400kHz
    mp_hal_delay_us(10);
    SDIO->POWER = 3; // the card is clocked
    mp_hal_delay_us(10);
    SDIO->DCTRL = 1 << 10; // RWMOD is SDIO_CK
    SDIO->CLKCR |= SDMMC_CLKCR_CLKEN;
    mp_hal_delay_us(10);

    __HAL_RCC_DMA2_CLK_ENABLE(); // enable DMA2 peripheral

    NVIC_SetPriority(SDMMC1_IRQn, irq_pri);

    SDIO->MASK = 0;
    HAL_NVIC_EnableIRQ(SDMMC1_IRQn);
}

void sdio_deinit(void) {
    RCC->APB2ENR &= ~RCC_APB2ENR_SDMMC1EN; // disable SDIO peripheral
    RCC->AHB1ENR &= ~RCC_AHB1ENR_DMA2EN; // disable DMA2 peripheral
}

void sdio_enable_high_speed_4bit(void) {
    SDMMC_TypeDef *SDIO = SDMMC1;
    SDIO->POWER = 0; // power off
    mp_hal_delay_us(10);
    SDIO->CLKCR = SDMMC_CLKCR_HWFC_EN | SDMMC_CLKCR_WIDBUS_0 | SDMMC_CLKCR_BYPASS /*| SDMMC_CLKCR_PWRSAV*/; // 4-bit, 48MHz
    mp_hal_delay_us(10);
    SDIO->POWER = 3; // the card is clocked
    mp_hal_delay_us(10);
    SDIO->DCTRL = 1 << 11 | 1 << 10; // SDIOEN, RWMOD is SDIO_CK
    SDIO->CLKCR |= SDMMC_CLKCR_CLKEN;
    SDIO->MASK = DEFAULT_MASK;
    mp_hal_delay_us(10);
}

void SDMMC1_IRQHandler(void) {
    if (SDMMC1->STA & SDMMC_STA_CMDREND) {
        SDMMC1->ICR = SDMMC_ICR_CMDRENDC;
        uint32_t r1 = SDMMC1->RESP1;
        if (SDMMC1->RESPCMD == 53 && r1 & 0x800) {
            printf("bad RESP1: %lu %lx\n", SDMMC1->RESPCMD, r1);
            sdmmc_error = 0xffffffff;
            SDMMC1->MASK &= SDMMC_MASK_SDIOITIE;
            sdmmc_irq_state = SDMMC_IRQ_STATE_DONE;
            return;
        }
        if (sdmmc_buf_cur >= sdmmc_buf_top) {
            // data transfer finished, so we are done
            SDMMC1->MASK &= SDMMC_MASK_SDIOITIE;
            sdmmc_irq_state = SDMMC_IRQ_STATE_DONE;
            return;
        }
        if (sdmmc_write) {
            SDMMC1->DCTRL = (sdmmc_block_size_log2 << 4) | 1 | (1 << 11) | (!sdmmc_write << 1) | (sdmmc_dma << 3) | (0 << 10);
            if (!sdmmc_dma) {
                SDMMC1->MASK |= SDMMC_MASK_TXFIFOHEIE;
            }
        }
        sdmmc_irq_state = SDMMC_IRQ_STATE_CMD_DONE;
    } else if (SDMMC1->STA & SDMMC_STA_DATAEND) {
        // data transfer complete
        // note: it's possible to get DATAEND before CMDREND
        SDMMC1->ICR = SDMMC_ICR_DATAENDC;
        // check if there is some remaining data in RXFIFO
        if (!sdmmc_dma) {
            while (SDMMC1->STA & SDMMC_STA_RXDAVL) {
                *(uint32_t*)sdmmc_buf_cur = SDMMC1->FIFO;
                sdmmc_buf_cur += 4;
            }
        }
        if (sdmmc_irq_state == SDMMC_IRQ_STATE_CMD_DONE) {
            // command and data finished, so we are done
            SDMMC1->MASK &= SDMMC_MASK_SDIOITIE;
            sdmmc_irq_state = SDMMC_IRQ_STATE_DONE;
        }
    } else if (SDMMC1->STA & SDMMC_STA_TXFIFOHE) {
        if (!sdmmc_dma && sdmmc_write) {
            // write up to 8 words to fifo
            for (size_t i = 8; i && sdmmc_buf_cur < sdmmc_buf_top; --i) {
                SDMMC1->FIFO = *(uint32_t*)sdmmc_buf_cur;
                sdmmc_buf_cur += 4;
            }
            if (sdmmc_buf_cur >= sdmmc_buf_top) {
                // finished, disable IRQ
                SDMMC1->MASK &= ~SDMMC_MASK_TXFIFOHEIE;
            }
        }
    } else if (SDMMC1->STA & SDMMC_STA_RXFIFOHF) {
        if (!sdmmc_dma && !sdmmc_write) {
            // read up to 8 words from fifo
            for (size_t i = 8; i && sdmmc_buf_cur < sdmmc_buf_top; --i) {
                *(uint32_t*)sdmmc_buf_cur = SDMMC1->FIFO;
                sdmmc_buf_cur += 4;
            }
        }
    } else if (SDMMC1->STA & SDMMC_STA_SDIOIT) {
        SDMMC1->MASK &= ~SDMMC_MASK_SDIOITIE;
        SDMMC1->ICR = SDMMC_ICR_SDIOITC;

        #if MICROPY_PY_NETWORK_CYW43
        extern void (*cyw43_poll)(void);
        if (cyw43_poll) {
            pendsv_schedule_dispatch(PENDSV_DISPATCH_CYW43, cyw43_poll);
        }
        #endif
    } else if (SDMMC1->STA & 0x3f) {
        // an error
        sdmmc_error = SDMMC1->STA;
        SDMMC1->ICR = SDMMC_STATIC_FLAGS;
        SDMMC1->MASK &= SDMMC_MASK_SDIOITIE;
        sdmmc_irq_state = SDMMC_IRQ_STATE_DONE;
    }
}

int sdio_transfer(uint32_t cmd, uint32_t arg, uint32_t *resp) {
    // Wait for any outstanding TX to complete
    while (SDMMC1->STA & SDMMC_STA_TXACT) {
    }

    DMA2_Stream3->CR = 0; // ensure DMA is reset
    SDMMC1->ICR = SDMMC_STATIC_FLAGS; // clear interrupts
    SDMMC1->ARG = arg;
    SDMMC1->CMD = cmd | SDMMC_CMD_WAITRESP_0 | SDMMC_CMD_CPSMEN;

    sdmmc_irq_state = SDMMC_IRQ_STATE_CMD_DATA_PENDING;
    sdmmc_error = 0;
    sdmmc_buf_cur = NULL;
    sdmmc_buf_top = NULL;
    SDMMC1->MASK = (SDMMC1->MASK & SDMMC_MASK_SDIOITIE) | SDMMC_MASK_CMDRENDIE | 0x3f;

    uint32_t start = mp_hal_ticks_ms();
    for (;;) {
        __WFI();
        if (sdmmc_irq_state == SDMMC_IRQ_STATE_DONE) {
            break;
        }
        if (mp_hal_ticks_ms() - start > 1000) {
            SDMMC1->MASK = DEFAULT_MASK;
            printf("sdio_transfer timeout STA=0x%08x\n", (uint)SDMMC1->STA);
            return -MP_ETIMEDOUT;
        }
    }

    SDMMC1->MASK &= SDMMC_MASK_SDIOITIE;

    if (sdmmc_error == SDMMC_STA_CCRCFAIL && cmd == 5) {
        // Errata: CMD CRC error is incorrectly generated for CMD 5
        return 0;
    } else if (sdmmc_error) {
        return -(0x1000000 | sdmmc_error);
    }

    uint32_t rcmd = SDMMC1->RESPCMD;
    if (rcmd != cmd) {
        printf("sdio_transfer: cmd=%lu rcmd=%lu\n", cmd, rcmd);
        return -MP_EIO;
    }
    if (resp != NULL) {
        *resp = SDMMC1->RESP1;
    }
    return 0;
}

int sdio_transfer_cmd53(bool write, uint32_t block_size, uint32_t arg, size_t len, uint8_t *buf) {
    // Wait for any outstanding TX to complete
    while (SDMMC1->STA & SDMMC_STA_TXACT) {
    }

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

    SDMMC1->ICR = SDMMC_STATIC_FLAGS; // clear interrupts
    SDMMC1->MASK &= SDMMC_MASK_SDIOITIE;

    SDMMC1->DTIMER = 0x2000000; // about 700ms running at 48MHz
    SDMMC1->DLEN = (len + block_size - 1) & ~(block_size - 1);

    DMA2_Stream3->CR = 0;

    if (dma) {
        // prepare DMA so it's ready when the DPSM starts its transfer

        // enable DMA2 peripheral in case it was turned off by someone else
        RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;

        if (write) {
            // make sure cache is flushed to RAM so the DMA can read the correct data
            MP_HAL_CLEAN_DCACHE(buf, len);
        } else {
            // make sure cache is flushed and invalidated so when DMA updates the RAM
            // from reading the peripheral the CPU then reads the new data
            MP_HAL_CLEANINVALIDATE_DCACHE(buf, len);
        }

        DMA2->LIFCR = 0x3f << 22;
        DMA2_Stream3->FCR = 0x07; // ?
        DMA2_Stream3->PAR = (uint32_t)&SDMMC1->FIFO;
        if ((uint32_t)buf & 3) {
            printf("sdio_transfer_cmd53: buf=%p is not aligned for DMA\n", buf);
            return -MP_EINVAL;
        }
        DMA2_Stream3->M0AR = (uint32_t)buf;
        DMA2_Stream3->NDTR = ((len + block_size - 1) & ~(block_size - 1)) / 4;
        DMA2_Stream3->CR = 4 << 25 // channel 4
            | 1 << 23 // MBURST INCR4
            | 1 << 21 // PBURST INCR4
            | 3 << 16 // PL very high
            | 2 << 13 // MSIZE word
            | 2 << 11 // PSIZE word
            | 1 << 10 // MINC enabled
            | 0 << 9 // PINC disabled
            | write << 6 // DIR mem-to-periph
            | 1 << 5 // PFCTRL periph is flow controller
            | 1 << 0 // EN
            ;
    }

    // for reading, need to initialise the DPSM before starting the CPSM
    // so that the DPSM is ready to receive when the device sends data
    // (and in case we get a long-running unrelated IRQ here on the host just
    // after writing to CMD to initiate the command)
    if (!write) {
        SDMMC1->DCTRL = (block_size_log2 << 4) | 1 | (1 << 11) | (!write << 1) | (dma << 3);
    }

    SDMMC1->ARG = arg;
    SDMMC1->CMD = 53 | SDMMC_CMD_WAITRESP_0 | SDMMC_CMD_CPSMEN;

    sdmmc_irq_state = SDMMC_IRQ_STATE_CMD_DATA_PENDING;
    sdmmc_block_size_log2 = block_size_log2;
    sdmmc_write = write;
    sdmmc_dma = dma;
    sdmmc_error = 0;
    sdmmc_buf_cur = (uint8_t*)buf;
    sdmmc_buf_top = (uint8_t*)buf + len;
    SDMMC1->MASK = (SDMMC1->MASK & SDMMC_MASK_SDIOITIE) | SDMMC_MASK_CMDRENDIE | SDMMC_MASK_DATAENDIE | SDMMC_MASK_RXFIFOHFIE | 0x3f;

    // wait to complete transfer
    uint32_t start = mp_hal_ticks_ms();
    for (;;) {
        __WFI();
        if (sdmmc_irq_state == SDMMC_IRQ_STATE_DONE) {
            break;
        }
        if (mp_hal_ticks_ms() - start > 200) {
            SDMMC1->MASK &= SDMMC_MASK_SDIOITIE;
            printf("sdio_transfer_cmd53: timeout wr=%d len=%u dma=%u buf_idx=%u STA=%08x SDMMC=%08x:%08x DMA=%08x:%08x:%08x RCC=%08x\n", write, (uint)len, (uint)dma, sdmmc_buf_cur - buf, (uint)SDMMC1->STA, (uint)SDMMC1->DCOUNT, (uint)SDMMC1->FIFOCNT, (uint)DMA2->LISR, (uint)DMA2->HISR, (uint)DMA2_Stream3->NDTR, (uint)RCC->AHB1ENR);
            return -MP_ETIMEDOUT;
        }
    }

    SDMMC1->MASK &= SDMMC_MASK_SDIOITIE;

    if (sdmmc_error) {
        printf("sdio_transfer_cmd53: error=%08lx wr=%d len=%u dma=%u buf_idx=%u STA=%08x SDMMC=%08x:%08x DMA=%08x:%08x:%08x RCC=%08x\n", sdmmc_error, write, (uint)len, (uint)dma, sdmmc_buf_cur - buf, (uint)SDMMC1->STA, (uint)SDMMC1->DCOUNT, (uint)SDMMC1->FIFOCNT, (uint)DMA2->LISR, (uint)DMA2->HISR, (uint)DMA2_Stream3->NDTR, (uint)RCC->AHB1ENR);
        return -(0x1000000 | sdmmc_error);
    }

    if (!sdmmc_dma) {
        if (sdmmc_buf_cur != sdmmc_buf_top) {
            printf("sdio_transfer_cmd53: didn't transfer correct length: cur=%p top=%p\n", sdmmc_buf_cur, sdmmc_buf_top);
            return -MP_EIO;
        }
    }

    return 0;
}

#endif
