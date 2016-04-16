/**
  ******************************************************************************
  * @file    stm32l4xx_hal_dac_ex.c
  * @author  MCD Application Team
  * @version V1.3.0
  * @date    29-January-2016
  * @brief   DAC HAL module driver.
  *          This file provides firmware functions to manage the extended 
  *          functionalities of the DAC peripheral.  
  *     
  *
  @verbatim      
  ==============================================================================
                      ##### How to use this driver #####
  ==============================================================================
    [..]          
      (+) When Dual mode is enabled (i.e. DAC Channel1 and Channel2 are used simultaneously) :
          Use HAL_DACEx_DualGetValue() to get digital data to be converted and use
          HAL_DACEx_DualSetValue() to set digital value to converted simultaneously in Channel 1 and Channel 2.  
      (+) Use HAL_DACEx_TriangleWaveGenerate() to generate Triangle signal.
      (+) Use HAL_DACEx_NoiseWaveGenerate() to generate Noise signal.

      (+) HAL_DACEx_SelfCalibrate to calibrate one DAC channel.
      (+) HAL_DACEx_SetUserTrimming to set user trimming value.
      (+) HAL_DACEx_GetTrimOffset to retrieve trimming value (factory setting 
          after reset, user setting if HAL_DACEx_SetUserTrimming have been used 
          at least one time after reset).

 @endverbatim    
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

/** @addtogroup STM32L4xx_HAL_Driver
  * @{
  */

/** @defgroup DACEx DACEx
  * @brief DAC Extended HAL module driver
  * @{
  */ 

#ifdef HAL_DAC_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
  
/** @defgroup DACEx_Exported_Functions DACEx Exported Functions
  * @{
  */

/** @defgroup DACEx_Exported_Functions_Group2 IO operation functions
 *  @brief    Extended IO operation functions 
 *
@verbatim   
  ==============================================================================
                 ##### Extended features functions #####
  ==============================================================================  
    [..]  This section provides functions allowing to:
      (+) Start conversion.
      (+) Stop conversion.
      (+) Start conversion and enable DMA transfer.
      (+) Stop conversion and disable DMA transfer.
      (+) Get result of conversion.
      (+) Get result of dual mode conversion.
                     
@endverbatim
  * @{
  */

