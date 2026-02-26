/*******************************************************************************
 * File Name: cycfg_qspi_memslot.c
 *
 * Description:
 * Provides definitions of the SMIF-driver memory configuration.
 * This file was automatically generated and should not be modified.
 * QSPI Configurator 4.60.0.2742
 *
 *******************************************************************************
 * Copyright 2026 Cypress Semiconductor Corporation (an Infineon company) or
 * an affiliate of Cypress Semiconductor Corporation.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ******************************************************************************/

#include "cycfg_qspi_memslot.h"

cy_stc_smif_mem_cmd_t S25HS512T_SMIF0_SlaveSlot_1_readCmd =
{
    /* The 8-bit command. 1 x I/O read command. */
    .command = 0xECU,
    /* The width of the command transfer. */
    .cmdWidth = CY_SMIF_WIDTH_SINGLE,
    /* The width of the address transfer. */
    .addrWidth = CY_SMIF_WIDTH_QUAD,
    /* The 8-bit mode byte. This value is 0xFFFFFFFF when there is no mode present. */
    .mode = 0x01U,
    /* The width of the mode command transfer. */
    .modeWidth = CY_SMIF_WIDTH_QUAD,
    /* The number of dummy cycles. A zero value suggests no dummy cycles. */
    .dummyCycles = 8U,
    /* The width of the data transfer. */
    .dataWidth = CY_SMIF_WIDTH_QUAD,
#if (CY_IP_MXSMIF_VERSION >= 2)
    /* The Data rate of data */
    .dataRate = CY_SMIF_SDR,
    /* This specifies the presence of the dummy field */
    .dummyCyclesPresence = CY_SMIF_PRESENT_1BYTE,
    /* This specifies the presence of the mode field */
    .modePresence = CY_SMIF_PRESENT_1BYTE,
    /* The high byte of a 16-bit mode. This value is 0x0 when there is no higher byte mode present */
    .modeH = 0x00,
    /* The Data rate of mode */
    .modeRate = CY_SMIF_SDR,
    /* The Data rate of address */
    .addrRate = CY_SMIF_SDR,
    /* This specifies the width of the command field */
    .cmdPresence = CY_SMIF_PRESENT_1BYTE,
    /* The high byte of a 16-bit command. This value is 0x0 when there is no higher byte command present */
    .commandH = 0x00,
    /* The Data rate of command */
    .cmdRate = CY_SMIF_SDR,
#endif /* CY_IP_MXSMIF_VERSION */
};

cy_stc_smif_mem_cmd_t S25HS512T_SMIF0_SlaveSlot_1_writeEnCmd =
{
    /* The 8-bit command. 1 x I/O read command. */
    .command = 0x06U,
    /* The width of the command transfer. */
    .cmdWidth = CY_SMIF_WIDTH_SINGLE,
    /* The width of the address transfer. */
    .addrWidth = CY_SMIF_WIDTH_SINGLE,
    /* The 8-bit mode byte. This value is 0xFFFFFFFF when there is no mode present. */
    .mode = 0xFFFFFFFFU,
    /* The width of the mode command transfer. */
    .modeWidth = CY_SMIF_WIDTH_SINGLE,
    /* The number of dummy cycles. A zero value suggests no dummy cycles. */
    .dummyCycles = 0U,
    /* The width of the data transfer. */
    .dataWidth = CY_SMIF_WIDTH_SINGLE,
#if (CY_IP_MXSMIF_VERSION >= 2)
    /* The Data rate of data */
    .dataRate = CY_SMIF_SDR,
    /* This specifies the presence of the dummy field */
    .dummyCyclesPresence = CY_SMIF_NOT_PRESENT,
    /* This specifies the presence of the mode field */
    .modePresence = CY_SMIF_NOT_PRESENT,
    /* The high byte of a 16-bit mode. This value is 0x0 when there is no higher byte mode present */
    .modeH = 0x00,
    /* The Data rate of mode */
    .modeRate = CY_SMIF_SDR,
    /* The Data rate of address */
    .addrRate = CY_SMIF_SDR,
    /* This specifies the width of the command field */
    .cmdPresence = CY_SMIF_PRESENT_1BYTE,
    /* The high byte of a 16-bit command. This value is 0x0 when there is no higher byte command present */
    .commandH = 0x00,
    /* The Data rate of command */
    .cmdRate = CY_SMIF_SDR,
#endif /* CY_IP_MXSMIF_VERSION */
};

