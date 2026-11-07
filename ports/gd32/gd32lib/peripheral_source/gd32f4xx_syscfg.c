/*!
    \file    gd32f4xx_syscfg.c
    \brief   SYSCFG driver

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

#include "gd32f4xx_syscfg.h"

/*!
    \brief    reset the SYSCFG registers
    \param[in]  none
    \param[out] none
    \retval     none
*/
void syscfg_deinit(void)
{
    rcu_periph_reset_enable(RCU_SYSCFGRST);
    rcu_periph_reset_disable(RCU_SYSCFGRST);
}

/*!
    \brief    configure the boot mode
    \param[in]  syscfg_bootmode: selects the memory remapping
                only one parameter can be selected which is shown as below:
      \arg        SYSCFG_BOOTMODE_FLASH: main flash memory (0x08000000~0x083BFFFF) is mapped at address 0x00000000
      \arg        SYSCFG_BOOTMODE_BOOTLOADER: boot loader (0x1FFF0000 - 0x1FFF77FF) is mapped at address 0x00000000
      \arg        SYSCFG_BOOTMODE_EXMC_SRAM: SRAM/NOR 0 and 1 of EXMC (0x60000000~0x67FFFFFF) is mapped at address 0x00000000
      \arg        SYSCFG_BOOTMODE_SRAM: SRAM0 of on-chip SRAM (0x20000000~0x2001BFFF) is mapped at address 0x00000000
      \arg        SYSCFG_BOOTMODE_EXMC_SDRAM: SDRAM bank0 of EXMC (0xC0000000~0xC7FFFFFF) is mapped at address 0x00000000
    \param[out] none
    \retval     none
*/
void syscfg_bootmode_config(uint8_t syscfg_bootmode)
{
    /* reset the SYSCFG_CFG0_BOOT_MODE bit and set according to syscfg_bootmode */
    SYSCFG_CFG0 &= ~SYSCFG_CFG0_BOOT_MODE;
    SYSCFG_CFG0 |= (uint32_t)syscfg_bootmode;
}

/*!
    \brief    FMC memory mapping swap
    \param[in]  syscfg_fmc_swap: selects the interal flash bank swapping
                only one parameter can be selected which is shown as below:
      \arg        SYSCFG_FMC_SWP_BANK0: bank 0 is mapped at address 0x08000000 and bank 1 is mapped at address 0x08100000
      \arg        SYSCFG_FMC_SWP_BANK1: bank 1 is mapped at address 0x08000000 and bank 0 is mapped at address 0x08100000
    \param[out] none
    \retval     none
*/
void syscfg_fmc_swap_config(uint32_t syscfg_fmc_swap)
{
    uint32_t reg;
    reg = SYSCFG_CFG0;
    /* reset the FMC_SWP bit and set according to syscfg_fmc_swap */
    reg &= ~SYSCFG_CFG0_FMC_SWP;
    SYSCFG_CFG0 = (reg | syscfg_fmc_swap);
}

/*!
    \brief    EXMC memory mapping swap
    \param[in]  syscfg_exmc_swap: selects the memories in EXMC swapping
                only one parameter can be selected which is shown as below:
      \arg        SYSCFG_EXMC_SWP_ENABLE: SDRAM bank 0 and bank 1 are swapped with NAND bank 1 and PC card
      \arg        SYSCFG_EXMC_SWP_DISABLE: no memory mapping swap
    \param[out] none
    \retval     none
*/
void syscfg_exmc_swap_config(uint32_t syscfg_exmc_swap)
{
    uint32_t reg;

    reg = SYSCFG_CFG0;
    /* reset the SYSCFG_CFG0_EXMC_SWP bits and set according to syscfg_exmc_swap */
    reg &= ~SYSCFG_CFG0_EXMC_SWP;
    SYSCFG_CFG0 = (reg | syscfg_exmc_swap);
}

