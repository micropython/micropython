/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#include "mf_justify.h"
#include "mf_kerning.h"

#ifndef MF_NO_COMPILE

#if MF_USE_TABS
/* Round the X coordinate up to the nearest tab stop. */
static int16_t mf_round_to_tab(const struct mf_font_s *font,
                               int16_t x0, int16_t x)
{
    int16_t tabw, dx;
    
    tabw = mf_character_width(font, 'm') * MF_TABSIZE;
    
    /* Always atleast 1 space */
    x += mf_character_width(font, ' ');
    
    /* Round to next tab stop */
    dx = x - x0 + font->baseline_x;
    x += tabw - (dx % tabw);
    
    return x;
}

/* Round the X coordinate down to the nearest tab stop. */
static int16_t mf_round_to_prev_tab(const struct mf_font_s *font,
                                    int16_t x0, int16_t x)
{
    int16_t tabw, dx;
    
    tabw = mf_character_width(font, 'm') * MF_TABSIZE;
    
    /* Always atleast 1 space */
    x -= mf_character_width(font, ' ');
    
    /* Round to previous tab stop */
    dx = x0 - x + font->baseline_x;
    x -= tabw - (dx % tabw);
    
    return x;
}
#endif

int16_t mf_get_string_width(const struct mf_font_s *font, mf_str text,
                            uint16_t count, bool kern)
{
    int16_t result = 0;
    uint16_t c1 = 0, c2;
    
    if (!count)
        count = 0xFFFF;
    
    while (count-- && *text)
    {
        c2 = mf_getchar(&text);

        if (c2 == '\t')
        {
#if MF_USE_TABS
            result = mf_round_to_tab(font, 0, result);
            c1 = ' ';
            continue;
#else
            c2 = ' ';
#endif
        }
        
        if (kern && c1 != 0)
            result += mf_compute_kerning(font, c1, c2);

        result += mf_character_width(font, c2);
        c1 = c2;
    }
    
    return result;
}

/* Return the length of the string without trailing spaces. */
static uint16_t strip_spaces(mf_str text, uint16_t count, mf_char *last_char)
{
    uint16_t i = 0, result = 0;
    mf_char tmp = 0;
    
    if (!count)
        count = 0xFFFF;
    
    while (count-- && *text)
    {
        i++;
        tmp = mf_getchar(&text);
        if (tmp != ' ' && tmp != (mf_char)0xA0 && tmp != '\n' &&
            tmp != '\r' && tmp != '\t')
        {
            result = i;
        }
    }
    
    if (last_char)
    {
        if (!*text)
            *last_char = 0;
        else
            *last_char = tmp;
    }
    
    return result;
}

/* Render left-aligned string, left edge at x0. */
static void render_left(const struct mf_font_s *font,
                        int16_t x0, int16_t y0,
                        mf_str text, uint16_t count,
                        mf_character_callback_t callback,
                        void *state)
{
    int16_t x;
    mf_char c1 = 0, c2;
    
    x = x0 - font->baseline_x;
    while (count--)
    {
        c2 = mf_getchar(&text);
        
        if (c2 == '\t')
        {
#if MF_USE_TABS
            x = mf_round_to_tab(font, x0, x);
            c1 = ' ';
            continue;
#else
            c2 = ' ';
#endif
        }
        
        if (c1 != 0)
            x += mf_compute_kerning(font, c1, c2);

        x += callback(x, y0, c2, state);
        c1 = c2;
    }
}

#if !MF_USE_ALIGN

void mf_render_aligned(const struct mf_font_s *font,
                       int16_t x0, int16_t y0,
                       enum mf_align_t align,
                       mf_str text, uint16_t count,
                       mf_character_callback_t callback,
                       void *state)
{
    int16_t string_width;
    count = strip_spaces(text, count, 0);
    render_left(font, x0, y0, text, count, callback, state);
}

#else