cy_stc_smif_mem_cmd_t S25HS512T_SMIF0_SlaveSlot_1_writeDisCmd =
{
    /* The 8-bit command. 1 x I/O read command. */
    .command = 0x04U,
    /* The width of the command transfer. */
    .cmdWidth = CY_SMIF_WIDTH_SINGLE,
    /* The width of the address transfer. */
    .addrWidth = CY_SMIF_WIDTH_SINGLE,
    /* The 8-bit mode byte. This value is 0xFFFFFFFF when there is no mode present. */
    .mode = 0xFFFFFFFFU,
    /* The width of the mode command transfer. */
    .modeWidth = CY_SMIF_WIDTH_SINGLE,
    /* The number of dummy cycles. A zero value suggests no dummy cycles. */
    .dummyCycles = 0U,
    /* The width of the data transfer. */
    .dataWidth = CY_SMIF_WIDTH_SINGLE,
#if (CY_IP_MXSMIF_VERSION >= 2)
    /* The Data rate of data */
    .dataRate = CY_SMIF_SDR,
    /* This specifies the presence of the dummy field */
    .dummyCyclesPresence = CY_SMIF_NOT_PRESENT,
    /* This specifies the presence of the mode field */
    .modePresence = CY_SMIF_NOT_PRESENT,
    /* The high byte of a 16-bit mode. This value is 0x0 when there is no higher byte mode present */
    .modeH = 0x00,
    /* The Data rate of mode */
    .modeRate = CY_SMIF_SDR,
    /* The Data rate of address */
    .addrRate = CY_SMIF_SDR,
    /* This specifies the width of the command field */
    .cmdPresence = CY_SMIF_PRESENT_1BYTE,
    /* The high byte of a 16-bit command. This value is 0x0 when there is no higher byte command present */
    .commandH = 0x00,
    /* The Data rate of command */
    .cmdRate = CY_SMIF_SDR,
#endif /* CY_IP_MXSMIF_VERSION */
};

cy_stc_smif_mem_cmd_t S25HS512T_SMIF0_SlaveSlot_1_eraseCmd =
{
    /* The 8-bit command. 1 x I/O read command. */
    .command = 0xDCU,
    /* The width of the command transfer. */
    .cmdWidth = CY_SMIF_WIDTH_SINGLE,
    /* The width of the address transfer. */
    .addrWidth = CY_SMIF_WIDTH_SINGLE,
    /* The 8-bit mode byte. This value is 0xFFFFFFFF when there is no mode present. */
    .mode = 0xFFFFFFFFU,
    /* The width of the mode command transfer. */
    .modeWidth = CY_SMIF_WIDTH_SINGLE,
    /* The number of dummy cycles. A zero value suggests no dummy cycles. */
    .dummyCycles = 0U,
    /* The width of the data transfer. */
    .dataWidth = CY_SMIF_WIDTH_SINGLE,
#if (CY_IP_MXSMIF_VERSION >= 2)
    /* The Data rate of data */
    .dataRate = CY_SMIF_SDR,
    /* This specifies the presence of the dummy field */
    .dummyCyclesPresence = CY_SMIF_NOT_PRESENT,
    /* This specifies the presence of the mode field */
    .modePresence = CY_SMIF_NOT_PRESENT,
    /* The high byte of a 16-bit mode. This value is 0x0 when there is no higher byte mode present */
    .modeH = 0x00,
    /* The Data rate of mode */
    .modeRate = CY_SMIF_SDR,
    /* The Data rate of address */
    .addrRate = CY_SMIF_SDR,
    /* This specifies the width of the command field */
    .cmdPresence = CY_SMIF_PRESENT_1BYTE,
    /* The high byte of a 16-bit command. This value is 0x0 when there is no higher byte command present */
    .commandH = 0x00,
    /* The Data rate of command */
    .cmdRate = CY_SMIF_SDR,
#endif /* CY_IP_MXSMIF_VERSION */
};

