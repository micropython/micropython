/**
  ******************************************************************************
  * @file    stm32l4xx_ll_iwdg.h
  * @author  MCD Application Team
  * @version V1.3.0
  * @date    29-January-2016
  * @brief   Header file of IWDG LL module.
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
#ifndef __STM32L4xx_LL_IWDG_H
#define __STM32L4xx_LL_IWDG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx.h"

/** @addtogroup STM32L4xx_LL_Driver
  * @{
  */

#if defined(IWDG)

/** @defgroup IWDG_LL IWDG
  * @{
  */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/
/** @defgroup IWDG_LL_Private_Constants IWDG Private Constants
  * @{
  */

#define LL_IWDG_KEY_RELOAD                 ((uint32_t)0x0000AAAAU)               /*!< IWDG Reload Counter Enable   */
#define LL_IWDG_KEY_ENABLE                 ((uint32_t)0x0000CCCCU)               /*!< IWDG Peripheral Enable       */
#define LL_IWDG_KEY_WR_ACCESS_ENABLE       ((uint32_t)0x00005555U)               /*!< IWDG KR Write Access Enable  */
#define LL_IWDG_KEY_WR_ACCESS_DISABLE      ((uint32_t)0x00000000U)               /*!< IWDG KR Write Access Disable */

/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/** @defgroup IWDG_LL_Exported_Constants IWDG Exported Constants
  * @{
  */

/** @defgroup IWDG_LL_EC_GET_FLAG Get Flags Defines
  * @brief    Flags defines which can be used with LL_IWDG_ReadReg function
  * @{
  */
#define LL_IWDG_SR_PVU                     IWDG_SR_PVU                           /*!< Watchdog prescaler value update */
#define LL_IWDG_SR_RVU                     IWDG_SR_RVU                           /*!< Watchdog counter reload value update */
#define LL_IWDG_SR_WVU                     IWDG_SR_WVU                           /*!< Watchdog counter window value update */

/**
  * @}
  */

/** @defgroup IWDG_LL_EC_PRESCALER  Prescaler Divider
  * @{
  */
#define LL_IWDG_PRESCALER_4                ((uint32_t)0x00000000U)               /*!< Divider by 4   */
#define LL_IWDG_PRESCALER_8                (IWDG_PR_PR_0)                        /*!< Divider by 8   */
#define LL_IWDG_PRESCALER_16               (IWDG_PR_PR_1)                        /*!< Divider by 16  */
#define LL_IWDG_PRESCALER_32               (IWDG_PR_PR_1 | IWDG_PR_PR_0)         /*!< Divider by 32  */
#define LL_IWDG_PRESCALER_64               (IWDG_PR_PR_2)                        /*!< Divider by 64  */
#define LL_IWDG_PRESCALER_128              (IWDG_PR_PR_2 | IWDG_PR_PR_0)         /*!< Divider by 128 */
#define LL_IWDG_PRESCALER_256              (IWDG_PR_PR_2 | IWDG_PR_PR_1)         /*!< Divider by 256 */
/**
  * @}
  */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
/** @defgroup IWDG_LL_Exported_Macros IWDG Exported Macros
  * @{
  */

/** @defgroup IWDG_LL_EM_WRITE_READ Common Write and read registers Macros
  * @{
  */

/**
  * @brief  Write a value in IWDG register
  * @param  __INSTANCE__ IWDG Instance
  * @param  __REG__ Register to be written
  * @param  __VALUE__ Value to be written in the register
  * @retval None
  */
#define LL_IWDG_WriteReg(__INSTANCE__, __REG__, __VALUE__) WRITE_REG(__INSTANCE__->__REG__, (__VALUE__))

/**
  * @brief  Read a value in IWDG register
  * @param  __INSTANCE__ IWDG Instance
  * @param  __REG__ Register to be read
  * @retval Register value
  */
#define LL_IWDG_ReadReg(__INSTANCE__, __REG__) READ_REG(__INSTANCE__->__REG__)
/**
  * @}
  */

/**
  * @}
  */


