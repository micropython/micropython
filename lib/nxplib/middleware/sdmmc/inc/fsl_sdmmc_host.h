/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_SDMMC_HOST_H
#define _FSL_SDMMC_HOST_H

#include "fsl_common.h"
#include "board.h"
#if defined(FSL_FEATURE_SOC_SDHC_COUNT) && FSL_FEATURE_SOC_SDHC_COUNT > 0U
#include "fsl_sdhc.h"
#elif defined(FSL_FEATURE_SOC_SDIF_COUNT) && FSL_FEATURE_SOC_SDIF_COUNT > 0U
#include "fsl_sdif.h"
#elif defined(FSL_FEATURE_SOC_USDHC_COUNT) && FSL_FEATURE_SOC_USDHC_COUNT > 0U
#include "fsl_usdhc.h"
#endif

/*!
 * @addtogroup SDMMCHOST
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Common definition for support and not support macro */
#define SDMMCHOST_NOT_SUPPORT 0U /*!< use this define to indicate the host not support feature*/
#define SDMMCHOST_SUPPORT 1U     /*!< use this define to indicate the host support feature*/

/* Common definition for board support SDR104/HS200/HS400 frequency */
/* SDR104 mode freq */
#if defined BOARD_SD_HOST_SUPPORT_SDR104_FREQ
#define SDMMCHOST_SUPPORT_SDR104_FREQ BOARD_SD_HOST_SUPPORT_SDR104_FREQ
#else
#define SDMMCHOST_SUPPORT_SDR104_FREQ SD_CLOCK_208MHZ
#endif
/* HS200 mode freq */
#if defined BOARD_SD_HOST_SUPPORT_HS200_FREQ
#define SDMMCHOST_SUPPORT_HS200_FREQ BOARD_SD_HOST_SUPPORT_HS200_FREQ
#else
#define SDMMCHOST_SUPPORT_HS200_FREQ MMC_CLOCK_HS200
#endif
/* HS400 mode freq */
#if defined BOARD_SD_HOST_SUPPORT_HS400_FREQ
#define SDMMCHOST_SUPPORT_HS400_FREQ BOARD_SD_HOST_SUPPORT_HS400_FREQ
#else
#define SDMMCHOST_SUPPORT_HS400_FREQ MMC_CLOCK_HS400
#endif

/* Common definition for SDMMCHOST transfer complete timeout */
#define SDMMCHOST_TRANSFER_COMPLETE_TIMEOUT (500U)
/* Common definition for card detect timeout */
#define SDMMCHOST_CARD_DETECT_TIMEOUT (~0U)

/* Common definition for IRQ */
#if defined(__CORTEX_M)
#define SDMMCHOST_SET_IRQ_PRIORITY(id, priority) (NVIC_SetPriority(id, priority))
#else
#define SDMMCHOST_SET_IRQ_PRIORITY(id, priority) (GIC_SetPriority(id, priority))
#endif

#define SDMMCHOST_ENABLE_IRQ(id) (EnableIRQ(id))

/*********************************************************SDHC**********************************************************/
#if (defined(FSL_FEATURE_SOC_SDHC_COUNT) && (FSL_FEATURE_SOC_SDHC_COUNT > 0U))

/*define host baseaddr ,clk freq, IRQ number*/
#define MMC_HOST_BASEADDR BOARD_SDHC_BASEADDR
#define MMC_HOST_CLK_FREQ BOARD_SDHC_CLK_FREQ
#define MMC_HOST_IRQ BOARD_SDHC_IRQ
#define SD_HOST_BASEADDR BOARD_SDHC_BASEADDR
#define SD_HOST_CLK_FREQ BOARD_SDHC_CLK_FREQ
#define SD_HOST_IRQ BOARD_SDHC_IRQ

/* define for card bus speed/strength cnofig */
#define CARD_BUS_FREQ_50MHZ (0U)
#define CARD_BUS_FREQ_100MHZ0 (0U)
#define CARD_BUS_FREQ_100MHZ1 (0U)
#define CARD_BUS_FREQ_200MHZ (0U)

#define CARD_BUS_STRENGTH_0 (0U)
#define CARD_BUS_STRENGTH_1 (0U)
#define CARD_BUS_STRENGTH_2 (0U)
#define CARD_BUS_STRENGTH_3 (0U)
#define CARD_BUS_STRENGTH_4 (0U)
#define CARD_BUS_STRENGTH_5 (0U)
#define CARD_BUS_STRENGTH_6 (0U)
#define CARD_BUS_STRENGTH_7 (0U)

#define SDMMCHOST_TYPE SDHC_Type
#define SDMMCHOST_CONFIG sdhc_host_t
#define SDMMCHOST_TRANSFER sdhc_transfer_t
#define SDMMCHOST_COMMAND sdhc_command_t
#define SDMMCHOST_DATA sdhc_data_t
#define SDMMCHOST_BUS_WIDTH_TYPE sdhc_data_bus_width_t
#define SDMMCHOST_CAPABILITY sdhc_capability_t
#define SDMMCHOST_BOOT_CONFIG sdhc_boot_config_t

#define CARD_DATA0_STATUS_MASK (kSDHC_Data0LineLevelFlag)
#define CARD_DATA0_NOT_BUSY (kSDHC_Data0LineLevelFlag)
#define CARD_DATA1_STATUS_MASK (kSDHC_Data1LineLevelFlag)
#define CARD_DATA2_STATUS_MASK (kSDHC_Data2LineLevelFlag)
#define CARD_DATA3_STATUS_MASK (kSDHC_Data3LineLevelFlag)

#define kSDMMCHOST_DATABUSWIDTH1BIT kSDHC_DataBusWidth1Bit /*!< 1-bit mode */
#define kSDMMCHOST_DATABUSWIDTH4BIT kSDHC_DataBusWidth4Bit /*!< 4-bit mode */
#define kSDMMCHOST_DATABUSWIDTH8BIT kSDHC_DataBusWidth8Bit /*!< 8-bit mode */

#define SDMMCHOST_STANDARD_TUNING_START (0U) /*!< standard tuning start point */
#define SDMMCHOST_TUINIG_STEP (1U)           /*!< standard tuning step */
#define SDMMCHOST_RETUNING_TIMER_COUNT (4U)  /*!< Re-tuning timer */
#define SDMMCHOST_TUNING_DELAY_MAX (0x7FU)
#define SDMMCHOST_RETUNING_REQUEST (1U)
#define SDMMCHOST_TUNING_ERROR (2U)