cy_stc_smif_mem_cmd_t S25HS512T_SMIF0_SlaveSlot_1_chipEraseCmd =
{
    /* The 8-bit command. 1 x I/O read command. */
    .command = 0x60U,
    /* The width of the command transfer. */
    .cmdWidth = CY_SMIF_WIDTH_SINGLE,
    /* The width of the address transfer. */
    .addrWidth = CY_SMIF_WIDTH_SINGLE,
    /* The 8-bit mode byte. This value is 0xFFFFFFFF when there is no mode present. */
    .mode = 0xFFFFFFFFU,
    /* The width of the mode command transfer. */
    .modeWidth = CY_SMIF_WIDTH_SINGLE,
    /* The number of dummy cycles. A zero value suggests no dummy cycles. */
    .dummyCycles = 0U,
    /* The width of the data transfer. */
    .dataWidth = CY_SMIF_WIDTH_SINGLE,
#if (CY_IP_MXSMIF_VERSION >= 2)
    /* The Data rate of data */
    .dataRate = CY_SMIF_SDR,
    /* This specifies the presence of the dummy field */
    .dummyCyclesPresence = CY_SMIF_NOT_PRESENT,
    /* This specifies the presence of the mode field */
    .modePresence = CY_SMIF_NOT_PRESENT,
    /* The high byte of a 16-bit mode. This value is 0x0 when there is no higher byte mode present */
    .modeH = 0x00,
    /* The Data rate of mode */
    .modeRate = CY_SMIF_SDR,
    /* The Data rate of address */
    .addrRate = CY_SMIF_SDR,
    /* This specifies the width of the command field */
    .cmdPresence = CY_SMIF_PRESENT_1BYTE,
    /* The high byte of a 16-bit command. This value is 0x0 when there is no higher byte command present */
    .commandH = 0x00,
    /* The Data rate of command */
    .cmdRate = CY_SMIF_SDR,
#endif /* CY_IP_MXSMIF_VERSION */
};

cy_stc_smif_mem_cmd_t S25HS512T_SMIF0_SlaveSlot_1_programCmd =
{
    /* The 8-bit command. 1 x I/O read command. */
    .command = 0x12U,
    /* The width of the command transfer. */
    .cmdWidth = CY_SMIF_WIDTH_SINGLE,
    /* The width of the address transfer. */
    .addrWidth = CY_SMIF_WIDTH_SINGLE,
    /* The 8-bit mode byte. This value is 0xFFFFFFFF when there is no mode present. */
    .mode = 0xFFFFFFFFU,
    /* The width of the mode command transfer. */
    .modeWidth = CY_SMIF_WIDTH_SINGLE,
    /* The number of dummy cycles. A zero value suggests no dummy cycles. */
    .dummyCycles = 0U,
    /* The width of the data transfer. */
    .dataWidth = CY_SMIF_WIDTH_SINGLE,
#if (CY_IP_MXSMIF_VERSION >= 2)
    /* The Data rate of data */
    .dataRate = CY_SMIF_SDR,
    /* This specifies the presence of the dummy field */
    .dummyCyclesPresence = CY_SMIF_NOT_PRESENT,
    /* This specifies the presence of the mode field */
    .modePresence = CY_SMIF_NOT_PRESENT,
    /* The high byte of a 16-bit mode. This value is 0x0 when there is no higher byte mode present */
    .modeH = 0x00,
    /* The Data rate of mode */
    .modeRate = CY_SMIF_SDR,
    /* The Data rate of address */
    .addrRate = CY_SMIF_SDR,
    /* This specifies the width of the command field */
    .cmdPresence = CY_SMIF_PRESENT_1BYTE,
    /* The high byte of a 16-bit command. This value is 0x0 when there is no higher byte command present */
    .commandH = 0x00,
    /* The Data rate of command */
    .cmdRate = CY_SMIF_SDR,
#endif /* CY_IP_MXSMIF_VERSION */
};

