/**
 * \file
 *
 * \brief USART related functionality declaration.
 *
 * Copyright (C) 2014 Atmel Corporation. All rights reserved.
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

#ifndef _HPL_USART_H_INCLUDED
#define _HPL_USART_H_INCLUDED

/**
 * \addtogroup HPL USART SYNC
 *
 * \section hpl_usart_sync_rev Revision History
 * - v1.0.0 Initial Release
 *
 *@{
 */

#include <compiler.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief USART flow control state
 */
union usart_flow_control_state {
	struct {
		uint8_t cts : 1;
		uint8_t rts : 1;
		uint8_t unavailable : 1;
		uint8_t reserved : 5;
	} bit;
	uint8_t value;
};

/**
 * \brief USART baud rate mode
 */
enum usart_baud_rate_mode { USART_BAUDRATE_ASYNCH_ARITHMETIC, USART_BAUDRATE_ASYNCH_FRACTIONAL, USART_BAUDRATE_SYNCH };

/**
 * \brief USART data order
 */
enum usart_data_order { USART_DATA_ORDER_MSB = 0, USART_DATA_ORDER_LSB = 1 };

/**
 * \brief USART mode
 */
enum usart_mode { USART_MODE_ASYNCHRONOUS = 0, USART_MODE_SYNCHRONOUS = 1 };

/**
 * \brief USART parity
 */
enum usart_parity {
	USART_PARITY_EVEN  = 0,
	USART_PARITY_ODD   = 1,
	USART_PARITY_NONE  = 2,
	USART_PARITY_SPACE = 3,
	USART_PARITY_MARK  = 4
};

/**
 * \brief USART stop bits mode
 */
enum usart_stop_bits { USART_STOP_BITS_ONE = 0, USART_STOP_BITS_TWO = 1, USART_STOP_BITS_ONE_P_FIVE = 2 };

/**
 * \brief USART character size
 */
enum usart_character_size {
	USART_CHARACTER_SIZE_8BITS = 0,
	USART_CHARACTER_SIZE_9BITS = 1,
	USART_CHARACTER_SIZE_5BITS = 5,
	USART_CHARACTER_SIZE_6BITS = 6,
	USART_CHARACTER_SIZE_7BITS = 7
};

//@}

#ifdef __cplusplus
}
#endif
/**@}*/
#endif /* _HPL_USART_H_INCLUDED */
