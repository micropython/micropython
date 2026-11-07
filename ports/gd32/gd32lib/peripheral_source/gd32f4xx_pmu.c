/*!
    \file    gd32f4xx_pmu.c
    \brief   PMU driver

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

#include "gd32f4xx_pmu.h"
#include "core_cm4.h"

/*!
    \brief      reset PMU registers
    \param[in]  none
    \param[out] none
    \retval     none
*/
void pmu_deinit(void)
{
    /* reset PMU */
    rcu_periph_reset_enable(RCU_PMURST);
    rcu_periph_reset_disable(RCU_PMURST);
}

/*!
    \brief      select low voltage detector threshold
    \param[in]  lvdt_n:
      \arg        PMU_LVDT_0: voltage threshold is 2.1V
      \arg        PMU_LVDT_1: voltage threshold is 2.3V
      \arg        PMU_LVDT_2: voltage threshold is 2.4V
      \arg        PMU_LVDT_3: voltage threshold is 2.6V
      \arg        PMU_LVDT_4: voltage threshold is 2.7V
      \arg        PMU_LVDT_5: voltage threshold is 2.9V
      \arg        PMU_LVDT_6: voltage threshold is 3.0V
      \arg        PMU_LVDT_7: voltage threshold is 3.1V
    \param[out] none
    \retval     none
*/
void pmu_lvd_select(uint32_t lvdt_n)
{
    /* disable LVD */
    PMU_CTL &= ~PMU_CTL_LVDEN;
    /* clear LVDT bits */
    PMU_CTL &= ~PMU_CTL_LVDT;
    /* set LVDT bits according to pmu_lvdt_n */
    PMU_CTL |= lvdt_n;
    /* enable LVD */
    PMU_CTL |= PMU_CTL_LVDEN;
}

/*!
    \brief      disable PMU lvd
    \param[in]  none
    \param[out] none
    \retval     none
*/
void pmu_lvd_disable(void)
{
    /* disable LVD */
    PMU_CTL &= ~PMU_CTL_LVDEN;
}

/*!
    \brief      select LDO output voltage
                this bit set by software when the main PLL closed, before closing PLL, change the system clock to IRC16M or HXTAL
    \param[in]  ldo_output:
      \arg        PMU_LDOVS_LOW: low-driver mode enable in deep-sleep mode
      \arg        PMU_LDOVS_MID: mid-driver mode disable in deep-sleep mode
      \arg        PMU_LDOVS_HIGH: high-driver mode disable in deep-sleep mode
    \param[out] none
    \retval     none
*/
void pmu_ldo_output_select(uint32_t ldo_output)
{
    PMU_CTL &= ~PMU_CTL_LDOVS;
    PMU_CTL |= ldo_output;
}

/*!
    \brief      enable high-driver mode
                this bit set by software only when IRC16M or HXTAL used as system clock
    \param[in]  none
    \param[out] none
    \retval     none
*/
void pmu_highdriver_mode_enable(void)
{
    PMU_CTL |= PMU_CTL_HDEN;
}

/*!
    \brief      disable high-driver mode
    \param[in]  none
    \param[out] none
    \retval     none
*/
void pmu_highdriver_mode_disable(void)
{
    PMU_CTL &= ~PMU_CTL_HDEN;
}

/*!
    \brief      switch high-driver mode
                this bit set by software only when IRC16M or HXTAL used as system clock
    \param[in]  highdr_switch:
      \arg        PMU_HIGHDR_SWITCH_NONE: disable high-driver mode switch
      \arg        PMU_HIGHDR_SWITCH_EN: enable high-driver mode switch
    \param[out] none
    \retval     none
*/
void pmu_highdriver_switch_select(uint32_t highdr_switch)
{
    /* wait for HDRF flag set */
    while(SET != pmu_flag_get(PMU_FLAG_HDRF)) {
    }
    PMU_CTL &= ~PMU_CTL_HDS;
    PMU_CTL |= highdr_switch;
}

/*!
    \brief      enable low-driver mode in deep-sleep
    \param[in]  none
    \param[out] none
    \retval     none
*/
void pmu_lowdriver_mode_enable(void)
{
    PMU_CTL |= PMU_CTL_LDEN;
}

/*!
    \brief      disable low-driver mode in deep-sleep
    \param[in]  none
    \param[out] none
    \retval     none
*/
void pmu_lowdriver_mode_disable(void)
{
    PMU_CTL &= ~PMU_CTL_LDEN;
}