cy_stc_smif_mem_cmd_t S25HS512T_SMIF0_SlaveSlot_1_readStsRegQeCmd =
{
    /* The 8-bit command. 1 x I/O read command. */
    .command = 0x35U,
    /* The width of the command transfer. */
    .cmdWidth = CY_SMIF_WIDTH_SINGLE,
    /* The width of the address transfer. */
    .addrWidth = CY_SMIF_WIDTH_SINGLE,
    /* The 8-bit mode byte. This value is 0xFFFFFFFF when there is no mode present. */
    .mode = 0xFFFFFFFFU,
    /* The width of the mode command transfer. */
    .modeWidth = CY_SMIF_WIDTH_SINGLE,
    /* The number of dummy cycles. A zero value suggests no dummy cycles. */
    .dummyCycles = 0U,
    /* The width of the data transfer. */
    .dataWidth = CY_SMIF_WIDTH_SINGLE,
#if (CY_IP_MXSMIF_VERSION >= 2)
    /* The Data rate of data */
    .dataRate = CY_SMIF_SDR,
    /* This specifies the presence of the dummy field */
    .dummyCyclesPresence = CY_SMIF_NOT_PRESENT,
    /* This specifies the presence of the mode field */
    .modePresence = CY_SMIF_NOT_PRESENT,
    /* The high byte of a 16-bit mode. This value is 0x0 when there is no higher byte mode present */
    .modeH = 0x00,
    /* The Data rate of mode */
    .modeRate = CY_SMIF_SDR,
    /* The Data rate of address */
    .addrRate = CY_SMIF_SDR,
    /* This specifies the width of the command field */
    .cmdPresence = CY_SMIF_PRESENT_1BYTE,
    /* The high byte of a 16-bit command. This value is 0x0 when there is no higher byte command present */
    .commandH = 0x00,
    /* The Data rate of command */
    .cmdRate = CY_SMIF_SDR,
#endif /* CY_IP_MXSMIF_VERSION */
};

cy_stc_smif_mem_cmd_t S25HS512T_SMIF0_SlaveSlot_1_readStsRegWipCmd =
{
    /* The 8-bit command. 1 x I/O read command. */
    .command = 0x05U,
    /* The width of the command transfer. */
    .cmdWidth = CY_SMIF_WIDTH_SINGLE,
    /* The width of the address transfer. */
    .addrWidth = CY_SMIF_WIDTH_SINGLE,
    /* The 8-bit mode byte. This value is 0xFFFFFFFF when there is no mode present. */
    .mode = 0xFFFFFFFFU,
    /* The width of the mode command transfer. */
    .modeWidth = CY_SMIF_WIDTH_SINGLE,
    /* The number of dummy cycles. A zero value suggests no dummy cycles. */
    .dummyCycles = 0U,
    /* The width of the data transfer. */
    .dataWidth = CY_SMIF_WIDTH_SINGLE,
#if (CY_IP_MXSMIF_VERSION >= 2)
    /* The Data rate of data */
    .dataRate = CY_SMIF_SDR,
    /* This specifies the presence of the dummy field */
    .dummyCyclesPresence = CY_SMIF_NOT_PRESENT,
    /* This specifies the presence of the mode field */
    .modePresence = CY_SMIF_NOT_PRESENT,
    /* The high byte of a 16-bit mode. This value is 0x0 when there is no higher byte mode present */
    .modeH = 0x00,
    /* The Data rate of mode */
    .modeRate = CY_SMIF_SDR,
    /* The Data rate of address */
    .addrRate = CY_SMIF_SDR,
    /* This specifies the width of the command field */
    .cmdPresence = CY_SMIF_PRESENT_1BYTE,
    /* The high byte of a 16-bit command. This value is 0x0 when there is no higher byte command present */
    .commandH = 0x00,
    /* The Data rate of command */
    .cmdRate = CY_SMIF_SDR,
#endif /* CY_IP_MXSMIF_VERSION */
};

