/**
  ******************************************************************************
  * @file    stm32f2xx_hal_rcc_ex.h
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    25-March-2014
  * @brief   Header file of RCC HAL Extension module.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2014 STMicroelectronics</center></h2>
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
#ifndef __STM32F2xx_HAL_RCC_EX_H
#define __STM32F2xx_HAL_RCC_EX_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f2xx_hal_def.h"

/** @addtogroup STM32F2xx_HAL_Driver
  * @{
  */

/** @addtogroup RCCEx
  * @{
  */ 

/* Exported types ------------------------------------------------------------*/ 

/** 
  * @brief  PLLI2S Clock structure definition  
  */
typedef struct
{
  uint32_t PLLI2SN;    /*!< Specifies the multiplication factor for PLLI2S VCO output clock
                            This parameter must be a number between Min_Data = 192 and Max_Data = 432
                            This parameter will be used only when PLLI2S is selected as Clock Source I2S or SAI */

  uint32_t PLLI2SR;    /*!< Specifies the division factor for I2S clock
                            This parameter must be a number between Min_Data = 2 and Max_Data = 7 
                            This parameter will be used only when PLLI2S is selected as Clock Source I2S or SAI */

}RCC_PLLI2SInitTypeDef;

/** 
  * @brief  RCC extended clocks structure definition  
  */
typedef struct
{
  uint32_t PeriphClockSelection; /*!< The Extended Clock to be configured.
                                      This parameter can be a value of @ref RCCEx_Periph_Clock_Selection */

  RCC_PLLI2SInitTypeDef PLLI2S;  /*!< PLL I2S structure parameters
                                      This parameter will be used only when PLLI2S is selected as Clock Source I2S or SAI */

  uint32_t RTCClockSelection;      /*!< Specifies RTC Clock Prescalers Selection 
                                      This parameter can be a value of @ref RCC_RTC_Clock_Source */

  uint8_t TIMPresSelection;      /*!< Specifies TIM Clock Prescalers Selection 
                                      This parameter can be a value of @ref RCCEx_TIM_PRescaler_Selection */

}RCC_PeriphCLKInitTypeDef;


/* Exported constants --------------------------------------------------------*/
/** @defgroup RCCEx_Exported_Constants
  * @{
  */

/** @defgroup RCCEx_Periph_Clock_Selection
  * @{
  */
#define RCC_PERIPHCLK_I2S             ((uint32_t)0x00000001)
#define RCC_PERIPHCLK_TIM             ((uint32_t)0x00000004)
#define RCC_PERIPHCLK_RTC             ((uint32_t)0x00000008)
#define IS_RCC_PERIPHCLOCK(SELECTION) ((1 <= (SELECTION)) && ((SELECTION) <= 0x0000000f))

/**
  * @}
  */

/** @defgroup RCCEx_TIM_PRescaler_Selection
  * @{
  */
#define RCC_TIMPRES_DESACTIVATED        ((uint8_t)0x00)
#define RCC_TIMPRES_ACTIVATED           ((uint8_t)0x01)
/**
  * @}
  */

/**
  * @}
  */
     
/* Exported macro ------------------------------------------------------------*/

/** @brief  Enables or disables the AHB1 peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before 
  *         using it.
  */

#if defined(STM32F207xx) || defined(STM32F217xx)
#define __ETHMAC_CLK_ENABLE()     (RCC->AHB1ENR |= (RCC_AHB1ENR_ETHMACEN))
#define __ETHMACTX_CLK_ENABLE()   (RCC->AHB1ENR |= (RCC_AHB1ENR_ETHMACTXEN))
#define __ETHMACRX_CLK_ENABLE()   (RCC->AHB1ENR |= (RCC_AHB1ENR_ETHMACRXEN))
#define __ETHMACPTP_CLK_ENABLE()  (RCC->AHB1ENR |= (RCC_AHB1ENR_ETHMACPTPEN))

