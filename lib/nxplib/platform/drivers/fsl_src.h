/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_SRC_H_
#define _FSL_SRC_H_

#include "fsl_common.h"

/*!
 * @addtogroup src
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
/*@{*/
/*! @brief SRC driver version 2.0.1. */
#define FSL_SRC_DRIVER_VERSION (MAKE_VERSION(2, 0, 1))
/*@}*/

/*!
 * @brief SRC reset status flags.
 */
enum _src_reset_status_flags
{
#if (defined(FSL_FEATURE_SRC_HAS_SRSR_RESET_OUT) && FSL_FEATURE_SRC_HAS_SRSR_RESET_OUT)
    kSRC_ResetOutputEnableFlag = SRC_SRSR_RESET_OUT_MASK, /*!< This bit indicates if RESET status is
                                                               driven out on PTE0 pin. */
#endif                                                    /* FSL_FEATURE_SRC_HAS_SRSR_RESET_OUT */
#if !(defined(FSL_FEATURE_SRC_HAS_NO_SRSR_WBI) && FSL_FEATURE_SRC_HAS_NO_SRSR_WBI)
    kSRC_WarmBootIndicationFlag = SRC_SRSR_WBI_MASK,     /*!< WARM boot indication shows that WARM boot
                                                              was initiated by software. */
#endif                                                   /* FSL_FEATURE_SRC_HAS_NO_SRSR_WBI */
    kSRC_TemperatureSensorResetFlag = SRC_SRSR_TSR_MASK, /*!< Indicates whether the reset was the
                                                              result of software reset from on-chip
                                                              Temperature Sensor. Temperature Sensor
                                                              Interrupt needs to be served before this
                                                              bit can be cleaned.*/
#if (defined(FSL_FEATURE_SRC_HAS_SRSR_WDOG3_RST_B) && FSL_FEATURE_SRC_HAS_SRSR_WDOG3_RST_B)
    kSRC_Wdog3ResetFlag = SRC_SRSR_WDOG3_RST_B_MASK, /*!< IC Watchdog3 Time-out reset. Indicates
                                                          whether the reset was the result of the
                                                          watchdog3 time-out event. */
#endif                                               /* FSL_FEATURE_SRC_HAS_SRSR_WDOG3_RST_B */
#if (defined(FSL_FEATURE_SRC_HAS_SRSR_SW) && FSL_FEATURE_SRC_HAS_SRSR_SW)
    kSRC_SoftwareResetFlag = SRC_SRSR_SW_MASK, /*!< Indicates a reset has been caused by software
                                                    setting of SYSRESETREQ bit in Application
                                                    Interrupt and Reset Control Register in the
                                                    ARM core. */
#endif                                         /* FSL_FEATURE_SRC_HAS_SRSR_SW */
#if (defined(FSL_FEATURE_SRC_HAS_SRSR_JTAG_SW_RST) && FSL_FEATURE_SRC_HAS_SRSR_JTAG_SW_RST)
    kSRC_JTAGSystemResetFlag =
        SRC_SRSR_JTAG_SW_RST_MASK, /*!< Indicates whether the reset was the result of software reset form JTAG */
#endif                             /* FSL_FEATURE_SRC_HAS_SRSR_JTAG_SW_RST */
    kSRC_JTAGSoftwareResetFlag = SRC_SRSR_SJC_MASK,   /*!< Indicates whether the reset was the result of
                                                      setting SJC_GPCCR bit 31. */
    kSRC_JTAGGeneratedResetFlag = SRC_SRSR_JTAG_MASK, /*!< Indicates a reset has been caused by JTAG
                                                           selection of certain IR codes: EXTEST or
                                                           HIGHZ. */
    kSRC_WatchdogResetFlag = SRC_SRSR_WDOG_MASK,      /*!< Indicates a reset has been caused by the
                                                           watchdog timer timing out. This reset source
                                                           can be blocked by disabling the watchdog. */
#if (defined(FSL_FEATURE_SRC_HAS_SRSR_IPP_USER_RESET_B) && FSL_FEATURE_SRC_HAS_SRSR_IPP_USER_RESET_B)
    kSRC_IppUserResetFlag = SRC_SRSR_IPP_USER_RESET_B_MASK, /*!< Indicates whether the reset was the
                                                                 result of the ipp_user_reset_b
                                                                 qualified reset. */
#endif                                                      /* FSL_FEATURE_SRC_HAS_SRSR_IPP_USER_RESET_B */
#if (defined(FSL_FEATURE_SRC_HAS_SRSR_SNVS) && FSL_FEATURE_SRC_HAS_SRSR_SNVS)
    kSRC_SNVSFailResetFlag = SRC_SRSR_SNVS_MASK, /*!< SNVS hardware failure will always cause a cold
                                                      reset. This flag indicates whether the reset
                                                      is a result of SNVS hardware failure. */
#endif                                           /* FSL_FEATURE_SRC_HAS_SRSR_SNVS */
#if (defined(FSL_FEATURE_SRC_HAS_SRSR_CSU_RESET_B) && FSL_FEATURE_SRC_HAS_SRSR_CSU_RESET_B)
    kSRC_CsuResetFlag = SRC_SRSR_CSU_RESET_B_MASK, /*!< Indicates whether the reset was the result
                                                        of the csu_reset_b input. */
#endif                                             /* FSL_FEATURE_SRC_HAS_SRSR_CSU_RESET_B */
#if (defined(FSL_FEATURE_SRC_HAS_SRSR_LOCKUP) && FSL_FEATURE_SRC_HAS_SRSR_LOCKUP)
    kSRC_CoreLockupResetFlag = SRC_SRSR_LOCKUP_MASK, /*!< Indicates a reset has been caused by the
                                                          ARM core indication of a LOCKUP event. */
#endif                                               /* FSL_FEATURE_SRC_HAS_SRSR_LOCKUP */
#if (defined(FSL_FEATURE_SRC_HAS_SRSR_POR) && FSL_FEATURE_SRC_HAS_SRSR_POR)
    kSRC_PowerOnResetFlag = SRC_SRSR_POR_MASK, /*!< Indicates a reset has been caused by the
                                                    power-on detection logic. */
#endif                                         /* FSL_FEATURE_SRC_HAS_SRSR_POR */
#if (defined(FSL_FEATURE_SRC_HAS_SRSR_LOCKUP_SYSRESETREQ) && FSL_FEATURE_SRC_HAS_SRSR_LOCKUP_SYSRESETREQ)
    kSRC_LockupSysResetFlag =
        SRC_SRSR_LOCKUP_SYSRESETREQ_MASK, /*!< Indicates a reset has been caused by CPU lockup or software
                                               setting of SYSRESETREQ bit in Application Interrupt and
                                               Reset Control Register of the ARM core. */
#endif                                    /* FSL_FEATURE_SRC_HAS_SRSR_LOCKUP_SYSRESETREQ */
#if (defined(FSL_FEATURE_SRC_HAS_SRSR_IPP_RESET_B) && FSL_FEATURE_SRC_HAS_SRSR_IPP_RESET_B)
    kSRC_IppResetPinFlag = SRC_SRSR_IPP_RESET_B_MASK, /*!< Indicates whether reset was the result of
                                                           ipp_reset_b pin (Power-up sequence). */
#endif                                                /* FSL_FEATURE_SRC_HAS_SRSR_IPP_RESET_B */
};

