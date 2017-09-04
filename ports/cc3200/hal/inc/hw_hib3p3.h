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

#ifndef __HW_HIB3P3_H__
#define __HW_HIB3P3_H__

//*****************************************************************************
//
// The following are defines for the HIB3P3 register offsets.
//
//*****************************************************************************
#define HIB3P3_O_MEM_HIB_REQ    0x00000000
#define HIB3P3_O_MEM_HIB_RTC_TIMER_ENABLE \
                                0x00000004

#define HIB3P3_O_MEM_HIB_RTC_TIMER_RESET \
                                0x00000008

#define HIB3P3_O_MEM_HIB_RTC_TIMER_READ \
                                0x0000000C

#define HIB3P3_O_MEM_HIB_RTC_TIMER_LSW \
                                0x00000010

#define HIB3P3_O_MEM_HIB_RTC_TIMER_MSW \
                                0x00000014

#define HIB3P3_O_MEM_HIB_RTC_WAKE_EN \
                                0x00000018

#define HIB3P3_O_MEM_HIB_RTC_WAKE_LSW_CONF \
                                0x0000001C

#define HIB3P3_O_MEM_HIB_RTC_WAKE_MSW_CONF \
                                0x00000020

#define HIB3P3_O_MEM_INT_OSC_CONF \
                                0x0000002C

#define HIB3P3_O_MEM_XTAL_OSC_CONF \
                                0x00000034

#define HIB3P3_O_MEM_BGAP_PARAMETERS0 \
                                0x00000038

#define HIB3P3_O_MEM_BGAP_PARAMETERS1 \
                                0x0000003C

#define HIB3P3_O_MEM_HIB_DETECTION_STATUS \
                                0x00000040

#define HIB3P3_O_MEM_HIB_MISC_CONTROLS \
                                0x00000044

#define HIB3P3_O_MEM_HIB_CONFIG 0x00000050
#define HIB3P3_O_MEM_HIB_RTC_IRQ_ENABLE \
                                0x00000054

#define HIB3P3_O_MEM_HIB_RTC_IRQ_LSW_CONF \
                                0x00000058

#define HIB3P3_O_MEM_HIB_RTC_IRQ_MSW_CONF \
                                0x0000005C

#define HIB3P3_O_MEM_HIB_UART_CONF \
                                0x00000400

#define HIB3P3_O_MEM_GPIO_WAKE_EN \
                                0x00000404

#define HIB3P3_O_MEM_GPIO_WAKE_CONF \
                                0x00000408

#define HIB3P3_O_MEM_PAD_OEN_RET33_CONF \
                                0x0000040C

#define HIB3P3_O_MEM_UART_RTS_OEN_RET33_CONF \
                                0x00000410

#define HIB3P3_O_MEM_JTAG_CONF  0x00000414
#define HIB3P3_O_MEM_HIB_REG0   0x00000418
#define HIB3P3_O_MEM_HIB_REG1   0x0000041C
#define HIB3P3_O_MEM_HIB_REG2   0x00000420
#define HIB3P3_O_MEM_HIB_REG3   0x00000424
#define HIB3P3_O_MEM_HIB_SEQUENCER_CFG0 \
                                0x0000045C

#define HIB3P3_O_MEM_HIB_SEQUENCER_CFG1 \
                                0x00000460

#define HIB3P3_O_MEM_HIB_MISC_CONFIG \
                                0x00000464

#define HIB3P3_O_MEM_HIB_WAKE_STATUS \
                                0x00000468

#define HIB3P3_O_MEM_HIB_LPDS_GPIO_SEL \
                                0x0000046C

#define HIB3P3_O_MEM_HIB_SEQUENCER_CFG2 \
                                0x00000470

#define HIB3P3_O_HIBANA_SPARE_LOWV \
                                0x00000474

#define HIB3P3_O_HIB_TMUX_CTRL  0x00000478
#define HIB3P3_O_HIB_1P2_1P8_LDO_TRIM \
                                0x0000047C

#define HIB3P3_O_HIB_COMP_TRIM  0x00000480
#define HIB3P3_O_HIB_EN_TS      0x00000484
#define HIB3P3_O_HIB_1P8V_DET_EN \
                                0x00000488

#define HIB3P3_O_HIB_VBAT_MON_EN \
                                0x0000048C

#define HIB3P3_O_HIB_NHIB_ENABLE \
                                0x00000490

#define HIB3P3_O_HIB_UART_RTS_SW_ENABLE \
                                0x00000494




//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB3P3_O_MEM_HIB_REQ register.
//
//******************************************************************************
#define HIB3P3_MEM_HIB_REQ_reserved_M \
                                0xFFFFFE00

#define HIB3P3_MEM_HIB_REQ_reserved_S 9
#define HIB3P3_MEM_HIB_REQ_NU1_M \
                                0x000001FC

#define HIB3P3_MEM_HIB_REQ_NU1_S 2
#define HIB3P3_MEM_HIB_REQ_mem_hib_clk_disable \
                                0x00000002  // 1 - Specifies that the Hiberante
                                            // mode is without clocks ; 0 -
                                            // Specified that the Hibernate mode
                                            // is with clocks This register will
                                            // be reset during Hibernate
                                            // -WO-Clks mode (but not during
                                            // Hibernate-W-Clks mode).

#define HIB3P3_MEM_HIB_REQ_mem_hib_req \
                                0x00000001  // 1 - Request for hibernate mode
                                            // (This is an auto-clear bit) ; 0 -
                                            // Donot request for hibernate mode
                                            // This register will be reset
                                            // during Hibernate -WO-Clks mode
                                            // (but not during Hibernate-W-Clks
                                            // mode).

//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB3P3_O_MEM_HIB_RTC_TIMER_ENABLE register.
//
//******************************************************************************
#define HIB3P3_MEM_HIB_RTC_TIMER_ENABLE_reserved_M \
                                0xFFFFFFFE

