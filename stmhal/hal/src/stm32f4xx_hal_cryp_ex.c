/**
  ******************************************************************************
  * @file    stm32f4xx_hal_cryp_ex.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    18-February-2014
  * @brief   Extended CRYP HAL module driver
  *          This file provides firmware functions to manage the following 
  *          functionalities of CRYP extension peripheral:
  *           + Extended AES processing functions     
  *  
  @verbatim
  ==============================================================================
                     ##### How to use this driver #####
  ==============================================================================
    [..]
    The CRYP Extension HAL driver can be used as follows:
    (#)Initialize the CRYP low level resources by implementing the HAL_CRYP_MspInit():
        (##) Enable the CRYP interface clock using __CRYP_CLK_ENABLE()
        (##) In case of using interrupts (e.g. HAL_CRYPEx_AESGCM_Encrypt_IT())
            (+++) Configure the CRYP interrupt priority using HAL_NVIC_SetPriority()
            (+++) Enable the CRYP IRQ handler using HAL_NVIC_EnableIRQ()
            (+) In CRYP IRQ handler, call HAL_CRYP_IRQHandler()
        (##) In case of using DMA to control data transfer (e.g. HAL_AES_ECB_Encrypt_DMA())
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
        (##) The encryption/decryption key. Its size depends on the algorithm
                used for encryption/decryption
        (##) The initialization vector (counter). It is not used ECB mode.
    (#)Three processing (encryption/decryption) functions are available:
        (##) Polling mode: encryption and decryption APIs are blocking functions
             i.e. they process the data and wait till the processing is finished
             e.g. HAL_CRYPEx_AESGCM_Encrypt()
        (##) Interrupt mode: encryption and decryption APIs are not blocking functions
                i.e. they process the data under interrupt
                e.g. HAL_CRYPEx_AESGCM_Encrypt_IT()
        (##) DMA mode: encryption and decryption APIs are not blocking functions
                i.e. the data transfer is ensured by DMA
                e.g. HAL_CRYPEx_AESGCM_Encrypt_DMA()
    (#)When the processing function is called at first time after HAL_CRYP_Init()
       the CRYP peripheral is initialized and processes the buffer in input.
       At second call, the processing function performs an append of the already
       processed buffer.
       When a new data block is to be processed, call HAL_CRYP_Init() then the
       processing function.
    (#)In AES-GCM and AES-CCM modes are an authenticated encryption algorithms
       which provide authentication messages.
       HAL_AES_GCM_Finish() and HAL_AES_CCM_Finish() are used to provide those
       authentication messages.
       Call those functions after the processing ones (polling, interrupt or DMA).
       e.g. in AES-CCM mode call HAL_CRYPEx_AESCCM_Encrypt() to encrypt the plain data
            then call HAL_CRYPEx_AESCCM_Finish() to get the authentication message
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

/** @defgroup CRYPEx 
  * @brief CRYP Extension HAL module driver.
  * @{
  */

#ifdef HAL_CRYP_MODULE_ENABLED

#if defined(STM32F437xx) || defined(STM32F439xx)

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void CRYPEx_GCMCCM_SetInitVector(CRYP_HandleTypeDef *hcryp, uint8_t *InitVector, uint32_t IVSize);
static void CRYPEx_GCMCCM_SetKey(CRYP_HandleTypeDef *hcryp, uint8_t *Key, uint32_t KeySize);
static HAL_StatusTypeDef CRYPEx_GCMCCM_ProcessData(CRYP_HandleTypeDef *hcryp, uint8_t *Input, uint16_t Ilength, uint8_t *Output, uint32_t Timeout);
static HAL_StatusTypeDef CRYPEx_GCMCCM_SetHeaderPhase(CRYP_HandleTypeDef *hcryp, uint8_t* Input, uint16_t Ilength, uint32_t Timeout);
static void CRYPEx_GCMCCM_DMAInCplt(DMA_HandleTypeDef *hdma);
static void CRYPEx_GCMCCM_DMAOutCplt(DMA_HandleTypeDef *hdma);
static void CRYPEx_GCMCCM_DMAError(DMA_HandleTypeDef *hdma);
static void CRYPEx_GCMCCM_SetDMAConfig(CRYP_HandleTypeDef *hcryp, uint32_t inputaddr, uint16_t Size, uint32_t outputaddr);

/* Private functions ---------------------------------------------------------*/

/** @defgroup CRYPEx_Private_Functions
  * @{
  */

/** @defgroup CRYPEx_Group1 Extended AES processing functions 
 *  @brief   Extended processing functions. 
 *
@verbatim   
  ==============================================================================
              ##### Extended AES processing functions #####
  ==============================================================================  
    [..]  This section provides functions allowing to:
      (+) Encrypt plaintext using AES-128/192/256 using GCM and CCM chaining modes
      (+) Decrypt cyphertext using AES-128/192/256 using GCM and CCM chaining modes
      (+) Finish the processing. This function is available only for GCM and CCM
    [..]  Three processing methods are available:
      (+) Polling mode
      (+) Interrupt mode
      (+) DMA mode

@endverbatim
  * @{
  */