/**
  * @brief  Enable or disable the selected DAC channel wave generation.
  * @param  hdac: pointer to a DAC_HandleTypeDef structure that contains
  *         the configuration information for the specified DAC.
  * @param  Channel: The selected DAC channel. 
  *          This parameter can be one of the following values:
  *            DAC_CHANNEL_1 / DAC_CHANNEL_2
  * @param  Amplitude: Select max triangle amplitude. 
  *          This parameter can be one of the following values:
  *            @arg DAC_TRIANGLEAMPLITUDE_1: Select max triangle amplitude of 1
  *            @arg DAC_TRIANGLEAMPLITUDE_3: Select max triangle amplitude of 3
  *            @arg DAC_TRIANGLEAMPLITUDE_7: Select max triangle amplitude of 7
  *            @arg DAC_TRIANGLEAMPLITUDE_15: Select max triangle amplitude of 15
  *            @arg DAC_TRIANGLEAMPLITUDE_31: Select max triangle amplitude of 31
  *            @arg DAC_TRIANGLEAMPLITUDE_63: Select max triangle amplitude of 63
  *            @arg DAC_TRIANGLEAMPLITUDE_127: Select max triangle amplitude of 127
  *            @arg DAC_TRIANGLEAMPLITUDE_255: Select max triangle amplitude of 255
  *            @arg DAC_TRIANGLEAMPLITUDE_511: Select max triangle amplitude of 511
  *            @arg DAC_TRIANGLEAMPLITUDE_1023: Select max triangle amplitude of 1023
  *            @arg DAC_TRIANGLEAMPLITUDE_2047: Select max triangle amplitude of 2047
  *            @arg DAC_TRIANGLEAMPLITUDE_4095: Select max triangle amplitude of 4095                               
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DACEx_TriangleWaveGenerate(DAC_HandleTypeDef* hdac, uint32_t Channel, uint32_t Amplitude)
{  
  /* Check the parameters */
  assert_param(IS_DAC_CHANNEL(Channel));
  assert_param(IS_DAC_LFSR_UNMASK_TRIANGLE_AMPLITUDE(Amplitude));
  
  /* Process locked */
  __HAL_LOCK(hdac);
  
  /* Change DAC state */
  hdac->State = HAL_DAC_STATE_BUSY;
  
  /* Enable the triangle wave generation for the selected DAC channel */
  MODIFY_REG(hdac->Instance->CR, ((DAC_CR_WAVE1)|(DAC_CR_MAMP1))<<Channel, (DAC_CR_WAVE1_1 | Amplitude) << Channel);
  
  /* Change DAC state */
  hdac->State = HAL_DAC_STATE_READY;
  
  /* Process unlocked */
  __HAL_UNLOCK(hdac);
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Enable or disable the selected DAC channel wave generation.
  * @param  hdac: pointer to a DAC_HandleTypeDef structure that contains
  *         the configuration information for the specified DAC. 
  * @param  Channel: The selected DAC channel. 
  *          This parameter can be one of the following values:
  *            DAC_CHANNEL_1 / DAC_CHANNEL_2
  * @param  Amplitude: Unmask DAC channel LFSR for noise wave generation. 
  *          This parameter can be one of the following values: 
  *            @arg DAC_LFSRUNMASK_BIT0: Unmask DAC channel LFSR bit0 for noise wave generation
  *            @arg DAC_LFSRUNMASK_BITS1_0: Unmask DAC channel LFSR bit[1:0] for noise wave generation  
  *            @arg DAC_LFSRUNMASK_BITS2_0: Unmask DAC channel LFSR bit[2:0] for noise wave generation
  *            @arg DAC_LFSRUNMASK_BITS3_0: Unmask DAC channel LFSR bit[3:0] for noise wave generation 
  *            @arg DAC_LFSRUNMASK_BITS4_0: Unmask DAC channel LFSR bit[4:0] for noise wave generation 
  *            @arg DAC_LFSRUNMASK_BITS5_0: Unmask DAC channel LFSR bit[5:0] for noise wave generation 
  *            @arg DAC_LFSRUNMASK_BITS6_0: Unmask DAC channel LFSR bit[6:0] for noise wave generation 
  *            @arg DAC_LFSRUNMASK_BITS7_0: Unmask DAC channel LFSR bit[7:0] for noise wave generation 
  *            @arg DAC_LFSRUNMASK_BITS8_0: Unmask DAC channel LFSR bit[8:0] for noise wave generation 
  *            @arg DAC_LFSRUNMASK_BITS9_0: Unmask DAC channel LFSR bit[9:0] for noise wave generation 
  *            @arg DAC_LFSRUNMASK_BITS10_0: Unmask DAC channel LFSR bit[10:0] for noise wave generation 
  *            @arg DAC_LFSRUNMASK_BITS11_0: Unmask DAC channel LFSR bit[11:0] for noise wave generation 
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DACEx_NoiseWaveGenerate(DAC_HandleTypeDef* hdac, uint32_t Channel, uint32_t Amplitude)
{   
  /* Check the parameters */
  assert_param(IS_DAC_CHANNEL(Channel));
  assert_param(IS_DAC_LFSR_UNMASK_TRIANGLE_AMPLITUDE(Amplitude));
  
  /* Process locked */
  __HAL_LOCK(hdac);
  
  /* Change DAC state */
  hdac->State = HAL_DAC_STATE_BUSY;
     
  /* Enable the noise wave generation for the selected DAC channel */
  MODIFY_REG(hdac->Instance->CR, ((DAC_CR_WAVE1)|(DAC_CR_MAMP1))<<Channel, (DAC_CR_WAVE1_0 | Amplitude) << Channel);
  
  /* Change DAC state */
  hdac->State = HAL_DAC_STATE_READY;
  
  /* Process unlocked */
  __HAL_UNLOCK(hdac);
  
  /* Return function status */
  return HAL_OK;
}



