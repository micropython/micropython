/*!
    \file    gd32f4xx_rcu.c
    \brief   RCU driver

    \version 2026-02-05, V3.3.3, firmware for GD32F4xx
*/

/*
    Copyright (c) 2026, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software without
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/

#include "gd32f4xx_rcu.h"

/* define clock source */
#define SEL_IRC16M                  ((uint16_t)0U)                            /* IRC16M is selected as CK_SYS */
#define SEL_HXTAL                   ((uint16_t)1U)                            /* HXTAL is selected as CK_SYS */
#define SEL_PLLP                    ((uint16_t)2U)                            /* PLLP is selected as CK_SYS */
/* define startup timeout count */
#define OSC_STARTUP_TIMEOUT         ((uint32_t)0x000fffffU)
#define LXTAL_STARTUP_TIMEOUT       ((uint32_t)0x0fffffffU)

/* RCU IRC16M adjust value mask and offset*/
#define RCU_IRC16M_ADJUST_MASK      ((uint8_t)0x1FU)
#define RCU_IRC16M_ADJUST_OFFSET    ((uint32_t)3U)

/*!
    \brief    deinitialize the RCU
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_deinit(void)
{
    /* enable IRC16M */
    RCU_CTL |= RCU_CTL_IRC16MEN;
    rcu_osci_stab_wait(RCU_IRC16M);
    RCU_CFG0 &= ~RCU_CFG0_SCS;

    /* reset CTL register */
    RCU_CTL &= ~(RCU_CTL_HXTALEN | RCU_CTL_CKMEN | RCU_CTL_PLLEN | RCU_CTL_PLLI2SEN
                 | RCU_CTL_PLLSAIEN);
    RCU_CTL &= ~(RCU_CTL_HXTALBPS);
    /* reset CFG0 register */
    RCU_CFG0 &= ~(RCU_CFG0_SCS | RCU_CFG0_AHBPSC | RCU_CFG0_APB1PSC | RCU_CFG0_APB2PSC |
                  RCU_CFG0_RTCDIV | RCU_CFG0_CKOUT0SEL | RCU_CFG0_I2SSEL | RCU_CFG0_CKOUT0DIV |
                  RCU_CFG0_CKOUT1DIV | RCU_CFG0_CKOUT1SEL);
    /* reset PLL register */
    RCU_PLL = 0x24003010U;
    /* reset PLLI2S register */
    RCU_PLLI2S = 0x24003000U;
    /* reset PLLSAI register */
    RCU_PLLSAI = 0x24003010U;
    /* reset INT register */
    RCU_INT = 0x00000000U;
    /* reset CFG1 register */
    RCU_CFG1 &= ~(RCU_CFG1_PLLSAIRDIV | RCU_CFG1_TIMERSEL);
}

/*!
    \brief    enable the peripherals clock
    \param[in]  periph: RCU peripherals, refer to rcu_periph_enum
                only one parameter can be selected which is shown as below:
      \arg        RCU_GPIOx (x = A, B, C, D, E, F, G, H, I): GPIO ports clock
      \arg        RCU_CRC: CRC clock
      \arg        RCU_BKPSRAM: BKPSRAM clock
      \arg        RCU_TCMSRAM: TCMSRAM clock
      \arg        RCU_DMAx (x=0,1): DMA clock
      \arg        RCU_IPA: IPA clock
      \arg        RCU_ENET: ENET clock
      \arg        RCU_ENETTX: ENETTX clock
      \arg        RCU_ENETRX: ENETRX clock
      \arg        RCU_ENETPTP: ENETPTP clock
      \arg        RCU_USBHS: USBHS clock
      \arg        RCU_USBHSULPI: USBHSULPI clock
      \arg        RCU_DCI: DCI clock
      \arg        RCU_TRNG: TRNG clock
      \arg        RCU_USBFS: USBFS clock
      \arg        RCU_EXMC: EXMC clock
      \arg        RCU_TIMERx (x = 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13): TIMER clock
      \arg        RCU_WWDGT: WWDGT clock
      \arg        RCU_SPIx (x = 0, 1, 2, 3, 4, 5): SPI clock
      \arg        RCU_USARTx (x = 0, 1, 2, 5): USART clock
      \arg        RCU_UARTx (x = 3, 4, 6, 7): UART clock
      \arg        RCU_I2Cx (x = 0, 1, 2): I2C clock
      \arg        RCU_CANx (x = 0, 1): CAN clock
      \arg        RCU_PMU: PMU clock
      \arg        RCU_DAC: DAC clock
      \arg        RCU_RTC: RTC clock
      \arg        RCU_ADCx (x = 0, 1, 2): ADC clock
      \arg        RCU_SDIO: SDIO clock
      \arg        RCU_SYSCFG: SYSCFG clock
      \arg        RCU_TLI: TLI clock
      \arg        RCU_CTC: CTC clock
      \arg        RCU_IREF: IREF clock
    \param[out] none
    \retval     none
*/
void rcu_periph_clock_enable(rcu_periph_enum periph)
{
    RCU_REG_VAL(periph) |= BIT(RCU_BIT_POS(periph));
}

/*!
    \brief    disable the peripherals clock
    \param[in]  periph: RCU peripherals, refer to rcu_periph_enum
                only one parameter can be selected which is shown as below:
      \arg        RCU_GPIOx (x = A, B, C, D, E, F, G, H, I): GPIO ports clock
      \arg        RCU_CRC: CRC clock
      \arg        RCU_BKPSRAM: BKPSRAM clock
      \arg        RCU_TCMSRAM: TCMSRAM clock
      \arg        RCU_DMAx (x=0,1): DMA clock
      \arg        RCU_IPA: IPA clock
      \arg        RCU_ENET: ENET clock
      \arg        RCU_ENETTX: ENETTX clock
      \arg        RCU_ENETRX: ENETRX clock
      \arg        RCU_ENETPTP: ENETPTP clock
      \arg        RCU_USBHS: USBHS clock
      \arg        RCU_USBHSULPI: USBHSULPI clock
      \arg        RCU_DCI: DCI clock
      \arg        RCU_TRNG: TRNG clock
      \arg        RCU_USBFS: USBFS clock
      \arg        RCU_EXMC: EXMC clock
      \arg        RCU_TIMERx (x = 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13): TIMER clock
      \arg        RCU_WWDGT: WWDGT clock
      \arg        RCU_SPIx (x = 0, 1, 2, 3, 4, 5): SPI clock
      \arg        RCU_USARTx (x = 0, 1, 2, 5): USART clock
      \arg        RCU_UARTx (x = 3, 4, 6, 7): UART clock
      \arg        RCU_I2Cx (x = 0, 1, 2): I2C clock
      \arg        RCU_CANx (x = 0, 1): CAN clock
      \arg        RCU_PMU: PMU clock
      \arg        RCU_DAC: DAC clock
      \arg        RCU_RTC: RTC clock
      \arg        RCU_ADCx (x = 0, 1, 2): ADC clock
      \arg        RCU_SDIO: SDIO clock
      \arg        RCU_SYSCFG: SYSCFG clock
      \arg        RCU_TLI: TLI clock
      \arg        RCU_CTC: CTC clock
      \arg        RCU_IREF: IREF clock
    \param[out] none
    \retval     none
*/
void rcu_periph_clock_disable(rcu_periph_enum periph)
{
    RCU_REG_VAL(periph) &= ~BIT(RCU_BIT_POS(periph));
}

