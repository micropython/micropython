/*
 * The Clear BSD License
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 * that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _CLOCK_CONFIG_H_
#define _CLOCK_CONFIG_H_

#include "fsl_common.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define BOARD_XTAL0_CLK_HZ                         12000000U  /*!< Board xtal0 frequency in Hz */
#define BOARD_XTAL32K_CLK_HZ                          32768U  /*!< Board xtal32K frequency in Hz */
#define BOARD_BootClockRUN BOARD_BootClockFROHF48M


/*******************************************************************************
 ********************* Configuration BOARD_BootClockFRO12M ***********************
 ******************************************************************************/
/*******************************************************************************
 * Definitions for BOARD_BootClockFRO12M configuration
 ******************************************************************************/
#define BOARD_BOOTCLOCKFRO12M_CORE_CLOCK   12000000U    /*!< Core clock frequency:12000000Hz */

/*******************************************************************************
 * API for BOARD_BootClockFRO12M configuration
 ******************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus*/

/*!
 * @brief This function executes configuration of clocks.
 *
 */
void BOARD_BootClockFRO12M(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus*/

/*******************************************************************************
 ********************** Configuration BOARD_BootClockFROHF48M ***********************
 ******************************************************************************/
/*******************************************************************************
 * Definitions for BOARD_BootClockFROHF48M configuration
 ******************************************************************************/
#define BOARD_BOOTCLOCKFROHF48M_CORE_CLOCK   48000000U    /*!< Core clock frequency:48000000Hz */

/*******************************************************************************
 * API for BOARD_BootClockFROHF48M configuration
 ******************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus*/

/*!
 * @brief This function executes configuration of clocks.
 *
 */
void BOARD_BootClockFROHF48M(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus*/

/*******************************************************************************
 ********************* Configuration BOARD_BootClockFROHF96M **********************
 ******************************************************************************/
/*******************************************************************************
 * Definitions for BOARD_BootClockFROHF96M configuration
 ******************************************************************************/
#define BOARD_BOOTCLOCKFROHF96M_CORE_CLOCK   96000000U    /*!< Core clock frequency:96000000Hz */

/*******************************************************************************
 * API for BOARD_BootClockFROHF96M configuration
 ******************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus*/

/*!
 * @brief This function executes configuration of clocks.
 *
 */
void BOARD_BootClockFROHF96M(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus*/

/*******************************************************************************
 ********************* Configuration BOARD_BootClockPLL180M **********************
 ******************************************************************************/
/*******************************************************************************
 * Definitions for BOARD_BootClockPLL180M configuration
 ******************************************************************************/
#define BOARD_BOOTCLOCKPLL180M_CORE_CLOCK   180000000U    /*!< Core clock frequency:180000000Hz */

/*******************************************************************************
 * API for BOARD_BootClockPLL180M configuration
 ******************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus*/

/*!
 * @brief This function executes configuration of clocks.
 *
 */
void BOARD_BootClockPLL180M(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus*/
#endif /* _CLOCK_CONFIG_H_ */

