/**
  ******************************************************************************
  * @file    stm32n6xx_hal_cryp_ex.c
  * @author  MCD Application Team
  * @brief   Extended CRYP HAL module driver
  *          This file provides firmware functions to manage the following
  *          functionalities of CRYP extension peripheral:
  *           + Extended AES processing functions
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
  ==============================================================================
                     ##### How to use this driver #####
  ==============================================================================
    [..]
    The CRYP extension HAL driver can be used after AES-GCM or AES-CCM
    Encryption/Decryption to get the authentication messages.

  @endverbatim
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal.h"

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */
#if defined (CRYP)
/** @defgroup CRYPEx CRYPEx
  * @brief CRYP Extension HAL module driver.
  * @{
  */

#ifdef HAL_CRYP_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/** @addtogroup CRYPEx_Private_Defines
  * @{
  */

#define CRYP_PHASE_INIT                 0x00000000U
#define CRYP_PHASE_HEADER               CRYP_CR_GCM_CCMPH_0
#define CRYP_PHASE_PAYLOAD              CRYP_CR_GCM_CCMPH_1
#define CRYP_PHASE_FINAL                CRYP_CR_GCM_CCMPH

#if !defined(USE_HAL_SAES_ONLY) || (USE_HAL_SAES_ONLY == 1)
#define SAES_PHASE_INIT                  0x00000000U             /*!< GCM/GMAC (or CCM) init phase */
#define SAES_PHASE_HEADER                SAES_CR_GCMPH_0         /*!< GCM/GMAC or CCM header phase */
#define SAES_PHASE_PAYLOAD               SAES_CR_GCMPH_1         /*!< GCM(/CCM) payload phase      */
#define SAES_PHASE_FINAL                 SAES_CR_GCMPH           /*!< GCM/GMAC or CCM  final phase */
#endif /* USE_HAL_SAES_ONLY */

#define  CRYPEx_PHASE_PROCESS       0x02U     /*!< CRYP peripheral is in processing phase */
#define  CRYPEx_PHASE_FINAL         0x03U     /*!< CRYP peripheral is in final phase this is relevant 
                                                   only with CCM and GCM modes */

/*  CTR0 information to use in CCM algorithm */
#define CRYP_CCM_CTR0_0            0x07FFFFFFU
#define CRYP_CCM_CTR0_3            0xFFFFFF00U

/**
  * @}
  */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
#if !defined(USE_HAL_SAES_ONLY) || (USE_HAL_SAES_ONLY == 1)
static HAL_StatusTypeDef CRYPEx_KeyDecrypt(CRYP_HandleTypeDef *hcryp, uint32_t Timeout);
static HAL_StatusTypeDef CRYPEx_KeyEncrypt(CRYP_HandleTypeDef *hcryp, uint32_t Timeout);
static void CRYPEx_SetKey(CRYP_HandleTypeDef *hcryp, uint32_t KeySize);
#endif /* USE_HAL_SAES_ONLY */
/* Exported functions---------------------------------------------------------*/
/** @addtogroup CRYPEx_Exported_Functions
  * @{
  */

/** @defgroup CRYPEx_Exported_Functions_Group1 Extended AES processing functions
  *  @brief    CRYPEx Extended processing functions.
  *
@verbatim
  ==============================================================================
              ##### Extended AES processing functions #####
  ==============================================================================
    [..]  This section provides functions allowing to generate the authentication
          TAG in Polling mode
      (+)HAL_CRYPEx_AESGCM_GenerateAuthTAG
      (+)HAL_CRYPEx_AESCCM_GenerateAuthTAG
         they should be used after Encrypt/Decrypt operation.

@endverbatim
  * @{
  */


/**
  * @brief  generate the GCM authentication TAG.
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  pAuthTag: Pointer to the authentication buffer
  *         the AuthTag generated here is 128bits length, if the TAG length is
  *         less than 128bits, user should consider only the valid part of AuthTag
  *         buffer which correspond exactly to TAG length.
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYPEx_AESGCM_GenerateAuthTAG(CRYP_HandleTypeDef *hcryp, const uint32_t *pAuthTag,
                                                    uint32_t Timeout)
{
  uint32_t tickstart;
  uint64_t headerlength;
  uint64_t inputlength = (uint64_t)hcryp->SizesSum * 8U; /* input length in bits */
  uint32_t tagaddr = (uint32_t)pAuthTag;
  uint8_t i;


  if (hcryp->Init.HeaderWidthUnit == CRYP_HEADERWIDTHUNIT_WORD)
  {
    headerlength = (uint64_t)(hcryp->Init.HeaderSize) * 32U; /* Header length in bits */
  }
  else
  {
    headerlength = (uint64_t)(hcryp->Init.HeaderSize) * 8U;  /* Header length in bits */
  }


  if (hcryp->State == HAL_CRYP_STATE_READY)
  {
    /* Process locked */
    __HAL_LOCK(hcryp);

    /* Change the CRYP peripheral state */
    hcryp->State = HAL_CRYP_STATE_BUSY;

    /* Check if initialization phase has already been performed */
    if (hcryp->Phase == CRYPEx_PHASE_PROCESS)
    {
      /* Change the CRYP phase */
      hcryp->Phase = CRYPEx_PHASE_FINAL;
    }
    else /* Initialization phase has not been performed*/
    {
      /* Disable the Peripheral */
      __HAL_CRYP_DISABLE(hcryp);

      /* Sequence error code field */
      hcryp->ErrorCode |= HAL_CRYP_ERROR_AUTH_TAG_SEQUENCE;

      /* Change the CRYP peripheral state */
      hcryp->State = HAL_CRYP_STATE_READY;

      /* Process unlocked */
      __HAL_UNLOCK(hcryp);
      return HAL_ERROR;
    }

    /* Select final phase */
#if !defined(USE_HAL_CRYP_ONLY) || (USE_HAL_CRYP_ONLY == 1)
    if (IS_CRYP_INSTANCE(hcryp->Instance))
    {
      /* Disable CRYP to start the final phase */
      __HAL_CRYP_DISABLE(hcryp);

      /* ALGODIR bit must be set to '0'. */
      MODIFY_REG(((CRYP_TypeDef *)(hcryp->Instance))->CR, CRYP_CR_GCM_CCMPH, CRYP_PHASE_FINAL);
      ((CRYP_TypeDef *)(hcryp->Instance))->CR &= ~CRYP_CR_ALGODIR;

      /* Enable the CRYP peripheral */
      __HAL_CRYP_ENABLE(hcryp);

      /* Write the number of bits in header (64 bits) followed by the number of bits
      in the payload */
      ((CRYP_TypeDef *)(hcryp->Instance))->DIN = 0U;
      ((CRYP_TypeDef *)(hcryp->Instance))->DIN = (uint32_t)(headerlength);
      ((CRYP_TypeDef *)(hcryp->Instance))->DIN = 0U;
      ((CRYP_TypeDef *)(hcryp->Instance))->DIN = (uint32_t)(inputlength);

      /* Wait for OFNE flag to be raised */
      tickstart = HAL_GetTick();
      while (HAL_IS_BIT_CLR(((CRYP_TypeDef *)(hcryp->Instance))->SR, CRYP_FLAG_OFNE))
      {
        /* Check for the Timeout */
        if (Timeout != HAL_MAX_DELAY)
        {
          if (((HAL_GetTick() - tickstart) > Timeout) || (Timeout == 0U))
          {
            /* Disable the CRYP Peripheral Clock */
            __HAL_CRYP_DISABLE(hcryp);

            /* Change state */
            hcryp->ErrorCode |= HAL_CRYP_ERROR_TIMEOUT;
            hcryp->State = HAL_CRYP_STATE_READY;

            /* Process unlocked */
            __HAL_UNLOCK(hcryp);
            return HAL_ERROR;
          }
        }
      }
      /* Read the authentication TAG in the output FIFO */
      for (i = 0U; i < 4U; i++)
      {
        *(uint32_t *)(tagaddr) = ((CRYP_TypeDef *)(hcryp->Instance))->DOUT;
        tagaddr += 4U;
      }
    }
#endif /* USE_HAL_CRYP_ONLY */
#if !defined(USE_HAL_SAES_ONLY) || (USE_HAL_SAES_ONLY == 1)
    if (IS_SAES_INSTANCE(hcryp->Instance))
    {
      /* Select final phase */
      MODIFY_REG(((SAES_TypeDef *)(hcryp->Instance))->CR, SAES_CR_GCMPH, SAES_PHASE_FINAL);

      /* Write the number of bits in header (64 bits) followed by the number of bits
      in the payload */
      ((SAES_TypeDef *)(hcryp->Instance))->DINR = 0U;
      ((SAES_TypeDef *)(hcryp->Instance))->DINR = (uint32_t)(headerlength);
      ((SAES_TypeDef *)(hcryp->Instance))->DINR = 0U;
      ((SAES_TypeDef *)(hcryp->Instance))->DINR = (uint32_t)(inputlength);

      /* Wait for CCF flag to be raised */
      tickstart = HAL_GetTick();
      while (HAL_IS_BIT_CLR(((SAES_TypeDef *)(hcryp->Instance))->SR, SAES_SR_CCF))
      {
        /* Check for the Timeout */
        if (Timeout != HAL_MAX_DELAY)
        {
          if (((HAL_GetTick() - tickstart) > Timeout) || (Timeout == 0U))
          {
            /* Disable the CRYP peripheral Clock */
            __HAL_CRYP_DISABLE(hcryp);

            /* Change state */
            hcryp->ErrorCode |= HAL_CRYP_ERROR_TIMEOUT;
            hcryp->State = HAL_CRYP_STATE_READY;
            __HAL_UNLOCK(hcryp);
            return HAL_ERROR;
          }
        }
      }

      /* Read the authentication TAG in the output FIFO */
      for (i = 0U; i < 4U; i++)
      {
        *(uint32_t *)(tagaddr) = ((SAES_TypeDef *)(hcryp->Instance))->DOUTR;
        tagaddr += 4U;
      }

      /* Clear CCF Flag */
      __HAL_CRYP_CLEAR_FLAG(hcryp, CRYP_CLEAR_CCF);
    }
#endif /* USE_HAL_SAES_ONLY */

    /* Disable the peripheral */
    __HAL_CRYP_DISABLE(hcryp);

    /* Change the CRYP peripheral state */
    hcryp->State = HAL_CRYP_STATE_READY;

    /* Process unlocked */
    __HAL_UNLOCK(hcryp);
  }
  else
  {
    /* Busy error code field */
    hcryp->ErrorCode |= HAL_CRYP_ERROR_BUSY;
    return HAL_ERROR;
  }
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  AES CCM Authentication TAG generation.
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  pAuthTag: Pointer to the authentication buffer
  *         the AuthTag generated here is 128bits length, if the TAG length is
  *         less than 128bits, user should consider only the valid part of AuthTag
  *         buffer which correspond exactly to TAG length.
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYPEx_AESCCM_GenerateAuthTAG(CRYP_HandleTypeDef *hcryp, const uint32_t *pAuthTag,
                                                    uint32_t Timeout)
{
  uint32_t tagaddr = (uint32_t)pAuthTag;
  uint32_t ctr0 [4] = {0};
  uint32_t ctr0addr = (uint32_t)ctr0;
  uint32_t tickstart;
  uint8_t i;

  if (hcryp->State == HAL_CRYP_STATE_READY)
  {
    /* Process locked */
    __HAL_LOCK(hcryp);
#if !defined(USE_HAL_SAES_ONLY) || (USE_HAL_SAES_ONLY == 1)
    if (IS_SAES_INSTANCE(hcryp->Instance))
    {
      /* Disable interrupts, we are now in polling mode to TAG generation */
      __HAL_CRYP_DISABLE_IT(hcryp, CRYP_IT_CCFIE | CRYP_IT_RWEIE | CRYP_IT_KEIE);
    }
#endif /* USE_HAL_SAES_ONLY */
    /* Change the CRYP peripheral state */
    hcryp->State = HAL_CRYP_STATE_BUSY;

    /* Check if initialization phase has already been performed */
    if (hcryp->Phase == CRYPEx_PHASE_PROCESS)
    {
      /* Change the CRYP phase */
      hcryp->Phase = CRYPEx_PHASE_FINAL;
    }
    else /* Initialization phase has not been performed*/
    {
      /* Disable the peripheral */
      __HAL_CRYP_DISABLE(hcryp);

      /* Sequence error code field */
      hcryp->ErrorCode |= HAL_CRYP_ERROR_AUTH_TAG_SEQUENCE;

      /* Change the CRYP peripheral state */
      hcryp->State = HAL_CRYP_STATE_READY;

      /* Process unlocked */
      __HAL_UNLOCK(hcryp);
      return HAL_ERROR;
    }

    /* Select final phase */
#if !defined(USE_HAL_CRYP_ONLY) || (USE_HAL_CRYP_ONLY == 1)
    if (IS_CRYP_INSTANCE(hcryp->Instance))
    {
      /* Disable CRYP to start the final phase */
      __HAL_CRYP_DISABLE(hcryp);

      /* ALGODIR bit must be set to '0' */
      MODIFY_REG(((CRYP_TypeDef *)(hcryp->Instance))->CR, CRYP_CR_GCM_CCMPH, CRYP_PHASE_FINAL);
      ((CRYP_TypeDef *)(hcryp->Instance))->CR &= ~CRYP_CR_ALGODIR;

      /* Write the counter block in the IN FIFO, CTR0 information from B0
      data has to be swapped according to the DATATYPE*/
      ctr0[0] = hcryp->Init.B0[0] & CRYP_CCM_CTR0_0;
      ctr0[1] = hcryp->Init.B0[1];
      ctr0[2] = hcryp->Init.B0[2];
      ctr0[3] = hcryp->Init.B0[3] & CRYP_CCM_CTR0_3;

      for (i = 0U; i < 4U; i++)
      {
        ((CRYP_TypeDef *)(hcryp->Instance))->DIN = *(uint32_t *)(ctr0addr);
        ctr0addr += 4U;
      }
      /* Enable the CRYP peripheral */
      __HAL_CRYP_ENABLE(hcryp);

      /* Wait for OFNE flag to be raised */
      tickstart = HAL_GetTick();
      while (HAL_IS_BIT_CLR(((CRYP_TypeDef *)((hcryp->Instance)))->SR, CRYP_FLAG_OFNE))
      {
        /* Check for the Timeout */
        if (Timeout != HAL_MAX_DELAY)
        {
          if (((HAL_GetTick() - tickstart) > Timeout) || (Timeout == 0U))
          {
            /* Disable the CRYP peripheral Clock */
            __HAL_CRYP_DISABLE(hcryp);

            /* Change state */
            hcryp->ErrorCode |= HAL_CRYP_ERROR_TIMEOUT;
            hcryp->State = HAL_CRYP_STATE_READY;

            /* Process unlocked */
            __HAL_UNLOCK(hcryp);
            return HAL_ERROR;
          }
        }
      }
      /* Read the authentication TAG in the output FIFO */
      for (i = 0U; i < 4U; i++)
      {
        *(uint32_t *)(tagaddr) = ((CRYP_TypeDef *)(hcryp->Instance))->DOUT;
        tagaddr += 4U;
      }
    }
#endif /* USE_HAL_CRYP_ONLY */
#if !defined(USE_HAL_SAES_ONLY) || (USE_HAL_SAES_ONLY == 1)
    if (IS_SAES_INSTANCE(hcryp->Instance))
    {
      /* Change SAES final phase */
      MODIFY_REG(((SAES_TypeDef *)(hcryp->Instance))->CR, SAES_CR_GCMPH, SAES_PHASE_FINAL);

      for (i = 0U; i < 4U; i++)
      {
        ((SAES_TypeDef *)(hcryp->Instance))->DINR = *(uint32_t *)(ctr0addr);
        ctr0addr += 4U;
      }

      /* Wait for CCF flag to be raised */
      tickstart = HAL_GetTick();
      while (HAL_IS_BIT_CLR(((SAES_TypeDef *)(hcryp->Instance))->SR, CRYP_FLAG_CCF))
      {
        /* Check for the Timeout */
        if (Timeout != HAL_MAX_DELAY)
        {
          if (((HAL_GetTick() - tickstart) > Timeout) || (Timeout == 0U))
          {
            /* Disable the CRYP peripheral Clock */
            __HAL_CRYP_DISABLE(hcryp);

            /* Change state */
            hcryp->ErrorCode |= HAL_CRYP_ERROR_TIMEOUT;
            hcryp->State = HAL_CRYP_STATE_READY;

            /* Process unlocked */
            __HAL_UNLOCK(hcryp);
            return HAL_ERROR;
          }
        }
      }
      /* Read the authentication TAG in the output FIFO */
      for (i = 0U; i < 4U; i++)
      {
        *(uint32_t *)(tagaddr) = ((SAES_TypeDef *)(hcryp->Instance))->DOUTR;
        tagaddr += 4U;
      }
      /* Clear CCF flag */
      __HAL_CRYP_CLEAR_FLAG(hcryp, CRYP_CLEAR_CCF);
    }
#endif /* USE_HAL_SAES_ONLY */

    /* Change the CRYP peripheral state */
    hcryp->State = HAL_CRYP_STATE_READY;

    /* Process unlocked */
    __HAL_UNLOCK(hcryp);

    /* Disable CRYP  */
    __HAL_CRYP_DISABLE(hcryp);
  }
  else
  {
    /* Busy error code field */
    hcryp->ErrorCode = HAL_CRYP_ERROR_BUSY;
    return HAL_ERROR;
  }
  /* Return function status */
  return HAL_OK;
}


/** @defgroup CRYPEx_Exported_Functions_Group2 Wrap and Unwrap key functions
  * @brief    Wrap and Unwrap key functions.
  *
@verbatim
  ==============================================================================
                      ##### Wrap and Unwrap key #####
  ==============================================================================
    [..]  This section provides API allowing to wrap (encrypt) and unwrap (decrypt)
          key using one of the following keys, and AES Algorithm.
          Key selection :
           - Derived hardware unique key (DHUK)
           - XOR of DHUK and BHK
           - Boot hardware key (BHK)
           - Key registers AES_KEYx

@endverbatim
  * @{
  */

#if !defined(USE_HAL_SAES_ONLY) || (USE_HAL_SAES_ONLY == 1)
/**
  * @brief  Wrap (encrypt) application keys .
  * @param  hcryp pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  Input Pointer to the Key buffer to encrypt
  * @param  Output Pointer to the Key buffer encrypted
  * @param  Timeout Specify Timeout value
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYPEx_WrapKey(CRYP_HandleTypeDef *hcryp, uint32_t *Input, uint32_t *Output, uint32_t Timeout)
{
  HAL_StatusTypeDef status;

  if (hcryp->State == HAL_CRYP_STATE_READY)
  {
    /* Change state Busy */
    hcryp->State = HAL_CRYP_STATE_BUSY;

    /* Process locked */
    __HAL_LOCK(hcryp);

    /*  Reset CrypInCount, CrypOutCount and Initialize pCrypInBuffPtr and pCrypOutBuffPtr parameters*/
    hcryp->CrypInCount = 0U;
    hcryp->CrypOutCount = 0U;
    hcryp->pCrypInBuffPtr = Input;
    hcryp->pCrypOutBuffPtr = Output;

    /* Disable the CRYP peripheral clock */
    __HAL_CRYP_DISABLE(hcryp);

    /* Set the operating mode*/
    MODIFY_REG(((SAES_TypeDef *)(hcryp->Instance))->CR, SAES_CR_KMOD, CRYP_KEYMODE_WRAPPED);

    status = CRYPEx_KeyEncrypt(hcryp, Timeout);

    if (status == HAL_OK)
    {
      /* Change the CRYP peripheral state */
      hcryp->State = HAL_CRYP_STATE_READY;

      /* Process unlocked */
      __HAL_UNLOCK(hcryp);
    }
  }
  else
  {
    /* Busy error code field */
    hcryp->ErrorCode |= HAL_CRYP_ERROR_BUSY;
    status = HAL_ERROR;
  }
  /* Return function status */
  return status;
}

