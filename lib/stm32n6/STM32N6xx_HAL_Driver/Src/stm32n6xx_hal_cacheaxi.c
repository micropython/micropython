/**
  ******************************************************************************
  * @file    stm32n6xx_hal_cacheaxi.c
  * @author  MCD Application Team
  * @brief   CACHEAXI HAL module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities of the CACHEAXI.
  *           + Initialization and Configuration
  *           + Cache coherency command
  *           + Monitoring management
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
 ===============================================================================
                        ##### How to use this driver #####
 ===============================================================================
  [..]
   (#) Configure and enable the MPU to override default config if needed, please refers to
       ARM manual for default memory attribute. Then enable CACHEAXI with HAL_CACHEAXI_Init()
       to cache memories data when accessed by a master peripheral through an AXI interconnect.

    [..]
        (+) Use HAL_CACHEAXI_Invalidate() to invalidate the full cache content:
            (++) Cache content is lost, and reloaded when needed.
            (++) Blocking call until operation is done.
        (+) Use HAL_CACHEAXI_CleanByAddr() to clean cache content for a specific memory range:
            (++) Cache content for specific range is written back to specific memory range.
            (++) Used when buffer is updated by CPU before usage by a peripheral (typically DMA transfer)
            (++) Blocking call until operation is done.
        (+) Use HAL_CACHEAXI_CleanInvalidateByAddr() to clean and invalidate cache content for a specific memory range:
            (++) Cache content for specific range is written back to specific memory range, and reloaded when needed.
            (++) Recommended to use for MPU reprogramming.
            (++) Blocking call until operation is done.

     *** Interrupt mode IO operation ***
     ===================================
    [..]
        (+) Configure the CACHEAXI interrupt priority using HAL_NVIC_SetPriority()
        (+) Enable the CACHEAXI IRQ handler using HAL_NVIC_EnableIRQ()
        (+) Override weak definition for following callbacks (if needed):
            (++)HAL_CACHEAXI_CleanAndInvalidateByAddrCallback()
            (++)HAL_CACHEAXI_InvalidateCompleteCallback()
            (++)HAL_CACHEAXI_CleanByAddrCallback()
            (++)HAL_CACHEAXI_ErrorCallback()
        (+) Use HAL_CACHEAXI_<COMMAND>_IT() to start a CACHEAXI operation with IT enabled.
        (+) Use HAL_CACHEAXI_IRQHandler() called under CACHEAXIx_IRQHandler() Interrupt subroutine

    [..]  Use HAL_CACHEAXI_GetState() function to return the CACHEAXI state and HAL_CACHEAXI_GetError()
          in case of error detection.

     *** CACHEAXI HAL driver macros list ***
     =============================================
     [..]
       Below the list of macros defined in the CACHEAXI HAL driver.

      (+) __HAL_CACHEAXI_ENABLE_IT    : Enable CACHEAXI interrupts.
      (+) __HAL_CACHEAXI_DISABLE_IT   : Disable CACHEAXI interrupts.
      (+) __HAL_CACHEAXI_GET_IT_SOURCE: Check whether the specified CACHEAXI interrupt source is enabled or not.
      (+) __HAL_CACHEAXI_GET_FLAG     : Check whether the selected CACHEAXI flag is set or not.
      (+) __HAL_CACHEAXI_CLEAR_FLAG   : Clear the selected CACHEAXI flags.

     [..]
      (@) You can refer to the header file of the CACHEAXI HAL driver for more useful macros.

    [..]

  @endverbatim
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal.h"

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */

#if defined (CACHEAXI)
/** @defgroup CACHEAXI CACHEAXI
  * @brief HAL CACHEAXI module driver
  * @{
  */

#ifdef HAL_CACHEAXI_MODULE_ENABLED

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/** @defgroup CACHEAXI_Private_Macros CACHEAXI Private Macros
  * @{
  */
#define IS_CACHEAXI_REGION_SIZE(__SIZE__)                ((__SIZE__) > 0U)

#define IS_CACHEAXI_MONITOR_TYPE(__TYPE__)               (((__TYPE__) & ~CACHEAXI_MONITOR_ALL) == 0U)

#define IS_CACHEAXI_SINGLE_MONITOR_TYPE(__TYPE__)      (((__TYPE__) == CACHEAXI_MONITOR_READ_HIT)       || \
                                                        ((__TYPE__) == CACHEAXI_MONITOR_READ_MISS)      || \
                                                        ((__TYPE__) == CACHEAXI_MONITOR_WRITE_HIT)      || \
                                                        ((__TYPE__) == CACHEAXI_MONITOR_WRITE_MISS)     || \
                                                        ((__TYPE__) == CACHEAXI_MONITOR_READALLOC_MISS) || \
                                                        ((__TYPE__) == CACHEAXI_MONITOR_WRITEALLOC_MISS)|| \
                                                        ((__TYPE__) == CACHEAXI_MONITOR_WRITETHROUGH)   || \
                                                        ((__TYPE__) == CACHEAXI_MONITOR_EVICTION))

/**
  * @}
  */

/* Private typedef -----------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/** @addtogroup CACHEAXI_Private_Constants CACHEAXI Private Constants
  * @{
  */
#define CACHEAXI_COMMAND_TIMEOUT_VALUE           200U    /* 200ms*/
#define CACHEAXI_ENABLE_TIMEOUT_VALUE            1U      /* 1ms  */
#define CACHEAXI_DISABLE_TIMEOUT_VALUE           1U      /* 1ms  */

#define CACHEAXI_COMMAND_CLEAN                   CACHEAXI_CR2_CACHECMD_0
#define CACHEAXI_COMMAND_CLEAN_INVALIDATE        (CACHEAXI_CR2_CACHECMD_0|CACHEAXI_CR2_CACHECMD_1)

#define CACHEAXI_POLLING_MODE                    0U
#define CACHEAXI_IT_MODE                         1U

/**
  * @}
  */

/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static HAL_StatusTypeDef CACHEAXI_CommandByAddr(CACHEAXI_HandleTypeDef *hcacheaxi, uint32_t Command,
                                                const uint32_t *pAddr, uint32_t dSize, uint32_t mode);

/* Exported functions --------------------------------------------------------*/
/** @addtogroup CACHEAXI_Exported_Functions CACHEAXI Exported Functions
  * @{
  */

/** @addtogroup CACHEAXI_Exported_Functions_Group1
  *
@verbatim
 ===============================================================================
              ##### Initialization and de-initialization functions #####
 ===============================================================================
    [..]  This subsection provides a set of functions allowing to initialize and
          deinitialize the CACHEAXIx peripheral:

      (+) User must implement HAL_CACHEAXI_MspInit() function in which he configures
          all related peripherals resources (CLOCK, MPU, IT and NVIC ).

      (+) Call the function HAL_CACHEAXI_Init() to initialize the CACHEAXIx handle and
          enable the CACHEAXI

      (+) Call the function HAL_CACHEAXI_DeInit() to restore the reset configuration
          of the selected CACHEAXIx peripheral.

@endverbatim
  * @{
  */

/**
  * @brief  Initializes the CACHEAXI handle and enables the CACHEAXI.
  * @param  hcacheaxi Pointer to a CACHEAXI_HandleTypeDef structure that contains
  *                   the configuration information for the specified CACHEAXI.
  * @retval HAL status
  * @note   In case HAL_CACHEAXI_Init() returns HAL_BUSY because an invalidation
  *         procedure is ongoing, the application should call again HAL_CACHEAXI_Init()
  *         until it returns HAL_OK to have the CACHEAXI enabled
  */
