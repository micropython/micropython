/**
  ******************************************************************************
  * @file    stm32l4xx_ll_cortex.h
  * @author  MCD Application Team
  * @version V1.3.0
  * @date    29-January-2016
  * @brief   Header file of CORTEX LL module.
  @verbatim
  ==============================================================================
                     ##### How to use this driver #####
  ==============================================================================
    [..]
    The LL CORTEX driver contains a set of generic APIs that can be
    used by user:
      (+) SYSTICK configuration used by @ref LL_mDelay and @ref LL_Init1msTick
          functions
      (+) Low power mode configuration (SCB register of Cortex-MCU)
      (+) MPU API to configure and enable regions
      (+) API to access to MCU info (CPUID register)
      (+) API to enable fault handler (SHCSR accesses)

  @endverbatim
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
#ifndef __STM32L4xx_LL_CORTEX_H
#define __STM32L4xx_LL_CORTEX_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx.h"

/** @addtogroup STM32L4xx_LL_Driver
  * @{
  */

/** @defgroup CORTEX_LL CORTEX
  * @{
  */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/** @defgroup CORTEX_LL_Exported_Constants CORTEX Exported Constants
  * @{
  */

/** @defgroup CORTEX_LL_EC_CLKSOURCE_HCLK SYSTICK Clock Source
  * @{
  */
#define LL_SYSTICK_CLKSOURCE_HCLK_DIV8     ((uint32_t)0x00000000)                 /*!< AHB clock divided by 8 selected as SysTick clock source.*/
#define LL_SYSTICK_CLKSOURCE_HCLK          ((uint32_t)SysTick_CTRL_CLKSOURCE_Msk) /*!< AHB clock selected as SysTick clock source. */
/**
  * @}
  */

/** @defgroup CORTEX_LL_EC_FAULT Handler Fault type
  * @{
  */
#define LL_HANDLER_FAULT_USG               SCB_SHCSR_USGFAULTENA_Msk              /*!< Usage fault */
#define LL_HANDLER_FAULT_BUS               SCB_SHCSR_BUSFAULTENA_Msk              /*!< Bus fault */
#define LL_HANDLER_FAULT_MEM               SCB_SHCSR_MEMFAULTENA_Msk              /*!< Memory management fault */
/**
  * @}
  */

#if __MPU_PRESENT

/** @defgroup CORTEX_LL_EC_CTRL_HFNMI_PRIVDEF MPU Control
  * @{
  */
#define LL_MPU_CTRL_HFNMI_PRIVDEF_NONE     ((uint32_t)0x00000000)                            /*!< Disable NMI and privileged SW access */
#define LL_MPU_CTRL_HARDFAULT_NMI          MPU_CTRL_HFNMIENA_Msk                             /*!< Enables the operation of MPU during hard fault, NMI, and FAULTMASK handlers */
#define LL_MPU_CTRL_PRIVILEGED_DEFAULT     MPU_CTRL_PRIVDEFENA_Msk                           /*!< Enable privileged software access to default memory map */
#define LL_MPU_CTRL_HFNMI_PRIVDEF          (MPU_CTRL_HFNMIENA_Msk | MPU_CTRL_PRIVDEFENA_Msk) /*!< Enable NMI and privileged SW access */
/**
  * @}
  */

/** @defgroup CORTEX_LL_EC_REGION MPU Region Number
  * @{
  */
#define LL_MPU_REGION_NUMBER0              ((uint32_t)0x00) /*!< REGION Number 0 */
#define LL_MPU_REGION_NUMBER1              ((uint32_t)0x01) /*!< REGION Number 1 */
#define LL_MPU_REGION_NUMBER2              ((uint32_t)0x02) /*!< REGION Number 2 */
#define LL_MPU_REGION_NUMBER3              ((uint32_t)0x03) /*!< REGION Number 3 */
#define LL_MPU_REGION_NUMBER4              ((uint32_t)0x04) /*!< REGION Number 4 */
#define LL_MPU_REGION_NUMBER5              ((uint32_t)0x05) /*!< REGION Number 5 */
#define LL_MPU_REGION_NUMBER6              ((uint32_t)0x06) /*!< REGION Number 6 */
#define LL_MPU_REGION_NUMBER7              ((uint32_t)0x07) /*!< REGION Number 7 */
/**
  * @}
  */