/*!
    \brief    enable the peripherals clock when sleep mode
    \param[in]  periph: RCU peripherals, refer to rcu_periph_sleep_enum
                only one parameter can be selected which is shown as below:
      \arg        RCU_GPIOx_SLP (x = A, B, C, D, E, F, G, H, I): GPIO ports clock
      \arg        RCU_CRC_SLP: CRC clock
      \arg        RCU_FMC_SLP: FMC clock
      \arg        RCU_SRAM0_SLP: SRAM0 clock
      \arg        RCU_SRAM1_SLP: SRAM1 clock
      \arg        RCU_BKPSRAM: BKPSRAM clock
      \arg        RCU_SRAM2_SLP: SRAM2 clock
      \arg        RCU_DMAx_SLP (x=0,1): DMA clock
      \arg        RCU_IPA_SLP: IPA clock
      \arg        RCU_ENET_SLP: ENET clock
      \arg        RCU_ENETTX_SLP: ENETTX clock
      \arg        RCU_ENETRX_SLP: ENETRX clock
      \arg        RCU_ENETPTP_SLP: ENETPTP clock
      \arg        RCU_USBHS_SLP: USBHS clock
      \arg        RCU_USBHSULPI_SLP: USBHSULPI clock
      \arg        RCU_DCI_SLP: DCI clock
      \arg        RCU_TRNG_SLP: TRNG clock
      \arg        RCU_USBFS_SLP: USBFS clock
      \arg        RCU_EXMC_SLP: EXMC clock
      \arg        RCU_TIMERx_SLP (x = 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13): TIMER clock
      \arg        RCU_WWDGT_SLP: WWDGT clock
      \arg        RCU_SPIx_SLP (x = 0, 1, 2, 3, 4, 5): SPI clock
      \arg        RCU_USARTx_SLP (x = 0, 1, 2, 5): USART clock
      \arg        RCU_UARTx_SLP (x = 3, 4, 6, 7): UART clock
      \arg        RCU_I2Cx_SLP (x = 0, 1, 2): I2C clock
      \arg        RCU_CANx_SLP (x = 0, 1): CAN clock
      \arg        RCU_PMU_SLP: PMU clock
      \arg        RCU_DAC_SLP: DAC clock
      \arg        RCU_RTC_SLP: RTC clock
      \arg        RCU_ADCx_SLP (x = 0, 1, 2): ADC clock
      \arg        RCU_SDIO_SLP: SDIO clock
      \arg        RCU_SYSCFG_SLP: SYSCFG clock
      \arg        RCU_TLI_SLP: TLI clock
      \arg        RCU_CTC_SLP: CTC clock
      \arg        RCU_IREF_SLP: IREF clock
    \param[out] none
    \retval     none
*/
void rcu_periph_clock_sleep_enable(rcu_periph_sleep_enum periph)
{
    RCU_REG_VAL(periph) |= BIT(RCU_BIT_POS(periph));
}

/*!
    \brief    disable the peripherals clock when sleep mode
    \param[in]  periph: RCU peripherals, refer to rcu_periph_sleep_enum
                only one parameter can be selected which is shown as below:
      \arg        RCU_GPIOx_SLP (x = A, B, C, D, E, F, G, H, I): GPIO ports clock
      \arg        RCU_CRC_SLP: CRC clock
      \arg        RCU_FMC_SLP: FMC clock
      \arg        RCU_SRAM0_SLP: SRAM0 clock
      \arg        RCU_SRAM1_SLP: SRAM1 clock
      \arg        RCU_BKPSRAM: BKPSRAM clock
      \arg        RCU_SRAM2_SLP: SRAM2 clock
      \arg        RCU_DMAx_SLP (x=0,1): DMA clock
      \arg        RCU_IPA_SLP: IPA clock
      \arg        RCU_ENET_SLP: ENET clock
      \arg        RCU_ENETTX_SLP: ENETTX clock
      \arg        RCU_ENETRX_SLP: ENETRX clock
      \arg        RCU_ENETPTP_SLP: ENETPTP clock
      \arg        RCU_USBHS_SLP: USBHS clock
      \arg        RCU_USBHSULPI_SLP: USBHSULPI clock
      \arg        RCU_DCI_SLP: DCI clock
      \arg        RCU_TRNG_SLP: TRNG clock
      \arg        RCU_USBFS_SLP: USBFS clock
      \arg        RCU_EXMC_SLP: EXMC clock
      \arg        RCU_TIMERx_SLP (x = 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13): TIMER clock
      \arg        RCU_WWDGT_SLP: WWDGT clock
      \arg        RCU_SPIx_SLP (x = 0, 1, 2, 3, 4, 5): SPI clock
      \arg        RCU_USARTx_SLP (x = 0, 1, 2, 5): USART clock
      \arg        RCU_UARTx_SLP (x = 3, 4, 6, 7): UART clock
      \arg        RCU_I2Cx_SLP (x = 0, 1, 2): I2C clock
      \arg        RCU_CANx_SLP (x = 0, 1): CAN clock
      \arg        RCU_PMU_SLP: PMU clock
      \arg        RCU_DAC_SLP: DAC clock
      \arg        RCU_RTC_SLP: RTC clock
      \arg        RCU_ADCx_SLP (x = 0, 1, 2): ADC clock
      \arg        RCU_SDIO_SLP: SDIO clock
      \arg        RCU_SYSCFG_SLP: SYSCFG clock
      \arg        RCU_TLI_SLP: TLI clock
      \arg        RCU_CTC_SLP: CTC clock
      \arg        RCU_IREF_SLP: IREF clock
    \param[out] none
    \retval     none
*/
void rcu_periph_clock_sleep_disable(rcu_periph_sleep_enum periph)
{
    RCU_REG_VAL(periph) &= ~BIT(RCU_BIT_POS(periph));
}

/*!
    \brief    reset the peripherals
    \param[in]  periph_reset: RCU peripherals reset, refer to rcu_periph_reset_enum
                only one parameter can be selected which is shown as below:
      \arg        RCU_GPIOxRST (x = A, B, C, D, E, F, G, H, I): reset GPIO ports
      \arg        RCU_CRCRST: reset CRC
      \arg        RCU_DMAxRST (x=0,1): reset DMA
      \arg        RCU_IPARST: reset IPA
      \arg        RCU_ENETRST: reset ENET
      \arg        RCU_USBHSRST: reset USBHS
      \arg        RCU_DCIRST: reset DCI
      \arg        RCU_TRNGRST: reset TRNG
      \arg        RCU_USBFSRST: reset USBFS
      \arg        RCU_EXMCRST: reset EXMC
      \arg        RCU_TIMERxRST (x = 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13): reset TIMER
      \arg        RCU_WWDGTRST: reset WWDGT
      \arg        RCU_SPIxRST (x = 0, 1, 2, 3, 4, 5): reset SPI
      \arg        RCU_USARTxRST (x = 0, 1, 2, 5): reset USART
      \arg        RCU_UARTxRST (x = 3, 4, 6, 7): reset UART
      \arg        RCU_I2CxRST (x = 0, 1, 2): reset I2C
      \arg        RCU_CANxRST (x = 0, 1): reset CAN
      \arg        RCU_PMURST: reset PMU
      \arg        RCU_DACRST: reset DAC
      \arg        RCU_ADCRST (x = 0, 1, 2): reset ADC
      \arg        RCU_SDIORST: reset SDIO
      \arg        RCU_SYSCFGRST: reset SYSCFG
      \arg        RCU_TLIRST: reset TLI
      \arg        RCU_CTCRST: reset CTC
      \arg        RCU_IREFRST: reset IREF
    \param[out] none
    \retval     none
*/
void rcu_periph_reset_enable(rcu_periph_reset_enum periph_reset)
{
    RCU_REG_VAL(periph_reset) |= BIT(RCU_BIT_POS(periph_reset));
}

