/**
  ******************************************************************************
  * @file    stm32l4xx_hal.h
  * @author  MCD Application Team
  * @version V1.3.0
  * @date    29-January-2016
  * @brief   This file contains all the functions prototypes for the HAL
  *          module driver.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32L4xx_HAL_H
#define __STM32L4xx_HAL_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal_conf.h"

/** @addtogroup STM32L4xx_HAL_Driver
  * @{
  */

/** @addtogroup HAL
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/** @defgroup SYSCFG_Exported_Constants SYSCFG Exported Constants
  * @{
  */

/** @defgroup SYSCFG_BootMode Boot Mode
  * @{
  */
#define SYSCFG_BOOT_MAINFLASH          ((uint32_t)0x00000000)
#define SYSCFG_BOOT_SYSTEMFLASH        SYSCFG_MEMRMP_MEM_MODE_0
#define SYSCFG_BOOT_FMC                SYSCFG_MEMRMP_MEM_MODE_1
#define SYSCFG_BOOT_SRAM               (SYSCFG_MEMRMP_MEM_MODE_1 | SYSCFG_MEMRMP_MEM_MODE_0)
#define SYSCFG_BOOT_QUADSPI            (SYSCFG_MEMRMP_MEM_MODE_2 | SYSCFG_MEMRMP_MEM_MODE_1)

/**
  * @}
  */

/** @defgroup SYSCFG_FPU_Interrupts FPU Interrupts
  * @{
  */
#define SYSCFG_IT_FPU_IOC              SYSCFG_CFGR1_FPU_IE_0  /*!< Floating Point Unit Invalid operation Interrupt */
#define SYSCFG_IT_FPU_DZC              SYSCFG_CFGR1_FPU_IE_1  /*!< Floating Point Unit Divide-by-zero Interrupt */
#define SYSCFG_IT_FPU_UFC              SYSCFG_CFGR1_FPU_IE_2  /*!< Floating Point Unit Underflow Interrupt */
#define SYSCFG_IT_FPU_OFC              SYSCFG_CFGR1_FPU_IE_3  /*!< Floating Point Unit Overflow Interrupt */
#define SYSCFG_IT_FPU_IDC              SYSCFG_CFGR1_FPU_IE_4  /*!< Floating Point Unit Input denormal Interrupt */
#define SYSCFG_IT_FPU_IXC              SYSCFG_CFGR1_FPU_IE_5  /*!< Floating Point Unit Inexact Interrupt */

/**
  * @}
  */

/** @defgroup SYSCFG_SRAM2WRP SRAM2 Write protection
  * @{
  */
