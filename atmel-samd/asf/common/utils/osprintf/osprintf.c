/**
 *
 * \file
 *
 * \brief
 *
 * Copyright (c) 2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */
#include "asf.h"
#include <stdio.h>
#include <stdarg.h>
#include "FreeRTOS.h"
#include "osprintf.h"

xSemaphoreHandle trace_lock;

void osprintf_init(void *usart, const void *opt)
{
	trace_lock = xSemaphoreCreateMutex();
	stdio_serial_init(usart, opt);
}

void osprintf(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	xSemaphoreTake(trace_lock, portMAX_DELAY);
	vprintf(fmt, args);
	xSemaphoreGive(trace_lock);
	va_end(args);
}

void osprint(const char *s)
{
	xSemaphoreTake(trace_lock, portMAX_DELAY);
	puts(s);
	xSemaphoreGive(trace_lock);		
}

void osprint_hex_array(uint8_t *p, int sz)
{
	xSemaphoreTake(trace_lock, portMAX_DELAY);
	while (sz) {
		if (sz < 8) {
			while (sz--) {
				printf("%02x ", *p);
				p++;
			}
			break;
		} 
		printf("%02x %02x %02x %02x %02x %02x %02x %02x\n",
				p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);
		p += 8;
		sz -= 8;
	}
	printf("\n");
	xSemaphoreGive(trace_lock);
}
 
void osprint_sem_take(void)
{
	xSemaphoreTake(trace_lock, portMAX_DELAY);
}

void osprint_sem_give(void)
{
	xSemaphoreGive(trace_lock);
}
