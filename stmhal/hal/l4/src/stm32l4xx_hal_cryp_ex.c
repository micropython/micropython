/**
  ******************************************************************************
  * @file    stm32l4xx_hal_cryp_ex.c
  * @author  MCD Application Team
  * @version V1.3.0
  * @date    29-January-2016
  * @brief   CRYPEx HAL module driver.
  *          This file provides firmware functions to manage the extended 
  *          functionalities of the Cryptography (CRYP) peripheral.  
  *         
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
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
#include "stm32l4xx_hal.h"

#ifdef HAL_CRYP_MODULE_ENABLED

#if defined(STM32L485xx) || defined(STM32L486xx)

/** @addtogroup STM32L4xx_HAL_Driver
  * @{
  */

/** @defgroup CRYPEx CRYPEx
  * @brief CRYP Extended HAL module driver
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/** @defgroup CRYPEx_Private_Constants CRYPEx Private Constants
  * @{
  */
#define CRYP_CCF_TIMEOUTVALUE                      22000  /*!< CCF flag raising time-out value */
#define CRYP_BUSY_TIMEOUTVALUE                     22000  /*!< BUSY flag reset time-out value  */
/**
  * @}
  */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/** @defgroup CRYPEx_Private_Functions CRYPEx Private Functions
 * @{
 */
static HAL_StatusTypeDef CRYP_ProcessData(CRYP_HandleTypeDef *hcryp, uint8_t* Input, uint16_t Ilength, uint8_t* Output, uint32_t Timeout);
static HAL_StatusTypeDef CRYP_ReadKey(CRYP_HandleTypeDef *hcryp, uint8_t* Output, uint32_t Timeout);
static void CRYP_SetDMAConfig(CRYP_HandleTypeDef *hcryp, uint32_t inputaddr, uint16_t Size, uint32_t outputaddr);
static void CRYP_GCMCMAC_SetDMAConfig(CRYP_HandleTypeDef *hcryp, uint32_t inputaddr, uint16_t Size, uint32_t outputaddr);
static void CRYP_GCMCMAC_DMAInCplt(DMA_HandleTypeDef *hdma);
static void CRYP_GCMCMAC_DMAError(DMA_HandleTypeDef *hdma);
static void CRYP_GCMCMAC_DMAOutCplt(DMA_HandleTypeDef *hdma);
static HAL_StatusTypeDef CRYP_WaitOnCCFlag(CRYP_HandleTypeDef *hcryp, uint32_t Timeout);
static HAL_StatusTypeDef CRYP_WaitOnBusyFlagReset(CRYP_HandleTypeDef *hcryp, uint32_t Timeout);
static void CRYP_DMAInCplt(DMA_HandleTypeDef *hdma);
static void CRYP_DMAOutCplt(DMA_HandleTypeDef *hdma);
static void CRYP_DMAError(DMA_HandleTypeDef *hdma);
/**
  * @}
  */

/* Exported functions ---------------------------------------------------------*/

/** @defgroup CRYPEx_Exported_Functions CRYPEx Exported Functions
  * @{
  */


/** @defgroup CRYPEx_Exported_Functions_Group1 Extended callback function 
 *  @brief    Extended callback functions. 
 *
@verbatim   
 ===============================================================================
                 ##### Extended callback functions #####
 =============================================================================== 
    [..]  This section provides callback function:
      (+) Computation completed.

@endverbatim
  * @{
  */


/**
  * @brief  Computation completed callbacks.
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @retval None
  */
__weak void HAL_CRYPEx_ComputationCpltCallback(CRYP_HandleTypeDef *hcryp)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hcryp);

  /* NOTE : This function should not be modified; when the callback is needed,
            the HAL_CRYPEx_ComputationCpltCallback can be implemented in the user file
   */ 
}

/**
  * @}
  */

/** @defgroup CRYPEx_Exported_Functions_Group2 AES extended processing functions 
 *  @brief   Extended processing functions. 
 *
@verbatim   
  ==============================================================================
                      ##### AES extended processing functions #####
  ==============================================================================  
    [..]  This section provides functions allowing to:
      (+) Encrypt plaintext or decrypt cipher text using AES algorithm in different chaining modes.
          Functions are generic (handles ECB, CBC and CTR and all modes) and are only differentiated
          based on the processing type. Three processing types are available:
          (++) Polling mode
          (++) Interrupt mode
          (++) DMA mode
      (+) Generate and authentication tag in addition to encrypt/decrypt a plain/cipher text using AES 
          algorithm in different chaining modes.
          Functions are generic (handles GCM, GMAC and CMAC) and process only one phase so that steps
          can be skipped if so required. Functions are only differentiated based on the processing type. 
          Three processing types are available:
          (++) Polling mode
          (++) Interrupt mode
          (++) DMA mode          

@endverbatim
  * @{
  */

/**
  * @brief  Carry out in polling mode the ciphering or deciphering operation according to
  *         hcryp->Init structure fields, all operating modes (encryption, key derivation and/or decryption) and 
  *         chaining modes ECB, CBC and CTR are managed by this function in polling mode.
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  pInputData: Pointer to the plain text in case of encryption or cipher text in case of decryption
  *                     or key derivation+decryption.
  *                     Parameter is meaningless in case of key derivation.      
  * @param  Size: Length of the input data buffer in bytes, must be a multiple of 16.
  *               Parameter is meaningless in case of key derivation.  
  * @param  pOutputData: Pointer to the cipher text in case of encryption or plain text in case of 
  *                     decryption/key derivation+decryption, or pointer to the derivative keys in
  *                     case of key derivation only.   
  * @param  Timeout: Specify Timeout value 
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYPEx_AES(CRYP_HandleTypeDef *hcryp, uint8_t *pInputData, uint16_t Size, uint8_t *pOutputData, uint32_t Timeout)
{

  if (hcryp->State == HAL_CRYP_STATE_READY)
  {
    /* Check parameters setting */
    if (hcryp->Init.OperatingMode == CRYP_ALGOMODE_KEYDERIVATION)
    {
      if (pOutputData == NULL) 
      {
        return  HAL_ERROR;
      }
    }
    else
    {
      if ((pInputData == NULL) || (pOutputData == NULL) || (Size == 0))
      {
        return  HAL_ERROR;
      }
    }
    
    /* Process Locked */
    __HAL_LOCK(hcryp);
  
    /* Change the CRYP state */
    hcryp->State = HAL_CRYP_STATE_BUSY;
  
    /* Call CRYP_ReadKey() API if the operating mode is set to
       key derivation, CRYP_ProcessData() otherwise  */
    if (hcryp->Init.OperatingMode == CRYP_ALGOMODE_KEYDERIVATION)
    {
      if(CRYP_ReadKey(hcryp, pOutputData, Timeout) != HAL_OK)
      {
        return HAL_TIMEOUT;
      }    
    }
    else
    {
      if(CRYP_ProcessData(hcryp, pInputData, Size, pOutputData, Timeout) != HAL_OK)
      {
        return HAL_TIMEOUT;
      }
    }
  
    /* If the state has not been set to SUSPENDED, set it to
       READY, otherwise keep it as it is */
    if (hcryp->State != HAL_CRYP_STATE_SUSPENDED)
    {
      hcryp->State = HAL_CRYP_STATE_READY;
    }
  
    /* Process Unlocked */
    __HAL_UNLOCK(hcryp);
  
    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}



/**
  * @brief  Carry out in interrupt mode the ciphering or deciphering operation according to
  *         hcryp->Init structure fields, all operating modes (encryption, key derivation and/or decryption) and 
  *         chaining modes ECB, CBC and CTR are managed by this function in interrupt mode.
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  pInputData: Pointer to the plain text in case of encryption or cipher text in case of decryption
  *                     or key derivation+decryption.
  *                     Parameter is meaningless in case of key derivation.      
  * @param  Size: Length of the input data buffer in bytes, must be a multiple of 16.
  *               Parameter is meaningless in case of key derivation.  
  * @param  pOutputData: Pointer to the cipher text in case of encryption or plain text in case of 
  *                     decryption/key derivation+decryption, or pointer to the derivative keys in 
  *                     case of key derivation only.    
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYPEx_AES_IT(CRYP_HandleTypeDef *hcryp,  uint8_t *pInputData, uint16_t Size, uint8_t *pOutputData)
{
  uint32_t inputaddr = 0;
  
  if(hcryp->State == HAL_CRYP_STATE_READY)
  {
    /* Check parameters setting */
    if (hcryp->Init.OperatingMode == CRYP_ALGOMODE_KEYDERIVATION)
    {
      if (pOutputData == NULL) 
      {
        return  HAL_ERROR;
      }
    }
    else
    {
      if ((pInputData == NULL) || (pOutputData == NULL) || (Size == 0))
      {
        return  HAL_ERROR;
      }
    }
    /* Process Locked */
    __HAL_LOCK(hcryp);
    
    /* If operating mode is not limited to key derivation only,
       get the buffers addresses and sizes */
    if (hcryp->Init.OperatingMode != CRYP_ALGOMODE_KEYDERIVATION)
    {

      hcryp->CrypInCount = Size;
      hcryp->pCrypInBuffPtr = pInputData;
      hcryp->pCrypOutBuffPtr = pOutputData;
      hcryp->CrypOutCount = Size;
    }
    
    /* Change the CRYP state */
    hcryp->State = HAL_CRYP_STATE_BUSY;
  
      /* Process Unlocked */
    __HAL_UNLOCK(hcryp);
    
    /* Enable Computation Complete Flag and Error Interrupts */
    __HAL_CRYP_ENABLE_IT(CRYP_IT_CCFIE|CRYP_IT_ERRIE);
    
    
    /* If operating mode is key derivation only, the input data have 
       already been entered during the initialization process. For
       the other operating modes, they are fed to the CRYP hardware 
       block at this point. */
    if (hcryp->Init.OperatingMode != CRYP_ALGOMODE_KEYDERIVATION)
    {
      /* Initiate the processing under interrupt in entering 
         the first input data */
      inputaddr = (uint32_t)hcryp->pCrypInBuffPtr;
      /* Increment/decrement instance pointer/counter */
      hcryp->pCrypInBuffPtr += 16;
      hcryp->CrypInCount -= 16;
      /* Write the first input block in the Data Input register */
      hcryp->Instance->DINR = *(uint32_t*)(inputaddr);
      inputaddr+=4;
      hcryp->Instance->DINR = *(uint32_t*)(inputaddr);
      inputaddr+=4;
      hcryp->Instance->DINR  = *(uint32_t*)(inputaddr);
      inputaddr+=4;
      hcryp->Instance->DINR = *(uint32_t*)(inputaddr);
    }
    
    /* Return function status */
    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;   
  }
}
  
  
  


