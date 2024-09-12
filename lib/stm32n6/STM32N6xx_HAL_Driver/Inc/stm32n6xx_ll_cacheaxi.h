/**
  ******************************************************************************
  * @file    stm32n6xx_ll_cacheaxi.h
  * @author  MCD Application Team
  * @brief   Header file of CACHEAXI LL module.
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

/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef STM32N6xx_LL_CACHEAXI_H
#define STM32N6xx_LL_CACHEAXI_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes -----------------------------------------------------------------*/
#include "stm32n6xx.h"

/** @addtogroup STM32N6xx_LL_Driver
  * @{
  */

/** @defgroup CACHEAXI_LL CACHEAXI
  * @{
  */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/** @defgroup CACHEAXI_Exported_Constants CACHEAXI Exported Constants
  * @{
  */
/** @defgroup CACHEAXI_Command_Operation Command Operation
  * @{
  */
#define LL_CACHEAXI_COMMAND_NO_OPERATION                    (0x00000000)
#define LL_CACHEAXI_COMMAND_CLEAN_BY_ADDR                   CACHEAXI_CR2_CACHECMD_0
#define LL_CACHEAXI_COMMAND_CLEAN_INVALIDATE_BY_ADDR        (CACHEAXI_CR2_CACHECMD_0|CACHEAXI_CR2_CACHECMD_1)
/**
  * @}
  */

/** @defgroup CACHEAXI_LL_EC_GET_FLAG Get Flags Defines
  * @brief    Flags defines which can be used with LL_CACHEAXI_ReadReg function
  * @{
  */
#define LL_CACHEAXI_SR_ERRF              CACHEAXI_SR_ERRF      /*!< Cache error flag */
#define LL_CACHEAXI_SR_BUSYF             CACHEAXI_SR_BUSYF     /*!< Busy flag */
#define LL_CACHEAXI_SR_CMDENDF           CACHEAXI_SR_CMDENDF   /*!< Command end flag */
#define LL_CACHEAXI_SR_BSYENDF           CACHEAXI_SR_BSYENDF   /*!< Full invalidate busy end flag */
#define LL_CACHEAXI_SR_BUSYCMDF          CACHEAXI_SR_BUSYCMDF  /*!< Command busy flag */
/**
  * @}
  */

/** @defgroup CACHEAXI_LL_EC_CLEAR_FLAG Clear Flags Defines
  * @brief    Flags defines which can be used with LL_CACHEAXI_WriteReg function
  * @{
  */
#define LL_CACHEAXI_FCR_CERRF            CACHEAXI_FCR_CERRF    /*!< Cache error flag */
#define LL_CACHEAXI_FCR_CBSYENDF         CACHEAXI_FCR_CBSYENDF /*!< Full invalidate busy end flag */
#define LL_CACHEAXI_FCR_CCMDENDF         CACHEAXI_FCR_CCMDENDF /*!< Command end flag*/
/**
  * @}
  */

/** @defgroup CACHEAXI_LL_EC_IT IT Defines
  * @brief    IT defines which can be used with LL_CACHEAXI_ReadReg and  LL_CACHEAXI_WriteReg functions
  * @{
  */
#define LL_CACHEAXI_IER_BSYENDIE         CACHEAXI_IER_BSYENDIE /*!< Busy end interrupt */
#define LL_CACHEAXI_IER_ERRIE            CACHEAXI_IER_ERRIE    /*!< Cache error interrupt */
#define LL_CACHEAXI_IER_CMDENDIE         CACHEAXI_IER_CMDENDIE /*!< Command end interrupt */
/**
  * @}
  */

/** @defgroup CACHEAXI_Monitor_Type Monitor type
  * @{
  */
