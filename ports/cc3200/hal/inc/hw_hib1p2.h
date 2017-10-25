//*****************************************************************************
//
//  Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/
//
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//    Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
//    Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the
//    distribution.
//
//    Neither the name of Texas Instruments Incorporated nor the names of
//    its contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************

#ifndef __HW_HIB1P2_H__
#define __HW_HIB1P2_H__

//*****************************************************************************
//
// The following are defines for the HIB1P2 register offsets.
//
//*****************************************************************************
#define HIB1P2_O_SRAM_SKA_LDO_PARAMETERS0 \
                                0x00000000

#define HIB1P2_O_SRAM_SKA_LDO_PARAMETERS1 \
                                0x00000004

#define HIB1P2_O_DIG_DCDC_PARAMETERS0 \
                                0x00000008

#define HIB1P2_O_DIG_DCDC_PARAMETERS1 \
                                0x0000000C

#define HIB1P2_O_DIG_DCDC_PARAMETERS2 \
                                0x00000010

#define HIB1P2_O_DIG_DCDC_PARAMETERS3 \
                                0x00000014

#define HIB1P2_O_DIG_DCDC_PARAMETERS4 \
                                0x00000018

#define HIB1P2_O_DIG_DCDC_PARAMETERS5 \
                                0x0000001C

#define HIB1P2_O_DIG_DCDC_PARAMETERS6 \
                                0x00000020

#define HIB1P2_O_ANA_DCDC_PARAMETERS0 \
                                0x00000024

#define HIB1P2_O_ANA_DCDC_PARAMETERS1 \
                                0x00000028

#define HIB1P2_O_ANA_DCDC_PARAMETERS16 \
                                0x00000064

#define HIB1P2_O_ANA_DCDC_PARAMETERS17 \
                                0x00000068

#define HIB1P2_O_ANA_DCDC_PARAMETERS18 \
                                0x0000006C

#define HIB1P2_O_ANA_DCDC_PARAMETERS19 \
                                0x00000070

#define HIB1P2_O_FLASH_DCDC_PARAMETERS0 \
                                0x00000074

#define HIB1P2_O_FLASH_DCDC_PARAMETERS1 \
                                0x00000078

#define HIB1P2_O_FLASH_DCDC_PARAMETERS2 \
                                0x0000007C

#define HIB1P2_O_FLASH_DCDC_PARAMETERS3 \
                                0x00000080

#define HIB1P2_O_FLASH_DCDC_PARAMETERS4 \
                                0x00000084

#define HIB1P2_O_FLASH_DCDC_PARAMETERS5 \
                                0x00000088

#define HIB1P2_O_FLASH_DCDC_PARAMETERS6 \
                                0x0000008C

#define HIB1P2_O_PMBIST_PARAMETERS0 \
                                0x00000094

#define HIB1P2_O_PMBIST_PARAMETERS1 \
                                0x00000098

#define HIB1P2_O_PMBIST_PARAMETERS2 \
                                0x0000009C

#define HIB1P2_O_PMBIST_PARAMETERS3 \
                                0x000000A0

#define HIB1P2_O_FLASH_DCDC_PARAMETERS8 \
                                0x000000A4

#define HIB1P2_O_ANA_DCDC_PARAMETERS_OVERRIDE \
                                0x000000A8

#define HIB1P2_O_FLASH_DCDC_PARAMETERS_OVERRIDE \
                                0x000000AC

#define HIB1P2_O_DIG_DCDC_VTRIM_CFG \
                                0x000000B0

#define HIB1P2_O_DIG_DCDC_FSM_PARAMETERS \
                                0x000000B4

#define HIB1P2_O_ANA_DCDC_FSM_PARAMETERS \
                                0x000000B8

#define HIB1P2_O_SRAM_SKA_LDO_FSM_PARAMETERS \
                                0x000000BC

#define HIB1P2_O_BGAP_DUTY_CYCLING_EXIT_CFG \
                                0x000000C0

#define HIB1P2_O_CM_OSC_16M_CONFIG \
                                0x000000C4

#define HIB1P2_O_SOP_SENSE_VALUE \
                                0x000000C8

#define HIB1P2_O_HIB_RTC_TIMER_LSW_1P2 \
                                0x000000CC

#define HIB1P2_O_HIB_RTC_TIMER_MSW_1P2 \
                                0x000000D0

#define HIB1P2_O_HIB1P2_BGAP_TRIM_OVERRIDES \
                                0x000000D4

#define HIB1P2_O_HIB1P2_EFUSE_READ_REG0 \
                                0x000000D8

#define HIB1P2_O_HIB1P2_EFUSE_READ_REG1 \
                                0x000000DC

#define HIB1P2_O_HIB1P2_POR_TEST_CTRL \
                                0x000000E0

#define HIB1P2_O_HIB_TIMER_SYNC_CALIB_CFG0 \
                                0x000000E4

#define HIB1P2_O_HIB_TIMER_SYNC_CALIB_CFG1 \
                                0x000000E8

#define HIB1P2_O_HIB_TIMER_SYNC_CFG2 \
                                0x000000EC

#define HIB1P2_O_HIB_TIMER_SYNC_TSF_ADJ_VAL \
                                0x000000F0

#define HIB1P2_O_HIB_TIMER_RTC_GTS_TIMESTAMP_LSW \
                                0x000000F4

#define HIB1P2_O_HIB_TIMER_RTC_GTS_TIMESTAMP_MSW \
                                0x000000F8

#define HIB1P2_O_HIB_TIMER_RTC_WUP_TIMESTAMP_LSW \
                                0x000000FC

#define HIB1P2_O_HIB_TIMER_RTC_WUP_TIMESTAMP_MSW \
                                0x00000100

#define HIB1P2_O_HIB_TIMER_SYNC_WAKE_OFFSET_ERR \
                                0x00000104

#define HIB1P2_O_HIB_TIMER_SYNC_TSF_CURR_VAL_LSW \
                                0x00000108

#define HIB1P2_O_HIB_TIMER_SYNC_TSF_CURR_VAL_MSW \
                                0x0000010C

#define HIB1P2_O_CM_SPARE       0x00000110
#define HIB1P2_O_PORPOL_SPARE   0x00000114
#define HIB1P2_O_MEM_DIG_DCDC_CLK_CONFIG \
                                0x00000118

#define HIB1P2_O_MEM_ANA_DCDC_CLK_CONFIG \
                                0x0000011C

#define HIB1P2_O_MEM_FLASH_DCDC_CLK_CONFIG \
                                0x00000120

#define HIB1P2_O_MEM_PA_DCDC_CLK_CONFIG \
                                0x00000124

#define HIB1P2_O_MEM_SLDO_VNWA_OVERRIDE \
                                0x00000128

#define HIB1P2_O_MEM_BGAP_DUTY_CYCLING_ENABLE_OVERRIDE \
                                0x0000012C

#define HIB1P2_O_MEM_HIB_FSM_DEBUG \
                                0x00000130

#define HIB1P2_O_MEM_SLDO_VNWA_SW_CTRL \
                                0x00000134

#define HIB1P2_O_MEM_SLDO_WEAK_PROCESS \
                                0x00000138

#define HIB1P2_O_MEM_PA_DCDC_OV_UV_STATUS \
                                0x0000013C

#define HIB1P2_O_MEM_CM_TEST_MODE \
                                0x00000140




//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_SRAM_SKA_LDO_PARAMETERS0 register.
//
//******************************************************************************
#define HIB1P2_SRAM_SKA_LDO_PARAMETERS0_mem_sldo_en_sc_itrim_lowv_M \
                                0xC0000000

#define HIB1P2_SRAM_SKA_LDO_PARAMETERS0_mem_sldo_en_sc_itrim_lowv_S 30
#define HIB1P2_SRAM_SKA_LDO_PARAMETERS0_mem_sldo_en_iq_trim_lowv_M \
                                0x30000000

#define HIB1P2_SRAM_SKA_LDO_PARAMETERS0_mem_sldo_en_iq_trim_lowv_S 28
#define HIB1P2_SRAM_SKA_LDO_PARAMETERS0_mem_sldo_en_sc_prot_lowv \
                                0x08000000

#define HIB1P2_SRAM_SKA_LDO_PARAMETERS0_mem_sldo_en_lowv_override \
                                0x04000000  // FSM Override value for SLDO_EN :
                                            // Applicable only when bit [4] of
                                            // this register is set to 1.

#define HIB1P2_SRAM_SKA_LDO_PARAMETERS0_mem_sldo_en_low_pwr_lowv \
                                0x02000000

#define HIB1P2_SRAM_SKA_LDO_PARAMETERS0_mem_sldo_int_cap_sel_lowv \
                                0x01000000

#define HIB1P2_SRAM_SKA_LDO_PARAMETERS0_mem_sldo_vtrim_lowv_M \
                                0x00FC0000

#define HIB1P2_SRAM_SKA_LDO_PARAMETERS0_mem_sldo_vtrim_lowv_S 18
#define HIB1P2_SRAM_SKA_LDO_PARAMETERS0_mem_sldo_spare_lowv_M \
                                0x0003FF00

#define HIB1P2_SRAM_SKA_LDO_PARAMETERS0_mem_sldo_spare_lowv_S 8
#define HIB1P2_SRAM_SKA_LDO_PARAMETERS0_mem_skaldo_en_lowv_override \
                                0x00000080  // FSM Override value for
                                            // SKA_LDO_EN : Applicable only when
                                            // bit [3] of this register is set
                                            // to 1.

#define HIB1P2_SRAM_SKA_LDO_PARAMETERS0_mem_skaldo_en_cap_ref_lowv \
                                0x00000040

#define HIB1P2_SRAM_SKA_LDO_PARAMETERS0_mem_skaldo_en_resdiv_ref_lowv \
                                0x00000020

#define HIB1P2_SRAM_SKA_LDO_PARAMETERS0_mem_sldo_en_lowv_fsm_override_ctrl \
                                0x00000010  // When 1, bit[26] of this register
                                            // will be used as SLDO_EN

#define HIB1P2_SRAM_SKA_LDO_PARAMETERS0_mem_skaldo_en_lowv_fsm_override_ctrl \
                                0x00000008  // When 1, bit[26] of this register
                                            // will be used as SKA_LDO_EN