/* Exported functions --------------------------------------------------------*/
/** @defgroup IWDG_LL_Exported_Functions IWDG Exported Functions
  * @{
  */
/** @defgroup IWDG_LL_EF_Configuration Configuration
  * @{
  */

/**
  * @brief  Start the Independent Watchdog
  * @note   Except if the hardware watchdog option is selected
  * @rmtoll KR           KEY           LL_IWDG_Enable
  * @param  IWDGx IWDG Instance
  * @retval None
  */
__STATIC_INLINE void LL_IWDG_Enable(IWDG_TypeDef *IWDGx)
{
  WRITE_REG(IWDG->KR, LL_IWDG_KEY_ENABLE);
}

/**
  * @brief  Reloads IWDG counter with value defined in the reload register
  * @rmtoll KR           KEY           LL_IWDG_ReloadCounter
  * @param  IWDGx IWDG Instance
  * @retval None
  */
__STATIC_INLINE void LL_IWDG_ReloadCounter(IWDG_TypeDef *IWDGx)
{
  WRITE_REG(IWDG->KR, LL_IWDG_KEY_RELOAD);
}

/**
  * @brief  Enable write access to IWDG_PR, IWDG_RLR and IWDG_WINR registers
  * @rmtoll KR           KEY           LL_IWDG_EnableWriteAccess
  * @param  IWDGx IWDG Instance
  * @retval None
  */
__STATIC_INLINE void LL_IWDG_EnableWriteAccess(IWDG_TypeDef *IWDGx)
{
  WRITE_REG(IWDG->KR, LL_IWDG_KEY_WR_ACCESS_ENABLE);
}

/**
  * @brief  Disable write access to IWDG_PR, IWDG_RLR and IWDG_WINR registers
  * @rmtoll KR           KEY           LL_IWDG_DisableWriteAccess
  * @param  IWDGx IWDG Instance
  * @retval None
  */
__STATIC_INLINE void LL_IWDG_DisableWriteAccess(IWDG_TypeDef *IWDGx)
{
  WRITE_REG(IWDG->KR, LL_IWDG_KEY_WR_ACCESS_DISABLE);
}

/**
  * @brief  Select the prescaler of the IWDG
  * @rmtoll PR           PR            LL_IWDG_SetPrescaler
  * @param  IWDGx IWDG Instance
  * @param  Prescaler This parameter can be one of the following values:
  *         @arg @ref LL_IWDG_PRESCALER_4
  *         @arg @ref LL_IWDG_PRESCALER_8
  *         @arg @ref LL_IWDG_PRESCALER_16
  *         @arg @ref LL_IWDG_PRESCALER_32
  *         @arg @ref LL_IWDG_PRESCALER_64
  *         @arg @ref LL_IWDG_PRESCALER_128
  *         @arg @ref LL_IWDG_PRESCALER_256
  * @retval None
  */
__STATIC_INLINE void LL_IWDG_SetPrescaler(IWDG_TypeDef *IWDGx, uint32_t Prescaler)
{
  WRITE_REG(IWDGx->PR, IWDG_PR_PR & Prescaler);
}

/**
  * @brief  Get the selected prescaler of the IWDG
  * @rmtoll PR           PR            LL_IWDG_GetPrescaler
  * @param  IWDGx IWDG Instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_IWDG_PRESCALER_4
  *         @arg @ref LL_IWDG_PRESCALER_8
  *         @arg @ref LL_IWDG_PRESCALER_16
  *         @arg @ref LL_IWDG_PRESCALER_32
  *         @arg @ref LL_IWDG_PRESCALER_64
  *         @arg @ref LL_IWDG_PRESCALER_128
  *         @arg @ref LL_IWDG_PRESCALER_256
  */
__STATIC_INLINE uint32_t LL_IWDG_GetPrescaler(IWDG_TypeDef *IWDGx)
{
  return (uint32_t)(READ_REG(IWDGx->PR));
}

/**
  * @brief  Specify the IWDG down-counter reload value
  * @rmtoll RLR          RL            LL_IWDG_SetReloadCounter
  * @param  IWDGx IWDG Instance
  * @param  Counter Value between Min_Data=0 and Max_Data=0x0FFF
  * @retval None
  */