/**
  * @brief  Unwrap (Decrypt) application keys .
  * @param  hcryp pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  Input Pointer to the Key buffer to decrypt
  * @param  Timeout Specify Timeout value
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYPEx_UnwrapKey(CRYP_HandleTypeDef *hcryp, uint32_t *Input, uint32_t Timeout)
{
  HAL_StatusTypeDef status;

  if (hcryp->State == HAL_CRYP_STATE_READY)
  {
    /* Change state Busy */
    hcryp->State = HAL_CRYP_STATE_BUSY;

    /* Process locked */
    __HAL_LOCK(hcryp);

    /*  Reset CrypInCount, CrypOutCount and Initialize pCrypInBuffPtr and pCrypOutBuffPtr parameters*/
    hcryp->CrypInCount = 0U;
    hcryp->CrypOutCount = 0U;
    hcryp->pCrypInBuffPtr = Input;

    /* Disable the CRYP peripheral clock */
    __HAL_CRYP_DISABLE(hcryp);

    /* Set the operating mode*/
    MODIFY_REG(((SAES_TypeDef *)(hcryp->Instance))->CR, SAES_CR_KMOD, CRYP_KEYMODE_WRAPPED);

    status = CRYPEx_KeyDecrypt(hcryp, Timeout);

    if (status == HAL_OK)
    {
      /* Change the CRYP peripheral state */
      hcryp->State = HAL_CRYP_STATE_READY;

      /* Process unlocked */
      __HAL_UNLOCK(hcryp);
    }
  }
  else
  {
    /* Busy error code field */
    hcryp->ErrorCode |= HAL_CRYP_ERROR_BUSY;
    status = HAL_ERROR;
  }
  /* Return function status */
  return status;
}

