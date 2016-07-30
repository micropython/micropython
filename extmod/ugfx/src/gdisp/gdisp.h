/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/gdisp/gdisp.h
 * @brief   GDISP Graphic Driver subsystem header file.
 *
 * @addtogroup GDISP
 *
 * @brief		Module to interface graphic / pixel oriented displays
 *
 * @details		The GDISP module provides high level abstraction to interface pixel oriented graphic displays.
 *
 * @pre			GFX_USE_GDISP must be set to TRUE in gfxconf.h
 *
 * @note		Each drawing routine supports a gdispXXXX and a gdispGXXXX function. The difference is that the
 * 				gdispXXXX function does not require a display to be specified. Note there is a slight anomaly
 * 				in the naming with gdispGBlitArea() vs gdispBlitAreaEx() and gdispBlitArea(), the latter of
 * 				which is now deprecated.
 * @{
 */

#ifndef _GDISP_H
#define _GDISP_H

#include "../../gfx.h"

/* This type definition is defined here as it gets used in other gfx sub-systems even
 * if GFX_USE_GDISP is FALSE.
 */

/**
 * @brief   The type for a coordinate or length on the screen.
 */
typedef int16_t	coord_t;

#if GFX_USE_GDISP || defined(__DOXYGEN__)

/*===========================================================================*/
/* Type definitions                                                          */
/*===========================================================================*/

/**
 * @struct point
 * @brief   Type for a 2D point on the screen.
 */
typedef struct point {
	coord_t x;		/**< The x coordinate of the point. */
	coord_t y;		/**< The y coordinate of the point. */
} point, point_t;

/**
 * @enum justify
 * @brief   Type for the text justification.
 */
typedef enum justify {
	justifyLeft = 0,		   /**< Justify Left */
	justifyCenter = 1,		   /**< Justify Center */
	justifyRight = 2,		   /**< Justify Right */
	justifyLeftTop = 0 + 4,	   /**< Justify Left */
	justifyCenterTop = 1 + 4,  /**< Justify Center */
	justifyRightTop = 2 + 4	   /**< Justify Right */
} justify_t;

/**
 * @enum fontmetric
 * @brief   Type for the font metric.
 */
typedef enum fontmetric {
	fontHeight,				/**< The height of the font */
	fontDescendersHeight,	/**< The descenders height */
	fontLineSpacing,		/**< The line spacing */
	fontCharPadding,		/**< The char padding */
	fontMinWidth,			/**< The minimum width */
	fontMaxWidth,			/**< The maximum width */
	fontBaselineX,			/**< The base line in x direction */
	fontBaselineY			/**< The base line in y direction */
} fontmetric_t;

/**
 * @brief   The type of a font.
 */
typedef const struct mf_font_s* font_t;

/**
 * @enum 	orientation
 * @brief   Type for the screen orientation.
 * @note	GDISP_ROTATE_LANDSCAPE and GDISP_ROTATE_PORTRAIT are internally converted to the
 * 			most appropriate other orientation.
 */
typedef enum orientation {
	GDISP_ROTATE_0 = 0,				/**< Don't rotate. This is the displays native orientation. */
	GDISP_ROTATE_90 = 90,			/**< Rotate by 90 degrees absolute to the native rotation. */
	GDISP_ROTATE_180 = 180,			/**< Rotate by 180 degrees absolute to the native rotation. */
	GDISP_ROTATE_270 = 270,			/**< Rotate by 270 degrees absolute to the native rotation. */
	GDISP_ROTATE_PORTRAIT = 1000,	/**< Put the display into portrait mode. */
	GDISP_ROTATE_LANDSCAPE = 1001	/**< Put the display into landscape mode. */
} orientation_t;

/**
 * @enum 	powermode
 * @brief   Type for the available power modes for the screen.
 */
typedef enum powermode {
	powerOff,						/**< Turn the display off. */
	powerSleep,						/**< Put the display into sleep mode. */
	powerDeepSleep,					/**< Put the display into deep-sleep mode. */
	powerOn							/**< Turn the display on. */
} powermode_t;

/*
 * Our black box display structure.
 */
typedef struct GDisplay		GDisplay;

/**
 * @brief   The default screen to use for the gdispXXXX calls.
 * @note	This is set by default to the first display in the system. You can change
 * 			it by calling @p gdispGSetDisplay().
 */
extern GDisplay	*GDISP;

/*===========================================================================*/
/* Constants.                                                                */
/*===========================================================================*/

/**
 * @brief   Driver Control Constants
 * @details	Unsupported control codes are ignored.
 * @note	The value parameter should always be typecast to (void *).
 * @note	There are some predefined and some specific to the low level driver.
 * @note	GDISP_CONTROL_POWER			- Takes a gdisp_powermode_t
 * 			GDISP_CONTROL_ORIENTATION	- Takes a gdisp_orientation_t
 * 			GDISP_CONTROL_BACKLIGHT -	 Takes an int from 0 to 100. For a driver
 * 											that only supports off/on anything other
 * 											than zero is on.
 * 			GDISP_CONTROL_CONTRAST		- Takes an int from 0 to 100.
 * 			GDISP_CONTROL_LLD			- Low level driver control constants start at
 * 											this value.
 */
#define GDISP_CONTROL_POWER			0
#define GDISP_CONTROL_ORIENTATION	1
#define GDISP_CONTROL_BACKLIGHT		2
#define GDISP_CONTROL_CONTRAST		3
#define GDISP_CONTROL_LLD			1000

/*===========================================================================*/
/* Defines relating to the display hardware									 */
/*===========================================================================*/

#if !defined(GDISP_DRIVER_LIST)
	// Pull in the default hardware configuration for a single controller.
	// If we have multiple controllers the settings must be set in the
	// users gfxconf.h file.
	// Use the compiler include path to find it
	#include "gdisp_lld_config.h"

	// Unless the user has specified a specific pixel format, use
	// the native format for the controller.
	#if !defined(GDISP_PIXELFORMAT) && defined(GDISP_LLD_PIXELFORMAT)
		#define GDISP_PIXELFORMAT 			GDISP_LLD_PIXELFORMAT
	#endif
#endif

/**
 * @name    GDISP pixel format choices
 * @{
 */
	/**
	 * @brief   The pixel format.
	 * @details	It generally defaults to the hardware pixel format.
	 * @note	This doesn't need to match the hardware pixel format.
	 * 			It is definitely more efficient when it does.
	 * @note	When GDISP_DRIVER_LIST is defined, this must
	 * 			be explicitly defined and you should ensure the best match
	 * 			with your hardware across all devices.
	 */
	#ifndef GDISP_PIXELFORMAT
		#define GDISP_PIXELFORMAT 			GDISP_PIXELFORMAT_ERROR
	#endif
	/**
	 * @brief   Do pixels require packing for a blit
	 * @note	Is only valid for a pixel format that doesn't fill it's datatype. eg formats:
	 *				GDISP_PIXELFORMAT_RGB888
	 *				GDISP_PIXELFORMAT_RGB444
	 *				GDISP_PIXELFORMAT_RGB666
	 *				GDISP_PIXELFORMAT_CUSTOM
	 * @note	Very few cases should actually require packed pixels as the low
	 *				level driver can also pack on the fly as it is sending it
	 *				to the graphics device.
	 * @note	Packed pixels are not really supported at this point.
	 */
	#ifndef GDISP_PACKED_PIXELS
		#define GDISP_PACKED_PIXELS			FALSE
	#endif

	/**
	 * @brief   Do lines of pixels require packing for a blit
	 * @note	Ignored if GDISP_PACKED_PIXELS is FALSE
	 */
	#ifndef GDISP_PACKED_LINES
		#define GDISP_PACKED_LINES			FALSE
	#endif
/** @} */

/*===========================================================================*/
/* Defines related to the pixel format										 */
/*===========================================================================*/

/* Load our color definitions and pixel formats */
#include "gdisp_colors.h"

/**
 * @brief   The type of a pixel.
 */
typedef color_t		pixel_t;

#ifdef __cplusplus
extern "C" {
#endif

/* Color Utility Functions */

/**
 * @brief   Blend 2 colors according to the alpha
 * @return	The combined color
 *
 * @param[in] fg		The foreground color
 * @param[in] bg		The background color
 * @param[in] alpha		The alpha value (0-255). 0 is all background, 255 is all foreground.
 *
 * @api
 */
color_t gdispBlendColor(color_t fg, color_t bg, uint8_t alpha);

/**
 * @brief   Find a contrasting color
 * @return	The contrasting color
 *
 * @param[in] color		The color to contrast
 *
 * @api
 */
color_t gdispContrastColor(color_t color);

/* Base Functions */

/**
 * @brief   Get the specified display
 * @return	The pointer to the display or NULL if the display doesn't exist
 * @note	The GDISP variable contains the display used by the gdispXxxx routines
 * 			as opposed to the gdispGXxxx routines which take an explicit display
 * 			parameter.
 * @note	Displays are numbered from 0 to @p gdispGetDisplayCount() - 1
 *
 * @param[in] display	The display number (0..n)
 *
 * @api
 */
GDisplay *gdispGetDisplay(unsigned display);

/**
 * @brief   Set the current default display to the specified display
 * @note	The default display is used for the gdispXxxx functions.
 * @note	The default display is contained in the variable GDISP. Using
 * 			this function to set it protects against it being set to a NULL
 * 			value.
 * @note	If a NULL is passed for the dispay this call is ignored.
 *
 * @param[in] g 	The display to use
 *
 * @api
 */
void gdispSetDisplay(GDisplay *g);

/**
 * @brief   Get the count of currently active displays
 * @return  The count of displays currently in the system
 *
 * @note	Displays are numbered from 0 to @p gdispGetDisplayCount() - 1
 */
unsigned gdispGetDisplayCount(void);

/* Property Functions */

/**
 * @brief   Get the display width in pixels.
 *
 * @param[in] g 		The display to use
 *
 * @return	The width of the display
 *
 * @api
 */
coord_t gdispGGetWidth(GDisplay *g);
#define gdispGetWidth()								gdispGGetWidth(GDISP)

/**
 * @brief   Get the display height in pixels.
 *
 * @param[in] g 		The display to use
 *
 * @return	The height of the display
 *
 * @api
 */
coord_t gdispGGetHeight(GDisplay *g);
#define gdispGetHeight()							gdispGGetHeight(GDISP)

/**
 * @brief   Get the current display power mode.
 *
 * @param[in] g 		The display to use
 *
 * @return	The current power mode
 *
 * @api
 */
powermode_t gdispGGetPowerMode(GDisplay *g);
#define gdispGetPowerMode()							gdispGGetPowerMode(GDISP)

/**
 * @brief   Get the current display orientation.
 *
 * @param[in] g 		The display to use
 *
 * @return	The current orientation
 *
 * @api
 */
orientation_t gdispGGetOrientation(GDisplay *g);
#define gdispGetOrientation()						gdispGGetOrientation(GDISP)

/**
 * @brief   Get the current display backlight brightness.
 *
 * @param[in] g 		The display to use
 *
 * @return	The current backlight value
 *
 * @api
 */
uint8_t gdispGGetBacklight(GDisplay *g);
#define gdispGetBacklight()							gdispGGetBacklight(GDISP)

/**
 * @brief   Get the current display contrast.
 *
 * @param[in] g 		The display to use
 *
 * @return	The current contrast value
 *
 * @api
 */
uint8_t gdispGGetContrast(GDisplay *g);
#define gdispGetContrast()							gdispGGetContrast(GDISP)

/* Drawing Functions */

/**
 * @brief   Flush current drawing operations to the display
 * @note	Some low level drivers do not update the display until
 * 			the display is flushed. For others it is optional but can
 * 			help prevent tearing effects. For some it is ignored.
 * 			Calling it at the end of a logic set of drawing operations
 * 			in your application will ensure controller portability. If you
 * 			know your controller does not need to be flushed there is no
 * 			need to call it (which is in reality most controllers).
 * @note	Even for displays that require flushing, there is no need to
 * 			call this function if GDISP_NEED_AUTOFLUSH is TRUE.
 * 			Calling it again won't hurt though.
 *
 *
 * @param[in] g 	The display to use
 *
 * @api
 */
void gdispGFlush(GDisplay *g);
#define gdispFlush()									gdispGFlush(GDISP)

/**
 * @brief   Clear the display to the specified color.
 *
 * @param[in] g 	The display to use
 * @param[in] color The color to use when clearing the screen
 *
 * @api
 */
void gdispGClear(GDisplay *g, color_t color);
#define gdispClear(c)									gdispGClear(GDISP, c)

/**
 * @brief   Set a pixel in the specified color.
 *
 * @param[in] g 	The display to use
 * @param[in] x,y   The position to set the pixel.
 * @param[in] color The color to use
 *
 * @api
 */
void gdispGDrawPixel(GDisplay *g, coord_t x, coord_t y, color_t color);
#define gdispDrawPixel(x,y,c)							gdispGDrawPixel(GDISP,x,y,c)

/**
 * @brief   Draw a line.
 *
 * @param[in] g 	The display to use
 * @param[in] x0,y0		The start position
 * @param[in] x1,y1 	The end position
 * @param[in] color		The color to use
 *
 * @api
 */
void gdispGDrawLine(GDisplay *g, coord_t x0, coord_t y0, coord_t x1, coord_t y1, color_t color);
#define gdispDrawLine(x0,y0,x1,y1,c)					gdispGDrawLine(GDISP,x0,y0,x1,y1,c)

/**
 * @brief   Fill an area with a color.
 *
 * @param[in] g 		The display to use
 * @param[in] x,y		The start position
 * @param[in] cx,cy		The size of the box (outside dimensions)
 * @param[in] color		The color to use
 *
 * @api
 */
void gdispGFillArea(GDisplay *g, coord_t x, coord_t y, coord_t cx, coord_t cy, color_t color);
#define gdispFillArea(x,y,cx,cy,c)						gdispGFillArea(GDISP,x,y,cx,cy,c)

/**
 * @brief   Fill an area using the supplied bitmap.
 * @details The bitmap is in the pixel format specified by the low level driver
 * @note	If a packed pixel format is used and the width doesn't
 *			match a whole number of bytes, the next line will start on a
 *			non-byte boundary (no end-of-line padding).
 * @note	If GDISP_NEED_ASYNC is defined then the buffer must be static
 * 			or at least retained until this call has finished the blit. You can
 * 			tell when all graphics drawing is finished by @p gdispIsBusy() going FALSE.
 *
 * @param[in] g 		The display to use
 * @param[in] x,y		The start position
 * @param[in] cx,cy		The size of the filled area
 * @param[in] srcx,srcy The bitmap position to start the fill form
 * @param[in] srccx		The width of a line in the bitmap
 * @param[in] buffer	The bitmap in the driver's pixel format
 *
 * @api
 */
void gdispGBlitArea(GDisplay *g, coord_t x, coord_t y, coord_t cx, coord_t cy, coord_t srcx, coord_t srcy, coord_t srccx, const pixel_t *buffer);
#define gdispBlitAreaEx(x,y,cx,cy,sx,sy,rx,b)			gdispGBlitArea(GDISP,x,y,cx,cy,sx,sy,rx,b)

/**
 * @brief   Draw a rectangular box.
 *
 * @param[in] g 		The display to use
 * @param[in] x,y		The start position
 * @param[in] cx,cy		The size of the box (outside dimensions)
 * @param[in] color		The color to use
 *
 * @api
 */
void gdispGDrawBox(GDisplay *g, coord_t x, coord_t y, coord_t cx, coord_t cy, color_t color);
#define gdispDrawBox(x,y,cx,cy,c)						gdispGDrawBox(GDISP,x,y,cx,cy,c)

/* Streaming Functions */

#if GDISP_NEED_STREAMING || defined(__DOXYGEN__)
	/**
	 * @brief   Start a streaming operation.
	 * @details Stream data to a window on the display sequentially and very fast.
	 * @pre		GDISP_NEED_STREAMING must be TRUE in your gfxconf.h
	 * @note	While streaming is in operation - no other calls to GDISP functions
	 * 			can be made (with the exception of @p gdispBlendColor() and streaming
	 * 			functions). If a call is made (eg in a multi-threaded application) the other
	 * 			call is blocked waiting for the streaming operation to finish.
	 * @note	@p gdispStreamStop() must be called to finish the streaming operation.
	 * @note	If more data is written than the defined area then the results are unspecified.
	 * 			Some drivers may wrap back to the beginning of the area, others may just
	 * 			ignore subsequent data.
	 * @note	Unlike most operations that clip the defined area to the display to generate
	 * 			a smaller active area, this call will just silently fail if any of the stream
	 * 			region lies outside the current clipping area.
	 * @note	A streaming operation may be terminated early (without writing to every location
	 * 			in the stream area) by calling @p gdispStreamStop().
	 *
	 * @param[in] g 		The display to use
	 * @param[in] x,y		The start position
	 * @param[in] cx,cy		The size of the streamable area
	 *
	 * @api
	 */
	void gdispGStreamStart(GDisplay *g, coord_t x, coord_t y, coord_t cx, coord_t cy);
	#define gdispStreamStart(x,y,cx,cy)						gdispGStreamStart(GDISP,x,y,cx,cy)

	/**
	 * @brief   Send pixel data to the stream.
	 * @details Write a pixel to the next position in the streamed area and increment the position
	 * @pre		GDISP_NEED_STREAMING must be TRUE in your gfxconf.h
	 * @pre		@p gdispStreamStart() has been called.
	 * @note	If the gdispStreamStart() has not been called (or failed due to clipping), the
	 * 			data provided here is simply thrown away.
	 *
	 * @param[in] g 		The display to use
	 * @param[in] color		The color of the pixel to write
	 *
	 * @api
	 */
	void gdispGStreamColor(GDisplay *g, color_t color);
	#define gdispStreamColor(c)								gdispGStreamColor(GDISP,c)

	/**
	 * @brief   Finish the current streaming operation.
	 * @details	Completes the current streaming operation and allows other GDISP calls to operate again.
	 * @pre		GDISP_NEED_STREAMING must be TRUE in your gfxconf.h
	 * @pre		@p gdispStreamStart() has been called.
	 * @note	If the gdispStreamStart() has not been called (or failed due to clipping), this
	 * 			call is simply ignored.
	 *
	 * @param[in] g 		The display to use
	 *
	 * @api
	 */
	void gdispGStreamStop(GDisplay *g);
	#define gdispStreamStop()								gdispGStreamStop(GDISP)
#endif

/* Clipping Functions */

#if GDISP_NEED_CLIP || defined(__DOXYGEN__)
	/**
	 * @brief   Clip all drawing to the defined area.
	 * @pre		GDISP_NEED_CLIP must be TRUE in your gfxconf.h
	 *
	 * @param[in] g 		The display to use
	 * @param[in] x,y		The start position
	 * @param[in] cx,cy		The size of the clip area
	 *
	 * @api
	 */
	void gdispGSetClip(GDisplay *g, coord_t x, coord_t y, coord_t cx, coord_t cy);
	#define gdispSetClip(x,y,cx,cy)							gdispGSetClip(GDISP,x,y,cx,cy)
#endif

/* Circle Functions */

#if GDISP_NEED_CIRCLE || defined(__DOXYGEN__)
	/**
	 * @brief   Draw a circle.
	 * @pre		GDISP_NEED_CIRCLE must be TRUE in your gfxconf.h
	 *
	 * @param[in] g 		The display to use
	 * @param[in] x,y		The center of the circle
	 * @param[in] radius	The radius of the circle
	 * @param[in] color		The color to use
	 *
	 * @api
	 */
	void gdispGDrawCircle(GDisplay *g, coord_t x, coord_t y, coord_t radius, color_t color);
	#define gdispDrawCircle(x,y,r,c)						gdispGDrawCircle(GDISP,x,y,r,c)

	/**
	 * @brief   Draw a filled circle.
	 * @pre		GDISP_NEED_CIRCLE must be TRUE in your gfxconf.h
	 *
	 * @param[in] g 		The display to use
	 * @param[in] x,y		The center of the circle
	 * @param[in] radius	The radius of the circle
	 * @param[in] color		The color to use
	 *
	 * @api
	 */
	void gdispGFillCircle(GDisplay *g, coord_t x, coord_t y, coord_t radius, color_t color);
	#define gdispFillCircle(x,y,r,c)						gdispGFillCircle(GDISP,x,y,r,c)
#endif

/* Ellipse Functions */

#if GDISP_NEED_ELLIPSE || defined(__DOXYGEN__)
	/**
	 * @brief   Draw an ellipse.
	 * @pre		GDISP_NEED_ELLIPSE must be TRUE in your gfxconf.h
	 *
	 * @param[in] g 		The display to use
	 * @param[in] x,y		The center of the ellipse
	 * @param[in] a,b		The dimensions of the ellipse
	 * @param[in] color		The color to use
	 *
	 * @api
	 */
	void gdispGDrawEllipse(GDisplay *g, coord_t x, coord_t y, coord_t a, coord_t b, color_t color);
	#define gdispDrawEllipse(x,y,a,b,c)						gdispGDrawEllipse(GDISP,x,y,a,b,c)

	/**
	 * @brief   Draw a filled ellipse.
	 * @pre		GDISP_NEED_ELLIPSE must be TRUE in your gfxconf.h
	 *
	 * @param[in] g 		The display to use
	 * @param[in] x,y		The center of the ellipse
	 * @param[in] a,b		The dimensions of the ellipse
	 * @param[in] color		The color to use
	 *
	 * @api
	 */
	void gdispGFillEllipse(GDisplay *g, coord_t x, coord_t y, coord_t a, coord_t b, color_t color);
	#define gdispFillEllipse(x,y,a,b,c)						gdispGFillEllipse(GDISP,x,y,a,b,c)
#endif

/* Arc Functions */
#if GDISP_NEED_ARCSECTORS || defined(__DOXYGEN__)
	/**
	 * @brief	Draw a selection of 45 degree arcs of a circle
	 * @pre		GDISP_NEED_ARCSECTORS must be TRUE in your gfxconf.h
	 *
	 * @param[in] g 		The display to use
	 * @param[in] x,y		The center of the circle
	 * @param[in] radius	The radius of the circle
	 * @param[in] sectors	Bits determine which sectors are drawn.
	 * 						Bits go anti-clockwise from the 0 degree mark (y = 0, x is positive), as follows:
	 *  						bit 0 - upper right right		  -----
	 *  						bit 1 - upper upper right		 /2   1\
	 *  						bit 2 - upper upper left		/3     0\
	 *  						bit 3 - upper left  left		\4     7/
	 *  						bit 4 - lower left  left		 \5   6/
	 *  						bit 5 - lower lower left		  -----
	 *  						bit 6 - lower lower right
	 *  						bit 7 - lower left  left
	 * @param[in] color		The color to use
	 *
	 * @note	This is a more limited versions of the general arc drawing routine. It
	 * 			doesn't require trig libraries or tables or floating point and is smaller in code size.
	 * 			There is probably little point in including both this and the general
	 * 			arc routine as the general arc routine can do everything this can do.
	 *
	 * @api
	 */
	void gdispGDrawArcSectors(GDisplay *g, coord_t x, coord_t y, coord_t radius, uint8_t sectors, color_t color);
	#define gdispDrawArcSectors(x,y,r,s,c)						gdispGDrawArcSectors(GDISP,x,y,r,s,c)

	/**
	 * @brief	Fill a selection of 45 degree arcs of a circle
	 * @pre		GDISP_NEED_ARCSECTORS must be TRUE in your gfxconf.h
	 *
	 * @param[in] g 		The display to use
	 * @param[in] x,y		The center of the circle
	 * @param[in] radius	The radius of the circle
	 * @param[in] sectors	Bits determine which sectors are drawn.
	 * 						Bits go anti-clockwise from the 0 degree mark (y = 0, x is positive), as follows:
	 *  						bit 0 - upper right right		  -----
	 *  						bit 1 - upper upper right		 /2   1\
	 *  						bit 2 - upper upper left		/3     0\
	 *  						bit 3 - upper left  left		\4     7/
	 *  						bit 4 - lower left  left		 \5   6/
	 *  						bit 5 - lower lower left		  -----
	 *  						bit 6 - lower lower right
	 *  						bit 7 - lower left  left
	 * @param[in] color		The color to use
	 *
	 * @note	This is a more limited versions of the general arc filling routine. It
	 * 			doesn't require trig libraries or tables or floating point and is smaller in code size.
	 * 			There is probably little point in including both this and the general
	 * 			arc routine as the general arc routine can do everything this can do.
	 *
	 * @api
	 */
	void gdispGFillArcSectors(GDisplay *g, coord_t x, coord_t y, coord_t radius, uint8_t sectors, color_t color);
	#define gdispFillArcSectors(x,y,r,s,c)						gdispGFillArcSectors(GDISP,x,y,r,s,c)
#endif

#if GDISP_NEED_ARC || defined(__DOXYGEN__)
	/*
	 * @brief	Draw an arc.
	 * @pre		GDISP_NEED_ARC must be TRUE in your gfxconf.h
	 *
	 * @param[in] g 		The display to use
	 * @param[in] x0,y0		The center point
	 * @param[in] radius	The radius of the arc
	 * @param[in] start		The start angle (0 to 360)
	 * @param[in] end		The end angle (0 to 360)
	 * @param[in] color		The color of the arc
	 *
	 * @note		If you are just doing 45 degree angles consider using @p gdispDrawArcSectors() instead.
	 * @note		This routine requires trig support. It can either come from your C runtime library
	 * 				cos() and sin() which requires floating point support (and is slow), or you can define GFX_USE_GMISC
	 * 				and either GMISC_NEED_FIXEDTRIG or GMISC_NEED_FASTTRIG.
	 * 				GMISC_NEED_FASTTRIG uses table based floating point trig operations.
	 * 				GMISC_NEED_FIXEDTRIG uses fixed point integer trig operations.
	 * 				Note accuracy on both the table based options are more than adequate for the one degree
	 * 				resolution provided by these arc routines. Both are much faster than your C runtime library.
	 *
	 * @api
	 */
	void gdispGDrawArc(GDisplay *g, coord_t x, coord_t y, coord_t radius, coord_t startangle, coord_t endangle, color_t color);
	#define gdispDrawArc(x,y,r,s,e,c)						gdispGDrawArc(GDISP,x,y,r,s,e,c)

	/*
	 * @brief	Draw a filled arc.
	 * @pre		GDISP_NEED_ARC must be TRUE in your gfxconf.h
	 *
	 * @param[in] g 		The display to use
	 * @param[in] x0,y0		The center point
	 * @param[in] radius	The radius of the arc
	 * @param[in] start		The start angle (0 to 360)
	 * @param[in] end		The end angle (0 to 360)
	 * @param[in] color		The color of the arc
	 *
	 * @note		If you are just doing 45 degree angles consider using @p gdispFillArcSectors() instead.
	 * @note		This routine requires trig support. It can either come from your C runtime library
	 * 				cos() and sin() which requires floating point support (and is slow), or you can define GFX_USE_GMISC
	 * 				and either GMISC_NEED_FIXEDTRIG or GMISC_NEED_FASTTRIG.
	 * 				GMISC_NEED_FASTTRIG uses table based floating point trig operations.
	 * 				GMISC_NEED_FIXEDTRIG uses fixed point integer trig operations.
	 * 				Note accuracy on both the table based options are more than adequate for the one degree
	 * 				resolution provided by these arc routines. Both are much faster than your C runtime library.
	 *
	 * @api
	 */
	void gdispGFillArc(GDisplay *g, coord_t x, coord_t y, coord_t radius, coord_t startangle, coord_t endangle, color_t color);
	#define gdispFillArc(x,y,r,s,e,c)						gdispGFillArc(GDISP,x,y,r,s,e,c)
#endif

/* Read a pixel Function */

#if GDISP_NEED_PIXELREAD || defined(__DOXYGEN__)
	/**
	 * @brief   Get the color of a pixel.
	 * @return  The color of the pixel.
	 * @pre		GDISP_NEED_PIXELREAD must be TRUE in your gfxconf.h
	 *
	 * @param[in] g 		The display to use
	 * @param[in] x,y		The position of the pixel
	 *
	 * @api
	 */
	color_t gdispGGetPixelColor(GDisplay *g, coord_t x, coord_t y);
	#define gdispGetPixelColor(x,y)							gdispGGetPixelColor(GDISP,x,y)
#endif

/* Scrolling Function - clears the area scrolled out */

#if GDISP_NEED_SCROLL || defined(__DOXYGEN__)
	/**
	 * @brief   Scroll vertically a section of the screen.
	 * @pre		GDISP_NEED_SCROLL must be set to TRUE in gfxconf.h
	 * @note    Optional.
	 * @note    If lines is >= cy, it is equivelent to an area fill with bgcolor.
	 *
	 * @param[in] g 		The display to use
	 * @param[in] x, y		The start of the area to be scrolled
	 * @param[in] cx, cy	The size of the area to be scrolled
	 * @param[in] lines		The number of lines to scroll (Can be positive or negative)
	 * @param[in] bgcolor	The color to fill the newly exposed area.
	 *
	 * @api
	 */
	void gdispGVerticalScroll(GDisplay *g, coord_t x, coord_t y, coord_t cx, coord_t cy, int lines, color_t bgcolor);
	#define gdispVerticalScroll(x,y,cx,cy,l,b)				gdispGVerticalScroll(GDISP,x,y,cx,cy,l,b)
#endif

/* Set driver specific control */

#if GDISP_NEED_CONTROL || defined(__DOXYGEN__)
	/**
	 * @brief   Control hardware specific parts of the display. eg powermodes, backlight etc
	 * @pre		GDISP_NEED_CONTROL must be TRUE in your gfxconf.h
	 * @note    Depending on the hardware implementation this function may not
	 *          support some codes. They will be ignored.
	 *
	 * @param[in] g 		The display to use
	 * @param[in] what		what you want to control
	 * @param[in] value		The value to be assigned
	 *
	 * @api
	 */
	void gdispGControl(GDisplay *g, unsigned what, void *value);
	#define gdispControl(w,v)								gdispGControl(GDISP,w,v)
#endif

/* Query driver specific data */

#if GDISP_NEED_QUERY || defined(__DOXYGEN__)
	/**
	 * @brief   Query a property of the display.
	 * @pre		GDISP_NEED_QUERY must be TRUE in your gfxconf.h
	 * @note    The result must be typecast to the correct type.
	 * @note    An unsupported query will return (void *)-1.
	 *
	 * @param[in] g 		The display to use
	 * @param[in] what		What to query
	 *
	 * @api
	 */
	void *gdispGQuery(GDisplay *g, unsigned what);
	#define gdispQuery(w)									gdispGQuery(GDISP,w)
#endif

#if GDISP_NEED_CONVEX_POLYGON || defined(__DOXYGEN__)
	/**
	 * @brief   Draw an enclosed polygon (convex, non-convex or complex).
	 * @pre		GDISP_NEED_CONVEX_POLYGON must be TRUE in your gfxconf.h
	 *
	 * @param[in] g 		The display to use
	 * @param[in] tx, ty	Transform all points in pntarray by tx, ty
	 * @param[in] pntarray	An array of points
	 * @param[in] cnt		The number of points in the array
	 * @param[in] color		The color to use
	 *
	 * @api
	 */
	void gdispGDrawPoly(GDisplay *g, coord_t tx, coord_t ty, const point *pntarray, unsigned cnt, color_t color);
	#define gdispDrawPoly(x,y,p,i,c)						gdispGDrawPoly(GDISP,x,y,p,i,c)

	/**
	 * @brief   Fill a convex polygon
	 * @details Doesn't handle non-convex or complex polygons.
	 * @pre		GDISP_NEED_CONVEX_POLYGON must be TRUE in your gfxconf.h
	 *
	 * @param[in] g 		The display to use
	 * @param[in] tx, ty	Transform all points in pntarray by tx, ty
	 * @param[in] pntarray	An array of points
	 * @param[in] cnt		The number of points in the array
	 * @param[in] color		The color to use
	 *
	 * @note	Convex polygons are those that have no internal angles. That is;
	 * 			you can draw a line from any point on the polygon to any other point
	 * 			on the polygon without it going outside the polygon. In our case we generalise
	 * 			this a little by saying that an infinite horizontal line (at any y value) will cross
	 * 			no more than two edges on the polygon. Some non-convex polygons do fit this criteria
	 * 			and can therefore be drawn.
	 * @note	This routine is designed to be very efficient with even simple display hardware.
	 *
	 * @api
	 */
	void gdispGFillConvexPoly(GDisplay *g, coord_t tx, coord_t ty, const point *pntarray, unsigned cnt, color_t color);
	#define gdispFillConvexPoly(x,y,p,i,c)					gdispGFillConvexPoly(GDISP,x,y,p,i,c)

	/**
	 * @brief   Draw a line with a specified thickness
	 * @details The line thickness is specified in pixels. The line ends can
	 *          be selected to be either flat or round.
	 * @pre		GDISP_NEED_CONVEX_POLYGON must be TRUE in your gfxconf.h
	 * @note	Uses gdispGFillConvexPoly() internally to perform the drawing.
	 *
	 * @param[in] g			The display to use
	 * @param[in] x0,y0		The start position
	 * @param[in] x1,y1		The end position
	 * @param[in] color		The color to use
	 * @param[in] width		The width of the line
	 * @param[in] round		Use round ends for the line
	 *
	 * @api
	 */
	void gdispGDrawThickLine(GDisplay *g, coord_t x0, coord_t y0, coord_t x1, coord_t y1, color_t color, coord_t width, bool_t round);
	#define gdispDrawThickLine(x0,y0,x1,y1,c,w,r)			gdispGDrawThickLine(GDISP,x0,y0,x1,y1,c,w,r)
#endif

/* Text Functions */

#if GDISP_NEED_TEXT || defined(__DOXYGEN__)
	/**
	 * @brief   Draw a text character.
	 * @pre		GDISP_NEED_TEXT must be TRUE in your gfxconf.h
	 *
	 * @param[in] g 		The display to use
	 * @param[in] x,y		The position for the text
	 * @param[in] c			The character to draw
	 * @param[in] font		The font to use
	 * @param[in] color		The color to use
	 *
	 * @api
	 */
	void gdispGDrawChar(GDisplay *g, coord_t x, coord_t y, uint16_t c, font_t font, color_t color);
	#define	gdispDrawChar(x,y,s,f,c)						gdispGDrawChar(GDISP,x,y,s,f,c)

	/**
	 * @brief   Draw a text character with a filled background.
	 * @pre		GDISP_NEED_TEXT must be TRUE in your gfxconf.h
	 *
	 * @param[in] g 		The display to use
	 * @param[in] x,y		The position for the text
	 * @param[in] c			The character to draw
	 * @param[in] font		The font to use
	 * @param[in] color		The color to use
	 * @param[in] bgcolor	The background color to use
	 *
	 * @api
	 */
	void gdispGFillChar(GDisplay *g, coord_t x, coord_t y, uint16_t c, font_t font, color_t color, color_t bgcolor);
	#define	gdispFillChar(x,y,s,f,c,b)						gdispGFillChar(GDISP,x,y,s,f,c,b)

	/**
	 * @brief   Draw a text string.
	 * @pre		GDISP_NEED_TEXT must be TRUE in your gfxconf.h
	 *
	 * @param[in] g 		The display to use
	 * @param[in] x,y		The position for the text
	 * @param[in] str		The string to draw
	 * @param[in] font		The font to use
	 * @param[in] color		The color to use
	 *
	 * @api
	 */
	void gdispGDrawString(GDisplay *g, coord_t x, coord_t y, const char *str, font_t font, color_t color);
	#define	gdispDrawString(x,y,s,f,c)						gdispGDrawString(GDISP,x,y,s,f,c)

	/**
	 * @brief   Draw a text string.
	 * @pre		GDISP_NEED_TEXT must be TRUE in your gfxconf.h
	 *
	 * @param[in] g 		The display to use
	 * @param[in] x,y		The position for the text
	 * @param[in] str		The string to draw
	 * @param[in] font		The font to use
	 * @param[in] color		The color to use
	 * @param[in] bgcolor	The background color to use
	 *
	 * @api
	 */
	void gdispGFillString(GDisplay *g, coord_t x, coord_t y, const char *str, font_t font, color_t color, color_t bgcolor);
	#define	gdispFillString(x,y,s,f,c,b)					gdispGFillString(GDISP,x,y,s,f,c,b)

	/**
	 * @brief   Draw a text string vertically centered within the specified box.
	 * @pre		GDISP_NEED_TEXT must be TRUE in your gfxconf.h
	 *
	 * @param[in] g 		The display to use
	 * @param[in] x,y		The position for the text (need to define top-right or base-line - check code)
	 * @param[in] cx,cy		The width and height of the box
	 * @param[in] str		The string to draw
	 * @param[in] font		The font to use
	 * @param[in] color		The color to use
	 * @param[in] justify	Justify the text left, center or right within the box
	 *
	 * @api
	 */
	void gdispGDrawStringBox(GDisplay *g, coord_t x, coord_t y, coord_t cx, coord_t cy, const char* str, font_t font, color_t color, justify_t justify);
	#define	gdispDrawStringBox(x,y,cx,cy,s,f,c,j)			gdispGDrawStringBox(GDISP,x,y,cx,cy,s,f,c,j)
	void gdispGDrawStringBoxNoWrap(GDisplay *g, coord_t x, coord_t y, coord_t cx, coord_t cy, const char* str, font_t font, color_t color, justify_t justify);
	#define	gdispDrawStringBoxNoWrap(x,y,cx,cy,s,f,c,j)		gdispGDrawStringBoxNoWrap(GDISP,x,y,cx,cy,s,f,c,j)

	
	/**
	 * @brief   Draw a text string vertically centered within the specified box. The box background is filled with the specified background color.
	 * @pre		GDISP_NEED_TEXT must be TRUE in your gfxconf.h
	 * @note    The entire box is filled
	 *
	 * @param[in] g 		The display to use
	 * @param[in] x,y		The position for the text (need to define top-right or base-line - check code)
	 * @param[in] cx,cy		The width and height of the box
	 * @param[in] str		The string to draw
	 * @param[in] font		The font to use
	 * @param[in] color		The color to use
	 * @param[in] bgColor	The background color to use
	 * @param[in] justify	Justify the text left, center or right within the box
	 *
	 * @api
	 */
	void gdispGFillStringBox(GDisplay *g, coord_t x, coord_t y, coord_t cx, coord_t cy, const char* str, font_t font, color_t color, color_t bgColor, justify_t justify);
	#define	gdispFillStringBox(x,y,cx,cy,s,f,c,b,j)			gdispGFillStringBox(GDISP,x,y,cx,cy,s,f,c,b,j)
	void gdispGFillStringBoxNoWrap(GDisplay *g, coord_t x, coord_t y, coord_t cx, coord_t cy, const char* str, font_t font, color_t color, color_t bgColor, justify_t justify);
	#define	gdispFillStringBoxNoWrap(x,y,cx,cy,s,f,c,b,j)	gdispGFillStringBoxNoWrap(GDISP,x,y,cx,cy,s,f,c,b,j)

	/**
	 * @brief   Get a metric of a font.
	 * @return  The metric requested in pixels.
	 * @pre		GDISP_NEED_TEXT must be TRUE in your gfxconf.h
	 *
	 * @param[in] font    The font to test
	 * @param[in] metric  The metric to measure
	 *
	 * @api
	 */
	coord_t gdispGetFontMetric(font_t font, fontmetric_t metric);

	/**
	 * @brief   Get the pixel width of a character.
	 * @return  The width of the character in pixels. Does not include any between character padding.
	 * @pre		GDISP_NEED_TEXT must be TRUE in your gfxconf.h
	 *
	 * @param[in] c       The character to draw
	 * @param[in] font    The font to use
	 *
	 * @api
	 */
	coord_t gdispGetCharWidth(char c, font_t font);

	/**
	 * @brief   Get the pixel width of a string of a given character length.
	 * @return  The width of the string in pixels.
	 * @pre		GDISP_NEED_TEXT must be TRUE in your gfxconf.h
	 *
	 * @note	Passing 0 to count has the same effect as calling gdispGetStringWidt()
	 *
	 * @param[in] str     The string to measure
	 * @param[in] font    The font to use
	 * @param[in] count   The number of characters to take into account
	 *
	 * @api
	 */
	coord_t gdispGetStringWidthCount(const char* str, font_t font, uint16_t count);

	/**
	 * @brief   Get the pixel width of an entire string.
	 * @return  The width of the string in pixels.
	 * @pre		GDISP_NEED_TEXT must be TRUE in your gfxconf.h
	 *
	 * @param[in] str     The string to measure
	 * @param[in] font    The font to use
	 *
	 * @api
	 */
	coord_t gdispGetStringWidth(const char* str, font_t font);

	/**
	 * @brief	Find a font and return it.
	 * @details	The supplied name is matched against the font name. A '*' will replace 0 or more characters.
	 * @return	Returns a font or NULL if no matching font could be found.
	 * @pre		GDISP_NEED_TEXT must be TRUE in your gfxconf.h
	 *
	 * @param[in] name		The font name to find.
	 *
	 * @note				Wildcard matching will match the shortest possible match.
	 *
	 * @api
	 */
	font_t gdispOpenFont(const char *name);

	/**
	 * @brief	Release a font after use.
	 * @pre		GDISP_NEED_TEXT must be TRUE in your gfxconf.h
	 *
	 * @param[in] font		The font to release.
	 *
	 * @api
	 */
	void gdispCloseFont(font_t font);

	/**
	 * @brief	Make a scaled copy of an existing font.
	 * @details	Allocates memory for new font metadata using gfxAlloc, remember to close font after use!
	 * @return	A new font or NULL if out of memory.
	 * @pre		GDISP_NEED_TEXT must be TRUE in your gfxconf.h
	 *
	 * @param[in] font	The base font to use.
	 * @param[in] scale_x	The scale factor in horizontal direction.
	 * @param[in] scale_y	The scale factor in vertical direction.
	 */
	font_t gdispScaleFont(font_t font, uint8_t scale_x, uint8_t scale_y);

	/**
	 * @brief	Get the name of the specified font.
	 * @returns	The name of the font.
	 * @pre		GDISP_NEED_TEXT must be TRUE in your gfxconf.h
	 *
	 * @param[in] font		The font to get the name for.
	 *
	 * @api
	 */
	const char *gdispGetFontName(font_t font);
#endif

/* Extra Arc Functions */

#if GDISP_NEED_ARC || GDISP_NEED_ARCSECTORS || defined(__DOXYGEN__)
	/**
	 * @brief   Draw a rectangular box with rounded corners
	 * @pre		GDISP_NEED_ARC or GDISP_NEED_ARCSECTORS must be TRUE in your gfxconf.h
	 *
	 * @param[in] g 		The display to use
	 * @param[in] x,y		The start position
	 * @param[in] cx,cy		The size of the box (outside dimensions)
	 * @param[in] radius	The radius of the rounded corners
	 * @param[in] color		The color to use
	 *
	 * @api
	 */
	void gdispGDrawRoundedBox(GDisplay *g, coord_t x, coord_t y, coord_t cx, coord_t cy, coord_t radius, color_t color);
	#define gdispDrawRoundedBox(x,y,cx,cy,r,c)		gdispGDrawRoundedBox(GDISP,x,y,cx,cy,r,c)

	/**
	 * @brief   Draw a filled rectangular box with rounded corners
	 * @pre		GDISP_NEED_ARC or GDISP_NEED_ARCSECTORS must be TRUE in your gfxconf.h
	 *
	 * @param[in] g 		The display to use
	 * @param[in] x,y		The start position
	 * @param[in] cx,cy		The size of the box (outside dimensions)
	 * @param[in] radius	The radius of the rounded corners
	 * @param[in] color		The color to use
	 *
	 * @api
	 */
	void gdispGFillRoundedBox(GDisplay *g, coord_t x, coord_t y, coord_t cx, coord_t cy, coord_t radius, color_t color);
	#define gdispFillRoundedBox(x,y,cx,cy,r,c)		gdispGFillRoundedBox(GDISP,x,y,cx,cy,r,c)
#endif

/*
 * Macro definitions
 */

/* Now obsolete functions */
#define gdispBlitArea(x, y, cx, cy, buffer)			gdispGBlitArea(GDISP, x, y, cx, cy, 0, 0, cx, buffer)

/* Macro definitions for common gets and sets */

/**
 * @brief   Set the display power mode.
 * @note    Ignored if not supported by the display.
 *
 * @param[in] g 			The display to use
 * @param[in] powerMode		The new power mode
 *
 * @api
 */
#define gdispGSetPowerMode(g, powerMode)			gdispGControl((g), GDISP_CONTROL_POWER, (void *)(unsigned)(powerMode))
#define gdispSetPowerMode(powerMode)				gdispGControl(GDISP, GDISP_CONTROL_POWER, (void *)(unsigned)(powerMode))

/**
 * @brief   Set the display orientation.
 * @note    Ignored if not supported by the display.
 *
 * @param[in] g 					The display to use
 * @param[in] newOrientation		The new orientation
 *
 * @api
 */
#define gdispGSetOrientation(g, newOrientation)		gdispGControl((g), GDISP_CONTROL_ORIENTATION, (void *)(unsigned)(newOrientation))
#define gdispSetOrientation(newOrientation)			gdispGControl(GDISP, GDISP_CONTROL_ORIENTATION, (void *)(unsigned)(newOrientation))

/**
 * @brief   Set the display backlight.
 * @note    Ignored if not supported by the display.
 *
 * @param[in] g 			The display to use
 * @param[in] percent		The new brightness (0 - 100%)
 *
 * @note	For displays that only support backlight off and on,
 * 			0 = off, anything else = on
 *
 * @api
 */
#define gdispGSetBacklight(g, percent)				gdispGControl((g), GDISP_CONTROL_BACKLIGHT, (void *)(unsigned)(percent))
#define gdispSetBacklight(percent)					gdispGControl(GDISP, GDISP_CONTROL_BACKLIGHT, (void *)(unsigned)(percent))

/**
 * @brief   Set the display contrast.
 * @note    Ignored if not supported by the display.
 *
 * @param[in] g 			The display to use
 * @param[in] percent		The new contrast (0 - 100%)
 *
 * @api
 */
#define gdispGSetContrast(g, percent)				gdispGControl((g), GDISP_CONTROL_CONTRAST, (void *)(unsigned)(percent))
#define gdispSetContrast(percent)					gdispGControl(GDISP, GDISP_CONTROL_CONTRAST, (void *)(unsigned)(percent))

/* More interesting macros */

/**
 * @brief   Reset the clip area to the full screen
 *
 * @param[in] g 		The display to use
 *
 * @api
 */
#define gdispGUnsetClip(g)							gdispGSetClip((g),0,0,gdispGGetWidth(g),gdispGGetHeight(g))
#define gdispUnsetClip()							gdispGUnsetClip(GDISP)

#ifdef __cplusplus
}
#endif

#if GDISP_NEED_IMAGE || defined(__DOXYGEN__)
	#include "gdisp_image.h"
#endif
#if GDISP_NEED_PIXMAP || defined(__DOXYGEN__)
	#include "gdisp_pixmap.h"
#endif


#endif /* GFX_USE_GDISP */

#endif /* _GDISP_H */
/** @} */