__STATIC_INLINE void LL_IWDG_SetReloadCounter(IWDG_TypeDef *IWDGx, uint32_t Counter)
{
  WRITE_REG(IWDGx->RLR, IWDG_RLR_RL & Counter);
}

/**
  * @brief  Get the specified IWDG down-counter reload value
  * @rmtoll RLR          RL            LL_IWDG_GetReloadCounter
  * @param  IWDGx IWDG Instance
  * @retval Value between Min_Data=0 and Max_Data=0x0FFF
  */
__STATIC_INLINE uint32_t LL_IWDG_GetReloadCounter(IWDG_TypeDef *IWDGx)
{
  return (uint32_t)(READ_REG(IWDGx->RLR));
}

/**
  * @brief  Specify high limit of the window value to be compared to the down-counter.
  * @rmtoll WINR         WIN           LL_IWDG_SetWindow
  * @param  IWDGx IWDG Instance
  * @param  Window Value between Min_Data=0 and Max_Data=0x0FFF
  * @retval None
  */
__STATIC_INLINE void LL_IWDG_SetWindow(IWDG_TypeDef *IWDGx, uint32_t Window)
{
  WRITE_REG(IWDGx->WINR, IWDG_WINR_WIN & Window);
}

/**
  * @brief  Get the high limit of the window value specified.
  * @rmtoll WINR         WIN           LL_IWDG_GetWindow
  * @param  IWDGx IWDG Instance
  * @retval Value between Min_Data=0 and Max_Data=0x0FFF
  */
__STATIC_INLINE uint32_t LL_IWDG_GetWindow(IWDG_TypeDef *IWDGx)
{
  return (uint32_t)(READ_REG(IWDGx->WINR));
}

/**
  * @}
  */

/** @defgroup IWDG_LL_EF_FLAG_Management FLAG_Management
  * @{
  */

/**
  * @brief  Check if flag Prescaler Value Update is set or not
  * @rmtoll SR           PVU           LL_IWDG_IsActiveFlag_PVU
  * @param  IWDGx IWDG Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_IWDG_IsActiveFlag_PVU(IWDG_TypeDef *IWDGx)
{
  return (READ_BIT(IWDGx->SR, IWDG_SR_PVU) == (IWDG_SR_PVU));
}

/**
  * @brief  Check if flag Reload Value Update is set or not
  * @rmtoll SR           RVU           LL_IWDG_IsActiveFlag_RVU
  * @param  IWDGx IWDG Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_IWDG_IsActiveFlag_RVU(IWDG_TypeDef *IWDGx)
{
  return (READ_BIT(IWDGx->SR, IWDG_SR_RVU) == (IWDG_SR_RVU));
}

/**
  * @brief  Check if flag Window Value Update is set or not
  * @rmtoll SR           WVU           LL_IWDG_IsActiveFlag_WVU
  * @param  IWDGx IWDG Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_IWDG_IsActiveFlag_WVU(IWDG_TypeDef *IWDGx)
{
  return (READ_BIT(IWDGx->SR, IWDG_SR_WVU) == (IWDG_SR_WVU));
}

/**
  * @brief  Check if all flags Prescaler, Reload & Window Value Update are reset or not
  * @rmtoll SR           PVU           LL_IWDG_IsReady\n
  *         SR           WVU           LL_IWDG_IsReady\n
  *         SR           RVU           LL_IWDG_IsReady
  * @param  IWDGx IWDG Instance
  * @retval State of bits (1 or 0).
  */
__STATIC_INLINE uint32_t LL_IWDG_IsReady(IWDG_TypeDef *IWDGx)
{
  return (READ_BIT(IWDGx->SR, IWDG_SR_PVU | IWDG_SR_RVU | IWDG_SR_WVU) == 0);
}

/**
  * @}
  */


/**
  * @}
  */

/**
  * @}
  */

#endif /* IWDG) */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __STM32L4xx_LL_IWDG_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
