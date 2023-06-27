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
    R_SPI0,
    R_SPI1,
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

    #elif defined(RA6M2) || defined(RA6M3)

    { AF_SPI, 0, P101 }, /* MOSIA_A */
    { AF_SPI, 0, P411 }, /* MOSIA_B */
    { AF_SPI, 1, P109 }, /* MOSIB_B */
    { AF_SPI, 1, P203 }, /* MOSIB_A */
    { AF_SPI, 1, P701 }, /* MOSIB_C */

    #elif defined(RA6M5)

    { AF_SPI, 0, P109 }, /* MOSIA_B */
    { AF_SPI, 0, P203 }, /* MOSIA_A */
    { AF_SPI, 0, P701 }, /* MOSIA_C */
    { AF_SPI, 1, P101 }, /* MOSIB_A */
    { AF_SPI, 1, P411 }, /* MOSIB_B */

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

    #elif defined(RA6M2) || defined(RA6M3)

    { AF_SPI, 0, P100 }, /* MISOA_A */
    { AF_SPI, 0, P410 }, /* MISOA_B */
    { AF_SPI, 1, P110 }, /* MISOB_B */
    { AF_SPI, 1, P202 }, /* MISOB_A */
    { AF_SPI, 1, P700 }, /* MISOB_C */

    #elif defined(RA6M5)

    { AF_SPI, 0, P110 }, /* MISOA_B */
    { AF_SPI, 0, P202 }, /* MISOA_A */
    { AF_SPI, 0, P700 }, /* MISOA_C */
    { AF_SPI, 1, P100 }, /* MISOB_A */
    { AF_SPI, 1, P410 }, /* MISOB_B */

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

    #elif defined(RA6M2) || defined(RA6M3)

    { AF_SPI, 0, P102 }, /* RSPCKA_A */
    { AF_SPI, 0, P412 }, /* RSPCKA_B */
    { AF_SPI, 1, P111 }, /* RSPCKB_B */
    { AF_SPI, 1, P204 }, /* RSPCKB_A */
    { AF_SPI, 1, P702 }, /* RSPCKB_C */

    #elif defined(RA6M5)

    { AF_SPI, 0, P111 }, /* RSPCKA_B */
    { AF_SPI, 0, P204 }, /* RSPCKA_A */
    { AF_SPI, 0, P702 }, /* RSPCKA_C */
    { AF_SPI, 1, P102 }, /* RSPCKB_A */
    { AF_SPI, 1, P412 }, /* RSPCKB_B */

    #else
    #error "CMSIS MCU Series is not specified."
    #endif
};
#define SCK_PINS_SIZE sizeof(sck_pins) / sizeof(ra_af_pin_t)

typedef struct ra_ssl_pin {
    uint8_t ssln;
    uint32_t pin;
} ra_ssl_pin_t;

