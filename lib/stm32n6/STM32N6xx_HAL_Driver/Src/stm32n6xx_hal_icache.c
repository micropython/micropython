/**
  ******************************************************************************
  * @file    stm32n6xx_hal_icache.c
  * @author  MCD Application Team
  * @brief   ICACHE HAL module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities of the Instruction Cache (ICACHE).
  *           + Initialization and Configuration
  *           + Invalidate functions
  *           + Monitoring management
  *           + Memory address remap management
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
                        ##### ICACHE main features #####
  ==============================================================================
  [..]
    The Texture Cache (ICACHE) is introduced on AXI read-only texture port of
    the GPU2D to improve performance when reading texture data from internal and
    external memories.

  ===============================================================================
                        ##### How to use this driver #####
  ===============================================================================
  [..]
     The ICACHE HAL driver can be used as follows:

    (#) Optionally configure the Instruction Cache mode with
        HAL_ICACHE_ConfigAssociativityMode() if the default configuration
        does not suit the application requirements.

    (#) Enable and disable the Instruction Cache with respectively
        HAL_ICACHE_Enable() and HAL_ICACHE_Disable().
        Use HAL_ICACHE_IsEnabled() to get the Instruction Cache status.
        To ensure a deterministic cache behavior after power on, system reset or after
        a call to @ref HAL_ICACHE_Disable(), the application must call
        @ref HAL_ICACHE_WaitForInvalidateComplete(). Indeed on power on, system reset
        or cache disable, an automatic cache invalidation procedure is launched and the
        cache is bypassed until the operation completes.

    (#) Initiate the cache maintenance invalidation procedure with either
        HAL_ICACHE_Invalidate() (blocking mode) or HAL_ICACHE_Invalidate_IT()
        (interrupt mode). When interrupt mode is used, the callback function
        HAL_ICACHE_InvalidateCompleteCallback() is called when the invalidate
        procedure is complete. The function HAL_ICACHE_WaitForInvalidateComplete()
        may be called to wait for the end of the invalidate procedure automatically
        initiated when disabling the Instruction Cache with HAL_ICACHE_Disable().
        The cache operation is bypassed during the invalidation procedure.

    (#) Use the performance monitoring counters for Hit and Miss with the following
        functions: HAL_ICACHE_Monitor_Start(), HAL_ICACHE_Monitor_Stop(),
        HAL_ICACHE_Monitor_Reset(), HAL_ICACHE_Monitor_GetHitValue() and
        HAL_ICACHE_Monitor_GetMissValue()

  @endverbatim
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal.h"

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */

/** @defgroup ICACHE ICACHE
  * @brief HAL ICACHE module driver
  * @{
  */
#if defined(ICACHE) && defined (HAL_ICACHE_MODULE_ENABLED)

/* Private typedef -----------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/** @addtogroup ICACHE_Private_Constants ICACHE Private Constants
  * @{
  */
#define ICACHE_INVALIDATE_TIMEOUT_VALUE        1U   /* 1ms */
#define ICACHE_DISABLE_TIMEOUT_VALUE           1U   /* 1ms */

/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/** @defgroup ICACHE_Private_Macros ICACHE Private Macros
  * @{
  */

#define IS_ICACHE_ASSOCIATIVITY_MODE(__MODE__) (((__MODE__) == ICACHE_1WAY) || \
                                                ((__MODE__) == ICACHE_4WAYS))

#define IS_ICACHE_MONITOR_TYPE(__TYPE__)    (((__TYPE__) == ICACHE_MONITOR_HIT_MISS) || \
                                             ((__TYPE__) == ICACHE_MONITOR_HIT)      || \
                                             ((__TYPE__) == ICACHE_MONITOR_MISS))

/**
  * @}
  */

/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/** @defgroup ICACHE_Exported_Functions ICACHE Exported Functions
  * @{
  */

/** @defgroup ICACHE_Exported_Functions_Group1 Initialization and control functions
  * @brief    Initialization and control functions
  *
  @verbatim
  ==============================================================================
            ##### Initialization and control functions #####
  ==============================================================================
  [..]
    This section provides functions allowing to initialize and control the
    Instruction Cache (mode, invalidate procedure, performance counters).
  @endverbatim
  * @{
  */