#define __ETHMAC_CLK_DISABLE()    (RCC->AHB1ENR &= ~(RCC_AHB1ENR_ETHMACEN))
#define __ETHMACTX_CLK_DISABLE()  (RCC->AHB1ENR &= ~(RCC_AHB1ENR_ETHMACTXEN))
#define __ETHMACRX_CLK_DISABLE()  (RCC->AHB1ENR &= ~(RCC_AHB1ENR_ETHMACRXEN))
#define __ETHMACPTP_CLK_DISABLE() (RCC->AHB1ENR &= ~(RCC_AHB1ENR_ETHMACPTPEN))
#endif /* STM32F207xx || STM32F217xx */

/**
  * @brief  Enable ETHERNET clock.
  */
#if defined(STM32F207xx) || defined(STM32F217xx)
#define __ETH_CLK_ENABLE()       do {                            \
                                     __ETHMAC_CLK_ENABLE();      \
                                     __ETHMACTX_CLK_ENABLE();    \
                                     __ETHMACRX_CLK_ENABLE();    \
                                    } while(0)

/**
  * @brief  Disable ETHERNET clock.
  */
#define __ETH_CLK_DISABLE()       do {                             \
                                      __ETHMACTX_CLK_DISABLE();    \
                                      __ETHMACRX_CLK_DISABLE();    \
                                      __ETHMAC_CLK_DISABLE();      \
                                     } while(0)
#endif /* STM32F207xx || STM32F217xx */

/** @brief  Enable or disable the AHB2 peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before 
  *         using it.
  */
#if defined(STM32F207xx) || defined(STM32F217xx)
#define __DCMI_CLK_ENABLE()   (RCC->AHB2ENR |= (RCC_AHB2ENR_DCMIEN))
#define __DCMI_CLK_DISABLE()  (RCC->AHB2ENR &= ~(RCC_AHB2ENR_DCMIEN))
#endif /* STM32F207xx || STM32F217xx */

#if defined(STM32F215xx) || defined(STM32F217xx)
#define __CRYP_CLK_ENABLE()   (RCC->AHB2ENR |= (RCC_AHB2ENR_CRYPEN))
#define __HASH_CLK_ENABLE()   (RCC->AHB2ENR |= (RCC_AHB2ENR_HASHEN))


#define __CRYP_CLK_DISABLE()  (RCC->AHB2ENR &= ~(RCC_AHB2ENR_CRYPEN))
#define __HASH_CLK_DISABLE()  (RCC->AHB2ENR &= ~(RCC_AHB2ENR_HASHEN))

#endif /* STM32F215xx || STM32F217xx */

/** @brief  Force or release AHB1 peripheral reset.
  */
#if defined(STM32F207xx) || defined(STM32F217xx)
#define __ETHMAC_FORCE_RESET()   (RCC->AHB1RSTR |= (RCC_AHB1RSTR_ETHMACRST))

#define __ETHMAC_RELEASE_RESET() (RCC->AHB1RSTR &= ~(RCC_AHB1RSTR_ETHMACRST))
#endif /* STM32F207xx || STM32F217xx */

/** @brief  Force or release AHB2 peripheral reset.
  */
#if defined(STM32F207xx) || defined(STM32F217xx)
#define __DCMI_FORCE_RESET()   (RCC->AHB2RSTR |= (RCC_AHB2RSTR_DCMIRST))
#define __DCMI_RELEASE_RESET() (RCC->AHB2RSTR &= ~(RCC_AHB2RSTR_DCMIRST))
#endif /* STM32F207xx || STM32F217xx */

#if defined(STM32F215xx) || defined(STM32F217xx)
#define __CRYP_FORCE_RESET()   (RCC->AHB2RSTR |= (RCC_AHB2RSTR_CRYPRST))
#define __HASH_FORCE_RESET()   (RCC->AHB2RSTR |= (RCC_AHB2RSTR_HASHRST))


#define __CRYP_RELEASE_RESET() (RCC->AHB2RSTR &= ~(RCC_AHB2RSTR_CRYPRST))
#define __HASH_RELEASE_RESET() (RCC->AHB2RSTR &= ~(RCC_AHB2RSTR_HASHRST))

#endif /* STM32F215xx || STM32F217xx */

