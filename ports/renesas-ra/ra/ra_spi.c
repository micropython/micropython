/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2021,2022 Renesas Electronics Corporation
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

#include "hal_data.h"
#include "ra_config.h"
#include "ra_gpio.h"
#include "ra_utils.h"
#include "ra_spi.h"

#if defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wconversion"
// #pragma GCC diagnostic ignored "-Wshift-negative-value"
// #pragma GCC diagnostic ignored "-Wunused-but-set-variable"
// #pragma GCC diagnostic ignored "-Wsequence-point"
// #pragma GCC diagnostic ignored "-Wunused-function"
#endif

static R_SPI0_Type *spi_regs[] = {
    (R_SPI0_Type *)0x40072000,
    (R_SPI0_Type *)0x40072100,
};

static const ra_af_pin_t mosi_pins[] = {
    #if defined(RA4M1)

    { AF_SPI, 0, P101 }, /* MOSIA */
    { AF_SPI, 0, P411 }, /* MOSIA */
    { AF_SPI, 1, P109 }, /* MOSIB */
    { AF_SPI, 1, P203 }, /* MOSIB */

    #elif defined(RA4W1)

    { AF_SPI, 0, P101 }, /* MOSIA */
    { AF_SPI, 1, P109 }, /* MOSIB */

    #elif defined(RA6M1)

    { AF_SPI, 0, P101 }, /* MOSIA_A */
    { AF_SPI, 0, P411 }, /* MOSIA_B */
    { AF_SPI, 1, P109 }, /* MOSIB_B */

    #elif defined(RA6M2)

    { AF_SPI, 0, P101 }, /* MOSIA_A */
    { AF_SPI, 0, P411 }, /* MOSIA_B */
    { AF_SPI, 1, P109 }, /* MOSIB_B */
    { AF_SPI, 1, P203 }, /* MOSIB_A */
    { AF_SPI, 1, P701 }, /* MOSIB_C */

    #else
    #error "CMSIS MCU Series is not specified."
    #endif
};
#define MOSI_PINS_SIZE sizeof(mosi_pins) / sizeof(ra_af_pin_t)

static const ra_af_pin_t miso_pins[] = {
    #if defined(RA4M1)

    { AF_SPI, 0, P100 }, /* MISOA */
    { AF_SPI, 0, P410 }, /* MISOA */
    { AF_SPI, 1, P110 }, /* MISOB */
    { AF_SPI, 1, P202 }, /* MISOB */

    #elif defined(RA4W1)

    { AF_SPI, 0, P100 }, /* MISOA */
    { AF_SPI, 1, P110 }, /* MISOB */

    #elif defined(RA6M1)

    { AF_SPI, 0, P100 }, /* MISOA_A */
    { AF_SPI, 0, P410 }, /* MISOA_B */
    { AF_SPI, 1, P110 }, /* MISOB_B */

    #elif defined(RA6M2)

    { AF_SPI, 0, P100 }, /* MISOA_A */
    { AF_SPI, 0, P410 }, /* MISOA_B */
    { AF_SPI, 1, P110 }, /* MISOB_B */
    { AF_SPI, 1, P202 }, /* MISOB_A */
    { AF_SPI, 1, P700 }, /* MISOB_C */

    #else
    #error "CMSIS MCU Series is not specified."
    #endif
};
#define MISO_PINS_SIZE sizeof(miso_pins) / sizeof(ra_af_pin_t)

static const ra_af_pin_t sck_pins[] = {
    #if defined(RA4M1)

    { AF_SPI, 0, P102 }, /* RSPCKA */
    { AF_SPI, 0, P412 }, /* RSPCKA */
    { AF_SPI, 1, P111 }, /* RSPCKB */
    { AF_SPI, 1, P204 }, /* RSPCKB */

    #elif defined(RA4W1)

    { AF_SPI, 0, P102 }, /* RSPCKA */
    { AF_SPI, 1, P111 }, /* RSPCKB */

    #elif defined(RA6M1)

    { AF_SPI, 0, P102 }, /* RSPCKA_A */
    { AF_SPI, 0, P412 }, /* RSPCKA_B */
    { AF_SPI, 1, P111 }, /* RSPCKB_B */

    #elif defined(RA6M2)

    { AF_SPI, 0, P102 }, /* RSPCKA_A */
    { AF_SPI, 0, P412 }, /* RSPCKA_B */
    { AF_SPI, 1, P111 }, /* RSPCKB_B */
    { AF_SPI, 1, P204 }, /* RSPCKB_A */
    { AF_SPI, 1, P702 }, /* RSPCKB_C */

    #else
    #error "CMSIS MCU Series is not specified."
    #endif
};
#define SCK_PINS_SIZE sizeof(sck_pins) / sizeof(ra_af_pin_t)