/** @defgroup CORTEX_LL_EC_REGION_SIZE MPU Region Size
  * @{
  */
#define LL_MPU_REGION_SIZE_32B             ((uint32_t)(0x04 << MPU_RASR_SIZE_Pos)) /*!< 32B Size of the MPU protection region */
#define LL_MPU_REGION_SIZE_64B             ((uint32_t)(0x05 << MPU_RASR_SIZE_Pos)) /*!< 64B Size of the MPU protection region */
#define LL_MPU_REGION_SIZE_128B            ((uint32_t)(0x06 << MPU_RASR_SIZE_Pos)) /*!< 128B Size of the MPU protection region */
#define LL_MPU_REGION_SIZE_256B            ((uint32_t)(0x07 << MPU_RASR_SIZE_Pos)) /*!< 256B Size of the MPU protection region */
#define LL_MPU_REGION_SIZE_512B            ((uint32_t)(0x08 << MPU_RASR_SIZE_Pos)) /*!< 512B Size of the MPU protection region */
#define LL_MPU_REGION_SIZE_1KB             ((uint32_t)(0x09 << MPU_RASR_SIZE_Pos)) /*!< 1KB Size of the MPU protection region */
#define LL_MPU_REGION_SIZE_2KB             ((uint32_t)(0x0A << MPU_RASR_SIZE_Pos)) /*!< 2KB Size of the MPU protection region */
#define LL_MPU_REGION_SIZE_4KB             ((uint32_t)(0x0B << MPU_RASR_SIZE_Pos)) /*!< 4KB Size of the MPU protection region */
#define LL_MPU_REGION_SIZE_8KB             ((uint32_t)(0x0C << MPU_RASR_SIZE_Pos)) /*!< 8KB Size of the MPU protection region */
#define LL_MPU_REGION_SIZE_16KB            ((uint32_t)(0x0D << MPU_RASR_SIZE_Pos)) /*!< 16KB Size of the MPU protection region */
#define LL_MPU_REGION_SIZE_32KB            ((uint32_t)(0x0E << MPU_RASR_SIZE_Pos)) /*!< 32KB Size of the MPU protection region */
#define LL_MPU_REGION_SIZE_64KB            ((uint32_t)(0x0F << MPU_RASR_SIZE_Pos)) /*!< 64KB Size of the MPU protection region */
#define LL_MPU_REGION_SIZE_128KB           ((uint32_t)(0x10 << MPU_RASR_SIZE_Pos)) /*!< 128KB Size of the MPU protection region */
#define LL_MPU_REGION_SIZE_256KB           ((uint32_t)(0x11 << MPU_RASR_SIZE_Pos)) /*!< 256KB Size of the MPU protection region */
#define LL_MPU_REGION_SIZE_512KB           ((uint32_t)(0x12 << MPU_RASR_SIZE_Pos)) /*!< 512KB Size of the MPU protection region */
#define LL_MPU_REGION_SIZE_1MB             ((uint32_t)(0x13 << MPU_RASR_SIZE_Pos)) /*!< 1MB Size of the MPU protection region */
#define LL_MPU_REGION_SIZE_2MB             ((uint32_t)(0x14 << MPU_RASR_SIZE_Pos)) /*!< 2MB Size of the MPU protection region */
#define LL_MPU_REGION_SIZE_4MB             ((uint32_t)(0x15 << MPU_RASR_SIZE_Pos)) /*!< 4MB Size of the MPU protection region */
#define LL_MPU_REGION_SIZE_8MB             ((uint32_t)(0x16 << MPU_RASR_SIZE_Pos)) /*!< 8MB Size of the MPU protection region */
#define LL_MPU_REGION_SIZE_16MB            ((uint32_t)(0x17 << MPU_RASR_SIZE_Pos)) /*!< 16MB Size of the MPU protection region */
#define LL_MPU_REGION_SIZE_32MB            ((uint32_t)(0x18 << MPU_RASR_SIZE_Pos)) /*!< 32MB Size of the MPU protection region */
#define LL_MPU_REGION_SIZE_64MB            ((uint32_t)(0x19 << MPU_RASR_SIZE_Pos)) /*!< 64MB Size of the MPU protection region */
#define LL_MPU_REGION_SIZE_128MB           ((uint32_t)(0x1A << MPU_RASR_SIZE_Pos)) /*!< 128MB Size of the MPU protection region */
#define LL_MPU_REGION_SIZE_256MB           ((uint32_t)(0x1B << MPU_RASR_SIZE_Pos)) /*!< 256MB Size of the MPU protection region */
#define LL_MPU_REGION_SIZE_512MB           ((uint32_t)(0x1C << MPU_RASR_SIZE_Pos)) /*!< 512MB Size of the MPU protection region */
#define LL_MPU_REGION_SIZE_1GB             ((uint32_t)(0x1D << MPU_RASR_SIZE_Pos)) /*!< 1GB Size of the MPU protection region */
#define LL_MPU_REGION_SIZE_2GB             ((uint32_t)(0x1E << MPU_RASR_SIZE_Pos)) /*!< 2GB Size of the MPU protection region */
#define LL_MPU_REGION_SIZE_4GB             ((uint32_t)(0x1F << MPU_RASR_SIZE_Pos)) /*!< 4GB Size of the MPU protection region */
/**
  * @}
  */