/**
  * @brief  Carry out in DMA mode the ciphering or deciphering operation according to
  *         hcryp->Init structure fields.
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  pInputData: Pointer to the plain text in case of encryption or cipher text in case of decryption
  *                     or key derivation+decryption.    
  * @param  Size: Length of the input data buffer in bytes, must be a multiple of 16.
  * @param  pOutputData: Pointer to the cipher text in case of encryption or plain text in case of 
  *                     decryption/key derivation+decryption.
  * @note   Chaining modes ECB, CBC and CTR are managed by this function in DMA mode.   
  * @note   Supported operating modes are encryption, decryption and key derivation with decryption. 
  * @note   No DMA channel is provided for key derivation only and therefore, access to AES_KEYRx 
  *         registers must be done by software.   
  * @note   This API is not applicable to key derivation only; for such a mode, access to AES_KEYRx 
  *         registers must be done by software thru HAL_CRYPEx_AES() or HAL_CRYPEx_AES_IT() APIs.
  * @note   pInputData and pOutputData buffers must be 32-bit aligned to ensure a correct DMA transfer to and from the IP.   
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYPEx_AES_DMA(CRYP_HandleTypeDef *hcryp,  uint8_t *pInputData, uint16_t Size, uint8_t *pOutputData)
{
  uint32_t inputaddr = 0;
  uint32_t outputaddr = 0;
  
  if (hcryp->State == HAL_CRYP_STATE_READY)
  {
    /* Check parameters setting */
    if (hcryp->Init.OperatingMode == CRYP_ALGOMODE_KEYDERIVATION)
    {
      /* no DMA channel is provided for key derivation operating mode, 
         access to AES_KEYRx registers must be done by software */
      return  HAL_ERROR;
    }
    else
    {
      if ((pInputData == NULL) || (pOutputData == NULL) || (Size == 0))
      {
        return  HAL_ERROR;
      }
    }
    
    
    /* Process Locked */
    __HAL_LOCK(hcryp);
    
    inputaddr  = (uint32_t)pInputData;
    outputaddr = (uint32_t)pOutputData;
    
    /* Change the CRYP state */
    hcryp->State = HAL_CRYP_STATE_BUSY;
    
    /* Set the input and output addresses and start DMA transfer */ 
    CRYP_SetDMAConfig(hcryp, inputaddr, Size, outputaddr);
    
    /* Process Unlocked */
    __HAL_UNLOCK(hcryp);
    
    /* Return function status */
    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;   
  }
}
  





/**
  * @brief  Carry out in polling mode the authentication tag generation as well as the ciphering or deciphering 
  *         operation according to hcryp->Init structure fields. 
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  pInputData: Pointer to payload data in GCM payload phase,
  *                     Parameter is meaningless in case of GCM/GMAC init, header and final phases, 
  *                     Pointer to B0 blocks in CMAC header phase,
  *                     Pointer to C block in CMAC final phase.                             
  * @param  Size: Length of the input payload data buffer in bytes, must be a multiple of 16,
  *               Parameter is meaningless in case of GCM/GMAC init and header phases,
  *               Length of B blocks (in bytes, must be a multiple of 16) in CMAC header phase,
  *               Length of C block (in bytes) in CMAC final phase.                              
  * @param  pOutputData: Pointer to plain or cipher text in GCM payload phase, 
  *                      pointer to authentication tag in GCM/GMAC and CMAC final phases.
  *                      Parameter is meaningless in case of GCM/GMAC init and header phases
  *                      and in case of CMAC header phase.  
  * @param  Timeout: Specify Timeout value 
  * @note   Supported operating modes are encryption and decryption, supported chaining modes are GCM, GMAC and CMAC.
  * @note   Phases are singly processed according to hcryp->Init.GCMCMACPhase so that steps in these specific chaining modes 
  *         can be skipped by the user if so required.          
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYPEx_AES_Auth(CRYP_HandleTypeDef *hcryp, uint8_t *pInputData, uint64_t Size, uint8_t *pOutputData, uint32_t Timeout)
{
  uint32_t index        = 0;
  uint32_t inputaddr    = 0;
  uint32_t outputaddr   = 0;
  uint32_t tagaddr      = 0;
  uint64_t headerlength = 0; 
  uint64_t inputlength  = 0; 
  
  if (hcryp->State == HAL_CRYP_STATE_READY)
  {
    /* input/output parameters check */
    if (hcryp->Init.GCMCMACPhase == CRYP_GCMCMAC_HEADER_PHASE)
    {
      if ((hcryp->Init.Header == NULL) || (hcryp->Init.HeaderSize == 0))
      {
        return  HAL_ERROR;
      }
      if (hcryp->Init.ChainingMode == CRYP_CHAINMODE_AES_CMAC)
      {
        /* In case of CMAC header phase resumption, we can have pInputData = NULL and  Size = 0 */
        if (((pInputData != NULL) && (Size == 0)) || ((pInputData == NULL) && (Size != 0)))
        {
          return  HAL_ERROR;
        }
      }
    }
    else if (hcryp->Init.GCMCMACPhase == CRYP_GCM_PAYLOAD_PHASE)
    {   
      if ((pInputData == NULL) || (pOutputData == NULL) || (Size == 0))
      {
        return  HAL_ERROR;
      }
    }
    else if (hcryp->Init.GCMCMACPhase == CRYP_GCMCMAC_FINAL_PHASE)
    {
      if (pOutputData == NULL)
      {
        return  HAL_ERROR;
      }
      if ((hcryp->Init.ChainingMode == CRYP_CHAINMODE_AES_CMAC) && (pInputData == NULL))
      {
        return  HAL_ERROR;
      }
    }
      
      
    /* Process Locked */
    __HAL_LOCK(hcryp);
  
    /* Change the CRYP state */
    hcryp->State = HAL_CRYP_STATE_BUSY;
  
    /*=====================*/
    /* GCM/GMAC init phase */
    /*=====================*/
    /* In case of init phase, the input data (Key and Initialization Vector) have 
       already been entered during the initialization process. Therefore, the
       API just waits for the CCF flag to be set. */
    if (hcryp->Init.GCMCMACPhase == CRYP_GCM_INIT_PHASE)
    {
      /* just wait for hash computation */
      if(CRYP_WaitOnCCFlag(hcryp, Timeout) != HAL_OK)  
      { 
        hcryp->State = HAL_CRYP_STATE_READY;        
        __HAL_UNLOCK(hcryp);
        return HAL_TIMEOUT;
      }
      
      /* Clear CCF Flag */
      __HAL_CRYP_CLEAR_FLAG(CRYP_CCF_CLEAR);
      /* Mark that the initialization phase is over */
      hcryp->Phase = HAL_CRYP_PHASE_INIT_OVER;
    }
    /*===============================*/
    /* GCM/GMAC or CMAC header phase */
    /*===============================*/
    else if (hcryp->Init.GCMCMACPhase == CRYP_GCMCMAC_HEADER_PHASE)
    {      
      /* Set header phase; for GCM or GMAC, set data-byte at this point */
      if (hcryp->Init.ChainingMode == CRYP_CHAINMODE_AES_GCM_GMAC)
      {
        MODIFY_REG(hcryp->Instance->CR, AES_CR_GCMPH|AES_CR_DATATYPE, CRYP_GCMCMAC_HEADER_PHASE|hcryp->Init.DataType);
      }
      else
      {
        MODIFY_REG(hcryp->Instance->CR, AES_CR_GCMPH, CRYP_GCMCMAC_HEADER_PHASE);
      }
         
      /* Enable the Peripheral */
      __HAL_CRYP_ENABLE();
      
      /* in case of CMAC, enter B0 block in header phase, before the header itself. */
      /* If Size = 0 (possible case of resumption after CMAC header phase suspension),
         skip these steps and go directly to header buffer feeding to the HW */
      if ((hcryp->Init.ChainingMode == CRYP_CHAINMODE_AES_CMAC) && (Size != 0))
      {
        inputaddr = (uint32_t)pInputData; 
        
        for(index=0; (index < Size); index += 16)
        {
          /* Write the Input block in the Data Input register */
          hcryp->Instance->DINR = *(uint32_t*)(inputaddr);
          inputaddr+=4;
          hcryp->Instance->DINR = *(uint32_t*)(inputaddr);
          inputaddr+=4;
          hcryp->Instance->DINR  = *(uint32_t*)(inputaddr);
          inputaddr+=4;
          hcryp->Instance->DINR = *(uint32_t*)(inputaddr);
          inputaddr+=4;
          
          if(CRYP_WaitOnCCFlag(hcryp, Timeout) != HAL_OK)  
          { 
            hcryp->State = HAL_CRYP_STATE_READY;        
            __HAL_UNLOCK(hcryp);
            return HAL_TIMEOUT;
          }
          /* Clear CCF Flag */
          __HAL_CRYP_CLEAR_FLAG(CRYP_CCF_CLEAR);           

          /* If the suspension flag has been raised and if the processing is not about
           to end, suspend processing */  
          if ((hcryp->SuspendRequest == HAL_CRYP_SUSPEND) && ((index+16) < Size))        
          {
            /* reset SuspendRequest */
            hcryp->SuspendRequest = HAL_CRYP_SUSPEND_NONE;
            /* Change the CRYP state */
            hcryp->State = HAL_CRYP_STATE_SUSPENDED;
            /* Mark that the header phase is over */
            hcryp->Phase = HAL_CRYP_PHASE_HEADER_SUSPENDED;
            
           /* Save current reading and writing locations of Input and Output buffers */
           hcryp->pCrypInBuffPtr  =  (uint8_t *)inputaddr;
           /* Save the total number of bytes (B blocks + header) that remain to be 
              processed at this point */
           hcryp->CrypInCount     =  hcryp->Init.HeaderSize + Size - (index+16);
        
           /* Process Unlocked */
            __HAL_UNLOCK(hcryp);
        
            return HAL_OK;
          } 
        } /* for(index=0; (index < Size); index += 16) */             
      }
      
      /* Enter header */  
      inputaddr = (uint32_t)hcryp->Init.Header; 
      for(index=0; (index < hcryp->Init.HeaderSize); index += 16)
      {
        /* Write the Input block in the Data Input register */
        hcryp->Instance->DINR = *(uint32_t*)(inputaddr);
        inputaddr+=4;
        hcryp->Instance->DINR = *(uint32_t*)(inputaddr);
        inputaddr+=4;
        hcryp->Instance->DINR  = *(uint32_t*)(inputaddr);
        inputaddr+=4;
        hcryp->Instance->DINR = *(uint32_t*)(inputaddr);
        inputaddr+=4;
        
        if(CRYP_WaitOnCCFlag(hcryp, Timeout) != HAL_OK)  
        { 
          hcryp->State = HAL_CRYP_STATE_READY;        
          __HAL_UNLOCK(hcryp);
          return HAL_TIMEOUT;
        }
        /* Clear CCF Flag */
        __HAL_CRYP_CLEAR_FLAG(CRYP_CCF_CLEAR); 
        
        /* If the suspension flag has been raised and if the processing is not about
         to end, suspend processing */  
        if ((hcryp->SuspendRequest == HAL_CRYP_SUSPEND) && ((index+16) < hcryp->Init.HeaderSize))        
        {
          /* reset SuspendRequest */
          hcryp->SuspendRequest = HAL_CRYP_SUSPEND_NONE;
          /* Change the CRYP state */
          hcryp->State = HAL_CRYP_STATE_SUSPENDED;
          /* Mark that the header phase is over */
          hcryp->Phase = HAL_CRYP_PHASE_HEADER_SUSPENDED;
          
         /* Save current reading and writing locations of Input and Output buffers */
         hcryp->pCrypInBuffPtr  =  (uint8_t *)inputaddr;
         /* Save the total number of bytes that remain to be processed at this point */
          hcryp->CrypInCount =  hcryp->Init.HeaderSize - (index+16);
      
         /* Process Unlocked */
          __HAL_UNLOCK(hcryp);
      
          return HAL_OK;
        }       
      }
      /* Mark that the header phase is over */
      hcryp->Phase = HAL_CRYP_PHASE_HEADER_OVER;
    }
    /*========================*/
    /* GCM/GMAC payload phase */
    /*========================*/
    else if (hcryp->Init.GCMCMACPhase == CRYP_GCM_PAYLOAD_PHASE)
    {
      
      MODIFY_REG(hcryp->Instance->CR, AES_CR_GCMPH, CRYP_GCM_PAYLOAD_PHASE);
      
      /* if the header phase has been bypassed, AES must be enabled again */
      if (hcryp->Phase == HAL_CRYP_PHASE_INIT_OVER)
      {
        __HAL_CRYP_ENABLE();  
      }
      
      inputaddr  = (uint32_t)pInputData;
      outputaddr = (uint32_t)pOutputData;
      
      /* Enter payload */
      for(index=0; (index < Size); index += 16)
      {
        /* Write the Input block in the Data Input register */
        hcryp->Instance->DINR = *(uint32_t*)(inputaddr);
        inputaddr+=4;
        hcryp->Instance->DINR = *(uint32_t*)(inputaddr);
        inputaddr+=4;
        hcryp->Instance->DINR  = *(uint32_t*)(inputaddr);
        inputaddr+=4;
        hcryp->Instance->DINR = *(uint32_t*)(inputaddr);
        inputaddr+=4;
        
        if(CRYP_WaitOnCCFlag(hcryp, Timeout) != HAL_OK)  
        { 
          hcryp->State = HAL_CRYP_STATE_READY;        
          __HAL_UNLOCK(hcryp);
          return HAL_TIMEOUT;
        }
          
        /* Clear CCF Flag */
        __HAL_CRYP_CLEAR_FLAG(CRYP_CCF_CLEAR);
        
        /* Retrieve output data: read the output block 
           from the Data Output Register */
        *(uint32_t*)(outputaddr) = hcryp->Instance->DOUTR;
        outputaddr+=4;
        *(uint32_t*)(outputaddr) = hcryp->Instance->DOUTR;
        outputaddr+=4;
        *(uint32_t*)(outputaddr) = hcryp->Instance->DOUTR;
        outputaddr+=4;
        *(uint32_t*)(outputaddr) = hcryp->Instance->DOUTR;
        outputaddr+=4;
       
        /* If the suspension flag has been raised and if the processing is not about
         to end, suspend processing */  
        if ((hcryp->SuspendRequest == HAL_CRYP_SUSPEND) && ((index+16) < Size))
        {
          /* no flag waiting under IRQ handling */
          if (hcryp->Init.OperatingMode == CRYP_ALGOMODE_ENCRYPT)
          {
            /* Ensure that Busy flag is reset */
            if(CRYP_WaitOnBusyFlagReset(hcryp, CRYP_BUSY_TIMEOUTVALUE) != HAL_OK)  
            {   
              hcryp->State = HAL_CRYP_STATE_READY;        
              __HAL_UNLOCK(hcryp);
              return HAL_TIMEOUT;
            }
          }               
          /* reset SuspendRequest */
          hcryp->SuspendRequest = HAL_CRYP_SUSPEND_NONE;
          /* Change the CRYP state */
          hcryp->State = HAL_CRYP_STATE_SUSPENDED;
          /* Mark that the header phase is over */
          hcryp->Phase = HAL_CRYP_PHASE_HEADER_SUSPENDED;
          
         /* Save current reading and writing locations of Input and Output buffers */
         hcryp->pCrypOutBuffPtr =  (uint8_t *)outputaddr;
         hcryp->pCrypInBuffPtr  =  (uint8_t *)inputaddr;
         /* Save the number of bytes that remain to be processed at this point */
         hcryp->CrypInCount     =  Size - (index+16);          
        
         /* Process Unlocked */
          __HAL_UNLOCK(hcryp);
        
          return HAL_OK;
        }            
        
      }
      /* Mark that the payload phase is over */
      hcryp->Phase = HAL_CRYP_PHASE_PAYLOAD_OVER;         
    }
    /*==============================*/
    /* GCM/GMAC or CMAC final phase */
    /*==============================*/
    else if (hcryp->Init.GCMCMACPhase == CRYP_GCMCMAC_FINAL_PHASE)
    {    
      tagaddr = (uint32_t)pOutputData;
      
      MODIFY_REG(hcryp->Instance->CR, AES_CR_GCMPH, CRYP_GCMCMAC_FINAL_PHASE);
      
      /* if the header and payload phases have been bypassed, AES must be enabled again */
      if (hcryp->Phase == HAL_CRYP_PHASE_INIT_OVER)
      {
        __HAL_CRYP_ENABLE();  
      }
      
      if (hcryp->Init.ChainingMode == CRYP_CHAINMODE_AES_GCM_GMAC)
      {
        headerlength = hcryp->Init.HeaderSize * 8; /* Header length in bits */
        inputlength = Size * 8;                    /* input length in bits */ 
        
           
        if(hcryp->Init.DataType == CRYP_DATATYPE_1B)
        {
          hcryp->Instance->DINR = __RBIT((headerlength)>>32);
          hcryp->Instance->DINR = __RBIT(headerlength);
          hcryp->Instance->DINR = __RBIT((inputlength)>>32);
          hcryp->Instance->DINR = __RBIT(inputlength);
        }
        else if(hcryp->Init.DataType == CRYP_DATATYPE_8B)
        {
          hcryp->Instance->DINR = __REV((headerlength)>>32);
          hcryp->Instance->DINR = __REV(headerlength);
          hcryp->Instance->DINR = __REV((inputlength)>>32);
          hcryp->Instance->DINR = __REV(inputlength);
        } 
        else if(hcryp->Init.DataType == CRYP_DATATYPE_16B)
        {
          hcryp->Instance->DINR = __ROR((headerlength)>>32, 16);
          hcryp->Instance->DINR = __ROR(headerlength, 16);
          hcryp->Instance->DINR = __ROR((inputlength)>>32, 16);
          hcryp->Instance->DINR = __ROR(inputlength, 16);          
        }
        else if(hcryp->Init.DataType == CRYP_DATATYPE_32B)
        {
          hcryp->Instance->DINR = (uint32_t)(headerlength>>32);
          hcryp->Instance->DINR = (uint32_t)(headerlength);
          hcryp->Instance->DINR = (uint32_t)(inputlength>>32);
          hcryp->Instance->DINR = (uint32_t)(inputlength);
        }
      }
      else if (hcryp->Init.ChainingMode == CRYP_CHAINMODE_AES_CMAC)
      {
        inputaddr  = (uint32_t)pInputData;
        /* Enter the last block made of a 128-bit value formatted
           from the original B0 packet. */
        hcryp->Instance->DINR = *(uint32_t*)(inputaddr);
        inputaddr+=4;
        hcryp->Instance->DINR = *(uint32_t*)(inputaddr);
        inputaddr+=4;
        hcryp->Instance->DINR  = *(uint32_t*)(inputaddr);
        inputaddr+=4;
        hcryp->Instance->DINR = *(uint32_t*)(inputaddr);
      }
      
      
      if(CRYP_WaitOnCCFlag(hcryp, Timeout) != HAL_OK)  
      { 
          hcryp->State = HAL_CRYP_STATE_READY;        
          __HAL_UNLOCK(hcryp);
          return HAL_TIMEOUT;
      }

      /* Read the Auth TAG in the Data Out register */
      *(uint32_t*)(tagaddr) = hcryp->Instance->DOUTR;
      tagaddr+=4;
      *(uint32_t*)(tagaddr) = hcryp->Instance->DOUTR;
      tagaddr+=4;
      *(uint32_t*)(tagaddr) = hcryp->Instance->DOUTR;
      tagaddr+=4;
      *(uint32_t*)(tagaddr) = hcryp->Instance->DOUTR; 
         

      /* Clear CCF Flag */
      __HAL_CRYP_CLEAR_FLAG(CRYP_CCF_CLEAR);
      /* Mark that the final phase is over */
      hcryp->Phase = HAL_CRYP_PHASE_FINAL_OVER;
      /* Disable the Peripheral */
      __HAL_CRYP_DISABLE();
    }
    /*=================================================*/
    /* case incorrect hcryp->Init.GCMCMACPhase setting */
    /*=================================================*/
    else
    {
      hcryp->State = HAL_CRYP_STATE_ERROR; 
      __HAL_UNLOCK(hcryp); 
      return HAL_ERROR;
    }
 
    /* Change the CRYP state */
    hcryp->State = HAL_CRYP_STATE_READY;
  
    /* Process Unlocked */
    __HAL_UNLOCK(hcryp);
  
    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}




