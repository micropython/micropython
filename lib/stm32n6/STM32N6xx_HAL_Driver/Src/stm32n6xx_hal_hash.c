/**
  ******************************************************************************
  * @file    stm32n6xx_hal_hash.c
  * @author  MCD Application Team
  * @brief   HASH HAL module driver.
  *          This file provides firmware functions to manage HASH peripheral
  *
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
    The HASH HAL driver can be used as follows:

    (#)Initialize the HASH low level resources by implementing the HAL_HASH_MspInit():
        (##) Enable the HASH interface clock using __HAL_RCC_HASH_CLK_ENABLE()
        (##) When resorting to interrupt-based APIs (e.g. HAL_HASH_Start_IT())
            (+++) Configure the HASH interrupt priority using HAL_NVIC_SetPriority()
            (+++) Enable the HASH IRQ handler using HAL_NVIC_EnableIRQ()
            (+++) In HASH IRQ handler, call HAL_HASH_IRQHandler() API
        (##) When resorting to DMA-based APIs  (e.g. HAL_HASH_Start_DMA())
            (+++) Enable the DMA interface clock
            (+++) Configure and enable one DMA to manage data transfer from
                memory to peripheral (input DMA). Managing data transfer from
                peripheral to memory can be performed only using CPU.
            (+++) Associate the initialized DMA handle to the HASH DMA handle
                using  __HAL_LINKDMA()
            (+++) Configure the priority and enable the NVIC for the transfer complete
                interrupt on the DMA: use
                 HAL_NVIC_SetPriority() and
                 HAL_NVIC_EnableIRQ()

    (#)Initialize the HASH HAL using HAL_HASH_Init(). This function:
        (##) resorts to HAL_HASH_MspInit() for low-level initialization,
        (##) configures the data type: no swap, half word swap, bit swap or byte swap,
        (##) configures the Algorithm : MD5, SHA1 or SHA2

    (#)Three processing schemes are available:
        (##) Polling mode: processing APIs are blocking functions
             i.e. they process the data and wait till the digest computation is finished,
             e.g. HAL_HASH_Start() for HASH or HAL_HMAC_Start() for HMAC
        (##) Interrupt mode: processing APIs are not blocking functions
                i.e. they process the data under interrupt,
                e.g. HAL_HASH_Start_IT() for HASH or HAL_HMAC_Start_IT() for HMAC
        (##) DMA mode: processing APIs are not blocking functions and the CPU is
             not used for data transfer i.e. the data transfer is ensured by DMA,
                e.g. HAL_HASH_Start_DMA() for HASH or HAL_HMAC_Start_DMA() for HMAC.

    (#)When the processing function is called after HAL_HASH_Init(), the HASH peripheral is
       initialized and processes the buffer fed in input. When the input data have all been
       fed to the Peripheral, the digest computation can start.

    (#)Multi-buffer processing HASH and HMAC are possible in polling, interrupt and DMA modes.
        (##) In polling mode, API HAL_HASH_Accumulate()/HAL_HASH_HMAC_Accumulate() must be called
             for each input buffer, except for the last one.
             User must resort to HAL_HASH_AccumulateLast()/HAL_HASH_HMAC_AccumulateLast()
             to enter the last one and retrieve as well the computed digest.

        (##) In interrupt mode, API HAL_HASH_Accumulate_IT()/HAL_HASH_HMAC_Accumulate_IT() must
             be called for each input buffer, except for the last one.
             User must resort to HAL_HASH_AccumulateLast_IT()/HAL_HASH_HMAC_AccumulateLast_IT()
             to enter the last one and retrieve as well the computed digest.

        (##) In DMA mode, once initialization is done, MDMAT bit must be set through
             __HAL_HASH_SET_MDMAT() macro.
             From that point, each buffer can be fed to the Peripheral through HAL_HASH_Start_DMA() API
             for HASH and HAL_HASH_HMAC_Start_DMA() API for HMAC .
             Before entering the last buffer, reset the MDMAT bit with __HAL_HASH_RESET_MDMAT()
             macro then wrap-up the HASH processing in feeding the last input buffer through the
             same API HAL_HASH_Start_DMA()for HASH and HAL_HASH_HMAC_Start_DMA() API for HMAC and
             retrieve as well the computed digest.

    (#)To use this driver (version 2.0.0) with application developed with old driver (version 1.0.0) user have to:
        (##) Add Algorithm as parameter like DataType or KeySize.
        (##) Use new API HAL_HASH_Start() for HASH and HAL_HASH_HMAC_Start() for HMAC processing instead of old API
             like HAL_HASH_SHA1_Start and HAL_HMAC_SHA1_Start.


  @endverbatim
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal.h"


/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */

#if defined (HASH)

/** @defgroup HASH  HASH
  * @brief HASH HAL module driver.
  * @{
  */

#ifdef HAL_HASH_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/** @defgroup HASH_Private_Defines HASH Private Defines
  * @{
  */
#define HASH_TIMEOUTVALUE                         1000U  /*!< Time-out value  */
#define BLOCK_64B                                 64U    /*!< block Size equal to 64 bytes */
#define BLOCK_128B                                128U   /*!< block Size equal to 128 bytes */
/**
  * @}
  */

/** @defgroup HASH_Number_Of_CSR_Registers HASH Number of Context Swap Registers
  * @{
  */
#define HASH_NUMBER_OF_CSR_REGISTERS              103U /*!< Number of Context Swap Registers */
/**
  * @}
  */

/* Private Constants ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/** @defgroup HASH_Private_Functions HASH Private Functions
  * @{
  */
static void HASH_GetDigest(const HASH_HandleTypeDef *hhash, const uint8_t *pMsgDigest, uint8_t Size);
static void HASH_WriteData(HASH_HandleTypeDef *hhash, const uint8_t *pInBuffer, uint32_t Size);
static HAL_StatusTypeDef HASH_WriteData_IT(HASH_HandleTypeDef *hhash);
static void HASH_DMAXferCplt(DMA_HandleTypeDef *hdma);
static void HASH_DMAError(DMA_HandleTypeDef *hdma);
static HAL_StatusTypeDef HASH_WaitOnFlagUntilTimeout(HASH_HandleTypeDef *hhash, uint32_t Flag, FlagStatus Status,
                                                     uint32_t Timeout);
/**
  * @}
  */

/* Exported functions ---------------------------------------------------------*/

/** @defgroup HASH_Exported_Functions HASH Exported Functions
  * @{
  */

/** @defgroup HASH_Exported_Functions_Group1 Initialization and de-initialization functions
  *  @brief    Initialization and configuration functions.
  *
@verbatim
 ===============================================================================
              ##### Initialization and de-initialization functions #####
 ===============================================================================
    [..]  This section provides functions allowing to:
      (+) Initialize the HASH according to the specified parameters
          in the HASH_InitTypeDef and create the associated handle
      (+) DeInitialize the HASH peripheral
      (+) Initialize the HASH MCU Specific Package (MSP)
      (+) DeInitialize the HASH MSP
      (+) Configure HASH (HAL_HASH_SetConfig) with the specified parameters in the HASH_ConfigTypeDef
          Parameters which are configured in This section are :
          (+) Data Type : no swap, half word swap, bit swap or byte swap
          (+) Algorithm : MD5,SHA1 or SHA2
      (+) Get HASH configuration (HAL_HASH_GetConfig) from the specified parameters in the HASH_HandleTypeDef

@endverbatim
  * @{
  */

/**
  * @brief  Initializes the HASH according to the specified parameters in the
            HASH_HandleTypeDef and create the associated handle.
  * @note   Only Algorithm and DATATYPE bits of HASH Peripheral are set by HAL_HASH_Init(),
  *         other configuration bits are set by HASH or HMAC processing APIs.
  * @param  hhash pointer to a HASH_HandleTypeDef structure that contains
  *         the configuration information for HASH module.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HASH_Init(HASH_HandleTypeDef *hhash)
{
  uint32_t cr_value;

  /* Check the hash handle allocation */
  if (hhash == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_HASH_DATATYPE(hhash->Init.DataType));
  assert_param(IS_HASH_ALGORITHM(hhash->Init.Algorithm));

#if (USE_HAL_HASH_REGISTER_CALLBACKS == 1)
  if (hhash->State == HAL_HASH_STATE_RESET)
  {
    /* Allocate lock resource and initialize it */
    hhash->Lock = HAL_UNLOCKED;

    /* Reset Callback pointers in HAL_HASH_STATE_RESET only */
    hhash->InCpltCallback =  HAL_HASH_InCpltCallback;     /* Legacy weak InCpltCallback */
    hhash->DgstCpltCallback =  HAL_HASH_DgstCpltCallback; /* Legacy weak DgstCpltCallback */
    hhash->ErrorCallback =  HAL_HASH_ErrorCallback;       /* Legacy weak ErrorCallback */
    if (hhash->MspInitCallback == NULL)
    {
      hhash->MspInitCallback = HAL_HASH_MspInit;
    }

    /* Init the low level hardware */
    hhash->MspInitCallback(hhash);
  }
#else
  if (hhash->State == HAL_HASH_STATE_RESET)
  {
    /* Allocate lock resource and initialize it */
    hhash->Lock = HAL_UNLOCKED;

    /* Init the low level hardware */
    HAL_HASH_MspInit(hhash);
  }
#endif /* (USE_HAL_HASH_REGISTER_CALLBACKS) */

  /* Set the key size, data type and Algorithm */
  cr_value = (uint32_t)(hhash->Init.DataType | hhash->Init.Algorithm);
  /* Set the key size, data type, algorithm and mode */
  MODIFY_REG(hhash->Instance->CR, HASH_CR_DATATYPE | HASH_CR_ALGO | HASH_CR_INIT, cr_value);

  /* Change HASH phase to Ready */
  hhash->Phase = HAL_HASH_PHASE_READY;

  /* Change HASH state to Ready */
  hhash->State = HAL_HASH_STATE_READY;

  /* Reset error code field */
  hhash->ErrorCode = HAL_HASH_ERROR_NONE;

#if (USE_HAL_HASH_SUSPEND_RESUME == 1U)
  /* Reset suspension request flag */
  hhash->SuspendRequest = HAL_HASH_SUSPEND_NONE;
#endif /* (USE_HAL_HASH_SUSPEND_RESUME) */
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  DeInitialize the HASH peripheral.
  * @param  hhash pointer to a HASH_HandleTypeDef structure that contains
  *         the configuration information for HASH module.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HASH_DeInit(HASH_HandleTypeDef *hhash)
{
  /* Check the HASH handle allocation */
  if (hhash == NULL)
  {
    return HAL_ERROR;
  }

  /* Change the default HASH phase */
  hhash->Phase = HAL_HASH_PHASE_READY;

  /* Reset HashInCount */
  hhash->HashInCount = 0U;

  /* Reset multi buffers accumulation flag */
  hhash->Accumulation = 0U;

#if (USE_HAL_HASH_REGISTER_CALLBACKS == 1)
  if (hhash->MspDeInitCallback == NULL)
  {
    hhash->MspDeInitCallback = HAL_HASH_MspDeInit;
  }

  /* DeInit the low level hardware */
  hhash->MspDeInitCallback(hhash);
#else
  /* DeInit the low level hardware: CLOCK, NVIC */
  HAL_HASH_MspDeInit(hhash);
#endif /* (USE_HAL_HASH_REGISTER_CALLBACKS) */

  /* Set the HASH state to Ready */
  hhash->State = HAL_HASH_STATE_RESET;

  __HAL_UNLOCK(hhash);

  return HAL_OK;
}

/**
  * @brief  Configure the HASH according to the specified
  *         parameters in the HASH_ConfigTypeDef
  * @param  hhash pointer to a HASH_HandleTypeDef structure
  * @param  pConf pointer to a HASH_ConfigTypeDef structure that contains
  *         the configuration information for HASH module
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HASH_SetConfig(HASH_HandleTypeDef *hhash, const HASH_ConfigTypeDef *pConf)
{
  uint32_t cr_value;

  /* Check the HASH handle allocation */
  if ((hhash == NULL) || (pConf == NULL))
  {
    return HAL_ERROR;
  }

  /* Check parameters */
  assert_param(IS_HASH_DATATYPE(pConf->DataType));
  assert_param(IS_HASH_ALGORITHM(pConf->Algorithm));

  if (hhash->State == HAL_HASH_STATE_READY)
  {
    /* Change the HASH state */
    hhash->State = HAL_HASH_STATE_BUSY;
    __HAL_LOCK(hhash);

    /* Set HASH parameters */
    hhash->Init.DataType   = pConf->DataType;
    hhash->Init.pKey       = pConf->pKey;
    hhash->Init.Algorithm  = pConf->Algorithm;
    hhash->Init.KeySize    = pConf->KeySize;

    /* Set the key size, data type and Algorithm */
    cr_value = (uint32_t)(hhash->Init.DataType | hhash->Init.Algorithm);
    /* Set the key size, data type, algorithm and mode */
    MODIFY_REG(hhash->Instance->CR, HASH_CR_DATATYPE | HASH_CR_ALGO | HASH_CR_INIT, cr_value);

    /* Change HASH phase to Ready */
    hhash->Phase = HAL_HASH_PHASE_READY;

    /* Change HASH state to Ready */
    hhash->State = HAL_HASH_STATE_READY;

    /* Reset error code field */
    hhash->ErrorCode = HAL_HASH_ERROR_NONE;

    __HAL_UNLOCK(hhash);

    return HAL_OK;

  }
  else
  {
    /* Busy error code field */
    hhash->ErrorCode |= HAL_HASH_ERROR_BUSY;
    return HAL_ERROR;
  }
}

/**
  * @brief  Get HASH Configuration parameters in associated handle
  * @param  pConf pointer to a HASH_HandleTypeDef structure
  * @param  hhash pointer to a HASH_ConfigTypeDef structure that contains
  *         the configuration information for HASH module
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HASH_GetConfig(HASH_HandleTypeDef *hhash, HASH_ConfigTypeDef *pConf)
{

  /* Check the HASH handle allocation */
  if ((hhash == NULL) || (pConf == NULL))
  {
    return HAL_ERROR;
  }

  if (hhash->State == HAL_HASH_STATE_READY)
  {
    /* Change the HASH state */
    hhash->State = HAL_HASH_STATE_BUSY;
    __HAL_LOCK(hhash);

    /* Set HASH parameters */
    pConf->DataType   = hhash->Init.DataType;
    pConf->pKey       = hhash->Init.pKey;
    pConf->Algorithm  = hhash->Init.Algorithm;
    pConf->KeySize    = hhash->Init.KeySize;

    /* Change HASH state to Ready */
    hhash->State = HAL_HASH_STATE_READY;
    __HAL_UNLOCK(hhash);

    return HAL_OK;

  }
  else
  {
    /* Busy error code field */
    hhash->ErrorCode |= HAL_HASH_ERROR_BUSY;
    return HAL_ERROR;
  }
}

/**
  * @brief  Initialize the HASH MSP.
  * @param  hhash pointer to a HASH_HandleTypeDef structure that contains
  *         the configuration information for HASH module.
  * @retval None
  */