/** @defgroup CORTEX_LL_EC_REGION_PRIVILEDGES MPU Region Privileges
  * @{
  */
#define LL_MPU_REGION_NO_ACCESS            ((uint32_t)(0x00 << MPU_RASR_AP_Pos)) /*!< No access*/
#define LL_MPU_REGION_PRIV_RW              ((uint32_t)(0x01 << MPU_RASR_AP_Pos)) /*!< RW privileged (privileged access only)*/
#define LL_MPU_REGION_PRIV_RW_URO          ((uint32_t)(0x02 << MPU_RASR_AP_Pos)) /*!< RW privileged - RO user (Write in a user program generates a fault) */
#define LL_MPU_REGION_FULL_ACCESS          ((uint32_t)(0x03 << MPU_RASR_AP_Pos)) /*!< RW privileged & user (Full access) */
#define LL_MPU_REGION_PRIV_RO              ((uint32_t)(0x05 << MPU_RASR_AP_Pos)) /*!< RO privileged (privileged read only)*/
#define LL_MPU_REGION_PRIV_RO_URO          ((uint32_t)(0x06 << MPU_RASR_AP_Pos)) /*!< RO privileged & user (read only) */
/**
  * @}
  */

/** @defgroup CORTEX_LL_EC_TEX MPU TEX Level
  * @{
  */
#define LL_MPU_TEX_LEVEL0                  ((uint32_t)(0x00 << MPU_RASR_TEX_Pos)) /*!< b000 for TEX bits */
#define LL_MPU_TEX_LEVEL1                  ((uint32_t)(0x01 << MPU_RASR_TEX_Pos)) /*!< b001 for TEX bits */
#define LL_MPU_TEX_LEVEL2                  ((uint32_t)(0x02 << MPU_RASR_TEX_Pos)) /*!< b010 for TEX bits */
#define LL_MPU_TEX_LEVEL4                  ((uint32_t)(0x04 << MPU_RASR_TEX_Pos)) /*!< b100 for TEX bits */
/**
  * @}
  */

/** @defgroup CORTEX_LL_EC_INSTRUCTION_ACCESS MPU Instruction Access
  * @{
  */
#define LL_MPU_INSTRUCTION_ACCESS_ENABLE   ((uint32_t)0x00) /*!< Instruction fetches enabled */
#define LL_MPU_INSTRUCTION_ACCESS_DISABLE  MPU_RASR_XN_Msk  /*!< Instruction fetches disabled*/
/**
  * @}
  */

/** @defgroup CORTEX_LL_EC_SHAREABLE_ACCESS MPU Shareable Access
  * @{
  */
#define LL_MPU_ACCESS_SHAREABLE            MPU_RASR_S_Msk   /*!< Shareable memory attribute */
#define LL_MPU_ACCESS_NOT_SHAREABLE        ((uint32_t)0x00) /*!< Not Shareable memory attribute */
/**
  * @}
  */

