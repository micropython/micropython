/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#include "mf_encoding.h"

#ifndef MF_NO_COMPILE

#if MF_ENCODING == MF_ENCODING_UTF8

mf_char mf_getchar(mf_str *str)
{
    uint8_t c;
    uint8_t tmp, seqlen;
    uint16_t result;
    
    c = **str;
    if (!c)
        return 0;
    
    (*str)++;
    
    if ((c & 0x80) == 0)
    {
        /* Just normal ASCII character. */
        return c;
    }
    else if ((c & 0xC0) == 0x80)
    {
        /* Dangling piece of corrupted multibyte sequence.
         * Did you cut the string in the wrong place?
         */
        return c;
    }
    else if ((**str & 0xC0) == 0xC0)
    {
        /* Start of multibyte sequence without any following bytes.
         * Silly. Maybe you are using the wrong encoding.
         */
        return c;
    }
    else
    {
        /* Beginning of a multi-byte sequence.
         * Find out how many characters and combine them.
         */
        seqlen = 2;
        tmp = 0x20;
        result = 0;
        while ((c & tmp) && (seqlen < 5))
        {
            seqlen++;
            tmp >>= 1;
            
            result = (result << 6) | (**str & 0x3F);
            (*str)++;
        }
        
        result = (result << 6) | (**str & 0x3F);
        (*str)++;
        
        result |= (c & (tmp - 1)) << ((seqlen - 1) * 6);
        return result;
    }
}

void mf_rewind(mf_str *str)
{
    (*str)--;
    
    while ((**str & 0x80) != 0x00 && (**str & 0xC0) != 0xC0)
        (*str)--;
}

#else

mf_char mf_getchar(mf_str *str)
{
    if (!(**str))
        return 0;
    else
        return *(*str)++;
}

void mf_rewind(mf_str *str)
{
	(*str)--;
}

#endif

#endif //MF_NO_COMPILE

