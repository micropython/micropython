/**
 * \file
 *
 * \brief SAM USART Quick Start
 *
 * Copyright (C) 2012-2015 Atmel Corporation. All rights reserved.
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
#include "asf/sam0/drivers/sercom/usart/usart.h"
#include "asf/sam0/drivers/port/port.h"

#include "uart.h"

#include "conf_board.h"

extern struct usart_module usart_instance;

void configure_usart(void)
{
  struct usart_config config_usart;

  usart_get_config_defaults(&config_usart);

  config_usart.baudrate    = 115200;
  config_usart.mux_setting = EDBG_CDC_SERCOM_MUX_SETTING;
  config_usart.pinmux_pad0 = EDBG_CDC_SERCOM_PINMUX_PAD0;
  config_usart.pinmux_pad1 = EDBG_CDC_SERCOM_PINMUX_PAD1;
  config_usart.pinmux_pad2 = EDBG_CDC_SERCOM_PINMUX_PAD2;
  config_usart.pinmux_pad3 = EDBG_CDC_SERCOM_PINMUX_PAD3;

  while (usart_init(&usart_instance, EDBG_CDC_MODULE, &config_usart) != STATUS_OK) {
  }
  usart_enable(&usart_instance);
}
