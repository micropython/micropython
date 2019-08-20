/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2019 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <unistd.h>
#include "py/mpconfig.h"
#include "fsl_lpuart.h"

/*
 * Core UART functions to implement for a port
 */
/* Be invoked in readline function in readline.c */
int mp_hal_stdin_rx_chr(void) {
	unsigned char c;
	LPUART_ReadBlocking(LPUART1, &c, 1);
	return (int)c;
}

void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) 
{
    uint8_t *str_uint8=(uint8_t *)str;
	LPUART_WriteBlocking(LPUART1, str_uint8, len);
}

void mp_hal_stdout_tx_str(const char *str)
{
	mp_hal_stdout_tx_strn(str, strlen(str));
}

void mp_hal_stdout_tx_strn_cooked(const char *str, size_t len)
{
	while (len--) 
	{
        if (*str == '\n') 
		{
            mp_hal_stdout_tx_strn("\r", 1);
        }
        mp_hal_stdout_tx_strn(str++, 1);
    }
}