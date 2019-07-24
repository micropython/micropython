/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _FSL_USDHC_H_
#define _FSL_USDHC_H_

#include "fsl_common.h"

/*!
 * @addtogroup usdhc
 * @{
 */

/******************************************************************************
 * Definitions.
 *****************************************************************************/

/*! @name Driver version */
/*@{*/
/*! @brief Driver version 2.2.8. */
#define FSL_USDHC_DRIVER_VERSION (MAKE_VERSION(2U, 2U, 8U))
/*@}*/

/*! @brief Maximum block count can be set one time */
#define USDHC_MAX_BLOCK_COUNT (USDHC_BLK_ATT_BLKCNT_MASK >> USDHC_BLK_ATT_BLKCNT_SHIFT)

/*! @brief USDHC status */
enum _usdhc_status
{
    kStatus_USDHC_BusyTransferring            = MAKE_STATUS(kStatusGroup_USDHC, 0U), /*!< Transfer is on-going */
    kStatus_USDHC_PrepareAdmaDescriptorFailed = MAKE_STATUS(kStatusGroup_USDHC, 1U), /*!< Set DMA descriptor failed */
    kStatus_USDHC_SendCommandFailed           = MAKE_STATUS(kStatusGroup_USDHC, 2U), /*!< Send command failed */
    kStatus_USDHC_TransferDataFailed          = MAKE_STATUS(kStatusGroup_USDHC, 3U), /*!< Transfer data failed */
    kStatus_USDHC_DMADataAddrNotAlign         = MAKE_STATUS(kStatusGroup_USDHC, 4U), /*!< data address not align */
    kStatus_USDHC_ReTuningRequest             = MAKE_STATUS(kStatusGroup_USDHC, 5U), /*!< re-tuning request */
    kStatus_USDHC_TuningError                 = MAKE_STATUS(kStatusGroup_USDHC, 6U), /*!< tuning error */
    kStatus_USDHC_NotSupport                  = MAKE_STATUS(kStatusGroup_USDHC, 7U), /*!< not support */
};

/*! @brief Host controller capabilities flag mask */
enum _usdhc_capability_flag
{
    kUSDHC_SupportAdmaFlag          = USDHC_HOST_CTRL_CAP_ADMAS_MASK, /*!< Support ADMA */
    kUSDHC_SupportHighSpeedFlag     = USDHC_HOST_CTRL_CAP_HSS_MASK,   /*!< Support high-speed */
    kUSDHC_SupportDmaFlag           = USDHC_HOST_CTRL_CAP_DMAS_MASK,  /*!< Support DMA */
    kUSDHC_SupportSuspendResumeFlag = USDHC_HOST_CTRL_CAP_SRS_MASK,   /*!< Support suspend/resume */
    kUSDHC_SupportV330Flag          = USDHC_HOST_CTRL_CAP_VS33_MASK,  /*!< Support voltage 3.3V */
    kUSDHC_SupportV300Flag          = USDHC_HOST_CTRL_CAP_VS30_MASK,  /*!< Support voltage 3.0V */
    kUSDHC_SupportV180Flag          = USDHC_HOST_CTRL_CAP_VS18_MASK,  /*!< Support voltage 1.8V */
    /* Put additional two flags in HTCAPBLT_MBL's position. */
    kUSDHC_Support4BitFlag = (USDHC_HOST_CTRL_CAP_MBL_SHIFT << 0U), /*!< Support 4 bit mode */
    kUSDHC_Support8BitFlag = (USDHC_HOST_CTRL_CAP_MBL_SHIFT << 1U), /*!< Support 8 bit mode */
    /* sd version 3.0 new feature */
    kUSDHC_SupportDDR50Flag = USDHC_HOST_CTRL_CAP_DDR50_SUPPORT_MASK, /*!< support DDR50 mode */

#if defined(FSL_FEATURE_USDHC_HAS_SDR104_MODE) && (!FSL_FEATURE_USDHC_HAS_SDR104_MODE)
    kUSDHC_SupportSDR104Flag = 0, /*!< not support SDR104 mode */
#else
    kUSDHC_SupportSDR104Flag   = USDHC_HOST_CTRL_CAP_SDR104_SUPPORT_MASK, /*!< support SDR104 mode */
#endif
#if defined(FSL_FEATURE_USDHC_HAS_SDR50_MODE) && (!FSL_FEATURE_USDHC_HAS_SDR50_MODE)
    kUSDHC_SupportSDR50Flag = 0U, /*!< not support SDR50 mode */
#else
    kUSDHC_SupportSDR50Flag    = USDHC_HOST_CTRL_CAP_SDR50_SUPPORT_MASK,  /*!< support SDR50 mode */
#endif
};

/*! @brief Wakeup event mask */
enum _usdhc_wakeup_event
{
    kUSDHC_WakeupEventOnCardInt    = USDHC_PROT_CTRL_WECINT_MASK, /*!< Wakeup on card interrupt */
    kUSDHC_WakeupEventOnCardInsert = USDHC_PROT_CTRL_WECINS_MASK, /*!< Wakeup on card insertion */
    kUSDHC_WakeupEventOnCardRemove = USDHC_PROT_CTRL_WECRM_MASK,  /*!< Wakeup on card removal */

    kUSDHC_WakeupEventsAll = (kUSDHC_WakeupEventOnCardInt | kUSDHC_WakeupEventOnCardInsert |
                              kUSDHC_WakeupEventOnCardRemove), /*!< All wakeup events */
};

/*! @brief Reset type mask */
enum _usdhc_reset
{
    kUSDHC_ResetAll     = USDHC_SYS_CTRL_RSTA_MASK, /*!< Reset all except card detection */
    kUSDHC_ResetCommand = USDHC_SYS_CTRL_RSTC_MASK, /*!< Reset command line */
    kUSDHC_ResetData    = USDHC_SYS_CTRL_RSTD_MASK, /*!< Reset data line */

#if defined(FSL_FEATURE_USDHC_HAS_SDR50_MODE) && (!FSL_FEATURE_USDHC_HAS_SDR50_MODE)
    kUSDHC_ResetTuning = 0U, /*!< no reset tuning circuit bit */
#else
    kUSDHC_ResetTuning         = USDHC_SYS_CTRL_RSTT_MASK,                /*!< reset tuning circuit */
#endif

    kUSDHC_ResetsAll =
        (kUSDHC_ResetAll | kUSDHC_ResetCommand | kUSDHC_ResetData | kUSDHC_ResetTuning), /*!< All reset types */
};

/*! @brief Transfer flag mask */
enum _usdhc_transfer_flag
{
    kUSDHC_EnableDmaFlag = USDHC_MIX_CTRL_DMAEN_MASK, /*!< Enable DMA */

    kUSDHC_CommandTypeSuspendFlag = (USDHC_CMD_XFR_TYP_CMDTYP(1U)), /*!< Suspend command */
    kUSDHC_CommandTypeResumeFlag  = (USDHC_CMD_XFR_TYP_CMDTYP(2U)), /*!< Resume command */
    kUSDHC_CommandTypeAbortFlag   = (USDHC_CMD_XFR_TYP_CMDTYP(3U)), /*!< Abort command */

    kUSDHC_EnableBlockCountFlag    = USDHC_MIX_CTRL_BCEN_MASK,   /*!< Enable block count */
    kUSDHC_EnableAutoCommand12Flag = USDHC_MIX_CTRL_AC12EN_MASK, /*!< Enable auto CMD12 */
    kUSDHC_DataReadFlag            = USDHC_MIX_CTRL_DTDSEL_MASK, /*!< Enable data read */
    kUSDHC_MultipleBlockFlag       = USDHC_MIX_CTRL_MSBSEL_MASK, /*!< Multiple block data read/write */
    kUSDHC_EnableAutoCommand23Flag = USDHC_MIX_CTRL_AC23EN_MASK, /*!< Enable auto CMD23 */

    kUSDHC_ResponseLength136Flag    = USDHC_CMD_XFR_TYP_RSPTYP(1U), /*!< 136 bit response length */
    kUSDHC_ResponseLength48Flag     = USDHC_CMD_XFR_TYP_RSPTYP(2U), /*!< 48 bit response length */
    kUSDHC_ResponseLength48BusyFlag = USDHC_CMD_XFR_TYP_RSPTYP(3U), /*!< 48 bit response length with busy status */

    kUSDHC_EnableCrcCheckFlag   = USDHC_CMD_XFR_TYP_CCCEN_MASK, /*!< Enable CRC check */
    kUSDHC_EnableIndexCheckFlag = USDHC_CMD_XFR_TYP_CICEN_MASK, /*!< Enable index check */
    kUSDHC_DataPresentFlag      = USDHC_CMD_XFR_TYP_DPSEL_MASK, /*!< Data present flag */
};