/**
  * @brief  Configure the Instruction Cache cache associativity mode selection.
  * @param  AssociativityMode  Associativity mode selection
  *         This parameter can be one of the following values:
  *            @arg ICACHE_1WAY   1-way cache (direct mapped cache)
  *            @arg ICACHE_4WAYS  4-ways set associative cache (default)
  * @retval HAL status (HAL_OK/HAL_ERROR)
  */
HAL_StatusTypeDef HAL_ICACHE_ConfigAssociativityMode(uint32_t AssociativityMode)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check the parameters */
  assert_param(IS_ICACHE_ASSOCIATIVITY_MODE(AssociativityMode));

  /* Check cache is not enabled */
  if (READ_BIT(ICACHE->CR, ICACHE_CR_EN) != 0U)
  {
    status = HAL_ERROR;
  }
  else
  {
    MODIFY_REG(ICACHE->CR, ICACHE_CR_WAYSEL, AssociativityMode);
  }

  return status;
}

/**
  * @brief  DeInitialize the Instruction Cache.
  * @retval HAL status (HAL_OK)
  */
HAL_StatusTypeDef HAL_ICACHE_DeInit(void)
{
  /* Reset interrupt enable value */
  WRITE_REG(ICACHE->IER, 0U);

  /* Clear any pending flags */
  WRITE_REG(ICACHE->FCR, ICACHE_FCR_CBSYENDF | ICACHE_FCR_CERRF);

  /* Disable cache then set default associative mode value */
  CLEAR_BIT(ICACHE->CR, ICACHE_CR_EN);
  WRITE_REG(ICACHE->CR, ICACHE_CR_WAYSEL);

  /* Stop monitor and reset monitor values */
  CLEAR_BIT(ICACHE->CR, ICACHE_MONITOR_HIT_MISS);
  SET_BIT(ICACHE->CR, (ICACHE_MONITOR_HIT_MISS << 2U));
  CLEAR_BIT(ICACHE->CR, (ICACHE_MONITOR_HIT_MISS << 2U));


  return HAL_OK;
}

/**
  * @brief  Enable the Instruction Cache.
  * @note   This function always returns HAL_OK even if there is any ongoing
  *         cache operation. The Instruction Cache is bypassed until the
  *         cache operation completes.
  * @retval HAL status (HAL_OK)
  */
HAL_StatusTypeDef HAL_ICACHE_Enable(void)
{
  SET_BIT(ICACHE->CR, ICACHE_CR_EN);

  return HAL_OK;
}

/**
  * @brief  Disable the Instruction Cache.
  * @note   This function waits for the cache being disabled but
  *         not for the end of the automatic cache invalidation procedure.
  * @retval HAL status (HAL_OK/HAL_TIMEOUT)
  */
HAL_StatusTypeDef HAL_ICACHE_Disable(void)
{
  HAL_StatusTypeDef status = HAL_OK;
  uint32_t tickstart;

  /* Make sure BSYENDF is reset before to disable the instruction cache */
  /* as it automatically starts a cache invalidation procedure */
  WRITE_REG(ICACHE->FCR, ICACHE_FCR_CBSYENDF);

  CLEAR_BIT(ICACHE->CR, ICACHE_CR_EN);

  /* Get tick */
  tickstart = HAL_GetTick();

  /* Wait for instruction cache being disabled */
  while (READ_BIT(ICACHE->CR, ICACHE_CR_EN) != 0U)
  {
    if ((HAL_GetTick() - tickstart) > ICACHE_DISABLE_TIMEOUT_VALUE)
    {
      /* New check to avoid false timeout detection in case of preemption */
      if (READ_BIT(ICACHE->CR, ICACHE_CR_EN) != 0U)
      {
        status = HAL_TIMEOUT;
        break;
      }
    }
  }

  return status;
}

/**
  * @brief  Check whether the Instruction Cache is enabled or not.
  * @retval Status (0: disabled, 1: enabled)
  */
uint32_t HAL_ICACHE_IsEnabled(void)
{
  return ((READ_BIT(ICACHE->CR, ICACHE_CR_EN) != 0U) ? 1UL : 0UL);
}

/**
  * @brief  Invalidate the Instruction Cache.
  * @note   This function waits for the end of cache invalidation procedure
  *         and clears the associated BSYENDF flag.
  * @retval HAL status (HAL_OK/HAL_ERROR/HAL_TIMEOUT)
  */
