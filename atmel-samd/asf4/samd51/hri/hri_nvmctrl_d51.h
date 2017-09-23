/**
 * \file
 *
 * \brief SAM NVMCTRL
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

#ifdef _SAMD51_NVMCTRL_COMPONENT_
#ifndef _HRI_NVMCTRL_D51_H_INCLUDED_
#define _HRI_NVMCTRL_D51_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <hal_atomic.h>

#if defined(ENABLE_NVMCTRL_CRITICAL_SECTIONS)
#define NVMCTRL_CRITICAL_SECTION_ENTER() CRITICAL_SECTION_ENTER()
#define NVMCTRL_CRITICAL_SECTION_LEAVE() CRITICAL_SECTION_LEAVE()
#else
#define NVMCTRL_CRITICAL_SECTION_ENTER()
#define NVMCTRL_CRITICAL_SECTION_LEAVE()
#endif

typedef uint16_t hri_nvmctrl_ctrla_reg_t;
typedef uint16_t hri_nvmctrl_ctrlb_reg_t;
typedef uint16_t hri_nvmctrl_intenset_reg_t;
typedef uint16_t hri_nvmctrl_intflag_reg_t;
typedef uint16_t hri_nvmctrl_status_reg_t;
typedef uint32_t hri_nvmctrl_addr_reg_t;
typedef uint32_t hri_nvmctrl_eccerr_reg_t;
typedef uint32_t hri_nvmctrl_param_reg_t;
typedef uint32_t hri_nvmctrl_pbldata_reg_t;
typedef uint32_t hri_nvmctrl_runlock_reg_t;
typedef uint32_t hri_nvmctrl_seestat_reg_t;
typedef uint8_t  hri_nvmctrl_dbgctrl_reg_t;
typedef uint8_t  hri_nvmctrl_seecfg_reg_t;

static inline void hri_nvmctrl_set_INTEN_DONE_bit(const void *const hw)
{
	((Nvmctrl *)hw)->INTENSET.reg = NVMCTRL_INTENSET_DONE;
}

static inline bool hri_nvmctrl_get_INTEN_DONE_bit(const void *const hw)
{
	return (((Nvmctrl *)hw)->INTENSET.reg & NVMCTRL_INTENSET_DONE) >> NVMCTRL_INTENSET_DONE_Pos;
}

static inline void hri_nvmctrl_write_INTEN_DONE_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Nvmctrl *)hw)->INTENCLR.reg = NVMCTRL_INTENSET_DONE;
	} else {
		((Nvmctrl *)hw)->INTENSET.reg = NVMCTRL_INTENSET_DONE;
	}
}

static inline void hri_nvmctrl_clear_INTEN_DONE_bit(const void *const hw)
{
	((Nvmctrl *)hw)->INTENCLR.reg = NVMCTRL_INTENSET_DONE;
}

static inline void hri_nvmctrl_set_INTEN_ADDRE_bit(const void *const hw)
{
	((Nvmctrl *)hw)->INTENSET.reg = NVMCTRL_INTENSET_ADDRE;
}

static inline bool hri_nvmctrl_get_INTEN_ADDRE_bit(const void *const hw)
{
	return (((Nvmctrl *)hw)->INTENSET.reg & NVMCTRL_INTENSET_ADDRE) >> NVMCTRL_INTENSET_ADDRE_Pos;
}

static inline void hri_nvmctrl_write_INTEN_ADDRE_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Nvmctrl *)hw)->INTENCLR.reg = NVMCTRL_INTENSET_ADDRE;
	} else {
		((Nvmctrl *)hw)->INTENSET.reg = NVMCTRL_INTENSET_ADDRE;
	}
}

static inline void hri_nvmctrl_clear_INTEN_ADDRE_bit(const void *const hw)
{
	((Nvmctrl *)hw)->INTENCLR.reg = NVMCTRL_INTENSET_ADDRE;
}

static inline void hri_nvmctrl_set_INTEN_PROGE_bit(const void *const hw)
{
	((Nvmctrl *)hw)->INTENSET.reg = NVMCTRL_INTENSET_PROGE;
}

static inline bool hri_nvmctrl_get_INTEN_PROGE_bit(const void *const hw)
{
	return (((Nvmctrl *)hw)->INTENSET.reg & NVMCTRL_INTENSET_PROGE) >> NVMCTRL_INTENSET_PROGE_Pos;
}

static inline void hri_nvmctrl_write_INTEN_PROGE_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Nvmctrl *)hw)->INTENCLR.reg = NVMCTRL_INTENSET_PROGE;
	} else {
		((Nvmctrl *)hw)->INTENSET.reg = NVMCTRL_INTENSET_PROGE;
	}
}

static inline void hri_nvmctrl_clear_INTEN_PROGE_bit(const void *const hw)
{
	((Nvmctrl *)hw)->INTENCLR.reg = NVMCTRL_INTENSET_PROGE;
}

static inline void hri_nvmctrl_set_INTEN_LOCKE_bit(const void *const hw)
{
	((Nvmctrl *)hw)->INTENSET.reg = NVMCTRL_INTENSET_LOCKE;
}

static inline bool hri_nvmctrl_get_INTEN_LOCKE_bit(const void *const hw)
{
	return (((Nvmctrl *)hw)->INTENSET.reg & NVMCTRL_INTENSET_LOCKE) >> NVMCTRL_INTENSET_LOCKE_Pos;
}

static inline void hri_nvmctrl_write_INTEN_LOCKE_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Nvmctrl *)hw)->INTENCLR.reg = NVMCTRL_INTENSET_LOCKE;
	} else {
		((Nvmctrl *)hw)->INTENSET.reg = NVMCTRL_INTENSET_LOCKE;
	}
}

static inline void hri_nvmctrl_clear_INTEN_LOCKE_bit(const void *const hw)
{
	((Nvmctrl *)hw)->INTENCLR.reg = NVMCTRL_INTENSET_LOCKE;
}

static inline void hri_nvmctrl_set_INTEN_ECCSE_bit(const void *const hw)
{
	((Nvmctrl *)hw)->INTENSET.reg = NVMCTRL_INTENSET_ECCSE;
}

static inline bool hri_nvmctrl_get_INTEN_ECCSE_bit(const void *const hw)
{
	return (((Nvmctrl *)hw)->INTENSET.reg & NVMCTRL_INTENSET_ECCSE) >> NVMCTRL_INTENSET_ECCSE_Pos;
}

static inline void hri_nvmctrl_write_INTEN_ECCSE_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Nvmctrl *)hw)->INTENCLR.reg = NVMCTRL_INTENSET_ECCSE;
	} else {
		((Nvmctrl *)hw)->INTENSET.reg = NVMCTRL_INTENSET_ECCSE;
	}
}

static inline void hri_nvmctrl_clear_INTEN_ECCSE_bit(const void *const hw)
{
	((Nvmctrl *)hw)->INTENCLR.reg = NVMCTRL_INTENSET_ECCSE;
}

static inline void hri_nvmctrl_set_INTEN_ECCDE_bit(const void *const hw)
{
	((Nvmctrl *)hw)->INTENSET.reg = NVMCTRL_INTENSET_ECCDE;
}

static inline bool hri_nvmctrl_get_INTEN_ECCDE_bit(const void *const hw)
{
	return (((Nvmctrl *)hw)->INTENSET.reg & NVMCTRL_INTENSET_ECCDE) >> NVMCTRL_INTENSET_ECCDE_Pos;
}

static inline void hri_nvmctrl_write_INTEN_ECCDE_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Nvmctrl *)hw)->INTENCLR.reg = NVMCTRL_INTENSET_ECCDE;
	} else {
		((Nvmctrl *)hw)->INTENSET.reg = NVMCTRL_INTENSET_ECCDE;
	}
}

static inline void hri_nvmctrl_clear_INTEN_ECCDE_bit(const void *const hw)
{
	((Nvmctrl *)hw)->INTENCLR.reg = NVMCTRL_INTENSET_ECCDE;
}

static inline void hri_nvmctrl_set_INTEN_NVME_bit(const void *const hw)
{
	((Nvmctrl *)hw)->INTENSET.reg = NVMCTRL_INTENSET_NVME;
}

static inline bool hri_nvmctrl_get_INTEN_NVME_bit(const void *const hw)
{
	return (((Nvmctrl *)hw)->INTENSET.reg & NVMCTRL_INTENSET_NVME) >> NVMCTRL_INTENSET_NVME_Pos;
}

static inline void hri_nvmctrl_write_INTEN_NVME_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Nvmctrl *)hw)->INTENCLR.reg = NVMCTRL_INTENSET_NVME;
	} else {
		((Nvmctrl *)hw)->INTENSET.reg = NVMCTRL_INTENSET_NVME;
	}
}

static inline void hri_nvmctrl_clear_INTEN_NVME_bit(const void *const hw)
{
	((Nvmctrl *)hw)->INTENCLR.reg = NVMCTRL_INTENSET_NVME;
}

static inline void hri_nvmctrl_set_INTEN_SUSP_bit(const void *const hw)
{
	((Nvmctrl *)hw)->INTENSET.reg = NVMCTRL_INTENSET_SUSP;
}

static inline bool hri_nvmctrl_get_INTEN_SUSP_bit(const void *const hw)
{
	return (((Nvmctrl *)hw)->INTENSET.reg & NVMCTRL_INTENSET_SUSP) >> NVMCTRL_INTENSET_SUSP_Pos;
}

static inline void hri_nvmctrl_write_INTEN_SUSP_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Nvmctrl *)hw)->INTENCLR.reg = NVMCTRL_INTENSET_SUSP;
	} else {
		((Nvmctrl *)hw)->INTENSET.reg = NVMCTRL_INTENSET_SUSP;
	}
}

static inline void hri_nvmctrl_clear_INTEN_SUSP_bit(const void *const hw)
{
	((Nvmctrl *)hw)->INTENCLR.reg = NVMCTRL_INTENSET_SUSP;
}

static inline void hri_nvmctrl_set_INTEN_SEESFULL_bit(const void *const hw)
{
	((Nvmctrl *)hw)->INTENSET.reg = NVMCTRL_INTENSET_SEESFULL;
}

static inline bool hri_nvmctrl_get_INTEN_SEESFULL_bit(const void *const hw)
{
	return (((Nvmctrl *)hw)->INTENSET.reg & NVMCTRL_INTENSET_SEESFULL) >> NVMCTRL_INTENSET_SEESFULL_Pos;
}

static inline void hri_nvmctrl_write_INTEN_SEESFULL_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Nvmctrl *)hw)->INTENCLR.reg = NVMCTRL_INTENSET_SEESFULL;
	} else {
		((Nvmctrl *)hw)->INTENSET.reg = NVMCTRL_INTENSET_SEESFULL;
	}
}

static inline void hri_nvmctrl_clear_INTEN_SEESFULL_bit(const void *const hw)
{
	((Nvmctrl *)hw)->INTENCLR.reg = NVMCTRL_INTENSET_SEESFULL;
}

static inline void hri_nvmctrl_set_INTEN_SEESOVF_bit(const void *const hw)
{
	((Nvmctrl *)hw)->INTENSET.reg = NVMCTRL_INTENSET_SEESOVF;
}

static inline bool hri_nvmctrl_get_INTEN_SEESOVF_bit(const void *const hw)
{
	return (((Nvmctrl *)hw)->INTENSET.reg & NVMCTRL_INTENSET_SEESOVF) >> NVMCTRL_INTENSET_SEESOVF_Pos;
}

static inline void hri_nvmctrl_write_INTEN_SEESOVF_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Nvmctrl *)hw)->INTENCLR.reg = NVMCTRL_INTENSET_SEESOVF;
	} else {
		((Nvmctrl *)hw)->INTENSET.reg = NVMCTRL_INTENSET_SEESOVF;
	}
}

static inline void hri_nvmctrl_clear_INTEN_SEESOVF_bit(const void *const hw)
{
	((Nvmctrl *)hw)->INTENCLR.reg = NVMCTRL_INTENSET_SEESOVF;
}

static inline void hri_nvmctrl_set_INTEN_SEEWRC_bit(const void *const hw)
{
	((Nvmctrl *)hw)->INTENSET.reg = NVMCTRL_INTENSET_SEEWRC;
}

static inline bool hri_nvmctrl_get_INTEN_SEEWRC_bit(const void *const hw)
{
	return (((Nvmctrl *)hw)->INTENSET.reg & NVMCTRL_INTENSET_SEEWRC) >> NVMCTRL_INTENSET_SEEWRC_Pos;
}

static inline void hri_nvmctrl_write_INTEN_SEEWRC_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Nvmctrl *)hw)->INTENCLR.reg = NVMCTRL_INTENSET_SEEWRC;
	} else {
		((Nvmctrl *)hw)->INTENSET.reg = NVMCTRL_INTENSET_SEEWRC;
	}
}

static inline void hri_nvmctrl_clear_INTEN_SEEWRC_bit(const void *const hw)
{
	((Nvmctrl *)hw)->INTENCLR.reg = NVMCTRL_INTENSET_SEEWRC;
}

static inline void hri_nvmctrl_set_INTEN_reg(const void *const hw, hri_nvmctrl_intenset_reg_t mask)
{
	((Nvmctrl *)hw)->INTENSET.reg = mask;
}

static inline hri_nvmctrl_intenset_reg_t hri_nvmctrl_get_INTEN_reg(const void *const          hw,
                                                                   hri_nvmctrl_intenset_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Nvmctrl *)hw)->INTENSET.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_nvmctrl_intenset_reg_t hri_nvmctrl_read_INTEN_reg(const void *const hw)
{
	return ((Nvmctrl *)hw)->INTENSET.reg;
}

static inline void hri_nvmctrl_write_INTEN_reg(const void *const hw, hri_nvmctrl_intenset_reg_t data)
{
	((Nvmctrl *)hw)->INTENSET.reg = data;
	((Nvmctrl *)hw)->INTENCLR.reg = ~data;
}

static inline void hri_nvmctrl_clear_INTEN_reg(const void *const hw, hri_nvmctrl_intenset_reg_t mask)
{
	((Nvmctrl *)hw)->INTENCLR.reg = mask;
}

static inline bool hri_nvmctrl_get_INTFLAG_DONE_bit(const void *const hw)
{
	return (((Nvmctrl *)hw)->INTFLAG.reg & NVMCTRL_INTFLAG_DONE) >> NVMCTRL_INTFLAG_DONE_Pos;
}

static inline void hri_nvmctrl_clear_INTFLAG_DONE_bit(const void *const hw)
{
	((Nvmctrl *)hw)->INTFLAG.reg = NVMCTRL_INTFLAG_DONE;
}

static inline bool hri_nvmctrl_get_INTFLAG_ADDRE_bit(const void *const hw)
{
	return (((Nvmctrl *)hw)->INTFLAG.reg & NVMCTRL_INTFLAG_ADDRE) >> NVMCTRL_INTFLAG_ADDRE_Pos;
}

static inline void hri_nvmctrl_clear_INTFLAG_ADDRE_bit(const void *const hw)
{
	((Nvmctrl *)hw)->INTFLAG.reg = NVMCTRL_INTFLAG_ADDRE;
}

static inline bool hri_nvmctrl_get_INTFLAG_PROGE_bit(const void *const hw)
{
	return (((Nvmctrl *)hw)->INTFLAG.reg & NVMCTRL_INTFLAG_PROGE) >> NVMCTRL_INTFLAG_PROGE_Pos;
}

static inline void hri_nvmctrl_clear_INTFLAG_PROGE_bit(const void *const hw)
{
	((Nvmctrl *)hw)->INTFLAG.reg = NVMCTRL_INTFLAG_PROGE;
}

static inline bool hri_nvmctrl_get_INTFLAG_LOCKE_bit(const void *const hw)
{
	return (((Nvmctrl *)hw)->INTFLAG.reg & NVMCTRL_INTFLAG_LOCKE) >> NVMCTRL_INTFLAG_LOCKE_Pos;
}

static inline void hri_nvmctrl_clear_INTFLAG_LOCKE_bit(const void *const hw)
{
	((Nvmctrl *)hw)->INTFLAG.reg = NVMCTRL_INTFLAG_LOCKE;
}

static inline bool hri_nvmctrl_get_INTFLAG_ECCSE_bit(const void *const hw)
{
	return (((Nvmctrl *)hw)->INTFLAG.reg & NVMCTRL_INTFLAG_ECCSE) >> NVMCTRL_INTFLAG_ECCSE_Pos;
}

static inline void hri_nvmctrl_clear_INTFLAG_ECCSE_bit(const void *const hw)
{
	((Nvmctrl *)hw)->INTFLAG.reg = NVMCTRL_INTFLAG_ECCSE;
}

static inline bool hri_nvmctrl_get_INTFLAG_ECCDE_bit(const void *const hw)
{
	return (((Nvmctrl *)hw)->INTFLAG.reg & NVMCTRL_INTFLAG_ECCDE) >> NVMCTRL_INTFLAG_ECCDE_Pos;
}

static inline void hri_nvmctrl_clear_INTFLAG_ECCDE_bit(const void *const hw)
{
	((Nvmctrl *)hw)->INTFLAG.reg = NVMCTRL_INTFLAG_ECCDE;
}

static inline bool hri_nvmctrl_get_INTFLAG_NVME_bit(const void *const hw)
{
	return (((Nvmctrl *)hw)->INTFLAG.reg & NVMCTRL_INTFLAG_NVME) >> NVMCTRL_INTFLAG_NVME_Pos;
}

static inline void hri_nvmctrl_clear_INTFLAG_NVME_bit(const void *const hw)
{
	((Nvmctrl *)hw)->INTFLAG.reg = NVMCTRL_INTFLAG_NVME;
}

static inline bool hri_nvmctrl_get_INTFLAG_SUSP_bit(const void *const hw)
{
	return (((Nvmctrl *)hw)->INTFLAG.reg & NVMCTRL_INTFLAG_SUSP) >> NVMCTRL_INTFLAG_SUSP_Pos;
}

static inline void hri_nvmctrl_clear_INTFLAG_SUSP_bit(const void *const hw)
{
	((Nvmctrl *)hw)->INTFLAG.reg = NVMCTRL_INTFLAG_SUSP;
}

static inline bool hri_nvmctrl_get_INTFLAG_SEESFULL_bit(const void *const hw)
{
	return (((Nvmctrl *)hw)->INTFLAG.reg & NVMCTRL_INTFLAG_SEESFULL) >> NVMCTRL_INTFLAG_SEESFULL_Pos;
}

static inline void hri_nvmctrl_clear_INTFLAG_SEESFULL_bit(const void *const hw)
{
	((Nvmctrl *)hw)->INTFLAG.reg = NVMCTRL_INTFLAG_SEESFULL;
}

static inline bool hri_nvmctrl_get_INTFLAG_SEESOVF_bit(const void *const hw)
{
	return (((Nvmctrl *)hw)->INTFLAG.reg & NVMCTRL_INTFLAG_SEESOVF) >> NVMCTRL_INTFLAG_SEESOVF_Pos;
}

static inline void hri_nvmctrl_clear_INTFLAG_SEESOVF_bit(const void *const hw)
{
	((Nvmctrl *)hw)->INTFLAG.reg = NVMCTRL_INTFLAG_SEESOVF;
}

static inline bool hri_nvmctrl_get_INTFLAG_SEEWRC_bit(const void *const hw)
{
	return (((Nvmctrl *)hw)->INTFLAG.reg & NVMCTRL_INTFLAG_SEEWRC) >> NVMCTRL_INTFLAG_SEEWRC_Pos;
}

static inline void hri_nvmctrl_clear_INTFLAG_SEEWRC_bit(const void *const hw)
{
	((Nvmctrl *)hw)->INTFLAG.reg = NVMCTRL_INTFLAG_SEEWRC;
}

static inline bool hri_nvmctrl_get_interrupt_DONE_bit(const void *const hw)
{
	return (((Nvmctrl *)hw)->INTFLAG.reg & NVMCTRL_INTFLAG_DONE) >> NVMCTRL_INTFLAG_DONE_Pos;
}

static inline void hri_nvmctrl_clear_interrupt_DONE_bit(const void *const hw)
{
	((Nvmctrl *)hw)->INTFLAG.reg = NVMCTRL_INTFLAG_DONE;
}

static inline bool hri_nvmctrl_get_interrupt_ADDRE_bit(const void *const hw)
{
	return (((Nvmctrl *)hw)->INTFLAG.reg & NVMCTRL_INTFLAG_ADDRE) >> NVMCTRL_INTFLAG_ADDRE_Pos;
}

static inline void hri_nvmctrl_clear_interrupt_ADDRE_bit(const void *const hw)
{
	((Nvmctrl *)hw)->INTFLAG.reg = NVMCTRL_INTFLAG_ADDRE;
}

static inline bool hri_nvmctrl_get_interrupt_PROGE_bit(const void *const hw)
{
	return (((Nvmctrl *)hw)->INTFLAG.reg & NVMCTRL_INTFLAG_PROGE) >> NVMCTRL_INTFLAG_PROGE_Pos;
}

static inline void hri_nvmctrl_clear_interrupt_PROGE_bit(const void *const hw)
{
	((Nvmctrl *)hw)->INTFLAG.reg = NVMCTRL_INTFLAG_PROGE;
}

static inline bool hri_nvmctrl_get_interrupt_LOCKE_bit(const void *const hw)
{
	return (((Nvmctrl *)hw)->INTFLAG.reg & NVMCTRL_INTFLAG_LOCKE) >> NVMCTRL_INTFLAG_LOCKE_Pos;
}

static inline void hri_nvmctrl_clear_interrupt_LOCKE_bit(const void *const hw)
{
	((Nvmctrl *)hw)->INTFLAG.reg = NVMCTRL_INTFLAG_LOCKE;
}

static inline bool hri_nvmctrl_get_interrupt_ECCSE_bit(const void *const hw)
{
	return (((Nvmctrl *)hw)->INTFLAG.reg & NVMCTRL_INTFLAG_ECCSE) >> NVMCTRL_INTFLAG_ECCSE_Pos;
}

static inline void hri_nvmctrl_clear_interrupt_ECCSE_bit(const void *const hw)
{
	((Nvmctrl *)hw)->INTFLAG.reg = NVMCTRL_INTFLAG_ECCSE;
}

static inline bool hri_nvmctrl_get_interrupt_ECCDE_bit(const void *const hw)
{
	return (((Nvmctrl *)hw)->INTFLAG.reg & NVMCTRL_INTFLAG_ECCDE) >> NVMCTRL_INTFLAG_ECCDE_Pos;
}

static inline void hri_nvmctrl_clear_interrupt_ECCDE_bit(const void *const hw)
{
	((Nvmctrl *)hw)->INTFLAG.reg = NVMCTRL_INTFLAG_ECCDE;
}

static inline bool hri_nvmctrl_get_interrupt_NVME_bit(const void *const hw)
{
	return (((Nvmctrl *)hw)->INTFLAG.reg & NVMCTRL_INTFLAG_NVME) >> NVMCTRL_INTFLAG_NVME_Pos;
}

static inline void hri_nvmctrl_clear_interrupt_NVME_bit(const void *const hw)
{
	((Nvmctrl *)hw)->INTFLAG.reg = NVMCTRL_INTFLAG_NVME;
}

static inline bool hri_nvmctrl_get_interrupt_SUSP_bit(const void *const hw)
{
	return (((Nvmctrl *)hw)->INTFLAG.reg & NVMCTRL_INTFLAG_SUSP) >> NVMCTRL_INTFLAG_SUSP_Pos;
}

static inline void hri_nvmctrl_clear_interrupt_SUSP_bit(const void *const hw)
{
	((Nvmctrl *)hw)->INTFLAG.reg = NVMCTRL_INTFLAG_SUSP;
}

static inline bool hri_nvmctrl_get_interrupt_SEESFULL_bit(const void *const hw)
{
	return (((Nvmctrl *)hw)->INTFLAG.reg & NVMCTRL_INTFLAG_SEESFULL) >> NVMCTRL_INTFLAG_SEESFULL_Pos;
}

static inline void hri_nvmctrl_clear_interrupt_SEESFULL_bit(const void *const hw)
{
	((Nvmctrl *)hw)->INTFLAG.reg = NVMCTRL_INTFLAG_SEESFULL;
}

static inline bool hri_nvmctrl_get_interrupt_SEESOVF_bit(const void *const hw)
{
	return (((Nvmctrl *)hw)->INTFLAG.reg & NVMCTRL_INTFLAG_SEESOVF) >> NVMCTRL_INTFLAG_SEESOVF_Pos;
}

static inline void hri_nvmctrl_clear_interrupt_SEESOVF_bit(const void *const hw)
{
	((Nvmctrl *)hw)->INTFLAG.reg = NVMCTRL_INTFLAG_SEESOVF;
}

static inline bool hri_nvmctrl_get_interrupt_SEEWRC_bit(const void *const hw)
{
	return (((Nvmctrl *)hw)->INTFLAG.reg & NVMCTRL_INTFLAG_SEEWRC) >> NVMCTRL_INTFLAG_SEEWRC_Pos;
}

static inline void hri_nvmctrl_clear_interrupt_SEEWRC_bit(const void *const hw)
{
	((Nvmctrl *)hw)->INTFLAG.reg = NVMCTRL_INTFLAG_SEEWRC;
}

static inline hri_nvmctrl_intflag_reg_t hri_nvmctrl_get_INTFLAG_reg(const void *const         hw,
                                                                    hri_nvmctrl_intflag_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Nvmctrl *)hw)->INTFLAG.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_nvmctrl_intflag_reg_t hri_nvmctrl_read_INTFLAG_reg(const void *const hw)
{
	return ((Nvmctrl *)hw)->INTFLAG.reg;
}

static inline void hri_nvmctrl_clear_INTFLAG_reg(const void *const hw, hri_nvmctrl_intflag_reg_t mask)
{
	((Nvmctrl *)hw)->INTFLAG.reg = mask;
}

static inline void hri_nvmctrl_write_CTRLB_reg(const void *const hw, hri_nvmctrl_ctrlb_reg_t data)
{
	NVMCTRL_CRITICAL_SECTION_ENTER();
	((Nvmctrl *)hw)->CTRLB.reg = data;
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvmctrl_set_CTRLA_AUTOWS_bit(const void *const hw)
{
	NVMCTRL_CRITICAL_SECTION_ENTER();
	((Nvmctrl *)hw)->CTRLA.reg |= NVMCTRL_CTRLA_AUTOWS;
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_nvmctrl_get_CTRLA_AUTOWS_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Nvmctrl *)hw)->CTRLA.reg;
	tmp = (tmp & NVMCTRL_CTRLA_AUTOWS) >> NVMCTRL_CTRLA_AUTOWS_Pos;
	return (bool)tmp;
}

static inline void hri_nvmctrl_write_CTRLA_AUTOWS_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	NVMCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Nvmctrl *)hw)->CTRLA.reg;
	tmp &= ~NVMCTRL_CTRLA_AUTOWS;
	tmp |= value << NVMCTRL_CTRLA_AUTOWS_Pos;
	((Nvmctrl *)hw)->CTRLA.reg = tmp;
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvmctrl_clear_CTRLA_AUTOWS_bit(const void *const hw)
{
	NVMCTRL_CRITICAL_SECTION_ENTER();
	((Nvmctrl *)hw)->CTRLA.reg &= ~NVMCTRL_CTRLA_AUTOWS;
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvmctrl_toggle_CTRLA_AUTOWS_bit(const void *const hw)
{
	NVMCTRL_CRITICAL_SECTION_ENTER();
	((Nvmctrl *)hw)->CTRLA.reg ^= NVMCTRL_CTRLA_AUTOWS;
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvmctrl_set_CTRLA_SUSPEN_bit(const void *const hw)
{
	NVMCTRL_CRITICAL_SECTION_ENTER();
	((Nvmctrl *)hw)->CTRLA.reg |= NVMCTRL_CTRLA_SUSPEN;
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_nvmctrl_get_CTRLA_SUSPEN_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Nvmctrl *)hw)->CTRLA.reg;
	tmp = (tmp & NVMCTRL_CTRLA_SUSPEN) >> NVMCTRL_CTRLA_SUSPEN_Pos;
	return (bool)tmp;
}

static inline void hri_nvmctrl_write_CTRLA_SUSPEN_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	NVMCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Nvmctrl *)hw)->CTRLA.reg;
	tmp &= ~NVMCTRL_CTRLA_SUSPEN;
	tmp |= value << NVMCTRL_CTRLA_SUSPEN_Pos;
	((Nvmctrl *)hw)->CTRLA.reg = tmp;
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvmctrl_clear_CTRLA_SUSPEN_bit(const void *const hw)
{
	NVMCTRL_CRITICAL_SECTION_ENTER();
	((Nvmctrl *)hw)->CTRLA.reg &= ~NVMCTRL_CTRLA_SUSPEN;
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvmctrl_toggle_CTRLA_SUSPEN_bit(const void *const hw)
{
	NVMCTRL_CRITICAL_SECTION_ENTER();
	((Nvmctrl *)hw)->CTRLA.reg ^= NVMCTRL_CTRLA_SUSPEN;
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvmctrl_set_CTRLA_AHBNS0_bit(const void *const hw)
{
	NVMCTRL_CRITICAL_SECTION_ENTER();
	((Nvmctrl *)hw)->CTRLA.reg |= NVMCTRL_CTRLA_AHBNS0;
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_nvmctrl_get_CTRLA_AHBNS0_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Nvmctrl *)hw)->CTRLA.reg;
	tmp = (tmp & NVMCTRL_CTRLA_AHBNS0) >> NVMCTRL_CTRLA_AHBNS0_Pos;
	return (bool)tmp;
}

static inline void hri_nvmctrl_write_CTRLA_AHBNS0_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	NVMCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Nvmctrl *)hw)->CTRLA.reg;
	tmp &= ~NVMCTRL_CTRLA_AHBNS0;
	tmp |= value << NVMCTRL_CTRLA_AHBNS0_Pos;
	((Nvmctrl *)hw)->CTRLA.reg = tmp;
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvmctrl_clear_CTRLA_AHBNS0_bit(const void *const hw)
{
	NVMCTRL_CRITICAL_SECTION_ENTER();
	((Nvmctrl *)hw)->CTRLA.reg &= ~NVMCTRL_CTRLA_AHBNS0;
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvmctrl_toggle_CTRLA_AHBNS0_bit(const void *const hw)
{
	NVMCTRL_CRITICAL_SECTION_ENTER();
	((Nvmctrl *)hw)->CTRLA.reg ^= NVMCTRL_CTRLA_AHBNS0;
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvmctrl_set_CTRLA_AHBNS1_bit(const void *const hw)
{
	NVMCTRL_CRITICAL_SECTION_ENTER();
	((Nvmctrl *)hw)->CTRLA.reg |= NVMCTRL_CTRLA_AHBNS1;
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_nvmctrl_get_CTRLA_AHBNS1_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Nvmctrl *)hw)->CTRLA.reg;
	tmp = (tmp & NVMCTRL_CTRLA_AHBNS1) >> NVMCTRL_CTRLA_AHBNS1_Pos;
	return (bool)tmp;
}

static inline void hri_nvmctrl_write_CTRLA_AHBNS1_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	NVMCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Nvmctrl *)hw)->CTRLA.reg;
	tmp &= ~NVMCTRL_CTRLA_AHBNS1;
	tmp |= value << NVMCTRL_CTRLA_AHBNS1_Pos;
	((Nvmctrl *)hw)->CTRLA.reg = tmp;
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvmctrl_clear_CTRLA_AHBNS1_bit(const void *const hw)
{
	NVMCTRL_CRITICAL_SECTION_ENTER();
	((Nvmctrl *)hw)->CTRLA.reg &= ~NVMCTRL_CTRLA_AHBNS1;
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvmctrl_toggle_CTRLA_AHBNS1_bit(const void *const hw)
{
	NVMCTRL_CRITICAL_SECTION_ENTER();
	((Nvmctrl *)hw)->CTRLA.reg ^= NVMCTRL_CTRLA_AHBNS1;
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvmctrl_set_CTRLA_CACHEDIS0_bit(const void *const hw)
{
	NVMCTRL_CRITICAL_SECTION_ENTER();
	((Nvmctrl *)hw)->CTRLA.reg |= NVMCTRL_CTRLA_CACHEDIS0;
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_nvmctrl_get_CTRLA_CACHEDIS0_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Nvmctrl *)hw)->CTRLA.reg;
	tmp = (tmp & NVMCTRL_CTRLA_CACHEDIS0) >> NVMCTRL_CTRLA_CACHEDIS0_Pos;
	return (bool)tmp;
}

static inline void hri_nvmctrl_write_CTRLA_CACHEDIS0_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	NVMCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Nvmctrl *)hw)->CTRLA.reg;
	tmp &= ~NVMCTRL_CTRLA_CACHEDIS0;
	tmp |= value << NVMCTRL_CTRLA_CACHEDIS0_Pos;
	((Nvmctrl *)hw)->CTRLA.reg = tmp;
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvmctrl_clear_CTRLA_CACHEDIS0_bit(const void *const hw)
{
	NVMCTRL_CRITICAL_SECTION_ENTER();
	((Nvmctrl *)hw)->CTRLA.reg &= ~NVMCTRL_CTRLA_CACHEDIS0;
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvmctrl_toggle_CTRLA_CACHEDIS0_bit(const void *const hw)
{
	NVMCTRL_CRITICAL_SECTION_ENTER();
	((Nvmctrl *)hw)->CTRLA.reg ^= NVMCTRL_CTRLA_CACHEDIS0;
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvmctrl_set_CTRLA_CACHEDIS1_bit(const void *const hw)
{
	NVMCTRL_CRITICAL_SECTION_ENTER();
	((Nvmctrl *)hw)->CTRLA.reg |= NVMCTRL_CTRLA_CACHEDIS1;
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_nvmctrl_get_CTRLA_CACHEDIS1_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Nvmctrl *)hw)->CTRLA.reg;
	tmp = (tmp & NVMCTRL_CTRLA_CACHEDIS1) >> NVMCTRL_CTRLA_CACHEDIS1_Pos;
	return (bool)tmp;
}

static inline void hri_nvmctrl_write_CTRLA_CACHEDIS1_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	NVMCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Nvmctrl *)hw)->CTRLA.reg;
	tmp &= ~NVMCTRL_CTRLA_CACHEDIS1;
	tmp |= value << NVMCTRL_CTRLA_CACHEDIS1_Pos;
	((Nvmctrl *)hw)->CTRLA.reg = tmp;
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvmctrl_clear_CTRLA_CACHEDIS1_bit(const void *const hw)
{
	NVMCTRL_CRITICAL_SECTION_ENTER();
	((Nvmctrl *)hw)->CTRLA.reg &= ~NVMCTRL_CTRLA_CACHEDIS1;
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvmctrl_toggle_CTRLA_CACHEDIS1_bit(const void *const hw)
{
	NVMCTRL_CRITICAL_SECTION_ENTER();
	((Nvmctrl *)hw)->CTRLA.reg ^= NVMCTRL_CTRLA_CACHEDIS1;
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvmctrl_set_CTRLA_WMODE_bf(const void *const hw, hri_nvmctrl_ctrla_reg_t mask)
{
	NVMCTRL_CRITICAL_SECTION_ENTER();
	((Nvmctrl *)hw)->CTRLA.reg |= NVMCTRL_CTRLA_WMODE(mask);
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvmctrl_ctrla_reg_t hri_nvmctrl_get_CTRLA_WMODE_bf(const void *const hw, hri_nvmctrl_ctrla_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Nvmctrl *)hw)->CTRLA.reg;
	tmp = (tmp & NVMCTRL_CTRLA_WMODE(mask)) >> NVMCTRL_CTRLA_WMODE_Pos;
	return tmp;
}

static inline void hri_nvmctrl_write_CTRLA_WMODE_bf(const void *const hw, hri_nvmctrl_ctrla_reg_t data)
{
	uint16_t tmp;
	NVMCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Nvmctrl *)hw)->CTRLA.reg;
	tmp &= ~NVMCTRL_CTRLA_WMODE_Msk;
	tmp |= NVMCTRL_CTRLA_WMODE(data);
	((Nvmctrl *)hw)->CTRLA.reg = tmp;
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvmctrl_clear_CTRLA_WMODE_bf(const void *const hw, hri_nvmctrl_ctrla_reg_t mask)
{
	NVMCTRL_CRITICAL_SECTION_ENTER();
	((Nvmctrl *)hw)->CTRLA.reg &= ~NVMCTRL_CTRLA_WMODE(mask);
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvmctrl_toggle_CTRLA_WMODE_bf(const void *const hw, hri_nvmctrl_ctrla_reg_t mask)
{
	NVMCTRL_CRITICAL_SECTION_ENTER();
	((Nvmctrl *)hw)->CTRLA.reg ^= NVMCTRL_CTRLA_WMODE(mask);
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvmctrl_ctrla_reg_t hri_nvmctrl_read_CTRLA_WMODE_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Nvmctrl *)hw)->CTRLA.reg;
	tmp = (tmp & NVMCTRL_CTRLA_WMODE_Msk) >> NVMCTRL_CTRLA_WMODE_Pos;
	return tmp;
}

static inline void hri_nvmctrl_set_CTRLA_PRM_bf(const void *const hw, hri_nvmctrl_ctrla_reg_t mask)
{
	NVMCTRL_CRITICAL_SECTION_ENTER();
	((Nvmctrl *)hw)->CTRLA.reg |= NVMCTRL_CTRLA_PRM(mask);
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvmctrl_ctrla_reg_t hri_nvmctrl_get_CTRLA_PRM_bf(const void *const hw, hri_nvmctrl_ctrla_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Nvmctrl *)hw)->CTRLA.reg;
	tmp = (tmp & NVMCTRL_CTRLA_PRM(mask)) >> NVMCTRL_CTRLA_PRM_Pos;
	return tmp;
}

static inline void hri_nvmctrl_write_CTRLA_PRM_bf(const void *const hw, hri_nvmctrl_ctrla_reg_t data)
{
	uint16_t tmp;
	NVMCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Nvmctrl *)hw)->CTRLA.reg;
	tmp &= ~NVMCTRL_CTRLA_PRM_Msk;
	tmp |= NVMCTRL_CTRLA_PRM(data);
	((Nvmctrl *)hw)->CTRLA.reg = tmp;
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvmctrl_clear_CTRLA_PRM_bf(const void *const hw, hri_nvmctrl_ctrla_reg_t mask)
{
	NVMCTRL_CRITICAL_SECTION_ENTER();
	((Nvmctrl *)hw)->CTRLA.reg &= ~NVMCTRL_CTRLA_PRM(mask);
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvmctrl_toggle_CTRLA_PRM_bf(const void *const hw, hri_nvmctrl_ctrla_reg_t mask)
{
	NVMCTRL_CRITICAL_SECTION_ENTER();
	((Nvmctrl *)hw)->CTRLA.reg ^= NVMCTRL_CTRLA_PRM(mask);
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvmctrl_ctrla_reg_t hri_nvmctrl_read_CTRLA_PRM_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Nvmctrl *)hw)->CTRLA.reg;
	tmp = (tmp & NVMCTRL_CTRLA_PRM_Msk) >> NVMCTRL_CTRLA_PRM_Pos;
	return tmp;
}

static inline void hri_nvmctrl_set_CTRLA_RWS_bf(const void *const hw, hri_nvmctrl_ctrla_reg_t mask)
{
	NVMCTRL_CRITICAL_SECTION_ENTER();
	((Nvmctrl *)hw)->CTRLA.reg |= NVMCTRL_CTRLA_RWS(mask);
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvmctrl_ctrla_reg_t hri_nvmctrl_get_CTRLA_RWS_bf(const void *const hw, hri_nvmctrl_ctrla_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Nvmctrl *)hw)->CTRLA.reg;
	tmp = (tmp & NVMCTRL_CTRLA_RWS(mask)) >> NVMCTRL_CTRLA_RWS_Pos;
	return tmp;
}

static inline void hri_nvmctrl_write_CTRLA_RWS_bf(const void *const hw, hri_nvmctrl_ctrla_reg_t data)
{
	uint16_t tmp;
	NVMCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Nvmctrl *)hw)->CTRLA.reg;
	tmp &= ~NVMCTRL_CTRLA_RWS_Msk;
	tmp |= NVMCTRL_CTRLA_RWS(data);
	((Nvmctrl *)hw)->CTRLA.reg = tmp;
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvmctrl_clear_CTRLA_RWS_bf(const void *const hw, hri_nvmctrl_ctrla_reg_t mask)
{
	NVMCTRL_CRITICAL_SECTION_ENTER();
	((Nvmctrl *)hw)->CTRLA.reg &= ~NVMCTRL_CTRLA_RWS(mask);
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvmctrl_toggle_CTRLA_RWS_bf(const void *const hw, hri_nvmctrl_ctrla_reg_t mask)
{
	NVMCTRL_CRITICAL_SECTION_ENTER();
	((Nvmctrl *)hw)->CTRLA.reg ^= NVMCTRL_CTRLA_RWS(mask);
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvmctrl_ctrla_reg_t hri_nvmctrl_read_CTRLA_RWS_bf(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Nvmctrl *)hw)->CTRLA.reg;
	tmp = (tmp & NVMCTRL_CTRLA_RWS_Msk) >> NVMCTRL_CTRLA_RWS_Pos;
	return tmp;
}

static inline void hri_nvmctrl_set_CTRLA_reg(const void *const hw, hri_nvmctrl_ctrla_reg_t mask)
{
	NVMCTRL_CRITICAL_SECTION_ENTER();
	((Nvmctrl *)hw)->CTRLA.reg |= mask;
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvmctrl_ctrla_reg_t hri_nvmctrl_get_CTRLA_reg(const void *const hw, hri_nvmctrl_ctrla_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Nvmctrl *)hw)->CTRLA.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvmctrl_write_CTRLA_reg(const void *const hw, hri_nvmctrl_ctrla_reg_t data)
{
	NVMCTRL_CRITICAL_SECTION_ENTER();
	((Nvmctrl *)hw)->CTRLA.reg = data;
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvmctrl_clear_CTRLA_reg(const void *const hw, hri_nvmctrl_ctrla_reg_t mask)
{
	NVMCTRL_CRITICAL_SECTION_ENTER();
	((Nvmctrl *)hw)->CTRLA.reg &= ~mask;
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvmctrl_toggle_CTRLA_reg(const void *const hw, hri_nvmctrl_ctrla_reg_t mask)
{
	NVMCTRL_CRITICAL_SECTION_ENTER();
	((Nvmctrl *)hw)->CTRLA.reg ^= mask;
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvmctrl_ctrla_reg_t hri_nvmctrl_read_CTRLA_reg(const void *const hw)
{
	return ((Nvmctrl *)hw)->CTRLA.reg;
}

static inline void hri_nvmctrl_set_ADDR_ADDR_bf(const void *const hw, hri_nvmctrl_addr_reg_t mask)
{
	NVMCTRL_CRITICAL_SECTION_ENTER();
	((Nvmctrl *)hw)->ADDR.reg |= NVMCTRL_ADDR_ADDR(mask);
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvmctrl_addr_reg_t hri_nvmctrl_get_ADDR_ADDR_bf(const void *const hw, hri_nvmctrl_addr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvmctrl *)hw)->ADDR.reg;
	tmp = (tmp & NVMCTRL_ADDR_ADDR(mask)) >> NVMCTRL_ADDR_ADDR_Pos;
	return tmp;
}

static inline void hri_nvmctrl_write_ADDR_ADDR_bf(const void *const hw, hri_nvmctrl_addr_reg_t data)
{
	uint32_t tmp;
	NVMCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Nvmctrl *)hw)->ADDR.reg;
	tmp &= ~NVMCTRL_ADDR_ADDR_Msk;
	tmp |= NVMCTRL_ADDR_ADDR(data);
	((Nvmctrl *)hw)->ADDR.reg = tmp;
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvmctrl_clear_ADDR_ADDR_bf(const void *const hw, hri_nvmctrl_addr_reg_t mask)
{
	NVMCTRL_CRITICAL_SECTION_ENTER();
	((Nvmctrl *)hw)->ADDR.reg &= ~NVMCTRL_ADDR_ADDR(mask);
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvmctrl_toggle_ADDR_ADDR_bf(const void *const hw, hri_nvmctrl_addr_reg_t mask)
{
	NVMCTRL_CRITICAL_SECTION_ENTER();
	((Nvmctrl *)hw)->ADDR.reg ^= NVMCTRL_ADDR_ADDR(mask);
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvmctrl_addr_reg_t hri_nvmctrl_read_ADDR_ADDR_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Nvmctrl *)hw)->ADDR.reg;
	tmp = (tmp & NVMCTRL_ADDR_ADDR_Msk) >> NVMCTRL_ADDR_ADDR_Pos;
	return tmp;
}

static inline void hri_nvmctrl_set_ADDR_reg(const void *const hw, hri_nvmctrl_addr_reg_t mask)
{
	NVMCTRL_CRITICAL_SECTION_ENTER();
	((Nvmctrl *)hw)->ADDR.reg |= mask;
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvmctrl_addr_reg_t hri_nvmctrl_get_ADDR_reg(const void *const hw, hri_nvmctrl_addr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvmctrl *)hw)->ADDR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvmctrl_write_ADDR_reg(const void *const hw, hri_nvmctrl_addr_reg_t data)
{
	NVMCTRL_CRITICAL_SECTION_ENTER();
	((Nvmctrl *)hw)->ADDR.reg = data;
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvmctrl_clear_ADDR_reg(const void *const hw, hri_nvmctrl_addr_reg_t mask)
{
	NVMCTRL_CRITICAL_SECTION_ENTER();
	((Nvmctrl *)hw)->ADDR.reg &= ~mask;
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvmctrl_toggle_ADDR_reg(const void *const hw, hri_nvmctrl_addr_reg_t mask)
{
	NVMCTRL_CRITICAL_SECTION_ENTER();
	((Nvmctrl *)hw)->ADDR.reg ^= mask;
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvmctrl_addr_reg_t hri_nvmctrl_read_ADDR_reg(const void *const hw)
{
	return ((Nvmctrl *)hw)->ADDR.reg;
}

static inline void hri_nvmctrl_set_DBGCTRL_ECCDIS_bit(const void *const hw)
{
	NVMCTRL_CRITICAL_SECTION_ENTER();
	((Nvmctrl *)hw)->DBGCTRL.reg |= NVMCTRL_DBGCTRL_ECCDIS;
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_nvmctrl_get_DBGCTRL_ECCDIS_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvmctrl *)hw)->DBGCTRL.reg;
	tmp = (tmp & NVMCTRL_DBGCTRL_ECCDIS) >> NVMCTRL_DBGCTRL_ECCDIS_Pos;
	return (bool)tmp;
}

static inline void hri_nvmctrl_write_DBGCTRL_ECCDIS_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	NVMCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Nvmctrl *)hw)->DBGCTRL.reg;
	tmp &= ~NVMCTRL_DBGCTRL_ECCDIS;
	tmp |= value << NVMCTRL_DBGCTRL_ECCDIS_Pos;
	((Nvmctrl *)hw)->DBGCTRL.reg = tmp;
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvmctrl_clear_DBGCTRL_ECCDIS_bit(const void *const hw)
{
	NVMCTRL_CRITICAL_SECTION_ENTER();
	((Nvmctrl *)hw)->DBGCTRL.reg &= ~NVMCTRL_DBGCTRL_ECCDIS;
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvmctrl_toggle_DBGCTRL_ECCDIS_bit(const void *const hw)
{
	NVMCTRL_CRITICAL_SECTION_ENTER();
	((Nvmctrl *)hw)->DBGCTRL.reg ^= NVMCTRL_DBGCTRL_ECCDIS;
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvmctrl_set_DBGCTRL_ECCELOG_bit(const void *const hw)
{
	NVMCTRL_CRITICAL_SECTION_ENTER();
	((Nvmctrl *)hw)->DBGCTRL.reg |= NVMCTRL_DBGCTRL_ECCELOG;
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_nvmctrl_get_DBGCTRL_ECCELOG_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvmctrl *)hw)->DBGCTRL.reg;
	tmp = (tmp & NVMCTRL_DBGCTRL_ECCELOG) >> NVMCTRL_DBGCTRL_ECCELOG_Pos;
	return (bool)tmp;
}

static inline void hri_nvmctrl_write_DBGCTRL_ECCELOG_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	NVMCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Nvmctrl *)hw)->DBGCTRL.reg;
	tmp &= ~NVMCTRL_DBGCTRL_ECCELOG;
	tmp |= value << NVMCTRL_DBGCTRL_ECCELOG_Pos;
	((Nvmctrl *)hw)->DBGCTRL.reg = tmp;
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvmctrl_clear_DBGCTRL_ECCELOG_bit(const void *const hw)
{
	NVMCTRL_CRITICAL_SECTION_ENTER();
	((Nvmctrl *)hw)->DBGCTRL.reg &= ~NVMCTRL_DBGCTRL_ECCELOG;
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvmctrl_toggle_DBGCTRL_ECCELOG_bit(const void *const hw)
{
	NVMCTRL_CRITICAL_SECTION_ENTER();
	((Nvmctrl *)hw)->DBGCTRL.reg ^= NVMCTRL_DBGCTRL_ECCELOG;
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvmctrl_set_DBGCTRL_reg(const void *const hw, hri_nvmctrl_dbgctrl_reg_t mask)
{
	NVMCTRL_CRITICAL_SECTION_ENTER();
	((Nvmctrl *)hw)->DBGCTRL.reg |= mask;
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvmctrl_dbgctrl_reg_t hri_nvmctrl_get_DBGCTRL_reg(const void *const         hw,
                                                                    hri_nvmctrl_dbgctrl_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvmctrl *)hw)->DBGCTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvmctrl_write_DBGCTRL_reg(const void *const hw, hri_nvmctrl_dbgctrl_reg_t data)
{
	NVMCTRL_CRITICAL_SECTION_ENTER();
	((Nvmctrl *)hw)->DBGCTRL.reg = data;
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvmctrl_clear_DBGCTRL_reg(const void *const hw, hri_nvmctrl_dbgctrl_reg_t mask)
{
	NVMCTRL_CRITICAL_SECTION_ENTER();
	((Nvmctrl *)hw)->DBGCTRL.reg &= ~mask;
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvmctrl_toggle_DBGCTRL_reg(const void *const hw, hri_nvmctrl_dbgctrl_reg_t mask)
{
	NVMCTRL_CRITICAL_SECTION_ENTER();
	((Nvmctrl *)hw)->DBGCTRL.reg ^= mask;
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvmctrl_dbgctrl_reg_t hri_nvmctrl_read_DBGCTRL_reg(const void *const hw)
{
	return ((Nvmctrl *)hw)->DBGCTRL.reg;
}

static inline void hri_nvmctrl_set_SEECFG_WMODE_bit(const void *const hw)
{
	NVMCTRL_CRITICAL_SECTION_ENTER();
	((Nvmctrl *)hw)->SEECFG.reg |= NVMCTRL_SEECFG_WMODE;
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_nvmctrl_get_SEECFG_WMODE_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvmctrl *)hw)->SEECFG.reg;
	tmp = (tmp & NVMCTRL_SEECFG_WMODE) >> NVMCTRL_SEECFG_WMODE_Pos;
	return (bool)tmp;
}

static inline void hri_nvmctrl_write_SEECFG_WMODE_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	NVMCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Nvmctrl *)hw)->SEECFG.reg;
	tmp &= ~NVMCTRL_SEECFG_WMODE;
	tmp |= value << NVMCTRL_SEECFG_WMODE_Pos;
	((Nvmctrl *)hw)->SEECFG.reg = tmp;
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvmctrl_clear_SEECFG_WMODE_bit(const void *const hw)
{
	NVMCTRL_CRITICAL_SECTION_ENTER();
	((Nvmctrl *)hw)->SEECFG.reg &= ~NVMCTRL_SEECFG_WMODE;
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvmctrl_toggle_SEECFG_WMODE_bit(const void *const hw)
{
	NVMCTRL_CRITICAL_SECTION_ENTER();
	((Nvmctrl *)hw)->SEECFG.reg ^= NVMCTRL_SEECFG_WMODE;
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvmctrl_set_SEECFG_APRDIS_bit(const void *const hw)
{
	NVMCTRL_CRITICAL_SECTION_ENTER();
	((Nvmctrl *)hw)->SEECFG.reg |= NVMCTRL_SEECFG_APRDIS;
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_nvmctrl_get_SEECFG_APRDIS_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Nvmctrl *)hw)->SEECFG.reg;
	tmp = (tmp & NVMCTRL_SEECFG_APRDIS) >> NVMCTRL_SEECFG_APRDIS_Pos;
	return (bool)tmp;
}

static inline void hri_nvmctrl_write_SEECFG_APRDIS_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	NVMCTRL_CRITICAL_SECTION_ENTER();
	tmp = ((Nvmctrl *)hw)->SEECFG.reg;
	tmp &= ~NVMCTRL_SEECFG_APRDIS;
	tmp |= value << NVMCTRL_SEECFG_APRDIS_Pos;
	((Nvmctrl *)hw)->SEECFG.reg = tmp;
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvmctrl_clear_SEECFG_APRDIS_bit(const void *const hw)
{
	NVMCTRL_CRITICAL_SECTION_ENTER();
	((Nvmctrl *)hw)->SEECFG.reg &= ~NVMCTRL_SEECFG_APRDIS;
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvmctrl_toggle_SEECFG_APRDIS_bit(const void *const hw)
{
	NVMCTRL_CRITICAL_SECTION_ENTER();
	((Nvmctrl *)hw)->SEECFG.reg ^= NVMCTRL_SEECFG_APRDIS;
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvmctrl_set_SEECFG_reg(const void *const hw, hri_nvmctrl_seecfg_reg_t mask)
{
	NVMCTRL_CRITICAL_SECTION_ENTER();
	((Nvmctrl *)hw)->SEECFG.reg |= mask;
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvmctrl_seecfg_reg_t hri_nvmctrl_get_SEECFG_reg(const void *const hw, hri_nvmctrl_seecfg_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Nvmctrl *)hw)->SEECFG.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_nvmctrl_write_SEECFG_reg(const void *const hw, hri_nvmctrl_seecfg_reg_t data)
{
	NVMCTRL_CRITICAL_SECTION_ENTER();
	((Nvmctrl *)hw)->SEECFG.reg = data;
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvmctrl_clear_SEECFG_reg(const void *const hw, hri_nvmctrl_seecfg_reg_t mask)
{
	NVMCTRL_CRITICAL_SECTION_ENTER();
	((Nvmctrl *)hw)->SEECFG.reg &= ~mask;
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_nvmctrl_toggle_SEECFG_reg(const void *const hw, hri_nvmctrl_seecfg_reg_t mask)
{
	NVMCTRL_CRITICAL_SECTION_ENTER();
	((Nvmctrl *)hw)->SEECFG.reg ^= mask;
	NVMCTRL_CRITICAL_SECTION_LEAVE();
}

static inline hri_nvmctrl_seecfg_reg_t hri_nvmctrl_read_SEECFG_reg(const void *const hw)
{
	return ((Nvmctrl *)hw)->SEECFG.reg;
}

static inline bool hri_nvmctrl_get_PARAM_SEE_bit(const void *const hw)
{
	return (((Nvmctrl *)hw)->PARAM.reg & NVMCTRL_PARAM_SEE) >> NVMCTRL_PARAM_SEE_Pos;
}

static inline hri_nvmctrl_param_reg_t hri_nvmctrl_get_PARAM_NVMP_bf(const void *const hw, hri_nvmctrl_param_reg_t mask)
{
	return (((Nvmctrl *)hw)->PARAM.reg & NVMCTRL_PARAM_NVMP(mask)) >> NVMCTRL_PARAM_NVMP_Pos;
}

static inline hri_nvmctrl_param_reg_t hri_nvmctrl_read_PARAM_NVMP_bf(const void *const hw)
{
	return (((Nvmctrl *)hw)->PARAM.reg & NVMCTRL_PARAM_NVMP_Msk) >> NVMCTRL_PARAM_NVMP_Pos;
}

static inline hri_nvmctrl_param_reg_t hri_nvmctrl_get_PARAM_PSZ_bf(const void *const hw, hri_nvmctrl_param_reg_t mask)
{
	return (((Nvmctrl *)hw)->PARAM.reg & NVMCTRL_PARAM_PSZ(mask)) >> NVMCTRL_PARAM_PSZ_Pos;
}

static inline hri_nvmctrl_param_reg_t hri_nvmctrl_read_PARAM_PSZ_bf(const void *const hw)
{
	return (((Nvmctrl *)hw)->PARAM.reg & NVMCTRL_PARAM_PSZ_Msk) >> NVMCTRL_PARAM_PSZ_Pos;
}

static inline hri_nvmctrl_param_reg_t hri_nvmctrl_get_PARAM_reg(const void *const hw, hri_nvmctrl_param_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvmctrl *)hw)->PARAM.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_nvmctrl_param_reg_t hri_nvmctrl_read_PARAM_reg(const void *const hw)
{
	return ((Nvmctrl *)hw)->PARAM.reg;
}

static inline bool hri_nvmctrl_get_STATUS_READY_bit(const void *const hw)
{
	return (((Nvmctrl *)hw)->STATUS.reg & NVMCTRL_STATUS_READY) >> NVMCTRL_STATUS_READY_Pos;
}

static inline bool hri_nvmctrl_get_STATUS_PRM_bit(const void *const hw)
{
	return (((Nvmctrl *)hw)->STATUS.reg & NVMCTRL_STATUS_PRM) >> NVMCTRL_STATUS_PRM_Pos;
}

static inline bool hri_nvmctrl_get_STATUS_LOAD_bit(const void *const hw)
{
	return (((Nvmctrl *)hw)->STATUS.reg & NVMCTRL_STATUS_LOAD) >> NVMCTRL_STATUS_LOAD_Pos;
}

static inline bool hri_nvmctrl_get_STATUS_SUSP_bit(const void *const hw)
{
	return (((Nvmctrl *)hw)->STATUS.reg & NVMCTRL_STATUS_SUSP) >> NVMCTRL_STATUS_SUSP_Pos;
}

static inline bool hri_nvmctrl_get_STATUS_AFIRST_bit(const void *const hw)
{
	return (((Nvmctrl *)hw)->STATUS.reg & NVMCTRL_STATUS_AFIRST) >> NVMCTRL_STATUS_AFIRST_Pos;
}

static inline bool hri_nvmctrl_get_STATUS_BPDIS_bit(const void *const hw)
{
	return (((Nvmctrl *)hw)->STATUS.reg & NVMCTRL_STATUS_BPDIS) >> NVMCTRL_STATUS_BPDIS_Pos;
}

static inline hri_nvmctrl_status_reg_t hri_nvmctrl_get_STATUS_BOOTPROT_bf(const void *const        hw,
                                                                          hri_nvmctrl_status_reg_t mask)
{
	return (((Nvmctrl *)hw)->STATUS.reg & NVMCTRL_STATUS_BOOTPROT(mask)) >> NVMCTRL_STATUS_BOOTPROT_Pos;
}

static inline hri_nvmctrl_status_reg_t hri_nvmctrl_read_STATUS_BOOTPROT_bf(const void *const hw)
{
	return (((Nvmctrl *)hw)->STATUS.reg & NVMCTRL_STATUS_BOOTPROT_Msk) >> NVMCTRL_STATUS_BOOTPROT_Pos;
}

static inline hri_nvmctrl_status_reg_t hri_nvmctrl_get_STATUS_reg(const void *const hw, hri_nvmctrl_status_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Nvmctrl *)hw)->STATUS.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_nvmctrl_status_reg_t hri_nvmctrl_read_STATUS_reg(const void *const hw)
{
	return ((Nvmctrl *)hw)->STATUS.reg;
}

static inline hri_nvmctrl_runlock_reg_t hri_nvmctrl_get_RUNLOCK_RUNLOCK_bf(const void *const         hw,
                                                                           hri_nvmctrl_runlock_reg_t mask)
{
	return (((Nvmctrl *)hw)->RUNLOCK.reg & NVMCTRL_RUNLOCK_RUNLOCK(mask)) >> NVMCTRL_RUNLOCK_RUNLOCK_Pos;
}

static inline hri_nvmctrl_runlock_reg_t hri_nvmctrl_read_RUNLOCK_RUNLOCK_bf(const void *const hw)
{
	return (((Nvmctrl *)hw)->RUNLOCK.reg & NVMCTRL_RUNLOCK_RUNLOCK_Msk) >> NVMCTRL_RUNLOCK_RUNLOCK_Pos;
}

static inline hri_nvmctrl_runlock_reg_t hri_nvmctrl_get_RUNLOCK_reg(const void *const         hw,
                                                                    hri_nvmctrl_runlock_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvmctrl *)hw)->RUNLOCK.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_nvmctrl_runlock_reg_t hri_nvmctrl_read_RUNLOCK_reg(const void *const hw)
{
	return ((Nvmctrl *)hw)->RUNLOCK.reg;
}

static inline hri_nvmctrl_pbldata_reg_t hri_nvmctrl_get_PBLDATA_DATA_bf(const void *const hw, uint8_t index,
                                                                        hri_nvmctrl_pbldata_reg_t mask)
{
	return (((Nvmctrl *)hw)->PBLDATA[index].reg & NVMCTRL_PBLDATA_DATA(mask)) >> NVMCTRL_PBLDATA_DATA_Pos;
}

static inline hri_nvmctrl_pbldata_reg_t hri_nvmctrl_read_PBLDATA_DATA_bf(const void *const hw, uint8_t index)
{
	return (((Nvmctrl *)hw)->PBLDATA[index].reg & NVMCTRL_PBLDATA_DATA_Msk) >> NVMCTRL_PBLDATA_DATA_Pos;
}

static inline hri_nvmctrl_pbldata_reg_t hri_nvmctrl_get_PBLDATA_reg(const void *const hw, uint8_t index,
                                                                    hri_nvmctrl_pbldata_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvmctrl *)hw)->PBLDATA[index].reg;
	tmp &= mask;
	return tmp;
}

static inline hri_nvmctrl_pbldata_reg_t hri_nvmctrl_read_PBLDATA_reg(const void *const hw, uint8_t index)
{
	return ((Nvmctrl *)hw)->PBLDATA[index].reg;
}

static inline hri_nvmctrl_eccerr_reg_t hri_nvmctrl_get_ECCERR_ADDR_bf(const void *const        hw,
                                                                      hri_nvmctrl_eccerr_reg_t mask)
{
	return (((Nvmctrl *)hw)->ECCERR.reg & NVMCTRL_ECCERR_ADDR(mask)) >> NVMCTRL_ECCERR_ADDR_Pos;
}

static inline hri_nvmctrl_eccerr_reg_t hri_nvmctrl_read_ECCERR_ADDR_bf(const void *const hw)
{
	return (((Nvmctrl *)hw)->ECCERR.reg & NVMCTRL_ECCERR_ADDR_Msk) >> NVMCTRL_ECCERR_ADDR_Pos;
}

static inline hri_nvmctrl_eccerr_reg_t hri_nvmctrl_get_ECCERR_TYPEL_bf(const void *const        hw,
                                                                       hri_nvmctrl_eccerr_reg_t mask)
{
	return (((Nvmctrl *)hw)->ECCERR.reg & NVMCTRL_ECCERR_TYPEL(mask)) >> NVMCTRL_ECCERR_TYPEL_Pos;
}

static inline hri_nvmctrl_eccerr_reg_t hri_nvmctrl_read_ECCERR_TYPEL_bf(const void *const hw)
{
	return (((Nvmctrl *)hw)->ECCERR.reg & NVMCTRL_ECCERR_TYPEL_Msk) >> NVMCTRL_ECCERR_TYPEL_Pos;
}

static inline hri_nvmctrl_eccerr_reg_t hri_nvmctrl_get_ECCERR_TYPEH_bf(const void *const        hw,
                                                                       hri_nvmctrl_eccerr_reg_t mask)
{
	return (((Nvmctrl *)hw)->ECCERR.reg & NVMCTRL_ECCERR_TYPEH(mask)) >> NVMCTRL_ECCERR_TYPEH_Pos;
}

static inline hri_nvmctrl_eccerr_reg_t hri_nvmctrl_read_ECCERR_TYPEH_bf(const void *const hw)
{
	return (((Nvmctrl *)hw)->ECCERR.reg & NVMCTRL_ECCERR_TYPEH_Msk) >> NVMCTRL_ECCERR_TYPEH_Pos;
}

static inline hri_nvmctrl_eccerr_reg_t hri_nvmctrl_get_ECCERR_reg(const void *const hw, hri_nvmctrl_eccerr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvmctrl *)hw)->ECCERR.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_nvmctrl_eccerr_reg_t hri_nvmctrl_read_ECCERR_reg(const void *const hw)
{
	return ((Nvmctrl *)hw)->ECCERR.reg;
}

static inline bool hri_nvmctrl_get_SEESTAT_ASEES_bit(const void *const hw)
{
	return (((Nvmctrl *)hw)->SEESTAT.reg & NVMCTRL_SEESTAT_ASEES) >> NVMCTRL_SEESTAT_ASEES_Pos;
}

static inline bool hri_nvmctrl_get_SEESTAT_LOAD_bit(const void *const hw)
{
	return (((Nvmctrl *)hw)->SEESTAT.reg & NVMCTRL_SEESTAT_LOAD) >> NVMCTRL_SEESTAT_LOAD_Pos;
}

static inline bool hri_nvmctrl_get_SEESTAT_BUSY_bit(const void *const hw)
{
	return (((Nvmctrl *)hw)->SEESTAT.reg & NVMCTRL_SEESTAT_BUSY) >> NVMCTRL_SEESTAT_BUSY_Pos;
}

static inline bool hri_nvmctrl_get_SEESTAT_LOCK_bit(const void *const hw)
{
	return (((Nvmctrl *)hw)->SEESTAT.reg & NVMCTRL_SEESTAT_LOCK) >> NVMCTRL_SEESTAT_LOCK_Pos;
}

static inline bool hri_nvmctrl_get_SEESTAT_RLOCK_bit(const void *const hw)
{
	return (((Nvmctrl *)hw)->SEESTAT.reg & NVMCTRL_SEESTAT_RLOCK) >> NVMCTRL_SEESTAT_RLOCK_Pos;
}

static inline hri_nvmctrl_seestat_reg_t hri_nvmctrl_get_SEESTAT_SBLK_bf(const void *const         hw,
                                                                        hri_nvmctrl_seestat_reg_t mask)
{
	return (((Nvmctrl *)hw)->SEESTAT.reg & NVMCTRL_SEESTAT_SBLK(mask)) >> NVMCTRL_SEESTAT_SBLK_Pos;
}

static inline hri_nvmctrl_seestat_reg_t hri_nvmctrl_read_SEESTAT_SBLK_bf(const void *const hw)
{
	return (((Nvmctrl *)hw)->SEESTAT.reg & NVMCTRL_SEESTAT_SBLK_Msk) >> NVMCTRL_SEESTAT_SBLK_Pos;
}

static inline hri_nvmctrl_seestat_reg_t hri_nvmctrl_get_SEESTAT_PSZ_bf(const void *const         hw,
                                                                       hri_nvmctrl_seestat_reg_t mask)
{
	return (((Nvmctrl *)hw)->SEESTAT.reg & NVMCTRL_SEESTAT_PSZ(mask)) >> NVMCTRL_SEESTAT_PSZ_Pos;
}

static inline hri_nvmctrl_seestat_reg_t hri_nvmctrl_read_SEESTAT_PSZ_bf(const void *const hw)
{
	return (((Nvmctrl *)hw)->SEESTAT.reg & NVMCTRL_SEESTAT_PSZ_Msk) >> NVMCTRL_SEESTAT_PSZ_Pos;
}

static inline hri_nvmctrl_seestat_reg_t hri_nvmctrl_get_SEESTAT_reg(const void *const         hw,
                                                                    hri_nvmctrl_seestat_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Nvmctrl *)hw)->SEESTAT.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_nvmctrl_seestat_reg_t hri_nvmctrl_read_SEESTAT_reg(const void *const hw)
{
	return ((Nvmctrl *)hw)->SEESTAT.reg;
}

#ifdef __cplusplus
}
#endif

#endif /* _HRI_NVMCTRL_D51_H_INCLUDED */
#endif /* _SAMD51_NVMCTRL_COMPONENT_ */
