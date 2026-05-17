/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Paul Stoffregen
 *               2026 Dryw Wade
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

// This implementation is adapted from here:
// https://github.com/PaulStoffregen/cores/blob/10025393e83ca9f4dc5646643a41cb2f32022ae4/teensy4/startup.c#L421-L615
#include "py/mphal.h"
#include "fsl_flexspi.h"

#if MICROPY_HW_ENABLE_PSRAM

/*!
 * @brief Clock divider value.
 *
 * See https://www.pjrc.com/teensy/IMXRT1060RM_rev3_annotations.pdf (p1010 and p1050)
 */
typedef enum _clock_mux_value {
    kCLOCK_Flexspi2Mux_396MHz = 0U, /*!< FLEXSPI2 clock source is PLL2 PFD2. */
    kCLOCK_Flexspi2Mux_720MHz = 1U, /*!< FLEXSPI2 clock source is PLL3 PFD0. */
    kCLOCK_Flexspi2Mux_664_62MHz = 2U, /*!< FLEXSPI2 clock source is PLL3 PFD1. */
    kCLOCK_Flexspi2Mux_528MHz = 3U, /*!< FLEXSPI2 clock source is PLL2 (pll2_main_clk). */
} clock_mux_value_t;

static void flexspi2_command(uint32_t index, uint32_t addr, flexspi_port_t port) {
    flexspi_transfer_t xfer = {
        .deviceAddress = addr,
        .port = port,
        .cmdType = kFLEXSPI_Command,
        .seqIndex = index,
        .SeqNumber = 1,
        .data = NULL,
        .dataSize = 0,
    };
    FLEXSPI_TransferBlocking(FLEXSPI2, &xfer);
    FLEXSPI_ClearInterruptStatusFlags(FLEXSPI2, kFLEXSPI_IpCommandExecutionDoneFlag);
}

static uint32_t flexspi2_psram_id(uint32_t addr, flexspi_port_t port) {
    uint32_t id = 0;
    flexspi_transfer_t xfer = {
        .deviceAddress = addr,
        .port = port,
        .cmdType = kFLEXSPI_Read,
        .seqIndex = 3,
        .SeqNumber = 1,
        .data = &id,
        .dataSize = 4,
    };
    FLEXSPI_TransferBlocking(FLEXSPI2, &xfer);
    FLEXSPI_ClearInterruptStatusFlags(FLEXSPI2,
        kFLEXSPI_IpCommandExecutionDoneFlag | kFLEXSPI_IpRxFifoWatermarkAvailableFlag);
    return id;
}

/**
 * \return size of PSRAM in MBytes, or 0 if not present
 */
