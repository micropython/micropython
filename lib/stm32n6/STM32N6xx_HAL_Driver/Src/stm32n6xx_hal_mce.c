/**
  ******************************************************************************
  * @file    stm32n6xx_hal_mce.c
  * @author  MCD Application Team
  * @brief   MCE HAL module driver.
  *          This file provides firmware functions to manage the following
  *          features of the Memory Cipher Engine (MCE) peripheral:
  *           + Initialization and de-initialization functions
  *           + Region setting/enable functions
  *           + Peripheral State functions
  *
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
    The MCE HAL driver can be used as follows:

    (#) ToBC (refer to MCE comments)


    [..]

    *** Callback registration ***
    =============================================
    [..]

     The compilation flag USE_HAL_MCE_REGISTER_CALLBACKS, when set to 1,
     allows the user to configure dynamically the driver callbacks.
     Use Functions @ref HAL_MCE_RegisterCallback()
     to register an interrupt callback.
    [..]

     ToBC (refer to MCE comments)

     When the compilation flag USE_HAL_MCE_REGISTER_CALLBACKS is set to 0 or
     not defined, the callback registration feature is not available and all callbacks
     are set to the corresponding weak functions.

  @endverbatim
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal.h"

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */

/** @defgroup MCE MCE
  * @brief MCE HAL module driver.
  * @{
  */


#ifdef HAL_MCE_MODULE_ENABLED

#if defined(MCE1)

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
#define MCE_TIMEOUT_VALUE  255U   /* Internal timeout for keys valid flag */
#define MCE1_CONTEXT_NB    2U
/* Private macros ------------------------------------------------------------*/

#define IS_MCE_AES_INSTANCE(INSTANCE) ((INSTANCE) == MCE1)

/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
/** @addtogroup MCE_Exported_Functions
  * @{
  */

/** @defgroup MCE_Exported_Functions_Group1 Initialization and de-initialization functions
  *  @brief    Initialization and Configuration functions.
  *
@verbatim
  ==============================================================================
              ##### Initialization and de-initialization functions #####
  ==============================================================================

@endverbatim
  * @{
  */

/**
  * @brief  Initialize the MCE peripheral and create the associated handle.
  * @param  hmce pointer to an MCE_HandleTypeDef structure that contains
  *         the configuration information for MCE module
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_MCE_Init(MCE_HandleTypeDef *hmce)
{
  /* Check the MCE handle allocation */
  if (hmce == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_MCE_ALL_INSTANCE(hmce->Instance));

  if (hmce->State == HAL_MCE_STATE_RESET)
  {
    /* Allocate lock resource and initialize it */
    __HAL_UNLOCK(hmce);

#if (USE_HAL_MCE_REGISTER_CALLBACKS == 1)
    /* Init the MCE Callback settings */
    hmce->ErrorCallback = HAL_MCE_ErrorCallback; /* Legacy weak callback */

    if (hmce->MspInitCallback == NULL)
    {
      hmce->MspInitCallback = HAL_MCE_MspInit; /* Legacy weak MspInit */
    }

    /* Init the low level hardware */
    hmce->MspInitCallback(hmce);
#else
    /* Init the low level hardware */
    HAL_MCE_MspInit(hmce);
#endif /* USE_HAL_MCE_REGISTER_CALLBACKS */
  }

  /* Change the MCE state */
  hmce->State = HAL_MCE_STATE_READY;

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  DeInitialize the MCE peripheral.
  * @note   Any lock set at peripheral, key or cipher context level requires a MCE peripheral reset
  * @param  hmce pointer to an MCE_HandleTypeDef structure that contains
  *         the configuration information for MCE module
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_MCE_DeInit(MCE_HandleTypeDef *hmce)
{
  MCE_Region_TypeDef *p_region;
  MCE_Context_TypeDef *p_context;

  /* Check the MCE handle allocation */
  if (hmce == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_MCE_ALL_INSTANCE(hmce->Instance));

  /* Change the MCE state */
  hmce->State = HAL_MCE_STATE_BUSY;

  /* Disable interrupts */
  CLEAR_BIT(hmce->Instance->IAIER, MCE_IAIER_IAEIE);

  /* Disable all regions */
  for (uint32_t i = MCE_REGION1; i <= MCE_REGION4; i++)
  {
    p_region = (MCE_Region_TypeDef *)((uint32_t)(hmce->Instance) + 0x40U + (0x10U * i));
    CLEAR_BIT(p_region->REGCR, MCE_REGCR_BREN);
    /** we don't have the registry ATTR in this product */
  }

  /* Disable all cipher contexts if any for the MCE instance */
  if (hmce->Instance == MCE1)
  {
    for (uint32_t i = 0; i < MCE1_CONTEXT_NB; i++)
    {
      p_context = (MCE_Context_TypeDef *)((uint32_t)(hmce->Instance) + 0x240U + (0x30U * i));
      CLEAR_BIT(p_context->CCCFGR, MCE_CCCFGR_CCEN);
    }
  }

  /* Clear privileged-only configuration access */
  /** we don't have the registry PRIVCFGR in this product */

  /* Clear flags */
  WRITE_REG(hmce->Instance->IACR, MCE_IACR_IAEF);

#if (USE_HAL_MCE_REGISTER_CALLBACKS == 1)
  if (hmce->MspDeInitCallback == NULL)
  {
    hmce->MspDeInitCallback = HAL_MCE_MspDeInit; /* Legacy weak MspDeInit */
  }

  /* DeInit the low level hardware: CLOCK, NVIC */
  hmce->MspDeInitCallback(hmce);
#else
  /* DeInit the low level hardware: CLOCK, NVIC */
  HAL_MCE_MspDeInit(hmce);
#endif /* USE_HAL_MCE_REGISTER_CALLBACKS */

  /* Change the MCE state */
  hmce->State = HAL_MCE_STATE_RESET;

  /* Reset MCE error status */
  hmce->ErrorCode = HAL_MCE_ERROR_NONE;

  /* Release Lock */
  __HAL_UNLOCK(hmce);

  /* Return function status */
  return HAL_OK;
}