cy_stc_smif_mem_cmd_t S25HS512T_SMIF0_SlaveSlot_1_writeStsRegQeCmd =
{
    /* The 8-bit command. 1 x I/O read command. */
    .command = 0x01U,
    /* The width of the command transfer. */
    .cmdWidth = CY_SMIF_WIDTH_SINGLE,
    /* The width of the address transfer. */
    .addrWidth = CY_SMIF_WIDTH_SINGLE,
    /* The 8-bit mode byte. This value is 0xFFFFFFFF when there is no mode present. */
    .mode = 0xFFFFFFFFU,
    /* The width of the mode command transfer. */
    .modeWidth = CY_SMIF_WIDTH_SINGLE,
    /* The number of dummy cycles. A zero value suggests no dummy cycles. */
    .dummyCycles = 0U,
    /* The width of the data transfer. */
    .dataWidth = CY_SMIF_WIDTH_SINGLE,
#if (CY_IP_MXSMIF_VERSION >= 2)
    /* The Data rate of data */
    .dataRate = CY_SMIF_SDR,
    /* This specifies the presence of the dummy field */
    .dummyCyclesPresence = CY_SMIF_NOT_PRESENT,
    /* This specifies the presence of the mode field */
    .modePresence = CY_SMIF_NOT_PRESENT,
    /* The high byte of a 16-bit mode. This value is 0x0 when there is no higher byte mode present */
    .modeH = 0x00,
    /* The Data rate of mode */
    .modeRate = CY_SMIF_SDR,
    /* The Data rate of address */
    .addrRate = CY_SMIF_SDR,
    /* This specifies the width of the command field */
    .cmdPresence = CY_SMIF_PRESENT_1BYTE,
    /* The high byte of a 16-bit command. This value is 0x0 when there is no higher byte command present */
    .commandH = 0x00,
    /* The Data rate of command */
    .cmdRate = CY_SMIF_SDR,
#endif /* CY_IP_MXSMIF_VERSION */
};

#if (CY_SMIF_DRV_VERSION_MAJOR > 1) || (CY_SMIF_DRV_VERSION_MINOR >= 50)
static cy_stc_smif_hybrid_region_info_t S25HS512T_SMIF0_SlaveSlot_1_region0 =
{
    /* The region start address. */
    .regionAddress = 0x0U,
    /* The number of sectors in this region. */
    .sectorsCount = 32U,
    /* The erase command to use for erasing sectors in this region. */
    .eraseCmd = 0x21U,
    /* The sector size in bytes. */
    .eraseSize = 0x1000U,
    /* The erase time for these sectors in ms. */
    .eraseTime = 335U,
};

static cy_stc_smif_hybrid_region_info_t S25HS512T_SMIF0_SlaveSlot_1_region1 =
{
    /* The region start address. */
    .regionAddress = 0x20000U,
    /* The number of sectors in this region. */
    .sectorsCount = 1U,
    /* The erase command to use for erasing sectors in this region. */
    .eraseCmd = 0xDCU,
    /* The sector size in bytes. */
    .eraseSize = 0x20000U,
    /* The erase time for these sectors in ms. */
    .eraseTime = 5869U,
};

static cy_stc_smif_hybrid_region_info_t S25HS512T_SMIF0_SlaveSlot_1_region2 =
{
    /* The region start address. */
    .regionAddress = 0x40000U,
    /* The number of sectors in this region. */
    .sectorsCount = 255U,
    /* The erase command to use for erasing sectors in this region. */
    .eraseCmd = 0xDCU,
    /* The sector size in bytes. */
    .eraseSize = 0x40000U,
    /* The erase time for these sectors in ms. */
    .eraseTime = 5869U,
};

