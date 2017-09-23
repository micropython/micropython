
/**
 * \file
 *
 * \brief SAM Oscillators Controller.
 *
 * Copyright (C) 2016 Atmel Corporation. All rights reserved.
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
#include <hpl_oscctrl_config.h>
#include <hpl_gclk_config.h>

/**
 * \brief Initialize clock sources
 */
void _oscctrl_init_sources(void)
{
	void *hw = (void *)OSCCTRL;

#if CONF_XOSC0_CONFIG == 1
	hri_oscctrl_write_XOSCCTRL_reg(hw,
	                               0,
	                               OSCCTRL_XOSCCTRL_CFDPRESC(CONF_XOSC0_CFDPRESC)
	                                   | OSCCTRL_XOSCCTRL_STARTUP(CONF_XOSC0_STARTUP)
	                                   | (CONF_XOSC0_SWBEN << OSCCTRL_XOSCCTRL_SWBEN_Pos)
	                                   | (CONF_XOSC0_CFDEN << OSCCTRL_XOSCCTRL_CFDEN_Pos)
	                                   | (CONF_XOSC0_ENALC << OSCCTRL_XOSCCTRL_ENALC_Pos)
	                                   | OSCCTRL_XOSCCTRL_IMULT(CONF_XOSC0_IMULT)
	                                   | OSCCTRL_XOSCCTRL_IPTAT(CONF_XOSC0_IPTAT)
	                                   | (CONF_XOSC0_LOWBUFGAIN << OSCCTRL_XOSCCTRL_LOWBUFGAIN_Pos)
	                                   | (CONF_XOSC0_ONDEMAND << OSCCTRL_XOSCCTRL_ONDEMAND_Pos)
	                                   | (CONF_XOSC0_RUNSTDBY << OSCCTRL_XOSCCTRL_RUNSTDBY_Pos)
	                                   | (CONF_XOSC0_XTALEN << OSCCTRL_XOSCCTRL_XTALEN_Pos)
	                                   | (CONF_XOSC0_ENABLE << OSCCTRL_XOSCCTRL_ENABLE_Pos));
#endif

#if CONF_XOSC0_CONFIG == 1
#if CONF_XOSC0_ENABLE == 1
	while (!hri_oscctrl_get_STATUS_XOSCRDY0_bit(hw))
		;
#endif
#if CONF_XOSC0_ONDEMAND == 1
	hri_oscctrl_set_XOSCCTRL_ONDEMAND_bit(hw, 0);
#endif
#endif

#if CONF_XOSC1_CONFIG == 1
	hri_oscctrl_write_XOSCCTRL_reg(hw,
	                               1,
	                               OSCCTRL_XOSCCTRL_CFDPRESC(CONF_XOSC1_CFDPRESC)
	                                   | OSCCTRL_XOSCCTRL_STARTUP(CONF_XOSC1_STARTUP)
	                                   | (CONF_XOSC1_SWBEN << OSCCTRL_XOSCCTRL_SWBEN_Pos)
	                                   | (CONF_XOSC1_CFDEN << OSCCTRL_XOSCCTRL_CFDEN_Pos)
	                                   | (CONF_XOSC1_ENALC << OSCCTRL_XOSCCTRL_ENALC_Pos)
	                                   | OSCCTRL_XOSCCTRL_IMULT(CONF_XOSC1_IMULT)
	                                   | OSCCTRL_XOSCCTRL_IPTAT(CONF_XOSC1_IPTAT)
	                                   | (CONF_XOSC1_LOWBUFGAIN << OSCCTRL_XOSCCTRL_LOWBUFGAIN_Pos)
	                                   | (CONF_XOSC1_ONDEMAND << OSCCTRL_XOSCCTRL_ONDEMAND_Pos)
	                                   | (CONF_XOSC1_RUNSTDBY << OSCCTRL_XOSCCTRL_RUNSTDBY_Pos)
	                                   | (CONF_XOSC1_XTALEN << OSCCTRL_XOSCCTRL_XTALEN_Pos)
	                                   | (CONF_XOSC1_ENABLE << OSCCTRL_XOSCCTRL_ENABLE_Pos));
#endif

#if CONF_XOSC1_CONFIG == 1
#if CONF_XOSC1_ENABLE == 1
	while (!hri_oscctrl_get_STATUS_XOSCRDY1_bit(hw))
		;
#endif
#if CONF_XOSC1_ONDEMAND == 1
	hri_oscctrl_set_XOSCCTRL_ONDEMAND_bit(hw, 1);
#endif
#endif

	(void)hw;
}