/**
  * @}
  */

/** @defgroup CRYPEx_Exported_Functions_Group3 Encrypt/Decrypt Shared key functions
  * @brief    Encrypt/Decrypt Shared key functions.
  *
@verbatim
  ==============================================================================
                      ##### Encrypt/Decrypt Shared key functions #####
  ==============================================================================
    [..]  This section provides API allowing to Encrypt/Decrypt Shared key

@endverbatim
  * @{
  */

/**
  * @brief  Encrypt Shared key.
  * @param  hcryp pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  Key Pointer to the Key buffer to share
  * @param  Output buffer pointer
  * @param  ID Key share identification
  * @param  Timeout Specify Timeout value
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYPEx_EncryptSharedKey(CRYP_HandleTypeDef *hcryp, uint32_t *Key, uint32_t *Output, uint32_t ID,
                                              uint32_t Timeout)
{
  HAL_StatusTypeDef status;

  if (hcryp->State == HAL_CRYP_STATE_READY)
  {
    /* Change state Busy */
    hcryp->State = HAL_CRYP_STATE_BUSY;

    /* Process locked */
    __HAL_LOCK(hcryp);

    /* Reset CrypInCount, CrypOutCount and Initialize pCrypInBuffPtr and pCrypOutBuffPtr parameters*/
    hcryp->CrypInCount = 0U;
    hcryp->CrypOutCount = 0U;
    hcryp->pCrypInBuffPtr = Key;
    hcryp->pCrypOutBuffPtr = Output;

    /* Disable the CRYP peripheral clock */
    __HAL_CRYP_DISABLE(hcryp);

    /* Set the operating mode*/
    MODIFY_REG(((SAES_TypeDef *)(hcryp->Instance))->CR, SAES_CR_KMOD | SAES_CR_KSHAREID, CRYP_KEYMODE_SHARED | ID);

    status = CRYPEx_KeyEncrypt(hcryp, Timeout);

    if (status == HAL_OK)
    {
      /* Change the CRYP peripheral state */
      hcryp->State = HAL_CRYP_STATE_READY;

      /* Process unlocked */
      __HAL_UNLOCK(hcryp);
    }
  }
  else
  {
    /* Busy error code field */
    hcryp->ErrorCode |= HAL_CRYP_ERROR_BUSY;
    status = HAL_ERROR;
  }
  /* Return function status */
  return status;
}

