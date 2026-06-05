/*
 * BADGE.TEAM framebuffer driver
 * Uses parts of the Adafruit GFX Arduino libray
 */

/*
This is the core graphics library for all our displays, providing a common
set of graphics primitives (points, lines, circles, etc.).  It needs to be
paired with a hardware-specific library for each display device we carry
(to handle the lower-level functions).

Adafruit invests time and resources providing this open source code, please
support Adafruit & open-source hardware by purchasing products from Adafruit!

Copyright (c) 2013 Adafruit Industries.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

- Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
 */

#include "include/driver_framebuffer_internal.h"

#include "esp_heap_caps.h"
#include "esp_log.h"

#define TAG "fb-text"

#ifdef CONFIG_DRIVER_FRAMEBUFFER_ENABLE

/* Fonts */

const char* fontNames[] = {
	"org18",                //Legacy font name
	"org01_8",              //
	"fairlight8",           //
	"fairlight12",          //
	"dejavusans20",         //SHA2017
	"permanentmarker22",    //SHA2017
	"permanentmarker36",    //SHA2017
	"roboto_black22",       //SHA2017
	"roboto_blackitalic24", //SHA2017
	"roboto_regular12",     //SHA2017
	"roboto_regular18",     //SHA2017
	"roboto_regular22",     //SHA2017
	"weather42",            //SHA2017
	"pixelade13",           //SHA2017
	"7x5",                  //CAMPZONE2019
	"ocra16",               //CYBER
	"ocra22",               //CYBER
	"exo2_regular22",       //TROOPERS
	"exo2_thin22",          //TROOPERS
	"exo2_bold22",          //TROOPERS
	"exo2_regular18",       //TROOPERS
	"exo2_thin18",          //TROOPERS
	"exo2_bold18",          //TROOPERS
	"exo2_regular12",       //TROOPERS
	"exo2_thin12",          //TROOPERS
	"exo2_bold12",          //TROOPERS
	"press_start_2p6",      //MCH2022
	"press_start_2p8",      //MCH2022
	"press_start_2p9",      //MCH2022
	"press_start_2p12",     //MCH2022
	"press_start_2p18",     //MCH2022
	"press_start_2p22",     //MCH2022
	NULL
};

const GFXfont* fontPointers[] = {
	&org_01_8pt7b,
	&org_01_8pt7b,
	&fairlight_8pt7b,
	&fairlight_12pt7b,
	&dejavusans_20pt7b,
	&permanentmarker_22pt7b,
	&permanentmarker_36pt7b,
	&roboto_Black_22pt7b,
	&roboto_BlackItalic_24pt7b,
	&roboto_12pt7b,
	&roboto_18pt7b,
	&roboto_22pt7b,
	&weather_42pt8b,
	&roboto_12pt7b,
	&ipane7x5,
	&ocra_16pt7b,
	&ocra_22pt7b,
	&Exo2_Regular_22pt7b,
	&Exo2_Thin_22pt7b,
	&Exo2_Bold_22pt7b,
	&Exo2_Regular_18pt7b,
	&Exo2_Thin_18pt7b,
	&Exo2_Bold_18pt7b,
	&Exo2_Regular_12pt7b,
	&Exo2_Thin_12pt7b,
	&Exo2_Bold_12pt7b,
	&PressStart2P_6pt7b,
	&PressStart2P_8pt7b,
	&PressStart2P_9pt7b,
	&PressStart2P_12pt7b,
	&PressStart2P_18pt7b,
	&PressStart2P_22pt7b,
};

/* Private functions */
// Draws a character to the screen
void _print_char(Window* window, unsigned char c, int16_t x0, int16_t y0, uint8_t xScale, uint8_t yScale, uint32_t color, const GFXfont *font)
{
	if ((c < font->first) || (c > font->last)) {
		ESP_LOGE(TAG, "print_char called with unprintable character");
		return;
	}

	c -= (uint8_t) font->first;
	const GFXglyph *glyph   = font->glyph + c;
	const uint8_t  *bitmap  = font->bitmap;

	uint16_t bitmapOffset = glyph->bitmapOffset;
	uint8_t  width        = glyph->width;
	uint8_t  height       = glyph->height;
	int8_t   xOffset      = glyph->xOffset;
	int8_t   yOffset      = glyph->yOffset;

	uint8_t  bit = 0, bits = 0;
	for (uint8_t y = 0; y < height; y++) {
		for (uint8_t x = 0; x < width; x++) {
			if(!(bit++ & 7)) bits = bitmap[bitmapOffset++];
			if(bits & 0x80) {
				if (xScale == 1 && yScale == 1) {
					driver_framebuffer_setPixel(window, x0+xOffset+x, y0+yOffset+y-1, color);
				} else {
					driver_framebuffer_rect(window, x0+(xOffset+x)*xScale, y0+(yOffset+y)*yScale-1, xScale, yScale, true, color);
				}
			}
			bits <<= 1;
		}
	}
}