#define SYSCFG_SRAM2WRP_PAGE0          SYSCFG_SWPR_PAGE0 /*!< SRAM2 Write protection page 0 */
#define SYSCFG_SRAM2WRP_PAGE1          SYSCFG_SWPR_PAGE1 /*!< SRAM2 Write protection page 1 */
#define SYSCFG_SRAM2WRP_PAGE2          SYSCFG_SWPR_PAGE2 /*!< SRAM2 Write protection page 2 */
#define SYSCFG_SRAM2WRP_PAGE3          SYSCFG_SWPR_PAGE3 /*!< SRAM2 Write protection page 3 */
#define SYSCFG_SRAM2WRP_PAGE4          SYSCFG_SWPR_PAGE4 /*!< SRAM2 Write protection page 4 */
#define SYSCFG_SRAM2WRP_PAGE5          SYSCFG_SWPR_PAGE5 /*!< SRAM2 Write protection page 5 */
#define SYSCFG_SRAM2WRP_PAGE6          SYSCFG_SWPR_PAGE6 /*!< SRAM2 Write protection page 6 */
#define SYSCFG_SRAM2WRP_PAGE7          SYSCFG_SWPR_PAGE7 /*!< SRAM2 Write protection page 7 */
#define SYSCFG_SRAM2WRP_PAGE8          SYSCFG_SWPR_PAGE8 /*!< SRAM2 Write protection page 8 */
#define SYSCFG_SRAM2WRP_PAGE9          SYSCFG_SWPR_PAGE9 /*!< SRAM2 Write protection page 9 */
#define SYSCFG_SRAM2WRP_PAGE10         SYSCFG_SWPR_PAGE10 /*!< SRAM2 Write protection page 10 */
#define SYSCFG_SRAM2WRP_PAGE11         SYSCFG_SWPR_PAGE11 /*!< SRAM2 Write protection page 11 */
#define SYSCFG_SRAM2WRP_PAGE12         SYSCFG_SWPR_PAGE12 /*!< SRAM2 Write protection page 12 */
#define SYSCFG_SRAM2WRP_PAGE13         SYSCFG_SWPR_PAGE13 /*!< SRAM2 Write protection page 13 */
#define SYSCFG_SRAM2WRP_PAGE14         SYSCFG_SWPR_PAGE14 /*!< SRAM2 Write protection page 14 */
#define SYSCFG_SRAM2WRP_PAGE15         SYSCFG_SWPR_PAGE15 /*!< SRAM2 Write protection page 15 */
#define SYSCFG_SRAM2WRP_PAGE16         SYSCFG_SWPR_PAGE16 /*!< SRAM2 Write protection page 16 */
#define SYSCFG_SRAM2WRP_PAGE17         SYSCFG_SWPR_PAGE17 /*!< SRAM2 Write protection page 17 */
#define SYSCFG_SRAM2WRP_PAGE18         SYSCFG_SWPR_PAGE18 /*!< SRAM2 Write protection page 18 */
#define SYSCFG_SRAM2WRP_PAGE19         SYSCFG_SWPR_PAGE19 /*!< SRAM2 Write protection page 19 */
#define SYSCFG_SRAM2WRP_PAGE20         SYSCFG_SWPR_PAGE20 /*!< SRAM2 Write protection page 20 */
#define SYSCFG_SRAM2WRP_PAGE21         SYSCFG_SWPR_PAGE21 /*!< SRAM2 Write protection page 21 */
#define SYSCFG_SRAM2WRP_PAGE22         SYSCFG_SWPR_PAGE22 /*!< SRAM2 Write protection page 22 */
#define SYSCFG_SRAM2WRP_PAGE23         SYSCFG_SWPR_PAGE23 /*!< SRAM2 Write protection page 23 */
#define SYSCFG_SRAM2WRP_PAGE24         SYSCFG_SWPR_PAGE24 /*!< SRAM2 Write protection page 24 */
#define SYSCFG_SRAM2WRP_PAGE25         SYSCFG_SWPR_PAGE25 /*!< SRAM2 Write protection page 25 */
#define SYSCFG_SRAM2WRP_PAGE26         SYSCFG_SWPR_PAGE26 /*!< SRAM2 Write protection page 26 */
#define SYSCFG_SRAM2WRP_PAGE27         SYSCFG_SWPR_PAGE27 /*!< SRAM2 Write protection page 27 */
#define SYSCFG_SRAM2WRP_PAGE28         SYSCFG_SWPR_PAGE28 /*!< SRAM2 Write protection page 28 */
#define SYSCFG_SRAM2WRP_PAGE29         SYSCFG_SWPR_PAGE29 /*!< SRAM2 Write protection page 29 */
#define SYSCFG_SRAM2WRP_PAGE30         SYSCFG_SWPR_PAGE30 /*!< SRAM2 Write protection page 30 */
#define SYSCFG_SRAM2WRP_PAGE31         SYSCFG_SWPR_PAGE31 /*!< SRAM2 Write protection page 31 */

/**
  * @}
  */

#if defined(VREFBUF)
/** @defgroup SYSCFG_VREFBUF_VoltageScale VREFBUF Voltage Scale
  * @{
  */
#define SYSCFG_VREFBUF_VOLTAGE_SCALE0  ((uint32_t)0x00000000) /*!< Voltage reference scale 0 (VREF_OUT1) */
#define SYSCFG_VREFBUF_VOLTAGE_SCALE1  VREFBUF_CSR_VRS        /*!< Voltage reference scale 1 (VREF_OUT2) */

/**
  * @}
  */

/** @defgroup SYSCFG_VREFBUF_HighImpedance VREFBUF High Impedance
  * @{
  */
#define SYSCFG_VREFBUF_HIGH_IMPEDANCE_DISABLE  ((uint32_t)0x00000000) /*!< VREF_plus pin is internally connected to Voltage reference buffer output */
#define SYSCFG_VREFBUF_HIGH_IMPEDANCE_ENABLE   VREFBUF_CSR_HIZ        /*!< VREF_plus pin is high impedance */

/**
  * @}
  */
#endif /* VREFBUF */

/** @defgroup SYSCFG_flags_definition Flags
  * @{
  */

#define SYSCFG_FLAG_SRAM2_PE            SYSCFG_CFGR2_SPF       /*!< SRAM2 parity error */
#define SYSCFG_FLAG_SRAM2_BUSY          SYSCFG_SCSR_SRAM2BSY   /*!< SRAM2 busy by erase operation */

/**
  * @}
  */

/** @defgroup SYSCFG_FastModePlus_GPIO Fast-mode Plus on GPIO
  * @{
  */

