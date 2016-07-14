/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/gmisc/gmisc_options.h
 * @brief   GMISC - Miscellaneous Routines options header file.
 *
 * @addtogroup GMISC
 * @{
 */

#ifndef _GMISC_OPTIONS_H
#define _GMISC_OPTIONS_H

/**
 * @name    GMISC Functionality to be included
 * @{
 */
	/**
	 * @brief   Include array operation functions
	 * @details	Defaults to FALSE
	 */
	#ifndef GMISC_NEED_ARRAYOPS
		#define GMISC_NEED_ARRAYOPS			FALSE
	#endif
	/**
	 * @brief   Include fast floating point trig functions (fsin, fcos)
	 * @details	Defaults to FALSE
	 */
	#ifndef GMISC_NEED_FASTTRIG
		#define GMISC_NEED_FASTTRIG			FALSE
	#endif
	/**
	 * @brief   Include fast fixed point trig functions (ffsin, ffcos)
	 * @details	Defaults to FALSE
	 */
	#ifndef GMISC_NEED_FIXEDTRIG
		#define GMISC_NEED_FIXEDTRIG		FALSE
	#endif
	/**
	 * @brief   Include fast inverse square root (x^-1/2)
	 * @details	Defaults to FALSE
	 */
	#ifndef GMISC_NEED_INVSQRT
		#define GMISC_NEED_INVSQRT		FALSE
	#endif
/**
 * @}
 *
 * @name    GMISC Optional Parameters
 * @{
 */
	/**
	 * @brief	Modifies the @p invsqrt() function to assume a different integer to floating point endianness.
	 * @note	Normally the floating point format and the integer format have
	 * 			the same endianness. Unfortunately there are some strange
	 * 			processors that don't eg. some very early ARM devices.
	 * 			For those where the endianness doesn't match you can fix it by
	 * 			defining GMISC_INVSQRT_MIXED_ENDIAN.
	 * @note	This still assumes the processor is using an ieee floating point format.
	 *
	 * If you have a software floating point that uses a non-standard
	 * floating point format (or very strange hardware) then define
	 * GMISC_INVSQRT_REAL_SLOW and it will do it the hard way.
	 */
	#ifndef GMISC_INVSQRT_MIXED_ENDIAN
		#define GMISC_INVSQRT_MIXED_ENDIAN	FALSE
	#endif
	/**
	 * @brief	Modifies the @p invsqrt() function to do things the long slow way.
	 * @note	This causes the @p invsqrt() function to work regardless of the
	 * 			processor floating point format.
	 * @note	This makes the @p invsqrt() function very slow.
	 */
	#ifndef GMISC_INVSQRT_REAL_SLOW
		#define GMISC_INVSQRT_REAL_SLOW		FALSE
	#endif
/** @} */

#endif /* _GMISC_OPTIONS_H */
/** @} */