HAL_StatusTypeDef  HAL_CACHEAXI_Init(CACHEAXI_HandleTypeDef *hcacheaxi)
{
  HAL_StatusTypeDef status;

  /* Check the CACHEAXI handle allocation */
  if (hcacheaxi == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_CACHEAXI_ALL_INSTANCE(hcacheaxi->Instance));

  if (hcacheaxi->State == HAL_CACHEAXI_STATE_RESET)
  {
#if (USE_HAL_CACHEAXI_REGISTER_CALLBACKS == 1)
    /* Init the CACHEAXI Callback settings with legacy weak */
    hcacheaxi->ErrorCallback                      = HAL_CACHEAXI_ErrorCallback;
    hcacheaxi->CleanByAddrCallback                = HAL_CACHEAXI_CleanByAddrCallback;
    hcacheaxi->InvalidateCompleteCallback         = HAL_CACHEAXI_InvalidateCompleteCallback;
    hcacheaxi->CleanAndInvalidateByAddrCallback   = HAL_CACHEAXI_CleanAndInvalidateByAddrCallback;

    if (hcacheaxi->MspInitCallback == NULL)
    {
      hcacheaxi->MspInitCallback = HAL_CACHEAXI_MspInit;
    }

    /* Init the low level hardware */
    hcacheaxi->MspInitCallback(hcacheaxi);
#else
    /* Init the low level hardware */
    HAL_CACHEAXI_MspInit(hcacheaxi);
#endif /* USE_HAL_CACHEAXI_REGISTER_CALLBACKS */
  }

  /* Init the error code */
  hcacheaxi->ErrorCode = HAL_CACHEAXI_ERROR_NONE;

  /* Init the CACHEAXI handle state */
  hcacheaxi->State = HAL_CACHEAXI_STATE_READY;

  /* Enable the selected CACHEAXI peripheral */
  status = HAL_CACHEAXI_Enable(hcacheaxi);

  return status;
}

/**
  * @brief  DeInitialize the Data cache.
  * @param  hcacheaxi Pointer to a CACHEAXI_HandleTypeDef structure that contains
  *                   the configuration information for the specified CACHEAXIx peripheral.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CACHEAXI_DeInit(CACHEAXI_HandleTypeDef *hcacheaxi)
{
  HAL_StatusTypeDef status;

  /* Check the cacheaxi handle allocation */
  if (hcacheaxi == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_CACHEAXI_ALL_INSTANCE(hcacheaxi->Instance));

  /* Update the error code */
  hcacheaxi->ErrorCode = HAL_CACHEAXI_ERROR_NONE;

  /* Return to the reset state */
  hcacheaxi->State = HAL_CACHEAXI_STATE_RESET;

  /* Disable cache */
  status = HAL_CACHEAXI_Disable(hcacheaxi);

  /* reset monitor values */
  (void)HAL_CACHEAXI_Monitor_Reset(hcacheaxi, CACHEAXI_MONITOR_ALL);

  /* Reset all remaining bit */
  WRITE_REG(hcacheaxi->Instance->CR1, 0U);
  WRITE_REG(hcacheaxi->Instance->CR2, 0U);
  WRITE_REG(hcacheaxi->Instance->CMDRSADDRR, 0U);
  WRITE_REG(hcacheaxi->Instance->CMDREADDRR, 0U);
  WRITE_REG(hcacheaxi->Instance->FCR, 0U);
  WRITE_REG(hcacheaxi->Instance->IER, 0U);

#if (USE_HAL_CACHEAXI_REGISTER_CALLBACKS == 1)
  if (hcacheaxi->MspDeInitCallback == NULL)
  {
    hcacheaxi->MspDeInitCallback = HAL_CACHEAXI_MspDeInit;
  }

  /* DeInitialize the low level hardware */
  hcacheaxi->MspDeInitCallback(hcacheaxi);
#else
  /* DeInitialize the low level hardware */
  HAL_CACHEAXI_MspDeInit(hcacheaxi);
#endif /* USE_HAL_CACHEAXI_REGISTER_CALLBACKS */

  return status;
}

/**
  * @brief Initialize the CACHEAXI MSP.
  * @param  hcacheaxi Pointer to a CACHEAXI_HandleTypeDef structure that contains
  *                   the configuration information for the specified CACHEAXIx peripheral.
  * @retval None
  */
__weak void HAL_CACHEAXI_MspInit(CACHEAXI_HandleTypeDef *hcacheaxi)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hcacheaxi);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_CACHEAXI_MspInit can be implemented in the user file
   */
}

/**
  * @brief DeInitialize the CACHEAXI MSP.
  * @param  hcacheaxi Pointer to a CACHEAXI_HandleTypeDef structure that contains
  *                 the configuration information for the specified CACHEAXIx peripheral.
  * @retval None
  */
__weak void HAL_CACHEAXI_MspDeInit(CACHEAXI_HandleTypeDef *hcacheaxi)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hcacheaxi);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_CACHEAXI_MspDeInit can be implemented in the user file
   */
}
/**
  * @}
  */

/** @addtogroup CACHEAXI_Exported_Functions_Group2
  *
@verbatim
  ==============================================================================
             ##### IO operation functions #####
  ==============================================================================
    [..]  This section provides functions allowing to:
      (+) Enable CACHEAXI.
      (+) Disable CACHEAXI.
      (+) Invalidate the CACHEAXI.
      (+) Invalidate the CACHEAXI with interrupt.
      (+) Clean the CACHEAXI by Addr.
      (+) Clean and Invalidate the CACHEAXI by Addr.
      (+) Clean the CACHEAXI by Addr with interrupt.
      (+) Clean and Invalidate the CACHEAXI by Addr with interrupt.
      (+) Start the CACHEAXI performance monitoring.
      (+) Stop the CACHEAXI performance monitoring.
      (+) Reset the CACHEAXI performance monitoring values.
      (+) Get the CACHEAXI performance Read Hit monitoring value.
      (+) Get the CACHEAXI performance Read Miss monitoring value.
      (+) Get the CACHEAXI performance Write Hit monitoring value.
      (+) Get the CACHEAXI performance Write Miss monitoring value.
      (+) Get the CACHEAXI performance Read Allocation Miss monitoring value.
      (+) Get the CACHEAXI performance Write Allocation Miss monitoring value.
      (+) Get the CACHEAXI performance Write Through monitoring value.
      (+) Get the CACHEAXI performance Eviction monitoring value.
@endverbatim
  * @{
  */