/** @brief  Fast-mode Plus driving capability on a specific GPIO
  */  
#define SYSCFG_FASTMODEPLUS_PB6        SYSCFG_CFGR1_I2C_PB6_FMP  /*!< Enable Fast-mode Plus on PB6 */
#define SYSCFG_FASTMODEPLUS_PB7        SYSCFG_CFGR1_I2C_PB7_FMP  /*!< Enable Fast-mode Plus on PB7 */
#if defined(SYSCFG_CFGR1_I2C_PB8_FMP)
#define SYSCFG_FASTMODEPLUS_PB8        SYSCFG_CFGR1_I2C_PB8_FMP  /*!< Enable Fast-mode Plus on PB8 */
#endif /* SYSCFG_CFGR1_I2C_PB8_FMP */
#if defined(SYSCFG_CFGR1_I2C_PB9_FMP)
#define SYSCFG_FASTMODEPLUS_PB9        SYSCFG_CFGR1_I2C_PB9_FMP  /*!< Enable Fast-mode Plus on PB9 */
#endif /* SYSCFG_CFGR1_I2C_PB9_FMP */

/**
 * @}
 */

/**
  * @}
  */

/* Exported macros -----------------------------------------------------------*/

/** @defgroup DBGMCU_Exported_Macros DBGMCU Exported Macros
  * @{
  */

/** @brief  Freeze/Unfreeze Peripherals in Debug mode
  */
#if defined(DBGMCU_APB1FZR1_DBG_TIM2_STOP)
#define __HAL_DBGMCU_FREEZE_TIM2()           SET_BIT(DBGMCU->APB1FZR1, DBGMCU_APB1FZR1_DBG_TIM2_STOP)
#define __HAL_DBGMCU_UNFREEZE_TIM2()         CLEAR_BIT(DBGMCU->APB1FZR1, DBGMCU_APB1FZR1_DBG_TIM2_STOP)
#endif

#if defined(DBGMCU_APB1FZR1_DBG_TIM3_STOP)
#define __HAL_DBGMCU_FREEZE_TIM3()           SET_BIT(DBGMCU->APB1FZR1, DBGMCU_APB1FZR1_DBG_TIM3_STOP)
#define __HAL_DBGMCU_UNFREEZE_TIM3()         CLEAR_BIT(DBGMCU->APB1FZR1, DBGMCU_APB1FZR1_DBG_TIM3_STOP)
#endif

#if defined(DBGMCU_APB1FZR1_DBG_TIM4_STOP)
#define __HAL_DBGMCU_FREEZE_TIM4()           SET_BIT(DBGMCU->APB1FZR1, DBGMCU_APB1FZR1_DBG_TIM4_STOP)
#define __HAL_DBGMCU_UNFREEZE_TIM4()         CLEAR_BIT(DBGMCU->APB1FZR1, DBGMCU_APB1FZR1_DBG_TIM4_STOP)
#endif

#if defined(DBGMCU_APB1FZR1_DBG_TIM5_STOP)
#define __HAL_DBGMCU_FREEZE_TIM5()           SET_BIT(DBGMCU->APB1FZR1, DBGMCU_APB1FZR1_DBG_TIM5_STOP)
#define __HAL_DBGMCU_UNFREEZE_TIM5()         CLEAR_BIT(DBGMCU->APB1FZR1, DBGMCU_APB1FZR1_DBG_TIM5_STOP)
#endif

#if defined(DBGMCU_APB1FZR1_DBG_TIM6_STOP)
#define __HAL_DBGMCU_FREEZE_TIM6()           SET_BIT(DBGMCU->APB1FZR1, DBGMCU_APB1FZR1_DBG_TIM6_STOP)
#define __HAL_DBGMCU_UNFREEZE_TIM6()         CLEAR_BIT(DBGMCU->APB1FZR1, DBGMCU_APB1FZR1_DBG_TIM6_STOP)
#endif

#if defined(DBGMCU_APB1FZR1_DBG_TIM7_STOP)
#define __HAL_DBGMCU_FREEZE_TIM7()           SET_BIT(DBGMCU->APB1FZR1, DBGMCU_APB1FZR1_DBG_TIM7_STOP)
#define __HAL_DBGMCU_UNFREEZE_TIM7()         CLEAR_BIT(DBGMCU->APB1FZR1, DBGMCU_APB1FZR1_DBG_TIM7_STOP)
#endif

#if defined(DBGMCU_APB1FZR1_DBG_RTC_STOP)
#define __HAL_DBGMCU_FREEZE_RTC()            SET_BIT(DBGMCU->APB1FZR1, DBGMCU_APB1FZR1_DBG_RTC_STOP)
#define __HAL_DBGMCU_UNFREEZE_RTC()          CLEAR_BIT(DBGMCU->APB1FZR1, DBGMCU_APB1FZR1_DBG_RTC_STOP)
#endif

