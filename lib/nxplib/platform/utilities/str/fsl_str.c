/*
 * Copyright 2017 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include <math.h>
#include <stdarg.h>
#include <stdlib.h>
#include "fsl_str.h"
#include "fsl_debug_console_conf.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief The overflow value.*/
#ifndef HUGE_VAL
#define HUGE_VAL (99.e99)
#endif /* HUGE_VAL */

#if PRINTF_ADVANCED_ENABLE
/*! @brief Specification modifier flags for printf. */
enum _debugconsole_printf_flag
{
    kPRINTF_Minus             = 0x01U,  /*!< Minus FLag. */
    kPRINTF_Plus              = 0x02U,  /*!< Plus Flag. */
    kPRINTF_Space             = 0x04U,  /*!< Space Flag. */
    kPRINTF_Zero              = 0x08U,  /*!< Zero Flag. */
    kPRINTF_Pound             = 0x10U,  /*!< Pound Flag. */
    kPRINTF_LengthChar        = 0x20U,  /*!< Length: Char Flag. */
    kPRINTF_LengthShortInt    = 0x40U,  /*!< Length: Short Int Flag. */
    kPRINTF_LengthLongInt     = 0x80U,  /*!< Length: Long Int Flag. */
    kPRINTF_LengthLongLongInt = 0x100U, /*!< Length: Long Long Int Flag. */
};
#endif /* PRINTF_ADVANCED_ENABLE */

/*! @brief Specification modifier flags for scanf. */
enum _debugconsole_scanf_flag
{
    kSCANF_Suppress   = 0x2U,    /*!< Suppress Flag. */
    kSCANF_DestMask   = 0x7cU,   /*!< Destination Mask. */
    kSCANF_DestChar   = 0x4U,    /*!< Destination Char Flag. */
    kSCANF_DestString = 0x8U,    /*!< Destination String FLag. */
    kSCANF_DestSet    = 0x10U,   /*!< Destination Set Flag. */
    kSCANF_DestInt    = 0x20U,   /*!< Destination Int Flag. */
    kSCANF_DestFloat  = 0x30U,   /*!< Destination Float Flag. */
    kSCANF_LengthMask = 0x1f00U, /*!< Length Mask Flag. */
#if SCANF_ADVANCED_ENABLE
    kSCANF_LengthChar        = 0x100U, /*!< Length Char Flag. */
    kSCANF_LengthShortInt    = 0x200U, /*!< Length ShortInt Flag. */
    kSCANF_LengthLongInt     = 0x400U, /*!< Length LongInt Flag. */
    kSCANF_LengthLongLongInt = 0x800U, /*!< Length LongLongInt Flag. */
#endif                                 /* SCANF_ADVANCED_ENABLE */
#if SCANF_FLOAT_ENABLE
    kSCANF_LengthLongLongDouble = 0x1000U, /*!< Length LongLongDuoble Flag. */
#endif                                     /*PRINTF_FLOAT_ENABLE */
    kSCANF_TypeSinged = 0x2000U,           /*!< TypeSinged Flag. */
};

/*! @brief Keil: suppress ellipsis warning in va_arg usage below. */
#if defined(__CC_ARM)
#pragma diag_suppress 1256
#endif /* __CC_ARM */

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief Scanline function which ignores white spaces.
 *
 * @param[in]   s The address of the string pointer to update.
 * @return      String without white spaces.
 */
static uint32_t ScanIgnoreWhiteSpace(const char **s);

/*!
 * @brief Converts a radix number to a string and return its length.
 *
 * @param[in] numstr    Converted string of the number.
 * @param[in] nump      Pointer to the number.
 * @param[in] neg       Polarity of the number.
 * @param[in] radix     The radix to be converted to.
 * @param[in] use_caps  Used to identify %x/X output format.

 * @return Length of the converted string.
 */
static int32_t ConvertRadixNumToString(char *numstr, void *nump, int32_t neg, int32_t radix, bool use_caps);

#if PRINTF_FLOAT_ENABLE
/*!
 * @brief Converts a floating radix number to a string and return its length.
 *
 * @param[in] numstr            Converted string of the number.
 * @param[in] nump              Pointer to the number.
 * @param[in] radix             The radix to be converted to.
 * @param[in] precision_width   Specify the precision width.

 * @return Length of the converted string.
 */
static int32_t ConvertFloatRadixNumToString(char *numstr, void *nump, int32_t radix, uint32_t precision_width);
#endif /* PRINTF_FLOAT_ENABLE */

