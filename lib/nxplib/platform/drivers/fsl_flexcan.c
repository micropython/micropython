/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_flexcan.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.flexcan"
#endif

#if (defined(FSL_FEATURE_FLEXCAN_HAS_ERRATA_6032) && FSL_FEATURE_FLEXCAN_HAS_ERRATA_6032)
#define RXINTERMISSION (CAN_DBG1_CFSM(0x2f))
#define TXINTERMISSION (CAN_DBG1_CFSM(0x14))
#define BUSIDLE (CAN_DBG1_CFSM(0x02))
#define CBN_VALUE3 (CAN_DBG1_CBN(0x03))
#define DELAY_BUSIDLE (200)
#endif

#define IDEAL_SP_LOW (750U)
#define IDEAL_SP_MID (800U)
#define IDEAL_SP_HIGH (875U)
#define IDEAL_SP_FACTOR (1000U)

#define MAX_PROPSEG (CAN_CTRL1_PROPSEG_MASK >> CAN_CTRL1_PROPSEG_SHIFT)
#define MAX_PSEG1 (CAN_CTRL1_PSEG1_MASK >> CAN_CTRL1_PSEG1_SHIFT)
#define MAX_PSEG2 (CAN_CTRL1_PSEG2_MASK >> CAN_CTRL1_PSEG2_SHIFT)
#define MAX_RJW (CAN_CTRL1_RJW_MASK >> CAN_CTRL1_RJW_SHIFT)
#define MAX_PRESDIV (CAN_CTRL1_PRESDIV_MASK >> CAN_CTRL1_PRESDIV_SHIFT)
#define CTRL1_MAX_TIME_QUANTA (1U + MAX_PROPSEG + 1U + MAX_PSEG1 + 1U + MAX_PSEG2 + 1U)
#define CTRL1_MIN_TIME_QUANTA (8U)

#define MAX_EPROPSEG (CAN_CBT_EPROPSEG_MASK >> CAN_CBT_EPROPSEG_SHIFT)
#define MAX_EPSEG1 (CAN_CBT_EPSEG1_MASK >> CAN_CBT_EPSEG1_SHIFT)
#define MAX_EPSEG2 (CAN_CBT_EPSEG2_MASK >> CAN_CBT_EPSEG2_SHIFT)
#define MAX_ERJW (CAN_CBT_ERJW_MASK >> CAN_CBT_ERJW_SHIFT)
#define MAX_EPRESDIV (CAN_CBT_EPRESDIV_MASK >> CAN_CBT_EPRESDIV_SHIFT)
#define CBT_MAX_TIME_QUANTA (1U + MAX_EPROPSEG + 1U + MAX_EPSEG1 + 1U + MAX_EPSEG2 + 1U)
#define CBT_MIN_TIME_QUANTA (8U)

#define FDCBT_MAX_TIME_QUANTA (1U + MAX_FPROPSEG + 0U + MAX_FPSEG1 + 1U + MAX_FPSEG2 + 1U)
#define FDCBT_MIN_TIME_QUANTA (5U)
#define MAX_FPROPSEG (CAN_FDCBT_FPROPSEG_MASK >> CAN_FDCBT_FPROPSEG_SHIFT)
#define MAX_FPSEG1 (CAN_FDCBT_FPSEG1_MASK >> CAN_FDCBT_FPSEG1_SHIFT)
#define MAX_FPSEG2 (CAN_FDCBT_FPSEG2_MASK >> CAN_FDCBT_FPSEG2_SHIFT)
#define MAX_FRJW (CAN_FDCBT_FRJW_MASK >> CAN_FDCBT_FRJW_SHIFT)
#define MAX_FPRESDIV (CAN_FDCBT_FPRESDIV_MASK >> CAN_FDCBT_FPRESDIV_SHIFT)

#define MAX_CANFD_BAUDRATE (8000000U)
#define MAX_CAN_BAUDRATE (1000000U)

#if (defined(FSL_FEATURE_FLEXCAN_HAS_ERRATA_9595) && FSL_FEATURE_FLEXCAN_HAS_ERRATA_9595)
#define CAN_ESR1_FLTCONF_BUSOFF CAN_ESR1_FLTCONF(2U)
#endif

/*! @brief FlexCAN Internal State. */
enum _flexcan_state
{
    kFLEXCAN_StateIdle     = 0x0, /*!< MB/RxFIFO idle.*/
    kFLEXCAN_StateRxData   = 0x1, /*!< MB receiving.*/
    kFLEXCAN_StateRxRemote = 0x2, /*!< MB receiving remote reply.*/
    kFLEXCAN_StateTxData   = 0x3, /*!< MB transmitting.*/
    kFLEXCAN_StateTxRemote = 0x4, /*!< MB transmitting remote request.*/
    kFLEXCAN_StateRxFifo   = 0x5, /*!< RxFIFO receiving.*/
};

/*! @brief FlexCAN message buffer CODE for Rx buffers. */
enum _flexcan_mb_code_rx
{
    kFLEXCAN_RxMbInactive = 0x0, /*!< MB is not active.*/
    kFLEXCAN_RxMbFull     = 0x2, /*!< MB is full.*/
    kFLEXCAN_RxMbEmpty    = 0x4, /*!< MB is active and empty.*/
    kFLEXCAN_RxMbOverrun  = 0x6, /*!< MB is overwritten into a full buffer.*/
    kFLEXCAN_RxMbBusy     = 0x8, /*!< FlexCAN is updating the contents of the MB.*/
                                 /*!  The CPU must not access the MB.*/
    kFLEXCAN_RxMbRanswer = 0xA,  /*!< A frame was configured to recognize a Remote Request Frame */
                                 /*!  and transmit a Response Frame in return.*/
    kFLEXCAN_RxMbNotUsed = 0xF,  /*!< Not used.*/
};

/*! @brief FlexCAN message buffer CODE FOR Tx buffers. */
enum _flexcan_mb_code_tx
{
    kFLEXCAN_TxMbInactive     = 0x8, /*!< MB is not active.*/
    kFLEXCAN_TxMbAbort        = 0x9, /*!< MB is aborted.*/
    kFLEXCAN_TxMbDataOrRemote = 0xC, /*!< MB is a TX Data Frame(when MB RTR = 0) or */
                                     /*!< MB is a TX Remote Request Frame (when MB RTR = 1).*/
    kFLEXCAN_TxMbTanswer = 0xE,      /*!< MB is a TX Response Request Frame from */
                                     /*!  an incoming Remote Request Frame.*/
    kFLEXCAN_TxMbNotUsed = 0xF,      /*!< Not used.*/
};

/* Typedef for interrupt handler. */
typedef void (*flexcan_isr_t)(CAN_Type *base, flexcan_handle_t *handle);

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*!
 * @brief Enter FlexCAN Freeze Mode.
 *
 * This function makes the FlexCAN work under Freeze Mode.
 *
 * @param base FlexCAN peripheral base address.
 */
static void FLEXCAN_EnterFreezeMode(CAN_Type *base);

/*!
 * @brief Exit FlexCAN Freeze Mode.
 *
 * This function makes the FlexCAN leave Freeze Mode.
 *
 * @param base FlexCAN peripheral base address.
 */
static void FLEXCAN_ExitFreezeMode(CAN_Type *base);

#if !defined(NDEBUG)
/*!
 * @brief Check if Message Buffer is occupied by Rx FIFO.
 *
 * This function check if Message Buffer is occupied by Rx FIFO.
 *
 * @param base FlexCAN peripheral base address.
 * @param mbIdx The FlexCAN Message Buffer index.
 */
static bool FLEXCAN_IsMbOccupied(CAN_Type *base, uint8_t mbIdx);
#endif

#if ((defined(FSL_FEATURE_FLEXCAN_HAS_ERRATA_5641) && FSL_FEATURE_FLEXCAN_HAS_ERRATA_5641) || \
     (defined(FSL_FEATURE_FLEXCAN_HAS_ERRATA_5829) && FSL_FEATURE_FLEXCAN_HAS_ERRATA_5829))
/*!
 * @brief Get the first valid Message buffer ID of give FlexCAN instance.
 *
 * This function is a helper function for Errata 5641 workaround.
 *
 * @param base FlexCAN peripheral base address.
 * @return The first valid Message Buffer Number.
 */
static uint32_t FLEXCAN_GetFirstValidMb(CAN_Type *base);
#endif

/*!
 * @brief Check if Message Buffer interrupt is enabled.
 *
 * This function check if Message Buffer interrupt is enabled.
 *
 * @param base FlexCAN peripheral base address.
 * @param mbIdx The FlexCAN Message Buffer index.
 */
static bool FLEXCAN_IsMbIntEnabled(CAN_Type *base, uint8_t mbIdx);

/*!
 * @brief Reset the FlexCAN Instance.
 *
 * Restores the FlexCAN module to reset state, notice that this function
 * will set all the registers to reset state so the FlexCAN module can not work
 * after calling this API.
 *
 * @param base FlexCAN peripheral base address.
 */
static void FLEXCAN_Reset(CAN_Type *base);

/*!
 * @brief Set Baud Rate of FlexCAN.
 *
 * This function set the baud rate of FlexCAN.
 *
 * @param base FlexCAN peripheral base address.
 * @param sourceClock_Hz Source Clock in Hz.
 * @param baudRate_Bps Baud Rate in Bps.
 * @param timingConfig FlexCAN timingConfig.
 */
static void FLEXCAN_SetBaudRate(CAN_Type *base,
                                uint32_t sourceClock_Hz,
                                uint32_t baudRate_Bps,
                                flexcan_timing_config_t timingConfig);
/*!
 * @brief Calculates the segment values for a single bit time for classical CAN
 *
 * @param baudRate The data speed in bps
 * @param tqNum Number of time quantas per bit
 * @param pconfig Pointer to the FlexCAN timing configuration structure.
 *
 */
static void FLEXCAN_GetSegments(uint32_t baudRate, uint32_t tqNum, flexcan_timing_config_t *pconfig);

#if (defined(FSL_FEATURE_FLEXCAN_HAS_FLEXIBLE_DATA_RATE) && FSL_FEATURE_FLEXCAN_HAS_FLEXIBLE_DATA_RATE)
/*!
 * @brief Set Baud Rate of FlexCAN FD frame.
 *
 * This function set the baud rate of FlexCAN FD frame.
 *
 * @param base FlexCAN peripheral base address.
 * @param sourceClock_Hz Source Clock in Hz.
 * @param baudRateFD_Bps FD frame Baud Rate in Bps.
 * @param timingConfig FlexCAN timingConfig.
 */
static void FLEXCAN_SetFDBaudRate(CAN_Type *base,
                                  uint32_t sourceClock_Hz,
                                  uint32_t baudRateFD_Bps,
                                  flexcan_timing_config_t timingConfig);

/*!
 * @brief Get Mailbox offset number by dword.
 *
 * This function gets the offset number of the specified mailbox.
 * Mailbox is not consecutive between memory regions when payload is not 8 bytes
 * so need to calculate the specified mailbox address.
 * For example, in the first memory region, MB[0].CS address is 0x4002_4080. For 32 bytes
 * payload frame, the second mailbox is ((1/12)*512 + 1%12*40)/4 = 10, meaning 10 dword
 * after the 0x4002_4080, which is actually the address of mailbox MB[1].CS.
 *
 * @param base FlexCAN peripheral base address.
 * @param mbIdx Mailbox index.
 */
static uint32_t FLEXCAN_GetFDMailboxOffset(CAN_Type *base, uint8_t mbIdx);

/*!
 * @brief Calculates the segment values for a single bit time for CANFD bus control baud Rate
 *
 * @param baudRate The canfd bus control speed in bps
 * @param tqNum Number of time quanta per bit
 * @param pconfig Pointer to the FlexCAN timing configuration structure.
 *
 */
static void FLEXCAN_FDGetSegments(uint32_t baudRateFD, uint32_t tqNum, flexcan_timing_config_t *pconfig);
/*!
 * @brief Calculates the segment values for a single bit time for CANFD bus data baud Rate
 *
 * @param baudRate The canfd bus data speed in bps
 * @param tqNum Number of time quanta per bit
 * @param pconfig Pointer to the FlexCAN timing configuration structure.
 *
 */
static void FLEXCAN_FDGetSegmentswithBRS(uint32_t baudRateFDbrs, uint32_t tqNum, flexcan_timing_config_t *pconfig);

#endif

/*******************************************************************************
 * Variables
 ******************************************************************************/

/* Array of FlexCAN peripheral base address. */
static CAN_Type *const s_flexcanBases[] = CAN_BASE_PTRS;

/* Array of FlexCAN IRQ number. */
static const IRQn_Type s_flexcanRxWarningIRQ[] = CAN_Rx_Warning_IRQS;
static const IRQn_Type s_flexcanTxWarningIRQ[] = CAN_Tx_Warning_IRQS;
static const IRQn_Type s_flexcanWakeUpIRQ[]    = CAN_Wake_Up_IRQS;
static const IRQn_Type s_flexcanErrorIRQ[]     = CAN_Error_IRQS;
static const IRQn_Type s_flexcanBusOffIRQ[]    = CAN_Bus_Off_IRQS;
static const IRQn_Type s_flexcanMbIRQ[]        = CAN_ORed_Message_buffer_IRQS;

/* Array of FlexCAN handle. */
static flexcan_handle_t *s_flexcanHandle[ARRAY_SIZE(s_flexcanBases)];

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
/* Array of FlexCAN clock name. */
static const clock_ip_name_t s_flexcanClock[] = FLEXCAN_CLOCKS;
#if defined(FLEXCAN_PERIPH_CLOCKS)
/* Array of FlexCAN serial clock name. */
static const clock_ip_name_t s_flexcanPeriphClock[] = FLEXCAN_PERIPH_CLOCKS;
#endif /* FLEXCAN_PERIPH_CLOCKS */
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

/* FlexCAN ISR for transactional APIs. */
static flexcan_isr_t s_flexcanIsr;

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * brief Get the FlexCAN instance from peripheral base address.
 *
 * param base FlexCAN peripheral base address.
 * return FlexCAN instance.
 */
uint32_t FLEXCAN_GetInstance(CAN_Type *base)
{
    uint32_t instance;

    /* Find the instance index from base address mappings. */
    for (instance = 0; instance < ARRAY_SIZE(s_flexcanBases); instance++)
    {
        if (s_flexcanBases[instance] == base)
        {
            break;
        }
    }

    assert(instance < ARRAY_SIZE(s_flexcanBases));

    return instance;
}

#if (defined(FSL_FEATURE_FLEXCAN_HAS_ERRATA_9595) && FSL_FEATURE_FLEXCAN_HAS_ERRATA_9595)
static void FLEXCAN_EnterFreezeMode(CAN_Type *base)
{
    uint32_t u32TimeoutCount = 0U;
    uint32_t u32TempMCR      = 0U;
    uint32_t u32TempIMASK1   = 0U;
#if (defined(FSL_FEATURE_FLEXCAN_HAS_EXTENDED_FLAG_REGISTER)) && (FSL_FEATURE_FLEXCAN_HAS_EXTENDED_FLAG_REGISTER > 0)
    uint32_t u32TempIMASK2 = 0U;
#endif

    /* Step1: set FRZ enable in MCR. */
    base->MCR |= CAN_MCR_FRZ_MASK;

    /* Step2: to check if MDIS bit set in MCR. if yes, clear it. */
    if (0U != (base->MCR & CAN_MCR_MDIS_MASK))
    {
        base->MCR &= ~CAN_MCR_MDIS_MASK;
    }

    /* Step3: polling LPMACK. */
    u32TimeoutCount = (uint32_t)FLEXCAN_WAIT_TIMEOUT;
    while ((0U == (base->MCR & CAN_MCR_LPMACK_MASK)) && (u32TimeoutCount > 0U))
    {
        u32TimeoutCount--;
    }

    /* Step4: to check FLTCONF in ESR1 register */
    if (0U == (base->ESR1 & CAN_ESR1_FLTCONF_BUSOFF))
    {
        /* Step5B: Set Halt bits. */
        base->MCR |= CAN_MCR_HALT_MASK;

        /* Step6B: Poll the MCR register until the Freeze Acknowledge (FRZACK) bit is set, timeout need more than 178
         * CAN bit length, so 20 multiply timeout is enough. */
        u32TimeoutCount = (uint32_t)FLEXCAN_WAIT_TIMEOUT * 20U;
        while ((0U == (base->MCR & CAN_MCR_FRZACK_MASK)) && (u32TimeoutCount > 0U))
        {
            u32TimeoutCount--;
        }
    }
    else
    {
        /* backup MCR and IMASK register. Errata document not descript it, but we need backup for step 8A and 9A. */
        u32TempMCR    = base->MCR;
        u32TempIMASK1 = base->IMASK1;
#if (defined(FSL_FEATURE_FLEXCAN_HAS_EXTENDED_FLAG_REGISTER)) && (FSL_FEATURE_FLEXCAN_HAS_EXTENDED_FLAG_REGISTER > 0)
        u32TempIMASK2 = base->IMASK2;
#endif

        /* Step5A: Set the Soft Reset bit ((SOFTRST) in the MCR.*/
        base->MCR |= CAN_MCR_SOFTRST_MASK;

        /* Step6A: Poll the MCR register until the Soft Reset (SOFTRST) bit is cleared. */
        u32TimeoutCount = (uint32_t)FLEXCAN_WAIT_TIMEOUT;
        while ((CAN_MCR_SOFTRST_MASK == (base->MCR & CAN_MCR_SOFTRST_MASK)) && (u32TimeoutCount > 0U))
        {
            u32TimeoutCount--;
        }

        /* Step7A: Poll the MCR register until the Freeze Acknowledge (FRZACK) bit is set. */
        u32TimeoutCount = (uint32_t)FLEXCAN_WAIT_TIMEOUT;
        while ((0U == (base->MCR & CAN_MCR_FRZACK_MASK)) && (u32TimeoutCount > 0U))
        {
            u32TimeoutCount--;
        }

        /* Step8A: reconfig MCR. */
        base->MCR = u32TempMCR;

        /* Step9A: reconfig IMASK. */
        base->IMASK1 = u32TempIMASK1;
#if (defined(FSL_FEATURE_FLEXCAN_HAS_EXTENDED_FLAG_REGISTER)) && (FSL_FEATURE_FLEXCAN_HAS_EXTENDED_FLAG_REGISTER > 0)
        base->IMASK2 = u32TempIMASK2;
#endif
    }
}
#else
static void FLEXCAN_EnterFreezeMode(CAN_Type *base)
{
    /* Set Freeze, Halt bits. */
    base->MCR |= CAN_MCR_FRZ_MASK;
    base->MCR |= CAN_MCR_HALT_MASK;
    while (0U == (base->MCR & CAN_MCR_FRZACK_MASK))
    {
    }
}
#endif