/* function pointer define */
#define SDMMCHOST_TRANSFER_FUNCTION sdhc_transfer_function_t
#define GET_SDMMCHOST_CAPABILITY(base, capability) (SDHC_GetCapability(base, capability))
#define GET_SDMMCHOST_STATUS(base) (SDHC_GetPresentStatusFlags(base))
#define SDMMCHOST_SET_CARD_CLOCK(base, sourceClock_HZ, busClock_HZ) (SDHC_SetSdClock(base, sourceClock_HZ, busClock_HZ))
#define SDMMCHOST_SET_CARD_BUS_WIDTH(base, busWidth) (SDHC_SetDataBusWidth(base, busWidth))
#define SDMMCHOST_SEND_CARD_ACTIVE(base, timeout) (SDHC_SetCardActive(base, timeout))
#define SDMMCHOST_SWITCH_VOLTAGE180V(base, enable18v)
#define SDMMCHOST_SWITCH_VOLTAGE120V(base, enable12v)
#define SDMMCHOST_CONFIG_IO_STRENGTH(speed, strength)
#define SDMMCHOST_EXECUTE_STANDARD_TUNING_ENABLE(base, flag)
#define SDMMCHOST_EXECUTE_STANDARD_TUNING_STATUS(base) (0U)
#define SDMMCHOST_EXECUTE_STANDARD_TUNING_RESULT(base) (1U)
#define SDMMCHOST_CONFIG_SD_IO(speed, strength)
#define SDMMCHOST_CONFIG_MMC_IO(speed, strength)
#define SDMMCHOST_ENABLE_DDR_MODE(base, flag, nibblePos)
#define SDMMCHOST_FORCE_SDCLOCK_ON(base, enable)
#define SDMMCHOST_EXECUTE_MANUAL_TUNING_ENABLE(base, flag)
#define SDMMCHOST_ADJUST_MANUAL_TUNING_DELAY(base, delay)
#define SDMMCHOST_AUTO_MANUAL_TUNING_ENABLE(base, flag)
#define SDMMCHOST_ENABLE_CARD_CLOCK(base, enable) (SDHC_EnableSdClock(base, enable))
#define SDMMCHOST_RESET_TUNING(base, timeout)
#define SDMMCHOST_CHECK_TUNING_ERROR(base) (0U)
#define SDMMCHOST_ADJUST_TUNING_DELAY(base, delay)
#define SDMMCHOST_AUTO_STANDARD_RETUNING_TIMER(base)
#define SDMMCHOST_TRANSFER_DATA_ERROR kStatus_SDHC_TransferDataFailed
#define SDMMCHOST_TRANSFER_CMD_ERROR kStatus_SDHC_SendCommandFailed
#define SDMMCHOST_ENABLE_HS400_MODE(base, flag)
#define SDMMCHOST_RESET_STROBE_DLL(base)
#define SDMMCHOST_ENABLE_STROBE_DLL(base, flag)
#define SDMMCHOST_CONFIG_STROBE_DLL(base, delay, updateInterval)
#define SDMMCHOST_GET_STROBE_DLL_STATUS(base)
/* sd card power */
#define SDMMCHOST_INIT_SD_POWER()
#define SDMMCHOST_ENABLE_SD_POWER(enable)
#define SDMMCHOST_SWITCH_VCC_TO_180V()
#define SDMMCHOST_SWITCH_VCC_TO_330V()
/* mmc card power */
#define SDMMCHOST_INIT_MMC_POWER()
#define SDMMCHOST_ENABLE_MMC_POWER(enable)
#define SDMMCHOST_ENABLE_TUNING_FLAG(data)
#define SDMMCHOST_ENABLE_BOOT_FLAG(data)
#define SDMMCHOST_ENABLE_BOOT_CONTINOUS_FLAG(data)
#define SDMMCHOST_GET_HOST_CONFIG_BLOCK_SIZE(config) (0U)
#define SDMMCHOST_GET_HOST_CONFIG_BLOCK_COUNT(config) (0U)
#define SDMMCHOST_GET_HOST_CONFIG_BOOT_MODE(config) (0U)
#define SDMMCHOST_EMPTY_CMD_FLAG(command)
#define SDMMCHOST_CARD_DETECT_GPIO_INTERRUPT_HANDLER BOARD_SDHC_CD_PORT_IRQ_HANDLER
#define SDMMCHOST_CARD_DETECT_IRQ BOARD_SDHC_CD_PORT_IRQ
/* sd card detect through host CD */
#define SDMMCHOST_CARD_DETECT_INSERT_ENABLE(base) (SDHC_EnableInterruptStatus(base, kSDHC_CardInsertionFlag))
#define SDMMCHOST_CARD_DETECT_REMOVE_ENABLE(base) (SDHC_EnableInterruptStatus(base, kSDHC_CardRemovalFlag))
#define SDMMCHOST_CARD_DETECT_INSERT_STATUS(base) (SDHC_GetInterruptStatusFlags(base) & kSDHC_CardInsertionFlag)
#define SDMMCHOST_CARD_DETECT_REMOVE_STATUS(base) (SDHC_GetInterruptStatusFlags(base, kSDHC_CardRemovalFlag))
#define SDMMCHOST_CARD_DETECT_INSERT_INTERRUPT_ENABLE(base) (SDHC_EnableInterruptSignal(base, kSDHC_CardInsertionFlag))
#define SDMMCHOST_CARD_DETECT_INSERT_INTERRUPT_DISABLE(base) \
    (SDHC_DisableInterruptSignal(base, kSDHC_CardInsertionFlag))
#define SDMMCHOST_CARD_DETECT_REMOVE_INTERRUPT_ENABLE(base) (SDHC_EnableInterruptSignal(base, kSDHC_CardRemovalFlag))
#define SDMMCHOST_CARD_DETECT_DATA3_ENABLE(base, flag) (SDHC_CardDetectByData3(base, flag))
#define SDMMCHOST_ENABLE_MMC_BOOT(base, flag)
#define SDMMCHOST_SETMMCBOOTCONFIG(base, config) (SDHC_SetMmcBootConfig(base, config))
/* define card detect pin voltage level when card inserted */
#if defined BOARD_SDHC_CARD_INSERT_CD_LEVEL
#define SDMMCHOST_CARD_INSERT_CD_LEVEL BOARD_SDHC_CARD_INSERT_CD_LEVEL
#else
#define SDMMCHOST_CARD_INSERT_CD_LEVEL (0U)
#endif
#define SDMMCHOST_AUTO_TUNING_ENABLE(base, flag)
#define SDMMCHOST_ENABLE_SDIO_INT(base)                        \
    SDHC_EnableInterruptStatus(base, kSDHC_CardInterruptFlag); \
    SDHC_EnableInterruptSignal(base, kSDHC_CardInterruptFlag)
