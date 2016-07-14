/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/* Configuration constants for mcufont. */

#ifndef _MF_CONFIG_H_
#define _MF_CONFIG_H_

/*******************************************************
 * Configuration settings related to GFX               *
 *******************************************************/

#include "../../../gfx.h"

#if !GFX_USE_GDISP || !GDISP_NEED_TEXT
	#define MF_NO_COMPILE				// Don't compile any font code
#endif

/* Prevent double definitions of standard int types */
#define MF_NO_STDINT_H

/* Mapping from uGFX settings to mcufont settings */
#if GDISP_NEED_UTF8
	#define MF_ENCODING MF_ENCODING_UTF8
#else
	#define MF_ENCODING MF_ENCODING_ASCII
#endif

#if GDISP_NEED_TEXT_WORDWRAP
	#define MF_USE_ADVANCED_WORDWRAP 1
#else
	#define MF_USE_ADVANCED_WORDWRAP 0
#endif

#define MF_USE_KERNING GDISP_NEED_TEXT_KERNING
#define MF_FONT_FILE_NAME "src/gdisp/fonts/fonts.h"

/* These are not used for now */
#define MF_USE_JUSTIFY 0

/*******************************************************
 * Configuration settings related to build environment *
 *******************************************************/

/* Name of the file that contains all the included fonts. */
#ifndef MF_FONT_FILE_NAME
#define MF_FONT_FILE_NAME "fonts.h"
#endif


/*****************************************
 * Configuration settings related to API *
 *****************************************/

/* Encoding for the input data.
 * With the unicode encodings, the library supports the range of unicode
 * characters 0x0000-0xFFFF (the Basic Multilingual Plane).
 * 
 * ASCII: Plain ascii (somewhat works with ISO8859-1 also)
 * UTF8:  UTF8 encoding (variable number of bytes)
 * UTF16: UTF16 encoding (2 bytes per character, compatible with UCS-2)
 * WCHAR: Use compiler's wchar_t (usually same as UTF16)
 */
#define MF_ENCODING_ASCII 0
#define MF_ENCODING_UTF8  1
#define MF_ENCODING_UTF16 2
#define MF_ENCODING_WCHAR 3
#ifndef MF_ENCODING
#define MF_ENCODING MF_ENCODING_UTF8
#endif


/************************************************************************
 * Configuration settings related to visual appearance of rendered text *
 ************************************************************************/

/* Minimum space between characters, in percents of the glyph width.
 * Increasing this causes the kerning module to leave more space between
 * characters.
 */
#ifndef MF_KERNING_SPACE_PERCENT
#define MF_KERNING_SPACE_PERCENT 15
#endif

/* Minimum space between characters, in pixels. Added to the percentual
 * spacing. This pixel-based value guarantees enough space even with small
 * fonts.
 */
#ifndef MF_KERNING_SPACE_PIXELS
#define MF_KERNING_SPACE_PIXELS 3
#endif

/* Maximum adjustment done by the kerning algorithm, as percent of the
 * glyph width.
 */
#ifndef MF_KERNING_LIMIT
#define MF_KERNING_LIMIT 20
#endif

/* Spacing of tabulator stops. The value is multiplied by the width of the
 * 'm' character in the current font.
 */
#ifndef MF_TABSIZE
#define MF_TABSIZE 8
#endif


/*************************************************************************
 * Configuration settings to strip down library to reduce resource usage *
 *************************************************************************/

/* Enable or disable the kerning module.
 * Disabling it saves some code size and run time, but causes the spacing
 * between characters to be less consistent.
 */
#ifndef MF_USE_KERNING
#define MF_USE_KERNING 1
#endif

/* Enable or disable the advanced word wrap algorithm.
 * If disabled, uses a simpler algorithm.
 */
#ifndef MF_USE_ADVANCED_WORDWRAP
#define MF_USE_ADVANCED_WORDWRAP 1
#endif

/* Enable of disable the justification algorithm.
 * If disabled, mf_render_justified renders just left-aligned.
 */
#ifndef MF_USE_JUSTIFY
#define MF_USE_JUSTIFY 1
#endif

/* Enable or disable the center and right alignment code.
 * If disabled, any alignment results in MF_ALIGN_LEFT.
 */
#ifndef MF_USE_ALIGN
#define MF_USE_ALIGN 1
#endif

/* Enable or disable the support for tab alignment.
 * If disabled, tabs will be rendered as regular space character.
 */
#ifndef MF_USE_TABS
#define MF_USE_TABS 1
#endif

/* Number of vertical zones to use when computing kerning.
 * Larger values give more accurate kerning, but are slower and use somewhat
 * more memory. There is no point to increase this beyond the height of the
 * font.
 */
#ifndef MF_KERNING_ZONES
#define MF_KERNING_ZONES 16
#endif



/* Add extern "C" when used from C++. */
#ifdef __cplusplus
#define MF_EXTERN extern "C"
#else
#define MF_EXTERN
#endif

#endif

