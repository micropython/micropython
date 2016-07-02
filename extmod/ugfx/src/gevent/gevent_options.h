/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/gevent/gevent_options.h
 * @brief   GEVENT sub-system options header file.
 *
 * @addtogroup GEVENT
 * @{
 */

#ifndef _GEVENT_OPTIONS_H
#define _GEVENT_OPTIONS_H

/**
 * @name    GEVENT Functionality to be included
 * @{
 */
	/**
	 * @brief   Should routines assert() if they run out of resources.
	 * @details	Defaults to FALSE.
	 * @details	If FALSE the application must be prepared to handle these
	 *			failures.
	 */
	#ifndef GEVENT_ASSERT_NO_RESOURCE
		#define GEVENT_ASSERT_NO_RESOURCE		FALSE
	#endif
/**
 * @}
 *
 * @name    GEVENT Optional Sizing Parameters
 * @{
 */
	/**
	 * @brief   Defines the maximum size of an event status variable.
	 * @details	Defaults to 32 bytes
	 */
	#ifndef GEVENT_MAXIMUM_SIZE
		#define GEVENT_MAXIMUM_SIZE				32
	#endif
	/**
	 * @brief   Defines the maximum Source/Listener pairs in the system.
	 * @details	Defaults to 32
	 */
	#ifndef GEVENT_MAX_SOURCE_LISTENERS
		#define GEVENT_MAX_SOURCE_LISTENERS		32
	#endif
/** @} */

#endif /* _GEVENT_OPTIONS_H */
/** @} */
