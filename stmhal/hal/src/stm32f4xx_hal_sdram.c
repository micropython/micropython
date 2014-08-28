/**
  ******************************************************************************
  * @file    stm32f4xx_hal_sdram.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-June-2014
  * @brief   SDRAM HAL module driver.
  *          This file provides a generic firmware to drive SDRAM memories mounted 
  *          as external device.
  *         
  @verbatim
  ==============================================================================
                       ##### How to use this driver #####
  ============================================================================== 
  [..]
    This driver is a generic layered driver which contains a set of APIs used to 
    control SDRAM memories. It uses the FMC layer functions to interface 
    with SDRAM devices.  
    The following sequence should be followed to configure the FMC to interface
    with SDRAM memories: 
      
   (#) Declare a SDRAM_HandleTypeDef handle structure, for example:
          SDRAM_HandleTypeDef  hdsram 
          
       (++) Fill the SDRAM_HandleTypeDef handle "Init" field with the allowed 
            values of the structure member.
            
       (++) Fill the SDRAM_HandleTypeDef handle "Instance" field with a predefined 
            base register instance for NOR or SDRAM device 
             
   (#) Declare a FMC_SDRAM_TimingTypeDef structure; for example:
          FMC_SDRAM_TimingTypeDef  Timing;
      and fill its fields with the allowed values of the structure member.
      
   (#) Initialize the SDRAM Controller by calling the function HAL_SDRAM_Init(). This function
       performs the following sequence:
          
       (##) MSP hardware layer configuration using the function HAL_SDRAM_MspInit()
       (##) Control register configuration using the FMC SDRAM interface function 
            FMC_SDRAM_Init()
       (##) Timing register configuration using the FMC SDRAM interface function 
            FMC_SDRAM_Timing_Init()
       (##) Program the SDRAM external device by applying its initialization sequence
            according to the device plugged in your hardware. This step is mandatory
            for accessing the SDRAM device.   

   (#) At this stage you can perform read/write accesses from/to the memory connected 
       to the SDRAM Bank. You can perform either polling or DMA transfer using the
       following APIs:
       (++) HAL_SDRAM_Read()/HAL_SDRAM_Write() for polling read/write access
       (++) HAL_SDRAM_Read_DMA()/HAL_SDRAM_Write_DMA() for DMA read/write transfer
       
   (#) You can also control the SDRAM device by calling the control APIs HAL_SDRAM_WriteOperation_Enable()/
       HAL_SDRAM_WriteOperation_Disable() to respectively enable/disable the SDRAM write operation or 
       the function HAL_SDRAM_SendCommand() to send a specified command to the SDRAM
       device. The command to be sent must be configured with the FMC_SDRAM_CommandTypeDef 
       structure.   
       
   (#) You can continuously monitor the SDRAM device HAL state by calling the function
       HAL_SDRAM_GetState()         
      
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

/** @defgroup SDRAM 
  * @brief SDRAM driver modules
  * @{
  */
#ifdef HAL_SDRAM_MODULE_ENABLED
#if defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx)

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/    
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/** @defgroup SDRAM_Private_Functions
  * @{
  */

/** @defgroup SDRAM_Group1 Initialization and de-initialization functions 
  * @brief    Initialization and Configuration functions 
  *
  @verbatim    
  ==============================================================================
           ##### SDRAM Initialization and de_initialization functions #####
  ==============================================================================
  [..]  
    This section provides functions allowing to initialize/de-initialize
    the SDRAM memory
  
@endverbatim
  * @{
  */
    