#define HIB3P3_MEM_HIB_RTC_TIMER_ENABLE_reserved_S 1
#define HIB3P3_MEM_HIB_RTC_TIMER_ENABLE_mem_hib_rtc_timer_enable \
                                0x00000001  // 1 - Enable the RTC timer to
                                            // start running ; 0 - Keep the RTC
                                            // timer disabled This register will
                                            // be reset during Hibernate
                                            // -WO-Clks mode (but not during
                                            // Hibernate-W-Clks mode).

//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB3P3_O_MEM_HIB_RTC_TIMER_RESET register.
//
//******************************************************************************
#define HIB3P3_MEM_HIB_RTC_TIMER_RESET_reserved_M \
                                0xFFFFFFFE

#define HIB3P3_MEM_HIB_RTC_TIMER_RESET_reserved_S 1
#define HIB3P3_MEM_HIB_RTC_TIMER_RESET_mem_hib_rtc_timer_reset \
                                0x00000001  // 1 - Reset the RTC timer ; 0 -
                                            // Donot reset the RTC timer. This
                                            // is an auto-clear bit. This
                                            // register will be reset during
                                            // Hibernate -WO-Clks mode (but not
                                            // during Hibernate-W-Clks mode).

//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB3P3_O_MEM_HIB_RTC_TIMER_READ register.
//
//******************************************************************************
#define HIB3P3_MEM_HIB_RTC_TIMER_READ_reserved_M \
                                0xFFFFFFFE

#define HIB3P3_MEM_HIB_RTC_TIMER_READ_reserved_S 1
#define HIB3P3_MEM_HIB_RTC_TIMER_READ_mem_hib_rtc_timer_read \
                                0x00000001  // 1 - Latch the running RTC timer
                                            // into local registers. After
                                            // programming this bit to 1, the
                                            // F/w can read the latched RTC
                                            // timer values from
                                            // MEM_HIB_RTC_TIMER_LSW and
                                            // MEM_HIB_RTC_TIMER_MSW. Before the
                                            // F/w (APPS or NWP) wants to read
                                            // the RTC-Timer, it has to program
                                            // this bit to 1, then only read the
                                            // MSW and LSW values. This is an
                                            // auto-clear bit. This register
                                            // will be reset during Hibernate
                                            // -WO-Clks mode (but not during
                                            // Hibernate-W-Clks mode).

//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB3P3_O_MEM_HIB_RTC_TIMER_LSW register.
//
//******************************************************************************
#define HIB3P3_MEM_HIB_RTC_TIMER_LSW_hib_rtc_timer_lsw_M \
                                0xFFFFFFFF  // Lower 32b value of the latched
                                            // RTC-Timer.

#define HIB3P3_MEM_HIB_RTC_TIMER_LSW_hib_rtc_timer_lsw_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB3P3_O_MEM_HIB_RTC_TIMER_MSW register.
//
//******************************************************************************
#define HIB3P3_MEM_HIB_RTC_TIMER_MSW_reserved_M \
                                0xFFFF0000

#define HIB3P3_MEM_HIB_RTC_TIMER_MSW_reserved_S 16
#define HIB3P3_MEM_HIB_RTC_TIMER_MSW_hib_rtc_timer_msw_M \
                                0x0000FFFF  // Upper 32b value of the latched
                                            // RTC-Timer.

#define HIB3P3_MEM_HIB_RTC_TIMER_MSW_hib_rtc_timer_msw_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB3P3_O_MEM_HIB_RTC_WAKE_EN register.
//
//******************************************************************************
#define HIB3P3_MEM_HIB_RTC_WAKE_EN_reserved_M \
                                0xFFFFFFFE

#define HIB3P3_MEM_HIB_RTC_WAKE_EN_reserved_S 1
#define HIB3P3_MEM_HIB_RTC_WAKE_EN_mem_hib_rtc_wake_en \
                                0x00000001  // 1 - Enable the RTC timer based
                                            // wakeup during Hibernate mode ; 0
                                            // - Disable the RTC timer based
                                            // wakeup during Hibernate mode This
                                            // register will be reset during
                                            // Hibernate-WO-Clks mode (but not
                                            // during Hibernate-W-Clks mode).

//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB3P3_O_MEM_HIB_RTC_WAKE_LSW_CONF register.
//
//******************************************************************************
#define HIB3P3_MEM_HIB_RTC_WAKE_LSW_CONF_mem_hib_rtc_wake_lsw_conf_M \
                                0xFFFFFFFF  // Configuration for RTC-Timer
                                            // Wakeup (Lower 32b word)

#define HIB3P3_MEM_HIB_RTC_WAKE_LSW_CONF_mem_hib_rtc_wake_lsw_conf_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB3P3_O_MEM_HIB_RTC_WAKE_MSW_CONF register.
//
//******************************************************************************
#define HIB3P3_MEM_HIB_RTC_WAKE_MSW_CONF_reserved_M \
                                0xFFFF0000

#define HIB3P3_MEM_HIB_RTC_WAKE_MSW_CONF_reserved_S 16
#define HIB3P3_MEM_HIB_RTC_WAKE_MSW_CONF_mem_hib_rtc_wake_msw_conf_M \
                                0x0000FFFF  // Configuration for RTC-Timer
                                            // Wakeup (Upper 16b word)

#define HIB3P3_MEM_HIB_RTC_WAKE_MSW_CONF_mem_hib_rtc_wake_msw_conf_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB3P3_O_MEM_INT_OSC_CONF register.
//
//******************************************************************************
#define HIB3P3_MEM_INT_OSC_CONF_reserved_M \
                                0xFFFF0000

#define HIB3P3_MEM_INT_OSC_CONF_reserved_S 16
#define HIB3P3_MEM_INT_OSC_CONF_cm_clk_good_32k_int \
                                0x00008000  // 1 - Internal 32kHz Oscillator is
                                            // valid ; 0 - Internal 32k
                                            // oscillator clk is not valid

#define HIB3P3_MEM_INT_OSC_CONF_mem_cm_intosc_32k_spare_M \
                                0x00007E00