bool ra_af_find_ch(ra_af_pin_t *af_pin, uint32_t size, uint32_t pin, uint8_t *ch) {
    bool find = false;
    uint32_t i;
    for (i = 0; i < size; i++) {
        if (af_pin->pin == pin) {
            find = true;
            *ch = af_pin->ch;
            break;
        }
        af_pin++;
    }
    return find;
}

bool ra_spi_find_af_ch(uint32_t mosi, uint32_t miso, uint32_t sck, uint8_t *ch) {
    bool find = false;
    uint8_t mosi_ch;
    uint8_t miso_ch;
    uint8_t sck_ch;
    find = ra_af_find_ch((ra_af_pin_t *)&mosi_pins, MOSI_PINS_SIZE, mosi, &mosi_ch);
    if (find) {
        find = ra_af_find_ch((ra_af_pin_t *)&miso_pins, MISO_PINS_SIZE, miso, &miso_ch);
        if (find) {
            find = ra_af_find_ch((ra_af_pin_t *)&sck_pins, SCK_PINS_SIZE, sck, &sck_ch);
            if (find) {
                find = (mosi_ch == miso_ch) && (miso_ch == sck_ch);
                if (find) {
                    *ch = mosi_ch;
                } else {
                    *ch = 0;
                }
            }
        }
    }
    return find;
}

static void ra_spi_module_start(uint32_t ch) {
    if (ch == 0) {
        ra_mstpcrb_start(R_MSTP_MSTPCRB_MSTPB19_Msk);
    } else {
        ra_mstpcrb_start(R_MSTP_MSTPCRB_MSTPB18_Msk);
    }
}

static void ra_spi_module_stop(uint32_t ch) {
    if (ch == 0) {
        ra_mstpcrb_stop(R_MSTP_MSTPCRB_MSTPB19_Msk);
    } else {
        ra_mstpcrb_stop(R_MSTP_MSTPCRB_MSTPB18_Msk);
    }
}

static void ra_spi_set_pin(uint32_t pin, bool miso) {
    if (miso) {
        ra_gpio_config(pin, GPIO_MODE_INPUT, GPIO_PULLUP, GPIO_LOW_POWER, AF_SPI);
    } else {
        ra_gpio_config(pin, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_LOW_POWER, AF_SPI);
    }
}

void ra_spi_set_bits(uint32_t ch, uint32_t bits) {
    R_SPI0_Type *spi_reg = (R_SPI0_Type *)spi_regs[ch];
    if (bits == 8) {
        spi_reg->SPDCR_b.SPBYT = 1;
        spi_reg->SPDCR_b.SPLW = 0;
        spi_reg->SPCMD_b[0].SPB = 0x7;
    } else if (bits == 16) {
        spi_reg->SPDCR_b.SPBYT = 0;
        spi_reg->SPDCR_b.SPLW = 0;
        spi_reg->SPCMD_b[0].SPB = 0xf;
    } else if (bits == 32) {
        spi_reg->SPDCR_b.SPBYT = 0;
        spi_reg->SPDCR_b.SPLW = 1;
        spi_reg->SPCMD_b[0].SPB = 0x3;
    }
}

void ra_spi_set_clk(uint32_t ch, uint32_t baud) {
    R_SPI0_Type *spi_reg = (R_SPI0_Type *)spi_regs[ch];
    if (baud == 0) {
        return;
    }
    spi_reg->SPCR_b.SPE = 0;
    spi_reg->SPBR = (PCLK / 2 / baud) - 1;
    spi_reg->SPCR_b.SPE = 1;
}

void ra_spi_set_lsb_first(uint32_t ch, uint32_t lsb_first) {
    R_SPI0_Type *spi_reg = (R_SPI0_Type *)spi_regs[ch];
    if (lsb_first) {
        spi_reg->SPCMD_b[0].LSBF = 1;  // LSB first
    } else {
        spi_reg->SPCMD_b[0].LSBF = 0;  // MSB first
    }
}

