/**
  ******************************************************************************
  * @file    stm32f4xx_hal_ltdc.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-June-2014
  * @brief   LTDC HAL module driver.
  *          This file provides firmware functions to manage the following 
  *          functionalities of the LTDC peripheral:
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
     (#) Program the required configuration through the following parameters:   
         the LTDC timing, the horizontal and vertical polarity, 
         the pixel clock polarity, Data Enable polarity and the LTDC background color value 
         using HAL_LTDC_Init() function

     (#) Program the required configuration through the following parameters:   
         the pixel format, the blending factors, input alpha value, the window size 
         and the image size using HAL_LTDC_ConfigLayer() function for foreground
         or/and background layer.     
  
     (#) Optionally, configure and enable the CLUT using HAL_LTDC_ConfigCLUT() and 
         HAL_LTDC_EnableCLUT functions.
       
     (#) Optionally, enable the Dither using HAL_LTDC_EnableDither().       

     (#) Optionally, configure and enable the Color keying using HAL_LTDC_ConfigColorKeying()
         and HAL_LTDC_EnableColorKeying functions.

     (#) Optionally, configure LineInterrupt using HAL_LTDC_ProgramLineInterrupt()
         function

     (#) If needed, reconfigure and change the pixel format value, the alpha value
         value, the window size, the window position and the layer start address 
         for foreground or/and background layer using respectively the following 
         functions: HAL_LTDC_SetPixelFormat(), HAL_LTDC_SetAlpha(), HAL_LTDC_SetWindowSize(),
         HAL_LTDC_SetWindowPosition(), HAL_LTDC_SetAddress.
                     
     (#) To control LTDC state you can use the following function: HAL_LTDC_GetState()               

     *** LTDC HAL driver macros list ***
     ============================================= 
     [..]
       Below the list of most used macros in LTDC HAL driver.
       
      (+) __HAL_LTDC_ENABLE: Enable the LTDC.
      (+) __HAL_LTDC_DISABLE: Disable the LTDC.
      (+) __HAL_LTDC_LAYER_ENABLE: Enable the LTDC Layer.
      (+) __HAL_LTDC_LAYER_DISABLE: Disable the LTDC Layer.
      (+) __HAL_LTDC_RELOAD_CONFIG: Reload  Layer Configuration.
      (+) __HAL_LTDC_GET_FLAG: Get the LTDC pending flags.
      (+) __HAL_LTDC_CLEAR_FLAG: Clear the LTDC pending flags.
      (+) __HAL_LTDC_ENABLE_IT: Enable the specified LTDC interrupts. 
      (+) __HAL_LTDC_DISABLE_IT: Disable the specified LTDC interrupts.
      (+) __HAL_LTDC_GET_IT_SOURCE: Check whether the specified LTDC interrupt has occurred or not.
      
     [..] 
       (@) You can refer to the LTDC HAL driver header file for more useful macros
  
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
/** @defgroup LTDC 
  * @brief LTDC HAL module driver
  * @{
  */

#ifdef HAL_LTDC_MODULE_ENABLED

#if defined(STM32F429xx) || defined(STM32F439xx)

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/    
/* Private function prototypes -----------------------------------------------*/
static void LTDC_SetConfig(LTDC_HandleTypeDef *hltdc, LTDC_LayerCfgTypeDef *pLayerCfg, uint32_t LayerIdx);
/* Private functions ---------------------------------------------------------*/

/** @defgroup LTDC_Private_Functions
  * @{
  */

/** @defgroup LTDC_Group1 Initialization and Configuration functions
 *  @brief   Initialization and Configuration functions
 *
@verbatim   
 ===============================================================================
                ##### Initialization and Configuration functions #####
 ===============================================================================  
    [..]  This section provides functions allowing to:
      (+) Initialize and configure the LTDC
      (+) De-initialize the LTDC 

@endverbatim
  * @{
  */
  
