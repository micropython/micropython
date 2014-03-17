#include <stdint.h>
#include <string.h>
#include <stdarg.h>

#include "std.h"
#include "misc.h"
#include "systick.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#if 0
#include "lcd.h"
#endif
#include "usart.h"
#include "usb.h"

#if MICROPY_ENABLE_FLOAT
#include "formatfloat.h"
#endif

#define PF_FLAG_LEFT_ADJUST (0x01)
#define PF_FLAG_SHOW_SIGN   (0x02)
#define PF_FLAG_SPACE_SIGN  (0x04)
#define PF_FLAG_NO_TRAILZ   (0x08)
#define PF_FLAG_ZERO_PAD    (0x10)

// tricky; we compute pad string by: pad_chars + (flags & PF_FLAG_ZERO_PAD)
#define PF_PAD_SIZE PF_FLAG_ZERO_PAD
static const char *pad_chars = "                0000000000000000";

typedef struct _pfenv_t {
    void *data;
    void (*print_strn)(void *, const char *str, unsigned int len);
} pfenv_t;

static void print_str_dummy(void *data, const char *str, unsigned int len) {
}

const pfenv_t pfenv_dummy = {0, print_str_dummy};

static int pfenv_print_strn(const pfenv_t *pfenv, const char *str, unsigned int len, int flags, int width) {
    int pad = width - len;
    if (pad > 0 && (flags & PF_FLAG_LEFT_ADJUST) == 0) {
        while (pad > 0) {
            int p = pad;
            if (p > PF_PAD_SIZE)
                p = PF_PAD_SIZE;
            pfenv->print_strn(pfenv->data, pad_chars + (flags & PF_FLAG_ZERO_PAD), p);
            pad -= p;
        }
    }
    pfenv->print_strn(pfenv->data, str, len);
    while (pad > 0) {
        int p = pad;
        if (p > PF_PAD_SIZE)
            p = PF_PAD_SIZE;
        pfenv->print_strn(pfenv->data, pad_chars, p);
        pad -= p;
    }
    return len;
}

// enough room for 32 signed number
#define INT_BUF_SIZE (12)

static int pfenv_print_int(const pfenv_t *pfenv, unsigned int x, int sgn, int base, int base_char, int flags, int width) {
    char sign = 0;
    if (sgn) {
        if ((int)x < 0) {
            sign = '-';
            x = -x;
        } else if (flags & PF_FLAG_SHOW_SIGN) {
            sign = '+';
        } else if (flags & PF_FLAG_SPACE_SIGN) {
            sign = ' ';
        }
    }

    char buf[INT_BUF_SIZE];
    char *b = buf + INT_BUF_SIZE;

    if (x == 0) {
        *(--b) = '0';
    } else {
        do {
            int c = x % base;
            x /= base;
            if (c >= 10) {
                c += base_char - 10;
            } else {
                c += '0';
            }
            *(--b) = c;
        } while (b > buf && x != 0);
    }

    if (b > buf && sign != 0) {
        *(--b) = sign;
    }

    return pfenv_print_strn(pfenv, b, buf + INT_BUF_SIZE - b, flags, width);
}

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
        while (*fmt != '\0') {
            if (*fmt == '-') flags |= PF_FLAG_LEFT_ADJUST;
            else if (*fmt == '+') flags |= PF_FLAG_SHOW_SIGN;
            else if (*fmt == ' ') flags |= PF_FLAG_SPACE_SIGN;
            else if (*fmt == '!') flags |= PF_FLAG_NO_TRAILZ;
            else if (*fmt == '0') flags |= PF_FLAG_ZERO_PAD;
            else break;
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
                    chrs += pfenv_print_strn(pfenv, "true", 4, flags, width);
                } else {
                    chrs += pfenv_print_strn(pfenv, "false", 5, flags, width);
                }
                break;
            case 'c':
            {
                char str = va_arg(args, int);
                chrs += pfenv_print_strn(pfenv, &str, 1, flags, width);
                break;
            }
            case 's':
            {
                const char *str = va_arg(args, const char*);
                if (str) {
                    if (prec < 0) {
                        prec = strlen(str);
                    }
                    chrs += pfenv_print_strn(pfenv, str, prec, flags, width);
                } else {
                    chrs += pfenv_print_strn(pfenv, "(null)", 6, flags, width);
                }
                break;
            }
            case 'u':
                chrs += pfenv_print_int(pfenv, va_arg(args, int), 0, 10, 'a', flags, width);
                break;
            case 'd':
                chrs += pfenv_print_int(pfenv, va_arg(args, int), 1, 10, 'a', flags, width);
                break;
            case 'x':
            case 'p': // ?
                chrs += pfenv_print_int(pfenv, va_arg(args, int), 0, 16, 'a', flags, width);
                break;
            case 'X':
            case 'P': // ?
                chrs += pfenv_print_int(pfenv, va_arg(args, int), 0, 16, 'A', flags, width);
                break;
#if MICROPY_ENABLE_FLOAT
            case 'e':
            case 'E':
            case 'f':
            case 'F':
            case 'g':
            case 'G':
            {
                char buf[32];
                char sign = '\0';

                if (flags & PF_FLAG_SHOW_SIGN) {
                    sign = '+';
                }
                else
                if (flags & PF_FLAG_SPACE_SIGN) {
                    sign = ' ';
                }
                float f = va_arg(args, double);
                int len = format_float(f, buf, sizeof(buf), *fmt, prec, sign);
                char *s = buf;

                // buf[0] < '0' returns true if the first character is space, + or -
                // buf[1] < '9' matches a digit, and doesn't match when we get back +nan or +inf
                if (buf[0] < '0' && buf[1] <= '9' && (flags & PF_FLAG_ZERO_PAD)) {
                    chrs += pfenv_print_strn(pfenv, &buf[0], 1, 0, 1);
                    s++;
                    width--;
                    len--;
                }
                if (*s < '0' || *s >= '9') {
                    // For inf or nan, we don't want to zero pad.
                    flags &= ~PF_FLAG_ZERO_PAD;
                }
                chrs += pfenv_print_strn(pfenv, s, len, flags, width); 
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