/** *********  main sequence functions : set the configuration of regions, AES context and Noekeon config *********/

/**
  * @brief  Write instance master or fast master keys
  * @param  hmce pointer to an MCE_HandleTypeDef structure that contains
  *         the configuration information for MCE module
  * @param  pConfig pointer at an MCE_NoekeonConfig structure that contains
            the configuration on the Noekeon encryption engine
  * @note   Writes are ignored if MKLOCK or GLOCK bit is set in MCE_CR register and HAL error
  *         is reported in that case
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_MCE_ConfigNoekeon(MCE_HandleTypeDef *hmce, const MCE_NoekeonConfigTypeDef *pConfig)
{
  HAL_StatusTypeDef ret = HAL_OK;
  uint32_t tickstart;

  /* Check the parameters */
  assert_param(IS_MCE_ALL_INSTANCE(hmce->Instance));

  /* If Key is null, or
     if the global lock is set or
     if the master keys lock is set,
     return an error */
  tickstart = HAL_GetTick();
  if (((hmce->Instance->CR & (MCE_CR_GLOCK | MCE_CR_MKLOCK)) == 0U) && (pConfig->pKey != NULL))
  {
    /* Take Lock */
    __HAL_LOCK(hmce);

    MODIFY_REG(hmce->Instance->CR, MCE_CR_CIPHERSEL, MCE_NOEKEON);
    /* master keys are used for encryption */
    if (pConfig->KeyType == MCE_USE_MASTERKEYS)
    {
      /* If Master Key valid flag is set, need to write dummy value in MKEYRx to reset it */
      if ((hmce->Instance->SR & MCE_SR_MKVALID) != 0U)
      {
        WRITE_REG(hmce->Instance->MKEYR0, 0U);
      }

      /* Set Key */
      WRITE_REG(hmce->Instance->MKEYR0, pConfig->pKey[0]);
      WRITE_REG(hmce->Instance->MKEYR1, pConfig->pKey[1]);
      WRITE_REG(hmce->Instance->MKEYR2, pConfig->pKey[2]);
      WRITE_REG(hmce->Instance->MKEYR3, pConfig->pKey[3]);
      WRITE_REG(hmce->Instance->MKEYR4, pConfig->pKey[4]);
      WRITE_REG(hmce->Instance->MKEYR5, pConfig->pKey[5]);
      WRITE_REG(hmce->Instance->MKEYR6, pConfig->pKey[6]);
      WRITE_REG(hmce->Instance->MKEYR7, pConfig->pKey[7]);
      while (HAL_IS_BIT_CLR(hmce->Instance->SR, MCE_SR_MKVALID))
      {
        /* Check for the Timeout */
        if ((HAL_GetTick() - tickstart) > MCE_TIMEOUT_VALUE)
        {

          hmce->ErrorCode |= HAL_MCE_MASTER_KEY_ERROR;
          ret = HAL_ERROR;
          break;
        }
      }

    }
    else if (pConfig->KeyType == MCE_USE_FASTMASTERKEYS)
    {
      /* If Fast Master Key valid flag is set, need to write dummy value in FMKEYRx to reset it */
      if ((hmce->Instance->SR & MCE_SR_FMKVALID) != 0U)
      {
        WRITE_REG(hmce->Instance->FMKEYR0, 0U);
      }
      /* Set Key */
      WRITE_REG(hmce->Instance->FMKEYR0, pConfig->pKey[0]);
      WRITE_REG(hmce->Instance->FMKEYR1, pConfig->pKey[1]);
      WRITE_REG(hmce->Instance->FMKEYR2, pConfig->pKey[2]);
      WRITE_REG(hmce->Instance->FMKEYR3, pConfig->pKey[3]);
      WRITE_REG(hmce->Instance->FMKEYR4, pConfig->pKey[4]);
      WRITE_REG(hmce->Instance->FMKEYR5, pConfig->pKey[5]);
      WRITE_REG(hmce->Instance->FMKEYR6, pConfig->pKey[6]);
      WRITE_REG(hmce->Instance->FMKEYR7, pConfig->pKey[7]);
      while (HAL_IS_BIT_CLR(hmce->Instance->SR, MCE_SR_FMKVALID))
      {
        /* Check for the Timeout */
        if ((HAL_GetTick() - tickstart) > MCE_TIMEOUT_VALUE)
        {
          hmce->ErrorCode |= HAL_MCE_FASTMASTER_KEY_ERROR;
          ret = HAL_ERROR;
          break;
        }
      }
    }
    else
    {
      /* nothing to do, the keytype is not valid */
    }
    __HAL_UNLOCK(hmce);
  }
  return ret;
}