/**
  * @brief  Carry out in interrupt mode the authentication tag generation as well as the ciphering or deciphering 
  *         operation according to hcryp->Init structure fields. 
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  pInputData: Pointer to payload data in GCM payload phase,
  *                     Parameter is meaningless in case of GCM/GMAC init, header and final phases, 
  *                     Pointer to B0 blocks in CMAC header phase,
  *                     Pointer to C block in CMAC final phase.       
  * @param  Size: Length of the input payload data buffer in bytes, must be a multiple of 16,
  *               Parameter is meaningless in case of GCM/GMAC init and header phases,
  *               Length of B blocks (in bytes, must be a multiple of 16) in CMAC header phase,
  *               Length of C block (in bytes) in CMAC final phase.           
  * @param  pOutputData: Pointer to plain or cipher text in GCM payload phase, 
  *                      pointer to authentication tag in GCM/GMAC and CMAC final phases.
  *                      Parameter is meaningless in case of GCM/GMAC init and header phases
  *                      and in case of CMAC header phase.
  * @note   Supported operating modes are encryption and decryption, supported chaining modes are GCM, GMAC and CMAC.
  * @note   Phases are singly processed according to hcryp->Init.GCMCMACPhase so that steps in these specific chaining modes 
  *         can be skipped by the user if so required.                                 
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYPEx_AES_Auth_IT(CRYP_HandleTypeDef *hcryp, uint8_t *pInputData, uint64_t Size, uint8_t *pOutputData)
{

  uint32_t inputaddr    = 0;
  uint64_t headerlength = 0;
  uint64_t inputlength  = 0;
 

  if (hcryp->State == HAL_CRYP_STATE_READY)
  {
    /* input/output parameters check */
    if (hcryp->Init.GCMCMACPhase == CRYP_GCMCMAC_HEADER_PHASE)
    {
      if ((hcryp->Init.Header == NULL) || (hcryp->Init.HeaderSize == 0))
      {
        return  HAL_ERROR;
      }
      if (hcryp->Init.ChainingMode == CRYP_CHAINMODE_AES_CMAC)
      {
        /* In case of CMAC header phase resumption, we can have pInputData = NULL and  Size = 0 */
        if (((pInputData != NULL) && (Size == 0)) || ((pInputData == NULL) && (Size != 0)))
        {
          return  HAL_ERROR;
        }
      }      
    }
    else if (hcryp->Init.GCMCMACPhase == CRYP_GCM_PAYLOAD_PHASE)
    {   
      if ((pInputData == NULL) || (pOutputData == NULL) || (Size == 0))
      {
        return  HAL_ERROR;
      }
    }
    else if (hcryp->Init.GCMCMACPhase == CRYP_GCMCMAC_FINAL_PHASE)
    {
      if (pOutputData == NULL)
      {
        return  HAL_ERROR;
      }
      if ((hcryp->Init.ChainingMode == CRYP_CHAINMODE_AES_CMAC) && (pInputData == NULL))
      {
        return  HAL_ERROR;
      }
    }
    
    
    /* Process Locked */
    __HAL_LOCK(hcryp);
    
    /* Change the CRYP state */
    hcryp->State = HAL_CRYP_STATE_BUSY;
    
    /* Process Unlocked */
    __HAL_UNLOCK(hcryp);
                           
    /* Enable Computation Complete Flag and Error Interrupts */
    __HAL_CRYP_ENABLE_IT(CRYP_IT_CCFIE|CRYP_IT_ERRIE);
    

    
    /*=====================*/
    /* GCM/GMAC init phase */
    /*=====================*/
    if (hcryp->Init.GCMCMACPhase == CRYP_GCM_INIT_PHASE)
    {    
    /* In case of init phase, the input data (Key and Initialization Vector) have 
       already been entered during the initialization process. Therefore, the
       software just waits for the CCF interrupt to be raised and which will
       be handled by CRYP_AES_Auth_IT() API. */
    }
    /*===============================*/
    /* GCM/GMAC or CMAC header phase */
    /*===============================*/   
    else if (hcryp->Init.GCMCMACPhase == CRYP_GCMCMAC_HEADER_PHASE)
    {
      if (hcryp->Init.ChainingMode == CRYP_CHAINMODE_AES_CMAC)
      {
        /* In case of CMAC, B blocks are first entered, before the header.
           Therefore, B blocks and the header are entered back-to-back
           as if it was only one single block. 
           However, in case of resumption after suspension, if all the
           B blocks have been entered (in that case, Size = 0), only the
           remainder of the non-processed header bytes are entered. */
          if (Size != 0)
          {
            hcryp->CrypInCount = Size + hcryp->Init.HeaderSize;
            hcryp->pCrypInBuffPtr = pInputData;
          }
          else
          {
            hcryp->CrypInCount = hcryp->Init.HeaderSize;
            hcryp->pCrypInBuffPtr = hcryp->Init.Header;
          }
      }
      else
      {
        /* Get the header addresses and sizes */
        hcryp->CrypInCount = hcryp->Init.HeaderSize;
        hcryp->pCrypInBuffPtr = hcryp->Init.Header;
      }    
    
      inputaddr = (uint32_t)hcryp->pCrypInBuffPtr;
      
      /* Set header phase; for GCM or GMAC, set data-byte at this point */
      if (hcryp->Init.ChainingMode == CRYP_CHAINMODE_AES_GCM_GMAC)
      {
        MODIFY_REG(hcryp->Instance->CR, AES_CR_GCMPH|AES_CR_DATATYPE, CRYP_GCMCMAC_HEADER_PHASE|hcryp->Init.DataType);
      }
      else
      {
        MODIFY_REG(hcryp->Instance->CR, AES_CR_GCMPH, CRYP_GCMCMAC_HEADER_PHASE);
      }
       
      /* Enable the Peripheral */
      __HAL_CRYP_ENABLE();
    
      /* Increment/decrement instance pointer/counter */
      hcryp->pCrypInBuffPtr += 16;
      hcryp->CrypInCount -= 16;
      
      
      if (hcryp->Init.ChainingMode == CRYP_CHAINMODE_AES_CMAC)
      { 
        if (hcryp->CrypInCount == hcryp->Init.HeaderSize)
        {
          /* All B blocks will have been entered after the next
             four DINR writing, so point at header buffer for
             the next iteration */
          hcryp->pCrypInBuffPtr = hcryp->Init.Header;
        }
      }       
    
      /* Enter header first block to initiate the process
         in the Data Input register */
      hcryp->Instance->DINR = *(uint32_t*)(inputaddr);
      inputaddr+=4;
      hcryp->Instance->DINR = *(uint32_t*)(inputaddr);
      inputaddr+=4;
      hcryp->Instance->DINR  = *(uint32_t*)(inputaddr);
      inputaddr+=4;
      hcryp->Instance->DINR = *(uint32_t*)(inputaddr);
    }
    /*========================*/
    /* GCM/GMAC payload phase */
    /*========================*/
    else if (hcryp->Init.GCMCMACPhase == CRYP_GCM_PAYLOAD_PHASE)
    {
      /* Get the buffer addresses and sizes */
      hcryp->CrypInCount = Size;
      hcryp->pCrypInBuffPtr = pInputData;
      hcryp->pCrypOutBuffPtr = pOutputData;
      hcryp->CrypOutCount = Size; 
    
      inputaddr = (uint32_t)hcryp->pCrypInBuffPtr;
      
      MODIFY_REG(hcryp->Instance->CR, AES_CR_GCMPH, CRYP_GCM_PAYLOAD_PHASE);
       
      /* if the header phase has been bypassed, AES must be enabled again */
      if (hcryp->Phase == HAL_CRYP_PHASE_INIT_OVER)
      {
        __HAL_CRYP_ENABLE();  
      }
    
      /* Increment/decrement instance pointer/counter */
      hcryp->pCrypInBuffPtr += 16;
      hcryp->CrypInCount -= 16;
    
      /* Enter payload first block to initiate the process
         in the Data Input register */
      hcryp->Instance->DINR = *(uint32_t*)(inputaddr);
      inputaddr+=4;
      hcryp->Instance->DINR = *(uint32_t*)(inputaddr);
      inputaddr+=4;
      hcryp->Instance->DINR  = *(uint32_t*)(inputaddr);
      inputaddr+=4;
      hcryp->Instance->DINR = *(uint32_t*)(inputaddr);
    }
    /*==============================*/
    /* GCM/GMAC or CMAC final phase */
    /*==============================*/
    else if (hcryp->Init.GCMCMACPhase == CRYP_GCMCMAC_FINAL_PHASE)
    {
       hcryp->pCrypOutBuffPtr = pOutputData;
       
       MODIFY_REG(hcryp->Instance->CR, AES_CR_GCMPH, CRYP_GCMCMAC_FINAL_PHASE);
       
      /* if the header and payload phases have been bypassed, AES must be enabled again */
      if (hcryp->Phase == HAL_CRYP_PHASE_INIT_OVER)
      {
        __HAL_CRYP_ENABLE();  
      }
      
      if (hcryp->Init.ChainingMode == CRYP_CHAINMODE_AES_GCM_GMAC)
      {
        headerlength = hcryp->Init.HeaderSize * 8; /* Header length in bits */
        inputlength = Size * 8;                   /* input length in bits */ 
        /* Write the number of bits in the header on 64 bits followed by the number
           of bits in the payload on 64 bits as well */
        if(hcryp->Init.DataType == CRYP_DATATYPE_1B)
        {
          hcryp->Instance->DINR = __RBIT((headerlength)>>32);
          hcryp->Instance->DINR = __RBIT(headerlength);
          hcryp->Instance->DINR = __RBIT((inputlength)>>32);
          hcryp->Instance->DINR = __RBIT(inputlength);
        }
        else if(hcryp->Init.DataType == CRYP_DATATYPE_8B)
        {
          hcryp->Instance->DINR = __REV((headerlength)>>32);
          hcryp->Instance->DINR = __REV(headerlength);
          hcryp->Instance->DINR = __REV((inputlength)>>32);
          hcryp->Instance->DINR = __REV(inputlength);
        }
        else if(hcryp->Init.DataType == CRYP_DATATYPE_16B)
        {
          hcryp->Instance->DINR = __ROR((headerlength)>>32, 16);
          hcryp->Instance->DINR = __ROR(headerlength, 16);
          hcryp->Instance->DINR = __ROR((inputlength)>>32, 16);
          hcryp->Instance->DINR = __ROR(inputlength, 16);             
        }
        else if(hcryp->Init.DataType == CRYP_DATATYPE_32B)
        {
          hcryp->Instance->DINR = (uint32_t)(headerlength>>32);
          hcryp->Instance->DINR = (uint32_t)(headerlength);
          hcryp->Instance->DINR = (uint32_t)(inputlength>>32);
          hcryp->Instance->DINR = (uint32_t)(inputlength);
        }
      }
      else if (hcryp->Init.ChainingMode == CRYP_CHAINMODE_AES_CMAC)
      {
        inputaddr  = (uint32_t)pInputData;
        /* Enter the last block made of a 128-bit value formatted
           from the original B0 packet. */
        hcryp->Instance->DINR = *(uint32_t*)(inputaddr);
        inputaddr+=4;
        hcryp->Instance->DINR = *(uint32_t*)(inputaddr);
        inputaddr+=4;
        hcryp->Instance->DINR  = *(uint32_t*)(inputaddr);
        inputaddr+=4;
        hcryp->Instance->DINR = *(uint32_t*)(inputaddr);
        inputaddr+=4;
      }
    }
    /*=================================================*/
    /* case incorrect hcryp->Init.GCMCMACPhase setting */
    /*=================================================*/
    else
    {
      hcryp->State = HAL_CRYP_STATE_ERROR; 
      return HAL_ERROR;
    }
  
    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}




