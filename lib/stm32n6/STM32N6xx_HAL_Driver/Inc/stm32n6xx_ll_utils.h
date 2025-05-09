/**
  ******************************************************************************
  * @file    stm32n6xx_ll_utils.h
  * @author  MCD Application Team
  * @brief   Header file of UTILS LL module.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  @verbatim
  ==============================================================================
                     ##### How to use this driver #####
  ==============================================================================
    [..]
    The LL UTILS driver contains a set of generic APIs that can be
    used by user:
      (+) Device electronic signature
      (+) Timing functions
      (+) PLL configuration functions

  @endverbatim
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32N6xx_LL_UTILS_H
#define STM32N6xx_LL_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx.h"
#include "stm32n6xx_ll_system.h"
#include "stm32n6xx_ll_bus.h"
#include "stm32n6xx_ll_rcc.h"

/** @addtogroup STM32N6xx_LL_Driver
  * @{
  */

/** @defgroup UTILS_LL UTILS
  * @{
  */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/
/** @defgroup UTILS_LL_Private_Constants UTILS Private Constants
  * @{
  */

/* Max delay can be used in LL_mDelay */
#define LL_MAX_DELAY                  0xFFFFFFFFUL

/**
  * @brief Unique device ID register base address
  *        Available from BSEC_OTP_DATA5...BSEC_OTP_DATA7
  */
#define UID_BASE_ADDRESS              (BSEC_BASE + 0x14U)

/**
  * @brief Device RPN register base address
  *        Available from BSEC_OTP_DATA9
  */
#define RPN_BASE_ADDRESS              (BSEC_BASE + 0x24U)

/**
  * @brief Package data register base address
  *        Available from BSEC_OTP_DATA122
  */
#define PACKAGE_BASE_ADDRESS          (BSEC_BASE + 0x1E8U)

/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/** @defgroup UTILS_LL_ES_INIT UTILS Exported structures
  * @{
  */

/**
  * @brief  UTILS PLL structure definition
  */
typedef struct
{
  uint32_t PLLM;   /*!< Division factor M for PLL VCO input clock.
                        This parameter must be a number between Min_Data = 1 and Max_Data = 63
                        and shall ensure that VCO input is below 50 MHz.

                        This feature can be modified afterwards using unitary function
                        @ref LL_RCC_PLL1_SetM(). */

  uint32_t PLLN;   /*!< Multiplication factor N for PLL VCO output clock.
                        This parameter must be a number between Min_Data = 16 and Max_Data = 640
                        in integer mode and between Min_Data = 20 and Max_Data = 320 in fractional mode

                        This feature can be modified afterwards using unitary function
                        @ref LL_RCC_PLL1_SetN(). */

  uint32_t PLLP1;   /*!< Division factor level 1 for PLL VCO output clock.
                        This parameter must be a number between Min_Data = 1 and Max_Data = 7

                        This feature can be modified afterwards using unitary function
                        @ref LL_RCC_PLL1_SetP1(). */

  uint32_t PLLP2;   /*!< Division factor level 2 for PLL VCO output clock.
                        This parameter must be a number between Min_Data = 1 and Max_Data = 7

                        This feature can be modified afterwards using unitary function
                        @ref LL_RCC_PLL1_SetP2(). */

  uint32_t FRACN;  /*!< Fractional part of the multiplication factor for PLL VCO.
                        This parameter can be a value between 0 and (2^24)-1

                        This feature can be modified afterwards using unitary function
                        @ref LL_RCC_PLL1_SetFRACN(). */

} LL_UTILS_PLLInitTypeDef;

/**
  * @brief  UTILS IC structure definition
  */
typedef struct
{
  uint32_t IC1Source;   /*!< The IC1 clock source for sysa_ck(cpu_ck) */
  uint32_t IC1Divider;  /*!< The IC1 clock divider for sysa_ck(cpu_ck) */
  uint32_t IC2Source;   /*!< The IC2 clock source for sysb_ck */
  uint32_t IC2Divider;  /*!< The IC2 clock divider for sysb_ck */
  uint32_t IC6Source;   /*!< The IC6 clock source for sysc_ck */
  uint32_t IC6Divider;  /*!< The IC6 clock divider for sysc_ck */
  uint32_t IC11Source;  /*!< The IC11 clock source for sysd_ck */
  uint32_t IC11Divider; /*!< The IC11 clock divider for sysd_ck */

} LL_UTILS_ICInitTypeDef;

/**
  * @brief  UTILS System, AHB and APB buses clock configuration structure definition
  */
