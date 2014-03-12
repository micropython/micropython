/**
  ******************************************************************************
  * @file    stm32f4xx_hal_hash.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    18-February-2014
  * @brief   HASH HAL module driver.
  *          This file provides firmware functions to manage the following 
  *          functionalities of the HASH peripheral:
  *           + Initialization and de-initialization functions
  *           + HASH/HMAC Processing functions by algorithm using polling mode
  *           + HASH/HMAC functions by algorithm using interrupt mode
  *           + HASH/HMAC functions by algorithm using DMA mode
  *           + Peripheral State functions
  *         
  @verbatim
  ==============================================================================
                     ##### How to use this driver #####
  ==============================================================================
    [..]
    The HASH HAL driver can be used as follows:
    (#)Initialize the HASH low level resources by implementing the HAL_HASH_MspInit():
        (##) Enable the HASH interface clock using __HASH_CLK_ENABLE()
        (##) In case of using processing APIs based on interrupts (e.g. HAL_HMAC_SHA1_Start_IT())
            (+++) Configure the HASH interrupt priority using HAL_NVIC_SetPriority()
            (+++) Enable the HASH IRQ handler using HAL_NVIC_EnableIRQ()
            (+++) In HASH IRQ handler, call HAL_HASH_IRQHandler()
        (##) In case of using DMA to control data transfer (e.g. HAL_HMAC_SHA1_Start_DMA())
            (+++) Enable the DMAx interface clock using __DMAx_CLK_ENABLE()
            (+++) Configure and enable one DMA stream one for managing data transfer from
                memory to peripheral (input stream). Managing data transfer from
                peripheral to memory can be performed only using CPU
            (+++) Associate the initialized DMA handle to the HASH DMA handle
                using  __HAL_LINKDMA()
            (+++) Configure the priority and enable the NVIC for the transfer complete
                interrupt on the DMA Stream using HAL_NVIC_SetPriority() and HAL_NVIC_EnableIRQ()
    (#)Initialize the HASH HAL using HAL_HASH_Init(). This function configures mainly:
        (##) The data type: 1-bit, 8-bit, 16-bit and 32-bit.
        (##) For HMAC, the encryption key.
        (##) For HMAC, the key size used for encryption.
    (#)Three processing functions are available:
        (##) Polling mode: processing APIs are blocking functions
             i.e. they process the data and wait till the digest computation is finished
             e.g. HAL_HASH_SHA1_Start()
        (##) Interrupt mode: encryption and decryption APIs are not blocking functions
                i.e. they process the data under interrupt
                e.g. HAL_HASH_SHA1_Start_IT()
        (##) DMA mode: processing APIs are not blocking functions and the CPU is
             not used for data transfer i.e. the data transfer is ensured by DMA
                e.g. HAL_HASH_SHA1_Start_DMA()
    (#)When the processing function is called at first time after HAL_HASH_Init()
       the HASH peripheral is initialized and processes the buffer in input.
       After that, the digest computation is started.
       When processing multi-buffer use the accumulate function to write the
       data in the peripheral without starting the digest computation. In last 
       buffer use the start function to input the last buffer ans start the digest
       computation.
       (##) e.g. HAL_HASH_SHA1_Accumulate() : write 1st data buffer in the peripheral without starting the digest computation
       (##) write (n-1)th data buffer in the peripheral without starting the digest computation
       (##) HAL_HASH_SHA1_Start() : write (n)th data buffer in the peripheral and start the digest computation
    (#)In HMAC mode, there is no Accumulate API. Only Start API is available.
    (#)In case of using DMA, call the DMA start processing e.g. HAL_HASH_SHA1_Start_DMA().
       After that, call the finish function in order to get the digest value
       e.g. HAL_HASH_SHA1_Finish()
    (#)Call HAL_HASH_DeInit() to deinitialize the HASH peripheral.

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

/** @defgroup HASH 
  * @brief HASH HAL module driver.
  * @{
  */

#ifdef HAL_HASH_MODULE_ENABLED

#if defined(STM32F415xx) || defined(STM32F417xx) || defined(STM32F437xx) || defined(STM32F439xx)

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void HASH_DMAXferCplt(DMA_HandleTypeDef *hdma);
static void HASH_DMAError(DMA_HandleTypeDef *hdma);
static void HASH_GetDigest(uint8_t *pMsgDigest, uint8_t Size);
static void HASH_WriteData(uint8_t *pInBuffer, uint32_t Size);

/* Private functions ---------------------------------------------------------*/

/** @defgroup HASH_Private_Functions
  * @{
  */

/** @defgroup HASH_Group1 Initialization and de-initialization functions 
 *  @brief    Initialization and Configuration functions. 
 *
@verbatim    
 ===============================================================================
              ##### Initialization and de-initialization functions #####
 ===============================================================================
    [..]  This section provides functions allowing to:
      (+) Initialize the HASH according to the specified parameters 
          in the HASH_InitTypeDef and creates the associated handle.
      (+) DeInitialize the HASH peripheral.
      (+) Initialize the HASH MSP.
      (+) DeInitialize HASH MSP. 
 
@endverbatim
  * @{
  */

