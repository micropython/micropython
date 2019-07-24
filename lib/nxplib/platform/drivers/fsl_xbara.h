/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_XBARA_H_
#define _FSL_XBARA_H_

#include "fsl_common.h"

/*!
 * @addtogroup xbara
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define FSL_XBARA_DRIVER_VERSION (MAKE_VERSION(2, 0, 4)) /*!< Version 2.0.4. */

/* Macros for entire XBARA_SELx register.  */
#define XBARA_SELx(base, output) (*(volatile uint16_t *)((uintptr_t) & (base->SEL0) + ((output) / 2U) * 2U))
/* Set the XBARA_SELx_SELx field to a new value. */
#define XBARA_WR_SELx_SELx(base, input, output)                                                    \
    (XBARA_SELx((base), (output)) =                                                                \
         ((XBARA_SELx((base), (output)) & ~(0xFFU << (XBARA_SEL0_SEL1_SHIFT * ((output) % 2U)))) | \
          ((input) << (XBARA_SEL0_SEL1_SHIFT * ((output) % 2U)))))

/*!
 * @brief XBARA active edge for detection
 */
typedef enum _xbara_active_edge
{
    kXBARA_EdgeNone             = 0U, /*!< Edge detection status bit never asserts. */
    kXBARA_EdgeRising           = 1U, /*!< Edge detection status bit asserts on rising edges. */
    kXBARA_EdgeFalling          = 2U, /*!< Edge detection status bit asserts on falling edges. */
    kXBARA_EdgeRisingAndFalling = 3U  /*!< Edge detection status bit asserts on rising and falling edges. */
} xbara_active_edge_t;

/*!
 * @brief Defines the XBARA DMA and interrupt configurations.
 */
typedef enum _xbar_request
{
    kXBARA_RequestDisable         = 0U, /*!< Interrupt and DMA are disabled. */
    kXBARA_RequestDMAEnable       = 1U, /*!< DMA enabled, interrupt disabled. */
    kXBARA_RequestInterruptEnalbe = 2U  /*!< Interrupt enabled, DMA disabled. */
} xbara_request_t;

/*!
 * @brief XBARA status flags.
 *
 * This provides constants for the XBARA status flags for use in the XBARA functions.
 */
typedef enum _xbara_status_flag_t
{
    kXBARA_EdgeDetectionOut0 =
        (XBARA_CTRL0_STS0_MASK), /*!< XBAR_OUT0 active edge interrupt flag, sets when active edge detected. */
    kXBARA_EdgeDetectionOut1 =
        (XBARA_CTRL0_STS1_MASK), /*!< XBAR_OUT1 active edge interrupt flag, sets when active edge detected. */
    kXBARA_EdgeDetectionOut2 =
        (XBARA_CTRL1_STS2_MASK << 16U), /*!< XBAR_OUT2 active edge interrupt flag, sets when active edge detected. */
    kXBARA_EdgeDetectionOut3 =
        (XBARA_CTRL1_STS3_MASK << 16U), /*!< XBAR_OUT3 active edge interrupt flag, sets when active edge detected. */
} xbara_status_flag_t;

/*!
 * @brief Defines the configuration structure of the XBARA control register.
 *
 * This structure keeps the configuration of XBARA control register for one output.
 * Control registers are available only for a few outputs. Not every XBARA module has
 * control registers.
 */
typedef struct XBARAControlConfig
{
    xbara_active_edge_t activeEdge; /*!< Active edge to be detected. */
    xbara_request_t requestType;    /*!< Selects DMA/Interrupt request. */
} xbara_control_config_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*!
 * @name XBARA functional Operation.
 * @{
 */

/*!
 * @brief Initializes the XBARA module.
 *
 * This function un-gates the XBARA clock.
 *
 * @param base XBARA peripheral address.
 */
void XBARA_Init(XBARA_Type *base);

/*!
 * @brief Shuts down the XBARA module.
 *
 * This function disables XBARA clock.
 *
 * @param base XBARA peripheral address.
 */
void XBARA_Deinit(XBARA_Type *base);

/*!
 * @brief Sets a connection between the selected XBARA_IN[*] input and the XBARA_OUT[*] output signal.
 *
 * This function connects the XBARA input to the selected XBARA output.
 * If more than one XBARA module is available, only the inputs and outputs from the same module
 * can be connected.
 *
 * Example:
   @code
   XBARA_SetSignalsConnection(XBARA, kXBARA_InputPIT_TRG0, kXBARA_OutputDMAMUX18);
   @endcode
 *
 * @param base XBARA peripheral address.
 * @param input XBARA input signal.
 * @param output XBARA output signal.
 */
void XBARA_SetSignalsConnection(XBARA_Type *base, xbar_input_signal_t input, xbar_output_signal_t output);

/*!
 * @brief Gets the active edge detection status.
 *
 * This function gets the active edge detect status of all XBARA_OUTs. If the
 * active edge occurs, the return value is asserted. When the interrupt or the DMA
 * functionality is enabled for the XBARA_OUTx, this field is 1 when the interrupt
 * or DMA request is asserted and 0 when the interrupt or DMA request has been
 * cleared.
 *
 * @param base XBARA peripheral address.
 * @return the mask of these status flag bits.
 */
uint32_t XBARA_GetStatusFlags(XBARA_Type *base);

/*!
 * @brief Clears the edge detection status flags of relative mask.
 *
 * @param base XBARA peripheral address.
 * @param mask the status flags to clear.
 */
void XBARA_ClearStatusFlags(XBARA_Type *base, uint32_t mask);

/*!
 * @brief Configures the XBARA control register.
 *
 * This function configures an XBARA control register. The active edge detection
 * and the DMA/IRQ function on the corresponding XBARA output can be set.
 *
 * Example:
   @code
   xbara_control_config_t userConfig;
   userConfig.activeEdge = kXBARA_EdgeRising;
   userConfig.requestType = kXBARA_RequestInterruptEnalbe;
   XBARA_SetOutputSignalConfig(XBARA, kXBARA_OutputDMAMUX18, &userConfig);
   @endcode
 *
 * @param base XBARA peripheral address.
 * @param output XBARA output number.
 * @param controlConfig Pointer to structure that keeps configuration of control register.
 */
void XBARA_SetOutputSignalConfig(XBARA_Type *base,
                                 xbar_output_signal_t output,
                                 const xbara_control_config_t *controlConfig);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

/*! @}*/

/*!* @} */

#endif /* _FSL_XBARA_H_ */
