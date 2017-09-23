/**
 * \file
 *
 * \brief SAM CMCC
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

#ifdef _SAMD51_CMCC_COMPONENT_
#ifndef _HRI_CMCC_D51_H_INCLUDED_
#define _HRI_CMCC_D51_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <hal_atomic.h>

#if defined(ENABLE_CMCC_CRITICAL_SECTIONS)
#define CMCC_CRITICAL_SECTION_ENTER() CRITICAL_SECTION_ENTER()
#define CMCC_CRITICAL_SECTION_LEAVE() CRITICAL_SECTION_LEAVE()
#else
#define CMCC_CRITICAL_SECTION_ENTER()
#define CMCC_CRITICAL_SECTION_LEAVE()
#endif

typedef uint32_t hri_cmcc_cfg_reg_t;
typedef uint32_t hri_cmcc_ctrl_reg_t;
typedef uint32_t hri_cmcc_lckway_reg_t;
typedef uint32_t hri_cmcc_maint0_reg_t;
typedef uint32_t hri_cmcc_maint1_reg_t;
typedef uint32_t hri_cmcc_mcfg_reg_t;
typedef uint32_t hri_cmcc_mctrl_reg_t;
typedef uint32_t hri_cmcc_men_reg_t;
typedef uint32_t hri_cmcc_msr_reg_t;
typedef uint32_t hri_cmcc_sr_reg_t;
typedef uint32_t hri_cmcc_type_reg_t;

static inline void hri_cmcc_write_CTRL_reg(const void *const hw, hri_cmcc_ctrl_reg_t data)
{
	CMCC_CRITICAL_SECTION_ENTER();
	((Cmcc *)hw)->CTRL.reg = data;
	CMCC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_cmcc_write_MAINT0_reg(const void *const hw, hri_cmcc_maint0_reg_t data)
{
	CMCC_CRITICAL_SECTION_ENTER();
	((Cmcc *)hw)->MAINT0.reg = data;
	CMCC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_cmcc_write_MAINT1_reg(const void *const hw, hri_cmcc_maint1_reg_t data)
{
	CMCC_CRITICAL_SECTION_ENTER();
	((Cmcc *)hw)->MAINT1.reg = data;
	CMCC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_cmcc_write_MCTRL_reg(const void *const hw, hri_cmcc_mctrl_reg_t data)
{
	CMCC_CRITICAL_SECTION_ENTER();
	((Cmcc *)hw)->MCTRL.reg = data;
	CMCC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_cmcc_set_CFG_reg(const void *const hw, hri_cmcc_cfg_reg_t mask)
{
	CMCC_CRITICAL_SECTION_ENTER();
	((Cmcc *)hw)->CFG.reg |= mask;
	CMCC_CRITICAL_SECTION_LEAVE();
}

static inline hri_cmcc_cfg_reg_t hri_cmcc_get_CFG_reg(const void *const hw, hri_cmcc_cfg_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Cmcc *)hw)->CFG.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_cmcc_write_CFG_reg(const void *const hw, hri_cmcc_cfg_reg_t data)
{
	CMCC_CRITICAL_SECTION_ENTER();
	((Cmcc *)hw)->CFG.reg = data;
	CMCC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_cmcc_clear_CFG_reg(const void *const hw, hri_cmcc_cfg_reg_t mask)
{
	CMCC_CRITICAL_SECTION_ENTER();
	((Cmcc *)hw)->CFG.reg &= ~mask;
	CMCC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_cmcc_toggle_CFG_reg(const void *const hw, hri_cmcc_cfg_reg_t mask)
{
	CMCC_CRITICAL_SECTION_ENTER();
	((Cmcc *)hw)->CFG.reg ^= mask;
	CMCC_CRITICAL_SECTION_LEAVE();
}

static inline hri_cmcc_cfg_reg_t hri_cmcc_read_CFG_reg(const void *const hw)
{
	return ((Cmcc *)hw)->CFG.reg;
}

static inline void hri_cmcc_set_LCKWAY_reg(const void *const hw, hri_cmcc_lckway_reg_t mask)
{
	CMCC_CRITICAL_SECTION_ENTER();
	((Cmcc *)hw)->LCKWAY.reg |= mask;
	CMCC_CRITICAL_SECTION_LEAVE();
}

static inline hri_cmcc_lckway_reg_t hri_cmcc_get_LCKWAY_reg(const void *const hw, hri_cmcc_lckway_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Cmcc *)hw)->LCKWAY.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_cmcc_write_LCKWAY_reg(const void *const hw, hri_cmcc_lckway_reg_t data)
{
	CMCC_CRITICAL_SECTION_ENTER();
	((Cmcc *)hw)->LCKWAY.reg = data;
	CMCC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_cmcc_clear_LCKWAY_reg(const void *const hw, hri_cmcc_lckway_reg_t mask)
{
	CMCC_CRITICAL_SECTION_ENTER();
	((Cmcc *)hw)->LCKWAY.reg &= ~mask;
	CMCC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_cmcc_toggle_LCKWAY_reg(const void *const hw, hri_cmcc_lckway_reg_t mask)
{
	CMCC_CRITICAL_SECTION_ENTER();
	((Cmcc *)hw)->LCKWAY.reg ^= mask;
	CMCC_CRITICAL_SECTION_LEAVE();
}

static inline hri_cmcc_lckway_reg_t hri_cmcc_read_LCKWAY_reg(const void *const hw)
{
	return ((Cmcc *)hw)->LCKWAY.reg;
}

static inline void hri_cmcc_set_MCFG_reg(const void *const hw, hri_cmcc_mcfg_reg_t mask)
{
	CMCC_CRITICAL_SECTION_ENTER();
	((Cmcc *)hw)->MCFG.reg |= mask;
	CMCC_CRITICAL_SECTION_LEAVE();
}

static inline hri_cmcc_mcfg_reg_t hri_cmcc_get_MCFG_reg(const void *const hw, hri_cmcc_mcfg_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Cmcc *)hw)->MCFG.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_cmcc_write_MCFG_reg(const void *const hw, hri_cmcc_mcfg_reg_t data)
{
	CMCC_CRITICAL_SECTION_ENTER();
	((Cmcc *)hw)->MCFG.reg = data;
	CMCC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_cmcc_clear_MCFG_reg(const void *const hw, hri_cmcc_mcfg_reg_t mask)
{
	CMCC_CRITICAL_SECTION_ENTER();
	((Cmcc *)hw)->MCFG.reg &= ~mask;
	CMCC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_cmcc_toggle_MCFG_reg(const void *const hw, hri_cmcc_mcfg_reg_t mask)
{
	CMCC_CRITICAL_SECTION_ENTER();
	((Cmcc *)hw)->MCFG.reg ^= mask;
	CMCC_CRITICAL_SECTION_LEAVE();
}

static inline hri_cmcc_mcfg_reg_t hri_cmcc_read_MCFG_reg(const void *const hw)
{
	return ((Cmcc *)hw)->MCFG.reg;
}

static inline void hri_cmcc_set_MEN_reg(const void *const hw, hri_cmcc_men_reg_t mask)
{
	CMCC_CRITICAL_SECTION_ENTER();
	((Cmcc *)hw)->MEN.reg |= mask;
	CMCC_CRITICAL_SECTION_LEAVE();
}

static inline hri_cmcc_men_reg_t hri_cmcc_get_MEN_reg(const void *const hw, hri_cmcc_men_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Cmcc *)hw)->MEN.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_cmcc_write_MEN_reg(const void *const hw, hri_cmcc_men_reg_t data)
{
	CMCC_CRITICAL_SECTION_ENTER();
	((Cmcc *)hw)->MEN.reg = data;
	CMCC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_cmcc_clear_MEN_reg(const void *const hw, hri_cmcc_men_reg_t mask)
{
	CMCC_CRITICAL_SECTION_ENTER();
	((Cmcc *)hw)->MEN.reg &= ~mask;
	CMCC_CRITICAL_SECTION_LEAVE();
}

static inline void hri_cmcc_toggle_MEN_reg(const void *const hw, hri_cmcc_men_reg_t mask)
{
	CMCC_CRITICAL_SECTION_ENTER();
	((Cmcc *)hw)->MEN.reg ^= mask;
	CMCC_CRITICAL_SECTION_LEAVE();
}

static inline hri_cmcc_men_reg_t hri_cmcc_read_MEN_reg(const void *const hw)
{
	return ((Cmcc *)hw)->MEN.reg;
}

static inline bool hri_cmcc_get_TYPE_GCLK_bit(const void *const hw)
{
	return (((Cmcc *)hw)->TYPE.reg & CMCC_TYPE_GCLK) >> CMCC_TYPE_GCLK_Pos;
}

static inline bool hri_cmcc_get_TYPE_RRP_bit(const void *const hw)
{
	return (((Cmcc *)hw)->TYPE.reg & CMCC_TYPE_RRP) >> CMCC_TYPE_RRP_Pos;
}

static inline bool hri_cmcc_get_TYPE_LCKDOWN_bit(const void *const hw)
{
	return (((Cmcc *)hw)->TYPE.reg & CMCC_TYPE_LCKDOWN) >> CMCC_TYPE_LCKDOWN_Pos;
}

static inline hri_cmcc_type_reg_t hri_cmcc_get_TYPE_WAYNUM_bf(const void *const hw, hri_cmcc_type_reg_t mask)
{
	return (((Cmcc *)hw)->TYPE.reg & CMCC_TYPE_WAYNUM(mask)) >> CMCC_TYPE_WAYNUM_Pos;
}

static inline hri_cmcc_type_reg_t hri_cmcc_read_TYPE_WAYNUM_bf(const void *const hw)
{
	return (((Cmcc *)hw)->TYPE.reg & CMCC_TYPE_WAYNUM_Msk) >> CMCC_TYPE_WAYNUM_Pos;
}

static inline hri_cmcc_type_reg_t hri_cmcc_get_TYPE_CSIZE_bf(const void *const hw, hri_cmcc_type_reg_t mask)
{
	return (((Cmcc *)hw)->TYPE.reg & CMCC_TYPE_CSIZE(mask)) >> CMCC_TYPE_CSIZE_Pos;
}

static inline hri_cmcc_type_reg_t hri_cmcc_read_TYPE_CSIZE_bf(const void *const hw)
{
	return (((Cmcc *)hw)->TYPE.reg & CMCC_TYPE_CSIZE_Msk) >> CMCC_TYPE_CSIZE_Pos;
}

static inline hri_cmcc_type_reg_t hri_cmcc_get_TYPE_CLSIZE_bf(const void *const hw, hri_cmcc_type_reg_t mask)
{
	return (((Cmcc *)hw)->TYPE.reg & CMCC_TYPE_CLSIZE(mask)) >> CMCC_TYPE_CLSIZE_Pos;
}

static inline hri_cmcc_type_reg_t hri_cmcc_read_TYPE_CLSIZE_bf(const void *const hw)
{
	return (((Cmcc *)hw)->TYPE.reg & CMCC_TYPE_CLSIZE_Msk) >> CMCC_TYPE_CLSIZE_Pos;
}

static inline hri_cmcc_type_reg_t hri_cmcc_get_TYPE_reg(const void *const hw, hri_cmcc_type_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Cmcc *)hw)->TYPE.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_cmcc_type_reg_t hri_cmcc_read_TYPE_reg(const void *const hw)
{
	return ((Cmcc *)hw)->TYPE.reg;
}

static inline bool hri_cmcc_get_SR_CSTS_bit(const void *const hw)
{
	return (((Cmcc *)hw)->SR.reg & CMCC_SR_CSTS) >> CMCC_SR_CSTS_Pos;
}

static inline hri_cmcc_sr_reg_t hri_cmcc_get_SR_reg(const void *const hw, hri_cmcc_sr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Cmcc *)hw)->SR.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_cmcc_sr_reg_t hri_cmcc_read_SR_reg(const void *const hw)
{
	return ((Cmcc *)hw)->SR.reg;
}

static inline hri_cmcc_msr_reg_t hri_cmcc_get_MSR_EVENT_CNT_bf(const void *const hw, hri_cmcc_msr_reg_t mask)
{
	return (((Cmcc *)hw)->MSR.reg & CMCC_MSR_EVENT_CNT(mask)) >> CMCC_MSR_EVENT_CNT_Pos;
}

static inline hri_cmcc_msr_reg_t hri_cmcc_read_MSR_EVENT_CNT_bf(const void *const hw)
{
	return (((Cmcc *)hw)->MSR.reg & CMCC_MSR_EVENT_CNT_Msk) >> CMCC_MSR_EVENT_CNT_Pos;
}

static inline hri_cmcc_msr_reg_t hri_cmcc_get_MSR_reg(const void *const hw, hri_cmcc_msr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Cmcc *)hw)->MSR.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_cmcc_msr_reg_t hri_cmcc_read_MSR_reg(const void *const hw)
{
	return ((Cmcc *)hw)->MSR.reg;
}

#ifdef __cplusplus
}
#endif

#endif /* _HRI_CMCC_D51_H_INCLUDED */
#endif /* _SAMD51_CMCC_COMPONENT_ */