/**
  * @brief  Set the specified data holding register value for dual DAC channel.
  * @param  hdac: pointer to a DAC_HandleTypeDef structure that contains
  *               the configuration information for the specified DAC.
  * @param  Alignment: Specifies the data alignment for dual channel DAC.
  *          This parameter can be one of the following values:
  *            DAC_ALIGN_8B_R: 8bit right data alignment selected
  *            DAC_ALIGN_12B_L: 12bit left data alignment selected
  *            DAC_ALIGN_12B_R: 12bit right data alignment selected
  * @param  Data1: Data for DAC Channel2 to be loaded in the selected data holding register.
  * @param  Data2: Data for DAC Channel1 to be loaded in the selected data  holding register.
  * @note   In dual mode, a unique register access is required to write in both
  *          DAC channels at the same time.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DACEx_DualSetValue(DAC_HandleTypeDef* hdac, uint32_t Alignment, uint32_t Data1, uint32_t Data2)
{  
  uint32_t data = 0, tmp = 0;
  
  /* Check the parameters */
  assert_param(IS_DAC_ALIGN(Alignment));
  assert_param(IS_DAC_DATA(Data1));
  assert_param(IS_DAC_DATA(Data2));
  
  /* Calculate and set dual DAC data holding register value */
  if (Alignment == DAC_ALIGN_8B_R)
  {
    data = ((uint32_t)Data2 << 8) | Data1; 
  }
  else
  {
    data = ((uint32_t)Data2 << 16) | Data1;
  }
  
  tmp = (uint32_t)hdac->Instance;
  tmp += DAC_DHR12RD_ALIGNMENT(Alignment);

  /* Set the dual DAC selected data holding register */
  *(__IO uint32_t *)tmp = data;
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Conversion complete callback in non-blocking mode for Channel2.
  * @param  hdac: pointer to a DAC_HandleTypeDef structure that contains
  *         the configuration information for the specified DAC.
  * @retval None
  */
__weak void HAL_DACEx_ConvCpltCallbackCh2(DAC_HandleTypeDef* hdac)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hdac);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_DACEx_ConvCpltCallbackCh2 could be implemented in the user file
   */
}

/**
  * @brief  Conversion half DMA transfer callback in non-blocking mode for Channel2. 
  * @param  hdac: pointer to a DAC_HandleTypeDef structure that contains
  *         the configuration information for the specified DAC.
  * @retval None
  */
__weak void HAL_DACEx_ConvHalfCpltCallbackCh2(DAC_HandleTypeDef* hdac)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hdac);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_DACEx_ConvHalfCpltCallbackCh2 could be implemented in the user file
   */
}

/**
  * @brief  Error DAC callback for Channel2.
  * @param  hdac: pointer to a DAC_HandleTypeDef structure that contains
  *         the configuration information for the specified DAC.
  * @retval None
  */
__weak void HAL_DACEx_ErrorCallbackCh2(DAC_HandleTypeDef *hdac)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hdac);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_DACEx_ErrorCallbackCh2 could be implemented in the user file
   */
}

/**
  * @brief  DMA underrun DAC callback for Channel2.
  * @param  hdac: pointer to a DAC_HandleTypeDef structure that contains
  *         the configuration information for the specified DAC.
  * @retval None
  */
__weak void HAL_DACEx_DMAUnderrunCallbackCh2(DAC_HandleTypeDef *hdac)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hdac);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_DACEx_DMAUnderrunCallbackCh2 could be implemented in the user file
   */
}

/**
  * @brief  Run the self calibration of one DAC channel.
  * @param  hdac: pointer to a DAC_HandleTypeDef structure that contains
  *         the configuration information for the specified DAC.
  * @param  sConfig: DAC channel configuration structure.
  * @param  Channel: The selected DAC channel. 
  *          This parameter can be one of the following values:
  *            @arg DAC_CHANNEL_1: DAC Channel1 selected
  *            @arg DAC_CHANNEL_2: DAC Channel2 selected
  * @retval Updates DAC_TrimmingValue. , DAC_UserTrimming set to DAC_UserTrimming
  * @retval HAL status
  * @note   Calibration runs about 7 ms.
  */