#define HIB1P2_SRAM_SKA_LDO_PARAMETERS0_NA1_M \
                                0x00000007

#define HIB1P2_SRAM_SKA_LDO_PARAMETERS0_NA1_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_SRAM_SKA_LDO_PARAMETERS1 register.
//
//******************************************************************************
#define HIB1P2_SRAM_SKA_LDO_PARAMETERS1_mem_skaldo_ctrl_lowv_M \
                                0xFFC00000

#define HIB1P2_SRAM_SKA_LDO_PARAMETERS1_mem_skaldo_ctrl_lowv_S 22
#define HIB1P2_SRAM_SKA_LDO_PARAMETERS1_mem_skaldo_vtrim_lowv_M \
                                0x003F0000

#define HIB1P2_SRAM_SKA_LDO_PARAMETERS1_mem_skaldo_vtrim_lowv_S 16
#define HIB1P2_SRAM_SKA_LDO_PARAMETERS1_mem_sldo_en_tload_lowv \
                                0x00008000

#define HIB1P2_SRAM_SKA_LDO_PARAMETERS1_mem_skaldo_en_tload_lowv \
                                0x00004000

#define HIB1P2_SRAM_SKA_LDO_PARAMETERS1_mem_skaldo_cap_sw_en_lowv \
                                0x00002000

#define HIB1P2_SRAM_SKA_LDO_PARAMETERS1_mem_skaldo_en_hib_lowv \
                                0x00001000

#define HIB1P2_SRAM_SKA_LDO_PARAMETERS1_mem_skaldo_en_vref_buf_lowv \
                                0x00000800

#define HIB1P2_SRAM_SKA_LDO_PARAMETERS1_NA2_M \
                                0x000007FF

#define HIB1P2_SRAM_SKA_LDO_PARAMETERS1_NA2_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_DIG_DCDC_PARAMETERS0 register.
//
//******************************************************************************
#define HIB1P2_DIG_DCDC_PARAMETERS0_mem_dcdc_dig_en_lowv_override \
                                0x80000000  // Override value for DCDC_DIG_EN :
                                            // Applicable only when bit [31] of
                                            // DIG_DCDC_PARAMETERS1 [0x000C] is
                                            // set to 1. Else from FSM

#define HIB1P2_DIG_DCDC_PARAMETERS0_mem_dcdc_dig_delayed_en_lowv \
                                0x40000000

#define HIB1P2_DIG_DCDC_PARAMETERS0_mem_dcdc_dig_en_subreg_1p8v_lowv_override \
                                0x20000000  // Override value for
                                            // DCDC_DIG_EN_SUBREG_1P8V :
                                            // Applicable only when bit [30] of
                                            // DIG_DCDC_PARAMETERS1 [0x000C] is
                                            // set to 1. Else from FSM

#define HIB1P2_DIG_DCDC_PARAMETERS0_mem_dcdc_dig_en_subreg_1p2v_lowv_override \
                                0x10000000  // Override value for
                                            // DCDC_DIG_EN_SUBREG_1P2V :
                                            // Applicable only when bit [29] of
                                            // DIG_DCDC_PARAMETERS1 [0x000C] is
                                            // set to 1. Else from FSM

#define HIB1P2_DIG_DCDC_PARAMETERS0_mem_dcdc_dig_en_slp_mode_lowv_override \
                                0x08000000  // Override value for
                                            // DCDC_DIG_SLP_EN : Applicable only
                                            // when bit [28] of
                                            // DIG_DCDC_PARAMETERS1 [0x000C] is
                                            // set to 1. Else from FSM

#define HIB1P2_DIG_DCDC_PARAMETERS0_mem_dcdc_dig_en_ldo_mode_lowv \
                                0x04000000

#define HIB1P2_DIG_DCDC_PARAMETERS0_mem_dcdc_dig_en_nfet_rds_mode_lowv \
                                0x02000000

#define HIB1P2_DIG_DCDC_PARAMETERS0_mem_dcdc_dig_en_pfet_rds_mode_lowv \
                                0x01000000

#define HIB1P2_DIG_DCDC_PARAMETERS0_mem_dcdc_dig_ext_smps_override_mode_lowv \
                                0x00800000

#define HIB1P2_DIG_DCDC_PARAMETERS0_mem_dcdc_dig_clk_in_lowv_enable \
                                0x00400000

#define HIB1P2_DIG_DCDC_PARAMETERS0_mem_dcdc_dig_vtrim_lowv_override_M \
                                0x003F0000  // Override value for
                                            // DCDC_DIG_VTRIM : Applicable only
                                            // when bit [27] of
                                            // DIG_DCDC_PARAMETERS1 [0x000C] is
                                            // set to 1.

#define HIB1P2_DIG_DCDC_PARAMETERS0_mem_dcdc_dig_vtrim_lowv_override_S 16
#define HIB1P2_DIG_DCDC_PARAMETERS0_mem_dcdc_dig_pfm_ripple_trim_lowv_M \
                                0x0000C000

#define HIB1P2_DIG_DCDC_PARAMETERS0_mem_dcdc_dig_pfm_ripple_trim_lowv_S 14
#define HIB1P2_DIG_DCDC_PARAMETERS0_mem_dcdc_dig_iq_ctrl_lowv_M \
                                0x00003000

#define HIB1P2_DIG_DCDC_PARAMETERS0_mem_dcdc_dig_iq_ctrl_lowv_S 12
#define HIB1P2_DIG_DCDC_PARAMETERS0_mem_dcdc_dig_en_cl_non_ov_lowv \
                                0x00000800

#define HIB1P2_DIG_DCDC_PARAMETERS0_mem_dcdc_dig_non_ov_ctrl_lowv_M \
                                0x00000780

#define HIB1P2_DIG_DCDC_PARAMETERS0_mem_dcdc_dig_non_ov_ctrl_lowv_S 7
#define HIB1P2_DIG_DCDC_PARAMETERS0_mem_dcdc_dig_slp_drv_dly_sel_lowv_M \
                                0x00000078

#define HIB1P2_DIG_DCDC_PARAMETERS0_mem_dcdc_dig_slp_drv_dly_sel_lowv_S 3
#define HIB1P2_DIG_DCDC_PARAMETERS0_NA3_M \
                                0x00000007

#define HIB1P2_DIG_DCDC_PARAMETERS0_NA3_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_DIG_DCDC_PARAMETERS1 register.
//
//******************************************************************************
#define HIB1P2_DIG_DCDC_PARAMETERS1_mem_dcdc_dig_en_lowv_fsm_override_ctrl \
                                0x80000000

#define HIB1P2_DIG_DCDC_PARAMETERS1_mem_dcdc_dig_en_subreg_1p8v_fsm_override_ctrl \
                                0x40000000

#define HIB1P2_DIG_DCDC_PARAMETERS1_mem_dcdc_dig_en_subreg_1p2v_fsm_override_ctrl \
                                0x20000000

#define HIB1P2_DIG_DCDC_PARAMETERS1_mem_dcdc_dig_en_slp_mode_lowv_fsm_override_ctrl \
                                0x10000000

#define HIB1P2_DIG_DCDC_PARAMETERS1_mem_dcdc_dig_vtrim_fsm_override_ctrl \
                                0x08000000

#define HIB1P2_DIG_DCDC_PARAMETERS1_mem_dcdc_dig_cot_mode_en_lowv_fsm_override_ctrl \
                                0x04000000

#define HIB1P2_DIG_DCDC_PARAMETERS1_mem_dcdc_dig_ilim_trim_lowv_efc_override_ctrl \
                                0x02000000

#define HIB1P2_DIG_DCDC_PARAMETERS1_NA4_M \
                                0x01FFFFFF

#define HIB1P2_DIG_DCDC_PARAMETERS1_NA4_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_DIG_DCDC_PARAMETERS2 register.
//
//******************************************************************************
#define HIB1P2_DIG_DCDC_PARAMETERS2_mem_dcdc_dig_pfet_sel_lowv_M \
                                0xF0000000

#define HIB1P2_DIG_DCDC_PARAMETERS2_mem_dcdc_dig_pfet_sel_lowv_S 28
#define HIB1P2_DIG_DCDC_PARAMETERS2_mem_dcdc_dig_nfet_sel_lowv_M \
                                0x0F000000

#define HIB1P2_DIG_DCDC_PARAMETERS2_mem_dcdc_dig_nfet_sel_lowv_S 24
#define HIB1P2_DIG_DCDC_PARAMETERS2_mem_dcdc_dig_pdrv_stagger_ctrl_lowv_M \
                                0x00C00000

#define HIB1P2_DIG_DCDC_PARAMETERS2_mem_dcdc_dig_pdrv_stagger_ctrl_lowv_S 22
#define HIB1P2_DIG_DCDC_PARAMETERS2_mem_dcdc_dig_ndrv_stagger_ctrl_lowv_M \
                                0x00300000

#define HIB1P2_DIG_DCDC_PARAMETERS2_mem_dcdc_dig_ndrv_stagger_ctrl_lowv_S 20
#define HIB1P2_DIG_DCDC_PARAMETERS2_mem_dcdc_dig_pdrv_str_sel_lowv_M \
                                0x000F0000

#define HIB1P2_DIG_DCDC_PARAMETERS2_mem_dcdc_dig_pdrv_str_sel_lowv_S 16
#define HIB1P2_DIG_DCDC_PARAMETERS2_NA5 \
                                0x00008000

#define HIB1P2_DIG_DCDC_PARAMETERS2_mem_dcdc_dig_ndrv_str_sel_lowv_M \
                                0x00007800

#define HIB1P2_DIG_DCDC_PARAMETERS2_mem_dcdc_dig_ndrv_str_sel_lowv_S 11
#define HIB1P2_DIG_DCDC_PARAMETERS2_mem_dcdc_dig_en_shootthru_ctrl_lowv \
                                0x00000400

#define HIB1P2_DIG_DCDC_PARAMETERS2_mem_dcdc_dig_ton_trim_lowv_M \
                                0x000003FC

