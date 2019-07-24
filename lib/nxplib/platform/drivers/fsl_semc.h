/*
 * Copyright 2017-2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _FSL_SEMC_H_
#define _FSL_SEMC_H_

#include "fsl_common.h"

/*!
 * @addtogroup semc
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
/*@{*/
/*! @brief SEMC driver version 2.0.4. */
#define FSL_SEMC_DRIVER_VERSION (MAKE_VERSION(2, 0, 4))
/*@}*/

/*! @brief SEMC status. */
enum _semc_status
{
    kStatus_SEMC_InvalidDeviceType            = MAKE_STATUS(kStatusGroup_SEMC, 0),
    kStatus_SEMC_IpCommandExecutionError      = MAKE_STATUS(kStatusGroup_SEMC, 1),
    kStatus_SEMC_AxiCommandExecutionError     = MAKE_STATUS(kStatusGroup_SEMC, 2),
    kStatus_SEMC_InvalidMemorySize            = MAKE_STATUS(kStatusGroup_SEMC, 3),
    kStatus_SEMC_InvalidIpcmdDataSize         = MAKE_STATUS(kStatusGroup_SEMC, 4),
    kStatus_SEMC_InvalidAddressPortWidth      = MAKE_STATUS(kStatusGroup_SEMC, 5),
    kStatus_SEMC_InvalidDataPortWidth         = MAKE_STATUS(kStatusGroup_SEMC, 6),
    kStatus_SEMC_InvalidSwPinmuxSelection     = MAKE_STATUS(kStatusGroup_SEMC, 7),
    kStatus_SEMC_InvalidBurstLength           = MAKE_STATUS(kStatusGroup_SEMC, 8),
    kStatus_SEMC_InvalidColumnAddressBitWidth = MAKE_STATUS(kStatusGroup_SEMC, 9),
    kStatus_SEMC_InvalidBaseAddress           = MAKE_STATUS(kStatusGroup_SEMC, 10),
    kStatus_SEMC_InvalidTimerSetting          = MAKE_STATUS(kStatusGroup_SEMC, 11),
};

/*! @brief SEMC memory device type. */
typedef enum _semc_mem_type
{
    kSEMC_MemType_SDRAM = 0, /*!< SDRAM */
    kSEMC_MemType_SRAM,      /*!< SRAM */
    kSEMC_MemType_NOR,       /*!< NOR */
    kSEMC_MemType_NAND,      /*!< NAND */
    kSEMC_MemType_8080       /*!< 8080. */
} semc_mem_type_t;

/*! @brief SEMC WAIT/RDY polarity. */
typedef enum _semc_waitready_polarity
{
    kSEMC_LowActive = 0, /*!< Low active. */
    kSEMC_HighActive,    /*!< High active. */
} semc_waitready_polarity_t;

/*! @brief SEMC SDRAM Chip selection . */
typedef enum _semc_sdram_cs
{
    kSEMC_SDRAM_CS0 = 0, /*!< SEMC SDRAM CS0. */
    kSEMC_SDRAM_CS1,     /*!< SEMC SDRAM CS1. */
    kSEMC_SDRAM_CS2,     /*!< SEMC SDRAM CS2. */
    kSEMC_SDRAM_CS3      /*!< SEMC SDRAM CS3. */
} semc_sdram_cs_t;

/*! @brief SEMC NAND device type. */
typedef enum _semc_nand_access_type
{
    kSEMC_NAND_ACCESS_BY_AXI = 0,
    kSEMC_NAND_ACCESS_BY_IPCMD,
} semc_nand_access_type_t;

/*! @brief SEMC interrupts . */
typedef enum _semc_interrupt_enable
{
    kSEMC_IPCmdDoneInterrupt = SEMC_INTEN_IPCMDDONEEN_MASK, /*!< Ip command done interrupt. */
    kSEMC_IPCmdErrInterrupt  = SEMC_INTEN_IPCMDERREN_MASK,  /*!< Ip command error interrupt. */
    kSEMC_AXICmdErrInterrupt = SEMC_INTEN_AXICMDERREN_MASK, /*!< AXI command error interrupt. */
    kSEMC_AXIBusErrInterrupt = SEMC_INTEN_AXIBUSERREN_MASK  /*!< AXI bus error interrupt. */
} semc_interrupt_enable_t;

/*! @brief SEMC IP command data size in bytes. */
typedef enum _semc_ipcmd_datasize
{
    kSEMC_IPcmdDataSize_1bytes = 1, /*!< The IP command data size 1 byte. */
    kSEMC_IPcmdDataSize_2bytes,     /*!< The IP command data size 2 byte. */
    kSEMC_IPcmdDataSize_3bytes,     /*!< The IP command data size 3 byte. */
    kSEMC_IPcmdDataSize_4bytes      /*!< The IP command data size 4 byte. */
} semc_ipcmd_datasize_t;

/*! @brief SEMC auto-refresh timing. */
typedef enum _semc_refresh_time
{
    kSEMC_RefreshThreeClocks = 0x0U, /*!< The refresh timing with three bus clocks. */
    kSEMC_RefreshSixClocks,          /*!< The refresh timing with six bus clocks. */
    kSEMC_RefreshNineClocks          /*!< The refresh timing with nine bus clocks. */
} semc_refresh_time_t;

/*! @brief CAS latency */
typedef enum _semc_caslatency
{
    kSEMC_LatencyOne = 1, /*!< Latency  1. */
    kSEMC_LatencyTwo,     /*!< Latency  2. */
    kSEMC_LatencyThree,   /*!< Latency  3. */
} semc_caslatency_t;