/**
  * @brief  Enable the Data cache.
  * @param  hcacheaxi Pointer to a CACHEAXI_HandleTypeDef structure that contains
  *                   the configuration information for the specified CACHEAXIx peripheral.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CACHEAXI_Enable(CACHEAXI_HandleTypeDef *hcacheaxi)
{
  HAL_StatusTypeDef status = HAL_OK;
  uint32_t tickstart;

  /* Check the cacheaxi handle allocation */
  if (hcacheaxi == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_CACHEAXI_ALL_INSTANCE(hcacheaxi->Instance));

  /* Check if ongoing full invalidation operation */
  if (READ_BIT(hcacheaxi->Instance->SR, CACHEAXI_SR_BUSYF) != 0U)
  {
    /* Get tick */
    tickstart = HAL_GetTick();

    while (READ_BIT(hcacheaxi->Instance->SR, CACHEAXI_SR_BUSYF) != 0U)
    {
      if ((HAL_GetTick() - tickstart) > CACHEAXI_ENABLE_TIMEOUT_VALUE)
      {
        /* New check to avoid false timeout detection in case of preemption */
        if (READ_BIT(hcacheaxi->Instance->SR, CACHEAXI_SR_BUSYF) == 0U)
        {
          /* Update error code */
          hcacheaxi->ErrorCode = HAL_CACHEAXI_ERROR_TIMEOUT;
          /* Return error status */
          status =  HAL_ERROR;
          break;
        }
      }
    }
  }

  if (status == HAL_OK)
  {
    /* Update the error code */
    hcacheaxi->ErrorCode = HAL_CACHEAXI_ERROR_NONE;
    /* Enable the selected CACHEAXI peripheral */
    SET_BIT(hcacheaxi->Instance->CR1, CACHEAXI_CR1_EN);
  }

  return status;
}

/**
  * @brief  Disable the Data cache.
  * @param  hcacheaxi Pointer to a CACHEAXI_HandleTypeDef structure that contains
  *                   the configuration information for the specified CACHEAXIx peripheral.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CACHEAXI_Disable(CACHEAXI_HandleTypeDef *hcacheaxi)
{
  HAL_StatusTypeDef status = HAL_OK;

  uint32_t tickstart;

  /* Check the cacheaxi handle allocation */
  if (hcacheaxi == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_CACHEAXI_ALL_INSTANCE(hcacheaxi->Instance));

  /* Check CACHEAXI handle status */
  if (HAL_CACHEAXI_IsEnabled(hcacheaxi) != 0U)
  {
    /* Update the error code */
    hcacheaxi->ErrorCode = HAL_CACHEAXI_ERROR_NONE;

    /* Change CACHEAXI handle state */
    hcacheaxi->State = HAL_CACHEAXI_STATE_READY;

    /* Disable the selected CACHEAXI peripheral */
    CLEAR_BIT(hcacheaxi->Instance->CR1, CACHEAXI_CR1_EN);

    /* Get timeout */
    tickstart = HAL_GetTick();

    /* Wait for end of data cache disabling */
    while (READ_BIT(hcacheaxi->Instance->SR, (CACHEAXI_SR_BUSYF | CACHEAXI_SR_BUSYCMDF)) != 0U)
    {
      if ((HAL_GetTick() - tickstart) > CACHEAXI_DISABLE_TIMEOUT_VALUE)
      {
        if (READ_BIT(hcacheaxi->Instance->SR, (CACHEAXI_SR_BUSYF | CACHEAXI_SR_BUSYCMDF)) != 0U)
        {
          /* Update error code */
          hcacheaxi->ErrorCode = HAL_CACHEAXI_ERROR_TIMEOUT;

          /* Change the CACHEAXI handle state */
          hcacheaxi->State = HAL_CACHEAXI_STATE_READY;

          /* Return error status */
          status =  HAL_ERROR;
          break;
        }
      }
    }
  }

  return status;
}
/**
  * @brief  Check whether the CACHEAXI is enabled or not.
  * @param  hcacheaxi Pointer to a CACHEAXI_HandleTypeDef structure that contains
  *                   the configuration information for the specified CACHEAXIx peripheral.
  * @retval Status (0: disabled, 1: enabled)
  */
uint32_t HAL_CACHEAXI_IsEnabled(const CACHEAXI_HandleTypeDef *hcacheaxi)
{
  return ((READ_BIT(hcacheaxi->Instance->CR1, CACHEAXI_CR1_EN) != 0U) ? 1UL : 0UL);
}

/**
  * @brief  Invalidate the Data cache.
  * @param  hcacheaxi Pointer to a CACHEAXI_HandleTypeDef structure that contains
  *                 the configuration information for the specified CACHEAXIx peripheral.
  * @note   This function waits for end of full cache invalidation
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CACHEAXI_Invalidate(CACHEAXI_HandleTypeDef *hcacheaxi)
{
  HAL_StatusTypeDef status = HAL_OK;
  uint32_t tickstart;

  /* Check the cacheaxi handle allocation */
  if (hcacheaxi == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_CACHEAXI_ALL_INSTANCE(hcacheaxi->Instance));

  /* Check no ongoing operation */
  if (READ_BIT(hcacheaxi->Instance->SR, (CACHEAXI_SR_BUSYF | CACHEAXI_SR_BUSYCMDF)) != 0U)
  {
    /* Return busy status */
    status =  HAL_BUSY;
  }
  else
  {
    /* Update the error code */
    hcacheaxi->ErrorCode = HAL_CACHEAXI_ERROR_NONE;

    /* Change CACHEAXI Handle state */
    hcacheaxi->State = HAL_CACHEAXI_STATE_READY;

    /* Make sure flags are reset */
    WRITE_REG(hcacheaxi->Instance->FCR, (CACHEAXI_FCR_CBSYENDF | CACHEAXI_FCR_CCMDENDF));

    /* Set no operation on address range */
    MODIFY_REG(hcacheaxi->Instance->CR2, CACHEAXI_CR2_CACHECMD, 0U);

    /* Launch cache invalidation */
    SET_BIT(hcacheaxi->Instance->CR1, CACHEAXI_CR1_CACHEINV);

    /* Get timeout */
    tickstart = HAL_GetTick();

    /* Wait for end of cache invalidation */
    while (READ_BIT(hcacheaxi->Instance->SR, CACHEAXI_SR_BUSYF) != 0U)
    {
      if ((HAL_GetTick() - tickstart) > CACHEAXI_COMMAND_TIMEOUT_VALUE)
      {
        if (READ_BIT(hcacheaxi->Instance->SR, CACHEAXI_SR_BUSYF) != 0U)
        {
          /* Update error code */
          hcacheaxi->ErrorCode = HAL_CACHEAXI_ERROR_TIMEOUT;

          /* Change the CACHEAXI state */
          hcacheaxi->State = HAL_CACHEAXI_STATE_ERROR;

          /* Return error status */
          status =  HAL_ERROR;
          break;
        }
      }
    }
  }

  return status;
}

/**
  * @brief  Clean the Data cache by Addr.
  * @param  hcacheaxi Pointer to a CACHEAXI_HandleTypeDef structure that contains
  *                   the configuration information for the specified CACHEAXIx peripheral.
  * @param  pAddr Start address of the region to be Cleaned
  * @param  dSize Size of the region to be Cleaned (in bytes)
  * @note   This function waits for end of cache Clean
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CACHEAXI_CleanByAddr(CACHEAXI_HandleTypeDef *hcacheaxi, const uint32_t *const pAddr,
                                           uint32_t dSize)
{
  HAL_StatusTypeDef status;

  /* Check the cacheaxi handle allocation */
  if (hcacheaxi == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_CACHEAXI_ALL_INSTANCE(hcacheaxi->Instance));
  assert_param(IS_CACHEAXI_REGION_SIZE(dSize));

  status = CACHEAXI_CommandByAddr(hcacheaxi, CACHEAXI_COMMAND_CLEAN, pAddr, dSize, CACHEAXI_POLLING_MODE);

  return status;
}