/**
  * @brief  Set context configuration.
  * @param  hmce pointer to an MCE_HandleTypeDef structure that contains
  *         the configuration information for MCE module
  * @param  AESConfig context configuration
  * @param  ContextIndex index of context that is configured
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_MCE_ConfigAESContext(MCE_HandleTypeDef *hmce, const MCE_AESConfigTypeDef  *AESConfig,
                                           uint32_t ContextIndex)
{
  HAL_StatusTypeDef ret = HAL_ERROR;
  MCE_Context_TypeDef *p_context;
  uint32_t *p_key;
  __IO uint32_t address;

  /* Check the parameters */
  assert_param(IS_MCE_ALL_INSTANCE(hmce->Instance));
  assert_param(IS_MCE_CONTEXT(hmce->Instance, ContextIndex));


  /* If global lock is set or no configuration provided, context cannot be configured */
  if (((hmce->Instance->CR & MCE_CR_GLOCK) != MCE_CR_GLOCK) && (AESConfig != NULL) && \
      ((ContextIndex == MCE_CONTEXT1) || (ContextIndex == MCE_CONTEXT2)))
  {
    address = (__IO uint32_t)((uint32_t)hmce->Instance + 0x240UL + \
                              (0x30UL * ((ContextIndex - MCE_CONTEXT1) >> MCE_REGCR_CTXID_Pos)));
    p_context = (MCE_Context_TypeDef *)address;
    MODIFY_REG(hmce->Instance->CR, MCE_CR_CIPHERSEL, AESConfig->KeySize);
    /* Check cipher context is not locked */
    if ((p_context->CCCFGR & MCE_CCCFGR_CCLOCK) != MCE_CCCFGR_CCLOCK)
    {
      if ((p_context->CCCFGR & MCE_CCCFGR_KEYLOCK) != MCE_CCCFGR_KEYLOCK)
      {
        /* Take Lock */
        __HAL_LOCK(hmce);

        /* Write nonce */
        WRITE_REG(p_context->CCNR0, AESConfig->Nonce[0]);
        WRITE_REG(p_context->CCNR1, AESConfig->Nonce[1]);

        if (AESConfig->pKey != NULL)
        {
          p_key = AESConfig->pKey;
          WRITE_REG(p_context->CCKEYR0, *p_key);
          p_key++;
          WRITE_REG(p_context->CCKEYR1, *p_key);
          p_key++;
          WRITE_REG(p_context->CCKEYR2, *p_key);
          p_key++;
          WRITE_REG(p_context->CCKEYR3, *p_key);
        }
        /* Compute theoretically expected CRC and compare it with that reported by the peripheral */

        /* Write version */
        MODIFY_REG(p_context->CCCFGR, MCE_CCCFGR_VERSION, ((uint32_t) AESConfig->Version) << MCE_CCCFGR_VERSION_Pos);

        MODIFY_REG(p_context->CCCFGR, MCE_CCCFGR_MODE, (uint32_t) AESConfig->Cipher_Mode);
        ret = HAL_OK;

        /* Release Lock */
        __HAL_UNLOCK(hmce);
      }
    }
  }
  return ret;
}