/*! @brief Present status flag mask */
enum _usdhc_present_status_flag
{
    kUSDHC_CommandInhibitFlag      = USDHC_PRES_STATE_CIHB_MASK,  /*!< Command inhibit */
    kUSDHC_DataInhibitFlag         = USDHC_PRES_STATE_CDIHB_MASK, /*!< Data inhibit */
    kUSDHC_DataLineActiveFlag      = USDHC_PRES_STATE_DLA_MASK,   /*!< Data line active */
    kUSDHC_SdClockStableFlag       = USDHC_PRES_STATE_SDSTB_MASK, /*!< SD bus clock stable */
    kUSDHC_WriteTransferActiveFlag = USDHC_PRES_STATE_WTA_MASK,   /*!< Write transfer active */
    kUSDHC_ReadTransferActiveFlag  = USDHC_PRES_STATE_RTA_MASK,   /*!< Read transfer active */
    kUSDHC_BufferWriteEnableFlag   = USDHC_PRES_STATE_BWEN_MASK,  /*!< Buffer write enable */
    kUSDHC_BufferReadEnableFlag    = USDHC_PRES_STATE_BREN_MASK,  /*!< Buffer read enable */

#if defined(FSL_FEATURE_USDHC_HAS_SDR50_MODE) && (!FSL_FEATURE_USDHC_HAS_SDR50_MODE)
    kUSDHC_DelaySettingFinishedFlag = 0U, /*!< not support */
    kUSDHC_ReTuningRequestFlag      = 0U, /*!< not support */
#else
    kUSDHC_ReTuningRequestFlag = USDHC_PRES_STATE_RTR_MASK, /*!< re-tuning request flag ,only used for SDR104 mode */
    kUSDHC_DelaySettingFinishedFlag = USDHC_PRES_STATE_TSCD_MASK, /*!< delay setting finished flag */
#endif

    kUSDHC_CardInsertedFlag     = USDHC_PRES_STATE_CINST_MASK, /*!< Card inserted */
    kUSDHC_CommandLineLevelFlag = USDHC_PRES_STATE_CLSL_MASK,  /*!< Command line signal level */

    kUSDHC_Data0LineLevelFlag = 1U << USDHC_PRES_STATE_DLSL_SHIFT,               /*!< Data0 line signal level */
    kUSDHC_Data1LineLevelFlag = 1U << (USDHC_PRES_STATE_DLSL_SHIFT + 1U),        /*!< Data1 line signal level */
    kUSDHC_Data2LineLevelFlag = 1U << (USDHC_PRES_STATE_DLSL_SHIFT + 2U),        /*!< Data2 line signal level */
    kUSDHC_Data3LineLevelFlag = 1U << (USDHC_PRES_STATE_DLSL_SHIFT + 3U),        /*!< Data3 line signal level */
    kUSDHC_Data4LineLevelFlag = 1U << (USDHC_PRES_STATE_DLSL_SHIFT + 4U),        /*!< Data4 line signal level */
    kUSDHC_Data5LineLevelFlag = 1U << (USDHC_PRES_STATE_DLSL_SHIFT + 5U),        /*!< Data5 line signal level */
    kUSDHC_Data6LineLevelFlag = 1U << (USDHC_PRES_STATE_DLSL_SHIFT + 6U),        /*!< Data6 line signal level */
    kUSDHC_Data7LineLevelFlag = (int)(1U << (USDHC_PRES_STATE_DLSL_SHIFT + 7U)), /*!< Data7 line signal level */
};

/*! @brief Interrupt status flag mask */
enum _usdhc_interrupt_status_flag
{
    kUSDHC_CommandCompleteFlag  = USDHC_INT_STATUS_CC_MASK,   /*!< Command complete */
    kUSDHC_DataCompleteFlag     = USDHC_INT_STATUS_TC_MASK,   /*!< Data complete */
    kUSDHC_BlockGapEventFlag    = USDHC_INT_STATUS_BGE_MASK,  /*!< Block gap event */
    kUSDHC_DmaCompleteFlag      = USDHC_INT_STATUS_DINT_MASK, /*!< DMA interrupt */
    kUSDHC_BufferWriteReadyFlag = USDHC_INT_STATUS_BWR_MASK,  /*!< Buffer write ready */
    kUSDHC_BufferReadReadyFlag  = USDHC_INT_STATUS_BRR_MASK,  /*!< Buffer read ready */
    kUSDHC_CardInsertionFlag    = USDHC_INT_STATUS_CINS_MASK, /*!< Card inserted */
    kUSDHC_CardRemovalFlag      = USDHC_INT_STATUS_CRM_MASK,  /*!< Card removed */
    kUSDHC_CardInterruptFlag    = USDHC_INT_STATUS_CINT_MASK, /*!< Card interrupt */

#if defined(FSL_FEATURE_USDHC_HAS_SDR50_MODE) && (!FSL_FEATURE_USDHC_HAS_SDR50_MODE)
    kUSDHC_ReTuningEventFlag = 0U, /*!< Re-Tuning event,only for SD3.0 SDR104 mode */
    kUSDHC_TuningPassFlag    = 0U, /*!< SDR104 mode tuning pass flag */
    kUSDHC_TuningErrorFlag   = 0U, /*!< SDR104 tuning error flag */
#else
    kUSDHC_ReTuningEventFlag        = USDHC_INT_STATUS_RTE_MASK,  /*!< Re-Tuning event,only for SD3.0 SDR104 mode */
    kUSDHC_TuningPassFlag           = USDHC_INT_STATUS_TP_MASK,   /*!< SDR104 mode tuning pass flag */
    kUSDHC_TuningErrorFlag          = USDHC_INT_STATUS_TNE_MASK,  /*!< SDR104 tuning error flag */
#endif

    kUSDHC_CommandTimeoutFlag     = USDHC_INT_STATUS_CTOE_MASK,  /*!< Command timeout error */
    kUSDHC_CommandCrcErrorFlag    = USDHC_INT_STATUS_CCE_MASK,   /*!< Command CRC error */
    kUSDHC_CommandEndBitErrorFlag = USDHC_INT_STATUS_CEBE_MASK,  /*!< Command end bit error */
    kUSDHC_CommandIndexErrorFlag  = USDHC_INT_STATUS_CIE_MASK,   /*!< Command index error */
    kUSDHC_DataTimeoutFlag        = USDHC_INT_STATUS_DTOE_MASK,  /*!< Data timeout error */
    kUSDHC_DataCrcErrorFlag       = USDHC_INT_STATUS_DCE_MASK,   /*!< Data CRC error */
    kUSDHC_DataEndBitErrorFlag    = USDHC_INT_STATUS_DEBE_MASK,  /*!< Data end bit error */
    kUSDHC_AutoCommand12ErrorFlag = USDHC_INT_STATUS_AC12E_MASK, /*!< Auto CMD12 error */
    kUSDHC_DmaErrorFlag           = USDHC_INT_STATUS_DMAE_MASK,  /*!< DMA error */

    kUSDHC_CommandErrorFlag = (kUSDHC_CommandTimeoutFlag | kUSDHC_CommandCrcErrorFlag | kUSDHC_CommandEndBitErrorFlag |
                               kUSDHC_CommandIndexErrorFlag), /*!< Command error */
    kUSDHC_DataErrorFlag    = (kUSDHC_DataTimeoutFlag | kUSDHC_DataCrcErrorFlag | kUSDHC_DataEndBitErrorFlag |
                            kUSDHC_AutoCommand12ErrorFlag),                                        /*!< Data error */
    kUSDHC_ErrorFlag        = (kUSDHC_CommandErrorFlag | kUSDHC_DataErrorFlag | kUSDHC_DmaErrorFlag), /*!< All error */
    kUSDHC_DataFlag         = (kUSDHC_DataCompleteFlag | kUSDHC_DmaCompleteFlag | kUSDHC_BufferWriteReadyFlag |
                       kUSDHC_BufferReadReadyFlag | kUSDHC_DataErrorFlag | kUSDHC_DmaErrorFlag), /*!< Data interrupts */
    kUSDHC_CommandFlag      = (kUSDHC_CommandErrorFlag | kUSDHC_CommandCompleteFlag), /*!< Command interrupts */
    kUSDHC_CardDetectFlag   = (kUSDHC_CardInsertionFlag | kUSDHC_CardRemovalFlag),    /*!< Card detection interrupts */
    kUSDHC_SDR104TuningFlag = (kUSDHC_TuningErrorFlag | kUSDHC_TuningPassFlag | kUSDHC_ReTuningEventFlag),

    kUSDHC_AllInterruptFlags = (kUSDHC_BlockGapEventFlag | kUSDHC_CardInterruptFlag | kUSDHC_CommandFlag |
                                kUSDHC_DataFlag | kUSDHC_ErrorFlag | kUSDHC_SDR104TuningFlag), /*!< All flags mask */
};

/*! @brief Auto CMD12 error status flag mask */
enum _usdhc_auto_command12_error_status_flag
{
    kUSDHC_AutoCommand12NotExecutedFlag = USDHC_AUTOCMD12_ERR_STATUS_AC12NE_MASK,    /*!< Not executed error */
    kUSDHC_AutoCommand12TimeoutFlag     = USDHC_AUTOCMD12_ERR_STATUS_AC12TOE_MASK,   /*!< Timeout error */
    kUSDHC_AutoCommand12EndBitErrorFlag = USDHC_AUTOCMD12_ERR_STATUS_AC12EBE_MASK,   /*!< End bit error */
    kUSDHC_AutoCommand12CrcErrorFlag    = USDHC_AUTOCMD12_ERR_STATUS_AC12CE_MASK,    /*!< CRC error */
    kUSDHC_AutoCommand12IndexErrorFlag  = USDHC_AUTOCMD12_ERR_STATUS_AC12IE_MASK,    /*!< Index error */
    kUSDHC_AutoCommand12NotIssuedFlag   = USDHC_AUTOCMD12_ERR_STATUS_CNIBAC12E_MASK, /*!< Not issued error */
};

