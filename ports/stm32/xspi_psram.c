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

#include <string.h>
#include "py/mphal.h"

#if MICROPY_HW_XSPI_PSRAM

#include "xspi_psram.h"

// Based on HCLK, which typically runs at 200MHz.
#ifndef MICROPY_HW_XSPI_PSRAM_PRESCALER
#define MICROPY_HW_XSPI_PSRAM_PRESCALER (1)
#endif

// Whether the PSRAM uses 1.8V I/O, otherwise 3.3V.  If enabled, the HSLV_VDDIO2 OTP
// fuse is programmed (if not already), which is permanent.
#ifndef MICROPY_HW_XSPI_PSRAM_1V8
#define MICROPY_HW_XSPI_PSRAM_1V8 (1)
#endif

// The type of PSRAM, one of XSPI_PSRAM_TYPE_xxx.
#ifndef MICROPY_HW_XSPI_PSRAM_TYPE
#define MICROPY_HW_XSPI_PSRAM_TYPE (XSPI_PSRAM_TYPE_APMEM_16BITS)
#endif

static XSPI_HandleTypeDef xspi_psram;

#if MICROPY_HW_XSPI_PSRAM_TYPE == XSPI_PSRAM_TYPE_APMEM_16BITS

#define XSPI_PSRAM_MEMTYPE HAL_XSPI_MEMTYPE_APMEM_16BITS
// Release CS at least this often so the PSRAM can refresh itself: tCEM is 4us for
// standard temperature parts (up to 85C) and 1us for extended ones (up to 105C).
#define XSPI_PSRAM_CS_LOW_US (1)

static const mp_hal_pin_obj_t xspi_psram_pin_table[] = {
    pyb_pin_XSPIM_P1_CS, pyb_pin_XSPIM_P1_DQS0, pyb_pin_XSPIM_P1_DQS1, pyb_pin_XSPIM_P1_SCK,
    pyb_pin_XSPIM_P1_IO0, pyb_pin_XSPIM_P1_IO1, pyb_pin_XSPIM_P1_IO2, pyb_pin_XSPIM_P1_IO3,
    pyb_pin_XSPIM_P1_IO4, pyb_pin_XSPIM_P1_IO5, pyb_pin_XSPIM_P1_IO6, pyb_pin_XSPIM_P1_IO7,
    pyb_pin_XSPIM_P1_IO8, pyb_pin_XSPIM_P1_IO9, pyb_pin_XSPIM_P1_IO10, pyb_pin_XSPIM_P1_IO11,
    pyb_pin_XSPIM_P1_IO12, pyb_pin_XSPIM_P1_IO13, pyb_pin_XSPIM_P1_IO14, pyb_pin_XSPIM_P1_IO15,
};

#define APS_READ_LINEAR_BURST (0x20)
#define APS_WRITE_LINEAR_BURST (0xa0)
#define APS_READ_REG (0x40)
#define APS_WRITE_REG (0xc0)
#define APS_LATENCY (7)
#define APS_VENDOR_APM (0x0d)

static void aps_cmd(XSPI_RegularCmdTypeDef *cmd, uint32_t optype, uint8_t instr, uint32_t addr, bool x16) {
    memset(cmd, 0, sizeof(*cmd));
    cmd->OperationType = optype;
    cmd->InstructionMode = HAL_XSPI_INSTRUCTION_8_LINES;
    cmd->InstructionWidth = HAL_XSPI_INSTRUCTION_8_BITS;
    cmd->InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
    cmd->Instruction = instr;
    cmd->AddressMode = HAL_XSPI_ADDRESS_8_LINES;
    cmd->AddressWidth = HAL_XSPI_ADDRESS_32_BITS;
    cmd->AddressDTRMode = HAL_XSPI_ADDRESS_DTR_ENABLE;
    cmd->Address = addr;
    cmd->AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
    cmd->DataMode = x16 ? HAL_XSPI_DATA_16_LINES : HAL_XSPI_DATA_8_LINES;
    cmd->DataDTRMode = HAL_XSPI_DATA_DTR_ENABLE;
}