/**
  * @brief  Decrypt Shared key.
  * @param  hcryp pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  Key Pointer to the Key buffer to share
  * @param  ID Key share identification
  * @param  Timeout Specify Timeout value
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYPEx_DecryptSharedKey(CRYP_HandleTypeDef *hcryp, uint32_t *Key, uint32_t ID, uint32_t Timeout)
{
  HAL_StatusTypeDef status;

  if (hcryp->State == HAL_CRYP_STATE_READY)
  {
    /* Change state Busy */
    hcryp->State = HAL_CRYP_STATE_BUSY;

    /* Process locked */
    __HAL_LOCK(hcryp);

    /*  Reset CrypInCount, CrypOutCount and Initialize pCrypInBuffPtr and pCrypOutBuffPtr parameters*/
    hcryp->CrypInCount = 0U;
    hcryp->CrypOutCount = 0U;
    hcryp->pCrypInBuffPtr = Key;

    /* Disable the CRYP peripheral clock */
    __HAL_CRYP_DISABLE(hcryp);

    /* Set the operating mode*/
    MODIFY_REG(((SAES_TypeDef *)(hcryp->Instance))->CR, SAES_CR_KMOD | SAES_CR_KSHAREID, CRYP_KEYMODE_SHARED | ID);

    status = CRYPEx_KeyDecrypt(hcryp, Timeout);

    if (status == HAL_OK)
    {
      /* Change the CRYP peripheral state */
      hcryp->State = HAL_CRYP_STATE_READY;

      /* Process unlocked */
      __HAL_UNLOCK(hcryp);
    }
  }
  else
  {
    /* Busy error code field */
    hcryp->ErrorCode |= HAL_CRYP_ERROR_BUSY;
    status = HAL_ERROR;
  }
  /* Return function status */
  return status;
}