/*! @brief standard tuning flag */
enum _usdhc_standard_tuning
{
#if defined(FSL_FEATURE_USDHC_HAS_SDR50_MODE) && (!FSL_FEATURE_USDHC_HAS_SDR50_MODE)
    kUSDHC_ExecuteTuning        = 0U, /*!< not support */
    kUSDHC_TuningSampleClockSel = 0U, /*!< not support */
#else
    kUSDHC_ExecuteTuning        = USDHC_AUTOCMD12_ERR_STATUS_EXECUTE_TUNING_MASK, /*!< used to start tuning procedure */
    kUSDHC_TuningSampleClockSel = USDHC_AUTOCMD12_ERR_STATUS_SMP_CLK_SEL_MASK, /*!< when std_tuning_en bit is set, this
                                                                                bit is used select sampleing clock */
#endif
};

/*! @brief ADMA error status flag mask */
enum _usdhc_adma_error_status_flag
{
    kUSDHC_AdmaLenghMismatchFlag   = USDHC_ADMA_ERR_STATUS_ADMALME_MASK, /*!< Length mismatch error */
    kUSDHC_AdmaDescriptorErrorFlag = USDHC_ADMA_ERR_STATUS_ADMADCE_MASK, /*!< Descriptor error */
};

/*!
 * @brief ADMA error state
 *
 * This state is the detail state when ADMA error has occurred.
 */
enum _usdhc_adma_error_state
{
    kUSDHC_AdmaErrorStateStopDma =
        0x00U, /*!< Stop DMA, previous location set in the ADMA system address is error address */
    kUSDHC_AdmaErrorStateFetchDescriptor =
        0x01U, /*!< Fetch descriptor, current location set in the ADMA system address is error address */
    kUSDHC_AdmaErrorStateChangeAddress = 0x02U, /*!< Change address, no DMA error is occurred */
    kUSDHC_AdmaErrorStateTransferData =
        0x03U, /*!< Transfer data, previous location set in the ADMA system address is error address */
    kUSDHC_AdmaErrorStateInvalidLength     = 0x04U, /*!< Invalid length in ADMA descriptor */
    kUSDHC_AdmaErrorStateInvalidDescriptor = 0x08U, /*!< Invalid descriptor fetched by ADMA */

    kUSDHC_AdmaErrorState = kUSDHC_AdmaErrorStateInvalidLength | kUSDHC_AdmaErrorStateInvalidDescriptor |
                            kUSDHC_AdmaErrorStateFetchDescriptor, /*!< ADMA error state */
};

/*! @brief Force event bit position */
enum _usdhc_force_event
{
    kUSDHC_ForceEventAutoCommand12NotExecuted = USDHC_FORCE_EVENT_FEVTAC12NE_MASK, /*!< Auto CMD12 not executed error */
    kUSDHC_ForceEventAutoCommand12Timeout     = USDHC_FORCE_EVENT_FEVTAC12TOE_MASK, /*!< Auto CMD12 timeout error */
    kUSDHC_ForceEventAutoCommand12CrcError    = USDHC_FORCE_EVENT_FEVTAC12CE_MASK,  /*!< Auto CMD12 CRC error */
    kUSDHC_ForceEventEndBitError              = USDHC_FORCE_EVENT_FEVTAC12EBE_MASK, /*!< Auto CMD12 end bit error */
    kUSDHC_ForceEventAutoCommand12IndexError  = USDHC_FORCE_EVENT_FEVTAC12IE_MASK,  /*!< Auto CMD12 index error */
    kUSDHC_ForceEventAutoCommand12NotIssued = USDHC_FORCE_EVENT_FEVTCNIBAC12E_MASK, /*!< Auto CMD12 not issued error */
    kUSDHC_ForceEventCommandTimeout         = USDHC_FORCE_EVENT_FEVTCTOE_MASK,      /*!< Command timeout error */
    kUSDHC_ForceEventCommandCrcError        = USDHC_FORCE_EVENT_FEVTCCE_MASK,       /*!< Command CRC error */
    kUSDHC_ForceEventCommandEndBitError     = USDHC_FORCE_EVENT_FEVTCEBE_MASK,      /*!< Command end bit error */
    kUSDHC_ForceEventCommandIndexError      = USDHC_FORCE_EVENT_FEVTCIE_MASK,       /*!< Command index error */
    kUSDHC_ForceEventDataTimeout            = USDHC_FORCE_EVENT_FEVTDTOE_MASK,      /*!< Data timeout error */
    kUSDHC_ForceEventDataCrcError           = USDHC_FORCE_EVENT_FEVTDCE_MASK,       /*!< Data CRC error */
    kUSDHC_ForceEventDataEndBitError        = USDHC_FORCE_EVENT_FEVTDEBE_MASK,      /*!< Data end bit error */
    kUSDHC_ForceEventAutoCommand12Error     = USDHC_FORCE_EVENT_FEVTAC12E_MASK,     /*!< Auto CMD12 error */
    kUSDHC_ForceEventCardInt                = (int)USDHC_FORCE_EVENT_FEVTCINT_MASK, /*!< Card interrupt */
    kUSDHC_ForceEventDmaError               = USDHC_FORCE_EVENT_FEVTDMAE_MASK,      /*!< Dma error */
#if defined(FSL_FEATURE_USDHC_HAS_SDR50_MODE) && (!FSL_FEATURE_USDHC_HAS_SDR50_MODE)
    kUSDHC_ForceEventTuningError = 0U, /*!< not support */
#else
    kUSDHC_ForceEventTuningError = USDHC_FORCE_EVENT_FEVTTNE_MASK,             /*!< Tuning error */
#endif

    kUSDHC_ForceEventsAll =
        (int)(USDHC_FORCE_EVENT_FEVTAC12NE_MASK | USDHC_FORCE_EVENT_FEVTAC12TOE_MASK |
              USDHC_FORCE_EVENT_FEVTAC12CE_MASK | USDHC_FORCE_EVENT_FEVTAC12EBE_MASK |
              USDHC_FORCE_EVENT_FEVTAC12IE_MASK | USDHC_FORCE_EVENT_FEVTCNIBAC12E_MASK |
              USDHC_FORCE_EVENT_FEVTCTOE_MASK | USDHC_FORCE_EVENT_FEVTCCE_MASK | USDHC_FORCE_EVENT_FEVTCEBE_MASK |
              USDHC_FORCE_EVENT_FEVTCIE_MASK | USDHC_FORCE_EVENT_FEVTDTOE_MASK | USDHC_FORCE_EVENT_FEVTDCE_MASK |
              USDHC_FORCE_EVENT_FEVTDEBE_MASK | USDHC_FORCE_EVENT_FEVTAC12E_MASK | USDHC_FORCE_EVENT_FEVTCINT_MASK |
              USDHC_FORCE_EVENT_FEVTDMAE_MASK | kUSDHC_ForceEventTuningError), /*!< All force event flags mask */
};

/*! @brief Data transfer width */
typedef enum _usdhc_data_bus_width
{
    kUSDHC_DataBusWidth1Bit = 0U, /*!< 1-bit mode */
    kUSDHC_DataBusWidth4Bit = 1U, /*!< 4-bit mode */
    kUSDHC_DataBusWidth8Bit = 2U, /*!< 8-bit mode */
} usdhc_data_bus_width_t;

/*! @brief Endian mode */
typedef enum _usdhc_endian_mode
{
    kUSDHC_EndianModeBig         = 0U, /*!< Big endian mode */
    kUSDHC_EndianModeHalfWordBig = 1U, /*!< Half word big endian mode */
    kUSDHC_EndianModeLittle      = 2U, /*!< Little endian mode */
} usdhc_endian_mode_t;

/*! @brief DMA mode */
typedef enum _usdhc_dma_mode
{
    kUSDHC_DmaModeSimple = 0U, /*!< external DMA */
    kUSDHC_DmaModeAdma1  = 1U, /*!< ADMA1 is selected */
    kUSDHC_DmaModeAdma2  = 2U, /*!< ADMA2 is selected */
    kUSDHC_ExternalDMA   = 3U, /*!< external dma mode select */
} usdhc_dma_mode_t;

/*! @brief SDIO control flag mask */
enum _usdhc_sdio_control_flag
{
    kUSDHC_StopAtBlockGapFlag      = USDHC_PROT_CTRL_SABGREQ_MASK,         /*!< Stop at block gap */
    kUSDHC_ReadWaitControlFlag     = USDHC_PROT_CTRL_RWCTL_MASK,           /*!< Read wait control */
    kUSDHC_InterruptAtBlockGapFlag = USDHC_PROT_CTRL_IABG_MASK,            /*!< Interrupt at block gap */
    kUSDHC_ReadDoneNo8CLK          = USDHC_PROT_CTRL_RD_DONE_NO_8CLK_MASK, /*!< read done without 8 clk for block gap */
    kUSDHC_ExactBlockNumberReadFlag = USDHC_PROT_CTRL_NON_EXACT_BLK_RD_MASK, /*!< Exact block number read */
};