#define LL_CACHEAXI_MONITOR_READ_HIT         CACHEAXI_CR1_RHITMEN   /*!< Read Hit monitoring */
#define LL_CACHEAXI_MONITOR_READ_MISS        CACHEAXI_CR1_RMISSMEN  /*!< Read Miss monitoring */
#define LL_CACHEAXI_MONITOR_WRITE_HIT        CACHEAXI_CR1_WHITMEN   /*!< Write Hit monitoring */
#define LL_CACHEAXI_MONITOR_WRITE_MISS       CACHEAXI_CR1_WMISSMEN  /*!< Write Miss monitoring */
#define LL_CACHEAXI_MONITOR_READALLOC_MISS   CACHEAXI_CR1_RAMMEN /*!< Read-alloc Miss monitoring */
#define LL_CACHEAXI_MONITOR_WRITEALLOC_MISS  CACHEAXI_CR1_WAMMEN /*!< Write-alloc Miss monitoring */
#define LL_CACHEAXI_MONITOR_WRITETHROUGH     CACHEAXI_CR1_WTMEN     /*!< Write-through monitoring */
#define LL_CACHEAXI_MONITOR_EVICTION         CACHEAXI_CR1_EVIMEN    /*!< Eviction monitoring */
#define LL_CACHEAXI_MONITOR_ALL             (CACHEAXI_CR1_RHITMEN | CACHEAXI_CR1_RMISSMEN   \
                                             | CACHEAXI_CR1_WHITMEN | CACHEAXI_CR1_WMISSMEN \
                                             | CACHEAXI_CR1_RAMMEN  | CACHEAXI_CR1_WAMMEN   \
                                             | CACHEAXI_CR1_WTMEN   | CACHEAXI_CR1_EVIMEN)
/**
  * @}
  */

/**
  * @}
  */

/* Exported macros --------------------------------------------------------*/
/** @defgroup CACHEAXI_LL_Exported_Macros CACHEAXI Exported Macros
  * @{
  */

/** @defgroup CACHEAXI_LL_EM_WRITE_READ Common write and read registers Macros
  * @{
  */

/**
  * @brief  Write a value in CACHEAXI register
  * @param  __INSTANCE__ CACHEAXI Instance
  * @param  __REG__ Register to be written
  * @param  __VALUE__ Value to be written in the register
  * @retval None
  */
#define LL_CACHEAXI_WriteReg(__INSTANCE__, __REG__, __VALUE__) WRITE_REG(__INSTANCE__->__REG__, (__VALUE__))

/**
  * @brief  Read a value in CACHEAXI register
  * @param  __INSTANCE__ CACHEAXI Instance
  * @param  __REG__ Register to be read
  * @retval Register value
  */
#define LL_CACHEAXI_ReadReg(__INSTANCE__, __REG__) READ_REG(__INSTANCE__->__REG__)
/**
  * @}
  */

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup CACHEAXI_LL_Exported_Functions CACHEAXI Exported Functions
  * @{
  */

/** @defgroup CACHEAXI_LL_EF_Configuration Configuration
  * @{
  */

/**
  * @brief  Enable the selected CACHEAXI instance.
  * @rmtoll CR1       EN           LL_CACHEAXI_Enable
  * @param  CACHEAXIx CACHEAXI instance
  * @retval None
  */
__STATIC_INLINE void LL_CACHEAXI_Enable(CACHEAXI_TypeDef *CACHEAXIx)
{
  SET_BIT(CACHEAXIx->CR1, CACHEAXI_CR1_EN);
}

/**
  * @brief  Disable the selected CACHEAXI instance.
  * @rmtoll CR1       EN           LL_CACHEAXI_Disable
  * @param  CACHEAXIx CACHEAXI instance
  * @retval None
  */
__STATIC_INLINE void LL_CACHEAXI_Disable(CACHEAXI_TypeDef *CACHEAXIx)
{
  CLEAR_BIT(CACHEAXIx->CR1, CACHEAXI_CR1_EN);
}

/**
  * @brief  Get the selected CACHEAXI instance enable state.
  * @rmtoll CR1       EN           LL_CACHEAXI_IsEnabled
  * @param  CACHEAXIx CACHEAXI instance
  * @retval 0: CACHEAXI is disabled, 1: CACHEAXI is enabled.
  */
__STATIC_INLINE uint32_t LL_CACHEAXI_IsEnabled(const CACHEAXI_TypeDef *CACHEAXIx)
{
  return ((READ_BIT(CACHEAXIx->CR1, CACHEAXI_CR1_EN) == (CACHEAXI_CR1_EN)) ? 1UL : 0UL);
}

/**
  * @brief  Set the cacheaxi instance start command address.
  * @rmtoll CR2       CMDRSADDRR          LL_CACHEAXI_SetStartAddress
  * @param  addr cacheaxi command start address(Clean, Invalidate or Clean and Invalidate).
  * @param  CACHEAXIx CACHEAXI instance
  * @retval None
  */
__STATIC_INLINE void LL_CACHEAXI_SetStartAddress(CACHEAXI_TypeDef *CACHEAXIx, uint32_t addr)
{
  WRITE_REG(CACHEAXIx->CMDRSADDRR, addr);
}

/**
  * @brief  Get the cacheaxi command start address.
  * @rmtoll CR2       CMDRSADDRR         LL_CACHEAXI_GetStartAddress
  * @param  CACHEAXIx CACHEAXI instance
  * @retval Start address of cacheaxi command
  */
__STATIC_INLINE uint32_t LL_CACHEAXI_GetStartAddress(const CACHEAXI_TypeDef *CACHEAXIx)
{
  return (uint32_t)(READ_REG(CACHEAXIx->CMDRSADDRR));
}

/**
  * @brief  Set the cacheaxi instance End command address.
  * @rmtoll CR2       CMDREADDRR          LL_CACHEAXI_SetEndAddress
  * @param  CACHEAXIx CACHEAXI instance
  * @param  addr cacheaxi command end address(Clean, Invalidate or Clean and Invalidate).
  * @retval None
  */
__STATIC_INLINE void LL_CACHEAXI_SetEndAddress(CACHEAXI_TypeDef *CACHEAXIx, uint32_t addr)
{
  WRITE_REG(CACHEAXIx->CMDREADDRR, addr);
}

/**
  * @brief  Get the cacheaxi command End address.
  * @rmtoll CR2       CMDREADDRR          LL_CACHEAXI_GetEndAddress
  * @param  CACHEAXIx CACHEAXI instance
  * @retval End address of cacheaxi command
  */
__STATIC_INLINE uint32_t LL_CACHEAXI_GetEndAddress(const CACHEAXI_TypeDef *CACHEAXIx)
{
  return (uint32_t)(READ_REG(CACHEAXIx->CMDREADDRR));
}

/**
  * @brief  Set Dcache command.
  * @rmtoll CR2       CACHECMD          LL_CACHEAXI_SetCommand
  * @param  CACHEAXIx CACHEAXI instance
  * @param  Command command to be applied for the cacheaxi
  *         Command can be one of the following values:
  *         @arg @ref LL_CACHEAXI_COMMAND_CLEAN_BY_ADDR
  *         @arg @ref LL_CACHEAXI_COMMAND_CLEAN_INVALIDATE_BY_ADDR
  *         @arg @ref LL_CACHEAXI_COMMAND_NO_OPERATION
  * @retval None
  */
__STATIC_INLINE void LL_CACHEAXI_SetCommand(CACHEAXI_TypeDef *CACHEAXIx, uint32_t Command)
{
  /* Set cacheaxi command */
  MODIFY_REG(CACHEAXIx->CR2, CACHEAXI_CR2_CACHECMD, Command);
}

/**
  * @brief  Set Dcache command.
  * @rmtoll CR2       CACHECMD          LL_CACHEAXI_GetCommand
  * @param  CACHEAXIx CACHEAXI instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_CACHEAXI_COMMAND_NO_OPERATION
  *         @arg @ref LL_CACHEAXI_COMMAND_CLEAN_BY_ADDR
  *         @arg @ref LL_CACHEAXI_COMMAND_CLEAN_INVALIDATE_BY_ADDR
  */
__STATIC_INLINE uint32_t LL_CACHEAXI_GetCommand(const CACHEAXI_TypeDef *CACHEAXIx)
{
  /*Get Dcache Command */
  return (uint32_t)(READ_BIT(CACHEAXIx->CR2, CACHEAXI_CR2_CACHECMD));
}

/**
  * @brief  Launch Dcache Command.
  * @rmtoll CR2       CACHECMD          LL_CACHEAXI_StartCommand
  * @param  CACHEAXIx CACHEAXI instance
  * @retval None
  */
__STATIC_INLINE void LL_CACHEAXI_StartCommand(CACHEAXI_TypeDef *CACHEAXIx)
{
  SET_BIT(CACHEAXIx->CR2, CACHEAXI_CR2_STARTCMD);
}

/**
  * @brief  Invalidate the Data cache.
  * @rmtoll CR1       CACHEINV          LL_CACHEAXI_Invalidate
  * @param  CACHEAXIx CACHEAXI instance
  * @retval None
  */
__STATIC_INLINE void LL_CACHEAXI_Invalidate(CACHEAXI_TypeDef *CACHEAXIx)
{
  SET_BIT(CACHEAXIx->CR1, CACHEAXI_CR1_CACHEINV);
}

/**
  * @}
  */


/** @defgroup CACHEAXI_LL_EF_Monitor Monitor
  * @{
  */

/**
  * @brief  Enable the hit/miss monitor(s).
  * @rmtoll CR1  (RMISSMEN/RHITMEN/WMISSMEN/WHITMEN/RAMMEN/WAMMEN/WTMEN/EVIMEN)   LL_CACHEAXI_EnableMonitors
  * @param  CACHEAXIx CACHEAXI instance
  * @param  Monitors This parameter can be one or a combination of the following values:
  *            @arg LL_CACHEAXI_MONITOR_READ_HIT
  *            @arg LL_CACHEAXI_MONITOR_READ_MISS
  *            @arg LL_CACHEAXI_MONITOR_WRITE_HIT
  *            @arg LL_CACHEAXI_MONITOR_WRITE_MISS
  *            @arg LL_CACHEAXI_MONITOR_READALLOC_MISS
  *            @arg LL_CACHEAXI_MONITOR_WRITEALLOC_MISS
  *            @arg LL_CACHEAXI_MONITOR_WRITETHROUGH
  *            @arg LL_CACHEAXI_MONITOR_EVICTION
  *            @arg LL_CACHEAXI_MONITOR_ALL
  * @retval None
  */
__STATIC_INLINE void LL_CACHEAXI_EnableMonitors(CACHEAXI_TypeDef *CACHEAXIx, uint32_t Monitors)
{
  SET_BIT(CACHEAXIx->CR1, Monitors);
}

/**
  * @brief  Disable the hit/miss monitor(s).
  * @rmtoll CR1  (RMISSMEN/RHITMEN/WMISSMEN/WHITMEN/RAMMEN/WAMMEN/WTMEN/EVIMEN)   LL_CACHEAXI_DisableMonitors
  * @param  CACHEAXIx CACHEAXI instance
  * @param  Monitors This parameter can be one or a combination of the following values:
  *            @arg LL_CACHEAXI_MONITOR_READ_HIT
  *            @arg LL_CACHEAXI_MONITOR_READ_MISS
  *            @arg LL_CACHEAXI_MONITOR_WRITE_HIT
  *            @arg LL_CACHEAXI_MONITOR_WRITE_MISS
  *            @arg LL_CACHEAXI_MONITOR_READALLOC_MISS
  *            @arg LL_CACHEAXI_MONITOR_WRITEALLOC_MISS
  *            @arg LL_CACHEAXI_MONITOR_WRITETHROUGH
  *            @arg LL_CACHEAXI_MONITOR_EVICTION
  *            @arg LL_CACHEAXI_MONITOR_ALL
  * @retval None
  */
__STATIC_INLINE void LL_CACHEAXI_DisableMonitors(CACHEAXI_TypeDef *CACHEAXIx, uint32_t Monitors)
{
  CLEAR_BIT(CACHEAXIx->CR1, Monitors);
}

/**
  * @brief  Return the hit/miss monitor(s) enable state.
  * @rmtoll CR1  (RMISSMEN/RHITMEN/WMISSMEN/WHITMEN/RAMMEN/WAMMEN/WTMEN/EVIMEN)   LL_CACHEAXI_IsEnabledMonitors
  * @param  CACHEAXIx CACHEAXI instance
  * @param  Monitors This parameter can be one or a combination of the following values:
  *            @arg LL_CACHEAXI_MONITOR_READ_HIT
  *            @arg LL_CACHEAXI_MONITOR_READ_MISS
  *            @arg LL_CACHEAXI_MONITOR_WRITE_HIT
  *            @arg LL_CACHEAXI_MONITOR_WRITE_MISS
  *            @arg LL_CACHEAXI_MONITOR_READALLOC_MISS
  *            @arg LL_CACHEAXI_MONITOR_WRITEALLOC_MISS
  *            @arg LL_CACHEAXI_MONITOR_WRITETHROUGH
  *            @arg LL_CACHEAXI_MONITOR_EVICTION
  *            @arg LL_CACHEAXI_MONITOR_ALL
  * @retval State of parameter value (1 or 0).
  */
__STATIC_INLINE uint32_t LL_CACHEAXI_IsEnabledMonitors(const CACHEAXI_TypeDef *CACHEAXIx, uint32_t Monitors)
{
  return (((READ_BIT(CACHEAXIx->CR1, (CACHEAXI_CR1_WMISSMEN | CACHEAXI_CR1_WHITMEN | CACHEAXI_CR1_RMISSMEN | \
                                      CACHEAXI_CR1_RHITMEN  | CACHEAXI_CR1_RAMMEN  | CACHEAXI_CR1_WAMMEN   | \
                                      CACHEAXI_CR1_WTMEN    | CACHEAXI_CR1_EVIMEN))\
            & Monitors) == (Monitors)) ? 1UL : 0UL);
}

/**
  * @brief  Reset the Data Cache performance monitoring.
  * @rmtoll CR1  (RHITMRST/RMISSMRST/WHITMRST/WMISSMRST/RAMMRST/WAMMRST/WTMRST/EVIMRST)   LL_CACHEAXI_ResetMonitors
  * @param  CACHEAXIx CACHEAXI instance
  * @param  Monitors Monitoring type
  *         This parameter can be a combination of the following values:
  *            @arg LL_CACHEAXI_MONITOR_READ_HIT
  *            @arg LL_CACHEAXI_MONITOR_READ_MISS
  *            @arg LL_CACHEAXI_MONITOR_WRITE_HIT
  *            @arg LL_CACHEAXI_MONITOR_WRITE_MISS
  *            @arg LL_CACHEAXI_MONITOR_READALLOC_MISS
  *            @arg LL_CACHEAXI_MONITOR_WRITEALLOC_MISS
  *            @arg LL_CACHEAXI_MONITOR_WRITETHROUGH
  *            @arg LL_CACHEAXI_MONITOR_EVICTION
  *            @arg LL_CACHEAXI_MONITOR_ALL
  * @retval None
  */
__STATIC_INLINE void LL_CACHEAXI_ResetMonitors(CACHEAXI_TypeDef *CACHEAXIx, uint32_t Monitors)
{
  /* Reset */
  SET_BIT(CACHEAXIx->CR1, (Monitors << 2U));

  /* Release reset */
  CLEAR_BIT(CACHEAXIx->CR1, (Monitors << 2U));
}

/**
  * @brief  Get the Read Hit monitor Value
  * @rmtoll RHMONR       LL_CACHEAXI_Monitor_GetReadHitValue
  * @param  CACHEAXIx CACHEAXI instance
  * @retval Value between Min_Data=0 and Max_Data=0xFFFFFFFF
  */
__STATIC_INLINE uint32_t LL_CACHEAXI_Monitor_GetReadHitValue(CACHEAXI_TypeDef *CACHEAXIx)
{
  return CACHEAXIx->RHMONR;
}

/**
  * @brief  Get the Read Miss monitor Value
  * @rmtoll RMMONR       LL_CACHEAXI_Monitor_GetReadMissValue
  * @param  CACHEAXIx CACHEAXI instance
  * @retval Value between Min_Data=0 and Max_Data=0xFFFF
  */
__STATIC_INLINE uint32_t LL_CACHEAXI_Monitor_GetReadMissValue(CACHEAXI_TypeDef *CACHEAXIx)
{
  return CACHEAXIx->RMMONR;
}

/**
  * @brief  Get the Write Hit monitor Value
  * @rmtoll WHMONR       LL_CACHEAXI_Monitor_GetWriteHitValue
  * @param  CACHEAXIx CACHEAXI instance
  * @retval Value between Min_Data=0 and Max_Data=0xFFFFFFFF
  */
__STATIC_INLINE uint32_t LL_CACHEAXI_Monitor_GetWriteHitValue(CACHEAXI_TypeDef *CACHEAXIx)
{
  return CACHEAXIx->WHMONR;
}

/**
  * @brief  Get the Write Miss monitor Value
  * @rmtoll WMMONR       LL_CACHEAXI_Monitor_GetWriteMissValue
  * @param  CACHEAXIx CACHEAXI instance
  * @retval Value between Min_Data=0 and Max_Data=0xFFFF
  */
__STATIC_INLINE uint32_t LL_CACHEAXI_Monitor_GetWriteMissValue(CACHEAXI_TypeDef *CACHEAXIx)
{
  return CACHEAXIx->WMMONR;
}

/**
  * @brief  Get the Read-allocate Miss monitor Value
  * @rmtoll WMMONR       LL_CACHEAXI_Monitor_GetReadAllocMissValue
  * @param  CACHEAXIx CACHEAXI instance
  * @retval Value between Min_Data=0 and Max_Data=0xFFFF
  */
__STATIC_INLINE uint32_t LL_CACHEAXI_Monitor_GetReadAllocMissValue(CACHEAXI_TypeDef *CACHEAXIx)
{
  return CACHEAXIx->RAMMONR;
}

/**
  * @brief  Get the Write-allocate Miss monitor Value
  * @rmtoll WMMONR       LL_CACHEAXI_Monitor_GetWriteAllocMissValue
  * @param  CACHEAXIx CACHEAXI instance
  * @retval Value between Min_Data=0 and Max_Data=0xFFFF
  */
__STATIC_INLINE uint32_t LL_CACHEAXI_Monitor_GetWriteAllocMissValue(CACHEAXI_TypeDef *CACHEAXIx)
{
  return CACHEAXIx->WAMMONR;
}

/**
  * @brief  Get the Write-through monitor Value
  * @rmtoll WMMONR       LL_CACHEAXI_Monitor_GetWriteThroughValue
  * @param  CACHEAXIx CACHEAXI instance
  * @retval Value between Min_Data=0 and Max_Data=0xFFFF
  */
__STATIC_INLINE uint32_t LL_CACHEAXI_Monitor_GetWriteThroughValue(CACHEAXI_TypeDef *CACHEAXIx)
{
  return CACHEAXIx->WTMONR;
}

/**
  * @brief  Get the Eviction monitor Value
  * @rmtoll WMMONR       LL_CACHEAXI_Monitor_GetEvictionValue
  * @param  CACHEAXIx CACHEAXI instance
  * @retval Value between Min_Data=0 and Max_Data=0xFFFF
  */
__STATIC_INLINE uint32_t LL_CACHEAXI_Monitor_GetEvictionValue(CACHEAXI_TypeDef *CACHEAXIx)
{
  return CACHEAXIx->EVIMONR;
}

/**
  * @}
  */

/** @defgroup CACHEAXI_LL_EF_IT_Management IT-Management
  * @{
  */

/**
  * @brief  Enable BusyEnd interrupt.
  * @rmtoll IER         BSYENDIE          LL_CACHEAXI_EnableIT_BSYEND
  * @param  CACHEAXIx CACHEAXI instance
  * @retval None
  */
__STATIC_INLINE void LL_CACHEAXI_EnableIT_BSYEND(CACHEAXI_TypeDef *CACHEAXIx)
{
  SET_BIT(CACHEAXIx->IER, CACHEAXI_IER_BSYENDIE);
}

/**
  * @brief  Disable BusyEnd interrupt.
  * @rmtoll IER         BSYENDIE          LL_CACHEAXI_DisableIT_BSYEND
  * @param  CACHEAXIx CACHEAXI instance
  * @retval None
  */
__STATIC_INLINE void LL_CACHEAXI_DisableIT_BSYEND(CACHEAXI_TypeDef *CACHEAXIx)
{
  CLEAR_BIT(CACHEAXIx->IER, CACHEAXI_IER_BSYENDIE);
}

/**
  * @brief  Indicates whether the Busyend interrupt is enabled.
  * @rmtoll IER         BSYENDIE          LL_CACHEAXI_IsEnabledIT_BSYEND
  * @param  CACHEAXIx CACHEAXI instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_CACHEAXI_IsEnabledIT_BSYEND(const CACHEAXI_TypeDef *CACHEAXIx)
{
  return ((READ_BIT(CACHEAXIx->IER, CACHEAXI_IER_BSYENDIE) == (CACHEAXI_IER_BSYENDIE)) ? 1UL : 0UL);
}

/**
  * @brief  Enable Error interrupt.
  * @rmtoll IER         ERRIE          LL_CACHEAXI_EnableIT_ERR
  * @param  CACHEAXIx CACHEAXI instance
  * @retval None
  */
__STATIC_INLINE void LL_CACHEAXI_EnableIT_ERR(CACHEAXI_TypeDef *CACHEAXIx)
{
  SET_BIT(CACHEAXIx->IER, CACHEAXI_IER_ERRIE);
}

/**
  * @brief  Disable Error interrupt.
  * @rmtoll IER         ERRIE          LL_CACHEAXI_DisableIT_ERR
  * @param  CACHEAXIx CACHEAXI instance
  * @retval None
  */
__STATIC_INLINE void LL_CACHEAXI_DisableIT_ERR(CACHEAXI_TypeDef *CACHEAXIx)
{
  CLEAR_BIT(CACHEAXIx->IER, CACHEAXI_IER_ERRIE);
}

/**
  * @brief  Indicates whether the Error interrupt is enabled.
  * @rmtoll IER         ERRIE          LL_CACHEAXI_IsEnabledIT_ERR
  * @param  CACHEAXIx CACHEAXI instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_CACHEAXI_IsEnabledIT_ERR(const CACHEAXI_TypeDef *CACHEAXIx)
{
  return ((READ_BIT(CACHEAXIx->IER, CACHEAXI_IER_ERRIE) == (CACHEAXI_IER_ERRIE)) ? 1UL : 0UL);
}

/**
  * @brief  Enable command end interrupt.
  * @rmtoll IER         CMDENDIE         LL_CACHEAXI_EnableIT_CMDEND
  * @param  CACHEAXIx CACHEAXI instance
  * @retval None
  */
__STATIC_INLINE void LL_CACHEAXI_EnableIT_CMDEND(CACHEAXI_TypeDef *CACHEAXIx)
{
  SET_BIT(CACHEAXIx->IER, CACHEAXI_IER_CMDENDIE);
}

/**
  * @brief  Disable command end interrupt.
  * @rmtoll IER         CMDENDIE         LL_CACHEAXI_DisableIT_CMDEND
  * @param  CACHEAXIx CACHEAXI instance
  * @retval None
  */
__STATIC_INLINE void LL_CACHEAXI_DisableIT_CMDEND(CACHEAXI_TypeDef *CACHEAXIx)
{
  CLEAR_BIT(CACHEAXIx->IER, CACHEAXI_IER_CMDENDIE);
}

/**
  * @brief  Indicates whether the command end interrupt is enabled.
  * @rmtoll IER         CMDENDIE          LL_CACHEAXI_IsEnabledIT_CMDEND
  * @param  CACHEAXIx CACHEAXI instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_CACHEAXI_IsEnabledIT_CMDEND(const CACHEAXI_TypeDef *CACHEAXIx)
{
  return ((READ_BIT(CACHEAXIx->IER, CACHEAXI_IER_CMDENDIE) == (CACHEAXI_IER_CMDENDIE)) ? 1UL : 0UL);
}

/**
  * @brief  Clear full invalidate busy end flag.
  * @rmtoll FCR      CBSYENDF         LL_CACHEAXI_ClearFlag_BSYEND
  * @param  CACHEAXIx CACHEAXI instance
  * @retval None
  */
__STATIC_INLINE void LL_CACHEAXI_ClearFlag_BSYEND(CACHEAXI_TypeDef *CACHEAXIx)
{
  WRITE_REG(CACHEAXIx->FCR, CACHEAXI_FCR_CBSYENDF);
}

/**
  * @brief  Clear cache error flag.
  * @rmtoll FCR      CERRF        LL_CACHEAXI_ClearFlag_ERR
  * @param  CACHEAXIx CACHEAXI instance
  * @retval None
  */
__STATIC_INLINE void LL_CACHEAXI_ClearFlag_ERR(CACHEAXI_TypeDef *CACHEAXIx)
{
  WRITE_REG(CACHEAXIx->FCR, CACHEAXI_FCR_CERRF);
}

/**
  * @brief  Clear command end flag.
  * @rmtoll FCR      CCMDENDF        LL_CACHEAXI_ClearFlag_CMDEND
  * @param  CACHEAXIx CACHEAXI instance
  * @retval None
  */
__STATIC_INLINE void LL_CACHEAXI_ClearFlag_CMDEND(CACHEAXI_TypeDef *CACHEAXIx)
{
  WRITE_REG(CACHEAXIx->FCR, CACHEAXI_FCR_CCMDENDF);
}

/**
  * @brief  Get flag Dcache BUSY.
  * @rmtoll SR      BUSYF            LL_CACHEAXI_IsActiveFlag_BUSY
  * @param  CACHEAXIx CACHEAXI instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_CACHEAXI_IsActiveFlag_BUSY(const CACHEAXI_TypeDef *CACHEAXIx)
{
  return ((READ_BIT(CACHEAXIx->SR, CACHEAXI_SR_BUSYF) == (CACHEAXI_SR_BUSYF)) ? 1UL : 0UL);
}

/**
  * @brief  Get flag Dcache Busyend.
  * @rmtoll SR      BSYENDF            LL_CACHEAXI_IsActiveFlag_BSYEND
  * @param  CACHEAXIx CACHEAXI instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_CACHEAXI_IsActiveFlag_BSYEND(const CACHEAXI_TypeDef *CACHEAXIx)
{
  return ((READ_BIT(CACHEAXIx->SR, CACHEAXI_SR_BSYENDF) == (CACHEAXI_SR_BSYENDF)) ? 1UL : 0UL);
}

/**
  * @brief  Get flag Dcache Error.
  * @rmtoll SR      ERRF            LL_CACHEAXI_IsActiveFlag_ERR
  * @param  CACHEAXIx CACHEAXI instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_CACHEAXI_IsActiveFlag_ERR(const CACHEAXI_TypeDef *CACHEAXIx)
{
  return ((READ_BIT(CACHEAXIx->SR, CACHEAXI_SR_ERRF) == (CACHEAXI_SR_ERRF)) ? 1UL : 0UL);
}

/**
  * @brief  Get flag Dcache Busy command.
  * @rmtoll SR      BUSYCMDF            LL_CACHEAXI_IsActiveFlag_BUSYCMD
  * @param  CACHEAXIx CACHEAXI instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_CACHEAXI_IsActiveFlag_BUSYCMD(const CACHEAXI_TypeDef *CACHEAXIx)
{
  return ((READ_BIT(CACHEAXIx->SR, CACHEAXI_SR_BUSYCMDF) == (CACHEAXI_SR_BUSYCMDF)) ? 1UL : 0UL);
}

/**
  * @brief  Get flag Dcache command end.
  * @rmtoll SR      CMDENDF            LL_CACHEAXI_IsActiveFlag_CMDEND
  * @param  CACHEAXIx CACHEAXI instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_CACHEAXI_IsActiveFlag_CMDEND(const CACHEAXI_TypeDef *CACHEAXIx)
{
  return ((READ_BIT(CACHEAXIx->SR, CACHEAXI_SR_CMDENDF) == (CACHEAXI_SR_CMDENDF)) ? 1UL : 0UL);
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

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* STM32N6xx_LL_CACHEAXI_H */