/**
  * @brief  Clean and Invalidate the Data cache by Addr.
  * @param  hcacheaxi Pointer to a CACHEAXI_HandleTypeDef structure that contains
  *                   the configuration information for the specified CACHEAXIx peripheral.
  * @param  pAddr Start address of the region to be Cleaned and Invalidated
  * @param  dSize Size of the region to be Cleaned and Invalidated (in bytes)
  * @note   This function waits for end of cache Clean and Invalidation
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CACHEAXI_CleanInvalidByAddr(CACHEAXI_HandleTypeDef *hcacheaxi, const uint32_t *const pAddr,
                                                  uint32_t dSize)
{
  HAL_StatusTypeDef status;

  /* Check the cacheaxi handle allocation */
  if (hcacheaxi == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_CACHEAXI_ALL_INSTANCE(hcacheaxi->Instance));
  assert_param(IS_CACHEAXI_REGION_SIZE(dSize));

  status = CACHEAXI_CommandByAddr(hcacheaxi, CACHEAXI_COMMAND_CLEAN_INVALIDATE, pAddr, dSize, CACHEAXI_POLLING_MODE);

  return status;
}

/**
  * @brief  Invalidate the Data cache with interrupt.
  * @param  hcacheaxi Pointer to a CACHEAXI_HandleTypeDef structure that contains
  *                   the configuration information for the specified CACHEAXIx peripheral.
  * @note   This function launches maintenance operation and returns immediately.
  *         User application shall resort to interrupt generation to check
  *         the end of operation.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CACHEAXI_Invalidate_IT(CACHEAXI_HandleTypeDef *hcacheaxi)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check the cacheaxi handle allocation */
  if (hcacheaxi == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_CACHEAXI_ALL_INSTANCE(hcacheaxi->Instance));

  /* Check no ongoing operation */
  if (READ_BIT(hcacheaxi->Instance->SR, (CACHEAXI_SR_BUSYF | CACHEAXI_SR_BUSYCMDF)) != 0U)
  {
    /* Return busy status */
    status =  HAL_BUSY;
  }
  else
  {
    /* Update the error code */
    hcacheaxi->ErrorCode = HAL_CACHEAXI_ERROR_NONE;

    /* Change CACHEAXI Handle state */
    hcacheaxi->State = HAL_CACHEAXI_STATE_READY;

    /* Make sure BSYENDF is reset */
    WRITE_REG(hcacheaxi->Instance->FCR, (CACHEAXI_FCR_CBSYENDF | CACHEAXI_FCR_CCMDENDF));

    /* Set no operation on address range for callback under interrupt */
    MODIFY_REG(hcacheaxi->Instance->CR2, CACHEAXI_CR2_CACHECMD, 0U);

    /* Enable end of cache invalidation interrupt */
    SET_BIT(hcacheaxi->Instance->IER, CACHEAXI_IER_BSYENDIE);

    /* Launch cache invalidation */
    SET_BIT(hcacheaxi->Instance->CR1, CACHEAXI_CR1_CACHEINV);
  }

  return status;
}

/**
  * @brief  Clean the Data cache by Addr with interrupt.
  * @param  hcacheaxi Pointer to a CACHEAXI_HandleTypeDef structure that contains
  *                   the configuration information for the specified CACHEAXIx peripheral.
  * @param  pAddr Start address of the region to be Cleaned
  * @param  dSize Size of the region to be Cleaned
  * @note   This function launches maintenance operation and returns immediately.
  *         User application shall resort to interrupt generation to check
  *         the end of operation.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CACHEAXI_CleanByAddr_IT(CACHEAXI_HandleTypeDef *hcacheaxi, const uint32_t *const pAddr,
                                              uint32_t dSize)
{
  HAL_StatusTypeDef status;

  /* Check the cacheaxi handle allocation */
  if (hcacheaxi == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_CACHEAXI_ALL_INSTANCE(hcacheaxi->Instance));
  assert_param(IS_CACHEAXI_REGION_SIZE(dSize));

  status = CACHEAXI_CommandByAddr(hcacheaxi, CACHEAXI_COMMAND_CLEAN, pAddr, dSize, CACHEAXI_IT_MODE);

  return status;
}

/**
  * @brief  Clean and Invalidate the Data cache by Addr with interrupt.
  * @param  hcacheaxi Pointer to a CACHEAXI_HandleTypeDef structure that contains
  *                   the configuration information for the specified CACHEAXIx peripheral.
  * @param  pAddr Start address of the region to be Cleaned and Invalidated
  * @param  dSize Size of the region to be Cleaned and Invalidated
  * @note   This function launches maintenance operation and returns immediately.
  *         User application shall resort to interrupt generation to check
  *         the end of operation.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CACHEAXI_CleanInvalidByAddr_IT(CACHEAXI_HandleTypeDef *hcacheaxi, const uint32_t *const pAddr,
                                                     uint32_t dSize)
{
  HAL_StatusTypeDef status;

  /* Check the cacheaxi handle allocation */
  if (hcacheaxi == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_CACHEAXI_ALL_INSTANCE(hcacheaxi->Instance));
  assert_param(IS_CACHEAXI_REGION_SIZE(dSize));

  status = CACHEAXI_CommandByAddr(hcacheaxi, CACHEAXI_COMMAND_CLEAN_INVALIDATE, pAddr, dSize, CACHEAXI_IT_MODE);

  return status;
}

/**
  * @brief  Start the CACHEAXI performance monitoring.
  * @param  hcacheaxi Pointer to a CACHEAXI_HandleTypeDef structure that contains
  *                   the configuration information for the specified CACHEAXIx peripheral.
  * @param  MonitorType Monitoring type
  *         This parameter can be a combination of the following values:
  *            @arg CACHEAXI_MONITOR_READ_HIT
  *            @arg CACHEAXI_MONITOR_READ_MISS
  *            @arg CACHEAXI_MONITOR_WRITE_HIT
  *            @arg CACHEAXI_MONITOR_WRITE_MISS
  *            @arg CACHEAXI_MONITOR_READALLOC_MISS
  *            @arg CACHEAXI_MONITOR_WRITEALLOC_MISS
  *            @arg CACHEAXI_MONITOR_WRITETHROUGH
  *            @arg CACHEAXI_MONITOR_EVICTION
  *            @arg CACHEAXI_MONITOR_ALL
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CACHEAXI_Monitor_Start(CACHEAXI_HandleTypeDef *hcacheaxi, uint32_t MonitorType)
{
  /* Check the cacheaxi handle allocation */
  if (hcacheaxi == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_CACHEAXI_ALL_INSTANCE(hcacheaxi->Instance));
  assert_param(IS_CACHEAXI_MONITOR_TYPE(MonitorType));

  SET_BIT(hcacheaxi->Instance->CR1, MonitorType);

  return HAL_OK;
}