HAL_StatusTypeDef HAL_DACEx_SelfCalibrate (DAC_HandleTypeDef* hdac, DAC_ChannelConfTypeDef* sConfig, uint32_t Channel)
{ 
  HAL_StatusTypeDef status = HAL_OK;
   
  __IO uint32_t tmp = 0;
  uint32_t trimmingvalue = 0;
  uint32_t delta;
  
  /* store/restore channel configuration structure purpose */
  uint32_t oldmodeconfiguration = 0;
  
  /* Check the parameters */
  assert_param(IS_DAC_CHANNEL(Channel));
    
 /* Check the DAC handle allocation */
 /* Check if DAC running */
  if((hdac == NULL) || (hdac->State == HAL_DAC_STATE_BUSY))
  {
    status = HAL_ERROR;
  }
  
  /* Process locked */
  __HAL_LOCK(hdac);
  
  /* Store configuration */
  oldmodeconfiguration = (hdac->Instance->MCR & (DAC_MCR_MODE1 << Channel));
  
  /* Disable the selected DAC channel */
  CLEAR_BIT ((hdac->Instance->CR), (DAC_CR_EN1 << Channel));
  
  /* Set mode in MCR  for calibration */
   MODIFY_REG(hdac->Instance->MCR, (DAC_MCR_MODE1 << Channel), 0);
   
  /* Set DAC Channel1 DHR register to the middle value */
  /* HAL_DAC_SetValue(hdac, Channel, DAC_ALIGN_12B_R, 0x0800); */
  tmp = (uint32_t)hdac->Instance; 
  if(Channel == DAC_CHANNEL_1)
  {
    tmp += DAC_DHR12R1_ALIGNMENT(DAC_ALIGN_12B_R);
  }
  else
  {
    tmp += DAC_DHR12R2_ALIGNMENT(DAC_ALIGN_12B_R);
  }
  *(__IO uint32_t *) tmp = 0x0800;
     
  /* Enable the selected DAC channel calibration */
  /* i.e. set DAC_CR_CENx bit */
  SET_BIT ((hdac->Instance->CR), (DAC_CR_CEN1 << Channel));
  
  /* Init trimming counter */    
  /* Medium value */
  trimmingvalue = 16; 
  delta = 8;
  while (delta != 0) 
    {
    /* Set candidate trimming */
    MODIFY_REG(hdac->Instance->CCR, (DAC_CCR_OTRIM1<<Channel), (trimmingvalue<<Channel));
  
    /* tOFFTRIMmax delay x ms as per datasheet (electrical characteristics */ 
    /* i.e. minimum time needed between two calibration steps */
    HAL_Delay(1);
  
    if ((hdac->Instance->SR & (DAC_SR_CAL_FLAG1<<Channel)) == RESET)
      { 
        /* DAC_SR_CAL_FLAGx is HIGH try higher trimming */
        trimmingvalue += delta;
      }
      else
      {
        /* DAC_SR_CAL_FLAGx is LOW try lower trimming */
        trimmingvalue -= delta;
       }                   
     delta >>= 1;
    }
  
    /* Still need to check if right calibration is current value or one step below */
    /* Indeed the first value that causes the DAC_SR_CAL_FLAGx bit to change from 0 to 1  */
    /* Set candidate trimming */
    MODIFY_REG(hdac->Instance->CCR, (DAC_CCR_OTRIM1<<Channel), (trimmingvalue<<Channel));
  
    /* tOFFTRIMmax delay x ms as per datasheet (electrical characteristics */ 
    /* i.e. minimum time needed between two calibration steps */
    HAL_Delay(1);
    
    if ((hdac->Instance->SR & (DAC_SR_CAL_FLAG1<<Channel)) == RESET)
      { 
      /* OPAMP_CSR_OUTCAL is actually one value more */
      trimmingvalue++;
      /* Set right trimming */
        MODIFY_REG(hdac->Instance->CCR, (DAC_CCR_OTRIM1<<Channel), (trimmingvalue<<Channel));
      }
    
  /* Disable the selected DAC channel calibration */
  /* i.e. clear DAC_CR_CENx bit */
  CLEAR_BIT ((hdac->Instance->CR), (DAC_CR_CEN1 << Channel));
  
  sConfig->DAC_TrimmingValue = trimmingvalue;
  sConfig->DAC_UserTrimming = DAC_TRIMMING_USER;
  
  /* Restore configuration */
  MODIFY_REG(hdac->Instance->MCR, (DAC_MCR_MODE1 << Channel), oldmodeconfiguration);
  
  /* Process unlocked */
  __HAL_UNLOCK(hdac);
  
  return status;
}

/**
  * @brief  Set the trimming mode and trimming value (user trimming mode applied).
  * @param  hdac: pointer to a DAC_HandleTypeDef structure that contains
  *         the configuration information for the specified DAC.
  * @param  sConfig: DAC configuration structure updated with new DAC trimming value.
  * @param  Channel: The selected DAC channel. 
  *          This parameter can be one of the following values:
  *            @arg DAC_CHANNEL_1: DAC Channel1 selected
  *            @arg DAC_CHANNEL_2: DAC Channel2 selected
  * @param  NewTrimmingValue: DAC new trimming value
  * @retval HAL status
  */