/*! @brief MMC card boot mode */
typedef enum _usdhc_boot_mode
{
    kUSDHC_BootModeNormal      = 0U, /*!< Normal boot */
    kUSDHC_BootModeAlternative = 1U, /*!< Alternative boot */
} usdhc_boot_mode_t;

/*! @brief The command type */
typedef enum _usdhc_card_command_type
{
    kCARD_CommandTypeNormal  = 0U, /*!< Normal command */
    kCARD_CommandTypeSuspend = 1U, /*!< Suspend command */
    kCARD_CommandTypeResume  = 2U, /*!< Resume command */
    kCARD_CommandTypeAbort   = 3U, /*!< Abort command */
    kCARD_CommandTypeEmpty   = 4U, /*!< Empty command */
} usdhc_card_command_type_t;

/*!
 * @brief The command response type.
 *
 * Define the command response type from card to host controller.
 */
typedef enum _usdhc_card_response_type
{
    kCARD_ResponseTypeNone = 0U, /*!< Response type: none */
    kCARD_ResponseTypeR1   = 1U, /*!< Response type: R1 */
    kCARD_ResponseTypeR1b  = 2U, /*!< Response type: R1b */
    kCARD_ResponseTypeR2   = 3U, /*!< Response type: R2 */
    kCARD_ResponseTypeR3   = 4U, /*!< Response type: R3 */
    kCARD_ResponseTypeR4   = 5U, /*!< Response type: R4 */
    kCARD_ResponseTypeR5   = 6U, /*!< Response type: R5 */
    kCARD_ResponseTypeR5b  = 7U, /*!< Response type: R5b */
    kCARD_ResponseTypeR6   = 8U, /*!< Response type: R6 */
    kCARD_ResponseTypeR7   = 9U, /*!< Response type: R7 */
} usdhc_card_response_type_t;

/*! @brief The alignment size for ADDRESS filed in ADMA1's descriptor */
#define USDHC_ADMA1_ADDRESS_ALIGN (4096U)
/*! @brief The alignment size for LENGTH field in ADMA1's descriptor */
#define USDHC_ADMA1_LENGTH_ALIGN (4096U)
/*! @brief The alignment size for ADDRESS field in ADMA2's descriptor */
#define USDHC_ADMA2_ADDRESS_ALIGN (4U)
/*! @brief The alignment size for LENGTH filed in ADMA2's descriptor */
#define USDHC_ADMA2_LENGTH_ALIGN (4U)

/* ADMA1 descriptor table
 * |------------------------|---------|--------------------------|
 * | Address/page field     |Reserved |         Attribute        |
 * |------------------------|---------|--------------------------|
 * |31                    12|11      6|05  |04  |03|02 |01 |00   |
 * |------------------------|---------|----|----|--|---|---|-----|
 * | address or data length | 000000  |Act2|Act1| 0|Int|End|Valid|
 * |------------------------|---------|----|----|--|---|---|-----|
 *
 *
 * |------|------|-----------------|-------|-------------|
 * | Act2 | Act1 |     Comment     | 31-28 | 27 - 12     |
 * |------|------|-----------------|---------------------|
 * |   0  |   0  | No op           | Don't care          |
 * |------|------|-----------------|-------|-------------|
 * |   0  |   1  | Set data length |  0000 | Data Length |
 * |------|------|-----------------|-------|-------------|
 * |   1  |   0  | Transfer data   | Data address        |
 * |------|------|-----------------|---------------------|
 * |   1  |   1  | Link descriptor | Descriptor address  |
 * |------|------|-----------------|---------------------|
 */
/*! @brief The bit shift for ADDRESS filed in ADMA1's descriptor */
#define USDHC_ADMA1_DESCRIPTOR_ADDRESS_SHIFT (12U)
/*! @brief The bit mask for ADDRESS field in ADMA1's descriptor */
#define USDHC_ADMA1_DESCRIPTOR_ADDRESS_MASK (0xFFFFFU)
/*! @brief The bit shift for LENGTH filed in ADMA1's descriptor */
#define USDHC_ADMA1_DESCRIPTOR_LENGTH_SHIFT (12U)
/*! @brief The mask for LENGTH field in ADMA1's descriptor */
#define USDHC_ADMA1_DESCRIPTOR_LENGTH_MASK (0xFFFFU)
/*! @brief The maximum value of LENGTH filed in ADMA1's descriptor
 * Since the ADMA1 support max transfer size is 65535 which is not divisible by
 * 4096, so to make sure a large data load transfer(>64KB) continuously(require the data
 * address should be alwawys align with 4096), software will set the maximum data length
 * for ADMA1 to (64 - 4)KB.
 */
#define USDHC_ADMA1_DESCRIPTOR_MAX_LENGTH_PER_ENTRY (USDHC_ADMA1_DESCRIPTOR_LENGTH_MASK + 1U - 4096U)

/*! @brief The mask for the control/status field in ADMA1 descriptor */
enum _usdhc_adma1_descriptor_flag
{
    kUSDHC_Adma1DescriptorValidFlag     = (1U << 0U),                        /*!< Valid flag */
    kUSDHC_Adma1DescriptorEndFlag       = (1U << 1U),                        /*!< End flag */
    kUSDHC_Adma1DescriptorInterrupFlag  = (1U << 2U),                        /*!< Interrupt flag */
    kUSDHC_Adma1DescriptorActivity1Flag = (1U << 4U),                        /*!< Activity 1 flag */
    kUSDHC_Adma1DescriptorActivity2Flag = (1U << 5U),                        /*!< Activity 2 flag */
    kUSDHC_Adma1DescriptorTypeNop       = (kUSDHC_Adma1DescriptorValidFlag), /*!< No operation */
    kUSDHC_Adma1DescriptorTypeTransfer =
        (kUSDHC_Adma1DescriptorActivity2Flag | kUSDHC_Adma1DescriptorValidFlag), /*!< Transfer data */
    kUSDHC_Adma1DescriptorTypeLink = (kUSDHC_Adma1DescriptorActivity1Flag | kUSDHC_Adma1DescriptorActivity2Flag |
                                      kUSDHC_Adma1DescriptorValidFlag), /*!< Link descriptor */
    kUSDHC_Adma1DescriptorTypeSetLength =
        (kUSDHC_Adma1DescriptorActivity1Flag | kUSDHC_Adma1DescriptorValidFlag), /*!< Set data length */
};

/* ADMA2 descriptor table
 * |----------------|---------------|-------------|--------------------------|
 * | Address field  |     Length    | Reserved    |         Attribute        |
 * |----------------|---------------|-------------|--------------------------|
 * |63            32|31           16|15         06|05  |04  |03|02 |01 |00   |
 * |----------------|---------------|-------------|----|----|--|---|---|-----|
 * | 32-bit address | 16-bit length | 0000000000  |Act2|Act1| 0|Int|End|Valid|
 * |----------------|---------------|-------------|----|----|--|---|---|-----|
 *
 *
 * | Act2 | Act1 |     Comment     | Operation                                                         |
 * |------|------|-----------------|-------------------------------------------------------------------|
 * |   0  |   0  | No op           | Don't care                                                        |
 * |------|------|-----------------|-------------------------------------------------------------------|
 * |   0  |   1  | Reserved        | Read this line and go to next one                                 |
 * |------|------|-----------------|-------------------------------------------------------------------|
 * |   1  |   0  | Transfer data   | Transfer data with address and length set in this descriptor line |
 * |------|------|-----------------|-------------------------------------------------------------------|
 * |   1  |   1  | Link descriptor | Link to another descriptor                                        |
 * |------|------|-----------------|-------------------------------------------------------------------|
 */
/*! @brief The bit shift for LENGTH field in ADMA2's descriptor */
#define USDHC_ADMA2_DESCRIPTOR_LENGTH_SHIFT (16U)
/*! @brief The bit mask for LENGTH field in ADMA2's descriptor */
#define USDHC_ADMA2_DESCRIPTOR_LENGTH_MASK (0xFFFFU)
/*! @brief The maximum value of LENGTH field in ADMA2's descriptor */
#define USDHC_ADMA2_DESCRIPTOR_MAX_LENGTH_PER_ENTRY (USDHC_ADMA2_DESCRIPTOR_LENGTH_MASK - 3U)

/*! @brief ADMA1 descriptor control and status mask */
enum _usdhc_adma2_descriptor_flag
{
    kUSDHC_Adma2DescriptorValidFlag     = (1U << 0U), /*!< Valid flag */
    kUSDHC_Adma2DescriptorEndFlag       = (1U << 1U), /*!< End flag */
    kUSDHC_Adma2DescriptorInterruptFlag = (1U << 2U), /*!< Interrupt flag */
    kUSDHC_Adma2DescriptorActivity1Flag = (1U << 4U), /*!< Activity 1 mask */
    kUSDHC_Adma2DescriptorActivity2Flag = (1U << 5U), /*!< Activity 2 mask */