/**
  * @brief  Stop the CACHEAXI performance monitoring.
  * @param  hcacheaxi Pointer to a CACHEAXI_HandleTypeDef structure that contains
  *                   the configuration information for the specified CACHEAXIx peripheral.
  * @note   Stopping the monitoring does not reset the values.
  * @param  MonitorType Monitoring type
  *         This parameter can be a combination of the following values:
  *            @arg CACHEAXI_MONITOR_READ_HIT
  *            @arg CACHEAXI_MONITOR_READ_MISS
  *            @arg CACHEAXI_MONITOR_WRITE_HIT
  *            @arg CACHEAXI_MONITOR_WRITE_MISS
  *            @arg CACHEAXI_MONITOR_READALLOC_MISS
  *            @arg CACHEAXI_MONITOR_WRITEALLOC_MISS
  *            @arg CACHEAXI_MONITOR_WRITETHROUGH
  *            @arg CACHEAXI_MONITOR_EVICTION
  *            @arg CACHEAXI_MONITOR_ALL
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CACHEAXI_Monitor_Stop(CACHEAXI_HandleTypeDef *hcacheaxi, uint32_t MonitorType)
{
  /* Check the cacheaxi handle allocation */
  if (hcacheaxi == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_CACHEAXI_ALL_INSTANCE(hcacheaxi->Instance));
  assert_param(IS_CACHEAXI_MONITOR_TYPE(MonitorType));

  CLEAR_BIT(hcacheaxi->Instance->CR1, MonitorType);

  return HAL_OK;
}

/**
  * @brief  Reset the CACHEAXI performance monitoring values.
  * @param  hcacheaxi Pointer to a CACHEAXI_HandleTypeDef structure that contains
  *                   the configuration information for the specified CACHEAXIx peripheral.
  * @param  MonitorType Monitoring type
  *         This parameter can be a combination of the following values:
  *            @arg CACHEAXI_MONITOR_READ_HIT
  *            @arg CACHEAXI_MONITOR_READ_MISS
  *            @arg CACHEAXI_MONITOR_WRITE_HIT
  *            @arg CACHEAXI_MONITOR_WRITE_MISS
  *            @arg CACHEAXI_MONITOR_READALLOC_MISS
  *            @arg CACHEAXI_MONITOR_WRITEALLOC_MISS
  *            @arg CACHEAXI_MONITOR_WRITETHROUGH
  *            @arg CACHEAXI_MONITOR_EVICTION
  *            @arg CACHEAXI_MONITOR_ALL
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CACHEAXI_Monitor_Reset(CACHEAXI_HandleTypeDef *hcacheaxi, uint32_t MonitorType)
{
  /* Check the cacheaxi handle allocation */
  if (hcacheaxi == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_CACHEAXI_ALL_INSTANCE(hcacheaxi->Instance));
  assert_param(IS_CACHEAXI_MONITOR_TYPE(MonitorType));

  /* Force/Release reset */
  SET_BIT(hcacheaxi->Instance->CR1, (MonitorType << 2U));
  CLEAR_BIT(hcacheaxi->Instance->CR1, (MonitorType << 2U));

  return HAL_OK;
}

/**
  * @brief  Get the CACHEAXI performance Read Hit monitoring value.
  * @param  hcacheaxi Pointer to a CACHEAXI_HandleTypeDef structure that contains
  *                   the configuration information for the specified CACHEAXIx peripheral.
  * @note   Upon reaching the 32-bit maximum value, monitor does not wrap.
  * @retval Read Hit monitoring value
  */
uint32_t HAL_CACHEAXI_Monitor_GetReadHitValue(const CACHEAXI_HandleTypeDef *hcacheaxi)
{
  /* Check the parameters */
  assert_param(IS_CACHEAXI_ALL_INSTANCE(hcacheaxi->Instance));

  /*return the Read Hit monitor value*/
  return hcacheaxi->Instance->RHMONR;
}

/**
  * @brief  Get the CACHEAXI performance Read Miss monitoring value.
  * @param  hcacheaxi Pointer to a CACHEAXI_HandleTypeDef structure that contains
  *                   the configuration information for the specified CACHEAXIx peripheral.
  * @note   Upon reaching the 16-bit maximum value, monitor does not wrap.
  * @retval Read Miss monitoring value
  */
uint32_t HAL_CACHEAXI_Monitor_GetReadMissValue(const CACHEAXI_HandleTypeDef *hcacheaxi)
{
  /* Check the parameters */
  assert_param(IS_CACHEAXI_ALL_INSTANCE(hcacheaxi->Instance));

  /*return the Read Miss monitor value*/
  return hcacheaxi->Instance->RMMONR;
}

/**
  * @brief  Get the CACHEAXI performance Write Hit monitoring value.
  * @param  hcacheaxi Pointer to a CACHEAXI_HandleTypeDef structure that contains
  *                   the configuration information for the specified CACHEAXIx peripheral.
  * @note   Upon reaching the 32-bit maximum value, monitor does not wrap.
  * @retval Write Hit monitoring value
  */
uint32_t HAL_CACHEAXI_Monitor_GetWriteHitValue(const CACHEAXI_HandleTypeDef *hcacheaxi)
{
  /* Check the parameters */
  assert_param(IS_CACHEAXI_ALL_INSTANCE(hcacheaxi->Instance));

  /*return the Write Hit monitor value*/
  return hcacheaxi->Instance->WHMONR;
}

/**
  * @brief  Get the CACHEAXI performance Write Miss monitoring value.
  * @param  hcacheaxi Pointer to a CACHEAXI_HandleTypeDef structure that contains
  *                   the configuration information for the specified CACHEAXIx peripheral.
  * @note   Upon reaching the 16-bit maximum value, monitor does not wrap.
  * @retval Write Miss monitoring value
  */
uint32_t HAL_CACHEAXI_Monitor_GetWriteMissValue(const CACHEAXI_HandleTypeDef *hcacheaxi)
{
  /* Check the parameters */
  assert_param(IS_CACHEAXI_ALL_INSTANCE(hcacheaxi->Instance));

  /*return the Write Miss monitor value*/
  return hcacheaxi->Instance->WMMONR;
}

/**
  * @brief  Get the CACHEAXI performance Read-allocate Miss monitoring value.
  * @param  hcacheaxi Pointer to a CACHEAXI_HandleTypeDef structure that contains
  *                   the configuration information for the specified CACHEAXIx peripheral.
  * @note   Upon reaching the 16-bit maximum value, monitor does not wrap.
  * @retval Read-allocate Miss monitoring value
  */
uint32_t HAL_CACHEAXI_Monitor_GetReadAllocMissValue(const CACHEAXI_HandleTypeDef *hcacheaxi)
{
  /* Check the parameters */
  assert_param(IS_CACHEAXI_ALL_INSTANCE(hcacheaxi->Instance));

  /*return the read-allocate Miss monitor value*/
  return hcacheaxi->Instance->RAMMONR;
}

/**
  * @brief  Get the CACHEAXI performance Write-allocate Miss monitoring value.
  * @param  hcacheaxi Pointer to a CACHEAXI_HandleTypeDef structure that contains
  *                   the configuration information for the specified CACHEAXIx peripheral.
  * @note   Upon reaching the 16-bit maximum value, monitor does not wrap.
  * @retval Write-allocate Miss monitoring value
  */
uint32_t HAL_CACHEAXI_Monitor_GetWriteAllocMissValue(const CACHEAXI_HandleTypeDef *hcacheaxi)
{
  /* Check the parameters */
  assert_param(IS_CACHEAXI_ALL_INSTANCE(hcacheaxi->Instance));

  /*return the write-allocate Miss monitor value*/
  return hcacheaxi->Instance->WAMMONR;
}

