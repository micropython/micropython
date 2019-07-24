/*
 * Copyright 2017 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_FLEXRAM_H_
#define _FSL_FLEXRAM_H_

#include "fsl_common.h"

/*!
 * @addtogroup flexram
 * @{
 */

/******************************************************************************
 * Definitions.
 *****************************************************************************/

/*! @name Driver version */
/*@{*/
/*! @brief Driver version 2.0.4. */
#define FSL_FLEXRAM_DRIVER_VERSION (MAKE_VERSION(2U, 0U, 4U))
/*@}*/

/*! @brief flexram write read sel */
enum _flexram_wr_rd_sel
{
    kFLEXRAM_Read  = 0U, /*!< read */
    kFLEXRAM_Write = 1U, /*!< write */
};

/*! @brief Interrupt status flag mask */
enum _flexram_interrupt_status
{
    kFLEXRAM_OCRAMAccessError = FLEXRAM_INT_STATUS_OCRAM_ERR_STATUS_MASK, /*!< ocram access unallocated address */
    kFLEXRAM_DTCMAccessError  = FLEXRAM_INT_STATUS_DTCM_ERR_STATUS_MASK,  /*!< dtcm access unallocated address */
    kFLEXRAM_ITCMAccessError  = FLEXRAM_INT_STATUS_ITCM_ERR_STATUS_MASK,  /*!< itcm access unallocated address */

    kFLEXRAM_InterruptStatusAll = FLEXRAM_INT_STATUS_OCRAM_ERR_STATUS_MASK | FLEXRAM_INT_STATUS_DTCM_ERR_STATUS_MASK |
                                  FLEXRAM_INT_STATUS_ITCM_ERR_STATUS_MASK, /*!< all the interrupt status mask */
};

/*! @brief FLEXRAM TCM access mode
 * Fast access mode expected to be finished in 1-cycle
 * Wait access mode expected to be finished in 2-cycle
 * Wait access mode is a feature of the flexram and it should be used when
 * the cpu clock too fast to finish tcm access in 1-cycle.
 * Normally, fast mode is the default mode, the efficiency of the tcm access will better.
 */
typedef enum _flexram_tcm_access_mode
{
    kFLEXRAM_TCMAccessFastMode = 0U, /*!< fast access mode */
    kFLEXRAM_TCMAccessWaitMode = 1U, /*!< wait access mode */
} flexram_tcm_access_mode_t;

/*! @brief FLEXRAM bank type */
enum _flexram_bank_type
{
    kFLEXRAM_BankNotUsed = 0U, /*!< bank is not used */
    kFLEXRAM_BankOCRAM   = 1U, /*!< bank is OCRAM */
    kFLEXRAM_BankDTCM    = 2U, /*!< bank is DTCM */
    kFLEXRAM_BankITCM    = 3U, /*!< bank is ITCM */
};

/*! @brief FLEXRAM tcm support size */
enum _flexram_tcm_size
{
    kFLEXRAM_TCMSize32KB  = 32 * 1024U,  /*!< TCM total size 32KB */
    kFLEXRAM_TCMSize64KB  = 64 * 1024U,  /*!< TCM total size 64KB */
    kFLEXRAM_TCMSize128KB = 128 * 1024U, /*!< TCM total size 128KB */
    kFLEXRAM_TCMSize256KB = 256 * 1024U, /*!< TCM total size 256KB */
    kFLEXRAM_TCMSize512KB = 512 * 1024U, /*!< TCM total size 512KB */
};

/*! @brief FLEXRAM bank allocate source */
typedef enum _flexram_bank_allocate_src
{
    kFLEXRAM_BankAllocateThroughHardwareFuse = 0U, /*!< allocate ram through hardware fuse value */
    kFLEXRAM_BankAllocateThroughBankCfg      = 1U, /*!< allocate ram through FLEXRAM_BANK_CFG */
} flexram_bank_allocate_src_t;

/*! @brief FLEXRAM allocate ocram, itcm, dtcm size */
typedef struct _flexram_allocate_ram
{
    const uint8_t ocramBankNum; /*!< ocram banknumber which the SOC support */
    const uint8_t dtcmBankNum;  /*!< dtcm bank number to allocate, the number should be power of 2 */
    const uint8_t itcmBankNum;  /*!< itcm bank number to allocate, the number should be power of 2 */
} flexram_allocate_ram_t;

/*******************************************************************************
 * APIs
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name Initialization and deinitialization
 * @{
 */

/*!
 * @brief FLEXRAM module initialization function.
 *
 * @param base FLEXRAM base address.
 */
void FLEXRAM_Init(FLEXRAM_Type *base);

/*!
 * @brief Deinitializes the FLEXRAM.
 *
 */
void FLEXRAN_Deinit(FLEXRAM_Type *base);

/* @} */

/*!
 * @name Status
 * @{
 */
/*!
 * @brief FLEXRAM module get interrupt status.
 *
 * @param base FLEXRAM base address.
 */