#define HIB1P2_DIG_DCDC_PARAMETERS2_mem_dcdc_dig_ton_trim_lowv_S 2
#define HIB1P2_DIG_DCDC_PARAMETERS2_mem_dcdc_dig_swcap_res_hf_clk_lowv \
                                0x00000002

#define HIB1P2_DIG_DCDC_PARAMETERS2_mem_dcdc_dig_cot_mode_en_lowv_override \
                                0x00000001  // Override value for
                                            // DCDC_DIG_COT_EN : Applicable only
                                            // when bit[26] of
                                            // DIG_DCDC_PARAMETERS1 [0x000C] is
                                            // set to 1.

//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_DIG_DCDC_PARAMETERS3 register.
//
//******************************************************************************
#define HIB1P2_DIG_DCDC_PARAMETERS3_NA6 \
                                0x80000000

#define HIB1P2_DIG_DCDC_PARAMETERS3_mem_dcdc_dig_cot_ctrl_lowv_M \
                                0x7F800000

#define HIB1P2_DIG_DCDC_PARAMETERS3_mem_dcdc_dig_cot_ctrl_lowv_S 23
#define HIB1P2_DIG_DCDC_PARAMETERS3_mem_dcdc_dig_en_ilim_lowv \
                                0x00400000

#define HIB1P2_DIG_DCDC_PARAMETERS3_mem_dcdc_dig_en_ilim_hib_lowv \
                                0x00200000

#define HIB1P2_DIG_DCDC_PARAMETERS3_mem_dcdc_dig_ilim_trim_lowv_override_M \
                                0x001FE000  // Override value for
                                            // DCDC_DIG_ILIM_TRIM : Applicable
                                            // only when bit [25] of
                                            // DIG_DCDC_PARAMETERS1 [0x000C] is
                                            // set to 1

#define HIB1P2_DIG_DCDC_PARAMETERS3_mem_dcdc_dig_ilim_trim_lowv_override_S 13
#define HIB1P2_DIG_DCDC_PARAMETERS3_mem_dcdc_dig_ilim_mask_dly_sel_lowv_M \
                                0x00001800

#define HIB1P2_DIG_DCDC_PARAMETERS3_mem_dcdc_dig_ilim_mask_dly_sel_lowv_S 11
#define HIB1P2_DIG_DCDC_PARAMETERS3_mem_dcdc_dig_en_ncomp_lowv \
                                0x00000400

#define HIB1P2_DIG_DCDC_PARAMETERS3_mem_dcdc_dig_en_ncomp_hib_lowv \
                                0x00000200

#define HIB1P2_DIG_DCDC_PARAMETERS3_mem_dcdc_dig_ncomp_trim_lowv_M \
                                0x000001F0

#define HIB1P2_DIG_DCDC_PARAMETERS3_mem_dcdc_dig_ncomp_trim_lowv_S 4
#define HIB1P2_DIG_DCDC_PARAMETERS3_mem_dcdc_dig_ncomp_mask_dly_sel_lowv_M \
                                0x0000000C

#define HIB1P2_DIG_DCDC_PARAMETERS3_mem_dcdc_dig_ncomp_mask_dly_sel_lowv_S 2
#define HIB1P2_DIG_DCDC_PARAMETERS3_mem_dcdc_dig_en_uv_prot_lowv \
                                0x00000002

#define HIB1P2_DIG_DCDC_PARAMETERS3_mem_dcdc_dig_en_ov_prot_lowv \
                                0x00000001

//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_DIG_DCDC_PARAMETERS4 register.
//
//******************************************************************************
#define HIB1P2_DIG_DCDC_PARAMETERS4_dcdc_dig_uv_prot_out_lowv \
                                0x80000000

#define HIB1P2_DIG_DCDC_PARAMETERS4_dcdc_dig_ov_prot_out_lowv \
                                0x40000000

#define HIB1P2_DIG_DCDC_PARAMETERS4_mem_dcdc_dig_en_tmux_lowv \
                                0x20000000

#define HIB1P2_DIG_DCDC_PARAMETERS4_NA7_M \
                                0x1FFFFFFF

#define HIB1P2_DIG_DCDC_PARAMETERS4_NA7_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_DIG_DCDC_PARAMETERS5 register.
//
//******************************************************************************
#define HIB1P2_DIG_DCDC_PARAMETERS5_mem_dcdc_dig_tmux_ctrl_lowv_M \
                                0xFFFFFFFF

#define HIB1P2_DIG_DCDC_PARAMETERS5_mem_dcdc_dig_tmux_ctrl_lowv_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_DIG_DCDC_PARAMETERS6 register.
//
//******************************************************************************
#define HIB1P2_DIG_DCDC_PARAMETERS6_mem_dcdc_dig_spare_lowv_M \
                                0xFFFFFFFF

#define HIB1P2_DIG_DCDC_PARAMETERS6_mem_dcdc_dig_spare_lowv_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_ANA_DCDC_PARAMETERS0 register.
//
//******************************************************************************
#define HIB1P2_ANA_DCDC_PARAMETERS0_mem_dcdc_ana_en_lowv_override \
                                0x80000000  // Override for ANA DCDC EN

#define HIB1P2_ANA_DCDC_PARAMETERS0_mem_dcdc_ana_delayed_en_lowv \
                                0x40000000

#define HIB1P2_ANA_DCDC_PARAMETERS0_mem_dcdc_ana_en_subreg_1p8v_lowv \
                                0x20000000

#define HIB1P2_ANA_DCDC_PARAMETERS0_mem_dcdc_ana_en_subreg_1p2v_lowv \
                                0x10000000

#define HIB1P2_ANA_DCDC_PARAMETERS0_mem_dcdc_ana_en_pwm_mode_lowv_override \
                                0x08000000  // Override for ANA DCDC PWM

#define HIB1P2_ANA_DCDC_PARAMETERS0_mem_dcdc_ana_en_slp_mode_lowv_override \
                                0x04000000  // Override for ANA DCDC SLP

#define HIB1P2_ANA_DCDC_PARAMETERS0_mem_dcdc_ana_en_ldo_mode_lowv \
                                0x02000000

#define HIB1P2_ANA_DCDC_PARAMETERS0_mem_dcdc_ana_en_pfet_rds_mode_lowv \
                                0x01000000

#define HIB1P2_ANA_DCDC_PARAMETERS0_mem_dcdc_ana_en_nfet_rds_mode_lowv \
                                0x00800000

#define HIB1P2_ANA_DCDC_PARAMETERS0_mem_dcdc_ana_ext_smps_override_mode_lowv \
                                0x00400000

#define HIB1P2_ANA_DCDC_PARAMETERS0_mem_dcdc_ana_clk_in_lowv_enable \
                                0x00200000

#define HIB1P2_ANA_DCDC_PARAMETERS0_mem_dcdc_ana_vtrim_lowv_M \
                                0x001E0000

#define HIB1P2_ANA_DCDC_PARAMETERS0_mem_dcdc_ana_vtrim_lowv_S 17
#define HIB1P2_ANA_DCDC_PARAMETERS0_mem_dcdc_ana_pfm_ripple_trim_lowv_M \
                                0x00018000

#define HIB1P2_ANA_DCDC_PARAMETERS0_mem_dcdc_ana_pfm_ripple_trim_lowv_S 15
#define HIB1P2_ANA_DCDC_PARAMETERS0_mem_dcdc_ana_iq_ctrl_lowv_M \
                                0x00006000

#define HIB1P2_ANA_DCDC_PARAMETERS0_mem_dcdc_ana_iq_ctrl_lowv_S 13
#define HIB1P2_ANA_DCDC_PARAMETERS0_mem_dcdc_ana_en_cl_non_ov_lowv \
                                0x00001000

#define HIB1P2_ANA_DCDC_PARAMETERS0_mem_dcdc_ana_non_ov_ctrl_lowv_M \
                                0x00000F00

#define HIB1P2_ANA_DCDC_PARAMETERS0_mem_dcdc_ana_non_ov_ctrl_lowv_S 8
#define HIB1P2_ANA_DCDC_PARAMETERS0_mem_dcdc_ana_slp_drv_dly_sel_lowv_M \
                                0x000000F0

#define HIB1P2_ANA_DCDC_PARAMETERS0_mem_dcdc_ana_slp_drv_dly_sel_lowv_S 4
#define HIB1P2_ANA_DCDC_PARAMETERS0_mem_dcdc_ana_pfet_sel_lowv_M \
                                0x0000000F

#define HIB1P2_ANA_DCDC_PARAMETERS0_mem_dcdc_ana_pfet_sel_lowv_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_ANA_DCDC_PARAMETERS1 register.
//
//******************************************************************************
#define HIB1P2_ANA_DCDC_PARAMETERS1_mem_dcdc_ana_nfet_sel_lowv_M \
                                0xF0000000

#define HIB1P2_ANA_DCDC_PARAMETERS1_mem_dcdc_ana_nfet_sel_lowv_S 28
#define HIB1P2_ANA_DCDC_PARAMETERS1_mem_dcdc_ana_pdrv_stagger_ctrl_lowv_M \
                                0x0C000000

#define HIB1P2_ANA_DCDC_PARAMETERS1_mem_dcdc_ana_pdrv_stagger_ctrl_lowv_S 26
#define HIB1P2_ANA_DCDC_PARAMETERS1_mem_dcdc_ana_ndrv_stagger_ctrl_lowv_M \
                                0x03000000

#define HIB1P2_ANA_DCDC_PARAMETERS1_mem_dcdc_ana_ndrv_stagger_ctrl_lowv_S 24
#define HIB1P2_ANA_DCDC_PARAMETERS1_mem_dcdc_ana_pdrv_str_sel_lowv_M \
                                0x00F00000

#define HIB1P2_ANA_DCDC_PARAMETERS1_mem_dcdc_ana_pdrv_str_sel_lowv_S 20
#define HIB1P2_ANA_DCDC_PARAMETERS1_mem_dcdc_ana_ndrv_str_sel_lowv_M \
                                0x000F0000

#define HIB1P2_ANA_DCDC_PARAMETERS1_mem_dcdc_ana_ndrv_str_sel_lowv_S 16
#define HIB1P2_ANA_DCDC_PARAMETERS1_mem_dcdc_ana_en_rtrim_lowv \
                                0x00008000  // (Earlier SHOOTTHRU CTRL)