/**
  * @brief  Key Decryption
  * @param  hcryp pointer to a CRYP_HandleTypeDef structure
  * @param  Timeout specify Timeout value
  * @note   It is strongly recommended to select hardware secret keys
  * @retval HAL status
  */
static HAL_StatusTypeDef CRYPEx_KeyDecrypt(CRYP_HandleTypeDef *hcryp, uint32_t Timeout)
{
  uint32_t incount;  /* Temporary CrypInCount Value */
  uint32_t tickstart;

  /* key preparation for decryption, operating mode 2*/
  MODIFY_REG(((SAES_TypeDef *)(hcryp->Instance))->CR, SAES_CR_MODE, CRYP_MODE_KEY_DERIVATION);

  /*It is strongly recommended to select hardware secret keys*/
  if (hcryp->Init.KeySelect == CRYP_KEYSEL_NORMAL)
  {
    /*  Set the Key*/
    CRYPEx_SetKey(hcryp, hcryp->Init.KeySize);
  }
  /* Enable CRYP */
  __HAL_CRYP_ENABLE(hcryp);

  /* Wait for CCF flag to be raised */
  tickstart = HAL_GetTick();
  while (HAL_IS_BIT_CLR(((SAES_TypeDef *)(hcryp->Instance))->SR, SAES_SR_CCF))
  {
    /* Check for the Timeout */
    if (Timeout != HAL_MAX_DELAY)
    {
      if (((HAL_GetTick() - tickstart) > Timeout) || (Timeout == 0U))
      {
        /* Disable the CRYP peripheral clock */
        __HAL_CRYP_DISABLE(hcryp);

        /* Change state */
        hcryp->ErrorCode |= HAL_CRYP_ERROR_TIMEOUT;
        hcryp->State = HAL_CRYP_STATE_READY;

        /* Process unlocked */
        __HAL_UNLOCK(hcryp);
        return HAL_ERROR;
      }
    }
  }
  /* Clear CCF Flag */
  __HAL_CRYP_CLEAR_FLAG(hcryp, CRYP_CLEAR_CCF);

  /*  End of Key preparation for ECB/CBC */
  /* Return to decryption operating mode(Mode 3)*/
  MODIFY_REG(((SAES_TypeDef *)(hcryp->Instance))->CR, SAES_CR_MODE, CRYP_MODE_DECRYPT);


  if (hcryp->Init.Algorithm != CRYP_AES_ECB)
  {
    /* Set the Initialization Vector*/
    ((SAES_TypeDef *)(hcryp->Instance))->IVR3 = *(uint32_t *)(hcryp->Init.pInitVect);
    ((SAES_TypeDef *)(hcryp->Instance))->IVR2 = *(uint32_t *)(hcryp->Init.pInitVect + 1U);
    ((SAES_TypeDef *)(hcryp->Instance))->IVR1 = *(uint32_t *)(hcryp->Init.pInitVect + 2U);
    ((SAES_TypeDef *)(hcryp->Instance))->IVR0 = *(uint32_t *)(hcryp->Init.pInitVect + 3U);
  }
  /* Enable CRYP */
  __HAL_CRYP_ENABLE(hcryp);

  /* Set the phase */
  hcryp->Phase = CRYPEx_PHASE_PROCESS;

  if (hcryp->Init.KeySize == CRYP_KEYSIZE_128B)
  {
    incount = 4;
  }
  else
  {
    incount = 8;
  }
  while (hcryp->CrypInCount < incount)
  {
    /* Write four times to input the key to encrypt */
    ((SAES_TypeDef *)(hcryp->Instance))->DINR  = *(uint32_t *)(hcryp->pCrypInBuffPtr + hcryp->CrypInCount);
    hcryp->CrypInCount++;
    ((SAES_TypeDef *)(hcryp->Instance))->DINR  = *(uint32_t *)(hcryp->pCrypInBuffPtr + hcryp->CrypInCount);
    hcryp->CrypInCount++;
    ((SAES_TypeDef *)(hcryp->Instance))->DINR  = *(uint32_t *)(hcryp->pCrypInBuffPtr + hcryp->CrypInCount);
    hcryp->CrypInCount++;
    ((SAES_TypeDef *)(hcryp->Instance))->DINR  = *(uint32_t *)(hcryp->pCrypInBuffPtr + hcryp->CrypInCount);
    hcryp->CrypInCount++;

    /* Wait for CCF flag to be raised */
    tickstart = HAL_GetTick();
    while (HAL_IS_BIT_CLR(((SAES_TypeDef *)(hcryp->Instance))->SR, SAES_SR_CCF))
    {
      /* Check for the Timeout */
      if (Timeout != HAL_MAX_DELAY)
      {
        if (((HAL_GetTick() - tickstart) > Timeout) || (Timeout == 0U))
        {
          /* Disable the CRYP peripheral clock */
          __HAL_CRYP_DISABLE(hcryp);

          /* Change state */
          hcryp->ErrorCode |= HAL_CRYP_ERROR_TIMEOUT;
          hcryp->State = HAL_CRYP_STATE_READY;

          /* Process unlocked */
          __HAL_UNLOCK(hcryp);
          return HAL_ERROR;
        }
      }
    }

    /* Clear CCF Flag */
    __HAL_CRYP_CLEAR_FLAG(hcryp, CRYP_CLEAR_CCF);
  }

  /* Disable the CRYP peripheral clock */
  __HAL_CRYP_DISABLE(hcryp);

  return HAL_OK;
}

