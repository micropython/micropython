/**
  ******************************************************************************
  * @file    stm32n6xx_ll_iwdg.h
  * @author  MCD Application Team
  * @brief   Header file of IWDG LL module.
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
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32N6xx_LL_IWDG_H
#define STM32N6xx_LL_IWDG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx.h"

/** @addtogroup STM32N6xx_LL_Driver
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
#define LL_IWDG_KEY_RELOAD                 0x0000AAAAU               /*!< IWDG Reload Counter Enable   */
#define LL_IWDG_KEY_ENABLE                 0x0000CCCCU               /*!< IWDG Peripheral Enable       */
#define LL_IWDG_KEY_WR_ACCESS_ENABLE       0x00005555U               /*!< IWDG KR Write Access Enable  */
#define LL_IWDG_KEY_WR_ACCESS_DISABLE      0x00000000U               /*!< IWDG KR Write Access Disable */
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
#define LL_IWDG_PRESCALER_4                0x00000000U                                   /*!< Divider by 4    */
#define LL_IWDG_PRESCALER_8                (IWDG_PR_PR_0)                                /*!< Divider by 8    */
#define LL_IWDG_PRESCALER_16               (IWDG_PR_PR_1)                                /*!< Divider by 16   */
#define LL_IWDG_PRESCALER_32               (IWDG_PR_PR_1 | IWDG_PR_PR_0)                 /*!< Divider by 32   */
#define LL_IWDG_PRESCALER_64               (IWDG_PR_PR_2)                                /*!< Divider by 64   */
#define LL_IWDG_PRESCALER_128              (IWDG_PR_PR_2 | IWDG_PR_PR_0)                 /*!< Divider by 128  */
#define LL_IWDG_PRESCALER_256              (IWDG_PR_PR_2 | IWDG_PR_PR_1)                 /*!< Divider by 256  */
#define LL_IWDG_PRESCALER_512              (IWDG_PR_PR_2 | IWDG_PR_PR_1 | IWDG_PR_PR_0)  /*!< Divider by 512  */
#define LL_IWDG_PRESCALER_1024             IWDG_PR_PR_3                                  /*!< Divider by 1024 */
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
  WRITE_REG(IWDGx->KR, LL_IWDG_KEY_ENABLE);
}

/**
  * @brief  Reloads IWDG counter with value defined in the reload register
  * @rmtoll KR           KEY           LL_IWDG_ReloadCounter
  * @param  IWDGx IWDG Instance
  * @retval None
  */
__STATIC_INLINE void LL_IWDG_ReloadCounter(IWDG_TypeDef *IWDGx)
{
  WRITE_REG(IWDGx->KR, LL_IWDG_KEY_RELOAD);
}

/**
  * @brief  Enable write access to IWDG_PR, IWDG_RLR and IWDG_WINR registers
  * @rmtoll KR           KEY           LL_IWDG_EnableWriteAccess
  * @param  IWDGx IWDG Instance
  * @retval None
  */
__STATIC_INLINE void LL_IWDG_EnableWriteAccess(IWDG_TypeDef *IWDGx)
{
  WRITE_REG(IWDGx->KR, LL_IWDG_KEY_WR_ACCESS_ENABLE);
}

/**
  * @brief  Disable write access to IWDG_PR, IWDG_RLR and IWDG_WINR registers
  * @rmtoll KR           KEY           LL_IWDG_DisableWriteAccess
  * @param  IWDGx IWDG Instance
  * @retval None
  */
