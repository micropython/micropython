/**
  ******************************************************************************
  * @file    stm32f4xx_hal_dcmi.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    18-February-2014
  * @brief   DCMI HAL module driver
  *          This file provides firmware functions to manage the following 
  *          functionalities of the Digital Camera Interface (DCMI) peripheral:
  *           + Initialization and de-initialization functions
  *           + IO operation functions
  *           + Peripheral Control functions 
  *           + Peripheral State and Error functions  
  *           
  @verbatim      
  ==============================================================================
                        ##### How to use this driver #####
  ==============================================================================
  [..]
      The sequence below describes how to use this driver to capture image
      from a camera module connected to the DCMI Interface.
      This sequence does not take into account the configuration of the
      camera module, which should be made before to configure and enable
      the DCMI to capture images.

    (#) Program the required configuration through following parameters:
        horizontal and vertical polarity, pixel clock polarity, Capture Rate,
        Synchronization Mode, code of the frame delimiter and data width 
        using HAL_DCMI_Init() function.

    (#) Configure the DMA2_Stream1 channel1 to transfer Data from DCMI DR
        register to the destination memory buffer.

    (#) Program the required configuration through following parameters:
        DCMI mode, destination memory Buffer address and the data length 
        and enable capture using HAL_DCMI_Start_DMA() function.

    (#) Optionally, configure and Enable the CROP feature to select a rectangular
        window from the received image using HAL_DCMI_ConfigCrop() 
        and HAL_DCMI_EnableCROP() functions

    (#) The capture can be stopped using the following HAL_DCMI_Stop() function.

    (#) To control DCMI state you can use the following function : HAL_DCMI_GetState()

     *** DCMI HAL driver macros list ***
     ============================================= 
     [..]
       Below the list of most used macros in DCMI HAL driver.
       
      (+) __HAL_DCMI_ENABLE: Enable the DCMI peripheral.
      (+) __HAL_DCMI_DISABLE: Disable the DCMI peripheral.
      (+) __HAL_DCMI_GET_FLAG: Get the DCMI pending flags.
      (+) __HAL_DCMI_CLEAR_FLAG: Clear the DCMI pending flags.
      (+) __HAL_DCMI_ENABLE_IT: Enable the specified DCMI interrupts.
      (+) __HAL_DCMI_DISABLE_IT: Disable the specified DCMI interrupts.
      (+) __HAL_DCMI_GET_IT_SOURCE: Check whether the specified DCMI interrupt has occurred or not.
 
     [..] 
       (@) You can refer to the DCMI HAL driver header file for more useful macros
      
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
/** @defgroup DCMI 
  * @brief DCMI HAL module driver
  * @{
  */

#ifdef HAL_DCMI_MODULE_ENABLED

#if defined(STM32F407xx) || defined(STM32F417xx) || defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx)
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define HAL_TIMEOUT_DCMI_STOP    ((uint32_t)1000)  /* 1s  */
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void       DCMI_DMAConvCplt(DMA_HandleTypeDef *hdma);
static void       DCMI_DMAError(DMA_HandleTypeDef *hdma);

/* Private functions ---------------------------------------------------------*/

/** @defgroup DCMI_Private_Functions
  * @{
  */

/** @defgroup DCMI_Group1 Initialization and Configuration functions
 *  @brief   Initialization and Configuration functions
 *
@verbatim   
 ===============================================================================
                ##### Initialization and Configuration functions #####
 ===============================================================================  
    [..]  This section provides functions allowing to:
      (+) Initialize and configure the DCMI
      (+) De-initialize the DCMI 

@endverbatim
  * @{
  */
  
