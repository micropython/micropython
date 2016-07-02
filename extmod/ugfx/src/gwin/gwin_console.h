/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/gwin/gwin_console.h
 * @brief   GWIN Graphic window subsystem header file.
 *
 * @defgroup Console Console
 * @ingroup Windows
 *
 * @brief		Console widget that can be used similar a terminal on a computer.
 *
 * @details		GWIN allows it to create a console/terminal like window.
 *				You can simply use chprintf() to print to the terminal.
 *
 * @pre			GFX_USE_GWIN must be set to TRUE in your gfxconf.h
 * @pre			GWIN_NEED_CONSOLE must be set to TRUE in your gfxconf.h
 *
 * @{
 */

#ifndef _GWIN_CONSOLE_H
#define _GWIN_CONSOLE_H

/* This file is included within "src/gwin/gwin.h" */

// A console window. Supports wrapped text writing and a cursor.
typedef struct GConsoleObject {
	GWindowObject	g;
	coord_t			cx, cy;			// Cursor position

	#if GWIN_CONSOLE_ESCSEQ
		uint8_t		startattr;		// ANSI-like escape sequences
		uint8_t		currattr;
		uint16_t	escstate;
	#endif

	#if GWIN_CONSOLE_USE_HISTORY
		char *		buffer;			// buffer to store console content
		size_t		bufsize;		// size of buffer
		size_t		bufpos;			// the position of the next char
	#endif

	#if GFX_USE_OS_CHIBIOS && GWIN_CONSOLE_USE_BASESTREAM
		struct GConsoleWindowStream_t {
			const struct GConsoleWindowVMT_t *vmt;
			_base_asynchronous_channel_data
		} stream;
	#endif
	
} GConsoleObject;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Create a console window.
 * @details	A console window allows text to be written.
 * @note	Text in a console window supports newlines and will wrap text as required.
 * @return  NULL if there is no resultant drawing area, otherwise a window handle.
 *
 * @param[in] g			The GDisplay to display this window on
 * @param[in] gc		The GConsoleObject structure to initialise. If this is NULL the structure is dynamically allocated.
 * @param[in] pInit		The initialization parameters to use
 *
 * @note				The drawing color and the background color get set to the current defaults. If you haven't called
 * 						@p gwinSetDefaultColor() or @p gwinSetDefaultBgColor() then these are White and Black respectively.
 * @note				The font gets set to the current default font. If you haven't called @p gwinSetDefaultFont() then there
 * 						is no default font and text drawing operations will no nothing.
 * @note				On creation even if the window is visible it is not automatically cleared.
 * 						You may do that by calling @p gwinClear() (possibly after changing your background color)
 * @note				A console does not save the drawing state. It is not automatically redrawn if the window is moved or
 * 						its visibility state is changed.
 *
 * @api
 */
GHandle gwinGConsoleCreate(GDisplay *g, GConsoleObject *gc, const GWindowInit *pInit);
#define gwinConsoleCreate(gc, pInit)			gwinGConsoleCreate(GDISP, gc, pInit)

#if GFX_USE_OS_CHIBIOS && GWIN_CONSOLE_USE_BASESTREAM
	/**
	 * @brief   Get a stream from a console window suitable for use with chprintf().
	 * @return	The stream handle or NULL if this is not a console window.
	 *
	 * @param[in] gh	The window handle (must be a console window)
	 *
	 * @note		Only useful in ChibiOS
	 *
	 * @api
	 */
	BaseSequentialStream *gwinConsoleGetStream(GHandle gh);
#endif

#if GWIN_CONSOLE_USE_HISTORY
	/**
	 * @brief	Assign a buffer to keep track of the content while the widget is invisible.
	 * @pre		GWIN_CONSOLE_USE_HISTORY must be set to TRUE in your gfxconf.h
	 *
	 * @param[in] gh		The window handle (must be a console window)
	 * @param[in] onoff		If TRUE a buffer is allocated to maintain console text
	 * 						when the console is obscured or invisible. If FALSE, then
	 * 						any existing buffer is deallocated.
	 * @note	When the history buffer is turned on, scrolling is implemented using the
	 * 			history buffer.
	 *
	 * @return	TRUE if the history buffer is now turned on.
	 */
	bool_t gwinConsoleSetBuffer(GHandle gh, bool_t onoff);
#endif

/**
 * @brief   Put a character at the cursor position in the window.
 * @note	Uses the current foreground color to draw the character and fills the background using the background drawing color
 *
 * @param[in] gh	The window handle (must be a console window)
 * @param[in] c		The character to draw
 *
 * @api
 */
void gwinPutChar(GHandle gh, char c);

/**
 * @brief   Put a string at the cursor position in the window. It will wrap lines as required.
 * @note	Uses the current foreground color to draw the string and fills the background using the background drawing color
 *
 * @param[in] gh	The window handle (must be a console window)
 * @param[in] str	The string to draw
 *
 * @api
 */
void gwinPutString(GHandle gh, const char *str);

/**
 * @brief   Put the character array at the cursor position in the window. It will wrap lines as required.
 * @note	Uses the current foreground color to draw the string and fills the background using the background drawing color
 *
 * @param[in] gh	The window handle (must be a console window)
 * @param[in] str	The string to draw
 * @param[in] n		The number of characters to draw
 *
 * @api
 */
void gwinPutCharArray(GHandle gh, const char *str, size_t n);

/**
 * @brief   Print a formatted string at the cursor position in the window. It will wrap lines as required.
 * @details This function implements a minimal printf() like functionality
 *          The general parameters format is: %[-][width|*][.precision|*][l|L]p.
 *          The following parameter types (p) are supported:
 *          - <b>x</b> hexadecimal integer.
 *          - <b>X</b> hexadecimal long.
 *          - <b>o</b> octal integer.
 *          - <b>O</b> octal long.
 *          - <b>d</b> decimal signed integer.
 *          - <b>D</b> decimal signed long.
 *          - <b>u</b> decimal unsigned integer.
 *          - <b>U</b> decimal unsigned long.
 *          - <b>c</b> character.
 *          - <b>s</b> string.
 * @note	Uses the current foreground color to draw the string and fills the background using the background drawing color
 *
 * @param[in] gh	The window handle (must be a console window)
 * @param[in] fmt	The format string (as per printf)
 * @param[in] ...	The format string arguments.
 *
 * @api
 */
void gwinPrintf(GHandle gh, const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif /* _GWIN_CONSOLE_H */
/** @} */