    kUSDHC_Adma2DescriptorTypeNop = (kUSDHC_Adma2DescriptorValidFlag), /*!< No operation */
    kUSDHC_Adma2DescriptorTypeReserved =
        (kUSDHC_Adma2DescriptorActivity1Flag | kUSDHC_Adma2DescriptorValidFlag), /*!< Reserved */
    kUSDHC_Adma2DescriptorTypeTransfer =
        (kUSDHC_Adma2DescriptorActivity2Flag | kUSDHC_Adma2DescriptorValidFlag), /*!< Transfer type */
    kUSDHC_Adma2DescriptorTypeLink = (kUSDHC_Adma2DescriptorActivity1Flag | kUSDHC_Adma2DescriptorActivity2Flag |
                                      kUSDHC_Adma2DescriptorValidFlag), /*!< Link type */
};

/*! @brief ADMA descriptor configuration flag */
enum _usdhc_adma_flag
{
    kUSDHC_AdmaDescriptorSingleFlag =
        0U, /*!< try to finish the transfer in a single ADMA descriptor, if transfer size is bigger than one
            ADMA descriptor's ability, new another descriptor for data transfer */
    kUSDHC_AdmaDescriptorMultipleFlag = 1U, /*!< create multiple ADMA descriptor within the ADMA table, this is used for
                                             mmc boot mode specifically, which need
                                             to modify the ADMA descriptor on the fly, so the flag should be used
                                             combine with stop at block gap feature */
};

/*! @brief dma transfer burst len config. */
typedef enum _usdhc_burst_len
{
    kUSDHC_EnBurstLenForINCR         = 0x01U, /*!< enable burst len for INCR */
    kUSDHC_EnBurstLenForINCR4816     = 0x02U, /*!< enable burst len for INCR4/INCR8/INCR16 */
    kUSDHC_EnBurstLenForINCR4816WRAP = 0x04U, /*!< enable burst len for INCR4/8/16 WRAP */
} usdhc_burst_len_t;

/*! @brief transfer data type definition. */
enum _usdhc_transfer_data_type
{
    kUSDHC_TransferDataNormal        = 0U, /*!< transfer normal read/write data */
    kUSDHC_TransferDataTuning        = 1U, /*!< transfer tuning data */
    kUSDHC_TransferDataBoot          = 2U, /*!< transfer boot data */
    kUSDHC_TransferDataBootcontinous = 3U, /*!< transfer boot data continous */
};

/*! @brief Defines the adma1 descriptor structure. */
typedef uint32_t usdhc_adma1_descriptor_t;

/*! @brief Defines the ADMA2 descriptor structure. */
typedef struct _usdhc_adma2_descriptor
{
    uint32_t attribute;      /*!< The control and status field */
    const uint32_t *address; /*!< The address field */
} usdhc_adma2_descriptor_t;

/*!
 * @brief USDHC capability information.
 *
 * Defines a structure to save the capability information of USDHC.
 */
typedef struct _usdhc_capability
{
    uint32_t sdVersion;      /*!< support SD card/sdio version */
    uint32_t mmcVersion;     /*!< support emmc card version */
    uint32_t maxBlockLength; /*!< Maximum block length united as byte */
    uint32_t maxBlockCount;  /*!< Maximum block count can be set one time */
    uint32_t flags;          /*!< Capability flags to indicate the support information(_usdhc_capability_flag) */
} usdhc_capability_t;

/*! @brief Data structure to configure the MMC boot feature */
typedef struct _usdhc_boot_config
{
    uint32_t ackTimeoutCount;      /*!< Timeout value for the boot ACK. The available range is 0 ~ 15. */
    usdhc_boot_mode_t bootMode;    /*!< Boot mode selection. */
    uint32_t blockCount;           /*!< Stop at block gap value of automatic mode. Available range is 0 ~ 65535. */
    size_t blockSize;              /*!< Block size */
    bool enableBootAck;            /*!< Enable or disable boot ACK */
    bool enableAutoStopAtBlockGap; /*!< Enable or disable auto stop at block gap function in boot period */
} usdhc_boot_config_t;

/*! @brief Data structure to initialize the USDHC */
typedef struct _usdhc_config
{
    uint32_t dataTimeout;           /*!< Data timeout value */
    usdhc_endian_mode_t endianMode; /*!< Endian mode */
    uint8_t readWatermarkLevel;     /*!< Watermark level for DMA read operation. Available range is 1 ~ 128. */
    uint8_t writeWatermarkLevel;    /*!< Watermark level for DMA write operation. Available range is 1 ~ 128. */
    uint8_t readBurstLen;           /*!< Read burst len */
    uint8_t writeBurstLen;          /*!< Write burst len */
} usdhc_config_t;

/*!
 * @brief Card data descriptor
 *
 * Defines a structure to contain data-related attribute. 'enableIgnoreError' is used for the case that upper card
 * driver
 * want to ignore the error event to read/write all the data not to stop read/write immediately when error event
 * happen for example bus testing procedure for MMC card.
 */
typedef struct _usdhc_data
{
    bool enableAutoCommand12; /*!< Enable auto CMD12 */
    bool enableAutoCommand23; /*!< Enable auto CMD23 */
    bool enableIgnoreError;   /*!< Enable to ignore error event to read/write all the data */
    uint8_t dataType;         /*!< this is used to distinguish the normal/tuning/boot data */
    size_t blockSize;         /*!< Block size */
    uint32_t blockCount;      /*!< Block count */
    uint32_t *rxData;         /*!< Buffer to save data read */
    const uint32_t *txData;   /*!< Data buffer to write */
} usdhc_data_t;

/*!
 * @brief Card command descriptor
 *
 * Define card command-related attribute.
 */
typedef struct _usdhc_command
{
    uint32_t index;                          /*!< Command index */
    uint32_t argument;                       /*!< Command argument */
    usdhc_card_command_type_t type;          /*!< Command type */
    usdhc_card_response_type_t responseType; /*!< Command response type */
    uint32_t response[4U];                   /*!< Response for this command */
    uint32_t responseErrorFlags;             /*!< response error flag, the flag which need to check
                                                 the command reponse*/
    uint32_t flags;                          /*!< Cmd flags */
} usdhc_command_t;

/*! @brief ADMA configuration */
typedef struct _usdhc_adma_config
{
    usdhc_dma_mode_t dmaMode; /*!< DMA mode */

    usdhc_burst_len_t burstLen; /*!< burst len config */

    uint32_t *admaTable;     /*!< ADMA table address, can't be null if transfer way is ADMA1/ADMA2 */
    uint32_t admaTableWords; /*!< ADMA table length united as words, can't be 0 if transfer way is ADMA1/ADMA2 */
} usdhc_adma_config_t;

/*! @brief Transfer state */
typedef struct _usdhc_transfer
{
    usdhc_data_t *data;       /*!< Data to transfer */
    usdhc_command_t *command; /*!< Command to send */
} usdhc_transfer_t;

/*! @brief USDHC handle typedef */
typedef struct _usdhc_handle usdhc_handle_t;

/*! @brief USDHC callback functions. */
typedef struct _usdhc_transfer_callback
{
    void (*CardInserted)(USDHC_Type *base,
                         void *userData); /*!< Card inserted occurs when DAT3/CD pin is for card detect */
    void (*CardRemoved)(USDHC_Type *base, void *userData);   /*!< Card removed occurs */
    void (*SdioInterrupt)(USDHC_Type *base, void *userData); /*!< SDIO card interrupt occurs */
    void (*BlockGap)(USDHC_Type *base, void *userData);      /*!< stopped at block gap event */
    void (*TransferComplete)(USDHC_Type *base,
                             usdhc_handle_t *handle,
                             status_t status,
                             void *userData);           /*!< Transfer complete callback */
    void (*ReTuning)(USDHC_Type *base, void *userData); /*!< handle the re-tuning */
} usdhc_transfer_callback_t;

/*!
 * @brief USDHC handle
 *
 * Defines the structure to save the USDHC state information and callback function. The detailed interrupt status when
 * sending a command or transfering data can be obtained from the interruptFlags field by using the mask defined in
 * usdhc_interrupt_flag_t.
 *
 * @note All the fields except interruptFlags and transferredWords must be allocated by the user.
 */
struct _usdhc_handle
{
    /* Transfer parameter */
    usdhc_data_t *volatile data;       /*!< Data to transfer */
    usdhc_command_t *volatile command; /*!< Command to send */

    /* Transfer status */
    volatile uint32_t transferredWords; /*!< Words transferred by DATAPORT way */

    /* Callback functions */
    usdhc_transfer_callback_t callback; /*!< Callback function */
    void *userData;                     /*!< Parameter for transfer complete callback */
};

/*! @brief USDHC transfer function. */
typedef status_t (*usdhc_transfer_function_t)(USDHC_Type *base, usdhc_transfer_t *content);

/*! @brief USDHC host descriptor */
typedef struct _usdhc_host
{
    USDHC_Type *base;                   /*!< USDHC peripheral base address */
    uint32_t sourceClock_Hz;            /*!< USDHC source clock frequency united in Hz */
    usdhc_config_t config;              /*!< USDHC configuration */
    usdhc_capability_t capability;      /*!< USDHC capability information */
    usdhc_transfer_function_t transfer; /*!< USDHC transfer function */
} usdhc_host_t;