#if (defined(FSL_FEATURE_SRC_HAS_SISR) && FSL_FEATURE_SRC_HAS_SISR)
/*!
 * @brief SRC interrupt status flag.
 */
enum _src_status_flags
{
    kSRC_Core0WdogResetReqFlag =
        SRC_SISR_CORE0_WDOG_RST_REQ_MASK, /*!< WDOG reset request from core0. Read-only status bit. */
};
#endif /* FSL_FEATURE_SRC_HAS_SISR */

#if (defined(FSL_FEATURE_SRC_HAS_SCR_MIX_RST_STRCH) && FSL_FEATURE_SRC_HAS_SCR_MIX_RST_STRCH)
/*!
 * @brief Selection of SoC mix power reset stretch.
 *
 * This type defines the SoC mix (Audio, ENET, uSDHC, EIM, QSPI, OCRAM, MMDC, etc) power up reset
 * stretch mix reset width with the optional count of cycles
 */
typedef enum _src_mix_reset_stretch_cycles
{
    kSRC_MixResetStretchCycleAlt0 = 0U, /*!< mix reset width is 1 x 88 ipg_cycle cycles. */
    kSRC_MixResetStretchCycleAlt1 = 1U, /*!< mix reset width is 2 x 88 ipg_cycle cycles. */
    kSRC_MixResetStretchCycleAlt2 = 2U, /*!< mix reset width is 3 x 88 ipg_cycle cycles. */
    kSRC_MixResetStretchCycleAlt3 = 3U, /*!< mix reset width is 4 x 88 ipg_cycle cycles. */
} src_mix_reset_stretch_cycles_t;
#endif /* FSL_FEATURE_SRC_HAS_SCR_MIX_RST_STRCH */

