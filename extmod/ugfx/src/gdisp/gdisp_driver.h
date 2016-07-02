/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/gdisp/gdisp_driver.h
 * @brief   GDISP Graphic Driver subsystem low level driver header.
 *
 * @addtogroup GDISP
 * @{
 */

#ifndef _GDISP_LLD_H
#define _GDISP_LLD_H

#if GFX_USE_GDISP

// Include the GDRIVER infrastructure
#include "../gdriver/gdriver.h"

// Are we currently compiling the driver itself?
#if defined(GDISP_DRIVER_VMT)
	#define	IN_DRIVER			TRUE
#else
	#define	IN_DRIVER			FALSE
#endif

// Is this a multiple driver situation?
#if defined(GDISP_DRIVER_LIST)
	#define IS_MULTIPLE			TRUE
#else
	#define IS_MULTIPLE			FALSE
#endif

// Do we need to use VMT calling rather than direct calls to the driver?
#if IS_MULTIPLE || GDISP_NEED_PIXMAP
	#define USE_VMT				TRUE
#else
	#define USE_VMT				FALSE
#endif

// Are we in the pixmap virtual driver
#ifndef IN_PIXMAP_DRIVER
	#define IN_PIXMAP_DRIVER	FALSE
#endif

//------------------------------------------------------------------------------------------------------------

// Our special auto-detect hardware code which uses the VMT.
#define HARDWARE_AUTODETECT		2

#if USE_VMT && !IN_DRIVER
	// Multiple controllers the default is to hardware detect
	#define HARDWARE_DEFAULT		HARDWARE_AUTODETECT
#else
	// The default is not to include code functions that aren't needed
	#define HARDWARE_DEFAULT		FALSE
#endif

//------------------------------------------------------------------------------------------------------------

/**
 * @name    GDISP hardware accelerated support
 * @{
 */
	/**
	 * @brief   The display hardware can benefit from being de-initialized when usage is complete.
	 * @details Can be set to TRUE, FALSE or HARDWARE_AUTODETECT
	 *
	 * @note	HARDWARE_AUTODETECT is only meaningful when GDISP_DRIVER_LIST is defined
	 * @note	This is most useful for displays such as remote network displays.
	 */
	#ifndef GDISP_HARDWARE_DEINIT
		#define GDISP_HARDWARE_DEINIT		HARDWARE_DEFAULT
	#endif

	/**
	 * @brief   The display hardware can benefit from being flushed.
	 * @details Can be set to TRUE, FALSE or HARDWARE_AUTODETECT
	 *
	 * @note	HARDWARE_AUTODETECT is only meaningful when GDISP_DRIVER_LIST is defined
	 * @note	Some controllers ** require ** the application to flush
	 */
	#ifndef GDISP_HARDWARE_FLUSH
		#define GDISP_HARDWARE_FLUSH		HARDWARE_DEFAULT
	#endif

	/**
	 * @brief   Hardware streaming writing is supported.
	 * @details Can be set to TRUE, FALSE or HARDWARE_AUTODETECT
	 *
	 * @note	HARDWARE_AUTODETECT is only meaningful when GDISP_DRIVER_LIST is defined
	 * @note	Either GDISP_HARDWARE_STREAM_WRITE or GDISP_HARDWARE_DRAWPIXEL must be provided by each driver
	 */
	#ifndef GDISP_HARDWARE_STREAM_WRITE
		#define GDISP_HARDWARE_STREAM_WRITE		HARDWARE_DEFAULT
	#endif

	/**
	 * @brief   Hardware streaming reading of the display surface is supported.
	 * @details Can be set to TRUE, FALSE or HARDWARE_AUTODETECT
	 *
	 * @note	HARDWARE_AUTODETECT is only meaningful when GDISP_DRIVER_LIST is defined
	 *
	 */
	#ifndef GDISP_HARDWARE_STREAM_READ
		#define GDISP_HARDWARE_STREAM_READ		HARDWARE_DEFAULT
	#endif

	/**
	 * @brief   Hardware supports setting the cursor position within the stream window.
	 * @details Can be set to TRUE, FALSE or HARDWARE_AUTODETECT
	 *
	 * @note	HARDWARE_AUTODETECT is only meaningful when GDISP_DRIVER_LIST is defined
	 * @note	This is used to optimise setting of individual pixels within a stream window.
	 * 			It should therefore not be implemented unless it is cheaper than just setting
	 * 			a new window.
	 */
	#ifndef GDISP_HARDWARE_STREAM_POS
		#define GDISP_HARDWARE_STREAM_POS		HARDWARE_DEFAULT
	#endif

	/**
	 * @brief   Hardware accelerated draw pixel.
	 * @details Can be set to TRUE, FALSE or HARDWARE_AUTODETECT
	 *
	 * @note	HARDWARE_AUTODETECT is only meaningful when GDISP_DRIVER_LIST is defined
	 * @note	Either GDISP_HARDWARE_STREAM_WRITE or GDISP_HARDWARE_DRAWPIXEL must be provided by the driver
	 */
	#ifndef GDISP_HARDWARE_DRAWPIXEL
		#define GDISP_HARDWARE_DRAWPIXEL		HARDWARE_DEFAULT
	#endif

	/**
	 * @brief   Hardware accelerated screen clears.
	 * @details Can be set to TRUE, FALSE or HARDWARE_AUTODETECT
	 *
	 * @note	HARDWARE_AUTODETECT is only meaningful when GDISP_DRIVER_LIST is defined
	 * @note	This clears the entire display surface regardless of the clipping area currently set
	 */
	#ifndef GDISP_HARDWARE_CLEARS
		#define GDISP_HARDWARE_CLEARS			HARDWARE_DEFAULT
	#endif

	/**
	 * @brief   Hardware accelerated rectangular fills.
	 * @details Can be set to TRUE, FALSE or HARDWARE_AUTODETECT
	 *
	 * @note	HARDWARE_AUTODETECT is only meaningful when GDISP_DRIVER_LIST is defined
	 */
	#ifndef GDISP_HARDWARE_FILLS
		#define GDISP_HARDWARE_FILLS			HARDWARE_DEFAULT
	#endif

	/**
	 * @brief   Hardware accelerated fills from an image.
	 * @details Can be set to TRUE, FALSE or HARDWARE_AUTODETECT
	 *
	 * @note	HARDWARE_AUTODETECT is only meaningful when GDISP_DRIVER_LIST is defined
	 */
	#ifndef GDISP_HARDWARE_BITFILLS
		#define GDISP_HARDWARE_BITFILLS			HARDWARE_DEFAULT
	#endif

	/**
	 * @brief   Hardware accelerated scrolling.
	 * @details Can be set to TRUE, FALSE or HARDWARE_AUTODETECT
	 *
	 * @note	HARDWARE_AUTODETECT is only meaningful when GDISP_DRIVER_LIST is defined
	 */
	#ifndef GDISP_HARDWARE_SCROLL
		#define GDISP_HARDWARE_SCROLL			HARDWARE_DEFAULT
	#endif

	/**
	 * @brief   Reading back of pixel values.
	 * @details Can be set to TRUE, FALSE or HARDWARE_AUTODETECT
	 *
	 * @note	HARDWARE_AUTODETECT is only meaningful when GDISP_DRIVER_LIST is defined
	 */
	#ifndef GDISP_HARDWARE_PIXELREAD
		#define GDISP_HARDWARE_PIXELREAD		HARDWARE_DEFAULT
	#endif

	/**
	 * @brief   The driver supports one or more control commands.
	 * @details Can be set to TRUE, FALSE or HARDWARE_AUTODETECT
	 *
	 * @note	HARDWARE_AUTODETECT is only meaningful when GDISP_DRIVER_LIST is defined
	 */
	#ifndef GDISP_HARDWARE_CONTROL
		#define GDISP_HARDWARE_CONTROL			HARDWARE_DEFAULT
	#endif

	/**
	 * @brief   The driver supports a non-standard query.
	 * @details Can be set to TRUE, FALSE or HARDWARE_AUTODETECT
	 *
	 * @note	HARDWARE_AUTODETECT is only meaningful when GDISP_DRIVER_LIST is defined
	 */
	#ifndef GDISP_HARDWARE_QUERY
		#define GDISP_HARDWARE_QUERY			HARDWARE_DEFAULT
	#endif

	/**
	 * @brief   The driver supports a clipping in hardware.
	 * @details Can be set to TRUE, FALSE or HARDWARE_AUTODETECT
	 *
	 * @note	HARDWARE_AUTODETECT is only meaningful when GDISP_DRIVER_LIST is defined
	 * @note	If this is defined the driver must perform its own clipping on all calls to
	 * 			the driver and respond appropriately if a parameter is outside the display area.
	 * @note	If this is not defined then the software ensures that all calls to the
	 * 			driver do not exceed the display area (provided GDISP_NEED_CLIP or GDISP_NEED_VALIDATION
	 * 			has been set).
	 */
	#ifndef GDISP_HARDWARE_CLIP
		#define GDISP_HARDWARE_CLIP				HARDWARE_DEFAULT
	#endif
/** @} */

//------------------------------------------------------------------------------------------------------------

// For pixmaps certain routines MUST not be FALSE as they are needed for pixmap drawing
//	Similarly some routines MUST not be TRUE as pixmap's don't provide them.
#if GDISP_NEED_PIXMAP && !IN_DRIVER
	#if !GDISP_HARDWARE_DEINIT
		#undef GDISP_HARDWARE_DEINIT
		#define GDISP_HARDWARE_DEINIT		HARDWARE_AUTODETECT
	#endif
	#if !GDISP_HARDWARE_DRAWPIXEL
		#undef GDISP_HARDWARE_DRAWPIXEL
		#define GDISP_HARDWARE_DRAWPIXEL	HARDWARE_AUTODETECT
	#endif
	#if !GDISP_HARDWARE_PIXELREAD
		#undef GDISP_HARDWARE_PIXELREAD
		#define GDISP_HARDWARE_PIXELREAD	HARDWARE_AUTODETECT
	#endif
	#if !GDISP_HARDWARE_CONTROL
		#undef GDISP_HARDWARE_CONTROL
		#define GDISP_HARDWARE_CONTROL		HARDWARE_AUTODETECT
	#endif
	#if GDISP_HARDWARE_FLUSH == TRUE
		#undef GDISP_HARDWARE_FLUSH
		#define GDISP_HARDWARE_FLUSH		HARDWARE_AUTODETECT
	#endif
	#if GDISP_HARDWARE_STREAM_WRITE == TRUE
		#undef GDISP_HARDWARE_STREAM_WRITE
		#define GDISP_HARDWARE_STREAM_WRITE	HARDWARE_AUTODETECT
	#endif
	#if GDISP_HARDWARE_STREAM_READ == TRUE
		#undef GDISP_HARDWARE_STREAM_READ
		#define GDISP_HARDWARE_STREAM_READ	HARDWARE_AUTODETECT
	#endif
	#if GDISP_HARDWARE_CLEARS == TRUE
		#undef GDISP_HARDWARE_CLEARS
		#define GDISP_HARDWARE_CLEARS		HARDWARE_AUTODETECT
	#endif
	#if GDISP_HARDWARE_FILLS == TRUE
		#undef GDISP_HARDWARE_FILLS
		#define GDISP_HARDWARE_FILLS		HARDWARE_AUTODETECT
	#endif
	#if GDISP_HARDWARE_BITFILLS == TRUE
		#undef GDISP_HARDWARE_BITFILLS
		#define GDISP_HARDWARE_BITFILLS		HARDWARE_AUTODETECT
	#endif
	#if GDISP_HARDWARE_SCROLL == TRUE
		#undef GDISP_HARDWARE_SCROLL
		#define GDISP_HARDWARE_SCROLL		HARDWARE_AUTODETECT
	#endif
	#if GDISP_HARDWARE_QUERY == TRUE
		#undef GDISP_HARDWARE_QUERY
		#define GDISP_HARDWARE_QUERY		HARDWARE_AUTODETECT
	#endif
	#if GDISP_HARDWARE_CLIP == TRUE
		#undef GDISP_HARDWARE_CLIP
		#define GDISP_HARDWARE_CLIP			HARDWARE_AUTODETECT
	#endif
#endif

//------------------------------------------------------------------------------------------------------------

/* Verify information for packed pixels and define a non-packed pixel macro */
#if !GDISP_PACKED_PIXELS
	#define gdispPackPixels(buf,cx,x,y,c)	{ ((color_t *)(buf))[(y)*(cx)+(x)] = (c); }
#elif !GDISP_HARDWARE_BITFILLS
	#error "GDISP: packed pixel formats are only supported for hardware accelerated drivers."
#elif GDISP_PIXELFORMAT != GDISP_PIXELFORMAT_RGB888 \
		&& GDISP_PIXELFORMAT != GDISP_PIXELFORMAT_RGB444 \
		&& GDISP_PIXELFORMAT != GDISP_PIXELFORMAT_RGB666 \
		&& GDISP_PIXELFORMAT != GDISP_PIXELFORMAT_CUSTOM
	#error "GDISP: A packed pixel format has been specified for an unsupported pixel format."
#endif

/* Support routine for packed pixel formats */
#if !defined(gdispPackPixels) || defined(__DOXYGEN__)
	/**
	 * @brief   Pack a pixel into a pixel buffer.
	 * @note    This function performs no buffer boundary checking
	 *			regardless of whether GDISP_NEED_CLIP has been specified.
	 *
	 * @param[in] buf		The buffer to put the pixel in
	 * @param[in] cx		The width of a pixel line
	 * @param[in] x, y		The location of the pixel to place
	 * @param[in] color		The color to put into the buffer
	 *
	 * @api
	 */
	void gdispPackPixels(const pixel_t *buf, coord_t cx, coord_t x, coord_t y, color_t color);
#endif

//------------------------------------------------------------------------------------------------------------

struct GDisplay {
	struct GDriver				d;					// This must be the first element
		#define gvmt(g)		((const GDISPVMT const *)((g)->d.vmt))	// For ease of access to the vmt member

	struct GDISPControl {
		coord_t					Width;
		coord_t					Height;
		orientation_t			Orientation;
		powermode_t				Powermode;
		uint8_t					Backlight;
		uint8_t					Contrast;
	} g;

	void *						priv;				// A private area just for the drivers use.
	void *						board;				// A private area just for the board interfaces use.

	uint8_t						systemdisplay;
	uint8_t						controllerdisplay;
	uint16_t					flags;
		#define GDISP_FLG_INSTREAM		0x0001		// We are in a user based stream operation
		#define GDISP_FLG_SCRSTREAM		0x0002		// The stream area currently covers the whole screen
		#define GDISP_FLG_DRIVER		0x0004		// This flags and above are for use by the driver

	// Multithread Mutex
	#if GDISP_NEED_MULTITHREAD
		gfxMutex				mutex;
	#endif

	// Software clipping
	#if GDISP_HARDWARE_CLIP != TRUE && (GDISP_NEED_CLIP || GDISP_NEED_VALIDATION)
		coord_t					clipx0, clipy0;
		coord_t					clipx1, clipy1;		/* not inclusive */
	#endif

	// Driver call parameters
	struct {
		coord_t			x, y;
		coord_t			cx, cy;
		coord_t			x1, y1;
		coord_t			x2, y2;
		color_t			color;
		void			*ptr;
	} p;

	// In call working buffers

	#if GDISP_NEED_TEXT
		// Text rendering parameters
		struct {
			font_t		font;
			color_t		color;
			color_t		bgcolor;
			coord_t		clipx0, clipy0;
			coord_t		clipx1, clipy1;
		} t;
	#endif
	#if GDISP_LINEBUF_SIZE != 0 && ((GDISP_NEED_SCROLL && !GDISP_HARDWARE_SCROLL) || (!GDISP_HARDWARE_STREAM_WRITE && GDISP_HARDWARE_BITFILLS))
		// A pixel line buffer
		color_t		linebuf[GDISP_LINEBUF_SIZE];
	#endif
};

typedef struct GDISPVMT {
	GDriverVMT	d;
		#define GDISP_VFLG_DYNAMICONLY		0x0001		// This display should never be statically initialised
		#define GDISP_VFLG_PIXMAP			0x0002		// This is a pixmap display
	bool_t (*init)(GDisplay *g);
	void (*deinit)(GDisplay *g);
	void (*writestart)(GDisplay *g);				// Uses p.x,p.y  p.cx,p.cy
	void (*writepos)(GDisplay *g);					// Uses p.x,p.y
	void (*writecolor)(GDisplay *g);				// Uses p.color
	void (*writestop)(GDisplay *g);					// Uses no parameters
	void (*readstart)(GDisplay *g);					// Uses p.x,p.y  p.cx,p.cy
	color_t (*readcolor)(GDisplay *g);				// Uses no parameters
	void (*readstop)(GDisplay *g);					// Uses no parameters
	void (*pixel)(GDisplay *g);						// Uses p.x,p.y  p.color
	void (*clear)(GDisplay *g);						// Uses p.color
	void (*fill)(GDisplay *g);						// Uses p.x,p.y  p.cx,p.cy  p.color
	void (*blit)(GDisplay *g);						// Uses p.x,p.y  p.cx,p.cy  p.x1,p.y1 (=srcx,srcy)  p.x2 (=srccx), p.ptr (=buffer)
	color_t (*get)(GDisplay *g);					// Uses p.x,p.y
	void (*vscroll)(GDisplay *g);					// Uses p.x,p.y  p.cx,p.cy, p.y1 (=lines) p.color
	void (*control)(GDisplay *g);					// Uses p.x (=what)  p.ptr (=value)
	void *(*query)(GDisplay *g);					// Uses p.x (=what);
	void (*setclip)(GDisplay *g);					// Uses p.x,p.y  p.cx,p.cy
	void (*flush)(GDisplay *g);						// Uses no parameters
} GDISPVMT;

//------------------------------------------------------------------------------------------------------------

// Do we need function definitions or macro's (via the VMT)
#if IN_DRIVER || !USE_VMT || defined(__DOXYGEN__)
	#ifdef __cplusplus
	extern "C" {
	#endif

	// Should the driver routines be static or not
	#if USE_VMT
		#define LLDSPEC         static
	#else
		#define LLDSPEC
	#endif

	/**
	 * @brief   Initialize the driver.
	 * @return	TRUE if successful.
	 * @param[in]	g					The driver structure
	 * @param[out]	g->g				The driver must fill in the GDISPControl structure
	 */
	LLDSPEC	bool_t gdisp_lld_init(GDisplay *g);

	#if GDISP_HARDWARE_DEINIT || defined(__DOXYGEN__)
		/**
		 * @brief   The driver is being de-initialized
		 * @pre		GDISP_HARDWARE_FLUSH is TRUE
		 *
		 * @param[in]	g				The driver structure
		 *
		 */
		LLDSPEC	void gdisp_lld_deinit(GDisplay *g);
	#endif

	#if GDISP_HARDWARE_FLUSH || defined(__DOXYGEN__)
		/**
		 * @brief   Flush the current drawing operations to the display
		 * @pre		GDISP_HARDWARE_FLUSH is TRUE
		 *
		 * @param[in]	g				The driver structure
		 *
		 * @note		The parameter variables must not be altered by the driver.
		 */
		LLDSPEC	void gdisp_lld_flush(GDisplay *g);
	#endif

	#if GDISP_HARDWARE_STREAM_WRITE || defined(__DOXYGEN__)
		/**
		 * @brief   Start a streamed write operation
		 * @pre		GDISP_HARDWARE_STREAM_WRITE is TRUE
		 *
		 * @param[in]	g				The driver structure
		 *
		 * @note		g->p.x,g->p.y	The window position
		 * @note		g->p.cx,g->p.cy	The window size
		 *
		 * @note		The parameter variables must not be altered by the driver.
		 * @note		Streaming operations that wrap the defined window have
		 * 				undefined results.
		 * @note		This must be followed by a call to @p gdisp_lld_write_pos() if GDISP_HARDWARE_STREAM_POS is TRUE.
		 */
		LLDSPEC	void gdisp_lld_write_start(GDisplay *g);

		/**
		 * @brief   Send a pixel to the current streaming position and then increment that position
		 * @pre		GDISP_HARDWARE_STREAM_WRITE is TRUE
		 *
		 * @param[in]	g				The driver structure
		 *
		 * @note		g->p.color		The color to display at the curent position
		 * @note		The parameter variables must not be altered by the driver.
		 */
		LLDSPEC	void gdisp_lld_write_color(GDisplay *g);

		/**
		 * @brief   End the current streaming write operation
		 * @pre		GDISP_HARDWARE_STREAM_WRITE is TRUE
		 *
		 * @param[in]	g				The driver structure
		 *
		 * @note		The parameter variables must not be altered by the driver.
		 */
		LLDSPEC	void gdisp_lld_write_stop(GDisplay *g);

		#if GDISP_HARDWARE_STREAM_POS || defined(__DOXYGEN__)
			/**
			 * @brief   Change the current position within the current streaming window
			 * @pre		GDISP_HARDWARE_STREAM_POS is TRUE and GDISP_HARDWARE_STREAM_WRITE is TRUE
			 *
			 * @param[in]	g				The driver structure
			 * @param[in]	g->p.x,g->p.y	The new position (which will always be within the existing stream window)
			 *
			 * @note		The parameter variables must not be altered by the driver.
			 */
			LLDSPEC	void gdisp_lld_write_pos(GDisplay *g);
		#endif
	#endif

	#if GDISP_HARDWARE_STREAM_READ || defined(__DOXYGEN__)
		/**
		 * @brief   Start a streamed read operation
		 * @pre		GDISP_HARDWARE_STREAM_READ is TRUE
		 *
		 * @param[in]	g				The driver structure
		 * @param[in]	g->p.x,g->p.y	The window position
		 * @param[in]	g->p.cx,g->p.cy	The window size
		 *
		 * @note		The parameter variables must not be altered by the driver.
		 * @note		Streaming operations that wrap the defined window have
		 * 				undefined results.
		 */
		LLDSPEC	void gdisp_lld_read_start(GDisplay *g);

		/**
		 * @brief   Read a pixel from the current streaming position and then increment that position
		 * @return	The color at the current position
		 * @pre		GDISP_HARDWARE_STREAM_READ is TRUE
		 *
		 * @param[in]	g				The driver structure
		 *
		 * @note		The parameter variables must not be altered by the driver.
		 */
		LLDSPEC	color_t gdisp_lld_read_color(GDisplay *g);

		/**
		 * @brief   End the current streaming operation
		 * @pre		GDISP_HARDWARE_STREAM_READ is TRUE
		 *
		 * @param[in]	g				The driver structure
		 *
		 * @note		The parameter variables must not be altered by the driver.
		 */
		LLDSPEC	void gdisp_lld_read_stop(GDisplay *g);
	#endif

	#if GDISP_HARDWARE_DRAWPIXEL || defined(__DOXYGEN__)
		/**
		 * @brief   Draw a pixel
		 * @pre		GDISP_HARDWARE_DRAWPIXEL is TRUE
		 *
		 * @param[in]	g				The driver structure
		 * @param[in]	g->p.x,g->p.y	The pixel position
		 * @param[in]	g->p.color		The color to set
		 *
		 * @note		The parameter variables must not be altered by the driver.
		 */
		LLDSPEC	void gdisp_lld_draw_pixel(GDisplay *g);
	#endif

	#if GDISP_HARDWARE_CLEARS || defined(__DOXYGEN__)
		/**
		 * @brief   Clear the screen using the defined color
		 * @pre		GDISP_HARDWARE_CLEARS is TRUE
		 *
		 * @param[in]	g				The driver structure
		 * @param[in]	g->p.color		The color to set
		 *
		 * @note		The parameter variables must not be altered by the driver.
		 */
		LLDSPEC	void gdisp_lld_clear(GDisplay *g);
	#endif

	#if GDISP_HARDWARE_FILLS || defined(__DOXYGEN__)
		/**
		 * @brief   Fill an area with a single color
		 * @pre		GDISP_HARDWARE_FILLS is TRUE
		 *
		 * @param[in]	g				The driver structure
		 * @param[in]	g->p.x,g->p.y	The area position
		 * @param[in]	g->p.cx,g->p.cy	The area size
		 * @param[in]	g->p.color		The color to set
		 *
		 * @note		The parameter variables must not be altered by the driver.
		 */
		LLDSPEC	void gdisp_lld_fill_area(GDisplay *g);
	#endif

	#if GDISP_HARDWARE_BITFILLS || defined(__DOXYGEN__)
		/**
		 * @brief   Fill an area using a bitmap
		 * @pre		GDISP_HARDWARE_BITFILLS is TRUE
		 *
		 * @param[in]	g				The driver structure
		 * @param[in]	g->p.x,g->p.y	The area position
		 * @param[in]	g->p.cx,g->p.cy	The area size
		 * @param[in]	g->p.x1,g->p.y1	The starting position in the bitmap
		 * @param[in]	g->p.x2			The width of a bitmap line
		 * @param[in]	g->p.ptr		The pointer to the bitmap
		 *
		 * @note		The parameter variables must not be altered by the driver.
		 */
		LLDSPEC	void gdisp_lld_blit_area(GDisplay *g);
	#endif

	#if GDISP_HARDWARE_PIXELREAD || defined(__DOXYGEN__)
		/**
		 * @brief   Read a pixel from the display
		 * @return	The color at the defined position
		 * @pre		GDISP_HARDWARE_PIXELREAD is TRUE (and the application needs it)
		 *
		 * @param[in]	g				The driver structure
		 * @param[in]	g->p.x,g->p.y	The pixel position
		 *
		 * @note		The parameter variables must not be altered by the driver.
		 */
		LLDSPEC	color_t gdisp_lld_get_pixel_color(GDisplay *g);
	#endif

	#if (GDISP_HARDWARE_SCROLL && GDISP_NEED_SCROLL) || defined(__DOXYGEN__)
		/**
		 * @brief   Scroll an area of the screen
		 * @pre		GDISP_HARDWARE_SCROLL is TRUE (and the application needs it)
		 *
		 * @param[in]	g				The driver structure
		 * @param[in]	g->p.x,g->p.y	The area position
		 * @param[in]	g->p.cx,g->p.cy	The area size
		 * @param[in]	g->p.y1			The number of lines to scroll (positive or negative)
		 *
		 * @note		The parameter variables must not be altered by the driver.
		 * @note		This can be easily implemented if the hardware supports
		 * 				display area to display area copying.
		 * @note		Clearing the exposed area on the scroll operation is not
		 * 				needed as the high level code handles this.
		 */
		LLDSPEC	void gdisp_lld_vertical_scroll(GDisplay *g);
	#endif

	#if (GDISP_HARDWARE_CONTROL && GDISP_NEED_CONTROL) || defined(__DOXYGEN__)
		/**
		 * @brief   Control some feature of the hardware
		 * @pre		GDISP_HARDWARE_CONTROL is TRUE (and the application needs it)
		 *
		 * @param[in]	g				The driver structure
		 * @param[in]	g->p.x			The operation to perform
		 * @param[in]	g->p.ptr		The operation parameter
		 *
		 * @note		The parameter variables must not be altered by the driver.
		 */
		LLDSPEC	void gdisp_lld_control(GDisplay *g);
	#endif

	#if (GDISP_HARDWARE_QUERY && GDISP_NEED_QUERY) || defined(__DOXYGEN__)
		/**
		 * @brief   Query some feature of the hardware
		 * @return	The information requested (typecast as void *)
		 * @pre		GDISP_HARDWARE_QUERY is TRUE (and the application needs it)
		 *
		 * @param[in]	g				The driver structure
		 * @param[in]	g->p.x			What to query
		 *
		 * @note		The parameter variables must not be altered by the driver.
		 */
		LLDSPEC	void *gdisp_lld_query(GDisplay *g);				// Uses p.x (=what);
	#endif

	#if (GDISP_HARDWARE_CLIP && (GDISP_NEED_CLIP || GDISP_NEED_VALIDATION)) || defined(__DOXYGEN__)
		/**
		 * @brief   Set the hardware clipping area
		 * @pre		GDISP_HARDWARE_CLIP is TRUE (and the application needs it)
		 *
		 * @param[in]	g				The driver structure
		 * @param[in]	g->p.x,g->p.y	The area position
		 * @param[in]	g->p.cx,g->p.cy	The area size
		 *
		 * @note		The parameter variables must not be altered by the driver.
		 */
		LLDSPEC	void gdisp_lld_set_clip(GDisplay *g);
	#endif

	#ifdef __cplusplus
	}
	#endif

#else
	#define gdisp_lld_init(g)				gvmt(g)->init(g)
	#define gdisp_lld_deinit(g)				gvmt(g)->deinit(g)
	#define gdisp_lld_flush(g)				gvmt(g)->flush(g)
	#define gdisp_lld_write_start(g)		gvmt(g)->writestart(g)
	#define gdisp_lld_write_pos(g)			gvmt(g)->writepos(g)
	#define gdisp_lld_write_color(g)		gvmt(g)->writecolor(g)
	#define gdisp_lld_write_stop(g)			gvmt(g)->writestop(g)
	#define gdisp_lld_read_start(g)			gvmt(g)->readstart(g)
	#define gdisp_lld_read_color(g)			gvmt(g)->readcolor(g)
	#define gdisp_lld_read_stop(g)			gvmt(g)->readstop(g)
	#define gdisp_lld_draw_pixel(g)			gvmt(g)->pixel(g)
	#define gdisp_lld_clear(g)				gvmt(g)->clear(g)
	#define gdisp_lld_fill_area(g)			gvmt(g)->fill(g)
	#define gdisp_lld_blit_area(g)			gvmt(g)->blit(g)
	#define gdisp_lld_get_pixel_color(g)	gvmt(g)->get(g)
	#define gdisp_lld_vertical_scroll(g)	gvmt(g)->vscroll(g)
	#define gdisp_lld_control(g)			gvmt(g)->control(g)
	#define gdisp_lld_query(g)				gvmt(g)->query(g)
	#define gdisp_lld_set_clip(g)			gvmt(g)->setclip(g)
#endif

//------------------------------------------------------------------------------------------------------------

// If compiling the driver then build the VMT and set the low level driver color macros.
#if IN_DRIVER

	// Make sure the driver has a valid model
	#if !GDISP_HARDWARE_STREAM_WRITE && !GDISP_HARDWARE_DRAWPIXEL
		#error "GDISP Driver: Either GDISP_HARDWARE_STREAM_WRITE or GDISP_HARDWARE_DRAWPIXEL must be TRUE"
	#endif

	// If we are not using multiple displays then hard-code the VMT name (except for the pixmap driver)
	#if !IS_MULTIPLE && !IN_PIXMAP_DRIVER
		#undef GDISP_DRIVER_VMT
		#define GDISP_DRIVER_VMT		GDISPVMT_OnlyOne
	#endif

	// Default the flags if the driver doesn't specify any
	#ifndef GDISP_DRIVER_VMT_FLAGS
		#define GDISP_DRIVER_VMT_FLAGS		0
	#endif

	// Routines needed by the general driver VMT
	#ifdef __cplusplus
	extern "C" {
	#endif
		bool_t _gdispInitDriver(GDriver *g, void *param, unsigned driverinstance, unsigned systeminstance);
		void _gdispPostInitDriver(GDriver *g);
		void _gdispDeInitDriver(GDriver *g);
	#ifdef __cplusplus
	}
	#endif

	// Build the VMT
	const GDISPVMT const GDISP_DRIVER_VMT[1] = {{
		{ GDRIVER_TYPE_DISPLAY, 0, sizeof(GDisplay), _gdispInitDriver, _gdispPostInitDriver, _gdispDeInitDriver },
		gdisp_lld_init,
		#if GDISP_HARDWARE_DEINIT
			gdisp_lld_deinit,
		#else
			0,
		#endif
		#if GDISP_HARDWARE_STREAM_WRITE
			gdisp_lld_write_start,
			#if GDISP_HARDWARE_STREAM_POS
				gdisp_lld_write_pos,
			#else
				0,
			#endif
			gdisp_lld_write_color,
			gdisp_lld_write_stop,
		#else
			0, 0, 0, 0,
		#endif
		#if GDISP_HARDWARE_STREAM_READ
			gdisp_lld_read_start,
			gdisp_lld_read_color,
			gdisp_lld_read_stop,
		#else
			0, 0, 0,
		#endif
		#if GDISP_HARDWARE_DRAWPIXEL
			gdisp_lld_draw_pixel,
		#else
			0,
		#endif
		#if GDISP_HARDWARE_CLEARS
			gdisp_lld_clear,
		#else
			0,
		#endif
		#if GDISP_HARDWARE_FILLS
			gdisp_lld_fill_area,
		#else
			0,
		#endif
		#if GDISP_HARDWARE_BITFILLS
			gdisp_lld_blit_area,
		#else
			0,
		#endif
		#if GDISP_HARDWARE_PIXELREAD
			gdisp_lld_get_pixel_color,
		#else
			0,
		#endif
		#if GDISP_HARDWARE_SCROLL && GDISP_NEED_SCROLL
			gdisp_lld_vertical_scroll,
		#else
			0,
		#endif
		#if GDISP_HARDWARE_CONTROL && GDISP_NEED_CONTROL
			gdisp_lld_control,
		#else
			0,
		#endif
		#if GDISP_HARDWARE_QUERY && GDISP_NEED_QUERY
			gdisp_lld_query,
		#else
			0,
		#endif
		#if GDISP_HARDWARE_CLIP && (GDISP_NEED_CLIP || GDISP_NEED_VALIDATION)
			gdisp_lld_set_clip,
		#else
			0,
		#endif
		#if GDISP_HARDWARE_FLUSH
			gdisp_lld_flush,
		#else
			0,
		#endif
	}};

	//--------------------------------------------------------------------------------------------------------

	/* Low level driver pixel format information */
	//-------------------------
	//	True-Color color system
	//-------------------------
	#if GDISP_LLD_PIXELFORMAT & GDISP_COLORSYSTEM_TRUECOLOR
		#define LLDCOLOR_SYSTEM			GDISP_COLORSYSTEM_TRUECOLOR

		// Calculate the number of bits
		#define LLDCOLOR_BITS_R			((GDISP_LLD_PIXELFORMAT>>8) & 0x0F)
		#define LLDCOLOR_BITS_G			((GDISP_LLD_PIXELFORMAT>>4) & 0x0F)
		#define LLDCOLOR_BITS_B			((GDISP_LLD_PIXELFORMAT>>0) & 0x0F)
		#define LLDCOLOR_BITS			(LLDCOLOR_BITS_R + LLDCOLOR_BITS_G + LLDCOLOR_BITS_B)

		// From the number of bits determine COLOR_TYPE, COLOR_TYPE_BITS and masking
		#if LLDCOLOR_BITS <= 8
			#define LLDCOLOR_TYPE			uint8_t
			#define LLDCOLOR_TYPE_BITS		8
		#elif LLDCOLOR_BITS <= 16
			#define LLDCOLOR_TYPE			uint16_t
			#define LLDCOLOR_TYPE_BITS		16
		#elif LLDCOLOR_BITS <= 32
			#define LLDCOLOR_TYPE			uint32_t
			#define LLDCOLOR_TYPE_BITS		32
		#else
			#error "GDISP: Cannot define low level driver color types with more than 32 bits"
		#endif
		#if LLDCOLOR_TYPE_BITS == LLDCOLOR_BITS
			#define LLDCOLOR_NEEDS_MASK	FALSE
		#else
			#define LLDCOLOR_NEEDS_MASK	TRUE
		#endif
		#define LLDCOLOR_MASK()			((1 << LLDCOLOR_BITS)-1)

		// Calculate the component bit shifts
		#if (GDISP_LLD_PIXELFORMAT & GDISP_COLORSYSTEM_MASK) == GDISP_COLORSYSTEM_RGB
			#define LLDCOLOR_SHIFT_R		(LLDCOLOR_BITS_B+LLDCOLOR_BITS_G)
			#define LLDCOLOR_SHIFT_G		LLDCOLOR_BITS_B
			#define LLDCOLOR_SHIFT_B		0
		#else
			#define LLDCOLOR_SHIFT_B		(LLDCOLOR_BITS_R+LLDCOLOR_BITS_G)
			#define LLDCOLOR_SHIFT_G		LLDCOLOR_BITS_R
			#define LLDCOLOR_SHIFT_R		0
		#endif

		// Calculate LLDRED_OF, LLDGREEN_OF, LLDBLUE_OF and LLDRGB2COLOR
		#if LLDCOLOR_BITS_R + LLDCOLOR_SHIFT_R == 8
			#define LLDRED_OF(c)			((c) & (((1<<LLDCOLOR_BITS_R)-1) << LLDCOLOR_SHIFT_R))
			#define LLDRGB2COLOR_R(r)		((LLDCOLOR_TYPE)((r) & (0xFF & ~((1<<(8-LLDCOLOR_BITS_R))-1))))
		#elif LLDCOLOR_BITS_R + LLDCOLOR_SHIFT_R > 8
			#define LLDRED_OF(c)			(((c) & (((1<<LLDCOLOR_BITS_R)-1) << LLDCOLOR_SHIFT_R)) >> (LLDCOLOR_BITS_R+LLDCOLOR_SHIFT_R-8))
			#define LLDRGB2COLOR_R(r)		(((LLDCOLOR_TYPE)((r) & (0xFF & ~((1<<(8-LLDCOLOR_BITS_R))-1)))) << (LLDCOLOR_BITS_R+LLDCOLOR_SHIFT_R-8))
		#else // LLDCOLOR_BITS_R + LLDCOLOR_SHIFT_R < 8
			#define LLDRED_OF(c)			(((c) & (((1<<LLDCOLOR_BITS_R)-1) << LLDCOLOR_SHIFT_R)) << (8-(LLDCOLOR_BITS_R+LLDCOLOR_SHIFT_R)))
			#define LLDRGB2COLOR_R(r)		(((LLDCOLOR_TYPE)((r) & (0xFF & ~((1<<(8-LLDCOLOR_BITS_R))-1)))) >> (8-(LLDCOLOR_BITS_R+LLDCOLOR_SHIFT_R)))
		#endif
		#if LLDCOLOR_BITS_G + LLDCOLOR_SHIFT_G == 8
			#define LLDGREEN_OF(c)			((c) & (((1<<LLDCOLOR_BITS_G)-1) << LLDCOLOR_SHIFT_G))
			#define LLDRGB2COLOR_G(g)		((LLDCOLOR_TYPE)((g) & (0xFF & ~((1<<(8-LLDCOLOR_BITS_G))-1))))
		#elif LLDCOLOR_BITS_G + LLDCOLOR_SHIFT_G > 8
			#define LLDGREEN_OF(c)			(((c) & (((1<<LLDCOLOR_BITS_G)-1) << LLDCOLOR_SHIFT_G)) >> (LLDCOLOR_BITS_G+LLDCOLOR_SHIFT_G-8))
			#define LLDRGB2COLOR_G(g)		(((LLDCOLOR_TYPE)((g) & (0xFF & ~((1<<(8-LLDCOLOR_BITS_G))-1)))) << (LLDCOLOR_BITS_G+LLDCOLOR_SHIFT_G-8))
		#else // LLDCOLOR_BITS_G + LLDCOLOR_SHIFT_G < 8
			#define LLDGREEN_OF(c)			(((c) & (((1<<LLDCOLOR_BITS_G)-1) << LLDCOLOR_SHIFT_G)) << (8-(LLDCOLOR_BITS_G+LLDCOLOR_SHIFT_G)))
			#define LLDRGB2COLOR_G(g)		(((LLDCOLOR_TYPE)((g) & (0xFF & ~((1<<(8-LLDCOLOR_BITS_G))-1)))) >> (8-(LLDCOLOR_BITS_G+LLDCOLOR_SHIFT_G)))
		#endif
		#if LLDCOLOR_BITS_B + LLDCOLOR_SHIFT_B == 8
			#define LLDBLUE_OF(c)			((c) & (((1<<LLDCOLOR_BITS_B)-1) << LLDCOLOR_SHIFT_B))
			#define LLDRGB2COLOR_B(b)		((LLDCOLOR_TYPE)((b) & (0xFF & ~((1<<(8-LLDCOLOR_BITS_B))-1))))
		#elif LLDCOLOR_BITS_B + LLDCOLOR_SHIFT_B > 8
			#define LLDBLUE_OF(c)			(((c) & (((1<<LLDCOLOR_BITS_B)-1) << LLDCOLOR_SHIFT_B)) >> (LLDCOLOR_BITS_B+LLDCOLOR_SHIFT_B-8))
			#define LLDRGB2COLOR_B(b)		(((LLDCOLOR_TYPE)((b) & (0xFF & ~((1<<(8-LLDCOLOR_BITS_B))-1)))) << (LLDCOLOR_BITS_B+LLDCOLOR_SHIFT_B-8))
		#else // LLDCOLOR_BITS_B + LLDCOLOR_SHIFT_B < 8
			#define LLDBLUE_OF(c)			(((c) & (((1<<LLDCOLOR_BITS_B)-1) << LLDCOLOR_SHIFT_B)) << (8-(LLDCOLOR_BITS_B+LLDCOLOR_SHIFT_B)))
			#define LLDRGB2COLOR_B(b)		(((LLDCOLOR_TYPE)((b) & (0xFF & ~((1<<(8-LLDCOLOR_BITS_B))-1)))) >> (8-(LLDCOLOR_BITS_B+LLDCOLOR_SHIFT_B)))
		#endif
		#define LLDLUMA_OF(c)				((LLDRED_OF(c)+((uint16_t)LLDGREEN_OF(c)<<1)+LLDBLUE_OF(c))>>2)
		#define LLDEXACT_RED_OF(c)			(((uint16_t)(((c)>>LLDCOLOR_SHIFT_R)&((1<<LLDCOLOR_BITS_R)-1))*255)/((1<<LLDCOLOR_BITS_R)-1))
		#define LLDEXACT_GREEN_OF(c)		(((uint16_t)(((c)>>LLDCOLOR_SHIFT_G)&((1<<LLDCOLOR_BITS_G)-1))*255)/((1<<LLDCOLOR_BITS_G)-1))
		#define LLDEXACT_BLUE_OF(c)			(((uint16_t)(((c)>>LLDCOLOR_SHIFT_B)&((1<<LLDCOLOR_BITS_B)-1))*255)/((1<<LLDCOLOR_BITS_B)-1))
		#define LLDEXACT_LUMA_OF(c)			((LLDEXACT_RED_OF(c)+((uint16_t)LLDEXACT_GREEN_OF(c)<<1)+LLDEXACT_BLUE_OF(c))>>2)
		#define LLDLUMA2COLOR(l)			(LLDRGB2COLOR_R(l) | LLDRGB2COLOR_G(l) | LLDRGB2COLOR_B(l))
		#define LLDRGB2COLOR(r,g,b)			(LLDRGB2COLOR_R(r) | LLDRGB2COLOR_G(g) | LLDRGB2COLOR_B(b))

		// Calculate LLDHTML2COLOR
		#if LLDCOLOR_BITS_R + LLDCOLOR_SHIFT_R == 24
			#define LLDHTML2COLOR_R(h)		((h) & ((0xFF & ~((1<<(8-LLDCOLOR_BITS_R))-1))<<16))
		#elif COLOR_BITS_R + COLOR_SHIFT_R > 24
			#define LLDHTML2COLOR_R(h)		(((h) & ((0xFF & ~((1<<(8-LLDCOLOR_BITS_R))-1))<<16)) << (LLDCOLOR_BITS_R+LLDCOLOR_SHIFT_R-24))
		#else // COLOR_BITS_R + COLOR_SHIFT_R < 24
			#define LLDHTML2COLOR_R(h)		(((h) & ((0xFF & ~((1<<(8-LLDCOLOR_BITS_R))-1))<<16)) >> (24-(LLDCOLOR_BITS_R+LLDCOLOR_SHIFT_R)))
		#endif
		#if LLDCOLOR_BITS_G + LLDCOLOR_SHIFT_G == 16
			#define LLDHTML2COLOR_G(h)		((h) & ((0xFF & ~((1<<(8-LLDCOLOR_BITS_G))-1))<<8))
		#elif LLDCOLOR_BITS_G + LLDCOLOR_SHIFT_G > 16
			#define LLDHTML2COLOR_G(h)		(((h) & ((0xFF & ~((1<<(8-LLDCOLOR_BITS_G))-1))<<8)) << (LLDCOLOR_BITS_G+LLDCOLOR_SHIFT_G-16))
		#else // LLDCOLOR_BITS_G + LLDCOLOR_SHIFT_G < 16
			#define LLDHTML2COLOR_G(h)		(((h) & ((0xFF & ~((1<<(8-LLDCOLOR_BITS_G))-1))<<8)) >> (16-(LLDCOLOR_BITS_G+LLDCOLOR_SHIFT_G)))
		#endif
		#if LLDCOLOR_BITS_B + LLDCOLOR_SHIFT_B == 8
			#define LLDHTML2COLOR_B(h)		((h) & (0xFF & ~((1<<(8-LLDCOLOR_BITS_B))-1)))
		#elif LLDCOLOR_BITS_B + LLDCOLOR_SHIFT_B > 8
			#define LLDHTML2COLOR_B(h)		(((h) & (0xFF & ~((1<<(8-LLDCOLOR_BITS_B))-1))) << (LLDCOLOR_BITS_B+LLDCOLOR_SHIFT_B-8))
		#else // LLDCOLOR_BITS_B + LLDCOLOR_SHIFT_B < 8
			#define LLDHTML2COLOR_B(h)		(((h) & (0xFF & ~((1<<(8-LLDCOLOR_BITS_B))-1))) >> (8-(LLDCOLOR_BITS_B+LLDCOLOR_SHIFT_B)))
		#endif
		#define LLDHTML2COLOR(h)		((LLDCOLOR_TYPE)(LLDHTML2COLOR_R(h) | LLDHTML2COLOR_G(h) | LLDHTML2COLOR_B(h)))

	//-------------------------
	//	Gray-scale color system
	//-------------------------
	#elif (GDISP_LLD_PIXELFORMAT & GDISP_COLORSYSTEM_MASK) == GDISP_COLORSYSTEM_GRAYSCALE
		#define LLDCOLOR_SYSTEM			GDISP_COLORSYSTEM_GRAYSCALE

		// Calculate the number of bits and shifts
		#define LLDCOLOR_BITS			(GDISP_LLD_PIXELFORMAT & 0xFF)
		#define LLDCOLOR_BITS_R			LLDCOLOR_BITS
		#define LLDCOLOR_BITS_G			LLDCOLOR_BITS
		#define LLDCOLOR_BITS_B			LLDCOLOR_BITS
		#define LLDCOLOR_SHIFT_R		0
		#define LLDCOLOR_SHIFT_G		0
		#define LLDCOLOR_SHIFT_B		0

		// From the number of bits determine COLOR_TYPE, COLOR_TYPE_BITS and masking
		#if LLDCOLOR_BITS <= 8
			#define LLDCOLOR_TYPE		uint8_t
			#define LLDCOLOR_TYPE_BITS	8
		#else
			#error "GDISP: Cannot define gray-scale low level driver color types with more than 8 bits"
		#endif
		#if LLDCOLOR_TYPE_BITS == LLDCOLOR_BITS
			#define LLDCOLOR_NEEDS_MASK	FALSE
		#else
			#define LLDCOLOR_NEEDS_MASK	TRUE
		#endif
		#define LLDCOLOR_MASK()			((1 << LLDCOLOR_BITS)-1)

		#if COLOR_BITS == 1
			#define LLDRGB2COLOR(r,g,b)		(((r)|(g)|(b)) ? 1 : 0)
			#define LLDLUMA2COLOR(l)		((l) ? 1 : 0)
			#define LLDHTML2COLOR(h)		((h) ? 1 : 0)
			#define LLDLUMA_OF(c)			((c) ? 255 : 0)
			#define LLDEXACT_LUMA_OF(c)		LLDLUMA_OF(c)
		#else
			// They eye is more sensitive to green
			#define LLDRGB2COLOR(r,g,b)		((LLDCOLOR_TYPE)(((uint16_t)(r)+(g)+(g)+(b)) >> (10-LLDCOLOR_BITS)))
			#define LLDLUMA2COLOR(l)		((LLDCOLOR_TYPE)((l)>>(8-LLDCOLOR_BITS)))
			#define LLDHTML2COLOR(h)		((LLDCOLOR_TYPE)(((((h)&0xFF0000)>>16)+(((h)&0x00FF00)>>7)+((h)&0x0000FF)) >> (10-LLDCOLOR_BITS)))
			#define LLDLUMA_OF(c)			(((c) & ((1<<LLDCOLOR_BITS)-1)) << (8-LLDCOLOR_BITS))
			#define LLDEXACT_LUMA_OF(c)		((((uint16_t)(c) & ((1<<LLDCOLOR_BITS)-1))*255)/((1<<LLDCOLOR_BITS)-1))
		#endif

		#define LLDRED_OF(c)			LLDLUMA_OF(c)
		#define LLDGREEN_OF(c)			LLDLUMA_OF(c)
		#define LLDBLUE_OF(c)			LLDLUMA_OF(c)
		#define LLDEXACT_RED_OF(c)		LLDEXACT_LUMA_OF(c)
		#define LLDEXACT_GREEN_OF(c)	LLDEXACT_LUMA_OF(c)
		#define LLDEXACT_BLUE_OF(c)		LLDEXACT_LUMA_OF(c)

	//-------------------------
	//	Palette color system
	//-------------------------
	#elif (GDISP_LLD_PIXELFORMAT & GDISP_COLORSYSTEM_MASK) == GDISP_COLORSYSTEM_PALETTE
		#define LLDCOLOR_SYSTEM			GDISP_COLORSYSTEM_PALETTE

		#error "GDISP: A palette color system for low level drivers is not currently supported"

	//-------------------------
	//	Some other color system
	//-------------------------
	#else
		#error "GDISP: Unsupported color system for low level drivers"
	#endif

	/* Which is the larger color type */
	#if COLOR_BITS > LLDCOLOR_BITS
		#define LARGER_COLOR_BITS	COLOR_BITS
		#define LARGER_COLOR_TYPE	COLOR_TYPE
	#else
		#define LARGER_COLOR_BITS	LLDCOLOR_BITS
		#define LARGER_COLOR_TYPE	LLDCOLOR_TYPE
	#endif

	/**
	 * @brief	Controls color conversion accuracy for a low level driver
	 * @details	Should higher precision be used when converting colors.
	 * @note	Color conversion is only necessary if GDISP_PIXELFORMAT != GDISP_LLD_PIXELFORMAT
	 * @note	It only makes sense to turn this on if you have a high bit depth display but
	 * 			are running the application in low bit depths.
	 * @note	To achieve higher color accuracy bit shifting is replaced with multiplies and divides.
	 */
	#ifndef GDISP_HARDWARE_USE_EXACT_COLOR
		#if LLDCOLOR_BITS_R - COLOR_BITS_R >= LLDCOLOR_BITS_R/2 || LLDCOLOR_BITS_G - COLOR_BITS_G >= LLDCOLOR_BITS_G/2 || LLDCOLOR_BITS_B - COLOR_BITS_B >= LLDCOLOR_BITS_B/2
			#define GDISP_HARDWARE_USE_EXACT_COLOR	TRUE
		#else
			#define GDISP_HARDWARE_USE_EXACT_COLOR	FALSE
		#endif
	#endif

	/* Low level driver pixel format conversion functions */
	#if GDISP_PIXELFORMAT == GDISP_LLD_PIXELFORMAT || defined(__DOXYGEN__)
		/**
		 * @brief	Convert from a standard color format to the low level driver pixel format
		 * @note	For use only by low level drivers
		 */
		#define gdispColor2Native(c)	(c)
		/**
		 * @brief	Convert from a low level driver pixel format to the standard color format
		 * @note	For use only by low level drivers
		 */
		#define gdispNative2Color(c)	(c)
	#elif COLOR_SYSTEM == GDISP_COLORSYSTEM_GRAYSCALE || LLDCOLOR_SYSTEM == GDISP_COLORSYSTEM_GRAYSCALE
		#if GDISP_HARDWARE_USE_EXACT_COLOR
			#define gdispColor2Native(c)	LLDLUMA2COLOR(EXACT_LUMA_OF(c))
			#define gdispNative2Color(c)	LUMA2COLOR(LLDEXACT_LUMA_OF(c))
		#else
			#define gdispColor2Native(c)	LLDLUMA2COLOR(LUMA_OF(c))
			#define gdispNative2Color(c)	LUMA2COLOR(LLDLUMA_OF(c))
		#endif
	#elif COLOR_SYSTEM == GDISP_COLORSYSTEM_TRUECOLOR && LLDCOLOR_SYSTEM == GDISP_COLORSYSTEM_TRUECOLOR
		#if GDISP_HARDWARE_USE_EXACT_COLOR
			#define gdispColor2Native(c)	LLDRGB2COLOR(EXACT_RED_OF(c), EXACT_GREEN_OF(c), EXACT_BLUE_OF(c))
			#define gdispNative2Color(c)	RGB2COLOR(LLDEXACT_RED_OF(c), LLDEXACT_GREEN_OF(c), LLDEXACT_BLUE_OF(c))
		#else
			#define gdispColor2Native(c)	LLDRGB2COLOR(RED_OF(c), GREEN_OF(c), BLUE_OF(c))
			#define gdispNative2Color(c)	RGB2COLOR(LLDRED_OF(c), LLDGREEN_OF(c), LLDBLUE_OF(c))
		#endif
	#else
		#error "GDISP: This pixel format conversion is not supported yet"
	#endif

#endif

//------------------------------------------------------------------------------------------------------------

#undef IN_PIXMAP_DRIVER
#undef IS_MULTIPLE
#undef IN_DRIVER
#undef USE_VMT
#endif	/* GFX_USE_GDISP */

#endif	/* _GDISP_LLD_H */
/** @} */