#define HIB3P3_MEM_INT_OSC_CONF_mem_cm_intosc_32k_spare_S 9
#define HIB3P3_MEM_INT_OSC_CONF_mem_cm_en_intosc_32k_override_ctrl \
                                0x00000100  // When 1, the INT_32K_OSC_EN comes
                                            // from bit [0] of this register,
                                            // else comes from the FSM. This
                                            // register will be reset during
                                            // Hibernate-WO-Clks mode (but not
                                            // during Hibernate-W-Clks mode)

#define HIB3P3_MEM_INT_OSC_CONF_NU1 \
                                0x00000080

#define HIB3P3_MEM_INT_OSC_CONF_mem_cm_intosc_32k_trim_M \
                                0x0000007E

#define HIB3P3_MEM_INT_OSC_CONF_mem_cm_intosc_32k_trim_S 1
#define HIB3P3_MEM_INT_OSC_CONF_mem_cm_en_intosc_32k \
                                0x00000001  // Override value for INT_OSC_EN.
                                            // Applicable only when bit [3] of
                                            // this register is set to 1.

//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB3P3_O_MEM_XTAL_OSC_CONF register.
//
//******************************************************************************
#define HIB3P3_MEM_XTAL_OSC_CONF_reserved_M \
                                0xFFF00000

#define HIB3P3_MEM_XTAL_OSC_CONF_reserved_S 20
#define HIB3P3_MEM_XTAL_OSC_CONF_mem_cm_en_sli_32k_override_ctrl \
                                0x00080000  // When 1, the SLICER_EN comes from
                                            // bit [10] of this register, else
                                            // comes from the FSM.

#define HIB3P3_MEM_XTAL_OSC_CONF_mem_cm_en_xtal_32k_override_ctrl \
                                0x00040000  // When 1, the XTAL_EN comes from
                                            // bit [0] of this register, else
                                            // comes from the FSM.

#define HIB3P3_MEM_XTAL_OSC_CONF_cm_clk_good_xtal \
                                0x00020000  // 1 - XTAL Clk is good ; 0 - XTAL
                                            // Clk is yet to be valid.

#define HIB3P3_MEM_XTAL_OSC_CONF_mem_cm_xtal_trim_M \
                                0x0001F800

#define HIB3P3_MEM_XTAL_OSC_CONF_mem_cm_xtal_trim_S 11
#define HIB3P3_MEM_XTAL_OSC_CONF_mem_cm_en_sli_32k \
                                0x00000400  // SLICER_EN Override value :
                                            // Applicable only when bit [19] of
                                            // this register is set to 1.

#define HIB3P3_MEM_XTAL_OSC_CONF_mem_cm_sli_32k_trim_M \
                                0x00000380

#define HIB3P3_MEM_XTAL_OSC_CONF_mem_cm_sli_32k_trim_S 7
#define HIB3P3_MEM_XTAL_OSC_CONF_mem_cm_fref_32k_slicer_itrim_M \
                                0x00000070

#define HIB3P3_MEM_XTAL_OSC_CONF_mem_cm_fref_32k_slicer_itrim_S 4
#define HIB3P3_MEM_XTAL_OSC_CONF_mem_cm_en_fref_32k_slicer \
                                0x00000008

#define HIB3P3_MEM_XTAL_OSC_CONF_mem_cm_en_input_sense_M \
                                0x00000006

#define HIB3P3_MEM_XTAL_OSC_CONF_mem_cm_en_input_sense_S 1
#define HIB3P3_MEM_XTAL_OSC_CONF_mem_cm_en_xtal_32k \
                                0x00000001  // XTAL_EN Override value :
                                            // Applicable only when bit [18] of
                                            // this register is set to 1.

//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB3P3_O_MEM_BGAP_PARAMETERS0 register.
//
//******************************************************************************
#define HIB3P3_MEM_BGAP_PARAMETERS0_reserved_M \
                                0xFFF80000

#define HIB3P3_MEM_BGAP_PARAMETERS0_reserved_S 19
#define HIB3P3_MEM_BGAP_PARAMETERS0_mem_en_seq \
                                0x00040000

#define HIB3P3_MEM_BGAP_PARAMETERS0_mem_vbok4bg_comp_trim_M \
                                0x0001C000

#define HIB3P3_MEM_BGAP_PARAMETERS0_mem_vbok4bg_comp_trim_S 14
#define HIB3P3_MEM_BGAP_PARAMETERS0_mem_bgap_en_vbat_ok_4bg \
                                0x00001000

#define HIB3P3_MEM_BGAP_PARAMETERS0_mem_bgap_en_vbok4bg_comp \
                                0x00000800

#define HIB3P3_MEM_BGAP_PARAMETERS0_mem_bgap_en_vbok4bg_comp_ref \
                                0x00000400

#define HIB3P3_MEM_BGAP_PARAMETERS0_mem_bgap_spare_M \
                                0x000003FF

#define HIB3P3_MEM_BGAP_PARAMETERS0_mem_bgap_spare_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB3P3_O_MEM_BGAP_PARAMETERS1 register.
//
//******************************************************************************
#define HIB3P3_MEM_BGAP_PARAMETERS1_reserved_M \
                                0xE0000000

#define HIB3P3_MEM_BGAP_PARAMETERS1_reserved_S 29
#define HIB3P3_MEM_BGAP_PARAMETERS1_mem_bgap_act_iref_itrim_M \
                                0x1F000000

#define HIB3P3_MEM_BGAP_PARAMETERS1_mem_bgap_act_iref_itrim_S 24
#define HIB3P3_MEM_BGAP_PARAMETERS1_mem_bgap_en_act_iref \
                                0x00000008

#define HIB3P3_MEM_BGAP_PARAMETERS1_mem_bgap_en_v2i \
                                0x00000004

#define HIB3P3_MEM_BGAP_PARAMETERS1_mem_bgap_en_cap_sw \
                                0x00000002