/*!
 *
 */
double modf(double input_dbl, double *intpart_ptr);

/*************Code for process formatted data*******************************/

static uint32_t ScanIgnoreWhiteSpace(const char **s)
{
    uint8_t count = 0;
    uint8_t c;

    c = **s;
    while ((c == ' ') || (c == '\t') || (c == '\n') || (c == '\r') || (c == '\v') || (c == '\f'))
    {
        count++;
        (*s)++;
        c = **s;
    }
    return count;
}

static int32_t ConvertRadixNumToString(char *numstr, void *nump, int32_t neg, int32_t radix, bool use_caps)
{
#if PRINTF_ADVANCED_ENABLE
    int64_t a;
    int64_t b;
    int64_t c;

    uint64_t ua;
    uint64_t ub;
    uint64_t uc;
#else
    int32_t a;
    int32_t b;
    int32_t c;

    uint32_t ua;
    uint32_t ub;
    uint32_t uc;
#endif /* PRINTF_ADVANCED_ENABLE */

    int32_t nlen;
    char *nstrp;

    nlen     = 0;
    nstrp    = numstr;
    *nstrp++ = '\0';

    if (neg)
    {
#if PRINTF_ADVANCED_ENABLE
        a = *(int64_t *)nump;
#else
        a = *(int32_t *)nump;
#endif /* PRINTF_ADVANCED_ENABLE */
        if (a == 0)
        {
            *nstrp = '0';
            ++nlen;
            return nlen;
        }
        while (a != 0)
        {
#if PRINTF_ADVANCED_ENABLE
            b = (int64_t)a / (int64_t)radix;
            c = (int64_t)a - ((int64_t)b * (int64_t)radix);
            if (c < 0)
            {
                uc = (uint64_t)c;
                c  = (int64_t)(~uc) + 1 + '0';
            }
#else
            b = a / radix;
            c = a - (b * radix);
            if (c < 0)
            {
                uc = (uint32_t)c;
                c  = (uint32_t)(~uc) + 1 + '0';
            }
#endif /* PRINTF_ADVANCED_ENABLE */
            else
            {
                c = c + '0';
            }
            a        = b;
            *nstrp++ = (char)c;
            ++nlen;
        }
    }
    else
    {
#if PRINTF_ADVANCED_ENABLE
        ua = *(uint64_t *)nump;
#else
        ua = *(uint32_t *)nump;
#endif /* PRINTF_ADVANCED_ENABLE */
        if (ua == 0)
        {
            *nstrp = '0';
            ++nlen;
            return nlen;
        }
        while (ua != 0)
        {
#if PRINTF_ADVANCED_ENABLE
            ub = (uint64_t)ua / (uint64_t)radix;
            uc = (uint64_t)ua - ((uint64_t)ub * (uint64_t)radix);
#else
            ub = ua / (uint32_t)radix;
            uc = ua - (ub * (uint32_t)radix);
#endif /* PRINTF_ADVANCED_ENABLE */

            if (uc < 10)
            {
                uc = uc + '0';
            }
            else
            {
                uc = uc - 10 + (use_caps ? 'A' : 'a');
            }
            ua       = ub;
            *nstrp++ = (char)uc;
            ++nlen;
        }
    }
    return nlen;
}

#if PRINTF_FLOAT_ENABLE
static int32_t ConvertFloatRadixNumToString(char *numstr, void *nump, int32_t radix, uint32_t precision_width)
{
    int32_t a;
    int32_t b;
    int32_t c;
    int32_t i;
    uint32_t uc;
    double fa;
    double dc;
    double fb;
    double r;
    double fractpart;
    double intpart;

    int32_t nlen;
    char *nstrp;
    nlen     = 0;
    nstrp    = numstr;
    *nstrp++ = '\0';
    r        = *(double *)nump;
    if (!r)
    {
        *nstrp = '0';
        ++nlen;
        return nlen;
    }
    fractpart = modf((double)r, (double *)&intpart);
    /* Process fractional part. */
    for (i = 0; i < precision_width; i++)
    {
        fractpart *= radix;
    }
    if (r >= 0)
    {
        fa = fractpart + (double)0.5;
        if (fa >= pow(10, precision_width))
        {
            intpart++;
        }
    }
    else
    {
        fa = fractpart - (double)0.5;
        if (fa <= -pow(10, precision_width))
        {
            intpart--;
        }
    }
    for (i = 0; i < precision_width; i++)
    {
        fb = fa / (int32_t)radix;
        dc = (fa - (int64_t)fb * (int32_t)radix);
        c  = (int32_t)dc;
        if (c < 0)
        {
            uc = (uint32_t)c;
            c  = (int32_t)(~uc) + 1 + '0';
        }
        else
        {
            c = c + '0';
        }
        fa       = fb;
        *nstrp++ = (char)c;
        ++nlen;
    }
    *nstrp++ = (char)'.';
    ++nlen;
    a = (int32_t)intpart;
    if (a == 0)
    {
        *nstrp++ = '0';
        ++nlen;
    }
    else
    {
        while (a != 0)
        {
            b = (int32_t)a / (int32_t)radix;
            c = (int32_t)a - ((int32_t)b * (int32_t)radix);
            if (c < 0)
            {
                uc = (uint32_t)c;
                c  = (int32_t)(~uc) + 1 + '0';
            }
            else
            {
                c = c + '0';
            }
            a        = b;
            *nstrp++ = (char)c;
            ++nlen;
        }
    }
    return nlen;
}
#endif /* PRINTF_FLOAT_ENABLE */