/**
  * @brief  Initializes the DCMI according to the specified
  *         parameters in the DCMI_InitTypeDef and create the associated handle.
  * @param  hdcmi: pointer to a DCMI_HandleTypeDef structure that contains
  *                the configuration information for DCMI.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DCMI_Init(DCMI_HandleTypeDef *hdcmi)
{     
  /* Check the DCMI peripheral state */
  if(hdcmi == NULL)
  {
     return HAL_ERROR;
  }
  
  /* Check function parameters */
  assert_param(IS_DCMI_ALL_INSTANCE(hdcmi->Instance));
  assert_param(IS_DCMI_PCKPOLARITY(hdcmi->Init.PCKPolarity));
  assert_param(IS_DCMI_VSPOLARITY(hdcmi->Init.VSPolarity));
  assert_param(IS_DCMI_HSPOLARITY(hdcmi->Init.HSPolarity));
  assert_param(IS_DCMI_SYNCHRO(hdcmi->Init.SynchroMode));
  assert_param(IS_DCMI_CAPTURE_RATE(hdcmi->Init.CaptureRate));
  assert_param(IS_DCMI_EXTENDED_DATA(hdcmi->Init.ExtendedDataMode));
  assert_param(IS_DCMI_MODE_JPEG(hdcmi->Init.JPEGMode));

  if(hdcmi->State == HAL_DCMI_STATE_RESET)
  {
    /* Init the low level hardware */
    HAL_DCMI_MspInit(hdcmi);
  } 
  
  /* Change the DCMI state */
  hdcmi->State = HAL_DCMI_STATE_BUSY; 

  /* Configures the HS, VS, DE and PC polarity */
  hdcmi->Instance->CR &= ~(DCMI_CR_PCKPOL | DCMI_CR_HSPOL  | DCMI_CR_VSPOL  | DCMI_CR_EDM_0 |
                           DCMI_CR_EDM_1  | DCMI_CR_FCRC_0 | DCMI_CR_FCRC_1 | DCMI_CR_JPEG  |
                           DCMI_CR_ESS);
  hdcmi->Instance->CR |=  (uint32_t)(hdcmi->Init.SynchroMode | hdcmi->Init.CaptureRate | \
                                     hdcmi->Init.VSPolarity  | hdcmi->Init.HSPolarity  | \
                                     hdcmi->Init.PCKPolarity | hdcmi->Init.ExtendedDataMode | \
                                     hdcmi->Init.JPEGMode);

  if(hdcmi->Init.SynchroMode == DCMI_SYNCHRO_EMBEDDED)
  {
    DCMI->ESCR = (((uint32_t)hdcmi->Init.SyncroCode.FrameStartCode)    |
                  ((uint32_t)hdcmi->Init.SyncroCode.LineStartCode << 8)|
                  ((uint32_t)hdcmi->Init.SyncroCode.LineEndCode << 16) |
                  ((uint32_t)hdcmi->Init.SyncroCode.FrameEndCode << 24));
  }

  /* Enable the Line interrupt */
  __HAL_DCMI_ENABLE_IT(hdcmi, DCMI_IT_LINE);

  /* Enable the VSYNC interrupt */
  __HAL_DCMI_ENABLE_IT(hdcmi, DCMI_IT_VSYNC);

  /* Enable the Frame capture complete interrupt */
  __HAL_DCMI_ENABLE_IT(hdcmi, DCMI_IT_FRAME);

  /* Enable the Synchronization error interrupt */
  __HAL_DCMI_ENABLE_IT(hdcmi, DCMI_IT_ERR);

  /* Enable the Overflow interrupt */
  __HAL_DCMI_ENABLE_IT(hdcmi, DCMI_IT_OVF);

  /* Enable DCMI by setting DCMIEN bit */
  __HAL_DCMI_ENABLE(hdcmi);

  /* Update error code */
  hdcmi->ErrorCode = HAL_DCMI_ERROR_NONE;
  
  /* Initialize the DCMI state*/
  hdcmi->State  = HAL_DCMI_STATE_READY;

  return HAL_OK;
}

/**
  * @brief  Deinitializes the DCMI peripheral registers to their default reset
  *         values.
  * @param  hdcmi: pointer to a DCMI_HandleTypeDef structure that contains
  *                the configuration information for DCMI.
  * @retval None
  */

HAL_StatusTypeDef HAL_DCMI_DeInit(DCMI_HandleTypeDef *hdcmi)
{
  /* DeInit the low level hardware */
  HAL_DCMI_MspDeInit(hdcmi);

  /* Update error code */
  hdcmi->ErrorCode = HAL_DCMI_ERROR_NONE;

  /* Initialize the DCMI state*/
  hdcmi->State = HAL_DCMI_STATE_RESET;

  /* Release Lock */
  __HAL_UNLOCK(hdcmi);

  return HAL_OK;
}