/**
  * @brief  Performs the SDRAM device initialization sequence.
  * @param  hsdram: pointer to a SDRAM_HandleTypeDef structure that contains
  *                the configuration information for SDRAM module.
  * @param  Timing: Pointer to SDRAM control timing structure 
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SDRAM_Init(SDRAM_HandleTypeDef *hsdram, FMC_SDRAM_TimingTypeDef *Timing)
{   
  /* Check the SDRAM handle parameter */
  if(hsdram == NULL)
  {
    return HAL_ERROR;
  }
  
  if(hsdram->State == HAL_SDRAM_STATE_RESET)
  {  
    /* Initialize the low level hardware (MSP) */
    HAL_SDRAM_MspInit(hsdram);
  }
  
  /* Initialize the SDRAM controller state */
  hsdram->State = HAL_SDRAM_STATE_BUSY;
  
  /* Initialize SDRAM control Interface */
  FMC_SDRAM_Init(hsdram->Instance, &(hsdram->Init));
  
  /* Initialize SDRAM timing Interface */
  FMC_SDRAM_Timing_Init(hsdram->Instance, Timing, hsdram->Init.SDBank); 
  
  /* Update the SDRAM controller state */
  hsdram->State = HAL_SDRAM_STATE_READY;
  
  return HAL_OK;
}

/**
  * @brief  Perform the SDRAM device initialization sequence.
  * @param  hsdram: pointer to a SDRAM_HandleTypeDef structure that contains
  *                the configuration information for SDRAM module.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SDRAM_DeInit(SDRAM_HandleTypeDef *hsdram)
{
  /* Initialize the low level hardware (MSP) */
  HAL_SDRAM_MspDeInit(hsdram);

  /* Configure the SDRAM registers with their reset values */
  FMC_SDRAM_DeInit(hsdram->Instance, hsdram->Init.SDBank);

  /* Reset the SDRAM controller state */
  hsdram->State = HAL_SDRAM_STATE_RESET;

  /* Release Lock */
  __HAL_UNLOCK(hsdram);

  return HAL_OK;
}

/**
  * @brief  SDRAM MSP Init.
  * @param  hsdram: pointer to a SDRAM_HandleTypeDef structure that contains
  *                the configuration information for SDRAM module.
  * @retval None
  */
__weak void HAL_SDRAM_MspInit(SDRAM_HandleTypeDef *hsdram)
{
  /* NOTE: This function Should not be modified, when the callback is needed,
            the HAL_SDRAM_MspInit could be implemented in the user file
   */ 
}

/**
  * @brief  SDRAM MSP DeInit.
  * @param  hsdram: pointer to a SDRAM_HandleTypeDef structure that contains
  *                the configuration information for SDRAM module.
  * @retval None
  */
__weak void HAL_SDRAM_MspDeInit(SDRAM_HandleTypeDef *hsdram)
{
  /* NOTE: This function Should not be modified, when the callback is needed,
            the HAL_SDRAM_MspDeInit could be implemented in the user file
   */ 
}

/**
  * @brief  This function handles SDRAM refresh error interrupt request.
  * @param  hsdram: pointer to a SDRAM_HandleTypeDef structure that contains
  *                the configuration information for SDRAM module.
  * @retval HAL status
*/
void HAL_SDRAM_IRQHandler(SDRAM_HandleTypeDef *hsdram)
{
  /* Check SDRAM interrupt Rising edge flag */
  if(__FMC_SDRAM_GET_FLAG(hsdram->Instance, FMC_SDRAM_FLAG_REFRESH_IT))
  {
    /* SDRAM refresh error interrupt callback */
    HAL_SDRAM_RefreshErrorCallback(hsdram);
    
    /* Clear SDRAM refresh error interrupt pending bit */
    __FMC_SDRAM_CLEAR_FLAG(hsdram->Instance, FMC_SDRAM_FLAG_REFRESH_ERROR);
  }
}

/**
  * @brief  SDRAM Refresh error callback.
  * @param  hsdram: pointer to a SDRAM_HandleTypeDef structure that contains
  *                the configuration information for SDRAM module. 
  * @retval None
  */
__weak void HAL_SDRAM_RefreshErrorCallback(SDRAM_HandleTypeDef *hsdram)
{
  /* NOTE: This function Should not be modified, when the callback is needed,
            the HAL_SDRAM_RefreshErrorCallback could be implemented in the user file
   */ 
}

/**
  * @brief  DMA transfer complete callback.
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *                the configuration information for the specified DMA module.
  * @retval None
  */