/**
  * @brief  Key Encryption
  * @param  hcryp pointer to a CRYP_HandleTypeDef structure
  * @param  Timeout specify Timeout value
  * @retval HAL status
  */
static HAL_StatusTypeDef CRYPEx_KeyEncrypt(CRYP_HandleTypeDef *hcryp, uint32_t Timeout)
{
  uint32_t incount;  /* Temporary CrypInCount Value */
  uint32_t tickstart;
  uint32_t temp;  /* Temporary CrypOutBuff */

  MODIFY_REG(((SAES_TypeDef *)(hcryp->Instance))->CR, SAES_CR_MODE, CRYP_OPERATINGMODE_ENCRYPT);

  if (hcryp->Init.Algorithm != CRYP_AES_ECB)
  {
    /* Set the Initialization Vector*/
    ((SAES_TypeDef *)(hcryp->Instance))->IVR3 = *(uint32_t *)(hcryp->Init.pInitVect);
    ((SAES_TypeDef *)(hcryp->Instance))->IVR2 = *(uint32_t *)(hcryp->Init.pInitVect + 1U);
    ((SAES_TypeDef *)(hcryp->Instance))->IVR1 = *(uint32_t *)(hcryp->Init.pInitVect + 2U);
    ((SAES_TypeDef *)(hcryp->Instance))->IVR0 = *(uint32_t *)(hcryp->Init.pInitVect + 3U);
  }
  /*It is strongly recommended to select hardware secret keys*/
  if (hcryp->Init.KeySelect == CRYP_KEYSEL_NORMAL)
  {
    /*  Set the Key*/
    CRYPEx_SetKey(hcryp, hcryp->Init.KeySize);
  }
  /* Get tick */
  tickstart = HAL_GetTick();

  /* Wait for Valid KEY flag to set  */
  while (HAL_IS_BIT_CLR(((SAES_TypeDef *)(hcryp->Instance))->SR, SAES_SR_KEYVALID))
  {
    /* Check for the Timeout */
    if (Timeout != HAL_MAX_DELAY)
    {
      if (((HAL_GetTick() - tickstart) > Timeout) || (Timeout == 0U))
      {
        /* Change state */
        hcryp->ErrorCode |= HAL_CRYP_ERROR_TIMEOUT;
        hcryp->State = HAL_CRYP_STATE_READY;
        /* Process unlocked */
        return HAL_ERROR;
      }
    }
  }

  /* Enable CRYP */
  __HAL_CRYP_ENABLE(hcryp);

  /* Set the phase */
  hcryp->Phase = CRYPEx_PHASE_PROCESS;

  if (hcryp->Init.KeySize == CRYP_KEYSIZE_128B)
  {
    incount = 4;
  }
  else
  {
    incount = 8;
  }
  while (hcryp->CrypInCount < incount)
  {
    /* Write four times to input the key to encrypt */
    ((SAES_TypeDef *)(hcryp->Instance))->DINR  = *(uint32_t *)(hcryp->pCrypInBuffPtr + hcryp->CrypInCount);
    hcryp->CrypInCount++;
    ((SAES_TypeDef *)(hcryp->Instance))->DINR  = *(uint32_t *)(hcryp->pCrypInBuffPtr + hcryp->CrypInCount);
    hcryp->CrypInCount++;
    ((SAES_TypeDef *)(hcryp->Instance))->DINR  = *(uint32_t *)(hcryp->pCrypInBuffPtr + hcryp->CrypInCount);
    hcryp->CrypInCount++;
    ((SAES_TypeDef *)(hcryp->Instance))->DINR  = *(uint32_t *)(hcryp->pCrypInBuffPtr + hcryp->CrypInCount);
    hcryp->CrypInCount++;

    /* Wait for CCF flag to be raised */
    tickstart = HAL_GetTick();
    while (HAL_IS_BIT_CLR(((SAES_TypeDef *)(hcryp->Instance))->SR, SAES_SR_CCF))
    {
      /* Check for the Timeout */
      if (Timeout != HAL_MAX_DELAY)
      {
        if (((HAL_GetTick() - tickstart) > Timeout) || (Timeout == 0U))
        {
          /* Disable the CRYP peripheral clock */
          __HAL_CRYP_DISABLE(hcryp);

          /* Change state */
          hcryp->ErrorCode |= HAL_CRYP_ERROR_TIMEOUT;
          hcryp->State = HAL_CRYP_STATE_READY;

          /* Process unlocked */
          __HAL_UNLOCK(hcryp);
          return HAL_ERROR;
        }
      }
    }

    /* Clear CCF Flag */
    __HAL_CRYP_CLEAR_FLAG(hcryp, CRYP_CLEAR_CCF);

    /* Read the output block from the output FIFO and put them in temporary buffer then
       get CrypOutBuff from temporary buffer*/
    temp  = ((SAES_TypeDef *)(hcryp->Instance))->DOUTR;
    *(uint32_t *)(hcryp->pCrypOutBuffPtr + (hcryp->CrypOutCount)) = temp;
    hcryp->CrypOutCount++;
    temp  = ((SAES_TypeDef *)(hcryp->Instance))->DOUTR;
    *(uint32_t *)(hcryp->pCrypOutBuffPtr + hcryp->CrypOutCount)   = temp;
    hcryp->CrypOutCount++;
    temp  = ((SAES_TypeDef *)(hcryp->Instance))->DOUTR;
    *(uint32_t *)(hcryp->pCrypOutBuffPtr + (hcryp->CrypOutCount)) = temp;
    hcryp->CrypOutCount++;
    temp  = ((SAES_TypeDef *)(hcryp->Instance))->DOUTR;
    *(uint32_t *)(hcryp->pCrypOutBuffPtr + hcryp->CrypOutCount)   = temp;
    hcryp->CrypOutCount++;
  }

  /* Disable the CRYP peripheral clock */
  __HAL_CRYP_DISABLE(hcryp);

  return HAL_OK;
}