static const ra_ssl_pin_t ssl_pins[] = {
    #if defined(RA4M1)

    { 0, P103 }, /* SSLA0 */
    { 1, P104 }, /* SSLA1 */
    { 2, P105 }, /* SSLA2 */
    { 3, P106 }, /* SSLA3 */
    { 0, P108 }, /* SSLB0 */
    { 0, P112 }, /* SSLB0 */

    { 0, P205 }, /* SSLB0 */
    { 1, P206 }, /* SSLB1 */

    { 1, P300 }, /* SSLB1 */
    { 2, P301 }, /* SSLB2 */
    { 3, P302 }, /* SSLB3 */

    { 3, P407 }, /* SSLB3 */
    { 0, P413 }, /* SSLA0 */
    { 1, P414 }, /* SSLA1 */
    { 2, P415 }, /* SSLA2 */

    #elif defined(RA4W1)

    { 0, P103 }, /* SSLA0 */
    { 1, P104 }, /* SSLA1 */
    { 2, P105 }, /* SSLA2 */
    { 3, P106 }, /* SSLA3 */
    { 0, P108 }, /* SSLB0 */

    { 0, P205 }, /* SSLB0 */
    { 1, P206 }, /* SSLB1 */

    { 1, P300 }, /* SSLB1 */

    { 3, P407 }, /* SSLB3 */
    { 1, P414 }, /* SSLA1 */

    #elif defined(RA6M1)

    { 0, P103 }, /* SSLA0_A */
    { 1, P104 }, /* SSLA1_A */
    { 2, P105 }, /* SSLA2_A */
    { 3, P106 }, /* SSLA3_A */
    { 0, P108 }, /* SSLB0_B */
    { 0, P112 }, /* SSLB0_B */

    { 1, P300 }, /* SSLB1_B */
    { 2, P301 }, /* SSLB2_B */
    { 3, P302 }, /* SSLB3_B */

    { 0, P413 }, /* SSLA0_B */
    { 1, P414 }, /* SSLA1_B */
    { 2, P415 }, /* SSLA2_B */

    { 3, P708 }, /* SSLA3_B */

    #elif defined(RA6M2) || defined(RA6M3)

    { 0, P103 }, /* SSLA0_A */
    { 1, P104 }, /* SSLA1_A */
    { 2, P105 }, /* SSLA2_A */
    { 3, P106 }, /* SSLA3_A */
    { 0, P108 }, /* SSLB0_B */
    { 0, P112 }, /* SSLB0_B */

    { 0, P205 }, /* SSLB0_A */
    { 1, P206 }, /* SSLB1_A */
    { 2, P207 }, /* SSLB2_A */

    { 1, P300 }, /* SSLB1_B */
    { 2, P301 }, /* SSLB2_B */
    { 3, P302 }, /* SSLB3_B */

    { 3, P406 }, /* SSLB3_C */
    { 3, P407 }, /* SSLB3_A */
    { 0, P413 }, /* SSLA0_B */
    { 1, P414 }, /* SSLA1_B */
    { 2, P415 }, /* SSLA2_B */

    { 0, P703 }, /* SSLB0_C */
    { 1, P704 }, /* SSLB1_C */
    { 2, P705 }, /* SSLB2_C */
    { 3, P708 }, /* SSLA3_B */

    #elif defined(RA6M5)

    { 0, P103 }, /* SSLB0_A */
    { 1, P104 }, /* SSLB1_A */
    { 2, P105 }, /* SSLB2_A */
    { 3, P106 }, /* SSLB3_A */
    { 0, P108 }, /* SSLA0_B */
    { 0, P112 }, /* SSLA0_B */

    { 0, P205 }, /* SSLA0_A */
    { 1, P206 }, /* SSLA1_A */
    { 2, P207 }, /* SSLA2_A */

    { 1, P300 }, /* SSLA1_B */
    { 2, P301 }, /* SSLA2_B */
    { 3, P302 }, /* SSLA3_B */

    { 3, P406 }, /* SSLA3_C */
    { 3, P407 }, /* SSLA3_A */
    { 0, P413 }, /* SSLB0_B */
    { 1, P414 }, /* SSLB1_B */
    { 2, P415 }, /* SSLB2_B */

    { 0, P703 }, /* SSLA0_C */
    { 1, P704 }, /* SSLA1_C */
    { 2, P705 }, /* SSLA2_C */
    { 3, P708 }, /* SSLB3_B */

    #else
    #error "CMSIS MCU Series is not specified."
    #endif
};
#define SSL_PINS_SIZE sizeof(ssl_pins) / sizeof(ra_ssl_pin_t)

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