#define SDMMCHOST_DISABLE_SDIO_INT(base)                        \
    SDHC_DisableInterruptStatus(base, kSDHC_CardInterruptFlag); \
    SDHC_DisableInterruptSignal(base, kSDHC_CardInterruptFlag)

/*! @brief SDHC host capability*/
enum _host_capability
{
    kSDMMCHOST_SupportAdma          = kSDHC_SupportAdmaFlag,
    kSDMMCHOST_SupportHighSpeed     = kSDHC_SupportHighSpeedFlag,
    kSDMMCHOST_SupportDma           = kSDHC_SupportDmaFlag,
    kSDMMCHOST_SupportSuspendResume = kSDHC_SupportSuspendResumeFlag,
    kSDMMCHOST_SupportV330          = kSDHC_SupportV330Flag,
    kSDMMCHOST_SupportV300          = SDMMCHOST_NOT_SUPPORT,
    kSDMMCHOST_SupportV180          = SDMMCHOST_NOT_SUPPORT,
    kSDMMCHOST_SupportV120          = SDMMCHOST_NOT_SUPPORT,
    kSDMMCHOST_Support4BitBusWidth  = kSDHC_Support4BitFlag,
    kSDMMCHOST_Support8BitBusWidth  = kSDHC_Support8BitFlag,
    kSDMMCHOST_SupportDDR50         = SDMMCHOST_NOT_SUPPORT,
    kSDMMCHOST_SupportSDR104        = SDMMCHOST_NOT_SUPPORT,
    kSDMMCHOST_SupportSDR50         = SDMMCHOST_NOT_SUPPORT,
    kSDMMCHOST_SupportHS200         = SDMMCHOST_NOT_SUPPORT,
    kSDMMCHOST_SupportHS400         = SDMMCHOST_NOT_SUPPORT,
};

/* Endian mode. */
#define SDHC_ENDIAN_MODE kSDHC_EndianModeLittle

/* DMA mode */
#define SDHC_DMA_MODE kSDHC_DmaModeAdma2
/* address align */
#define SDMMCHOST_DMA_BUFFER_ADDR_ALIGN (SDHC_ADMA2_ADDRESS_ALIGN)

/* Read/write watermark level. The bigger value indicates DMA has higher read/write performance. */
#define SDHC_READ_WATERMARK_LEVEL (0x80U)
#define SDHC_WRITE_WATERMARK_LEVEL (0x80U)

/* ADMA table length united as word.
 *
 * SD card driver can't support ADMA1 transfer mode currently.
 * One ADMA2 table item occupy two words which can transfer maximum 0xFFFFU bytes one time.
 * The more data to be transferred in one time, the bigger value of SDHC_ADMA_TABLE_WORDS need to be set.
 */
#define SDHC_ADMA_TABLE_WORDS (8U)

/*********************************************************SDIF**********************************************************/
#elif (defined(FSL_FEATURE_SOC_SDIF_COUNT) && (FSL_FEATURE_SOC_SDIF_COUNT > 0U))

/*define host baseaddr ,clk freq, IRQ number*/
#define MMC_HOST_BASEADDR BOARD_SDIF_BASEADDR
#define MMC_HOST_CLK_FREQ BOARD_SDIF_CLK_FREQ
#define MMC_HOST_IRQ BOARD_SDIF_IRQ
#define SD_HOST_BASEADDR BOARD_SDIF_BASEADDR
#define SD_HOST_CLK_FREQ BOARD_SDIF_CLK_FREQ
#define SD_HOST_IRQ BOARD_SDIF_IRQ

/* define for card bus speed/strength cnofig */
#define CARD_BUS_FREQ_50MHZ (0U)
#define CARD_BUS_FREQ_100MHZ0 (0U)
#define CARD_BUS_FREQ_100MHZ1 (0U)
#define CARD_BUS_FREQ_200MHZ (0U)

#define CARD_BUS_STRENGTH_0 (0U)
#define CARD_BUS_STRENGTH_1 (0U)
#define CARD_BUS_STRENGTH_2 (0U)
#define CARD_BUS_STRENGTH_3 (0U)
#define CARD_BUS_STRENGTH_4 (0U)
#define CARD_BUS_STRENGTH_5 (0U)
#define CARD_BUS_STRENGTH_6 (0U)
#define CARD_BUS_STRENGTH_7 (0U)

#define SDMMCHOST_TYPE SDIF_Type
#define SDMMCHOST_CONFIG sdif_host_t
#define SDMMCHOST_TRANSFER sdif_transfer_t
#define SDMMCHOST_COMMAND sdif_command_t
#define SDMMCHOST_DATA sdif_data_t
#define SDMMCHOST_BUS_WIDTH_TYPE sdif_bus_width_t
#define SDMMCHOST_CAPABILITY sdif_capability_t
#define SDMMCHOST_BOOT_CONFIG void

#define CARD_DATA0_STATUS_MASK SDIF_STATUS_DATA_BUSY_MASK
#define CARD_DATA0_NOT_BUSY 0U

#define CARD_DATA1_STATUS_MASK (0U)
#define CARD_DATA2_STATUS_MASK (0U)
#define CARD_DATA3_STATUS_MASK (0U)

#define kSDMMCHOST_DATABUSWIDTH1BIT kSDIF_Bus1BitWidth /*!< 1-bit mode */
#define kSDMMCHOST_DATABUSWIDTH4BIT kSDIF_Bus4BitWidth /*!< 4-bit mode */
#define kSDMMCHOST_DATABUSWIDTH8BIT kSDIF_Bus8BitWidth /*!< 8-bit mode */

#define SDMMCHOST_STANDARD_TUNING_START (0U) /*!< standard tuning start point */
#define SDMMCHOST_TUINIG_STEP (1U)           /*!< standard tuning step */
#define SDMMCHOST_RETUNING_TIMER_COUNT (4U)  /*!< Re-tuning timer */
#define SDMMCHOST_TUNING_DELAY_MAX (0x7FU)
#define SDMMCHOST_RETUNING_REQUEST (1U)
#define SDMMCHOST_TUNING_ERROR (2U)
/* function pointer define */
#define SDMMCHOST_TRANSFER_FUNCTION sdif_transfer_function_t
#define GET_SDMMCHOST_CAPABILITY(base, capability) (SDIF_GetCapability(base, capability))
#define GET_SDMMCHOST_STATUS(base) (SDIF_GetControllerStatus(base))
#define SDMMCHOST_SET_CARD_CLOCK(base, sourceClock_HZ, busClock_HZ) \
    (SDIF_SetCardClock(base, sourceClock_HZ, busClock_HZ))