/**
  * @brief  Set region configuration.
  * @param  hmce pointer to an MCE_HandleTypeDef structure that contains
  *         the configuration information for MCE module
  * @param  RegionIndex index of region that is configured
  * @param  pConfig region configuration
  * @note   The region is enabled by default once the configuration is complete
  * @note   An enabled region is temporary disabled to apply the new configuration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_MCE_ConfigRegion(MCE_HandleTypeDef *hmce, uint32_t RegionIndex,
                                       const MCE_RegionConfigTypeDef *pConfig)
{
  HAL_StatusTypeDef ret = HAL_ERROR;
  MCE_Region_TypeDef *p_region;
  __IO uint32_t address;

  /* Check the parameters */
  assert_param(IS_MCE_ALL_INSTANCE(hmce->Instance));
  assert_param(IS_MCE_REGIONINDEX(RegionIndex));
  assert_param(IS_MCE_ALGORITHM(hmce->Instance, pConfig->Mode));

  /** we have no AccessMode nor PrivilegedAccess fields in this product */

  /* If global lock is set or no configuration provided, region cannot be configured */
  if (((hmce->Instance->CR & MCE_CR_GLOCK) != MCE_CR_GLOCK) && (pConfig != NULL))
  {
    /* Take Lock */
    __HAL_LOCK(hmce);

    address = (__IO uint32_t)((uint32_t)hmce->Instance + 0x40U + (0x10U * RegionIndex));
    p_region = (MCE_Region_TypeDef *)address;

    /* If region is enabled, disable it during the configuration process */
    if ((p_region->REGCR & MCE_REGCR_BREN) == MCE_REGCR_BREN)
    {
      CLEAR_BIT(p_region->REGCR, MCE_REGCR_BREN);
    }

    /* Set privileged access, ciphering algorithm and context ID */
    MODIFY_REG(p_region->REGCR,  MCE_REGCR_ENC, pConfig->Mode);

    /* Set start and end addresses */
    WRITE_REG(p_region->SADDR, pConfig->StartAddress);
    WRITE_REG(p_region->EADDR, pConfig->EndAddress);

    /* Set write attribute for the region */
    /** we have no ATTR register in this product */

    /* Enable the region by default */
    SET_BIT(p_region->REGCR, MCE_REGCR_BREN);

    ret = HAL_OK;

    /* Release Lock */
    __HAL_UNLOCK(hmce);
  }

  return ret;

}

/**
  * @brief  Set region AES Context.
  * @param  hmce pointer to an MCE_HandleTypeDef structure that contains
  *         the configuration information for MCE module
  * @param  ContextIndex index of context that is configured
  * @param  RegionIndex index of region that is configured
  * @note   The Region is enabled by default once the configuration is complete
  * @note   An enabled region is temporary disabled to apply the new configuration
  * @retval HAL status
  */

HAL_StatusTypeDef HAL_MCE_SetRegionAESContext(MCE_HandleTypeDef *hmce, uint32_t ContextIndex, uint32_t RegionIndex)
{
  HAL_StatusTypeDef ret = HAL_ERROR;
  MCE_Region_TypeDef *p_region;
  __IO uint32_t address;

  assert_param(IS_MCE_CONTEXT(hmce->Instance, ContextIndex));
  assert_param(IS_MCE_REGIONINDEX(RegionIndex));

  /* If global lock is set, region cannot be configured */
  if ((hmce->Instance->CR & MCE_CR_GLOCK) != MCE_CR_GLOCK)
  {

    /* Take Lock */
    __HAL_LOCK(hmce);

    address = (__IO uint32_t)((uint32_t)hmce->Instance + 0x40U + (0x10U * RegionIndex));
    p_region = (MCE_Region_TypeDef *)address;

    /* If region is enabled, disable it during the configuration process */
    if ((p_region->REGCR & MCE_REGCR_BREN) == MCE_REGCR_BREN)
    {
      CLEAR_BIT(p_region->REGCR, MCE_REGCR_BREN);
    }

    /* Set context ID */
    MODIFY_REG(p_region->REGCR, MCE_REGCR_CTXID, ContextIndex);

    /* Enable the region by default */
    SET_BIT(p_region->REGCR, MCE_REGCR_BREN);

    ret = HAL_OK;

    /* Release Lock */
    __HAL_UNLOCK(hmce);

  }

  return ret;
}