static void FLEXCAN_ExitFreezeMode(CAN_Type *base)
{
    /* Clear Freeze, Halt bits. */
    base->MCR &= ~CAN_MCR_HALT_MASK;
    base->MCR &= ~CAN_MCR_FRZ_MASK;

    /* Wait until the FlexCAN Module exit freeze mode. */
    while (0U != (base->MCR & CAN_MCR_FRZACK_MASK))
    {
    }
}

#if !defined(NDEBUG)
static bool FLEXCAN_IsMbOccupied(CAN_Type *base, uint8_t mbIdx)
{
    uint8_t lastOccupiedMb;

    /* Is Rx FIFO enabled? */
    if (base->MCR & CAN_MCR_RFEN_MASK)
    {
        /* Get RFFN value. */
        lastOccupiedMb = (uint8_t)((base->CTRL2 & CAN_CTRL2_RFFN_MASK) >> CAN_CTRL2_RFFN_SHIFT);
        /* Calculate the number of last Message Buffer occupied by Rx FIFO. */
        lastOccupiedMb = ((lastOccupiedMb + 1U) * 2U) + 5U;

#if ((defined(FSL_FEATURE_FLEXCAN_HAS_ERRATA_5641) && FSL_FEATURE_FLEXCAN_HAS_ERRATA_5641) || \
     (defined(FSL_FEATURE_FLEXCAN_HAS_ERRATA_5829) && FSL_FEATURE_FLEXCAN_HAS_ERRATA_5829))
        if (mbIdx <= (lastOccupiedMb + 1U))
#else
        if (mbIdx <= lastOccupiedMb)
#endif
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
#if ((defined(FSL_FEATURE_FLEXCAN_HAS_ERRATA_5641) && FSL_FEATURE_FLEXCAN_HAS_ERRATA_5641) || \
     (defined(FSL_FEATURE_FLEXCAN_HAS_ERRATA_5829) && FSL_FEATURE_FLEXCAN_HAS_ERRATA_5829))
        if (0U == mbIdx)
        {
            return true;
        }
        else
        {
            return false;
        }
#else
        return false;
#endif
    }
}
#endif

#if ((defined(FSL_FEATURE_FLEXCAN_HAS_ERRATA_5641) && FSL_FEATURE_FLEXCAN_HAS_ERRATA_5641) || \
     (defined(FSL_FEATURE_FLEXCAN_HAS_ERRATA_5829) && FSL_FEATURE_FLEXCAN_HAS_ERRATA_5829))
static uint32_t FLEXCAN_GetFirstValidMb(CAN_Type *base)
{
    uint32_t firstValidMbNum;

    if (base->MCR & CAN_MCR_RFEN_MASK)
    {
        firstValidMbNum = ((base->CTRL2 & CAN_CTRL2_RFFN_MASK) >> CAN_CTRL2_RFFN_SHIFT);
        firstValidMbNum = ((firstValidMbNum + 1U) * 2U) + 6U;
    }
    else
    {
        firstValidMbNum = 0;
    }

    return firstValidMbNum;
}
#endif