static bool ra_spi_pin_to_ssln(uint32_t pin, uint8_t *ssln) {
    ra_ssl_pin_t *ssl_pin = (ra_ssl_pin_t *)&ssl_pins;
    bool find = false;
    uint32_t i;
    for (i = 0; i < SSL_PINS_SIZE; i++) {
        if (ssl_pin->pin == pin) {
            find = true;
            if (find) {
                *ssln = ssl_pin->ssln;
            } else {
                *ssln = 0;
            }
            break;
        }
        ssl_pin++;
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

static void ra_spi_set_bits(uint32_t ch, uint32_t bits) {
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

static void ra_spi_set_lsb_first(uint32_t ch, uint32_t lsb_first) {
    R_SPI0_Type *spi_reg = (R_SPI0_Type *)spi_regs[ch];
    if (lsb_first) {
        spi_reg->SPCMD_b[0].LSBF = 1;  // LSB first
    } else {
        spi_reg->SPCMD_b[0].LSBF = 0;  // MSB first
    }
}

static void ra_spi_set_mode(uint32_t ch, uint32_t polarity, uint32_t phase) {
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
    while (count-- > 0) {
        while (spi_reg->SPSR_b.SPTEF == 0) {
            ;
        }
        spi_reg->SPDR_BY = (uint8_t)(*src++);
        while (spi_reg->SPSR_b.SPRF == 0) {
            ;
        }
        if (dst) {
            *dst++ = (uint8_t)(spi_reg->SPDR_BY);
        } else {
            spi_reg->SPDR_BY;
        }
    }
}

void ra_spi_transfer16(uint32_t ch, uint16_t *dst, uint16_t *src, uint32_t count) {
    R_SPI0_Type *spi_reg = (R_SPI0_Type *)spi_regs[ch];
    while (count-- > 0) {
        while (spi_reg->SPSR_b.SPTEF == 0) {
            ;
        }
        spi_reg->SPDR_HA = (uint16_t)(*src++);
        while (spi_reg->SPSR_b.SPRF == 0) {
            ;
        }
        if (dst) {
            *dst++ = (uint16_t)(spi_reg->SPDR_HA);
        } else {
            spi_reg->SPDR_HA;
        }
    }
}

void ra_spi_transfer32(uint32_t ch, uint32_t *dst, uint32_t *src, uint32_t count) {
    R_SPI0_Type *spi_reg = (R_SPI0_Type *)spi_regs[ch];
    while (count-- > 0) {
        while (spi_reg->SPSR_b.SPTEF == 0) {
            ;
        }
        spi_reg->SPDR = (uint32_t)(*src++);
        while (spi_reg->SPSR_b.SPRF == 0) {
            ;
        }
        if (dst) {
            *dst++ = (uint32_t)(spi_reg->SPDR);
        } else {
            spi_reg->SPDR;
        }
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

void ra_spi_init(uint32_t ch, uint32_t mosi, uint32_t miso, uint32_t sck, uint32_t cs, uint32_t baud, uint32_t bits, uint32_t polarity, uint32_t phase, uint32_t firstbit) {
    R_SPI0_Type *spi_reg = (R_SPI0_Type *)spi_regs[ch];
    uint8_t ssln = 0;
    uint8_t sslp = 0;

    ra_spi_pin_to_ssln(cs, &ssln);
    sslp &= ~0x0fU;
    sslp |= (uint8_t)polarity << ssln;

    ra_spi_module_start(ch);

    spi_reg->SPSR;              // dummy read to clear OVRF
    spi_reg->SPSR = 0xa0;       // clear all status

    spi_reg->SPCR = 0x00;       // disable SPI
    spi_reg->SSLP = sslp;       // select slave active polarity
    spi_reg->SPPCR = 0x00;      // fixed idle value, disable loop-back mode
    spi_reg->SPBR = (PCLK / 2 / baud) - 1;      // Set baudrate
    spi_reg->SPDCR = 0x40;      // SPBYT=1, SPLW=0 byte access
    spi_reg->SPCKD = 0x00;      // 1RSPCK
    spi_reg->SPDCR = 0x00;      // 1RSPCK
    spi_reg->SPND = 0x00;       // 1RSPCK + 2PCLKA
    spi_reg->SPCR2 = 0x10;      // SCKASE=1
    spi_reg->SPSCR = 0x00;      // Disable sequence control
    spi_reg->SPCMD[0] = (0xe700 | (ssln << 4));  // LSBF=0, SPB=7, SSLA:ssln, BRDV=0, CPOL=0, CPHA=0
    spi_reg->SPDCR2 = 0x00;     // BYSW=0, SINV=0(RA6M5)

    /* set other setting */
    ra_spi_set_mode(ch, polarity, phase);
    ra_spi_set_bits(ch, bits);
    ra_spi_set_lsb_first(ch, firstbit);

    /* NIVC, DMAC setting */

    /* I/O port setting */
    ra_spi_set_pin(mosi, false);
    ra_spi_set_pin(miso, true);
    ra_spi_set_pin(sck, false);
    ra_spi_set_pin(cs, false);

    #if defined(RA6M5)
    spi_reg->SPCR3 = 0x00;      // default
    #endif

    spi_reg->SPCR = 0x48;       // Start SPI in master mode
    spi_reg->SPCR;              // wait for completion

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
