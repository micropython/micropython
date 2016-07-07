/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/gdisp/gdisp_options.h
 * @brief   GDISP sub-system options header file.
 *
 * @addtogroup GDISP
 * @{
 */

#ifndef _GDISP_OPTIONS_H
#define _GDISP_OPTIONS_H

/**
 * @name    GDISP Functionality to be included
 * @{
 */
	/**
	 * @brief   Should drawing operations be automatically flushed.
	 * @details	Defaults to FALSE
	 * @note	If set to FALSE and the controller requires flushing
	 * 			then the application must manually call @p gdispGFlush().
	 * 			Setting this to TRUE causes GDISP to automatically flush
	 * 			after each drawing operation. Note this may be slow but enables
	 * 			an application to avoid having to manually call the flush routine.
	 * @note	If TRUE and GDISP_NEED_TIMERFLUSH is also TRUE, this takes precedence.
	 * @note	Most controllers don't need flushing which is why this is set to
	 * 			FALSE by default.
	 */
	#ifndef GDISP_NEED_AUTOFLUSH
		#define GDISP_NEED_AUTOFLUSH			FALSE
	#endif
	/**
	 * @brief   Should drawing operations be automatically flushed on a timer.
	 * @details	Defaults to FALSE, Can be set to FALSE or a timer period in milliseconds.
	 * @note	The period should not be set too short or it will consume all your CPU. A
	 * 			value between 250 and 500 milliseconds would probably be suitable.
	 * @note	If TRUE and GDISP_NEED_AUTOFLUSH is also TRUE, this is ineffective.
	 * @note	Most controllers don't need flushing which is why this is set to
	 * 			FALSE by default.
	 */
	#ifndef GDISP_NEED_TIMERFLUSH
		#define GDISP_NEED_TIMERFLUSH			FALSE
	#endif
	/**
	 * @brief   Should all operations be clipped to the screen and colors validated.
	 * @details	Defaults to TRUE.
	 * @note    If this is FALSE, any operations that extend beyond the
	 *          edge of the screen will have undefined results. Any
	 *			out-of-range colors will produce undefined results.
	 * @note	This should always be left as the default (TRUE) unless you
	 * 			are a maniac for speed and you have thoroughly tested your code
	 * 			and it never overwrites the edges of the screen.
	 * @note	Setting GDISP_NEED_CLIP to TRUE internally uses the same mechanism
	 * 			as this validation. There is no advantage in setting this FALSE if
	 * 			GDISP_NEED_CLIP is TRUE.
	 */
	#ifndef GDISP_NEED_VALIDATION
		#define GDISP_NEED_VALIDATION			TRUE
	#endif
	/**
	 * @brief   Are clipping functions needed.
	 * @details	Defaults to TRUE
	 */
	#ifndef GDISP_NEED_CLIP
		#define GDISP_NEED_CLIP					TRUE
	#endif
	/**
	 * @brief   Streaming functions are needed
	 * @details	Defaults to FALSE.
	 */
	#ifndef GDISP_NEED_STREAMING
		#define GDISP_NEED_STREAMING			FALSE
	#endif
	/**
	 * @brief   Are text functions needed.
	 * @details	Defaults to FALSE
	 * @note	You must also define at least one font.
	 */
	#ifndef GDISP_NEED_TEXT
		#define GDISP_NEED_TEXT					FALSE
	#endif
	/**
	 * @brief   Are circle functions needed.
	 * @details	Defaults to FALSE
	 * @note	Uses integer algorithms only. It does not use any trig or floating point.
	 */
	#ifndef GDISP_NEED_CIRCLE
		#define GDISP_NEED_CIRCLE				FALSE
	#endif
	/**
	 * @brief   Are ellipse functions needed.
	 * @details	Defaults to FALSE
	 * @note	Uses integer algorithms only. It does not use any trig or floating point.
	 */
	#ifndef GDISP_NEED_ELLIPSE
		#define GDISP_NEED_ELLIPSE				FALSE
	#endif
	/**
	 * @brief   Are arc sector functions needed.
	 * @details	Defaults to FALSE
	 * @note	Uses integer algorithms only. It does not use any trig or floating point.
	 */
	#ifndef GDISP_NEED_ARCSECTORS
		#define GDISP_NEED_ARCSECTORS			FALSE
	#endif
	/**
	 * @brief   Are arc functions needed.
	 * @details	Defaults to FALSE
	 * @note	This can be compiled using fully integer mathematics by
	 * 			defining GFX_USE_GMISC and GMISC_NEED_FIXEDTRIG as TRUE.
	 * @note	This can be compiled to use floating point but no trig functions
	 * 			by defining GFX_USE_GMISC and GMISC_NEED_FASTTRIG as TRUE.
	 * @note	If neither of the above are defined it requires the maths library
	 * 			to be included in the link to provide floating point and trig support.
	 * 			ie  include -lm in your compiler flags.
	 */
	#ifndef GDISP_NEED_ARC
		#define GDISP_NEED_ARC					FALSE
	#endif
	/**
	 * @brief   Are convex polygon functions needed.
	 * @details	Defaults to FALSE
	 * @note	Convex polygons are those that have no internal angles. That is;
	 * 			you can draw a line from any point on the polygon to any other point
	 * 			on the polygon without it going outside the polygon.
	 */
	#ifndef GDISP_NEED_CONVEX_POLYGON
		#define GDISP_NEED_CONVEX_POLYGON		FALSE
	#endif
	/**
	 * @brief   Are scrolling functions needed.
	 * @details	Defaults to FALSE
	 * @note	This function must be supported by the low level GDISP driver
	 * 			you have included in your project. If it isn't, defining this
	 * 			option will cause a compile error.
	 */
	#ifndef GDISP_NEED_SCROLL
		#define GDISP_NEED_SCROLL				FALSE
	#endif
	/**
	 * @brief   Is the capability to read pixels back needed.
	 * @details	Defaults to FALSE
	 * @note	This function must be supported by the low level GDISP driver
	 * 			you have included in your project. If it isn't, defining this
	 * 			option will cause a compile error.
	 */
	#ifndef GDISP_NEED_PIXELREAD
		#define GDISP_NEED_PIXELREAD			FALSE
	#endif
	/**
	 * @brief   Control some aspect of the hardware operation.
	 * @details	Defaults to FALSE
	 * @note	This allows control of hardware specific features such as
	 * 			screen rotation, backlight levels, contrast etc
	 */
	#ifndef GDISP_NEED_CONTROL
		#define GDISP_NEED_CONTROL				FALSE
	#endif
	/**
	 * @brief   Query some aspect of the hardware operation.
	 * @details	Defaults to FALSE
	 * @note	This allows query of hardware specific features
	 */
	#ifndef GDISP_NEED_QUERY
		#define GDISP_NEED_QUERY				FALSE
	#endif
	/**
	 * @brief   Is the image interface required.
	 * @details	Defaults to FALSE
	 */
	#ifndef GDISP_NEED_IMAGE
		#define GDISP_NEED_IMAGE				FALSE
	#endif
	/**
	 * @brief   Is the image interface required.
	 * @details	Defaults to FALSE
	 */
	#ifndef GDISP_NEED_PIXMAP
		#define GDISP_NEED_PIXMAP				FALSE
	#endif
/**
 * @}
 *
 * @name    GDISP Multi-Threading Options
 * @{
 */
	/**
	 * @brief   Do the drawing functions need to be thread-safe.
	 * @details	Defaults to FALSE
	 */
	#ifndef GDISP_NEED_MULTITHREAD
		#define GDISP_NEED_MULTITHREAD			FALSE
	#endif
/**
 * @}
 *
 * @name    GDISP Optional Parameters
 * @{
 */
	/**
	 * @brief	Should the startup logo be displayed
	 *
	 * @details Defaults to TRUE
	 */
	#ifndef GDISP_NEED_STARTUP_LOGO
		#define GDISP_NEED_STARTUP_LOGO 		TRUE
	#endif
	/**
	 * @brief	Define the initial background color for all displays in the system.
	 */
	#ifndef GDISP_STARTUP_COLOR
		#define GDISP_STARTUP_COLOR				Black
	#endif
	/**
	 * @brief	Define the default orientation for all displays in the system.
	 * @note	GDISP_NEED_CONTROL must also be set (and the hardware must support it)
	 * @note	If not specified then displays default to the native hardware orientation
	 */
	// #define GDISP_DEFAULT_ORIENTATION		GDISP_ROTATE_LANDSCAPE
	/**
	 * @brief   The size of pixel buffer (in pixels) used for optimization.
	 * @details	Set to zero to guarantee disabling of the buffer.
	 * @note	Depending on the driver and what operations the application
	 * 			needs, this buffer may never be allocated.
	 * @note	Setting the size to zero may cause some operations to not
	 * 			compile eg. Scrolling if there is no hardware scroll support.
	 * @note	Increasing the size will speedup certain operations
	 * 			at the expense of RAM.
	 * @note	Currently only used to support scrolling on hardware without
	 * 			scrolling support, and to increase the speed of streaming
	 * 			operations on non-streaming hardware where there is a
	 * 			hardware supported bit-blit.
	 */
	#ifndef GDISP_LINEBUF_SIZE
		#define GDISP_LINEBUF_SIZE				128
	#endif
/**
 * @}
 *
 * @name    GDISP Multiple Display Support
 * @{
 */
	/**
	 * @brief   The total number of displays using the default driver.
	 * @note	If you want to use multiple displays either set GDISP_TOTAL_DISPLAYS or GDISP_DRIVER_LIST
	 *          but not both.
	 */
	#ifndef GDISP_TOTAL_DISPLAYS
		#define GDISP_TOTAL_DISPLAYS		1
	#endif
	#if defined(__DOXYGEN__)
		/**
		 * @brief   The list of display drivers.
		 * @note	Replace this example with your own definition in your gfxconf.h file. See the gdisp_lld.c
		 *          in each driver (near the top) to get the name of the VMT for a driver.
		 * @note    The same driver can occur more than once in the list to create an extra instance of that driver.
		 * @note    If defining this you must also define GDISP_PIXELFORMAT for your application to use.
         *          Choose a value that is most common accross all your drivers for efficiency.
         * @note    If using this you may optionally define the GDISP_HARDWARE_xxx values as either TRUE or FALSE.
         *          Doing this causes GDISP to assume that all (TRUE) or none (FALSE) of the listed drivers have that
         *          capability. This can help improve drawing speed and efficiency.
		 */
		#define GDISP_DRIVER_LIST		  GDISPVMT_Win32, GDISPVMT_SSD1963
	#endif
/**
 * @}
 *
 * @name    GDISP Image Options
 * @pre		GDISP_NEED_IMAGE must be TRUE
 * @{
 */
	/**
	 * @brief   Is native image decoding required.
	 * @details	Defaults to FALSE
	 */
	#ifndef GDISP_NEED_IMAGE_NATIVE
		#define GDISP_NEED_IMAGE_NATIVE			FALSE
	#endif
	/**
	 * @brief   Is GIF image decoding required.
	 * @details	Defaults to FALSE
	 */
	#ifndef GDISP_NEED_IMAGE_GIF
		#define GDISP_NEED_IMAGE_GIF			FALSE
	#endif
	/**
	 * @brief   Is BMP image decoding required.
	 * @details	Defaults to FALSE
	 */
	#ifndef GDISP_NEED_IMAGE_BMP
		#define GDISP_NEED_IMAGE_BMP			FALSE
	#endif
	/**
	 * @brief   Is JPG image decoding required.
	 * @details	Defaults to FALSE
	 */
	#ifndef GDISP_NEED_IMAGE_JPG
		#define GDISP_NEED_IMAGE_JPG			FALSE
	#endif
	/**
	 * @brief   Is PNG image decoding required.
	 * @details	Defaults to FALSE
	 */
	#ifndef GDISP_NEED_IMAGE_PNG
		#define GDISP_NEED_IMAGE_PNG			FALSE
	#endif
	/**
	 * @brief   Is memory accounting required during image decoding.
	 * @details	Defaults to FALSE
	 */
	#ifndef GDISP_NEED_IMAGE_ACCOUNTING
		#define GDISP_NEED_IMAGE_ACCOUNTING		FALSE
	#endif
/**
 * @}
 *
 * @name    GDISP BMP Image Options
 * @pre		GDISP_NEED_IMAGE and GDISP_NEED_IMAGE_BMP must be TRUE
 * @{
 */
	/**
	 * @brief   Is BMP 1 bit per pixel (monochrome/2 color) image decoding required.
	 * @details	Defaults to TRUE
	 */
	#ifndef GDISP_NEED_IMAGE_BMP_1
		#define GDISP_NEED_IMAGE_BMP_1		TRUE
	#endif
	/**
	 * @brief   Is BMP 4 bits per pixel (16 color) image decoding required.
	 * @details	Defaults to TRUE
	 */
	#ifndef GDISP_NEED_IMAGE_BMP_4
		#define GDISP_NEED_IMAGE_BMP_4		TRUE
	#endif
	/**
	 * @brief   Is BMP 4 bits per pixel (16 color) with RLE compression image decoding required.
	 * @details	Defaults to TRUE
	 */
	#ifndef GDISP_NEED_IMAGE_BMP_4_RLE
		#define GDISP_NEED_IMAGE_BMP_4_RLE	TRUE
	#endif
	/**
	 * @brief   Is BMP 8 bits per pixel (256 color) image decoding required.
	 * @details	Defaults to TRUE
	 */
	#ifndef GDISP_NEED_IMAGE_BMP_8
		#define GDISP_NEED_IMAGE_BMP_8		TRUE
	#endif
	/**
	 * @brief   Is BMP 8 bits per pixel (256 color) with RLE compression image decoding required.
	 * @details	Defaults to TRUE
	 */
	#ifndef GDISP_NEED_IMAGE_BMP_8_RLE
		#define GDISP_NEED_IMAGE_BMP_8_RLE	TRUE
	#endif
	/**
	 * @brief   Is BMP 16 bits per pixel (65536 color) image decoding required.
	 * @details	Defaults to TRUE
	 */
	#ifndef GDISP_NEED_IMAGE_BMP_16
		#define GDISP_NEED_IMAGE_BMP_16		TRUE
	#endif
	/**
	 * @brief   Is BMP 24 bits per pixel (true-color) image decoding required.
	 * @details	Defaults to TRUE
	 */
	#ifndef GDISP_NEED_IMAGE_BMP_24
		#define GDISP_NEED_IMAGE_BMP_24		TRUE
	#endif
	/**
	 * @brief   Is BMP 32 bits per pixel (true-color) image decoding required.
	 * @details	Defaults to TRUE
	 */
	#ifndef GDISP_NEED_IMAGE_BMP_32
		#define GDISP_NEED_IMAGE_BMP_32		TRUE
	#endif
/**
 * @}
 *
 * @name    GDISP PNG Image Options
 * @pre		GDISP_NEED_IMAGE and GDISP_NEED_IMAGE_PNG must be TRUE
 * @{
 */
	/**
	 * @brief   Is PNG Interlaced image decoding required.
	 * @details	Defaults to FALSE
	 * @note	Currently not supported due to the complex decoding and display requirements
	 */
	#ifndef GDISP_NEED_IMAGE_PNG_INTERLACED
		#define GDISP_NEED_IMAGE_PNG_INTERLACED			FALSE
	#endif
	/**
	 * @brief   Is PNG image transparency processed.
	 * @details	Defaults to TRUE
	 */
	#ifndef GDISP_NEED_IMAGE_PNG_TRANSPARENCY
		#define GDISP_NEED_IMAGE_PNG_TRANSPARENCY		TRUE
	#endif
	/**
	 * @brief   Is PNG background data processed.
	 * @details	Defaults to TRUE
	 * @note	If the background is specified in the image file and this define is TRUE,
	 * 			that background color is used for transparency and alpha blending.
	 */
	#ifndef GDISP_NEED_IMAGE_PNG_BACKGROUND
		#define GDISP_NEED_IMAGE_PNG_BACKGROUND			TRUE
	#endif
	/**
	 * @brief   What is the cliff between non-blended alpha pixels being displayed or not.
	 * @details	Range of 0 to 255
	 * @note	If GDISP_NEED_IMAGE_PNG_BACKGROUND is TRUE and the PNG file contains a
	 * 			background color then the pixel will be blended with the background color
	 * 			according to the alpha.
	 * 			If not then no blending occurs. The pixel will either be set or not.
	 * 			Any alpha value greater or equal to this number will be displayed.
	 * 			Anything less than this number is not displayed.
	 */
	#ifndef GDISP_NEED_IMAGE_PNG_ALPHACLIFF
		#define GDISP_NEED_IMAGE_PNG_ALPHACLIFF			32
	#endif
	/**
	 * @brief   Is 1, 2 and 4 bit PNG palettized image decoding required.
	 * @details	Defaults to TRUE
	 */
	#ifndef GDISP_NEED_IMAGE_PNG_PALETTE_124
		#define GDISP_NEED_IMAGE_PNG_PALETTE_124		TRUE
	#endif
	/**
	 * @brief   Is 8 bit PNG palettized image decoding required.
	 * @details	Defaults to TRUE
	 */
	#ifndef GDISP_NEED_IMAGE_PNG_PALETTE_8
		#define GDISP_NEED_IMAGE_PNG_PALETTE_8			TRUE
	#endif
	/**
	 * @brief   Is 1,2 and 4 bit PNG grayscale image decoding required.
	 * @details	Defaults to TRUE
	 */
	#ifndef GDISP_NEED_IMAGE_PNG_GRAYSCALE_124
		#define GDISP_NEED_IMAGE_PNG_GRAYSCALE_124		TRUE
	#endif
	/**
	 * @brief   Is 8 bit PNG grayscale image decoding required.
	 * @details	Defaults to TRUE
	 */
	#ifndef GDISP_NEED_IMAGE_PNG_GRAYSCALE_8
		#define GDISP_NEED_IMAGE_PNG_GRAYSCALE_8		TRUE
	#endif
	/**
	 * @brief   Is 16 bit PNG grayscale image decoding required.
	 * @details	Defaults to TRUE
	 */
	#ifndef GDISP_NEED_IMAGE_PNG_GRAYSCALE_16
		#define GDISP_NEED_IMAGE_PNG_GRAYSCALE_16		TRUE
	#endif
	/**
	 * @brief   Is 8 bit PNG grayscale with 8 bit alpha image decoding required.
	 * @details	Defaults to TRUE
	 */
	#ifndef GDISP_NEED_IMAGE_PNG_GRAYALPHA_8
		#define GDISP_NEED_IMAGE_PNG_GRAYALPHA_8		TRUE
	#endif
	/**
	 * @brief   Is 16 bit PNG grayscale with 16 bit alpha image decoding required.
	 * @details	Defaults to TRUE
	 */
	#ifndef GDISP_NEED_IMAGE_PNG_GRAYALPHA_16
		#define GDISP_NEED_IMAGE_PNG_GRAYALPHA_16		TRUE
	#endif
	/**
	 * @brief   Is 8/8/8 bit PNG RGB image decoding required.
	 * @details	Defaults to TRUE
	 */
	#ifndef GDISP_NEED_IMAGE_PNG_RGB_8
		#define GDISP_NEED_IMAGE_PNG_RGB_8				TRUE
	#endif
	/**
	 * @brief   Is 16/16/16 bit PNG RGB image decoding required.
	 * @details	Defaults to TRUE
	 */
	#ifndef GDISP_NEED_IMAGE_PNG_RGB_16
		#define GDISP_NEED_IMAGE_PNG_RGB_16				TRUE
	#endif
	/**
	 * @brief   Is 8/8/8 bit PNG RGB with 8 bit alpha image decoding required.
	 * @details	Defaults to TRUE
	 */
	#ifndef GDISP_NEED_IMAGE_PNG_RGBALPHA_8
		#define GDISP_NEED_IMAGE_PNG_RGBALPHA_8			TRUE
	#endif
	/**
	 * @brief   Is 16/16/16 bit PNG RGB with 16 bit alpha image decoding required.
	 * @details	Defaults to TRUE
	 */
	#ifndef GDISP_NEED_IMAGE_PNG_RGBALPHA_16
		#define GDISP_NEED_IMAGE_PNG_RGBALPHA_16		TRUE
	#endif
/**
 * @}
 *
 * @name	GDISP Text Rendering Options
 * @{
 */
	/**
	 * @brief	Enable advanced word-wrapping.
	 * @details	Only has an effect with @p gdispGDrawStringBox() and @p gdispGFillStringBox()
	 * @details Defaults to FALSE
	 */
	#ifndef GDISP_NEED_TEXT_WORDWRAP
		#define GDISP_NEED_TEXT_WORDWRAP		FALSE
	#endif
	/**
	 * @brief	Enable UTF-8 support for text rendering.
	 * @details Defaults to FALSE
	 */
	#ifndef GDISP_NEED_UTF8
		#define GDISP_NEED_UTF8					FALSE
	#endif
	/**
	 * @brief	Enable kerning for font rendering (improves character placement).
	 * @details	Defaults to FALSE
	 */
	#ifndef GDISP_NEED_TEXT_KERNING
		#define GDISP_NEED_TEXT_KERNING			FALSE
	#endif
	/**
	 * @brief	Enable antialiased font support
	 * @details	Defaults to FALSE
	 */
	#ifndef GDISP_NEED_ANTIALIAS
		#define GDISP_NEED_ANTIALIAS			FALSE
	#endif
/**
 * @}
 *
 * @name	GDISP Pixmap Options
 * @{
 */
	#ifndef GDISP_NEED_PIXMAP_IMAGE
		#define GDISP_NEED_PIXMAP_IMAGE			FALSE
	#endif
/**
 * @}
 *
 * @name    GDISP Optional Low Level Driver Defines
 * @{
 */
	/**
	 * @brief   Set the screen height and width.
	 * @note	Ignored by some low level GDISP drivers, optional for others.
	 * @note	Where these values are allowed, a default is always provided be the low level driver.
	 * @note	The list of GDISP low level drivers that allow these to be set are...
	 *				WIN32, SSD1289, SSD1963, TestStub
	 */
	/* #define GDISP_SCREEN_WIDTH		nnnn */
	/* #define GDISP_SCREEN_HEIGHT		nnnn */
	/**
	 * @brief   Define which bus interface to use.
	 * @details	Only required by the SSD1963 driver.
	 * @note	This will be replaced eventually by board definition files
	 */
	// #define GDISP_USE_FSMC
	// #define GDISP_USE_GPIO
/** @} */

#endif /* _GDISP_OPTIONS_H */
/** @} */