/**
  * @brief  Carry out in DMA mode the authentication tag generation as well as the ciphering or deciphering 
  *         operation according to hcryp->Init structure fields. 
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @param  pInputData: Pointer to payload data in GCM payload phase,
  *                     Parameter is meaningless in case of GCM/GMAC init, header and final phases, 
  *                     Pointer to B0 blocks in CMAC header phase,
  *                     Pointer to C block in CMAC final phase.      
  * @param  Size: Length of the input payload data buffer in bytes, must be a multiple of 16,
  *               Parameter is meaningless in case of GCM/GMAC init and header phases,
  *               Length of B blocks (in bytes, must be a multiple of 16) in CMAC header phase,
  *               Length of C block (in bytes) in CMAC final phase.          
  * @param  pOutputData: Pointer to plain or cipher text in GCM payload phase, 
  *                      pointer to authentication tag in GCM/GMAC and CMAC final phases.
  *                      Parameter is meaningless in case of GCM/GMAC init and header phases
  *                      and in case of CMAC header phase.
  * @note   Supported operating modes are encryption and decryption, supported chaining modes are GCM, GMAC and CMAC.
  * @note   Phases are singly processed according to hcryp->Init.GCMCMACPhase so that steps in these specific chaining modes 
  *         can be skipped by the user if so required.
  * @note   pInputData and pOutputData buffers must be 32-bit aligned to ensure a correct DMA transfer to and from the IP.            
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CRYPEx_AES_Auth_DMA(CRYP_HandleTypeDef *hcryp, uint8_t *pInputData, uint64_t Size, uint8_t *pOutputData)
{
  uint32_t inputaddr    = 0;
  uint32_t outputaddr   = 0;
  uint32_t tagaddr      = 0;
  uint64_t headerlength = 0;
  uint64_t inputlength  = 0; 
  
  
  if (hcryp->State == HAL_CRYP_STATE_READY)
  {
    /* input/output parameters check */
    if (hcryp->Init.GCMCMACPhase == CRYP_GCMCMAC_HEADER_PHASE)
    {
      if ((hcryp->Init.Header == NULL) || (hcryp->Init.HeaderSize == 0))
      {
        return  HAL_ERROR;
      }
      if (hcryp->Init.ChainingMode == CRYP_CHAINMODE_AES_CMAC)
      {
        if ((pInputData == NULL) || (Size == 0))
        {
          return  HAL_ERROR;
        }
      }      
    }
    else if (hcryp->Init.GCMCMACPhase == CRYP_GCM_PAYLOAD_PHASE)
    {   
      if ((pInputData == NULL) || (pOutputData == NULL) || (Size == 0))
      {
        return  HAL_ERROR;
      }
    }
    else if (hcryp->Init.GCMCMACPhase == CRYP_GCMCMAC_FINAL_PHASE)
    {
      if (pOutputData == NULL)
      {
        return  HAL_ERROR;
      }
      if ((hcryp->Init.ChainingMode == CRYP_CHAINMODE_AES_CMAC) && (pInputData == NULL))
      {
        return  HAL_ERROR;
      }
    }
    
    
    /* Process Locked */
    __HAL_LOCK(hcryp);
  
    /* Change the CRYP state */
    hcryp->State = HAL_CRYP_STATE_BUSY;
  
    /*=====================*/
    /* GCM/GMAC init phase */
    /*=====================*/
    /* In case of init phase, the input data (Key and Initialization Vector) have 
       already been entered during the initialization process. No DMA transfer is
       required at that point therefore, the software just waits for the CCF flag 
       to be raised. */
    if (hcryp->Init.GCMCMACPhase == CRYP_GCM_INIT_PHASE)
    {
      /* just wait for hash computation */
      if(CRYP_WaitOnCCFlag(hcryp, CRYP_CCF_TIMEOUTVALUE) != HAL_OK)  
      { 
        hcryp->State = HAL_CRYP_STATE_READY;        
        __HAL_UNLOCK(hcryp);
        return HAL_TIMEOUT;
      }
      
      /* Clear CCF Flag */
      __HAL_CRYP_CLEAR_FLAG(CRYP_CCF_CLEAR);
      /* Mark that the initialization phase is over */
      hcryp->Phase = HAL_CRYP_PHASE_INIT_OVER;
      hcryp->State = HAL_CRYP_STATE_READY;
    }
    /*===============================*/
    /* GCM/GMAC or CMAC header phase */
    /*===============================*/     
    else if (hcryp->Init.GCMCMACPhase == CRYP_GCMCMAC_HEADER_PHASE)
    {
      /* Set header phase; for GCM or GMAC, set data-byte at this point */
      if (hcryp->Init.ChainingMode == CRYP_CHAINMODE_AES_GCM_GMAC)
      {
        MODIFY_REG(hcryp->Instance->CR, AES_CR_GCMPH|AES_CR_DATATYPE, CRYP_GCMCMAC_HEADER_PHASE|hcryp->Init.DataType);
      }
      else
      {
        MODIFY_REG(hcryp->Instance->CR, AES_CR_GCMPH, CRYP_GCMCMAC_HEADER_PHASE);
      }
      
      /* enter first B0 block in polling mode (no DMA transfer for B0) */
      if (hcryp->Init.ChainingMode == CRYP_CHAINMODE_AES_CMAC)
      {
         /* Enable the CRYP peripheral */
        __HAL_CRYP_ENABLE();
  
        inputaddr  = (uint32_t)pInputData;
        hcryp->Instance->DINR = *(uint32_t*)(inputaddr);
        inputaddr+=4;
        hcryp->Instance->DINR = *(uint32_t*)(inputaddr);
        inputaddr+=4;
        hcryp->Instance->DINR  = *(uint32_t*)(inputaddr);
        inputaddr+=4;
        hcryp->Instance->DINR = *(uint32_t*)(inputaddr);
      
        if(CRYP_WaitOnCCFlag(hcryp, CRYP_CCF_TIMEOUTVALUE) != HAL_OK)  
        { 
          hcryp->State = HAL_CRYP_STATE_READY;        
          __HAL_UNLOCK(hcryp);
          return HAL_TIMEOUT;
        }
        /* Clear CCF Flag */
        __HAL_CRYP_CLEAR_FLAG(CRYP_CCF_CLEAR);
      }
      
      
      inputaddr = (uint32_t)hcryp->Init.Header;
      /* Set the input address and start DMA transfer */ 
      CRYP_GCMCMAC_SetDMAConfig(hcryp, inputaddr, hcryp->Init.HeaderSize, 0);
    }
    /*========================*/
    /* GCM/GMAC payload phase */
    /*========================*/
    else if (hcryp->Init.GCMCMACPhase == CRYP_GCM_PAYLOAD_PHASE)
    {
      MODIFY_REG(hcryp->Instance->CR, AES_CR_GCMPH, CRYP_GCM_PAYLOAD_PHASE);
      
      inputaddr  = (uint32_t)pInputData;
      outputaddr = (uint32_t)pOutputData;
      /* Set the input and output addresses and start DMA transfer */ 
      CRYP_GCMCMAC_SetDMAConfig(hcryp, inputaddr, Size, outputaddr);   
    }
    /*==============================*/
    /* GCM/GMAC or CMAC final phase */
    /*==============================*/
    else if (hcryp->Init.GCMCMACPhase == CRYP_GCMCMAC_FINAL_PHASE)
    {
      tagaddr = (uint32_t)pOutputData;
      
      MODIFY_REG(hcryp->Instance->CR, AES_CR_GCMPH, CRYP_GCMCMAC_FINAL_PHASE);
      
      /* if the header and payload phases have been bypassed, AES must be enabled again */
      if (hcryp->Phase == HAL_CRYP_PHASE_INIT_OVER)
      {
        __HAL_CRYP_ENABLE();  
      }
      
      if (hcryp->Init.ChainingMode == CRYP_CHAINMODE_AES_GCM_GMAC)
      {
        headerlength = hcryp->Init.HeaderSize * 8; /* Header length in bits */
        inputlength = Size * 8;  /* input length in bits */ 
        /* Write the number of bits in the header on 64 bits followed by the number
           of bits in the payload on 64 bits as well */
        if(hcryp->Init.DataType == CRYP_DATATYPE_1B)
        {
          hcryp->Instance->DINR = __RBIT((headerlength)>>32);
          hcryp->Instance->DINR = __RBIT(headerlength);
          hcryp->Instance->DINR = __RBIT((inputlength)>>32);
          hcryp->Instance->DINR = __RBIT(inputlength);
        }
        else if(hcryp->Init.DataType == CRYP_DATATYPE_8B)
        {
          hcryp->Instance->DINR = __REV((headerlength)>>32);
          hcryp->Instance->DINR = __REV(headerlength);
          hcryp->Instance->DINR = __REV((inputlength)>>32);
          hcryp->Instance->DINR = __REV(inputlength);
        }
        else if(hcryp->Init.DataType == CRYP_DATATYPE_16B)
        {
          hcryp->Instance->DINR = __ROR((headerlength)>>32, 16);
          hcryp->Instance->DINR = __ROR(headerlength, 16);
          hcryp->Instance->DINR = __ROR((inputlength)>>32, 16);
          hcryp->Instance->DINR = __ROR(inputlength, 16);            
        }
        else if(hcryp->Init.DataType == CRYP_DATATYPE_32B)
        {
          hcryp->Instance->DINR = (uint32_t)(headerlength>>32);
          hcryp->Instance->DINR = (uint32_t)(headerlength);
          hcryp->Instance->DINR = (uint32_t)(inputlength>>32);
          hcryp->Instance->DINR = (uint32_t)(inputlength);
        }
      }
      else if (hcryp->Init.ChainingMode == CRYP_CHAINMODE_AES_CMAC)
      {
        __HAL_CRYP_CLEAR_FLAG(CRYP_CCF_CLEAR);
        
        inputaddr  = (uint32_t)pInputData;
        /* Enter the last block made of a 128-bit value formatted
           from the original B0 packet. */
        hcryp->Instance->DINR = *(uint32_t*)(inputaddr);
        inputaddr+=4;
        hcryp->Instance->DINR = *(uint32_t*)(inputaddr);
        inputaddr+=4;
        hcryp->Instance->DINR  = *(uint32_t*)(inputaddr);
        inputaddr+=4;
        hcryp->Instance->DINR = *(uint32_t*)(inputaddr);
        inputaddr+=4;
      }
      
      /* No DMA transfer is required at that point therefore, the software 
         just waits for the CCF flag to be raised. */
      if(CRYP_WaitOnCCFlag(hcryp, CRYP_CCF_TIMEOUTVALUE) != HAL_OK)  
      { 
          hcryp->State = HAL_CRYP_STATE_READY;        
          __HAL_UNLOCK(hcryp);
          return HAL_TIMEOUT;
      }
      /* Clear CCF Flag */
      __HAL_CRYP_CLEAR_FLAG(CRYP_CCF_CLEAR);
      /* Read the Auth TAG in the IN FIFO */
      *(uint32_t*)(tagaddr) = hcryp->Instance->DOUTR;
      tagaddr+=4;
      *(uint32_t*)(tagaddr) = hcryp->Instance->DOUTR;
      tagaddr+=4;
      *(uint32_t*)(tagaddr) = hcryp->Instance->DOUTR;
      tagaddr+=4;
      *(uint32_t*)(tagaddr) = hcryp->Instance->DOUTR;
  
      /* Mark that the final phase is over */
      hcryp->Phase = HAL_CRYP_PHASE_FINAL_OVER;
      hcryp->State = HAL_CRYP_STATE_READY;
      /* Disable the Peripheral */
      __HAL_CRYP_DISABLE();
    }
    /*=================================================*/
    /* case incorrect hcryp->Init.GCMCMACPhase setting */
    /*=================================================*/
    else
    {
      hcryp->State = HAL_CRYP_STATE_ERROR;
      __HAL_UNLOCK(hcryp); 
      return HAL_ERROR;
    }    
  
    /* Process Unlocked */
    __HAL_UNLOCK(hcryp);
  
    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @}
  */

