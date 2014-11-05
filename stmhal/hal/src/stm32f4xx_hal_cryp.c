/**
  ******************************************************************************
  * @file    stm32f4xx_hal_cryp.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-June-2014
  * @brief   CRYP HAL module driver.
  *          This file provides firmware functions to manage the following 
  *          functionalities of the Cryptography (CRYP) peripheral:
  *           + Initialization and de-initialization functions
  *           + AES processing functions
  *           + DES processing functions
  *           + TDES processing functions
  *           + DMA callback functions
  *           + CRYP IRQ handler management
  *           + Peripheral State functions
  *
  @verbatim
  ==============================================================================
                     ##### How to use this driver #####
  ==============================================================================
    [..]
      The CRYP HAL driver can be used as follows:

      (#)Initialize the CRYP low level resources by implementing the HAL_CRYP_MspInit():
         (##) Enable the CRYP interface clock using __CRYP_CLK_ENABLE()
         (##) In case of using interrupts (e.g. HAL_CRYP_AESECB_Encrypt_IT())
             (+++) Configure the CRYP interrupt priority using HAL_NVIC_SetPriority()
             (+++) Enable the CRYP IRQ handler using HAL_NVIC_EnableIRQ()
             (+++) In CRYP IRQ handler, call HAL_CRYP_IRQHandler()
         (##) In case of using DMA to control data transfer (e.g. HAL_CRYP_AESECB_Encrypt_DMA())
             (+++) Enable the DMAx interface clock using __DMAx_CLK_ENABLE()
             (+++) Configure and enable two DMA streams one for managing data transfer from
                 memory to peripheral (input stream) and another stream for managing data
                 transfer from peripheral to memory (output stream)
             (+++) Associate the initilalized DMA handle to the CRYP DMA handle
                 using  __HAL_LINKDMA()
             (+++) Configure the priority and enable the NVIC for the transfer complete
                 interrupt on the two DMA Streams. The output stream should have higher
                 priority than the input stream HAL_NVIC_SetPriority() and HAL_NVIC_EnableIRQ()
    
      (#)Initialize the CRYP HAL using HAL_CRYP_Init(). This function configures mainly:
         (##) The data type: 1-bit, 8-bit, 16-bit and 32-bit
         (##) The key size: 128, 192 and 256. This parameter is relevant only for AES
         (##) The encryption/decryption key. It's size depends on the algorithm
              used for encryption/decryption
         (##) The initialization vector (counter). It is not used ECB mode.
    
      (#)Three processing (encryption/decryption) functions are available:
         (##) Polling mode: encryption and decryption APIs are blocking functions
              i.e. they process the data and wait till the processing is finished,
              e.g. HAL_CRYP_AESCBC_Encrypt()
         (##) Interrupt mode: encryption and decryption APIs are not blocking functions
              i.e. they process the data under interrupt,
              e.g. HAL_CRYP_AESCBC_Encrypt_IT()
         (##) DMA mode: encryption and decryption APIs are not blocking functions
              i.e. the data transfer is ensured by DMA,
              e.g. HAL_CRYP_AESCBC_Encrypt_DMA()
    
      (#)When the processing function is called at first time after HAL_CRYP_Init()
         the CRYP peripheral is initialized and processes the buffer in input.
         At second call, the processing function performs an append of the already
         processed buffer.
         When a new data block is to be processed, call HAL_CRYP_Init() then the
         processing function.
    
       (#)Call HAL_CRYP_DeInit() to deinitialize the CRYP peripheral.

  @endverbatim
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

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/** @addtogroup STM32F4xx_HAL_Driver
  * @{
  */

/** @defgroup CRYP 
  * @brief CRYP HAL module driver.
  * @{
  */

#ifdef HAL_CRYP_MODULE_ENABLED

#if defined(STM32F415xx) || defined(STM32F417xx) || defined(STM32F437xx) || defined(STM32F439xx)

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define CRYP_TIMEOUT_VALUE  1
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void CRYP_SetInitVector(CRYP_HandleTypeDef *hcryp, uint8_t *InitVector, uint32_t IVSize);
static void CRYP_SetKey(CRYP_HandleTypeDef *hcryp, uint8_t *Key, uint32_t KeySize);
static HAL_StatusTypeDef CRYP_ProcessData(CRYP_HandleTypeDef *hcryp, uint8_t* Input, uint16_t Ilength, uint8_t* Output, uint32_t Timeout);
static HAL_StatusTypeDef CRYP_ProcessData2Words(CRYP_HandleTypeDef *hcryp, uint8_t* Input, uint16_t Ilength, uint8_t* Output, uint32_t Timeout);
static void CRYP_DMAInCplt(DMA_HandleTypeDef *hdma);
static void CRYP_DMAOutCplt(DMA_HandleTypeDef *hdma);
static void CRYP_DMAError(DMA_HandleTypeDef *hdma);
static void CRYP_SetDMAConfig(CRYP_HandleTypeDef *hcryp, uint32_t inputaddr, uint16_t Size, uint32_t outputaddr);
static void CRYP_SetTDESECBMode(CRYP_HandleTypeDef *hcryp, uint32_t Direction);
static void CRYP_SetTDESCBCMode(CRYP_HandleTypeDef *hcryp, uint32_t Direction);
static void CRYP_SetDESECBMode(CRYP_HandleTypeDef *hcryp, uint32_t Direction);
static void CRYP_SetDESCBCMode(CRYP_HandleTypeDef *hcryp, uint32_t Direction);

/* Private functions ---------------------------------------------------------*/

/** @defgroup CRYP_Private_Functions
  * @{
  */

/** @defgroup CRYP_Group1 Initialization and de-initialization functions 
 *  @brief    Initialization and Configuration functions. 
 *
@verbatim    
  ==============================================================================
              ##### Initialization and de-initialization functions #####
  ==============================================================================
    [..]  This section provides functions allowing to:
      (+) Initialize the CRYP according to the specified parameters 
          in the CRYP_InitTypeDef and creates the associated handle
      (+) DeInitialize the CRYP peripheral
      (+) Initialize the CRYP MSP
      (+) DeInitialize CRYP MSP 
 
@endverbatim
  * @{
  */

/**
  * @brief  Initializes the CRYP according to the specified
  *         parameters in the CRYP_InitTypeDef and creates the associated handle.
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYP_Init(CRYP_HandleTypeDef *hcryp)
{ 
  /* Check the CRYP handle allocation */
  if(hcryp == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_CRYP_KEYSIZE(hcryp->Init.KeySize));
  assert_param(IS_CRYP_DATATYPE(hcryp->Init.DataType));
    
  if(hcryp->State == HAL_CRYP_STATE_RESET)
  {
    /* Init the low level hardware */
    HAL_CRYP_MspInit(hcryp);
  }
  
  /* Change the CRYP state */
  hcryp->State = HAL_CRYP_STATE_BUSY;
  
  /* Set the key size and data type*/
  CRYP->CR = (uint32_t) (hcryp->Init.KeySize | hcryp->Init.DataType);
  
  /* Reset CrypInCount and CrypOutCount */
  hcryp->CrypInCount = 0;
  hcryp->CrypOutCount = 0;
  
  /* Change the CRYP state */
  hcryp->State = HAL_CRYP_STATE_READY;
  
  /* Set the default CRYP phase */
  hcryp->Phase = HAL_CRYP_PHASE_READY;
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  DeInitializes the CRYP peripheral. 
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYP_DeInit(CRYP_HandleTypeDef *hcryp)
{
  /* Check the CRYP handle allocation */
  if(hcryp == NULL)
  {
    return HAL_ERROR;
  }
  
  /* Change the CRYP state */
  hcryp->State = HAL_CRYP_STATE_BUSY;
  
  /* Set the default CRYP phase */
  hcryp->Phase = HAL_CRYP_PHASE_READY;
  
  /* Reset CrypInCount and CrypOutCount */
  hcryp->CrypInCount = 0;
  hcryp->CrypOutCount = 0;
  
  /* Disable the CRYP Peripheral Clock */
  __HAL_CRYP_DISABLE();
  
  /* DeInit the low level hardware: CLOCK, NVIC.*/
  HAL_CRYP_MspDeInit(hcryp);
  
  /* Change the CRYP state */
  hcryp->State = HAL_CRYP_STATE_RESET;

  /* Release Lock */
  __HAL_UNLOCK(hcryp);

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Initializes the CRYP MSP.
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @retval None
  */
__weak void HAL_CRYP_MspInit(CRYP_HandleTypeDef *hcryp)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_CRYP_MspInit could be implemented in the user file
   */
}

/**
  * @brief  DeInitializes CRYP MSP.
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @retval None
  */
__weak void HAL_CRYP_MspDeInit(CRYP_HandleTypeDef *hcryp)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_CRYP_MspDeInit could be implemented in the user file
   */
}

/**
  * @}
  */

/** @defgroup CRYP_Group2 AES processing functions 
 *  @brief   processing functions. 
 *
@verbatim   
  ==============================================================================
                      ##### AES processing functions #####
  ==============================================================================  
    [..]  This section provides functions allowing to:
      (+) Encrypt plaintext using AES-128/192/256 using chaining modes
      (+) Decrypt cyphertext using AES-128/192/256 using chaining modes
    [..]  Three processing functions are available:
      (+) Polling mode
      (+) Interrupt mode
      (+) DMA mode

@endverbatim
  * @{
  */

