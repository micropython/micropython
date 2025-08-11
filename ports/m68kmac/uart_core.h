#pragma once

#include "py/mpprint.h"

void mp_console_showhide(bool shown);
void mp_console_idle();
void mp_hal_stdin_init(void);
bool mp_hal_stdin_available(void);
int mp_hal_stdin_rx_chr(void);
extern mp_print_t debug_print;
mp_uint_t mp_hal_stdout_tx_strn(const char *str, mp_uint_t len);
#define DEBUG_PRINT(...) mp_printf(&debug_print, __VA_ARGS__)