/**
  * @brief  Initializes the LTDC according to the specified
  *         parameters in the LTDC_InitTypeDef and create the associated handle.
  * @param  hltdc: pointer to a LTDC_HandleTypeDef structure that contains
  *                the configuration information for the LTDC.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_LTDC_Init(LTDC_HandleTypeDef *hltdc)
{
  uint32_t tmp = 0, tmp1 = 0;

  /* Check the LTDC peripheral state */
  if(hltdc == NULL)
  {
    return HAL_ERROR;
  }

  /* Check function parameters */
  assert_param(IS_LTDC_ALL_INSTANCE(hltdc->Instance));
  assert_param(IS_LTDC_HSYNC(hltdc->Init.HorizontalSync));
  assert_param(IS_LTDC_VSYNC(hltdc->Init.VerticalSync));
  assert_param(IS_LTDC_AHBP(hltdc->Init.AccumulatedHBP));
  assert_param(IS_LTDC_AVBP(hltdc->Init.AccumulatedVBP));
  assert_param(IS_LTDC_AAH(hltdc->Init.AccumulatedActiveH));
  assert_param(IS_LTDC_AAW(hltdc->Init.AccumulatedActiveW));
  assert_param(IS_LTDC_TOTALH(hltdc->Init.TotalHeigh));
  assert_param(IS_LTDC_TOTALW(hltdc->Init.TotalWidth));
  assert_param(IS_LTDC_HSPOL(hltdc->Init.HSPolarity));
  assert_param(IS_LTDC_VSPOL(hltdc->Init.VSPolarity));
  assert_param(IS_LTDC_DEPOL(hltdc->Init.DEPolarity));
  assert_param(IS_LTDC_PCPOL(hltdc->Init.PCPolarity));

  if(hltdc->State == HAL_LTDC_STATE_RESET)
  {
    /* Init the low level hardware */
    HAL_LTDC_MspInit(hltdc);
  }
  
  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  /* Configures the HS, VS, DE and PC polarity */
  hltdc->Instance->GCR &= ~(LTDC_GCR_HSPOL | LTDC_GCR_VSPOL | LTDC_GCR_DEPOL | LTDC_GCR_PCPOL);
  hltdc->Instance->GCR |=  (uint32_t)(hltdc->Init.HSPolarity | hltdc->Init.VSPolarity | \
  hltdc->Init.DEPolarity | hltdc->Init.PCPolarity);

  /* Sets Synchronization size */
  hltdc->Instance->SSCR &= ~(LTDC_SSCR_VSH | LTDC_SSCR_HSW);
  tmp = (hltdc->Init.HorizontalSync << 16);
  hltdc->Instance->SSCR |= (tmp | hltdc->Init.VerticalSync);

  /* Sets Accumulated Back porch */
  hltdc->Instance->BPCR &= ~(LTDC_BPCR_AVBP | LTDC_BPCR_AHBP);
  tmp = (hltdc->Init.AccumulatedHBP << 16);
  hltdc->Instance->BPCR |= (tmp | hltdc->Init.AccumulatedVBP);

  /* Sets Accumulated Active Width */
  hltdc->Instance->AWCR &= ~(LTDC_AWCR_AAH | LTDC_AWCR_AAW);
  tmp = (hltdc->Init.AccumulatedActiveW << 16);
  hltdc->Instance->AWCR |= (tmp | hltdc->Init.AccumulatedActiveH);

  /* Sets Total Width */
  hltdc->Instance->TWCR &= ~(LTDC_TWCR_TOTALH | LTDC_TWCR_TOTALW);
  tmp = (hltdc->Init.TotalWidth << 16);
  hltdc->Instance->TWCR |= (tmp | hltdc->Init.TotalHeigh);

  /* Sets the background color value */
  tmp = ((uint32_t)(hltdc->Init.Backcolor.Green) << 8);
  tmp1 = ((uint32_t)(hltdc->Init.Backcolor.Red) << 16);
  hltdc->Instance->BCCR &= ~(LTDC_BCCR_BCBLUE | LTDC_BCCR_BCGREEN | LTDC_BCCR_BCRED);
  hltdc->Instance->BCCR |= (tmp1 | tmp | hltdc->Init.Backcolor.Blue);

  /* Enable the transfer Error interrupt */
  __HAL_LTDC_ENABLE_IT(hltdc, LTDC_IT_TE);

  /* Enable the FIFO underrun interrupt */
  __HAL_LTDC_ENABLE_IT(hltdc, LTDC_IT_FU);

  /* Enable LTDC by setting LTDCEN bit */
  __HAL_LTDC_ENABLE(hltdc);

  /* Initialise the error code */
  hltdc->ErrorCode = HAL_LTDC_ERROR_NONE;  

  /* Initialize the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_READY;

  return HAL_OK;
}

/**
  * @brief  Deinitializes the LTDC peripheral registers to their default reset
  *         values.
  * @param  hltdc: pointer to a LTDC_HandleTypeDef structure that contains
  *                the configuration information for the LTDC.
  * @retval None
  */

HAL_StatusTypeDef HAL_LTDC_DeInit(LTDC_HandleTypeDef *hltdc)
{
  /* DeInit the low level hardware */
  HAL_LTDC_MspDeInit(hltdc); 

  /* Initialise the error code */
  hltdc->ErrorCode = HAL_LTDC_ERROR_NONE;

  /* Initialize the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_RESET;

  /* Release Lock */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}

/**
  * @brief  Initializes the LTDC MSP.
  * @param  hltdc : pointer to a LTDC_HandleTypeDef structure that contains
  *                the configuration information for the LTDC.
  * @retval None
  */
__weak void HAL_LTDC_MspInit(LTDC_HandleTypeDef* hltdc)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_LTDC_MspInit could be implemented in the user file
   */ 
}

/**
  * @brief  DeInitializes the LTDC MSP.
  * @param  hltdc : pointer to a LTDC_HandleTypeDef structure that contains
  *                the configuration information for the LTDC.
  * @retval None
  */
__weak void HAL_LTDC_MspDeInit(LTDC_HandleTypeDef* hltdc)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_LTDC_MspDeInit could be implemented in the user file
   */
}

/**
  * @}
  */
  
