/**
 * \file
 *
 * \brief SAM SystemControl
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

#ifdef _SAMD21_SystemControl_COMPONENT_
#ifndef _HRI_SystemControl_D21_H_INCLUDED_
#define _HRI_SystemControl_D21_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <hal_atomic.h>

#if defined(ENABLE_SystemControl_CRITICAL_SECTIONS)
#define SystemControl_CRITICAL_SECTION_ENTER() CRITICAL_SECTION_ENTER()
#define SystemControl_CRITICAL_SECTION_LEAVE() CRITICAL_SECTION_LEAVE()
#else
#define SystemControl_CRITICAL_SECTION_ENTER()
#define SystemControl_CRITICAL_SECTION_LEAVE()
#endif

typedef uint32_t hri_systemcontrol_aircr_reg_t;
typedef uint32_t hri_systemcontrol_ccr_reg_t;
typedef uint32_t hri_systemcontrol_cpuid_reg_t;
typedef uint32_t hri_systemcontrol_icsr_reg_t;
typedef uint32_t hri_systemcontrol_scr_reg_t;
typedef uint32_t hri_systemcontrol_shpr2_reg_t;
typedef uint32_t hri_systemcontrol_shpr3_reg_t;
typedef uint32_t hri_systemcontrol_vtor_reg_t;

static inline void hri_systemcontrol_set_CPUID_REVISION_bf(const void *const hw, hri_systemcontrol_cpuid_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CPUID.reg |= SystemControl_CPUID_REVISION(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_cpuid_reg_t hri_systemcontrol_get_CPUID_REVISION_bf(const void *const             hw,
                                                                                    hri_systemcontrol_cpuid_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->CPUID.reg;
	tmp = (tmp & SystemControl_CPUID_REVISION(mask)) >> 0;
	return tmp;
}

static inline void hri_systemcontrol_write_CPUID_REVISION_bf(const void *const hw, hri_systemcontrol_cpuid_reg_t data)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->CPUID.reg;
	tmp &= ~SystemControl_CPUID_REVISION_Msk;
	tmp |= SystemControl_CPUID_REVISION(data);
	((Systemcontrol *)hw)->CPUID.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_CPUID_REVISION_bf(const void *const hw, hri_systemcontrol_cpuid_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CPUID.reg &= ~SystemControl_CPUID_REVISION(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_CPUID_REVISION_bf(const void *const hw, hri_systemcontrol_cpuid_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CPUID.reg ^= SystemControl_CPUID_REVISION(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_cpuid_reg_t hri_systemcontrol_read_CPUID_REVISION_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->CPUID.reg;
	tmp = (tmp & SystemControl_CPUID_REVISION_Msk) >> 0;
	return tmp;
}

static inline void hri_systemcontrol_set_CPUID_PARTNO_bf(const void *const hw, hri_systemcontrol_cpuid_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CPUID.reg |= SystemControl_CPUID_PARTNO(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_cpuid_reg_t hri_systemcontrol_get_CPUID_PARTNO_bf(const void *const             hw,
                                                                                  hri_systemcontrol_cpuid_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->CPUID.reg;
	tmp = (tmp & SystemControl_CPUID_PARTNO(mask)) >> 4;
	return tmp;
}

static inline void hri_systemcontrol_write_CPUID_PARTNO_bf(const void *const hw, hri_systemcontrol_cpuid_reg_t data)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->CPUID.reg;
	tmp &= ~SystemControl_CPUID_PARTNO_Msk;
	tmp |= SystemControl_CPUID_PARTNO(data);
	((Systemcontrol *)hw)->CPUID.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_CPUID_PARTNO_bf(const void *const hw, hri_systemcontrol_cpuid_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CPUID.reg &= ~SystemControl_CPUID_PARTNO(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_CPUID_PARTNO_bf(const void *const hw, hri_systemcontrol_cpuid_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CPUID.reg ^= SystemControl_CPUID_PARTNO(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_cpuid_reg_t hri_systemcontrol_read_CPUID_PARTNO_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->CPUID.reg;
	tmp = (tmp & SystemControl_CPUID_PARTNO_Msk) >> 4;
	return tmp;
}

static inline void hri_systemcontrol_set_CPUID_ARCHITECTURE_bf(const void *const hw, hri_systemcontrol_cpuid_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CPUID.reg |= SystemControl_CPUID_ARCHITECTURE(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_cpuid_reg_t
hri_systemcontrol_get_CPUID_ARCHITECTURE_bf(const void *const hw, hri_systemcontrol_cpuid_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->CPUID.reg;
	tmp = (tmp & SystemControl_CPUID_ARCHITECTURE(mask)) >> 16;
	return tmp;
}

static inline void hri_systemcontrol_write_CPUID_ARCHITECTURE_bf(const void *const             hw,
                                                                 hri_systemcontrol_cpuid_reg_t data)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->CPUID.reg;
	tmp &= ~SystemControl_CPUID_ARCHITECTURE_Msk;
	tmp |= SystemControl_CPUID_ARCHITECTURE(data);
	((Systemcontrol *)hw)->CPUID.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_CPUID_ARCHITECTURE_bf(const void *const             hw,
                                                                 hri_systemcontrol_cpuid_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CPUID.reg &= ~SystemControl_CPUID_ARCHITECTURE(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_CPUID_ARCHITECTURE_bf(const void *const             hw,
                                                                  hri_systemcontrol_cpuid_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CPUID.reg ^= SystemControl_CPUID_ARCHITECTURE(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_cpuid_reg_t hri_systemcontrol_read_CPUID_ARCHITECTURE_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->CPUID.reg;
	tmp = (tmp & SystemControl_CPUID_ARCHITECTURE_Msk) >> 16;
	return tmp;
}

static inline void hri_systemcontrol_set_CPUID_VARIANT_bf(const void *const hw, hri_systemcontrol_cpuid_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CPUID.reg |= SystemControl_CPUID_VARIANT(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_cpuid_reg_t hri_systemcontrol_get_CPUID_VARIANT_bf(const void *const             hw,
                                                                                   hri_systemcontrol_cpuid_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->CPUID.reg;
	tmp = (tmp & SystemControl_CPUID_VARIANT(mask)) >> 20;
	return tmp;
}

static inline void hri_systemcontrol_write_CPUID_VARIANT_bf(const void *const hw, hri_systemcontrol_cpuid_reg_t data)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->CPUID.reg;
	tmp &= ~SystemControl_CPUID_VARIANT_Msk;
	tmp |= SystemControl_CPUID_VARIANT(data);
	((Systemcontrol *)hw)->CPUID.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_CPUID_VARIANT_bf(const void *const hw, hri_systemcontrol_cpuid_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CPUID.reg &= ~SystemControl_CPUID_VARIANT(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_CPUID_VARIANT_bf(const void *const hw, hri_systemcontrol_cpuid_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CPUID.reg ^= SystemControl_CPUID_VARIANT(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_cpuid_reg_t hri_systemcontrol_read_CPUID_VARIANT_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->CPUID.reg;
	tmp = (tmp & SystemControl_CPUID_VARIANT_Msk) >> 20;
	return tmp;
}

static inline void hri_systemcontrol_set_CPUID_IMPLEMENTER_bf(const void *const hw, hri_systemcontrol_cpuid_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CPUID.reg |= SystemControl_CPUID_IMPLEMENTER(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_cpuid_reg_t
hri_systemcontrol_get_CPUID_IMPLEMENTER_bf(const void *const hw, hri_systemcontrol_cpuid_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->CPUID.reg;
	tmp = (tmp & SystemControl_CPUID_IMPLEMENTER(mask)) >> 24;
	return tmp;
}

static inline void hri_systemcontrol_write_CPUID_IMPLEMENTER_bf(const void *const             hw,
                                                                hri_systemcontrol_cpuid_reg_t data)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->CPUID.reg;
	tmp &= ~SystemControl_CPUID_IMPLEMENTER_Msk;
	tmp |= SystemControl_CPUID_IMPLEMENTER(data);
	((Systemcontrol *)hw)->CPUID.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_CPUID_IMPLEMENTER_bf(const void *const             hw,
                                                                hri_systemcontrol_cpuid_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CPUID.reg &= ~SystemControl_CPUID_IMPLEMENTER(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_CPUID_IMPLEMENTER_bf(const void *const             hw,
                                                                 hri_systemcontrol_cpuid_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CPUID.reg ^= SystemControl_CPUID_IMPLEMENTER(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_cpuid_reg_t hri_systemcontrol_read_CPUID_IMPLEMENTER_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->CPUID.reg;
	tmp = (tmp & SystemControl_CPUID_IMPLEMENTER_Msk) >> 24;
	return tmp;
}

static inline void hri_systemcontrol_set_CPUID_reg(const void *const hw, hri_systemcontrol_cpuid_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CPUID.reg |= mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_cpuid_reg_t hri_systemcontrol_get_CPUID_reg(const void *const             hw,
                                                                            hri_systemcontrol_cpuid_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->CPUID.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_systemcontrol_write_CPUID_reg(const void *const hw, hri_systemcontrol_cpuid_reg_t data)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CPUID.reg = data;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_CPUID_reg(const void *const hw, hri_systemcontrol_cpuid_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CPUID.reg &= ~mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_CPUID_reg(const void *const hw, hri_systemcontrol_cpuid_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CPUID.reg ^= mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_cpuid_reg_t hri_systemcontrol_read_CPUID_reg(const void *const hw)
{
	return ((Systemcontrol *)hw)->CPUID.reg;
}

static inline void hri_systemcontrol_set_ICSR_PENDSTCLR_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ICSR.reg |= SystemControl_ICSR_PENDSTCLR;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_ICSR_PENDSTCLR_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->ICSR.reg;
	tmp = (tmp & SystemControl_ICSR_PENDSTCLR) >> 25;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_ICSR_PENDSTCLR_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->ICSR.reg;
	tmp &= ~SystemControl_ICSR_PENDSTCLR;
	tmp |= value << 25;
	((Systemcontrol *)hw)->ICSR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_ICSR_PENDSTCLR_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ICSR.reg &= ~SystemControl_ICSR_PENDSTCLR;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_ICSR_PENDSTCLR_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ICSR.reg ^= SystemControl_ICSR_PENDSTCLR;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_ICSR_PENDSTSET_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ICSR.reg |= SystemControl_ICSR_PENDSTSET;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_ICSR_PENDSTSET_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->ICSR.reg;
	tmp = (tmp & SystemControl_ICSR_PENDSTSET) >> 26;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_ICSR_PENDSTSET_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->ICSR.reg;
	tmp &= ~SystemControl_ICSR_PENDSTSET;
	tmp |= value << 26;
	((Systemcontrol *)hw)->ICSR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_ICSR_PENDSTSET_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ICSR.reg &= ~SystemControl_ICSR_PENDSTSET;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_ICSR_PENDSTSET_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ICSR.reg ^= SystemControl_ICSR_PENDSTSET;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_ICSR_PENDSVCLR_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ICSR.reg |= SystemControl_ICSR_PENDSVCLR;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_ICSR_PENDSVCLR_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->ICSR.reg;
	tmp = (tmp & SystemControl_ICSR_PENDSVCLR) >> 27;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_ICSR_PENDSVCLR_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->ICSR.reg;
	tmp &= ~SystemControl_ICSR_PENDSVCLR;
	tmp |= value << 27;
	((Systemcontrol *)hw)->ICSR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_ICSR_PENDSVCLR_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ICSR.reg &= ~SystemControl_ICSR_PENDSVCLR;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_ICSR_PENDSVCLR_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ICSR.reg ^= SystemControl_ICSR_PENDSVCLR;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_ICSR_PENDSVSET_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ICSR.reg |= SystemControl_ICSR_PENDSVSET;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_ICSR_PENDSVSET_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->ICSR.reg;
	tmp = (tmp & SystemControl_ICSR_PENDSVSET) >> 28;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_ICSR_PENDSVSET_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->ICSR.reg;
	tmp &= ~SystemControl_ICSR_PENDSVSET;
	tmp |= value << 28;
	((Systemcontrol *)hw)->ICSR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_ICSR_PENDSVSET_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ICSR.reg &= ~SystemControl_ICSR_PENDSVSET;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_ICSR_PENDSVSET_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ICSR.reg ^= SystemControl_ICSR_PENDSVSET;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_ICSR_NMIPENDSET_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ICSR.reg |= SystemControl_ICSR_NMIPENDSET;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_ICSR_NMIPENDSET_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->ICSR.reg;
	tmp = (tmp & SystemControl_ICSR_NMIPENDSET) >> 31;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_ICSR_NMIPENDSET_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->ICSR.reg;
	tmp &= ~SystemControl_ICSR_NMIPENDSET;
	tmp |= value << 31;
	((Systemcontrol *)hw)->ICSR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_ICSR_NMIPENDSET_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ICSR.reg &= ~SystemControl_ICSR_NMIPENDSET;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_ICSR_NMIPENDSET_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ICSR.reg ^= SystemControl_ICSR_NMIPENDSET;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_ICSR_VECTPENDING_bf(const void *const hw, hri_systemcontrol_icsr_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ICSR.reg |= SystemControl_ICSR_VECTPENDING(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_icsr_reg_t hri_systemcontrol_get_ICSR_VECTPENDING_bf(const void *const            hw,
                                                                                     hri_systemcontrol_icsr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->ICSR.reg;
	tmp = (tmp & SystemControl_ICSR_VECTPENDING(mask)) >> 12;
	return tmp;
}

static inline void hri_systemcontrol_write_ICSR_VECTPENDING_bf(const void *const hw, hri_systemcontrol_icsr_reg_t data)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->ICSR.reg;
	tmp &= ~SystemControl_ICSR_VECTPENDING_Msk;
	tmp |= SystemControl_ICSR_VECTPENDING(data);
	((Systemcontrol *)hw)->ICSR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_ICSR_VECTPENDING_bf(const void *const hw, hri_systemcontrol_icsr_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ICSR.reg &= ~SystemControl_ICSR_VECTPENDING(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_ICSR_VECTPENDING_bf(const void *const hw, hri_systemcontrol_icsr_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ICSR.reg ^= SystemControl_ICSR_VECTPENDING(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_icsr_reg_t hri_systemcontrol_read_ICSR_VECTPENDING_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->ICSR.reg;
	tmp = (tmp & SystemControl_ICSR_VECTPENDING_Msk) >> 12;
	return tmp;
}

static inline void hri_systemcontrol_set_ICSR_reg(const void *const hw, hri_systemcontrol_icsr_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ICSR.reg |= mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_icsr_reg_t hri_systemcontrol_get_ICSR_reg(const void *const            hw,
                                                                          hri_systemcontrol_icsr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->ICSR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_systemcontrol_write_ICSR_reg(const void *const hw, hri_systemcontrol_icsr_reg_t data)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ICSR.reg = data;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_ICSR_reg(const void *const hw, hri_systemcontrol_icsr_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ICSR.reg &= ~mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_ICSR_reg(const void *const hw, hri_systemcontrol_icsr_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->ICSR.reg ^= mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_icsr_reg_t hri_systemcontrol_read_ICSR_reg(const void *const hw)
{
	return ((Systemcontrol *)hw)->ICSR.reg;
}

static inline void hri_systemcontrol_set_VTOR_TBLOFF_bf(const void *const hw, hri_systemcontrol_vtor_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->VTOR.reg |= SystemControl_VTOR_TBLOFF(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_vtor_reg_t hri_systemcontrol_get_VTOR_TBLOFF_bf(const void *const            hw,
                                                                                hri_systemcontrol_vtor_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->VTOR.reg;
	tmp = (tmp & SystemControl_VTOR_TBLOFF(mask)) >> 7;
	return tmp;
}

static inline void hri_systemcontrol_write_VTOR_TBLOFF_bf(const void *const hw, hri_systemcontrol_vtor_reg_t data)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->VTOR.reg;
	tmp &= ~SystemControl_VTOR_TBLOFF_Msk;
	tmp |= SystemControl_VTOR_TBLOFF(data);
	((Systemcontrol *)hw)->VTOR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_VTOR_TBLOFF_bf(const void *const hw, hri_systemcontrol_vtor_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->VTOR.reg &= ~SystemControl_VTOR_TBLOFF(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_VTOR_TBLOFF_bf(const void *const hw, hri_systemcontrol_vtor_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->VTOR.reg ^= SystemControl_VTOR_TBLOFF(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_vtor_reg_t hri_systemcontrol_read_VTOR_TBLOFF_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->VTOR.reg;
	tmp = (tmp & SystemControl_VTOR_TBLOFF_Msk) >> 7;
	return tmp;
}

static inline void hri_systemcontrol_set_VTOR_reg(const void *const hw, hri_systemcontrol_vtor_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->VTOR.reg |= mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_vtor_reg_t hri_systemcontrol_get_VTOR_reg(const void *const            hw,
                                                                          hri_systemcontrol_vtor_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->VTOR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_systemcontrol_write_VTOR_reg(const void *const hw, hri_systemcontrol_vtor_reg_t data)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->VTOR.reg = data;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_VTOR_reg(const void *const hw, hri_systemcontrol_vtor_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->VTOR.reg &= ~mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_VTOR_reg(const void *const hw, hri_systemcontrol_vtor_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->VTOR.reg ^= mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_vtor_reg_t hri_systemcontrol_read_VTOR_reg(const void *const hw)
{
	return ((Systemcontrol *)hw)->VTOR.reg;
}

static inline void hri_systemcontrol_set_AIRCR_VECTCLRACTIVE_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->AIRCR.reg |= SystemControl_AIRCR_VECTCLRACTIVE;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_AIRCR_VECTCLRACTIVE_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->AIRCR.reg;
	tmp = (tmp & SystemControl_AIRCR_VECTCLRACTIVE) >> 1;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_AIRCR_VECTCLRACTIVE_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->AIRCR.reg;
	tmp &= ~SystemControl_AIRCR_VECTCLRACTIVE;
	tmp |= value << 1;
	((Systemcontrol *)hw)->AIRCR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_AIRCR_VECTCLRACTIVE_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->AIRCR.reg &= ~SystemControl_AIRCR_VECTCLRACTIVE;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_AIRCR_VECTCLRACTIVE_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->AIRCR.reg ^= SystemControl_AIRCR_VECTCLRACTIVE;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_AIRCR_SYSRESETREQ_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->AIRCR.reg |= SystemControl_AIRCR_SYSRESETREQ;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_AIRCR_SYSRESETREQ_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->AIRCR.reg;
	tmp = (tmp & SystemControl_AIRCR_SYSRESETREQ) >> 2;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_AIRCR_SYSRESETREQ_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->AIRCR.reg;
	tmp &= ~SystemControl_AIRCR_SYSRESETREQ;
	tmp |= value << 2;
	((Systemcontrol *)hw)->AIRCR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_AIRCR_SYSRESETREQ_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->AIRCR.reg &= ~SystemControl_AIRCR_SYSRESETREQ;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_AIRCR_SYSRESETREQ_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->AIRCR.reg ^= SystemControl_AIRCR_SYSRESETREQ;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_AIRCR_ENDIANNESS_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->AIRCR.reg |= SystemControl_AIRCR_ENDIANNESS;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_AIRCR_ENDIANNESS_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->AIRCR.reg;
	tmp = (tmp & SystemControl_AIRCR_ENDIANNESS) >> 15;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_AIRCR_ENDIANNESS_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->AIRCR.reg;
	tmp &= ~SystemControl_AIRCR_ENDIANNESS;
	tmp |= value << 15;
	((Systemcontrol *)hw)->AIRCR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_AIRCR_ENDIANNESS_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->AIRCR.reg &= ~SystemControl_AIRCR_ENDIANNESS;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_AIRCR_ENDIANNESS_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->AIRCR.reg ^= SystemControl_AIRCR_ENDIANNESS;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_AIRCR_VECTKEY_bf(const void *const hw, hri_systemcontrol_aircr_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->AIRCR.reg |= SystemControl_AIRCR_VECTKEY(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_aircr_reg_t hri_systemcontrol_get_AIRCR_VECTKEY_bf(const void *const             hw,
                                                                                   hri_systemcontrol_aircr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->AIRCR.reg;
	tmp = (tmp & SystemControl_AIRCR_VECTKEY(mask)) >> 16;
	return tmp;
}

static inline void hri_systemcontrol_write_AIRCR_VECTKEY_bf(const void *const hw, hri_systemcontrol_aircr_reg_t data)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->AIRCR.reg;
	tmp &= ~SystemControl_AIRCR_VECTKEY_Msk;
	tmp |= SystemControl_AIRCR_VECTKEY(data);
	((Systemcontrol *)hw)->AIRCR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_AIRCR_VECTKEY_bf(const void *const hw, hri_systemcontrol_aircr_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->AIRCR.reg &= ~SystemControl_AIRCR_VECTKEY(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_AIRCR_VECTKEY_bf(const void *const hw, hri_systemcontrol_aircr_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->AIRCR.reg ^= SystemControl_AIRCR_VECTKEY(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_aircr_reg_t hri_systemcontrol_read_AIRCR_VECTKEY_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->AIRCR.reg;
	tmp = (tmp & SystemControl_AIRCR_VECTKEY_Msk) >> 16;
	return tmp;
}

static inline void hri_systemcontrol_set_AIRCR_reg(const void *const hw, hri_systemcontrol_aircr_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->AIRCR.reg |= mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_aircr_reg_t hri_systemcontrol_get_AIRCR_reg(const void *const             hw,
                                                                            hri_systemcontrol_aircr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->AIRCR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_systemcontrol_write_AIRCR_reg(const void *const hw, hri_systemcontrol_aircr_reg_t data)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->AIRCR.reg = data;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_AIRCR_reg(const void *const hw, hri_systemcontrol_aircr_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->AIRCR.reg &= ~mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_AIRCR_reg(const void *const hw, hri_systemcontrol_aircr_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->AIRCR.reg ^= mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_aircr_reg_t hri_systemcontrol_read_AIRCR_reg(const void *const hw)
{
	return ((Systemcontrol *)hw)->AIRCR.reg;
}

static inline void hri_systemcontrol_set_SCR_SLEEPONEXIT_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SCR.reg |= SystemControl_SCR_SLEEPONEXIT;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_SCR_SLEEPONEXIT_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->SCR.reg;
	tmp = (tmp & SystemControl_SCR_SLEEPONEXIT) >> 1;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_SCR_SLEEPONEXIT_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->SCR.reg;
	tmp &= ~SystemControl_SCR_SLEEPONEXIT;
	tmp |= value << 1;
	((Systemcontrol *)hw)->SCR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_SCR_SLEEPONEXIT_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SCR.reg &= ~SystemControl_SCR_SLEEPONEXIT;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_SCR_SLEEPONEXIT_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SCR.reg ^= SystemControl_SCR_SLEEPONEXIT;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_SCR_SLEEPDEEP_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SCR.reg |= SystemControl_SCR_SLEEPDEEP;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_SCR_SLEEPDEEP_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->SCR.reg;
	tmp = (tmp & SystemControl_SCR_SLEEPDEEP) >> 2;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_SCR_SLEEPDEEP_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->SCR.reg;
	tmp &= ~SystemControl_SCR_SLEEPDEEP;
	tmp |= value << 2;
	((Systemcontrol *)hw)->SCR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_SCR_SLEEPDEEP_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SCR.reg &= ~SystemControl_SCR_SLEEPDEEP;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_SCR_SLEEPDEEP_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SCR.reg ^= SystemControl_SCR_SLEEPDEEP;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_SCR_SEVONPEND_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SCR.reg |= SystemControl_SCR_SEVONPEND;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_SCR_SEVONPEND_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->SCR.reg;
	tmp = (tmp & SystemControl_SCR_SEVONPEND) >> 4;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_SCR_SEVONPEND_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->SCR.reg;
	tmp &= ~SystemControl_SCR_SEVONPEND;
	tmp |= value << 4;
	((Systemcontrol *)hw)->SCR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_SCR_SEVONPEND_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SCR.reg &= ~SystemControl_SCR_SEVONPEND;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_SCR_SEVONPEND_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SCR.reg ^= SystemControl_SCR_SEVONPEND;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_SCR_reg(const void *const hw, hri_systemcontrol_scr_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SCR.reg |= mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_scr_reg_t hri_systemcontrol_get_SCR_reg(const void *const           hw,
                                                                        hri_systemcontrol_scr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->SCR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_systemcontrol_write_SCR_reg(const void *const hw, hri_systemcontrol_scr_reg_t data)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SCR.reg = data;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_SCR_reg(const void *const hw, hri_systemcontrol_scr_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SCR.reg &= ~mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_SCR_reg(const void *const hw, hri_systemcontrol_scr_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SCR.reg ^= mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_scr_reg_t hri_systemcontrol_read_SCR_reg(const void *const hw)
{
	return ((Systemcontrol *)hw)->SCR.reg;
}

static inline void hri_systemcontrol_set_CCR_UNALIGN_TRP_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CCR.reg |= SystemControl_CCR_UNALIGN_TRP;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_CCR_UNALIGN_TRP_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->CCR.reg;
	tmp = (tmp & SystemControl_CCR_UNALIGN_TRP) >> 3;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_CCR_UNALIGN_TRP_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->CCR.reg;
	tmp &= ~SystemControl_CCR_UNALIGN_TRP;
	tmp |= value << 3;
	((Systemcontrol *)hw)->CCR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_CCR_UNALIGN_TRP_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CCR.reg &= ~SystemControl_CCR_UNALIGN_TRP;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_CCR_UNALIGN_TRP_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CCR.reg ^= SystemControl_CCR_UNALIGN_TRP;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_CCR_STKALIGN_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CCR.reg |= SystemControl_CCR_STKALIGN;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_systemcontrol_get_CCR_STKALIGN_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->CCR.reg;
	tmp = (tmp & SystemControl_CCR_STKALIGN) >> 9;
	return (bool)tmp;
}

static inline void hri_systemcontrol_write_CCR_STKALIGN_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->CCR.reg;
	tmp &= ~SystemControl_CCR_STKALIGN;
	tmp |= value << 9;
	((Systemcontrol *)hw)->CCR.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_CCR_STKALIGN_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CCR.reg &= ~SystemControl_CCR_STKALIGN;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_CCR_STKALIGN_bit(const void *const hw)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CCR.reg ^= SystemControl_CCR_STKALIGN;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_set_CCR_reg(const void *const hw, hri_systemcontrol_ccr_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CCR.reg |= mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_ccr_reg_t hri_systemcontrol_get_CCR_reg(const void *const           hw,
                                                                        hri_systemcontrol_ccr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->CCR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_systemcontrol_write_CCR_reg(const void *const hw, hri_systemcontrol_ccr_reg_t data)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CCR.reg = data;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_CCR_reg(const void *const hw, hri_systemcontrol_ccr_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CCR.reg &= ~mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_CCR_reg(const void *const hw, hri_systemcontrol_ccr_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->CCR.reg ^= mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_ccr_reg_t hri_systemcontrol_read_CCR_reg(const void *const hw)
{
	return ((Systemcontrol *)hw)->CCR.reg;
}

static inline void hri_systemcontrol_set_SHPR2_PRI_11_bf(const void *const hw, hri_systemcontrol_shpr2_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHPR2.reg |= SystemControl_SHPR2_PRI_11(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_shpr2_reg_t hri_systemcontrol_get_SHPR2_PRI_11_bf(const void *const             hw,
                                                                                  hri_systemcontrol_shpr2_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->SHPR2.reg;
	tmp = (tmp & SystemControl_SHPR2_PRI_11(mask)) >> 24;
	return tmp;
}

static inline void hri_systemcontrol_write_SHPR2_PRI_11_bf(const void *const hw, hri_systemcontrol_shpr2_reg_t data)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->SHPR2.reg;
	tmp &= ~SystemControl_SHPR2_PRI_11_Msk;
	tmp |= SystemControl_SHPR2_PRI_11(data);
	((Systemcontrol *)hw)->SHPR2.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_SHPR2_PRI_11_bf(const void *const hw, hri_systemcontrol_shpr2_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHPR2.reg &= ~SystemControl_SHPR2_PRI_11(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_SHPR2_PRI_11_bf(const void *const hw, hri_systemcontrol_shpr2_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHPR2.reg ^= SystemControl_SHPR2_PRI_11(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_shpr2_reg_t hri_systemcontrol_read_SHPR2_PRI_11_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->SHPR2.reg;
	tmp = (tmp & SystemControl_SHPR2_PRI_11_Msk) >> 24;
	return tmp;
}

static inline void hri_systemcontrol_set_SHPR2_reg(const void *const hw, hri_systemcontrol_shpr2_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHPR2.reg |= mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_shpr2_reg_t hri_systemcontrol_get_SHPR2_reg(const void *const             hw,
                                                                            hri_systemcontrol_shpr2_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->SHPR2.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_systemcontrol_write_SHPR2_reg(const void *const hw, hri_systemcontrol_shpr2_reg_t data)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHPR2.reg = data;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_SHPR2_reg(const void *const hw, hri_systemcontrol_shpr2_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHPR2.reg &= ~mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_SHPR2_reg(const void *const hw, hri_systemcontrol_shpr2_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHPR2.reg ^= mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_shpr2_reg_t hri_systemcontrol_read_SHPR2_reg(const void *const hw)
{
	return ((Systemcontrol *)hw)->SHPR2.reg;
}

static inline void hri_systemcontrol_set_SHPR3_PRI_14_bf(const void *const hw, hri_systemcontrol_shpr3_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHPR3.reg |= SystemControl_SHPR3_PRI_14(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_shpr3_reg_t hri_systemcontrol_get_SHPR3_PRI_14_bf(const void *const             hw,
                                                                                  hri_systemcontrol_shpr3_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->SHPR3.reg;
	tmp = (tmp & SystemControl_SHPR3_PRI_14(mask)) >> 16;
	return tmp;
}

static inline void hri_systemcontrol_write_SHPR3_PRI_14_bf(const void *const hw, hri_systemcontrol_shpr3_reg_t data)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->SHPR3.reg;
	tmp &= ~SystemControl_SHPR3_PRI_14_Msk;
	tmp |= SystemControl_SHPR3_PRI_14(data);
	((Systemcontrol *)hw)->SHPR3.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_SHPR3_PRI_14_bf(const void *const hw, hri_systemcontrol_shpr3_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHPR3.reg &= ~SystemControl_SHPR3_PRI_14(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_SHPR3_PRI_14_bf(const void *const hw, hri_systemcontrol_shpr3_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHPR3.reg ^= SystemControl_SHPR3_PRI_14(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_shpr3_reg_t hri_systemcontrol_read_SHPR3_PRI_14_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->SHPR3.reg;
	tmp = (tmp & SystemControl_SHPR3_PRI_14_Msk) >> 16;
	return tmp;
}

static inline void hri_systemcontrol_set_SHPR3_PRI_15_bf(const void *const hw, hri_systemcontrol_shpr3_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHPR3.reg |= SystemControl_SHPR3_PRI_15(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_shpr3_reg_t hri_systemcontrol_get_SHPR3_PRI_15_bf(const void *const             hw,
                                                                                  hri_systemcontrol_shpr3_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->SHPR3.reg;
	tmp = (tmp & SystemControl_SHPR3_PRI_15(mask)) >> 24;
	return tmp;
}

static inline void hri_systemcontrol_write_SHPR3_PRI_15_bf(const void *const hw, hri_systemcontrol_shpr3_reg_t data)
{
	uint32_t tmp;
	SystemControl_CRITICAL_SECTION_ENTER();
	tmp = ((Systemcontrol *)hw)->SHPR3.reg;
	tmp &= ~SystemControl_SHPR3_PRI_15_Msk;
	tmp |= SystemControl_SHPR3_PRI_15(data);
	((Systemcontrol *)hw)->SHPR3.reg = tmp;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_SHPR3_PRI_15_bf(const void *const hw, hri_systemcontrol_shpr3_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHPR3.reg &= ~SystemControl_SHPR3_PRI_15(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_SHPR3_PRI_15_bf(const void *const hw, hri_systemcontrol_shpr3_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHPR3.reg ^= SystemControl_SHPR3_PRI_15(mask);
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_shpr3_reg_t hri_systemcontrol_read_SHPR3_PRI_15_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->SHPR3.reg;
	tmp = (tmp & SystemControl_SHPR3_PRI_15_Msk) >> 24;
	return tmp;
}

static inline void hri_systemcontrol_set_SHPR3_reg(const void *const hw, hri_systemcontrol_shpr3_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHPR3.reg |= mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_shpr3_reg_t hri_systemcontrol_get_SHPR3_reg(const void *const             hw,
                                                                            hri_systemcontrol_shpr3_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Systemcontrol *)hw)->SHPR3.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_systemcontrol_write_SHPR3_reg(const void *const hw, hri_systemcontrol_shpr3_reg_t data)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHPR3.reg = data;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_clear_SHPR3_reg(const void *const hw, hri_systemcontrol_shpr3_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHPR3.reg &= ~mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline void hri_systemcontrol_toggle_SHPR3_reg(const void *const hw, hri_systemcontrol_shpr3_reg_t mask)
{
	SystemControl_CRITICAL_SECTION_ENTER();
	((Systemcontrol *)hw)->SHPR3.reg ^= mask;
	SystemControl_CRITICAL_SECTION_LEAVE();
}

static inline hri_systemcontrol_shpr3_reg_t hri_systemcontrol_read_SHPR3_reg(const void *const hw)
{
	return ((Systemcontrol *)hw)->SHPR3.reg;
}

#ifdef __cplusplus
}
#endif

#endif /* _HRI_SystemControl_D21_H_INCLUDED */
#endif /* _SAMD21_SystemControl_COMPONENT_ */