#if defined(DBGMCU_APB1FZR1_DBG_WWDG_STOP)
#define __HAL_DBGMCU_FREEZE_WWDG()           SET_BIT(DBGMCU->APB1FZR1, DBGMCU_APB1FZR1_DBG_WWDG_STOP)
#define __HAL_DBGMCU_UNFREEZE_WWDG()         CLEAR_BIT(DBGMCU->APB1FZR1, DBGMCU_APB1FZR1_DBG_WWDG_STOP)
#endif

#if defined(DBGMCU_APB1FZR1_DBG_IWDG_STOP)
#define __HAL_DBGMCU_FREEZE_IWDG()           SET_BIT(DBGMCU->APB1FZR1, DBGMCU_APB1FZR1_DBG_IWDG_STOP)
#define __HAL_DBGMCU_UNFREEZE_IWDG()         CLEAR_BIT(DBGMCU->APB1FZR1, DBGMCU_APB1FZR1_DBG_IWDG_STOP)
#endif

#if defined(DBGMCU_APB1FZR1_DBG_I2C1_STOP)
#define __HAL_DBGMCU_FREEZE_I2C1_TIMEOUT()   SET_BIT(DBGMCU->APB1FZR1, DBGMCU_APB1FZR1_DBG_I2C1_STOP)
#define __HAL_DBGMCU_UNFREEZE_I2C1_TIMEOUT() CLEAR_BIT(DBGMCU->APB1FZR1, DBGMCU_APB1FZR1_DBG_I2C1_STOP)
#endif

#if defined(DBGMCU_APB1FZR1_DBG_I2C2_STOP)
#define __HAL_DBGMCU_FREEZE_I2C2_TIMEOUT()   SET_BIT(DBGMCU->APB1FZR1, DBGMCU_APB1FZR1_DBG_I2C2_STOP)
#define __HAL_DBGMCU_UNFREEZE_I2C2_TIMEOUT() CLEAR_BIT(DBGMCU->APB1FZR1, DBGMCU_APB1FZR1_DBG_I2C2_STOP)
#endif

#if defined(DBGMCU_APB1FZR1_DBG_I2C3_STOP)
#define __HAL_DBGMCU_FREEZE_I2C3_TIMEOUT()   SET_BIT(DBGMCU->APB1FZR1, DBGMCU_APB1FZR1_DBG_I2C3_STOP)
#define __HAL_DBGMCU_UNFREEZE_I2C3_TIMEOUT() CLEAR_BIT(DBGMCU->APB1FZR1, DBGMCU_APB1FZR1_DBG_I2C3_STOP)
#endif

#if defined(DBGMCU_APB1FZR1_DBG_CAN_STOP)
#define __HAL_DBGMCU_FREEZE_CAN1()           SET_BIT(DBGMCU->APB1FZR1, DBGMCU_APB1FZR1_DBG_CAN_STOP)
#define __HAL_DBGMCU_UNFREEZE_CAN1()         CLEAR_BIT(DBGMCU->APB1FZR1, DBGMCU_APB1FZR1_DBG_CAN_STOP)
#endif

#if defined(DBGMCU_APB1FZR1_DBG_LPTIM1_STOP)
#define __HAL_DBGMCU_FREEZE_LPTIM1()         SET_BIT(DBGMCU->APB1FZR1, DBGMCU_APB1FZR1_DBG_LPTIM1_STOP)
#define __HAL_DBGMCU_UNFREEZE_LPTIM1()       CLEAR_BIT(DBGMCU->APB1FZR1, DBGMCU_APB1FZR1_DBG_LPTIM1_STOP)
#endif

#if defined(DBGMCU_APB1FZR2_DBG_LPTIM2_STOP)
#define __HAL_DBGMCU_FREEZE_LPTIM2()         SET_BIT(DBGMCU->APB1FZR2, DBGMCU_APB1FZR2_DBG_LPTIM2_STOP)
#define __HAL_DBGMCU_UNFREEZE_LPTIM2()       CLEAR_BIT(DBGMCU->APB1FZR2, DBGMCU_APB1FZR2_DBG_LPTIM2_STOP)
#endif

#if defined(DBGMCU_APB2FZ_DBG_TIM1_STOP)
#define __HAL_DBGMCU_FREEZE_TIM1()           SET_BIT(DBGMCU->APB2FZ, DBGMCU_APB2FZ_DBG_TIM1_STOP)
#define __HAL_DBGMCU_UNFREEZE_TIM1()         CLEAR_BIT(DBGMCU->APB2FZ, DBGMCU_APB2FZ_DBG_TIM1_STOP)
#endif