#if (defined(FSL_FEATURE_SRC_HAS_SCR_WDOG3_RST_OPTN) && FSL_FEATURE_SRC_HAS_SCR_WDOG3_RST_OPTN)
/*!
 * @brief Selection of WDOG3 reset option.
 */
typedef enum _src_wdog3_reset_option
{
    kSRC_Wdog3ResetOptionAlt0 = 0U, /*!< Wdog3_rst_b asserts M4 reset (default). */
    kSRC_Wdog3ResetOptionAlt1 = 1U, /*!< Wdog3_rst_b asserts global reset. */
} src_wdog3_reset_option_t;
#endif /* FSL_FEATURE_SRC_HAS_SCR_WDOG3_RST_OPTN */

/*!
 * @brief Selection of WARM reset bypass count.
 *
 * This type defines the  32KHz clock cycles to count before bypassing the MMDC acknowledge for WARM
 * reset. If the MMDC acknowledge is not asserted before this counter is elapsed, a COLD reset will
 * be initiated.
 */
typedef enum _src_warm_reset_bypass_count
{
    kSRC_WarmResetWaitAlways = 0U, /*!< System will wait until MMDC acknowledge is asserted. */
    kSRC_WarmResetWaitClk16  = 1U, /*!< Wait 16 32KHz clock cycles before switching the reset. */
    kSRC_WarmResetWaitClk32  = 2U, /*!< Wait 32 32KHz clock cycles before switching the reset. */
    kSRC_WarmResetWaitClk64  = 3U, /*!< Wait 64 32KHz clock cycles before switching the reset. */
} src_warm_reset_bypass_count_t;