/** @brief  Force or release AHB3 peripheral reset
  */

/** @brief  Force or release APB1 peripheral reset.
  */

/** @brief  Force or release APB2 peripheral reset.
  */

/** @brief  Enable or disable the AHB1 peripheral clock during Low Power (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is enabled again.
  * @note   By default, all peripheral clocks are enabled during SLEEP mode.
  */

#if defined(STM32F207xx) || defined(STM32F217xx)
#define __ETHMAC_CLK_SLEEP_ENABLE()     (RCC->AHB1LPENR |= (RCC_AHB1LPENR_ETHMACLPEN))
#define __ETHMACTX_CLK_SLEEP_ENABLE()   (RCC->AHB1LPENR |= (RCC_AHB1LPENR_ETHMACTXLPEN))
#define __ETHMACRX_CLK_SLEEP_ENABLE()   (RCC->AHB1LPENR |= (RCC_AHB1LPENR_ETHMACRXLPEN))
#define __ETHMACPTP_CLK_SLEEP_ENABLE()  (RCC->AHB1LPENR |= (RCC_AHB1LPENR_ETHMACPTPLPEN))

#define __ETHMAC_CLK_SLEEP_DISABLE()    (RCC->AHB1LPENR &= ~(RCC_AHB1LPENR_ETHMACLPEN))
#define __ETHMACTX_CLK_SLEEP_DISABLE()  (RCC->AHB1LPENR &= ~(RCC_AHB1LPENR_ETHMACTXLPEN))
#define __ETHMACRX_CLK_SLEEP_DISABLE()  (RCC->AHB1LPENR &= ~(RCC_AHB1LPENR_ETHMACRXLPEN))
#define __ETHMACPTP_CLK_SLEEP_DISABLE() (RCC->AHB1LPENR &= ~(RCC_AHB1LPENR_ETHMACPTPLPEN))
#endif /* STM32F207xx || STM32F217xx */

/** @brief  Enable or disable the AHB2 peripheral clock during Low Power (Sleep) mode.
  * @note   Peripheral clock gating in SLEEP mode can be used to further reduce
  *         power consumption.
  * @note   After wakeup from SLEEP mode, the peripheral clock is enabled again.
  * @note   By default, all peripheral clocks are enabled during SLEEP mode.
  */
#if defined(STM32F207xx) || defined(STM32F217xx)
#define __DCMI_CLK_SLEEP_ENABLE()  (RCC->AHB2LPENR |= (RCC_AHB2LPENR_DCMILPEN))
#define __DCMI_CLK_SLEEP_DISABLE() (RCC->AHB2LPENR &= ~(RCC_AHB2LPENR_DCMILPEN))
#endif /* STM32F207xx || STM32F217xx */

#if defined(STM32F215xx) || defined(STM32F217xx)
#define __CRYP_CLK_SLEEP_ENABLE()  (RCC->AHB2LPENR |= (RCC_AHB2LPENR_CRYPLPEN))
#define __HASH_CLK_SLEEP_ENABLE()  (RCC->AHB2LPENR |= (RCC_AHB2LPENR_HASHLPEN))


#define __CRYP_CLK_SLEEP_DISABLE() (RCC->AHB2LPENR &= ~(RCC_AHB2LPENR_CRYPLPEN))
#define __HASH_CLK_SLEEP_DISABLE() (RCC->AHB2LPENR &= ~(RCC_AHB2LPENR_HASHLPEN))

#endif /* STM32F215xx || STM32F217xx */

/* Exported functions --------------------------------------------------------*/


/* Exported functions --------------------------------------------------------*/
HAL_StatusTypeDef HAL_RCCEx_PeriphCLKConfig(RCC_PeriphCLKInitTypeDef  *PeriphClkInit);
void HAL_RCCEx_GetPeriphCLKConfig(RCC_PeriphCLKInitTypeDef  *PeriphClkInit);

/**
  * @}
  */ 

/**
  * @}
  */
  
#ifdef __cplusplus
}
#endif

#endif /* __STM32F2xx_HAL_RCC_EX_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