/** @defgroup CORTEX_LL_EC_CACHEABLE_ACCESS MPU Cacheable Access
  * @{
  */
#define LL_MPU_ACCESS_CACHEABLE            MPU_RASR_C_Msk   /*!< Cacheable memory attribute */
#define LL_MPU_ACCESS_NOT_CACHEABLE        ((uint32_t)0x00) /*!< Not Cacheable memory attribute */
/**
  * @}
  */

/** @defgroup CORTEX_LL_EC_BUFFERABLE_ACCESS MPU Bufferable Access
  * @{
  */
#define LL_MPU_ACCESS_BUFFERABLE           MPU_RASR_B_Msk   /*!< Bufferable memory attribute */
#define LL_MPU_ACCESS_NOT_BUFFERABLE       ((uint32_t)0x00) /*!< Not Bufferable memory attribute */
/**
  * @}
  */
#endif /* __MPU_PRESENT */
/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
/** @defgroup CORTEX_LL_Exported_Functions CORTEX Exported Functions
  * @{
  */

/** @defgroup CORTEX_LL_EF_SYSTICK SYSTICK
  * @{
  */

/**
  * @brief  This function checks if the Systick counter flag is active or not.
  * @note   It can be used in timeout function on application side.
  * @rmtoll STK_CTRL     COUNTFLAG     LL_SYSTICK_IsActiveCounterFlag
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_SYSTICK_IsActiveCounterFlag(void)
{
  return ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == (SysTick_CTRL_COUNTFLAG_Msk));
}

/**
  * @brief  Configures the SysTick clock source
  * @rmtoll STK_CTRL     CLKSOURCE     LL_SYSTICK_SetClkSource
  * @param  Source This parameter can be one of the following values:
  *         @arg @ref LL_SYSTICK_CLKSOURCE_HCLK_DIV8
  *         @arg @ref LL_SYSTICK_CLKSOURCE_HCLK
  * @retval None
  */
__STATIC_INLINE void LL_SYSTICK_SetClkSource(uint32_t Source)
{
  if (Source == LL_SYSTICK_CLKSOURCE_HCLK)
  {
    SET_BIT(SysTick->CTRL, LL_SYSTICK_CLKSOURCE_HCLK);
  }
  else
  {
    CLEAR_BIT(SysTick->CTRL, LL_SYSTICK_CLKSOURCE_HCLK);
  }
}

/**
  * @brief  Get the SysTick clock source
  * @rmtoll STK_CTRL     CLKSOURCE     LL_SYSTICK_GetClkSource
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_SYSTICK_CLKSOURCE_HCLK_DIV8
  *         @arg @ref LL_SYSTICK_CLKSOURCE_HCLK
  */
__STATIC_INLINE uint32_t LL_SYSTICK_GetClkSource(void)
{
  return READ_BIT(SysTick->CTRL, LL_SYSTICK_CLKSOURCE_HCLK);
}

/**
  * @brief  Enable SysTick exception request
  * @rmtoll STK_CTRL     TICKINT       LL_SYSTICK_EnableIT
  * @retval None
  */
__STATIC_INLINE void LL_SYSTICK_EnableIT(void)
{
  SET_BIT(SysTick->CTRL, SysTick_CTRL_TICKINT_Msk);
}

/**
  * @brief  Disable SysTick exception request
  * @rmtoll STK_CTRL     TICKINT       LL_SYSTICK_DisableIT
  * @retval None
  */
__STATIC_INLINE void LL_SYSTICK_DisableIT(void)
{
  CLEAR_BIT(SysTick->CTRL, SysTick_CTRL_TICKINT_Msk);
}

/**
  * @brief  Checks if the SYSTICK interrupt is enabled or disabled.
  * @rmtoll STK_CTRL     TICKINT       LL_SYSTICK_IsEnabledIT
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_SYSTICK_IsEnabledIT(void)
{
  return (READ_BIT(SysTick->CTRL, SysTick_CTRL_TICKINT_Msk) == (SysTick_CTRL_TICKINT_Msk));
}

/**
  * @}
  */

