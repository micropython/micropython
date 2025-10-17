/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Paul Stoffregen
 *               2025 Dryw Wade
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

#include "py/mphal.h"
#include "fsl_flexspi.h"

#if MICROPY_HW_ENABLE_PSRAM

// These #defines are from here:
// https://github.com/PaulStoffregen/cores/blob/10025393e83ca9f4dc5646643a41cb2f32022ae4/teensy4/imxrt.h
#define CCM_CCGR_ON                             3
#define FLEXSPI_MCR2_CLRLEARNPHASE(x)           ((uint32_t)(x << 14))
#define FLEXSPI_LUTKEY_VALUE                    ((uint32_t)0x5AF05AF0)

// The rest of the implementation is adapted from here:
// https://github.com/PaulStoffregen/cores/blob/10025393e83ca9f4dc5646643a41cb2f32022ae4/teensy4/startup.c#L421-L615
static void flexspi2_command(uint32_t index, uint32_t addr) {
    FLEXSPI2->IPCR0 = addr;
    FLEXSPI2->IPCR1 = FLEXSPI_IPCR1_ISEQID(index);
    FLEXSPI2->IPCMD = FLEXSPI_IPCMD_TRG(1);
    while (!(FLEXSPI2->INTR & FLEXSPI_INTR_IPCMDDONE(1))) {
        ;                                                      // wait
    }
    FLEXSPI2->INTR = FLEXSPI_INTR_IPCMDDONE(1);
}

static uint32_t flexspi2_psram_id(uint32_t addr) {
    FLEXSPI2->IPCR0 = addr;
    FLEXSPI2->IPCR1 = FLEXSPI_IPCR1_ISEQID(3) | FLEXSPI_IPCR1_IDATSZ(4);
    FLEXSPI2->IPCMD = FLEXSPI_IPCMD_TRG(1);
    while (!(FLEXSPI2->INTR & FLEXSPI_INTR_IPCMDDONE(1))) {
        ;                                                      // wait
    }
    uint32_t id = FLEXSPI2->RFDR[0];
    FLEXSPI2->INTR = FLEXSPI_INTR_IPCMDDONE(1) | FLEXSPI_INTR_IPRXWA(1);
    return id;
}

/**
 * \return size of PSRAM in MBytes, or 0 if not present
 */
static uint8_t flexspi2_psram_size(uint32_t addr) {
    uint8_t result = 0;     // assume we don't have PSRAM at this address
    flexspi2_command(0, addr);     // exit quad mode
    flexspi2_command(1, addr);     // reset enable
    flexspi2_command(2, addr);     // reset (is this really necessary?)
    uint32_t id = flexspi2_psram_id(addr);

    switch (id & 0xFFFF)
    {
        default:
            break;

        case 0x5D0D:         // AP / Ipus / ESP / Lyontek
            result = 8;
            break;

        case 0x5D9D:         // ISSI
            switch ((id >> 21) & 0x7)             // get size (Datasheet Table 6.2)
            {
                case 0b011:
                    result = 8;
                    break;
                case 0b100:
                    result = 16;
                    break;
            }
            break;
    }

    return result;
}