// Draws a string to the screen
void _write(Window* window, uint8_t c, int16_t x0, int16_t *x, int16_t *y, uint8_t xScale, uint8_t yScale, uint32_t color, const GFXfont *font)
{
	if (font == NULL) { ESP_LOGE(TAG, "write called without font"); return; }
	const GFXglyph *glyph = font->glyph + c - (uint8_t) font->first;
	if (c == '\n') {
		*x = x0;
		*y += font->yAdvance * yScale;
	} else if (c != '\r') {
		_print_char(window, c, *x, *y+(font->yAdvance*yScale), xScale, yScale, color, font);
		*x += glyph->xAdvance * xScale;
	}
}

#ifdef CONFIG_G_NEW_TEXT
// Maps a character directly onto a texture
void _print_char_texture(texture_2d* __restrict__ texture, unsigned char c, int16_t x0, int16_t y0, uint8_t xScale, uint8_t yScale, uint32_t color, const GFXfont *font)
{
	if ((c < font->first) || (c > font->last)) {
		ESP_LOGE(TAG, "print_char_texture called with unprintable character");
		return;
	}

	c -= (uint8_t) font->first;
	const GFXglyph *glyph   = font->glyph + c;
	const uint8_t  *bitmap  = font->bitmap;

	uint16_t bitmapOffset = glyph->bitmapOffset;
	uint8_t  width        = glyph->width;
	uint8_t  height       = glyph->height;
	int8_t   xOffset      = glyph->xOffset;
	int8_t   yOffset      = glyph->yOffset;

	uint8_t  bit = 0, bits = 0;
	for (uint8_t y = 0; y < height; y++) {
		for (uint8_t x = 0; x < width; x++) {
			if(!(bit++ & 7)) bits = bitmap[bitmapOffset++];
			if(bits & 0x80) {
				if (xScale == 1 && yScale == 1) {
					int resX = x0+xOffset+x;
					int resY = y0+yOffset+y-1;
					if (resX >= 0 && resX < texture->width && resY >= 0 && resY < texture->height) {
						texture->buffer[resX + resY * texture->width] = color | 0xff000000;
					}
				} else {
					int startX = x0+(xOffset+x)*xScale;
					int startY = y0+(yOffset+y)*yScale-1;
					for (int resY = startY; resY < startY + yScale; resY++) {
						for (int resX = startX; resX < startX + xScale; resX++) {
							if (resX >= 0 && resX < texture->width && resY >= 0 && resY < texture->height) {
								texture->buffer[resX + resY * texture->width] = color | 0xff000000;
							}
						}
					}
				}
			}
			bits <<= 1;
		}
	}
}

//Maps a string to a texture
void _write_texture(texture_2d* __restrict__ texture, uint8_t c, int16_t x0, int16_t *x, int16_t *y, uint8_t xScale, uint8_t yScale, uint32_t color, const GFXfont *font)
{
	if (font == NULL) { ESP_LOGE(TAG, "write_texture called without font"); return; }
	const GFXglyph *glyph = font->glyph + c - (uint8_t) font->first;
	if (c == '\n') {
		*x = x0;
		*y += font->yAdvance * yScale;
	} else if (c != '\r') {
		_print_char_texture(texture, c, *x, *y+(font->yAdvance*yScale), xScale, yScale, color, font);
		*x += glyph->xAdvance * xScale;
	}
}
#endif

uint16_t _char_width(uint8_t c, const GFXfont *font)
{
	if (font == NULL) return 0;
	if ((c < font->first) || (c > font->last)) return 0;
	const GFXglyph *glyph = font->glyph + c - (uint8_t) font->first;
	if ((c == '\r') || (c == '\n')) return 0;
	return glyph->xAdvance;
}

/* Public functions */

const GFXfont* driver_framebuffer_findFontByName(const char* fontName)
{
	char buffer[32];
	if (strlen(fontName) > 31) return NULL;
	strcpy(buffer, fontName);
	strlwr(buffer);
	uint8_t i = 0;
	while (fontNames[i] != NULL) {
		if (strcmp(fontNames[i],buffer)==0) {
			return fontPointers[i];
		}
		i++;
	}
	return NULL;
}

