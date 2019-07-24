/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_XBARB_H_
#define _FSL_XBARB_H_

#include "fsl_common.h"

/*!
 * @addtogroup xbarb
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define FSL_XBARB_DRIVER_VERSION (MAKE_VERSION(2, 0, 1)) /*!< Version 2.0.1. */

/* Macros for entire XBARB_SELx register. */
#define XBARB_SELx(base, output) (*(volatile uint16_t *)((uintptr_t) & (base->SEL0) + ((output) / 2U) * 2U))
/* Set the SELx field to a new value. */
#define XBARB_WR_SELx_SELx(base, input, output)                                                    \
    (XBARB_SELx((base), (output)) =                                                                \
         ((XBARB_SELx((base), (output)) & ~(0xFFU << (XBARB_SEL0_SEL1_SHIFT * ((output) % 2U)))) | \
          ((input) << (XBARB_SEL0_SEL1_SHIFT * ((output) % 2U)))))

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*!
 * @name XBARB functional Operation.
 * @{
 */

/*!
 * @brief Initializes the XBARB module.
 *
 * This function un-gates the XBARB clock.
 *
 * @param base XBARB peripheral address.
 */
void XBARB_Init(XBARB_Type *base);

/*!
 * @brief Shuts down the XBARB module.
 *
 * This function disables XBARB clock.
 *
 * @param base XBARB peripheral address.
 */
void XBARB_Deinit(XBARB_Type *base);

/*!
 * @brief Configures a connection between the selected XBARB_IN[*] input and the XBARB_OUT[*] output signal.
 *
 * This function configures which XBARB input is connected to the selected XBARB output.
 * If more than one XBARB module is available, only the inputs and outputs from the same module
 * can be connected.
 *
 * @param base XBARB peripheral address.
 * @param input XBARB input signal.
 * @param output XBARB output signal.
 */
void XBARB_SetSignalsConnection(XBARB_Type *base, xbar_input_signal_t input, xbar_output_signal_t output);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

/*! @}*/

/*!* @} */

#endif /* _FSL_XBARB_H_ */