static uint8_t flexspi2_psram_size(uint32_t addr, flexspi_port_t port) {
    uint8_t result = 0;     // assume we don't have PSRAM at this address
    flexspi2_command(0, addr, port);     // exit quad mode
    flexspi2_command(1, addr, port);     // reset enable
    flexspi2_command(2, addr, port);     // reset (is this really necessary?)
    uint32_t id = flexspi2_psram_id(addr, port);

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
    IOMUXC->SW_PAD_CTL_PAD[kIOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_22] = 0x1B0F9;     // 100K pullup, strong drive, max speed, hyst
    IOMUXC->SW_PAD_CTL_PAD[kIOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_23] = 0x110F9;     // keeper, strong drive, max speed, hyst
    IOMUXC->SW_PAD_CTL_PAD[kIOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_24] = 0x1B0F9;     // 100K pullup, strong drive, max speed, hyst
    IOMUXC->SW_PAD_CTL_PAD[kIOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_25] = 0x100F9;     // strong drive, max speed, hyst
    IOMUXC->SW_PAD_CTL_PAD[kIOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_26] = 0x170F9;     // 47K pullup, strong drive, max speed, hyst
    IOMUXC->SW_PAD_CTL_PAD[kIOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_27] = 0x170F9;     // 47K pullup, strong drive, max speed, hyst
    IOMUXC->SW_PAD_CTL_PAD[kIOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_28] = 0x170F9;     // 47K pullup, strong drive, max speed, hyst
    IOMUXC->SW_PAD_CTL_PAD[kIOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_29] = 0x170F9;     // 47K pullup, strong drive, max speed, hyst

    IOMUXC->SW_MUX_CTL_PAD[kIOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_22] = 8 | 0x10;     // ALT1 = FLEXSPI2_A_SS1_B (Flash)
    IOMUXC->SW_MUX_CTL_PAD[kIOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_23] = 8 | 0x10;     // ALT1 = FLEXSPI2_A_DQS
    IOMUXC->SW_MUX_CTL_PAD[kIOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_24] = 8 | 0x10;     // ALT1 = FLEXSPI2_A_SS0_B (RAM)
    IOMUXC->SW_MUX_CTL_PAD[kIOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_25] = 8 | 0x10;     // ALT1 = FLEXSPI2_A_SCLK
    IOMUXC->SW_MUX_CTL_PAD[kIOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_26] = 8 | 0x10;     // ALT1 = FLEXSPI2_A_DATA0
    IOMUXC->SW_MUX_CTL_PAD[kIOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_27] = 8 | 0x10;     // ALT1 = FLEXSPI2_A_DATA1
    IOMUXC->SW_MUX_CTL_PAD[kIOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_28] = 8 | 0x10;     // ALT1 = FLEXSPI2_A_DATA2
    IOMUXC->SW_MUX_CTL_PAD[kIOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_29] = 8 | 0x10;     // ALT1 = FLEXSPI2_A_DATA3

    IOMUXC->SELECT_INPUT_1[kIOMUXC_FLEXSPI2_IPP_IND_DQS_FA_SELECT_INPUT] = 1;     // GPIO_EMC_23 for Mode: ALT8, pg 986
    IOMUXC->SELECT_INPUT_1[kIOMUXC_FLEXSPI2_IPP_IND_IO_FA_BIT0_SELECT_INPUT] = 1;     // GPIO_EMC_26 for Mode: ALT8
    IOMUXC->SELECT_INPUT_1[kIOMUXC_FLEXSPI2_IPP_IND_IO_FA_BIT1_SELECT_INPUT] = 1;     // GPIO_EMC_27 for Mode: ALT8
    IOMUXC->SELECT_INPUT_1[kIOMUXC_FLEXSPI2_IPP_IND_IO_FA_BIT2_SELECT_INPUT] = 1;     // GPIO_EMC_28 for Mode: ALT8
    IOMUXC->SELECT_INPUT_1[kIOMUXC_FLEXSPI2_IPP_IND_IO_FA_BIT3_SELECT_INPUT] = 1;     // GPIO_EMC_29 for Mode: ALT8
    IOMUXC->SELECT_INPUT_1[kIOMUXC_FLEXSPI2_IPP_IND_SCK_FA_SELECT_INPUT] = 1;     // GPIO_EMC_25 for Mode: ALT8

    // turn on clock  (QSPI flash & PSRAM chips usually spec max clock 100 to 133 MHz)
    // CLOCK_SetDiv(kCLOCK_Flexspi2Div, kCLOCK_Flexspi2DivBy6); // 88.0 MHz
    // CLOCK_SetMux(kCLOCK_Flexspi2Mux, kCLOCK_Flexspi2Mux_528MHz); // 88.0 MHz
    // CLOCK_SetDiv(kCLOCK_Flexspi2Div, kCLOCK_Flexspi2DivBy4); // 99.0 MHz
    // CLOCK_SetMux(kCLOCK_Flexspi2Mux, kCLOCK_Flexspi2Mux_396MHz); // 99.0 MHz
    // CLOCK_SetDiv(kCLOCK_Flexspi2Div, kCLOCK_Flexspi2DivBy7); // 102.9 MHz
    // CLOCK_SetMux(kCLOCK_Flexspi2Mux, kCLOCK_Flexspi2Mux_720MHz); // 102.9 MHz
    CLOCK_SetDiv(kCLOCK_Flexspi2Div, kCLOCK_Flexspi2DivBy5); // 105.6 MHz
    CLOCK_SetMux(kCLOCK_Flexspi2Mux, kCLOCK_Flexspi2Mux_528MHz); // 105.6 MHz
    // CLOCK_SetDiv(kCLOCK_Flexspi2Div, kCLOCK_Flexspi2DivBy6); // 110.8 MHz
    // CLOCK_SetMux(kCLOCK_Flexspi2Mux, kCLOCK_Flexspi2Mux_664_62MHz); // 110.8 MHz
    // CLOCK_SetDiv(kCLOCK_Flexspi2Div, kCLOCK_Flexspi2DivBy6); // 120.0 MHz
    // CLOCK_SetMux(kCLOCK_Flexspi2Mux, kCLOCK_Flexspi2Mux_720MHz); // 120.0 MHz
    // CLOCK_SetDiv(kCLOCK_Flexspi2Div, kCLOCK_Flexspi2DivBy4); // 132.0 MHz
    // CLOCK_SetMux(kCLOCK_Flexspi2Mux, kCLOCK_Flexspi2Mux_528MHz); // 132.0 MHz
    // CLOCK_SetDiv(kCLOCK_Flexspi2Div, kCLOCK_Flexspi2DivBy5); // 144.0 MHz
    // CLOCK_SetMux(kCLOCK_Flexspi2Mux, kCLOCK_Flexspi2Mux_720MHz); // 144.0 MHz
    // CLOCK_SetDiv(kCLOCK_Flexspi2Div, kCLOCK_Flexspi2DivBy4); // 166.2 MHz
    // CLOCK_SetMux(kCLOCK_Flexspi2Mux, kCLOCK_Flexspi2Mux_664_62MHz); // 166.2 MHz
    // CLOCK_SetDiv(kCLOCK_Flexspi2Div, kCLOCK_Flexspi2DivBy3); // 176.0 MHz
    // CLOCK_SetMux(kCLOCK_Flexspi2Mux, kCLOCK_Flexspi2Mux_528MHz); // 176.0 MHz

    flexspi_config_t flexspi_config = {
        .rxSampleClock = kFLEXSPI_ReadSampleClkLoopbackFromDqsPad,
        .enableSckFreeRunning = false,
        .enableCombination = false,
        .enableDoze = false,
        .enableHalfSpeedAccess = false,
        .enableSckBDiffOpt = false,
        .enableSameConfigForAll = false,
        .seqTimeoutCycle = 0xFFFF,
        .ipGrantTimeoutCycle = 0xFF,
        .txWatermark = 0,
        .rxWatermark = 0,
        .ahbConfig = {
            .enableAHBWriteIpTxFifo = false,
            .enableAHBWriteIpRxFifo = false,
            .ahbGrantTimeoutCycle = 0xFF,
            .ahbBusTimeoutCycle = 0xFFFF,
            .resumeWaitCycle = 0x20,
            .buffer = {
                {.priority = 0, .masterIndex = 0, .bufferSize = 512, .enablePrefetch = true},
                {.priority = 0, .masterIndex = 0, .bufferSize = 512, .enablePrefetch = true},
                {.priority = 0, .masterIndex = 0, .bufferSize = 0, .enablePrefetch = false},
                {.priority = 0, .masterIndex = 0, .bufferSize = 0, .enablePrefetch = false},
            },
            .enableClearAHBBufferOpt = false,
            .enableReadAddressOpt = false,
            .enableAHBPrefetch = false,
            .enableAHBBufferable = false,
            .enableAHBCachable = false,
        },
    };
    FLEXSPI_Init(FLEXSPI2, &flexspi_config);

    FLEXSPI_DisableInterrupts(FLEXSPI2, kFLEXSPI_AllInterruptFlags);

    flexspi_device_config_t flexspi_device_config = {
        .flexspiRootClk = 0,
        .isSck2Enabled = false,
        .flashSize = 1 << 16, // Default value, will be updated later
            .CSIntervalUnit = kFLEXSPI_CsIntervalUnit1SckCycle,
            .CSInterval = 0,
            .CSHoldTime = 1,
            .CSSetupTime = 1,
            .dataValidTime = 0,
            .columnspace = 0,
            .enableWordAddress = false,
            .AWRSeqIndex = 6,
            .AWRSeqNumber = 1,
            .ARDSeqIndex = 5,
            .ARDSeqNumber = 1,
            .AHBWriteWaitUnit = kFLEXSPI_AhbWriteWaitUnit2AhbCycle,
            .AHBWriteWaitInterval = 0,
            .enableWriteMask = false,
    };
    FLEXSPI_SetFlashConfig(FLEXSPI2, &flexspi_device_config, kFLEXSPI_PortA1);
    FLEXSPI_SetFlashConfig(FLEXSPI2, &flexspi_device_config, kFLEXSPI_PortA2);

    uint32_t cmd[64] = {0};
    // cmd index 0 = exit QPI mode
    cmd[0] = FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_4PAD, 0xF5, 0, 0, 0);
    // cmd index 1 = reset enable
    cmd[4] = FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x66, 0, 0, 0);
    // cmd index 2 = reset
    cmd[8] = FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x99, 0, 0, 0);
    // cmd index 3 = read ID bytes
    cmd[12] = FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x9F, kFLEXSPI_Command_DUMMY_SDR, kFLEXSPI_1PAD, 24);
    cmd[13] = FLEXSPI_LUT_SEQ(kFLEXSPI_Command_READ_SDR, kFLEXSPI_1PAD, 1, 0, 0, 0);
    // cmd index 4 = enter QPI mode
    cmd[16] = FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x35, 0, 0, 0);
    // cmd index 5 = read QPI
    cmd[20] = FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_4PAD, 0xEB, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_4PAD, 24);
    cmd[21] = FLEXSPI_LUT_SEQ(kFLEXSPI_Command_DUMMY_SDR, kFLEXSPI_4PAD, 6, kFLEXSPI_Command_READ_SDR, kFLEXSPI_4PAD, 1);
    // cmd index 6 = write QPI
    cmd[24] = FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_4PAD, 0x38, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_4PAD, 24);
    cmd[25] = FLEXSPI_LUT_SEQ(kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_4PAD, 1, 0, 0, 0);
    FLEXSPI_UpdateLUT(FLEXSPI2, 0, cmd, 64);

    // Detected PSRAM size in MB
    uint8_t external_psram_size = 0;

    // look for the first PSRAM chip
    uint8_t size1 = flexspi2_psram_size(0, kFLEXSPI_PortA1);
    if (size1 > 0) {
        flexspi_device_config.flashSize = size1 << 10;
        FLEXSPI_SetFlashConfig(FLEXSPI2, &flexspi_device_config, kFLEXSPI_PortA1);
        flexspi2_command(4, 0, kFLEXSPI_PortA1);         // enter QPI mode
        // look for a second PSRAM chip
        uint8_t size2 = flexspi2_psram_size(size1 << 20, kFLEXSPI_PortA2);
        external_psram_size = size1 + size2;
        if (size2 > 0) {
            flexspi_device_config.flashSize = size2 << 10;
            FLEXSPI_SetFlashConfig(FLEXSPI2, &flexspi_device_config, kFLEXSPI_PortA2);
            flexspi2_command(4, size1 << 20, kFLEXSPI_PortA2);              // enter QPI mode
        }
    } else {
        // No PSRAM
        external_psram_size = 0;
    }

    // Return the size of the PSRAM in bytes
    return external_psram_size * 0x100000;
}

#endif