size_t configure_external_ram() {
    // initialize pins
    IOMUXC->SW_PAD_CTL_PAD[22] = 0x1B0F9;     // 100K pullup, strong drive, max speed, hyst
    IOMUXC->SW_PAD_CTL_PAD[23] = 0x110F9;     // keeper, strong drive, max speed, hyst
    IOMUXC->SW_PAD_CTL_PAD[24] = 0x1B0F9;     // 100K pullup, strong drive, max speed, hyst
    IOMUXC->SW_PAD_CTL_PAD[25] = 0x100F9;     // strong drive, max speed, hyst
    IOMUXC->SW_PAD_CTL_PAD[26] = 0x170F9;     // 47K pullup, strong drive, max speed, hyst
    IOMUXC->SW_PAD_CTL_PAD[27] = 0x170F9;     // 47K pullup, strong drive, max speed, hyst
    IOMUXC->SW_PAD_CTL_PAD[28] = 0x170F9;     // 47K pullup, strong drive, max speed, hyst
    IOMUXC->SW_PAD_CTL_PAD[29] = 0x170F9;     // 47K pullup, strong drive, max speed, hyst

    IOMUXC->SW_MUX_CTL_PAD[22] = 8 | 0x10;     // ALT1 = FLEXSPI2_A_SS1_B (Flash)
    IOMUXC->SW_MUX_CTL_PAD[23] = 8 | 0x10;     // ALT1 = FLEXSPI2_A_DQS
    IOMUXC->SW_MUX_CTL_PAD[24] = 8 | 0x10;     // ALT1 = FLEXSPI2_A_SS0_B (RAM)
    IOMUXC->SW_MUX_CTL_PAD[25] = 8 | 0x10;     // ALT1 = FLEXSPI2_A_SCLK
    IOMUXC->SW_MUX_CTL_PAD[26] = 8 | 0x10;     // ALT1 = FLEXSPI2_A_DATA0
    IOMUXC->SW_MUX_CTL_PAD[27] = 8 | 0x10;     // ALT1 = FLEXSPI2_A_DATA1
    IOMUXC->SW_MUX_CTL_PAD[28] = 8 | 0x10;     // ALT1 = FLEXSPI2_A_DATA2
    IOMUXC->SW_MUX_CTL_PAD[29] = 8 | 0x10;     // ALT1 = FLEXSPI2_A_DATA3

    IOMUXC->SELECT_INPUT_1[kIOMUXC_FLEXSPI2_IPP_IND_DQS_FA_SELECT_INPUT] = 1;     // GPIO_EMC_23 for Mode: ALT8, pg 986
    IOMUXC->SELECT_INPUT_1[kIOMUXC_FLEXSPI2_IPP_IND_IO_FA_BIT0_SELECT_INPUT] = 1;     // GPIO_EMC_26 for Mode: ALT8
    IOMUXC->SELECT_INPUT_1[kIOMUXC_FLEXSPI2_IPP_IND_IO_FA_BIT1_SELECT_INPUT] = 1;     // GPIO_EMC_27 for Mode: ALT8
    IOMUXC->SELECT_INPUT_1[kIOMUXC_FLEXSPI2_IPP_IND_IO_FA_BIT2_SELECT_INPUT] = 1;     // GPIO_EMC_28 for Mode: ALT8
    IOMUXC->SELECT_INPUT_1[kIOMUXC_FLEXSPI2_IPP_IND_IO_FA_BIT3_SELECT_INPUT] = 1;     // GPIO_EMC_29 for Mode: ALT8
    IOMUXC->SELECT_INPUT_1[kIOMUXC_FLEXSPI2_IPP_IND_SCK_FA_SELECT_INPUT] = 1;     // GPIO_EMC_25 for Mode: ALT8

    // turn on clock  (QSPI flash & PSRAM chips usually spec max clock 100 to 133 MHz)
    CCM->CBCMR = (CCM->CBCMR & ~(CCM_CBCMR_FLEXSPI2_PODF_MASK | CCM_CBCMR_FLEXSPI2_CLK_SEL_MASK))
        // | CCM_CBCMR_FLEXSPI2_PODF(5) | CCM_CBCMR_FLEXSPI2_CLK_SEL(3); // 88.0 MHz
        // | CCM_CBCMR_FLEXSPI2_PODF(3) | CCM_CBCMR_FLEXSPI2_CLK_SEL(0); // 99.0 MHz
        // | CCM_CBCMR_FLEXSPI2_PODF(6) | CCM_CBCMR_FLEXSPI2_CLK_SEL(1); // 102.9 MHz
        | CCM_CBCMR_FLEXSPI2_PODF(4) | CCM_CBCMR_FLEXSPI2_CLK_SEL(3);         // 105.6 MHz
    // | CCM_CBCMR_FLEXSPI2_PODF(5) | CCM_CBCMR_FLEXSPI2_CLK_SEL(2); // 110.8 MHz
    // | CCM_CBCMR_FLEXSPI2_PODF(5) | CCM_CBCMR_FLEXSPI2_CLK_SEL(1); // 120.0 MHz
    // | CCM_CBCMR_FLEXSPI2_PODF(3) | CCM_CBCMR_FLEXSPI2_CLK_SEL(3); // 132.0 MHz
    // | CCM_CBCMR_FLEXSPI2_PODF(4) | CCM_CBCMR_FLEXSPI2_CLK_SEL(1); // 144.0 MHz
    // | CCM_CBCMR_FLEXSPI2_PODF(3) | CCM_CBCMR_FLEXSPI2_CLK_SEL(2); // 166.2 MHz
    // | CCM_CBCMR_FLEXSPI2_PODF(2) | CCM_CBCMR_FLEXSPI2_CLK_SEL(3); // 176.0 MHz
    CCM->CCGR7 |= CCM_CCGR7_CG1(CCM_CCGR_ON);

    FLEXSPI2->MCR0 |= FLEXSPI_MCR0_MDIS(1);
    FLEXSPI2->MCR0 = (FLEXSPI2->MCR0 & ~(FLEXSPI_MCR0_AHBGRANTWAIT_MASK
        | FLEXSPI_MCR0_IPGRANTWAIT_MASK | FLEXSPI_MCR0_SCKFREERUNEN(1)
        | FLEXSPI_MCR0_COMBINATIONEN(1) | FLEXSPI_MCR0_DOZEEN(1)
        | FLEXSPI_MCR0_HSEN(1) | FLEXSPI_MCR0_ATDFEN(1) | FLEXSPI_MCR0_ARDFEN(1)
        | FLEXSPI_MCR0_RXCLKSRC_MASK | FLEXSPI_MCR0_SWRESET(1)))
        | FLEXSPI_MCR0_AHBGRANTWAIT(0xFF) | FLEXSPI_MCR0_IPGRANTWAIT(0xFF)
        | FLEXSPI_MCR0_RXCLKSRC(1) | FLEXSPI_MCR0_MDIS(1);
    FLEXSPI2->MCR1 = FLEXSPI_MCR1_SEQWAIT(0xFFFF) | FLEXSPI_MCR1_AHBBUSWAIT(0xFFFF);
    FLEXSPI2->MCR2 = (FLEXSPI->MCR2 & ~(FLEXSPI_MCR2_RESUMEWAIT_MASK
        | FLEXSPI_MCR2_SCKBDIFFOPT(1) | FLEXSPI_MCR2_SAMEDEVICEEN(1)
        | FLEXSPI_MCR2_CLRLEARNPHASE(1) | FLEXSPI_MCR2_CLRAHBBUFOPT(1)))
        | FLEXSPI_MCR2_RESUMEWAIT(0x20) /*| FLEXSPI_MCR2_SAMEDEVICEEN*/;

    FLEXSPI2->AHBCR = FLEXSPI2->AHBCR & ~(FLEXSPI_AHBCR_READADDROPT(1) | FLEXSPI_AHBCR_PREFETCHEN(1)
        | FLEXSPI_AHBCR_BUFFERABLEEN(1) | FLEXSPI_AHBCR_CACHABLEEN(1));
    uint32_t mask = (FLEXSPI_AHBRXBUFCR0_PREFETCHEN(1) | FLEXSPI_AHBRXBUFCR0_PRIORITY_MASK
        | FLEXSPI_AHBRXBUFCR0_MSTRID_MASK | FLEXSPI_AHBRXBUFCR0_BUFSZ_MASK);
    FLEXSPI2->AHBRXBUFCR0[0] = (FLEXSPI2->AHBRXBUFCR0[0] & ~mask)
        | FLEXSPI_AHBRXBUFCR0_PREFETCHEN(1) | FLEXSPI_AHBRXBUFCR0_BUFSZ(64);
    FLEXSPI2->AHBRXBUFCR0[1] = (FLEXSPI2->AHBRXBUFCR0[0] & ~mask)
        | FLEXSPI_AHBRXBUFCR0_PREFETCHEN(1) | FLEXSPI_AHBRXBUFCR0_BUFSZ(64);
    FLEXSPI2->AHBRXBUFCR0[2] = mask;
    FLEXSPI2->AHBRXBUFCR0[3] = mask;

    // RX watermark = one 64 bit line
    FLEXSPI2->IPRXFCR = (FLEXSPI->IPRXFCR & 0xFFFFFFC0) | FLEXSPI_IPRXFCR_CLRIPRXF(1);
    // TX watermark = one 64 bit line
    FLEXSPI2->IPTXFCR = (FLEXSPI->IPTXFCR & 0xFFFFFFC0) | FLEXSPI_IPTXFCR_CLRIPTXF(1);

    FLEXSPI2->INTEN = 0;
    FLEXSPI2->FLSHCR1[0] = FLEXSPI_FLSHCR1_CSINTERVAL(0)
        | FLEXSPI_FLSHCR1_TCSH(1) | FLEXSPI_FLSHCR1_TCSS(1);
    FLEXSPI2->FLSHCR2[0] = FLEXSPI_FLSHCR2_AWRSEQID(6) | FLEXSPI_FLSHCR2_AWRSEQNUM(0)
        | FLEXSPI_FLSHCR2_ARDSEQID(5) | FLEXSPI_FLSHCR2_ARDSEQNUM(0);

    FLEXSPI2->FLSHCR1[1] = FLEXSPI_FLSHCR1_CSINTERVAL(0)
        | FLEXSPI_FLSHCR1_TCSH(1) | FLEXSPI_FLSHCR1_TCSS(1);
    FLEXSPI2->FLSHCR2[1] = FLEXSPI_FLSHCR2_AWRSEQID(6) | FLEXSPI_FLSHCR2_AWRSEQNUM(0)
        | FLEXSPI_FLSHCR2_ARDSEQID(5) | FLEXSPI_FLSHCR2_ARDSEQNUM(0);

    FLEXSPI2->MCR0 &= ~FLEXSPI_MCR0_MDIS(1);

    FLEXSPI2->LUTKEY = FLEXSPI_LUTKEY_VALUE;
    FLEXSPI2->LUTCR = FLEXSPI_LUTCR_UNLOCK(1);
    volatile uint32_t *luttable = &FLEXSPI2->LUT[0];
    for (int i = 0; i < 64; i++) { luttable[i] = 0;
    }
    FLEXSPI2->MCR0 |= FLEXSPI_MCR0_SWRESET(1);
    while (FLEXSPI2->MCR0 & FLEXSPI_MCR0_SWRESET(1)) {
        ;                                                  // wait

    }
    FLEXSPI2->LUTKEY = FLEXSPI_LUTKEY_VALUE;
    FLEXSPI2->LUTCR = FLEXSPI_LUTCR_UNLOCK(1);

    // cmd index 0 = exit QPI mode
    FLEXSPI2->LUT[0] = FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_4PAD, 0xF5, 0, 0, 0);
    // cmd index 1 = reset enable
    FLEXSPI2->LUT[4] = FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x66, 0, 0, 0);
    // cmd index 2 = reset
    FLEXSPI2->LUT[8] = FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x99, 0, 0, 0);
    // cmd index 3 = read ID bytes
    FLEXSPI2->LUT[12] = FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x9F, kFLEXSPI_Command_DUMMY_SDR, kFLEXSPI_1PAD, 24);
    FLEXSPI2->LUT[13] = FLEXSPI_LUT_SEQ(kFLEXSPI_Command_READ_SDR, kFLEXSPI_1PAD, 1, 0, 0, 0);
    // cmd index 4 = enter QPI mode
    FLEXSPI2->LUT[16] = FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x35, 0, 0, 0);
    // cmd index 5 = read QPI
    FLEXSPI2->LUT[20] = FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_4PAD, 0xEB, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_4PAD, 24);
    FLEXSPI2->LUT[21] = FLEXSPI_LUT_SEQ(kFLEXSPI_Command_DUMMY_SDR, kFLEXSPI_4PAD, 6, kFLEXSPI_Command_READ_SDR, kFLEXSPI_4PAD, 1);
    // cmd index 6 = write QPI
    FLEXSPI2->LUT[24] = FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_4PAD, 0x38, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_4PAD, 24);
    FLEXSPI2->LUT[25] = FLEXSPI_LUT_SEQ(kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_4PAD, 1, 0, 0, 0);

    // Detected PSRAM size in MB
    uint8_t external_psram_size = 0;

    // look for the first PSRAM chip
    uint8_t size1 = flexspi2_psram_size(0);
    if (size1 > 0) {
        FLEXSPI2->FLSHCR0[0] = size1 << 10;
        flexspi2_command(4, 0);         // enter QPI mode
        // look for a second PSRAM chip
        uint8_t size2 = flexspi2_psram_size(size1 << 20);
        external_psram_size = size1 + size2;
        if (size2 > 0) {
            FLEXSPI2->FLSHCR0[1] = size2 << 10;
            flexspi2_command(4, size1 << 20);              // enter QPI mode
        }
    } else {
        // No PSRAM
        external_psram_size = 0;
    }

    // Return the size of the PSRAM in bytes
    return external_psram_size * 0x100000;
}

#endif