__weak void HAL_HASH_MspInit(HASH_HandleTypeDef *hhash)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hhash);

  /* NOTE : This function should not be modified; when the callback is needed,
            HAL_HASH_MspInit() can be implemented in the user file.
   */
}

/**
  * @brief  DeInitialize the HASH MSP.
  * @param  hhash pointer to a HASH_HandleTypeDef structure that contains
  *         the configuration information for HASH module.
  * @retval None
  */
__weak void HAL_HASH_MspDeInit(HASH_HandleTypeDef *hhash)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hhash);

  /* NOTE : This function should not be modified; when the callback is needed,
            HAL_HASH_MspDeInit() can be implemented in the user file.
   */
}


#if (USE_HAL_HASH_REGISTER_CALLBACKS == 1)
/**
  * @brief  Register a User HASH Callback
  *         To be used instead of the weak (overridden) predefined callback
  * @param hhash HASH handle
  * @param CallbackID ID of the callback to be registered
  *        This parameter can be one of the following values:
  *          @arg HAL_HASH_INPUTCPLT_CB_ID input completion callback ID
  *          @arg HAL_HASH_DGSTCPLT_CB_ID digest computation completion callback ID
  *          @arg HAL_HASH_ERROR_CB_ID error callback ID
  *          @arg HAL_HASH_MSPINIT_CB_ID MspInit callback ID
  *          @arg HAL_HASH_MSPDEINIT_CB_ID MspDeInit callback ID
  * @param pCallback pointer to the Callback function
  * @retval status
  */
HAL_StatusTypeDef HAL_HASH_RegisterCallback(HASH_HandleTypeDef *hhash, HAL_HASH_CallbackIDTypeDef CallbackID,
                                            pHASH_CallbackTypeDef pCallback)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (pCallback == NULL)
  {
    /* Update the error code */
    hhash->ErrorCode |= HAL_HASH_ERROR_INVALID_CALLBACK;
    return HAL_ERROR;
  }

  if (hhash->State == HAL_HASH_STATE_READY)
  {
    switch (CallbackID)
    {
      case HAL_HASH_INPUTCPLT_CB_ID :
        hhash->InCpltCallback = pCallback;
        break;

      case HAL_HASH_DGSTCPLT_CB_ID :
        hhash->DgstCpltCallback = pCallback;
        break;

      case HAL_HASH_ERROR_CB_ID :
        hhash->ErrorCallback = pCallback;
        break;

      case HAL_HASH_MSPINIT_CB_ID :
        hhash->MspInitCallback = pCallback;
        break;

      case HAL_HASH_MSPDEINIT_CB_ID :
        hhash->MspDeInitCallback = pCallback;
        break;

      default :
        /* Update the error code */
        hhash->ErrorCode |= HAL_HASH_ERROR_INVALID_CALLBACK;
        /* update return status */
        status =  HAL_ERROR;
        break;
    }
  }
  else if (hhash->State == HAL_HASH_STATE_RESET)
  {
    switch (CallbackID)
    {
      case HAL_HASH_MSPINIT_CB_ID :
        hhash->MspInitCallback = pCallback;
        break;

      case HAL_HASH_MSPDEINIT_CB_ID :
        hhash->MspDeInitCallback = pCallback;
        break;

      default :
        /* Update the error code */
        hhash->ErrorCode |= HAL_HASH_ERROR_INVALID_CALLBACK;
        /* update return status */
        status =  HAL_ERROR;
        break;
    }
  }
  else
  {
    /* Update the error code */
    hhash->ErrorCode |= HAL_HASH_ERROR_INVALID_CALLBACK;
    /* update return status */
    status =  HAL_ERROR;
  }

  return status;
}

/**
  * @brief  Unregister a HASH Callback
  *         HASH Callback is redirected to the weak (overridden) predefined callback
  * @param hhash HASH handle
  * @param CallbackID ID of the callback to be unregistered
  *        This parameter can be one of the following values:
  *          @arg HAL_HASH_INPUTCPLT_CB_ID HASH input completion Callback ID
  *          @arg HAL_HASH_DGSTCPLT_CB_ID HASH digest computation completion Callback ID
  *          @arg HAL_HASH_ERROR_CB_ID HASH error Callback ID
  *          @arg HAL_HASH_MSPINIT_CB_ID HASH MspInit callback ID
  *          @arg HAL_HASH_MSPDEINIT_CB_ID HASH MspDeInit callback ID
  * @retval status
  */
HAL_StatusTypeDef HAL_HASH_UnRegisterCallback(HASH_HandleTypeDef *hhash, HAL_HASH_CallbackIDTypeDef CallbackID)
{
  HAL_StatusTypeDef status = HAL_OK;


  if (hhash->State == HAL_HASH_STATE_READY)
  {
    switch (CallbackID)
    {
      case HAL_HASH_INPUTCPLT_CB_ID :
        hhash->InCpltCallback = HAL_HASH_InCpltCallback;     /* Legacy weak input completion callback */
        break;

      case HAL_HASH_DGSTCPLT_CB_ID :
        hhash->DgstCpltCallback = HAL_HASH_DgstCpltCallback; /* Legacy weak digest computation
      completion callback */
        break;

      case HAL_HASH_ERROR_CB_ID :
        hhash->ErrorCallback = HAL_HASH_ErrorCallback;       /* Legacy weak error callback */
        break;

      case HAL_HASH_MSPINIT_CB_ID :
        hhash->MspInitCallback = HAL_HASH_MspInit;           /* Legacy weak MspInit Callback */
        break;

      case HAL_HASH_MSPDEINIT_CB_ID :
        hhash->MspDeInitCallback = HAL_HASH_MspDeInit;       /* Legacy weak MspDeInit Callback */
        break;

      default :
        /* Update the error code */
        hhash->ErrorCode |= HAL_HASH_ERROR_INVALID_CALLBACK;
        /* update return status */
        status =  HAL_ERROR;
        break;
    }
  }
  else if (hhash->State == HAL_HASH_STATE_RESET)
  {
    switch (CallbackID)
    {
      case HAL_HASH_MSPINIT_CB_ID :
        hhash->MspInitCallback = HAL_HASH_MspInit;           /* Legacy weak MspInit Callback */
        break;

      case HAL_HASH_MSPDEINIT_CB_ID :
        hhash->MspDeInitCallback = HAL_HASH_MspDeInit;       /* Legacy weak MspDeInit Callback */
        break;

      default :
        /* Update the error code */
        hhash->ErrorCode |= HAL_HASH_ERROR_INVALID_CALLBACK;
        /* update return status */
        status =  HAL_ERROR;
        break;
    }
  }
  else
  {
    /* Update the error code */
    hhash->ErrorCode |= HAL_HASH_ERROR_INVALID_CALLBACK;
    /* update return status */
    status =  HAL_ERROR;
  }

  return status;
}
#endif /* USE_HAL_HASH_REGISTER_CALLBACKS */

#if (USE_HAL_HASH_SUSPEND_RESUME == 1U)
/**
  * @brief  Save the HASH context in case of processing suspension.
  * @param  hhash HASH handle.
  * @param  pMemBuffer pointer to the memory buffer where the HASH context
  *         is saved.
  * @note   The IMR, STR, CR then all the CSR registers are saved
  *         in that order. Only the r/w bits are read to be restored later on.
  * @note   By default, all the context swap registers (there are
  *         HASH_NUMBER_OF_CSR_REGISTERS of those) are saved.
  * @note   pMemBuffer points to a buffer allocated by the user. The buffer size
  *         must be at least (HASH_NUMBER_OF_CSR_REGISTERS + 3) * 4 uint8 long.
  * @retval None
  */
void HAL_HASH_Suspend(HASH_HandleTypeDef *hhash, uint8_t *pMemBuffer)
{
  uint32_t mem_ptr = (uint32_t)pMemBuffer;
  uint32_t csr_ptr = (uint32_t)(hhash->Instance->CSR);
  uint32_t i;

  /* Prevent unused argument(s) compilation warning */
  UNUSED(hhash);

  /* Save IMR register content */
  *(uint32_t *)(mem_ptr) = READ_BIT(hhash->Instance->IMR, HASH_IT_DINI | HASH_IT_DCI);
  mem_ptr += 4U;
  /* Save STR register content */
  *(uint32_t *)(mem_ptr) = READ_BIT(hhash->Instance->STR, HASH_STR_NBLW);
  mem_ptr += 4U;
  /* Save CR register content */
  *(uint32_t *)(mem_ptr) = READ_BIT(hhash->Instance->CR, HASH_CR_DMAE | HASH_CR_DATATYPE | HASH_CR_MODE | HASH_CR_ALGO |
                                    HASH_CR_LKEY | HASH_CR_MDMAT);

  mem_ptr += 4U;
  /* By default, save all CSRs registers */
  for (i = HASH_NUMBER_OF_CSR_REGISTERS; i > 0U; i--)
  {
    *(uint32_t *)(mem_ptr) = *(uint32_t *)(csr_ptr);
    mem_ptr += 4U;
    csr_ptr += 4U;
  }
  /* Save low-priority block HASH handle parameters */
  hhash->Init_saved              = hhash->Init;
  hhash->pHashOutBuffPtr_saved   = hhash->pHashOutBuffPtr;
  hhash->HashInCount_saved       = hhash->HashInCount;
  hhash->Size_saved              = hhash->Size;
  hhash->pHashInBuffPtr_saved    = hhash->pHashInBuffPtr;
  hhash->Phase_saved             = hhash->Phase;
  hhash->pHashKeyBuffPtr_saved   = hhash->pHashKeyBuffPtr;
}


/**
  * @brief  Restore the HASH context in case of processing resumption.
  * @param  hhash HASH handle.
  * @param  pMemBuffer pointer to the memory buffer where the HASH context
  *         is stored.
  * @note   The IMR, STR, CR then all the CSR registers are restored
  *         in that order. Only the r/w bits are restored.
  * @note   By default, all the context swap registers (HASH_NUMBER_OF_CSR_REGISTERS
  *         of those) are restored (all of them have been saved by default
  *         beforehand).
  * @retval None
  */
void HAL_HASH_Resume(HASH_HandleTypeDef *hhash, uint8_t *pMemBuffer)
{
  uint32_t mem_ptr = (uint32_t)pMemBuffer;
  uint32_t csr_ptr = (uint32_t)(hhash->Instance->CSR);
  uint32_t i;

  /* Prevent unused argument(s) compilation warning */
  UNUSED(hhash);

  /* Restore IMR register content */
  WRITE_REG(hhash->Instance->IMR, (*(uint32_t *)(mem_ptr)));
  mem_ptr += 4U;
  /* Restore STR register content */
  WRITE_REG(hhash->Instance->STR, (*(uint32_t *)(mem_ptr)));
  mem_ptr += 4U;
  /* Restore CR register content */
  WRITE_REG(hhash->Instance->CR, (*(uint32_t *)(mem_ptr)));
  mem_ptr += 4U;

  /* Reset the HASH processor before restoring the Context
  Swap Registers (CSR) */
  SET_BIT(hhash->Instance->CR, HASH_CR_INIT);


  /* By default, restore all CSR registers */
  for (i = HASH_NUMBER_OF_CSR_REGISTERS; i > 0U; i--)
  {
    WRITE_REG((*(uint32_t *)(csr_ptr)), (*(uint32_t *)(mem_ptr)));
    mem_ptr += 4U;
    csr_ptr += 4U;
  }

  /* Restore low-priority block HASH handle parameters */
  hhash->Init              = hhash->Init_saved;
  hhash->pHashOutBuffPtr   = hhash->pHashOutBuffPtr_saved;
  hhash->HashInCount       = hhash->HashInCount_saved;
  hhash->Size              = hhash->Size_saved;
  hhash->pHashInBuffPtr    = hhash->pHashInBuffPtr_saved;
  hhash->Phase             = hhash->Phase_saved;
  hhash->State             = HAL_HASH_STATE_SUSPENDED;
  hhash->pHashKeyBuffPtr   = hhash->pHashKeyBuffPtr_saved;
}

/**
  * @brief  Initiate HASH processing suspension when in interruption mode.
  * @param  hhash HASH handle.
  * @note   Set the handle field SuspendRequest to the appropriate value so that
  *         the on-going HASH processing is suspended as soon as the required
  *         conditions are met. Note that the actual suspension is carried out
  *         by the functions HASH_WriteData() in polling mode and HASH_IT() in
  *         interruption mode.
  * @retval None
  */
HAL_StatusTypeDef HAL_HASH_ProcessSuspend(HASH_HandleTypeDef *hhash)
{
  uint32_t remainingwords; /*remaining number in of source block to be transferred.*/
  uint32_t nbbytePartialHash  = (((hhash->Instance->SR) >> 16U) * 4U); /* Nb byte  to enter in HASH fifo
                                                                      to trig a partial HASH computation*/
  uint32_t sizeinwords;/* number in word of source block to be transferred.*/

  /* suspension in DMA mode*/
  if (__HAL_HASH_GET_FLAG(hhash, HASH_FLAG_DMAS) != RESET)
  {
    if (hhash->State == HAL_HASH_STATE_READY)
    {
      return HAL_ERROR;
    }
    else
    {

      /* Clear the DMAE bit to disable the DMA interface */
      CLEAR_BIT(HASH->CR, HASH_CR_DMAE);

      /* Wait until the last DMA transfer is complete (DMAS = 0 in the HASH_SR register) */
      if (HASH_WaitOnFlagUntilTimeout(hhash, HASH_FLAG_DMAS, SET, HASH_TIMEOUTVALUE) != HAL_OK)
      {
        return HAL_TIMEOUT;
      }

      /* At this point, DMA interface is disabled and no transfer is on-going */
      /* Retrieve from the DMA handle how many words remain to be written */
      /* DMA3 used, DMA_CBR1_BNDT in bytes, DMA_CSR_FIFOL in words */
      remainingwords = ((((DMA_Channel_TypeDef *)hhash->hdmain->Instance)->CBR1) \
                        & DMA_CBR1_BNDT) / 4U;
      remainingwords += ((((DMA_Channel_TypeDef *)hhash->hdmain->Instance)->CSR) \
                         & DMA_CSR_FIFOL) >> DMA_CSR_FIFOL_Pos;

      if (remainingwords <= nbbytePartialHash)
      {
        /* No suspension attempted since almost to the end of the transferred data. */
        /* Best option for user code is to wrap up low priority message hashing     */
        return HAL_ERROR;
      }

      /* Disable DMA channel */
      /* Note that the Abort function will
        - Clear the transfer error flags
        - Unlock
        - Set the State
      */
      if (HAL_DMA_Abort(hhash->hdmain) != HAL_OK)
      {
        return HAL_ERROR;
      }

      if (__HAL_HASH_GET_FLAG(hhash, HASH_FLAG_DCIS) != RESET)
      {
        return HAL_ERROR;
      }

      /* Wait until the hash processor is ready (no block is being processed), that is wait for DINIS=1 in HASH_SR */
      if (HASH_WaitOnFlagUntilTimeout(hhash, HASH_FLAG_DINIS, RESET, HASH_TIMEOUTVALUE) != HAL_OK)
      {
        return HAL_TIMEOUT;
      }

      /* Compute how many words were supposed to be transferred by DMA */
      sizeinwords = (((hhash->Size % 4U) != 0U) ? \
                     ((hhash->Size + 3U) / 4U) : (hhash->Size / 4U));
      /* Accordingly, update the input pointer that points at the next word to be
         transferred to the Peripheral by DMA */
      hhash->pHashInBuffPtr +=  4U * (sizeinwords - remainingwords) ;

      /* And store in HashInCount the remaining size to transfer (in bytes) */
      hhash->HashInCount = 4U * remainingwords;


      hhash->State = HAL_HASH_STATE_SUSPENDED;
      __HAL_UNLOCK(hhash);
      return HAL_OK;
    }

  }
  else /* suspension when in interruption mode*/
  {
    /* Set Handle Suspend Request field */
    hhash->SuspendRequest = HAL_HASH_SUSPEND;
    return HAL_OK;
  }
}
#endif /* USE_HAL_HASH_SUSPEND_RESUME */
/**
  * @}
  */