/**
  * @brief  Initializes the CRYP peripheral in AES ECB encryption mode
  *         then encrypt pPlainData. The cypher data are available in pCypherData
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  pPlainData: Pointer to the plaintext buffer
  * @param  Size: Length of the plaintext buffer, must be a multiple of 16.
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @param  Timeout: Specify Timeout value 
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYP_AESECB_Encrypt(CRYP_HandleTypeDef *hcryp, uint8_t *pPlainData, uint16_t Size, uint8_t *pCypherData, uint32_t Timeout)
{
  /* Process Locked */
  __HAL_LOCK(hcryp);
  
  /* Change the CRYP state */
  hcryp->State = HAL_CRYP_STATE_BUSY;
  
  /* Check if initialization phase has already been performed */
  if(hcryp->Phase == HAL_CRYP_PHASE_READY)
  {
    /* Set the key */
    CRYP_SetKey(hcryp, hcryp->Init.pKey, hcryp->Init.KeySize);
    
    /* Set the CRYP peripheral in AES ECB mode */
    __HAL_CRYP_SET_MODE(CRYP_CR_ALGOMODE_AES_ECB);
    
    /* Flush FIFO */
    __HAL_CRYP_FIFO_FLUSH();
    
    /* Enable CRYP */
    __HAL_CRYP_ENABLE();
    
    /* Set the phase */
    hcryp->Phase = HAL_CRYP_PHASE_PROCESS;
  }
  
    /* Write Plain Data and Get Cypher Data */
    if(CRYP_ProcessData(hcryp,pPlainData, Size, pCypherData, Timeout) != HAL_OK)
    {
      return HAL_TIMEOUT;
    }
  
  /* Change the CRYP state */
  hcryp->State = HAL_CRYP_STATE_READY;
  
  /* Process Unlocked */
  __HAL_UNLOCK(hcryp);
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Initializes the CRYP peripheral in AES CBC encryption mode
  *         then encrypt pPlainData. The cypher data are available in pCypherData
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  pPlainData: Pointer to the plaintext buffer
  * @param  Size: Length of the plaintext buffer, must be a multiple of 16.
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @param  Timeout: Specify Timeout value  
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYP_AESCBC_Encrypt(CRYP_HandleTypeDef *hcryp, uint8_t *pPlainData, uint16_t Size, uint8_t *pCypherData, uint32_t Timeout)
{
  /* Process Locked */
  __HAL_LOCK(hcryp);
  
  /* Change the CRYP state */
  hcryp->State = HAL_CRYP_STATE_BUSY;
  
  /* Check if initialization phase has already been performed */
  if(hcryp->Phase == HAL_CRYP_PHASE_READY)
  {
    /* Set the key */
    CRYP_SetKey(hcryp, hcryp->Init.pKey, hcryp->Init.KeySize);
    
    /* Set the CRYP peripheral in AES ECB mode */
    __HAL_CRYP_SET_MODE(CRYP_CR_ALGOMODE_AES_CBC);
    
    /* Set the Initialization Vector */
    CRYP_SetInitVector(hcryp, hcryp->Init.pInitVect, CRYP_KEYSIZE_128B);
    
    /* Flush FIFO */
    __HAL_CRYP_FIFO_FLUSH();
    
    /* Enable CRYP */
    __HAL_CRYP_ENABLE();
    
    /* Set the phase */
    hcryp->Phase = HAL_CRYP_PHASE_PROCESS;
  }
  
    /* Write Plain Data and Get Cypher Data */
    if(CRYP_ProcessData(hcryp,pPlainData, Size, pCypherData, Timeout) != HAL_OK)
    {
      return HAL_TIMEOUT;
    }
  
  /* Change the CRYP state */
  hcryp->State = HAL_CRYP_STATE_READY;
  
  /* Process Unlocked */
  __HAL_UNLOCK(hcryp);
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Initializes the CRYP peripheral in AES CTR encryption mode
  *         then encrypt pPlainData. The cypher data are available in pCypherData
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  pPlainData: Pointer to the plaintext buffer
  * @param  Size: Length of the plaintext buffer, must be a multiple of 16.
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @param  Timeout: Specify Timeout value  
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYP_AESCTR_Encrypt(CRYP_HandleTypeDef *hcryp, uint8_t *pPlainData, uint16_t Size, uint8_t *pCypherData, uint32_t Timeout)
{  
  /* Process Locked */
  __HAL_LOCK(hcryp);
  
  /* Change the CRYP state */
  hcryp->State = HAL_CRYP_STATE_BUSY;
  
  /* Check if initialization phase has already been performed */
  if(hcryp->Phase == HAL_CRYP_PHASE_READY)
  {
    /* Set the key */
    CRYP_SetKey(hcryp, hcryp->Init.pKey, hcryp->Init.KeySize);
    
    /* Set the CRYP peripheral in AES ECB mode */
    __HAL_CRYP_SET_MODE(CRYP_CR_ALGOMODE_AES_CTR);
    
    /* Set the Initialization Vector */
    CRYP_SetInitVector(hcryp, hcryp->Init.pInitVect, CRYP_KEYSIZE_128B);
    
    /* Flush FIFO */
    __HAL_CRYP_FIFO_FLUSH();
    
    /* Enable CRYP */
    __HAL_CRYP_ENABLE();
    
    /* Set the phase */
    hcryp->Phase = HAL_CRYP_PHASE_PROCESS;
  }
  
    /* Write Plain Data and Get Cypher Data */
    if(CRYP_ProcessData(hcryp, pPlainData, Size, pCypherData, Timeout) != HAL_OK)
    {
      return HAL_TIMEOUT;
    }
  
  /* Change the CRYP state */
  hcryp->State = HAL_CRYP_STATE_READY;
  
  /* Process Unlocked */
  __HAL_UNLOCK(hcryp);
  
  /* Return function status */
  return HAL_OK;
}



/**
  * @brief  Initializes the CRYP peripheral in AES ECB decryption mode
  *         then decrypted pCypherData. The cypher data are available in pPlainData
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @param  Size: Length of the plaintext buffer, must be a multiple of 16.
  * @param  pPlainData: Pointer to the plaintext buffer
  * @param  Timeout: Specify Timeout value  
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYP_AESECB_Decrypt(CRYP_HandleTypeDef *hcryp, uint8_t *pCypherData, uint16_t Size, uint8_t *pPlainData, uint32_t Timeout)
{
   uint32_t tickstart = 0;
  
  /* Process Locked */
  __HAL_LOCK(hcryp);
  
  /* Change the CRYP state */
  hcryp->State = HAL_CRYP_STATE_BUSY;
  
  /* Check if initialization phase has already been performed */
  if(hcryp->Phase == HAL_CRYP_PHASE_READY)
  {
    /* Set the key */
    CRYP_SetKey(hcryp, hcryp->Init.pKey, hcryp->Init.KeySize);
    
    /* Set the CRYP peripheral in AES Key mode */
    __HAL_CRYP_SET_MODE(CRYP_CR_ALGOMODE_AES_KEY | CRYP_CR_ALGODIR);
    
    /* Enable CRYP */
    __HAL_CRYP_ENABLE();
    
    /* Get tick */ 
    tickstart = HAL_GetTick();

    while(HAL_IS_BIT_SET(CRYP->SR, CRYP_FLAG_BUSY))
    {
      /* Check for the Timeout */
      if(Timeout != HAL_MAX_DELAY)
      {
        if((Timeout == 0)||((HAL_GetTick() - tickstart ) > Timeout))
        {
          /* Change state */
          hcryp->State = HAL_CRYP_STATE_TIMEOUT;
          
          /* Process Unlocked */          
          __HAL_UNLOCK(hcryp);
        
          return HAL_TIMEOUT;
        }
      }
    }
    
    /* Disable CRYP */
    __HAL_CRYP_DISABLE();
    
    /* Reset the ALGOMODE bits*/
    CRYP->CR &= (uint32_t)(~CRYP_CR_ALGOMODE);
    
    /* Set the CRYP peripheral in AES ECB decryption mode */
    __HAL_CRYP_SET_MODE(CRYP_CR_ALGOMODE_AES_ECB | CRYP_CR_ALGODIR);
    /* Flush FIFO */
    __HAL_CRYP_FIFO_FLUSH();
    
    /* Enable CRYP */
    __HAL_CRYP_ENABLE();
    
    /* Set the phase */
    hcryp->Phase = HAL_CRYP_PHASE_PROCESS;
  }
    
    /* Write Plain Data and Get Cypher Data */
    if(CRYP_ProcessData(hcryp, pCypherData, Size, pPlainData, Timeout) != HAL_OK)
    {
      return HAL_TIMEOUT;
    }
  
  /* Change the CRYP state */
  hcryp->State = HAL_CRYP_STATE_READY;
  
  /* Process Unlocked */
  __HAL_UNLOCK(hcryp);
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Initializes the CRYP peripheral in AES ECB decryption mode
  *         then decrypted pCypherData. The cypher data are available in pPlainData
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @param  Size: Length of the plaintext buffer, must be a multiple of 16.
  * @param  pPlainData: Pointer to the plaintext buffer
  * @param  Timeout: Specify Timeout value  
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYP_AESCBC_Decrypt(CRYP_HandleTypeDef *hcryp, uint8_t *pCypherData, uint16_t Size, uint8_t *pPlainData, uint32_t Timeout)
{
  uint32_t tickstart = 0;
  
  /* Process Locked */
  __HAL_LOCK(hcryp);
  
  /* Change the CRYP state */
  hcryp->State = HAL_CRYP_STATE_BUSY;
  
  /* Check if initialization phase has already been performed */
  if(hcryp->Phase == HAL_CRYP_PHASE_READY)
  {
    /* Set the key */
    CRYP_SetKey(hcryp, hcryp->Init.pKey, hcryp->Init.KeySize);
    
    /* Set the CRYP peripheral in AES Key mode */
    __HAL_CRYP_SET_MODE(CRYP_CR_ALGOMODE_AES_KEY | CRYP_CR_ALGODIR);
    
    /* Enable CRYP */
    __HAL_CRYP_ENABLE();
    
    /* Get tick */ 
    tickstart = HAL_GetTick();

    while(HAL_IS_BIT_SET(CRYP->SR, CRYP_FLAG_BUSY))
    {
      /* Check for the Timeout */
      if(Timeout != HAL_MAX_DELAY)
      {
        if((Timeout == 0)||((HAL_GetTick() - tickstart ) > Timeout))
        {
          /* Change state */
          hcryp->State = HAL_CRYP_STATE_TIMEOUT;
          
          /* Process Unlocked */
          __HAL_UNLOCK(hcryp);
          
          return HAL_TIMEOUT;
        }
      }
    }
    
    /* Reset the ALGOMODE bits*/
    CRYP->CR &= (uint32_t)(~CRYP_CR_ALGOMODE);
    
    /* Set the CRYP peripheral in AES CBC decryption mode */
    __HAL_CRYP_SET_MODE(CRYP_CR_ALGOMODE_AES_CBC | CRYP_CR_ALGODIR);
    
    /* Set the Initialization Vector */
    CRYP_SetInitVector(hcryp, hcryp->Init.pInitVect, CRYP_KEYSIZE_128B);
    
    /* Flush FIFO */
    __HAL_CRYP_FIFO_FLUSH();
    
    /* Enable CRYP */
    __HAL_CRYP_ENABLE();
    
    /* Set the phase */
    hcryp->Phase = HAL_CRYP_PHASE_PROCESS;
  }
  
    /* Write Plain Data and Get Cypher Data */
    if(CRYP_ProcessData(hcryp, pCypherData, Size, pPlainData, Timeout) != HAL_OK)
    {
      return HAL_TIMEOUT;
    }
  
  /* Change the CRYP state */
  hcryp->State = HAL_CRYP_STATE_READY;
  
  /* Process Unlocked */
  __HAL_UNLOCK(hcryp);
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Initializes the CRYP peripheral in AES CTR decryption mode
  *         then decrypted pCypherData. The cypher data are available in pPlainData
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @param  Size: Length of the plaintext buffer, must be a multiple of 16.
  * @param  pPlainData: Pointer to the plaintext buffer
  * @param  Timeout: Specify Timeout value  
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYP_AESCTR_Decrypt(CRYP_HandleTypeDef *hcryp, uint8_t *pCypherData, uint16_t Size, uint8_t *pPlainData, uint32_t Timeout)
{  
  /* Process Locked */
  __HAL_LOCK(hcryp);
  
  /* Check if initialization phase has already been performed */
  if(hcryp->Phase == HAL_CRYP_PHASE_READY)
  {
    /* Change the CRYP state */
    hcryp->State = HAL_CRYP_STATE_BUSY;
    
    /* Set the key */
    CRYP_SetKey(hcryp, hcryp->Init.pKey, hcryp->Init.KeySize);
    
    /* Set the CRYP peripheral in AES CTR mode */
    __HAL_CRYP_SET_MODE(CRYP_CR_ALGOMODE_AES_CTR | CRYP_CR_ALGODIR);
    
    /* Set the Initialization Vector */
    CRYP_SetInitVector(hcryp, hcryp->Init.pInitVect, CRYP_KEYSIZE_128B);
    
    /* Flush FIFO */
    __HAL_CRYP_FIFO_FLUSH();
    
    /* Enable CRYP */
    __HAL_CRYP_ENABLE();
    
    /* Set the phase */
    hcryp->Phase = HAL_CRYP_PHASE_PROCESS;
  }
  
    /* Write Plain Data and Get Cypher Data */
    if(CRYP_ProcessData(hcryp, pCypherData, Size, pPlainData, Timeout) != HAL_OK)
    {
      return HAL_TIMEOUT;
    }
  
  /* Change the CRYP state */
  hcryp->State = HAL_CRYP_STATE_READY;
  
  /* Process Unlocked */
  __HAL_UNLOCK(hcryp);
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Initializes the CRYP peripheral in AES ECB encryption mode using Interrupt.
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  pPlainData: Pointer to the plaintext buffer
  * @param  Size: Length of the plaintext buffer, must be a multiple of 16 bytes
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYP_AESECB_Encrypt_IT(CRYP_HandleTypeDef *hcryp, uint8_t *pPlainData, uint16_t Size, uint8_t *pCypherData)
{
  uint32_t inputaddr;
  uint32_t outputaddr;
  
  if(hcryp->State == HAL_CRYP_STATE_READY)
  {
    /* Process Locked */
    __HAL_LOCK(hcryp);
    
    hcryp->CrypInCount = Size;
    hcryp->pCrypInBuffPtr = pPlainData;
    hcryp->pCrypOutBuffPtr = pCypherData;
    hcryp->CrypOutCount = Size;
    
    /* Change the CRYP state */
    hcryp->State = HAL_CRYP_STATE_BUSY;
    
    /* Check if initialization phase has already been performed */
    if(hcryp->Phase == HAL_CRYP_PHASE_READY)
    {
      /* Set the key */
      CRYP_SetKey(hcryp, hcryp->Init.pKey, hcryp->Init.KeySize);
      
      /* Set the CRYP peripheral in AES ECB mode */
      __HAL_CRYP_SET_MODE(CRYP_CR_ALGOMODE_AES_ECB);
      
      /* Flush FIFO */
      __HAL_CRYP_FIFO_FLUSH();
      
     /* Set the phase */
     hcryp->Phase = HAL_CRYP_PHASE_PROCESS;
    }
    
    /* Enable Interrupts */
    __HAL_CRYP_ENABLE_IT(CRYP_IT_INI | CRYP_IT_OUTI);
    
    /* Enable CRYP */
    __HAL_CRYP_ENABLE();
    
    /* Return function status */
    return HAL_OK;
  }
  else if(__HAL_CRYP_GET_IT(CRYP_IT_INI))
  {
    inputaddr = (uint32_t)hcryp->pCrypInBuffPtr;
    /* Write the Input block in the IN FIFO */
    CRYP->DR = *(uint32_t*)(inputaddr);
    inputaddr+=4;
    CRYP->DR = *(uint32_t*)(inputaddr);
    inputaddr+=4;
    CRYP->DR  = *(uint32_t*)(inputaddr);
    inputaddr+=4;
    CRYP->DR = *(uint32_t*)(inputaddr);
    hcryp->pCrypInBuffPtr += 16;
    hcryp->CrypInCount -= 16;
    if(hcryp->CrypInCount == 0)
    {
      __HAL_CRYP_DISABLE_IT(CRYP_IT_INI);
      /* Call the Input data transfer complete callback */
      HAL_CRYP_InCpltCallback(hcryp);
    }
  }
  else if(__HAL_CRYP_GET_IT(CRYP_IT_OUTI))
  {
    outputaddr = (uint32_t)hcryp->pCrypOutBuffPtr;
    /* Read the Output block from the Output FIFO */
    *(uint32_t*)(outputaddr) = CRYP->DOUT;
    outputaddr+=4;
    *(uint32_t*)(outputaddr) = CRYP->DOUT;
    outputaddr+=4;
    *(uint32_t*)(outputaddr) = CRYP->DOUT;
    outputaddr+=4;
    *(uint32_t*)(outputaddr) = CRYP->DOUT;
    hcryp->pCrypOutBuffPtr += 16;
    hcryp->CrypOutCount -= 16;
    if(hcryp->CrypOutCount == 0)
    {
      __HAL_CRYP_DISABLE_IT(CRYP_IT_OUTI);
      /* Process Locked */
      __HAL_UNLOCK(hcryp);
      /* Change the CRYP state */
      hcryp->State = HAL_CRYP_STATE_READY;
      /* Call Input transfer complete callback */
      HAL_CRYP_OutCpltCallback(hcryp);
    }
  }
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Initializes the CRYP peripheral in AES CBC encryption mode using Interrupt.
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  pPlainData: Pointer to the plaintext buffer
  * @param  Size: Length of the plaintext buffer, must be a multiple of 16 bytes
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYP_AESCBC_Encrypt_IT(CRYP_HandleTypeDef *hcryp, uint8_t *pPlainData, uint16_t Size, uint8_t *pCypherData)
{
  uint32_t inputaddr;
  uint32_t outputaddr;
  
  if(hcryp->State == HAL_CRYP_STATE_READY)
  {
    /* Process Locked */
    __HAL_LOCK(hcryp);
    
    hcryp->CrypInCount = Size;
    hcryp->pCrypInBuffPtr = pPlainData;
    hcryp->pCrypOutBuffPtr = pCypherData;
    hcryp->CrypOutCount = Size;
    
    /* Change the CRYP state */
    hcryp->State = HAL_CRYP_STATE_BUSY;
    
    /* Check if initialization phase has already been performed */
    if(hcryp->Phase == HAL_CRYP_PHASE_READY)
    {      
      /* Set the key */
      CRYP_SetKey(hcryp, hcryp->Init.pKey, hcryp->Init.KeySize);
      
      /* Set the CRYP peripheral in AES CBC mode */
      __HAL_CRYP_SET_MODE(CRYP_CR_ALGOMODE_AES_CBC);
      
      /* Set the Initialization Vector */
      CRYP_SetInitVector(hcryp, hcryp->Init.pInitVect, CRYP_KEYSIZE_128B);
      
      /* Flush FIFO */
      __HAL_CRYP_FIFO_FLUSH();
      
     /* Set the phase */
     hcryp->Phase = HAL_CRYP_PHASE_PROCESS;
    }
    /* Enable Interrupts */
    __HAL_CRYP_ENABLE_IT(CRYP_IT_INI | CRYP_IT_OUTI);
    
    /* Enable CRYP */
    __HAL_CRYP_ENABLE();
    
    /* Return function status */
    return HAL_OK;
  }
  else if(__HAL_CRYP_GET_IT(CRYP_IT_INI))
  {
    inputaddr = (uint32_t)hcryp->pCrypInBuffPtr;
    /* Write the Input block in the IN FIFO */
    CRYP->DR = *(uint32_t*)(inputaddr);
    inputaddr+=4;
    CRYP->DR = *(uint32_t*)(inputaddr);
    inputaddr+=4;
    CRYP->DR  = *(uint32_t*)(inputaddr);
    inputaddr+=4;
    CRYP->DR = *(uint32_t*)(inputaddr);
    hcryp->pCrypInBuffPtr += 16;
    hcryp->CrypInCount -= 16;
    if(hcryp->CrypInCount == 0)
    {
      __HAL_CRYP_DISABLE_IT(CRYP_IT_INI);
      /* Call the Input data transfer complete callback */
      HAL_CRYP_InCpltCallback(hcryp);
    }
  }
  else if(__HAL_CRYP_GET_IT(CRYP_IT_OUTI))
  {
    outputaddr = (uint32_t)hcryp->pCrypOutBuffPtr;
    /* Read the Output block from the Output FIFO */
    *(uint32_t*)(outputaddr) = CRYP->DOUT;
    outputaddr+=4;
    *(uint32_t*)(outputaddr) = CRYP->DOUT;
    outputaddr+=4;
    *(uint32_t*)(outputaddr) = CRYP->DOUT;
    outputaddr+=4;
    *(uint32_t*)(outputaddr) = CRYP->DOUT;
    hcryp->pCrypOutBuffPtr += 16;
    hcryp->CrypOutCount -= 16;
    if(hcryp->CrypOutCount == 0)
    {
      __HAL_CRYP_DISABLE_IT(CRYP_IT_OUTI);
      /* Process Locked */
      __HAL_UNLOCK(hcryp);
      /* Change the CRYP state */
      hcryp->State = HAL_CRYP_STATE_READY;
      /* Call Input transfer complete callback */
      HAL_CRYP_OutCpltCallback(hcryp);
    }
  }
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Initializes the CRYP peripheral in AES CTR encryption mode using Interrupt.
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  pPlainData: Pointer to the plaintext buffer
  * @param  Size: Length of the plaintext buffer, must be a multiple of 16 bytes
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYP_AESCTR_Encrypt_IT(CRYP_HandleTypeDef *hcryp, uint8_t *pPlainData, uint16_t Size, uint8_t *pCypherData)
{
  uint32_t inputaddr;
  uint32_t outputaddr;
  
  if(hcryp->State == HAL_CRYP_STATE_READY)
  {
    /* Process Locked */
    __HAL_LOCK(hcryp);
    
    hcryp->CrypInCount = Size;
    hcryp->pCrypInBuffPtr = pPlainData;
    hcryp->pCrypOutBuffPtr = pCypherData;
    hcryp->CrypOutCount = Size;
    
    /* Change the CRYP state */
    hcryp->State = HAL_CRYP_STATE_BUSY;
    
    /* Check if initialization phase has already been performed */
    if(hcryp->Phase == HAL_CRYP_PHASE_READY)
    {
      /* Set the key */
      CRYP_SetKey(hcryp, hcryp->Init.pKey, hcryp->Init.KeySize);
      
      /* Set the CRYP peripheral in AES CTR mode */
      __HAL_CRYP_SET_MODE(CRYP_CR_ALGOMODE_AES_CTR);
      
      /* Set the Initialization Vector */
      CRYP_SetInitVector(hcryp, hcryp->Init.pInitVect, CRYP_KEYSIZE_128B);
      
      /* Flush FIFO */
      __HAL_CRYP_FIFO_FLUSH();
      
     /* Set the phase */
     hcryp->Phase = HAL_CRYP_PHASE_PROCESS;
    }
    /* Enable Interrupts */
    __HAL_CRYP_ENABLE_IT(CRYP_IT_INI | CRYP_IT_OUTI);
    
    /* Enable CRYP */
    __HAL_CRYP_ENABLE();
    
    /* Return function status */
    return HAL_OK;
  }
  else if(__HAL_CRYP_GET_IT(CRYP_IT_INI))
  {
    inputaddr = (uint32_t)hcryp->pCrypInBuffPtr;
    /* Write the Input block in the IN FIFO */
    CRYP->DR = *(uint32_t*)(inputaddr);
    inputaddr+=4;
    CRYP->DR = *(uint32_t*)(inputaddr);
    inputaddr+=4;
    CRYP->DR  = *(uint32_t*)(inputaddr);
    inputaddr+=4;
    CRYP->DR = *(uint32_t*)(inputaddr);
    hcryp->pCrypInBuffPtr += 16;
    hcryp->CrypInCount -= 16;
    if(hcryp->CrypInCount == 0)
    {
      __HAL_CRYP_DISABLE_IT(CRYP_IT_INI);
      /* Call the Input data transfer complete callback */
      HAL_CRYP_InCpltCallback(hcryp);
    }
  }
  else if(__HAL_CRYP_GET_IT(CRYP_IT_OUTI))
  {
    outputaddr = (uint32_t)hcryp->pCrypOutBuffPtr;
    /* Read the Output block from the Output FIFO */
    *(uint32_t*)(outputaddr) = CRYP->DOUT;
    outputaddr+=4;
    *(uint32_t*)(outputaddr) = CRYP->DOUT;
    outputaddr+=4;
    *(uint32_t*)(outputaddr) = CRYP->DOUT;
    outputaddr+=4;
    *(uint32_t*)(outputaddr) = CRYP->DOUT;
    hcryp->pCrypOutBuffPtr += 16;
    hcryp->CrypOutCount -= 16;
    if(hcryp->CrypOutCount == 0)
    {
      __HAL_CRYP_DISABLE_IT(CRYP_IT_OUTI);
      /* Process Unlocked */
      __HAL_UNLOCK(hcryp);
      /* Change the CRYP state */
      hcryp->State = HAL_CRYP_STATE_READY;
      /* Call Input transfer complete callback */
      HAL_CRYP_OutCpltCallback(hcryp);
    }
  }
  
  /* Return function status */
  return HAL_OK;
}


/**
  * @brief  Initializes the CRYP peripheral in AES ECB decryption mode using Interrupt.
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @param  Size: Length of the plaintext buffer, must be a multiple of 16.
  * @param  pPlainData: Pointer to the plaintext buffer
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYP_AESECB_Decrypt_IT(CRYP_HandleTypeDef *hcryp, uint8_t *pCypherData, uint16_t Size, uint8_t *pPlainData)
{
  uint32_t tickstart = 0;

  uint32_t inputaddr;
  uint32_t outputaddr;
  
  if(hcryp->State == HAL_CRYP_STATE_READY)
  {
    /* Process Locked */
    __HAL_LOCK(hcryp);
    
    hcryp->CrypInCount = Size;
    hcryp->pCrypInBuffPtr = pCypherData;
    hcryp->pCrypOutBuffPtr = pPlainData;
    hcryp->CrypOutCount = Size;
    
    /* Change the CRYP state */
    hcryp->State = HAL_CRYP_STATE_BUSY;
    
  /* Check if initialization phase has already been performed */
  if(hcryp->Phase == HAL_CRYP_PHASE_READY)
  {
    /* Set the key */
    CRYP_SetKey(hcryp, hcryp->Init.pKey, hcryp->Init.KeySize);
    
    /* Set the CRYP peripheral in AES Key mode */
    __HAL_CRYP_SET_MODE(CRYP_CR_ALGOMODE_AES_KEY | CRYP_CR_ALGODIR);
    /* Enable CRYP */
    __HAL_CRYP_ENABLE();
    
    /* Get tick */ 
    tickstart = HAL_GetTick();

    while(HAL_IS_BIT_SET(CRYP->SR, CRYP_FLAG_BUSY))
    {
      /* Check for the Timeout */
      if((HAL_GetTick() - tickstart ) > CRYP_TIMEOUT_VALUE)
      {
        /* Change state */
        hcryp->State = HAL_CRYP_STATE_TIMEOUT;
        
        /* Process Unlocked */
        __HAL_UNLOCK(hcryp);
        
        return HAL_TIMEOUT;
      }
    }
    
    /* Reset the ALGOMODE bits*/
    CRYP->CR &= (uint32_t)(~CRYP_CR_ALGOMODE);
    
    /* Set the CRYP peripheral in AES ECB decryption mode */
    __HAL_CRYP_SET_MODE(CRYP_CR_ALGOMODE_AES_ECB | CRYP_CR_ALGODIR);
    
    /* Flush FIFO */
    __HAL_CRYP_FIFO_FLUSH();
    
     /* Set the phase */
     hcryp->Phase = HAL_CRYP_PHASE_PROCESS;
  }
     
    /* Enable Interrupts */
    __HAL_CRYP_ENABLE_IT(CRYP_IT_INI | CRYP_IT_OUTI);
    
    /* Enable CRYP */
    __HAL_CRYP_ENABLE();
    
    /* Return function status */
    return HAL_OK;
  }
  else if(__HAL_CRYP_GET_IT(CRYP_IT_INI))
  {
    inputaddr = (uint32_t)hcryp->pCrypInBuffPtr;
    /* Write the Input block in the IN FIFO */
    CRYP->DR = *(uint32_t*)(inputaddr);
    inputaddr+=4;
    CRYP->DR = *(uint32_t*)(inputaddr);
    inputaddr+=4;
    CRYP->DR  = *(uint32_t*)(inputaddr);
    inputaddr+=4;
    CRYP->DR = *(uint32_t*)(inputaddr);
    hcryp->pCrypInBuffPtr += 16;
    hcryp->CrypInCount -= 16;
    if(hcryp->CrypInCount == 0)
    {
      __HAL_CRYP_DISABLE_IT(CRYP_IT_INI);
      /* Call the Input data transfer complete callback */
      HAL_CRYP_InCpltCallback(hcryp);
    }
  }
  else if(__HAL_CRYP_GET_IT(CRYP_IT_OUTI))
  {
    outputaddr = (uint32_t)hcryp->pCrypOutBuffPtr;
    /* Read the Output block from the Output FIFO */
    *(uint32_t*)(outputaddr) = CRYP->DOUT;
    outputaddr+=4;
    *(uint32_t*)(outputaddr) = CRYP->DOUT;
    outputaddr+=4;
    *(uint32_t*)(outputaddr) = CRYP->DOUT;
    outputaddr+=4;
    *(uint32_t*)(outputaddr) = CRYP->DOUT;
    hcryp->pCrypOutBuffPtr += 16;
    hcryp->CrypOutCount -= 16;
    if(hcryp->CrypOutCount == 0)
    {
      __HAL_CRYP_DISABLE_IT(CRYP_IT_OUTI);
      /* Process Unlocked */
      __HAL_UNLOCK(hcryp);
      /* Change the CRYP state */
      hcryp->State = HAL_CRYP_STATE_READY;
      /* Call Input transfer complete callback */
      HAL_CRYP_OutCpltCallback(hcryp);
    }
  }
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Initializes the CRYP peripheral in AES CBC decryption mode using IT.
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @param  Size: Length of the plaintext buffer, must be a multiple of 16
  * @param  pPlainData: Pointer to the plaintext buffer
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYP_AESCBC_Decrypt_IT(CRYP_HandleTypeDef *hcryp, uint8_t *pCypherData, uint16_t Size, uint8_t *pPlainData)
{

  uint32_t tickstart = 0;   
  uint32_t inputaddr;
  uint32_t outputaddr;
  
  if(hcryp->State == HAL_CRYP_STATE_READY)
  {
    /* Process Locked */
    __HAL_LOCK(hcryp);
    
    /* Get the buffer addresses and sizes */    
    hcryp->CrypInCount = Size;
    hcryp->pCrypInBuffPtr = pCypherData;
    hcryp->pCrypOutBuffPtr = pPlainData;
    hcryp->CrypOutCount = Size;
    
    /* Change the CRYP state */
    hcryp->State = HAL_CRYP_STATE_BUSY;
    
    /* Check if initialization phase has already been performed */
    if(hcryp->Phase == HAL_CRYP_PHASE_READY)
    {
      /* Set the key */
      CRYP_SetKey(hcryp, hcryp->Init.pKey, hcryp->Init.KeySize);
      
      /* Set the CRYP peripheral in AES Key mode */
      __HAL_CRYP_SET_MODE(CRYP_CR_ALGOMODE_AES_KEY | CRYP_CR_ALGODIR);
      
      /* Enable CRYP */
      __HAL_CRYP_ENABLE();
      
    /* Get tick */
    tickstart = HAL_GetTick();

    while(HAL_IS_BIT_SET(CRYP->SR, CRYP_FLAG_BUSY))
    {
      /* Check for the Timeout */
      if((HAL_GetTick() - tickstart ) > CRYP_TIMEOUT_VALUE)
      {
        /* Change state */
        hcryp->State = HAL_CRYP_STATE_TIMEOUT;
        
        /* Process Unlocked */
        __HAL_UNLOCK(hcryp);
        
        return HAL_TIMEOUT;
      }
    }
    
      /* Reset the ALGOMODE bits*/
      CRYP->CR &= (uint32_t)(~CRYP_CR_ALGOMODE);
    
      /* Set the CRYP peripheral in AES CBC decryption mode */
      __HAL_CRYP_SET_MODE(CRYP_CR_ALGOMODE_AES_CBC | CRYP_CR_ALGODIR);
    
      /* Set the Initialization Vector */
      CRYP_SetInitVector(hcryp, hcryp->Init.pInitVect, CRYP_KEYSIZE_128B);
    
      /* Flush FIFO */
      __HAL_CRYP_FIFO_FLUSH();
    
      /* Enable CRYP */
      __HAL_CRYP_ENABLE();
      
      /* Set the phase */
      hcryp->Phase = HAL_CRYP_PHASE_PROCESS;
    }
    
    /* Enable Interrupts */
    __HAL_CRYP_ENABLE_IT(CRYP_IT_INI | CRYP_IT_OUTI);
    
    /* Enable CRYP */
    __HAL_CRYP_ENABLE();
    
    /* Return function status */
    return HAL_OK;
  }
  else if(__HAL_CRYP_GET_IT(CRYP_IT_INI))
  {
    inputaddr = (uint32_t)hcryp->pCrypInBuffPtr;
    /* Write the Input block in the IN FIFO */
    CRYP->DR = *(uint32_t*)(inputaddr);
    inputaddr+=4;
    CRYP->DR = *(uint32_t*)(inputaddr);
    inputaddr+=4;
    CRYP->DR  = *(uint32_t*)(inputaddr);
    inputaddr+=4;
    CRYP->DR = *(uint32_t*)(inputaddr);
    hcryp->pCrypInBuffPtr += 16;
    hcryp->CrypInCount -= 16;
    if(hcryp->CrypInCount == 0)
    {
      __HAL_CRYP_DISABLE_IT(CRYP_IT_INI);
      /* Call the Input data transfer complete callback */
      HAL_CRYP_InCpltCallback(hcryp);
    }
  }
  else if(__HAL_CRYP_GET_IT(CRYP_IT_OUTI))
  {
    outputaddr = (uint32_t)hcryp->pCrypOutBuffPtr;
    /* Read the Output block from the Output FIFO */
    *(uint32_t*)(outputaddr) = CRYP->DOUT;
    outputaddr+=4;
    *(uint32_t*)(outputaddr) = CRYP->DOUT;
    outputaddr+=4;
    *(uint32_t*)(outputaddr) = CRYP->DOUT;
    outputaddr+=4;
    *(uint32_t*)(outputaddr) = CRYP->DOUT;
    hcryp->pCrypOutBuffPtr += 16;
    hcryp->CrypOutCount -= 16;
    if(hcryp->CrypOutCount == 0)
    {
      __HAL_CRYP_DISABLE_IT(CRYP_IT_OUTI);
      /* Process Unlocked */
      __HAL_UNLOCK(hcryp);
      /* Change the CRYP state */
      hcryp->State = HAL_CRYP_STATE_READY;
      /* Call Input transfer complete callback */
      HAL_CRYP_OutCpltCallback(hcryp);
    }
  }
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Initializes the CRYP peripheral in AES CTR decryption mode using Interrupt.
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @param  Size: Length of the plaintext buffer, must be a multiple of 16
  * @param  pPlainData: Pointer to the plaintext buffer
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYP_AESCTR_Decrypt_IT(CRYP_HandleTypeDef *hcryp, uint8_t *pCypherData, uint16_t Size, uint8_t *pPlainData)
{
  uint32_t inputaddr;
  uint32_t outputaddr;
  
  if(hcryp->State == HAL_CRYP_STATE_READY)
  {
    /* Process Locked */
    __HAL_LOCK(hcryp);
    
    /* Get the buffer addresses and sizes */    
    hcryp->CrypInCount = Size;
    hcryp->pCrypInBuffPtr = pCypherData;
    hcryp->pCrypOutBuffPtr = pPlainData;
    hcryp->CrypOutCount = Size;
    
    /* Change the CRYP state */
    hcryp->State = HAL_CRYP_STATE_BUSY;
    
    /* Check if initialization phase has already been performed */
    if(hcryp->Phase == HAL_CRYP_PHASE_READY)
    {
      /* Set the key */
      CRYP_SetKey(hcryp, hcryp->Init.pKey, hcryp->Init.KeySize);
      
      /* Set the CRYP peripheral in AES CTR mode */
      __HAL_CRYP_SET_MODE(CRYP_CR_ALGOMODE_AES_CTR | CRYP_CR_ALGODIR);
      
      /* Set the Initialization Vector */
      CRYP_SetInitVector(hcryp, hcryp->Init.pInitVect, CRYP_KEYSIZE_128B);
      
      /* Flush FIFO */
      __HAL_CRYP_FIFO_FLUSH();
      
      /* Set the phase */
      hcryp->Phase = HAL_CRYP_PHASE_PROCESS;
    }
    
    /* Enable Interrupts */
    __HAL_CRYP_ENABLE_IT(CRYP_IT_INI | CRYP_IT_OUTI);
    
    /* Enable CRYP */
    __HAL_CRYP_ENABLE();
    
    /* Return function status */
    return HAL_OK;
  }
  else if(__HAL_CRYP_GET_IT(CRYP_IT_INI))
  {
    inputaddr = (uint32_t)hcryp->pCrypInBuffPtr;
    /* Write the Input block in the IN FIFO */
    CRYP->DR = *(uint32_t*)(inputaddr);
    inputaddr+=4;
    CRYP->DR = *(uint32_t*)(inputaddr);
    inputaddr+=4;
    CRYP->DR  = *(uint32_t*)(inputaddr);
    inputaddr+=4;
    CRYP->DR = *(uint32_t*)(inputaddr);
    hcryp->pCrypInBuffPtr += 16;
    hcryp->CrypInCount -= 16;
    if(hcryp->CrypInCount == 0)
    {
      __HAL_CRYP_DISABLE_IT(CRYP_IT_INI);
      /* Call the Input data transfer complete callback */
      HAL_CRYP_InCpltCallback(hcryp);
    }
  }
  else if(__HAL_CRYP_GET_IT(CRYP_IT_OUTI))
  {
    outputaddr = (uint32_t)hcryp->pCrypOutBuffPtr;
    /* Read the Output block from the Output FIFO */
    *(uint32_t*)(outputaddr) = CRYP->DOUT;
    outputaddr+=4;
    *(uint32_t*)(outputaddr) = CRYP->DOUT;
    outputaddr+=4;
    *(uint32_t*)(outputaddr) = CRYP->DOUT;
    outputaddr+=4;
    *(uint32_t*)(outputaddr) = CRYP->DOUT;
    hcryp->pCrypOutBuffPtr += 16;
    hcryp->CrypOutCount -= 16;
    if(hcryp->CrypOutCount == 0)
    {
      __HAL_CRYP_DISABLE_IT(CRYP_IT_OUTI);
      /* Process Unlocked */
      __HAL_UNLOCK(hcryp);
      /* Change the CRYP state */
      hcryp->State = HAL_CRYP_STATE_READY;
      /* Call Input transfer complete callback */
      HAL_CRYP_OutCpltCallback(hcryp);
    }
  }
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Initializes the CRYP peripheral in AES ECB encryption mode using DMA.
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  pPlainData: Pointer to the plaintext buffer
  * @param  Size: Length of the plaintext buffer, must be a multiple of 16 bytes
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYP_AESECB_Encrypt_DMA(CRYP_HandleTypeDef *hcryp, uint8_t *pPlainData, uint16_t Size, uint8_t *pCypherData)
{
  uint32_t inputaddr;
  uint32_t outputaddr;
  
  if((hcryp->State == HAL_CRYP_STATE_READY) || (hcryp->Phase == HAL_CRYP_PHASE_PROCESS))
  {
    /* Process Locked */
    __HAL_LOCK(hcryp);
    
    inputaddr  = (uint32_t)pPlainData;
    outputaddr = (uint32_t)pCypherData;
    
    /* Change the CRYP state */
    hcryp->State = HAL_CRYP_STATE_BUSY;
    
    /* Check if initialization phase has already been performed */
    if(hcryp->Phase == HAL_CRYP_PHASE_READY)
    {
      /* Set the key */
      CRYP_SetKey(hcryp, hcryp->Init.pKey, hcryp->Init.KeySize);
      
      /* Set the CRYP peripheral in AES ECB mode */
      __HAL_CRYP_SET_MODE(CRYP_CR_ALGOMODE_AES_ECB);
      
      /* Flush FIFO */
      __HAL_CRYP_FIFO_FLUSH();
      
     /* Set the phase */
     hcryp->Phase = HAL_CRYP_PHASE_PROCESS;
    }
    /* Set the input and output addresses and start DMA transfer */ 
    CRYP_SetDMAConfig(hcryp, inputaddr, Size, outputaddr);
    
    /* Process Unlocked */
    __HAL_UNLOCK(hcryp);
     
    /* Return function status */
    return HAL_OK;
  }
  else
  {
    return HAL_ERROR;   
  }
}

/**
  * @brief  Initializes the CRYP peripheral in AES CBC encryption mode using DMA.
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  pPlainData: Pointer to the plaintext buffer
  * @param  Size: Length of the plaintext buffer, must be a multiple of 16.
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYP_AESCBC_Encrypt_DMA(CRYP_HandleTypeDef *hcryp, uint8_t *pPlainData, uint16_t Size, uint8_t *pCypherData)
{
  uint32_t inputaddr;
  uint32_t outputaddr;
  
  if((hcryp->State == HAL_CRYP_STATE_READY) || (hcryp->Phase == HAL_CRYP_PHASE_PROCESS))
  {
    /* Process Locked */
    __HAL_LOCK(hcryp);
    
    inputaddr  = (uint32_t)pPlainData;
    outputaddr = (uint32_t)pCypherData;
    
    /* Change the CRYP state */
    hcryp->State = HAL_CRYP_STATE_BUSY;
  
    /* Check if initialization phase has already been performed */
    if(hcryp->Phase == HAL_CRYP_PHASE_READY)
    {
      /* Set the key */
      CRYP_SetKey(hcryp, hcryp->Init.pKey, hcryp->Init.KeySize);
      
      /* Set the CRYP peripheral in AES ECB mode */
      __HAL_CRYP_SET_MODE(CRYP_CR_ALGOMODE_AES_CBC);
      
      /* Set the Initialization Vector */
      CRYP_SetInitVector(hcryp, hcryp->Init.pInitVect, CRYP_KEYSIZE_128B);
      
      /* Flush FIFO */
      __HAL_CRYP_FIFO_FLUSH();
      
       /* Set the phase */
       hcryp->Phase = HAL_CRYP_PHASE_PROCESS;
     }
     /* Set the input and output addresses and start DMA transfer */ 
     CRYP_SetDMAConfig(hcryp, inputaddr, Size, outputaddr);
     
     /* Process Unlocked */
     __HAL_UNLOCK(hcryp);
     
     /* Return function status */
     return HAL_OK;
  }
  else
  {
    return HAL_ERROR;   
  }
}

/**
  * @brief  Initializes the CRYP peripheral in AES CTR encryption mode using DMA.
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  pPlainData: Pointer to the plaintext buffer
  * @param  Size: Length of the plaintext buffer, must be a multiple of 16.
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYP_AESCTR_Encrypt_DMA(CRYP_HandleTypeDef *hcryp, uint8_t *pPlainData, uint16_t Size, uint8_t *pCypherData)
{
  uint32_t inputaddr;
  uint32_t outputaddr;
  
  if((hcryp->State == HAL_CRYP_STATE_READY) || (hcryp->Phase == HAL_CRYP_PHASE_PROCESS))
  {
    /* Process Locked */
    __HAL_LOCK(hcryp);
    
    inputaddr  = (uint32_t)pPlainData;
    outputaddr = (uint32_t)pCypherData;
    
  /* Change the CRYP state */
  hcryp->State = HAL_CRYP_STATE_BUSY;
  
    /* Check if initialization phase has already been performed */
    if(hcryp->Phase == HAL_CRYP_PHASE_READY)
    {
      /* Set the key */
      CRYP_SetKey(hcryp, hcryp->Init.pKey, hcryp->Init.KeySize);
      
      /* Set the CRYP peripheral in AES ECB mode */
      __HAL_CRYP_SET_MODE(CRYP_CR_ALGOMODE_AES_CTR);
      
      /* Set the Initialization Vector */
      CRYP_SetInitVector(hcryp, hcryp->Init.pInitVect, CRYP_KEYSIZE_128B);
      
      /* Flush FIFO */
      __HAL_CRYP_FIFO_FLUSH();
      
       /* Set the phase */
       hcryp->Phase = HAL_CRYP_PHASE_PROCESS;
    }
    
    /* Set the input and output addresses and start DMA transfer */ 
    CRYP_SetDMAConfig(hcryp, inputaddr, Size, outputaddr);
    
    /* Process Unlocked */
    __HAL_UNLOCK(hcryp);
    
    /* Return function status */
    return HAL_OK;
  }
  else
  {
    return HAL_ERROR;   
  }
}

/**
  * @brief  Initializes the CRYP peripheral in AES ECB decryption mode using DMA.
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @param  Size: Length of the plaintext buffer, must be a multiple of 16 bytes
  * @param  pPlainData: Pointer to the plaintext buffer
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYP_AESECB_Decrypt_DMA(CRYP_HandleTypeDef *hcryp, uint8_t *pCypherData, uint16_t Size, uint8_t *pPlainData)
{
  uint32_t tickstart = 0;   
  uint32_t inputaddr;
  uint32_t outputaddr;
  
  if((hcryp->State == HAL_CRYP_STATE_READY) || (hcryp->Phase == HAL_CRYP_PHASE_PROCESS))
  {
    /* Process Locked */
    __HAL_LOCK(hcryp);
    
    inputaddr  = (uint32_t)pCypherData;
    outputaddr = (uint32_t)pPlainData;
    
    /* Change the CRYP state */
    hcryp->State = HAL_CRYP_STATE_BUSY;
    
    /* Check if initialization phase has already been performed */
    if(hcryp->Phase == HAL_CRYP_PHASE_READY)
    {
    /* Set the key */
    CRYP_SetKey(hcryp, hcryp->Init.pKey, hcryp->Init.KeySize);
    
    /* Set the CRYP peripheral in AES Key mode */
    __HAL_CRYP_SET_MODE(CRYP_CR_ALGOMODE_AES_KEY | CRYP_CR_ALGODIR);
    
    /* Enable CRYP */
    __HAL_CRYP_ENABLE();
    
    /* Get tick */
    tickstart = HAL_GetTick();
    
    while(HAL_IS_BIT_SET(CRYP->SR, CRYP_FLAG_BUSY))
    {
      /* Check for the Timeout */
      if((HAL_GetTick() - tickstart ) > CRYP_TIMEOUT_VALUE)
      {
        /* Change state */
        hcryp->State = HAL_CRYP_STATE_TIMEOUT;
        
        /* Process Unlocked */
        __HAL_UNLOCK(hcryp);
        
        return HAL_TIMEOUT;
      }
    }
    
    /* Reset the ALGOMODE bits*/
    CRYP->CR &= (uint32_t)(~CRYP_CR_ALGOMODE);
    
    /* Set the CRYP peripheral in AES ECB decryption mode */
    __HAL_CRYP_SET_MODE(CRYP_CR_ALGOMODE_AES_ECB | CRYP_CR_ALGODIR);
    
    /* Flush FIFO */
    __HAL_CRYP_FIFO_FLUSH();
    
     /* Set the phase */
     hcryp->Phase = HAL_CRYP_PHASE_PROCESS;
    }
     
    /* Set the input and output addresses and start DMA transfer */ 
    CRYP_SetDMAConfig(hcryp, inputaddr, Size, outputaddr);
    
     /* Process Unlocked */
     __HAL_UNLOCK(hcryp);
    
    /* Return function status */
    return HAL_OK;
  }
  else
  {
    return HAL_ERROR;   
  }
}