/** @defgroup CRYPEx_Exported_Functions_Group3 AES suspension/resumption functions 
 *  @brief   Extended processing functions. 
 *
@verbatim   
  ==============================================================================
                    ##### AES extended suspension and resumption functions #####
  ==============================================================================  
    [..]  This section provides functions allowing to:
      (+) save in memory the Initialization Vector, the Key registers, the Control register or
          the Suspend registers when a process is suspended by a higher priority message
      (+) write back in CRYP hardware block the saved values listed above when the suspended
          lower priority message processing is resumed.     

@endverbatim
  * @{
  */


/**
  * @brief  In case of message processing suspension, read the Initialization Vector. 
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module.    
  * @param  Output: Pointer to the buffer containing the saved Initialization Vector.
  * @note   This value has to be stored for reuse by writing the AES_IVRx registers
  *         as soon as the interrupted processing has to be resumed.
  *         Applicable to all chaining modes.    
  * @note   AES must be disabled when reading or resetting the IV values.   
  * @retval None
  */
void HAL_CRYPEx_Read_IVRegisters(CRYP_HandleTypeDef *hcryp, uint8_t* Output)
{
  uint32_t outputaddr = (uint32_t)Output;
    
  *(uint32_t*)(outputaddr) = __REV(hcryp->Instance->IVR3);
  outputaddr+=4;
  *(uint32_t*)(outputaddr) = __REV(hcryp->Instance->IVR2);
  outputaddr+=4;
  *(uint32_t*)(outputaddr) = __REV(hcryp->Instance->IVR1);
  outputaddr+=4;
  *(uint32_t*)(outputaddr) = __REV(hcryp->Instance->IVR0);
}

/**
  * @brief  In case of message processing resumption, rewrite the Initialization
  *         Vector in the AES_IVRx registers.
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module.      
  * @param  Input: Pointer to the buffer containing the saved Initialization Vector to
  *         write back in the CRYP hardware block. 
  * @note   Applicable to all chaining modes.       
  * @note   AES must be disabled when reading or resetting the IV values.     
  * @retval None
  */
void HAL_CRYPEx_Write_IVRegisters(CRYP_HandleTypeDef *hcryp, uint8_t* Input)
{
  uint32_t ivaddr = (uint32_t)Input;
  
  hcryp->Instance->IVR3 = __REV(*(uint32_t*)(ivaddr));
  ivaddr+=4;
  hcryp->Instance->IVR2 = __REV(*(uint32_t*)(ivaddr));
  ivaddr+=4;
  hcryp->Instance->IVR1 = __REV(*(uint32_t*)(ivaddr));
  ivaddr+=4;
  hcryp->Instance->IVR0 = __REV(*(uint32_t*)(ivaddr));
}


/**
  * @brief  In case of message GCM/GMAC or CMAC processing suspension, read the Suspend Registers.
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module.  
  * @param  Output: Pointer to the buffer containing the saved Suspend Registers.
  * @note   These values have to be stored for reuse by writing back the AES_SUSPxR registers
  *         as soon as the interrupted processing has to be resumed.       
  * @retval None
  */