/*!
 * brief This function outputs its parameters according to a formatted string.
 *
 * note I/O is performed by calling given function pointer using following
 * (*func_ptr)(c);
 *
 * param[in] fmt_ptr   Format string for printf.
 * param[in] args_ptr  Arguments to printf.
 * param[in] buf  pointer to the buffer
 * param cb print callback function pointer
 *
 * return Number of characters to be print
 */
int StrFormatPrintf(const char *fmt, va_list ap, char *buf, printfCb cb)
{
    /* va_list ap; */
    char *p;
    int32_t c;

    char vstr[33];
    char *vstrp  = NULL;
    int32_t vlen = 0;

    int32_t done;
    int32_t count = 0;

    uint32_t field_width;
    uint32_t precision_width;
    char *sval;
    int32_t cval;
    bool use_caps;
    uint8_t radix = 0;

#if PRINTF_ADVANCED_ENABLE
    uint32_t flags_used;
    int32_t schar, dschar;
    int64_t ival;
    uint64_t uval = 0;
    bool valid_precision_width;
#else
    int32_t ival;
    uint32_t uval = 0;
#endif /* PRINTF_ADVANCED_ENABLE */

#if PRINTF_FLOAT_ENABLE
    double fval;
#endif /* PRINTF_FLOAT_ENABLE */

    /* Start parsing apart the format string and display appropriate formats and data. */
    for (p = (char *)fmt; (c = *p) != 0; p++)
    {
        /*
         * All formats begin with a '%' marker.  Special chars like
         * '\n' or '\t' are normally converted to the appropriate
         * character by the __compiler__.  Thus, no need for this
         * routine to account for the '\' character.
         */
        if (c != '%')
        {
            cb(buf, &count, c, 1);
            /* By using 'continue', the next iteration of the loop is used, skipping the code that follows. */
            continue;
        }

        use_caps = true;

#if PRINTF_ADVANCED_ENABLE
        /* First check for specification modifier flags. */
        flags_used = 0;
        done       = false;
        while (!done)
        {
            switch (*++p)
            {
                case '-':
                    flags_used |= kPRINTF_Minus;
                    break;
                case '+':
                    flags_used |= kPRINTF_Plus;
                    break;
                case ' ':
                    flags_used |= kPRINTF_Space;
                    break;
                case '0':
                    flags_used |= kPRINTF_Zero;
                    break;
                case '#':
                    flags_used |= kPRINTF_Pound;
                    break;
                default:
                    /* We've gone one char too far. */
                    --p;
                    done = true;
                    break;
            }
        }
#endif /* PRINTF_ADVANCED_ENABLE */

        /* Next check for minimum field width. */
        field_width = 0;
        done        = false;
        while (!done)
        {
            c = *++p;
            if ((c >= '0') && (c <= '9'))
            {
                field_width = (field_width * 10) + (c - '0');
            }
#if PRINTF_ADVANCED_ENABLE
            else if (c == '*')
            {
                field_width = (uint32_t)va_arg(ap, uint32_t);
            }
#endif /* PRINTF_ADVANCED_ENABLE */
            else
            {
                /* We've gone one char too far. */
                --p;
                done = true;
            }
        }
        /* Next check for the width and precision field separator. */
        precision_width = 6;
#if PRINTF_ADVANCED_ENABLE
        valid_precision_width = false;
#endif /* PRINTF_ADVANCED_ENABLE */
        if (*++p == '.')
        {
            /* Must get precision field width, if present. */
            precision_width = 0;
            done            = false;
            while (!done)
            {
                c = *++p;
                if ((c >= '0') && (c <= '9'))
                {
                    precision_width = (precision_width * 10) + (c - '0');
#if PRINTF_ADVANCED_ENABLE
                    valid_precision_width = true;
#endif /* PRINTF_ADVANCED_ENABLE */
                }
#if PRINTF_ADVANCED_ENABLE
                else if (c == '*')
                {
                    precision_width       = (uint32_t)va_arg(ap, uint32_t);
                    valid_precision_width = true;
                }
#endif /* PRINTF_ADVANCED_ENABLE */
                else
                {
                    /* We've gone one char too far. */
                    --p;
                    done = true;
                }
            }
        }
        else
        {
            /* We've gone one char too far. */
            --p;
        }
#if PRINTF_ADVANCED_ENABLE
        /*
         * Check for the length modifier.
         */
        switch (/* c = */ *++p)
        {
            case 'h':
                if (*++p != 'h')
                {
                    flags_used |= kPRINTF_LengthShortInt;
                    --p;
                }
                else
                {
                    flags_used |= kPRINTF_LengthChar;
                }
                break;
            case 'l':
                if (*++p != 'l')
                {
                    flags_used |= kPRINTF_LengthLongInt;
                    --p;
                }
                else
                {
                    flags_used |= kPRINTF_LengthLongLongInt;
                }
                break;
            default:
                /* we've gone one char too far */
                --p;
                break;
        }
#endif /* PRINTF_ADVANCED_ENABLE */
        /* Now we're ready to examine the format. */
        c = *++p;
        {
            if ((c == 'd') || (c == 'i') || (c == 'f') || (c == 'F') || (c == 'x') || (c == 'X') || (c == 'o') ||
                (c == 'b') || (c == 'p') || (c == 'u'))
            {
                if ((c == 'd') || (c == 'i'))
                {
#if PRINTF_ADVANCED_ENABLE
                    if (flags_used & kPRINTF_LengthLongLongInt)
                    {
                        ival = (int64_t)va_arg(ap, int64_t);
                    }
                    else
#endif /* PRINTF_ADVANCED_ENABLE */
                    {
                        ival = (int32_t)va_arg(ap, int32_t);
                    }
                    vlen  = ConvertRadixNumToString(vstr, &ival, true, 10, use_caps);
                    vstrp = &vstr[vlen];
#if PRINTF_ADVANCED_ENABLE
                    if (ival < 0)
                    {
                        schar = '-';
                        ++vlen;
                    }
                    else
                    {
                        if (flags_used & kPRINTF_Plus)
                        {
                            schar = '+';
                            ++vlen;
                        }
                        else
                        {
                            if (flags_used & kPRINTF_Space)
                            {
                                schar = ' ';
                                ++vlen;
                            }
                            else
                            {
                                schar = 0;
                            }
                        }
                    }
                    dschar = false;
                    /* Do the ZERO pad. */
                    if (flags_used & kPRINTF_Zero)
                    {
                        if (schar)
                        {
                            cb(buf, &count, schar, 1);
                        }
                        dschar = true;

                        cb(buf, &count, '0', field_width - vlen);
                        vlen = field_width;
                    }
                    else
                    {
                        if (!(flags_used & kPRINTF_Minus))
                        {
                            cb(buf, &count, ' ', field_width - vlen);
                            if (schar)
                            {
                                cb(buf, &count, schar, 1);
                            }
                            dschar = true;
                        }
                    }
                    /* The string was built in reverse order, now display in correct order. */
                    if ((!dschar) && schar)
                    {
                        cb(buf, &count, schar, 1);
                    }
#endif /* PRINTF_ADVANCED_ENABLE */
                }

#if PRINTF_FLOAT_ENABLE
                if ((c == 'f') || (c == 'F'))
                {
                    fval  = (double)va_arg(ap, double);
                    vlen  = ConvertFloatRadixNumToString(vstr, &fval, 10, precision_width);
                    vstrp = &vstr[vlen];

#if PRINTF_ADVANCED_ENABLE
                    if (fval < 0)
                    {
                        schar = '-';
                        ++vlen;
                    }
                    else
                    {
                        if (flags_used & kPRINTF_Plus)
                        {
                            schar = '+';
                            ++vlen;
                        }
                        else
                        {
                            if (flags_used & kPRINTF_Space)
                            {
                                schar = ' ';
                                ++vlen;
                            }
                            else
                            {
                                schar = 0;
                            }
                        }
                    }
                    dschar = false;
                    if (flags_used & kPRINTF_Zero)
                    {
                        if (schar)
                        {
                            cb(buf, &count, schar, 1);
                        }
                        dschar = true;
                        cb(buf, &count, '0', field_width - vlen);
                        vlen = field_width;
                    }
                    else
                    {
                        if (!(flags_used & kPRINTF_Minus))
                        {
                            cb(buf, &count, ' ', field_width - vlen);
                            if (schar)
                            {
                                cb(buf, &count, schar, 1);
                            }
                            dschar = true;
                        }
                    }
                    if ((!dschar) && schar)
                    {
                        cb(buf, &count, schar, 1);
                    }
#endif /* PRINTF_ADVANCED_ENABLE */
                }
#endif /* PRINTF_FLOAT_ENABLE */
                if ((c == 'X') || (c == 'x'))
                {
                    if (c == 'x')
                    {
                        use_caps = false;
                    }
#if PRINTF_ADVANCED_ENABLE
                    if (flags_used & kPRINTF_LengthLongLongInt)
                    {
                        uval = (uint64_t)va_arg(ap, uint64_t);
                    }
                    else
#endif /* PRINTF_ADVANCED_ENABLE */
                    {
                        uval = (uint32_t)va_arg(ap, uint32_t);
                    }
                    vlen  = ConvertRadixNumToString(vstr, &uval, false, 16, use_caps);
                    vstrp = &vstr[vlen];

#if PRINTF_ADVANCED_ENABLE
                    dschar = false;
                    if (flags_used & kPRINTF_Zero)
                    {
                        if (flags_used & kPRINTF_Pound)
                        {
                            cb(buf, &count, '0', 1);
                            cb(buf, &count, (use_caps ? 'X' : 'x'), 1);
                            dschar = true;
                        }
                        cb(buf, &count, '0', field_width - vlen);
                        vlen = field_width;
                    }
                    else
                    {
                        if (!(flags_used & kPRINTF_Minus))
                        {
                            if (flags_used & kPRINTF_Pound)
                            {
                                vlen += 2;
                            }
                            cb(buf, &count, ' ', field_width - vlen);
                            if (flags_used & kPRINTF_Pound)
                            {
                                cb(buf, &count, '0', 1);
                                cb(buf, &count, (use_caps ? 'X' : 'x'), 1);
                                dschar = true;
                            }
                        }
                    }

                    if ((flags_used & kPRINTF_Pound) && (!dschar))
                    {
                        cb(buf, &count, '0', 1);
                        cb(buf, &count, (use_caps ? 'X' : 'x'), 1);
                        vlen += 2;
                    }
#endif /* PRINTF_ADVANCED_ENABLE */
                }
                if ((c == 'o') || (c == 'b') || (c == 'p') || (c == 'u'))
                {
#if PRINTF_ADVANCED_ENABLE
                    if (flags_used & kPRINTF_LengthLongLongInt)
                    {
                        uval = (uint64_t)va_arg(ap, uint64_t);
                    }
                    else
#endif /* PRINTF_ADVANCED_ENABLE */
                    {
                        uval = (uint32_t)va_arg(ap, uint32_t);
                    }

                    if (c == 'o')
                    {
                        radix = 8;
                    }
                    else if (c == 'b')
                    {
                        radix = 2;
                    }
                    else if (c == 'p')
                    {
                        radix = 16;
                    }
                    else
                    {
                        radix = 10;
                    }

                    vlen  = ConvertRadixNumToString(vstr, &uval, false, radix, use_caps);
                    vstrp = &vstr[vlen];
#if PRINTF_ADVANCED_ENABLE
                    if (flags_used & kPRINTF_Zero)
                    {
                        cb(buf, &count, '0', field_width - vlen);
                        vlen = field_width;
                    }
                    else
                    {
                        if (!(flags_used & kPRINTF_Minus))
                        {
                            cb(buf, &count, ' ', field_width - vlen);
                        }
                    }
#endif /* PRINTF_ADVANCED_ENABLE */
                }
#if !PRINTF_ADVANCED_ENABLE
                cb(buf, &count, ' ', field_width - vlen);
#endif /* !PRINTF_ADVANCED_ENABLE */
                if (vstrp != NULL)
                {
                    while (*vstrp)
                    {
                        cb(buf, &count, *vstrp--, 1);
                    }
                }
#if PRINTF_ADVANCED_ENABLE
                if (flags_used & kPRINTF_Minus)
                {
                    cb(buf, &count, ' ', field_width - vlen);
                }
#endif /* PRINTF_ADVANCED_ENABLE */
            }
            else if (c == 'c')
            {
                cval = (char)va_arg(ap, uint32_t);
                cb(buf, &count, cval, 1);
            }
            else if (c == 's')
            {
                sval = (char *)va_arg(ap, char *);
                if (sval)
                {
#if PRINTF_ADVANCED_ENABLE
                    if (valid_precision_width)
                    {
                        vlen = precision_width;
                    }
                    else
                    {
                        vlen = strlen(sval);
                    }
#else
                    vlen = strlen(sval);
#endif /* PRINTF_ADVANCED_ENABLE */
#if PRINTF_ADVANCED_ENABLE
                    if (!(flags_used & kPRINTF_Minus))
#endif /* PRINTF_ADVANCED_ENABLE */
                    {
                        cb(buf, &count, ' ', field_width - vlen);
                    }

#if PRINTF_ADVANCED_ENABLE
                    if (valid_precision_width)
                    {
                        while ((*sval) && (vlen > 0))
                        {
                            cb(buf, &count, *sval++, 1);
                            vlen--;
                        }
                        /* In case that vlen sval is shorter than vlen */
                        vlen = precision_width - vlen;
                    }
                    else
                    {
#endif /* PRINTF_ADVANCED_ENABLE */
                        while (*sval)
                        {
                            cb(buf, &count, *sval++, 1);
                        }
#if PRINTF_ADVANCED_ENABLE
                    }
#endif /* PRINTF_ADVANCED_ENABLE */

#if PRINTF_ADVANCED_ENABLE
                    if (flags_used & kPRINTF_Minus)
                    {
                        cb(buf, &count, ' ', field_width - vlen);
                    }
#endif /* PRINTF_ADVANCED_ENABLE */
                }
            }
            else
            {
                cb(buf, &count, c, 1);
            }
        }
    }

    return count;
}