uint16_t driver_framebuffer_print(Window* window, const char* str, int16_t x0, int16_t y0, uint8_t xScale, uint8_t yScale, uint32_t color, const GFXfont *font)
{
	matrix_stack_2d *stack;
	if (window == NULL) {
		stack = &stack_2d_global;
	}
	else
	{
		stack = window->stack_2d;
	}

	int16_t x;
	int16_t y;
	#ifdef CONFIG_G_NEW_TEXT
	if (matrix_2d_is_identity(stack->current)) {
	#endif
		x = x0;
		y = y0;
		// We can run the non-matrix code if the matrix is (effectively) identity
		for (uint16_t i = 0; i < strlen(str); i++) {
			_write(window, str[i], x0, &x, &y, xScale, yScale, color, font);
		}
	#ifdef CONFIG_G_NEW_TEXT
	}
	else
	{
		// Otherwise, we need to call the matrix functions
		// Note that this is one of the only functions which absolutely requires alpha blending to work

		// Get size of text and allocate a texture
		uint16_t textWidth = driver_framebuffer_get_string_width(str, font);
		uint16_t textHeight = driver_framebuffer_get_string_height(str, font);
		texture_2d *texture = (texture_2d *) malloc(sizeof(texture_2d));
		texture->width = textWidth;
		texture->height = textHeight;
		texture->buffer = (uint32_t *) malloc(sizeof(uint32_t) * textWidth * textHeight);
		for (int i = 0; i < textWidth * textHeight; i++) {
			// Zero out the entire texture, set alpha to 0
			texture->buffer[i] = 0;
		}

		// Map the text onto the texture
		x = 0;
		y = 0;
		for (uint16_t i = 0; i < strlen(str); i++) {
			_write_texture(texture, str[i], x0, &x, &y, xScale, yScale, color, font);
		}
		
		// Draw using textured triangles
		float tx0 = x0;
		float ty0 = y0;
		float tx1 = x0 + textWidth;
		float ty1 = y0;
		float tx2 = x0 + textWidth;
		float ty2 = y0 + textHeight;
		float tx3 = x0;
		float ty3 = y0 + textHeight;
		matrix_2d_transform_point(stack->current, &tx0, &ty0);
		matrix_2d_transform_point(stack->current, &tx1, &ty1);
		matrix_2d_transform_point(stack->current, &tx2, &ty2);
		matrix_2d_transform_point(stack->current, &tx3, &ty3);
		triangle_uv uv0 = {
			.u0 = 0,
			.v0 = 0,
			.u1 = 1,
			.v1 = 0,
			.u2 = 1,
			.v2 = 1
		};
		driver_framebuffer_triangle_textured(window, tx0, ty0, tx1, ty1, tx2, ty2, uv0, texture, &shader_2d_nolerp);
		triangle_uv uv1 = {
			.u0 = 0,
			.v0 = 0,
			.u1 = 0,
			.v1 = 1,
			.u2 = 1,
			.v2 = 1
		};
		driver_framebuffer_triangle_textured(window, tx0, ty0, tx3, ty3, tx2, ty2, uv1, texture, &shader_2d_nolerp);

		// Clean up
		free(texture->buffer);
		free(texture);
	}
	#endif

	return y;
}

uint16_t driver_framebuffer_print_len(Window* window, const char* str, int16_t len, int16_t x0, int16_t y0, uint8_t xScale, uint8_t yScale, uint32_t color, const GFXfont *font)
{
	int16_t x = x0, y = y0;
	for (uint16_t i = 0; i < len; i++) {
		_write(window, str[i], x0, &x, &y, xScale, yScale, color, font);
	}
	return y;
}

uint16_t driver_framebuffer_get_string_width(const char* str, const GFXfont *font)
{
	uint16_t width = 0;
	uint16_t maxWidth = 0;
	for (uint16_t i = 0; i < strlen(str); i++) {
		if (str[i] == '\n') {
			if (maxWidth < width) maxWidth = width;
			width = 0;
		} else {
			width += _char_width(str[i], font);
		}
	}
	if (maxWidth > width) width = maxWidth;
	return width;
}

uint16_t driver_framebuffer_get_string_height(const char* str, const GFXfont *font)
{
	uint16_t height = font->yAdvance;
	if (strlen(str) < 1) return 0;
	for (uint16_t i = 0; i < strlen(str)-1; i++) {
		if (str[i]=='\n') height += font->yAdvance;
	}
	return height;
}

#endif