#define SDMMCHOST_SET_CARD_BUS_WIDTH(base, busWidth) (SDIF_SetCardBusWidth(base, busWidth))
#define SDMMCHOST_SEND_CARD_ACTIVE(base, timeout) (SDIF_SendCardActive(base, timeout))
#define SDMMCHOST_SWITCH_VOLTAGE180V(base, enable18v)
#define SDMMCHOST_SWITCH_VOLTAGE120V(base, enable12v)
#define SDMMCHOST_CONFIG_IO_STRENGTH(speed, strength)
#define SDMMCHOST_EXECUTE_STANDARD_TUNING_ENABLE(base, flag)
#define SDMMCHOST_EXECUTE_STANDARD_TUNING_STATUS(base) (0U)
#define SDMMCHOST_EXECUTE_STANDARD_TUNING_RESULT(base) (1U)
#define SDMMCHOST_CONFIG_SD_IO(speed, strength)
#define SDMMCHOST_CONFIG_MMC_IO(speed, strength)
#define SDMMCHOST_ENABLE_DDR_MODE(base, flag, nibblePos)
#define SDMMCHOST_FORCE_SDCLOCK_ON(base, enable)
#define SDMMCHOST_EXECUTE_MANUAL_TUNING_ENABLE(base, flag)
#define SDMMCHOST_ADJUST_MANUAL_TUNING_DELAY(base, delay)
#define SDMMCHOST_AUTO_MANUAL_TUNING_ENABLE(base, flag)
#define SDMMCHOST_ENABLE_CARD_CLOCK(base, enable) (SDIF_EnableCardClock(base, enable))
#define SDMMCHOST_RESET_TUNING(base, timeout)
#define SDMMCHOST_CHECK_TUNING_ERROR(base) (0U)
#define SDMMCHOST_ADJUST_TUNING_DELAY(base, delay)
#define SDMMCHOST_AUTO_STANDARD_RETUNING_TIMER(base)

#define SDMMCHOST_ENABLE_HS400_MODE(base, flag)
#define SDMMCHOST_RESET_STROBE_DLL(base)
#define SDMMCHOST_ENABLE_STROBE_DLL(base, flag)
#define SDMMCHOST_CONFIG_STROBE_DLL(base, delay, updateInterval)
#define SDMMCHOST_GET_STROBE_DLL_STATUS(base)
/* sd card power */
#define SDMMCHOST_INIT_SD_POWER()
#define SDMMCHOST_ENABLE_SD_POWER(enable)
#define SDMMCHOST_SWITCH_VCC_TO_180V()
#define SDMMCHOST_SWITCH_VCC_TO_330V()
/* mmc card power */
#define SDMMCHOST_INIT_MMC_POWER()
#define SDMMCHOST_ENABLE_MMC_POWER(enable)
#define SDMMCHOST_ENABLE_TUNING_FLAG(data)
#define SDMMCHOST_ENABLE_MMC_BOOT(base, flag)
#define SDMMCHOST_SETMMCBOOTCONFIG(base, config)
#define SDMMCHOST_ENABLE_BOOT_FLAG(data)
#define SDMMCHOST_ENABLE_BOOT_CONTINOUS_FLAG(data)
#define SDMMCHOST_GET_HOST_CONFIG_BLOCK_SIZE(config) (0U)
#define SDMMCHOST_GET_HOST_CONFIG_BLOCK_COUNT(config) (0U)
#define SDMMCHOST_GET_HOST_CONFIG_BOOT_MODE(config) (0U)
#define SDMMCHOST_EMPTY_CMD_FLAG(command)
#define SDMMCHOST_CARD_DETECT_STATUS() BOARD_SDIF_CD_STATUS()
#define SDMMCHOST_CARD_DETECT_INIT() BOARD_SDIF_CD_GPIO_INIT()
#define SDMMCHOST_CARD_DETECT_INTERRUPT_STATUS() BOARD_SDIF_CD_INTERRUPT_STATUS()
#define SDMMCHOST_CARD_DETECT_INTERRUPT_CLEAR(flag) BOARD_SDIF_CD_CLEAR_INTERRUPT(flag)
#define SDMMCHOST_CARD_DETECT_GPIO_INTERRUPT_HANDLER BOARD_SDIF_CD_PORT_IRQ_HANDLER
#define SDMMCHOST_CARD_DETECT_IRQ BOARD_SDIF_CD_PORT_IRQ
#define SDMMCHOST_TRANSFER_DATA_ERROR kStatus_SDIF_DataTransferFail
#define SDMMCHOST_TRANSFER_CMD_ERROR kStatus_SDIF_SendCmdFail
/* define card detect pin voltage level when card inserted */
#if defined BOARD_SDIF_CARD_INSERT_CD_LEVEL
#define SDMMCHOST_CARD_INSERT_CD_LEVEL BOARD_SDIF_CARD_INSERT_CD_LEVEL
#else
#define SDMMCHOST_CARD_INSERT_CD_LEVEL (0U)
#endif
#define SDMMCHOST_AUTO_TUNING_ENABLE(base, flag)
/* sd card detect through host CD */
#define SDMMCHOST_CARD_DETECT_INSERT_ENABLE(base) (SDIF_EnableInterrupt(base, kSDIF_CardDetect))
#define SDMMCHOST_CARD_DETECT_INSERT_STATUS(base, data3) (SDIF_DetectCardInsert(base, data3))
#define SDMMCHOST_ENABLE_SDIO_INT(base)
#define SDMMCHOST_DISABLE_SDIO_INT(base)
/*! @brief SDIF host capability*/
enum _host_capability
{
    kSDMMCHOST_SupportHighSpeed     = kSDIF_SupportHighSpeedFlag,
    kSDMMCHOST_SupportDma           = kSDIF_SupportDmaFlag,
    kSDMMCHOST_SupportSuspendResume = kSDIF_SupportSuspendResumeFlag,
    kSDMMCHOST_SupportV330          = kSDIF_SupportV330Flag,
    kSDMMCHOST_SupportV300          = SDMMCHOST_NOT_SUPPORT,
    kSDMMCHOST_SupportV180          = SDMMCHOST_NOT_SUPPORT,
    kSDMMCHOST_SupportV120          = SDMMCHOST_NOT_SUPPORT,
    kSDMMCHOST_Support4BitBusWidth  = kSDIF_Support4BitFlag,
    kSDMMCHOST_Support8BitBusWidth =
        SDMMCHOST_NOT_SUPPORT, /* mask the 8 bit here,user can change depend on your board */
    kSDMMCHOST_SupportDDR50  = SDMMCHOST_NOT_SUPPORT,
    kSDMMCHOST_SupportSDR104 = SDMMCHOST_NOT_SUPPORT,
    kSDMMCHOST_SupportSDR50  = SDMMCHOST_NOT_SUPPORT,
    kSDMMCHOST_SupportHS200  = SDMMCHOST_NOT_SUPPORT,
    kSDMMCHOST_SupportHS400  = SDMMCHOST_NOT_SUPPORT,

};