#define HIB1P2_ANA_DCDC_PARAMETERS1_mem_dcdc_ana_apwm_en_lowv \
                                0x00004000

#define HIB1P2_ANA_DCDC_PARAMETERS1_mem_dcdc_ana_ramp_hgt_lowv_M \
                                0x00003E00

#define HIB1P2_ANA_DCDC_PARAMETERS1_mem_dcdc_ana_ramp_hgt_lowv_S 9
#define HIB1P2_ANA_DCDC_PARAMETERS1_mem_dcdc_ana_en_anti_glitch_lowv \
                                0x00000100

#define HIB1P2_ANA_DCDC_PARAMETERS1_mem_dcdc_ana_en_hi_clamp_lowv \
                                0x00000080

#define HIB1P2_ANA_DCDC_PARAMETERS1_mem_dcdc_ana_hi_clamp_trim_lowv_M \
                                0x00000060

#define HIB1P2_ANA_DCDC_PARAMETERS1_mem_dcdc_ana_hi_clamp_trim_lowv_S 5
#define HIB1P2_ANA_DCDC_PARAMETERS1_mem_dcdc_ana_en_lo_clamp_lowv \
                                0x00000010

#define HIB1P2_ANA_DCDC_PARAMETERS1_mem_dcdc_ana_lo_clamp_trim_lowv_M \
                                0x0000000C

#define HIB1P2_ANA_DCDC_PARAMETERS1_mem_dcdc_ana_lo_clamp_trim_lowv_S 2
#define HIB1P2_ANA_DCDC_PARAMETERS1_NA8_M \
                                0x00000003

#define HIB1P2_ANA_DCDC_PARAMETERS1_NA8_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_ANA_DCDC_PARAMETERS16 register.
//
//******************************************************************************
#define HIB1P2_ANA_DCDC_PARAMETERS16_mem_dcdc_ana_en_ilim_lowv \
                                0x00200000

#define HIB1P2_ANA_DCDC_PARAMETERS16_mem_dcdc_ana_en_ilim_hib_lowv \
                                0x00100000

#define HIB1P2_ANA_DCDC_PARAMETERS16_mem_dcdc_ana_ilim_trim_lowv_override_M \
                                0x000FF000

#define HIB1P2_ANA_DCDC_PARAMETERS16_mem_dcdc_ana_ilim_trim_lowv_override_S 12
#define HIB1P2_ANA_DCDC_PARAMETERS16_mem_dcdc_ana_ilim_mask_dly_sel_lowv_M \
                                0x00000C00

#define HIB1P2_ANA_DCDC_PARAMETERS16_mem_dcdc_ana_ilim_mask_dly_sel_lowv_S 10
#define HIB1P2_ANA_DCDC_PARAMETERS16_mem_dcdc_ana_en_ncomp_lowv \
                                0x00000200

#define HIB1P2_ANA_DCDC_PARAMETERS16_mem_dcdc_ana_en_ncomp_hib_lowv \
                                0x00000100

#define HIB1P2_ANA_DCDC_PARAMETERS16_mem_dcdc_ana_ncomp_trim_lowv_M \
                                0x000000F8

#define HIB1P2_ANA_DCDC_PARAMETERS16_mem_dcdc_ana_ncomp_trim_lowv_S 3
#define HIB1P2_ANA_DCDC_PARAMETERS16_mem_dcdc_ana_ncomp_mask_dly_sel_lowv_M \
                                0x00000006

#define HIB1P2_ANA_DCDC_PARAMETERS16_mem_dcdc_ana_ncomp_mask_dly_sel_lowv_S 1
#define HIB1P2_ANA_DCDC_PARAMETERS16_mem_dcdc_ana_en_ov_prot_lowv \
                                0x00000001

//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_ANA_DCDC_PARAMETERS17 register.
//
//******************************************************************************
#define HIB1P2_ANA_DCDC_PARAMETERS17_dcdc_ana_ov_prot_out_lowv \
                                0x80000000

#define HIB1P2_ANA_DCDC_PARAMETERS17_mem_dcdc_ana_en_tmux_lowv \
                                0x40000000

#define HIB1P2_ANA_DCDC_PARAMETERS17_NA17_M \
                                0x3FFFFFFF

#define HIB1P2_ANA_DCDC_PARAMETERS17_NA17_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_ANA_DCDC_PARAMETERS18 register.
//
//******************************************************************************
#define HIB1P2_ANA_DCDC_PARAMETERS18_mem_dcdc_ana_tmux_ctrl_lowv_M \
                                0xFFFFFFFF

#define HIB1P2_ANA_DCDC_PARAMETERS18_mem_dcdc_ana_tmux_ctrl_lowv_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_ANA_DCDC_PARAMETERS19 register.
//
//******************************************************************************
#define HIB1P2_ANA_DCDC_PARAMETERS19_mem_dcdc_ana_spare_lowv_M \
                                0xFFFFFFFF

#define HIB1P2_ANA_DCDC_PARAMETERS19_mem_dcdc_ana_spare_lowv_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_FLASH_DCDC_PARAMETERS0 register.
//
//******************************************************************************
#define HIB1P2_FLASH_DCDC_PARAMETERS0_mem_dcdc_flash_en_lowv \
                                0x80000000

#define HIB1P2_FLASH_DCDC_PARAMETERS0_mem_dcdc_flash_delayed_en_lowv \
                                0x40000000

#define HIB1P2_FLASH_DCDC_PARAMETERS0_mem_dcdc_flash_clk_in_lowv_enable \
                                0x20000000

#define HIB1P2_FLASH_DCDC_PARAMETERS0_mem_dcdc_flash_iq_ctrl_lowv_M \
                                0x18000000

#define HIB1P2_FLASH_DCDC_PARAMETERS0_mem_dcdc_flash_iq_ctrl_lowv_S 27
#define HIB1P2_FLASH_DCDC_PARAMETERS0_mem_dcdc_flash_en_buck_mode_lowv \
                                0x04000000

#define HIB1P2_FLASH_DCDC_PARAMETERS0_mem_dcdc_flash_en_boost_mode_lowv \
                                0x02000000

#define HIB1P2_FLASH_DCDC_PARAMETERS0_mem_dcdc_flash_en_buck_boost_mode_lowv \
                                0x01000000

#define HIB1P2_FLASH_DCDC_PARAMETERS0_mem_dcdc_flash_en_bb_alt_cycles_lowv \
                                0x00800000

#define HIB1P2_FLASH_DCDC_PARAMETERS0_mem_dcdc_flash_en_cl_non_ov_lowv \
                                0x00400000

#define HIB1P2_FLASH_DCDC_PARAMETERS0_mem_dcdc_flash_non_ov_ctrl_lowv_M \
                                0x003C0000

#define HIB1P2_FLASH_DCDC_PARAMETERS0_mem_dcdc_flash_non_ov_ctrl_lowv_S 18
#define HIB1P2_FLASH_DCDC_PARAMETERS0_mem_dcdc_flash_en_drv_lowv \
                                0x00020000

#define HIB1P2_FLASH_DCDC_PARAMETERS0_mem_dcdc_flash_en_pwm_mode_lowv \
                                0x00010000

#define HIB1P2_FLASH_DCDC_PARAMETERS0_mem_dcdc_flash_en_pfm_comp_lowv \
                                0x00008000

#define HIB1P2_FLASH_DCDC_PARAMETERS0_mem_dcdc_flash_en_slp_mode_lowv \
                                0x00004000

#define HIB1P2_FLASH_DCDC_PARAMETERS0_mem_dcdc_flash_en_n1fet_rds_mode_lowv \
                                0x00002000

#define HIB1P2_FLASH_DCDC_PARAMETERS0_mem_dcdc_flash_en_n2fet_rds_mode_lowv \
                                0x00001000

#define HIB1P2_FLASH_DCDC_PARAMETERS0_mem_dcdc_flash_en_p1fet_rds_mode_lowv \
                                0x00000800

#define HIB1P2_FLASH_DCDC_PARAMETERS0_mem_dcdc_flash_en_p2fet_rds_mode_lowv \
                                0x00000400

#define HIB1P2_FLASH_DCDC_PARAMETERS0_mem_dcdc_flash_ext_smps_mode_override_lowv \
                                0x00000200

#define HIB1P2_FLASH_DCDC_PARAMETERS0_mem_dcdc_flash_p1fet_sel_lowv_M \
                                0x000001E0

#define HIB1P2_FLASH_DCDC_PARAMETERS0_mem_dcdc_flash_p1fet_sel_lowv_S 5
#define HIB1P2_FLASH_DCDC_PARAMETERS0_mem_dcdc_flash_n1fet_sel_lowv_M \
                                0x0000001E

#define HIB1P2_FLASH_DCDC_PARAMETERS0_mem_dcdc_flash_n1fet_sel_lowv_S 1
#define HIB1P2_FLASH_DCDC_PARAMETERS0_NA18 \
                                0x00000001

//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_FLASH_DCDC_PARAMETERS1 register.
//
//******************************************************************************
#define HIB1P2_FLASH_DCDC_PARAMETERS1_mem_dcdc_flash_p2fet_sel_lowv_M \
                                0xF0000000

#define HIB1P2_FLASH_DCDC_PARAMETERS1_mem_dcdc_flash_p2fet_sel_lowv_S 28
#define HIB1P2_FLASH_DCDC_PARAMETERS1_mem_dcdc_flash_n2fet_sel_lowv_M \
                                0x0F000000

#define HIB1P2_FLASH_DCDC_PARAMETERS1_mem_dcdc_flash_n2fet_sel_lowv_S 24
#define HIB1P2_FLASH_DCDC_PARAMETERS1_mem_dcdc_flash_p1drv_str_sel_lowv_M \
                                0x00F00000

#define HIB1P2_FLASH_DCDC_PARAMETERS1_mem_dcdc_flash_p1drv_str_sel_lowv_S 20
#define HIB1P2_FLASH_DCDC_PARAMETERS1_mem_dcdc_flash_n1drv_str_sel_lowv_M \
                                0x000F0000

