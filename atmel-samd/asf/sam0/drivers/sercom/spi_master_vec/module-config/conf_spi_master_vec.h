/**
 * \file
 *
 * \brief SERCOM SPI master with vectored I/O driver configuration
 *
 * Copyright (C) 2013-2015 Atmel Corporation. All rights reserved.
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef CONF_SPI_MASTER_VEC_H
#define CONF_SPI_MASTER_VEC_H

#if defined(__FREERTOS__) || defined(__DOXYGEN__)
#  include <FreeRTOS.h>
#  include <semphr.h>

#  define CONF_SPI_MASTER_VEC_OS_SUPPORT
#  define CONF_SPI_MASTER_VEC_SEMAPHORE_TYPE                   xSemaphoreHandle
#  define CONF_SPI_MASTER_VEC_CREATE_SEMAPHORE(semaphore)  \
		vSemaphoreCreateBinary(semaphore)
#  define CONF_SPI_MASTER_VEC_DELETE_SEMAPHORE(semaphore)  \
		vSemaphoreDelete(semaphore)
#  define CONF_SPI_MASTER_VEC_TAKE_SEMAPHORE(semaphore)  \
		xSemaphoreTake((semaphore), portMAX_DELAY)
#  define CONF_SPI_MASTER_VEC_GIVE_SEMAPHORE(semaphore)  \
		xSemaphoreGive((semaphore))
#  define CONF_SPI_MASTER_VEC_GIVE_SEMAPHORE_FROM_ISR(semaphore)  \
		xSemaphoreGiveFromISR((semaphore), NULL)
#endif

#endif // CONF_SPI_MASTER_VEC_H