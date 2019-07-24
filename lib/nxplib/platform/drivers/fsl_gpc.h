/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_GPC_H_
#define _FSL_GPC_H_

#include "fsl_common.h"

/*!
 * @addtogroup gpc
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
/*@{*/
/*! @brief GPC driver version 2.1.0. */
#define FSL_GPC_DRIVER_VERSION (MAKE_VERSION(2, 1, 0))
/*@}*/

#if defined(__cplusplus)
extern "C" {
#endif

/*******************************************************************************
 * API
 ******************************************************************************/

#if (defined(FSL_FEATURE_GPC_HAS_CNTR_GPCIRQM) && FSL_FEATURE_GPC_HAS_CNTR_GPCIRQM)
/*!
 * @brief Allow all the IRQ/Events within the charge of GPC.
 *
 * @param base GPC peripheral base address.
 */
static inline void GPC_AllowIRQs(GPC_Type *base)
{
    base->CNTR &= ~GPC_CNTR_GPCIRQM_MASK; /* Events would not be masked. */
}

/*!
 * @brief Disallow all the IRQ/Events within the charge of GPC.
 *
 * @param base GPC peripheral base address.
 */
static inline void GPC_DisallowIRQs(GPC_Type *base)
{
    base->CNTR |= GPC_CNTR_GPCIRQM_MASK; /* Mask all the events. */
}
#endif /* FSL_FEATURE_GPC_HAS_CNTR_GPCIRQM */

/*!
 * @brief Enable the IRQ.
 *
 * @param base GPC peripheral base address.
 * @param irqId ID number of IRQ to be enabled, available range is 32-159.
 */
void GPC_EnableIRQ(GPC_Type *base, uint32_t irqId);

/*!
 * @brief Disable the IRQ.
 *
 * @param base GPC peripheral base address.
 * @param irqId ID number of IRQ to be disabled, available range is 32-159.
 */
void GPC_DisableIRQ(GPC_Type *base, uint32_t irqId);

/*!
 * @brief Get the IRQ/Event flag.
 *
 * @param base GPC peripheral base address.
 * @param irqId ID number of IRQ to be enabled, available range is 32-159.
 * @return Indicated IRQ/Event is asserted or not.
 */
bool GPC_GetIRQStatusFlag(GPC_Type *base, uint32_t irqId);

#if (defined(FSL_FEATURE_GPC_HAS_CNTR_L2PGE) && FSL_FEATURE_GPC_HAS_CNTR_L2PGE)
/*!
 * @brief L2 Cache Power Gate Enable
 *
 * This function configures the L2 cache if it will keep power when in low power mode.
 * When the L2 cache power is OFF, L2 cache will be power down once when CPU core is power down
 * and will be hardware invalidated automatically when CPU core is re-power up.
 * When the L2 cache power is ON, L2 cache will keep power on even if CPU core is power down and
 * will not be hardware invalidated.
 * When CPU core is re-power up, the default setting is OFF.
 *
 * @param base GPC peripheral base address.
 * @param enable Enable the request or not.
 */
static inline void GPC_RequestL2CachePowerDown(GPC_Type *base, bool enable)
{
    if (enable)
    {
        base->CNTR |= GPC_CNTR_L2_PGE_MASK; /* OFF. */
    }
    else
    {
        base->CNTR &= ~GPC_CNTR_L2_PGE_MASK; /* ON. */
    }
}
#endif /* FSL_FEATURE_GPC_HAS_CNTR_L2PGE */

#if (defined(FSL_FEATURE_GPC_HAS_CNTR_PDRAM0PGE) && FSL_FEATURE_GPC_HAS_CNTR_PDRAM0PGE)
/*!
 * @brief FLEXRAM PDRAM0 Power Gate Enable
 *
 * This function configures the FLEXRAM PDRAM0 if it will keep power when cpu core is power down.
 * When the PDRAM0 Power is 1, PDRAM0 will be power down once when CPU core is power down.
 * When the PDRAM0 Power is 0, PDRAM0 will keep power on even if CPU core is power down.
 * When CPU core is re-power up, the default setting is 1.
 *
 * @param base GPC peripheral base address.
 * @param enable Enable the request or not.
 */
static inline void GPC_RequestPdram0PowerDown(GPC_Type *base, bool enable)
{
    if (enable)
    {
        base->CNTR |= GPC_CNTR_PDRAM0_PGE_MASK; /* OFF. */
    }
    else
    {
        base->CNTR &= ~GPC_CNTR_PDRAM0_PGE_MASK; /* ON. */
    }
}
#endif /* FSL_FEATURE_GPC_HAS_CNTR_PDRAM0PGE */

#if (defined(FSL_FEATURE_GPC_HAS_CNTR_VADC) && FSL_FEATURE_GPC_HAS_CNTR_VADC)
/*!
 * @brief VADC power down.
 *
 * This function requests the VADC power down.
 *
 * @param base GPC peripheral base address.
 * @param enable Enable the request or not.
 */
static inline void GPC_RequestVADCPowerDown(GPC_Type *base, bool enable)
{
    if (enable)
    {
        base->CNTR &= ~GPC_CNTR_VADC_EXT_PWD_N_MASK; /* VADC power down. */
    }
    else
    {
        base->CNTR |= GPC_CNTR_VADC_EXT_PWD_N_MASK; /* VADC not power down. */
    }
}

/*!
 * @brief Checks if the VADC is power off.
 *
 * @param base GPC peripheral base address.
 * @return Whether the VADC is power off or not.
 */
static inline bool GPC_GetVADCPowerDownFlag(GPC_Type *base)
{
    return (GPC_CNTR_VADC_ANALOG_OFF_MASK == (GPC_CNTR_VADC_ANALOG_OFF_MASK & base->CNTR));
}
#endif /* FSL_FEATURE_GPC_HAS_CNTR_VADC */

#if (defined(FSL_FEATURE_GPC_HAS_CNTR_DVFS0CR) && FSL_FEATURE_GPC_HAS_CNTR_DVFS0CR)
/*!
 * @brief Checks if the DVFS0 is requesting for frequency/voltage update.
 *
 * @param base GPC peripheral base address.
 * @return Whether the DVFS0 is requesting for frequency/voltage update.
 */
static inline bool GPC_HasDVFS0ChangeRequest(GPC_Type *base)
{
    return (GPC_CNTR_DVFS0CR_MASK == (GPC_CNTR_DVFS0CR_MASK & base->CNTR));
}
#endif /* FSL_FEATURE_GPC_HAS_CNTR_DVFS0CR */

#if (defined(FSL_FEATURE_GPC_HAS_CNTR_DISPLAY) && FSL_FEATURE_GPC_HAS_CNTR_DISPLAY)
/*!
 * @brief Requests the display power switch sequence.
 *
 * @param base GPC peripheral base address.
 * @param enable Enable the power on sequence, or the power down sequence.
 */
static inline void GPC_RequestDisplayPowerOn(GPC_Type *base, bool enable)
{
    if (enable)
    {
        base->CNTR |= GPC_CNTR_DISPLAY_PUP_REQ_MASK; /* Power on sequence. */
    }
    else
    {
        base->CNTR |= GPC_CNTR_DISPLAY_PDN_REQ_MASK; /* Power down sequence. */
    }
}
#endif /* FSL_FEATURE_GPC_HAS_CNTR_DISPLAY */

/*!
 * @brief Requests the MEGA power switch sequence.
 *
 * @param base GPC peripheral base address.
 * @param enable Enable the power on sequence, or the power down sequence.
 */
static inline void GPC_RequestMEGAPowerOn(GPC_Type *base, bool enable)
{
    if (enable)
    {
        base->CNTR |= GPC_CNTR_MEGA_PUP_REQ_MASK; /* Power on sequence. */
    }
    else
    {
        base->CNTR |= GPC_CNTR_MEGA_PDN_REQ_MASK; /* Power down sequence. */
    }
}

/*!
 * @}
 */

#if defined(__cplusplus)
}
#endif
/*!
 * @}
 */
#endif /* _FSL_GPC_H_ */