/** @defgroup LTDC_Group2 IO operation functions 
 *  @brief   IO operation functions  
 *
@verbatim
 ===============================================================================
                      #####  IO operation functions  #####
 ===============================================================================  
    [..]  This section provides function allowing to:
      (+) Handle LTDC interrupt request

@endverbatim
  * @{
  */
/**
  * @brief  Handles LTDC interrupt request.
  * @param  hltdc: pointer to a LTDC_HandleTypeDef structure that contains
  *                the configuration information for the LTDC.  
  * @retval HAL status
  */
void HAL_LTDC_IRQHandler(LTDC_HandleTypeDef *hltdc)
{
  /* Transfer Error Interrupt management ***************************************/
  if(__HAL_LTDC_GET_FLAG(hltdc, LTDC_FLAG_TE) != RESET)
  {
    if(__HAL_LTDC_GET_IT_SOURCE(hltdc, LTDC_IT_TE) != RESET)
    {
      /* Disable the transfer Error interrupt */
      __HAL_LTDC_DISABLE_IT(hltdc, LTDC_IT_TE);

      /* Clear the transfer error flag */
      __HAL_LTDC_CLEAR_FLAG(hltdc, LTDC_FLAG_TE);

      /* Update error code */
      hltdc->ErrorCode |= HAL_LTDC_ERROR_TE;

      /* Change LTDC state */
      hltdc->State = HAL_LTDC_STATE_ERROR;

      /* Process unlocked */
      __HAL_UNLOCK(hltdc);

      /* Transfer error Callback */
      HAL_LTDC_ErrorCallback(hltdc);
    }
  }
  /* FIFO underrun Interrupt management ***************************************/
  if(__HAL_LTDC_GET_FLAG(hltdc, LTDC_FLAG_FU) != RESET)
  {
    if(__HAL_LTDC_GET_IT_SOURCE(hltdc, LTDC_IT_FU) != RESET)
    {
      /* Disable the FIFO underrun interrupt */
      __HAL_LTDC_DISABLE_IT(hltdc, LTDC_IT_FU);

      /* Clear the FIFO underrun flag */
      __HAL_LTDC_CLEAR_FLAG(hltdc, LTDC_FLAG_FU);

      /* Update error code */
      hltdc->ErrorCode |= HAL_LTDC_ERROR_FU;

      /* Change LTDC state */
      hltdc->State = HAL_LTDC_STATE_ERROR;

      /* Process unlocked */
      __HAL_UNLOCK(hltdc);
      
      /* Transfer error Callback */
      HAL_LTDC_ErrorCallback(hltdc);
    }
  }
  /* Line Interrupt management ************************************************/
  if(__HAL_LTDC_GET_FLAG(hltdc, LTDC_FLAG_LI) != RESET)
  {
    if(__HAL_LTDC_GET_IT_SOURCE(hltdc, LTDC_IT_LI) != RESET)
    {
      /* Disable the Line interrupt */
      __HAL_LTDC_DISABLE_IT(hltdc, LTDC_IT_LI);

      /* Clear the Line interrupt flag */  
      __HAL_LTDC_CLEAR_FLAG(hltdc, LTDC_FLAG_LI);

      /* Change LTDC state */
      hltdc->State = HAL_LTDC_STATE_READY;

      /* Process unlocked */
      __HAL_UNLOCK(hltdc);

      /* Line interrupt Callback */
      HAL_LTDC_LineEvenCallback(hltdc);
    }
  }
}

/**
  * @brief  Error LTDC callback.
  * @param  hltdc: pointer to a LTDC_HandleTypeDef structure that contains
  *                the configuration information for the LTDC.
  * @retval None
  */
__weak void HAL_LTDC_ErrorCallback(LTDC_HandleTypeDef *hltdc)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_LTDC_ErrorCallback could be implemented in the user file
   */
}

/**
  * @brief  Line Event callback.
  * @param  hltdc: pointer to a LTDC_HandleTypeDef structure that contains
  *                the configuration information for the LTDC.
  * @retval None
  */
__weak void HAL_LTDC_LineEvenCallback(LTDC_HandleTypeDef *hltdc)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_LTDC_LineEvenCallback could be implemented in the user file
   */
}

/**
  * @}
  */

/** @defgroup LTDC_Group3 Peripheral Control functions
 *  @brief    Peripheral Control functions 
 *
@verbatim   
 ===============================================================================
                    ##### Peripheral Control functions #####
 ===============================================================================  
    [..]  This section provides functions allowing to:
      (+) Configure the LTDC foreground or/and background parameters.
      (+) Set the active layer.
      (+) Configure the color keying.
      (+) Configure the C-LUT.
      (+) Enable / Disable the color keying.
      (+) Enable / Disable the C-LUT.
      (+) Update the layer position.
      (+) Update the layer size.
      (+) Update pixel format on the fly. 
      (+) Update transparency on the fly.
      (+) Update address on the fly.

@endverbatim
  * @{
  */

