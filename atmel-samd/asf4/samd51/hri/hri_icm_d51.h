/**
 * \file
 *
 * \brief SAM ICM
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

#ifdef _SAMD51_ICM_COMPONENT_
#ifndef _HRI_ICM_D51_H_INCLUDED_
#define _HRI_ICM_D51_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <hal_atomic.h>

#if defined(ENABLE_ICM_CRITICAL_SECTIONS)
#define ICM_CRITICAL_SECTION_ENTER() CRITICAL_SECTION_ENTER()
#define ICM_CRITICAL_SECTION_LEAVE() CRITICAL_SECTION_LEAVE()
#else
#define ICM_CRITICAL_SECTION_ENTER()
#define ICM_CRITICAL_SECTION_LEAVE()
#endif

typedef uint32_t hri_icm_cfg_reg_t;
typedef uint32_t hri_icm_ctrl_reg_t;
typedef uint32_t hri_icm_dscr_reg_t;
typedef uint32_t hri_icm_hash_reg_t;
typedef uint32_t hri_icm_imr_reg_t;
typedef uint32_t hri_icm_isr_reg_t;
typedef uint32_t hri_icm_sr_reg_t;
typedef uint32_t hri_icm_uasr_reg_t;
typedef uint32_t hri_icm_uihval_reg_t;
typedef uint32_t hri_icmdescriptor_raddr_reg_t;
typedef uint32_t hri_icmdescriptor_rcfg_reg_t;
typedef uint32_t hri_icmdescriptor_rctrl_reg_t;
typedef uint32_t hri_icmdescriptor_rnext_reg_t;

static inline void hri_icmdescriptor_set_RADDR_reg(const void *const hw, hri_icmdescriptor_raddr_reg_t mask)
{
	ICM_CRITICAL_SECTION_ENTER();
	((IcmDescriptor *)hw)->RADDR.reg |= mask;
	ICM_CRITICAL_SECTION_LEAVE();
}

static inline hri_icmdescriptor_raddr_reg_t hri_icmdescriptor_get_RADDR_reg(const void *const             hw,
                                                                            hri_icmdescriptor_raddr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((IcmDescriptor *)hw)->RADDR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_icmdescriptor_write_RADDR_reg(const void *const hw, hri_icmdescriptor_raddr_reg_t data)
{
	ICM_CRITICAL_SECTION_ENTER();
	((IcmDescriptor *)hw)->RADDR.reg = data;
	ICM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_icmdescriptor_clear_RADDR_reg(const void *const hw, hri_icmdescriptor_raddr_reg_t mask)
{
	ICM_CRITICAL_SECTION_ENTER();
	((IcmDescriptor *)hw)->RADDR.reg &= ~mask;
	ICM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_icmdescriptor_toggle_RADDR_reg(const void *const hw, hri_icmdescriptor_raddr_reg_t mask)
{
	ICM_CRITICAL_SECTION_ENTER();
	((IcmDescriptor *)hw)->RADDR.reg ^= mask;
	ICM_CRITICAL_SECTION_LEAVE();
}

static inline hri_icmdescriptor_raddr_reg_t hri_icmdescriptor_read_RADDR_reg(const void *const hw)
{
	return ((IcmDescriptor *)hw)->RADDR.reg;
}

static inline void hri_icmdescriptor_set_RCFG_reg(const void *const hw, hri_icmdescriptor_rcfg_reg_t mask)
{
	ICM_CRITICAL_SECTION_ENTER();
	((IcmDescriptor *)hw)->RCFG.reg |= mask;
	ICM_CRITICAL_SECTION_LEAVE();
}

static inline hri_icmdescriptor_rcfg_reg_t hri_icmdescriptor_get_RCFG_reg(const void *const            hw,
                                                                          hri_icmdescriptor_rcfg_reg_t mask)
{
	uint32_t tmp;
	tmp = ((IcmDescriptor *)hw)->RCFG.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_icmdescriptor_write_RCFG_reg(const void *const hw, hri_icmdescriptor_rcfg_reg_t data)
{
	ICM_CRITICAL_SECTION_ENTER();
	((IcmDescriptor *)hw)->RCFG.reg = data;
	ICM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_icmdescriptor_clear_RCFG_reg(const void *const hw, hri_icmdescriptor_rcfg_reg_t mask)
{
	ICM_CRITICAL_SECTION_ENTER();
	((IcmDescriptor *)hw)->RCFG.reg &= ~mask;
	ICM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_icmdescriptor_toggle_RCFG_reg(const void *const hw, hri_icmdescriptor_rcfg_reg_t mask)
{
	ICM_CRITICAL_SECTION_ENTER();
	((IcmDescriptor *)hw)->RCFG.reg ^= mask;
	ICM_CRITICAL_SECTION_LEAVE();
}

static inline hri_icmdescriptor_rcfg_reg_t hri_icmdescriptor_read_RCFG_reg(const void *const hw)
{
	return ((IcmDescriptor *)hw)->RCFG.reg;
}

static inline void hri_icmdescriptor_set_RCTRL_reg(const void *const hw, hri_icmdescriptor_rctrl_reg_t mask)
{
	ICM_CRITICAL_SECTION_ENTER();
	((IcmDescriptor *)hw)->RCTRL.reg |= mask;
	ICM_CRITICAL_SECTION_LEAVE();
}

static inline hri_icmdescriptor_rctrl_reg_t hri_icmdescriptor_get_RCTRL_reg(const void *const             hw,
                                                                            hri_icmdescriptor_rctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((IcmDescriptor *)hw)->RCTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_icmdescriptor_write_RCTRL_reg(const void *const hw, hri_icmdescriptor_rctrl_reg_t data)
{
	ICM_CRITICAL_SECTION_ENTER();
	((IcmDescriptor *)hw)->RCTRL.reg = data;
	ICM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_icmdescriptor_clear_RCTRL_reg(const void *const hw, hri_icmdescriptor_rctrl_reg_t mask)
{
	ICM_CRITICAL_SECTION_ENTER();
	((IcmDescriptor *)hw)->RCTRL.reg &= ~mask;
	ICM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_icmdescriptor_toggle_RCTRL_reg(const void *const hw, hri_icmdescriptor_rctrl_reg_t mask)
{
	ICM_CRITICAL_SECTION_ENTER();
	((IcmDescriptor *)hw)->RCTRL.reg ^= mask;
	ICM_CRITICAL_SECTION_LEAVE();
}

static inline hri_icmdescriptor_rctrl_reg_t hri_icmdescriptor_read_RCTRL_reg(const void *const hw)
{
	return ((IcmDescriptor *)hw)->RCTRL.reg;
}

static inline void hri_icmdescriptor_set_RNEXT_reg(const void *const hw, hri_icmdescriptor_rnext_reg_t mask)
{
	ICM_CRITICAL_SECTION_ENTER();
	((IcmDescriptor *)hw)->RNEXT.reg |= mask;
	ICM_CRITICAL_SECTION_LEAVE();
}

static inline hri_icmdescriptor_rnext_reg_t hri_icmdescriptor_get_RNEXT_reg(const void *const             hw,
                                                                            hri_icmdescriptor_rnext_reg_t mask)
{
	uint32_t tmp;
	tmp = ((IcmDescriptor *)hw)->RNEXT.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_icmdescriptor_write_RNEXT_reg(const void *const hw, hri_icmdescriptor_rnext_reg_t data)
{
	ICM_CRITICAL_SECTION_ENTER();
	((IcmDescriptor *)hw)->RNEXT.reg = data;
	ICM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_icmdescriptor_clear_RNEXT_reg(const void *const hw, hri_icmdescriptor_rnext_reg_t mask)
{
	ICM_CRITICAL_SECTION_ENTER();
	((IcmDescriptor *)hw)->RNEXT.reg &= ~mask;
	ICM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_icmdescriptor_toggle_RNEXT_reg(const void *const hw, hri_icmdescriptor_rnext_reg_t mask)
{
	ICM_CRITICAL_SECTION_ENTER();
	((IcmDescriptor *)hw)->RNEXT.reg ^= mask;
	ICM_CRITICAL_SECTION_LEAVE();
}

static inline hri_icmdescriptor_rnext_reg_t hri_icmdescriptor_read_RNEXT_reg(const void *const hw)
{
	return ((IcmDescriptor *)hw)->RNEXT.reg;
}

static inline void hri_icm_set_IMR_URAD_bit(const void *const hw)
{
	((Icm *)hw)->IER.reg = ICM_IMR_URAD;
}

static inline bool hri_icm_get_IMR_URAD_bit(const void *const hw)
{
	return (((Icm *)hw)->IMR.reg & ICM_IMR_URAD) >> ICM_IMR_URAD_Pos;
}

static inline void hri_icm_write_IMR_URAD_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Icm *)hw)->IDR.reg = ICM_IMR_URAD;
	} else {
		((Icm *)hw)->IER.reg = ICM_IMR_URAD;
	}
}

static inline void hri_icm_clear_IMR_URAD_bit(const void *const hw)
{
	((Icm *)hw)->IDR.reg = ICM_IMR_URAD;
}

static inline void hri_icm_set_IMR_RHC_bf(const void *const hw, hri_icm_imr_reg_t mask)
{
	((Icm *)hw)->IER.reg = ICM_IMR_RHC(mask);
}

static inline hri_icm_imr_reg_t hri_icm_get_IMR_RHC_bf(const void *const hw, hri_icm_imr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Icm *)hw)->IMR.reg;
	tmp = (tmp & ICM_IMR_RHC(mask)) >> ICM_IMR_RHC_Pos;
	return tmp;
}

static inline hri_icm_imr_reg_t hri_icm_read_IMR_RHC_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Icm *)hw)->IMR.reg;
	tmp = (tmp & ICM_IMR_RHC_Msk) >> ICM_IMR_RHC_Pos;
	return tmp;
}

static inline void hri_icm_write_IMR_RHC_bf(const void *const hw, hri_icm_imr_reg_t data)
{
	((Icm *)hw)->IER.reg = ICM_IMR_RHC(data);
	((Icm *)hw)->IDR.reg = ~ICM_IMR_RHC(data);
}

static inline void hri_icm_clear_IMR_RHC_bf(const void *const hw, hri_icm_imr_reg_t mask)
{
	((Icm *)hw)->IDR.reg = ICM_IMR_RHC(mask);
}

static inline void hri_icm_set_IMR_RDM_bf(const void *const hw, hri_icm_imr_reg_t mask)
{
	((Icm *)hw)->IER.reg = ICM_IMR_RDM(mask);
}

static inline hri_icm_imr_reg_t hri_icm_get_IMR_RDM_bf(const void *const hw, hri_icm_imr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Icm *)hw)->IMR.reg;
	tmp = (tmp & ICM_IMR_RDM(mask)) >> ICM_IMR_RDM_Pos;
	return tmp;
}

static inline hri_icm_imr_reg_t hri_icm_read_IMR_RDM_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Icm *)hw)->IMR.reg;
	tmp = (tmp & ICM_IMR_RDM_Msk) >> ICM_IMR_RDM_Pos;
	return tmp;
}

static inline void hri_icm_write_IMR_RDM_bf(const void *const hw, hri_icm_imr_reg_t data)
{
	((Icm *)hw)->IER.reg = ICM_IMR_RDM(data);
	((Icm *)hw)->IDR.reg = ~ICM_IMR_RDM(data);
}

static inline void hri_icm_clear_IMR_RDM_bf(const void *const hw, hri_icm_imr_reg_t mask)
{
	((Icm *)hw)->IDR.reg = ICM_IMR_RDM(mask);
}

static inline void hri_icm_set_IMR_RBE_bf(const void *const hw, hri_icm_imr_reg_t mask)
{
	((Icm *)hw)->IER.reg = ICM_IMR_RBE(mask);
}

static inline hri_icm_imr_reg_t hri_icm_get_IMR_RBE_bf(const void *const hw, hri_icm_imr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Icm *)hw)->IMR.reg;
	tmp = (tmp & ICM_IMR_RBE(mask)) >> ICM_IMR_RBE_Pos;
	return tmp;
}

static inline hri_icm_imr_reg_t hri_icm_read_IMR_RBE_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Icm *)hw)->IMR.reg;
	tmp = (tmp & ICM_IMR_RBE_Msk) >> ICM_IMR_RBE_Pos;
	return tmp;
}

static inline void hri_icm_write_IMR_RBE_bf(const void *const hw, hri_icm_imr_reg_t data)
{
	((Icm *)hw)->IER.reg = ICM_IMR_RBE(data);
	((Icm *)hw)->IDR.reg = ~ICM_IMR_RBE(data);
}

static inline void hri_icm_clear_IMR_RBE_bf(const void *const hw, hri_icm_imr_reg_t mask)
{
	((Icm *)hw)->IDR.reg = ICM_IMR_RBE(mask);
}

static inline void hri_icm_set_IMR_RWC_bf(const void *const hw, hri_icm_imr_reg_t mask)
{
	((Icm *)hw)->IER.reg = ICM_IMR_RWC(mask);
}

static inline hri_icm_imr_reg_t hri_icm_get_IMR_RWC_bf(const void *const hw, hri_icm_imr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Icm *)hw)->IMR.reg;
	tmp = (tmp & ICM_IMR_RWC(mask)) >> ICM_IMR_RWC_Pos;
	return tmp;
}

static inline hri_icm_imr_reg_t hri_icm_read_IMR_RWC_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Icm *)hw)->IMR.reg;
	tmp = (tmp & ICM_IMR_RWC_Msk) >> ICM_IMR_RWC_Pos;
	return tmp;
}

static inline void hri_icm_write_IMR_RWC_bf(const void *const hw, hri_icm_imr_reg_t data)
{
	((Icm *)hw)->IER.reg = ICM_IMR_RWC(data);
	((Icm *)hw)->IDR.reg = ~ICM_IMR_RWC(data);
}

static inline void hri_icm_clear_IMR_RWC_bf(const void *const hw, hri_icm_imr_reg_t mask)
{
	((Icm *)hw)->IDR.reg = ICM_IMR_RWC(mask);
}

static inline void hri_icm_set_IMR_REC_bf(const void *const hw, hri_icm_imr_reg_t mask)
{
	((Icm *)hw)->IER.reg = ICM_IMR_REC(mask);
}

static inline hri_icm_imr_reg_t hri_icm_get_IMR_REC_bf(const void *const hw, hri_icm_imr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Icm *)hw)->IMR.reg;
	tmp = (tmp & ICM_IMR_REC(mask)) >> ICM_IMR_REC_Pos;
	return tmp;
}

static inline hri_icm_imr_reg_t hri_icm_read_IMR_REC_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Icm *)hw)->IMR.reg;
	tmp = (tmp & ICM_IMR_REC_Msk) >> ICM_IMR_REC_Pos;
	return tmp;
}

static inline void hri_icm_write_IMR_REC_bf(const void *const hw, hri_icm_imr_reg_t data)
{
	((Icm *)hw)->IER.reg = ICM_IMR_REC(data);
	((Icm *)hw)->IDR.reg = ~ICM_IMR_REC(data);
}

static inline void hri_icm_clear_IMR_REC_bf(const void *const hw, hri_icm_imr_reg_t mask)
{
	((Icm *)hw)->IDR.reg = ICM_IMR_REC(mask);
}

static inline void hri_icm_set_IMR_RSU_bf(const void *const hw, hri_icm_imr_reg_t mask)
{
	((Icm *)hw)->IER.reg = ICM_IMR_RSU(mask);
}

static inline hri_icm_imr_reg_t hri_icm_get_IMR_RSU_bf(const void *const hw, hri_icm_imr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Icm *)hw)->IMR.reg;
	tmp = (tmp & ICM_IMR_RSU(mask)) >> ICM_IMR_RSU_Pos;
	return tmp;
}

static inline hri_icm_imr_reg_t hri_icm_read_IMR_RSU_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Icm *)hw)->IMR.reg;
	tmp = (tmp & ICM_IMR_RSU_Msk) >> ICM_IMR_RSU_Pos;
	return tmp;
}

static inline void hri_icm_write_IMR_RSU_bf(const void *const hw, hri_icm_imr_reg_t data)
{
	((Icm *)hw)->IER.reg = ICM_IMR_RSU(data);
	((Icm *)hw)->IDR.reg = ~ICM_IMR_RSU(data);
}

static inline void hri_icm_clear_IMR_RSU_bf(const void *const hw, hri_icm_imr_reg_t mask)
{
	((Icm *)hw)->IDR.reg = ICM_IMR_RSU(mask);
}

static inline void hri_icm_set_IMR_reg(const void *const hw, hri_icm_imr_reg_t mask)
{
	((Icm *)hw)->IER.reg = mask;
}

static inline hri_icm_imr_reg_t hri_icm_get_IMR_reg(const void *const hw, hri_icm_imr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Icm *)hw)->IMR.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_icm_imr_reg_t hri_icm_read_IMR_reg(const void *const hw)
{
	return ((Icm *)hw)->IMR.reg;
}

static inline void hri_icm_write_IMR_reg(const void *const hw, hri_icm_imr_reg_t data)
{
	((Icm *)hw)->IER.reg = data;
	((Icm *)hw)->IDR.reg = ~data;
}

static inline void hri_icm_clear_IMR_reg(const void *const hw, hri_icm_imr_reg_t mask)
{
	((Icm *)hw)->IDR.reg = mask;
}

static inline void hri_icm_write_CTRL_reg(const void *const hw, hri_icm_ctrl_reg_t data)
{
	ICM_CRITICAL_SECTION_ENTER();
	((Icm *)hw)->CTRL.reg = data;
	ICM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_icm_write_UIHVAL_reg(const void *const hw, uint8_t index, hri_icm_uihval_reg_t data)
{
	ICM_CRITICAL_SECTION_ENTER();
	((Icm *)hw)->UIHVAL[index].reg = data;
	ICM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_icm_set_CFG_reg(const void *const hw, hri_icm_cfg_reg_t mask)
{
	ICM_CRITICAL_SECTION_ENTER();
	((Icm *)hw)->CFG.reg |= mask;
	ICM_CRITICAL_SECTION_LEAVE();
}

static inline hri_icm_cfg_reg_t hri_icm_get_CFG_reg(const void *const hw, hri_icm_cfg_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Icm *)hw)->CFG.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_icm_write_CFG_reg(const void *const hw, hri_icm_cfg_reg_t data)
{
	ICM_CRITICAL_SECTION_ENTER();
	((Icm *)hw)->CFG.reg = data;
	ICM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_icm_clear_CFG_reg(const void *const hw, hri_icm_cfg_reg_t mask)
{
	ICM_CRITICAL_SECTION_ENTER();
	((Icm *)hw)->CFG.reg &= ~mask;
	ICM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_icm_toggle_CFG_reg(const void *const hw, hri_icm_cfg_reg_t mask)
{
	ICM_CRITICAL_SECTION_ENTER();
	((Icm *)hw)->CFG.reg ^= mask;
	ICM_CRITICAL_SECTION_LEAVE();
}

static inline hri_icm_cfg_reg_t hri_icm_read_CFG_reg(const void *const hw)
{
	return ((Icm *)hw)->CFG.reg;
}

static inline void hri_icm_set_DSCR_reg(const void *const hw, hri_icm_dscr_reg_t mask)
{
	ICM_CRITICAL_SECTION_ENTER();
	((Icm *)hw)->DSCR.reg |= mask;
	ICM_CRITICAL_SECTION_LEAVE();
}

static inline hri_icm_dscr_reg_t hri_icm_get_DSCR_reg(const void *const hw, hri_icm_dscr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Icm *)hw)->DSCR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_icm_write_DSCR_reg(const void *const hw, hri_icm_dscr_reg_t data)
{
	ICM_CRITICAL_SECTION_ENTER();
	((Icm *)hw)->DSCR.reg = data;
	ICM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_icm_clear_DSCR_reg(const void *const hw, hri_icm_dscr_reg_t mask)
{
	ICM_CRITICAL_SECTION_ENTER();
	((Icm *)hw)->DSCR.reg &= ~mask;
	ICM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_icm_toggle_DSCR_reg(const void *const hw, hri_icm_dscr_reg_t mask)
{
	ICM_CRITICAL_SECTION_ENTER();
	((Icm *)hw)->DSCR.reg ^= mask;
	ICM_CRITICAL_SECTION_LEAVE();
}

static inline hri_icm_dscr_reg_t hri_icm_read_DSCR_reg(const void *const hw)
{
	return ((Icm *)hw)->DSCR.reg;
}

static inline void hri_icm_set_HASH_reg(const void *const hw, hri_icm_hash_reg_t mask)
{
	ICM_CRITICAL_SECTION_ENTER();
	((Icm *)hw)->HASH.reg |= mask;
	ICM_CRITICAL_SECTION_LEAVE();
}

static inline hri_icm_hash_reg_t hri_icm_get_HASH_reg(const void *const hw, hri_icm_hash_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Icm *)hw)->HASH.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_icm_write_HASH_reg(const void *const hw, hri_icm_hash_reg_t data)
{
	ICM_CRITICAL_SECTION_ENTER();
	((Icm *)hw)->HASH.reg = data;
	ICM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_icm_clear_HASH_reg(const void *const hw, hri_icm_hash_reg_t mask)
{
	ICM_CRITICAL_SECTION_ENTER();
	((Icm *)hw)->HASH.reg &= ~mask;
	ICM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_icm_toggle_HASH_reg(const void *const hw, hri_icm_hash_reg_t mask)
{
	ICM_CRITICAL_SECTION_ENTER();
	((Icm *)hw)->HASH.reg ^= mask;
	ICM_CRITICAL_SECTION_LEAVE();
}

static inline hri_icm_hash_reg_t hri_icm_read_HASH_reg(const void *const hw)
{
	return ((Icm *)hw)->HASH.reg;
}

static inline bool hri_icm_get_SR_ENABLE_bit(const void *const hw)
{
	return (((Icm *)hw)->SR.reg & ICM_SR_ENABLE) >> ICM_SR_ENABLE_Pos;
}

static inline hri_icm_sr_reg_t hri_icm_get_SR_RAWRMDIS_bf(const void *const hw, hri_icm_sr_reg_t mask)
{
	return (((Icm *)hw)->SR.reg & ICM_SR_RAWRMDIS(mask)) >> ICM_SR_RAWRMDIS_Pos;
}

static inline hri_icm_sr_reg_t hri_icm_read_SR_RAWRMDIS_bf(const void *const hw)
{
	return (((Icm *)hw)->SR.reg & ICM_SR_RAWRMDIS_Msk) >> ICM_SR_RAWRMDIS_Pos;
}

static inline hri_icm_sr_reg_t hri_icm_get_SR_RMDIS_bf(const void *const hw, hri_icm_sr_reg_t mask)
{
	return (((Icm *)hw)->SR.reg & ICM_SR_RMDIS(mask)) >> ICM_SR_RMDIS_Pos;
}

static inline hri_icm_sr_reg_t hri_icm_read_SR_RMDIS_bf(const void *const hw)
{
	return (((Icm *)hw)->SR.reg & ICM_SR_RMDIS_Msk) >> ICM_SR_RMDIS_Pos;
}

static inline hri_icm_sr_reg_t hri_icm_get_SR_reg(const void *const hw, hri_icm_sr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Icm *)hw)->SR.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_icm_sr_reg_t hri_icm_read_SR_reg(const void *const hw)
{
	return ((Icm *)hw)->SR.reg;
}

static inline bool hri_icm_get_ISR_URAD_bit(const void *const hw)
{
	return (((Icm *)hw)->ISR.reg & ICM_ISR_URAD) >> ICM_ISR_URAD_Pos;
}

static inline hri_icm_isr_reg_t hri_icm_get_ISR_RHC_bf(const void *const hw, hri_icm_isr_reg_t mask)
{
	return (((Icm *)hw)->ISR.reg & ICM_ISR_RHC(mask)) >> ICM_ISR_RHC_Pos;
}

static inline hri_icm_isr_reg_t hri_icm_read_ISR_RHC_bf(const void *const hw)
{
	return (((Icm *)hw)->ISR.reg & ICM_ISR_RHC_Msk) >> ICM_ISR_RHC_Pos;
}

static inline hri_icm_isr_reg_t hri_icm_get_ISR_RDM_bf(const void *const hw, hri_icm_isr_reg_t mask)
{
	return (((Icm *)hw)->ISR.reg & ICM_ISR_RDM(mask)) >> ICM_ISR_RDM_Pos;
}

static inline hri_icm_isr_reg_t hri_icm_read_ISR_RDM_bf(const void *const hw)
{
	return (((Icm *)hw)->ISR.reg & ICM_ISR_RDM_Msk) >> ICM_ISR_RDM_Pos;
}

static inline hri_icm_isr_reg_t hri_icm_get_ISR_RBE_bf(const void *const hw, hri_icm_isr_reg_t mask)
{
	return (((Icm *)hw)->ISR.reg & ICM_ISR_RBE(mask)) >> ICM_ISR_RBE_Pos;
}

static inline hri_icm_isr_reg_t hri_icm_read_ISR_RBE_bf(const void *const hw)
{
	return (((Icm *)hw)->ISR.reg & ICM_ISR_RBE_Msk) >> ICM_ISR_RBE_Pos;
}

static inline hri_icm_isr_reg_t hri_icm_get_ISR_RWC_bf(const void *const hw, hri_icm_isr_reg_t mask)
{
	return (((Icm *)hw)->ISR.reg & ICM_ISR_RWC(mask)) >> ICM_ISR_RWC_Pos;
}

static inline hri_icm_isr_reg_t hri_icm_read_ISR_RWC_bf(const void *const hw)
{
	return (((Icm *)hw)->ISR.reg & ICM_ISR_RWC_Msk) >> ICM_ISR_RWC_Pos;
}

static inline hri_icm_isr_reg_t hri_icm_get_ISR_REC_bf(const void *const hw, hri_icm_isr_reg_t mask)
{
	return (((Icm *)hw)->ISR.reg & ICM_ISR_REC(mask)) >> ICM_ISR_REC_Pos;
}

static inline hri_icm_isr_reg_t hri_icm_read_ISR_REC_bf(const void *const hw)
{
	return (((Icm *)hw)->ISR.reg & ICM_ISR_REC_Msk) >> ICM_ISR_REC_Pos;
}

static inline hri_icm_isr_reg_t hri_icm_get_ISR_RSU_bf(const void *const hw, hri_icm_isr_reg_t mask)
{
	return (((Icm *)hw)->ISR.reg & ICM_ISR_RSU(mask)) >> ICM_ISR_RSU_Pos;
}

static inline hri_icm_isr_reg_t hri_icm_read_ISR_RSU_bf(const void *const hw)
{
	return (((Icm *)hw)->ISR.reg & ICM_ISR_RSU_Msk) >> ICM_ISR_RSU_Pos;
}

static inline hri_icm_isr_reg_t hri_icm_get_ISR_reg(const void *const hw, hri_icm_isr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Icm *)hw)->ISR.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_icm_isr_reg_t hri_icm_read_ISR_reg(const void *const hw)
{
	return ((Icm *)hw)->ISR.reg;
}

static inline hri_icm_uasr_reg_t hri_icm_get_UASR_URAT_bf(const void *const hw, hri_icm_uasr_reg_t mask)
{
	return (((Icm *)hw)->UASR.reg & ICM_UASR_URAT(mask)) >> ICM_UASR_URAT_Pos;
}

static inline hri_icm_uasr_reg_t hri_icm_read_UASR_URAT_bf(const void *const hw)
{
	return (((Icm *)hw)->UASR.reg & ICM_UASR_URAT_Msk) >> ICM_UASR_URAT_Pos;
}

static inline hri_icm_uasr_reg_t hri_icm_get_UASR_reg(const void *const hw, hri_icm_uasr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Icm *)hw)->UASR.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_icm_uasr_reg_t hri_icm_read_UASR_reg(const void *const hw)
{
	return ((Icm *)hw)->UASR.reg;
}

#ifdef __cplusplus
}
#endif

#endif /* _HRI_ICM_D51_H_INCLUDED */
#endif /* _SAMD51_ICM_COMPONENT_ */