#define HIB1P2_FLASH_DCDC_PARAMETERS1_mem_dcdc_flash_n1drv_str_sel_lowv_S 16
#define HIB1P2_FLASH_DCDC_PARAMETERS1_mem_dcdc_flash_p2drv_str_sel_lowv_M \
                                0x0000F000

#define HIB1P2_FLASH_DCDC_PARAMETERS1_mem_dcdc_flash_p2drv_str_sel_lowv_S 12
#define HIB1P2_FLASH_DCDC_PARAMETERS1_mem_dcdc_flash_n2drv_str_sel_lowv_M \
                                0x00000F00

#define HIB1P2_FLASH_DCDC_PARAMETERS1_mem_dcdc_flash_n2drv_str_sel_lowv_S 8
#define HIB1P2_FLASH_DCDC_PARAMETERS1_mem_dcdc_flash_p1fet_non_ov_lowv_M \
                                0x000000C0

#define HIB1P2_FLASH_DCDC_PARAMETERS1_mem_dcdc_flash_p1fet_non_ov_lowv_S 6
#define HIB1P2_FLASH_DCDC_PARAMETERS1_mem_dcdc_flash_n1fet_non_ov_lowv_M \
                                0x00000030

#define HIB1P2_FLASH_DCDC_PARAMETERS1_mem_dcdc_flash_n1fet_non_ov_lowv_S 4
#define HIB1P2_FLASH_DCDC_PARAMETERS1_mem_dcdc_flash_p2fet_non_ov_lowv_M \
                                0x0000000C

#define HIB1P2_FLASH_DCDC_PARAMETERS1_mem_dcdc_flash_p2fet_non_ov_lowv_S 2
#define HIB1P2_FLASH_DCDC_PARAMETERS1_mem_dcdc_flash_n2fet_non_ov_lowv_M \
                                0x00000003

#define HIB1P2_FLASH_DCDC_PARAMETERS1_mem_dcdc_flash_n2fet_non_ov_lowv_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_FLASH_DCDC_PARAMETERS2 register.
//
//******************************************************************************
#define HIB1P2_FLASH_DCDC_PARAMETERS2_mem_dcdc_flash_p1fet_stagger_lowv_M \
                                0xC0000000

#define HIB1P2_FLASH_DCDC_PARAMETERS2_mem_dcdc_flash_p1fet_stagger_lowv_S 30
#define HIB1P2_FLASH_DCDC_PARAMETERS2_mem_dcdc_flash_n1fet_stagger_lowv_M \
                                0x30000000

#define HIB1P2_FLASH_DCDC_PARAMETERS2_mem_dcdc_flash_n1fet_stagger_lowv_S 28
#define HIB1P2_FLASH_DCDC_PARAMETERS2_mem_dcdc_flash_p2fet_stagger_lowv_M \
                                0x0C000000

#define HIB1P2_FLASH_DCDC_PARAMETERS2_mem_dcdc_flash_p2fet_stagger_lowv_S 26
#define HIB1P2_FLASH_DCDC_PARAMETERS2_mem_dcdc_flash_n2fet_stagger_lowv_M \
                                0x03000000

#define HIB1P2_FLASH_DCDC_PARAMETERS2_mem_dcdc_flash_n2fet_stagger_lowv_S 24
#define HIB1P2_FLASH_DCDC_PARAMETERS2_mem_dcdc_flash_shoot_thru_ctrl_lowv \
                                0x00800000

#define HIB1P2_FLASH_DCDC_PARAMETERS2_mem_dcdc_flash_en_ncomp_lowv \
                                0x00400000

#define HIB1P2_FLASH_DCDC_PARAMETERS2_mem_dcdc_flash_en_ncomp_hib_lowv \
                                0x00200000

#define HIB1P2_FLASH_DCDC_PARAMETERS2_mem_dcdc_flash_ncomp_trim_lowv_M \
                                0x001F0000

#define HIB1P2_FLASH_DCDC_PARAMETERS2_mem_dcdc_flash_ncomp_trim_lowv_S 16
#define HIB1P2_FLASH_DCDC_PARAMETERS2_mem_dcdc_flash_ncomp_mask_dly_trim_lowv_M \
                                0x0000F000

#define HIB1P2_FLASH_DCDC_PARAMETERS2_mem_dcdc_flash_ncomp_mask_dly_trim_lowv_S 12
#define HIB1P2_FLASH_DCDC_PARAMETERS2_mem_dcdc_flash_en_ilim_lowv \
                                0x00000800

#define HIB1P2_FLASH_DCDC_PARAMETERS2_mem_dcdc_flash_en_ilim_hib_lowv \
                                0x00000400

#define HIB1P2_FLASH_DCDC_PARAMETERS2_mem_dcdc_flash_ilim_trim_lowv_override_M \
                                0x000003FC

#define HIB1P2_FLASH_DCDC_PARAMETERS2_mem_dcdc_flash_ilim_trim_lowv_override_S 2
#define HIB1P2_FLASH_DCDC_PARAMETERS2_mem_dcdc_flash_ilim_mask_dly_sel_lowv_M \
                                0x00000003

#define HIB1P2_FLASH_DCDC_PARAMETERS2_mem_dcdc_flash_ilim_mask_dly_sel_lowv_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_FLASH_DCDC_PARAMETERS3 register.
//
//******************************************************************************
#define HIB1P2_FLASH_DCDC_PARAMETERS3_mem_dcdc_flash_en_anti_glitch_lowv \
                                0x80000000

#define HIB1P2_FLASH_DCDC_PARAMETERS3_mem_dcdc_flash_en_hi_clamp_lowv \
                                0x40000000

#define HIB1P2_FLASH_DCDC_PARAMETERS3_mem_dcdc_flash_en_lo_clamp_lowv \
                                0x20000000

#define HIB1P2_FLASH_DCDC_PARAMETERS3_mem_dcdc_flash_ramp_hgt_lowv_M \
                                0x1F000000

#define HIB1P2_FLASH_DCDC_PARAMETERS3_mem_dcdc_flash_ramp_hgt_lowv_S 24
#define HIB1P2_FLASH_DCDC_PARAMETERS3_mem_dcdc_flash_vclamph_trim_lowv_M \
                                0x00E00000

#define HIB1P2_FLASH_DCDC_PARAMETERS3_mem_dcdc_flash_vclamph_trim_lowv_S 21
#define HIB1P2_FLASH_DCDC_PARAMETERS3_mem_dcdc_flash_vclampl_trim_lowv_M \
                                0x001C0000

#define HIB1P2_FLASH_DCDC_PARAMETERS3_mem_dcdc_flash_vclampl_trim_lowv_S 18
#define HIB1P2_FLASH_DCDC_PARAMETERS3_mem_dcdc_flash_vtrim_lowv_M \
                                0x0003C000

#define HIB1P2_FLASH_DCDC_PARAMETERS3_mem_dcdc_flash_vtrim_lowv_S 14
#define HIB1P2_FLASH_DCDC_PARAMETERS3_mem_dcdc_flash_pfm_ripple_trim_lowv_M \
                                0x00003C00

#define HIB1P2_FLASH_DCDC_PARAMETERS3_mem_dcdc_flash_pfm_ripple_trim_lowv_S 10
#define HIB1P2_FLASH_DCDC_PARAMETERS3_mem_dcdc_flash_slp_drv_dly_sel_lowv_M \
                                0x00000300

#define HIB1P2_FLASH_DCDC_PARAMETERS3_mem_dcdc_flash_slp_drv_dly_sel_lowv_S 8
#define HIB1P2_FLASH_DCDC_PARAMETERS3_mem_dcdc_flash_en_ov_prot_lowv \
                                0x00000080

#define HIB1P2_FLASH_DCDC_PARAMETERS3_mem_dcdc_flash_en_uv_prot_lowv \
                                0x00000040

#define HIB1P2_FLASH_DCDC_PARAMETERS3_mem_dcdc_flash_en_tmux_lowv \
                                0x00000020

#define HIB1P2_FLASH_DCDC_PARAMETERS3_NA19_M \
                                0x0000001F

#define HIB1P2_FLASH_DCDC_PARAMETERS3_NA19_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_FLASH_DCDC_PARAMETERS4 register.
//
//******************************************************************************
#define HIB1P2_FLASH_DCDC_PARAMETERS4_mem_dcdc_flash_tmux_ctrl_lowv_M \
                                0xFFFFFFFF

#define HIB1P2_FLASH_DCDC_PARAMETERS4_mem_dcdc_flash_tmux_ctrl_lowv_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_FLASH_DCDC_PARAMETERS5 register.
//
//******************************************************************************
#define HIB1P2_FLASH_DCDC_PARAMETERS5_mem_dcdc_flash_spare_lowv_M \
                                0xFFFFFFFF

#define HIB1P2_FLASH_DCDC_PARAMETERS5_mem_dcdc_flash_spare_lowv_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_FLASH_DCDC_PARAMETERS6 register.
//
//******************************************************************************
#define HIB1P2_FLASH_DCDC_PARAMETERS6_dcdc_flash_ov_prot_out_lowv \
                                0x80000000

#define HIB1P2_FLASH_DCDC_PARAMETERS6_dcdc_flash_uv_prot_out_lowv \
                                0x40000000

#define HIB1P2_FLASH_DCDC_PARAMETERS6_NA20_M \
                                0x3FFFFFFF

#define HIB1P2_FLASH_DCDC_PARAMETERS6_NA20_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_PMBIST_PARAMETERS0 register.
//
//******************************************************************************
#define HIB1P2_PMBIST_PARAMETERS0_mem_pm_bist_en_lowv \
                                0x80000000

#define HIB1P2_PMBIST_PARAMETERS0_mem_pm_bist_ctrl_lowv_M \
                                0x7FFFF800

#define HIB1P2_PMBIST_PARAMETERS0_mem_pm_bist_ctrl_lowv_S 11
#define HIB1P2_PMBIST_PARAMETERS0_NA21_M \
                                0x000007FF

#define HIB1P2_PMBIST_PARAMETERS0_NA21_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_PMBIST_PARAMETERS1 register.
//
//******************************************************************************
#define HIB1P2_PMBIST_PARAMETERS1_mem_pm_bist_spare_lowv_M \
                                0xFFFF0000

