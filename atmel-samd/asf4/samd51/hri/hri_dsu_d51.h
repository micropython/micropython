/**
 * \file
 *
 * \brief SAM DSU
 *
 * Copyright (C) 2017 Atmel Corporation. All rights reserved.
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

#ifdef _SAMD51_DSU_COMPONENT_
#ifndef _HRI_DSU_D51_H_INCLUDED_
#define _HRI_DSU_D51_H_INCLUDED_

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
typedef uint32_t hri_dsu_cfg_reg_t;
typedef uint32_t hri_dsu_cid0_reg_t;
typedef uint32_t hri_dsu_cid1_reg_t;
typedef uint32_t hri_dsu_cid2_reg_t;
typedef uint32_t hri_dsu_cid3_reg_t;
typedef uint32_t hri_dsu_data_reg_t;
typedef uint32_t hri_dsu_dcc_reg_t;
typedef uint32_t hri_dsu_dcfg_reg_t;
typedef uint32_t hri_dsu_did_reg_t;
typedef uint32_t hri_dsu_end_reg_t;
typedef uint32_t hri_dsu_entry0_reg_t;
typedef uint32_t hri_dsu_entry1_reg_t;
typedef uint32_t hri_dsu_length_reg_t;
typedef uint32_t hri_dsu_mbafail_reg_t;
typedef uint32_t hri_dsu_mbbusy0_reg_t;
typedef uint32_t hri_dsu_mbconfig_reg_t;
typedef uint32_t hri_dsu_mbcontext_reg_t;
typedef uint32_t hri_dsu_mbctrl_reg_t;
typedef uint32_t hri_dsu_mbdexp_reg_t;
typedef uint32_t hri_dsu_mbdfail_reg_t;
typedef uint32_t hri_dsu_mbenable0_reg_t;
typedef uint32_t hri_dsu_mbgstat_reg_t;
typedef uint32_t hri_dsu_mbstatus0_reg_t;
typedef uint32_t hri_dsu_mbword_reg_t;
typedef uint32_t hri_dsu_memtype_reg_t;
typedef uint32_t hri_dsu_pid0_reg_t;
typedef uint32_t hri_dsu_pid1_reg_t;
typedef uint32_t hri_dsu_pid2_reg_t;
typedef uint32_t hri_dsu_pid3_reg_t;
typedef uint32_t hri_dsu_pid4_reg_t;
typedef uint32_t hri_dsu_pid5_reg_t;
typedef uint32_t hri_dsu_pid6_reg_t;
typedef uint32_t hri_dsu_pid7_reg_t;
typedef uint8_t  hri_dsu_ctrl_reg_t;
typedef uint8_t  hri_dsu_statusa_reg_t;
typedef uint8_t  hri_dsu_statusb_reg_t;

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

static inline bool hri_dsu_get_STATUSB_CELCK_bit(const void *const hw)
{
	return (((Dsu *)hw)->STATUSB.reg & DSU_STATUSB_CELCK) >> DSU_STATUSB_CELCK_Pos;
}

static inline bool hri_dsu_get_STATUSB_TDCCD0_bit(const void *const hw)
{
	return (((Dsu *)hw)->STATUSB.reg & DSU_STATUSB_TDCCD0) >> DSU_STATUSB_TDCCD0_Pos;
}

static inline bool hri_dsu_get_STATUSB_TDCCD1_bit(const void *const hw)
{
	return (((Dsu *)hw)->STATUSB.reg & DSU_STATUSB_TDCCD1) >> DSU_STATUSB_TDCCD1_Pos;
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

static inline hri_dsu_mbdfail_reg_t hri_dsu_get_MBDFAIL_DATA_bf(const void *const hw, hri_dsu_mbdfail_reg_t mask)
{
	return (((Dsu *)hw)->MBDFAIL.reg & DSU_MBDFAIL_DATA(mask)) >> DSU_MBDFAIL_DATA_Pos;
}

static inline hri_dsu_mbdfail_reg_t hri_dsu_read_MBDFAIL_DATA_bf(const void *const hw)
{
	return (((Dsu *)hw)->MBDFAIL.reg & DSU_MBDFAIL_DATA_Msk) >> DSU_MBDFAIL_DATA_Pos;
}

static inline hri_dsu_mbdfail_reg_t hri_dsu_get_MBDFAIL_reg(const void *const hw, hri_dsu_mbdfail_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->MBDFAIL.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_dsu_mbdfail_reg_t hri_dsu_read_MBDFAIL_reg(const void *const hw)
{
	return ((Dsu *)hw)->MBDFAIL.reg;
}

static inline hri_dsu_mbdexp_reg_t hri_dsu_get_MBDEXP_DATA_bf(const void *const hw, hri_dsu_mbdexp_reg_t mask)
{
	return (((Dsu *)hw)->MBDEXP.reg & DSU_MBDEXP_DATA(mask)) >> DSU_MBDEXP_DATA_Pos;
}

static inline hri_dsu_mbdexp_reg_t hri_dsu_read_MBDEXP_DATA_bf(const void *const hw)
{
	return (((Dsu *)hw)->MBDEXP.reg & DSU_MBDEXP_DATA_Msk) >> DSU_MBDEXP_DATA_Pos;
}

static inline hri_dsu_mbdexp_reg_t hri_dsu_get_MBDEXP_reg(const void *const hw, hri_dsu_mbdexp_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->MBDEXP.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_dsu_mbdexp_reg_t hri_dsu_read_MBDEXP_reg(const void *const hw)
{
	return ((Dsu *)hw)->MBDEXP.reg;
}

static inline hri_dsu_mbafail_reg_t hri_dsu_get_MBAFAIL_ADDR_bf(const void *const hw, hri_dsu_mbafail_reg_t mask)
{
	return (((Dsu *)hw)->MBAFAIL.reg & DSU_MBAFAIL_ADDR(mask)) >> DSU_MBAFAIL_ADDR_Pos;
}

static inline hri_dsu_mbafail_reg_t hri_dsu_read_MBAFAIL_ADDR_bf(const void *const hw)
{
	return (((Dsu *)hw)->MBAFAIL.reg & DSU_MBAFAIL_ADDR_Msk) >> DSU_MBAFAIL_ADDR_Pos;
}

static inline hri_dsu_mbafail_reg_t hri_dsu_get_MBAFAIL_reg(const void *const hw, hri_dsu_mbafail_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->MBAFAIL.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_dsu_mbafail_reg_t hri_dsu_read_MBAFAIL_reg(const void *const hw)
{
	return ((Dsu *)hw)->MBAFAIL.reg;
}

static inline bool hri_dsu_get_MBCONTEXT_PORT_bit(const void *const hw)
{
	return (((Dsu *)hw)->MBCONTEXT.reg & DSU_MBCONTEXT_PORT) >> DSU_MBCONTEXT_PORT_Pos;
}

static inline hri_dsu_mbcontext_reg_t hri_dsu_get_MBCONTEXT_SUBSTEP_bf(const void *const       hw,
                                                                       hri_dsu_mbcontext_reg_t mask)
{
	return (((Dsu *)hw)->MBCONTEXT.reg & DSU_MBCONTEXT_SUBSTEP(mask)) >> DSU_MBCONTEXT_SUBSTEP_Pos;
}

static inline hri_dsu_mbcontext_reg_t hri_dsu_read_MBCONTEXT_SUBSTEP_bf(const void *const hw)
{
	return (((Dsu *)hw)->MBCONTEXT.reg & DSU_MBCONTEXT_SUBSTEP_Msk) >> DSU_MBCONTEXT_SUBSTEP_Pos;
}

static inline hri_dsu_mbcontext_reg_t hri_dsu_get_MBCONTEXT_STEP_bf(const void *const hw, hri_dsu_mbcontext_reg_t mask)
{
	return (((Dsu *)hw)->MBCONTEXT.reg & DSU_MBCONTEXT_STEP(mask)) >> DSU_MBCONTEXT_STEP_Pos;
}

static inline hri_dsu_mbcontext_reg_t hri_dsu_read_MBCONTEXT_STEP_bf(const void *const hw)
{
	return (((Dsu *)hw)->MBCONTEXT.reg & DSU_MBCONTEXT_STEP_Msk) >> DSU_MBCONTEXT_STEP_Pos;
}

static inline hri_dsu_mbcontext_reg_t hri_dsu_get_MBCONTEXT_reg(const void *const hw, hri_dsu_mbcontext_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->MBCONTEXT.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_dsu_mbcontext_reg_t hri_dsu_read_MBCONTEXT_reg(const void *const hw)
{
	return ((Dsu *)hw)->MBCONTEXT.reg;
}

static inline bool hri_dsu_get_MBBUSY0_BUSY0_bit(const void *const hw)
{
	return (((Dsu *)hw)->MBBUSY0.reg & DSU_MBBUSY0_BUSY0) >> DSU_MBBUSY0_BUSY0_Pos;
}

static inline bool hri_dsu_get_MBBUSY0_BUSY1_bit(const void *const hw)
{
	return (((Dsu *)hw)->MBBUSY0.reg & DSU_MBBUSY0_BUSY1) >> DSU_MBBUSY0_BUSY1_Pos;
}

static inline bool hri_dsu_get_MBBUSY0_BUSY2_bit(const void *const hw)
{
	return (((Dsu *)hw)->MBBUSY0.reg & DSU_MBBUSY0_BUSY2) >> DSU_MBBUSY0_BUSY2_Pos;
}

static inline bool hri_dsu_get_MBBUSY0_BUSY3_bit(const void *const hw)
{
	return (((Dsu *)hw)->MBBUSY0.reg & DSU_MBBUSY0_BUSY3) >> DSU_MBBUSY0_BUSY3_Pos;
}

static inline bool hri_dsu_get_MBBUSY0_BUSY4_bit(const void *const hw)
{
	return (((Dsu *)hw)->MBBUSY0.reg & DSU_MBBUSY0_BUSY4) >> DSU_MBBUSY0_BUSY4_Pos;
}

static inline bool hri_dsu_get_MBBUSY0_BUSY5_bit(const void *const hw)
{
	return (((Dsu *)hw)->MBBUSY0.reg & DSU_MBBUSY0_BUSY5) >> DSU_MBBUSY0_BUSY5_Pos;
}

static inline bool hri_dsu_get_MBBUSY0_BUSY6_bit(const void *const hw)
{
	return (((Dsu *)hw)->MBBUSY0.reg & DSU_MBBUSY0_BUSY6) >> DSU_MBBUSY0_BUSY6_Pos;
}

static inline bool hri_dsu_get_MBBUSY0_BUSY7_bit(const void *const hw)
{
	return (((Dsu *)hw)->MBBUSY0.reg & DSU_MBBUSY0_BUSY7) >> DSU_MBBUSY0_BUSY7_Pos;
}

static inline bool hri_dsu_get_MBBUSY0_BUSY8_bit(const void *const hw)
{
	return (((Dsu *)hw)->MBBUSY0.reg & DSU_MBBUSY0_BUSY8) >> DSU_MBBUSY0_BUSY8_Pos;
}

static inline bool hri_dsu_get_MBBUSY0_BUSY9_bit(const void *const hw)
{
	return (((Dsu *)hw)->MBBUSY0.reg & DSU_MBBUSY0_BUSY9) >> DSU_MBBUSY0_BUSY9_Pos;
}

static inline bool hri_dsu_get_MBBUSY0_BUSY10_bit(const void *const hw)
{
	return (((Dsu *)hw)->MBBUSY0.reg & DSU_MBBUSY0_BUSY10) >> DSU_MBBUSY0_BUSY10_Pos;
}

static inline bool hri_dsu_get_MBBUSY0_BUSY11_bit(const void *const hw)
{
	return (((Dsu *)hw)->MBBUSY0.reg & DSU_MBBUSY0_BUSY11) >> DSU_MBBUSY0_BUSY11_Pos;
}

static inline bool hri_dsu_get_MBBUSY0_BUSY12_bit(const void *const hw)
{
	return (((Dsu *)hw)->MBBUSY0.reg & DSU_MBBUSY0_BUSY12) >> DSU_MBBUSY0_BUSY12_Pos;
}

static inline bool hri_dsu_get_MBBUSY0_BUSY13_bit(const void *const hw)
{
	return (((Dsu *)hw)->MBBUSY0.reg & DSU_MBBUSY0_BUSY13) >> DSU_MBBUSY0_BUSY13_Pos;
}

static inline bool hri_dsu_get_MBBUSY0_BUSY14_bit(const void *const hw)
{
	return (((Dsu *)hw)->MBBUSY0.reg & DSU_MBBUSY0_BUSY14) >> DSU_MBBUSY0_BUSY14_Pos;
}

static inline bool hri_dsu_get_MBBUSY0_BUSY15_bit(const void *const hw)
{
	return (((Dsu *)hw)->MBBUSY0.reg & DSU_MBBUSY0_BUSY15) >> DSU_MBBUSY0_BUSY15_Pos;
}

static inline bool hri_dsu_get_MBBUSY0_BUSY16_bit(const void *const hw)
{
	return (((Dsu *)hw)->MBBUSY0.reg & DSU_MBBUSY0_BUSY16) >> DSU_MBBUSY0_BUSY16_Pos;
}

static inline bool hri_dsu_get_MBBUSY0_BUSY17_bit(const void *const hw)
{
	return (((Dsu *)hw)->MBBUSY0.reg & DSU_MBBUSY0_BUSY17) >> DSU_MBBUSY0_BUSY17_Pos;
}

static inline bool hri_dsu_get_MBBUSY0_BUSY18_bit(const void *const hw)
{
	return (((Dsu *)hw)->MBBUSY0.reg & DSU_MBBUSY0_BUSY18) >> DSU_MBBUSY0_BUSY18_Pos;
}

static inline bool hri_dsu_get_MBBUSY0_BUSY19_bit(const void *const hw)
{
	return (((Dsu *)hw)->MBBUSY0.reg & DSU_MBBUSY0_BUSY19) >> DSU_MBBUSY0_BUSY19_Pos;
}

static inline bool hri_dsu_get_MBBUSY0_BUSY20_bit(const void *const hw)
{
	return (((Dsu *)hw)->MBBUSY0.reg & DSU_MBBUSY0_BUSY20) >> DSU_MBBUSY0_BUSY20_Pos;
}

static inline bool hri_dsu_get_MBBUSY0_BUSY21_bit(const void *const hw)
{
	return (((Dsu *)hw)->MBBUSY0.reg & DSU_MBBUSY0_BUSY21) >> DSU_MBBUSY0_BUSY21_Pos;
}

static inline bool hri_dsu_get_MBBUSY0_BUSY22_bit(const void *const hw)
{
	return (((Dsu *)hw)->MBBUSY0.reg & DSU_MBBUSY0_BUSY22) >> DSU_MBBUSY0_BUSY22_Pos;
}

static inline bool hri_dsu_get_MBBUSY0_BUSY23_bit(const void *const hw)
{
	return (((Dsu *)hw)->MBBUSY0.reg & DSU_MBBUSY0_BUSY23) >> DSU_MBBUSY0_BUSY23_Pos;
}

static inline bool hri_dsu_get_MBBUSY0_BUSY24_bit(const void *const hw)
{
	return (((Dsu *)hw)->MBBUSY0.reg & DSU_MBBUSY0_BUSY24) >> DSU_MBBUSY0_BUSY24_Pos;
}

static inline bool hri_dsu_get_MBBUSY0_BUSY25_bit(const void *const hw)
{
	return (((Dsu *)hw)->MBBUSY0.reg & DSU_MBBUSY0_BUSY25) >> DSU_MBBUSY0_BUSY25_Pos;
}

static inline bool hri_dsu_get_MBBUSY0_BUSY26_bit(const void *const hw)
{
	return (((Dsu *)hw)->MBBUSY0.reg & DSU_MBBUSY0_BUSY26) >> DSU_MBBUSY0_BUSY26_Pos;
}

static inline bool hri_dsu_get_MBBUSY0_BUSY27_bit(const void *const hw)
{
	return (((Dsu *)hw)->MBBUSY0.reg & DSU_MBBUSY0_BUSY27) >> DSU_MBBUSY0_BUSY27_Pos;
}

static inline bool hri_dsu_get_MBBUSY0_BUSY28_bit(const void *const hw)
{
	return (((Dsu *)hw)->MBBUSY0.reg & DSU_MBBUSY0_BUSY28) >> DSU_MBBUSY0_BUSY28_Pos;
}

static inline hri_dsu_mbbusy0_reg_t hri_dsu_get_MBBUSY0_reg(const void *const hw, hri_dsu_mbbusy0_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->MBBUSY0.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_dsu_mbbusy0_reg_t hri_dsu_read_MBBUSY0_reg(const void *const hw)
{
	return ((Dsu *)hw)->MBBUSY0.reg;
}

static inline bool hri_dsu_get_ENTRY0_EPRES_bit(const void *const hw)
{
	return (((Dsu *)hw)->ENTRY0.reg & DSU_ENTRY0_EPRES) >> DSU_ENTRY0_EPRES_Pos;
}

static inline bool hri_dsu_get_ENTRY0_FMT_bit(const void *const hw)
{
	return (((Dsu *)hw)->ENTRY0.reg & DSU_ENTRY0_FMT) >> DSU_ENTRY0_FMT_Pos;
}

static inline hri_dsu_entry0_reg_t hri_dsu_get_ENTRY0_ADDOFF_bf(const void *const hw, hri_dsu_entry0_reg_t mask)
{
	return (((Dsu *)hw)->ENTRY0.reg & DSU_ENTRY0_ADDOFF(mask)) >> DSU_ENTRY0_ADDOFF_Pos;
}

static inline hri_dsu_entry0_reg_t hri_dsu_read_ENTRY0_ADDOFF_bf(const void *const hw)
{
	return (((Dsu *)hw)->ENTRY0.reg & DSU_ENTRY0_ADDOFF_Msk) >> DSU_ENTRY0_ADDOFF_Pos;
}

static inline hri_dsu_entry0_reg_t hri_dsu_get_ENTRY0_reg(const void *const hw, hri_dsu_entry0_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->ENTRY0.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_dsu_entry0_reg_t hri_dsu_read_ENTRY0_reg(const void *const hw)
{
	return ((Dsu *)hw)->ENTRY0.reg;
}

static inline hri_dsu_entry1_reg_t hri_dsu_get_ENTRY1_reg(const void *const hw, hri_dsu_entry1_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->ENTRY1.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_dsu_entry1_reg_t hri_dsu_read_ENTRY1_reg(const void *const hw)
{
	return ((Dsu *)hw)->ENTRY1.reg;
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

static inline hri_dsu_pid5_reg_t hri_dsu_get_PID5_reg(const void *const hw, hri_dsu_pid5_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->PID5.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_dsu_pid5_reg_t hri_dsu_read_PID5_reg(const void *const hw)
{
	return ((Dsu *)hw)->PID5.reg;
}

static inline hri_dsu_pid6_reg_t hri_dsu_get_PID6_reg(const void *const hw, hri_dsu_pid6_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->PID6.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_dsu_pid6_reg_t hri_dsu_read_PID6_reg(const void *const hw)
{
	return ((Dsu *)hw)->PID6.reg;
}

static inline hri_dsu_pid7_reg_t hri_dsu_get_PID7_reg(const void *const hw, hri_dsu_pid7_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->PID7.reg;
	tmp &= mask;
	return tmp;
}

static inline hri_dsu_pid7_reg_t hri_dsu_read_PID7_reg(const void *const hw)
{
	return ((Dsu *)hw)->PID7.reg;
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

static inline void hri_dsu_set_ADDR_AMOD_bf(const void *const hw, hri_dsu_addr_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->ADDR.reg |= DSU_ADDR_AMOD(mask);
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline hri_dsu_addr_reg_t hri_dsu_get_ADDR_AMOD_bf(const void *const hw, hri_dsu_addr_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->ADDR.reg;
	tmp = (tmp & DSU_ADDR_AMOD(mask)) >> DSU_ADDR_AMOD_Pos;
	return tmp;
}

static inline void hri_dsu_write_ADDR_AMOD_bf(const void *const hw, hri_dsu_addr_reg_t data)
{
	uint32_t tmp;
	DSU_CRITICAL_SECTION_ENTER();
	tmp = ((Dsu *)hw)->ADDR.reg;
	tmp &= ~DSU_ADDR_AMOD_Msk;
	tmp |= DSU_ADDR_AMOD(data);
	((Dsu *)hw)->ADDR.reg = tmp;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_clear_ADDR_AMOD_bf(const void *const hw, hri_dsu_addr_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->ADDR.reg &= ~DSU_ADDR_AMOD(mask);
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_toggle_ADDR_AMOD_bf(const void *const hw, hri_dsu_addr_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->ADDR.reg ^= DSU_ADDR_AMOD(mask);
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline hri_dsu_addr_reg_t hri_dsu_read_ADDR_AMOD_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->ADDR.reg;
	tmp = (tmp & DSU_ADDR_AMOD_Msk) >> DSU_ADDR_AMOD_Pos;
	return tmp;
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

static inline void hri_dsu_set_CFG_ETBRAMEN_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->CFG.reg |= DSU_CFG_ETBRAMEN;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_CFG_ETBRAMEN_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->CFG.reg;
	tmp = (tmp & DSU_CFG_ETBRAMEN) >> DSU_CFG_ETBRAMEN_Pos;
	return (bool)tmp;
}

static inline void hri_dsu_write_CFG_ETBRAMEN_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DSU_CRITICAL_SECTION_ENTER();
	tmp = ((Dsu *)hw)->CFG.reg;
	tmp &= ~DSU_CFG_ETBRAMEN;
	tmp |= value << DSU_CFG_ETBRAMEN_Pos;
	((Dsu *)hw)->CFG.reg = tmp;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_clear_CFG_ETBRAMEN_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->CFG.reg &= ~DSU_CFG_ETBRAMEN;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_toggle_CFG_ETBRAMEN_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->CFG.reg ^= DSU_CFG_ETBRAMEN;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_set_CFG_LQOS_bf(const void *const hw, hri_dsu_cfg_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->CFG.reg |= DSU_CFG_LQOS(mask);
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline hri_dsu_cfg_reg_t hri_dsu_get_CFG_LQOS_bf(const void *const hw, hri_dsu_cfg_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->CFG.reg;
	tmp = (tmp & DSU_CFG_LQOS(mask)) >> DSU_CFG_LQOS_Pos;
	return tmp;
}

static inline void hri_dsu_write_CFG_LQOS_bf(const void *const hw, hri_dsu_cfg_reg_t data)
{
	uint32_t tmp;
	DSU_CRITICAL_SECTION_ENTER();
	tmp = ((Dsu *)hw)->CFG.reg;
	tmp &= ~DSU_CFG_LQOS_Msk;
	tmp |= DSU_CFG_LQOS(data);
	((Dsu *)hw)->CFG.reg = tmp;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_clear_CFG_LQOS_bf(const void *const hw, hri_dsu_cfg_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->CFG.reg &= ~DSU_CFG_LQOS(mask);
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_toggle_CFG_LQOS_bf(const void *const hw, hri_dsu_cfg_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->CFG.reg ^= DSU_CFG_LQOS(mask);
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline hri_dsu_cfg_reg_t hri_dsu_read_CFG_LQOS_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->CFG.reg;
	tmp = (tmp & DSU_CFG_LQOS_Msk) >> DSU_CFG_LQOS_Pos;
	return tmp;
}

static inline void hri_dsu_set_CFG_DCCDMALEVEL_bf(const void *const hw, hri_dsu_cfg_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->CFG.reg |= DSU_CFG_DCCDMALEVEL(mask);
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline hri_dsu_cfg_reg_t hri_dsu_get_CFG_DCCDMALEVEL_bf(const void *const hw, hri_dsu_cfg_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->CFG.reg;
	tmp = (tmp & DSU_CFG_DCCDMALEVEL(mask)) >> DSU_CFG_DCCDMALEVEL_Pos;
	return tmp;
}

static inline void hri_dsu_write_CFG_DCCDMALEVEL_bf(const void *const hw, hri_dsu_cfg_reg_t data)
{
	uint32_t tmp;
	DSU_CRITICAL_SECTION_ENTER();
	tmp = ((Dsu *)hw)->CFG.reg;
	tmp &= ~DSU_CFG_DCCDMALEVEL_Msk;
	tmp |= DSU_CFG_DCCDMALEVEL(data);
	((Dsu *)hw)->CFG.reg = tmp;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_clear_CFG_DCCDMALEVEL_bf(const void *const hw, hri_dsu_cfg_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->CFG.reg &= ~DSU_CFG_DCCDMALEVEL(mask);
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_toggle_CFG_DCCDMALEVEL_bf(const void *const hw, hri_dsu_cfg_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->CFG.reg ^= DSU_CFG_DCCDMALEVEL(mask);
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline hri_dsu_cfg_reg_t hri_dsu_read_CFG_DCCDMALEVEL_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->CFG.reg;
	tmp = (tmp & DSU_CFG_DCCDMALEVEL_Msk) >> DSU_CFG_DCCDMALEVEL_Pos;
	return tmp;
}

static inline void hri_dsu_set_CFG_reg(const void *const hw, hri_dsu_cfg_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->CFG.reg |= mask;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline hri_dsu_cfg_reg_t hri_dsu_get_CFG_reg(const void *const hw, hri_dsu_cfg_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->CFG.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_dsu_write_CFG_reg(const void *const hw, hri_dsu_cfg_reg_t data)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->CFG.reg = data;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_clear_CFG_reg(const void *const hw, hri_dsu_cfg_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->CFG.reg &= ~mask;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_toggle_CFG_reg(const void *const hw, hri_dsu_cfg_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->CFG.reg ^= mask;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline hri_dsu_cfg_reg_t hri_dsu_read_CFG_reg(const void *const hw)
{
	return ((Dsu *)hw)->CFG.reg;
}

static inline void hri_dsu_set_MBCTRL_SWRST_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBCTRL.reg |= DSU_MBCTRL_SWRST;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBCTRL_SWRST_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->MBCTRL.reg;
	tmp = (tmp & DSU_MBCTRL_SWRST) >> DSU_MBCTRL_SWRST_Pos;
	return (bool)tmp;
}

static inline void hri_dsu_set_MBCTRL_ENABLE_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBCTRL.reg |= DSU_MBCTRL_ENABLE;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBCTRL_ENABLE_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->MBCTRL.reg;
	tmp = (tmp & DSU_MBCTRL_ENABLE) >> DSU_MBCTRL_ENABLE_Pos;
	return (bool)tmp;
}

static inline void hri_dsu_write_MBCTRL_ENABLE_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DSU_CRITICAL_SECTION_ENTER();
	tmp = ((Dsu *)hw)->MBCTRL.reg;
	tmp &= ~DSU_MBCTRL_ENABLE;
	tmp |= value << DSU_MBCTRL_ENABLE_Pos;
	((Dsu *)hw)->MBCTRL.reg = tmp;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_clear_MBCTRL_ENABLE_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBCTRL.reg &= ~DSU_MBCTRL_ENABLE;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_toggle_MBCTRL_ENABLE_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBCTRL.reg ^= DSU_MBCTRL_ENABLE;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_set_MBCTRL_reg(const void *const hw, hri_dsu_mbctrl_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBCTRL.reg |= mask;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline hri_dsu_mbctrl_reg_t hri_dsu_get_MBCTRL_reg(const void *const hw, hri_dsu_mbctrl_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->MBCTRL.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_dsu_write_MBCTRL_reg(const void *const hw, hri_dsu_mbctrl_reg_t data)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBCTRL.reg = data;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_clear_MBCTRL_reg(const void *const hw, hri_dsu_mbctrl_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBCTRL.reg &= ~mask;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_toggle_MBCTRL_reg(const void *const hw, hri_dsu_mbctrl_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBCTRL.reg ^= mask;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline hri_dsu_mbctrl_reg_t hri_dsu_read_MBCTRL_reg(const void *const hw)
{
	return ((Dsu *)hw)->MBCTRL.reg;
}

static inline void hri_dsu_set_MBCONFIG_DEFRDMARGIN_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBCONFIG.reg |= DSU_MBCONFIG_DEFRDMARGIN;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBCONFIG_DEFRDMARGIN_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->MBCONFIG.reg;
	tmp = (tmp & DSU_MBCONFIG_DEFRDMARGIN) >> DSU_MBCONFIG_DEFRDMARGIN_Pos;
	return (bool)tmp;
}

static inline void hri_dsu_write_MBCONFIG_DEFRDMARGIN_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DSU_CRITICAL_SECTION_ENTER();
	tmp = ((Dsu *)hw)->MBCONFIG.reg;
	tmp &= ~DSU_MBCONFIG_DEFRDMARGIN;
	tmp |= value << DSU_MBCONFIG_DEFRDMARGIN_Pos;
	((Dsu *)hw)->MBCONFIG.reg = tmp;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_clear_MBCONFIG_DEFRDMARGIN_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBCONFIG.reg &= ~DSU_MBCONFIG_DEFRDMARGIN;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_toggle_MBCONFIG_DEFRDMARGIN_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBCONFIG.reg ^= DSU_MBCONFIG_DEFRDMARGIN;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_set_MBCONFIG_DBG_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBCONFIG.reg |= DSU_MBCONFIG_DBG;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBCONFIG_DBG_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->MBCONFIG.reg;
	tmp = (tmp & DSU_MBCONFIG_DBG) >> DSU_MBCONFIG_DBG_Pos;
	return (bool)tmp;
}

static inline void hri_dsu_write_MBCONFIG_DBG_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DSU_CRITICAL_SECTION_ENTER();
	tmp = ((Dsu *)hw)->MBCONFIG.reg;
	tmp &= ~DSU_MBCONFIG_DBG;
	tmp |= value << DSU_MBCONFIG_DBG_Pos;
	((Dsu *)hw)->MBCONFIG.reg = tmp;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_clear_MBCONFIG_DBG_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBCONFIG.reg &= ~DSU_MBCONFIG_DBG;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_toggle_MBCONFIG_DBG_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBCONFIG.reg ^= DSU_MBCONFIG_DBG;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_set_MBCONFIG_ALGO_bf(const void *const hw, hri_dsu_mbconfig_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBCONFIG.reg |= DSU_MBCONFIG_ALGO(mask);
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline hri_dsu_mbconfig_reg_t hri_dsu_get_MBCONFIG_ALGO_bf(const void *const hw, hri_dsu_mbconfig_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->MBCONFIG.reg;
	tmp = (tmp & DSU_MBCONFIG_ALGO(mask)) >> DSU_MBCONFIG_ALGO_Pos;
	return tmp;
}

static inline void hri_dsu_write_MBCONFIG_ALGO_bf(const void *const hw, hri_dsu_mbconfig_reg_t data)
{
	uint32_t tmp;
	DSU_CRITICAL_SECTION_ENTER();
	tmp = ((Dsu *)hw)->MBCONFIG.reg;
	tmp &= ~DSU_MBCONFIG_ALGO_Msk;
	tmp |= DSU_MBCONFIG_ALGO(data);
	((Dsu *)hw)->MBCONFIG.reg = tmp;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_clear_MBCONFIG_ALGO_bf(const void *const hw, hri_dsu_mbconfig_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBCONFIG.reg &= ~DSU_MBCONFIG_ALGO(mask);
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_toggle_MBCONFIG_ALGO_bf(const void *const hw, hri_dsu_mbconfig_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBCONFIG.reg ^= DSU_MBCONFIG_ALGO(mask);
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline hri_dsu_mbconfig_reg_t hri_dsu_read_MBCONFIG_ALGO_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->MBCONFIG.reg;
	tmp = (tmp & DSU_MBCONFIG_ALGO_Msk) >> DSU_MBCONFIG_ALGO_Pos;
	return tmp;
}

static inline void hri_dsu_set_MBCONFIG_reg(const void *const hw, hri_dsu_mbconfig_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBCONFIG.reg |= mask;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline hri_dsu_mbconfig_reg_t hri_dsu_get_MBCONFIG_reg(const void *const hw, hri_dsu_mbconfig_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->MBCONFIG.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_dsu_write_MBCONFIG_reg(const void *const hw, hri_dsu_mbconfig_reg_t data)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBCONFIG.reg = data;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_clear_MBCONFIG_reg(const void *const hw, hri_dsu_mbconfig_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBCONFIG.reg &= ~mask;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_toggle_MBCONFIG_reg(const void *const hw, hri_dsu_mbconfig_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBCONFIG.reg ^= mask;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline hri_dsu_mbconfig_reg_t hri_dsu_read_MBCONFIG_reg(const void *const hw)
{
	return ((Dsu *)hw)->MBCONFIG.reg;
}

static inline void hri_dsu_set_MBWORD_DATA_bf(const void *const hw, hri_dsu_mbword_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBWORD.reg |= DSU_MBWORD_DATA(mask);
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline hri_dsu_mbword_reg_t hri_dsu_get_MBWORD_DATA_bf(const void *const hw, hri_dsu_mbword_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->MBWORD.reg;
	tmp = (tmp & DSU_MBWORD_DATA(mask)) >> DSU_MBWORD_DATA_Pos;
	return tmp;
}

static inline void hri_dsu_write_MBWORD_DATA_bf(const void *const hw, hri_dsu_mbword_reg_t data)
{
	uint32_t tmp;
	DSU_CRITICAL_SECTION_ENTER();
	tmp = ((Dsu *)hw)->MBWORD.reg;
	tmp &= ~DSU_MBWORD_DATA_Msk;
	tmp |= DSU_MBWORD_DATA(data);
	((Dsu *)hw)->MBWORD.reg = tmp;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_clear_MBWORD_DATA_bf(const void *const hw, hri_dsu_mbword_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBWORD.reg &= ~DSU_MBWORD_DATA(mask);
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_toggle_MBWORD_DATA_bf(const void *const hw, hri_dsu_mbword_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBWORD.reg ^= DSU_MBWORD_DATA(mask);
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline hri_dsu_mbword_reg_t hri_dsu_read_MBWORD_DATA_bf(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->MBWORD.reg;
	tmp = (tmp & DSU_MBWORD_DATA_Msk) >> DSU_MBWORD_DATA_Pos;
	return tmp;
}

static inline void hri_dsu_set_MBWORD_reg(const void *const hw, hri_dsu_mbword_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBWORD.reg |= mask;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline hri_dsu_mbword_reg_t hri_dsu_get_MBWORD_reg(const void *const hw, hri_dsu_mbword_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->MBWORD.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_dsu_write_MBWORD_reg(const void *const hw, hri_dsu_mbword_reg_t data)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBWORD.reg = data;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_clear_MBWORD_reg(const void *const hw, hri_dsu_mbword_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBWORD.reg &= ~mask;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_toggle_MBWORD_reg(const void *const hw, hri_dsu_mbword_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBWORD.reg ^= mask;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline hri_dsu_mbword_reg_t hri_dsu_read_MBWORD_reg(const void *const hw)
{
	return ((Dsu *)hw)->MBWORD.reg;
}

static inline void hri_dsu_set_MBGSTAT_ALLDONE_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBGSTAT.reg |= DSU_MBGSTAT_ALLDONE;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBGSTAT_ALLDONE_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->MBGSTAT.reg;
	tmp = (tmp & DSU_MBGSTAT_ALLDONE) >> DSU_MBGSTAT_ALLDONE_Pos;
	return (bool)tmp;
}

static inline void hri_dsu_write_MBGSTAT_ALLDONE_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DSU_CRITICAL_SECTION_ENTER();
	tmp = ((Dsu *)hw)->MBGSTAT.reg;
	tmp &= ~DSU_MBGSTAT_ALLDONE;
	tmp |= value << DSU_MBGSTAT_ALLDONE_Pos;
	((Dsu *)hw)->MBGSTAT.reg = tmp;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_clear_MBGSTAT_ALLDONE_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBGSTAT.reg &= ~DSU_MBGSTAT_ALLDONE;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_toggle_MBGSTAT_ALLDONE_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBGSTAT.reg ^= DSU_MBGSTAT_ALLDONE;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_set_MBGSTAT_FAILED_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBGSTAT.reg |= DSU_MBGSTAT_FAILED;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBGSTAT_FAILED_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->MBGSTAT.reg;
	tmp = (tmp & DSU_MBGSTAT_FAILED) >> DSU_MBGSTAT_FAILED_Pos;
	return (bool)tmp;
}

static inline void hri_dsu_write_MBGSTAT_FAILED_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DSU_CRITICAL_SECTION_ENTER();
	tmp = ((Dsu *)hw)->MBGSTAT.reg;
	tmp &= ~DSU_MBGSTAT_FAILED;
	tmp |= value << DSU_MBGSTAT_FAILED_Pos;
	((Dsu *)hw)->MBGSTAT.reg = tmp;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_clear_MBGSTAT_FAILED_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBGSTAT.reg &= ~DSU_MBGSTAT_FAILED;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_toggle_MBGSTAT_FAILED_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBGSTAT.reg ^= DSU_MBGSTAT_FAILED;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_set_MBGSTAT_ERRINFO_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBGSTAT.reg |= DSU_MBGSTAT_ERRINFO;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBGSTAT_ERRINFO_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->MBGSTAT.reg;
	tmp = (tmp & DSU_MBGSTAT_ERRINFO) >> DSU_MBGSTAT_ERRINFO_Pos;
	return (bool)tmp;
}

static inline void hri_dsu_write_MBGSTAT_ERRINFO_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DSU_CRITICAL_SECTION_ENTER();
	tmp = ((Dsu *)hw)->MBGSTAT.reg;
	tmp &= ~DSU_MBGSTAT_ERRINFO;
	tmp |= value << DSU_MBGSTAT_ERRINFO_Pos;
	((Dsu *)hw)->MBGSTAT.reg = tmp;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_clear_MBGSTAT_ERRINFO_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBGSTAT.reg &= ~DSU_MBGSTAT_ERRINFO;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_toggle_MBGSTAT_ERRINFO_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBGSTAT.reg ^= DSU_MBGSTAT_ERRINFO;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_set_MBGSTAT_CONFIGURED_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBGSTAT.reg |= DSU_MBGSTAT_CONFIGURED;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBGSTAT_CONFIGURED_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->MBGSTAT.reg;
	tmp = (tmp & DSU_MBGSTAT_CONFIGURED) >> DSU_MBGSTAT_CONFIGURED_Pos;
	return (bool)tmp;
}

static inline void hri_dsu_write_MBGSTAT_CONFIGURED_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DSU_CRITICAL_SECTION_ENTER();
	tmp = ((Dsu *)hw)->MBGSTAT.reg;
	tmp &= ~DSU_MBGSTAT_CONFIGURED;
	tmp |= value << DSU_MBGSTAT_CONFIGURED_Pos;
	((Dsu *)hw)->MBGSTAT.reg = tmp;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_clear_MBGSTAT_CONFIGURED_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBGSTAT.reg &= ~DSU_MBGSTAT_CONFIGURED;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_toggle_MBGSTAT_CONFIGURED_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBGSTAT.reg ^= DSU_MBGSTAT_CONFIGURED;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_set_MBGSTAT_reg(const void *const hw, hri_dsu_mbgstat_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBGSTAT.reg |= mask;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline hri_dsu_mbgstat_reg_t hri_dsu_get_MBGSTAT_reg(const void *const hw, hri_dsu_mbgstat_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->MBGSTAT.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_dsu_write_MBGSTAT_reg(const void *const hw, hri_dsu_mbgstat_reg_t data)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBGSTAT.reg = data;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_clear_MBGSTAT_reg(const void *const hw, hri_dsu_mbgstat_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBGSTAT.reg &= ~mask;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_toggle_MBGSTAT_reg(const void *const hw, hri_dsu_mbgstat_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBGSTAT.reg ^= mask;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline hri_dsu_mbgstat_reg_t hri_dsu_read_MBGSTAT_reg(const void *const hw)
{
	return ((Dsu *)hw)->MBGSTAT.reg;
}

static inline void hri_dsu_set_MBENABLE0_ENABLE0_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg |= DSU_MBENABLE0_ENABLE0;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBENABLE0_ENABLE0_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->MBENABLE0.reg;
	tmp = (tmp & DSU_MBENABLE0_ENABLE0) >> DSU_MBENABLE0_ENABLE0_Pos;
	return (bool)tmp;
}

static inline void hri_dsu_write_MBENABLE0_ENABLE0_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DSU_CRITICAL_SECTION_ENTER();
	tmp = ((Dsu *)hw)->MBENABLE0.reg;
	tmp &= ~DSU_MBENABLE0_ENABLE0;
	tmp |= value << DSU_MBENABLE0_ENABLE0_Pos;
	((Dsu *)hw)->MBENABLE0.reg = tmp;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_clear_MBENABLE0_ENABLE0_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg &= ~DSU_MBENABLE0_ENABLE0;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_toggle_MBENABLE0_ENABLE0_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg ^= DSU_MBENABLE0_ENABLE0;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_set_MBENABLE0_ENABLE1_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg |= DSU_MBENABLE0_ENABLE1;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBENABLE0_ENABLE1_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->MBENABLE0.reg;
	tmp = (tmp & DSU_MBENABLE0_ENABLE1) >> DSU_MBENABLE0_ENABLE1_Pos;
	return (bool)tmp;
}

static inline void hri_dsu_write_MBENABLE0_ENABLE1_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DSU_CRITICAL_SECTION_ENTER();
	tmp = ((Dsu *)hw)->MBENABLE0.reg;
	tmp &= ~DSU_MBENABLE0_ENABLE1;
	tmp |= value << DSU_MBENABLE0_ENABLE1_Pos;
	((Dsu *)hw)->MBENABLE0.reg = tmp;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_clear_MBENABLE0_ENABLE1_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg &= ~DSU_MBENABLE0_ENABLE1;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_toggle_MBENABLE0_ENABLE1_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg ^= DSU_MBENABLE0_ENABLE1;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_set_MBENABLE0_ENABLE2_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg |= DSU_MBENABLE0_ENABLE2;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBENABLE0_ENABLE2_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->MBENABLE0.reg;
	tmp = (tmp & DSU_MBENABLE0_ENABLE2) >> DSU_MBENABLE0_ENABLE2_Pos;
	return (bool)tmp;
}

static inline void hri_dsu_write_MBENABLE0_ENABLE2_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DSU_CRITICAL_SECTION_ENTER();
	tmp = ((Dsu *)hw)->MBENABLE0.reg;
	tmp &= ~DSU_MBENABLE0_ENABLE2;
	tmp |= value << DSU_MBENABLE0_ENABLE2_Pos;
	((Dsu *)hw)->MBENABLE0.reg = tmp;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_clear_MBENABLE0_ENABLE2_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg &= ~DSU_MBENABLE0_ENABLE2;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_toggle_MBENABLE0_ENABLE2_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg ^= DSU_MBENABLE0_ENABLE2;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_set_MBENABLE0_ENABLE3_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg |= DSU_MBENABLE0_ENABLE3;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBENABLE0_ENABLE3_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->MBENABLE0.reg;
	tmp = (tmp & DSU_MBENABLE0_ENABLE3) >> DSU_MBENABLE0_ENABLE3_Pos;
	return (bool)tmp;
}

static inline void hri_dsu_write_MBENABLE0_ENABLE3_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DSU_CRITICAL_SECTION_ENTER();
	tmp = ((Dsu *)hw)->MBENABLE0.reg;
	tmp &= ~DSU_MBENABLE0_ENABLE3;
	tmp |= value << DSU_MBENABLE0_ENABLE3_Pos;
	((Dsu *)hw)->MBENABLE0.reg = tmp;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_clear_MBENABLE0_ENABLE3_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg &= ~DSU_MBENABLE0_ENABLE3;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_toggle_MBENABLE0_ENABLE3_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg ^= DSU_MBENABLE0_ENABLE3;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_set_MBENABLE0_ENABLE4_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg |= DSU_MBENABLE0_ENABLE4;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBENABLE0_ENABLE4_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->MBENABLE0.reg;
	tmp = (tmp & DSU_MBENABLE0_ENABLE4) >> DSU_MBENABLE0_ENABLE4_Pos;
	return (bool)tmp;
}

static inline void hri_dsu_write_MBENABLE0_ENABLE4_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DSU_CRITICAL_SECTION_ENTER();
	tmp = ((Dsu *)hw)->MBENABLE0.reg;
	tmp &= ~DSU_MBENABLE0_ENABLE4;
	tmp |= value << DSU_MBENABLE0_ENABLE4_Pos;
	((Dsu *)hw)->MBENABLE0.reg = tmp;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_clear_MBENABLE0_ENABLE4_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg &= ~DSU_MBENABLE0_ENABLE4;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_toggle_MBENABLE0_ENABLE4_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg ^= DSU_MBENABLE0_ENABLE4;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_set_MBENABLE0_ENABLE5_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg |= DSU_MBENABLE0_ENABLE5;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBENABLE0_ENABLE5_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->MBENABLE0.reg;
	tmp = (tmp & DSU_MBENABLE0_ENABLE5) >> DSU_MBENABLE0_ENABLE5_Pos;
	return (bool)tmp;
}

static inline void hri_dsu_write_MBENABLE0_ENABLE5_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DSU_CRITICAL_SECTION_ENTER();
	tmp = ((Dsu *)hw)->MBENABLE0.reg;
	tmp &= ~DSU_MBENABLE0_ENABLE5;
	tmp |= value << DSU_MBENABLE0_ENABLE5_Pos;
	((Dsu *)hw)->MBENABLE0.reg = tmp;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_clear_MBENABLE0_ENABLE5_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg &= ~DSU_MBENABLE0_ENABLE5;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_toggle_MBENABLE0_ENABLE5_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg ^= DSU_MBENABLE0_ENABLE5;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_set_MBENABLE0_ENABLE6_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg |= DSU_MBENABLE0_ENABLE6;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBENABLE0_ENABLE6_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->MBENABLE0.reg;
	tmp = (tmp & DSU_MBENABLE0_ENABLE6) >> DSU_MBENABLE0_ENABLE6_Pos;
	return (bool)tmp;
}

static inline void hri_dsu_write_MBENABLE0_ENABLE6_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DSU_CRITICAL_SECTION_ENTER();
	tmp = ((Dsu *)hw)->MBENABLE0.reg;
	tmp &= ~DSU_MBENABLE0_ENABLE6;
	tmp |= value << DSU_MBENABLE0_ENABLE6_Pos;
	((Dsu *)hw)->MBENABLE0.reg = tmp;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_clear_MBENABLE0_ENABLE6_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg &= ~DSU_MBENABLE0_ENABLE6;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_toggle_MBENABLE0_ENABLE6_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg ^= DSU_MBENABLE0_ENABLE6;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_set_MBENABLE0_ENABLE7_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg |= DSU_MBENABLE0_ENABLE7;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBENABLE0_ENABLE7_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->MBENABLE0.reg;
	tmp = (tmp & DSU_MBENABLE0_ENABLE7) >> DSU_MBENABLE0_ENABLE7_Pos;
	return (bool)tmp;
}

static inline void hri_dsu_write_MBENABLE0_ENABLE7_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DSU_CRITICAL_SECTION_ENTER();
	tmp = ((Dsu *)hw)->MBENABLE0.reg;
	tmp &= ~DSU_MBENABLE0_ENABLE7;
	tmp |= value << DSU_MBENABLE0_ENABLE7_Pos;
	((Dsu *)hw)->MBENABLE0.reg = tmp;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_clear_MBENABLE0_ENABLE7_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg &= ~DSU_MBENABLE0_ENABLE7;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_toggle_MBENABLE0_ENABLE7_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg ^= DSU_MBENABLE0_ENABLE7;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_set_MBENABLE0_ENABLE8_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg |= DSU_MBENABLE0_ENABLE8;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBENABLE0_ENABLE8_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->MBENABLE0.reg;
	tmp = (tmp & DSU_MBENABLE0_ENABLE8) >> DSU_MBENABLE0_ENABLE8_Pos;
	return (bool)tmp;
}

static inline void hri_dsu_write_MBENABLE0_ENABLE8_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DSU_CRITICAL_SECTION_ENTER();
	tmp = ((Dsu *)hw)->MBENABLE0.reg;
	tmp &= ~DSU_MBENABLE0_ENABLE8;
	tmp |= value << DSU_MBENABLE0_ENABLE8_Pos;
	((Dsu *)hw)->MBENABLE0.reg = tmp;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_clear_MBENABLE0_ENABLE8_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg &= ~DSU_MBENABLE0_ENABLE8;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_toggle_MBENABLE0_ENABLE8_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg ^= DSU_MBENABLE0_ENABLE8;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_set_MBENABLE0_ENABLE9_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg |= DSU_MBENABLE0_ENABLE9;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBENABLE0_ENABLE9_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->MBENABLE0.reg;
	tmp = (tmp & DSU_MBENABLE0_ENABLE9) >> DSU_MBENABLE0_ENABLE9_Pos;
	return (bool)tmp;
}

static inline void hri_dsu_write_MBENABLE0_ENABLE9_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DSU_CRITICAL_SECTION_ENTER();
	tmp = ((Dsu *)hw)->MBENABLE0.reg;
	tmp &= ~DSU_MBENABLE0_ENABLE9;
	tmp |= value << DSU_MBENABLE0_ENABLE9_Pos;
	((Dsu *)hw)->MBENABLE0.reg = tmp;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_clear_MBENABLE0_ENABLE9_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg &= ~DSU_MBENABLE0_ENABLE9;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_toggle_MBENABLE0_ENABLE9_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg ^= DSU_MBENABLE0_ENABLE9;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_set_MBENABLE0_ENABLE10_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg |= DSU_MBENABLE0_ENABLE10;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBENABLE0_ENABLE10_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->MBENABLE0.reg;
	tmp = (tmp & DSU_MBENABLE0_ENABLE10) >> DSU_MBENABLE0_ENABLE10_Pos;
	return (bool)tmp;
}

static inline void hri_dsu_write_MBENABLE0_ENABLE10_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DSU_CRITICAL_SECTION_ENTER();
	tmp = ((Dsu *)hw)->MBENABLE0.reg;
	tmp &= ~DSU_MBENABLE0_ENABLE10;
	tmp |= value << DSU_MBENABLE0_ENABLE10_Pos;
	((Dsu *)hw)->MBENABLE0.reg = tmp;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_clear_MBENABLE0_ENABLE10_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg &= ~DSU_MBENABLE0_ENABLE10;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_toggle_MBENABLE0_ENABLE10_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg ^= DSU_MBENABLE0_ENABLE10;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_set_MBENABLE0_ENABLE11_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg |= DSU_MBENABLE0_ENABLE11;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBENABLE0_ENABLE11_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->MBENABLE0.reg;
	tmp = (tmp & DSU_MBENABLE0_ENABLE11) >> DSU_MBENABLE0_ENABLE11_Pos;
	return (bool)tmp;
}

static inline void hri_dsu_write_MBENABLE0_ENABLE11_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DSU_CRITICAL_SECTION_ENTER();
	tmp = ((Dsu *)hw)->MBENABLE0.reg;
	tmp &= ~DSU_MBENABLE0_ENABLE11;
	tmp |= value << DSU_MBENABLE0_ENABLE11_Pos;
	((Dsu *)hw)->MBENABLE0.reg = tmp;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_clear_MBENABLE0_ENABLE11_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg &= ~DSU_MBENABLE0_ENABLE11;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_toggle_MBENABLE0_ENABLE11_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg ^= DSU_MBENABLE0_ENABLE11;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_set_MBENABLE0_ENABLE12_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg |= DSU_MBENABLE0_ENABLE12;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBENABLE0_ENABLE12_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->MBENABLE0.reg;
	tmp = (tmp & DSU_MBENABLE0_ENABLE12) >> DSU_MBENABLE0_ENABLE12_Pos;
	return (bool)tmp;
}

static inline void hri_dsu_write_MBENABLE0_ENABLE12_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DSU_CRITICAL_SECTION_ENTER();
	tmp = ((Dsu *)hw)->MBENABLE0.reg;
	tmp &= ~DSU_MBENABLE0_ENABLE12;
	tmp |= value << DSU_MBENABLE0_ENABLE12_Pos;
	((Dsu *)hw)->MBENABLE0.reg = tmp;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_clear_MBENABLE0_ENABLE12_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg &= ~DSU_MBENABLE0_ENABLE12;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_toggle_MBENABLE0_ENABLE12_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg ^= DSU_MBENABLE0_ENABLE12;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_set_MBENABLE0_ENABLE13_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg |= DSU_MBENABLE0_ENABLE13;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBENABLE0_ENABLE13_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->MBENABLE0.reg;
	tmp = (tmp & DSU_MBENABLE0_ENABLE13) >> DSU_MBENABLE0_ENABLE13_Pos;
	return (bool)tmp;
}

static inline void hri_dsu_write_MBENABLE0_ENABLE13_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DSU_CRITICAL_SECTION_ENTER();
	tmp = ((Dsu *)hw)->MBENABLE0.reg;
	tmp &= ~DSU_MBENABLE0_ENABLE13;
	tmp |= value << DSU_MBENABLE0_ENABLE13_Pos;
	((Dsu *)hw)->MBENABLE0.reg = tmp;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_clear_MBENABLE0_ENABLE13_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg &= ~DSU_MBENABLE0_ENABLE13;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_toggle_MBENABLE0_ENABLE13_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg ^= DSU_MBENABLE0_ENABLE13;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_set_MBENABLE0_ENABLE14_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg |= DSU_MBENABLE0_ENABLE14;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBENABLE0_ENABLE14_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->MBENABLE0.reg;
	tmp = (tmp & DSU_MBENABLE0_ENABLE14) >> DSU_MBENABLE0_ENABLE14_Pos;
	return (bool)tmp;
}

static inline void hri_dsu_write_MBENABLE0_ENABLE14_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DSU_CRITICAL_SECTION_ENTER();
	tmp = ((Dsu *)hw)->MBENABLE0.reg;
	tmp &= ~DSU_MBENABLE0_ENABLE14;
	tmp |= value << DSU_MBENABLE0_ENABLE14_Pos;
	((Dsu *)hw)->MBENABLE0.reg = tmp;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_clear_MBENABLE0_ENABLE14_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg &= ~DSU_MBENABLE0_ENABLE14;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_toggle_MBENABLE0_ENABLE14_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg ^= DSU_MBENABLE0_ENABLE14;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_set_MBENABLE0_ENABLE15_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg |= DSU_MBENABLE0_ENABLE15;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBENABLE0_ENABLE15_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->MBENABLE0.reg;
	tmp = (tmp & DSU_MBENABLE0_ENABLE15) >> DSU_MBENABLE0_ENABLE15_Pos;
	return (bool)tmp;
}

static inline void hri_dsu_write_MBENABLE0_ENABLE15_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DSU_CRITICAL_SECTION_ENTER();
	tmp = ((Dsu *)hw)->MBENABLE0.reg;
	tmp &= ~DSU_MBENABLE0_ENABLE15;
	tmp |= value << DSU_MBENABLE0_ENABLE15_Pos;
	((Dsu *)hw)->MBENABLE0.reg = tmp;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_clear_MBENABLE0_ENABLE15_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg &= ~DSU_MBENABLE0_ENABLE15;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_toggle_MBENABLE0_ENABLE15_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg ^= DSU_MBENABLE0_ENABLE15;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_set_MBENABLE0_ENABLE16_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg |= DSU_MBENABLE0_ENABLE16;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBENABLE0_ENABLE16_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->MBENABLE0.reg;
	tmp = (tmp & DSU_MBENABLE0_ENABLE16) >> DSU_MBENABLE0_ENABLE16_Pos;
	return (bool)tmp;
}

static inline void hri_dsu_write_MBENABLE0_ENABLE16_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DSU_CRITICAL_SECTION_ENTER();
	tmp = ((Dsu *)hw)->MBENABLE0.reg;
	tmp &= ~DSU_MBENABLE0_ENABLE16;
	tmp |= value << DSU_MBENABLE0_ENABLE16_Pos;
	((Dsu *)hw)->MBENABLE0.reg = tmp;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_clear_MBENABLE0_ENABLE16_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg &= ~DSU_MBENABLE0_ENABLE16;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_toggle_MBENABLE0_ENABLE16_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg ^= DSU_MBENABLE0_ENABLE16;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_set_MBENABLE0_ENABLE17_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg |= DSU_MBENABLE0_ENABLE17;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBENABLE0_ENABLE17_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->MBENABLE0.reg;
	tmp = (tmp & DSU_MBENABLE0_ENABLE17) >> DSU_MBENABLE0_ENABLE17_Pos;
	return (bool)tmp;
}

static inline void hri_dsu_write_MBENABLE0_ENABLE17_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DSU_CRITICAL_SECTION_ENTER();
	tmp = ((Dsu *)hw)->MBENABLE0.reg;
	tmp &= ~DSU_MBENABLE0_ENABLE17;
	tmp |= value << DSU_MBENABLE0_ENABLE17_Pos;
	((Dsu *)hw)->MBENABLE0.reg = tmp;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_clear_MBENABLE0_ENABLE17_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg &= ~DSU_MBENABLE0_ENABLE17;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_toggle_MBENABLE0_ENABLE17_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg ^= DSU_MBENABLE0_ENABLE17;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_set_MBENABLE0_ENABLE18_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg |= DSU_MBENABLE0_ENABLE18;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBENABLE0_ENABLE18_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->MBENABLE0.reg;
	tmp = (tmp & DSU_MBENABLE0_ENABLE18) >> DSU_MBENABLE0_ENABLE18_Pos;
	return (bool)tmp;
}

static inline void hri_dsu_write_MBENABLE0_ENABLE18_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DSU_CRITICAL_SECTION_ENTER();
	tmp = ((Dsu *)hw)->MBENABLE0.reg;
	tmp &= ~DSU_MBENABLE0_ENABLE18;
	tmp |= value << DSU_MBENABLE0_ENABLE18_Pos;
	((Dsu *)hw)->MBENABLE0.reg = tmp;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_clear_MBENABLE0_ENABLE18_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg &= ~DSU_MBENABLE0_ENABLE18;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_toggle_MBENABLE0_ENABLE18_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg ^= DSU_MBENABLE0_ENABLE18;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_set_MBENABLE0_ENABLE19_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg |= DSU_MBENABLE0_ENABLE19;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBENABLE0_ENABLE19_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->MBENABLE0.reg;
	tmp = (tmp & DSU_MBENABLE0_ENABLE19) >> DSU_MBENABLE0_ENABLE19_Pos;
	return (bool)tmp;
}

static inline void hri_dsu_write_MBENABLE0_ENABLE19_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DSU_CRITICAL_SECTION_ENTER();
	tmp = ((Dsu *)hw)->MBENABLE0.reg;
	tmp &= ~DSU_MBENABLE0_ENABLE19;
	tmp |= value << DSU_MBENABLE0_ENABLE19_Pos;
	((Dsu *)hw)->MBENABLE0.reg = tmp;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_clear_MBENABLE0_ENABLE19_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg &= ~DSU_MBENABLE0_ENABLE19;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_toggle_MBENABLE0_ENABLE19_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg ^= DSU_MBENABLE0_ENABLE19;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_set_MBENABLE0_ENABLE20_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg |= DSU_MBENABLE0_ENABLE20;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBENABLE0_ENABLE20_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->MBENABLE0.reg;
	tmp = (tmp & DSU_MBENABLE0_ENABLE20) >> DSU_MBENABLE0_ENABLE20_Pos;
	return (bool)tmp;
}

static inline void hri_dsu_write_MBENABLE0_ENABLE20_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DSU_CRITICAL_SECTION_ENTER();
	tmp = ((Dsu *)hw)->MBENABLE0.reg;
	tmp &= ~DSU_MBENABLE0_ENABLE20;
	tmp |= value << DSU_MBENABLE0_ENABLE20_Pos;
	((Dsu *)hw)->MBENABLE0.reg = tmp;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_clear_MBENABLE0_ENABLE20_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg &= ~DSU_MBENABLE0_ENABLE20;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_toggle_MBENABLE0_ENABLE20_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg ^= DSU_MBENABLE0_ENABLE20;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_set_MBENABLE0_ENABLE21_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg |= DSU_MBENABLE0_ENABLE21;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBENABLE0_ENABLE21_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->MBENABLE0.reg;
	tmp = (tmp & DSU_MBENABLE0_ENABLE21) >> DSU_MBENABLE0_ENABLE21_Pos;
	return (bool)tmp;
}

static inline void hri_dsu_write_MBENABLE0_ENABLE21_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DSU_CRITICAL_SECTION_ENTER();
	tmp = ((Dsu *)hw)->MBENABLE0.reg;
	tmp &= ~DSU_MBENABLE0_ENABLE21;
	tmp |= value << DSU_MBENABLE0_ENABLE21_Pos;
	((Dsu *)hw)->MBENABLE0.reg = tmp;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_clear_MBENABLE0_ENABLE21_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg &= ~DSU_MBENABLE0_ENABLE21;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_toggle_MBENABLE0_ENABLE21_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg ^= DSU_MBENABLE0_ENABLE21;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_set_MBENABLE0_ENABLE22_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg |= DSU_MBENABLE0_ENABLE22;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBENABLE0_ENABLE22_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->MBENABLE0.reg;
	tmp = (tmp & DSU_MBENABLE0_ENABLE22) >> DSU_MBENABLE0_ENABLE22_Pos;
	return (bool)tmp;
}

static inline void hri_dsu_write_MBENABLE0_ENABLE22_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DSU_CRITICAL_SECTION_ENTER();
	tmp = ((Dsu *)hw)->MBENABLE0.reg;
	tmp &= ~DSU_MBENABLE0_ENABLE22;
	tmp |= value << DSU_MBENABLE0_ENABLE22_Pos;
	((Dsu *)hw)->MBENABLE0.reg = tmp;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_clear_MBENABLE0_ENABLE22_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg &= ~DSU_MBENABLE0_ENABLE22;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_toggle_MBENABLE0_ENABLE22_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg ^= DSU_MBENABLE0_ENABLE22;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_set_MBENABLE0_ENABLE23_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg |= DSU_MBENABLE0_ENABLE23;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBENABLE0_ENABLE23_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->MBENABLE0.reg;
	tmp = (tmp & DSU_MBENABLE0_ENABLE23) >> DSU_MBENABLE0_ENABLE23_Pos;
	return (bool)tmp;
}

static inline void hri_dsu_write_MBENABLE0_ENABLE23_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DSU_CRITICAL_SECTION_ENTER();
	tmp = ((Dsu *)hw)->MBENABLE0.reg;
	tmp &= ~DSU_MBENABLE0_ENABLE23;
	tmp |= value << DSU_MBENABLE0_ENABLE23_Pos;
	((Dsu *)hw)->MBENABLE0.reg = tmp;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_clear_MBENABLE0_ENABLE23_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg &= ~DSU_MBENABLE0_ENABLE23;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_toggle_MBENABLE0_ENABLE23_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg ^= DSU_MBENABLE0_ENABLE23;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_set_MBENABLE0_ENABLE24_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg |= DSU_MBENABLE0_ENABLE24;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBENABLE0_ENABLE24_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->MBENABLE0.reg;
	tmp = (tmp & DSU_MBENABLE0_ENABLE24) >> DSU_MBENABLE0_ENABLE24_Pos;
	return (bool)tmp;
}

static inline void hri_dsu_write_MBENABLE0_ENABLE24_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DSU_CRITICAL_SECTION_ENTER();
	tmp = ((Dsu *)hw)->MBENABLE0.reg;
	tmp &= ~DSU_MBENABLE0_ENABLE24;
	tmp |= value << DSU_MBENABLE0_ENABLE24_Pos;
	((Dsu *)hw)->MBENABLE0.reg = tmp;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_clear_MBENABLE0_ENABLE24_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg &= ~DSU_MBENABLE0_ENABLE24;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_toggle_MBENABLE0_ENABLE24_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg ^= DSU_MBENABLE0_ENABLE24;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_set_MBENABLE0_ENABLE25_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg |= DSU_MBENABLE0_ENABLE25;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBENABLE0_ENABLE25_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->MBENABLE0.reg;
	tmp = (tmp & DSU_MBENABLE0_ENABLE25) >> DSU_MBENABLE0_ENABLE25_Pos;
	return (bool)tmp;
}

static inline void hri_dsu_write_MBENABLE0_ENABLE25_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DSU_CRITICAL_SECTION_ENTER();
	tmp = ((Dsu *)hw)->MBENABLE0.reg;
	tmp &= ~DSU_MBENABLE0_ENABLE25;
	tmp |= value << DSU_MBENABLE0_ENABLE25_Pos;
	((Dsu *)hw)->MBENABLE0.reg = tmp;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_clear_MBENABLE0_ENABLE25_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg &= ~DSU_MBENABLE0_ENABLE25;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_toggle_MBENABLE0_ENABLE25_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg ^= DSU_MBENABLE0_ENABLE25;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_set_MBENABLE0_ENABLE26_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg |= DSU_MBENABLE0_ENABLE26;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBENABLE0_ENABLE26_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->MBENABLE0.reg;
	tmp = (tmp & DSU_MBENABLE0_ENABLE26) >> DSU_MBENABLE0_ENABLE26_Pos;
	return (bool)tmp;
}

static inline void hri_dsu_write_MBENABLE0_ENABLE26_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DSU_CRITICAL_SECTION_ENTER();
	tmp = ((Dsu *)hw)->MBENABLE0.reg;
	tmp &= ~DSU_MBENABLE0_ENABLE26;
	tmp |= value << DSU_MBENABLE0_ENABLE26_Pos;
	((Dsu *)hw)->MBENABLE0.reg = tmp;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_clear_MBENABLE0_ENABLE26_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg &= ~DSU_MBENABLE0_ENABLE26;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_toggle_MBENABLE0_ENABLE26_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg ^= DSU_MBENABLE0_ENABLE26;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_set_MBENABLE0_ENABLE27_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg |= DSU_MBENABLE0_ENABLE27;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBENABLE0_ENABLE27_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->MBENABLE0.reg;
	tmp = (tmp & DSU_MBENABLE0_ENABLE27) >> DSU_MBENABLE0_ENABLE27_Pos;
	return (bool)tmp;
}

static inline void hri_dsu_write_MBENABLE0_ENABLE27_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DSU_CRITICAL_SECTION_ENTER();
	tmp = ((Dsu *)hw)->MBENABLE0.reg;
	tmp &= ~DSU_MBENABLE0_ENABLE27;
	tmp |= value << DSU_MBENABLE0_ENABLE27_Pos;
	((Dsu *)hw)->MBENABLE0.reg = tmp;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_clear_MBENABLE0_ENABLE27_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg &= ~DSU_MBENABLE0_ENABLE27;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_toggle_MBENABLE0_ENABLE27_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg ^= DSU_MBENABLE0_ENABLE27;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_set_MBENABLE0_ENABLE28_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg |= DSU_MBENABLE0_ENABLE28;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBENABLE0_ENABLE28_bit(const void *const hw)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->MBENABLE0.reg;
	tmp = (tmp & DSU_MBENABLE0_ENABLE28) >> DSU_MBENABLE0_ENABLE28_Pos;
	return (bool)tmp;
}

static inline void hri_dsu_write_MBENABLE0_ENABLE28_bit(const void *const hw, bool value)
{
	uint32_t tmp;
	DSU_CRITICAL_SECTION_ENTER();
	tmp = ((Dsu *)hw)->MBENABLE0.reg;
	tmp &= ~DSU_MBENABLE0_ENABLE28;
	tmp |= value << DSU_MBENABLE0_ENABLE28_Pos;
	((Dsu *)hw)->MBENABLE0.reg = tmp;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_clear_MBENABLE0_ENABLE28_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg &= ~DSU_MBENABLE0_ENABLE28;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_toggle_MBENABLE0_ENABLE28_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg ^= DSU_MBENABLE0_ENABLE28;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_set_MBENABLE0_reg(const void *const hw, hri_dsu_mbenable0_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg |= mask;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline hri_dsu_mbenable0_reg_t hri_dsu_get_MBENABLE0_reg(const void *const hw, hri_dsu_mbenable0_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->MBENABLE0.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_dsu_write_MBENABLE0_reg(const void *const hw, hri_dsu_mbenable0_reg_t data)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg = data;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_clear_MBENABLE0_reg(const void *const hw, hri_dsu_mbenable0_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg &= ~mask;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_toggle_MBENABLE0_reg(const void *const hw, hri_dsu_mbenable0_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBENABLE0.reg ^= mask;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline hri_dsu_mbenable0_reg_t hri_dsu_read_MBENABLE0_reg(const void *const hw)
{
	return ((Dsu *)hw)->MBENABLE0.reg;
}

static inline void hri_dsu_set_DCFG_DCFG_bf(const void *const hw, uint8_t index, hri_dsu_dcfg_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->DCFG[index].reg |= DSU_DCFG_DCFG(mask);
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline hri_dsu_dcfg_reg_t hri_dsu_get_DCFG_DCFG_bf(const void *const hw, uint8_t index, hri_dsu_dcfg_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->DCFG[index].reg;
	tmp = (tmp & DSU_DCFG_DCFG(mask)) >> DSU_DCFG_DCFG_Pos;
	return tmp;
}

static inline void hri_dsu_write_DCFG_DCFG_bf(const void *const hw, uint8_t index, hri_dsu_dcfg_reg_t data)
{
	uint32_t tmp;
	DSU_CRITICAL_SECTION_ENTER();
	tmp = ((Dsu *)hw)->DCFG[index].reg;
	tmp &= ~DSU_DCFG_DCFG_Msk;
	tmp |= DSU_DCFG_DCFG(data);
	((Dsu *)hw)->DCFG[index].reg = tmp;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_clear_DCFG_DCFG_bf(const void *const hw, uint8_t index, hri_dsu_dcfg_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->DCFG[index].reg &= ~DSU_DCFG_DCFG(mask);
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_toggle_DCFG_DCFG_bf(const void *const hw, uint8_t index, hri_dsu_dcfg_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->DCFG[index].reg ^= DSU_DCFG_DCFG(mask);
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline hri_dsu_dcfg_reg_t hri_dsu_read_DCFG_DCFG_bf(const void *const hw, uint8_t index)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->DCFG[index].reg;
	tmp = (tmp & DSU_DCFG_DCFG_Msk) >> DSU_DCFG_DCFG_Pos;
	return tmp;
}

static inline void hri_dsu_set_DCFG_reg(const void *const hw, uint8_t index, hri_dsu_dcfg_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->DCFG[index].reg |= mask;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline hri_dsu_dcfg_reg_t hri_dsu_get_DCFG_reg(const void *const hw, uint8_t index, hri_dsu_dcfg_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->DCFG[index].reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_dsu_write_DCFG_reg(const void *const hw, uint8_t index, hri_dsu_dcfg_reg_t data)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->DCFG[index].reg = data;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_clear_DCFG_reg(const void *const hw, uint8_t index, hri_dsu_dcfg_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->DCFG[index].reg &= ~mask;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline void hri_dsu_toggle_DCFG_reg(const void *const hw, uint8_t index, hri_dsu_dcfg_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->DCFG[index].reg ^= mask;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline hri_dsu_dcfg_reg_t hri_dsu_read_DCFG_reg(const void *const hw, uint8_t index)
{
	return ((Dsu *)hw)->DCFG[index].reg;
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

static inline bool hri_dsu_get_MBSTATUS0_STATUS0_bit(const void *const hw)
{
	return (((Dsu *)hw)->MBSTATUS0.reg & DSU_MBSTATUS0_STATUS0) >> DSU_MBSTATUS0_STATUS0_Pos;
}

static inline void hri_dsu_clear_MBSTATUS0_STATUS0_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBSTATUS0.reg = DSU_MBSTATUS0_STATUS0;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBSTATUS0_STATUS1_bit(const void *const hw)
{
	return (((Dsu *)hw)->MBSTATUS0.reg & DSU_MBSTATUS0_STATUS1) >> DSU_MBSTATUS0_STATUS1_Pos;
}

static inline void hri_dsu_clear_MBSTATUS0_STATUS1_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBSTATUS0.reg = DSU_MBSTATUS0_STATUS1;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBSTATUS0_STATUS2_bit(const void *const hw)
{
	return (((Dsu *)hw)->MBSTATUS0.reg & DSU_MBSTATUS0_STATUS2) >> DSU_MBSTATUS0_STATUS2_Pos;
}

static inline void hri_dsu_clear_MBSTATUS0_STATUS2_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBSTATUS0.reg = DSU_MBSTATUS0_STATUS2;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBSTATUS0_STATUS3_bit(const void *const hw)
{
	return (((Dsu *)hw)->MBSTATUS0.reg & DSU_MBSTATUS0_STATUS3) >> DSU_MBSTATUS0_STATUS3_Pos;
}

static inline void hri_dsu_clear_MBSTATUS0_STATUS3_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBSTATUS0.reg = DSU_MBSTATUS0_STATUS3;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBSTATUS0_STATUS4_bit(const void *const hw)
{
	return (((Dsu *)hw)->MBSTATUS0.reg & DSU_MBSTATUS0_STATUS4) >> DSU_MBSTATUS0_STATUS4_Pos;
}

static inline void hri_dsu_clear_MBSTATUS0_STATUS4_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBSTATUS0.reg = DSU_MBSTATUS0_STATUS4;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBSTATUS0_STATUS5_bit(const void *const hw)
{
	return (((Dsu *)hw)->MBSTATUS0.reg & DSU_MBSTATUS0_STATUS5) >> DSU_MBSTATUS0_STATUS5_Pos;
}

static inline void hri_dsu_clear_MBSTATUS0_STATUS5_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBSTATUS0.reg = DSU_MBSTATUS0_STATUS5;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBSTATUS0_STATUS6_bit(const void *const hw)
{
	return (((Dsu *)hw)->MBSTATUS0.reg & DSU_MBSTATUS0_STATUS6) >> DSU_MBSTATUS0_STATUS6_Pos;
}

static inline void hri_dsu_clear_MBSTATUS0_STATUS6_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBSTATUS0.reg = DSU_MBSTATUS0_STATUS6;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBSTATUS0_STATUS7_bit(const void *const hw)
{
	return (((Dsu *)hw)->MBSTATUS0.reg & DSU_MBSTATUS0_STATUS7) >> DSU_MBSTATUS0_STATUS7_Pos;
}

static inline void hri_dsu_clear_MBSTATUS0_STATUS7_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBSTATUS0.reg = DSU_MBSTATUS0_STATUS7;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBSTATUS0_STATUS8_bit(const void *const hw)
{
	return (((Dsu *)hw)->MBSTATUS0.reg & DSU_MBSTATUS0_STATUS8) >> DSU_MBSTATUS0_STATUS8_Pos;
}

static inline void hri_dsu_clear_MBSTATUS0_STATUS8_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBSTATUS0.reg = DSU_MBSTATUS0_STATUS8;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBSTATUS0_STATUS9_bit(const void *const hw)
{
	return (((Dsu *)hw)->MBSTATUS0.reg & DSU_MBSTATUS0_STATUS9) >> DSU_MBSTATUS0_STATUS9_Pos;
}

static inline void hri_dsu_clear_MBSTATUS0_STATUS9_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBSTATUS0.reg = DSU_MBSTATUS0_STATUS9;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBSTATUS0_STATUS10_bit(const void *const hw)
{
	return (((Dsu *)hw)->MBSTATUS0.reg & DSU_MBSTATUS0_STATUS10) >> DSU_MBSTATUS0_STATUS10_Pos;
}

static inline void hri_dsu_clear_MBSTATUS0_STATUS10_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBSTATUS0.reg = DSU_MBSTATUS0_STATUS10;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBSTATUS0_STATUS11_bit(const void *const hw)
{
	return (((Dsu *)hw)->MBSTATUS0.reg & DSU_MBSTATUS0_STATUS11) >> DSU_MBSTATUS0_STATUS11_Pos;
}

static inline void hri_dsu_clear_MBSTATUS0_STATUS11_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBSTATUS0.reg = DSU_MBSTATUS0_STATUS11;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBSTATUS0_STATUS12_bit(const void *const hw)
{
	return (((Dsu *)hw)->MBSTATUS0.reg & DSU_MBSTATUS0_STATUS12) >> DSU_MBSTATUS0_STATUS12_Pos;
}

static inline void hri_dsu_clear_MBSTATUS0_STATUS12_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBSTATUS0.reg = DSU_MBSTATUS0_STATUS12;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBSTATUS0_STATUS13_bit(const void *const hw)
{
	return (((Dsu *)hw)->MBSTATUS0.reg & DSU_MBSTATUS0_STATUS13) >> DSU_MBSTATUS0_STATUS13_Pos;
}

static inline void hri_dsu_clear_MBSTATUS0_STATUS13_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBSTATUS0.reg = DSU_MBSTATUS0_STATUS13;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBSTATUS0_STATUS14_bit(const void *const hw)
{
	return (((Dsu *)hw)->MBSTATUS0.reg & DSU_MBSTATUS0_STATUS14) >> DSU_MBSTATUS0_STATUS14_Pos;
}

static inline void hri_dsu_clear_MBSTATUS0_STATUS14_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBSTATUS0.reg = DSU_MBSTATUS0_STATUS14;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBSTATUS0_STATUS15_bit(const void *const hw)
{
	return (((Dsu *)hw)->MBSTATUS0.reg & DSU_MBSTATUS0_STATUS15) >> DSU_MBSTATUS0_STATUS15_Pos;
}

static inline void hri_dsu_clear_MBSTATUS0_STATUS15_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBSTATUS0.reg = DSU_MBSTATUS0_STATUS15;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBSTATUS0_STATUS16_bit(const void *const hw)
{
	return (((Dsu *)hw)->MBSTATUS0.reg & DSU_MBSTATUS0_STATUS16) >> DSU_MBSTATUS0_STATUS16_Pos;
}

static inline void hri_dsu_clear_MBSTATUS0_STATUS16_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBSTATUS0.reg = DSU_MBSTATUS0_STATUS16;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBSTATUS0_STATUS17_bit(const void *const hw)
{
	return (((Dsu *)hw)->MBSTATUS0.reg & DSU_MBSTATUS0_STATUS17) >> DSU_MBSTATUS0_STATUS17_Pos;
}

static inline void hri_dsu_clear_MBSTATUS0_STATUS17_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBSTATUS0.reg = DSU_MBSTATUS0_STATUS17;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBSTATUS0_STATUS18_bit(const void *const hw)
{
	return (((Dsu *)hw)->MBSTATUS0.reg & DSU_MBSTATUS0_STATUS18) >> DSU_MBSTATUS0_STATUS18_Pos;
}

static inline void hri_dsu_clear_MBSTATUS0_STATUS18_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBSTATUS0.reg = DSU_MBSTATUS0_STATUS18;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBSTATUS0_STATUS19_bit(const void *const hw)
{
	return (((Dsu *)hw)->MBSTATUS0.reg & DSU_MBSTATUS0_STATUS19) >> DSU_MBSTATUS0_STATUS19_Pos;
}

static inline void hri_dsu_clear_MBSTATUS0_STATUS19_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBSTATUS0.reg = DSU_MBSTATUS0_STATUS19;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBSTATUS0_STATUS20_bit(const void *const hw)
{
	return (((Dsu *)hw)->MBSTATUS0.reg & DSU_MBSTATUS0_STATUS20) >> DSU_MBSTATUS0_STATUS20_Pos;
}

static inline void hri_dsu_clear_MBSTATUS0_STATUS20_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBSTATUS0.reg = DSU_MBSTATUS0_STATUS20;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBSTATUS0_STATUS21_bit(const void *const hw)
{
	return (((Dsu *)hw)->MBSTATUS0.reg & DSU_MBSTATUS0_STATUS21) >> DSU_MBSTATUS0_STATUS21_Pos;
}

static inline void hri_dsu_clear_MBSTATUS0_STATUS21_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBSTATUS0.reg = DSU_MBSTATUS0_STATUS21;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBSTATUS0_STATUS22_bit(const void *const hw)
{
	return (((Dsu *)hw)->MBSTATUS0.reg & DSU_MBSTATUS0_STATUS22) >> DSU_MBSTATUS0_STATUS22_Pos;
}

static inline void hri_dsu_clear_MBSTATUS0_STATUS22_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBSTATUS0.reg = DSU_MBSTATUS0_STATUS22;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBSTATUS0_STATUS23_bit(const void *const hw)
{
	return (((Dsu *)hw)->MBSTATUS0.reg & DSU_MBSTATUS0_STATUS23) >> DSU_MBSTATUS0_STATUS23_Pos;
}

static inline void hri_dsu_clear_MBSTATUS0_STATUS23_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBSTATUS0.reg = DSU_MBSTATUS0_STATUS23;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBSTATUS0_STATUS24_bit(const void *const hw)
{
	return (((Dsu *)hw)->MBSTATUS0.reg & DSU_MBSTATUS0_STATUS24) >> DSU_MBSTATUS0_STATUS24_Pos;
}

static inline void hri_dsu_clear_MBSTATUS0_STATUS24_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBSTATUS0.reg = DSU_MBSTATUS0_STATUS24;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBSTATUS0_STATUS25_bit(const void *const hw)
{
	return (((Dsu *)hw)->MBSTATUS0.reg & DSU_MBSTATUS0_STATUS25) >> DSU_MBSTATUS0_STATUS25_Pos;
}

static inline void hri_dsu_clear_MBSTATUS0_STATUS25_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBSTATUS0.reg = DSU_MBSTATUS0_STATUS25;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBSTATUS0_STATUS26_bit(const void *const hw)
{
	return (((Dsu *)hw)->MBSTATUS0.reg & DSU_MBSTATUS0_STATUS26) >> DSU_MBSTATUS0_STATUS26_Pos;
}

static inline void hri_dsu_clear_MBSTATUS0_STATUS26_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBSTATUS0.reg = DSU_MBSTATUS0_STATUS26;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBSTATUS0_STATUS27_bit(const void *const hw)
{
	return (((Dsu *)hw)->MBSTATUS0.reg & DSU_MBSTATUS0_STATUS27) >> DSU_MBSTATUS0_STATUS27_Pos;
}

static inline void hri_dsu_clear_MBSTATUS0_STATUS27_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBSTATUS0.reg = DSU_MBSTATUS0_STATUS27;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline bool hri_dsu_get_MBSTATUS0_STATUS28_bit(const void *const hw)
{
	return (((Dsu *)hw)->MBSTATUS0.reg & DSU_MBSTATUS0_STATUS28) >> DSU_MBSTATUS0_STATUS28_Pos;
}

static inline void hri_dsu_clear_MBSTATUS0_STATUS28_bit(const void *const hw)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBSTATUS0.reg = DSU_MBSTATUS0_STATUS28;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline hri_dsu_mbstatus0_reg_t hri_dsu_get_MBSTATUS0_reg(const void *const hw, hri_dsu_mbstatus0_reg_t mask)
{
	uint32_t tmp;
	tmp = ((Dsu *)hw)->MBSTATUS0.reg;
	tmp &= mask;
	return tmp;
}

static inline void hri_dsu_clear_MBSTATUS0_reg(const void *const hw, hri_dsu_mbstatus0_reg_t mask)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->MBSTATUS0.reg = mask;
	DSU_CRITICAL_SECTION_LEAVE();
}

static inline hri_dsu_mbstatus0_reg_t hri_dsu_read_MBSTATUS0_reg(const void *const hw)
{
	return ((Dsu *)hw)->MBSTATUS0.reg;
}

static inline void hri_dsu_write_CTRL_reg(const void *const hw, hri_dsu_ctrl_reg_t data)
{
	DSU_CRITICAL_SECTION_ENTER();
	((Dsu *)hw)->CTRL.reg = data;
	DSU_CRITICAL_SECTION_LEAVE();
}

#ifdef __cplusplus
}
#endif

#endif /* _HRI_DSU_D51_H_INCLUDED */
#endif /* _SAMD51_DSU_COMPONENT_ */