#define HIB3P3_MEM_BGAP_PARAMETERS1_mem_bgap_en \
                                0x00000001

//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB3P3_O_MEM_HIB_DETECTION_STATUS register.
//
//******************************************************************************
#define HIB3P3_MEM_HIB_DETECTION_STATUS_reserved_M \
                                0xFFFFFF80

#define HIB3P3_MEM_HIB_DETECTION_STATUS_reserved_S 7
#define HIB3P3_MEM_HIB_DETECTION_STATUS_hib_forced_ana_status \
                                0x00000040  // 1 - 1.8 V supply forced mode.

#define HIB3P3_MEM_HIB_DETECTION_STATUS_hib_forced_flash_status \
                                0x00000004  // 1 - 3.3 V supply forced mode for
                                            // Flash supply

#define HIB3P3_MEM_HIB_DETECTION_STATUS_hib_ext_clk_det_out_status \
                                0x00000002  // 1 - Forced clock mode

#define HIB3P3_MEM_HIB_DETECTION_STATUS_hib_xtal_det_out_status \
                                0x00000001  // 1 - XTAL clock mode

//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB3P3_O_MEM_HIB_MISC_CONTROLS register.
//
//******************************************************************************
#define HIB3P3_MEM_HIB_MISC_CONTROLS_reserved_M \
                                0xFFFFF800

#define HIB3P3_MEM_HIB_MISC_CONTROLS_reserved_S 11
#define HIB3P3_MEM_HIB_MISC_CONTROLS_mem_hib_en_pok_por_comp \
                                0x00000400

#define HIB3P3_MEM_HIB_MISC_CONTROLS_mem_hib_en_pok_por_comp_ref \
                                0x00000200

#define HIB3P3_MEM_HIB_MISC_CONTROLS_mem_hib_pok_por_comp_trim_M \
                                0x000001C0

#define HIB3P3_MEM_HIB_MISC_CONTROLS_mem_hib_pok_por_comp_trim_S 6
#define HIB3P3_MEM_HIB_MISC_CONTROLS_NU1 \
                                0x00000020

#define HIB3P3_MEM_HIB_MISC_CONTROLS_mem_hib_flash_det_en \
                                0x00000010

#define HIB3P3_MEM_HIB_MISC_CONTROLS_mem_hib_en_tmux \
                                0x00000001

//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB3P3_O_MEM_HIB_CONFIG register.
//
//******************************************************************************
#define HIB3P3_MEM_HIB_CONFIG_TOP_MUX_CTRL_SOP_SPIO_M \
                                0xFF000000

#define HIB3P3_MEM_HIB_CONFIG_TOP_MUX_CTRL_SOP_SPIO_S 24
#define HIB3P3_MEM_HIB_CONFIG_EN_ANA_DIG_SHARED3 \
                                0x00080000  // 1 - Enable VDD_FLASH_INDP_PAD
                                            // for digital path (SHARED4) ; 0 -
                                            // Disable VDD_FLASH_INDP_PAD for
                                            // digital path (SHARED4) ; Before
                                            // programming this bit to 1, ensure
                                            // that the device is in FORCED 3.3
                                            // supply Mode, which can be
                                            // inferred from the register :
                                            // MEM_HIB_DETECTION_STATUS : 0x0040

#define HIB3P3_MEM_HIB_CONFIG_EN_ANA_DIG_SHARED2 \
                                0x00040000  // 1 - Enable the
                                            // VDD_FB_GPIO_MUX_PAD for digital
                                            // path (SHARED3) ; 0 - Disable the
                                            // VDD_FB_GPIO_MUX_PAD for digital
                                            // path (SHARED3) ; This pin can be
                                            // used only in modes other than
                                            // SOP("111")

#define HIB3P3_MEM_HIB_CONFIG_EN_ANA_DIG_SHARED1 \
                                0x00020000  // 1 - Enable the PM_TEST_PAD for
                                            // digital GPIO path (SHARED2) ; 0 -
                                            // Disable the PM_TEST_PAD for
                                            // digital GPIO path (SHARED2) This
                                            // pin can be used for digital only
                                            // in modes other then SOP-111

#define HIB3P3_MEM_HIB_CONFIG_EN_ANA_DIG_SHARED0 \
                                0x00010000  // 1 - Enable the XTAL_N pin
                                            // digital GPIO path (SHARED1); 0 -
                                            // Disable the XTAL_N pin digital
                                            // GPIO path (SHARED1). Before
                                            // programming this bit to 1, ensure
                                            // that the device is in FORCED CLK
                                            // Mode, which can inferred from the
                                            // register :
                                            // MEM_HIB_DETECTION_STATUS :
                                            // 0x0040.

#define HIB3P3_MEM_HIB_CONFIG_mem_hib_xtal_enable \
                                0x00000100  // 1 - Enable the XTAL Clock ; 0 -
                                            // Donot enable the XTAL Clock. This
                                            // bit has to be programmed to 1 (by
                                            // APPS Devinit F/w), during exit
                                            // from OFF or Hib_wo_clks modes,
                                            // after checking if the slow_clk
                                            // mode is XTAL_CLK mode. Once
                                            // enabled the XTAL will be disabled
                                            // only after entering HIB_WO_CLKS
                                            // mode. This register will be reset
                                            // during Hibernate -WO-Clks mode
                                            // (but not during Hibernate-W-Clks
                                            // mode).

//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB3P3_O_MEM_HIB_RTC_IRQ_ENABLE register.
//
//******************************************************************************
#define HIB3P3_MEM_HIB_RTC_IRQ_ENABLE_HIB_RTC_IRQ_ENABLE \
                                0x00000001  // 1 - Enable the HIB RTC - IRQ ; 0
                                            // - Disable the HIB RTC - IRQ