static inline uint32_t FLEXRAM_GetInterruptStatus(FLEXRAM_Type *base)
{
    return base->INT_STATUS & kFLEXRAM_InterruptStatusAll;
}

/*!
 * @brief FLEXRAM module clear interrupt status.
 *
 * @param base FLEXRAM base address.
 * @param status status to clear.
 */
static inline void FLEXRAM_ClearInterruptStatus(FLEXRAM_Type *base, uint32_t status)
{
    base->INT_STATUS |= status;
}

/*!
 * @brief FLEXRAM module enable interrupt status.
 *
 * @param base FLEXRAM base address.
 * @param status status to enable.
 */
static inline void FLEXRAM_EnableInterruptStatus(FLEXRAM_Type *base, uint32_t status)
{
    base->INT_STAT_EN |= status;
}

/*!
 * @brief FLEXRAM module disable interrupt status.
 *
 * @param base FLEXRAM base address.
 * @param status status to disable.
 */
static inline void FLEXRAM_DisableInterruptStatus(FLEXRAM_Type *base, uint32_t status)
{
    base->INT_STAT_EN &= ~status;
}

/* @} */

/*!
 * @name Interrupts
 * @{
 */

/*!
 * @brief FLEXRAM module enable interrupt.
 *
 * @param base FLEXRAM base address.
 * @param status status interrupt to enable.
 */
static inline void FLEXRAM_EnableInterruptSignal(FLEXRAM_Type *base, uint32_t status)
{
    base->INT_SIG_EN |= status;
}

/*!
 * @brief FLEXRAM module disable interrupt.
 *
 * @param base FLEXRAM base address.
 * @param status status interrupt to disable.
 */
static inline void FLEXRAM_DisableInterruptSignal(FLEXRAM_Type *base, uint32_t status)
{
    base->INT_SIG_EN &= ~status;
}
/* @} */

/*!
 * @name functional
 * @{
 */

/*!
 * @brief FLEXRAM module set TCM read access mode
 *
 * @param base  FLEXRAM base address.
 * @param mode  access mode.
 */
static inline void FLEXRAM_SetTCMReadAccessMode(FLEXRAM_Type *base, flexram_tcm_access_mode_t mode)
{
    base->TCM_CTRL &= ~FLEXRAM_TCM_CTRL_TCM_RWAIT_EN_MASK;
    base->TCM_CTRL |= mode;
}

/*!
 * @brief FLEXRAM module set TCM write access mode
 *
 * @param base  FLEXRAM base address.
 * @param mode  access mode.
 */
static inline void FLEXRAM_SetTCMWriteAccessMode(FLEXRAM_Type *base, flexram_tcm_access_mode_t mode)
{
    base->TCM_CTRL &= ~FLEXRAM_TCM_CTRL_TCM_WWAIT_EN_MASK;
    base->TCM_CTRL |= mode;
}

/*!
 * @brief FLEXRAM module force ram clock on
 *
 * @param base  FLEXRAM base address.
 * @param enable enable or disable clock force on.
 */
static inline void FLEXRAM_EnableForceRamClockOn(FLEXRAM_Type *base, bool enable)
{
    if (enable)
    {
        base->TCM_CTRL |= FLEXRAM_TCM_CTRL_FORCE_CLK_ON_MASK;
    }
    else
    {
        base->TCM_CTRL &= ~FLEXRAM_TCM_CTRL_FORCE_CLK_ON_MASK;
    }
}

/*!
 * @brief FLEXRAM allocate on-chip ram for OCRAM,ITCM,DTCM
 * This function is independent of FLEXRAM_Init, it can be called directly if ram re-allocate
 * is needed.
 * @param config allocate configuration.
 * @retval kStatus_InvalidArgument the argument is invalid
 * 		   kStatus_Success allocate success
 */
status_t FLEXRAM_AllocateRam(flexram_allocate_ram_t *config);

/*!
 * @brief FLEXRAM set allocate on-chip ram source
 * @param src bank config source select value.
 */
static inline void FLEXRAM_SetAllocateRamSrc(flexram_bank_allocate_src_t src)
{
    IOMUXC_GPR->GPR16 &= ~IOMUXC_GPR_GPR16_FLEXRAM_BANK_CFG_SEL_MASK;
    IOMUXC_GPR->GPR16 |= IOMUXC_GPR_GPR16_FLEXRAM_BANK_CFG_SEL(src);
}

/*!
 * @brief FLEXRAM configure TCM size
 * This function  is used to set the TCM to the target size. If a odd bank number is used,
 * a new banknumber will be used which is bigger than target value, application can set tcm
 * size to the biggest bank number always, then boundary access error can be captured by flexram only.
 * When access to the TCM memory boundary ,hardfault will raised by core.
 * @param itcmBankNum itcm bank number to allocate
 * @param dtcmBankNum dtcm bank number to allocate
 *
 */
void FLEXRAM_SetTCMSize(uint8_t itcmBankNum, uint8_t dtcmBankNum);

#if defined(__cplusplus)
}
#endif

/*! @}*/

#endif