/*************************************************************************************************
 * API
 ************************************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name Initialization and deinitialization
 * @{
 */

/*!
 * @brief USDHC module initialization function.
 *
 * Configures the USDHC according to the user configuration.
 *
 * Example:
   @code
   usdhc_config_t config;
   config.cardDetectDat3 = false;
   config.endianMode = kUSDHC_EndianModeLittle;
   config.dmaMode = kUSDHC_DmaModeAdma2;
   config.readWatermarkLevel = 128U;
   config.writeWatermarkLevel = 128U;
   USDHC_Init(USDHC, &config);
   @endcode
 *
 * @param base USDHC peripheral base address.
 * @param config USDHC configuration information.
 * @retval kStatus_Success Operate successfully.
 */
void USDHC_Init(USDHC_Type *base, const usdhc_config_t *config);

/*!
 * @brief Deinitializes the USDHC.
 *
 * @param base USDHC peripheral base address.
 */
void USDHC_Deinit(USDHC_Type *base);

/*!
 * @brief Resets the USDHC.
 *
 * @param base USDHC peripheral base address.
 * @param mask The reset type mask(_usdhc_reset).
 * @param timeout Timeout for reset.
 * @retval true Reset successfully.
 * @retval false Reset failed.
 */
bool USDHC_Reset(USDHC_Type *base, uint32_t mask, uint32_t timeout);

/* @} */

/*!
 * @name DMA Control
 * @{
 */

/*!
 * @brief Sets the DMA descriptor table configuration.
 * A high level DMA descriptor configuration function.
 * @param base USDHC peripheral base address.
 * @param adma configuration
 * @param data Data descriptor
 * @param flags ADAM descriptor flag, used to indicate to create multiple or single descriptor, please
 *  reference _usdhc_adma_flag
 * @retval kStatus_OutOfRange ADMA descriptor table length isn't enough to describe data.
 * @retval kStatus_Success Operate successfully.
 */
status_t USDHC_SetAdmaTableConfig(USDHC_Type *base,
                                  usdhc_adma_config_t *dmaConfig,
                                  usdhc_data_t *dataConfig,
                                  uint32_t flags);

/*!
 * @brief Internal DMA configuration.
 * This function is used to config the USDHC DMA related registers.
 * @param base USDHC peripheral base address.
 * @param adma configuration
 * @param dataAddr transfer data address, a simple DMA parameter, if ADMA is used, leave it to NULL.
 * @param enAutoCmd23 flag to indicate Auto CMD23 is enable or not, a simple DMA parameter,if ADMA is used, leave it
 * to false.
 * @retval kStatus_OutOfRange ADMA descriptor table length isn't enough to describe data.
 * @retval kStatus_Success Operate successfully.
 */
status_t USDHC_SetInternalDmaConfig(USDHC_Type *base,
                                    usdhc_adma_config_t *dmaConfig,
                                    const uint32_t *dataAddr,
                                    bool enAutoCmd23);

/*!
 * @brief Sets the ADMA2 descriptor table configuration.
 *
 * @param admaTable Adma table address.
 * @param admaTableWords Adma table length.
 * @param dataBufferAddr Data buffer address.
 * @param dataBytes Data Data length.
 * @param flags ADAM descriptor flag, used to indicate to create multiple or single descriptor, please
 *  reference _usdhc_adma_flag.
 * @retval kStatus_OutOfRange ADMA descriptor table length isn't enough to describe data.
 * @retval kStatus_Success Operate successfully.
 */
status_t USDHC_SetADMA2Descriptor(
    uint32_t *admaTable, uint32_t admaTableWords, const uint32_t *dataBufferAddr, uint32_t dataBytes, uint32_t flags);

/*!
 * @brief Sets the ADMA1 descriptor table configuration.
 *
 * @param admaTable Adma table address.
 * @param admaTableWords Adma table length.
 * @param dataBufferAddr Data buffer address.
 * @param dataBytes Data length.
 * @param flags ADAM descriptor flag, used to indicate to create multiple or single descriptor, please
 *  reference _usdhc_adma_flag.
 * @retval kStatus_OutOfRange ADMA descriptor table length isn't enough to describe data.
 * @retval kStatus_Success Operate successfully.
 */
status_t USDHC_SetADMA1Descriptor(
    uint32_t *admaTable, uint32_t admaTableWords, const uint32_t *dataBufferAddr, uint32_t dataBytes, uint32_t flags);

/*!
 * @brief enable internal DMA.
 *
 * @param base USDHC peripheral base address.
 * @param enable enable or disable flag
 */
static inline void USDHC_EnableInternalDMA(USDHC_Type *base, bool enable)
{
    if (enable)
    {
        base->MIX_CTRL |= USDHC_MIX_CTRL_DMAEN_MASK;
    }
    else
    {
        base->MIX_CTRL &= ~USDHC_MIX_CTRL_DMAEN_MASK;
        base->PROT_CTRL &= ~USDHC_PROT_CTRL_DMASEL_MASK;
    }
}

/* @} */

/*!
 * @name Interrupts
 * @{
 */

/*!
 * @brief Enables the interrupt status.
 *
 * @param base USDHC peripheral base address.
 * @param mask Interrupt status flags mask(_usdhc_interrupt_status_flag).
 */
static inline void USDHC_EnableInterruptStatus(USDHC_Type *base, uint32_t mask)
{
    base->INT_STATUS_EN |= mask;
}

/*!
 * @brief Disables the interrupt status.
 *
 * @param base USDHC peripheral base address.
 * @param mask The interrupt status flags mask(_usdhc_interrupt_status_flag).
 */
static inline void USDHC_DisableInterruptStatus(USDHC_Type *base, uint32_t mask)
{
    base->INT_STATUS_EN &= ~mask;
}

/*!
 * @brief Enables the interrupt signal corresponding to the interrupt status flag.
 *
 * @param base USDHC peripheral base address.
 * @param mask The interrupt status flags mask(_usdhc_interrupt_status_flag).
 */
static inline void USDHC_EnableInterruptSignal(USDHC_Type *base, uint32_t mask)
{
    base->INT_SIGNAL_EN |= mask;
}

/*!
 * @brief Disables the interrupt signal corresponding to the interrupt status flag.
 *
 * @param base USDHC peripheral base address.
 * @param mask The interrupt status flags mask(_usdhc_interrupt_status_flag).
 */
static inline void USDHC_DisableInterruptSignal(USDHC_Type *base, uint32_t mask)
{
    base->INT_SIGNAL_EN &= ~mask;
}

/* @} */

/*!
 * @name Status
 * @{
 */

/*!
 * @brief Gets the enabled interrupt status.
 *
 * @param base USDHC peripheral base address.
 * @return Current interrupt status flags mask(_usdhc_interrupt_status_flag).
 */
static inline uint32_t USDHC_GetEnabledInterruptStatusFlags(USDHC_Type *base)
{
    return (base->INT_STATUS) & (base->INT_SIGNAL_EN);
}

/*!
 * @brief Gets the current interrupt status.
 *
 * @param base USDHC peripheral base address.
 * @return Current interrupt status flags mask(_usdhc_interrupt_status_flag).
 */
static inline uint32_t USDHC_GetInterruptStatusFlags(USDHC_Type *base)
{
    return base->INT_STATUS;
}

/*!
 * @brief Clears a specified interrupt status.
 * write 1 clears
 * @param base USDHC peripheral base address.
 * @param mask The interrupt status flags mask(_usdhc_interrupt_status_flag).
 */
static inline void USDHC_ClearInterruptStatusFlags(USDHC_Type *base, uint32_t mask)
{
    base->INT_STATUS = mask;
}

/*!
 * @brief Gets the status of auto command 12 error.
 *
 * @param base USDHC peripheral base address.
 * @return Auto command 12 error status flags mask(_usdhc_auto_command12_error_status_flag).
 */
static inline uint32_t USDHC_GetAutoCommand12ErrorStatusFlags(USDHC_Type *base)
{
    return base->AUTOCMD12_ERR_STATUS;
}

/*!
 * @brief Gets the status of the ADMA error.
 *
 * @param base USDHC peripheral base address.
 * @return ADMA error status flags mask(_usdhc_adma_error_status_flag).
 */
static inline uint32_t USDHC_GetAdmaErrorStatusFlags(USDHC_Type *base)
{
    return base->ADMA_ERR_STATUS & 0xFU;
}

/*!
 * @brief Gets a present status.
 *
 * This function gets the present USDHC's status except for an interrupt status and an error status.
 *
 * @param base USDHC peripheral base address.
 * @return Present USDHC's status flags mask(_usdhc_present_status_flag).
 */
static inline uint32_t USDHC_GetPresentStatusFlags(USDHC_Type *base)
{
    return base->PRES_STATE;
}

/* @} */

/*!
 * @name Bus Operations
 * @{
 */

/*!
 * @brief Gets the capability information.
 *
 * @param base USDHC peripheral base address.
 * @param capability Structure to save capability information.
 */
void USDHC_GetCapability(USDHC_Type *base, usdhc_capability_t *capability);

/*!
 * @brief force the card clock on.
 *
 * @param base USDHC peripheral base address.
 * @param enable/disable flag.
 */
