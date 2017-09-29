/**
 * \file
 *
 * \brief gcc starttup file for SAMD51
 *
 * Copyright (c) 2017 Microchip Technology Inc.
 *
 * \asf_license_start
 *
 * \page License
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the Licence at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * \asf_license_stop
 *
 */

#include "samd51.h"

/* Initialize segments */
extern uint32_t _sfixed;
extern uint32_t _efixed;
extern uint32_t _etext;
extern uint32_t _srelocate;
extern uint32_t _erelocate;
extern uint32_t _szero;
extern uint32_t _ezero;
extern uint32_t _sstack;
extern uint32_t _estack;

/** \cond DOXYGEN_SHOULD_SKIP_THIS */
int main(void);
/** \endcond */

void __libc_init_array(void);

/* Default empty handler */
void Dummy_Handler(void);

/* Cortex-M4 core handlers */
void NMI_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void HardFault_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void MemManage_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void BusFault_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void UsageFault_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void SVC_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void DebugMon_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void PendSV_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void SysTick_Handler(void) __attribute__((weak, alias("Dummy_Handler")));

/* Peripherals handlers */
void PM_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void MCLK_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void OSCCTRL_0_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* OSCCTRL_XOSCFAIL_0, OSCCTRL_XOSCRDY_0 */
void OSCCTRL_1_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* OSCCTRL_XOSCFAIL_1, OSCCTRL_XOSCRDY_1 */
void OSCCTRL_2_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* OSCCTRL_DFLLLOCKC, OSCCTRL_DFLLLOCKF,
                                                                               OSCCTRL_DFLLOOB, OSCCTRL_DFLLRCS,
                                                                               OSCCTRL_DFLLRDY */
void OSCCTRL_3_Handler(void) __attribute__((
    weak, alias("Dummy_Handler"))); /* OSCCTRL_DPLLLCKF_0, OSCCTRL_DPLLLCKR_0, OSCCTRL_DPLLLDRTO_0, OSCCTRL_DPLLLTO_0 */
void OSCCTRL_4_Handler(void) __attribute__((
    weak, alias("Dummy_Handler"))); /* OSCCTRL_DPLLLCKF_1, OSCCTRL_DPLLLCKR_1, OSCCTRL_DPLLLDRTO_1, OSCCTRL_DPLLLTO_1 */
void OSC32KCTRL_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void SUPC_0_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* SUPC_B12SRDY, SUPC_B33SRDY, SUPC_BOD12RDY,
                                                                            SUPC_BOD33RDY, SUPC_VCORERDY, SUPC_VREGRDY
                                                                            */
void SUPC_1_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* SUPC_BOD12DET, SUPC_BOD33DET */
void WDT_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void RTC_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void EIC_0_Handler(void) __attribute__((weak, alias("Dummy_Handler")));  /* EIC_EXTINT_0 */
void EIC_1_Handler(void) __attribute__((weak, alias("Dummy_Handler")));  /* EIC_EXTINT_1 */
void EIC_2_Handler(void) __attribute__((weak, alias("Dummy_Handler")));  /* EIC_EXTINT_2 */
void EIC_3_Handler(void) __attribute__((weak, alias("Dummy_Handler")));  /* EIC_EXTINT_3 */
void EIC_4_Handler(void) __attribute__((weak, alias("Dummy_Handler")));  /* EIC_EXTINT_4 */
void EIC_5_Handler(void) __attribute__((weak, alias("Dummy_Handler")));  /* EIC_EXTINT_5 */
void EIC_6_Handler(void) __attribute__((weak, alias("Dummy_Handler")));  /* EIC_EXTINT_6 */
void EIC_7_Handler(void) __attribute__((weak, alias("Dummy_Handler")));  /* EIC_EXTINT_7 */
void EIC_8_Handler(void) __attribute__((weak, alias("Dummy_Handler")));  /* EIC_EXTINT_8 */
void EIC_9_Handler(void) __attribute__((weak, alias("Dummy_Handler")));  /* EIC_EXTINT_9 */
void EIC_10_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* EIC_EXTINT_10 */
void EIC_11_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* EIC_EXTINT_11 */
void EIC_12_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* EIC_EXTINT_12 */
void EIC_13_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* EIC_EXTINT_13 */
void EIC_14_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* EIC_EXTINT_14 */
void EIC_15_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* EIC_EXTINT_15 */
void FREQM_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void NVMCTRL_0_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* NVMCTRL_0, NVMCTRL_1, NVMCTRL_2,
                                                                               NVMCTRL_3, NVMCTRL_4, NVMCTRL_5,
                                                                               NVMCTRL_6, NVMCTRL_7 */
void NVMCTRL_1_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* NVMCTRL_10, NVMCTRL_8, NVMCTRL_9 */
void DMAC_0_Handler(void) __attribute__((weak, alias("Dummy_Handler")));    /* DMAC_SUSP_0, DMAC_TCMPL_0, DMAC_TERR_0 */
void DMAC_1_Handler(void) __attribute__((weak, alias("Dummy_Handler")));    /* DMAC_SUSP_1, DMAC_TCMPL_1, DMAC_TERR_1 */
void DMAC_2_Handler(void) __attribute__((weak, alias("Dummy_Handler")));    /* DMAC_SUSP_2, DMAC_TCMPL_2, DMAC_TERR_2 */
void DMAC_3_Handler(void) __attribute__((weak, alias("Dummy_Handler")));    /* DMAC_SUSP_3, DMAC_TCMPL_3, DMAC_TERR_3 */
void DMAC_4_Handler(void)
    __attribute__((weak,
                   alias("Dummy_Handler"))); /* DMAC_SUSP_10, DMAC_SUSP_11, DMAC_SUSP_12, DMAC_SUSP_13, DMAC_SUSP_14,
                                                DMAC_SUSP_15, DMAC_SUSP_16, DMAC_SUSP_17, DMAC_SUSP_18, DMAC_SUSP_19,
                                                DMAC_SUSP_20, DMAC_SUSP_21, DMAC_SUSP_22, DMAC_SUSP_23, DMAC_SUSP_24,
                                                DMAC_SUSP_25, DMAC_SUSP_26, DMAC_SUSP_27, DMAC_SUSP_28, DMAC_SUSP_29,
                                                DMAC_SUSP_30, DMAC_SUSP_31, DMAC_SUSP_4, DMAC_SUSP_5, DMAC_SUSP_6,
                                                DMAC_SUSP_7, DMAC_SUSP_8, DMAC_SUSP_9, DMAC_TCMPL_10, DMAC_TCMPL_11,
                                                DMAC_TCMPL_12, DMAC_TCMPL_13, DMAC_TCMPL_14, DMAC_TCMPL_15,
                                                DMAC_TCMPL_16, DMAC_TCMPL_17, DMAC_TCMPL_18, DMAC_TCMPL_19,
                                                DMAC_TCMPL_20, DMAC_TCMPL_21, DMAC_TCMPL_22, DMAC_TCMPL_23,
                                                DMAC_TCMPL_24, DMAC_TCMPL_25, DMAC_TCMPL_26, DMAC_TCMPL_27,
                                                DMAC_TCMPL_28, DMAC_TCMPL_29, DMAC_TCMPL_30, DMAC_TCMPL_31,
                                                DMAC_TCMPL_4, DMAC_TCMPL_5, DMAC_TCMPL_6, DMAC_TCMPL_7, DMAC_TCMPL_8,
                                                DMAC_TCMPL_9, DMAC_TERR_10, DMAC_TERR_11, DMAC_TERR_12, DMAC_TERR_13,
                                                DMAC_TERR_14, DMAC_TERR_15, DMAC_TERR_16, DMAC_TERR_17, DMAC_TERR_18,
                                                DMAC_TERR_19, DMAC_TERR_20, DMAC_TERR_21, DMAC_TERR_22, DMAC_TERR_23,
                                                DMAC_TERR_24, DMAC_TERR_25, DMAC_TERR_26, DMAC_TERR_27, DMAC_TERR_28,
                                                DMAC_TERR_29, DMAC_TERR_30, DMAC_TERR_31, DMAC_TERR_4, DMAC_TERR_5,
                                                DMAC_TERR_6, DMAC_TERR_7, DMAC_TERR_8, DMAC_TERR_9 */