static bool FLEXCAN_IsMbIntEnabled(CAN_Type *base, uint8_t mbIdx)
{
    /* Assertion. */
    assert(mbIdx < (uint8_t)FSL_FEATURE_FLEXCAN_HAS_MESSAGE_BUFFER_MAX_NUMBERn(base));

#if (FSL_FEATURE_FLEXCAN_HAS_EXTENDED_FLAG_REGISTER > 0)
    if (mbIdx < 32U)
    {
#endif
        if (0U != (base->IMASK1 & (((uint32_t)1U << mbIdx))))
        {
            return true;
        }
        else
        {
            return false;
        }
#if (FSL_FEATURE_FLEXCAN_HAS_EXTENDED_FLAG_REGISTER > 0)
    }
    else
    {
        if (base->IMASK2 & (1U << (mbIdx - 32U)))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
#endif
}

static void FLEXCAN_Reset(CAN_Type *base)
{
    /* The module must should be first exit from low power
     * mode, and then soft reset can be applied.
     */
    assert(0U == (base->MCR & CAN_MCR_MDIS_MASK));

    uint8_t i;

#if (defined(FSL_FEATURE_FLEXCAN_HAS_DOZE_MODE_SUPPORT) && FSL_FEATURE_FLEXCAN_HAS_DOZE_MODE_SUPPORT)
    if (0 != (FSL_FEATURE_FLEXCAN_INSTANCE_HAS_DOZE_MODE_SUPPORTn(base)))
    {
        /* De-assert DOZE Enable Bit. */
        base->MCR &= ~CAN_MCR_DOZE_MASK;
    }
#endif

    /* Wait until FlexCAN exit from any Low Power Mode. */
    while (0U != (base->MCR & CAN_MCR_LPMACK_MASK))
    {
    }

    /* Assert Soft Reset Signal. */
    base->MCR |= CAN_MCR_SOFTRST_MASK;
    /* Wait until FlexCAN reset completes. */
    while (0U != (base->MCR & CAN_MCR_SOFTRST_MASK))
    {
    }

/* Reset MCR register. */
#if (defined(FSL_FEATURE_FLEXCAN_HAS_GLITCH_FILTER) && FSL_FEATURE_FLEXCAN_HAS_GLITCH_FILTER)
    base->MCR |= CAN_MCR_WRNEN_MASK | CAN_MCR_WAKSRC_MASK |
                 CAN_MCR_MAXMB(FSL_FEATURE_FLEXCAN_HAS_MESSAGE_BUFFER_MAX_NUMBERn(base) - 1);
#else
    base->MCR |= CAN_MCR_WRNEN_MASK | CAN_MCR_MAXMB(FSL_FEATURE_FLEXCAN_HAS_MESSAGE_BUFFER_MAX_NUMBERn(base) - 1);
#endif

/* Reset CTRL1 and CTRL2 register. */
#if (defined(FSL_FEATURE_FLEXCAN_HAS_FLEXIBLE_DATA_RATE) && FSL_FEATURE_FLEXCAN_HAS_FLEXIBLE_DATA_RATE)
    /* SMP bit cannot be asserted when CAN FD is enabled */
    if (0 != FSL_FEATURE_FLEXCAN_INSTANCE_HAS_FLEXIBLE_DATA_RATEn(base))
    {
        base->CTRL1 = 0x0;
    }
    else
    {
        base->CTRL1 = CAN_CTRL1_SMP_MASK;
    }
#else
    base->CTRL1 = CAN_CTRL1_SMP_MASK;
#endif
    base->CTRL2 = CAN_CTRL2_TASD(0x16) | CAN_CTRL2_RRS_MASK | CAN_CTRL2_EACEN_MASK;

    /* Clean all individual Rx Mask of Message Buffers. */
    for (i = 0; i < (uint32_t)FSL_FEATURE_FLEXCAN_HAS_MESSAGE_BUFFER_MAX_NUMBERn(base); i++)
    {
        base->RXIMR[i] = 0x3FFFFFFF;
    }

    /* Clean Global Mask of Message Buffers. */
    base->RXMGMASK = 0x3FFFFFFF;
    /* Clean Global Mask of Message Buffer 14. */
    base->RX14MASK = 0x3FFFFFFF;
    /* Clean Global Mask of Message Buffer 15. */
    base->RX15MASK = 0x3FFFFFFF;
    /* Clean Global Mask of Rx FIFO. */
    base->RXFGMASK = 0x3FFFFFFF;

    /* Clean all Message Buffer CS fields. */
    for (i = 0; i < (uint32_t)FSL_FEATURE_FLEXCAN_HAS_MESSAGE_BUFFER_MAX_NUMBERn(base); i++)
    {
        base->MB[i].CS = 0x0;
    }
}

static void FLEXCAN_SetBaudRate(CAN_Type *base,
                                uint32_t sourceClock_Hz,
                                uint32_t baudRate_Bps,
                                flexcan_timing_config_t timingConfig)
{
    /* FlexCAN timing setting formula:
     * quantum = 1 + (PSEG1 + 1) + (PSEG2 + 1) + (PROPSEG + 1);
     */
    uint32_t quantum = (1U + ((uint32_t)timingConfig.phaseSeg1 + 1U) + ((uint32_t)timingConfig.phaseSeg2 + 1U) +
                        ((uint32_t)timingConfig.propSeg + 1U));
    uint32_t priDiv  = baudRate_Bps * quantum;

    /* Assertion: Desired baud rate is too high. */
    assert(baudRate_Bps <= 1000000U);
    /* Assertion: Source clock should greater than baud rate * quantum. */
    assert(priDiv <= sourceClock_Hz);

    if (0U == priDiv)
    {
        priDiv = 1;
    }

    priDiv = (sourceClock_Hz / priDiv) - 1U;

    /* Desired baud rate is too low. */
    if (priDiv > 0xFFU)
    {
        priDiv = 0xFF;
    }

    timingConfig.preDivider = (uint16_t)priDiv;

    /* Update actual timing characteristic. */
    FLEXCAN_SetTimingConfig(base, (const flexcan_timing_config_t *)(uint32_t)&timingConfig);
}

#if (defined(FSL_FEATURE_FLEXCAN_HAS_FLEXIBLE_DATA_RATE) && FSL_FEATURE_FLEXCAN_HAS_FLEXIBLE_DATA_RATE)
static void FLEXCAN_SetFDBaudRate(CAN_Type *base,
                                  uint32_t sourceClock_Hz,
                                  uint32_t baudRateFD_Bps,
                                  flexcan_timing_config_t timingConfig)
{
    /* FlexCAN FD timing setting formula:
     * quantum = 1 + (FPSEG1 + 1) + (FPSEG2 + 1) + FPROPSEG;
     */
    uint32_t quantum = (1U + ((uint32_t)timingConfig.fphaseSeg1 + 1U) + ((uint32_t)timingConfig.fphaseSeg2 + 1U) +
                        (uint32_t)timingConfig.fpropSeg);
    uint32_t priDiv  = baudRateFD_Bps * quantum;

    /* Assertion: Desired baud rate is too high. */
    assert(baudRateFD_Bps <= 8000000U);
    /* Assertion: Source clock should greater than baud rate * FLEXCAN_TIME_QUANTA_NUM. */
    assert(priDiv <= sourceClock_Hz);

    if (0U == priDiv)
    {
        priDiv = 1;
    }

    priDiv = (sourceClock_Hz / priDiv) - 1U;

    /* Desired baud rate is too low. */
    if (priDiv > 0xFFU)
    {
        priDiv = 0xFF;
    }

    timingConfig.fpreDivider = (uint16_t)priDiv;

    /* Update actual timing characteristic. */
    FLEXCAN_SetFDTimingConfig(base, (const flexcan_timing_config_t *)(uint32_t)&timingConfig);
}
#endif

/*!
 * brief Initializes a FlexCAN instance.
 *
 * This function initializes the FlexCAN module with user-defined settings.
 * This example shows how to set up the flexcan_config_t parameters and how
 * to call the FLEXCAN_Init function by passing in these parameters.
 *  code
 *   flexcan_config_t flexcanConfig;
 *   flexcanConfig.clkSrc            = kFLEXCAN_ClkSrcOsc;
 *   flexcanConfig.baudRate          = 1000000U;
 *   flexcanConfig.maxMbNum          = 16;
 *   flexcanConfig.enableLoopBack    = false;
 *   flexcanConfig.enableSelfWakeup  = false;
 *   flexcanConfig.enableIndividMask = false;
 *   flexcanConfig.enableDoze        = false;
 *   flexcanConfig.timingConfig      = timingConfig;
 *   FLEXCAN_Init(CAN0, &flexcanConfig, 8000000UL);
 *   endcode
 *
 * param base FlexCAN peripheral base address.
 * param config Pointer to the user-defined configuration structure.
 * param sourceClock_Hz FlexCAN Protocol Engine clock source frequency in Hz.
 */
void FLEXCAN_Init(CAN_Type *base, const flexcan_config_t *config, uint32_t sourceClock_Hz)
{
    uint32_t mcrTemp;
#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    uint32_t instance;
#endif

    /* Assertion. */
    assert(NULL != config);
    assert((config->maxMbNum > 0U) &&
           (config->maxMbNum <= (uint8_t)FSL_FEATURE_FLEXCAN_HAS_MESSAGE_BUFFER_MAX_NUMBERn(base)));

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    instance = FLEXCAN_GetInstance(base);
    /* Enable FlexCAN clock. */
    CLOCK_EnableClock(s_flexcanClock[instance]);
#if defined(FLEXCAN_PERIPH_CLOCKS)
    /* Enable FlexCAN serial clock. */
    CLOCK_EnableClock(s_flexcanPeriphClock[instance]);
#endif /* FLEXCAN_PERIPH_CLOCKS */
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

#if (!defined(FSL_FEATURE_FLEXCAN_SUPPORT_ENGINE_CLK_SEL_REMOVE)) || !FSL_FEATURE_FLEXCAN_SUPPORT_ENGINE_CLK_SEL_REMOVE
    /* Disable FlexCAN Module. */
    FLEXCAN_Enable(base, false);

    /* Protocol-Engine clock source selection, This bit must be set
     * when FlexCAN Module in Disable Mode.
     */
    base->CTRL1 = (kFLEXCAN_ClkSrcOsc == config->clkSrc) ? (base->CTRL1 & ~CAN_CTRL1_CLKSRC_MASK) :
                                                           (base->CTRL1 | CAN_CTRL1_CLKSRC_MASK);
#else
#if defined(CAN_CTRL1_CLKSRC_MASK)
    if (!FSL_FEATURE_FLEXCAN_INSTANCE_SUPPORT_ENGINE_CLK_SEL_REMOVEn(base))
    {
        /* Disable FlexCAN Module. */
        FLEXCAN_Enable(base, false);

        /* Protocol-Engine clock source selection, This bit must be set
         * when FlexCAN Module in Disable Mode.
         */
        base->CTRL1 = (kFLEXCAN_ClkSrcOsc == config->clkSrc) ? (base->CTRL1 & ~CAN_CTRL1_CLKSRC_MASK) :
                                                               (base->CTRL1 | CAN_CTRL1_CLKSRC_MASK);
    }
#endif
#endif /* FSL_FEATURE_FLEXCAN_SUPPORT_ENGINE_CLK_SEL_REMOVE */

    /* Enable FlexCAN Module for configuration. */
    FLEXCAN_Enable(base, true);

    /* Reset to known status. */
    FLEXCAN_Reset(base);

    /* Save current MCR value and enable to enter Freeze mode(enabled by default). */
    mcrTemp = base->MCR;

    /* Set the maximum number of Message Buffers */
    mcrTemp = (mcrTemp & ~CAN_MCR_MAXMB_MASK) | CAN_MCR_MAXMB((uint32_t)config->maxMbNum - 1U);

    /* Enable Loop Back Mode? */
    base->CTRL1 =
        (true == config->enableLoopBack) ? (base->CTRL1 | CAN_CTRL1_LPB_MASK) : (base->CTRL1 & ~CAN_CTRL1_LPB_MASK);

    /* Enable Timer Sync? */
    base->CTRL1 =
        (true == config->enableTimerSync) ? (base->CTRL1 | CAN_CTRL1_TSYN_MASK) : (base->CTRL1 & ~CAN_CTRL1_TSYN_MASK);

    /* Enable Self Wake Up Mode and configure the wake up source. */
    mcrTemp = (true == config->enableSelfWakeup) ? (mcrTemp | CAN_MCR_SLFWAK_MASK) : (mcrTemp & ~CAN_MCR_SLFWAK_MASK);
    mcrTemp = (kFLEXCAN_WakeupSrcFiltered == config->wakeupSrc) ? (mcrTemp | CAN_MCR_WAKSRC_MASK) :
                                                                  (mcrTemp & ~CAN_MCR_WAKSRC_MASK);

    /* Enable Individual Rx Masking? */
    mcrTemp = (true == config->enableIndividMask) ? (mcrTemp | CAN_MCR_IRMQ_MASK) : (mcrTemp & ~CAN_MCR_IRMQ_MASK);

#if (defined(FSL_FEATURE_FLEXCAN_HAS_DOZE_MODE_SUPPORT) && FSL_FEATURE_FLEXCAN_HAS_DOZE_MODE_SUPPORT)
    if (0 != FSL_FEATURE_FLEXCAN_INSTANCE_HAS_DOZE_MODE_SUPPORTn(base))
    {
        /* Enable Doze Mode? */
        mcrTemp = (true == config->enableDoze) ? (mcrTemp | CAN_MCR_DOZE_MASK) : (mcrTemp & ~CAN_MCR_DOZE_MASK);
    }
#endif

    /* Save MCR Configuration. */
    base->MCR = mcrTemp;

    /* Baud Rate Configuration.*/
    FLEXCAN_SetBaudRate(base, sourceClock_Hz, config->baudRate, config->timingConfig);
}

#if (defined(FSL_FEATURE_FLEXCAN_HAS_FLEXIBLE_DATA_RATE) && FSL_FEATURE_FLEXCAN_HAS_FLEXIBLE_DATA_RATE)
/*!
 * brief Initializes a FlexCAN instance.
 *
 * This function initializes the FlexCAN module with user-defined settings.
 * This example shows how to set up the flexcan_config_t parameters and how
 * to call the FLEXCAN_FDInit function by passing in these parameters.
 *  code
 *   flexcan_config_t flexcanConfig;
 *   flexcanConfig.clkSrc            = kFLEXCAN_ClkSrcOsc;
 *   flexcanConfig.baudRate          = 1000000U;
 *   flexcanConfig.baudRateFD        = 2000000U;
 *   flexcanConfig.maxMbNum          = 16;
 *   flexcanConfig.enableLoopBack    = false;
 *   flexcanConfig.enableSelfWakeup  = false;
 *   flexcanConfig.enableIndividMask = false;
 *   flexcanConfig.enableDoze        = false;
 *   flexcanConfig.timingConfig      = timingConfig;
 *   FLEXCAN_FDInit(CAN0, &flexcanConfig, 8000000UL, kFLEXCAN_16BperMB, false);
 *   endcode
 *
 * param base FlexCAN peripheral base address.
 * param config Pointer to the user-defined configuration structure.
 * param sourceClock_Hz FlexCAN Protocol Engine clock source frequency in Hz.
 * param dataSize FlexCAN FD frame payload size.
 * param brs If bitrate switch is enabled in FD mode.
 */
void FLEXCAN_FDInit(
    CAN_Type *base, const flexcan_config_t *config, uint32_t sourceClock_Hz, flexcan_mb_size_t dataSize, bool brs)
{
    uint32_t fdctrl = 0U;
    assert((uint32_t)dataSize <= 3U);

    /* Initialization of classical CAN. */
    FLEXCAN_Init(base, config, sourceClock_Hz);

    /* Extra bitrate setting for CANFD. */
    FLEXCAN_SetFDBaudRate(base, sourceClock_Hz, config->baudRateFD, config->timingConfig);

    /* read FDCTRL register. */
    fdctrl = base->FDCTRL;

    /* Enable FD operation and set bitrate switch. */
    if (brs)
    {
        fdctrl &= CAN_FDCTRL_FDRATE_MASK;
    }
    else
    {
        fdctrl &= ~CAN_FDCTRL_FDRATE_MASK;
    }

    if ((true == brs) && (false == config->enableLoopBack))
    {
        /* Before use "|=" operation for multi-bits field, CPU should Clean previous Setting. */
        fdctrl = (fdctrl & ~CAN_FDCTRL_TDCOFF_MASK) | CAN_FDCTRL_TDCOFF(0x2U);
    }

    /* Before use "|=" operation for multi-bits field, CPU should clean previous Setting. */
    fdctrl = (fdctrl & ~CAN_FDCTRL_MBDSR0_MASK) | CAN_FDCTRL_MBDSR0(dataSize);
#if defined(CAN_FDCTRL_MBDSR1_MASK)
    fdctrl = (fdctrl & ~CAN_FDCTRL_MBDSR1_MASK) | CAN_FDCTRL_MBDSR1(dataSize);
#endif
#if defined(CAN_FDCTRL_MBDSR2_MASK)
    fdctrl = (fdctrl & ~CAN_FDCTRL_MBDSR2_MASK) | CAN_FDCTRL_MBDSR2(dataSize);
#endif
#if defined(CAN_FDCTRL_MBDSR3_MASK)
    fdctrl = (fdctrl & ~CAN_FDCTRL_MBDSR3_MASK) | CAN_FDCTRL_MBDSR3(dataSize);
#endif

    /* Enter Freeze Mode. */
    FLEXCAN_EnterFreezeMode(base);

    base->MCR |= CAN_MCR_FDEN_MASK;

    /* update the FDCTL register. */
    base->FDCTRL = fdctrl;

    /* Exit Freeze Mode. */
    FLEXCAN_ExitFreezeMode(base);
}
#endif

/*!
 * brief De-initializes a FlexCAN instance.
 *
 * This function disables the FlexCAN module clock and sets all register values
 * to the reset value.
 *
 * param base FlexCAN peripheral base address.
 */
void FLEXCAN_Deinit(CAN_Type *base)
{
#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    uint32_t instance;
#endif
    /* Reset all Register Contents. */
    FLEXCAN_Reset(base);

    /* Disable FlexCAN module. */
    FLEXCAN_Enable(base, false);

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    instance = FLEXCAN_GetInstance(base);
#if defined(FLEXCAN_PERIPH_CLOCKS)
    /* Disable FlexCAN serial clock. */
    CLOCK_DisableClock(s_flexcanPeriphClock[instance]);
#endif /* FLEXCAN_PERIPH_CLOCKS */
    /* Disable FlexCAN clock. */
    CLOCK_DisableClock(s_flexcanClock[instance]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */
}

/*!
 * brief Gets the default configuration structure.
 *
 * This function initializes the FlexCAN configuration structure to default values. The default
 * values are as follows.
 *   flexcanConfig->clkSrc            = kFLEXCAN_ClkSrcOsc;
 *   flexcanConfig->baudRate          = 1000000U;
 *   flexcanConfig->baudRateFD        = 2000000U;
 *   flexcanConfig->maxMbNum          = 16;
 *   flexcanConfig->enableLoopBack    = false;
 *   flexcanConfig->enableSelfWakeup  = false;
 *   flexcanConfig->enableIndividMask = false;
 *   flexcanConfig->enableDoze        = false;
 *   flexcanConfig.timingConfig       = timingConfig;
 *
 * param config Pointer to the FlexCAN configuration structure.
 */
void FLEXCAN_GetDefaultConfig(flexcan_config_t *config)
{
    /* Assertion. */
    assert(NULL != config);

    /* Initializes the configure structure to zero. */
    (void)memset(config, 0, sizeof(*config));

    /* Initialize FlexCAN Module config struct with default value. */
    config->clkSrc   = kFLEXCAN_ClkSrcOsc;
    config->baudRate = 1000000U;
#if (defined(FSL_FEATURE_FLEXCAN_HAS_FLEXIBLE_DATA_RATE) && FSL_FEATURE_FLEXCAN_HAS_FLEXIBLE_DATA_RATE)
    config->baudRateFD = 2000000U;
#endif
    config->maxMbNum          = 16;
    config->enableLoopBack    = false;
    config->enableTimerSync   = true;
    config->enableSelfWakeup  = false;
    config->wakeupSrc         = kFLEXCAN_WakeupSrcUnfiltered;
    config->enableIndividMask = false;
#if (defined(FSL_FEATURE_FLEXCAN_HAS_DOZE_MODE_SUPPORT) && FSL_FEATURE_FLEXCAN_HAS_DOZE_MODE_SUPPORT)
    config->enableDoze = false;
#endif
    /* Default protocol timing configuration, time quantum is 10. */
    config->timingConfig.phaseSeg1  = 3;
    config->timingConfig.phaseSeg2  = 2;
    config->timingConfig.propSeg    = 1;
    config->timingConfig.rJumpwidth = 1;
#if (defined(FSL_FEATURE_FLEXCAN_HAS_FLEXIBLE_DATA_RATE) && FSL_FEATURE_FLEXCAN_HAS_FLEXIBLE_DATA_RATE)
    config->timingConfig.fphaseSeg1  = 3;
    config->timingConfig.fphaseSeg2  = 3;
    config->timingConfig.fpropSeg    = 1;
    config->timingConfig.frJumpwidth = 1;
#endif
}

/*!
 * brief Sets the FlexCAN protocol timing characteristic.
 *
 * This function gives user settings to CAN bus timing characteristic.
 * The function is for an experienced user. For less experienced users, call
 * the FLEXCAN_Init() and fill the baud rate field with a desired value.
 * This provides the default timing characteristics to the module.
 *
 * Note that calling FLEXCAN_SetTimingConfig() overrides the baud rate set
 * in FLEXCAN_Init().
 *
 * param base FlexCAN peripheral base address.
 * param config Pointer to the timing configuration structure.
 */
void FLEXCAN_SetTimingConfig(CAN_Type *base, const flexcan_timing_config_t *config)
{
    /* Assertion. */
    assert(NULL != config);

    /* Enter Freeze Mode. */
    FLEXCAN_EnterFreezeMode(base);

#if (defined(FSL_FEATURE_FLEXCAN_HAS_FLEXIBLE_DATA_RATE) && FSL_FEATURE_FLEXCAN_HAS_FLEXIBLE_DATA_RATE)
    if (0 != FSL_FEATURE_FLEXCAN_INSTANCE_HAS_FLEXIBLE_DATA_RATEn(base))
    {
        /* Cleaning previous Timing Setting. */
        base->CBT &= ~(CAN_CBT_EPRESDIV_MASK | CAN_CBT_ERJW_MASK | CAN_CBT_EPSEG1_MASK | CAN_CBT_EPSEG2_MASK |
                       CAN_CBT_EPROPSEG_MASK);

        /* Updating Timing Setting according to configuration structure. */
        base->CBT |=
            (CAN_CBT_EPRESDIV(config->preDivider) | CAN_CBT_ERJW(config->rJumpwidth) |
             CAN_CBT_EPSEG1(config->phaseSeg1) | CAN_CBT_EPSEG2(config->phaseSeg2) | CAN_CBT_EPROPSEG(config->propSeg));
    }
    else
    {
        /* Cleaning previous Timing Setting. */
        base->CTRL1 &= ~(CAN_CTRL1_PRESDIV_MASK | CAN_CTRL1_RJW_MASK | CAN_CTRL1_PSEG1_MASK | CAN_CTRL1_PSEG2_MASK |
                         CAN_CTRL1_PROPSEG_MASK);

        /* Updating Timing Setting according to configuration structure. */
        base->CTRL1 |= (CAN_CTRL1_PRESDIV(config->preDivider) | CAN_CTRL1_RJW(config->rJumpwidth) |
                        CAN_CTRL1_PSEG1(config->phaseSeg1) | CAN_CTRL1_PSEG2(config->phaseSeg2) |
                        CAN_CTRL1_PROPSEG(config->propSeg));
    }
#else
    /* Cleaning previous Timing Setting. */
    base->CTRL1 &= ~(CAN_CTRL1_PRESDIV_MASK | CAN_CTRL1_RJW_MASK | CAN_CTRL1_PSEG1_MASK | CAN_CTRL1_PSEG2_MASK |
                     CAN_CTRL1_PROPSEG_MASK);

    /* Updating Timing Setting according to configuration structure. */
    base->CTRL1 |=
        (CAN_CTRL1_PRESDIV(config->preDivider) | CAN_CTRL1_RJW(config->rJumpwidth) |
         CAN_CTRL1_PSEG1(config->phaseSeg1) | CAN_CTRL1_PSEG2(config->phaseSeg2) | CAN_CTRL1_PROPSEG(config->propSeg));
#endif

    /* Exit Freeze Mode. */
    FLEXCAN_ExitFreezeMode(base);
}

#if (defined(FSL_FEATURE_FLEXCAN_HAS_FLEXIBLE_DATA_RATE) && FSL_FEATURE_FLEXCAN_HAS_FLEXIBLE_DATA_RATE)
/*!
 * brief Sets the FlexCAN FD protocol timing characteristic.
 *
 * This function gives user settings to CAN bus timing characteristic.
 * The function is for an experienced user. For less experienced users, call
 * the FLEXCAN_Init() and fill the baud rate field with a desired value.
 * This provides the default timing characteristics to the module.
 *
 * Note that calling FLEXCAN_SetFDTimingConfig() overrides the baud rate set
 * in FLEXCAN_Init().
 *
 * param base FlexCAN peripheral base address.
 * param config Pointer to the timing configuration structure.
 */
void FLEXCAN_SetFDTimingConfig(CAN_Type *base, const flexcan_timing_config_t *config)
{
    /* Assertion. */
    assert(NULL != config);

    /* Enter Freeze Mode. */
    FLEXCAN_EnterFreezeMode(base);

    base->CBT |= CAN_CBT_BTF(1);
    /* Cleaning previous Timing Setting. */
    base->FDCBT &= ~(CAN_FDCBT_FPRESDIV_MASK | CAN_FDCBT_FRJW_MASK | CAN_FDCBT_FPSEG1_MASK | CAN_FDCBT_FPSEG2_MASK |
                     CAN_FDCBT_FPROPSEG_MASK);

    /* Updating Timing Setting according to configuration structure. */
    base->FDCBT |= (CAN_FDCBT_FPRESDIV(config->fpreDivider) | CAN_FDCBT_FRJW(config->frJumpwidth) |
                    CAN_FDCBT_FPSEG1(config->fphaseSeg1) | CAN_FDCBT_FPSEG2(config->fphaseSeg2) |
                    CAN_FDCBT_FPROPSEG(config->fpropSeg));

    /* Exit Freeze Mode. */
    FLEXCAN_ExitFreezeMode(base);
}
#endif

/*!
 * brief Sets the FlexCAN receive message buffer global mask.
 *
 * This function sets the global mask for the FlexCAN message buffer in a matching process.
 * The configuration is only effective when the Rx individual mask is disabled in the FLEXCAN_Init().
 *
 * param base FlexCAN peripheral base address.
 * param mask Rx Message Buffer Global Mask value.
 */
void FLEXCAN_SetRxMbGlobalMask(CAN_Type *base, uint32_t mask)
{
    /* Enter Freeze Mode. */
    FLEXCAN_EnterFreezeMode(base);

    /* Setting Rx Message Buffer Global Mask value. */
    base->RXMGMASK = mask;
    base->RX14MASK = mask;
    base->RX15MASK = mask;

    /* Exit Freeze Mode. */
    FLEXCAN_ExitFreezeMode(base);
}

/*!
 * brief Sets the FlexCAN receive FIFO global mask.
 *
 * This function sets the global mask for FlexCAN FIFO in a matching process.
 *
 * param base FlexCAN peripheral base address.
 * param mask Rx Fifo Global Mask value.
 */
void FLEXCAN_SetRxFifoGlobalMask(CAN_Type *base, uint32_t mask)
{
    /* Enter Freeze Mode. */
    FLEXCAN_EnterFreezeMode(base);

    /* Setting Rx FIFO Global Mask value. */
    base->RXFGMASK = mask;

    /* Exit Freeze Mode. */
    FLEXCAN_ExitFreezeMode(base);
}

/*!
 * brief Sets the FlexCAN receive individual mask.
 *
 * This function sets the individual mask for the FlexCAN matching process.
 * The configuration is only effective when the Rx individual mask is enabled in the FLEXCAN_Init().
 * If the Rx FIFO is disabled, the individual mask is applied to the corresponding Message Buffer.
 * If the Rx FIFO is enabled, the individual mask for Rx FIFO occupied Message Buffer is applied to
 * the Rx Filter with the same index. Note that only the first 32
 * individual masks can be used as the Rx FIFO filter mask.
 *
 * param base FlexCAN peripheral base address.
 * param maskIdx The Index of individual Mask.
 * param mask Rx Individual Mask value.
 */
void FLEXCAN_SetRxIndividualMask(CAN_Type *base, uint8_t maskIdx, uint32_t mask)
{
    assert(maskIdx <= (base->MCR & CAN_MCR_MAXMB_MASK));

    /* Enter Freeze Mode. */
    FLEXCAN_EnterFreezeMode(base);

    /* Setting Rx Individual Mask value. */
    base->RXIMR[maskIdx] = mask;

    /* Exit Freeze Mode. */
    FLEXCAN_ExitFreezeMode(base);
}

/*!
 * brief Configures a FlexCAN transmit message buffer.
 *
 * This function aborts the previous transmission, cleans the Message Buffer, and
 * configures it as a Transmit Message Buffer.
 *
 * param base FlexCAN peripheral base address.
 * param mbIdx The Message Buffer index.
 * param enable Enable/disable Tx Message Buffer.
 *               - true: Enable Tx Message Buffer.
 *               - false: Disable Tx Message Buffer.
 */
void FLEXCAN_SetTxMbConfig(CAN_Type *base, uint8_t mbIdx, bool enable)
{
    /* Assertion. */
    assert(mbIdx <= (base->MCR & CAN_MCR_MAXMB_MASK));
    assert(!FLEXCAN_IsMbOccupied(base, mbIdx));

    /* Inactivate Message Buffer. */
    if (enable)
    {
        base->MB[mbIdx].CS = CAN_CS_CODE(kFLEXCAN_TxMbInactive);
    }
    else
    {
        base->MB[mbIdx].CS = 0;
    }

    /* Clean Message Buffer content. */
    base->MB[mbIdx].ID    = 0x0;
    base->MB[mbIdx].WORD0 = 0x0;
    base->MB[mbIdx].WORD1 = 0x0;
}

/*!
 * @brief Calculates the segment values for a single bit time for classical CAN
 *
 * @param baudRate The data speed in bps
 * @param tqNum Number of time quantas per bit
 * @param pconfig Pointer to the FlexCAN timing configuration structure.
 *
 */
static void FLEXCAN_GetSegments(uint32_t baudRate, uint32_t tqNum, flexcan_timing_config_t *pconfig)
{
    uint32_t ideal_sp;
    uint32_t p1;

    /* get ideal sample point. */
    if (baudRate >= 1000000)
        ideal_sp = IDEAL_SP_LOW;
    else if (baudRate >= 800000)
        ideal_sp = IDEAL_SP_MID;
    else
        ideal_sp = IDEAL_SP_HIGH;

    /* distribute time quanta. */
    p1                 = tqNum * (uint32_t)ideal_sp;
    pconfig->propSeg   = 0;
    pconfig->phaseSeg1 = p1 / IDEAL_SP_FACTOR - 2U;
    if (pconfig->phaseSeg1 > (MAX_PSEG1 + MAX_PROPSEG))
    {
        pconfig->phaseSeg1 = MAX_PSEG1;
        pconfig->propSeg   = MAX_PROPSEG;
    }
    else if (pconfig->phaseSeg1 > MAX_PSEG1)
    {
        pconfig->propSeg   = pconfig->phaseSeg1 - MAX_PSEG1;
        pconfig->phaseSeg1 = MAX_PSEG1;
    }
    pconfig->phaseSeg2 = tqNum - (1U + pconfig->phaseSeg1 + 1U + pconfig->propSeg + 1U + 1U);
    assert(pconfig->phaseSeg2 <= MAX_PSEG2);

    /* subtract one TQ for sync seg. */
    /* sjw is 20% of total TQ, rounded to nearest int. */
    pconfig->rJumpwidth = (tqNum + (5 - 1)) / 5 - 1U;
    if (pconfig->rJumpwidth > MAX_RJW)
    {
        pconfig->rJumpwidth = MAX_RJW;
    }
}

/*!
 * @brief Calculates the improved timing values by specific baudrates for classical CAN
 *
 * @param baudRate  The classical CAN speed in bps defined by user
 * @param sourceClock_Hz The Source clock data speed in bps. Zero to disable baudrate switching
 * @param pconfig Pointer to the FlexCAN timing configuration structure.
 *
 * @return TRUE if timing configuration found, FALSE if failed to find configuration
 */
bool FLEXCAN_CalculateImprovedTimingValues(uint32_t baudRate, uint32_t sourceClock_Hz, flexcan_timing_config_t *pconfig)
{
    uint32_t clk;   /* the clock is tqNumb x baudRateFD. */
    uint32_t clk2;  /* the clock2 is clk2 / Pre-scaler Division Factor. */
    uint32_t tqNum; /* Numbers of TQ. */

    /* observe baud rate maximums. */
    assert(baudRate <= MAX_CAN_BAUDRATE);

    /*  Auto Improved Protocal timing for CBT. */
    for (tqNum = CTRL1_MAX_TIME_QUANTA; tqNum >= CTRL1_MIN_TIME_QUANTA; tqNum--)
    {
        clk = baudRate * tqNum;
        if (clk > sourceClock_Hz)
        {
            continue; /* tqNum too large, clk has been exceed sourceClock_Hz. */
        }

        for (pconfig->preDivider = 0x00; pconfig->preDivider <= MAX_PRESDIV; (pconfig->preDivider)++)
        {
            /* Consider some proessor not contain FPU, the parameter need to be exact division. */
            if ((clk / (pconfig->preDivider + 1U) * (pconfig->preDivider + 1U)) != clk)
            {
                continue; /* clk need to be exact division by preDivider + 1. */
            }
            clk2 = clk / (pconfig->preDivider + 1U);
            if (((sourceClock_Hz / clk2) * clk2) != sourceClock_Hz)
            {
                continue; /* sourceClock_Hz need to be exact division by preDivider. */
            }
            FLEXCAN_GetSegments(baudRate, tqNum, pconfig);
            return true;
        }
    }
    /* failed to find solution. */
    return false;
}

#if (defined(FSL_FEATURE_FLEXCAN_HAS_FLEXIBLE_DATA_RATE) && FSL_FEATURE_FLEXCAN_HAS_FLEXIBLE_DATA_RATE)
static uint32_t FLEXCAN_GetFDMailboxOffset(CAN_Type *base, uint8_t mbIdx)
{
    uint32_t dataSize;
    uint32_t offset = 0;
    dataSize        = (base->FDCTRL & CAN_FDCTRL_MBDSR0_MASK) >> CAN_FDCTRL_MBDSR0_SHIFT;
    switch (dataSize)
    {
        case (uint32_t)kFLEXCAN_8BperMB:
            offset = (((uint32_t)mbIdx / 32U) * 512U + ((uint32_t)mbIdx % 32U) * 16U);
            break;
        case (uint32_t)kFLEXCAN_16BperMB:
            offset = (((uint32_t)mbIdx / 21U) * 512U + ((uint32_t)mbIdx % 21U) * 24U);
            break;
        case (uint32_t)kFLEXCAN_32BperMB:
            offset = (((uint32_t)mbIdx / 12U) * 512U + ((uint32_t)mbIdx % 12U) * 40U);
            break;
        case (uint32_t)kFLEXCAN_64BperMB:
            offset = (((uint32_t)mbIdx / 7U) * 512U + ((uint32_t)mbIdx % 7U) * 72U);
            break;
        default:
            /* All the cases have been listed above, the default clause should not be reached. */
            assert(false);
            break;
    }
    /* To get the dword aligned offset, need to divide by 4. */
    offset = offset / 4U;
    return offset;
}

/*!
 * @brief Calculates the segment values for a single bit time for CANFD bus control baud Rate
 *
 * @param baudRate The canfd bus control speed in bps
 * @param tqNum Number of time quanta per bit
 * @param pconfig Pointer to the FlexCAN timing configuration structure.
 *
 */
static void FLEXCAN_FDGetSegments(uint32_t baudRate, uint32_t tqNum, flexcan_timing_config_t *pconfig)
{
    uint32_t ideal_sp;
    uint32_t p1;

    /* get ideal sample point. */
    if (baudRate >= 1000000)
        ideal_sp = IDEAL_SP_LOW;
    else if (baudRate >= 800000)
        ideal_sp = IDEAL_SP_MID;
    else
        ideal_sp = IDEAL_SP_HIGH;

    /* distribute time quanta. */
    p1                 = tqNum * (uint32_t)ideal_sp;
    pconfig->propSeg   = 0;
    pconfig->phaseSeg1 = p1 / IDEAL_SP_FACTOR - 2U;
    if (pconfig->phaseSeg1 > (MAX_EPSEG1 + MAX_EPROPSEG))
    {
        pconfig->phaseSeg1 = MAX_EPSEG1;
        pconfig->propSeg   = MAX_EPROPSEG;
    }
    else if (pconfig->phaseSeg1 > MAX_EPSEG1)
    {
        pconfig->propSeg   = pconfig->phaseSeg1 - MAX_EPSEG1;
        pconfig->phaseSeg1 = MAX_EPSEG1;
    }

    pconfig->phaseSeg2 = tqNum - (1U + pconfig->phaseSeg1 + 1U + pconfig->propSeg + 1U + 1U);
    assert(pconfig->phaseSeg2 <= MAX_EPSEG2);

    /* subtract one TQ for sync seg. */
    /* sjw is 20% of total TQ, rounded to nearest int. */
    pconfig->rJumpwidth = (tqNum + (5 - 1)) / 5 - 1U;
    /* the max tqNum for CBT will reach to 129, ERJW would not larger than 26. */
    /* Consider and max ERJW is 31, so rJumpwidth will always smaller than MAX_ERJW. */
    assert(pconfig->rJumpwidth <= MAX_ERJW);
}

/*!
 * @brief Calculates the segment values for a single bit time for CANFD bus data baud Rate
 *
 * @param baudRate The canfd bus data speed in bps
 * @param tqNum Number of time quanta per bit
 * @param pconfig Pointer to the FlexCAN timing configuration structure.
 *
 */
static void FLEXCAN_FDGetSegmentswithBRS(uint32_t baudRateFD, uint32_t tqNum, flexcan_timing_config_t *pconfig)
{
    uint32_t ideal_sp;
    uint32_t p1;

    /* get ideal sample point. */
    if (baudRateFD >= 1000000)
        ideal_sp = IDEAL_SP_LOW;
    else if (baudRateFD >= 800000)
        ideal_sp = IDEAL_SP_MID;
    else
        ideal_sp = IDEAL_SP_HIGH;

    /* distribute time quanta. */
    p1                  = tqNum * (uint32_t)ideal_sp;
    pconfig->fpropSeg   = 0;
    pconfig->fphaseSeg1 = p1 / IDEAL_SP_FACTOR - 1U;
    if (pconfig->fphaseSeg1 > (MAX_FPSEG1 + MAX_FPROPSEG))
    {
        pconfig->fphaseSeg1 = MAX_FPSEG1;
        pconfig->fpropSeg   = MAX_FPROPSEG;
    }
    else if (pconfig->fphaseSeg1 > MAX_FPSEG1)
    {
        pconfig->fpropSeg   = pconfig->fphaseSeg1 - MAX_FPSEG1;
        pconfig->fphaseSeg1 = MAX_FPSEG1;
    }

    pconfig->fphaseSeg2 = tqNum - (1U + pconfig->fphaseSeg1 + 1U + pconfig->fpropSeg + 0U + 1U);

    assert(pconfig->fphaseSeg2 <= MAX_FPSEG2);

    /* subtract one TQ for sync seg. */
    /* sjw is 20% of total TQ, rounded to nearest int. */
    pconfig->frJumpwidth = (tqNum + (5 - 1)) / 5 - 1U;

    if (pconfig->frJumpwidth > MAX_FRJW)
    {
        pconfig->frJumpwidth = MAX_FRJW;
    }
}

/*!
 * @brief Calculates the improved timing values by specific baudrates for CANFD
 *
 * @param baudRate  The CANFD bus control speed in bps defined by user
 * @param baudRateFD  The CANFD bus data speed in bps defined by user
 * @param sourceClock_Hz The Source clock data speed in bps. Zero to disable baudrate switching
 * @param pconfig Pointer to the FlexCAN timing configuration structure.
 *
 * @return TRUE if timing configuration found, FALSE if failed to find configuration
 */
bool FLEXCAN_FDCalculateImprovedTimingValues(uint32_t baudRate,
                                             uint32_t baudRateFD,
                                             uint32_t sourceClock_Hz,
                                             flexcan_timing_config_t *pconfig)
{
    uint32_t clk, clkbrs;
    uint32_t clk2, clkbrs2;
    uint32_t tqNum, tqNumbrs; /* Numbers of TQ. */

    /* observe baud rate maximums */
    assert(baudRate <= MAX_CAN_BAUDRATE);
    assert(baudRateFD <= MAX_CANFD_BAUDRATE);

    /*  Auto Improved Protocal timing for CBT. */
    for (tqNum = CBT_MAX_TIME_QUANTA; tqNum >= CBT_MIN_TIME_QUANTA; tqNum--)
    {
        clk = baudRate * tqNum;
        if (clk > sourceClock_Hz)
        {
            continue; /* tqNum too large, clk x tqNum has been exceed sourceClock_Hz. */
        }

        for (pconfig->preDivider = 0x00; pconfig->preDivider <= MAX_EPRESDIV; (pconfig->preDivider)++)
        {
            /* Consider some proessor not contain FPU, the parameter need to be exact division. */
            if ((clk / (pconfig->preDivider + 1U) * (pconfig->preDivider + 1U)) != clk)
            {
                continue; /* clk need to be exact division by preDivider + 1. */
            }
            clk2 = clk / (pconfig->preDivider + 1U);

            if ((sourceClock_Hz / clk2 * clk2) != sourceClock_Hz)
            {
                continue; /* sourceClock_Hz need to be exact division by clk2. */
            }
            FLEXCAN_FDGetSegments(baudRate, tqNum, pconfig);
            /* After calculate for CBT, continue to calculate FBCBT field timing configuration. */
            for (tqNumbrs = FDCBT_MAX_TIME_QUANTA; tqNumbrs >= FDCBT_MIN_TIME_QUANTA; tqNumbrs--)
            {
                clkbrs = baudRateFD * tqNumbrs;
                if (clkbrs > sourceClock_Hz)
                {
                    continue; /* tqNumbrs too large, clkbrs x tqNumbrs has been exceed sourceClock_Hz. */
                }
                for (pconfig->fpreDivider = 0x00; pconfig->fpreDivider <= MAX_FPRESDIV; (pconfig->fpreDivider)++)
                {
                    /* Consider some proessor not contain FPU, the parameter need to be exact division. */
                    if ((clkbrs / (pconfig->fpreDivider + 1U) * (pconfig->fpreDivider + 1U)) != clkbrs)
                    {
                        continue; /* clk need to be exact division by preDivider + 1. */
                    }
                    clkbrs2 = clkbrs / (pconfig->fpreDivider + 1U);
                    if ((sourceClock_Hz / clkbrs2 * clkbrs2) != sourceClock_Hz)
                    {
                        continue; /* sourceClock_Hz need to be exact division by preDivider. */
                    }
                    FLEXCAN_FDGetSegmentswithBRS(baudRateFD, tqNumbrs, pconfig);
                    return true;
                }
            }
        }
    }
    /* failed to find solution. */
    return false;
}
#endif

#if (defined(FSL_FEATURE_FLEXCAN_HAS_FLEXIBLE_DATA_RATE) && FSL_FEATURE_FLEXCAN_HAS_FLEXIBLE_DATA_RATE)
/*!
 * brief Configures a FlexCAN transmit message buffer.
 *
 * This function aborts the previous transmission, cleans the Message Buffer, and
 * configures it as a Transmit Message Buffer.
 *
 * param base FlexCAN peripheral base address.
 * param mbIdx The Message Buffer index.
 * param enable Enable/disable Tx Message Buffer.
 *               - true: Enable Tx Message Buffer.
 *               - false: Disable Tx Message Buffer.
 */
void FLEXCAN_SetFDTxMbConfig(CAN_Type *base, uint8_t mbIdx, bool enable)
{
    /* Assertion. */
    assert(mbIdx <= (base->MCR & CAN_MCR_MAXMB_MASK));
    assert(!FLEXCAN_IsMbOccupied(base, mbIdx));
    uint8_t cnt           = 0;
    uint8_t payload_dword = 1;
    uint32_t dataSize;
    dataSize                  = (base->FDCTRL & CAN_FDCTRL_MBDSR0_MASK) >> CAN_FDCTRL_MBDSR0_SHIFT;
    volatile uint32_t *mbAddr = &(base->MB[0].CS);
    uint32_t offset           = FLEXCAN_GetFDMailboxOffset(base, mbIdx);
#if ((defined(FSL_FEATURE_FLEXCAN_HAS_ERRATA_5641) && FSL_FEATURE_FLEXCAN_HAS_ERRATA_5641) || \
     (defined(FSL_FEATURE_FLEXCAN_HAS_ERRATA_5829) && FSL_FEATURE_FLEXCAN_HAS_ERRATA_5829))
    uint32_t availoffset = FLEXCAN_GetFDMailboxOffset(base, FLEXCAN_GetFirstValidMb(base));
#endif

    /* Inactivate Message Buffer. */
    if (enable)
    {
        /* Inactivate by writing CS. */
        mbAddr[offset] = CAN_CS_CODE(kFLEXCAN_TxMbInactive);
    }
    else
    {
        mbAddr[offset] = 0x0;
    }

    /* Calculate the DWORD number, dataSize 0/1/2/3 corresponds to 8/16/32/64
       Bytes payload. */
    for (cnt = 0; cnt < (dataSize + 1U); cnt++)
    {
        payload_dword *= 2U;
    }

    /* Clean ID. */
    mbAddr[offset + 1U] = 0x0U;
    /* Clean Message Buffer content, DWORD by DWORD. */
    for (cnt = 0; cnt < payload_dword; cnt++)
    {
        mbAddr[offset + 2U + cnt] = 0x0U;
    }

#if ((defined(FSL_FEATURE_FLEXCAN_HAS_ERRATA_5641) && FSL_FEATURE_FLEXCAN_HAS_ERRATA_5641) || \
     (defined(FSL_FEATURE_FLEXCAN_HAS_ERRATA_5829) && FSL_FEATURE_FLEXCAN_HAS_ERRATA_5829))
    mbAddr[availoffset] = CAN_CS_CODE(kFLEXCAN_TxMbInactive);
#endif
}
#endif