/*! @brief SEMC sdram column address bit number. */
typedef enum _semc_sdram_column_bit_num
{
    kSEMC_SdramColunm_12bit = 0x0U, /*!< 12 bit. */
    kSEMC_SdramColunm_11bit,        /*!< 11 bit. */
    kSEMC_SdramColunm_10bit,        /*!< 10 bit. */
    kSEMC_SdramColunm_9bit,         /*!< 9 bit. */
} semc_sdram_column_bit_num_t;

/*! @brief SEMC sdram burst length. */
typedef enum _semc_sdram_burst_len
{
    kSEMC_Sdram_BurstLen1 = 0, /*!< Burst length 1*/
    kSEMC_Sdram_BurstLen2,     /*!< Burst length 2*/
    kSEMC_Sdram_BurstLen4,     /*!< Burst length 4*/
    kSEMC_Sdram_BurstLen8      /*!< Burst length 8*/
} sem_sdram_burst_len_t;

/*! @brief SEMC nand column address bit number. */
typedef enum _semc_nand_column_bit_num
{
    kSEMC_NandColum_16bit = 0x0U, /*!< 16 bit. */
    kSEMC_NandColum_15bit,        /*!< 15 bit. */
    kSEMC_NandColum_14bit,        /*!< 14 bit. */
    kSEMC_NandColum_13bit,        /*!< 13 bit. */
    kSEMC_NandColum_12bit,        /*!< 12 bit. */
    kSEMC_NandColum_11bit,        /*!< 11 bit. */
    kSEMC_NandColum_10bit,        /*!< 10 bit. */
    kSEMC_NandColum_9bit,         /*!< 9 bit. */
} semc_nand_column_bit_num_t;

/*! @brief SEMC nand burst length. */
typedef enum _semc_nand_burst_len
{
    kSEMC_Nand_BurstLen1 = 0, /*!< Burst length 1*/
    kSEMC_Nand_BurstLen2,     /*!< Burst length 2*/
    kSEMC_Nand_BurstLen4,     /*!< Burst length 4*/
    kSEMC_Nand_BurstLen8,     /*!< Burst length 8*/
    kSEMC_Nand_BurstLen16,    /*!< Burst length 16*/
    kSEMC_Nand_BurstLen32,    /*!< Burst length 32*/
    kSEMC_Nand_BurstLen64     /*!< Burst length 64*/
} sem_nand_burst_len_t;

/*! @brief SEMC nor/sram column address bit number. */
typedef enum _semc_norsram_column_bit_num
{
    kSEMC_NorColum_12bit = 0x0U, /*!< 12 bit. */
    kSEMC_NorColum_11bit,        /*!< 11 bit. */
    kSEMC_NorColum_10bit,        /*!< 10 bit. */
    kSEMC_NorColum_9bit,         /*!< 9 bit. */
    kSEMC_NorColum_8bit,         /*!< 8 bit. */
    kSEMC_NorColum_7bit,         /*!< 7 bit. */
    kSEMC_NorColum_6bit,         /*!< 6 bit. */
    kSEMC_NorColum_5bit,         /*!< 5 bit. */
    kSEMC_NorColum_4bit,         /*!< 4 bit. */
    kSEMC_NorColum_3bit,         /*!< 3 bit. */
    kSEMC_NorColum_2bit          /*!< 2 bit. */
} semc_norsram_column_bit_num_t;

/*! @brief SEMC nor/sram burst length. */
typedef enum _semc_norsram_burst_len
{
    kSEMC_Nor_BurstLen1 = 0, /*!< Burst length 1*/
    kSEMC_Nor_BurstLen2,     /*!< Burst length 2*/
    kSEMC_Nor_BurstLen4,     /*!< Burst length 4*/
    kSEMC_Nor_BurstLen8,     /*!< Burst length 8*/
    kSEMC_Nor_BurstLen16,    /*!< Burst length 16*/
    kSEMC_Nor_BurstLen32,    /*!< Burst length 32*/
    kSEMC_Nor_BurstLen64     /*!< Burst length 64*/
} sem_norsram_burst_len_t;

/*! @brief SEMC dbi column address bit number. */
typedef enum _semc_dbi_column_bit_num
{
    kSEMC_Dbi_Colum_12bit = 0x0U, /*!< 12 bit. */
    kSEMC_Dbi_Colum_11bit,        /*!< 11 bit. */
    kSEMC_Dbi_Colum_10bit,        /*!< 10 bit. */
    kSEMC_Dbi_Colum_9bit,         /*!< 9 bit. */
    kSEMC_Dbi_Colum_8bit,         /*!< 8 bit. */
    kSEMC_Dbi_Colum_7bit,         /*!< 7 bit. */
    kSEMC_Dbi_Colum_6bit,         /*!< 6 bit. */
    kSEMC_Dbi_Colum_5bit,         /*!< 5 bit. */
    kSEMC_Dbi_Colum_4bit,         /*!< 4 bit. */
    kSEMC_Dbi_Colum_3bit,         /*!< 3 bit. */
    kSEMC_Dbi_Colum_2bit          /*!< 2 bit. */
} semc_dbi_column_bit_num_t;