void EVSYS_0_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* EVSYS_EVD_0, EVSYS_OVR_0 */
void EVSYS_1_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* EVSYS_EVD_1, EVSYS_OVR_1 */
void EVSYS_2_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* EVSYS_EVD_2, EVSYS_OVR_2 */
void EVSYS_3_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* EVSYS_EVD_3, EVSYS_OVR_3 */
void EVSYS_4_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* EVSYS_EVD_10, EVSYS_EVD_11, EVSYS_EVD_4,
                                                                             EVSYS_EVD_5, EVSYS_EVD_6, EVSYS_EVD_7,
                                                                             EVSYS_EVD_8, EVSYS_EVD_9, EVSYS_OVR_10,
                                                                             EVSYS_OVR_11, EVSYS_OVR_4, EVSYS_OVR_5,
                                                                             EVSYS_OVR_6, EVSYS_OVR_7, EVSYS_OVR_8,
                                                                             EVSYS_OVR_9 */
void PAC_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void TAL_0_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* TAL_BRK */
void TAL_1_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* TAL_IPS_0, TAL_IPS_1 */
void RAMECC_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void SERCOM0_0_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* SERCOM0_0 */
void SERCOM0_1_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* SERCOM0_1 */
void SERCOM0_2_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* SERCOM0_2 */
void SERCOM0_3_Handler(void)
    __attribute__((weak, alias("Dummy_Handler"))); /* SERCOM0_3, SERCOM0_4, SERCOM0_5, SERCOM0_6 */
void SERCOM1_0_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* SERCOM1_0 */
void SERCOM1_1_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* SERCOM1_1 */
void SERCOM1_2_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* SERCOM1_2 */
void SERCOM1_3_Handler(void)
    __attribute__((weak, alias("Dummy_Handler"))); /* SERCOM1_3, SERCOM1_4, SERCOM1_5, SERCOM1_6 */
void SERCOM2_0_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* SERCOM2_0 */
void SERCOM2_1_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* SERCOM2_1 */
void SERCOM2_2_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* SERCOM2_2 */
void SERCOM2_3_Handler(void)
    __attribute__((weak, alias("Dummy_Handler"))); /* SERCOM2_3, SERCOM2_4, SERCOM2_5, SERCOM2_6 */
void SERCOM3_0_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* SERCOM3_0 */
void SERCOM3_1_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* SERCOM3_1 */
void SERCOM3_2_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* SERCOM3_2 */
void SERCOM3_3_Handler(void)
    __attribute__((weak, alias("Dummy_Handler"))); /* SERCOM3_3, SERCOM3_4, SERCOM3_5, SERCOM3_6 */
#ifdef ID_SERCOM4
void SERCOM4_0_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* SERCOM4_0 */
void SERCOM4_1_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* SERCOM4_1 */
void SERCOM4_2_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* SERCOM4_2 */
void SERCOM4_3_Handler(void)
    __attribute__((weak, alias("Dummy_Handler"))); /* SERCOM4_3, SERCOM4_4, SERCOM4_5, SERCOM4_6 */
#endif
#ifdef ID_SERCOM5
void SERCOM5_0_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* SERCOM5_0 */
void SERCOM5_1_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* SERCOM5_1 */
void SERCOM5_2_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* SERCOM5_2 */
void SERCOM5_3_Handler(void)
    __attribute__((weak, alias("Dummy_Handler"))); /* SERCOM5_3, SERCOM5_4, SERCOM5_5, SERCOM5_6 */
#endif
#ifdef ID_SERCOM6
void SERCOM6_0_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* SERCOM6_0 */
void SERCOM6_1_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* SERCOM6_1 */
void SERCOM6_2_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* SERCOM6_2 */
void SERCOM6_3_Handler(void)
    __attribute__((weak, alias("Dummy_Handler"))); /* SERCOM6_3, SERCOM6_4, SERCOM6_5, SERCOM6_6 */
#endif
#ifdef ID_SERCOM7
void SERCOM7_0_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* SERCOM7_0 */
void SERCOM7_1_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* SERCOM7_1 */
void SERCOM7_2_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* SERCOM7_2 */
void SERCOM7_3_Handler(void)
    __attribute__((weak, alias("Dummy_Handler"))); /* SERCOM7_3, SERCOM7_4, SERCOM7_5, SERCOM7_6 */