/**
  * @brief  Initializes the CRYP peripheral in AES CBC encryption mode using DMA.
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @param  Size: Length of the plaintext buffer, must be a multiple of 16 bytes
  * @param  pPlainData: Pointer to the plaintext buffer
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYP_AESCBC_Decrypt_DMA(CRYP_HandleTypeDef *hcryp, uint8_t *pCypherData, uint16_t Size, uint8_t *pPlainData)
{
  uint32_t tickstart = 0;   
  uint32_t inputaddr;
  uint32_t outputaddr;
  
  if((hcryp->State == HAL_CRYP_STATE_READY) || (hcryp->Phase == HAL_CRYP_PHASE_PROCESS))
  {
    /* Process Locked */
    __HAL_LOCK(hcryp);
    
    inputaddr  = (uint32_t)pCypherData;
    outputaddr = (uint32_t)pPlainData;
    
    /* Change the CRYP state */
    hcryp->State = HAL_CRYP_STATE_BUSY;
    
    /* Check if initialization phase has already been performed */
    if(hcryp->Phase == HAL_CRYP_PHASE_READY)
    {
      /* Set the key */
      CRYP_SetKey(hcryp, hcryp->Init.pKey, hcryp->Init.KeySize);
      
      /* Set the CRYP peripheral in AES Key mode */
      __HAL_CRYP_SET_MODE(CRYP_CR_ALGOMODE_AES_KEY | CRYP_CR_ALGODIR);
      
      /* Enable CRYP */
      __HAL_CRYP_ENABLE();
      
      /* Get tick */
      tickstart = HAL_GetTick();

      while(HAL_IS_BIT_SET(CRYP->SR, CRYP_FLAG_BUSY))
      {
        /* Check for the Timeout */
        if((HAL_GetTick() - tickstart ) > CRYP_TIMEOUT_VALUE)
        {
          /* Change state */
          hcryp->State = HAL_CRYP_STATE_TIMEOUT;
          
          /* Process Unlocked */
          __HAL_UNLOCK(hcryp);
          
          return HAL_TIMEOUT;
        }
      }
      
      /* Reset the ALGOMODE bits*/
      CRYP->CR &= (uint32_t)(~CRYP_CR_ALGOMODE);
      
      /* Set the CRYP peripheral in AES CBC decryption mode */
      __HAL_CRYP_SET_MODE(CRYP_CR_ALGOMODE_AES_CBC | CRYP_CR_ALGODIR);
      
      /* Set the Initialization Vector */
      CRYP_SetInitVector(hcryp, hcryp->Init.pInitVect, CRYP_KEYSIZE_128B);
      
      /* Flush FIFO */
      __HAL_CRYP_FIFO_FLUSH();
      
      /* Set the phase */
      hcryp->Phase = HAL_CRYP_PHASE_PROCESS;
    }
    
    /* Set the input and output addresses and start DMA transfer */ 
    CRYP_SetDMAConfig(hcryp, inputaddr, Size, outputaddr);
    
    /* Process Unlocked */
    __HAL_UNLOCK(hcryp);
    
    /* Return function status */
    return HAL_OK;
  }
  else
  {
    return HAL_ERROR;   
  }
}