/** @defgroup CORTEX_LL_EF_LOW_POWER_MODE LOW POWER MODE
  * @{
  */

/**
  * @brief  Processor uses sleep as its low power mode
  * @rmtoll SCB_SCR      SLEEPDEEP     LL_LPM_EnableSleep
  * @retval None
  */
__STATIC_INLINE void LL_LPM_EnableSleep(void)
{
  /* Clear SLEEPDEEP bit of Cortex System Control Register */
  CLEAR_BIT(SCB->SCR, ((uint32_t)SCB_SCR_SLEEPDEEP_Msk));
}

/**
  * @brief  Processor uses deep sleep as its low power mode
  * @rmtoll SCB_SCR      SLEEPDEEP     LL_LPM_EnableDeepSleep
  * @retval None
  */
__STATIC_INLINE void LL_LPM_EnableDeepSleep(void)
{
  /* Set SLEEPDEEP bit of Cortex System Control Register */
  SET_BIT(SCB->SCR, ((uint32_t)SCB_SCR_SLEEPDEEP_Msk));
}

/**
  * @brief  Configures sleep-on-exit when returning from Handler mode to Thread mode.
  * @note   Setting this bit to 1 enables an interrupt-driven application to avoid returning to an
  *         empty main application.
  * @rmtoll SCB_SCR      SLEEPONEXIT   LL_LPM_EnableSleepOnExit
  * @retval None
  */
__STATIC_INLINE void LL_LPM_EnableSleepOnExit(void)
{
  /* Set SLEEPONEXIT bit of Cortex System Control Register */
  SET_BIT(SCB->SCR, ((uint32_t)SCB_SCR_SLEEPONEXIT_Msk));
}

/**
  * @brief  Do not sleep when returning to Thread mode.
  * @rmtoll SCB_SCR      SLEEPONEXIT   LL_LPM_DisableSleepOnExit
  * @retval None
  */
__STATIC_INLINE void LL_LPM_DisableSleepOnExit(void)
{
  /* Clear SLEEPONEXIT bit of Cortex System Control Register */
  CLEAR_BIT(SCB->SCR, ((uint32_t)SCB_SCR_SLEEPONEXIT_Msk));
}

/**
  * @brief  Enabled events and all interrupts, including disabled interrupts, can wakeup the
  *         processor.
  * @rmtoll SCB_SCR      SEVEONPEND    LL_LPM_EnableEventOnPend
  * @retval None
  */
__STATIC_INLINE void LL_LPM_EnableEventOnPend(void)
{
  /* Set SEVEONPEND bit of Cortex System Control Register */
  SET_BIT(SCB->SCR, ((uint32_t)SCB_SCR_SEVONPEND_Msk));
}

/**
  * @brief  Only enabled interrupts or events can wakeup the processor, disabled interrupts are
  *         excluded
  * @rmtoll SCB_SCR      SEVEONPEND    LL_LPM_DisableEventOnPend
  * @retval None
  */
__STATIC_INLINE void LL_LPM_DisableEventOnPend(void)
{
  /* Clear SEVEONPEND bit of Cortex System Control Register */
  CLEAR_BIT(SCB->SCR, ((uint32_t)SCB_SCR_SEVONPEND_Msk));
}

/**
  * @}
  */

/** @defgroup CORTEX_LL_EF_HANDLER HANDLER
  * @{
  */

/**
  * @brief  Enable a fault in System handler control register (SHCSR)
  * @rmtoll SCB_SHCSR    MEMFAULTENA   LL_HANDLER_EnableFault
  * @param  Fault This parameter can be a combination of the following values:
  *         @arg @ref LL_HANDLER_FAULT_USG
  *         @arg @ref LL_HANDLER_FAULT_BUS
  *         @arg @ref LL_HANDLER_FAULT_MEM
  * @retval None
  */
__STATIC_INLINE void LL_HANDLER_EnableFault(uint32_t Fault)
{
  /* Enable the system handler fault */
  SET_BIT(SCB->SHCSR, Fault);
}