#endif
#ifdef ID_CAN0
void CAN0_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
#endif
#ifdef ID_CAN1
void CAN1_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
#endif
#ifdef ID_USB
void USB_0_Handler(void)
    __attribute__((weak,
                   alias("Dummy_Handler"))); /* USB_EORSM_DNRSM, USB_EORST_RST, USB_LPMSUSP_DDISC, USB_LPM_DCONN,
                                                USB_MSOF, USB_RAMACER, USB_RXSTP_TXSTP_0, USB_RXSTP_TXSTP_1,
                                                USB_RXSTP_TXSTP_2, USB_RXSTP_TXSTP_3, USB_RXSTP_TXSTP_4,
                                                USB_RXSTP_TXSTP_5, USB_RXSTP_TXSTP_6, USB_RXSTP_TXSTP_7,
                                                USB_STALL0_STALL_0, USB_STALL0_STALL_1, USB_STALL0_STALL_2,
                                                USB_STALL0_STALL_3, USB_STALL0_STALL_4, USB_STALL0_STALL_5,
                                                USB_STALL0_STALL_6, USB_STALL0_STALL_7, USB_STALL1_0, USB_STALL1_1,
                                                USB_STALL1_2, USB_STALL1_3, USB_STALL1_4, USB_STALL1_5, USB_STALL1_6,
                                                USB_STALL1_7, USB_SUSPEND, USB_TRFAIL0_TRFAIL_0, USB_TRFAIL0_TRFAIL_1,
                                                USB_TRFAIL0_TRFAIL_2, USB_TRFAIL0_TRFAIL_3, USB_TRFAIL0_TRFAIL_4,
                                                USB_TRFAIL0_TRFAIL_5, USB_TRFAIL0_TRFAIL_6, USB_TRFAIL0_TRFAIL_7,
                                                USB_TRFAIL1_PERR_0, USB_TRFAIL1_PERR_1, USB_TRFAIL1_PERR_2,
                                                USB_TRFAIL1_PERR_3, USB_TRFAIL1_PERR_4, USB_TRFAIL1_PERR_5,
                                                USB_TRFAIL1_PERR_6, USB_TRFAIL1_PERR_7, USB_UPRSM, USB_WAKEUP */
void USB_1_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* USB_SOF_HSOF */
void USB_2_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* USB_TRCPT0_0, USB_TRCPT0_1, USB_TRCPT0_2,
                                                                           USB_TRCPT0_3, USB_TRCPT0_4, USB_TRCPT0_5,
                                                                           USB_TRCPT0_6, USB_TRCPT0_7 */
void USB_3_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* USB_TRCPT1_0, USB_TRCPT1_1, USB_TRCPT1_2,
                                                                           USB_TRCPT1_3, USB_TRCPT1_4, USB_TRCPT1_5,
                                                                           USB_TRCPT1_6, USB_TRCPT1_7 */
#endif
#ifdef ID_GMAC
void GMAC_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
#endif
void TCC0_0_Handler(void)
    __attribute__((weak,
                   alias("Dummy_Handler"))); /* TCC0_CNT_A, TCC0_DFS_A, TCC0_ERR_A, TCC0_FAULT0_A, TCC0_FAULT1_A,
                                                TCC0_FAULTA_A, TCC0_FAULTB_A, TCC0_OVF, TCC0_TRG, TCC0_UFS_A */
void TCC0_1_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* TCC0_MC_0 */
void TCC0_2_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* TCC0_MC_1 */
void TCC0_3_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* TCC0_MC_2 */
void TCC0_4_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* TCC0_MC_3 */
void TCC0_5_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* TCC0_MC_4 */
void TCC0_6_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* TCC0_MC_5 */
void TCC1_0_Handler(void)
    __attribute__((weak,
                   alias("Dummy_Handler"))); /* TCC1_CNT_A, TCC1_DFS_A, TCC1_ERR_A, TCC1_FAULT0_A, TCC1_FAULT1_A,
                                                TCC1_FAULTA_A, TCC1_FAULTB_A, TCC1_OVF, TCC1_TRG, TCC1_UFS_A */
void TCC1_1_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* TCC1_MC_0 */
void TCC1_2_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* TCC1_MC_1 */
void TCC1_3_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* TCC1_MC_2 */
void TCC1_4_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* TCC1_MC_3 */
void TCC2_0_Handler(void)
    __attribute__((weak,
                   alias("Dummy_Handler"))); /* TCC2_CNT_A, TCC2_DFS_A, TCC2_ERR_A, TCC2_FAULT0_A, TCC2_FAULT1_A,
                                                TCC2_FAULTA_A, TCC2_FAULTB_A, TCC2_OVF, TCC2_TRG, TCC2_UFS_A */
void TCC2_1_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* TCC2_MC_0 */
void TCC2_2_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* TCC2_MC_1 */
void TCC2_3_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* TCC2_MC_2 */
#ifdef ID_TCC3
void TCC3_0_Handler(void)
    __attribute__((weak,
                   alias("Dummy_Handler"))); /* TCC3_CNT_A, TCC3_DFS_A, TCC3_ERR_A, TCC3_FAULT0_A, TCC3_FAULT1_A,
                                                TCC3_FAULTA_A, TCC3_FAULTB_A, TCC3_OVF, TCC3_TRG, TCC3_UFS_A */
void TCC3_1_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* TCC3_MC_0 */
void TCC3_2_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* TCC3_MC_1 */
#endif
#ifdef ID_TCC4
void TCC4_0_Handler(void)
    __attribute__((weak,
                   alias("Dummy_Handler"))); /* TCC4_CNT_A, TCC4_DFS_A, TCC4_ERR_A, TCC4_FAULT0_A, TCC4_FAULT1_A,
                                                TCC4_FAULTA_A, TCC4_FAULTB_A, TCC4_OVF, TCC4_TRG, TCC4_UFS_A */
void TCC4_1_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* TCC4_MC_0 */
void TCC4_2_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* TCC4_MC_1 */
#endif
void TC0_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void TC1_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void TC2_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void TC3_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
#ifdef ID_TC4
void TC4_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
#endif
#ifdef ID_TC5
void TC5_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
#endif
#ifdef ID_TC6
void TC6_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
#endif
#ifdef ID_TC7
void TC7_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
#endif
void PDEC_0_Handler(void)
    __attribute__((weak, alias("Dummy_Handler"))); /* PDEC_DIR_A, PDEC_ERR_A, PDEC_OVF, PDEC_VLC_A */
void PDEC_1_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* PDEC_MC_0 */
void PDEC_2_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* PDEC_MC_1 */
void ADC0_0_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* ADC0_OVERRUN, ADC0_WINMON */
void ADC0_1_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* ADC0_RESRDY */
void ADC1_0_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* ADC1_OVERRUN, ADC1_WINMON */
void ADC1_1_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* ADC1_RESRDY */
void AC_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void DAC_0_Handler(void)
    __attribute__((weak,
                   alias("Dummy_Handler"))); /* DAC_OVERRUN_A_0, DAC_OVERRUN_A_1, DAC_UNDERRUN_A_0, DAC_UNDERRUN_A_1 */
void DAC_1_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* DAC_EMPTY_0 */
void DAC_2_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* DAC_EMPTY_1 */
void DAC_3_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* DAC_RESRDY_0 */
void DAC_4_Handler(void) __attribute__((weak, alias("Dummy_Handler"))); /* DAC_RESRDY_1 */
#ifdef ID_I2S
void I2S_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
#endif
void PCC_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void AES_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
void TRNG_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
#ifdef ID_ICM
void ICM_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
#endif
#ifdef ID_PUKCC
void PUKCC_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
#endif
void QSPI_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
#ifdef ID_SDHC0
void SDHC0_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
#endif
#ifdef ID_SDHC1
void SDHC1_Handler(void) __attribute__((weak, alias("Dummy_Handler")));
#endif

/* Exception Table */
__attribute__((section(".vectors"), used)) const DeviceVectors exception_table
    = {

        /* Configure Initial Stack Pointer, using linker-generated symbols */
        .pvStack = (void *)(&_estack),

        .pfnReset_Handler      = (void *)Reset_Handler,
        .pfnNMI_Handler        = (void *)NMI_Handler,
        .pfnHardFault_Handler  = (void *)HardFault_Handler,
        .pfnMemManage_Handler  = (void *)MemManage_Handler,
        .pfnBusFault_Handler   = (void *)BusFault_Handler,
        .pfnUsageFault_Handler = (void *)UsageFault_Handler,
        .pvReservedM9          = (void *)(0UL), /* Reserved */
        .pvReservedM8          = (void *)(0UL), /* Reserved */
        .pvReservedM7          = (void *)(0UL), /* Reserved */
        .pvReservedM6          = (void *)(0UL), /* Reserved */
        .pfnSVC_Handler        = (void *)SVC_Handler,
        .pfnDebugMon_Handler   = (void *)DebugMon_Handler,
        .pvReservedM3          = (void *)(0UL), /* Reserved */
        .pfnPendSV_Handler     = (void *)PendSV_Handler,
        .pfnSysTick_Handler    = (void *)SysTick_Handler,

        /* Configurable interrupts */
        .pfnPM_Handler        = (void *)PM_Handler,        /*  0 Power Manager */
        .pfnMCLK_Handler      = (void *)MCLK_Handler,      /*  1 Main Clock */
        .pfnOSCCTRL_0_Handler = (void *)OSCCTRL_0_Handler, /*  2 OSCCTRL_XOSCFAIL_0, OSCCTRL_XOSCRDY_0 */
        .pfnOSCCTRL_1_Handler = (void *)OSCCTRL_1_Handler, /*  3 OSCCTRL_XOSCFAIL_1, OSCCTRL_XOSCRDY_1 */
        .pfnOSCCTRL_2_Handler
        = (void *)OSCCTRL_2_Handler, /*  4 OSCCTRL_DFLLLOCKC, OSCCTRL_DFLLLOCKF, OSCCTRL_DFLLOOB, OSCCTRL_DFLLRCS,
                                        OSCCTRL_DFLLRDY */
        .pfnOSCCTRL_3_Handler = (void *)
            OSCCTRL_3_Handler, /*  5 OSCCTRL_DPLLLCKF_0, OSCCTRL_DPLLLCKR_0, OSCCTRL_DPLLLDRTO_0, OSCCTRL_DPLLLTO_0 */
        .pfnOSCCTRL_4_Handler = (void *)
            OSCCTRL_4_Handler, /*  6 OSCCTRL_DPLLLCKF_1, OSCCTRL_DPLLLCKR_1, OSCCTRL_DPLLLDRTO_1, OSCCTRL_DPLLLTO_1 */
        .pfnOSC32KCTRL_Handler = (void *)OSC32KCTRL_Handler, /*  7 32kHz Oscillators Control */
        .pfnSUPC_0_Handler     = (void *)SUPC_0_Handler, /*  8 SUPC_B12SRDY, SUPC_B33SRDY, SUPC_BOD12RDY, SUPC_BOD33RDY,
                                                            SUPC_VCORERDY, SUPC_VREGRDY */
        .pfnSUPC_1_Handler    = (void *)SUPC_1_Handler,  /*  9 SUPC_BOD12DET, SUPC_BOD33DET */
        .pfnWDT_Handler       = (void *)WDT_Handler,     /* 10 Watchdog Timer */
        .pfnRTC_Handler       = (void *)RTC_Handler,     /* 11 Real-Time Counter */
        .pfnEIC_0_Handler     = (void *)EIC_0_Handler,   /* 12 EIC_EXTINT_0 */
        .pfnEIC_1_Handler     = (void *)EIC_1_Handler,   /* 13 EIC_EXTINT_1 */
        .pfnEIC_2_Handler     = (void *)EIC_2_Handler,   /* 14 EIC_EXTINT_2 */
        .pfnEIC_3_Handler     = (void *)EIC_3_Handler,   /* 15 EIC_EXTINT_3 */
        .pfnEIC_4_Handler     = (void *)EIC_4_Handler,   /* 16 EIC_EXTINT_4 */
        .pfnEIC_5_Handler     = (void *)EIC_5_Handler,   /* 17 EIC_EXTINT_5 */
        .pfnEIC_6_Handler     = (void *)EIC_6_Handler,   /* 18 EIC_EXTINT_6 */
        .pfnEIC_7_Handler     = (void *)EIC_7_Handler,   /* 19 EIC_EXTINT_7 */
        .pfnEIC_8_Handler     = (void *)EIC_8_Handler,   /* 20 EIC_EXTINT_8 */
        .pfnEIC_9_Handler     = (void *)EIC_9_Handler,   /* 21 EIC_EXTINT_9 */
        .pfnEIC_10_Handler    = (void *)EIC_10_Handler,  /* 22 EIC_EXTINT_10 */
        .pfnEIC_11_Handler    = (void *)EIC_11_Handler,  /* 23 EIC_EXTINT_11 */
        .pfnEIC_12_Handler    = (void *)EIC_12_Handler,  /* 24 EIC_EXTINT_12 */
        .pfnEIC_13_Handler    = (void *)EIC_13_Handler,  /* 25 EIC_EXTINT_13 */
        .pfnEIC_14_Handler    = (void *)EIC_14_Handler,  /* 26 EIC_EXTINT_14 */
        .pfnEIC_15_Handler    = (void *)EIC_15_Handler,  /* 27 EIC_EXTINT_15 */
        .pfnFREQM_Handler     = (void *)FREQM_Handler,   /* 28 Frequency Meter */
        .pfnNVMCTRL_0_Handler = (void *)
            NVMCTRL_0_Handler, /* 29 NVMCTRL_0, NVMCTRL_1, NVMCTRL_2, NVMCTRL_3, NVMCTRL_4, NVMCTRL_5, NVMCTRL_6,
                                  NVMCTRL_7 */
        .pfnNVMCTRL_1_Handler = (void *)NVMCTRL_1_Handler, /* 30 NVMCTRL_10, NVMCTRL_8, NVMCTRL_9 */
        .pfnDMAC_0_Handler    = (void *)DMAC_0_Handler,    /* 31 DMAC_SUSP_0, DMAC_TCMPL_0, DMAC_TERR_0 */
        .pfnDMAC_1_Handler    = (void *)DMAC_1_Handler,    /* 32 DMAC_SUSP_1, DMAC_TCMPL_1, DMAC_TERR_1 */
        .pfnDMAC_2_Handler    = (void *)DMAC_2_Handler,    /* 33 DMAC_SUSP_2, DMAC_TCMPL_2, DMAC_TERR_2 */
        .pfnDMAC_3_Handler    = (void *)DMAC_3_Handler,    /* 34 DMAC_SUSP_3, DMAC_TCMPL_3, DMAC_TERR_3 */
        .pfnDMAC_4_Handler    = (void *)DMAC_4_Handler,    /* 35 DMAC_SUSP_10, DMAC_SUSP_11, DMAC_SUSP_12, DMAC_SUSP_13,
                                                              DMAC_SUSP_14, DMAC_SUSP_15, DMAC_SUSP_16, DMAC_SUSP_17,
                                                              DMAC_SUSP_18, DMAC_SUSP_19, DMAC_SUSP_20, DMAC_SUSP_21,
                                                              DMAC_SUSP_22, DMAC_SUSP_23, DMAC_SUSP_24, DMAC_SUSP_25,
                                                              DMAC_SUSP_26, DMAC_SUSP_27, DMAC_SUSP_28, DMAC_SUSP_29,
                                                              DMAC_SUSP_30, DMAC_SUSP_31, DMAC_SUSP_4, DMAC_SUSP_5,
                                                              DMAC_SUSP_6, DMAC_SUSP_7, DMAC_SUSP_8, DMAC_SUSP_9,
                                                              DMAC_TCMPL_10, DMAC_TCMPL_11, DMAC_TCMPL_12, DMAC_TCMPL_13,
                                                              DMAC_TCMPL_14, DMAC_TCMPL_15, DMAC_TCMPL_16, DMAC_TCMPL_17,
                                                              DMAC_TCMPL_18, DMAC_TCMPL_19, DMAC_TCMPL_20, DMAC_TCMPL_21,
                                                              DMAC_TCMPL_22, DMAC_TCMPL_23, DMAC_TCMPL_24, DMAC_TCMPL_25,
                                                              DMAC_TCMPL_26, DMAC_TCMPL_27, DMAC_TCMPL_28, DMAC_TCMPL_29,
                                                              DMAC_TCMPL_30, DMAC_TCMPL_31, DMAC_TCMPL_4, DMAC_TCMPL_5,
                                                              DMAC_TCMPL_6, DMAC_TCMPL_7, DMAC_TCMPL_8, DMAC_TCMPL_9,
                                                              DMAC_TERR_10, DMAC_TERR_11, DMAC_TERR_12, DMAC_TERR_13,
                                                              DMAC_TERR_14, DMAC_TERR_15, DMAC_TERR_16, DMAC_TERR_17,
                                                              DMAC_TERR_18, DMAC_TERR_19, DMAC_TERR_20, DMAC_TERR_21,
                                                              DMAC_TERR_22, DMAC_TERR_23, DMAC_TERR_24, DMAC_TERR_25,
                                                              DMAC_TERR_26, DMAC_TERR_27, DMAC_TERR_28, DMAC_TERR_29,
                                                              DMAC_TERR_30, DMAC_TERR_31, DMAC_TERR_4, DMAC_TERR_5,
                                                              DMAC_TERR_6, DMAC_TERR_7, DMAC_TERR_8, DMAC_TERR_9 */
        .pfnEVSYS_0_Handler = (void *)EVSYS_0_Handler,     /* 36 EVSYS_EVD_0, EVSYS_OVR_0 */
        .pfnEVSYS_1_Handler = (void *)EVSYS_1_Handler,     /* 37 EVSYS_EVD_1, EVSYS_OVR_1 */
        .pfnEVSYS_2_Handler = (void *)EVSYS_2_Handler,     /* 38 EVSYS_EVD_2, EVSYS_OVR_2 */
        .pfnEVSYS_3_Handler = (void *)EVSYS_3_Handler,     /* 39 EVSYS_EVD_3, EVSYS_OVR_3 */
        .pfnEVSYS_4_Handler = (void *)EVSYS_4_Handler,     /* 40 EVSYS_EVD_10, EVSYS_EVD_11, EVSYS_EVD_4, EVSYS_EVD_5,
                                                              EVSYS_EVD_6, EVSYS_EVD_7, EVSYS_EVD_8, EVSYS_EVD_9,
                                                              EVSYS_OVR_10, EVSYS_OVR_11, EVSYS_OVR_4, EVSYS_OVR_5,
                                                              EVSYS_OVR_6, EVSYS_OVR_7, EVSYS_OVR_8, EVSYS_OVR_9 */
        .pfnPAC_Handler       = (void *)PAC_Handler,       /* 41 Peripheral Access Controller */
        .pfnTAL_0_Handler     = (void *)TAL_0_Handler,     /* 42 TAL_BRK */
        .pfnTAL_1_Handler     = (void *)TAL_1_Handler,     /* 43 TAL_IPS_0, TAL_IPS_1 */
        .pvReserved44         = (void *)(0UL),             /* 44 Reserved */
        .pfnRAMECC_Handler    = (void *)RAMECC_Handler,    /* 45 RAM ECC */
        .pfnSERCOM0_0_Handler = (void *)SERCOM0_0_Handler, /* 46 SERCOM0_0 */
        .pfnSERCOM0_1_Handler = (void *)SERCOM0_1_Handler, /* 47 SERCOM0_1 */
        .pfnSERCOM0_2_Handler = (void *)SERCOM0_2_Handler, /* 48 SERCOM0_2 */
        .pfnSERCOM0_3_Handler = (void *)SERCOM0_3_Handler, /* 49 SERCOM0_3, SERCOM0_4, SERCOM0_5, SERCOM0_6 */
        .pfnSERCOM1_0_Handler = (void *)SERCOM1_0_Handler, /* 50 SERCOM1_0 */
        .pfnSERCOM1_1_Handler = (void *)SERCOM1_1_Handler, /* 51 SERCOM1_1 */
        .pfnSERCOM1_2_Handler = (void *)SERCOM1_2_Handler, /* 52 SERCOM1_2 */
        .pfnSERCOM1_3_Handler = (void *)SERCOM1_3_Handler, /* 53 SERCOM1_3, SERCOM1_4, SERCOM1_5, SERCOM1_6 */
        .pfnSERCOM2_0_Handler = (void *)SERCOM2_0_Handler, /* 54 SERCOM2_0 */
        .pfnSERCOM2_1_Handler = (void *)SERCOM2_1_Handler, /* 55 SERCOM2_1 */
        .pfnSERCOM2_2_Handler = (void *)SERCOM2_2_Handler, /* 56 SERCOM2_2 */
        .pfnSERCOM2_3_Handler = (void *)SERCOM2_3_Handler, /* 57 SERCOM2_3, SERCOM2_4, SERCOM2_5, SERCOM2_6 */
        .pfnSERCOM3_0_Handler = (void *)SERCOM3_0_Handler, /* 58 SERCOM3_0 */
        .pfnSERCOM3_1_Handler = (void *)SERCOM3_1_Handler, /* 59 SERCOM3_1 */
        .pfnSERCOM3_2_Handler = (void *)SERCOM3_2_Handler, /* 60 SERCOM3_2 */
        .pfnSERCOM3_3_Handler = (void *)SERCOM3_3_Handler, /* 61 SERCOM3_3, SERCOM3_4, SERCOM3_5, SERCOM3_6 */
#ifdef ID_SERCOM4
        .pfnSERCOM4_0_Handler = (void *)SERCOM4_0_Handler, /* 62 SERCOM4_0 */
        .pfnSERCOM4_1_Handler = (void *)SERCOM4_1_Handler, /* 63 SERCOM4_1 */
        .pfnSERCOM4_2_Handler = (void *)SERCOM4_2_Handler, /* 64 SERCOM4_2 */
        .pfnSERCOM4_3_Handler = (void *)SERCOM4_3_Handler, /* 65 SERCOM4_3, SERCOM4_4, SERCOM4_5, SERCOM4_6 */
#else
        .pvReserved62  = (void *)(0UL), /* 62 Reserved */
        .pvReserved63  = (void *)(0UL), /* 63 Reserved */
        .pvReserved64  = (void *)(0UL), /* 64 Reserved */
        .pvReserved65  = (void *)(0UL), /* 65 Reserved */
#endif
#ifdef ID_SERCOM5
        .pfnSERCOM5_0_Handler = (void *)SERCOM5_0_Handler, /* 66 SERCOM5_0 */
        .pfnSERCOM5_1_Handler = (void *)SERCOM5_1_Handler, /* 67 SERCOM5_1 */
        .pfnSERCOM5_2_Handler = (void *)SERCOM5_2_Handler, /* 68 SERCOM5_2 */
        .pfnSERCOM5_3_Handler = (void *)SERCOM5_3_Handler, /* 69 SERCOM5_3, SERCOM5_4, SERCOM5_5, SERCOM5_6 */
#else
        .pvReserved66  = (void *)(0UL), /* 66 Reserved */
        .pvReserved67  = (void *)(0UL), /* 67 Reserved */
        .pvReserved68  = (void *)(0UL), /* 68 Reserved */
        .pvReserved69  = (void *)(0UL), /* 69 Reserved */
#endif
#ifdef ID_SERCOM6
        .pfnSERCOM6_0_Handler = (void *)SERCOM6_0_Handler, /* 70 SERCOM6_0 */
        .pfnSERCOM6_1_Handler = (void *)SERCOM6_1_Handler, /* 71 SERCOM6_1 */
        .pfnSERCOM6_2_Handler = (void *)SERCOM6_2_Handler, /* 72 SERCOM6_2 */
        .pfnSERCOM6_3_Handler = (void *)SERCOM6_3_Handler, /* 73 SERCOM6_3, SERCOM6_4, SERCOM6_5, SERCOM6_6 */
#else
        .pvReserved70  = (void *)(0UL), /* 70 Reserved */
        .pvReserved71  = (void *)(0UL), /* 71 Reserved */
        .pvReserved72  = (void *)(0UL), /* 72 Reserved */
        .pvReserved73  = (void *)(0UL), /* 73 Reserved */
#endif
#ifdef ID_SERCOM7
        .pfnSERCOM7_0_Handler = (void *)SERCOM7_0_Handler, /* 74 SERCOM7_0 */
        .pfnSERCOM7_1_Handler = (void *)SERCOM7_1_Handler, /* 75 SERCOM7_1 */
        .pfnSERCOM7_2_Handler = (void *)SERCOM7_2_Handler, /* 76 SERCOM7_2 */
        .pfnSERCOM7_3_Handler = (void *)SERCOM7_3_Handler, /* 77 SERCOM7_3, SERCOM7_4, SERCOM7_5, SERCOM7_6 */
#else
        .pvReserved74  = (void *)(0UL), /* 74 Reserved */
        .pvReserved75  = (void *)(0UL), /* 75 Reserved */
        .pvReserved76  = (void *)(0UL), /* 76 Reserved */
        .pvReserved77  = (void *)(0UL), /* 77 Reserved */
#endif
#ifdef ID_CAN0
        .pfnCAN0_Handler = (void *)CAN0_Handler, /* 78 Control Area Network 0 */
#else
        .pvReserved78  = (void *)(0UL), /* 78 Reserved */
#endif
#ifdef ID_CAN1
        .pfnCAN1_Handler = (void *)CAN1_Handler, /* 79 Control Area Network 1 */
#else
        .pvReserved79  = (void *)(0UL), /* 79 Reserved */
#endif
#ifdef ID_USB
        .pfnUSB_0_Handler = (void *)
            USB_0_Handler, /* 80 USB_EORSM_DNRSM, USB_EORST_RST, USB_LPMSUSP_DDISC, USB_LPM_DCONN, USB_MSOF,
                              USB_RAMACER, USB_RXSTP_TXSTP_0, USB_RXSTP_TXSTP_1, USB_RXSTP_TXSTP_2, USB_RXSTP_TXSTP_3,
                              USB_RXSTP_TXSTP_4, USB_RXSTP_TXSTP_5, USB_RXSTP_TXSTP_6, USB_RXSTP_TXSTP_7,
                              USB_STALL0_STALL_0, USB_STALL0_STALL_1, USB_STALL0_STALL_2, USB_STALL0_STALL_3,
                              USB_STALL0_STALL_4, USB_STALL0_STALL_5, USB_STALL0_STALL_6, USB_STALL0_STALL_7,
                              USB_STALL1_0, USB_STALL1_1, USB_STALL1_2, USB_STALL1_3, USB_STALL1_4, USB_STALL1_5,
                              USB_STALL1_6, USB_STALL1_7, USB_SUSPEND, USB_TRFAIL0_TRFAIL_0, USB_TRFAIL0_TRFAIL_1,
                              USB_TRFAIL0_TRFAIL_2, USB_TRFAIL0_TRFAIL_3, USB_TRFAIL0_TRFAIL_4, USB_TRFAIL0_TRFAIL_5,
                              USB_TRFAIL0_TRFAIL_6, USB_TRFAIL0_TRFAIL_7, USB_TRFAIL1_PERR_0, USB_TRFAIL1_PERR_1,
                              USB_TRFAIL1_PERR_2, USB_TRFAIL1_PERR_3, USB_TRFAIL1_PERR_4, USB_TRFAIL1_PERR_5,
                              USB_TRFAIL1_PERR_6, USB_TRFAIL1_PERR_7, USB_UPRSM, USB_WAKEUP */
        .pfnUSB_1_Handler = (void *)USB_1_Handler, /* 81 USB_SOF_HSOF */
        .pfnUSB_2_Handler = (void *)USB_2_Handler, /* 82 USB_TRCPT0_0, USB_TRCPT0_1, USB_TRCPT0_2, USB_TRCPT0_3,
                                                      USB_TRCPT0_4, USB_TRCPT0_5, USB_TRCPT0_6, USB_TRCPT0_7 */
        .pfnUSB_3_Handler = (void *)USB_3_Handler, /* 83 USB_TRCPT1_0, USB_TRCPT1_1, USB_TRCPT1_2, USB_TRCPT1_3,
                                                      USB_TRCPT1_4, USB_TRCPT1_5, USB_TRCPT1_6, USB_TRCPT1_7 */
#else
        .pvReserved80  = (void *)(0UL), /* 80 Reserved */
        .pvReserved81  = (void *)(0UL), /* 81 Reserved */
        .pvReserved82  = (void *)(0UL), /* 82 Reserved */
        .pvReserved83  = (void *)(0UL), /* 83 Reserved */
#endif
#ifdef ID_GMAC
        .pfnGMAC_Handler = (void *)GMAC_Handler, /* 84 Ethernet MAC */
#else
        .pvReserved84  = (void *)(0UL), /* 84 Reserved */
#endif
        .pfnTCC0_0_Handler = (void *)
            TCC0_0_Handler, /* 85 TCC0_CNT_A, TCC0_DFS_A, TCC0_ERR_A, TCC0_FAULT0_A, TCC0_FAULT1_A, TCC0_FAULTA_A,
                               TCC0_FAULTB_A, TCC0_OVF, TCC0_TRG, TCC0_UFS_A */
        .pfnTCC0_1_Handler = (void *)TCC0_1_Handler, /* 86 TCC0_MC_0 */
        .pfnTCC0_2_Handler = (void *)TCC0_2_Handler, /* 87 TCC0_MC_1 */
        .pfnTCC0_3_Handler = (void *)TCC0_3_Handler, /* 88 TCC0_MC_2 */
        .pfnTCC0_4_Handler = (void *)TCC0_4_Handler, /* 89 TCC0_MC_3 */
        .pfnTCC0_5_Handler = (void *)TCC0_5_Handler, /* 90 TCC0_MC_4 */
        .pfnTCC0_6_Handler = (void *)TCC0_6_Handler, /* 91 TCC0_MC_5 */
        .pfnTCC1_0_Handler = (void *)
            TCC1_0_Handler, /* 92 TCC1_CNT_A, TCC1_DFS_A, TCC1_ERR_A, TCC1_FAULT0_A, TCC1_FAULT1_A, TCC1_FAULTA_A,
                               TCC1_FAULTB_A, TCC1_OVF, TCC1_TRG, TCC1_UFS_A */
        .pfnTCC1_1_Handler = (void *)TCC1_1_Handler, /* 93 TCC1_MC_0 */
        .pfnTCC1_2_Handler = (void *)TCC1_2_Handler, /* 94 TCC1_MC_1 */
        .pfnTCC1_3_Handler = (void *)TCC1_3_Handler, /* 95 TCC1_MC_2 */
        .pfnTCC1_4_Handler = (void *)TCC1_4_Handler, /* 96 TCC1_MC_3 */
        .pfnTCC2_0_Handler = (void *)
            TCC2_0_Handler, /* 97 TCC2_CNT_A, TCC2_DFS_A, TCC2_ERR_A, TCC2_FAULT0_A, TCC2_FAULT1_A, TCC2_FAULTA_A,
                               TCC2_FAULTB_A, TCC2_OVF, TCC2_TRG, TCC2_UFS_A */
        .pfnTCC2_1_Handler = (void *)TCC2_1_Handler, /* 98 TCC2_MC_0 */
        .pfnTCC2_2_Handler = (void *)TCC2_2_Handler, /* 99 TCC2_MC_1 */
        .pfnTCC2_3_Handler = (void *)TCC2_3_Handler, /* 100 TCC2_MC_2 */
#ifdef ID_TCC3
        .pfnTCC3_0_Handler
        = (void *)TCC3_0_Handler, /* 101 TCC3_CNT_A, TCC3_DFS_A, TCC3_ERR_A, TCC3_FAULT0_A, TCC3_FAULT1_A,
                                     TCC3_FAULTA_A, TCC3_FAULTB_A, TCC3_OVF, TCC3_TRG, TCC3_UFS_A */
        .pfnTCC3_1_Handler = (void *)TCC3_1_Handler, /* 102 TCC3_MC_0 */
        .pfnTCC3_2_Handler = (void *)TCC3_2_Handler, /* 103 TCC3_MC_1 */
#else
        .pvReserved101 = (void *)(0UL), /* 101 Reserved */
        .pvReserved102 = (void *)(0UL), /* 102 Reserved */
        .pvReserved103 = (void *)(0UL), /* 103 Reserved */
#endif
#ifdef ID_TCC4
        .pfnTCC4_0_Handler
        = (void *)TCC4_0_Handler, /* 104 TCC4_CNT_A, TCC4_DFS_A, TCC4_ERR_A, TCC4_FAULT0_A, TCC4_FAULT1_A,
                                     TCC4_FAULTA_A, TCC4_FAULTB_A, TCC4_OVF, TCC4_TRG, TCC4_UFS_A */
        .pfnTCC4_1_Handler = (void *)TCC4_1_Handler, /* 105 TCC4_MC_0 */
        .pfnTCC4_2_Handler = (void *)TCC4_2_Handler, /* 106 TCC4_MC_1 */
#else
        .pvReserved104 = (void *)(0UL), /* 104 Reserved */
        .pvReserved105 = (void *)(0UL), /* 105 Reserved */
        .pvReserved106 = (void *)(0UL), /* 106 Reserved */
#endif
        .pfnTC0_Handler = (void *)TC0_Handler, /* 107 Basic Timer Counter 0 */
        .pfnTC1_Handler = (void *)TC1_Handler, /* 108 Basic Timer Counter 1 */
        .pfnTC2_Handler = (void *)TC2_Handler, /* 109 Basic Timer Counter 2 */
        .pfnTC3_Handler = (void *)TC3_Handler, /* 110 Basic Timer Counter 3 */
#ifdef ID_TC4
        .pfnTC4_Handler = (void *)TC4_Handler, /* 111 Basic Timer Counter 4 */
#else
        .pvReserved111 = (void *)(0UL), /* 111 Reserved */
#endif
#ifdef ID_TC5
        .pfnTC5_Handler = (void *)TC5_Handler, /* 112 Basic Timer Counter 5 */
#else
        .pvReserved112 = (void *)(0UL), /* 112 Reserved */
#endif
#ifdef ID_TC6
        .pfnTC6_Handler = (void *)TC6_Handler, /* 113 Basic Timer Counter 6 */
#else
        .pvReserved113 = (void *)(0UL), /* 113 Reserved */
#endif
#ifdef ID_TC7
        .pfnTC7_Handler = (void *)TC7_Handler, /* 114 Basic Timer Counter 7 */
#else
        .pvReserved114 = (void *)(0UL), /* 114 Reserved */
#endif
        .pfnPDEC_0_Handler = (void *)PDEC_0_Handler, /* 115 PDEC_DIR_A, PDEC_ERR_A, PDEC_OVF, PDEC_VLC_A */
        .pfnPDEC_1_Handler = (void *)PDEC_1_Handler, /* 116 PDEC_MC_0 */
        .pfnPDEC_2_Handler = (void *)PDEC_2_Handler, /* 117 PDEC_MC_1 */
        .pfnADC0_0_Handler = (void *)ADC0_0_Handler, /* 118 ADC0_OVERRUN, ADC0_WINMON */
        .pfnADC0_1_Handler = (void *)ADC0_1_Handler, /* 119 ADC0_RESRDY */
        .pfnADC1_0_Handler = (void *)ADC1_0_Handler, /* 120 ADC1_OVERRUN, ADC1_WINMON */
        .pfnADC1_1_Handler = (void *)ADC1_1_Handler, /* 121 ADC1_RESRDY */
        .pfnAC_Handler     = (void *)AC_Handler,     /* 122 Analog Comparators */
        .pfnDAC_0_Handler
        = (void *)DAC_0_Handler, /* 123 DAC_OVERRUN_A_0, DAC_OVERRUN_A_1, DAC_UNDERRUN_A_0, DAC_UNDERRUN_A_1 */
        .pfnDAC_1_Handler = (void *)DAC_1_Handler, /* 124 DAC_EMPTY_0 */
        .pfnDAC_2_Handler = (void *)DAC_2_Handler, /* 125 DAC_EMPTY_1 */
        .pfnDAC_3_Handler = (void *)DAC_3_Handler, /* 126 DAC_RESRDY_0 */
        .pfnDAC_4_Handler = (void *)DAC_4_Handler, /* 127 DAC_RESRDY_1 */
#ifdef ID_I2S
        .pfnI2S_Handler = (void *)I2S_Handler, /* 128 Inter-IC Sound Interface */
#else
        .pvReserved128 = (void *)(0UL), /* 128 Reserved */
#endif
        .pfnPCC_Handler  = (void *)PCC_Handler,  /* 129 Parallel Capture Controller */
        .pfnAES_Handler  = (void *)AES_Handler,  /* 130 Advanced Encryption Standard */
        .pfnTRNG_Handler = (void *)TRNG_Handler, /* 131 True Random Generator */
#ifdef ID_ICM
        .pfnICM_Handler = (void *)ICM_Handler, /* 132 Integrity Check Monitor */
#else
        .pvReserved132 = (void *)(0UL), /* 132 Reserved */
#endif
#ifdef ID_PUKCC
        .pfnPUKCC_Handler = (void *)PUKCC_Handler, /* 133 PUblic-Key Cryptography Controller */
#else
        .pvReserved133 = (void *)(0UL), /* 133 Reserved */
#endif
        .pfnQSPI_Handler = (void *)QSPI_Handler, /* 134 Quad SPI interface */
#ifdef ID_SDHC0
        .pfnSDHC0_Handler = (void *)SDHC0_Handler, /* 135 SD/MMC Host Controller 0 */
#else
        .pvReserved135 = (void *)(0UL), /* 135 Reserved */
#endif
#ifdef ID_SDHC1
        .pfnSDHC1_Handler = (void *)SDHC1_Handler /* 136 SD/MMC Host Controller 1 */
#else
        .pvReserved136 = (void *)(0UL)  /* 136 Reserved */
#endif
};

/**
 * \brief This is the code that gets called on processor reset.
 * To initialize the device, and call the main() routine.
 */
void Reset_Handler(void)
{
	uint32_t *pSrc, *pDest;

	/* Initialize the relocate segment */
	pSrc  = &_etext;
	pDest = &_srelocate;

	if (pSrc != pDest) {
		for (; pDest < &_erelocate;) {
			*pDest++ = *pSrc++;
		}
	}

	/* Clear the zero segment */
	for (pDest = &_szero; pDest < &_ezero;) {
		*pDest++ = 0;
	}

	/* Set the vector table base address */
	pSrc      = (uint32_t *)&_sfixed;
	SCB->VTOR = ((uint32_t)pSrc & SCB_VTOR_TBLOFF_Msk);

#if __FPU_USED
	/* Enable FPU */
	SCB->CPACR |= (0xFu << 20);
	__DSB();
	__ISB();
#endif

	/* Initialize the C library */
	//__libc_init_array();

	/* Branch to main function */
	main();

	/* Infinite loop */
	while (1)
		;
}

/**
 * \brief Default interrupt handler for unused IRQs.
 */
void Dummy_Handler(void)
{
	while (1) {
	}
}