/*! @brief SEMC dbi burst length. */
typedef enum _semc_dbi_burst_len
{
    kSEMC_Dbi_BurstLen1 = 0, /*!< Burst length 1*/
    kSEMC_Dbi_BurstLen2,     /*!< Burst length 2*/
    kSEMC_Dbi_Dbi_BurstLen4, /*!< Burst length 4*/
    kSEMC_Dbi_BurstLen8,     /*!< Burst length 8*/
    kSEMC_Dbi_BurstLen16,    /*!< Burst length 16*/
    kSEMC_Dbi_BurstLen32,    /*!< Burst length 32*/
    kSEMC_Dbi_BurstLen64     /*!< Burst length 64*/
} sem_dbi_burst_len_t;

/*! @brief SEMC IOMUXC. */
typedef enum _semc_iomux_pin
{
    kSEMC_MUXA8   = SEMC_IOCR_MUX_A8_SHIFT,   /*!< MUX A8 pin. */
    kSEMC_MUXCSX0 = SEMC_IOCR_MUX_CSX0_SHIFT, /*!< MUX CSX0 pin */
    kSEMC_MUXCSX1 = SEMC_IOCR_MUX_CSX1_SHIFT, /*!< MUX CSX1 Pin.*/
    kSEMC_MUXCSX2 = SEMC_IOCR_MUX_CSX2_SHIFT, /*!< MUX CSX2 Pin. */
    kSEMC_MUXCSX3 = SEMC_IOCR_MUX_CSX3_SHIFT, /*!< MUX CSX3 Pin. */
    kSEMC_MUXRDY  = SEMC_IOCR_MUX_RDY_SHIFT   /*!< MUX RDY pin. */
} semc_iomux_pin;

/*! @brief SEMC NOR/PSRAM Address bit 27 A27. */
typedef enum _semc_iomux_nora27_pin
{
    kSEMC_MORA27_NONE    = 0,                        /*!< No NOR/SRAM A27 pin. */
    kSEMC_NORA27_MUXCSX3 = SEMC_IOCR_MUX_CSX3_SHIFT, /*!< MUX CSX3 Pin. */
    kSEMC_NORA27_MUXRDY  = SEMC_IOCR_MUX_RDY_SHIFT   /*!< MUX RDY pin. */
} semc_iomux_nora27_pin;

/*! @brief SEMC port size. */
typedef enum _semc_port_size
{
    kSEMC_PortSize8Bit = 0, /*!< 8-Bit port size. */
    kSEMC_PortSize16Bit     /*!< 16-Bit port size. */
} smec_port_size_t;

/*! @brief SEMC address mode. */
typedef enum _semc_addr_mode
{
    kSEMC_AddrDataMux = 0, /*!< SEMC address/data mux mode. */
    kSEMC_AdvAddrdataMux,  /*!< Advanced address/data mux mode. */
    kSEMC_AddrDataNonMux   /*!< Address/data non-mux mode. */
} semc_addr_mode_t;

/*! @brief SEMC DQS read strobe mode. */
typedef enum _semc_dqs_mode
{
    kSEMC_Loopbackinternal = 0, /*!< Dummy read strobe loopbacked internally. */
    kSEMC_Loopbackdqspad,       /*!< Dummy read strobe loopbacked from DQS pad. */
} semc_dqs_mode_t;

/*! @brief SEMC ADV signal active polarity. */
typedef enum _semc_adv_polarity
{
    kSEMC_AdvActiveLow = 0, /*!< Adv active low. */
    kSEMC_AdvActivehigh,    /*!< Adv active low. */
} semc_adv_polarity_t;

/*! @brief SEMC RDY signal active polarity. */
typedef enum _semc_rdy_polarity
{
    kSEMC_RdyActiveLow = 0, /*!< Adv active low. */
    kSEMC_RdyActivehigh,    /*!< Adv active low. */
} semc_rdy_polarity_t;

/*! @brief SEMC IP command for NAND: address mode. */
typedef enum _semc_ipcmd_nand_addrmode
{
    kSEMC_NANDAM_ColumnRow = 0x0U, /*!< Address mode: column and row address(5Byte-CA0/CA1/RA0/RA1/RA2). */
    kSEMC_NANDAM_ColumnCA0,        /*!< Address mode: column address only(1 Byte-CA0).  */
    kSEMC_NANDAM_ColumnCA0CA1,     /*!< Address mode: column address only(2 Byte-CA0/CA1). */
    kSEMC_NANDAM_RawRA0,           /*!< Address mode: row address only(1 Byte-RA0). */
    kSEMC_NANDAM_RawRA0RA1,        /*!< Address mode: row address only(2 Byte-RA0/RA1). */
    kSEMC_NANDAM_RawRA0RA1RA2      /*!< Address mode: row address only(3 Byte-RA0).  */
} semc_ipcmd_nand_addrmode_t;

/*! @brief SEMC IP command for NAND： command mode. */
typedef enum _semc_ipcmd_nand_cmdmode
{
    kSEMC_NANDCM_Command = 0x2U,      /*!< command. */
    kSEMC_NANDCM_CommandHold,         /*!< Command hold. */
    kSEMC_NANDCM_CommandAddress,      /*!< Command address. */
    kSEMC_NANDCM_CommandAddressHold,  /*!< Command address hold.  */
    kSEMC_NANDCM_CommandAddressRead,  /*!< Command address read.  */
    kSEMC_NANDCM_CommandAddressWrite, /*!< Command address write.  */
    kSEMC_NANDCM_CommandRead,         /*!< Command read.  */
    kSEMC_NANDCM_CommandWrite,        /*!< Command write.  */
    kSEMC_NANDCM_Read,                /*!< Read.  */
    kSEMC_NANDCM_Write                /*!< Write.  */
} semc_ipcmd_nand_cmdmode_t;