//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB3P3_O_MEM_HIB_RTC_IRQ_LSW_CONF register.
//
//******************************************************************************
#define HIB3P3_MEM_HIB_RTC_IRQ_LSW_CONF_HIB_RTC_IRQ_LSW_CONF_M \
                                0xFFFFFFFF  // Configuration for LSW of the
                                            // RTC-Timestamp at which interrupt
                                            // need to be generated

#define HIB3P3_MEM_HIB_RTC_IRQ_LSW_CONF_HIB_RTC_IRQ_LSW_CONF_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB3P3_O_MEM_HIB_RTC_IRQ_MSW_CONF register.
//
//******************************************************************************
#define HIB3P3_MEM_HIB_RTC_IRQ_MSW_CONF_HIB_RTC_IRQ_MSW_CONF_M \
                                0x0000FFFF  // Configuration for MSW of thr
                                            // RTC-Timestamp at which the
                                            // interrupt need to be generated

#define HIB3P3_MEM_HIB_RTC_IRQ_MSW_CONF_HIB_RTC_IRQ_MSW_CONF_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB3P3_O_MEM_HIB_UART_CONF register.
//
//******************************************************************************
#define HIB3P3_MEM_HIB_UART_CONF_reserved_M \
                                0xFFFFFFFE

#define HIB3P3_MEM_HIB_UART_CONF_reserved_S 1
#define HIB3P3_MEM_HIB_UART_CONF_mem_hib_uart_wake_en \
                                0x00000001  // 1 - Enable the UART-Autonomous
                                            // mode wakeup during Hibernate mode
                                            // ; This is an auto-clear bit, once
                                            // programmed to 1, it will latched
                                            // into an internal register which
                                            // remain asserted until the
                                            // Hib-wakeup is initiated.

//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB3P3_O_MEM_GPIO_WAKE_EN register.
//
//******************************************************************************
#define HIB3P3_MEM_GPIO_WAKE_EN_reserved_M \
                                0xFFFFFF00

#define HIB3P3_MEM_GPIO_WAKE_EN_reserved_S 8
#define HIB3P3_MEM_GPIO_WAKE_EN_mem_gpio_wake_en_M \
                                0x000000FF  // 1 - Enable the GPIO-Autonomous
                                            // mode wakeup during Hibernate mode
                                            // ; This is an auto-clear bit, once
                                            // programmed to 1, it will latched
                                            // into an internal register which
                                            // remain asserted until the
                                            // Hib-wakeup is initiated.

#define HIB3P3_MEM_GPIO_WAKE_EN_mem_gpio_wake_en_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB3P3_O_MEM_GPIO_WAKE_CONF register.
//
//******************************************************************************
#define HIB3P3_MEM_GPIO_WAKE_CONF_reserved_M \
                                0xFFFF0000

#define HIB3P3_MEM_GPIO_WAKE_CONF_reserved_S 16
#define HIB3P3_MEM_GPIO_WAKE_CONF_mem_gpio_wake_conf_M \
                                0x0000FFFF  // Configuration to say whether the
                                            // GPIO wakeup has to happen on
                                            // Level0 or falling-edge for the
                                            // given group. “00” – Level0 “01” –
                                            // Level1 “10”- Fall-edge “11”-
                                            // Rise-edge [1:0] – Conf for GPIO0
                                            // [3:2] – Conf for GPIO1 [5:4] –
                                            // Conf for GPIO2 [7:6] – Conf for
                                            // GPIO3 [9:8] – Conf for GPIO4
                                            // [11:10] – Conf for GPIO5 [13:12]
                                            // – Conf for GPIO6

#define HIB3P3_MEM_GPIO_WAKE_CONF_mem_gpio_wake_conf_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB3P3_O_MEM_PAD_OEN_RET33_CONF register.
//
//******************************************************************************
#define HIB3P3_MEM_PAD_OEN_RET33_CONF_mem_pad_oen_ret33_override_ctrl \
                                0x00000004  // 1 - Override the OEN33 and RET33
                                            // controls of GPIOs during
                                            // SOP-Bootdebug mode ; 0 - Donot
                                            // override the OEN33 and RET33
                                            // controls of GPIOs during
                                            // SOP-Bootdebug mode

#define HIB3P3_MEM_PAD_OEN_RET33_CONF_PAD_OEN33_CONF \
                                0x00000002

#define HIB3P3_MEM_PAD_OEN_RET33_CONF_PAD_RET33_CONF \
                                0x00000001

//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB3P3_O_MEM_UART_RTS_OEN_RET33_CONF register.
//
//******************************************************************************
#define HIB3P3_MEM_UART_RTS_OEN_RET33_CONF_mem_uart_nrts_oen_ret33_override_ctrl \
                                0x00000004  // 1 - Override the OEN33 and RET33
                                            // controls of UART NRTS GPIO during
                                            // SOP-Bootdebug mode ; 0 - Donot
                                            // override the OEN33 and RET33
                                            // controls of UART NRTS GPIO during
                                            // SOP-Bootdebug mode

#define HIB3P3_MEM_UART_RTS_OEN_RET33_CONF_PAD_UART_RTS_OEN33_CONF \
                                0x00000002

#define HIB3P3_MEM_UART_RTS_OEN_RET33_CONF_PAD_UART_RTS_RET33_CONF \
                                0x00000001

//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB3P3_O_MEM_JTAG_CONF register.
//
//******************************************************************************
#define HIB3P3_MEM_JTAG_CONF_mem_jtag1_oen_ret33_override_ctrl \
                                0x00000200

#define HIB3P3_MEM_JTAG_CONF_mem_jtag0_oen_ret33_override_ctrl \
                                0x00000100

#define HIB3P3_MEM_JTAG_CONF_PAD_JTAG1_RTS_OEN33_CONF \
                                0x00000008

#define HIB3P3_MEM_JTAG_CONF_PAD_JTAG1_RTS_RET33_CONF \
                                0x00000004

#define HIB3P3_MEM_JTAG_CONF_PAD_JTAG0_RTS_OEN33_CONF \
                                0x00000002

#define HIB3P3_MEM_JTAG_CONF_PAD_JTAG0_RTS_RET33_CONF \
                                0x00000001