/** @defgroup HASH_Exported_Functions_Group2 HASH processing functions
  *  @brief   HASH processing functions using different mode.
  *
@verbatim
 ===============================================================================
                 ##### HASH processing functions #####
 ===============================================================================
    [..]  This section provides API allowing to calculate the hash value using
          one of the HASH algorithms supported by the peripheral.

    [..] For a single buffer to be hashed, user can resort to one of three processing
         functions available .
      (+) Polling mode : HAL_HASH_Start()
      (+) Interrupt mode : HAL_HASH_Start_IT()
      (+) DMA mode : HAL_HASH_Start_DMA()

    [..]  In case of multi-buffer HASH processing (a single digest is computed while
          several buffers are fed to the Peripheral), the user can resort to successive calls
          to :
      (+) Polling mode : HAL_HASH_Accumulate() and wrap-up the digest computation by a call
          to HAL_HASH_AccumulateLast()
      (+) Interrupt mode : HAL_HASH_Accumulate_IT() and wrap-up the digest computation by a call
          to HAL_HASH_AccumulateLast_IT()
      (+) DMA mode : HAL_HASH_Start_DMA(), MDMAT bit must be set through __HAL_HASH_SET_MDMAT() macro,
          before entering the last buffer, reset the MDMAT bit with __HAL_HASH_RESET_MDMAT()
          macro then wrap-up the HASH processing in feeding the last input buffer through the
          same API HAL_HASH_Start_DMA()

@endverbatim
  * @{
  */

/**
  * @brief  HASH peripheral processes in polling mode pInBuffer then reads the computed digest.
  * @param  hhash HASH handle.
  * @param  pInBuffer pointer to the input buffer (buffer to be hashed).
  * @param  Size length of the input buffer in bytes.
  * @param  pOutBuffer pointer to the computed digest.
  * @param  Timeout specify timeout value
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HASH_Start(HASH_HandleTypeDef *hhash, const uint8_t *const pInBuffer, uint32_t Size,
                                 uint8_t *const pOutBuffer, uint32_t Timeout)
{
  /* Check the hash handle allocation */
  if (hhash == NULL)
  {
    return HAL_ERROR;
  }

  /* Check if peripheral is ready to start process */
  if (hhash->State == HAL_HASH_STATE_READY)
  {
    /* Process Locked */
    __HAL_LOCK(hhash);

    /* Change the HASH state */
    hhash->State = HAL_HASH_STATE_BUSY;

    /* Reset HashInCount and Initialize Size, pHashInBuffPtr and pHashOutBuffPtr parameters */
    hhash->pHashInBuffPtr = pInBuffer;
    hhash->pHashOutBuffPtr = pOutBuffer;
    hhash->HashInCount =  0U;
    hhash->Size = Size;

    /* Set HASH mode */
    CLEAR_BIT(hhash->Instance->CR, HASH_CR_MODE);
    /* Reset the HASH processor core */
    MODIFY_REG(hhash->Instance->CR, HASH_CR_INIT, HASH_CR_INIT);

    /* Configure the number of valid bits in last word of the message */
    MODIFY_REG(hhash->Instance->STR, HASH_STR_NBLW, 8U * (Size % 4U));

    /* Set the phase */
    hhash->Phase = HAL_HASH_PHASE_PROCESS;

    HASH_WriteData(hhash, pInBuffer, Size);

    /* Start the message padding then the Digest calculation */
    SET_BIT(hhash->Instance->STR, HASH_STR_DCAL);

    /* Wait for digest calculation completion status(DCIS) flag to be set */
    if (HASH_WaitOnFlagUntilTimeout(hhash, HASH_FLAG_DCIS, RESET, Timeout) != HAL_OK)
    {
      return HAL_ERROR;
    }

    /* Read the message digest */
    HASH_GetDigest(hhash, pOutBuffer, HASH_DIGEST_LENGTH(hhash));

    /* Change the HASH state */
    hhash->State = HAL_HASH_STATE_READY;

    /* Reset HASH state machine */
    hhash->Phase = HAL_HASH_PHASE_READY;

    /* Process Unlocked */
    __HAL_UNLOCK(hhash);

    /* Return function status */
    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}


/**
  * @brief  HASH peripheral processes in interrupt mode pInBuffer then reads the computed digest.
  * @param  hhash HASH handle.
  * @param  pInBuffer pointer to the input buffer (buffer to be hashed).
  * @param  Size length of the input buffer in bytes.
  * @param  pOutBuffer pointer to the computed digest.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HASH_Start_IT(HASH_HandleTypeDef *hhash, const uint8_t *const pInBuffer, uint32_t Size,
                                    uint8_t *const pOutBuffer)
{
  HAL_StatusTypeDef status;
  HAL_HASH_StateTypeDef temp_state;

  /* Check the hash handle allocation */
  if (hhash == NULL)
  {
    return HAL_ERROR;
  }

  /* Check if peripheral is ready to start process or suspended */
  temp_state = hhash->State;
  if ((temp_state == HAL_HASH_STATE_READY) || (temp_state == HAL_HASH_STATE_SUSPENDED))
  {
    /* Process Locked */
    __HAL_LOCK(hhash);

    if (hhash->State == HAL_HASH_STATE_READY)
    {
      /* Reset HashInCount and Initialize Size, pHashInBuffPtr and pHashOutBuffPtr parameters */
      hhash->HashInCount =  0U;
      hhash->pHashInBuffPtr = pInBuffer;
      hhash->pHashOutBuffPtr = pOutBuffer;
      hhash->Size = Size;

      /* Set HASH mode */
      CLEAR_BIT(hhash->Instance->CR, HASH_CR_MODE);
      /* Reset the HASH processor core */
      MODIFY_REG(hhash->Instance->CR, HASH_CR_INIT, HASH_CR_INIT);

      /* Configure the number of valid bits in last word of the message */
      MODIFY_REG(hhash->Instance->STR, HASH_STR_NBLW, 8U * (Size % 4U));
    }
    /* Change the HASH state */
    hhash->State = HAL_HASH_STATE_BUSY;
    /* Set the phase */
    hhash->Phase = HAL_HASH_PHASE_PROCESS;

    /* Enable the specified HASH interrupt*/
    __HAL_HASH_ENABLE_IT(hhash, HASH_IT_DINI | HASH_IT_DCI);

    status = HASH_WriteData_IT(hhash);
  }
  else
  {
    status =  HAL_BUSY;
  }
  /* Return function status */
  return status;
}

/**
  * @brief  HASH peripheral processes in DMA mode pInBuffer then reads the computed digest.
  * @note   Multi-buffer HASH processing is possible, consecutive calls to HAL_HASH_Start_DMA
  *         (MDMAT bit must be set) can be used to feed several input buffers
  *         back-to-back to the Peripheral that will yield a single
  *         HASH signature once all buffers have been entered. Wrap-up of input
  *         buffers feeding and retrieval of digest is done by a call to
  *         HAL_HASH_Start_DMA (MDMAT bit must be reset).
  * @param  hhash HASH handle.
  * @param  pInBuffer pointer to the input buffer (buffer to be hashed).
  * @param  Size length of the input buffer in bytes (must be a multiple of 4 in
  *         case of Multi-buffer and not last buffer).
  * @param  pOutBuffer pointer to the computed digest.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HASH_Start_DMA(HASH_HandleTypeDef *hhash, const uint8_t *const pInBuffer, uint32_t Size,
                                     uint8_t *const pOutBuffer)
{
  HAL_StatusTypeDef status;
  HAL_HASH_StateTypeDef temp_state;

  /* Check the hash handle allocation */
  if (hhash == NULL)
  {
    return HAL_ERROR;
  }

  /* Check if peripheral is ready to start process or suspended  */
  temp_state = hhash->State;
  if ((temp_state == HAL_HASH_STATE_READY) || (temp_state == HAL_HASH_STATE_SUSPENDED))
  {

    /* Process Locked */
    __HAL_LOCK(hhash);

    /* Check if initialization phase has not been already performed */
    if (hhash->State == HAL_HASH_STATE_READY)
    {
      /* Change the HASH state */
      hhash->State = HAL_HASH_STATE_BUSY;

      /* Reset HashInCount and Initialize Size, pHashInBuffPtr and pHashOutBuffPtr parameters */
      hhash->HashInCount =  0U;
      hhash->pHashInBuffPtr = pInBuffer;
      hhash->pHashOutBuffPtr = pOutBuffer;
      hhash->HashInCount =  0U;
      hhash->Size = Size;

      /* Check if initialization phase has already been performed.
      If Phase is already set to HAL_HASH_PHASE_PROCESS, this means the
      API is processing a new input data message in case of multi-buffer HASH
      computation. */
      if (hhash->Phase == HAL_HASH_PHASE_READY)
      {
        /* Set HASH mode */
        CLEAR_BIT(hhash->Instance->CR, HASH_CR_MODE);
        /* Reset the HASH processor core */
        MODIFY_REG(hhash->Instance->CR, HASH_CR_INIT, HASH_CR_INIT);

        /* Set the phase */
        hhash->Phase = HAL_HASH_PHASE_PROCESS;
      }
      /* Configure the number of valid bits in last word of the message */
      if ((hhash->Instance->CR & HASH_CR_MDMAT) == 0U)
      {
        /* Configure the number of valid bits in last word of the message */
        MODIFY_REG(hhash->Instance->STR, HASH_STR_NBLW, 8U * ((hhash->Size) % 4U));
      }
      else
      {
        /* Configure the number of valid bits in last word of the message */
        MODIFY_REG(hhash->Instance->STR, HASH_STR_NBLW, 0U);
      }

    }
    else /* HAL_HASH_STATE_SUSPENDED */
    {
      /* Change the HASH state */
      hhash->State = HAL_HASH_STATE_BUSY;
      /*only part not yet hashed to compute  */
      hhash->Size = hhash->HashInCount;
    }
    /* Set the HASH DMA transfer complete callback */
    hhash->hdmain->XferCpltCallback = HASH_DMAXferCplt;
    /* Set the DMA error callback */
    hhash->hdmain->XferErrorCallback = HASH_DMAError;

    if ((hhash->hdmain->Mode & DMA_LINKEDLIST) == DMA_LINKEDLIST)
    {
      if ((hhash->hdmain->LinkedListQueue != NULL) && (hhash->hdmain->LinkedListQueue->Head != NULL))
      {
        /* Enable the DMA channel */
        hhash->hdmain->LinkedListQueue->Head->LinkRegisters[NODE_CBR1_DEFAULT_OFFSET]\
          = ((((hhash->Size) % 4U) != 0U) ? ((hhash->Size) + (4U - ((hhash->Size) % 4U))) : (hhash->Size));
        hhash->hdmain->LinkedListQueue->Head->LinkRegisters[NODE_CSAR_DEFAULT_OFFSET]\
          = (uint32_t)(hhash->pHashInBuffPtr);  /* Set DMA source address */
        hhash->hdmain->LinkedListQueue->Head->LinkRegisters[NODE_CDAR_DEFAULT_OFFSET]\
          = (uint32_t)&hhash->Instance->DIN; /* Set DMA destination address */

        status = HAL_DMAEx_List_Start_IT(hhash->hdmain);
      }
      else
      {
        /* Return error status */
        status = HAL_ERROR;
      }
    }
    else
    {
      status = HAL_DMA_Start_IT(hhash->hdmain, (uint32_t)pInBuffer, (uint32_t)&hhash->Instance->DIN, \
                                ((((hhash->Size) % 4U) != 0U) ? ((hhash->Size) + (4U - ((hhash->Size) % 4U))) : \
                                 (hhash->Size)));
    }
    if (status != HAL_OK)
    {
      /* DMA error code field */
      hhash->ErrorCode |= HAL_HASH_ERROR_DMA;

      /* Return error */
#if (USE_HAL_HASH_REGISTER_CALLBACKS == 1U)
      /*Call registered error callback*/
      hhash->ErrorCallback(hhash);
#else
      /*Call legacy weak error callback*/
      HAL_HASH_ErrorCallback(hhash);
#endif /* USE_HAL_HASH_REGISTER_CALLBACKS */
    }
    else
    {
      /* Enable DMA requests */
      SET_BIT(hhash->Instance->CR, HASH_CR_DMAE);
    }
  }
  else
  {
    status =  HAL_BUSY;

  }

  /* Return function status */
  return status;
}


/**
  * @brief  HASH peripheral processes in polling mode several input buffers.
  * @note   Consecutive calls to HAL_HASH_Accumulate() can be used to feed
  *         several input buffers back-to-back to the Peripheral that will yield a single
  *         HASH signature once all buffers have been entered. Wrap-up of input
  *         buffers feeding and retrieval of digest is done by a call to
  *         HAL_HASH_AccumulateLast()
  * @param  hhash HASH handle.
  * @param  pInBuffer pointer to the input buffer (buffer to be hashed).
  * @param  Size length of the input buffer in bytes and a multiple of 4.
  * @param  Timeout specify timeout value
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HASH_Accumulate(HASH_HandleTypeDef *hhash, const uint8_t *const pInBuffer, uint32_t Size,
                                      uint32_t Timeout)
{
  HAL_HASH_StateTypeDef temp_state;

  /* Check the hash handle allocation and buffer length multiple of 4 */
  if ((hhash == NULL) || ((Size % 4U) != 0U))
  {
    return HAL_ERROR;
  }

  /* Check if peripheral is ready to start process or suspended  */
  temp_state = hhash->State;
  if ((temp_state == HAL_HASH_STATE_READY) || (temp_state == HAL_HASH_STATE_SUSPENDED))
  {
    /* Process Locked */
    __HAL_LOCK(hhash);

    /* Change the HASH state */
    hhash->State = HAL_HASH_STATE_BUSY;

    /* Reset HashInCount and Initialize Size, pHashInBuffPtr and pHashOutBuffPtr parameters */
    hhash->pHashInBuffPtr = pInBuffer;
    hhash->HashInCount =  0U;
    hhash->Size = Size;

    if (hhash->Phase == HAL_HASH_PHASE_READY)
    {
      /* Set HASH mode */
      CLEAR_BIT(hhash->Instance->CR, HASH_CR_MODE);
      /* Reset the HASH processor core */
      MODIFY_REG(hhash->Instance->CR, HASH_CR_INIT, HASH_CR_INIT);

      /* Set the phase */
      hhash->Phase = HAL_HASH_PHASE_PROCESS;
    }
    HASH_WriteData(hhash, pInBuffer, Size);

    /* Wait for BUSY flag to be cleared */
    if (HASH_WaitOnFlagUntilTimeout(hhash, HASH_FLAG_BUSY, SET, Timeout) != HAL_OK)
    {
      return HAL_ERROR;
    }
    /* Change the HASH state */
    hhash->State = HAL_HASH_STATE_READY;

    /* Process Unlocked */
    __HAL_UNLOCK(hhash);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}