#if defined(DBGMCU_APB2FZ_DBG_TIM8_STOP)
#define __HAL_DBGMCU_FREEZE_TIM8()           SET_BIT(DBGMCU->APB2FZ, DBGMCU_APB2FZ_DBG_TIM8_STOP)
#define __HAL_DBGMCU_UNFREEZE_TIM8()         CLEAR_BIT(DBGMCU->APB2FZ, DBGMCU_APB2FZ_DBG_TIM8_STOP)
#endif

#if defined(DBGMCU_APB2FZ_DBG_TIM15_STOP)
#define __HAL_DBGMCU_FREEZE_TIM15()          SET_BIT(DBGMCU->APB2FZ, DBGMCU_APB2FZ_DBG_TIM15_STOP)
#define __HAL_DBGMCU_UNFREEZE_TIM15()        CLEAR_BIT(DBGMCU->APB2FZ, DBGMCU_APB2FZ_DBG_TIM15_STOP)
#endif

#if defined(DBGMCU_APB2FZ_DBG_TIM16_STOP)
#define __HAL_DBGMCU_FREEZE_TIM16()          SET_BIT(DBGMCU->APB2FZ, DBGMCU_APB2FZ_DBG_TIM16_STOP)
#define __HAL_DBGMCU_UNFREEZE_TIM16()        CLEAR_BIT(DBGMCU->APB2FZ, DBGMCU_APB2FZ_DBG_TIM16_STOP)
#endif

#if defined(DBGMCU_APB2FZ_DBG_TIM17_STOP)
#define __HAL_DBGMCU_FREEZE_TIM17()          SET_BIT(DBGMCU->APB2FZ, DBGMCU_APB2FZ_DBG_TIM17_STOP)
#define __HAL_DBGMCU_UNFREEZE_TIM17()        CLEAR_BIT(DBGMCU->APB2FZ, DBGMCU_APB2FZ_DBG_TIM17_STOP)
#endif

/**
  * @}
  */

/** @defgroup SYSCFG_Exported_Macros SYSCFG Exported Macros
  * @{
  */

/** @brief  Main Flash memory mapped at 0x00000000.
  */
#define __HAL_SYSCFG_REMAPMEMORY_FLASH()       CLEAR_BIT(SYSCFG->MEMRMP, SYSCFG_MEMRMP_MEM_MODE)

/** @brief  System Flash memory mapped at 0x00000000.
  */
#define __HAL_SYSCFG_REMAPMEMORY_SYSTEMFLASH() MODIFY_REG(SYSCFG->MEMRMP, SYSCFG_MEMRMP_MEM_MODE, SYSCFG_MEMRMP_MEM_MODE_0)

/** @brief  Embedded SRAM mapped at 0x00000000.
  */
#define __HAL_SYSCFG_REMAPMEMORY_SRAM()        MODIFY_REG(SYSCFG->MEMRMP, SYSCFG_MEMRMP_MEM_MODE, (SYSCFG_MEMRMP_MEM_MODE_1|SYSCFG_MEMRMP_MEM_MODE_0))

/** @brief  FMC Bank1 (NOR/PSRAM 1 and 2) mapped at 0x00000000.
  */
#define __HAL_SYSCFG_REMAPMEMORY_FMC()         MODIFY_REG(SYSCFG->MEMRMP, SYSCFG_MEMRMP_MEM_MODE, SYSCFG_MEMRMP_MEM_MODE_1)

/** @brief  QUADSPI mapped at 0x00000000.
  */
#define __HAL_SYSCFG_REMAPMEMORY_QUADSPI()     MODIFY_REG(SYSCFG->MEMRMP, SYSCFG_MEMRMP_MEM_MODE, (SYSCFG_MEMRMP_MEM_MODE_2|SYSCFG_MEMRMP_MEM_MODE_1))

/**
  * @brief  Return the boot mode as configured by user.
  * @retval The boot mode as configured by user. The returned value can be one
  *         of the following values:
  *           @arg @ref SYSCFG_BOOT_MAINFLASH
  *           @arg @ref SYSCFG_BOOT_SYSTEMFLASH
  *           @arg @ref SYSCFG_BOOT_FMC
  *           @arg @ref SYSCFG_BOOT_SRAM
  *           @arg @ref SYSCFG_BOOT_QUADSPI
  */
#define __HAL_SYSCFG_GET_BOOT_MODE()           READ_BIT(SYSCFG->MEMRMP, SYSCFG_MEMRMP_MEM_MODE)