static inline void USDHC_ForceClockOn(USDHC_Type *base, bool enable)
{
    if (enable)
    {
        base->VEND_SPEC |= USDHC_VEND_SPEC_FRC_SDCLK_ON_MASK;
    }
    else
    {
        base->VEND_SPEC &= ~USDHC_VEND_SPEC_FRC_SDCLK_ON_MASK;
    }
}

/*!
 * @brief Sets the SD bus clock frequency.
 *
 * @param base USDHC peripheral base address.
 * @param srcClock_Hz USDHC source clock frequency united in Hz.
 * @param busClock_Hz SD bus clock frequency united in Hz.
 *
 * @return The nearest frequency of busClock_Hz configured to SD bus.
 */
uint32_t USDHC_SetSdClock(USDHC_Type *base, uint32_t srcClock_Hz, uint32_t busClock_Hz);

/*!
 * @brief Sends 80 clocks to the card to set it to the active state.
 *
 * This function must be called each time the card is inserted to ensure that the card can receive the command
 * correctly.
 *
 * @param base USDHC peripheral base address.
 * @param timeout Timeout to initialize card.
 * @retval true Set card active successfully.
 * @retval false Set card active failed.
 */
bool USDHC_SetCardActive(USDHC_Type *base, uint32_t timeout);

/*!
 * @brief trigger a hardware reset.
 *
 * @param base USDHC peripheral base address.
 * @param 1 or 0 level
 */
static inline void USDHC_AssertHardwareReset(USDHC_Type *base, bool high)
{
    if (high)
    {
        base->SYS_CTRL |= USDHC_SYS_CTRL_IPP_RST_N_MASK;
    }
    else
    {
        base->SYS_CTRL &= ~USDHC_SYS_CTRL_IPP_RST_N_MASK;
    }
}

/*!
 * @brief Sets the data transfer width.
 *
 * @param base USDHC peripheral base address.
 * @param width Data transfer width.
 */
static inline void USDHC_SetDataBusWidth(USDHC_Type *base, usdhc_data_bus_width_t width)
{
    base->PROT_CTRL = ((base->PROT_CTRL & ~USDHC_PROT_CTRL_DTW_MASK) | USDHC_PROT_CTRL_DTW(width));
}

/*!
 * @brief Fills the data port.
 *
 * This function is used to implement the data transfer by Data Port instead of DMA.
 *
 * @param base USDHC peripheral base address.
 * @param data The data about to be sent.
 */
static inline void USDHC_WriteData(USDHC_Type *base, uint32_t data)
{
    base->DATA_BUFF_ACC_PORT = data;
}

/*!
 * @brief Retrieves the data from the data port.
 *
 * This function is used to implement the data transfer by Data Port instead of DMA.
 *
 * @param base USDHC peripheral base address.
 * @return The data has been read.
 */
static inline uint32_t USDHC_ReadData(USDHC_Type *base)
{
    return base->DATA_BUFF_ACC_PORT;
}

/*!
 * @brief send command function
 *
 * @param base USDHC peripheral base address.
 * @param command configuration
 */
void USDHC_SendCommand(USDHC_Type *base, usdhc_command_t *command);

/*!
 * @brief Enables or disables a wakeup event in low-power mode.
 *
 * @param base USDHC peripheral base address.
 * @param mask Wakeup events mask(_usdhc_wakeup_event).
 * @param enable True to enable, false to disable.
 */
static inline void USDHC_EnableWakeupEvent(USDHC_Type *base, uint32_t mask, bool enable)
{
    if (enable)
    {
        base->PROT_CTRL |= mask;
    }
    else
    {
        base->PROT_CTRL &= ~mask;
    }
}

/*!
 * @brief detect card insert status.
 *
 * @param base USDHC peripheral base address.
 * @param enable/disable flag
 */
static inline void USDHC_CardDetectByData3(USDHC_Type *base, bool enable)
{
    if (enable)
    {
        base->PROT_CTRL |= USDHC_PROT_CTRL_D3CD_MASK;
    }
    else
    {
        base->PROT_CTRL &= ~USDHC_PROT_CTRL_D3CD_MASK;
    }
}

/*!
 * @brief detect card insert status.
 *
 * @param base USDHC peripheral base address.
 */
static inline bool USDHC_DetectCardInsert(USDHC_Type *base)
{
    return (base->PRES_STATE & kUSDHC_CardInsertedFlag) ? true : false;
}

/*!
 * @brief Enables or disables the SDIO card control.
 *
 * @param base USDHC peripheral base address.
 * @param mask SDIO card control flags mask(_usdhc_sdio_control_flag).
 * @param enable True to enable, false to disable.
 */
static inline void USDHC_EnableSdioControl(USDHC_Type *base, uint32_t mask, bool enable)
{
    if (enable)
    {
        base->PROT_CTRL |= mask;
    }
    else
    {
        base->PROT_CTRL &= ~mask;
    }
}

/*!
 * @brief Restarts a transaction which has stopped at the block GAP for the SDIO card.
 *
 * @param base USDHC peripheral base address.
 */
static inline void USDHC_SetContinueRequest(USDHC_Type *base)
{
    base->PROT_CTRL |= USDHC_PROT_CTRL_CREQ_MASK;
}

/*!
 * @brief Request stop at block gap function.
 *
 * @param base USDHC peripheral base address.
 * @param enable true to stop at block gap, false to normal transfer
 */
static inline void USDHC_RequestStopAtBlockGap(USDHC_Type *base, bool enable)
{
    if (enable)
    {
        base->PROT_CTRL |= USDHC_PROT_CTRL_SABGREQ_MASK;
    }
    else
    {
        base->PROT_CTRL &= ~USDHC_PROT_CTRL_SABGREQ_MASK;
    }
}

/*!
 * @brief Configures the MMC boot feature.
 *
 * Example:
   @code
   usdhc_boot_config_t config;
   config.ackTimeoutCount = 4;
   config.bootMode = kUSDHC_BootModeNormal;
   config.blockCount = 5;
   config.enableBootAck = true;
   config.enableBoot = true;
   config.enableAutoStopAtBlockGap = true;
   USDHC_SetMmcBootConfig(USDHC, &config);
   @endcode
 *
 * @param base USDHC peripheral base address.
 * @param config The MMC boot configuration information.
 */
void USDHC_SetMmcBootConfig(USDHC_Type *base, const usdhc_boot_config_t *config);

/*!
 * @brief Enables or disables the mmc boot mode.
 *
 * @param base USDHC peripheral base address.
 * @param enable True to enable, false to disable.
 */
static inline void USDHC_EnableMmcBoot(USDHC_Type *base, bool enable)
{
    if (enable)
    {
        base->MMC_BOOT |= USDHC_MMC_BOOT_BOOT_EN_MASK;
    }
    else
    {
        base->MMC_BOOT &= ~USDHC_MMC_BOOT_BOOT_EN_MASK;
    }
}

/*!
 * @brief Forces generating events according to the given mask.
 *
 * @param base USDHC peripheral base address.
 * @param mask The force events bit posistion (_usdhc_force_event).
 */
static inline void USDHC_SetForceEvent(USDHC_Type *base, uint32_t mask)
{
    base->FORCE_EVENT = mask;
}

/*!
 * @brief select the usdhc output voltage
 *
 * @param base USDHC peripheral base address.
 * @param true 1.8V, false 3.0V
 */
static inline void UDSHC_SelectVoltage(USDHC_Type *base, bool en18v)
{
    if (en18v)
    {
        base->VEND_SPEC |= USDHC_VEND_SPEC_VSELECT_MASK;
    }
    else
    {
        base->VEND_SPEC &= ~USDHC_VEND_SPEC_VSELECT_MASK;
    }
}

#if defined(FSL_FEATURE_USDHC_HAS_SDR50_MODE) && (FSL_FEATURE_USDHC_HAS_SDR50_MODE)
/*!
 * @brief check the SDR50 mode request tuning bit
 * When this bit set, user should call USDHC_StandardTuning function
 * @param base USDHC peripheral base address.
 */
static inline bool USDHC_RequestTuningForSDR50(USDHC_Type *base)
{
    return base->HOST_CTRL_CAP & USDHC_HOST_CTRL_CAP_USE_TUNING_SDR50_MASK ? true : false;
}

/*!
 * @brief check the request re-tuning bit
 * When this bit is set, user should do manual tuning or standard tuning function
 * @param base USDHC peripheral base address.
 */
static inline bool USDHC_RequestReTuning(USDHC_Type *base)
{
    return base->PRES_STATE & USDHC_PRES_STATE_RTR_MASK ? true : false;
}

/*!
 * @brief the SDR104 mode auto tuning enable and disable
 * This function should call after tuning function execute pass, auto tuning will handle
 * by hardware
 * @param base USDHC peripheral base address.
 * @param enable/disable flag
 */
static inline void USDHC_EnableAutoTuning(USDHC_Type *base, bool enable)
{
    if (enable)
    {
        base->MIX_CTRL |= USDHC_MIX_CTRL_AUTO_TUNE_EN_MASK;
    }
    else
    {
        base->MIX_CTRL &= ~USDHC_MIX_CTRL_AUTO_TUNE_EN_MASK;
    }
}