__STATIC_INLINE void LL_IWDG_DisableWriteAccess(IWDG_TypeDef *IWDGx)
{
  WRITE_REG(IWDGx->KR, LL_IWDG_KEY_WR_ACCESS_DISABLE);
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
  *         @arg @ref LL_IWDG_PRESCALER_512
  *         @arg @ref LL_IWDG_PRESCALER_1024
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
  *         @arg @ref LL_IWDG_PRESCALER_512
  *         @arg @ref LL_IWDG_PRESCALER_1024
  */
__STATIC_INLINE uint32_t LL_IWDG_GetPrescaler(const IWDG_TypeDef *IWDGx)
{
  return (READ_REG(IWDGx->PR));
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
__STATIC_INLINE uint32_t LL_IWDG_GetReloadCounter(const IWDG_TypeDef *IWDGx)
{
  return (READ_REG(IWDGx->RLR));
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
__STATIC_INLINE uint32_t LL_IWDG_GetWindow(const IWDG_TypeDef *IWDGx)
{
  return (READ_REG(IWDGx->WINR));
}

/**
  * @}
  */

/** @defgroup IWDG_LL_EF_IT_Management IT_Management
  * @{
  */

/**
  * @brief  Specify comparator value that will be used to trig Early Wakeup interrupt
  * @rmtoll EWCR         EWIT          LL_IWDG_SetEwiTime
  * @param  IWDGx IWDG Instance
  * @param  Time Value between Min_Data=0 and Max_Data=0x0FFF
  * @retval None
  */
__STATIC_INLINE void LL_IWDG_SetEwiTime(IWDG_TypeDef *IWDGx, uint32_t Time)
{
  MODIFY_REG(IWDGx->EWCR, IWDG_EWCR_EWIT, Time);
}

/**
  * @brief  Get the Early Wakeup interrupt comparator value
  * @rmtoll EWCR         EWIT          LL_IWDG_GetEwiTime
  * @param  IWDGx IWDG Instance
  * @retval Value between Min_Data=0 and Max_Data=0x0FFF
  */
__STATIC_INLINE uint32_t LL_IWDG_GetEwiTime(const IWDG_TypeDef *IWDGx)
{
  return (READ_BIT(IWDGx->EWCR, IWDG_EWCR_EWIT));
}

/**
  * @brief  Enable Early wakeup interrupt
  * @rmtoll EWCR         EWIE          LL_IWDG_EnableIT_EWI
  * @param  IWDGx IWDG Instance
  * @retval None
  */
__STATIC_INLINE void LL_IWDG_EnableIT_EWI(IWDG_TypeDef *IWDGx)
{
  SET_BIT(IWDGx->EWCR, IWDG_EWCR_EWIE);
}

/**
  * @brief  Disable Early wakeup interrupt
  * @rmtoll EWCR         EWIE          LL_IWDG_DisableIT_EWI
  * @param  IWDGx IWDG Instance
  * @retval None
  */
__STATIC_INLINE void LL_IWDG_DisableIT_EWI(IWDG_TypeDef *IWDGx)
{
  CLEAR_BIT(IWDGx->EWCR, IWDG_EWCR_EWIE);
}

/**
  * @brief  Indicates whether Early wakeup interrupt is enable
  * @rmtoll EWCR         EWIE          LL_IWDG_IsEnabledIT_EWI
  * @param  IWDGx IWDG Instance
  * @retval None
  */
__STATIC_INLINE uint32_t LL_IWDG_IsEnabledIT_EWI(const IWDG_TypeDef *IWDGx)
{
  return ((READ_BIT(IWDGx->EWCR, IWDG_EWCR_EWIE) == (IWDG_EWCR_EWIE)) ? 1UL : 0UL);
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
__STATIC_INLINE uint32_t LL_IWDG_IsActiveFlag_PVU(const IWDG_TypeDef *IWDGx)
{
  return ((READ_BIT(IWDGx->SR, IWDG_SR_PVU) == (IWDG_SR_PVU)) ? 1UL : 0UL);
}

/**
  * @brief  Check if flag Reload Value Update is set or not
  * @rmtoll SR           RVU           LL_IWDG_IsActiveFlag_RVU
  * @param  IWDGx IWDG Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_IWDG_IsActiveFlag_RVU(const IWDG_TypeDef *IWDGx)
{
  return ((READ_BIT(IWDGx->SR, IWDG_SR_RVU) == (IWDG_SR_RVU)) ? 1UL : 0UL);
}

/**
  * @brief  Check if flag Window Value Update is set or not
  * @rmtoll SR           WVU           LL_IWDG_IsActiveFlag_WVU
  * @param  IWDGx IWDG Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_IWDG_IsActiveFlag_WVU(const IWDG_TypeDef *IWDGx)
{
  return ((READ_BIT(IWDGx->SR, IWDG_SR_WVU) == (IWDG_SR_WVU)) ? 1UL : 0UL);
}

/**
  * @brief  Check if flag EWI Value Update is set or not
  * @rmtoll SR           EVU           LL_IWDG_IsActiveFlag_EWU
  * @param  IWDGx IWDG Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_IWDG_IsActiveFlag_EWU(const IWDG_TypeDef *IWDGx)
{
  return ((READ_BIT(IWDGx->SR, IWDG_SR_EWU) == (IWDG_SR_EWU)) ? 1UL : 0UL);
}

/**
  * @brief  Check if all flags Prescaler, Reload, Window & Early Interrupt Value Update are reset or not
  * @rmtoll SR           PVU           LL_IWDG_IsReady\n
  *         SR           RVU           LL_IWDG_IsReady\n
  *         SR           WVU           LL_IWDG_IsReady\n
  *         SR           EWU           LL_IWDG_IsReady
  * @param  IWDGx IWDG Instance
  * @retval State of bits (1 or 0).
  */
__STATIC_INLINE uint32_t LL_IWDG_IsReady(const IWDG_TypeDef *IWDGx)
{
  return ((READ_BIT(IWDGx->SR, IWDG_SR_PVU | IWDG_SR_RVU | IWDG_SR_WVU | IWDG_SR_EWU) == 0U) ? 1UL : 0UL);
}

/**
  * @brief  Check if IWDG has been started or not
  * @rmtoll SR           ONF           LL_IWDG_IsActiveFlag_ONF
  * @param  IWDGx IWDG Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_IWDG_IsActiveFlag_ONF(const IWDG_TypeDef *IWDGx)
{
  return ((READ_BIT(IWDGx->SR, IWDG_SR_ONF) == (IWDG_SR_ONF)) ? 1UL : 0UL);
}

/**
  * @brief  Check if Early Wakeup interrupt flag is set or not
  * @rmtoll SR           EWIF          LL_IWDG_IsActiveFlag_EWIF
  * @param  IWDGx IWDG Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_IWDG_IsActiveFlag_EWIF(const IWDG_TypeDef *IWDGx)
{
  return ((READ_BIT(IWDGx->SR, IWDG_SR_EWIF) == (IWDG_SR_EWIF)) ? 1UL : 0UL);
}

/**
  * @brief  Clear the Early Wakeup interrupt flag
  * @rmtoll EWCR         ICR          LL_IWDG_ClearFlag_EWIF
  * @param  IWDGx IWDG Instance
  * @retval None
  */
__STATIC_INLINE void LL_IWDG_ClearFlag_EWIF(IWDG_TypeDef *IWDGx)
{
  WRITE_REG(IWDGx->ICR, IWDG_ICR_EWIC);
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

#endif /* IWDG */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* STM32N6xx_LL_IWDG_H */
