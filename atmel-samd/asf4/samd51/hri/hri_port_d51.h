/**
 * \file
 *
 * \brief SAM PORT
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

#ifdef _SAMD51_PORT_COMPONENT_
#ifndef _HRI_PORT_D51_H_INCLUDED_
#define _HRI_PORT_D51_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <hal_atomic.h>

#if defined(ENABLE_PORT_CRITICAL_SECTIONS)
#define PORT_CRITICAL_SECTION_ENTER() CRITICAL_SECTION_ENTER()
#define PORT_CRITICAL_SECTION_LEAVE() CRITICAL_SECTION_LEAVE()
#else
#define PORT_CRITICAL_SECTION_ENTER()
#define PORT_CRITICAL_SECTION_LEAVE()
#endif

typedef uint32_t hri_port_ctrl_reg_t;
typedef uint32_t hri_port_dir_reg_t;
typedef uint32_t hri_port_evctrl_reg_t;
typedef uint32_t hri_port_in_reg_t;
typedef uint32_t hri_port_out_reg_t;
typedef uint32_t hri_port_wrconfig_reg_t;
typedef uint32_t hri_portgroup_ctrl_reg_t;
typedef uint32_t hri_portgroup_dir_reg_t;
typedef uint32_t hri_portgroup_evctrl_reg_t;
typedef uint32_t hri_portgroup_in_reg_t;
typedef uint32_t hri_portgroup_out_reg_t;
typedef uint32_t hri_portgroup_wrconfig_reg_t;
typedef uint8_t  hri_port_pincfg_reg_t;
typedef uint8_t  hri_port_pmux_reg_t;
typedef uint8_t  hri_portgroup_pincfg_reg_t;
typedef uint8_t  hri_portgroup_pmux_reg_t;

static inline void hri_portgroup_set_DIR_DIR_bf(const void *const hw, hri_port_dir_reg_t mask)
{
	((PortGroup *)hw)->DIRSET.reg = PORT_DIR_DIR(mask);
}

static inline hri_port_dir_reg_t hri_portgroup_get_DIR_DIR_bf(const void *const hw, hri_port_dir_reg_t mask)
{
	uint32_t tmp;
	tmp = ((PortGroup *)hw)->DIR.reg;
	tmp = (tmp & PORT_DIR_DIR(mask)) >> PORT_DIR_DIR_Pos;
	return tmp;
}

static inline hri_port_dir_reg_t hri_portgroup_read_DIR_DIR_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((PortGroup *)hw)->DIR.reg;
	tmp = (tmp & PORT_DIR_DIR_Msk) >> PORT_DIR_DIR_Pos;
	return tmp;
}

static inline void hri_portgroup_write_DIR_DIR_bf(const void *const hw, hri_port_dir_reg_t data)
{
	((PortGroup *)hw)->DIRSET.reg = PORT_DIR_DIR(data);
	((PortGroup *)hw)->DIRCLR.reg = ~PORT_DIR_DIR(data);
}

static inline void hri_portgroup_clear_DIR_DIR_bf(const void *const hw, hri_port_dir_reg_t mask)
{
	((PortGroup *)hw)->DIRCLR.reg = PORT_DIR_DIR(mask);
}

static inline void hri_portgroup_toggle_DIR_DIR_bf(const void *const hw, hri_port_dir_reg_t mask)
{
	((PortGroup *)hw)->DIRTGL.reg = PORT_DIR_DIR(mask);
}

static inline void hri_portgroup_set_DIR_reg(const void *const hw, hri_port_dir_reg_t mask)
{
	((PortGroup *)hw)->DIRSET.reg = mask;
}

static inline hri_port_dir_reg_t hri_portgroup_get_DIR_reg(const void *const hw, hri_port_dir_reg_t mask)
{
	uint32_t tmp;
	tmp = ((PortGroup *)hw)->DIR.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_port_dir_reg_t hri_portgroup_read_DIR_reg(const void *const hw)
{
	return ((PortGroup *)hw)->DIR.reg;
}

static inline void hri_portgroup_write_DIR_reg(const void *const hw, hri_port_dir_reg_t data)
{
	((PortGroup *)hw)->DIRSET.reg = data;
	((PortGroup *)hw)->DIRCLR.reg = ~data;
}

static inline void hri_portgroup_clear_DIR_reg(const void *const hw, hri_port_dir_reg_t mask)
{
	((PortGroup *)hw)->DIRCLR.reg = mask;
}

static inline void hri_portgroup_toggle_DIR_reg(const void *const hw, hri_port_dir_reg_t mask)
{
	((PortGroup *)hw)->DIRTGL.reg = mask;
}

static inline void hri_portgroup_set_OUT_OUT_bf(const void *const hw, hri_port_out_reg_t mask)
{
	((PortGroup *)hw)->OUTSET.reg = PORT_OUT_OUT(mask);
}

static inline hri_port_out_reg_t hri_portgroup_get_OUT_OUT_bf(const void *const hw, hri_port_out_reg_t mask)
{
	uint32_t tmp;
	tmp = ((PortGroup *)hw)->OUT.reg;
	tmp = (tmp & PORT_OUT_OUT(mask)) >> PORT_OUT_OUT_Pos;
	return tmp;
}

static inline hri_port_out_reg_t hri_portgroup_read_OUT_OUT_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((PortGroup *)hw)->OUT.reg;
	tmp = (tmp & PORT_OUT_OUT_Msk) >> PORT_OUT_OUT_Pos;
	return tmp;
}

static inline void hri_portgroup_write_OUT_OUT_bf(const void *const hw, hri_port_out_reg_t data)
{
	((PortGroup *)hw)->OUTSET.reg = PORT_OUT_OUT(data);
	((PortGroup *)hw)->OUTCLR.reg = ~PORT_OUT_OUT(data);
}

static inline void hri_portgroup_clear_OUT_OUT_bf(const void *const hw, hri_port_out_reg_t mask)
{
	((PortGroup *)hw)->OUTCLR.reg = PORT_OUT_OUT(mask);
}

static inline void hri_portgroup_toggle_OUT_OUT_bf(const void *const hw, hri_port_out_reg_t mask)
{
	((PortGroup *)hw)->OUTTGL.reg = PORT_OUT_OUT(mask);
}

static inline void hri_portgroup_set_OUT_reg(const void *const hw, hri_port_out_reg_t mask)
{
	((PortGroup *)hw)->OUTSET.reg = mask;
}

static inline hri_port_out_reg_t hri_portgroup_get_OUT_reg(const void *const hw, hri_port_out_reg_t mask)
{
	uint32_t tmp;
	tmp = ((PortGroup *)hw)->OUT.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_port_out_reg_t hri_portgroup_read_OUT_reg(const void *const hw)
{
	return ((PortGroup *)hw)->OUT.reg;
}

static inline void hri_portgroup_write_OUT_reg(const void *const hw, hri_port_out_reg_t data)
{
	((PortGroup *)hw)->OUTSET.reg = data;
	((PortGroup *)hw)->OUTCLR.reg = ~data;
}

static inline void hri_portgroup_clear_OUT_reg(const void *const hw, hri_port_out_reg_t mask)
{
	((PortGroup *)hw)->OUTCLR.reg = mask;
}

static inline void hri_portgroup_toggle_OUT_reg(const void *const hw, hri_port_out_reg_t mask)
{
	((PortGroup *)hw)->OUTTGL.reg = mask;
}

static inline void hri_portgroup_write_WRCONFIG_reg(const void *const hw, hri_port_wrconfig_reg_t data)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->WRCONFIG.reg = data;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_portgroup_set_CTRL_SAMPLING_bf(const void *const hw, hri_port_ctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->CTRL.reg |= PORT_CTRL_SAMPLING(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline hri_port_ctrl_reg_t hri_portgroup_get_CTRL_SAMPLING_bf(const void *const hw, hri_port_ctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((PortGroup *)hw)->CTRL.reg;
	tmp = (tmp & PORT_CTRL_SAMPLING(mask)) >> PORT_CTRL_SAMPLING_Pos;
	return tmp;
}

static inline void hri_portgroup_write_CTRL_SAMPLING_bf(const void *const hw, hri_port_ctrl_reg_t data)
{
	uint32_t tmp;
	PORT_CRITICAL_SECTION_ENTER();
	tmp = ((PortGroup *)hw)->CTRL.reg;
	tmp &= ~PORT_CTRL_SAMPLING_Msk;
	tmp |= PORT_CTRL_SAMPLING(data);
	((PortGroup *)hw)->CTRL.reg = tmp;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_portgroup_clear_CTRL_SAMPLING_bf(const void *const hw, hri_port_ctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->CTRL.reg &= ~PORT_CTRL_SAMPLING(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_portgroup_toggle_CTRL_SAMPLING_bf(const void *const hw, hri_port_ctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->CTRL.reg ^= PORT_CTRL_SAMPLING(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline hri_port_ctrl_reg_t hri_portgroup_read_CTRL_SAMPLING_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((PortGroup *)hw)->CTRL.reg;
	tmp = (tmp & PORT_CTRL_SAMPLING_Msk) >> PORT_CTRL_SAMPLING_Pos;
	return tmp;
}

static inline void hri_portgroup_set_CTRL_reg(const void *const hw, hri_port_ctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->CTRL.reg |= mask;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline hri_port_ctrl_reg_t hri_portgroup_get_CTRL_reg(const void *const hw, hri_port_ctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((PortGroup *)hw)->CTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_portgroup_write_CTRL_reg(const void *const hw, hri_port_ctrl_reg_t data)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->CTRL.reg = data;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_portgroup_clear_CTRL_reg(const void *const hw, hri_port_ctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->CTRL.reg &= ~mask;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_portgroup_toggle_CTRL_reg(const void *const hw, hri_port_ctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->CTRL.reg ^= mask;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline hri_port_ctrl_reg_t hri_portgroup_read_CTRL_reg(const void *const hw)
{
	return ((PortGroup *)hw)->CTRL.reg;
}

static inline void hri_portgroup_set_EVCTRL_PORTEI0_bit(const void *const hw)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->EVCTRL.reg |= PORT_EVCTRL_PORTEI0;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_portgroup_get_EVCTRL_PORTEI0_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((PortGroup *)hw)->EVCTRL.reg;
	tmp = (tmp & PORT_EVCTRL_PORTEI0) >> PORT_EVCTRL_PORTEI0_Pos;
	return (bool)tmp;
}

static inline void hri_portgroup_write_EVCTRL_PORTEI0_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PORT_CRITICAL_SECTION_ENTER();
	tmp = ((PortGroup *)hw)->EVCTRL.reg;
	tmp &= ~PORT_EVCTRL_PORTEI0;
	tmp |= value << PORT_EVCTRL_PORTEI0_Pos;
	((PortGroup *)hw)->EVCTRL.reg = tmp;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_portgroup_clear_EVCTRL_PORTEI0_bit(const void *const hw)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->EVCTRL.reg &= ~PORT_EVCTRL_PORTEI0;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_portgroup_toggle_EVCTRL_PORTEI0_bit(const void *const hw)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->EVCTRL.reg ^= PORT_EVCTRL_PORTEI0;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_portgroup_set_EVCTRL_PORTEI1_bit(const void *const hw)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->EVCTRL.reg |= PORT_EVCTRL_PORTEI1;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_portgroup_get_EVCTRL_PORTEI1_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((PortGroup *)hw)->EVCTRL.reg;
	tmp = (tmp & PORT_EVCTRL_PORTEI1) >> PORT_EVCTRL_PORTEI1_Pos;
	return (bool)tmp;
}

static inline void hri_portgroup_write_EVCTRL_PORTEI1_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PORT_CRITICAL_SECTION_ENTER();
	tmp = ((PortGroup *)hw)->EVCTRL.reg;
	tmp &= ~PORT_EVCTRL_PORTEI1;
	tmp |= value << PORT_EVCTRL_PORTEI1_Pos;
	((PortGroup *)hw)->EVCTRL.reg = tmp;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_portgroup_clear_EVCTRL_PORTEI1_bit(const void *const hw)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->EVCTRL.reg &= ~PORT_EVCTRL_PORTEI1;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_portgroup_toggle_EVCTRL_PORTEI1_bit(const void *const hw)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->EVCTRL.reg ^= PORT_EVCTRL_PORTEI1;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_portgroup_set_EVCTRL_PORTEI2_bit(const void *const hw)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->EVCTRL.reg |= PORT_EVCTRL_PORTEI2;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_portgroup_get_EVCTRL_PORTEI2_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((PortGroup *)hw)->EVCTRL.reg;
	tmp = (tmp & PORT_EVCTRL_PORTEI2) >> PORT_EVCTRL_PORTEI2_Pos;
	return (bool)tmp;
}

static inline void hri_portgroup_write_EVCTRL_PORTEI2_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PORT_CRITICAL_SECTION_ENTER();
	tmp = ((PortGroup *)hw)->EVCTRL.reg;
	tmp &= ~PORT_EVCTRL_PORTEI2;
	tmp |= value << PORT_EVCTRL_PORTEI2_Pos;
	((PortGroup *)hw)->EVCTRL.reg = tmp;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_portgroup_clear_EVCTRL_PORTEI2_bit(const void *const hw)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->EVCTRL.reg &= ~PORT_EVCTRL_PORTEI2;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_portgroup_toggle_EVCTRL_PORTEI2_bit(const void *const hw)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->EVCTRL.reg ^= PORT_EVCTRL_PORTEI2;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_portgroup_set_EVCTRL_PORTEI3_bit(const void *const hw)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->EVCTRL.reg |= PORT_EVCTRL_PORTEI3;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_portgroup_get_EVCTRL_PORTEI3_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((PortGroup *)hw)->EVCTRL.reg;
	tmp = (tmp & PORT_EVCTRL_PORTEI3) >> PORT_EVCTRL_PORTEI3_Pos;
	return (bool)tmp;
}

static inline void hri_portgroup_write_EVCTRL_PORTEI3_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	PORT_CRITICAL_SECTION_ENTER();
	tmp = ((PortGroup *)hw)->EVCTRL.reg;
	tmp &= ~PORT_EVCTRL_PORTEI3;
	tmp |= value << PORT_EVCTRL_PORTEI3_Pos;
	((PortGroup *)hw)->EVCTRL.reg = tmp;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_portgroup_clear_EVCTRL_PORTEI3_bit(const void *const hw)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->EVCTRL.reg &= ~PORT_EVCTRL_PORTEI3;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_portgroup_toggle_EVCTRL_PORTEI3_bit(const void *const hw)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->EVCTRL.reg ^= PORT_EVCTRL_PORTEI3;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_portgroup_set_EVCTRL_PID0_bf(const void *const hw, hri_port_evctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->EVCTRL.reg |= PORT_EVCTRL_PID0(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline hri_port_evctrl_reg_t hri_portgroup_get_EVCTRL_PID0_bf(const void *const hw, hri_port_evctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((PortGroup *)hw)->EVCTRL.reg;
	tmp = (tmp & PORT_EVCTRL_PID0(mask)) >> PORT_EVCTRL_PID0_Pos;
	return tmp;
}

static inline void hri_portgroup_write_EVCTRL_PID0_bf(const void *const hw, hri_port_evctrl_reg_t data)
{
	uint32_t tmp;
	PORT_CRITICAL_SECTION_ENTER();
	tmp = ((PortGroup *)hw)->EVCTRL.reg;
	tmp &= ~PORT_EVCTRL_PID0_Msk;
	tmp |= PORT_EVCTRL_PID0(data);
	((PortGroup *)hw)->EVCTRL.reg = tmp;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_portgroup_clear_EVCTRL_PID0_bf(const void *const hw, hri_port_evctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->EVCTRL.reg &= ~PORT_EVCTRL_PID0(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_portgroup_toggle_EVCTRL_PID0_bf(const void *const hw, hri_port_evctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->EVCTRL.reg ^= PORT_EVCTRL_PID0(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline hri_port_evctrl_reg_t hri_portgroup_read_EVCTRL_PID0_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((PortGroup *)hw)->EVCTRL.reg;
	tmp = (tmp & PORT_EVCTRL_PID0_Msk) >> PORT_EVCTRL_PID0_Pos;
	return tmp;
}

static inline void hri_portgroup_set_EVCTRL_EVACT0_bf(const void *const hw, hri_port_evctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->EVCTRL.reg |= PORT_EVCTRL_EVACT0(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline hri_port_evctrl_reg_t hri_portgroup_get_EVCTRL_EVACT0_bf(const void *const hw, hri_port_evctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((PortGroup *)hw)->EVCTRL.reg;
	tmp = (tmp & PORT_EVCTRL_EVACT0(mask)) >> PORT_EVCTRL_EVACT0_Pos;
	return tmp;
}

static inline void hri_portgroup_write_EVCTRL_EVACT0_bf(const void *const hw, hri_port_evctrl_reg_t data)
{
	uint32_t tmp;
	PORT_CRITICAL_SECTION_ENTER();
	tmp = ((PortGroup *)hw)->EVCTRL.reg;
	tmp &= ~PORT_EVCTRL_EVACT0_Msk;
	tmp |= PORT_EVCTRL_EVACT0(data);
	((PortGroup *)hw)->EVCTRL.reg = tmp;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_portgroup_clear_EVCTRL_EVACT0_bf(const void *const hw, hri_port_evctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->EVCTRL.reg &= ~PORT_EVCTRL_EVACT0(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_portgroup_toggle_EVCTRL_EVACT0_bf(const void *const hw, hri_port_evctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->EVCTRL.reg ^= PORT_EVCTRL_EVACT0(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline hri_port_evctrl_reg_t hri_portgroup_read_EVCTRL_EVACT0_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((PortGroup *)hw)->EVCTRL.reg;
	tmp = (tmp & PORT_EVCTRL_EVACT0_Msk) >> PORT_EVCTRL_EVACT0_Pos;
	return tmp;
}

static inline void hri_portgroup_set_EVCTRL_PID1_bf(const void *const hw, hri_port_evctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->EVCTRL.reg |= PORT_EVCTRL_PID1(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline hri_port_evctrl_reg_t hri_portgroup_get_EVCTRL_PID1_bf(const void *const hw, hri_port_evctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((PortGroup *)hw)->EVCTRL.reg;
	tmp = (tmp & PORT_EVCTRL_PID1(mask)) >> PORT_EVCTRL_PID1_Pos;
	return tmp;
}

static inline void hri_portgroup_write_EVCTRL_PID1_bf(const void *const hw, hri_port_evctrl_reg_t data)
{
	uint32_t tmp;
	PORT_CRITICAL_SECTION_ENTER();
	tmp = ((PortGroup *)hw)->EVCTRL.reg;
	tmp &= ~PORT_EVCTRL_PID1_Msk;
	tmp |= PORT_EVCTRL_PID1(data);
	((PortGroup *)hw)->EVCTRL.reg = tmp;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_portgroup_clear_EVCTRL_PID1_bf(const void *const hw, hri_port_evctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->EVCTRL.reg &= ~PORT_EVCTRL_PID1(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_portgroup_toggle_EVCTRL_PID1_bf(const void *const hw, hri_port_evctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->EVCTRL.reg ^= PORT_EVCTRL_PID1(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline hri_port_evctrl_reg_t hri_portgroup_read_EVCTRL_PID1_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((PortGroup *)hw)->EVCTRL.reg;
	tmp = (tmp & PORT_EVCTRL_PID1_Msk) >> PORT_EVCTRL_PID1_Pos;
	return tmp;
}

static inline void hri_portgroup_set_EVCTRL_EVACT1_bf(const void *const hw, hri_port_evctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->EVCTRL.reg |= PORT_EVCTRL_EVACT1(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline hri_port_evctrl_reg_t hri_portgroup_get_EVCTRL_EVACT1_bf(const void *const hw, hri_port_evctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((PortGroup *)hw)->EVCTRL.reg;
	tmp = (tmp & PORT_EVCTRL_EVACT1(mask)) >> PORT_EVCTRL_EVACT1_Pos;
	return tmp;
}

static inline void hri_portgroup_write_EVCTRL_EVACT1_bf(const void *const hw, hri_port_evctrl_reg_t data)
{
	uint32_t tmp;
	PORT_CRITICAL_SECTION_ENTER();
	tmp = ((PortGroup *)hw)->EVCTRL.reg;
	tmp &= ~PORT_EVCTRL_EVACT1_Msk;
	tmp |= PORT_EVCTRL_EVACT1(data);
	((PortGroup *)hw)->EVCTRL.reg = tmp;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_portgroup_clear_EVCTRL_EVACT1_bf(const void *const hw, hri_port_evctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->EVCTRL.reg &= ~PORT_EVCTRL_EVACT1(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_portgroup_toggle_EVCTRL_EVACT1_bf(const void *const hw, hri_port_evctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->EVCTRL.reg ^= PORT_EVCTRL_EVACT1(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline hri_port_evctrl_reg_t hri_portgroup_read_EVCTRL_EVACT1_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((PortGroup *)hw)->EVCTRL.reg;
	tmp = (tmp & PORT_EVCTRL_EVACT1_Msk) >> PORT_EVCTRL_EVACT1_Pos;
	return tmp;
}

static inline void hri_portgroup_set_EVCTRL_PID2_bf(const void *const hw, hri_port_evctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->EVCTRL.reg |= PORT_EVCTRL_PID2(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline hri_port_evctrl_reg_t hri_portgroup_get_EVCTRL_PID2_bf(const void *const hw, hri_port_evctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((PortGroup *)hw)->EVCTRL.reg;
	tmp = (tmp & PORT_EVCTRL_PID2(mask)) >> PORT_EVCTRL_PID2_Pos;
	return tmp;
}

static inline void hri_portgroup_write_EVCTRL_PID2_bf(const void *const hw, hri_port_evctrl_reg_t data)
{
	uint32_t tmp;
	PORT_CRITICAL_SECTION_ENTER();
	tmp = ((PortGroup *)hw)->EVCTRL.reg;
	tmp &= ~PORT_EVCTRL_PID2_Msk;
	tmp |= PORT_EVCTRL_PID2(data);
	((PortGroup *)hw)->EVCTRL.reg = tmp;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_portgroup_clear_EVCTRL_PID2_bf(const void *const hw, hri_port_evctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->EVCTRL.reg &= ~PORT_EVCTRL_PID2(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_portgroup_toggle_EVCTRL_PID2_bf(const void *const hw, hri_port_evctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->EVCTRL.reg ^= PORT_EVCTRL_PID2(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline hri_port_evctrl_reg_t hri_portgroup_read_EVCTRL_PID2_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((PortGroup *)hw)->EVCTRL.reg;
	tmp = (tmp & PORT_EVCTRL_PID2_Msk) >> PORT_EVCTRL_PID2_Pos;
	return tmp;
}

static inline void hri_portgroup_set_EVCTRL_EVACT2_bf(const void *const hw, hri_port_evctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->EVCTRL.reg |= PORT_EVCTRL_EVACT2(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline hri_port_evctrl_reg_t hri_portgroup_get_EVCTRL_EVACT2_bf(const void *const hw, hri_port_evctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((PortGroup *)hw)->EVCTRL.reg;
	tmp = (tmp & PORT_EVCTRL_EVACT2(mask)) >> PORT_EVCTRL_EVACT2_Pos;
	return tmp;
}

static inline void hri_portgroup_write_EVCTRL_EVACT2_bf(const void *const hw, hri_port_evctrl_reg_t data)
{
	uint32_t tmp;
	PORT_CRITICAL_SECTION_ENTER();
	tmp = ((PortGroup *)hw)->EVCTRL.reg;
	tmp &= ~PORT_EVCTRL_EVACT2_Msk;
	tmp |= PORT_EVCTRL_EVACT2(data);
	((PortGroup *)hw)->EVCTRL.reg = tmp;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_portgroup_clear_EVCTRL_EVACT2_bf(const void *const hw, hri_port_evctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->EVCTRL.reg &= ~PORT_EVCTRL_EVACT2(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_portgroup_toggle_EVCTRL_EVACT2_bf(const void *const hw, hri_port_evctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->EVCTRL.reg ^= PORT_EVCTRL_EVACT2(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline hri_port_evctrl_reg_t hri_portgroup_read_EVCTRL_EVACT2_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((PortGroup *)hw)->EVCTRL.reg;
	tmp = (tmp & PORT_EVCTRL_EVACT2_Msk) >> PORT_EVCTRL_EVACT2_Pos;
	return tmp;
}

static inline void hri_portgroup_set_EVCTRL_PID3_bf(const void *const hw, hri_port_evctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->EVCTRL.reg |= PORT_EVCTRL_PID3(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline hri_port_evctrl_reg_t hri_portgroup_get_EVCTRL_PID3_bf(const void *const hw, hri_port_evctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((PortGroup *)hw)->EVCTRL.reg;
	tmp = (tmp & PORT_EVCTRL_PID3(mask)) >> PORT_EVCTRL_PID3_Pos;
	return tmp;
}

static inline void hri_portgroup_write_EVCTRL_PID3_bf(const void *const hw, hri_port_evctrl_reg_t data)
{
	uint32_t tmp;
	PORT_CRITICAL_SECTION_ENTER();
	tmp = ((PortGroup *)hw)->EVCTRL.reg;
	tmp &= ~PORT_EVCTRL_PID3_Msk;
	tmp |= PORT_EVCTRL_PID3(data);
	((PortGroup *)hw)->EVCTRL.reg = tmp;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_portgroup_clear_EVCTRL_PID3_bf(const void *const hw, hri_port_evctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->EVCTRL.reg &= ~PORT_EVCTRL_PID3(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_portgroup_toggle_EVCTRL_PID3_bf(const void *const hw, hri_port_evctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->EVCTRL.reg ^= PORT_EVCTRL_PID3(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline hri_port_evctrl_reg_t hri_portgroup_read_EVCTRL_PID3_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((PortGroup *)hw)->EVCTRL.reg;
	tmp = (tmp & PORT_EVCTRL_PID3_Msk) >> PORT_EVCTRL_PID3_Pos;
	return tmp;
}

static inline void hri_portgroup_set_EVCTRL_EVACT3_bf(const void *const hw, hri_port_evctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->EVCTRL.reg |= PORT_EVCTRL_EVACT3(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline hri_port_evctrl_reg_t hri_portgroup_get_EVCTRL_EVACT3_bf(const void *const hw, hri_port_evctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((PortGroup *)hw)->EVCTRL.reg;
	tmp = (tmp & PORT_EVCTRL_EVACT3(mask)) >> PORT_EVCTRL_EVACT3_Pos;
	return tmp;
}

static inline void hri_portgroup_write_EVCTRL_EVACT3_bf(const void *const hw, hri_port_evctrl_reg_t data)
{
	uint32_t tmp;
	PORT_CRITICAL_SECTION_ENTER();
	tmp = ((PortGroup *)hw)->EVCTRL.reg;
	tmp &= ~PORT_EVCTRL_EVACT3_Msk;
	tmp |= PORT_EVCTRL_EVACT3(data);
	((PortGroup *)hw)->EVCTRL.reg = tmp;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_portgroup_clear_EVCTRL_EVACT3_bf(const void *const hw, hri_port_evctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->EVCTRL.reg &= ~PORT_EVCTRL_EVACT3(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_portgroup_toggle_EVCTRL_EVACT3_bf(const void *const hw, hri_port_evctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->EVCTRL.reg ^= PORT_EVCTRL_EVACT3(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline hri_port_evctrl_reg_t hri_portgroup_read_EVCTRL_EVACT3_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((PortGroup *)hw)->EVCTRL.reg;
	tmp = (tmp & PORT_EVCTRL_EVACT3_Msk) >> PORT_EVCTRL_EVACT3_Pos;
	return tmp;
}

static inline void hri_portgroup_set_EVCTRL_reg(const void *const hw, hri_port_evctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->EVCTRL.reg |= mask;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline hri_port_evctrl_reg_t hri_portgroup_get_EVCTRL_reg(const void *const hw, hri_port_evctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((PortGroup *)hw)->EVCTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_portgroup_write_EVCTRL_reg(const void *const hw, hri_port_evctrl_reg_t data)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->EVCTRL.reg = data;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_portgroup_clear_EVCTRL_reg(const void *const hw, hri_port_evctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->EVCTRL.reg &= ~mask;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_portgroup_toggle_EVCTRL_reg(const void *const hw, hri_port_evctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->EVCTRL.reg ^= mask;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline hri_port_evctrl_reg_t hri_portgroup_read_EVCTRL_reg(const void *const hw)
{
	return ((PortGroup *)hw)->EVCTRL.reg;
}

static inline void hri_portgroup_set_PMUX_PMUXE_bf(const void *const hw, uint8_t index, hri_port_pmux_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->PMUX[index].reg |= PORT_PMUX_PMUXE(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline hri_port_pmux_reg_t hri_portgroup_get_PMUX_PMUXE_bf(const void *const hw, uint8_t index,
                                                                  hri_port_pmux_reg_t mask)
{
	uint8_t tmp;
	tmp = ((PortGroup *)hw)->PMUX[index].reg;
	tmp = (tmp & PORT_PMUX_PMUXE(mask)) >> PORT_PMUX_PMUXE_Pos;
	return tmp;
}

static inline void hri_portgroup_write_PMUX_PMUXE_bf(const void *const hw, uint8_t index, hri_port_pmux_reg_t data)
{
	uint8_t tmp;
	PORT_CRITICAL_SECTION_ENTER();
	tmp = ((PortGroup *)hw)->PMUX[index].reg;
	tmp &= ~PORT_PMUX_PMUXE_Msk;
	tmp |= PORT_PMUX_PMUXE(data);
	((PortGroup *)hw)->PMUX[index].reg = tmp;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_portgroup_clear_PMUX_PMUXE_bf(const void *const hw, uint8_t index, hri_port_pmux_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->PMUX[index].reg &= ~PORT_PMUX_PMUXE(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_portgroup_toggle_PMUX_PMUXE_bf(const void *const hw, uint8_t index, hri_port_pmux_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->PMUX[index].reg ^= PORT_PMUX_PMUXE(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline hri_port_pmux_reg_t hri_portgroup_read_PMUX_PMUXE_bf(const void *const hw, uint8_t index)
{
	uint8_t tmp;
	tmp = ((PortGroup *)hw)->PMUX[index].reg;
	tmp = (tmp & PORT_PMUX_PMUXE_Msk) >> PORT_PMUX_PMUXE_Pos;
	return tmp;
}

static inline void hri_portgroup_set_PMUX_PMUXO_bf(const void *const hw, uint8_t index, hri_port_pmux_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->PMUX[index].reg |= PORT_PMUX_PMUXO(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline hri_port_pmux_reg_t hri_portgroup_get_PMUX_PMUXO_bf(const void *const hw, uint8_t index,
                                                                  hri_port_pmux_reg_t mask)
{
	uint8_t tmp;
	tmp = ((PortGroup *)hw)->PMUX[index].reg;
	tmp = (tmp & PORT_PMUX_PMUXO(mask)) >> PORT_PMUX_PMUXO_Pos;
	return tmp;
}

static inline void hri_portgroup_write_PMUX_PMUXO_bf(const void *const hw, uint8_t index, hri_port_pmux_reg_t data)
{
	uint8_t tmp;
	PORT_CRITICAL_SECTION_ENTER();
	tmp = ((PortGroup *)hw)->PMUX[index].reg;
	tmp &= ~PORT_PMUX_PMUXO_Msk;
	tmp |= PORT_PMUX_PMUXO(data);
	((PortGroup *)hw)->PMUX[index].reg = tmp;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_portgroup_clear_PMUX_PMUXO_bf(const void *const hw, uint8_t index, hri_port_pmux_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->PMUX[index].reg &= ~PORT_PMUX_PMUXO(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_portgroup_toggle_PMUX_PMUXO_bf(const void *const hw, uint8_t index, hri_port_pmux_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->PMUX[index].reg ^= PORT_PMUX_PMUXO(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline hri_port_pmux_reg_t hri_portgroup_read_PMUX_PMUXO_bf(const void *const hw, uint8_t index)
{
	uint8_t tmp;
	tmp = ((PortGroup *)hw)->PMUX[index].reg;
	tmp = (tmp & PORT_PMUX_PMUXO_Msk) >> PORT_PMUX_PMUXO_Pos;
	return tmp;
}

static inline void hri_portgroup_set_PMUX_reg(const void *const hw, uint8_t index, hri_port_pmux_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->PMUX[index].reg |= mask;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline hri_port_pmux_reg_t hri_portgroup_get_PMUX_reg(const void *const hw, uint8_t index,
                                                             hri_port_pmux_reg_t mask)
{
	uint8_t tmp;
	tmp = ((PortGroup *)hw)->PMUX[index].reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_portgroup_write_PMUX_reg(const void *const hw, uint8_t index, hri_port_pmux_reg_t data)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->PMUX[index].reg = data;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_portgroup_clear_PMUX_reg(const void *const hw, uint8_t index, hri_port_pmux_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->PMUX[index].reg &= ~mask;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_portgroup_toggle_PMUX_reg(const void *const hw, uint8_t index, hri_port_pmux_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->PMUX[index].reg ^= mask;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline hri_port_pmux_reg_t hri_portgroup_read_PMUX_reg(const void *const hw, uint8_t index)
{
	return ((PortGroup *)hw)->PMUX[index].reg;
}

static inline void hri_portgroup_set_PINCFG_PMUXEN_bit(const void *const hw, uint8_t index)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->PINCFG[index].reg |= PORT_PINCFG_PMUXEN;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_portgroup_get_PINCFG_PMUXEN_bit(const void *const hw, uint8_t index)
{
	uint8_t tmp;
	tmp = ((PortGroup *)hw)->PINCFG[index].reg;
	tmp = (tmp & PORT_PINCFG_PMUXEN) >> PORT_PINCFG_PMUXEN_Pos;
	return (bool)tmp;
}

static inline void hri_portgroup_write_PINCFG_PMUXEN_bit(const void *const hw, uint8_t index, bool value)
{
	uint8_t tmp;
	PORT_CRITICAL_SECTION_ENTER();
	tmp = ((PortGroup *)hw)->PINCFG[index].reg;
	tmp &= ~PORT_PINCFG_PMUXEN;
	tmp |= value << PORT_PINCFG_PMUXEN_Pos;
	((PortGroup *)hw)->PINCFG[index].reg = tmp;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_portgroup_clear_PINCFG_PMUXEN_bit(const void *const hw, uint8_t index)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->PINCFG[index].reg &= ~PORT_PINCFG_PMUXEN;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_portgroup_toggle_PINCFG_PMUXEN_bit(const void *const hw, uint8_t index)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->PINCFG[index].reg ^= PORT_PINCFG_PMUXEN;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_portgroup_set_PINCFG_INEN_bit(const void *const hw, uint8_t index)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->PINCFG[index].reg |= PORT_PINCFG_INEN;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_portgroup_get_PINCFG_INEN_bit(const void *const hw, uint8_t index)
{
	uint8_t tmp;
	tmp = ((PortGroup *)hw)->PINCFG[index].reg;
	tmp = (tmp & PORT_PINCFG_INEN) >> PORT_PINCFG_INEN_Pos;
	return (bool)tmp;
}

static inline void hri_portgroup_write_PINCFG_INEN_bit(const void *const hw, uint8_t index, bool value)
{
	uint8_t tmp;
	PORT_CRITICAL_SECTION_ENTER();
	tmp = ((PortGroup *)hw)->PINCFG[index].reg;
	tmp &= ~PORT_PINCFG_INEN;
	tmp |= value << PORT_PINCFG_INEN_Pos;
	((PortGroup *)hw)->PINCFG[index].reg = tmp;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_portgroup_clear_PINCFG_INEN_bit(const void *const hw, uint8_t index)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->PINCFG[index].reg &= ~PORT_PINCFG_INEN;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_portgroup_toggle_PINCFG_INEN_bit(const void *const hw, uint8_t index)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->PINCFG[index].reg ^= PORT_PINCFG_INEN;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_portgroup_set_PINCFG_PULLEN_bit(const void *const hw, uint8_t index)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->PINCFG[index].reg |= PORT_PINCFG_PULLEN;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_portgroup_get_PINCFG_PULLEN_bit(const void *const hw, uint8_t index)
{
	uint8_t tmp;
	tmp = ((PortGroup *)hw)->PINCFG[index].reg;
	tmp = (tmp & PORT_PINCFG_PULLEN) >> PORT_PINCFG_PULLEN_Pos;
	return (bool)tmp;
}

static inline void hri_portgroup_write_PINCFG_PULLEN_bit(const void *const hw, uint8_t index, bool value)
{
	uint8_t tmp;
	PORT_CRITICAL_SECTION_ENTER();
	tmp = ((PortGroup *)hw)->PINCFG[index].reg;
	tmp &= ~PORT_PINCFG_PULLEN;
	tmp |= value << PORT_PINCFG_PULLEN_Pos;
	((PortGroup *)hw)->PINCFG[index].reg = tmp;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_portgroup_clear_PINCFG_PULLEN_bit(const void *const hw, uint8_t index)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->PINCFG[index].reg &= ~PORT_PINCFG_PULLEN;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_portgroup_toggle_PINCFG_PULLEN_bit(const void *const hw, uint8_t index)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->PINCFG[index].reg ^= PORT_PINCFG_PULLEN;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_portgroup_set_PINCFG_DRVSTR_bit(const void *const hw, uint8_t index)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->PINCFG[index].reg |= PORT_PINCFG_DRVSTR;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_portgroup_get_PINCFG_DRVSTR_bit(const void *const hw, uint8_t index)
{
	uint8_t tmp;
	tmp = ((PortGroup *)hw)->PINCFG[index].reg;
	tmp = (tmp & PORT_PINCFG_DRVSTR) >> PORT_PINCFG_DRVSTR_Pos;
	return (bool)tmp;
}

static inline void hri_portgroup_write_PINCFG_DRVSTR_bit(const void *const hw, uint8_t index, bool value)
{
	uint8_t tmp;
	PORT_CRITICAL_SECTION_ENTER();
	tmp = ((PortGroup *)hw)->PINCFG[index].reg;
	tmp &= ~PORT_PINCFG_DRVSTR;
	tmp |= value << PORT_PINCFG_DRVSTR_Pos;
	((PortGroup *)hw)->PINCFG[index].reg = tmp;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_portgroup_clear_PINCFG_DRVSTR_bit(const void *const hw, uint8_t index)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->PINCFG[index].reg &= ~PORT_PINCFG_DRVSTR;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_portgroup_toggle_PINCFG_DRVSTR_bit(const void *const hw, uint8_t index)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->PINCFG[index].reg ^= PORT_PINCFG_DRVSTR;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_portgroup_set_PINCFG_reg(const void *const hw, uint8_t index, hri_port_pincfg_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->PINCFG[index].reg |= mask;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline hri_port_pincfg_reg_t hri_portgroup_get_PINCFG_reg(const void *const hw, uint8_t index,
                                                                 hri_port_pincfg_reg_t mask)
{
	uint8_t tmp;
	tmp = ((PortGroup *)hw)->PINCFG[index].reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_portgroup_write_PINCFG_reg(const void *const hw, uint8_t index, hri_port_pincfg_reg_t data)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->PINCFG[index].reg = data;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_portgroup_clear_PINCFG_reg(const void *const hw, uint8_t index, hri_port_pincfg_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->PINCFG[index].reg &= ~mask;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_portgroup_toggle_PINCFG_reg(const void *const hw, uint8_t index, hri_port_pincfg_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((PortGroup *)hw)->PINCFG[index].reg ^= mask;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline hri_port_pincfg_reg_t hri_portgroup_read_PINCFG_reg(const void *const hw, uint8_t index)
{
	return ((PortGroup *)hw)->PINCFG[index].reg;
}

static inline hri_port_in_reg_t hri_portgroup_get_IN_IN_bf(const void *const hw, hri_port_in_reg_t mask)
{
	return (((PortGroup *)hw)->IN.reg & PORT_IN_IN(mask)) >> PORT_IN_IN_Pos;
}

static inline hri_port_in_reg_t hri_portgroup_read_IN_IN_bf(const void *const hw)
{
	return (((PortGroup *)hw)->IN.reg & PORT_IN_IN_Msk) >> PORT_IN_IN_Pos;
}

static inline hri_port_in_reg_t hri_portgroup_get_IN_reg(const void *const hw, hri_port_in_reg_t mask)
{
	uint32_t tmp;
	tmp = ((PortGroup *)hw)->IN.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_port_in_reg_t hri_portgroup_read_IN_reg(const void *const hw)
{
	return ((PortGroup *)hw)->IN.reg;
}

static inline void hri_port_set_DIR_DIR_bf(const void *const hw, uint8_t submodule_index, hri_port_dir_reg_t mask)
{
	((Port *)hw)->Group[submodule_index].DIRSET.reg = PORT_DIR_DIR(mask);
}

static inline hri_port_dir_reg_t hri_port_get_DIR_DIR_bf(const void *const hw, uint8_t submodule_index,
                                                         hri_port_dir_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Port *)hw)->Group[submodule_index].DIR.reg;
	tmp = (tmp & PORT_DIR_DIR(mask)) >> PORT_DIR_DIR_Pos;
	return tmp;
}

static inline hri_port_dir_reg_t hri_port_read_DIR_DIR_bf(const void *const hw, uint8_t submodule_index)
{
	uint32_t tmp;
	tmp = ((Port *)hw)->Group[submodule_index].DIR.reg;
	tmp = (tmp & PORT_DIR_DIR_Msk) >> PORT_DIR_DIR_Pos;
	return tmp;
}

static inline void hri_port_write_DIR_DIR_bf(const void *const hw, uint8_t submodule_index, hri_port_dir_reg_t data)
{
	((Port *)hw)->Group[submodule_index].DIRSET.reg = PORT_DIR_DIR(data);
	((Port *)hw)->Group[submodule_index].DIRCLR.reg = ~PORT_DIR_DIR(data);
}

static inline void hri_port_clear_DIR_DIR_bf(const void *const hw, uint8_t submodule_index, hri_port_dir_reg_t mask)
{
	((Port *)hw)->Group[submodule_index].DIRCLR.reg = PORT_DIR_DIR(mask);
}

static inline void hri_port_toggle_DIR_DIR_bf(const void *const hw, uint8_t submodule_index, hri_port_dir_reg_t mask)
{
	((Port *)hw)->Group[submodule_index].DIRTGL.reg = PORT_DIR_DIR(mask);
}

static inline void hri_port_set_DIR_reg(const void *const hw, uint8_t submodule_index, hri_port_dir_reg_t mask)
{
	((Port *)hw)->Group[submodule_index].DIRSET.reg = mask;
}

static inline hri_port_dir_reg_t hri_port_get_DIR_reg(const void *const hw, uint8_t submodule_index,
                                                      hri_port_dir_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Port *)hw)->Group[submodule_index].DIR.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_port_dir_reg_t hri_port_read_DIR_reg(const void *const hw, uint8_t submodule_index)
{
	return ((Port *)hw)->Group[submodule_index].DIR.reg;
}

static inline void hri_port_write_DIR_reg(const void *const hw, uint8_t submodule_index, hri_port_dir_reg_t data)
{
	((Port *)hw)->Group[submodule_index].DIRSET.reg = data;
	((Port *)hw)->Group[submodule_index].DIRCLR.reg = ~data;
}

static inline void hri_port_clear_DIR_reg(const void *const hw, uint8_t submodule_index, hri_port_dir_reg_t mask)
{
	((Port *)hw)->Group[submodule_index].DIRCLR.reg = mask;
}

static inline void hri_port_toggle_DIR_reg(const void *const hw, uint8_t submodule_index, hri_port_dir_reg_t mask)
{
	((Port *)hw)->Group[submodule_index].DIRTGL.reg = mask;
}

static inline void hri_port_set_OUT_OUT_bf(const void *const hw, uint8_t submodule_index, hri_port_out_reg_t mask)
{
	((Port *)hw)->Group[submodule_index].OUTSET.reg = PORT_OUT_OUT(mask);
}

static inline hri_port_out_reg_t hri_port_get_OUT_OUT_bf(const void *const hw, uint8_t submodule_index,
                                                         hri_port_out_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Port *)hw)->Group[submodule_index].OUT.reg;
	tmp = (tmp & PORT_OUT_OUT(mask)) >> PORT_OUT_OUT_Pos;
	return tmp;
}

static inline hri_port_out_reg_t hri_port_read_OUT_OUT_bf(const void *const hw, uint8_t submodule_index)
{
	uint32_t tmp;
	tmp = ((Port *)hw)->Group[submodule_index].OUT.reg;
	tmp = (tmp & PORT_OUT_OUT_Msk) >> PORT_OUT_OUT_Pos;
	return tmp;
}

static inline void hri_port_write_OUT_OUT_bf(const void *const hw, uint8_t submodule_index, hri_port_out_reg_t data)
{
	((Port *)hw)->Group[submodule_index].OUTSET.reg = PORT_OUT_OUT(data);
	((Port *)hw)->Group[submodule_index].OUTCLR.reg = ~PORT_OUT_OUT(data);
}

static inline void hri_port_clear_OUT_OUT_bf(const void *const hw, uint8_t submodule_index, hri_port_out_reg_t mask)
{
	((Port *)hw)->Group[submodule_index].OUTCLR.reg = PORT_OUT_OUT(mask);
}

static inline void hri_port_toggle_OUT_OUT_bf(const void *const hw, uint8_t submodule_index, hri_port_out_reg_t mask)
{
	((Port *)hw)->Group[submodule_index].OUTTGL.reg = PORT_OUT_OUT(mask);
}

static inline void hri_port_set_OUT_reg(const void *const hw, uint8_t submodule_index, hri_port_out_reg_t mask)
{
	((Port *)hw)->Group[submodule_index].OUTSET.reg = mask;
}

static inline hri_port_out_reg_t hri_port_get_OUT_reg(const void *const hw, uint8_t submodule_index,
                                                      hri_port_out_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Port *)hw)->Group[submodule_index].OUT.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_port_out_reg_t hri_port_read_OUT_reg(const void *const hw, uint8_t submodule_index)
{
	return ((Port *)hw)->Group[submodule_index].OUT.reg;
}

static inline void hri_port_write_OUT_reg(const void *const hw, uint8_t submodule_index, hri_port_out_reg_t data)
{
	((Port *)hw)->Group[submodule_index].OUTSET.reg = data;
	((Port *)hw)->Group[submodule_index].OUTCLR.reg = ~data;
}

static inline void hri_port_clear_OUT_reg(const void *const hw, uint8_t submodule_index, hri_port_out_reg_t mask)
{
	((Port *)hw)->Group[submodule_index].OUTCLR.reg = mask;
}

static inline void hri_port_toggle_OUT_reg(const void *const hw, uint8_t submodule_index, hri_port_out_reg_t mask)
{
	((Port *)hw)->Group[submodule_index].OUTTGL.reg = mask;
}

static inline void hri_port_write_WRCONFIG_reg(const void *const hw, uint8_t submodule_index,
                                               hri_port_wrconfig_reg_t data)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].WRCONFIG.reg = data;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_port_set_CTRL_SAMPLING_bf(const void *const hw, uint8_t submodule_index,
                                                 hri_port_ctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].CTRL.reg |= PORT_CTRL_SAMPLING(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline hri_port_ctrl_reg_t hri_port_get_CTRL_SAMPLING_bf(const void *const hw, uint8_t submodule_index,
                                                                hri_port_ctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Port *)hw)->Group[submodule_index].CTRL.reg;
	tmp = (tmp & PORT_CTRL_SAMPLING(mask)) >> PORT_CTRL_SAMPLING_Pos;
	return tmp;
}

static inline void hri_port_write_CTRL_SAMPLING_bf(const void *const hw, uint8_t submodule_index,
                                                   hri_port_ctrl_reg_t data)
{
	uint32_t tmp;
	PORT_CRITICAL_SECTION_ENTER();
	tmp = ((Port *)hw)->Group[submodule_index].CTRL.reg;
	tmp &= ~PORT_CTRL_SAMPLING_Msk;
	tmp |= PORT_CTRL_SAMPLING(data);
	((Port *)hw)->Group[submodule_index].CTRL.reg = tmp;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_port_clear_CTRL_SAMPLING_bf(const void *const hw, uint8_t submodule_index,
                                                   hri_port_ctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].CTRL.reg &= ~PORT_CTRL_SAMPLING(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_port_toggle_CTRL_SAMPLING_bf(const void *const hw, uint8_t submodule_index,
                                                    hri_port_ctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].CTRL.reg ^= PORT_CTRL_SAMPLING(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline hri_port_ctrl_reg_t hri_port_read_CTRL_SAMPLING_bf(const void *const hw, uint8_t submodule_index)
{
	uint32_t tmp;
	tmp = ((Port *)hw)->Group[submodule_index].CTRL.reg;
	tmp = (tmp & PORT_CTRL_SAMPLING_Msk) >> PORT_CTRL_SAMPLING_Pos;
	return tmp;
}

static inline void hri_port_set_CTRL_reg(const void *const hw, uint8_t submodule_index, hri_port_ctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].CTRL.reg |= mask;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline hri_port_ctrl_reg_t hri_port_get_CTRL_reg(const void *const hw, uint8_t submodule_index,
                                                        hri_port_ctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Port *)hw)->Group[submodule_index].CTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_port_write_CTRL_reg(const void *const hw, uint8_t submodule_index, hri_port_ctrl_reg_t data)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].CTRL.reg = data;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_port_clear_CTRL_reg(const void *const hw, uint8_t submodule_index, hri_port_ctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].CTRL.reg &= ~mask;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_port_toggle_CTRL_reg(const void *const hw, uint8_t submodule_index, hri_port_ctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].CTRL.reg ^= mask;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline hri_port_ctrl_reg_t hri_port_read_CTRL_reg(const void *const hw, uint8_t submodule_index)
{
	return ((Port *)hw)->Group[submodule_index].CTRL.reg;
}

static inline void hri_port_set_EVCTRL_PORTEI0_bit(const void *const hw, uint8_t submodule_index)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].EVCTRL.reg |= PORT_EVCTRL_PORTEI0;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_port_get_EVCTRL_PORTEI0_bit(const void *const hw, uint8_t submodule_index)
{
	uint32_t tmp;
	tmp = ((Port *)hw)->Group[submodule_index].EVCTRL.reg;
	tmp = (tmp & PORT_EVCTRL_PORTEI0) >> PORT_EVCTRL_PORTEI0_Pos;
	return (bool)tmp;
}

static inline void hri_port_write_EVCTRL_PORTEI0_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	uint32_t tmp;
	PORT_CRITICAL_SECTION_ENTER();
	tmp = ((Port *)hw)->Group[submodule_index].EVCTRL.reg;
	tmp &= ~PORT_EVCTRL_PORTEI0;
	tmp |= value << PORT_EVCTRL_PORTEI0_Pos;
	((Port *)hw)->Group[submodule_index].EVCTRL.reg = tmp;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_port_clear_EVCTRL_PORTEI0_bit(const void *const hw, uint8_t submodule_index)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].EVCTRL.reg &= ~PORT_EVCTRL_PORTEI0;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_port_toggle_EVCTRL_PORTEI0_bit(const void *const hw, uint8_t submodule_index)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].EVCTRL.reg ^= PORT_EVCTRL_PORTEI0;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_port_set_EVCTRL_PORTEI1_bit(const void *const hw, uint8_t submodule_index)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].EVCTRL.reg |= PORT_EVCTRL_PORTEI1;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_port_get_EVCTRL_PORTEI1_bit(const void *const hw, uint8_t submodule_index)
{
	uint32_t tmp;
	tmp = ((Port *)hw)->Group[submodule_index].EVCTRL.reg;
	tmp = (tmp & PORT_EVCTRL_PORTEI1) >> PORT_EVCTRL_PORTEI1_Pos;
	return (bool)tmp;
}

static inline void hri_port_write_EVCTRL_PORTEI1_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	uint32_t tmp;
	PORT_CRITICAL_SECTION_ENTER();
	tmp = ((Port *)hw)->Group[submodule_index].EVCTRL.reg;
	tmp &= ~PORT_EVCTRL_PORTEI1;
	tmp |= value << PORT_EVCTRL_PORTEI1_Pos;
	((Port *)hw)->Group[submodule_index].EVCTRL.reg = tmp;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_port_clear_EVCTRL_PORTEI1_bit(const void *const hw, uint8_t submodule_index)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].EVCTRL.reg &= ~PORT_EVCTRL_PORTEI1;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_port_toggle_EVCTRL_PORTEI1_bit(const void *const hw, uint8_t submodule_index)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].EVCTRL.reg ^= PORT_EVCTRL_PORTEI1;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_port_set_EVCTRL_PORTEI2_bit(const void *const hw, uint8_t submodule_index)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].EVCTRL.reg |= PORT_EVCTRL_PORTEI2;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_port_get_EVCTRL_PORTEI2_bit(const void *const hw, uint8_t submodule_index)
{
	uint32_t tmp;
	tmp = ((Port *)hw)->Group[submodule_index].EVCTRL.reg;
	tmp = (tmp & PORT_EVCTRL_PORTEI2) >> PORT_EVCTRL_PORTEI2_Pos;
	return (bool)tmp;
}

static inline void hri_port_write_EVCTRL_PORTEI2_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	uint32_t tmp;
	PORT_CRITICAL_SECTION_ENTER();
	tmp = ((Port *)hw)->Group[submodule_index].EVCTRL.reg;
	tmp &= ~PORT_EVCTRL_PORTEI2;
	tmp |= value << PORT_EVCTRL_PORTEI2_Pos;
	((Port *)hw)->Group[submodule_index].EVCTRL.reg = tmp;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_port_clear_EVCTRL_PORTEI2_bit(const void *const hw, uint8_t submodule_index)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].EVCTRL.reg &= ~PORT_EVCTRL_PORTEI2;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_port_toggle_EVCTRL_PORTEI2_bit(const void *const hw, uint8_t submodule_index)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].EVCTRL.reg ^= PORT_EVCTRL_PORTEI2;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_port_set_EVCTRL_PORTEI3_bit(const void *const hw, uint8_t submodule_index)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].EVCTRL.reg |= PORT_EVCTRL_PORTEI3;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_port_get_EVCTRL_PORTEI3_bit(const void *const hw, uint8_t submodule_index)
{
	uint32_t tmp;
	tmp = ((Port *)hw)->Group[submodule_index].EVCTRL.reg;
	tmp = (tmp & PORT_EVCTRL_PORTEI3) >> PORT_EVCTRL_PORTEI3_Pos;
	return (bool)tmp;
}

static inline void hri_port_write_EVCTRL_PORTEI3_bit(const void *const hw, uint8_t submodule_index, bool value)
{
	uint32_t tmp;
	PORT_CRITICAL_SECTION_ENTER();
	tmp = ((Port *)hw)->Group[submodule_index].EVCTRL.reg;
	tmp &= ~PORT_EVCTRL_PORTEI3;
	tmp |= value << PORT_EVCTRL_PORTEI3_Pos;
	((Port *)hw)->Group[submodule_index].EVCTRL.reg = tmp;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_port_clear_EVCTRL_PORTEI3_bit(const void *const hw, uint8_t submodule_index)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].EVCTRL.reg &= ~PORT_EVCTRL_PORTEI3;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_port_toggle_EVCTRL_PORTEI3_bit(const void *const hw, uint8_t submodule_index)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].EVCTRL.reg ^= PORT_EVCTRL_PORTEI3;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_port_set_EVCTRL_PID0_bf(const void *const hw, uint8_t submodule_index,
                                               hri_port_evctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].EVCTRL.reg |= PORT_EVCTRL_PID0(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline hri_port_evctrl_reg_t hri_port_get_EVCTRL_PID0_bf(const void *const hw, uint8_t submodule_index,
                                                                hri_port_evctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Port *)hw)->Group[submodule_index].EVCTRL.reg;
	tmp = (tmp & PORT_EVCTRL_PID0(mask)) >> PORT_EVCTRL_PID0_Pos;
	return tmp;
}

static inline void hri_port_write_EVCTRL_PID0_bf(const void *const hw, uint8_t submodule_index,
                                                 hri_port_evctrl_reg_t data)
{
	uint32_t tmp;
	PORT_CRITICAL_SECTION_ENTER();
	tmp = ((Port *)hw)->Group[submodule_index].EVCTRL.reg;
	tmp &= ~PORT_EVCTRL_PID0_Msk;
	tmp |= PORT_EVCTRL_PID0(data);
	((Port *)hw)->Group[submodule_index].EVCTRL.reg = tmp;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_port_clear_EVCTRL_PID0_bf(const void *const hw, uint8_t submodule_index,
                                                 hri_port_evctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].EVCTRL.reg &= ~PORT_EVCTRL_PID0(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_port_toggle_EVCTRL_PID0_bf(const void *const hw, uint8_t submodule_index,
                                                  hri_port_evctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].EVCTRL.reg ^= PORT_EVCTRL_PID0(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline hri_port_evctrl_reg_t hri_port_read_EVCTRL_PID0_bf(const void *const hw, uint8_t submodule_index)
{
	uint32_t tmp;
	tmp = ((Port *)hw)->Group[submodule_index].EVCTRL.reg;
	tmp = (tmp & PORT_EVCTRL_PID0_Msk) >> PORT_EVCTRL_PID0_Pos;
	return tmp;
}

static inline void hri_port_set_EVCTRL_EVACT0_bf(const void *const hw, uint8_t submodule_index,
                                                 hri_port_evctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].EVCTRL.reg |= PORT_EVCTRL_EVACT0(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline hri_port_evctrl_reg_t hri_port_get_EVCTRL_EVACT0_bf(const void *const hw, uint8_t submodule_index,
                                                                  hri_port_evctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Port *)hw)->Group[submodule_index].EVCTRL.reg;
	tmp = (tmp & PORT_EVCTRL_EVACT0(mask)) >> PORT_EVCTRL_EVACT0_Pos;
	return tmp;
}

static inline void hri_port_write_EVCTRL_EVACT0_bf(const void *const hw, uint8_t submodule_index,
                                                   hri_port_evctrl_reg_t data)
{
	uint32_t tmp;
	PORT_CRITICAL_SECTION_ENTER();
	tmp = ((Port *)hw)->Group[submodule_index].EVCTRL.reg;
	tmp &= ~PORT_EVCTRL_EVACT0_Msk;
	tmp |= PORT_EVCTRL_EVACT0(data);
	((Port *)hw)->Group[submodule_index].EVCTRL.reg = tmp;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_port_clear_EVCTRL_EVACT0_bf(const void *const hw, uint8_t submodule_index,
                                                   hri_port_evctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].EVCTRL.reg &= ~PORT_EVCTRL_EVACT0(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_port_toggle_EVCTRL_EVACT0_bf(const void *const hw, uint8_t submodule_index,
                                                    hri_port_evctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].EVCTRL.reg ^= PORT_EVCTRL_EVACT0(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline hri_port_evctrl_reg_t hri_port_read_EVCTRL_EVACT0_bf(const void *const hw, uint8_t submodule_index)
{
	uint32_t tmp;
	tmp = ((Port *)hw)->Group[submodule_index].EVCTRL.reg;
	tmp = (tmp & PORT_EVCTRL_EVACT0_Msk) >> PORT_EVCTRL_EVACT0_Pos;
	return tmp;
}

static inline void hri_port_set_EVCTRL_PID1_bf(const void *const hw, uint8_t submodule_index,
                                               hri_port_evctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].EVCTRL.reg |= PORT_EVCTRL_PID1(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline hri_port_evctrl_reg_t hri_port_get_EVCTRL_PID1_bf(const void *const hw, uint8_t submodule_index,
                                                                hri_port_evctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Port *)hw)->Group[submodule_index].EVCTRL.reg;
	tmp = (tmp & PORT_EVCTRL_PID1(mask)) >> PORT_EVCTRL_PID1_Pos;
	return tmp;
}

static inline void hri_port_write_EVCTRL_PID1_bf(const void *const hw, uint8_t submodule_index,
                                                 hri_port_evctrl_reg_t data)
{
	uint32_t tmp;
	PORT_CRITICAL_SECTION_ENTER();
	tmp = ((Port *)hw)->Group[submodule_index].EVCTRL.reg;
	tmp &= ~PORT_EVCTRL_PID1_Msk;
	tmp |= PORT_EVCTRL_PID1(data);
	((Port *)hw)->Group[submodule_index].EVCTRL.reg = tmp;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_port_clear_EVCTRL_PID1_bf(const void *const hw, uint8_t submodule_index,
                                                 hri_port_evctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].EVCTRL.reg &= ~PORT_EVCTRL_PID1(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_port_toggle_EVCTRL_PID1_bf(const void *const hw, uint8_t submodule_index,
                                                  hri_port_evctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].EVCTRL.reg ^= PORT_EVCTRL_PID1(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline hri_port_evctrl_reg_t hri_port_read_EVCTRL_PID1_bf(const void *const hw, uint8_t submodule_index)
{
	uint32_t tmp;
	tmp = ((Port *)hw)->Group[submodule_index].EVCTRL.reg;
	tmp = (tmp & PORT_EVCTRL_PID1_Msk) >> PORT_EVCTRL_PID1_Pos;
	return tmp;
}

static inline void hri_port_set_EVCTRL_EVACT1_bf(const void *const hw, uint8_t submodule_index,
                                                 hri_port_evctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].EVCTRL.reg |= PORT_EVCTRL_EVACT1(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline hri_port_evctrl_reg_t hri_port_get_EVCTRL_EVACT1_bf(const void *const hw, uint8_t submodule_index,
                                                                  hri_port_evctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Port *)hw)->Group[submodule_index].EVCTRL.reg;
	tmp = (tmp & PORT_EVCTRL_EVACT1(mask)) >> PORT_EVCTRL_EVACT1_Pos;
	return tmp;
}

static inline void hri_port_write_EVCTRL_EVACT1_bf(const void *const hw, uint8_t submodule_index,
                                                   hri_port_evctrl_reg_t data)
{
	uint32_t tmp;
	PORT_CRITICAL_SECTION_ENTER();
	tmp = ((Port *)hw)->Group[submodule_index].EVCTRL.reg;
	tmp &= ~PORT_EVCTRL_EVACT1_Msk;
	tmp |= PORT_EVCTRL_EVACT1(data);
	((Port *)hw)->Group[submodule_index].EVCTRL.reg = tmp;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_port_clear_EVCTRL_EVACT1_bf(const void *const hw, uint8_t submodule_index,
                                                   hri_port_evctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].EVCTRL.reg &= ~PORT_EVCTRL_EVACT1(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_port_toggle_EVCTRL_EVACT1_bf(const void *const hw, uint8_t submodule_index,
                                                    hri_port_evctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].EVCTRL.reg ^= PORT_EVCTRL_EVACT1(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline hri_port_evctrl_reg_t hri_port_read_EVCTRL_EVACT1_bf(const void *const hw, uint8_t submodule_index)
{
	uint32_t tmp;
	tmp = ((Port *)hw)->Group[submodule_index].EVCTRL.reg;
	tmp = (tmp & PORT_EVCTRL_EVACT1_Msk) >> PORT_EVCTRL_EVACT1_Pos;
	return tmp;
}

static inline void hri_port_set_EVCTRL_PID2_bf(const void *const hw, uint8_t submodule_index,
                                               hri_port_evctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].EVCTRL.reg |= PORT_EVCTRL_PID2(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline hri_port_evctrl_reg_t hri_port_get_EVCTRL_PID2_bf(const void *const hw, uint8_t submodule_index,
                                                                hri_port_evctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Port *)hw)->Group[submodule_index].EVCTRL.reg;
	tmp = (tmp & PORT_EVCTRL_PID2(mask)) >> PORT_EVCTRL_PID2_Pos;
	return tmp;
}

static inline void hri_port_write_EVCTRL_PID2_bf(const void *const hw, uint8_t submodule_index,
                                                 hri_port_evctrl_reg_t data)
{
	uint32_t tmp;
	PORT_CRITICAL_SECTION_ENTER();
	tmp = ((Port *)hw)->Group[submodule_index].EVCTRL.reg;
	tmp &= ~PORT_EVCTRL_PID2_Msk;
	tmp |= PORT_EVCTRL_PID2(data);
	((Port *)hw)->Group[submodule_index].EVCTRL.reg = tmp;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_port_clear_EVCTRL_PID2_bf(const void *const hw, uint8_t submodule_index,
                                                 hri_port_evctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].EVCTRL.reg &= ~PORT_EVCTRL_PID2(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_port_toggle_EVCTRL_PID2_bf(const void *const hw, uint8_t submodule_index,
                                                  hri_port_evctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].EVCTRL.reg ^= PORT_EVCTRL_PID2(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline hri_port_evctrl_reg_t hri_port_read_EVCTRL_PID2_bf(const void *const hw, uint8_t submodule_index)
{
	uint32_t tmp;
	tmp = ((Port *)hw)->Group[submodule_index].EVCTRL.reg;
	tmp = (tmp & PORT_EVCTRL_PID2_Msk) >> PORT_EVCTRL_PID2_Pos;
	return tmp;
}

static inline void hri_port_set_EVCTRL_EVACT2_bf(const void *const hw, uint8_t submodule_index,
                                                 hri_port_evctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].EVCTRL.reg |= PORT_EVCTRL_EVACT2(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline hri_port_evctrl_reg_t hri_port_get_EVCTRL_EVACT2_bf(const void *const hw, uint8_t submodule_index,
                                                                  hri_port_evctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Port *)hw)->Group[submodule_index].EVCTRL.reg;
	tmp = (tmp & PORT_EVCTRL_EVACT2(mask)) >> PORT_EVCTRL_EVACT2_Pos;
	return tmp;
}

static inline void hri_port_write_EVCTRL_EVACT2_bf(const void *const hw, uint8_t submodule_index,
                                                   hri_port_evctrl_reg_t data)
{
	uint32_t tmp;
	PORT_CRITICAL_SECTION_ENTER();
	tmp = ((Port *)hw)->Group[submodule_index].EVCTRL.reg;
	tmp &= ~PORT_EVCTRL_EVACT2_Msk;
	tmp |= PORT_EVCTRL_EVACT2(data);
	((Port *)hw)->Group[submodule_index].EVCTRL.reg = tmp;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_port_clear_EVCTRL_EVACT2_bf(const void *const hw, uint8_t submodule_index,
                                                   hri_port_evctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].EVCTRL.reg &= ~PORT_EVCTRL_EVACT2(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_port_toggle_EVCTRL_EVACT2_bf(const void *const hw, uint8_t submodule_index,
                                                    hri_port_evctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].EVCTRL.reg ^= PORT_EVCTRL_EVACT2(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline hri_port_evctrl_reg_t hri_port_read_EVCTRL_EVACT2_bf(const void *const hw, uint8_t submodule_index)
{
	uint32_t tmp;
	tmp = ((Port *)hw)->Group[submodule_index].EVCTRL.reg;
	tmp = (tmp & PORT_EVCTRL_EVACT2_Msk) >> PORT_EVCTRL_EVACT2_Pos;
	return tmp;
}

static inline void hri_port_set_EVCTRL_PID3_bf(const void *const hw, uint8_t submodule_index,
                                               hri_port_evctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].EVCTRL.reg |= PORT_EVCTRL_PID3(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline hri_port_evctrl_reg_t hri_port_get_EVCTRL_PID3_bf(const void *const hw, uint8_t submodule_index,
                                                                hri_port_evctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Port *)hw)->Group[submodule_index].EVCTRL.reg;
	tmp = (tmp & PORT_EVCTRL_PID3(mask)) >> PORT_EVCTRL_PID3_Pos;
	return tmp;
}

static inline void hri_port_write_EVCTRL_PID3_bf(const void *const hw, uint8_t submodule_index,
                                                 hri_port_evctrl_reg_t data)
{
	uint32_t tmp;
	PORT_CRITICAL_SECTION_ENTER();
	tmp = ((Port *)hw)->Group[submodule_index].EVCTRL.reg;
	tmp &= ~PORT_EVCTRL_PID3_Msk;
	tmp |= PORT_EVCTRL_PID3(data);
	((Port *)hw)->Group[submodule_index].EVCTRL.reg = tmp;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_port_clear_EVCTRL_PID3_bf(const void *const hw, uint8_t submodule_index,
                                                 hri_port_evctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].EVCTRL.reg &= ~PORT_EVCTRL_PID3(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_port_toggle_EVCTRL_PID3_bf(const void *const hw, uint8_t submodule_index,
                                                  hri_port_evctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].EVCTRL.reg ^= PORT_EVCTRL_PID3(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline hri_port_evctrl_reg_t hri_port_read_EVCTRL_PID3_bf(const void *const hw, uint8_t submodule_index)
{
	uint32_t tmp;
	tmp = ((Port *)hw)->Group[submodule_index].EVCTRL.reg;
	tmp = (tmp & PORT_EVCTRL_PID3_Msk) >> PORT_EVCTRL_PID3_Pos;
	return tmp;
}

static inline void hri_port_set_EVCTRL_EVACT3_bf(const void *const hw, uint8_t submodule_index,
                                                 hri_port_evctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].EVCTRL.reg |= PORT_EVCTRL_EVACT3(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline hri_port_evctrl_reg_t hri_port_get_EVCTRL_EVACT3_bf(const void *const hw, uint8_t submodule_index,
                                                                  hri_port_evctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Port *)hw)->Group[submodule_index].EVCTRL.reg;
	tmp = (tmp & PORT_EVCTRL_EVACT3(mask)) >> PORT_EVCTRL_EVACT3_Pos;
	return tmp;
}

static inline void hri_port_write_EVCTRL_EVACT3_bf(const void *const hw, uint8_t submodule_index,
                                                   hri_port_evctrl_reg_t data)
{
	uint32_t tmp;
	PORT_CRITICAL_SECTION_ENTER();
	tmp = ((Port *)hw)->Group[submodule_index].EVCTRL.reg;
	tmp &= ~PORT_EVCTRL_EVACT3_Msk;
	tmp |= PORT_EVCTRL_EVACT3(data);
	((Port *)hw)->Group[submodule_index].EVCTRL.reg = tmp;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_port_clear_EVCTRL_EVACT3_bf(const void *const hw, uint8_t submodule_index,
                                                   hri_port_evctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].EVCTRL.reg &= ~PORT_EVCTRL_EVACT3(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_port_toggle_EVCTRL_EVACT3_bf(const void *const hw, uint8_t submodule_index,
                                                    hri_port_evctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].EVCTRL.reg ^= PORT_EVCTRL_EVACT3(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline hri_port_evctrl_reg_t hri_port_read_EVCTRL_EVACT3_bf(const void *const hw, uint8_t submodule_index)
{
	uint32_t tmp;
	tmp = ((Port *)hw)->Group[submodule_index].EVCTRL.reg;
	tmp = (tmp & PORT_EVCTRL_EVACT3_Msk) >> PORT_EVCTRL_EVACT3_Pos;
	return tmp;
}

static inline void hri_port_set_EVCTRL_reg(const void *const hw, uint8_t submodule_index, hri_port_evctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].EVCTRL.reg |= mask;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline hri_port_evctrl_reg_t hri_port_get_EVCTRL_reg(const void *const hw, uint8_t submodule_index,
                                                            hri_port_evctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Port *)hw)->Group[submodule_index].EVCTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_port_write_EVCTRL_reg(const void *const hw, uint8_t submodule_index, hri_port_evctrl_reg_t data)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].EVCTRL.reg = data;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_port_clear_EVCTRL_reg(const void *const hw, uint8_t submodule_index, hri_port_evctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].EVCTRL.reg &= ~mask;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_port_toggle_EVCTRL_reg(const void *const hw, uint8_t submodule_index, hri_port_evctrl_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].EVCTRL.reg ^= mask;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline hri_port_evctrl_reg_t hri_port_read_EVCTRL_reg(const void *const hw, uint8_t submodule_index)
{
	return ((Port *)hw)->Group[submodule_index].EVCTRL.reg;
}

static inline void hri_port_set_PMUX_PMUXE_bf(const void *const hw, uint8_t submodule_index, uint8_t index,
                                              hri_port_pmux_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].PMUX[index].reg |= PORT_PMUX_PMUXE(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline hri_port_pmux_reg_t hri_port_get_PMUX_PMUXE_bf(const void *const hw, uint8_t submodule_index,
                                                             uint8_t index, hri_port_pmux_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Port *)hw)->Group[submodule_index].PMUX[index].reg;
	tmp = (tmp & PORT_PMUX_PMUXE(mask)) >> PORT_PMUX_PMUXE_Pos;
	return tmp;
}

static inline void hri_port_write_PMUX_PMUXE_bf(const void *const hw, uint8_t submodule_index, uint8_t index,
                                                hri_port_pmux_reg_t data)
{
	uint8_t tmp;
	PORT_CRITICAL_SECTION_ENTER();
	tmp = ((Port *)hw)->Group[submodule_index].PMUX[index].reg;
	tmp &= ~PORT_PMUX_PMUXE_Msk;
	tmp |= PORT_PMUX_PMUXE(data);
	((Port *)hw)->Group[submodule_index].PMUX[index].reg = tmp;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_port_clear_PMUX_PMUXE_bf(const void *const hw, uint8_t submodule_index, uint8_t index,
                                                hri_port_pmux_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].PMUX[index].reg &= ~PORT_PMUX_PMUXE(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_port_toggle_PMUX_PMUXE_bf(const void *const hw, uint8_t submodule_index, uint8_t index,
                                                 hri_port_pmux_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].PMUX[index].reg ^= PORT_PMUX_PMUXE(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline hri_port_pmux_reg_t hri_port_read_PMUX_PMUXE_bf(const void *const hw, uint8_t submodule_index,
                                                              uint8_t index)
{
	uint8_t tmp;
	tmp = ((Port *)hw)->Group[submodule_index].PMUX[index].reg;
	tmp = (tmp & PORT_PMUX_PMUXE_Msk) >> PORT_PMUX_PMUXE_Pos;
	return tmp;
}

static inline void hri_port_set_PMUX_PMUXO_bf(const void *const hw, uint8_t submodule_index, uint8_t index,
                                              hri_port_pmux_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].PMUX[index].reg |= PORT_PMUX_PMUXO(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline hri_port_pmux_reg_t hri_port_get_PMUX_PMUXO_bf(const void *const hw, uint8_t submodule_index,
                                                             uint8_t index, hri_port_pmux_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Port *)hw)->Group[submodule_index].PMUX[index].reg;
	tmp = (tmp & PORT_PMUX_PMUXO(mask)) >> PORT_PMUX_PMUXO_Pos;
	return tmp;
}

static inline void hri_port_write_PMUX_PMUXO_bf(const void *const hw, uint8_t submodule_index, uint8_t index,
                                                hri_port_pmux_reg_t data)
{
	uint8_t tmp;
	PORT_CRITICAL_SECTION_ENTER();
	tmp = ((Port *)hw)->Group[submodule_index].PMUX[index].reg;
	tmp &= ~PORT_PMUX_PMUXO_Msk;
	tmp |= PORT_PMUX_PMUXO(data);
	((Port *)hw)->Group[submodule_index].PMUX[index].reg = tmp;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_port_clear_PMUX_PMUXO_bf(const void *const hw, uint8_t submodule_index, uint8_t index,
                                                hri_port_pmux_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].PMUX[index].reg &= ~PORT_PMUX_PMUXO(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_port_toggle_PMUX_PMUXO_bf(const void *const hw, uint8_t submodule_index, uint8_t index,
                                                 hri_port_pmux_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].PMUX[index].reg ^= PORT_PMUX_PMUXO(mask);
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline hri_port_pmux_reg_t hri_port_read_PMUX_PMUXO_bf(const void *const hw, uint8_t submodule_index,
                                                              uint8_t index)
{
	uint8_t tmp;
	tmp = ((Port *)hw)->Group[submodule_index].PMUX[index].reg;
	tmp = (tmp & PORT_PMUX_PMUXO_Msk) >> PORT_PMUX_PMUXO_Pos;
	return tmp;
}

static inline void hri_port_set_PMUX_reg(const void *const hw, uint8_t submodule_index, uint8_t index,
                                         hri_port_pmux_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].PMUX[index].reg |= mask;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline hri_port_pmux_reg_t hri_port_get_PMUX_reg(const void *const hw, uint8_t submodule_index, uint8_t index,
                                                        hri_port_pmux_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Port *)hw)->Group[submodule_index].PMUX[index].reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_port_write_PMUX_reg(const void *const hw, uint8_t submodule_index, uint8_t index,
                                           hri_port_pmux_reg_t data)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].PMUX[index].reg = data;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_port_clear_PMUX_reg(const void *const hw, uint8_t submodule_index, uint8_t index,
                                           hri_port_pmux_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].PMUX[index].reg &= ~mask;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_port_toggle_PMUX_reg(const void *const hw, uint8_t submodule_index, uint8_t index,
                                            hri_port_pmux_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].PMUX[index].reg ^= mask;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline hri_port_pmux_reg_t hri_port_read_PMUX_reg(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	return ((Port *)hw)->Group[submodule_index].PMUX[index].reg;
}

static inline void hri_port_set_PINCFG_PMUXEN_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].PINCFG[index].reg |= PORT_PINCFG_PMUXEN;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_port_get_PINCFG_PMUXEN_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	uint8_t tmp;
	tmp = ((Port *)hw)->Group[submodule_index].PINCFG[index].reg;
	tmp = (tmp & PORT_PINCFG_PMUXEN) >> PORT_PINCFG_PMUXEN_Pos;
	return (bool)tmp;
}

static inline void hri_port_write_PINCFG_PMUXEN_bit(const void *const hw, uint8_t submodule_index, uint8_t index,
                                                    bool value)
{
	uint8_t tmp;
	PORT_CRITICAL_SECTION_ENTER();
	tmp = ((Port *)hw)->Group[submodule_index].PINCFG[index].reg;
	tmp &= ~PORT_PINCFG_PMUXEN;
	tmp |= value << PORT_PINCFG_PMUXEN_Pos;
	((Port *)hw)->Group[submodule_index].PINCFG[index].reg = tmp;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_port_clear_PINCFG_PMUXEN_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].PINCFG[index].reg &= ~PORT_PINCFG_PMUXEN;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_port_toggle_PINCFG_PMUXEN_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].PINCFG[index].reg ^= PORT_PINCFG_PMUXEN;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_port_set_PINCFG_INEN_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].PINCFG[index].reg |= PORT_PINCFG_INEN;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_port_get_PINCFG_INEN_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	uint8_t tmp;
	tmp = ((Port *)hw)->Group[submodule_index].PINCFG[index].reg;
	tmp = (tmp & PORT_PINCFG_INEN) >> PORT_PINCFG_INEN_Pos;
	return (bool)tmp;
}

static inline void hri_port_write_PINCFG_INEN_bit(const void *const hw, uint8_t submodule_index, uint8_t index,
                                                  bool value)
{
	uint8_t tmp;
	PORT_CRITICAL_SECTION_ENTER();
	tmp = ((Port *)hw)->Group[submodule_index].PINCFG[index].reg;
	tmp &= ~PORT_PINCFG_INEN;
	tmp |= value << PORT_PINCFG_INEN_Pos;
	((Port *)hw)->Group[submodule_index].PINCFG[index].reg = tmp;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_port_clear_PINCFG_INEN_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].PINCFG[index].reg &= ~PORT_PINCFG_INEN;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_port_toggle_PINCFG_INEN_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].PINCFG[index].reg ^= PORT_PINCFG_INEN;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_port_set_PINCFG_PULLEN_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].PINCFG[index].reg |= PORT_PINCFG_PULLEN;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_port_get_PINCFG_PULLEN_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	uint8_t tmp;
	tmp = ((Port *)hw)->Group[submodule_index].PINCFG[index].reg;
	tmp = (tmp & PORT_PINCFG_PULLEN) >> PORT_PINCFG_PULLEN_Pos;
	return (bool)tmp;
}

static inline void hri_port_write_PINCFG_PULLEN_bit(const void *const hw, uint8_t submodule_index, uint8_t index,
                                                    bool value)
{
	uint8_t tmp;
	PORT_CRITICAL_SECTION_ENTER();
	tmp = ((Port *)hw)->Group[submodule_index].PINCFG[index].reg;
	tmp &= ~PORT_PINCFG_PULLEN;
	tmp |= value << PORT_PINCFG_PULLEN_Pos;
	((Port *)hw)->Group[submodule_index].PINCFG[index].reg = tmp;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_port_clear_PINCFG_PULLEN_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].PINCFG[index].reg &= ~PORT_PINCFG_PULLEN;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_port_toggle_PINCFG_PULLEN_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].PINCFG[index].reg ^= PORT_PINCFG_PULLEN;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_port_set_PINCFG_DRVSTR_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].PINCFG[index].reg |= PORT_PINCFG_DRVSTR;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_port_get_PINCFG_DRVSTR_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	uint8_t tmp;
	tmp = ((Port *)hw)->Group[submodule_index].PINCFG[index].reg;
	tmp = (tmp & PORT_PINCFG_DRVSTR) >> PORT_PINCFG_DRVSTR_Pos;
	return (bool)tmp;
}

static inline void hri_port_write_PINCFG_DRVSTR_bit(const void *const hw, uint8_t submodule_index, uint8_t index,
                                                    bool value)
{
	uint8_t tmp;
	PORT_CRITICAL_SECTION_ENTER();
	tmp = ((Port *)hw)->Group[submodule_index].PINCFG[index].reg;
	tmp &= ~PORT_PINCFG_DRVSTR;
	tmp |= value << PORT_PINCFG_DRVSTR_Pos;
	((Port *)hw)->Group[submodule_index].PINCFG[index].reg = tmp;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_port_clear_PINCFG_DRVSTR_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].PINCFG[index].reg &= ~PORT_PINCFG_DRVSTR;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_port_toggle_PINCFG_DRVSTR_bit(const void *const hw, uint8_t submodule_index, uint8_t index)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].PINCFG[index].reg ^= PORT_PINCFG_DRVSTR;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_port_set_PINCFG_reg(const void *const hw, uint8_t submodule_index, uint8_t index,
                                           hri_port_pincfg_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].PINCFG[index].reg |= mask;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline hri_port_pincfg_reg_t hri_port_get_PINCFG_reg(const void *const hw, uint8_t submodule_index,
                                                            uint8_t index, hri_port_pincfg_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Port *)hw)->Group[submodule_index].PINCFG[index].reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_port_write_PINCFG_reg(const void *const hw, uint8_t submodule_index, uint8_t index,
                                             hri_port_pincfg_reg_t data)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].PINCFG[index].reg = data;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_port_clear_PINCFG_reg(const void *const hw, uint8_t submodule_index, uint8_t index,
                                             hri_port_pincfg_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].PINCFG[index].reg &= ~mask;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline void hri_port_toggle_PINCFG_reg(const void *const hw, uint8_t submodule_index, uint8_t index,
                                              hri_port_pincfg_reg_t mask)
{
	PORT_CRITICAL_SECTION_ENTER();
	((Port *)hw)->Group[submodule_index].PINCFG[index].reg ^= mask;
	PORT_CRITICAL_SECTION_LEAVE();
}

static inline hri_port_pincfg_reg_t hri_port_read_PINCFG_reg(const void *const hw, uint8_t submodule_index,
                                                             uint8_t index)
{
	return ((Port *)hw)->Group[submodule_index].PINCFG[index].reg;
}

static inline hri_port_in_reg_t hri_port_get_IN_IN_bf(const void *const hw, uint8_t submodule_index,
                                                      hri_port_in_reg_t mask)
{
	return (((Port *)hw)->Group[submodule_index].IN.reg & PORT_IN_IN(mask)) >> PORT_IN_IN_Pos;
}

static inline hri_port_in_reg_t hri_port_read_IN_IN_bf(const void *const hw, uint8_t submodule_index)
{
	return (((Port *)hw)->Group[submodule_index].IN.reg & PORT_IN_IN_Msk) >> PORT_IN_IN_Pos;
}

static inline hri_port_in_reg_t hri_port_get_IN_reg(const void *const hw, uint8_t submodule_index,
                                                    hri_port_in_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Port *)hw)->Group[submodule_index].IN.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_port_in_reg_t hri_port_read_IN_reg(const void *const hw, uint8_t submodule_index)
{
	return ((Port *)hw)->Group[submodule_index].IN.reg;
}

#ifdef __cplusplus
}
#endif

#endif /* _HRI_PORT_D51_H_INCLUDED */
#endif /* _SAMD51_PORT_COMPONENT_ */