/**
  * @brief  Configure the LTDC Layer according to the specified
  *         parameters in the LTDC_InitTypeDef and create the associated handle.
  * @param  hltdc:     pointer to a LTDC_HandleTypeDef structure that contains
  *                    the configuration information for the LTDC.
  * @param  pLayerCfg: pointer to a LTDC_LayerCfgTypeDef structure that contains
  *                    the configuration information for the Layer.
  * @param  LayerIdx:  LTDC Layer index.
  *                    This parameter can be one of the following values:
  *                    0 or 1
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_LTDC_ConfigLayer(LTDC_HandleTypeDef *hltdc, LTDC_LayerCfgTypeDef *pLayerCfg, uint32_t LayerIdx)
{   
  /* Process locked */
  __HAL_LOCK(hltdc);
  
  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  /* Check the parameters */
  assert_param(IS_LTDC_LAYER(LayerIdx));
  assert_param(IS_LTDC_PIXEL_FORMAT(pLayerCfg->PixelFormat));
  assert_param(IS_LTDC_BLENDING_FACTOR1(pLayerCfg->BlendingFactor1));
  assert_param(IS_LTDC_BLENDING_FACTOR2(pLayerCfg->BlendingFactor2));
  assert_param(IS_LTDC_HCONFIGST(pLayerCfg->WindowX0));
  assert_param(IS_LTDC_HCONFIGSP(pLayerCfg->WindowX1));
  assert_param(IS_LTDC_VCONFIGST(pLayerCfg->WindowY0));
  assert_param(IS_LTDC_VCONFIGSP(pLayerCfg->WindowY1));
  assert_param(IS_LTDC_ALPHA(pLayerCfg->Alpha0));
  assert_param(IS_LTDC_CFBLL(pLayerCfg->ImageWidth));
  assert_param(IS_LTDC_CFBLNBR(pLayerCfg->ImageHeight));

  /* Copy new layer configuration into handle structure */
  hltdc->LayerCfg[LayerIdx] = *pLayerCfg;  

  /* Configure the LTDC Layer */  
  LTDC_SetConfig(hltdc, pLayerCfg, LayerIdx);

  /* Sets the Reload type */
  hltdc->Instance->SRCR = LTDC_SRCR_IMR;

  /* Initialize the LTDC state*/
  hltdc->State  = HAL_LTDC_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}

/**
  * @brief  Configure the color keying.
  * @param  hltdc:    pointer to a LTDC_HandleTypeDef structure that contains
  *                   the configuration information for the LTDC.
  * @param  RGBValue: the color key value
  * @param  LayerIdx:  LTDC Layer index.
  *                   This parameter can be one of the following values:
  *                   0 or 1
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_LTDC_ConfigColorKeying(LTDC_HandleTypeDef *hltdc, uint32_t RGBValue, uint32_t LayerIdx)
{
  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  /* Check the parameters */
  assert_param(IS_LTDC_LAYER(LayerIdx));

  /* Configures the default color values */
  __HAL_LTDC_LAYER(hltdc, LayerIdx)->CKCR &=  ~(LTDC_LxCKCR_CKBLUE | LTDC_LxCKCR_CKGREEN | LTDC_LxCKCR_CKRED);
  __HAL_LTDC_LAYER(hltdc, LayerIdx)->CKCR  = RGBValue;

  /* Sets the Reload type */
  hltdc->Instance->SRCR = LTDC_SRCR_IMR;

  /* Change the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}

/**
  * @brief  Load the color lookup table.
  * @param  hltdc:    pointer to a LTDC_HandleTypeDef structure that contains
  *                   the configuration information for the LTDC.
  * @param  pCLUT:    pointer to the color lookup table address.
  * @param  CLUTSize: the color lookup table size.  
  * @param  LayerIdx:  LTDC Layer index.
  *                   This parameter can be one of the following values:
  *                   0 or 1
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_LTDC_ConfigCLUT(LTDC_HandleTypeDef *hltdc, uint32_t *pCLUT, uint32_t CLUTSize, uint32_t LayerIdx)
{
  uint32_t tmp = 0;
  uint32_t counter = 0;
  uint32_t pcounter = 0;

  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;  

  /* Check the parameters */
  assert_param(IS_LTDC_LAYER(LayerIdx)); 

  for(counter = 0; (counter < CLUTSize); counter++)
  {
    tmp  = ((counter << 24) | ((uint32_t)(*pCLUT) & 0xFF) | ((uint32_t)(*pCLUT) & 0xFF00) | ((uint32_t)(*pCLUT) & 0xFF0000));
    pcounter = (uint32_t)pCLUT + sizeof(*pCLUT);
    pCLUT = (uint32_t *)pcounter;

    /* Specifies the C-LUT address and RGB value */
    __HAL_LTDC_LAYER(hltdc, LayerIdx)->CLUTWR  = tmp;
  }
  
  /* Change the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_READY; 

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);  

  return HAL_OK;
}

/**
  * @brief  Enable the color keying.
  * @param  hltdc:    pointer to a LTDC_HandleTypeDef structure that contains
  *                   the configuration information for the LTDC.
  * @param  LayerIdx:  LTDC Layer index.
  *                   This parameter can be one of the following values:
  *                   0 or 1
  * @retval  HAL status
  */
