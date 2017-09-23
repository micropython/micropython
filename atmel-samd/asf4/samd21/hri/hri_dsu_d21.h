/**
 * \file
 *
 * \brief SAM DSU
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

#ifdef _SAMD21_DSU_COMPONENT_
#ifndef _HRI_DSU_D21_H_INCLUDED_
#define _HRI_DSU_D21_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <hal_atomic.h>

#if defined(ENABLE_DSU_CRITICAL_SECTIONS)
#define DSU_CRITICAL_SECTION_ENTER() CRITICAL_SECTION_ENTER()
#define DSU_CRITICAL_SECTION_LEAVE() CRITICAL_SECTION_LEAVE()
#else
#define DSU_CRITICAL_SECTION_ENTER()
#define DSU_CRITICAL_SECTION_LEAVE()
#endif

typedef uint32_t hri_dsu_addr_reg_t;
typedef uint32_t hri_dsu_cid0_reg_t;
typedef uint32_t hri_dsu_cid1_reg_t;
typedef uint32_t hri_dsu_cid2_reg_t;
typedef uint32_t hri_dsu_cid3_reg_t;
typedef uint32_t hri_dsu_data_reg_t;
typedef uint32_t hri_dsu_dcc_reg_t;
typedef uint32_t hri_dsu_did_reg_t;
typedef uint32_t hri_dsu_end_reg_t;
typedef uint32_t hri_dsu_entry_reg_t;
typedef uint32_t hri_dsu_length_reg_t;
typedef uint32_t hri_dsu_memtype_reg_t;
typedef uint32_t hri_dsu_pid0_reg_t;
typedef uint32_t hri_dsu_pid1_reg_t;
typedef uint32_t hri_dsu_pid2_reg_t;
typedef uint32_t hri_dsu_pid3_reg_t;
typedef uint32_t hri_dsu_pid4_reg_t;
typedef uint8_t  hri_dsu_ctrl_reg_t;
typedef uint8_t  hri_dsu_statusa_reg_t;
typedef uint8_t  hri_dsu_statusb_reg_t;

static inline void hri_dsu_write_CTRL_reg(const void *const hw, hri_dsu_ctrl_reg_t data)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->CTRL.reg = data;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_set_ADDR_ADDR_bf(const void *const hw, hri_dsu_addr_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->ADDR.reg |= DSU_ADDR_ADDR(mask);
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline hri_dsu_addr_reg_t hri_dsu_get_ADDR_ADDR_bf(const void *const hw, hri_dsu_addr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->ADDR.reg;
	tmp = (tmp & DSU_ADDR_ADDR(mask)) >> DSU_ADDR_ADDR_Pos;
	return tmp;
}

static inline void hri_dsu_write_ADDR_ADDR_bf(const void *const hw, hri_dsu_addr_reg_t data)
{
	uint32_t tmp;
	DSU_CRITICAL_SECTION_ENTER();
	tmp = ((Dsu *)hw)->ADDR.reg;
	tmp &= ~DSU_ADDR_ADDR_Msk;
	tmp |= DSU_ADDR_ADDR(data);
	((Dsu *)hw)->ADDR.reg = tmp;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_clear_ADDR_ADDR_bf(const void *const hw, hri_dsu_addr_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->ADDR.reg &= ~DSU_ADDR_ADDR(mask);
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_toggle_ADDR_ADDR_bf(const void *const hw, hri_dsu_addr_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->ADDR.reg ^= DSU_ADDR_ADDR(mask);
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline hri_dsu_addr_reg_t hri_dsu_read_ADDR_ADDR_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->ADDR.reg;
	tmp = (tmp & DSU_ADDR_ADDR_Msk) >> DSU_ADDR_ADDR_Pos;
	return tmp;
}

static inline void hri_dsu_set_ADDR_reg(const void *const hw, hri_dsu_addr_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->ADDR.reg |= mask;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline hri_dsu_addr_reg_t hri_dsu_get_ADDR_reg(const void *const hw, hri_dsu_addr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->ADDR.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_dsu_write_ADDR_reg(const void *const hw, hri_dsu_addr_reg_t data)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->ADDR.reg = data;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_clear_ADDR_reg(const void *const hw, hri_dsu_addr_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->ADDR.reg &= ~mask;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_toggle_ADDR_reg(const void *const hw, hri_dsu_addr_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->ADDR.reg ^= mask;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline hri_dsu_addr_reg_t hri_dsu_read_ADDR_reg(const void *const hw)
{
	return ((Dsu *)hw)->ADDR.reg;
}

static inline void hri_dsu_set_LENGTH_LENGTH_bf(const void *const hw, hri_dsu_length_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->LENGTH.reg |= DSU_LENGTH_LENGTH(mask);
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline hri_dsu_length_reg_t hri_dsu_get_LENGTH_LENGTH_bf(const void *const hw, hri_dsu_length_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->LENGTH.reg;
	tmp = (tmp & DSU_LENGTH_LENGTH(mask)) >> DSU_LENGTH_LENGTH_Pos;
	return tmp;
}

static inline void hri_dsu_write_LENGTH_LENGTH_bf(const void *const hw, hri_dsu_length_reg_t data)
{
	uint32_t tmp;
	DSU_CRITICAL_SECTION_ENTER();
	tmp = ((Dsu *)hw)->LENGTH.reg;
	tmp &= ~DSU_LENGTH_LENGTH_Msk;
	tmp |= DSU_LENGTH_LENGTH(data);
	((Dsu *)hw)->LENGTH.reg = tmp;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_clear_LENGTH_LENGTH_bf(const void *const hw, hri_dsu_length_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->LENGTH.reg &= ~DSU_LENGTH_LENGTH(mask);
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_toggle_LENGTH_LENGTH_bf(const void *const hw, hri_dsu_length_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->LENGTH.reg ^= DSU_LENGTH_LENGTH(mask);
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline hri_dsu_length_reg_t hri_dsu_read_LENGTH_LENGTH_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->LENGTH.reg;
	tmp = (tmp & DSU_LENGTH_LENGTH_Msk) >> DSU_LENGTH_LENGTH_Pos;
	return tmp;
}

static inline void hri_dsu_set_LENGTH_reg(const void *const hw, hri_dsu_length_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->LENGTH.reg |= mask;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline hri_dsu_length_reg_t hri_dsu_get_LENGTH_reg(const void *const hw, hri_dsu_length_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->LENGTH.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_dsu_write_LENGTH_reg(const void *const hw, hri_dsu_length_reg_t data)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->LENGTH.reg = data;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_clear_LENGTH_reg(const void *const hw, hri_dsu_length_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->LENGTH.reg &= ~mask;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_toggle_LENGTH_reg(const void *const hw, hri_dsu_length_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->LENGTH.reg ^= mask;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline hri_dsu_length_reg_t hri_dsu_read_LENGTH_reg(const void *const hw)
{
	return ((Dsu *)hw)->LENGTH.reg;
}

static inline void hri_dsu_set_DATA_DATA_bf(const void *const hw, hri_dsu_data_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->DATA.reg |= DSU_DATA_DATA(mask);
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline hri_dsu_data_reg_t hri_dsu_get_DATA_DATA_bf(const void *const hw, hri_dsu_data_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->DATA.reg;
	tmp = (tmp & DSU_DATA_DATA(mask)) >> DSU_DATA_DATA_Pos;
	return tmp;
}

static inline void hri_dsu_write_DATA_DATA_bf(const void *const hw, hri_dsu_data_reg_t data)
{
	uint32_t tmp;
	DSU_CRITICAL_SECTION_ENTER();
	tmp = ((Dsu *)hw)->DATA.reg;
	tmp &= ~DSU_DATA_DATA_Msk;
	tmp |= DSU_DATA_DATA(data);
	((Dsu *)hw)->DATA.reg = tmp;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_clear_DATA_DATA_bf(const void *const hw, hri_dsu_data_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->DATA.reg &= ~DSU_DATA_DATA(mask);
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_toggle_DATA_DATA_bf(const void *const hw, hri_dsu_data_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->DATA.reg ^= DSU_DATA_DATA(mask);
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline hri_dsu_data_reg_t hri_dsu_read_DATA_DATA_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->DATA.reg;
	tmp = (tmp & DSU_DATA_DATA_Msk) >> DSU_DATA_DATA_Pos;
	return tmp;
}

static inline void hri_dsu_set_DATA_reg(const void *const hw, hri_dsu_data_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->DATA.reg |= mask;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline hri_dsu_data_reg_t hri_dsu_get_DATA_reg(const void *const hw, hri_dsu_data_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->DATA.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_dsu_write_DATA_reg(const void *const hw, hri_dsu_data_reg_t data)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->DATA.reg = data;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_clear_DATA_reg(const void *const hw, hri_dsu_data_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->DATA.reg &= ~mask;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_toggle_DATA_reg(const void *const hw, hri_dsu_data_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->DATA.reg ^= mask;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline hri_dsu_data_reg_t hri_dsu_read_DATA_reg(const void *const hw)
{
	return ((Dsu *)hw)->DATA.reg;
}

static inline void hri_dsu_set_DCC_DATA_bf(const void *const hw, uint8_t index, hri_dsu_dcc_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->DCC[index].reg |= DSU_DCC_DATA(mask);
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline hri_dsu_dcc_reg_t hri_dsu_get_DCC_DATA_bf(const void *const hw, uint8_t index, hri_dsu_dcc_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->DCC[index].reg;
	tmp = (tmp & DSU_DCC_DATA(mask)) >> DSU_DCC_DATA_Pos;
	return tmp;
}

static inline void hri_dsu_write_DCC_DATA_bf(const void *const hw, uint8_t index, hri_dsu_dcc_reg_t data)
{
	uint32_t tmp;
	DSU_CRITICAL_SECTION_ENTER();
	tmp = ((Dsu *)hw)->DCC[index].reg;
	tmp &= ~DSU_DCC_DATA_Msk;
	tmp |= DSU_DCC_DATA(data);
	((Dsu *)hw)->DCC[index].reg = tmp;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_clear_DCC_DATA_bf(const void *const hw, uint8_t index, hri_dsu_dcc_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->DCC[index].reg &= ~DSU_DCC_DATA(mask);
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_toggle_DCC_DATA_bf(const void *const hw, uint8_t index, hri_dsu_dcc_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->DCC[index].reg ^= DSU_DCC_DATA(mask);
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline hri_dsu_dcc_reg_t hri_dsu_read_DCC_DATA_bf(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->DCC[index].reg;
	tmp = (tmp & DSU_DCC_DATA_Msk) >> DSU_DCC_DATA_Pos;
	return tmp;
}

static inline void hri_dsu_set_DCC_reg(const void *const hw, uint8_t index, hri_dsu_dcc_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->DCC[index].reg |= mask;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline hri_dsu_dcc_reg_t hri_dsu_get_DCC_reg(const void *const hw, uint8_t index, hri_dsu_dcc_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->DCC[index].reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_dsu_write_DCC_reg(const void *const hw, uint8_t index, hri_dsu_dcc_reg_t data)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->DCC[index].reg = data;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_clear_DCC_reg(const void *const hw, uint8_t index, hri_dsu_dcc_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->DCC[index].reg &= ~mask;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_toggle_DCC_reg(const void *const hw, uint8_t index, hri_dsu_dcc_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->DCC[index].reg ^= mask;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline hri_dsu_dcc_reg_t hri_dsu_read_DCC_reg(const void *const hw, uint8_t index)
{
	return ((Dsu *)hw)->DCC[index].reg;
}

static inline bool hri_dsu_get_STATUSB_PROT_bit(const void *const hw)
{
	return (((Dsu *)hw)->STATUSB.reg & DSU_STATUSB_PROT) >> DSU_STATUSB_PROT_Pos;
}

static inline bool hri_dsu_get_STATUSB_DBGPRES_bit(const void *const hw)
{
	return (((Dsu *)hw)->STATUSB.reg & DSU_STATUSB_DBGPRES) >> DSU_STATUSB_DBGPRES_Pos;
}

static inline bool hri_dsu_get_STATUSB_DCCD0_bit(const void *const hw)
{
	return (((Dsu *)hw)->STATUSB.reg & DSU_STATUSB_DCCD0) >> DSU_STATUSB_DCCD0_Pos;
}

static inline bool hri_dsu_get_STATUSB_DCCD1_bit(const void *const hw)
{
	return (((Dsu *)hw)->STATUSB.reg & DSU_STATUSB_DCCD1) >> DSU_STATUSB_DCCD1_Pos;
}

static inline bool hri_dsu_get_STATUSB_HPE_bit(const void *const hw)
{
	return (((Dsu *)hw)->STATUSB.reg & DSU_STATUSB_HPE) >> DSU_STATUSB_HPE_Pos;
}

static inline hri_dsu_statusb_reg_t hri_dsu_get_STATUSB_reg(const void *const hw, hri_dsu_statusb_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Dsu *)hw)->STATUSB.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_dsu_statusb_reg_t hri_dsu_read_STATUSB_reg(const void *const hw)
{
	return ((Dsu *)hw)->STATUSB.reg;
}

static inline hri_dsu_did_reg_t hri_dsu_get_DID_DEVSEL_bf(const void *const hw, hri_dsu_did_reg_t mask)
{
	return (((Dsu *)hw)->DID.reg & DSU_DID_DEVSEL(mask)) >> DSU_DID_DEVSEL_Pos;
}

static inline hri_dsu_did_reg_t hri_dsu_read_DID_DEVSEL_bf(const void *const hw)
{
	return (((Dsu *)hw)->DID.reg & DSU_DID_DEVSEL_Msk) >> DSU_DID_DEVSEL_Pos;
}

static inline hri_dsu_did_reg_t hri_dsu_get_DID_REVISION_bf(const void *const hw, hri_dsu_did_reg_t mask)
{
	return (((Dsu *)hw)->DID.reg & DSU_DID_REVISION(mask)) >> DSU_DID_REVISION_Pos;
}

static inline hri_dsu_did_reg_t hri_dsu_read_DID_REVISION_bf(const void *const hw)
{
	return (((Dsu *)hw)->DID.reg & DSU_DID_REVISION_Msk) >> DSU_DID_REVISION_Pos;
}

static inline hri_dsu_did_reg_t hri_dsu_get_DID_DIE_bf(const void *const hw, hri_dsu_did_reg_t mask)
{
	return (((Dsu *)hw)->DID.reg & DSU_DID_DIE(mask)) >> DSU_DID_DIE_Pos;
}

static inline hri_dsu_did_reg_t hri_dsu_read_DID_DIE_bf(const void *const hw)
{
	return (((Dsu *)hw)->DID.reg & DSU_DID_DIE_Msk) >> DSU_DID_DIE_Pos;
}

static inline hri_dsu_did_reg_t hri_dsu_get_DID_SERIES_bf(const void *const hw, hri_dsu_did_reg_t mask)
{
	return (((Dsu *)hw)->DID.reg & DSU_DID_SERIES(mask)) >> DSU_DID_SERIES_Pos;
}

static inline hri_dsu_did_reg_t hri_dsu_read_DID_SERIES_bf(const void *const hw)
{
	return (((Dsu *)hw)->DID.reg & DSU_DID_SERIES_Msk) >> DSU_DID_SERIES_Pos;
}

static inline hri_dsu_did_reg_t hri_dsu_get_DID_FAMILY_bf(const void *const hw, hri_dsu_did_reg_t mask)
{
	return (((Dsu *)hw)->DID.reg & DSU_DID_FAMILY(mask)) >> DSU_DID_FAMILY_Pos;
}

static inline hri_dsu_did_reg_t hri_dsu_read_DID_FAMILY_bf(const void *const hw)
{
	return (((Dsu *)hw)->DID.reg & DSU_DID_FAMILY_Msk) >> DSU_DID_FAMILY_Pos;
}

static inline hri_dsu_did_reg_t hri_dsu_get_DID_PROCESSOR_bf(const void *const hw, hri_dsu_did_reg_t mask)
{
	return (((Dsu *)hw)->DID.reg & DSU_DID_PROCESSOR(mask)) >> DSU_DID_PROCESSOR_Pos;
}

static inline hri_dsu_did_reg_t hri_dsu_read_DID_PROCESSOR_bf(const void *const hw)
{
	return (((Dsu *)hw)->DID.reg & DSU_DID_PROCESSOR_Msk) >> DSU_DID_PROCESSOR_Pos;
}

static inline hri_dsu_did_reg_t hri_dsu_get_DID_reg(const void *const hw, hri_dsu_did_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->DID.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_dsu_did_reg_t hri_dsu_read_DID_reg(const void *const hw)
{
	return ((Dsu *)hw)->DID.reg;
}

static inline bool hri_dsu_get_ENTRY_EPRES_bit(const void *const hw, uint8_t index)
{
	return (((Dsu *)hw)->ENTRY[index].reg & DSU_ENTRY_EPRES) >> DSU_ENTRY_EPRES_Pos;
}

static inline bool hri_dsu_get_ENTRY_FMT_bit(const void *const hw, uint8_t index)
{
	return (((Dsu *)hw)->ENTRY[index].reg & DSU_ENTRY_FMT) >> DSU_ENTRY_FMT_Pos;
}

static inline hri_dsu_entry_reg_t hri_dsu_get_ENTRY_ADDOFF_bf(const void *const hw, uint8_t index,
                                                              hri_dsu_entry_reg_t mask)
{
	return (((Dsu *)hw)->ENTRY[index].reg & DSU_ENTRY_ADDOFF(mask)) >> DSU_ENTRY_ADDOFF_Pos;
}

static inline hri_dsu_entry_reg_t hri_dsu_read_ENTRY_ADDOFF_bf(const void *const hw, uint8_t index)
{
	return (((Dsu *)hw)->ENTRY[index].reg & DSU_ENTRY_ADDOFF_Msk) >> DSU_ENTRY_ADDOFF_Pos;
}

static inline hri_dsu_entry_reg_t hri_dsu_get_ENTRY_reg(const void *const hw, uint8_t index, hri_dsu_entry_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->ENTRY[index].reg;
	tmp &= mask;
	return tmp;
}

static inline hri_dsu_entry_reg_t hri_dsu_read_ENTRY_reg(const void *const hw, uint8_t index)
{
	return ((Dsu *)hw)->ENTRY[index].reg;
}

static inline hri_dsu_end_reg_t hri_dsu_get_END_END_bf(const void *const hw, hri_dsu_end_reg_t mask)
{
	return (((Dsu *)hw)->END.reg & DSU_END_END(mask)) >> DSU_END_END_Pos;
}

static inline hri_dsu_end_reg_t hri_dsu_read_END_END_bf(const void *const hw)
{
	return (((Dsu *)hw)->END.reg & DSU_END_END_Msk) >> DSU_END_END_Pos;
}

static inline hri_dsu_end_reg_t hri_dsu_get_END_reg(const void *const hw, hri_dsu_end_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->END.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_dsu_end_reg_t hri_dsu_read_END_reg(const void *const hw)
{
	return ((Dsu *)hw)->END.reg;
}

static inline bool hri_dsu_get_MEMTYPE_SMEMP_bit(const void *const hw)
{
	return (((Dsu *)hw)->MEMTYPE.reg & DSU_MEMTYPE_SMEMP) >> DSU_MEMTYPE_SMEMP_Pos;
}

static inline hri_dsu_memtype_reg_t hri_dsu_get_MEMTYPE_reg(const void *const hw, hri_dsu_memtype_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->MEMTYPE.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_dsu_memtype_reg_t hri_dsu_read_MEMTYPE_reg(const void *const hw)
{
	return ((Dsu *)hw)->MEMTYPE.reg;
}

static inline hri_dsu_pid4_reg_t hri_dsu_get_PID4_JEPCC_bf(const void *const hw, hri_dsu_pid4_reg_t mask)
{
	return (((Dsu *)hw)->PID4.reg & DSU_PID4_JEPCC(mask)) >> DSU_PID4_JEPCC_Pos;
}

static inline hri_dsu_pid4_reg_t hri_dsu_read_PID4_JEPCC_bf(const void *const hw)
{
	return (((Dsu *)hw)->PID4.reg & DSU_PID4_JEPCC_Msk) >> DSU_PID4_JEPCC_Pos;
}

static inline hri_dsu_pid4_reg_t hri_dsu_get_PID4_FKBC_bf(const void *const hw, hri_dsu_pid4_reg_t mask)
{
	return (((Dsu *)hw)->PID4.reg & DSU_PID4_FKBC(mask)) >> DSU_PID4_FKBC_Pos;
}

static inline hri_dsu_pid4_reg_t hri_dsu_read_PID4_FKBC_bf(const void *const hw)
{
	return (((Dsu *)hw)->PID4.reg & DSU_PID4_FKBC_Msk) >> DSU_PID4_FKBC_Pos;
}

static inline hri_dsu_pid4_reg_t hri_dsu_get_PID4_reg(const void *const hw, hri_dsu_pid4_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->PID4.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_dsu_pid4_reg_t hri_dsu_read_PID4_reg(const void *const hw)
{
	return ((Dsu *)hw)->PID4.reg;
}

static inline hri_dsu_pid0_reg_t hri_dsu_get_PID0_PARTNBL_bf(const void *const hw, hri_dsu_pid0_reg_t mask)
{
	return (((Dsu *)hw)->PID0.reg & DSU_PID0_PARTNBL(mask)) >> DSU_PID0_PARTNBL_Pos;
}

static inline hri_dsu_pid0_reg_t hri_dsu_read_PID0_PARTNBL_bf(const void *const hw)
{
	return (((Dsu *)hw)->PID0.reg & DSU_PID0_PARTNBL_Msk) >> DSU_PID0_PARTNBL_Pos;
}

static inline hri_dsu_pid0_reg_t hri_dsu_get_PID0_reg(const void *const hw, hri_dsu_pid0_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->PID0.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_dsu_pid0_reg_t hri_dsu_read_PID0_reg(const void *const hw)
{
	return ((Dsu *)hw)->PID0.reg;
}

static inline hri_dsu_pid1_reg_t hri_dsu_get_PID1_PARTNBH_bf(const void *const hw, hri_dsu_pid1_reg_t mask)
{
	return (((Dsu *)hw)->PID1.reg & DSU_PID1_PARTNBH(mask)) >> DSU_PID1_PARTNBH_Pos;
}

static inline hri_dsu_pid1_reg_t hri_dsu_read_PID1_PARTNBH_bf(const void *const hw)
{
	return (((Dsu *)hw)->PID1.reg & DSU_PID1_PARTNBH_Msk) >> DSU_PID1_PARTNBH_Pos;
}

static inline hri_dsu_pid1_reg_t hri_dsu_get_PID1_JEPIDCL_bf(const void *const hw, hri_dsu_pid1_reg_t mask)
{
	return (((Dsu *)hw)->PID1.reg & DSU_PID1_JEPIDCL(mask)) >> DSU_PID1_JEPIDCL_Pos;
}

static inline hri_dsu_pid1_reg_t hri_dsu_read_PID1_JEPIDCL_bf(const void *const hw)
{
	return (((Dsu *)hw)->PID1.reg & DSU_PID1_JEPIDCL_Msk) >> DSU_PID1_JEPIDCL_Pos;
}

static inline hri_dsu_pid1_reg_t hri_dsu_get_PID1_reg(const void *const hw, hri_dsu_pid1_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->PID1.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_dsu_pid1_reg_t hri_dsu_read_PID1_reg(const void *const hw)
{
	return ((Dsu *)hw)->PID1.reg;
}

static inline bool hri_dsu_get_PID2_JEPU_bit(const void *const hw)
{
	return (((Dsu *)hw)->PID2.reg & DSU_PID2_JEPU) >> DSU_PID2_JEPU_Pos;
}

static inline hri_dsu_pid2_reg_t hri_dsu_get_PID2_JEPIDCH_bf(const void *const hw, hri_dsu_pid2_reg_t mask)
{
	return (((Dsu *)hw)->PID2.reg & DSU_PID2_JEPIDCH(mask)) >> DSU_PID2_JEPIDCH_Pos;
}

static inline hri_dsu_pid2_reg_t hri_dsu_read_PID2_JEPIDCH_bf(const void *const hw)
{
	return (((Dsu *)hw)->PID2.reg & DSU_PID2_JEPIDCH_Msk) >> DSU_PID2_JEPIDCH_Pos;
}

static inline hri_dsu_pid2_reg_t hri_dsu_get_PID2_REVISION_bf(const void *const hw, hri_dsu_pid2_reg_t mask)
{
	return (((Dsu *)hw)->PID2.reg & DSU_PID2_REVISION(mask)) >> DSU_PID2_REVISION_Pos;
}

static inline hri_dsu_pid2_reg_t hri_dsu_read_PID2_REVISION_bf(const void *const hw)
{
	return (((Dsu *)hw)->PID2.reg & DSU_PID2_REVISION_Msk) >> DSU_PID2_REVISION_Pos;
}

static inline hri_dsu_pid2_reg_t hri_dsu_get_PID2_reg(const void *const hw, hri_dsu_pid2_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->PID2.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_dsu_pid2_reg_t hri_dsu_read_PID2_reg(const void *const hw)
{
	return ((Dsu *)hw)->PID2.reg;
}

static inline hri_dsu_pid3_reg_t hri_dsu_get_PID3_CUSMOD_bf(const void *const hw, hri_dsu_pid3_reg_t mask)
{
	return (((Dsu *)hw)->PID3.reg & DSU_PID3_CUSMOD(mask)) >> DSU_PID3_CUSMOD_Pos;
}

static inline hri_dsu_pid3_reg_t hri_dsu_read_PID3_CUSMOD_bf(const void *const hw)
{
	return (((Dsu *)hw)->PID3.reg & DSU_PID3_CUSMOD_Msk) >> DSU_PID3_CUSMOD_Pos;
}

static inline hri_dsu_pid3_reg_t hri_dsu_get_PID3_REVAND_bf(const void *const hw, hri_dsu_pid3_reg_t mask)
{
	return (((Dsu *)hw)->PID3.reg & DSU_PID3_REVAND(mask)) >> DSU_PID3_REVAND_Pos;
}

static inline hri_dsu_pid3_reg_t hri_dsu_read_PID3_REVAND_bf(const void *const hw)
{
	return (((Dsu *)hw)->PID3.reg & DSU_PID3_REVAND_Msk) >> DSU_PID3_REVAND_Pos;
}

static inline hri_dsu_pid3_reg_t hri_dsu_get_PID3_reg(const void *const hw, hri_dsu_pid3_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->PID3.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_dsu_pid3_reg_t hri_dsu_read_PID3_reg(const void *const hw)
{
	return ((Dsu *)hw)->PID3.reg;
}

static inline hri_dsu_cid0_reg_t hri_dsu_get_CID0_PREAMBLEB0_bf(const void *const hw, hri_dsu_cid0_reg_t mask)
{
	return (((Dsu *)hw)->CID0.reg & DSU_CID0_PREAMBLEB0(mask)) >> DSU_CID0_PREAMBLEB0_Pos;
}

static inline hri_dsu_cid0_reg_t hri_dsu_read_CID0_PREAMBLEB0_bf(const void *const hw)
{
	return (((Dsu *)hw)->CID0.reg & DSU_CID0_PREAMBLEB0_Msk) >> DSU_CID0_PREAMBLEB0_Pos;
}

static inline hri_dsu_cid0_reg_t hri_dsu_get_CID0_reg(const void *const hw, hri_dsu_cid0_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->CID0.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_dsu_cid0_reg_t hri_dsu_read_CID0_reg(const void *const hw)
{
	return ((Dsu *)hw)->CID0.reg;
}

static inline hri_dsu_cid1_reg_t hri_dsu_get_CID1_PREAMBLE_bf(const void *const hw, hri_dsu_cid1_reg_t mask)
{
	return (((Dsu *)hw)->CID1.reg & DSU_CID1_PREAMBLE(mask)) >> DSU_CID1_PREAMBLE_Pos;
}

static inline hri_dsu_cid1_reg_t hri_dsu_read_CID1_PREAMBLE_bf(const void *const hw)
{
	return (((Dsu *)hw)->CID1.reg & DSU_CID1_PREAMBLE_Msk) >> DSU_CID1_PREAMBLE_Pos;
}

static inline hri_dsu_cid1_reg_t hri_dsu_get_CID1_CCLASS_bf(const void *const hw, hri_dsu_cid1_reg_t mask)
{
	return (((Dsu *)hw)->CID1.reg & DSU_CID1_CCLASS(mask)) >> DSU_CID1_CCLASS_Pos;
}

static inline hri_dsu_cid1_reg_t hri_dsu_read_CID1_CCLASS_bf(const void *const hw)
{
	return (((Dsu *)hw)->CID1.reg & DSU_CID1_CCLASS_Msk) >> DSU_CID1_CCLASS_Pos;
}

static inline hri_dsu_cid1_reg_t hri_dsu_get_CID1_reg(const void *const hw, hri_dsu_cid1_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->CID1.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_dsu_cid1_reg_t hri_dsu_read_CID1_reg(const void *const hw)
{
	return ((Dsu *)hw)->CID1.reg;
}

static inline hri_dsu_cid2_reg_t hri_dsu_get_CID2_PREAMBLEB2_bf(const void *const hw, hri_dsu_cid2_reg_t mask)
{
	return (((Dsu *)hw)->CID2.reg & DSU_CID2_PREAMBLEB2(mask)) >> DSU_CID2_PREAMBLEB2_Pos;
}

static inline hri_dsu_cid2_reg_t hri_dsu_read_CID2_PREAMBLEB2_bf(const void *const hw)
{
	return (((Dsu *)hw)->CID2.reg & DSU_CID2_PREAMBLEB2_Msk) >> DSU_CID2_PREAMBLEB2_Pos;
}

static inline hri_dsu_cid2_reg_t hri_dsu_get_CID2_reg(const void *const hw, hri_dsu_cid2_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->CID2.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_dsu_cid2_reg_t hri_dsu_read_CID2_reg(const void *const hw)
{
	return ((Dsu *)hw)->CID2.reg;
}

static inline hri_dsu_cid3_reg_t hri_dsu_get_CID3_PREAMBLEB3_bf(const void *const hw, hri_dsu_cid3_reg_t mask)
{
	return (((Dsu *)hw)->CID3.reg & DSU_CID3_PREAMBLEB3(mask)) >> DSU_CID3_PREAMBLEB3_Pos;
}

static inline hri_dsu_cid3_reg_t hri_dsu_read_CID3_PREAMBLEB3_bf(const void *const hw)
{
	return (((Dsu *)hw)->CID3.reg & DSU_CID3_PREAMBLEB3_Msk) >> DSU_CID3_PREAMBLEB3_Pos;
}

static inline hri_dsu_cid3_reg_t hri_dsu_get_CID3_reg(const void *const hw, hri_dsu_cid3_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->CID3.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_dsu_cid3_reg_t hri_dsu_read_CID3_reg(const void *const hw)
{
	return ((Dsu *)hw)->CID3.reg;
}

static inline bool hri_dsu_get_STATUSA_DONE_bit(const void *const hw)
{
	return (((Dsu *)hw)->STATUSA.reg & DSU_STATUSA_DONE) >> DSU_STATUSA_DONE_Pos;
}

static inline void hri_dsu_clear_STATUSA_DONE_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->STATUSA.reg = DSU_STATUSA_DONE;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_STATUSA_CRSTEXT_bit(const void *const hw)
{
	return (((Dsu *)hw)->STATUSA.reg & DSU_STATUSA_CRSTEXT) >> DSU_STATUSA_CRSTEXT_Pos;
}

static inline void hri_dsu_clear_STATUSA_CRSTEXT_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->STATUSA.reg = DSU_STATUSA_CRSTEXT;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_STATUSA_BERR_bit(const void *const hw)
{
	return (((Dsu *)hw)->STATUSA.reg & DSU_STATUSA_BERR) >> DSU_STATUSA_BERR_Pos;
}

static inline void hri_dsu_clear_STATUSA_BERR_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->STATUSA.reg = DSU_STATUSA_BERR;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_STATUSA_FAIL_bit(const void *const hw)
{
	return (((Dsu *)hw)->STATUSA.reg & DSU_STATUSA_FAIL) >> DSU_STATUSA_FAIL_Pos;
}

static inline void hri_dsu_clear_STATUSA_FAIL_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->STATUSA.reg = DSU_STATUSA_FAIL;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_STATUSA_PERR_bit(const void *const hw)
{
	return (((Dsu *)hw)->STATUSA.reg & DSU_STATUSA_PERR) >> DSU_STATUSA_PERR_Pos;
}

static inline void hri_dsu_clear_STATUSA_PERR_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->STATUSA.reg = DSU_STATUSA_PERR;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline hri_dsu_statusa_reg_t hri_dsu_get_STATUSA_reg(const void *const hw, hri_dsu_statusa_reg_t mask)
{
	uint8_t tmp;
	tmp = ((Dsu *)hw)->STATUSA.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_dsu_clear_STATUSA_reg(const void *const hw, hri_dsu_statusa_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->STATUSA.reg = mask;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline hri_dsu_statusa_reg_t hri_dsu_read_STATUSA_reg(const void *const hw)
{
	return ((Dsu *)hw)->STATUSA.reg;
}

#ifdef __cplusplus
}
#endif

#endif /* _HRI_DSU_D21_H_INCLUDED */
#endif /* _SAMD21_DSU_COMPONENT_ */
