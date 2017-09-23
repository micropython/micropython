/**
 * \file
 *
 * \brief SAM SDHC
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
 */

#ifdef _SAMD51_SDHC_COMPONENT_
#ifndef _HRI_SDHC_D51_H_INCLUDED_
#define _HRI_SDHC_D51_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <hal_atomic.h>

#if defined(ENABLE_SDHC_CRITICAL_SECTIONS)
#define SDHC_CRITICAL_SECTION_ENTER() CRITICAL_SECTION_ENTER()
#define SDHC_CRITICAL_SECTION_LEAVE() CRITICAL_SECTION_LEAVE()
#else
#define SDHC_CRITICAL_SECTION_ENTER()
#define SDHC_CRITICAL_SECTION_LEAVE()
#endif

typedef uint16_t hri_sdhc_acesr_reg_t;
typedef uint16_t hri_sdhc_bcr_reg_t;
typedef uint16_t hri_sdhc_bsr_reg_t;
typedef uint16_t hri_sdhc_ccr_reg_t;
typedef uint16_t hri_sdhc_cr_reg_t;
typedef uint16_t hri_sdhc_eisier_reg_t;
typedef uint16_t hri_sdhc_eister_reg_t;
typedef uint16_t hri_sdhc_eistr_reg_t;
typedef uint16_t hri_sdhc_feraces_reg_t;
typedef uint16_t hri_sdhc_fereis_reg_t;
typedef uint16_t hri_sdhc_hc2r_reg_t;
typedef uint16_t hri_sdhc_hcvr_reg_t;
typedef uint16_t hri_sdhc_nisier_reg_t;
typedef uint16_t hri_sdhc_nister_reg_t;
typedef uint16_t hri_sdhc_nistr_reg_t;
typedef uint16_t hri_sdhc_pvr_reg_t;
typedef uint16_t hri_sdhc_sisr_reg_t;
typedef uint16_t hri_sdhc_tmr_reg_t;
typedef uint32_t hri_sdhc_acr_reg_t;
typedef uint32_t hri_sdhc_arg1r_reg_t;
typedef uint32_t hri_sdhc_asar_reg_t;
typedef uint32_t hri_sdhc_bdpr_reg_t;
typedef uint32_t hri_sdhc_ca0r_reg_t;
typedef uint32_t hri_sdhc_ca1r_reg_t;
typedef uint32_t hri_sdhc_cacr_reg_t;
typedef uint32_t hri_sdhc_cc2r_reg_t;
typedef uint32_t hri_sdhc_mccar_reg_t;
typedef uint32_t hri_sdhc_psr_reg_t;
typedef uint32_t hri_sdhc_rr_reg_t;
typedef uint32_t hri_sdhc_ssar_reg_t;
typedef uint8_t  hri_sdhc_aesr_reg_t;
typedef uint8_t  hri_sdhc_bgcr_reg_t;
typedef uint8_t  hri_sdhc_dbgr_reg_t;
typedef uint8_t  hri_sdhc_hc1r_reg_t;
typedef uint8_t  hri_sdhc_mc1r_reg_t;
typedef uint8_t  hri_sdhc_mc2r_reg_t;
typedef uint8_t  hri_sdhc_pcr_reg_t;
typedef uint8_t  hri_sdhc_srr_reg_t;
typedef uint8_t  hri_sdhc_tcr_reg_t;
typedef uint8_t  hri_sdhc_wcr_reg_t;

static inline void hri_sdhc_set_SSAR_CMD23_ARG2_bf(const void *const hw, hri_sdhc_ssar_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->SSAR.reg |= SDHC_SSAR_CMD23_ARG2(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_ssar_reg_t hri_sdhc_get_SSAR_CMD23_ARG2_bf(const void *const hw, hri_sdhc_ssar_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sdhc *)hw)->SSAR.reg;
	tmp = (tmp & SDHC_SSAR_CMD23_ARG2(mask)) >> SDHC_SSAR_CMD23_ARG2_Pos;
	return tmp;
}

static inline void hri_sdhc_write_SSAR_CMD23_ARG2_bf(const void *const hw, hri_sdhc_ssar_reg_t data)
{
	uint32_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->SSAR.reg;
	tmp &= ~SDHC_SSAR_CMD23_ARG2_Msk;
	tmp |= SDHC_SSAR_CMD23_ARG2(data);
	((Sdhc *)hw)->SSAR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_SSAR_CMD23_ARG2_bf(const void *const hw, hri_sdhc_ssar_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->SSAR.reg &= ~SDHC_SSAR_CMD23_ARG2(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_SSAR_CMD23_ARG2_bf(const void *const hw, hri_sdhc_ssar_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->SSAR.reg ^= SDHC_SSAR_CMD23_ARG2(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_ssar_reg_t hri_sdhc_read_SSAR_CMD23_ARG2_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sdhc *)hw)->SSAR.reg;
	tmp = (tmp & SDHC_SSAR_CMD23_ARG2_Msk) >> SDHC_SSAR_CMD23_ARG2_Pos;
	return tmp;
}

static inline void hri_sdhc_set_SSAR_ADDR_bf(const void *const hw, hri_sdhc_ssar_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->SSAR.reg |= SDHC_SSAR_ADDR(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_ssar_reg_t hri_sdhc_get_SSAR_ADDR_bf(const void *const hw, hri_sdhc_ssar_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sdhc *)hw)->SSAR.reg;
	tmp = (tmp & SDHC_SSAR_ADDR(mask)) >> SDHC_SSAR_ADDR_Pos;
	return tmp;
}

static inline void hri_sdhc_write_SSAR_ADDR_bf(const void *const hw, hri_sdhc_ssar_reg_t data)
{
	uint32_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->SSAR.reg;
	tmp &= ~SDHC_SSAR_ADDR_Msk;
	tmp |= SDHC_SSAR_ADDR(data);
	((Sdhc *)hw)->SSAR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_SSAR_ADDR_bf(const void *const hw, hri_sdhc_ssar_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->SSAR.reg &= ~SDHC_SSAR_ADDR(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_SSAR_ADDR_bf(const void *const hw, hri_sdhc_ssar_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->SSAR.reg ^= SDHC_SSAR_ADDR(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_ssar_reg_t hri_sdhc_read_SSAR_ADDR_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sdhc *)hw)->SSAR.reg;
	tmp = (tmp & SDHC_SSAR_ADDR_Msk) >> SDHC_SSAR_ADDR_Pos;
	return tmp;
}

static inline void hri_sdhc_set_SSAR_CMD23_reg(const void *const hw, hri_sdhc_ssar_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->SSAR.reg |= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_ssar_reg_t hri_sdhc_get_SSAR_CMD23_reg(const void *const hw, hri_sdhc_ssar_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sdhc *)hw)->SSAR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sdhc_write_SSAR_CMD23_reg(const void *const hw, hri_sdhc_ssar_reg_t data)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->SSAR.reg = data;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_SSAR_CMD23_reg(const void *const hw, hri_sdhc_ssar_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->SSAR.reg &= ~mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_SSAR_CMD23_reg(const void *const hw, hri_sdhc_ssar_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->SSAR.reg ^= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_ssar_reg_t hri_sdhc_read_SSAR_CMD23_reg(const void *const hw)
{
	return ((Sdhc *)hw)->SSAR.reg;
}

static inline void hri_sdhc_set_SSAR_reg(const void *const hw, hri_sdhc_ssar_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->SSAR.reg |= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_ssar_reg_t hri_sdhc_get_SSAR_reg(const void *const hw, hri_sdhc_ssar_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sdhc *)hw)->SSAR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sdhc_write_SSAR_reg(const void *const hw, hri_sdhc_ssar_reg_t data)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->SSAR.reg = data;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_SSAR_reg(const void *const hw, hri_sdhc_ssar_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->SSAR.reg &= ~mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_SSAR_reg(const void *const hw, hri_sdhc_ssar_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->SSAR.reg ^= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_ssar_reg_t hri_sdhc_read_SSAR_reg(const void *const hw)
{
	return ((Sdhc *)hw)->SSAR.reg;
}

static inline void hri_sdhc_set_HC1R_LEDCTRL_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC1R.reg |= SDHC_HC1R_LEDCTRL;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_HC1R_LEDCTRL_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Sdhc *)hw)->HC1R.reg;
	tmp = (tmp & SDHC_HC1R_LEDCTRL) >> SDHC_HC1R_LEDCTRL_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_HC1R_LEDCTRL_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->HC1R.reg;
	tmp &= ~SDHC_HC1R_LEDCTRL;
	tmp |= value << SDHC_HC1R_LEDCTRL_Pos;
	((Sdhc *)hw)->HC1R.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_HC1R_LEDCTRL_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC1R.reg &= ~SDHC_HC1R_LEDCTRL;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_HC1R_LEDCTRL_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC1R.reg ^= SDHC_HC1R_LEDCTRL;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_HC1R_DW_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC1R.reg |= SDHC_HC1R_DW;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_HC1R_DW_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Sdhc *)hw)->HC1R.reg;
	tmp = (tmp & SDHC_HC1R_DW) >> SDHC_HC1R_DW_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_HC1R_DW_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->HC1R.reg;
	tmp &= ~SDHC_HC1R_DW;
	tmp |= value << SDHC_HC1R_DW_Pos;
	((Sdhc *)hw)->HC1R.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_HC1R_DW_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC1R.reg &= ~SDHC_HC1R_DW;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_HC1R_DW_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC1R.reg ^= SDHC_HC1R_DW;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_HC1R_HSEN_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC1R.reg |= SDHC_HC1R_HSEN;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_HC1R_HSEN_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Sdhc *)hw)->HC1R.reg;
	tmp = (tmp & SDHC_HC1R_HSEN) >> SDHC_HC1R_HSEN_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_HC1R_HSEN_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->HC1R.reg;
	tmp &= ~SDHC_HC1R_HSEN;
	tmp |= value << SDHC_HC1R_HSEN_Pos;
	((Sdhc *)hw)->HC1R.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_HC1R_HSEN_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC1R.reg &= ~SDHC_HC1R_HSEN;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_HC1R_HSEN_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC1R.reg ^= SDHC_HC1R_HSEN;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_HC1R_CARDDTL_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC1R.reg |= SDHC_HC1R_CARDDTL;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_HC1R_CARDDTL_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Sdhc *)hw)->HC1R.reg;
	tmp = (tmp & SDHC_HC1R_CARDDTL) >> SDHC_HC1R_CARDDTL_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_HC1R_CARDDTL_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->HC1R.reg;
	tmp &= ~SDHC_HC1R_CARDDTL;
	tmp |= value << SDHC_HC1R_CARDDTL_Pos;
	((Sdhc *)hw)->HC1R.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_HC1R_CARDDTL_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC1R.reg &= ~SDHC_HC1R_CARDDTL;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_HC1R_CARDDTL_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC1R.reg ^= SDHC_HC1R_CARDDTL;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_HC1R_CARDDSEL_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC1R.reg |= SDHC_HC1R_CARDDSEL;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_HC1R_CARDDSEL_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Sdhc *)hw)->HC1R.reg;
	tmp = (tmp & SDHC_HC1R_CARDDSEL) >> SDHC_HC1R_CARDDSEL_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_HC1R_CARDDSEL_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->HC1R.reg;
	tmp &= ~SDHC_HC1R_CARDDSEL;
	tmp |= value << SDHC_HC1R_CARDDSEL_Pos;
	((Sdhc *)hw)->HC1R.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_HC1R_CARDDSEL_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC1R.reg &= ~SDHC_HC1R_CARDDSEL;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_HC1R_CARDDSEL_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC1R.reg ^= SDHC_HC1R_CARDDSEL;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_HC1R_EMMC_DW_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC1R.reg |= SDHC_HC1R_EMMC_DW;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_HC1R_EMMC_DW_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Sdhc *)hw)->HC1R.reg;
	tmp = (tmp & SDHC_HC1R_EMMC_DW) >> SDHC_HC1R_EMMC_DW_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_HC1R_EMMC_DW_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->HC1R.reg;
	tmp &= ~SDHC_HC1R_EMMC_DW;
	tmp |= value << SDHC_HC1R_EMMC_DW_Pos;
	((Sdhc *)hw)->HC1R.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_HC1R_EMMC_DW_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC1R.reg &= ~SDHC_HC1R_EMMC_DW;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_HC1R_EMMC_DW_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC1R.reg ^= SDHC_HC1R_EMMC_DW;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_HC1R_EMMC_HSEN_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC1R.reg |= SDHC_HC1R_EMMC_HSEN;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_HC1R_EMMC_HSEN_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Sdhc *)hw)->HC1R.reg;
	tmp = (tmp & SDHC_HC1R_EMMC_HSEN) >> SDHC_HC1R_EMMC_HSEN_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_HC1R_EMMC_HSEN_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->HC1R.reg;
	tmp &= ~SDHC_HC1R_EMMC_HSEN;
	tmp |= value << SDHC_HC1R_EMMC_HSEN_Pos;
	((Sdhc *)hw)->HC1R.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_HC1R_EMMC_HSEN_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC1R.reg &= ~SDHC_HC1R_EMMC_HSEN;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_HC1R_EMMC_HSEN_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC1R.reg ^= SDHC_HC1R_EMMC_HSEN;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_HC1R_DMASEL_bf(const void *const hw, hri_sdhc_hc1r_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC1R.reg |= SDHC_HC1R_DMASEL(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_hc1r_reg_t hri_sdhc_get_HC1R_DMASEL_bf(const void *const hw, hri_sdhc_hc1r_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Sdhc *)hw)->HC1R.reg;
	tmp = (tmp & SDHC_HC1R_DMASEL(mask)) >> SDHC_HC1R_DMASEL_Pos;
	return tmp;
}

