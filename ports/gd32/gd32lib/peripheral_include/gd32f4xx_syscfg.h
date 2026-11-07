/*!
    \file    gd32f4xx_syscfg.h
    \brief   definitions for the SYSCFG
    
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

#ifndef GD32F4XX_SYSCFG_H
#define GD32F4XX_SYSCFG_H

#include "gd32f4xx.h"

/* SYSCFG definitions */
#define SYSCFG                              SYSCFG_BASE

/* registers definitions */
#define SYSCFG_CFG0                         REG32(SYSCFG + 0x00U)    /*!< system configuration register 0 */
#define SYSCFG_CFG1                         REG32(SYSCFG + 0x04U)    /*!< system configuration register 1 */
#define SYSCFG_EXTISS0                      REG32(SYSCFG + 0x08U)    /*!< EXTI sources selection register 0 */
#define SYSCFG_EXTISS1                      REG32(SYSCFG + 0x0CU)    /*!< EXTI sources selection register 1 */
#define SYSCFG_EXTISS2                      REG32(SYSCFG + 0x10U)    /*!< EXTI sources selection register 2 */
#define SYSCFG_EXTISS3                      REG32(SYSCFG + 0x14U)    /*!< EXTI sources selection register 3 */
#define SYSCFG_CPSCTL                       REG32(SYSCFG + 0x20U)    /*!< system I/O compensation control register */

/* SYSCFG_CFG0 bits definitions */
#define SYSCFG_CFG0_BOOT_MODE               BITS(0,2)                /*!< SYSCFG memory remap config */
#define SYSCFG_CFG0_FMC_SWP                 BIT(8)                   /*!< FMC memory swap config */
#define SYSCFG_CFG0_EXMC_SWP                BITS(10,11)              /*!< EXMC memory swap config */

/* SYSCFG_CFG1 bits definitions */
#define SYSCFG_CFG1_ENET_PHY_SEL            BIT(23)                  /*!< Ethernet PHY selection config */

/* SYSCFG_EXTISS0 bits definitions */
#define SYSCFG_EXTISS0_EXTI0_SS             BITS(0,3)                /*!< EXTI 0 configuration */
#define SYSCFG_EXTISS0_EXTI1_SS             BITS(4,7)                /*!< EXTI 1 configuration */
#define SYSCFG_EXTISS0_EXTI2_SS             BITS(8,11)               /*!< EXTI 2 configuration */
#define SYSCFG_EXTISS0_EXTI3_SS             BITS(12,15)              /*!< EXTI 3 configuration */

/* SYSCFG_EXTISS1 bits definitions */
#define SYSCFG_EXTISS1_EXTI4_SS             BITS(0,3)                /*!< EXTI 4 configuration */
#define SYSCFG_EXTISS1_EXTI5_SS             BITS(4,7)                /*!< EXTI 5 configuration */
#define SYSCFG_EXTISS1_EXTI6_SS             BITS(8,11)               /*!< EXTI 6 configuration */
#define SYSCFG_EXTISS1_EXTI7_SS             BITS(12,15)              /*!< EXTI 7 configuration */

/* SYSCFG_EXTISS2 bits definitions */
#define SYSCFG_EXTISS2_EXTI8_SS             BITS(0,3)                /*!< EXTI 8 configuration */
#define SYSCFG_EXTISS2_EXTI9_SS             BITS(4,7)                /*!< EXTI 9 configuration */
#define SYSCFG_EXTISS2_EXTI10_SS            BITS(8,11)               /*!< EXTI 10 configuration */
#define SYSCFG_EXTISS2_EXTI11_SS            BITS(12,15)              /*!< EXTI 11 configuration */

/* SYSCFG_EXTISS3 bits definitions */
#define SYSCFG_EXTISS3_EXTI12_SS            BITS(0,3)                /*!< EXTI 12 configuration */
#define SYSCFG_EXTISS3_EXTI13_SS            BITS(4,7)                /*!< EXTI 13 configuration */
#define SYSCFG_EXTISS3_EXTI14_SS            BITS(8,11)               /*!< EXTI 14 configuration */
#define SYSCFG_EXTISS3_EXTI15_SS            BITS(12,15)              /*!< EXTI 15 configuration */