/**
  * @brief  Initializes the CRYP peripheral in AES CTR decryption mode using DMA.
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @param  Size: Length of the plaintext buffer, must be a multiple of 16
  * @param  pPlainData: Pointer to the plaintext buffer
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYP_AESCTR_Decrypt_DMA(CRYP_HandleTypeDef *hcryp, uint8_t *pCypherData, uint16_t Size, uint8_t *pPlainData)
{  
  uint32_t inputaddr;
  uint32_t outputaddr;
  
  if((hcryp->State == HAL_CRYP_STATE_READY) || (hcryp->Phase == HAL_CRYP_PHASE_PROCESS))
  {
    /* Process Locked */
    __HAL_LOCK(hcryp);
    
    inputaddr  = (uint32_t)pCypherData;
    outputaddr = (uint32_t)pPlainData;
    
    /* Change the CRYP state */
    hcryp->State = HAL_CRYP_STATE_BUSY;
    
    /* Check if initialization phase has already been performed */
    if(hcryp->Phase == HAL_CRYP_PHASE_READY)
    {
      /* Set the key */
      CRYP_SetKey(hcryp, hcryp->Init.pKey, hcryp->Init.KeySize);
      
      /* Set the CRYP peripheral in AES CTR mode */
      __HAL_CRYP_SET_MODE(CRYP_CR_ALGOMODE_AES_CTR | CRYP_CR_ALGODIR);
      
      /* Set the Initialization Vector */
      CRYP_SetInitVector(hcryp, hcryp->Init.pInitVect, CRYP_KEYSIZE_128B);
      
      /* Flush FIFO */
      __HAL_CRYP_FIFO_FLUSH();
      
      /* Set the phase */
      hcryp->Phase = HAL_CRYP_PHASE_PROCESS;
    }
    
    /* Set the input and output addresses and start DMA transfer */ 
    CRYP_SetDMAConfig(hcryp, inputaddr, Size, outputaddr);
    
    /* Process Unlocked */
    __HAL_UNLOCK(hcryp);
    
    /* Return function status */
    return HAL_OK;
  }
  else
  {
    return HAL_ERROR;   
  }
}


/**
  * @}
  */
/** @defgroup CRYP_Group3 DES processing functions 
 *  @brief   processing functions. 
 *
@verbatim   
  ==============================================================================
                      ##### DES processing functions #####
  ==============================================================================  
    [..]  This section provides functions allowing to:
      (+) Encrypt plaintext using DES using ECB or CBC chaining modes
      (+) Decrypt cyphertext using ECB or CBC chaining modes
    [..]  Three processing functions are available:
      (+) Polling mode
      (+) Interrupt mode
      (+) DMA mode

@endverbatim
  * @{
  */

