/*
 * Copyright 2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _FSL_KPP_H_
#define _FSL_KPP_H_

#include "fsl_common.h"

/*!
 * @addtogroup kpp
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
/*@{*/
/*! @brief KPP driver version 2.0.0. */
#define FSL_KPP_DRIVER_VERSION (MAKE_VERSION(2, 0, 0))
/*@}*/

#define KPP_KEYPAD_COLUMNNUM_MAX (8U)
#define KPP_KEYPAD_ROWNUM_MAX (8U)

/*! @brief List of interrupts supported by the peripheral. This
 * enumeration uses one-bot encoding to allow a logical OR of multiple
 * members. Members usually map to interrupt enable bits in one or more
 * peripheral registers.
 */
typedef enum _kpp_interrupt_enable
{
    kKPP_keyDepressInterrupt = KPP_KPSR_KDIE_MASK, /*!< Keypad depress interrupt source */
    kKPP_keyReleaseInterrupt = KPP_KPSR_KRIE_MASK  /*!< Keypad release interrupt source */
} kpp_interrupt_enable_t;

/*! @brief Lists of KPP synchronize chain operation. */
typedef enum _kpp_sync_operation
{
    kKPP_ClearKeyDepressSyncChain = KPP_KPSR_KDSC_MASK, /*!< Keypad depress interrupt status. */
    kKPP_SetKeyReleasesSyncChain  = KPP_KPSR_KRSS_MASK, /*!< Keypad release interrupt status. */
} kpp_sync_operation_t;

/*! @brief Lists of KPP status. */
typedef struct _kpp_config
{
    uint8_t activeRow;    /*!< The row number: bit 7 ~ 0 represents the row 7 ~ 0. */
    uint8_t activeColumn; /*!< The column number: bit 7 ~ 0 represents the column 7 ~ 0. */
    uint16_t interrupt;   /*!< KPP interrupt source. A logical OR of "kpp_interrupt_enable_t". */
} kpp_config_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name Initialization and De-initialization
 * @{
 */

/*!
 * @brief KPP initialize.
 * This function ungates the KPP clock and initializes KPP.
 * This function must be called before calling any other KPP driver functions.
 *
 * @param base KPP peripheral base address.
 * @param configure The KPP configuration structure pointer.
 */
void KPP_Init(KPP_Type *base, kpp_config_t *configure);

/*!
 * @brief Deinitializes the KPP module and gates the clock.
 * This function gates the KPP clock. As a result, the KPP
 * module doesn't work after calling this function.
 *
 * @param base KPP peripheral base address.
 */
void KPP_Deinit(KPP_Type *base);

/* @} */

/*!
 * @name KPP Basic Operation
 * @{
 */

/*!
 * @brief Enable the interrupt.
 *
 * @param base KPP peripheral base address.
 * @param mask  KPP interrupts to enable. This is a logical OR of the
 *             enumeration :: kpp_interrupt_enable_t.
 */
static inline void KPP_EnableInterrupts(KPP_Type *base, uint16_t mask)
{
    uint16_t data = base->KPSR & ~(KPP_KPSR_KPKR_MASK | KPP_KPSR_KPKD_MASK);
    base->KPSR    = data | mask;
}

/*!
 * @brief Disable the interrupt.
 *
 * @param base KPP peripheral base address.
 * @param mask  KPP interrupts to disable. This is a logical OR of the
 *             enumeration :: kpp_interrupt_enable_t.
 */
static inline void KPP_DisableInterrupts(KPP_Type *base, uint16_t mask)
{
    base->KPSR &= ~(mask | KPP_KPSR_KPKR_MASK | KPP_KPSR_KPKD_MASK);
}

/*!
 * @brief Gets the KPP interrupt event status.
 *
 * @param base KPP peripheral base address.
 * @return The status of the KPP. Application can use the enum type in the "kpp_interrupt_enable_t"
 * to get the right status of the related event.
 */
static inline uint16_t KPP_GetStatusFlag(KPP_Type *base)
{
    return (base->KPSR & (KPP_KPSR_KPKR_MASK | KPP_KPSR_KPKD_MASK)) << KPP_KPSR_KDIE_SHIFT;
}

/*!
 * @brief Clears KPP status flag.
 *
 * @param base KPP peripheral base address.
 * @param mask KPP mask to be cleared. This is a logical OR of the
 *             enumeration :: kpp_interrupt_enable_t.
 */
static inline void KPP_ClearStatusFlag(KPP_Type *base, uint16_t mask)
{
    base->KPSR |= (uint16_t)((mask) >> KPP_KPSR_KDIE_SHIFT);
}

/*!
 * @brief Set KPP synchronization chain.
 *
 * @param base KPP peripheral base address.
 * @param mask KPP mask to be cleared. This is a logical OR of the
 *             enumeration :: kpp_sync_operation_t.
 */
static inline void KPP_SetSynchronizeChain(KPP_Type *base, uint16_t mask)
{
    uint16_t data = base->KPSR & (KPP_KPSR_KRSS_MASK | KPP_KPSR_KDSC_MASK | KPP_KPSR_KRIE_MASK | KPP_KPSR_KDIE_MASK);
    base->KPSR    = data | mask;
}

/*!
 * @brief Keypad press scanning.
 *
 * This function will scanning all columns and rows. so
 * all scanning data will be stored in the data pointer.
 *
 * @param base  KPP peripheral base address.
 * @param data  KPP key press scanning data. The data buffer should be prepared with
 * length at least equal to KPP_KEYPAD_COLUMNNUM_MAX * KPP_KEYPAD_ROWNUM_MAX.
 * the data pointer is recommended to be a array like uint8_t data[KPP_KEYPAD_COLUMNNUM_MAX].
 * for example the data[2] = 4, that means in column 1 row 2 has a key press event.
 */
void KPP_keyPressScanning(KPP_Type *base, uint8_t *data, uint32_t clockSrc_Hz);

/* @} */

#if defined(__cplusplus)
}
#endif

/*! @}*/

#endif /* _FSL_KPP_H_*/