__weak void HAL_SDRAM_DMA_XferCpltCallback(DMA_HandleTypeDef *hdma)
{
  /* NOTE: This function Should not be modified, when the callback is needed,
            the HAL_SDRAM_DMA_XferCpltCallback could be implemented in the user file
   */ 
}

/**
  * @brief  DMA transfer complete error callback.
  * @param  hdma: DMA handle
  * @retval None
  */
__weak void HAL_SDRAM_DMA_XferErrorCallback(DMA_HandleTypeDef *hdma)
{
  /* NOTE: This function Should not be modified, when the callback is needed,
            the HAL_SDRAM_DMA_XferErrorCallback could be implemented in the user file
   */ 
}

/**
  * @}
  */

/** @defgroup SDRAM_Group2 Input and Output functions 
  * @brief    Input Output and memory control functions 
  *
  @verbatim    
  ==============================================================================
                    ##### SDRAM Input and Output functions #####
  ==============================================================================
  [..]  
    This section provides functions allowing to use and control the SDRAM memory
  
@endverbatim
  * @{
  */

/**
  * @brief  Reads 8-bit data buffer from the SDRAM memory.
  * @param  hsdram: pointer to a SDRAM_HandleTypeDef structure that contains
  *                the configuration information for SDRAM module.
  * @param  pAddress: Pointer to read start address
  * @param  pDstBuffer: Pointer to destination buffer  
  * @param  BufferSize: Size of the buffer to read from memory
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SDRAM_Read_8b(SDRAM_HandleTypeDef *hsdram, uint32_t *pAddress, uint8_t *pDstBuffer, uint32_t BufferSize)
{
  __IO uint8_t *pSdramAddress = (uint8_t *)pAddress;
  
  /* Process Locked */
  __HAL_LOCK(hsdram);
  
  /* Check the SDRAM controller state */
  if(hsdram->State == HAL_SDRAM_STATE_BUSY)
  {
    return HAL_BUSY;
  }
  else if(hsdram->State == HAL_SDRAM_STATE_PRECHARGED)
  {
    return  HAL_ERROR; 
  }  
  
  /* Read data from source */
  for(; BufferSize != 0; BufferSize--)
  {
    *pDstBuffer = *(__IO uint8_t *)pSdramAddress;  
    pDstBuffer++;
    pSdramAddress++;
  }
  
  /* Process Unlocked */
  __HAL_UNLOCK(hsdram);
  
  return HAL_OK; 
}


/**
  * @brief  Writes 8-bit data buffer to SDRAM memory.
  * @param  hsdram: pointer to a SDRAM_HandleTypeDef structure that contains
  *                the configuration information for SDRAM module.
  * @param  pAddress: Pointer to write start address
  * @param  pSrcBuffer: Pointer to source buffer to write  
  * @param  BufferSize: Size of the buffer to write to memory
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SDRAM_Write_8b(SDRAM_HandleTypeDef *hsdram, uint32_t *pAddress, uint8_t *pSrcBuffer, uint32_t BufferSize)
{
  __IO uint8_t *pSdramAddress = (uint8_t *)pAddress;
  uint32_t tmp = 0;
  
  /* Process Locked */
  __HAL_LOCK(hsdram);
  
  /* Check the SDRAM controller state */
  tmp = hsdram->State;
  
  if(tmp == HAL_SDRAM_STATE_BUSY)
  {
    return HAL_BUSY;
  }
  else if((tmp == HAL_SDRAM_STATE_PRECHARGED) || (tmp == HAL_SDRAM_STATE_WRITE_PROTECTED))
  {
    return  HAL_ERROR; 
  }
  
  /* Write data to memory */
  for(; BufferSize != 0; BufferSize--)
  {
    *(__IO uint8_t *)pSdramAddress = *pSrcBuffer;
    pSrcBuffer++;
    pSdramAddress++;
  }
  
  /* Process Unlocked */
  __HAL_UNLOCK(hsdram);    
  
  return HAL_OK;   
}