/*! @brief SEMC NAND address option. */
typedef enum _semc_nand_address_option
{
    kSEMC_NandAddrOption_5byte_CA2RA3 = 0U, /*!< CA0+CA1+RA0+RA1+RA2 */
    kSEMC_NandAddrOption_4byte_CA2RA2 = 2U, /*!< CA0+CA1+RA0+RA1 */
    kSEMC_NandAddrOption_3byte_CA2RA1 = 4U, /*!< CA0+CA1+RA0 */
    kSEMC_NandAddrOption_4byte_CA1RA3 = 1U, /*!< CA0+RA0+RA1+RA2 */
    kSEMC_NandAddrOption_3byte_CA1RA2 = 3U, /*!< CA0+RA0+RA1 */
    kSEMC_NandAddrOption_2byte_CA1RA1 = 7U, /*!< CA0+RA0 */
} semc_nand_address_option_t;

/*! @brief SEMC IP command for NOR. */
typedef enum _semc_ipcmd_nor_dbi
{
    kSEMC_NORDBICM_Read = 0x2U, /*!< NOR read. */
    kSEMC_NORDBICM_Write        /*!< NOR write.  */
} semc_ipcmd_nor_dbi_t;

/*! @brief SEMC IP command for SRAM. */
typedef enum _semc_ipcmd_sram
{
    kSEMC_SRAMCM_ArrayRead = 0x2U, /*!< SRAM memory array read. */
    kSEMC_SRAMCM_ArrayWrite,       /*!< SRAM memory array write. */
    kSEMC_SRAMCM_RegRead,          /*!< SRAM memory register read. */
    kSEMC_SRAMCM_RegWrite          /*!< SRAM memory register write. */
} semc_ipcmd_sram_t;

/*! @brief SEMC IP command for SDARM. */
typedef enum _semc_ipcmd_sdram
{
    kSEMC_SDRAMCM_Read = 0x8U, /*!< SDRAM memory read. */
    kSEMC_SDRAMCM_Write,       /*!< SDRAM memory write. */
    kSEMC_SDRAMCM_Modeset,     /*!< SDRAM MODE SET. */
    kSEMC_SDRAMCM_Active,      /*!< SDRAM active. */
    kSEMC_SDRAMCM_AutoRefresh, /*!< SDRAM auto-refresh. */
    kSEMC_SDRAMCM_SelfRefresh, /*!< SDRAM self-refresh. */
    kSEMC_SDRAMCM_Precharge,   /*!< SDRAM precharge. */
    kSEMC_SDRAMCM_Prechargeall /*!< SDRAM precharge all. */
} semc_ipcmd_sdram_t;

/*! @brief SEMC SDRAM configuration structure.
 *
 * 1. The memory size in the configuration is in the unit of KB. So memsize_kbytes
 * should be set as 2^2, 2^3, 2^4 .etc which is base 2KB exponential function.
 * Take refer to BR0~BR3 register in RM for details.
 * 2. The prescalePeriod_N16Cycle is in unit of 16 clock cycle. It is a exception for prescaleTimer_n16cycle = 0,
 * it means the prescaler timer period is 256 * 16 clock cycles. For precalerIf precalerTimer_n16cycle not equal to 0,
 * The  prescaler timer period is prescalePeriod_N16Cycle * 16 clock cycles.
 * idleTimeout_NprescalePeriod,  refreshUrgThreshold_NprescalePeriod, refreshPeriod_NprescalePeriod are
 * similar to prescalePeriod_N16Cycle.
 *
 */
typedef struct _semc_sdram_config
{
    semc_iomux_pin csxPinMux;       /*!< CS pin mux. The kSEMC_MUXA8 is not valid in sdram pin mux setting. */
    uint32_t address;               /*!< The base address. */
    uint32_t memsize_kbytes;        /*!< The memory size in unit of kbytes. */
    smec_port_size_t portSize;      /*!< Port size. */
    sem_sdram_burst_len_t burstLen; /*!< Burst length. */
    semc_sdram_column_bit_num_t columnAddrBitNum; /*!< Column address bit number. */
    semc_caslatency_t casLatency;                 /*!< CAS latency. */
    uint8_t tPrecharge2Act_Ns;                    /*!< Precharge to active wait time in unit of nanosecond. */
    uint8_t tAct2ReadWrite_Ns;                    /*!< Act to read/write wait time in unit of nanosecond. */
    uint8_t tRefreshRecovery_Ns;                  /*!< Refresh recovery time in unit of nanosecond. */
    uint8_t tWriteRecovery_Ns;                    /*!< write recovery time in unit of nanosecond. */
    uint8_t tCkeOff_Ns;                           /*!< CKE off minimum time in unit of nanosecond. */
    uint8_t tAct2Prechage_Ns;                     /*!< Active to precharge in unit of nanosecond. */
    uint8_t tSelfRefRecovery_Ns;                  /*!< Self refresh recovery time in unit of nanosecond. */
    uint8_t tRefresh2Refresh_Ns;                  /*!< Refresh to refresh wait time in unit of nanosecond. */
    uint8_t tAct2Act_Ns;                          /*!< Active to active wait time in unit of nanosecond. */
    uint32_t tPrescalePeriod_Ns;     /*!< Prescaler timer period should not be larger than 256 * 16 * clock cycle. */
    uint32_t tIdleTimeout_Ns;        /*!< Idle timeout in unit of prescale time period. */
    uint32_t refreshPeriod_nsPerRow; /*!< Refresh timer period like 64ms * 1000000/8192 . */
    uint32_t refreshUrgThreshold;    /*!< Refresh urgent threshold. */
    uint8_t refreshBurstLen;         /*!< Refresh burst length. */
} semc_sdram_config_t;

