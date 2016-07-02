/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/* Generic font type that supports fonts with multiple kinds of compression.
 * Provides an interface for decoding and rendering single characters.
 */

#ifndef _MF_FONT_H_
#define _MF_FONT_H_

#include "mf_encoding.h"

/* Callback function that writes pixels to screen / buffer / whatever.
 *
 * x:     X coordinate of the first pixel to write.
 * y:     Y coordinate of the first pixel to write.
 * count: Number of pixels to fill (horizontally).
 * alpha: The "opaqueness" of the pixels, 0 for background, 255 for text.
 * state: Free variable that was passed to render_character().
 */
typedef void (*mf_pixel_callback_t) (int16_t x, int16_t y, uint8_t count,
                                     uint8_t alpha, void *state);

/* General information about a font. */
struct mf_font_s
{
    /* Full name of the font, comes from the original font file. */
    const char *full_name;
    
    /* Short name of the font, comes from file name. */
    const char *short_name;
    
    /* Width and height of the character bounding box. */
    uint8_t width;
    uint8_t height;
    
    /* Minimum and maximum tracking width of characters. */
    uint8_t min_x_advance;
    uint8_t max_x_advance;
    
    /* Location of the text baseline relative to character. */
    uint8_t baseline_x;
    uint8_t baseline_y;
    
    /* Line height of the font (vertical advance). */
    uint8_t line_height;
    
    /* Flags identifying various aspects of the font. */
    uint8_t flags;
    
    /* Fallback character to use for missing glyphs. */
    uint16_t fallback_character;
    
    /* Function to get character width. Should return 0 if character is
     * not found. */
    uint8_t (*character_width)(const struct mf_font_s *font, uint16_t character);
    
    /* Function to render a character. Returns the character width or 0 if
     * character is not found. */
    uint8_t (*render_character)(const struct mf_font_s *font,
                                int16_t x0, int16_t y0,
                                uint16_t character,
                                mf_pixel_callback_t callback,
                                void *state);
};

/* The flag definitions for the font.flags field. */
#define MF_FONT_FLAG_MONOSPACE 0x01
#define MF_FONT_FLAG_BW        0x02

/* Lookup structure for searching fonts by name. */
struct mf_font_list_s
{
    const struct mf_font_list_s *next;
    const struct mf_font_s *font;
};


/* Function to decode and render a single character. 
 * 
 * font:      Pointer to the font definition.
 * x0, y0:    Upper left corner of the target area.
 * character: The character code (unicode) to render.
 * callback:  Callback function to write out the pixels.
 * state:     Free variable for caller to use (can be NULL).
 * 
 * Returns width of the character.
 */
MF_EXTERN uint8_t mf_render_character(const struct mf_font_s *font,
                                      int16_t x0, int16_t y0,
                                      mf_char character,
                                      mf_pixel_callback_t callback,
                                      void *state);

/* Function to get the width of a single character.
 * This is not necessarily the bounding box of the character
 * data, but rather the tracking width.
 *
 * font:      Pointer to the font definition.
 * character: The character code (unicode) to render.
 * 
 * Returns width of the character in pixels.
 */
MF_EXTERN uint8_t mf_character_width(const struct mf_font_s *font,
                                     mf_char character);

/* Find a font based on name. The name can be either short name or full name.
 * Note: You can pass MF_INCLUDED_FONTS to search among all the included .h
 * files.
 *
 * name: Font name to search for.
 * fonts: Pointer to the first font search entry.
 *
 * Returns a pointer to the font or NULL if not found.
 */
MF_EXTERN const struct mf_font_s *mf_find_font(const char *name);

/* Get the list of included fonts */
MF_EXTERN const struct mf_font_list_s *mf_get_font_list(void);

#endif