/*!
 * brief Configures a FlexCAN Receive Message Buffer.
 *
 * This function cleans a FlexCAN build-in Message Buffer and configures it
 * as a Receive Message Buffer.
 *
 * param base FlexCAN peripheral base address.
 * param mbIdx The Message Buffer index.
 * param config Pointer to the FlexCAN Message Buffer configuration structure.
 * param enable Enable/disable Rx Message Buffer.
 *               - true: Enable Rx Message Buffer.
 *               - false: Disable Rx Message Buffer.
 */
void FLEXCAN_SetRxMbConfig(CAN_Type *base, uint8_t mbIdx, const flexcan_rx_mb_config_t *config, bool enable)
{
    /* Assertion. */
    assert(mbIdx <= (base->MCR & CAN_MCR_MAXMB_MASK));
    assert(((NULL != config) || (false == enable)));
    assert(false == FLEXCAN_IsMbOccupied(base, mbIdx));

    uint32_t cs_temp = 0;

    /* Inactivate Message Buffer. */
    base->MB[mbIdx].CS = 0;

    /* Clean Message Buffer content. */
    base->MB[mbIdx].ID    = 0x0;
    base->MB[mbIdx].WORD0 = 0x0;
    base->MB[mbIdx].WORD1 = 0x0;

    if (enable)
    {
        /* Setup Message Buffer ID. */
        base->MB[mbIdx].ID = config->id;

        /* Setup Message Buffer format. */
        if (kFLEXCAN_FrameFormatExtend == config->format)
        {
            cs_temp |= CAN_CS_IDE_MASK;
        }

        /* Setup Message Buffer type. */
        if (kFLEXCAN_FrameTypeRemote == config->type)
        {
            cs_temp |= CAN_CS_RTR_MASK;
        }

        /* Activate Rx Message Buffer. */
        cs_temp |= CAN_CS_CODE(kFLEXCAN_RxMbEmpty);
        base->MB[mbIdx].CS = cs_temp;
    }
}

#if (defined(FSL_FEATURE_FLEXCAN_HAS_FLEXIBLE_DATA_RATE) && FSL_FEATURE_FLEXCAN_HAS_FLEXIBLE_DATA_RATE)
/*!
 * brief Configures a FlexCAN Receive Message Buffer.
 *
 * This function cleans a FlexCAN build-in Message Buffer and configures it
 * as a Receive Message Buffer.
 *
 * param base FlexCAN peripheral base address.
 * param mbIdx The Message Buffer index.
 * param config Pointer to the FlexCAN Message Buffer configuration structure.
 * param enable Enable/disable Rx Message Buffer.
 *               - true: Enable Rx Message Buffer.
 *               - false: Disable Rx Message Buffer.
 */
void FLEXCAN_SetFDRxMbConfig(CAN_Type *base, uint8_t mbIdx, const flexcan_rx_mb_config_t *config, bool enable)
{
    /* Assertion. */
    assert(mbIdx <= (base->MCR & CAN_MCR_MAXMB_MASK));
    assert(((NULL != config) || (false == enable)));
    assert(false == FLEXCAN_IsMbOccupied(base, mbIdx));

    uint32_t cs_temp          = 0;
    uint8_t cnt               = 0;
    volatile uint32_t *mbAddr = &(base->MB[0].CS);
    uint32_t offset           = FLEXCAN_GetFDMailboxOffset(base, mbIdx);
    uint8_t payload_dword     = 1;
    uint32_t dataSize         = (base->FDCTRL & CAN_FDCTRL_MBDSR0_MASK) >> CAN_FDCTRL_MBDSR0_SHIFT;

    /* Calculate the DWORD number, dataSize 0/1/2/3 corresponds to 8/16/32/64
       Bytes payload. */
    payload_dword = (2U << dataSize);
    for (cnt = 0; cnt < payload_dword; cnt++)
    {
        mbAddr[offset + 2U + cnt] = 0x0;
    }

    if (enable)
    {
        /* Setup Message Buffer ID. */
        mbAddr[offset + 1U] = config->id;

        /* Setup Message Buffer format. */
        if (kFLEXCAN_FrameFormatExtend == config->format)
        {
            cs_temp |= CAN_CS_IDE_MASK;
        }

        /* Setup Message Buffer type. */
        if (kFLEXCAN_FrameTypeRemote == config->type)
        {
            cs_temp |= CAN_CS_RTR_MASK;
        }

        /* Activate Rx Message Buffer. */
        cs_temp |= CAN_CS_CODE(kFLEXCAN_RxMbEmpty);
        mbAddr[offset] = cs_temp;
    }
}
#endif