/**
  * @brief  Get the CACHEAXI performance Write-through monitoring value.
  * @param  hcacheaxi Pointer to a CACHEAXI_HandleTypeDef structure that contains
  *                   the configuration information for the specified CACHEAXIx peripheral.
  * @note   Upon reaching the 16-bit maximum value, monitor does not wrap.
  * @retval Write-through monitoring value
  */
uint32_t HAL_CACHEAXI_Monitor_GetWriteThroughValue(const CACHEAXI_HandleTypeDef *hcacheaxi)
{
  /* Check the parameters */
  assert_param(IS_CACHEAXI_ALL_INSTANCE(hcacheaxi->Instance));

  /*return the Write-through monitor value*/
  return hcacheaxi->Instance->WTMONR;
}

/**
  * @brief  Get the CACHEAXI performance Eviction Miss monitoring value.
  * @param  hcacheaxi Pointer to a CACHEAXI_HandleTypeDef structure that contains
  *                   the configuration information for the specified CACHEAXIx peripheral.
  * @note   Upon reaching the 16-bit maximum value, monitor does not wrap.
  * @retval Eviction monitoring value
  */
uint32_t HAL_CACHEAXI_Monitor_GetEvictionValue(const CACHEAXI_HandleTypeDef *hcacheaxi)
{
  /* Check the parameters */
  assert_param(IS_CACHEAXI_ALL_INSTANCE(hcacheaxi->Instance));

  /*return the Eviction  monitor value*/
  return hcacheaxi->Instance->EVIMONR;
}

/**
  * @brief Handle the CACHEAXI interrupt request.
  * @param  hcacheaxi Pointer to a CACHEAXI_HandleTypeDef structure that contains
  *                   the configuration information for the specified CACHEAXIx peripheral.
  * @note  This API should be called under the CACHEAXIx_IRQHandler().
  * @retval None
  */
void HAL_CACHEAXI_IRQHandler(CACHEAXI_HandleTypeDef *hcacheaxi)
{
  uint32_t itflags;
  uint32_t itsources;

  /* Check the parameters */
  assert_param(IS_CACHEAXI_ALL_INSTANCE(hcacheaxi->Instance));

  /* Get current interrupt flags and interrupt sources value */
  itflags   = READ_REG(hcacheaxi->Instance->SR);
  itsources = READ_REG(hcacheaxi->Instance->IER);

  /* Check Data cache Error interrupt flag  */
  if (((itflags & itsources) & CACHEAXI_FLAG_ERROR) != 0U)
  {
    /* Clear CACHEAXI error pending flag */
    __HAL_CACHEAXI_CLEAR_FLAG(hcacheaxi, CACHEAXI_FLAG_ERROR);

    /* Update data cache error code */
    hcacheaxi->ErrorCode = HAL_CACHEAXI_ERROR_EVICTION_CLEAN;

#if (USE_HAL_CACHEAXI_REGISTER_CALLBACKS == 1)
    /* Data cache error interrupt user callback */
    hcacheaxi->ErrorCallback(hcacheaxi);
#else
    /* Data cache error interrupt user callback */
    HAL_CACHEAXI_ErrorCallback(hcacheaxi);
#endif /* USE_HAL_CACHEAXI_REGISTER_CALLBACKS */
  }
  /* Check for end of full invalidate operation */
  if (READ_BIT(hcacheaxi->Instance->CR2, CACHEAXI_CR2_CACHECMD) == 0U)
  {
    /* Clear CACHEAXI busyend pending flag */
    __HAL_CACHEAXI_CLEAR_FLAG(hcacheaxi, CACHEAXI_FLAG_BUSYEND);

#if (USE_HAL_CACHEAXI_REGISTER_CALLBACKS == 1)
    /* Data cache invalidate complete interrupt user callback */
    hcacheaxi->InvalidateCompleteCallback(hcacheaxi);
#else
    /* Data cache invalidate complete interrupt user callback */
    HAL_CACHEAXI_InvalidateCompleteCallback(hcacheaxi);
#endif /* USE_HAL_CACHEAXI_REGISTER_CALLBACKS */
  }

  /* Check for end of clean and invalidate by address operation */
  else if (READ_BIT(hcacheaxi->Instance->CR1, CACHEAXI_COMMAND_CLEAN_INVALIDATE) == \
           (CACHEAXI_COMMAND_CLEAN_INVALIDATE))
  {
    /* Clear CACHEAXI cmdend pending flag */
    __HAL_CACHEAXI_CLEAR_FLAG(hcacheaxi, CACHEAXI_FLAG_CMDEND);

#if (USE_HAL_CACHEAXI_REGISTER_CALLBACKS == 1)
    /* Data cache clean and invalidate range cmdend interrupt user callback */
    hcacheaxi->CleanAndInvalidateByAddrCallback(hcacheaxi);
#else
    /* Data cache clean and invalidate range cmdend interrupt user callback */
    HAL_CACHEAXI_CleanAndInvalidateByAddrCallback(hcacheaxi);
#endif /* USE_HAL_CACHEAXI_REGISTER_CALLBACKS */
  }

  /* Check for end of clean by address operation */
  else
  {
    /* Clear CACHEAXI cmdend pending flag */
    __HAL_CACHEAXI_CLEAR_FLAG(hcacheaxi, CACHEAXI_FLAG_CMDEND);

#if (USE_HAL_CACHEAXI_REGISTER_CALLBACKS == 1)
    /* Data cache clean range cmdend interrupt user callback */
    hcacheaxi->CleanByAddrCallback(hcacheaxi);
#else
    /* Data cache clean range cmdend interrupt user callback */
    HAL_CACHEAXI_CleanByAddrCallback(hcacheaxi);
#endif /* USE_HAL_CACHEAXI_REGISTER_CALLBACKS */
  }
}