/** *********  Enabling and disabling functions : enable/disable AES and Noekeon configurations and regions *********/


/**
  * @brief  Cipher context enable
  * @param  hmce pointer to an MCE_HandleTypeDef structure that contains
  *         the configuration information for MCE module
  * @param  ContextIndex index of context that is enabled
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_MCE_EnableAESContext(MCE_HandleTypeDef *hmce, uint32_t ContextIndex)
{
  MCE_Context_TypeDef *p_context;
  __IO uint32_t address;

  /* Check the parameters */
  assert_param(IS_MCE_ALL_INSTANCE(hmce->Instance));
  assert_param(IS_MCE_CONTEXT(hmce->Instance, ContextIndex));

  if ((ContextIndex != MCE_CONTEXT1) && (ContextIndex != MCE_CONTEXT2))
  {
    return HAL_ERROR;
  }

  address = (__IO uint32_t)((uint32_t)hmce->Instance + 0x240UL + \
                            (0x30UL * ((ContextIndex - MCE_CONTEXT1) >> MCE_REGCR_CTXID_Pos)));
  p_context = (MCE_Context_TypeDef *)address;

  /* If cipher context is locked or if global lock is set,
     and if cipher context is not enabled already, return an error */

  if ((p_context->CCCFGR & MCE_CCCFGR_CCEN) != MCE_CCCFGR_CCEN)
  {
    if ((p_context->CCCFGR & MCE_CCCFGR_CCLOCK) == MCE_CCCFGR_CCLOCK)
    {
      return HAL_ERROR;
    }
    if ((hmce->Instance->CR & MCE_CR_GLOCK) == MCE_CR_GLOCK)
    {
      return HAL_ERROR;
    }
  }

  __HAL_LOCK(hmce);

  /* Enable context */
  SET_BIT(p_context->CCCFGR, MCE_CCCFGR_CCEN);

  __HAL_UNLOCK(hmce);

  return HAL_OK;

}

/**
  * @brief  Cipher context disable
  * @param  hmce pointer to an MCE_HandleTypeDef structure that contains
  *         the configuration information for MCE module
  * @param  ContextIndex index of context that is disabled
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_MCE_DisableAESContext(MCE_HandleTypeDef *hmce, uint32_t ContextIndex)
{
  MCE_Context_TypeDef *p_context;
  __IO uint32_t address;

  /* Check the parameters */
  assert_param(IS_MCE_ALL_INSTANCE(hmce->Instance));
  assert_param(IS_MCE_CONTEXT(hmce->Instance, ContextIndex));

  if ((ContextIndex != MCE_CONTEXT1) && (ContextIndex != MCE_CONTEXT2))
  {
    return HAL_ERROR;
  }

  address = (__IO uint32_t)((uint32_t)hmce->Instance + 0x240UL + \
                            (0x30UL * ((ContextIndex - MCE_CONTEXT1) >> MCE_REGCR_CTXID_Pos)));
  p_context = (MCE_Context_TypeDef *)address;

  /* If cipher context is locked or if global lock is set,
     and if cipher context is not disabled already, return an error */

  if ((p_context->CCCFGR & MCE_CCCFGR_CCEN) == MCE_CCCFGR_CCEN)
  {
    if ((p_context->CCCFGR & MCE_CCCFGR_CCLOCK) == MCE_CCCFGR_CCLOCK)
    {
      return HAL_ERROR;
    }
    if ((hmce->Instance->CR & MCE_CR_GLOCK) == MCE_CR_GLOCK)
    {
      return HAL_ERROR;
    }
  }

  __HAL_LOCK(hmce);

  /* Disable context */
  CLEAR_BIT(p_context->CCCFGR, MCE_CCCFGR_CCEN);

  __HAL_UNLOCK(hmce);

  return HAL_OK;

}


/**
  * @brief  Enable region.
  * @param  hmce pointer to an MCE_HandleTypeDef structure that contains
  *         the configuration information for MCE module
  * @param  RegionIndex index of region that is enabled
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_MCE_EnableRegion(MCE_HandleTypeDef *hmce, uint32_t RegionIndex)
{
  HAL_StatusTypeDef ret = HAL_ERROR;
  MCE_Region_TypeDef *p_region;
  __IO uint32_t address;

  /* Check the parameters */
  assert_param(IS_MCE_ALL_INSTANCE(hmce->Instance));
  assert_param(IS_MCE_REGIONINDEX(RegionIndex));

  /* If global lock is set, region cannot be configured */
  if ((hmce->Instance->CR & MCE_CR_GLOCK) != MCE_CR_GLOCK)
  {
    address = (__IO uint32_t)((uint32_t)hmce->Instance + 0x40U + (0x10U * RegionIndex));
    p_region = (MCE_Region_TypeDef *)address;

    /* Take Lock */
    __HAL_LOCK(hmce);

    /* Check region is not enabled, else error */
    if ((p_region->REGCR & MCE_REGCR_BREN) != MCE_REGCR_BREN)
    {
      /* Enable the region */
      SET_BIT(p_region->REGCR, MCE_REGCR_BREN);

      ret = HAL_OK;
    }

    /* Release Lock */
    __HAL_UNLOCK(hmce);
  }

  return ret;

}