#define HIB1P2_PMBIST_PARAMETERS1_mem_pm_bist_spare_lowv_S 16
#define HIB1P2_PMBIST_PARAMETERS1_mem_pmtest_en_lowv \
                                0x00008000

#define HIB1P2_PMBIST_PARAMETERS1_NA22_M \
                                0x00007FFF

#define HIB1P2_PMBIST_PARAMETERS1_NA22_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_PMBIST_PARAMETERS2 register.
//
//******************************************************************************
#define HIB1P2_PMBIST_PARAMETERS2_mem_pmtest_tmux_ctrl_lowv_M \
                                0xFFFFFFFF

#define HIB1P2_PMBIST_PARAMETERS2_mem_pmtest_tmux_ctrl_lowv_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_PMBIST_PARAMETERS3 register.
//
//******************************************************************************
#define HIB1P2_PMBIST_PARAMETERS3_mem_pmtest_spare_lowv_M \
                                0xFFFF0000

#define HIB1P2_PMBIST_PARAMETERS3_mem_pmtest_spare_lowv_S 16
#define HIB1P2_PMBIST_PARAMETERS3_mem_pmtest_load_trim_lowv_M \
                                0x0000E000

#define HIB1P2_PMBIST_PARAMETERS3_mem_pmtest_load_trim_lowv_S 13
#define HIB1P2_PMBIST_PARAMETERS3_mem_rnwell_calib_en_lowv \
                                0x00001000

#define HIB1P2_PMBIST_PARAMETERS3_NA23_M \
                                0x00000FFF

#define HIB1P2_PMBIST_PARAMETERS3_NA23_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_FLASH_DCDC_PARAMETERS8 register.
//
//******************************************************************************
#define HIB1P2_FLASH_DCDC_PARAMETERS8_mem_en_flash_sup_comp_lowv \
                                0x80000000

#define HIB1P2_FLASH_DCDC_PARAMETERS8_mem_flash_high_sup_trim_lowv_M \
                                0x7C000000

#define HIB1P2_FLASH_DCDC_PARAMETERS8_mem_flash_high_sup_trim_lowv_S 26
#define HIB1P2_FLASH_DCDC_PARAMETERS8_mem_flash_low_sup_trim_lowv_M \
                                0x03E00000

#define HIB1P2_FLASH_DCDC_PARAMETERS8_mem_flash_low_sup_trim_lowv_S 21
#define HIB1P2_FLASH_DCDC_PARAMETERS8_NA24_M \
                                0x001FFFFF

#define HIB1P2_FLASH_DCDC_PARAMETERS8_NA24_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_ANA_DCDC_PARAMETERS_OVERRIDE register.
//
//******************************************************************************
#define HIB1P2_ANA_DCDC_PARAMETERS_OVERRIDE_reserved_M \
                                0xFFFFFFC0

#define HIB1P2_ANA_DCDC_PARAMETERS_OVERRIDE_reserved_S 6
#define HIB1P2_ANA_DCDC_PARAMETERS_OVERRIDE_mem_dcdc_ana_en_subreg_1p2v_lowv_override_ctrl \
                                0x00000020

#define HIB1P2_ANA_DCDC_PARAMETERS_OVERRIDE_mem_dcdc_ana_en_subreg_1p8v_lowv_override_ctrl \
                                0x00000010

#define HIB1P2_ANA_DCDC_PARAMETERS_OVERRIDE_mem_dcdc_ana_ilim_trim_lowv_efc_override_ctrl \
                                0x00000008

#define HIB1P2_ANA_DCDC_PARAMETERS_OVERRIDE_mem_dcdc_ana_en_slp_mode_lowv_fsm_override_ctrl \
                                0x00000004

#define HIB1P2_ANA_DCDC_PARAMETERS_OVERRIDE_mem_dcdc_ana_en_pwm_mode_lowv_fsm_override_ctrl \
                                0x00000002

#define HIB1P2_ANA_DCDC_PARAMETERS_OVERRIDE_mem_dcdc_ana_en_lowv_fsm_override_ctrl \
                                0x00000001

//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_FLASH_DCDC_PARAMETERS_OVERRIDE register.
//
//******************************************************************************
#define HIB1P2_FLASH_DCDC_PARAMETERS_OVERRIDE_reserved_M \
                                0xFFFFFFFC

#define HIB1P2_FLASH_DCDC_PARAMETERS_OVERRIDE_reserved_S 2
#define HIB1P2_FLASH_DCDC_PARAMETERS_OVERRIDE_mem_dcdc_flash_en_lowv_override_ctrl \
                                0x00000002

#define HIB1P2_FLASH_DCDC_PARAMETERS_OVERRIDE_mem_dcdc_flash_ilim_trim_lowv_override_ctrl \
                                0x00000001

//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_DIG_DCDC_VTRIM_CFG register.
//
//******************************************************************************
#define HIB1P2_DIG_DCDC_VTRIM_CFG_reserved_M \
                                0xFF000000

#define HIB1P2_DIG_DCDC_VTRIM_CFG_reserved_S 24
#define HIB1P2_DIG_DCDC_VTRIM_CFG_mem_dcdc_dig_run_vtrim_M \
                                0x00FC0000

#define HIB1P2_DIG_DCDC_VTRIM_CFG_mem_dcdc_dig_run_vtrim_S 18
#define HIB1P2_DIG_DCDC_VTRIM_CFG_mem_dcdc_dig_dslp_vtrim_M \
                                0x0003F000

#define HIB1P2_DIG_DCDC_VTRIM_CFG_mem_dcdc_dig_dslp_vtrim_S 12
#define HIB1P2_DIG_DCDC_VTRIM_CFG_mem_dcdc_dig_lpds_vtrim_M \
                                0x00000FC0

#define HIB1P2_DIG_DCDC_VTRIM_CFG_mem_dcdc_dig_lpds_vtrim_S 6
#define HIB1P2_DIG_DCDC_VTRIM_CFG_Spare_RW_M \
                                0x0000003F

#define HIB1P2_DIG_DCDC_VTRIM_CFG_Spare_RW_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_DIG_DCDC_FSM_PARAMETERS register.
//
//******************************************************************************
#define HIB1P2_DIG_DCDC_FSM_PARAMETERS_reserved_M \
                                0xFFFF8000

#define HIB1P2_DIG_DCDC_FSM_PARAMETERS_reserved_S 15
#define HIB1P2_DIG_DCDC_FSM_PARAMETERS_mem_dcdc_dig_dslp_enter_cot_to_vtrim_M \
                                0x00007000

#define HIB1P2_DIG_DCDC_FSM_PARAMETERS_mem_dcdc_dig_dslp_enter_cot_to_vtrim_S 12
#define HIB1P2_DIG_DCDC_FSM_PARAMETERS_mem_dcdc_dig_dslp_enter_vtrim_to_sleep_M \
                                0x00000E00

#define HIB1P2_DIG_DCDC_FSM_PARAMETERS_mem_dcdc_dig_dslp_enter_vtrim_to_sleep_S 9
#define HIB1P2_DIG_DCDC_FSM_PARAMETERS_mem_dcdc_dig_dslp_exit_sleep_to_vtrim_M \
                                0x000001C0

#define HIB1P2_DIG_DCDC_FSM_PARAMETERS_mem_dcdc_dig_dslp_exit_sleep_to_vtrim_S 6
#define HIB1P2_DIG_DCDC_FSM_PARAMETERS_mem_dcdc_dig_dslp_exit_vtrim_to_cot_M \
                                0x00000038

#define HIB1P2_DIG_DCDC_FSM_PARAMETERS_mem_dcdc_dig_dslp_exit_vtrim_to_cot_S 3
#define HIB1P2_DIG_DCDC_FSM_PARAMETERS_mem_dcdc_dig_dslp_exit_cot_to_run_M \
                                0x00000007

#define HIB1P2_DIG_DCDC_FSM_PARAMETERS_mem_dcdc_dig_dslp_exit_cot_to_run_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_ANA_DCDC_FSM_PARAMETERS register.
//
//******************************************************************************
#define HIB1P2_ANA_DCDC_FSM_PARAMETERS_reserved_M \
                                0xFFFFFFF8

#define HIB1P2_ANA_DCDC_FSM_PARAMETERS_reserved_S 3
#define HIB1P2_ANA_DCDC_FSM_PARAMETERS_mem_dcdc_ana_dslp_exit_sleep_to_run_M \
                                0x00000007

#define HIB1P2_ANA_DCDC_FSM_PARAMETERS_mem_dcdc_ana_dslp_exit_sleep_to_run_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_SRAM_SKA_LDO_FSM_PARAMETERS register.
//
//******************************************************************************
#define HIB1P2_SRAM_SKA_LDO_FSM_PARAMETERS_reserved_M \
                                0xFFFFFFC0

#define HIB1P2_SRAM_SKA_LDO_FSM_PARAMETERS_reserved_S 6
#define HIB1P2_SRAM_SKA_LDO_FSM_PARAMETERS_mem_ska_ldo_en_to_sram_ldo_dis_M \
                                0x00000038

#define HIB1P2_SRAM_SKA_LDO_FSM_PARAMETERS_mem_ska_ldo_en_to_sram_ldo_dis_S 3
#define HIB1P2_SRAM_SKA_LDO_FSM_PARAMETERS_mem_sram_ldo_en_to_ska_ldo_dis_M \
                                0x00000007

#define HIB1P2_SRAM_SKA_LDO_FSM_PARAMETERS_mem_sram_ldo_en_to_ska_ldo_dis_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_BGAP_DUTY_CYCLING_EXIT_CFG register.
//
//******************************************************************************
#define HIB1P2_BGAP_DUTY_CYCLING_EXIT_CFG_reserved_M \
                                0xFFFFFFF8

#define HIB1P2_BGAP_DUTY_CYCLING_EXIT_CFG_reserved_S 3
#define HIB1P2_BGAP_DUTY_CYCLING_EXIT_CFG_mem_bgap_duty_cycling_exit_time_M \
                                0x00000007