/*!
 * brief Configures the FlexCAN Rx FIFO.
 *
 * This function configures the Rx FIFO with given Rx FIFO configuration.
 *
 * param base FlexCAN peripheral base address.
 * param config Pointer to the FlexCAN Rx FIFO configuration structure.
 * param enable Enable/disable Rx FIFO.
 *               - true: Enable Rx FIFO.
 *               - false: Disable Rx FIFO.
 */
void FLEXCAN_SetRxFifoConfig(CAN_Type *base, const flexcan_rx_fifo_config_t *config, bool enable)
{
    /* Assertion. */
    assert((NULL != config) || (false == enable));

    volatile uint32_t *idFilterRegion = (volatile uint32_t *)(&base->MB[6].CS);
    uint8_t i, rffn = 0;
    uint32_t setup_mb = 0;

    /* Enter Freeze Mode. */
    FLEXCAN_EnterFreezeMode(base);

    if (enable)
    {
        assert(config->idFilterNum <= 128U);

        /* Get the setup_mb value. */
        setup_mb = (uint8_t)((base->MCR & CAN_MCR_MAXMB_MASK) >> CAN_MCR_MAXMB_SHIFT);
        setup_mb = (setup_mb < (uint32_t)FSL_FEATURE_FLEXCAN_HAS_MESSAGE_BUFFER_MAX_NUMBERn(base)) ?
                       setup_mb :
                       (uint32_t)FSL_FEATURE_FLEXCAN_HAS_MESSAGE_BUFFER_MAX_NUMBERn(base);

        /* Determine RFFN value. */
        for (i = 0; i <= 0xFU; i++)
        {
            if ((8U * (i + 1U)) >= config->idFilterNum)
            {
                rffn = i;
                assert(((setup_mb - 8U) - (2U * rffn)) > 0U);

                base->CTRL2 = (base->CTRL2 & ~CAN_CTRL2_RFFN_MASK) | CAN_CTRL2_RFFN(rffn);
                break;
            }
        }
    }
    else
    {
        rffn = (uint8_t)((base->CTRL2 & CAN_CTRL2_RFFN_MASK) >> CAN_CTRL2_RFFN_SHIFT);
    }

    /* Clean ID filter table occuyied Message Buffer Region. */
    rffn = (uint8_t)((rffn + 1U) * 8U);
    for (i = 0; i < rffn; i++)
    {
        idFilterRegion[i] = 0x0;
    }

    if (enable)
    {
        /* Disable unused Rx FIFO Filter. */
        for (i = config->idFilterNum; i < rffn; i++)
        {
            idFilterRegion[i] = 0xFFFFFFFFU;
        }

        /* Copy ID filter table to Message Buffer Region. */
        for (i = 0; i < config->idFilterNum; i++)
        {
            idFilterRegion[i] = config->idFilterTable[i];
        }

        /* Setup ID Fitlter Type. */
        switch (config->idFilterType)
        {
            case kFLEXCAN_RxFifoFilterTypeA:
                base->MCR = (base->MCR & ~CAN_MCR_IDAM_MASK) | CAN_MCR_IDAM(0x0);
                break;
            case kFLEXCAN_RxFifoFilterTypeB:
                base->MCR = (base->MCR & ~CAN_MCR_IDAM_MASK) | CAN_MCR_IDAM(0x1);
                break;
            case kFLEXCAN_RxFifoFilterTypeC:
                base->MCR = (base->MCR & ~CAN_MCR_IDAM_MASK) | CAN_MCR_IDAM(0x2);
                break;
            case kFLEXCAN_RxFifoFilterTypeD:
                /* All frames rejected. */
                base->MCR = (base->MCR & ~CAN_MCR_IDAM_MASK) | CAN_MCR_IDAM(0x3);
                break;
            default:
                /* All the cases have been listed above, the default clause should not be reached. */
                assert(false);
                break;
        }

        /* Setting Message Reception Priority. */
        base->CTRL2 = (config->priority == kFLEXCAN_RxFifoPrioHigh) ? (base->CTRL2 & ~CAN_CTRL2_MRP_MASK) :
                                                                      (base->CTRL2 | CAN_CTRL2_MRP_MASK);

        /* Enable Rx Message FIFO. */
        base->MCR |= CAN_MCR_RFEN_MASK;
    }
    else
    {
        /* Disable Rx Message FIFO. */
        base->MCR &= ~CAN_MCR_RFEN_MASK;

        /* Clean MB0 ~ MB5. */
        FLEXCAN_SetRxMbConfig(base, 0, NULL, false);
        FLEXCAN_SetRxMbConfig(base, 1, NULL, false);
        FLEXCAN_SetRxMbConfig(base, 2, NULL, false);
        FLEXCAN_SetRxMbConfig(base, 3, NULL, false);
        FLEXCAN_SetRxMbConfig(base, 4, NULL, false);
        FLEXCAN_SetRxMbConfig(base, 5, NULL, false);
    }

    /* Exit Freeze Mode. */
    FLEXCAN_ExitFreezeMode(base);
}

#if (defined(FSL_FEATURE_FLEXCAN_HAS_RX_FIFO_DMA) && FSL_FEATURE_FLEXCAN_HAS_RX_FIFO_DMA)
/*!
 * brief Enables or disables the FlexCAN Rx FIFO DMA request.
 *
 * This function enables or disables the DMA feature of FlexCAN build-in Rx FIFO.
 *
 * param base FlexCAN peripheral base address.
 * param enable true to enable, false to disable.
 */
void FLEXCAN_EnableRxFifoDMA(CAN_Type *base, bool enable)
{
    if (enable)
    {
        /* Enter Freeze Mode. */
        FLEXCAN_EnterFreezeMode(base);

        /* Enable FlexCAN DMA. */
        base->MCR |= CAN_MCR_DMA_MASK;

        /* Exit Freeze Mode. */
        FLEXCAN_ExitFreezeMode(base);
    }
    else
    {
        /* Enter Freeze Mode. */
        FLEXCAN_EnterFreezeMode(base);

        /* Disable FlexCAN DMA. */
        base->MCR &= ~CAN_MCR_DMA_MASK;

        /* Exit Freeze Mode. */
        FLEXCAN_ExitFreezeMode(base);
    }
}
#endif /* FSL_FEATURE_FLEXCAN_HAS_RX_FIFO_DMA */

#if (defined(FSL_FEATURE_FLEXCAN_HAS_ERRATA_6032) && FSL_FEATURE_FLEXCAN_HAS_ERRATA_6032)
/*!
 * FlexCAN: A frame with wrong ID or payload is transmitted into
 * the CAN bus when the Message Buffer under transmission is
 * either aborted or deactivated while the CAN bus is in the Bus Idle state
 *
 * This function to do workaround for ERR006032
 *
 * param base FlexCAN peripheral base address.
 * param mbIdx The FlexCAN Message Buffer index.
 */
static void FLEXCAN_ERRATA_6032(CAN_Type *base, volatile uint32_t *mbCSAddr)
{
    uint32_t dbg_temp      = 0U;
    uint32_t u32TempCS     = 0U;
    uint32_t u32Timeout    = DELAY_BUSIDLE;
    uint32_t u32TempIMASK1 = base->IMASK1;
/*after backup all interruption, disable ALL interruption*/
#if (defined(FSL_FEATURE_FLEXCAN_HAS_EXTENDED_FLAG_REGISTER)) && (FSL_FEATURE_FLEXCAN_HAS_EXTENDED_FLAG_REGISTER > 0)
    uint32_t u32TempIMASK2 = base->IMASK2;
    base->IMASK2           = 0;
#endif
    base->IMASK1 = 0;
    dbg_temp     = (uint32_t)(base->DBG1);
    switch (dbg_temp & CAN_DBG1_CFSM_MASK)
    {
        case RXINTERMISSION:
            if (CBN_VALUE3 == (dbg_temp & CAN_DBG1_CBN_MASK))
            {
                /*wait until CFSM is different from RXINTERMISSION */
                while (RXINTERMISSION == (base->DBG1 & CAN_DBG1_CFSM_MASK))
                {
                    __NOP();
                }
            }
            break;
        case TXINTERMISSION:
            if (CBN_VALUE3 == (dbg_temp & CAN_DBG1_CBN_MASK))
            {
                /*wait until CFSM is different from TXINTERMISSION*/
                while (TXINTERMISSION == (base->DBG1 & CAN_DBG1_CFSM_MASK))
                {
                    __NOP();
                }
            }
            break;
        default:
            break;
    }
    /*Anyway, BUSIDLE need to delay*/
    if (BUSIDLE == (base->DBG1 & CAN_DBG1_CFSM_MASK))
    {
        while (u32Timeout-- > 0)
        {
            __NOP();
        }

        /*Write 0x0 into Code field of CS word.*/
        u32TempCS = (uint32_t)(*mbCSAddr);
        u32TempCS &= ~CAN_CS_CODE_MASK;
        *mbCSAddr = u32TempCS;
    }
    /*restore interruption*/
    base->IMASK1 = u32TempIMASK1;
#if (defined(FSL_FEATURE_FLEXCAN_HAS_EXTENDED_FLAG_REGISTER)) && (FSL_FEATURE_FLEXCAN_HAS_EXTENDED_FLAG_REGISTER > 0)
    base->IMASK2 = u32TempIMASK2;
#endif
}
#endif

/*!
 * brief Writes a FlexCAN Message to the Transmit Message Buffer.
 *
 * This function writes a CAN Message to the specified Transmit Message Buffer
 * and changes the Message Buffer state to start CAN Message transmit. After
 * that the function returns immediately.
 *
 * param base FlexCAN peripheral base address.
 * param mbIdx The FlexCAN Message Buffer index.
 * param txFrame Pointer to CAN message frame to be sent.
 * retval kStatus_Success - Write Tx Message Buffer Successfully.
 * retval kStatus_Fail    - Tx Message Buffer is currently in use.
 */
status_t FLEXCAN_WriteTxMb(CAN_Type *base, uint8_t mbIdx, const flexcan_frame_t *txFrame)
{
    /* Assertion. */
    assert(mbIdx <= (base->MCR & CAN_MCR_MAXMB_MASK));
    assert(NULL != txFrame);
    assert(txFrame->length <= 8U);
    assert(false == FLEXCAN_IsMbOccupied(base, mbIdx));

    uint32_t cs_temp = 0;

#if (defined(FSL_FEATURE_FLEXCAN_HAS_ERRATA_6032) && FSL_FEATURE_FLEXCAN_HAS_ERRATA_6032)
    FLEXCAN_ERRATA_6032(base, &(base->MB[mbIdx].CS));
#endif
    /* Check if Message Buffer is available. */
    if (CAN_CS_CODE(kFLEXCAN_TxMbDataOrRemote) != (base->MB[mbIdx].CS & CAN_CS_CODE_MASK))
    {
        /* Inactive Tx Message Buffer. */
        base->MB[mbIdx].CS = (base->MB[mbIdx].CS & ~CAN_CS_CODE_MASK) | CAN_CS_CODE(kFLEXCAN_TxMbInactive);

        /* Fill Message ID field. */
        base->MB[mbIdx].ID = txFrame->id;

        /* Fill Message Format field. */
        if ((uint32_t)kFLEXCAN_FrameFormatExtend == txFrame->format)
        {
            cs_temp |= CAN_CS_SRR_MASK | CAN_CS_IDE_MASK;
        }

        /* Fill Message Type field. */
        if ((uint32_t)kFLEXCAN_FrameTypeRemote == txFrame->type)
        {
            cs_temp |= CAN_CS_RTR_MASK;
        }

        cs_temp |= CAN_CS_CODE(kFLEXCAN_TxMbDataOrRemote) | CAN_CS_DLC(txFrame->length);

        /* Load Message Payload. */
        base->MB[mbIdx].WORD0 = txFrame->dataWord0;
        base->MB[mbIdx].WORD1 = txFrame->dataWord1;

        /* Activate Tx Message Buffer. */
        base->MB[mbIdx].CS = cs_temp;

#if ((defined(FSL_FEATURE_FLEXCAN_HAS_ERRATA_5641) && FSL_FEATURE_FLEXCAN_HAS_ERRATA_5641) || \
     (defined(FSL_FEATURE_FLEXCAN_HAS_ERRATA_5829) && FSL_FEATURE_FLEXCAN_HAS_ERRATA_5829))
        base->MB[FLEXCAN_GetFirstValidMb(base)].CS = CAN_CS_CODE(kFLEXCAN_TxMbInactive);
        base->MB[FLEXCAN_GetFirstValidMb(base)].CS = CAN_CS_CODE(kFLEXCAN_TxMbInactive);
#endif

        return kStatus_Success;
    }
    else
    {
        /* Tx Message Buffer is activated, return immediately. */
        return kStatus_Fail;
    }
}

#if (defined(FSL_FEATURE_FLEXCAN_HAS_FLEXIBLE_DATA_RATE) && FSL_FEATURE_FLEXCAN_HAS_FLEXIBLE_DATA_RATE)
/*!
 * brief Writes a FlexCAN FD Message to the Transmit Message Buffer.
 *
 * This function writes a CAN FD Message to the specified Transmit Message Buffer
 * and changes the Message Buffer state to start CAN FD Message transmit. After
 * that the function returns immediately.
 *
 * param base FlexCAN peripheral base address.
 * param mbIdx The FlexCAN FD Message Buffer index.
 * param txFrame Pointer to CAN FD message frame to be sent.
 * retval kStatus_Success - Write Tx Message Buffer Successfully.
 * retval kStatus_Fail    - Tx Message Buffer is currently in use.
 */
status_t FLEXCAN_WriteFDTxMb(CAN_Type *base, uint8_t mbIdx, const flexcan_fd_frame_t *txFrame)
{
    /* Assertion. */
    assert(mbIdx <= (base->MCR & CAN_MCR_MAXMB_MASK));
    assert(NULL != txFrame);
    assert(false == FLEXCAN_IsMbOccupied(base, mbIdx));

    uint32_t cs_temp      = 0;
    uint8_t cnt           = 0;
    uint32_t can_cs       = 0;
    uint8_t payload_dword = 1;
    uint32_t dataSize     = (base->FDCTRL & CAN_FDCTRL_MBDSR0_MASK) >> CAN_FDCTRL_MBDSR0_SHIFT;
#if ((defined(FSL_FEATURE_FLEXCAN_HAS_ERRATA_5641) && FSL_FEATURE_FLEXCAN_HAS_ERRATA_5641) || \
     (defined(FSL_FEATURE_FLEXCAN_HAS_ERRATA_5829) && FSL_FEATURE_FLEXCAN_HAS_ERRATA_5829))
    uint32_t availoffset = FLEXCAN_GetFDMailboxOffset(base, FLEXCAN_GetFirstValidMb(base));
#endif
    volatile uint32_t *mbAddr = &(base->MB[0].CS);
    uint32_t offset           = FLEXCAN_GetFDMailboxOffset(base, mbIdx);

#if (defined(FSL_FEATURE_FLEXCAN_HAS_ERRATA_6032) && FSL_FEATURE_FLEXCAN_HAS_ERRATA_6032)
    FLEXCAN_ERRATA_6032(base, &(mbAddr[offset]));
#endif

    can_cs = mbAddr[offset];
    /* Check if Message Buffer is available. */
    if (CAN_CS_CODE(kFLEXCAN_TxMbDataOrRemote) != (can_cs & CAN_CS_CODE_MASK))
    {
        /* Inactive Tx Message Buffer and Fill Message ID field. */
        mbAddr[offset]      = (can_cs & ~CAN_CS_CODE_MASK) | CAN_CS_CODE(kFLEXCAN_TxMbInactive);
        mbAddr[offset + 1U] = txFrame->id;

        /* Fill Message Format field. */
        if ((uint32_t)kFLEXCAN_FrameFormatExtend == txFrame->format)
        {
            cs_temp |= CAN_CS_SRR_MASK | CAN_CS_IDE_MASK;
        }

        /* Fill Message Type field. */
        if ((uint32_t)kFLEXCAN_FrameTypeRemote == txFrame->type)
        {
            cs_temp |= CAN_CS_RTR_MASK;
        }

        cs_temp |= CAN_CS_CODE(kFLEXCAN_TxMbDataOrRemote) | CAN_CS_DLC(txFrame->length) | CAN_CS_EDL(1) |
                   CAN_CS_BRS(txFrame->brs);

        /* Calculate the DWORD number, dataSize 0/1/2/3 corresponds to 8/16/32/64
           Bytes payload. */
        for (cnt = 0; cnt < (dataSize + 1U); cnt++)
        {
            payload_dword *= 2U;
        }

        /* Load Message Payload and Activate Tx Message Buffer. */
        for (cnt = 0; cnt < payload_dword; cnt++)
        {
            mbAddr[offset + 2U + cnt] = txFrame->dataWord[cnt];
        }
        mbAddr[offset] = cs_temp;

#if ((defined(FSL_FEATURE_FLEXCAN_HAS_ERRATA_5641) && FSL_FEATURE_FLEXCAN_HAS_ERRATA_5641) || \
     (defined(FSL_FEATURE_FLEXCAN_HAS_ERRATA_5829) && FSL_FEATURE_FLEXCAN_HAS_ERRATA_5829))
        mbAddr[availoffset] = CAN_CS_CODE(kFLEXCAN_TxMbInactive);
        mbAddr[availoffset] = CAN_CS_CODE(kFLEXCAN_TxMbInactive);
