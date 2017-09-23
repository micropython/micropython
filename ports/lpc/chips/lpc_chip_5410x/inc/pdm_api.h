/*
 * @brief PDM mic interface module
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2014
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#ifndef __PDM_API_H_
#define __PDM_API_H_

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "lpc_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup SH_PDM PDMLIB: PDM interface API
 * @ingroup SENSOR_HUB
 * @{
 */

/**
 * @brief	Initialize PDM module
 * @return	Nothing
 * @note PDM Clock (Clk out or Timer1) should be configured by the application.
 */
extern void PDM_Init(void);

/**
 * @brief	Reset PDM interface
 * @return	Nothing
 * @note	This function resets the PDM (SI interface) state variables for a fresh start.
 */
extern void PDM_Reset(void);

/**
 * @brief	Start capturing PDM data
 * @return	Nothing
 */
extern void PDM_Start(void);

/**
 * @brief	Stop capturing PDM data
 * @return	Nothing
 */
extern void PDM_Stop(void);

/**
 * @brief	Change PDM Decimation factor.
 * @return	0 Success, 1 Error New decimation factor is not within 8 - 216.
 */
extern uint32_t PDM_ChgDecm(uint32_t new_decm);

/**
 * @brief	Change PDM Clock & Data pin. This function should be called after 
 * @ref PDM_Init if pdm clock and data pins needs to be changed.
 * @return	0 Success, 1 Error PDM Clock and/or Data pin is not within 0 - 15.
 */
extern uint32_t PDM_ChgPin(uint8_t pdm_clk_pin, uint8_t pdm_data_pin);

/******************************************************************************
 *          Callback routines
 *****************************************************************************/
/**
 * @brief	PDM data ready callback
 * @return	Nothing
 * @note	This function is called by PDM block when data is ready. Partially
 *	decimated data is available through @ref pdm_audio buffer.
 *  data for voice activity detection is available through @ref envelope_buffer.
 */
extern void PDM_DataReady(void);

/** 
 * @brief Audio envelop data.
 * Envelope information:
 * - envelope_buffer[0] Fast detector
 * - envelope_buffer[1] Slow detector
 * - envelope_buffer[2] Reserved for internal use
 * - envelope_buffer[3] Reserved for internal use
 * - envelope_buffer[4] Latest biggest difference between fast and slow detector
 * - envelope_buffer[5] latest output of pre envelope lowpass filter
 * - envelope_buffer[6] latest output of pre envelope highpass filter
 */
extern volatile int32_t envelope_buffer[];

/** @brief Decimated PDM data.
    A DC cut filter should be applied on this data to get 16bit PCM samples @ 16KHz
 */
extern volatile int32_t pdm_audio[];

#define PDM_PONG (*((volatile unsigned *) 0x4004C044)) & (1 << 6)

#define PDM_IRQn (IRQn_Type) 30

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __PDM_API_H_ */