/* SYSCFG_CPSCTL bits definitions */
#define SYSCFG_CPSCTL_CPS_EN                BIT(0)                   /*!< I/O compensation cell enable */
#define SYSCFG_CPSCTL_CPS_RDY               BIT(8)                   /*!< I/O compensation cell is ready or not */

/* constants definitions */
/* boot mode definitions */
#define SYSCFG_BOOTMODE_FLASH               ((uint8_t)0x00U)          /*!< main flash memory remap */
#define SYSCFG_BOOTMODE_BOOTLOADER          ((uint8_t)0x01U)          /*!< boot loader remap */
#define SYSCFG_BOOTMODE_EXMC_SRAM           ((uint8_t)0x02U)          /*!< SRAM/NOR 0 and 1 of EXMC remap */
#define SYSCFG_BOOTMODE_SRAM                ((uint8_t)0x03U)          /*!< SRAM0 of on-chip SRAM remap */
#define SYSCFG_BOOTMODE_EXMC_SDRAM          ((uint8_t)0x04U)          /*!< SDRAM bank0 of EXMC remap */

/* FMC swap definitions */
#define SYSCFG_FMC_SWP_BANK0                ((uint32_t)0x00000000U)   /*!< main flash Bank 0 is mapped at address 0x08000000 */
#define SYSCFG_FMC_SWP_BANK1                ((uint32_t)0x00000100U)   /*!< main flash Bank 1 is mapped at address 0x08000000 */

/* EXMC swap enable/disable */
#define SYSCFG_EXMC_SWP_ENABLE              ((uint32_t)0x00000400U)   /*!< SDRAM bank 0 and bank 1 are swapped with NAND bank 1 and PC card */
#define SYSCFG_EXMC_SWP_DISABLE             ((uint32_t)0x00000000U)   /*!< no memory mapping swap */

/* EXTI source select definition */
#define EXTISS0                             ((uint8_t)0x00U)          /*!< EXTI source select GPIOx pin 0~3 */
#define EXTISS1                             ((uint8_t)0x01U)          /*!< EXTI source select GPIOx pin 4~7 */
#define EXTISS2                             ((uint8_t)0x02U)          /*!< EXTI source select GPIOx pin 8~11 */
#define EXTISS3                             ((uint8_t)0x03U)          /*!< EXTI source select GPIOx pin 12~15 */

/* EXTI source select mask bits definition */
#define EXTI_SS_MASK                        BITS(0,3)                 /*!< EXTI source select mask */

/* EXTI source select jumping step definition */
#define EXTI_SS_JSTEP                       ((uint8_t)(0x04U))        /*!< EXTI source select jumping step */

/* EXTI source select moving step definition */
#define EXTI_SS_MSTEP(pin)                  (EXTI_SS_JSTEP*((pin)%EXTI_SS_JSTEP))   /*!< EXTI source select moving step */

/* EXTI source port definitions */
#define EXTI_SOURCE_GPIOA                   ((uint8_t)0x00U)          /*!< EXTI GPIOA configuration */
#define EXTI_SOURCE_GPIOB                   ((uint8_t)0x01U)          /*!< EXTI GPIOB configuration */
#define EXTI_SOURCE_GPIOC                   ((uint8_t)0x02U)          /*!< EXTI GPIOC configuration */
#define EXTI_SOURCE_GPIOD                   ((uint8_t)0x03U)          /*!< EXTI GPIOD configuration */
#define EXTI_SOURCE_GPIOE                   ((uint8_t)0x04U)          /*!< EXTI GPIOE configuration */
#define EXTI_SOURCE_GPIOF                   ((uint8_t)0x05U)          /*!< EXTI GPIOF configuration */
#define EXTI_SOURCE_GPIOG                   ((uint8_t)0x06U)          /*!< EXTI GPIOG configuration */
#define EXTI_SOURCE_GPIOH                   ((uint8_t)0x07U)          /*!< EXTI GPIOH configuration */
#define EXTI_SOURCE_GPIOI                   ((uint8_t)0x08U)          /*!< EXTI GPIOI configuration */