/** @brief  SRAM2 page write protection enable macro
  * @param __SRAM2WRP__: This parameter can be a value of @ref SYSCFG_SRAM2WRP
  * @note   write protection can only be disabled by a system reset
  */
#define __HAL_SYSCFG_SRAM2_WRP_ENABLE(__SRAM2WRP__)   do {assert_param(IS_SYSCFG_SRAM2WRP_PAGE((__SRAM2WRP__)));\
                                                         SET_BIT(SYSCFG->SWPR, (__SRAM2WRP__));\
                                                        }while(0)

/** @brief  SRAM2 page write protection unlock prior to erase
  * @note   Writing a wrong key reactivates the write protection
  */
#define __HAL_SYSCFG_SRAM2_WRP_UNLOCK()      do {SYSCFG->SKR = 0xCA;\
                                                 SYSCFG->SKR = 0x53;\
                                                }while(0)

/** @brief  SRAM2 erase
  * @note   __SYSCFG_GET_FLAG(SYSCFG_FLAG_SRAM2_BUSY) may be used to check end of erase
  */
#define __HAL_SYSCFG_SRAM2_ERASE()           SET_BIT(SYSCFG->SCSR, SYSCFG_SCSR_SRAM2ER)

/** @brief  Floating Point Unit interrupt enable/disable macros
  * @param __INTERRUPT__: This parameter can be a value of @ref SYSCFG_FPU_Interrupts
  */
#define __HAL_SYSCFG_FPU_INTERRUPT_ENABLE(__INTERRUPT__)    do {assert_param(IS_SYSCFG_FPU_INTERRUPT((__INTERRUPT__)));\
                                                                SET_BIT(SYSCFG->CFGR1, (__INTERRUPT__));\
                                                            }while(0)

#define __HAL_SYSCFG_FPU_INTERRUPT_DISABLE(__INTERRUPT__)   do {assert_param(IS_SYSCFG_FPU_INTERRUPT((__INTERRUPT__)));\
                                                                CLEAR_BIT(SYSCFG->CFGR1, (__INTERRUPT__));\
                                                            }while(0)

/** @brief  SYSCFG Break ECC lock.
  *         Enable and lock the connection of Flash ECC error connection to TIM1/8/15/16/17 Break input.
  * @note   The selected configuration is locked and can be unlocked only by system reset.
  */
#define __HAL_SYSCFG_BREAK_ECC_LOCK()        SET_BIT(SYSCFG->CFGR2, SYSCFG_CFGR2_ECCL)

/** @brief  SYSCFG Break Cortex-M4 Lockup lock.
  *         Enable and lock the connection of Cortex-M4 LOCKUP (Hardfault) output to TIM1/8/15/16/17 Break input.
  * @note   The selected configuration is locked and can be unlocked only by system reset.
  */
#define __HAL_SYSCFG_BREAK_LOCKUP_LOCK()     SET_BIT(SYSCFG->CFGR2, SYSCFG_CFGR2_CLL)

/** @brief  SYSCFG Break PVD lock.
  *         Enable and lock the PVD connection to Timer1/8/15/16/17 Break input, as well as the PVDE and PLS[2:0] in the PWR_CR2 register.
  * @note   The selected configuration is locked and can be unlocked only by system reset.
  */
#define __HAL_SYSCFG_BREAK_PVD_LOCK()        SET_BIT(SYSCFG->CFGR2, SYSCFG_CFGR2_PVDL)

/** @brief  SYSCFG Break SRAM2 parity lock.
  *         Enable and lock the SRAM2 parity error signal connection to TIM1/8/15/16/17 Break input.
  * @note   The selected configuration is locked and can be unlocked by system reset.
  */
#define __HAL_SYSCFG_BREAK_SRAM2PARITY_LOCK()  SET_BIT(SYSCFG->CFGR2, SYSCFG_CFGR2_SPL)

/** @brief  Check SYSCFG flag is set or not.
  * @param  __FLAG__: specifies the flag to check.
  *         This parameter can be one of the following values:
  *            @arg @ref SYSCFG_FLAG_SRAM2_PE   SRAM2 Parity Error Flag
  *            @arg @ref SYSCFG_FLAG_SRAM2_BUSY SRAM2 Erase Ongoing
  * @retval The new state of __FLAG__ (TRUE or FALSE).
  */
#define __HAL_SYSCFG_GET_FLAG(__FLAG__)      ((((((__FLAG__) == SYSCFG_SCSR_SRAM2BSY)? SYSCFG->SCSR : SYSCFG->CFGR2) & (__FLAG__))!= 0) ? 1 : 0)