/**
  * @brief  Disable region.
  * @param  hmce pointer to an MCE_HandleTypeDef structure that contains
  *         the configuration information for MCE module
  * @param  RegionIndex index of region that is disabled
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_MCE_DisableRegion(MCE_HandleTypeDef *hmce, uint32_t RegionIndex)
{
  HAL_StatusTypeDef ret = HAL_ERROR;
  MCE_Region_TypeDef *p_region;
  __IO uint32_t address;

  /* Check the parameters */
  assert_param(IS_MCE_ALL_INSTANCE(hmce->Instance));
  assert_param(IS_MCE_REGIONINDEX(RegionIndex));

  address = (__IO uint32_t)((uint32_t)hmce->Instance + 0x40U + (0x10U * RegionIndex));
  p_region = (MCE_Region_TypeDef *)address;

  /* If global lock is set, region cannot be configured */
  if ((hmce->Instance->CR & MCE_CR_GLOCK) != MCE_CR_GLOCK)
  {
    /* Take Lock */
    __HAL_LOCK(hmce);

    /* Check region is enabled, else error */
    if ((p_region->REGCR & MCE_REGCR_BREN) == MCE_REGCR_BREN)
    {
      /* Disable the region */
      CLEAR_BIT(p_region->REGCR, MCE_REGCR_BREN);

      ret = HAL_OK;
    }

    /* Release Lock */
    __HAL_UNLOCK(hmce);
  }

  return ret;

}


/** *************  Lock functions : Lock AES and Noekeon configurations and keys *********/

/**
  * @brief  Lock MCE instance registers configuration
  * @param  hmce pointer to an MCE_HandleTypeDef structure that contains
  *         the configuration information for MCE module
  * @note   Once MCE_CR_GLOCK is set, all writes to MCE registers are ignored,
  *         with the exception of MCE_IACR and MCE_IAIER registers.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_MCE_LockGlobalConfig(MCE_HandleTypeDef *hmce)
{
  /* Check the parameters */
  assert_param(IS_MCE_ALL_INSTANCE(hmce->Instance));

  /* Take Lock */
  __HAL_LOCK(hmce);

  SET_BIT(hmce->Instance->CR, MCE_CR_GLOCK);

  /* Release Lock */
  __HAL_UNLOCK(hmce);

  return HAL_OK;

}

/**
  * @brief  Lock cipher context
  * @param  hmce pointer to an MCE_HandleTypeDef structure that contains
  *         the configuration information for MCE module
  * @param  ContextIndex index of context that is locked
  * @note   Once MCE_CCCFGR_CCLOCK is set, writes to MCE_CCxCFGR and MCE_CCxNR registers
  *         are ignored until next MCE reset.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_MCE_LockAESContextConfig(MCE_HandleTypeDef *hmce, uint32_t ContextIndex)
{
  HAL_StatusTypeDef ret = HAL_ERROR;
  MCE_Context_TypeDef *p_context;
  __IO uint32_t address;

  /* Check the parameters */
  assert_param(IS_MCE_ALL_INSTANCE(hmce->Instance));
  assert_param(IS_MCE_CONTEXT(hmce->Instance, ContextIndex));

  /* If global lock is set or no configuration provided, context cannot be configured */
  if (((hmce->Instance->CR & MCE_CR_GLOCK) != MCE_CR_GLOCK) && \
      ((ContextIndex == MCE_CONTEXT1) || (ContextIndex == MCE_CONTEXT2)))
  {
    address = (__IO uint32_t)((uint32_t)hmce->Instance + 0x240UL + \
                              (0x30UL * ((ContextIndex - MCE_CONTEXT1) >> MCE_REGCR_CTXID_Pos)));
    p_context = (MCE_Context_TypeDef *)address;

    /* Take Lock */
    __HAL_LOCK(hmce);

    SET_BIT(p_context->CCCFGR, MCE_CCCFGR_CCLOCK);

    ret = HAL_OK;

    /* Release Lock */
    __HAL_UNLOCK(hmce);
  }

  return ret;

}

