#pragma once

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct _mp_print_t mp_print_t;
extern mp_print_t debug_print;
int mp_printf(const mp_print_t *print, const char *fmt, ...);
#define DPRINTF(fmt, ...) mp_printf(&debug_print, fmt "\n",##__VA_ARGS__)

#if defined(__cplusplus)
}
#endif