/**
  * @brief  Disable a fault in System handler control register (SHCSR)
  * @rmtoll SCB_SHCSR    MEMFAULTENA   LL_HANDLER_DisableFault
  * @param  Fault This parameter can be a combination of the following values:
  *         @arg @ref LL_HANDLER_FAULT_USG
  *         @arg @ref LL_HANDLER_FAULT_BUS
  *         @arg @ref LL_HANDLER_FAULT_MEM
  * @retval None
  */
__STATIC_INLINE void LL_HANDLER_DisableFault(uint32_t Fault)
{
  /* Disable the system handler fault */
  CLEAR_BIT(SCB->SHCSR, Fault);
}

/**
  * @}
  */

/** @defgroup CORTEX_LL_EF_MCU_INFO MCU INFO
  * @{
  */

/**
  * @brief  Get Implementer code
  * @rmtoll SCB_CPUID    IMPLEMENTER   LL_CPUID_GetImplementer
  * @retval Value should be equal to 0x41 for ARM
  */
__STATIC_INLINE uint32_t LL_CPUID_GetImplementer(void)
{
  return (uint32_t)(READ_BIT(SCB->CPUID, SCB_CPUID_IMPLEMENTER_Msk) >> SCB_CPUID_IMPLEMENTER_Pos);
}

/**
  * @brief  Get Variant number (The r value in the rnpn product revision identifier)
  * @rmtoll SCB_CPUID    VARIANT       LL_CPUID_GetVariant
  * @retval Value between 0 and 255 (0x0: revision 0)
  */
__STATIC_INLINE uint32_t LL_CPUID_GetVariant(void)
{
  return (uint32_t)(READ_BIT(SCB->CPUID, SCB_CPUID_VARIANT_Msk) >> SCB_CPUID_VARIANT_Pos);
}

/**
  * @brief  Get Constant number
  * @rmtoll SCB_CPUID    ARCHITECTURE  LL_CPUID_GetConstant
  * @retval Value should be equal to 0xF for Cortex-M4 devices
  */
__STATIC_INLINE uint32_t LL_CPUID_GetConstant(void)
{
  return (uint32_t)(READ_BIT(SCB->CPUID, SCB_CPUID_ARCHITECTURE_Msk) >> SCB_CPUID_ARCHITECTURE_Pos);
}

/**
  * @brief  Get Part number
  * @rmtoll SCB_CPUID    PARTNO        LL_CPUID_GetParNo
  * @retval Value should be equal to 0xC24 for Cortex-M4
  */
__STATIC_INLINE uint32_t LL_CPUID_GetParNo(void)
{
  return (uint32_t)(READ_BIT(SCB->CPUID, SCB_CPUID_PARTNO_Msk) >> SCB_CPUID_PARTNO_Pos);
}

/**
  * @brief  Get Revision number (The p value in the rnpn product revision identifier, indicates patch release)
  * @rmtoll SCB_CPUID    REVISION      LL_CPUID_GetRevision
  * @retval Value between 0 and 255 (0x1: patch 1)
  */
__STATIC_INLINE uint32_t LL_CPUID_GetRevision(void)
{
  return (uint32_t)(READ_BIT(SCB->CPUID, SCB_CPUID_REVISION_Msk) >> SCB_CPUID_REVISION_Pos);
}

/**
  * @}
  */

#if __MPU_PRESENT
/** @defgroup CORTEX_LL_EF_MPU MPU
  * @{
  */

/**
  * @brief  Enable MPU with input options
  * @rmtoll MPU_CTRL     ENABLE        LL_MPU_Enable
  * @param  Options This parameter can be one of the following values:
  *         @arg @ref LL_MPU_CTRL_HFNMI_PRIVDEF_NONE
  *         @arg @ref LL_MPU_CTRL_HARDFAULT_NMI
  *         @arg @ref LL_MPU_CTRL_PRIVILEGED_DEFAULT
  *         @arg @ref LL_MPU_CTRL_HFNMI_PRIVDEF
  * @retval None
  */
__STATIC_INLINE void LL_MPU_Enable(uint32_t Options)
{
  /* Enable the MPU*/
  WRITE_REG(MPU->CTRL, (MPU_CTRL_ENABLE_Msk | Options));
  /* Ensure MPU settings take effects */
  __DSB();
  /* Sequence instruction fetches using update settings */
  __ISB();
}