/**
  * @brief  Initializes the CRYP peripheral in DES ECB encryption mode.
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  pPlainData: Pointer to the plaintext buffer
  * @param  Size: Length of the plaintext buffer, must be a multiple of 8
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @param  Timeout: Specify Timeout value  
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYP_DESECB_Encrypt(CRYP_HandleTypeDef *hcryp, uint8_t *pPlainData, uint16_t Size, uint8_t *pCypherData, uint32_t Timeout)
{
  /* Process Locked */
  __HAL_LOCK(hcryp);
  
  /* Change the CRYP state */
  hcryp->State = HAL_CRYP_STATE_BUSY;
  
  /* Set CRYP peripheral in DES ECB encryption mode */
  CRYP_SetDESECBMode(hcryp, 0);
  
  /* Enable CRYP */
  __HAL_CRYP_ENABLE();
  
  /* Write Plain Data and Get Cypher Data */
  if(CRYP_ProcessData2Words(hcryp, pPlainData, Size, pCypherData, Timeout) != HAL_OK)
  {
    return HAL_TIMEOUT;
  }
  
  /* Change the CRYP state */
  hcryp->State = HAL_CRYP_STATE_READY;
  
  /* Process Unlocked */
  __HAL_UNLOCK(hcryp);
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Initializes the CRYP peripheral in DES ECB decryption mode.
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  pPlainData: Pointer to the plaintext buffer
  * @param  Size: Length of the plaintext buffer, must be a multiple of 8
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @param  Timeout: Specify Timeout value  
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYP_DESECB_Decrypt(CRYP_HandleTypeDef *hcryp, uint8_t *pPlainData, uint16_t Size, uint8_t *pCypherData, uint32_t Timeout)
{
  /* Process Locked */
  __HAL_LOCK(hcryp);
  
  /* Change the CRYP state */
  hcryp->State = HAL_CRYP_STATE_BUSY;
  
  /* Set CRYP peripheral in DES ECB decryption mode */
  CRYP_SetDESECBMode(hcryp, CRYP_CR_ALGODIR);
  
  /* Enable CRYP */
  __HAL_CRYP_ENABLE();
  
  /* Write Plain Data and Get Cypher Data */
  if(CRYP_ProcessData2Words(hcryp, pPlainData, Size, pCypherData, Timeout) != HAL_OK)
  {
    return HAL_TIMEOUT;
  }
  
  /* Change the CRYP state */
  hcryp->State = HAL_CRYP_STATE_READY;
  
  /* Process Unlocked */
  __HAL_UNLOCK(hcryp);
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Initializes the CRYP peripheral in DES CBC encryption mode.
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  pPlainData: Pointer to the plaintext buffer
  * @param  Size: Length of the plaintext buffer, must be a multiple of 8
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @param  Timeout: Specify Timeout value  
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYP_DESCBC_Encrypt(CRYP_HandleTypeDef *hcryp, uint8_t *pPlainData, uint16_t Size, uint8_t *pCypherData, uint32_t Timeout)
{
  /* Process Locked */
  __HAL_LOCK(hcryp);
  
  /* Change the CRYP state */
  hcryp->State = HAL_CRYP_STATE_BUSY;
  
  /* Set CRYP peripheral in DES CBC encryption mode */
  CRYP_SetDESCBCMode(hcryp, 0);
  
  /* Enable CRYP */
  __HAL_CRYP_ENABLE();
  
  /* Write Plain Data and Get Cypher Data */
  if(CRYP_ProcessData2Words(hcryp, pPlainData, Size, pCypherData, Timeout) != HAL_OK)
  {
    return HAL_TIMEOUT;
  }
  
  /* Change the CRYP state */
  hcryp->State = HAL_CRYP_STATE_READY;
  
  /* Process Unlocked */
  __HAL_UNLOCK(hcryp);
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Initializes the CRYP peripheral in DES ECB decryption mode.
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  pPlainData: Pointer to the plaintext buffer
  * @param  Size: Length of the plaintext buffer, must be a multiple of 8
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @param  Timeout: Specify Timeout value  
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYP_DESCBC_Decrypt(CRYP_HandleTypeDef *hcryp, uint8_t *pPlainData, uint16_t Size, uint8_t *pCypherData, uint32_t Timeout)
{
  /* Process Locked */
  __HAL_LOCK(hcryp);
  
  /* Change the CRYP state */
  hcryp->State = HAL_CRYP_STATE_BUSY;
  
  /* Set CRYP peripheral in DES CBC decryption mode */
  CRYP_SetDESCBCMode(hcryp, CRYP_CR_ALGODIR);
  
  /* Enable CRYP */
  __HAL_CRYP_ENABLE();
  
  /* Write Plain Data and Get Cypher Data */
  if(CRYP_ProcessData2Words(hcryp, pPlainData, Size, pCypherData, Timeout) != HAL_OK)
  {
    return HAL_TIMEOUT;
  }
  
  /* Change the CRYP state */
  hcryp->State = HAL_CRYP_STATE_READY;
  
  /* Process Unlocked */
  __HAL_UNLOCK(hcryp);
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Initializes the CRYP peripheral in DES ECB encryption mode using IT.
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  pPlainData: Pointer to the plaintext buffer
  * @param  Size: Length of the plaintext buffer, must be a multiple of 8
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYP_DESECB_Encrypt_IT(CRYP_HandleTypeDef *hcryp, uint8_t *pPlainData, uint16_t Size, uint8_t *pCypherData)
{
  uint32_t inputaddr;
  uint32_t outputaddr;
  
  if(hcryp->State == HAL_CRYP_STATE_READY)
  {
    /* Process Locked */
    __HAL_LOCK(hcryp);
    
    hcryp->CrypInCount = Size;
    hcryp->pCrypInBuffPtr = pPlainData;
    hcryp->pCrypOutBuffPtr = pCypherData;
    hcryp->CrypOutCount = Size;
    
    /* Change the CRYP state */
    hcryp->State = HAL_CRYP_STATE_BUSY;
    
    /* Set CRYP peripheral in DES ECB encryption mode */
    CRYP_SetDESECBMode(hcryp, 0);
    
    /* Enable Interrupts */
    __HAL_CRYP_ENABLE_IT(CRYP_IT_INI | CRYP_IT_OUTI);
    
    /* Enable CRYP */
    __HAL_CRYP_ENABLE();
    
    /* Return function status */
    return HAL_OK;
  }
  else if(__HAL_CRYP_GET_IT(CRYP_IT_INI))
  {
    inputaddr = (uint32_t)hcryp->pCrypInBuffPtr;
    /* Write the Input block in the IN FIFO */
    CRYP->DR = *(uint32_t*)(inputaddr);
    inputaddr+=4;
    CRYP->DR = *(uint32_t*)(inputaddr);
    
    hcryp->pCrypInBuffPtr += 8;
    hcryp->CrypInCount -= 8;
    if(hcryp->CrypInCount == 0)
    {
      __HAL_CRYP_DISABLE_IT(CRYP_IT_INI);
      /* Call the Input data transfer complete callback */
      HAL_CRYP_InCpltCallback(hcryp);
    }
  }
  else if(__HAL_CRYP_GET_IT(CRYP_IT_OUTI))
  {
    outputaddr = (uint32_t)hcryp->pCrypOutBuffPtr;
    /* Read the Output block from the Output FIFO */
    *(uint32_t*)(outputaddr) = CRYP->DOUT;
    outputaddr+=4;
    *(uint32_t*)(outputaddr) = CRYP->DOUT;
    
    hcryp->pCrypOutBuffPtr += 8;
    hcryp->CrypOutCount -= 8;
    if(hcryp->CrypOutCount == 0)
    {
      /* Disable IT */
      __HAL_CRYP_DISABLE_IT(CRYP_IT_OUTI);
      /* Disable CRYP */
      __HAL_CRYP_DISABLE();
      /* Process Unlocked */
      __HAL_UNLOCK(hcryp);
      /* Change the CRYP state */
      hcryp->State = HAL_CRYP_STATE_READY;
      /* Call Input transfer complete callback */
      HAL_CRYP_OutCpltCallback(hcryp);
    }
  }
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Initializes the CRYP peripheral in DES CBC encryption mode using interrupt.
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  pPlainData: Pointer to the plaintext buffer
  * @param  Size: Length of the plaintext buffer, must be a multiple of 8
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYP_DESCBC_Encrypt_IT(CRYP_HandleTypeDef *hcryp, uint8_t *pPlainData, uint16_t Size, uint8_t *pCypherData)
{
  uint32_t inputaddr;
  uint32_t outputaddr;
  
  if(hcryp->State == HAL_CRYP_STATE_READY)
  {
    /* Process Locked */
    __HAL_LOCK(hcryp);
    
    hcryp->CrypInCount = Size;
    hcryp->pCrypInBuffPtr = pPlainData;
    hcryp->pCrypOutBuffPtr = pCypherData;
    hcryp->CrypOutCount = Size;
    
    /* Change the CRYP state */
    hcryp->State = HAL_CRYP_STATE_BUSY;
    
    /* Set CRYP peripheral in DES CBC encryption mode */
    CRYP_SetDESCBCMode(hcryp, 0);
    
    /* Enable Interrupts */
    __HAL_CRYP_ENABLE_IT(CRYP_IT_INI | CRYP_IT_OUTI);
    
    /* Enable CRYP */
    __HAL_CRYP_ENABLE();
    
    /* Return function status */
    return HAL_OK;
  }
  
  else if(__HAL_CRYP_GET_IT(CRYP_IT_INI))
  {
    inputaddr = (uint32_t)hcryp->pCrypInBuffPtr;
    /* Write the Input block in the IN FIFO */
    CRYP->DR = *(uint32_t*)(inputaddr);
    inputaddr+=4;
    CRYP->DR = *(uint32_t*)(inputaddr);

    hcryp->pCrypInBuffPtr += 8;
    hcryp->CrypInCount -= 8;
    if(hcryp->CrypInCount == 0)
    {
      __HAL_CRYP_DISABLE_IT(CRYP_IT_INI);
      /* Call the Input data transfer complete callback */
      HAL_CRYP_InCpltCallback(hcryp);
    }
  }
  else if(__HAL_CRYP_GET_IT(CRYP_IT_OUTI))
  {
    outputaddr = (uint32_t)hcryp->pCrypOutBuffPtr;
    /* Read the Output block from the Output FIFO */
    *(uint32_t*)(outputaddr) = CRYP->DOUT;
    outputaddr+=4;
    *(uint32_t*)(outputaddr) = CRYP->DOUT;

    hcryp->pCrypOutBuffPtr += 8;
    hcryp->CrypOutCount -= 8;
    if(hcryp->CrypOutCount == 0)
    {
      /* Disable IT */
      __HAL_CRYP_DISABLE_IT(CRYP_IT_OUTI);
      /* Disable CRYP */
      __HAL_CRYP_DISABLE();
      /* Process Unlocked */
      __HAL_UNLOCK(hcryp);
      /* Change the CRYP state */
      hcryp->State = HAL_CRYP_STATE_READY;
      /* Call Input transfer complete callback */
      HAL_CRYP_OutCpltCallback(hcryp);
    }
  }
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Initializes the CRYP peripheral in DES ECB decryption mode using IT.
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  pPlainData: Pointer to the plaintext buffer
  * @param  Size: Length of the plaintext buffer, must be a multiple of 8
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYP_DESECB_Decrypt_IT(CRYP_HandleTypeDef *hcryp, uint8_t *pCypherData, uint16_t Size, uint8_t *pPlainData)
{
  uint32_t inputaddr;
  uint32_t outputaddr;
  
  if(hcryp->State == HAL_CRYP_STATE_READY)
  {
    /* Process Locked */
    __HAL_LOCK(hcryp);
    
    hcryp->CrypInCount = Size;
    hcryp->pCrypInBuffPtr = pCypherData;
    hcryp->pCrypOutBuffPtr = pPlainData;
    hcryp->CrypOutCount = Size;
    
    /* Change the CRYP state */
    hcryp->State = HAL_CRYP_STATE_BUSY;
    
    /* Set CRYP peripheral in DES ECB decryption mode */
    CRYP_SetDESECBMode(hcryp, CRYP_CR_ALGODIR);
    
    /* Enable Interrupts */
    __HAL_CRYP_ENABLE_IT(CRYP_IT_INI | CRYP_IT_OUTI);
    
    /* Enable CRYP */
    __HAL_CRYP_ENABLE();
    
    /* Return function status */
    return HAL_OK;
  }
  else if(__HAL_CRYP_GET_IT(CRYP_IT_INI))
  {
    inputaddr = (uint32_t)hcryp->pCrypInBuffPtr;
    /* Write the Input block in the IN FIFO */
    CRYP->DR = *(uint32_t*)(inputaddr);
    inputaddr+=4;
    CRYP->DR = *(uint32_t*)(inputaddr);
    
    hcryp->pCrypInBuffPtr += 8;
    hcryp->CrypInCount -= 8;
    if(hcryp->CrypInCount == 0)
    {
      __HAL_CRYP_DISABLE_IT(CRYP_IT_INI);
      /* Call the Input data transfer complete callback */
      HAL_CRYP_InCpltCallback(hcryp);
    }
  }
  else if(__HAL_CRYP_GET_IT(CRYP_IT_OUTI))
  {
    outputaddr = (uint32_t)hcryp->pCrypOutBuffPtr;
    /* Read the Output block from the Output FIFO */
    *(uint32_t*)(outputaddr) = CRYP->DOUT;
    outputaddr+=4;
    *(uint32_t*)(outputaddr) = CRYP->DOUT;

    hcryp->pCrypOutBuffPtr += 8;
    hcryp->CrypOutCount -= 8;
    if(hcryp->CrypOutCount == 0)
    {
      /* Disable IT */
      __HAL_CRYP_DISABLE_IT(CRYP_IT_OUTI);
      /* Disable CRYP */
      __HAL_CRYP_DISABLE();
      /* Process Unlocked */
      __HAL_UNLOCK(hcryp);
      /* Change the CRYP state */
      hcryp->State = HAL_CRYP_STATE_READY;
      /* Call Input transfer complete callback */
      HAL_CRYP_OutCpltCallback(hcryp);
    }
  }
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Initializes the CRYP peripheral in DES ECB decryption mode using interrupt.
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  pPlainData: Pointer to the plaintext buffer
  * @param  Size: Length of the plaintext buffer, must be a multiple of 8
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYP_DESCBC_Decrypt_IT(CRYP_HandleTypeDef *hcryp, uint8_t *pCypherData, uint16_t Size, uint8_t *pPlainData)
{
  uint32_t inputaddr;
  uint32_t outputaddr;
  
  if(hcryp->State == HAL_CRYP_STATE_READY)
  {
    /* Process Locked */
    __HAL_LOCK(hcryp);
    
    hcryp->CrypInCount = Size;
    hcryp->pCrypInBuffPtr = pCypherData;
    hcryp->pCrypOutBuffPtr = pPlainData;
    hcryp->CrypOutCount = Size;
    
    /* Change the CRYP state */
    hcryp->State = HAL_CRYP_STATE_BUSY;
    
    /* Set CRYP peripheral in DES CBC decryption mode */
    CRYP_SetDESCBCMode(hcryp, CRYP_CR_ALGODIR);
    
    /* Enable Interrupts */
    __HAL_CRYP_ENABLE_IT(CRYP_IT_INI | CRYP_IT_OUTI);
    
    /* Enable CRYP */
    __HAL_CRYP_ENABLE();
    
    /* Return function status */
    return HAL_OK;
  }
  else if(__HAL_CRYP_GET_IT(CRYP_IT_INI))
  {
    inputaddr = (uint32_t)hcryp->pCrypInBuffPtr;
    /* Write the Input block in the IN FIFO */
    CRYP->DR = *(uint32_t*)(inputaddr);
    inputaddr+=4;
    CRYP->DR = *(uint32_t*)(inputaddr);

    hcryp->pCrypInBuffPtr += 8;
    hcryp->CrypInCount -= 8;
    if(hcryp->CrypInCount == 0)
    {
      __HAL_CRYP_DISABLE_IT(CRYP_IT_INI);
      /* Call the Input data transfer complete callback */
      HAL_CRYP_InCpltCallback(hcryp);
    }
  }
  else if(__HAL_CRYP_GET_IT(CRYP_IT_OUTI))
  {
    outputaddr = (uint32_t)hcryp->pCrypOutBuffPtr;
    /* Read the Output block from the Output FIFO */
    *(uint32_t*)(outputaddr) = CRYP->DOUT;
    outputaddr+=4;
    *(uint32_t*)(outputaddr) = CRYP->DOUT;

    hcryp->pCrypOutBuffPtr += 8;
    hcryp->CrypOutCount -= 8;
    if(hcryp->CrypOutCount == 0)
    {
      /* Disable IT */
      __HAL_CRYP_DISABLE_IT(CRYP_IT_OUTI);
      /* Disable CRYP */
      __HAL_CRYP_DISABLE();
      /* Process Unlocked */
      __HAL_UNLOCK(hcryp);
      /* Change the CRYP state */
      hcryp->State = HAL_CRYP_STATE_READY;
      /* Call Input transfer complete callback */
      HAL_CRYP_OutCpltCallback(hcryp);
    }
  }
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Initializes the CRYP peripheral in DES ECB encryption mode using DMA.
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  pPlainData: Pointer to the plaintext buffer
  * @param  Size: Length of the plaintext buffer, must be a multiple of 8
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYP_DESECB_Encrypt_DMA(CRYP_HandleTypeDef *hcryp, uint8_t *pPlainData, uint16_t Size, uint8_t *pCypherData)
{
  uint32_t inputaddr;
  uint32_t outputaddr;
  
  if((hcryp->State == HAL_CRYP_STATE_READY) || (hcryp->Phase == HAL_CRYP_PHASE_PROCESS))
  {
    /* Process Locked */
    __HAL_LOCK(hcryp);
    
    inputaddr  = (uint32_t)pPlainData;
    outputaddr = (uint32_t)pCypherData;
    
    /* Change the CRYP state */
    hcryp->State = HAL_CRYP_STATE_BUSY;
    
    /* Set CRYP peripheral in DES ECB encryption mode */
    CRYP_SetDESECBMode(hcryp, 0);
    
    /* Set the input and output addresses and start DMA transfer */ 
    CRYP_SetDMAConfig(hcryp, inputaddr, Size, outputaddr);
    
    /* Process Unlocked */
    __HAL_UNLOCK(hcryp);
    
    /* Return function status */
    return HAL_OK;
  }
  else
  {
    return HAL_ERROR;   
  }
}

/**
  * @brief  Initializes the CRYP peripheral in DES CBC encryption mode using DMA.
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  pPlainData: Pointer to the plaintext buffer
  * @param  Size: Length of the plaintext buffer, must be a multiple of 8
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYP_DESCBC_Encrypt_DMA(CRYP_HandleTypeDef *hcryp, uint8_t *pPlainData, uint16_t Size, uint8_t *pCypherData)
{
  uint32_t inputaddr;
  uint32_t outputaddr;
  
  if((hcryp->State == HAL_CRYP_STATE_READY) || (hcryp->Phase == HAL_CRYP_PHASE_PROCESS))
  {
    /* Process Locked */
    __HAL_LOCK(hcryp);
    
    inputaddr  = (uint32_t)pPlainData;
    outputaddr = (uint32_t)pCypherData;
    
    /* Change the CRYP state */
    hcryp->State = HAL_CRYP_STATE_BUSY;
    
    /* Set CRYP peripheral in DES CBC encryption mode */
    CRYP_SetDESCBCMode(hcryp, 0);
    
    /* Set the input and output addresses and start DMA transfer */ 
    CRYP_SetDMAConfig(hcryp, inputaddr, Size, outputaddr);
    
    /* Process Unlocked */
    __HAL_UNLOCK(hcryp);
    
    /* Return function status */
    return HAL_OK;
  }
  else
  {
    return HAL_ERROR;   
  }
}

/**
  * @brief  Initializes the CRYP peripheral in DES ECB decryption mode using DMA.
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  pPlainData: Pointer to the plaintext buffer
  * @param  Size: Length of the plaintext buffer, must be a multiple of 8
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYP_DESECB_Decrypt_DMA(CRYP_HandleTypeDef *hcryp, uint8_t *pPlainData, uint16_t Size, uint8_t *pCypherData)
{
  uint32_t inputaddr;
  uint32_t outputaddr;
  
  if((hcryp->State == HAL_CRYP_STATE_READY) || (hcryp->Phase == HAL_CRYP_PHASE_PROCESS))
  {
    /* Process Locked */
    __HAL_LOCK(hcryp);
    
    inputaddr  = (uint32_t)pCypherData;
    outputaddr = (uint32_t)pPlainData;
    
    /* Change the CRYP state */
    hcryp->State = HAL_CRYP_STATE_BUSY;
    
    /* Set CRYP peripheral in DES ECB decryption mode */
    CRYP_SetDESECBMode(hcryp, CRYP_CR_ALGODIR);
    
    /* Set the input and output addresses and start DMA transfer */ 
    CRYP_SetDMAConfig(hcryp, inputaddr, Size, outputaddr);
    
    /* Process Unlocked */
    __HAL_UNLOCK(hcryp);
    
    /* Return function status */
    return HAL_OK;
  }
  else
  {
    return HAL_ERROR;   
  }
}

/**
  * @brief  Initializes the CRYP peripheral in DES ECB decryption mode using DMA.
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  pPlainData: Pointer to the plaintext buffer
  * @param  Size: Length of the plaintext buffer, must be a multiple of 8
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYP_DESCBC_Decrypt_DMA(CRYP_HandleTypeDef *hcryp, uint8_t *pCypherData, uint16_t Size, uint8_t *pPlainData)
{
  uint32_t inputaddr;
  uint32_t outputaddr;
  
  if((hcryp->State == HAL_CRYP_STATE_READY) || (hcryp->Phase == HAL_CRYP_PHASE_PROCESS))
  {
    /* Process Locked */
    __HAL_LOCK(hcryp);
    
    inputaddr  = (uint32_t)pCypherData;
    outputaddr = (uint32_t)pPlainData;
    
    /* Change the CRYP state */
    hcryp->State = HAL_CRYP_STATE_BUSY;
    
    /* Set CRYP peripheral in DES CBC decryption mode */
    CRYP_SetDESCBCMode(hcryp, CRYP_CR_ALGODIR);
    
    /* Set the input and output addresses and start DMA transfer */ 
    CRYP_SetDMAConfig(hcryp, inputaddr, Size, outputaddr);
    
    /* Process Unlocked */
    __HAL_UNLOCK(hcryp);
    
    /* Return function status */
    return HAL_OK;
  }
  else
  {
    return HAL_ERROR;   
  }
}

/**
  * @}
  */

/** @defgroup CRYP_Group4 TDES processing functions 
 *  @brief   processing functions. 
 *
@verbatim   
  ==============================================================================
                      ##### TDES processing functions #####
  ==============================================================================  
    [..]  This section provides functions allowing to:
      (+) Encrypt plaintext using TDES based on ECB or CBC chaining modes
      (+) Decrypt cyphertext using TDES based on ECB or CBC chaining modes
    [..]  Three processing functions are available:
      (+) Polling mode
      (+) Interrupt mode
      (+) DMA mode

@endverbatim
  * @{
  */

/**
  * @brief  Initializes the CRYP peripheral in TDES ECB encryption mode
  *         then encrypt pPlainData. The cypher data are available in pCypherData
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  pPlainData: Pointer to the plaintext buffer
  * @param  Size: Length of the plaintext buffer, must be a multiple of 8
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @param  Timeout: Specify Timeout value  
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYP_TDESECB_Encrypt(CRYP_HandleTypeDef *hcryp, uint8_t *pPlainData, uint16_t Size, uint8_t *pCypherData, uint32_t Timeout)
{
  /* Process Locked */
  __HAL_LOCK(hcryp);
  
  /* Change the CRYP state */
  hcryp->State = HAL_CRYP_STATE_BUSY;
  
  /* Set CRYP peripheral in TDES ECB encryption mode */
  CRYP_SetTDESECBMode(hcryp, 0);
  
  /* Enable CRYP */
  __HAL_CRYP_ENABLE();
  
  /* Write Plain Data and Get Cypher Data */
  if(CRYP_ProcessData2Words(hcryp, pPlainData, Size, pCypherData, Timeout) != HAL_OK)
  {
    return HAL_TIMEOUT;
  }
  
  /* Change the CRYP state */
  hcryp->State = HAL_CRYP_STATE_READY;
  
  /* Process Unlocked */
  __HAL_UNLOCK(hcryp);
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Initializes the CRYP peripheral in TDES ECB decryption mode
  *         then decrypted pCypherData. The cypher data are available in pPlainData
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  pPlainData: Pointer to the plaintext buffer
  * @param  Size: Length of the plaintext buffer, must be a multiple of 8
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @param  Timeout: Specify Timeout value  
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYP_TDESECB_Decrypt(CRYP_HandleTypeDef *hcryp, uint8_t *pCypherData, uint16_t Size, uint8_t *pPlainData, uint32_t Timeout)
{  
  /* Process Locked */
  __HAL_LOCK(hcryp);
  
  /* Change the CRYP state */
  hcryp->State = HAL_CRYP_STATE_BUSY;
  
  /* Set CRYP peripheral in TDES ECB decryption mode */
  CRYP_SetTDESECBMode(hcryp, CRYP_CR_ALGODIR);
  
  /* Enable CRYP */
  __HAL_CRYP_ENABLE();
  
  /* Write Cypher Data and Get Plain Data */
  if(CRYP_ProcessData2Words(hcryp, pCypherData, Size, pPlainData, Timeout) != HAL_OK)
  {
    return HAL_TIMEOUT;
  }
  
  /* Change the CRYP state */
  hcryp->State = HAL_CRYP_STATE_READY;
  
  /* Process Unlocked */
  __HAL_UNLOCK(hcryp);
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Initializes the CRYP peripheral in TDES CBC encryption mode
  *         then encrypt pPlainData. The cypher data are available in pCypherData
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  pPlainData: Pointer to the plaintext buffer
  * @param  Size: Length of the plaintext buffer, must be a multiple of 8
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @param  Timeout: Specify Timeout value  
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYP_TDESCBC_Encrypt(CRYP_HandleTypeDef *hcryp, uint8_t *pPlainData, uint16_t Size, uint8_t *pCypherData, uint32_t Timeout)
{
  /* Process Locked */
  __HAL_LOCK(hcryp);
  
  /* Change the CRYP state */
  hcryp->State = HAL_CRYP_STATE_BUSY;
  
  /* Set CRYP peripheral in TDES CBC encryption mode */
  CRYP_SetTDESCBCMode(hcryp, 0);
  
  /* Enable CRYP */
  __HAL_CRYP_ENABLE();
  
  /* Write Plain Data and Get Cypher Data */
  if(CRYP_ProcessData2Words(hcryp, pPlainData, Size, pCypherData, Timeout) != HAL_OK)
  {
    return HAL_TIMEOUT;
  }
  
  /* Change the CRYP state */
  hcryp->State = HAL_CRYP_STATE_READY;
  
  /* Process Unlocked */
  __HAL_UNLOCK(hcryp);
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Initializes the CRYP peripheral in TDES CBC decryption mode
  *         then decrypted pCypherData. The cypher data are available in pPlainData
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @param  Size: Length of the plaintext buffer, must be a multiple of 8
  * @param  pPlainData: Pointer to the plaintext buffer
  * @param  Timeout: Specify Timeout value  
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYP_TDESCBC_Decrypt(CRYP_HandleTypeDef *hcryp, uint8_t *pCypherData, uint16_t Size, uint8_t *pPlainData, uint32_t Timeout)
{
  /* Process Locked */
  __HAL_LOCK(hcryp);
  
  /* Change the CRYP state */
  hcryp->State = HAL_CRYP_STATE_BUSY;
  
  /* Set CRYP peripheral in TDES CBC decryption mode */
  CRYP_SetTDESCBCMode(hcryp, CRYP_CR_ALGODIR);
  
  /* Enable CRYP */
  __HAL_CRYP_ENABLE();
  
  /* Write Cypher Data and Get Plain Data */
  if(CRYP_ProcessData2Words(hcryp, pCypherData, Size, pPlainData, Timeout) != HAL_OK)
  {
    return HAL_TIMEOUT;
  }
  
  /* Change the CRYP state */
  hcryp->State = HAL_CRYP_STATE_READY;
  
  /* Process Unlocked */
  __HAL_UNLOCK(hcryp);
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Initializes the CRYP peripheral in TDES ECB encryption mode using interrupt.
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  pPlainData: Pointer to the plaintext buffer
  * @param  Size: Length of the plaintext buffer, must be a multiple of 8
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYP_TDESECB_Encrypt_IT(CRYP_HandleTypeDef *hcryp, uint8_t *pPlainData, uint16_t Size, uint8_t *pCypherData)
{
  uint32_t inputaddr;
  uint32_t outputaddr;
  
  if(hcryp->State == HAL_CRYP_STATE_READY)
  {
    /* Process Locked */
    __HAL_LOCK(hcryp);
    
    hcryp->CrypInCount = Size;
    hcryp->pCrypInBuffPtr = pPlainData;
    hcryp->pCrypOutBuffPtr = pCypherData;
    hcryp->CrypOutCount = Size;
    
    /* Change the CRYP state */
    hcryp->State = HAL_CRYP_STATE_BUSY;
    
    /* Set CRYP peripheral in TDES ECB encryption mode */
    CRYP_SetTDESECBMode(hcryp, 0);
    
    /* Enable Interrupts */
    __HAL_CRYP_ENABLE_IT(CRYP_IT_INI | CRYP_IT_OUTI);
    
    /* Enable CRYP */
    __HAL_CRYP_ENABLE();
    
    /* Return function status */
    return HAL_OK;
  }
  else if(__HAL_CRYP_GET_IT(CRYP_IT_INI))
  {
    inputaddr = (uint32_t)hcryp->pCrypInBuffPtr;
    /* Write the Input block in the IN FIFO */
    CRYP->DR = *(uint32_t*)(inputaddr);
    inputaddr+=4;
    CRYP->DR = *(uint32_t*)(inputaddr);

    hcryp->pCrypInBuffPtr += 8;
    hcryp->CrypInCount -= 8;
    if(hcryp->CrypInCount == 0)
    {
      __HAL_CRYP_DISABLE_IT(CRYP_IT_INI);
      /* Call the Input data transfer complete callback */
      HAL_CRYP_InCpltCallback(hcryp);
    }
  }
  else if(__HAL_CRYP_GET_IT(CRYP_IT_OUTI))
  {
    outputaddr = (uint32_t)hcryp->pCrypOutBuffPtr;
    /* Read the Output block from the Output FIFO */
    *(uint32_t*)(outputaddr) = CRYP->DOUT;
    outputaddr+=4;
    *(uint32_t*)(outputaddr) = CRYP->DOUT;

    hcryp->pCrypOutBuffPtr += 8;
    hcryp->CrypOutCount -= 8;
    if(hcryp->CrypOutCount == 0)
    {
      /* Disable IT */
      __HAL_CRYP_DISABLE_IT(CRYP_IT_OUTI);
      /* Disable CRYP */
      __HAL_CRYP_DISABLE();
      /* Process Unlocked */
      __HAL_UNLOCK(hcryp);
      /* Change the CRYP state */
      hcryp->State = HAL_CRYP_STATE_READY;
      /* Call the Output data transfer complete callback */
      HAL_CRYP_OutCpltCallback(hcryp);
    }
  }
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Initializes the CRYP peripheral in TDES CBC encryption mode.
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  pPlainData: Pointer to the plaintext buffer
  * @param  Size: Length of the plaintext buffer, must be a multiple of 8
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYP_TDESCBC_Encrypt_IT(CRYP_HandleTypeDef *hcryp, uint8_t *pPlainData, uint16_t Size, uint8_t *pCypherData)
{
  uint32_t inputaddr;
  uint32_t outputaddr;
  
  if(hcryp->State == HAL_CRYP_STATE_READY)
  {
    /* Process Locked */
    __HAL_LOCK(hcryp);
    
    hcryp->CrypInCount = Size;
    hcryp->pCrypInBuffPtr = pPlainData;
    hcryp->pCrypOutBuffPtr = pCypherData;
    hcryp->CrypOutCount = Size;
    
    /* Change the CRYP state */
    hcryp->State = HAL_CRYP_STATE_BUSY;
    
    /* Set CRYP peripheral in TDES CBC encryption mode */
    CRYP_SetTDESCBCMode(hcryp, 0);
    
    /* Enable Interrupts */
    __HAL_CRYP_ENABLE_IT(CRYP_IT_INI | CRYP_IT_OUTI);
    
    /* Enable CRYP */
    __HAL_CRYP_ENABLE();
    
    /* Return function status */
    return HAL_OK;
  }
  else if(__HAL_CRYP_GET_IT(CRYP_IT_INI))
  {
    inputaddr = (uint32_t)hcryp->pCrypInBuffPtr;
    /* Write the Input block in the IN FIFO */
    CRYP->DR = *(uint32_t*)(inputaddr);
    inputaddr+=4;
    CRYP->DR = *(uint32_t*)(inputaddr);

    hcryp->pCrypInBuffPtr += 8;
    hcryp->CrypInCount -= 8;
    if(hcryp->CrypInCount == 0)
    {
      __HAL_CRYP_DISABLE_IT(CRYP_IT_INI);
      /* Call the Input data transfer complete callback */
      HAL_CRYP_InCpltCallback(hcryp);
    }
  }
  else if(__HAL_CRYP_GET_IT(CRYP_IT_OUTI))
  {
    outputaddr = (uint32_t)hcryp->pCrypOutBuffPtr;
    /* Read the Output block from the Output FIFO */
    *(uint32_t*)(outputaddr) = CRYP->DOUT;
    outputaddr+=4;
    *(uint32_t*)(outputaddr) = CRYP->DOUT;
        
    hcryp->pCrypOutBuffPtr += 8;
    hcryp->CrypOutCount -= 8;
    if(hcryp->CrypOutCount == 0)
    {
      __HAL_CRYP_DISABLE_IT(CRYP_IT_OUTI);
      /* Disable CRYP */
      __HAL_CRYP_DISABLE();
      /* Process Unlocked */
      __HAL_UNLOCK(hcryp);
      /* Change the CRYP state */
      hcryp->State = HAL_CRYP_STATE_READY;
      /* Call Input transfer complete callback */
      HAL_CRYP_OutCpltCallback(hcryp);
    }
  }
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Initializes the CRYP peripheral in TDES ECB decryption mode.
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  pPlainData: Pointer to the plaintext buffer
  * @param  Size: Length of the plaintext buffer, must be a multiple of 8
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYP_TDESECB_Decrypt_IT(CRYP_HandleTypeDef *hcryp, uint8_t *pCypherData, uint16_t Size, uint8_t *pPlainData)
{
  uint32_t inputaddr;
  uint32_t outputaddr;
  
  if(hcryp->State == HAL_CRYP_STATE_READY)
  {
    /* Process Locked */
    __HAL_LOCK(hcryp);
    
    hcryp->CrypInCount = Size;
    hcryp->pCrypInBuffPtr = pCypherData;
    hcryp->pCrypOutBuffPtr = pPlainData;
    hcryp->CrypOutCount = Size;
    
    /* Change the CRYP state */
    hcryp->State = HAL_CRYP_STATE_BUSY;
    
    /* Set CRYP peripheral in TDES ECB decryption mode */
    CRYP_SetTDESECBMode(hcryp, CRYP_CR_ALGODIR);
    
    /* Enable Interrupts */
    __HAL_CRYP_ENABLE_IT(CRYP_IT_INI | CRYP_IT_OUTI);
    
    /* Enable CRYP */
    __HAL_CRYP_ENABLE();
    
    /* Return function status */
    return HAL_OK;
  }
  else if(__HAL_CRYP_GET_IT(CRYP_IT_INI))
  {
    inputaddr = (uint32_t)hcryp->pCrypInBuffPtr;
    /* Write the Input block in the IN FIFO */
    CRYP->DR = *(uint32_t*)(inputaddr);
    inputaddr+=4;
    CRYP->DR = *(uint32_t*)(inputaddr);

    hcryp->pCrypInBuffPtr += 8;
    hcryp->CrypInCount -= 8;
    if(hcryp->CrypInCount == 0)
    {
      __HAL_CRYP_DISABLE_IT(CRYP_IT_INI);
      /* Call the Input data transfer complete callback */
      HAL_CRYP_InCpltCallback(hcryp);
    }
  }
  else if(__HAL_CRYP_GET_IT(CRYP_IT_OUTI))
  {
    outputaddr = (uint32_t)hcryp->pCrypOutBuffPtr;
    /* Read the Output block from the Output FIFO */
    *(uint32_t*)(outputaddr) = CRYP->DOUT;
    outputaddr+=4;
    *(uint32_t*)(outputaddr) = CRYP->DOUT;

    hcryp->pCrypOutBuffPtr += 8;
    hcryp->CrypOutCount -= 8;
    if(hcryp->CrypOutCount == 0)
    {
      __HAL_CRYP_DISABLE_IT(CRYP_IT_OUTI);
      /* Disable CRYP */
      __HAL_CRYP_DISABLE();
      /* Process Unlocked */
      __HAL_UNLOCK(hcryp);
      /* Change the CRYP state */
      hcryp->State = HAL_CRYP_STATE_READY;
      /* Call Input transfer complete callback */
      HAL_CRYP_OutCpltCallback(hcryp);
    }
  }
  
  /* Return function status */
  return HAL_OK;
} 

/**
  * @brief  Initializes the CRYP peripheral in TDES CBC decryption mode.
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @param  Size: Length of the plaintext buffer, must be a multiple of 8
  * @param  pPlainData: Pointer to the plaintext buffer
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYP_TDESCBC_Decrypt_IT(CRYP_HandleTypeDef *hcryp, uint8_t *pCypherData, uint16_t Size, uint8_t *pPlainData)
{
  uint32_t inputaddr;
  uint32_t outputaddr;
  
  if(hcryp->State == HAL_CRYP_STATE_READY)
  {
    /* Process Locked */
    __HAL_LOCK(hcryp);
    
    hcryp->CrypInCount = Size;
    hcryp->pCrypInBuffPtr = pCypherData;
    hcryp->pCrypOutBuffPtr = pPlainData;
    hcryp->CrypOutCount = Size;
    
    /* Change the CRYP state */
    hcryp->State = HAL_CRYP_STATE_BUSY;
    
    /* Set CRYP peripheral in TDES CBC decryption mode */
    CRYP_SetTDESCBCMode(hcryp, CRYP_CR_ALGODIR);
    
    /* Enable Interrupts */
    __HAL_CRYP_ENABLE_IT(CRYP_IT_INI | CRYP_IT_OUTI);
    
    /* Enable CRYP */
    __HAL_CRYP_ENABLE();
    
    /* Return function status */
    return HAL_OK;
  }
  else if(__HAL_CRYP_GET_IT(CRYP_IT_INI))
  {
    inputaddr = (uint32_t)hcryp->pCrypInBuffPtr;
    /* Write the Input block in the IN FIFO */
    CRYP->DR = *(uint32_t*)(inputaddr);
    inputaddr+=4;
    CRYP->DR = *(uint32_t*)(inputaddr);

    hcryp->pCrypInBuffPtr += 8;
    hcryp->CrypInCount -= 8;
    if(hcryp->CrypInCount == 0)
    {
      __HAL_CRYP_DISABLE_IT(CRYP_IT_INI);
      /* Call the Input data transfer complete callback */
      HAL_CRYP_InCpltCallback(hcryp);
    }
  }
  else if(__HAL_CRYP_GET_IT(CRYP_IT_OUTI))
  {
    outputaddr = (uint32_t)hcryp->pCrypOutBuffPtr;
    /* Read the Output block from the Output FIFO */
    *(uint32_t*)(outputaddr) = CRYP->DOUT;
    outputaddr+=4;
    *(uint32_t*)(outputaddr) = CRYP->DOUT;

    hcryp->pCrypOutBuffPtr += 8;
    hcryp->CrypOutCount -= 8;
    if(hcryp->CrypOutCount == 0)
    {
      __HAL_CRYP_DISABLE_IT(CRYP_IT_OUTI);
      /* Disable CRYP */
      __HAL_CRYP_DISABLE();
      /* Process Unlocked */
      __HAL_UNLOCK(hcryp);
      /* Change the CRYP state */
      hcryp->State = HAL_CRYP_STATE_READY;
      /* Call Input transfer complete callback */
      HAL_CRYP_OutCpltCallback(hcryp);
    }
  }
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Initializes the CRYP peripheral in TDES ECB encryption mode using DMA.
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  pPlainData: Pointer to the plaintext buffer
  * @param  Size: Length of the plaintext buffer, must be a multiple of 8
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYP_TDESECB_Encrypt_DMA(CRYP_HandleTypeDef *hcryp, uint8_t *pPlainData, uint16_t Size, uint8_t *pCypherData)
{
  uint32_t inputaddr;
  uint32_t outputaddr;
  
  if((hcryp->State == HAL_CRYP_STATE_READY) || (hcryp->Phase == HAL_CRYP_PHASE_PROCESS))
  {
    /* Process Locked */
    __HAL_LOCK(hcryp);
    
    inputaddr  = (uint32_t)pPlainData;
    outputaddr = (uint32_t)pCypherData;
    
    /* Change the CRYP state */
    hcryp->State = HAL_CRYP_STATE_BUSY;
    
    /* Set CRYP peripheral in TDES ECB encryption mode */
    CRYP_SetTDESECBMode(hcryp, 0);
    
    /* Set the input and output addresses and start DMA transfer */ 
    CRYP_SetDMAConfig(hcryp, inputaddr, Size, outputaddr);
    
    /* Process Unlocked */
    __HAL_UNLOCK(hcryp);
    
    /* Return function status */
    return HAL_OK;
  }
  else
  {
    return HAL_ERROR;   
  }
}

/**
  * @brief  Initializes the CRYP peripheral in TDES CBC encryption mode using DMA.
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  pPlainData: Pointer to the plaintext buffer
  * @param  Size: Length of the plaintext buffer, must be a multiple of 8
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYP_TDESCBC_Encrypt_DMA(CRYP_HandleTypeDef *hcryp, uint8_t *pPlainData, uint16_t Size, uint8_t *pCypherData)
{
  uint32_t inputaddr;
  uint32_t outputaddr;
  
  if((hcryp->State == HAL_CRYP_STATE_READY) || (hcryp->Phase == HAL_CRYP_PHASE_PROCESS))
  {
    /* Process Locked */
    __HAL_LOCK(hcryp);
    
    inputaddr  = (uint32_t)pPlainData;
    outputaddr = (uint32_t)pCypherData;
    
    /* Change the CRYP state */
    hcryp->State = HAL_CRYP_STATE_BUSY;
    
    /* Set CRYP peripheral in TDES CBC encryption mode */
    CRYP_SetTDESCBCMode(hcryp, 0);
    
    /* Set the input and output addresses and start DMA transfer */ 
    CRYP_SetDMAConfig(hcryp, inputaddr, Size, outputaddr);
    
    /* Process Unlocked */
    __HAL_UNLOCK(hcryp);
    
    /* Return function status */
    return HAL_OK;
  }
  else
  {
    return HAL_ERROR;   
  }
}

/**
  * @brief  Initializes the CRYP peripheral in TDES ECB decryption mode using DMA.
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  pPlainData: Pointer to the plaintext buffer
  * @param  Size: Length of the plaintext buffer, must be a multiple of 8
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYP_TDESECB_Decrypt_DMA(CRYP_HandleTypeDef *hcryp, uint8_t *pCypherData, uint16_t Size, uint8_t *pPlainData)
{
  uint32_t inputaddr;
  uint32_t outputaddr;
  
  if((hcryp->State == HAL_CRYP_STATE_READY) || (hcryp->Phase == HAL_CRYP_PHASE_PROCESS))
  {
    /* Process Locked */
    __HAL_LOCK(hcryp);
    
    inputaddr  = (uint32_t)pCypherData;
    outputaddr = (uint32_t)pPlainData;
    
    /* Change the CRYP state */
    hcryp->State = HAL_CRYP_STATE_BUSY;
    
    /* Set CRYP peripheral in TDES ECB decryption mode */
    CRYP_SetTDESECBMode(hcryp, CRYP_CR_ALGODIR);
    
    /* Set the input and output addresses and start DMA transfer */ 
    CRYP_SetDMAConfig(hcryp, inputaddr, Size, outputaddr);
    
    /* Process Unlocked */
    __HAL_UNLOCK(hcryp);
    
    /* Return function status */
    return HAL_OK;
  }
  else
  {
    return HAL_ERROR;   
  }
}

/**
  * @brief  Initializes the CRYP peripheral in TDES CBC decryption mode using DMA.
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @param  Size: Length of the plaintext buffer, must be a multiple of 8
  * @param  pPlainData: Pointer to the plaintext buffer
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYP_TDESCBC_Decrypt_DMA(CRYP_HandleTypeDef *hcryp, uint8_t *pCypherData, uint16_t Size, uint8_t *pPlainData)
{
  uint32_t inputaddr;
  uint32_t outputaddr;
  
  if((hcryp->State == HAL_CRYP_STATE_READY) || (hcryp->Phase == HAL_CRYP_PHASE_PROCESS))
  {
    /* Process Locked */
    __HAL_LOCK(hcryp);
    
    inputaddr  = (uint32_t)pCypherData;
    outputaddr = (uint32_t)pPlainData;
    
    /* Change the CRYP state */
    hcryp->State = HAL_CRYP_STATE_BUSY;
    
    /* Set CRYP peripheral in TDES CBC decryption mode */
    CRYP_SetTDESCBCMode(hcryp, CRYP_CR_ALGODIR);
    
    /* Set the input and output addresses and start DMA transfer */ 
    CRYP_SetDMAConfig(hcryp, inputaddr, Size, outputaddr);
    
    /* Process Unlocked */
    __HAL_UNLOCK(hcryp);
    
    /* Return function status */
    return HAL_OK;
  }
  else
  {
    return HAL_ERROR;   
  }
}