/*! @brief SEMC NAND device timing configuration structure. */
typedef struct _semc_nand_timing_config
{
    uint8_t tCeSetup_Ns;        /*!< CE setup time: tCS. */
    uint8_t tCeHold_Ns;         /*!< CE hold time: tCH. */
    uint8_t tCeInterval_Ns;     /*!< CE interval time:tCEITV. */
    uint8_t tWeLow_Ns;          /*!< WE low time: tWP. */
    uint8_t tWeHigh_Ns;         /*!< WE high time: tWH. */
    uint8_t tReLow_Ns;          /*!< RE low time: tRP. */
    uint8_t tReHigh_Ns;         /*!< RE high time: tREH. */
    uint8_t tTurnAround_Ns;     /*!< Turnaround time for async mode: tTA. */
    uint8_t tWehigh2Relow_Ns;   /*!< WE# high to RE# wait time: tWHR. */
    uint8_t tRehigh2Welow_Ns;   /*!< RE# high to WE# low wait time: tRHW. */
    uint8_t tAle2WriteStart_Ns; /*!< ALE to write start wait time: tADL. */
    uint8_t tReady2Relow_Ns;    /*!< Ready to RE# low min wait time: tRR. */
    uint8_t tWehigh2Busy_Ns;    /*!< WE# high to busy wait time: tWB. */
} semc_nand_timing_config_t;

/*! @brief SEMC NAND configuration structure. */
typedef struct _semc_nand_config
{
    semc_iomux_pin cePinMux;    /*!< The CE pin mux setting. The kSEMC_MUXRDY is not valid for CE pin setting. */
    uint32_t axiAddress;        /*!< The base address for AXI nand. */
    uint32_t axiMemsize_kbytes; /*!< The memory size in unit of kbytes for AXI nand. */
    uint32_t ipgAddress;        /*!< The base address for IPG nand . */
    uint32_t ipgMemsize_kbytes; /*!< The memory size in unit of kbytes for IPG nand. */
    semc_rdy_polarity_t rdyactivePolarity;       /*!< Wait ready polarity. */
    bool edoModeEnabled;                         /*!< EDO mode enabled. */
    semc_nand_column_bit_num_t columnAddrBitNum; /*!< Column address bit number. */
    semc_nand_address_option_t arrayAddrOption;  /*!< Address option. */
    sem_nand_burst_len_t burstLen;               /*!< Burst length. */
    smec_port_size_t portSize;                   /*!< Port size. */
    semc_nand_timing_config_t *timingConfig;     /*!< SEMC nand timing configuration. */
} semc_nand_config_t;

/*! @brief SEMC NOR configuration structure. */
typedef struct _semc_nor_config
{
    semc_iomux_pin cePinMux;                        /*!< The CE# pin mux setting. */
    semc_iomux_nora27_pin addr27;                   /*!< The Addr bit 27 pin mux setting. */
    uint32_t address;                               /*!< The base address. */
    uint32_t memsize_kbytes;                        /*!< The memory size in unit of kbytes. */
    uint8_t addrPortWidth;                          /*!< The address port width. */
    semc_rdy_polarity_t rdyactivePolarity;          /*!< Wait ready polarity. */
    semc_adv_polarity_t advActivePolarity;          /*!< ADV# polarity. */
    semc_norsram_column_bit_num_t columnAddrBitNum; /*!< Column address bit number. */
    semc_addr_mode_t addrMode;                      /*!< Address mode. */
    sem_norsram_burst_len_t burstLen;               /*!< Burst length. */
    smec_port_size_t portSize;                      /*!< Port size. */
    uint8_t tCeSetup_Ns;                            /*!< The CE setup time. */
    uint8_t tCeHold_Ns;                             /*!< The CE hold time. */
    uint8_t tCeInterval_Ns;                         /*!< CE interval minimum time. */
    uint8_t tAddrSetup_Ns;                          /*!< The address setup time. */
    uint8_t tAddrHold_Ns;                           /*!< The address hold time. */
    uint8_t tWeLow_Ns;                              /*!< WE low time for async mode. */
    uint8_t tWeHigh_Ns;                             /*!< WE high time for async mode. */
    uint8_t tReLow_Ns;                              /*!< RE low time for async mode. */
    uint8_t tReHigh_Ns;                             /*!< RE high time for async mode. */
    uint8_t tTurnAround_Ns;                         /*!< Turnaround time for async mode. */
    uint8_t tAddr2WriteHold_Ns;                     /*!< Address to write data hold time for async mode. */
#if defined(FSL_FEATURE_SEMC_HAS_NOR_WDS_TIME) && (FSL_FEATURE_SEMC_HAS_NOR_WDS_TIME)
    uint8_t tWriteSetup_Ns; /*!< Write data setup time for sync mode.*/
#endif
#if defined(FSL_FEATURE_SEMC_HAS_NOR_WDH_TIME) && (FSL_FEATURE_SEMC_HAS_NOR_WDH_TIME)
    uint8_t tWriteHold_Ns; /*!< Write hold time for sync mode. */
#endif
    uint8_t latencyCount; /*!< Latency count for sync mode. */
    uint8_t readCycle;    /*!< Read cycle time for sync mode. */
} semc_nor_config_t;