/**
  * @brief  Initializes the DCMI MSP.
  * @param  hdcmi: pointer to a DCMI_HandleTypeDef structure that contains
  *                the configuration information for DCMI.
  * @retval None
  */
__weak void HAL_DCMI_MspInit(DCMI_HandleTypeDef* hdcmi)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_DCMI_MspInit could be implemented in the user file
   */ 
}

/**
  * @brief  DeInitializes the DCMI MSP.
  * @param  hdcmi: pointer to a DCMI_HandleTypeDef structure that contains
  *                the configuration information for DCMI.
  * @retval None
  */
__weak void HAL_DCMI_MspDeInit(DCMI_HandleTypeDef* hdcmi)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_DCMI_MspDeInit could be implemented in the user file
   */
}

/**
  * @}
  */
/** @defgroup DCMI_Group2 IO operation functions 
 *  @brief   IO operation functions  
 *
@verbatim   
 ===============================================================================
                      #####  IO operation functions  #####
 ===============================================================================  
    [..]  This section provides functions allowing to:
      (+) Configure destination address and data length and 
          Enables DCMI DMA request and enables DCMI capture
      (+) Stop the DCMI capture.
      (+) Handles DCMI interrupt request.

@endverbatim
  * @{
  */

/**
  * @brief  Enables DCMI DMA request and enables DCMI capture  
  * @param  hdcmi:     pointer to a DCMI_HandleTypeDef structure that contains
  *                    the configuration information for DCMI.
  * @param  DCMI_Mode: DCMI capture mode snapshot or continuous grab.
  * @param  pData:     The destination memory Buffer address (LCD Frame buffer).
  * @param  Length:    The length of capture to be transferred.
  * @retval None
  */