/**
  * @brief  Initializes the CRYP peripheral in AES CCM encryption mode then 
  *         encrypt pPlainData. The cypher data are available in pCypherData.
  * @param  hcryp: CRYP handle
  * @param  pPlainData: Pointer to the plaintext buffer
  * @param  Size: Length of the plaintext buffer, must be a multiple of 16
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYPEx_AESCCM_Encrypt(CRYP_HandleTypeDef *hcryp, uint8_t *pPlainData, uint16_t Size, uint8_t *pCypherData, uint32_t Timeout)
{
  uint32_t timeout = 0;   
  uint32_t headersize = hcryp->Init.HeaderSize;
  uint32_t headeraddr = (uint32_t)hcryp->Init.Header;
  uint32_t loopcounter = 0;
  uint32_t bufferidx = 0;
  uint8_t blockb0[16] = {0};/* Block B0 */
  uint8_t ctr[16] = {0}; /* Counter */
  uint32_t b0addr = (uint32_t)blockb0;
  
  /* Process Locked */
  __HAL_LOCK(hcryp);
  
  /* Change the CRYP peripheral state */
  hcryp->State = HAL_CRYP_STATE_BUSY;
  
  /* Check if initialization phase has already been performed */
  if(hcryp->Phase == HAL_CRYP_PHASE_READY)
  {
    /************************ Formatting the header block *********************/
    if(headersize != 0)
    {
      /* Check that the associated data (or header) length is lower than 2^16 - 2^8 = 65536 - 256 = 65280 */
      if(headersize < 65280)
      {
        hcryp->Init.pScratch[bufferidx++] = (uint8_t) ((headersize >> 8) & 0xFF);
        hcryp->Init.pScratch[bufferidx++] = (uint8_t) ((headersize) & 0xFF);
        headersize += 2;
      }
      else
      {
        /* Header is encoded as 0xff || 0xfe || [headersize]32, i.e., six octets */
        hcryp->Init.pScratch[bufferidx++] = 0xFF;
        hcryp->Init.pScratch[bufferidx++] = 0xFE;
        hcryp->Init.pScratch[bufferidx++] = headersize & 0xff000000;
        hcryp->Init.pScratch[bufferidx++] = headersize & 0x00ff0000;
        hcryp->Init.pScratch[bufferidx++] = headersize & 0x0000ff00;
        hcryp->Init.pScratch[bufferidx++] = headersize & 0x000000ff;
        headersize += 6;
      }
      /* Copy the header buffer in internal buffer "hcryp->Init.pScratch" */
      for(loopcounter = 0; loopcounter < headersize; loopcounter++)
      {
        hcryp->Init.pScratch[bufferidx++] = hcryp->Init.Header[loopcounter];
      }
      /* Check if the header size is modulo 16 */
      if ((headersize % 16) != 0)
      {
        /* Padd the header buffer with 0s till the hcryp->Init.pScratch length is modulo 16 */
        for(loopcounter = headersize; loopcounter <= ((headersize/16) + 1) * 16; loopcounter++)
        {
          hcryp->Init.pScratch[loopcounter] = 0;
        }
        /* Set the header size to modulo 16 */
        headersize = ((headersize/16) + 1) * 16;
      }
      /* Set the pointer headeraddr to hcryp->Init.pScratch */
      headeraddr = (uint32_t)hcryp->Init.pScratch;
    }
    /*********************** Formatting the block B0 **************************/
    if(headersize != 0)
    {
      blockb0[0] = 0x40;
    }
    /* Flags byte */
    /* blockb0[0] |= 0u | (((( (uint8_t) hcryp->Init.TagSize - 2) / 2) & 0x07 ) << 3 ) | ( ( (uint8_t) (15 - hcryp->Init.IVSize) - 1) & 0x07) */
    blockb0[0] |= (uint8_t)((uint8_t)((uint8_t)(((uint8_t)(hcryp->Init.TagSize - (uint8_t)(2))) >> 1) & (uint8_t)0x07 ) << 3);
    blockb0[0] |= (uint8_t)((uint8_t)((uint8_t)((uint8_t)(15) - hcryp->Init.IVSize) - (uint8_t)1) & (uint8_t)0x07);
 
    for (loopcounter = 0; loopcounter < hcryp->Init.IVSize; loopcounter++)
    {
      blockb0[loopcounter+1] = hcryp->Init.pInitVect[loopcounter];
    }
    for ( ; loopcounter < 13; loopcounter++)
    {
      blockb0[loopcounter+1] = 0;
    }
    
    blockb0[14] = (Size >> 8);
    blockb0[15] = (Size & 0xFF);
    
    /************************* Formatting the initial counter *****************/
    /* Byte 0:
       Bits 7 and 6 are reserved and shall be set to 0
       Bits 3, 4, and 5 shall also be set to 0, to ensure that all the counter blocks
       are distinct from B0
       Bits 0, 1, and 2 contain the same encoding of q as in B0
    */
    ctr[0] = blockb0[0] & 0x07;
    /* byte 1 to NonceSize is the IV (Nonce) */
    for(loopcounter = 1; loopcounter < hcryp->Init.IVSize + 1; loopcounter++)
    {
      ctr[loopcounter] = blockb0[loopcounter];
    }
    /* Set the LSB to 1 */
    ctr[15] |= 0x01;
    
    /* Set the key */
    CRYPEx_GCMCCM_SetKey(hcryp, hcryp->Init.pKey, hcryp->Init.KeySize);
    
    /* Set the CRYP peripheral in AES CCM mode */
    __HAL_CRYP_SET_MODE(CRYP_CR_ALGOMODE_AES_CCM_ENCRYPT);
    
    /* Set the Initialization Vector */
    CRYPEx_GCMCCM_SetInitVector(hcryp, ctr, CRYP_KEYSIZE_128B);
    
    /* Select init phase */
    __HAL_CRYP_SET_PHASE(CRYP_PHASE_INIT);
    
    b0addr = (uint32_t)blockb0;
    /* Write the blockb0 block in the IN FIFO */
    CRYP->DR = *(uint32_t*)(b0addr);
    b0addr+=4;
    CRYP->DR = *(uint32_t*)(b0addr);
    b0addr+=4;
    CRYP->DR = *(uint32_t*)(b0addr);
    b0addr+=4;
    CRYP->DR = *(uint32_t*)(b0addr);
    
    /* Enable the CRYP peripheral */
    __HAL_CRYP_ENABLE();
    
    /* Get timeout */
    timeout = HAL_GetTick() + Timeout;

    while((CRYP->CR & CRYP_CR_CRYPEN) == CRYP_CR_CRYPEN)
    {
      /* Check for the Timeout */
      if(Timeout != HAL_MAX_DELAY)
      {
        if(HAL_GetTick() >= timeout)
        {
          /* Change state */
          hcryp->State = HAL_CRYP_STATE_TIMEOUT;
          
          /* Process Unlocked */          
          __HAL_UNLOCK(hcryp);
        
          return HAL_TIMEOUT;
        }
      }
    }
    /***************************** Header phase *******************************/
    if(headersize != 0)
    {
      /* Select header phase */
      __HAL_CRYP_SET_PHASE(CRYP_PHASE_HEADER);
      
      /* Enable the CRYP peripheral */
      __HAL_CRYP_ENABLE();
      
      for(loopcounter = 0; (loopcounter < headersize); loopcounter+=16)
      {
        /* Get timeout */
        timeout = HAL_GetTick() + Timeout;
        while(HAL_IS_BIT_CLR(CRYP->SR, CRYP_FLAG_IFEM))
        {
          {
            /* Check for the Timeout */
            if(Timeout != HAL_MAX_DELAY)
            {
              if(HAL_GetTick() >= timeout)
              {
                /* Change state */
                hcryp->State = HAL_CRYP_STATE_TIMEOUT;
                
                /* Process Unlocked */          
                __HAL_UNLOCK(hcryp);
                
                return HAL_TIMEOUT;
              }
            }
          }
        }
        /* Write the header block in the IN FIFO */
        CRYP->DR = *(uint32_t*)(headeraddr);
        headeraddr+=4;
        CRYP->DR = *(uint32_t*)(headeraddr);
        headeraddr+=4;
        CRYP->DR = *(uint32_t*)(headeraddr);
        headeraddr+=4;
        CRYP->DR = *(uint32_t*)(headeraddr);
        headeraddr+=4;
      }
      
      /* Get timeout */
      timeout = HAL_GetTick() + Timeout;

      while((CRYP->SR & CRYP_FLAG_BUSY) == CRYP_FLAG_BUSY)
      {
        /* Check for the Timeout */
        if(Timeout != HAL_MAX_DELAY)
        {
          if(HAL_GetTick() >= timeout)
          {
            /* Change state */
            hcryp->State = HAL_CRYP_STATE_TIMEOUT;
            
            /* Process Unlocked */          
            __HAL_UNLOCK(hcryp);
            
            return HAL_TIMEOUT;
          }
        }
      }
    }
    /* Save formatted counter into the scratch buffer pScratch */
    for(loopcounter = 0; (loopcounter < 16); loopcounter++)
    {
      hcryp->Init.pScratch[loopcounter] = ctr[loopcounter];
    }
    /* Reset bit 0 */
    hcryp->Init.pScratch[15] &= 0xfe;
    
    /* Select payload phase once the header phase is performed */
    __HAL_CRYP_SET_PHASE(CRYP_PHASE_PAYLOAD);
    
    /* Flush FIFO */
    __HAL_CRYP_FIFO_FLUSH();
    
    /* Enable the CRYP peripheral */
    __HAL_CRYP_ENABLE();
    
    /* Set the phase */
    hcryp->Phase = HAL_CRYP_PHASE_PROCESS;
  }
  
  /* Write Plain Data and Get Cypher Data */
  if(CRYPEx_GCMCCM_ProcessData(hcryp,pPlainData, Size, pCypherData, Timeout) != HAL_OK)
  {
    return HAL_TIMEOUT;
  }
  
  /* Change the CRYP peripheral state */
  hcryp->State = HAL_CRYP_STATE_READY;
  
  /* Process Unlocked */
  __HAL_UNLOCK(hcryp);
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Initializes the CRYP peripheral in AES GCM encryption mode then 
  *         encrypt pPlainData. The cypher data are available in pCypherData.
  * @param  hcryp: CRYP handle
  * @param  pPlainData: Pointer to the plaintext buffer
  * @param  Size: Length of the plaintext buffer, must be a multiple of 16
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYPEx_AESGCM_Encrypt(CRYP_HandleTypeDef *hcryp, uint8_t *pPlainData, uint16_t Size, uint8_t *pCypherData, uint32_t Timeout)
{
  uint32_t timeout = 0;   
  
  /* Process Locked */
  __HAL_LOCK(hcryp);
  
  /* Change the CRYP peripheral state */
  hcryp->State = HAL_CRYP_STATE_BUSY;
  
  /* Check if initialization phase has already been performed */
  if(hcryp->Phase == HAL_CRYP_PHASE_READY)
  {
    /* Set the key */
    CRYPEx_GCMCCM_SetKey(hcryp, hcryp->Init.pKey, hcryp->Init.KeySize);
    
    /* Set the CRYP peripheral in AES GCM mode */
    __HAL_CRYP_SET_MODE(CRYP_CR_ALGOMODE_AES_GCM_ENCRYPT);
    
    /* Set the Initialization Vector */
    CRYPEx_GCMCCM_SetInitVector(hcryp, hcryp->Init.pInitVect, CRYP_KEYSIZE_128B);
    
    /* Flush FIFO */
    __HAL_CRYP_FIFO_FLUSH();
    
    /* Enable the CRYP peripheral */
    __HAL_CRYP_ENABLE();
    
    /* Get timeout */
    timeout = HAL_GetTick() + Timeout;

    while((CRYP->CR & CRYP_CR_CRYPEN) == CRYP_CR_CRYPEN)
    {
      /* Check for the Timeout */
      if(Timeout != HAL_MAX_DELAY)
      {
        if(HAL_GetTick() >= timeout)
        {
          /* Change state */
          hcryp->State = HAL_CRYP_STATE_TIMEOUT;
          
          /* Process Unlocked */          
          __HAL_UNLOCK(hcryp);
          
          return HAL_TIMEOUT;
        }
      }
    }
    
    /* Set the header phase */
    if(CRYPEx_GCMCCM_SetHeaderPhase(hcryp, hcryp->Init.Header, hcryp->Init.HeaderSize, Timeout) != HAL_OK)
    {
      return HAL_TIMEOUT;
    }
    
    /* Disable the CRYP peripheral */
    __HAL_CRYP_DISABLE();
    
    /* Select payload phase once the header phase is performed */
    __HAL_CRYP_SET_PHASE(CRYP_PHASE_PAYLOAD);
    
    /* Flush FIFO */
    __HAL_CRYP_FIFO_FLUSH();
    
    /* Enable the CRYP peripheral */
    __HAL_CRYP_ENABLE();
    
    /* Set the phase */
    hcryp->Phase = HAL_CRYP_PHASE_PROCESS;
  }
  
  /* Write Plain Data and Get Cypher Data */
  if(CRYPEx_GCMCCM_ProcessData(hcryp, pPlainData, Size, pCypherData, Timeout) != HAL_OK)
  {
    return HAL_TIMEOUT;
  }
  
  /* Change the CRYP peripheral state */
  hcryp->State = HAL_CRYP_STATE_READY;
  
  /* Process Unlocked */
  __HAL_UNLOCK(hcryp);
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Initializes the CRYP peripheral in AES GCM decryption mode then
  *         decrypted pCypherData. The cypher data are available in pPlainData.
  * @param  hcryp: CRYP handle
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @param  Size: Length of the cyphertext buffer, must be a multiple of 16
  * @param  pPlainData: Pointer to the plaintext buffer 
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYPEx_AESGCM_Decrypt(CRYP_HandleTypeDef *hcryp, uint8_t *pCypherData, uint16_t Size, uint8_t *pPlainData, uint32_t Timeout)
{
  uint32_t timeout = 0;   
  
  /* Process Locked */
  __HAL_LOCK(hcryp);
  
  /* Change the CRYP peripheral state */
  hcryp->State = HAL_CRYP_STATE_BUSY;
  
  /* Check if initialization phase has already been performed */
  if(hcryp->Phase == HAL_CRYP_PHASE_READY)
  {
    /* Set the key */
    CRYPEx_GCMCCM_SetKey(hcryp, hcryp->Init.pKey, hcryp->Init.KeySize);
    
    /* Set the CRYP peripheral in AES GCM decryption mode */
    __HAL_CRYP_SET_MODE(CRYP_CR_ALGOMODE_AES_GCM_DECRYPT);
    
    /* Set the Initialization Vector */
    CRYPEx_GCMCCM_SetInitVector(hcryp, hcryp->Init.pInitVect, CRYP_KEYSIZE_128B);
    
    /* Flush FIFO */
    __HAL_CRYP_FIFO_FLUSH();
    
    /* Enable the CRYP peripheral */
    __HAL_CRYP_ENABLE();
    
    /* Get the timeout */
    timeout = HAL_GetTick() + Timeout;

    while((CRYP->CR & CRYP_CR_CRYPEN) == CRYP_CR_CRYPEN)
    {
      /* Check for the Timeout */
      if(Timeout != HAL_MAX_DELAY)
      {
        if(HAL_GetTick() >= timeout)
        {
          /* Change state */
          hcryp->State = HAL_CRYP_STATE_TIMEOUT;
          
          /* Process Unlocked */          
          __HAL_UNLOCK(hcryp);
          
          return HAL_TIMEOUT;
        }
      }
    }
    
    /* Set the header phase */
    if(CRYPEx_GCMCCM_SetHeaderPhase(hcryp, hcryp->Init.Header, hcryp->Init.HeaderSize, Timeout) != HAL_OK)
    {
      return HAL_TIMEOUT;
    }
    /* Disable the CRYP peripheral */
    __HAL_CRYP_DISABLE();
    
    /* Select payload phase once the header phase is performed */
    __HAL_CRYP_SET_PHASE(CRYP_PHASE_PAYLOAD);
    
    /* Enable the CRYP peripheral */
    __HAL_CRYP_ENABLE();
    
    /* Set the phase */
    hcryp->Phase = HAL_CRYP_PHASE_PROCESS;
  }
  
  /* Write Plain Data and Get Cypher Data */
  if(CRYPEx_GCMCCM_ProcessData(hcryp, pCypherData, Size, pPlainData, Timeout) != HAL_OK)
  {
    return HAL_TIMEOUT;
  }
  
  /* Change the CRYP peripheral state */
  hcryp->State = HAL_CRYP_STATE_READY;
  
  /* Process Unlocked */
  __HAL_UNLOCK(hcryp);
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Computes the authentication TAG.
  * @param  hcryp: CRYP handle
  * @param  Size: Total length of the plain/cyphertext buffer
  * @param  AuthTag: Pointer to the authentication buffer
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYPEx_AESGCM_Finish(CRYP_HandleTypeDef *hcryp, uint16_t Size, uint8_t *AuthTag, uint32_t Timeout)
{
  uint32_t timeout = 0;   
  uint32_t headerlength = hcryp->Init.HeaderSize * 8; /* Header length in bits */
  uint32_t inputlength = Size * 8; /* input length in bits */
  uint32_t tagaddr = (uint32_t)AuthTag;
  
  /* Process Locked */
  __HAL_LOCK(hcryp);
  
  /* Change the CRYP peripheral state */
  hcryp->State = HAL_CRYP_STATE_BUSY;
  
  /* Check if initialization phase has already been performed */
  if(hcryp->Phase == HAL_CRYP_PHASE_PROCESS)
  {
    /* Change the CRYP phase */
    hcryp->Phase = HAL_CRYP_PHASE_FINAL;
    
    /* Disable CRYP to start the final phase */
    __HAL_CRYP_DISABLE();
    
    /* Select final phase */
    __HAL_CRYP_SET_PHASE(CRYP_PHASE_FINAL);
    
    /* Enable the CRYP peripheral */
    __HAL_CRYP_ENABLE();
    
    /* Write the number of bits in header (64 bits) followed by the number of bits
       in the payload */
    if(hcryp->Init.DataType == CRYP_DATATYPE_1B)
    {
      CRYP->DR = 0;
      CRYP->DR = __RBIT(headerlength);
      CRYP->DR = 0;
      CRYP->DR = __RBIT(inputlength);
    }
    else if(hcryp->Init.DataType == CRYP_DATATYPE_8B)
    {
      CRYP->DR = 0;
      CRYP->DR = __REV(headerlength);
      CRYP->DR = 0;
      CRYP->DR = __REV(inputlength);
    }
    else if(hcryp->Init.DataType == CRYP_DATATYPE_16B)
    {
      CRYP->DR = 0;
      CRYP->DR = __REV16(headerlength);
      CRYP->DR = 0;
      CRYP->DR = __REV16(inputlength);
    }
    else if(hcryp->Init.DataType == CRYP_DATATYPE_32B)
    {
      CRYP->DR = 0;
      CRYP->DR = (uint32_t)(headerlength);
      CRYP->DR = 0;
      CRYP->DR = (uint32_t)(inputlength);
    }
    /* Get timeout */
    timeout = HAL_GetTick() + Timeout;

    while(HAL_IS_BIT_CLR(CRYP->SR, CRYP_FLAG_OFNE))
    {
      /* Check for the Timeout */
      if(Timeout != HAL_MAX_DELAY)
      {
        if(HAL_GetTick() >= timeout)
        {
          /* Change state */
          hcryp->State = HAL_CRYP_STATE_TIMEOUT;
          
          /* Process Unlocked */          
          __HAL_UNLOCK(hcryp);
        
          return HAL_TIMEOUT;
        }
      }
    }
    
    /* Read the Auth TAG in the IN FIFO */
    *(uint32_t*)(tagaddr) = CRYP->DOUT;
    tagaddr+=4;
    *(uint32_t*)(tagaddr) = CRYP->DOUT;
    tagaddr+=4;
    *(uint32_t*)(tagaddr) = CRYP->DOUT;
    tagaddr+=4;
    *(uint32_t*)(tagaddr) = CRYP->DOUT;
  }
  
  /* Change the CRYP peripheral state */
  hcryp->State = HAL_CRYP_STATE_READY;
  
  /* Process Unlocked */
  __HAL_UNLOCK(hcryp);
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Computes the authentication TAG for AES CCM mode.
  * @note   This API is called after HAL_AES_CCM_Encrypt()/HAL_AES_CCM_Decrypt()   
  * @param  hcryp: CRYP handle
  * @param  AuthTag: Pointer to the authentication buffer
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYPEx_AESCCM_Finish(CRYP_HandleTypeDef *hcryp, uint8_t *AuthTag, uint32_t Timeout)
{
  uint32_t timeout = 0;   
  uint32_t tagaddr = (uint32_t)AuthTag;
  uint32_t ctraddr = (uint32_t)hcryp->Init.pScratch;
  uint32_t temptag[4] = {0}; /* Temporary TAG (MAC) */
  uint32_t loopcounter;
  
  /* Process Locked */
  __HAL_LOCK(hcryp);
  
  /* Change the CRYP peripheral state */
  hcryp->State = HAL_CRYP_STATE_BUSY;
  
  /* Check if initialization phase has already been performed */
  if(hcryp->Phase == HAL_CRYP_PHASE_PROCESS)
  {
    /* Change the CRYP phase */
    hcryp->Phase = HAL_CRYP_PHASE_FINAL;
    
    /* Disable CRYP to start the final phase */
    __HAL_CRYP_DISABLE();
    
    /* Select final phase */
    __HAL_CRYP_SET_PHASE(CRYP_PHASE_FINAL);
    
    /* Enable the CRYP peripheral */
    __HAL_CRYP_ENABLE();
    
    /* Write the counter block in the IN FIFO */
    CRYP->DR = *(uint32_t*)ctraddr;
    ctraddr+=4;
    CRYP->DR = *(uint32_t*)ctraddr;
    ctraddr+=4;
    CRYP->DR = *(uint32_t*)ctraddr;
    ctraddr+=4;
    CRYP->DR = *(uint32_t*)ctraddr;
    
    /* Get timeout */
    timeout = HAL_GetTick() + Timeout;

    while(HAL_IS_BIT_CLR(CRYP->SR, CRYP_FLAG_OFNE))
    {
      /* Check for the Timeout */
      if(Timeout != HAL_MAX_DELAY)
      {
        if(HAL_GetTick() >= timeout)
        {
          /* Change state */
          hcryp->State = HAL_CRYP_STATE_TIMEOUT;
          
          /* Process Unlocked */          
          __HAL_UNLOCK(hcryp);
          
          return HAL_TIMEOUT;
        }
      }
    }
    
    /* Read the Auth TAG in the IN FIFO */
    temptag[0] = CRYP->DOUT;
    temptag[1] = CRYP->DOUT;
    temptag[2] = CRYP->DOUT;
    temptag[3] = CRYP->DOUT;
  }
  
  /* Copy temporary authentication TAG in user TAG buffer */
  for(loopcounter = 0; loopcounter < hcryp->Init.TagSize ; loopcounter++)
  {
    /* Set the authentication TAG buffer */
    *((uint8_t*)tagaddr+loopcounter) = *((uint8_t*)temptag+loopcounter);
  }
  
  /* Change the CRYP peripheral state */
  hcryp->State = HAL_CRYP_STATE_READY;
  
  /* Process Unlocked */
  __HAL_UNLOCK(hcryp);
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Initializes the CRYP peripheral in AES CCM decryption mode then
  *         decrypted pCypherData. The cypher data are available in pPlainData.
  * @param  hcryp: CRYP handle
  * @param  pPlainData: Pointer to the plaintext buffer
  * @param  Size: Length of the plaintext buffer, must be a multiple of 16
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYPEx_AESCCM_Decrypt(CRYP_HandleTypeDef *hcryp, uint8_t *pCypherData, uint16_t Size, uint8_t *pPlainData, uint32_t Timeout)
{
  uint32_t timeout = 0;   
  uint32_t headersize = hcryp->Init.HeaderSize;
  uint32_t headeraddr = (uint32_t)hcryp->Init.Header;
  uint32_t loopcounter = 0;
  uint32_t bufferidx = 0;
  uint8_t blockb0[16] = {0};/* Block B0 */
  uint8_t ctr[16] = {0}; /* Counter */
  uint32_t b0addr = (uint32_t)blockb0;
  
  /* Process Locked */
  __HAL_LOCK(hcryp);
  
  /* Change the CRYP peripheral state */
  hcryp->State = HAL_CRYP_STATE_BUSY;
  
  /* Check if initialization phase has already been performed */
  if(hcryp->Phase == HAL_CRYP_PHASE_READY)
  {
    /************************ Formatting the header block *********************/
    if(headersize != 0)
    {
      /* Check that the associated data (or header) length is lower than 2^16 - 2^8 = 65536 - 256 = 65280 */
      if(headersize < 65280)
      {
        hcryp->Init.pScratch[bufferidx++] = (uint8_t) ((headersize >> 8) & 0xFF);
        hcryp->Init.pScratch[bufferidx++] = (uint8_t) ((headersize) & 0xFF);
        headersize += 2;
      }
      else
      {
        /* Header is encoded as 0xff || 0xfe || [headersize]32, i.e., six octets */
        hcryp->Init.pScratch[bufferidx++] = 0xFF;
        hcryp->Init.pScratch[bufferidx++] = 0xFE;
        hcryp->Init.pScratch[bufferidx++] = headersize & 0xff000000;
        hcryp->Init.pScratch[bufferidx++] = headersize & 0x00ff0000;
        hcryp->Init.pScratch[bufferidx++] = headersize & 0x0000ff00;
        hcryp->Init.pScratch[bufferidx++] = headersize & 0x000000ff;
        headersize += 6;
      }
      /* Copy the header buffer in internal buffer "hcryp->Init.pScratch" */
      for(loopcounter = 0; loopcounter < headersize; loopcounter++)
      {
        hcryp->Init.pScratch[bufferidx++] = hcryp->Init.Header[loopcounter];
      }
      /* Check if the header size is modulo 16 */
      if ((headersize % 16) != 0)
      {
        /* Padd the header buffer with 0s till the hcryp->Init.pScratch length is modulo 16 */
        for(loopcounter = headersize; loopcounter <= ((headersize/16) + 1) * 16; loopcounter++)
        {
          hcryp->Init.pScratch[loopcounter] = 0;
        }
        /* Set the header size to modulo 16 */
        headersize = ((headersize/16) + 1) * 16;
      }
      /* Set the pointer headeraddr to hcryp->Init.pScratch */
      headeraddr = (uint32_t)hcryp->Init.pScratch;
    }
    /*********************** Formatting the block B0 **************************/
    if(headersize != 0)
    {
      blockb0[0] = 0x40;
    }
    /* Flags byte */
    /* blockb0[0] |= 0u | (((( (uint8_t) hcryp->Init.TagSize - 2) / 2) & 0x07 ) << 3 ) | ( ( (uint8_t) (15 - hcryp->Init.IVSize) - 1) & 0x07) */
    blockb0[0] |= (uint8_t)((uint8_t)((uint8_t)(((uint8_t)(hcryp->Init.TagSize - (uint8_t)(2))) >> 1) & (uint8_t)0x07 ) << 3);
    blockb0[0] |= (uint8_t)((uint8_t)((uint8_t)((uint8_t)(15) - hcryp->Init.IVSize) - (uint8_t)1) & (uint8_t)0x07);
    
    for (loopcounter = 0; loopcounter < hcryp->Init.IVSize; loopcounter++)
    {
      blockb0[loopcounter+1] = hcryp->Init.pInitVect[loopcounter];
    }
    for ( ; loopcounter < 13; loopcounter++)
    {
      blockb0[loopcounter+1] = 0;
    }
    
    blockb0[14] = (Size >> 8);
    blockb0[15] = (Size & 0xFF);
    
    /************************* Formatting the initial counter *****************/
    /* Byte 0:
       Bits 7 and 6 are reserved and shall be set to 0
       Bits 3, 4, and 5 shall also be set to 0, to ensure that all the counter 
       blocks are distinct from B0
       Bits 0, 1, and 2 contain the same encoding of q as in B0
    */
    ctr[0] = blockb0[0] & 0x07;
    /* byte 1 to NonceSize is the IV (Nonce) */
    for(loopcounter = 1; loopcounter < hcryp->Init.IVSize + 1; loopcounter++)
    {
      ctr[loopcounter] = blockb0[loopcounter];
    }
    /* Set the LSB to 1 */
    ctr[15] |= 0x01;
    
    /* Set the key */
    CRYPEx_GCMCCM_SetKey(hcryp, hcryp->Init.pKey, hcryp->Init.KeySize);
    
    /* Set the CRYP peripheral in AES CCM mode */
    __HAL_CRYP_SET_MODE(CRYP_CR_ALGOMODE_AES_CCM_DECRYPT);
    
    /* Set the Initialization Vector */
    CRYPEx_GCMCCM_SetInitVector(hcryp, ctr, CRYP_KEYSIZE_128B);
    
    /* Select init phase */
    __HAL_CRYP_SET_PHASE(CRYP_PHASE_INIT);
    
    b0addr = (uint32_t)blockb0;
    /* Write the blockb0 block in the IN FIFO */
    CRYP->DR = *(uint32_t*)(b0addr);
    b0addr+=4;
    CRYP->DR = *(uint32_t*)(b0addr);
    b0addr+=4;
    CRYP->DR = *(uint32_t*)(b0addr);
    b0addr+=4;
    CRYP->DR = *(uint32_t*)(b0addr);
    
    /* Enable the CRYP peripheral */
    __HAL_CRYP_ENABLE();
    
    /* Get timeout */
    timeout = HAL_GetTick() + Timeout;
 
    while((CRYP->CR & CRYP_CR_CRYPEN) == CRYP_CR_CRYPEN)
    {
      /* Check for the Timeout */
      if(Timeout != HAL_MAX_DELAY)
      {
        if(HAL_GetTick() >= timeout)
        {
          /* Change state */
          hcryp->State = HAL_CRYP_STATE_TIMEOUT;
          
          /* Process Unlocked */          
          __HAL_UNLOCK(hcryp);
        
          return HAL_TIMEOUT;
        }
      }
    }
    /***************************** Header phase *******************************/
    if(headersize != 0)
    {
      /* Select header phase */
      __HAL_CRYP_SET_PHASE(CRYP_PHASE_HEADER);
      
      /* Enable Crypto processor */
      __HAL_CRYP_ENABLE();
      
      for(loopcounter = 0; (loopcounter < headersize); loopcounter+=16)
      {
        /* Get timeout */
        timeout = HAL_GetTick() + Timeout;

        while(HAL_IS_BIT_CLR(CRYP->SR, CRYP_FLAG_IFEM))
        {
          /* Check for the Timeout */
          if(Timeout != HAL_MAX_DELAY)
          {
            if(HAL_GetTick() >= timeout)
            {
              /* Change state */
              hcryp->State = HAL_CRYP_STATE_TIMEOUT;
              
              /* Process Unlocked */          
              __HAL_UNLOCK(hcryp);
              
              return HAL_TIMEOUT;
            }
          }
        }
        /* Write the header block in the IN FIFO */
        CRYP->DR = *(uint32_t*)(headeraddr);
        headeraddr+=4;
        CRYP->DR = *(uint32_t*)(headeraddr);
        headeraddr+=4;
        CRYP->DR = *(uint32_t*)(headeraddr);
        headeraddr+=4;
        CRYP->DR = *(uint32_t*)(headeraddr);
        headeraddr+=4;
      }
      
      /* Get timeout */
      timeout = HAL_GetTick() + Timeout;

      while((CRYP->SR & CRYP_FLAG_BUSY) == CRYP_FLAG_BUSY)
      {
      /* Check for the Timeout */
        if(Timeout != HAL_MAX_DELAY)
        {
          if(HAL_GetTick() >= timeout)
          {
            /* Change state */
            hcryp->State = HAL_CRYP_STATE_TIMEOUT;
            
            /* Process Unlocked */          
            __HAL_UNLOCK(hcryp);
            
            return HAL_TIMEOUT;
          }
        }
      }
    }
    /* Save formatted counter into the scratch buffer pScratch */
    for(loopcounter = 0; (loopcounter < 16); loopcounter++)
    {
      hcryp->Init.pScratch[loopcounter] = ctr[loopcounter];
    }
    /* Reset bit 0 */
    hcryp->Init.pScratch[15] &= 0xfe;
    /* Select payload phase once the header phase is performed */
    __HAL_CRYP_SET_PHASE(CRYP_PHASE_PAYLOAD);
    
    /* Flush FIFO */
    __HAL_CRYP_FIFO_FLUSH();
    
    /* Enable the CRYP peripheral */
    __HAL_CRYP_ENABLE();
    
    /* Set the phase */
    hcryp->Phase = HAL_CRYP_PHASE_PROCESS;
  }
  
  /* Write Plain Data and Get Cypher Data */
  if(CRYPEx_GCMCCM_ProcessData(hcryp, pCypherData, Size, pPlainData, Timeout) != HAL_OK)
  {
    return HAL_TIMEOUT;
  }
  
  /* Change the CRYP peripheral state */
  hcryp->State = HAL_CRYP_STATE_READY;
  
  /* Process Unlocked */
  __HAL_UNLOCK(hcryp);
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Initializes the CRYP peripheral in AES GCM encryption mode using IT.
  * @param  hcryp: CRYP handle
  * @param  pPlainData: Pointer to the plaintext buffer
  * @param  Size: Length of the plaintext buffer, must be a multiple of 16
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYPEx_AESGCM_Encrypt_IT(CRYP_HandleTypeDef *hcryp, uint8_t *pPlainData, uint16_t Size, uint8_t *pCypherData)
{
  uint32_t timeout = 0;   
  uint32_t inputaddr;
  uint32_t outputaddr;
  
  if(hcryp->State == HAL_CRYP_STATE_READY)
  {
    /* Process Locked */
    __HAL_LOCK(hcryp);
    
    /* Get the buffer addresses and sizes */    
    hcryp->CrypInCount = Size;
    hcryp->pCrypInBuffPtr = pPlainData;
    hcryp->pCrypOutBuffPtr = pCypherData;
    hcryp->CrypOutCount = Size;
    
    /* Change the CRYP peripheral state */
    hcryp->State = HAL_CRYP_STATE_BUSY;
    
    /* Check if initialization phase has already been performed */
    if(hcryp->Phase == HAL_CRYP_PHASE_READY)
    {
      /* Set the key */
      CRYPEx_GCMCCM_SetKey(hcryp, hcryp->Init.pKey, hcryp->Init.KeySize);
      
      /* Set the CRYP peripheral in AES GCM mode */
      __HAL_CRYP_SET_MODE(CRYP_CR_ALGOMODE_AES_GCM_ENCRYPT);
      
      /* Set the Initialization Vector */
      CRYPEx_GCMCCM_SetInitVector(hcryp, hcryp->Init.pInitVect, CRYP_KEYSIZE_128B);
      
      /* Flush FIFO */
      __HAL_CRYP_FIFO_FLUSH();
      
      /* Enable CRYP to start the init phase */
      __HAL_CRYP_ENABLE();
      
      /* Get timeout */
      timeout = HAL_GetTick() + 1;

      while((CRYP->CR & CRYP_CR_CRYPEN) == CRYP_CR_CRYPEN)
      {
        /* Check for the Timeout */
        
        if(HAL_GetTick() >= timeout)
        {
          /* Change state */
          hcryp->State = HAL_CRYP_STATE_TIMEOUT;
          
          /* Process Unlocked */          
          __HAL_UNLOCK(hcryp);
          
          return HAL_TIMEOUT;
          
        }
      }
      
      /* Set the header phase */
      if(CRYPEx_GCMCCM_SetHeaderPhase(hcryp, hcryp->Init.Header, hcryp->Init.HeaderSize, 1) != HAL_OK)
      {
        return HAL_TIMEOUT;
      }
      /* Disable the CRYP peripheral */
      __HAL_CRYP_DISABLE();
      
      /* Select payload phase once the header phase is performed */
      __HAL_CRYP_SET_PHASE(CRYP_PHASE_PAYLOAD);
      
      /* Flush FIFO */
      __HAL_CRYP_FIFO_FLUSH();
      
      /* Set the phase */
      hcryp->Phase = HAL_CRYP_PHASE_PROCESS;
    }
    
    if(Size != 0)
    {
      /* Enable Interrupts */
      __HAL_CRYP_ENABLE_IT(CRYP_IT_INI | CRYP_IT_OUTI);
      /* Enable the CRYP peripheral */
      __HAL_CRYP_ENABLE();
    }
    else
    {
      /* Process Locked */
      __HAL_UNLOCK(hcryp);
      /* Change the CRYP state and phase */
      hcryp->State = HAL_CRYP_STATE_READY;
    }
    /* Return function status */
    return HAL_OK;
  }
  else if (__HAL_CRYP_GET_IT(CRYP_IT_INI))
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
  else if (__HAL_CRYP_GET_IT(CRYP_IT_OUTI))
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
      /* Change the CRYP peripheral state */
      hcryp->State = HAL_CRYP_STATE_READY;
      /* Call Input transfer complete callback */
      HAL_CRYP_OutCpltCallback(hcryp);
    }
  }
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Initializes the CRYP peripheral in AES CCM encryption mode using interrupt.
  * @param  hcryp: CRYP handle
  * @param  pPlainData: Pointer to the plaintext buffer
  * @param  Size: Length of the plaintext buffer, must be a multiple of 16
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYPEx_AESCCM_Encrypt_IT(CRYP_HandleTypeDef *hcryp, uint8_t *pPlainData, uint16_t Size, uint8_t *pCypherData)
{
  uint32_t timeout = 0;   
  uint32_t inputaddr;
  uint32_t outputaddr;
  
  uint32_t headersize = hcryp->Init.HeaderSize;
  uint32_t headeraddr = (uint32_t)hcryp->Init.Header;
  uint32_t loopcounter = 0;
  uint32_t bufferidx = 0;
  uint8_t blockb0[16] = {0};/* Block B0 */
  uint8_t ctr[16] = {0}; /* Counter */
  uint32_t b0addr = (uint32_t)blockb0;
  
  if(hcryp->State == HAL_CRYP_STATE_READY)
  {
    /* Process Locked */
    __HAL_LOCK(hcryp);
    
    hcryp->CrypInCount = Size;
    hcryp->pCrypInBuffPtr = pPlainData;
    hcryp->pCrypOutBuffPtr = pCypherData;
    hcryp->CrypOutCount = Size;
    
    /* Change the CRYP peripheral state */
    hcryp->State = HAL_CRYP_STATE_BUSY;
    
    /* Check if initialization phase has already been performed */
    if(hcryp->Phase == HAL_CRYP_PHASE_READY)
    {    
      /************************ Formatting the header block *******************/
      if(headersize != 0)
      {
        /* Check that the associated data (or header) length is lower than 2^16 - 2^8 = 65536 - 256 = 65280 */
        if(headersize < 65280)
        {
          hcryp->Init.pScratch[bufferidx++] = (uint8_t) ((headersize >> 8) & 0xFF);
          hcryp->Init.pScratch[bufferidx++] = (uint8_t) ((headersize) & 0xFF);
          headersize += 2;
        }
        else
        {
          /* Header is encoded as 0xff || 0xfe || [headersize]32, i.e., six octets */
          hcryp->Init.pScratch[bufferidx++] = 0xFF;
          hcryp->Init.pScratch[bufferidx++] = 0xFE;
          hcryp->Init.pScratch[bufferidx++] = headersize & 0xff000000;
          hcryp->Init.pScratch[bufferidx++] = headersize & 0x00ff0000;
          hcryp->Init.pScratch[bufferidx++] = headersize & 0x0000ff00;
          hcryp->Init.pScratch[bufferidx++] = headersize & 0x000000ff;
          headersize += 6;
        }
        /* Copy the header buffer in internal buffer "hcryp->Init.pScratch" */
        for(loopcounter = 0; loopcounter < headersize; loopcounter++)
        {
          hcryp->Init.pScratch[bufferidx++] = hcryp->Init.Header[loopcounter];
        }
        /* Check if the header size is modulo 16 */
        if ((headersize % 16) != 0)
        {
          /* Padd the header buffer with 0s till the hcryp->Init.pScratch length is modulo 16 */
          for(loopcounter = headersize; loopcounter <= ((headersize/16) + 1) * 16; loopcounter++)
          {
            hcryp->Init.pScratch[loopcounter] = 0;
          }
          /* Set the header size to modulo 16 */
          headersize = ((headersize/16) + 1) * 16;
        }
        /* Set the pointer headeraddr to hcryp->Init.pScratch */
        headeraddr = (uint32_t)hcryp->Init.pScratch;
      }
      /*********************** Formatting the block B0 ************************/
      if(headersize != 0)
      {
        blockb0[0] = 0x40;
      }
      /* Flags byte */
      /* blockb0[0] |= 0u | (((( (uint8_t) hcryp->Init.TagSize - 2) / 2) & 0x07 ) << 3 ) | ( ( (uint8_t) (15 - hcryp->Init.IVSize) - 1) & 0x07) */
      blockb0[0] |= (uint8_t)((uint8_t)((uint8_t)(((uint8_t)(hcryp->Init.TagSize - (uint8_t)(2))) >> 1) & (uint8_t)0x07 ) << 3);
      blockb0[0] |= (uint8_t)((uint8_t)((uint8_t)((uint8_t)(15) - hcryp->Init.IVSize) - (uint8_t)1) & (uint8_t)0x07);
      
      for (loopcounter = 0; loopcounter < hcryp->Init.IVSize; loopcounter++)
      {
        blockb0[loopcounter+1] = hcryp->Init.pInitVect[loopcounter];
      }
      for ( ; loopcounter < 13; loopcounter++)
      {
        blockb0[loopcounter+1] = 0;
      }
      
      blockb0[14] = (Size >> 8);
      blockb0[15] = (Size & 0xFF);
      
      /************************* Formatting the initial counter ***************/
      /* Byte 0:
         Bits 7 and 6 are reserved and shall be set to 0
         Bits 3, 4, and 5 shall also be set to 0, to ensure that all the counter 
         blocks are distinct from B0
         Bits 0, 1, and 2 contain the same encoding of q as in B0
      */
      ctr[0] = blockb0[0] & 0x07;
      /* byte 1 to NonceSize is the IV (Nonce) */
      for(loopcounter = 1; loopcounter < hcryp->Init.IVSize + 1; loopcounter++)
      {
        ctr[loopcounter] = blockb0[loopcounter];
      }
      /* Set the LSB to 1 */
      ctr[15] |= 0x01;
      
      /* Set the key */
      CRYPEx_GCMCCM_SetKey(hcryp, hcryp->Init.pKey, hcryp->Init.KeySize);
      
      /* Set the CRYP peripheral in AES CCM mode */
      __HAL_CRYP_SET_MODE(CRYP_CR_ALGOMODE_AES_CCM_ENCRYPT);
      
      /* Set the Initialization Vector */
      CRYPEx_GCMCCM_SetInitVector(hcryp, ctr, hcryp->Init.KeySize);
      
      /* Select init phase */
      __HAL_CRYP_SET_PHASE(CRYP_PHASE_INIT);
      
      b0addr = (uint32_t)blockb0;
      /* Write the blockb0 block in the IN FIFO */
      CRYP->DR = *(uint32_t*)(b0addr);
      b0addr+=4;
      CRYP->DR = *(uint32_t*)(b0addr);
      b0addr+=4;
      CRYP->DR = *(uint32_t*)(b0addr);
      b0addr+=4;
      CRYP->DR = *(uint32_t*)(b0addr);
      
      /* Enable the CRYP peripheral */
      __HAL_CRYP_ENABLE();
      
      /* Get timeout */
      timeout = HAL_GetTick() + 1;

      while((CRYP->CR & CRYP_CR_CRYPEN) == CRYP_CR_CRYPEN)
      {
        /* Check for the Timeout */
        if(HAL_GetTick() >= timeout)
        {
          /* Change state */
          hcryp->State = HAL_CRYP_STATE_TIMEOUT;
          
          /* Process Unlocked */          
          __HAL_UNLOCK(hcryp);
          
          return HAL_TIMEOUT;
        }
      }
      /***************************** Header phase *****************************/
      if(headersize != 0)
      {
        /* Select header phase */
        __HAL_CRYP_SET_PHASE(CRYP_PHASE_HEADER);
        
        /* Enable Crypto processor */
        __HAL_CRYP_ENABLE();
        
        for(loopcounter = 0; (loopcounter < headersize); loopcounter+=16)
        {
          /* Get timeout */
          timeout = HAL_GetTick() + 1;

          while(HAL_IS_BIT_CLR(CRYP->SR, CRYP_FLAG_IFEM))
          {
            /* Check for the Timeout */
            if(HAL_GetTick() >= timeout)
            {
              /* Change state */
              hcryp->State = HAL_CRYP_STATE_TIMEOUT;
              
              /* Process Unlocked */          
              __HAL_UNLOCK(hcryp);
              
              return HAL_TIMEOUT;
            }
          }
          /* Write the header block in the IN FIFO */
          CRYP->DR = *(uint32_t*)(headeraddr);
          headeraddr+=4;
          CRYP->DR = *(uint32_t*)(headeraddr);
          headeraddr+=4;
          CRYP->DR = *(uint32_t*)(headeraddr);
          headeraddr+=4;
          CRYP->DR = *(uint32_t*)(headeraddr);
          headeraddr+=4;
        }
        
        /* Get timeout */
        timeout = HAL_GetTick() + 1;

        while((CRYP->SR & CRYP_FLAG_BUSY) == CRYP_FLAG_BUSY)
        {
          /* Check for the Timeout */
          if(HAL_GetTick() >= timeout)
          {
            /* Change state */
            hcryp->State = HAL_CRYP_STATE_TIMEOUT;
            
            /* Process Unlocked */          
            __HAL_UNLOCK(hcryp);
            
            return HAL_TIMEOUT;
          }
        }
      }
      /* Save formatted counter into the scratch buffer pScratch */
      for(loopcounter = 0; (loopcounter < 16); loopcounter++)
      {
        hcryp->Init.pScratch[loopcounter] = ctr[loopcounter];
      }
      /* Reset bit 0 */
      hcryp->Init.pScratch[15] &= 0xfe;
      
      /* Select payload phase once the header phase is performed */
      __HAL_CRYP_SET_PHASE(CRYP_PHASE_PAYLOAD);
      
      /* Flush FIFO */
      __HAL_CRYP_FIFO_FLUSH();
      
      /* Set the phase */
      hcryp->Phase = HAL_CRYP_PHASE_PROCESS;
    }
    
    if(Size != 0)
    {
      /* Enable Interrupts */
      __HAL_CRYP_ENABLE_IT(CRYP_IT_INI | CRYP_IT_OUTI);
      /* Enable the CRYP peripheral */
      __HAL_CRYP_ENABLE();
    }
    else
    {
      /* Change the CRYP state and phase */
      hcryp->State = HAL_CRYP_STATE_READY;
    }
    
    /* Return function status */
    return HAL_OK;
  }
  else if (__HAL_CRYP_GET_IT(CRYP_IT_INI))
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
      /* Call Input transfer complete callback */
      HAL_CRYP_InCpltCallback(hcryp);
    }
  }
  else if (__HAL_CRYP_GET_IT(CRYP_IT_OUTI))
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
      /* Change the CRYP peripheral state */
      hcryp->State = HAL_CRYP_STATE_READY;
      /* Call Input transfer complete callback */
      HAL_CRYP_OutCpltCallback(hcryp);
    }
  }
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Initializes the CRYP peripheral in AES GCM decryption mode using IT.
  * @param  hcryp: CRYP handle
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @param  Size: Length of the cyphertext buffer, must be a multiple of 16
  * @param  pPlainData: Pointer to the plaintext buffer
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYPEx_AESGCM_Decrypt_IT(CRYP_HandleTypeDef *hcryp, uint8_t *pCypherData, uint16_t Size, uint8_t *pPlainData)
{
  uint32_t timeout = 0;   
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
    
    /* Change the CRYP peripheral state */
    hcryp->State = HAL_CRYP_STATE_BUSY;
    
    /* Check if initialization phase has already been performed */
    if(hcryp->Phase == HAL_CRYP_PHASE_READY)
    {
      /* Set the key */
      CRYPEx_GCMCCM_SetKey(hcryp, hcryp->Init.pKey, hcryp->Init.KeySize);
      
      /* Set the CRYP peripheral in AES GCM decryption mode */
      __HAL_CRYP_SET_MODE(CRYP_CR_ALGOMODE_AES_GCM_DECRYPT);
      
      /* Set the Initialization Vector */
      CRYPEx_GCMCCM_SetInitVector(hcryp, hcryp->Init.pInitVect, CRYP_KEYSIZE_128B);
      
      /* Flush FIFO */
      __HAL_CRYP_FIFO_FLUSH();
      
      /* Enable CRYP to start the init phase */
      __HAL_CRYP_ENABLE();
      
      /* Get timeout */
      timeout = HAL_GetTick() + 1;
      
      while((CRYP->CR & CRYP_CR_CRYPEN) == CRYP_CR_CRYPEN)
      {
        /* Check for the Timeout */
        if(HAL_GetTick() >= timeout)
        {
          /* Change state */
          hcryp->State = HAL_CRYP_STATE_TIMEOUT;
          
          /* Process Unlocked */          
          __HAL_UNLOCK(hcryp);
          
          return HAL_TIMEOUT;
        }
      }
      
      /* Set the header phase */
      if(CRYPEx_GCMCCM_SetHeaderPhase(hcryp, hcryp->Init.Header, hcryp->Init.HeaderSize, 1) != HAL_OK)
      {
        return HAL_TIMEOUT;
      }
      /* Disable the CRYP peripheral */
      __HAL_CRYP_DISABLE();
      
      /* Select payload phase once the header phase is performed */
      __HAL_CRYP_SET_PHASE(CRYP_PHASE_PAYLOAD);
      
      /* Set the phase */
      hcryp->Phase = HAL_CRYP_PHASE_PROCESS;
    }
    
    if(Size != 0)
    {
      /* Enable Interrupts */
      __HAL_CRYP_ENABLE_IT(CRYP_IT_INI | CRYP_IT_OUTI);
      /* Enable the CRYP peripheral */
      __HAL_CRYP_ENABLE();
    }
    else
    {
      /* Process Locked */
      __HAL_UNLOCK(hcryp);
      /* Change the CRYP state and phase */
      hcryp->State = HAL_CRYP_STATE_READY;
    }
    
    /* Return function status */
    return HAL_OK;
  }
  else if (__HAL_CRYP_GET_IT(CRYP_IT_INI))
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
  else if (__HAL_CRYP_GET_IT(CRYP_IT_OUTI))
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
      /* Change the CRYP peripheral state */
      hcryp->State = HAL_CRYP_STATE_READY;
      /* Call Input transfer complete callback */
      HAL_CRYP_OutCpltCallback(hcryp);
    }
  }
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Initializes the CRYP peripheral in AES CCM decryption mode using interrupt
  *         then decrypted pCypherData. The cypher data are available in pPlainData.
  * @param  hcryp: CRYP handle
  * @param  pCypherData: Pointer to the cyphertext buffer 
  * @param  Size: Length of the plaintext buffer, must be a multiple of 16
  * @param  pPlainData: Pointer to the plaintext buffer  
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYPEx_AESCCM_Decrypt_IT(CRYP_HandleTypeDef *hcryp, uint8_t *pCypherData, uint16_t Size, uint8_t *pPlainData)
{
  uint32_t inputaddr;
  uint32_t outputaddr;
  uint32_t timeout = 0;
  uint32_t headersize = hcryp->Init.HeaderSize;
  uint32_t headeraddr = (uint32_t)hcryp->Init.Header;
  uint32_t loopcounter = 0;
  uint32_t bufferidx = 0;
  uint8_t blockb0[16] = {0};/* Block B0 */
  uint8_t ctr[16] = {0}; /* Counter */
  uint32_t b0addr = (uint32_t)blockb0;
  
  if(hcryp->State == HAL_CRYP_STATE_READY)
  {
    /* Process Locked */
    __HAL_LOCK(hcryp);
    
    hcryp->CrypInCount = Size;
    hcryp->pCrypInBuffPtr = pCypherData;
    hcryp->pCrypOutBuffPtr = pPlainData;
    hcryp->CrypOutCount = Size;
    
    /* Change the CRYP peripheral state */
    hcryp->State = HAL_CRYP_STATE_BUSY;
    
    /* Check if initialization phase has already been performed */
    if(hcryp->Phase == HAL_CRYP_PHASE_READY)
    {
      /************************ Formatting the header block *******************/
      if(headersize != 0)
      {
        /* Check that the associated data (or header) length is lower than 2^16 - 2^8 = 65536 - 256 = 65280 */
        if(headersize < 65280)
        {
          hcryp->Init.pScratch[bufferidx++] = (uint8_t) ((headersize >> 8) & 0xFF);
          hcryp->Init.pScratch[bufferidx++] = (uint8_t) ((headersize) & 0xFF);
          headersize += 2;
        }
        else
        {
          /* Header is encoded as 0xff || 0xfe || [headersize]32, i.e., six octets */
          hcryp->Init.pScratch[bufferidx++] = 0xFF;
          hcryp->Init.pScratch[bufferidx++] = 0xFE;
          hcryp->Init.pScratch[bufferidx++] = headersize & 0xff000000;
          hcryp->Init.pScratch[bufferidx++] = headersize & 0x00ff0000;
          hcryp->Init.pScratch[bufferidx++] = headersize & 0x0000ff00;
          hcryp->Init.pScratch[bufferidx++] = headersize & 0x000000ff;
          headersize += 6;
        }
        /* Copy the header buffer in internal buffer "hcryp->Init.pScratch" */
        for(loopcounter = 0; loopcounter < headersize; loopcounter++)
        {
          hcryp->Init.pScratch[bufferidx++] = hcryp->Init.Header[loopcounter];
        }
        /* Check if the header size is modulo 16 */
        if ((headersize % 16) != 0)
        {
          /* Padd the header buffer with 0s till the hcryp->Init.pScratch length is modulo 16 */
          for(loopcounter = headersize; loopcounter <= ((headersize/16) + 1) * 16; loopcounter++)
          {
            hcryp->Init.pScratch[loopcounter] = 0;
          }
          /* Set the header size to modulo 16 */
          headersize = ((headersize/16) + 1) * 16;
        }
        /* Set the pointer headeraddr to hcryp->Init.pScratch */
        headeraddr = (uint32_t)hcryp->Init.pScratch;
      }
      /*********************** Formatting the block B0 ************************/
      if(headersize != 0)
      {
        blockb0[0] = 0x40;
      }
      /* Flags byte */
      /* blockb0[0] |= 0u | (((( (uint8_t) hcryp->Init.TagSize - 2) / 2) & 0x07 ) << 3 ) | ( ( (uint8_t) (15 - hcryp->Init.IVSize) - 1) & 0x07) */
      blockb0[0] |= (uint8_t)((uint8_t)((uint8_t)(((uint8_t)(hcryp->Init.TagSize - (uint8_t)(2))) >> 1) & (uint8_t)0x07 ) << 3);
      blockb0[0] |= (uint8_t)((uint8_t)((uint8_t)((uint8_t)(15) - hcryp->Init.IVSize) - (uint8_t)1) & (uint8_t)0x07);
      
      for (loopcounter = 0; loopcounter < hcryp->Init.IVSize; loopcounter++)
      {
        blockb0[loopcounter+1] = hcryp->Init.pInitVect[loopcounter];
      }
      for ( ; loopcounter < 13; loopcounter++)
      {
        blockb0[loopcounter+1] = 0;
      }
      
      blockb0[14] = (Size >> 8);
      blockb0[15] = (Size & 0xFF);
      
      /************************* Formatting the initial counter ***************/
      /* Byte 0:
         Bits 7 and 6 are reserved and shall be set to 0
         Bits 3, 4, and 5 shall also be set to 0, to ensure that all the counter 
         blocks are distinct from B0
         Bits 0, 1, and 2 contain the same encoding of q as in B0
      */
      ctr[0] = blockb0[0] & 0x07;
      /* byte 1 to NonceSize is the IV (Nonce) */
      for(loopcounter = 1; loopcounter < hcryp->Init.IVSize + 1; loopcounter++)
      {
        ctr[loopcounter] = blockb0[loopcounter];
      }
      /* Set the LSB to 1 */
      ctr[15] |= 0x01;
      
      /* Set the key */
      CRYPEx_GCMCCM_SetKey(hcryp, hcryp->Init.pKey, hcryp->Init.KeySize);
      
      /* Set the CRYP peripheral in AES CCM mode */
      __HAL_CRYP_SET_MODE(CRYP_CR_ALGOMODE_AES_CCM_DECRYPT);
      
      /* Set the Initialization Vector */
      CRYPEx_GCMCCM_SetInitVector(hcryp, ctr, hcryp->Init.KeySize);
      
      /* Select init phase */
      __HAL_CRYP_SET_PHASE(CRYP_PHASE_INIT);
      
      b0addr = (uint32_t)blockb0;
      /* Write the blockb0 block in the IN FIFO */
      CRYP->DR = *(uint32_t*)(b0addr);
      b0addr+=4;
      CRYP->DR = *(uint32_t*)(b0addr);
      b0addr+=4;
      CRYP->DR = *(uint32_t*)(b0addr);
      b0addr+=4;
      CRYP->DR = *(uint32_t*)(b0addr);
      
      /* Enable the CRYP peripheral */
      __HAL_CRYP_ENABLE();
      
      /* Get timeout */
      timeout = HAL_GetTick() + 1;

      while((CRYP->CR & CRYP_CR_CRYPEN) == CRYP_CR_CRYPEN)
      {
        /* Check for the Timeout */
        if(HAL_GetTick() >= timeout)
        {
          /* Change state */
          hcryp->State = HAL_CRYP_STATE_TIMEOUT;
          
          /* Process Unlocked */          
          __HAL_UNLOCK(hcryp);
          
          return HAL_TIMEOUT;
        }
      }
      /***************************** Header phase *****************************/
      if(headersize != 0)
      {
        /* Select header phase */
        __HAL_CRYP_SET_PHASE(CRYP_PHASE_HEADER);
        
        /* Enable Crypto processor */
        __HAL_CRYP_ENABLE();
        
        for(loopcounter = 0; (loopcounter < headersize); loopcounter+=16)
        {
          /* Get timeout */
          timeout = HAL_GetTick() + 1;

          while(HAL_IS_BIT_CLR(CRYP->SR, CRYP_FLAG_IFEM))
          {
            /* Check for the Timeout */
            if(HAL_GetTick() >= timeout)
            {
              /* Change state */
              hcryp->State = HAL_CRYP_STATE_TIMEOUT;
              
              /* Process Unlocked */          
              __HAL_UNLOCK(hcryp);
              
              return HAL_TIMEOUT;
            }
          }
          /* Write the header block in the IN FIFO */
          CRYP->DR = *(uint32_t*)(headeraddr);
          headeraddr+=4;
          CRYP->DR = *(uint32_t*)(headeraddr);
          headeraddr+=4;
          CRYP->DR = *(uint32_t*)(headeraddr);
          headeraddr+=4;
          CRYP->DR = *(uint32_t*)(headeraddr);
          headeraddr+=4;
        }
        
        /* Get timeout */
        timeout = HAL_GetTick() + 1;

        while((CRYP->SR & CRYP_FLAG_BUSY) == CRYP_FLAG_BUSY)
        {
          /* Check for the Timeout */
          if(HAL_GetTick() >= timeout)
          {
            /* Change state */
            hcryp->State = HAL_CRYP_STATE_TIMEOUT;
            
            /* Process Unlocked */          
            __HAL_UNLOCK(hcryp);
            
            return HAL_TIMEOUT;
          }
        }
      }
      /* Save formatted counter into the scratch buffer pScratch */
      for(loopcounter = 0; (loopcounter < 16); loopcounter++)
      {
        hcryp->Init.pScratch[loopcounter] = ctr[loopcounter];
      }
      /* Reset bit 0 */
      hcryp->Init.pScratch[15] &= 0xfe;
      /* Select payload phase once the header phase is performed */
      __HAL_CRYP_SET_PHASE(CRYP_PHASE_PAYLOAD);
      
      /* Flush FIFO */
      __HAL_CRYP_FIFO_FLUSH();
      
      /* Set the phase */
      hcryp->Phase = HAL_CRYP_PHASE_PROCESS;
    }
    
    /* Enable Interrupts */
    __HAL_CRYP_ENABLE_IT(CRYP_IT_INI | CRYP_IT_OUTI);
    
    /* Enable the CRYP peripheral */
    __HAL_CRYP_ENABLE();
    
    /* Return function status */
    return HAL_OK;
  }
  else if (__HAL_CRYP_GET_IT(CRYP_IT_INI))
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
  else if (__HAL_CRYP_GET_IT(CRYP_IT_OUTI))
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
      /* Change the CRYP peripheral state */
      hcryp->State = HAL_CRYP_STATE_READY;
      /* Call Input transfer complete callback */
      HAL_CRYP_OutCpltCallback(hcryp);
    }
  }
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Initializes the CRYP peripheral in AES GCM encryption mode using DMA.
  * @param  hcryp: CRYP handle
  * @param  pPlainData: Pointer to the plaintext buffer
  * @param  Size: Length of the plaintext buffer, must be a multiple of 16
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYPEx_AESGCM_Encrypt_DMA(CRYP_HandleTypeDef *hcryp, uint8_t *pPlainData, uint16_t Size, uint8_t *pCypherData)
{
  uint32_t timeout = 0;
  uint32_t inputaddr;
  uint32_t outputaddr;
  
  if((hcryp->State == HAL_CRYP_STATE_READY) || (hcryp->Phase == HAL_CRYP_PHASE_PROCESS))
  {
    /* Process Locked */
    __HAL_LOCK(hcryp);
    
    inputaddr  = (uint32_t)pPlainData;
    outputaddr = (uint32_t)pCypherData;
    
    /* Change the CRYP peripheral state */
    hcryp->State = HAL_CRYP_STATE_BUSY;
    
    /* Check if initialization phase has already been performed */
    if(hcryp->Phase == HAL_CRYP_PHASE_READY)
    {
      /* Set the key */
      CRYPEx_GCMCCM_SetKey(hcryp, hcryp->Init.pKey, hcryp->Init.KeySize);
      
      /* Set the CRYP peripheral in AES GCM mode */
      __HAL_CRYP_SET_MODE(CRYP_CR_ALGOMODE_AES_GCM_ENCRYPT);
      
      /* Set the Initialization Vector */
      CRYPEx_GCMCCM_SetInitVector(hcryp, hcryp->Init.pInitVect, CRYP_KEYSIZE_128B);
      
      /* Flush FIFO */
      __HAL_CRYP_FIFO_FLUSH();
      
      /* Enable CRYP to start the init phase */
      __HAL_CRYP_ENABLE();
      
      /* Get timeout */
      timeout = HAL_GetTick() + 1;

      while((CRYP->CR & CRYP_CR_CRYPEN) == CRYP_CR_CRYPEN)
      {
        /* Check for the Timeout */
        if(HAL_GetTick() >= timeout)
        {
          /* Change state */
          hcryp->State = HAL_CRYP_STATE_TIMEOUT;
          
          /* Process Unlocked */          
          __HAL_UNLOCK(hcryp);
          
          return HAL_TIMEOUT;
        }
      }
      /* Flush FIFO */
      __HAL_CRYP_FIFO_FLUSH();
      
      /* Set the header phase */
      if(CRYPEx_GCMCCM_SetHeaderPhase(hcryp, hcryp->Init.Header, hcryp->Init.HeaderSize, 1) != HAL_OK)
      {
        return HAL_TIMEOUT;
      }
      /* Disable the CRYP peripheral */
      __HAL_CRYP_DISABLE();
      
      /* Select payload phase once the header phase is performed */
      __HAL_CRYP_SET_PHASE(CRYP_PHASE_PAYLOAD);
      
      /* Flush FIFO */
      __HAL_CRYP_FIFO_FLUSH();
      
      /* Set the phase */
      hcryp->Phase = HAL_CRYP_PHASE_PROCESS;
    }
    
    /* Set the input and output addresses and start DMA transfer */ 
    CRYPEx_GCMCCM_SetDMAConfig(hcryp, inputaddr, Size, outputaddr);
    
    /* Unlock process */
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
  * @brief  Initializes the CRYP peripheral in AES CCM encryption mode using interrupt.
  * @param  hcryp: CRYP handle
  * @param  pPlainData: Pointer to the plaintext buffer
  * @param  Size: Length of the plaintext buffer, must be a multiple of 16
  * @param  pCypherData: Pointer to the cyphertext buffer
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYPEx_AESCCM_Encrypt_DMA(CRYP_HandleTypeDef *hcryp, uint8_t *pPlainData, uint16_t Size, uint8_t *pCypherData)
{
  uint32_t timeout = 0;   
  uint32_t inputaddr;
  uint32_t outputaddr;
  uint32_t headersize;
  uint32_t headeraddr;
  uint32_t loopcounter = 0;
  uint32_t bufferidx = 0;
  uint8_t blockb0[16] = {0};/* Block B0 */
  uint8_t ctr[16] = {0}; /* Counter */
  uint32_t b0addr = (uint32_t)blockb0;
  
  if((hcryp->State == HAL_CRYP_STATE_READY) || (hcryp->Phase == HAL_CRYP_PHASE_PROCESS))
  {
    /* Process Locked */
    __HAL_LOCK(hcryp);
    
    inputaddr  = (uint32_t)pPlainData;
    outputaddr = (uint32_t)pCypherData;
    
    headersize = hcryp->Init.HeaderSize;
    headeraddr = (uint32_t)hcryp->Init.Header;
    
    hcryp->CrypInCount = Size;
    hcryp->pCrypInBuffPtr = pPlainData;
    hcryp->pCrypOutBuffPtr = pCypherData;
    hcryp->CrypOutCount = Size;
    
    /* Change the CRYP peripheral state */
    hcryp->State = HAL_CRYP_STATE_BUSY;
    
    /* Check if initialization phase has already been performed */
    if(hcryp->Phase == HAL_CRYP_PHASE_READY)
    {
      /************************ Formatting the header block *******************/
      if(headersize != 0)
      {
        /* Check that the associated data (or header) length is lower than 2^16 - 2^8 = 65536 - 256 = 65280 */
        if(headersize < 65280)
        {
          hcryp->Init.pScratch[bufferidx++] = (uint8_t) ((headersize >> 8) & 0xFF);
          hcryp->Init.pScratch[bufferidx++] = (uint8_t) ((headersize) & 0xFF);
          headersize += 2;
        }
        else
        {
          /* Header is encoded as 0xff || 0xfe || [headersize]32, i.e., six octets */
          hcryp->Init.pScratch[bufferidx++] = 0xFF;
          hcryp->Init.pScratch[bufferidx++] = 0xFE;
          hcryp->Init.pScratch[bufferidx++] = headersize & 0xff000000;
          hcryp->Init.pScratch[bufferidx++] = headersize & 0x00ff0000;
          hcryp->Init.pScratch[bufferidx++] = headersize & 0x0000ff00;
          hcryp->Init.pScratch[bufferidx++] = headersize & 0x000000ff;
          headersize += 6;
        }
        /* Copy the header buffer in internal buffer "hcryp->Init.pScratch" */
        for(loopcounter = 0; loopcounter < headersize; loopcounter++)
        {
          hcryp->Init.pScratch[bufferidx++] = hcryp->Init.Header[loopcounter];
        }
        /* Check if the header size is modulo 16 */
        if ((headersize % 16) != 0)
        {
          /* Padd the header buffer with 0s till the hcryp->Init.pScratch length is modulo 16 */
          for(loopcounter = headersize; loopcounter <= ((headersize/16) + 1) * 16; loopcounter++)
          {
            hcryp->Init.pScratch[loopcounter] = 0;
          }
          /* Set the header size to modulo 16 */
          headersize = ((headersize/16) + 1) * 16;
        }
        /* Set the pointer headeraddr to hcryp->Init.pScratch */
        headeraddr = (uint32_t)hcryp->Init.pScratch;
      }
      /*********************** Formatting the block B0 ************************/
      if(headersize != 0)
      {
        blockb0[0] = 0x40;
      }
      /* Flags byte */
      /* blockb0[0] |= 0u | (((( (uint8_t) hcryp->Init.TagSize - 2) / 2) & 0x07 ) << 3 ) | ( ( (uint8_t) (15 - hcryp->Init.IVSize) - 1) & 0x07) */
      blockb0[0] |= (uint8_t)((uint8_t)((uint8_t)(((uint8_t)(hcryp->Init.TagSize - (uint8_t)(2))) >> 1) & (uint8_t)0x07 ) << 3);
      blockb0[0] |= (uint8_t)((uint8_t)((uint8_t)((uint8_t)(15) - hcryp->Init.IVSize) - (uint8_t)1) & (uint8_t)0x07);
      
      for (loopcounter = 0; loopcounter < hcryp->Init.IVSize; loopcounter++)
      {
        blockb0[loopcounter+1] = hcryp->Init.pInitVect[loopcounter];
      }
      for ( ; loopcounter < 13; loopcounter++)
      {
        blockb0[loopcounter+1] = 0;
      }
      
      blockb0[14] = (Size >> 8);
      blockb0[15] = (Size & 0xFF);
      
      /************************* Formatting the initial counter ***************/
      /* Byte 0:
         Bits 7 and 6 are reserved and shall be set to 0
         Bits 3, 4, and 5 shall also be set to 0, to ensure that all the counter 
         blocks are distinct from B0
         Bits 0, 1, and 2 contain the same encoding of q as in B0
      */
      ctr[0] = blockb0[0] & 0x07;
      /* byte 1 to NonceSize is the IV (Nonce) */
      for(loopcounter = 1; loopcounter < hcryp->Init.IVSize + 1; loopcounter++)
      {
        ctr[loopcounter] = blockb0[loopcounter];
      }
      /* Set the LSB to 1 */
      ctr[15] |= 0x01;
      
      /* Set the key */
      CRYPEx_GCMCCM_SetKey(hcryp, hcryp->Init.pKey, hcryp->Init.KeySize);
      
      /* Set the CRYP peripheral in AES CCM mode */
      __HAL_CRYP_SET_MODE(CRYP_CR_ALGOMODE_AES_CCM_ENCRYPT);
      
      /* Set the Initialization Vector */
      CRYPEx_GCMCCM_SetInitVector(hcryp, ctr, CRYP_KEYSIZE_128B);
      
      /* Select init phase */
      __HAL_CRYP_SET_PHASE(CRYP_PHASE_INIT);
      
      b0addr = (uint32_t)blockb0;
      /* Write the blockb0 block in the IN FIFO */
      CRYP->DR = *(uint32_t*)(b0addr);
      b0addr+=4;
      CRYP->DR = *(uint32_t*)(b0addr);
      b0addr+=4;
      CRYP->DR = *(uint32_t*)(b0addr);
      b0addr+=4;
      CRYP->DR = *(uint32_t*)(b0addr);
      
      /* Enable the CRYP peripheral */
      __HAL_CRYP_ENABLE();
      
      /* Get timeout */
      timeout = HAL_GetTick() + 1;
 
      while((CRYP->CR & CRYP_CR_CRYPEN) == CRYP_CR_CRYPEN)
      {
        /* Check for the Timeout */
        if(HAL_GetTick() >= timeout)
        {
          /* Change state */
          hcryp->State = HAL_CRYP_STATE_TIMEOUT;
          
          /* Process Unlocked */          
          __HAL_UNLOCK(hcryp);
          
          return HAL_TIMEOUT;
        }
      }
      /***************************** Header phase *****************************/
      if(headersize != 0)
      {
        /* Select header phase */
        __HAL_CRYP_SET_PHASE(CRYP_PHASE_HEADER);
        
        /* Enable Crypto processor */
        __HAL_CRYP_ENABLE();
        
        for(loopcounter = 0; (loopcounter < headersize); loopcounter+=16)
        {
          /* Get timeout */
          timeout = HAL_GetTick() + 1;
          
          while(HAL_IS_BIT_CLR(CRYP->SR, CRYP_FLAG_IFEM))
          {
            /* Check for the Timeout */
            if(HAL_GetTick() >= timeout)
            {
              /* Change state */
              hcryp->State = HAL_CRYP_STATE_TIMEOUT;
              
              /* Process Unlocked */          
              __HAL_UNLOCK(hcryp);
              
              return HAL_TIMEOUT;
            }
          }
          /* Write the header block in the IN FIFO */
          CRYP->DR = *(uint32_t*)(headeraddr);
          headeraddr+=4;
          CRYP->DR = *(uint32_t*)(headeraddr);
          headeraddr+=4;
          CRYP->DR = *(uint32_t*)(headeraddr);
          headeraddr+=4;
          CRYP->DR = *(uint32_t*)(headeraddr);
          headeraddr+=4;
        }
        
        /* Get timeout */
        timeout = HAL_GetTick() + 1;

        while((CRYP->SR & CRYP_FLAG_BUSY) == CRYP_FLAG_BUSY)
        {
          /* Check for the Timeout */
          if(HAL_GetTick() >= timeout)
          {
            /* Change state */
            hcryp->State = HAL_CRYP_STATE_TIMEOUT;
            
            /* Process Unlocked */          
            __HAL_UNLOCK(hcryp);
            
            return HAL_TIMEOUT;
          }
        }
      }
      /* Save formatted counter into the scratch buffer pScratch */
      for(loopcounter = 0; (loopcounter < 16); loopcounter++)
      {
        hcryp->Init.pScratch[loopcounter] = ctr[loopcounter];
      }
      /* Reset bit 0 */
      hcryp->Init.pScratch[15] &= 0xfe;
      
      /* Select payload phase once the header phase is performed */
      __HAL_CRYP_SET_PHASE(CRYP_PHASE_PAYLOAD);
      
      /* Flush FIFO */
      __HAL_CRYP_FIFO_FLUSH();
      
      /* Set the phase */
      hcryp->Phase = HAL_CRYP_PHASE_PROCESS;
    }
    
    /* Set the input and output addresses and start DMA transfer */ 
    CRYPEx_GCMCCM_SetDMAConfig(hcryp, inputaddr, Size, outputaddr);
    
    /* Unlock process */
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
  * @brief  Initializes the CRYP peripheral in AES GCM decryption mode using DMA.
  * @param  hcryp: CRYP handle
  * @param  pCypherData: Pointer to the cyphertext buffer.
  * @param  Size: Length of the cyphertext buffer, must be a multiple of 16
  * @param  pPlainData: Pointer to the plaintext buffer
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYPEx_AESGCM_Decrypt_DMA(CRYP_HandleTypeDef *hcryp, uint8_t *pCypherData, uint16_t Size, uint8_t *pPlainData)
{
  uint32_t timeout = 0;   
  uint32_t inputaddr;
  uint32_t outputaddr;
  
  if((hcryp->State == HAL_CRYP_STATE_READY) || (hcryp->Phase == HAL_CRYP_PHASE_PROCESS))
  {
    /* Process Locked */
    __HAL_LOCK(hcryp);
    
    inputaddr  = (uint32_t)pCypherData;
    outputaddr = (uint32_t)pPlainData;
    
    /* Change the CRYP peripheral state */
    hcryp->State = HAL_CRYP_STATE_BUSY;
    
    /* Check if initialization phase has already been performed */
    if(hcryp->Phase == HAL_CRYP_PHASE_READY)
    {
      /* Set the key */
      CRYPEx_GCMCCM_SetKey(hcryp, hcryp->Init.pKey, hcryp->Init.KeySize);
      
      /* Set the CRYP peripheral in AES GCM decryption mode */
      __HAL_CRYP_SET_MODE(CRYP_CR_ALGOMODE_AES_GCM_DECRYPT);
      
      /* Set the Initialization Vector */
      CRYPEx_GCMCCM_SetInitVector(hcryp, hcryp->Init.pInitVect, CRYP_KEYSIZE_128B);
      
      /* Enable CRYP to start the init phase */
      __HAL_CRYP_ENABLE();
      
      /* Get timeout */
      timeout = HAL_GetTick() + 1;

      while((CRYP->CR & CRYP_CR_CRYPEN) == CRYP_CR_CRYPEN)
      {
        /* Check for the Timeout */
        if(HAL_GetTick() >= timeout)
        {
          /* Change state */
          hcryp->State = HAL_CRYP_STATE_TIMEOUT;
          
          /* Process Unlocked */          
          __HAL_UNLOCK(hcryp);
          
          return HAL_TIMEOUT;
        }
      }
      
      /* Set the header phase */
      if(CRYPEx_GCMCCM_SetHeaderPhase(hcryp, hcryp->Init.Header, hcryp->Init.HeaderSize, 1) != HAL_OK)
      {
        return HAL_TIMEOUT;
      }
      /* Disable the CRYP peripheral */
      __HAL_CRYP_DISABLE();
      
      /* Select payload phase once the header phase is performed */
      __HAL_CRYP_SET_PHASE(CRYP_PHASE_PAYLOAD);
      
      /* Set the phase */
      hcryp->Phase = HAL_CRYP_PHASE_PROCESS;
    }
    
    /* Set the input and output addresses and start DMA transfer */ 
    CRYPEx_GCMCCM_SetDMAConfig(hcryp, inputaddr, Size, outputaddr);
    
    /* Unlock process */
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
  * @brief  Initializes the CRYP peripheral in AES CCM decryption mode using DMA
  *         then decrypted pCypherData. The cypher data are available in pPlainData.
  * @param  hcryp: CRYP handle
  * @param  pCypherData: Pointer to the cyphertext buffer  
  * @param  Size: Length of the plaintext buffer, must be a multiple of 16
  * @param  pPlainData: Pointer to the plaintext buffer  
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYPEx_AESCCM_Decrypt_DMA(CRYP_HandleTypeDef *hcryp, uint8_t *pCypherData, uint16_t Size, uint8_t *pPlainData)
{
  uint32_t timeout = 0;   
  uint32_t inputaddr;
  uint32_t outputaddr;
  uint32_t headersize;
  uint32_t headeraddr;
  uint32_t loopcounter = 0;
  uint32_t bufferidx = 0;
  uint8_t blockb0[16] = {0};/* Block B0 */
  uint8_t ctr[16] = {0}; /* Counter */
  uint32_t b0addr = (uint32_t)blockb0;
  
  if((hcryp->State == HAL_CRYP_STATE_READY) || (hcryp->Phase == HAL_CRYP_PHASE_PROCESS))
  {
    /* Process Locked */
    __HAL_LOCK(hcryp);
    
    inputaddr  = (uint32_t)pCypherData;
    outputaddr = (uint32_t)pPlainData;
    
    headersize = hcryp->Init.HeaderSize;
    headeraddr = (uint32_t)hcryp->Init.Header;
    
    hcryp->CrypInCount = Size;
    hcryp->pCrypInBuffPtr = pCypherData;
    hcryp->pCrypOutBuffPtr = pPlainData;
    hcryp->CrypOutCount = Size;
    
    /* Change the CRYP peripheral state */
    hcryp->State = HAL_CRYP_STATE_BUSY;
    
    /* Check if initialization phase has already been performed */
    if(hcryp->Phase == HAL_CRYP_PHASE_READY)
    {
      /************************ Formatting the header block *******************/
      if(headersize != 0)
      {
        /* Check that the associated data (or header) length is lower than 2^16 - 2^8 = 65536 - 256 = 65280 */
        if(headersize < 65280)
        {
          hcryp->Init.pScratch[bufferidx++] = (uint8_t) ((headersize >> 8) & 0xFF);
          hcryp->Init.pScratch[bufferidx++] = (uint8_t) ((headersize) & 0xFF);
          headersize += 2;
        }
        else
        {
          /* Header is encoded as 0xff || 0xfe || [headersize]32, i.e., six octets */
          hcryp->Init.pScratch[bufferidx++] = 0xFF;
          hcryp->Init.pScratch[bufferidx++] = 0xFE;
          hcryp->Init.pScratch[bufferidx++] = headersize & 0xff000000;
          hcryp->Init.pScratch[bufferidx++] = headersize & 0x00ff0000;
          hcryp->Init.pScratch[bufferidx++] = headersize & 0x0000ff00;
          hcryp->Init.pScratch[bufferidx++] = headersize & 0x000000ff;
          headersize += 6;
        }
        /* Copy the header buffer in internal buffer "hcryp->Init.pScratch" */
        for(loopcounter = 0; loopcounter < headersize; loopcounter++)
        {
          hcryp->Init.pScratch[bufferidx++] = hcryp->Init.Header[loopcounter];
        }
        /* Check if the header size is modulo 16 */
        if ((headersize % 16) != 0)
        {
          /* Padd the header buffer with 0s till the hcryp->Init.pScratch length is modulo 16 */
          for(loopcounter = headersize; loopcounter <= ((headersize/16) + 1) * 16; loopcounter++)
          {
            hcryp->Init.pScratch[loopcounter] = 0;
          }
          /* Set the header size to modulo 16 */
          headersize = ((headersize/16) + 1) * 16;
        }
        /* Set the pointer headeraddr to hcryp->Init.pScratch */
        headeraddr = (uint32_t)hcryp->Init.pScratch;
      }
      /*********************** Formatting the block B0 ************************/
      if(headersize != 0)
      {
        blockb0[0] = 0x40;
      }
      /* Flags byte */
      /* blockb0[0] |= 0u | (((( (uint8_t) hcryp->Init.TagSize - 2) / 2) & 0x07 ) << 3 ) | ( ( (uint8_t) (15 - hcryp->Init.IVSize) - 1) & 0x07) */
      blockb0[0] |= (uint8_t)((uint8_t)((uint8_t)(((uint8_t)(hcryp->Init.TagSize - (uint8_t)(2))) >> 1) & (uint8_t)0x07 ) << 3);
      blockb0[0] |= (uint8_t)((uint8_t)((uint8_t)((uint8_t)(15) - hcryp->Init.IVSize) - (uint8_t)1) & (uint8_t)0x07);
      
      for (loopcounter = 0; loopcounter < hcryp->Init.IVSize; loopcounter++)
      {
        blockb0[loopcounter+1] = hcryp->Init.pInitVect[loopcounter];
      }
      for ( ; loopcounter < 13; loopcounter++)
      {
        blockb0[loopcounter+1] = 0;
      }
      
      blockb0[14] = (Size >> 8);
      blockb0[15] = (Size & 0xFF);
      
      /************************* Formatting the initial counter ***************/
      /* Byte 0:
         Bits 7 and 6 are reserved and shall be set to 0
         Bits 3, 4, and 5 shall also be set to 0, to ensure that all the counter 
         blocks are distinct from B0
         Bits 0, 1, and 2 contain the same encoding of q as in B0
      */
      ctr[0] = blockb0[0] & 0x07;
      /* byte 1 to NonceSize is the IV (Nonce) */
      for(loopcounter = 1; loopcounter < hcryp->Init.IVSize + 1; loopcounter++)
      {
        ctr[loopcounter] = blockb0[loopcounter];
      }
      /* Set the LSB to 1 */
      ctr[15] |= 0x01;
      
      /* Set the key */
      CRYPEx_GCMCCM_SetKey(hcryp, hcryp->Init.pKey, hcryp->Init.KeySize);
      
      /* Set the CRYP peripheral in AES CCM mode */
      __HAL_CRYP_SET_MODE(CRYP_CR_ALGOMODE_AES_CCM_DECRYPT);
      
      /* Set the Initialization Vector */
      CRYPEx_GCMCCM_SetInitVector(hcryp, ctr, CRYP_KEYSIZE_128B);
      
      /* Select init phase */
      __HAL_CRYP_SET_PHASE(CRYP_PHASE_INIT);
      
      b0addr = (uint32_t)blockb0;
      /* Write the blockb0 block in the IN FIFO */
      CRYP->DR = *(uint32_t*)(b0addr);
      b0addr+=4;
      CRYP->DR = *(uint32_t*)(b0addr);
      b0addr+=4;
      CRYP->DR = *(uint32_t*)(b0addr);
      b0addr+=4;
      CRYP->DR = *(uint32_t*)(b0addr);
      
      /* Enable the CRYP peripheral */
      __HAL_CRYP_ENABLE();
      
      /* Get timeout */
      timeout = HAL_GetTick() + 1;
 
      while((CRYP->CR & CRYP_CR_CRYPEN) == CRYP_CR_CRYPEN)
      {
        /* Check for the Timeout */
        
        if(HAL_GetTick() >= timeout)
        {
          /* Change state */
          hcryp->State = HAL_CRYP_STATE_TIMEOUT;
          
          /* Process Unlocked */          
          __HAL_UNLOCK(hcryp);
          
          return HAL_TIMEOUT;
          
        }
      }
      /***************************** Header phase *****************************/
      if(headersize != 0)
      {
        /* Select header phase */
        __HAL_CRYP_SET_PHASE(CRYP_PHASE_HEADER);
        
        /* Enable Crypto processor */
        __HAL_CRYP_ENABLE();
        
        for(loopcounter = 0; (loopcounter < headersize); loopcounter+=16)
        {
          /* Get timeout */
          timeout = HAL_GetTick() + 1;
 
          while(HAL_IS_BIT_CLR(CRYP->SR, CRYP_FLAG_IFEM))
          {
            /* Check for the Timeout */
            if(HAL_GetTick() >= timeout)
            {
              /* Change state */
              hcryp->State = HAL_CRYP_STATE_TIMEOUT;
              
              /* Process Unlocked */          
              __HAL_UNLOCK(hcryp);
              
              return HAL_TIMEOUT;
            }
          }
          /* Write the header block in the IN FIFO */
          CRYP->DR = *(uint32_t*)(headeraddr);
          headeraddr+=4;
          CRYP->DR = *(uint32_t*)(headeraddr);
          headeraddr+=4;
          CRYP->DR = *(uint32_t*)(headeraddr);
          headeraddr+=4;
          CRYP->DR = *(uint32_t*)(headeraddr);
          headeraddr+=4;
        }
        
/* Get timeout */
        timeout = HAL_GetTick() + 1;

        while((CRYP->SR & CRYP_FLAG_BUSY) == CRYP_FLAG_BUSY)
        {
          /* Check for the Timeout */
          if(HAL_GetTick() >= timeout)
          {
            /* Change state */
            hcryp->State = HAL_CRYP_STATE_TIMEOUT;
            
            /* Process Unlocked */          
            __HAL_UNLOCK(hcryp);
            
            return HAL_TIMEOUT;
          }
        }
      }
      /* Save formatted counter into the scratch buffer pScratch */
      for(loopcounter = 0; (loopcounter < 16); loopcounter++)
      {
        hcryp->Init.pScratch[loopcounter] = ctr[loopcounter];
      }
      /* Reset bit 0 */
      hcryp->Init.pScratch[15] &= 0xfe;
      /* Select payload phase once the header phase is performed */
      __HAL_CRYP_SET_PHASE(CRYP_PHASE_PAYLOAD);
      
      /* Flush FIFO */
      __HAL_CRYP_FIFO_FLUSH();
      
      /* Set the phase */
      hcryp->Phase = HAL_CRYP_PHASE_PROCESS;
    }
    /* Set the input and output addresses and start DMA transfer */ 
    CRYPEx_GCMCCM_SetDMAConfig(hcryp, inputaddr, Size, outputaddr);
    
    /* Unlock process */
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
  * @brief  This function handles CRYP interrupt request.
  * @param  hcryp: CRYP handle
  * @retval None
  */
void HAL_CRYPEx_GCMCCM_IRQHandler(CRYP_HandleTypeDef *hcryp)
{
  switch(CRYP->CR & CRYP_CR_ALGOMODE_DIRECTION)
  {    
  case CRYP_CR_ALGOMODE_AES_GCM_ENCRYPT:
    HAL_CRYPEx_AESGCM_Encrypt_IT(hcryp, NULL, 0, NULL);
    break;
    
  case CRYP_CR_ALGOMODE_AES_GCM_DECRYPT:
    HAL_CRYPEx_AESGCM_Decrypt_IT(hcryp, NULL, 0, NULL);
    break;
    
  case CRYP_CR_ALGOMODE_AES_CCM_ENCRYPT:
    HAL_CRYPEx_AESCCM_Encrypt_IT(hcryp, NULL, 0, NULL);
    break;
    
  case CRYP_CR_ALGOMODE_AES_CCM_DECRYPT:
    HAL_CRYPEx_AESCCM_Decrypt_IT(hcryp, NULL, 0, NULL);
    break;
    
  default:
    break;
  }
}

/**
  * @}
  */

/**
  * @brief  DMA CRYP Input Data process complete callback. 
  * @param  hdma: DMA handle
  * @retval None
  */
static void CRYPEx_GCMCCM_DMAInCplt(DMA_HandleTypeDef *hdma)  
{
  CRYP_HandleTypeDef* hcryp = ( CRYP_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;
  
  /* Disable the DMA transfer for input Fifo request by resetting the DIEN bit
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
static void CRYPEx_GCMCCM_DMAOutCplt(DMA_HandleTypeDef *hdma)
{
  CRYP_HandleTypeDef* hcryp = ( CRYP_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;
  
  /* Disable the DMA transfer for output Fifo request by resetting the DOEN bit
     in the DMACR register */
  CRYP->DMACR &= (uint32_t)(~CRYP_DMACR_DOEN);
  
  /* Enable the CRYP peripheral */
  __HAL_CRYP_DISABLE();
  
  /* Change the CRYP peripheral state */
  hcryp->State = HAL_CRYP_STATE_READY;
  
  /* Call output data transfer complete callback */
  HAL_CRYP_OutCpltCallback(hcryp);
}

/**
  * @brief  DMA CRYP communication error callback. 
  * @param  hdma: DMA handle
  * @retval None
  */
static void CRYPEx_GCMCCM_DMAError(DMA_HandleTypeDef *hdma)
{
  CRYP_HandleTypeDef* hcryp = ( CRYP_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;
  hcryp->State= HAL_CRYP_STATE_READY;
  HAL_CRYP_ErrorCallback(hcryp);
}

/**
  * @brief  Writes the Key in Key registers. 
  * @param  hcryp: CRYP handle
  * @param  Key: Pointer to Key buffer
  * @param  KeySize: Size of Key
  * @retval None
  */
static void CRYPEx_GCMCCM_SetKey(CRYP_HandleTypeDef *hcryp, uint8_t *Key, uint32_t KeySize)
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
  * @param  hcryp: CRYP handle
  * @param  InitVector: Pointer to InitVector/InitCounter buffer
  * @param  IVSize: Size of the InitVector/InitCounter
  * @retval None
  */
static void CRYPEx_GCMCCM_SetInitVector(CRYP_HandleTypeDef *hcryp, uint8_t *InitVector, uint32_t IVSize)
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
  * @brief  Process Data: Writes Input data in polling mode and read the Output data.
  * @param  hcryp: CRYP handle
  * @param  Input: Pointer to the Input buffer.
  * @param  Ilength: Length of the Input buffer, must be a multiple of 16
  * @param  Output: Pointer to the returned buffer
  * @param  Timeout: Timeout value 
  * @retval None
  */
static HAL_StatusTypeDef CRYPEx_GCMCCM_ProcessData(CRYP_HandleTypeDef *hcryp, uint8_t *Input, uint16_t Ilength, uint8_t *Output, uint32_t Timeout)
{
  uint32_t timeout = 0;   
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
    
    /* Get timeout */
    timeout = HAL_GetTick() + Timeout;
 
    while(HAL_IS_BIT_CLR(CRYP->SR, CRYP_FLAG_OFNE))
    {
      /* Check for the Timeout */
      if(Timeout != HAL_MAX_DELAY)
      {
        if(HAL_GetTick() >= timeout)
        {
          /* Change state */
          hcryp->State = HAL_CRYP_STATE_TIMEOUT;
          
          /* Process Unlocked */          
          __HAL_UNLOCK(hcryp);
          
          return HAL_TIMEOUT;
        }
      }
    }
    /* Read the Output block from the OUT FIFO */
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
  * @brief  Sets the header phase
  * @param  hcryp: CRYP handle
  * @param  Input: Pointer to the Input buffer.
  * @param  Ilength: Length of the Input buffer, must be a multiple of 16
  * @param  Timeout: Timeout value   
  * @retval None
  */
static HAL_StatusTypeDef CRYPEx_GCMCCM_SetHeaderPhase(CRYP_HandleTypeDef *hcryp, uint8_t* Input, uint16_t Ilength, uint32_t Timeout)
{
  uint32_t timeout = 0;   
  uint32_t loopcounter = 0;
  uint32_t headeraddr = (uint32_t)Input;
  
  /***************************** Header phase *********************************/
  if(hcryp->Init.HeaderSize != 0)
  {
    /* Select header phase */
    __HAL_CRYP_SET_PHASE(CRYP_PHASE_HEADER);
    /* Enable the CRYP peripheral */
    __HAL_CRYP_ENABLE();
    
    for(loopcounter = 0; (loopcounter < hcryp->Init.HeaderSize); loopcounter+=16)
    {
      /* Get timeout */
      timeout = HAL_GetTick() + Timeout;
      
      while(HAL_IS_BIT_CLR(CRYP->SR, CRYP_FLAG_IFEM))
      {
        /* Check for the Timeout */
        if(Timeout != HAL_MAX_DELAY)
        {
          if(HAL_GetTick() >= timeout)
          {
            /* Change state */
            hcryp->State = HAL_CRYP_STATE_TIMEOUT;
            
            /* Process Unlocked */          
            __HAL_UNLOCK(hcryp);
            
            return HAL_TIMEOUT;
          }
        }
      }
      /* Write the Input block in the IN FIFO */
      CRYP->DR = *(uint32_t*)(headeraddr);
      headeraddr+=4;
      CRYP->DR = *(uint32_t*)(headeraddr);
      headeraddr+=4;
      CRYP->DR = *(uint32_t*)(headeraddr);
      headeraddr+=4;
      CRYP->DR = *(uint32_t*)(headeraddr);
      headeraddr+=4;
    }
    
    /* Wait until the complete message has been processed */

    /* Get timeout */
    timeout = HAL_GetTick() + Timeout;

    while((CRYP->SR & CRYP_FLAG_BUSY) == CRYP_FLAG_BUSY)
    {
      /* Check for the Timeout */
      if(Timeout != HAL_MAX_DELAY)
      {
        if(HAL_GetTick() >= timeout)
        {
          /* Change state */
          hcryp->State = HAL_CRYP_STATE_TIMEOUT;
          
          /* Process Unlocked */          
          __HAL_UNLOCK(hcryp);
          
          return HAL_TIMEOUT;
        }
      }
    }
  }
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Sets the DMA configuration and start the DMA transfert.
  * @param  hcryp: CRYP handle
  * @param  inputaddr: Address of the Input buffer
  * @param  Size: Size of the Input buffer, must be a multiple of 16
  * @param  outputaddr: Address of the Output buffer
  * @retval None
  */
static void CRYPEx_GCMCCM_SetDMAConfig(CRYP_HandleTypeDef *hcryp, uint32_t inputaddr, uint16_t Size, uint32_t outputaddr)
{
  /* Set the CRYP DMA transfer complete callback */
  hcryp->hdmain->XferCpltCallback = CRYPEx_GCMCCM_DMAInCplt;
  /* Set the DMA error callback */
  hcryp->hdmain->XferErrorCallback = CRYPEx_GCMCCM_DMAError;
  
  /* Set the CRYP DMA transfer complete callback */
  hcryp->hdmaout->XferCpltCallback = CRYPEx_GCMCCM_DMAOutCplt;
  /* Set the DMA error callback */
  hcryp->hdmaout->XferErrorCallback = CRYPEx_GCMCCM_DMAError;
  
  /* Enable the CRYP peripheral */
  __HAL_CRYP_ENABLE();
  
  /* Enable the DMA In DMA Stream */
  HAL_DMA_Start_IT(hcryp->hdmain, inputaddr, (uint32_t)&CRYP->DR, Size/4);
  
  /* Enable In DMA request */
  CRYP->DMACR = CRYP_DMACR_DIEN;
  
  /* Enable the DMA Out DMA Stream */
  HAL_DMA_Start_IT(hcryp->hdmaout, (uint32_t)&CRYP->DOUT, outputaddr, Size/4);
  
  /* Enable Out DMA request */
  CRYP->DMACR |= CRYP_DMACR_DOEN;
}

/**
  * @}
  */
#endif /* STM32F437xx || STM32F439xx */

#endif /* HAL_CRYP_MODULE_ENABLED */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