/*!
 * @brief the config the re-tuning timer for mode 1 and mode 3
 * This timer is used for standard tuning auto re-tuning,
 * @param base USDHC peripheral base address.
 * @param timer counter value
 */
static inline void USDHC_SetRetuningTimer(USDHC_Type *base, uint32_t counter)
{
    base->HOST_CTRL_CAP &= ~USDHC_HOST_CTRL_CAP_TIME_COUNT_RETUNING_MASK;
    base->HOST_CTRL_CAP |= USDHC_HOST_CTRL_CAP_TIME_COUNT_RETUNING(counter);
}

/*!
 * @brief the auto tuning enbale for CMD/DATA line
 *
 * @param base USDHC peripheral base address.
 */
void USDHC_EnableAutoTuningForCmdAndData(USDHC_Type *base);

/*!
 * @brief manual tuning trigger or abort
 * User should handle the tuning cmd and find the boundary of the delay
 * then calucate a average value which will be config to the CLK_TUNE_CTRL_STATUS
 * This function should called before USDHC_AdjustDelayforSDR104 function
 * @param base USDHC peripheral base address.
 * @param tuning enable flag
 */
void USDHC_EnableManualTuning(USDHC_Type *base, bool enable);

/*!
 * @brief the SDR104 mode delay setting adjust
 * This function should called after USDHC_ManualTuningForSDR104
 * @param base USDHC peripheral base address.
 * @param delay setting configuration
 * @retval kStatus_Fail config the delay setting fail
 * @retval kStatus_Success config the delay setting success
 */
status_t USDHC_AdjustDelayForManualTuning(USDHC_Type *base, uint32_t delay);

/*!
 * @brief the enable standard tuning function
 * The standard tuning window and tuning counter use the default config
 * tuning cmd is send by the software, user need to check the tuning result
 * can be used for SDR50,SDR104,HS200 mode tuning
 * @param base USDHC peripheral base address.
 * @param tuning start tap
 * @param tuning step
 * @param enable/disable flag
 */
void USDHC_EnableStandardTuning(USDHC_Type *base, uint32_t tuningStartTap, uint32_t step, bool enable);

/*!
 * @brief Get execute std tuning status
 *
 * @param base USDHC peripheral base address.
 */
static inline uint32_t USDHC_GetExecuteStdTuningStatus(USDHC_Type *base)
{
    return (base->AUTOCMD12_ERR_STATUS & USDHC_AUTOCMD12_ERR_STATUS_EXECUTE_TUNING_MASK);
}

/*!
 * @brief check std tuning result
 *
 * @param base USDHC peripheral base address.
 */
static inline uint32_t USDHC_CheckStdTuningResult(USDHC_Type *base)
{
    return (base->AUTOCMD12_ERR_STATUS & USDHC_AUTOCMD12_ERR_STATUS_SMP_CLK_SEL_MASK);
}

/*!
 * @brief check tuning error
 *
 * @param base USDHC peripheral base address.
 */
static inline uint32_t USDHC_CheckTuningError(USDHC_Type *base)
{
    return (base->CLK_TUNE_CTRL_STATUS &
            (USDHC_CLK_TUNE_CTRL_STATUS_NXT_ERR_MASK | USDHC_CLK_TUNE_CTRL_STATUS_PRE_ERR_MASK));
}

#endif
/*!
 * @brief the enable/disable DDR mode
 *
 * @param base USDHC peripheral base address.
 * @param enable/disable flag
 * @param nibble position
 */
void USDHC_EnableDDRMode(USDHC_Type *base, bool enable, uint32_t nibblePos);

/*!
 * @brief the enable/disable HS400 mode
 *
 * @param base USDHC peripheral base address.
 * @param enable/disable flag
 */
#if FSL_FEATURE_USDHC_HAS_HS400_MODE
static inline void USDHC_EnableHS400Mode(USDHC_Type *base, bool enable)
{
    if (enable)
    {
        base->MIX_CTRL |= USDHC_MIX_CTRL_HS400_MODE_MASK;
    }
    else
    {
        base->MIX_CTRL &= ~USDHC_MIX_CTRL_HS400_MODE_MASK;
    }
}

/*!
 * @brief reset the strobe DLL
 *
 * @param base USDHC peripheral base address.
 */
static inline void USDHC_ResetStrobeDLL(USDHC_Type *base)
{
    base->STROBE_DLL_CTRL |= USDHC_STROBE_DLL_CTRL_STROBE_DLL_CTRL_RESET_MASK;
}

/*!
 * @brief enable/disable the strobe DLL
 *
 * @param base USDHC peripheral base address.
 * @param enable/disable flag
 */
static inline void USDHC_EnableStrobeDLL(USDHC_Type *base, bool enable)
{
    if (enable)
    {
        base->STROBE_DLL_CTRL |= USDHC_STROBE_DLL_CTRL_STROBE_DLL_CTRL_ENABLE_MASK;
    }
    else
    {
        base->STROBE_DLL_CTRL &= ~USDHC_STROBE_DLL_CTRL_STROBE_DLL_CTRL_ENABLE_MASK;
    }
}

/*!
 * @brief config the strobe DLL delay target and update interval
 *
 * @param base USDHC peripheral base address.
 * @param delay target
 * @param update interval
 */
static inline void USDHC_ConfigStrobeDLL(USDHC_Type *base, uint32_t delayTarget, uint32_t updateInterval)
{
    base->STROBE_DLL_CTRL &= (USDHC_STROBE_DLL_CTRL_STROBE_DLL_CTRL_SLV_UPDATE_INT_MASK |
                              USDHC_STROBE_DLL_CTRL_STROBE_DLL_CTRL_SLV_DLY_TARGET_MASK);

    base->STROBE_DLL_CTRL |= USDHC_STROBE_DLL_CTRL_STROBE_DLL_CTRL_SLV_UPDATE_INT(updateInterval) |
                             USDHC_STROBE_DLL_CTRL_STROBE_DLL_CTRL_SLV_DLY_TARGET(delayTarget);
}

/*!
 * @brief get the strobe DLL status
 *
 * @param base USDHC peripheral base address.
 */
static inline uint32_t USDHC_GetStrobeDLLStatus(USDHC_Type *base)
{
    return base->STROBE_DLL_STATUS;
}

#endif

/* @} */

/*!
 * @name Transactional
 * @{
 */

/*!
 * @brief Transfers the command/data using a blocking method.
 *
 * This function waits until the command response/data is received or the USDHC encounters an error by polling the
 * status
 * flag.
 * The application must not call this API in multiple threads at the same time. Because of that this API doesn't
 * support the re-entry mechanism.
 *
 * @note There is no need to call the API 'USDHC_TransferCreateHandle' when calling this API.
 *
 * @param base USDHC peripheral base address.
 * @param adma configuration
 * @param transfer Transfer content.
 * @retval kStatus_InvalidArgument Argument is invalid.
 * @retval kStatus_USDHC_PrepareAdmaDescriptorFailed Prepare ADMA descriptor failed.
 * @retval kStatus_USDHC_SendCommandFailed Send command failed.
 * @retval kStatus_USDHC_TransferDataFailed Transfer data failed.
 * @retval kStatus_Success Operate successfully.
 */
status_t USDHC_TransferBlocking(USDHC_Type *base, usdhc_adma_config_t *dmaConfig, usdhc_transfer_t *transfer);

/*!
 * @brief Creates the USDHC handle.
 *
 * @param base USDHC peripheral base address.
 * @param handle USDHC handle pointer.
 * @param callback Structure pointer to contain all callback functions.
 * @param userData Callback function parameter.
 */
void USDHC_TransferCreateHandle(USDHC_Type *base,
                                usdhc_handle_t *handle,
                                const usdhc_transfer_callback_t *callback,
                                void *userData);

/*!
 * @brief Transfers the command/data using an interrupt and an asynchronous method.
 *
 * This function sends a command and data and returns immediately. It doesn't wait the transfer complete or
 * encounter an error. The application must not call this API in multiple threads at the same time. Because of that
 * this API doesn't support the re-entry mechanism.
 *
 * @note Call the API 'USDHC_TransferCreateHandle' when calling this API.
 *
 * @param base USDHC peripheral base address.
 * @param handle USDHC handle.
 * @param adma configuration.
 * @param transfer Transfer content.
 * @retval kStatus_InvalidArgument Argument is invalid.
 * @retval kStatus_USDHC_BusyTransferring Busy transferring.
 * @retval kStatus_USDHC_PrepareAdmaDescriptorFailed Prepare ADMA descriptor failed.
 * @retval kStatus_Success Operate successfully.
 */
status_t USDHC_TransferNonBlocking(USDHC_Type *base,
                                   usdhc_handle_t *handle,
                                   usdhc_adma_config_t *dmaConfig,
                                   usdhc_transfer_t *transfer);

/*!
 * @brief IRQ handler for the USDHC.
 *
 * This function deals with the IRQs on the given host controller.
 *
 * @param base USDHC peripheral base address.
 * @param handle USDHC handle.
 */
void USDHC_TransferHandleIRQ(USDHC_Type *base, usdhc_handle_t *handle);

/* @} */

#if defined(__cplusplus)
}
#endif
/*! @} */

#endif /* _FSL_USDHC_H_*/