/* EXTI source pin definitions */
#define EXTI_SOURCE_PIN0                    ((uint8_t)0x00U)          /*!< EXTI GPIO pin0 configuration */
#define EXTI_SOURCE_PIN1                    ((uint8_t)0x01U)          /*!< EXTI GPIO pin1 configuration */
#define EXTI_SOURCE_PIN2                    ((uint8_t)0x02U)          /*!< EXTI GPIO pin2 configuration */
#define EXTI_SOURCE_PIN3                    ((uint8_t)0x03U)          /*!< EXTI GPIO pin3 configuration */
#define EXTI_SOURCE_PIN4                    ((uint8_t)0x04U)          /*!< EXTI GPIO pin4 configuration */
#define EXTI_SOURCE_PIN5                    ((uint8_t)0x05U)          /*!< EXTI GPIO pin5 configuration */
#define EXTI_SOURCE_PIN6                    ((uint8_t)0x06U)          /*!< EXTI GPIO pin6 configuration */
#define EXTI_SOURCE_PIN7                    ((uint8_t)0x07U)          /*!< EXTI GPIO pin7 configuration */
#define EXTI_SOURCE_PIN8                    ((uint8_t)0x08U)          /*!< EXTI GPIO pin8 configuration */
#define EXTI_SOURCE_PIN9                    ((uint8_t)0x09U)          /*!< EXTI GPIO pin9 configuration */
#define EXTI_SOURCE_PIN10                   ((uint8_t)0x0AU)          /*!< EXTI GPIO pin10 configuration */
#define EXTI_SOURCE_PIN11                   ((uint8_t)0x0BU)          /*!< EXTI GPIO pin11 configuration */
#define EXTI_SOURCE_PIN12                   ((uint8_t)0x0CU)          /*!< EXTI GPIO pin12 configuration */
#define EXTI_SOURCE_PIN13                   ((uint8_t)0x0DU)          /*!< EXTI GPIO pin13 configuration */
#define EXTI_SOURCE_PIN14                   ((uint8_t)0x0EU)          /*!< EXTI GPIO pin14 configuration */
#define EXTI_SOURCE_PIN15                   ((uint8_t)0x0FU)          /*!< EXTI GPIO pin15 configuration */

/* ethernet PHY selection */
#define SYSCFG_ENET_PHY_MII                 ((uint32_t)0x00000000U)   /*!< MII is selected for the Ethernet MAC */
#define SYSCFG_ENET_PHY_RMII                ((uint32_t)0x00800000U)   /*!< RMII is selected for the Ethernet MAC */

/* I/O compensation cell enable/disable */
#define SYSCFG_COMPENSATION_ENABLE          ((uint32_t)0x00000001U)   /*!< I/O compensation cell enable */
#define SYSCFG_COMPENSATION_DISABLE         ((uint32_t)0x00000000U)   /*!< I/O compensation cell disable */

/* function declarations */
/* initialization functions */
/* deinit syscfg module */
void syscfg_deinit(void);

/* function configuration */
/* configure the boot mode */
void syscfg_bootmode_config(uint8_t syscfg_bootmode);
/* configure FMC memory mapping swap */
void syscfg_fmc_swap_config(uint32_t syscfg_fmc_swap);
/* configure the EXMC swap */
void syscfg_exmc_swap_config(uint32_t syscfg_exmc_swap); 
/* configure the GPIO pin as EXTI Line */
void syscfg_exti_line_config(uint8_t exti_port, uint8_t exti_pin);
/* configure the PHY interface for the ethernet MAC */
void syscfg_enet_phy_interface_config(uint32_t syscfg_enet_phy_interface);
/* configure the I/O compensation cell */
void syscfg_compensation_config(uint32_t syscfg_compensation);

/* interrupt & flag functions */
/* check the I/O compensation cell is ready or not */
FlagStatus syscfg_flag_get(void);

#endif /* GD32F4XX_SYSCFG_H */