/**
  * @brief  Disable MPU
  * @rmtoll MPU_CTRL     ENABLE        LL_MPU_Disable
  * @retval None
  */
__STATIC_INLINE void LL_MPU_Disable(void)
{
  /* Make sure outstanding transfers are done */
  __DMB();
  /* Disable MPU*/
  WRITE_REG(MPU->CTRL, 0);
}

/**
  * @brief  Check if MPU is enabled or not
  * @rmtoll MPU_CTRL     ENABLE        LL_MPU_IsEnabled
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_MPU_IsEnabled(void)
{
  return (READ_BIT(MPU->CTRL, MPU_CTRL_ENABLE_Msk) == (MPU_CTRL_ENABLE_Msk));
}

/**
  * @brief  Enable a MPU region
  * @rmtoll MPU_RASR     ENABLE        LL_MPU_EnableRegion
  * @param  Region This parameter can be one of the following values:
  *         @arg @ref LL_MPU_REGION_NUMBER0
  *         @arg @ref LL_MPU_REGION_NUMBER1
  *         @arg @ref LL_MPU_REGION_NUMBER2
  *         @arg @ref LL_MPU_REGION_NUMBER3
  *         @arg @ref LL_MPU_REGION_NUMBER4
  *         @arg @ref LL_MPU_REGION_NUMBER5
  *         @arg @ref LL_MPU_REGION_NUMBER6
  *         @arg @ref LL_MPU_REGION_NUMBER7
  * @retval None
  */
__STATIC_INLINE void LL_MPU_EnableRegion(uint32_t Region)
{
  /* Set Region number */
  WRITE_REG(MPU->RNR, Region);
  /* Enable the MPU region */
  SET_BIT(MPU->RASR, MPU_RASR_ENABLE_Msk);
}