/*! @brief DMA table length united as word
 * One dma table item occupy four words which can transfer maximum 2*8188 bytes in dual DMA mode
 * and 8188 bytes in chain mode
 * The more data to be transferred in one time, the bigger value of SDHC_ADMA_TABLE_WORDS need to be set.
 * user need check the DMA descriptor table lenght if bigger enough.
 */
#define SDIF_DMA_TABLE_WORDS (0x40U)
/* address align */
#define SDMMCHOST_DMA_BUFFER_ADDR_ALIGN (4U)

/*********************************************************USDHC**********************************************************/
#elif (defined(FSL_FEATURE_SOC_USDHC_COUNT) && (FSL_FEATURE_SOC_USDHC_COUNT > 0U))

/*define host baseaddr ,clk freq, IRQ number*/
#define MMC_HOST_BASEADDR BOARD_MMC_HOST_BASEADDR
#define MMC_HOST_CLK_FREQ BOARD_MMC_HOST_CLK_FREQ
#define MMC_HOST_IRQ BOARD_MMC_HOST_IRQ
#define SD_HOST_BASEADDR BOARD_SD_HOST_BASEADDR
#define SD_HOST_CLK_FREQ BOARD_SD_HOST_CLK_FREQ
#define SD_HOST_IRQ BOARD_SD_HOST_IRQ

#define SDMMCHOST_TYPE USDHC_Type
#define SDMMCHOST_CONFIG usdhc_host_t
#define SDMMCHOST_TRANSFER usdhc_transfer_t
#define SDMMCHOST_COMMAND usdhc_command_t
#define SDMMCHOST_DATA usdhc_data_t
#define SDMMCHOST_BOOT_CONFIG usdhc_boot_config_t
#define CARD_DATA0_STATUS_MASK (kUSDHC_Data0LineLevelFlag)
#define CARD_DATA1_STATUS_MASK (kUSDHC_Data1LineLevelFlag)
#define CARD_DATA2_STATUS_MASK (kUSDHC_Data2LineLevelFlag)
#define CARD_DATA3_STATUS_MASK (kUSDHC_Data3LineLevelFlag)
#define CARD_DATA0_NOT_BUSY (kUSDHC_Data0LineLevelFlag)

#define SDMMCHOST_BUS_WIDTH_TYPE usdhc_data_bus_width_t
#define SDMMCHOST_CAPABILITY usdhc_capability_t

#define kSDMMCHOST_DATABUSWIDTH1BIT kUSDHC_DataBusWidth1Bit /*!< 1-bit mode */
#define kSDMMCHOST_DATABUSWIDTH4BIT kUSDHC_DataBusWidth4Bit /*!< 4-bit mode */
#define kSDMMCHOST_DATABUSWIDTH8BIT kUSDHC_DataBusWidth8Bit /*!< 8-bit mode */

#if defined BOARD_SDMMCHOST_TUNING_START_DELAY_POINT
#define SDMMCHOST_STANDARD_TUNING_START BOARD_SDMMCHOST_TUNING_START_DELAY_POINT /*!< standard tuning start point */
#else
#define SDMMCHOST_STANDARD_TUNING_START (10U) /*!< standard tuning start point */
#endif

#define SDMMCHOST_TUINIG_STEP (2U)          /*!< standard tuning step */
#define SDMMCHOST_RETUNING_TIMER_COUNT (0U) /*!< Re-tuning timer */
#define SDMMCHOST_TUNING_DELAY_MAX (0x7FU)
#define SDMMCHOST_RETUNING_REQUEST kStatus_USDHC_ReTuningRequest
#define SDMMCHOST_TUNING_ERROR kStatus_USDHC_TuningError
#define SDMMCHOST_TRANSFER_DATA_ERROR kStatus_USDHC_TransferDataFailed
#define SDMMCHOST_TRANSFER_CMD_ERROR kStatus_USDHC_SendCommandFailed
/* define for card bus speed/strength cnofig */
#define CARD_BUS_FREQ_50MHZ (0U)
#define CARD_BUS_FREQ_100MHZ0 (1U)
#define CARD_BUS_FREQ_100MHZ1 (2U)
#define CARD_BUS_FREQ_200MHZ (3U)

#define CARD_BUS_STRENGTH_0 (0U)
#define CARD_BUS_STRENGTH_1 (1U)
#define CARD_BUS_STRENGTH_2 (2U)
#define CARD_BUS_STRENGTH_3 (3U)
#define CARD_BUS_STRENGTH_4 (4U)
#define CARD_BUS_STRENGTH_5 (5U)
#define CARD_BUS_STRENGTH_6 (6U)
#define CARD_BUS_STRENGTH_7 (7U)

#define SDMMCHOST_STROBE_DLL_DELAY_TARGET (7U)
#define SDMMCHOST_STROBE_DLL_DELAY_UPDATE_INTERVAL (4U)

/* function pointer define */
#define SDMMCHOST_TRANSFER_FUNCTION usdhc_transfer_function_t
#define GET_SDMMCHOST_CAPABILITY(base, capability) (USDHC_GetCapability(base, capability))
#define GET_SDMMCHOST_STATUS(base) (USDHC_GetPresentStatusFlags(base))
#define SDMMCHOST_SET_CARD_CLOCK(base, sourceClock_HZ, busClock_HZ) \
    (USDHC_SetSdClock(base, sourceClock_HZ, busClock_HZ))
