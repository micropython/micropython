/**
 * \file
 *
 * \brief SAM System Controller.
 *
 * Copyright (C) 2015 - 2016 Atmel Corporation. All rights reserved.
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
#include <hpl_sysctrl_config.h>
#include <utils_assert.h>

/**
 * \brief Initializes clock generators
 *
 * All GCLK generators are running when this function returns.
 */
void _sysctrl_init_sources(void)
{
	void *   hw = (void *)SYSCTRL;
	uint16_t calib;

#if CONF_XOSC32K_CONFIG == 1
	hri_sysctrl_write_XOSC32K_reg(hw,
	                              (CONF_XOSC32K_WRTLOCK << SYSCTRL_XOSC32K_WRTLOCK_Pos)
	                                  | SYSCTRL_XOSC32K_STARTUP(CONF_XOSC32K_STARTUP)
	                                  | (CONF_XOSC32K_RUNSTDBY << SYSCTRL_XOSC32K_RUNSTDBY_Pos)
	                                  | (CONF_XOSC32K_AAMPEN << SYSCTRL_XOSC32K_AAMPEN_Pos)
	                                  | (CONF_XOSC32K_EN1K << SYSCTRL_XOSC32K_EN1K_Pos)
	                                  | (CONF_XOSC32K_EN32K << SYSCTRL_XOSC32K_EN32K_Pos)
	                                  | (CONF_XOSC32K_XTALEN << SYSCTRL_XOSC32K_XTALEN_Pos)
	                                  | (CONF_XOSC32K_ENABLE << SYSCTRL_XOSC32K_ENABLE_Pos));
#endif

#if CONF_XOSC_CONFIG == 1
	hri_sysctrl_write_XOSC_reg(hw,
	                           SYSCTRL_XOSC_STARTUP(CONF_XOSC_STARTUP) | (CONF_XOSC_AMPGC << SYSCTRL_XOSC_AMPGC_Pos)
	                               | SYSCTRL_XOSC_GAIN(CONF_XOSC_GAIN)
	                               | (CONF_XOSC_RUNSTDBY << SYSCTRL_XOSC_RUNSTDBY_Pos)
	                               | (CONF_XOSC_XTALEN << SYSCTRL_XOSC_XTALEN_Pos)
	                               | (CONF_XOSC_ENABLE << SYSCTRL_XOSC_ENABLE_Pos));
#endif

#if CONF_OSC8M_CONFIG == 1
	calib = hri_sysctrl_read_OSC8M_CALIB_bf(hw);

	hri_sysctrl_write_OSC8M_reg(hw,
	                            SYSCTRL_OSC8M_FRANGE(hri_sysctrl_read_OSC8M_FRANGE_bf(hw)) |
#if CONF_OSC8M_OVERWRITE_CALIBRATION == 1
	                                SYSCTRL_OSC8M_CALIB(CONF_OSC8M_CALIB) |
#else
	                                SYSCTRL_OSC8M_CALIB(calib) |
#endif
	                                SYSCTRL_OSC8M_PRESC(CONF_OSC8M_PRESC)
	                                | (CONF_OSC8M_RUNSTDBY << SYSCTRL_OSC8M_RUNSTDBY_Pos)
	                                | (CONF_OSC8M_ENABLE << SYSCTRL_OSC8M_ENABLE_Pos));
#endif

#if CONF_OSC32K_CONFIG == 1
	/* OSC32K calibration value at bit 44:38 of memory 0x00806020 */
	calib = (*((uint32_t *)0x00806024) & 0x0001FC0) >> 6;

	hri_sysctrl_write_OSC32K_reg(hw,
#if CONF_OSC32K_OVERWRITE_CALIBRATION == 1
	                             SYSCTRL_OSC32K_CALIB(CONF_OSC32K_CALIB) |
#else
	                             SYSCTRL_OSC32K_CALIB(calib) |
#endif
	                                 (CONF_OSC32K_WRTLOCK << SYSCTRL_OSC32K_WRTLOCK_Pos)
	                                 | SYSCTRL_OSC32K_STARTUP(CONF_OSC32K_STARTUP)
	                                 | (CONF_OSC32K_RUNSTDBY << SYSCTRL_OSC32K_RUNSTDBY_Pos)
	                                 | (CONF_OSC32K_EN1K << SYSCTRL_OSC32K_EN1K_Pos)
	                                 | (CONF_OSC32K_EN32K << SYSCTRL_OSC32K_EN32K_Pos)
	                                 | (1 << SYSCTRL_OSC32K_ENABLE_Pos));
#else
	/* Enable OSC32K anyway since GCLK configuration may need it to sync */
	hri_sysctrl_set_OSC32K_ENABLE_bit(hw);
#endif

#if CONF_OSCULP32K_CONFIG == 1
	hri_sysctrl_write_OSCULP32K_reg(hw,
#if CONF_OSC32K_OVERWRITE_CALIBRATION == 1
	                                SYSCTRL_OSCULP32K_CALIB(CONF_OSCULP32K_CALIB) |
#else
	                                SYSCTRL_OSCULP32K_CALIB(calib) |
#endif
	                                    (CONF_OSC32K_WRTLOCK << SYSCTRL_OSCULP32K_WRTLOCK_Pos));
#endif

#if CONF_XOSC32K_CONFIG == 1
#if CONF_XOSC32K_ENABLE == 1
	while (!hri_sysctrl_get_PCLKSR_XOSC32KRDY_bit(hw))
		;
#endif
#if CONF_XOSC32K_ONDEMAND == 1
	hri_sysctrl_set_XOSC32K_ONDEMAND_bit(hw);
#endif
#endif

#if CONF_XOSC_CONFIG == 1
#if CONF_XOSC_ENABLE == 1
	while (!hri_sysctrl_get_PCLKSR_XOSCRDY_bit(hw))
		;
#endif
#if CONF_XOSC_ONDEMAND == 1
	hri_sysctrl_set_XOSC_ONDEMAND_bit(hw);
#endif
#endif

#if CONF_OSC32K_CONFIG == 1
#if CONF_OSC32K_ENABLE == 1
	while (!hri_sysctrl_get_PCLKSR_OSC32KRDY_bit(hw))
		;
#endif
#if CONF_OSC32K_ONDEMAND == 1
	hri_sysctrl_set_OSC32K_ONDEMAND_bit(hw);
#endif
#endif

#if CONF_OSC8M_CONFIG == 1
#if CONF_OSC8M_ENABLE == 1
	while (!hri_sysctrl_get_PCLKSR_OSC8MRDY_bit(hw))
		;
#endif
#if CONF_OSC8M_ONDEMAND == 1
	hri_sysctrl_set_OSC8M_ONDEMAND_bit(hw);
#endif
#endif

	(void)calib, (void)hw;
}