#if (USE_HAL_CACHEAXI_REGISTER_CALLBACKS == 1)
/**
  * @brief  Register a User CACHEAXI Callback
  *         To be used instead of the weak predefined callback
  * @param  hcacheaxi Pointer to a CACHEAXI_HandleTypeDef structure that contains
  *                   the configuration information for the specified CACHEAXIx peripheral.
  * @param  CallbackID ID of the callback to be registered
  *         This parameter can be one of the following values:
  *          @arg @ref HAL_CACHEAXI_CLEAN_BY_ADDRESS_CB_ID Clean By Addr callback ID
  *          @arg @ref HAL_CACHEAXI_INVALIDATE_BY_ADDRESS_CB_ID Invalidate By Addr callback ID
  *          @arg @ref HAL_CACHEAXI_CLEAN_AND_INVALIDATE_BY_ADDRESS_CB_ID Clean and Invalidate By Addr callback ID
  *          @arg @ref HAL_CACHEAXI_INVALIDATE_COMPLETE_CB_ID Invalidate Complete ID
  *          @arg @ref HAL_CACHEAXI_ERROR_CB_ID  Error callback ID
  *          @arg @ref HAL_CACHEAXI_MSPINIT_CB_ID MspInit callback ID
  *          @arg @ref HAL_CACHEAXI_MSPDEINIT_CB_ID MspDeInit callback ID
  * @param  pCallback pointer to the Callback function
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CACHEAXI_RegisterCallback(CACHEAXI_HandleTypeDef *hcacheaxi,
                                                HAL_CACHEAXI_CallbackIDTypeDef CallbackID,
                                                pCACHEAXI_CallbackTypeDef pCallback)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check the cacheaxi handle allocation */
  if (hcacheaxi == NULL)
  {
    return HAL_ERROR;
  }

  if (pCallback == NULL)
  {
    /* Update the error code */
    hcacheaxi->ErrorCode |= HAL_CACHEAXI_ERROR_INVALID_CALLBACK;

    /* Return error status */
    return HAL_ERROR;
  }

  if (hcacheaxi->State == HAL_CACHEAXI_STATE_READY)
  {
    switch (CallbackID)
    {
      case HAL_CACHEAXI_CLEAN_BY_ADDRESS_CB_ID :
        hcacheaxi->CleanByAddrCallback = pCallback;
        break;

      case HAL_CACHEAXI_CLEAN_AND_INVALIDATE_BY_ADDRESS_CB_ID :
        hcacheaxi->CleanAndInvalidateByAddrCallback = pCallback;
        break;

      case HAL_CACHEAXI_INVALIDATE_COMPLETE_CB_ID :
        hcacheaxi->InvalidateCompleteCallback = pCallback;
        break;

      case HAL_CACHEAXI_ERROR_CB_ID :
        hcacheaxi->ErrorCallback = pCallback;
        break;

      case HAL_CACHEAXI_MSPINIT_CB_ID :
        hcacheaxi->MspInitCallback = pCallback;
        break;

      case HAL_CACHEAXI_MSPDEINIT_CB_ID :
        hcacheaxi->MspDeInitCallback = pCallback;
        break;

      default :
        /* Update the error code */
        hcacheaxi->ErrorCode |= HAL_CACHEAXI_ERROR_INVALID_CALLBACK;

        /* Return error status */
        status =  HAL_ERROR;
        break;
    }
  }
  else if (hcacheaxi->State == HAL_CACHEAXI_STATE_RESET)
  {
    switch (CallbackID)
    {
      case HAL_CACHEAXI_MSPINIT_CB_ID :
        hcacheaxi->MspInitCallback = pCallback;
        break;

      case HAL_CACHEAXI_MSPDEINIT_CB_ID :
        hcacheaxi->MspDeInitCallback = pCallback;
        break;

      default :
        /* Update the error code */
        hcacheaxi->ErrorCode |= HAL_CACHEAXI_ERROR_INVALID_CALLBACK;

        /* Return error status */
        status =  HAL_ERROR;
        break;
    }
  }
  else
  {
    /* Update the error code */
    hcacheaxi->ErrorCode |= HAL_CACHEAXI_ERROR_INVALID_CALLBACK;

    /* Return error status */
    status =  HAL_ERROR;
  }

  return status;
}

/**
  * @brief  Unregister an CACHEAXI Callback
  *         CACHEAXI callback is redirected to the weak predefined callback
  * @param  hcacheaxi Pointer to a CACHEAXI_HandleTypeDef structure that contains
  *                   the configuration information for the specified CACHEAXIx peripheral.
  * @param  CallbackID ID of the callback to be unregistered
  *         This parameter can be one of the following values:
  *          @arg @ref HAL_CACHEAXI_CLEAN_BY_ADDRESS_CB_ID Clean By Addr callback ID
  *          @arg @ref HAL_CACHEAXI_CLEAN_AND_INVALIDATE_BY_ADDRESS_CB_ID Clean and Invalidate By Addr callback ID
  *          @arg @ref HAL_CACHEAXI_INVALIDATE_COMPLETE_CB_ID Invalidate Complete callback ID
  *          @arg @ref HAL_CACHEAXI_ERROR_CB_ID  Error callback ID
  *          @arg @ref HAL_CACHEAXI_MSPINIT_CB_ID MspInit callback ID
  *          @arg @ref HAL_CACHEAXI_MSPDEINIT_CB_ID MspDeInit callback ID
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CACHEAXI_UnRegisterCallback(CACHEAXI_HandleTypeDef *hcacheaxi,
                                                  HAL_CACHEAXI_CallbackIDTypeDef CallbackID)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check the cacheaxi handle allocation */
  if (hcacheaxi == NULL)
  {
    return HAL_ERROR;
  }

  if (hcacheaxi->State == HAL_CACHEAXI_STATE_READY)
  {
    switch (CallbackID)
    {
      case HAL_CACHEAXI_CLEAN_BY_ADDRESS_CB_ID :
        /* Legacy weak Clean By Addr Callback */
        hcacheaxi->CleanByAddrCallback = HAL_CACHEAXI_CleanByAddrCallback;
        break;

      case HAL_CACHEAXI_CLEAN_AND_INVALIDATE_BY_ADDRESS_CB_ID :
        /* Legacy weak Clean and Invalidate By Addr Callback */
        hcacheaxi->CleanAndInvalidateByAddrCallback = HAL_CACHEAXI_CleanAndInvalidateByAddrCallback;
        break;

      case HAL_CACHEAXI_INVALIDATE_COMPLETE_CB_ID :
        /* Legacy weak Invalidate Complete Callback */
        hcacheaxi->InvalidateCompleteCallback = HAL_CACHEAXI_InvalidateCompleteCallback;
        break;

      case HAL_CACHEAXI_ERROR_CB_ID :
        /* Legacy weak ErrorCallback */
        hcacheaxi->ErrorCallback = HAL_CACHEAXI_ErrorCallback;
        break;

      case HAL_CACHEAXI_MSPINIT_CB_ID :
        /* Legacy weak MspInit */
        hcacheaxi->MspInitCallback = HAL_CACHEAXI_MspInit;
        break;

      case HAL_CACHEAXI_MSPDEINIT_CB_ID :
        /* Legacy weak MspDeInit */
        hcacheaxi->MspDeInitCallback = HAL_CACHEAXI_MspDeInit;
        break;

      default :
        /* Update the error code */
        hcacheaxi->ErrorCode |= HAL_CACHEAXI_ERROR_INVALID_CALLBACK;

        /* Return error status */
        status =  HAL_ERROR;
        break;
    }
  }
  else if (HAL_CACHEAXI_STATE_RESET == hcacheaxi->State)
  {
    switch (CallbackID)
    {
      case HAL_CACHEAXI_MSPINIT_CB_ID :
        /* Legacy weak MspInit */
        hcacheaxi->MspInitCallback = HAL_CACHEAXI_MspInit;
        break;

      case HAL_CACHEAXI_MSPDEINIT_CB_ID :
        /* Legacy weak MspDeInit */
        hcacheaxi->MspDeInitCallback = HAL_CACHEAXI_MspDeInit;
        break;

      default :
        /* Update the error code */
        hcacheaxi->ErrorCode |= HAL_CACHEAXI_ERROR_INVALID_CALLBACK;

        /* Return error status */
        status =  HAL_ERROR;
        break;
    }
  }
  else
  {
    /* Update the error code */
    hcacheaxi->ErrorCode |= HAL_CACHEAXI_ERROR_INVALID_CALLBACK;

    /* Return error status */
    status =  HAL_ERROR;
  }

  return status;
}

#endif  /* USE_HAL_CACHEAXI_REGISTER_CALLBACKS */

/**
  * @brief  Cache clean  command  by address callback.
  * @param  hcacheaxi Pointer to a CACHEAXI_HandleTypeDef structure that contains
  *                   the configuration information for the specified CACHEAXIx peripheral.
  * @retval None
  */
