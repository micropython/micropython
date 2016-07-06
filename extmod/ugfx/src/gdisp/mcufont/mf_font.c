/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#include "mf_config.h"

#ifndef MF_NO_COMPILE

#define MF_BWFONT_INTERNALS
#define MF_RLEFONT_INTERNALS
#define MF_SCALEDFONT_INTERNALS
#include "mf_font.h"
#include "mf_rlefont.h"
#include "mf_bwfont.h"
#include "mf_scaledfont.h"

#include <stdbool.h>

/* This will be made into a list of included fonts using macro magic. */
#define MF_INCLUDED_FONTS 0

/* Included fonts begin here */
#include MF_FONT_FILE_NAME
/* Include fonts end here */

uint8_t mf_render_character(const struct mf_font_s *font,
                            int16_t x0, int16_t y0,
                            mf_char character,
                            mf_pixel_callback_t callback,
                            void *state)
{
    uint8_t width;
    width = font->render_character(font, x0, y0, character, callback, state);
    
    if (!width)
    {
        width = font->render_character(font, x0, y0, font->fallback_character,
                                       callback, state);
    }
    
    return width;
}

uint8_t mf_character_width(const struct mf_font_s *font,
                           mf_char character)
{
    uint8_t width;
    width = font->character_width(font, MFCHAR2UINT16(character));
    
    if (!width)
    {
        width = font->character_width(font, font->fallback_character);
    }
    
    return width;
}

/* Avoids a dependency on libc */
static bool strequals(const char *a, const char *b)
{
    while (*a)
    {
        if (*a++ != *b++)
            return false;
    }
    return (!*b);
}

const struct mf_font_s *mf_find_font(const char *name)
{
    const struct mf_font_list_s *f;
    f = MF_INCLUDED_FONTS;
    
    while (f)
    {
        if (strequals(f->font->full_name, name) ||
            strequals(f->font->short_name, name))
        {
            return f->font;
        }
        
        f = f->next;
    }
    
    return 0;
}

const struct mf_font_list_s *mf_get_font_list()
{
    return MF_INCLUDED_FONTS;
}

#endif //MF_NO_COMPILE

