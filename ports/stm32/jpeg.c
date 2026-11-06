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
// YCbCr MCUs.  The CPU moves the data through the codec's FIFOs and colour converts
// each MCU as it comes out, while the codec decodes the next ones.

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

static void jpeg_clock_enable(void) {
    #if defined(STM32H7)
    __HAL_RCC_JPGDECEN_CLK_ENABLE();
    #else
    __HAL_RCC_JPEG_CLK_ENABLE();
    #endif
}

static void jpeg_stop(void) {
    JPEG->CR = JPEG_CR_JCEN;
    JPEG->CONFR0 = 0;
    JPEG->CR |= JPEG_CR_IFF | JPEG_CR_OFF;
    JPEG->CFR = JPEG_CFR_CEOCF | JPEG_CFR_CHPDF;
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

static void jpeg_decode(mp_jpeg_decode_t *d, const uint8_t *src, size_t len) {
    uint32_t mcu[JPEG_MCU_MAX_BYTES / 4];
    size_t mcu_words = 0; // words per MCU, once the layout is known
    size_t mcu_fill = 0;
    size_t mcus_left = 0;
    size_t in_pos = 0;
    mp_uint_t t_progress = mp_hal_ticks_ms();

    JPEG->CR = JPEG_CR_JCEN;
    JPEG->CONFR0 = 0;
    JPEG->CONFR1 = JPEG_CONFR1_DE | JPEG_CONFR1_HDR;
    JPEG->CR |= JPEG_CR_IFF | JPEG_CR_OFF;
    JPEG->CFR = JPEG_CFR_CEOCF | JPEG_CFR_CHPDF;
    JPEG->CONFR0 = JPEG_CONFR0_START;

    for (;;) {
        bool progress = false;

        // Keep the input FIFO topped up.  The last word may run past the end of the
        // data, but it's word aligned so stays within the same word.
        while (in_pos < len && (JPEG->SR & JPEG_SR_IFNFF)) {
            JPEG->DIR = *(const uint32_t *)(src + in_pos);
            in_pos += 4;
            progress = true;
        }

        if (mcu_words == 0) {
            if (!(JPEG->SR & JPEG_SR_HPDF)) {
                goto check_timeout;
            }
            jpeg_get_layout(d);
            JPEG->CFR = JPEG_CFR_CHPDF;
            mcu_words = d->num_blocks * 64 / 4;
            mcus_left = (size_t)((d->width + d->mcu_w - 1) / d->mcu_w) * ((d->height + d->mcu_h - 1) / d->mcu_h);
        }

        // Empty the output FIFO, 8 words at a time when it's at least half full, and
        // convert each MCU once it's complete.
        for (;;) {
            const uint32_t sr = JPEG->SR;
            size_t n;
            if (sr & JPEG_SR_OFTF) {
                n = 8;
            } else if (sr & JPEG_SR_OFNEF) {
                n = 1;
            } else {
                break;
            }
            progress = true;
            while (n--) {
                mcu[mcu_fill++] = JPEG->DOR;
                if (mcu_fill == mcu_words) {
                    mp_jpeg_output_mcu(d, (const uint8_t *)mcu);
                    mcu_fill = 0;
                    if (--mcus_left == 0) {
                        return;
                    }
                }
            }
        }

    check_timeout:
        if (progress) {
            t_progress = mp_hal_ticks_ms();
        } else if (mp_hal_ticks_ms() - t_progress > JPEG_TIMEOUT_MS) {
            mp_raise_OSError(MP_ETIMEDOUT);
        }
    }
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