HAL_StatusTypeDef HAL_DCMI_Start_DMA(DCMI_HandleTypeDef* hdcmi, uint32_t DCMI_Mode, uint32_t pData, uint32_t Length)
{  
  /* Initialise the second memory address */
  uint32_t SecondMemAddress = 0;

  /* Check function parameters */
  assert_param(IS_DCMI_CAPTURE_MODE(DCMI_Mode));

  /* Process Locked */
  __HAL_LOCK(hdcmi);

  /* Lock the DCMI peripheral state */
  hdcmi->State = HAL_DCMI_STATE_BUSY;

  /* Check the parameters */
  assert_param(IS_DCMI_CAPTURE_MODE(DCMI_Mode));

  /* Configure the DCMI Mode */
  hdcmi->Instance->CR &= ~(DCMI_CR_CM);
  hdcmi->Instance->CR |=  (uint32_t)(DCMI_Mode);

  /* Set the DMA memory0 conversion complete callback */
  hdcmi->DMA_Handle->XferCpltCallback = DCMI_DMAConvCplt;

  /* Set the DMA error callback */
  hdcmi->DMA_Handle->XferErrorCallback = DCMI_DMAError;

  if(Length <= 0xFFFF)
  {
    /* Enable the DMA Stream */
    HAL_DMA_Start_IT(hdcmi->DMA_Handle, (uint32_t)&hdcmi->Instance->DR, (uint32_t)pData, Length);
  }
  else /* DCMI_DOUBLE_BUFFER Mode */
  {
    /* Set the DMA memory1 conversion complete callback */
    hdcmi->DMA_Handle->XferM1CpltCallback = DCMI_DMAConvCplt; 

    /* Initialise transfer parameters */
    hdcmi->XferCount = 1;
    hdcmi->XferSize = Length;
    hdcmi->pBuffPtr = pData;
      
    /* Get the number of buffer */
    while(hdcmi->XferSize > 0xFFFF)
    {
      hdcmi->XferSize = (hdcmi->XferSize/2);
      hdcmi->XferCount = hdcmi->XferCount*2;
    }

    /* Update DCMI counter  and transfer number*/
    hdcmi->XferCount = (hdcmi->XferCount - 2);
    hdcmi->XferTransferNumber = hdcmi->XferCount;

    /* Update second memory address */
    SecondMemAddress = (uint32_t)(pData + (4*hdcmi->XferSize));

    /* Start DMA multi buffer transfer */
    HAL_DMAEx_MultiBufferStart_IT(hdcmi->DMA_Handle, (uint32_t)&hdcmi->Instance->DR, (uint32_t)pData, SecondMemAddress, hdcmi->XferSize);
  }

  /* Enable Capture */
  DCMI->CR |= DCMI_CR_CAPTURE;

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Disable DCMI DMA request and Disable DCMI capture  
  * @param  hdcmi: pointer to a DCMI_HandleTypeDef structure that contains
  *                the configuration information for DCMI. 
  */
HAL_StatusTypeDef HAL_DCMI_Stop(DCMI_HandleTypeDef* hdcmi)
{
  uint32_t timeout = 0x00;

  /* Lock the DCMI peripheral state */
  hdcmi->State = HAL_DCMI_STATE_BUSY;

  __HAL_DCMI_DISABLE(hdcmi);

  /* Disable Capture */
  DCMI->CR &= ~(DCMI_CR_CAPTURE);

  /* Get timeout */
  timeout = HAL_GetTick() + HAL_TIMEOUT_DCMI_STOP;

  /* Check if the DCMI capture effectively disabled */
  while((hdcmi->Instance->CR & DCMI_CR_CAPTURE) != 0)
  {
    if(HAL_GetTick() >= timeout)
    {
      /* Process Unlocked */
      __HAL_UNLOCK(hdcmi);
      
      /* Update error code */
      hdcmi->ErrorCode |= HAL_DCMI_ERROR_TIMEOUT;
      
      /* Change DCMI state */
      hdcmi->State = HAL_DCMI_STATE_TIMEOUT;
      
      return HAL_TIMEOUT;
    }
  }

  /* Disable the DMA */
  HAL_DMA_Abort(hdcmi->DMA_Handle);

  /* Update error code */
  hdcmi->ErrorCode |= HAL_DCMI_ERROR_NONE;

  /* Change DCMI state */
  hdcmi->State = HAL_DCMI_STATE_READY;

  /* Process Unlocked */
  __HAL_UNLOCK(hdcmi);

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Handles DCMI interrupt request.
  * @param  hdcmi: pointer to a DCMI_HandleTypeDef structure that contains
  *                the configuration information for the DCMI.
  * @retval HAL status
  */
void HAL_DCMI_IRQHandler(DCMI_HandleTypeDef *hdcmi)
{  
  /* Synchronization error interrupt management *******************************/
  if(__HAL_DCMI_GET_FLAG(hdcmi, DCMI_FLAG_ERRRI) != RESET)
  {
    if(__HAL_DCMI_GET_IT_SOURCE(hdcmi, DCMI_IT_ERR) != RESET)
    {
      /* Disable the Synchronization error interrupt */
      __HAL_DCMI_DISABLE_IT(hdcmi, DCMI_IT_ERR); 

      /* Clear the Synchronization error flag */
      __HAL_DCMI_CLEAR_FLAG(hdcmi, DCMI_FLAG_ERRRI);

      /* Update error code */
      hdcmi->ErrorCode |= HAL_DCMI_ERROR_SYNC;

      /* Change DCMI state */
      hdcmi->State = HAL_DCMI_STATE_ERROR;

      /* Process Unlocked */
      __HAL_UNLOCK(hdcmi);

      /* Abort the DMA Transfer */
      HAL_DMA_Abort(hdcmi->DMA_Handle);
      
      /* Synchronization error Callback */
      HAL_DCMI_ErrorCallback(hdcmi);
    }
  }
  /* Overflow interrupt management ********************************************/
  if(__HAL_DCMI_GET_FLAG(hdcmi, DCMI_FLAG_OVFRI) != RESET) 
  {
    if(__HAL_DCMI_GET_IT_SOURCE(hdcmi, DCMI_IT_OVF) != RESET)
    {
      /* Disable the Overflow interrupt */
      __HAL_DCMI_DISABLE_IT(hdcmi, DCMI_IT_OVF);

      /* Clear the Overflow flag */
      __HAL_DCMI_CLEAR_FLAG(hdcmi, DCMI_FLAG_OVFRI);

      /* Update error code */
      hdcmi->ErrorCode |= HAL_DCMI_ERROR_OVF;

      /* Change DCMI state */
      hdcmi->State = HAL_DCMI_STATE_ERROR;

      /* Process Unlocked */
      __HAL_UNLOCK(hdcmi);

      /* Abort the DMA Transfer */
      HAL_DMA_Abort(hdcmi->DMA_Handle);

      /* Overflow Callback */
      HAL_DCMI_ErrorCallback(hdcmi);
    }
  }
  /* Line Interrupt management ************************************************/
  if(__HAL_DCMI_GET_FLAG(hdcmi, DCMI_FLAG_LINERI) != RESET)
  {
    if(__HAL_DCMI_GET_IT_SOURCE(hdcmi, DCMI_IT_LINE) != RESET)
    {
      /* Clear the Line interrupt flag */  
      __HAL_DCMI_CLEAR_FLAG(hdcmi, DCMI_FLAG_LINERI);

      /* Process Unlocked */
      __HAL_UNLOCK(hdcmi);

      /* Line interrupt Callback */
      HAL_DCMI_LineEventCallback(hdcmi);
    }
  }
  /* VSYNC interrupt management ***********************************************/
  if(__HAL_DCMI_GET_FLAG(hdcmi, DCMI_FLAG_VSYNCRI) != RESET)
  {
    if(__HAL_DCMI_GET_IT_SOURCE(hdcmi, DCMI_IT_VSYNC) != RESET)
    {
      /* Disable the VSYNC interrupt */
      __HAL_DCMI_DISABLE_IT(hdcmi, DCMI_IT_VSYNC);   

      /* Clear the VSYNC flag */
      __HAL_DCMI_CLEAR_FLAG(hdcmi, DCMI_FLAG_VSYNCRI);

      /* Process Unlocked */
      __HAL_UNLOCK(hdcmi);

      /* VSYNC Callback */
      HAL_DCMI_VsyncEventCallback(hdcmi);
    }
  }
  /* End of Frame interrupt management ****************************************/
  if(__HAL_DCMI_GET_FLAG(hdcmi, DCMI_FLAG_FRAMERI) != RESET)
  {
    if(__HAL_DCMI_GET_IT_SOURCE(hdcmi, DCMI_IT_FRAME) != RESET)
    {
      /* Disable the End of Frame interrupt */
      __HAL_DCMI_DISABLE_IT(hdcmi, DCMI_IT_FRAME);

      /* Clear the End of Frame flag */
      __HAL_DCMI_CLEAR_FLAG(hdcmi, DCMI_FLAG_FRAMERI);

      /* Process Unlocked */
      __HAL_UNLOCK(hdcmi);

      /* End of Frame Callback */
      HAL_DCMI_FrameEventCallback(hdcmi);
    }
  }
}

/**
  * @brief  Error DCMI callback.
  * @param  hdcmi: pointer to a DCMI_HandleTypeDef structure that contains
  *                the configuration information for DCMI.
  * @retval None
  */
__weak void HAL_DCMI_ErrorCallback(DCMI_HandleTypeDef *hdcmi)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_DCMI_ErrorCallback could be implemented in the user file
   */
}

/**
  * @brief  Line Event callback.
  * @param  hdcmi: pointer to a DCMI_HandleTypeDef structure that contains
  *                the configuration information for DCMI.
  * @retval None
  */
__weak void HAL_DCMI_LineEventCallback(DCMI_HandleTypeDef *hdcmi)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_DCMI_LineEventCallback could be implemented in the user file
   */
}