/**
  * @}
  */

/** @defgroup CRYP_Group5 DMA callback functions 
 *  @brief   DMA callback functions. 
 *
@verbatim   
  ==============================================================================
                      ##### DMA callback functions  #####
  ==============================================================================  
    [..]  This section provides DMA callback functions:
      (+) DMA Input data transfer complete
      (+) DMA Output data transfer complete
      (+) DMA error

@endverbatim
  * @{
  */

/**
  * @brief  Input FIFO transfer completed callbacks.
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @retval None
  */
__weak void HAL_CRYP_InCpltCallback(CRYP_HandleTypeDef *hcryp)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_CRYP_InCpltCallback could be implemented in the user file
   */ 
}

/**
  * @brief  Output FIFO transfer completed callbacks.
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @retval None
  */
__weak void HAL_CRYP_OutCpltCallback(CRYP_HandleTypeDef *hcryp)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_CRYP_OutCpltCallback could be implemented in the user file
   */
}

/**
  * @brief  CRYP error callbacks.
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @retval None
  */
 __weak void HAL_CRYP_ErrorCallback(CRYP_HandleTypeDef *hcryp)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_CRYP_ErrorCallback could be implemented in the user file
   */ 
}

/**
  * @}
  */

/** @defgroup CRYP_Group6 CRYP IRQ handler management  
 *  @brief   CRYP IRQ handler.
 *
@verbatim   
  ==============================================================================
                ##### CRYP IRQ handler management #####
  ==============================================================================  
[..]  This section provides CRYP IRQ handler function.

@endverbatim
  * @{
  */