void HAL_CRYPEx_Read_SuspendRegisters(CRYP_HandleTypeDef *hcryp, uint8_t* Output)
{
  uint32_t outputaddr = (uint32_t)Output;
    
  *(uint32_t*)(outputaddr) = __REV(hcryp->Instance->SUSP7R);
  outputaddr+=4;
  *(uint32_t*)(outputaddr) = __REV(hcryp->Instance->SUSP6R);
  outputaddr+=4;
  *(uint32_t*)(outputaddr) = __REV(hcryp->Instance->SUSP5R);
  outputaddr+=4;
  *(uint32_t*)(outputaddr) = __REV(hcryp->Instance->SUSP4R);
  outputaddr+=4;
  *(uint32_t*)(outputaddr) = __REV(hcryp->Instance->SUSP3R);
  outputaddr+=4;
  *(uint32_t*)(outputaddr) = __REV(hcryp->Instance->SUSP2R);
  outputaddr+=4;
  *(uint32_t*)(outputaddr) = __REV(hcryp->Instance->SUSP1R); 
  outputaddr+=4;
  *(uint32_t*)(outputaddr) = __REV(hcryp->Instance->SUSP0R);   
}

/**
  * @brief  In case of message GCM/GMAC or CMAC processing resumption, rewrite the Suspend
  *         Registers in the AES_SUSPxR registers.
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module.    
  * @param  Input: Pointer to the buffer containing the saved suspend registers to
  *         write back in the CRYP hardware block. 
  * @retval None
  */
void HAL_CRYPEx_Write_SuspendRegisters(CRYP_HandleTypeDef *hcryp, uint8_t* Input)
{
  uint32_t ivaddr = (uint32_t)Input;
  
  hcryp->Instance->SUSP7R = __REV(*(uint32_t*)(ivaddr));
  ivaddr+=4;
  hcryp->Instance->SUSP6R = __REV(*(uint32_t*)(ivaddr));
  ivaddr+=4;
  hcryp->Instance->SUSP5R = __REV(*(uint32_t*)(ivaddr));
  ivaddr+=4;
  hcryp->Instance->SUSP4R = __REV(*(uint32_t*)(ivaddr));
  ivaddr+=4;
  hcryp->Instance->SUSP3R = __REV(*(uint32_t*)(ivaddr));
  ivaddr+=4;
  hcryp->Instance->SUSP2R = __REV(*(uint32_t*)(ivaddr));
  ivaddr+=4;
  hcryp->Instance->SUSP1R = __REV(*(uint32_t*)(ivaddr));
  ivaddr+=4;
  hcryp->Instance->SUSP0R = __REV(*(uint32_t*)(ivaddr));  
}


/**
  * @brief  In case of message GCM/GMAC or CMAC processing suspension, read the Key Registers.
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module.   
  * @param  Output: Pointer to the buffer containing the saved Key Registers. 
  * @param  KeySize: Indicates the key size (128 or 256 bits).
  * @note   These values have to be stored for reuse by writing back the AES_KEYRx registers
  *         as soon as the interrupted processing has to be resumed.           
  * @retval None
  */
void HAL_CRYPEx_Read_KeyRegisters(CRYP_HandleTypeDef *hcryp, uint8_t* Output, uint32_t KeySize)
{
  uint32_t keyaddr = (uint32_t)Output;
  
  if (KeySize == CRYP_KEYSIZE_256B)
  {
    *(uint32_t*)(keyaddr) = __REV(hcryp->Instance->KEYR7);
    keyaddr+=4;
    *(uint32_t*)(keyaddr) = __REV(hcryp->Instance->KEYR6);
    keyaddr+=4;
    *(uint32_t*)(keyaddr) = __REV(hcryp->Instance->KEYR5);
    keyaddr+=4;
    *(uint32_t*)(keyaddr) = __REV(hcryp->Instance->KEYR4);
    keyaddr+=4;                 
  }  
  
  *(uint32_t*)(keyaddr) = __REV(hcryp->Instance->KEYR3);
  keyaddr+=4;
  *(uint32_t*)(keyaddr) = __REV(hcryp->Instance->KEYR2);
  keyaddr+=4;
  *(uint32_t*)(keyaddr) = __REV(hcryp->Instance->KEYR1);
  keyaddr+=4;
  *(uint32_t*)(keyaddr) = __REV(hcryp->Instance->KEYR0); 
}

/**
  * @brief  In case of message GCM/GMAC or CMAC processing resumption, rewrite the Key
  *         Registers in the AES_KEYRx registers.
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module.   
  * @param  Input: Pointer to the buffer containing the saved key registers to
  *         write back in the CRYP hardware block. 
  * @param  KeySize: Indicates the key size (128 or 256 bits)     
  * @retval None
  */
void HAL_CRYPEx_Write_KeyRegisters(CRYP_HandleTypeDef *hcryp, uint8_t* Input, uint32_t KeySize)
{  
  uint32_t keyaddr = (uint32_t)Input;
  
  if (KeySize == CRYP_KEYSIZE_256B)
  {
    hcryp->Instance->KEYR7 = __REV(*(uint32_t*)(keyaddr));
    keyaddr+=4;
    hcryp->Instance->KEYR6 = __REV(*(uint32_t*)(keyaddr));
    keyaddr+=4;
    hcryp->Instance->KEYR5 = __REV(*(uint32_t*)(keyaddr));
    keyaddr+=4;
    hcryp->Instance->KEYR4 = __REV(*(uint32_t*)(keyaddr));
    keyaddr+=4;      
  }  
  
    hcryp->Instance->KEYR3 = __REV(*(uint32_t*)(keyaddr));
    keyaddr+=4;
    hcryp->Instance->KEYR2 = __REV(*(uint32_t*)(keyaddr));
    keyaddr+=4;
    hcryp->Instance->KEYR1 = __REV(*(uint32_t*)(keyaddr));
    keyaddr+=4;
    hcryp->Instance->KEYR0 = __REV(*(uint32_t*)(keyaddr));    
}


/**
  * @brief  In case of message GCM/GMAC or CMAC processing suspension, read the Control Register.
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module.   
  * @param  Output: Pointer to the buffer containing the saved Control Register.
  * @note   This values has to be stored for reuse by writing back the AES_CR register
  *         as soon as the interrupted processing has to be resumed.          
  * @retval None
  */
void HAL_CRYPEx_Read_ControlRegister(CRYP_HandleTypeDef *hcryp, uint8_t* Output)
{
  *(uint32_t*)(Output) = hcryp->Instance->CR;    
}

/**
  * @brief  In case of message GCM/GMAC or CMAC processing resumption, rewrite the Control
  *         Registers in the AES_CR register.
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module.   
  * @param  Input: Pointer to the buffer containing the saved Control Register to
  *         write back in the CRYP hardware block.   
  * @retval None
  */
void HAL_CRYPEx_Write_ControlRegister(CRYP_HandleTypeDef *hcryp, uint8_t* Input)
{  
  hcryp->Instance->CR = *(uint32_t*)(Input);
  /* At the same time, set handle state back to READY to be able to resume the AES calculations 
     without the processing APIs returning HAL_BUSY when called. */
  hcryp->State        = HAL_CRYP_STATE_READY;
}

/**
  * @brief  Request CRYP processing suspension when in polling or interruption mode.
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module. 
  * @note   Set the handle field SuspendRequest to the appropriate value so that 
  *         the on-going CRYP processing is suspended as soon as the required 
  *         conditions are met.
  * @note   It is advised not to suspend the CRYP processing when the DMA controller 
  *         is managing the data transfer     
  * @retval None
  */
void HAL_CRYPEx_ProcessSuspend(CRYP_HandleTypeDef *hcryp)  
{
  /* Set Handle Suspend Request field */
  hcryp->SuspendRequest = HAL_CRYP_SUSPEND;
}

/**
  * @}
  */

/**
  * @}
  */

/** @addtogroup CRYPEx_Private_Functions
  * @{
  */

/**
  * @brief  DMA CRYP Input Data process complete callback
  *         for GCM, GMAC or CMAC chainging modes.
  * @note   Specific setting of hcryp fields are required only
  *         in the case of header phase where no output data DMA
  *         transfer is on-going (only input data transfer is enabled
  *         in such a case).      
  * @param  hdma: DMA handle.
  * @retval None
  */
static void CRYP_GCMCMAC_DMAInCplt(DMA_HandleTypeDef *hdma)  
{
  CRYP_HandleTypeDef* hcryp = (CRYP_HandleTypeDef*)((DMA_HandleTypeDef*)hdma)->Parent;
  
  /* Disable the DMA transfer for input request  */
  CLEAR_BIT(hcryp->Instance->CR, AES_CR_DMAINEN);
  
  if (hcryp->Init.GCMCMACPhase == CRYP_GCMCMAC_HEADER_PHASE)
  {    
    /* Wait for CCF flag to be raised */
    if(CRYP_WaitOnCCFlag(hcryp, CRYP_CCF_TIMEOUTVALUE) != HAL_OK)  
    { 
      /* In case of time out, update hcryp->State to flag the issue */
      hcryp->State = HAL_CRYP_STATE_TIMEOUT ;        
      __HAL_UNLOCK(hcryp);
    } 
    else
    {   
      /* Clear CCF Flag */
      __HAL_CRYP_CLEAR_FLAG(CRYP_CCF_CLEAR); 
      /* Change the CRYP state */
      hcryp->State = HAL_CRYP_STATE_READY;
   
      /* Mark that the header phase is over */
      hcryp->Phase = HAL_CRYP_PHASE_HEADER_OVER;
    }
  }
  
  /* Call input data transfer complete callback */
  HAL_CRYP_InCpltCallback(hcryp);
}

/**
  * @brief  DMA CRYP Output Data process complete callback
  *         for GCM, GMAC or CMAC chainging modes.
  * @note   This callback is called only in the payload phase.  
  * @param  hdma: DMA handle.
  * @retval None
  */
static void CRYP_GCMCMAC_DMAOutCplt(DMA_HandleTypeDef *hdma)
{
  CRYP_HandleTypeDef* hcryp = (CRYP_HandleTypeDef*)((DMA_HandleTypeDef*)hdma)->Parent;
  
  /* Disable the DMA transfer for output request */
  CLEAR_BIT(hcryp->Instance->CR, AES_CR_DMAOUTEN);

  /* Clear CCF Flag */
  __HAL_CRYP_CLEAR_FLAG(CRYP_CCF_CLEAR);
  
  /* Change the CRYP state to ready */
  hcryp->State = HAL_CRYP_STATE_READY;
  /* Mark that the payload phase is over */
  hcryp->Phase = HAL_CRYP_PHASE_PAYLOAD_OVER; 
  
  /* Call output data transfer complete callback */
  HAL_CRYP_OutCpltCallback(hcryp);
}

/**
  * @brief  DMA CRYP communication error callback
  *         for GCM, GMAC or CMAC chainging modes.
  * @param  hdma: DMA handle
  * @retval None
  */
static void CRYP_GCMCMAC_DMAError(DMA_HandleTypeDef *hdma)
{
  CRYP_HandleTypeDef* hcryp = (CRYP_HandleTypeDef*)((DMA_HandleTypeDef*)hdma)->Parent;
  
  hcryp->State= HAL_CRYP_STATE_ERROR;
  hcryp->ErrorCode |= HAL_CRYP_DMA_ERROR;
  HAL_CRYP_ErrorCallback(hcryp);
  /* Clear Error Flag */
  __HAL_CRYP_CLEAR_FLAG(CRYP_ERR_CLEAR);
}



/** 
  * @brief  Handle CRYP block input/output data handling under interruption
  *         for GCM, GMAC or CMAC chainging modes.  
  * @note   The function is called under interruption only, once
  *         interruptions have been enabled by HAL_CRYPEx_AES_Auth_IT().  
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @retval HAL status
  */