/**
  * @brief  Lock context cipher key
  * @param  hmce pointer to an MCE_HandleTypeDef structure that contains
  *         the configuration information for MCE module
  * @param  ContextIndex index of context the cipher key of which is locked
  * @note   Once MCE_CCCFGR_KEYLOCK is set, writes to MCE_CCxKEYR registers are ignored
  *         until next MCE reset. KEYCRC bitfield value does not change.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_MCE_LockAESContextKey(MCE_HandleTypeDef *hmce, uint32_t ContextIndex)
{
  HAL_StatusTypeDef ret = HAL_ERROR;
  MCE_Context_TypeDef *p_context;
  __IO uint32_t address;

  /* Check the parameters */
  assert_param(IS_MCE_ALL_INSTANCE(hmce->Instance));
  assert_param(IS_MCE_CONTEXT(hmce->Instance, ContextIndex));

  /* If global lock is set or no configuration provided, context cannot be configured */
  if (((hmce->Instance->CR & MCE_CR_GLOCK) != MCE_CR_GLOCK) && \
      ((ContextIndex == MCE_CONTEXT1) || (ContextIndex == MCE_CONTEXT2)))
  {
    address = (__IO uint32_t)((uint32_t)hmce->Instance + 0x240UL + \
                              (0x30UL * ((ContextIndex - MCE_CONTEXT1) >> MCE_REGCR_CTXID_Pos)));
    p_context = (MCE_Context_TypeDef *)address;

    /* Take Lock */
    __HAL_LOCK(hmce);

    SET_BIT(p_context->CCCFGR, MCE_CCCFGR_KEYLOCK);

    ret = HAL_OK;

    /* Release Lock */
    __HAL_UNLOCK(hmce);
  }

  return ret;

}

/**
  * @brief  Lock Noekeon master keys
  * @param  hmce pointer to an MCE_HandleTypeDef structure that contains
  *         the configuration information for MCE module
  * @note   Once MCE_CR_MKLOCK is set, writes to MCE_MKEYRx and MCE_FMKEYRx registers are ignored until next MCE reset.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_MCE_LockNoekeonMasterKeys(MCE_HandleTypeDef *hmce)
{
  HAL_StatusTypeDef ret = HAL_ERROR;

  /* Check the parameters */
  assert_param(IS_MCE_ALL_INSTANCE(hmce->Instance));

  /* If global lock is set, master keys lock cannot be set */
  if ((hmce->Instance->CR & MCE_CR_GLOCK) != MCE_CR_GLOCK)
  {
    /* Take Lock */
    __HAL_LOCK(hmce);

    SET_BIT(hmce->Instance->CR, MCE_CR_MKLOCK);

    ret = HAL_OK;

    /* Release Lock */
    __HAL_UNLOCK(hmce);
  }

  return ret;

}


/**
  * @brief  Initialize the MCE MSP.
  * @param  hmce pointer to an MCE_HandleTypeDef structure that contains
  *         the configuration information for MCE module
  * @retval None
  */
__weak void HAL_MCE_MspInit(MCE_HandleTypeDef *hmce)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hmce);

  /* NOTE : This function should not be modified; when the callback is needed,
            the HAL_MCE_MspInit can be implemented in the user file.
   */
}

/**
  * @brief  DeInitialize MCE MSP.
  * @param  hmce pointer to an MCE_HandleTypeDef structure that contains
  *         the configuration information for MCE module
  * @retval None
  */
__weak void HAL_MCE_MspDeInit(MCE_HandleTypeDef *hmce)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hmce);

  /* NOTE : This function should not be modified; when the callback is needed,
            the HAL_MCE_MspDeInit can be implemented in the user file.
   */
}

/**
  * @brief  get the CRC key of the specified context.
  * @param  hmce pointer to an MCE_HandleTypeDef structure that contains
  *         the configuration information for MCE module
  * @param  pCRCKey pointer to the CRC key
  * @param  ContextIndex index of CONTEXT the CRC key
  * @retval HAL status
  */