/**
  * @brief  End computation of a single HASH signature after several calls to HAL_HASH_Accumulate() API.
  * @note   Digest is available in pOutBuffer
  * @param  hhash HASH handle.
  * @param  pInBuffer pointer to the input buffer (buffer to be hashed).
  * @param  Size length of the input buffer in bytes.
  * @param  pOutBuffer pointer to the computed digest.
  * @param  Timeout specify timeout value
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HASH_AccumulateLast(HASH_HandleTypeDef *hhash, const uint8_t *const pInBuffer, uint32_t Size,
                                          uint8_t *const pOutBuffer, uint32_t Timeout)
{
  HAL_HASH_StateTypeDef temp_state;

  /* Check the hash handle allocation */
  if (hhash == NULL)
  {
    return HAL_ERROR;
  }

  /* Check if peripheral is ready to start process or suspended */
  temp_state = hhash->State;
  if ((temp_state == HAL_HASH_STATE_READY) || (temp_state == HAL_HASH_STATE_SUSPENDED))
  {
    /* Process Locked */
    __HAL_LOCK(hhash);

    /* Change the HASH state */
    hhash->State = HAL_HASH_STATE_BUSY;

    /* Reset HashInCount and Initialize Size, pHashInBuffPtr and pHashOutBuffPtr parameters */
    hhash->pHashInBuffPtr = pInBuffer;
    hhash->pHashOutBuffPtr = pOutBuffer;
    hhash->HashInCount =  0U;
    hhash->Size = Size;

    if (hhash->Phase == HAL_HASH_PHASE_READY)
    {
      /* Set HASH mode */
      CLEAR_BIT(hhash->Instance->CR, HASH_CR_MODE);
      /* Reset the HASH processor core */
      MODIFY_REG(hhash->Instance->CR, HASH_CR_INIT, HASH_CR_INIT);

      /* Set the phase */
      hhash->Phase = HAL_HASH_PHASE_PROCESS;
    }
    /* Configure the number of valid bits in last word of the message */
    MODIFY_REG(hhash->Instance->STR, HASH_STR_NBLW, 8U * (Size % 4U));


    HASH_WriteData(hhash, pInBuffer, Size);

    /* Start the message padding then the Digest calculation */
    SET_BIT(hhash->Instance->STR, HASH_STR_DCAL);

    /* Wait for digest calculation completion status(DCIS) flag to be set */
    if (HASH_WaitOnFlagUntilTimeout(hhash, HASH_FLAG_DCIS, RESET, Timeout) != HAL_OK)
    {
      return HAL_ERROR;
    }
    /* Read the message digest */
    HASH_GetDigest(hhash, pOutBuffer, HASH_DIGEST_LENGTH(hhash));

    /* Change the HASH state */
    hhash->State = HAL_HASH_STATE_READY;

    /* Reset HASH state machine */
    hhash->Phase = HAL_HASH_PHASE_READY;
    hhash->Accumulation = 0;
    /* Process Unlocked */
    __HAL_UNLOCK(hhash);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  HASH peripheral processes in interrupt mode several input buffers.
  * @note   Consecutive calls to HAL_HASH_Accumulate_IT() can be used to feed
  *         several input buffers back-to-back to the Peripheral that will yield a single
  *         HASH signature once all buffers have been entered. Wrap-up of input
  *         buffers feeding and retrieval of digest is done by a call to
  *         HAL_HASH_AccumulateLast_IT()
  * @param  hhash HASH handle.
  * @param  pInBuffer pointer to the input buffer (buffer to be hashed).
  * @param  Size length of the input buffer in bytes and a multiple of 4.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HASH_Accumulate_IT(HASH_HandleTypeDef *hhash, const uint8_t *const pInBuffer, uint32_t Size)
{
  HAL_StatusTypeDef status;

  /* Check the hash handle allocation */
  if ((hhash == NULL) || ((Size % 4U) != 0U))
  {
    return HAL_ERROR;
  }

  /* Check if peripheral is ready to start process */
  if (hhash->State == HAL_HASH_STATE_READY)
  {
    /* Process Locked */
    __HAL_LOCK(hhash);

    /* Change the HASH state */
    hhash->State = HAL_HASH_STATE_BUSY;

    /* Reset HashInCount and Initialize Size and pHashInBuffPtr parameters  */
    hhash->pHashInBuffPtr = pInBuffer;
    hhash->HashInCount =  0U;
    hhash->Size = Size;
    /* Set multi buffers accumulation flag */
    hhash->Accumulation = 1U;

    if (hhash->Phase == HAL_HASH_PHASE_READY)
    {
      /* Set HASH mode */
      CLEAR_BIT(hhash->Instance->CR, HASH_CR_MODE);
      /* Reset the HASH processor core */
      MODIFY_REG(hhash->Instance->CR, HASH_CR_INIT, HASH_CR_INIT);
      /* Set the phase */
      hhash->Phase = HAL_HASH_PHASE_PROCESS;
    }
    /* Enable the specified HASH interrupt*/
    __HAL_HASH_ENABLE_IT(hhash, HASH_IT_DINI);

    status = HASH_WriteData_IT(hhash);

    /* Change the HASH state */
    hhash->State = HAL_HASH_STATE_READY;

    /* Process Unlocked */
    __HAL_UNLOCK(hhash);
  }
  else
  {
    status = HAL_BUSY;
  }
  /* Return function status */
  return status;
}


/**
  * @brief  End computation of a single HASH signature after several calls to HAL_HASH_Accumulate_IT() API.
  * @note   Digest is available in pOutBuffer
  * @param  hhash HASH handle.
  * @param  pInBuffer pointer to the input buffer (buffer to be hashed).
  * @param  Size length of the input buffer in bytes.
  * @param  pOutBuffer pointer to the computed digest.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HASH_AccumulateLast_IT(HASH_HandleTypeDef *hhash, const uint8_t *const pInBuffer, uint32_t Size,
                                             uint8_t *const pOutBuffer)
{
  HAL_StatusTypeDef status;

  /* Check the hash handle allocation */
  if (hhash == NULL)
  {
    return HAL_ERROR;
  }

  /* Check if peripheral is ready to start process */
  if (hhash->State == HAL_HASH_STATE_READY)
  {
    /* Process Locked */
    __HAL_LOCK(hhash);

    /* Change the HASH state */
    hhash->State = HAL_HASH_STATE_BUSY;

    /* Reset HashInCount and Initialize Size, pHashInBuffPtr and pHashOutBuffPtr parameters */
    hhash->pHashInBuffPtr = pInBuffer;
    hhash->pHashOutBuffPtr = pOutBuffer;
    hhash->HashInCount =  0U;
    hhash->Size = Size;
    if (hhash->Phase == HAL_HASH_PHASE_READY)
    {
      /* Set HASH mode */
      CLEAR_BIT(hhash->Instance->CR, HASH_CR_MODE);
      /* Reset the HASH processor core */
      MODIFY_REG(hhash->Instance->CR, HASH_CR_INIT, HASH_CR_INIT);

      /* Set the phase */
      hhash->Phase = HAL_HASH_PHASE_PROCESS;
    }
    /* Configure the number of valid bits in last word of the message */
    MODIFY_REG(hhash->Instance->STR, HASH_STR_NBLW, 8U * (Size % 4U));

    /* Enable the specified HASH interrupt*/
    __HAL_HASH_ENABLE_IT(hhash, HASH_IT_DINI | HASH_IT_DCI);

    status = HASH_WriteData_IT(hhash);

    /* Change the HASH state */
    hhash->State = HAL_HASH_STATE_READY;

    /* Reset HASH state machine */
    hhash->Phase = HAL_HASH_PHASE_READY;

    /* Process Unlocked */
    __HAL_UNLOCK(hhash);
  }
  else
  {
    status =  HAL_BUSY;
  }
  /* Return function status */
  return status;
}

/**
  * @}
  */


/** @defgroup HASH_Exported_Functions_Group3 HMAC processing functions
  *  @brief   HMAC processing functions using different mode.
  *
@verbatim
 ===============================================================================
                 ##### HMAC processing functions #####
 ===============================================================================
    [..]  This section provides API allowing to calculate the HMAC (keyed-hash
          message authentication code) value using:
      (+) one of the algorithms supported by the peripheral
      (+) Key selection
         (++) Long key : HMAC key is longer than the block size
         (++) Short key : HMAC key is shorter or equal to the block size

    [..] To calculate the HMAC for a single buffer, user can resort to one of three processing
         functions available .
      (+) Polling mode : HAL_HASH_HMAC_Start()
      (+) Interrupt mode : HAL_HASH_HMAC_Start_IT()
      (+) DMA mode : HAL_HASH_HMAC_Start_DMA()

    [..]  In case of multi-buffer HMAC processing (a single digest is computed while
          several buffers are fed to the Peripheral), the user can resort to successive calls
          to :
      (+) Polling mode : HAL_HASH_HMAC_Accumulate() and wrap-up the digest computation by a call
          to HAL_HASH_HMAC_AccumulateLast()
      (+) Interrupt mode : HAL_HASH_HMAC_Accumulate_IT() and wrap-up the digest computation by a call
          to HAL_HASH_HMAC_AccumulateLast_IT()
      (+) DMA mode : HAL_HASH_HMAC_Start_DMA(),MDMAT bit must be set through __HAL_HASH_SET_MDMAT() macro,
          before entering the last buffer, reset the MDMAT bit with __HAL_HASH_RESET_MDMAT()
          macro then wrap-up the HMAC processing in feeding the last input buffer through the
          same API HAL_HASH_HMAC_Start_DMA()

@endverbatim
  * @{
  */