HAL_StatusTypeDef HAL_ICACHE_Invalidate(void)
{
  HAL_StatusTypeDef status;

  /* Check if no ongoing operation */
  if (READ_BIT(ICACHE->SR, ICACHE_SR_BUSYF) == 0U)
  {
    /* Launch cache invalidation */
    SET_BIT(ICACHE->CR, ICACHE_CR_CACHEINV);
  }

  status = HAL_ICACHE_WaitForInvalidateComplete();

  return status;
}

/**
  * @brief  Invalidate the Instruction Cache with interrupt.
  * @note   This function launches cache invalidation and returns.
  *         User application shall resort to interrupt generation to check
  *         the end of the cache invalidation with the BSYENDF flag and the
  *         HAL_ICACHE_InvalidateCompleteCallback() callback.
  * @retval HAL status (HAL_OK/HAL_ERROR)
  */
HAL_StatusTypeDef HAL_ICACHE_Invalidate_IT(void)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check no ongoing operation */
  if (READ_BIT(ICACHE->SR, ICACHE_SR_BUSYF) != 0U)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Make sure BSYENDF is reset before to start cache invalidation */
    WRITE_REG(ICACHE->FCR, ICACHE_FCR_CBSYENDF);

    /* Enable end of cache invalidation interrupt */
    SET_BIT(ICACHE->IER, ICACHE_IER_BSYENDIE);

    /* Launch cache invalidation */
    SET_BIT(ICACHE->CR, ICACHE_CR_CACHEINV);
  }

  return status;
}

/**
  * @brief Wait for the end of the Instruction Cache invalidate procedure.
  * @note This function checks and clears the BSYENDF flag when set.
  * @retval HAL status (HAL_OK/HAL_TIMEOUT)
  */
HAL_StatusTypeDef HAL_ICACHE_WaitForInvalidateComplete(void)
{
  HAL_StatusTypeDef status = HAL_OK;
  uint32_t tickstart;

  /* Check if ongoing invalidation operation */
  if (READ_BIT(ICACHE->SR, ICACHE_SR_BUSYF) != 0U)
  {
    /* Get tick */
    tickstart = HAL_GetTick();

    /* Wait for end of cache invalidation */
    while (READ_BIT(ICACHE->SR, ICACHE_SR_BSYENDF) == 0U)
    {
      if ((HAL_GetTick() - tickstart) > ICACHE_INVALIDATE_TIMEOUT_VALUE)
      {
        /* New check to avoid false timeout detection in case of preemption */
        if (READ_BIT(ICACHE->SR, ICACHE_SR_BSYENDF) == 0U)
        {
          status = HAL_TIMEOUT;
          break;
        }
      }
    }
  }

  /* Clear BSYENDF */
  WRITE_REG(ICACHE->FCR, ICACHE_FCR_CBSYENDF);

  return status;
}


/**
  * @brief  Start the Instruction Cache performance monitoring.
  * @param  MonitorType  Monitoring type
  *         This parameter can be one of the following values:
  *            @arg ICACHE_MONITOR_HIT_MISS   Hit & Miss monitoring
  *            @arg ICACHE_MONITOR_HIT        Hit monitoring
  *            @arg ICACHE_MONITOR_MISS       Miss monitoring
  * @retval HAL status (HAL_OK)
  */
HAL_StatusTypeDef HAL_ICACHE_Monitor_Start(uint32_t MonitorType)
{
  /* Check the parameters */
  assert_param(IS_ICACHE_MONITOR_TYPE(MonitorType));

  SET_BIT(ICACHE->CR, MonitorType);

  return HAL_OK;
}

/**
  * @brief  Stop the Instruction Cache performance monitoring.
  * @note   Stopping the monitoring does not reset the values.
  * @param  MonitorType  Monitoring type
  *         This parameter can be one of the following values:
  *            @arg ICACHE_MONITOR_HIT_MISS   Hit & Miss monitoring
  *            @arg ICACHE_MONITOR_HIT        Hit monitoring
  *            @arg ICACHE_MONITOR_MISS       Miss monitoring
  * @retval HAL status (HAL_OK)
  */
HAL_StatusTypeDef HAL_ICACHE_Monitor_Stop(uint32_t MonitorType)
{
  /* Check the parameters */
  assert_param(IS_ICACHE_MONITOR_TYPE(MonitorType));

  CLEAR_BIT(ICACHE->CR, MonitorType);

  return HAL_OK;
}

