/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/gos/gos_options.h
 * @brief   GOS - Operating System options header file.
 *
 * @addtogroup GOS
 * @{
 */

#ifndef _GOS_OPTIONS_H
#define _GOS_OPTIONS_H

/**
 * @name    The operating system to use. One (and only one) of these must be defined.
 * @{
 */
	/**
	 * @brief   Use ChibiOS
	 * @details	Defaults to FALSE
	 */
	#ifndef GFX_USE_OS_CHIBIOS
		#define GFX_USE_OS_CHIBIOS		FALSE
	#endif
	/**
	 * @brief   Use FreeRTOS
	 * @details	Defaults to FALSE
	 */
	#ifndef GFX_USE_OS_FREERTOS
		#define GFX_USE_OS_FREERTOS		FALSE
	#endif
	/**
	 * @brief   Use Win32
	 * @details	Defaults to FALSE
	 */
	#ifndef GFX_USE_OS_WIN32
		#define GFX_USE_OS_WIN32		FALSE
	#endif
	/**
	 * @brief   Use a linux based system running X11
	 * @details	Defaults to FALSE
	 */
	#ifndef GFX_USE_OS_LINUX
		#define GFX_USE_OS_LINUX		FALSE
	#endif
	/**
	 * @brief   Use a Mac OS-X based system
	 * @details	Defaults to FALSE
	 */
	#ifndef GFX_USE_OS_OSX
		#define GFX_USE_OS_OSX			FALSE
	#endif
	/**
	 * @brief   Use a Raw 32-bit CPU based system (Bare Metal)
	 * @details	Defaults to FALSE
	 */
	#ifndef GFX_USE_OS_RAW32
		#define GFX_USE_OS_RAW32		FALSE
	#endif
	/**
	 * @brief   Use a eCos
	 * @details	Defaults to FALSE
	 */
	#ifndef GFX_USE_OS_ECOS
		#define GFX_USE_OS_ECOS			FALSE
	#endif
	/**
	 * @brief   Use RAWRTOS
	 * @details Defaults to FALSE
	 */
	#ifndef GFX_USE_OS_RAWRTOS
		#define GFX_USE_OS_RAWRTOS		FALSE
	#endif
	/**
	 * @brief   Use Arduino
	 * @details	Defaults to FALSE
	 */
	#ifndef GFX_USE_OS_ARDUINO
		#define GFX_USE_OS_ARDUINO		FALSE
	#endif
	/**
	 * @brief	Use CMSIS RTOS compatible OS
	 * @details	Defaults to FALSE
	 */
	#ifndef GFX_USE_OS_CMSIS
		#define GFX_USE_OS_CMSIS		FALSE
	#endif
	/**
	 * @brief   Use Keil CMSIS
	 * @details	Defaults to FALSE
	 */
	#ifndef GFX_USE_OS_KEIL
		#define GFX_USE_OS_KEIL			FALSE
	#endif
	/**
	 * @brief   Use NIOS-II
	 * @details	Defaults to FALSE
	 */
	#ifndef GFX_USE_OS_NIOS
		#define GFX_USE_OS_NIOS			FALSE
	#endif
/**
 * @}
 *
 * @name    GOS Optional Parameters
 * @{
 */
 	/**
 	 * @name	GFX_OS_PRE_INIT_FUNCTION
 	 * @brief	A macro that defines a function that uGFX calls as part of gfxInit() in order to initialize hardware
 	 * @details	Defaults to undefined
 	 * @note	If defined the specified function is called before any other initialization.
 	 * 			It is typically used to initialize hardware or the C runtime.
 	 * @note	Eg. In your source:
 	 * 					void myHardwareInitRoutine(void);
 	 * 				In gfxconf.h:
 	 * 					#define GFX_OS_PRE_INIT_FUNCTION myHardwareInitRoutine
 	 */
    //#define GFX_OS_PRE_INIT_FUNCTION                 myHardwareInitRoutine
 	/**
 	 * @name	GFX_OS_EXTRA_INIT_FUNCTION
 	 * @brief	A macro that defines a function that uGFX calls as part of gfxInit() just after initializing
 	 * 			the operating system.
 	 * @details	Defaults to undefined
 	 * @note	If defined the specified function is called just after the operating system is initialized by
 	 * 			gfxInit(). Note that if gfxInit() is set up to not initialize an operating system it is called after
 	 * 			the GFX_OS_PRE_INIT_FUNCTION function (if any).
 	 * @note	Eg. In your source:
 	 * 					void myOSInitRoutine(void);
 	 * 				In gfxconf.h:
 	 * 					#define GFX_OS_EXTRA_INIT_FUNCTION myOSInitRoutine
 	 */
    //#define GFX_OS_EXTRA_INIT_FUNCTION               myOSInitRoutine
	/**
 	 * @name	GFX_OS_EXTRA_DEINIT_FUNCTION
 	 * @brief	A macro that defines a function that uGFX calls as part of gfxDeInit() just before de-initializing
 	 * 			the operating system.
 	 * @details	Defaults to undefined
 	 * @note	If defined the specified function is called just before the operating system is de-initialized by
 	 * 			gfxDeInit().
 	 * @note	Eg. In your source:
 	 * 					void myOSDeInitRoutine(void);
 	 * 				In gfxconf.h:
 	 * 					#define GFX_OS_EXTRA_DEINIT_FUNCTION myOSDeInitRoutine
 	 */
    //#define GFX_OS_EXTRA_DEINIT_FUNCTION             myOSDeInitRoutine
 	/**
 	 * @brief	Should uGFX avoid initializing the operating system
 	 * @details	Defaults to FALSE
 	 * @note	This is not relevant to all operating systems eg Win32 never initializes the
 	 * 			operating system as uGFX runs as an application outside the boot process.
 	 * @note	Operating system initialization is not necessarily implemented for all
 	 * 			operating systems yet even when it is relevant. These operating systems
 	 * 			will display a compile warning reminding you to initialize the operating
 	 * 			system in your application code. Note that on these operating systems the
 	 * 			demo applications will not work without modification.
 	 */
 	#ifndef GFX_OS_NO_INIT
 		#define GFX_OS_NO_INIT			FALSE
 	#endif
 	/**
 	 * @brief	Turn off warnings about initializing the operating system
 	 * @details	Defaults to FALSE
 	 * @note	This is only relevant where GOS cannot initialize the operating
 	 * 			system automatically or the operating system initialization has been
 	 * 			explicitly turned off.
 	 */
	#ifndef GFX_OS_INIT_NO_WARNING
		#define GFX_OS_INIT_NO_WARNING	FALSE
	#endif
 	/**
 	 * @brief	Should uGFX stuff be added to the FreeRTOS+Tracer
 	 * @details	Defaults to FALSE
 	 */
 	#ifndef GFX_FREERTOS_USE_TRACE
 		#define GFX_FREERTOS_USE_TRACE	FALSE
 	#endif
 	/**
 	 * @brief	How much RAM should uGFX use for the heap when using its own internal heap allocator
 	 * @details	Defaults to 0.
 	 * @note	Only used when the internal ugfx heap allocator is used
 	 * 				(GFX_USE_OS_RAW32, GFX_USE_OS_ARDUINO, GFX_US_OS_KEIL, GFX_USE_OS_CMSIS)
 	 * @note	If 0 then the standard C runtime malloc(), free() and realloc()
 	 * 			are used.
 	 * @note	If it is non-zero then this is the number of bytes of RAM
 	 * 			to use for the heap (gfxAlloc() and gfxFree()). No C
 	 * 			runtime routines will be used and a new routine @p gfxAddHeapBlock()
 	 * 			is added allowing the user to add extra memory blocks to the heap.
 	 */
	#ifndef GFX_OS_HEAP_SIZE
		#define GFX_OS_HEAP_SIZE		0
	#endif
 	/**
 	 * @brief	Enable wrappers for malloc() and free()
 	 * @details	Defaults to FALSE
 	 * @note	If enabled, malloc() and free() will be implemented as wrappers that call gfxAlloc() and
 	 *			gfxFree() in order to provide portability to libraries using these function.
 	 * @note	Enabling this can solve 'unresolved _sbrk' issues
 	 * @note	It is strongly recommended to always use gfxAlloc() and gfxFree() directy to ensure
 	 *			portability of uGFX applications across all platforms.
 	 */
	#ifndef GFX_EMULATE_MALLOC
		#define GFX_EMULATE_MALLOC	FALSE
	#endif
/** @} */

#endif /* _GOS_OPTIONS_H */
/** @} */