/**
  * @brief  VSYNC Event callback.
  * @param  hdcmi: pointer to a DCMI_HandleTypeDef structure that contains
  *                the configuration information for DCMI.
  * @retval None
  */
__weak void HAL_DCMI_VsyncEventCallback(DCMI_HandleTypeDef *hdcmi)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_DCMI_VsyncEventCallback could be implemented in the user file
   */
}

/**
  * @brief  Frame Event callback.
  * @param  hdcmi: pointer to a DCMI_HandleTypeDef structure that contains
  *                the configuration information for DCMI.
  * @retval None
  */
__weak void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef *hdcmi)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_DCMI_FrameEventCallback could be implemented in the user file
   */
}

/**
  * @}
  */

/** @defgroup DCMI_Group3 Peripheral Control functions
 *  @brief    Peripheral Control functions 
 *
@verbatim   
 ===============================================================================
                    ##### Peripheral Control functions #####
 ===============================================================================  
[..]  This section provides functions allowing to:
      (+) Configure the CROP feature.
      (+) ENABLE/DISABLE the CROP feature.

@endverbatim
  * @{
  */

/**
  * @brief  Configure the DCMI CROP coordinate.
  * @param  hdcmi: pointer to a DCMI_HandleTypeDef structure that contains
  *                the configuration information for DCMI.
  * @param  YSize: DCMI Line number
  * @param  XSize: DCMI Pixel per line
  * @param  X0:    DCMI window X offset
  * @param  Y0:    DCMI window Y offset
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DCMI_ConfigCROP(DCMI_HandleTypeDef *hdcmi, uint32_t X0, uint32_t Y0, uint32_t XSize, uint32_t YSize)
{
  /* Process Locked */
  __HAL_LOCK(hdcmi);

  /* Lock the DCMI peripheral state */
  hdcmi->State = HAL_DCMI_STATE_BUSY;

  /* Check the parameters */
  assert_param(IS_DCMI_WINDOW_COORDINATE(X0));
  assert_param(IS_DCMI_WINDOW_COORDINATE(Y0));
  assert_param(IS_DCMI_WINDOW_COORDINATE(XSize));
  assert_param(IS_DCMI_WINDOW_HEIGHT(YSize));

  /* Configure CROP */
  DCMI->CWSIZER = (XSize | (YSize << 16));
  DCMI->CWSTRTR = (X0 | (Y0 << 16));

  /* Initialize the DCMI state*/
  hdcmi->State  = HAL_DCMI_STATE_READY;

  /* Process Unlocked */
  __HAL_UNLOCK(hdcmi);

  return HAL_OK;
}

/**
  * @brief  Disable the Crop feature.
  * @param  hdcmi: pointer to a DCMI_HandleTypeDef structure that contains
  *                the configuration information for DCMI.
  * @retval None
  */
HAL_StatusTypeDef HAL_DCMI_DisableCROP(DCMI_HandleTypeDef *hdcmi)
{
  /* Process Locked */
  __HAL_LOCK(hdcmi);

  /* Lock the DCMI peripheral state */
  hdcmi->State = HAL_DCMI_STATE_BUSY;

  /* Disable DCMI Crop feature */
  DCMI->CR &= ~(uint32_t)DCMI_CR_CROP;  

  /* Change the DCMI state*/
  hdcmi->State = HAL_DCMI_STATE_READY;   

  /* Process Unlocked */
  __HAL_UNLOCK(hdcmi);

  return HAL_OK;  
}

/**
  * @brief  Enable the Crop feature.
  * @param  hdcmi: pointer to a DCMI_HandleTypeDef structure that contains
  *                the configuration information for DCMI.
  * @retval None
  */
HAL_StatusTypeDef HAL_DCMI_EnableCROP(DCMI_HandleTypeDef *hdcmi)
{
  /* Process Locked */
  __HAL_LOCK(hdcmi);

  /* Lock the DCMI peripheral state */
  hdcmi->State = HAL_DCMI_STATE_BUSY;

  /* Enable DCMI Crop feature */
  DCMI->CR |= (uint32_t)DCMI_CR_CROP;

  /* Change the DCMI state*/
  hdcmi->State = HAL_DCMI_STATE_READY;

  /* Process Unlocked */
  __HAL_UNLOCK(hdcmi);

  return HAL_OK;  
}

/**
  * @}
  */

