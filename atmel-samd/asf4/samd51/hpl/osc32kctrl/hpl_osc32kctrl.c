
/**
 * \file
 *
 * \brief SAM 32k Oscillators Controller.
 *
 * Copyright (C) 2015-2016 Atmel Corporation. All rights reserved.
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
#include <hpl_init.h>
#include <hpl_osc32kctrl_config.h>

/**
 * \brief Initialize 32 kHz clock sources
 */
void _osc32kctrl_init_sources(void)
{
	void *   hw    = (void *)OSC32KCTRL;
	uint16_t calib = 0;

#if CONF_XOSC32K_CONFIG == 1
	hri_osc32kctrl_write_XOSC32K_reg(hw,
	                                 OSC32KCTRL_XOSC32K_STARTUP(CONF_XOSC32K_STARTUP)
	                                     | (CONF_XOSC32K_ONDEMAND << OSC32KCTRL_XOSC32K_ONDEMAND_Pos)
	                                     | (CONF_XOSC32K_RUNSTDBY << OSC32KCTRL_XOSC32K_RUNSTDBY_Pos)
	                                     | (CONF_XOSC32K_EN1K << OSC32KCTRL_XOSC32K_EN1K_Pos)
	                                     | (CONF_XOSC32K_EN32K << OSC32KCTRL_XOSC32K_EN32K_Pos)
	                                     | (CONF_XOSC32K_XTALEN << OSC32KCTRL_XOSC32K_XTALEN_Pos)
	                                     |
#ifdef CONF_XOSC32K_CGM
	                                     OSC32KCTRL_XOSC32K_CGM(CONF_XOSC32K_CGM) |
#endif
	                                     (CONF_XOSC32K_ENABLE << OSC32KCTRL_XOSC32K_ENABLE_Pos));

	hri_osc32kctrl_write_CFDCTRL_reg(hw, (CONF_XOSC32K_CFDEN << OSC32KCTRL_CFDCTRL_CFDEN_Pos));

	hri_osc32kctrl_write_EVCTRL_reg(hw, (CONF_XOSC32K_CFDEO << OSC32KCTRL_EVCTRL_CFDEO_Pos));
#endif

#if CONF_OSCULP32K_CONFIG == 1
	calib = hri_osc32kctrl_read_OSCULP32K_CALIB_bf(hw);
	hri_osc32kctrl_write_OSCULP32K_reg(hw,
#if CONF_OSCULP32K_CALIB_ENABLE == 1
	                                   OSC32KCTRL_OSCULP32K_CALIB(CONF_OSCULP32K_CALIB)
#else
	                                   OSC32KCTRL_OSCULP32K_CALIB(calib)
#endif
	                                       );
#endif

#if CONF_XOSC32K_CONFIG
#if CONF_XOSC32K_ENABLE == 1 && CONF_XOSC32K_ONDEMAND == 0
	while (!hri_osc32kctrl_get_STATUS_XOSC32KRDY_bit(hw))
		;
#endif
#endif

	hri_osc32kctrl_write_RTCCTRL_reg(hw, OSC32KCTRL_RTCCTRL_RTCSEL(CONF_RTCCTRL));
	(void)calib;
}