#define HIB1P2_BGAP_DUTY_CYCLING_EXIT_CFG_mem_bgap_duty_cycling_exit_time_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_CM_OSC_16M_CONFIG register.
//
//******************************************************************************
#define HIB1P2_CM_OSC_16M_CONFIG_reserved_M \
                                0xFFFC0000

#define HIB1P2_CM_OSC_16M_CONFIG_reserved_S 18
#define HIB1P2_CM_OSC_16M_CONFIG_cm_clk_good_16m \
                                0x00020000

#define HIB1P2_CM_OSC_16M_CONFIG_mem_cm_en_osc_16m \
                                0x00010000

#define HIB1P2_CM_OSC_16M_CONFIG_mem_cm_osc_16m_trim_M \
                                0x0000FC00

#define HIB1P2_CM_OSC_16M_CONFIG_mem_cm_osc_16m_trim_S 10
#define HIB1P2_CM_OSC_16M_CONFIG_mem_cm_osc_16m_spare_M \
                                0x000003F0

#define HIB1P2_CM_OSC_16M_CONFIG_mem_cm_osc_16m_spare_S 4
#define HIB1P2_CM_OSC_16M_CONFIG_mem_cm_osc_en_sli_16m \
                                0x00000008

#define HIB1P2_CM_OSC_16M_CONFIG_mem_cm_sli_16m_trim_M \
                                0x00000007

#define HIB1P2_CM_OSC_16M_CONFIG_mem_cm_sli_16m_trim_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_SOP_SENSE_VALUE register.
//
//******************************************************************************
#define HIB1P2_SOP_SENSE_VALUE_reserved_M \
                                0xFFFFFF00

#define HIB1P2_SOP_SENSE_VALUE_reserved_S 8
#define HIB1P2_SOP_SENSE_VALUE_sop_sense_value_M \
                                0x000000FF

#define HIB1P2_SOP_SENSE_VALUE_sop_sense_value_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_HIB_RTC_TIMER_LSW_1P2 register.
//
//******************************************************************************
#define HIB1P2_HIB_RTC_TIMER_LSW_1P2_hib_rtc_timer_lsw_M \
                                0xFFFFFFFF

#define HIB1P2_HIB_RTC_TIMER_LSW_1P2_hib_rtc_timer_lsw_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_HIB_RTC_TIMER_MSW_1P2 register.
//
//******************************************************************************
#define HIB1P2_HIB_RTC_TIMER_MSW_1P2_hib_rtc_timer_msw_M \
                                0x0000FFFF

#define HIB1P2_HIB_RTC_TIMER_MSW_1P2_hib_rtc_timer_msw_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_HIB1P2_BGAP_TRIM_OVERRIDES register.
//
//******************************************************************************
#define HIB1P2_HIB1P2_BGAP_TRIM_OVERRIDES_reserved_M \
                                0xFF800000

#define HIB1P2_HIB1P2_BGAP_TRIM_OVERRIDES_reserved_S 23
#define HIB1P2_HIB1P2_BGAP_TRIM_OVERRIDES_mem_bgap_mag_trim_override_ctrl \
                                0x00400000

#define HIB1P2_HIB1P2_BGAP_TRIM_OVERRIDES_mem_bgap_mag_trim_override_M \
                                0x003FC000

#define HIB1P2_HIB1P2_BGAP_TRIM_OVERRIDES_mem_bgap_mag_trim_override_S 14
#define HIB1P2_HIB1P2_BGAP_TRIM_OVERRIDES_mem_bgap_temp_trim_override_ctrl \
                                0x00002000

#define HIB1P2_HIB1P2_BGAP_TRIM_OVERRIDES_mem_bgap_temp_trim_override_M \
                                0x00001FC0

#define HIB1P2_HIB1P2_BGAP_TRIM_OVERRIDES_mem_bgap_temp_trim_override_S 6
#define HIB1P2_HIB1P2_BGAP_TRIM_OVERRIDES_mem_bgap_rtrim_override_ctrl \
                                0x00000020

#define HIB1P2_HIB1P2_BGAP_TRIM_OVERRIDES_mem_bgap_rtrim_override_M \
                                0x0000001F

#define HIB1P2_HIB1P2_BGAP_TRIM_OVERRIDES_mem_bgap_rtrim_override_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_HIB1P2_EFUSE_READ_REG0 register.
//
//******************************************************************************
#define HIB1P2_HIB1P2_EFUSE_READ_REG0_FUSEFARM_ROW_12_M \
                                0xFFFFFFFF  // Corresponds to ROW_12 of
                                            // FUSEFARM. [7:0] :
                                            // DCDC_DIG_ILIM_TRIM_LOWV(7:0)
                                            // [15:8] :
                                            // DCDC_ANA_ILIM_TRIM_LOWV(7:0)
                                            // [23:16] :
                                            // DCDC_FLASH_ILIM_TRIM_LOWV(7:0)
                                            // [24:24] : DTHE SHA DISABLE
                                            // [25:25] : DTHE DES DISABLE
                                            // [26:26] : DTHE AES DISABLE
                                            // [31:27] : HD_BG_RTRIM (4:0)

#define HIB1P2_HIB1P2_EFUSE_READ_REG0_FUSEFARM_ROW_12_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_HIB1P2_EFUSE_READ_REG1 register.
//
//******************************************************************************
#define HIB1P2_HIB1P2_EFUSE_READ_REG1_FUSEFARM_ROW_13_M \
                                0xFFFFFFFF  // Corresponds to ROW_13 of the
                                            // FUSEFARM. [7:0] : HD_BG_MAG_TRIM
                                            // (7:0) [14:8] : HD_BG_TEMP_TRIM
                                            // (6:0) [15:15] : GREYOUT ENABLE
                                            // DUTY CYCLING [31:16] :
                                            // Reserved/Checksum

#define HIB1P2_HIB1P2_EFUSE_READ_REG1_FUSEFARM_ROW_13_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_HIB1P2_POR_TEST_CTRL register.
//
//******************************************************************************
#define HIB1P2_HIB1P2_POR_TEST_CTRL_reserved_M \
                                0xFFFFFF00

#define HIB1P2_HIB1P2_POR_TEST_CTRL_reserved_S 8
#define HIB1P2_HIB1P2_POR_TEST_CTRL_mem_prcm_por_test_ctrl_M \
                                0x000000FF

#define HIB1P2_HIB1P2_POR_TEST_CTRL_mem_prcm_por_test_ctrl_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_HIB_TIMER_SYNC_CALIB_CFG0 register.
//
//******************************************************************************
#define HIB1P2_HIB_TIMER_SYNC_CALIB_CFG0_reserved_M \
                                0xFFFF0000

#define HIB1P2_HIB_TIMER_SYNC_CALIB_CFG0_reserved_S 16
#define HIB1P2_HIB_TIMER_SYNC_CALIB_CFG0_mem_cfg_calib_time_M \
                                0x0000FF00

#define HIB1P2_HIB_TIMER_SYNC_CALIB_CFG0_mem_cfg_calib_time_S 8
#define HIB1P2_HIB_TIMER_SYNC_CALIB_CFG0_NU1_M \
                                0x000000FE

#define HIB1P2_HIB_TIMER_SYNC_CALIB_CFG0_NU1_S 1
#define HIB1P2_HIB_TIMER_SYNC_CALIB_CFG0_mem_cfg_calib_start \
                                0x00000001

//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_HIB_TIMER_SYNC_CALIB_CFG1 register.
//
//******************************************************************************
#define HIB1P2_HIB_TIMER_SYNC_CALIB_CFG1_reserved_M \
                                0xFFF00000

#define HIB1P2_HIB_TIMER_SYNC_CALIB_CFG1_reserved_S 20
#define HIB1P2_HIB_TIMER_SYNC_CALIB_CFG1_fast_calib_count_M \
                                0x000FFFFF

#define HIB1P2_HIB_TIMER_SYNC_CALIB_CFG1_fast_calib_count_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_HIB_TIMER_SYNC_CFG2 register.
//
//******************************************************************************
#define HIB1P2_HIB_TIMER_SYNC_CFG2_reserved_M \
                                0xFFFFFE00

#define HIB1P2_HIB_TIMER_SYNC_CFG2_reserved_S 9
#define HIB1P2_HIB_TIMER_SYNC_CFG2_mem_cfg_hib_unload \
                                0x00000100

#define HIB1P2_HIB_TIMER_SYNC_CFG2_NU1_M \
                                0x000000FC

#define HIB1P2_HIB_TIMER_SYNC_CFG2_NU1_S 2
#define HIB1P2_HIB_TIMER_SYNC_CFG2_mem_cfg_tsf_adj \
                                0x00000002

#define HIB1P2_HIB_TIMER_SYNC_CFG2_mem_cfg_update_tsf \
                                0x00000001

//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_HIB_TIMER_SYNC_TSF_ADJ_VAL register.
//
//******************************************************************************
#define HIB1P2_HIB_TIMER_SYNC_TSF_ADJ_VAL_mem_tsf_adj_val_M \
                                0xFFFFFFFF

#define HIB1P2_HIB_TIMER_SYNC_TSF_ADJ_VAL_mem_tsf_adj_val_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_HIB_TIMER_RTC_GTS_TIMESTAMP_LSW register.
//
//******************************************************************************
#define HIB1P2_HIB_TIMER_RTC_GTS_TIMESTAMP_LSW_rtc_gts_timestamp_lsw_M \
                                0xFFFFFFFF

#define HIB1P2_HIB_TIMER_RTC_GTS_TIMESTAMP_LSW_rtc_gts_timestamp_lsw_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_HIB_TIMER_RTC_GTS_TIMESTAMP_MSW register.
//
//******************************************************************************
#define HIB1P2_HIB_TIMER_RTC_GTS_TIMESTAMP_MSW_reserved_M \
                                0xFFFF0000

#define HIB1P2_HIB_TIMER_RTC_GTS_TIMESTAMP_MSW_reserved_S 16
#define HIB1P2_HIB_TIMER_RTC_GTS_TIMESTAMP_MSW_rtc_gts_timestamp_msw_M \
                                0x0000FFFF