HAL_StatusTypeDef HAL_DACEx_SetUserTrimming (DAC_HandleTypeDef* hdac, DAC_ChannelConfTypeDef* sConfig, uint32_t Channel, uint32_t NewTrimmingValue)
{ 
  HAL_StatusTypeDef status = HAL_OK;
     
  /* Check the parameters */
  assert_param(IS_DAC_CHANNEL(Channel));
  assert_param(IS_DAC_NEWTRIMMINGVALUE(NewTrimmingValue));
  
 /* Check the DAC handle allocation */
  if(hdac == NULL)
  {
    status = HAL_ERROR;
  }
  
  /* Process locked */
  __HAL_LOCK(hdac);
  
  /* Set new trimming */
  MODIFY_REG(hdac->Instance->CCR, (DAC_CCR_OTRIM1<<Channel), (NewTrimmingValue<<Channel));
  
  /* Update trimming mode */
  sConfig->DAC_UserTrimming = DAC_TRIMMING_USER;
  sConfig->DAC_TrimmingValue = NewTrimmingValue;
  
  /* Process unlocked */
  __HAL_UNLOCK(hdac);
    
  return status;
}

/**
  * @brief  Return the DAC trimming value.
  * @param  hdac : DAC handle
  * @param  Channel: The selected DAC channel. 
  *          This parameter can be one of the following values:
  *            @arg DAC_CHANNEL_1: DAC Channel1 selected
  *            @arg DAC_CHANNEL_2: DAC Channel2 selected
  * @retval Trimming value : range: 0->31
  *         
 */

uint32_t HAL_DACEx_GetTrimOffset (DAC_HandleTypeDef *hdac, uint32_t Channel)
{
  uint32_t trimmingvalue = 0;
  
  /* Check the DAC handle allocation */
  /* And not in Reset state */
  if((hdac == NULL) || (hdac->State == HAL_DAC_STATE_RESET))
  {
    return HAL_ERROR;
  }
  else
  {
    /* Check the parameter */
    assert_param(IS_DAC_CHANNEL(Channel));

    /* Retrieve trimming  */
    trimmingvalue = ((hdac->Instance->CCR & (DAC_CCR_OTRIM1 << Channel)) >> Channel);
  }  
  return trimmingvalue;
}

/**
  * @}
  */

/** @defgroup DACEx_Exported_Functions_Group3 Peripheral Control functions
 *  @brief    Extended Peripheral Control functions 
 *
@verbatim   
  ==============================================================================
             ##### Peripheral Control functions #####
  ==============================================================================  
    [..]  This section provides functions allowing to:
      (+) Configure channels. 
      (+) Set the specified data holding register value for DAC channel.
      
@endverbatim
  * @{
  */

/**
  * @brief  Return the last data output value of the selected DAC channel.
  * @param  hdac: pointer to a DAC_HandleTypeDef structure that contains
  *         the configuration information for the specified DAC.
  * @retval The selected DAC channel data output value.
  */
uint32_t HAL_DACEx_DualGetValue(DAC_HandleTypeDef* hdac)
{
  uint32_t tmp = 0;
  
  tmp |= hdac->Instance->DOR1;

  tmp |= hdac->Instance->DOR2 << 16;
  
  /* Returns the DAC channel data output register value */
  return tmp;
}

/**
  * @}
  */

/**
  * @}
  */

/* Private functions ---------------------------------------------------------*/
/** @defgroup DACEx_Private_Functions DACEx private functions
 *  @brief    Extended private functions
   * @{
  */

/**
  * @brief  DMA conversion complete callback. 
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *                the configuration information for the specified DMA module.
  * @retval None
  */
void DAC_DMAConvCpltCh2(DMA_HandleTypeDef *hdma)   
{
  DAC_HandleTypeDef* hdac = ( DAC_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;
  
  HAL_DACEx_ConvCpltCallbackCh2(hdac); 
  
  hdac->State= HAL_DAC_STATE_READY;
}

/**
  * @brief  DMA half transfer complete callback. 
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *                the configuration information for the specified DMA module.
  * @retval None
  */
void DAC_DMAHalfConvCpltCh2(DMA_HandleTypeDef *hdma)   
{
    DAC_HandleTypeDef* hdac = ( DAC_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;
    /* Conversion complete callback */
    HAL_DACEx_ConvHalfCpltCallbackCh2(hdac); 
}

/**
  * @brief  DMA error callback.
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *                the configuration information for the specified DMA module.
  * @retval None
  */
void DAC_DMAErrorCh2(DMA_HandleTypeDef *hdma)   
{
  DAC_HandleTypeDef* hdac = ( DAC_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;
    
  /* Set DAC error code to DMA error */
  hdac->ErrorCode |= HAL_DAC_ERROR_DMA;
    
  HAL_DACEx_ErrorCallbackCh2(hdac); 
    
  hdac->State= HAL_DAC_STATE_READY;
}

/**
  * @}
  */

#endif /* HAL_DAC_MODULE_ENABLED */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
