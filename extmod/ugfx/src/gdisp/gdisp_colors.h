/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/gdisp/gdisp_colors.h
 *
 * @defgroup Colors Colors
 * @ingroup GDISP
 *
 * @brief   Sub-Module for color handling.
 *
 * @{
 */

#ifndef _GDISP_COLORS_H
#define _GDISP_COLORS_H

#include "../../gfx.h"

#if GFX_USE_GDISP || defined(__DOXYGEN__)

typedef uint16_t	colorformat;

/**
 * @name	Color system masks
 *
 * @details For pixel formats we do some assignment of codes to enable
 * 			format auto-calculation. (Undocumented).
 * 			0x2RGB			TRUECOLOR	RGB format, R = red bits, G = green bits, B = blue bits
 * 			0x3RGB			TRUECOLOR	BGR format, R = red bits, G = green bits, B = blue bits
 * 			0x40XX			GRAYSCALE	XX = bits
 * 			0x60XX			PALLETTE	XX = bits
 * 			0x8XXX			CUSTOM format.
 * @{
 */
#define GDISP_COLORSYSTEM_MASK			0xF000
#define GDISP_COLORSYSTEM_RGB			0x2000
#define GDISP_COLORSYSTEM_BGR			0x3000
/** @} */

/**
 * @name   Color Type Constants
 * @{
 */
#define GDISP_COLORSYSTEM_TRUECOLOR		0x2000
#define GDISP_COLORSYSTEM_GRAYSCALE		0x4000
#define GDISP_COLORSYSTEM_PALETTE		0x6000
/** @} */

/**
 * @name   Pixel Format Constants
 * @{
 */
#define GDISP_PIXELFORMAT_MONO		(GDISP_COLORSYSTEM_GRAYSCALE|0x0001)
#define GDISP_PIXELFORMAT_GRAY4		(GDISP_COLORSYSTEM_GRAYSCALE|0x0002)
#define GDISP_PIXELFORMAT_GRAY16	(GDISP_COLORSYSTEM_GRAYSCALE|0x0004)
#define GDISP_PIXELFORMAT_GRAY256	(GDISP_COLORSYSTEM_GRAYSCALE|0x0008)
#define GDISP_PIXELFORMAT_RGB565	(GDISP_COLORSYSTEM_RGB|0x0565)
#define GDISP_PIXELFORMAT_BGR565	(GDISP_COLORSYSTEM_BGR|0x0565)
#define GDISP_PIXELFORMAT_RGB555	(GDISP_COLORSYSTEM_RGB|0x0555)
#define GDISP_PIXELFORMAT_BGR555	(GDISP_COLORSYSTEM_BGR|0x0555)
#define GDISP_PIXELFORMAT_RGB888	(GDISP_COLORSYSTEM_RGB|0x0888)
#define GDISP_PIXELFORMAT_BGR888	(GDISP_COLORSYSTEM_BGR|0x0888)
#define GDISP_PIXELFORMAT_RGB444	(GDISP_COLORSYSTEM_RGB|0x0444)
#define GDISP_PIXELFORMAT_BGR444	(GDISP_COLORSYSTEM_BGR|0x0444)
#define GDISP_PIXELFORMAT_RGB332	(GDISP_COLORSYSTEM_RGB|0x0332)
#define GDISP_PIXELFORMAT_BGR332	(GDISP_COLORSYSTEM_BGR|0x0233)
#define GDISP_PIXELFORMAT_RGB233	(GDISP_COLORSYSTEM_RGB|0x0233)
#define GDISP_PIXELFORMAT_BGR233	(GDISP_COLORSYSTEM_BGR|0x0332)
#define GDISP_PIXELFORMAT_RGB666	(GDISP_COLORSYSTEM_RGB|0x0666)
#define GDISP_PIXELFORMAT_BGR666	(GDISP_COLORSYSTEM_BGR|0x0666)
#define GDISP_PIXELFORMAT_ERROR		0x0000
/** @} */

/**
 * @name   Some basic colors
 * @{
 */
#define White			HTML2COLOR(0xFFFFFF)
#define Black			HTML2COLOR(0x000000)
#define Gray			HTML2COLOR(0x808080)
#define Grey			Gray
#define Blue			HTML2COLOR(0x0000FF)
#define Red				HTML2COLOR(0xFF0000)
#define Fuchsia			HTML2COLOR(0xFF00FF)
#define Magenta			Fuchsia
#define Green			HTML2COLOR(0x008000)
#define Yellow			HTML2COLOR(0xFFFF00)
#define Aqua			HTML2COLOR(0x00FFFF)
#define Cyan			Aqua
#define Lime			HTML2COLOR(0x00FF00)
#define Maroon			HTML2COLOR(0x800000)
#define Navy			HTML2COLOR(0x000080)
#define Olive			HTML2COLOR(0x808000)
#define Purple			HTML2COLOR(0x800080)
#define Silver			HTML2COLOR(0xC0C0C0)
#define Teal			HTML2COLOR(0x008080)
#define Orange			HTML2COLOR(0xFFA500)
#define Pink			HTML2COLOR(0xFFC0CB)
#define SkyBlue			HTML2COLOR(0x87CEEB)
/** @} */

#if defined(__DOXYGEN__)
	/**
	 * @brief	The color system (grayscale, palette or truecolor)
	 */
	#define COLOR_SYSTEM			GDISP_COLORSYSTEM_TRUECOLOR

	/**
	 * @brief	The number of bits in a color value
	 */
	#define COLOR_BITS			16

	/**
	 * @name	Color bits
	 * @brief	The number of bits for each of red, green and blue
	 * @{
	 */
	#define COLOR_BITS_R		5
	#define COLOR_BITS_G		6
	#define COLOR_BITS_B		5
	/** @} */

	/**
	 * @name	Color bit shifts
	 * @brief	The number of bits to shift each of red, green and blue to put it in the correct place in the color
	 * @{
	 */
	#define COLOR_SHIFT_R		11
	#define COLOR_SHIFT_G		5
	#define COLOR_SHIFT_B		0
	/** @} */

	/**
	 * @brief	Does the color need masking to remove invalid bits
	 */
	#define COLOR_NEEDS_MASK	FALSE

	/**
	 * @brief	If the color needs masking to remove invalid bits, this is the mask
	 */
	#define COLOR_MASK			0xFFFF

	/**
	 * @brief   The color type
	 */
	#define COLOR_TYPE			uint16_t

	/**
	 * @brief	The number of bits in the color type (not necessarily the same as COLOR_BITS).
	 */
	#define COLOR_TYPE_BITS		16

	/**
	 * @brief   Convert a luminance (0 to 255) into a color value.
	 * @note	The word "Luma" is used instead of grey or gray due to the spelling ambiguities of the word grey
	 * @note	This is not a weighted luminance conversion in the color tv style.
	 * @note	@p LUMA2COLOR() uses a linear conversion (0.33R + 0.33G + 0.33B). Note this is different to color
	 * 			tv luminance (0.26126R + 0.7152G + 0.0722B),  digital tv luminance of (0.299R + 0.587G + 0.114B), or
	 * 			@p LUMA_OF() which uses (0.25R + 0.5G + 0.25B).
	 */
	#define LUMA2COLOR(l)		((color_t)((((l) & 0xF8)<<8) | (((l) & 0xFC)<<3) | (((l) & 0xF8)>>3)))

	/**
	 * @brief   Convert red, green, blue (each 0 to 255) into a color value.
	 */
	#define RGB2COLOR(r,g,b)	((color_t)((((r) & 0xF8)<<8) | (((g) & 0xFC)<<3) | (((b) & 0xF8)>>3)))

	/**
	 * @brief   Convert a 6 digit HTML code (hex) into a color value.
	 */
	#define HTML2COLOR(h)		((color_t)((((h) & 0xF80000)>>8) | (((h) & 0x00FC00)>>5) | (((h) & 0x0000F8)>>3)))

	/**
	 * @name	Extraction macros (quick)
	 *
	 * @brief   Extract the luma/red/green/blue component (0 to 255) of a color value.
	 * @note	This uses quick and dirty bit shifting. If you want more exact colors
	 * 			use @p EXACT_RED_OF() etc which uses multiplies and divides. For constant
	 * 			colors using @p EXACT_RED_OF() is no more expensive because the compiler
	 * 			evaluates the arithmetic.
	 * @note	@p LUMA_OF() returns a roughly weighted luminance (0.25R + 0.5G + 0.25B). Note this is
	 * 			different to @p LUMA2COLOR() which uses a linear conversion (0.33R + 0.33G + 0.33B) and
	 * 			color tv luminance of (0.26126R + 0.7152G + 0.0722B) and digital tv luminance of (0.299R + 0.587G + 0.114B).
	 * @note	A 5 bit color component maximum value (0x1F) converts to 0xF8 (slightly off-color)
	 * @{
	 */
	#define LUMA_OF(c)			((RED_OF(c)+((uint16_t)GREEN_OF(c)<<1)+BLUE_OF(c))>>2)
	#define RED_OF(c)			(((c) & 0xF800)>>8)
	#define GREEN_OF(c)			(((c)&0x007E)>>3)
	#define BLUE_OF(c)			(((c)&0x001F)<<3)
	/** @} */

	/**
	 * @name	Extraction macros (precise)
	 *
	 * @brief   Extract the exact luma/red/green/blue component (0 to 255) of a color value.
	 * @note	This uses multiplies and divides rather than bit shifting.
	 * 			This gives exact equivalent colors at the expense of more cpu intensive
	 * 			operations. Note for constants this is no more expensive than @p REF_OF()
	 * 			because the compiler evaluates the arithmetic.
	 * @note	@p EXACT_LUMA_OF() returns a roughly weighted luminance (0.25R + 0.5G + 0.25B). Note this is
	 * 			different to @p LUMA2COLOR() which uses a linear conversion (0.33R + 0.33G + 0.33B) and
	 * 			color tv luminance of (0.26126R + 0.7152G + 0.0722B) and digital tv luminance of (0.299R + 0.587G + 0.114B).
	 * @note	A 5 bit color component maximum value (0x1F) converts to 0xFF (the true equivalent color)
	 * @{
	 */
	#define EXACT_LUMA_OF(c)	((EXACT_RED_OF(c)+((uint16_t)EXACT_GREEN_OF(c)<<1)+EXACT_BLUE_OF(c))>>2)
	#define EXACT_RED_OF(c)		(((((c)>>11)&0x1F)*255)/31)
	#define EXACT_GREEN_OF(c)	(((((c)>>5)&0x3F)*255)/63)
	#define EXACT_BLUE_OF(c)	(((((c)>>0)&0x1F)*255)/31)
	/** @} */
#endif

/*
 * We use this big mess of macros to calculate all the components
 * to prevent user errors in the color definitions. It greatly simplifies
 * the above definitions and ensures a consistent implementation.
 */

//-------------------------
//	True-Color color system
//-------------------------
#if GDISP_PIXELFORMAT & GDISP_COLORSYSTEM_TRUECOLOR
	#define COLOR_SYSTEM			GDISP_COLORSYSTEM_TRUECOLOR

	// Calculate the number of bits
	#define COLOR_BITS_R			((GDISP_PIXELFORMAT>>8) & 0x0F)
	#define COLOR_BITS_G			((GDISP_PIXELFORMAT>>4) & 0x0F)
	#define COLOR_BITS_B			((GDISP_PIXELFORMAT>>0) & 0x0F)
	#define COLOR_BITS				(COLOR_BITS_R + COLOR_BITS_G + COLOR_BITS_B)

	// From the number of bits determine COLOR_TYPE, COLOR_TYPE_BITS and masking
	#if COLOR_BITS <= 8
		#define COLOR_TYPE			uint8_t
		#define COLOR_TYPE_BITS		8
	#elif COLOR_BITS <= 16
		#define COLOR_TYPE			uint16_t
		#define COLOR_TYPE_BITS		16
	#elif COLOR_BITS <= 32
		#define COLOR_TYPE			uint32_t
		#define COLOR_TYPE_BITS		32
	#else
		#error "GDISP: Cannot define color types with more than 32 bits"
	#endif
	#if COLOR_TYPE_BITS == COLOR_BITS
		#define COLOR_NEEDS_MASK	FALSE
	#else
		#define COLOR_NEEDS_MASK	TRUE
	#endif
	#define COLOR_MASK()			((1 << COLOR_BITS)-1)

	// Calculate the component bit shifts
	#if (GDISP_PIXELFORMAT & GDISP_COLORSYSTEM_MASK) == GDISP_COLORSYSTEM_RGB
		#define COLOR_SHIFT_R		(COLOR_BITS_B+COLOR_BITS_G)
		#define COLOR_SHIFT_G		COLOR_BITS_B
		#define COLOR_SHIFT_B		0
	#else
		#define COLOR_SHIFT_B		(COLOR_BITS_R+COLOR_BITS_G)
		#define COLOR_SHIFT_G		COLOR_BITS_R
		#define COLOR_SHIFT_R		0
	#endif

	// Calculate RED_OF, GREEN_OF, BLUE_OF and RGB2COLOR
	#if COLOR_BITS_R + COLOR_SHIFT_R == 8
		#define RED_OF(c)			((c) & (((1<<COLOR_BITS_R)-1) << COLOR_SHIFT_R))
		#define RGB2COLOR_R(r)		((COLOR_TYPE)((r) & (0xFF & ~((1<<(8-COLOR_BITS_R))-1))))
	#elif COLOR_BITS_R + COLOR_SHIFT_R > 8
		#define RED_OF(c)			(((c) & (((1<<COLOR_BITS_R)-1) << COLOR_SHIFT_R)) >> (COLOR_BITS_R+COLOR_SHIFT_R-8))
		#define RGB2COLOR_R(r)		(((COLOR_TYPE)((r) & (0xFF & ~((1<<(8-COLOR_BITS_R))-1)))) << (COLOR_BITS_R+COLOR_SHIFT_R-8))
	#else // COLOR_BITS_R + COLOR_SHIFT_R < 8
		#define RED_OF(c)			(((c) & (((1<<COLOR_BITS_R)-1) << COLOR_SHIFT_R)) << (8-(COLOR_BITS_R+COLOR_SHIFT_R)))
		#define RGB2COLOR_R(r)		(((COLOR_TYPE)((r) & (0xFF & ~((1<<(8-COLOR_BITS_R))-1)))) >> (8-(COLOR_BITS_R+COLOR_SHIFT_R)))
	#endif
	#if COLOR_BITS_G + COLOR_SHIFT_G == 8
		#define GREEN_OF(c)			((c) & (((1<<COLOR_BITS_G)-1) << COLOR_SHIFT_G))
		#define RGB2COLOR_G(g)		((COLOR_TYPE)((g) & (0xFF & ~((1<<(8-COLOR_BITS_G))-1))))
	#elif COLOR_BITS_G + COLOR_SHIFT_G > 8
		#define GREEN_OF(c)			(((c) & (((1<<COLOR_BITS_G)-1) << COLOR_SHIFT_G)) >> (COLOR_BITS_G+COLOR_SHIFT_G-8))
		#define RGB2COLOR_G(g)		(((COLOR_TYPE)((g) & (0xFF & ~((1<<(8-COLOR_BITS_G))-1)))) << (COLOR_BITS_G+COLOR_SHIFT_G-8))
	#else // COLOR_BITS_G + COLOR_SHIFT_G < 8
		#define GREEN_OF(c)			(((c) & (((1<<COLOR_BITS_G)-1) << COLOR_SHIFT_G)) << (8-(COLOR_BITS_G+COLOR_SHIFT_G)))
		#define RGB2COLOR_G(g)		(((COLOR_TYPE)((g) & (0xFF & ~((1<<(8-COLOR_BITS_G))-1)))) >> (8-(COLOR_BITS_G+COLOR_SHIFT_G)))
	#endif
	#if COLOR_BITS_B + COLOR_SHIFT_B == 8
		#define BLUE_OF(c)			((c) & (((1<<COLOR_BITS_B)-1) << COLOR_SHIFT_B))
		#define RGB2COLOR_B(b)		((COLOR_TYPE)((b) & (0xFF & ~((1<<(8-COLOR_BITS_B))-1))))
	#elif COLOR_BITS_B + COLOR_SHIFT_B > 8
		#define BLUE_OF(c)			(((c) & (((1<<COLOR_BITS_B)-1) << COLOR_SHIFT_B)) >> (COLOR_BITS_B+COLOR_SHIFT_B-8))
		#define RGB2COLOR_B(b)		(((COLOR_TYPE)((b) & (0xFF & ~((1<<(8-COLOR_BITS_B))-1)))) << (COLOR_BITS_B+COLOR_SHIFT_B-8))
	#else // COLOR_BITS_B + COLOR_SHIFT_B < 8
		#define BLUE_OF(c)			(((c) & (((1<<COLOR_BITS_B)-1) << COLOR_SHIFT_B)) << (8-(COLOR_BITS_B+COLOR_SHIFT_B)))
		#define RGB2COLOR_B(b)		(((COLOR_TYPE)((b) & (0xFF & ~((1<<(8-COLOR_BITS_B))-1)))) >> (8-(COLOR_BITS_B+COLOR_SHIFT_B)))
	#endif
	#define LUMA_OF(c)				((RED_OF(c)+((uint16_t)GREEN_OF(c)<<1)+BLUE_OF(c))>>2)
	#define EXACT_RED_OF(c)			(((uint16_t)(((c)>>COLOR_SHIFT_R)&((1<<COLOR_BITS_R)-1))*255)/((1<<COLOR_BITS_R)-1))
	#define EXACT_GREEN_OF(c)		(((uint16_t)(((c)>>COLOR_SHIFT_G)&((1<<COLOR_BITS_G)-1))*255)/((1<<COLOR_BITS_G)-1))
	#define EXACT_BLUE_OF(c)		(((uint16_t)(((c)>>COLOR_SHIFT_B)&((1<<COLOR_BITS_B)-1))*255)/((1<<COLOR_BITS_B)-1))
	#define EXACT_LUMA_OF(c)		((EXACT_RED_OF(c)+((uint16_t)EXACT_GREEN_OF(c)<<1)+EXACT_BLUE_OF(c))>>2)
	#define LUMA2COLOR(l)			(RGB2COLOR_R(l) | RGB2COLOR_G(l) | RGB2COLOR_B(l))
	#define RGB2COLOR(r,g,b)		(RGB2COLOR_R(r) | RGB2COLOR_G(g) | RGB2COLOR_B(b))

	// Calculate HTML2COLOR
	#if COLOR_BITS_R + COLOR_SHIFT_R == 24
		#define HTML2COLOR_R(h)		((h) & ((0xFFL & ~((1<<(8-COLOR_BITS_R))-1))<<16))
	#elif COLOR_BITS_R + COLOR_SHIFT_R > 24
		#define HTML2COLOR_R(h)		(((h) & ((0xFFL & ~((1<<(8-COLOR_BITS_R))-1))<<16)) << (COLOR_BITS_R+COLOR_SHIFT_R-24))
	#else // COLOR_BITS_R + COLOR_SHIFT_R < 24
		#define HTML2COLOR_R(h)		(((h) & ((0xFFL & ~((1<<(8-COLOR_BITS_R))-1))<<16)) >> (24-(COLOR_BITS_R+COLOR_SHIFT_R)))
	#endif
	#if COLOR_BITS_G + COLOR_SHIFT_G == 16
		#define HTML2COLOR_G(h)		((h) & ((0xFFL & ~((1<<(8-COLOR_BITS_G))-1))<<8))
	#elif COLOR_BITS_G + COLOR_SHIFT_G > 16
		#define HTML2COLOR_G(h)		(((h) & ((0xFFL & ~((1<<(8-COLOR_BITS_G))-1))<<8)) << (COLOR_BITS_G+COLOR_SHIFT_G-16))
	#else // COLOR_BITS_G + COLOR_SHIFT_G < 16
		#define HTML2COLOR_G(h)		(((h) & ((0xFFL & ~((1<<(8-COLOR_BITS_G))-1))<<8)) >> (16-(COLOR_BITS_G+COLOR_SHIFT_G)))
	#endif
	#if COLOR_BITS_B + COLOR_SHIFT_B == 8
		#define HTML2COLOR_B(h)		((h) & (0xFFL & ~((1<<(8-COLOR_BITS_B))-1)))
	#elif COLOR_BITS_B + COLOR_SHIFT_B > 8
		#define HTML2COLOR_B(h)		(((h) & (0xFFL & ~((1<<(8-COLOR_BITS_B))-1))) << (COLOR_BITS_B+COLOR_SHIFT_B-8))
	#else // COLOR_BITS_B + COLOR_SHIFT_B < 8
		#define HTML2COLOR_B(h)		(((h) & (0xFFL & ~((1<<(8-COLOR_BITS_B))-1))) >> (8-(COLOR_BITS_B+COLOR_SHIFT_B)))
	#endif
	#define HTML2COLOR(h)		((COLOR_TYPE)(HTML2COLOR_R(h) | HTML2COLOR_G(h) | HTML2COLOR_B(h)))

//-------------------------
//	Gray-scale color system
//-------------------------
#elif (GDISP_PIXELFORMAT & GDISP_COLORSYSTEM_MASK) == GDISP_COLORSYSTEM_GRAYSCALE
	#define COLOR_SYSTEM			GDISP_COLORSYSTEM_GRAYSCALE

	// Calculate the number of bits and shifts
	#define COLOR_BITS			(GDISP_PIXELFORMAT & 0xFF)
	#define COLOR_BITS_R		COLOR_BITS
	#define COLOR_BITS_G		COLOR_BITS
	#define COLOR_BITS_B		COLOR_BITS
	#define COLOR_SHIFT_R		0
	#define COLOR_SHIFT_G		0
	#define COLOR_SHIFT_B		0

	// From the number of bits determine COLOR_TYPE, COLOR_TYPE_BITS and masking
	#if COLOR_BITS <= 8
		#define COLOR_TYPE			uint8_t
		#define COLOR_TYPE_BITS		8
	#else
		#error "GDISP: Cannot define gray-scale color types with more than 8 bits"
	#endif
	#if COLOR_TYPE_BITS == COLOR_BITS
		#define COLOR_NEEDS_MASK	FALSE
	#else
		#define COLOR_NEEDS_MASK	TRUE
	#endif
	#define COLOR_MASK()			((1 << COLOR_BITS)-1)

	#if COLOR_BITS == 1
		#define RGB2COLOR(r,g,b)	(((r)|(g)|(b)) ? 1 : 0)
		#define LUMA2COLOR(l)		((l) ? 1 : 0)
		#define HTML2COLOR(h)		((h) ? 1 : 0)
		#define LUMA_OF(c)			((c) ? 255 : 0)
		#define EXACT_LUMA_OF(c)	LUMA_OF(c)
	#else
		// They eye is more sensitive to green
		#define RGB2COLOR(r,g,b)	((COLOR_TYPE)(((uint16_t)(r)+(g)+(g)+(b)) >> (10-COLOR_BITS)))
		#define LUMA2COLOR(l)		((COLOR_TYPE)((l)>>(8-COLOR_BITS)))
		#define HTML2COLOR(h)		((COLOR_TYPE)(((((h)&0xFF0000)>>16)+(((h)&0x00FF00)>>7)+((h)&0x0000FF)) >> (10-COLOR_BITS)))
		#define LUMA_OF(c)			(((c) & ((1<<COLOR_BITS)-1)) << (8-COLOR_BITS))
		#define EXACT_LUMA_OF(c)	((((uint16_t)(c) & ((1<<COLOR_BITS)-1))*255)/((1<<COLOR_BITS)-1))
	#endif

	#define RED_OF(c)			LUMA_OF(c)
	#define GREEN_OF(c)			LUMA_OF(c)
	#define BLUE_OF(c)			LUMA_OF(c)
	#define EXACT_RED_OF(c)		EXACT_LUMA_OF(c)
	#define EXACT_GREEN_OF(c)	EXACT_LUMA_OF(c)
	#define EXACT_BLUE_OF(c)	EXACT_LUMA_OF(c)

//-------------------------
//	Palette color system
//-------------------------
#elif (GDISP_PIXELFORMAT & GDISP_COLORSYSTEM_MASK) == GDISP_COLORSYSTEM_PALETTE
	#define COLOR_SYSTEM			GDISP_COLORSYSTEM_PALETTE

	#error "GDISP: A palette color system is not currently supported"

//-------------------------
//	Some other color system
//-------------------------
#else
	#error "GDISP: Unsupported color system"
#endif

/**
 * @brief	The color type definition
 */
typedef COLOR_TYPE			color_t;

#endif /* GFX_USE_GDISP */

#endif /* _GDISP_COLORS_H */
/** @} */