/*!
    \brief      in deep-sleep mode, driver mode when use low power LDO
    \param[in]  mode:
      \arg        PMU_NORMALDR_LOWPWR:  normal driver when use low power LDO
      \arg        PMU_LOWDR_LOWPWR:  low-driver mode enabled when LDEN is 11 and use low power LDO
    \param[out] none
    \retval     none
*/
void pmu_lowpower_driver_config(uint32_t mode)
{
    PMU_CTL &= ~PMU_CTL_LDLP;
    PMU_CTL |= mode;
}

/*!
    \brief      in deep-sleep mode, driver mode when use normal power LDO
    \param[in]  mode:
      \arg        PMU_NORMALDR_NORMALPWR: normal driver when use normal power LDO
      \arg        PMU_LOWDR_NORMALPWR: low-driver mode enabled when LDEN is 11 and use normal power LDO
    \param[out] none
    \retval     none
*/
void pmu_normalpower_driver_config(uint32_t mode)
{
    PMU_CTL &= ~PMU_CTL_LDNP;
    PMU_CTL |= mode;
}

/*!
    \brief      PMU work in sleep mode
    \param[in]  sleepmodecmd:
      \arg        WFI_CMD: use WFI command
      \arg        WFE_CMD: use WFE command
    \param[out] none
    \retval     none
*/
void pmu_to_sleepmode(uint8_t sleepmodecmd)
{
    /* clear sleepdeep bit of Cortex-M4 system control register */
    SCB->SCR &= ~((uint32_t)SCB_SCR_SLEEPDEEP_Msk);

    /* select WFI or WFE command to enter sleep mode */
    if(WFI_CMD == sleepmodecmd) {
        __WFI();
    } else {
        __SEV();
        __WFE();
        __WFE();
    }
}

/*!
    \brief      PMU work in deep-sleep mode
    \param[in]  ldo
      \arg        PMU_LDO_NORMAL: LDO normal work when pmu enter deep-sleep mode
      \arg        PMU_LDO_LOWPOWER: LDO work at low power mode when pmu enter deep-sleep mode
    \param[in]  lowdrive:
                only one parameter can be selected which is shown as below:
      \arg        PMU_LOWDRIVER_DISABLE: Low-driver mode disable in deep-sleep mode
      \arg        PMU_LOWDRIVER_ENABLE: Low-driver mode enable in deep-sleep mode
    \param[in]  deepsleepmodecmd:
      \arg        WFI_CMD: use WFI command
      \arg        WFE_CMD: use WFE command
    \param[out] none
    \retval     none
*/
void pmu_to_deepsleepmode(uint32_t ldo, uint32_t lowdrive, uint8_t deepsleepmodecmd)
{
    static uint32_t reg_snap[4];
    /* clear stbmod and ldolp bits */
    PMU_CTL &= ~((uint32_t)(PMU_CTL_STBMOD | PMU_CTL_LDOLP | PMU_CTL_LDEN | PMU_CTL_LDNP | PMU_CTL_LDLP));

    /* set ldolp bit according to pmu_ldo */
    PMU_CTL |= ldo;

    /* configure low drive mode in deep-sleep mode */
    if(PMU_LOWDRIVER_ENABLE == lowdrive) {
        if(PMU_LDO_NORMAL == ldo) {
            PMU_CTL |= (uint32_t)(PMU_CTL_LDEN | PMU_CTL_LDNP);
        } else {
            PMU_CTL |= (uint32_t)(PMU_CTL_LDEN | PMU_CTL_LDLP);
        }
    }
    /* set sleepdeep bit of Cortex-M4 system control register */
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

    reg_snap[0] = REG32(0xE000E010U);
    reg_snap[1] = REG32(0xE000E100U);
    reg_snap[2] = REG32(0xE000E104U);
    reg_snap[3] = REG32(0xE000E108U);

    REG32(0xE000E010U) &= 0x00010004U;
    REG32(0xE000E180U)  = 0XFF7FF831U;
    REG32(0xE000E184U)  = 0XBFFFF8FFU;
    REG32(0xE000E188U)  = 0xFFFFEFFFU;

    /* select WFI or WFE command to enter deep-sleep mode */
    if(WFI_CMD == deepsleepmodecmd) {
        __WFI();
    } else {
        __SEV();
        __WFE();
        __WFE();
    }

    REG32(0xE000E010U) = reg_snap[0];
    REG32(0xE000E100U) = reg_snap[1];
    REG32(0xE000E104U) = reg_snap[2];
    REG32(0xE000E108U) = reg_snap[3];

    /* reset sleepdeep bit of Cortex-M4 system control register */
    SCB->SCR &= ~((uint32_t)SCB_SCR_SLEEPDEEP_Msk);
}