void _sysctrl_init_referenced_generators(void)
{
	void *hw = (void *)SYSCTRL;

#if CONF_DFLL_CONFIG == 1

#if CONF_DFLL_USBCRM != 1 && CONF_DFLL_MODE != CONF_DFLL_OPEN_LOOP_MODE
	hri_gclk_write_CLKCTRL_reg(GCLK,
	                           GCLK_CLKCTRL_ID(0) | GCLK_CLKCTRL_GEN(CONF_DFLL_GCLK) | (1 << GCLK_CLKCTRL_CLKEN_Pos));
#endif

	hri_sysctrl_write_DFLLCTRL_reg(hw, SYSCTRL_DFLLCTRL_ENABLE);
	while (!hri_sysctrl_get_PCLKSR_DFLLRDY_bit(hw))
		;

	hri_sysctrl_write_DFLLMUL_reg(hw,
	                              SYSCTRL_DFLLMUL_CSTEP(CONF_DFLL_CSTEP) | SYSCTRL_DFLLMUL_FSTEP(CONF_DFLL_FSTEP)
	                                  | SYSCTRL_DFLLMUL_MUL(CONF_DFLL_MUL));
	hri_sysctrl_write_DFLLVAL_reg(hw, CONF_DFLLVAL);

	hri_sysctrl_dfllctrl_reg_t tmp =

	    (CONF_DFLL_WAITLOCK << SYSCTRL_DFLLCTRL_WAITLOCK_Pos) | (CONF_DFLL_BPLCKC << SYSCTRL_DFLLCTRL_BPLCKC_Pos)
	    | (CONF_DFLL_QLDIS << SYSCTRL_DFLLCTRL_QLDIS_Pos) | (CONF_DFLL_CCDIS << SYSCTRL_DFLLCTRL_CCDIS_Pos)
	    | (CONF_DFLL_RUNSTDBY << SYSCTRL_DFLLCTRL_RUNSTDBY_Pos) | (CONF_DFLL_USBCRM << SYSCTRL_DFLLCTRL_USBCRM_Pos)
	    | (CONF_DFLL_LLAW << SYSCTRL_DFLLCTRL_LLAW_Pos) | (CONF_DFLL_STABLE << SYSCTRL_DFLLCTRL_STABLE_Pos)
	    | (CONF_DFLL_MODE << SYSCTRL_DFLLCTRL_MODE_Pos) | (CONF_DFLL_ENABLE << SYSCTRL_DFLLCTRL_ENABLE_Pos);

	hri_sysctrl_write_DFLLCTRL_reg(hw, tmp);
#endif

#if CONF_DPLL_CONFIG == 1
#if CONF_DPLL_REFCLK == SYSCTRL_DPLLCTRLB_REFCLK_GCLK_Val
	hri_gclk_write_CLKCTRL_reg(GCLK,
	                           GCLK_CLKCTRL_ID(1) | GCLK_CLKCTRL_GEN(CONF_DPLL_GCLK) | (1 << GCLK_CLKCTRL_CLKEN_Pos));
#endif

	hri_sysctrl_write_DPLLCTRLA_reg(hw,
	                                (CONF_DPLL_RUNSTDBY << SYSCTRL_DPLLCTRLA_RUNSTDBY_Pos)
	                                    | (CONF_DPLL_ENABLE << SYSCTRL_DPLLCTRLA_ENABLE_Pos));
	hri_sysctrl_write_DPLLRATIO_reg(
	    hw, SYSCTRL_DPLLRATIO_LDRFRAC(CONF_DPLL_LDRFRAC) | SYSCTRL_DPLLRATIO_LDR(CONF_DPLL_LDR));
	hri_sysctrl_write_DPLLCTRLB_reg(hw,
	                                SYSCTRL_DPLLCTRLB_DIV(CONF_DPLL_DIV)
	                                    | (CONF_DPLL_LBYPASS << SYSCTRL_DPLLCTRLB_LBYPASS_Pos)
	                                    | SYSCTRL_DPLLCTRLB_LTIME(CONF_DPLL_LTIME)
	                                    | SYSCTRL_DPLLCTRLB_REFCLK(CONF_DPLL_REFCLK)
	                                    | (CONF_DPLL_WUF << SYSCTRL_DPLLCTRLB_WUF_Pos)
	                                    | (CONF_DPLL_LPEN << SYSCTRL_DPLLCTRLB_LPEN_Pos)
	                                    | SYSCTRL_DPLLCTRLB_FILTER(CONF_DPLL_FILTER));
#endif

#if CONF_DFLL_CONFIG == 1
#if CONF_DFLL_ENABLE == 1
	if (hri_sysctrl_get_DFLLCTRL_MODE_bit(hw)) {

#if CONF_DFLL_USBCRM == 0
		hri_sysctrl_pclksr_reg_t status_mask
		    = SYSCTRL_PCLKSR_DFLLRDY | SYSCTRL_PCLKSR_DFLLLCKF | SYSCTRL_PCLKSR_DFLLLCKC;
#else
		hri_sysctrl_pclksr_reg_t status_mask = SYSCTRL_PCLKSR_DFLLRDY;
#endif

		while (hri_sysctrl_get_PCLKSR_reg(hw, status_mask) != status_mask)
			;
	} else {
		while (!hri_sysctrl_get_PCLKSR_DFLLRDY_bit(hw))
			;
	}
#endif
#if CONF_DFLL_ONDEMAND == 1
	hri_sysctrl_set_DFLLCTRL_ONDEMAND_bit(hw);
#endif
#endif

#if CONF_DPLL_CONFIG == 1
#if CONF_DPLL_ENABLE == 1
	while (!(hri_sysctrl_get_DPLLSTATUS_ENABLE_bit(hw) || hri_sysctrl_get_DPLLSTATUS_LOCK_bit(hw)
	         || hri_sysctrl_get_DPLLSTATUS_CLKRDY_bit(hw)))
		;
#endif
#if CONF_DPLL_ONDEMAND == 1
	hri_sysctrl_set_DPLLCTRLA_ONDEMAND_bit(hw);
#endif
#endif

#if CONF_DFLL_CONFIG == 1
	while (hri_gclk_get_STATUS_SYNCBUSY_bit(GCLK))
		;
#endif

#if CONF_OSC32K_CONFIG == 0 || CONF_OSC32K_ENABLE == 0
	/* Disable after all possible configurations needs sync written. */
	hri_sysctrl_clear_OSC32K_ENABLE_bit(hw);
#endif

	(void)hw;
}