/**
  * @brief  Reads 16-bit data buffer from the SDRAM memory. 
  * @param  hsdram: pointer to a SDRAM_HandleTypeDef structure that contains
  *                the configuration information for SDRAM module.
  * @param  pAddress: Pointer to read start address
  * @param  pDstBuffer: Pointer to destination buffer  
  * @param  BufferSize: Size of the buffer to read from memory
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SDRAM_Read_16b(SDRAM_HandleTypeDef *hsdram, uint32_t *pAddress, uint16_t *pDstBuffer, uint32_t BufferSize)
{
  __IO uint16_t *pSdramAddress = (uint16_t *)pAddress;
  
  /* Process Locked */
  __HAL_LOCK(hsdram);
  
  /* Check the SDRAM controller state */
  if(hsdram->State == HAL_SDRAM_STATE_BUSY)
  {
    return HAL_BUSY;
  }
  else if(hsdram->State == HAL_SDRAM_STATE_PRECHARGED)
  {
    return  HAL_ERROR; 
  }  
  
  /* Read data from source */
  for(; BufferSize != 0; BufferSize--)
  {
    *pDstBuffer = *(__IO uint16_t *)pSdramAddress;  
    pDstBuffer++;
    pSdramAddress++;               
  }
  
  /* Process Unlocked */
  __HAL_UNLOCK(hsdram);       
  
  return HAL_OK; 
}

/**
  * @brief  Writes 16-bit data buffer to SDRAM memory. 
  * @param  hsdram: pointer to a SDRAM_HandleTypeDef structure that contains
  *                the configuration information for SDRAM module.
  * @param  pAddress: Pointer to write start address
  * @param  pSrcBuffer: Pointer to source buffer to write  
  * @param  BufferSize: Size of the buffer to write to memory
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SDRAM_Write_16b(SDRAM_HandleTypeDef *hsdram, uint32_t *pAddress, uint16_t *pSrcBuffer, uint32_t BufferSize)
{
  __IO uint16_t *pSdramAddress = (uint16_t *)pAddress;
  uint32_t tmp = 0;
  
  /* Process Locked */
  __HAL_LOCK(hsdram);
  
  /* Check the SDRAM controller state */
  tmp = hsdram->State;
  
  if(tmp == HAL_SDRAM_STATE_BUSY)
  {
    return HAL_BUSY;
  }
  else if((tmp == HAL_SDRAM_STATE_PRECHARGED) || (tmp == HAL_SDRAM_STATE_WRITE_PROTECTED))
  {
    return  HAL_ERROR; 
  }
  
  /* Write data to memory */
  for(; BufferSize != 0; BufferSize--)
  {
    *(__IO uint16_t *)pSdramAddress = *pSrcBuffer;
    pSrcBuffer++;
    pSdramAddress++;            
  }
  
  /* Process Unlocked */
  __HAL_UNLOCK(hsdram);    
  
  return HAL_OK;   
}

/**
  * @brief  Reads 32-bit data buffer from the SDRAM memory. 
  * @param  hsdram: pointer to a SDRAM_HandleTypeDef structure that contains
  *                the configuration information for SDRAM module.
  * @param  pAddress: Pointer to read start address
  * @param  pDstBuffer: Pointer to destination buffer  
  * @param  BufferSize: Size of the buffer to read from memory
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SDRAM_Read_32b(SDRAM_HandleTypeDef *hsdram, uint32_t *pAddress, uint32_t *pDstBuffer, uint32_t BufferSize)
{
  __IO uint32_t *pSdramAddress = (uint32_t *)pAddress;
  
  /* Process Locked */
  __HAL_LOCK(hsdram);
  
  /* Check the SDRAM controller state */
  if(hsdram->State == HAL_SDRAM_STATE_BUSY)
  {
    return HAL_BUSY;
  }
  else if(hsdram->State == HAL_SDRAM_STATE_PRECHARGED)
  {
    return  HAL_ERROR; 
  }  
  
  /* Read data from source */
  for(; BufferSize != 0; BufferSize--)
  {
    *pDstBuffer = *(__IO uint32_t *)pSdramAddress;  
    pDstBuffer++;
    pSdramAddress++;               
  }
  
  /* Process Unlocked */
  __HAL_UNLOCK(hsdram);       
  
  return HAL_OK; 
}