/*!
    \brief      pmu work in standby mode
    \param[in]  none
    \param[out] none
    \retval     none
*/
void pmu_to_standbymode(void)
{
    /* set stbmod bit */
    PMU_CTL |= PMU_CTL_STBMOD;

    /* reset wakeup flag */
    PMU_CTL |= PMU_CTL_WURST;

    /* set sleepdeep bit of Cortex-M4 system control register */
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

    REG32(0xE000E010U) &= 0x00010004U;
    REG32(0xE000E180U)  = 0XFFFFFFF3U;
    REG32(0xE000E184U)  = 0XFFFFFDFFU;
    REG32(0xE000E188U)  = 0xFFFFFFFFU;
    REG32(0xE000E18CU)  = 0xFFFFFFFFU;

    /* select WFI command to enter standby mode */
    __WFI();
}

/*!
    \brief      enable PMU wakeup pin
    \param[in]  none
    \param[out] none
    \retval     none
*/
void pmu_wakeup_pin_enable(void)
{
    PMU_CS |= PMU_CS_WUPEN;
}

/*!
    \brief      disable PMU wakeup pin
    \param[in]  none
    \param[out] none
    \retval     none
*/
void pmu_wakeup_pin_disable(void)
{
    PMU_CS &= ~PMU_CS_WUPEN;
}

/*!
    \brief      backup SRAM LDO on
    \param[in]  bkp_ldo:
      \arg        PMU_BLDOON_OFF: backup SRAM LDO closed
      \arg        PMU_BLDOON_ON: open the backup SRAM LDO
    \param[out] none
    \retval     none
*/
void pmu_backup_ldo_config(uint32_t bkp_ldo)
{
    PMU_CS &= ~PMU_CS_BLDOON;
    PMU_CS |= bkp_ldo;
}

/*!
    \brief      enable write access to the registers in backup domain
    \param[in]  none
    \param[out] none
    \retval     none
*/
void pmu_backup_write_enable(void)
{
    PMU_CTL |= PMU_CTL_BKPWEN;
}

/*!
    \brief      disable write access to the registers in backup domain
    \param[in]  none
    \param[out] none
    \retval     none
*/
void pmu_backup_write_disable(void)
{
    PMU_CTL &= ~PMU_CTL_BKPWEN;
}

/*!
    \brief      get flag state
    \param[in]  flag:
      \arg        PMU_FLAG_WAKEUP: wakeup flag
      \arg        PMU_FLAG_STANDBY: standby flag
      \arg        PMU_FLAG_LVD: lvd flag
      \arg        PMU_FLAG_BLDORF: backup SRAM LDO ready flag
      \arg        PMU_FLAG_LDOVSRF: LDO voltage select ready flag
      \arg        PMU_FLAG_HDRF: high-driver ready flag
      \arg        PMU_FLAG_HDSRF: high-driver switch ready flag
      \arg        PMU_FLAG_LDRF: low-driver mode ready flag
    \param[out] none
    \retval     FlagStatus: SET or RESET
*/
FlagStatus pmu_flag_get(uint32_t flag)
{
    FlagStatus status;
    if(PMU_CS & flag) {
        status = SET;
    } else {
        status = RESET;
    }
    return status;
}

/*!
    \brief      clear flag bit
    \param[in]  flag:
      \arg        PMU_FLAG_RESET_WAKEUP: reset wakeup flag
      \arg        PMU_FLAG_RESET_STANDBY: reset standby flag
    \param[out] none
    \retval     none
*/
void pmu_flag_clear(uint32_t flag)
{
    switch(flag) {
    case PMU_FLAG_RESET_WAKEUP:
        /* reset wakeup flag */
        PMU_CTL |= PMU_CTL_WURST;
        break;
    case PMU_FLAG_RESET_STANDBY:
        /* reset standby flag */
        PMU_CTL |= PMU_CTL_STBRST;
        break;
    default :
        break;
    }
}