typedef struct
{
  uint32_t AHBCLKDivider;         /*!< The AHB clock (HCLK) divider. This clock is derived from the system clock (SYSCLK).
                                       This parameter can be a value of @ref RCC_LL_EC_AHB_DIV

                                       This feature can be modified afterwards using unitary function
                                       @ref LL_RCC_SetAHBPrescaler(). */

  uint32_t APB1CLKDivider;        /*!< The APB1 clock (PCLK1) divider. This clock is derived from the AHB clock (HCLK).
                                       This parameter can be a value of @ref RCC_LL_EC_APB1_DIV

                                       This feature can be modified afterwards using unitary function
                                       @ref LL_RCC_SetAPB1Prescaler(). */

  uint32_t APB2CLKDivider;        /*!< The APB2 clock (PCLK2) divider. This clock is derived from the AHB clock (HCLK).
                                       This parameter can be a value of @ref RCC_LL_EC_APB2_DIV

                                       This feature can be modified afterwards using unitary function
                                       @ref LL_RCC_SetAPB2Prescaler(). */

  uint32_t APB4CLKDivider;        /*!< The APB4 clock (PCLK4) divider. This clock is derived from the AHB clock (HCLK).
                                       This parameter can be a value of @ref RCC_LL_EC_APB4_DIV

                                       This feature can be modified afterwards using unitary function
                                       @ref LL_RCC_SetAPB4Prescaler(). */

  uint32_t APB5CLKDivider;        /*!< The APB5 clock (PCLK5) divider. This clock is derived from the AHB clock (HCLK).
                                       This parameter can be a value of @ref RCC_LL_EC_APB5_DIV

                                       This feature can be modified afterwards using unitary function
                                       @ref LL_RCC_SetAPB5Prescaler(). */

} LL_UTILS_ClkInitTypeDef;

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup UTILS_LL_Exported_Constants UTILS Exported Constants
  * @{
  */

/** @defgroup UTILS_EC_HSE_BYPASS HSE Bypass activation
  * @{
  */
#define LL_UTILS_HSEBYPASS_OFF        0U  /*!< HSE Bypass is not enabled */
#define LL_UTILS_HSEBYPASS_ON         1U  /*!< HSE Bypass is enabled */
/**
  * @}
  */

/** @defgroup UTILS_EC_DEVICE_RPN DEVICE RPN
  * @{
  */
#define LL_UTILS_DEVICE_STM32N657     0x2000U  /*!< STM32N657 */
#define LL_UTILS_DEVICE_STM32N647     0x2200U  /*!< STM32N647 */
#define LL_UTILS_DEVICE_STM32N655     0x6000U  /*!< STM32N655 */
#define LL_UTILS_DEVICE_STM32N645     0x6200U  /*!< STM32N645 */
/**
  * @}
  */

/** @defgroup UTILS_EC_PACKAGETYPE PACKAGE TYPE
  * @{
  */
#define LL_UTILS_PACKAGETYPE_BGA142   2U  /*!< BGA142 package type */
#define LL_UTILS_PACKAGETYPE_BGA169   4U  /*!< BGA169 package type */
#define LL_UTILS_PACKAGETYPE_BGA178   6U  /*!< BGA178 package type */
#define LL_UTILS_PACKAGETYPE_BGA198   8U  /*!< BGA198 package type */
#define LL_UTILS_PACKAGETYPE_BGA223  10U  /*!< BGA223 package type */
#define LL_UTILS_PACKAGETYPE_BGA264  12U  /*!< BGA264 package type */
/**
  * @}
  */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
/** @defgroup UTILS_LL_Exported_Functions UTILS Exported Functions
  * @{
  */

/** @defgroup UTILS_EF_DEVICE_ELECTRONIC_SIGNATURE DEVICE ELECTRONIC SIGNATURE
  * @{
  */

/**
  * @brief  Get Word0 of the unique device identifier (UID based on 96 bits)
  * @note  The application must ensures that SYSCFG clock is enabled.
  * @retval UID[31:0]
  */
__STATIC_INLINE uint32_t LL_GetUID_Word0(void)
{
  return (uint32_t)(READ_REG(*((__IO uint32_t *)UID_BASE_ADDRESS)));
}

/**
  * @brief  Get Word1 of the unique device identifier (UID based on 96 bits)
  * @note  The application must ensures that SYSCFG clock is enabled.
  * @retval UID[63:32]
  */
__STATIC_INLINE uint32_t LL_GetUID_Word1(void)
{
  return (uint32_t)(READ_REG(*((__IO uint32_t *)(UID_BASE_ADDRESS + 4U))));
}

/**
  * @brief  Get Word2 of the unique device identifier (UID based on 96 bits)
  * @note  The application must ensures that SYSCFG clock is enabled.
  * @retval UID[95:64]
  */
__STATIC_INLINE uint32_t LL_GetUID_Word2(void)
{
  return (uint32_t)(READ_REG(*((__IO uint32_t *)(UID_BASE_ADDRESS + 8U))));
}

/**
  * @brief  Get Package type
  * @note  The application must ensures that SYSCFG clock is enabled.
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_UTILS_PACKAGETYPE_BGA142
  *         @arg @ref LL_UTILS_PACKAGETYPE_BGA169
  *         @arg @ref LL_UTILS_PACKAGETYPE_BGA178
  *         @arg @ref LL_UTILS_PACKAGETYPE_BGA198
  *         @arg @ref LL_UTILS_PACKAGETYPE_BGA223
  *         @arg @ref LL_UTILS_PACKAGETYPE_BGA264
  */
__STATIC_INLINE uint32_t LL_GetPackageType(void)
{
  return (uint32_t)(READ_REG(*((__IO uint32_t *)PACKAGE_BASE_ADDRESS)) & 0x0FUL);
}

/**
  * @brief  Get Device Part Number (RPN)
  * @note  The application must ensures that SYSCFG clock is enabled.
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_UTILS_DEVICE_STM32N657
  *         @arg @ref LL_UTILS_DEVICE_STM32N647
  *         @arg @ref LL_UTILS_DEVICE_STM32N645
  */
__STATIC_INLINE uint32_t LL_GetDevicePartNumber(void)
{
  return (uint32_t)(READ_REG(*((__IO uint32_t *)RPN_BASE_ADDRESS)));
}

/**
  * @}
  */

/** @defgroup UTILS_LL_EF_DELAY DELAY
  * @{
  */

/**
  * @brief  This function configures the Cortex-M SysTick source of the time base.
  * @param  HCLKFrequency HCLK frequency in Hz (can be calculated thanks to RCC helper macro)
  * @note   When a RTOS is used, it is recommended to avoid changing the SysTick
  *         configuration by calling this function, for a delay use rather osDelay RTOS service.
  * @param  Ticks Number of ticks
  * @retval None
  */
__STATIC_INLINE void LL_InitTick(uint32_t HCLKFrequency, uint32_t Ticks)
{
  if (Ticks > 0U)
  {
    /* Configure the SysTick to have interrupt in 1ms time base */
    SysTick->LOAD  = (uint32_t)((HCLKFrequency / Ticks) - 1UL);  /* set reload register */
    SysTick->VAL   = 0UL;                                       /* Load the SysTick Counter Value */
    SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
                     SysTick_CTRL_ENABLE_Msk;                   /* Enable the Systick Timer */
  }
}

void        LL_Init1msTick(uint32_t CPU_Frequency);
void        LL_mDelay(uint32_t Delay);

/**
  * @}
  */

/** @defgroup UTILS_EF_SYSTEM SYSTEM
  * @{
  */

void        LL_SetSystemCoreClock(uint32_t CPU_Frequency);
ErrorStatus LL_PLL_ConfigSystemClock_HSI(const LL_UTILS_PLLInitTypeDef *pUTILS_PLLInitStruct,
                                         const LL_UTILS_ICInitTypeDef  *pUTILS_ICInitStruct,
                                         const LL_UTILS_ClkInitTypeDef *pUTILS_ClkInitStruct);
ErrorStatus LL_PLL_ConfigSystemClock_MSI(const LL_UTILS_PLLInitTypeDef *pUTILS_PLLInitStruct,
                                         const LL_UTILS_ICInitTypeDef  *pUTILS_ICInitStruct,
                                         const LL_UTILS_ClkInitTypeDef *pUTILS_ClkInitStruct);
ErrorStatus LL_PLL_ConfigSystemClock_HSE(uint32_t HSEFrequency,
                                         uint32_t HSEBypass,
                                         const LL_UTILS_PLLInitTypeDef *pUTILS_PLLInitStruct,
                                         const LL_UTILS_ICInitTypeDef  *pUTILS_ICInitStruct,
                                         const LL_UTILS_ClkInitTypeDef *pUTILS_ClkInitStruct);

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

#endif /* STM32N6xx_LL_UTILS_H */
