// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

// This file defines board specific functions.

#pragma once

#include <stdbool.h>

#include "mpconfigboard.h"  // For flash size settings

#include "fsl_common.h"
#ifdef CPU_MIMXRT1011DAE5A
// TODO: Remove this when the 1011 has a romapi header that matches the others.
#include "sdk/boards/evkmimxrt1010/xip/evkmimxrt1010_flexspi_nor_config.h"
typedef enum _flexspi_serial_clk_freq_caps
{
    kFLEXSPISerialClk_NoChange = 0U,
    kFLEXSPISerialClk_30MHz    = 1U,
    kFLEXSPISerialClk_50MHz    = 2U,
    kFLEXSPISerialClk_60MHz    = 3U,
    kFLEXSPISerialClk_75MHz    = 4U,
    kFLEXSPISerialClk_80MHz    = 5U,
    kFLEXSPISerialClk_100MHz   = 6U,
    kFLEXSPISerialClk_133MHz   = 7U,
    kFLEXSPISerialClk_166MHz   = 8U,
    kFLEXSPISerialClk_200MHz   = 9U,
} caps_flexspi_serial_clk_freq_t;

/*! @brief FLEXSPI Read Sample Clock Source definition */
typedef enum _flexspi_read_sample_clk_caps
{
    kFLEXSPIReadSampleClk_LoopbackInternally      = 0U,
    kFLEXSPIReadSampleClk_LoopbackFromDqsPad      = 1U,
    kFLEXSPIReadSampleClk_LoopbackFromSckPad      = 2U,
    kFLEXSPIReadSampleClk_ExternalInputFromDqsPad = 3U,
} caps_flexspi_read_sample_clk_t;

enum
{
    kFLEXSPIDeviceType_SerialNOR = 1U, /*!< Flash device is Serial NOR */
};

#define FSL_ROM_FLEXSPI_LUT_SEQ FLEXSPI_LUT_SEQ
#else
#include "fsl_romapi.h"
#endif

#define SEQUENCE(first, second, third, fourth) first, second, third, fourth
#define TWO_EMPTY_STEPS 0x00000000
#define EMPTY_SEQUENCE SEQUENCE(TWO_EMPTY_STEPS, TWO_EMPTY_STEPS, TWO_EMPTY_STEPS, TWO_EMPTY_STEPS)

// FlexSPI configuration that stores command info.
extern const flexspi_nor_config_t qspiflash_config;