/*! @brief SEMC SRAM  configuration structure. */
typedef struct _semc_sram_config
{
    semc_iomux_pin cePinMux;               /*!< The CE# pin mux setting. */
    semc_iomux_nora27_pin addr27;          /*!< The Addr bit 27 pin mux setting. */
    uint32_t address;                      /*!< The base address. */
    uint32_t memsize_kbytes;               /*!< The memory size in unit of kbytes. */
    uint8_t addrPortWidth;                 /*!< The address port width. */
    semc_adv_polarity_t advActivePolarity; /*!< ADV# polarity 1: active high, 0: active low. */
    semc_addr_mode_t addrMode;             /*!< Address mode. */
    sem_norsram_burst_len_t burstLen;      /*!< Burst length. */
    smec_port_size_t portSize;             /*!< Port size. */
    uint8_t tCeSetup_Ns;                   /*!< The CE setup time. */
    uint8_t tCeHold_Ns;                    /*!< The CE hold time. */
    uint8_t tCeInterval_Ns;                /*!< CE interval minimum time. */
    uint8_t tAddrSetup_Ns;                 /*!< The address setup time. */
    uint8_t tAddrHold_Ns;                  /*!< The address hold time. */
    uint8_t tWeLow_Ns;                     /*!< WE low time for async mode. */
    uint8_t tWeHigh_Ns;                    /*!< WE high time for async mode. */
    uint8_t tReLow_Ns;                     /*!< RE low time for async mode. */
    uint8_t tReHigh_Ns;                    /*!< RE high time for async mode. */
    uint8_t tTurnAround_Ns;                /*!< Turnaround time for async mode. */
    uint8_t tAddr2WriteHold_Ns;            /*!< Address to write data hold time for async mode. */
    uint8_t tWriteSetup_Ns;                /*!< Write data setup time for sync mode.*/
    uint8_t tWriteHold_Ns;                 /*!< Write hold time for sync mode. */
    uint8_t latencyCount;                  /*!< Latency count for sync mode. */
    uint8_t readCycle;                     /*!< Read cycle time for sync mode. */
} semc_sram_config_t;

/*! @brief SEMC DBI configuration structure. */
typedef struct _semc_dbi_config
{
    semc_iomux_pin csxPinMux;                   /*!< The CE# pin mux. */
    uint32_t address;                           /*!< The base address. */
    uint32_t memsize_kbytes;                    /*!< The memory size in unit of 4kbytes. */
    semc_dbi_column_bit_num_t columnAddrBitNum; /*!< Column address bit number. */
    sem_dbi_burst_len_t burstLen;               /*!< Burst length. */
    smec_port_size_t portSize;                  /*!< Port size. */
    uint8_t tCsxSetup_Ns;                       /*!< The CSX setup time. */
    uint8_t tCsxHold_Ns;                        /*!< The CSX hold time. */
    uint8_t tWexLow_Ns;                         /*!< WEX low time. */
    uint8_t tWexHigh_Ns;                        /*!< WEX high time. */
    uint8_t tRdxLow_Ns;                         /*!< RDX low time. */
    uint8_t tRdxHigh_Ns;                        /*!< RDX high time. */
    uint8_t tCsxInterval_Ns;                    /*!< Write data setup time.*/
} semc_dbi_config_t;

/*! @brief SEMC AXI queue a weight setting structure. */
typedef struct _semc_queuea_weight_struct
{
    uint32_t qos : 4;              /*!< weight of qos for queue 0 . */
    uint32_t aging : 4;            /*!< weight of aging for queue 0.*/
    uint32_t slaveHitSwith : 8;    /*!< weight of read/write switch for queue 0.*/
    uint32_t slaveHitNoswitch : 8; /*!< weight of read/write no switch for queue 0  .*/
} semc_queuea_weight_struct_t;

/*! @brief SEMC AXI queue a weight setting union. */
typedef union _semc_queuea_weight
{
    semc_queuea_weight_struct_t queueaConfig; /*!< Structure configuration for queueA. */
    uint32_t queueaValue; /*!< Configuration value for queueA which could directly write to the reg. */
} semc_queuea_weight_t;

/*! @brief SEMC AXI queue b weight setting structure. */
typedef struct _semc_queueb_weight_struct
{
    uint32_t qos : 4;           /*!< weight of qos for queue 1. */
    uint32_t aging : 4;         /*!< weight of aging for queue 1.*/
    uint32_t slaveHitSwith : 8; /*!< weight of read/write switch for queue 1.*/
    uint32_t weightPagehit : 8; /*!< weight of page hit for queue 1 only .*/
    uint32_t bankRotation : 8;  /*!< weight of bank rotation for queue 1 only .*/
} semc_queueb_weight_struct_t;

/*! @brief SEMC AXI queue b weight setting union. */
typedef union _semc_queueb_weight
{
    semc_queueb_weight_struct_t queuebConfig; /*!< Structure configuration for queueB. */
    uint32_t queuebValue; /*!< Configuration value for queueB which could directly write to the reg. */
} semc_queueb_weight_t;

/*! @brief SEMC AXI queue weight setting. */
typedef struct _semc_axi_queueweight
{
    semc_queuea_weight_t queueaWeight; /*!< Weight settings for queue a. */
    semc_queueb_weight_t queuebWeight; /*!< Weight settings for queue b. */
} semc_axi_queueweight_t;