#endif

        return kStatus_Success;
    }
    else
    {
        /* Tx Message Buffer is activated, return immediately. */
        return kStatus_Fail;
    }
}
#endif

/*!
 * brief Reads a FlexCAN Message from Receive Message Buffer.
 *
 * This function reads a CAN message from a specified Receive Message Buffer.
 * The function fills a receive CAN message frame structure with
 * just received data and activates the Message Buffer again.
 * The function returns immediately.
 *
 * param base FlexCAN peripheral base address.
 * param mbIdx The FlexCAN Message Buffer index.
 * param rxFrame Pointer to CAN message frame structure for reception.
 * retval kStatus_Success            - Rx Message Buffer is full and has been read successfully.
 * retval kStatus_FLEXCAN_RxOverflow - Rx Message Buffer is already overflowed and has been read successfully.
 * retval kStatus_Fail               - Rx Message Buffer is empty.
 */
status_t FLEXCAN_ReadRxMb(CAN_Type *base, uint8_t mbIdx, flexcan_frame_t *rxFrame)
{
    /* Assertion. */
    assert(mbIdx <= (base->MCR & CAN_MCR_MAXMB_MASK));
    assert(NULL != rxFrame);
    assert(false == FLEXCAN_IsMbOccupied(base, mbIdx));

    uint32_t cs_temp;
    uint32_t rx_code;

    /* Read CS field of Rx Message Buffer to lock Message Buffer. */
    cs_temp = base->MB[mbIdx].CS;
    /* Get Rx Message Buffer Code field. */
    rx_code = (cs_temp & CAN_CS_CODE_MASK) >> CAN_CS_CODE_SHIFT;

    /* Check to see if Rx Message Buffer is full. */
    if (((uint32_t)kFLEXCAN_RxMbFull == rx_code) || ((uint32_t)kFLEXCAN_RxMbOverrun == rx_code))
    {
        /* Store Message ID. */
        rxFrame->id = base->MB[mbIdx].ID & (CAN_ID_EXT_MASK | CAN_ID_STD_MASK);

        /* Get the message ID and format. */
        rxFrame->format = (cs_temp & CAN_CS_IDE_MASK) != 0U ? (uint32_t)kFLEXCAN_FrameFormatExtend :
                                                              (uint32_t)kFLEXCAN_FrameFormatStandard;

        /* Get the message type. */
        rxFrame->type =
            (cs_temp & CAN_CS_RTR_MASK) != 0U ? (uint32_t)kFLEXCAN_FrameTypeRemote : (uint32_t)kFLEXCAN_FrameTypeData;

        /* Get the message length. */
        rxFrame->length = (uint8_t)((cs_temp & CAN_CS_DLC_MASK) >> CAN_CS_DLC_SHIFT);

        /* Get the time stamp. */
        rxFrame->timestamp = (uint16_t)((cs_temp & CAN_CS_TIME_STAMP_MASK) >> CAN_CS_TIME_STAMP_SHIFT);

        /* Store Message Payload. */
        rxFrame->dataWord0 = base->MB[mbIdx].WORD0;
        rxFrame->dataWord1 = base->MB[mbIdx].WORD1;

        /* Read free-running timer to unlock Rx Message Buffer. */
        (void)base->TIMER;

        if ((uint32_t)kFLEXCAN_RxMbFull == rx_code)
        {
            return kStatus_Success;
        }
        else
        {
            return kStatus_FLEXCAN_RxOverflow;
        }
    }
    else
    {
        /* Read free-running timer to unlock Rx Message Buffer. */
        (void)base->TIMER;

        return kStatus_Fail;
    }
}

#if (defined(FSL_FEATURE_FLEXCAN_HAS_FLEXIBLE_DATA_RATE) && FSL_FEATURE_FLEXCAN_HAS_FLEXIBLE_DATA_RATE)
/*!
 * brief Reads a FlexCAN FD Message from Receive Message Buffer.
 *
 * This function reads a CAN FD message from a specified Receive Message Buffer.
 * The function fills a receive CAN FD message frame structure with
 * just received data and activates the Message Buffer again.
 * The function returns immediately.
 *
 * param base FlexCAN peripheral base address.
 * param mbIdx The FlexCAN FD Message Buffer index.
 * param rxFrame Pointer to CAN FD message frame structure for reception.
 * retval kStatus_Success            - Rx Message Buffer is full and has been read successfully.
 * retval kStatus_FLEXCAN_RxOverflow - Rx Message Buffer is already overflowed and has been read successfully.
 * retval kStatus_Fail               - Rx Message Buffer is empty.
 */
status_t FLEXCAN_ReadFDRxMb(CAN_Type *base, uint8_t mbIdx, flexcan_fd_frame_t *rxFrame)
{
    /* Assertion. */
    assert(mbIdx <= (base->MCR & CAN_MCR_MAXMB_MASK));
    assert(NULL != rxFrame);
    assert(!FLEXCAN_IsMbOccupied(base, mbIdx));

    uint32_t cs_temp;
    uint8_t rx_code;
    uint8_t cnt     = 0;
    uint32_t can_id = 0;
    uint32_t dataSize;
    dataSize                  = (base->FDCTRL & CAN_FDCTRL_MBDSR0_MASK) >> CAN_FDCTRL_MBDSR0_SHIFT;
    uint8_t payload_dword     = 1;
    volatile uint32_t *mbAddr = &(base->MB[0].CS);
    uint32_t offset           = FLEXCAN_GetFDMailboxOffset(base, mbIdx);

    /* Read CS field of Rx Message Buffer to lock Message Buffer. */
    cs_temp = mbAddr[offset];
    can_id  = mbAddr[offset + 1U];

    /* Get Rx Message Buffer Code field. */
    rx_code = (uint8_t)((cs_temp & CAN_CS_CODE_MASK) >> CAN_CS_CODE_SHIFT);

    /* Check to see if Rx Message Buffer is full. */
    if (((uint8_t)kFLEXCAN_RxMbFull == rx_code) || ((uint8_t)kFLEXCAN_RxMbOverrun == rx_code))
    {
        /* Store Message ID. */
        rxFrame->id = can_id & (CAN_ID_EXT_MASK | CAN_ID_STD_MASK);

        /* Get the message ID and format. */
        rxFrame->format = (cs_temp & CAN_CS_IDE_MASK) != 0U ? (uint32_t)kFLEXCAN_FrameFormatExtend :
                                                              (uint32_t)kFLEXCAN_FrameFormatStandard;

        /* Get the message type. */
        rxFrame->type =
            (cs_temp & CAN_CS_RTR_MASK) != 0U ? (uint32_t)kFLEXCAN_FrameTypeRemote : (uint32_t)kFLEXCAN_FrameTypeData;

        /* Get the message length. */
        rxFrame->length = (uint8_t)((cs_temp & CAN_CS_DLC_MASK) >> CAN_CS_DLC_SHIFT);

        /* Get the time stamp. */
        rxFrame->timestamp = (uint16_t)((cs_temp & CAN_CS_TIME_STAMP_MASK) >> CAN_CS_TIME_STAMP_SHIFT);

        /* Calculate the DWORD number, dataSize 0/1/2/3 corresponds to 8/16/32/64
           Bytes payload. */
        for (cnt = 0; cnt < (dataSize + 1U); cnt++)
        {
            payload_dword *= 2U;
        }

        /* Store Message Payload. */
        for (cnt = 0; cnt < payload_dword; cnt++)
        {
            rxFrame->dataWord[cnt] = mbAddr[offset + 2U + cnt];
        }

        /* Read free-running timer to unlock Rx Message Buffer. */
        (void)base->TIMER;

        if ((uint32_t)kFLEXCAN_RxMbFull == rx_code)
        {
            return kStatus_Success;
        }
        else
        {
            return kStatus_FLEXCAN_RxOverflow;
        }
    }
    else
    {
        /* Read free-running timer to unlock Rx Message Buffer. */
        (void)base->TIMER;

        return kStatus_Fail;
    }
}
#endif

/*!
 * brief Reads a FlexCAN Message from Rx FIFO.
 *
 * This function reads a CAN message from the FlexCAN build-in Rx FIFO.
 *
 * param base FlexCAN peripheral base address.
 * param rxFrame Pointer to CAN message frame structure for reception.
 * retval kStatus_Success - Read Message from Rx FIFO successfully.
 * retval kStatus_Fail    - Rx FIFO is not enabled.
 */
status_t FLEXCAN_ReadRxFifo(CAN_Type *base, flexcan_frame_t *rxFrame)
{
    /* Assertion. */
    assert(NULL != rxFrame);

    uint32_t cs_temp;

    /* Check if Rx FIFO is Enabled. */
    if (0U != (base->MCR & CAN_MCR_RFEN_MASK))
    {
        /* Read CS field of Rx Message Buffer to lock Message Buffer. */
        cs_temp = base->MB[0].CS;

        /* Read data from Rx FIFO output port. */
        /* Store Message ID. */
        rxFrame->id = base->MB[0].ID & (CAN_ID_EXT_MASK | CAN_ID_STD_MASK);

        /* Get the message ID and format. */
        rxFrame->format = (cs_temp & CAN_CS_IDE_MASK) != 0U ? (uint32_t)kFLEXCAN_FrameFormatExtend :
                                                              (uint32_t)kFLEXCAN_FrameFormatStandard;

        /* Get the message type. */
        rxFrame->type =
            (cs_temp & CAN_CS_RTR_MASK) != 0U ? (uint32_t)kFLEXCAN_FrameTypeRemote : (uint32_t)kFLEXCAN_FrameTypeData;

        /* Get the message length. */
        rxFrame->length = (uint8_t)((cs_temp & CAN_CS_DLC_MASK) >> CAN_CS_DLC_SHIFT);

        /* Get the time stamp. */
        rxFrame->timestamp = (uint16_t)((cs_temp & CAN_CS_TIME_STAMP_MASK) >> CAN_CS_TIME_STAMP_SHIFT);

        /* Store Message Payload. */
        rxFrame->dataWord0 = base->MB[0].WORD0;
        rxFrame->dataWord1 = base->MB[0].WORD1;

        /* Store ID Filter Hit Index. */
        rxFrame->idhit = (uint8_t)(base->RXFIR & CAN_RXFIR_IDHIT_MASK);

        /* Read free-running timer to unlock Rx Message Buffer. */
        (void)base->TIMER;

        return kStatus_Success;
    }
    else
    {
        return kStatus_Fail;
    }
}

/*!
 * brief Performs a polling send transaction on the CAN bus.
 *
 * Note that a transfer handle does not need to be created  before calling this API.
 *
 * param base FlexCAN peripheral base pointer.
 * param mbIdx The FlexCAN Message Buffer index.
 * param txFrame Pointer to CAN message frame to be sent.
 * retval kStatus_Success - Write Tx Message Buffer Successfully.
 * retval kStatus_Fail    - Tx Message Buffer is currently in use.
 */
status_t FLEXCAN_TransferSendBlocking(CAN_Type *base, uint8_t mbIdx, flexcan_frame_t *txFrame)
{
    /* Write Tx Message Buffer to initiate a data sending. */
    if (kStatus_Success == FLEXCAN_WriteTxMb(base, mbIdx, (const flexcan_frame_t *)(uint32_t)txFrame))
    {
/* Wait until CAN Message send out. */
#if (defined(FSL_FEATURE_FLEXCAN_HAS_EXTENDED_FLAG_REGISTER)) && (FSL_FEATURE_FLEXCAN_HAS_EXTENDED_FLAG_REGISTER > 0)
        uint64_t u64flag = 1;
        while (0U == FLEXCAN_GetMbStatusFlags(base, u64flag << mbIdx))
#else
        uint32_t u32flag = 1;
        while (0U == FLEXCAN_GetMbStatusFlags(base, u32flag << mbIdx))
#endif
        {
        }

/* Clean Tx Message Buffer Flag. */
#if (defined(FSL_FEATURE_FLEXCAN_HAS_EXTENDED_FLAG_REGISTER)) && (FSL_FEATURE_FLEXCAN_HAS_EXTENDED_FLAG_REGISTER > 0)
        FLEXCAN_ClearMbStatusFlags(base, u64flag << mbIdx);
#else
        FLEXCAN_ClearMbStatusFlags(base, u32flag << mbIdx);
#endif
        /*After TX MB tranfered success, update the Timestamp from MB[mbIdx].CS register*/
        txFrame->timestamp = (uint16_t)((base->MB[mbIdx].CS & CAN_CS_TIME_STAMP_MASK) >> CAN_CS_TIME_STAMP_SHIFT);

        return kStatus_Success;
    }
    else
    {
        return kStatus_Fail;
    }
}

/*!
 * brief Performs a polling receive transaction on the CAN bus.
 *
 * Note that a transfer handle does not need to be created  before calling this API.
 *
 * param base FlexCAN peripheral base pointer.
 * param mbIdx The FlexCAN Message Buffer index.
 * param rxFrame Pointer to CAN message frame structure for reception.
 * retval kStatus_Success            - Rx Message Buffer is full and has been read successfully.
 * retval kStatus_FLEXCAN_RxOverflow - Rx Message Buffer is already overflowed and has been read successfully.
 * retval kStatus_Fail               - Rx Message Buffer is empty.
 */
status_t FLEXCAN_TransferReceiveBlocking(CAN_Type *base, uint8_t mbIdx, flexcan_frame_t *rxFrame)
{
/* Wait until Rx Message Buffer non-empty. */
#if (defined(FSL_FEATURE_FLEXCAN_HAS_EXTENDED_FLAG_REGISTER)) && (FSL_FEATURE_FLEXCAN_HAS_EXTENDED_FLAG_REGISTER > 0)
    uint64_t u64flag = 1;
    while (0U == FLEXCAN_GetMbStatusFlags(base, u64flag << mbIdx))
#else
    uint32_t u32flag = 1;
    while (0U == FLEXCAN_GetMbStatusFlags(base, u32flag << mbIdx))
#endif
    {
    }

/* Clean Rx Message Buffer Flag. */
#if (defined(FSL_FEATURE_FLEXCAN_HAS_EXTENDED_FLAG_REGISTER)) && (FSL_FEATURE_FLEXCAN_HAS_EXTENDED_FLAG_REGISTER > 0)
    FLEXCAN_ClearMbStatusFlags(base, u64flag << mbIdx);
#else
    FLEXCAN_ClearMbStatusFlags(base, u32flag << mbIdx);
#endif

    /* Read Received CAN Message. */
    return FLEXCAN_ReadRxMb(base, mbIdx, rxFrame);
}

#if (defined(FSL_FEATURE_FLEXCAN_HAS_FLEXIBLE_DATA_RATE) && FSL_FEATURE_FLEXCAN_HAS_FLEXIBLE_DATA_RATE)
/*!
 * brief Performs a polling send transaction on the CAN bus.
 *
 * Note that a transfer handle does not need to be created before calling this API.
 *
 * param base FlexCAN peripheral base pointer.
 * param mbIdx The FlexCAN FD Message Buffer index.
 * param txFrame Pointer to CAN FD message frame to be sent.
 * retval kStatus_Success - Write Tx Message Buffer Successfully.
 * retval kStatus_Fail    - Tx Message Buffer is currently in use.
 */
status_t FLEXCAN_TransferFDSendBlocking(CAN_Type *base, uint8_t mbIdx, flexcan_fd_frame_t *txFrame)
{
    /* Write Tx Message Buffer to initiate a data sending. */
    if (kStatus_Success == FLEXCAN_WriteFDTxMb(base, mbIdx, (const flexcan_fd_frame_t *)(uint32_t)txFrame))
    {
/* Wait until CAN Message send out. */
#if (defined(FSL_FEATURE_FLEXCAN_HAS_EXTENDED_FLAG_REGISTER)) && (FSL_FEATURE_FLEXCAN_HAS_EXTENDED_FLAG_REGISTER > 0)
        uint64_t u64flag = 1;
        while (0U == FLEXCAN_GetMbStatusFlags(base, u64flag << mbIdx))
#else
        uint32_t u32flag = 1;
        while (0U == FLEXCAN_GetMbStatusFlags(base, u32flag << mbIdx))
#endif
        {
        }

/* Clean Tx Message Buffer Flag. */
#if (defined(FSL_FEATURE_FLEXCAN_HAS_EXTENDED_FLAG_REGISTER)) && (FSL_FEATURE_FLEXCAN_HAS_EXTENDED_FLAG_REGISTER > 0)
        FLEXCAN_ClearMbStatusFlags(base, u64flag << mbIdx);
#else
        FLEXCAN_ClearMbStatusFlags(base, u32flag << mbIdx);
#endif
        /*After TX MB tranfered success, update the Timestamp from base->MB[offset for CANFD].CS register*/
        volatile uint32_t *mbAddr = &(base->MB[0].CS);
        uint32_t offset           = FLEXCAN_GetFDMailboxOffset(base, mbIdx);
        txFrame->timestamp        = (uint16_t)((mbAddr[offset] & CAN_CS_TIME_STAMP_MASK) >> CAN_CS_TIME_STAMP_SHIFT);

        return kStatus_Success;
    }
    else
    {
        return kStatus_Fail;
    }
}

/*!
 * brief Performs a polling receive transaction on the CAN bus.
 *
 * Note that a transfer handle does not need to be created before calling this API.
 *
 * param base FlexCAN peripheral base pointer.
 * param mbIdx The FlexCAN FD Message Buffer index.
 * param rxFrame Pointer to CAN FD message frame structure for reception.
 * retval kStatus_Success            - Rx Message Buffer is full and has been read successfully.
 * retval kStatus_FLEXCAN_RxOverflow - Rx Message Buffer is already overflowed and has been read successfully.
 * retval kStatus_Fail               - Rx Message Buffer is empty.
 */