#define SDMMCHOST_ENABLE_CARD_CLOCK(base, enable)
#define SDMMCHOST_FORCE_SDCLOCK_ON(base, enable) (USDHC_ForceClockOn(base, enable))
#define SDMMCHOST_SET_CARD_BUS_WIDTH(base, busWidth) (USDHC_SetDataBusWidth(base, busWidth))
#define SDMMCHOST_SEND_CARD_ACTIVE(base, timeout) (USDHC_SetCardActive(base, timeout))
#define SDMMCHOST_SWITCH_VOLTAGE180V(base, enable18v) (UDSHC_SelectVoltage(base, enable18v))
#define SDMMCHOST_SWITCH_VOLTAGE120V(base, enable12v)
#define SDMMCHOST_CONFIG_SD_IO(speed, strength) BOARD_SD_Pin_Config(speed, strength)
#define SDMMCHOST_CONFIG_MMC_IO(speed, strength) BOARD_MMC_Pin_Config(speed, strength)
#define SDMMCHOST_SWITCH_VCC_TO_180V()
#define SDMMCHOST_SWITCH_VCC_TO_330V()

#if defined(FSL_FEATURE_USDHC_HAS_SDR50_MODE) && (!FSL_FEATURE_USDHC_HAS_SDR50_MODE)
#define SDMMCHOST_EXECUTE_STANDARD_TUNING_STATUS(base) (0U)
#define SDMMCHOST_EXECUTE_STANDARD_TUNING_RESULT(base) (1U)
#define SDMMCHOST_AUTO_STANDARD_RETUNING_TIMER(base)
#define SDMMCHOST_EXECUTE_STANDARD_TUNING_ENABLE(base, flag)
#define SDMMCHOST_CHECK_TUNING_ERROR(base) (0U)
#define SDMMCHOST_ADJUST_TUNING_DELAY(base, delay)
#else
#define SDMMCHOST_EXECUTE_STANDARD_TUNING_ENABLE(base, flag) \
    (USDHC_EnableStandardTuning(base, SDMMCHOST_STANDARD_TUNING_START, SDMMCHOST_TUINIG_STEP, flag))
#define SDMMCHOST_EXECUTE_STANDARD_TUNING_STATUS(base) (USDHC_GetExecuteStdTuningStatus(base))
#define SDMMCHOST_EXECUTE_STANDARD_TUNING_RESULT(base) (USDHC_CheckStdTuningResult(base))
#define SDMMCHOST_AUTO_STANDARD_RETUNING_TIMER(base) (USDHC_SetRetuningTimer(base, SDMMCHOST_RETUNING_TIMER_COUNT))
#define SDMMCHOST_EXECUTE_MANUAL_TUNING_ENABLE(base, flag) (USDHC_EnableManualTuning(base, flag))
#define SDMMCHOST_ADJUST_TUNING_DELAY(base, delay) (USDHC_AdjustDelayForManualTuning(base, delay))
#define SDMMCHOST_AUTO_TUNING_ENABLE(base, flag) (USDHC_EnableAutoTuning(base, flag))
#define SDMMCHOST_CHECK_TUNING_ERROR(base) (USDHC_CheckTuningError(base))
#endif

#define SDMMCHOST_AUTO_TUNING_CONFIG(base) (USDHC_EnableAutoTuningForCmdAndData(base))
#define SDMMCHOST_RESET_TUNING(base, timeout)                                                      \
    {                                                                                              \
        (USDHC_Reset(base, kUSDHC_ResetTuning | kUSDHC_ResetData | kUSDHC_ResetCommand, timeout)); \
    }

#define SDMMCHOST_ENABLE_DDR_MODE(base, flag, nibblePos) (USDHC_EnableDDRMode(base, flag, nibblePos))

#if FSL_FEATURE_USDHC_HAS_HS400_MODE
#define SDMMCHOST_ENABLE_HS400_MODE(base, flag) (USDHC_EnableHS400Mode(base, flag))
#define SDMMCHOST_RESET_STROBE_DLL(base) (USDHC_ResetStrobeDLL(base))
#define SDMMCHOST_ENABLE_STROBE_DLL(base, flag) (USDHC_EnableStrobeDLL(base, flag))
#define SDMMCHOST_CONFIG_STROBE_DLL(base, delay, updateInterval) (USDHC_ConfigStrobeDLL(base, delay, updateInterval))
#define SDMMCHOST_GET_STROBE_DLL_STATUS (base)(USDHC_GetStrobeDLLStatus(base))
#else
#define SDMMCHOST_ENABLE_HS400_MODE(base, flag)
#define SDMMCHOST_RESET_STROBE_DLL(base)
#define SDMMCHOST_ENABLE_STROBE_DLL(base, flag)
#define SDMMCHOST_CONFIG_STROBE_DLL(base, delay, updateInterval)
#define SDMMCHOST_GET_STROBE_DLL_STATUS(base)
#endif

#define SDMMCHOST_ENABLE_MMC_BOOT(base, flag) (USDHC_EnableMmcBoot(base, flag))
#define SDMMCHOST_SETMMCBOOTCONFIG(base, config) (USDHC_SetMmcBootConfig(base, config))
/* sd card power */
#define SDMMCHOST_INIT_SD_POWER() BOARD_USDHC_SDCARD_POWER_CONTROL_INIT()
#define SDMMCHOST_ENABLE_SD_POWER(enable) BOARD_USDHC_SDCARD_POWER_CONTROL(enable)
/* mmc card power */
#define SDMMCHOST_INIT_MMC_POWER() BOARD_USDHC_MMCCARD_POWER_CONTROL_INIT()
#define SDMMCHOST_ENABLE_MMC_POWER(enable) BOARD_USDHC_MMCCARD_POWER_CONTROL(enable)
/* sd card detect through gpio */
#define SDMMCHOST_CARD_DETECT_GPIO_STATUS() BOARD_USDHC_CD_STATUS()
#define SDMMCHOST_CARD_DETECT_GPIO_INIT() BOARD_USDHC_CD_GPIO_INIT()
#define SDMMCHOST_CARD_DETECT_GPIO_INTERRUPT_STATUS() BOARD_USDHC_CD_INTERRUPT_STATUS()
#define SDMMCHOST_CARD_DETECT_GPIO_INTERRUPT_STATUS_CLEAR(flag) BOARD_USDHC_CD_CLEAR_INTERRUPT(flag)
#define SDMMCHOST_CARD_DETECT_GPIO_INTERRUPT_HANDLER BOARD_USDHC_CD_PORT_IRQ_HANDLER
#define SDMMCHOST_CARD_DETECT_GPIO_IRQ BOARD_USDHC_CD_PORT_IRQ
/* sd card detect through host CD */
#define SDMMCHOST_CARD_DETECT_INSERT_ENABLE(base) (USDHC_EnableInterruptStatus(base, kUSDHC_CardInsertionFlag))
#define SDMMCHOST_CARD_DETECT_REMOVE_ENABLE(base) (USDHC_EnableInterruptStatus(base, kUSDHC_CardRemovalFlag))
#define SDMMCHOST_CARD_DETECT_INSERT_STATUS(base) (USDHC_DetectCardInsert(base))
#define SDMMCHOST_CARD_DETECT_REMOVE_STATUS(base) (USDHC_GetInterruptStatusFlags(base, kUSDHC_CardRemovalFlag))
#define SDMMCHOST_CARD_DETECT_INSERT_INTERRUPT_ENABLE(base) \
    (USDHC_EnableInterruptSignal(base, kUSDHC_CardInsertionFlag))