HAL_StatusTypeDef CRYP_AES_Auth_IT(CRYP_HandleTypeDef *hcryp)
{
  uint32_t inputaddr   = 0x0;
  uint32_t outputaddr  = 0x0;  
  
  if(hcryp->State == HAL_CRYP_STATE_BUSY)
  {
    /*=====================*/
    /* GCM/GMAC init phase */
    /*=====================*/  
    if (hcryp->Init.GCMCMACPhase == CRYP_GCM_INIT_PHASE)
    {
      /* Clear Computation Complete Flag */
      __HAL_CRYP_CLEAR_FLAG(CRYP_CCF_CLEAR);
      /* Disable Computation Complete Flag and Errors Interrupts */
      __HAL_CRYP_DISABLE_IT(CRYP_IT_CCFIE|CRYP_IT_ERRIE);
      /* Change the CRYP state */
      hcryp->State = HAL_CRYP_STATE_READY;
    
      /* Mark that the initialization phase is over */
      hcryp->Phase = HAL_CRYP_PHASE_INIT_OVER;
          
      /* Process Unlocked */
      __HAL_UNLOCK(hcryp);
      /* Call computation complete callback */
      HAL_CRYPEx_ComputationCpltCallback(hcryp);
      return HAL_OK;
    }
    /*===============================*/
    /* GCM/GMAC or CMAC header phase */
    /*===============================*/    
    else if (hcryp->Init.GCMCMACPhase == CRYP_GCMCMAC_HEADER_PHASE)
    {
      /* Check if all input header data have been entered */
      if (hcryp->CrypInCount == 0)
      {
        /* Clear Computation Complete Flag */
        __HAL_CRYP_CLEAR_FLAG(CRYP_CCF_CLEAR);
        /* Disable Computation Complete Flag and Errors Interrupts */
        __HAL_CRYP_DISABLE_IT(CRYP_IT_CCFIE|CRYP_IT_ERRIE);
        /* Change the CRYP state */
        hcryp->State = HAL_CRYP_STATE_READY;
       /* Mark that the header phase is over */
        hcryp->Phase = HAL_CRYP_PHASE_HEADER_OVER;
      
       /* Process Unlocked */
        __HAL_UNLOCK(hcryp);
      
        /* Call computation complete callback */
        HAL_CRYPEx_ComputationCpltCallback(hcryp);
      
        return HAL_OK;
      }
      /* If suspension flag has been raised, suspend processing */
      else if (hcryp->SuspendRequest == HAL_CRYP_SUSPEND)
      {
        /* Ensure that CCF flag is set */
        if(CRYP_WaitOnCCFlag(hcryp, CRYP_CCF_TIMEOUTVALUE) != HAL_OK)  
        { 
          hcryp->State = HAL_CRYP_STATE_READY;        
          __HAL_UNLOCK(hcryp);
          return HAL_TIMEOUT;
        }
        /* Clear CCF Flag */
        __HAL_CRYP_CLEAR_FLAG(CRYP_CCF_CLEAR);
       
        /* reset SuspendRequest */
        hcryp->SuspendRequest = HAL_CRYP_SUSPEND_NONE;
        /* Disable Computation Complete Flag and Errors Interrupts */
        __HAL_CRYP_DISABLE_IT(CRYP_IT_CCFIE|CRYP_IT_ERRIE);
        /* Change the CRYP state */
        hcryp->State = HAL_CRYP_STATE_SUSPENDED;
        /* Mark that the header phase is over */
        hcryp->Phase = HAL_CRYP_PHASE_HEADER_SUSPENDED;
      
       /* Process Unlocked */
        __HAL_UNLOCK(hcryp);
      
        return HAL_OK;
      }      
      else /* Carry on feeding input data to the CRYP hardware block */
      {
        /* Clear Computation Complete Flag */
        __HAL_CRYP_CLEAR_FLAG(CRYP_CCF_CLEAR);
        /* Get the last Input data address */
        inputaddr = (uint32_t)hcryp->pCrypInBuffPtr;
      
        /* Increment/decrement instance pointer/counter */
        hcryp->pCrypInBuffPtr += 16;
        hcryp->CrypInCount -= 16;
        
        if (hcryp->Init.ChainingMode == CRYP_CHAINMODE_AES_CMAC)
        { 
          if (hcryp->CrypInCount == hcryp->Init.HeaderSize)
          {
            /* All B blocks will have been entered after the next
              four DINR writing, so point at header buffer for
              the next iteration */
            hcryp->pCrypInBuffPtr = hcryp->Init.Header;
          }
        }           
      
        /* Write the Input block in the Data Input register */
        hcryp->Instance->DINR = *(uint32_t*)(inputaddr);
        inputaddr+=4;
        hcryp->Instance->DINR = *(uint32_t*)(inputaddr);
        inputaddr+=4;
        hcryp->Instance->DINR  = *(uint32_t*)(inputaddr);
        inputaddr+=4;
        hcryp->Instance->DINR = *(uint32_t*)(inputaddr);
      
        return HAL_OK;      
      }
    }
    /*========================*/
    /* GCM/GMAC payload phase */
    /*========================*/    
    else if (hcryp->Init.GCMCMACPhase == CRYP_GCM_PAYLOAD_PHASE)
    {
      /* Get the last output data address */
      outputaddr = (uint32_t)hcryp->pCrypOutBuffPtr;
      
      /* Retrieve the last block available from the CRYP hardware block:
         read the output block from the Data Output Register */
      *(uint32_t*)(outputaddr) = hcryp->Instance->DOUTR;
      outputaddr+=4;
      *(uint32_t*)(outputaddr) = hcryp->Instance->DOUTR;
      outputaddr+=4;
      *(uint32_t*)(outputaddr) = hcryp->Instance->DOUTR;
      outputaddr+=4;
      *(uint32_t*)(outputaddr) = hcryp->Instance->DOUTR;

      /* Increment/decrement instance pointer/counter */
      hcryp->pCrypOutBuffPtr += 16;
      hcryp->CrypOutCount -= 16;            
      
      /* Check if all output text has been retrieved */
      if (hcryp->CrypOutCount == 0)
      {
        /* Clear Computation Complete Flag */
        __HAL_CRYP_CLEAR_FLAG(CRYP_CCF_CLEAR);        
        /* Disable Computation Complete Flag and Errors Interrupts */
        __HAL_CRYP_DISABLE_IT(CRYP_IT_CCFIE|CRYP_IT_ERRIE);
        /* Change the CRYP state */
        hcryp->State = HAL_CRYP_STATE_READY;
       /* Mark that the payload phase is over */
        hcryp->Phase = HAL_CRYP_PHASE_PAYLOAD_OVER;
      
       /* Process Unlocked */
        __HAL_UNLOCK(hcryp);
      
        /* Call computation complete callback */
        HAL_CRYPEx_ComputationCpltCallback(hcryp);
      
        return HAL_OK;
      }
      /* If suspension flag has been raised, suspend processing */
      else if (hcryp->SuspendRequest == HAL_CRYP_SUSPEND)
      {
        if (hcryp->Init.OperatingMode == CRYP_ALGOMODE_ENCRYPT)
        {
          /* Ensure that Busy flag is reset */
          if(CRYP_WaitOnBusyFlagReset(hcryp, CRYP_BUSY_TIMEOUTVALUE) != HAL_OK)  
          {   
            hcryp->State = HAL_CRYP_STATE_READY;        
            __HAL_UNLOCK(hcryp);
            return HAL_TIMEOUT;
          }
        }
        /* Clear CCF Flag */
        __HAL_CRYP_CLEAR_FLAG(CRYP_CCF_CLEAR);
       
        /* reset SuspendRequest */
        hcryp->SuspendRequest = HAL_CRYP_SUSPEND_NONE;
        /* Disable Computation Complete Flag and Errors Interrupts */
        __HAL_CRYP_DISABLE_IT(CRYP_IT_CCFIE|CRYP_IT_ERRIE);
        /* Change the CRYP state */
        hcryp->State = HAL_CRYP_STATE_SUSPENDED;
        /* Mark that the header phase is over */
        hcryp->Phase = HAL_CRYP_PHASE_HEADER_SUSPENDED;
      
       /* Process Unlocked */
        __HAL_UNLOCK(hcryp);
      
        return HAL_OK;
      }            
      else /* Output data are still expected, carry on feeding the CRYP
               hardware block with input data */
      {
        /* Clear Computation Complete Flag */
        __HAL_CRYP_CLEAR_FLAG(CRYP_CCF_CLEAR);          
        /* Get the last Input data address */
        inputaddr = (uint32_t)hcryp->pCrypInBuffPtr;
      
        /* Increment/decrement instance pointer/counter */
        hcryp->pCrypInBuffPtr += 16;
        hcryp->CrypInCount -= 16;
      
        /* Write the Input block in the Data Input register */
        hcryp->Instance->DINR = *(uint32_t*)(inputaddr);
        inputaddr+=4;
        hcryp->Instance->DINR = *(uint32_t*)(inputaddr);
        inputaddr+=4;
        hcryp->Instance->DINR  = *(uint32_t*)(inputaddr);
        inputaddr+=4;
        hcryp->Instance->DINR = *(uint32_t*)(inputaddr);
      
        return HAL_OK;      
      }
    }
    /*==============================*/
    /* GCM/GMAC or CMAC final phase */
    /*==============================*/    
    else if (hcryp->Init.GCMCMACPhase == CRYP_GCMCMAC_FINAL_PHASE)
    {
      /* Clear Computation Complete Flag */
      __HAL_CRYP_CLEAR_FLAG(CRYP_CCF_CLEAR);  
            
      /* Get the last output data address */
      outputaddr = (uint32_t)hcryp->pCrypOutBuffPtr;
      
      /* Retrieve the last expected data from the CRYP hardware block:
         read the output block from the Data Output Register */
      *(uint32_t*)(outputaddr) = hcryp->Instance->DOUTR;
      outputaddr+=4;
      *(uint32_t*)(outputaddr) = hcryp->Instance->DOUTR;
      outputaddr+=4;
      *(uint32_t*)(outputaddr) = hcryp->Instance->DOUTR;
      outputaddr+=4;
      *(uint32_t*)(outputaddr) = hcryp->Instance->DOUTR;
  
      /* Disable Computation Complete Flag and Errors Interrupts */
      __HAL_CRYP_DISABLE_IT(CRYP_IT_CCFIE|CRYP_IT_ERRIE);
      /* Change the CRYP state */
      hcryp->State = HAL_CRYP_STATE_READY;
      /* Mark that the header phase is over */
      hcryp->Phase = HAL_CRYP_PHASE_FINAL_OVER;
      
      /* Disable the Peripheral */
      __HAL_CRYP_DISABLE();
      /* Process Unlocked */
       __HAL_UNLOCK(hcryp);
      
      /* Call computation complete callback */
      HAL_CRYPEx_ComputationCpltCallback(hcryp);
      
      return HAL_OK;
    }
    else
    {
      /* Clear Computation Complete Flag */
      __HAL_CRYP_CLEAR_FLAG(CRYP_CCF_CLEAR);       
      hcryp->State = HAL_CRYP_STATE_ERROR; 
      __HAL_UNLOCK(hcryp); 
      return HAL_ERROR; 
    }
  }
  else
  {
    return HAL_BUSY; 
  }  
}
    
  
  
/** 
  * @brief  Set the DMA configuration and start the DMA transfer
  *         for GCM, GMAC or CMAC chainging modes.   
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module.
  * @param  inputaddr: Address of the Input buffer.
  * @param  Size: Size of the Input buffer un bytes, must be a multiple of 16.
  * @param  outputaddr: Address of the Output buffer, null pointer when no output DMA stream
  *         has to be configured.  
  * @retval None
  */