static int aps_write_reg(uint32_t addr, uint8_t value) {
    // Even though we're writing a single byte, DDR transfers require an even
    // number of bytes; the second one is ignored.
    uint8_t buf[2] = {value, value};
    XSPI_RegularCmdTypeDef cmd;
    aps_cmd(&cmd, HAL_XSPI_OPTYPE_COMMON_CFG, APS_WRITE_REG, addr, false);
    cmd.DataLength = 2;
    cmd.DummyCycles = 0;
    cmd.DQSMode = HAL_XSPI_DQS_DISABLE;
    if (HAL_XSPI_Command(&xspi_psram, &cmd, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK
        || HAL_XSPI_Transmit(&xspi_psram, buf, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        return -1;
    }
    return 0;
}

static int aps_read_reg(uint32_t addr, uint8_t *value) {
    XSPI_RegularCmdTypeDef cmd;
    aps_cmd(&cmd, HAL_XSPI_OPTYPE_COMMON_CFG, APS_READ_REG, addr, false);
    cmd.DataLength = 2;
    cmd.DummyCycles = APS_LATENCY - 1;
    cmd.DQSMode = HAL_XSPI_DQS_ENABLE;
    if (HAL_XSPI_Command(&xspi_psram, &cmd, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK
        || HAL_XSPI_Receive(&xspi_psram, value, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        return -1;
    }
    return 0;
}

// Configure the PSRAM and set the memory-mapped read and write commands.  Returns its size
// in bytes, or 0 if it couldn't be configured or identified.
static size_t xspi_psram_chip_init(void) {
    // Read and write latency 7 (fixed), x16 mode, row boundary crossing for reads, and
    // 2K byte bursts.
    if (aps_write_reg(0, 0x30) != 0
        || aps_write_reg(4, 0x20) != 0
        || aps_write_reg(8, 0x4b) != 0) {
        return 0;
    }

    // Identify the memory from mode registers 1 (vendor) and 2 (generation, density).
    // Register reads return an aligned pair of registers, so read MR0/MR1 and MR2/MR3.
    uint8_t mr01[2] = {0};
    uint8_t mr23[2] = {0};
    if (aps_read_reg(0, mr01) != 0
        || aps_read_reg(2, mr23) != 0) {
        return 0;
    }
    if ((mr01[1] & 0x1f) != APS_VENDOR_APM) {
        return 0;
    }
    size_t size;
    switch (mr23[0] & 0x07) {
        case 5:
            size = 16 * 1024 * 1024;
            break;
        case 7:
            size = 32 * 1024 * 1024;
            break;
        case 6:
            size = 64 * 1024 * 1024;
            break;
        default:
            return 0;
    }

    // Read and write with linear bursts.
    XSPI_RegularCmdTypeDef cmd;
    aps_cmd(&cmd, HAL_XSPI_OPTYPE_WRITE_CFG, APS_WRITE_LINEAR_BURST, 0, true);
    cmd.DummyCycles = APS_LATENCY - 1;
    cmd.DQSMode = HAL_XSPI_DQS_ENABLE;
    if (HAL_XSPI_Command(&xspi_psram, &cmd, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        return 0;
    }
    cmd.OperationType = HAL_XSPI_OPTYPE_READ_CFG;
    cmd.Instruction = APS_READ_LINEAR_BURST;
    if (HAL_XSPI_Command(&xspi_psram, &cmd, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        return 0;
    }
    return size;
}

#else
#error "Unsupported MICROPY_HW_XSPI_PSRAM_TYPE"
#endif

static void xspi_psram_pins(void) {
    for (size_t i = 0; i < MP_ARRAY_SIZE(xspi_psram_pin_table); ++i) {
        mp_hal_pin_config(xspi_psram_pin_table[i], MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, GPIO_AF9_XSPIM_P1);
        mp_hal_pin_config_speed(xspi_psram_pin_table[i], MP_HAL_PIN_SPEED_VERY_HIGH);
    }
}

// Set up VDDIO2, which supplies the PSRAM's I/Os.  1.8V I/Os need VDDIO2 in its 1.8V range:
// in the 3.3V range they work in a degraded mode, too slow for the PSRAM's full speed.  The
// 1.8V range needs the HSLV_VDDIO2 fuse, so program it if it isn't set (this is permanent).
static bool xspi_psram_vddio2_init(void) {
    LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_PWR);
    #if MICROPY_HW_XSPI_PSRAM_1V8
    // OTP fuse word and bit that allow VDDIO2 to use its 1.8V I/O range (RM0486, Table 18).
    #define BSEC_HW_CONFIG_ID (124U)
    #define BSEC_HWS_HSLV_VDDIO2 (1U << 16)
    LL_APB4_GRP2_EnableClock(LL_APB4_GRP2_PERIPH_BSEC);
    BSEC_HandleTypeDef hbsec = { .Instance = BSEC };
    uint32_t fuse;
    if (HAL_BSEC_OTP_Read(&hbsec, BSEC_HW_CONFIG_ID, &fuse) != HAL_OK) {
        return false;
    }
    if (!(fuse & BSEC_HWS_HSLV_VDDIO2)) {
        // Fuse not set yet, program it to enable 1.8V I/Os for VDDIO2.
        if (HAL_BSEC_OTP_Program(&hbsec, BSEC_HW_CONFIG_ID, fuse | BSEC_HWS_HSLV_VDDIO2, HAL_BSEC_NORMAL_PROG) != HAL_OK
            || HAL_BSEC_OTP_Read(&hbsec, BSEC_HW_CONFIG_ID, &fuse) != HAL_OK
            || !(fuse & BSEC_HWS_HSLV_VDDIO2)) {
            return false;
        }
    }
    #endif
    LL_PWR_EnableVddIO2();
    #if MICROPY_HW_XSPI_PSRAM_1V8
    LL_PWR_SetVddIO2VoltageRange(LL_PWR_VDDIO_VOLTAGE_RANGE_1V8);
    #else
    LL_PWR_SetVddIO2VoltageRange(LL_PWR_VDDIO_VOLTAGE_RANGE_3V3);
    #endif
    return true;
}

size_t xspi_psram_init(void) {
    if (!xspi_psram_vddio2_init()) {
        return 0;
    }

    // The XSPIM is set up in direct mode by xspi_init(), so XSPI1 drives port 1.
    LL_RCC_SetXSPIClockSource(LL_RCC_XSPI1_CLKSOURCE_HCLK);
    LL_AHB5_GRP1_EnableClock(LL_AHB5_GRP1_PERIPH_XSPI1);
    xspi_psram_pins();

    xspi_psram.Instance = XSPI1;
    xspi_psram.Init.FifoThresholdByte = 8;
    xspi_psram.Init.MemoryType = XSPI_PSRAM_MEMTYPE;
    xspi_psram.Init.MemoryMode = HAL_XSPI_SINGLE_MEM;
    xspi_psram.Init.MemorySize = HAL_XSPI_SIZE_256MB; // updated once the size is known
    xspi_psram.Init.MemorySelect = HAL_XSPI_CSSEL_NCS1;
    xspi_psram.Init.ChipSelectHighTimeCycle = 5;
    xspi_psram.Init.ClockMode = HAL_XSPI_CLOCK_MODE_0;
    xspi_psram.Init.ClockPrescaler = MICROPY_HW_XSPI_PSRAM_PRESCALER - 1;
    xspi_psram.Init.SampleShifting = HAL_XSPI_SAMPLE_SHIFT_NONE;
    xspi_psram.Init.DelayHoldQuarterCycle = HAL_XSPI_DHQC_DISABLE;
    xspi_psram.Init.ChipSelectBoundary = HAL_XSPI_BONDARYOF_16KB;
    xspi_psram.Init.FreeRunningClock = HAL_XSPI_FREERUNCLK_DISABLE;
    xspi_psram.Init.WrapSize = HAL_XSPI_WRAP_NOT_SUPPORTED;
    // Release CS often enough for the PSRAM to refresh itself, in XSPI clock cycles.
    // CS is released every Refresh + 4 cycles for reads, Refresh + 1 for writes
    // (RM0486, XSPI_DCR4), so subtract 4.
    uint32_t clk_mhz = HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_XSPI1) / MICROPY_HW_XSPI_PSRAM_PRESCALER / 1000000;
    if (clk_mhz == 0) {
        return 0;
    }
    xspi_psram.Init.Refresh = XSPI_PSRAM_CS_LOW_US * clk_mhz - 4;
    if (HAL_XSPI_Init(&xspi_psram) != HAL_OK) {
        return 0;
    }

    size_t size = xspi_psram_chip_init();
    if (size == 0) {
        return 0;
    }

    // Set the device size to match the memory (2^(DEVSIZE + 1) bytes), then map it.
    MODIFY_REG(xspi_psram.Instance->DCR1, XSPI_DCR1_DEVSIZE, (__builtin_ctz(size) - 1) << XSPI_DCR1_DEVSIZE_Pos);
    // Release CS when memory-mapped accesses stop.
    XSPI_MemoryMappedTypeDef mm = {
        .TimeOutActivation = HAL_XSPI_TIMEOUT_COUNTER_ENABLE,
        .TimeoutPeriodClock = 0x34,
    };
    if (HAL_XSPI_MemoryMapped(&xspi_psram, &mm) != HAL_OK) {
        return 0;
    }
    return size;
}

#endif // MICROPY_HW_XSPI_PSRAM
