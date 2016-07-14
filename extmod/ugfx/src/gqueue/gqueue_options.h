/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/gqueue/gqueue_options.h
 * @brief   GQUEUE - Queue options header file.
 *
 * @addtogroup GQUEUE
 * @{
 */

#ifndef _GQUEUE_OPTIONS_H
#define _GQUEUE_OPTIONS_H

/**
 * @name    GQUEUE Functions to include.
 * @{
 */
	/**
	 * @brief   Enable Asynchronous Queues
	 * @details	Defaults to FALSE
	 */
	#ifndef GQUEUE_NEED_ASYNC
		#define GQUEUE_NEED_ASYNC		FALSE
	#endif
	/**
	 * @brief   Enable Get-Synchronous Queues
	 * @details	Defaults to FALSE
	 */
	#ifndef GQUEUE_NEED_GSYNC
		#define GQUEUE_NEED_GSYNC		FALSE
	#endif
	/**
	 * @brief   Enable Fully Synchronous Queues
	 * @details	Defaults to FALSE
	 */
	#ifndef GQUEUE_NEED_FSYNC
		#define GQUEUE_NEED_FSYNC		FALSE
	#endif
	/**
	 * @brief	Enable Queue-able Data Buffers
	 */
	#ifndef GQUEUE_NEED_BUFFERS
		#define GQUEUE_NEED_BUFFERS		FALSE
	#endif
/**
 * @}
 *
 * @name    GQUEUE Optional Sizing Parameters
 * @{
 */
/** @} */

#endif /* _GQUEUE_OPTIONS_H */
/** @} */