/**
  * @brief  Configure and enable a region
  * @rmtoll MPU_RNR      REGION        LL_MPU_ConfigRegion\n
  *         MPU_RBAR     REGION        LL_MPU_ConfigRegion\n
  *         MPU_RBAR     ADDR          LL_MPU_ConfigRegion\n
  *         MPU_RASR     XN            LL_MPU_ConfigRegion\n
  *         MPU_RASR     AP            LL_MPU_ConfigRegion\n
  *         MPU_RASR     S             LL_MPU_ConfigRegion\n
  *         MPU_RASR     C             LL_MPU_ConfigRegion\n
  *         MPU_RASR     B             LL_MPU_ConfigRegion\n
  *         MPU_RASR     SIZE          LL_MPU_ConfigRegion
  * @param  Region This parameter can be one of the following values:
  *         @arg @ref LL_MPU_REGION_NUMBER0
  *         @arg @ref LL_MPU_REGION_NUMBER1
  *         @arg @ref LL_MPU_REGION_NUMBER2
  *         @arg @ref LL_MPU_REGION_NUMBER3
  *         @arg @ref LL_MPU_REGION_NUMBER4
  *         @arg @ref LL_MPU_REGION_NUMBER5
  *         @arg @ref LL_MPU_REGION_NUMBER6
  *         @arg @ref LL_MPU_REGION_NUMBER7
  * @param  Address Value of region base address
  * @param  SubRegionDisable Sub-region disable value between Min_Data = 0x00 and Max_Data = 0xFF
  * @param  Attributes This parameter can be a combination of the following values:
  *         @arg @ref LL_MPU_REGION_SIZE_32B or @ref LL_MPU_REGION_SIZE_64B or @ref LL_MPU_REGION_SIZE_128B or @ref LL_MPU_REGION_SIZE_256B or @ref LL_MPU_REGION_SIZE_512B
  *           or @ref LL_MPU_REGION_SIZE_1KB or @ref LL_MPU_REGION_SIZE_2KB or @ref LL_MPU_REGION_SIZE_4KB or @ref LL_MPU_REGION_SIZE_8KB or @ref LL_MPU_REGION_SIZE_16KB
  *           or @ref LL_MPU_REGION_SIZE_32KB or @ref LL_MPU_REGION_SIZE_64KB or @ref LL_MPU_REGION_SIZE_128KB or @ref LL_MPU_REGION_SIZE_256KB or @ref LL_MPU_REGION_SIZE_512KB
  *           or @ref LL_MPU_REGION_SIZE_1MB or @ref LL_MPU_REGION_SIZE_2MB or @ref LL_MPU_REGION_SIZE_4MB or @ref LL_MPU_REGION_SIZE_8MB or @ref LL_MPU_REGION_SIZE_16MB
  *           or @ref LL_MPU_REGION_SIZE_32MB or @ref LL_MPU_REGION_SIZE_64MB or @ref LL_MPU_REGION_SIZE_128MB or @ref LL_MPU_REGION_SIZE_256MB or @ref LL_MPU_REGION_SIZE_512MB
  *           or @ref LL_MPU_REGION_SIZE_1GB or @ref LL_MPU_REGION_SIZE_2GB or @ref LL_MPU_REGION_SIZE_4GB
  *         @arg @ref LL_MPU_REGION_NO_ACCESS or @ref LL_MPU_REGION_PRIV_RW or @ref LL_MPU_REGION_PRIV_RW_URO or @ref LL_MPU_REGION_FULL_ACCESS
  *           or @ref LL_MPU_REGION_PRIV_RO or @ref LL_MPU_REGION_PRIV_RO_URO
  *         @arg @ref LL_MPU_TEX_LEVEL0 or @ref LL_MPU_TEX_LEVEL1 or @ref LL_MPU_TEX_LEVEL2 or @ref LL_MPU_TEX_LEVEL4
  *         @arg @ref LL_MPU_INSTRUCTION_ACCESS_ENABLE or  @ref LL_MPU_INSTRUCTION_ACCESS_DISABLE
  *         @arg @ref LL_MPU_ACCESS_SHAREABLE or @ref LL_MPU_ACCESS_NOT_SHAREABLE
  *         @arg @ref LL_MPU_ACCESS_CACHEABLE or @ref LL_MPU_ACCESS_NOT_CACHEABLE
  *         @arg @ref LL_MPU_ACCESS_BUFFERABLE or @ref LL_MPU_ACCESS_NOT_BUFFERABLE
  * @retval None
  */
__STATIC_INLINE void LL_MPU_ConfigRegion(uint32_t Region, uint32_t SubRegionDisable, uint32_t Address, uint32_t Attributes)
{
  /* Set Region number */
  WRITE_REG(MPU->RNR, Region);
  /* Set base address */
  WRITE_REG(MPU->RBAR, (Address & 0xFFFFFFE0U));
  /* Configure MPU */
  WRITE_REG(MPU->RASR, (MPU_RASR_ENABLE_Msk | Attributes | SubRegionDisable << MPU_RASR_SRD_Pos));
}

/**
  * @brief  Disable a region
  * @rmtoll MPU_RNR      REGION        LL_MPU_DisableRegion\n
  *         MPU_RASR     ENABLE        LL_MPU_DisableRegion
  * @param  Region This parameter can be one of the following values:
  *         @arg @ref LL_MPU_REGION_NUMBER0
  *         @arg @ref LL_MPU_REGION_NUMBER1
  *         @arg @ref LL_MPU_REGION_NUMBER2
  *         @arg @ref LL_MPU_REGION_NUMBER3
  *         @arg @ref LL_MPU_REGION_NUMBER4
  *         @arg @ref LL_MPU_REGION_NUMBER5
  *         @arg @ref LL_MPU_REGION_NUMBER6
  *         @arg @ref LL_MPU_REGION_NUMBER7
  * @retval None
  */
__STATIC_INLINE void LL_MPU_DisableRegion(uint32_t Region)
{
  /* Set Region number */
  WRITE_REG(MPU->RNR, Region);
  /* Disable the MPU region */
  CLEAR_BIT(MPU->RASR, MPU_RASR_ENABLE_Msk);
}

/**
  * @}
  */

#endif /* __MPU_PRESENT */
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

#endif /* __STM32L4xx_LL_CORTEX_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