static void CRYP_GCMCMAC_SetDMAConfig(CRYP_HandleTypeDef *hcryp, uint32_t inputaddr, uint16_t Size, uint32_t outputaddr)
{

  /* Set the input CRYP DMA transfer complete callback */
  hcryp->hdmain->XferCpltCallback = CRYP_GCMCMAC_DMAInCplt;
  /* Set the DMA error callback */
  hcryp->hdmain->XferErrorCallback = CRYP_GCMCMAC_DMAError;
  
  if (outputaddr != 0) 
  { 
    /* Set the output CRYP DMA transfer complete callback */
    hcryp->hdmaout->XferCpltCallback = CRYP_GCMCMAC_DMAOutCplt;
    /* Set the DMA error callback */
    hcryp->hdmaout->XferErrorCallback = CRYP_GCMCMAC_DMAError;
  }
  
  /* Enable the CRYP peripheral */
  __HAL_CRYP_ENABLE();
  
  /* Enable the DMA input stream */
  HAL_DMA_Start_IT(hcryp->hdmain, inputaddr, (uint32_t)&hcryp->Instance->DINR, Size/4);
  
  /* Enable the DMA input request */
  SET_BIT(hcryp->Instance->CR, AES_CR_DMAINEN);

  
  if (outputaddr != 0) 
  {   
    /* Enable the DMA output stream */
    HAL_DMA_Start_IT(hcryp->hdmaout, (uint32_t)&hcryp->Instance->DOUTR, outputaddr, Size/4);
  
    /* Enable the DMA output request */
    SET_BIT(hcryp->Instance->CR, AES_CR_DMAOUTEN);
  }
}  



/**
  * @brief  Write/read input/output data in polling mode.
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module.
  * @param  Input: Pointer to the Input buffer.
  * @param  Ilength: Length of the Input buffer in bytes, must be a multiple of 16.
  * @param  Output: Pointer to the returned buffer.
  * @param  Timeout: Specify Timeout value.  
  * @retval HAL status
  */
static HAL_StatusTypeDef CRYP_ProcessData(CRYP_HandleTypeDef *hcryp, uint8_t* Input, uint16_t Ilength, uint8_t* Output, uint32_t Timeout)
{
  uint32_t index = 0;
  uint32_t inputaddr  = (uint32_t)Input;
  uint32_t outputaddr = (uint32_t)Output;
  

  for(index=0; (index < Ilength); index += 16)
  {
    /* Write the Input block in the Data Input register */
    hcryp->Instance->DINR = *(uint32_t*)(inputaddr);
    inputaddr+=4;
    hcryp->Instance->DINR = *(uint32_t*)(inputaddr);
    inputaddr+=4;
    hcryp->Instance->DINR  = *(uint32_t*)(inputaddr);
    inputaddr+=4;
    hcryp->Instance->DINR = *(uint32_t*)(inputaddr);
    inputaddr+=4;
    
    /* Wait for CCF flag to be raised */
    if(CRYP_WaitOnCCFlag(hcryp, Timeout) != HAL_OK)  
    { 
      hcryp->State = HAL_CRYP_STATE_READY;        
      __HAL_UNLOCK(hcryp);
      return HAL_TIMEOUT;
    }
      
    /* Clear CCF Flag */
    __HAL_CRYP_CLEAR_FLAG(CRYP_CCF_CLEAR);
    
    /* Read the Output block from the Data Output Register */
    *(uint32_t*)(outputaddr) = hcryp->Instance->DOUTR;
    outputaddr+=4;
    *(uint32_t*)(outputaddr) = hcryp->Instance->DOUTR;
    outputaddr+=4;
    *(uint32_t*)(outputaddr) = hcryp->Instance->DOUTR;
    outputaddr+=4;
    *(uint32_t*)(outputaddr) = hcryp->Instance->DOUTR;
    outputaddr+=4;
    
    /* If the suspension flag has been raised and if the processing is not about
       to end, suspend processing */
    if ((hcryp->SuspendRequest == HAL_CRYP_SUSPEND) && ((index+16) < Ilength))
    {
      /* Reset SuspendRequest */
      hcryp->SuspendRequest = HAL_CRYP_SUSPEND_NONE;
      
      /* Save current reading and writing locations of Input and Output buffers */
      hcryp->pCrypOutBuffPtr =  (uint8_t *)outputaddr;
      hcryp->pCrypInBuffPtr  =  (uint8_t *)inputaddr;
      /* Save the number of bytes that remain to be processed at this point */
      hcryp->CrypInCount     =  Ilength - (index+16);
      
      /* Change the CRYP state */
      hcryp->State = HAL_CRYP_STATE_SUSPENDED;
      
      return HAL_OK;
    }
    
    
  }
  /* Return function status */
  return HAL_OK;

}





/**
  * @brief  Read derivative key in polling mode when CRYP hardware block is set
  *         in key derivation operating mode (mode 2).
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module.
  * @param  Output: Pointer to the returned buffer.
  * @param  Timeout: Specify Timeout value.  
  * @retval HAL status
  */
static HAL_StatusTypeDef CRYP_ReadKey(CRYP_HandleTypeDef *hcryp, uint8_t* Output, uint32_t Timeout)
{
  uint32_t outputaddr = (uint32_t)Output;
  
  /* Wait for CCF flag to be raised */  
  if(CRYP_WaitOnCCFlag(hcryp, Timeout) != HAL_OK)  
  { 
    hcryp->State = HAL_CRYP_STATE_READY;        
    __HAL_UNLOCK(hcryp);
    return HAL_TIMEOUT;
  }
  /* Clear CCF Flag */
  __HAL_CRYP_CLEAR_FLAG( CRYP_CCF_CLEAR);
  
    /* Read the derivative key from the AES_KEYRx registers */
  if (hcryp->Init.KeySize == CRYP_KEYSIZE_256B)
  {   
    *(uint32_t*)(outputaddr) = __REV(hcryp->Instance->KEYR7);
    outputaddr+=4;
    *(uint32_t*)(outputaddr) = __REV(hcryp->Instance->KEYR6);
    outputaddr+=4;
    *(uint32_t*)(outputaddr) = __REV(hcryp->Instance->KEYR5);
    outputaddr+=4;
    *(uint32_t*)(outputaddr) = __REV(hcryp->Instance->KEYR4);
    outputaddr+=4;
  }
  
    *(uint32_t*)(outputaddr) = __REV(hcryp->Instance->KEYR3);
    outputaddr+=4;
    *(uint32_t*)(outputaddr) = __REV(hcryp->Instance->KEYR2);
    outputaddr+=4;
    *(uint32_t*)(outputaddr) = __REV(hcryp->Instance->KEYR1);
    outputaddr+=4;
    *(uint32_t*)(outputaddr) = __REV(hcryp->Instance->KEYR0);

    
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Set the DMA configuration and start the DMA transfer.
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module.
  * @param  inputaddr: Address of the Input buffer.
  * @param  Size: Size of the Input buffer in bytes, must be a multiple of 16.
  * @param  outputaddr: Address of the Output buffer.
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

  /* Enable the DMA input stream */
  HAL_DMA_Start_IT(hcryp->hdmain, inputaddr, (uint32_t)&hcryp->Instance->DINR, Size/4);

  /* Enable the DMA output stream */
  HAL_DMA_Start_IT(hcryp->hdmaout, (uint32_t)&hcryp->Instance->DOUTR, outputaddr, Size/4);

  /* Enable In and Out DMA requests */
  SET_BIT(hcryp->Instance->CR, (AES_CR_DMAINEN | AES_CR_DMAOUTEN));
  
  /* Enable the CRYP peripheral */
  __HAL_CRYP_ENABLE();
}


/**
  * @brief  Handle CRYP hardware block Timeout when waiting for CCF flag to be raised.
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module.      
  * @param  Timeout: Timeout duration.
  * @retval HAL status
  */
static HAL_StatusTypeDef CRYP_WaitOnCCFlag(CRYP_HandleTypeDef *hcryp, uint32_t Timeout)
{
  uint32_t tickstart = 0;
  
  /* Get timeout */
  tickstart = HAL_GetTick();
  
  while(HAL_IS_BIT_CLR(hcryp->Instance->SR, AES_SR_CCF))
  {    
    /* Check for the Timeout */
    if(Timeout != HAL_MAX_DELAY)
    {
      if((HAL_GetTick() - tickstart ) > Timeout)
      {    
        return HAL_TIMEOUT;
      }
    }
  }
  return HAL_OK; 
}

/**
  * @brief  Wait for Busy Flag to be reset during a GCM payload encryption process suspension. 
  * @param  hcryp: pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module.         
  * @param  Timeout: Timeout duration.
  * @retval HAL status
  */
static HAL_StatusTypeDef CRYP_WaitOnBusyFlagReset(CRYP_HandleTypeDef *hcryp, uint32_t Timeout)
{
  uint32_t tickstart = 0;
  
  /* Get timeout */
  tickstart = HAL_GetTick();
  
  while(HAL_IS_BIT_SET(hcryp->Instance->SR, AES_SR_BUSY))
  {    
    /* Check for the Timeout */
    if(Timeout != HAL_MAX_DELAY)
    {
      if((HAL_GetTick() - tickstart ) > Timeout)
      {    
        return HAL_TIMEOUT;
      }
    }
  }
  return HAL_OK; 
}


/**
  * @brief  DMA CRYP Input Data process complete callback.
  * @param  hdma: DMA handle.
  * @retval None
  */
static void CRYP_DMAInCplt(DMA_HandleTypeDef *hdma)  
{
  CRYP_HandleTypeDef* hcryp = (CRYP_HandleTypeDef*)((DMA_HandleTypeDef*)hdma)->Parent;
  
  /* Disable the DMA transfer for input request  */
  CLEAR_BIT(hcryp->Instance->CR, AES_CR_DMAINEN);
  
  /* Call input data transfer complete callback */
  HAL_CRYP_InCpltCallback(hcryp);
}

/**
  * @brief  DMA CRYP Output Data process complete callback.
  * @param  hdma: DMA handle.
  * @retval None
  */
static void CRYP_DMAOutCplt(DMA_HandleTypeDef *hdma)
{
  CRYP_HandleTypeDef* hcryp = (CRYP_HandleTypeDef*)((DMA_HandleTypeDef*)hdma)->Parent;
  
  /* Disable the DMA transfer for output request */
  CLEAR_BIT(hcryp->Instance->CR, AES_CR_DMAOUTEN);

  /* Clear CCF Flag */
  __HAL_CRYP_CLEAR_FLAG(CRYP_CCF_CLEAR);

  /* Disable CRYP */
  __HAL_CRYP_DISABLE();
  
  /* Change the CRYP state to ready */
  hcryp->State = HAL_CRYP_STATE_READY;
  
  /* Call output data transfer complete callback */
  HAL_CRYP_OutCpltCallback(hcryp);
}

/**
  * @brief  DMA CRYP communication error callback. 
  * @param  hdma: DMA handle.
  * @retval None
  */
static void CRYP_DMAError(DMA_HandleTypeDef *hdma)
{
  CRYP_HandleTypeDef* hcryp = (CRYP_HandleTypeDef*)((DMA_HandleTypeDef*)hdma)->Parent;
  
  hcryp->State= HAL_CRYP_STATE_ERROR;
  hcryp->ErrorCode |= HAL_CRYP_DMA_ERROR;  
  HAL_CRYP_ErrorCallback(hcryp);
  /* Clear Error Flag */
  __HAL_CRYP_CLEAR_FLAG(CRYP_ERR_CLEAR);
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

#endif /* defined(STM32L485xx) || defined(STM32L486xx) */

#endif /* HAL_CRYP_MODULE_ENABLED */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