status_t FLEXCAN_TransferFDReceiveBlocking(CAN_Type *base, uint8_t mbIdx, flexcan_fd_frame_t *rxFrame)
{
/* Wait until Rx Message Buffer non-empty. */
#if (defined(FSL_FEATURE_FLEXCAN_HAS_EXTENDED_FLAG_REGISTER)) && (FSL_FEATURE_FLEXCAN_HAS_EXTENDED_FLAG_REGISTER > 0)
    uint64_t u64flag = 1;
    while (0U == FLEXCAN_GetMbStatusFlags(base, u64flag << mbIdx))
#else
    uint32_t u32flag = 1;
    while (0U == FLEXCAN_GetMbStatusFlags(base, u32flag << mbIdx))
#endif
    {
    }

/* Clean Rx Message Buffer Flag. */
#if (defined(FSL_FEATURE_FLEXCAN_HAS_EXTENDED_FLAG_REGISTER)) && (FSL_FEATURE_FLEXCAN_HAS_EXTENDED_FLAG_REGISTER > 0)
    FLEXCAN_ClearMbStatusFlags(base, u64flag << mbIdx);
#else
    FLEXCAN_ClearMbStatusFlags(base, u32flag << mbIdx);
#endif

    /* Read Received CAN Message. */
    return FLEXCAN_ReadFDRxMb(base, mbIdx, rxFrame);
}
#endif

/*!
 * brief Performs a polling receive transaction from Rx FIFO on the CAN bus.
 *
 * Note that a transfer handle does not need to be created  before calling this API.
 *
 * param base FlexCAN peripheral base pointer.
 * param rxFrame Pointer to CAN message frame structure for reception.
 * retval kStatus_Success - Read Message from Rx FIFO successfully.
 * retval kStatus_Fail    - Rx FIFO is not enabled.
 */
status_t FLEXCAN_TransferReceiveFifoBlocking(CAN_Type *base, flexcan_frame_t *rxFrame)
{
    status_t rxFifoStatus;

    /* Wait until Rx FIFO non-empty. */
    while (0U == FLEXCAN_GetMbStatusFlags(base, (uint32_t)kFLEXCAN_RxFifoFrameAvlFlag))
    {
    }

    /*  */
    rxFifoStatus = FLEXCAN_ReadRxFifo(base, rxFrame);

    /* Clean Rx Fifo available flag. */
    FLEXCAN_ClearMbStatusFlags(base, (uint32_t)kFLEXCAN_RxFifoFrameAvlFlag);

    return rxFifoStatus;
}

/*!
 * brief Initializes the FlexCAN handle.
 *
 * This function initializes the FlexCAN handle, which can be used for other FlexCAN
 * transactional APIs. Usually, for a specified FlexCAN instance,
 * call this API once to get the initialized handle.
 *
 * param base FlexCAN peripheral base address.
 * param handle FlexCAN handle pointer.
 * param callback The callback function.
 * param userData The parameter of the callback function.
 */
void FLEXCAN_TransferCreateHandle(CAN_Type *base,
                                  flexcan_handle_t *handle,
                                  flexcan_transfer_callback_t callback,
                                  void *userData)
{
    assert(NULL != handle);

    uint8_t instance;

    /* Clean FlexCAN transfer handle. */
    (void)memset(handle, 0, sizeof(*handle));

    /* Get instance from peripheral base address. */
    instance = (uint8_t)FLEXCAN_GetInstance(base);

    /* Save the context in global variables to support the double weak mechanism. */
    s_flexcanHandle[instance] = handle;

    /* Register Callback function. */
    handle->callback = callback;
    handle->userData = userData;

    s_flexcanIsr = FLEXCAN_TransferHandleIRQ;

    /* We Enable Error & Status interrupt here, because this interrupt just
     * report current status of FlexCAN module through Callback function.
     * It is insignificance without a available callback function.
     */
    if (handle->callback != NULL)
    {
        FLEXCAN_EnableInterrupts(
            base, (uint32_t)kFLEXCAN_BusOffInterruptEnable | (uint32_t)kFLEXCAN_ErrorInterruptEnable |
                      (uint32_t)kFLEXCAN_RxWarningInterruptEnable | (uint32_t)kFLEXCAN_TxWarningInterruptEnable |
                      (uint32_t)kFLEXCAN_WakeUpInterruptEnable);
    }
    else
    {
        FLEXCAN_DisableInterrupts(
            base, (uint32_t)kFLEXCAN_BusOffInterruptEnable | (uint32_t)kFLEXCAN_ErrorInterruptEnable |
                      (uint32_t)kFLEXCAN_RxWarningInterruptEnable | (uint32_t)kFLEXCAN_TxWarningInterruptEnable |
                      (uint32_t)kFLEXCAN_WakeUpInterruptEnable);
    }

    /* Enable interrupts in NVIC. */
    (void)EnableIRQ((IRQn_Type)(s_flexcanRxWarningIRQ[instance]));
    (void)EnableIRQ((IRQn_Type)(s_flexcanTxWarningIRQ[instance]));
    (void)EnableIRQ((IRQn_Type)(s_flexcanWakeUpIRQ[instance]));
    (void)EnableIRQ((IRQn_Type)(s_flexcanErrorIRQ[instance]));
    (void)EnableIRQ((IRQn_Type)(s_flexcanBusOffIRQ[instance]));
    (void)EnableIRQ((IRQn_Type)(s_flexcanMbIRQ[instance]));
}

/*!
 * brief Sends a message using IRQ.
 *
 * This function sends a message using IRQ. This is a non-blocking function, which returns
 * right away. When messages have been sent out, the send callback function is called.
 *
 * param base FlexCAN peripheral base address.
 * param handle FlexCAN handle pointer.
 * param xfer FlexCAN Message Buffer transfer structure. See the #flexcan_mb_transfer_t.
 * retval kStatus_Success        Start Tx Message Buffer sending process successfully.
 * retval kStatus_Fail           Write Tx Message Buffer failed.
 * retval kStatus_FLEXCAN_TxBusy Tx Message Buffer is in use.
 */
status_t FLEXCAN_TransferSendNonBlocking(CAN_Type *base, flexcan_handle_t *handle, flexcan_mb_transfer_t *xfer)
{
    /* Assertion. */
    assert(NULL != handle);
    assert(NULL != xfer);
    assert(xfer->mbIdx <= (base->MCR & CAN_MCR_MAXMB_MASK));
    assert(false == FLEXCAN_IsMbOccupied(base, xfer->mbIdx));

    /* Check if Message Buffer is idle. */
    if ((uint8_t)kFLEXCAN_StateIdle == handle->mbState[xfer->mbIdx])
    {
        /* Distinguish transmit type. */
        if ((uint32_t)kFLEXCAN_FrameTypeRemote == xfer->frame->type)
        {
            handle->mbState[xfer->mbIdx] = (uint8_t)kFLEXCAN_StateTxRemote;
        }
        else
        {
            handle->mbState[xfer->mbIdx] = (uint8_t)kFLEXCAN_StateTxData;
        }

        /* Register user Frame buffer to the handle. */
        handle->mbFrameBuf[xfer->mbIdx] = xfer->frame;

        if (kStatus_Success == FLEXCAN_WriteTxMb(base, xfer->mbIdx, (const flexcan_frame_t *)(uint32_t)xfer->frame))
        {
/* Enable Message Buffer Interrupt. */
#if (defined(FSL_FEATURE_FLEXCAN_HAS_EXTENDED_FLAG_REGISTER)) && (FSL_FEATURE_FLEXCAN_HAS_EXTENDED_FLAG_REGISTER > 0)
            uint64_t u64mask = 1;
            FLEXCAN_EnableMbInterrupts(base, u64mask << xfer->mbIdx);
#else
            uint32_t u32mask = 1;
            FLEXCAN_EnableMbInterrupts(base, u32mask << xfer->mbIdx);
#endif

            return kStatus_Success;
        }
        else
        {
            handle->mbState[xfer->mbIdx] = (uint8_t)kFLEXCAN_StateIdle;
            return kStatus_Fail;
        }
    }
    else
    {
        return kStatus_FLEXCAN_TxBusy;
    }
}

/*!
 * brief Receives a message using IRQ.
 *
 * This function receives a message using IRQ. This is non-blocking function, which returns
 * right away. When the message has been received, the receive callback function is called.
 *
 * param base FlexCAN peripheral base address.
 * param handle FlexCAN handle pointer.
 * param xfer FlexCAN Message Buffer transfer structure. See the #flexcan_mb_transfer_t.
 * retval kStatus_Success        - Start Rx Message Buffer receiving process successfully.
 * retval kStatus_FLEXCAN_RxBusy - Rx Message Buffer is in use.
 */
status_t FLEXCAN_TransferReceiveNonBlocking(CAN_Type *base, flexcan_handle_t *handle, flexcan_mb_transfer_t *xfer)
{
    /* Assertion. */
    assert(NULL != handle);
    assert(NULL != xfer);
    assert(xfer->mbIdx <= (base->MCR & CAN_MCR_MAXMB_MASK));
    assert(false == FLEXCAN_IsMbOccupied(base, xfer->mbIdx));

    /* Check if Message Buffer is idle. */
    if ((uint8_t)kFLEXCAN_StateIdle == handle->mbState[xfer->mbIdx])
    {
        handle->mbState[xfer->mbIdx] = (uint8_t)kFLEXCAN_StateRxData;

        /* Register Message Buffer. */
        handle->mbFrameBuf[xfer->mbIdx] = xfer->frame;

/* Enable Message Buffer Interrupt. */
#if (defined(FSL_FEATURE_FLEXCAN_HAS_EXTENDED_FLAG_REGISTER)) && (FSL_FEATURE_FLEXCAN_HAS_EXTENDED_FLAG_REGISTER > 0)
        uint64_t u64mask = 1;
        FLEXCAN_EnableMbInterrupts(base, u64mask << xfer->mbIdx);
#else
        uint32_t u32mask = 1;
        FLEXCAN_EnableMbInterrupts(base, u32mask << xfer->mbIdx);
#endif

        return kStatus_Success;
    }
    else
    {
        return kStatus_FLEXCAN_RxBusy;
    }
}

#if (defined(FSL_FEATURE_FLEXCAN_HAS_FLEXIBLE_DATA_RATE) && FSL_FEATURE_FLEXCAN_HAS_FLEXIBLE_DATA_RATE)
/*!
 * brief Sends a message using IRQ.
 *
 * This function sends a message using IRQ. This is a non-blocking function, which returns
 * right away. When messages have been sent out, the send callback function is called.
 *
 * param base FlexCAN peripheral base address.
 * param handle FlexCAN handle pointer.
 * param xfer FlexCAN FD Message Buffer transfer structure. See the #flexcan_mb_transfer_t.
 * retval kStatus_Success        Start Tx Message Buffer sending process successfully.
 * retval kStatus_Fail           Write Tx Message Buffer failed.
 * retval kStatus_FLEXCAN_TxBusy Tx Message Buffer is in use.
 */
status_t FLEXCAN_TransferFDSendNonBlocking(CAN_Type *base, flexcan_handle_t *handle, flexcan_mb_transfer_t *xfer)
{
    /* Assertion. */
    assert(NULL != handle);
    assert(NULL != xfer);
    assert(xfer->mbIdx <= (base->MCR & CAN_MCR_MAXMB_MASK));
    assert(false == FLEXCAN_IsMbOccupied(base, xfer->mbIdx));

    /* Check if Message Buffer is idle. */
    if ((uint8_t)kFLEXCAN_StateIdle == handle->mbState[xfer->mbIdx])
    {
        /* Distinguish transmit type. */
        if ((uint32_t)kFLEXCAN_FrameTypeRemote == xfer->framefd->type)
        {
            handle->mbState[xfer->mbIdx] = (uint8_t)kFLEXCAN_StateTxRemote;
        }
        else
        {
            handle->mbState[xfer->mbIdx] = (uint8_t)kFLEXCAN_StateTxData;
        }

        /* Register user Frame buffer to the handle. */
        handle->mbFDFrameBuf[xfer->mbIdx] = xfer->framefd;

        if (kStatus_Success ==
            FLEXCAN_WriteFDTxMb(base, xfer->mbIdx, (const flexcan_fd_frame_t *)(uint32_t)xfer->framefd))
        {
/* Enable Message Buffer Interrupt. */
#if (defined(FSL_FEATURE_FLEXCAN_HAS_EXTENDED_FLAG_REGISTER)) && (FSL_FEATURE_FLEXCAN_HAS_EXTENDED_FLAG_REGISTER > 0)
            uint64_t u64mask = 1;
            FLEXCAN_EnableMbInterrupts(base, u64mask << xfer->mbIdx);
#else
            uint32_t u32mask = 1;
            FLEXCAN_EnableMbInterrupts(base, u32mask << xfer->mbIdx);
#endif

            return kStatus_Success;
        }
        else
        {
            handle->mbState[xfer->mbIdx] = (uint8_t)kFLEXCAN_StateIdle;
            return kStatus_Fail;
        }
    }
    else
    {
        return kStatus_FLEXCAN_TxBusy;
    }
}

/*!
 * brief Receives a message using IRQ.
 *
 * This function receives a message using IRQ. This is non-blocking function, which returns
 * right away. When the message has been received, the receive callback function is called.
 *
 * param base FlexCAN peripheral base address.
 * param handle FlexCAN handle pointer.
 * param xfer FlexCAN FD Message Buffer transfer structure. See the #flexcan_mb_transfer_t.
 * retval kStatus_Success        - Start Rx Message Buffer receiving process successfully.
 * retval kStatus_FLEXCAN_RxBusy - Rx Message Buffer is in use.
 */
status_t FLEXCAN_TransferFDReceiveNonBlocking(CAN_Type *base, flexcan_handle_t *handle, flexcan_mb_transfer_t *xfer)
{
    /* Assertion. */
    assert(NULL != handle);
    assert(NULL != xfer);
    assert(xfer->mbIdx <= (base->MCR & CAN_MCR_MAXMB_MASK));
    assert(false == FLEXCAN_IsMbOccupied(base, xfer->mbIdx));

    /* Check if Message Buffer is idle. */
    if ((uint8_t)kFLEXCAN_StateIdle == handle->mbState[xfer->mbIdx])
    {
        handle->mbState[xfer->mbIdx] = (uint8_t)kFLEXCAN_StateRxData;

        /* Register Message Buffer. */
        handle->mbFDFrameBuf[xfer->mbIdx] = xfer->framefd;

/* Enable Message Buffer Interrupt. */
#if (defined(FSL_FEATURE_FLEXCAN_HAS_EXTENDED_FLAG_REGISTER)) && (FSL_FEATURE_FLEXCAN_HAS_EXTENDED_FLAG_REGISTER > 0)
        uint64_t u64mask = 1;
        FLEXCAN_EnableMbInterrupts(base, u64mask << xfer->mbIdx);
#else
        uint32_t u32mask = 1;
        FLEXCAN_EnableMbInterrupts(base, u32mask << xfer->mbIdx);
#endif

        return kStatus_Success;
    }
    else
    {
        return kStatus_FLEXCAN_RxBusy;
    }
}
#endif

/*!
 * brief Receives a message from Rx FIFO using IRQ.
 *
 * This function receives a message using IRQ. This is a non-blocking function, which returns
 * right away. When all messages have been received, the receive callback function is called.
 *
 * param base FlexCAN peripheral base address.
 * param handle FlexCAN handle pointer.
 * param xfer FlexCAN Rx FIFO transfer structure. See the ref flexcan_fifo_transfer_t.
 * retval kStatus_Success            - Start Rx FIFO receiving process successfully.
 * retval kStatus_FLEXCAN_RxFifoBusy - Rx FIFO is currently in use.
 */
status_t FLEXCAN_TransferReceiveFifoNonBlocking(CAN_Type *base, flexcan_handle_t *handle, flexcan_fifo_transfer_t *xfer)
{
    /* Assertion. */
    assert(NULL != handle);
    assert(NULL != xfer);

    /* Check if Message Buffer is idle. */
    if ((uint8_t)kFLEXCAN_StateIdle == handle->rxFifoState)
    {
        handle->rxFifoState = (uint8_t)kFLEXCAN_StateRxFifo;

        /* Register Message Buffer. */
        handle->rxFifoFrameBuf = xfer->frame;

        /* Enable Message Buffer Interrupt. */
        FLEXCAN_EnableMbInterrupts(base, (uint32_t)kFLEXCAN_RxFifoOverflowFlag | (uint32_t)kFLEXCAN_RxFifoWarningFlag |
                                             (uint32_t)kFLEXCAN_RxFifoFrameAvlFlag);

        return kStatus_Success;
    }
    else
    {
        return kStatus_FLEXCAN_RxFifoBusy;
    }
}

/*!
 * brief Aborts the interrupt driven message send process.
 *
 * This function aborts the interrupt driven message send process.
 *
 * param base FlexCAN peripheral base address.
 * param handle FlexCAN handle pointer.
 * param mbIdx The FlexCAN Message Buffer index.
 */
void FLEXCAN_TransferAbortSend(CAN_Type *base, flexcan_handle_t *handle, uint8_t mbIdx)
{
    /* Assertion. */
    assert(NULL != handle);
    assert(mbIdx <= (base->MCR & CAN_MCR_MAXMB_MASK));
    assert(NULL != handle->mbFrameBuf[mbIdx]);
    assert(false == FLEXCAN_IsMbOccupied(base, mbIdx));

/* Disable Message Buffer Interrupt. */
#if (defined(FSL_FEATURE_FLEXCAN_HAS_EXTENDED_FLAG_REGISTER)) && (FSL_FEATURE_FLEXCAN_HAS_EXTENDED_FLAG_REGISTER > 0)
    uint64_t u64mask = 1;
    FLEXCAN_DisableMbInterrupts(base, u64mask << mbIdx);
#else
    uint32_t u32mask = 1;
    FLEXCAN_DisableMbInterrupts(base, u32mask << mbIdx);
#endif

    /*Update the TX frame 's time stamp by MB[mbIdx].cs*/
    handle->mbFrameBuf[mbIdx]->timestamp =
        (uint16_t)((base->MB[mbIdx].CS & CAN_CS_TIME_STAMP_MASK) >> CAN_CS_TIME_STAMP_SHIFT);

    /* Un-register handle. */
    handle->mbFrameBuf[mbIdx] = NULL;

    /* Clean Message Buffer. */
    FLEXCAN_SetTxMbConfig(base, mbIdx, true);

    handle->mbState[mbIdx] = (uint8_t)kFLEXCAN_StateIdle;
}