/** @defgroup DCMI_Group4 Peripheral State functions
 *  @brief    Peripheral State functions 
 *
@verbatim   
 ===============================================================================
               ##### Peripheral State and Errors functions #####
 ===============================================================================  
    [..]
    This subsection provides functions allowing to
      (+) Check the DCMI state.
      (+) Get the specific DCMI error flag.  

@endverbatim
  * @{
  */ 

/**
  * @brief  Return the DCMI state
  * @param  hdcmi: pointer to a DCMI_HandleTypeDef structure that contains
  *                the configuration information for DCMI.
  * @retval HAL state
  */
HAL_DCMI_StateTypeDef HAL_DCMI_GetState(DCMI_HandleTypeDef *hdcmi)  
{
  return hdcmi->State;
}

/**
* @brief  Return the DCMI error code
* @param  hdcmi : pointer to a DCMI_HandleTypeDef structure that contains
  *               the configuration information for DCMI.
* @retval DCMI Error Code
*/
uint32_t HAL_DCMI_GetError(DCMI_HandleTypeDef *hdcmi)
{
  return hdcmi->ErrorCode;
}

/**
  * @}
  */

  /**
  * @brief  DMA conversion complete callback. 
  * @param  hdma: pointer to DMA handle.
  * @retval None
  */
static void DCMI_DMAConvCplt(DMA_HandleTypeDef *hdma)
{
  uint32_t tmp = 0;
 
  DCMI_HandleTypeDef* hdcmi = ( DCMI_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;
  hdcmi->State= HAL_DCMI_STATE_READY;

  if(hdcmi->XferCount != 0)
  {
    /* Update memory 0 address location */
    tmp = ((hdcmi->DMA_Handle->Instance->CR) & DMA_SxCR_CT);
    if(((hdcmi->XferCount % 2) == 0) && (tmp != 0))
    {
      tmp = hdcmi->DMA_Handle->Instance->M0AR;
      HAL_DMAEx_ChangeMemory(hdcmi->DMA_Handle, (tmp + (8*hdcmi->XferSize)), MEMORY0);
      hdcmi->XferCount--;
    }
    /* Update memory 1 address location */
    else if((hdcmi->DMA_Handle->Instance->CR & DMA_SxCR_CT) == 0)
    {
      tmp = hdcmi->DMA_Handle->Instance->M1AR;
      HAL_DMAEx_ChangeMemory(hdcmi->DMA_Handle, (tmp + (8*hdcmi->XferSize)), MEMORY1);
      hdcmi->XferCount--;
    }
  }
  /* Update memory 0 address location */
  else if((hdcmi->DMA_Handle->Instance->CR & DMA_SxCR_CT) != 0)
  {
    hdcmi->DMA_Handle->Instance->M0AR = hdcmi->pBuffPtr;
  }
  /* Update memory 1 address location */
  else if((hdcmi->DMA_Handle->Instance->CR & DMA_SxCR_CT) == 0)
  {
    tmp = hdcmi->pBuffPtr;
    hdcmi->DMA_Handle->Instance->M1AR = (tmp + (4*hdcmi->XferSize));
    hdcmi->XferCount = hdcmi->XferTransferNumber;
  }

  if(__HAL_DCMI_GET_FLAG(hdcmi, DCMI_FLAG_FRAMERI) != RESET)
  {
    /* Process Unlocked */
    __HAL_UNLOCK(hdcmi);

    /* FRAME Callback */
    HAL_DCMI_FrameEventCallback(hdcmi);
  }
}

/**
  * @brief  DMA error callback 
  * @param  hdma: pointer to DMA handle.
  * @retval None
  */
static void DCMI_DMAError(DMA_HandleTypeDef *hdma)
{
    DCMI_HandleTypeDef* hdcmi = ( DCMI_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;     
    hdcmi->State= HAL_DCMI_STATE_READY;
    HAL_DCMI_ErrorCallback(hdcmi);
}

/**
  * @}
  */
#endif /* STM32F405xx || STM32F415xx || STM32F407xx || STM32F417xx || STM32F427xx || STM32F437xx || STM32F429xx || STM32F439xx */
#endif /* HAL_DCMI_MODULE_ENABLED */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