/*!
    \brief    configure the GPIO pin as EXTI Line
    \param[in]  exti_port: specify the GPIO port used in EXTI
                only one parameter can be selected which is shown as below:
      \arg        EXTI_SOURCE_GPIOx(x = A,B,C,D,E,F,G,H,I): EXTI GPIO port
    \param[in]  exti_pin: specify the EXTI line
                only one parameter can be selected which is shown as below:
      \arg        EXTI_SOURCE_PINx(x = 0..15): EXTI GPIO pin
    \param[out] none
    \retval     none
*/
void syscfg_exti_line_config(uint8_t exti_port, uint8_t exti_pin)
{
    uint32_t clear_exti_mask = ~((uint32_t)EXTI_SS_MASK << (EXTI_SS_MSTEP(exti_pin)));
    uint32_t config_exti_mask = ((uint32_t)exti_port) << (EXTI_SS_MSTEP(exti_pin));

    switch(exti_pin / EXTI_SS_JSTEP) {
    case EXTISS0:
        /* clear EXTI source line(0..3) */
        SYSCFG_EXTISS0 &= clear_exti_mask;
        /* configure EXTI soure line(0..3) */
        SYSCFG_EXTISS0 |= config_exti_mask;
        break;
    case EXTISS1:
        /* clear EXTI soure line(4..7) */
        SYSCFG_EXTISS1 &= clear_exti_mask;
        /* configure EXTI soure line(4..7) */
        SYSCFG_EXTISS1 |= config_exti_mask;
        break;
    case EXTISS2:
        /* clear EXTI soure line(8..11) */
        SYSCFG_EXTISS2 &= clear_exti_mask;
        /* configure EXTI soure line(8..11) */
        SYSCFG_EXTISS2 |= config_exti_mask;
        break;
    case EXTISS3:
        /* clear EXTI soure line(12..15) */
        SYSCFG_EXTISS3 &= clear_exti_mask;
        /* configure EXTI soure line(12..15) */
        SYSCFG_EXTISS3 |= config_exti_mask;
        break;
    default:
        break;
    }
}

/*!
    \brief    configure the PHY interface for the ethernet MAC
    \param[in]  syscfg_enet_phy_interface: specifies the media interface mode.
                only one parameter can be selected which is shown as below:
      \arg        SYSCFG_ENET_PHY_MII: MII mode is selected
      \arg        SYSCFG_ENET_PHY_RMII: RMII mode is selected
    \param[out] none
    \retval     none
*/
void syscfg_enet_phy_interface_config(uint32_t syscfg_enet_phy_interface)
{
    uint32_t reg;

    reg = SYSCFG_CFG1;
    /* reset the ENET_PHY_SEL bit and set according to syscfg_enet_phy_interface */
    reg &= ~SYSCFG_CFG1_ENET_PHY_SEL;
    SYSCFG_CFG1 = (reg | syscfg_enet_phy_interface);
}

/*!
    \brief    configure the I/O compensation cell
    \param[in]  syscfg_compensation: specifies the I/O compensation cell mode
                only one parameter can be selected which is shown as below:
      \arg        SYSCFG_COMPENSATION_ENABLE: I/O compensation cell is enabled
      \arg        SYSCFG_COMPENSATION_DISABLE: I/O compensation cell is disabled
    \param[out] none
    \retval     none
*/
void syscfg_compensation_config(uint32_t syscfg_compensation)
{
    uint32_t reg;

    reg = SYSCFG_CPSCTL;
    /* reset the SYSCFG_CPSCTL_CPS_EN bit and set according to syscfg_compensation */
    reg &= ~SYSCFG_CPSCTL_CPS_EN;
    SYSCFG_CPSCTL = (reg | syscfg_compensation);
}

/*!
    \brief    checks whether the I/O compensation cell ready flag is set or not
    \param[in]  none
    \param[out] none
    \retval     FlagStatus: SET or RESET
  */
FlagStatus syscfg_flag_get(void)
{
    FlagStatus reval = RESET;

    if(((uint32_t)RESET) != (SYSCFG_CPSCTL & SYSCFG_CPSCTL_CPS_RDY)) {
        reval = SET;
    } else {
        reval = RESET;
    }

    return reval;
}