/**
  * @brief  Reset the Instruction Cache performance monitoring values.
  * @param  MonitorType  Monitoring type
  *         This parameter can be one of the following values:
  *            @arg ICACHE_MONITOR_HIT_MISS   Hit & Miss monitoring
  *            @arg ICACHE_MONITOR_HIT        Hit monitoring
  *            @arg ICACHE_MONITOR_MISS       Miss monitoring
  * @retval HAL status (HAL_OK)
  */
HAL_StatusTypeDef HAL_ICACHE_Monitor_Reset(uint32_t MonitorType)
{
  /* Check the parameters */
  assert_param(IS_ICACHE_MONITOR_TYPE(MonitorType));

  /* Force/Release reset */
  SET_BIT(ICACHE->CR, (MonitorType << 2U));
  CLEAR_BIT(ICACHE->CR, (MonitorType << 2U));

  return HAL_OK;
}

/**
  * @brief  Get the Instruction Cache performance Hit monitoring value.
  * @note   Upon reaching the 32-bit maximum value, monitor does not wrap.
  * @retval Hit monitoring value
  */
uint32_t HAL_ICACHE_Monitor_GetHitValue(void)
{
  return (ICACHE->HMONR);
}

/**
  * @brief  Get the Instruction Cache performance Miss monitoring value.
  * @note   Upon reaching the 32-bit maximum value, monitor does not wrap.
  * @retval Miss monitoring value
  */
uint32_t HAL_ICACHE_Monitor_GetMissValue(void)
{
  return (ICACHE->MMONR);
}

/**
  * @}
  */

/** @defgroup ICACHE_Exported_Functions_Group2 IRQ and callback functions
  * @brief    IRQ and callback functions
  *
  @verbatim
  ==============================================================================
            ##### IRQ and callback functions #####
  ==============================================================================
  [..]
    This section provides functions allowing to handle ICACHE global interrupt
    and the associated callback functions.
  @endverbatim
  * @{
  */

/**
  * @brief Handle the Instruction Cache interrupt request.
  * @note This function should be called under the ICACHE_IRQHandler().
  * @note This function respectively disables the interrupt and clears the
  *       flag of any pending flag before calling the associated user callback.
  * @retval None
  */
void HAL_ICACHE_IRQHandler(void)
{
  /* Get current interrupt flags and interrupt sources value */
  uint32_t itflags   = READ_REG(ICACHE->SR);
  uint32_t itsources = READ_REG(ICACHE->IER);

  /* Check Instruction cache Error interrupt flag */
  if (((itflags & itsources) & ICACHE_FLAG_ERROR) != 0U)
  {
    /* Disable error interrupt */
    CLEAR_BIT(ICACHE->IER, ICACHE_IER_ERRIE);

    /* Clear ERR pending flag */
    WRITE_REG(ICACHE->FCR, ICACHE_FCR_CERRF);

    /* Instruction cache error interrupt user callback */
    HAL_ICACHE_ErrorCallback();
  }

  /* Check Instruction cache BusyEnd interrupt flag */
  if (((itflags & itsources) & ICACHE_FLAG_BUSYEND) != 0U)
  {
    /* Disable end of cache invalidation interrupt */
    CLEAR_BIT(ICACHE->IER, ICACHE_IER_BSYENDIE);

    /* Clear BSYENDF pending flag */
    WRITE_REG(ICACHE->FCR, ICACHE_FCR_CBSYENDF);

    /* Instruction cache busyend interrupt user callback */
    HAL_ICACHE_InvalidateCompleteCallback();
  }
}

/**
  * @brief  Cache invalidation complete callback.
  */
__weak void HAL_ICACHE_InvalidateCompleteCallback(void)
{
  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_ICACHE_InvalidateCompleteCallback() should be implemented in the user file
   */
}

/**
  * @brief  Error callback.
  */
__weak void HAL_ICACHE_ErrorCallback(void)
{
  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_ICACHE_ErrorCallback() should be implemented in the user file
   */
}

/**
  * @}
  */


/**
  * @}
  */

#endif /* ICACHE && HAL_ICACHE_MODULE_ENABLED */

/**
  * @}
  */

/**
  * @}
  */
