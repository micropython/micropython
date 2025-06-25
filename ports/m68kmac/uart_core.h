#pragma once

#if defined(__cplusplus)
extern "C" {
#endif

#include "py/mpprint.h"

bool mp_hal_stdin_available(void);
int mp_hal_stdin_rx_chr(void);
extern mp_print_t debug_print;
#define DEBUG_PRINT(...) mp_printf(&debug_print, __VA_ARGS__)

#if defined(__cplusplus)
}
#endif