/**
  * @brief  Write Key in Key registers.
  * @param  hcryp pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  KeySize Size of Key
  * @note   If pKey is NULL, the Key registers are not written.
  * @retval None
  */
static void CRYPEx_SetKey(CRYP_HandleTypeDef *hcryp, uint32_t KeySize)
{
  if (hcryp->Init.pKey != NULL)
  {
    switch (KeySize)
    {
      case CRYP_KEYSIZE_256B:
        ((SAES_TypeDef *)(hcryp->Instance))->KEYR7 = *(uint32_t *)(hcryp->Init.pKey);
        ((SAES_TypeDef *)(hcryp->Instance))->KEYR6 = *(uint32_t *)(hcryp->Init.pKey + 1U);
        ((SAES_TypeDef *)(hcryp->Instance))->KEYR5 = *(uint32_t *)(hcryp->Init.pKey + 2U);
        ((SAES_TypeDef *)(hcryp->Instance))->KEYR4 = *(uint32_t *)(hcryp->Init.pKey + 3U);
        ((SAES_TypeDef *)(hcryp->Instance))->KEYR3 = *(uint32_t *)(hcryp->Init.pKey + 4U);
        ((SAES_TypeDef *)(hcryp->Instance))->KEYR2 = *(uint32_t *)(hcryp->Init.pKey + 5U);
        ((SAES_TypeDef *)(hcryp->Instance))->KEYR1 = *(uint32_t *)(hcryp->Init.pKey + 6U);
        ((SAES_TypeDef *)(hcryp->Instance))->KEYR0 = *(uint32_t *)(hcryp->Init.pKey + 7U);
        break;
      case CRYP_KEYSIZE_128B:
        ((SAES_TypeDef *)(hcryp->Instance))->KEYR3 = *(uint32_t *)(hcryp->Init.pKey);
        ((SAES_TypeDef *)(hcryp->Instance))->KEYR2 = *(uint32_t *)(hcryp->Init.pKey + 1U);
        ((SAES_TypeDef *)(hcryp->Instance))->KEYR1 = *(uint32_t *)(hcryp->Init.pKey + 2U);
        ((SAES_TypeDef *)(hcryp->Instance))->KEYR0 = *(uint32_t *)(hcryp->Init.pKey + 3U);

        break;
      default:
        break;
    }
  }
}

/**
  * @}
  */
#endif /* USE_HAL_SAES_ONLY */

/**
  * @}
  */

/**
  * @}
  */

#endif /* HAL_CRYP_MODULE_ENABLED */

/**
  * @}
  */

#endif /* CRYP */

/**
  * @}
  */