/** @brief  Set the SPF bit to clear the SRAM Parity Error Flag.
  */
#define __HAL_SYSCFG_CLEAR_FLAG()            SET_BIT(SYSCFG->CFGR2, SYSCFG_CFGR2_SPF)

/** @brief  Fast-mode Plus driving capability enable/disable macros
  * @param __FASTMODEPLUS__: This parameter can be a value of : 
  *     @arg @ref SYSCFG_FASTMODEPLUS_PB6 Fast-mode Plus driving capability activation on PB6
  *     @arg @ref SYSCFG_FASTMODEPLUS_PB7 Fast-mode Plus driving capability activation on PB7 
  *     @arg @ref SYSCFG_FASTMODEPLUS_PB8 Fast-mode Plus driving capability activation on PB8
  *     @arg @ref SYSCFG_FASTMODEPLUS_PB9 Fast-mode Plus driving capability activation on PB9
  */
#define __HAL_SYSCFG_FASTMODEPLUS_ENABLE(__FASTMODEPLUS__)  do {assert_param(IS_SYSCFG_FASTMODEPLUS((__FASTMODEPLUS__)));\
                                                                SET_BIT(SYSCFG->CFGR1, (__FASTMODEPLUS__));\
                                                               }while(0)

#define __HAL_SYSCFG_FASTMODEPLUS_DISABLE(__FASTMODEPLUS__) do {assert_param(IS_SYSCFG_FASTMODEPLUS((__FASTMODEPLUS__)));\
                                                                CLEAR_BIT(SYSCFG->CFGR1, (__FASTMODEPLUS__));\
                                                               }while(0)

/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/** @defgroup SYSCFG_Private_Macros SYSCFG Private Macros
  * @{
  */

#define IS_SYSCFG_FPU_INTERRUPT(__INTERRUPT__) ((((__INTERRUPT__) & SYSCFG_IT_FPU_IOC) == SYSCFG_IT_FPU_IOC) || \
                                                (((__INTERRUPT__) & SYSCFG_IT_FPU_DZC) == SYSCFG_IT_FPU_DZC) || \
                                                (((__INTERRUPT__) & SYSCFG_IT_FPU_UFC) == SYSCFG_IT_FPU_UFC) || \
                                                (((__INTERRUPT__) & SYSCFG_IT_FPU_OFC) == SYSCFG_IT_FPU_OFC) || \
                                                (((__INTERRUPT__) & SYSCFG_IT_FPU_IDC) == SYSCFG_IT_FPU_IDC) || \
                                                (((__INTERRUPT__) & SYSCFG_IT_FPU_IXC) == SYSCFG_IT_FPU_IXC))

#define IS_SYSCFG_BREAK_CONFIG(__CONFIG__) (((__CONFIG__) == SYSCFG_BREAK_ECC)           || \
                                            ((__CONFIG__) == SYSCFG_BREAK_PVD)           || \
                                            ((__CONFIG__) == SYSCFG_BREAK_SRAM2_PARITY)  || \
                                            ((__CONFIG__) == SYSCFG_BREAK_LOCKUP))

#define IS_SYSCFG_SRAM2WRP_PAGE(__PAGE__)   (((__PAGE__) > 0) && ((__PAGE__) <= 0xFFFFFFFF))

#if defined(VREFBUF)
#define IS_SYSCFG_VREFBUF_VOLTAGE_SCALE(__SCALE__)  (((__SCALE__) == SYSCFG_VREFBUF_VOLTAGE_SCALE0) || \
                                                     ((__SCALE__) == SYSCFG_VREFBUF_VOLTAGE_SCALE1))

#define IS_SYSCFG_VREFBUF_HIGH_IMPEDANCE(__VALUE__)  (((__VALUE__) == SYSCFG_VREFBUF_HIGH_IMPEDANCE_DISABLE) || \
                                                      ((__VALUE__) == SYSCFG_VREFBUF_HIGH_IMPEDANCE_ENABLE))

#define IS_SYSCFG_VREFBUF_TRIMMING(__VALUE__)  (((__VALUE__) > 0) && ((__VALUE__) <= VREFBUF_CCR_TRIM))
#endif /* VREFBUF */

