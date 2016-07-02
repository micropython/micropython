/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/gadc/gadc_options.h
 * @brief   GADC - Periodic ADC subsystem options header file.
 *
 * @addtogroup GADC
 * @{
 */

#ifndef _GADC_OPTIONS_H
#define _GADC_OPTIONS_H

/**
 * @name    GADC Functionality to be included
 * @{
 */
/**
 * @}
 *
 * @name    GADC Optional Sizing Parameters
 * @{
 */
	/**
	 * @brief   The maximum GADC sample rate
	 * @details	Defaults to 44000
	 * @note	This value must be less than half the maximum sample rate allowed by the CPU.
	 * 			This is to ensure there is time between high speed samples to perform low
	 * 			speed device sampling.
	 */
	#ifndef GADC_MAX_HIGH_SPEED_SAMPLERATE
		#define GADC_MAX_HIGH_SPEED_SAMPLERATE	44000
	#endif
/** @} */

#endif /* _GADC_OPTIONS_H */
/** @} */
