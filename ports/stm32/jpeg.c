/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Dryw Wade Clere at SparkFun Electronics
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

// Decoder for the jpeg module (see extmod/modjpeg.c), using the JPEG codec found on
// STM32F7, STM32H7 and STM32N6.  The codec parses the header and decodes the image to
// YCbCr MCUs, which are colour converted as they come out while the codec decodes the
// next ones.  The CPU feeds the input FIFO, and empties the output FIFO unless there's
// a DMA channel for it.  On STM32N6 the DMA2D does the colour conversion when it can.

#include <string.h>

#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/mphal.h"

#if MICROPY_PY_JPEG

#include "extmod/modjpeg.h"

#if !defined(JPEG)
#error "jpeg module needs an MCU with a JPEG codec"
#endif

// Give up if the codec makes no progress for this long, eg because the data is truncated.
#define JPEG_TIMEOUT_MS (100)

// The largest MCU, which is 4 luma blocks and 2 chroma blocks.
#define JPEG_MCU_MAX_BYTES (6 * 64)

#if defined(STM32N6)
// Moving the output by DMA is faster than reading the FIFO with the CPU, as the CPU can
// colour convert in the meantime.
#define JPEG_OUT_DMA (1)
#include "dma.h"
// Holds a whole number of MCUs of any size.
#define JPEG_OUT_HALF_SIZE (6144)
static DMA_HandleTypeDef jpeg_dma_out;
static uint8_t jpeg_out_buf[2][JPEG_OUT_HALF_SIZE] __attribute__((aligned(32)));
// The DMA2D can convert the MCUs in the output buffer to RGB.
#define JPEG_DMA2D (1)
#else
#define JPEG_OUT_DMA (0)
#define JPEG_DMA2D (0)
#endif

#if JPEG_DMA2D
// The part of the destination written by the DMA2D, while it's in use.
static uint8_t *jpeg_dma2d_dest;
static size_t jpeg_dma2d_len;
#endif

static void jpeg_clock_enable(void) {
    #if defined(STM32H7)
    __HAL_RCC_JPGDECEN_CLK_ENABLE();
    #else
    __HAL_RCC_JPEG_CLK_ENABLE();
    #endif
}

// Read the image layout from the codec once it has parsed the header.
static void jpeg_get_layout(mp_jpeg_decode_t *d) {
    const uint32_t confr1 = JPEG->CONFR1;
    d->height = (confr1 & JPEG_CONFR1_YSIZE_Msk) >> JPEG_CONFR1_YSIZE_Pos;
    d->width = (JPEG->CONFR3 & JPEG_CONFR3_XSIZE_Msk) >> JPEG_CONFR3_XSIZE_Pos;
    const uint32_t num_components = ((confr1 & JPEG_CONFR1_NF_Msk) >> JPEG_CONFR1_NF_Pos) + 1;

    if (num_components == 1) {
        // A single component scan has one block per MCU, whatever the sampling factors.
        d->gray = true;
        d->num_blocks = 1;
        d->y_blocks_h = 1;
        d->mcu_w = 8;
        d->mcu_h = 8;
        return;
    }

    // YCbCr, where only the luma may have more than one block per MCU.
    const uint32_t y_blocks = ((JPEG->CONFR4 & JPEG_CONFR4_NB_Msk) >> JPEG_CONFR4_NB_Pos) + 1;
    const uint32_t h = (JPEG->CONFR4 & JPEG_CONFR4_HSF_Msk) >> JPEG_CONFR4_HSF_Pos;
    const uint32_t v = (JPEG->CONFR4 & JPEG_CONFR4_VSF_Msk) >> JPEG_CONFR4_VSF_Pos;
    const uint32_t cb_blocks = ((JPEG->CONFR5 & JPEG_CONFR5_NB_Msk) >> JPEG_CONFR5_NB_Pos) + 1;
    const uint32_t cr_blocks = ((JPEG->CONFR6 & JPEG_CONFR6_NB_Msk) >> JPEG_CONFR6_NB_Pos) + 1;
    if (num_components != 3 || cb_blocks != 1 || cr_blocks != 1 || h < 1 || h > 2 || v < 1 || v > 2 || h * v != y_blocks) {
        mp_raise_ValueError(MP_ERROR_TEXT("unsupported JPEG type"));
    }
    d->gray = false;
    d->num_blocks = y_blocks + 2;
    d->y_blocks_h = h;
    d->mcu_w = 8 * h;
    d->mcu_h = 8 * v;
    d->chroma_shift_x = h - 1;
    d->chroma_shift_y = v - 1;
}