static inline void hri_sdhc_write_HC1R_DMASEL_bf(const void *const hw, hri_sdhc_hc1r_reg_t data)
{
	uint8_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->HC1R.reg;
	tmp &= ~SDHC_HC1R_DMASEL_Msk;
	tmp |= SDHC_HC1R_DMASEL(data);
	((Sdhc *)hw)->HC1R.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_HC1R_DMASEL_bf(const void *const hw, hri_sdhc_hc1r_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC1R.reg &= ~SDHC_HC1R_DMASEL(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_HC1R_DMASEL_bf(const void *const hw, hri_sdhc_hc1r_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC1R.reg ^= SDHC_HC1R_DMASEL(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_hc1r_reg_t hri_sdhc_read_HC1R_DMASEL_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Sdhc *)hw)->HC1R.reg;
	tmp = (tmp & SDHC_HC1R_DMASEL_Msk) >> SDHC_HC1R_DMASEL_Pos;
	return tmp;
}

static inline void hri_sdhc_set_HC1R_EMMC_DMASEL_bf(const void *const hw, hri_sdhc_hc1r_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC1R.reg |= SDHC_HC1R_EMMC_DMASEL(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_hc1r_reg_t hri_sdhc_get_HC1R_EMMC_DMASEL_bf(const void *const hw, hri_sdhc_hc1r_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Sdhc *)hw)->HC1R.reg;
	tmp = (tmp & SDHC_HC1R_EMMC_DMASEL(mask)) >> SDHC_HC1R_EMMC_DMASEL_Pos;
	return tmp;
}

static inline void hri_sdhc_write_HC1R_EMMC_DMASEL_bf(const void *const hw, hri_sdhc_hc1r_reg_t data)
{
	uint8_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->HC1R.reg;
	tmp &= ~SDHC_HC1R_EMMC_DMASEL_Msk;
	tmp |= SDHC_HC1R_EMMC_DMASEL(data);
	((Sdhc *)hw)->HC1R.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_HC1R_EMMC_DMASEL_bf(const void *const hw, hri_sdhc_hc1r_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC1R.reg &= ~SDHC_HC1R_EMMC_DMASEL(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_HC1R_EMMC_DMASEL_bf(const void *const hw, hri_sdhc_hc1r_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC1R.reg ^= SDHC_HC1R_EMMC_DMASEL(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_hc1r_reg_t hri_sdhc_read_HC1R_EMMC_DMASEL_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Sdhc *)hw)->HC1R.reg;
	tmp = (tmp & SDHC_HC1R_EMMC_DMASEL_Msk) >> SDHC_HC1R_EMMC_DMASEL_Pos;
	return tmp;
}

static inline void hri_sdhc_set_HC1R_reg(const void *const hw, hri_sdhc_hc1r_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC1R.reg |= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_hc1r_reg_t hri_sdhc_get_HC1R_reg(const void *const hw, hri_sdhc_hc1r_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Sdhc *)hw)->HC1R.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sdhc_write_HC1R_reg(const void *const hw, hri_sdhc_hc1r_reg_t data)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC1R.reg = data;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_HC1R_reg(const void *const hw, hri_sdhc_hc1r_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC1R.reg &= ~mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_HC1R_reg(const void *const hw, hri_sdhc_hc1r_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC1R.reg ^= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_hc1r_reg_t hri_sdhc_read_HC1R_reg(const void *const hw)
{
	return ((Sdhc *)hw)->HC1R.reg;
}

static inline void hri_sdhc_set_HC1R_EMMC_reg(const void *const hw, hri_sdhc_hc1r_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC1R.reg |= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_hc1r_reg_t hri_sdhc_get_HC1R_EMMC_reg(const void *const hw, hri_sdhc_hc1r_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Sdhc *)hw)->HC1R.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sdhc_write_HC1R_EMMC_reg(const void *const hw, hri_sdhc_hc1r_reg_t data)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC1R.reg = data;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_HC1R_EMMC_reg(const void *const hw, hri_sdhc_hc1r_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC1R.reg &= ~mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_HC1R_EMMC_reg(const void *const hw, hri_sdhc_hc1r_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC1R.reg ^= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_hc1r_reg_t hri_sdhc_read_HC1R_EMMC_reg(const void *const hw)
{
	return ((Sdhc *)hw)->HC1R.reg;
}

static inline void hri_sdhc_set_BGCR_STPBGR_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->BGCR.reg |= SDHC_BGCR_STPBGR;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_BGCR_STPBGR_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Sdhc *)hw)->BGCR.reg;
	tmp = (tmp & SDHC_BGCR_STPBGR) >> SDHC_BGCR_STPBGR_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_BGCR_STPBGR_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->BGCR.reg;
	tmp &= ~SDHC_BGCR_STPBGR;
	tmp |= value << SDHC_BGCR_STPBGR_Pos;
	((Sdhc *)hw)->BGCR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_BGCR_STPBGR_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->BGCR.reg &= ~SDHC_BGCR_STPBGR;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_BGCR_STPBGR_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->BGCR.reg ^= SDHC_BGCR_STPBGR;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_BGCR_CONTR_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->BGCR.reg |= SDHC_BGCR_CONTR;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_BGCR_CONTR_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Sdhc *)hw)->BGCR.reg;
	tmp = (tmp & SDHC_BGCR_CONTR) >> SDHC_BGCR_CONTR_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_BGCR_CONTR_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->BGCR.reg;
	tmp &= ~SDHC_BGCR_CONTR;
	tmp |= value << SDHC_BGCR_CONTR_Pos;
	((Sdhc *)hw)->BGCR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_BGCR_CONTR_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->BGCR.reg &= ~SDHC_BGCR_CONTR;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_BGCR_CONTR_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->BGCR.reg ^= SDHC_BGCR_CONTR;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_BGCR_RWCTRL_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->BGCR.reg |= SDHC_BGCR_RWCTRL;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_BGCR_RWCTRL_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Sdhc *)hw)->BGCR.reg;
	tmp = (tmp & SDHC_BGCR_RWCTRL) >> SDHC_BGCR_RWCTRL_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_BGCR_RWCTRL_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->BGCR.reg;
	tmp &= ~SDHC_BGCR_RWCTRL;
	tmp |= value << SDHC_BGCR_RWCTRL_Pos;
	((Sdhc *)hw)->BGCR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_BGCR_RWCTRL_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->BGCR.reg &= ~SDHC_BGCR_RWCTRL;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_BGCR_RWCTRL_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->BGCR.reg ^= SDHC_BGCR_RWCTRL;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_BGCR_INTBG_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->BGCR.reg |= SDHC_BGCR_INTBG;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_BGCR_INTBG_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Sdhc *)hw)->BGCR.reg;
	tmp = (tmp & SDHC_BGCR_INTBG) >> SDHC_BGCR_INTBG_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_BGCR_INTBG_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->BGCR.reg;
	tmp &= ~SDHC_BGCR_INTBG;
	tmp |= value << SDHC_BGCR_INTBG_Pos;
	((Sdhc *)hw)->BGCR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_BGCR_INTBG_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->BGCR.reg &= ~SDHC_BGCR_INTBG;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_BGCR_INTBG_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->BGCR.reg ^= SDHC_BGCR_INTBG;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_BGCR_EMMC_STPBGR_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->BGCR.reg |= SDHC_BGCR_EMMC_STPBGR;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_BGCR_EMMC_STPBGR_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Sdhc *)hw)->BGCR.reg;
	tmp = (tmp & SDHC_BGCR_EMMC_STPBGR) >> SDHC_BGCR_EMMC_STPBGR_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_BGCR_EMMC_STPBGR_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->BGCR.reg;
	tmp &= ~SDHC_BGCR_EMMC_STPBGR;
	tmp |= value << SDHC_BGCR_EMMC_STPBGR_Pos;
	((Sdhc *)hw)->BGCR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_BGCR_EMMC_STPBGR_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->BGCR.reg &= ~SDHC_BGCR_EMMC_STPBGR;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_BGCR_EMMC_STPBGR_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->BGCR.reg ^= SDHC_BGCR_EMMC_STPBGR;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_BGCR_EMMC_CONTR_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->BGCR.reg |= SDHC_BGCR_EMMC_CONTR;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_BGCR_EMMC_CONTR_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Sdhc *)hw)->BGCR.reg;
	tmp = (tmp & SDHC_BGCR_EMMC_CONTR) >> SDHC_BGCR_EMMC_CONTR_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_BGCR_EMMC_CONTR_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->BGCR.reg;
	tmp &= ~SDHC_BGCR_EMMC_CONTR;
	tmp |= value << SDHC_BGCR_EMMC_CONTR_Pos;
	((Sdhc *)hw)->BGCR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_BGCR_EMMC_CONTR_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->BGCR.reg &= ~SDHC_BGCR_EMMC_CONTR;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_BGCR_EMMC_CONTR_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->BGCR.reg ^= SDHC_BGCR_EMMC_CONTR;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_BGCR_reg(const void *const hw, hri_sdhc_bgcr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->BGCR.reg |= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_bgcr_reg_t hri_sdhc_get_BGCR_reg(const void *const hw, hri_sdhc_bgcr_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Sdhc *)hw)->BGCR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sdhc_write_BGCR_reg(const void *const hw, hri_sdhc_bgcr_reg_t data)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->BGCR.reg = data;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_BGCR_reg(const void *const hw, hri_sdhc_bgcr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->BGCR.reg &= ~mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_BGCR_reg(const void *const hw, hri_sdhc_bgcr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->BGCR.reg ^= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_bgcr_reg_t hri_sdhc_read_BGCR_reg(const void *const hw)
{
	return ((Sdhc *)hw)->BGCR.reg;
}

static inline void hri_sdhc_set_BGCR_EMMC_reg(const void *const hw, hri_sdhc_bgcr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->BGCR.reg |= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_bgcr_reg_t hri_sdhc_get_BGCR_EMMC_reg(const void *const hw, hri_sdhc_bgcr_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Sdhc *)hw)->BGCR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sdhc_write_BGCR_EMMC_reg(const void *const hw, hri_sdhc_bgcr_reg_t data)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->BGCR.reg = data;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_BGCR_EMMC_reg(const void *const hw, hri_sdhc_bgcr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->BGCR.reg &= ~mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_BGCR_EMMC_reg(const void *const hw, hri_sdhc_bgcr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->BGCR.reg ^= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_bgcr_reg_t hri_sdhc_read_BGCR_EMMC_reg(const void *const hw)
{
	return ((Sdhc *)hw)->BGCR.reg;
}

static inline void hri_sdhc_set_NISTR_CMDC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTR.reg |= SDHC_NISTR_CMDC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_NISTR_CMDC_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->NISTR.reg;
	tmp = (tmp & SDHC_NISTR_CMDC) >> SDHC_NISTR_CMDC_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_NISTR_CMDC_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->NISTR.reg;
	tmp &= ~SDHC_NISTR_CMDC;
	tmp |= value << SDHC_NISTR_CMDC_Pos;
	((Sdhc *)hw)->NISTR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_NISTR_CMDC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTR.reg &= ~SDHC_NISTR_CMDC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_NISTR_CMDC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTR.reg ^= SDHC_NISTR_CMDC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_NISTR_TRFC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTR.reg |= SDHC_NISTR_TRFC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_NISTR_TRFC_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->NISTR.reg;
	tmp = (tmp & SDHC_NISTR_TRFC) >> SDHC_NISTR_TRFC_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_NISTR_TRFC_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->NISTR.reg;
	tmp &= ~SDHC_NISTR_TRFC;
	tmp |= value << SDHC_NISTR_TRFC_Pos;
	((Sdhc *)hw)->NISTR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_NISTR_TRFC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTR.reg &= ~SDHC_NISTR_TRFC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_NISTR_TRFC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTR.reg ^= SDHC_NISTR_TRFC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_NISTR_BLKGE_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTR.reg |= SDHC_NISTR_BLKGE;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_NISTR_BLKGE_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->NISTR.reg;
	tmp = (tmp & SDHC_NISTR_BLKGE) >> SDHC_NISTR_BLKGE_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_NISTR_BLKGE_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->NISTR.reg;
	tmp &= ~SDHC_NISTR_BLKGE;
	tmp |= value << SDHC_NISTR_BLKGE_Pos;
	((Sdhc *)hw)->NISTR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_NISTR_BLKGE_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTR.reg &= ~SDHC_NISTR_BLKGE;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_NISTR_BLKGE_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTR.reg ^= SDHC_NISTR_BLKGE;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_NISTR_DMAINT_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTR.reg |= SDHC_NISTR_DMAINT;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_NISTR_DMAINT_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->NISTR.reg;
	tmp = (tmp & SDHC_NISTR_DMAINT) >> SDHC_NISTR_DMAINT_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_NISTR_DMAINT_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->NISTR.reg;
	tmp &= ~SDHC_NISTR_DMAINT;
	tmp |= value << SDHC_NISTR_DMAINT_Pos;
	((Sdhc *)hw)->NISTR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_NISTR_DMAINT_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTR.reg &= ~SDHC_NISTR_DMAINT;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_NISTR_DMAINT_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTR.reg ^= SDHC_NISTR_DMAINT;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_NISTR_BWRRDY_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTR.reg |= SDHC_NISTR_BWRRDY;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_NISTR_BWRRDY_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->NISTR.reg;
	tmp = (tmp & SDHC_NISTR_BWRRDY) >> SDHC_NISTR_BWRRDY_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_NISTR_BWRRDY_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->NISTR.reg;
	tmp &= ~SDHC_NISTR_BWRRDY;
	tmp |= value << SDHC_NISTR_BWRRDY_Pos;
	((Sdhc *)hw)->NISTR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_NISTR_BWRRDY_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTR.reg &= ~SDHC_NISTR_BWRRDY;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_NISTR_BWRRDY_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTR.reg ^= SDHC_NISTR_BWRRDY;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_NISTR_BRDRDY_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTR.reg |= SDHC_NISTR_BRDRDY;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_NISTR_BRDRDY_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->NISTR.reg;
	tmp = (tmp & SDHC_NISTR_BRDRDY) >> SDHC_NISTR_BRDRDY_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_NISTR_BRDRDY_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->NISTR.reg;
	tmp &= ~SDHC_NISTR_BRDRDY;
	tmp |= value << SDHC_NISTR_BRDRDY_Pos;
	((Sdhc *)hw)->NISTR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_NISTR_BRDRDY_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTR.reg &= ~SDHC_NISTR_BRDRDY;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_NISTR_BRDRDY_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTR.reg ^= SDHC_NISTR_BRDRDY;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_NISTR_CINS_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTR.reg |= SDHC_NISTR_CINS;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_NISTR_CINS_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->NISTR.reg;
	tmp = (tmp & SDHC_NISTR_CINS) >> SDHC_NISTR_CINS_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_NISTR_CINS_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->NISTR.reg;
	tmp &= ~SDHC_NISTR_CINS;
	tmp |= value << SDHC_NISTR_CINS_Pos;
	((Sdhc *)hw)->NISTR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_NISTR_CINS_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTR.reg &= ~SDHC_NISTR_CINS;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_NISTR_CINS_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTR.reg ^= SDHC_NISTR_CINS;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_NISTR_CREM_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTR.reg |= SDHC_NISTR_CREM;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_NISTR_CREM_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->NISTR.reg;
	tmp = (tmp & SDHC_NISTR_CREM) >> SDHC_NISTR_CREM_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_NISTR_CREM_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->NISTR.reg;
	tmp &= ~SDHC_NISTR_CREM;
	tmp |= value << SDHC_NISTR_CREM_Pos;
	((Sdhc *)hw)->NISTR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_NISTR_CREM_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTR.reg &= ~SDHC_NISTR_CREM;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_NISTR_CREM_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTR.reg ^= SDHC_NISTR_CREM;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_NISTR_CINT_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTR.reg |= SDHC_NISTR_CINT;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_NISTR_CINT_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->NISTR.reg;
	tmp = (tmp & SDHC_NISTR_CINT) >> SDHC_NISTR_CINT_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_NISTR_CINT_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->NISTR.reg;
	tmp &= ~SDHC_NISTR_CINT;
	tmp |= value << SDHC_NISTR_CINT_Pos;
	((Sdhc *)hw)->NISTR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_NISTR_CINT_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTR.reg &= ~SDHC_NISTR_CINT;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_NISTR_CINT_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTR.reg ^= SDHC_NISTR_CINT;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_NISTR_ERRINT_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTR.reg |= SDHC_NISTR_ERRINT;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_NISTR_ERRINT_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->NISTR.reg;
	tmp = (tmp & SDHC_NISTR_ERRINT) >> SDHC_NISTR_ERRINT_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_NISTR_ERRINT_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->NISTR.reg;
	tmp &= ~SDHC_NISTR_ERRINT;
	tmp |= value << SDHC_NISTR_ERRINT_Pos;
	((Sdhc *)hw)->NISTR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_NISTR_ERRINT_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTR.reg &= ~SDHC_NISTR_ERRINT;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_NISTR_ERRINT_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTR.reg ^= SDHC_NISTR_ERRINT;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_NISTR_EMMC_CMDC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTR.reg |= SDHC_NISTR_EMMC_CMDC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_NISTR_EMMC_CMDC_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->NISTR.reg;
	tmp = (tmp & SDHC_NISTR_EMMC_CMDC) >> SDHC_NISTR_EMMC_CMDC_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_NISTR_EMMC_CMDC_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->NISTR.reg;
	tmp &= ~SDHC_NISTR_EMMC_CMDC;
	tmp |= value << SDHC_NISTR_EMMC_CMDC_Pos;
	((Sdhc *)hw)->NISTR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_NISTR_EMMC_CMDC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTR.reg &= ~SDHC_NISTR_EMMC_CMDC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_NISTR_EMMC_CMDC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTR.reg ^= SDHC_NISTR_EMMC_CMDC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_NISTR_EMMC_TRFC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTR.reg |= SDHC_NISTR_EMMC_TRFC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_NISTR_EMMC_TRFC_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->NISTR.reg;
	tmp = (tmp & SDHC_NISTR_EMMC_TRFC) >> SDHC_NISTR_EMMC_TRFC_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_NISTR_EMMC_TRFC_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->NISTR.reg;
	tmp &= ~SDHC_NISTR_EMMC_TRFC;
	tmp |= value << SDHC_NISTR_EMMC_TRFC_Pos;
	((Sdhc *)hw)->NISTR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_NISTR_EMMC_TRFC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTR.reg &= ~SDHC_NISTR_EMMC_TRFC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_NISTR_EMMC_TRFC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTR.reg ^= SDHC_NISTR_EMMC_TRFC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_NISTR_EMMC_BLKGE_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTR.reg |= SDHC_NISTR_EMMC_BLKGE;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_NISTR_EMMC_BLKGE_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->NISTR.reg;
	tmp = (tmp & SDHC_NISTR_EMMC_BLKGE) >> SDHC_NISTR_EMMC_BLKGE_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_NISTR_EMMC_BLKGE_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->NISTR.reg;
	tmp &= ~SDHC_NISTR_EMMC_BLKGE;
	tmp |= value << SDHC_NISTR_EMMC_BLKGE_Pos;
	((Sdhc *)hw)->NISTR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_NISTR_EMMC_BLKGE_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTR.reg &= ~SDHC_NISTR_EMMC_BLKGE;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_NISTR_EMMC_BLKGE_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTR.reg ^= SDHC_NISTR_EMMC_BLKGE;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_NISTR_EMMC_DMAINT_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTR.reg |= SDHC_NISTR_EMMC_DMAINT;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_NISTR_EMMC_DMAINT_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->NISTR.reg;
	tmp = (tmp & SDHC_NISTR_EMMC_DMAINT) >> SDHC_NISTR_EMMC_DMAINT_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_NISTR_EMMC_DMAINT_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->NISTR.reg;
	tmp &= ~SDHC_NISTR_EMMC_DMAINT;
	tmp |= value << SDHC_NISTR_EMMC_DMAINT_Pos;
	((Sdhc *)hw)->NISTR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_NISTR_EMMC_DMAINT_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTR.reg &= ~SDHC_NISTR_EMMC_DMAINT;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_NISTR_EMMC_DMAINT_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTR.reg ^= SDHC_NISTR_EMMC_DMAINT;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_NISTR_EMMC_BWRRDY_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTR.reg |= SDHC_NISTR_EMMC_BWRRDY;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_NISTR_EMMC_BWRRDY_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->NISTR.reg;
	tmp = (tmp & SDHC_NISTR_EMMC_BWRRDY) >> SDHC_NISTR_EMMC_BWRRDY_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_NISTR_EMMC_BWRRDY_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->NISTR.reg;
	tmp &= ~SDHC_NISTR_EMMC_BWRRDY;
	tmp |= value << SDHC_NISTR_EMMC_BWRRDY_Pos;
	((Sdhc *)hw)->NISTR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_NISTR_EMMC_BWRRDY_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTR.reg &= ~SDHC_NISTR_EMMC_BWRRDY;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_NISTR_EMMC_BWRRDY_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTR.reg ^= SDHC_NISTR_EMMC_BWRRDY;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_NISTR_EMMC_BRDRDY_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTR.reg |= SDHC_NISTR_EMMC_BRDRDY;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_NISTR_EMMC_BRDRDY_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->NISTR.reg;
	tmp = (tmp & SDHC_NISTR_EMMC_BRDRDY) >> SDHC_NISTR_EMMC_BRDRDY_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_NISTR_EMMC_BRDRDY_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->NISTR.reg;
	tmp &= ~SDHC_NISTR_EMMC_BRDRDY;
	tmp |= value << SDHC_NISTR_EMMC_BRDRDY_Pos;
	((Sdhc *)hw)->NISTR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_NISTR_EMMC_BRDRDY_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTR.reg &= ~SDHC_NISTR_EMMC_BRDRDY;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_NISTR_EMMC_BRDRDY_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTR.reg ^= SDHC_NISTR_EMMC_BRDRDY;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_NISTR_EMMC_BOOTAR_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTR.reg |= SDHC_NISTR_EMMC_BOOTAR;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_NISTR_EMMC_BOOTAR_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->NISTR.reg;
	tmp = (tmp & SDHC_NISTR_EMMC_BOOTAR) >> SDHC_NISTR_EMMC_BOOTAR_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_NISTR_EMMC_BOOTAR_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->NISTR.reg;
	tmp &= ~SDHC_NISTR_EMMC_BOOTAR;
	tmp |= value << SDHC_NISTR_EMMC_BOOTAR_Pos;
	((Sdhc *)hw)->NISTR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_NISTR_EMMC_BOOTAR_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTR.reg &= ~SDHC_NISTR_EMMC_BOOTAR;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_NISTR_EMMC_BOOTAR_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTR.reg ^= SDHC_NISTR_EMMC_BOOTAR;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_NISTR_EMMC_ERRINT_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTR.reg |= SDHC_NISTR_EMMC_ERRINT;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_NISTR_EMMC_ERRINT_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->NISTR.reg;
	tmp = (tmp & SDHC_NISTR_EMMC_ERRINT) >> SDHC_NISTR_EMMC_ERRINT_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_NISTR_EMMC_ERRINT_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->NISTR.reg;
	tmp &= ~SDHC_NISTR_EMMC_ERRINT;
	tmp |= value << SDHC_NISTR_EMMC_ERRINT_Pos;
	((Sdhc *)hw)->NISTR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_NISTR_EMMC_ERRINT_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTR.reg &= ~SDHC_NISTR_EMMC_ERRINT;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_NISTR_EMMC_ERRINT_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTR.reg ^= SDHC_NISTR_EMMC_ERRINT;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_NISTR_reg(const void *const hw, hri_sdhc_nistr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTR.reg |= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_nistr_reg_t hri_sdhc_get_NISTR_reg(const void *const hw, hri_sdhc_nistr_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->NISTR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sdhc_write_NISTR_reg(const void *const hw, hri_sdhc_nistr_reg_t data)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTR.reg = data;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_NISTR_reg(const void *const hw, hri_sdhc_nistr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTR.reg &= ~mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_NISTR_reg(const void *const hw, hri_sdhc_nistr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTR.reg ^= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_nistr_reg_t hri_sdhc_read_NISTR_reg(const void *const hw)
{
	return ((Sdhc *)hw)->NISTR.reg;
}

static inline void hri_sdhc_set_NISTR_EMMC_reg(const void *const hw, hri_sdhc_nistr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTR.reg |= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_nistr_reg_t hri_sdhc_get_NISTR_EMMC_reg(const void *const hw, hri_sdhc_nistr_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->NISTR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sdhc_write_NISTR_EMMC_reg(const void *const hw, hri_sdhc_nistr_reg_t data)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTR.reg = data;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_NISTR_EMMC_reg(const void *const hw, hri_sdhc_nistr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTR.reg &= ~mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_NISTR_EMMC_reg(const void *const hw, hri_sdhc_nistr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTR.reg ^= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_nistr_reg_t hri_sdhc_read_NISTR_EMMC_reg(const void *const hw)
{
	return ((Sdhc *)hw)->NISTR.reg;
}

static inline void hri_sdhc_set_EISTR_CMDTEO_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg |= SDHC_EISTR_CMDTEO;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_EISTR_CMDTEO_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISTR.reg;
	tmp = (tmp & SDHC_EISTR_CMDTEO) >> SDHC_EISTR_CMDTEO_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_EISTR_CMDTEO_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->EISTR.reg;
	tmp &= ~SDHC_EISTR_CMDTEO;
	tmp |= value << SDHC_EISTR_CMDTEO_Pos;
	((Sdhc *)hw)->EISTR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISTR_CMDTEO_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg &= ~SDHC_EISTR_CMDTEO;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISTR_CMDTEO_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg ^= SDHC_EISTR_CMDTEO;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_EISTR_CMDCRC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg |= SDHC_EISTR_CMDCRC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_EISTR_CMDCRC_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISTR.reg;
	tmp = (tmp & SDHC_EISTR_CMDCRC) >> SDHC_EISTR_CMDCRC_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_EISTR_CMDCRC_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->EISTR.reg;
	tmp &= ~SDHC_EISTR_CMDCRC;
	tmp |= value << SDHC_EISTR_CMDCRC_Pos;
	((Sdhc *)hw)->EISTR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISTR_CMDCRC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg &= ~SDHC_EISTR_CMDCRC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISTR_CMDCRC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg ^= SDHC_EISTR_CMDCRC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_EISTR_CMDEND_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg |= SDHC_EISTR_CMDEND;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_EISTR_CMDEND_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISTR.reg;
	tmp = (tmp & SDHC_EISTR_CMDEND) >> SDHC_EISTR_CMDEND_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_EISTR_CMDEND_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->EISTR.reg;
	tmp &= ~SDHC_EISTR_CMDEND;
	tmp |= value << SDHC_EISTR_CMDEND_Pos;
	((Sdhc *)hw)->EISTR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISTR_CMDEND_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg &= ~SDHC_EISTR_CMDEND;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISTR_CMDEND_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg ^= SDHC_EISTR_CMDEND;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_EISTR_CMDIDX_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg |= SDHC_EISTR_CMDIDX;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_EISTR_CMDIDX_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISTR.reg;
	tmp = (tmp & SDHC_EISTR_CMDIDX) >> SDHC_EISTR_CMDIDX_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_EISTR_CMDIDX_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->EISTR.reg;
	tmp &= ~SDHC_EISTR_CMDIDX;
	tmp |= value << SDHC_EISTR_CMDIDX_Pos;
	((Sdhc *)hw)->EISTR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISTR_CMDIDX_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg &= ~SDHC_EISTR_CMDIDX;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISTR_CMDIDX_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg ^= SDHC_EISTR_CMDIDX;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_EISTR_DATTEO_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg |= SDHC_EISTR_DATTEO;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_EISTR_DATTEO_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISTR.reg;
	tmp = (tmp & SDHC_EISTR_DATTEO) >> SDHC_EISTR_DATTEO_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_EISTR_DATTEO_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->EISTR.reg;
	tmp &= ~SDHC_EISTR_DATTEO;
	tmp |= value << SDHC_EISTR_DATTEO_Pos;
	((Sdhc *)hw)->EISTR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISTR_DATTEO_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg &= ~SDHC_EISTR_DATTEO;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISTR_DATTEO_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg ^= SDHC_EISTR_DATTEO;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_EISTR_DATCRC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg |= SDHC_EISTR_DATCRC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_EISTR_DATCRC_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISTR.reg;
	tmp = (tmp & SDHC_EISTR_DATCRC) >> SDHC_EISTR_DATCRC_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_EISTR_DATCRC_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->EISTR.reg;
	tmp &= ~SDHC_EISTR_DATCRC;
	tmp |= value << SDHC_EISTR_DATCRC_Pos;
	((Sdhc *)hw)->EISTR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISTR_DATCRC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg &= ~SDHC_EISTR_DATCRC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISTR_DATCRC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg ^= SDHC_EISTR_DATCRC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_EISTR_DATEND_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg |= SDHC_EISTR_DATEND;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_EISTR_DATEND_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISTR.reg;
	tmp = (tmp & SDHC_EISTR_DATEND) >> SDHC_EISTR_DATEND_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_EISTR_DATEND_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->EISTR.reg;
	tmp &= ~SDHC_EISTR_DATEND;
	tmp |= value << SDHC_EISTR_DATEND_Pos;
	((Sdhc *)hw)->EISTR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISTR_DATEND_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg &= ~SDHC_EISTR_DATEND;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISTR_DATEND_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg ^= SDHC_EISTR_DATEND;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_EISTR_CURLIM_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg |= SDHC_EISTR_CURLIM;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_EISTR_CURLIM_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISTR.reg;
	tmp = (tmp & SDHC_EISTR_CURLIM) >> SDHC_EISTR_CURLIM_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_EISTR_CURLIM_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->EISTR.reg;
	tmp &= ~SDHC_EISTR_CURLIM;
	tmp |= value << SDHC_EISTR_CURLIM_Pos;
	((Sdhc *)hw)->EISTR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISTR_CURLIM_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg &= ~SDHC_EISTR_CURLIM;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISTR_CURLIM_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg ^= SDHC_EISTR_CURLIM;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_EISTR_ACMD_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg |= SDHC_EISTR_ACMD;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_EISTR_ACMD_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISTR.reg;
	tmp = (tmp & SDHC_EISTR_ACMD) >> SDHC_EISTR_ACMD_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_EISTR_ACMD_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->EISTR.reg;
	tmp &= ~SDHC_EISTR_ACMD;
	tmp |= value << SDHC_EISTR_ACMD_Pos;
	((Sdhc *)hw)->EISTR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISTR_ACMD_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg &= ~SDHC_EISTR_ACMD;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISTR_ACMD_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg ^= SDHC_EISTR_ACMD;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_EISTR_ADMA_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg |= SDHC_EISTR_ADMA;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_EISTR_ADMA_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISTR.reg;
	tmp = (tmp & SDHC_EISTR_ADMA) >> SDHC_EISTR_ADMA_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_EISTR_ADMA_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->EISTR.reg;
	tmp &= ~SDHC_EISTR_ADMA;
	tmp |= value << SDHC_EISTR_ADMA_Pos;
	((Sdhc *)hw)->EISTR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISTR_ADMA_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg &= ~SDHC_EISTR_ADMA;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISTR_ADMA_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg ^= SDHC_EISTR_ADMA;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_EISTR_EMMC_CMDTEO_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg |= SDHC_EISTR_EMMC_CMDTEO;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_EISTR_EMMC_CMDTEO_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISTR.reg;
	tmp = (tmp & SDHC_EISTR_EMMC_CMDTEO) >> SDHC_EISTR_EMMC_CMDTEO_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_EISTR_EMMC_CMDTEO_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->EISTR.reg;
	tmp &= ~SDHC_EISTR_EMMC_CMDTEO;
	tmp |= value << SDHC_EISTR_EMMC_CMDTEO_Pos;
	((Sdhc *)hw)->EISTR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISTR_EMMC_CMDTEO_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg &= ~SDHC_EISTR_EMMC_CMDTEO;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISTR_EMMC_CMDTEO_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg ^= SDHC_EISTR_EMMC_CMDTEO;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_EISTR_EMMC_CMDCRC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg |= SDHC_EISTR_EMMC_CMDCRC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_EISTR_EMMC_CMDCRC_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISTR.reg;
	tmp = (tmp & SDHC_EISTR_EMMC_CMDCRC) >> SDHC_EISTR_EMMC_CMDCRC_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_EISTR_EMMC_CMDCRC_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->EISTR.reg;
	tmp &= ~SDHC_EISTR_EMMC_CMDCRC;
	tmp |= value << SDHC_EISTR_EMMC_CMDCRC_Pos;
	((Sdhc *)hw)->EISTR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISTR_EMMC_CMDCRC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg &= ~SDHC_EISTR_EMMC_CMDCRC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISTR_EMMC_CMDCRC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg ^= SDHC_EISTR_EMMC_CMDCRC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_EISTR_EMMC_CMDEND_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg |= SDHC_EISTR_EMMC_CMDEND;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_EISTR_EMMC_CMDEND_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISTR.reg;
	tmp = (tmp & SDHC_EISTR_EMMC_CMDEND) >> SDHC_EISTR_EMMC_CMDEND_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_EISTR_EMMC_CMDEND_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->EISTR.reg;
	tmp &= ~SDHC_EISTR_EMMC_CMDEND;
	tmp |= value << SDHC_EISTR_EMMC_CMDEND_Pos;
	((Sdhc *)hw)->EISTR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISTR_EMMC_CMDEND_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg &= ~SDHC_EISTR_EMMC_CMDEND;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISTR_EMMC_CMDEND_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg ^= SDHC_EISTR_EMMC_CMDEND;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_EISTR_EMMC_CMDIDX_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg |= SDHC_EISTR_EMMC_CMDIDX;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_EISTR_EMMC_CMDIDX_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISTR.reg;
	tmp = (tmp & SDHC_EISTR_EMMC_CMDIDX) >> SDHC_EISTR_EMMC_CMDIDX_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_EISTR_EMMC_CMDIDX_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->EISTR.reg;
	tmp &= ~SDHC_EISTR_EMMC_CMDIDX;
	tmp |= value << SDHC_EISTR_EMMC_CMDIDX_Pos;
	((Sdhc *)hw)->EISTR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISTR_EMMC_CMDIDX_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg &= ~SDHC_EISTR_EMMC_CMDIDX;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISTR_EMMC_CMDIDX_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg ^= SDHC_EISTR_EMMC_CMDIDX;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_EISTR_EMMC_DATTEO_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg |= SDHC_EISTR_EMMC_DATTEO;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_EISTR_EMMC_DATTEO_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISTR.reg;
	tmp = (tmp & SDHC_EISTR_EMMC_DATTEO) >> SDHC_EISTR_EMMC_DATTEO_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_EISTR_EMMC_DATTEO_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->EISTR.reg;
	tmp &= ~SDHC_EISTR_EMMC_DATTEO;
	tmp |= value << SDHC_EISTR_EMMC_DATTEO_Pos;
	((Sdhc *)hw)->EISTR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISTR_EMMC_DATTEO_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg &= ~SDHC_EISTR_EMMC_DATTEO;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISTR_EMMC_DATTEO_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg ^= SDHC_EISTR_EMMC_DATTEO;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_EISTR_EMMC_DATCRC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg |= SDHC_EISTR_EMMC_DATCRC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_EISTR_EMMC_DATCRC_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISTR.reg;
	tmp = (tmp & SDHC_EISTR_EMMC_DATCRC) >> SDHC_EISTR_EMMC_DATCRC_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_EISTR_EMMC_DATCRC_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->EISTR.reg;
	tmp &= ~SDHC_EISTR_EMMC_DATCRC;
	tmp |= value << SDHC_EISTR_EMMC_DATCRC_Pos;
	((Sdhc *)hw)->EISTR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISTR_EMMC_DATCRC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg &= ~SDHC_EISTR_EMMC_DATCRC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISTR_EMMC_DATCRC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg ^= SDHC_EISTR_EMMC_DATCRC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_EISTR_EMMC_DATEND_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg |= SDHC_EISTR_EMMC_DATEND;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_EISTR_EMMC_DATEND_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISTR.reg;
	tmp = (tmp & SDHC_EISTR_EMMC_DATEND) >> SDHC_EISTR_EMMC_DATEND_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_EISTR_EMMC_DATEND_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->EISTR.reg;
	tmp &= ~SDHC_EISTR_EMMC_DATEND;
	tmp |= value << SDHC_EISTR_EMMC_DATEND_Pos;
	((Sdhc *)hw)->EISTR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISTR_EMMC_DATEND_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg &= ~SDHC_EISTR_EMMC_DATEND;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISTR_EMMC_DATEND_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg ^= SDHC_EISTR_EMMC_DATEND;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_EISTR_EMMC_CURLIM_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg |= SDHC_EISTR_EMMC_CURLIM;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_EISTR_EMMC_CURLIM_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISTR.reg;
	tmp = (tmp & SDHC_EISTR_EMMC_CURLIM) >> SDHC_EISTR_EMMC_CURLIM_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_EISTR_EMMC_CURLIM_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->EISTR.reg;
	tmp &= ~SDHC_EISTR_EMMC_CURLIM;
	tmp |= value << SDHC_EISTR_EMMC_CURLIM_Pos;
	((Sdhc *)hw)->EISTR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISTR_EMMC_CURLIM_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg &= ~SDHC_EISTR_EMMC_CURLIM;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISTR_EMMC_CURLIM_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg ^= SDHC_EISTR_EMMC_CURLIM;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_EISTR_EMMC_ACMD_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg |= SDHC_EISTR_EMMC_ACMD;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_EISTR_EMMC_ACMD_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISTR.reg;
	tmp = (tmp & SDHC_EISTR_EMMC_ACMD) >> SDHC_EISTR_EMMC_ACMD_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_EISTR_EMMC_ACMD_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->EISTR.reg;
	tmp &= ~SDHC_EISTR_EMMC_ACMD;
	tmp |= value << SDHC_EISTR_EMMC_ACMD_Pos;
	((Sdhc *)hw)->EISTR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISTR_EMMC_ACMD_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg &= ~SDHC_EISTR_EMMC_ACMD;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISTR_EMMC_ACMD_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg ^= SDHC_EISTR_EMMC_ACMD;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_EISTR_EMMC_ADMA_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg |= SDHC_EISTR_EMMC_ADMA;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_EISTR_EMMC_ADMA_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISTR.reg;
	tmp = (tmp & SDHC_EISTR_EMMC_ADMA) >> SDHC_EISTR_EMMC_ADMA_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_EISTR_EMMC_ADMA_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->EISTR.reg;
	tmp &= ~SDHC_EISTR_EMMC_ADMA;
	tmp |= value << SDHC_EISTR_EMMC_ADMA_Pos;
	((Sdhc *)hw)->EISTR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISTR_EMMC_ADMA_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg &= ~SDHC_EISTR_EMMC_ADMA;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISTR_EMMC_ADMA_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg ^= SDHC_EISTR_EMMC_ADMA;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_EISTR_EMMC_BOOTAE_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg |= SDHC_EISTR_EMMC_BOOTAE;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_EISTR_EMMC_BOOTAE_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISTR.reg;
	tmp = (tmp & SDHC_EISTR_EMMC_BOOTAE) >> SDHC_EISTR_EMMC_BOOTAE_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_EISTR_EMMC_BOOTAE_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->EISTR.reg;
	tmp &= ~SDHC_EISTR_EMMC_BOOTAE;
	tmp |= value << SDHC_EISTR_EMMC_BOOTAE_Pos;
	((Sdhc *)hw)->EISTR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISTR_EMMC_BOOTAE_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg &= ~SDHC_EISTR_EMMC_BOOTAE;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISTR_EMMC_BOOTAE_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg ^= SDHC_EISTR_EMMC_BOOTAE;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_EISTR_reg(const void *const hw, hri_sdhc_eistr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg |= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_eistr_reg_t hri_sdhc_get_EISTR_reg(const void *const hw, hri_sdhc_eistr_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISTR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sdhc_write_EISTR_reg(const void *const hw, hri_sdhc_eistr_reg_t data)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg = data;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISTR_reg(const void *const hw, hri_sdhc_eistr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg &= ~mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISTR_reg(const void *const hw, hri_sdhc_eistr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg ^= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_eistr_reg_t hri_sdhc_read_EISTR_reg(const void *const hw)
{
	return ((Sdhc *)hw)->EISTR.reg;
}

static inline void hri_sdhc_set_EISTR_EMMC_reg(const void *const hw, hri_sdhc_eistr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg |= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_eistr_reg_t hri_sdhc_get_EISTR_EMMC_reg(const void *const hw, hri_sdhc_eistr_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISTR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sdhc_write_EISTR_EMMC_reg(const void *const hw, hri_sdhc_eistr_reg_t data)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg = data;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISTR_EMMC_reg(const void *const hw, hri_sdhc_eistr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg &= ~mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISTR_EMMC_reg(const void *const hw, hri_sdhc_eistr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTR.reg ^= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_eistr_reg_t hri_sdhc_read_EISTR_EMMC_reg(const void *const hw)
{
	return ((Sdhc *)hw)->EISTR.reg;
}

static inline void hri_sdhc_set_NISTER_CMDC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTER.reg |= SDHC_NISTER_CMDC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_NISTER_CMDC_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->NISTER.reg;
	tmp = (tmp & SDHC_NISTER_CMDC) >> SDHC_NISTER_CMDC_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_NISTER_CMDC_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->NISTER.reg;
	tmp &= ~SDHC_NISTER_CMDC;
	tmp |= value << SDHC_NISTER_CMDC_Pos;
	((Sdhc *)hw)->NISTER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_NISTER_CMDC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTER.reg &= ~SDHC_NISTER_CMDC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_NISTER_CMDC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTER.reg ^= SDHC_NISTER_CMDC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_NISTER_TRFC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTER.reg |= SDHC_NISTER_TRFC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_NISTER_TRFC_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->NISTER.reg;
	tmp = (tmp & SDHC_NISTER_TRFC) >> SDHC_NISTER_TRFC_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_NISTER_TRFC_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->NISTER.reg;
	tmp &= ~SDHC_NISTER_TRFC;
	tmp |= value << SDHC_NISTER_TRFC_Pos;
	((Sdhc *)hw)->NISTER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_NISTER_TRFC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTER.reg &= ~SDHC_NISTER_TRFC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_NISTER_TRFC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTER.reg ^= SDHC_NISTER_TRFC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_NISTER_BLKGE_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTER.reg |= SDHC_NISTER_BLKGE;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_NISTER_BLKGE_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->NISTER.reg;
	tmp = (tmp & SDHC_NISTER_BLKGE) >> SDHC_NISTER_BLKGE_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_NISTER_BLKGE_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->NISTER.reg;
	tmp &= ~SDHC_NISTER_BLKGE;
	tmp |= value << SDHC_NISTER_BLKGE_Pos;
	((Sdhc *)hw)->NISTER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_NISTER_BLKGE_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTER.reg &= ~SDHC_NISTER_BLKGE;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_NISTER_BLKGE_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTER.reg ^= SDHC_NISTER_BLKGE;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_NISTER_DMAINT_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTER.reg |= SDHC_NISTER_DMAINT;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_NISTER_DMAINT_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->NISTER.reg;
	tmp = (tmp & SDHC_NISTER_DMAINT) >> SDHC_NISTER_DMAINT_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_NISTER_DMAINT_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->NISTER.reg;
	tmp &= ~SDHC_NISTER_DMAINT;
	tmp |= value << SDHC_NISTER_DMAINT_Pos;
	((Sdhc *)hw)->NISTER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_NISTER_DMAINT_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTER.reg &= ~SDHC_NISTER_DMAINT;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_NISTER_DMAINT_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTER.reg ^= SDHC_NISTER_DMAINT;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_NISTER_BWRRDY_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTER.reg |= SDHC_NISTER_BWRRDY;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_NISTER_BWRRDY_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->NISTER.reg;
	tmp = (tmp & SDHC_NISTER_BWRRDY) >> SDHC_NISTER_BWRRDY_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_NISTER_BWRRDY_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->NISTER.reg;
	tmp &= ~SDHC_NISTER_BWRRDY;
	tmp |= value << SDHC_NISTER_BWRRDY_Pos;
	((Sdhc *)hw)->NISTER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_NISTER_BWRRDY_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTER.reg &= ~SDHC_NISTER_BWRRDY;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_NISTER_BWRRDY_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTER.reg ^= SDHC_NISTER_BWRRDY;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_NISTER_BRDRDY_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTER.reg |= SDHC_NISTER_BRDRDY;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_NISTER_BRDRDY_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->NISTER.reg;
	tmp = (tmp & SDHC_NISTER_BRDRDY) >> SDHC_NISTER_BRDRDY_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_NISTER_BRDRDY_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->NISTER.reg;
	tmp &= ~SDHC_NISTER_BRDRDY;
	tmp |= value << SDHC_NISTER_BRDRDY_Pos;
	((Sdhc *)hw)->NISTER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_NISTER_BRDRDY_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTER.reg &= ~SDHC_NISTER_BRDRDY;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_NISTER_BRDRDY_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTER.reg ^= SDHC_NISTER_BRDRDY;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_NISTER_CINS_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTER.reg |= SDHC_NISTER_CINS;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_NISTER_CINS_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->NISTER.reg;
	tmp = (tmp & SDHC_NISTER_CINS) >> SDHC_NISTER_CINS_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_NISTER_CINS_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->NISTER.reg;
	tmp &= ~SDHC_NISTER_CINS;
	tmp |= value << SDHC_NISTER_CINS_Pos;
	((Sdhc *)hw)->NISTER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_NISTER_CINS_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTER.reg &= ~SDHC_NISTER_CINS;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_NISTER_CINS_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTER.reg ^= SDHC_NISTER_CINS;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_NISTER_CREM_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTER.reg |= SDHC_NISTER_CREM;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_NISTER_CREM_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->NISTER.reg;
	tmp = (tmp & SDHC_NISTER_CREM) >> SDHC_NISTER_CREM_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_NISTER_CREM_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->NISTER.reg;
	tmp &= ~SDHC_NISTER_CREM;
	tmp |= value << SDHC_NISTER_CREM_Pos;
	((Sdhc *)hw)->NISTER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_NISTER_CREM_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTER.reg &= ~SDHC_NISTER_CREM;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_NISTER_CREM_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTER.reg ^= SDHC_NISTER_CREM;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_NISTER_CINT_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTER.reg |= SDHC_NISTER_CINT;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_NISTER_CINT_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->NISTER.reg;
	tmp = (tmp & SDHC_NISTER_CINT) >> SDHC_NISTER_CINT_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_NISTER_CINT_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->NISTER.reg;
	tmp &= ~SDHC_NISTER_CINT;
	tmp |= value << SDHC_NISTER_CINT_Pos;
	((Sdhc *)hw)->NISTER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_NISTER_CINT_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTER.reg &= ~SDHC_NISTER_CINT;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_NISTER_CINT_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTER.reg ^= SDHC_NISTER_CINT;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_NISTER_EMMC_CMDC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTER.reg |= SDHC_NISTER_EMMC_CMDC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_NISTER_EMMC_CMDC_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->NISTER.reg;
	tmp = (tmp & SDHC_NISTER_EMMC_CMDC) >> SDHC_NISTER_EMMC_CMDC_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_NISTER_EMMC_CMDC_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->NISTER.reg;
	tmp &= ~SDHC_NISTER_EMMC_CMDC;
	tmp |= value << SDHC_NISTER_EMMC_CMDC_Pos;
	((Sdhc *)hw)->NISTER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_NISTER_EMMC_CMDC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTER.reg &= ~SDHC_NISTER_EMMC_CMDC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_NISTER_EMMC_CMDC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTER.reg ^= SDHC_NISTER_EMMC_CMDC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_NISTER_EMMC_TRFC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTER.reg |= SDHC_NISTER_EMMC_TRFC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_NISTER_EMMC_TRFC_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->NISTER.reg;
	tmp = (tmp & SDHC_NISTER_EMMC_TRFC) >> SDHC_NISTER_EMMC_TRFC_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_NISTER_EMMC_TRFC_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->NISTER.reg;
	tmp &= ~SDHC_NISTER_EMMC_TRFC;
	tmp |= value << SDHC_NISTER_EMMC_TRFC_Pos;
	((Sdhc *)hw)->NISTER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_NISTER_EMMC_TRFC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTER.reg &= ~SDHC_NISTER_EMMC_TRFC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_NISTER_EMMC_TRFC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTER.reg ^= SDHC_NISTER_EMMC_TRFC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_NISTER_EMMC_BLKGE_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTER.reg |= SDHC_NISTER_EMMC_BLKGE;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_NISTER_EMMC_BLKGE_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->NISTER.reg;
	tmp = (tmp & SDHC_NISTER_EMMC_BLKGE) >> SDHC_NISTER_EMMC_BLKGE_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_NISTER_EMMC_BLKGE_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->NISTER.reg;
	tmp &= ~SDHC_NISTER_EMMC_BLKGE;
	tmp |= value << SDHC_NISTER_EMMC_BLKGE_Pos;
	((Sdhc *)hw)->NISTER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_NISTER_EMMC_BLKGE_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTER.reg &= ~SDHC_NISTER_EMMC_BLKGE;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_NISTER_EMMC_BLKGE_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTER.reg ^= SDHC_NISTER_EMMC_BLKGE;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_NISTER_EMMC_DMAINT_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTER.reg |= SDHC_NISTER_EMMC_DMAINT;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_NISTER_EMMC_DMAINT_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->NISTER.reg;
	tmp = (tmp & SDHC_NISTER_EMMC_DMAINT) >> SDHC_NISTER_EMMC_DMAINT_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_NISTER_EMMC_DMAINT_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->NISTER.reg;
	tmp &= ~SDHC_NISTER_EMMC_DMAINT;
	tmp |= value << SDHC_NISTER_EMMC_DMAINT_Pos;
	((Sdhc *)hw)->NISTER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_NISTER_EMMC_DMAINT_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTER.reg &= ~SDHC_NISTER_EMMC_DMAINT;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_NISTER_EMMC_DMAINT_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTER.reg ^= SDHC_NISTER_EMMC_DMAINT;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_NISTER_EMMC_BWRRDY_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTER.reg |= SDHC_NISTER_EMMC_BWRRDY;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_NISTER_EMMC_BWRRDY_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->NISTER.reg;
	tmp = (tmp & SDHC_NISTER_EMMC_BWRRDY) >> SDHC_NISTER_EMMC_BWRRDY_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_NISTER_EMMC_BWRRDY_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->NISTER.reg;
	tmp &= ~SDHC_NISTER_EMMC_BWRRDY;
	tmp |= value << SDHC_NISTER_EMMC_BWRRDY_Pos;
	((Sdhc *)hw)->NISTER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_NISTER_EMMC_BWRRDY_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTER.reg &= ~SDHC_NISTER_EMMC_BWRRDY;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_NISTER_EMMC_BWRRDY_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTER.reg ^= SDHC_NISTER_EMMC_BWRRDY;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_NISTER_EMMC_BRDRDY_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTER.reg |= SDHC_NISTER_EMMC_BRDRDY;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_NISTER_EMMC_BRDRDY_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->NISTER.reg;
	tmp = (tmp & SDHC_NISTER_EMMC_BRDRDY) >> SDHC_NISTER_EMMC_BRDRDY_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_NISTER_EMMC_BRDRDY_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->NISTER.reg;
	tmp &= ~SDHC_NISTER_EMMC_BRDRDY;
	tmp |= value << SDHC_NISTER_EMMC_BRDRDY_Pos;
	((Sdhc *)hw)->NISTER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_NISTER_EMMC_BRDRDY_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTER.reg &= ~SDHC_NISTER_EMMC_BRDRDY;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_NISTER_EMMC_BRDRDY_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTER.reg ^= SDHC_NISTER_EMMC_BRDRDY;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_NISTER_EMMC_BOOTAR_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTER.reg |= SDHC_NISTER_EMMC_BOOTAR;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_NISTER_EMMC_BOOTAR_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->NISTER.reg;
	tmp = (tmp & SDHC_NISTER_EMMC_BOOTAR) >> SDHC_NISTER_EMMC_BOOTAR_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_NISTER_EMMC_BOOTAR_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->NISTER.reg;
	tmp &= ~SDHC_NISTER_EMMC_BOOTAR;
	tmp |= value << SDHC_NISTER_EMMC_BOOTAR_Pos;
	((Sdhc *)hw)->NISTER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_NISTER_EMMC_BOOTAR_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTER.reg &= ~SDHC_NISTER_EMMC_BOOTAR;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_NISTER_EMMC_BOOTAR_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTER.reg ^= SDHC_NISTER_EMMC_BOOTAR;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_NISTER_reg(const void *const hw, hri_sdhc_nister_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTER.reg |= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_nister_reg_t hri_sdhc_get_NISTER_reg(const void *const hw, hri_sdhc_nister_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->NISTER.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sdhc_write_NISTER_reg(const void *const hw, hri_sdhc_nister_reg_t data)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTER.reg = data;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_NISTER_reg(const void *const hw, hri_sdhc_nister_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTER.reg &= ~mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_NISTER_reg(const void *const hw, hri_sdhc_nister_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTER.reg ^= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_nister_reg_t hri_sdhc_read_NISTER_reg(const void *const hw)
{
	return ((Sdhc *)hw)->NISTER.reg;
}

static inline void hri_sdhc_set_NISTER_EMMC_reg(const void *const hw, hri_sdhc_nister_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTER.reg |= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_nister_reg_t hri_sdhc_get_NISTER_EMMC_reg(const void *const hw, hri_sdhc_nister_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->NISTER.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sdhc_write_NISTER_EMMC_reg(const void *const hw, hri_sdhc_nister_reg_t data)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTER.reg = data;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_NISTER_EMMC_reg(const void *const hw, hri_sdhc_nister_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTER.reg &= ~mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_NISTER_EMMC_reg(const void *const hw, hri_sdhc_nister_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISTER.reg ^= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_nister_reg_t hri_sdhc_read_NISTER_EMMC_reg(const void *const hw)
{
	return ((Sdhc *)hw)->NISTER.reg;
}

static inline void hri_sdhc_set_EISTER_CMDTEO_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg |= SDHC_EISTER_CMDTEO;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_EISTER_CMDTEO_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISTER.reg;
	tmp = (tmp & SDHC_EISTER_CMDTEO) >> SDHC_EISTER_CMDTEO_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_EISTER_CMDTEO_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->EISTER.reg;
	tmp &= ~SDHC_EISTER_CMDTEO;
	tmp |= value << SDHC_EISTER_CMDTEO_Pos;
	((Sdhc *)hw)->EISTER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISTER_CMDTEO_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg &= ~SDHC_EISTER_CMDTEO;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISTER_CMDTEO_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg ^= SDHC_EISTER_CMDTEO;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_EISTER_CMDCRC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg |= SDHC_EISTER_CMDCRC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_EISTER_CMDCRC_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISTER.reg;
	tmp = (tmp & SDHC_EISTER_CMDCRC) >> SDHC_EISTER_CMDCRC_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_EISTER_CMDCRC_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->EISTER.reg;
	tmp &= ~SDHC_EISTER_CMDCRC;
	tmp |= value << SDHC_EISTER_CMDCRC_Pos;
	((Sdhc *)hw)->EISTER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISTER_CMDCRC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg &= ~SDHC_EISTER_CMDCRC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISTER_CMDCRC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg ^= SDHC_EISTER_CMDCRC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_EISTER_CMDEND_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg |= SDHC_EISTER_CMDEND;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_EISTER_CMDEND_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISTER.reg;
	tmp = (tmp & SDHC_EISTER_CMDEND) >> SDHC_EISTER_CMDEND_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_EISTER_CMDEND_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->EISTER.reg;
	tmp &= ~SDHC_EISTER_CMDEND;
	tmp |= value << SDHC_EISTER_CMDEND_Pos;
	((Sdhc *)hw)->EISTER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISTER_CMDEND_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg &= ~SDHC_EISTER_CMDEND;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISTER_CMDEND_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg ^= SDHC_EISTER_CMDEND;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_EISTER_CMDIDX_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg |= SDHC_EISTER_CMDIDX;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_EISTER_CMDIDX_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISTER.reg;
	tmp = (tmp & SDHC_EISTER_CMDIDX) >> SDHC_EISTER_CMDIDX_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_EISTER_CMDIDX_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->EISTER.reg;
	tmp &= ~SDHC_EISTER_CMDIDX;
	tmp |= value << SDHC_EISTER_CMDIDX_Pos;
	((Sdhc *)hw)->EISTER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISTER_CMDIDX_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg &= ~SDHC_EISTER_CMDIDX;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISTER_CMDIDX_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg ^= SDHC_EISTER_CMDIDX;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_EISTER_DATTEO_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg |= SDHC_EISTER_DATTEO;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_EISTER_DATTEO_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISTER.reg;
	tmp = (tmp & SDHC_EISTER_DATTEO) >> SDHC_EISTER_DATTEO_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_EISTER_DATTEO_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->EISTER.reg;
	tmp &= ~SDHC_EISTER_DATTEO;
	tmp |= value << SDHC_EISTER_DATTEO_Pos;
	((Sdhc *)hw)->EISTER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISTER_DATTEO_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg &= ~SDHC_EISTER_DATTEO;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISTER_DATTEO_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg ^= SDHC_EISTER_DATTEO;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_EISTER_DATCRC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg |= SDHC_EISTER_DATCRC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_EISTER_DATCRC_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISTER.reg;
	tmp = (tmp & SDHC_EISTER_DATCRC) >> SDHC_EISTER_DATCRC_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_EISTER_DATCRC_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->EISTER.reg;
	tmp &= ~SDHC_EISTER_DATCRC;
	tmp |= value << SDHC_EISTER_DATCRC_Pos;
	((Sdhc *)hw)->EISTER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISTER_DATCRC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg &= ~SDHC_EISTER_DATCRC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISTER_DATCRC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg ^= SDHC_EISTER_DATCRC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_EISTER_DATEND_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg |= SDHC_EISTER_DATEND;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_EISTER_DATEND_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISTER.reg;
	tmp = (tmp & SDHC_EISTER_DATEND) >> SDHC_EISTER_DATEND_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_EISTER_DATEND_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->EISTER.reg;
	tmp &= ~SDHC_EISTER_DATEND;
	tmp |= value << SDHC_EISTER_DATEND_Pos;
	((Sdhc *)hw)->EISTER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISTER_DATEND_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg &= ~SDHC_EISTER_DATEND;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISTER_DATEND_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg ^= SDHC_EISTER_DATEND;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_EISTER_CURLIM_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg |= SDHC_EISTER_CURLIM;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_EISTER_CURLIM_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISTER.reg;
	tmp = (tmp & SDHC_EISTER_CURLIM) >> SDHC_EISTER_CURLIM_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_EISTER_CURLIM_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->EISTER.reg;
	tmp &= ~SDHC_EISTER_CURLIM;
	tmp |= value << SDHC_EISTER_CURLIM_Pos;
	((Sdhc *)hw)->EISTER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISTER_CURLIM_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg &= ~SDHC_EISTER_CURLIM;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISTER_CURLIM_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg ^= SDHC_EISTER_CURLIM;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_EISTER_ACMD_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg |= SDHC_EISTER_ACMD;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_EISTER_ACMD_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISTER.reg;
	tmp = (tmp & SDHC_EISTER_ACMD) >> SDHC_EISTER_ACMD_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_EISTER_ACMD_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->EISTER.reg;
	tmp &= ~SDHC_EISTER_ACMD;
	tmp |= value << SDHC_EISTER_ACMD_Pos;
	((Sdhc *)hw)->EISTER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISTER_ACMD_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg &= ~SDHC_EISTER_ACMD;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISTER_ACMD_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg ^= SDHC_EISTER_ACMD;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_EISTER_ADMA_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg |= SDHC_EISTER_ADMA;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_EISTER_ADMA_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISTER.reg;
	tmp = (tmp & SDHC_EISTER_ADMA) >> SDHC_EISTER_ADMA_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_EISTER_ADMA_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->EISTER.reg;
	tmp &= ~SDHC_EISTER_ADMA;
	tmp |= value << SDHC_EISTER_ADMA_Pos;
	((Sdhc *)hw)->EISTER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISTER_ADMA_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg &= ~SDHC_EISTER_ADMA;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISTER_ADMA_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg ^= SDHC_EISTER_ADMA;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_EISTER_EMMC_CMDTEO_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg |= SDHC_EISTER_EMMC_CMDTEO;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_EISTER_EMMC_CMDTEO_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISTER.reg;
	tmp = (tmp & SDHC_EISTER_EMMC_CMDTEO) >> SDHC_EISTER_EMMC_CMDTEO_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_EISTER_EMMC_CMDTEO_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->EISTER.reg;
	tmp &= ~SDHC_EISTER_EMMC_CMDTEO;
	tmp |= value << SDHC_EISTER_EMMC_CMDTEO_Pos;
	((Sdhc *)hw)->EISTER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISTER_EMMC_CMDTEO_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg &= ~SDHC_EISTER_EMMC_CMDTEO;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISTER_EMMC_CMDTEO_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg ^= SDHC_EISTER_EMMC_CMDTEO;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_EISTER_EMMC_CMDCRC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg |= SDHC_EISTER_EMMC_CMDCRC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_EISTER_EMMC_CMDCRC_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISTER.reg;
	tmp = (tmp & SDHC_EISTER_EMMC_CMDCRC) >> SDHC_EISTER_EMMC_CMDCRC_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_EISTER_EMMC_CMDCRC_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->EISTER.reg;
	tmp &= ~SDHC_EISTER_EMMC_CMDCRC;
	tmp |= value << SDHC_EISTER_EMMC_CMDCRC_Pos;
	((Sdhc *)hw)->EISTER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISTER_EMMC_CMDCRC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg &= ~SDHC_EISTER_EMMC_CMDCRC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISTER_EMMC_CMDCRC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg ^= SDHC_EISTER_EMMC_CMDCRC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_EISTER_EMMC_CMDEND_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg |= SDHC_EISTER_EMMC_CMDEND;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_EISTER_EMMC_CMDEND_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISTER.reg;
	tmp = (tmp & SDHC_EISTER_EMMC_CMDEND) >> SDHC_EISTER_EMMC_CMDEND_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_EISTER_EMMC_CMDEND_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->EISTER.reg;
	tmp &= ~SDHC_EISTER_EMMC_CMDEND;
	tmp |= value << SDHC_EISTER_EMMC_CMDEND_Pos;
	((Sdhc *)hw)->EISTER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISTER_EMMC_CMDEND_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg &= ~SDHC_EISTER_EMMC_CMDEND;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISTER_EMMC_CMDEND_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg ^= SDHC_EISTER_EMMC_CMDEND;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_EISTER_EMMC_CMDIDX_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg |= SDHC_EISTER_EMMC_CMDIDX;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_EISTER_EMMC_CMDIDX_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISTER.reg;
	tmp = (tmp & SDHC_EISTER_EMMC_CMDIDX) >> SDHC_EISTER_EMMC_CMDIDX_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_EISTER_EMMC_CMDIDX_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->EISTER.reg;
	tmp &= ~SDHC_EISTER_EMMC_CMDIDX;
	tmp |= value << SDHC_EISTER_EMMC_CMDIDX_Pos;
	((Sdhc *)hw)->EISTER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISTER_EMMC_CMDIDX_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg &= ~SDHC_EISTER_EMMC_CMDIDX;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISTER_EMMC_CMDIDX_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg ^= SDHC_EISTER_EMMC_CMDIDX;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_EISTER_EMMC_DATTEO_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg |= SDHC_EISTER_EMMC_DATTEO;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_EISTER_EMMC_DATTEO_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISTER.reg;
	tmp = (tmp & SDHC_EISTER_EMMC_DATTEO) >> SDHC_EISTER_EMMC_DATTEO_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_EISTER_EMMC_DATTEO_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->EISTER.reg;
	tmp &= ~SDHC_EISTER_EMMC_DATTEO;
	tmp |= value << SDHC_EISTER_EMMC_DATTEO_Pos;
	((Sdhc *)hw)->EISTER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISTER_EMMC_DATTEO_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg &= ~SDHC_EISTER_EMMC_DATTEO;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISTER_EMMC_DATTEO_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg ^= SDHC_EISTER_EMMC_DATTEO;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_EISTER_EMMC_DATCRC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg |= SDHC_EISTER_EMMC_DATCRC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_EISTER_EMMC_DATCRC_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISTER.reg;
	tmp = (tmp & SDHC_EISTER_EMMC_DATCRC) >> SDHC_EISTER_EMMC_DATCRC_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_EISTER_EMMC_DATCRC_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->EISTER.reg;
	tmp &= ~SDHC_EISTER_EMMC_DATCRC;
	tmp |= value << SDHC_EISTER_EMMC_DATCRC_Pos;
	((Sdhc *)hw)->EISTER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISTER_EMMC_DATCRC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg &= ~SDHC_EISTER_EMMC_DATCRC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISTER_EMMC_DATCRC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg ^= SDHC_EISTER_EMMC_DATCRC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_EISTER_EMMC_DATEND_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg |= SDHC_EISTER_EMMC_DATEND;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_EISTER_EMMC_DATEND_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISTER.reg;
	tmp = (tmp & SDHC_EISTER_EMMC_DATEND) >> SDHC_EISTER_EMMC_DATEND_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_EISTER_EMMC_DATEND_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->EISTER.reg;
	tmp &= ~SDHC_EISTER_EMMC_DATEND;
	tmp |= value << SDHC_EISTER_EMMC_DATEND_Pos;
	((Sdhc *)hw)->EISTER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISTER_EMMC_DATEND_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg &= ~SDHC_EISTER_EMMC_DATEND;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISTER_EMMC_DATEND_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg ^= SDHC_EISTER_EMMC_DATEND;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_EISTER_EMMC_CURLIM_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg |= SDHC_EISTER_EMMC_CURLIM;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_EISTER_EMMC_CURLIM_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISTER.reg;
	tmp = (tmp & SDHC_EISTER_EMMC_CURLIM) >> SDHC_EISTER_EMMC_CURLIM_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_EISTER_EMMC_CURLIM_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->EISTER.reg;
	tmp &= ~SDHC_EISTER_EMMC_CURLIM;
	tmp |= value << SDHC_EISTER_EMMC_CURLIM_Pos;
	((Sdhc *)hw)->EISTER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISTER_EMMC_CURLIM_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg &= ~SDHC_EISTER_EMMC_CURLIM;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISTER_EMMC_CURLIM_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg ^= SDHC_EISTER_EMMC_CURLIM;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_EISTER_EMMC_ACMD_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg |= SDHC_EISTER_EMMC_ACMD;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_EISTER_EMMC_ACMD_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISTER.reg;
	tmp = (tmp & SDHC_EISTER_EMMC_ACMD) >> SDHC_EISTER_EMMC_ACMD_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_EISTER_EMMC_ACMD_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->EISTER.reg;
	tmp &= ~SDHC_EISTER_EMMC_ACMD;
	tmp |= value << SDHC_EISTER_EMMC_ACMD_Pos;
	((Sdhc *)hw)->EISTER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISTER_EMMC_ACMD_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg &= ~SDHC_EISTER_EMMC_ACMD;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISTER_EMMC_ACMD_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg ^= SDHC_EISTER_EMMC_ACMD;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_EISTER_EMMC_ADMA_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg |= SDHC_EISTER_EMMC_ADMA;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_EISTER_EMMC_ADMA_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISTER.reg;
	tmp = (tmp & SDHC_EISTER_EMMC_ADMA) >> SDHC_EISTER_EMMC_ADMA_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_EISTER_EMMC_ADMA_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->EISTER.reg;
	tmp &= ~SDHC_EISTER_EMMC_ADMA;
	tmp |= value << SDHC_EISTER_EMMC_ADMA_Pos;
	((Sdhc *)hw)->EISTER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISTER_EMMC_ADMA_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg &= ~SDHC_EISTER_EMMC_ADMA;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISTER_EMMC_ADMA_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg ^= SDHC_EISTER_EMMC_ADMA;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_EISTER_EMMC_BOOTAE_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg |= SDHC_EISTER_EMMC_BOOTAE;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_EISTER_EMMC_BOOTAE_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISTER.reg;
	tmp = (tmp & SDHC_EISTER_EMMC_BOOTAE) >> SDHC_EISTER_EMMC_BOOTAE_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_EISTER_EMMC_BOOTAE_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->EISTER.reg;
	tmp &= ~SDHC_EISTER_EMMC_BOOTAE;
	tmp |= value << SDHC_EISTER_EMMC_BOOTAE_Pos;
	((Sdhc *)hw)->EISTER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISTER_EMMC_BOOTAE_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg &= ~SDHC_EISTER_EMMC_BOOTAE;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISTER_EMMC_BOOTAE_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg ^= SDHC_EISTER_EMMC_BOOTAE;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_EISTER_reg(const void *const hw, hri_sdhc_eister_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg |= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_eister_reg_t hri_sdhc_get_EISTER_reg(const void *const hw, hri_sdhc_eister_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISTER.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sdhc_write_EISTER_reg(const void *const hw, hri_sdhc_eister_reg_t data)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg = data;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISTER_reg(const void *const hw, hri_sdhc_eister_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg &= ~mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISTER_reg(const void *const hw, hri_sdhc_eister_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg ^= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_eister_reg_t hri_sdhc_read_EISTER_reg(const void *const hw)
{
	return ((Sdhc *)hw)->EISTER.reg;
}

static inline void hri_sdhc_set_EISTER_EMMC_reg(const void *const hw, hri_sdhc_eister_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg |= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_eister_reg_t hri_sdhc_get_EISTER_EMMC_reg(const void *const hw, hri_sdhc_eister_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISTER.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sdhc_write_EISTER_EMMC_reg(const void *const hw, hri_sdhc_eister_reg_t data)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg = data;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISTER_EMMC_reg(const void *const hw, hri_sdhc_eister_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg &= ~mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISTER_EMMC_reg(const void *const hw, hri_sdhc_eister_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISTER.reg ^= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_eister_reg_t hri_sdhc_read_EISTER_EMMC_reg(const void *const hw)
{
	return ((Sdhc *)hw)->EISTER.reg;
}

static inline void hri_sdhc_set_NISIER_CMDC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISIER.reg |= SDHC_NISIER_CMDC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_NISIER_CMDC_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->NISIER.reg;
	tmp = (tmp & SDHC_NISIER_CMDC) >> SDHC_NISIER_CMDC_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_NISIER_CMDC_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->NISIER.reg;
	tmp &= ~SDHC_NISIER_CMDC;
	tmp |= value << SDHC_NISIER_CMDC_Pos;
	((Sdhc *)hw)->NISIER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_NISIER_CMDC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISIER.reg &= ~SDHC_NISIER_CMDC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_NISIER_CMDC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISIER.reg ^= SDHC_NISIER_CMDC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_NISIER_TRFC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISIER.reg |= SDHC_NISIER_TRFC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_NISIER_TRFC_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->NISIER.reg;
	tmp = (tmp & SDHC_NISIER_TRFC) >> SDHC_NISIER_TRFC_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_NISIER_TRFC_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->NISIER.reg;
	tmp &= ~SDHC_NISIER_TRFC;
	tmp |= value << SDHC_NISIER_TRFC_Pos;
	((Sdhc *)hw)->NISIER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_NISIER_TRFC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISIER.reg &= ~SDHC_NISIER_TRFC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_NISIER_TRFC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISIER.reg ^= SDHC_NISIER_TRFC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_NISIER_BLKGE_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISIER.reg |= SDHC_NISIER_BLKGE;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_NISIER_BLKGE_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->NISIER.reg;
	tmp = (tmp & SDHC_NISIER_BLKGE) >> SDHC_NISIER_BLKGE_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_NISIER_BLKGE_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->NISIER.reg;
	tmp &= ~SDHC_NISIER_BLKGE;
	tmp |= value << SDHC_NISIER_BLKGE_Pos;
	((Sdhc *)hw)->NISIER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_NISIER_BLKGE_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISIER.reg &= ~SDHC_NISIER_BLKGE;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_NISIER_BLKGE_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISIER.reg ^= SDHC_NISIER_BLKGE;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_NISIER_DMAINT_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISIER.reg |= SDHC_NISIER_DMAINT;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_NISIER_DMAINT_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->NISIER.reg;
	tmp = (tmp & SDHC_NISIER_DMAINT) >> SDHC_NISIER_DMAINT_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_NISIER_DMAINT_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->NISIER.reg;
	tmp &= ~SDHC_NISIER_DMAINT;
	tmp |= value << SDHC_NISIER_DMAINT_Pos;
	((Sdhc *)hw)->NISIER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_NISIER_DMAINT_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISIER.reg &= ~SDHC_NISIER_DMAINT;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_NISIER_DMAINT_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISIER.reg ^= SDHC_NISIER_DMAINT;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_NISIER_BWRRDY_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISIER.reg |= SDHC_NISIER_BWRRDY;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_NISIER_BWRRDY_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->NISIER.reg;
	tmp = (tmp & SDHC_NISIER_BWRRDY) >> SDHC_NISIER_BWRRDY_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_NISIER_BWRRDY_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->NISIER.reg;
	tmp &= ~SDHC_NISIER_BWRRDY;
	tmp |= value << SDHC_NISIER_BWRRDY_Pos;
	((Sdhc *)hw)->NISIER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_NISIER_BWRRDY_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISIER.reg &= ~SDHC_NISIER_BWRRDY;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_NISIER_BWRRDY_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISIER.reg ^= SDHC_NISIER_BWRRDY;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_NISIER_BRDRDY_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISIER.reg |= SDHC_NISIER_BRDRDY;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_NISIER_BRDRDY_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->NISIER.reg;
	tmp = (tmp & SDHC_NISIER_BRDRDY) >> SDHC_NISIER_BRDRDY_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_NISIER_BRDRDY_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->NISIER.reg;
	tmp &= ~SDHC_NISIER_BRDRDY;
	tmp |= value << SDHC_NISIER_BRDRDY_Pos;
	((Sdhc *)hw)->NISIER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_NISIER_BRDRDY_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISIER.reg &= ~SDHC_NISIER_BRDRDY;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_NISIER_BRDRDY_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISIER.reg ^= SDHC_NISIER_BRDRDY;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_NISIER_CINS_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISIER.reg |= SDHC_NISIER_CINS;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_NISIER_CINS_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->NISIER.reg;
	tmp = (tmp & SDHC_NISIER_CINS) >> SDHC_NISIER_CINS_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_NISIER_CINS_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->NISIER.reg;
	tmp &= ~SDHC_NISIER_CINS;
	tmp |= value << SDHC_NISIER_CINS_Pos;
	((Sdhc *)hw)->NISIER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_NISIER_CINS_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISIER.reg &= ~SDHC_NISIER_CINS;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_NISIER_CINS_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISIER.reg ^= SDHC_NISIER_CINS;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_NISIER_CREM_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISIER.reg |= SDHC_NISIER_CREM;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_NISIER_CREM_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->NISIER.reg;
	tmp = (tmp & SDHC_NISIER_CREM) >> SDHC_NISIER_CREM_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_NISIER_CREM_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->NISIER.reg;
	tmp &= ~SDHC_NISIER_CREM;
	tmp |= value << SDHC_NISIER_CREM_Pos;
	((Sdhc *)hw)->NISIER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_NISIER_CREM_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISIER.reg &= ~SDHC_NISIER_CREM;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_NISIER_CREM_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISIER.reg ^= SDHC_NISIER_CREM;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_NISIER_CINT_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISIER.reg |= SDHC_NISIER_CINT;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_NISIER_CINT_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->NISIER.reg;
	tmp = (tmp & SDHC_NISIER_CINT) >> SDHC_NISIER_CINT_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_NISIER_CINT_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->NISIER.reg;
	tmp &= ~SDHC_NISIER_CINT;
	tmp |= value << SDHC_NISIER_CINT_Pos;
	((Sdhc *)hw)->NISIER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_NISIER_CINT_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISIER.reg &= ~SDHC_NISIER_CINT;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_NISIER_CINT_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISIER.reg ^= SDHC_NISIER_CINT;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_NISIER_EMMC_CMDC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISIER.reg |= SDHC_NISIER_EMMC_CMDC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_NISIER_EMMC_CMDC_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->NISIER.reg;
	tmp = (tmp & SDHC_NISIER_EMMC_CMDC) >> SDHC_NISIER_EMMC_CMDC_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_NISIER_EMMC_CMDC_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->NISIER.reg;
	tmp &= ~SDHC_NISIER_EMMC_CMDC;
	tmp |= value << SDHC_NISIER_EMMC_CMDC_Pos;
	((Sdhc *)hw)->NISIER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_NISIER_EMMC_CMDC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISIER.reg &= ~SDHC_NISIER_EMMC_CMDC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_NISIER_EMMC_CMDC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISIER.reg ^= SDHC_NISIER_EMMC_CMDC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_NISIER_EMMC_TRFC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISIER.reg |= SDHC_NISIER_EMMC_TRFC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_NISIER_EMMC_TRFC_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->NISIER.reg;
	tmp = (tmp & SDHC_NISIER_EMMC_TRFC) >> SDHC_NISIER_EMMC_TRFC_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_NISIER_EMMC_TRFC_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->NISIER.reg;
	tmp &= ~SDHC_NISIER_EMMC_TRFC;
	tmp |= value << SDHC_NISIER_EMMC_TRFC_Pos;
	((Sdhc *)hw)->NISIER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_NISIER_EMMC_TRFC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISIER.reg &= ~SDHC_NISIER_EMMC_TRFC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_NISIER_EMMC_TRFC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISIER.reg ^= SDHC_NISIER_EMMC_TRFC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_NISIER_EMMC_BLKGE_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISIER.reg |= SDHC_NISIER_EMMC_BLKGE;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_NISIER_EMMC_BLKGE_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->NISIER.reg;
	tmp = (tmp & SDHC_NISIER_EMMC_BLKGE) >> SDHC_NISIER_EMMC_BLKGE_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_NISIER_EMMC_BLKGE_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->NISIER.reg;
	tmp &= ~SDHC_NISIER_EMMC_BLKGE;
	tmp |= value << SDHC_NISIER_EMMC_BLKGE_Pos;
	((Sdhc *)hw)->NISIER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_NISIER_EMMC_BLKGE_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISIER.reg &= ~SDHC_NISIER_EMMC_BLKGE;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_NISIER_EMMC_BLKGE_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISIER.reg ^= SDHC_NISIER_EMMC_BLKGE;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_NISIER_EMMC_DMAINT_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISIER.reg |= SDHC_NISIER_EMMC_DMAINT;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_NISIER_EMMC_DMAINT_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->NISIER.reg;
	tmp = (tmp & SDHC_NISIER_EMMC_DMAINT) >> SDHC_NISIER_EMMC_DMAINT_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_NISIER_EMMC_DMAINT_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->NISIER.reg;
	tmp &= ~SDHC_NISIER_EMMC_DMAINT;
	tmp |= value << SDHC_NISIER_EMMC_DMAINT_Pos;
	((Sdhc *)hw)->NISIER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_NISIER_EMMC_DMAINT_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISIER.reg &= ~SDHC_NISIER_EMMC_DMAINT;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_NISIER_EMMC_DMAINT_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISIER.reg ^= SDHC_NISIER_EMMC_DMAINT;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_NISIER_EMMC_BWRRDY_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISIER.reg |= SDHC_NISIER_EMMC_BWRRDY;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_NISIER_EMMC_BWRRDY_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->NISIER.reg;
	tmp = (tmp & SDHC_NISIER_EMMC_BWRRDY) >> SDHC_NISIER_EMMC_BWRRDY_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_NISIER_EMMC_BWRRDY_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->NISIER.reg;
	tmp &= ~SDHC_NISIER_EMMC_BWRRDY;
	tmp |= value << SDHC_NISIER_EMMC_BWRRDY_Pos;
	((Sdhc *)hw)->NISIER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_NISIER_EMMC_BWRRDY_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISIER.reg &= ~SDHC_NISIER_EMMC_BWRRDY;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_NISIER_EMMC_BWRRDY_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISIER.reg ^= SDHC_NISIER_EMMC_BWRRDY;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_NISIER_EMMC_BRDRDY_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISIER.reg |= SDHC_NISIER_EMMC_BRDRDY;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_NISIER_EMMC_BRDRDY_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->NISIER.reg;
	tmp = (tmp & SDHC_NISIER_EMMC_BRDRDY) >> SDHC_NISIER_EMMC_BRDRDY_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_NISIER_EMMC_BRDRDY_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->NISIER.reg;
	tmp &= ~SDHC_NISIER_EMMC_BRDRDY;
	tmp |= value << SDHC_NISIER_EMMC_BRDRDY_Pos;
	((Sdhc *)hw)->NISIER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_NISIER_EMMC_BRDRDY_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISIER.reg &= ~SDHC_NISIER_EMMC_BRDRDY;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_NISIER_EMMC_BRDRDY_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISIER.reg ^= SDHC_NISIER_EMMC_BRDRDY;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_NISIER_EMMC_BOOTAR_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISIER.reg |= SDHC_NISIER_EMMC_BOOTAR;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_NISIER_EMMC_BOOTAR_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->NISIER.reg;
	tmp = (tmp & SDHC_NISIER_EMMC_BOOTAR) >> SDHC_NISIER_EMMC_BOOTAR_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_NISIER_EMMC_BOOTAR_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->NISIER.reg;
	tmp &= ~SDHC_NISIER_EMMC_BOOTAR;
	tmp |= value << SDHC_NISIER_EMMC_BOOTAR_Pos;
	((Sdhc *)hw)->NISIER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_NISIER_EMMC_BOOTAR_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISIER.reg &= ~SDHC_NISIER_EMMC_BOOTAR;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_NISIER_EMMC_BOOTAR_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISIER.reg ^= SDHC_NISIER_EMMC_BOOTAR;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_NISIER_reg(const void *const hw, hri_sdhc_nisier_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISIER.reg |= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_nisier_reg_t hri_sdhc_get_NISIER_reg(const void *const hw, hri_sdhc_nisier_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->NISIER.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sdhc_write_NISIER_reg(const void *const hw, hri_sdhc_nisier_reg_t data)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISIER.reg = data;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_NISIER_reg(const void *const hw, hri_sdhc_nisier_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISIER.reg &= ~mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_NISIER_reg(const void *const hw, hri_sdhc_nisier_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISIER.reg ^= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_nisier_reg_t hri_sdhc_read_NISIER_reg(const void *const hw)
{
	return ((Sdhc *)hw)->NISIER.reg;
}

static inline void hri_sdhc_set_NISIER_EMMC_reg(const void *const hw, hri_sdhc_nisier_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISIER.reg |= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_nisier_reg_t hri_sdhc_get_NISIER_EMMC_reg(const void *const hw, hri_sdhc_nisier_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->NISIER.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sdhc_write_NISIER_EMMC_reg(const void *const hw, hri_sdhc_nisier_reg_t data)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISIER.reg = data;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_NISIER_EMMC_reg(const void *const hw, hri_sdhc_nisier_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISIER.reg &= ~mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_NISIER_EMMC_reg(const void *const hw, hri_sdhc_nisier_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->NISIER.reg ^= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_nisier_reg_t hri_sdhc_read_NISIER_EMMC_reg(const void *const hw)
{
	return ((Sdhc *)hw)->NISIER.reg;
}

static inline void hri_sdhc_set_EISIER_CMDTEO_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg |= SDHC_EISIER_CMDTEO;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_EISIER_CMDTEO_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISIER.reg;
	tmp = (tmp & SDHC_EISIER_CMDTEO) >> SDHC_EISIER_CMDTEO_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_EISIER_CMDTEO_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->EISIER.reg;
	tmp &= ~SDHC_EISIER_CMDTEO;
	tmp |= value << SDHC_EISIER_CMDTEO_Pos;
	((Sdhc *)hw)->EISIER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISIER_CMDTEO_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg &= ~SDHC_EISIER_CMDTEO;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISIER_CMDTEO_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg ^= SDHC_EISIER_CMDTEO;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_EISIER_CMDCRC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg |= SDHC_EISIER_CMDCRC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_EISIER_CMDCRC_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISIER.reg;
	tmp = (tmp & SDHC_EISIER_CMDCRC) >> SDHC_EISIER_CMDCRC_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_EISIER_CMDCRC_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->EISIER.reg;
	tmp &= ~SDHC_EISIER_CMDCRC;
	tmp |= value << SDHC_EISIER_CMDCRC_Pos;
	((Sdhc *)hw)->EISIER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISIER_CMDCRC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg &= ~SDHC_EISIER_CMDCRC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISIER_CMDCRC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg ^= SDHC_EISIER_CMDCRC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_EISIER_CMDEND_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg |= SDHC_EISIER_CMDEND;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_EISIER_CMDEND_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISIER.reg;
	tmp = (tmp & SDHC_EISIER_CMDEND) >> SDHC_EISIER_CMDEND_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_EISIER_CMDEND_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->EISIER.reg;
	tmp &= ~SDHC_EISIER_CMDEND;
	tmp |= value << SDHC_EISIER_CMDEND_Pos;
	((Sdhc *)hw)->EISIER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISIER_CMDEND_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg &= ~SDHC_EISIER_CMDEND;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISIER_CMDEND_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg ^= SDHC_EISIER_CMDEND;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_EISIER_CMDIDX_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg |= SDHC_EISIER_CMDIDX;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_EISIER_CMDIDX_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISIER.reg;
	tmp = (tmp & SDHC_EISIER_CMDIDX) >> SDHC_EISIER_CMDIDX_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_EISIER_CMDIDX_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->EISIER.reg;
	tmp &= ~SDHC_EISIER_CMDIDX;
	tmp |= value << SDHC_EISIER_CMDIDX_Pos;
	((Sdhc *)hw)->EISIER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISIER_CMDIDX_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg &= ~SDHC_EISIER_CMDIDX;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISIER_CMDIDX_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg ^= SDHC_EISIER_CMDIDX;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_EISIER_DATTEO_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg |= SDHC_EISIER_DATTEO;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_EISIER_DATTEO_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISIER.reg;
	tmp = (tmp & SDHC_EISIER_DATTEO) >> SDHC_EISIER_DATTEO_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_EISIER_DATTEO_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->EISIER.reg;
	tmp &= ~SDHC_EISIER_DATTEO;
	tmp |= value << SDHC_EISIER_DATTEO_Pos;
	((Sdhc *)hw)->EISIER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISIER_DATTEO_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg &= ~SDHC_EISIER_DATTEO;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISIER_DATTEO_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg ^= SDHC_EISIER_DATTEO;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_EISIER_DATCRC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg |= SDHC_EISIER_DATCRC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_EISIER_DATCRC_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISIER.reg;
	tmp = (tmp & SDHC_EISIER_DATCRC) >> SDHC_EISIER_DATCRC_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_EISIER_DATCRC_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->EISIER.reg;
	tmp &= ~SDHC_EISIER_DATCRC;
	tmp |= value << SDHC_EISIER_DATCRC_Pos;
	((Sdhc *)hw)->EISIER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISIER_DATCRC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg &= ~SDHC_EISIER_DATCRC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISIER_DATCRC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg ^= SDHC_EISIER_DATCRC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_EISIER_DATEND_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg |= SDHC_EISIER_DATEND;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_EISIER_DATEND_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISIER.reg;
	tmp = (tmp & SDHC_EISIER_DATEND) >> SDHC_EISIER_DATEND_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_EISIER_DATEND_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->EISIER.reg;
	tmp &= ~SDHC_EISIER_DATEND;
	tmp |= value << SDHC_EISIER_DATEND_Pos;
	((Sdhc *)hw)->EISIER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISIER_DATEND_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg &= ~SDHC_EISIER_DATEND;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISIER_DATEND_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg ^= SDHC_EISIER_DATEND;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_EISIER_CURLIM_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg |= SDHC_EISIER_CURLIM;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_EISIER_CURLIM_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISIER.reg;
	tmp = (tmp & SDHC_EISIER_CURLIM) >> SDHC_EISIER_CURLIM_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_EISIER_CURLIM_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->EISIER.reg;
	tmp &= ~SDHC_EISIER_CURLIM;
	tmp |= value << SDHC_EISIER_CURLIM_Pos;
	((Sdhc *)hw)->EISIER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISIER_CURLIM_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg &= ~SDHC_EISIER_CURLIM;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISIER_CURLIM_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg ^= SDHC_EISIER_CURLIM;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_EISIER_ACMD_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg |= SDHC_EISIER_ACMD;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_EISIER_ACMD_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISIER.reg;
	tmp = (tmp & SDHC_EISIER_ACMD) >> SDHC_EISIER_ACMD_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_EISIER_ACMD_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->EISIER.reg;
	tmp &= ~SDHC_EISIER_ACMD;
	tmp |= value << SDHC_EISIER_ACMD_Pos;
	((Sdhc *)hw)->EISIER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISIER_ACMD_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg &= ~SDHC_EISIER_ACMD;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISIER_ACMD_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg ^= SDHC_EISIER_ACMD;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_EISIER_ADMA_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg |= SDHC_EISIER_ADMA;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_EISIER_ADMA_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISIER.reg;
	tmp = (tmp & SDHC_EISIER_ADMA) >> SDHC_EISIER_ADMA_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_EISIER_ADMA_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->EISIER.reg;
	tmp &= ~SDHC_EISIER_ADMA;
	tmp |= value << SDHC_EISIER_ADMA_Pos;
	((Sdhc *)hw)->EISIER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISIER_ADMA_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg &= ~SDHC_EISIER_ADMA;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISIER_ADMA_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg ^= SDHC_EISIER_ADMA;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_EISIER_EMMC_CMDTEO_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg |= SDHC_EISIER_EMMC_CMDTEO;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_EISIER_EMMC_CMDTEO_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISIER.reg;
	tmp = (tmp & SDHC_EISIER_EMMC_CMDTEO) >> SDHC_EISIER_EMMC_CMDTEO_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_EISIER_EMMC_CMDTEO_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->EISIER.reg;
	tmp &= ~SDHC_EISIER_EMMC_CMDTEO;
	tmp |= value << SDHC_EISIER_EMMC_CMDTEO_Pos;
	((Sdhc *)hw)->EISIER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISIER_EMMC_CMDTEO_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg &= ~SDHC_EISIER_EMMC_CMDTEO;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISIER_EMMC_CMDTEO_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg ^= SDHC_EISIER_EMMC_CMDTEO;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_EISIER_EMMC_CMDCRC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg |= SDHC_EISIER_EMMC_CMDCRC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_EISIER_EMMC_CMDCRC_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISIER.reg;
	tmp = (tmp & SDHC_EISIER_EMMC_CMDCRC) >> SDHC_EISIER_EMMC_CMDCRC_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_EISIER_EMMC_CMDCRC_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->EISIER.reg;
	tmp &= ~SDHC_EISIER_EMMC_CMDCRC;
	tmp |= value << SDHC_EISIER_EMMC_CMDCRC_Pos;
	((Sdhc *)hw)->EISIER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISIER_EMMC_CMDCRC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg &= ~SDHC_EISIER_EMMC_CMDCRC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISIER_EMMC_CMDCRC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg ^= SDHC_EISIER_EMMC_CMDCRC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_EISIER_EMMC_CMDEND_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg |= SDHC_EISIER_EMMC_CMDEND;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_EISIER_EMMC_CMDEND_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISIER.reg;
	tmp = (tmp & SDHC_EISIER_EMMC_CMDEND) >> SDHC_EISIER_EMMC_CMDEND_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_EISIER_EMMC_CMDEND_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->EISIER.reg;
	tmp &= ~SDHC_EISIER_EMMC_CMDEND;
	tmp |= value << SDHC_EISIER_EMMC_CMDEND_Pos;
	((Sdhc *)hw)->EISIER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISIER_EMMC_CMDEND_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg &= ~SDHC_EISIER_EMMC_CMDEND;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISIER_EMMC_CMDEND_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg ^= SDHC_EISIER_EMMC_CMDEND;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_EISIER_EMMC_CMDIDX_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg |= SDHC_EISIER_EMMC_CMDIDX;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_EISIER_EMMC_CMDIDX_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISIER.reg;
	tmp = (tmp & SDHC_EISIER_EMMC_CMDIDX) >> SDHC_EISIER_EMMC_CMDIDX_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_EISIER_EMMC_CMDIDX_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->EISIER.reg;
	tmp &= ~SDHC_EISIER_EMMC_CMDIDX;
	tmp |= value << SDHC_EISIER_EMMC_CMDIDX_Pos;
	((Sdhc *)hw)->EISIER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISIER_EMMC_CMDIDX_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg &= ~SDHC_EISIER_EMMC_CMDIDX;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISIER_EMMC_CMDIDX_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg ^= SDHC_EISIER_EMMC_CMDIDX;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_EISIER_EMMC_DATTEO_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg |= SDHC_EISIER_EMMC_DATTEO;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_EISIER_EMMC_DATTEO_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISIER.reg;
	tmp = (tmp & SDHC_EISIER_EMMC_DATTEO) >> SDHC_EISIER_EMMC_DATTEO_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_EISIER_EMMC_DATTEO_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->EISIER.reg;
	tmp &= ~SDHC_EISIER_EMMC_DATTEO;
	tmp |= value << SDHC_EISIER_EMMC_DATTEO_Pos;
	((Sdhc *)hw)->EISIER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISIER_EMMC_DATTEO_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg &= ~SDHC_EISIER_EMMC_DATTEO;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISIER_EMMC_DATTEO_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg ^= SDHC_EISIER_EMMC_DATTEO;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_EISIER_EMMC_DATCRC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg |= SDHC_EISIER_EMMC_DATCRC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_EISIER_EMMC_DATCRC_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISIER.reg;
	tmp = (tmp & SDHC_EISIER_EMMC_DATCRC) >> SDHC_EISIER_EMMC_DATCRC_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_EISIER_EMMC_DATCRC_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->EISIER.reg;
	tmp &= ~SDHC_EISIER_EMMC_DATCRC;
	tmp |= value << SDHC_EISIER_EMMC_DATCRC_Pos;
	((Sdhc *)hw)->EISIER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISIER_EMMC_DATCRC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg &= ~SDHC_EISIER_EMMC_DATCRC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISIER_EMMC_DATCRC_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg ^= SDHC_EISIER_EMMC_DATCRC;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_EISIER_EMMC_DATEND_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg |= SDHC_EISIER_EMMC_DATEND;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_EISIER_EMMC_DATEND_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISIER.reg;
	tmp = (tmp & SDHC_EISIER_EMMC_DATEND) >> SDHC_EISIER_EMMC_DATEND_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_EISIER_EMMC_DATEND_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->EISIER.reg;
	tmp &= ~SDHC_EISIER_EMMC_DATEND;
	tmp |= value << SDHC_EISIER_EMMC_DATEND_Pos;
	((Sdhc *)hw)->EISIER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISIER_EMMC_DATEND_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg &= ~SDHC_EISIER_EMMC_DATEND;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISIER_EMMC_DATEND_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg ^= SDHC_EISIER_EMMC_DATEND;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_EISIER_EMMC_CURLIM_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg |= SDHC_EISIER_EMMC_CURLIM;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_EISIER_EMMC_CURLIM_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISIER.reg;
	tmp = (tmp & SDHC_EISIER_EMMC_CURLIM) >> SDHC_EISIER_EMMC_CURLIM_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_EISIER_EMMC_CURLIM_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->EISIER.reg;
	tmp &= ~SDHC_EISIER_EMMC_CURLIM;
	tmp |= value << SDHC_EISIER_EMMC_CURLIM_Pos;
	((Sdhc *)hw)->EISIER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISIER_EMMC_CURLIM_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg &= ~SDHC_EISIER_EMMC_CURLIM;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISIER_EMMC_CURLIM_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg ^= SDHC_EISIER_EMMC_CURLIM;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_EISIER_EMMC_ACMD_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg |= SDHC_EISIER_EMMC_ACMD;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_EISIER_EMMC_ACMD_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISIER.reg;
	tmp = (tmp & SDHC_EISIER_EMMC_ACMD) >> SDHC_EISIER_EMMC_ACMD_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_EISIER_EMMC_ACMD_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->EISIER.reg;
	tmp &= ~SDHC_EISIER_EMMC_ACMD;
	tmp |= value << SDHC_EISIER_EMMC_ACMD_Pos;
	((Sdhc *)hw)->EISIER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISIER_EMMC_ACMD_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg &= ~SDHC_EISIER_EMMC_ACMD;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISIER_EMMC_ACMD_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg ^= SDHC_EISIER_EMMC_ACMD;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_EISIER_EMMC_ADMA_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg |= SDHC_EISIER_EMMC_ADMA;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_EISIER_EMMC_ADMA_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISIER.reg;
	tmp = (tmp & SDHC_EISIER_EMMC_ADMA) >> SDHC_EISIER_EMMC_ADMA_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_EISIER_EMMC_ADMA_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->EISIER.reg;
	tmp &= ~SDHC_EISIER_EMMC_ADMA;
	tmp |= value << SDHC_EISIER_EMMC_ADMA_Pos;
	((Sdhc *)hw)->EISIER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISIER_EMMC_ADMA_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg &= ~SDHC_EISIER_EMMC_ADMA;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISIER_EMMC_ADMA_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg ^= SDHC_EISIER_EMMC_ADMA;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_EISIER_EMMC_BOOTAE_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg |= SDHC_EISIER_EMMC_BOOTAE;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_EISIER_EMMC_BOOTAE_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISIER.reg;
	tmp = (tmp & SDHC_EISIER_EMMC_BOOTAE) >> SDHC_EISIER_EMMC_BOOTAE_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_EISIER_EMMC_BOOTAE_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->EISIER.reg;
	tmp &= ~SDHC_EISIER_EMMC_BOOTAE;
	tmp |= value << SDHC_EISIER_EMMC_BOOTAE_Pos;
	((Sdhc *)hw)->EISIER.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISIER_EMMC_BOOTAE_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg &= ~SDHC_EISIER_EMMC_BOOTAE;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISIER_EMMC_BOOTAE_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg ^= SDHC_EISIER_EMMC_BOOTAE;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_EISIER_reg(const void *const hw, hri_sdhc_eisier_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg |= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_eisier_reg_t hri_sdhc_get_EISIER_reg(const void *const hw, hri_sdhc_eisier_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISIER.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sdhc_write_EISIER_reg(const void *const hw, hri_sdhc_eisier_reg_t data)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg = data;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISIER_reg(const void *const hw, hri_sdhc_eisier_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg &= ~mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISIER_reg(const void *const hw, hri_sdhc_eisier_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg ^= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_eisier_reg_t hri_sdhc_read_EISIER_reg(const void *const hw)
{
	return ((Sdhc *)hw)->EISIER.reg;
}

static inline void hri_sdhc_set_EISIER_EMMC_reg(const void *const hw, hri_sdhc_eisier_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg |= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_eisier_reg_t hri_sdhc_get_EISIER_EMMC_reg(const void *const hw, hri_sdhc_eisier_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->EISIER.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sdhc_write_EISIER_EMMC_reg(const void *const hw, hri_sdhc_eisier_reg_t data)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg = data;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_EISIER_EMMC_reg(const void *const hw, hri_sdhc_eisier_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg &= ~mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_EISIER_EMMC_reg(const void *const hw, hri_sdhc_eisier_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->EISIER.reg ^= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_eisier_reg_t hri_sdhc_read_EISIER_EMMC_reg(const void *const hw)
{
	return ((Sdhc *)hw)->EISIER.reg;
}

static inline void hri_sdhc_set_HC2R_VS18EN_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC2R.reg |= SDHC_HC2R_VS18EN;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_HC2R_VS18EN_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->HC2R.reg;
	tmp = (tmp & SDHC_HC2R_VS18EN) >> SDHC_HC2R_VS18EN_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_HC2R_VS18EN_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->HC2R.reg;
	tmp &= ~SDHC_HC2R_VS18EN;
	tmp |= value << SDHC_HC2R_VS18EN_Pos;
	((Sdhc *)hw)->HC2R.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_HC2R_VS18EN_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC2R.reg &= ~SDHC_HC2R_VS18EN;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_HC2R_VS18EN_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC2R.reg ^= SDHC_HC2R_VS18EN;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_HC2R_EXTUN_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC2R.reg |= SDHC_HC2R_EXTUN;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_HC2R_EXTUN_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->HC2R.reg;
	tmp = (tmp & SDHC_HC2R_EXTUN) >> SDHC_HC2R_EXTUN_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_HC2R_EXTUN_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->HC2R.reg;
	tmp &= ~SDHC_HC2R_EXTUN;
	tmp |= value << SDHC_HC2R_EXTUN_Pos;
	((Sdhc *)hw)->HC2R.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_HC2R_EXTUN_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC2R.reg &= ~SDHC_HC2R_EXTUN;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_HC2R_EXTUN_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC2R.reg ^= SDHC_HC2R_EXTUN;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_HC2R_SLCKSEL_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC2R.reg |= SDHC_HC2R_SLCKSEL;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_HC2R_SLCKSEL_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->HC2R.reg;
	tmp = (tmp & SDHC_HC2R_SLCKSEL) >> SDHC_HC2R_SLCKSEL_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_HC2R_SLCKSEL_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->HC2R.reg;
	tmp &= ~SDHC_HC2R_SLCKSEL;
	tmp |= value << SDHC_HC2R_SLCKSEL_Pos;
	((Sdhc *)hw)->HC2R.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_HC2R_SLCKSEL_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC2R.reg &= ~SDHC_HC2R_SLCKSEL;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_HC2R_SLCKSEL_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC2R.reg ^= SDHC_HC2R_SLCKSEL;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_HC2R_ASINTEN_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC2R.reg |= SDHC_HC2R_ASINTEN;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_HC2R_ASINTEN_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->HC2R.reg;
	tmp = (tmp & SDHC_HC2R_ASINTEN) >> SDHC_HC2R_ASINTEN_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_HC2R_ASINTEN_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->HC2R.reg;
	tmp &= ~SDHC_HC2R_ASINTEN;
	tmp |= value << SDHC_HC2R_ASINTEN_Pos;
	((Sdhc *)hw)->HC2R.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_HC2R_ASINTEN_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC2R.reg &= ~SDHC_HC2R_ASINTEN;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_HC2R_ASINTEN_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC2R.reg ^= SDHC_HC2R_ASINTEN;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_HC2R_PVALEN_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC2R.reg |= SDHC_HC2R_PVALEN;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_HC2R_PVALEN_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->HC2R.reg;
	tmp = (tmp & SDHC_HC2R_PVALEN) >> SDHC_HC2R_PVALEN_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_HC2R_PVALEN_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->HC2R.reg;
	tmp &= ~SDHC_HC2R_PVALEN;
	tmp |= value << SDHC_HC2R_PVALEN_Pos;
	((Sdhc *)hw)->HC2R.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_HC2R_PVALEN_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC2R.reg &= ~SDHC_HC2R_PVALEN;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_HC2R_PVALEN_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC2R.reg ^= SDHC_HC2R_PVALEN;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_HC2R_EMMC_EXTUN_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC2R.reg |= SDHC_HC2R_EMMC_EXTUN;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_HC2R_EMMC_EXTUN_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->HC2R.reg;
	tmp = (tmp & SDHC_HC2R_EMMC_EXTUN) >> SDHC_HC2R_EMMC_EXTUN_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_HC2R_EMMC_EXTUN_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->HC2R.reg;
	tmp &= ~SDHC_HC2R_EMMC_EXTUN;
	tmp |= value << SDHC_HC2R_EMMC_EXTUN_Pos;
	((Sdhc *)hw)->HC2R.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_HC2R_EMMC_EXTUN_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC2R.reg &= ~SDHC_HC2R_EMMC_EXTUN;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_HC2R_EMMC_EXTUN_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC2R.reg ^= SDHC_HC2R_EMMC_EXTUN;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_HC2R_EMMC_SLCKSEL_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC2R.reg |= SDHC_HC2R_EMMC_SLCKSEL;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_HC2R_EMMC_SLCKSEL_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->HC2R.reg;
	tmp = (tmp & SDHC_HC2R_EMMC_SLCKSEL) >> SDHC_HC2R_EMMC_SLCKSEL_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_HC2R_EMMC_SLCKSEL_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->HC2R.reg;
	tmp &= ~SDHC_HC2R_EMMC_SLCKSEL;
	tmp |= value << SDHC_HC2R_EMMC_SLCKSEL_Pos;
	((Sdhc *)hw)->HC2R.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_HC2R_EMMC_SLCKSEL_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC2R.reg &= ~SDHC_HC2R_EMMC_SLCKSEL;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_HC2R_EMMC_SLCKSEL_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC2R.reg ^= SDHC_HC2R_EMMC_SLCKSEL;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_HC2R_EMMC_PVALEN_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC2R.reg |= SDHC_HC2R_EMMC_PVALEN;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_HC2R_EMMC_PVALEN_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->HC2R.reg;
	tmp = (tmp & SDHC_HC2R_EMMC_PVALEN) >> SDHC_HC2R_EMMC_PVALEN_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_HC2R_EMMC_PVALEN_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->HC2R.reg;
	tmp &= ~SDHC_HC2R_EMMC_PVALEN;
	tmp |= value << SDHC_HC2R_EMMC_PVALEN_Pos;
	((Sdhc *)hw)->HC2R.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_HC2R_EMMC_PVALEN_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC2R.reg &= ~SDHC_HC2R_EMMC_PVALEN;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_HC2R_EMMC_PVALEN_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC2R.reg ^= SDHC_HC2R_EMMC_PVALEN;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_HC2R_UHSMS_bf(const void *const hw, hri_sdhc_hc2r_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC2R.reg |= SDHC_HC2R_UHSMS(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_hc2r_reg_t hri_sdhc_get_HC2R_UHSMS_bf(const void *const hw, hri_sdhc_hc2r_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->HC2R.reg;
	tmp = (tmp & SDHC_HC2R_UHSMS(mask)) >> SDHC_HC2R_UHSMS_Pos;
	return tmp;
}

static inline void hri_sdhc_write_HC2R_UHSMS_bf(const void *const hw, hri_sdhc_hc2r_reg_t data)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->HC2R.reg;
	tmp &= ~SDHC_HC2R_UHSMS_Msk;
	tmp |= SDHC_HC2R_UHSMS(data);
	((Sdhc *)hw)->HC2R.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_HC2R_UHSMS_bf(const void *const hw, hri_sdhc_hc2r_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC2R.reg &= ~SDHC_HC2R_UHSMS(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_HC2R_UHSMS_bf(const void *const hw, hri_sdhc_hc2r_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC2R.reg ^= SDHC_HC2R_UHSMS(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_hc2r_reg_t hri_sdhc_read_HC2R_UHSMS_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->HC2R.reg;
	tmp = (tmp & SDHC_HC2R_UHSMS_Msk) >> SDHC_HC2R_UHSMS_Pos;
	return tmp;
}

static inline void hri_sdhc_set_HC2R_DRVSEL_bf(const void *const hw, hri_sdhc_hc2r_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC2R.reg |= SDHC_HC2R_DRVSEL(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_hc2r_reg_t hri_sdhc_get_HC2R_DRVSEL_bf(const void *const hw, hri_sdhc_hc2r_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->HC2R.reg;
	tmp = (tmp & SDHC_HC2R_DRVSEL(mask)) >> SDHC_HC2R_DRVSEL_Pos;
	return tmp;
}

static inline void hri_sdhc_write_HC2R_DRVSEL_bf(const void *const hw, hri_sdhc_hc2r_reg_t data)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->HC2R.reg;
	tmp &= ~SDHC_HC2R_DRVSEL_Msk;
	tmp |= SDHC_HC2R_DRVSEL(data);
	((Sdhc *)hw)->HC2R.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_HC2R_DRVSEL_bf(const void *const hw, hri_sdhc_hc2r_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC2R.reg &= ~SDHC_HC2R_DRVSEL(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_HC2R_DRVSEL_bf(const void *const hw, hri_sdhc_hc2r_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC2R.reg ^= SDHC_HC2R_DRVSEL(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_hc2r_reg_t hri_sdhc_read_HC2R_DRVSEL_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->HC2R.reg;
	tmp = (tmp & SDHC_HC2R_DRVSEL_Msk) >> SDHC_HC2R_DRVSEL_Pos;
	return tmp;
}

static inline void hri_sdhc_set_HC2R_EMMC_HS200EN_bf(const void *const hw, hri_sdhc_hc2r_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC2R.reg |= SDHC_HC2R_EMMC_HS200EN(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_hc2r_reg_t hri_sdhc_get_HC2R_EMMC_HS200EN_bf(const void *const hw, hri_sdhc_hc2r_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->HC2R.reg;
	tmp = (tmp & SDHC_HC2R_EMMC_HS200EN(mask)) >> SDHC_HC2R_EMMC_HS200EN_Pos;
	return tmp;
}

static inline void hri_sdhc_write_HC2R_EMMC_HS200EN_bf(const void *const hw, hri_sdhc_hc2r_reg_t data)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->HC2R.reg;
	tmp &= ~SDHC_HC2R_EMMC_HS200EN_Msk;
	tmp |= SDHC_HC2R_EMMC_HS200EN(data);
	((Sdhc *)hw)->HC2R.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_HC2R_EMMC_HS200EN_bf(const void *const hw, hri_sdhc_hc2r_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC2R.reg &= ~SDHC_HC2R_EMMC_HS200EN(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_HC2R_EMMC_HS200EN_bf(const void *const hw, hri_sdhc_hc2r_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC2R.reg ^= SDHC_HC2R_EMMC_HS200EN(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_hc2r_reg_t hri_sdhc_read_HC2R_EMMC_HS200EN_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->HC2R.reg;
	tmp = (tmp & SDHC_HC2R_EMMC_HS200EN_Msk) >> SDHC_HC2R_EMMC_HS200EN_Pos;
	return tmp;
}

static inline void hri_sdhc_set_HC2R_EMMC_DRVSEL_bf(const void *const hw, hri_sdhc_hc2r_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC2R.reg |= SDHC_HC2R_EMMC_DRVSEL(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_hc2r_reg_t hri_sdhc_get_HC2R_EMMC_DRVSEL_bf(const void *const hw, hri_sdhc_hc2r_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->HC2R.reg;
	tmp = (tmp & SDHC_HC2R_EMMC_DRVSEL(mask)) >> SDHC_HC2R_EMMC_DRVSEL_Pos;
	return tmp;
}

static inline void hri_sdhc_write_HC2R_EMMC_DRVSEL_bf(const void *const hw, hri_sdhc_hc2r_reg_t data)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->HC2R.reg;
	tmp &= ~SDHC_HC2R_EMMC_DRVSEL_Msk;
	tmp |= SDHC_HC2R_EMMC_DRVSEL(data);
	((Sdhc *)hw)->HC2R.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_HC2R_EMMC_DRVSEL_bf(const void *const hw, hri_sdhc_hc2r_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC2R.reg &= ~SDHC_HC2R_EMMC_DRVSEL(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_HC2R_EMMC_DRVSEL_bf(const void *const hw, hri_sdhc_hc2r_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC2R.reg ^= SDHC_HC2R_EMMC_DRVSEL(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_hc2r_reg_t hri_sdhc_read_HC2R_EMMC_DRVSEL_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->HC2R.reg;
	tmp = (tmp & SDHC_HC2R_EMMC_DRVSEL_Msk) >> SDHC_HC2R_EMMC_DRVSEL_Pos;
	return tmp;
}

static inline void hri_sdhc_set_HC2R_reg(const void *const hw, hri_sdhc_hc2r_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC2R.reg |= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_hc2r_reg_t hri_sdhc_get_HC2R_reg(const void *const hw, hri_sdhc_hc2r_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->HC2R.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sdhc_write_HC2R_reg(const void *const hw, hri_sdhc_hc2r_reg_t data)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC2R.reg = data;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_HC2R_reg(const void *const hw, hri_sdhc_hc2r_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC2R.reg &= ~mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_HC2R_reg(const void *const hw, hri_sdhc_hc2r_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC2R.reg ^= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_hc2r_reg_t hri_sdhc_read_HC2R_reg(const void *const hw)
{
	return ((Sdhc *)hw)->HC2R.reg;
}

static inline void hri_sdhc_set_HC2R_EMMC_reg(const void *const hw, hri_sdhc_hc2r_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC2R.reg |= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_hc2r_reg_t hri_sdhc_get_HC2R_EMMC_reg(const void *const hw, hri_sdhc_hc2r_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->HC2R.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sdhc_write_HC2R_EMMC_reg(const void *const hw, hri_sdhc_hc2r_reg_t data)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC2R.reg = data;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_HC2R_EMMC_reg(const void *const hw, hri_sdhc_hc2r_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC2R.reg &= ~mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_HC2R_EMMC_reg(const void *const hw, hri_sdhc_hc2r_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->HC2R.reg ^= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_hc2r_reg_t hri_sdhc_read_HC2R_EMMC_reg(const void *const hw)
{
	return ((Sdhc *)hw)->HC2R.reg;
}

static inline void hri_sdhc_write_FERACES_reg(const void *const hw, hri_sdhc_feraces_reg_t data)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->FERACES.reg = data;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_write_FEREIS_reg(const void *const hw, hri_sdhc_fereis_reg_t data)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->FEREIS.reg = data;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_write_MC2R_reg(const void *const hw, hri_sdhc_mc2r_reg_t data)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->MC2R.reg = data;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_BSR_BLOCKSIZE_bf(const void *const hw, hri_sdhc_bsr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->BSR.reg |= SDHC_BSR_BLOCKSIZE(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_bsr_reg_t hri_sdhc_get_BSR_BLOCKSIZE_bf(const void *const hw, hri_sdhc_bsr_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->BSR.reg;
	tmp = (tmp & SDHC_BSR_BLOCKSIZE(mask)) >> SDHC_BSR_BLOCKSIZE_Pos;
	return tmp;
}

static inline void hri_sdhc_write_BSR_BLOCKSIZE_bf(const void *const hw, hri_sdhc_bsr_reg_t data)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->BSR.reg;
	tmp &= ~SDHC_BSR_BLOCKSIZE_Msk;
	tmp |= SDHC_BSR_BLOCKSIZE(data);
	((Sdhc *)hw)->BSR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_BSR_BLOCKSIZE_bf(const void *const hw, hri_sdhc_bsr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->BSR.reg &= ~SDHC_BSR_BLOCKSIZE(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_BSR_BLOCKSIZE_bf(const void *const hw, hri_sdhc_bsr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->BSR.reg ^= SDHC_BSR_BLOCKSIZE(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_bsr_reg_t hri_sdhc_read_BSR_BLOCKSIZE_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->BSR.reg;
	tmp = (tmp & SDHC_BSR_BLOCKSIZE_Msk) >> SDHC_BSR_BLOCKSIZE_Pos;
	return tmp;
}

static inline void hri_sdhc_set_BSR_BOUNDARY_bf(const void *const hw, hri_sdhc_bsr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->BSR.reg |= SDHC_BSR_BOUNDARY(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_bsr_reg_t hri_sdhc_get_BSR_BOUNDARY_bf(const void *const hw, hri_sdhc_bsr_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->BSR.reg;
	tmp = (tmp & SDHC_BSR_BOUNDARY(mask)) >> SDHC_BSR_BOUNDARY_Pos;
	return tmp;
}

static inline void hri_sdhc_write_BSR_BOUNDARY_bf(const void *const hw, hri_sdhc_bsr_reg_t data)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->BSR.reg;
	tmp &= ~SDHC_BSR_BOUNDARY_Msk;
	tmp |= SDHC_BSR_BOUNDARY(data);
	((Sdhc *)hw)->BSR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_BSR_BOUNDARY_bf(const void *const hw, hri_sdhc_bsr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->BSR.reg &= ~SDHC_BSR_BOUNDARY(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_BSR_BOUNDARY_bf(const void *const hw, hri_sdhc_bsr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->BSR.reg ^= SDHC_BSR_BOUNDARY(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_bsr_reg_t hri_sdhc_read_BSR_BOUNDARY_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->BSR.reg;
	tmp = (tmp & SDHC_BSR_BOUNDARY_Msk) >> SDHC_BSR_BOUNDARY_Pos;
	return tmp;
}

static inline void hri_sdhc_set_BSR_reg(const void *const hw, hri_sdhc_bsr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->BSR.reg |= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_bsr_reg_t hri_sdhc_get_BSR_reg(const void *const hw, hri_sdhc_bsr_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->BSR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sdhc_write_BSR_reg(const void *const hw, hri_sdhc_bsr_reg_t data)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->BSR.reg = data;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_BSR_reg(const void *const hw, hri_sdhc_bsr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->BSR.reg &= ~mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_BSR_reg(const void *const hw, hri_sdhc_bsr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->BSR.reg ^= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_bsr_reg_t hri_sdhc_read_BSR_reg(const void *const hw)
{
	return ((Sdhc *)hw)->BSR.reg;
}

static inline void hri_sdhc_set_BCR_BCNT_bf(const void *const hw, hri_sdhc_bcr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->BCR.reg |= SDHC_BCR_BCNT(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_bcr_reg_t hri_sdhc_get_BCR_BCNT_bf(const void *const hw, hri_sdhc_bcr_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->BCR.reg;
	tmp = (tmp & SDHC_BCR_BCNT(mask)) >> SDHC_BCR_BCNT_Pos;
	return tmp;
}

static inline void hri_sdhc_write_BCR_BCNT_bf(const void *const hw, hri_sdhc_bcr_reg_t data)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->BCR.reg;
	tmp &= ~SDHC_BCR_BCNT_Msk;
	tmp |= SDHC_BCR_BCNT(data);
	((Sdhc *)hw)->BCR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_BCR_BCNT_bf(const void *const hw, hri_sdhc_bcr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->BCR.reg &= ~SDHC_BCR_BCNT(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_BCR_BCNT_bf(const void *const hw, hri_sdhc_bcr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->BCR.reg ^= SDHC_BCR_BCNT(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_bcr_reg_t hri_sdhc_read_BCR_BCNT_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->BCR.reg;
	tmp = (tmp & SDHC_BCR_BCNT_Msk) >> SDHC_BCR_BCNT_Pos;
	return tmp;
}

static inline void hri_sdhc_set_BCR_reg(const void *const hw, hri_sdhc_bcr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->BCR.reg |= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_bcr_reg_t hri_sdhc_get_BCR_reg(const void *const hw, hri_sdhc_bcr_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->BCR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sdhc_write_BCR_reg(const void *const hw, hri_sdhc_bcr_reg_t data)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->BCR.reg = data;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_BCR_reg(const void *const hw, hri_sdhc_bcr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->BCR.reg &= ~mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_BCR_reg(const void *const hw, hri_sdhc_bcr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->BCR.reg ^= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_bcr_reg_t hri_sdhc_read_BCR_reg(const void *const hw)
{
	return ((Sdhc *)hw)->BCR.reg;
}

static inline void hri_sdhc_set_ARG1R_ARG_bf(const void *const hw, hri_sdhc_arg1r_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->ARG1R.reg |= SDHC_ARG1R_ARG(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_arg1r_reg_t hri_sdhc_get_ARG1R_ARG_bf(const void *const hw, hri_sdhc_arg1r_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sdhc *)hw)->ARG1R.reg;
	tmp = (tmp & SDHC_ARG1R_ARG(mask)) >> SDHC_ARG1R_ARG_Pos;
	return tmp;
}

static inline void hri_sdhc_write_ARG1R_ARG_bf(const void *const hw, hri_sdhc_arg1r_reg_t data)
{
	uint32_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->ARG1R.reg;
	tmp &= ~SDHC_ARG1R_ARG_Msk;
	tmp |= SDHC_ARG1R_ARG(data);
	((Sdhc *)hw)->ARG1R.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_ARG1R_ARG_bf(const void *const hw, hri_sdhc_arg1r_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->ARG1R.reg &= ~SDHC_ARG1R_ARG(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_ARG1R_ARG_bf(const void *const hw, hri_sdhc_arg1r_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->ARG1R.reg ^= SDHC_ARG1R_ARG(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_arg1r_reg_t hri_sdhc_read_ARG1R_ARG_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sdhc *)hw)->ARG1R.reg;
	tmp = (tmp & SDHC_ARG1R_ARG_Msk) >> SDHC_ARG1R_ARG_Pos;
	return tmp;
}

static inline void hri_sdhc_set_ARG1R_reg(const void *const hw, hri_sdhc_arg1r_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->ARG1R.reg |= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_arg1r_reg_t hri_sdhc_get_ARG1R_reg(const void *const hw, hri_sdhc_arg1r_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sdhc *)hw)->ARG1R.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sdhc_write_ARG1R_reg(const void *const hw, hri_sdhc_arg1r_reg_t data)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->ARG1R.reg = data;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_ARG1R_reg(const void *const hw, hri_sdhc_arg1r_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->ARG1R.reg &= ~mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_ARG1R_reg(const void *const hw, hri_sdhc_arg1r_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->ARG1R.reg ^= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_arg1r_reg_t hri_sdhc_read_ARG1R_reg(const void *const hw)
{
	return ((Sdhc *)hw)->ARG1R.reg;
}

static inline void hri_sdhc_set_TMR_DMAEN_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->TMR.reg |= SDHC_TMR_DMAEN;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_TMR_DMAEN_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->TMR.reg;
	tmp = (tmp & SDHC_TMR_DMAEN) >> SDHC_TMR_DMAEN_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_TMR_DMAEN_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->TMR.reg;
	tmp &= ~SDHC_TMR_DMAEN;
	tmp |= value << SDHC_TMR_DMAEN_Pos;
	((Sdhc *)hw)->TMR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_TMR_DMAEN_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->TMR.reg &= ~SDHC_TMR_DMAEN;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_TMR_DMAEN_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->TMR.reg ^= SDHC_TMR_DMAEN;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_TMR_BCEN_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->TMR.reg |= SDHC_TMR_BCEN;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_TMR_BCEN_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->TMR.reg;
	tmp = (tmp & SDHC_TMR_BCEN) >> SDHC_TMR_BCEN_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_TMR_BCEN_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->TMR.reg;
	tmp &= ~SDHC_TMR_BCEN;
	tmp |= value << SDHC_TMR_BCEN_Pos;
	((Sdhc *)hw)->TMR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_TMR_BCEN_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->TMR.reg &= ~SDHC_TMR_BCEN;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_TMR_BCEN_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->TMR.reg ^= SDHC_TMR_BCEN;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_TMR_DTDSEL_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->TMR.reg |= SDHC_TMR_DTDSEL;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_TMR_DTDSEL_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->TMR.reg;
	tmp = (tmp & SDHC_TMR_DTDSEL) >> SDHC_TMR_DTDSEL_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_TMR_DTDSEL_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->TMR.reg;
	tmp &= ~SDHC_TMR_DTDSEL;
	tmp |= value << SDHC_TMR_DTDSEL_Pos;
	((Sdhc *)hw)->TMR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_TMR_DTDSEL_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->TMR.reg &= ~SDHC_TMR_DTDSEL;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_TMR_DTDSEL_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->TMR.reg ^= SDHC_TMR_DTDSEL;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_TMR_MSBSEL_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->TMR.reg |= SDHC_TMR_MSBSEL;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_TMR_MSBSEL_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->TMR.reg;
	tmp = (tmp & SDHC_TMR_MSBSEL) >> SDHC_TMR_MSBSEL_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_TMR_MSBSEL_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->TMR.reg;
	tmp &= ~SDHC_TMR_MSBSEL;
	tmp |= value << SDHC_TMR_MSBSEL_Pos;
	((Sdhc *)hw)->TMR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_TMR_MSBSEL_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->TMR.reg &= ~SDHC_TMR_MSBSEL;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_TMR_MSBSEL_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->TMR.reg ^= SDHC_TMR_MSBSEL;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_TMR_ACMDEN_bf(const void *const hw, hri_sdhc_tmr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->TMR.reg |= SDHC_TMR_ACMDEN(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_tmr_reg_t hri_sdhc_get_TMR_ACMDEN_bf(const void *const hw, hri_sdhc_tmr_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->TMR.reg;
	tmp = (tmp & SDHC_TMR_ACMDEN(mask)) >> SDHC_TMR_ACMDEN_Pos;
	return tmp;
}

static inline void hri_sdhc_write_TMR_ACMDEN_bf(const void *const hw, hri_sdhc_tmr_reg_t data)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->TMR.reg;
	tmp &= ~SDHC_TMR_ACMDEN_Msk;
	tmp |= SDHC_TMR_ACMDEN(data);
	((Sdhc *)hw)->TMR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_TMR_ACMDEN_bf(const void *const hw, hri_sdhc_tmr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->TMR.reg &= ~SDHC_TMR_ACMDEN(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_TMR_ACMDEN_bf(const void *const hw, hri_sdhc_tmr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->TMR.reg ^= SDHC_TMR_ACMDEN(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_tmr_reg_t hri_sdhc_read_TMR_ACMDEN_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->TMR.reg;
	tmp = (tmp & SDHC_TMR_ACMDEN_Msk) >> SDHC_TMR_ACMDEN_Pos;
	return tmp;
}

static inline void hri_sdhc_set_TMR_reg(const void *const hw, hri_sdhc_tmr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->TMR.reg |= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_tmr_reg_t hri_sdhc_get_TMR_reg(const void *const hw, hri_sdhc_tmr_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->TMR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sdhc_write_TMR_reg(const void *const hw, hri_sdhc_tmr_reg_t data)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->TMR.reg = data;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_TMR_reg(const void *const hw, hri_sdhc_tmr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->TMR.reg &= ~mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_TMR_reg(const void *const hw, hri_sdhc_tmr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->TMR.reg ^= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_tmr_reg_t hri_sdhc_read_TMR_reg(const void *const hw)
{
	return ((Sdhc *)hw)->TMR.reg;
}

static inline void hri_sdhc_set_CR_CMDCCEN_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->CR.reg |= SDHC_CR_CMDCCEN;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_CR_CMDCCEN_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->CR.reg;
	tmp = (tmp & SDHC_CR_CMDCCEN) >> SDHC_CR_CMDCCEN_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_CR_CMDCCEN_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->CR.reg;
	tmp &= ~SDHC_CR_CMDCCEN;
	tmp |= value << SDHC_CR_CMDCCEN_Pos;
	((Sdhc *)hw)->CR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_CR_CMDCCEN_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->CR.reg &= ~SDHC_CR_CMDCCEN;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_CR_CMDCCEN_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->CR.reg ^= SDHC_CR_CMDCCEN;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_CR_CMDICEN_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->CR.reg |= SDHC_CR_CMDICEN;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_CR_CMDICEN_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->CR.reg;
	tmp = (tmp & SDHC_CR_CMDICEN) >> SDHC_CR_CMDICEN_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_CR_CMDICEN_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->CR.reg;
	tmp &= ~SDHC_CR_CMDICEN;
	tmp |= value << SDHC_CR_CMDICEN_Pos;
	((Sdhc *)hw)->CR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_CR_CMDICEN_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->CR.reg &= ~SDHC_CR_CMDICEN;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_CR_CMDICEN_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->CR.reg ^= SDHC_CR_CMDICEN;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_CR_DPSEL_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->CR.reg |= SDHC_CR_DPSEL;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_CR_DPSEL_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->CR.reg;
	tmp = (tmp & SDHC_CR_DPSEL) >> SDHC_CR_DPSEL_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_CR_DPSEL_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->CR.reg;
	tmp &= ~SDHC_CR_DPSEL;
	tmp |= value << SDHC_CR_DPSEL_Pos;
	((Sdhc *)hw)->CR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_CR_DPSEL_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->CR.reg &= ~SDHC_CR_DPSEL;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_CR_DPSEL_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->CR.reg ^= SDHC_CR_DPSEL;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_CR_RESPTYP_bf(const void *const hw, hri_sdhc_cr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->CR.reg |= SDHC_CR_RESPTYP(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_cr_reg_t hri_sdhc_get_CR_RESPTYP_bf(const void *const hw, hri_sdhc_cr_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->CR.reg;
	tmp = (tmp & SDHC_CR_RESPTYP(mask)) >> SDHC_CR_RESPTYP_Pos;
	return tmp;
}

static inline void hri_sdhc_write_CR_RESPTYP_bf(const void *const hw, hri_sdhc_cr_reg_t data)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->CR.reg;
	tmp &= ~SDHC_CR_RESPTYP_Msk;
	tmp |= SDHC_CR_RESPTYP(data);
	((Sdhc *)hw)->CR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_CR_RESPTYP_bf(const void *const hw, hri_sdhc_cr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->CR.reg &= ~SDHC_CR_RESPTYP(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_CR_RESPTYP_bf(const void *const hw, hri_sdhc_cr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->CR.reg ^= SDHC_CR_RESPTYP(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_cr_reg_t hri_sdhc_read_CR_RESPTYP_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->CR.reg;
	tmp = (tmp & SDHC_CR_RESPTYP_Msk) >> SDHC_CR_RESPTYP_Pos;
	return tmp;
}

static inline void hri_sdhc_set_CR_CMDTYP_bf(const void *const hw, hri_sdhc_cr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->CR.reg |= SDHC_CR_CMDTYP(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_cr_reg_t hri_sdhc_get_CR_CMDTYP_bf(const void *const hw, hri_sdhc_cr_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->CR.reg;
	tmp = (tmp & SDHC_CR_CMDTYP(mask)) >> SDHC_CR_CMDTYP_Pos;
	return tmp;
}

static inline void hri_sdhc_write_CR_CMDTYP_bf(const void *const hw, hri_sdhc_cr_reg_t data)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->CR.reg;
	tmp &= ~SDHC_CR_CMDTYP_Msk;
	tmp |= SDHC_CR_CMDTYP(data);
	((Sdhc *)hw)->CR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_CR_CMDTYP_bf(const void *const hw, hri_sdhc_cr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->CR.reg &= ~SDHC_CR_CMDTYP(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_CR_CMDTYP_bf(const void *const hw, hri_sdhc_cr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->CR.reg ^= SDHC_CR_CMDTYP(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_cr_reg_t hri_sdhc_read_CR_CMDTYP_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->CR.reg;
	tmp = (tmp & SDHC_CR_CMDTYP_Msk) >> SDHC_CR_CMDTYP_Pos;
	return tmp;
}

static inline void hri_sdhc_set_CR_CMDIDX_bf(const void *const hw, hri_sdhc_cr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->CR.reg |= SDHC_CR_CMDIDX(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_cr_reg_t hri_sdhc_get_CR_CMDIDX_bf(const void *const hw, hri_sdhc_cr_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->CR.reg;
	tmp = (tmp & SDHC_CR_CMDIDX(mask)) >> SDHC_CR_CMDIDX_Pos;
	return tmp;
}

static inline void hri_sdhc_write_CR_CMDIDX_bf(const void *const hw, hri_sdhc_cr_reg_t data)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->CR.reg;
	tmp &= ~SDHC_CR_CMDIDX_Msk;
	tmp |= SDHC_CR_CMDIDX(data);
	((Sdhc *)hw)->CR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_CR_CMDIDX_bf(const void *const hw, hri_sdhc_cr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->CR.reg &= ~SDHC_CR_CMDIDX(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_CR_CMDIDX_bf(const void *const hw, hri_sdhc_cr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->CR.reg ^= SDHC_CR_CMDIDX(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_cr_reg_t hri_sdhc_read_CR_CMDIDX_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->CR.reg;
	tmp = (tmp & SDHC_CR_CMDIDX_Msk) >> SDHC_CR_CMDIDX_Pos;
	return tmp;
}

static inline void hri_sdhc_set_CR_reg(const void *const hw, hri_sdhc_cr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->CR.reg |= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_cr_reg_t hri_sdhc_get_CR_reg(const void *const hw, hri_sdhc_cr_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->CR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sdhc_write_CR_reg(const void *const hw, hri_sdhc_cr_reg_t data)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->CR.reg = data;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_CR_reg(const void *const hw, hri_sdhc_cr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->CR.reg &= ~mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_CR_reg(const void *const hw, hri_sdhc_cr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->CR.reg ^= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_cr_reg_t hri_sdhc_read_CR_reg(const void *const hw)
{
	return ((Sdhc *)hw)->CR.reg;
}

static inline void hri_sdhc_set_BDPR_BUFDATA_bf(const void *const hw, hri_sdhc_bdpr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->BDPR.reg |= SDHC_BDPR_BUFDATA(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_bdpr_reg_t hri_sdhc_get_BDPR_BUFDATA_bf(const void *const hw, hri_sdhc_bdpr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sdhc *)hw)->BDPR.reg;
	tmp = (tmp & SDHC_BDPR_BUFDATA(mask)) >> SDHC_BDPR_BUFDATA_Pos;
	return tmp;
}

static inline void hri_sdhc_write_BDPR_BUFDATA_bf(const void *const hw, hri_sdhc_bdpr_reg_t data)
{
	uint32_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->BDPR.reg;
	tmp &= ~SDHC_BDPR_BUFDATA_Msk;
	tmp |= SDHC_BDPR_BUFDATA(data);
	((Sdhc *)hw)->BDPR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_BDPR_BUFDATA_bf(const void *const hw, hri_sdhc_bdpr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->BDPR.reg &= ~SDHC_BDPR_BUFDATA(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_BDPR_BUFDATA_bf(const void *const hw, hri_sdhc_bdpr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->BDPR.reg ^= SDHC_BDPR_BUFDATA(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_bdpr_reg_t hri_sdhc_read_BDPR_BUFDATA_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sdhc *)hw)->BDPR.reg;
	tmp = (tmp & SDHC_BDPR_BUFDATA_Msk) >> SDHC_BDPR_BUFDATA_Pos;
	return tmp;
}

static inline void hri_sdhc_set_BDPR_reg(const void *const hw, hri_sdhc_bdpr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->BDPR.reg |= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_bdpr_reg_t hri_sdhc_get_BDPR_reg(const void *const hw, hri_sdhc_bdpr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sdhc *)hw)->BDPR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sdhc_write_BDPR_reg(const void *const hw, hri_sdhc_bdpr_reg_t data)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->BDPR.reg = data;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_BDPR_reg(const void *const hw, hri_sdhc_bdpr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->BDPR.reg &= ~mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_BDPR_reg(const void *const hw, hri_sdhc_bdpr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->BDPR.reg ^= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_bdpr_reg_t hri_sdhc_read_BDPR_reg(const void *const hw)
{
	return ((Sdhc *)hw)->BDPR.reg;
}

static inline void hri_sdhc_set_PCR_SDBPWR_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->PCR.reg |= SDHC_PCR_SDBPWR;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_PCR_SDBPWR_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Sdhc *)hw)->PCR.reg;
	tmp = (tmp & SDHC_PCR_SDBPWR) >> SDHC_PCR_SDBPWR_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_PCR_SDBPWR_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->PCR.reg;
	tmp &= ~SDHC_PCR_SDBPWR;
	tmp |= value << SDHC_PCR_SDBPWR_Pos;
	((Sdhc *)hw)->PCR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_PCR_SDBPWR_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->PCR.reg &= ~SDHC_PCR_SDBPWR;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_PCR_SDBPWR_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->PCR.reg ^= SDHC_PCR_SDBPWR;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_PCR_SDBVSEL_bf(const void *const hw, hri_sdhc_pcr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->PCR.reg |= SDHC_PCR_SDBVSEL(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_pcr_reg_t hri_sdhc_get_PCR_SDBVSEL_bf(const void *const hw, hri_sdhc_pcr_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Sdhc *)hw)->PCR.reg;
	tmp = (tmp & SDHC_PCR_SDBVSEL(mask)) >> SDHC_PCR_SDBVSEL_Pos;
	return tmp;
}

static inline void hri_sdhc_write_PCR_SDBVSEL_bf(const void *const hw, hri_sdhc_pcr_reg_t data)
{
	uint8_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->PCR.reg;
	tmp &= ~SDHC_PCR_SDBVSEL_Msk;
	tmp |= SDHC_PCR_SDBVSEL(data);
	((Sdhc *)hw)->PCR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_PCR_SDBVSEL_bf(const void *const hw, hri_sdhc_pcr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->PCR.reg &= ~SDHC_PCR_SDBVSEL(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_PCR_SDBVSEL_bf(const void *const hw, hri_sdhc_pcr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->PCR.reg ^= SDHC_PCR_SDBVSEL(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_pcr_reg_t hri_sdhc_read_PCR_SDBVSEL_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Sdhc *)hw)->PCR.reg;
	tmp = (tmp & SDHC_PCR_SDBVSEL_Msk) >> SDHC_PCR_SDBVSEL_Pos;
	return tmp;
}

static inline void hri_sdhc_set_PCR_reg(const void *const hw, hri_sdhc_pcr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->PCR.reg |= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_pcr_reg_t hri_sdhc_get_PCR_reg(const void *const hw, hri_sdhc_pcr_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Sdhc *)hw)->PCR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sdhc_write_PCR_reg(const void *const hw, hri_sdhc_pcr_reg_t data)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->PCR.reg = data;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_PCR_reg(const void *const hw, hri_sdhc_pcr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->PCR.reg &= ~mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_PCR_reg(const void *const hw, hri_sdhc_pcr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->PCR.reg ^= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_pcr_reg_t hri_sdhc_read_PCR_reg(const void *const hw)
{
	return ((Sdhc *)hw)->PCR.reg;
}

static inline void hri_sdhc_set_WCR_WKENCINT_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->WCR.reg |= SDHC_WCR_WKENCINT;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_WCR_WKENCINT_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Sdhc *)hw)->WCR.reg;
	tmp = (tmp & SDHC_WCR_WKENCINT) >> SDHC_WCR_WKENCINT_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_WCR_WKENCINT_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->WCR.reg;
	tmp &= ~SDHC_WCR_WKENCINT;
	tmp |= value << SDHC_WCR_WKENCINT_Pos;
	((Sdhc *)hw)->WCR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_WCR_WKENCINT_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->WCR.reg &= ~SDHC_WCR_WKENCINT;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_WCR_WKENCINT_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->WCR.reg ^= SDHC_WCR_WKENCINT;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_WCR_WKENCINS_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->WCR.reg |= SDHC_WCR_WKENCINS;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_WCR_WKENCINS_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Sdhc *)hw)->WCR.reg;
	tmp = (tmp & SDHC_WCR_WKENCINS) >> SDHC_WCR_WKENCINS_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_WCR_WKENCINS_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->WCR.reg;
	tmp &= ~SDHC_WCR_WKENCINS;
	tmp |= value << SDHC_WCR_WKENCINS_Pos;
	((Sdhc *)hw)->WCR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_WCR_WKENCINS_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->WCR.reg &= ~SDHC_WCR_WKENCINS;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_WCR_WKENCINS_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->WCR.reg ^= SDHC_WCR_WKENCINS;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_WCR_WKENCREM_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->WCR.reg |= SDHC_WCR_WKENCREM;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_WCR_WKENCREM_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Sdhc *)hw)->WCR.reg;
	tmp = (tmp & SDHC_WCR_WKENCREM) >> SDHC_WCR_WKENCREM_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_WCR_WKENCREM_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->WCR.reg;
	tmp &= ~SDHC_WCR_WKENCREM;
	tmp |= value << SDHC_WCR_WKENCREM_Pos;
	((Sdhc *)hw)->WCR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_WCR_WKENCREM_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->WCR.reg &= ~SDHC_WCR_WKENCREM;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_WCR_WKENCREM_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->WCR.reg ^= SDHC_WCR_WKENCREM;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_WCR_reg(const void *const hw, hri_sdhc_wcr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->WCR.reg |= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_wcr_reg_t hri_sdhc_get_WCR_reg(const void *const hw, hri_sdhc_wcr_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Sdhc *)hw)->WCR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sdhc_write_WCR_reg(const void *const hw, hri_sdhc_wcr_reg_t data)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->WCR.reg = data;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_WCR_reg(const void *const hw, hri_sdhc_wcr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->WCR.reg &= ~mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_WCR_reg(const void *const hw, hri_sdhc_wcr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->WCR.reg ^= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_wcr_reg_t hri_sdhc_read_WCR_reg(const void *const hw)
{
	return ((Sdhc *)hw)->WCR.reg;
}

static inline void hri_sdhc_set_CCR_INTCLKEN_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->CCR.reg |= SDHC_CCR_INTCLKEN;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_CCR_INTCLKEN_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->CCR.reg;
	tmp = (tmp & SDHC_CCR_INTCLKEN) >> SDHC_CCR_INTCLKEN_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_CCR_INTCLKEN_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->CCR.reg;
	tmp &= ~SDHC_CCR_INTCLKEN;
	tmp |= value << SDHC_CCR_INTCLKEN_Pos;
	((Sdhc *)hw)->CCR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_CCR_INTCLKEN_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->CCR.reg &= ~SDHC_CCR_INTCLKEN;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_CCR_INTCLKEN_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->CCR.reg ^= SDHC_CCR_INTCLKEN;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_CCR_INTCLKS_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->CCR.reg |= SDHC_CCR_INTCLKS;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_CCR_INTCLKS_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->CCR.reg;
	tmp = (tmp & SDHC_CCR_INTCLKS) >> SDHC_CCR_INTCLKS_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_CCR_INTCLKS_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->CCR.reg;
	tmp &= ~SDHC_CCR_INTCLKS;
	tmp |= value << SDHC_CCR_INTCLKS_Pos;
	((Sdhc *)hw)->CCR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_CCR_INTCLKS_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->CCR.reg &= ~SDHC_CCR_INTCLKS;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_CCR_INTCLKS_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->CCR.reg ^= SDHC_CCR_INTCLKS;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_CCR_SDCLKEN_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->CCR.reg |= SDHC_CCR_SDCLKEN;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_CCR_SDCLKEN_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->CCR.reg;
	tmp = (tmp & SDHC_CCR_SDCLKEN) >> SDHC_CCR_SDCLKEN_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_CCR_SDCLKEN_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->CCR.reg;
	tmp &= ~SDHC_CCR_SDCLKEN;
	tmp |= value << SDHC_CCR_SDCLKEN_Pos;
	((Sdhc *)hw)->CCR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_CCR_SDCLKEN_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->CCR.reg &= ~SDHC_CCR_SDCLKEN;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_CCR_SDCLKEN_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->CCR.reg ^= SDHC_CCR_SDCLKEN;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_CCR_CLKGSEL_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->CCR.reg |= SDHC_CCR_CLKGSEL;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_CCR_CLKGSEL_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->CCR.reg;
	tmp = (tmp & SDHC_CCR_CLKGSEL) >> SDHC_CCR_CLKGSEL_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_CCR_CLKGSEL_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->CCR.reg;
	tmp &= ~SDHC_CCR_CLKGSEL;
	tmp |= value << SDHC_CCR_CLKGSEL_Pos;
	((Sdhc *)hw)->CCR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_CCR_CLKGSEL_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->CCR.reg &= ~SDHC_CCR_CLKGSEL;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_CCR_CLKGSEL_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->CCR.reg ^= SDHC_CCR_CLKGSEL;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_CCR_USDCLKFSEL_bf(const void *const hw, hri_sdhc_ccr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->CCR.reg |= SDHC_CCR_USDCLKFSEL(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_ccr_reg_t hri_sdhc_get_CCR_USDCLKFSEL_bf(const void *const hw, hri_sdhc_ccr_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->CCR.reg;
	tmp = (tmp & SDHC_CCR_USDCLKFSEL(mask)) >> SDHC_CCR_USDCLKFSEL_Pos;
	return tmp;
}

static inline void hri_sdhc_write_CCR_USDCLKFSEL_bf(const void *const hw, hri_sdhc_ccr_reg_t data)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->CCR.reg;
	tmp &= ~SDHC_CCR_USDCLKFSEL_Msk;
	tmp |= SDHC_CCR_USDCLKFSEL(data);
	((Sdhc *)hw)->CCR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_CCR_USDCLKFSEL_bf(const void *const hw, hri_sdhc_ccr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->CCR.reg &= ~SDHC_CCR_USDCLKFSEL(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_CCR_USDCLKFSEL_bf(const void *const hw, hri_sdhc_ccr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->CCR.reg ^= SDHC_CCR_USDCLKFSEL(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_ccr_reg_t hri_sdhc_read_CCR_USDCLKFSEL_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->CCR.reg;
	tmp = (tmp & SDHC_CCR_USDCLKFSEL_Msk) >> SDHC_CCR_USDCLKFSEL_Pos;
	return tmp;
}

static inline void hri_sdhc_set_CCR_SDCLKFSEL_bf(const void *const hw, hri_sdhc_ccr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->CCR.reg |= SDHC_CCR_SDCLKFSEL(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_ccr_reg_t hri_sdhc_get_CCR_SDCLKFSEL_bf(const void *const hw, hri_sdhc_ccr_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->CCR.reg;
	tmp = (tmp & SDHC_CCR_SDCLKFSEL(mask)) >> SDHC_CCR_SDCLKFSEL_Pos;
	return tmp;
}

static inline void hri_sdhc_write_CCR_SDCLKFSEL_bf(const void *const hw, hri_sdhc_ccr_reg_t data)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->CCR.reg;
	tmp &= ~SDHC_CCR_SDCLKFSEL_Msk;
	tmp |= SDHC_CCR_SDCLKFSEL(data);
	((Sdhc *)hw)->CCR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_CCR_SDCLKFSEL_bf(const void *const hw, hri_sdhc_ccr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->CCR.reg &= ~SDHC_CCR_SDCLKFSEL(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_CCR_SDCLKFSEL_bf(const void *const hw, hri_sdhc_ccr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->CCR.reg ^= SDHC_CCR_SDCLKFSEL(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_ccr_reg_t hri_sdhc_read_CCR_SDCLKFSEL_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->CCR.reg;
	tmp = (tmp & SDHC_CCR_SDCLKFSEL_Msk) >> SDHC_CCR_SDCLKFSEL_Pos;
	return tmp;
}

static inline void hri_sdhc_set_CCR_reg(const void *const hw, hri_sdhc_ccr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->CCR.reg |= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_ccr_reg_t hri_sdhc_get_CCR_reg(const void *const hw, hri_sdhc_ccr_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->CCR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sdhc_write_CCR_reg(const void *const hw, hri_sdhc_ccr_reg_t data)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->CCR.reg = data;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_CCR_reg(const void *const hw, hri_sdhc_ccr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->CCR.reg &= ~mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_CCR_reg(const void *const hw, hri_sdhc_ccr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->CCR.reg ^= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_ccr_reg_t hri_sdhc_read_CCR_reg(const void *const hw)
{
	return ((Sdhc *)hw)->CCR.reg;
}

static inline void hri_sdhc_set_TCR_DTCVAL_bf(const void *const hw, hri_sdhc_tcr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->TCR.reg |= SDHC_TCR_DTCVAL(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_tcr_reg_t hri_sdhc_get_TCR_DTCVAL_bf(const void *const hw, hri_sdhc_tcr_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Sdhc *)hw)->TCR.reg;
	tmp = (tmp & SDHC_TCR_DTCVAL(mask)) >> SDHC_TCR_DTCVAL_Pos;
	return tmp;
}

static inline void hri_sdhc_write_TCR_DTCVAL_bf(const void *const hw, hri_sdhc_tcr_reg_t data)
{
	uint8_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->TCR.reg;
	tmp &= ~SDHC_TCR_DTCVAL_Msk;
	tmp |= SDHC_TCR_DTCVAL(data);
	((Sdhc *)hw)->TCR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_TCR_DTCVAL_bf(const void *const hw, hri_sdhc_tcr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->TCR.reg &= ~SDHC_TCR_DTCVAL(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_TCR_DTCVAL_bf(const void *const hw, hri_sdhc_tcr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->TCR.reg ^= SDHC_TCR_DTCVAL(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_tcr_reg_t hri_sdhc_read_TCR_DTCVAL_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Sdhc *)hw)->TCR.reg;
	tmp = (tmp & SDHC_TCR_DTCVAL_Msk) >> SDHC_TCR_DTCVAL_Pos;
	return tmp;
}

static inline void hri_sdhc_set_TCR_reg(const void *const hw, hri_sdhc_tcr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->TCR.reg |= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_tcr_reg_t hri_sdhc_get_TCR_reg(const void *const hw, hri_sdhc_tcr_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Sdhc *)hw)->TCR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sdhc_write_TCR_reg(const void *const hw, hri_sdhc_tcr_reg_t data)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->TCR.reg = data;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_TCR_reg(const void *const hw, hri_sdhc_tcr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->TCR.reg &= ~mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_TCR_reg(const void *const hw, hri_sdhc_tcr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->TCR.reg ^= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_tcr_reg_t hri_sdhc_read_TCR_reg(const void *const hw)
{
	return ((Sdhc *)hw)->TCR.reg;
}

static inline void hri_sdhc_set_SRR_SWRSTALL_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->SRR.reg |= SDHC_SRR_SWRSTALL;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_SRR_SWRSTALL_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Sdhc *)hw)->SRR.reg;
	tmp = (tmp & SDHC_SRR_SWRSTALL) >> SDHC_SRR_SWRSTALL_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_SRR_SWRSTALL_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->SRR.reg;
	tmp &= ~SDHC_SRR_SWRSTALL;
	tmp |= value << SDHC_SRR_SWRSTALL_Pos;
	((Sdhc *)hw)->SRR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_SRR_SWRSTALL_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->SRR.reg &= ~SDHC_SRR_SWRSTALL;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_SRR_SWRSTALL_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->SRR.reg ^= SDHC_SRR_SWRSTALL;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_SRR_SWRSTCMD_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->SRR.reg |= SDHC_SRR_SWRSTCMD;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_SRR_SWRSTCMD_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Sdhc *)hw)->SRR.reg;
	tmp = (tmp & SDHC_SRR_SWRSTCMD) >> SDHC_SRR_SWRSTCMD_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_SRR_SWRSTCMD_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->SRR.reg;
	tmp &= ~SDHC_SRR_SWRSTCMD;
	tmp |= value << SDHC_SRR_SWRSTCMD_Pos;
	((Sdhc *)hw)->SRR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_SRR_SWRSTCMD_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->SRR.reg &= ~SDHC_SRR_SWRSTCMD;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_SRR_SWRSTCMD_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->SRR.reg ^= SDHC_SRR_SWRSTCMD;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_SRR_SWRSTDAT_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->SRR.reg |= SDHC_SRR_SWRSTDAT;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_SRR_SWRSTDAT_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Sdhc *)hw)->SRR.reg;
	tmp = (tmp & SDHC_SRR_SWRSTDAT) >> SDHC_SRR_SWRSTDAT_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_SRR_SWRSTDAT_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->SRR.reg;
	tmp &= ~SDHC_SRR_SWRSTDAT;
	tmp |= value << SDHC_SRR_SWRSTDAT_Pos;
	((Sdhc *)hw)->SRR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_SRR_SWRSTDAT_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->SRR.reg &= ~SDHC_SRR_SWRSTDAT;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_SRR_SWRSTDAT_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->SRR.reg ^= SDHC_SRR_SWRSTDAT;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_SRR_reg(const void *const hw, hri_sdhc_srr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->SRR.reg |= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_srr_reg_t hri_sdhc_get_SRR_reg(const void *const hw, hri_sdhc_srr_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Sdhc *)hw)->SRR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sdhc_write_SRR_reg(const void *const hw, hri_sdhc_srr_reg_t data)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->SRR.reg = data;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_SRR_reg(const void *const hw, hri_sdhc_srr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->SRR.reg &= ~mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_SRR_reg(const void *const hw, hri_sdhc_srr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->SRR.reg ^= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_srr_reg_t hri_sdhc_read_SRR_reg(const void *const hw)
{
	return ((Sdhc *)hw)->SRR.reg;
}

static inline void hri_sdhc_set_ASAR_ADMASA_bf(const void *const hw, uint8_t index, hri_sdhc_asar_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->ASAR[index].reg |= SDHC_ASAR_ADMASA(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_asar_reg_t hri_sdhc_get_ASAR_ADMASA_bf(const void *const hw, uint8_t index,
                                                              hri_sdhc_asar_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sdhc *)hw)->ASAR[index].reg;
	tmp = (tmp & SDHC_ASAR_ADMASA(mask)) >> SDHC_ASAR_ADMASA_Pos;
	return tmp;
}

static inline void hri_sdhc_write_ASAR_ADMASA_bf(const void *const hw, uint8_t index, hri_sdhc_asar_reg_t data)
{
	uint32_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->ASAR[index].reg;
	tmp &= ~SDHC_ASAR_ADMASA_Msk;
	tmp |= SDHC_ASAR_ADMASA(data);
	((Sdhc *)hw)->ASAR[index].reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_ASAR_ADMASA_bf(const void *const hw, uint8_t index, hri_sdhc_asar_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->ASAR[index].reg &= ~SDHC_ASAR_ADMASA(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_ASAR_ADMASA_bf(const void *const hw, uint8_t index, hri_sdhc_asar_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->ASAR[index].reg ^= SDHC_ASAR_ADMASA(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_asar_reg_t hri_sdhc_read_ASAR_ADMASA_bf(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Sdhc *)hw)->ASAR[index].reg;
	tmp = (tmp & SDHC_ASAR_ADMASA_Msk) >> SDHC_ASAR_ADMASA_Pos;
	return tmp;
}

static inline void hri_sdhc_set_ASAR_reg(const void *const hw, uint8_t index, hri_sdhc_asar_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->ASAR[index].reg |= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_asar_reg_t hri_sdhc_get_ASAR_reg(const void *const hw, uint8_t index, hri_sdhc_asar_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sdhc *)hw)->ASAR[index].reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sdhc_write_ASAR_reg(const void *const hw, uint8_t index, hri_sdhc_asar_reg_t data)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->ASAR[index].reg = data;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_ASAR_reg(const void *const hw, uint8_t index, hri_sdhc_asar_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->ASAR[index].reg &= ~mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_ASAR_reg(const void *const hw, uint8_t index, hri_sdhc_asar_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->ASAR[index].reg ^= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_asar_reg_t hri_sdhc_read_ASAR_reg(const void *const hw, uint8_t index)
{
	return ((Sdhc *)hw)->ASAR[index].reg;
}

static inline void hri_sdhc_set_PVR_CLKGSEL_bit(const void *const hw, uint8_t index)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->PVR[index].reg |= SDHC_PVR_CLKGSEL;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_PVR_CLKGSEL_bit(const void *const hw, uint8_t index)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->PVR[index].reg;
	tmp = (tmp & SDHC_PVR_CLKGSEL) >> SDHC_PVR_CLKGSEL_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_PVR_CLKGSEL_bit(const void *const hw, uint8_t index, bool value)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->PVR[index].reg;
	tmp &= ~SDHC_PVR_CLKGSEL;
	tmp |= value << SDHC_PVR_CLKGSEL_Pos;
	((Sdhc *)hw)->PVR[index].reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_PVR_CLKGSEL_bit(const void *const hw, uint8_t index)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->PVR[index].reg &= ~SDHC_PVR_CLKGSEL;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_PVR_CLKGSEL_bit(const void *const hw, uint8_t index)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->PVR[index].reg ^= SDHC_PVR_CLKGSEL;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_PVR_SDCLKFSEL_bf(const void *const hw, uint8_t index, hri_sdhc_pvr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->PVR[index].reg |= SDHC_PVR_SDCLKFSEL(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_pvr_reg_t hri_sdhc_get_PVR_SDCLKFSEL_bf(const void *const hw, uint8_t index,
                                                               hri_sdhc_pvr_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->PVR[index].reg;
	tmp = (tmp & SDHC_PVR_SDCLKFSEL(mask)) >> SDHC_PVR_SDCLKFSEL_Pos;
	return tmp;
}

static inline void hri_sdhc_write_PVR_SDCLKFSEL_bf(const void *const hw, uint8_t index, hri_sdhc_pvr_reg_t data)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->PVR[index].reg;
	tmp &= ~SDHC_PVR_SDCLKFSEL_Msk;
	tmp |= SDHC_PVR_SDCLKFSEL(data);
	((Sdhc *)hw)->PVR[index].reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_PVR_SDCLKFSEL_bf(const void *const hw, uint8_t index, hri_sdhc_pvr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->PVR[index].reg &= ~SDHC_PVR_SDCLKFSEL(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_PVR_SDCLKFSEL_bf(const void *const hw, uint8_t index, hri_sdhc_pvr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->PVR[index].reg ^= SDHC_PVR_SDCLKFSEL(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_pvr_reg_t hri_sdhc_read_PVR_SDCLKFSEL_bf(const void *const hw, uint8_t index)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->PVR[index].reg;
	tmp = (tmp & SDHC_PVR_SDCLKFSEL_Msk) >> SDHC_PVR_SDCLKFSEL_Pos;
	return tmp;
}

static inline void hri_sdhc_set_PVR_DRVSEL_bf(const void *const hw, uint8_t index, hri_sdhc_pvr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->PVR[index].reg |= SDHC_PVR_DRVSEL(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_pvr_reg_t hri_sdhc_get_PVR_DRVSEL_bf(const void *const hw, uint8_t index,
                                                            hri_sdhc_pvr_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->PVR[index].reg;
	tmp = (tmp & SDHC_PVR_DRVSEL(mask)) >> SDHC_PVR_DRVSEL_Pos;
	return tmp;
}

static inline void hri_sdhc_write_PVR_DRVSEL_bf(const void *const hw, uint8_t index, hri_sdhc_pvr_reg_t data)
{
	uint16_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->PVR[index].reg;
	tmp &= ~SDHC_PVR_DRVSEL_Msk;
	tmp |= SDHC_PVR_DRVSEL(data);
	((Sdhc *)hw)->PVR[index].reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_PVR_DRVSEL_bf(const void *const hw, uint8_t index, hri_sdhc_pvr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->PVR[index].reg &= ~SDHC_PVR_DRVSEL(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_PVR_DRVSEL_bf(const void *const hw, uint8_t index, hri_sdhc_pvr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->PVR[index].reg ^= SDHC_PVR_DRVSEL(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_pvr_reg_t hri_sdhc_read_PVR_DRVSEL_bf(const void *const hw, uint8_t index)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->PVR[index].reg;
	tmp = (tmp & SDHC_PVR_DRVSEL_Msk) >> SDHC_PVR_DRVSEL_Pos;
	return tmp;
}

static inline void hri_sdhc_set_PVR_reg(const void *const hw, uint8_t index, hri_sdhc_pvr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->PVR[index].reg |= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_pvr_reg_t hri_sdhc_get_PVR_reg(const void *const hw, uint8_t index, hri_sdhc_pvr_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->PVR[index].reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sdhc_write_PVR_reg(const void *const hw, uint8_t index, hri_sdhc_pvr_reg_t data)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->PVR[index].reg = data;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_PVR_reg(const void *const hw, uint8_t index, hri_sdhc_pvr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->PVR[index].reg &= ~mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_PVR_reg(const void *const hw, uint8_t index, hri_sdhc_pvr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->PVR[index].reg ^= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_pvr_reg_t hri_sdhc_read_PVR_reg(const void *const hw, uint8_t index)
{
	return ((Sdhc *)hw)->PVR[index].reg;
}

static inline void hri_sdhc_set_MC1R_DDR_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->MC1R.reg |= SDHC_MC1R_DDR;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_MC1R_DDR_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Sdhc *)hw)->MC1R.reg;
	tmp = (tmp & SDHC_MC1R_DDR) >> SDHC_MC1R_DDR_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_MC1R_DDR_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->MC1R.reg;
	tmp &= ~SDHC_MC1R_DDR;
	tmp |= value << SDHC_MC1R_DDR_Pos;
	((Sdhc *)hw)->MC1R.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_MC1R_DDR_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->MC1R.reg &= ~SDHC_MC1R_DDR;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_MC1R_DDR_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->MC1R.reg ^= SDHC_MC1R_DDR;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_MC1R_OPD_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->MC1R.reg |= SDHC_MC1R_OPD;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_MC1R_OPD_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Sdhc *)hw)->MC1R.reg;
	tmp = (tmp & SDHC_MC1R_OPD) >> SDHC_MC1R_OPD_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_MC1R_OPD_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->MC1R.reg;
	tmp &= ~SDHC_MC1R_OPD;
	tmp |= value << SDHC_MC1R_OPD_Pos;
	((Sdhc *)hw)->MC1R.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_MC1R_OPD_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->MC1R.reg &= ~SDHC_MC1R_OPD;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_MC1R_OPD_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->MC1R.reg ^= SDHC_MC1R_OPD;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_MC1R_BOOTA_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->MC1R.reg |= SDHC_MC1R_BOOTA;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_MC1R_BOOTA_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Sdhc *)hw)->MC1R.reg;
	tmp = (tmp & SDHC_MC1R_BOOTA) >> SDHC_MC1R_BOOTA_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_MC1R_BOOTA_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->MC1R.reg;
	tmp &= ~SDHC_MC1R_BOOTA;
	tmp |= value << SDHC_MC1R_BOOTA_Pos;
	((Sdhc *)hw)->MC1R.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_MC1R_BOOTA_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->MC1R.reg &= ~SDHC_MC1R_BOOTA;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_MC1R_BOOTA_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->MC1R.reg ^= SDHC_MC1R_BOOTA;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_MC1R_RSTN_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->MC1R.reg |= SDHC_MC1R_RSTN;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_MC1R_RSTN_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Sdhc *)hw)->MC1R.reg;
	tmp = (tmp & SDHC_MC1R_RSTN) >> SDHC_MC1R_RSTN_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_MC1R_RSTN_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->MC1R.reg;
	tmp &= ~SDHC_MC1R_RSTN;
	tmp |= value << SDHC_MC1R_RSTN_Pos;
	((Sdhc *)hw)->MC1R.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_MC1R_RSTN_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->MC1R.reg &= ~SDHC_MC1R_RSTN;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_MC1R_RSTN_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->MC1R.reg ^= SDHC_MC1R_RSTN;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_MC1R_FCD_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->MC1R.reg |= SDHC_MC1R_FCD;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_MC1R_FCD_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Sdhc *)hw)->MC1R.reg;
	tmp = (tmp & SDHC_MC1R_FCD) >> SDHC_MC1R_FCD_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_MC1R_FCD_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->MC1R.reg;
	tmp &= ~SDHC_MC1R_FCD;
	tmp |= value << SDHC_MC1R_FCD_Pos;
	((Sdhc *)hw)->MC1R.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_MC1R_FCD_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->MC1R.reg &= ~SDHC_MC1R_FCD;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_MC1R_FCD_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->MC1R.reg ^= SDHC_MC1R_FCD;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_MC1R_CMDTYP_bf(const void *const hw, hri_sdhc_mc1r_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->MC1R.reg |= SDHC_MC1R_CMDTYP(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_mc1r_reg_t hri_sdhc_get_MC1R_CMDTYP_bf(const void *const hw, hri_sdhc_mc1r_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Sdhc *)hw)->MC1R.reg;
	tmp = (tmp & SDHC_MC1R_CMDTYP(mask)) >> SDHC_MC1R_CMDTYP_Pos;
	return tmp;
}

static inline void hri_sdhc_write_MC1R_CMDTYP_bf(const void *const hw, hri_sdhc_mc1r_reg_t data)
{
	uint8_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->MC1R.reg;
	tmp &= ~SDHC_MC1R_CMDTYP_Msk;
	tmp |= SDHC_MC1R_CMDTYP(data);
	((Sdhc *)hw)->MC1R.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_MC1R_CMDTYP_bf(const void *const hw, hri_sdhc_mc1r_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->MC1R.reg &= ~SDHC_MC1R_CMDTYP(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_MC1R_CMDTYP_bf(const void *const hw, hri_sdhc_mc1r_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->MC1R.reg ^= SDHC_MC1R_CMDTYP(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_mc1r_reg_t hri_sdhc_read_MC1R_CMDTYP_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Sdhc *)hw)->MC1R.reg;
	tmp = (tmp & SDHC_MC1R_CMDTYP_Msk) >> SDHC_MC1R_CMDTYP_Pos;
	return tmp;
}

static inline void hri_sdhc_set_MC1R_reg(const void *const hw, hri_sdhc_mc1r_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->MC1R.reg |= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_mc1r_reg_t hri_sdhc_get_MC1R_reg(const void *const hw, hri_sdhc_mc1r_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Sdhc *)hw)->MC1R.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sdhc_write_MC1R_reg(const void *const hw, hri_sdhc_mc1r_reg_t data)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->MC1R.reg = data;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_MC1R_reg(const void *const hw, hri_sdhc_mc1r_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->MC1R.reg &= ~mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_MC1R_reg(const void *const hw, hri_sdhc_mc1r_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->MC1R.reg ^= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_mc1r_reg_t hri_sdhc_read_MC1R_reg(const void *const hw)
{
	return ((Sdhc *)hw)->MC1R.reg;
}

static inline void hri_sdhc_set_ACR_BMAX_bf(const void *const hw, hri_sdhc_acr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->ACR.reg |= SDHC_ACR_BMAX(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_acr_reg_t hri_sdhc_get_ACR_BMAX_bf(const void *const hw, hri_sdhc_acr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sdhc *)hw)->ACR.reg;
	tmp = (tmp & SDHC_ACR_BMAX(mask)) >> SDHC_ACR_BMAX_Pos;
	return tmp;
}

static inline void hri_sdhc_write_ACR_BMAX_bf(const void *const hw, hri_sdhc_acr_reg_t data)
{
	uint32_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->ACR.reg;
	tmp &= ~SDHC_ACR_BMAX_Msk;
	tmp |= SDHC_ACR_BMAX(data);
	((Sdhc *)hw)->ACR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_ACR_BMAX_bf(const void *const hw, hri_sdhc_acr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->ACR.reg &= ~SDHC_ACR_BMAX(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_ACR_BMAX_bf(const void *const hw, hri_sdhc_acr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->ACR.reg ^= SDHC_ACR_BMAX(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_acr_reg_t hri_sdhc_read_ACR_BMAX_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sdhc *)hw)->ACR.reg;
	tmp = (tmp & SDHC_ACR_BMAX_Msk) >> SDHC_ACR_BMAX_Pos;
	return tmp;
}

static inline void hri_sdhc_set_ACR_reg(const void *const hw, hri_sdhc_acr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->ACR.reg |= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_acr_reg_t hri_sdhc_get_ACR_reg(const void *const hw, hri_sdhc_acr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sdhc *)hw)->ACR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sdhc_write_ACR_reg(const void *const hw, hri_sdhc_acr_reg_t data)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->ACR.reg = data;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_ACR_reg(const void *const hw, hri_sdhc_acr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->ACR.reg &= ~mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_ACR_reg(const void *const hw, hri_sdhc_acr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->ACR.reg ^= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_acr_reg_t hri_sdhc_read_ACR_reg(const void *const hw)
{
	return ((Sdhc *)hw)->ACR.reg;
}

static inline void hri_sdhc_set_CC2R_FSDCLKD_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->CC2R.reg |= SDHC_CC2R_FSDCLKD;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_CC2R_FSDCLKD_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sdhc *)hw)->CC2R.reg;
	tmp = (tmp & SDHC_CC2R_FSDCLKD) >> SDHC_CC2R_FSDCLKD_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_CC2R_FSDCLKD_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->CC2R.reg;
	tmp &= ~SDHC_CC2R_FSDCLKD;
	tmp |= value << SDHC_CC2R_FSDCLKD_Pos;
	((Sdhc *)hw)->CC2R.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_CC2R_FSDCLKD_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->CC2R.reg &= ~SDHC_CC2R_FSDCLKD;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_CC2R_FSDCLKD_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->CC2R.reg ^= SDHC_CC2R_FSDCLKD;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_CC2R_reg(const void *const hw, hri_sdhc_cc2r_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->CC2R.reg |= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_cc2r_reg_t hri_sdhc_get_CC2R_reg(const void *const hw, hri_sdhc_cc2r_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sdhc *)hw)->CC2R.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sdhc_write_CC2R_reg(const void *const hw, hri_sdhc_cc2r_reg_t data)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->CC2R.reg = data;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_CC2R_reg(const void *const hw, hri_sdhc_cc2r_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->CC2R.reg &= ~mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_CC2R_reg(const void *const hw, hri_sdhc_cc2r_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->CC2R.reg ^= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_cc2r_reg_t hri_sdhc_read_CC2R_reg(const void *const hw)
{
	return ((Sdhc *)hw)->CC2R.reg;
}

static inline void hri_sdhc_set_CACR_CAPWREN_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->CACR.reg |= SDHC_CACR_CAPWREN;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_CACR_CAPWREN_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sdhc *)hw)->CACR.reg;
	tmp = (tmp & SDHC_CACR_CAPWREN) >> SDHC_CACR_CAPWREN_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_CACR_CAPWREN_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->CACR.reg;
	tmp &= ~SDHC_CACR_CAPWREN;
	tmp |= value << SDHC_CACR_CAPWREN_Pos;
	((Sdhc *)hw)->CACR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_CACR_CAPWREN_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->CACR.reg &= ~SDHC_CACR_CAPWREN;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_CACR_CAPWREN_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->CACR.reg ^= SDHC_CACR_CAPWREN;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_CACR_KEY_bf(const void *const hw, hri_sdhc_cacr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->CACR.reg |= SDHC_CACR_KEY(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_cacr_reg_t hri_sdhc_get_CACR_KEY_bf(const void *const hw, hri_sdhc_cacr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sdhc *)hw)->CACR.reg;
	tmp = (tmp & SDHC_CACR_KEY(mask)) >> SDHC_CACR_KEY_Pos;
	return tmp;
}

static inline void hri_sdhc_write_CACR_KEY_bf(const void *const hw, hri_sdhc_cacr_reg_t data)
{
	uint32_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->CACR.reg;
	tmp &= ~SDHC_CACR_KEY_Msk;
	tmp |= SDHC_CACR_KEY(data);
	((Sdhc *)hw)->CACR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_CACR_KEY_bf(const void *const hw, hri_sdhc_cacr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->CACR.reg &= ~SDHC_CACR_KEY(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_CACR_KEY_bf(const void *const hw, hri_sdhc_cacr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->CACR.reg ^= SDHC_CACR_KEY(mask);
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_cacr_reg_t hri_sdhc_read_CACR_KEY_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Sdhc *)hw)->CACR.reg;
	tmp = (tmp & SDHC_CACR_KEY_Msk) >> SDHC_CACR_KEY_Pos;
	return tmp;
}

static inline void hri_sdhc_set_CACR_reg(const void *const hw, hri_sdhc_cacr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->CACR.reg |= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_cacr_reg_t hri_sdhc_get_CACR_reg(const void *const hw, hri_sdhc_cacr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sdhc *)hw)->CACR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sdhc_write_CACR_reg(const void *const hw, hri_sdhc_cacr_reg_t data)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->CACR.reg = data;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_CACR_reg(const void *const hw, hri_sdhc_cacr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->CACR.reg &= ~mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_CACR_reg(const void *const hw, hri_sdhc_cacr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->CACR.reg ^= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_cacr_reg_t hri_sdhc_read_CACR_reg(const void *const hw)
{
	return ((Sdhc *)hw)->CACR.reg;
}

static inline void hri_sdhc_set_DBGR_NIDBG_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->DBGR.reg |= SDHC_DBGR_NIDBG;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_sdhc_get_DBGR_NIDBG_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Sdhc *)hw)->DBGR.reg;
	tmp = (tmp & SDHC_DBGR_NIDBG) >> SDHC_DBGR_NIDBG_Pos;
	return (bool)tmp;
}

static inline void hri_sdhc_write_DBGR_NIDBG_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	SDHC_CRITICAL_SECTION_ENTER();
	tmp = ((Sdhc *)hw)->DBGR.reg;
	tmp &= ~SDHC_DBGR_NIDBG;
	tmp |= value << SDHC_DBGR_NIDBG_Pos;
	((Sdhc *)hw)->DBGR.reg = tmp;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_DBGR_NIDBG_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->DBGR.reg &= ~SDHC_DBGR_NIDBG;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_DBGR_NIDBG_bit(const void *const hw)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->DBGR.reg ^= SDHC_DBGR_NIDBG;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_set_DBGR_reg(const void *const hw, hri_sdhc_dbgr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->DBGR.reg |= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_dbgr_reg_t hri_sdhc_get_DBGR_reg(const void *const hw, hri_sdhc_dbgr_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Sdhc *)hw)->DBGR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_sdhc_write_DBGR_reg(const void *const hw, hri_sdhc_dbgr_reg_t data)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->DBGR.reg = data;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_clear_DBGR_reg(const void *const hw, hri_sdhc_dbgr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->DBGR.reg &= ~mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_sdhc_toggle_DBGR_reg(const void *const hw, hri_sdhc_dbgr_reg_t mask)
{
	SDHC_CRITICAL_SECTION_ENTER();
	((Sdhc *)hw)->DBGR.reg ^= mask;
	SDHC_CRITICAL_SECTION_LEAVE();
}

static inline hri_sdhc_dbgr_reg_t hri_sdhc_read_DBGR_reg(const void *const hw)
{
	return ((Sdhc *)hw)->DBGR.reg;
}

static inline hri_sdhc_rr_reg_t hri_sdhc_get_RR_CMDRESP_bf(const void *const hw, uint8_t index, hri_sdhc_rr_reg_t mask)
{
	return (((Sdhc *)hw)->RR[index].reg & SDHC_RR_CMDRESP(mask)) >> SDHC_RR_CMDRESP_Pos;
}

static inline hri_sdhc_rr_reg_t hri_sdhc_read_RR_CMDRESP_bf(const void *const hw, uint8_t index)
{
	return (((Sdhc *)hw)->RR[index].reg & SDHC_RR_CMDRESP_Msk) >> SDHC_RR_CMDRESP_Pos;
}

static inline hri_sdhc_rr_reg_t hri_sdhc_get_RR_reg(const void *const hw, uint8_t index, hri_sdhc_rr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sdhc *)hw)->RR[index].reg;
	tmp &= mask;
	return tmp;
}

static inline hri_sdhc_rr_reg_t hri_sdhc_read_RR_reg(const void *const hw, uint8_t index)
{
	return ((Sdhc *)hw)->RR[index].reg;
}

static inline bool hri_sdhc_get_PSR_CMDINHC_bit(const void *const hw)
{
	return (((Sdhc *)hw)->PSR.reg & SDHC_PSR_CMDINHC) >> SDHC_PSR_CMDINHC_Pos;
}

static inline bool hri_sdhc_get_PSR_CMDINHD_bit(const void *const hw)
{
	return (((Sdhc *)hw)->PSR.reg & SDHC_PSR_CMDINHD) >> SDHC_PSR_CMDINHD_Pos;
}

static inline bool hri_sdhc_get_PSR_DLACT_bit(const void *const hw)
{
	return (((Sdhc *)hw)->PSR.reg & SDHC_PSR_DLACT) >> SDHC_PSR_DLACT_Pos;
}

static inline bool hri_sdhc_get_PSR_RTREQ_bit(const void *const hw)
{
	return (((Sdhc *)hw)->PSR.reg & SDHC_PSR_RTREQ) >> SDHC_PSR_RTREQ_Pos;
}

static inline bool hri_sdhc_get_PSR_WTACT_bit(const void *const hw)
{
	return (((Sdhc *)hw)->PSR.reg & SDHC_PSR_WTACT) >> SDHC_PSR_WTACT_Pos;
}

static inline bool hri_sdhc_get_PSR_RTACT_bit(const void *const hw)
{
	return (((Sdhc *)hw)->PSR.reg & SDHC_PSR_RTACT) >> SDHC_PSR_RTACT_Pos;
}

static inline bool hri_sdhc_get_PSR_BUFWREN_bit(const void *const hw)
{
	return (((Sdhc *)hw)->PSR.reg & SDHC_PSR_BUFWREN) >> SDHC_PSR_BUFWREN_Pos;
}

static inline bool hri_sdhc_get_PSR_BUFRDEN_bit(const void *const hw)
{
	return (((Sdhc *)hw)->PSR.reg & SDHC_PSR_BUFRDEN) >> SDHC_PSR_BUFRDEN_Pos;
}

static inline bool hri_sdhc_get_PSR_CARDINS_bit(const void *const hw)
{
	return (((Sdhc *)hw)->PSR.reg & SDHC_PSR_CARDINS) >> SDHC_PSR_CARDINS_Pos;
}

static inline bool hri_sdhc_get_PSR_CARDSS_bit(const void *const hw)
{
	return (((Sdhc *)hw)->PSR.reg & SDHC_PSR_CARDSS) >> SDHC_PSR_CARDSS_Pos;
}

static inline bool hri_sdhc_get_PSR_CARDDPL_bit(const void *const hw)
{
	return (((Sdhc *)hw)->PSR.reg & SDHC_PSR_CARDDPL) >> SDHC_PSR_CARDDPL_Pos;
}

static inline bool hri_sdhc_get_PSR_WRPPL_bit(const void *const hw)
{
	return (((Sdhc *)hw)->PSR.reg & SDHC_PSR_WRPPL) >> SDHC_PSR_WRPPL_Pos;
}

static inline bool hri_sdhc_get_PSR_CMDLL_bit(const void *const hw)
{
	return (((Sdhc *)hw)->PSR.reg & SDHC_PSR_CMDLL) >> SDHC_PSR_CMDLL_Pos;
}

static inline hri_sdhc_psr_reg_t hri_sdhc_get_PSR_DATLL_bf(const void *const hw, hri_sdhc_psr_reg_t mask)
{
	return (((Sdhc *)hw)->PSR.reg & SDHC_PSR_DATLL(mask)) >> SDHC_PSR_DATLL_Pos;
}

static inline hri_sdhc_psr_reg_t hri_sdhc_read_PSR_DATLL_bf(const void *const hw)
{
	return (((Sdhc *)hw)->PSR.reg & SDHC_PSR_DATLL_Msk) >> SDHC_PSR_DATLL_Pos;
}

static inline hri_sdhc_psr_reg_t hri_sdhc_get_PSR_reg(const void *const hw, hri_sdhc_psr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sdhc *)hw)->PSR.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_sdhc_psr_reg_t hri_sdhc_read_PSR_reg(const void *const hw)
{
	return ((Sdhc *)hw)->PSR.reg;
}

static inline bool hri_sdhc_get_ACESR_ACMD12NE_bit(const void *const hw)
{
	return (((Sdhc *)hw)->ACESR.reg & SDHC_ACESR_ACMD12NE) >> SDHC_ACESR_ACMD12NE_Pos;
}

static inline bool hri_sdhc_get_ACESR_ACMDTEO_bit(const void *const hw)
{
	return (((Sdhc *)hw)->ACESR.reg & SDHC_ACESR_ACMDTEO) >> SDHC_ACESR_ACMDTEO_Pos;
}

static inline bool hri_sdhc_get_ACESR_ACMDCRC_bit(const void *const hw)
{
	return (((Sdhc *)hw)->ACESR.reg & SDHC_ACESR_ACMDCRC) >> SDHC_ACESR_ACMDCRC_Pos;
}

static inline bool hri_sdhc_get_ACESR_ACMDEND_bit(const void *const hw)
{
	return (((Sdhc *)hw)->ACESR.reg & SDHC_ACESR_ACMDEND) >> SDHC_ACESR_ACMDEND_Pos;
}

static inline bool hri_sdhc_get_ACESR_ACMDIDX_bit(const void *const hw)
{
	return (((Sdhc *)hw)->ACESR.reg & SDHC_ACESR_ACMDIDX) >> SDHC_ACESR_ACMDIDX_Pos;
}

static inline bool hri_sdhc_get_ACESR_CMDNI_bit(const void *const hw)
{
	return (((Sdhc *)hw)->ACESR.reg & SDHC_ACESR_CMDNI) >> SDHC_ACESR_CMDNI_Pos;
}

static inline hri_sdhc_acesr_reg_t hri_sdhc_get_ACESR_reg(const void *const hw, hri_sdhc_acesr_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->ACESR.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_sdhc_acesr_reg_t hri_sdhc_read_ACESR_reg(const void *const hw)
{
	return ((Sdhc *)hw)->ACESR.reg;
}

static inline bool hri_sdhc_get_CA0R_TEOCLKU_bit(const void *const hw)
{
	return (((Sdhc *)hw)->CA0R.reg & SDHC_CA0R_TEOCLKU) >> SDHC_CA0R_TEOCLKU_Pos;
}

static inline bool hri_sdhc_get_CA0R_ED8SUP_bit(const void *const hw)
{
	return (((Sdhc *)hw)->CA0R.reg & SDHC_CA0R_ED8SUP) >> SDHC_CA0R_ED8SUP_Pos;
}

static inline bool hri_sdhc_get_CA0R_ADMA2SUP_bit(const void *const hw)
{
	return (((Sdhc *)hw)->CA0R.reg & SDHC_CA0R_ADMA2SUP) >> SDHC_CA0R_ADMA2SUP_Pos;
}

static inline bool hri_sdhc_get_CA0R_HSSUP_bit(const void *const hw)
{
	return (((Sdhc *)hw)->CA0R.reg & SDHC_CA0R_HSSUP) >> SDHC_CA0R_HSSUP_Pos;
}

static inline bool hri_sdhc_get_CA0R_SDMASUP_bit(const void *const hw)
{
	return (((Sdhc *)hw)->CA0R.reg & SDHC_CA0R_SDMASUP) >> SDHC_CA0R_SDMASUP_Pos;
}

static inline bool hri_sdhc_get_CA0R_SRSUP_bit(const void *const hw)
{
	return (((Sdhc *)hw)->CA0R.reg & SDHC_CA0R_SRSUP) >> SDHC_CA0R_SRSUP_Pos;
}

static inline bool hri_sdhc_get_CA0R_V33VSUP_bit(const void *const hw)
{
	return (((Sdhc *)hw)->CA0R.reg & SDHC_CA0R_V33VSUP) >> SDHC_CA0R_V33VSUP_Pos;
}

static inline bool hri_sdhc_get_CA0R_V30VSUP_bit(const void *const hw)
{
	return (((Sdhc *)hw)->CA0R.reg & SDHC_CA0R_V30VSUP) >> SDHC_CA0R_V30VSUP_Pos;
}

static inline bool hri_sdhc_get_CA0R_V18VSUP_bit(const void *const hw)
{
	return (((Sdhc *)hw)->CA0R.reg & SDHC_CA0R_V18VSUP) >> SDHC_CA0R_V18VSUP_Pos;
}

static inline bool hri_sdhc_get_CA0R_SB64SUP_bit(const void *const hw)
{
	return (((Sdhc *)hw)->CA0R.reg & SDHC_CA0R_SB64SUP) >> SDHC_CA0R_SB64SUP_Pos;
}

static inline bool hri_sdhc_get_CA0R_ASINTSUP_bit(const void *const hw)
{
	return (((Sdhc *)hw)->CA0R.reg & SDHC_CA0R_ASINTSUP) >> SDHC_CA0R_ASINTSUP_Pos;
}

static inline hri_sdhc_ca0r_reg_t hri_sdhc_get_CA0R_TEOCLKF_bf(const void *const hw, hri_sdhc_ca0r_reg_t mask)
{
	return (((Sdhc *)hw)->CA0R.reg & SDHC_CA0R_TEOCLKF(mask)) >> SDHC_CA0R_TEOCLKF_Pos;
}

static inline hri_sdhc_ca0r_reg_t hri_sdhc_read_CA0R_TEOCLKF_bf(const void *const hw)
{
	return (((Sdhc *)hw)->CA0R.reg & SDHC_CA0R_TEOCLKF_Msk) >> SDHC_CA0R_TEOCLKF_Pos;
}

static inline hri_sdhc_ca0r_reg_t hri_sdhc_get_CA0R_BASECLKF_bf(const void *const hw, hri_sdhc_ca0r_reg_t mask)
{
	return (((Sdhc *)hw)->CA0R.reg & SDHC_CA0R_BASECLKF(mask)) >> SDHC_CA0R_BASECLKF_Pos;
}

static inline hri_sdhc_ca0r_reg_t hri_sdhc_read_CA0R_BASECLKF_bf(const void *const hw)
{
	return (((Sdhc *)hw)->CA0R.reg & SDHC_CA0R_BASECLKF_Msk) >> SDHC_CA0R_BASECLKF_Pos;
}

static inline hri_sdhc_ca0r_reg_t hri_sdhc_get_CA0R_MAXBLKL_bf(const void *const hw, hri_sdhc_ca0r_reg_t mask)
{
	return (((Sdhc *)hw)->CA0R.reg & SDHC_CA0R_MAXBLKL(mask)) >> SDHC_CA0R_MAXBLKL_Pos;
}

static inline hri_sdhc_ca0r_reg_t hri_sdhc_read_CA0R_MAXBLKL_bf(const void *const hw)
{
	return (((Sdhc *)hw)->CA0R.reg & SDHC_CA0R_MAXBLKL_Msk) >> SDHC_CA0R_MAXBLKL_Pos;
}

static inline hri_sdhc_ca0r_reg_t hri_sdhc_get_CA0R_SLTYPE_bf(const void *const hw, hri_sdhc_ca0r_reg_t mask)
{
	return (((Sdhc *)hw)->CA0R.reg & SDHC_CA0R_SLTYPE(mask)) >> SDHC_CA0R_SLTYPE_Pos;
}

static inline hri_sdhc_ca0r_reg_t hri_sdhc_read_CA0R_SLTYPE_bf(const void *const hw)
{
	return (((Sdhc *)hw)->CA0R.reg & SDHC_CA0R_SLTYPE_Msk) >> SDHC_CA0R_SLTYPE_Pos;
}

static inline hri_sdhc_ca0r_reg_t hri_sdhc_get_CA0R_reg(const void *const hw, hri_sdhc_ca0r_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sdhc *)hw)->CA0R.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_sdhc_ca0r_reg_t hri_sdhc_read_CA0R_reg(const void *const hw)
{
	return ((Sdhc *)hw)->CA0R.reg;
}

static inline bool hri_sdhc_get_CA1R_SDR50SUP_bit(const void *const hw)
{
	return (((Sdhc *)hw)->CA1R.reg & SDHC_CA1R_SDR50SUP) >> SDHC_CA1R_SDR50SUP_Pos;
}

static inline bool hri_sdhc_get_CA1R_SDR104SUP_bit(const void *const hw)
{
	return (((Sdhc *)hw)->CA1R.reg & SDHC_CA1R_SDR104SUP) >> SDHC_CA1R_SDR104SUP_Pos;
}

static inline bool hri_sdhc_get_CA1R_DDR50SUP_bit(const void *const hw)
{
	return (((Sdhc *)hw)->CA1R.reg & SDHC_CA1R_DDR50SUP) >> SDHC_CA1R_DDR50SUP_Pos;
}

static inline bool hri_sdhc_get_CA1R_DRVASUP_bit(const void *const hw)
{
	return (((Sdhc *)hw)->CA1R.reg & SDHC_CA1R_DRVASUP) >> SDHC_CA1R_DRVASUP_Pos;
}

static inline bool hri_sdhc_get_CA1R_DRVCSUP_bit(const void *const hw)
{
	return (((Sdhc *)hw)->CA1R.reg & SDHC_CA1R_DRVCSUP) >> SDHC_CA1R_DRVCSUP_Pos;
}

static inline bool hri_sdhc_get_CA1R_DRVDSUP_bit(const void *const hw)
{
	return (((Sdhc *)hw)->CA1R.reg & SDHC_CA1R_DRVDSUP) >> SDHC_CA1R_DRVDSUP_Pos;
}

static inline bool hri_sdhc_get_CA1R_TSDR50_bit(const void *const hw)
{
	return (((Sdhc *)hw)->CA1R.reg & SDHC_CA1R_TSDR50) >> SDHC_CA1R_TSDR50_Pos;
}

static inline hri_sdhc_ca1r_reg_t hri_sdhc_get_CA1R_TCNTRT_bf(const void *const hw, hri_sdhc_ca1r_reg_t mask)
{
	return (((Sdhc *)hw)->CA1R.reg & SDHC_CA1R_TCNTRT(mask)) >> SDHC_CA1R_TCNTRT_Pos;
}

static inline hri_sdhc_ca1r_reg_t hri_sdhc_read_CA1R_TCNTRT_bf(const void *const hw)
{
	return (((Sdhc *)hw)->CA1R.reg & SDHC_CA1R_TCNTRT_Msk) >> SDHC_CA1R_TCNTRT_Pos;
}

static inline hri_sdhc_ca1r_reg_t hri_sdhc_get_CA1R_CLKMULT_bf(const void *const hw, hri_sdhc_ca1r_reg_t mask)
{
	return (((Sdhc *)hw)->CA1R.reg & SDHC_CA1R_CLKMULT(mask)) >> SDHC_CA1R_CLKMULT_Pos;
}

static inline hri_sdhc_ca1r_reg_t hri_sdhc_read_CA1R_CLKMULT_bf(const void *const hw)
{
	return (((Sdhc *)hw)->CA1R.reg & SDHC_CA1R_CLKMULT_Msk) >> SDHC_CA1R_CLKMULT_Pos;
}

static inline hri_sdhc_ca1r_reg_t hri_sdhc_get_CA1R_reg(const void *const hw, hri_sdhc_ca1r_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sdhc *)hw)->CA1R.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_sdhc_ca1r_reg_t hri_sdhc_read_CA1R_reg(const void *const hw)
{
	return ((Sdhc *)hw)->CA1R.reg;
}

static inline hri_sdhc_mccar_reg_t hri_sdhc_get_MCCAR_MAXCUR33V_bf(const void *const hw, hri_sdhc_mccar_reg_t mask)
{
	return (((Sdhc *)hw)->MCCAR.reg & SDHC_MCCAR_MAXCUR33V(mask)) >> SDHC_MCCAR_MAXCUR33V_Pos;
}

static inline hri_sdhc_mccar_reg_t hri_sdhc_read_MCCAR_MAXCUR33V_bf(const void *const hw)
{
	return (((Sdhc *)hw)->MCCAR.reg & SDHC_MCCAR_MAXCUR33V_Msk) >> SDHC_MCCAR_MAXCUR33V_Pos;
}

static inline hri_sdhc_mccar_reg_t hri_sdhc_get_MCCAR_MAXCUR30V_bf(const void *const hw, hri_sdhc_mccar_reg_t mask)
{
	return (((Sdhc *)hw)->MCCAR.reg & SDHC_MCCAR_MAXCUR30V(mask)) >> SDHC_MCCAR_MAXCUR30V_Pos;
}

static inline hri_sdhc_mccar_reg_t hri_sdhc_read_MCCAR_MAXCUR30V_bf(const void *const hw)
{
	return (((Sdhc *)hw)->MCCAR.reg & SDHC_MCCAR_MAXCUR30V_Msk) >> SDHC_MCCAR_MAXCUR30V_Pos;
}

static inline hri_sdhc_mccar_reg_t hri_sdhc_get_MCCAR_MAXCUR18V_bf(const void *const hw, hri_sdhc_mccar_reg_t mask)
{
	return (((Sdhc *)hw)->MCCAR.reg & SDHC_MCCAR_MAXCUR18V(mask)) >> SDHC_MCCAR_MAXCUR18V_Pos;
}

static inline hri_sdhc_mccar_reg_t hri_sdhc_read_MCCAR_MAXCUR18V_bf(const void *const hw)
{
	return (((Sdhc *)hw)->MCCAR.reg & SDHC_MCCAR_MAXCUR18V_Msk) >> SDHC_MCCAR_MAXCUR18V_Pos;
}

static inline hri_sdhc_mccar_reg_t hri_sdhc_get_MCCAR_reg(const void *const hw, hri_sdhc_mccar_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Sdhc *)hw)->MCCAR.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_sdhc_mccar_reg_t hri_sdhc_read_MCCAR_reg(const void *const hw)
{
	return ((Sdhc *)hw)->MCCAR.reg;
}

static inline bool hri_sdhc_get_AESR_LMIS_bit(const void *const hw)
{
	return (((Sdhc *)hw)->AESR.reg & SDHC_AESR_LMIS) >> SDHC_AESR_LMIS_Pos;
}

static inline hri_sdhc_aesr_reg_t hri_sdhc_get_AESR_ERRST_bf(const void *const hw, hri_sdhc_aesr_reg_t mask)
{
	return (((Sdhc *)hw)->AESR.reg & SDHC_AESR_ERRST(mask)) >> SDHC_AESR_ERRST_Pos;
}

static inline hri_sdhc_aesr_reg_t hri_sdhc_read_AESR_ERRST_bf(const void *const hw)
{
	return (((Sdhc *)hw)->AESR.reg & SDHC_AESR_ERRST_Msk) >> SDHC_AESR_ERRST_Pos;
}

static inline hri_sdhc_aesr_reg_t hri_sdhc_get_AESR_reg(const void *const hw, hri_sdhc_aesr_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Sdhc *)hw)->AESR.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_sdhc_aesr_reg_t hri_sdhc_read_AESR_reg(const void *const hw)
{
	return ((Sdhc *)hw)->AESR.reg;
}

static inline bool hri_sdhc_get_SISR_INTSSL_bit(const void *const hw)
{
	return (((Sdhc *)hw)->SISR.reg & SDHC_SISR_INTSSL_Msk) >> SDHC_SISR_INTSSL_Pos;
}

static inline hri_sdhc_sisr_reg_t hri_sdhc_get_SISR_reg(const void *const hw, hri_sdhc_sisr_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->SISR.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_sdhc_sisr_reg_t hri_sdhc_read_SISR_reg(const void *const hw)
{
	return ((Sdhc *)hw)->SISR.reg;
}

static inline hri_sdhc_hcvr_reg_t hri_sdhc_get_HCVR_SVER_bf(const void *const hw, hri_sdhc_hcvr_reg_t mask)
{
	return (((Sdhc *)hw)->HCVR.reg & SDHC_HCVR_SVER(mask)) >> SDHC_HCVR_SVER_Pos;
}

static inline hri_sdhc_hcvr_reg_t hri_sdhc_read_HCVR_SVER_bf(const void *const hw)
{
	return (((Sdhc *)hw)->HCVR.reg & SDHC_HCVR_SVER_Msk) >> SDHC_HCVR_SVER_Pos;
}

static inline hri_sdhc_hcvr_reg_t hri_sdhc_get_HCVR_VVER_bf(const void *const hw, hri_sdhc_hcvr_reg_t mask)
{
	return (((Sdhc *)hw)->HCVR.reg & SDHC_HCVR_VVER(mask)) >> SDHC_HCVR_VVER_Pos;
}

static inline hri_sdhc_hcvr_reg_t hri_sdhc_read_HCVR_VVER_bf(const void *const hw)
{
	return (((Sdhc *)hw)->HCVR.reg & SDHC_HCVR_VVER_Msk) >> SDHC_HCVR_VVER_Pos;
}

static inline hri_sdhc_hcvr_reg_t hri_sdhc_get_HCVR_reg(const void *const hw, hri_sdhc_hcvr_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Sdhc *)hw)->HCVR.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_sdhc_hcvr_reg_t hri_sdhc_read_HCVR_reg(const void *const hw)
{
	return ((Sdhc *)hw)->HCVR.reg;
}

#ifdef __cplusplus
}
#endif

#endif /* _HRI_SDHC_D51_H_INCLUDED */
#endif /* _SAMD51_SDHC_COMPONENT_ */