//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB3P3_O_MEM_HIB_REG0 register.
//
//******************************************************************************
#define HIB3P3_MEM_HIB_REG0_mem_hib_reg0_M \
                                0xFFFFFFFF

#define HIB3P3_MEM_HIB_REG0_mem_hib_reg0_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB3P3_O_MEM_HIB_REG1 register.
//
//******************************************************************************
#define HIB3P3_MEM_HIB_REG1_mem_hib_reg1_M \
                                0xFFFFFFFF

#define HIB3P3_MEM_HIB_REG1_mem_hib_reg1_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB3P3_O_MEM_HIB_REG2 register.
//
//******************************************************************************
#define HIB3P3_MEM_HIB_REG2_mem_hib_reg2_M \
                                0xFFFFFFFF

#define HIB3P3_MEM_HIB_REG2_mem_hib_reg2_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB3P3_O_MEM_HIB_REG3 register.
//
//******************************************************************************
#define HIB3P3_MEM_HIB_REG3_mem_hib_reg3_M \
                                0xFFFFFFFF

#define HIB3P3_MEM_HIB_REG3_mem_hib_reg3_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB3P3_O_MEM_HIB_SEQUENCER_CFG0 register.
//
//******************************************************************************
#define HIB3P3_MEM_HIB_SEQUENCER_CFG0_mem_bdc_ev0_to_ev1_time_M \
                                0xFFFF0000  // Configuration for the number of
                                            // slow-clks between de-assertion of
                                            // EN_BG_3P3V to assertion of
                                            // EN_BG_3P3V

#define HIB3P3_MEM_HIB_SEQUENCER_CFG0_mem_bdc_ev0_to_ev1_time_S 16
#define HIB3P3_MEM_HIB_SEQUENCER_CFG0_NU1 \
                                0x00008000

#define HIB3P3_MEM_HIB_SEQUENCER_CFG0_mem_bdc_ev3_to_ev4_time_M \
                                0x00006000  // Configuration for the number of
                                            // slow-clks between assertion of
                                            // EN_COMP_3P3V and assertion of
                                            // EN_COMP_LATCH_3P3V

#define HIB3P3_MEM_HIB_SEQUENCER_CFG0_mem_bdc_ev3_to_ev4_time_S 13
#define HIB3P3_MEM_HIB_SEQUENCER_CFG0_mem_bdc_ev2_to_ev3_time_M \
                                0x00001800  // Configuration for the number of
                                            // slow-clks between assertion of
                                            // (EN_CAP_SW_3P3V,EN_COMP_REF) and
                                            // assertion of (EN_COMP_3P3V)

#define HIB3P3_MEM_HIB_SEQUENCER_CFG0_mem_bdc_ev2_to_ev3_time_S 11
#define HIB3P3_MEM_HIB_SEQUENCER_CFG0_mem_bdc_ev1_to_ev2_time_M \
                                0x00000600  // Configuration for the number of
                                            // slow-clks between assertion of
                                            // (EN_BG_3P3V) and assertion of
                                            // (EN_CAP_SW_3P3V,
                                            // EN_COMP_REF_3P3V)

#define HIB3P3_MEM_HIB_SEQUENCER_CFG0_mem_bdc_ev1_to_ev2_time_S 9
#define HIB3P3_MEM_HIB_SEQUENCER_CFG0_mem_en_crude_ref_comp \
                                0x00000100

#define HIB3P3_MEM_HIB_SEQUENCER_CFG0_mem_en_vbok4bg_ref_override_ctrl \
                                0x00000080  // 1 - EN_VBOK4BG_REF comes from
                                            // bit[10] of the register
                                            // MEM_BGAP_PARAMETERS0 [0x0038]. 0
                                            // - EN_VBOK4BG_REF comes directly
                                            // from the Hib-Sequencer.

#define HIB3P3_MEM_HIB_SEQUENCER_CFG0_mem_en_vbok4bg_comp_override_ctrl \
                                0x00000040  // 1 - EN_VBOK4BG comes from
                                            // bit[11] of the register
                                            // MEM_BGAP_PARAMETERS0 [0x0038]. 0
                                            // - EN_VBOK4BG comes directly from
                                            // the Hib-Sequencer.

#define HIB3P3_MEM_HIB_SEQUENCER_CFG0_mem_en_v2i_override_ctrl \
                                0x00000020  // 1 - EN_V2I comes from bit[2] of
                                            // the register MEM_BGAP_PARAMETERS1
                                            // [0x003C]. 0 - EN_V2I comes
                                            // directly from the Hib-Sequencer.

#define HIB3P3_MEM_HIB_SEQUENCER_CFG0_mem_por_comp_ref_override_ctrl \
                                0x00000010  // 1 - EN_POR_COMP_REF comes from
                                            // bit[9] of the register
                                            // MEM_HIB_MISC_CONTROLS [0x0044]. 0
                                            // - EN_POR_COMP_REF comes directly
                                            // from the Hib-Sequencer.

#define HIB3P3_MEM_HIB_SEQUENCER_CFG0_mem_en_por_comp_override_ctrl \
                                0x00000008  // 1 - EN_POR_COMP comes from
                                            // bit[10] of the register
                                            // MEM_HIB_MISC_CONTROLS [0x044]. 0
                                            // - EN_POR_COMP comes directly from
                                            // the Hib-Sequencer.

#define HIB3P3_MEM_HIB_SEQUENCER_CFG0_mem_cap_sw_override_ctrl \
                                0x00000004  // 1 - EN_CAP_SW comes from bit[1]
                                            // of the register
                                            // MEM_BGAP_PARAMETERS1 [0x003C]. 0
                                            // - EN_CAP_SW comes directly from
                                            // Hib-Sequencer.

#define HIB3P3_MEM_HIB_SEQUENCER_CFG0_mem_bg_override_ctrl \
                                0x00000002  // 1 - EN_BGAP comes from bit[0] of
                                            // the register MEM_BGAP_PARAMETERS1
                                            // [0x003C]. 0 - EN_BGAP comes
                                            // directly from Hib-Sequencer.