/**
  * @brief  This function handles CRYP interrupt request.
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @retval None
  */
void HAL_CRYP_IRQHandler(CRYP_HandleTypeDef *hcryp)
{
  switch(CRYP->CR & CRYP_CR_ALGOMODE_DIRECTION)
  {
  case CRYP_CR_ALGOMODE_TDES_ECB_ENCRYPT:
    HAL_CRYP_TDESECB_Encrypt_IT(hcryp, NULL, 0, NULL);
    break;
    
  case CRYP_CR_ALGOMODE_TDES_ECB_DECRYPT:
    HAL_CRYP_TDESECB_Decrypt_IT(hcryp, NULL, 0, NULL);
    break;
    
  case CRYP_CR_ALGOMODE_TDES_CBC_ENCRYPT:
    HAL_CRYP_TDESCBC_Encrypt_IT(hcryp, NULL, 0, NULL);
    break;
    
  case CRYP_CR_ALGOMODE_TDES_CBC_DECRYPT:
    HAL_CRYP_TDESCBC_Decrypt_IT(hcryp, NULL, 0, NULL);
    break;
    
  case CRYP_CR_ALGOMODE_DES_ECB_ENCRYPT:
    HAL_CRYP_DESECB_Encrypt_IT(hcryp, NULL, 0, NULL);
    break;
    
  case CRYP_CR_ALGOMODE_DES_ECB_DECRYPT:
    HAL_CRYP_DESECB_Decrypt_IT(hcryp, NULL, 0, NULL);
    break;
    
  case CRYP_CR_ALGOMODE_DES_CBC_ENCRYPT:
    HAL_CRYP_DESCBC_Encrypt_IT(hcryp, NULL, 0, NULL);
    break;
    
  case CRYP_CR_ALGOMODE_DES_CBC_DECRYPT:
    HAL_CRYP_DESCBC_Decrypt_IT(hcryp, NULL, 0, NULL);
    break;
    
  case CRYP_CR_ALGOMODE_AES_ECB_ENCRYPT:
    HAL_CRYP_AESECB_Encrypt_IT(hcryp, NULL, 0, NULL);
    break;
    
  case CRYP_CR_ALGOMODE_AES_ECB_DECRYPT:
    HAL_CRYP_AESECB_Decrypt_IT(hcryp, NULL, 0, NULL);
    break;
    
  case CRYP_CR_ALGOMODE_AES_CBC_ENCRYPT:
    HAL_CRYP_AESCBC_Encrypt_IT(hcryp, NULL, 0, NULL);
    break;
    
  case CRYP_CR_ALGOMODE_AES_CBC_DECRYPT:
    HAL_CRYP_AESCBC_Decrypt_IT(hcryp, NULL, 0, NULL);
    break;
    
  case CRYP_CR_ALGOMODE_AES_CTR_ENCRYPT:
    HAL_CRYP_AESCTR_Encrypt_IT(hcryp, NULL, 0, NULL);       
    break;
    
  case CRYP_CR_ALGOMODE_AES_CTR_DECRYPT:
    HAL_CRYP_AESCTR_Decrypt_IT(hcryp, NULL, 0, NULL);        
    break;
    
  default:
    break;
  }
}

/**
  * @}
  */