/* Render right-aligned string, right edge at x0. */
static void render_right(const struct mf_font_s *font,
                         int16_t x0, int16_t y0,
                         mf_str text, uint16_t count,
                         mf_character_callback_t callback,
                         void *state)
{
    int16_t x;
    uint16_t i;
    mf_char c1, c2 = 0;
    mf_str tmp;
    
    /* Go to the end of the line. */
    for (i = 0; i < count; i++)
        mf_getchar(&text);
    
    x = x0 - font->baseline_x;
    for (i = 0; i < count; i++)
    {
        mf_rewind(&text);
        tmp = text;
        c1 = mf_getchar(&tmp);
        
        /* Perform tab alignment */
        if (c1 == '\t')
        {
#if MF_USE_TABS
            x = mf_round_to_prev_tab(font, x0, x);
            c2 = ' ';
            continue;
#else
            c1 = ' ';
#endif
        }
        
        /* Apply the nominal character width */
        x -= mf_character_width(font, c1);
        
        /* Apply kerning */
        if (c2 != 0)
            x -= mf_compute_kerning(font, c1, c2);
        
        callback(x, y0, c1, state);
        c2 = c1;
    }
}

void mf_render_aligned(const struct mf_font_s *font,
                       int16_t x0, int16_t y0,
                       enum mf_align_t align,
                       mf_str text, uint16_t count,
                       mf_character_callback_t callback,
                       void *state)
{
    int16_t string_width;
    count = strip_spaces(text, count, 0);
    
    if (align == MF_ALIGN_LEFT)
    {
        render_left(font, x0, y0, text, count, callback, state);
    }
    if (align == MF_ALIGN_CENTER)
    {
        string_width = mf_get_string_width(font, text, count, false);
        x0 -= string_width / 2;
        render_left(font, x0, y0, text, count, callback, state);
    }
    else if (align == MF_ALIGN_RIGHT)
    {
        render_right(font, x0, y0, text, count, callback, state);
    }
}

#endif


#if !MF_USE_JUSTIFY

void mf_render_justified(const struct mf_font_s *font,
                         int16_t x0, int16_t y0, int16_t width,
                         mf_str text, uint16_t count,
                         mf_character_callback_t callback,
                         void *state)
{
	(void) width;
    mf_render_aligned(font, x0, y0, MF_ALIGN_LEFT, text, count, callback, state);
}

#else

/* Returns true if the character is a justification point, i.e. expands
 * when the text is being justified. */
static bool is_justify_space(uint16_t c)
{
    return c == ' ' || c == 0xA0;
}

/* Count the number of space characters in string */
static uint16_t count_spaces(mf_str text, uint16_t count)
{
    uint16_t spaces = 0;
    while (count-- && *text)
    {
        if (is_justify_space(mf_getchar(&text)))
            spaces++;
    }
    return spaces;
}

void mf_render_justified(const struct mf_font_s *font,
                         int16_t x0, int16_t y0, int16_t width,
                         mf_str text, uint16_t count,
                         mf_character_callback_t callback,
                         void *state)
{
    int16_t string_width, adjustment;
    uint16_t num_spaces;
    mf_char last_char;
    
    count = strip_spaces(text, count, &last_char);
    
    if (last_char == '\n' || last_char == 0)
    {
        /* Line ends in linefeed, do not justify. */
        render_left(font, x0, y0, text, count, callback, state);
        return;
    }
    
    string_width = mf_get_string_width(font, text, count, false);
    adjustment = width - string_width;
    num_spaces = count_spaces(text, count);
    
    {
        int16_t x, tmp;
        mf_char c1 = 0, c2;
        
        x = x0 - font->baseline_x;
        while (count--)
        {
            c2 = mf_getchar(&text);
            
            if (c2 == '\t')
            {
#if MF_USE_TABS
                tmp = x;
                x = mf_round_to_tab(font, x0, x);
                adjustment -= x - tmp - mf_character_width(font, '\t');
                c1 = c2;
                continue;
#else
                c2 = ' ';
#endif
            }
            
            if (is_justify_space(c2))
            {
                tmp = (adjustment + num_spaces / 2) / num_spaces;
                adjustment -= tmp;
                num_spaces--;
                x += tmp;
            }
            
            if (c1 != 0)
            {
                tmp = mf_compute_kerning(font, c1, c2);
                x += tmp;
                adjustment -= tmp;
            }

            x += callback(x, y0, c2, state);
            c1 = c2;
        }
    }
}

#endif

#endif //MF_NO_COMPILE