/*!
    \brief    disable reset the peripheral
    \param[in]  periph_reset: RCU peripherals reset, refer to rcu_periph_reset_enum
                only one parameter can be selected which is shown as below:
      \arg        RCU_GPIOxRST (x = A, B, C, D, E, F, G, H, I): reset GPIO ports
      \arg        RCU_CRCRST: reset CRC
      \arg        RCU_DMAxRST (x=0,1): reset DMA
      \arg        RCU_IPARST: reset IPA
      \arg        RCU_ENETRST: reset ENET
      \arg        RCU_USBHSRST: reset USBHS
      \arg        RCU_DCIRST: reset DCI
      \arg        RCU_TRNGRST: reset TRNG
      \arg        RCU_USBFSRST: reset USBFS
      \arg        RCU_EXMCRST: reset EXMC
      \arg        RCU_TIMERxRST (x = 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13): reset TIMER
      \arg        RCU_WWDGTRST: reset WWDGT
      \arg        RCU_SPIxRST (x = 0, 1, 2, 3, 4, 5): reset SPI
      \arg        RCU_USARTxRST (x = 0, 1, 2, 5): reset USART
      \arg        RCU_UARTxRST (x = 3, 4, 6, 7): reset UART
      \arg        RCU_I2CxRST (x = 0, 1, 2): reset I2C
      \arg        RCU_CANxRST (x = 0, 1): reset CAN
      \arg        RCU_PMURST: reset PMU
      \arg        RCU_DACRST: reset DAC
      \arg        RCU_ADCRST (x = 0, 1, 2): reset ADC
      \arg        RCU_SDIORST: reset SDIO
      \arg        RCU_SYSCFGRST: reset SYSCFG
      \arg        RCU_TLIRST: reset TLI
      \arg        RCU_CTCRST: reset CTC
      \arg        RCU_IREFRST: reset IREF
    \param[out] none
    \retval     none
*/
void rcu_periph_reset_disable(rcu_periph_reset_enum periph_reset)
{
    RCU_REG_VAL(periph_reset) &= ~BIT(RCU_BIT_POS(periph_reset));
}

/*!
    \brief    reset the BKP
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_bkp_reset_enable(void)
{
    RCU_BDCTL |= RCU_BDCTL_BKPRST;
}

/*!
    \brief    disable the BKP reset
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_bkp_reset_disable(void)
{
    RCU_BDCTL &= ~RCU_BDCTL_BKPRST;
}

/*!
    \brief    configure the system clock source
    \param[in]  ck_sys: system clock source select
                only one parameter can be selected which is shown as below:
      \arg        RCU_CKSYSSRC_IRC16M: select CK_IRC16M as the CK_SYS source
      \arg        RCU_CKSYSSRC_HXTAL: select CK_HXTAL as the CK_SYS source
      \arg        RCU_CKSYSSRC_PLLP: select CK_PLLP as the CK_SYS source
    \param[out] none
    \retval     none
*/
void rcu_system_clock_source_config(uint32_t ck_sys)
{
    uint32_t reg;

    reg = RCU_CFG0;
    /* reset the SCS bits and set according to ck_sys */
    reg &= ~RCU_CFG0_SCS;
    RCU_CFG0 = (reg | ck_sys);
}

/*!
    \brief    get the system clock source
    \param[in]  none
    \param[out] none
    \retval     which clock is selected as CK_SYS source
      \arg        RCU_SCSS_IRC16M: CK_IRC16M is selected as the CK_SYS source
      \arg        RCU_SCSS_HXTAL: CK_HXTAL is selected as the CK_SYS source
      \arg        RCU_SCSS_PLLP: CK_PLLP is selected as the CK_SYS source
*/
uint32_t rcu_system_clock_source_get(void)
{
    return (RCU_CFG0 & RCU_CFG0_SCSS);
}

/*!
    \brief    configure the AHB clock prescaler selection
    \param[in]  ck_ahb: AHB clock prescaler selection
                only one parameter can be selected which is shown as below:
      \arg        RCU_AHB_CKSYS_DIVx (x = 1, 2, 4, 8, 16, 64, 128, 256, 512): select CK_SYS / x as CK_AHB
    \param[out] none
    \retval     none
*/
void rcu_ahb_clock_config(uint32_t ck_ahb)
{
    uint32_t reg;

    reg = RCU_CFG0;
    /* reset the AHBPSC bits and set according to ck_ahb */
    reg &= ~RCU_CFG0_AHBPSC;
    RCU_CFG0 = (reg | ck_ahb);
}

/*!
    \brief    configure the APB1 clock prescaler selection
    \param[in]  ck_apb1: APB1 clock prescaler selection
                only one parameter can be selected which is shown as below:
      \arg        RCU_APB1_CKAHB_DIV1: select CK_AHB as CK_APB1
      \arg        RCU_APB1_CKAHB_DIV2: select CK_AHB / 2 as CK_APB1
      \arg        RCU_APB1_CKAHB_DIV4: select CK_AHB / 4 as CK_APB1
      \arg        RCU_APB1_CKAHB_DIV8: select CK_AHB / 8 as CK_APB1
      \arg        RCU_APB1_CKAHB_DIV16: select CK_AHB / 16 as CK_APB1
    \param[out] none
    \retval     none
*/
void rcu_apb1_clock_config(uint32_t ck_apb1)
{
    uint32_t reg;

    reg = RCU_CFG0;
    /* reset the APB1PSC and set according to ck_apb1 */
    reg &= ~RCU_CFG0_APB1PSC;
    RCU_CFG0 = (reg | ck_apb1);
}

/*!
    \brief    configure the APB2 clock prescaler selection
    \param[in]  ck_apb2: APB2 clock prescaler selection
                only one parameter can be selected which is shown as below:
      \arg        RCU_APB2_CKAHB_DIV1: select CK_AHB as CK_APB2
      \arg        RCU_APB2_CKAHB_DIV2: select CK_AHB / 2 as CK_APB2
      \arg        RCU_APB2_CKAHB_DIV4: select CK_AHB / 4 as CK_APB2
      \arg        RCU_APB2_CKAHB_DIV8: select CK_AHB / 8 as CK_APB2
      \arg        RCU_APB2_CKAHB_DIV16: select CK_AHB / 16 as CK_APB2
    \param[out] none
    \retval     none
*/
void rcu_apb2_clock_config(uint32_t ck_apb2)
{
    uint32_t reg;

    reg = RCU_CFG0;
    /* reset the APB2PSC and set according to ck_apb2 */
    reg &= ~RCU_CFG0_APB2PSC;
    RCU_CFG0 = (reg | ck_apb2);
}

/*!
    \brief    configure the CK_OUT0 clock source and divider
    \param[in]  ckout0_src: CK_OUT0 clock source selection
                only one parameter can be selected which is shown as below:
      \arg        RCU_CKOUT0SRC_IRC16M: IRC16M selected
      \arg        RCU_CKOUT0SRC_LXTAL: LXTAL selected
      \arg        RCU_CKOUT0SRC_HXTAL: HXTAL selected
      \arg        RCU_CKOUT0SRC_PLLP: PLLP selected
    \param[in]  ckout0_div: CK_OUT0 divider
      \arg        RCU_CKOUT0_DIVx(x = 1, 2, 3, 4, 5): CK_OUT0 is divided by x
    \param[out] none
    \retval     none
*/
void rcu_ckout0_config(uint32_t ckout0_src, uint32_t ckout0_div)
{
    uint32_t reg;

    reg = RCU_CFG0;
    /* reset the CKOUT0SRC, CKOUT0DIV and set according to ckout0_src and ckout0_div */
    reg &= ~(RCU_CFG0_CKOUT0SEL | RCU_CFG0_CKOUT0DIV);
    RCU_CFG0 = (reg | ckout0_src | ckout0_div);
}

/*!
    \brief    configure the CK_OUT1 clock source and divider
    \param[in]  ckout1_src: CK_OUT1 clock source selection
                only one parameter can be selected which is shown as below:
      \arg        RCU_CKOUT1SRC_SYSTEMCLOCK: system clock selected
      \arg        RCU_CKOUT1SRC_PLLI2SR: PLLI2SR selected
      \arg        RCU_CKOUT1SRC_HXTAL: HXTAL selected
      \arg        RCU_CKOUT1SRC_PLLP: PLLP selected
    \param[in]  ckout1_div: CK_OUT1 divider
      \arg        RCU_CKOUT1_DIVx(x = 1, 2, 3, 4, 5): CK_OUT1 is divided by x
    \param[out] none
    \retval     none
*/
void rcu_ckout1_config(uint32_t ckout1_src, uint32_t ckout1_div)
{
    uint32_t reg;

    reg = RCU_CFG0;
    /* reset the CKOUT1SRC, CKOUT1DIV and set according to ckout1_src and ckout1_div */
    reg &= ~(RCU_CFG0_CKOUT1SEL | RCU_CFG0_CKOUT1DIV);
    RCU_CFG0 = (reg | ckout1_src | ckout1_div);
}

/*!
    \brief    configure the main PLL clock
    \param[in]  pll_src: PLL clock source selection
      \arg        RCU_PLLSRC_IRC16M: select IRC16M as PLL source clock
      \arg        RCU_PLLSRC_HXTAL: select HXTAL as PLL source clock
    \param[in]  pll_psc: the PLL VCO source clock prescaler
      \arg         this parameter should be selected between 2 and 63
    \param[in]  pll_n: the PLL VCO clock multi factor
      \arg        this parameter should be selected between 64 and 500
    \param[in]  pll_p: the PLLP output frequency division factor from PLL VCO clock
      \arg        this parameter should be selected 2,4,6,8
    \param[in]  pll_q: the PLL Q output frequency division factor from PLL VCO clock
      \arg        this parameter should be selected between 2 and 15
    \param[out] none
    \retval     ErrStatus: SUCCESS or ERROR
*/
ErrStatus rcu_pll_config(uint32_t pll_src, uint32_t pll_psc, uint32_t pll_n, uint32_t pll_p, uint32_t pll_q)
{
    ErrStatus rcu_flag = SUCCESS;
    uint32_t ss_modulation_inc;
    uint32_t ss_modulation_reg;

    ss_modulation_inc = 0U;
    ss_modulation_reg = RCU_PLLSSCTL;

    /* calculate the minimum factor of PLLN */
    if((ss_modulation_reg & RCU_PLLSSCTL_SSCGON) == RCU_PLLSSCTL_SSCGON) {
        if((ss_modulation_reg & RCU_SS_TYPE_DOWN) == RCU_SS_TYPE_DOWN) {
            ss_modulation_inc += RCU_SS_MODULATION_DOWN_INC;
        } else {
            ss_modulation_inc += RCU_SS_MODULATION_CENTER_INC;
        }
    }

    /* check the function parameter */
    if(CHECK_PLL_PSC_VALID(pll_psc) && CHECK_PLL_N_VALID(pll_n, ss_modulation_inc) &&
            CHECK_PLL_P_VALID(pll_p) && CHECK_PLL_Q_VALID(pll_q)) {
        RCU_PLL = pll_psc | (pll_n << 6) | (((pll_p >> 1) - 1U) << 16) |
                  (pll_src) | (pll_q << 24);
    } else {
        rcu_flag = ERROR;
    }

    /* return status */
    return rcu_flag;
}

/*!
    \brief    configure the PLLI2S clock
    \param[in]  plli2s_n: the PLLI2S VCO clock multi factor
      \arg        this parameter should be selected between 50 and 500
    \param[in]  plli2s_r: the PLLI2S R output frequency division factor from PLLI2S VCO clock
      \arg        this parameter should be selected between 2 and 7
    \param[out] none
    \retval     ErrStatus: SUCCESS or ERROR
*/
ErrStatus rcu_plli2s_config(uint32_t plli2s_n, uint32_t plli2s_r)
{
    ErrStatus rcu_flag = SUCCESS;

    /* check the function parameter */
    if(CHECK_PLLI2S_N_VALID(plli2s_n) && CHECK_PLLI2S_R_VALID(plli2s_r)) {
        RCU_PLLI2S = (plli2s_n << 6) | (plli2s_r << 28);
    } else {
        rcu_flag = ERROR;
    }

    /* return status */
    return rcu_flag;
}

/*!
    \brief    configure the PLLSAI clock
    \param[in]  pllsai_n: the PLLSAI VCO clock multi factor
      \arg        this parameter should be selected between 50 and 500
    \param[in]  pllsai_p: the PLLSAI P output frequency division factor from PLL VCO clock
      \arg        this parameter should be selected 2,4,6,8
    \param[in]  pllsai_r: the PLLSAI R output frequency division factor from PLL VCO clock
      \arg        this parameter should be selected between 2 and 7
    \param[out] none
    \retval     ErrStatus: SUCCESS or ERROR
*/
ErrStatus rcu_pllsai_config(uint32_t pllsai_n, uint32_t pllsai_p, uint32_t pllsai_r)
{
    ErrStatus rcu_flag = SUCCESS;
 
    /* check the function parameter */
    if(CHECK_PLLSAI_N_VALID(pllsai_n) && CHECK_PLLSAI_P_VALID(pllsai_p) && CHECK_PLLSAI_R_VALID(pllsai_r)) {
        RCU_PLLSAI = (pllsai_n << 6U) | (((pllsai_p >> 1U) - 1U) << 16U) | (pllsai_r << 28U);
    } else {
        rcu_flag = ERROR;
    }

    /* return status */
    return rcu_flag;
}

/*!
    \brief    configure the RTC clock source selection
    \param[in]  rtc_clock_source: RTC clock source selection
                only one parameter can be selected which is shown as below:
      \arg        RCU_RTCSRC_NONE: no clock selected
      \arg        RCU_RTCSRC_LXTAL: CK_LXTAL selected as RTC source clock
      \arg        RCU_RTCSRC_IRC32K: CK_IRC32K selected as RTC source clock
      \arg        RCU_RTCSRC_HXTAL_DIV_RTCDIV: CK_HXTAL / RTCDIV selected as RTC source clock
    \param[out] none
    \retval     none
*/
void rcu_rtc_clock_config(uint32_t rtc_clock_source)
{
    uint32_t reg;

    reg = RCU_BDCTL;
    /* reset the RTCSRC bits and set according to rtc_clock_source */
    reg &= ~RCU_BDCTL_RTCSRC;
    RCU_BDCTL = (reg | rtc_clock_source);
}

/*!
    \brief    configure the frequency division of RTC clock when HXTAL was selected as its clock source
    \param[in]  rtc_div: RTC clock frequency division
                only one parameter can be selected which is shown as below:
      \arg        RCU_RTC_HXTAL_NONE: no clock for RTC
      \arg        RCU_RTC_HXTAL_DIVx: RTCDIV clock select CK_HXTAL / x, x = 2....31
    \param[out] none
    \retval     none
*/
void rcu_rtc_div_config(uint32_t rtc_div)
{
    uint32_t reg;

    reg = RCU_CFG0;
    /* reset the RTCDIV bits and set according to rtc_div value */
    reg &= ~RCU_CFG0_RTCDIV;
    RCU_CFG0 = (reg | rtc_div);
}


/*!
    \brief    configure the I2S clock source selection
    \param[in]  i2s_clock_source: I2S clock source selection
                only one parameter can be selected which is shown as below:
      \arg        RCU_I2SSRC_PLLI2S: CK_PLLI2S selected as I2S source clock
      \arg        RCU_I2SSRC_I2S_CKIN: external i2s_ckin pin selected as I2S source clock
    \param[out] none
    \retval     none
*/
void rcu_i2s_clock_config(uint32_t i2s_clock_source)
{
    uint32_t reg;

    reg = RCU_CFG0;
    /* reset the I2SSEL bit and set according to i2s_clock_source */
    reg &= ~RCU_CFG0_I2SSEL;
    RCU_CFG0 = (reg | i2s_clock_source);
}

/*!
    \brief    configure the CK48M clock source selection
    \param[in]  ck48m_clock_source: CK48M clock source selection
                only one parameter can be selected which is shown as below:
      \arg        RCU_CK48MSRC_PLL48M: CK_PLL48M selected as CK48M source clock
      \arg        RCU_CK48MSRC_IRC48M: CK_IRC48M selected as CK48M source clock
    \param[out] none
    \retval     none
*/
void rcu_ck48m_clock_config(uint32_t ck48m_clock_source)
{
    uint32_t reg;

    reg = RCU_ADDCTL;
    /* reset the CK48MSEL bit and set according to i2s_clock_source */
    reg &= ~RCU_ADDCTL_CK48MSEL;
    RCU_ADDCTL = (reg | ck48m_clock_source);
}

/*!
    \brief    configure the PLL48M clock source selection
    \param[in]  pll48m_clock_source: PLL48M clock source selection
                only one parameter can be selected which is shown as below:
      \arg        RCU_PLL48MSRC_PLLQ: CK_PLLQ selected as PLL48M source clock
      \arg        RCU_PLL48MSRC_PLLSAIP: CK_PLLSAIP selected as PLL48M source clock
    \param[out] none
    \retval     none
*/
void rcu_pll48m_clock_config(uint32_t pll48m_clock_source)
{
    uint32_t reg;

    reg = RCU_ADDCTL;
    /* reset the PLL48MSEL bit and set according to pll48m_clock_source */
    reg &= ~RCU_ADDCTL_PLL48MSEL;
    RCU_ADDCTL = (reg | pll48m_clock_source);
}

/*!
    \brief    configure the TIMER clock prescaler selection
    \param[in]  timer_clock_prescaler: TIMER clock selection
                only one parameter can be selected which is shown as below:
      \arg        RCU_TIMER_PSC_MUL2: if APB1PSC/APB2PSC in RCU_CFG0 register is 0b0xx(CK_APBx = CK_AHB)
                                      or 0b100(CK_APBx = CK_AHB/2), the TIMER clock is equal to CK_AHB(CK_TIMERx = CK_AHB).
                                      or else, the TIMER clock is twice the corresponding APB clock (TIMER in APB1 domain: CK_TIMERx = 2 x CK_APB1;
                                      TIMER in APB2 domain: CK_TIMERx = 2 x CK_APB2)
      \arg        RCU_TIMER_PSC_MUL4: if APB1PSC/APB2PSC in RCU_CFG0 register is 0b0xx(CK_APBx = CK_AHB),
                                      0b100(CK_APBx = CK_AHB/2), or 0b101(CK_APBx = CK_AHB/4), the TIMER clock is equal to CK_AHB(CK_TIMERx = CK_AHB).
                                      or else, the TIMER clock is four timers the corresponding APB clock (TIMER in APB1 domain: CK_TIMERx = 4 x CK_APB1;
                                      TIMER in APB2 domain: CK_TIMERx = 4 x CK_APB2)
    \param[out] none
    \retval     none
*/
void rcu_timer_clock_prescaler_config(uint32_t timer_clock_prescaler)
{
    /* configure the TIMERSEL bit and select the TIMER clock prescaler */
    if(timer_clock_prescaler == RCU_TIMER_PSC_MUL2) {
        RCU_CFG1 &= timer_clock_prescaler;
    } else {
        RCU_CFG1 |= timer_clock_prescaler;
    }
}

/*!
    \brief    configure the PLLSAIR divider used as input of TLI
    \param[in]  pllsai_r_div: PLLSAIR divider used as input of TLI
                only one parameter can be selected which is shown as below:
      \arg        RCU_PLLSAIR_DIVx(x=2,4,8,16): PLLSAIR divided x used as input of TLI
    \param[out] none
    \retval     none
*/
void rcu_tli_clock_div_config(uint32_t pllsai_r_div)
{
    uint32_t reg;

    reg = RCU_CFG1;
    /* reset the PLLSAIRDIV bit and set according to pllsai_r_div */
    reg &= ~RCU_CFG1_PLLSAIRDIV;
    RCU_CFG1 = (reg | pllsai_r_div);
}

/*!
    \brief    configure the LXTAL drive capability
    \param[in]  lxtal_dricap: drive capability of LXTAL
                only one parameter can be selected which is shown as below:
      \arg        RCU_LXTALDRI_LOWER_DRIVE: lower driving capability
      \arg        RCU_LXTALDRI_HIGHER_DRIVE: higher driving capability
    \param[out] none
    \retval     none
*/
void rcu_lxtal_drive_capability_config(uint32_t lxtal_dricap)
{
    uint32_t reg;

    reg = RCU_BDCTL;

    /* reset the LXTALDRI bits and set according to lxtal_dricap */
    reg &= ~RCU_BDCTL_LXTALDRI;
    RCU_BDCTL = (reg | lxtal_dricap);
}

/*!
    \brief    wait for oscillator stabilization flags is SET or oscillator startup is timeout
    \param[in]  osci: oscillator types, refer to rcu_osci_type_enum
                only one parameter can be selected which is shown as below:
      \arg        RCU_HXTAL: HXTAL
      \arg        RCU_LXTAL: LXTAL
      \arg        RCU_IRC16M: IRC16M
      \arg        RCU_IRC48M: IRC48M
      \arg        RCU_IRC32K: IRC32K
      \arg        RCU_PLL_CK: PLL
      \arg        RCU_PLLI2S_CK: PLLI2S
      \arg        RCU_PLLSAI_CK: PLLSAI
    \param[out] none
    \retval     ErrStatus: SUCCESS or ERROR
*/
ErrStatus rcu_osci_stab_wait(rcu_osci_type_enum osci)
{
    uint32_t stb_cnt = 0U;
    ErrStatus reval = ERROR;
    FlagStatus osci_stat = RESET;

    switch(osci) {
    /* wait HXTAL stable */
    case RCU_HXTAL:
        while((RESET == osci_stat) && (HXTAL_STARTUP_TIMEOUT != stb_cnt)) {
            osci_stat = rcu_flag_get(RCU_FLAG_HXTALSTB);
            stb_cnt++;
        }

        /* check whether flag is set */
        if(RESET != rcu_flag_get(RCU_FLAG_HXTALSTB)) {
            reval = SUCCESS;
        }
        break;
    /* wait LXTAL stable */
    case RCU_LXTAL:
        while((RESET == osci_stat) && (LXTAL_STARTUP_TIMEOUT != stb_cnt)) {
            osci_stat = rcu_flag_get(RCU_FLAG_LXTALSTB);
            stb_cnt++;
        }

        /* check whether flag is set */
        if(RESET != rcu_flag_get(RCU_FLAG_LXTALSTB)) {
            reval = SUCCESS;
        }
        break;
    /* wait IRC16M stable */
    case RCU_IRC16M:
        while((RESET == osci_stat) && (IRC16M_STARTUP_TIMEOUT != stb_cnt)) {
            osci_stat = rcu_flag_get(RCU_FLAG_IRC16MSTB);
            stb_cnt++;
        }

        /* check whether flag is set */
        if(RESET != rcu_flag_get(RCU_FLAG_IRC16MSTB)) {
            reval = SUCCESS;
        }
        break;
    /* wait IRC48M stable */
    case RCU_IRC48M:
        while((RESET == osci_stat) && (OSC_STARTUP_TIMEOUT != stb_cnt)) {
            osci_stat = rcu_flag_get(RCU_FLAG_IRC48MSTB);
            stb_cnt++;
        }

        /* check whether flag is set */
        if(RESET != rcu_flag_get(RCU_FLAG_IRC48MSTB)) {
            reval = SUCCESS;
        }
        break;
    /* wait IRC32K stable */
    case RCU_IRC32K:
        while((RESET == osci_stat) && (OSC_STARTUP_TIMEOUT != stb_cnt)) {
            osci_stat = rcu_flag_get(RCU_FLAG_IRC32KSTB);
            stb_cnt++;
        }

        /* check whether flag is set */
        if(RESET != rcu_flag_get(RCU_FLAG_IRC32KSTB)) {
            reval = SUCCESS;
        }
        break;
    /* wait PLL stable */
    case RCU_PLL_CK:
        while((RESET == osci_stat) && (OSC_STARTUP_TIMEOUT != stb_cnt)) {
            osci_stat = rcu_flag_get(RCU_FLAG_PLLSTB);
            stb_cnt++;
        }

        /* check whether flag is set */
        if(RESET != rcu_flag_get(RCU_FLAG_PLLSTB)) {
            reval = SUCCESS;
        }
        break;
    /* wait PLLI2S stable */
    case RCU_PLLI2S_CK:
        while((RESET == osci_stat) && (OSC_STARTUP_TIMEOUT != stb_cnt)) {
            osci_stat = rcu_flag_get(RCU_FLAG_PLLI2SSTB);
            stb_cnt++;
        }

        /* check whether flag is set */
        if(RESET != rcu_flag_get(RCU_FLAG_PLLI2SSTB)) {
            reval = SUCCESS;
        }
        break;
    /* wait PLLSAI stable */
    case RCU_PLLSAI_CK:
        while((RESET == osci_stat) && (OSC_STARTUP_TIMEOUT != stb_cnt)) {
            osci_stat = rcu_flag_get(RCU_FLAG_PLLSAISTB);
            stb_cnt++;
        }

        /* check whether flag is set */
        if(RESET != rcu_flag_get(RCU_FLAG_PLLSAISTB)) {
            reval = SUCCESS;
        }
        break;

    default:
        break;
    }

    /* return value */
    return reval;
}

/*!
    \brief    turn on the oscillator
    \param[in]  osci: oscillator types, refer to rcu_osci_type_enum
                only one parameter can be selected which is shown as below:
      \arg        RCU_HXTAL: HXTAL
      \arg        RCU_LXTAL: LXTAL
      \arg        RCU_IRC16M: IRC16M
      \arg        RCU_IRC48M: IRC48M
      \arg        RCU_IRC32K: IRC32K
      \arg        RCU_PLL_CK: PLL
      \arg        RCU_PLLI2S_CK: PLLI2S
      \arg        RCU_PLLSAI_CK: PLLSAI
    \param[out] none
    \retval     none
*/
void rcu_osci_on(rcu_osci_type_enum osci)
{
    RCU_REG_VAL(osci) |= BIT(RCU_BIT_POS(osci));
}

/*!
    \brief    turn off the oscillator
    \param[in]  osci: oscillator types, refer to rcu_osci_type_enum
                only one parameter can be selected which is shown as below:
      \arg        RCU_HXTAL: HXTAL
      \arg        RCU_LXTAL: LXTAL
      \arg        RCU_IRC16M: IRC16M
      \arg        RCU_IRC48M: IRC48M
      \arg        RCU_IRC32K: IRC32K
      \arg        RCU_PLL_CK: PLL
      \arg        RCU_PLLI2S_CK: PLLI2S
      \arg        RCU_PLLSAI_CK: PLLSAI
    \param[out] none
    \retval     none
*/
void rcu_osci_off(rcu_osci_type_enum osci)
{
    RCU_REG_VAL(osci) &= ~BIT(RCU_BIT_POS(osci));
}

/*!
    \brief    enable the oscillator bypass mode, HXTALEN or LXTALEN must be reset before it
    \param[in]  osci: oscillator types, refer to rcu_osci_type_enum
                only one parameter can be selected which is shown as below:
      \arg        RCU_HXTAL: high speed crystal oscillator(HXTAL)
      \arg        RCU_LXTAL: low speed crystal oscillator(LXTAL)
    \param[out] none
    \retval     none
*/
void rcu_osci_bypass_mode_enable(rcu_osci_type_enum osci)
{
    uint32_t reg;

    switch(osci) {
    /* enable HXTAL to bypass mode */
    case RCU_HXTAL:
        reg = RCU_CTL;
        RCU_CTL &= ~RCU_CTL_HXTALEN;
        RCU_CTL = (reg | RCU_CTL_HXTALBPS);
        break;
    /* enable LXTAL to bypass mode */
    case RCU_LXTAL:
        reg = RCU_BDCTL;
        RCU_BDCTL &= ~RCU_BDCTL_LXTALEN;
        RCU_BDCTL = (reg | RCU_BDCTL_LXTALBPS);
        break;
    case RCU_IRC16M:
    case RCU_IRC48M:
    case RCU_IRC32K:
    case RCU_PLL_CK:
    case RCU_PLLI2S_CK:
    case RCU_PLLSAI_CK:
        break;
    default:
        break;
    }
}

/*!
    \brief    disable the oscillator bypass mode, HXTALEN or LXTALEN must be reset before it
    \param[in]  osci: oscillator types, refer to rcu_osci_type_enum
                only one parameter can be selected which is shown as below:
      \arg        RCU_HXTAL: high speed crystal oscillator(HXTAL)
      \arg        RCU_LXTAL: low speed crystal oscillator(LXTAL)
    \param[out] none
    \retval     none
*/
void rcu_osci_bypass_mode_disable(rcu_osci_type_enum osci)
{
    uint32_t reg;

    switch(osci) {
    /* disable HXTAL to bypass mode */
    case RCU_HXTAL:
        reg = RCU_CTL;
        RCU_CTL &= ~RCU_CTL_HXTALEN;
        RCU_CTL = (reg & ~RCU_CTL_HXTALBPS);
        break;
    /* disable LXTAL to bypass mode */
    case RCU_LXTAL:
        reg = RCU_BDCTL;
        RCU_BDCTL &= ~RCU_BDCTL_LXTALEN;
        RCU_BDCTL = (reg & ~RCU_BDCTL_LXTALBPS);
        break;
    case RCU_IRC16M:
    case RCU_IRC48M:
    case RCU_IRC32K:
    case RCU_PLL_CK:
    case RCU_PLLI2S_CK:
    case RCU_PLLSAI_CK:
        break;
    default:
        break;
    }
}

/*!
    \brief    set the IRC16M adjust value
    \param[in]  irc16m_adjval: IRC16M adjust value, must be between 0 and 0x1F
      \arg        0x00 - 0x1F
    \param[out] none
    \retval     none
*/
void rcu_irc16m_adjust_value_set(uint32_t irc16m_adjval)
{
    uint32_t reg;

    reg = RCU_CTL;
    /* reset the IRC16MADJ bits and set according to irc16m_adjval */
    reg &= ~RCU_CTL_IRC16MADJ;
    RCU_CTL = (reg | ((irc16m_adjval & RCU_IRC16M_ADJUST_MASK) << RCU_IRC16M_ADJUST_OFFSET));
}

/*!
    \brief    configure the spread spectrum modulation for the main PLL clock
    \param[in]  spread_spectrum_type: PLL spread spectrum modulation type select
      \arg        RCU_SS_TYPE_CENTER: center spread type is selected
      \arg        RCU_SS_TYPE_DOWN: down spread type is selected
    \param[in]  modstep: configure PLL spread spectrum modulation profile amplitude and frequency
      \arg        This parameter should be selected between 0 and 7FFF.The following criteria must be met: MODSTEP*MODCNT <=2^15-1
    \param[in]  modcnt: configure PLL spread spectrum modulation profile amplitude and frequency
      \arg        This parameter should be selected between 0 and 1FFF.The following criteria must be met: MODSTEP*MODCNT <=2^15-1
    \param[out] none
    \retval     none
*/
void rcu_spread_spectrum_config(uint32_t spread_spectrum_type, uint32_t modstep, uint32_t modcnt)
{
    uint32_t reg;

    reg = RCU_PLLSSCTL;
    /* reset the RCU_PLLSSCTL register bits */
    reg &= ~(RCU_PLLSSCTL_MODCNT | RCU_PLLSSCTL_MODSTEP | RCU_PLLSSCTL_SS_TYPE);
    RCU_PLLSSCTL = (reg | spread_spectrum_type | modstep << 13 | modcnt);
}

/*!
    \brief    enable the PLL spread spectrum modulation
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_spread_spectrum_enable(void)
{
    RCU_PLLSSCTL |= RCU_PLLSSCTL_SSCGON;
}

/*!
    \brief    disable the PLL spread spectrum modulation
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_spread_spectrum_disable(void)
{
    RCU_PLLSSCTL &= ~RCU_PLLSSCTL_SSCGON;
}

/*!
    \brief    enable the HXTAL clock monitor
    \param[in]  none
    \param[out] none
    \retval     none
*/

void rcu_hxtal_clock_monitor_enable(void)
{
    RCU_CTL |= RCU_CTL_CKMEN;
}

/*!
    \brief    disable the HXTAL clock monitor
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_hxtal_clock_monitor_disable(void)
{
    RCU_CTL &= ~RCU_CTL_CKMEN;
}

/*!
    \brief    unlock the voltage key
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_voltage_key_unlock(void)
{
    RCU_VKEY = RCU_VKEY_UNLOCK;
}

/*!
    \brief    deep-sleep mode voltage select
    \param[in]  dsvol: deep sleep mode voltage
                only one parameter can be selected which is shown as below:
      \arg        RCU_DEEPSLEEP_V_0: the core voltage is default value
      \arg        RCU_DEEPSLEEP_V_1: the core voltage is (default value-0.1)V(customers are not recommended to use it)
      \arg        RCU_DEEPSLEEP_V_2: the core voltage is (default value-0.2)V(customers are not recommended to use it)
      \arg        RCU_DEEPSLEEP_V_3: the core voltage is (default value-0.3)V(customers are not recommended to use it)
    \param[out] none
    \retval     none
*/
void rcu_deepsleep_voltage_set(uint32_t dsvol)
{
    dsvol &= RCU_DSV_DSLPVS;
    RCU_DSV = dsvol;
}

/*!
    \brief    get the system clock, bus and peripheral clock frequency
    \param[in]  clock: the clock frequency which to get
                only one parameter can be selected which is shown as below:
      \arg        CK_SYS: system clock frequency
      \arg        CK_AHB: AHB clock frequency
      \arg        CK_APB1: APB1 clock frequency
      \arg        CK_APB2: APB2 clock frequency
    \param[out] none
    \retval     clock frequency of system, AHB, APB1, APB2
*/
uint32_t rcu_clock_freq_get(rcu_clock_freq_enum clock)
{
    uint32_t sws, ck_freq = 0U;
    uint32_t cksys_freq, ahb_freq, apb1_freq, apb2_freq;
    uint32_t pllpsc, plln, pllsel, pllp, ck_src, idx, clk_exp;

    /* exponent of AHB, APB1 and APB2 clock divider */
    const uint8_t ahb_exp[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};
    const uint8_t apb1_exp[8] = {0, 0, 0, 0, 1, 2, 3, 4};
    const uint8_t apb2_exp[8] = {0, 0, 0, 0, 1, 2, 3, 4};

    sws = GET_BITS(RCU_CFG0, 2, 3);
    switch(sws) {
    /* IRC16M is selected as CK_SYS */
    case SEL_IRC16M:
        cksys_freq = IRC16M_VALUE;
        break;
    /* HXTAL is selected as CK_SYS */
    case SEL_HXTAL:
        cksys_freq = HXTAL_VALUE;
        break;
    /* PLLP is selected as CK_SYS */
    case SEL_PLLP:
        /* get the value of PLLPSC[5:0] */
        pllpsc = GET_BITS(RCU_PLL, 0U, 5U);
        plln = GET_BITS(RCU_PLL, 6U, 14U);
        pllp = (GET_BITS(RCU_PLL, 16U, 17U) + 1U) * 2U;
        /* PLL clock source selection, HXTAL or IRC16M/2 */
        pllsel = (RCU_PLL & RCU_PLL_PLLSEL);
        if(RCU_PLLSRC_HXTAL == pllsel) {
            ck_src = HXTAL_VALUE;
        } else {
            ck_src = IRC16M_VALUE;
        }
        cksys_freq = ((ck_src / pllpsc) * plln) / pllp;
        break;
    /* IRC16M is selected as CK_SYS */
    default:
        cksys_freq = IRC16M_VALUE;
        break;
    }
    /* calculate AHB clock frequency */
    idx = GET_BITS(RCU_CFG0, 4, 7);
    clk_exp = ahb_exp[idx];
    ahb_freq = cksys_freq >> clk_exp;

    /* calculate APB1 clock frequency */
    idx = GET_BITS(RCU_CFG0, 10, 12);
    clk_exp = apb1_exp[idx];
    apb1_freq = ahb_freq >> clk_exp;

    /* calculate APB2 clock frequency */
    idx = GET_BITS(RCU_CFG0, 13, 15);
    clk_exp = apb2_exp[idx];
    apb2_freq = ahb_freq >> clk_exp;

    /* return the clocks frequency */
    switch(clock) {
    case CK_SYS:
        ck_freq = cksys_freq;
        break;
    case CK_AHB:
        ck_freq = ahb_freq;
        break;
    case CK_APB1:
        ck_freq = apb1_freq;
        break;
    case CK_APB2:
        ck_freq = apb2_freq;
        break;
    default:
        break;
    }
    return ck_freq;
}