#define HIB3P3_MEM_HIB_SEQUENCER_CFG0_mem_act_iref_override_ctrl \
                                0x00000001

//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB3P3_O_MEM_HIB_SEQUENCER_CFG1 register.
//
//******************************************************************************
#define HIB3P3_MEM_HIB_SEQUENCER_CFG1_reserved_M \
                                0xFFFF0000

#define HIB3P3_MEM_HIB_SEQUENCER_CFG1_reserved_S 16
#define HIB3P3_MEM_HIB_SEQUENCER_CFG1_mem_bdc_ev5_to_ev6_time_M \
                                0x0000C000  // Configuration for number of
                                            // slow-clks between de-assertion of
                                            // EN_COMP_LATCH and assertion of

#define HIB3P3_MEM_HIB_SEQUENCER_CFG1_mem_bdc_ev5_to_ev6_time_S 14
#define HIB3P3_MEM_HIB_SEQUENCER_CFG1_mem_bdc_to_active_ev1_to_ev2_time_M \
                                0x00003000  // Configuration for number of
                                            // slow-clks between assertion of
                                            // EN_COMP_REF to assertion of
                                            // EN_COMP during HIB-Exit

#define HIB3P3_MEM_HIB_SEQUENCER_CFG1_mem_bdc_to_active_ev1_to_ev2_time_S 12
#define HIB3P3_MEM_HIB_SEQUENCER_CFG1_mem_bdc_to_active_ev0_to_ev1_time_M \
                                0x00000C00  // TBD

#define HIB3P3_MEM_HIB_SEQUENCER_CFG1_mem_bdc_to_active_ev0_to_ev1_time_S 10
#define HIB3P3_MEM_HIB_SEQUENCER_CFG1_mem_bdc_to_active_ev0_to_active_M \
                                0x00000300  // Configuration in number of
                                            // slow-clks between assertion of
                                            // (EN_BGAP_3P3V, EN_CAP_SW_3P3V,
                                            // EN_ACT_IREF_3P3V, EN_COMP_REF) to
                                            // assertion of EN_COMP_3P3V

#define HIB3P3_MEM_HIB_SEQUENCER_CFG1_mem_bdc_to_active_ev0_to_active_S 8
#define HIB3P3_MEM_HIB_SEQUENCER_CFG1_mem_active_to_bdc_ev1_to_bdc_ev0_time_M \
                                0x000000C0  // Configuration in number of
                                            // slow-clks between de-assertion of
                                            // (EN_COMP_3P3V, EN_COMP_REF_3P3V,
                                            // EN_ACT_IREF_3P3V, EN_CAP_SW_3P3V)
                                            // to deassertion of EN_BGAP_3P3V.

#define HIB3P3_MEM_HIB_SEQUENCER_CFG1_mem_active_to_bdc_ev1_to_bdc_ev0_time_S 6
#define HIB3P3_MEM_HIB_SEQUENCER_CFG1_NU1_M \
                                0x0000003F

#define HIB3P3_MEM_HIB_SEQUENCER_CFG1_NU1_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB3P3_O_MEM_HIB_MISC_CONFIG register.
//
//******************************************************************************
#define HIB3P3_MEM_HIB_MISC_CONFIG_mem_en_pll_untrim_current \
                                0x00000001

//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB3P3_O_MEM_HIB_WAKE_STATUS register.
//
//******************************************************************************
#define HIB3P3_MEM_HIB_WAKE_STATUS_hib_wake_src_M \
                                0x0000001E  // "0100" - GPIO ; "0010" - RTC ;
                                            // "0001" - UART Others - Reserved

#define HIB3P3_MEM_HIB_WAKE_STATUS_hib_wake_src_S 1
#define HIB3P3_MEM_HIB_WAKE_STATUS_hib_wake_status \
                                0x00000001  // 1 - Wake from Hibernate ; 0 -
                                            // Wake from OFF

//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB3P3_O_MEM_HIB_LPDS_GPIO_SEL register.
//
//******************************************************************************
#define HIB3P3_MEM_HIB_LPDS_GPIO_SEL_HIB_LPDS_GPIO_SEL_M \
                                0x00000007

#define HIB3P3_MEM_HIB_LPDS_GPIO_SEL_HIB_LPDS_GPIO_SEL_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB3P3_O_MEM_HIB_SEQUENCER_CFG2 register.
//
//******************************************************************************
#define HIB3P3_MEM_HIB_SEQUENCER_CFG2_reserved_M \
                                0xFFFFF800

#define HIB3P3_MEM_HIB_SEQUENCER_CFG2_reserved_S 11
#define HIB3P3_MEM_HIB_SEQUENCER_CFG2_mem_active_to_bdc_ev0_to_active_to_bdc_ev1_time_M \
                                0x00000600  // Deassertion of EN_COMP_LATCH_3P3
                                            // to deassertion of (EN_COMP_3P3,
                                            // EN_COMP_REF_3P3, EN_ACT_IREF_3P3,
                                            // EN_CAP_SW_3P3)

#define HIB3P3_MEM_HIB_SEQUENCER_CFG2_mem_active_to_bdc_ev0_to_active_to_bdc_ev1_time_S 9
#define HIB3P3_MEM_HIB_SEQUENCER_CFG2_mem_bdc_ev4_to_ev5_time_M \
                                0x000001C0  // Assertion of EN_COMP_LATCH_3P3
                                            // to deassertion of
                                            // EN_COMP_LATCH_3P3

#define HIB3P3_MEM_HIB_SEQUENCER_CFG2_mem_bdc_ev4_to_ev5_time_S 6
#define HIB3P3_MEM_HIB_SEQUENCER_CFG2_mem_bdc_ev6_to_ev7_time_M \
                                0x00000030  // Deassertion of (EN_CAP_SW_3P3,
                                            // EN_COMP_REF_3P3, EN_COMP_3P3,
                                            // EN_COMP_OUT_LATCH_3P3) to
                                            // deassertion of EN_BGAP_3P3