/**
  * @brief  Writes 32-bit data buffer to SDRAM memory. 
  * @param  hsdram: pointer to a SDRAM_HandleTypeDef structure that contains
  *                the configuration information for SDRAM module.
  * @param  pAddress: Pointer to write start address
  * @param  pSrcBuffer: Pointer to source buffer to write  
  * @param  BufferSize: Size of the buffer to write to memory
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SDRAM_Write_32b(SDRAM_HandleTypeDef *hsdram, uint32_t *pAddress, uint32_t *pSrcBuffer, uint32_t BufferSize)
{
  __IO uint32_t *pSdramAddress = (uint32_t *)pAddress;
  uint32_t tmp = 0;
  
  /* Process Locked */
  __HAL_LOCK(hsdram);
  
  /* Check the SDRAM controller state */
  tmp = hsdram->State;
  
  if(tmp == HAL_SDRAM_STATE_BUSY)
  {
    return HAL_BUSY;
  }
  else if((tmp == HAL_SDRAM_STATE_PRECHARGED) || (tmp == HAL_SDRAM_STATE_WRITE_PROTECTED))
  {
    return  HAL_ERROR; 
  }
  
  /* Write data to memory */
  for(; BufferSize != 0; BufferSize--)
  {
    *(__IO uint32_t *)pSdramAddress = *pSrcBuffer;
    pSrcBuffer++;
    pSdramAddress++;          
  }
  
  /* Process Unlocked */
  __HAL_UNLOCK(hsdram);    
  
  return HAL_OK;  
}

/**
  * @brief  Reads a Words data from the SDRAM memory using DMA transfer. 
  * @param  hsdram: pointer to a SDRAM_HandleTypeDef structure that contains
  *                the configuration information for SDRAM module.
  * @param  pAddress: Pointer to read start address
  * @param  pDstBuffer: Pointer to destination buffer  
  * @param  BufferSize: Size of the buffer to read from memory
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SDRAM_Read_DMA(SDRAM_HandleTypeDef *hsdram, uint32_t *pAddress, uint32_t *pDstBuffer, uint32_t BufferSize)
{
  uint32_t tmp = 0;
    
  /* Process Locked */
  __HAL_LOCK(hsdram);
  
  /* Check the SDRAM controller state */  
  tmp = hsdram->State;
  
  if(tmp == HAL_SDRAM_STATE_BUSY)
  {
    return HAL_BUSY;
  }
  else if(tmp == HAL_SDRAM_STATE_PRECHARGED)
  {
    return  HAL_ERROR; 
  }  
  
  /* Configure DMA user callbacks */
  hsdram->hdma->XferCpltCallback  = HAL_SDRAM_DMA_XferCpltCallback;
  hsdram->hdma->XferErrorCallback = HAL_SDRAM_DMA_XferErrorCallback;
  
  /* Enable the DMA Stream */
  HAL_DMA_Start_IT(hsdram->hdma, (uint32_t)pAddress, (uint32_t)pDstBuffer, (uint32_t)BufferSize);
  
  /* Process Unlocked */
  __HAL_UNLOCK(hsdram);  
  
  return HAL_OK; 
}