static cy_stc_smif_hybrid_region_info_t * S25HS512T_SMIF0_SlaveSlot_1_regionInfo[3] = {
   &S25HS512T_SMIF0_SlaveSlot_1_region0,
   &S25HS512T_SMIF0_SlaveSlot_1_region1,
   &S25HS512T_SMIF0_SlaveSlot_1_region2
};

#endif

cy_stc_smif_mem_device_cfg_t deviceCfg_S25HS512T_SMIF0_SlaveSlot_1 =
{
    /* Specifies the number of address bytes used by the memory slave device. */
    .numOfAddrBytes = 0x04U,
    /* The size of the memory. */
    .memSize = 0x04000000U,
    /* Specifies the Read command. */
    .readCmd = &S25HS512T_SMIF0_SlaveSlot_1_readCmd,
    /* Specifies the Write Enable command. */
    .writeEnCmd = &S25HS512T_SMIF0_SlaveSlot_1_writeEnCmd,
    /* Specifies the Write Disable command. */
    .writeDisCmd = &S25HS512T_SMIF0_SlaveSlot_1_writeDisCmd,
    /* Specifies the Erase command. */
    .eraseCmd = &S25HS512T_SMIF0_SlaveSlot_1_eraseCmd,
    /* Specifies the sector size of each erase. */
    .eraseSize = 0x0040000U,
    /* Specifies the Chip Erase command. */
    .chipEraseCmd = &S25HS512T_SMIF0_SlaveSlot_1_chipEraseCmd,
    /* Specifies the Program command. */
    .programCmd = &S25HS512T_SMIF0_SlaveSlot_1_programCmd,
    /* Specifies the page size for programming. */
    .programSize = 0x0000100U,
    /* Specifies the command to read the QE-containing status register. */
    .readStsRegQeCmd = &S25HS512T_SMIF0_SlaveSlot_1_readStsRegQeCmd,
    /* Specifies the command to read the WIP-containing status register. */
    .readStsRegWipCmd = &S25HS512T_SMIF0_SlaveSlot_1_readStsRegWipCmd,
    /* Specifies the command to write into the QE-containing status register. */
    .writeStsRegQeCmd = &S25HS512T_SMIF0_SlaveSlot_1_writeStsRegQeCmd,
    /* The mask for the status register. */
    .stsRegBusyMask = 0x01U,
    /* The mask for the status register. */
    .stsRegQuadEnableMask = 0x02U,
    /* The max time for the erase type-1 cycle-time in ms. */
    .eraseTime = 5869U,
    /* The max time for the chip-erase cycle-time in ms. */
    .chipEraseTime = 696000U,
    /* The max time for the page-program cycle-time in us. */
    .programTime = 2175U,
#if (CY_SMIF_DRV_VERSION_MAJOR > 1) || (CY_SMIF_DRV_VERSION_MINOR >= 50)
    /* Points to NULL or to structure with info about sectors for hybrid memory. */
    .hybridRegionCount = 3U,
    .hybridRegionInfo = S25HS512T_SMIF0_SlaveSlot_1_regionInfo,
#else
#error Hybrid erase sectors require SMIF driver 1.50 or greater.
#endif
    /* Specifies the command to read variable latency cycles configuration register */
    .readLatencyCmd = 0,
    /* Specifies the command to write variable latency cycles configuration register */
    .writeLatencyCmd = 0,
    /* Specifies the address for variable latency cycle address */
    .latencyCyclesRegAddr = 0x00U,
    /* Specifies variable latency cycles Mask */
    .latencyCyclesMask = 0x00U,
#if (CY_IP_MXSMIF_VERSION >= 2)
    /* Specifies data for memory with hybrid sectors */
    .octalDDREnableSeq = 0,
    /* Specifies the command to read the OE-containing status register. */
    .readStsRegOeCmd = 0,
    /* Specifies the command to write the OE-containing status register. */
    .writeStsRegOeCmd = 0,
    /* QE mask for the status registers */
    .stsRegOctalEnableMask = 0x00U,
    /* Octal enable register address */
    .octalEnableRegAddr = 0x00U,
    /* Frequency of operation used in Octal mode */
    .freq_of_operation = CY_SMIF_100MHZ_OPERATION,
#endif
};

