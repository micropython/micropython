// Host-test stub: printing surface only.
#ifndef HALOW_HOSTTEST_MPPRINT_H
#define HALOW_HOSTTEST_MPPRINT_H
#include <stdarg.h>
#include <stdio.h>
typedef struct _mp_print_t { void *data;
                             void *print_strn;
} mp_print_t;
extern const mp_print_t mp_plat_print;
int mp_printf(const mp_print_t *print, const char *fmt, ...);
int mp_vprintf(const mp_print_t *print, const char *fmt, va_list args);
#endif