void ra_spi_set_mode(uint32_t ch, uint32_t polarity, uint32_t phase) {
    R_SPI0_Type *spi_reg = (R_SPI0_Type *)spi_regs[ch];
    if (polarity != 0) {
        // CPOL(Clock Polarity)
        spi_reg->SPCMD_b[0].CPOL = 1;
    } else {
        spi_reg->SPCMD_b[0].CPOL = 0;
    }
    if (phase != 0) {
        // CPHA(Clock Phase)
        spi_reg->SPCMD_b[0].CPHA = 1;
    } else {
        spi_reg->SPCMD_b[0].CPHA = 0;
    }
}

void ra_spi_set_ch(uint32_t ch) {
    R_SPI0_Type *spi_reg = (R_SPI0_Type *)spi_regs[ch];
    spi_reg->SPCR = 0x00;        // disable SPI
    spi_reg->SPSR = 0xa0;
    spi_reg->SPPCR = 0x00;       // fixed idle value, disable loop-back mode
    spi_reg->SPSCR = 0x00;       // Disable sequence control
    spi_reg->SPDCR = 0x40;       // SPBYT=1, SPLW=0 byte access
    spi_reg->SPCMD[0] = 0xe700;  // LSBF=0, SPB=7, BRDV=0, CPOL=0, CPHA=0
    spi_reg->SPCR2 = 0x10;
    spi_reg->SPCR = 0x48;        // Start SPI in master mode
}

uint8_t ra_spi_write_byte(uint32_t ch, uint8_t b) {
    R_SPI0_Type *spi_reg = (R_SPI0_Type *)spi_regs[ch];
    while (spi_reg->SPSR_b.SPTEF == 0) {
        ;
    }
    spi_reg->SPDR_BY = (uint8_t)(b);
    while (spi_reg->SPSR_b.SPRF == 0) {
        ;
    }
    return (uint8_t)(spi_reg->SPDR_BY);
}

void ra_spi_write_bytes8(uint32_t ch, uint8_t *buf, uint32_t count) {
    R_SPI0_Type *spi_reg = (R_SPI0_Type *)spi_regs[ch];
    ra_spi_set_bits(ch, 8);
    while (count-- > 0) {
        while (spi_reg->SPSR_b.SPTEF == 0) {
            ;
        }
        spi_reg->SPDR_BY = (uint8_t)(*buf++);
        while (spi_reg->SPSR_b.SPRF == 0) {
            ;
        }
        spi_reg->SPDR_BY;
    }
}

void ra_spi_read_bytes8(uint32_t ch, uint8_t *buf, uint32_t count) {
    R_SPI0_Type *spi_reg = (R_SPI0_Type *)spi_regs[ch];
    ra_spi_set_bits(ch, 8);
    while (count-- > 0) {
        while (spi_reg->SPSR_b.SPTEF == 0) {
            ;
        }
        spi_reg->SPDR_BY = (uint8_t)(0);
        while (spi_reg->SPSR_b.SPRF == 0) {
            ;
        }
        *buf++ = (uint8_t)spi_reg->SPDR_BY;
    }
}

void ra_spi_write_bytes16(uint32_t ch, uint16_t *buf, uint32_t count) {
    R_SPI0_Type *spi_reg = (R_SPI0_Type *)spi_regs[ch];
    ra_spi_set_bits(ch, 16);
    while (count-- > 0) {
        while (spi_reg->SPSR_b.SPTEF == 0) {
            ;
        }
        spi_reg->SPDR_HA = (uint16_t)(*buf++);
        while (spi_reg->SPSR_b.SPRF == 0) {
            ;
        }
        spi_reg->SPDR_HA;
    }
}

void ra_spi_write_bytes32(uint32_t ch, uint32_t *buf, uint32_t count) {
    R_SPI0_Type *spi_reg = (R_SPI0_Type *)spi_regs[ch];
    ra_spi_set_bits(ch, 32);
    while (count-- > 0) {
        while (spi_reg->SPSR_b.SPTEF == 0) {
            ;
        }
        spi_reg->SPDR = (uint32_t)(*buf++);
        while (spi_reg->SPSR_b.SPRF == 0) {
            ;
        }
        spi_reg->SPDR;
    }
}

void ra_spi_write_bytes(uint32_t ch, uint32_t bits, uint8_t *buf, uint32_t count) {
    if (bits == 8) {
        ra_spi_write_bytes8(ch, buf, count);
    } else if (bits == 16) {
        ra_spi_write_bytes16(ch, (uint16_t *)buf, count >> 1);
    } else if (bits == 32) {
        ra_spi_write_bytes32(ch, (uint32_t *)buf, count >> 2);
    }
}