#define HIB3P3_MEM_HIB_SEQUENCER_CFG2_mem_bdc_ev6_to_ev7_time_S 4
#define HIB3P3_MEM_HIB_SEQUENCER_CFG2_mem_bdc_to_active_ev1_to_ev2_time_M \
                                0x0000000C  // Assertion of EN_COMP_3P3 to
                                            // assertion of EN_COMPOUT_LATCH_3P3

#define HIB3P3_MEM_HIB_SEQUENCER_CFG2_mem_bdc_to_active_ev1_to_ev2_time_S 2
#define HIB3P3_MEM_HIB_SEQUENCER_CFG2_mem_hib_to_active_ev2_to_ev3_time_M \
                                0x00000003  // Assertion of EN_COMP_3P3 to
                                            // assertion of EN_COMPOUT_LATCH_3P3

#define HIB3P3_MEM_HIB_SEQUENCER_CFG2_mem_hib_to_active_ev2_to_ev3_time_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB3P3_O_HIBANA_SPARE_LOWV register.
//
//******************************************************************************
#define HIB3P3_HIBANA_SPARE_LOWV_mem_hibana_spare1_M \
                                0xFFC00000

#define HIB3P3_HIBANA_SPARE_LOWV_mem_hibana_spare1_S 22
#define HIB3P3_HIBANA_SPARE_LOWV_mem_hibana_spare0_M \
                                0x0001FFFF

#define HIB3P3_HIBANA_SPARE_LOWV_mem_hibana_spare0_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB3P3_O_HIB_TMUX_CTRL register.
//
//******************************************************************************
#define HIB3P3_HIB_TMUX_CTRL_reserved_M \
                                0xFFFFFC00

#define HIB3P3_HIB_TMUX_CTRL_reserved_S 10
#define HIB3P3_HIB_TMUX_CTRL_mem_hd_tmux_cntrl_M \
                                0x000003FF

#define HIB3P3_HIB_TMUX_CTRL_mem_hd_tmux_cntrl_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB3P3_O_HIB_1P2_1P8_LDO_TRIM register.
//
//******************************************************************************
#define HIB3P3_HIB_1P2_1P8_LDO_TRIM_reserved_M \
                                0xFFFFF000

#define HIB3P3_HIB_1P2_1P8_LDO_TRIM_reserved_S 12
#define HIB3P3_HIB_1P2_1P8_LDO_TRIM_mem_hd_1p2_ldo_en_override_ctrl \
                                0x00000800

#define HIB3P3_HIB_1P2_1P8_LDO_TRIM_mem_hd_1p8_ldo_en_override_ctrl \
                                0x00000400

#define HIB3P3_HIB_1P2_1P8_LDO_TRIM_mem_hd_1p2_ldo_en_override \
                                0x00000200

#define HIB3P3_HIB_1P2_1P8_LDO_TRIM_mem_hd_1p8_ldo_en_override \
                                0x00000100

#define HIB3P3_HIB_1P2_1P8_LDO_TRIM_mem_hd_1p2_ldo_vtrim_M \
                                0x000000F0

#define HIB3P3_HIB_1P2_1P8_LDO_TRIM_mem_hd_1p2_ldo_vtrim_S 4
#define HIB3P3_HIB_1P2_1P8_LDO_TRIM_mem_hd_1p8_ldo_vtrim_M \
                                0x0000000F

#define HIB3P3_HIB_1P2_1P8_LDO_TRIM_mem_hd_1p8_ldo_vtrim_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB3P3_O_HIB_COMP_TRIM register.
//
//******************************************************************************
#define HIB3P3_HIB_COMP_TRIM_reserved_M \
                                0xFFFFFFF8

#define HIB3P3_HIB_COMP_TRIM_reserved_S 3
#define HIB3P3_HIB_COMP_TRIM_mem_hd_comp_trim_M \
                                0x00000007

#define HIB3P3_HIB_COMP_TRIM_mem_hd_comp_trim_S 0
//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB3P3_O_HIB_EN_TS register.
//
//******************************************************************************
#define HIB3P3_HIB_EN_TS_reserved_M \
                                0xFFFFFFFE

#define HIB3P3_HIB_EN_TS_reserved_S 1
#define HIB3P3_HIB_EN_TS_mem_hd_en_ts \
                                0x00000001

//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB3P3_O_HIB_1P8V_DET_EN register.
//
//******************************************************************************
#define HIB3P3_HIB_1P8V_DET_EN_reserved_M \
                                0xFFFFFFFE

#define HIB3P3_HIB_1P8V_DET_EN_reserved_S 1
#define HIB3P3_HIB_1P8V_DET_EN_mem_hib_1p8v_det_en \
                                0x00000001

//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB3P3_O_HIB_VBAT_MON_EN register.
//
//******************************************************************************
#define HIB3P3_HIB_VBAT_MON_EN_reserved_M \
                                0xFFFFFFFC

#define HIB3P3_HIB_VBAT_MON_EN_reserved_S 2
#define HIB3P3_HIB_VBAT_MON_EN_mem_hib_vbat_mon_del_en \
                                0x00000002

#define HIB3P3_HIB_VBAT_MON_EN_mem_hib_vbat_mon_en \
                                0x00000001

//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB3P3_O_HIB_NHIB_ENABLE register.
//
//******************************************************************************
#define HIB3P3_HIB_NHIB_ENABLE_mem_hib_nhib_enable \
                                0x00000001

//******************************************************************************
//
// The following are defines for the bit fields in the
// HIB3P3_O_HIB_UART_RTS_SW_ENABLE register.
//
//******************************************************************************
#define HIB3P3_HIB_UART_RTS_SW_ENABLE_mem_hib_uart_rts_sw_enable \
                                0x00000001




#endif // __HW_HIB3P3_H__