HAL_StatusTypeDef HAL_LTDC_EnableColorKeying(LTDC_HandleTypeDef *hltdc, uint32_t LayerIdx)
{  
  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  /* Check the parameters */
  assert_param(IS_LTDC_LAYER(LayerIdx));

  /* Enable LTDC color keying by setting COLKEN bit */
  __HAL_LTDC_LAYER(hltdc, LayerIdx)->CR |= (uint32_t)LTDC_LxCR_COLKEN;

  /* Sets the Reload type */
  hltdc->Instance->SRCR = LTDC_SRCR_IMR;

  /* Change the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_READY; 

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;  
}
  
/**
  * @brief  Disable the color keying.
  * @param  hltdc:    pointer to a LTDC_HandleTypeDef structure that contains
  *                   the configuration information for the LTDC.
  * @param  LayerIdx:  LTDC Layer index.
  *                   This parameter can be one of the following values:
  *                   0 or 1
  * @retval  HAL status
  */
HAL_StatusTypeDef HAL_LTDC_DisableColorKeying(LTDC_HandleTypeDef *hltdc, uint32_t LayerIdx)
{
  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  /* Check the parameters */
  assert_param(IS_LTDC_LAYER(LayerIdx));

  /* Disable LTDC color keying by setting COLKEN bit */
  __HAL_LTDC_LAYER(hltdc, LayerIdx)->CR &= ~(uint32_t)LTDC_LxCR_COLKEN;

  /* Sets the Reload type */
  hltdc->Instance->SRCR = LTDC_SRCR_IMR;

  /* Change the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_READY; 

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}

/**
  * @brief  Enable the color lookup table.
  * @param  hltdc:    pointer to a LTDC_HandleTypeDef structure that contains
  *                   the configuration information for the LTDC.
  * @param  LayerIdx:  LTDC Layer index.
  *                   This parameter can be one of the following values:
  *                   0 or 1
  * @retval  HAL status
  */
HAL_StatusTypeDef HAL_LTDC_EnableCLUT(LTDC_HandleTypeDef *hltdc, uint32_t LayerIdx)
{

  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  /* Check the parameters */
  assert_param(IS_LTDC_LAYER(LayerIdx));

  /* Disable LTDC color lookup table by setting CLUTEN bit */
  __HAL_LTDC_LAYER(hltdc, LayerIdx)->CR |= (uint32_t)LTDC_LxCR_CLUTEN;

  /* Sets the Reload type */
  hltdc->Instance->SRCR = LTDC_SRCR_IMR;

  /* Change the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_READY; 

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}

/**
  * @brief  Disable the color lookup table.
  * @param  hltdc:    pointer to a LTDC_HandleTypeDef structure that contains
  *                   the configuration information for the LTDC.
  * @param  LayerIdx:  LTDC Layer index.
  *                   This parameter can be one of the following values:
  *                   0 or 1   
  * @retval  HAL status
  */
HAL_StatusTypeDef HAL_LTDC_DisableCLUT(LTDC_HandleTypeDef *hltdc, uint32_t LayerIdx)
{
 
  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  /* Check the parameters */
  assert_param(IS_LTDC_LAYER(LayerIdx));

  /* Disable LTDC color lookup table by setting CLUTEN bit */
  __HAL_LTDC_LAYER(hltdc, LayerIdx)->CR &= ~(uint32_t)LTDC_LxCR_CLUTEN;

  /* Sets the Reload type */
  hltdc->Instance->SRCR = LTDC_SRCR_IMR;

  /* Change the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_READY; 

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}

/**
  * @brief  Enables Dither.
  * @param  hltdc: pointer to a LTDC_HandleTypeDef structure that contains
  *                the configuration information for the LTDC.
  * @retval  HAL status
  */

HAL_StatusTypeDef HAL_LTDC_EnableDither(LTDC_HandleTypeDef *hltdc)
{
  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  /* Enable Dither by setting DTEN bit */
  LTDC->GCR |= (uint32_t)LTDC_GCR_DTEN;

  /* Change the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_READY; 

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}

/**
  * @brief  Disables Dither.
  * @param  hltdc: pointer to a LTDC_HandleTypeDef structure that contains
  *                the configuration information for the LTDC.
  * @retval  HAL status
  */

HAL_StatusTypeDef HAL_LTDC_DisableDither(LTDC_HandleTypeDef *hltdc)
{
  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  /* Disable Dither by setting DTEN bit */
  LTDC->GCR &= ~(uint32_t)LTDC_GCR_DTEN;

  /* Change the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}

/**
  * @brief  Set the LTDC window size.
  * @param  hltdc:    pointer to a LTDC_HandleTypeDef structure that contains
  *                   the configuration information for the LTDC.
  * @param  XSize:    LTDC Pixel per line
  * @param  YSize:    LTDC Line number
  * @param  LayerIdx:  LTDC Layer index.
  *                   This parameter can be one of the following values:
  *                   0 or 1
  * @retval  HAL status
  */
HAL_StatusTypeDef HAL_LTDC_SetWindowSize(LTDC_HandleTypeDef *hltdc, uint32_t XSize, uint32_t YSize, uint32_t LayerIdx) 
{
  LTDC_LayerCfgTypeDef *pLayerCfg;

  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY; 

  /* Get layer configuration from handle structure */
  pLayerCfg = &hltdc->LayerCfg[LayerIdx];

  /* Check the parameters (Layers parameters)*/
  assert_param(IS_LTDC_LAYER(LayerIdx));
  assert_param(IS_LTDC_HCONFIGST(pLayerCfg->WindowX0));
  assert_param(IS_LTDC_HCONFIGSP(pLayerCfg->WindowX1));
  assert_param(IS_LTDC_VCONFIGST(pLayerCfg->WindowY0));
  assert_param(IS_LTDC_VCONFIGSP(pLayerCfg->WindowY1));
  assert_param(IS_LTDC_CFBLL(XSize));
  assert_param(IS_LTDC_CFBLNBR(YSize));

  /* update horizontal start/stop */
  pLayerCfg->WindowX0 = 0;
  pLayerCfg->WindowX1 = XSize + pLayerCfg->WindowX0;

  /* update vertical start/stop */  
  pLayerCfg->WindowY0 = 0;
  pLayerCfg->WindowY1 = YSize + pLayerCfg->WindowY0;

  /* Reconfigures the color frame buffer pitch in byte */
  pLayerCfg->ImageWidth = XSize;

  /* Reconfigures the frame buffer line number */
  pLayerCfg->ImageHeight = YSize;

  /* Set LTDC parameters */
  LTDC_SetConfig(hltdc, pLayerCfg, LayerIdx);

  /* Sets the Reload type */
  hltdc->Instance->SRCR = LTDC_SRCR_IMR;

  /* Change the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}

/**
  * @brief  Set the LTDC window position.
  * @param  hltdc:    pointer to a LTDC_HandleTypeDef structure that contains
  *                   the configuration information for the LTDC.
  * @param  X0:       LTDC window X offset
  * @param  Y0:       LTDC window Y offset
  * @param  LayerIdx:  LTDC Layer index.
  *                         This parameter can be one of the following values:
  *                         0 or 1
  * @retval  HAL status
  */
HAL_StatusTypeDef HAL_LTDC_SetWindowPosition(LTDC_HandleTypeDef *hltdc, uint32_t X0, uint32_t Y0, uint32_t LayerIdx)
{
  LTDC_LayerCfgTypeDef *pLayerCfg;
  
  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  /* Get layer configuration from handle structure */
  pLayerCfg = &hltdc->LayerCfg[LayerIdx];

  /* Check the parameters */
  assert_param(IS_LTDC_LAYER(LayerIdx));
  assert_param(IS_LTDC_HCONFIGST(pLayerCfg->WindowX0));
  assert_param(IS_LTDC_HCONFIGSP(pLayerCfg->WindowX1));
  assert_param(IS_LTDC_VCONFIGST(pLayerCfg->WindowY0));
  assert_param(IS_LTDC_VCONFIGSP(pLayerCfg->WindowY1));

  /* update horizontal start/stop */
  pLayerCfg->WindowX0 = X0;
  pLayerCfg->WindowX1 = X0 + pLayerCfg->ImageWidth;

  /* update vertical start/stop */
  pLayerCfg->WindowY0 = Y0;
  pLayerCfg->WindowY1 = Y0 + pLayerCfg->ImageHeight;

  /* Set LTDC parameters */
  LTDC_SetConfig(hltdc, pLayerCfg, LayerIdx);

  /* Sets the Reload type */
  hltdc->Instance->SRCR = LTDC_SRCR_IMR;

  /* Change the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}

/**
  * @brief  Reconfigure the pixel format.
  * @param  hltdc:       pointer to a LTDC_HandleTypeDef structure that contains
  *                      the configuration information for the LTDC.
  * @param  Pixelformat: new pixel format value.
  * @param  LayerIdx:    LTDC Layer index.
  *                      This parameter can be one of the following values:
  *                      0 or 1.
  * @retval  HAL status
  */
HAL_StatusTypeDef HAL_LTDC_SetPixelFormat(LTDC_HandleTypeDef *hltdc, uint32_t Pixelformat, uint32_t LayerIdx)
{
  LTDC_LayerCfgTypeDef *pLayerCfg;

  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  /* Check the parameters */
  assert_param(IS_LTDC_LAYER(LayerIdx));
  assert_param(IS_LTDC_PIXEL_FORMAT(Pixelformat));

  /* Get layer configuration from handle structure */
  pLayerCfg = &hltdc->LayerCfg[LayerIdx];  

  /* Reconfigure the pixel format */
  pLayerCfg->PixelFormat = Pixelformat;

  /* Set LTDC parameters */
  LTDC_SetConfig(hltdc, pLayerCfg, LayerIdx);   

  /* Sets the Reload type */
  hltdc->Instance->SRCR = LTDC_SRCR_IMR;

  /* Change the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}

/**
  * @brief  Reconfigure the layer alpha value.
  * @param  hltdc:    pointer to a LTDC_HandleTypeDef structure that contains
  *                   the configuration information for the LTDC.
  * @param  Alpha:    new alpha value.
  * @param  LayerIdx:  LTDC Layer index.
  *                   This parameter can be one of the following values:
  *                   0 or 1
  * @retval  HAL status
  */
HAL_StatusTypeDef HAL_LTDC_SetAlpha(LTDC_HandleTypeDef *hltdc, uint32_t Alpha, uint32_t LayerIdx)
{
  LTDC_LayerCfgTypeDef *pLayerCfg;

  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  /* Check the parameters */
  assert_param(IS_LTDC_ALPHA(Alpha));
  assert_param(IS_LTDC_LAYER(LayerIdx));

  /* Get layer configuration from handle structure */
  pLayerCfg = &hltdc->LayerCfg[LayerIdx];

  /* Reconfigure the Alpha value */
  pLayerCfg->Alpha = Alpha;

  /* Set LTDC parameters */
  LTDC_SetConfig(hltdc, pLayerCfg, LayerIdx);

  /* Sets the Reload type */
  hltdc->Instance->SRCR = LTDC_SRCR_IMR;

  /* Change the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}
/**
  * @brief  Reconfigure the frame buffer Address.
  * @param  hltdc:    pointer to a LTDC_HandleTypeDef structure that contains
  *                   the configuration information for the LTDC.
  * @param  Address:  new address value.
  * @param  LayerIdx: LTDC Layer index.
  *                   This parameter can be one of the following values:
  *                   0 or 1.
  * @retval  HAL status
  */
HAL_StatusTypeDef HAL_LTDC_SetAddress(LTDC_HandleTypeDef *hltdc, uint32_t Address, uint32_t LayerIdx)
{
  LTDC_LayerCfgTypeDef *pLayerCfg;

  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  /* Check the parameters */
  assert_param(IS_LTDC_LAYER(LayerIdx));

  /* Get layer configuration from handle structure */
  pLayerCfg = &hltdc->LayerCfg[LayerIdx];

  /* Reconfigure the Address */
  pLayerCfg->FBStartAdress = Address;

  /* Set LTDC parameters */
  LTDC_SetConfig(hltdc, pLayerCfg, LayerIdx);

  /* Sets the Reload type */
  hltdc->Instance->SRCR = LTDC_SRCR_IMR;

  /* Change the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}

/**
  * @brief  Define the position of the line interrupt .
  * @param  hltdc:             pointer to a LTDC_HandleTypeDef structure that contains
  *                            the configuration information for the LTDC.
  * @param  Line:   Line Interrupt Position.
  * @retval  HAL status
  */
HAL_StatusTypeDef HAL_LTDC_ProgramLineEvent(LTDC_HandleTypeDef *hltdc, uint32_t Line)
{
  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  /* Check the parameters */
  assert_param(IS_LTDC_LIPOS(Line));

  /* Enable the Line interrupt */
  __HAL_LTDC_ENABLE_IT(hltdc, LTDC_IT_LI);

  /* Sets the Line Interrupt position */
  LTDC->LIPCR = (uint32_t)Line;

  /* Change the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}

/**
  * @}
  */

/** @defgroup LTDC_Group4 Peripheral State and Errors functions
 *  @brief    Peripheral State and Errors functions 
 *
@verbatim   
 ===============================================================================
                  ##### Peripheral State and Errors functions #####
 ===============================================================================  
    [..]
    This subsection provides functions allowing to
      (+) Check the LTDC state.
      (+) Get error code.  

@endverbatim
  * @{
  */ 

/**
  * @brief  Return the LTDC state
  * @param  hltdc: pointer to a LTDC_HandleTypeDef structure that contains
  *                the configuration information for the LTDC.
  * @retval HAL state
  */
HAL_LTDC_StateTypeDef HAL_LTDC_GetState(LTDC_HandleTypeDef *hltdc)
{
  return hltdc->State;
}

/**
* @brief  Return the LTDC error code
* @param  hltdc : pointer to a LTDC_HandleTypeDef structure that contains
  *               the configuration information for the LTDC.
* @retval LTDC Error Code
*/
uint32_t HAL_LTDC_GetError(LTDC_HandleTypeDef *hltdc)
{
  return hltdc->ErrorCode;
}

/**
  * @}
  */

/**
  * @brief  Configures the LTDC peripheral 
  * @param  hltdc   :  Pointer to a LTDC_HandleTypeDef structure that contains
  *                   the configuration information for the LTDC.
  * @param  pLayerCfg: Pointer LTDC Layer Configuration strusture
  * @param  LayerIdx:  LTDC Layer index.
  *                    This parameter can be one of the following values: 0 or 1
  * @retval None
  */
static void LTDC_SetConfig(LTDC_HandleTypeDef *hltdc, LTDC_LayerCfgTypeDef *pLayerCfg, uint32_t LayerIdx)
{
  uint32_t tmp = 0;
  uint32_t tmp1 = 0;
  uint32_t tmp2 = 0;

  /* Configures the horizontal start and stop position */
  tmp = ((pLayerCfg->WindowX1 + ((hltdc->Instance->BPCR & LTDC_BPCR_AHBP) >> 16)) << 16);
  __HAL_LTDC_LAYER(hltdc, LayerIdx)->WHPCR &= ~(LTDC_LxWHPCR_WHSTPOS | LTDC_LxWHPCR_WHSPPOS);
  __HAL_LTDC_LAYER(hltdc, LayerIdx)->WHPCR = ((pLayerCfg->WindowX0 + ((hltdc->Instance->BPCR & LTDC_BPCR_AHBP) >> 16) + 1) | tmp);

  /* Configures the vertical start and stop position */
  tmp = ((pLayerCfg->WindowY1 + (hltdc->Instance->BPCR & LTDC_BPCR_AVBP)) << 16);
  __HAL_LTDC_LAYER(hltdc, LayerIdx)->WVPCR &= ~(LTDC_LxWVPCR_WVSTPOS | LTDC_LxWVPCR_WVSPPOS);
  __HAL_LTDC_LAYER(hltdc, LayerIdx)->WVPCR  = ((pLayerCfg->WindowY0 + (hltdc->Instance->BPCR & LTDC_BPCR_AVBP) + 1) | tmp);  

  /* Specifies the pixel format */
  __HAL_LTDC_LAYER(hltdc, LayerIdx)->PFCR &= ~(LTDC_LxPFCR_PF);
  __HAL_LTDC_LAYER(hltdc, LayerIdx)->PFCR = (pLayerCfg->PixelFormat);

  /* Configures the default color values */
  tmp = ((uint32_t)(pLayerCfg->Backcolor.Green) << 8);
  tmp1 = ((uint32_t)(pLayerCfg->Backcolor.Red) << 16);
  tmp2 = (pLayerCfg->Alpha0 << 24);  
  __HAL_LTDC_LAYER(hltdc, LayerIdx)->DCCR &= ~(LTDC_LxDCCR_DCBLUE | LTDC_LxDCCR_DCGREEN | LTDC_LxDCCR_DCRED | LTDC_LxDCCR_DCALPHA);
  __HAL_LTDC_LAYER(hltdc, LayerIdx)->DCCR = (pLayerCfg->Backcolor.Blue | tmp | tmp1 | tmp2); 

  /* Specifies the constant alpha value */
  __HAL_LTDC_LAYER(hltdc, LayerIdx)->CACR &= ~(LTDC_LxCACR_CONSTA);
  __HAL_LTDC_LAYER(hltdc, LayerIdx)->CACR = (pLayerCfg->Alpha);

  /* Specifies the blending factors */
  __HAL_LTDC_LAYER(hltdc, LayerIdx)->BFCR &= ~(LTDC_LxBFCR_BF2 | LTDC_LxBFCR_BF1);
  __HAL_LTDC_LAYER(hltdc, LayerIdx)->BFCR = (pLayerCfg->BlendingFactor1 | pLayerCfg->BlendingFactor2);

  /* Configures the color frame buffer start address */
  __HAL_LTDC_LAYER(hltdc, LayerIdx)->CFBAR &= ~(LTDC_LxCFBAR_CFBADD);
  __HAL_LTDC_LAYER(hltdc, LayerIdx)->CFBAR = (pLayerCfg->FBStartAdress);

  if(pLayerCfg->PixelFormat == LTDC_PIXEL_FORMAT_ARGB8888)
  {
    tmp = 4;
  }
  else if (pLayerCfg->PixelFormat == LTDC_PIXEL_FORMAT_RGB888)
  {
    tmp = 3;
  }
  else if((pLayerCfg->PixelFormat == LTDC_PIXEL_FORMAT_ARGB4444) || \
    (pLayerCfg->PixelFormat == LTDC_PIXEL_FORMAT_RGB565)   || \
      (pLayerCfg->PixelFormat == LTDC_PIXEL_FORMAT_ARGB1555) || \
        (pLayerCfg->PixelFormat == LTDC_PIXEL_FORMAT_AL88))
  {
    tmp = 2;
  }
  else
  {
    tmp = 1;
  }

  /* Configures the color frame buffer pitch in byte */
  __HAL_LTDC_LAYER(hltdc, LayerIdx)->CFBLR  &= ~(LTDC_LxCFBLR_CFBLL | LTDC_LxCFBLR_CFBP);
  __HAL_LTDC_LAYER(hltdc, LayerIdx)->CFBLR  = (((pLayerCfg->ImageWidth * tmp) << 16) | ((pLayerCfg->ImageWidth * tmp)  + 3));

  /* Configures the frame buffer line number */
  __HAL_LTDC_LAYER(hltdc, LayerIdx)->CFBLNR  &= ~(LTDC_LxCFBLNR_CFBLNBR);
  __HAL_LTDC_LAYER(hltdc, LayerIdx)->CFBLNR  = (pLayerCfg->ImageHeight);

  /* Enable LTDC_Layer by setting LEN bit */  
  __HAL_LTDC_LAYER(hltdc, LayerIdx)->CR |= (uint32_t)LTDC_LxCR_LEN;
}

/**
  * @}
  */
#endif /* STM32F429xx || STM32F439xx */
#endif /* HAL_LTDC_MODULE_ENABLED */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
