/**
 * \file
 *
 * \brief SAM TAL
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

#ifdef _SAMD51_TAL_COMPONENT_
#ifndef _HRI_TAL_D51_H_INCLUDED_
#define _HRI_TAL_D51_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <hal_atomic.h>

#if defined(ENABLE_TAL_CRITICAL_SECTIONS)
#define TAL_CRITICAL_SECTION_ENTER() CRITICAL_SECTION_ENTER()
#define TAL_CRITICAL_SECTION_LEAVE() CRITICAL_SECTION_LEAVE()
#else
#define TAL_CRITICAL_SECTION_ENTER()
#define TAL_CRITICAL_SECTION_LEAVE()
#endif

typedef uint16_t hri_tal_brkstatus_reg_t;
typedef uint16_t hri_tal_evctrl_reg_t;
typedef uint16_t hri_tal_irqmon_reg_t;
typedef uint32_t hri_tal_cpuirqs_reg_t;
typedef uint32_t hri_tal_dmacpusel0_reg_t;
typedef uint32_t hri_tal_dmacpusel1_reg_t;
typedef uint32_t hri_tal_eiccpusel0_reg_t;
typedef uint32_t hri_tal_evcpusel0_reg_t;
typedef uint32_t hri_tal_intcpusel0_reg_t;
typedef uint32_t hri_tal_intcpusel1_reg_t;
typedef uint32_t hri_tal_intcpusel2_reg_t;
typedef uint32_t hri_tal_intcpusel3_reg_t;
typedef uint32_t hri_tal_intcpusel4_reg_t;
typedef uint32_t hri_tal_intcpusel5_reg_t;
typedef uint32_t hri_tal_intcpusel6_reg_t;
typedef uint32_t hri_tal_intcpusel7_reg_t;
typedef uint32_t hri_tal_intcpusel8_reg_t;
typedef uint32_t hri_tal_irqtrig_reg_t;
typedef uint32_t hri_tal_sflag_reg_t;
typedef uint32_t hri_tal_smask_reg_t;
typedef uint32_t hri_talcpuirqs_cpuirqs_reg_t;
typedef uint32_t hri_talsmasks_smask_reg_t;
typedef uint8_t  hri_tal_ctictrla_reg_t;
typedef uint8_t  hri_tal_ctimask_reg_t;
typedef uint8_t  hri_tal_ctrla_reg_t;
typedef uint8_t  hri_tal_extctrl_reg_t;
typedef uint8_t  hri_tal_globmask_reg_t;
typedef uint8_t  hri_tal_halt_reg_t;
typedef uint8_t  hri_tal_intenset_reg_t;
typedef uint8_t  hri_tal_intflag_reg_t;
typedef uint8_t  hri_tal_intstatus_reg_t;
typedef uint8_t  hri_tal_restart_reg_t;
typedef uint8_t  hri_tal_sflagclrr_reg_t;
typedef uint8_t  hri_talctis_ctictrla_reg_t;
typedef uint8_t  hri_talctis_ctimask_reg_t;

static inline void hri_talsmasks_set_SMASK_IPS0_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg |= TAL_SMASK_IPS0;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_talsmasks_get_SMASK_IPS0_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp = (tmp & TAL_SMASK_IPS0) >> TAL_SMASK_IPS0_Pos;
	return (bool)tmp;
}

static inline void hri_talsmasks_write_SMASK_IPS0_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp &= ~TAL_SMASK_IPS0;
	tmp |= value << TAL_SMASK_IPS0_Pos;
	((TalSmasks *)hw)->SMASK[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_clear_SMASK_IPS0_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg &= ~TAL_SMASK_IPS0;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_toggle_SMASK_IPS0_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg ^= TAL_SMASK_IPS0;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_set_SMASK_IPS1_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg |= TAL_SMASK_IPS1;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_talsmasks_get_SMASK_IPS1_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp = (tmp & TAL_SMASK_IPS1) >> TAL_SMASK_IPS1_Pos;
	return (bool)tmp;
}

static inline void hri_talsmasks_write_SMASK_IPS1_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp &= ~TAL_SMASK_IPS1;
	tmp |= value << TAL_SMASK_IPS1_Pos;
	((TalSmasks *)hw)->SMASK[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_clear_SMASK_IPS1_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg &= ~TAL_SMASK_IPS1;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_toggle_SMASK_IPS1_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg ^= TAL_SMASK_IPS1;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_set_SMASK_IPS2_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg |= TAL_SMASK_IPS2;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_talsmasks_get_SMASK_IPS2_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp = (tmp & TAL_SMASK_IPS2) >> TAL_SMASK_IPS2_Pos;
	return (bool)tmp;
}

static inline void hri_talsmasks_write_SMASK_IPS2_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp &= ~TAL_SMASK_IPS2;
	tmp |= value << TAL_SMASK_IPS2_Pos;
	((TalSmasks *)hw)->SMASK[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_clear_SMASK_IPS2_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg &= ~TAL_SMASK_IPS2;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_toggle_SMASK_IPS2_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg ^= TAL_SMASK_IPS2;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_set_SMASK_IPS3_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg |= TAL_SMASK_IPS3;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_talsmasks_get_SMASK_IPS3_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp = (tmp & TAL_SMASK_IPS3) >> TAL_SMASK_IPS3_Pos;
	return (bool)tmp;
}

static inline void hri_talsmasks_write_SMASK_IPS3_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp &= ~TAL_SMASK_IPS3;
	tmp |= value << TAL_SMASK_IPS3_Pos;
	((TalSmasks *)hw)->SMASK[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_clear_SMASK_IPS3_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg &= ~TAL_SMASK_IPS3;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_toggle_SMASK_IPS3_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg ^= TAL_SMASK_IPS3;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_set_SMASK_IPS4_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg |= TAL_SMASK_IPS4;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_talsmasks_get_SMASK_IPS4_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp = (tmp & TAL_SMASK_IPS4) >> TAL_SMASK_IPS4_Pos;
	return (bool)tmp;
}

static inline void hri_talsmasks_write_SMASK_IPS4_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp &= ~TAL_SMASK_IPS4;
	tmp |= value << TAL_SMASK_IPS4_Pos;
	((TalSmasks *)hw)->SMASK[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_clear_SMASK_IPS4_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg &= ~TAL_SMASK_IPS4;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_toggle_SMASK_IPS4_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg ^= TAL_SMASK_IPS4;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_set_SMASK_IPS5_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg |= TAL_SMASK_IPS5;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_talsmasks_get_SMASK_IPS5_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp = (tmp & TAL_SMASK_IPS5) >> TAL_SMASK_IPS5_Pos;
	return (bool)tmp;
}

static inline void hri_talsmasks_write_SMASK_IPS5_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp &= ~TAL_SMASK_IPS5;
	tmp |= value << TAL_SMASK_IPS5_Pos;
	((TalSmasks *)hw)->SMASK[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_clear_SMASK_IPS5_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg &= ~TAL_SMASK_IPS5;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_toggle_SMASK_IPS5_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg ^= TAL_SMASK_IPS5;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_set_SMASK_IPS6_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg |= TAL_SMASK_IPS6;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_talsmasks_get_SMASK_IPS6_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp = (tmp & TAL_SMASK_IPS6) >> TAL_SMASK_IPS6_Pos;
	return (bool)tmp;
}

static inline void hri_talsmasks_write_SMASK_IPS6_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp &= ~TAL_SMASK_IPS6;
	tmp |= value << TAL_SMASK_IPS6_Pos;
	((TalSmasks *)hw)->SMASK[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_clear_SMASK_IPS6_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg &= ~TAL_SMASK_IPS6;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_toggle_SMASK_IPS6_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg ^= TAL_SMASK_IPS6;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_set_SMASK_IPS7_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg |= TAL_SMASK_IPS7;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_talsmasks_get_SMASK_IPS7_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp = (tmp & TAL_SMASK_IPS7) >> TAL_SMASK_IPS7_Pos;
	return (bool)tmp;
}

static inline void hri_talsmasks_write_SMASK_IPS7_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp &= ~TAL_SMASK_IPS7;
	tmp |= value << TAL_SMASK_IPS7_Pos;
	((TalSmasks *)hw)->SMASK[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_clear_SMASK_IPS7_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg &= ~TAL_SMASK_IPS7;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_toggle_SMASK_IPS7_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg ^= TAL_SMASK_IPS7;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_set_SMASK_IPS8_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg |= TAL_SMASK_IPS8;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_talsmasks_get_SMASK_IPS8_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp = (tmp & TAL_SMASK_IPS8) >> TAL_SMASK_IPS8_Pos;
	return (bool)tmp;
}

static inline void hri_talsmasks_write_SMASK_IPS8_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp &= ~TAL_SMASK_IPS8;
	tmp |= value << TAL_SMASK_IPS8_Pos;
	((TalSmasks *)hw)->SMASK[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_clear_SMASK_IPS8_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg &= ~TAL_SMASK_IPS8;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_toggle_SMASK_IPS8_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg ^= TAL_SMASK_IPS8;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_set_SMASK_IPS9_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg |= TAL_SMASK_IPS9;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_talsmasks_get_SMASK_IPS9_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp = (tmp & TAL_SMASK_IPS9) >> TAL_SMASK_IPS9_Pos;
	return (bool)tmp;
}

static inline void hri_talsmasks_write_SMASK_IPS9_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp &= ~TAL_SMASK_IPS9;
	tmp |= value << TAL_SMASK_IPS9_Pos;
	((TalSmasks *)hw)->SMASK[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_clear_SMASK_IPS9_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg &= ~TAL_SMASK_IPS9;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_toggle_SMASK_IPS9_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg ^= TAL_SMASK_IPS9;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_set_SMASK_IPS10_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg |= TAL_SMASK_IPS10;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_talsmasks_get_SMASK_IPS10_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp = (tmp & TAL_SMASK_IPS10) >> TAL_SMASK_IPS10_Pos;
	return (bool)tmp;
}

static inline void hri_talsmasks_write_SMASK_IPS10_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp &= ~TAL_SMASK_IPS10;
	tmp |= value << TAL_SMASK_IPS10_Pos;
	((TalSmasks *)hw)->SMASK[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_clear_SMASK_IPS10_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg &= ~TAL_SMASK_IPS10;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_toggle_SMASK_IPS10_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg ^= TAL_SMASK_IPS10;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_set_SMASK_IPS11_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg |= TAL_SMASK_IPS11;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_talsmasks_get_SMASK_IPS11_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp = (tmp & TAL_SMASK_IPS11) >> TAL_SMASK_IPS11_Pos;
	return (bool)tmp;
}

static inline void hri_talsmasks_write_SMASK_IPS11_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp &= ~TAL_SMASK_IPS11;
	tmp |= value << TAL_SMASK_IPS11_Pos;
	((TalSmasks *)hw)->SMASK[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_clear_SMASK_IPS11_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg &= ~TAL_SMASK_IPS11;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_toggle_SMASK_IPS11_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg ^= TAL_SMASK_IPS11;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_set_SMASK_IPS12_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg |= TAL_SMASK_IPS12;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_talsmasks_get_SMASK_IPS12_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp = (tmp & TAL_SMASK_IPS12) >> TAL_SMASK_IPS12_Pos;
	return (bool)tmp;
}

static inline void hri_talsmasks_write_SMASK_IPS12_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp &= ~TAL_SMASK_IPS12;
	tmp |= value << TAL_SMASK_IPS12_Pos;
	((TalSmasks *)hw)->SMASK[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_clear_SMASK_IPS12_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg &= ~TAL_SMASK_IPS12;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_toggle_SMASK_IPS12_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg ^= TAL_SMASK_IPS12;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_set_SMASK_IPS13_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg |= TAL_SMASK_IPS13;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_talsmasks_get_SMASK_IPS13_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp = (tmp & TAL_SMASK_IPS13) >> TAL_SMASK_IPS13_Pos;
	return (bool)tmp;
}

static inline void hri_talsmasks_write_SMASK_IPS13_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp &= ~TAL_SMASK_IPS13;
	tmp |= value << TAL_SMASK_IPS13_Pos;
	((TalSmasks *)hw)->SMASK[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_clear_SMASK_IPS13_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg &= ~TAL_SMASK_IPS13;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_toggle_SMASK_IPS13_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg ^= TAL_SMASK_IPS13;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_set_SMASK_IPS14_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg |= TAL_SMASK_IPS14;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_talsmasks_get_SMASK_IPS14_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp = (tmp & TAL_SMASK_IPS14) >> TAL_SMASK_IPS14_Pos;
	return (bool)tmp;
}

static inline void hri_talsmasks_write_SMASK_IPS14_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp &= ~TAL_SMASK_IPS14;
	tmp |= value << TAL_SMASK_IPS14_Pos;
	((TalSmasks *)hw)->SMASK[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_clear_SMASK_IPS14_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg &= ~TAL_SMASK_IPS14;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_toggle_SMASK_IPS14_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg ^= TAL_SMASK_IPS14;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_set_SMASK_IPS15_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg |= TAL_SMASK_IPS15;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_talsmasks_get_SMASK_IPS15_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp = (tmp & TAL_SMASK_IPS15) >> TAL_SMASK_IPS15_Pos;
	return (bool)tmp;
}

static inline void hri_talsmasks_write_SMASK_IPS15_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp &= ~TAL_SMASK_IPS15;
	tmp |= value << TAL_SMASK_IPS15_Pos;
	((TalSmasks *)hw)->SMASK[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_clear_SMASK_IPS15_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg &= ~TAL_SMASK_IPS15;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_toggle_SMASK_IPS15_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg ^= TAL_SMASK_IPS15;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_set_SMASK_IPS16_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg |= TAL_SMASK_IPS16;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_talsmasks_get_SMASK_IPS16_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp = (tmp & TAL_SMASK_IPS16) >> TAL_SMASK_IPS16_Pos;
	return (bool)tmp;
}

static inline void hri_talsmasks_write_SMASK_IPS16_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp &= ~TAL_SMASK_IPS16;
	tmp |= value << TAL_SMASK_IPS16_Pos;
	((TalSmasks *)hw)->SMASK[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_clear_SMASK_IPS16_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg &= ~TAL_SMASK_IPS16;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_toggle_SMASK_IPS16_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg ^= TAL_SMASK_IPS16;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_set_SMASK_IPS17_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg |= TAL_SMASK_IPS17;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_talsmasks_get_SMASK_IPS17_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp = (tmp & TAL_SMASK_IPS17) >> TAL_SMASK_IPS17_Pos;
	return (bool)tmp;
}

static inline void hri_talsmasks_write_SMASK_IPS17_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp &= ~TAL_SMASK_IPS17;
	tmp |= value << TAL_SMASK_IPS17_Pos;
	((TalSmasks *)hw)->SMASK[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_clear_SMASK_IPS17_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg &= ~TAL_SMASK_IPS17;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_toggle_SMASK_IPS17_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg ^= TAL_SMASK_IPS17;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_set_SMASK_IPS18_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg |= TAL_SMASK_IPS18;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_talsmasks_get_SMASK_IPS18_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp = (tmp & TAL_SMASK_IPS18) >> TAL_SMASK_IPS18_Pos;
	return (bool)tmp;
}

static inline void hri_talsmasks_write_SMASK_IPS18_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp &= ~TAL_SMASK_IPS18;
	tmp |= value << TAL_SMASK_IPS18_Pos;
	((TalSmasks *)hw)->SMASK[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_clear_SMASK_IPS18_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg &= ~TAL_SMASK_IPS18;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_toggle_SMASK_IPS18_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg ^= TAL_SMASK_IPS18;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_set_SMASK_IPS19_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg |= TAL_SMASK_IPS19;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_talsmasks_get_SMASK_IPS19_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp = (tmp & TAL_SMASK_IPS19) >> TAL_SMASK_IPS19_Pos;
	return (bool)tmp;
}

static inline void hri_talsmasks_write_SMASK_IPS19_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp &= ~TAL_SMASK_IPS19;
	tmp |= value << TAL_SMASK_IPS19_Pos;
	((TalSmasks *)hw)->SMASK[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_clear_SMASK_IPS19_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg &= ~TAL_SMASK_IPS19;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_toggle_SMASK_IPS19_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg ^= TAL_SMASK_IPS19;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_set_SMASK_IPS20_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg |= TAL_SMASK_IPS20;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_talsmasks_get_SMASK_IPS20_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp = (tmp & TAL_SMASK_IPS20) >> TAL_SMASK_IPS20_Pos;
	return (bool)tmp;
}

static inline void hri_talsmasks_write_SMASK_IPS20_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp &= ~TAL_SMASK_IPS20;
	tmp |= value << TAL_SMASK_IPS20_Pos;
	((TalSmasks *)hw)->SMASK[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_clear_SMASK_IPS20_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg &= ~TAL_SMASK_IPS20;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_toggle_SMASK_IPS20_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg ^= TAL_SMASK_IPS20;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_set_SMASK_IPS21_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg |= TAL_SMASK_IPS21;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_talsmasks_get_SMASK_IPS21_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp = (tmp & TAL_SMASK_IPS21) >> TAL_SMASK_IPS21_Pos;
	return (bool)tmp;
}

static inline void hri_talsmasks_write_SMASK_IPS21_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp &= ~TAL_SMASK_IPS21;
	tmp |= value << TAL_SMASK_IPS21_Pos;
	((TalSmasks *)hw)->SMASK[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_clear_SMASK_IPS21_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg &= ~TAL_SMASK_IPS21;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_toggle_SMASK_IPS21_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg ^= TAL_SMASK_IPS21;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_set_SMASK_IPS22_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg |= TAL_SMASK_IPS22;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_talsmasks_get_SMASK_IPS22_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp = (tmp & TAL_SMASK_IPS22) >> TAL_SMASK_IPS22_Pos;
	return (bool)tmp;
}

static inline void hri_talsmasks_write_SMASK_IPS22_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp &= ~TAL_SMASK_IPS22;
	tmp |= value << TAL_SMASK_IPS22_Pos;
	((TalSmasks *)hw)->SMASK[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_clear_SMASK_IPS22_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg &= ~TAL_SMASK_IPS22;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_toggle_SMASK_IPS22_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg ^= TAL_SMASK_IPS22;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_set_SMASK_IPS23_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg |= TAL_SMASK_IPS23;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_talsmasks_get_SMASK_IPS23_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp = (tmp & TAL_SMASK_IPS23) >> TAL_SMASK_IPS23_Pos;
	return (bool)tmp;
}

static inline void hri_talsmasks_write_SMASK_IPS23_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp &= ~TAL_SMASK_IPS23;
	tmp |= value << TAL_SMASK_IPS23_Pos;
	((TalSmasks *)hw)->SMASK[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_clear_SMASK_IPS23_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg &= ~TAL_SMASK_IPS23;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_toggle_SMASK_IPS23_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg ^= TAL_SMASK_IPS23;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_set_SMASK_IPS24_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg |= TAL_SMASK_IPS24;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_talsmasks_get_SMASK_IPS24_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp = (tmp & TAL_SMASK_IPS24) >> TAL_SMASK_IPS24_Pos;
	return (bool)tmp;
}

static inline void hri_talsmasks_write_SMASK_IPS24_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp &= ~TAL_SMASK_IPS24;
	tmp |= value << TAL_SMASK_IPS24_Pos;
	((TalSmasks *)hw)->SMASK[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_clear_SMASK_IPS24_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg &= ~TAL_SMASK_IPS24;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_toggle_SMASK_IPS24_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg ^= TAL_SMASK_IPS24;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_set_SMASK_IPS25_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg |= TAL_SMASK_IPS25;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_talsmasks_get_SMASK_IPS25_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp = (tmp & TAL_SMASK_IPS25) >> TAL_SMASK_IPS25_Pos;
	return (bool)tmp;
}

static inline void hri_talsmasks_write_SMASK_IPS25_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp &= ~TAL_SMASK_IPS25;
	tmp |= value << TAL_SMASK_IPS25_Pos;
	((TalSmasks *)hw)->SMASK[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_clear_SMASK_IPS25_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg &= ~TAL_SMASK_IPS25;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_toggle_SMASK_IPS25_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg ^= TAL_SMASK_IPS25;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_set_SMASK_IPS26_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg |= TAL_SMASK_IPS26;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_talsmasks_get_SMASK_IPS26_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp = (tmp & TAL_SMASK_IPS26) >> TAL_SMASK_IPS26_Pos;
	return (bool)tmp;
}

static inline void hri_talsmasks_write_SMASK_IPS26_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp &= ~TAL_SMASK_IPS26;
	tmp |= value << TAL_SMASK_IPS26_Pos;
	((TalSmasks *)hw)->SMASK[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_clear_SMASK_IPS26_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg &= ~TAL_SMASK_IPS26;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_toggle_SMASK_IPS26_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg ^= TAL_SMASK_IPS26;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_set_SMASK_IPS27_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg |= TAL_SMASK_IPS27;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_talsmasks_get_SMASK_IPS27_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp = (tmp & TAL_SMASK_IPS27) >> TAL_SMASK_IPS27_Pos;
	return (bool)tmp;
}

static inline void hri_talsmasks_write_SMASK_IPS27_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp &= ~TAL_SMASK_IPS27;
	tmp |= value << TAL_SMASK_IPS27_Pos;
	((TalSmasks *)hw)->SMASK[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_clear_SMASK_IPS27_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg &= ~TAL_SMASK_IPS27;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_toggle_SMASK_IPS27_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg ^= TAL_SMASK_IPS27;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_set_SMASK_IPS28_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg |= TAL_SMASK_IPS28;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_talsmasks_get_SMASK_IPS28_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp = (tmp & TAL_SMASK_IPS28) >> TAL_SMASK_IPS28_Pos;
	return (bool)tmp;
}

static inline void hri_talsmasks_write_SMASK_IPS28_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp &= ~TAL_SMASK_IPS28;
	tmp |= value << TAL_SMASK_IPS28_Pos;
	((TalSmasks *)hw)->SMASK[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_clear_SMASK_IPS28_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg &= ~TAL_SMASK_IPS28;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_toggle_SMASK_IPS28_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg ^= TAL_SMASK_IPS28;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_set_SMASK_IPS29_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg |= TAL_SMASK_IPS29;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_talsmasks_get_SMASK_IPS29_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp = (tmp & TAL_SMASK_IPS29) >> TAL_SMASK_IPS29_Pos;
	return (bool)tmp;
}

static inline void hri_talsmasks_write_SMASK_IPS29_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp &= ~TAL_SMASK_IPS29;
	tmp |= value << TAL_SMASK_IPS29_Pos;
	((TalSmasks *)hw)->SMASK[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_clear_SMASK_IPS29_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg &= ~TAL_SMASK_IPS29;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_toggle_SMASK_IPS29_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg ^= TAL_SMASK_IPS29;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_set_SMASK_IPS30_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg |= TAL_SMASK_IPS30;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_talsmasks_get_SMASK_IPS30_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp = (tmp & TAL_SMASK_IPS30) >> TAL_SMASK_IPS30_Pos;
	return (bool)tmp;
}

static inline void hri_talsmasks_write_SMASK_IPS30_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp &= ~TAL_SMASK_IPS30;
	tmp |= value << TAL_SMASK_IPS30_Pos;
	((TalSmasks *)hw)->SMASK[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_clear_SMASK_IPS30_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg &= ~TAL_SMASK_IPS30;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_toggle_SMASK_IPS30_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg ^= TAL_SMASK_IPS30;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_set_SMASK_IPS31_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg |= TAL_SMASK_IPS31;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_talsmasks_get_SMASK_IPS31_bit(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp = (tmp & TAL_SMASK_IPS31) >> TAL_SMASK_IPS31_Pos;
	return (bool)tmp;
}

static inline void hri_talsmasks_write_SMASK_IPS31_bit(const void *const hw, uint8_t index, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp &= ~TAL_SMASK_IPS31;
	tmp |= value << TAL_SMASK_IPS31_Pos;
	((TalSmasks *)hw)->SMASK[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_clear_SMASK_IPS31_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg &= ~TAL_SMASK_IPS31;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_toggle_SMASK_IPS31_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg ^= TAL_SMASK_IPS31;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_set_SMASK_reg(const void *const hw, uint8_t index, hri_tal_smask_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg |= mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline hri_tal_smask_reg_t hri_talsmasks_get_SMASK_reg(const void *const hw, uint8_t index,
                                                              hri_tal_smask_reg_t mask)
{
	uint32_t tmp;
	tmp = ((TalSmasks *)hw)->SMASK[index].reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_talsmasks_write_SMASK_reg(const void *const hw, uint8_t index, hri_tal_smask_reg_t data)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg = data;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_clear_SMASK_reg(const void *const hw, uint8_t index, hri_tal_smask_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg &= ~mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talsmasks_toggle_SMASK_reg(const void *const hw, uint8_t index, hri_tal_smask_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalSmasks *)hw)->SMASK[index].reg ^= mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline hri_tal_smask_reg_t hri_talsmasks_read_SMASK_reg(const void *const hw, uint8_t index)
{
	return ((TalSmasks *)hw)->SMASK[index].reg;
}

static inline void hri_tal_set_SMASK_IPS0_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg |= TAL_SMASK_IPS0;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_SMASK_IPS0_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp = (tmp & TAL_SMASK_IPS0) >> TAL_SMASK_IPS0_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_SMASK_IPS0_bit(const void *const hw, uint8_t submodule_index, uint8_t index,
                                                bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp &= ~TAL_SMASK_IPS0;
	tmp |= value << TAL_SMASK_IPS0_Pos;
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_SMASK_IPS0_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg &= ~TAL_SMASK_IPS0;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_SMASK_IPS0_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg ^= TAL_SMASK_IPS0;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_SMASK_IPS1_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg |= TAL_SMASK_IPS1;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_SMASK_IPS1_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp = (tmp & TAL_SMASK_IPS1) >> TAL_SMASK_IPS1_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_SMASK_IPS1_bit(const void *const hw, uint8_t submodule_index, uint8_t index,
                                                bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp &= ~TAL_SMASK_IPS1;
	tmp |= value << TAL_SMASK_IPS1_Pos;
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_SMASK_IPS1_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg &= ~TAL_SMASK_IPS1;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_SMASK_IPS1_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg ^= TAL_SMASK_IPS1;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_SMASK_IPS2_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg |= TAL_SMASK_IPS2;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_SMASK_IPS2_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp = (tmp & TAL_SMASK_IPS2) >> TAL_SMASK_IPS2_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_SMASK_IPS2_bit(const void *const hw, uint8_t submodule_index, uint8_t index,
                                                bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp &= ~TAL_SMASK_IPS2;
	tmp |= value << TAL_SMASK_IPS2_Pos;
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_SMASK_IPS2_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg &= ~TAL_SMASK_IPS2;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_SMASK_IPS2_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg ^= TAL_SMASK_IPS2;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_SMASK_IPS3_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg |= TAL_SMASK_IPS3;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_SMASK_IPS3_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp = (tmp & TAL_SMASK_IPS3) >> TAL_SMASK_IPS3_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_SMASK_IPS3_bit(const void *const hw, uint8_t submodule_index, uint8_t index,
                                                bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp &= ~TAL_SMASK_IPS3;
	tmp |= value << TAL_SMASK_IPS3_Pos;
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_SMASK_IPS3_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg &= ~TAL_SMASK_IPS3;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_SMASK_IPS3_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg ^= TAL_SMASK_IPS3;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_SMASK_IPS4_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg |= TAL_SMASK_IPS4;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_SMASK_IPS4_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp = (tmp & TAL_SMASK_IPS4) >> TAL_SMASK_IPS4_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_SMASK_IPS4_bit(const void *const hw, uint8_t submodule_index, uint8_t index,
                                                bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp &= ~TAL_SMASK_IPS4;
	tmp |= value << TAL_SMASK_IPS4_Pos;
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_SMASK_IPS4_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg &= ~TAL_SMASK_IPS4;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_SMASK_IPS4_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg ^= TAL_SMASK_IPS4;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_SMASK_IPS5_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg |= TAL_SMASK_IPS5;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_SMASK_IPS5_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp = (tmp & TAL_SMASK_IPS5) >> TAL_SMASK_IPS5_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_SMASK_IPS5_bit(const void *const hw, uint8_t submodule_index, uint8_t index,
                                                bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp &= ~TAL_SMASK_IPS5;
	tmp |= value << TAL_SMASK_IPS5_Pos;
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_SMASK_IPS5_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg &= ~TAL_SMASK_IPS5;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_SMASK_IPS5_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg ^= TAL_SMASK_IPS5;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_SMASK_IPS6_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg |= TAL_SMASK_IPS6;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_SMASK_IPS6_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp = (tmp & TAL_SMASK_IPS6) >> TAL_SMASK_IPS6_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_SMASK_IPS6_bit(const void *const hw, uint8_t submodule_index, uint8_t index,
                                                bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp &= ~TAL_SMASK_IPS6;
	tmp |= value << TAL_SMASK_IPS6_Pos;
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_SMASK_IPS6_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg &= ~TAL_SMASK_IPS6;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_SMASK_IPS6_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg ^= TAL_SMASK_IPS6;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_SMASK_IPS7_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg |= TAL_SMASK_IPS7;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_SMASK_IPS7_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp = (tmp & TAL_SMASK_IPS7) >> TAL_SMASK_IPS7_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_SMASK_IPS7_bit(const void *const hw, uint8_t submodule_index, uint8_t index,
                                                bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp &= ~TAL_SMASK_IPS7;
	tmp |= value << TAL_SMASK_IPS7_Pos;
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_SMASK_IPS7_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg &= ~TAL_SMASK_IPS7;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_SMASK_IPS7_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg ^= TAL_SMASK_IPS7;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_SMASK_IPS8_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg |= TAL_SMASK_IPS8;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_SMASK_IPS8_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp = (tmp & TAL_SMASK_IPS8) >> TAL_SMASK_IPS8_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_SMASK_IPS8_bit(const void *const hw, uint8_t submodule_index, uint8_t index,
                                                bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp &= ~TAL_SMASK_IPS8;
	tmp |= value << TAL_SMASK_IPS8_Pos;
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_SMASK_IPS8_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg &= ~TAL_SMASK_IPS8;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_SMASK_IPS8_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg ^= TAL_SMASK_IPS8;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_SMASK_IPS9_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg |= TAL_SMASK_IPS9;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_SMASK_IPS9_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp = (tmp & TAL_SMASK_IPS9) >> TAL_SMASK_IPS9_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_SMASK_IPS9_bit(const void *const hw, uint8_t submodule_index, uint8_t index,
                                                bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp &= ~TAL_SMASK_IPS9;
	tmp |= value << TAL_SMASK_IPS9_Pos;
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_SMASK_IPS9_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg &= ~TAL_SMASK_IPS9;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_SMASK_IPS9_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg ^= TAL_SMASK_IPS9;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_SMASK_IPS10_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg |= TAL_SMASK_IPS10;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_SMASK_IPS10_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp = (tmp & TAL_SMASK_IPS10) >> TAL_SMASK_IPS10_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_SMASK_IPS10_bit(const void *const hw, uint8_t submodule_index, uint8_t index,
                                                 bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp &= ~TAL_SMASK_IPS10;
	tmp |= value << TAL_SMASK_IPS10_Pos;
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_SMASK_IPS10_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg &= ~TAL_SMASK_IPS10;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_SMASK_IPS10_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg ^= TAL_SMASK_IPS10;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_SMASK_IPS11_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg |= TAL_SMASK_IPS11;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_SMASK_IPS11_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp = (tmp & TAL_SMASK_IPS11) >> TAL_SMASK_IPS11_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_SMASK_IPS11_bit(const void *const hw, uint8_t submodule_index, uint8_t index,
                                                 bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp &= ~TAL_SMASK_IPS11;
	tmp |= value << TAL_SMASK_IPS11_Pos;
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_SMASK_IPS11_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg &= ~TAL_SMASK_IPS11;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_SMASK_IPS11_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg ^= TAL_SMASK_IPS11;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_SMASK_IPS12_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg |= TAL_SMASK_IPS12;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_SMASK_IPS12_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp = (tmp & TAL_SMASK_IPS12) >> TAL_SMASK_IPS12_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_SMASK_IPS12_bit(const void *const hw, uint8_t submodule_index, uint8_t index,
                                                 bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp &= ~TAL_SMASK_IPS12;
	tmp |= value << TAL_SMASK_IPS12_Pos;
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_SMASK_IPS12_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg &= ~TAL_SMASK_IPS12;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_SMASK_IPS12_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg ^= TAL_SMASK_IPS12;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_SMASK_IPS13_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg |= TAL_SMASK_IPS13;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_SMASK_IPS13_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp = (tmp & TAL_SMASK_IPS13) >> TAL_SMASK_IPS13_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_SMASK_IPS13_bit(const void *const hw, uint8_t submodule_index, uint8_t index,
                                                 bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp &= ~TAL_SMASK_IPS13;
	tmp |= value << TAL_SMASK_IPS13_Pos;
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_SMASK_IPS13_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg &= ~TAL_SMASK_IPS13;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_SMASK_IPS13_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg ^= TAL_SMASK_IPS13;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_SMASK_IPS14_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg |= TAL_SMASK_IPS14;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_SMASK_IPS14_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp = (tmp & TAL_SMASK_IPS14) >> TAL_SMASK_IPS14_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_SMASK_IPS14_bit(const void *const hw, uint8_t submodule_index, uint8_t index,
                                                 bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp &= ~TAL_SMASK_IPS14;
	tmp |= value << TAL_SMASK_IPS14_Pos;
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_SMASK_IPS14_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg &= ~TAL_SMASK_IPS14;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_SMASK_IPS14_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg ^= TAL_SMASK_IPS14;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_SMASK_IPS15_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg |= TAL_SMASK_IPS15;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_SMASK_IPS15_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp = (tmp & TAL_SMASK_IPS15) >> TAL_SMASK_IPS15_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_SMASK_IPS15_bit(const void *const hw, uint8_t submodule_index, uint8_t index,
                                                 bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp &= ~TAL_SMASK_IPS15;
	tmp |= value << TAL_SMASK_IPS15_Pos;
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_SMASK_IPS15_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg &= ~TAL_SMASK_IPS15;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_SMASK_IPS15_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg ^= TAL_SMASK_IPS15;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_SMASK_IPS16_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg |= TAL_SMASK_IPS16;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_SMASK_IPS16_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp = (tmp & TAL_SMASK_IPS16) >> TAL_SMASK_IPS16_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_SMASK_IPS16_bit(const void *const hw, uint8_t submodule_index, uint8_t index,
                                                 bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp &= ~TAL_SMASK_IPS16;
	tmp |= value << TAL_SMASK_IPS16_Pos;
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_SMASK_IPS16_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg &= ~TAL_SMASK_IPS16;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_SMASK_IPS16_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg ^= TAL_SMASK_IPS16;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_SMASK_IPS17_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg |= TAL_SMASK_IPS17;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_SMASK_IPS17_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp = (tmp & TAL_SMASK_IPS17) >> TAL_SMASK_IPS17_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_SMASK_IPS17_bit(const void *const hw, uint8_t submodule_index, uint8_t index,
                                                 bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp &= ~TAL_SMASK_IPS17;
	tmp |= value << TAL_SMASK_IPS17_Pos;
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_SMASK_IPS17_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg &= ~TAL_SMASK_IPS17;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_SMASK_IPS17_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg ^= TAL_SMASK_IPS17;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_SMASK_IPS18_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg |= TAL_SMASK_IPS18;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_SMASK_IPS18_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp = (tmp & TAL_SMASK_IPS18) >> TAL_SMASK_IPS18_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_SMASK_IPS18_bit(const void *const hw, uint8_t submodule_index, uint8_t index,
                                                 bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp &= ~TAL_SMASK_IPS18;
	tmp |= value << TAL_SMASK_IPS18_Pos;
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_SMASK_IPS18_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg &= ~TAL_SMASK_IPS18;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_SMASK_IPS18_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg ^= TAL_SMASK_IPS18;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_SMASK_IPS19_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg |= TAL_SMASK_IPS19;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_SMASK_IPS19_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp = (tmp & TAL_SMASK_IPS19) >> TAL_SMASK_IPS19_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_SMASK_IPS19_bit(const void *const hw, uint8_t submodule_index, uint8_t index,
                                                 bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp &= ~TAL_SMASK_IPS19;
	tmp |= value << TAL_SMASK_IPS19_Pos;
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_SMASK_IPS19_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg &= ~TAL_SMASK_IPS19;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_SMASK_IPS19_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg ^= TAL_SMASK_IPS19;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_SMASK_IPS20_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg |= TAL_SMASK_IPS20;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_SMASK_IPS20_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp = (tmp & TAL_SMASK_IPS20) >> TAL_SMASK_IPS20_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_SMASK_IPS20_bit(const void *const hw, uint8_t submodule_index, uint8_t index,
                                                 bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp &= ~TAL_SMASK_IPS20;
	tmp |= value << TAL_SMASK_IPS20_Pos;
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_SMASK_IPS20_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg &= ~TAL_SMASK_IPS20;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_SMASK_IPS20_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg ^= TAL_SMASK_IPS20;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_SMASK_IPS21_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg |= TAL_SMASK_IPS21;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_SMASK_IPS21_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp = (tmp & TAL_SMASK_IPS21) >> TAL_SMASK_IPS21_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_SMASK_IPS21_bit(const void *const hw, uint8_t submodule_index, uint8_t index,
                                                 bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp &= ~TAL_SMASK_IPS21;
	tmp |= value << TAL_SMASK_IPS21_Pos;
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_SMASK_IPS21_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg &= ~TAL_SMASK_IPS21;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_SMASK_IPS21_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg ^= TAL_SMASK_IPS21;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_SMASK_IPS22_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg |= TAL_SMASK_IPS22;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_SMASK_IPS22_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp = (tmp & TAL_SMASK_IPS22) >> TAL_SMASK_IPS22_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_SMASK_IPS22_bit(const void *const hw, uint8_t submodule_index, uint8_t index,
                                                 bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp &= ~TAL_SMASK_IPS22;
	tmp |= value << TAL_SMASK_IPS22_Pos;
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_SMASK_IPS22_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg &= ~TAL_SMASK_IPS22;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_SMASK_IPS22_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg ^= TAL_SMASK_IPS22;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_SMASK_IPS23_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg |= TAL_SMASK_IPS23;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_SMASK_IPS23_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp = (tmp & TAL_SMASK_IPS23) >> TAL_SMASK_IPS23_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_SMASK_IPS23_bit(const void *const hw, uint8_t submodule_index, uint8_t index,
                                                 bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp &= ~TAL_SMASK_IPS23;
	tmp |= value << TAL_SMASK_IPS23_Pos;
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_SMASK_IPS23_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg &= ~TAL_SMASK_IPS23;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_SMASK_IPS23_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg ^= TAL_SMASK_IPS23;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_SMASK_IPS24_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg |= TAL_SMASK_IPS24;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_SMASK_IPS24_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp = (tmp & TAL_SMASK_IPS24) >> TAL_SMASK_IPS24_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_SMASK_IPS24_bit(const void *const hw, uint8_t submodule_index, uint8_t index,
                                                 bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp &= ~TAL_SMASK_IPS24;
	tmp |= value << TAL_SMASK_IPS24_Pos;
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_SMASK_IPS24_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg &= ~TAL_SMASK_IPS24;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_SMASK_IPS24_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg ^= TAL_SMASK_IPS24;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_SMASK_IPS25_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg |= TAL_SMASK_IPS25;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_SMASK_IPS25_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp = (tmp & TAL_SMASK_IPS25) >> TAL_SMASK_IPS25_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_SMASK_IPS25_bit(const void *const hw, uint8_t submodule_index, uint8_t index,
                                                 bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp &= ~TAL_SMASK_IPS25;
	tmp |= value << TAL_SMASK_IPS25_Pos;
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_SMASK_IPS25_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg &= ~TAL_SMASK_IPS25;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_SMASK_IPS25_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg ^= TAL_SMASK_IPS25;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_SMASK_IPS26_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg |= TAL_SMASK_IPS26;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_SMASK_IPS26_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp = (tmp & TAL_SMASK_IPS26) >> TAL_SMASK_IPS26_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_SMASK_IPS26_bit(const void *const hw, uint8_t submodule_index, uint8_t index,
                                                 bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp &= ~TAL_SMASK_IPS26;
	tmp |= value << TAL_SMASK_IPS26_Pos;
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_SMASK_IPS26_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg &= ~TAL_SMASK_IPS26;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_SMASK_IPS26_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg ^= TAL_SMASK_IPS26;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_SMASK_IPS27_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg |= TAL_SMASK_IPS27;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_SMASK_IPS27_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp = (tmp & TAL_SMASK_IPS27) >> TAL_SMASK_IPS27_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_SMASK_IPS27_bit(const void *const hw, uint8_t submodule_index, uint8_t index,
                                                 bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp &= ~TAL_SMASK_IPS27;
	tmp |= value << TAL_SMASK_IPS27_Pos;
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_SMASK_IPS27_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg &= ~TAL_SMASK_IPS27;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_SMASK_IPS27_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg ^= TAL_SMASK_IPS27;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_SMASK_IPS28_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg |= TAL_SMASK_IPS28;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_SMASK_IPS28_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp = (tmp & TAL_SMASK_IPS28) >> TAL_SMASK_IPS28_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_SMASK_IPS28_bit(const void *const hw, uint8_t submodule_index, uint8_t index,
                                                 bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp &= ~TAL_SMASK_IPS28;
	tmp |= value << TAL_SMASK_IPS28_Pos;
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_SMASK_IPS28_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg &= ~TAL_SMASK_IPS28;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_SMASK_IPS28_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg ^= TAL_SMASK_IPS28;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_SMASK_IPS29_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg |= TAL_SMASK_IPS29;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_SMASK_IPS29_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp = (tmp & TAL_SMASK_IPS29) >> TAL_SMASK_IPS29_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_SMASK_IPS29_bit(const void *const hw, uint8_t submodule_index, uint8_t index,
                                                 bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp &= ~TAL_SMASK_IPS29;
	tmp |= value << TAL_SMASK_IPS29_Pos;
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_SMASK_IPS29_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg &= ~TAL_SMASK_IPS29;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_SMASK_IPS29_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg ^= TAL_SMASK_IPS29;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_SMASK_IPS30_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg |= TAL_SMASK_IPS30;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_SMASK_IPS30_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp = (tmp & TAL_SMASK_IPS30) >> TAL_SMASK_IPS30_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_SMASK_IPS30_bit(const void *const hw, uint8_t submodule_index, uint8_t index,
                                                 bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp &= ~TAL_SMASK_IPS30;
	tmp |= value << TAL_SMASK_IPS30_Pos;
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_SMASK_IPS30_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg &= ~TAL_SMASK_IPS30;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_SMASK_IPS30_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg ^= TAL_SMASK_IPS30;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_SMASK_IPS31_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg |= TAL_SMASK_IPS31;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_SMASK_IPS31_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp = (tmp & TAL_SMASK_IPS31) >> TAL_SMASK_IPS31_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_SMASK_IPS31_bit(const void *const hw, uint8_t submodule_index, uint8_t index,
                                                 bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp &= ~TAL_SMASK_IPS31;
	tmp |= value << TAL_SMASK_IPS31_Pos;
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_SMASK_IPS31_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg &= ~TAL_SMASK_IPS31;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_SMASK_IPS31_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg ^= TAL_SMASK_IPS31;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_SMASK_reg(const void *const hw, uint8_t submodule_index, uint8_t index,
                                         hri_tal_smask_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg |= mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline hri_tal_smask_reg_t hri_tal_get_SMASK_reg(const void *const hw, uint8_t submodule_index, uint8_t index,
                                                        hri_tal_smask_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_tal_write_SMASK_reg(const void *const hw, uint8_t submodule_index, uint8_t index,
                                           hri_tal_smask_reg_t data)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg = data;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_SMASK_reg(const void *const hw, uint8_t submodule_index, uint8_t index,
                                           hri_tal_smask_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg &= ~mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_SMASK_reg(const void *const hw, uint8_t submodule_index, uint8_t index,
                                            hri_tal_smask_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg ^= mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline hri_tal_smask_reg_t hri_tal_read_SMASK_reg(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	return ((Tal *)hw)->Smasks[submodule_index].SMASK[index].reg;
}

static inline void hri_tal_set_INTEN_BRK_bit(const void *const hw)
{
	((Tal *)hw)->INTENSET.reg = TAL_INTENSET_BRK;
}

static inline bool hri_tal_get_INTEN_BRK_bit(const void *const hw)
{
	return (((Tal *)hw)->INTENSET.reg & TAL_INTENSET_BRK) >> TAL_INTENSET_BRK_Pos;
}

static inline void hri_tal_write_INTEN_BRK_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Tal *)hw)->INTENCLR.reg = TAL_INTENSET_BRK;
	} else {
		((Tal *)hw)->INTENSET.reg = TAL_INTENSET_BRK;
	}
}

static inline void hri_tal_clear_INTEN_BRK_bit(const void *const hw)
{
	((Tal *)hw)->INTENCLR.reg = TAL_INTENSET_BRK;
}

static inline void hri_tal_set_INTEN_IPS0_bit(const void *const hw)
{
	((Tal *)hw)->INTENSET.reg = TAL_INTENSET_IPS0;
}

static inline bool hri_tal_get_INTEN_IPS0_bit(const void *const hw)
{
	return (((Tal *)hw)->INTENSET.reg & TAL_INTENSET_IPS0) >> TAL_INTENSET_IPS0_Pos;
}

static inline void hri_tal_write_INTEN_IPS0_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Tal *)hw)->INTENCLR.reg = TAL_INTENSET_IPS0;
	} else {
		((Tal *)hw)->INTENSET.reg = TAL_INTENSET_IPS0;
	}
}

static inline void hri_tal_clear_INTEN_IPS0_bit(const void *const hw)
{
	((Tal *)hw)->INTENCLR.reg = TAL_INTENSET_IPS0;
}

static inline void hri_tal_set_INTEN_IPS1_bit(const void *const hw)
{
	((Tal *)hw)->INTENSET.reg = TAL_INTENSET_IPS1;
}

static inline bool hri_tal_get_INTEN_IPS1_bit(const void *const hw)
{
	return (((Tal *)hw)->INTENSET.reg & TAL_INTENSET_IPS1) >> TAL_INTENSET_IPS1_Pos;
}

static inline void hri_tal_write_INTEN_IPS1_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Tal *)hw)->INTENCLR.reg = TAL_INTENSET_IPS1;
	} else {
		((Tal *)hw)->INTENSET.reg = TAL_INTENSET_IPS1;
	}
}

static inline void hri_tal_clear_INTEN_IPS1_bit(const void *const hw)
{
	((Tal *)hw)->INTENCLR.reg = TAL_INTENSET_IPS1;
}

static inline void hri_tal_set_INTEN_reg(const void *const hw, hri_tal_intenset_reg_t mask)
{
	((Tal *)hw)->INTENSET.reg = mask;
}

static inline hri_tal_intenset_reg_t hri_tal_get_INTEN_reg(const void *const hw, hri_tal_intenset_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Tal *)hw)->INTENSET.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_tal_intenset_reg_t hri_tal_read_INTEN_reg(const void *const hw)
{
	return ((Tal *)hw)->INTENSET.reg;
}

static inline void hri_tal_write_INTEN_reg(const void *const hw, hri_tal_intenset_reg_t data)
{
	((Tal *)hw)->INTENSET.reg = data;
	((Tal *)hw)->INTENCLR.reg = ~data;
}

static inline void hri_tal_clear_INTEN_reg(const void *const hw, hri_tal_intenset_reg_t mask)
{
	((Tal *)hw)->INTENCLR.reg = mask;
}

static inline void hri_tal_set_SFLAG_IPS0_bit(const void *const hw, uint8_t index)
{
	((Tal *)hw)->SFLAGSET[index].reg = TAL_SFLAG_IPS0;
}

static inline bool hri_tal_get_SFLAG_IPS0_bit(const void *const hw, uint8_t index)
{
	return (((Tal *)hw)->SFLAG[index].reg & TAL_SFLAG_IPS0) >> TAL_SFLAG_IPS0_Pos;
}

static inline void hri_tal_write_SFLAG_IPS0_bit(const void *const hw, uint8_t index, bool value)
{
	if (value == 0x0) {
		((Tal *)hw)->SFLAGCLR[index].reg = TAL_SFLAG_IPS0;
	} else {
		((Tal *)hw)->SFLAGSET[index].reg = TAL_SFLAG_IPS0;
	}
}

static inline void hri_tal_clear_SFLAG_IPS0_bit(const void *const hw, uint8_t index)
{
	((Tal *)hw)->SFLAGCLR[index].reg = TAL_SFLAG_IPS0;
}

static inline void hri_tal_set_SFLAG_IPS1_bit(const void *const hw, uint8_t index)
{
	((Tal *)hw)->SFLAGSET[index].reg = TAL_SFLAG_IPS1;
}

static inline bool hri_tal_get_SFLAG_IPS1_bit(const void *const hw, uint8_t index)
{
	return (((Tal *)hw)->SFLAG[index].reg & TAL_SFLAG_IPS1) >> TAL_SFLAG_IPS1_Pos;
}

static inline void hri_tal_write_SFLAG_IPS1_bit(const void *const hw, uint8_t index, bool value)
{
	if (value == 0x0) {
		((Tal *)hw)->SFLAGCLR[index].reg = TAL_SFLAG_IPS1;
	} else {
		((Tal *)hw)->SFLAGSET[index].reg = TAL_SFLAG_IPS1;
	}
}

static inline void hri_tal_clear_SFLAG_IPS1_bit(const void *const hw, uint8_t index)
{
	((Tal *)hw)->SFLAGCLR[index].reg = TAL_SFLAG_IPS1;
}

static inline void hri_tal_set_SFLAG_IPS2_bit(const void *const hw, uint8_t index)
{
	((Tal *)hw)->SFLAGSET[index].reg = TAL_SFLAG_IPS2;
}

static inline bool hri_tal_get_SFLAG_IPS2_bit(const void *const hw, uint8_t index)
{
	return (((Tal *)hw)->SFLAG[index].reg & TAL_SFLAG_IPS2) >> TAL_SFLAG_IPS2_Pos;
}

static inline void hri_tal_write_SFLAG_IPS2_bit(const void *const hw, uint8_t index, bool value)
{
	if (value == 0x0) {
		((Tal *)hw)->SFLAGCLR[index].reg = TAL_SFLAG_IPS2;
	} else {
		((Tal *)hw)->SFLAGSET[index].reg = TAL_SFLAG_IPS2;
	}
}

static inline void hri_tal_clear_SFLAG_IPS2_bit(const void *const hw, uint8_t index)
{
	((Tal *)hw)->SFLAGCLR[index].reg = TAL_SFLAG_IPS2;
}

static inline void hri_tal_set_SFLAG_IPS3_bit(const void *const hw, uint8_t index)
{
	((Tal *)hw)->SFLAGSET[index].reg = TAL_SFLAG_IPS3;
}

static inline bool hri_tal_get_SFLAG_IPS3_bit(const void *const hw, uint8_t index)
{
	return (((Tal *)hw)->SFLAG[index].reg & TAL_SFLAG_IPS3) >> TAL_SFLAG_IPS3_Pos;
}

static inline void hri_tal_write_SFLAG_IPS3_bit(const void *const hw, uint8_t index, bool value)
{
	if (value == 0x0) {
		((Tal *)hw)->SFLAGCLR[index].reg = TAL_SFLAG_IPS3;
	} else {
		((Tal *)hw)->SFLAGSET[index].reg = TAL_SFLAG_IPS3;
	}
}

static inline void hri_tal_clear_SFLAG_IPS3_bit(const void *const hw, uint8_t index)
{
	((Tal *)hw)->SFLAGCLR[index].reg = TAL_SFLAG_IPS3;
}

static inline void hri_tal_set_SFLAG_IPS4_bit(const void *const hw, uint8_t index)
{
	((Tal *)hw)->SFLAGSET[index].reg = TAL_SFLAG_IPS4;
}

static inline bool hri_tal_get_SFLAG_IPS4_bit(const void *const hw, uint8_t index)
{
	return (((Tal *)hw)->SFLAG[index].reg & TAL_SFLAG_IPS4) >> TAL_SFLAG_IPS4_Pos;
}

static inline void hri_tal_write_SFLAG_IPS4_bit(const void *const hw, uint8_t index, bool value)
{
	if (value == 0x0) {
		((Tal *)hw)->SFLAGCLR[index].reg = TAL_SFLAG_IPS4;
	} else {
		((Tal *)hw)->SFLAGSET[index].reg = TAL_SFLAG_IPS4;
	}
}

static inline void hri_tal_clear_SFLAG_IPS4_bit(const void *const hw, uint8_t index)
{
	((Tal *)hw)->SFLAGCLR[index].reg = TAL_SFLAG_IPS4;
}

static inline void hri_tal_set_SFLAG_IPS5_bit(const void *const hw, uint8_t index)
{
	((Tal *)hw)->SFLAGSET[index].reg = TAL_SFLAG_IPS5;
}

static inline bool hri_tal_get_SFLAG_IPS5_bit(const void *const hw, uint8_t index)
{
	return (((Tal *)hw)->SFLAG[index].reg & TAL_SFLAG_IPS5) >> TAL_SFLAG_IPS5_Pos;
}

static inline void hri_tal_write_SFLAG_IPS5_bit(const void *const hw, uint8_t index, bool value)
{
	if (value == 0x0) {
		((Tal *)hw)->SFLAGCLR[index].reg = TAL_SFLAG_IPS5;
	} else {
		((Tal *)hw)->SFLAGSET[index].reg = TAL_SFLAG_IPS5;
	}
}

static inline void hri_tal_clear_SFLAG_IPS5_bit(const void *const hw, uint8_t index)
{
	((Tal *)hw)->SFLAGCLR[index].reg = TAL_SFLAG_IPS5;
}

static inline void hri_tal_set_SFLAG_IPS6_bit(const void *const hw, uint8_t index)
{
	((Tal *)hw)->SFLAGSET[index].reg = TAL_SFLAG_IPS6;
}

static inline bool hri_tal_get_SFLAG_IPS6_bit(const void *const hw, uint8_t index)
{
	return (((Tal *)hw)->SFLAG[index].reg & TAL_SFLAG_IPS6) >> TAL_SFLAG_IPS6_Pos;
}

static inline void hri_tal_write_SFLAG_IPS6_bit(const void *const hw, uint8_t index, bool value)
{
	if (value == 0x0) {
		((Tal *)hw)->SFLAGCLR[index].reg = TAL_SFLAG_IPS6;
	} else {
		((Tal *)hw)->SFLAGSET[index].reg = TAL_SFLAG_IPS6;
	}
}

static inline void hri_tal_clear_SFLAG_IPS6_bit(const void *const hw, uint8_t index)
{
	((Tal *)hw)->SFLAGCLR[index].reg = TAL_SFLAG_IPS6;
}

static inline void hri_tal_set_SFLAG_IPS7_bit(const void *const hw, uint8_t index)
{
	((Tal *)hw)->SFLAGSET[index].reg = TAL_SFLAG_IPS7;
}

static inline bool hri_tal_get_SFLAG_IPS7_bit(const void *const hw, uint8_t index)
{
	return (((Tal *)hw)->SFLAG[index].reg & TAL_SFLAG_IPS7) >> TAL_SFLAG_IPS7_Pos;
}

static inline void hri_tal_write_SFLAG_IPS7_bit(const void *const hw, uint8_t index, bool value)
{
	if (value == 0x0) {
		((Tal *)hw)->SFLAGCLR[index].reg = TAL_SFLAG_IPS7;
	} else {
		((Tal *)hw)->SFLAGSET[index].reg = TAL_SFLAG_IPS7;
	}
}

static inline void hri_tal_clear_SFLAG_IPS7_bit(const void *const hw, uint8_t index)
{
	((Tal *)hw)->SFLAGCLR[index].reg = TAL_SFLAG_IPS7;
}

static inline void hri_tal_set_SFLAG_IPS8_bit(const void *const hw, uint8_t index)
{
	((Tal *)hw)->SFLAGSET[index].reg = TAL_SFLAG_IPS8;
}

static inline bool hri_tal_get_SFLAG_IPS8_bit(const void *const hw, uint8_t index)
{
	return (((Tal *)hw)->SFLAG[index].reg & TAL_SFLAG_IPS8) >> TAL_SFLAG_IPS8_Pos;
}

static inline void hri_tal_write_SFLAG_IPS8_bit(const void *const hw, uint8_t index, bool value)
{
	if (value == 0x0) {
		((Tal *)hw)->SFLAGCLR[index].reg = TAL_SFLAG_IPS8;
	} else {
		((Tal *)hw)->SFLAGSET[index].reg = TAL_SFLAG_IPS8;
	}
}

static inline void hri_tal_clear_SFLAG_IPS8_bit(const void *const hw, uint8_t index)
{
	((Tal *)hw)->SFLAGCLR[index].reg = TAL_SFLAG_IPS8;
}

static inline void hri_tal_set_SFLAG_IPS9_bit(const void *const hw, uint8_t index)
{
	((Tal *)hw)->SFLAGSET[index].reg = TAL_SFLAG_IPS9;
}

static inline bool hri_tal_get_SFLAG_IPS9_bit(const void *const hw, uint8_t index)
{
	return (((Tal *)hw)->SFLAG[index].reg & TAL_SFLAG_IPS9) >> TAL_SFLAG_IPS9_Pos;
}

static inline void hri_tal_write_SFLAG_IPS9_bit(const void *const hw, uint8_t index, bool value)
{
	if (value == 0x0) {
		((Tal *)hw)->SFLAGCLR[index].reg = TAL_SFLAG_IPS9;
	} else {
		((Tal *)hw)->SFLAGSET[index].reg = TAL_SFLAG_IPS9;
	}
}

static inline void hri_tal_clear_SFLAG_IPS9_bit(const void *const hw, uint8_t index)
{
	((Tal *)hw)->SFLAGCLR[index].reg = TAL_SFLAG_IPS9;
}

static inline void hri_tal_set_SFLAG_IPS10_bit(const void *const hw, uint8_t index)
{
	((Tal *)hw)->SFLAGSET[index].reg = TAL_SFLAG_IPS10;
}

static inline bool hri_tal_get_SFLAG_IPS10_bit(const void *const hw, uint8_t index)
{
	return (((Tal *)hw)->SFLAG[index].reg & TAL_SFLAG_IPS10) >> TAL_SFLAG_IPS10_Pos;
}

static inline void hri_tal_write_SFLAG_IPS10_bit(const void *const hw, uint8_t index, bool value)
{
	if (value == 0x0) {
		((Tal *)hw)->SFLAGCLR[index].reg = TAL_SFLAG_IPS10;
	} else {
		((Tal *)hw)->SFLAGSET[index].reg = TAL_SFLAG_IPS10;
	}
}

static inline void hri_tal_clear_SFLAG_IPS10_bit(const void *const hw, uint8_t index)
{
	((Tal *)hw)->SFLAGCLR[index].reg = TAL_SFLAG_IPS10;
}

static inline void hri_tal_set_SFLAG_IPS11_bit(const void *const hw, uint8_t index)
{
	((Tal *)hw)->SFLAGSET[index].reg = TAL_SFLAG_IPS11;
}

static inline bool hri_tal_get_SFLAG_IPS11_bit(const void *const hw, uint8_t index)
{
	return (((Tal *)hw)->SFLAG[index].reg & TAL_SFLAG_IPS11) >> TAL_SFLAG_IPS11_Pos;
}

static inline void hri_tal_write_SFLAG_IPS11_bit(const void *const hw, uint8_t index, bool value)
{
	if (value == 0x0) {
		((Tal *)hw)->SFLAGCLR[index].reg = TAL_SFLAG_IPS11;
	} else {
		((Tal *)hw)->SFLAGSET[index].reg = TAL_SFLAG_IPS11;
	}
}

static inline void hri_tal_clear_SFLAG_IPS11_bit(const void *const hw, uint8_t index)
{
	((Tal *)hw)->SFLAGCLR[index].reg = TAL_SFLAG_IPS11;
}

static inline void hri_tal_set_SFLAG_IPS12_bit(const void *const hw, uint8_t index)
{
	((Tal *)hw)->SFLAGSET[index].reg = TAL_SFLAG_IPS12;
}

static inline bool hri_tal_get_SFLAG_IPS12_bit(const void *const hw, uint8_t index)
{
	return (((Tal *)hw)->SFLAG[index].reg & TAL_SFLAG_IPS12) >> TAL_SFLAG_IPS12_Pos;
}

static inline void hri_tal_write_SFLAG_IPS12_bit(const void *const hw, uint8_t index, bool value)
{
	if (value == 0x0) {
		((Tal *)hw)->SFLAGCLR[index].reg = TAL_SFLAG_IPS12;
	} else {
		((Tal *)hw)->SFLAGSET[index].reg = TAL_SFLAG_IPS12;
	}
}

static inline void hri_tal_clear_SFLAG_IPS12_bit(const void *const hw, uint8_t index)
{
	((Tal *)hw)->SFLAGCLR[index].reg = TAL_SFLAG_IPS12;
}

static inline void hri_tal_set_SFLAG_IPS13_bit(const void *const hw, uint8_t index)
{
	((Tal *)hw)->SFLAGSET[index].reg = TAL_SFLAG_IPS13;
}

static inline bool hri_tal_get_SFLAG_IPS13_bit(const void *const hw, uint8_t index)
{
	return (((Tal *)hw)->SFLAG[index].reg & TAL_SFLAG_IPS13) >> TAL_SFLAG_IPS13_Pos;
}

static inline void hri_tal_write_SFLAG_IPS13_bit(const void *const hw, uint8_t index, bool value)
{
	if (value == 0x0) {
		((Tal *)hw)->SFLAGCLR[index].reg = TAL_SFLAG_IPS13;
	} else {
		((Tal *)hw)->SFLAGSET[index].reg = TAL_SFLAG_IPS13;
	}
}

static inline void hri_tal_clear_SFLAG_IPS13_bit(const void *const hw, uint8_t index)
{
	((Tal *)hw)->SFLAGCLR[index].reg = TAL_SFLAG_IPS13;
}

static inline void hri_tal_set_SFLAG_IPS14_bit(const void *const hw, uint8_t index)
{
	((Tal *)hw)->SFLAGSET[index].reg = TAL_SFLAG_IPS14;
}

static inline bool hri_tal_get_SFLAG_IPS14_bit(const void *const hw, uint8_t index)
{
	return (((Tal *)hw)->SFLAG[index].reg & TAL_SFLAG_IPS14) >> TAL_SFLAG_IPS14_Pos;
}

static inline void hri_tal_write_SFLAG_IPS14_bit(const void *const hw, uint8_t index, bool value)
{
	if (value == 0x0) {
		((Tal *)hw)->SFLAGCLR[index].reg = TAL_SFLAG_IPS14;
	} else {
		((Tal *)hw)->SFLAGSET[index].reg = TAL_SFLAG_IPS14;
	}
}

static inline void hri_tal_clear_SFLAG_IPS14_bit(const void *const hw, uint8_t index)
{
	((Tal *)hw)->SFLAGCLR[index].reg = TAL_SFLAG_IPS14;
}

static inline void hri_tal_set_SFLAG_IPS15_bit(const void *const hw, uint8_t index)
{
	((Tal *)hw)->SFLAGSET[index].reg = TAL_SFLAG_IPS15;
}

static inline bool hri_tal_get_SFLAG_IPS15_bit(const void *const hw, uint8_t index)
{
	return (((Tal *)hw)->SFLAG[index].reg & TAL_SFLAG_IPS15) >> TAL_SFLAG_IPS15_Pos;
}

static inline void hri_tal_write_SFLAG_IPS15_bit(const void *const hw, uint8_t index, bool value)
{
	if (value == 0x0) {
		((Tal *)hw)->SFLAGCLR[index].reg = TAL_SFLAG_IPS15;
	} else {
		((Tal *)hw)->SFLAGSET[index].reg = TAL_SFLAG_IPS15;
	}
}

static inline void hri_tal_clear_SFLAG_IPS15_bit(const void *const hw, uint8_t index)
{
	((Tal *)hw)->SFLAGCLR[index].reg = TAL_SFLAG_IPS15;
}

static inline void hri_tal_set_SFLAG_IPS16_bit(const void *const hw, uint8_t index)
{
	((Tal *)hw)->SFLAGSET[index].reg = TAL_SFLAG_IPS16;
}

static inline bool hri_tal_get_SFLAG_IPS16_bit(const void *const hw, uint8_t index)
{
	return (((Tal *)hw)->SFLAG[index].reg & TAL_SFLAG_IPS16) >> TAL_SFLAG_IPS16_Pos;
}

static inline void hri_tal_write_SFLAG_IPS16_bit(const void *const hw, uint8_t index, bool value)
{
	if (value == 0x0) {
		((Tal *)hw)->SFLAGCLR[index].reg = TAL_SFLAG_IPS16;
	} else {
		((Tal *)hw)->SFLAGSET[index].reg = TAL_SFLAG_IPS16;
	}
}

static inline void hri_tal_clear_SFLAG_IPS16_bit(const void *const hw, uint8_t index)
{
	((Tal *)hw)->SFLAGCLR[index].reg = TAL_SFLAG_IPS16;
}

static inline void hri_tal_set_SFLAG_IPS17_bit(const void *const hw, uint8_t index)
{
	((Tal *)hw)->SFLAGSET[index].reg = TAL_SFLAG_IPS17;
}

static inline bool hri_tal_get_SFLAG_IPS17_bit(const void *const hw, uint8_t index)
{
	return (((Tal *)hw)->SFLAG[index].reg & TAL_SFLAG_IPS17) >> TAL_SFLAG_IPS17_Pos;
}

static inline void hri_tal_write_SFLAG_IPS17_bit(const void *const hw, uint8_t index, bool value)
{
	if (value == 0x0) {
		((Tal *)hw)->SFLAGCLR[index].reg = TAL_SFLAG_IPS17;
	} else {
		((Tal *)hw)->SFLAGSET[index].reg = TAL_SFLAG_IPS17;
	}
}

static inline void hri_tal_clear_SFLAG_IPS17_bit(const void *const hw, uint8_t index)
{
	((Tal *)hw)->SFLAGCLR[index].reg = TAL_SFLAG_IPS17;
}

static inline void hri_tal_set_SFLAG_IPS18_bit(const void *const hw, uint8_t index)
{
	((Tal *)hw)->SFLAGSET[index].reg = TAL_SFLAG_IPS18;
}

static inline bool hri_tal_get_SFLAG_IPS18_bit(const void *const hw, uint8_t index)
{
	return (((Tal *)hw)->SFLAG[index].reg & TAL_SFLAG_IPS18) >> TAL_SFLAG_IPS18_Pos;
}

static inline void hri_tal_write_SFLAG_IPS18_bit(const void *const hw, uint8_t index, bool value)
{
	if (value == 0x0) {
		((Tal *)hw)->SFLAGCLR[index].reg = TAL_SFLAG_IPS18;
	} else {
		((Tal *)hw)->SFLAGSET[index].reg = TAL_SFLAG_IPS18;
	}
}

static inline void hri_tal_clear_SFLAG_IPS18_bit(const void *const hw, uint8_t index)
{
	((Tal *)hw)->SFLAGCLR[index].reg = TAL_SFLAG_IPS18;
}

static inline void hri_tal_set_SFLAG_IPS19_bit(const void *const hw, uint8_t index)
{
	((Tal *)hw)->SFLAGSET[index].reg = TAL_SFLAG_IPS19;
}

static inline bool hri_tal_get_SFLAG_IPS19_bit(const void *const hw, uint8_t index)
{
	return (((Tal *)hw)->SFLAG[index].reg & TAL_SFLAG_IPS19) >> TAL_SFLAG_IPS19_Pos;
}

static inline void hri_tal_write_SFLAG_IPS19_bit(const void *const hw, uint8_t index, bool value)
{
	if (value == 0x0) {
		((Tal *)hw)->SFLAGCLR[index].reg = TAL_SFLAG_IPS19;
	} else {
		((Tal *)hw)->SFLAGSET[index].reg = TAL_SFLAG_IPS19;
	}
}

static inline void hri_tal_clear_SFLAG_IPS19_bit(const void *const hw, uint8_t index)
{
	((Tal *)hw)->SFLAGCLR[index].reg = TAL_SFLAG_IPS19;
}

static inline void hri_tal_set_SFLAG_IPS20_bit(const void *const hw, uint8_t index)
{
	((Tal *)hw)->SFLAGSET[index].reg = TAL_SFLAG_IPS20;
}

static inline bool hri_tal_get_SFLAG_IPS20_bit(const void *const hw, uint8_t index)
{
	return (((Tal *)hw)->SFLAG[index].reg & TAL_SFLAG_IPS20) >> TAL_SFLAG_IPS20_Pos;
}

static inline void hri_tal_write_SFLAG_IPS20_bit(const void *const hw, uint8_t index, bool value)
{
	if (value == 0x0) {
		((Tal *)hw)->SFLAGCLR[index].reg = TAL_SFLAG_IPS20;
	} else {
		((Tal *)hw)->SFLAGSET[index].reg = TAL_SFLAG_IPS20;
	}
}

static inline void hri_tal_clear_SFLAG_IPS20_bit(const void *const hw, uint8_t index)
{
	((Tal *)hw)->SFLAGCLR[index].reg = TAL_SFLAG_IPS20;
}

static inline void hri_tal_set_SFLAG_IPS21_bit(const void *const hw, uint8_t index)
{
	((Tal *)hw)->SFLAGSET[index].reg = TAL_SFLAG_IPS21;
}

static inline bool hri_tal_get_SFLAG_IPS21_bit(const void *const hw, uint8_t index)
{
	return (((Tal *)hw)->SFLAG[index].reg & TAL_SFLAG_IPS21) >> TAL_SFLAG_IPS21_Pos;
}

static inline void hri_tal_write_SFLAG_IPS21_bit(const void *const hw, uint8_t index, bool value)
{
	if (value == 0x0) {
		((Tal *)hw)->SFLAGCLR[index].reg = TAL_SFLAG_IPS21;
	} else {
		((Tal *)hw)->SFLAGSET[index].reg = TAL_SFLAG_IPS21;
	}
}

static inline void hri_tal_clear_SFLAG_IPS21_bit(const void *const hw, uint8_t index)
{
	((Tal *)hw)->SFLAGCLR[index].reg = TAL_SFLAG_IPS21;
}

static inline void hri_tal_set_SFLAG_IPS22_bit(const void *const hw, uint8_t index)
{
	((Tal *)hw)->SFLAGSET[index].reg = TAL_SFLAG_IPS22;
}

static inline bool hri_tal_get_SFLAG_IPS22_bit(const void *const hw, uint8_t index)
{
	return (((Tal *)hw)->SFLAG[index].reg & TAL_SFLAG_IPS22) >> TAL_SFLAG_IPS22_Pos;
}

static inline void hri_tal_write_SFLAG_IPS22_bit(const void *const hw, uint8_t index, bool value)
{
	if (value == 0x0) {
		((Tal *)hw)->SFLAGCLR[index].reg = TAL_SFLAG_IPS22;
	} else {
		((Tal *)hw)->SFLAGSET[index].reg = TAL_SFLAG_IPS22;
	}
}

static inline void hri_tal_clear_SFLAG_IPS22_bit(const void *const hw, uint8_t index)
{
	((Tal *)hw)->SFLAGCLR[index].reg = TAL_SFLAG_IPS22;
}

static inline void hri_tal_set_SFLAG_IPS23_bit(const void *const hw, uint8_t index)
{
	((Tal *)hw)->SFLAGSET[index].reg = TAL_SFLAG_IPS23;
}

static inline bool hri_tal_get_SFLAG_IPS23_bit(const void *const hw, uint8_t index)
{
	return (((Tal *)hw)->SFLAG[index].reg & TAL_SFLAG_IPS23) >> TAL_SFLAG_IPS23_Pos;
}

static inline void hri_tal_write_SFLAG_IPS23_bit(const void *const hw, uint8_t index, bool value)
{
	if (value == 0x0) {
		((Tal *)hw)->SFLAGCLR[index].reg = TAL_SFLAG_IPS23;
	} else {
		((Tal *)hw)->SFLAGSET[index].reg = TAL_SFLAG_IPS23;
	}
}

static inline void hri_tal_clear_SFLAG_IPS23_bit(const void *const hw, uint8_t index)
{
	((Tal *)hw)->SFLAGCLR[index].reg = TAL_SFLAG_IPS23;
}

static inline void hri_tal_set_SFLAG_IPS24_bit(const void *const hw, uint8_t index)
{
	((Tal *)hw)->SFLAGSET[index].reg = TAL_SFLAG_IPS24;
}

static inline bool hri_tal_get_SFLAG_IPS24_bit(const void *const hw, uint8_t index)
{
	return (((Tal *)hw)->SFLAG[index].reg & TAL_SFLAG_IPS24) >> TAL_SFLAG_IPS24_Pos;
}

static inline void hri_tal_write_SFLAG_IPS24_bit(const void *const hw, uint8_t index, bool value)
{
	if (value == 0x0) {
		((Tal *)hw)->SFLAGCLR[index].reg = TAL_SFLAG_IPS24;
	} else {
		((Tal *)hw)->SFLAGSET[index].reg = TAL_SFLAG_IPS24;
	}
}

static inline void hri_tal_clear_SFLAG_IPS24_bit(const void *const hw, uint8_t index)
{
	((Tal *)hw)->SFLAGCLR[index].reg = TAL_SFLAG_IPS24;
}

static inline void hri_tal_set_SFLAG_IPS25_bit(const void *const hw, uint8_t index)
{
	((Tal *)hw)->SFLAGSET[index].reg = TAL_SFLAG_IPS25;
}

static inline bool hri_tal_get_SFLAG_IPS25_bit(const void *const hw, uint8_t index)
{
	return (((Tal *)hw)->SFLAG[index].reg & TAL_SFLAG_IPS25) >> TAL_SFLAG_IPS25_Pos;
}

static inline void hri_tal_write_SFLAG_IPS25_bit(const void *const hw, uint8_t index, bool value)
{
	if (value == 0x0) {
		((Tal *)hw)->SFLAGCLR[index].reg = TAL_SFLAG_IPS25;
	} else {
		((Tal *)hw)->SFLAGSET[index].reg = TAL_SFLAG_IPS25;
	}
}

static inline void hri_tal_clear_SFLAG_IPS25_bit(const void *const hw, uint8_t index)
{
	((Tal *)hw)->SFLAGCLR[index].reg = TAL_SFLAG_IPS25;
}

static inline void hri_tal_set_SFLAG_IPS26_bit(const void *const hw, uint8_t index)
{
	((Tal *)hw)->SFLAGSET[index].reg = TAL_SFLAG_IPS26;
}

static inline bool hri_tal_get_SFLAG_IPS26_bit(const void *const hw, uint8_t index)
{
	return (((Tal *)hw)->SFLAG[index].reg & TAL_SFLAG_IPS26) >> TAL_SFLAG_IPS26_Pos;
}

static inline void hri_tal_write_SFLAG_IPS26_bit(const void *const hw, uint8_t index, bool value)
{
	if (value == 0x0) {
		((Tal *)hw)->SFLAGCLR[index].reg = TAL_SFLAG_IPS26;
	} else {
		((Tal *)hw)->SFLAGSET[index].reg = TAL_SFLAG_IPS26;
	}
}

static inline void hri_tal_clear_SFLAG_IPS26_bit(const void *const hw, uint8_t index)
{
	((Tal *)hw)->SFLAGCLR[index].reg = TAL_SFLAG_IPS26;
}

static inline void hri_tal_set_SFLAG_IPS27_bit(const void *const hw, uint8_t index)
{
	((Tal *)hw)->SFLAGSET[index].reg = TAL_SFLAG_IPS27;
}

static inline bool hri_tal_get_SFLAG_IPS27_bit(const void *const hw, uint8_t index)
{
	return (((Tal *)hw)->SFLAG[index].reg & TAL_SFLAG_IPS27) >> TAL_SFLAG_IPS27_Pos;
}

static inline void hri_tal_write_SFLAG_IPS27_bit(const void *const hw, uint8_t index, bool value)
{
	if (value == 0x0) {
		((Tal *)hw)->SFLAGCLR[index].reg = TAL_SFLAG_IPS27;
	} else {
		((Tal *)hw)->SFLAGSET[index].reg = TAL_SFLAG_IPS27;
	}
}

static inline void hri_tal_clear_SFLAG_IPS27_bit(const void *const hw, uint8_t index)
{
	((Tal *)hw)->SFLAGCLR[index].reg = TAL_SFLAG_IPS27;
}

static inline void hri_tal_set_SFLAG_IPS28_bit(const void *const hw, uint8_t index)
{
	((Tal *)hw)->SFLAGSET[index].reg = TAL_SFLAG_IPS28;
}

static inline bool hri_tal_get_SFLAG_IPS28_bit(const void *const hw, uint8_t index)
{
	return (((Tal *)hw)->SFLAG[index].reg & TAL_SFLAG_IPS28) >> TAL_SFLAG_IPS28_Pos;
}

static inline void hri_tal_write_SFLAG_IPS28_bit(const void *const hw, uint8_t index, bool value)
{
	if (value == 0x0) {
		((Tal *)hw)->SFLAGCLR[index].reg = TAL_SFLAG_IPS28;
	} else {
		((Tal *)hw)->SFLAGSET[index].reg = TAL_SFLAG_IPS28;
	}
}

static inline void hri_tal_clear_SFLAG_IPS28_bit(const void *const hw, uint8_t index)
{
	((Tal *)hw)->SFLAGCLR[index].reg = TAL_SFLAG_IPS28;
}

static inline void hri_tal_set_SFLAG_IPS29_bit(const void *const hw, uint8_t index)
{
	((Tal *)hw)->SFLAGSET[index].reg = TAL_SFLAG_IPS29;
}

static inline bool hri_tal_get_SFLAG_IPS29_bit(const void *const hw, uint8_t index)
{
	return (((Tal *)hw)->SFLAG[index].reg & TAL_SFLAG_IPS29) >> TAL_SFLAG_IPS29_Pos;
}

static inline void hri_tal_write_SFLAG_IPS29_bit(const void *const hw, uint8_t index, bool value)
{
	if (value == 0x0) {
		((Tal *)hw)->SFLAGCLR[index].reg = TAL_SFLAG_IPS29;
	} else {
		((Tal *)hw)->SFLAGSET[index].reg = TAL_SFLAG_IPS29;
	}
}

static inline void hri_tal_clear_SFLAG_IPS29_bit(const void *const hw, uint8_t index)
{
	((Tal *)hw)->SFLAGCLR[index].reg = TAL_SFLAG_IPS29;
}

static inline void hri_tal_set_SFLAG_IPS30_bit(const void *const hw, uint8_t index)
{
	((Tal *)hw)->SFLAGSET[index].reg = TAL_SFLAG_IPS30;
}

static inline bool hri_tal_get_SFLAG_IPS30_bit(const void *const hw, uint8_t index)
{
	return (((Tal *)hw)->SFLAG[index].reg & TAL_SFLAG_IPS30) >> TAL_SFLAG_IPS30_Pos;
}

static inline void hri_tal_write_SFLAG_IPS30_bit(const void *const hw, uint8_t index, bool value)
{
	if (value == 0x0) {
		((Tal *)hw)->SFLAGCLR[index].reg = TAL_SFLAG_IPS30;
	} else {
		((Tal *)hw)->SFLAGSET[index].reg = TAL_SFLAG_IPS30;
	}
}

static inline void hri_tal_clear_SFLAG_IPS30_bit(const void *const hw, uint8_t index)
{
	((Tal *)hw)->SFLAGCLR[index].reg = TAL_SFLAG_IPS30;
}

static inline void hri_tal_set_SFLAG_IPS31_bit(const void *const hw, uint8_t index)
{
	((Tal *)hw)->SFLAGSET[index].reg = TAL_SFLAG_IPS31;
}

static inline bool hri_tal_get_SFLAG_IPS31_bit(const void *const hw, uint8_t index)
{
	return (((Tal *)hw)->SFLAG[index].reg & TAL_SFLAG_IPS31) >> TAL_SFLAG_IPS31_Pos;
}

static inline void hri_tal_write_SFLAG_IPS31_bit(const void *const hw, uint8_t index, bool value)
{
	if (value == 0x0) {
		((Tal *)hw)->SFLAGCLR[index].reg = TAL_SFLAG_IPS31;
	} else {
		((Tal *)hw)->SFLAGSET[index].reg = TAL_SFLAG_IPS31;
	}
}

static inline void hri_tal_clear_SFLAG_IPS31_bit(const void *const hw, uint8_t index)
{
	((Tal *)hw)->SFLAGCLR[index].reg = TAL_SFLAG_IPS31;
}

static inline void hri_tal_set_SFLAG_reg(const void *const hw, uint8_t index, hri_tal_sflag_reg_t mask)
{
	((Tal *)hw)->SFLAGSET[index].reg = mask;
}

static inline hri_tal_sflag_reg_t hri_tal_get_SFLAG_reg(const void *const hw, uint8_t index, hri_tal_sflag_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->SFLAG[index].reg;
	tmp &= mask;
	return tmp;
}

static inline hri_tal_sflag_reg_t hri_tal_read_SFLAG_reg(const void *const hw, uint8_t index)
{
	return ((Tal *)hw)->SFLAG[index].reg;
}

static inline void hri_tal_write_SFLAG_reg(const void *const hw, uint8_t index, hri_tal_sflag_reg_t data)
{
	((Tal *)hw)->SFLAGSET[index].reg = data;
	((Tal *)hw)->SFLAGCLR[index].reg = ~data;
}

static inline void hri_tal_clear_SFLAG_reg(const void *const hw, uint8_t index, hri_tal_sflag_reg_t mask)
{
	((Tal *)hw)->SFLAGCLR[index].reg = mask;
}

static inline bool hri_tal_get_INTFLAG_BRK_bit(const void *const hw)
{
	return (((Tal *)hw)->INTFLAG.reg & TAL_INTFLAG_BRK) >> TAL_INTFLAG_BRK_Pos;
}

static inline void hri_tal_clear_INTFLAG_BRK_bit(const void *const hw)
{
	((Tal *)hw)->INTFLAG.reg = TAL_INTFLAG_BRK;
}

static inline bool hri_tal_get_INTFLAG_IPS0_bit(const void *const hw)
{
	return (((Tal *)hw)->INTFLAG.reg & TAL_INTFLAG_IPS0) >> TAL_INTFLAG_IPS0_Pos;
}

static inline void hri_tal_clear_INTFLAG_IPS0_bit(const void *const hw)
{
	((Tal *)hw)->INTFLAG.reg = TAL_INTFLAG_IPS0;
}

static inline bool hri_tal_get_INTFLAG_IPS1_bit(const void *const hw)
{
	return (((Tal *)hw)->INTFLAG.reg & TAL_INTFLAG_IPS1) >> TAL_INTFLAG_IPS1_Pos;
}

static inline void hri_tal_clear_INTFLAG_IPS1_bit(const void *const hw)
{
	((Tal *)hw)->INTFLAG.reg = TAL_INTFLAG_IPS1;
}

static inline bool hri_tal_get_interrupt_BRK_bit(const void *const hw)
{
	return (((Tal *)hw)->INTFLAG.reg & TAL_INTFLAG_BRK) >> TAL_INTFLAG_BRK_Pos;
}

static inline void hri_tal_clear_interrupt_BRK_bit(const void *const hw)
{
	((Tal *)hw)->INTFLAG.reg = TAL_INTFLAG_BRK;
}

static inline bool hri_tal_get_interrupt_IPS0_bit(const void *const hw)
{
	return (((Tal *)hw)->INTFLAG.reg & TAL_INTFLAG_IPS0) >> TAL_INTFLAG_IPS0_Pos;
}

static inline void hri_tal_clear_interrupt_IPS0_bit(const void *const hw)
{
	((Tal *)hw)->INTFLAG.reg = TAL_INTFLAG_IPS0;
}

static inline bool hri_tal_get_interrupt_IPS1_bit(const void *const hw)
{
	return (((Tal *)hw)->INTFLAG.reg & TAL_INTFLAG_IPS1) >> TAL_INTFLAG_IPS1_Pos;
}

static inline void hri_tal_clear_interrupt_IPS1_bit(const void *const hw)
{
	((Tal *)hw)->INTFLAG.reg = TAL_INTFLAG_IPS1;
}

static inline hri_tal_intflag_reg_t hri_tal_get_INTFLAG_reg(const void *const hw, hri_tal_intflag_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Tal *)hw)->INTFLAG.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_tal_intflag_reg_t hri_tal_read_INTFLAG_reg(const void *const hw)
{
	return ((Tal *)hw)->INTFLAG.reg;
}

static inline void hri_tal_clear_INTFLAG_reg(const void *const hw, hri_tal_intflag_reg_t mask)
{
	((Tal *)hw)->INTFLAG.reg = mask;
}

static inline void hri_tal_write_HALT_reg(const void *const hw, hri_tal_halt_reg_t data)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->HALT.reg = data;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_write_RESTART_reg(const void *const hw, hri_tal_restart_reg_t data)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->RESTART.reg = data;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_CTRLA_SWRST_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->CTRLA.reg |= TAL_CTRLA_SWRST;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_CTRLA_SWRST_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Tal *)hw)->CTRLA.reg;
	tmp = (tmp & TAL_CTRLA_SWRST) >> TAL_CTRLA_SWRST_Pos;
	return (bool)tmp;
}

static inline void hri_tal_set_CTRLA_ENABLE_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->CTRLA.reg |= TAL_CTRLA_ENABLE;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_CTRLA_ENABLE_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Tal *)hw)->CTRLA.reg;
	tmp = (tmp & TAL_CTRLA_ENABLE) >> TAL_CTRLA_ENABLE_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_CTRLA_ENABLE_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->CTRLA.reg;
	tmp &= ~TAL_CTRLA_ENABLE;
	tmp |= value << TAL_CTRLA_ENABLE_Pos;
	((Tal *)hw)->CTRLA.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_CTRLA_ENABLE_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->CTRLA.reg &= ~TAL_CTRLA_ENABLE;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_CTRLA_ENABLE_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->CTRLA.reg ^= TAL_CTRLA_ENABLE;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_CTRLA_reg(const void *const hw, hri_tal_ctrla_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->CTRLA.reg |= mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline hri_tal_ctrla_reg_t hri_tal_get_CTRLA_reg(const void *const hw, hri_tal_ctrla_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Tal *)hw)->CTRLA.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_tal_write_CTRLA_reg(const void *const hw, hri_tal_ctrla_reg_t data)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->CTRLA.reg = data;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_CTRLA_reg(const void *const hw, hri_tal_ctrla_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->CTRLA.reg &= ~mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_CTRLA_reg(const void *const hw, hri_tal_ctrla_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->CTRLA.reg ^= mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline hri_tal_ctrla_reg_t hri_tal_read_CTRLA_reg(const void *const hw)
{
	return ((Tal *)hw)->CTRLA.reg;
}

static inline void hri_tal_set_EXTCTRL_ENABLE_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EXTCTRL.reg |= TAL_EXTCTRL_ENABLE;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_EXTCTRL_ENABLE_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Tal *)hw)->EXTCTRL.reg;
	tmp = (tmp & TAL_EXTCTRL_ENABLE) >> TAL_EXTCTRL_ENABLE_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_EXTCTRL_ENABLE_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->EXTCTRL.reg;
	tmp &= ~TAL_EXTCTRL_ENABLE;
	tmp |= value << TAL_EXTCTRL_ENABLE_Pos;
	((Tal *)hw)->EXTCTRL.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_EXTCTRL_ENABLE_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EXTCTRL.reg &= ~TAL_EXTCTRL_ENABLE;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_EXTCTRL_ENABLE_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EXTCTRL.reg ^= TAL_EXTCTRL_ENABLE;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_EXTCTRL_INV_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EXTCTRL.reg |= TAL_EXTCTRL_INV;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_EXTCTRL_INV_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Tal *)hw)->EXTCTRL.reg;
	tmp = (tmp & TAL_EXTCTRL_INV) >> TAL_EXTCTRL_INV_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_EXTCTRL_INV_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->EXTCTRL.reg;
	tmp &= ~TAL_EXTCTRL_INV;
	tmp |= value << TAL_EXTCTRL_INV_Pos;
	((Tal *)hw)->EXTCTRL.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_EXTCTRL_INV_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EXTCTRL.reg &= ~TAL_EXTCTRL_INV;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_EXTCTRL_INV_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EXTCTRL.reg ^= TAL_EXTCTRL_INV;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_EXTCTRL_reg(const void *const hw, hri_tal_extctrl_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EXTCTRL.reg |= mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline hri_tal_extctrl_reg_t hri_tal_get_EXTCTRL_reg(const void *const hw, hri_tal_extctrl_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Tal *)hw)->EXTCTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_tal_write_EXTCTRL_reg(const void *const hw, hri_tal_extctrl_reg_t data)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EXTCTRL.reg = data;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_EXTCTRL_reg(const void *const hw, hri_tal_extctrl_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EXTCTRL.reg &= ~mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_EXTCTRL_reg(const void *const hw, hri_tal_extctrl_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EXTCTRL.reg ^= mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline hri_tal_extctrl_reg_t hri_tal_read_EXTCTRL_reg(const void *const hw)
{
	return ((Tal *)hw)->EXTCTRL.reg;
}

static inline void hri_tal_set_EVCTRL_BRKEI_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EVCTRL.reg |= TAL_EVCTRL_BRKEI;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_EVCTRL_BRKEI_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Tal *)hw)->EVCTRL.reg;
	tmp = (tmp & TAL_EVCTRL_BRKEI) >> TAL_EVCTRL_BRKEI_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_EVCTRL_BRKEI_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->EVCTRL.reg;
	tmp &= ~TAL_EVCTRL_BRKEI;
	tmp |= value << TAL_EVCTRL_BRKEI_Pos;
	((Tal *)hw)->EVCTRL.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_EVCTRL_BRKEI_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EVCTRL.reg &= ~TAL_EVCTRL_BRKEI;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_EVCTRL_BRKEI_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EVCTRL.reg ^= TAL_EVCTRL_BRKEI;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_EVCTRL_BRKEO_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EVCTRL.reg |= TAL_EVCTRL_BRKEO;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_EVCTRL_BRKEO_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Tal *)hw)->EVCTRL.reg;
	tmp = (tmp & TAL_EVCTRL_BRKEO) >> TAL_EVCTRL_BRKEO_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_EVCTRL_BRKEO_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->EVCTRL.reg;
	tmp &= ~TAL_EVCTRL_BRKEO;
	tmp |= value << TAL_EVCTRL_BRKEO_Pos;
	((Tal *)hw)->EVCTRL.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_EVCTRL_BRKEO_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EVCTRL.reg &= ~TAL_EVCTRL_BRKEO;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_EVCTRL_BRKEO_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EVCTRL.reg ^= TAL_EVCTRL_BRKEO;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_EVCTRL_IRQMONEO0_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EVCTRL.reg |= TAL_EVCTRL_IRQMONEO0;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_EVCTRL_IRQMONEO0_bit(const void *const hw)
{
	uint16_t tmp;
	tmp = ((Tal *)hw)->EVCTRL.reg;
	tmp = (tmp & TAL_EVCTRL_IRQMONEO0) >> TAL_EVCTRL_IRQMONEO0_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_EVCTRL_IRQMONEO0_bit(const void *const hw, bool value)
{
	uint16_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->EVCTRL.reg;
	tmp &= ~TAL_EVCTRL_IRQMONEO0;
	tmp |= value << TAL_EVCTRL_IRQMONEO0_Pos;
	((Tal *)hw)->EVCTRL.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_EVCTRL_IRQMONEO0_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EVCTRL.reg &= ~TAL_EVCTRL_IRQMONEO0;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_EVCTRL_IRQMONEO0_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EVCTRL.reg ^= TAL_EVCTRL_IRQMONEO0;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_EVCTRL_reg(const void *const hw, hri_tal_evctrl_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EVCTRL.reg |= mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline hri_tal_evctrl_reg_t hri_tal_get_EVCTRL_reg(const void *const hw, hri_tal_evctrl_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Tal *)hw)->EVCTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_tal_write_EVCTRL_reg(const void *const hw, hri_tal_evctrl_reg_t data)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EVCTRL.reg = data;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_EVCTRL_reg(const void *const hw, hri_tal_evctrl_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EVCTRL.reg &= ~mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_EVCTRL_reg(const void *const hw, hri_tal_evctrl_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EVCTRL.reg ^= mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline hri_tal_evctrl_reg_t hri_tal_read_EVCTRL_reg(const void *const hw)
{
	return ((Tal *)hw)->EVCTRL.reg;
}

static inline void hri_tal_set_GLOBMASK_CPU0_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->GLOBMASK.reg |= TAL_GLOBMASK_CPU0;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_GLOBMASK_CPU0_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Tal *)hw)->GLOBMASK.reg;
	tmp = (tmp & TAL_GLOBMASK_CPU0) >> TAL_GLOBMASK_CPU0_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_GLOBMASK_CPU0_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->GLOBMASK.reg;
	tmp &= ~TAL_GLOBMASK_CPU0;
	tmp |= value << TAL_GLOBMASK_CPU0_Pos;
	((Tal *)hw)->GLOBMASK.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_GLOBMASK_CPU0_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->GLOBMASK.reg &= ~TAL_GLOBMASK_CPU0;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_GLOBMASK_CPU0_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->GLOBMASK.reg ^= TAL_GLOBMASK_CPU0;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_GLOBMASK_CPU1_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->GLOBMASK.reg |= TAL_GLOBMASK_CPU1;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_GLOBMASK_CPU1_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Tal *)hw)->GLOBMASK.reg;
	tmp = (tmp & TAL_GLOBMASK_CPU1) >> TAL_GLOBMASK_CPU1_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_GLOBMASK_CPU1_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->GLOBMASK.reg;
	tmp &= ~TAL_GLOBMASK_CPU1;
	tmp |= value << TAL_GLOBMASK_CPU1_Pos;
	((Tal *)hw)->GLOBMASK.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_GLOBMASK_CPU1_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->GLOBMASK.reg &= ~TAL_GLOBMASK_CPU1;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_GLOBMASK_CPU1_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->GLOBMASK.reg ^= TAL_GLOBMASK_CPU1;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_GLOBMASK_EVBRK_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->GLOBMASK.reg |= TAL_GLOBMASK_EVBRK;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_GLOBMASK_EVBRK_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Tal *)hw)->GLOBMASK.reg;
	tmp = (tmp & TAL_GLOBMASK_EVBRK) >> TAL_GLOBMASK_EVBRK_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_GLOBMASK_EVBRK_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->GLOBMASK.reg;
	tmp &= ~TAL_GLOBMASK_EVBRK;
	tmp |= value << TAL_GLOBMASK_EVBRK_Pos;
	((Tal *)hw)->GLOBMASK.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_GLOBMASK_EVBRK_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->GLOBMASK.reg &= ~TAL_GLOBMASK_EVBRK;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_GLOBMASK_EVBRK_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->GLOBMASK.reg ^= TAL_GLOBMASK_EVBRK;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_GLOBMASK_EXTBRK_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->GLOBMASK.reg |= TAL_GLOBMASK_EXTBRK;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_GLOBMASK_EXTBRK_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Tal *)hw)->GLOBMASK.reg;
	tmp = (tmp & TAL_GLOBMASK_EXTBRK) >> TAL_GLOBMASK_EXTBRK_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_GLOBMASK_EXTBRK_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->GLOBMASK.reg;
	tmp &= ~TAL_GLOBMASK_EXTBRK;
	tmp |= value << TAL_GLOBMASK_EXTBRK_Pos;
	((Tal *)hw)->GLOBMASK.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_GLOBMASK_EXTBRK_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->GLOBMASK.reg &= ~TAL_GLOBMASK_EXTBRK;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_GLOBMASK_EXTBRK_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->GLOBMASK.reg ^= TAL_GLOBMASK_EXTBRK;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_GLOBMASK_reg(const void *const hw, hri_tal_globmask_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->GLOBMASK.reg |= mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline hri_tal_globmask_reg_t hri_tal_get_GLOBMASK_reg(const void *const hw, hri_tal_globmask_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Tal *)hw)->GLOBMASK.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_tal_write_GLOBMASK_reg(const void *const hw, hri_tal_globmask_reg_t data)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->GLOBMASK.reg = data;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_GLOBMASK_reg(const void *const hw, hri_tal_globmask_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->GLOBMASK.reg &= ~mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_GLOBMASK_reg(const void *const hw, hri_tal_globmask_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->GLOBMASK.reg ^= mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline hri_tal_globmask_reg_t hri_tal_read_GLOBMASK_reg(const void *const hw)
{
	return ((Tal *)hw)->GLOBMASK.reg;
}

static inline void hri_tal_set_DMACPUSEL0_CH0_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL0.reg |= TAL_DMACPUSEL0_CH0_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_DMACPUSEL0_CH0_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->DMACPUSEL0.reg;
	tmp = (tmp & TAL_DMACPUSEL0_CH0_Msk) >> TAL_DMACPUSEL0_CH0_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_DMACPUSEL0_CH0_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->DMACPUSEL0.reg;
	tmp &= ~TAL_DMACPUSEL0_CH0_Msk;
	tmp |= value << TAL_DMACPUSEL0_CH0_Pos;
	((Tal *)hw)->DMACPUSEL0.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_DMACPUSEL0_CH0_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL0.reg &= ~TAL_DMACPUSEL0_CH0_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_DMACPUSEL0_CH0_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL0.reg ^= TAL_DMACPUSEL0_CH0_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_DMACPUSEL0_CH1_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL0.reg |= TAL_DMACPUSEL0_CH1_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_DMACPUSEL0_CH1_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->DMACPUSEL0.reg;
	tmp = (tmp & TAL_DMACPUSEL0_CH1_Msk) >> TAL_DMACPUSEL0_CH1_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_DMACPUSEL0_CH1_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->DMACPUSEL0.reg;
	tmp &= ~TAL_DMACPUSEL0_CH1_Msk;
	tmp |= value << TAL_DMACPUSEL0_CH1_Pos;
	((Tal *)hw)->DMACPUSEL0.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_DMACPUSEL0_CH1_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL0.reg &= ~TAL_DMACPUSEL0_CH1_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_DMACPUSEL0_CH1_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL0.reg ^= TAL_DMACPUSEL0_CH1_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_DMACPUSEL0_CH2_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL0.reg |= TAL_DMACPUSEL0_CH2_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_DMACPUSEL0_CH2_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->DMACPUSEL0.reg;
	tmp = (tmp & TAL_DMACPUSEL0_CH2_Msk) >> TAL_DMACPUSEL0_CH2_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_DMACPUSEL0_CH2_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->DMACPUSEL0.reg;
	tmp &= ~TAL_DMACPUSEL0_CH2_Msk;
	tmp |= value << TAL_DMACPUSEL0_CH2_Pos;
	((Tal *)hw)->DMACPUSEL0.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_DMACPUSEL0_CH2_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL0.reg &= ~TAL_DMACPUSEL0_CH2_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_DMACPUSEL0_CH2_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL0.reg ^= TAL_DMACPUSEL0_CH2_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_DMACPUSEL0_CH3_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL0.reg |= TAL_DMACPUSEL0_CH3_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_DMACPUSEL0_CH3_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->DMACPUSEL0.reg;
	tmp = (tmp & TAL_DMACPUSEL0_CH3_Msk) >> TAL_DMACPUSEL0_CH3_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_DMACPUSEL0_CH3_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->DMACPUSEL0.reg;
	tmp &= ~TAL_DMACPUSEL0_CH3_Msk;
	tmp |= value << TAL_DMACPUSEL0_CH3_Pos;
	((Tal *)hw)->DMACPUSEL0.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_DMACPUSEL0_CH3_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL0.reg &= ~TAL_DMACPUSEL0_CH3_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_DMACPUSEL0_CH3_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL0.reg ^= TAL_DMACPUSEL0_CH3_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_DMACPUSEL0_CH4_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL0.reg |= TAL_DMACPUSEL0_CH4_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_DMACPUSEL0_CH4_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->DMACPUSEL0.reg;
	tmp = (tmp & TAL_DMACPUSEL0_CH4_Msk) >> TAL_DMACPUSEL0_CH4_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_DMACPUSEL0_CH4_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->DMACPUSEL0.reg;
	tmp &= ~TAL_DMACPUSEL0_CH4_Msk;
	tmp |= value << TAL_DMACPUSEL0_CH4_Pos;
	((Tal *)hw)->DMACPUSEL0.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_DMACPUSEL0_CH4_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL0.reg &= ~TAL_DMACPUSEL0_CH4_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_DMACPUSEL0_CH4_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL0.reg ^= TAL_DMACPUSEL0_CH4_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_DMACPUSEL0_CH5_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL0.reg |= TAL_DMACPUSEL0_CH5_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_DMACPUSEL0_CH5_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->DMACPUSEL0.reg;
	tmp = (tmp & TAL_DMACPUSEL0_CH5_Msk) >> TAL_DMACPUSEL0_CH5_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_DMACPUSEL0_CH5_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->DMACPUSEL0.reg;
	tmp &= ~TAL_DMACPUSEL0_CH5_Msk;
	tmp |= value << TAL_DMACPUSEL0_CH5_Pos;
	((Tal *)hw)->DMACPUSEL0.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_DMACPUSEL0_CH5_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL0.reg &= ~TAL_DMACPUSEL0_CH5_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_DMACPUSEL0_CH5_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL0.reg ^= TAL_DMACPUSEL0_CH5_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_DMACPUSEL0_CH6_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL0.reg |= TAL_DMACPUSEL0_CH6_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_DMACPUSEL0_CH6_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->DMACPUSEL0.reg;
	tmp = (tmp & TAL_DMACPUSEL0_CH6_Msk) >> TAL_DMACPUSEL0_CH6_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_DMACPUSEL0_CH6_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->DMACPUSEL0.reg;
	tmp &= ~TAL_DMACPUSEL0_CH6_Msk;
	tmp |= value << TAL_DMACPUSEL0_CH6_Pos;
	((Tal *)hw)->DMACPUSEL0.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_DMACPUSEL0_CH6_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL0.reg &= ~TAL_DMACPUSEL0_CH6_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_DMACPUSEL0_CH6_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL0.reg ^= TAL_DMACPUSEL0_CH6_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_DMACPUSEL0_CH7_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL0.reg |= TAL_DMACPUSEL0_CH7_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_DMACPUSEL0_CH7_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->DMACPUSEL0.reg;
	tmp = (tmp & TAL_DMACPUSEL0_CH7_Msk) >> TAL_DMACPUSEL0_CH7_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_DMACPUSEL0_CH7_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->DMACPUSEL0.reg;
	tmp &= ~TAL_DMACPUSEL0_CH7_Msk;
	tmp |= value << TAL_DMACPUSEL0_CH7_Pos;
	((Tal *)hw)->DMACPUSEL0.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_DMACPUSEL0_CH7_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL0.reg &= ~TAL_DMACPUSEL0_CH7_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_DMACPUSEL0_CH7_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL0.reg ^= TAL_DMACPUSEL0_CH7_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_DMACPUSEL0_CH8_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL0.reg |= TAL_DMACPUSEL0_CH8_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_DMACPUSEL0_CH8_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->DMACPUSEL0.reg;
	tmp = (tmp & TAL_DMACPUSEL0_CH8_Msk) >> TAL_DMACPUSEL0_CH8_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_DMACPUSEL0_CH8_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->DMACPUSEL0.reg;
	tmp &= ~TAL_DMACPUSEL0_CH8_Msk;
	tmp |= value << TAL_DMACPUSEL0_CH8_Pos;
	((Tal *)hw)->DMACPUSEL0.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_DMACPUSEL0_CH8_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL0.reg &= ~TAL_DMACPUSEL0_CH8_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_DMACPUSEL0_CH8_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL0.reg ^= TAL_DMACPUSEL0_CH8_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_DMACPUSEL0_CH9_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL0.reg |= TAL_DMACPUSEL0_CH9_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_DMACPUSEL0_CH9_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->DMACPUSEL0.reg;
	tmp = (tmp & TAL_DMACPUSEL0_CH9_Msk) >> TAL_DMACPUSEL0_CH9_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_DMACPUSEL0_CH9_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->DMACPUSEL0.reg;
	tmp &= ~TAL_DMACPUSEL0_CH9_Msk;
	tmp |= value << TAL_DMACPUSEL0_CH9_Pos;
	((Tal *)hw)->DMACPUSEL0.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_DMACPUSEL0_CH9_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL0.reg &= ~TAL_DMACPUSEL0_CH9_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_DMACPUSEL0_CH9_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL0.reg ^= TAL_DMACPUSEL0_CH9_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_DMACPUSEL0_CH10_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL0.reg |= TAL_DMACPUSEL0_CH10_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_DMACPUSEL0_CH10_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->DMACPUSEL0.reg;
	tmp = (tmp & TAL_DMACPUSEL0_CH10_Msk) >> TAL_DMACPUSEL0_CH10_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_DMACPUSEL0_CH10_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->DMACPUSEL0.reg;
	tmp &= ~TAL_DMACPUSEL0_CH10_Msk;
	tmp |= value << TAL_DMACPUSEL0_CH10_Pos;
	((Tal *)hw)->DMACPUSEL0.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_DMACPUSEL0_CH10_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL0.reg &= ~TAL_DMACPUSEL0_CH10_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_DMACPUSEL0_CH10_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL0.reg ^= TAL_DMACPUSEL0_CH10_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_DMACPUSEL0_CH11_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL0.reg |= TAL_DMACPUSEL0_CH11_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_DMACPUSEL0_CH11_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->DMACPUSEL0.reg;
	tmp = (tmp & TAL_DMACPUSEL0_CH11_Msk) >> TAL_DMACPUSEL0_CH11_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_DMACPUSEL0_CH11_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->DMACPUSEL0.reg;
	tmp &= ~TAL_DMACPUSEL0_CH11_Msk;
	tmp |= value << TAL_DMACPUSEL0_CH11_Pos;
	((Tal *)hw)->DMACPUSEL0.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_DMACPUSEL0_CH11_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL0.reg &= ~TAL_DMACPUSEL0_CH11_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_DMACPUSEL0_CH11_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL0.reg ^= TAL_DMACPUSEL0_CH11_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_DMACPUSEL0_CH12_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL0.reg |= TAL_DMACPUSEL0_CH12_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_DMACPUSEL0_CH12_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->DMACPUSEL0.reg;
	tmp = (tmp & TAL_DMACPUSEL0_CH12_Msk) >> TAL_DMACPUSEL0_CH12_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_DMACPUSEL0_CH12_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->DMACPUSEL0.reg;
	tmp &= ~TAL_DMACPUSEL0_CH12_Msk;
	tmp |= value << TAL_DMACPUSEL0_CH12_Pos;
	((Tal *)hw)->DMACPUSEL0.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_DMACPUSEL0_CH12_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL0.reg &= ~TAL_DMACPUSEL0_CH12_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_DMACPUSEL0_CH12_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL0.reg ^= TAL_DMACPUSEL0_CH12_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_DMACPUSEL0_CH13_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL0.reg |= TAL_DMACPUSEL0_CH13_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_DMACPUSEL0_CH13_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->DMACPUSEL0.reg;
	tmp = (tmp & TAL_DMACPUSEL0_CH13_Msk) >> TAL_DMACPUSEL0_CH13_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_DMACPUSEL0_CH13_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->DMACPUSEL0.reg;
	tmp &= ~TAL_DMACPUSEL0_CH13_Msk;
	tmp |= value << TAL_DMACPUSEL0_CH13_Pos;
	((Tal *)hw)->DMACPUSEL0.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_DMACPUSEL0_CH13_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL0.reg &= ~TAL_DMACPUSEL0_CH13_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_DMACPUSEL0_CH13_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL0.reg ^= TAL_DMACPUSEL0_CH13_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_DMACPUSEL0_CH14_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL0.reg |= TAL_DMACPUSEL0_CH14_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_DMACPUSEL0_CH14_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->DMACPUSEL0.reg;
	tmp = (tmp & TAL_DMACPUSEL0_CH14_Msk) >> TAL_DMACPUSEL0_CH14_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_DMACPUSEL0_CH14_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->DMACPUSEL0.reg;
	tmp &= ~TAL_DMACPUSEL0_CH14_Msk;
	tmp |= value << TAL_DMACPUSEL0_CH14_Pos;
	((Tal *)hw)->DMACPUSEL0.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_DMACPUSEL0_CH14_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL0.reg &= ~TAL_DMACPUSEL0_CH14_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_DMACPUSEL0_CH14_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL0.reg ^= TAL_DMACPUSEL0_CH14_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_DMACPUSEL0_CH15_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL0.reg |= TAL_DMACPUSEL0_CH15_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_DMACPUSEL0_CH15_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->DMACPUSEL0.reg;
	tmp = (tmp & TAL_DMACPUSEL0_CH15_Msk) >> TAL_DMACPUSEL0_CH15_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_DMACPUSEL0_CH15_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->DMACPUSEL0.reg;
	tmp &= ~TAL_DMACPUSEL0_CH15_Msk;
	tmp |= value << TAL_DMACPUSEL0_CH15_Pos;
	((Tal *)hw)->DMACPUSEL0.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_DMACPUSEL0_CH15_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL0.reg &= ~TAL_DMACPUSEL0_CH15_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_DMACPUSEL0_CH15_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL0.reg ^= TAL_DMACPUSEL0_CH15_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_DMACPUSEL0_reg(const void *const hw, hri_tal_dmacpusel0_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL0.reg |= mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline hri_tal_dmacpusel0_reg_t hri_tal_get_DMACPUSEL0_reg(const void *const hw, hri_tal_dmacpusel0_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->DMACPUSEL0.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_tal_write_DMACPUSEL0_reg(const void *const hw, hri_tal_dmacpusel0_reg_t data)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL0.reg = data;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_DMACPUSEL0_reg(const void *const hw, hri_tal_dmacpusel0_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL0.reg &= ~mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_DMACPUSEL0_reg(const void *const hw, hri_tal_dmacpusel0_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL0.reg ^= mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline hri_tal_dmacpusel0_reg_t hri_tal_read_DMACPUSEL0_reg(const void *const hw)
{
	return ((Tal *)hw)->DMACPUSEL0.reg;
}

static inline void hri_tal_set_DMACPUSEL1_CH16_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL1.reg |= TAL_DMACPUSEL1_CH16_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_DMACPUSEL1_CH16_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->DMACPUSEL1.reg;
	tmp = (tmp & TAL_DMACPUSEL1_CH16_Msk) >> TAL_DMACPUSEL1_CH16_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_DMACPUSEL1_CH16_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->DMACPUSEL1.reg;
	tmp &= ~TAL_DMACPUSEL1_CH16_Msk;
	tmp |= value << TAL_DMACPUSEL1_CH16_Pos;
	((Tal *)hw)->DMACPUSEL1.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_DMACPUSEL1_CH16_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL1.reg &= ~TAL_DMACPUSEL1_CH16_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_DMACPUSEL1_CH16_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL1.reg ^= TAL_DMACPUSEL1_CH16_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_DMACPUSEL1_CH17_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL1.reg |= TAL_DMACPUSEL1_CH17_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_DMACPUSEL1_CH17_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->DMACPUSEL1.reg;
	tmp = (tmp & TAL_DMACPUSEL1_CH17_Msk) >> TAL_DMACPUSEL1_CH17_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_DMACPUSEL1_CH17_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->DMACPUSEL1.reg;
	tmp &= ~TAL_DMACPUSEL1_CH17_Msk;
	tmp |= value << TAL_DMACPUSEL1_CH17_Pos;
	((Tal *)hw)->DMACPUSEL1.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_DMACPUSEL1_CH17_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL1.reg &= ~TAL_DMACPUSEL1_CH17_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_DMACPUSEL1_CH17_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL1.reg ^= TAL_DMACPUSEL1_CH17_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_DMACPUSEL1_CH18_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL1.reg |= TAL_DMACPUSEL1_CH18_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_DMACPUSEL1_CH18_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->DMACPUSEL1.reg;
	tmp = (tmp & TAL_DMACPUSEL1_CH18_Msk) >> TAL_DMACPUSEL1_CH18_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_DMACPUSEL1_CH18_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->DMACPUSEL1.reg;
	tmp &= ~TAL_DMACPUSEL1_CH18_Msk;
	tmp |= value << TAL_DMACPUSEL1_CH18_Pos;
	((Tal *)hw)->DMACPUSEL1.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_DMACPUSEL1_CH18_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL1.reg &= ~TAL_DMACPUSEL1_CH18_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_DMACPUSEL1_CH18_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL1.reg ^= TAL_DMACPUSEL1_CH18_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_DMACPUSEL1_CH19_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL1.reg |= TAL_DMACPUSEL1_CH19_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_DMACPUSEL1_CH19_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->DMACPUSEL1.reg;
	tmp = (tmp & TAL_DMACPUSEL1_CH19_Msk) >> TAL_DMACPUSEL1_CH19_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_DMACPUSEL1_CH19_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->DMACPUSEL1.reg;
	tmp &= ~TAL_DMACPUSEL1_CH19_Msk;
	tmp |= value << TAL_DMACPUSEL1_CH19_Pos;
	((Tal *)hw)->DMACPUSEL1.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_DMACPUSEL1_CH19_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL1.reg &= ~TAL_DMACPUSEL1_CH19_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_DMACPUSEL1_CH19_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL1.reg ^= TAL_DMACPUSEL1_CH19_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_DMACPUSEL1_CH20_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL1.reg |= TAL_DMACPUSEL1_CH20_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_DMACPUSEL1_CH20_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->DMACPUSEL1.reg;
	tmp = (tmp & TAL_DMACPUSEL1_CH20_Msk) >> TAL_DMACPUSEL1_CH20_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_DMACPUSEL1_CH20_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->DMACPUSEL1.reg;
	tmp &= ~TAL_DMACPUSEL1_CH20_Msk;
	tmp |= value << TAL_DMACPUSEL1_CH20_Pos;
	((Tal *)hw)->DMACPUSEL1.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_DMACPUSEL1_CH20_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL1.reg &= ~TAL_DMACPUSEL1_CH20_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_DMACPUSEL1_CH20_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL1.reg ^= TAL_DMACPUSEL1_CH20_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_DMACPUSEL1_CH21_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL1.reg |= TAL_DMACPUSEL1_CH21_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_DMACPUSEL1_CH21_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->DMACPUSEL1.reg;
	tmp = (tmp & TAL_DMACPUSEL1_CH21_Msk) >> TAL_DMACPUSEL1_CH21_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_DMACPUSEL1_CH21_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->DMACPUSEL1.reg;
	tmp &= ~TAL_DMACPUSEL1_CH21_Msk;
	tmp |= value << TAL_DMACPUSEL1_CH21_Pos;
	((Tal *)hw)->DMACPUSEL1.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_DMACPUSEL1_CH21_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL1.reg &= ~TAL_DMACPUSEL1_CH21_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_DMACPUSEL1_CH21_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL1.reg ^= TAL_DMACPUSEL1_CH21_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_DMACPUSEL1_CH22_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL1.reg |= TAL_DMACPUSEL1_CH22_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_DMACPUSEL1_CH22_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->DMACPUSEL1.reg;
	tmp = (tmp & TAL_DMACPUSEL1_CH22_Msk) >> TAL_DMACPUSEL1_CH22_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_DMACPUSEL1_CH22_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->DMACPUSEL1.reg;
	tmp &= ~TAL_DMACPUSEL1_CH22_Msk;
	tmp |= value << TAL_DMACPUSEL1_CH22_Pos;
	((Tal *)hw)->DMACPUSEL1.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_DMACPUSEL1_CH22_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL1.reg &= ~TAL_DMACPUSEL1_CH22_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_DMACPUSEL1_CH22_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL1.reg ^= TAL_DMACPUSEL1_CH22_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_DMACPUSEL1_CH23_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL1.reg |= TAL_DMACPUSEL1_CH23_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_DMACPUSEL1_CH23_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->DMACPUSEL1.reg;
	tmp = (tmp & TAL_DMACPUSEL1_CH23_Msk) >> TAL_DMACPUSEL1_CH23_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_DMACPUSEL1_CH23_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->DMACPUSEL1.reg;
	tmp &= ~TAL_DMACPUSEL1_CH23_Msk;
	tmp |= value << TAL_DMACPUSEL1_CH23_Pos;
	((Tal *)hw)->DMACPUSEL1.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_DMACPUSEL1_CH23_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL1.reg &= ~TAL_DMACPUSEL1_CH23_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_DMACPUSEL1_CH23_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL1.reg ^= TAL_DMACPUSEL1_CH23_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_DMACPUSEL1_CH24_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL1.reg |= TAL_DMACPUSEL1_CH24_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_DMACPUSEL1_CH24_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->DMACPUSEL1.reg;
	tmp = (tmp & TAL_DMACPUSEL1_CH24_Msk) >> TAL_DMACPUSEL1_CH24_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_DMACPUSEL1_CH24_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->DMACPUSEL1.reg;
	tmp &= ~TAL_DMACPUSEL1_CH24_Msk;
	tmp |= value << TAL_DMACPUSEL1_CH24_Pos;
	((Tal *)hw)->DMACPUSEL1.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_DMACPUSEL1_CH24_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL1.reg &= ~TAL_DMACPUSEL1_CH24_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_DMACPUSEL1_CH24_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL1.reg ^= TAL_DMACPUSEL1_CH24_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_DMACPUSEL1_CH25_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL1.reg |= TAL_DMACPUSEL1_CH25_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_DMACPUSEL1_CH25_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->DMACPUSEL1.reg;
	tmp = (tmp & TAL_DMACPUSEL1_CH25_Msk) >> TAL_DMACPUSEL1_CH25_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_DMACPUSEL1_CH25_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->DMACPUSEL1.reg;
	tmp &= ~TAL_DMACPUSEL1_CH25_Msk;
	tmp |= value << TAL_DMACPUSEL1_CH25_Pos;
	((Tal *)hw)->DMACPUSEL1.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_DMACPUSEL1_CH25_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL1.reg &= ~TAL_DMACPUSEL1_CH25_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_DMACPUSEL1_CH25_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL1.reg ^= TAL_DMACPUSEL1_CH25_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_DMACPUSEL1_CH26_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL1.reg |= TAL_DMACPUSEL1_CH26_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_DMACPUSEL1_CH26_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->DMACPUSEL1.reg;
	tmp = (tmp & TAL_DMACPUSEL1_CH26_Msk) >> TAL_DMACPUSEL1_CH26_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_DMACPUSEL1_CH26_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->DMACPUSEL1.reg;
	tmp &= ~TAL_DMACPUSEL1_CH26_Msk;
	tmp |= value << TAL_DMACPUSEL1_CH26_Pos;
	((Tal *)hw)->DMACPUSEL1.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_DMACPUSEL1_CH26_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL1.reg &= ~TAL_DMACPUSEL1_CH26_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_DMACPUSEL1_CH26_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL1.reg ^= TAL_DMACPUSEL1_CH26_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_DMACPUSEL1_CH27_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL1.reg |= TAL_DMACPUSEL1_CH27_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_DMACPUSEL1_CH27_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->DMACPUSEL1.reg;
	tmp = (tmp & TAL_DMACPUSEL1_CH27_Msk) >> TAL_DMACPUSEL1_CH27_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_DMACPUSEL1_CH27_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->DMACPUSEL1.reg;
	tmp &= ~TAL_DMACPUSEL1_CH27_Msk;
	tmp |= value << TAL_DMACPUSEL1_CH27_Pos;
	((Tal *)hw)->DMACPUSEL1.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_DMACPUSEL1_CH27_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL1.reg &= ~TAL_DMACPUSEL1_CH27_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_DMACPUSEL1_CH27_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL1.reg ^= TAL_DMACPUSEL1_CH27_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_DMACPUSEL1_CH28_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL1.reg |= TAL_DMACPUSEL1_CH28_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_DMACPUSEL1_CH28_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->DMACPUSEL1.reg;
	tmp = (tmp & TAL_DMACPUSEL1_CH28_Msk) >> TAL_DMACPUSEL1_CH28_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_DMACPUSEL1_CH28_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->DMACPUSEL1.reg;
	tmp &= ~TAL_DMACPUSEL1_CH28_Msk;
	tmp |= value << TAL_DMACPUSEL1_CH28_Pos;
	((Tal *)hw)->DMACPUSEL1.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_DMACPUSEL1_CH28_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL1.reg &= ~TAL_DMACPUSEL1_CH28_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_DMACPUSEL1_CH28_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL1.reg ^= TAL_DMACPUSEL1_CH28_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_DMACPUSEL1_CH29_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL1.reg |= TAL_DMACPUSEL1_CH29_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_DMACPUSEL1_CH29_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->DMACPUSEL1.reg;
	tmp = (tmp & TAL_DMACPUSEL1_CH29_Msk) >> TAL_DMACPUSEL1_CH29_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_DMACPUSEL1_CH29_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->DMACPUSEL1.reg;
	tmp &= ~TAL_DMACPUSEL1_CH29_Msk;
	tmp |= value << TAL_DMACPUSEL1_CH29_Pos;
	((Tal *)hw)->DMACPUSEL1.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_DMACPUSEL1_CH29_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL1.reg &= ~TAL_DMACPUSEL1_CH29_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_DMACPUSEL1_CH29_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL1.reg ^= TAL_DMACPUSEL1_CH29_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_DMACPUSEL1_CH30_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL1.reg |= TAL_DMACPUSEL1_CH30_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_DMACPUSEL1_CH30_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->DMACPUSEL1.reg;
	tmp = (tmp & TAL_DMACPUSEL1_CH30_Msk) >> TAL_DMACPUSEL1_CH30_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_DMACPUSEL1_CH30_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->DMACPUSEL1.reg;
	tmp &= ~TAL_DMACPUSEL1_CH30_Msk;
	tmp |= value << TAL_DMACPUSEL1_CH30_Pos;
	((Tal *)hw)->DMACPUSEL1.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_DMACPUSEL1_CH30_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL1.reg &= ~TAL_DMACPUSEL1_CH30_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_DMACPUSEL1_CH30_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL1.reg ^= TAL_DMACPUSEL1_CH30_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_DMACPUSEL1_CH31_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL1.reg |= TAL_DMACPUSEL1_CH31_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_DMACPUSEL1_CH31_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->DMACPUSEL1.reg;
	tmp = (tmp & TAL_DMACPUSEL1_CH31_Msk) >> TAL_DMACPUSEL1_CH31_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_DMACPUSEL1_CH31_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->DMACPUSEL1.reg;
	tmp &= ~TAL_DMACPUSEL1_CH31_Msk;
	tmp |= value << TAL_DMACPUSEL1_CH31_Pos;
	((Tal *)hw)->DMACPUSEL1.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_DMACPUSEL1_CH31_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL1.reg &= ~TAL_DMACPUSEL1_CH31_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_DMACPUSEL1_CH31_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL1.reg ^= TAL_DMACPUSEL1_CH31_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_DMACPUSEL1_reg(const void *const hw, hri_tal_dmacpusel1_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL1.reg |= mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline hri_tal_dmacpusel1_reg_t hri_tal_get_DMACPUSEL1_reg(const void *const hw, hri_tal_dmacpusel1_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->DMACPUSEL1.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_tal_write_DMACPUSEL1_reg(const void *const hw, hri_tal_dmacpusel1_reg_t data)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL1.reg = data;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_DMACPUSEL1_reg(const void *const hw, hri_tal_dmacpusel1_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL1.reg &= ~mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_DMACPUSEL1_reg(const void *const hw, hri_tal_dmacpusel1_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->DMACPUSEL1.reg ^= mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline hri_tal_dmacpusel1_reg_t hri_tal_read_DMACPUSEL1_reg(const void *const hw)
{
	return ((Tal *)hw)->DMACPUSEL1.reg;
}

static inline void hri_tal_set_EVCPUSEL0_CH0_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EVCPUSEL0.reg |= TAL_EVCPUSEL0_CH0_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_EVCPUSEL0_CH0_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->EVCPUSEL0.reg;
	tmp = (tmp & TAL_EVCPUSEL0_CH0_Msk) >> TAL_EVCPUSEL0_CH0_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_EVCPUSEL0_CH0_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->EVCPUSEL0.reg;
	tmp &= ~TAL_EVCPUSEL0_CH0_Msk;
	tmp |= value << TAL_EVCPUSEL0_CH0_Pos;
	((Tal *)hw)->EVCPUSEL0.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_EVCPUSEL0_CH0_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EVCPUSEL0.reg &= ~TAL_EVCPUSEL0_CH0_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_EVCPUSEL0_CH0_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EVCPUSEL0.reg ^= TAL_EVCPUSEL0_CH0_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_EVCPUSEL0_CH1_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EVCPUSEL0.reg |= TAL_EVCPUSEL0_CH1_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_EVCPUSEL0_CH1_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->EVCPUSEL0.reg;
	tmp = (tmp & TAL_EVCPUSEL0_CH1_Msk) >> TAL_EVCPUSEL0_CH1_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_EVCPUSEL0_CH1_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->EVCPUSEL0.reg;
	tmp &= ~TAL_EVCPUSEL0_CH1_Msk;
	tmp |= value << TAL_EVCPUSEL0_CH1_Pos;
	((Tal *)hw)->EVCPUSEL0.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_EVCPUSEL0_CH1_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EVCPUSEL0.reg &= ~TAL_EVCPUSEL0_CH1_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_EVCPUSEL0_CH1_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EVCPUSEL0.reg ^= TAL_EVCPUSEL0_CH1_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_EVCPUSEL0_CH2_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EVCPUSEL0.reg |= TAL_EVCPUSEL0_CH2_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_EVCPUSEL0_CH2_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->EVCPUSEL0.reg;
	tmp = (tmp & TAL_EVCPUSEL0_CH2_Msk) >> TAL_EVCPUSEL0_CH2_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_EVCPUSEL0_CH2_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->EVCPUSEL0.reg;
	tmp &= ~TAL_EVCPUSEL0_CH2_Msk;
	tmp |= value << TAL_EVCPUSEL0_CH2_Pos;
	((Tal *)hw)->EVCPUSEL0.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_EVCPUSEL0_CH2_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EVCPUSEL0.reg &= ~TAL_EVCPUSEL0_CH2_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_EVCPUSEL0_CH2_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EVCPUSEL0.reg ^= TAL_EVCPUSEL0_CH2_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_EVCPUSEL0_CH3_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EVCPUSEL0.reg |= TAL_EVCPUSEL0_CH3_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_EVCPUSEL0_CH3_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->EVCPUSEL0.reg;
	tmp = (tmp & TAL_EVCPUSEL0_CH3_Msk) >> TAL_EVCPUSEL0_CH3_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_EVCPUSEL0_CH3_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->EVCPUSEL0.reg;
	tmp &= ~TAL_EVCPUSEL0_CH3_Msk;
	tmp |= value << TAL_EVCPUSEL0_CH3_Pos;
	((Tal *)hw)->EVCPUSEL0.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_EVCPUSEL0_CH3_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EVCPUSEL0.reg &= ~TAL_EVCPUSEL0_CH3_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_EVCPUSEL0_CH3_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EVCPUSEL0.reg ^= TAL_EVCPUSEL0_CH3_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_EVCPUSEL0_CH4_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EVCPUSEL0.reg |= TAL_EVCPUSEL0_CH4_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_EVCPUSEL0_CH4_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->EVCPUSEL0.reg;
	tmp = (tmp & TAL_EVCPUSEL0_CH4_Msk) >> TAL_EVCPUSEL0_CH4_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_EVCPUSEL0_CH4_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->EVCPUSEL0.reg;
	tmp &= ~TAL_EVCPUSEL0_CH4_Msk;
	tmp |= value << TAL_EVCPUSEL0_CH4_Pos;
	((Tal *)hw)->EVCPUSEL0.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_EVCPUSEL0_CH4_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EVCPUSEL0.reg &= ~TAL_EVCPUSEL0_CH4_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_EVCPUSEL0_CH4_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EVCPUSEL0.reg ^= TAL_EVCPUSEL0_CH4_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_EVCPUSEL0_CH5_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EVCPUSEL0.reg |= TAL_EVCPUSEL0_CH5_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_EVCPUSEL0_CH5_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->EVCPUSEL0.reg;
	tmp = (tmp & TAL_EVCPUSEL0_CH5_Msk) >> TAL_EVCPUSEL0_CH5_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_EVCPUSEL0_CH5_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->EVCPUSEL0.reg;
	tmp &= ~TAL_EVCPUSEL0_CH5_Msk;
	tmp |= value << TAL_EVCPUSEL0_CH5_Pos;
	((Tal *)hw)->EVCPUSEL0.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_EVCPUSEL0_CH5_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EVCPUSEL0.reg &= ~TAL_EVCPUSEL0_CH5_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_EVCPUSEL0_CH5_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EVCPUSEL0.reg ^= TAL_EVCPUSEL0_CH5_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_EVCPUSEL0_CH6_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EVCPUSEL0.reg |= TAL_EVCPUSEL0_CH6_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_EVCPUSEL0_CH6_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->EVCPUSEL0.reg;
	tmp = (tmp & TAL_EVCPUSEL0_CH6_Msk) >> TAL_EVCPUSEL0_CH6_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_EVCPUSEL0_CH6_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->EVCPUSEL0.reg;
	tmp &= ~TAL_EVCPUSEL0_CH6_Msk;
	tmp |= value << TAL_EVCPUSEL0_CH6_Pos;
	((Tal *)hw)->EVCPUSEL0.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_EVCPUSEL0_CH6_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EVCPUSEL0.reg &= ~TAL_EVCPUSEL0_CH6_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_EVCPUSEL0_CH6_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EVCPUSEL0.reg ^= TAL_EVCPUSEL0_CH6_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_EVCPUSEL0_CH7_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EVCPUSEL0.reg |= TAL_EVCPUSEL0_CH7_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_EVCPUSEL0_CH7_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->EVCPUSEL0.reg;
	tmp = (tmp & TAL_EVCPUSEL0_CH7_Msk) >> TAL_EVCPUSEL0_CH7_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_EVCPUSEL0_CH7_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->EVCPUSEL0.reg;
	tmp &= ~TAL_EVCPUSEL0_CH7_Msk;
	tmp |= value << TAL_EVCPUSEL0_CH7_Pos;
	((Tal *)hw)->EVCPUSEL0.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_EVCPUSEL0_CH7_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EVCPUSEL0.reg &= ~TAL_EVCPUSEL0_CH7_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_EVCPUSEL0_CH7_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EVCPUSEL0.reg ^= TAL_EVCPUSEL0_CH7_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_EVCPUSEL0_CH8_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EVCPUSEL0.reg |= TAL_EVCPUSEL0_CH8_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_EVCPUSEL0_CH8_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->EVCPUSEL0.reg;
	tmp = (tmp & TAL_EVCPUSEL0_CH8_Msk) >> TAL_EVCPUSEL0_CH8_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_EVCPUSEL0_CH8_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->EVCPUSEL0.reg;
	tmp &= ~TAL_EVCPUSEL0_CH8_Msk;
	tmp |= value << TAL_EVCPUSEL0_CH8_Pos;
	((Tal *)hw)->EVCPUSEL0.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_EVCPUSEL0_CH8_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EVCPUSEL0.reg &= ~TAL_EVCPUSEL0_CH8_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_EVCPUSEL0_CH8_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EVCPUSEL0.reg ^= TAL_EVCPUSEL0_CH8_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_EVCPUSEL0_CH9_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EVCPUSEL0.reg |= TAL_EVCPUSEL0_CH9_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_EVCPUSEL0_CH9_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->EVCPUSEL0.reg;
	tmp = (tmp & TAL_EVCPUSEL0_CH9_Msk) >> TAL_EVCPUSEL0_CH9_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_EVCPUSEL0_CH9_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->EVCPUSEL0.reg;
	tmp &= ~TAL_EVCPUSEL0_CH9_Msk;
	tmp |= value << TAL_EVCPUSEL0_CH9_Pos;
	((Tal *)hw)->EVCPUSEL0.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_EVCPUSEL0_CH9_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EVCPUSEL0.reg &= ~TAL_EVCPUSEL0_CH9_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_EVCPUSEL0_CH9_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EVCPUSEL0.reg ^= TAL_EVCPUSEL0_CH9_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_EVCPUSEL0_CH10_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EVCPUSEL0.reg |= TAL_EVCPUSEL0_CH10_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_EVCPUSEL0_CH10_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->EVCPUSEL0.reg;
	tmp = (tmp & TAL_EVCPUSEL0_CH10_Msk) >> TAL_EVCPUSEL0_CH10_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_EVCPUSEL0_CH10_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->EVCPUSEL0.reg;
	tmp &= ~TAL_EVCPUSEL0_CH10_Msk;
	tmp |= value << TAL_EVCPUSEL0_CH10_Pos;
	((Tal *)hw)->EVCPUSEL0.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_EVCPUSEL0_CH10_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EVCPUSEL0.reg &= ~TAL_EVCPUSEL0_CH10_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_EVCPUSEL0_CH10_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EVCPUSEL0.reg ^= TAL_EVCPUSEL0_CH10_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_EVCPUSEL0_CH11_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EVCPUSEL0.reg |= TAL_EVCPUSEL0_CH11_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_EVCPUSEL0_CH11_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->EVCPUSEL0.reg;
	tmp = (tmp & TAL_EVCPUSEL0_CH11_Msk) >> TAL_EVCPUSEL0_CH11_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_EVCPUSEL0_CH11_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->EVCPUSEL0.reg;
	tmp &= ~TAL_EVCPUSEL0_CH11_Msk;
	tmp |= value << TAL_EVCPUSEL0_CH11_Pos;
	((Tal *)hw)->EVCPUSEL0.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_EVCPUSEL0_CH11_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EVCPUSEL0.reg &= ~TAL_EVCPUSEL0_CH11_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_EVCPUSEL0_CH11_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EVCPUSEL0.reg ^= TAL_EVCPUSEL0_CH11_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_EVCPUSEL0_reg(const void *const hw, hri_tal_evcpusel0_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EVCPUSEL0.reg |= mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline hri_tal_evcpusel0_reg_t hri_tal_get_EVCPUSEL0_reg(const void *const hw, hri_tal_evcpusel0_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->EVCPUSEL0.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_tal_write_EVCPUSEL0_reg(const void *const hw, hri_tal_evcpusel0_reg_t data)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EVCPUSEL0.reg = data;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_EVCPUSEL0_reg(const void *const hw, hri_tal_evcpusel0_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EVCPUSEL0.reg &= ~mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_EVCPUSEL0_reg(const void *const hw, hri_tal_evcpusel0_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EVCPUSEL0.reg ^= mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline hri_tal_evcpusel0_reg_t hri_tal_read_EVCPUSEL0_reg(const void *const hw)
{
	return ((Tal *)hw)->EVCPUSEL0.reg;
}

static inline void hri_tal_set_EICCPUSEL0_EXTINT0_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EICCPUSEL0.reg |= TAL_EICCPUSEL0_EXTINT0_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_EICCPUSEL0_EXTINT0_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->EICCPUSEL0.reg;
	tmp = (tmp & TAL_EICCPUSEL0_EXTINT0_Msk) >> TAL_EICCPUSEL0_EXTINT0_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_EICCPUSEL0_EXTINT0_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->EICCPUSEL0.reg;
	tmp &= ~TAL_EICCPUSEL0_EXTINT0_Msk;
	tmp |= value << TAL_EICCPUSEL0_EXTINT0_Pos;
	((Tal *)hw)->EICCPUSEL0.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_EICCPUSEL0_EXTINT0_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EICCPUSEL0.reg &= ~TAL_EICCPUSEL0_EXTINT0_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_EICCPUSEL0_EXTINT0_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EICCPUSEL0.reg ^= TAL_EICCPUSEL0_EXTINT0_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_EICCPUSEL0_EXTINT1_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EICCPUSEL0.reg |= TAL_EICCPUSEL0_EXTINT1_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_EICCPUSEL0_EXTINT1_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->EICCPUSEL0.reg;
	tmp = (tmp & TAL_EICCPUSEL0_EXTINT1_Msk) >> TAL_EICCPUSEL0_EXTINT1_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_EICCPUSEL0_EXTINT1_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->EICCPUSEL0.reg;
	tmp &= ~TAL_EICCPUSEL0_EXTINT1_Msk;
	tmp |= value << TAL_EICCPUSEL0_EXTINT1_Pos;
	((Tal *)hw)->EICCPUSEL0.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_EICCPUSEL0_EXTINT1_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EICCPUSEL0.reg &= ~TAL_EICCPUSEL0_EXTINT1_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_EICCPUSEL0_EXTINT1_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EICCPUSEL0.reg ^= TAL_EICCPUSEL0_EXTINT1_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_EICCPUSEL0_EXTINT2_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EICCPUSEL0.reg |= TAL_EICCPUSEL0_EXTINT2_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_EICCPUSEL0_EXTINT2_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->EICCPUSEL0.reg;
	tmp = (tmp & TAL_EICCPUSEL0_EXTINT2_Msk) >> TAL_EICCPUSEL0_EXTINT2_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_EICCPUSEL0_EXTINT2_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->EICCPUSEL0.reg;
	tmp &= ~TAL_EICCPUSEL0_EXTINT2_Msk;
	tmp |= value << TAL_EICCPUSEL0_EXTINT2_Pos;
	((Tal *)hw)->EICCPUSEL0.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_EICCPUSEL0_EXTINT2_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EICCPUSEL0.reg &= ~TAL_EICCPUSEL0_EXTINT2_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_EICCPUSEL0_EXTINT2_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EICCPUSEL0.reg ^= TAL_EICCPUSEL0_EXTINT2_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_EICCPUSEL0_EXTINT3_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EICCPUSEL0.reg |= TAL_EICCPUSEL0_EXTINT3_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_EICCPUSEL0_EXTINT3_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->EICCPUSEL0.reg;
	tmp = (tmp & TAL_EICCPUSEL0_EXTINT3_Msk) >> TAL_EICCPUSEL0_EXTINT3_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_EICCPUSEL0_EXTINT3_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->EICCPUSEL0.reg;
	tmp &= ~TAL_EICCPUSEL0_EXTINT3_Msk;
	tmp |= value << TAL_EICCPUSEL0_EXTINT3_Pos;
	((Tal *)hw)->EICCPUSEL0.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_EICCPUSEL0_EXTINT3_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EICCPUSEL0.reg &= ~TAL_EICCPUSEL0_EXTINT3_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_EICCPUSEL0_EXTINT3_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EICCPUSEL0.reg ^= TAL_EICCPUSEL0_EXTINT3_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_EICCPUSEL0_EXTINT4_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EICCPUSEL0.reg |= TAL_EICCPUSEL0_EXTINT4_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_EICCPUSEL0_EXTINT4_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->EICCPUSEL0.reg;
	tmp = (tmp & TAL_EICCPUSEL0_EXTINT4_Msk) >> TAL_EICCPUSEL0_EXTINT4_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_EICCPUSEL0_EXTINT4_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->EICCPUSEL0.reg;
	tmp &= ~TAL_EICCPUSEL0_EXTINT4_Msk;
	tmp |= value << TAL_EICCPUSEL0_EXTINT4_Pos;
	((Tal *)hw)->EICCPUSEL0.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_EICCPUSEL0_EXTINT4_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EICCPUSEL0.reg &= ~TAL_EICCPUSEL0_EXTINT4_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_EICCPUSEL0_EXTINT4_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EICCPUSEL0.reg ^= TAL_EICCPUSEL0_EXTINT4_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_EICCPUSEL0_EXTINT5_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EICCPUSEL0.reg |= TAL_EICCPUSEL0_EXTINT5_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_EICCPUSEL0_EXTINT5_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->EICCPUSEL0.reg;
	tmp = (tmp & TAL_EICCPUSEL0_EXTINT5_Msk) >> TAL_EICCPUSEL0_EXTINT5_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_EICCPUSEL0_EXTINT5_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->EICCPUSEL0.reg;
	tmp &= ~TAL_EICCPUSEL0_EXTINT5_Msk;
	tmp |= value << TAL_EICCPUSEL0_EXTINT5_Pos;
	((Tal *)hw)->EICCPUSEL0.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_EICCPUSEL0_EXTINT5_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EICCPUSEL0.reg &= ~TAL_EICCPUSEL0_EXTINT5_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_EICCPUSEL0_EXTINT5_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EICCPUSEL0.reg ^= TAL_EICCPUSEL0_EXTINT5_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_EICCPUSEL0_EXTINT6_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EICCPUSEL0.reg |= TAL_EICCPUSEL0_EXTINT6_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_EICCPUSEL0_EXTINT6_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->EICCPUSEL0.reg;
	tmp = (tmp & TAL_EICCPUSEL0_EXTINT6_Msk) >> TAL_EICCPUSEL0_EXTINT6_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_EICCPUSEL0_EXTINT6_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->EICCPUSEL0.reg;
	tmp &= ~TAL_EICCPUSEL0_EXTINT6_Msk;
	tmp |= value << TAL_EICCPUSEL0_EXTINT6_Pos;
	((Tal *)hw)->EICCPUSEL0.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_EICCPUSEL0_EXTINT6_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EICCPUSEL0.reg &= ~TAL_EICCPUSEL0_EXTINT6_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_EICCPUSEL0_EXTINT6_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EICCPUSEL0.reg ^= TAL_EICCPUSEL0_EXTINT6_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_EICCPUSEL0_EXTINT7_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EICCPUSEL0.reg |= TAL_EICCPUSEL0_EXTINT7_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_EICCPUSEL0_EXTINT7_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->EICCPUSEL0.reg;
	tmp = (tmp & TAL_EICCPUSEL0_EXTINT7_Msk) >> TAL_EICCPUSEL0_EXTINT7_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_EICCPUSEL0_EXTINT7_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->EICCPUSEL0.reg;
	tmp &= ~TAL_EICCPUSEL0_EXTINT7_Msk;
	tmp |= value << TAL_EICCPUSEL0_EXTINT7_Pos;
	((Tal *)hw)->EICCPUSEL0.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_EICCPUSEL0_EXTINT7_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EICCPUSEL0.reg &= ~TAL_EICCPUSEL0_EXTINT7_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_EICCPUSEL0_EXTINT7_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EICCPUSEL0.reg ^= TAL_EICCPUSEL0_EXTINT7_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_EICCPUSEL0_EXTINT8_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EICCPUSEL0.reg |= TAL_EICCPUSEL0_EXTINT8_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_EICCPUSEL0_EXTINT8_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->EICCPUSEL0.reg;
	tmp = (tmp & TAL_EICCPUSEL0_EXTINT8_Msk) >> TAL_EICCPUSEL0_EXTINT8_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_EICCPUSEL0_EXTINT8_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->EICCPUSEL0.reg;
	tmp &= ~TAL_EICCPUSEL0_EXTINT8_Msk;
	tmp |= value << TAL_EICCPUSEL0_EXTINT8_Pos;
	((Tal *)hw)->EICCPUSEL0.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_EICCPUSEL0_EXTINT8_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EICCPUSEL0.reg &= ~TAL_EICCPUSEL0_EXTINT8_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_EICCPUSEL0_EXTINT8_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EICCPUSEL0.reg ^= TAL_EICCPUSEL0_EXTINT8_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_EICCPUSEL0_EXTINT9_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EICCPUSEL0.reg |= TAL_EICCPUSEL0_EXTINT9_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_EICCPUSEL0_EXTINT9_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->EICCPUSEL0.reg;
	tmp = (tmp & TAL_EICCPUSEL0_EXTINT9_Msk) >> TAL_EICCPUSEL0_EXTINT9_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_EICCPUSEL0_EXTINT9_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->EICCPUSEL0.reg;
	tmp &= ~TAL_EICCPUSEL0_EXTINT9_Msk;
	tmp |= value << TAL_EICCPUSEL0_EXTINT9_Pos;
	((Tal *)hw)->EICCPUSEL0.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_EICCPUSEL0_EXTINT9_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EICCPUSEL0.reg &= ~TAL_EICCPUSEL0_EXTINT9_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_EICCPUSEL0_EXTINT9_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EICCPUSEL0.reg ^= TAL_EICCPUSEL0_EXTINT9_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_EICCPUSEL0_EXTINT10_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EICCPUSEL0.reg |= TAL_EICCPUSEL0_EXTINT10_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_EICCPUSEL0_EXTINT10_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->EICCPUSEL0.reg;
	tmp = (tmp & TAL_EICCPUSEL0_EXTINT10_Msk) >> TAL_EICCPUSEL0_EXTINT10_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_EICCPUSEL0_EXTINT10_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->EICCPUSEL0.reg;
	tmp &= ~TAL_EICCPUSEL0_EXTINT10_Msk;
	tmp |= value << TAL_EICCPUSEL0_EXTINT10_Pos;
	((Tal *)hw)->EICCPUSEL0.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_EICCPUSEL0_EXTINT10_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EICCPUSEL0.reg &= ~TAL_EICCPUSEL0_EXTINT10_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_EICCPUSEL0_EXTINT10_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EICCPUSEL0.reg ^= TAL_EICCPUSEL0_EXTINT10_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_EICCPUSEL0_EXTINT11_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EICCPUSEL0.reg |= TAL_EICCPUSEL0_EXTINT11_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_EICCPUSEL0_EXTINT11_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->EICCPUSEL0.reg;
	tmp = (tmp & TAL_EICCPUSEL0_EXTINT11_Msk) >> TAL_EICCPUSEL0_EXTINT11_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_EICCPUSEL0_EXTINT11_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->EICCPUSEL0.reg;
	tmp &= ~TAL_EICCPUSEL0_EXTINT11_Msk;
	tmp |= value << TAL_EICCPUSEL0_EXTINT11_Pos;
	((Tal *)hw)->EICCPUSEL0.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_EICCPUSEL0_EXTINT11_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EICCPUSEL0.reg &= ~TAL_EICCPUSEL0_EXTINT11_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_EICCPUSEL0_EXTINT11_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EICCPUSEL0.reg ^= TAL_EICCPUSEL0_EXTINT11_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_EICCPUSEL0_EXTINT12_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EICCPUSEL0.reg |= TAL_EICCPUSEL0_EXTINT12_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_EICCPUSEL0_EXTINT12_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->EICCPUSEL0.reg;
	tmp = (tmp & TAL_EICCPUSEL0_EXTINT12_Msk) >> TAL_EICCPUSEL0_EXTINT12_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_EICCPUSEL0_EXTINT12_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->EICCPUSEL0.reg;
	tmp &= ~TAL_EICCPUSEL0_EXTINT12_Msk;
	tmp |= value << TAL_EICCPUSEL0_EXTINT12_Pos;
	((Tal *)hw)->EICCPUSEL0.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_EICCPUSEL0_EXTINT12_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EICCPUSEL0.reg &= ~TAL_EICCPUSEL0_EXTINT12_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_EICCPUSEL0_EXTINT12_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EICCPUSEL0.reg ^= TAL_EICCPUSEL0_EXTINT12_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_EICCPUSEL0_EXTINT13_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EICCPUSEL0.reg |= TAL_EICCPUSEL0_EXTINT13_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_EICCPUSEL0_EXTINT13_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->EICCPUSEL0.reg;
	tmp = (tmp & TAL_EICCPUSEL0_EXTINT13_Msk) >> TAL_EICCPUSEL0_EXTINT13_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_EICCPUSEL0_EXTINT13_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->EICCPUSEL0.reg;
	tmp &= ~TAL_EICCPUSEL0_EXTINT13_Msk;
	tmp |= value << TAL_EICCPUSEL0_EXTINT13_Pos;
	((Tal *)hw)->EICCPUSEL0.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_EICCPUSEL0_EXTINT13_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EICCPUSEL0.reg &= ~TAL_EICCPUSEL0_EXTINT13_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_EICCPUSEL0_EXTINT13_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EICCPUSEL0.reg ^= TAL_EICCPUSEL0_EXTINT13_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_EICCPUSEL0_EXTINT14_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EICCPUSEL0.reg |= TAL_EICCPUSEL0_EXTINT14_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_EICCPUSEL0_EXTINT14_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->EICCPUSEL0.reg;
	tmp = (tmp & TAL_EICCPUSEL0_EXTINT14_Msk) >> TAL_EICCPUSEL0_EXTINT14_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_EICCPUSEL0_EXTINT14_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->EICCPUSEL0.reg;
	tmp &= ~TAL_EICCPUSEL0_EXTINT14_Msk;
	tmp |= value << TAL_EICCPUSEL0_EXTINT14_Pos;
	((Tal *)hw)->EICCPUSEL0.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_EICCPUSEL0_EXTINT14_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EICCPUSEL0.reg &= ~TAL_EICCPUSEL0_EXTINT14_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_EICCPUSEL0_EXTINT14_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EICCPUSEL0.reg ^= TAL_EICCPUSEL0_EXTINT14_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_EICCPUSEL0_EXTINT15_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EICCPUSEL0.reg |= TAL_EICCPUSEL0_EXTINT15_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_EICCPUSEL0_EXTINT15_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->EICCPUSEL0.reg;
	tmp = (tmp & TAL_EICCPUSEL0_EXTINT15_Msk) >> TAL_EICCPUSEL0_EXTINT15_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_EICCPUSEL0_EXTINT15_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->EICCPUSEL0.reg;
	tmp &= ~TAL_EICCPUSEL0_EXTINT15_Msk;
	tmp |= value << TAL_EICCPUSEL0_EXTINT15_Pos;
	((Tal *)hw)->EICCPUSEL0.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_EICCPUSEL0_EXTINT15_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EICCPUSEL0.reg &= ~TAL_EICCPUSEL0_EXTINT15_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_EICCPUSEL0_EXTINT15_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EICCPUSEL0.reg ^= TAL_EICCPUSEL0_EXTINT15_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_EICCPUSEL0_reg(const void *const hw, hri_tal_eiccpusel0_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EICCPUSEL0.reg |= mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline hri_tal_eiccpusel0_reg_t hri_tal_get_EICCPUSEL0_reg(const void *const hw, hri_tal_eiccpusel0_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->EICCPUSEL0.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_tal_write_EICCPUSEL0_reg(const void *const hw, hri_tal_eiccpusel0_reg_t data)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EICCPUSEL0.reg = data;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_EICCPUSEL0_reg(const void *const hw, hri_tal_eiccpusel0_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EICCPUSEL0.reg &= ~mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_EICCPUSEL0_reg(const void *const hw, hri_tal_eiccpusel0_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->EICCPUSEL0.reg ^= mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline hri_tal_eiccpusel0_reg_t hri_tal_read_EICCPUSEL0_reg(const void *const hw)
{
	return ((Tal *)hw)->EICCPUSEL0.reg;
}

static inline void hri_tal_set_INTCPUSEL0_PAC_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL0.reg |= TAL_INTCPUSEL0_PAC_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_INTCPUSEL0_PAC_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->INTCPUSEL0.reg;
	tmp = (tmp & TAL_INTCPUSEL0_PAC_Msk) >> TAL_INTCPUSEL0_PAC_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_INTCPUSEL0_PAC_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->INTCPUSEL0.reg;
	tmp &= ~TAL_INTCPUSEL0_PAC_Msk;
	tmp |= value << TAL_INTCPUSEL0_PAC_Pos;
	((Tal *)hw)->INTCPUSEL0.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_INTCPUSEL0_PAC_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL0.reg &= ~TAL_INTCPUSEL0_PAC_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_INTCPUSEL0_PAC_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL0.reg ^= TAL_INTCPUSEL0_PAC_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_INTCPUSEL0_PM_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL0.reg |= TAL_INTCPUSEL0_PM_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_INTCPUSEL0_PM_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->INTCPUSEL0.reg;
	tmp = (tmp & TAL_INTCPUSEL0_PM_Msk) >> TAL_INTCPUSEL0_PM_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_INTCPUSEL0_PM_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->INTCPUSEL0.reg;
	tmp &= ~TAL_INTCPUSEL0_PM_Msk;
	tmp |= value << TAL_INTCPUSEL0_PM_Pos;
	((Tal *)hw)->INTCPUSEL0.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_INTCPUSEL0_PM_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL0.reg &= ~TAL_INTCPUSEL0_PM_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_INTCPUSEL0_PM_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL0.reg ^= TAL_INTCPUSEL0_PM_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_INTCPUSEL0_MCLK_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL0.reg |= TAL_INTCPUSEL0_MCLK_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_INTCPUSEL0_MCLK_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->INTCPUSEL0.reg;
	tmp = (tmp & TAL_INTCPUSEL0_MCLK_Msk) >> TAL_INTCPUSEL0_MCLK_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_INTCPUSEL0_MCLK_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->INTCPUSEL0.reg;
	tmp &= ~TAL_INTCPUSEL0_MCLK_Msk;
	tmp |= value << TAL_INTCPUSEL0_MCLK_Pos;
	((Tal *)hw)->INTCPUSEL0.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_INTCPUSEL0_MCLK_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL0.reg &= ~TAL_INTCPUSEL0_MCLK_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_INTCPUSEL0_MCLK_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL0.reg ^= TAL_INTCPUSEL0_MCLK_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_INTCPUSEL0_OSCCTRL_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL0.reg |= TAL_INTCPUSEL0_OSCCTRL_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_INTCPUSEL0_OSCCTRL_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->INTCPUSEL0.reg;
	tmp = (tmp & TAL_INTCPUSEL0_OSCCTRL_Msk) >> TAL_INTCPUSEL0_OSCCTRL_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_INTCPUSEL0_OSCCTRL_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->INTCPUSEL0.reg;
	tmp &= ~TAL_INTCPUSEL0_OSCCTRL_Msk;
	tmp |= value << TAL_INTCPUSEL0_OSCCTRL_Pos;
	((Tal *)hw)->INTCPUSEL0.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_INTCPUSEL0_OSCCTRL_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL0.reg &= ~TAL_INTCPUSEL0_OSCCTRL_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_INTCPUSEL0_OSCCTRL_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL0.reg ^= TAL_INTCPUSEL0_OSCCTRL_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_INTCPUSEL0_OSC32KCTRL_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL0.reg |= TAL_INTCPUSEL0_OSC32KCTRL_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_INTCPUSEL0_OSC32KCTRL_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->INTCPUSEL0.reg;
	tmp = (tmp & TAL_INTCPUSEL0_OSC32KCTRL_Msk) >> TAL_INTCPUSEL0_OSC32KCTRL_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_INTCPUSEL0_OSC32KCTRL_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->INTCPUSEL0.reg;
	tmp &= ~TAL_INTCPUSEL0_OSC32KCTRL_Msk;
	tmp |= value << TAL_INTCPUSEL0_OSC32KCTRL_Pos;
	((Tal *)hw)->INTCPUSEL0.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_INTCPUSEL0_OSC32KCTRL_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL0.reg &= ~TAL_INTCPUSEL0_OSC32KCTRL_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_INTCPUSEL0_OSC32KCTRL_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL0.reg ^= TAL_INTCPUSEL0_OSC32KCTRL_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_INTCPUSEL0_SUPC_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL0.reg |= TAL_INTCPUSEL0_SUPC_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_INTCPUSEL0_SUPC_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->INTCPUSEL0.reg;
	tmp = (tmp & TAL_INTCPUSEL0_SUPC_Msk) >> TAL_INTCPUSEL0_SUPC_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_INTCPUSEL0_SUPC_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->INTCPUSEL0.reg;
	tmp &= ~TAL_INTCPUSEL0_SUPC_Msk;
	tmp |= value << TAL_INTCPUSEL0_SUPC_Pos;
	((Tal *)hw)->INTCPUSEL0.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_INTCPUSEL0_SUPC_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL0.reg &= ~TAL_INTCPUSEL0_SUPC_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_INTCPUSEL0_SUPC_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL0.reg ^= TAL_INTCPUSEL0_SUPC_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_INTCPUSEL0_WDT_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL0.reg |= TAL_INTCPUSEL0_WDT_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_INTCPUSEL0_WDT_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->INTCPUSEL0.reg;
	tmp = (tmp & TAL_INTCPUSEL0_WDT_Msk) >> TAL_INTCPUSEL0_WDT_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_INTCPUSEL0_WDT_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->INTCPUSEL0.reg;
	tmp &= ~TAL_INTCPUSEL0_WDT_Msk;
	tmp |= value << TAL_INTCPUSEL0_WDT_Pos;
	((Tal *)hw)->INTCPUSEL0.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_INTCPUSEL0_WDT_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL0.reg &= ~TAL_INTCPUSEL0_WDT_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_INTCPUSEL0_WDT_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL0.reg ^= TAL_INTCPUSEL0_WDT_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_INTCPUSEL0_RTC_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL0.reg |= TAL_INTCPUSEL0_RTC_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_INTCPUSEL0_RTC_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->INTCPUSEL0.reg;
	tmp = (tmp & TAL_INTCPUSEL0_RTC_Msk) >> TAL_INTCPUSEL0_RTC_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_INTCPUSEL0_RTC_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->INTCPUSEL0.reg;
	tmp &= ~TAL_INTCPUSEL0_RTC_Msk;
	tmp |= value << TAL_INTCPUSEL0_RTC_Pos;
	((Tal *)hw)->INTCPUSEL0.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_INTCPUSEL0_RTC_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL0.reg &= ~TAL_INTCPUSEL0_RTC_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_INTCPUSEL0_RTC_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL0.reg ^= TAL_INTCPUSEL0_RTC_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_INTCPUSEL0_EIC_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL0.reg |= TAL_INTCPUSEL0_EIC_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_INTCPUSEL0_EIC_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->INTCPUSEL0.reg;
	tmp = (tmp & TAL_INTCPUSEL0_EIC_Msk) >> TAL_INTCPUSEL0_EIC_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_INTCPUSEL0_EIC_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->INTCPUSEL0.reg;
	tmp &= ~TAL_INTCPUSEL0_EIC_Msk;
	tmp |= value << TAL_INTCPUSEL0_EIC_Pos;
	((Tal *)hw)->INTCPUSEL0.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_INTCPUSEL0_EIC_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL0.reg &= ~TAL_INTCPUSEL0_EIC_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_INTCPUSEL0_EIC_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL0.reg ^= TAL_INTCPUSEL0_EIC_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_INTCPUSEL0_FREQM_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL0.reg |= TAL_INTCPUSEL0_FREQM_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_INTCPUSEL0_FREQM_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->INTCPUSEL0.reg;
	tmp = (tmp & TAL_INTCPUSEL0_FREQM_Msk) >> TAL_INTCPUSEL0_FREQM_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_INTCPUSEL0_FREQM_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->INTCPUSEL0.reg;
	tmp &= ~TAL_INTCPUSEL0_FREQM_Msk;
	tmp |= value << TAL_INTCPUSEL0_FREQM_Pos;
	((Tal *)hw)->INTCPUSEL0.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_INTCPUSEL0_FREQM_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL0.reg &= ~TAL_INTCPUSEL0_FREQM_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_INTCPUSEL0_FREQM_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL0.reg ^= TAL_INTCPUSEL0_FREQM_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_INTCPUSEL0_SERCOM0_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL0.reg |= TAL_INTCPUSEL0_SERCOM0_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_INTCPUSEL0_SERCOM0_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->INTCPUSEL0.reg;
	tmp = (tmp & TAL_INTCPUSEL0_SERCOM0_Msk) >> TAL_INTCPUSEL0_SERCOM0_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_INTCPUSEL0_SERCOM0_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->INTCPUSEL0.reg;
	tmp &= ~TAL_INTCPUSEL0_SERCOM0_Msk;
	tmp |= value << TAL_INTCPUSEL0_SERCOM0_Pos;
	((Tal *)hw)->INTCPUSEL0.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_INTCPUSEL0_SERCOM0_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL0.reg &= ~TAL_INTCPUSEL0_SERCOM0_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_INTCPUSEL0_SERCOM0_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL0.reg ^= TAL_INTCPUSEL0_SERCOM0_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_INTCPUSEL0_SERCOM1_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL0.reg |= TAL_INTCPUSEL0_SERCOM1_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_INTCPUSEL0_SERCOM1_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->INTCPUSEL0.reg;
	tmp = (tmp & TAL_INTCPUSEL0_SERCOM1_Msk) >> TAL_INTCPUSEL0_SERCOM1_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_INTCPUSEL0_SERCOM1_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->INTCPUSEL0.reg;
	tmp &= ~TAL_INTCPUSEL0_SERCOM1_Msk;
	tmp |= value << TAL_INTCPUSEL0_SERCOM1_Pos;
	((Tal *)hw)->INTCPUSEL0.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_INTCPUSEL0_SERCOM1_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL0.reg &= ~TAL_INTCPUSEL0_SERCOM1_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_INTCPUSEL0_SERCOM1_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL0.reg ^= TAL_INTCPUSEL0_SERCOM1_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_INTCPUSEL0_TC0_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL0.reg |= TAL_INTCPUSEL0_TC0_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_INTCPUSEL0_TC0_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->INTCPUSEL0.reg;
	tmp = (tmp & TAL_INTCPUSEL0_TC0_Msk) >> TAL_INTCPUSEL0_TC0_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_INTCPUSEL0_TC0_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->INTCPUSEL0.reg;
	tmp &= ~TAL_INTCPUSEL0_TC0_Msk;
	tmp |= value << TAL_INTCPUSEL0_TC0_Pos;
	((Tal *)hw)->INTCPUSEL0.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_INTCPUSEL0_TC0_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL0.reg &= ~TAL_INTCPUSEL0_TC0_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_INTCPUSEL0_TC0_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL0.reg ^= TAL_INTCPUSEL0_TC0_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_INTCPUSEL0_TC1_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL0.reg |= TAL_INTCPUSEL0_TC1_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_INTCPUSEL0_TC1_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->INTCPUSEL0.reg;
	tmp = (tmp & TAL_INTCPUSEL0_TC1_Msk) >> TAL_INTCPUSEL0_TC1_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_INTCPUSEL0_TC1_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->INTCPUSEL0.reg;
	tmp &= ~TAL_INTCPUSEL0_TC1_Msk;
	tmp |= value << TAL_INTCPUSEL0_TC1_Pos;
	((Tal *)hw)->INTCPUSEL0.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_INTCPUSEL0_TC1_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL0.reg &= ~TAL_INTCPUSEL0_TC1_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_INTCPUSEL0_TC1_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL0.reg ^= TAL_INTCPUSEL0_TC1_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_INTCPUSEL0_reg(const void *const hw, hri_tal_intcpusel0_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL0.reg |= mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline hri_tal_intcpusel0_reg_t hri_tal_get_INTCPUSEL0_reg(const void *const hw, hri_tal_intcpusel0_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->INTCPUSEL0.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_tal_write_INTCPUSEL0_reg(const void *const hw, hri_tal_intcpusel0_reg_t data)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL0.reg = data;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_INTCPUSEL0_reg(const void *const hw, hri_tal_intcpusel0_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL0.reg &= ~mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_INTCPUSEL0_reg(const void *const hw, hri_tal_intcpusel0_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL0.reg ^= mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline hri_tal_intcpusel0_reg_t hri_tal_read_INTCPUSEL0_reg(const void *const hw)
{
	return ((Tal *)hw)->INTCPUSEL0.reg;
}

static inline void hri_tal_set_INTCPUSEL1_reg(const void *const hw, hri_tal_intcpusel1_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL1.reg |= mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline hri_tal_intcpusel1_reg_t hri_tal_get_INTCPUSEL1_reg(const void *const hw, hri_tal_intcpusel1_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->INTCPUSEL1.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_tal_write_INTCPUSEL1_reg(const void *const hw, hri_tal_intcpusel1_reg_t data)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL1.reg = data;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_INTCPUSEL1_reg(const void *const hw, hri_tal_intcpusel1_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL1.reg &= ~mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_INTCPUSEL1_reg(const void *const hw, hri_tal_intcpusel1_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL1.reg ^= mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline hri_tal_intcpusel1_reg_t hri_tal_read_INTCPUSEL1_reg(const void *const hw)
{
	return ((Tal *)hw)->INTCPUSEL1.reg;
}

static inline void hri_tal_set_INTCPUSEL2_USB_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL2.reg |= TAL_INTCPUSEL2_USB_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_INTCPUSEL2_USB_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->INTCPUSEL2.reg;
	tmp = (tmp & TAL_INTCPUSEL2_USB_Msk) >> TAL_INTCPUSEL2_USB_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_INTCPUSEL2_USB_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->INTCPUSEL2.reg;
	tmp &= ~TAL_INTCPUSEL2_USB_Msk;
	tmp |= value << TAL_INTCPUSEL2_USB_Pos;
	((Tal *)hw)->INTCPUSEL2.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_INTCPUSEL2_USB_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL2.reg &= ~TAL_INTCPUSEL2_USB_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_INTCPUSEL2_USB_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL2.reg ^= TAL_INTCPUSEL2_USB_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_INTCPUSEL2_NVMCTRL_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL2.reg |= TAL_INTCPUSEL2_NVMCTRL_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_INTCPUSEL2_NVMCTRL_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->INTCPUSEL2.reg;
	tmp = (tmp & TAL_INTCPUSEL2_NVMCTRL_Msk) >> TAL_INTCPUSEL2_NVMCTRL_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_INTCPUSEL2_NVMCTRL_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->INTCPUSEL2.reg;
	tmp &= ~TAL_INTCPUSEL2_NVMCTRL_Msk;
	tmp |= value << TAL_INTCPUSEL2_NVMCTRL_Pos;
	((Tal *)hw)->INTCPUSEL2.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_INTCPUSEL2_NVMCTRL_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL2.reg &= ~TAL_INTCPUSEL2_NVMCTRL_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_INTCPUSEL2_NVMCTRL_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL2.reg ^= TAL_INTCPUSEL2_NVMCTRL_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_INTCPUSEL2_DMAC_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL2.reg |= TAL_INTCPUSEL2_DMAC_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_INTCPUSEL2_DMAC_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->INTCPUSEL2.reg;
	tmp = (tmp & TAL_INTCPUSEL2_DMAC_Msk) >> TAL_INTCPUSEL2_DMAC_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_INTCPUSEL2_DMAC_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->INTCPUSEL2.reg;
	tmp &= ~TAL_INTCPUSEL2_DMAC_Msk;
	tmp |= value << TAL_INTCPUSEL2_DMAC_Pos;
	((Tal *)hw)->INTCPUSEL2.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_INTCPUSEL2_DMAC_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL2.reg &= ~TAL_INTCPUSEL2_DMAC_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_INTCPUSEL2_DMAC_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL2.reg ^= TAL_INTCPUSEL2_DMAC_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_INTCPUSEL2_EVSYS_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL2.reg |= TAL_INTCPUSEL2_EVSYS_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_INTCPUSEL2_EVSYS_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->INTCPUSEL2.reg;
	tmp = (tmp & TAL_INTCPUSEL2_EVSYS_Msk) >> TAL_INTCPUSEL2_EVSYS_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_INTCPUSEL2_EVSYS_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->INTCPUSEL2.reg;
	tmp &= ~TAL_INTCPUSEL2_EVSYS_Msk;
	tmp |= value << TAL_INTCPUSEL2_EVSYS_Pos;
	((Tal *)hw)->INTCPUSEL2.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_INTCPUSEL2_EVSYS_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL2.reg &= ~TAL_INTCPUSEL2_EVSYS_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_INTCPUSEL2_EVSYS_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL2.reg ^= TAL_INTCPUSEL2_EVSYS_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_INTCPUSEL2_PICOP_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL2.reg |= TAL_INTCPUSEL2_PICOP_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_INTCPUSEL2_PICOP_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->INTCPUSEL2.reg;
	tmp = (tmp & TAL_INTCPUSEL2_PICOP_Msk) >> TAL_INTCPUSEL2_PICOP_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_INTCPUSEL2_PICOP_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->INTCPUSEL2.reg;
	tmp &= ~TAL_INTCPUSEL2_PICOP_Msk;
	tmp |= value << TAL_INTCPUSEL2_PICOP_Pos;
	((Tal *)hw)->INTCPUSEL2.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_INTCPUSEL2_PICOP_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL2.reg &= ~TAL_INTCPUSEL2_PICOP_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_INTCPUSEL2_PICOP_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL2.reg ^= TAL_INTCPUSEL2_PICOP_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_INTCPUSEL2_SERCOM2_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL2.reg |= TAL_INTCPUSEL2_SERCOM2_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_INTCPUSEL2_SERCOM2_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->INTCPUSEL2.reg;
	tmp = (tmp & TAL_INTCPUSEL2_SERCOM2_Msk) >> TAL_INTCPUSEL2_SERCOM2_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_INTCPUSEL2_SERCOM2_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->INTCPUSEL2.reg;
	tmp &= ~TAL_INTCPUSEL2_SERCOM2_Msk;
	tmp |= value << TAL_INTCPUSEL2_SERCOM2_Pos;
	((Tal *)hw)->INTCPUSEL2.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_INTCPUSEL2_SERCOM2_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL2.reg &= ~TAL_INTCPUSEL2_SERCOM2_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_INTCPUSEL2_SERCOM2_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL2.reg ^= TAL_INTCPUSEL2_SERCOM2_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_INTCPUSEL2_SERCOM3_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL2.reg |= TAL_INTCPUSEL2_SERCOM3_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_INTCPUSEL2_SERCOM3_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->INTCPUSEL2.reg;
	tmp = (tmp & TAL_INTCPUSEL2_SERCOM3_Msk) >> TAL_INTCPUSEL2_SERCOM3_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_INTCPUSEL2_SERCOM3_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->INTCPUSEL2.reg;
	tmp &= ~TAL_INTCPUSEL2_SERCOM3_Msk;
	tmp |= value << TAL_INTCPUSEL2_SERCOM3_Pos;
	((Tal *)hw)->INTCPUSEL2.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_INTCPUSEL2_SERCOM3_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL2.reg &= ~TAL_INTCPUSEL2_SERCOM3_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_INTCPUSEL2_SERCOM3_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL2.reg ^= TAL_INTCPUSEL2_SERCOM3_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_INTCPUSEL2_TCC0_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL2.reg |= TAL_INTCPUSEL2_TCC0_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_INTCPUSEL2_TCC0_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->INTCPUSEL2.reg;
	tmp = (tmp & TAL_INTCPUSEL2_TCC0_Msk) >> TAL_INTCPUSEL2_TCC0_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_INTCPUSEL2_TCC0_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->INTCPUSEL2.reg;
	tmp &= ~TAL_INTCPUSEL2_TCC0_Msk;
	tmp |= value << TAL_INTCPUSEL2_TCC0_Pos;
	((Tal *)hw)->INTCPUSEL2.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_INTCPUSEL2_TCC0_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL2.reg &= ~TAL_INTCPUSEL2_TCC0_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_INTCPUSEL2_TCC0_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL2.reg ^= TAL_INTCPUSEL2_TCC0_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_INTCPUSEL2_TCC1_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL2.reg |= TAL_INTCPUSEL2_TCC1_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_INTCPUSEL2_TCC1_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->INTCPUSEL2.reg;
	tmp = (tmp & TAL_INTCPUSEL2_TCC1_Msk) >> TAL_INTCPUSEL2_TCC1_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_INTCPUSEL2_TCC1_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->INTCPUSEL2.reg;
	tmp &= ~TAL_INTCPUSEL2_TCC1_Msk;
	tmp |= value << TAL_INTCPUSEL2_TCC1_Pos;
	((Tal *)hw)->INTCPUSEL2.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_INTCPUSEL2_TCC1_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL2.reg &= ~TAL_INTCPUSEL2_TCC1_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_INTCPUSEL2_TCC1_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL2.reg ^= TAL_INTCPUSEL2_TCC1_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_INTCPUSEL2_TC2_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL2.reg |= TAL_INTCPUSEL2_TC2_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_INTCPUSEL2_TC2_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->INTCPUSEL2.reg;
	tmp = (tmp & TAL_INTCPUSEL2_TC2_Msk) >> TAL_INTCPUSEL2_TC2_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_INTCPUSEL2_TC2_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->INTCPUSEL2.reg;
	tmp &= ~TAL_INTCPUSEL2_TC2_Msk;
	tmp |= value << TAL_INTCPUSEL2_TC2_Pos;
	((Tal *)hw)->INTCPUSEL2.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_INTCPUSEL2_TC2_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL2.reg &= ~TAL_INTCPUSEL2_TC2_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_INTCPUSEL2_TC2_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL2.reg ^= TAL_INTCPUSEL2_TC2_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_INTCPUSEL2_TC3_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL2.reg |= TAL_INTCPUSEL2_TC3_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_INTCPUSEL2_TC3_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->INTCPUSEL2.reg;
	tmp = (tmp & TAL_INTCPUSEL2_TC3_Msk) >> TAL_INTCPUSEL2_TC3_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_INTCPUSEL2_TC3_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->INTCPUSEL2.reg;
	tmp &= ~TAL_INTCPUSEL2_TC3_Msk;
	tmp |= value << TAL_INTCPUSEL2_TC3_Pos;
	((Tal *)hw)->INTCPUSEL2.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_INTCPUSEL2_TC3_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL2.reg &= ~TAL_INTCPUSEL2_TC3_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_INTCPUSEL2_TC3_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL2.reg ^= TAL_INTCPUSEL2_TC3_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_INTCPUSEL2_TAL_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL2.reg |= TAL_INTCPUSEL2_TAL_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_INTCPUSEL2_TAL_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->INTCPUSEL2.reg;
	tmp = (tmp & TAL_INTCPUSEL2_TAL_Msk) >> TAL_INTCPUSEL2_TAL_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_INTCPUSEL2_TAL_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->INTCPUSEL2.reg;
	tmp &= ~TAL_INTCPUSEL2_TAL_Msk;
	tmp |= value << TAL_INTCPUSEL2_TAL_Pos;
	((Tal *)hw)->INTCPUSEL2.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_INTCPUSEL2_TAL_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL2.reg &= ~TAL_INTCPUSEL2_TAL_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_INTCPUSEL2_TAL_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL2.reg ^= TAL_INTCPUSEL2_TAL_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_INTCPUSEL2_reg(const void *const hw, hri_tal_intcpusel2_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL2.reg |= mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline hri_tal_intcpusel2_reg_t hri_tal_get_INTCPUSEL2_reg(const void *const hw, hri_tal_intcpusel2_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->INTCPUSEL2.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_tal_write_INTCPUSEL2_reg(const void *const hw, hri_tal_intcpusel2_reg_t data)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL2.reg = data;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_INTCPUSEL2_reg(const void *const hw, hri_tal_intcpusel2_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL2.reg &= ~mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_INTCPUSEL2_reg(const void *const hw, hri_tal_intcpusel2_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL2.reg ^= mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline hri_tal_intcpusel2_reg_t hri_tal_read_INTCPUSEL2_reg(const void *const hw)
{
	return ((Tal *)hw)->INTCPUSEL2.reg;
}

static inline void hri_tal_set_INTCPUSEL3_RAMECC_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL3.reg |= TAL_INTCPUSEL3_RAMECC_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_INTCPUSEL3_RAMECC_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->INTCPUSEL3.reg;
	tmp = (tmp & TAL_INTCPUSEL3_RAMECC_Msk) >> TAL_INTCPUSEL3_RAMECC_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_INTCPUSEL3_RAMECC_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->INTCPUSEL3.reg;
	tmp &= ~TAL_INTCPUSEL3_RAMECC_Msk;
	tmp |= value << TAL_INTCPUSEL3_RAMECC_Pos;
	((Tal *)hw)->INTCPUSEL3.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_INTCPUSEL3_RAMECC_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL3.reg &= ~TAL_INTCPUSEL3_RAMECC_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_INTCPUSEL3_RAMECC_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL3.reg ^= TAL_INTCPUSEL3_RAMECC_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_INTCPUSEL3_reg(const void *const hw, hri_tal_intcpusel3_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL3.reg |= mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline hri_tal_intcpusel3_reg_t hri_tal_get_INTCPUSEL3_reg(const void *const hw, hri_tal_intcpusel3_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->INTCPUSEL3.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_tal_write_INTCPUSEL3_reg(const void *const hw, hri_tal_intcpusel3_reg_t data)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL3.reg = data;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_INTCPUSEL3_reg(const void *const hw, hri_tal_intcpusel3_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL3.reg &= ~mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_INTCPUSEL3_reg(const void *const hw, hri_tal_intcpusel3_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL3.reg ^= mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline hri_tal_intcpusel3_reg_t hri_tal_read_INTCPUSEL3_reg(const void *const hw)
{
	return ((Tal *)hw)->INTCPUSEL3.reg;
}

static inline void hri_tal_set_INTCPUSEL4_CAN0_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL4.reg |= TAL_INTCPUSEL4_CAN0_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_INTCPUSEL4_CAN0_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->INTCPUSEL4.reg;
	tmp = (tmp & TAL_INTCPUSEL4_CAN0_Msk) >> TAL_INTCPUSEL4_CAN0_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_INTCPUSEL4_CAN0_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->INTCPUSEL4.reg;
	tmp &= ~TAL_INTCPUSEL4_CAN0_Msk;
	tmp |= value << TAL_INTCPUSEL4_CAN0_Pos;
	((Tal *)hw)->INTCPUSEL4.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_INTCPUSEL4_CAN0_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL4.reg &= ~TAL_INTCPUSEL4_CAN0_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_INTCPUSEL4_CAN0_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL4.reg ^= TAL_INTCPUSEL4_CAN0_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_INTCPUSEL4_CAN1_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL4.reg |= TAL_INTCPUSEL4_CAN1_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_INTCPUSEL4_CAN1_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->INTCPUSEL4.reg;
	tmp = (tmp & TAL_INTCPUSEL4_CAN1_Msk) >> TAL_INTCPUSEL4_CAN1_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_INTCPUSEL4_CAN1_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->INTCPUSEL4.reg;
	tmp &= ~TAL_INTCPUSEL4_CAN1_Msk;
	tmp |= value << TAL_INTCPUSEL4_CAN1_Pos;
	((Tal *)hw)->INTCPUSEL4.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_INTCPUSEL4_CAN1_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL4.reg &= ~TAL_INTCPUSEL4_CAN1_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_INTCPUSEL4_CAN1_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL4.reg ^= TAL_INTCPUSEL4_CAN1_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_INTCPUSEL4_GMAC_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL4.reg |= TAL_INTCPUSEL4_GMAC_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_INTCPUSEL4_GMAC_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->INTCPUSEL4.reg;
	tmp = (tmp & TAL_INTCPUSEL4_GMAC_Msk) >> TAL_INTCPUSEL4_GMAC_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_INTCPUSEL4_GMAC_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->INTCPUSEL4.reg;
	tmp &= ~TAL_INTCPUSEL4_GMAC_Msk;
	tmp |= value << TAL_INTCPUSEL4_GMAC_Pos;
	((Tal *)hw)->INTCPUSEL4.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_INTCPUSEL4_GMAC_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL4.reg &= ~TAL_INTCPUSEL4_GMAC_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_INTCPUSEL4_GMAC_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL4.reg ^= TAL_INTCPUSEL4_GMAC_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_INTCPUSEL4_TCC2_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL4.reg |= TAL_INTCPUSEL4_TCC2_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_INTCPUSEL4_TCC2_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->INTCPUSEL4.reg;
	tmp = (tmp & TAL_INTCPUSEL4_TCC2_Msk) >> TAL_INTCPUSEL4_TCC2_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_INTCPUSEL4_TCC2_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->INTCPUSEL4.reg;
	tmp &= ~TAL_INTCPUSEL4_TCC2_Msk;
	tmp |= value << TAL_INTCPUSEL4_TCC2_Pos;
	((Tal *)hw)->INTCPUSEL4.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_INTCPUSEL4_TCC2_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL4.reg &= ~TAL_INTCPUSEL4_TCC2_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_INTCPUSEL4_TCC2_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL4.reg ^= TAL_INTCPUSEL4_TCC2_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_INTCPUSEL4_TCC3_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL4.reg |= TAL_INTCPUSEL4_TCC3_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_INTCPUSEL4_TCC3_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->INTCPUSEL4.reg;
	tmp = (tmp & TAL_INTCPUSEL4_TCC3_Msk) >> TAL_INTCPUSEL4_TCC3_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_INTCPUSEL4_TCC3_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->INTCPUSEL4.reg;
	tmp &= ~TAL_INTCPUSEL4_TCC3_Msk;
	tmp |= value << TAL_INTCPUSEL4_TCC3_Pos;
	((Tal *)hw)->INTCPUSEL4.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_INTCPUSEL4_TCC3_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL4.reg &= ~TAL_INTCPUSEL4_TCC3_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_INTCPUSEL4_TCC3_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL4.reg ^= TAL_INTCPUSEL4_TCC3_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_INTCPUSEL4_TC4_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL4.reg |= TAL_INTCPUSEL4_TC4_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_INTCPUSEL4_TC4_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->INTCPUSEL4.reg;
	tmp = (tmp & TAL_INTCPUSEL4_TC4_Msk) >> TAL_INTCPUSEL4_TC4_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_INTCPUSEL4_TC4_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->INTCPUSEL4.reg;
	tmp &= ~TAL_INTCPUSEL4_TC4_Msk;
	tmp |= value << TAL_INTCPUSEL4_TC4_Pos;
	((Tal *)hw)->INTCPUSEL4.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_INTCPUSEL4_TC4_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL4.reg &= ~TAL_INTCPUSEL4_TC4_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_INTCPUSEL4_TC4_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL4.reg ^= TAL_INTCPUSEL4_TC4_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_INTCPUSEL4_TC5_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL4.reg |= TAL_INTCPUSEL4_TC5_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_INTCPUSEL4_TC5_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->INTCPUSEL4.reg;
	tmp = (tmp & TAL_INTCPUSEL4_TC5_Msk) >> TAL_INTCPUSEL4_TC5_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_INTCPUSEL4_TC5_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->INTCPUSEL4.reg;
	tmp &= ~TAL_INTCPUSEL4_TC5_Msk;
	tmp |= value << TAL_INTCPUSEL4_TC5_Pos;
	((Tal *)hw)->INTCPUSEL4.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_INTCPUSEL4_TC5_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL4.reg &= ~TAL_INTCPUSEL4_TC5_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_INTCPUSEL4_TC5_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL4.reg ^= TAL_INTCPUSEL4_TC5_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_INTCPUSEL4_PDEC_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL4.reg |= TAL_INTCPUSEL4_PDEC_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_INTCPUSEL4_PDEC_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->INTCPUSEL4.reg;
	tmp = (tmp & TAL_INTCPUSEL4_PDEC_Msk) >> TAL_INTCPUSEL4_PDEC_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_INTCPUSEL4_PDEC_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->INTCPUSEL4.reg;
	tmp &= ~TAL_INTCPUSEL4_PDEC_Msk;
	tmp |= value << TAL_INTCPUSEL4_PDEC_Pos;
	((Tal *)hw)->INTCPUSEL4.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_INTCPUSEL4_PDEC_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL4.reg &= ~TAL_INTCPUSEL4_PDEC_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_INTCPUSEL4_PDEC_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL4.reg ^= TAL_INTCPUSEL4_PDEC_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_INTCPUSEL4_AC_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL4.reg |= TAL_INTCPUSEL4_AC_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_INTCPUSEL4_AC_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->INTCPUSEL4.reg;
	tmp = (tmp & TAL_INTCPUSEL4_AC_Msk) >> TAL_INTCPUSEL4_AC_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_INTCPUSEL4_AC_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->INTCPUSEL4.reg;
	tmp &= ~TAL_INTCPUSEL4_AC_Msk;
	tmp |= value << TAL_INTCPUSEL4_AC_Pos;
	((Tal *)hw)->INTCPUSEL4.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_INTCPUSEL4_AC_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL4.reg &= ~TAL_INTCPUSEL4_AC_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_INTCPUSEL4_AC_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL4.reg ^= TAL_INTCPUSEL4_AC_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_INTCPUSEL4_AES_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL4.reg |= TAL_INTCPUSEL4_AES_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_INTCPUSEL4_AES_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->INTCPUSEL4.reg;
	tmp = (tmp & TAL_INTCPUSEL4_AES_Msk) >> TAL_INTCPUSEL4_AES_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_INTCPUSEL4_AES_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->INTCPUSEL4.reg;
	tmp &= ~TAL_INTCPUSEL4_AES_Msk;
	tmp |= value << TAL_INTCPUSEL4_AES_Pos;
	((Tal *)hw)->INTCPUSEL4.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_INTCPUSEL4_AES_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL4.reg &= ~TAL_INTCPUSEL4_AES_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_INTCPUSEL4_AES_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL4.reg ^= TAL_INTCPUSEL4_AES_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_INTCPUSEL4_TRNG_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL4.reg |= TAL_INTCPUSEL4_TRNG_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_INTCPUSEL4_TRNG_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->INTCPUSEL4.reg;
	tmp = (tmp & TAL_INTCPUSEL4_TRNG_Msk) >> TAL_INTCPUSEL4_TRNG_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_INTCPUSEL4_TRNG_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->INTCPUSEL4.reg;
	tmp &= ~TAL_INTCPUSEL4_TRNG_Msk;
	tmp |= value << TAL_INTCPUSEL4_TRNG_Pos;
	((Tal *)hw)->INTCPUSEL4.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_INTCPUSEL4_TRNG_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL4.reg &= ~TAL_INTCPUSEL4_TRNG_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_INTCPUSEL4_TRNG_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL4.reg ^= TAL_INTCPUSEL4_TRNG_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_INTCPUSEL4_ICM_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL4.reg |= TAL_INTCPUSEL4_ICM_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_INTCPUSEL4_ICM_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->INTCPUSEL4.reg;
	tmp = (tmp & TAL_INTCPUSEL4_ICM_Msk) >> TAL_INTCPUSEL4_ICM_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_INTCPUSEL4_ICM_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->INTCPUSEL4.reg;
	tmp &= ~TAL_INTCPUSEL4_ICM_Msk;
	tmp |= value << TAL_INTCPUSEL4_ICM_Pos;
	((Tal *)hw)->INTCPUSEL4.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_INTCPUSEL4_ICM_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL4.reg &= ~TAL_INTCPUSEL4_ICM_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_INTCPUSEL4_ICM_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL4.reg ^= TAL_INTCPUSEL4_ICM_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_INTCPUSEL4_PUKCC_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL4.reg |= TAL_INTCPUSEL4_PUKCC_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_INTCPUSEL4_PUKCC_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->INTCPUSEL4.reg;
	tmp = (tmp & TAL_INTCPUSEL4_PUKCC_Msk) >> TAL_INTCPUSEL4_PUKCC_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_INTCPUSEL4_PUKCC_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->INTCPUSEL4.reg;
	tmp &= ~TAL_INTCPUSEL4_PUKCC_Msk;
	tmp |= value << TAL_INTCPUSEL4_PUKCC_Pos;
	((Tal *)hw)->INTCPUSEL4.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_INTCPUSEL4_PUKCC_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL4.reg &= ~TAL_INTCPUSEL4_PUKCC_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_INTCPUSEL4_PUKCC_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL4.reg ^= TAL_INTCPUSEL4_PUKCC_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_INTCPUSEL4_QSPI_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL4.reg |= TAL_INTCPUSEL4_QSPI_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_INTCPUSEL4_QSPI_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->INTCPUSEL4.reg;
	tmp = (tmp & TAL_INTCPUSEL4_QSPI_Msk) >> TAL_INTCPUSEL4_QSPI_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_INTCPUSEL4_QSPI_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->INTCPUSEL4.reg;
	tmp &= ~TAL_INTCPUSEL4_QSPI_Msk;
	tmp |= value << TAL_INTCPUSEL4_QSPI_Pos;
	((Tal *)hw)->INTCPUSEL4.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_INTCPUSEL4_QSPI_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL4.reg &= ~TAL_INTCPUSEL4_QSPI_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_INTCPUSEL4_QSPI_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL4.reg ^= TAL_INTCPUSEL4_QSPI_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_INTCPUSEL4_reg(const void *const hw, hri_tal_intcpusel4_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL4.reg |= mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline hri_tal_intcpusel4_reg_t hri_tal_get_INTCPUSEL4_reg(const void *const hw, hri_tal_intcpusel4_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->INTCPUSEL4.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_tal_write_INTCPUSEL4_reg(const void *const hw, hri_tal_intcpusel4_reg_t data)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL4.reg = data;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_INTCPUSEL4_reg(const void *const hw, hri_tal_intcpusel4_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL4.reg &= ~mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_INTCPUSEL4_reg(const void *const hw, hri_tal_intcpusel4_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL4.reg ^= mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline hri_tal_intcpusel4_reg_t hri_tal_read_INTCPUSEL4_reg(const void *const hw)
{
	return ((Tal *)hw)->INTCPUSEL4.reg;
}

static inline void hri_tal_set_INTCPUSEL5_reg(const void *const hw, hri_tal_intcpusel5_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL5.reg |= mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline hri_tal_intcpusel5_reg_t hri_tal_get_INTCPUSEL5_reg(const void *const hw, hri_tal_intcpusel5_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->INTCPUSEL5.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_tal_write_INTCPUSEL5_reg(const void *const hw, hri_tal_intcpusel5_reg_t data)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL5.reg = data;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_INTCPUSEL5_reg(const void *const hw, hri_tal_intcpusel5_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL5.reg &= ~mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_INTCPUSEL5_reg(const void *const hw, hri_tal_intcpusel5_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL5.reg ^= mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline hri_tal_intcpusel5_reg_t hri_tal_read_INTCPUSEL5_reg(const void *const hw)
{
	return ((Tal *)hw)->INTCPUSEL5.reg;
}

static inline void hri_tal_set_INTCPUSEL6_SERCOM4_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL6.reg |= TAL_INTCPUSEL6_SERCOM4_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_INTCPUSEL6_SERCOM4_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->INTCPUSEL6.reg;
	tmp = (tmp & TAL_INTCPUSEL6_SERCOM4_Msk) >> TAL_INTCPUSEL6_SERCOM4_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_INTCPUSEL6_SERCOM4_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->INTCPUSEL6.reg;
	tmp &= ~TAL_INTCPUSEL6_SERCOM4_Msk;
	tmp |= value << TAL_INTCPUSEL6_SERCOM4_Pos;
	((Tal *)hw)->INTCPUSEL6.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_INTCPUSEL6_SERCOM4_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL6.reg &= ~TAL_INTCPUSEL6_SERCOM4_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_INTCPUSEL6_SERCOM4_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL6.reg ^= TAL_INTCPUSEL6_SERCOM4_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_INTCPUSEL6_SERCOM5_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL6.reg |= TAL_INTCPUSEL6_SERCOM5_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_INTCPUSEL6_SERCOM5_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->INTCPUSEL6.reg;
	tmp = (tmp & TAL_INTCPUSEL6_SERCOM5_Msk) >> TAL_INTCPUSEL6_SERCOM5_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_INTCPUSEL6_SERCOM5_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->INTCPUSEL6.reg;
	tmp &= ~TAL_INTCPUSEL6_SERCOM5_Msk;
	tmp |= value << TAL_INTCPUSEL6_SERCOM5_Pos;
	((Tal *)hw)->INTCPUSEL6.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_INTCPUSEL6_SERCOM5_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL6.reg &= ~TAL_INTCPUSEL6_SERCOM5_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_INTCPUSEL6_SERCOM5_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL6.reg ^= TAL_INTCPUSEL6_SERCOM5_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_INTCPUSEL6_SERCOM6_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL6.reg |= TAL_INTCPUSEL6_SERCOM6_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_INTCPUSEL6_SERCOM6_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->INTCPUSEL6.reg;
	tmp = (tmp & TAL_INTCPUSEL6_SERCOM6_Msk) >> TAL_INTCPUSEL6_SERCOM6_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_INTCPUSEL6_SERCOM6_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->INTCPUSEL6.reg;
	tmp &= ~TAL_INTCPUSEL6_SERCOM6_Msk;
	tmp |= value << TAL_INTCPUSEL6_SERCOM6_Pos;
	((Tal *)hw)->INTCPUSEL6.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_INTCPUSEL6_SERCOM6_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL6.reg &= ~TAL_INTCPUSEL6_SERCOM6_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_INTCPUSEL6_SERCOM6_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL6.reg ^= TAL_INTCPUSEL6_SERCOM6_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_INTCPUSEL6_SERCOM7_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL6.reg |= TAL_INTCPUSEL6_SERCOM7_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_INTCPUSEL6_SERCOM7_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->INTCPUSEL6.reg;
	tmp = (tmp & TAL_INTCPUSEL6_SERCOM7_Msk) >> TAL_INTCPUSEL6_SERCOM7_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_INTCPUSEL6_SERCOM7_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->INTCPUSEL6.reg;
	tmp &= ~TAL_INTCPUSEL6_SERCOM7_Msk;
	tmp |= value << TAL_INTCPUSEL6_SERCOM7_Pos;
	((Tal *)hw)->INTCPUSEL6.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_INTCPUSEL6_SERCOM7_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL6.reg &= ~TAL_INTCPUSEL6_SERCOM7_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_INTCPUSEL6_SERCOM7_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL6.reg ^= TAL_INTCPUSEL6_SERCOM7_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_INTCPUSEL6_TCC4_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL6.reg |= TAL_INTCPUSEL6_TCC4_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_INTCPUSEL6_TCC4_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->INTCPUSEL6.reg;
	tmp = (tmp & TAL_INTCPUSEL6_TCC4_Msk) >> TAL_INTCPUSEL6_TCC4_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_INTCPUSEL6_TCC4_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->INTCPUSEL6.reg;
	tmp &= ~TAL_INTCPUSEL6_TCC4_Msk;
	tmp |= value << TAL_INTCPUSEL6_TCC4_Pos;
	((Tal *)hw)->INTCPUSEL6.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_INTCPUSEL6_TCC4_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL6.reg &= ~TAL_INTCPUSEL6_TCC4_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_INTCPUSEL6_TCC4_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL6.reg ^= TAL_INTCPUSEL6_TCC4_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_INTCPUSEL6_TC6_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL6.reg |= TAL_INTCPUSEL6_TC6_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_INTCPUSEL6_TC6_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->INTCPUSEL6.reg;
	tmp = (tmp & TAL_INTCPUSEL6_TC6_Msk) >> TAL_INTCPUSEL6_TC6_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_INTCPUSEL6_TC6_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->INTCPUSEL6.reg;
	tmp &= ~TAL_INTCPUSEL6_TC6_Msk;
	tmp |= value << TAL_INTCPUSEL6_TC6_Pos;
	((Tal *)hw)->INTCPUSEL6.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_INTCPUSEL6_TC6_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL6.reg &= ~TAL_INTCPUSEL6_TC6_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_INTCPUSEL6_TC6_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL6.reg ^= TAL_INTCPUSEL6_TC6_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_INTCPUSEL6_TC7_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL6.reg |= TAL_INTCPUSEL6_TC7_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_INTCPUSEL6_TC7_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->INTCPUSEL6.reg;
	tmp = (tmp & TAL_INTCPUSEL6_TC7_Msk) >> TAL_INTCPUSEL6_TC7_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_INTCPUSEL6_TC7_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->INTCPUSEL6.reg;
	tmp &= ~TAL_INTCPUSEL6_TC7_Msk;
	tmp |= value << TAL_INTCPUSEL6_TC7_Pos;
	((Tal *)hw)->INTCPUSEL6.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_INTCPUSEL6_TC7_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL6.reg &= ~TAL_INTCPUSEL6_TC7_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_INTCPUSEL6_TC7_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL6.reg ^= TAL_INTCPUSEL6_TC7_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_INTCPUSEL6_ADC0_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL6.reg |= TAL_INTCPUSEL6_ADC0_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_INTCPUSEL6_ADC0_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->INTCPUSEL6.reg;
	tmp = (tmp & TAL_INTCPUSEL6_ADC0_Msk) >> TAL_INTCPUSEL6_ADC0_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_INTCPUSEL6_ADC0_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->INTCPUSEL6.reg;
	tmp &= ~TAL_INTCPUSEL6_ADC0_Msk;
	tmp |= value << TAL_INTCPUSEL6_ADC0_Pos;
	((Tal *)hw)->INTCPUSEL6.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_INTCPUSEL6_ADC0_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL6.reg &= ~TAL_INTCPUSEL6_ADC0_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_INTCPUSEL6_ADC0_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL6.reg ^= TAL_INTCPUSEL6_ADC0_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_INTCPUSEL6_ADC1_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL6.reg |= TAL_INTCPUSEL6_ADC1_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_INTCPUSEL6_ADC1_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->INTCPUSEL6.reg;
	tmp = (tmp & TAL_INTCPUSEL6_ADC1_Msk) >> TAL_INTCPUSEL6_ADC1_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_INTCPUSEL6_ADC1_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->INTCPUSEL6.reg;
	tmp &= ~TAL_INTCPUSEL6_ADC1_Msk;
	tmp |= value << TAL_INTCPUSEL6_ADC1_Pos;
	((Tal *)hw)->INTCPUSEL6.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_INTCPUSEL6_ADC1_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL6.reg &= ~TAL_INTCPUSEL6_ADC1_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_INTCPUSEL6_ADC1_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL6.reg ^= TAL_INTCPUSEL6_ADC1_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_INTCPUSEL6_DAC_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL6.reg |= TAL_INTCPUSEL6_DAC_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_INTCPUSEL6_DAC_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->INTCPUSEL6.reg;
	tmp = (tmp & TAL_INTCPUSEL6_DAC_Msk) >> TAL_INTCPUSEL6_DAC_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_INTCPUSEL6_DAC_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->INTCPUSEL6.reg;
	tmp &= ~TAL_INTCPUSEL6_DAC_Msk;
	tmp |= value << TAL_INTCPUSEL6_DAC_Pos;
	((Tal *)hw)->INTCPUSEL6.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_INTCPUSEL6_DAC_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL6.reg &= ~TAL_INTCPUSEL6_DAC_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_INTCPUSEL6_DAC_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL6.reg ^= TAL_INTCPUSEL6_DAC_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_INTCPUSEL6_I2S_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL6.reg |= TAL_INTCPUSEL6_I2S_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_INTCPUSEL6_I2S_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->INTCPUSEL6.reg;
	tmp = (tmp & TAL_INTCPUSEL6_I2S_Msk) >> TAL_INTCPUSEL6_I2S_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_INTCPUSEL6_I2S_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->INTCPUSEL6.reg;
	tmp &= ~TAL_INTCPUSEL6_I2S_Msk;
	tmp |= value << TAL_INTCPUSEL6_I2S_Pos;
	((Tal *)hw)->INTCPUSEL6.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_INTCPUSEL6_I2S_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL6.reg &= ~TAL_INTCPUSEL6_I2S_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_INTCPUSEL6_I2S_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL6.reg ^= TAL_INTCPUSEL6_I2S_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_INTCPUSEL6_PCC_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL6.reg |= TAL_INTCPUSEL6_PCC_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_INTCPUSEL6_PCC_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->INTCPUSEL6.reg;
	tmp = (tmp & TAL_INTCPUSEL6_PCC_Msk) >> TAL_INTCPUSEL6_PCC_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_INTCPUSEL6_PCC_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->INTCPUSEL6.reg;
	tmp &= ~TAL_INTCPUSEL6_PCC_Msk;
	tmp |= value << TAL_INTCPUSEL6_PCC_Pos;
	((Tal *)hw)->INTCPUSEL6.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_INTCPUSEL6_PCC_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL6.reg &= ~TAL_INTCPUSEL6_PCC_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_INTCPUSEL6_PCC_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL6.reg ^= TAL_INTCPUSEL6_PCC_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_INTCPUSEL6_reg(const void *const hw, hri_tal_intcpusel6_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL6.reg |= mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline hri_tal_intcpusel6_reg_t hri_tal_get_INTCPUSEL6_reg(const void *const hw, hri_tal_intcpusel6_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->INTCPUSEL6.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_tal_write_INTCPUSEL6_reg(const void *const hw, hri_tal_intcpusel6_reg_t data)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL6.reg = data;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_INTCPUSEL6_reg(const void *const hw, hri_tal_intcpusel6_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL6.reg &= ~mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_INTCPUSEL6_reg(const void *const hw, hri_tal_intcpusel6_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL6.reg ^= mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline hri_tal_intcpusel6_reg_t hri_tal_read_INTCPUSEL6_reg(const void *const hw)
{
	return ((Tal *)hw)->INTCPUSEL6.reg;
}

static inline void hri_tal_set_INTCPUSEL7_reg(const void *const hw, hri_tal_intcpusel7_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL7.reg |= mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline hri_tal_intcpusel7_reg_t hri_tal_get_INTCPUSEL7_reg(const void *const hw, hri_tal_intcpusel7_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->INTCPUSEL7.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_tal_write_INTCPUSEL7_reg(const void *const hw, hri_tal_intcpusel7_reg_t data)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL7.reg = data;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_INTCPUSEL7_reg(const void *const hw, hri_tal_intcpusel7_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL7.reg &= ~mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_INTCPUSEL7_reg(const void *const hw, hri_tal_intcpusel7_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL7.reg ^= mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline hri_tal_intcpusel7_reg_t hri_tal_read_INTCPUSEL7_reg(const void *const hw)
{
	return ((Tal *)hw)->INTCPUSEL7.reg;
}

static inline void hri_tal_set_INTCPUSEL8_SDHC0_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL8.reg |= TAL_INTCPUSEL8_SDHC0_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_INTCPUSEL8_SDHC0_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->INTCPUSEL8.reg;
	tmp = (tmp & TAL_INTCPUSEL8_SDHC0_Msk) >> TAL_INTCPUSEL8_SDHC0_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_INTCPUSEL8_SDHC0_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->INTCPUSEL8.reg;
	tmp &= ~TAL_INTCPUSEL8_SDHC0_Msk;
	tmp |= value << TAL_INTCPUSEL8_SDHC0_Pos;
	((Tal *)hw)->INTCPUSEL8.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_INTCPUSEL8_SDHC0_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL8.reg &= ~TAL_INTCPUSEL8_SDHC0_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_INTCPUSEL8_SDHC0_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL8.reg ^= TAL_INTCPUSEL8_SDHC0_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_INTCPUSEL8_SDHC1_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL8.reg |= TAL_INTCPUSEL8_SDHC1_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_INTCPUSEL8_SDHC1_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->INTCPUSEL8.reg;
	tmp = (tmp & TAL_INTCPUSEL8_SDHC1_Msk) >> TAL_INTCPUSEL8_SDHC1_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_INTCPUSEL8_SDHC1_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->INTCPUSEL8.reg;
	tmp &= ~TAL_INTCPUSEL8_SDHC1_Msk;
	tmp |= value << TAL_INTCPUSEL8_SDHC1_Pos;
	((Tal *)hw)->INTCPUSEL8.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_INTCPUSEL8_SDHC1_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL8.reg &= ~TAL_INTCPUSEL8_SDHC1_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_INTCPUSEL8_SDHC1_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL8.reg ^= TAL_INTCPUSEL8_SDHC1_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_INTCPUSEL8_reg(const void *const hw, hri_tal_intcpusel8_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL8.reg |= mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline hri_tal_intcpusel8_reg_t hri_tal_get_INTCPUSEL8_reg(const void *const hw, hri_tal_intcpusel8_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->INTCPUSEL8.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_tal_write_INTCPUSEL8_reg(const void *const hw, hri_tal_intcpusel8_reg_t data)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL8.reg = data;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_INTCPUSEL8_reg(const void *const hw, hri_tal_intcpusel8_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL8.reg &= ~mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_INTCPUSEL8_reg(const void *const hw, hri_tal_intcpusel8_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->INTCPUSEL8.reg ^= mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline hri_tal_intcpusel8_reg_t hri_tal_read_INTCPUSEL8_reg(const void *const hw)
{
	return ((Tal *)hw)->INTCPUSEL8.reg;
}

static inline void hri_tal_set_IRQTRIG_ENABLE_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->IRQTRIG.reg |= TAL_IRQTRIG_ENABLE;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_IRQTRIG_ENABLE_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->IRQTRIG.reg;
	tmp = (tmp & TAL_IRQTRIG_ENABLE) >> TAL_IRQTRIG_ENABLE_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_IRQTRIG_ENABLE_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->IRQTRIG.reg;
	tmp &= ~TAL_IRQTRIG_ENABLE;
	tmp |= value << TAL_IRQTRIG_ENABLE_Pos;
	((Tal *)hw)->IRQTRIG.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_IRQTRIG_ENABLE_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->IRQTRIG.reg &= ~TAL_IRQTRIG_ENABLE;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_IRQTRIG_ENABLE_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->IRQTRIG.reg ^= TAL_IRQTRIG_ENABLE;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_IRQTRIG_IRQNUM_bf(const void *const hw, hri_tal_irqtrig_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->IRQTRIG.reg |= TAL_IRQTRIG_IRQNUM(mask);
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline hri_tal_irqtrig_reg_t hri_tal_get_IRQTRIG_IRQNUM_bf(const void *const hw, hri_tal_irqtrig_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->IRQTRIG.reg;
	tmp = (tmp & TAL_IRQTRIG_IRQNUM(mask)) >> TAL_IRQTRIG_IRQNUM_Pos;
	return tmp;
}

static inline void hri_tal_write_IRQTRIG_IRQNUM_bf(const void *const hw, hri_tal_irqtrig_reg_t data)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->IRQTRIG.reg;
	tmp &= ~TAL_IRQTRIG_IRQNUM_Msk;
	tmp |= TAL_IRQTRIG_IRQNUM(data);
	((Tal *)hw)->IRQTRIG.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_IRQTRIG_IRQNUM_bf(const void *const hw, hri_tal_irqtrig_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->IRQTRIG.reg &= ~TAL_IRQTRIG_IRQNUM(mask);
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_IRQTRIG_IRQNUM_bf(const void *const hw, hri_tal_irqtrig_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->IRQTRIG.reg ^= TAL_IRQTRIG_IRQNUM(mask);
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline hri_tal_irqtrig_reg_t hri_tal_read_IRQTRIG_IRQNUM_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->IRQTRIG.reg;
	tmp = (tmp & TAL_IRQTRIG_IRQNUM_Msk) >> TAL_IRQTRIG_IRQNUM_Pos;
	return tmp;
}

static inline void hri_tal_set_IRQTRIG_OVERRIDE_bf(const void *const hw, hri_tal_irqtrig_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->IRQTRIG.reg |= TAL_IRQTRIG_OVERRIDE(mask);
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline hri_tal_irqtrig_reg_t hri_tal_get_IRQTRIG_OVERRIDE_bf(const void *const hw, hri_tal_irqtrig_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->IRQTRIG.reg;
	tmp = (tmp & TAL_IRQTRIG_OVERRIDE(mask)) >> TAL_IRQTRIG_OVERRIDE_Pos;
	return tmp;
}

static inline void hri_tal_write_IRQTRIG_OVERRIDE_bf(const void *const hw, hri_tal_irqtrig_reg_t data)
{
	uint32_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->IRQTRIG.reg;
	tmp &= ~TAL_IRQTRIG_OVERRIDE_Msk;
	tmp |= TAL_IRQTRIG_OVERRIDE(data);
	((Tal *)hw)->IRQTRIG.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_IRQTRIG_OVERRIDE_bf(const void *const hw, hri_tal_irqtrig_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->IRQTRIG.reg &= ~TAL_IRQTRIG_OVERRIDE(mask);
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_IRQTRIG_OVERRIDE_bf(const void *const hw, hri_tal_irqtrig_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->IRQTRIG.reg ^= TAL_IRQTRIG_OVERRIDE(mask);
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline hri_tal_irqtrig_reg_t hri_tal_read_IRQTRIG_OVERRIDE_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->IRQTRIG.reg;
	tmp = (tmp & TAL_IRQTRIG_OVERRIDE_Msk) >> TAL_IRQTRIG_OVERRIDE_Pos;
	return tmp;
}

static inline void hri_tal_set_IRQTRIG_reg(const void *const hw, hri_tal_irqtrig_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->IRQTRIG.reg |= mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline hri_tal_irqtrig_reg_t hri_tal_get_IRQTRIG_reg(const void *const hw, hri_tal_irqtrig_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->IRQTRIG.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_tal_write_IRQTRIG_reg(const void *const hw, hri_tal_irqtrig_reg_t data)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->IRQTRIG.reg = data;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_IRQTRIG_reg(const void *const hw, hri_tal_irqtrig_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->IRQTRIG.reg &= ~mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_IRQTRIG_reg(const void *const hw, hri_tal_irqtrig_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->IRQTRIG.reg ^= mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline hri_tal_irqtrig_reg_t hri_tal_read_IRQTRIG_reg(const void *const hw)
{
	return ((Tal *)hw)->IRQTRIG.reg;
}

static inline void hri_tal_set_IRQMON_EXTEND_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->IRQMON[index].reg |= TAL_IRQMON_EXTEND;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_IRQMON_EXTEND_bit(const void *const hw, uint8_t index)
{
	uint16_t tmp;
	tmp = ((Tal *)hw)->IRQMON[index].reg;
	tmp = (tmp & TAL_IRQMON_EXTEND) >> TAL_IRQMON_EXTEND_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_IRQMON_EXTEND_bit(const void *const hw, uint8_t index, bool value)
{
	uint16_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->IRQMON[index].reg;
	tmp &= ~TAL_IRQMON_EXTEND;
	tmp |= value << TAL_IRQMON_EXTEND_Pos;
	((Tal *)hw)->IRQMON[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_IRQMON_EXTEND_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->IRQMON[index].reg &= ~TAL_IRQMON_EXTEND;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_IRQMON_EXTEND_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->IRQMON[index].reg ^= TAL_IRQMON_EXTEND;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_IRQMON_DROP_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->IRQMON[index].reg |= TAL_IRQMON_DROP;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_IRQMON_DROP_bit(const void *const hw, uint8_t index)
{
	uint16_t tmp;
	tmp = ((Tal *)hw)->IRQMON[index].reg;
	tmp = (tmp & TAL_IRQMON_DROP) >> TAL_IRQMON_DROP_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_IRQMON_DROP_bit(const void *const hw, uint8_t index, bool value)
{
	uint16_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->IRQMON[index].reg;
	tmp &= ~TAL_IRQMON_DROP;
	tmp |= value << TAL_IRQMON_DROP_Pos;
	((Tal *)hw)->IRQMON[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_IRQMON_DROP_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->IRQMON[index].reg &= ~TAL_IRQMON_DROP;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_IRQMON_DROP_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->IRQMON[index].reg ^= TAL_IRQMON_DROP;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_IRQMON_CPUID_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->IRQMON[index].reg |= TAL_IRQMON_CPUID_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_IRQMON_CPUID_bit(const void *const hw, uint8_t index)
{
	uint16_t tmp;
	tmp = ((Tal *)hw)->IRQMON[index].reg;
	tmp = (tmp & TAL_IRQMON_CPUID_Msk) >> TAL_IRQMON_CPUID_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_IRQMON_CPUID_bit(const void *const hw, uint8_t index, bool value)
{
	uint16_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->IRQMON[index].reg;
	tmp &= ~TAL_IRQMON_CPUID_Msk;
	tmp |= value << TAL_IRQMON_CPUID_Pos;
	((Tal *)hw)->IRQMON[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_IRQMON_CPUID_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->IRQMON[index].reg &= ~TAL_IRQMON_CPUID_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_IRQMON_CPUID_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->IRQMON[index].reg ^= TAL_IRQMON_CPUID_Msk;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_IRQMON_IRQNUM_bf(const void *const hw, uint8_t index, hri_tal_irqmon_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->IRQMON[index].reg |= TAL_IRQMON_IRQNUM(mask);
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline hri_tal_irqmon_reg_t hri_tal_get_IRQMON_IRQNUM_bf(const void *const hw, uint8_t index,
                                                                hri_tal_irqmon_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Tal *)hw)->IRQMON[index].reg;
	tmp = (tmp & TAL_IRQMON_IRQNUM(mask)) >> TAL_IRQMON_IRQNUM_Pos;
	return tmp;
}

static inline void hri_tal_write_IRQMON_IRQNUM_bf(const void *const hw, uint8_t index, hri_tal_irqmon_reg_t data)
{
	uint16_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->IRQMON[index].reg;
	tmp &= ~TAL_IRQMON_IRQNUM_Msk;
	tmp |= TAL_IRQMON_IRQNUM(data);
	((Tal *)hw)->IRQMON[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_IRQMON_IRQNUM_bf(const void *const hw, uint8_t index, hri_tal_irqmon_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->IRQMON[index].reg &= ~TAL_IRQMON_IRQNUM(mask);
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_IRQMON_IRQNUM_bf(const void *const hw, uint8_t index, hri_tal_irqmon_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->IRQMON[index].reg ^= TAL_IRQMON_IRQNUM(mask);
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline hri_tal_irqmon_reg_t hri_tal_read_IRQMON_IRQNUM_bf(const void *const hw, uint8_t index)
{
	uint16_t tmp;
	tmp = ((Tal *)hw)->IRQMON[index].reg;
	tmp = (tmp & TAL_IRQMON_IRQNUM_Msk) >> TAL_IRQMON_IRQNUM_Pos;
	return tmp;
}

static inline void hri_tal_set_IRQMON_reg(const void *const hw, uint8_t index, hri_tal_irqmon_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->IRQMON[index].reg |= mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline hri_tal_irqmon_reg_t hri_tal_get_IRQMON_reg(const void *const hw, uint8_t index,
                                                          hri_tal_irqmon_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Tal *)hw)->IRQMON[index].reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_tal_write_IRQMON_reg(const void *const hw, uint8_t index, hri_tal_irqmon_reg_t data)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->IRQMON[index].reg = data;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_IRQMON_reg(const void *const hw, uint8_t index, hri_tal_irqmon_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->IRQMON[index].reg &= ~mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_IRQMON_reg(const void *const hw, uint8_t index, hri_tal_irqmon_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->IRQMON[index].reg ^= mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline hri_tal_irqmon_reg_t hri_tal_read_IRQMON_reg(const void *const hw, uint8_t index)
{
	return ((Tal *)hw)->IRQMON[index].reg;
}

static inline void hri_tal_set_SFLAGCLRR_IPS_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->SFLAGCLRR[index].reg |= TAL_SFLAGCLRR_IPS;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_SFLAGCLRR_IPS_bit(const void *const hw, uint8_t index)
{
	uint8_t tmp;
	tmp = ((Tal *)hw)->SFLAGCLRR[index].reg;
	tmp = (tmp & TAL_SFLAGCLRR_IPS) >> TAL_SFLAGCLRR_IPS_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_SFLAGCLRR_IPS_bit(const void *const hw, uint8_t index, bool value)
{
	uint8_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->SFLAGCLRR[index].reg;
	tmp &= ~TAL_SFLAGCLRR_IPS;
	tmp |= value << TAL_SFLAGCLRR_IPS_Pos;
	((Tal *)hw)->SFLAGCLRR[index].reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_SFLAGCLRR_IPS_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->SFLAGCLRR[index].reg &= ~TAL_SFLAGCLRR_IPS;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_SFLAGCLRR_IPS_bit(const void *const hw, uint8_t index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->SFLAGCLRR[index].reg ^= TAL_SFLAGCLRR_IPS;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_SFLAGCLRR_reg(const void *const hw, uint8_t index, hri_tal_sflagclrr_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->SFLAGCLRR[index].reg |= mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline hri_tal_sflagclrr_reg_t hri_tal_get_SFLAGCLRR_reg(const void *const hw, uint8_t index,
                                                                hri_tal_sflagclrr_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Tal *)hw)->SFLAGCLRR[index].reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_tal_write_SFLAGCLRR_reg(const void *const hw, uint8_t index, hri_tal_sflagclrr_reg_t data)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->SFLAGCLRR[index].reg = data;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_SFLAGCLRR_reg(const void *const hw, uint8_t index, hri_tal_sflagclrr_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->SFLAGCLRR[index].reg &= ~mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_SFLAGCLRR_reg(const void *const hw, uint8_t index, hri_tal_sflagclrr_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->SFLAGCLRR[index].reg ^= mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline hri_tal_sflagclrr_reg_t hri_tal_read_SFLAGCLRR_reg(const void *const hw, uint8_t index)
{
	return ((Tal *)hw)->SFLAGCLRR[index].reg;
}

static inline hri_tal_brkstatus_reg_t hri_tal_get_BRKSTATUS_CPU0_bf(const void *const hw, hri_tal_brkstatus_reg_t mask)
{
	return (((Tal *)hw)->BRKSTATUS.reg & TAL_BRKSTATUS_CPU0(mask)) >> TAL_BRKSTATUS_CPU0_Pos;
}

static inline hri_tal_brkstatus_reg_t hri_tal_read_BRKSTATUS_CPU0_bf(const void *const hw)
{
	return (((Tal *)hw)->BRKSTATUS.reg & TAL_BRKSTATUS_CPU0_Msk) >> TAL_BRKSTATUS_CPU0_Pos;
}

static inline hri_tal_brkstatus_reg_t hri_tal_get_BRKSTATUS_CPU1_bf(const void *const hw, hri_tal_brkstatus_reg_t mask)
{
	return (((Tal *)hw)->BRKSTATUS.reg & TAL_BRKSTATUS_CPU1(mask)) >> TAL_BRKSTATUS_CPU1_Pos;
}

static inline hri_tal_brkstatus_reg_t hri_tal_read_BRKSTATUS_CPU1_bf(const void *const hw)
{
	return (((Tal *)hw)->BRKSTATUS.reg & TAL_BRKSTATUS_CPU1_Msk) >> TAL_BRKSTATUS_CPU1_Pos;
}

static inline hri_tal_brkstatus_reg_t hri_tal_get_BRKSTATUS_EVBRK_bf(const void *const hw, hri_tal_brkstatus_reg_t mask)
{
	return (((Tal *)hw)->BRKSTATUS.reg & TAL_BRKSTATUS_EVBRK(mask)) >> TAL_BRKSTATUS_EVBRK_Pos;
}

static inline hri_tal_brkstatus_reg_t hri_tal_read_BRKSTATUS_EVBRK_bf(const void *const hw)
{
	return (((Tal *)hw)->BRKSTATUS.reg & TAL_BRKSTATUS_EVBRK_Msk) >> TAL_BRKSTATUS_EVBRK_Pos;
}

static inline hri_tal_brkstatus_reg_t hri_tal_get_BRKSTATUS_EXTBRK_bf(const void *const       hw,
                                                                      hri_tal_brkstatus_reg_t mask)
{
	return (((Tal *)hw)->BRKSTATUS.reg & TAL_BRKSTATUS_EXTBRK(mask)) >> TAL_BRKSTATUS_EXTBRK_Pos;
}

static inline hri_tal_brkstatus_reg_t hri_tal_read_BRKSTATUS_EXTBRK_bf(const void *const hw)
{
	return (((Tal *)hw)->BRKSTATUS.reg & TAL_BRKSTATUS_EXTBRK_Msk) >> TAL_BRKSTATUS_EXTBRK_Pos;
}

static inline hri_tal_brkstatus_reg_t hri_tal_get_BRKSTATUS_reg(const void *const hw, hri_tal_brkstatus_reg_t mask)
{
	uint16_t tmp;
	tmp = ((Tal *)hw)->BRKSTATUS.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_tal_brkstatus_reg_t hri_tal_read_BRKSTATUS_reg(const void *const hw)
{
	return ((Tal *)hw)->BRKSTATUS.reg;
}

static inline bool hri_tal_get_INTSTATUS_IRQ0_bit(const void *const hw, uint8_t index)
{
	return (((Tal *)hw)->INTSTATUS[index].reg & TAL_INTSTATUS_IRQ0) >> TAL_INTSTATUS_IRQ0_Pos;
}

static inline bool hri_tal_get_INTSTATUS_IRQ1_bit(const void *const hw, uint8_t index)
{
	return (((Tal *)hw)->INTSTATUS[index].reg & TAL_INTSTATUS_IRQ1) >> TAL_INTSTATUS_IRQ1_Pos;
}

static inline bool hri_tal_get_INTSTATUS_IRQ2_bit(const void *const hw, uint8_t index)
{
	return (((Tal *)hw)->INTSTATUS[index].reg & TAL_INTSTATUS_IRQ2) >> TAL_INTSTATUS_IRQ2_Pos;
}

static inline bool hri_tal_get_INTSTATUS_IRQ3_bit(const void *const hw, uint8_t index)
{
	return (((Tal *)hw)->INTSTATUS[index].reg & TAL_INTSTATUS_IRQ3) >> TAL_INTSTATUS_IRQ3_Pos;
}

static inline bool hri_tal_get_INTSTATUS_IRQ4_bit(const void *const hw, uint8_t index)
{
	return (((Tal *)hw)->INTSTATUS[index].reg & TAL_INTSTATUS_IRQ4) >> TAL_INTSTATUS_IRQ4_Pos;
}

static inline bool hri_tal_get_INTSTATUS_IRQ5_bit(const void *const hw, uint8_t index)
{
	return (((Tal *)hw)->INTSTATUS[index].reg & TAL_INTSTATUS_IRQ5) >> TAL_INTSTATUS_IRQ5_Pos;
}

static inline bool hri_tal_get_INTSTATUS_IRQ6_bit(const void *const hw, uint8_t index)
{
	return (((Tal *)hw)->INTSTATUS[index].reg & TAL_INTSTATUS_IRQ6) >> TAL_INTSTATUS_IRQ6_Pos;
}

static inline bool hri_tal_get_INTSTATUS_IRQ7_bit(const void *const hw, uint8_t index)
{
	return (((Tal *)hw)->INTSTATUS[index].reg & TAL_INTSTATUS_IRQ7) >> TAL_INTSTATUS_IRQ7_Pos;
}

static inline hri_tal_intstatus_reg_t hri_tal_get_INTSTATUS_reg(const void *const hw, uint8_t index,
                                                                hri_tal_intstatus_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Tal *)hw)->INTSTATUS[index].reg;
	tmp &= mask;
	return tmp;
}

static inline hri_tal_intstatus_reg_t hri_tal_read_INTSTATUS_reg(const void *const hw, uint8_t index)
{
	return ((Tal *)hw)->INTSTATUS[index].reg;
}

static inline hri_tal_cpuirqs_reg_t hri_talcpuirqs_get_CPUIRQS_CPUIRQS_bf(const void *const hw, uint8_t index,
                                                                          hri_tal_cpuirqs_reg_t mask)
{
	return (((TalCpuirqs *)hw)->CPUIRQS[index].reg & TAL_CPUIRQS_CPUIRQS(mask)) >> TAL_CPUIRQS_CPUIRQS_Pos;
}

static inline hri_tal_cpuirqs_reg_t hri_talcpuirqs_read_CPUIRQS_CPUIRQS_bf(const void *const hw, uint8_t index)
{
	return (((TalCpuirqs *)hw)->CPUIRQS[index].reg & TAL_CPUIRQS_CPUIRQS_Msk) >> TAL_CPUIRQS_CPUIRQS_Pos;
}

static inline hri_tal_cpuirqs_reg_t hri_talcpuirqs_get_CPUIRQS_reg(const void *const hw, uint8_t index,
                                                                   hri_tal_cpuirqs_reg_t mask)
{
	uint32_t tmp;
	tmp = ((TalCpuirqs *)hw)->CPUIRQS[index].reg;
	tmp &= mask;
	return tmp;
}

static inline hri_tal_cpuirqs_reg_t hri_talcpuirqs_read_CPUIRQS_reg(const void *const hw, uint8_t index)
{
	return ((TalCpuirqs *)hw)->CPUIRQS[index].reg;
}

static inline hri_tal_cpuirqs_reg_t hri_tal_get_CPUIRQS_CPUIRQS_bf(const void *const hw, uint8_t submodule_index,
                                                                   uint8_t index, hri_tal_cpuirqs_reg_t mask)
{
	return (((Tal *)hw)->Cpuirqs[submodule_index].CPUIRQS[index].reg & TAL_CPUIRQS_CPUIRQS(mask))
	       >> TAL_CPUIRQS_CPUIRQS_Pos;
}

static inline hri_tal_cpuirqs_reg_t hri_tal_read_CPUIRQS_CPUIRQS_bf(const void *const hw, uint8_t submodule_index,
                                                                    uint8_t index)
{
	return (((Tal *)hw)->Cpuirqs[submodule_index].CPUIRQS[index].reg & TAL_CPUIRQS_CPUIRQS_Msk)
	       >> TAL_CPUIRQS_CPUIRQS_Pos;
}

static inline hri_tal_cpuirqs_reg_t hri_tal_get_CPUIRQS_reg(const void *const hw, uint8_t submodule_index,
                                                            uint8_t index, hri_tal_cpuirqs_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Tal *)hw)->Cpuirqs[submodule_index].CPUIRQS[index].reg;
	tmp &= mask;
	return tmp;
}

static inline hri_tal_cpuirqs_reg_t hri_tal_read_CPUIRQS_reg(const void *const hw, uint8_t submodule_index,
                                                             uint8_t index)
{
	return ((Tal *)hw)->Cpuirqs[submodule_index].CPUIRQS[index].reg;
}

static inline void hri_talctis_set_CTICTRLA_RESTART_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalCtis *)hw)->CTICTRLA.reg |= TAL_CTICTRLA_RESTART;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_talctis_get_CTICTRLA_RESTART_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((TalCtis *)hw)->CTICTRLA.reg;
	tmp = (tmp & TAL_CTICTRLA_RESTART) >> TAL_CTICTRLA_RESTART_Pos;
	return (bool)tmp;
}

static inline void hri_talctis_write_CTICTRLA_RESTART_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((TalCtis *)hw)->CTICTRLA.reg;
	tmp &= ~TAL_CTICTRLA_RESTART;
	tmp |= value << TAL_CTICTRLA_RESTART_Pos;
	((TalCtis *)hw)->CTICTRLA.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talctis_clear_CTICTRLA_RESTART_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalCtis *)hw)->CTICTRLA.reg &= ~TAL_CTICTRLA_RESTART;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talctis_toggle_CTICTRLA_RESTART_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalCtis *)hw)->CTICTRLA.reg ^= TAL_CTICTRLA_RESTART;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talctis_set_CTICTRLA_IPS_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalCtis *)hw)->CTICTRLA.reg |= TAL_CTICTRLA_IPS;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_talctis_get_CTICTRLA_IPS_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((TalCtis *)hw)->CTICTRLA.reg;
	tmp = (tmp & TAL_CTICTRLA_IPS) >> TAL_CTICTRLA_IPS_Pos;
	return (bool)tmp;
}

static inline void hri_talctis_write_CTICTRLA_IPS_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((TalCtis *)hw)->CTICTRLA.reg;
	tmp &= ~TAL_CTICTRLA_IPS;
	tmp |= value << TAL_CTICTRLA_IPS_Pos;
	((TalCtis *)hw)->CTICTRLA.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talctis_clear_CTICTRLA_IPS_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalCtis *)hw)->CTICTRLA.reg &= ~TAL_CTICTRLA_IPS;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talctis_toggle_CTICTRLA_IPS_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalCtis *)hw)->CTICTRLA.reg ^= TAL_CTICTRLA_IPS;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talctis_set_CTICTRLA_BRK_bf(const void *const hw, hri_tal_ctictrla_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalCtis *)hw)->CTICTRLA.reg |= TAL_CTICTRLA_BRK(mask);
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline hri_tal_ctictrla_reg_t hri_talctis_get_CTICTRLA_BRK_bf(const void *const hw, hri_tal_ctictrla_reg_t mask)
{
	uint8_t tmp;
	tmp = ((TalCtis *)hw)->CTICTRLA.reg;
	tmp = (tmp & TAL_CTICTRLA_BRK(mask)) >> TAL_CTICTRLA_BRK_Pos;
	return tmp;
}

static inline void hri_talctis_write_CTICTRLA_BRK_bf(const void *const hw, hri_tal_ctictrla_reg_t data)
{
	uint8_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((TalCtis *)hw)->CTICTRLA.reg;
	tmp &= ~TAL_CTICTRLA_BRK_Msk;
	tmp |= TAL_CTICTRLA_BRK(data);
	((TalCtis *)hw)->CTICTRLA.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talctis_clear_CTICTRLA_BRK_bf(const void *const hw, hri_tal_ctictrla_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalCtis *)hw)->CTICTRLA.reg &= ~TAL_CTICTRLA_BRK(mask);
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talctis_toggle_CTICTRLA_BRK_bf(const void *const hw, hri_tal_ctictrla_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalCtis *)hw)->CTICTRLA.reg ^= TAL_CTICTRLA_BRK(mask);
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline hri_tal_ctictrla_reg_t hri_talctis_read_CTICTRLA_BRK_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((TalCtis *)hw)->CTICTRLA.reg;
	tmp = (tmp & TAL_CTICTRLA_BRK_Msk) >> TAL_CTICTRLA_BRK_Pos;
	return tmp;
}

static inline void hri_talctis_set_CTICTRLA_reg(const void *const hw, hri_tal_ctictrla_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalCtis *)hw)->CTICTRLA.reg |= mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline hri_tal_ctictrla_reg_t hri_talctis_get_CTICTRLA_reg(const void *const hw, hri_tal_ctictrla_reg_t mask)
{
	uint8_t tmp;
	tmp = ((TalCtis *)hw)->CTICTRLA.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_talctis_write_CTICTRLA_reg(const void *const hw, hri_tal_ctictrla_reg_t data)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalCtis *)hw)->CTICTRLA.reg = data;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talctis_clear_CTICTRLA_reg(const void *const hw, hri_tal_ctictrla_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalCtis *)hw)->CTICTRLA.reg &= ~mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talctis_toggle_CTICTRLA_reg(const void *const hw, hri_tal_ctictrla_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalCtis *)hw)->CTICTRLA.reg ^= mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline hri_tal_ctictrla_reg_t hri_talctis_read_CTICTRLA_reg(const void *const hw)
{
	return ((TalCtis *)hw)->CTICTRLA.reg;
}

static inline void hri_talctis_set_CTIMASK_CPU0_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalCtis *)hw)->CTIMASK.reg |= TAL_CTIMASK_CPU0;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_talctis_get_CTIMASK_CPU0_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((TalCtis *)hw)->CTIMASK.reg;
	tmp = (tmp & TAL_CTIMASK_CPU0) >> TAL_CTIMASK_CPU0_Pos;
	return (bool)tmp;
}

static inline void hri_talctis_write_CTIMASK_CPU0_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((TalCtis *)hw)->CTIMASK.reg;
	tmp &= ~TAL_CTIMASK_CPU0;
	tmp |= value << TAL_CTIMASK_CPU0_Pos;
	((TalCtis *)hw)->CTIMASK.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talctis_clear_CTIMASK_CPU0_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalCtis *)hw)->CTIMASK.reg &= ~TAL_CTIMASK_CPU0;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talctis_toggle_CTIMASK_CPU0_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalCtis *)hw)->CTIMASK.reg ^= TAL_CTIMASK_CPU0;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talctis_set_CTIMASK_CPU1_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalCtis *)hw)->CTIMASK.reg |= TAL_CTIMASK_CPU1;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_talctis_get_CTIMASK_CPU1_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((TalCtis *)hw)->CTIMASK.reg;
	tmp = (tmp & TAL_CTIMASK_CPU1) >> TAL_CTIMASK_CPU1_Pos;
	return (bool)tmp;
}

static inline void hri_talctis_write_CTIMASK_CPU1_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((TalCtis *)hw)->CTIMASK.reg;
	tmp &= ~TAL_CTIMASK_CPU1;
	tmp |= value << TAL_CTIMASK_CPU1_Pos;
	((TalCtis *)hw)->CTIMASK.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talctis_clear_CTIMASK_CPU1_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalCtis *)hw)->CTIMASK.reg &= ~TAL_CTIMASK_CPU1;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talctis_toggle_CTIMASK_CPU1_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalCtis *)hw)->CTIMASK.reg ^= TAL_CTIMASK_CPU1;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talctis_set_CTIMASK_EVBRK_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalCtis *)hw)->CTIMASK.reg |= TAL_CTIMASK_EVBRK;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_talctis_get_CTIMASK_EVBRK_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((TalCtis *)hw)->CTIMASK.reg;
	tmp = (tmp & TAL_CTIMASK_EVBRK) >> TAL_CTIMASK_EVBRK_Pos;
	return (bool)tmp;
}

static inline void hri_talctis_write_CTIMASK_EVBRK_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((TalCtis *)hw)->CTIMASK.reg;
	tmp &= ~TAL_CTIMASK_EVBRK;
	tmp |= value << TAL_CTIMASK_EVBRK_Pos;
	((TalCtis *)hw)->CTIMASK.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talctis_clear_CTIMASK_EVBRK_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalCtis *)hw)->CTIMASK.reg &= ~TAL_CTIMASK_EVBRK;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talctis_toggle_CTIMASK_EVBRK_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalCtis *)hw)->CTIMASK.reg ^= TAL_CTIMASK_EVBRK;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talctis_set_CTIMASK_EXTBRK_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalCtis *)hw)->CTIMASK.reg |= TAL_CTIMASK_EXTBRK;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_talctis_get_CTIMASK_EXTBRK_bit(const void *const hw)
{
	uint8_t tmp;
	tmp = ((TalCtis *)hw)->CTIMASK.reg;
	tmp = (tmp & TAL_CTIMASK_EXTBRK) >> TAL_CTIMASK_EXTBRK_Pos;
	return (bool)tmp;
}

static inline void hri_talctis_write_CTIMASK_EXTBRK_bit(const void *const hw, bool value)
{
	uint8_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((TalCtis *)hw)->CTIMASK.reg;
	tmp &= ~TAL_CTIMASK_EXTBRK;
	tmp |= value << TAL_CTIMASK_EXTBRK_Pos;
	((TalCtis *)hw)->CTIMASK.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talctis_clear_CTIMASK_EXTBRK_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalCtis *)hw)->CTIMASK.reg &= ~TAL_CTIMASK_EXTBRK;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talctis_toggle_CTIMASK_EXTBRK_bit(const void *const hw)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalCtis *)hw)->CTIMASK.reg ^= TAL_CTIMASK_EXTBRK;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talctis_set_CTIMASK_reg(const void *const hw, hri_tal_ctimask_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalCtis *)hw)->CTIMASK.reg |= mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline hri_tal_ctimask_reg_t hri_talctis_get_CTIMASK_reg(const void *const hw, hri_tal_ctimask_reg_t mask)
{
	uint8_t tmp;
	tmp = ((TalCtis *)hw)->CTIMASK.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_talctis_write_CTIMASK_reg(const void *const hw, hri_tal_ctimask_reg_t data)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalCtis *)hw)->CTIMASK.reg = data;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talctis_clear_CTIMASK_reg(const void *const hw, hri_tal_ctimask_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalCtis *)hw)->CTIMASK.reg &= ~mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_talctis_toggle_CTIMASK_reg(const void *const hw, hri_tal_ctimask_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((TalCtis *)hw)->CTIMASK.reg ^= mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline hri_tal_ctimask_reg_t hri_talctis_read_CTIMASK_reg(const void *const hw)
{
	return ((TalCtis *)hw)->CTIMASK.reg;
}

static inline void hri_tal_set_CTICTRLA_RESTART_bit(const void *const hw, uint8_t submodule_index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Ctis[submodule_index].CTICTRLA.reg |= TAL_CTICTRLA_RESTART;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_CTICTRLA_RESTART_bit(const void *const hw, uint8_t submodule_index)
{
	uint8_t tmp;
	tmp = ((Tal *)hw)->Ctis[submodule_index].CTICTRLA.reg;
	tmp = (tmp & TAL_CTICTRLA_RESTART) >> TAL_CTICTRLA_RESTART_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_CTICTRLA_RESTART_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	uint8_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->Ctis[submodule_index].CTICTRLA.reg;
	tmp &= ~TAL_CTICTRLA_RESTART;
	tmp |= value << TAL_CTICTRLA_RESTART_Pos;
	((Tal *)hw)->Ctis[submodule_index].CTICTRLA.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_CTICTRLA_RESTART_bit(const void *const hw, uint8_t submodule_index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Ctis[submodule_index].CTICTRLA.reg &= ~TAL_CTICTRLA_RESTART;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_CTICTRLA_RESTART_bit(const void *const hw, uint8_t submodule_index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Ctis[submodule_index].CTICTRLA.reg ^= TAL_CTICTRLA_RESTART;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_CTICTRLA_IPS_bit(const void *const hw, uint8_t submodule_index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Ctis[submodule_index].CTICTRLA.reg |= TAL_CTICTRLA_IPS;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_CTICTRLA_IPS_bit(const void *const hw, uint8_t submodule_index)
{
	uint8_t tmp;
	tmp = ((Tal *)hw)->Ctis[submodule_index].CTICTRLA.reg;
	tmp = (tmp & TAL_CTICTRLA_IPS) >> TAL_CTICTRLA_IPS_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_CTICTRLA_IPS_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	uint8_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->Ctis[submodule_index].CTICTRLA.reg;
	tmp &= ~TAL_CTICTRLA_IPS;
	tmp |= value << TAL_CTICTRLA_IPS_Pos;
	((Tal *)hw)->Ctis[submodule_index].CTICTRLA.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_CTICTRLA_IPS_bit(const void *const hw, uint8_t submodule_index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Ctis[submodule_index].CTICTRLA.reg &= ~TAL_CTICTRLA_IPS;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_CTICTRLA_IPS_bit(const void *const hw, uint8_t submodule_index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Ctis[submodule_index].CTICTRLA.reg ^= TAL_CTICTRLA_IPS;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_CTICTRLA_BRK_bf(const void *const hw, uint8_t submodule_index,
                                               hri_tal_ctictrla_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Ctis[submodule_index].CTICTRLA.reg |= TAL_CTICTRLA_BRK(mask);
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline hri_tal_ctictrla_reg_t hri_tal_get_CTICTRLA_BRK_bf(const void *const hw, uint8_t submodule_index,
                                                                 hri_tal_ctictrla_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Tal *)hw)->Ctis[submodule_index].CTICTRLA.reg;
	tmp = (tmp & TAL_CTICTRLA_BRK(mask)) >> TAL_CTICTRLA_BRK_Pos;
	return tmp;
}

static inline void hri_tal_write_CTICTRLA_BRK_bf(const void *const hw, uint8_t submodule_index,
                                                 hri_tal_ctictrla_reg_t data)
{
	uint8_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->Ctis[submodule_index].CTICTRLA.reg;
	tmp &= ~TAL_CTICTRLA_BRK_Msk;
	tmp |= TAL_CTICTRLA_BRK(data);
	((Tal *)hw)->Ctis[submodule_index].CTICTRLA.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_CTICTRLA_BRK_bf(const void *const hw, uint8_t submodule_index,
                                                 hri_tal_ctictrla_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Ctis[submodule_index].CTICTRLA.reg &= ~TAL_CTICTRLA_BRK(mask);
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_CTICTRLA_BRK_bf(const void *const hw, uint8_t submodule_index,
                                                  hri_tal_ctictrla_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Ctis[submodule_index].CTICTRLA.reg ^= TAL_CTICTRLA_BRK(mask);
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline hri_tal_ctictrla_reg_t hri_tal_read_CTICTRLA_BRK_bf(const void *const hw, uint8_t submodule_index)
{
	uint8_t tmp;
	tmp = ((Tal *)hw)->Ctis[submodule_index].CTICTRLA.reg;
	tmp = (tmp & TAL_CTICTRLA_BRK_Msk) >> TAL_CTICTRLA_BRK_Pos;
	return tmp;
}

static inline void hri_tal_set_CTICTRLA_reg(const void *const hw, uint8_t submodule_index, hri_tal_ctictrla_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Ctis[submodule_index].CTICTRLA.reg |= mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline hri_tal_ctictrla_reg_t hri_tal_get_CTICTRLA_reg(const void *const hw, uint8_t submodule_index,
                                                              hri_tal_ctictrla_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Tal *)hw)->Ctis[submodule_index].CTICTRLA.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_tal_write_CTICTRLA_reg(const void *const hw, uint8_t submodule_index,
                                              hri_tal_ctictrla_reg_t data)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Ctis[submodule_index].CTICTRLA.reg = data;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_CTICTRLA_reg(const void *const hw, uint8_t submodule_index,
                                              hri_tal_ctictrla_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Ctis[submodule_index].CTICTRLA.reg &= ~mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_CTICTRLA_reg(const void *const hw, uint8_t submodule_index,
                                               hri_tal_ctictrla_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Ctis[submodule_index].CTICTRLA.reg ^= mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline hri_tal_ctictrla_reg_t hri_tal_read_CTICTRLA_reg(const void *const hw, uint8_t submodule_index)
{
	return ((Tal *)hw)->Ctis[submodule_index].CTICTRLA.reg;
}

static inline void hri_tal_set_CTIMASK_CPU0_bit(const void *const hw, uint8_t submodule_index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Ctis[submodule_index].CTIMASK.reg |= TAL_CTIMASK_CPU0;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_CTIMASK_CPU0_bit(const void *const hw, uint8_t submodule_index)
{
	uint8_t tmp;
	tmp = ((Tal *)hw)->Ctis[submodule_index].CTIMASK.reg;
	tmp = (tmp & TAL_CTIMASK_CPU0) >> TAL_CTIMASK_CPU0_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_CTIMASK_CPU0_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	uint8_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->Ctis[submodule_index].CTIMASK.reg;
	tmp &= ~TAL_CTIMASK_CPU0;
	tmp |= value << TAL_CTIMASK_CPU0_Pos;
	((Tal *)hw)->Ctis[submodule_index].CTIMASK.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_CTIMASK_CPU0_bit(const void *const hw, uint8_t submodule_index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Ctis[submodule_index].CTIMASK.reg &= ~TAL_CTIMASK_CPU0;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_CTIMASK_CPU0_bit(const void *const hw, uint8_t submodule_index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Ctis[submodule_index].CTIMASK.reg ^= TAL_CTIMASK_CPU0;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_CTIMASK_CPU1_bit(const void *const hw, uint8_t submodule_index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Ctis[submodule_index].CTIMASK.reg |= TAL_CTIMASK_CPU1;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_CTIMASK_CPU1_bit(const void *const hw, uint8_t submodule_index)
{
	uint8_t tmp;
	tmp = ((Tal *)hw)->Ctis[submodule_index].CTIMASK.reg;
	tmp = (tmp & TAL_CTIMASK_CPU1) >> TAL_CTIMASK_CPU1_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_CTIMASK_CPU1_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	uint8_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->Ctis[submodule_index].CTIMASK.reg;
	tmp &= ~TAL_CTIMASK_CPU1;
	tmp |= value << TAL_CTIMASK_CPU1_Pos;
	((Tal *)hw)->Ctis[submodule_index].CTIMASK.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_CTIMASK_CPU1_bit(const void *const hw, uint8_t submodule_index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Ctis[submodule_index].CTIMASK.reg &= ~TAL_CTIMASK_CPU1;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_CTIMASK_CPU1_bit(const void *const hw, uint8_t submodule_index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Ctis[submodule_index].CTIMASK.reg ^= TAL_CTIMASK_CPU1;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_CTIMASK_EVBRK_bit(const void *const hw, uint8_t submodule_index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Ctis[submodule_index].CTIMASK.reg |= TAL_CTIMASK_EVBRK;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_CTIMASK_EVBRK_bit(const void *const hw, uint8_t submodule_index)
{
	uint8_t tmp;
	tmp = ((Tal *)hw)->Ctis[submodule_index].CTIMASK.reg;
	tmp = (tmp & TAL_CTIMASK_EVBRK) >> TAL_CTIMASK_EVBRK_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_CTIMASK_EVBRK_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	uint8_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->Ctis[submodule_index].CTIMASK.reg;
	tmp &= ~TAL_CTIMASK_EVBRK;
	tmp |= value << TAL_CTIMASK_EVBRK_Pos;
	((Tal *)hw)->Ctis[submodule_index].CTIMASK.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_CTIMASK_EVBRK_bit(const void *const hw, uint8_t submodule_index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Ctis[submodule_index].CTIMASK.reg &= ~TAL_CTIMASK_EVBRK;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_CTIMASK_EVBRK_bit(const void *const hw, uint8_t submodule_index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Ctis[submodule_index].CTIMASK.reg ^= TAL_CTIMASK_EVBRK;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_CTIMASK_EXTBRK_bit(const void *const hw, uint8_t submodule_index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Ctis[submodule_index].CTIMASK.reg |= TAL_CTIMASK_EXTBRK;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_tal_get_CTIMASK_EXTBRK_bit(const void *const hw, uint8_t submodule_index)
{
	uint8_t tmp;
	tmp = ((Tal *)hw)->Ctis[submodule_index].CTIMASK.reg;
	tmp = (tmp & TAL_CTIMASK_EXTBRK) >> TAL_CTIMASK_EXTBRK_Pos;
	return (bool)tmp;
}

static inline void hri_tal_write_CTIMASK_EXTBRK_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	uint8_t tmp;
	TAL_CRITICAL_SECTION_ENTER();
	tmp = ((Tal *)hw)->Ctis[submodule_index].CTIMASK.reg;
	tmp &= ~TAL_CTIMASK_EXTBRK;
	tmp |= value << TAL_CTIMASK_EXTBRK_Pos;
	((Tal *)hw)->Ctis[submodule_index].CTIMASK.reg = tmp;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_CTIMASK_EXTBRK_bit(const void *const hw, uint8_t submodule_index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Ctis[submodule_index].CTIMASK.reg &= ~TAL_CTIMASK_EXTBRK;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_CTIMASK_EXTBRK_bit(const void *const hw, uint8_t submodule_index)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Ctis[submodule_index].CTIMASK.reg ^= TAL_CTIMASK_EXTBRK;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_set_CTIMASK_reg(const void *const hw, uint8_t submodule_index, hri_tal_ctimask_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Ctis[submodule_index].CTIMASK.reg |= mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline hri_tal_ctimask_reg_t hri_tal_get_CTIMASK_reg(const void *const hw, uint8_t submodule_index,
                                                            hri_tal_ctimask_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Tal *)hw)->Ctis[submodule_index].CTIMASK.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_tal_write_CTIMASK_reg(const void *const hw, uint8_t submodule_index, hri_tal_ctimask_reg_t data)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Ctis[submodule_index].CTIMASK.reg = data;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_clear_CTIMASK_reg(const void *const hw, uint8_t submodule_index, hri_tal_ctimask_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Ctis[submodule_index].CTIMASK.reg &= ~mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline void hri_tal_toggle_CTIMASK_reg(const void *const hw, uint8_t submodule_index, hri_tal_ctimask_reg_t mask)
{
	TAL_CRITICAL_SECTION_ENTER();
	((Tal *)hw)->Ctis[submodule_index].CTIMASK.reg ^= mask;
	TAL_CRITICAL_SECTION_LEAVE();
}

static inline hri_tal_ctimask_reg_t hri_tal_read_CTIMASK_reg(const void *const hw, uint8_t submodule_index)
{
	return ((Tal *)hw)->Ctis[submodule_index].CTIMASK.reg;
}

#ifdef __cplusplus
}
#endif

#endif /* _HRI_TAL_D51_H_INCLUDED */
#endif /* _SAMD51_TAL_COMPONENT_ */