#define HIB1P2_HIB_TIMER_RTC_GTS_TIMESTAMP_MSW_rtc_gts_timestamp_msw_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_HIB_TIMER_RTC_WUP_TIMESTAMP_LSW register.
//
//******************************************************************************
#define HIB1P2_HIB_TIMER_RTC_WUP_TIMESTAMP_LSW_rtc_wup_timestamp_lsw_M \
                                0xFFFFFFFF

#define HIB1P2_HIB_TIMER_RTC_WUP_TIMESTAMP_LSW_rtc_wup_timestamp_lsw_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_HIB_TIMER_RTC_WUP_TIMESTAMP_MSW register.
//
//******************************************************************************
#define HIB1P2_HIB_TIMER_RTC_WUP_TIMESTAMP_MSW_reserved_M \
                                0xFFFF0000

#define HIB1P2_HIB_TIMER_RTC_WUP_TIMESTAMP_MSW_reserved_S 16
#define HIB1P2_HIB_TIMER_RTC_WUP_TIMESTAMP_MSW_rtc_wup_timestamp_msw_M \
                                0x0000FFFF

#define HIB1P2_HIB_TIMER_RTC_WUP_TIMESTAMP_MSW_rtc_wup_timestamp_msw_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_HIB_TIMER_SYNC_WAKE_OFFSET_ERR register.
//
//******************************************************************************
#define HIB1P2_HIB_TIMER_SYNC_WAKE_OFFSET_ERR_reserved_M \
                                0xFFFFF000

#define HIB1P2_HIB_TIMER_SYNC_WAKE_OFFSET_ERR_reserved_S 12
#define HIB1P2_HIB_TIMER_SYNC_WAKE_OFFSET_ERR_wup_offset_error_M \
                                0x00000FFF

#define HIB1P2_HIB_TIMER_SYNC_WAKE_OFFSET_ERR_wup_offset_error_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_HIB_TIMER_SYNC_TSF_CURR_VAL_LSW register.
//
//******************************************************************************
#define HIB1P2_HIB_TIMER_SYNC_TSF_CURR_VAL_LSW_tsf_curr_val_lsw_M \
                                0xFFFFFFFF

#define HIB1P2_HIB_TIMER_SYNC_TSF_CURR_VAL_LSW_tsf_curr_val_lsw_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_HIB_TIMER_SYNC_TSF_CURR_VAL_MSW register.
//
//******************************************************************************
#define HIB1P2_HIB_TIMER_SYNC_TSF_CURR_VAL_MSW_tsf_curr_val_msw_M \
                                0xFFFFFFFF

#define HIB1P2_HIB_TIMER_SYNC_TSF_CURR_VAL_MSW_tsf_curr_val_msw_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the HIB1P2_O_CM_SPARE register.
//
//******************************************************************************
#define HIB1P2_CM_SPARE_CM_SPARE_OUT_M \
                                0xFF000000

#define HIB1P2_CM_SPARE_CM_SPARE_OUT_S 24
#define HIB1P2_CM_SPARE_MEM_CM_TEST_CTRL_M \
                                0x00FF0000

#define HIB1P2_CM_SPARE_MEM_CM_TEST_CTRL_S 16
#define HIB1P2_CM_SPARE_MEM_CM_SPARE_M \
                                0x0000FFFF

#define HIB1P2_CM_SPARE_MEM_CM_SPARE_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_PORPOL_SPARE register.
//
//******************************************************************************
#define HIB1P2_PORPOL_SPARE_MEM_PORPOL_SPARE_M \
                                0xFFFFFFFF

#define HIB1P2_PORPOL_SPARE_MEM_PORPOL_SPARE_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_MEM_DIG_DCDC_CLK_CONFIG register.
//
//******************************************************************************
#define HIB1P2_MEM_DIG_DCDC_CLK_CONFIG_MEM_DIG_DCDC_CLK_ENABLE \
                                0x00000100

#define HIB1P2_MEM_DIG_DCDC_CLK_CONFIG_MEM_DIG_DCDC_CLK_PLLGEN_OFF_TIME_M \
                                0x000000F0

#define HIB1P2_MEM_DIG_DCDC_CLK_CONFIG_MEM_DIG_DCDC_CLK_PLLGEN_OFF_TIME_S 4
#define HIB1P2_MEM_DIG_DCDC_CLK_CONFIG_MEM_DIG_DCDC_CLK_PLLGEN_ON_TIME_M \
                                0x0000000F

#define HIB1P2_MEM_DIG_DCDC_CLK_CONFIG_MEM_DIG_DCDC_CLK_PLLGEN_ON_TIME_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_MEM_ANA_DCDC_CLK_CONFIG register.
//
//******************************************************************************
#define HIB1P2_MEM_ANA_DCDC_CLK_CONFIG_MEM_ANA_DCDC_CLK_ENABLE \
                                0x00000100

#define HIB1P2_MEM_ANA_DCDC_CLK_CONFIG_MEM_ANA_DCDC_CLK_PLLGEN_OFF_TIME_M \
                                0x000000F0

#define HIB1P2_MEM_ANA_DCDC_CLK_CONFIG_MEM_ANA_DCDC_CLK_PLLGEN_OFF_TIME_S 4
#define HIB1P2_MEM_ANA_DCDC_CLK_CONFIG_MEM_ANA_DCDC_CLK_PLLGEN_ON_TIME_M \
                                0x0000000F

#define HIB1P2_MEM_ANA_DCDC_CLK_CONFIG_MEM_ANA_DCDC_CLK_PLLGEN_ON_TIME_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_MEM_FLASH_DCDC_CLK_CONFIG register.
//
//******************************************************************************
#define HIB1P2_MEM_FLASH_DCDC_CLK_CONFIG_MEM_FLASH_DCDC_CLK_ENABLE \
                                0x00000100

#define HIB1P2_MEM_FLASH_DCDC_CLK_CONFIG_MEM_FLASH_DCDC_CLK_PLLGEN_OFF_TIME_M \
                                0x000000F0

#define HIB1P2_MEM_FLASH_DCDC_CLK_CONFIG_MEM_FLASH_DCDC_CLK_PLLGEN_OFF_TIME_S 4
#define HIB1P2_MEM_FLASH_DCDC_CLK_CONFIG_MEM_FLASH_DCDC_CLK_PLLGEN_ON_TIME_M \
                                0x0000000F

#define HIB1P2_MEM_FLASH_DCDC_CLK_CONFIG_MEM_FLASH_DCDC_CLK_PLLGEN_ON_TIME_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_MEM_PA_DCDC_CLK_CONFIG register.
//
//******************************************************************************
#define HIB1P2_MEM_PA_DCDC_CLK_CONFIG_MEM_PA_DCDC_CLK_ENABLE \
                                0x00000100

#define HIB1P2_MEM_PA_DCDC_CLK_CONFIG_MEM_PA_DCDC_CLK_PLLGEN_OFF_TIME_M \
                                0x000000F0

#define HIB1P2_MEM_PA_DCDC_CLK_CONFIG_MEM_PA_DCDC_CLK_PLLGEN_OFF_TIME_S 4
#define HIB1P2_MEM_PA_DCDC_CLK_CONFIG_MEM_PA_DCDC_CLK_PLLGEN_ON_TIME_M \
                                0x0000000F

#define HIB1P2_MEM_PA_DCDC_CLK_CONFIG_MEM_PA_DCDC_CLK_PLLGEN_ON_TIME_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_MEM_SLDO_VNWA_OVERRIDE register.
//
//******************************************************************************
#define HIB1P2_MEM_SLDO_VNWA_OVERRIDE_MEM_SLDO_EN_TOP_VNWA_OVERRIDE_CTRL \
                                0x00000002

#define HIB1P2_MEM_SLDO_VNWA_OVERRIDE_MEM_SLDO_EN_TOP_VNWA_OVERRIDE \
                                0x00000001

//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_MEM_BGAP_DUTY_CYCLING_ENABLE_OVERRIDE register.
//
//******************************************************************************
#define HIB1P2_MEM_BGAP_DUTY_CYCLING_ENABLE_OVERRIDE_MEM_BGAP_DUTY_CYCLING_OVERRIDE_CTRL \
                                0x00000002

#define HIB1P2_MEM_BGAP_DUTY_CYCLING_ENABLE_OVERRIDE_MEM_BGAP_DUTY_CYCLING_OVERRIDE \
                                0x00000001

//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_MEM_HIB_FSM_DEBUG register.
//
//******************************************************************************
#define HIB1P2_MEM_HIB_FSM_DEBUG_SRAM_PS_M \
                                0x00000700

#define HIB1P2_MEM_HIB_FSM_DEBUG_SRAM_PS_S 8
#define HIB1P2_MEM_HIB_FSM_DEBUG_ANA_DCDC_PS_M \
                                0x000000F0

#define HIB1P2_MEM_HIB_FSM_DEBUG_ANA_DCDC_PS_S 4
#define HIB1P2_MEM_HIB_FSM_DEBUG_DIG_DCDC_PS_M \
                                0x0000000F

#define HIB1P2_MEM_HIB_FSM_DEBUG_DIG_DCDC_PS_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_MEM_SLDO_VNWA_SW_CTRL register.
//
//******************************************************************************
#define HIB1P2_MEM_SLDO_VNWA_SW_CTRL_MEM_SLDO_VNWA_SW_CTRL_M \
                                0x000FFFFF

#define HIB1P2_MEM_SLDO_VNWA_SW_CTRL_MEM_SLDO_VNWA_SW_CTRL_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_MEM_SLDO_WEAK_PROCESS register.
//
//******************************************************************************
#define HIB1P2_MEM_SLDO_WEAK_PROCESS_MEM_SLDO_WEAK_PROCESS \
                                0x00000001

//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_MEM_PA_DCDC_OV_UV_STATUS register.
//
//******************************************************************************
#define HIB1P2_MEM_PA_DCDC_OV_UV_STATUS_dcdc_pa_ov_prot_out_lowv \
                                0x00000002

//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB1P2_O_MEM_CM_TEST_MODE register.
//
//******************************************************************************
#define HIB1P2_MEM_CM_TEST_MODE_mem_cm_test_mode \
                                0x00000001




#endif // __HW_HIB1P2_H__
