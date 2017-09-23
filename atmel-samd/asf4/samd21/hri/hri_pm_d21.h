/**
 * \file
 *
 * \brief SAM PM
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

#ifdef _SAMD21_PM_COMPONENT_
#ifndef _HRI_PM_D21_H_INCLUDED_
#define _HRI_PM_D21_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <hal_atomic.h>

#if defined(ENABLE_PM_CRITICAL_SECTIONS)
#define PM_CRITICAL_SECTION_ENTER() CRITICAL_SECTION_ENTER()
#define PM_CRITICAL_SECTION_LEAVE() CRITICAL_SECTION_LEAVE()
#else
#define PM_CRITICAL_SECTION_ENTER()
#define PM_CRITICAL_SECTION_LEAVE()
#endif

typedef uint32_t hri_pm_ahbmask_reg_t;
typedef uint32_t hri_pm_apbamask_reg_t;
typedef uint32_t hri_pm_apbbmask_reg_t;
typedef uint32_t hri_pm_apbcmask_reg_t;
typedef uint8_t  hri_pm_apbasel_reg_t;
typedef uint8_t  hri_pm_apbbsel_reg_t;
typedef uint8_t  hri_pm_apbcsel_reg_t;
typedef uint8_t  hri_pm_cpusel_reg_t;
typedef uint8_t  hri_pm_ctrl_reg_t;
typedef uint8_t  hri_pm_intenset_reg_t;
typedef uint8_t  hri_pm_intflag_reg_t;
typedef uint8_t  hri_pm_rcause_reg_t;
typedef uint8_t  hri_pm_sleep_reg_t;

static inline void hri_pm_set_INTEN_CKRDY_bit(const void *const hw)
{
	((Pm *)hw)->INTENSET.reg = PM_INTENSET_CKRDY;
}

static inline bool hri_pm_get_INTEN_CKRDY_bit(const void *const hw)
{
	return (((Pm *)hw)->INTENSET.reg & PM_INTENSET_CKRDY) >> PM_INTENSET_CKRDY_Pos;
}

static inline void hri_pm_write_INTEN_CKRDY_bit(const void *const hw, bool value)
{
	if (value == 0x0) {
		((Pm *)hw)->INTENCLR.reg = PM_INTENSET_CKRDY;
	} else {
		((Pm *)hw)->INTENSET.reg = PM_INTENSET_CKRDY;
	}
}

static inline void hri_pm_clear_INTEN_CKRDY_bit(const void *const hw)
{
	((Pm *)hw)->INTENCLR.reg = PM_INTENSET_CKRDY;
}

static inline void hri_pm_set_INTEN_reg(const void *const hw, hri_pm_intenset_reg_t mask)
{
	((Pm *)hw)->INTENSET.reg = mask;
}

static inline hri_pm_intenset_reg_t hri_pm_get_INTEN_reg(const void *const hw, hri_pm_intenset_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Pm *)hw)->INTENSET.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_pm_intenset_reg_t hri_pm_read_INTEN_reg(const void *const hw)
{
	return ((Pm *)hw)->INTENSET.reg;
}

static inline void hri_pm_write_INTEN_reg(const void *const hw, hri_pm_intenset_reg_t data)
{
	((Pm *)hw)->INTENSET.reg = data;
	((Pm *)hw)->INTENCLR.reg = ~data;
}

static inline void hri_pm_clear_INTEN_reg(const void *const hw, hri_pm_intenset_reg_t mask)
{
	((Pm *)hw)->INTENCLR.reg = mask;
}

static inline bool hri_pm_get_INTFLAG_CKRDY_bit(const void *const hw)
{
	return (((Pm *)hw)->INTFLAG.reg & PM_INTFLAG_CKRDY) >> PM_INTFLAG_CKRDY_Pos;
}

static inline void hri_pm_clear_INTFLAG_CKRDY_bit(const void *const hw)
{
	((Pm *)hw)->INTFLAG.reg = PM_INTFLAG_CKRDY;
}

static inline bool hri_pm_get_interrupt_CKRDY_bit(const void *const hw)
{
	return (((Pm *)hw)->INTFLAG.reg & PM_INTFLAG_CKRDY) >> PM_INTFLAG_CKRDY_Pos;
}

static inline void hri_pm_clear_interrupt_CKRDY_bit(const void *const hw)
{
	((Pm *)hw)->INTFLAG.reg = PM_INTFLAG_CKRDY;
}

static inline hri_pm_intflag_reg_t hri_pm_get_INTFLAG_reg(const void *const hw, hri_pm_intflag_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Pm *)hw)->INTFLAG.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_pm_intflag_reg_t hri_pm_read_INTFLAG_reg(const void *const hw)
{
	return ((Pm *)hw)->INTFLAG.reg;
}

static inline void hri_pm_clear_INTFLAG_reg(const void *const hw, hri_pm_intflag_reg_t mask)
{
	((Pm *)hw)->INTFLAG.reg = mask;
}

static inline void hri_pm_set_CTRL_reg(const void *const hw, hri_pm_ctrl_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->CTRL.reg |= mask;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline hri_pm_ctrl_reg_t hri_pm_get_CTRL_reg(const void *const hw, hri_pm_ctrl_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Pm *)hw)->CTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_pm_write_CTRL_reg(const void *const hw, hri_pm_ctrl_reg_t data)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->CTRL.reg = data;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_CTRL_reg(const void *const hw, hri_pm_ctrl_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->CTRL.reg &= ~mask;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_CTRL_reg(const void *const hw, hri_pm_ctrl_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->CTRL.reg ^= mask;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline hri_pm_ctrl_reg_t hri_pm_read_CTRL_reg(const void *const hw)
{
	return ((Pm *)hw)->CTRL.reg;
}

static inline void hri_pm_set_SLEEP_IDLE_bf(const void *const hw, hri_pm_sleep_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->SLEEP.reg |= PM_SLEEP_IDLE(mask);
	PM_CRITICAL_SECTION_LEAVE();
}

static inline hri_pm_sleep_reg_t hri_pm_get_SLEEP_IDLE_bf(const void *const hw, hri_pm_sleep_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Pm *)hw)->SLEEP.reg;
	tmp = (tmp & PM_SLEEP_IDLE(mask)) >> PM_SLEEP_IDLE_Pos;
	return tmp;
}

static inline void hri_pm_write_SLEEP_IDLE_bf(const void *const hw, hri_pm_sleep_reg_t data)
{
	uint8_t tmp;
	PM_CRITICAL_SECTION_ENTER();
	tmp = ((Pm *)hw)->SLEEP.reg;
	tmp &= ~PM_SLEEP_IDLE_Msk;
	tmp |= PM_SLEEP_IDLE(data);
	((Pm *)hw)->SLEEP.reg = tmp;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_SLEEP_IDLE_bf(const void *const hw, hri_pm_sleep_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->SLEEP.reg &= ~PM_SLEEP_IDLE(mask);
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_SLEEP_IDLE_bf(const void *const hw, hri_pm_sleep_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->SLEEP.reg ^= PM_SLEEP_IDLE(mask);
	PM_CRITICAL_SECTION_LEAVE();
}

static inline hri_pm_sleep_reg_t hri_pm_read_SLEEP_IDLE_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Pm *)hw)->SLEEP.reg;
	tmp = (tmp & PM_SLEEP_IDLE_Msk) >> PM_SLEEP_IDLE_Pos;
	return tmp;
}

static inline void hri_pm_set_SLEEP_reg(const void *const hw, hri_pm_sleep_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->SLEEP.reg |= mask;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline hri_pm_sleep_reg_t hri_pm_get_SLEEP_reg(const void *const hw, hri_pm_sleep_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Pm *)hw)->SLEEP.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_pm_write_SLEEP_reg(const void *const hw, hri_pm_sleep_reg_t data)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->SLEEP.reg = data;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_SLEEP_reg(const void *const hw, hri_pm_sleep_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->SLEEP.reg &= ~mask;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_SLEEP_reg(const void *const hw, hri_pm_sleep_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->SLEEP.reg ^= mask;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline hri_pm_sleep_reg_t hri_pm_read_SLEEP_reg(const void *const hw)
{
	return ((Pm *)hw)->SLEEP.reg;
}

static inline void hri_pm_set_CPUSEL_CPUDIV_bf(const void *const hw, hri_pm_cpusel_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->CPUSEL.reg |= PM_CPUSEL_CPUDIV(mask);
	PM_CRITICAL_SECTION_LEAVE();
}

static inline hri_pm_cpusel_reg_t hri_pm_get_CPUSEL_CPUDIV_bf(const void *const hw, hri_pm_cpusel_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Pm *)hw)->CPUSEL.reg;
	tmp = (tmp & PM_CPUSEL_CPUDIV(mask)) >> PM_CPUSEL_CPUDIV_Pos;
	return tmp;
}

static inline void hri_pm_write_CPUSEL_CPUDIV_bf(const void *const hw, hri_pm_cpusel_reg_t data)
{
	uint8_t tmp;
	PM_CRITICAL_SECTION_ENTER();
	tmp = ((Pm *)hw)->CPUSEL.reg;
	tmp &= ~PM_CPUSEL_CPUDIV_Msk;
	tmp |= PM_CPUSEL_CPUDIV(data);
	((Pm *)hw)->CPUSEL.reg = tmp;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_CPUSEL_CPUDIV_bf(const void *const hw, hri_pm_cpusel_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->CPUSEL.reg &= ~PM_CPUSEL_CPUDIV(mask);
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_CPUSEL_CPUDIV_bf(const void *const hw, hri_pm_cpusel_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->CPUSEL.reg ^= PM_CPUSEL_CPUDIV(mask);
	PM_CRITICAL_SECTION_LEAVE();
}

static inline hri_pm_cpusel_reg_t hri_pm_read_CPUSEL_CPUDIV_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Pm *)hw)->CPUSEL.reg;
	tmp = (tmp & PM_CPUSEL_CPUDIV_Msk) >> PM_CPUSEL_CPUDIV_Pos;
	return tmp;
}

static inline void hri_pm_set_CPUSEL_reg(const void *const hw, hri_pm_cpusel_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->CPUSEL.reg |= mask;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline hri_pm_cpusel_reg_t hri_pm_get_CPUSEL_reg(const void *const hw, hri_pm_cpusel_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Pm *)hw)->CPUSEL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_pm_write_CPUSEL_reg(const void *const hw, hri_pm_cpusel_reg_t data)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->CPUSEL.reg = data;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_CPUSEL_reg(const void *const hw, hri_pm_cpusel_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->CPUSEL.reg &= ~mask;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_CPUSEL_reg(const void *const hw, hri_pm_cpusel_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->CPUSEL.reg ^= mask;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline hri_pm_cpusel_reg_t hri_pm_read_CPUSEL_reg(const void *const hw)
{
	return ((Pm *)hw)->CPUSEL.reg;
}

static inline void hri_pm_set_APBASEL_APBADIV_bf(const void *const hw, hri_pm_apbasel_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBASEL.reg |= PM_APBASEL_APBADIV(mask);
	PM_CRITICAL_SECTION_LEAVE();
}

static inline hri_pm_apbasel_reg_t hri_pm_get_APBASEL_APBADIV_bf(const void *const hw, hri_pm_apbasel_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Pm *)hw)->APBASEL.reg;
	tmp = (tmp & PM_APBASEL_APBADIV(mask)) >> PM_APBASEL_APBADIV_Pos;
	return tmp;
}

static inline void hri_pm_write_APBASEL_APBADIV_bf(const void *const hw, hri_pm_apbasel_reg_t data)
{
	uint8_t tmp;
	PM_CRITICAL_SECTION_ENTER();
	tmp = ((Pm *)hw)->APBASEL.reg;
	tmp &= ~PM_APBASEL_APBADIV_Msk;
	tmp |= PM_APBASEL_APBADIV(data);
	((Pm *)hw)->APBASEL.reg = tmp;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_APBASEL_APBADIV_bf(const void *const hw, hri_pm_apbasel_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBASEL.reg &= ~PM_APBASEL_APBADIV(mask);
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_APBASEL_APBADIV_bf(const void *const hw, hri_pm_apbasel_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBASEL.reg ^= PM_APBASEL_APBADIV(mask);
	PM_CRITICAL_SECTION_LEAVE();
}

static inline hri_pm_apbasel_reg_t hri_pm_read_APBASEL_APBADIV_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Pm *)hw)->APBASEL.reg;
	tmp = (tmp & PM_APBASEL_APBADIV_Msk) >> PM_APBASEL_APBADIV_Pos;
	return tmp;
}

static inline void hri_pm_set_APBASEL_reg(const void *const hw, hri_pm_apbasel_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBASEL.reg |= mask;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline hri_pm_apbasel_reg_t hri_pm_get_APBASEL_reg(const void *const hw, hri_pm_apbasel_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Pm *)hw)->APBASEL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_pm_write_APBASEL_reg(const void *const hw, hri_pm_apbasel_reg_t data)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBASEL.reg = data;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_APBASEL_reg(const void *const hw, hri_pm_apbasel_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBASEL.reg &= ~mask;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_APBASEL_reg(const void *const hw, hri_pm_apbasel_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBASEL.reg ^= mask;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline hri_pm_apbasel_reg_t hri_pm_read_APBASEL_reg(const void *const hw)
{
	return ((Pm *)hw)->APBASEL.reg;
}

static inline void hri_pm_set_APBBSEL_APBBDIV_bf(const void *const hw, hri_pm_apbbsel_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBBSEL.reg |= PM_APBBSEL_APBBDIV(mask);
	PM_CRITICAL_SECTION_LEAVE();
}

static inline hri_pm_apbbsel_reg_t hri_pm_get_APBBSEL_APBBDIV_bf(const void *const hw, hri_pm_apbbsel_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Pm *)hw)->APBBSEL.reg;
	tmp = (tmp & PM_APBBSEL_APBBDIV(mask)) >> PM_APBBSEL_APBBDIV_Pos;
	return tmp;
}

static inline void hri_pm_write_APBBSEL_APBBDIV_bf(const void *const hw, hri_pm_apbbsel_reg_t data)
{
	uint8_t tmp;
	PM_CRITICAL_SECTION_ENTER();
	tmp = ((Pm *)hw)->APBBSEL.reg;
	tmp &= ~PM_APBBSEL_APBBDIV_Msk;
	tmp |= PM_APBBSEL_APBBDIV(data);
	((Pm *)hw)->APBBSEL.reg = tmp;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_APBBSEL_APBBDIV_bf(const void *const hw, hri_pm_apbbsel_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBBSEL.reg &= ~PM_APBBSEL_APBBDIV(mask);
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_APBBSEL_APBBDIV_bf(const void *const hw, hri_pm_apbbsel_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBBSEL.reg ^= PM_APBBSEL_APBBDIV(mask);
	PM_CRITICAL_SECTION_LEAVE();
}

static inline hri_pm_apbbsel_reg_t hri_pm_read_APBBSEL_APBBDIV_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Pm *)hw)->APBBSEL.reg;
	tmp = (tmp & PM_APBBSEL_APBBDIV_Msk) >> PM_APBBSEL_APBBDIV_Pos;
	return tmp;
}

static inline void hri_pm_set_APBBSEL_reg(const void *const hw, hri_pm_apbbsel_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBBSEL.reg |= mask;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline hri_pm_apbbsel_reg_t hri_pm_get_APBBSEL_reg(const void *const hw, hri_pm_apbbsel_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Pm *)hw)->APBBSEL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_pm_write_APBBSEL_reg(const void *const hw, hri_pm_apbbsel_reg_t data)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBBSEL.reg = data;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_APBBSEL_reg(const void *const hw, hri_pm_apbbsel_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBBSEL.reg &= ~mask;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_APBBSEL_reg(const void *const hw, hri_pm_apbbsel_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBBSEL.reg ^= mask;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline hri_pm_apbbsel_reg_t hri_pm_read_APBBSEL_reg(const void *const hw)
{
	return ((Pm *)hw)->APBBSEL.reg;
}

static inline void hri_pm_set_APBCSEL_APBCDIV_bf(const void *const hw, hri_pm_apbcsel_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCSEL.reg |= PM_APBCSEL_APBCDIV(mask);
	PM_CRITICAL_SECTION_LEAVE();
}

static inline hri_pm_apbcsel_reg_t hri_pm_get_APBCSEL_APBCDIV_bf(const void *const hw, hri_pm_apbcsel_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Pm *)hw)->APBCSEL.reg;
	tmp = (tmp & PM_APBCSEL_APBCDIV(mask)) >> PM_APBCSEL_APBCDIV_Pos;
	return tmp;
}

static inline void hri_pm_write_APBCSEL_APBCDIV_bf(const void *const hw, hri_pm_apbcsel_reg_t data)
{
	uint8_t tmp;
	PM_CRITICAL_SECTION_ENTER();
	tmp = ((Pm *)hw)->APBCSEL.reg;
	tmp &= ~PM_APBCSEL_APBCDIV_Msk;
	tmp |= PM_APBCSEL_APBCDIV(data);
	((Pm *)hw)->APBCSEL.reg = tmp;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_APBCSEL_APBCDIV_bf(const void *const hw, hri_pm_apbcsel_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCSEL.reg &= ~PM_APBCSEL_APBCDIV(mask);
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_APBCSEL_APBCDIV_bf(const void *const hw, hri_pm_apbcsel_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCSEL.reg ^= PM_APBCSEL_APBCDIV(mask);
	PM_CRITICAL_SECTION_LEAVE();
}

static inline hri_pm_apbcsel_reg_t hri_pm_read_APBCSEL_APBCDIV_bf(const void *const hw)
{
	uint8_t tmp;
	tmp = ((Pm *)hw)->APBCSEL.reg;
	tmp = (tmp & PM_APBCSEL_APBCDIV_Msk) >> PM_APBCSEL_APBCDIV_Pos;
	return tmp;
}

static inline void hri_pm_set_APBCSEL_reg(const void *const hw, hri_pm_apbcsel_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCSEL.reg |= mask;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline hri_pm_apbcsel_reg_t hri_pm_get_APBCSEL_reg(const void *const hw, hri_pm_apbcsel_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Pm *)hw)->APBCSEL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_pm_write_APBCSEL_reg(const void *const hw, hri_pm_apbcsel_reg_t data)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCSEL.reg = data;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_APBCSEL_reg(const void *const hw, hri_pm_apbcsel_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCSEL.reg &= ~mask;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_APBCSEL_reg(const void *const hw, hri_pm_apbcsel_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCSEL.reg ^= mask;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline hri_pm_apbcsel_reg_t hri_pm_read_APBCSEL_reg(const void *const hw)
{
	return ((Pm *)hw)->APBCSEL.reg;
}

static inline void hri_pm_set_AHBMASK_HPB0_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->AHBMASK.reg |= PM_AHBMASK_HPB0;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pm_get_AHBMASK_HPB0_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pm *)hw)->AHBMASK.reg;
	tmp = (tmp & PM_AHBMASK_HPB0) >> PM_AHBMASK_HPB0_Pos;
	return (bool)tmp;
}

static inline void hri_pm_write_AHBMASK_HPB0_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PM_CRITICAL_SECTION_ENTER();
	tmp = ((Pm *)hw)->AHBMASK.reg;
	tmp &= ~PM_AHBMASK_HPB0;
	tmp |= value << PM_AHBMASK_HPB0_Pos;
	((Pm *)hw)->AHBMASK.reg = tmp;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_AHBMASK_HPB0_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->AHBMASK.reg &= ~PM_AHBMASK_HPB0;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_AHBMASK_HPB0_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->AHBMASK.reg ^= PM_AHBMASK_HPB0;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_set_AHBMASK_HPB1_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->AHBMASK.reg |= PM_AHBMASK_HPB1;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pm_get_AHBMASK_HPB1_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pm *)hw)->AHBMASK.reg;
	tmp = (tmp & PM_AHBMASK_HPB1) >> PM_AHBMASK_HPB1_Pos;
	return (bool)tmp;
}

static inline void hri_pm_write_AHBMASK_HPB1_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PM_CRITICAL_SECTION_ENTER();
	tmp = ((Pm *)hw)->AHBMASK.reg;
	tmp &= ~PM_AHBMASK_HPB1;
	tmp |= value << PM_AHBMASK_HPB1_Pos;
	((Pm *)hw)->AHBMASK.reg = tmp;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_AHBMASK_HPB1_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->AHBMASK.reg &= ~PM_AHBMASK_HPB1;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_AHBMASK_HPB1_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->AHBMASK.reg ^= PM_AHBMASK_HPB1;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_set_AHBMASK_HPB2_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->AHBMASK.reg |= PM_AHBMASK_HPB2;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pm_get_AHBMASK_HPB2_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pm *)hw)->AHBMASK.reg;
	tmp = (tmp & PM_AHBMASK_HPB2) >> PM_AHBMASK_HPB2_Pos;
	return (bool)tmp;
}

static inline void hri_pm_write_AHBMASK_HPB2_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PM_CRITICAL_SECTION_ENTER();
	tmp = ((Pm *)hw)->AHBMASK.reg;
	tmp &= ~PM_AHBMASK_HPB2;
	tmp |= value << PM_AHBMASK_HPB2_Pos;
	((Pm *)hw)->AHBMASK.reg = tmp;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_AHBMASK_HPB2_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->AHBMASK.reg &= ~PM_AHBMASK_HPB2;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_AHBMASK_HPB2_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->AHBMASK.reg ^= PM_AHBMASK_HPB2;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_set_AHBMASK_DSU_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->AHBMASK.reg |= PM_AHBMASK_DSU;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pm_get_AHBMASK_DSU_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pm *)hw)->AHBMASK.reg;
	tmp = (tmp & PM_AHBMASK_DSU) >> PM_AHBMASK_DSU_Pos;
	return (bool)tmp;
}

static inline void hri_pm_write_AHBMASK_DSU_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PM_CRITICAL_SECTION_ENTER();
	tmp = ((Pm *)hw)->AHBMASK.reg;
	tmp &= ~PM_AHBMASK_DSU;
	tmp |= value << PM_AHBMASK_DSU_Pos;
	((Pm *)hw)->AHBMASK.reg = tmp;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_AHBMASK_DSU_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->AHBMASK.reg &= ~PM_AHBMASK_DSU;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_AHBMASK_DSU_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->AHBMASK.reg ^= PM_AHBMASK_DSU;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_set_AHBMASK_NVMCTRL_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->AHBMASK.reg |= PM_AHBMASK_NVMCTRL;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pm_get_AHBMASK_NVMCTRL_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pm *)hw)->AHBMASK.reg;
	tmp = (tmp & PM_AHBMASK_NVMCTRL) >> PM_AHBMASK_NVMCTRL_Pos;
	return (bool)tmp;
}

static inline void hri_pm_write_AHBMASK_NVMCTRL_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PM_CRITICAL_SECTION_ENTER();
	tmp = ((Pm *)hw)->AHBMASK.reg;
	tmp &= ~PM_AHBMASK_NVMCTRL;
	tmp |= value << PM_AHBMASK_NVMCTRL_Pos;
	((Pm *)hw)->AHBMASK.reg = tmp;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_AHBMASK_NVMCTRL_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->AHBMASK.reg &= ~PM_AHBMASK_NVMCTRL;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_AHBMASK_NVMCTRL_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->AHBMASK.reg ^= PM_AHBMASK_NVMCTRL;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_set_AHBMASK_DMAC_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->AHBMASK.reg |= PM_AHBMASK_DMAC;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pm_get_AHBMASK_DMAC_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pm *)hw)->AHBMASK.reg;
	tmp = (tmp & PM_AHBMASK_DMAC) >> PM_AHBMASK_DMAC_Pos;
	return (bool)tmp;
}

static inline void hri_pm_write_AHBMASK_DMAC_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PM_CRITICAL_SECTION_ENTER();
	tmp = ((Pm *)hw)->AHBMASK.reg;
	tmp &= ~PM_AHBMASK_DMAC;
	tmp |= value << PM_AHBMASK_DMAC_Pos;
	((Pm *)hw)->AHBMASK.reg = tmp;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_AHBMASK_DMAC_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->AHBMASK.reg &= ~PM_AHBMASK_DMAC;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_AHBMASK_DMAC_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->AHBMASK.reg ^= PM_AHBMASK_DMAC;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_set_AHBMASK_USB_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->AHBMASK.reg |= PM_AHBMASK_USB;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pm_get_AHBMASK_USB_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pm *)hw)->AHBMASK.reg;
	tmp = (tmp & PM_AHBMASK_USB) >> PM_AHBMASK_USB_Pos;
	return (bool)tmp;
}

static inline void hri_pm_write_AHBMASK_USB_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PM_CRITICAL_SECTION_ENTER();
	tmp = ((Pm *)hw)->AHBMASK.reg;
	tmp &= ~PM_AHBMASK_USB;
	tmp |= value << PM_AHBMASK_USB_Pos;
	((Pm *)hw)->AHBMASK.reg = tmp;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_AHBMASK_USB_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->AHBMASK.reg &= ~PM_AHBMASK_USB;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_AHBMASK_USB_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->AHBMASK.reg ^= PM_AHBMASK_USB;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_set_AHBMASK_reg(const void *const hw, hri_pm_ahbmask_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->AHBMASK.reg |= mask;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline hri_pm_ahbmask_reg_t hri_pm_get_AHBMASK_reg(const void *const hw, hri_pm_ahbmask_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Pm *)hw)->AHBMASK.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_pm_write_AHBMASK_reg(const void *const hw, hri_pm_ahbmask_reg_t data)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->AHBMASK.reg = data;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_AHBMASK_reg(const void *const hw, hri_pm_ahbmask_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->AHBMASK.reg &= ~mask;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_AHBMASK_reg(const void *const hw, hri_pm_ahbmask_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->AHBMASK.reg ^= mask;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline hri_pm_ahbmask_reg_t hri_pm_read_AHBMASK_reg(const void *const hw)
{
	return ((Pm *)hw)->AHBMASK.reg;
}

static inline void hri_pm_set_APBAMASK_PAC0_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBAMASK.reg |= PM_APBAMASK_PAC0;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pm_get_APBAMASK_PAC0_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pm *)hw)->APBAMASK.reg;
	tmp = (tmp & PM_APBAMASK_PAC0) >> PM_APBAMASK_PAC0_Pos;
	return (bool)tmp;
}

static inline void hri_pm_write_APBAMASK_PAC0_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PM_CRITICAL_SECTION_ENTER();
	tmp = ((Pm *)hw)->APBAMASK.reg;
	tmp &= ~PM_APBAMASK_PAC0;
	tmp |= value << PM_APBAMASK_PAC0_Pos;
	((Pm *)hw)->APBAMASK.reg = tmp;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_APBAMASK_PAC0_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBAMASK.reg &= ~PM_APBAMASK_PAC0;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_APBAMASK_PAC0_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBAMASK.reg ^= PM_APBAMASK_PAC0;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_set_APBAMASK_PM_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBAMASK.reg |= PM_APBAMASK_PM;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pm_get_APBAMASK_PM_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pm *)hw)->APBAMASK.reg;
	tmp = (tmp & PM_APBAMASK_PM) >> PM_APBAMASK_PM_Pos;
	return (bool)tmp;
}

static inline void hri_pm_write_APBAMASK_PM_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PM_CRITICAL_SECTION_ENTER();
	tmp = ((Pm *)hw)->APBAMASK.reg;
	tmp &= ~PM_APBAMASK_PM;
	tmp |= value << PM_APBAMASK_PM_Pos;
	((Pm *)hw)->APBAMASK.reg = tmp;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_APBAMASK_PM_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBAMASK.reg &= ~PM_APBAMASK_PM;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_APBAMASK_PM_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBAMASK.reg ^= PM_APBAMASK_PM;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_set_APBAMASK_SYSCTRL_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBAMASK.reg |= PM_APBAMASK_SYSCTRL;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pm_get_APBAMASK_SYSCTRL_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pm *)hw)->APBAMASK.reg;
	tmp = (tmp & PM_APBAMASK_SYSCTRL) >> PM_APBAMASK_SYSCTRL_Pos;
	return (bool)tmp;
}

static inline void hri_pm_write_APBAMASK_SYSCTRL_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PM_CRITICAL_SECTION_ENTER();
	tmp = ((Pm *)hw)->APBAMASK.reg;
	tmp &= ~PM_APBAMASK_SYSCTRL;
	tmp |= value << PM_APBAMASK_SYSCTRL_Pos;
	((Pm *)hw)->APBAMASK.reg = tmp;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_APBAMASK_SYSCTRL_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBAMASK.reg &= ~PM_APBAMASK_SYSCTRL;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_APBAMASK_SYSCTRL_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBAMASK.reg ^= PM_APBAMASK_SYSCTRL;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_set_APBAMASK_GCLK_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBAMASK.reg |= PM_APBAMASK_GCLK;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pm_get_APBAMASK_GCLK_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pm *)hw)->APBAMASK.reg;
	tmp = (tmp & PM_APBAMASK_GCLK) >> PM_APBAMASK_GCLK_Pos;
	return (bool)tmp;
}

static inline void hri_pm_write_APBAMASK_GCLK_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PM_CRITICAL_SECTION_ENTER();
	tmp = ((Pm *)hw)->APBAMASK.reg;
	tmp &= ~PM_APBAMASK_GCLK;
	tmp |= value << PM_APBAMASK_GCLK_Pos;
	((Pm *)hw)->APBAMASK.reg = tmp;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_APBAMASK_GCLK_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBAMASK.reg &= ~PM_APBAMASK_GCLK;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_APBAMASK_GCLK_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBAMASK.reg ^= PM_APBAMASK_GCLK;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_set_APBAMASK_WDT_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBAMASK.reg |= PM_APBAMASK_WDT;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pm_get_APBAMASK_WDT_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pm *)hw)->APBAMASK.reg;
	tmp = (tmp & PM_APBAMASK_WDT) >> PM_APBAMASK_WDT_Pos;
	return (bool)tmp;
}

static inline void hri_pm_write_APBAMASK_WDT_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PM_CRITICAL_SECTION_ENTER();
	tmp = ((Pm *)hw)->APBAMASK.reg;
	tmp &= ~PM_APBAMASK_WDT;
	tmp |= value << PM_APBAMASK_WDT_Pos;
	((Pm *)hw)->APBAMASK.reg = tmp;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_APBAMASK_WDT_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBAMASK.reg &= ~PM_APBAMASK_WDT;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_APBAMASK_WDT_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBAMASK.reg ^= PM_APBAMASK_WDT;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_set_APBAMASK_RTC_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBAMASK.reg |= PM_APBAMASK_RTC;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pm_get_APBAMASK_RTC_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pm *)hw)->APBAMASK.reg;
	tmp = (tmp & PM_APBAMASK_RTC) >> PM_APBAMASK_RTC_Pos;
	return (bool)tmp;
}

static inline void hri_pm_write_APBAMASK_RTC_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PM_CRITICAL_SECTION_ENTER();
	tmp = ((Pm *)hw)->APBAMASK.reg;
	tmp &= ~PM_APBAMASK_RTC;
	tmp |= value << PM_APBAMASK_RTC_Pos;
	((Pm *)hw)->APBAMASK.reg = tmp;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_APBAMASK_RTC_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBAMASK.reg &= ~PM_APBAMASK_RTC;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_APBAMASK_RTC_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBAMASK.reg ^= PM_APBAMASK_RTC;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_set_APBAMASK_EIC_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBAMASK.reg |= PM_APBAMASK_EIC;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pm_get_APBAMASK_EIC_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pm *)hw)->APBAMASK.reg;
	tmp = (tmp & PM_APBAMASK_EIC) >> PM_APBAMASK_EIC_Pos;
	return (bool)tmp;
}

static inline void hri_pm_write_APBAMASK_EIC_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PM_CRITICAL_SECTION_ENTER();
	tmp = ((Pm *)hw)->APBAMASK.reg;
	tmp &= ~PM_APBAMASK_EIC;
	tmp |= value << PM_APBAMASK_EIC_Pos;
	((Pm *)hw)->APBAMASK.reg = tmp;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_APBAMASK_EIC_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBAMASK.reg &= ~PM_APBAMASK_EIC;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_APBAMASK_EIC_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBAMASK.reg ^= PM_APBAMASK_EIC;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_set_APBAMASK_reg(const void *const hw, hri_pm_apbamask_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBAMASK.reg |= mask;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline hri_pm_apbamask_reg_t hri_pm_get_APBAMASK_reg(const void *const hw, hri_pm_apbamask_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Pm *)hw)->APBAMASK.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_pm_write_APBAMASK_reg(const void *const hw, hri_pm_apbamask_reg_t data)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBAMASK.reg = data;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_APBAMASK_reg(const void *const hw, hri_pm_apbamask_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBAMASK.reg &= ~mask;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_APBAMASK_reg(const void *const hw, hri_pm_apbamask_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBAMASK.reg ^= mask;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline hri_pm_apbamask_reg_t hri_pm_read_APBAMASK_reg(const void *const hw)
{
	return ((Pm *)hw)->APBAMASK.reg;
}

static inline void hri_pm_set_APBBMASK_PAC1_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBBMASK.reg |= PM_APBBMASK_PAC1;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pm_get_APBBMASK_PAC1_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pm *)hw)->APBBMASK.reg;
	tmp = (tmp & PM_APBBMASK_PAC1) >> PM_APBBMASK_PAC1_Pos;
	return (bool)tmp;
}

static inline void hri_pm_write_APBBMASK_PAC1_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PM_CRITICAL_SECTION_ENTER();
	tmp = ((Pm *)hw)->APBBMASK.reg;
	tmp &= ~PM_APBBMASK_PAC1;
	tmp |= value << PM_APBBMASK_PAC1_Pos;
	((Pm *)hw)->APBBMASK.reg = tmp;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_APBBMASK_PAC1_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBBMASK.reg &= ~PM_APBBMASK_PAC1;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_APBBMASK_PAC1_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBBMASK.reg ^= PM_APBBMASK_PAC1;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_set_APBBMASK_DSU_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBBMASK.reg |= PM_APBBMASK_DSU;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pm_get_APBBMASK_DSU_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pm *)hw)->APBBMASK.reg;
	tmp = (tmp & PM_APBBMASK_DSU) >> PM_APBBMASK_DSU_Pos;
	return (bool)tmp;
}

static inline void hri_pm_write_APBBMASK_DSU_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PM_CRITICAL_SECTION_ENTER();
	tmp = ((Pm *)hw)->APBBMASK.reg;
	tmp &= ~PM_APBBMASK_DSU;
	tmp |= value << PM_APBBMASK_DSU_Pos;
	((Pm *)hw)->APBBMASK.reg = tmp;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_APBBMASK_DSU_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBBMASK.reg &= ~PM_APBBMASK_DSU;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_APBBMASK_DSU_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBBMASK.reg ^= PM_APBBMASK_DSU;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_set_APBBMASK_NVMCTRL_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBBMASK.reg |= PM_APBBMASK_NVMCTRL;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pm_get_APBBMASK_NVMCTRL_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pm *)hw)->APBBMASK.reg;
	tmp = (tmp & PM_APBBMASK_NVMCTRL) >> PM_APBBMASK_NVMCTRL_Pos;
	return (bool)tmp;
}

static inline void hri_pm_write_APBBMASK_NVMCTRL_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PM_CRITICAL_SECTION_ENTER();
	tmp = ((Pm *)hw)->APBBMASK.reg;
	tmp &= ~PM_APBBMASK_NVMCTRL;
	tmp |= value << PM_APBBMASK_NVMCTRL_Pos;
	((Pm *)hw)->APBBMASK.reg = tmp;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_APBBMASK_NVMCTRL_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBBMASK.reg &= ~PM_APBBMASK_NVMCTRL;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_APBBMASK_NVMCTRL_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBBMASK.reg ^= PM_APBBMASK_NVMCTRL;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_set_APBBMASK_PORT_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBBMASK.reg |= PM_APBBMASK_PORT;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pm_get_APBBMASK_PORT_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pm *)hw)->APBBMASK.reg;
	tmp = (tmp & PM_APBBMASK_PORT) >> PM_APBBMASK_PORT_Pos;
	return (bool)tmp;
}

static inline void hri_pm_write_APBBMASK_PORT_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PM_CRITICAL_SECTION_ENTER();
	tmp = ((Pm *)hw)->APBBMASK.reg;
	tmp &= ~PM_APBBMASK_PORT;
	tmp |= value << PM_APBBMASK_PORT_Pos;
	((Pm *)hw)->APBBMASK.reg = tmp;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_APBBMASK_PORT_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBBMASK.reg &= ~PM_APBBMASK_PORT;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_APBBMASK_PORT_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBBMASK.reg ^= PM_APBBMASK_PORT;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_set_APBBMASK_DMAC_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBBMASK.reg |= PM_APBBMASK_DMAC;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pm_get_APBBMASK_DMAC_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pm *)hw)->APBBMASK.reg;
	tmp = (tmp & PM_APBBMASK_DMAC) >> PM_APBBMASK_DMAC_Pos;
	return (bool)tmp;
}

static inline void hri_pm_write_APBBMASK_DMAC_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PM_CRITICAL_SECTION_ENTER();
	tmp = ((Pm *)hw)->APBBMASK.reg;
	tmp &= ~PM_APBBMASK_DMAC;
	tmp |= value << PM_APBBMASK_DMAC_Pos;
	((Pm *)hw)->APBBMASK.reg = tmp;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_APBBMASK_DMAC_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBBMASK.reg &= ~PM_APBBMASK_DMAC;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_APBBMASK_DMAC_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBBMASK.reg ^= PM_APBBMASK_DMAC;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_set_APBBMASK_USB_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBBMASK.reg |= PM_APBBMASK_USB;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pm_get_APBBMASK_USB_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pm *)hw)->APBBMASK.reg;
	tmp = (tmp & PM_APBBMASK_USB) >> PM_APBBMASK_USB_Pos;
	return (bool)tmp;
}

static inline void hri_pm_write_APBBMASK_USB_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PM_CRITICAL_SECTION_ENTER();
	tmp = ((Pm *)hw)->APBBMASK.reg;
	tmp &= ~PM_APBBMASK_USB;
	tmp |= value << PM_APBBMASK_USB_Pos;
	((Pm *)hw)->APBBMASK.reg = tmp;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_APBBMASK_USB_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBBMASK.reg &= ~PM_APBBMASK_USB;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_APBBMASK_USB_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBBMASK.reg ^= PM_APBBMASK_USB;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_set_APBBMASK_HMATRIX_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBBMASK.reg |= PM_APBBMASK_HMATRIX;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pm_get_APBBMASK_HMATRIX_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pm *)hw)->APBBMASK.reg;
	tmp = (tmp & PM_APBBMASK_HMATRIX) >> PM_APBBMASK_HMATRIX_Pos;
	return (bool)tmp;
}

static inline void hri_pm_write_APBBMASK_HMATRIX_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PM_CRITICAL_SECTION_ENTER();
	tmp = ((Pm *)hw)->APBBMASK.reg;
	tmp &= ~PM_APBBMASK_HMATRIX;
	tmp |= value << PM_APBBMASK_HMATRIX_Pos;
	((Pm *)hw)->APBBMASK.reg = tmp;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_APBBMASK_HMATRIX_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBBMASK.reg &= ~PM_APBBMASK_HMATRIX;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_APBBMASK_HMATRIX_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBBMASK.reg ^= PM_APBBMASK_HMATRIX;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_set_APBBMASK_reg(const void *const hw, hri_pm_apbbmask_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBBMASK.reg |= mask;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline hri_pm_apbbmask_reg_t hri_pm_get_APBBMASK_reg(const void *const hw, hri_pm_apbbmask_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Pm *)hw)->APBBMASK.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_pm_write_APBBMASK_reg(const void *const hw, hri_pm_apbbmask_reg_t data)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBBMASK.reg = data;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_APBBMASK_reg(const void *const hw, hri_pm_apbbmask_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBBMASK.reg &= ~mask;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_APBBMASK_reg(const void *const hw, hri_pm_apbbmask_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBBMASK.reg ^= mask;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline hri_pm_apbbmask_reg_t hri_pm_read_APBBMASK_reg(const void *const hw)
{
	return ((Pm *)hw)->APBBMASK.reg;
}

static inline void hri_pm_set_APBCMASK_PAC2_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg |= PM_APBCMASK_PAC2;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pm_get_APBCMASK_PAC2_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pm *)hw)->APBCMASK.reg;
	tmp = (tmp & PM_APBCMASK_PAC2) >> PM_APBCMASK_PAC2_Pos;
	return (bool)tmp;
}

static inline void hri_pm_write_APBCMASK_PAC2_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PM_CRITICAL_SECTION_ENTER();
	tmp = ((Pm *)hw)->APBCMASK.reg;
	tmp &= ~PM_APBCMASK_PAC2;
	tmp |= value << PM_APBCMASK_PAC2_Pos;
	((Pm *)hw)->APBCMASK.reg = tmp;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_APBCMASK_PAC2_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg &= ~PM_APBCMASK_PAC2;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_APBCMASK_PAC2_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg ^= PM_APBCMASK_PAC2;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_set_APBCMASK_EVSYS_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg |= PM_APBCMASK_EVSYS;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pm_get_APBCMASK_EVSYS_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pm *)hw)->APBCMASK.reg;
	tmp = (tmp & PM_APBCMASK_EVSYS) >> PM_APBCMASK_EVSYS_Pos;
	return (bool)tmp;
}

static inline void hri_pm_write_APBCMASK_EVSYS_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PM_CRITICAL_SECTION_ENTER();
	tmp = ((Pm *)hw)->APBCMASK.reg;
	tmp &= ~PM_APBCMASK_EVSYS;
	tmp |= value << PM_APBCMASK_EVSYS_Pos;
	((Pm *)hw)->APBCMASK.reg = tmp;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_APBCMASK_EVSYS_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg &= ~PM_APBCMASK_EVSYS;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_APBCMASK_EVSYS_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg ^= PM_APBCMASK_EVSYS;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_set_APBCMASK_SERCOM0_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg |= PM_APBCMASK_SERCOM0;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pm_get_APBCMASK_SERCOM0_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pm *)hw)->APBCMASK.reg;
	tmp = (tmp & PM_APBCMASK_SERCOM0) >> PM_APBCMASK_SERCOM0_Pos;
	return (bool)tmp;
}

static inline void hri_pm_write_APBCMASK_SERCOM0_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PM_CRITICAL_SECTION_ENTER();
	tmp = ((Pm *)hw)->APBCMASK.reg;
	tmp &= ~PM_APBCMASK_SERCOM0;
	tmp |= value << PM_APBCMASK_SERCOM0_Pos;
	((Pm *)hw)->APBCMASK.reg = tmp;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_APBCMASK_SERCOM0_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg &= ~PM_APBCMASK_SERCOM0;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_APBCMASK_SERCOM0_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg ^= PM_APBCMASK_SERCOM0;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_set_APBCMASK_SERCOM1_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg |= PM_APBCMASK_SERCOM1;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pm_get_APBCMASK_SERCOM1_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pm *)hw)->APBCMASK.reg;
	tmp = (tmp & PM_APBCMASK_SERCOM1) >> PM_APBCMASK_SERCOM1_Pos;
	return (bool)tmp;
}

static inline void hri_pm_write_APBCMASK_SERCOM1_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PM_CRITICAL_SECTION_ENTER();
	tmp = ((Pm *)hw)->APBCMASK.reg;
	tmp &= ~PM_APBCMASK_SERCOM1;
	tmp |= value << PM_APBCMASK_SERCOM1_Pos;
	((Pm *)hw)->APBCMASK.reg = tmp;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_APBCMASK_SERCOM1_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg &= ~PM_APBCMASK_SERCOM1;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_APBCMASK_SERCOM1_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg ^= PM_APBCMASK_SERCOM1;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_set_APBCMASK_SERCOM2_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg |= PM_APBCMASK_SERCOM2;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pm_get_APBCMASK_SERCOM2_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pm *)hw)->APBCMASK.reg;
	tmp = (tmp & PM_APBCMASK_SERCOM2) >> PM_APBCMASK_SERCOM2_Pos;
	return (bool)tmp;
}

static inline void hri_pm_write_APBCMASK_SERCOM2_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PM_CRITICAL_SECTION_ENTER();
	tmp = ((Pm *)hw)->APBCMASK.reg;
	tmp &= ~PM_APBCMASK_SERCOM2;
	tmp |= value << PM_APBCMASK_SERCOM2_Pos;
	((Pm *)hw)->APBCMASK.reg = tmp;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_APBCMASK_SERCOM2_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg &= ~PM_APBCMASK_SERCOM2;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_APBCMASK_SERCOM2_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg ^= PM_APBCMASK_SERCOM2;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_set_APBCMASK_SERCOM3_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg |= PM_APBCMASK_SERCOM3;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pm_get_APBCMASK_SERCOM3_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pm *)hw)->APBCMASK.reg;
	tmp = (tmp & PM_APBCMASK_SERCOM3) >> PM_APBCMASK_SERCOM3_Pos;
	return (bool)tmp;
}

static inline void hri_pm_write_APBCMASK_SERCOM3_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PM_CRITICAL_SECTION_ENTER();
	tmp = ((Pm *)hw)->APBCMASK.reg;
	tmp &= ~PM_APBCMASK_SERCOM3;
	tmp |= value << PM_APBCMASK_SERCOM3_Pos;
	((Pm *)hw)->APBCMASK.reg = tmp;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_APBCMASK_SERCOM3_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg &= ~PM_APBCMASK_SERCOM3;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_APBCMASK_SERCOM3_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg ^= PM_APBCMASK_SERCOM3;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_set_APBCMASK_SERCOM4_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg |= PM_APBCMASK_SERCOM4;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pm_get_APBCMASK_SERCOM4_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pm *)hw)->APBCMASK.reg;
	tmp = (tmp & PM_APBCMASK_SERCOM4) >> PM_APBCMASK_SERCOM4_Pos;
	return (bool)tmp;
}

static inline void hri_pm_write_APBCMASK_SERCOM4_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PM_CRITICAL_SECTION_ENTER();
	tmp = ((Pm *)hw)->APBCMASK.reg;
	tmp &= ~PM_APBCMASK_SERCOM4;
	tmp |= value << PM_APBCMASK_SERCOM4_Pos;
	((Pm *)hw)->APBCMASK.reg = tmp;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_APBCMASK_SERCOM4_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg &= ~PM_APBCMASK_SERCOM4;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_APBCMASK_SERCOM4_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg ^= PM_APBCMASK_SERCOM4;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_set_APBCMASK_SERCOM5_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg |= PM_APBCMASK_SERCOM5;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pm_get_APBCMASK_SERCOM5_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pm *)hw)->APBCMASK.reg;
	tmp = (tmp & PM_APBCMASK_SERCOM5) >> PM_APBCMASK_SERCOM5_Pos;
	return (bool)tmp;
}

static inline void hri_pm_write_APBCMASK_SERCOM5_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PM_CRITICAL_SECTION_ENTER();
	tmp = ((Pm *)hw)->APBCMASK.reg;
	tmp &= ~PM_APBCMASK_SERCOM5;
	tmp |= value << PM_APBCMASK_SERCOM5_Pos;
	((Pm *)hw)->APBCMASK.reg = tmp;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_APBCMASK_SERCOM5_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg &= ~PM_APBCMASK_SERCOM5;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_APBCMASK_SERCOM5_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg ^= PM_APBCMASK_SERCOM5;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_set_APBCMASK_TCC0_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg |= PM_APBCMASK_TCC0;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pm_get_APBCMASK_TCC0_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pm *)hw)->APBCMASK.reg;
	tmp = (tmp & PM_APBCMASK_TCC0) >> PM_APBCMASK_TCC0_Pos;
	return (bool)tmp;
}

static inline void hri_pm_write_APBCMASK_TCC0_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PM_CRITICAL_SECTION_ENTER();
	tmp = ((Pm *)hw)->APBCMASK.reg;
	tmp &= ~PM_APBCMASK_TCC0;
	tmp |= value << PM_APBCMASK_TCC0_Pos;
	((Pm *)hw)->APBCMASK.reg = tmp;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_APBCMASK_TCC0_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg &= ~PM_APBCMASK_TCC0;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_APBCMASK_TCC0_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg ^= PM_APBCMASK_TCC0;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_set_APBCMASK_TCC1_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg |= PM_APBCMASK_TCC1;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pm_get_APBCMASK_TCC1_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pm *)hw)->APBCMASK.reg;
	tmp = (tmp & PM_APBCMASK_TCC1) >> PM_APBCMASK_TCC1_Pos;
	return (bool)tmp;
}

static inline void hri_pm_write_APBCMASK_TCC1_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PM_CRITICAL_SECTION_ENTER();
	tmp = ((Pm *)hw)->APBCMASK.reg;
	tmp &= ~PM_APBCMASK_TCC1;
	tmp |= value << PM_APBCMASK_TCC1_Pos;
	((Pm *)hw)->APBCMASK.reg = tmp;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_APBCMASK_TCC1_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg &= ~PM_APBCMASK_TCC1;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_APBCMASK_TCC1_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg ^= PM_APBCMASK_TCC1;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_set_APBCMASK_TCC2_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg |= PM_APBCMASK_TCC2;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pm_get_APBCMASK_TCC2_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pm *)hw)->APBCMASK.reg;
	tmp = (tmp & PM_APBCMASK_TCC2) >> PM_APBCMASK_TCC2_Pos;
	return (bool)tmp;
}

static inline void hri_pm_write_APBCMASK_TCC2_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PM_CRITICAL_SECTION_ENTER();
	tmp = ((Pm *)hw)->APBCMASK.reg;
	tmp &= ~PM_APBCMASK_TCC2;
	tmp |= value << PM_APBCMASK_TCC2_Pos;
	((Pm *)hw)->APBCMASK.reg = tmp;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_APBCMASK_TCC2_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg &= ~PM_APBCMASK_TCC2;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_APBCMASK_TCC2_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg ^= PM_APBCMASK_TCC2;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_set_APBCMASK_TC3_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg |= PM_APBCMASK_TC3;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pm_get_APBCMASK_TC3_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pm *)hw)->APBCMASK.reg;
	tmp = (tmp & PM_APBCMASK_TC3) >> PM_APBCMASK_TC3_Pos;
	return (bool)tmp;
}

static inline void hri_pm_write_APBCMASK_TC3_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PM_CRITICAL_SECTION_ENTER();
	tmp = ((Pm *)hw)->APBCMASK.reg;
	tmp &= ~PM_APBCMASK_TC3;
	tmp |= value << PM_APBCMASK_TC3_Pos;
	((Pm *)hw)->APBCMASK.reg = tmp;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_APBCMASK_TC3_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg &= ~PM_APBCMASK_TC3;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_APBCMASK_TC3_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg ^= PM_APBCMASK_TC3;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_set_APBCMASK_TC4_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg |= PM_APBCMASK_TC4;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pm_get_APBCMASK_TC4_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pm *)hw)->APBCMASK.reg;
	tmp = (tmp & PM_APBCMASK_TC4) >> PM_APBCMASK_TC4_Pos;
	return (bool)tmp;
}

static inline void hri_pm_write_APBCMASK_TC4_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PM_CRITICAL_SECTION_ENTER();
	tmp = ((Pm *)hw)->APBCMASK.reg;
	tmp &= ~PM_APBCMASK_TC4;
	tmp |= value << PM_APBCMASK_TC4_Pos;
	((Pm *)hw)->APBCMASK.reg = tmp;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_APBCMASK_TC4_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg &= ~PM_APBCMASK_TC4;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_APBCMASK_TC4_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg ^= PM_APBCMASK_TC4;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_set_APBCMASK_TC5_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg |= PM_APBCMASK_TC5;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pm_get_APBCMASK_TC5_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pm *)hw)->APBCMASK.reg;
	tmp = (tmp & PM_APBCMASK_TC5) >> PM_APBCMASK_TC5_Pos;
	return (bool)tmp;
}

static inline void hri_pm_write_APBCMASK_TC5_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PM_CRITICAL_SECTION_ENTER();
	tmp = ((Pm *)hw)->APBCMASK.reg;
	tmp &= ~PM_APBCMASK_TC5;
	tmp |= value << PM_APBCMASK_TC5_Pos;
	((Pm *)hw)->APBCMASK.reg = tmp;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_APBCMASK_TC5_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg &= ~PM_APBCMASK_TC5;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_APBCMASK_TC5_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg ^= PM_APBCMASK_TC5;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_set_APBCMASK_TC6_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg |= PM_APBCMASK_TC6;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pm_get_APBCMASK_TC6_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pm *)hw)->APBCMASK.reg;
	tmp = (tmp & PM_APBCMASK_TC6) >> PM_APBCMASK_TC6_Pos;
	return (bool)tmp;
}

static inline void hri_pm_write_APBCMASK_TC6_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PM_CRITICAL_SECTION_ENTER();
	tmp = ((Pm *)hw)->APBCMASK.reg;
	tmp &= ~PM_APBCMASK_TC6;
	tmp |= value << PM_APBCMASK_TC6_Pos;
	((Pm *)hw)->APBCMASK.reg = tmp;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_APBCMASK_TC6_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg &= ~PM_APBCMASK_TC6;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_APBCMASK_TC6_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg ^= PM_APBCMASK_TC6;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_set_APBCMASK_TC7_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg |= PM_APBCMASK_TC7;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pm_get_APBCMASK_TC7_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pm *)hw)->APBCMASK.reg;
	tmp = (tmp & PM_APBCMASK_TC7) >> PM_APBCMASK_TC7_Pos;
	return (bool)tmp;
}

static inline void hri_pm_write_APBCMASK_TC7_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PM_CRITICAL_SECTION_ENTER();
	tmp = ((Pm *)hw)->APBCMASK.reg;
	tmp &= ~PM_APBCMASK_TC7;
	tmp |= value << PM_APBCMASK_TC7_Pos;
	((Pm *)hw)->APBCMASK.reg = tmp;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_APBCMASK_TC7_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg &= ~PM_APBCMASK_TC7;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_APBCMASK_TC7_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg ^= PM_APBCMASK_TC7;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_set_APBCMASK_ADC_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg |= PM_APBCMASK_ADC;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pm_get_APBCMASK_ADC_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pm *)hw)->APBCMASK.reg;
	tmp = (tmp & PM_APBCMASK_ADC) >> PM_APBCMASK_ADC_Pos;
	return (bool)tmp;
}

static inline void hri_pm_write_APBCMASK_ADC_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PM_CRITICAL_SECTION_ENTER();
	tmp = ((Pm *)hw)->APBCMASK.reg;
	tmp &= ~PM_APBCMASK_ADC;
	tmp |= value << PM_APBCMASK_ADC_Pos;
	((Pm *)hw)->APBCMASK.reg = tmp;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_APBCMASK_ADC_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg &= ~PM_APBCMASK_ADC;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_APBCMASK_ADC_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg ^= PM_APBCMASK_ADC;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_set_APBCMASK_AC_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg |= PM_APBCMASK_AC;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pm_get_APBCMASK_AC_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pm *)hw)->APBCMASK.reg;
	tmp = (tmp & PM_APBCMASK_AC) >> PM_APBCMASK_AC_Pos;
	return (bool)tmp;
}

static inline void hri_pm_write_APBCMASK_AC_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PM_CRITICAL_SECTION_ENTER();
	tmp = ((Pm *)hw)->APBCMASK.reg;
	tmp &= ~PM_APBCMASK_AC;
	tmp |= value << PM_APBCMASK_AC_Pos;
	((Pm *)hw)->APBCMASK.reg = tmp;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_APBCMASK_AC_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg &= ~PM_APBCMASK_AC;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_APBCMASK_AC_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg ^= PM_APBCMASK_AC;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_set_APBCMASK_DAC_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg |= PM_APBCMASK_DAC;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pm_get_APBCMASK_DAC_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pm *)hw)->APBCMASK.reg;
	tmp = (tmp & PM_APBCMASK_DAC) >> PM_APBCMASK_DAC_Pos;
	return (bool)tmp;
}

static inline void hri_pm_write_APBCMASK_DAC_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PM_CRITICAL_SECTION_ENTER();
	tmp = ((Pm *)hw)->APBCMASK.reg;
	tmp &= ~PM_APBCMASK_DAC;
	tmp |= value << PM_APBCMASK_DAC_Pos;
	((Pm *)hw)->APBCMASK.reg = tmp;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_APBCMASK_DAC_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg &= ~PM_APBCMASK_DAC;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_APBCMASK_DAC_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg ^= PM_APBCMASK_DAC;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_set_APBCMASK_PTC_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg |= PM_APBCMASK_PTC;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pm_get_APBCMASK_PTC_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pm *)hw)->APBCMASK.reg;
	tmp = (tmp & PM_APBCMASK_PTC) >> PM_APBCMASK_PTC_Pos;
	return (bool)tmp;
}

static inline void hri_pm_write_APBCMASK_PTC_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PM_CRITICAL_SECTION_ENTER();
	tmp = ((Pm *)hw)->APBCMASK.reg;
	tmp &= ~PM_APBCMASK_PTC;
	tmp |= value << PM_APBCMASK_PTC_Pos;
	((Pm *)hw)->APBCMASK.reg = tmp;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_APBCMASK_PTC_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg &= ~PM_APBCMASK_PTC;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_APBCMASK_PTC_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg ^= PM_APBCMASK_PTC;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_set_APBCMASK_I2S_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg |= PM_APBCMASK_I2S;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_pm_get_APBCMASK_I2S_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Pm *)hw)->APBCMASK.reg;
	tmp = (tmp & PM_APBCMASK_I2S) >> PM_APBCMASK_I2S_Pos;
	return (bool)tmp;
}

static inline void hri_pm_write_APBCMASK_I2S_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PM_CRITICAL_SECTION_ENTER();
	tmp = ((Pm *)hw)->APBCMASK.reg;
	tmp &= ~PM_APBCMASK_I2S;
	tmp |= value << PM_APBCMASK_I2S_Pos;
	((Pm *)hw)->APBCMASK.reg = tmp;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_APBCMASK_I2S_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg &= ~PM_APBCMASK_I2S;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_APBCMASK_I2S_bit(const void *const hw)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg ^= PM_APBCMASK_I2S;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_set_APBCMASK_reg(const void *const hw, hri_pm_apbcmask_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg |= mask;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline hri_pm_apbcmask_reg_t hri_pm_get_APBCMASK_reg(const void *const hw, hri_pm_apbcmask_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Pm *)hw)->APBCMASK.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_pm_write_APBCMASK_reg(const void *const hw, hri_pm_apbcmask_reg_t data)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg = data;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_clear_APBCMASK_reg(const void *const hw, hri_pm_apbcmask_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg &= ~mask;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline void hri_pm_toggle_APBCMASK_reg(const void *const hw, hri_pm_apbcmask_reg_t mask)
{
	PM_CRITICAL_SECTION_ENTER();
	((Pm *)hw)->APBCMASK.reg ^= mask;
	PM_CRITICAL_SECTION_LEAVE();
}

static inline hri_pm_apbcmask_reg_t hri_pm_read_APBCMASK_reg(const void *const hw)
{
	return ((Pm *)hw)->APBCMASK.reg;
}

static inline bool hri_pm_get_RCAUSE_POR_bit(const void *const hw)
{
	return (((Pm *)hw)->RCAUSE.reg & PM_RCAUSE_POR) >> PM_RCAUSE_POR_Pos;
}

static inline bool hri_pm_get_RCAUSE_BOD12_bit(const void *const hw)
{
	return (((Pm *)hw)->RCAUSE.reg & PM_RCAUSE_BOD12) >> PM_RCAUSE_BOD12_Pos;
}

static inline bool hri_pm_get_RCAUSE_BOD33_bit(const void *const hw)
{
	return (((Pm *)hw)->RCAUSE.reg & PM_RCAUSE_BOD33) >> PM_RCAUSE_BOD33_Pos;
}

static inline bool hri_pm_get_RCAUSE_EXT_bit(const void *const hw)
{
	return (((Pm *)hw)->RCAUSE.reg & PM_RCAUSE_EXT) >> PM_RCAUSE_EXT_Pos;
}

static inline bool hri_pm_get_RCAUSE_WDT_bit(const void *const hw)
{
	return (((Pm *)hw)->RCAUSE.reg & PM_RCAUSE_WDT) >> PM_RCAUSE_WDT_Pos;
}

static inline bool hri_pm_get_RCAUSE_SYST_bit(const void *const hw)
{
	return (((Pm *)hw)->RCAUSE.reg & PM_RCAUSE_SYST) >> PM_RCAUSE_SYST_Pos;
}

static inline hri_pm_rcause_reg_t hri_pm_get_RCAUSE_reg(const void *const hw, hri_pm_rcause_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Pm *)hw)->RCAUSE.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_pm_rcause_reg_t hri_pm_read_RCAUSE_reg(const void *const hw)
{
	return ((Pm *)hw)->RCAUSE.reg;
}

#ifdef __cplusplus
}
#endif

#endif /* _HRI_PM_D21_H_INCLUDED */
#endif /* _SAMD21_PM_COMPONENT_ */