HAL_StatusTypeDef HAL_MCE_GetAESContextCRCKey(const MCE_HandleTypeDef *hmce, uint32_t *pCRCKey, uint32_t ContextIndex)
{
  HAL_StatusTypeDef ret = HAL_OK;
  const MCE_Context_TypeDef *p_context;
  __IO uint32_t address;

  /* Check the parameters */
  assert_param(IS_MCE_AES_INSTANCE(hmce->Instance));
  assert_param(IS_MCE_CONTEXT(hmce->Instance, ContextIndex));

  if (*pCRCKey == 0x00U)
  {
    ret = HAL_ERROR;
  }

  address = (__IO uint32_t)((uint32_t)hmce->Instance + 0x240UL + \
                            (0x30UL * ((ContextIndex - MCE_CONTEXT1) >> MCE_REGCR_CTXID_Pos)));
  p_context = (MCE_Context_TypeDef *)address;

  *pCRCKey = READ_REG((p_context->CCCFGR)) & MCE_CCCFGR_KEYCRC;

  *pCRCKey >>= MCE_CCCFGR_KEYCRC_Pos;
  return ret;
}

/**
  * @brief  Compute Key CRC
  * @param  pKey pointer at set of keys
  * @retval CRC value
  */
uint32_t HAL_MCE_KeyCRCComputation(const uint32_t *pKey)
{
  uint8_t crc7_poly = 0x7;
  const uint32_t key_strobe[4] = {0xAA55AA55U, 0x3U, 0x18U, 0xC0U};
  uint8_t  i;
  uint8_t crc = 0;
  uint32_t  j;
  uint32_t  keyval;
  uint32_t k;
  const uint32_t *temp = pKey;

  for (j = 0U; j < 4U; j++)
  {
    keyval = *temp;
    temp++;
    if (j == 0U)
    {
      keyval ^= key_strobe[0];
    }
    else
    {
      keyval ^= (key_strobe[j] << 24) | ((uint32_t)crc << 16) | (key_strobe[j] << 8) | crc;
    }

    crc = 0;
    for (i = 0; i < (uint8_t)32; i++)
    {
      k = ((((uint32_t)crc >> 7) ^ ((keyval >> ((uint8_t)31 - i)) & ((uint8_t)0xF)))) & 1U;
      crc <<= 1;
      if (k != 0U)
      {
        crc ^= crc7_poly;
      }
    }

    crc ^= (uint8_t)0x55;
  }

  return (uint32_t) crc;
}

/**
  * @brief  Handle MCE interrupt request.
  * @param  hmce pointer to an MCE_HandleTypeDef structure that contains
  *         the configuration information for MCE module
  * @retval None
  */
void HAL_MCE_IRQHandler(MCE_HandleTypeDef *hmce)
{
  uint32_t isr_reg;

  isr_reg = READ_REG(hmce->Instance->IASR);
  if ((isr_reg & MCE_IASR_IAEF) == MCE_IASR_IAEF)
  {
    /* Set configuration access error */
    hmce->ErrorCode |= HAL_MCE_CONFIGURATION_ACCESS_ERROR;

    /* Clear configuration access error flag */
    WRITE_REG(hmce->Instance->IACR, MCE_IACR_IAEF);
  }

#if (USE_HAL_MCE_REGISTER_CALLBACKS == 1)
  hmce->ErrorCallback(hmce);
#else
  HAL_MCE_ErrorCallback(hmce);
#endif /* USE_HAL_MCE_REGISTER_CALLBACKS */
}

/**
  * @brief MCE error callback.
  * @param  hmce pointer to an MCE_HandleTypeDef structure that contains
  *         the configuration information for MCE module
  * @retval None
  */
__weak void HAL_MCE_ErrorCallback(MCE_HandleTypeDef *hmce)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hmce);

  /* NOTE : This function should not be modified; when the callback is needed,
            the HAL_MCE_ErrorCallback can be implemented in the user file.
   */
}

/**
  * @brief  Return the MCE state.
  * @param  hmce pointer to an MCE_HandleTypeDef structure that contains
  *         the configuration information for MCE module
  * @retval HAL status
  */
HAL_MCE_StateTypeDef HAL_MCE_GetState(MCE_HandleTypeDef const *hmce)
{
  /* Check the parameters */
  assert_param(IS_MCE_ALL_INSTANCE(hmce->Instance));

  return hmce->State;
}

/**
  * @brief  Return the MCE error code.
  * @param  hmce pointer to an MCE_HandleTypeDef structure that contains
  *         the configuration information for MCE module
  * @retval MCE error code (bitfield on 32 bits)
  */
uint32_t HAL_MCE_GetError(MCE_HandleTypeDef const *hmce)
{
  /* Check the parameters */
  assert_param(IS_MCE_ALL_INSTANCE(hmce->Instance));

  return hmce->ErrorCode;
}


/**
  * @}
  */

/**
  * @}
  */

#endif /* MCE1 */

#endif /* HAL_MCE_MODULE_ENABLED */


/**
  * @}
  */

/**
  * @}
  */