__weak void HAL_CACHEAXI_CleanByAddrCallback(CACHEAXI_HandleTypeDef *hcacheaxi)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hcacheaxi);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_CACHEAXI_CleanByAddrCallback() should be implemented in the user file
   */
}

/**
  * @brief  Cache clean and Invalidate command  by address callback.
  * @param  hcacheaxi Pointer to a CACHEAXI_HandleTypeDef structure that contains
  *                   the configuration information for the specified CACHEAXIx peripheral.
  * @retval None
  */
__weak void HAL_CACHEAXI_CleanAndInvalidateByAddrCallback(CACHEAXI_HandleTypeDef *hcacheaxi)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hcacheaxi);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_CACHEAXI_CleanAndInvalidateByAddrCallback() should be implemented in the user file
   */
}

/**
  * @brief  Cache full invalidation complete callback.
  * @param  hcacheaxi Pointer to a CACHEAXI_HandleTypeDef structure that contains
  *                   the configuration information for the specified CACHEAXIx peripheral.
  * @retval None
  */
__weak void HAL_CACHEAXI_InvalidateCompleteCallback(CACHEAXI_HandleTypeDef *hcacheaxi)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hcacheaxi);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_CACHEAXI_InvalidateCompleteCallback() should be implemented in the user file
   */
}

/**
  * @brief  Error callback.
  * @param  hcacheaxi Pointer to a CACHEAXI_HandleTypeDef structure that contains
  *                   the configuration information for the specified CACHEAXIx peripheral.
  * @retval None
  */
__weak void HAL_CACHEAXI_ErrorCallback(CACHEAXI_HandleTypeDef *hcacheaxi)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hcacheaxi);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_CACHEAXI_ErrorCallback() should be implemented in the user file
   */
}

/**
  * @}
  */

/** @addtogroup CACHEAXI_Exported_Functions_Group3
  *
@verbatim
 ===============================================================================
            #####          Peripheral State          #####
 ===============================================================================
    [..]
    This subsection permit to get in run-time the status of the peripheral
    and the data flow.

@endverbatim
  * @{
  */

/**
  * @brief  Return the CACHEAXI handle state.
  * @param  hcacheaxi Pointer to a CACHEAXI_HandleTypeDef structure that contains
  *                   the configuration information for the specified CACHEAXIx peripheral.
  * @retval HAL state
  */
HAL_CACHEAXI_StateTypeDef HAL_CACHEAXI_GetState(const CACHEAXI_HandleTypeDef *hcacheaxi)
{
  /* Return CACHEAXI handle state */
  return hcacheaxi->State;
}

/**
  * @brief  Return the CACHEAXI error code
  * @param  hcacheaxi pointer to a CACHEAXI_HandleTypeDef structure that contains
  *                   the configuration information for the specified CACHEAXI.
  * @retval CACHEAXI Error Code
  */
uint32_t HAL_CACHEAXI_GetError(const CACHEAXI_HandleTypeDef *hcacheaxi)
{
  /* Return CACHEAXI handle error code */
  return hcacheaxi->ErrorCode;
}

/**
  * @}
  */

/* Private functions -------------------------------------------------------------------------------------------------*/
/** @defgroup CACHEAXI_Private_Functions CACHEAXI Private Functions
  * @brief    CACHEAXI Private Functions
  * @{
  */

/**
  * @brief  Launch CACHEAXI command Clean, Invalidate or clean and invalidate by Addr.
  * @param  hcacheaxi Pointer to a CACHEAXI_HandleTypeDef structure that contains
  *                   the configuration information for the specified CACHEAXIx peripheral.
  * @param  Command command to be applied for the CACHEAXI
  *                       CACHEAXI_COMMAND_CLEAN, CACHEAXI_COMMAND_CLEAN_INVALIDATE
  * @param  pAddr Start address of region to be Cleaned, Invalidated or Cleaned and Invalidated.
  * @param  dSize Size of the region to be Cleaned, Invalidated or Cleaned and Invalidated (in bytes).
  * @param  mode mode to be applied for the CACHEAXI
  *                       CACHEAXI_IT_MODE, CACHEAXI_POLLING_MODE.
  * @retval HAL status
  */
static HAL_StatusTypeDef CACHEAXI_CommandByAddr(CACHEAXI_HandleTypeDef *hcacheaxi, uint32_t Command,
                                                const uint32_t *pAddr, uint32_t dSize, uint32_t mode)
{
  HAL_StatusTypeDef status = HAL_OK;
  uint32_t op_addr = (uint32_t)pAddr;
  uint32_t tickstart;

  /* Check no ongoing operation */
  if (READ_BIT(hcacheaxi->Instance->SR, (CACHEAXI_SR_BUSYF | CACHEAXI_SR_BUSYCMDF)) != 0U)
  {
    /* Return busy status */
    status =  HAL_BUSY;
  }
  else
  {
    /* Update the error code */
    hcacheaxi->ErrorCode = HAL_CACHEAXI_ERROR_NONE;

    /* Update the CACHEAXI handle State */
    hcacheaxi->State = HAL_CACHEAXI_STATE_READY;

    /* Make sure flags are reset */
    WRITE_REG(hcacheaxi->Instance->FCR, (CACHEAXI_FCR_CBSYENDF | CACHEAXI_FCR_CCMDENDF));

    /* Fill area start address */
    WRITE_REG(hcacheaxi->Instance->CMDRSADDRR, op_addr);

    /* Fill area end address */
    WRITE_REG(hcacheaxi->Instance->CMDREADDRR, (op_addr + dSize - 1U));

    /* Set command */
    MODIFY_REG(hcacheaxi->Instance->CR2, CACHEAXI_CR2_CACHECMD, Command);

    /* Enable IT if required */
    if (mode == CACHEAXI_IT_MODE)
    {
      /* Enable end of cache command interrupt */
      SET_BIT(hcacheaxi->Instance->IER, CACHEAXI_IER_CMDENDIE);

      /* Launch cache command */
      SET_BIT(hcacheaxi->Instance->CR2, CACHEAXI_CR2_STARTCMD);
    }
    else
    {
      /* Make sure that end of cache command interrupt is disabled */
      CLEAR_BIT(hcacheaxi->Instance->IER, CACHEAXI_IER_CMDENDIE);

      /* Launch cache command */
      SET_BIT(hcacheaxi->Instance->CR2, CACHEAXI_CR2_STARTCMD);

      /* Get timeout */
      tickstart = HAL_GetTick();

      /* Wait for end of cache command */
      while (READ_BIT(hcacheaxi->Instance->SR, CACHEAXI_SR_CMDENDF) == 0U)
      {
        if ((HAL_GetTick() - tickstart) > CACHEAXI_COMMAND_TIMEOUT_VALUE)
        {
          if (READ_BIT(hcacheaxi->Instance->SR, CACHEAXI_SR_CMDENDF) == 0U)
          {
            /* Update error code */
            hcacheaxi->ErrorCode = HAL_CACHEAXI_ERROR_TIMEOUT;

            /* Change the CACHEAXI state */
            hcacheaxi->State = HAL_CACHEAXI_STATE_ERROR;

            /* Return error status */
            status =  HAL_ERROR;
            break;
          }
        }
      }
    }
  }

  return status;
}

/**
  * @}
  */

#endif /* HAL_CACHEAXI_MODULE_ENABLED */

/**
  * @}
  */

#endif /* CACHEAXI */
/**
  * @}
  */