#if defined(SYSCFG_FASTMODEPLUS_PB8) && defined(SYSCFG_FASTMODEPLUS_PB9)
#define IS_SYSCFG_FASTMODEPLUS(__PIN__) ((((__PIN__) & SYSCFG_FASTMODEPLUS_PB6) == SYSCFG_FASTMODEPLUS_PB6) || \
                                         (((__PIN__) & SYSCFG_FASTMODEPLUS_PB7) == SYSCFG_FASTMODEPLUS_PB7) || \
                                         (((__PIN__) & SYSCFG_FASTMODEPLUS_PB8) == SYSCFG_FASTMODEPLUS_PB8) || \
                                         (((__PIN__) & SYSCFG_FASTMODEPLUS_PB9) == SYSCFG_FASTMODEPLUS_PB9))
#elif defined(SYSCFG_FASTMODEPLUS_PB8)
#define IS_SYSCFG_FASTMODEPLUS(__PIN__) ((((__PIN__) & SYSCFG_FASTMODEPLUS_PB6) == SYSCFG_FASTMODEPLUS_PB6) || \
                                         (((__PIN__) & SYSCFG_FASTMODEPLUS_PB7) == SYSCFG_FASTMODEPLUS_PB7) || \
                                         (((__PIN__) & SYSCFG_FASTMODEPLUS_PB8) == SYSCFG_FASTMODEPLUS_PB8))
#elif defined(SYSCFG_FASTMODEPLUS_PB9)
#define IS_SYSCFG_FASTMODEPLUS(__PIN__) ((((__PIN__) & SYSCFG_FASTMODEPLUS_PB6) == SYSCFG_FASTMODEPLUS_PB6) || \
                                         (((__PIN__) & SYSCFG_FASTMODEPLUS_PB7) == SYSCFG_FASTMODEPLUS_PB7) || \
                                         (((__PIN__) & SYSCFG_FASTMODEPLUS_PB9) == SYSCFG_FASTMODEPLUS_PB9))
#else
#define IS_SYSCFG_FASTMODEPLUS(__PIN__) ((((__PIN__) & SYSCFG_FASTMODEPLUS_PB6) == SYSCFG_FASTMODEPLUS_PB6) || \
                                         (((__PIN__) & SYSCFG_FASTMODEPLUS_PB7) == SYSCFG_FASTMODEPLUS_PB7))
#endif
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/

/** @addtogroup HAL_Exported_Functions
  * @{
  */

/** @addtogroup HAL_Exported_Functions_Group1
  * @{
  */

/* Initialization and de-initialization functions  ******************************/
HAL_StatusTypeDef HAL_Init(void);
HAL_StatusTypeDef HAL_DeInit(void);
void HAL_MspInit(void);
void HAL_MspDeInit(void);
HAL_StatusTypeDef HAL_InitTick (uint32_t TickPriority);

/**
  * @}
  */

/** @addtogroup HAL_Exported_Functions_Group2
  * @{
  */

/* Peripheral Control functions  ************************************************/
void HAL_IncTick(void);
void HAL_Delay(uint32_t Delay);
uint32_t HAL_GetTick(void);
void HAL_SuspendTick(void);
void HAL_ResumeTick(void);
uint32_t HAL_GetHalVersion(void);
uint32_t HAL_GetREVID(void);
uint32_t HAL_GetDEVID(void);

/**
  * @}
  */

/** @addtogroup HAL_Exported_Functions_Group3
  * @{
  */

/* DBGMCU Peripheral Control functions  *****************************************/
void HAL_DBGMCU_EnableDBGSleepMode(void);
void HAL_DBGMCU_DisableDBGSleepMode(void);
void HAL_DBGMCU_EnableDBGStopMode(void);
void HAL_DBGMCU_DisableDBGStopMode(void);
void HAL_DBGMCU_EnableDBGStandbyMode(void);
void HAL_DBGMCU_DisableDBGStandbyMode(void);

/**
  * @}
  */

/** @addtogroup HAL_Exported_Functions_Group4
  * @{
  */

/* SYSCFG Control functions  ****************************************************/
void HAL_SYSCFG_SRAM2Erase(void);
void HAL_SYSCFG_EnableMemorySwappingBank(void);
void HAL_SYSCFG_DisableMemorySwappingBank(void);

#if defined(VREFBUF)
void HAL_SYSCFG_VREFBUF_VoltageScalingConfig(uint32_t VoltageScaling);
void HAL_SYSCFG_VREFBUF_HighImpedanceConfig(uint32_t Mode);
void HAL_SYSCFG_VREFBUF_TrimmingConfig(uint32_t TrimmingValue);
HAL_StatusTypeDef HAL_SYSCFG_EnableVREFBUF(void);
void HAL_SYSCFG_DisableVREFBUF(void);
#endif /* VREFBUF */

void HAL_SYSCFG_EnableIOAnalogSwitchBooster(void);
void HAL_SYSCFG_DisableIOAnalogSwitchBooster(void);

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __STM32L4xx_HAL_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