void ra_spi_transfer8(uint32_t ch, uint8_t *dst, uint8_t *src, uint32_t count) {
    R_SPI0_Type *spi_reg = (R_SPI0_Type *)spi_regs[ch];
    ra_spi_set_bits(ch, 8);
    while (count-- > 0) {
        while (spi_reg->SPSR_b.SPTEF == 0) {
            ;
        }
        spi_reg->SPDR_BY = (uint8_t)(*src++);
        while (spi_reg->SPSR_b.SPRF == 0) {
            ;
        }
        *dst++ = (uint8_t)(spi_reg->SPDR_BY);
    }
}

void ra_spi_transfer16(uint32_t ch, uint16_t *dst, uint16_t *src, uint32_t count) {
    R_SPI0_Type *spi_reg = (R_SPI0_Type *)spi_regs[ch];
    ra_spi_set_bits(ch, 16);
    while (count-- > 0) {
        while (spi_reg->SPSR_b.SPTEF == 0) {
            ;
        }
        spi_reg->SPDR_HA = (uint16_t)(*src++);
        while (spi_reg->SPSR_b.SPRF == 0) {
            ;
        }
        *dst++ = (uint16_t)(spi_reg->SPDR_HA);
    }
}

void ra_spi_transfer32(uint32_t ch, uint32_t *dst, uint32_t *src, uint32_t count) {
    R_SPI0_Type *spi_reg = (R_SPI0_Type *)spi_regs[ch];
    ra_spi_set_bits(ch, 32);
    while (count-- > 0) {
        while (spi_reg->SPSR_b.SPTEF == 0) {
            ;
        }
        spi_reg->SPDR = (uint32_t)(*src++);
        while (spi_reg->SPSR_b.SPRF == 0) {
            ;
        }
        *dst++ = (uint32_t)(spi_reg->SPDR);
    }
}

void ra_spi_transfer(uint32_t ch, uint32_t bits, uint8_t *dst, uint8_t *src, uint32_t count, uint32_t timeout) {
    if (bits == 8) {
        ra_spi_transfer8(ch, dst, src, count);
    } else if (bits == 16) {
        ra_spi_transfer16(ch, (uint16_t *)dst, (uint16_t *)src, count >> 1);
    } else if (bits == 32) {
        ra_spi_transfer32(ch, (uint32_t *)dst, (uint32_t *)src, count >> 2);
    }
}

void ra_spi_start_xfer(uint32_t ch, uint16_t spcmd, uint8_t spbr) {
    R_SPI0_Type *spi_reg = (R_SPI0_Type *)spi_regs[ch];
    spi_reg->SPCR_b.SPE = 0;  // disable
    spi_reg->SPCMD[0] = spcmd;
    spi_reg->SPBR = spbr;
    spi_reg->SPCR_b.SPE = 1;  // enable
}

void ra_spi_end_xfer(uint32_t ch) {
    (void)ch;
}

void ra_spi_get_conf(uint32_t ch, uint16_t *spcmd, uint8_t *spbr) {
    R_SPI0_Type *spi_reg = (R_SPI0_Type *)spi_regs[ch];
    *spcmd = spi_reg->SPCMD[0];
    *spbr = spi_reg->SPBR;
}

void ra_spi_init(uint32_t ch, uint32_t mosi, uint32_t miso, uint32_t sck, uint32_t cs, uint32_t baud, uint32_t bits, uint32_t polarity, uint32_t phase) {
    ra_gpio_mode_output(cs);
    ra_gpio_write(cs, 1);
    ra_spi_module_start(ch);
    ra_spi_set_pin(mosi, false);
    ra_spi_set_pin(miso, true);
    ra_spi_set_pin(sck, false);
    ra_spi_set_mode(ch, polarity, phase);
    ra_spi_set_ch(ch);
    ra_spi_set_clk(ch, baud);
    ra_spi_set_bits(ch, bits);
    ra_spi_set_lsb_first(ch, 0);  // MSB first
    return;
}

void ra_spi_deinit(uint32_t ch, uint32_t cs) {
    ra_spi_module_stop(ch);
}

__WEAK void spi_rxi_isr(void) {
    // dummy
}

__WEAK void spi_txi_isr(void) {
    // dummy
}

__WEAK void spi_tei_isr(void) {
    // dummy
}

__WEAK void spi_eri_isr(void) {
    // dummy
}