#define SDMMCHOST_CARD_DETECT_REMOVE_INTERRUPT_ENABLE(base) (USDHC_EnableInterruptSignal(base, kUSDHC_CardRemovalFlag))
#define SDMMCHOST_CARD_DETECT_DATA3_ENABLE(base, flag) (USDHC_CardDetectByData3(base, flag))

/* define card detect pin voltage level when card inserted */
#if defined BOARD_USDHC_CARD_INSERT_CD_LEVEL
#define SDMMCHOST_CARD_INSERT_CD_LEVEL BOARD_USDHC_CARD_INSERT_CD_LEVEL
#else
#define SDMMCHOST_CARD_INSERT_CD_LEVEL (0U)
#endif
#define SDMMCHOST_ENABLE_TUNING_FLAG(data) (data.dataType = kUSDHC_TransferDataTuning)
#define SDMMCHOST_ENABLE_BOOT_FLAG(data) (data.dataType = kUSDHC_TransferDataBoot)
#define SDMMCHOST_ENABLE_BOOT_CONTINOUS_FLAG(data) (data.dataType = kUSDHC_TransferDataBootcontinous)
#define SDMMCHOST_GET_HOST_CONFIG_BLOCK_SIZE(config) (config->blockSize)
#define SDMMCHOST_GET_HOST_CONFIG_BLOCK_COUNT(config) (config->blockCount)
#define SDMMCHOST_GET_HOST_CONFIG_BOOT_MODE(config) (config->bootMode)
#define SDMMCHOST_EMPTY_CMD_FLAG(command) (command.type = kCARD_CommandTypeEmpty)
#define SDMMCHOST_ENABLE_SDIO_INT(base)                          \
    USDHC_EnableInterruptStatus(base, kUSDHC_CardInterruptFlag); \
    USDHC_EnableInterruptSignal(base, kUSDHC_CardInterruptFlag)
#define SDMMCHOST_DISABLE_SDIO_INT(base)                          \
    USDHC_DisableInterruptStatus(base, kUSDHC_CardInterruptFlag); \
    USDHC_DisableInterruptSignal(base, kUSDHC_CardInterruptFlag)
/*! @brief USDHC host capability*/
enum _host_capability
{
    kSDMMCHOST_SupportAdma          = kUSDHC_SupportAdmaFlag,
    kSDMMCHOST_SupportHighSpeed     = kUSDHC_SupportHighSpeedFlag,
    kSDMMCHOST_SupportDma           = kUSDHC_SupportDmaFlag,
    kSDMMCHOST_SupportSuspendResume = kUSDHC_SupportSuspendResumeFlag,
    kSDMMCHOST_SupportV330          = kUSDHC_SupportV330Flag, /* this define should depend on your board config */
    kSDMMCHOST_SupportV300          = kUSDHC_SupportV300Flag, /* this define should depend on your board config */
#if defined(BOARD_SD_SUPPORT_180V) && !BOARD_SD_SUPPORT_180V
    kSDMMCHOST_SupportV180          = SDMMCHOST_NOT_SUPPORT,  /* this define should depend on you board config */
#else
    kSDMMCHOST_SupportV180         = kUSDHC_SupportV180Flag, /* this define should depend on you board config */
#endif
    kSDMMCHOST_SupportV120          = SDMMCHOST_NOT_SUPPORT,
    kSDMMCHOST_Support4BitBusWidth  = kUSDHC_Support4BitFlag,
#if defined(BOARD_MMC_SUPPORT_8BIT_BUS)
#if BOARD_MMC_SUPPORT_8BIT_BUS
    kSDMMCHOST_Support8BitBusWidth  = kUSDHC_Support8BitFlag,
#else
    kSDMMCHOST_Support8BitBusWidth = SDMMCHOST_NOT_SUPPORT,
#endif
#else
    kSDMMCHOST_Support8BitBusWidth = kUSDHC_Support8BitFlag,
#endif
    kSDMMCHOST_SupportDDR50         = kUSDHC_SupportDDR50Flag,
    kSDMMCHOST_SupportSDR104        = kUSDHC_SupportSDR104Flag,
    kSDMMCHOST_SupportSDR50         = kUSDHC_SupportSDR50Flag,
    kSDMMCHOST_SupportHS200         = kUSDHC_SupportSDR104Flag,
#if FSL_FEATURE_USDHC_HAS_HS400_MODE
    kSDMMCHOST_SupportHS400         = SDMMCHOST_SUPPORT
#else
    kSDMMCHOST_SupportHS400        = SDMMCHOST_NOT_SUPPORT,
#endif
};

/* Endian mode. */
#define USDHC_ENDIAN_MODE kUSDHC_EndianModeLittle

/* DMA mode */
#define USDHC_DMA_MODE kUSDHC_DmaModeAdma2
/* address align */
#define SDMMCHOST_DMA_BUFFER_ADDR_ALIGN (USDHC_ADMA2_ADDRESS_ALIGN)

/* Read/write watermark level. The bigger value indicates DMA has higher read/write performance. */
#define USDHC_READ_WATERMARK_LEVEL (0x80U)
#define USDHC_WRITE_WATERMARK_LEVEL (0x80U)

/* ADMA table length united as word.
 *
 * One ADMA2 table item occupy two words which can transfer maximum 0xFFFFU bytes one time.
 * The more data to be transferred in one time, the bigger value of SDHC_ADMA_TABLE_WORDS need to be set.
 */
#define USDHC_ADMA_TABLE_WORDS (8U) /* define the ADMA descriptor table length */
#define USDHC_ADMA2_ADDR_ALIGN (4U) /* define the ADMA2 descriptor table addr align size */
#define USDHC_READ_BURST_LEN (8U)   /*!< number of words USDHC read in a single burst */
#define USDHC_WRITE_BURST_LEN (8U)  /*!< number of words USDHC write in a single burst */
#define USDHC_DATA_TIMEOUT (0xFU)   /*!< data timeout counter value */