void _oscctrl_init_referenced_generators(void)
{
	void *hw = (void *)OSCCTRL;

#if CONF_DFLL_CONFIG == 1
	hri_gclk_write_GENCTRL_SRC_bf(GCLK, 0, GCLK_GENCTRL_SRC_OSCULP32K);
	uint8_t tmp;
	hri_oscctrl_write_DFLLCTRLA_reg(hw, 0);
#if CONF_DFLL_USBCRM != 1 && CONF_DFLL_MODE != 0
	hri_gclk_write_PCHCTRL_reg(
	    GCLK, OSCCTRL_GCLK_ID_DFLL48, (1 << GCLK_PCHCTRL_CHEN_Pos) | GCLK_PCHCTRL_GEN(CONF_DFLL_GCLK));
#endif

	hri_oscctrl_write_DFLLMUL_reg(hw,
	                              OSCCTRL_DFLLMUL_CSTEP(CONF_DFLL_CSTEP) | OSCCTRL_DFLLMUL_FSTEP(CONF_DFLL_FSTEP)
	                                  | OSCCTRL_DFLLMUL_MUL(CONF_DFLL_MUL));
	while (hri_oscctrl_get_DFLLSYNC_DFLLMUL_bit(hw))
		;

	hri_oscctrl_write_DFLLCTRLB_reg(hw, 0);
	while (hri_oscctrl_get_DFLLSYNC_DFLLCTRLB_bit(hw))
		;

	tmp = (CONF_DFLL_RUNSTDBY << OSCCTRL_DFLLCTRLA_RUNSTDBY_Pos) | OSCCTRL_DFLLCTRLA_ENABLE;
	hri_oscctrl_write_DFLLCTRLA_reg(hw, tmp);
	while (hri_oscctrl_get_DFLLSYNC_ENABLE_bit(hw))
		;

#if CONF_DFLL_OVERWRITE_CALIBRATION == 1
	hri_oscctrl_write_DFLLVAL_reg(hw, OSCCTRL_DFLLVAL_COARSE(CONF_DFLL_COARSE) | OSCCTRL_DFLLVAL_FINE(CONF_DFLL_FINE));
#endif
	hri_oscctrl_write_DFLLVAL_reg(hw, hri_oscctrl_read_DFLLVAL_reg(hw));
	while (hri_oscctrl_get_DFLLSYNC_DFLLVAL_bit(hw))
		;

	tmp = (CONF_DFLL_WAITLOCK << OSCCTRL_DFLLCTRLB_WAITLOCK_Pos) | (CONF_DFLL_BPLCKC << OSCCTRL_DFLLCTRLB_BPLCKC_Pos)
	      | (CONF_DFLL_QLDIS << OSCCTRL_DFLLCTRLB_QLDIS_Pos) | (CONF_DFLL_CCDIS << OSCCTRL_DFLLCTRLB_CCDIS_Pos)
	      | (CONF_DFLL_USBCRM << OSCCTRL_DFLLCTRLB_USBCRM_Pos) | (CONF_DFLL_LLAW << OSCCTRL_DFLLCTRLB_LLAW_Pos)
	      | (CONF_DFLL_STABLE << OSCCTRL_DFLLCTRLB_STABLE_Pos) | (CONF_DFLL_MODE << OSCCTRL_DFLLCTRLB_MODE_Pos) | 0;
	hri_oscctrl_write_DFLLCTRLB_reg(hw, tmp);
	while (hri_oscctrl_get_DFLLSYNC_DFLLCTRLB_bit(hw))
		;
#endif

#if CONF_FDPLL0_CONFIG == 1
#if CONF_FDPLL0_REFCLK == 0
	hri_gclk_write_PCHCTRL_reg(
	    GCLK, OSCCTRL_GCLK_ID_FDPLL0, (1 << GCLK_PCHCTRL_CHEN_Pos) | GCLK_PCHCTRL_GEN(CONF_FDPLL0_GCLK));
#endif
	hri_oscctrl_write_DPLLRATIO_reg(
	    hw, 0, OSCCTRL_DPLLRATIO_LDRFRAC(CONF_FDPLL0_LDRFRAC) | OSCCTRL_DPLLRATIO_LDR(CONF_FDPLL0_LDR));
	hri_oscctrl_write_DPLLCTRLB_reg(hw,
	                                0,
	                                OSCCTRL_DPLLCTRLB_DIV(CONF_FDPLL0_DIV)
	                                    | (CONF_FDPLL0_DCOEN << OSCCTRL_DPLLCTRLB_DCOEN_Pos)
	                                    | OSCCTRL_DPLLCTRLB_DCOFILTER(CONF_FDPLL0_DCOFILTER)
	                                    | (CONF_FDPLL0_LBYPASS << OSCCTRL_DPLLCTRLB_LBYPASS_Pos)
	                                    | OSCCTRL_DPLLCTRLB_LTIME(CONF_FDPLL0_LTIME)
	                                    | OSCCTRL_DPLLCTRLB_REFCLK(CONF_FDPLL0_REFCLK)
	                                    | (CONF_FDPLL0_WUF << OSCCTRL_DPLLCTRLB_WUF_Pos)
	                                    | OSCCTRL_DPLLCTRLB_FILTER(CONF_FDPLL0_FILTER));
	hri_oscctrl_write_DPLLCTRLA_reg(hw,
	                                0,
	                                (CONF_FDPLL0_RUNSTDBY << OSCCTRL_DPLLCTRLA_RUNSTDBY_Pos)
	                                    | (CONF_FDPLL0_ENABLE << OSCCTRL_DPLLCTRLA_ENABLE_Pos));
#endif

#if CONF_FDPLL1_CONFIG == 1
#if CONF_FDPLL1_REFCLK == 0
	hri_gclk_write_PCHCTRL_reg(
	    GCLK, OSCCTRL_GCLK_ID_FDPLL1, (1 << GCLK_PCHCTRL_CHEN_Pos) | GCLK_PCHCTRL_GEN(CONF_FDPLL1_GCLK));
#endif
	hri_oscctrl_write_DPLLRATIO_reg(
	    hw, 1, OSCCTRL_DPLLRATIO_LDRFRAC(CONF_FDPLL1_LDRFRAC) | OSCCTRL_DPLLRATIO_LDR(CONF_FDPLL1_LDR));
	hri_oscctrl_write_DPLLCTRLB_reg(hw,
	                                1,
	                                OSCCTRL_DPLLCTRLB_DIV(CONF_FDPLL1_DIV)
	                                    | (CONF_FDPLL1_DCOEN << OSCCTRL_DPLLCTRLB_DCOEN_Pos)
	                                    | OSCCTRL_DPLLCTRLB_DCOFILTER(CONF_FDPLL1_DCOFILTER)
	                                    | (CONF_FDPLL1_LBYPASS << OSCCTRL_DPLLCTRLB_LBYPASS_Pos)
	                                    | OSCCTRL_DPLLCTRLB_LTIME(CONF_FDPLL1_LTIME)
	                                    | OSCCTRL_DPLLCTRLB_REFCLK(CONF_FDPLL1_REFCLK)
	                                    | (CONF_FDPLL1_WUF << OSCCTRL_DPLLCTRLB_WUF_Pos)
	                                    | OSCCTRL_DPLLCTRLB_FILTER(CONF_FDPLL1_FILTER));
	hri_oscctrl_write_DPLLCTRLA_reg(hw,
	                                1,
	                                (CONF_FDPLL1_RUNSTDBY << OSCCTRL_DPLLCTRLA_RUNSTDBY_Pos)
	                                    | (CONF_FDPLL1_ENABLE << OSCCTRL_DPLLCTRLA_ENABLE_Pos));
#endif

#if CONF_DFLL_CONFIG == 1
	if (hri_oscctrl_get_DFLLCTRLB_MODE_bit(hw)) {
		hri_oscctrl_status_reg_t status_mask = OSCCTRL_STATUS_DFLLRDY | OSCCTRL_STATUS_DFLLLCKC;

		while (hri_oscctrl_get_STATUS_reg(hw, status_mask) != status_mask)
			;
	} else {
		while (!hri_oscctrl_get_STATUS_DFLLRDY_bit(hw))
			;
	}
#if CONF_DFLL_ONDEMAND == 1
	hri_oscctrl_set_DFLLCTRLA_ONDEMAND_bit(hw);
#endif
#endif

#if CONF_FDPLL0_CONFIG == 1
#if CONF_FDPLL0_ENABLE == 1
	while (!(hri_oscctrl_get_DPLLSTATUS_LOCK_bit(hw, 0) || hri_oscctrl_get_DPLLSTATUS_CLKRDY_bit(hw, 0)))
		;
#endif
#if CONF_FDPLL0_ONDEMAND == 1
	hri_oscctrl_set_DPLLCTRLA_ONDEMAND_bit(hw, 0);
#endif
#endif

#if CONF_FDPLL1_CONFIG == 1
#if CONF_FDPLL1_ENABLE == 1
	while (!(hri_oscctrl_get_DPLLSTATUS_LOCK_bit(hw, 1) || hri_oscctrl_get_DPLLSTATUS_CLKRDY_bit(hw, 1)))
		;
#endif
#if CONF_FDPLL1_ONDEMAND == 1
	hri_oscctrl_set_DPLLCTRLA_ONDEMAND_bit(hw, 1);
#endif
#endif

#if CONF_DFLL_CONFIG == 1
	while (hri_gclk_read_SYNCBUSY_reg(GCLK))
		;
	hri_gclk_write_GENCTRL_SRC_bf(GCLK, 0, CONF_GCLK_GEN_0_SOURCE);
#endif
	(void)hw;
}