cy_stc_smif_mem_config_t S25HS512T_SMIF0_SlaveSlot_1 =
{
    /* Determines the slot number where the memory device is placed. */
    .slaveSelect = CY_SMIF_SLAVE_SELECT_1,
    /* Flags. */
#if (CY_IP_MXSMIF_VERSION >= 2)
    .flags = CY_SMIF_FLAG_SMIF_REV_3 | CY_SMIF_FLAG_MEMORY_MAPPED | CY_SMIF_FLAG_WR_EN | CY_SMIF_FLAG_MERGE_ENABLE,
#else
    .flags = CY_SMIF_FLAG_MEMORY_MAPPED | CY_SMIF_FLAG_WR_EN | CY_SMIF_FLAG_MERGE_ENABLE,
#endif /* CY_IP_MXSMIF_VERSION */
    /* The data-line selection options for a slave device. */
    .dataSelect = CY_SMIF_DATA_SEL0,
    /* The base address the memory slave is mapped to in the PSoC memory map.
    Valid when the memory-mapped mode is enabled. */
    .baseAddress = 0x60000000U,
    /* The size allocated in the PSoC memory map, for the memory slave device.
    The size is allocated from the base address. Valid when the memory mapped mode is enabled. */
    .memMappedSize = 0x4000000U,
    /* If this memory device is one of the devices in the dual quad SPI configuration.
    Valid when the memory mapped mode is enabled. */
    .dualQuadSlots = 0,
    /* The configuration of the device. */
    .deviceCfg = &deviceCfg_S25HS512T_SMIF0_SlaveSlot_1,
#if (CY_IP_MXSMIF_VERSION >= 2)
    /** Continous transfer merge timeout.
     * After this period the memory device is deselected. A later transfer, even from a
     * continuous address, starts with the overhead phases (command, address, mode, dummy cycles).
     * This configuration parameter is available for CAT1B devices. */
    .mergeTimeout = CY_SMIF_MERGE_TIMEOUT_256_CYCLES,
#else
#error Features used by this file require CY_IP_MXSMIF_VERSION >= 2.
#endif /* CY_IP_MXSMIF_VERSION */
};

cy_stc_smif_mem_config_t* smif0MemConfigs[CY_SMIF_DEVICE_NUM0] = {
   &S25HS512T_SMIF0_SlaveSlot_1,
};

cy_stc_smif_block_config_t smif0BlockConfig =
{
    /* The number of SMIF memories defined. */
    .memCount = CY_SMIF_DEVICE_NUM0,
    /* The pointer to the array of memory config structures of size memCount. */
    .memConfig = (cy_stc_smif_mem_config_t**)smif0MemConfigs,
    /* The version of the SMIF driver. */
    .majorVersion = CY_SMIF_DRV_VERSION_MAJOR,
    /* The version of the SMIF driver. */
    .minorVersion = CY_SMIF_DRV_VERSION_MINOR,
};
cy_stc_smif_block_config_t smif1BlockConfig =
{
    /* The number of SMIF memories defined. */
    .memCount = CY_SMIF_DEVICE_NUM1,
    /* The pointer to the array of memory config structures of size memCount. */
    .memConfig = 0,
    /* The version of the SMIF driver. */
    .majorVersion = CY_SMIF_DRV_VERSION_MAJOR,
    /* The version of the SMIF driver. */
    .minorVersion = CY_SMIF_DRV_VERSION_MINOR,
};