/*!
 * brief Converts an input line of ASCII characters based upon a provided
 * string format.
 *
 * param[in] line_ptr The input line of ASCII data.
 * param[in] format   Format first points to the format string.
 * param[in] args_ptr The list of parameters.
 *
 * return Number of input items converted and assigned.
 * retval IO_EOF When line_ptr is empty string "".
 */
int StrFormatScanf(const char *line_ptr, char *format, va_list args_ptr)
{
    uint8_t base;
    int8_t neg;
    /* Identifier for the format string. */
    char *c = format;
    char temp;
    char *buf;
    /* Flag telling the conversion specification. */
    uint32_t flag = 0;
    /* Filed width for the matching input streams. */
    uint32_t field_width;
    /* How many arguments are assigned except the suppress. */
    uint32_t nassigned = 0;
    /* How many characters are read from the input streams. */
    uint32_t n_decode = 0;

    int32_t val;

    const char *s;
    /* Identifier for the input string. */
    const char *p = line_ptr;

#if SCANF_FLOAT_ENABLE
    double fnum = 0.0;
#endif /* SCANF_FLOAT_ENABLE */
    /* Return EOF error before any conversion. */
    if (*p == '\0')
    {
        return -1;
    }

    /* Decode directives. */
    while ((*c) && (*p))
    {
        /* Ignore all white-spaces in the format strings. */
        if (ScanIgnoreWhiteSpace((const char **)&c))
        {
            n_decode += ScanIgnoreWhiteSpace(&p);
        }
        else if ((*c != '%') || ((*c == '%') && (*(c + 1) == '%')))
        {
            /* Ordinary characters. */
            c++;
            if (*p == *c)
            {
                n_decode++;
                p++;
                c++;
            }
            else
            {
                /* Match failure. Misalignment with C99, the unmatched characters need to be pushed back to stream.
                 * However, it is deserted now. */
                break;
            }
        }
        else
        {
            /* convernsion specification */
            c++;
            /* Reset. */
            flag        = 0;
            field_width = 0;
            base        = 0;

            /* Loop to get full conversion specification. */
            while ((*c) && (!(flag & kSCANF_DestMask)))
            {
                switch (*c)
                {
#if SCANF_ADVANCED_ENABLE
                    case '*':
                        if (flag & kSCANF_Suppress)
                        {
                            /* Match failure. */
                            return nassigned;
                        }
                        flag |= kSCANF_Suppress;
                        c++;
                        break;
                    case 'h':
                        if (flag & kSCANF_LengthMask)
                        {
                            /* Match failure. */
                            return nassigned;
                        }

                        if (c[1] == 'h')
                        {
                            flag |= kSCANF_LengthChar;
                            c++;
                        }
                        else
                        {
                            flag |= kSCANF_LengthShortInt;
                        }
                        c++;
                        break;
                    case 'l':
                        if (flag & kSCANF_LengthMask)
                        {
                            /* Match failure. */
                            return nassigned;
                        }

                        if (c[1] == 'l')
                        {
                            flag |= kSCANF_LengthLongLongInt;
                            c++;
                        }
                        else
                        {
                            flag |= kSCANF_LengthLongInt;
                        }
                        c++;
                        break;
#endif /* SCANF_ADVANCED_ENABLE */
#if SCANF_FLOAT_ENABLE
                    case 'L':
                        if (flag & kSCANF_LengthMask)
                        {
                            /* Match failure. */
                            return nassigned;
                        }
                        flag |= kSCANF_LengthLongLongDouble;
                        c++;
                        break;
#endif /* SCANF_FLOAT_ENABLE */
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                        if (field_width)
                        {
                            /* Match failure. */
                            return nassigned;
                        }
                        do
                        {
                            field_width = field_width * 10 + *c - '0';
                            c++;
                        } while ((*c >= '0') && (*c <= '9'));
                        break;
                    case 'd':
                        base = 10;
                        flag |= kSCANF_TypeSinged;
                        flag |= kSCANF_DestInt;
                        c++;
                        break;
                    case 'u':
                        base = 10;
                        flag |= kSCANF_DestInt;
                        c++;
                        break;
                    case 'o':
                        base = 8;
                        flag |= kSCANF_DestInt;
                        c++;
                        break;
                    case 'x':
                    case 'X':
                        base = 16;
                        flag |= kSCANF_DestInt;
                        c++;
                        break;
                    case 'i':
                        base = 0;
                        flag |= kSCANF_DestInt;
                        c++;
                        break;
#if SCANF_FLOAT_ENABLE
                    case 'a':
                    case 'A':
                    case 'e':
                    case 'E':
                    case 'f':
                    case 'F':
                    case 'g':
                    case 'G':
                        flag |= kSCANF_DestFloat;
                        c++;
                        break;
#endif /* SCANF_FLOAT_ENABLE */
                    case 'c':
                        flag |= kSCANF_DestChar;
                        if (!field_width)
                        {
                            field_width = 1;
                        }
                        c++;
                        break;
                    case 's':
                        flag |= kSCANF_DestString;
                        c++;
                        break;
                    default:
                        return nassigned;
                }
            }

            if (!(flag & kSCANF_DestMask))
            {
                /* Format strings are exhausted. */
                return nassigned;
            }

            if (!field_width)
            {
                /* Large than length of a line. */
                field_width = 99;
            }

            /* Matching strings in input streams and assign to argument. */
            switch (flag & kSCANF_DestMask)
            {
                case kSCANF_DestChar:
                    s   = (const char *)p;
                    buf = va_arg(args_ptr, char *);
                    while ((field_width--) && (*p))
                    {
                        if (!(flag & kSCANF_Suppress))
                        {
                            *buf++ = *p++;
                        }
                        else
                        {
                            p++;
                        }
                        n_decode++;
                    }

                    if ((!(flag & kSCANF_Suppress)) && (s != p))
                    {
                        nassigned++;
                    }
                    break;
                case kSCANF_DestString:
                    n_decode += ScanIgnoreWhiteSpace(&p);
                    s   = p;
                    buf = va_arg(args_ptr, char *);
                    while ((field_width--) && (*p != '\0') && (*p != ' ') && (*p != '\t') && (*p != '\n') &&
                           (*p != '\r') && (*p != '\v') && (*p != '\f'))
                    {
                        if (flag & kSCANF_Suppress)
                        {
                            p++;
                        }
                        else
                        {
                            *buf++ = *p++;
                        }
                        n_decode++;
                    }

                    if ((!(flag & kSCANF_Suppress)) && (s != p))
                    {
                        /* Add NULL to end of string. */
                        *buf = '\0';
                        nassigned++;
                    }
                    break;
                case kSCANF_DestInt:
                    n_decode += ScanIgnoreWhiteSpace(&p);
                    s   = p;
                    val = 0;
                    if ((base == 0) || (base == 16))
                    {
                        if ((s[0] == '0') && ((s[1] == 'x') || (s[1] == 'X')))
                        {
                            base = 16;
                            if (field_width >= 1)
                            {
                                p += 2;
                                n_decode += 2;
                                field_width -= 2;
                            }
                        }
                    }

                    if (base == 0)
                    {
                        if (s[0] == '0')
                        {
                            base = 8;
                        }
                        else
                        {
                            base = 10;
                        }
                    }

                    neg = 1;
                    switch (*p)
                    {
                        case '-':
                            neg = -1;
                            n_decode++;
                            p++;
                            field_width--;
                            break;
                        case '+':
                            neg = 1;
                            n_decode++;
                            p++;
                            field_width--;
                            break;
                        default:
                            break;
                    }

                    while ((*p) && (field_width--))
                    {
                        if ((*p <= '9') && (*p >= '0'))
                        {
                            temp = *p - '0';
                        }
                        else if ((*p <= 'f') && (*p >= 'a'))
                        {
                            temp = *p - 'a' + 10;
                        }
                        else if ((*p <= 'F') && (*p >= 'A'))
                        {
                            temp = *p - 'A' + 10;
                        }
                        else
                        {
                            temp = base;
                        }

                        if (temp >= base)
                        {
                            break;
                        }
                        else
                        {
                            val = base * val + temp;
                        }
                        p++;
                        n_decode++;
                    }
                    val *= neg;
                    if (!(flag & kSCANF_Suppress))
                    {
#if SCANF_ADVANCED_ENABLE
                        switch (flag & kSCANF_LengthMask)
                        {
                            case kSCANF_LengthChar:
                                if (flag & kSCANF_TypeSinged)
                                {
                                    *va_arg(args_ptr, signed char *) = (signed char)val;
                                }
                                else
                                {
                                    *va_arg(args_ptr, unsigned char *) = (unsigned char)val;
                                }
                                break;
                            case kSCANF_LengthShortInt:
                                if (flag & kSCANF_TypeSinged)
                                {
                                    *va_arg(args_ptr, signed short *) = (signed short)val;
                                }
                                else
                                {
                                    *va_arg(args_ptr, unsigned short *) = (unsigned short)val;
                                }
                                break;
                            case kSCANF_LengthLongInt:
                                if (flag & kSCANF_TypeSinged)
                                {
                                    *va_arg(args_ptr, signed long int *) = (signed long int)val;
                                }
                                else
                                {
                                    *va_arg(args_ptr, unsigned long int *) = (unsigned long int)val;
                                }
                                break;
                            case kSCANF_LengthLongLongInt:
                                if (flag & kSCANF_TypeSinged)
                                {
                                    *va_arg(args_ptr, signed long long int *) = (signed long long int)val;
                                }
                                else
                                {
                                    *va_arg(args_ptr, unsigned long long int *) = (unsigned long long int)val;
                                }
                                break;
                            default:
                                /* The default type is the type int. */
                                if (flag & kSCANF_TypeSinged)
                                {
                                    *va_arg(args_ptr, signed int *) = (signed int)val;
                                }
                                else
                                {
                                    *va_arg(args_ptr, unsigned int *) = (unsigned int)val;
                                }
                                break;
                        }
#else
                        /* The default type is the type int. */
                        if (flag & kSCANF_TypeSinged)
                        {
                            *va_arg(args_ptr, signed int *) = (signed int)val;
                        }
                        else
                        {
                            *va_arg(args_ptr, unsigned int *) = (unsigned int)val;
                        }
#endif /* SCANF_ADVANCED_ENABLE */
                        nassigned++;
                    }
                    break;
#if SCANF_FLOAT_ENABLE
                case kSCANF_DestFloat:
                    n_decode += ScanIgnoreWhiteSpace(&p);
                    fnum = strtod(p, (char **)&s);

                    if ((fnum >= HUGE_VAL) || (fnum <= -HUGE_VAL))
                    {
                        break;
                    }

                    n_decode += (int)(s) - (int)(p);
                    p = s;
                    if (!(flag & kSCANF_Suppress))
                    {
                        if (flag & kSCANF_LengthLongLongDouble)
                        {
                            *va_arg(args_ptr, double *) = fnum;
                        }
                        else
                        {
                            *va_arg(args_ptr, float *) = (float)fnum;
                        }
                        nassigned++;
                    }
                    break;
#endif /* SCANF_FLOAT_ENABLE */
                default:
                    return nassigned;
            }
        }
    }
    return nassigned;
}