/** @defgroup CRYP_Group7 Peripheral State functions 
 *  @brief   Peripheral State functions. 
 *
@verbatim   
  ==============================================================================
                      ##### Peripheral State functions #####
  ==============================================================================  
    [..]
    This subsection permits to get in run-time the status of the peripheral.

@endverbatim
  * @{
  */

/**
  * @brief  Returns the CRYP state.
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @retval HAL state
  */
HAL_CRYP_STATETypeDef HAL_CRYP_GetState(CRYP_HandleTypeDef *hcryp)
{
  return hcryp->State;
}

/**
  * @}
  */

/**
  * @brief  DMA CRYP Input Data process complete callback.
  * @param  hdma: DMA handle
  * @retval None
  */
static void CRYP_DMAInCplt(DMA_HandleTypeDef *hdma)  
{
  CRYP_HandleTypeDef* hcryp = (CRYP_HandleTypeDef*)((DMA_HandleTypeDef*)hdma)->Parent;
  
  /* Disable the DMA transfer for input FIFO request by resetting the DIEN bit
     in the DMACR register */
  CRYP->DMACR &= (uint32_t)(~CRYP_DMACR_DIEN);
  
  /* Call input data transfer complete callback */
  HAL_CRYP_InCpltCallback(hcryp);
}

/**
  * @brief  DMA CRYP Output Data process complete callback.
  * @param  hdma: DMA handle
  * @retval None
  */
static void CRYP_DMAOutCplt(DMA_HandleTypeDef *hdma)
{
  CRYP_HandleTypeDef* hcryp = (CRYP_HandleTypeDef*)((DMA_HandleTypeDef*)hdma)->Parent;
  
  /* Disable the DMA transfer for output FIFO request by resetting the DOEN bit
     in the DMACR register */
  CRYP->DMACR &= (uint32_t)(~CRYP_DMACR_DOEN);
  
  /* Disable CRYP */
  __HAL_CRYP_DISABLE();
  
  /* Change the CRYP state to ready */
  hcryp->State = HAL_CRYP_STATE_READY;
  
  /* Call output data transfer complete callback */
  HAL_CRYP_OutCpltCallback(hcryp);
}

/**
  * @brief  DMA CRYP communication error callback. 
  * @param  hdma: DMA handle
  * @retval None
  */
static void CRYP_DMAError(DMA_HandleTypeDef *hdma)
{
  CRYP_HandleTypeDef* hcryp = (CRYP_HandleTypeDef*)((DMA_HandleTypeDef*)hdma)->Parent;
  hcryp->State= HAL_CRYP_STATE_READY;
  HAL_CRYP_ErrorCallback(hcryp);
}

/**
  * @brief  Writes the Key in Key registers. 
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  Key: Pointer to Key buffer
  * @param  KeySize: Size of Key
  * @retval None
  */
static void CRYP_SetKey(CRYP_HandleTypeDef *hcryp, uint8_t *Key, uint32_t KeySize)
{
  uint32_t keyaddr = (uint32_t)Key;
  
  switch(KeySize)
  {
  case CRYP_KEYSIZE_256B:
    /* Key Initialisation */
    CRYP->K0LR = __REV(*(uint32_t*)(keyaddr));
    keyaddr+=4;
    CRYP->K0RR = __REV(*(uint32_t*)(keyaddr));
    keyaddr+=4;
    CRYP->K1LR = __REV(*(uint32_t*)(keyaddr));
    keyaddr+=4;
    CRYP->K1RR = __REV(*(uint32_t*)(keyaddr));
    keyaddr+=4;
    CRYP->K2LR = __REV(*(uint32_t*)(keyaddr));
    keyaddr+=4;
    CRYP->K2RR = __REV(*(uint32_t*)(keyaddr));
    keyaddr+=4;
    CRYP->K3LR = __REV(*(uint32_t*)(keyaddr));
    keyaddr+=4;
    CRYP->K3RR = __REV(*(uint32_t*)(keyaddr));
    break;
  case CRYP_KEYSIZE_192B:
    CRYP->K1LR = __REV(*(uint32_t*)(keyaddr));
    keyaddr+=4;
    CRYP->K1RR = __REV(*(uint32_t*)(keyaddr));
    keyaddr+=4;
    CRYP->K2LR = __REV(*(uint32_t*)(keyaddr));
    keyaddr+=4;
    CRYP->K2RR = __REV(*(uint32_t*)(keyaddr));
    keyaddr+=4;
    CRYP->K3LR = __REV(*(uint32_t*)(keyaddr));
    keyaddr+=4;
    CRYP->K3RR = __REV(*(uint32_t*)(keyaddr));
    break;
  case CRYP_KEYSIZE_128B:       
    CRYP->K2LR = __REV(*(uint32_t*)(keyaddr));
    keyaddr+=4;
    CRYP->K2RR = __REV(*(uint32_t*)(keyaddr));
    keyaddr+=4;
    CRYP->K3LR = __REV(*(uint32_t*)(keyaddr));
    keyaddr+=4;
    CRYP->K3RR = __REV(*(uint32_t*)(keyaddr));
    break;
  default:
    break;
  }
}

/**
  * @brief  Writes the InitVector/InitCounter in IV registers. 
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  InitVector: Pointer to InitVector/InitCounter buffer
  * @param  IVSize: Size of the InitVector/InitCounter
  * @retval None
  */
static void CRYP_SetInitVector(CRYP_HandleTypeDef *hcryp, uint8_t *InitVector, uint32_t IVSize)
{
  uint32_t ivaddr = (uint32_t)InitVector;
  
  switch(IVSize)
  {
  case CRYP_KEYSIZE_128B:
    CRYP->IV0LR = __REV(*(uint32_t*)(ivaddr));
    ivaddr+=4;
    CRYP->IV0RR = __REV(*(uint32_t*)(ivaddr));
    ivaddr+=4;
    CRYP->IV1LR = __REV(*(uint32_t*)(ivaddr));
    ivaddr+=4;
    CRYP->IV1RR = __REV(*(uint32_t*)(ivaddr));
    break;
    /* Whatever key size 192 or 256, Init vector is written in IV0LR and IV0RR */
  case CRYP_KEYSIZE_192B:
    CRYP->IV0LR = __REV(*(uint32_t*)(ivaddr));
    ivaddr+=4;
    CRYP->IV0RR = __REV(*(uint32_t*)(ivaddr));
    break;
  case CRYP_KEYSIZE_256B:
    CRYP->IV0LR = __REV(*(uint32_t*)(ivaddr));
    ivaddr+=4;
    CRYP->IV0RR = __REV(*(uint32_t*)(ivaddr));
    break;
  default:
    break;
  }
}

/**
  * @brief  Process Data: Writes Input data in polling mode and read the output data
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  Input: Pointer to the Input buffer
  * @param  Ilength: Length of the Input buffer, must be a multiple of 16.
  * @param  Output: Pointer to the returned buffer
  * @retval None
  */
static HAL_StatusTypeDef CRYP_ProcessData(CRYP_HandleTypeDef *hcryp, uint8_t* Input, uint16_t Ilength, uint8_t* Output, uint32_t Timeout)
{
  uint32_t tickstart = 0;
  
  uint32_t i = 0;
  uint32_t inputaddr  = (uint32_t)Input;
  uint32_t outputaddr = (uint32_t)Output;
  
  for(i=0; (i < Ilength); i+=16)
  {
    /* Write the Input block in the IN FIFO */
    CRYP->DR = *(uint32_t*)(inputaddr);
    inputaddr+=4;
    CRYP->DR = *(uint32_t*)(inputaddr);
    inputaddr+=4;
    CRYP->DR  = *(uint32_t*)(inputaddr);
    inputaddr+=4;
    CRYP->DR = *(uint32_t*)(inputaddr);
    inputaddr+=4;
    
    /* Get tick */
    tickstart = HAL_GetTick();

    while(HAL_IS_BIT_CLR(CRYP->SR, CRYP_FLAG_OFNE))
    {    
      /* Check for the Timeout */
      if(Timeout != HAL_MAX_DELAY)
      {
        if((Timeout == 0)||((HAL_GetTick() - tickstart ) > Timeout))
        {
          /* Change state */
          hcryp->State = HAL_CRYP_STATE_TIMEOUT;
          
          /* Process Unlocked */
          __HAL_UNLOCK(hcryp);
        
          return HAL_TIMEOUT;
        }
      }
    }
    /* Read the Output block from the Output FIFO */
    *(uint32_t*)(outputaddr) = CRYP->DOUT;
    outputaddr+=4;
    *(uint32_t*)(outputaddr) = CRYP->DOUT;
    outputaddr+=4;
    *(uint32_t*)(outputaddr) = CRYP->DOUT;
    outputaddr+=4;
    *(uint32_t*)(outputaddr) = CRYP->DOUT;
    outputaddr+=4;
  }
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Process Data: Write Input data in polling mode. 
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  Input: Pointer to the Input buffer
  * @param  Ilength: Length of the Input buffer, must be a multiple of 8
  * @param  Output: Pointer to the returned buffer
  * @param  Timeout: Specify Timeout value  
  * @retval None
  */
static HAL_StatusTypeDef CRYP_ProcessData2Words(CRYP_HandleTypeDef *hcryp, uint8_t* Input, uint16_t Ilength, uint8_t* Output, uint32_t Timeout)
{
  uint32_t tickstart = 0;   
  
  uint32_t i = 0;
  uint32_t inputaddr  = (uint32_t)Input;
  uint32_t outputaddr = (uint32_t)Output;
  
  for(i=0; (i < Ilength); i+=8)
  {
    /* Write the Input block in the IN FIFO */
    CRYP->DR  = *(uint32_t*)(inputaddr);
    inputaddr+=4;
    CRYP->DR = *(uint32_t*)(inputaddr);
    inputaddr+=4;
    
    /* Get tick */
    tickstart = HAL_GetTick();
    
    while(HAL_IS_BIT_CLR(CRYP->SR, CRYP_FLAG_OFNE))
    {
      /* Check for the Timeout */
      if(Timeout != HAL_MAX_DELAY)
      {
        if((Timeout == 0)||((HAL_GetTick() - tickstart ) > Timeout))
        {
          /* Change state */
          hcryp->State = HAL_CRYP_STATE_TIMEOUT;
          
          /* Process Unlocked */          
          __HAL_UNLOCK(hcryp);
          
          return HAL_TIMEOUT;
        }
      }
    }
    /* Read the Output block from the Output FIFO */
    *(uint32_t*)(outputaddr) = CRYP->DOUT;
    outputaddr+=4;
    *(uint32_t*)(outputaddr) = CRYP->DOUT;
    outputaddr+=4;
  }
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Set the DMA configuration and start the DMA transfer
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  inputaddr: address of the Input buffer
  * @param  Size: Size of the Input buffer, must be a multiple of 16.
  * @param  outputaddr: address of the Output buffer
  * @retval None
  */
static void CRYP_SetDMAConfig(CRYP_HandleTypeDef *hcryp, uint32_t inputaddr, uint16_t Size, uint32_t outputaddr)
{
  /* Set the CRYP DMA transfer complete callback */
  hcryp->hdmain->XferCpltCallback = CRYP_DMAInCplt;
  /* Set the DMA error callback */
  hcryp->hdmain->XferErrorCallback = CRYP_DMAError;
  
  /* Set the CRYP DMA transfer complete callback */
  hcryp->hdmaout->XferCpltCallback = CRYP_DMAOutCplt;
  /* Set the DMA error callback */
  hcryp->hdmaout->XferErrorCallback = CRYP_DMAError;
  
  /* Enable CRYP */
  __HAL_CRYP_ENABLE();
  
  /* Enable the DMA In DMA Stream */
  HAL_DMA_Start_IT(hcryp->hdmain, inputaddr, (uint32_t)&CRYP->DR, Size/4);
  
  /* Enable In DMA request */
  CRYP->DMACR = (CRYP_DMACR_DIEN);
  
  /* Enable the DMA Out DMA Stream */
  HAL_DMA_Start_IT(hcryp->hdmaout, (uint32_t)&CRYP->DOUT, outputaddr, Size/4);
  
  /* Enable Out DMA request */
  CRYP->DMACR |= CRYP_DMACR_DOEN;
 
}

/**
  * @brief  Sets the CRYP peripheral in DES ECB mode.
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  Direction: Encryption or decryption
  * @retval None
  */
static void CRYP_SetDESECBMode(CRYP_HandleTypeDef *hcryp, uint32_t Direction)
{
  /* Check if initialization phase has already been performed */
  if(hcryp->Phase == HAL_CRYP_PHASE_READY)
  {
    /* Set the CRYP peripheral in AES ECB mode */
    __HAL_CRYP_SET_MODE(CRYP_CR_ALGOMODE_DES_ECB | Direction);
    
    /* Set the key */
    CRYP->K1LR = __REV(*(uint32_t*)(hcryp->Init.pKey));
    CRYP->K1RR = __REV(*(uint32_t*)(hcryp->Init.pKey+4));
    
    /* Flush FIFO */
    __HAL_CRYP_FIFO_FLUSH();
    
    /* Set the phase */
    hcryp->Phase = HAL_CRYP_PHASE_PROCESS;
  }
}

/**
  * @brief  Sets the CRYP peripheral in DES CBC mode.
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  Direction: Encryption or decryption
  * @retval None
  */
static void CRYP_SetDESCBCMode(CRYP_HandleTypeDef *hcryp, uint32_t Direction)
{
  /* Check if initialization phase has already been performed */
  if(hcryp->Phase == HAL_CRYP_PHASE_READY)
  {
    /* Set the CRYP peripheral in AES ECB mode */
    __HAL_CRYP_SET_MODE(CRYP_CR_ALGOMODE_DES_CBC | Direction);
    
    /* Set the key */
    CRYP->K1LR = __REV(*(uint32_t*)(hcryp->Init.pKey));
    CRYP->K1RR = __REV(*(uint32_t*)(hcryp->Init.pKey+4));
    
    /* Set the Initialization Vector */
    CRYP_SetInitVector(hcryp, hcryp->Init.pInitVect, CRYP_KEYSIZE_256B);
    
    /* Flush FIFO */
    __HAL_CRYP_FIFO_FLUSH();
    
    /* Set the phase */
    hcryp->Phase = HAL_CRYP_PHASE_PROCESS;
  }
}

/**
  * @brief  Sets the CRYP peripheral in TDES ECB mode.
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  Direction: Encryption or decryption
  * @retval None
  */
static void CRYP_SetTDESECBMode(CRYP_HandleTypeDef *hcryp, uint32_t Direction)
{
  /* Check if initialization phase has already been performed */
  if(hcryp->Phase == HAL_CRYP_PHASE_READY)
  {
    /* Set the CRYP peripheral in AES ECB mode */
    __HAL_CRYP_SET_MODE(CRYP_CR_ALGOMODE_TDES_ECB | Direction);
    
    /* Set the key */
    CRYP_SetKey(hcryp, hcryp->Init.pKey, CRYP_KEYSIZE_192B);
    
    /* Flush FIFO */
    __HAL_CRYP_FIFO_FLUSH();
    
    /* Set the phase */
    hcryp->Phase = HAL_CRYP_PHASE_PROCESS;
  }
}

/**
  * @brief  Sets the CRYP peripheral in TDES CBC mode
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  Direction: Encryption or decryption
  * @retval None
  */
static void CRYP_SetTDESCBCMode(CRYP_HandleTypeDef *hcryp, uint32_t Direction)
{
  /* Check if initialization phase has already been performed */
  if(hcryp->Phase == HAL_CRYP_PHASE_READY)
  {
    /* Set the CRYP peripheral in AES CBC mode */
    __HAL_CRYP_SET_MODE(CRYP_CR_ALGOMODE_TDES_CBC | Direction);
    
    /* Set the key */
    CRYP_SetKey(hcryp, hcryp->Init.pKey, CRYP_KEYSIZE_192B);
    
    /* Set the Initialization Vector */
    CRYP_SetInitVector(hcryp, hcryp->Init.pInitVect, CRYP_KEYSIZE_256B);
    
    /* Flush FIFO */
    __HAL_CRYP_FIFO_FLUSH();
    
    /* Set the phase */
    hcryp->Phase = HAL_CRYP_PHASE_PROCESS;
  }
}

/**
  * @}
  */

#endif /* STM32F415xx || STM32F417xx || STM32F437xx || STM32F439xx */

#endif /* HAL_CRYP_MODULE_ENABLED */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