/**
  * @brief  Initializes the HASH according to the specified parameters in the
            HASH_HandleTypeDef and creates the associated handle.
  * @param  hhash: HASH handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HASH_Init(HASH_HandleTypeDef *hhash)
{
  /* Check the hash handle allocation */
  if(hhash == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_HASH_DATATYPE(hhash->Init.DataType));
   
  if(hhash->State == HAL_HASH_STATE_RESET)
  {
    /* Init the low level hardware */
    HAL_HASH_MspInit(hhash);
  }
  
  /* Change the HASH state */
  hhash->State = HAL_HASH_STATE_BUSY;
  
  /* Reset HashInCount, HashBuffSize and HashITCounter */
  hhash->HashInCount = 0;
  hhash->HashBuffSize = 0;
  hhash->HashITCounter = 0;
  
  /* Set the data type */
  HASH->CR |= (uint32_t) (hhash->Init.DataType);
  
  /* Change the HASH state */
  hhash->State = HAL_HASH_STATE_READY;
  
  /* Set the default HASH phase */
  hhash->Phase = HAL_HASH_PHASE_READY;
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  DeInitializes the HASH peripheral.
  * @note   This API must be called before starting a new processing. 
  * @param  hhash: HASH handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HASH_DeInit(HASH_HandleTypeDef *hhash)
{ 
  /* Check the HASH handle allocation */
  if(hhash == NULL)
  {
    return HAL_ERROR;
  }
  
  /* Change the HASH state */
  hhash->State = HAL_HASH_STATE_BUSY;
  
  /* Set the default HASH phase */
  hhash->Phase = HAL_HASH_PHASE_READY;
  
  /* Reset HashInCount, HashBuffSize and HashITCounter */
  hhash->HashInCount = 0;
  hhash->HashBuffSize = 0;
  hhash->HashITCounter = 0;
  
  /* DeInit the low level hardware */
  HAL_HASH_MspDeInit(hhash);
  
  /* Change the HASH state */
  hhash->State = HAL_HASH_STATE_RESET;  

  /* Release Lock */
  __HAL_UNLOCK(hhash);

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Initializes the HASH MSP.
  * @param  hhash: HASH handle
  * @retval None
  */
__weak void HAL_HASH_MspInit(HASH_HandleTypeDef *hhash)
{
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_HASH_MspInit could be implemented in the user file
   */
}

/**
  * @brief  DeInitializes HASH MSP.
  * @param  hhash: HASH handle
  * @retval None
  */
__weak void HAL_HASH_MspDeInit(HASH_HandleTypeDef *hhash)
{
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_HASH_MspDeInit could be implemented in the user file
   */
}

/**
  * @brief  Input data transfer complete callback.
  * @param  hhash: HASH handle
  * @retval None
  */
 __weak void HAL_HASH_InCpltCallback(HASH_HandleTypeDef *hhash)
{
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_HASH_InCpltCallback could be implemented in the user file
   */ 
}

/**
  * @brief  Data transfer Error callback.
  * @param  hhash: HASH handle
  * @retval None
  */
 __weak void HAL_HASH_ErrorCallback(HASH_HandleTypeDef *hhash)
{
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_HASH_ErrorCallback could be implemented in the user file
   */ 
}

/**
  * @brief  Digest computation complete callback. It is used only with interrupt.
  * @note   This callback is not relevant with DMA.
  * @param  hhash: HASH handle
  * @retval None
  */
 __weak void HAL_HASH_DgstCpltCallback(HASH_HandleTypeDef *hhash)
{
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_HASH_DgstCpltCallback could be implemented in the user file
   */ 
}

/**
  * @}
  */

/** @defgroup HASH_Group2 HASH processing functions using polling mode 
 *  @brief   processing functions using polling mode 
 *
@verbatim   
 ===============================================================================
              ##### HASH processing using polling mode functions#####
 ===============================================================================  
    [..]  This section provides functions allowing to calculate in polling mode
          the hash value using one of the following algorithms:
      (+) MD5
      (+) SHA1

@endverbatim
  * @{
  */

/**
  * @brief  Initializes the HASH peripheral in MD5 mode then processes pInBuffer.
            The digest is available in pOutBuffer.
  * @param  hhash: HASH handle
  * @param  pInBuffer: Pointer to the input buffer (buffer to be hashed).
  * @param  pOutBuffer: Pointer to the Output buffer (hashed buffer).  
  * @param  Size: Length of the input buffer in bytes.
  *          If the Size is multiple of 64 bytes, appending the input buffer is possible.
  *          If the Size is not multiple of 64 bytes, the padding is managed by hardware
  *          and appending the input buffer is no more possible.
  * @param  pOutBuffer: Pointer to the computed digest. Its size must be 16 bytes.
  * @param  Timeout: Timeout value
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HASH_MD5_Start(HASH_HandleTypeDef *hhash, uint8_t *pInBuffer, uint32_t Size, uint8_t* pOutBuffer, uint32_t Timeout)
{
  uint32_t timeout = 0;   
  
  /* Process Locked */
  __HAL_LOCK(hhash);
  
  /* Change the HASH state */
  hhash->State = HAL_HASH_STATE_BUSY;
  
  /* Check if initialization phase has already been performed */
  if(hhash->Phase == HAL_HASH_PHASE_READY)
  {
    /* Select the MD5 mode and reset the HASH processor core, so that the HASH will be ready to compute 
       the message digest of a new message */
    HASH->CR |= HASH_AlgoSelection_MD5 | HASH_CR_INIT;
  }
  
  /* Set the phase */
  hhash->Phase = HAL_HASH_PHASE_PROCESS;
  
  /* Configure the number of valid bits in last word of the message */
  __HAL_HASH_SET_NBVALIDBITS(Size);
  
  /* Write input buffer in data register */
  HASH_WriteData(pInBuffer, Size);
  
  /* Start the digest calculation */
  __HAL_HASH_START_DIGEST();
  
  /* Get timeout */
  timeout = HAL_GetTick() + Timeout;
  
  while(HAL_IS_BIT_SET(HASH->SR, HASH_FLAG_BUSY))
  {
    /* Check for the Timeout */
    if(Timeout != HAL_MAX_DELAY)
    {
      if(HAL_GetTick() >= timeout)
      {
        /* Change state */
        hhash->State = HAL_HASH_STATE_TIMEOUT;
        
        /* Process Unlocked */          
        __HAL_UNLOCK(hhash);
        
        return HAL_TIMEOUT;
      }
    }
  }
  
  /* Read the message digest */
  HASH_GetDigest(pOutBuffer, 16);
  
  /* Change the HASH state */
  hhash->State = HAL_HASH_STATE_READY;
   
  /* Process Unlocked */
  __HAL_UNLOCK(hhash);
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Initializes the HASH peripheral in MD5 mode then writes the pInBuffer.
  * @param  hhash: HASH handle
  * @param  pInBuffer: Pointer to the input buffer (buffer to be hashed).
  * @param  Size: Length of the input buffer in bytes.
  *          If the Size is multiple of 64 bytes, appending the input buffer is possible.
  *          If the Size is not multiple of 64 bytes, the padding is managed by hardware
  *          and appending the input buffer is no more possible.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HASH_MD5_Accumulate(HASH_HandleTypeDef *hhash, uint8_t *pInBuffer, uint32_t Size)
{  
  /* Process Locked */
  __HAL_LOCK(hhash);
  
  /* Change the HASH state */
  hhash->State = HAL_HASH_STATE_BUSY;
  
  /* Check if initialization phase has already been performed */
  if(hhash->Phase == HAL_HASH_PHASE_READY)
  {
    /* Select the MD5 mode and reset the HASH processor core, so that the HASH will be ready to compute 
       the message digest of a new message */
    HASH->CR |= HASH_AlgoSelection_MD5 | HASH_CR_INIT;
  }
  
  /* Set the phase */
  hhash->Phase = HAL_HASH_PHASE_PROCESS;
  
  /* Configure the number of valid bits in last word of the message */
  __HAL_HASH_SET_NBVALIDBITS(Size);
  
  /* Write input buffer in data register */
  HASH_WriteData(pInBuffer, Size);
  
  /* Change the HASH state */
  hhash->State = HAL_HASH_STATE_READY;
  
  /* Process Unlocked */
  __HAL_UNLOCK(hhash);
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Initializes the HASH peripheral in SHA1 mode then processes pInBuffer.
            The digest is available in pOutBuffer.
  * @param  hhash: HASH handle
  * @param  pInBuffer: Pointer to the input buffer (buffer to be hashed).
  * @param  pOutBuffer: Pointer to the Output buffer (hashed buffer).   
  * @param  Size: Length of the input buffer in bytes.
  *          If the Size is not multiple of 64 bytes, the padding is managed by hardware.
  * @param  pOutBuffer: Pointer to the computed digest. Its size must be 20 bytes.
  * @param  Timeout: Timeout value  
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HASH_SHA1_Start(HASH_HandleTypeDef *hhash, uint8_t *pInBuffer, uint32_t Size, uint8_t* pOutBuffer, uint32_t Timeout)
{
  uint32_t timeout = 0;   

  /* Process Locked */
  __HAL_LOCK(hhash);
  
  /* Change the HASH state */
  hhash->State = HAL_HASH_STATE_BUSY;
  
  /* Check if initialization phase has already been performed */
  if(hhash->Phase == HAL_HASH_PHASE_READY)
  {
    /* Select the SHA1 mode and reset the HASH processor core, so that the HASH will be ready to compute 
       the message digest of a new message */
    HASH->CR |= HASH_AlgoSelection_SHA1 | HASH_CR_INIT;
  }
  
  /* Set the phase */
  hhash->Phase = HAL_HASH_PHASE_PROCESS;
  
  /* Configure the number of valid bits in last word of the message */
  __HAL_HASH_SET_NBVALIDBITS(Size);
  
  /* Write input buffer in data register */
  HASH_WriteData(pInBuffer, Size);
  
  /* Start the digest calculation */
  __HAL_HASH_START_DIGEST();
  
  /* Get timeout */
    timeout = HAL_GetTick() + Timeout;

  while(HAL_IS_BIT_SET(HASH->SR, HASH_FLAG_BUSY))
    {
      /* Check for the Timeout */
      if(Timeout != HAL_MAX_DELAY)
      {
        if(HAL_GetTick() >= timeout)
        {
          /* Change state */
          hhash->State = HAL_HASH_STATE_TIMEOUT;
          
          /* Process Unlocked */          
          __HAL_UNLOCK(hhash);
          
          return HAL_TIMEOUT;
        }
      }
    }
  
  /* Read the message digest */
  HASH_GetDigest(pOutBuffer, 20);
  
  /* Change the HASH state */
  hhash->State = HAL_HASH_STATE_READY;
  
  /* Process Unlocked */
  __HAL_UNLOCK(hhash);
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Initializes the HASH peripheral in SHA1 mode then processes pInBuffer.
            The digest is available in pOutBuffer.
  * @param  hhash: HASH handle
  * @param  pInBuffer: Pointer to the input buffer (buffer to be hashed).
  * @param  Size: Length of the input buffer in bytes.
  *          If the Size is not multiple of 64 bytes, the padding is managed by hardware.
  * @param  pOutBuffer: Pointer to the computed digest. Its size must be 20 bytes.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HASH_SHA1_Accumulate(HASH_HandleTypeDef *hhash, uint8_t *pInBuffer, uint32_t Size)
{
  /* Process Locked */
  __HAL_LOCK(hhash);
  
  /* Change the HASH state */
  hhash->State = HAL_HASH_STATE_BUSY;
  
  /* Check if initialization phase has already been performed */
  if(hhash->Phase == HAL_HASH_PHASE_READY)
  {
    /* Select the SHA1 mode and reset the HASH processor core, so that the HASH will be ready to compute 
       the message digest of a new message */
    HASH->CR |= HASH_AlgoSelection_SHA1 | HASH_CR_INIT;
  }
  
  /* Set the phase */
  hhash->Phase = HAL_HASH_PHASE_PROCESS;
  
  /* Configure the number of valid bits in last word of the message */
  __HAL_HASH_SET_NBVALIDBITS(Size);
  
  /* Write input buffer in data register */
  HASH_WriteData(pInBuffer, Size);
  
  /* Change the HASH state */
  hhash->State = HAL_HASH_STATE_READY;
  
  /* Process Unlocked */
  __HAL_UNLOCK(hhash);
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @}
  */

/** @defgroup HASH_Group3 HASH processing functions using interrupt mode
 *  @brief   processing functions using interrupt mode. 
 *
@verbatim   
 ===============================================================================
              ##### HASH processing using interrupt mode functions #####
 ===============================================================================  
    [..]  This section provides functions allowing to calculate in interrupt mode
          the hash value using one of the following algorithms:
      (+) MD5
      (+) SHA1

@endverbatim
  * @{
  */

/**
  * @brief  Initializes the HASH peripheral in MD5 mode then processes pInBuffer.
  *         The digest is available in pOutBuffer.
  * @param  hhash: HASH handle
  * @param  pInBuffer: Pointer to the input buffer (buffer to be hashed).
  * @param  pOutBuffer: Pointer to the Output buffer (hashed buffer).   
  * @param  Size: Length of the input buffer in bytes.
  *          If the Size is not multiple of 64 bytes, the padding is managed by hardware.
  * @param  pOutBuffer: Pointer to the computed digest. Its size must be 16 bytes.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HASH_MD5_Start_IT(HASH_HandleTypeDef *hhash, uint8_t *pInBuffer, uint32_t Size, uint8_t* pOutBuffer)
{
  uint32_t inputaddr;
  uint32_t outputaddr;
  uint32_t buffercounter;
  uint32_t inputcounter;
  
  /* Process Locked */
  __HAL_LOCK(hhash);
  
  if(hhash->HashITCounter == 0)
  {
    hhash->HashITCounter = 1;
  }
  else
  {
    hhash->HashITCounter = 0;
  }
  if(hhash->State == HAL_HASH_STATE_READY)
  {
    /* Change the HASH state */
    hhash->State = HAL_HASH_STATE_BUSY;
    
    hhash->HashInCount = Size;
    hhash->pHashInBuffPtr = pInBuffer;
    hhash->pHashOutBuffPtr = pOutBuffer;
    
    /* Check if initialization phase has already been performed */
    if(hhash->Phase == HAL_HASH_PHASE_READY)
    {
      /* Select the SHA1 mode */
      HASH->CR |= HASH_AlgoSelection_MD5;
      /* Reset the HASH processor core, so that the HASH will be ready to compute 
         the message digest of a new message */
      HASH->CR |= HASH_CR_INIT;
    }
    
    /* Set the phase */
    hhash->Phase = HAL_HASH_PHASE_PROCESS;
    
    /* Process Unlocked */
    __HAL_UNLOCK(hhash);
    
    /* Enable Interrupts */
    HASH->IMR = (HASH_IT_DINI | HASH_IT_DCI);
    
    /* Return function status */
    return HAL_OK;
  }
  if(__HAL_HASH_GET_FLAG(HASH_FLAG_DCIS))
  {
    outputaddr = (uint32_t)hhash->pHashOutBuffPtr;
    /* Read the Output block from the Output FIFO */
    *(uint32_t*)(outputaddr) = __REV(HASH->HR[0]);
    outputaddr+=4;
    *(uint32_t*)(outputaddr) = __REV(HASH->HR[1]);
    outputaddr+=4;
    *(uint32_t*)(outputaddr) = __REV(HASH->HR[2]);
    outputaddr+=4;
    *(uint32_t*)(outputaddr) = __REV(HASH->HR[3]);
    
    if(hhash->HashInCount == 0)
    {
      /* Disable Interrupts */
      HASH->IMR = 0;
      /* Change the HASH state */
      hhash->State = HAL_HASH_STATE_READY;
      /* Call digest computation complete callback */
      HAL_HASH_DgstCpltCallback(hhash);
    }
  }
  if(__HAL_HASH_GET_FLAG(HASH_FLAG_DINIS))
  {
    if(hhash->HashInCount > 64)
    {
      inputaddr = (uint32_t)hhash->pHashInBuffPtr;
      /* Write the Input block in the Data IN register */
      for(buffercounter = 0; buffercounter < 64; buffercounter+=4)
      {
        HASH->DIN = *(uint32_t*)inputaddr;
      }
      if(hhash->HashITCounter == 0)
      {
        HASH->DIN = *(uint32_t*)inputaddr;

        if(hhash->HashInCount >= 68)
        {
          /* Decrement buffer counter */
          hhash->HashInCount -= 68;
          hhash->pHashInBuffPtr+= 68;
        }
        else
        {
          hhash->HashInCount -= 64;
        }
      }
      else
      {
        /* Decrement buffer counter */
        hhash->HashInCount -= 64;
        hhash->pHashInBuffPtr+= 64;
      }
    }
    else
    {
      /* Get the buffer address */
      inputaddr = (uint32_t)hhash->pHashInBuffPtr;
      /* Get the buffer counter */
      inputcounter = hhash->HashInCount;
      /* Disable Interrupts */
      HASH->IMR &= ~(HASH_IT_DINI);
      /* Configure the number of valid bits in last word of the message */
      __HAL_HASH_SET_NBVALIDBITS(inputcounter);
      
      if((inputcounter > 4) && (inputcounter%4))
      {
        inputcounter = (inputcounter+4-inputcounter%4);
      }
      
      /* Write the Input block in the Data IN register */
      for(buffercounter = 0; buffercounter < inputcounter/4; buffercounter++)
      {
        HASH->DIN = *(uint32_t*)inputaddr;
        inputaddr+=4;
      }
      /* Start the digest calculation */
      __HAL_HASH_START_DIGEST();
      /* Reset buffer counter */
      hhash->HashInCount = 0;
    }
    /* Call Input data transfer complete callback */
    HAL_HASH_InCpltCallback(hhash);
  }
  
  /* Process Unlocked */
  __HAL_UNLOCK(hhash);
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Initializes the HASH peripheral in SHA1 mode then processes pInBuffer.
  *         The digest is available in pOutBuffer.
  * @param  hhash: HASH handle
  * @param  pInBuffer: Pointer to the input buffer (buffer to be hashed).
  * @param  pOutBuffer: Pointer to the Output buffer (hashed buffer).   
  * @param  Size: Length of the input buffer in bytes.
  *          If the Size is not multiple of 64 bytes, the padding is managed by hardware.
  * @param  pOutBuffer: Pointer to the computed digest. Its size must be 20 bytes.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HASH_SHA1_Start_IT(HASH_HandleTypeDef *hhash, uint8_t *pInBuffer, uint32_t Size, uint8_t* pOutBuffer)
{
  uint32_t inputaddr;
  uint32_t outputaddr;
  uint32_t buffercounter;
  uint32_t inputcounter;
  
  /* Process Locked */
  __HAL_LOCK(hhash);
  
  if(hhash->HashITCounter == 0)
  {
    hhash->HashITCounter = 1;
  }
  else
  {
    hhash->HashITCounter = 0;
  }
  if(hhash->State == HAL_HASH_STATE_READY)
  {
    /* Change the HASH state */
    hhash->State = HAL_HASH_STATE_BUSY;
    
    hhash->HashInCount = Size;
    hhash->pHashInBuffPtr = pInBuffer;
    hhash->pHashOutBuffPtr = pOutBuffer;
    
    /* Check if initialization phase has already been performed */
    if(hhash->Phase == HAL_HASH_PHASE_READY)
    {
      /* Select the SHA1 mode */
      HASH->CR |= HASH_AlgoSelection_SHA1;
      /* Reset the HASH processor core, so that the HASH will be ready to compute 
         the message digest of a new message */
      HASH->CR |= HASH_CR_INIT;
    }
    
    /* Set the phase */
    hhash->Phase = HAL_HASH_PHASE_PROCESS;
    
    /* Process Unlocked */
    __HAL_UNLOCK(hhash);
    
    /* Enable Interrupts */
    HASH->IMR = (HASH_IT_DINI | HASH_IT_DCI);
    
    /* Return function status */
    return HAL_OK;
  }
  if(__HAL_HASH_GET_FLAG(HASH_FLAG_DCIS))
  {
    outputaddr = (uint32_t)hhash->pHashOutBuffPtr;
    /* Read the Output block from the Output FIFO */
    *(uint32_t*)(outputaddr) = __REV(HASH->HR[0]);
    outputaddr+=4;
    *(uint32_t*)(outputaddr) = __REV(HASH->HR[1]);
    outputaddr+=4;
    *(uint32_t*)(outputaddr) = __REV(HASH->HR[2]);
    outputaddr+=4;
    *(uint32_t*)(outputaddr) = __REV(HASH->HR[3]);
    outputaddr+=4;
    *(uint32_t*)(outputaddr) = __REV(HASH->HR[4]);
    if(hhash->HashInCount == 0)
    {
      /* Disable Interrupts */
      HASH->IMR = 0;
      /* Change the HASH state */
      hhash->State = HAL_HASH_STATE_READY;
      /* Call digest computation complete callback */
      HAL_HASH_DgstCpltCallback(hhash);
    }
  }
  if(__HAL_HASH_GET_FLAG(HASH_FLAG_DINIS))
  {
    if(hhash->HashInCount > 64)
    {
      inputaddr = (uint32_t)hhash->pHashInBuffPtr;
      /* Write the Input block in the Data IN register */
      for(buffercounter = 0; buffercounter < 64; buffercounter+=4)
      {
        HASH->DIN = *(uint32_t*)inputaddr;
        inputaddr+=4;
      }
      if(hhash->HashITCounter == 0)
      {
        HASH->DIN = *(uint32_t*)inputaddr;
      
        if(hhash->HashInCount >= 68)
        {
          /* Decrement buffer counter */
          hhash->HashInCount -= 68;
          hhash->pHashInBuffPtr+= 68;
        }
        else
        {
          hhash->HashInCount -= 64;
        }
      }
      else
      {
        /* Decrement buffer counter */
        hhash->HashInCount -= 64;
        hhash->pHashInBuffPtr+= 64;
      }
    }
    else
    {
      /* Get the buffer address */
      inputaddr = (uint32_t)hhash->pHashInBuffPtr;
      /* Get the buffer counter */
      inputcounter = hhash->HashInCount;
      /* Disable Interrupts */
      HASH->IMR &= ~(HASH_IT_DINI);
      /* Configure the number of valid bits in last word of the message */
      __HAL_HASH_SET_NBVALIDBITS(inputcounter);
      
      if((inputcounter > 4) && (inputcounter%4))
      {
        inputcounter = (inputcounter+4-inputcounter%4);
      }
      
      /* Write the Input block in the Data IN register */
      for(buffercounter = 0; buffercounter < inputcounter/4; buffercounter++)
      {
        HASH->DIN = *(uint32_t*)inputaddr;
        inputaddr+=4;
      }
      /* Start the digest calculation */
      __HAL_HASH_START_DIGEST();
      /* Reset buffer counter */
      hhash->HashInCount = 0;
    }
    /* Call Input data transfer complete callback */
    HAL_HASH_InCpltCallback(hhash);
  }
  
  /* Process Unlocked */
  __HAL_UNLOCK(hhash);
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief This function handles HASH interrupt request.
  * @param hhash: hash handle
  * @retval None
  */
void HAL_HASH_IRQHandler(HASH_HandleTypeDef *hhash)
{
  switch(HASH->CR & HASH_CR_ALGO)
  {
    case HASH_AlgoSelection_MD5:
       HAL_HASH_MD5_Start_IT(hhash, NULL, 0, NULL);
    break;
    
    case HASH_AlgoSelection_SHA1:
      HAL_HASH_SHA1_Start_IT(hhash, NULL, 0, NULL);
    break;
    
    default:
    break;
  }
}

/**
  * @}
  */

/** @defgroup HASH_Group4 HASH processing functions using DMA mode
 *  @brief   processing functions using DMA mode. 
 *
@verbatim   
 ===============================================================================
              ##### HASH processing using DMA mode functions #####
 ===============================================================================  
    [..]  This section provides functions allowing to calculate in DMA mode
          the hash value using one of the following algorithms:
      (+) MD5
      (+) SHA1

@endverbatim
  * @{
  */

/**
  * @brief  Initializes the HASH peripheral in MD5 mode then enables DMA to
            control data transfer. Use HAL_HASH_MD5_Finish() to get the digest.
  * @param  hhash: HASH handle
  * @param  pInBuffer: Pointer to the input buffer (buffer to be hashed).
  * @param  Size: Length of the input buffer in bytes.
  *          If the Size is not multiple of 64 bytes, the padding is managed by hardware.
  * @param  pOutBuffer: Pointer to the computed digest. Its size must be 16 bytes.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HASH_MD5_Start_DMA(HASH_HandleTypeDef *hhash, uint8_t *pInBuffer, uint32_t Size)
{
  uint32_t inputaddr  = (uint32_t)pInBuffer;
  
   /* Process Locked */
  __HAL_LOCK(hhash);
  
  /* Change the HASH state */
  hhash->State = HAL_HASH_STATE_BUSY;
  
  /* Check if initialization phase has already been performed */
  if(hhash->Phase == HAL_HASH_PHASE_READY)
  {
    /* Select the MD5 mode and reset the HASH processor core, so that the HASH will be ready to compute 
       the message digest of a new message */
    HASH->CR |= HASH_AlgoSelection_MD5 | HASH_CR_INIT;
  }
   
  /* Configure the number of valid bits in last word of the message */
  __HAL_HASH_SET_NBVALIDBITS(Size);
  
  /* Set the phase */
  hhash->Phase = HAL_HASH_PHASE_PROCESS;
    
  /* Set the HASH DMA transfer complete callback */
  hhash->hdmain->XferCpltCallback = HASH_DMAXferCplt;
  /* Set the DMA error callback */
  hhash->hdmain->XferErrorCallback = HASH_DMAError;
  
  /* Enable the DMA In DMA Stream */
  HAL_DMA_Start_IT(hhash->hdmain, inputaddr, (uint32_t)&HASH->DIN, (Size%4 ? (Size+3)/4:Size/4));
  
  /* Enable DMA requests */
  HASH->CR |= (HASH_CR_DMAE);
  
   /* Process Unlocked */
  __HAL_UNLOCK(hhash);
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Returns the computed digest in MD5 mode
  * @param  hhash: HASH handle
  * @param  pOutBuffer: Pointer to the computed digest. Its size must be 16 bytes.
  * @param  Timeout: Timeout value  
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HASH_MD5_Finish(HASH_HandleTypeDef *hhash, uint8_t* pOutBuffer, uint32_t Timeout)
{
  uint32_t timeout = 0;   
  
   /* Process Locked */
  __HAL_LOCK(hhash);
  
  /* Change HASH peripheral state */
  hhash->State = HAL_HASH_STATE_BUSY;
  
  /* Get timeout */
  timeout = HAL_GetTick() + Timeout;

  while(HAL_IS_BIT_CLR(HASH->SR, HASH_FLAG_DCIS))
  {
    /* Check for the Timeout */
    if(Timeout != HAL_MAX_DELAY)
    {
      if(HAL_GetTick() >= timeout)
      {
        /* Change state */
        hhash->State = HAL_HASH_STATE_TIMEOUT;
        
        /* Process Unlocked */          
        __HAL_UNLOCK(hhash);
        
        return HAL_TIMEOUT;
      }
    }
  }
  
  /* Read the message digest */
  HASH_GetDigest(pOutBuffer, 16);
      
  /* Change HASH peripheral state */
  hhash->State = HAL_HASH_STATE_READY;
  
   /* Process Unlocked */
  __HAL_UNLOCK(hhash);
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Initializes the HASH peripheral in SHA1 mode then enables DMA to
            control data transfer. Use HAL_HASH_SHA1_Finish() to get the digest.
  * @param  hhash: HASH handle
  * @param  pInBuffer: Pointer to the input buffer (buffer to be hashed).
  * @param  Size: Length of the input buffer in bytes.
  *          If the Size is not multiple of 64 bytes, the padding is managed by hardware.
  * @param  pOutBuffer: Pointer to the computed digest. Its size must be 20 bytes.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HASH_SHA1_Start_DMA(HASH_HandleTypeDef *hhash, uint8_t *pInBuffer, uint32_t Size)
{
  uint32_t inputaddr  = (uint32_t)pInBuffer;
  
   /* Process Locked */
  __HAL_LOCK(hhash);
  
  /* Change the HASH state */
  hhash->State = HAL_HASH_STATE_BUSY;
  
  /* Check if initialization phase has already been performed */
  if(hhash->Phase == HAL_HASH_PHASE_READY)
  {
    /* Select the SHA1 mode and reset the HASH processor core, so that the HASH will be ready to compute 
       the message digest of a new message */
    HASH->CR |= HASH_AlgoSelection_SHA1;
    HASH->CR |= HASH_CR_INIT;
  }
  
  /* Configure the number of valid bits in last word of the message */
  __HAL_HASH_SET_NBVALIDBITS(Size);
  
  /* Set the phase */
  hhash->Phase = HAL_HASH_PHASE_PROCESS;
  
  /* Set the HASH DMA transfer complete callback */
  hhash->hdmain->XferCpltCallback = HASH_DMAXferCplt;
  /* Set the DMA error callback */
  hhash->hdmain->XferErrorCallback = HASH_DMAError;
  
  /* Enable the DMA In DMA Stream */
  HAL_DMA_Start_IT(hhash->hdmain, inputaddr, (uint32_t)&HASH->DIN, (Size%4 ? (Size+3)/4:Size/4));
  
  /* Enable DMA requests */
  HASH->CR |= (HASH_CR_DMAE);
  
   /* Process Unlocked */
  __HAL_UNLOCK(hhash);
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Returns the computed digest in SHA1 mode.
  * @param  hhash: HASH handle
  * @param  pOutBuffer: Pointer to the computed digest. Its size must be 20 bytes.  
  * @param  Timeout: Timeout value    
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HASH_SHA1_Finish(HASH_HandleTypeDef *hhash, uint8_t* pOutBuffer, uint32_t Timeout)
{
  uint32_t timeout = 0;   
  
   /* Process Locked */
  __HAL_LOCK(hhash);
  
  /* Change HASH peripheral state */
  hhash->State = HAL_HASH_STATE_BUSY;
  
  /* Get timeout */
  timeout = HAL_GetTick() + Timeout;
  while(HAL_IS_BIT_CLR(HASH->SR, HASH_FLAG_DCIS))
  {
    /* Check for the Timeout */
    if(Timeout != HAL_MAX_DELAY)
    {
      if(HAL_GetTick() >= timeout)
      {
        /* Change state */
        hhash->State = HAL_HASH_STATE_TIMEOUT;
        
        /* Process Unlocked */          
        __HAL_UNLOCK(hhash);
        
        return HAL_TIMEOUT;
      }
    }
  }
  
  /* Read the message digest */
  HASH_GetDigest(pOutBuffer, 20);
  
  /* Change HASH peripheral state */
  hhash->State = HAL_HASH_STATE_READY;
  
   /* Process UnLock */
  __HAL_UNLOCK(hhash);
  
  /* Return function status */
  return HAL_OK;
}


/**
  * @}
  */

/** @defgroup HASH_Group5 HASH-MAC (HMAC) processing functions using polling mode 
 *  @brief   HMAC processing functions using polling mode . 
 *
@verbatim   
 ===============================================================================
              ##### HMAC processing using polling mode functions #####
 ===============================================================================  
    [..]  This section provides functions allowing to calculate in polling mode
          the HMAC value using one of the following algorithms:
      (+) MD5
      (+) SHA1

@endverbatim
  * @{
  */

/**
  * @brief  Initializes the HASH peripheral in HMAC MD5 mode
  *         then processes pInBuffer. The digest is available in pOutBuffer
  * @param  hhash: HASH handle
  * @param  pInBuffer: Pointer to the input buffer (buffer to be hashed).
  * @param  Size: Length of the input buffer in bytes.
  *          If the Size is not multiple of 64 bytes, the padding is managed by hardware.
  * @param  pOutBuffer: Pointer to the computed digest. Its size must be 20 bytes.
  * @param  Timeout: Timeout value  
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HMAC_MD5_Start(HASH_HandleTypeDef *hhash, uint8_t *pInBuffer, uint32_t Size, uint8_t* pOutBuffer, uint32_t Timeout)
{
  uint32_t timeout = 0;   
  
   /* Process Locked */
  __HAL_LOCK(hhash);
  
  /* Change the HASH state */
  hhash->State = HAL_HASH_STATE_BUSY;
  
  /* Check if initialization phase has already been performed */
  if(hhash->Phase == HAL_HASH_PHASE_READY)
  {
    /* Check if key size is greater than 64 bytes */
    if(hhash->Init.KeySize > 64)
    {
      /* Select the HMAC MD5 mode */
      HASH->CR |= (HASH_AlgoSelection_MD5 | HASH_AlgoMode_HMAC | HASH_HMACKeyType_LongKey | HASH_CR_INIT);
    }
    else
    {
      /* Select the HMAC MD5 mode */
      HASH->CR |= (HASH_AlgoSelection_MD5 | HASH_AlgoMode_HMAC | HASH_CR_INIT);
    }
  }
  
  /* Set the phase */
  hhash->Phase = HAL_HASH_PHASE_PROCESS;
  
  /************************** STEP 1 ******************************************/
  /* Configure the number of valid bits in last word of the message */
  __HAL_HASH_SET_NBVALIDBITS(hhash->Init.KeySize);
  
  /* Write input buffer in data register */
  HASH_WriteData(hhash->Init.pKey, hhash->Init.KeySize);
  
  /* Start the digest calculation */
  __HAL_HASH_START_DIGEST();
  
  /* Get timeout */
  timeout = HAL_GetTick() + Timeout;

  while(HAL_IS_BIT_SET(HASH->SR, HASH_FLAG_BUSY))
  {
    /* Check for the Timeout */
    if(Timeout != HAL_MAX_DELAY)
    {
      if(HAL_GetTick() >= timeout)
      {
        /* Change state */
        hhash->State = HAL_HASH_STATE_TIMEOUT;
        
        /* Process Unlocked */          
        __HAL_UNLOCK(hhash);
        
        return HAL_TIMEOUT;
      }
    }
  }
  /************************** STEP 2 ******************************************/
  /* Configure the number of valid bits in last word of the message */
  __HAL_HASH_SET_NBVALIDBITS(Size);
  
  /* Write input buffer in data register */
  HASH_WriteData(pInBuffer, Size);
  
  /* Start the digest calculation */
  __HAL_HASH_START_DIGEST();
  
  /* Get timeout */
  timeout = HAL_GetTick() + Timeout;
  
  while(HAL_IS_BIT_SET(HASH->SR, HASH_FLAG_BUSY))
  {
    /* Check for the Timeout */
    if(Timeout != HAL_MAX_DELAY)
    {
      if(HAL_GetTick() >= timeout)
      {
        /* Change state */
        hhash->State = HAL_HASH_STATE_TIMEOUT;
        
        /* Process Unlocked */          
        __HAL_UNLOCK(hhash);
        
        return HAL_TIMEOUT;
      }
    }
  }
  /************************** STEP 3 ******************************************/
  /* Configure the number of valid bits in last word of the message */
  __HAL_HASH_SET_NBVALIDBITS(hhash->Init.KeySize);
  
  /* Write input buffer in data register */
  HASH_WriteData(hhash->Init.pKey, hhash->Init.KeySize);
  
  /* Start the digest calculation */
  __HAL_HASH_START_DIGEST();
  
  /* Get timeout */
  timeout = HAL_GetTick() + Timeout;
  
  while(HAL_IS_BIT_SET(HASH->SR, HASH_FLAG_BUSY))
  {
    /* Check for the Timeout */
    if(Timeout != HAL_MAX_DELAY)
    {
      if(HAL_GetTick() >= timeout)
      {
        /* Change state */
        hhash->State = HAL_HASH_STATE_TIMEOUT;
        
        /* Process Unlocked */          
        __HAL_UNLOCK(hhash);
        
        return HAL_TIMEOUT;
      }
    }
  }
  
  /* Read the message digest */
  HASH_GetDigest(pOutBuffer, 16);
  
  /* Change the HASH state */
  hhash->State = HAL_HASH_STATE_READY;
  
  /* Process Unlocked */
  __HAL_UNLOCK(hhash);
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Initializes the HASH peripheral in HMAC SHA1 mode
  *         then processes pInBuffer. The digest is available in pOutBuffer.
  * @param  hhash: HASH handle
  * @param  pInBuffer: Pointer to the input buffer (buffer to be hashed).
  * @param   Size: Length of the input buffer in bytes.
  *          If the Size is not multiple of 64 bytes, the padding is managed by hardware.
  * @param  pOutBuffer: Pointer to the computed digest. Its size must be 20 bytes.
  * @param  Timeout: Timeout value  
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HMAC_SHA1_Start(HASH_HandleTypeDef *hhash, uint8_t *pInBuffer, uint32_t Size, uint8_t* pOutBuffer, uint32_t Timeout)
{
  uint32_t timeout = 0;   
  
  /* Process Locked */
  __HAL_LOCK(hhash);
  
  /* Change the HASH state */
  hhash->State = HAL_HASH_STATE_BUSY;
  
  /* Check if initialization phase has already been performed */
  if(hhash->Phase == HAL_HASH_PHASE_READY)
  {
    /* Check if key size is greater than 64 bytes */
    if(hhash->Init.KeySize > 64)
    {
      /* Select the HMAC SHA1 mode */
      HASH->CR |= (HASH_AlgoSelection_SHA1 | HASH_AlgoMode_HMAC | HASH_HMACKeyType_LongKey | HASH_CR_INIT);
    }
    else
    {
      /* Select the HMAC SHA1 mode */
      HASH->CR |= (HASH_AlgoSelection_SHA1 | HASH_AlgoMode_HMAC | HASH_CR_INIT);
    }
  }
  
  /* Set the phase */
  hhash->Phase = HAL_HASH_PHASE_PROCESS;
  
  /************************** STEP 1 ******************************************/
  /* Configure the number of valid bits in last word of the message */
  __HAL_HASH_SET_NBVALIDBITS(hhash->Init.KeySize);
  
  /* Write input buffer in data register */
  HASH_WriteData(hhash->Init.pKey, hhash->Init.KeySize);
  
  /* Start the digest calculation */
  __HAL_HASH_START_DIGEST();
  
  /* Get timeout */
  timeout = HAL_GetTick() + Timeout;
  
  while(HAL_IS_BIT_SET(HASH->SR, HASH_FLAG_BUSY))
  {
    /* Check for the Timeout */
    if(Timeout != HAL_MAX_DELAY)
    {
      if(HAL_GetTick() >= timeout)
      {
        /* Change state */
        hhash->State = HAL_HASH_STATE_TIMEOUT;
        
        /* Process Unlocked */          
        __HAL_UNLOCK(hhash);
        
        return HAL_TIMEOUT;
      }
    }
  }
  /************************** STEP 2 ******************************************/
  /* Configure the number of valid bits in last word of the message */  
  __HAL_HASH_SET_NBVALIDBITS(Size);
  
  /* Write input buffer in data register */
  HASH_WriteData(pInBuffer, Size);
  
  /* Start the digest calculation */
  __HAL_HASH_START_DIGEST();
  
  /* Get timeout */
  timeout = HAL_GetTick() + Timeout;
  
  while(HAL_IS_BIT_SET(HASH->SR, HASH_FLAG_BUSY))
  {
    /* Check for the Timeout */
    if(Timeout != HAL_MAX_DELAY)
    {
      if(HAL_GetTick() >= timeout)
      {
        /* Change state */
        hhash->State = HAL_HASH_STATE_TIMEOUT;
        
        /* Process Unlocked */          
        __HAL_UNLOCK(hhash);
        
        return HAL_TIMEOUT;
      }
    }
  }
  /************************** STEP 3 ******************************************/
  /* Configure the number of valid bits in last word of the message */
  __HAL_HASH_SET_NBVALIDBITS(hhash->Init.KeySize);
  
  /* Write input buffer in data register */
  HASH_WriteData(hhash->Init.pKey, hhash->Init.KeySize);
  
  /* Start the digest calculation */
  __HAL_HASH_START_DIGEST();
  
  /* Get timeout */
  timeout = HAL_GetTick() + Timeout;

  while(HAL_IS_BIT_SET(HASH->SR, HASH_FLAG_BUSY))
  {
    /* Check for the Timeout */
    if(Timeout != HAL_MAX_DELAY)
    {
      if(HAL_GetTick() >= timeout)
      {
        /* Change state */
        hhash->State = HAL_HASH_STATE_TIMEOUT;
        
        /* Process Unlocked */          
        __HAL_UNLOCK(hhash);
        
        return HAL_TIMEOUT;
      }
    }
  }
  /* Read the message digest */
  HASH_GetDigest(pOutBuffer, 20);
  
  /* Change the HASH state */
  hhash->State = HAL_HASH_STATE_READY;
  
  /* Process Unlocked */
  __HAL_UNLOCK(hhash);
  
  /* Return function status */
  return HAL_OK;
}




/**
  * @}
  */

/** @defgroup HASH_Group6 HASH-MAC (HMAC) processing functions using DMA mode 
 *  @brief   HMAC processing functions using DMA mode . 
 *
@verbatim   
 ===============================================================================
                ##### HMAC processing using DMA mode functions #####
 ===============================================================================  
    [..]  This section provides functions allowing to calculate in DMA mode
          the HMAC value using one of the following algorithms:
      (+) MD5
      (+) SHA1

@endverbatim
  * @{
  */

/**
  * @brief  Initializes the HASH peripheral in HMAC MD5 mode
  *         then enables DMA to control data transfer.
  * @param  hhash: HASH handle
  * @param  pInBuffer: Pointer to the input buffer (buffer to be hashed).
  * @param  Size: Length of the input buffer in bytes.
  *          If the Size is not multiple of 64 bytes, the padding is managed by hardware.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HMAC_MD5_Start_DMA(HASH_HandleTypeDef *hhash, uint8_t *pInBuffer, uint32_t Size)
{
  uint32_t inputaddr  = 0;
  
   /* Process Locked */
  __HAL_LOCK(hhash);
  
  /* Change the HASH state */
  hhash->State = HAL_HASH_STATE_BUSY;
  
  /* Save buffer pointer and size in handle */
  hhash->pHashInBuffPtr = pInBuffer;
  hhash->HashBuffSize = Size;
  hhash->HashInCount = 0;
  
  /* Check if initialization phase has already been performed */
  if(hhash->Phase == HAL_HASH_PHASE_READY)
  {
    /* Check if key size is greater than 64 bytes */
    if(hhash->Init.KeySize > 64)
    {
      /* Select the HMAC MD5 mode */
      HASH->CR |= (HASH_AlgoSelection_MD5 | HASH_AlgoMode_HMAC | HASH_HMACKeyType_LongKey | HASH_CR_INIT);
    }
    else
    {
      /* Select the HMAC MD5 mode */
      HASH->CR |= (HASH_AlgoSelection_MD5 | HASH_AlgoMode_HMAC | HASH_CR_INIT);
    }
  }
  
  /* Set the phase */
  hhash->Phase = HAL_HASH_PHASE_PROCESS;
  
  /* Configure the number of valid bits in last word of the message */
  __HAL_HASH_SET_NBVALIDBITS(hhash->Init.KeySize);
  
  /* Get the key address */
  inputaddr = (uint32_t)(hhash->Init.pKey);
  
  /* Set the HASH DMA transfer complete callback */
  hhash->hdmain->XferCpltCallback = HASH_DMAXferCplt;
  /* Set the DMA error callback */
  hhash->hdmain->XferErrorCallback = HASH_DMAError;
  
  /* Enable the DMA In DMA Stream */
  HAL_DMA_Start_IT(hhash->hdmain, inputaddr, (uint32_t)&HASH->DIN, (hhash->Init.KeySize%4 ? (hhash->Init.KeySize+3)/4:hhash->Init.KeySize/4));
  /* Enable DMA requests */
  HASH->CR |= (HASH_CR_DMAE);
  
  /* Process Unlocked */
  __HAL_UNLOCK(hhash);
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Initializes the HASH peripheral in HMAC SHA1 mode
  *         then enables DMA to control data transfer.
  * @param  hhash: HASH handle
  * @param  pInBuffer: Pointer to the input buffer (buffer to be hashed).
  * @param  Size: Length of the input buffer in bytes.
  *          If the Size is not multiple of 64 bytes, the padding is managed by hardware.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HMAC_SHA1_Start_DMA(HASH_HandleTypeDef *hhash, uint8_t *pInBuffer, uint32_t Size)
{
  uint32_t inputaddr  = 0;
  
  /* Process Locked */
  __HAL_LOCK(hhash);
  
  /* Change the HASH state */
  hhash->State = HAL_HASH_STATE_BUSY;
  
  /* Save buffer pointer and size in handle */
  hhash->pHashInBuffPtr = pInBuffer;
  hhash->HashBuffSize = Size;
  hhash->HashInCount = 0;
  
  /* Check if initialization phase has already been performed */
  if(hhash->Phase == HAL_HASH_PHASE_READY)
  {
    /* Check if key size is greater than 64 bytes */
    if(hhash->Init.KeySize > 64)
    {
      /* Select the HMAC SHA1 mode */
      HASH->CR |= (HASH_AlgoSelection_SHA1 | HASH_AlgoMode_HMAC | HASH_HMACKeyType_LongKey | HASH_CR_INIT);
    }
    else
    {
      /* Select the HMAC SHA1 mode */
      HASH->CR |= (HASH_AlgoSelection_SHA1 | HASH_AlgoMode_HMAC | HASH_CR_INIT);
    }
  }
  
  /* Set the phase */
  hhash->Phase = HAL_HASH_PHASE_PROCESS;
  
  /* Configure the number of valid bits in last word of the message */
  __HAL_HASH_SET_NBVALIDBITS(hhash->Init.KeySize);
  
  /* Get the key address */
  inputaddr = (uint32_t)(hhash->Init.pKey);
  
  /* Set the HASH DMA transfer complete callback */
  hhash->hdmain->XferCpltCallback = HASH_DMAXferCplt;
  /* Set the DMA error callback */
  hhash->hdmain->XferErrorCallback = HASH_DMAError;
  
  /* Enable the DMA In DMA Stream */
  HAL_DMA_Start_IT(hhash->hdmain, inputaddr, (uint32_t)&HASH->DIN, (hhash->Init.KeySize%4 ? (hhash->Init.KeySize+3)/4:hhash->Init.KeySize/4));
  /* Enable DMA requests */
  HASH->CR |= (HASH_CR_DMAE);
  
  /* Process Unlocked */
  __HAL_UNLOCK(hhash);
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @}
  */

/** @defgroup HASH_Group7 Peripheral State functions 
 *  @brief   Peripheral State functions. 
 *
@verbatim   
 ===============================================================================
                      ##### Peripheral State functions #####
 ===============================================================================  
    [..]
    This subsection permits to get in run-time the status of the peripheral.

@endverbatim
  * @{
  */

/**
  * @brief return the HASH state
  * @param  hhash: HASH handle
  * @retval HAL state
  */
HAL_HASH_STATETypeDef HAL_HASH_GetState(HASH_HandleTypeDef *hhash)
{
  return hhash->State;
}

/**
  * @}
  */

/**
  * @brief  DMA HASH Input Data complete callback. 
  * @param  hdma: DMA handle
  * @retval None
  */
static void HASH_DMAXferCplt(DMA_HandleTypeDef *hdma)
{
  HASH_HandleTypeDef* hhash = ( HASH_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;
  uint32_t inputaddr = 0;
  uint32_t buffersize = 0;
  
  if((HASH->CR & HASH_CR_MODE) != HASH_CR_MODE)
  {
    /* Disable the DMA transfer */
    HASH->CR &= (uint32_t)(~HASH_CR_DMAE);
    
    /* Change HASH peripheral state */
    hhash->State = HAL_HASH_STATE_READY;
    
    /* Call Input data transfer complete callback */
    HAL_HASH_InCpltCallback(hhash);
  }
  else
  {
    /* Increment Interrupt counter */
    hhash->HashInCount++;
    /* Disable the DMA transfer before starting the next transfer */
    HASH->CR &= (uint32_t)(~HASH_CR_DMAE);
    
    if(hhash->HashInCount <= 2)
    {
      /* In case HashInCount = 1, set the DMA to transfer data to HASH DIN register */
      if(hhash->HashInCount == 1)
      {
        inputaddr = (uint32_t)hhash->pHashInBuffPtr;
        buffersize = hhash->HashBuffSize;
      }
      /* In case HashInCount = 2, set the DMA to transfer key to HASH DIN register */
      else if(hhash->HashInCount == 2)
      {
        inputaddr = (uint32_t)hhash->Init.pKey;
        buffersize = hhash->Init.KeySize;
      }
      /* Configure the number of valid bits in last word of the message */
      HASH->STR |= 8 * (buffersize % 4);
      
      /* Set the HASH DMA transfer complete */
      hhash->hdmain->XferCpltCallback = HASH_DMAXferCplt;
      
      /* Enable the DMA In DMA Stream */
      HAL_DMA_Start_IT(hhash->hdmain, inputaddr, (uint32_t)&HASH->DIN, (buffersize%4 ? (buffersize+3)/4:buffersize/4));
      
      /* Enable DMA requests */
      HASH->CR |= (HASH_CR_DMAE);
    }
    else
    {
      /* Disable the DMA transfer */
      HASH->CR &= (uint32_t)(~HASH_CR_DMAE);
      
      /* Reset the InCount */
      hhash->HashInCount = 0;
      
      /* Change HASH peripheral state */
      hhash->State = HAL_HASH_STATE_READY;
      
      /* Call Input data transfer complete callback */
      HAL_HASH_InCpltCallback(hhash);
    }
  }
}

/**
  * @brief  DMA HASH communication error callback. 
  * @param  hdma: DMA handle
  * @retval None
  */
static void HASH_DMAError(DMA_HandleTypeDef *hdma)
{
  HASH_HandleTypeDef* hhash = ( HASH_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;
  hhash->State= HAL_HASH_STATE_READY;
  HAL_HASH_ErrorCallback(hhash);
}

/**
  * @brief  Writes the input buffer in data register.
  * @param  pInBuffer: Pointer to input buffer
  * @param  Size: The size of input buffer
  * @retval None
  */
static void HASH_WriteData(uint8_t *pInBuffer, uint32_t Size)
{
  uint32_t buffercounter;
  uint32_t inputaddr = (uint32_t) pInBuffer;
  
  for(buffercounter = 0; buffercounter < Size; buffercounter+=4)
  {
    HASH->DIN = *(uint32_t*)inputaddr;
    inputaddr+=4;
  }
}

/**
  * @brief  Provides the message digest result.
  * @param  pMsgDigest: Pointer to the message digest
  * @param  Size: The size of the message digest in bytes
  * @retval None
  */
static void HASH_GetDigest(uint8_t *pMsgDigest, uint8_t Size)
{
  uint32_t msgdigest = (uint32_t)pMsgDigest;
  
  switch(Size)
  {
  case 16:
    /* Read the message digest */
    *(uint32_t*)(msgdigest) = __REV(HASH->HR[0]);
    msgdigest+=4;
    *(uint32_t*)(msgdigest) = __REV(HASH->HR[1]);
    msgdigest+=4;
    *(uint32_t*)(msgdigest) = __REV(HASH->HR[2]);
    msgdigest+=4;
    *(uint32_t*)(msgdigest) = __REV(HASH->HR[3]);
    break;
  case 20:
    /* Read the message digest */
    *(uint32_t*)(msgdigest) = __REV(HASH->HR[0]);
    msgdigest+=4;
    *(uint32_t*)(msgdigest) = __REV(HASH->HR[1]);
    msgdigest+=4;
    *(uint32_t*)(msgdigest) = __REV(HASH->HR[2]);
    msgdigest+=4;
    *(uint32_t*)(msgdigest) = __REV(HASH->HR[3]);
    msgdigest+=4;
    *(uint32_t*)(msgdigest) = __REV(HASH->HR[4]);
    break;
  case 28:
    /* Read the message digest */
    *(uint32_t*)(msgdigest) = __REV(HASH->HR[0]);
    msgdigest+=4;
    *(uint32_t*)(msgdigest) = __REV(HASH->HR[1]);
    msgdigest+=4;
    *(uint32_t*)(msgdigest) = __REV(HASH->HR[2]);
    msgdigest+=4;
    *(uint32_t*)(msgdigest) = __REV(HASH->HR[3]);
    msgdigest+=4;
    *(uint32_t*)(msgdigest) = __REV(HASH->HR[4]);
    msgdigest+=4;
    *(uint32_t*)(msgdigest) = __REV(HASH_DIGEST->HR[5]);
    msgdigest+=4;
    *(uint32_t*)(msgdigest) = __REV(HASH_DIGEST->HR[6]);
    break;
  case 32:
    /* Read the message digest */
    *(uint32_t*)(msgdigest) = __REV(HASH->HR[0]);
    msgdigest+=4;
    *(uint32_t*)(msgdigest) = __REV(HASH->HR[1]);
    msgdigest+=4;
    *(uint32_t*)(msgdigest) = __REV(HASH->HR[2]);
    msgdigest+=4;
    *(uint32_t*)(msgdigest) = __REV(HASH->HR[3]);
    msgdigest+=4;
    *(uint32_t*)(msgdigest) = __REV(HASH->HR[4]);
    msgdigest+=4;
    *(uint32_t*)(msgdigest) = __REV(HASH_DIGEST->HR[5]);
    msgdigest+=4;
    *(uint32_t*)(msgdigest) = __REV(HASH_DIGEST->HR[6]);
    msgdigest+=4;
    *(uint32_t*)(msgdigest) = __REV(HASH_DIGEST->HR[7]);
    break;
  default:
    break;
  }
}

/**
  * @}
  */
#endif /* STM32F415xx || STM32F417xx || STM32F437xx || STM32F439xx */
#endif /* HAL_HASH_MODULE_ENABLED */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