/*!
    \brief    get the clock stabilization and periphral reset flags
    \param[in]  flag: the clock stabilization and periphral reset flags, refer to rcu_flag_enum
                only one parameter can be selected which is shown as below:
      \arg        RCU_FLAG_IRC16MSTB: IRC16M stabilization flag
      \arg        RCU_FLAG_HXTALSTB: HXTAL stabilization flag
      \arg        RCU_FLAG_PLLSTB: PLL stabilization flag
      \arg        RCU_FLAG_PLLI2SSTB: PLLI2S stabilization flag
      \arg        RCU_FLAG_PLLSAISTB: PLLSAI stabilization flag
      \arg        RCU_FLAG_LXTALSTB: LXTAL stabilization flag
      \arg        RCU_FLAG_IRC32KSTB: IRC32K stabilization flag
      \arg        RCU_FLAG_IRC48MSTB: IRC48M stabilization flag
      \arg        RCU_FLAG_BORRST: BOR reset flags
      \arg        RCU_FLAG_EPRST: external PIN reset flag
      \arg        RCU_FLAG_PORRST: Power reset flag
      \arg        RCU_FLAG_SWRST: software reset flag
      \arg        RCU_FLAG_FWDGTRST: free watchdog timer reset flag
      \arg        RCU_FLAG_WWDGTRST: window watchdog timer reset flag
      \arg        RCU_FLAG_LPRST: low-power reset flag
    \param[out] none
    \retval     none
*/
FlagStatus rcu_flag_get(rcu_flag_enum flag)
{
    FlagStatus rcu_flag = RESET;

    /* get the rcu flag */
    if(RESET != (RCU_REG_VAL(flag) & BIT(RCU_BIT_POS(flag)))) {
        rcu_flag = SET;
    } else {
        rcu_flag = RESET;
    }

    return rcu_flag;
}

/*!
    \brief    clear all the reset flag
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_all_reset_flag_clear(void)
{
    RCU_RSTSCK |= RCU_RSTSCK_RSTFC;
}

/*!
    \brief    get the clock stabilization interrupt and ckm flags
    \param[in]  int_flag: interrupt and ckm flags, refer to rcu_int_flag_enum
                only one parameter can be selected which is shown as below:
      \arg        RCU_INT_FLAG_IRC32KSTB: IRC32K stabilization interrupt flag
      \arg        RCU_INT_FLAG_LXTALSTB: LXTAL stabilization interrupt flag
      \arg        RCU_INT_FLAG_IRC16MSTB: IRC16M stabilization interrupt flag
      \arg        RCU_INT_FLAG_HXTALSTB: HXTAL stabilization interrupt flag
      \arg        RCU_INT_FLAG_PLLSTB: PLL stabilization interrupt flag
      \arg        RCU_INT_FLAG_PLLI2SSTB: PLLI2S stabilization interrupt flag
      \arg        RCU_INT_FLAG_PLLSAISTB: PLLSAI stabilization interrupt flag
      \arg        RCU_INT_FLAG_CKM: HXTAL clock stuck interrupt flag
      \arg        RCU_INT_FLAG_IRC48MSTB: IRC48M stabilization interrupt flag
    \param[out] none
    \retval     FlagStatus: SET or RESET
*/
FlagStatus rcu_interrupt_flag_get(rcu_int_flag_enum int_flag)
{
    FlagStatus rcu_flag = RESET;

    /* get the rcu interrupt flag */
    if(RESET != (RCU_REG_VAL(int_flag) & BIT(RCU_BIT_POS(int_flag)))) {
        rcu_flag = SET;
    } else {
        rcu_flag = RESET;
    }

    return rcu_flag;
}

/*!
    \brief    clear the interrupt flags
    \param[in]  int_flag: clock stabilization and stuck interrupt flags clear, refer to rcu_int_flag_clear_enum
                only one parameter can be selected which is shown as below:
      \arg        RCU_INT_FLAG_IRC32KSTB_CLR: IRC32K stabilization interrupt flag clear
      \arg        RCU_INT_FLAG_LXTALSTB_CLR: LXTAL stabilization interrupt flag clear
      \arg        RCU_INT_FLAG_IRC16MSTB_CLR: IRC16M stabilization interrupt flag clear
      \arg        RCU_INT_FLAG_HXTALSTB_CLR: HXTAL stabilization interrupt flag clear
      \arg        RCU_INT_FLAG_PLLSTB_CLR: PLL stabilization interrupt flag clear
      \arg        RCU_INT_FLAG_PLLI2SSTB_CLR: PLLI2S stabilization interrupt flag clear
      \arg        RCU_INT_FLAG_PLLSAISTB_CLR: PLLSAI stabilization interrupt flag clear
      \arg        RCU_INT_FLAG_CKM_CLR: clock stuck interrupt flag clear
      \arg        RCU_INT_FLAG_IRC48MSTB_CLR: IRC48M stabilization interrupt flag clear
    \param[out] none
    \retval     none
*/
void rcu_interrupt_flag_clear(rcu_int_flag_clear_enum int_flag)
{
    RCU_REG_VAL(int_flag) |= BIT(RCU_BIT_POS(int_flag));
}

/*!
    \brief    enable the stabilization interrupt
    \param[in]  interrupt: clock stabilization interrupt, refer to rcu_int_enum
                Only one parameter can be selected which is shown as below:
      \arg        RCU_INT_IRC32KSTB: IRC32K stabilization interrupt enable
      \arg        RCU_INT_LXTALSTB: LXTAL stabilization interrupt enable
      \arg        RCU_INT_IRC16MSTB: IRC16M stabilization interrupt enable
      \arg        RCU_INT_HXTALSTB: HXTAL stabilization interrupt enable
      \arg        RCU_INT_PLLSTB: PLL stabilization interrupt enable
      \arg        RCU_INT_PLLI2SSTB: PLLI2S stabilization interrupt enable
      \arg        RCU_INT_PLLSAISTB: PLLSAI stabilization interrupt enable
      \arg        RCU_INT_IRC48MSTB: IRC48M stabilization interrupt enable
    \param[out] none
    \retval     none
*/
void rcu_interrupt_enable(rcu_int_enum interrupt)
{
    RCU_REG_VAL(interrupt) |= BIT(RCU_BIT_POS(interrupt));
}


/*!
    \brief    disable the stabilization interrupt
    \param[in]  interrupt: clock stabilization interrupt, refer to rcu_int_enum
                only one parameter can be selected which is shown as below:
      \arg        RCU_INT_IRC32KSTB: IRC32K stabilization interrupt disable
      \arg        RCU_INT_LXTALSTB: LXTAL stabilization interrupt disable
      \arg        RCU_INT_IRC16MSTB: IRC16M stabilization interrupt disable
      \arg        RCU_INT_HXTALSTB: HXTAL stabilization interrupt disable
      \arg        RCU_INT_PLLSTB: PLL stabilization interrupt disable
      \arg        RCU_INT_PLLI2SSTB: PLLI2S stabilization interrupt disable
      \arg        RCU_INT_PLLSAISTB: PLLSAI stabilization interrupt disable
      \arg        RCU_INT_IRC48MSTB: IRC48M stabilization interrupt disable
    \param[out] none
    \retval     none
*/
void rcu_interrupt_disable(rcu_int_enum interrupt)
{
    RCU_REG_VAL(interrupt) &= ~BIT(RCU_BIT_POS(interrupt));
}