/**
  * @brief  HMAC in polling mode, HASH peripheral processes Key then pInBuffer then reads the computed digest.
  * @param  hhash HASH handle.
  * @param  pInBuffer pointer to the input buffer (buffer to be hashed).
  * @param  Size length of the input buffer in bytes.
  * @param  pOutBuffer pointer to the computed digest.
  * @param  Timeout specify timeout value
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HASH_HMAC_Start(HASH_HandleTypeDef *hhash, const uint8_t *const pInBuffer, uint32_t Size,
                                      uint8_t *const pOutBuffer, uint32_t Timeout)
{
  uint32_t blocksize; /* Block size in bytes */

  /* Check the hash handle allocation */
  if (hhash == NULL)
  {
    return HAL_ERROR;
  }

  /* Check if peripheral is ready to start process */
  if (hhash->State == HAL_HASH_STATE_READY)
  {

    /* Process Locked */
    __HAL_LOCK(hhash);

    /* Change the HASH state */
    hhash->State = HAL_HASH_STATE_BUSY;

    /* Reset HASH Phase */
    hhash->Phase = HAL_HASH_PHASE_READY;

    /* Reset HashInCount and Initialize Size, pHashKeyBuffPtr, pHashInBuffPtr and pHashOutBuffPtr parameters */
    hhash->pHashInBuffPtr = pInBuffer;
    hhash->pHashOutBuffPtr = pOutBuffer;
    hhash->pHashKeyBuffPtr =  hhash->Init.pKey;
    hhash->HashInCount =  0U;
    hhash->Size = Size;

    /* Check if key size is larger than block size of the algorithm, accordingly set LKEY and the other setting bits */
    if ((hhash->Init.Algorithm == HASH_ALGOSELECTION_SHA1) ||
        (hhash->Init.Algorithm == HASH_ALGOSELECTION_SHA224) ||
        (hhash->Init.Algorithm == HASH_ALGOSELECTION_SHA256))
    {
      blocksize = BLOCK_64B;
    }
    else
    {
      blocksize = BLOCK_128B;
    }
    if (hhash->Init.KeySize > blocksize)
    {
      MODIFY_REG(hhash->Instance->CR, HASH_CR_LKEY | HASH_CR_MODE | HASH_CR_INIT,
                 HASH_ALGOMODE_HMAC | HASH_LONGKEY | HASH_CR_INIT);
    }
    else
    {

      MODIFY_REG(hhash->Instance->CR, HASH_CR_LKEY | HASH_CR_MODE | HASH_CR_INIT,
                 HASH_ALGOMODE_HMAC | HASH_CR_INIT);
    }

    /* Configure the number of valid bits in last word of the Key */
    MODIFY_REG(hhash->Instance->STR, HASH_STR_NBLW, 8U * ((hhash->Init.KeySize) % 4U));


    /* Set the phase */
    hhash->Phase = HAL_HASH_PHASE_PROCESS;
    /* Write Key */
    HASH_WriteData(hhash, hhash->Init.pKey, hhash->Init.KeySize);

    /* Start the Key padding then the Digest calculation */
    SET_BIT(hhash->Instance->STR, HASH_STR_DCAL);

    /* Wait for BUSY flag to be cleared */
    if (HASH_WaitOnFlagUntilTimeout(hhash, HASH_FLAG_BUSY, SET, Timeout) != HAL_OK)
    {
      return HAL_ERROR;
    }

    /* Configure the number of valid bits in last word of the message */
    MODIFY_REG(hhash->Instance->STR, HASH_STR_NBLW, 8U * (Size % 4U));

    /* Write message */
    HASH_WriteData(hhash, pInBuffer, Size);

    /* Start the message padding then the Digest calculation */
    SET_BIT(hhash->Instance->STR, HASH_STR_DCAL);

    /* Wait for BUSY flag to be cleared */
    if (HASH_WaitOnFlagUntilTimeout(hhash, HASH_FLAG_BUSY, SET, Timeout) != HAL_OK)
    {
      return HAL_ERROR;
    }
    /* Configure the number of valid bits in last word of the Key */
    MODIFY_REG(hhash->Instance->STR, HASH_STR_NBLW, 8U * ((hhash->Init.KeySize) % 4U));

    /* Write Key */
    HASH_WriteData(hhash, hhash->Init.pKey, hhash->Init.KeySize);

    /* Start the Key padding then the Digest calculation */
    SET_BIT(hhash->Instance->STR, HASH_STR_DCAL);

    /* Wait for digest calculation completion status(DCIS) flag to be set */
    if (HASH_WaitOnFlagUntilTimeout(hhash, HASH_FLAG_DCIS, RESET, Timeout) != HAL_OK)
    {
      return HAL_ERROR;
    }

    /* Read the message digest */
    HASH_GetDigest(hhash, pOutBuffer, HASH_DIGEST_LENGTH(hhash));

    /* Change the HASH state */
    hhash->State = HAL_HASH_STATE_READY;

    /* Change the HASH phase  */
    hhash->Phase = HAL_HASH_PHASE_READY;

    /* Process Unlocked */
    __HAL_UNLOCK(hhash);

    /* Return function status */
    return HAL_OK;

  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  HMAC accumulate mode, HASH peripheral processes Key then  several input buffers.
  * @note   Consecutive calls to HAL_HASH_HMAC_Accumulate() can be used to feed
  *         several input buffers back-to-back to the Peripheral that will yield a single
  *         HASH signature once all buffers have been entered. Wrap-up of input
  *         buffers feeding and retrieval of digest is done by a call to
  *         HAL_HASH_HMAC_AccumulateLast()
  * @param  hhash HASH handle.
  * @param  pInBuffer pointer to the input buffer (buffer to be hashed).
  * @param  Size length of the input buffer in bytes and a multiple of 4
  * @param  Timeout specify timeout value
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HASH_HMAC_Accumulate(HASH_HandleTypeDef *hhash, const uint8_t *const pInBuffer, uint32_t Size,
                                           uint32_t Timeout)
{
  uint32_t blocksize; /* Block size in bytes */

  /* Check the hash handle allocation and buffer length multiple of 4 */
  if ((hhash == NULL) || ((Size % 4U) != 0U))
  {
    return HAL_ERROR;
  }

  /* Check if peripheral is ready to start process */
  if (hhash->State == HAL_HASH_STATE_READY)
  {
    /* Process Locked */
    __HAL_LOCK(hhash);

    /* Change the HASH state */
    hhash->State = HAL_HASH_STATE_BUSY;

    /* Initialize Size, pHashInBuffPtr and pHashKeyBuffPtr parameters */
    hhash->pHashInBuffPtr = pInBuffer;
    hhash->pHashKeyBuffPtr =  hhash->Init.pKey;
    hhash->Size = Size;

    if (hhash->Phase == HAL_HASH_PHASE_READY)
    {
      /* Reset HashInCount parameter */
      hhash->HashInCount =  0U;
      /* Check if key size is larger than 64 bytes, accordingly set LKEY and the other setting bits */
      /* Check if key size is larger than block size of the algorithm, accordingly set LKEY and the other setting */
      if ((hhash->Init.Algorithm == HASH_ALGOSELECTION_SHA1) ||
          (hhash->Init.Algorithm == HASH_ALGOSELECTION_SHA224) ||
          (hhash->Init.Algorithm == HASH_ALGOSELECTION_SHA256))
      {
        blocksize = BLOCK_64B;
      }
      else
      {
        blocksize = BLOCK_128B;
      }
      if (hhash->Init.KeySize > blocksize)
      {
        MODIFY_REG(hhash->Instance->CR, HASH_CR_LKEY | HASH_CR_MODE | HASH_CR_INIT,
                   HASH_ALGOMODE_HMAC | HASH_LONGKEY | HASH_CR_INIT);
      }
      else
      {

        MODIFY_REG(hhash->Instance->CR, HASH_CR_LKEY | HASH_CR_MODE | HASH_CR_INIT,
                   HASH_ALGOMODE_HMAC | HASH_CR_INIT);
      }
      /* Set phase process */
      hhash->Phase = HAL_HASH_PHASE_PROCESS;

      /* Configure the number of valid bits in last word of the Key */
      MODIFY_REG(hhash->Instance->STR, HASH_STR_NBLW, 8U * ((hhash->Init.KeySize) % 4U));

      /* Write Key */
      HASH_WriteData(hhash, hhash->Init.pKey, hhash->Init.KeySize);

      /* Start the Key padding then the Digest calculation */
      SET_BIT(hhash->Instance->STR, HASH_STR_DCAL);

      /* Wait for BUSY flag to be cleared */
      if (HASH_WaitOnFlagUntilTimeout(hhash, HASH_FLAG_BUSY, SET, Timeout) != HAL_OK)
      {
        return HAL_ERROR;
      }
    }

    /* Change the number of valid bits in last word of the message */
    MODIFY_REG(hhash->Instance->STR, HASH_STR_NBLW, 0U);

    /* Write message */
    HASH_WriteData(hhash, pInBuffer, Size);

    /* Change the HASH state */
    hhash->State = HAL_HASH_STATE_READY;

    /* Process Unlocked */
    __HAL_UNLOCK(hhash);

    /* Return function status */
    return HAL_OK;

  }
  else
  {
    return HAL_BUSY;
  }
}
/**
  * @brief  End computation of a single HMAC signature after several calls to HAL_HASH_HMAC_Accumulate() API.
  * @note   Digest is available in pOutBuffer
  * @param  hhash HASH handle.
  * @param  pInBuffer pointer to the input buffer (buffer to be hashed).
  * @param  Size length of the input buffer in bytes.
  * @param  pOutBuffer pointer to the computed digest.
  * @param  Timeout specify timeout value
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HASH_HMAC_AccumulateLast(HASH_HandleTypeDef *hhash, const uint8_t *const pInBuffer, uint32_t Size,
                                               uint8_t *const pOutBuffer, uint32_t Timeout)
{
  /* Check the hash handle allocation */
  if (hhash == NULL)
  {
    return HAL_ERROR;
  }

  /* Check if peripheral is ready to start process */
  if (hhash->State == HAL_HASH_STATE_READY)
  {

    /* Process Locked */
    __HAL_LOCK(hhash);

    /* Change the HASH state */
    hhash->State = HAL_HASH_STATE_BUSY;

    /* Initialize Size, pHashInBuffPtr, pHashKeyBuffPtr and pHashOutBuffPtr parameters */
    hhash->pHashInBuffPtr = pInBuffer;
    hhash->pHashOutBuffPtr = pOutBuffer;
    hhash->pHashKeyBuffPtr =  hhash->Init.pKey;
    hhash->Size = Size;

    if (hhash->Phase != HAL_HASH_PHASE_PROCESS)
    {
      return HAL_ERROR;
    }
    else
    {
      /* Configure the number of valid bits in last word of the message */
      MODIFY_REG(hhash->Instance->STR, HASH_STR_NBLW, 8U * (Size % 4U));

      /* Write message */
      HASH_WriteData(hhash, pInBuffer, Size);

      /* Start the message padding then the Digest calculation */
      SET_BIT(hhash->Instance->STR, HASH_STR_DCAL);

      /* Wait for BUSY flag to be cleared */
      if (HASH_WaitOnFlagUntilTimeout(hhash, HASH_FLAG_BUSY, SET, Timeout) != HAL_OK)
      {
        return HAL_ERROR;
      }
      /* Configure the number of valid bits in last word of the Key */
      MODIFY_REG(hhash->Instance->STR, HASH_STR_NBLW, 8U * ((hhash->Init.KeySize) % 4U));

      /* Write Key */
      HASH_WriteData(hhash, hhash->Init.pKey, hhash->Init.KeySize);

      /* Start the Key padding then the Digest calculation */
      SET_BIT(hhash->Instance->STR, HASH_STR_DCAL);

      /* Wait for digest calculation completion status(DCIS) flag to be set */
      if (HASH_WaitOnFlagUntilTimeout(hhash, HASH_FLAG_DCIS, RESET, Timeout) != HAL_OK)
      {
        return HAL_ERROR;
      }

      /* Read the message digest */
      HASH_GetDigest(hhash, pOutBuffer, HASH_DIGEST_LENGTH(hhash));
    }

    /* Change the HASH state */
    hhash->State = HAL_HASH_STATE_READY;

    /* Reset HASH state machine */
    hhash->Phase = HAL_HASH_PHASE_READY;

    /* Process Unlocked */
    __HAL_UNLOCK(hhash);

    /* Return function status */
    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  HMAC in interrupt mode, HASH peripheral process Key then pInBuffer then read the computed digest.
  * @param  hhash HASH handle.
  * @param  pInBuffer pointer to the input buffer (buffer to be hashed).
  * @param  Size length of the input buffer in bytes.
  * @param  pOutBuffer pointer to the computed digest.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HASH_HMAC_Start_IT(HASH_HandleTypeDef *hhash, const uint8_t *const pInBuffer, uint32_t Size,
                                         uint8_t *const pOutBuffer)
{
  HAL_StatusTypeDef status;
  uint32_t blocksize; /* Block size in bytes */

  /* Check the hash handle allocation */
  if (hhash == NULL)
  {
    return HAL_ERROR;
  }

  /* Check if peripheral is ready to start process */
  if (hhash->State == HAL_HASH_STATE_READY)
  {
    /* Process Locked */
    __HAL_LOCK(hhash);

    /* Change the HASH state */
    hhash->State = HAL_HASH_STATE_BUSY;

    /* Reset HASH Phase */
    hhash->Phase = HAL_HASH_PHASE_READY;

    /* Reset HashInCount and Initialize Size, pHashKeyBuffPtr, pHashInBuffPtr and pHashOutBuffPtr parameters */
    hhash->pHashInBuffPtr = pInBuffer;
    hhash->pHashOutBuffPtr = pOutBuffer;
    hhash->pHashKeyBuffPtr =  hhash->Init.pKey;
    hhash->HashInCount =  0U;
    hhash->Size = Size;

    /* Check if key size is larger than block size of the algorithm, accordingly set LKEY and the other setting bits */
    if ((hhash->Init.Algorithm == HASH_ALGOSELECTION_SHA1) ||
        (hhash->Init.Algorithm == HASH_ALGOSELECTION_SHA224) ||
        (hhash->Init.Algorithm == HASH_ALGOSELECTION_SHA256))
    {
      blocksize = BLOCK_64B;
    }
    else
    {
      blocksize = BLOCK_128B;
    }
    if (hhash->Init.KeySize > blocksize)
    {
      MODIFY_REG(hhash->Instance->CR, HASH_CR_LKEY | HASH_CR_MODE | HASH_CR_INIT,
                 HASH_ALGOMODE_HMAC | HASH_LONGKEY | HASH_CR_INIT);
    }
    else
    {

      MODIFY_REG(hhash->Instance->CR, HASH_CR_LKEY | HASH_CR_MODE | HASH_CR_INIT,
                 HASH_ALGOMODE_HMAC | HASH_CR_INIT);
    }

    /* Configure the number of valid bits in last word of the Key */
    MODIFY_REG(hhash->Instance->STR, HASH_STR_NBLW, 8U * ((hhash->Init.KeySize) % 4U));

    /* Set the phase */
    hhash->Phase = HAL_HASH_PHASE_PROCESS;
  }
  else if (hhash->State ==  HAL_HASH_STATE_SUSPENDED)
  {
    /* Process Locked */
    __HAL_LOCK(hhash);
    /* Change the HASH state */
    hhash->State = HAL_HASH_STATE_BUSY;
  }
  else
  {
    return HAL_BUSY;
  }

  /* Enable the specified HASH interrupt*/
  __HAL_HASH_ENABLE_IT(hhash, HASH_IT_DINI | HASH_IT_DCI);

  status = HASH_WriteData_IT(hhash);

  /* Return function status */
  return status;
}

/**
  * @brief  HMAC accumulate in interrupt mode, HASH peripheral processes Key then several input buffers.
  * @note   Consecutive calls to HAL_HASH_HMAC_Accumulate_IT() can be used to feed
  *         several input buffers back-to-back to the Peripheral that will yield a single
  *         HASH signature once all buffers have been entered. Wrap-up of input
  *         buffers feeding and retrieval of digest is done by a call to
  *         HAL_HASH_HMAC_AccumulateLast_IT()
  * @param  hhash HASH handle.
  * @param  pInBuffer pointer to the input buffer (buffer to be hashed).
  * @param  Size length of the input buffer in bytes and a multiple of 4.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HASH_HMAC_Accumulate_IT(HASH_HandleTypeDef *hhash, const uint8_t *const pInBuffer, uint32_t Size)
{
  HAL_StatusTypeDef status;
  uint32_t blocksize; /* Block size in bytes */

  /* Check the hash handle allocation and buffer length multiple of 4 */
  if ((hhash == NULL) || ((Size % 4U) != 0U))
  {
    return HAL_ERROR;
  }

  /* Check if peripheral is ready to start process */
  if (hhash->State == HAL_HASH_STATE_READY)
  {
    /* Process Locked */
    __HAL_LOCK(hhash);

    /* Change the HASH state */
    hhash->State = HAL_HASH_STATE_BUSY;

    /* Reset HashInCount and Initialize Size, pHashInBuffPtr and pHashOutBuffPtr parameters */
    hhash->pHashInBuffPtr = pInBuffer;
    hhash->pHashKeyBuffPtr =  hhash->Init.pKey;
    hhash->HashInCount =  0U;
    hhash->Size = Size;
    /* Set multi buffers accumulation flag */
    hhash->Accumulation = 1U;

    if (hhash->Phase == HAL_HASH_PHASE_READY)
    {
      /* Check if key size is larger than block size of the algorithm, accordingly set LKEY and the other setting */
      if ((hhash->Init.Algorithm == HASH_ALGOSELECTION_SHA1) ||
          (hhash->Init.Algorithm == HASH_ALGOSELECTION_SHA224) ||
          (hhash->Init.Algorithm == HASH_ALGOSELECTION_SHA256))
      {
        blocksize = BLOCK_64B;
      }
      else
      {
        blocksize = BLOCK_128B;
      }
      if (hhash->Init.KeySize > blocksize)
      {
        MODIFY_REG(hhash->Instance->CR, HASH_CR_LKEY | HASH_CR_MODE | HASH_CR_INIT,
                   HASH_ALGOMODE_HMAC | HASH_LONGKEY | HASH_CR_INIT);
      }
      else
      {

        MODIFY_REG(hhash->Instance->CR, HASH_CR_LKEY | HASH_CR_MODE | HASH_CR_INIT,
                   HASH_ALGOMODE_HMAC | HASH_CR_INIT);
      }

      /* Configure the number of valid bits in last word of the Key */
      MODIFY_REG(hhash->Instance->STR, HASH_STR_NBLW, 8U * ((hhash->Init.KeySize) % 4U));

      /* Set the phase */
      hhash->Phase = HAL_HASH_PHASE_PROCESS;
    }
    /* Enable the specified HASH interrupt*/
    __HAL_HASH_ENABLE_IT(hhash, HASH_IT_DINI | HASH_IT_DCI);

    status = HASH_WriteData_IT(hhash);
  }
  else
  {
    status =  HAL_BUSY;
  }
  /* Return function status */
  return status;
}
/**
  * @brief  End computation of a single HMAC signature in interrupt mode, after
  *         several calls to HAL_HASH_HMAC_Accumulate() API.
  * @note   Digest is available in pOutBuffer
  * @param  hhash HASH handle.
  * @param  pInBuffer pointer to the input buffer (buffer to be hashed).
  * @param  Size length of the input buffer in bytes.
  * @param  pOutBuffer pointer to the computed digest.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HASH_HMAC_AccumulateLast_IT(HASH_HandleTypeDef *hhash, const uint8_t *const pInBuffer,
                                                  uint32_t Size, uint8_t *const pOutBuffer)
{
  HAL_StatusTypeDef status;

  /* Check the hash handle allocation */
  if (hhash == NULL)
  {
    return HAL_ERROR;
  }

  /* Check if peripheral is ready to start process*/
  if (hhash->State == HAL_HASH_STATE_READY)
  {
    /* Process Locked */
    __HAL_LOCK(hhash);

    /* Change the HASH state */
    hhash->State = HAL_HASH_STATE_BUSY;

    /* Reset HashInCount and Initialize Size, pHashInBuffPtr and pHashOutBuffPtr parameters */
    hhash->pHashInBuffPtr = pInBuffer;
    hhash->pHashOutBuffPtr = pOutBuffer;
    hhash->pHashKeyBuffPtr =  hhash->Init.pKey;
    hhash->HashInCount =  0U;
    hhash->Size = Size;
    /* Set multi buffers accumulation flag */
    hhash->Accumulation = 0U;
    /* Enable the specified HASH interrupt*/
    __HAL_HASH_ENABLE_IT(hhash, HASH_IT_DINI | HASH_IT_DCI);

    status = HASH_WriteData_IT(hhash);
  }
  else
  {
    status =  HAL_BUSY;
  }
  /* Return function status */
  return status;
}