#if (defined(FSL_FEATURE_FLEXCAN_HAS_FLEXIBLE_DATA_RATE) && FSL_FEATURE_FLEXCAN_HAS_FLEXIBLE_DATA_RATE)
/*!
 * brief Aborts the interrupt driven message send process.
 *
 * This function aborts the interrupt driven message send process.
 *
 * param base FlexCAN peripheral base address.
 * param handle FlexCAN handle pointer.
 * param mbIdx The FlexCAN FD Message Buffer index.
 */
void FLEXCAN_TransferFDAbortSend(CAN_Type *base, flexcan_handle_t *handle, uint8_t mbIdx)
{
    /* Assertion. */
    assert(NULL != handle);
    assert(mbIdx <= (base->MCR & CAN_MCR_MAXMB_MASK));
    assert(NULL != handle->mbFDFrameBuf[mbIdx]);
    assert(false == FLEXCAN_IsMbOccupied(base, mbIdx));

/* Disable Message Buffer Interrupt. */
#if (defined(FSL_FEATURE_FLEXCAN_HAS_EXTENDED_FLAG_REGISTER)) && (FSL_FEATURE_FLEXCAN_HAS_EXTENDED_FLAG_REGISTER > 0)
    uint64_t u64mask = 1;
    FLEXCAN_DisableMbInterrupts(base, u64mask << mbIdx);
#else
    uint32_t u32mask = 1;
    FLEXCAN_DisableMbInterrupts(base, u32mask << mbIdx);
#endif

    /*Update the TX frame 's time stamp by base->MB[offset for CANFD].CS*/
    volatile uint32_t *mbAddr = &(base->MB[0].CS);
    uint32_t offset           = FLEXCAN_GetFDMailboxOffset(base, mbIdx);
    handle->mbFDFrameBuf[mbIdx]->timestamp =
        (uint16_t)((mbAddr[offset] & CAN_CS_TIME_STAMP_MASK) >> CAN_CS_TIME_STAMP_SHIFT);

    /* Un-register handle. */
    handle->mbFDFrameBuf[mbIdx] = NULL;

    /* Clean Message Buffer. */
    FLEXCAN_SetFDTxMbConfig(base, mbIdx, true);

    handle->mbState[mbIdx] = (uint8_t)kFLEXCAN_StateIdle;
}

/*!
 * brief Aborts the interrupt driven message receive process.
 *
 * This function aborts the interrupt driven message receive process.
 *
 * param base FlexCAN peripheral base address.
 * param handle FlexCAN handle pointer.
 * param mbIdx The FlexCAN FD Message Buffer index.
 */
void FLEXCAN_TransferFDAbortReceive(CAN_Type *base, flexcan_handle_t *handle, uint8_t mbIdx)
{
    /* Assertion. */
    assert(NULL != handle);
    assert(mbIdx <= (base->MCR & CAN_MCR_MAXMB_MASK));
    assert(false == FLEXCAN_IsMbOccupied(base, mbIdx));

/* Disable Message Buffer Interrupt. */
#if (defined(FSL_FEATURE_FLEXCAN_HAS_EXTENDED_FLAG_REGISTER)) && (FSL_FEATURE_FLEXCAN_HAS_EXTENDED_FLAG_REGISTER > 0)
    uint64_t u64mask = 1;
    FLEXCAN_DisableMbInterrupts(base, u64mask << mbIdx);
#else
    uint32_t u32mask = 1;
    FLEXCAN_DisableMbInterrupts(base, u32mask << mbIdx);
#endif

    /* Un-register handle. */
    handle->mbFDFrameBuf[mbIdx] = NULL;
    handle->mbState[mbIdx]      = (uint8_t)kFLEXCAN_StateIdle;
}
#endif

/*!
 * brief Aborts the interrupt driven message receive process.
 *
 * This function aborts the interrupt driven message receive process.
 *
 * param base FlexCAN peripheral base address.
 * param handle FlexCAN handle pointer.
 * param mbIdx The FlexCAN Message Buffer index.
 */
void FLEXCAN_TransferAbortReceive(CAN_Type *base, flexcan_handle_t *handle, uint8_t mbIdx)
{
    /* Assertion. */
    assert(NULL != handle);
    assert(mbIdx <= (base->MCR & CAN_MCR_MAXMB_MASK));
    assert(false == FLEXCAN_IsMbOccupied(base, mbIdx));

/* Disable Message Buffer Interrupt. */
#if (defined(FSL_FEATURE_FLEXCAN_HAS_EXTENDED_FLAG_REGISTER)) && (FSL_FEATURE_FLEXCAN_HAS_EXTENDED_FLAG_REGISTER > 0)
    uint64_t u64mask = 1;
    FLEXCAN_DisableMbInterrupts(base, (u64mask << mbIdx));
#else
    uint32_t u32mask = 1;
    FLEXCAN_DisableMbInterrupts(base, (u32mask << mbIdx));
#endif

    /* Un-register handle. */
    handle->mbFrameBuf[mbIdx] = NULL;
    handle->mbState[mbIdx]    = (uint8_t)kFLEXCAN_StateIdle;
}

/*!
 * brief Aborts the interrupt driven message receive from Rx FIFO process.
 *
 * This function aborts the interrupt driven message receive from Rx FIFO process.
 *
 * param base FlexCAN peripheral base address.
 * param handle FlexCAN handle pointer.
 */
void FLEXCAN_TransferAbortReceiveFifo(CAN_Type *base, flexcan_handle_t *handle)
{
    /* Assertion. */
    assert(NULL != handle);

    /* Check if Rx FIFO is enabled. */
    if (0U != (base->MCR & CAN_MCR_RFEN_MASK))
    {
        /* Disable Rx Message FIFO Interrupts. */
        FLEXCAN_DisableMbInterrupts(base, (uint32_t)kFLEXCAN_RxFifoOverflowFlag | (uint32_t)kFLEXCAN_RxFifoWarningFlag |
                                              (uint32_t)kFLEXCAN_RxFifoFrameAvlFlag);

        /* Un-register handle. */
        handle->rxFifoFrameBuf = NULL;
    }

    handle->rxFifoState = (uint8_t)kFLEXCAN_StateIdle;
}

/*!
 * brief FlexCAN IRQ handle function.
 *
 * This function handles the FlexCAN Error, the Message Buffer, and the Rx FIFO IRQ request.
 *
 * param base FlexCAN peripheral base address.
 * param handle FlexCAN handle pointer.
 */
void FLEXCAN_TransferHandleIRQ(CAN_Type *base, flexcan_handle_t *handle)
{
    /* Assertion. */
    assert(NULL != handle);

    status_t status = kStatus_FLEXCAN_UnHandled;
    uint32_t result = 0U;
    /* Store Current FlexCAN Module Error and Status. */
    uint32_t EsrStatus = base->ESR1;

    do
    {
        /* Solve FlexCAN Error and Status Interrupt. */
        if (0U != (EsrStatus & ((uint32_t)kFLEXCAN_TxWarningIntFlag | (uint32_t)kFLEXCAN_RxWarningIntFlag |
                                (uint32_t)kFLEXCAN_BusOffIntFlag | (uint32_t)kFLEXCAN_ErrorIntFlag)))
        {
            status = kStatus_FLEXCAN_ErrorStatus;

            /* Clear FlexCAN Error and Status Interrupt. */
            FLEXCAN_ClearStatusFlags(base, (uint32_t)kFLEXCAN_TxWarningIntFlag | (uint32_t)kFLEXCAN_RxWarningIntFlag |
                                               (uint32_t)kFLEXCAN_BusOffIntFlag | (uint32_t)kFLEXCAN_ErrorIntFlag);
        }
        else if (0U != (EsrStatus & (uint32_t)kFLEXCAN_WakeUpIntFlag))
        {
            status = kStatus_FLEXCAN_WakeUp;
            FLEXCAN_ClearStatusFlags(base, (uint32_t)kFLEXCAN_WakeUpIntFlag);
        }
        /* Solve FlexCAN Rx FIFO & Message Buffer Interrupt. */
        else
        {
            /* For this implementation, we solve the Message with lowest MB index first. */
            for (result = 0U; result < (uint32_t)FSL_FEATURE_FLEXCAN_HAS_MESSAGE_BUFFER_MAX_NUMBERn(base); result++)
            {
/* Get the lowest unhandled Message Buffer */
#if (defined(FSL_FEATURE_FLEXCAN_HAS_EXTENDED_FLAG_REGISTER)) && (FSL_FEATURE_FLEXCAN_HAS_EXTENDED_FLAG_REGISTER > 0)
                uint64_t u64flag = 1;
                if (0U != FLEXCAN_GetMbStatusFlags(base, (u64flag << result)))
                {
                    if (true == FLEXCAN_IsMbIntEnabled(base, result))
#else
                uint32_t u32flag = 1;
                if (0U != FLEXCAN_GetMbStatusFlags(base, (u32flag << result)))
                {
                    if (true == FLEXCAN_IsMbIntEnabled(base, (uint8_t)result))
#endif
                    {
                        break;
                    }
                }
            }

            /* Does not find Message to deal with. */
            if (result == (uint32_t)FSL_FEATURE_FLEXCAN_HAS_MESSAGE_BUFFER_MAX_NUMBERn(base))
            {
                break;
            }

            /* Solve Rx FIFO interrupt. */
            if (((uint8_t)kFLEXCAN_StateIdle != handle->rxFifoState) && (result <= (uint32_t)CAN_IFLAG1_BUF7I_SHIFT))
            {
                uint32_t u32mask = 1;
                switch (u32mask << result)
                {
                    case (int32_t)kFLEXCAN_RxFifoOverflowFlag:
                        status = kStatus_FLEXCAN_RxFifoOverflow;
                        break;

                    case (int32_t)kFLEXCAN_RxFifoWarningFlag:
                        status = kStatus_FLEXCAN_RxFifoWarning;
                        break;

                    case (int32_t)kFLEXCAN_RxFifoFrameAvlFlag:
                        status = FLEXCAN_ReadRxFifo(base, handle->rxFifoFrameBuf);
                        if (kStatus_Success == status)
                        {
                            status = kStatus_FLEXCAN_RxFifoIdle;
                        }
                        FLEXCAN_TransferAbortReceiveFifo(base, handle);
                        break;

                    default:
                        status = kStatus_FLEXCAN_UnHandled;
                        break;
                }
            }
            else
            {
                /* Get current State of Message Buffer. */
                switch (handle->mbState[result])
                {
                    /* Solve Rx Data Frame or Rx Remote Frame. */
                    case (uint8_t)kFLEXCAN_StateRxData:
                    case (uint8_t)kFLEXCAN_StateRxRemote:
#if (defined(FSL_FEATURE_FLEXCAN_HAS_FLEXIBLE_DATA_RATE) && FSL_FEATURE_FLEXCAN_HAS_FLEXIBLE_DATA_RATE)
                        if (0U != (base->MCR & CAN_MCR_FDEN_MASK))
                        {
                            status = FLEXCAN_ReadFDRxMb(base, (uint8_t)result, handle->mbFDFrameBuf[result]);
                        }
                        else
                        {
                            status = FLEXCAN_ReadRxMb(base, (uint8_t)result, handle->mbFrameBuf[result]);
                        }
#else
                        status = FLEXCAN_ReadRxMb(base, result, handle->mbFrameBuf[result]);
#endif
                        if (kStatus_Success == status)
                        {
                            status = kStatus_FLEXCAN_RxIdle;
                        }
#if (defined(FSL_FEATURE_FLEXCAN_HAS_FLEXIBLE_DATA_RATE) && FSL_FEATURE_FLEXCAN_HAS_FLEXIBLE_DATA_RATE)
                        if (0U != (base->MCR & CAN_MCR_FDEN_MASK))
                        {
                            FLEXCAN_TransferFDAbortReceive(base, handle, (uint8_t)result);
                        }
                        else
                        {
                            FLEXCAN_TransferAbortReceive(base, handle, (uint8_t)result);
                        }
#else
                        FLEXCAN_TransferAbortReceive(base, handle, result);
#endif
                        break;

                    /* Solve Tx Data Frame. */
                    case (uint8_t)kFLEXCAN_StateTxData:
                        status = kStatus_FLEXCAN_TxIdle;
#if (defined(FSL_FEATURE_FLEXCAN_HAS_FLEXIBLE_DATA_RATE) && FSL_FEATURE_FLEXCAN_HAS_FLEXIBLE_DATA_RATE)
                        if (0U != (base->MCR & CAN_MCR_FDEN_MASK))
                        {
                            FLEXCAN_TransferFDAbortSend(base, handle, (uint8_t)result);
                        }
                        else
                        {
                            FLEXCAN_TransferAbortSend(base, handle, (uint8_t)result);
                        }
#else
                        FLEXCAN_TransferAbortSend(base, handle, result);
#endif
                        break;

                    /* Solve Tx Remote Frame. */
                    case (uint8_t)kFLEXCAN_StateTxRemote:
                        handle->mbState[result] = (uint8_t)kFLEXCAN_StateRxRemote;
                        status                  = kStatus_FLEXCAN_TxSwitchToRx;
                        break;

                    default:
                        status = kStatus_FLEXCAN_UnHandled;
                        break;
                }
            }

/* Clear resolved Message Buffer IRQ. */
#if (defined(FSL_FEATURE_FLEXCAN_HAS_EXTENDED_FLAG_REGISTER)) && (FSL_FEATURE_FLEXCAN_HAS_EXTENDED_FLAG_REGISTER > 0)
            uint64_t u64flag = 1;
            FLEXCAN_ClearMbStatusFlags(base, u64flag << result);
#else
            uint32_t u32flag = 1;
            FLEXCAN_ClearMbStatusFlags(base, u32flag << result);
#endif
        }

        /* Calling Callback Function if has one. */
        if (handle->callback != NULL)
        {
            handle->callback(base, handle, status, result, handle->userData);
        }

        /* Reset return status */
        status = kStatus_FLEXCAN_UnHandled;

        /* Store Current FlexCAN Module Error and Status. */
        EsrStatus = base->ESR1;
    }
#if (defined(FSL_FEATURE_FLEXCAN_HAS_EXTENDED_FLAG_REGISTER)) && (FSL_FEATURE_FLEXCAN_HAS_EXTENDED_FLAG_REGISTER > 0)
    while ((0U != FLEXCAN_GetMbStatusFlags(base, 0xFFFFFFFFFFFFFFFFU)) ||
           (0U != (EsrStatus & ((uint32_t)kFLEXCAN_TxWarningIntFlag | (uint32_t)kFLEXCAN_RxWarningIntFlag |
                                (uint32_t)kFLEXCAN_BusOffIntFlag | (uint32_t)kFLEXCAN_ErrorIntFlag |
                                (uint32_t)kFLEXCAN_WakeUpIntFlag))));
#else
    while ((0U != FLEXCAN_GetMbStatusFlags(base, 0xFFFFFFFFU)) ||
           (0U != (EsrStatus & ((uint32_t)kFLEXCAN_TxWarningIntFlag | (uint32_t)kFLEXCAN_RxWarningIntFlag |
                                (uint32_t)kFLEXCAN_BusOffIntFlag | (uint32_t)kFLEXCAN_ErrorIntFlag |
                                (uint32_t)kFLEXCAN_WakeUpIntFlag))));
#endif
}

#if defined(CAN0)
void CAN0_DriverIRQHandler(void)
{
    assert(NULL != s_flexcanHandle[0]);

    s_flexcanIsr(CAN0, s_flexcanHandle[0]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif

#if defined(CAN1)
void CAN1_DriverIRQHandler(void)
{
    assert(NULL != s_flexcanHandle[1]);

    s_flexcanIsr(CAN1, s_flexcanHandle[1]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif

#if defined(CAN2)
void CAN2_DriverIRQHandler(void)
{
    assert(NULL != s_flexcanHandle[2]);

    s_flexcanIsr(CAN2, s_flexcanHandle[2]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif

#if defined(CAN3)
void CAN3_DriverIRQHandler(void)
{
    assert(NULL != s_flexcanHandle[3]);

    s_flexcanIsr(CAN3, s_flexcanHandle[3]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif

#if defined(CAN4)
void CAN4_DriverIRQHandler(void)
{
    assert(NULL != s_flexcanHandle[4]);

    s_flexcanIsr(CAN4, s_flexcanHandle[4]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif

#if defined(DMA__CAN0)
void DMA_FLEXCAN0_INT_DriverIRQHandler(void)
{
    assert(s_flexcanHandle[FLEXCAN_GetInstance(DMA__CAN0)]);

    s_flexcanIsr(DMA__CAN0, s_flexcanHandle[FLEXCAN_GetInstance(DMA__CAN0)]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif

#if defined(DMA__CAN1)
void DMA_FLEXCAN1_INT_DriverIRQHandler(void)
{
    assert(s_flexcanHandle[FLEXCAN_GetInstance(DMA__CAN1)]);

    s_flexcanIsr(DMA__CAN1, s_flexcanHandle[FLEXCAN_GetInstance(DMA__CAN1)]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif

#if defined(DMA__CAN2)
void DMA_FLEXCAN2_INT_DriverIRQHandler(void)
{
    assert(s_flexcanHandle[FLEXCAN_GetInstance(DMA__CAN2)]);

    s_flexcanIsr(DMA__CAN2, s_flexcanHandle[FLEXCAN_GetInstance(DMA__CAN2)]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif

#if defined(ADMA__CAN0)
void ADMA_FLEXCAN0_INT_DriverIRQHandler(void)
{
    assert(s_flexcanHandle[FLEXCAN_GetInstance(ADMA__CAN0)]);

    s_flexcanIsr(ADMA__CAN0, s_flexcanHandle[FLEXCAN_GetInstance(ADMA__CAN0)]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif

#if defined(ADMA__CAN1)
void ADMA_FLEXCAN1_INT_DriverIRQHandler(void)
{
    assert(s_flexcanHandle[FLEXCAN_GetInstance(ADMA__CAN1)]);

    s_flexcanIsr(ADMA__CAN1, s_flexcanHandle[FLEXCAN_GetInstance(ADMA__CAN1)]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif

#if defined(ADMA__CAN2)
void ADMA_FLEXCAN2_INT_DriverIRQHandler(void)
{
    assert(s_flexcanHandle[FLEXCAN_GetInstance(ADMA__CAN2)]);

    s_flexcanIsr(ADMA__CAN2, s_flexcanHandle[FLEXCAN_GetInstance(ADMA__CAN2)]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif
