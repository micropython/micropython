/**
  ******************************************************************************
  * @file    stm32f4xx_hal_dma2d.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-June-2014
  * @brief   DMA2D HAL module driver.
  *          This file provides firmware functions to manage the following 
  *          functionalities of the DMA2D peripheral:
  *           + Initialization and de-initialization functions
  *           + IO operation functions
  *           + Peripheral Control functions 
  *           + Peripheral State and Errors functions
  *           
  @verbatim                 
  ============================================================================== 
                        ##### How to use this driver #####
  ============================================================================== 
    [..]
      (#) Program the required configuration through following parameters:   
          the Transfer Mode, the output color mode and the output offset using 
          HAL_DMA2D_Init() function.

      (#) Program the required configuration through following parameters:   
          the input color mode, the input color, input alpha value, alpha mode 
          and the input offset using HAL_DMA2D_ConfigLayer() function for foreground
          or/and background layer.
          
     *** Polling mode IO operation ***
     =================================   
    [..]        
       (+) Configure the pdata, Destination and data length and Enable 
           the transfer using HAL_DMA2D_Start() 
       (+) Wait for end of transfer using HAL_DMA2D_PollForTransfer(), at this stage
           user can specify the value of timeout according to his end application.
               
     *** Interrupt mode IO operation ***    
     ===================================
     [..] 
       (#) Configure the pdata, Destination and data length and Enable 
           the transfer using HAL_DMA2D_Start_IT() 
       (#) Use HAL_DMA2D_IRQHandler() called under DMA2D_IRQHandler() Interrupt subroutine
       (#) At the end of data transfer HAL_DMA2D_IRQHandler() function is executed and user can 
           add his own function by customization of function pointer XferCpltCallback and 
           XferErrorCallback (i.e a member of DMA2D handle structure). 

         -@-   In Register-to-Memory transfer mode, the pdata parameter is the register
               color, in Memory-to-memory or memory-to-memory with pixel format
               conversion the pdata is the source address.

         -@-   Configure the foreground source address, the background source address, 
               the Destination and data length and Enable the transfer using 
               HAL_DMA2D_BlendingStart() in polling mode and HAL_DMA2D_BlendingStart_IT()
               in interrupt mode.
               
         -@-   HAL_DMA2D_BlendingStart() and HAL_DMA2D_BlendingStart_IT() functions
               are used if the memory to memory with blending transfer mode is selected.
                   
      (#) Optionally, configure and enable the CLUT using HAL_DMA2D_ConfigCLUT()
          HAL_DMA2D_EnableCLUT() functions.

      (#) Optionally, configure and enable LineInterrupt using the following function:
          HAL_DMA2D_ProgramLineEvent().
   
      (#) The transfer can be suspended, continued and aborted using the following
          functions: HAL_DMA2D_Suspend(), HAL_DMA2D_Resume(), HAL_DMA2D_Abort().
                     
      (#) To control DMA2D state you can use the following function: HAL_DMA2D_GetState()                   

     *** DMA2D HAL driver macros list ***
     ============================================= 
     [..]
       Below the list of most used macros in DMA2D HAL driver :
       
      (+) __HAL_DMA2D_ENABLE: Enable the DMA2D peripheral.
      (+) __HAL_DMA2D_DISABLE: Disable the DMA2D peripheral.
      (+) __HAL_DMA2D_GET_FLAG: Get the DMA2D pending flags.
      (+) __HAL_DMA2D_CLEAR_FLAG: Clear the DMA2D pending flags.
      (+) __HAL_DMA2D_ENABLE_IT: Enable the specified DMA2D interrupts.
      (+) __HAL_DMA2D_DISABLE_IT: Disable the specified DMA2D interrupts.
      (+) __HAL_DMA2D_GET_IT_SOURCE: Check whether the specified DMA2D interrupt has occurred or not.
     
     [..] 
      (@) You can refer to the DMA2D HAL driver header file for more useful macros
                                  
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
/** @defgroup DMA2D 
  * @brief DMA2D HAL module driver
  * @{
  */

#ifdef HAL_DMA2D_MODULE_ENABLED

#if defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx)

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define HAL_TIMEOUT_DMA2D_ABORT      ((uint32_t)1000)  /* 1s  */
#define HAL_TIMEOUT_DMA2D_SUSPEND    ((uint32_t)1000)  /* 1s  */
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void DMA2D_SetConfig(DMA2D_HandleTypeDef *hdma2d, uint32_t pdata, uint32_t DstAddress, uint32_t Width, uint32_t Heigh);

/* Private functions ---------------------------------------------------------*/

/** @defgroup DMA2D_Private_Functions
  * @{
  */

/** @defgroup DMA2D_Group1 Initialization and Configuration functions
 *  @brief   Initialization and Configuration functions
 *
@verbatim   
 ===============================================================================
                ##### Initialization and Configuration functions #####
 ===============================================================================  
    [..]  This section provides functions allowing to:
      (+) Initialize and configure the DMA2D
      (+) De-initialize the DMA2D 

@endverbatim
  * @{
  */
    
/**
  * @brief  Initializes the DMA2D according to the specified
  *         parameters in the DMA2D_InitTypeDef and create the associated handle.
  * @param  hdma2d: pointer to a DMA2D_HandleTypeDef structure that contains
  *                 the configuration information for the DMA2D.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMA2D_Init(DMA2D_HandleTypeDef *hdma2d)
{ 
  uint32_t tmp = 0;

  /* Check the DMA2D peripheral state */
  if(hdma2d == NULL)
  {
     return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_DMA2D_ALL_INSTANCE(hdma2d->Instance));
  assert_param(IS_DMA2D_MODE(hdma2d->Init.Mode));
  assert_param(IS_DMA2D_CMODE(hdma2d->Init.ColorMode));
  assert_param(IS_DMA2D_OFFSET(hdma2d->Init.OutputOffset));

  if(hdma2d->State == HAL_DMA2D_STATE_RESET)
  {
    /* Init the low level hardware */
    HAL_DMA2D_MspInit(hdma2d);
  }
  
  /* Change DMA2D peripheral state */
  hdma2d->State = HAL_DMA2D_STATE_BUSY;  

/* DMA2D CR register configuration -------------------------------------------*/
  /* Get the CR register value */
  tmp = hdma2d->Instance->CR;

  /* Clear Mode bits */
  tmp &= (uint32_t)~DMA2D_CR_MODE;

  /* Prepare the value to be wrote to the CR register */
  tmp |= hdma2d->Init.Mode;

  /* Write to DMA2D CR register */
  hdma2d->Instance->CR = tmp;

/* DMA2D OPFCCR register configuration ---------------------------------------*/
  /* Get the OPFCCR register value */
  tmp = hdma2d->Instance->OPFCCR;

  /* Clear Color Mode bits */
  tmp &= (uint32_t)~DMA2D_OPFCCR_CM;

  /* Prepare the value to be wrote to the OPFCCR register */
  tmp |= hdma2d->Init.ColorMode;

  /* Write to DMA2D OPFCCR register */
  hdma2d->Instance->OPFCCR = tmp;

/* DMA2D OOR register configuration ------------------------------------------*/  
  /* Get the OOR register value */
  tmp = hdma2d->Instance->OOR;

  /* Clear Offset bits */
  tmp &= (uint32_t)~DMA2D_OOR_LO;

  /* Prepare the value to be wrote to the OOR register */
  tmp |= hdma2d->Init.OutputOffset;

  /* Write to DMA2D OOR register */
  hdma2d->Instance->OOR = tmp;

  /* Update error code */
  hdma2d->ErrorCode = HAL_DMA2D_ERROR_NONE;

  /* Initialize the DMA2D state*/
  hdma2d->State  = HAL_DMA2D_STATE_READY;

  return HAL_OK;
}

/**
  * @brief  Deinitializes the DMA2D peripheral registers to their default reset
  *         values.
  * @param  hdma2d: pointer to a DMA2D_HandleTypeDef structure that contains
  *                 the configuration information for the DMA2D.
  * @retval None
  */

HAL_StatusTypeDef HAL_DMA2D_DeInit(DMA2D_HandleTypeDef *hdma2d)
{
  /* Check the DMA2D peripheral state */
  if(hdma2d == NULL)
  {
     return HAL_ERROR;
  }

  /* DeInit the low level hardware */
  HAL_DMA2D_MspDeInit(hdma2d);

  /* Update error code */
  hdma2d->ErrorCode = HAL_DMA2D_ERROR_NONE;

  /* Initialize the DMA2D state*/
  hdma2d->State  = HAL_DMA2D_STATE_RESET;

  /* Release Lock */
  __HAL_UNLOCK(hdma2d);

  return HAL_OK;
}

/**
  * @brief  Initializes the DMA2D MSP.
  * @param  hdma2d: pointer to a DMA2D_HandleTypeDef structure that contains
  *                 the configuration information for the DMA2D.
  * @retval None
  */
__weak void HAL_DMA2D_MspInit(DMA2D_HandleTypeDef* hdma2d)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_DMA2D_MspInit could be implemented in the user file
   */ 
}

/**
  * @brief  DeInitializes the DMA2D MSP.
  * @param  hdma2d: pointer to a DMA2D_HandleTypeDef structure that contains
  *                 the configuration information for the DMA2D.
  * @retval None
  */
__weak void HAL_DMA2D_MspDeInit(DMA2D_HandleTypeDef* hdma2d)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_DMA2D_MspDeInit could be implemented in the user file
   */ 
}

/**
  * @}
  */

/** @defgroup DMA2D_Group2 IO operation functions 
 *  @brief   IO operation functions  
 *
@verbatim   
 ===============================================================================
                      #####  IO operation functions  #####
 ===============================================================================  
    [..]  This section provides functions allowing to:
      (+) Configure the pdata, destination address and data size and 
          Start DMA2D transfer.
      (+) Configure the source for foreground and background, destination address 
          and data size and Start MultiBuffer DMA2D transfer.
      (+) Configure the pdata, destination address and data size and 
          Start DMA2D transfer with interrupt.
      (+) Configure the source for foreground and background, destination address 
          and data size and Start MultiBuffer DMA2D transfer with interrupt.
      (+) Abort DMA2D transfer.
      (+) Suspend DMA2D transfer.
      (+) Continue DMA2D transfer. 
      (+) Poll for transfer complete.
      (+) handle DMA2D interrupt request.
        
@endverbatim
  * @{
  */

/**
  * @brief  Start the DMA2D Transfer.
  * @param  hdma2d:     pointer to a DMA2D_HandleTypeDef structure that contains
  *                     the configuration information for the DMA2D.  
  * @param  pdata:      Configure the source memory Buffer address if 
  *                     the memory to memory or memory to memory with pixel format 
  *                     conversion DMA2D mode is selected, and configure 
  *                     the color value if register to memory DMA2D mode is selected.
  * @param  DstAddress: The destination memory Buffer address.
  * @param  Width:      The width of data to be transferred from source to destination.
  * @param  Heigh:      The heigh of data to be transferred from source to destination.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMA2D_Start(DMA2D_HandleTypeDef *hdma2d, uint32_t pdata, uint32_t DstAddress, uint32_t Width,  uint32_t Heigh)
{
  /* Process locked */
  __HAL_LOCK(hdma2d);

  /* Change DMA2D peripheral state */
  hdma2d->State = HAL_DMA2D_STATE_BUSY;

  /* Check the parameters */
  assert_param(IS_DMA2D_LINE(Heigh));
  assert_param(IS_DMA2D_PIXEL(Width));

  /* Disable the Peripheral */
  __HAL_DMA2D_DISABLE(hdma2d);

  /* Configure the source, destination address and the data size */
  DMA2D_SetConfig(hdma2d, pdata, DstAddress, Width, Heigh);

  /* Enable the Peripheral */
  __HAL_DMA2D_ENABLE(hdma2d);

  return HAL_OK;
}

