#include <stdint.h>
#include <string.h>
#include <stdarg.h>

#include "std.h"
#include "misc.h"
#include "systick.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "pfenv.h"
#if 0
#include "lcd.h"
#endif
#include "usart.h"
#include "usb.h"

#if MICROPY_ENABLE_FLOAT
#include "formatfloat.h"
#endif

void pfenv_prints(const pfenv_t *pfenv, const char *str) {
    pfenv->print_strn(pfenv->data, str, strlen(str));
}

int pfenv_printf(const pfenv_t *pfenv, const char *fmt, va_list args) {
    int chrs = 0;
    for (;;) {
        {
            const char *f = fmt;
            while (*f != '\0' && *f != '%') {
                ++f; // XXX UTF8 advance char
            }
            if (f > fmt) {
                pfenv->print_strn(pfenv->data, fmt, f - fmt);
                chrs += f - fmt;
                fmt = f;
            }
        }

        if (*fmt == '\0') {
            break;
        }

        // move past % character
        ++fmt;

        // parse flags, if they exist
        int flags = 0;
        char fill = ' ';
        while (*fmt != '\0') {
            if (*fmt == '-') flags |= PF_FLAG_LEFT_ADJUST;
            else if (*fmt == '+') flags |= PF_FLAG_SHOW_SIGN;
            else if (*fmt == ' ') flags |= PF_FLAG_SPACE_SIGN;
            else if (*fmt == '!') flags |= PF_FLAG_NO_TRAILZ;
            else if (*fmt == '0') {
                flags |= PF_FLAG_PAD_AFTER_SIGN;
                fill = '0';
            } else break;
            ++fmt;
        }

        // parse width, if it exists
        int width = 0;
        for (; '0' <= *fmt && *fmt <= '9'; ++fmt) {
            width = width * 10 + *fmt - '0';
        }

        // parse precision, if it exists
        int prec = -1;
        if (*fmt == '.') {
            ++fmt;
            if (*fmt == '*') {
                ++fmt;
                prec = va_arg(args, int);
            } else {
                prec = 0;
                for (; '0' <= *fmt && *fmt <= '9'; ++fmt) {
                    prec = prec * 10 + *fmt - '0';
                }
            }
            if (prec < 0) {
                prec = 0;
            }
        }

        // parse long specifiers (current not used)
        //bool long_arg = false;
        if (*fmt == 'l') {
            ++fmt;
            //long_arg = true;
        }

        if (*fmt == '\0') {
            break;
        }

        switch (*fmt) {
            case 'b':
                if (va_arg(args, int)) {
                    chrs += pfenv_print_strn(pfenv, "true", 4, flags, fill, width);
                } else {
                    chrs += pfenv_print_strn(pfenv, "false", 5, flags, fill, width);
                }
                break;
            case 'c':
            {
                char str = va_arg(args, int);
                chrs += pfenv_print_strn(pfenv, &str, 1, flags, fill, width);
                break;
            }
            case 's':
            {
                const char *str = va_arg(args, const char*);
                if (str) {
                    if (prec < 0) {
                        prec = strlen(str);
                    }
                    chrs += pfenv_print_strn(pfenv, str, prec, flags, fill, width);
                } else {
                    chrs += pfenv_print_strn(pfenv, "(null)", 6, flags, fill, width);
                }
                break;
            }
            case 'u':
                chrs += pfenv_print_int(pfenv, va_arg(args, int), 0, 10, 'a', flags, fill, width);
                break;
            case 'd':
                chrs += pfenv_print_int(pfenv, va_arg(args, int), 1, 10, 'a', flags, fill, width);
                break;
            case 'x':
            case 'p': // ?
                chrs += pfenv_print_int(pfenv, va_arg(args, int), 0, 16, 'a', flags, fill, width);
                break;
            case 'X':
            case 'P': // ?
                chrs += pfenv_print_int(pfenv, va_arg(args, int), 0, 16, 'A', flags, fill, width);
                break;
#if MICROPY_ENABLE_FLOAT
            case 'e':
            case 'E':
            case 'f':
            case 'F':
            case 'g':
            case 'G':
            {
#if MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_FLOAT
                mp_float_t f = va_arg(args, double);
                chrs += pfenv_print_float(pfenv, f, *fmt, flags, fill, width, prec);
#elif MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_DOUBLE
                // Currently pfenv_print_float uses snprintf, so if you want
                // to use pfenv_print_float with doubles then you'll need
                // fix it to not use snprintf first. Otherwise you'll have
                // inifinite recursion.
#error Calling pfenv_print_float with double not supported from within printf
#else
#error Unknown MICROPY FLOAT IMPL
#endif
                break;
            }
#endif
            default:
                pfenv->print_strn(pfenv->data, fmt, 1);
                chrs += 1;
                break;
        }
        ++fmt;
    }
    return chrs;
}