/*!
 * @brief SEMC configuration structure.
 *
 * busTimeoutCycles: when busTimeoutCycles is zero, the bus timeout cycle is
 * 255*1024. otherwise the bus timeout cycles is busTimeoutCycles*1024.
 * cmdTimeoutCycles: is used for command execution timeout cycles. it's
 * similar to the busTimeoutCycles.
 */
typedef struct _semc_config_t
{
    semc_dqs_mode_t dqsMode;            /*!< Dummy read strobe mode: use enum in "semc_dqs_mode_t". */
    uint8_t cmdTimeoutCycles;           /*!< Command execution timeout cycles. */
    uint8_t busTimeoutCycles;           /*!< Bus timeout cycles. */
    semc_axi_queueweight_t queueWeight; /*!< AXI queue weight. */
} semc_config_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name SEMC Initialization and De-initialization
 * @{
 */

/*!
 * @brief Gets the SEMC default basic configuration structure.
 *
 * The purpose of this API is to get the default SEMC
 * configure structure for SEMC_Init(). User may use the initialized
 * structure unchanged in SEMC_Init(), or modify some fields of the
 * structure before calling SEMC_Init().
 * Example:
   @code
   semc_config_t config;
   SEMC_GetDefaultConfig(&config);
   @endcode
 * @param config The SEMC configuration structure pointer.
 */
void SEMC_GetDefaultConfig(semc_config_t *config);

/*!
 * @brief Initializes SEMC.
 * This function ungates the SEMC clock and initializes SEMC.
 * This function must be called before calling any other SEMC driver functions.
 *
 * @param base SEMC peripheral base address.
 * @param configure The SEMC configuration structure pointer.
 */
void SEMC_Init(SEMC_Type *base, semc_config_t *configure);

/*!
 * @brief Deinitializes the SEMC module and gates the clock.
 *
 * This function gates the SEMC clock. As a result, the SEMC module doesn't work after
 * calling this function, for some IDE, calling this API may cause the next downloading
 * operation failed. so, please call this API cautiously. Additional, users can
 * using "#define FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL (1)" to disable the clock control
 * operation in drivers.
 *
 * @param base SEMC peripheral base address.
 */
void SEMC_Deinit(SEMC_Type *base);

/* @} */

/*!
 * @name SEMC Configuration Operation For Each Memory Type
 * @{
 */

/*!
 * @brief Configures SDRAM controller in SEMC.
 *
 * @param base SEMC peripheral base address.
 * @param cs The chip selection.
 * @param config The sdram configuration.
 * @param clkSrc_Hz The SEMC clock frequency.
 */
status_t SEMC_ConfigureSDRAM(SEMC_Type *base, semc_sdram_cs_t cs, semc_sdram_config_t *config, uint32_t clkSrc_Hz);

/*!
 * @brief Configures NAND controller in SEMC.
 *
 * @param base SEMC peripheral base address.
 * @param config The nand configuration.
 * @param clkSrc_Hz The SEMC clock frequency.
 */
status_t SEMC_ConfigureNAND(SEMC_Type *base, semc_nand_config_t *config, uint32_t clkSrc_Hz);

/*!
 * @brief Configures NOR controller in SEMC.
 *
 * @param base SEMC peripheral base address.
 * @param config The nor configuration.
 * @param clkSrc_Hz The SEMC clock frequency.
 */
status_t SEMC_ConfigureNOR(SEMC_Type *base, semc_nor_config_t *config, uint32_t clkSrc_Hz);

/*!
 * @brief Configures SRAM controller in SEMC.
 *
 * @param base SEMC peripheral base address.
 * @param config The sram configuration.
 * @param clkSrc_Hz The SEMC clock frequency.
 */
status_t SEMC_ConfigureSRAM(SEMC_Type *base, semc_sram_config_t *config, uint32_t clkSrc_Hz);

/*!
 * @brief Configures DBI controller in SEMC.
 *
 * @param base SEMC peripheral base address.
 * @param config The dbi configuration.
 * @param clkSrc_Hz The SEMC clock frequency.
 */
status_t SEMC_ConfigureDBI(SEMC_Type *base, semc_dbi_config_t *config, uint32_t clkSrc_Hz);

/* @} */

/*!
 * @name SEMC Interrupt Operation
 * @{
 */

/*!
 * @brief Enables the SEMC interrupt.
 *
 * This function enables the SEMC interrupts according to the provided mask. The mask
 * is a logical OR of enumeration members. See @ref semc_interrupt_enable_t.
 * For example, to enable the IP command done and error interrupt, do the following.
 * @code
 *     SEMC_EnableInterrupts(ENET, kSEMC_IPCmdDoneInterrupt | kSEMC_IPCmdErrInterrupt);
 * @endcode
 *
 * @param base  SEMC peripheral base address.
 * @param mask  SEMC interrupts to enable. This is a logical OR of the
 *             enumeration :: semc_interrupt_enable_t.
 */
static inline void SEMC_EnableInterrupts(SEMC_Type *base, uint32_t mask)
{
    base->INTEN |= mask;
}

/*!
 * @brief Disables the SEMC interrupt.
 *
 * This function disables the SEMC interrupts according to the provided mask. The mask
 * is a logical OR of enumeration members. See @ref semc_interrupt_enable_t.
 * For example, to disable the IP command done and error interrupt, do the following.
 * @code
 *     SEMC_DisableInterrupts(ENET, kSEMC_IPCmdDoneInterrupt | kSEMC_IPCmdErrInterrupt);
 * @endcode
 *
 * @param base  SEMC peripheral base address.
 * @param mask  SEMC interrupts to disable. This is a logical OR of the
 *             enumeration :: semc_interrupt_enable_t.
 */
static inline void SEMC_DisableInterrupts(SEMC_Type *base, uint32_t mask)
{
    base->INTEN &= ~mask;
}

/*!
 * @brief Gets the SEMC status.
 *
 * This function gets the SEMC interrupts event status.
 * User can use the a logical OR of enumeration member as a mask.
 * See @ref semc_interrupt_enable_t.
 *
 * @param base  SEMC peripheral base address.
 * @return status flag, use status flag in semc_interrupt_enable_t to get the related status.
 */
static inline bool SEMC_GetStatusFlag(SEMC_Type *base)
{
    return base->INTR;
}

/*!
 * @brief Clears the SEMC status flag state.
 *
 * The following status register flags can be cleared SEMC interrupt status.
 *
 * @param base SEMC base pointer
 * @param mask The status flag mask, a logical OR of enumeration member @ref semc_interrupt_enable_t.
 */
static inline void SEMC_ClearStatusFlags(SEMC_Type *base, uint32_t mask)
{
    base->INTR |= mask;
}

/* @} */

/*!
 * @name SEMC Memory Access Operation
 * @{
 */

/*!
 * @brief Check if SEMC is in idle.
 *
 * @param base  SEMC peripheral base address.
 * @return  True SEMC is in idle, false is not in idle.
 */
static inline bool SEMC_IsInIdle(SEMC_Type *base)
{
    return (base->STS0 & SEMC_STS0_IDLE_MASK) ? true : false;
}

/*!
 * @brief SEMC IP command access.
 *
 * @param base  SEMC peripheral base address.
 * @param type  SEMC memory type. refer to "semc_mem_type_t"
 * @param address  SEMC device address.
 * @param command  SEMC IP command.
 * For NAND device, we should use the SEMC_BuildNandIPCommand to get the right nand command.
 * For NOR/DBI device, take refer to "semc_ipcmd_nor_dbi_t".
 * For SRAM device, take refer to "semc_ipcmd_sram_t".
 * For SDRAM device, take refer to "semc_ipcmd_sdram_t".
 * @param write  Data for write access.
 * @param read   Data pointer for read data out.
 */
status_t SEMC_SendIPCommand(
    SEMC_Type *base, semc_mem_type_t type, uint32_t address, uint16_t command, uint32_t write, uint32_t *read);

/*!
 * @brief Build SEMC IP command for NAND.
 *
 * This function build SEMC NAND IP command. The command is build of user command code,
 * SEMC address mode and SEMC command mode.
 *
 * @param userCommand  NAND device normal command.
 * @param addrMode  NAND address mode. Refer to "semc_ipcmd_nand_addrmode_t".
 * @param cmdMode   NAND command mode. Refer to "semc_ipcmd_nand_cmdmode_t".
 */
static inline uint16_t SEMC_BuildNandIPCommand(uint8_t userCommand,
                                               semc_ipcmd_nand_addrmode_t addrMode,
                                               semc_ipcmd_nand_cmdmode_t cmdMode)
{
    return (uint16_t)((uint16_t)userCommand << 8) | (uint16_t)(addrMode << 4) | ((uint8_t)cmdMode & 0x0Fu);
}

/*!
 * @brief Check if the NAND device is ready.
 *
 * @param base  SEMC peripheral base address.
 * @return  True NAND is ready, false NAND is not ready.
 */
static inline bool SEMC_IsNandReady(SEMC_Type *base)
{
    return (base->STS0 & SEMC_STS0_NARDY_MASK) ? true : false;
}

/*!
 * @brief SEMC NAND device memory write through IP command.
 *
 * @param base  SEMC peripheral base address.
 * @param address  SEMC NAND device address.
 * @param data  Data for write access.
 * @param size_bytes   Data length.
 */
status_t SEMC_IPCommandNandWrite(SEMC_Type *base, uint32_t address, uint8_t *data, uint32_t size_bytes);

/*!
 * @brief SEMC NAND device memory read through IP command.
 *
 * @param base  SEMC peripheral base address.
 * @param address  SEMC NAND device address.
 * @param data  Data pointer for data read out.
 * @param size_bytes   Data length.
 */
status_t SEMC_IPCommandNandRead(SEMC_Type *base, uint32_t address, uint8_t *data, uint32_t size_bytes);

/*!
 * @brief SEMC NOR device memory write through IP command.
 *
 * @param base  SEMC peripheral base address.
 * @param address  SEMC NOR device address.
 * @param data  Data for write access.
 * @param size_bytes   Data length.
 */
status_t SEMC_IPCommandNorWrite(SEMC_Type *base, uint32_t address, uint8_t *data, uint32_t size_bytes);

/*!
 * @brief SEMC NOR device memory read through IP command.
 *
 * @param base  SEMC peripheral base address.
 * @param address  SEMC NOR device address.
 * @param data  Data pointer for data read out.
 * @param size_bytes   Data length.
 */
status_t SEMC_IPCommandNorRead(SEMC_Type *base, uint32_t address, uint8_t *data, uint32_t size_bytes);

/* @} */

#if defined(__cplusplus)
}
#endif

/*! @}*/

#endif /* _FSL_SEMC_H_*/