typedef struct {
    const uint8_t *src;
    size_t len;
    size_t pos;
    mp_uint_t t_progress;
} jpeg_input_t;

// Keep the input FIFO topped up.  The last word may run past the end of the data, but
// as the data is word aligned it stays within the same word.
static void jpeg_feed_input(jpeg_input_t *in) {
    while (in->pos < in->len && (JPEG->SR & JPEG_SR_IFNFF)) {
        JPEG->DIR = *(const uint32_t *)(in->src + in->pos);
        in->pos += 4;
        in->t_progress = mp_hal_ticks_ms();
    }
}

static void jpeg_check_timeout(jpeg_input_t *in) {
    if (mp_hal_ticks_ms() - in->t_progress > JPEG_TIMEOUT_MS) {
        mp_raise_OSError(MP_ETIMEDOUT);
    }
}

#if JPEG_DMA2D

// The DMA2D converts whole MCUs, so it can't scale, and the image must fit in the
// destination.  Swapping RGB565 bytes needs an even number of pixels per line.
static bool jpeg_dma2d_usable(const mp_jpeg_decode_t *d) {
    return (d->format == MP_JPEG_FORMAT_RGB565 || d->format == MP_JPEG_FORMAT_RGB888)
           && d->scale == 1 && !d->gray
           && d->dest_x >= 0 && d->dest_x + d->width <= d->dest_w
           && d->dest_y >= 0 && d->dest_y + d->height <= d->dest_h
           && !(d->swap && d->format == MP_JPEG_FORMAT_RGB565 && (d->width & 1));
}

static void jpeg_dma2d_start(mp_jpeg_decode_t *d) {
    __HAL_RCC_DMA2D_CLK_ENABLE();

    const uint32_t css = d->chroma_shift_y ? 2 : d->chroma_shift_x;
    DMA2D->FGPFCCR = 0xb << DMA2D_FGPFCCR_CM_Pos | css << DMA2D_FGPFCCR_CSS_Pos; // YCbCr
    if (d->format == MP_JPEG_FORMAT_RGB565) {
        DMA2D->OPFCCR = 2 << DMA2D_OPFCCR_CM_Pos | (d->swap ? DMA2D_OPFCCR_SB : 0);
    } else {
        // The DMA2D stores blue first, unless red and blue are swapped.
        DMA2D->OPFCCR = 1 << DMA2D_OPFCCR_CM_Pos | (d->swap ? 0 : DMA2D_OPFCCR_RBS);
    }

    const size_t bpp = d->format == MP_JPEG_FORMAT_RGB565 ? 2 : 3;
    jpeg_dma2d_dest = d->dest + (d->dest_y * d->dest_w + d->dest_x) * bpp;
    jpeg_dma2d_len = ((d->height - 1) * d->dest_w + d->width) * bpp;
    dma_protect_rx_region(jpeg_dma2d_dest, jpeg_dma2d_len);
}

static void jpeg_dma2d_stop(void) {
    if (jpeg_dma2d_dest == NULL) {
        return;
    }
    if (DMA2D->CR & DMA2D_CR_START) {
        DMA2D->CR |= DMA2D_CR_ABORT;
        mp_uint_t t0 = mp_hal_ticks_ms();
        while ((DMA2D->CR & DMA2D_CR_START) && mp_hal_ticks_ms() - t0 < 2) {
        }
    }
    dma_unprotect_rx_region(jpeg_dma2d_dest, jpeg_dma2d_len);
    jpeg_dma2d_dest = NULL;
    __HAL_RCC_DMA2D_CLK_DISABLE();
}

// Convert MCUs to the destination with the DMA2D, one run along an MCU row at a time.
static void jpeg_dma2d_convert(mp_jpeg_decode_t *d, jpeg_input_t *in, const uint8_t *src, size_t mcus) {
    const size_t mcu_bytes = d->num_blocks * 64;
    const size_t bpp = d->format == MP_JPEG_FORMAT_RGB565 ? 2 : 3;
    const size_t mcus_per_row = (d->width + d->mcu_w - 1) / d->mcu_w;
    while (mcus) {
        const size_t run = MIN(mcus, mcus_per_row - d->mcu_x / d->mcu_w);
        const uint32_t pixels = MIN(run * d->mcu_w, (size_t)(d->width - d->mcu_x));
        const uint32_t lines = MIN(d->mcu_h, d->height - d->mcu_y);
        DMA2D->IFCR = DMA2D_IFCR_CTEIF | DMA2D_IFCR_CTCIF | DMA2D_IFCR_CCEIF;
        DMA2D->FGMAR = (uint32_t)src;
        DMA2D->FGOR = run * d->mcu_w - pixels; // padding of a partial MCU
        DMA2D->OMAR = (uint32_t)(d->dest + ((d->dest_y + d->mcu_y) * d->dest_w + d->dest_x + d->mcu_x) * bpp);
        DMA2D->OOR = d->dest_w - pixels;
        DMA2D->NLR = pixels << DMA2D_NLR_PL_Pos | lines;
        DMA2D->CR = DMA2D_CR_MODE_0 | DMA2D_CR_START; // memory to memory with conversion
        while (DMA2D->CR & DMA2D_CR_START) {
            jpeg_feed_input(in);
            jpeg_check_timeout(in);
        }
        if (DMA2D->ISR & (DMA2D_ISR_TEIF | DMA2D_ISR_CEIF)) {
            mp_raise_OSError(MP_EIO);
        }
        src += run * mcu_bytes;
        mcus -= run;
        d->mcu_x += run * d->mcu_w;
        if (d->mcu_x >= d->width) {
            d->mcu_x = 0;
            d->mcu_y += d->mcu_h;
        }
    }
}

#endif

#if JPEG_OUT_DMA

static void jpeg_dma_out_start(uint8_t *dest, size_t len) {
    DMA_Channel_TypeDef *ch = jpeg_dma_out.Instance;
    ch->CFCR = DMA_CFCR_TCF | DMA_CFCR_HTF | DMA_CFCR_DTEF | DMA_CFCR_ULEF | DMA_CFCR_USEF | DMA_CFCR_SUSPF | DMA_CFCR_TOF;
    ch->CBR1 = len;
    ch->CSAR = (uint32_t)&JPEG->DOR;
    ch->CDAR = (uint32_t)dest;
    ch->CCR |= DMA_CCR_EN;
    JPEG->CR |= JPEG_CR_ODMAEN;
}

static bool jpeg_dma_out_done(void) {
    const uint32_t csr = jpeg_dma_out.Instance->CSR;
    if (csr & (DMA_CSR_DTEF | DMA_CSR_ULEF | DMA_CSR_USEF)) {
        mp_raise_OSError(MP_EIO);
    }
    return csr & DMA_CSR_TCF;
}

static void jpeg_dma_out_stop(void) {
    DMA_Channel_TypeDef *ch = jpeg_dma_out.Instance;
    if (ch == NULL) {
        return;
    }
    if (ch->CCR & DMA_CCR_EN) {
        ch->CCR |= DMA_CCR_SUSP;
        mp_uint_t t0 = mp_hal_ticks_ms();
        while (!(ch->CSR & DMA_CSR_SUSPF) && mp_hal_ticks_ms() - t0 < 2) {
        }
        ch->CCR |= DMA_CCR_RESET;
    }
    dma_deinit(&dma_JPEG_OUT);
}

// The DMA fills one half of the output buffer while the CPU or DMA2D converts the MCUs
// in the other half, feeding the input in between.
static void jpeg_output_dma(mp_jpeg_decode_t *d, jpeg_input_t *in, size_t mcus) {
    const size_t mcu_bytes = d->num_blocks * 64;
    const size_t half_size = JPEG_OUT_HALF_SIZE / mcu_bytes * mcu_bytes;
    const size_t total = mcus * mcu_bytes;
    size_t half = 0;
    size_t half_len[2];
    half_len[0] = MIN(half_size, total);
    size_t started = half_len[0];
    size_t done = 0;

    #if JPEG_DMA2D
    const bool use_dma2d = jpeg_dma2d_usable(d);
    if (use_dma2d) {
        jpeg_dma2d_start(d);
    }
    #endif

    dma_init(&jpeg_dma_out, &dma_JPEG_OUT, DMA_PERIPH_TO_MEMORY, NULL);
    jpeg_dma_out_start(jpeg_out_buf[0], half_len[0]);
    while (done < total) {
        jpeg_feed_input(in);
        if (!jpeg_dma_out_done()) {
            jpeg_check_timeout(in);
            continue;
        }
        const size_t full = half;
        JPEG->CR &= ~JPEG_CR_ODMAEN;
        if (started < total) {
            half ^= 1;
            half_len[half] = MIN(half_size, total - started);
            jpeg_dma_out_start(jpeg_out_buf[half], half_len[half]);
            started += half_len[half];
        }
        #if JPEG_DMA2D
        if (use_dma2d) {
            jpeg_dma2d_convert(d, in, jpeg_out_buf[full], half_len[full] / mcu_bytes);
        } else
        #endif
        {
            SCB_InvalidateDCache_by_Addr(jpeg_out_buf[full], half_len[full]);
            for (size_t i = 0; i < half_len[full]; i += mcu_bytes) {
                mp_jpeg_output_mcu(d, jpeg_out_buf[full] + i);
                jpeg_feed_input(in);
            }
        }
        done += half_len[full];
        in->t_progress = mp_hal_ticks_ms();
    }
}