void stdout_print_strn(void *data, const char *str, unsigned int len) {
    // send stdout to USART, USB CDC VCP, and LCD if nothing else
    bool any = false;

    if (pyb_usart_global_debug != PYB_USART_NONE) {
        usart_tx_strn_cooked(pyb_usart_global_debug, str, len);
        any = true;
    }
    if (usb_vcp_is_enabled()) {
        usb_vcp_send_strn_cooked(str, len);
        any = true;
    }
    if (!any) {
#if 0
#if MICROPY_HW_HAS_LCD
        lcd_print_strn(str, len);
#endif
#endif
    }
}

static const pfenv_t pfenv_stdout = {0, stdout_print_strn};

int printf(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int ret = pfenv_printf(&pfenv_stdout, fmt, ap);
    va_end(ap);
    return ret;
}

int vprintf(const char *fmt, va_list ap) {
    return pfenv_printf(&pfenv_stdout, fmt, ap);
}

#if MICROPY_DEBUG_PRINTERS
int DEBUG_printf(const char *fmt, ...) {
    (void)stream;
    va_list ap;
    va_start(ap, fmt);
    int ret = pfenv_printf(&pfenv_stdout, fmt, ap);
    va_end(ap);
    return ret;
}
#endif

// need this because gcc optimises printf("%c", c) -> putchar(c), and printf("a") -> putchar('a')
int putchar(int c) {
    char chr = c;
    stdout_print_strn(0, &chr, 1);
    return chr;
}

// need this because gcc optimises printf("string\n") -> puts("string")
int puts(const char *s) {
    stdout_print_strn(0, s, strlen(s));
    char chr = '\n';
    stdout_print_strn(0, &chr, 1);
    return 1;
}

typedef struct _strn_pfenv_t {
    char *cur;
    size_t remain;
} strn_pfenv_t;

void strn_print_strn(void *data, const char *str, unsigned int len) {
    strn_pfenv_t *strn_pfenv = data;
    if (len > strn_pfenv->remain) {
        len = strn_pfenv->remain;
    }
    memcpy(strn_pfenv->cur, str, len);
    strn_pfenv->cur += len;
    strn_pfenv->remain -= len;
}
    
int vsnprintf(char *str, size_t size, const char *fmt, va_list ap) {
    strn_pfenv_t strn_pfenv;
    strn_pfenv.cur = str;
    strn_pfenv.remain = size;
    pfenv_t pfenv;
    pfenv.data = &strn_pfenv;
    pfenv.print_strn = strn_print_strn;
    int len = pfenv_printf(&pfenv, fmt, ap);
    // add terminating null byte
    if (size > 0) {
        if (strn_pfenv.remain == 0) {
            strn_pfenv.cur[-1] = 0;
        } else {
            strn_pfenv.cur[0] = 0;
        }
    }
    return len;
}

int snprintf(char *str, size_t size, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int ret = vsnprintf(str, size, fmt, ap);
    va_end(ap);
    return ret;
}