#if defined(__cplusplus)
extern "C" {
#endif

/*******************************************************************************
 * API
 ******************************************************************************/

#if (defined(FSL_FEATURE_SRC_HAS_SCR_MASK_WDOG3_RST) && FSL_FEATURE_SRC_HAS_SCR_MASK_WDOG3_RST)
/*!
 * @brief Enable the WDOG3 reset.
 *
 * The WDOG3 reset is enabled by default.
 *
 * @param base SRC peripheral base address.
 * @param enable Enable the reset or not.
 */
static inline void SRC_EnableWDOG3Reset(SRC_Type *base, bool enable)
{
    if (enable)
    {
        base->SCR = (base->SCR & ~SRC_SCR_MASK_WDOG3_RST_MASK) | SRC_SCR_MASK_WDOG3_RST(0xA);
    }
    else
    {
        base->SCR = (base->SCR & ~SRC_SCR_MASK_WDOG3_RST_MASK) | SRC_SCR_MASK_WDOG3_RST(0x5);
    }
}
#endif /* FSL_FEATURE_SRC_HAS_SCR_MASK_WDOG3_RST */

#if (defined(FSL_FEATURE_SRC_HAS_SCR_MIX_RST_STRCH) && FSL_FEATURE_SRC_HAS_SCR_MIX_RST_STRCH)
/*!
 * @brief Set the mix power up reset stretch mix reset width.
 *
 * @param base SRC peripheral base address.
 * @param option Setting option, see to #src_mix_reset_stretch_cycles_t.
 */
static inline void SRC_SetMixResetStretchCycles(SRC_Type *base, src_mix_reset_stretch_cycles_t option)
{
    base->SCR = (base->SCR & ~SRC_SCR_MIX_RST_STRCH_MASK) | SRC_SCR_MIX_RST_STRCH(option);
}
#endif /* FSL_FEATURE_SRC_HAS_SCR_MIX_RST_STRCH */

#if (defined(FSL_FEATURE_SRC_HAS_SCR_DBG_RST_MSK_PG) && FSL_FEATURE_SRC_HAS_SCR_DBG_RST_MSK_PG)
/*!
 * @brief Debug reset would be asserted after power gating event.
 *
 * @param base SRC peripheral base address.
 * @param enable Enable the reset or not.
 */
static inline void SRC_EnableCoreDebugResetAfterPowerGate(SRC_Type *base, bool enable)
{
    if (enable)
    {
        base->SCR &= ~SRC_SCR_DBG_RST_MSK_PG_MASK;
    }
    else
    {
        base->SCR |= SRC_SCR_DBG_RST_MSK_PG_MASK;
    }
}
#endif /* FSL_FEATURE_SRC_HAS_SCR_DBG_RST_MSK_PG */

#if (defined(FSL_FEATURE_SRC_HAS_SCR_WDOG3_RST_OPTN) && FSL_FEATURE_SRC_HAS_SCR_WDOG3_RST_OPTN)
/*!
 * @brief Set the Wdog3_rst_b option.
 *
 * @param base SRC peripheral base address.
 * @param option Setting option, see to #src_wdog3_reset_option_t.
 */
static inline void SRC_SetWdog3ResetOption(SRC_Type *base, src_wdog3_reset_option_t option)
{
    base->SCR = (base->SCR & ~SRC_SCR_WDOG3_RST_OPTN_MASK) | SRC_SCR_WDOG3_RST_OPTN(option);
}
#endif /* FSL_FEATURE_SRC_HAS_SCR_WDOG3_RST_OPTN */

#if (defined(FSL_FEATURE_SRC_HAS_SCR_CORES_DBG_RST) && FSL_FEATURE_SRC_HAS_SCR_CORES_DBG_RST)
/*!
 * @brief Software reset for debug of arm platform only.
 *
 * @param base SRC peripheral base address.
 */
static inline void SRC_DoSoftwareResetARMCoreDebug(SRC_Type *base)
{
    base->SCR |= SRC_SCR_CORES_DBG_RST_MASK;
}

/*!
 * @brief Check if the software reset for debug of arm platform only is done.
 *
 * @param base SRC peripheral base address.
 */
static inline bool SRC_GetSoftwareResetARMCoreDebugDone(SRC_Type *base)
{
    return (0U == (base->SCR & SRC_SCR_CORES_DBG_RST_MASK));
}
#endif /* FSL_FEATURE_SRC_HAS_SCR_CORES_DBG_RST */

#if (defined(FSL_FEATURE_SRC_HAS_SCR_MTSR) && FSL_FEATURE_SRC_HAS_SCR_MTSR)
/*!
 * @brief Enable the temperature sensor reset.
 *
 * The temperature sersor reset is enabled by default. When the sensor reset happens, an flag bit
 * would be asserted. This flag bit can be cleared only by the hardware reset.
 *
 * @param base SRC peripheral base address.
 * @param enable Enable the reset or not.
 */
static inline void SRC_EnableTemperatureSensorReset(SRC_Type *base, bool enable)
{
    if (enable) /* Temperature sensor reset is not masked. (default) */
    {
        base->SCR = (base->SCR & ~SRC_SCR_MTSR_MASK) | SRC_SCR_MTSR(0x2);
    }
    else /* The on-chip temperature sensor interrupt will not create a reset to the chip. */
    {
        base->SCR = (base->SCR & ~SRC_SCR_MTSR_MASK) | SRC_SCR_MTSR(0x5);
    }
}
#endif /* FSL_FEATURE_SRC_HAS_SCR_MTSR */

#if (defined(FSL_FEATURE_SCR_HAS_SCR_CORE0_DBG_RST) && FSL_FEATURE_SCR_HAS_SCR_CORE0_DBG_RST)
/*!
 * @brief Do assert the core0 debug reset.
 *
 * @param base SRC peripheral base address.
 */
static inline void SRC_DoAssertCore0DebugReset(SRC_Type *base)
{
    base->SCR |= SRC_SCR_CORE0_DBG_RST_MASK;
}

/*!
 * @brief Check if the core0 debug reset is done.
 *
 * @param base SRC peripheral base address.
 */
static inline bool SRC_GetAssertCore0DebugResetDone(SRC_Type *base)
{
    return (0U == (base->SCR & SRC_SCR_CORE0_DBG_RST_MASK));
}
#endif /* FSL_FEATURE_SCR_HAS_SCR_CORE0_DBG_RST */

#if (defined(FSL_FEATURE_SRC_HAS_SCR_CORE0_RST) && FSL_FEATURE_SRC_HAS_SCR_CORE0_RST)
/*!
 * @brief Do software reset the ARM core0 only.
 *
 * @param base SRC peripheral base address.
 */
static inline void SRC_DoSoftwareResetARMCore0(SRC_Type *base)
{
    base->SCR |= SRC_SCR_CORE0_RST_MASK;
}

/*!
 * @brief Check if the software for ARM core0 is done.
 *
 * @param base SRC peripheral base address.
 * @return If the reset is done.
 */
static inline bool SRC_GetSoftwareResetARMCore0Done(SRC_Type *base)
{
    return (0U == (base->SCR & SRC_SCR_CORE0_RST_MASK));
}
#endif /* FSL_FEATURE_SRC_HAS_SCR_CORE0_RST */

#if (defined(FSL_FEATURE_SRC_HAS_SCR_SWRC) && FSL_FEATURE_SRC_HAS_SCR_SWRC)
/*!
 * @brief Do software reset for ARM core.
 *
 * This function can be used to assert the ARM core reset. Once it is called, the reset process will
 * begin. After the reset process is finished, the command bit would be self cleared.
 *
 * @param base SRC peripheral base address.
 */
static inline void SRC_DoSoftwareResetARMCore(SRC_Type *base)
{
    base->SCR |= SRC_SCR_SWRC_MASK;
}

/*!
 * @brief Check if the software for ARM core is done.
 *
 * @param base SRC peripheral base address.
 * @return If the reset is done.
 */
static inline bool SRC_GetSoftwareResetARMCoreDone(SRC_Type *base)
{
    return (0U == (base->SCR & SRC_SCR_SWRC_MASK));
}
#endif /* FSL_FEATURE_SRC_HAS_SCR_SWRC */

#if (defined(FSL_FEATURE_SRC_HAS_SCR_EIM_RST) && FSL_FEATURE_SRC_HAS_SCR_EIM_RST)
/*!
 * @brief Assert the EIM reset.
 *
 * EIM reset is needed in order to reconfigure the EIM chip select.
 * The software reset bit must de-asserted since this is not self-refresh.
 *
 * @param base SRC peripheral base address.
 * @param enable Make the assertion or not.
 */
static inline void SRC_AssertEIMReset(SRC_Type *base, bool enable)
{
    if (enable)
    {
        base->SCR |= SRC_SCR_EIM_RST_MASK;
    }
    else
    {
        base->SCR &= ~SRC_SCR_EIM_RST_MASK;
    }
}
#endif /* FSL_FEATURE_SRC_HAS_SCR_EIM_RST */

/*!
 * @brief Enable the WDOG Reset in SRC.
 *
 * WDOG Reset is enabled in SRC by default. If the WDOG event to SRC is masked, it would not create
 * a reset to the chip. During the time the WDOG event is masked, when the WDOG event flag is
 * asserted, it would remain asserted regardless of servicing the WDOG module. The only way to clear
 * that bit is the hardware reset.
 *
 * @param base SRC peripheral base address.
 * @param enable Enable the reset or not.
 */
static inline void SRC_EnableWDOGReset(SRC_Type *base, bool enable)
{
    if (enable) /* WDOG Reset is not masked in SRC (default). */
    {
        base->SCR = (base->SCR & ~SRC_SCR_MWDR_MASK) | SRC_SCR_MWDR(0xA);
    }
    else /* WDOG Reset is masked in SRC. */
    {
        base->SCR = (base->SCR & ~SRC_SCR_MWDR_MASK) | SRC_SCR_MWDR(0x5);
    }
}

#if !(defined(FSL_FEATURE_SRC_HAS_NO_SCR_WRBC) && FSL_FEATURE_SRC_HAS_NO_SCR_WRBC)
/*!
 * @brief Set the delay count of waiting MMDC's acknowledge.
 *
 * This function would define the 32KHz clock cycles to count before bypassing the MMDC acknowledge
 * for WARM reset. If the MMDC acknowledge is not asserted before this counter is elapsed, a COLD
 * reset will be initiated.
 *
 * @param base SRC peripheral base address.
 * @param option The option of setting mode, see to #src_warm_reset_bypass_count_t.
 */
static inline void SRC_SetWarmResetBypassCount(SRC_Type *base, src_warm_reset_bypass_count_t option)
{
    base->SCR = (base->SCR & ~SRC_SCR_WRBC_MASK) | SRC_SCR_WRBC(option);
}
#endif /* FSL_FEATURE_SRC_HAS_NO_SCR_WRBC */

#if (defined(FSL_FEATURE_SRC_HAS_SCR_LOCKUP_RST) && FSL_FEATURE_SRC_HAS_SCR_LOCKUP_RST)
/*!
 * @brief Enable the lockup reset.
 *
 * @param base SRC peripheral base address.
 * @param enable Enable the reset or not.
 */
static inline void SRC_EnableLockupReset(SRC_Type *base, bool enable)
{
    if (enable) /* Enable lockup reset. */
    {
        base->SCR |= SRC_SCR_LOCKUP_RST_MASK;
    }
    else /* Disable lockup reset. */
    {
        base->SCR &= ~SRC_SCR_LOCKUP_RST_MASK;
    }
}
#endif /* FSL_FEATURE_SRC_HAS_SCR_LOCKUP_RST */

#if (defined(FSL_FEATURE_SRC_HAS_SCR_LUEN) && FSL_FEATURE_SRC_HAS_SCR_LUEN)
/*!
 * @brief Enable the core lockup reset.
 *
 * When enable the core luckup reset, the system would be reset when core luckup event happens.
 *
 * @param base SRC peripheral base address.
 * @param enable Enable the reset or not.
 */
static inline void SRC_EnableCoreLockupReset(SRC_Type *base, bool enable)
{
    if (enable) /* Core lockup will cause system reset. */
    {
        base->SCR |= SRC_SCR_LUEN_MASK;
    }
    else /* Core lockup will not cause system reset. */
    {
        base->SCR &= ~SRC_SCR_LUEN_MASK;
    }
}
#endif /* FSL_FEATURE_SRC_HAS_SCR_LUEN */

#if !(defined(FSL_FEATURE_SRC_HAS_NO_SCR_WRE) && FSL_FEATURE_SRC_HAS_NO_SCR_WRE)
/*!
 * @brief Enable the WARM reset.
 *
 * Only when the WARM reset is enabled, the WARM reset requests would be served by WARM reset.
 * Otherwise, all the WARM reset sources would generate COLD reset.
 *
 * @param base SRC peripheral base address.
 * @param enable Enable the WARM reset or not.
 */
static inline void SRC_EnableWarmReset(SRC_Type *base, bool enable)
{
    if (enable)
    {
        base->SCR |= SRC_SCR_WRE_MASK;
    }
    else
    {
        base->SCR &= ~SRC_SCR_WRE_MASK;
    }
}
#endif /* FSL_FEATURE_SRC_HAS_NO_SCR_WRE */

#if (defined(FSL_FEATURE_SRC_HAS_SISR) && FSL_FEATURE_SRC_HAS_SISR)
/*!
 * @brief Get interrupt status flags.
 *
 * @param base SRC peripheral base address.
 * @return Mask value of status flags. See to $_src_status_flags.
 */
static inline uint32_t SRC_GetStatusFlags(SRC_Type *base)
{
    return base->SISR;
}
#endif /* FSL_FEATURE_SRC_HAS_SISR */

/*!
 * @brief Get the boot mode register 1 value.
 *
 * The Boot Mode register contains bits that reflect the status of BOOT_CFGx pins of the chip.
 * See to chip-specific document for detail information about value.
 *
 * @param base SRC peripheral base address.
 * @return status of BOOT_CFGx pins of the chip.
 */
static inline uint32_t SRC_GetBootModeWord1(SRC_Type *base)
{
    return base->SBMR1;
}

/*!
 * @brief Get the boot mode register 2 value.
 *
 * The Boot Mode register contains bits that reflect the status of BOOT_MODEx Pins and fuse values
 * that controls boot of the chip. See to chip-specific document for detail information about value.
 *
 * @param base SRC peripheral base address.
 * @return status of BOOT_MODEx Pins and fuse values that controls boot of the chip.
 */
static inline uint32_t SRC_GetBootModeWord2(SRC_Type *base)
{
    return base->SBMR2;
}

#if !(defined(FSL_FEATURE_SRC_HAS_NO_SRSR_WBI) && FSL_FEATURE_SRC_HAS_NO_SRSR_WBI)
/*!
 * @brief Set the warm boot indication flag.
 *
 * WARM boot indication shows that WARM boot was initiated by software. This indicates to the
 * software that it saved the needed information in the memory before initiating the WARM reset.
 * In this case, software will set this bit to '1', before initiating the WARM reset. The warm_boot
 * bit should be used as indication only after a warm_reset sequence. Software should clear this bit
 * after warm_reset to indicate that the next warm_reset is not performed with warm_boot.
 *
 * @param base SRC peripheral base address.
 * @param enable Assert the flag or not.
 */
static inline void SRC_SetWarmBootIndication(SRC_Type *base, bool enable)
{
    if (enable)
    {
        base->SRSR = (base->SRSR & ~SRC_SRSR_W1C_BITS_MASK) | SRC_SRSR_WBI_MASK;
    }
    else
    {
        base->SRSR = (base->SRSR & ~SRC_SRSR_W1C_BITS_MASK) & ~SRC_SRSR_WBI_MASK;
    }
}
#endif /* FSL_FEATURE_SRC_HAS_NO_SRSR_WBI */

/*!
 * @brief Get the status flags of SRC.
 *
 * @param base SRC peripheral base address.
 * @return Mask value of status flags, see to #_src_reset_status_flags.
 */
static inline uint32_t SRC_GetResetStatusFlags(SRC_Type *base)
{
    return base->SRSR;
}

/*!
 * @brief Clear the status flags of SRC.
 *
 * @param base SRC peripheral base address.
 * @param Mask value of status flags to be cleared, see to #_src_reset_status_flags.
 */
void SRC_ClearResetStatusFlags(SRC_Type *base, uint32_t flags);

/*!
 * @brief Set value to general purpose registers.
 *
 * General purpose registers (GPRx) would hold the value during reset process. Wakeup function could
 * be kept in these register. For example, the GPR1 holds the entry function for waking-up from
 * Partial SLEEP mode while the GPR2 holds the argument. Other GPRx register would store the
 * arbitray values.
 *
 * @param base SRC peripheral base address.
 * @param index The index of GPRx register array. Note index 0 reponses the GPR1 register.
 * @param value Setting value for GPRx register.
 */
static inline void SRC_SetGeneralPurposeRegister(SRC_Type *base, uint32_t index, uint32_t value)
{
    assert(index < SRC_GPR_COUNT);

    base->GPR[index] = value;
}

/*!
 * @brief Get the value from general purpose registers.
 *
 * @param base SRC peripheral base address.
 * @param index The index of GPRx register array. Note index 0 reponses the GPR1 register.
 * @return The setting value for GPRx register.
 */
static inline uint32_t SRC_GetGeneralPurposeRegister(SRC_Type *base, uint32_t index)
{
    assert(index < SRC_GPR_COUNT);

    return base->GPR[index];
}

#if defined(__cplusplus)
}
#endif
/*!
 * @}
 */
#endif /* _FSL_SRC_H_ */