#else

// The CPU empties the output FIFO, 8 words at a time when it's at least half full, and
// converts each MCU once it's complete.
static void jpeg_output_cpu(mp_jpeg_decode_t *d, jpeg_input_t *in, size_t mcus) {
    uint32_t mcu[JPEG_MCU_MAX_BYTES / 4];
    const size_t mcu_words = d->num_blocks * 64 / 4;
    size_t mcu_fill = 0;
    for (;;) {
        jpeg_feed_input(in);
        const uint32_t sr = JPEG->SR;
        size_t n;
        if (sr & JPEG_SR_OFTF) {
            n = 8;
        } else if (sr & JPEG_SR_OFNEF) {
            n = 1;
        } else {
            jpeg_check_timeout(in);
            continue;
        }
        in->t_progress = mp_hal_ticks_ms();
        while (n--) {
            mcu[mcu_fill++] = JPEG->DOR;
            if (mcu_fill == mcu_words) {
                mp_jpeg_output_mcu(d, (const uint8_t *)mcu);
                mcu_fill = 0;
                if (--mcus == 0) {
                    return;
                }
            }
        }
    }
}

#endif

static void jpeg_decode(mp_jpeg_decode_t *d, const uint8_t *src, size_t len) {
    jpeg_input_t in = { .src = src, .len = len, .pos = 0, .t_progress = mp_hal_ticks_ms() };

    JPEG->CR = JPEG_CR_JCEN;
    JPEG->CONFR0 = 0;
    JPEG->CONFR1 = JPEG_CONFR1_DE | JPEG_CONFR1_HDR;
    JPEG->CR |= JPEG_CR_IFF | JPEG_CR_OFF;
    JPEG->CFR = JPEG_CFR_CEOCF | JPEG_CFR_CHPDF;
    JPEG->CONFR0 = JPEG_CONFR0_START;

    // Feed the codec until it has parsed the header.
    while (!(JPEG->SR & JPEG_SR_HPDF)) {
        jpeg_feed_input(&in);
        jpeg_check_timeout(&in);
    }
    jpeg_get_layout(d);
    JPEG->CFR = JPEG_CFR_CHPDF;
    const size_t mcus = (size_t)((d->width + d->mcu_w - 1) / d->mcu_w) * ((d->height + d->mcu_h - 1) / d->mcu_h);

    #if JPEG_OUT_DMA
    jpeg_output_dma(d, &in, mcus);
    #else
    jpeg_output_cpu(d, &in, mcus);
    #endif
}

static void jpeg_stop(void) {
    #if JPEG_OUT_DMA
    jpeg_dma_out_stop();
    #endif
    #if JPEG_DMA2D
    jpeg_dma2d_stop();
    #endif
    JPEG->CR = JPEG_CR_JCEN;
    JPEG->CONFR0 = 0;
    JPEG->CR |= JPEG_CR_IFF | JPEG_CR_OFF;
    JPEG->CFR = JPEG_CFR_CEOCF | JPEG_CFR_CHPDF;
}

void mp_jpeg_port_decode(mp_jpeg_decode_t *d, const uint8_t *src, size_t len) {
    jpeg_clock_enable();
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        jpeg_decode(d, src, len);
        nlr_pop();
        jpeg_stop();
    } else {
        jpeg_stop();
        nlr_jump(nlr.ret_val);
    }
}

#endif // MICROPY_PY_JPEG