/**
  * @brief  Writes a Words data buffer to SDRAM memory using DMA transfer.
  * @param  hsdram: pointer to a SDRAM_HandleTypeDef structure that contains
  *                the configuration information for SDRAM module.
  * @param  pAddress: Pointer to write start address
  * @param  pSrcBuffer: Pointer to source buffer to write  
  * @param  BufferSize: Size of the buffer to write to memory
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SDRAM_Write_DMA(SDRAM_HandleTypeDef *hsdram, uint32_t *pAddress, uint32_t *pSrcBuffer, uint32_t BufferSize)
{
  uint32_t tmp = 0;
  
  /* Process Locked */
  __HAL_LOCK(hsdram);
  
  /* Check the SDRAM controller state */  
  tmp = hsdram->State;
  
  if(tmp == HAL_SDRAM_STATE_BUSY)
  {
    return HAL_BUSY;
  }
  else if((tmp == HAL_SDRAM_STATE_PRECHARGED) || (tmp == HAL_SDRAM_STATE_WRITE_PROTECTED))
  {
    return  HAL_ERROR; 
  }  
  
  /* Configure DMA user callbacks */
  hsdram->hdma->XferCpltCallback  = HAL_SDRAM_DMA_XferCpltCallback;
  hsdram->hdma->XferErrorCallback = HAL_SDRAM_DMA_XferErrorCallback;
  
  /* Enable the DMA Stream */
  HAL_DMA_Start_IT(hsdram->hdma, (uint32_t)pSrcBuffer, (uint32_t)pAddress, (uint32_t)BufferSize);
  
  /* Process Unlocked */
  __HAL_UNLOCK(hsdram);
  
  return HAL_OK;
}

/**
  * @}
  */
  
/** @defgroup SDRAM_Group3 Control functions 
 *  @brief   management functions 
 *
@verbatim   
  ==============================================================================
                         ##### SDRAM Control functions #####
  ==============================================================================  
  [..]
    This subsection provides a set of functions allowing to control dynamically
    the SDRAM interface.

@endverbatim
  * @{
  */

/**
  * @brief  Enables dynamically SDRAM write protection.
  * @param  hsdram: pointer to a SDRAM_HandleTypeDef structure that contains
  *                the configuration information for SDRAM module.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SDRAM_WriteProtection_Enable(SDRAM_HandleTypeDef *hsdram)
{ 
  /* Check the SDRAM controller state */ 
  if(hsdram->State == HAL_SDRAM_STATE_BUSY)
  {
    return HAL_BUSY;
  }
  
  /* Update the SDRAM state */
  hsdram->State = HAL_SDRAM_STATE_BUSY;
  
  /* Enable write protection */
  FMC_SDRAM_WriteProtection_Enable(hsdram->Instance, hsdram->Init.SDBank);
  
  /* Update the SDRAM state */
  hsdram->State = HAL_SDRAM_STATE_WRITE_PROTECTED;
  
  return HAL_OK;  
}

/**
  * @brief  Disables dynamically SDRAM write protection.
  * @param  hsdram: pointer to a SDRAM_HandleTypeDef structure that contains
  *                the configuration information for SDRAM module.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SDRAM_WriteProtection_Disable(SDRAM_HandleTypeDef *hsdram)
{
  /* Check the SDRAM controller state */
  if(hsdram->State == HAL_SDRAM_STATE_BUSY)
  {
    return HAL_BUSY;
  }
  
  /* Update the SDRAM state */
  hsdram->State = HAL_SDRAM_STATE_BUSY;
  
  /* Disable write protection */
  FMC_SDRAM_WriteProtection_Disable(hsdram->Instance, hsdram->Init.SDBank);
  
  /* Update the SDRAM state */
  hsdram->State = HAL_SDRAM_STATE_READY;
  
  return HAL_OK;
}

/**
  * @brief  Sends Command to the SDRAM bank.
  * @param  hsdram: pointer to a SDRAM_HandleTypeDef structure that contains
  *                the configuration information for SDRAM module.
  * @param  Command: SDRAM command structure
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */  
HAL_StatusTypeDef HAL_SDRAM_SendCommand(SDRAM_HandleTypeDef *hsdram, FMC_SDRAM_CommandTypeDef *Command, uint32_t Timeout)
{
  /* Check the SDRAM controller state */
  if(hsdram->State == HAL_SDRAM_STATE_BUSY)
  {
    return HAL_BUSY;
  }
  
  /* Update the SDRAM state */
  hsdram->State = HAL_SDRAM_STATE_BUSY;
  
  /* Send SDRAM command */
  FMC_SDRAM_SendCommand(hsdram->Instance, Command, Timeout);
  
  /* Update the SDRAM controller state state */
  if(Command->CommandMode == FMC_SDRAM_CMD_PALL)
  {
    hsdram->State = HAL_SDRAM_STATE_PRECHARGED;
  }
  else
  {
    hsdram->State = HAL_SDRAM_STATE_READY;
  }
  
  return HAL_OK;  
}

/**
  * @brief  Programs the SDRAM Memory Refresh rate.
  * @param  hsdram: pointer to a SDRAM_HandleTypeDef structure that contains
  *                the configuration information for SDRAM module.  
  * @param  RefreshRate: The SDRAM refresh rate value       
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SDRAM_ProgramRefreshRate(SDRAM_HandleTypeDef *hsdram, uint32_t RefreshRate)
{
  /* Check the SDRAM controller state */
  if(hsdram->State == HAL_SDRAM_STATE_BUSY)
  {
    return HAL_BUSY;
  } 
  
  /* Update the SDRAM state */
  hsdram->State = HAL_SDRAM_STATE_BUSY;
  
  /* Program the refresh rate */
  FMC_SDRAM_ProgramRefreshRate(hsdram->Instance ,RefreshRate);
  
  /* Update the SDRAM state */
  hsdram->State = HAL_SDRAM_STATE_READY;
  
  return HAL_OK;   
}

/**
  * @brief  Sets the Number of consecutive SDRAM Memory auto Refresh commands.
  * @param  hsdram: pointer to a SDRAM_HandleTypeDef structure that contains
  *                the configuration information for SDRAM module.  
  * @param  AutoRefreshNumber: The SDRAM auto Refresh number       
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SDRAM_SetAutoRefreshNumber(SDRAM_HandleTypeDef *hsdram, uint32_t AutoRefreshNumber)
{
  /* Check the SDRAM controller state */
  if(hsdram->State == HAL_SDRAM_STATE_BUSY)
  {
    return HAL_BUSY;
  } 
  
  /* Update the SDRAM state */
  hsdram->State = HAL_SDRAM_STATE_BUSY;
  
  /* Set the Auto-Refresh number */
  FMC_SDRAM_SetAutoRefreshNumber(hsdram->Instance ,AutoRefreshNumber);
  
  /* Update the SDRAM state */
  hsdram->State = HAL_SDRAM_STATE_READY;
  
  return HAL_OK;
}

/**
  * @brief  Returns the SDRAM memory current mode.
  * @param  hsdram: pointer to a SDRAM_HandleTypeDef structure that contains
  *                the configuration information for SDRAM module.
  * @retval The SDRAM memory mode.        
  */
uint32_t HAL_SDRAM_GetModeStatus(SDRAM_HandleTypeDef *hsdram)
{
  /* Return the SDRAM memory current mode */
  return(FMC_SDRAM_GetModeStatus(hsdram->Instance, hsdram->Init.SDBank));
}

/**
  * @}
  */
  
/** @defgroup SDRAM_Group4 State functions 
 *  @brief   Peripheral State functions 
 *
@verbatim   
  ==============================================================================
                      ##### SDRAM State functions #####
  ==============================================================================  
  [..]
    This subsection permits to get in run-time the status of the SDRAM controller 
    and the data flow.

@endverbatim
  * @{
  */

/**
  * @brief  Returns the SDRAM state.
  * @param  hsdram: pointer to a SDRAM_HandleTypeDef structure that contains
  *                the configuration information for SDRAM module.
  * @retval HAL state
  */
HAL_SDRAM_StateTypeDef HAL_SDRAM_GetState(SDRAM_HandleTypeDef *hsdram)
{
  return hsdram->State;
}

/**
  * @}
  */    

/**
  * @}
  */
#endif /* STM32F427xx || STM32F437xx || STM32F429xx || STM32F439xx */
#endif /* HAL_SDRAM_MODULE_ENABLED */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