/**
  * @brief  Start the DMA2D Transfer with interrupt enabled.
  * @param  hdma2d:     pointer to a DMA2D_HandleTypeDef structure that contains
  *                     the configuration information for the DMA2D.  
  * @param  pdata:      Configure the source memory Buffer address if 
  *                     the memory to memory or memory to memory with pixel format 
  *                     conversion DMA2D mode is selected, and configure 
  *                     the color value if register to memory DMA2D mode is selected.
  * @param  DstAddress: The destination memory Buffer address.
  * @param  Width:      The width of data to be transferred from source to destination.
  * @param  Heigh:      The heigh of data to be transferred from source to destination.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMA2D_Start_IT(DMA2D_HandleTypeDef *hdma2d, uint32_t pdata, uint32_t DstAddress, uint32_t Width,  uint32_t Heigh)
{
  /* Process locked */
  __HAL_LOCK(hdma2d);

  /* Change DMA2D peripheral state */
  hdma2d->State = HAL_DMA2D_STATE_BUSY;

  /* Check the parameters */
  assert_param(IS_DMA2D_LINE(Heigh));
  assert_param(IS_DMA2D_PIXEL(Width));

  /* Disable the Peripheral */
  __HAL_DMA2D_DISABLE(hdma2d);

  /* Configure the source, destination address and the data size */
  DMA2D_SetConfig(hdma2d, pdata, DstAddress, Width, Heigh);

  /* Enable the transfer complete interrupt */
  __HAL_DMA2D_ENABLE_IT(hdma2d, DMA2D_IT_TC);

  /* Enable the transfer Error interrupt */
  __HAL_DMA2D_ENABLE_IT(hdma2d, DMA2D_IT_TE);

  /* Enable the Peripheral */
  __HAL_DMA2D_ENABLE(hdma2d);

  /* Enable the configuration error interrupt */
  __HAL_DMA2D_ENABLE_IT(hdma2d, DMA2D_IT_CE);

  return HAL_OK;
}

/**
  * @brief  Start the multi-source DMA2D Transfer.
  * @param  hdma2d:      pointer to a DMA2D_HandleTypeDef structure that contains
  *                      the configuration information for the DMA2D.  
  * @param  SrcAddress1: The source memory Buffer address of the foreground layer.
  * @param  SrcAddress2: The source memory Buffer address of the background layer.
  * @param  DstAddress:  The destination memory Buffer address
  * @param  Width:       The width of data to be transferred from source to destination.
  * @param  Heigh:       The heigh of data to be transferred from source to destination.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMA2D_BlendingStart(DMA2D_HandleTypeDef *hdma2d, uint32_t SrcAddress1, uint32_t  SrcAddress2, uint32_t DstAddress, uint32_t Width,  uint32_t Heigh)
{
  /* Process locked */
  __HAL_LOCK(hdma2d);

  /* Change DMA2D peripheral state */
  hdma2d->State = HAL_DMA2D_STATE_BUSY; 

  /* Check the parameters */
  assert_param(IS_DMA2D_LINE(Heigh));
  assert_param(IS_DMA2D_PIXEL(Width));

  /* Disable the Peripheral */
  __HAL_DMA2D_DISABLE(hdma2d);

  /* Configure DMA2D Stream source2 address */
  hdma2d->Instance->BGMAR = SrcAddress2;

  /* Configure the source, destination address and the data size */
  DMA2D_SetConfig(hdma2d, SrcAddress1, DstAddress, Width, Heigh);

  /* Enable the Peripheral */
  __HAL_DMA2D_ENABLE(hdma2d);

  return HAL_OK;
}

/**
  * @brief  Start the multi-source DMA2D Transfer with interrupt enabled.
  * @param  hdma2d:     pointer to a DMA2D_HandleTypeDef structure that contains
  *                     the configuration information for the DMA2D.  
  * @param  SrcAddress1: The source memory Buffer address of the foreground layer.
  * @param  SrcAddress2: The source memory Buffer address of the background layer.
  * @param  DstAddress:  The destination memory Buffer address.
  * @param  Width:       The width of data to be transferred from source to destination.
  * @param  Heigh:       The heigh of data to be transferred from source to destination.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMA2D_BlendingStart_IT(DMA2D_HandleTypeDef *hdma2d, uint32_t SrcAddress1, uint32_t  SrcAddress2, uint32_t DstAddress, uint32_t Width,  uint32_t Heigh)
{
  /* Process locked */
  __HAL_LOCK(hdma2d);

  /* Change DMA2D peripheral state */
  hdma2d->State = HAL_DMA2D_STATE_BUSY;

  /* Check the parameters */
  assert_param(IS_DMA2D_LINE(Heigh));
  assert_param(IS_DMA2D_PIXEL(Width));

  /* Disable the Peripheral */
  __HAL_DMA2D_DISABLE(hdma2d);
 
  /* Configure DMA2D Stream source2 address */
  hdma2d->Instance->BGMAR = SrcAddress2;

  /* Configure the source, destination address and the data size */
  DMA2D_SetConfig(hdma2d, SrcAddress1, DstAddress, Width, Heigh);

  /* Enable the configuration error interrupt */
  __HAL_DMA2D_ENABLE_IT(hdma2d, DMA2D_IT_CE);

  /* Enable the transfer complete interrupt */
  __HAL_DMA2D_ENABLE_IT(hdma2d, DMA2D_IT_TC);

  /* Enable the transfer Error interrupt */
  __HAL_DMA2D_ENABLE_IT(hdma2d, DMA2D_IT_TE);

  /* Enable the Peripheral */
  __HAL_DMA2D_ENABLE(hdma2d);

  return HAL_OK;
}

/**
  * @brief  Abort the DMA2D Transfer.
  * @param  hdma2d : pointer to a DMA2D_HandleTypeDef structure that contains
  *                  the configuration information for the DMA2D.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMA2D_Abort(DMA2D_HandleTypeDef *hdma2d)
{
  uint32_t tickstart = 0;

  /* Disable the DMA2D */
  __HAL_DMA2D_DISABLE(hdma2d);

  /* Get tick */
  tickstart = HAL_GetTick();

  /* Check if the DMA2D is effectively disabled */
  while((hdma2d->Instance->CR & DMA2D_CR_START) != 0)
  {
    if((HAL_GetTick() - tickstart ) > HAL_TIMEOUT_DMA2D_ABORT)
    {
      /* Update error code */
      hdma2d->ErrorCode |= HAL_DMA2D_ERROR_TIMEOUT;
      
      /* Change the DMA2D state */
      hdma2d->State= HAL_DMA2D_STATE_TIMEOUT;
      
      /* Process Unlocked */
      __HAL_UNLOCK(hdma2d);
      
      return HAL_TIMEOUT;
    }
  }
  /* Process Unlocked */
  __HAL_UNLOCK(hdma2d);

  /* Change the DMA2D state*/
  hdma2d->State = HAL_DMA2D_STATE_READY;

  return HAL_OK;
}

/**
  * @brief  Suspend the DMA2D Transfer.
  * @param  hdma2d: pointer to a DMA2D_HandleTypeDef structure that contains
  *                 the configuration information for the DMA2D. 
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMA2D_Suspend(DMA2D_HandleTypeDef *hdma2d)
{
  uint32_t tickstart = 0;

  /* Suspend the DMA2D transfer */
  hdma2d->Instance->CR |= DMA2D_CR_SUSP;

  /* Get tick */
  tickstart = HAL_GetTick();

  /* Check if the DMA2D is effectively suspended */
  while((hdma2d->Instance->CR & DMA2D_CR_SUSP) != DMA2D_CR_SUSP)
  {
    if((HAL_GetTick() - tickstart ) > HAL_TIMEOUT_DMA2D_SUSPEND)
    {
      /* Update error code */
      hdma2d->ErrorCode |= HAL_DMA2D_ERROR_TIMEOUT;
      
      /* Change the DMA2D state */
      hdma2d->State= HAL_DMA2D_STATE_TIMEOUT;
      
      return HAL_TIMEOUT;
    }
  }
  /* Change the DMA2D state*/
  hdma2d->State = HAL_DMA2D_STATE_SUSPEND;

  return HAL_OK;
}

/**
  * @brief  Resume the DMA2D Transfer.
  * @param  hdma2d: pointer to a DMA2D_HandleTypeDef structure that contains
  *                 the configuration information for the DMA2D.  
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMA2D_Resume(DMA2D_HandleTypeDef *hdma2d)
{
  /* Resume the DMA2D transfer */
  hdma2d->Instance->CR &= ~DMA2D_CR_SUSP;

  /* Change the DMA2D state*/
  hdma2d->State = HAL_DMA2D_STATE_BUSY;

  return HAL_OK;
}

/**
  * @brief  Polling for transfer complete or CLUT loading.
  * @param  hdma2d: pointer to a DMA2D_HandleTypeDef structure that contains
  *                 the configuration information for the DMA2D. 
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMA2D_PollForTransfer(DMA2D_HandleTypeDef *hdma2d, uint32_t Timeout)
{
  uint32_t tmp, tmp1;
  uint32_t tickstart = 0;

  /* Polling for DMA2D transfer */
  if((hdma2d->Instance->CR & DMA2D_CR_START) != 0)
  {
   /* Get tick */
   tickstart = HAL_GetTick();

    while(__HAL_DMA2D_GET_FLAG(hdma2d, DMA2D_FLAG_TC) == RESET)
    {
      tmp  = __HAL_DMA2D_GET_FLAG(hdma2d, DMA2D_FLAG_CE);
      tmp1 = __HAL_DMA2D_GET_FLAG(hdma2d, DMA2D_FLAG_TE);

      if((tmp != RESET) || (tmp1 != RESET))
      {
        /* Clear the transfer and configuration error flags */
        __HAL_DMA2D_CLEAR_FLAG(hdma2d, DMA2D_FLAG_CE);
        __HAL_DMA2D_CLEAR_FLAG(hdma2d, DMA2D_FLAG_TE);

        /* Change DMA2D state */
        hdma2d->State= HAL_DMA2D_STATE_ERROR;

        /* Process unlocked */
        __HAL_UNLOCK(hdma2d);
        
        return HAL_ERROR;
      }
      /* Check for the Timeout */
      if(Timeout != HAL_MAX_DELAY)
      {
        if((Timeout == 0)||((HAL_GetTick() - tickstart ) > Timeout))
        {
          /* Process unlocked */
          __HAL_UNLOCK(hdma2d);
        
          /* Update error code */
          hdma2d->ErrorCode |= HAL_DMA2D_ERROR_TIMEOUT;

          /* Change the DMA2D state */
          hdma2d->State= HAL_DMA2D_STATE_TIMEOUT;
          
          return HAL_TIMEOUT;
        }
      }        
    }
  }
  /* Polling for CLUT loading */
  if((hdma2d->Instance->FGPFCCR & DMA2D_FGPFCCR_START) != 0)
  {
    /* Get tick */
    tickstart = HAL_GetTick();
   
    while(__HAL_DMA2D_GET_FLAG(hdma2d, DMA2D_FLAG_CTC) == RESET)
    {
      if((__HAL_DMA2D_GET_FLAG(hdma2d, DMA2D_FLAG_CAE) != RESET))
      {      
        /* Clear the transfer and configuration error flags */
        __HAL_DMA2D_CLEAR_FLAG(hdma2d, DMA2D_FLAG_CAE);
        
        /* Change DMA2D state */
        hdma2d->State= HAL_DMA2D_STATE_ERROR;
        
        return HAL_ERROR;      
      }      
      /* Check for the Timeout */
      if(Timeout != HAL_MAX_DELAY)
      {
        if((Timeout == 0)||((HAL_GetTick() - tickstart ) > Timeout))
        {
          /* Update error code */
          hdma2d->ErrorCode |= HAL_DMA2D_ERROR_TIMEOUT;
    
          /* Change the DMA2D state */
          hdma2d->State= HAL_DMA2D_STATE_TIMEOUT;
          
          return HAL_TIMEOUT;
        }
      }      
    }
  }
  /* Clear the transfer complete flag */
  __HAL_DMA2D_CLEAR_FLAG(hdma2d, DMA2D_FLAG_TC);
  
  /* Clear the CLUT loading flag */
  __HAL_DMA2D_CLEAR_FLAG(hdma2d, DMA2D_FLAG_CTC);  
  
  /* Change DMA2D state */
  hdma2d->State = HAL_DMA2D_STATE_READY;
  
  /* Process unlocked */
  __HAL_UNLOCK(hdma2d);
  
  return HAL_OK;
}
/**
  * @brief  Handles DMA2D interrupt request.
  * @param  hdma2d: pointer to a DMA2D_HandleTypeDef structure that contains
  *                 the configuration information for the DMA2D.  
  * @retval HAL status
  */
void HAL_DMA2D_IRQHandler(DMA2D_HandleTypeDef *hdma2d)
{    
  /* Transfer Error Interrupt management ***************************************/
  if(__HAL_DMA2D_GET_FLAG(hdma2d, DMA2D_FLAG_TE) != RESET)
  {
    if(__HAL_DMA2D_GET_IT_SOURCE(hdma2d, DMA2D_IT_TE) != RESET)
    {
      /* Disable the transfer Error interrupt */
      __HAL_DMA2D_DISABLE_IT(hdma2d, DMA2D_IT_TE);  

      /* Update error code */
      hdma2d->ErrorCode |= HAL_DMA2D_ERROR_TE;
    
      /* Clear the transfer error flag */
      __HAL_DMA2D_CLEAR_FLAG(hdma2d, DMA2D_FLAG_TE);

      /* Change DMA2D state */
      hdma2d->State = HAL_DMA2D_STATE_ERROR;

      /* Process Unlocked */
      __HAL_UNLOCK(hdma2d);       
      
      if(hdma2d->XferErrorCallback != NULL)
      {
        /* Transfer error Callback */
        hdma2d->XferErrorCallback(hdma2d);
      }
    }
  }
  /* Configuration Error Interrupt management **********************************/
  if(__HAL_DMA2D_GET_FLAG(hdma2d, DMA2D_FLAG_CE) != RESET)
  {
    if(__HAL_DMA2D_GET_IT_SOURCE(hdma2d, DMA2D_IT_CE) != RESET)
    {
      /* Disable the Configuration Error interrupt */
      __HAL_DMA2D_DISABLE_IT(hdma2d, DMA2D_IT_CE);
  
      /* Clear the Configuration error flag */
      __HAL_DMA2D_CLEAR_FLAG(hdma2d, DMA2D_FLAG_CE);

      /* Update error code */
      hdma2d->ErrorCode |= HAL_DMA2D_ERROR_CE;    
    
      /* Change DMA2D state */
      hdma2d->State = HAL_DMA2D_STATE_ERROR;

      /* Process Unlocked */
      __HAL_UNLOCK(hdma2d);       
      
      if(hdma2d->XferErrorCallback != NULL)
      {
        /* Transfer error Callback */
        hdma2d->XferErrorCallback(hdma2d);
      }
    }
  }
  /* Transfer Complete Interrupt management ************************************/
  if(__HAL_DMA2D_GET_FLAG(hdma2d, DMA2D_FLAG_TC) != RESET)
  {
    if(__HAL_DMA2D_GET_IT_SOURCE(hdma2d, DMA2D_IT_TC) != RESET)
    { 
      /* Disable the transfer complete interrupt */
      __HAL_DMA2D_DISABLE_IT(hdma2d, DMA2D_IT_TC);
  
      /* Clear the transfer complete flag */  
      __HAL_DMA2D_CLEAR_FLAG(hdma2d, DMA2D_FLAG_TC);

      /* Update error code */
      hdma2d->ErrorCode |= HAL_DMA2D_ERROR_NONE;    
    
      /* Change DMA2D state */
      hdma2d->State = HAL_DMA2D_STATE_READY;
    
      /* Process Unlocked */
      __HAL_UNLOCK(hdma2d);       
      
      if(hdma2d->XferCpltCallback != NULL)
      {
        /* Transfer complete Callback */
        hdma2d->XferCpltCallback(hdma2d);
      }         
    }
  }
} 

/**
  * @}
  */

/** @defgroup DMA2D_Group3 Peripheral Control functions
 *  @brief    Peripheral Control functions 
 *
@verbatim   
 ===============================================================================
                    ##### Peripheral Control functions #####
 ===============================================================================  
    [..]  This section provides functions allowing to:
      (+) Configure the DMA2D foreground or/and background parameters.
      (+) Configure the DMA2D CLUT transfer.
      (+) Enable DMA2D CLUT.
      (+) Disable DMA2D CLUT.
      (+) Configure the line watermark

@endverbatim
  * @{
  */
/**
  * @brief  Configure the DMA2D Layer according to the specified
  *         parameters in the DMA2D_InitTypeDef and create the associated handle.
  * @param  hdma2d: pointer to a DMA2D_HandleTypeDef structure that contains
  *                 the configuration information for the DMA2D.
  * @param  LayerIdx: DMA2D Layer index.
  *                   This parameter can be one of the following values:
  *                   0(background) / 1(foreground)
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMA2D_ConfigLayer(DMA2D_HandleTypeDef *hdma2d, uint32_t LayerIdx)
{ 
  DMA2D_LayerCfgTypeDef *pLayerCfg = &hdma2d->LayerCfg[LayerIdx];
  
  uint32_t tmp = 0;
  
  /* Process locked */
  __HAL_LOCK(hdma2d);
  
  /* Change DMA2D peripheral state */
  hdma2d->State = HAL_DMA2D_STATE_BUSY; 
  
  /* Check the parameters */
  assert_param(IS_DMA2D_LAYER(LayerIdx));  
  assert_param(IS_DMA2D_OFFSET(pLayerCfg->InputOffset));  
  if(hdma2d->Init.Mode != DMA2D_R2M)
  {  
    assert_param(IS_DMA2D_INPUT_COLOR_MODE(pLayerCfg->InputColorMode));
    if(hdma2d->Init.Mode != DMA2D_M2M)
    {
      assert_param(IS_DMA2D_ALPHA_MODE(pLayerCfg->AlphaMode));
    }
  }
  
  /* Configure the background DMA2D layer */
  if(LayerIdx == 0)
  {
    /* DMA2D BGPFCR register configuration -----------------------------------*/
    /* Get the BGPFCCR register value */
    tmp = hdma2d->Instance->BGPFCCR;
    
    /* Clear Input color mode, alpha value and alpha mode bits */
    tmp &= (uint32_t)~(DMA2D_BGPFCCR_CM | DMA2D_BGPFCCR_AM | DMA2D_BGPFCCR_ALPHA); 
    
    if ((pLayerCfg->InputColorMode == CM_A4) || (pLayerCfg->InputColorMode == CM_A8))
    {
      /* Prepare the value to be wrote to the BGPFCCR register */
      tmp |= (pLayerCfg->InputColorMode | (pLayerCfg->AlphaMode << 16) | ((pLayerCfg->InputAlpha) & 0xFF000000));
    }
    else
    {
      /* Prepare the value to be wrote to the BGPFCCR register */
      tmp |= (pLayerCfg->InputColorMode | (pLayerCfg->AlphaMode << 16) | (pLayerCfg->InputAlpha << 24));
    }
    
    /* Write to DMA2D BGPFCCR register */
    hdma2d->Instance->BGPFCCR = tmp; 
    
    /* DMA2D BGOR register configuration -------------------------------------*/  
    /* Get the BGOR register value */
    tmp = hdma2d->Instance->BGOR;
    
    /* Clear colors bits */
    tmp &= (uint32_t)~DMA2D_BGOR_LO; 
    
    /* Prepare the value to be wrote to the BGOR register */
    tmp |= pLayerCfg->InputOffset;
    
    /* Write to DMA2D BGOR register */
    hdma2d->Instance->BGOR = tmp;
    
    if ((pLayerCfg->InputColorMode == CM_A4) || (pLayerCfg->InputColorMode == CM_A8))
    {
      /* Prepare the value to be wrote to the BGCOLR register */
      tmp |= ((pLayerCfg->InputAlpha) & 0x00FFFFFF);
    
      /* Write to DMA2D BGCOLR register */
      hdma2d->Instance->BGCOLR = tmp;
    }    
  }
  /* Configure the foreground DMA2D layer */
  else
  {
    /* DMA2D FGPFCR register configuration -----------------------------------*/
    /* Get the FGPFCCR register value */
    tmp = hdma2d->Instance->FGPFCCR;
    
    /* Clear Input color mode, alpha value and alpha mode bits */
    tmp &= (uint32_t)~(DMA2D_FGPFCCR_CM | DMA2D_FGPFCCR_AM | DMA2D_FGPFCCR_ALPHA); 
    
    if ((pLayerCfg->InputColorMode == CM_A4) || (pLayerCfg->InputColorMode == CM_A8))
    {
      /* Prepare the value to be wrote to the FGPFCCR register */
      tmp |= (pLayerCfg->InputColorMode | (pLayerCfg->AlphaMode << 16) | ((pLayerCfg->InputAlpha) & 0xFF000000));
    }
    else
    {
      /* Prepare the value to be wrote to the FGPFCCR register */
      tmp |= (pLayerCfg->InputColorMode | (pLayerCfg->AlphaMode << 16) | (pLayerCfg->InputAlpha << 24));
    }
    
    /* Write to DMA2D FGPFCCR register */
    hdma2d->Instance->FGPFCCR = tmp; 
    
    /* DMA2D FGOR register configuration -------------------------------------*/  
    /* Get the FGOR register value */
    tmp = hdma2d->Instance->FGOR;
    
    /* Clear colors bits */
    tmp &= (uint32_t)~DMA2D_FGOR_LO; 
    
    /* Prepare the value to be wrote to the FGOR register */
    tmp |= pLayerCfg->InputOffset;
    
    /* Write to DMA2D FGOR register */
    hdma2d->Instance->FGOR = tmp;
   
    if ((pLayerCfg->InputColorMode == CM_A4) || (pLayerCfg->InputColorMode == CM_A8))
    {
      /* Prepare the value to be wrote to the FGCOLR register */
      tmp |= ((pLayerCfg->InputAlpha) & 0x00FFFFFF);
    
      /* Write to DMA2D FGCOLR register */
      hdma2d->Instance->FGCOLR = tmp;
    }   
  }    
  /* Initialize the DMA2D state*/
  hdma2d->State  = HAL_DMA2D_STATE_READY;
  
  /* Process unlocked */
  __HAL_UNLOCK(hdma2d);  
  
  return HAL_OK;
}

/**
  * @brief  Configure the DMA2D CLUT Transfer.
  * @param  hdma2d:   pointer to a DMA2D_HandleTypeDef structure that contains
  *                   the configuration information for the DMA2D.
  * @param  CLUTCfg:  pointer to a DMA2D_CLUTCfgTypeDef structure that contains
  *                   the configuration information for the color look up table.
  * @param  LayerIdx: DMA2D Layer index.
  *                   This parameter can be one of the following values:
  *                   0(background) / 1(foreground)
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMA2D_ConfigCLUT(DMA2D_HandleTypeDef *hdma2d, DMA2D_CLUTCfgTypeDef CLUTCfg, uint32_t LayerIdx)
{
  uint32_t tmp = 0, tmp1 = 0;

  /* Check the parameters */
  assert_param(IS_DMA2D_LAYER(LayerIdx));   
  assert_param(IS_DMA2D_CLUT_CM(CLUTCfg.CLUTColorMode));
  assert_param(IS_DMA2D_CLUT_SIZE(CLUTCfg.Size));
  
  /* Configure the CLUT of the background DMA2D layer */
  if(LayerIdx == 0)
  {
    /* Get the BGCMAR register value */
    tmp = hdma2d->Instance->BGCMAR;

    /* Clear CLUT address bits */
    tmp &= (uint32_t)~DMA2D_BGCMAR_MA; 
  
    /* Prepare the value to be wrote to the BGCMAR register */
    tmp |= (uint32_t)CLUTCfg.pCLUT;
  
    /* Write to DMA2D BGCMAR register */
    hdma2d->Instance->BGCMAR = tmp;
    
    /* Get the BGPFCCR register value */
    tmp = hdma2d->Instance->BGPFCCR;

    /* Clear CLUT size and CLUT address bits */
    tmp &= (uint32_t)~(DMA2D_BGPFCCR_CS | DMA2D_BGPFCCR_CCM); 

    /* Get the CLUT size */
    tmp1 = CLUTCfg.Size << 16;
    
    /* Prepare the value to be wrote to the BGPFCCR register */
    tmp |= (CLUTCfg.CLUTColorMode | tmp1);
  
    /* Write to DMA2D BGPFCCR register */
    hdma2d->Instance->BGPFCCR = tmp;       
  }
  /* Configure the CLUT of the foreground DMA2D layer */
  else
  {
    /* Get the FGCMAR register value */
    tmp = hdma2d->Instance->FGCMAR;

    /* Clear CLUT address bits */
    tmp &= (uint32_t)~DMA2D_FGCMAR_MA; 
  
    /* Prepare the value to be wrote to the FGCMAR register */
    tmp |= (uint32_t)CLUTCfg.pCLUT;
  
    /* Write to DMA2D FGCMAR register */
    hdma2d->Instance->FGCMAR = tmp;
    
    /* Get the FGPFCCR register value */
    tmp = hdma2d->Instance->FGPFCCR;

    /* Clear CLUT size and CLUT address bits */
    tmp &= (uint32_t)~(DMA2D_FGPFCCR_CS | DMA2D_FGPFCCR_CCM); 

    /* Get the CLUT size */
    tmp1 = CLUTCfg.Size << 8;
    
    /* Prepare the value to be wrote to the FGPFCCR register */
    tmp |= (CLUTCfg.CLUTColorMode | tmp1);
  
    /* Write to DMA2D FGPFCCR register */
    hdma2d->Instance->FGPFCCR = tmp;    
  }
  
  return HAL_OK;
}

/**
  * @brief  Enable the DMA2D CLUT Transfer.
  * @param  hdma2d:   pointer to a DMA2D_HandleTypeDef structure that contains
  *                   the configuration information for the DMA2D.
  * @param  LayerIdx: DMA2D Layer index.
  *                   This parameter can be one of the following values:
  *                   0(background) / 1(foreground)
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMA2D_EnableCLUT(DMA2D_HandleTypeDef *hdma2d, uint32_t LayerIdx)
{  
  /* Check the parameters */
  assert_param(IS_DMA2D_LAYER(LayerIdx));
  
  if(LayerIdx == 0)
  {
    /* Enable the CLUT loading for the background */
    hdma2d->Instance->BGPFCCR |= DMA2D_BGPFCCR_START;
  }
  else
  {
    /* Enable the CLUT loading for the foreground */
    hdma2d->Instance->FGPFCCR |= DMA2D_FGPFCCR_START;
  }
  
  return HAL_OK;
}

/**
  * @brief  Disable the DMA2D CLUT Transfer.
  * @param  hdma2d:   pointer to a DMA2D_HandleTypeDef structure that contains
  *                   the configuration information for the DMA2D.
  * @param  LayerIdx: DMA2D Layer index.
  *                   This parameter can be one of the following values:
  *                   0(background) / 1(foreground)
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMA2D_DisableCLUT(DMA2D_HandleTypeDef *hdma2d, uint32_t LayerIdx)
{
  /* Check the parameters */
  assert_param(IS_DMA2D_LAYER(LayerIdx));
  
  if(LayerIdx == 0)
  {
    /* Disable the CLUT loading for the background */
    hdma2d->Instance->BGPFCCR &= ~DMA2D_BGPFCCR_START;
  }
  else
  {
    /* Disable the CLUT loading for the foreground */
    hdma2d->Instance->FGPFCCR &= ~DMA2D_FGPFCCR_START;
  } 
  
  return HAL_OK;
}

/**
  * @brief  Define the configuration of the line watermark .
  * @param  hdma2d: pointer to a DMA2D_HandleTypeDef structure that contains
  *                 the configuration information for the DMA2D.
  * @param  Line:   Line Watermark configuration.
  * @retval HAL status
  */

HAL_StatusTypeDef HAL_DMA2D_ProgramLineEvent(DMA2D_HandleTypeDef *hdma2d, uint32_t Line)
{
  /* Process locked */
  __HAL_LOCK(hdma2d);
  
  /* Change DMA2D peripheral state */
  hdma2d->State = HAL_DMA2D_STATE_BUSY;
  
  /* Check the parameters */
  assert_param(IS_DMA2D_LineWatermark(Line));

  /* Sets the Line watermark configuration */
  DMA2D->LWR = (uint32_t)Line;
  
  /* Initialize the DMA2D state*/
  hdma2d->State = HAL_DMA2D_STATE_READY;
  
  /* Process unlocked */
  __HAL_UNLOCK(hdma2d);  
  
  return HAL_OK;  
}

/**
  * @}
  */

/** @defgroup DMA2D_Group4 Peripheral State functions
 *  @brief    Peripheral State functions 
 *
@verbatim   
 ===============================================================================
                  ##### Peripheral State and Errors functions #####
 ===============================================================================  
    [..]
    This subsection provides functions allowing to :
      (+) Check the DMA2D state
      (+) Get error code  

@endverbatim
  * @{
  */ 

/**
  * @brief  Return the DMA2D state
  * @param  hdma2d: pointer to a DMA2D_HandleTypeDef structure that contains
  *                 the configuration information for the DMA2D.  
  * @retval HAL state
  */
HAL_DMA2D_StateTypeDef HAL_DMA2D_GetState(DMA2D_HandleTypeDef *hdma2d)
{  
  return hdma2d->State;
}

/**
  * @brief  Return the DMA2D error code
  * @param  hdma2d : pointer to a DMA2D_HandleTypeDef structure that contains
  *               the configuration information for DMA2D.
  * @retval DMA2D Error Code
  */
uint32_t HAL_DMA2D_GetError(DMA2D_HandleTypeDef *hdma2d)
{
  return hdma2d->ErrorCode;
}

/**
  * @}
  */


/**
  * @brief  Set the DMA2D Transfer parameter.
  * @param  hdma2d:     pointer to a DMA2D_HandleTypeDef structure that contains
  *                     the configuration information for the specified DMA2D.  
  * @param  pdata:      The source memory Buffer address
  * @param  DstAddress: The destination memory Buffer address
  * @param  Width:      The width of data to be transferred from source to destination.
  * @param  Heigh:      The heigh of data to be transferred from source to destination.
  * @retval HAL status
  */
static void DMA2D_SetConfig(DMA2D_HandleTypeDef *hdma2d, uint32_t pdata, uint32_t DstAddress, uint32_t Width, uint32_t Heigh)
{  
  uint32_t tmp = 0;
  uint32_t tmp1 = 0;
  uint32_t tmp2 = 0;
  uint32_t tmp3 = 0;
  uint32_t tmp4 = 0;
  
  tmp = Width << 16;
  
  /* Configure DMA2D data size */
  hdma2d->Instance->NLR = (Heigh | tmp);
  
  /* Configure DMA2D destination address */
  hdma2d->Instance->OMAR = DstAddress;
 
  /* Register to memory DMA2D mode selected */
  if (hdma2d->Init.Mode == DMA2D_R2M)
  {    
    tmp1 = pdata & DMA2D_OCOLR_ALPHA_1;
    tmp2 = pdata & DMA2D_OCOLR_RED_1;
    tmp3 = pdata & DMA2D_OCOLR_GREEN_1;
    tmp4 = pdata & DMA2D_OCOLR_BLUE_1;
    
    /* Prepare the value to be wrote to the OCOLR register according to the color mode */
    if (hdma2d->Init.ColorMode == DMA2D_ARGB8888)
    {
      tmp = (tmp3 | tmp2 | tmp1| tmp4);
    }
    else if (hdma2d->Init.ColorMode == DMA2D_RGB888)
    {
      tmp = (tmp3 | tmp2 | tmp4);  
    }
    else if (hdma2d->Init.ColorMode == DMA2D_RGB565)
    {
      tmp2 = (tmp2 >> 19);
      tmp3 = (tmp3 >> 10);
      tmp4 = (tmp4 >> 3 );
      tmp  = ((tmp3 << 5) | (tmp2 << 11) | tmp4); 
    }
    else if (hdma2d->Init.ColorMode == DMA2D_ARGB1555)
    { 
      tmp1 = (tmp1 >> 31);
      tmp2 = (tmp2 >> 19);
      tmp3 = (tmp3 >> 11);
      tmp4 = (tmp4 >> 3 );      
      tmp  = ((tmp3 << 5) | (tmp2 << 10) | (tmp1 << 15) | tmp4);    
    } 
    else /* DMA2D_CMode = DMA2D_ARGB4444 */
    {
      tmp1 = (tmp1 >> 28);
      tmp2 = (tmp2 >> 20);
      tmp3 = (tmp3 >> 12);
      tmp4 = (tmp4 >> 4 );
      tmp  = ((tmp3 << 4) | (tmp2 << 8) | (tmp1 << 12) | tmp4);
    }    
    /* Write to DMA2D OCOLR register */
    hdma2d->Instance->OCOLR = tmp;
  } 
  else /* M2M, M2M_PFC or M2M_Blending DMA2D Mode */
  {
    /* Configure DMA2D source address */
    hdma2d->Instance->FGMAR = pdata;
  }
}

/**
  * @}
  */
#endif /* STM32F427xx || STM32F437xx || STM32F429xx || STM32F439xx */
#endif /* HAL_DMA2D_MODULE_ENABLED */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