#endif /* (defined(FSL_FEATURE_SOC_SDHC_COUNT) && (FSL_FEATURE_SOC_SDHC_COUNT > 0U)) */

/*! @brief card detect callback definition */
typedef void (*sdmmchost_cd_callback_t)(bool isInserted, void *userData);

/*! @brief host Endian mode
 * corresponding to driver define
 */
enum _sdmmchost_endian_mode
{
    kSDMMCHOST_EndianModeBig         = 0U, /*!< Big endian mode */
    kSDMMCHOST_EndianModeHalfWordBig = 1U, /*!< Half word big endian mode */
    kSDMMCHOST_EndianModeLittle      = 2U, /*!< Little endian mode */
};

/*! @brief sd card detect type */
typedef enum _sdmmchost_detect_card_type
{
    kSDMMCHOST_DetectCardByGpioCD,    /*!< sd card detect by CD pin through GPIO */
    kSDMMCHOST_DetectCardByHostCD,    /*!< sd card detect by CD pin through host */
    kSDMMCHOST_DetectCardByHostDATA3, /*!< sd card detect by DAT3 pin through host */
} sdmmchost_detect_card_type_t;

/*! @brief sd card detect */
typedef struct _sdmmchost_detect_card
{
    sdmmchost_detect_card_type_t cdType; /*!< card detect type */
    uint32_t cdTimeOut_ms; /*!< card detect timeout which allow 0 - 0xFFFFFFF, value 0 will return immediately, value
          0xFFFFFFFF will block until card is insert */

    sdmmchost_cd_callback_t cardInserted; /*!< card inserted callback which is meaningful for interrupt case */
    sdmmchost_cd_callback_t cardRemoved;  /*!< card removed callback which is meaningful for interrupt case */

    void *userData; /*!< user data */
} sdmmchost_detect_card_t;

/*! @brief card power control function pointer */
typedef void (*sdmmchost_pwr_t)(void);

/*! @brief card power control */
typedef struct _sdmmchost_pwr_card
{
    sdmmchost_pwr_t powerOn;  /*!< power on function pointer */
    uint32_t powerOnDelay_ms; /*!< power on delay */

    sdmmchost_pwr_t powerOff;  /*!< power off function pointer */
    uint32_t powerOffDelay_ms; /*!< power off delay */
} sdmmchost_pwr_card_t;

/*! @brief card interrupt function pointer */
typedef void (*sdmmchost_card_int_callback_t)(void *userData);
/*! @brief card interrupt application callback */
typedef struct _sdmmchost_card_int
{
    void *userData;                              /*!< user data */
    sdmmchost_card_int_callback_t cardInterrupt; /*!< card int call back */
} sdmmchost_card_int_t;

/*! @brief card switch voltage function pointer */
typedef void (*sdmmchost_card_switch_voltage_t)(void);
/*! @brief card switch voltage function collection */
typedef struct _sdmmchost_card_switch_voltage_func
{
    sdmmchost_card_switch_voltage_t cardSignalLine1V8; /*!< switch to 1.8v function pointer */
    sdmmchost_card_switch_voltage_t cardSignalLine3V3; /*!<switch to 3.3V function pointer */
} sdmmchost_card_switch_voltage_func_t;

/*! @brief card user parameter, user can define the parameter according the board, card capability */
typedef struct _sdmmhostcard_usr_param
{
    const sdmmchost_detect_card_t *cd;                       /*!< card detect type */
    const sdmmchost_pwr_card_t *pwr;                         /*!< power control configuration */
    const sdmmchost_card_int_t *cardInt;                     /*!< call back function for card interrupt */
    const sdmmchost_card_switch_voltage_func_t *cardVoltage; /*!< card voltage switch function */
} sdmmhostcard_usr_param_t;

/*! @ brief specifiy card user parameter name*/
typedef sdmmhostcard_usr_param_t sdcard_usr_param_t;
typedef sdmmhostcard_usr_param_t sdiocard_usr_param_t;
typedef sdmmhostcard_usr_param_t mmccard_usr_param_t;
/*******************************************************************************
 * API
 ******************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name adaptor function
 * @{
 */

/*!
 * @brief host not support function, this function is used for host not support feature
 * @param  void parameter ,used to avoid build warning
 * @retval kStatus_Fail ,host do not suppport
 */
static inline status_t SDMMCHOST_NotSupport(void *parameter)
{
    parameter = parameter;
    return kStatus_Success;
}

/*!
 * @brief Detect card insert, only need for SD cases.
 * @param base the pointer to host base address
 * @param cd card detect configuration
 * @param waitCardStatus status which user want to wait
 * @retval kStatus_Success detect card insert
 * @retval kStatus_Fail card insert event fail
 */
status_t SDMMCHOST_WaitCardDetectStatus(SDMMCHOST_TYPE *hostBase,
                                        const sdmmchost_detect_card_t *cd,
                                        bool waitCardStatus);

/*!
 * @brief check card is present or not.
 * @retval true card is present
 * @retval false card is not present
 */
bool SDMMCHOST_IsCardPresent(void);

/*!
 * @brief Init host controller.
 * @param host the pointer to host structure in card structure.
 * @param userData specific user data
 * @retval kStatus_Success host init success
 * @retval kStatus_Fail event fail
 */
status_t SDMMCHOST_Init(SDMMCHOST_CONFIG *host, void *userData);

/*!
 * @brief reset host controller.
 * @param host base address.
 */
void SDMMCHOST_Reset(SDMMCHOST_TYPE *base);

/*!
 * @brief host controller error recovery.
 * @param host base address.
 */
void SDMMCHOST_ErrorRecovery(SDMMCHOST_TYPE *base);

/*!
 * @brief Deinit host controller.
 * @param host the pointer to host structure in card structure.
 */
void SDMMCHOST_Deinit(void *host);

/*!
 * @brief host power off card function.
 * @param base host base address.
 * @param pwr depend on user define power configuration.
 */
void SDMMCHOST_PowerOffCard(SDMMCHOST_TYPE *base, const sdmmchost_pwr_card_t *pwr);

/*!
 * @brief host power on card function.
 * @param base host base address.
 * @param pwr depend on user define power configuration.
 */
void SDMMCHOST_PowerOnCard(SDMMCHOST_TYPE *base, const sdmmchost_pwr_card_t *pwr);

/*!
 * @brief SDMMC host delay function.
 * @param milliseconds delay counter.
 */
void SDMMCHOST_Delay(uint32_t milliseconds);

/* @} */

#if defined(__cplusplus)
}
#endif
/* @} */
#endif /* _FSL_SDMMC_HOST_H */