/**
  * @brief  HMAC in DMA mode,HASH peripheral processes Key then pInBuffer in DMA mode
  *         then read the computed digest.
  * @note   Multi-buffer HMAC processing is possible, consecutive calls to HAL_HASH_HMAC_Start_DMA
  *         (MDMAT bit must be set) can be used to feed several input buffers
  *         back-to-back to the Peripheral that will yield a single
  *         HASH signature once all buffers have been entered. Wrap-up of input
  *         buffers feeding and retrieval of digest is done by a call to
  *         HAL_HASH_HMAC_Start_DMA (MDMAT bit must be reset).
  * @param  hhash HASH handle.
  * @param  pInBuffer pointer to the input buffer (buffer to be hashed).
  * @param  Size length of the input buffer in bytes.
  * @param  pOutBuffer pointer to the computed digest.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HASH_HMAC_Start_DMA(HASH_HandleTypeDef *hhash, const uint8_t *const pInBuffer, uint32_t Size,
                                          uint8_t *const pOutBuffer)
{
  HAL_StatusTypeDef status;
  uint32_t count;
  uint32_t blocksize; /* Block size in bytes */

  /* Check the hash handle allocation */
  if (hhash == NULL)
  {
    return HAL_ERROR;
  }

  /* Check if peripheral is ready to start process*/
  if (hhash->State == HAL_HASH_STATE_READY)
  {
    /* Process Locked */
    __HAL_LOCK(hhash);

    /* Change the HASH state */
    hhash->State = HAL_HASH_STATE_BUSY;

    /* Reset HashInCount and Initialize Size, pHashInBuffPtr and pHashOutBuffPtr parameters */
    hhash->pHashInBuffPtr = pInBuffer;
    hhash->pHashOutBuffPtr = pOutBuffer;
    hhash->pHashKeyBuffPtr =  hhash->Init.pKey;
    hhash->HashInCount =  0U;
    hhash->Size = Size;

    /* Set the phase */
    if (hhash->Phase == HAL_HASH_PHASE_READY)
    {
      /* Check if key size is larger than block size of the algorithm, accordingly set LKEY and the other setting */
      if ((hhash->Init.Algorithm == HASH_ALGOSELECTION_SHA1) ||
          (hhash->Init.Algorithm == HASH_ALGOSELECTION_SHA224) ||
          (hhash->Init.Algorithm == HASH_ALGOSELECTION_SHA256))
      {
        blocksize = BLOCK_64B;
      }
      else
      {
        blocksize = BLOCK_128B;
      }
      if (hhash->Init.KeySize > blocksize)
      {
        MODIFY_REG(hhash->Instance->CR, HASH_CR_LKEY | HASH_CR_MODE | HASH_CR_INIT,
                   HASH_ALGOMODE_HMAC | HASH_LONGKEY | HASH_CR_INIT);
      }
      else
      {

        MODIFY_REG(hhash->Instance->CR, HASH_CR_LKEY | HASH_CR_MODE | HASH_CR_INIT,
                   HASH_ALGOMODE_HMAC | HASH_CR_INIT);
      }

      /* Set the phase */
      hhash->Phase = HAL_HASH_PHASE_HMAC_STEP_1;

      /* Configure the number of valid bits in last word of the Key */
      MODIFY_REG(hhash->Instance->STR, HASH_STR_NBLW, 8U * ((hhash->Init.KeySize) % 4U));

      /* Write Key */
      HASH_WriteData(hhash, hhash->Init.pKey, hhash->Init.KeySize);

      /* Start the Key padding then the Digest calculation */
      SET_BIT(hhash->Instance->STR, HASH_STR_DCAL);

      /* Wait for DCIS flag to be set */
      count = HASH_TIMEOUTVALUE;
      do
      {
        count--;
        if (count == 0U)
        {
          /* Change state */
          hhash->ErrorCode |= HAL_HASH_ERROR_TIMEOUT;
          hhash->State = HAL_HASH_STATE_READY;
          __HAL_UNLOCK(hhash);
          return HAL_ERROR;
        }
      } while (HAL_IS_BIT_CLR(hhash->Instance->SR, HASH_FLAG_BUSY));
    }

    hhash->Phase = HAL_HASH_PHASE_HMAC_STEP_2;
    if ((hhash->Instance->CR & HASH_CR_MDMAT) == 0U)
    {
      /* Configure the number of valid bits in last word of the message */
      MODIFY_REG(hhash->Instance->STR, HASH_STR_NBLW, 8U * ((hhash->Size) % 4U));
    }
    else
    {
      /* Configure the number of valid bits in last word of the message */
      MODIFY_REG(hhash->Instance->STR, HASH_STR_NBLW, 0U);
    }
  }
  else if (hhash->State ==  HAL_HASH_STATE_SUSPENDED)
  {
    /* Process Locked */
    __HAL_LOCK(hhash);
    /* Change the HASH state */
    hhash->State = HAL_HASH_STATE_BUSY;

    /*only part not yet hashed to compute  */
    hhash->Size = hhash->HashInCount;
  }

  else
  {
    /* Return busy status */
    return HAL_BUSY;
  }

  /* Set the HASH DMA transfer complete callback */
  hhash->hdmain->XferCpltCallback = HASH_DMAXferCplt;
  /* Set the DMA error callback */
  hhash->hdmain->XferErrorCallback = HASH_DMAError;

  if ((hhash->hdmain->Mode & DMA_LINKEDLIST) == DMA_LINKEDLIST)
  {
    if ((hhash->hdmain->LinkedListQueue != NULL) && (hhash->hdmain->LinkedListQueue->Head != NULL))
    {
      /* Enable the DMA channel */
      hhash->hdmain->LinkedListQueue->Head->LinkRegisters[NODE_CBR1_DEFAULT_OFFSET]\
        = ((((hhash->Size) % 4U) != 0U) ? ((hhash->Size) + (4U - ((hhash->Size) % 4U))) : ((hhash->Size)));
      hhash->hdmain->LinkedListQueue->Head->LinkRegisters[NODE_CSAR_DEFAULT_OFFSET]\
        = (uint32_t)(hhash->pHashInBuffPtr);  /* Set DMA source address */
      hhash->hdmain->LinkedListQueue->Head->LinkRegisters[NODE_CDAR_DEFAULT_OFFSET]\
        = (uint32_t)&hhash->Instance->DIN; /* Set DMA destination address */

      status = HAL_DMAEx_List_Start_IT(hhash->hdmain);
    }
    else
    {
      /* Return error status */
      status = HAL_ERROR;
    }
  }
  else
  {
    status = HAL_DMA_Start_IT(hhash->hdmain, (uint32_t)(hhash->pHashInBuffPtr), (uint32_t)&hhash->Instance->DIN, \
                              ((((hhash->Size) % 4U) != 0U) ? ((hhash->Size) + (4U - ((hhash->Size) % 4U))) : \
                               ((hhash->Size))));
  }
  if (status != HAL_OK)
  {
    /* DMA error code field */
    hhash->ErrorCode |= HAL_HASH_ERROR_DMA;

    /* Return error */
#if (USE_HAL_HASH_REGISTER_CALLBACKS == 1U)
    /*Call registered error callback*/
    hhash->ErrorCallback(hhash);
#else
    /*Call legacy weak error callback*/
    HAL_HASH_ErrorCallback(hhash);
#endif /* USE_HAL_HASH_REGISTER_CALLBACKS */
  }
  else
  {
    /* Enable DMA requests */
    SET_BIT(hhash->Instance->CR, HASH_CR_DMAE);
  }

  /* Return function status */
  return status;
}


/**
  * @}
  */

/** @defgroup HASH_Exported_Functions_Group4 HASH IRQ handler management
  * @brief    HASH IRQ handler.
  *
@verbatim
  ==============================================================================
                ##### HASH IRQ handler management #####
  ==============================================================================
[..]  This section provides HASH IRQ handler and callback functions.
      (+) HAL_HASH_IRQHandler HASH interrupt request
      (+) HAL_HASH_InCpltCallback input data transfer complete callback
      (+) HAL_HASH_DgstCpltCallback digest computation complete callback
      (+) HAL_HASH_ErrorCallback  HASH error callback
      (+) HAL_HASH_GetState return the HASH state
      (+) HAL_HASH_GetError return the HASH error code
@endverbatim
  * @{
  */

/**
  * @brief Handle HASH interrupt request.
  * @param hhash HASH handle.
  * @note  HAL_HASH_IRQHandler() handles interrupts in HMAC processing as well.
  * @retval None
  */
void HAL_HASH_IRQHandler(HASH_HandleTypeDef *hhash)
{
  HAL_StatusTypeDef status;
  uint32_t itsource = hhash->Instance->IMR;
  uint32_t itflag   = hhash->Instance->SR;

  /* If digest is ready */
  if ((itflag & HASH_FLAG_DCIS) == HASH_FLAG_DCIS)
  {
    /* Read the digest */
    HASH_GetDigest(hhash, hhash->pHashOutBuffPtr, HASH_DIGEST_LENGTH(hhash));

    /* Disable Interrupts */
    __HAL_HASH_DISABLE_IT(hhash, HASH_IT_DINI | HASH_IT_DCI);
    /* Change the HASH state */
    hhash->State = HAL_HASH_STATE_READY;
    /* Reset HASH state machine */
    hhash->Phase = HAL_HASH_PHASE_READY;
    /* Process Unlocked */
    __HAL_UNLOCK(hhash);
    /* Call digest computation complete call back */
#if (USE_HAL_HASH_REGISTER_CALLBACKS == 1)
    hhash->DgstCpltCallback(hhash);
#else
    HAL_HASH_DgstCpltCallback(hhash);
#endif /* USE_HAL_HASH_REGISTER_CALLBACKS */

  }
  /* If Peripheral ready to accept new data */
  if ((itflag & HASH_FLAG_DINIS) == HASH_FLAG_DINIS)
  {
    if ((itsource & HASH_IT_DINI) == HASH_IT_DINI)
    {
      status = HASH_WriteData_IT(hhash);
      if (status != HAL_OK)
      {
        /* Call error callback */
#if (USE_HAL_HASH_REGISTER_CALLBACKS == 1)
        hhash->ErrorCallback(hhash);
#else
        HAL_HASH_ErrorCallback(hhash);
#endif /* USE_HAL_HASH_REGISTER_CALLBACKS */
      }
    }
  }
}

/**
  * @brief  Input data transfer complete call back.
  * @note   HAL_HASH_InCpltCallback() is called when the complete input message
  *         has been fed to the Peripheral. This API is invoked only when input data are
  *         entered under interruption or through DMA.
  * @note   In case of HASH or HMAC multi-buffer DMA feeding case (MDMAT bit set),
  *         HAL_HASH_InCpltCallback() is called at the end of each buffer feeding
  *         to the Peripheral.
  * @param  hhash pointer to a HASH_HandleTypeDef structure that contains
  *         the configuration information for HASH module.
  * @retval None
  */
__weak void HAL_HASH_InCpltCallback(HASH_HandleTypeDef *hhash)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hhash);

  /* NOTE : This function should not be modified; when the callback is needed,
            HAL_HASH_InCpltCallback() can be implemented in the user file.
   */
}

/**
  * @brief  Digest computation complete call back.
  * @note   HAL_HASH_DgstCpltCallback() is used under interruption, is not
  *         relevant with DMA.
  * @param  hhash pointer to a HASH_HandleTypeDef structure that contains
  *         the configuration information for HASH module.
  * @retval None
  */
__weak void HAL_HASH_DgstCpltCallback(HASH_HandleTypeDef *hhash)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hhash);

  /* NOTE : This function should not be modified; when the callback is needed,
            HAL_HASH_DgstCpltCallback() can be implemented in the user file.
   */
}

/**
  * @brief  HASH error callback.
  * @note   Code user can resort to hhash->Status (HAL_ERROR, HAL_TIMEOUT,...)
  *         to retrieve the error type.
  * @param  hhash pointer to a HASH_HandleTypeDef structure that contains
  *         the configuration information for HASH module.
  * @retval None
  */
__weak void HAL_HASH_ErrorCallback(HASH_HandleTypeDef *hhash)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hhash);

  /* NOTE : This function should not be modified; when the callback is needed,
            HAL_HASH_ErrorCallback() can be implemented in the user file.
   */
}

/**
  * @brief  Return the HASH handle state.
  * @note   The API yields the current state of the handle (BUSY, READY,...).
  * @param  hhash HASH handle.
  * @retval HAL HASH state
  */
HAL_HASH_StateTypeDef HAL_HASH_GetState(const HASH_HandleTypeDef *hhash)
{
  return hhash->State;
}

/**
  * @brief  Return the HASH handle error code.
  * @param  hhash pointer to a HASH_HandleTypeDef structure.
  * @retval HASH Error Code
  */
uint32_t HAL_HASH_GetError(const HASH_HandleTypeDef *hhash)
{
  /* Return HASH Error Code */
  return hhash->ErrorCode;
}
/**
  * @}
  */

/**
  * @}
  */

/* Private functions ---------------------------------------------------------*/
/** @addtogroup HASH_Private_Functions
  * @{
  */

/**
  * @brief DMA HASH Input Data transfer completion callback.
  * @param hdma DMA handle.
  * @retval None
  */
