/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/gaudio/gaudio_options.h
 * @brief   GAUDIO - Audio subsystem options header file.
 *
 * @addtogroup GAUDIO
 * @{
 */

#ifndef _GAUDIO_OPTIONS_H
#define _GAUDIO_OPTIONS_H

/**
 * @name    GAUDIO Functionality to be included
 * @{
 */
	/**
	 * @brief	Audio Play capability is needed
	 */
	#ifndef GAUDIO_NEED_PLAY
		#define GAUDIO_NEED_PLAY			FALSE
	#endif
	/**
	 * @brief	Audio Recording capability is needed
	 */
	#ifndef GAUDIO_NEED_RECORD
		#define GAUDIO_NEED_RECORD			FALSE
	#endif
/**
 * @}
 *
 * @name    GAUDIO Optional Sizing Parameters
 * @{
 */
/** @} */

#endif /* _GAUDIO_OPTIONS_H */
/** @} */