static void HASH_DMAXferCplt(DMA_HandleTypeDef *hdma)
{
  HASH_HandleTypeDef *hhash = (HASH_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;
  uint32_t count;

  if (READ_BIT(hhash->Instance->CR, HASH_CR_MODE) == 0U)
  {
    if ((hhash->Instance->CR & HASH_CR_MDMAT) == 0U)
    {
      /* Disable the DMA transfer */
      CLEAR_BIT(hhash->Instance->CR, HASH_CR_DMAE);


      /* Wait for DCIS flag to be set */
      count = HASH_TIMEOUTVALUE;
      do
      {
        count--;
        if (count == 0U)
        {
          /* Change state */
          hhash->ErrorCode |= HAL_HASH_ERROR_TIMEOUT;
          hhash->State = HAL_HASH_STATE_READY;
          __HAL_UNLOCK(hhash);
#if (USE_HAL_HASH_REGISTER_CALLBACKS == 1)
          hhash->ErrorCallback(hhash);
#else
          HAL_HASH_ErrorCallback(hhash);
#endif /* USE_HAL_HASH_REGISTER_CALLBACKS */
        }
      } while (HAL_IS_BIT_CLR(hhash->Instance->SR, HASH_FLAG_DCIS));
      /* Call Input data transfer complete call back */
#if (USE_HAL_HASH_REGISTER_CALLBACKS == 1)
      hhash->InCpltCallback(hhash);
#else
      HAL_HASH_InCpltCallback(hhash);
#endif /* USE_HAL_HASH_REGISTER_CALLBACKS */

      /* Read the message digest */
      HASH_GetDigest(hhash, hhash->pHashOutBuffPtr, HASH_DIGEST_LENGTH(hhash));

      /* Change the HASH state to ready */
      hhash->State = HAL_HASH_STATE_READY;

      /* Reset HASH state machine */
      hhash->Phase = HAL_HASH_PHASE_READY;

      /* Process UnLock */
      __HAL_UNLOCK(hhash);

      /* Call digest complete call back */
#if (USE_HAL_HASH_REGISTER_CALLBACKS == 1)
      hhash->DgstCpltCallback(hhash);
#else
      HAL_HASH_DgstCpltCallback(hhash);
#endif /* USE_HAL_HASH_REGISTER_CALLBACKS */
    }
    else
    {
      hhash->State = HAL_HASH_STATE_READY;
      __HAL_UNLOCK(hhash);
    }
  }
  else /*HMAC DMA*/
  {
    if (hhash->Phase == HAL_HASH_PHASE_HMAC_STEP_2)
    {
      if ((hhash->Instance->CR & HASH_CR_MDMAT) == 0U)
      {
        /* Set the phase */
        hhash->Phase = HAL_HASH_PHASE_HMAC_STEP_3;
        /* Configure the number of valid bits in last word of the Key */
        MODIFY_REG(hhash->Instance->STR, HASH_STR_NBLW, 8U * ((hhash->Init.KeySize) % 4U));
        /* Write Key */
        HASH_WriteData(hhash, hhash->Init.pKey, hhash->Init.KeySize);

        /* Start the Key padding then the Digest calculation */
        SET_BIT(hhash->Instance->STR, HASH_STR_DCAL);

        /* Wait for DCIS flag to be set */
        count = HASH_TIMEOUTVALUE;
        do
        {
          count--;
          if (count == 0U)
          {
            /* Disable the DMA transfer */
            CLEAR_BIT(hhash->Instance->CR, HASH_CR_DMAE);

            /* Change state */
            hhash->ErrorCode |= HAL_HASH_ERROR_DMA;
            hhash->State = HAL_HASH_STATE_READY;
            __HAL_UNLOCK(hhash);
#if (USE_HAL_HASH_REGISTER_CALLBACKS == 1)
            hhash->ErrorCallback(hhash);
#else
            HAL_HASH_ErrorCallback(hhash);
#endif /* USE_HAL_HASH_REGISTER_CALLBACKS */
          }
        } while (HAL_IS_BIT_CLR(hhash->Instance->SR, HASH_FLAG_DCIS));

        /* Read the message digest */
        HASH_GetDigest(hhash, hhash->pHashOutBuffPtr, HASH_DIGEST_LENGTH(hhash));

        /* Change the HASH state to ready */
        hhash->State = HAL_HASH_STATE_READY;

        /* Reset HASH state machine */
        hhash->Phase = HAL_HASH_PHASE_READY;

        /* Process UnLock */
        __HAL_UNLOCK(hhash);

        /* Call digest complete call back */
#if (USE_HAL_HASH_REGISTER_CALLBACKS == 1)
        hhash->DgstCpltCallback(hhash);
#else
        HAL_HASH_DgstCpltCallback(hhash);
#endif /* USE_HAL_HASH_REGISTER_CALLBACKS */

      }
      else
      {
        hhash->State = HAL_HASH_STATE_READY;
        __HAL_UNLOCK(hhash);
        hhash->Accumulation = 1;
      }
    }
  }
}

/**
  * @brief DMA HASH communication error callback.
  * @param hdma DMA handle.
  * @retval None
  */
static void HASH_DMAError(DMA_HandleTypeDef *hdma)
{
  HASH_HandleTypeDef *hhash = (HASH_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;

  hhash->ErrorCode |= HAL_HASH_ERROR_DMA;
  /* Set HASH state to ready to prevent any blocking issue in user code
  present in HAL_HASH_ErrorCallback() */
  hhash->State = HAL_HASH_STATE_READY;

#if (USE_HAL_HASH_REGISTER_CALLBACKS == 1)
  hhash->ErrorCallback(hhash);
#else
  HAL_HASH_ErrorCallback(hhash);
#endif /* USE_HAL_HASH_REGISTER_CALLBACKS */
}

/**
  * @brief  Feed the input buffer to the HASH peripheral in polling.
  * @param  hhash HASH handle.
  * @param  pInBuffer pointer to input buffer.
  * @param  Size the size of input buffer in bytes.
  * @retval HAL status
  */
static void HASH_WriteData(HASH_HandleTypeDef *hhash, const uint8_t *pInBuffer, uint32_t Size)
{
  uint32_t buffercounter;
  __IO uint32_t inputaddr = (uint32_t) pInBuffer;
  uint8_t tmp1;
  uint8_t tmp2;
  uint8_t tmp3;

  for (buffercounter = 0U; buffercounter < (Size / 4U) ; buffercounter++)
  {
    /* Write input data 4 bytes at a time */
    hhash->Instance->DIN = *(uint32_t *)inputaddr;
    inputaddr += 4U;
    hhash->HashInCount += 4U;
  }

  if ((Size % 4U) != 0U)
  {
    if (hhash->Init.DataType == HASH_HALFWORD_SWAP)
    {
      /* Write remaining input data */
      if ((Size % 4U) <= 2U)
      {
        hhash->Instance->DIN = (uint32_t) * (uint16_t *)inputaddr;
      }
      if ((Size % 4U) == 3U)
      {
        hhash->Instance->DIN = *(uint32_t *)inputaddr;
      }
    }
    else if ((hhash->Init.DataType == HASH_BYTE_SWAP)
             || (hhash->Init.DataType == HASH_BIT_SWAP))  /* byte swap or bit swap or */
    {
      /* Write remaining input data */
      if ((Size % 4U) == 1U)
      {
        hhash->Instance->DIN = (uint32_t) * (uint8_t *)inputaddr;
      }
      if ((Size % 4U) == 2U)
      {
        hhash->Instance->DIN = (uint32_t) * (uint16_t *)inputaddr;
      }
      if ((Size % 4U) == 3U)
      {
        tmp1 = *(uint8_t *)inputaddr;
        tmp2 = *(((uint8_t *)inputaddr) + 1U);
        tmp3 = *(((uint8_t *)inputaddr) + 2U);
        hhash->Instance->DIN = ((uint32_t)tmp1) | ((uint32_t)tmp2 << 8U) | ((uint32_t)tmp3 << 16U);
      }
    }
    else
    {
      hhash->Instance->DIN = *(uint32_t *)inputaddr;
    }
    hhash->HashInCount += 4U;
  }
}

/**
  * @brief  Feed the input buffer to the HASH peripheral in interruption mode.
  * @param  hhash HASH handle.
  * @retval HAL status
  */
static HAL_StatusTypeDef HASH_WriteData_IT(HASH_HandleTypeDef *hhash)
{
  uint32_t buffercounter;
  uint32_t count;
  __IO uint32_t keyaddr = (uint32_t)(hhash->pHashKeyBuffPtr);
  __IO uint32_t inputaddr = (uint32_t)(hhash->pHashInBuffPtr);
  uint32_t nbbytePartialHash  = (((hhash->Instance->SR) >> 16U) * 4U); /* Nb byte  to enter in HASH fifo to trig
                                                                      a partial HASH computation*/

  if (hhash->State == HAL_HASH_STATE_BUSY)
  {
    if ((hhash->Instance->CR & HASH_CR_MODE) == 0U)
    {
#if (USE_HAL_HASH_SUSPEND_RESUME == 1U)
      /* If suspension flag has been raised, suspend processing */
      if (hhash->SuspendRequest == HAL_HASH_SUSPEND)
      {
        /* reset SuspendRequest */
        hhash->SuspendRequest = HAL_HASH_SUSPEND_NONE;
        /* Disable Computation Complete Flag and Errors Interrupts */
        __HAL_HASH_DISABLE_IT(hhash, HASH_IT_DINI | HASH_IT_DCI);
        /* Change the HASH state */
        hhash->State = HAL_HASH_STATE_SUSPENDED;
        __HAL_UNLOCK(hhash);
      }
      else
      {
#endif /* USE_HAL_HASH_SUSPEND_RESUME */

        if (((hhash->HashInCount) + nbbytePartialHash) < (hhash->Size))
        {
          for (buffercounter = 0U; buffercounter < nbbytePartialHash ; buffercounter += 4U)
          {
            /* Write input data 4 bytes at a time */
            hhash->Instance->DIN = *(uint32_t *)inputaddr;
            inputaddr += 4U;
            hhash->HashInCount += 4U;
            hhash->pHashInBuffPtr += 4U;
          }
          /* Wait for HASH_IT_DINI flag to be set */
          count = HASH_TIMEOUTVALUE;
          do
          {
            count--;
            if (count == 0U)
            {
              /* Disable Interrupts */
              __HAL_HASH_DISABLE_IT(hhash, HASH_IT_DINI | HASH_IT_DCI);

              /* Change state */
              hhash->ErrorCode |= HAL_HASH_ERROR_TIMEOUT;
              hhash->State = HAL_HASH_STATE_READY;
              __HAL_UNLOCK(hhash);
              return HAL_ERROR;
            }
          } while (HAL_IS_BIT_CLR(hhash->Instance->SR, HASH_FLAG_DINIS));
        }
        else
        {
          while ((hhash->HashInCount) < hhash->Size)
          {
            /* Write input data 4 bytes at a time */
            hhash->Instance->DIN = *(uint32_t *)inputaddr;
            inputaddr += 4U;
            hhash->HashInCount += 4U;
            hhash->pHashInBuffPtr += 4U;
          }
          /* Call Input transfer complete callback */
#if (USE_HAL_HASH_REGISTER_CALLBACKS == 1U)
          /*Call registered Input complete callback*/
          hhash->InCpltCallback(hhash);
#else
          /*Call legacy weak Input complete callback*/
          HAL_HASH_InCpltCallback(hhash);
#endif /* USE_HAL_HASH_REGISTER_CALLBACKS */
          if (hhash->Accumulation == 0U)
          {
            if (__HAL_HASH_GET_IT_SOURCE(hhash, HASH_IT_DINI))
            {
              /* Start the message padding then the Digest calculation */
              SET_BIT(hhash->Instance->STR, HASH_STR_DCAL);

              /* Wait for HASH_FLAG_DCIS flag to be set */
              count = HASH_TIMEOUTVALUE;
              do
              {
                count--;
                if (count == 0U)
                {
                  /* Disable Interrupts */
                  __HAL_HASH_DISABLE_IT(hhash, HASH_IT_DINI | HASH_IT_DCI);

                  /* Change state */
                  hhash->ErrorCode |= HAL_HASH_ERROR_TIMEOUT;
                  hhash->State = HAL_HASH_STATE_READY;
                  __HAL_UNLOCK(hhash);
                  return HAL_ERROR;
                }
              } while (HAL_IS_BIT_CLR(hhash->Instance->SR, HASH_FLAG_DCIS));
            }
          }
          else
          {
            /* Reset multi buffers accumulation flag */
            hhash->Accumulation = 0U;
            /* Disable Interrupts */
            __HAL_HASH_DISABLE_IT(hhash, HASH_IT_DINI);
          }
        }
#if (USE_HAL_HASH_SUSPEND_RESUME == 1U)
      }
#endif /* USE_HAL_HASH_SUSPEND_RESUME */
    }
    else /*HMAC */
    {
      if (hhash->Phase == HAL_HASH_PHASE_HMAC_STEP_2) /* loading input*/
      {
#if (USE_HAL_HASH_SUSPEND_RESUME == 1U)
        /* If suspension flag has been raised, suspend processing */
        if (hhash->SuspendRequest == HAL_HASH_SUSPEND)
        {
          /* reset SuspendRequest */
          hhash->SuspendRequest = HAL_HASH_SUSPEND_NONE;
          /* Disable Computation Complete Flag and Errors Interrupts */
          __HAL_HASH_DISABLE_IT(hhash, HASH_IT_DINI | HASH_IT_DCI);
          /* Change the HASH state */
          hhash->State = HAL_HASH_STATE_SUSPENDED;
          __HAL_UNLOCK(hhash);
        }
        else
        {
#endif /* USE_HAL_HASH_SUSPEND_RESUME */
          if (hhash->Accumulation == 1U)
          {
            /* Configure the number of valid bits in last word of the message */
            MODIFY_REG(hhash->Instance->STR, HASH_STR_NBLW, 0U);
          }
          else
          {
            /* Configure the number of valid bits in last word of the message */
            MODIFY_REG(hhash->Instance->STR, HASH_STR_NBLW, 8U * (hhash->Size % 4U));
          }
          if (((hhash->HashInCount) + nbbytePartialHash) < (hhash->Size))
          {
            for (buffercounter = 0U; buffercounter < nbbytePartialHash ; buffercounter += 4U)
            {
              /* Write input data 4 bytes at a time */
              hhash->Instance->DIN = *(uint32_t *)inputaddr;
              inputaddr += 4U;
              hhash->HashInCount += 4U;
              hhash->pHashInBuffPtr += 4U;
            }
            /* Wait for HASH_IT_DINI flag to be set */
            count = HASH_TIMEOUTVALUE;
            do
            {
              count--;
              if (count == 0U)
              {
                /* Disable Interrupts */
                __HAL_HASH_DISABLE_IT(hhash, HASH_IT_DINI | HASH_IT_DCI);

                /* Change state */
                hhash->ErrorCode |= HAL_HASH_ERROR_TIMEOUT;
                hhash->State = HAL_HASH_STATE_READY;
                __HAL_UNLOCK(hhash);
                return HAL_ERROR;
              }
            } while (HAL_IS_BIT_CLR(hhash->Instance->SR, HASH_FLAG_DINIS));
          }
          else
          {
            while ((hhash->HashInCount) < hhash->Size)
            {
              /* Write input data 4 bytes at a time */
              hhash->Instance->DIN = *(uint32_t *)inputaddr;
              inputaddr += 4U;
              hhash->HashInCount += 4U;
              hhash->pHashInBuffPtr += 4U;
            }
            /* Call Input transfer complete callback */
#if (USE_HAL_HASH_REGISTER_CALLBACKS == 1U)
            /*Call registered Input complete callback*/
            hhash->InCpltCallback(hhash);
#else
            /*Call legacy weak Input complete callback*/
            HAL_HASH_InCpltCallback(hhash);
#endif /* USE_HAL_HASH_REGISTER_CALLBACKS */

            if (hhash->Accumulation == 0U)
            {
              if (__HAL_HASH_GET_IT_SOURCE(hhash, HASH_IT_DINI))
              {
                /* Start the message padding then the Digest calculation */
                SET_BIT(hhash->Instance->STR, HASH_STR_DCAL);

                /* Wait for HASH_FLAG_BUSY flag to be set */
                count = HASH_TIMEOUTVALUE;
                do
                {
                  count--;
                  if (count == 0U)
                  {
                    /* Disable Interrupts */
                    __HAL_HASH_DISABLE_IT(hhash, HASH_IT_DINI | HASH_IT_DCI);

                    /* Change state */
                    hhash->ErrorCode |= HAL_HASH_ERROR_TIMEOUT;
                    hhash->State = HAL_HASH_STATE_READY;
                    __HAL_UNLOCK(hhash);
                    return HAL_ERROR;
                  }
                } while (HAL_IS_BIT_SET(hhash->Instance->SR, HASH_FLAG_BUSY));

                hhash->Phase = HAL_HASH_PHASE_HMAC_STEP_3;
                hhash->HashInCount = 0U;
                hhash->pHashKeyBuffPtr = hhash->Init.pKey;
              }
            }

            else
            {
              /* Disable Interrupts */
              __HAL_HASH_DISABLE_IT(hhash, HASH_IT_DINI | HASH_IT_DCI);
              hhash->State = HAL_HASH_STATE_READY;
              __HAL_UNLOCK(hhash);
              return HAL_OK;
            }
          }
#if (USE_HAL_HASH_SUSPEND_RESUME == 1U)
        }
#endif /* USE_HAL_HASH_SUSPEND_RESUME */
      }

      else if (hhash->Phase == HAL_HASH_PHASE_HMAC_STEP_3)/* loading Key*/
      {

        /* Configure the number of valid bits in last word of the Key */
        MODIFY_REG(hhash->Instance->STR, HASH_STR_NBLW, 8U * ((hhash->Init.KeySize) % 4U));

        if (((hhash->HashInCount) + nbbytePartialHash) < (hhash->Init.KeySize))
        {
          for (buffercounter = 0U; buffercounter < nbbytePartialHash ; buffercounter += 4U)
          {
            /* Write input data 4 bytes at a time */
            hhash->Instance->DIN = *(uint32_t *)keyaddr;
            keyaddr += 4U;
            hhash->HashInCount += 4U;
            hhash->pHashKeyBuffPtr += 4U;
          }
          /* Wait for HASH_IT_DINI flag to be set */
          count = HASH_TIMEOUTVALUE;
          do
          {
            count--;
            if (count == 0U)
            {
              /* Disable Interrupts */
              __HAL_HASH_DISABLE_IT(hhash, HASH_IT_DINI | HASH_IT_DCI);

              /* Change state */
              hhash->ErrorCode |= HAL_HASH_ERROR_TIMEOUT;
              hhash->State = HAL_HASH_STATE_READY;
              __HAL_UNLOCK(hhash);
              return HAL_ERROR;
            }
          } while (HAL_IS_BIT_CLR(hhash->Instance->SR, HASH_FLAG_DINIS));
        }
        else
        {
          while ((hhash->HashInCount) < (hhash->Init.KeySize))
          {
            /* Write input data 4 bytes at a time */
            hhash->Instance->DIN = *(uint32_t *)keyaddr;
            keyaddr += 4U;
            hhash->HashInCount += 4U;
          }
          /* Start the message padding then the Digest calculation */
          SET_BIT(hhash->Instance->STR, HASH_STR_DCAL);

          /* Wait for HASH_FLAG_DCIS flag to be set */
          count = HASH_TIMEOUTVALUE;
          do
          {
            count--;
            if (count == 0U)
            {
              /* Disable Interrupts */
              __HAL_HASH_DISABLE_IT(hhash, HASH_IT_DINI | HASH_IT_DCI);

              /* Change state */
              hhash->ErrorCode |= HAL_HASH_ERROR_TIMEOUT;
              hhash->State = HAL_HASH_STATE_READY;
              __HAL_UNLOCK(hhash);
              return HAL_ERROR;
            }
          } while (HAL_IS_BIT_CLR(hhash->Instance->SR, HASH_FLAG_DCIS));
        }
      }
      else  /*first step , loading key*/
      {

        hhash->Phase = HAL_HASH_PHASE_HMAC_STEP_1;

        if (((hhash->HashInCount) + nbbytePartialHash) < (hhash->Init.KeySize))
        {
          for (buffercounter = 0U; buffercounter < nbbytePartialHash ; buffercounter += 4U)
          {
            /* Write input data 4 bytes at a time */
            hhash->Instance->DIN = *(uint32_t *)keyaddr;
            keyaddr += 4U;
            hhash->HashInCount += 4U;
            hhash->pHashKeyBuffPtr += 4U;
          }
          /* Wait for HASH_IT_DINI flag to be set */
          count = HASH_TIMEOUTVALUE;
          do
          {
            count--;
            if (count == 0U)
            {
              /* Disable Interrupts */
              __HAL_HASH_DISABLE_IT(hhash, HASH_IT_DINI | HASH_IT_DCI);

              /* Change state */
              hhash->ErrorCode |= HAL_HASH_ERROR_TIMEOUT;
              hhash->State = HAL_HASH_STATE_READY;
              __HAL_UNLOCK(hhash);
              return HAL_ERROR;
            }
          } while (HAL_IS_BIT_CLR(hhash->Instance->SR, HASH_FLAG_DINIS));
        }
        else
        {
          while ((hhash->HashInCount) < (hhash->Init.KeySize))
          {
            /* Write input data 4 bytes at a time */
            hhash->Instance->DIN = *(uint32_t *)keyaddr;
            keyaddr += 4U;
            hhash->HashInCount += 4U;
            hhash->pHashKeyBuffPtr += 4U;
          }
          /* Start the message padding then the Digest calculation */
          SET_BIT(hhash->Instance->STR, HASH_STR_DCAL);

          /* Wait for HASH_FLAG_BUSY flag to be set */
          count = HASH_TIMEOUTVALUE;
          do
          {
            count--;
            if (count == 0U)
            {
              /* Disable Interrupts */
              __HAL_HASH_DISABLE_IT(hhash, HASH_IT_DINI | HASH_IT_DCI);

              /* Change state */
              hhash->ErrorCode |= HAL_HASH_ERROR_TIMEOUT;
              hhash->State = HAL_HASH_STATE_READY;
              __HAL_UNLOCK(hhash);
              return HAL_ERROR;
            }
          } while (HAL_IS_BIT_SET(hhash->Instance->SR, HASH_FLAG_BUSY));
          /*change Phase to step 2*/
          hhash->Phase = HAL_HASH_PHASE_HMAC_STEP_2;
          hhash->HashInCount = 0U;
        }
      }
    }
  }
  else if (hhash->State == HAL_HASH_STATE_SUSPENDED)
  {
    return HAL_OK;
  }
  else
  {
    /* Busy error code field */
    hhash->ErrorCode |= HAL_HASH_ERROR_BUSY;
#if (USE_HAL_HASH_REGISTER_CALLBACKS == 1U)
    /*Call registered error callback*/
    hhash->ErrorCallback(hhash);
#else
    /*Call legacy weak error callback*/
    HAL_HASH_ErrorCallback(hhash);
#endif /* USE_HAL_HASH_REGISTER_CALLBACKS */
  }

  return HAL_OK;
}

/**
  * @brief  Retrieve the message digest.
  * @param hhash HASH handle
  * @param  pMsgDigest pointer to the computed digest.
  * @param  Size message digest size in bytes.
  * @retval None
  */
static void HASH_GetDigest(const HASH_HandleTypeDef *hhash, const uint8_t *pMsgDigest, uint8_t Size)
{
  uint32_t msgdigest = (uint32_t)pMsgDigest;

  switch (Size)
  {
    case 20:  /* SHA1 */
      *(uint32_t *)(msgdigest) = __REV(hhash->Instance->HR[0]);
      msgdigest += 4U;
      *(uint32_t *)(msgdigest) = __REV(hhash->Instance->HR[1]);
      msgdigest += 4U;
      *(uint32_t *)(msgdigest) = __REV(hhash->Instance->HR[2]);
      msgdigest += 4U;
      *(uint32_t *)(msgdigest) = __REV(hhash->Instance->HR[3]);
      msgdigest += 4U;
      *(uint32_t *)(msgdigest) = __REV(hhash->Instance->HR[4]);
      break;

    case 28:  /* SHA224 */
      *(uint32_t *)(msgdigest) = __REV(hhash->Instance->HR[0]);
      msgdigest += 4U;
      *(uint32_t *)(msgdigest) = __REV(hhash->Instance->HR[1]);
      msgdigest += 4U;
      *(uint32_t *)(msgdigest) = __REV(hhash->Instance->HR[2]);
      msgdigest += 4U;
      *(uint32_t *)(msgdigest) = __REV(hhash->Instance->HR[3]);
      msgdigest += 4U;
      *(uint32_t *)(msgdigest) = __REV(hhash->Instance->HR[4]);
      msgdigest += 4U;
      *(uint32_t *)(msgdigest) = __REV(HASH_DIGEST->HR[5]);
      msgdigest += 4U;
      *(uint32_t *)(msgdigest) = __REV(HASH_DIGEST->HR[6]);

      break;
    case 32:   /* SHA256 */
      *(uint32_t *)(msgdigest) = __REV(hhash->Instance->HR[0]);
      msgdigest += 4U;
      *(uint32_t *)(msgdigest) = __REV(hhash->Instance->HR[1]);
      msgdigest += 4U;
      *(uint32_t *)(msgdigest) = __REV(hhash->Instance->HR[2]);
      msgdigest += 4U;
      *(uint32_t *)(msgdigest) = __REV(hhash->Instance->HR[3]);
      msgdigest += 4U;
      *(uint32_t *)(msgdigest) = __REV(hhash->Instance->HR[4]);
      msgdigest += 4U;
      *(uint32_t *)(msgdigest) = __REV(HASH_DIGEST->HR[5]);
      msgdigest += 4U;
      *(uint32_t *)(msgdigest) = __REV(HASH_DIGEST->HR[6]);
      msgdigest += 4U;
      *(uint32_t *)(msgdigest) = __REV(HASH_DIGEST->HR[7]);
      break;
    case 48:   /* SHA384 */
      *(uint32_t *)(msgdigest) = __REV(hhash->Instance->HR[0]);
      msgdigest += 4U;
      *(uint32_t *)(msgdigest) = __REV(hhash->Instance->HR[1]);
      msgdigest += 4U;
      *(uint32_t *)(msgdigest) = __REV(hhash->Instance->HR[2]);
      msgdigest += 4U;
      *(uint32_t *)(msgdigest) = __REV(hhash->Instance->HR[3]);
      msgdigest += 4U;
      *(uint32_t *)(msgdigest) = __REV(hhash->Instance->HR[4]);
      msgdigest += 4U;
      *(uint32_t *)(msgdigest) = __REV(HASH_DIGEST->HR[5]);
      msgdigest += 4U;
      *(uint32_t *)(msgdigest) = __REV(HASH_DIGEST->HR[6]);
      msgdigest += 4U;
      *(uint32_t *)(msgdigest) = __REV(HASH_DIGEST->HR[7]);
      msgdigest += 4U;
      *(uint32_t *)(msgdigest) = __REV(HASH_DIGEST->HR[8]);
      msgdigest += 4U;
      *(uint32_t *)(msgdigest) = __REV(HASH_DIGEST->HR[9]);
      msgdigest += 4U;
      *(uint32_t *)(msgdigest) = __REV(HASH_DIGEST->HR[10]);
      msgdigest += 4U;
      *(uint32_t *)(msgdigest) = __REV(HASH_DIGEST->HR[11]);
      break;

    case 64:   /* SHA 512 */
      *(uint32_t *)(msgdigest) = __REV(hhash->Instance->HR[0]);
      msgdigest += 4U;
      *(uint32_t *)(msgdigest) = __REV(hhash->Instance->HR[1]);
      msgdigest += 4U;
      *(uint32_t *)(msgdigest) = __REV(hhash->Instance->HR[2]);
      msgdigest += 4U;
      *(uint32_t *)(msgdigest) = __REV(hhash->Instance->HR[3]);
      msgdigest += 4U;
      *(uint32_t *)(msgdigest) = __REV(hhash->Instance->HR[4]);
      msgdigest += 4U;
      *(uint32_t *)(msgdigest) = __REV(HASH_DIGEST->HR[5]);
      msgdigest += 4U;
      *(uint32_t *)(msgdigest) = __REV(HASH_DIGEST->HR[6]);
      msgdigest += 4U;
      *(uint32_t *)(msgdigest) = __REV(HASH_DIGEST->HR[7]);
      msgdigest += 4U;
      *(uint32_t *)(msgdigest) = __REV(HASH_DIGEST->HR[8]);
      msgdigest += 4U;
      *(uint32_t *)(msgdigest) = __REV(HASH_DIGEST->HR[9]);
      msgdigest += 4U;
      *(uint32_t *)(msgdigest) = __REV(HASH_DIGEST->HR[10]);
      msgdigest += 4U;
      *(uint32_t *)(msgdigest) = __REV(HASH_DIGEST->HR[11]);
      msgdigest += 4U;
      *(uint32_t *)(msgdigest) = __REV(HASH_DIGEST->HR[12]);
      msgdigest += 4U;
      *(uint32_t *)(msgdigest) = __REV(HASH_DIGEST->HR[13]);
      msgdigest += 4U;
      *(uint32_t *)(msgdigest) = __REV(HASH_DIGEST->HR[14]);
      msgdigest += 4U;
      *(uint32_t *)(msgdigest) = __REV(HASH_DIGEST->HR[15]);

      break;
    default:
      break;
  }
}

/**
  * @brief  Handle HASH processing Timeout.
  * @param  hhash HASH handle.
  * @param  Flag specifies the HASH flag to check.
  * @param  Status the Flag status (SET or RESET).
  * @param  Timeout Timeout duration.
  * @retval HAL status
  */
static HAL_StatusTypeDef HASH_WaitOnFlagUntilTimeout(HASH_HandleTypeDef *hhash, uint32_t Flag, FlagStatus Status,
                                                     uint32_t Timeout)
{
  uint32_t tickstart = HAL_GetTick();

  while (__HAL_HASH_GET_FLAG(hhash, Flag) == Status)
  {
    /* Check for the Timeout */
    if (Timeout != HAL_MAX_DELAY)
    {
      if (((HAL_GetTick() - tickstart) > Timeout) || (Timeout == 0U))
      {
        /* Set State to Ready to be able to restart later on */
        hhash->State  = HAL_HASH_STATE_READY;
        hhash->ErrorCode |= HAL_HASH_ERROR_TIMEOUT;
        /* Process Unlocked */
        __HAL_UNLOCK(hhash);

        return HAL_ERROR;
      }
    }
  }
  return HAL_OK;
}

/**
  * @}
  */


#endif /* HAL_HASH_MODULE_ENABLED */

#endif /*  HASH*/
/**
  * @}
  */

/**
  * @}
  */
