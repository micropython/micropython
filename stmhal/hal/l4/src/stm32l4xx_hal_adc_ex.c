/**
  ******************************************************************************
  * @file    stm32l4xx_hal_adc_ex.c
  * @author  MCD Application Team
  * @version V1.3.0
  * @date    29-January-2016
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the Analog to Digital Convertor (ADC)
  *          peripheral:
  *           + Calibration functions
  *             ++ Calibration start-up
  *             ++ Calibration value reading or setting 
  *           + Operation functions
  *             ++ Start, stop, get result of conversions of injected
  *             groups, using 3 possible modes: polling or interruption.
  *             ++ Multimode feature when available
  *           + Control functions
  *             ++ Configure channels on injected group
  *           + State functions
  *             ++ Injected group queues management
  *  
   @verbatim           
  ==============================================================================
                    ##### ADC specific features #####
  ==============================================================================
  [..] 
  (#) Interrupt generation at the end of injected conversion and in case of 
      injected queues overflow.
  
  (#) External trigger (timer or EXTI) with configurable polarity for  
      injected groups.

  (#) Multimode Dual mode when multimode feature is available.
  
  (#) Configurable DMA data storage in Multimode Dual mode.
  
  (#) Configurable delay between conversions in Dual interleaved mode.
  
  (#) ADC calibration.

  (#) ADC channels selectable single/differential input.

  (#) ADC Injected sequencer&channels configuration context queue.

  (#) ADC offset on injected groups.
  
  (#) ADC oversampling.


                     ##### How to use this driver #####
  ==============================================================================
    [..]
  
     (#) Configure the ADC parameters (conversion resolution, data alignment,  
         continuous mode, ...) using the HAL_ADC_Init() function.

     (#) Activate the ADC peripheral using one of the start functions: 
         HAL_ADCEx_InjectedStart(), HAL_ADCEx_InjectedStart_IT() for injected conversions
         or 
         HAL_ADC_MultiModeStart_DMA() for multimode conversions when multimode
         feature is available.
  
  
     *** Channels to injected group configuration ***
     =============================================    
     [..]
       (+) To configure the ADC Injected channels group features, use 
           HAL_ADCEx_InjectedConfigChannel() functions.
       (+) To read the ADC converted values, use the HAL_ADCEx_InjectedGetValue() 
           function.  
            
            
     *** Multimode ADCs configuration (when  multimode feature is available) ***
     ========================================================================
     [..]
       (+) Multimode feature is available and applicable to Master and
           Slave ADCs.
       (+) Refer to "Channels to regular group configuration" description to  
           configure the Master and Slave regular groups.        
       (+) Select the Multi mode ADC  features (dual mode
           simultaneous, interleaved, ...) and configure the DMA mode using 
           HAL_ADCEx_MultiModeConfigChannel() functions. 
       (+) Read the ADCs converted values using the HAL_ADCEx_MultiModeGetValue()
           function.

  
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

/** @defgroup ADCEx ADCEx
  * @brief ADC Extended HAL module driver
  * @{
  */

#ifdef HAL_ADC_MODULE_ENABLED
    
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/** @defgroup ADCEx_Private_Constants ADC Extended Private Constants
  * @{
  */

#define ADC_JSQR_FIELDS  ((uint32_t)(ADC_JSQR_JL | ADC_JSQR_JEXTSEL | ADC_JSQR_JEXTEN |\
                                     ADC_JSQR_JSQ1  | ADC_JSQR_JSQ2 |\
                                      ADC_JSQR_JSQ3 | ADC_JSQR_JSQ4 ))  /*!< ADC_JSQR fields of parameters that can be updated anytime
                                                                             once the ADC is enabled */
                                      
#define ADC_CFGR2_INJ_FIELDS  ((uint32_t)(ADC_CFGR2_JOVSE | ADC_CFGR2_OVSR  |\
                                       ADC_CFGR2_OVSS ))     /*!< ADC_CFGR2 injected oversampling parameters that can be updated
                                                                  when no conversion is on-going (neither regular nor injected) */
                                       
/* Fixed timeout value for ADC calibration.                                   */
/* Values defined to be higher than worst cases: low clock frequency,         */
/* maximum prescalers.                                                        */
/* Ex of profile low frequency : f_ADC at 0.14 MHz (minimum value             */
/* according to Data sheet), calibration_time MAX = 112 / f_ADC               */
/*           112 / 140,000 = 0.8 ms                                           */
/* At maximum CPU speed (80 MHz), this means                                  */
/*    0.8 ms * 80 MHz = 64000 CPU cycles                                      */  
#define ADC_CALIBRATION_TIMEOUT         ((uint32_t) 64000)    /*!< ADC calibration time-out value */ 
                                    
/**
  * @}
  */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/** @defgroup ADCEx_Exported_Functions ADC Extended Exported Functions
  * @{
  */ 



/** @defgroup ADCEx_Exported_Functions_Group1 Extended Input and Output operation functions
  * @brief    Extended IO operation functions
  *
@verbatim   
 ===============================================================================
             ##### IO operation functions #####
 ===============================================================================  
    [..]  This section provides functions allowing to:
    
      (+) Perform the ADC self-calibration for single or differential ending.
      (+) Get calibration factors for single or differential ending.
      (+) Set calibration factors for single or differential ending.
          
      (+) Start conversion of injected group.
      (+) Stop conversion of injected group.
      (+) Poll for conversion complete on injected group.
      (+) Get result of injected channel conversion.
      (+) Start conversion of injected group and enable interruptions.
      (+) Stop conversion of injected group and disable interruptions.

      (+) When multimode feature is available, start multimode and enable DMA transfer.
      (+) Stop multimode and disable ADC DMA transfer.
      (+) Get result of multimode conversion.



@endverbatim
  * @{
  */
  


/**
  * @brief  Perform an ADC automatic self-calibration
  *         Calibration prerequisite: ADC must be disabled (execute this
  *         function before HAL_ADC_Start() or after HAL_ADC_Stop() ).
  * @param  hadc: ADC handle.
  * @param  SingleDiff: Selection of single-ended or differential input
  *          This parameter can be one of the following values:
  *            @arg @ref ADC_SINGLE_ENDED       Channel in mode input single ended
  *            @arg @ref ADC_DIFFERENTIAL_ENDED Channel in mode input differential ended
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ADCEx_Calibration_Start(ADC_HandleTypeDef* hadc, uint32_t SingleDiff)
{
  HAL_StatusTypeDef tmp_status = HAL_OK;
  uint32_t WaitLoopIndex = 0;
  
  /* Check the parameters */
  assert_param(IS_ADC_ALL_INSTANCE(hadc->Instance));
  assert_param(IS_ADC_SINGLE_DIFFERENTIAL(SingleDiff));

  /* Process locked */
  __HAL_LOCK(hadc);
   
  /* Calibration prerequisite: ADC must be disabled. */
   
  /* Disable the ADC (if not already disabled) */
  tmp_status = ADC_Disable(hadc);
  
  /* Check if ADC is effectively disabled */
  if (tmp_status == HAL_OK)
  {
    /* Change ADC state */
    /* Clear HAL_ADC_STATE_REG_BUSY and HAL_ADC_STATE_INJ_BUSY bits, set HAL_ADC_STATE_BUSY_INTERNAL bit */
    ADC_STATE_CLR_SET(hadc->State, (HAL_ADC_STATE_REG_BUSY|HAL_ADC_STATE_INJ_BUSY), HAL_ADC_STATE_BUSY_INTERNAL);
    
    /* Select calibration mode single ended or differential ended */
    MODIFY_REG(hadc->Instance->CR, ADC_CR_ADCALDIF, SingleDiff);

    /* Start ADC calibration */
    SET_BIT(hadc->Instance->CR, ADC_CR_ADCAL);


    /* Wait for calibration completion */
    while(HAL_IS_BIT_SET(hadc->Instance->CR, ADC_CR_ADCAL))
    {
      WaitLoopIndex++;
      if (WaitLoopIndex >= ADC_CALIBRATION_TIMEOUT)
      {
        /* Update ADC state machine to error */
        /* Clear HAL_ADC_STATE_BUSY_INTERNAL bit, set HAL_ADC_STATE_ERROR_INTERNAL bit */
        ADC_STATE_CLR_SET(hadc->State, HAL_ADC_STATE_BUSY_INTERNAL, HAL_ADC_STATE_ERROR_INTERNAL);
        
        /* Process unlocked */
        __HAL_UNLOCK(hadc);
        
        return HAL_ERROR;
      }
    }
    
    /* Clear HAL_ADC_STATE_BUSY_INTERNAL bit, set HAL_ADC_STATE_READY bit */
    ADC_STATE_CLR_SET(hadc->State, HAL_ADC_STATE_BUSY_INTERNAL, HAL_ADC_STATE_READY);
  }
  else
  {
    SET_BIT(hadc->State, HAL_ADC_STATE_ERROR_INTERNAL);
    
    /* Update ADC state machine to error */
    tmp_status = HAL_ERROR; 
  }
  
  
  /* Process unlocked */
  __HAL_UNLOCK(hadc);
  
  /* Return function status */
  return tmp_status;
}




/**
  * @brief  Get the calibration factor from automatic conversion result.
  * @param  hadc: ADC handle.
  * @param  SingleDiff: Selection of single-ended or differential input
  *          This parameter can be one of the following values:
  *            @arg @ref ADC_SINGLE_ENDED       Channel in mode input single ended
  *            @arg @ref ADC_DIFFERENTIAL_ENDED Channel in mode input differential ended
  * @retval Converted value
  */
uint32_t HAL_ADCEx_Calibration_GetValue(ADC_HandleTypeDef* hadc, uint32_t SingleDiff)
{
  /* Check the parameters */
  assert_param(IS_ADC_ALL_INSTANCE(hadc->Instance));
  assert_param(IS_ADC_SINGLE_DIFFERENTIAL(SingleDiff)); 
  
  /* Return the selected ADC calibration value */ 
  if (SingleDiff == ADC_DIFFERENTIAL_ENDED)
  {
    return ADC_CALFACT_DIFF_GET(hadc->Instance->CALFACT);
  }
  else
  {
    return ((hadc->Instance->CALFACT) & ADC_CALFACT_CALFACT_S);
  }
}



/**
  * @brief  Set the calibration factor to overwrite automatic conversion result. ADC must be enabled and no conversion on going.
  * @param  hadc: ADC handle.
  * @param  SingleDiff: Selection of single-ended or differential input.
  *          This parameter can be one of the following values:
  *            @arg @ref ADC_SINGLE_ENDED       Channel in mode input single ended
  *            @arg @ref ADC_DIFFERENTIAL_ENDED Channel in mode input differential ended
  * @param  CalibrationFactor: Calibration factor (coded on 7 bits maximum)
  * @retval HAL state
  */
HAL_StatusTypeDef HAL_ADCEx_Calibration_SetValue(ADC_HandleTypeDef* hadc, uint32_t SingleDiff, uint32_t CalibrationFactor)
{
  HAL_StatusTypeDef tmp_status = HAL_OK;
  
  /* Check the parameters */
  assert_param(IS_ADC_ALL_INSTANCE(hadc->Instance));
  assert_param(IS_ADC_SINGLE_DIFFERENTIAL(SingleDiff)); 
  assert_param(IS_ADC_CALFACT(CalibrationFactor)); 
  
  /* Process locked */
  __HAL_LOCK(hadc);
  
  /* Verification of hardware constraints before modifying the calibration    */
  /* factors register: ADC must be enabled, no conversion on going.           */
  if ( (ADC_IS_ENABLE(hadc) != RESET)                            &&
       (ADC_IS_CONVERSION_ONGOING_REGULAR_INJECTED(hadc) == RESET)  )
  {
    /* Set the selected ADC calibration value */ 
    if (SingleDiff == ADC_DIFFERENTIAL_ENDED)
    {
      MODIFY_REG(hadc->Instance->CALFACT, ADC_CALFACT_CALFACT_D, ADC_CALFACT_DIFF_SET(CalibrationFactor));
    }
    else
    {
      MODIFY_REG(hadc->Instance->CALFACT, ADC_CALFACT_CALFACT_S, CalibrationFactor);
    }
  }
  else
  {
    /* Update ADC state machine */
    SET_BIT(hadc->State, HAL_ADC_STATE_ERROR_CONFIG);
    
    /* Update ADC state machine to error */
    tmp_status = HAL_ERROR;
  }
  
  /* Process unlocked */
  __HAL_UNLOCK(hadc);
  
  /* Return function status */
  return tmp_status;
}



/**
  * @brief  Enable ADC, start conversion of injected group.
  * @note   Interruptions enabled in this function: None.
  * @note   Case of multimode enabled when multimode feature is available: 
  *         HAL_ADCEx_InjectedStart() API must be called for ADC slave first, 
  *         then for ADC master. 
  *         For ADC slave, ADC is enabled only (conversion is not started).  
  *         For ADC master, ADC is enabled and multimode conversion is started.
  * @param  hadc: ADC handle.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ADCEx_InjectedStart(ADC_HandleTypeDef* hadc)
{
  HAL_StatusTypeDef tmp_status = HAL_OK;  
  
  /* Check the parameters */
  assert_param(IS_ADC_ALL_INSTANCE(hadc->Instance));
  
  if (ADC_IS_CONVERSION_ONGOING_INJECTED(hadc))
  {
    return HAL_BUSY;
  }
  else
  {
  
    /* In case of software trigger detection enabled, JQDIS must be set 
      (which can be done only if ADSTART and JADSTART are both cleared).
       If JQDIS is not set at that point, returns an error
       - since software trigger detection is disabled. User needs to
       resort to HAL_ADCEx_DisableInjectedQueue() API to set JQDIS.  
       - or (if JQDIS is intentionally reset) since JEXTEN = 0 which means 
         the queue is empty */
    if ((READ_BIT(hadc->Instance->JSQR, ADC_JSQR_JEXTEN) == RESET)   
    && (READ_BIT(hadc->Instance->CFGR, ADC_CFGR_JQDIS) == RESET))  
    {
      SET_BIT(hadc->State, HAL_ADC_STATE_ERROR_CONFIG); 
      return HAL_ERROR;
    }

      
    /* Process locked */
    __HAL_LOCK(hadc);
      
    /* Enable the ADC peripheral */
    tmp_status = ADC_Enable(hadc);
    
    /* Start conversion if ADC is effectively enabled */
    if (tmp_status == HAL_OK)
    {
      /* Check if a regular conversion is ongoing */
      if (HAL_IS_BIT_SET(hadc->State, HAL_ADC_STATE_REG_BUSY)) 
      {
        /* Reset ADC error code field related to injected conversions only */
        CLEAR_BIT(hadc->ErrorCode, HAL_ADC_ERROR_JQOVF);             
      }
      else
      {
        /* Set ADC error code to none */
        ADC_CLEAR_ERRORCODE(hadc);        
      }
      /* Update ADC state */
      /* Clear HAL_ADC_STATE_READY and HAL_ADC_STATE_INJ_EOC bits, set HAL_ADC_STATE_INJ_BUSY bit */
      ADC_STATE_CLR_SET(hadc->State, (HAL_ADC_STATE_READY|HAL_ADC_STATE_INJ_EOC), HAL_ADC_STATE_INJ_BUSY);
      
      /* Reset HAL_ADC_STATE_MULTIMODE_SLAVE bit
        - by default if ADC is Master or Independent or if multimode feature is not available
        - if multimode setting is set to independent mode (no dual regular or injected conversions are configured) */
      if (ADC_NONMULTIMODE_OR_MULTIMODEMASTER(hadc))
      {
        CLEAR_BIT(hadc->State, HAL_ADC_STATE_MULTIMODE_SLAVE);
      }      

      
      /* Clear injected group conversion flag */
      /* (To ensure of no unknown state from potential previous ADC operations) */
      __HAL_ADC_CLEAR_FLAG(hadc, (ADC_FLAG_JEOC | ADC_FLAG_JEOS));
      
      /* Enable conversion of injected group, if automatic injected conversion  */
      /* is disabled.                                                           */
      /* If software start has been selected, conversion starts immediately.    */
      /* If external trigger has been selected, conversion will start at next   */
      /* trigger event.                                                         */
      /* Case of multimode enabled (when multimode feature is available):       */
      /* if ADC is slave,                                                       */
      /*    - ADC is enabled only (conversion is not started).                  */
      /*    - if multimode only concerns regular conversion, ADC is enabled     */
      /*     and conversion is started.                                         */
      /* If ADC is master or independent,                                       */
      /*    - ADC is enabled and conversion is started.                         */

      /* Are injected conversions that of a dual Slave ? */      
      if (ADC_INDEPENDENT_OR_NONMULTIMODEINJECTED_SLAVE(hadc))
      {
        /* hadc is not the handle of a Slave ADC with dual injected conversions enabled:
           set ADSTART only if JAUTO is cleared */
        /* Process unlocked */
        __HAL_UNLOCK(hadc);            
        if (HAL_IS_BIT_CLR(hadc->Instance->CFGR, ADC_CFGR_JAUTO))
        {
          SET_BIT(hadc->Instance->CR, ADC_CR_JADSTART) ;
        }
      }
      else
      {
        /* hadc is the handle of a Slave ADC with dual injected conversions enabled:
           ADSTART is not set */
          SET_BIT(hadc->State, HAL_ADC_STATE_MULTIMODE_SLAVE);
        /* Process unlocked */
        __HAL_UNLOCK(hadc);            
      }
    } 
    else
    {
      /* Process unlocked */
      __HAL_UNLOCK(hadc);    
    } /* if (tmp_status == HAL_OK) */
  
  
    /* Return function status */
    return tmp_status;
  } /*  if (ADC_IS_CONVERSION_ONGOING_INJECTED(hadc)) */
}



/**
  * @brief  Stop conversion of injected channels, disable ADC peripheral if no regular conversion is on going.
  * @note   If ADC must be disabled and if regular conversion
  *         is on going, function HAL_ADC_Stop() must be used.
  * @note   In case of auto-injection mode, HAL_ADC_Stop() must be used.
  * @note   In case of multimode enabled (when multimode feature is available), 
  *         HAL_ADCEx_InjectedStop() must be called for ADC master first, then for ADC slave.
  *         For ADC master, conversion is stopped and ADC is disabled. 
  *         For ADC slave, ADC is disabled only (conversion stop of ADC master
  *         has already stopped conversion of ADC slave).
  * @param  hadc: ADC handle.
  * @retval None
  */
HAL_StatusTypeDef HAL_ADCEx_InjectedStop(ADC_HandleTypeDef* hadc)
{  
  HAL_StatusTypeDef tmp_status = HAL_OK;
  
  /* Check the parameters */
  assert_param(IS_ADC_ALL_INSTANCE(hadc->Instance));
  
  /* Process locked */
  __HAL_LOCK(hadc);
  
  /* 1. Stop potential conversion on going on injected group only. */
  tmp_status = ADC_ConversionStop(hadc, ADC_INJECTED_GROUP);
  
  /* Disable ADC peripheral if injected conversions are effectively stopped   */
  /* and if no conversion on regular group is on-going                       */
  if (tmp_status == HAL_OK)
  {
    if (ADC_IS_CONVERSION_ONGOING_REGULAR(hadc) == RESET)
    {
      /* 2. Disable the ADC peripheral */
      tmp_status = ADC_Disable(hadc);
      
      /* Check if ADC is effectively disabled */
      if (tmp_status == HAL_OK)
      {
        /* Change ADC state */
      /* Clear HAL_ADC_STATE_REG_BUSY and HAL_ADC_STATE_INJ_BUSY bits, set HAL_ADC_STATE_READY bit */
      ADC_STATE_CLR_SET(hadc->State, (HAL_ADC_STATE_REG_BUSY|HAL_ADC_STATE_INJ_BUSY), HAL_ADC_STATE_READY);
      }
    }
    /* Conversion on injected group is stopped, but ADC not disabled since    */
    /* conversion on regular group is still running.                          */
    else
    {
      /* Clear HAL_ADC_STATE_INJ_BUSY bit */
      CLEAR_BIT(hadc->State, HAL_ADC_STATE_INJ_BUSY); 
    }
  }
  
  /* Process unlocked */
  __HAL_UNLOCK(hadc);
  
  /* Return function status */
  return tmp_status;
}



/**
  * @brief  Wait for injected group conversion to be completed.
  * @param  hadc: ADC handle
  * @param  Timeout: Timeout value in millisecond.
  * @note   Depending on hadc->Init.EOCSelection, JEOS or JEOC is 
  *         checked and cleared depending on AUTDLY bit status.    
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ADCEx_InjectedPollForConversion(ADC_HandleTypeDef* hadc, uint32_t Timeout)
{
  uint32_t           tickstart;
  uint32_t           tmp_Flag_End        = 0x00;
  ADC_TypeDef        *tmpADC_Master;
  uint32_t           tmp_cfgr            = 0x00;
  uint32_t           tmp_cfgr_jqm_autdly = 0x00;   
  uint32_t           tmp_jeos_raised     = 0x01; /* by default, assume that JEOS is set, 
                                                    tmp_jeos_raised will be corrected
                                                    accordingly during API execution */       
 
  /* Check the parameters */
  assert_param(IS_ADC_ALL_INSTANCE(hadc->Instance));

  /* If end of sequence selected */
  if (hadc->Init.EOCSelection == ADC_EOC_SEQ_CONV)
  {
    tmp_Flag_End = ADC_FLAG_JEOS;
  }
  else /* end of conversion selected */
  {
    tmp_Flag_End = ADC_FLAG_JEOC;
  }
  
  /* Get timeout */
  tickstart = HAL_GetTick();  
     
   /* Wait until End of Conversion or Sequence flag is raised */
  while(HAL_IS_BIT_CLR(hadc->Instance->ISR, tmp_Flag_End))
  {
    /* Check if timeout is disabled (set to infinite wait) */
    if(Timeout != HAL_MAX_DELAY)
    {
      if((Timeout == 0) || ((HAL_GetTick()-tickstart) > Timeout))
      {
        /* Update ADC state machine to timeout */
        SET_BIT(hadc->State, HAL_ADC_STATE_TIMEOUT);
        
        /* Process unlocked */
        __HAL_UNLOCK(hadc);
        
        return HAL_TIMEOUT;
      }
    }
  }
  
  /* Next, to clear the polled flag as well as to update the handle State,
     JEOS is checked and the relevant configuration registers are retrieved. 
     JQM, JAUTO and CONT bits will have to be read for the State update,
     AUTDLY for JEOS clearing. */
  /*   1. Check whether or not JEOS is set */
  if (HAL_IS_BIT_CLR(hadc->Instance->ISR, ADC_FLAG_JEOS))
  {
    tmp_jeos_raised = 0;   
  }
  /*  2. Check whether or not hadc is the handle of a Slave ADC with dual
        injected conversions enabled. */
  if (ADC_INDEPENDENT_OR_NONMULTIMODEINJECTED_SLAVE(hadc) == RESET)  
  {
    /* hadc is not the handle of a Slave ADC with dual injected conversions enabled:
        check JQM and AUTDLY bits directly in ADC CFGR register */     
    tmp_cfgr_jqm_autdly = READ_REG(hadc->Instance->CFGR);  
  }
  else
  {
    /* hadc is the handle of a Slave ADC with dual injected conversions enabled:
        need to check JQM and AUTDLY bits of Master ADC CFGR register */    
    tmpADC_Master = ADC_MASTER_REGISTER(hadc);
    tmp_cfgr_jqm_autdly = READ_REG(tmpADC_Master->CFGR);
  }  
  /* 3. Check whether or not hadc is the handle of a Slave ADC with dual
        regular conversions enabled. */ 
  if (ADC_INDEPENDENT_OR_NONMULTIMODEREGULAR_SLAVE(hadc))
  {
    /* hadc is not the handle of a Slave ADC with dual regular conversions enabled:
       check JAUTO and CONT bits directly in ADC CFGR register */        
    tmp_cfgr = READ_REG(hadc->Instance->CFGR); 
  }
  else
  {
    /* hadc is not the handle of a Slave ADC with dual regular conversions enabled:
      check JAUTO and CONT bits of Master ADC CFGR register */  
    tmpADC_Master = ADC_MASTER_REGISTER(hadc);
    tmp_cfgr = READ_REG(tmpADC_Master->CFGR); 
  }        
  
  
  
  /* Clear polled flag */                         
  if (tmp_Flag_End == ADC_FLAG_JEOS)
  {
    /* Clear end of sequence JEOS flag of injected group if low power feature */
    /* "LowPowerAutoWait " is disabled, to not interfere with this feature.   */
    /* For injected groups, no new conversion will start before JEOS is       */
    /* cleared.                                                               */
    /* Note that 1. reading ADCx_JDRy clears JEOC.                            */ 
    /*           2. in multimode with dual injected conversions enabled (when */
    /*              multimode feature is available), Master AUTDLY bit is     */
    /*              checked.                                                  */        
    if (READ_BIT (tmp_cfgr_jqm_autdly, ADC_CFGR_AUTDLY) == RESET)
    {
      __HAL_ADC_CLEAR_FLAG(hadc, ADC_FLAG_EOC);
    }
  }
  else
  {
    __HAL_ADC_CLEAR_FLAG(hadc, ADC_FLAG_JEOC);    
  }  
  
  
  /* Update ADC state machine */
  SET_BIT(hadc->State, HAL_ADC_STATE_INJ_EOC); 
  /* Are injected conversions over ? This is the case if JEOS is set AND
        - injected conversions are software-triggered when injected queue management is disabled
        OR
        - auto-injection is enabled, continuous mode is disabled,
          and regular conversions are software-triggered */
          
  if (tmp_jeos_raised)
  {  
    if ((ADC_IS_SOFTWARE_START_INJECTED(hadc) && (READ_BIT(tmp_cfgr_jqm_autdly, ADC_CFGR_JQM) != ADC_CFGR_JQM))  
       && (!((READ_BIT(tmp_cfgr, (ADC_CFGR_JAUTO|ADC_CFGR_CONT)) == (ADC_CFGR_JAUTO|ADC_CFGR_CONT)) &&    
           (ADC_IS_SOFTWARE_START_REGULAR(hadc)))    ))           
    {
      /* Clear HAL_ADC_STATE_INJ_BUSY bit */
      CLEAR_BIT(hadc->State, HAL_ADC_STATE_INJ_BUSY); 
      /* If no regular conversion on-going, set HAL_ADC_STATE_READY bit */                   
      if (HAL_IS_BIT_CLR(hadc->State, HAL_ADC_STATE_REG_BUSY))
      { 
        SET_BIT(hadc->State, HAL_ADC_STATE_READY); 
      }           
    }      
  }


    
  /* Return API HAL status */
  return HAL_OK;
}



/**
  * @brief  Enable ADC, start conversion of injected group with interruption.
  * @note   Interruptions enabled in this function according to initialization
  *         setting : JEOC (end of conversion) or JEOS (end of sequence) 
  * @note   Case of multimode enabled (when multimode feature is enabled): 
  *         HAL_ADCEx_InjectedStart_IT() API must be called for ADC slave first, 
  *         then for ADC master. 
  *         For ADC slave, ADC is enabled only (conversion is not started).  
  *         For ADC master, ADC is enabled and multimode conversion is started.
  * @param  hadc: ADC handle.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_ADCEx_InjectedStart_IT(ADC_HandleTypeDef* hadc)
{
  HAL_StatusTypeDef tmp_status = HAL_OK;  
  
  /* Check the parameters */
  assert_param(IS_ADC_ALL_INSTANCE(hadc->Instance));
  
  if (ADC_IS_CONVERSION_ONGOING_INJECTED(hadc))
  {
    return HAL_BUSY;
  }
  else
  {

    /* In case of software trigger detection enabled, JQDIS must be set 
      (which can be done only if ADSTART and JADSTART are both cleared).
       If JQDIS is not set at that point, returns an error
       - since software trigger detection is disabled. User needs to
       resort to HAL_ADCEx_DisableInjectedQueue() API to set JQDIS.  
       - or (if JQDIS is intentionally reset) since JEXTEN = 0 which means 
         the queue is empty */
    if ((READ_BIT(hadc->Instance->JSQR, ADC_JSQR_JEXTEN) == RESET)   
    && (READ_BIT(hadc->Instance->CFGR, ADC_CFGR_JQDIS) == RESET))  
    {
      SET_BIT(hadc->State, HAL_ADC_STATE_ERROR_CONFIG); 
      return HAL_ERROR;
    }

    /* Process locked */
    __HAL_LOCK(hadc);
      
    /* Enable the ADC peripheral */
    tmp_status = ADC_Enable(hadc);
    
    /* Start conversion if ADC is effectively enabled */
    if (tmp_status == HAL_OK)
    {
      /* Check if a regular conversion is ongoing */
      if (HAL_IS_BIT_SET(hadc->State, HAL_ADC_STATE_REG_BUSY))
      {
        /* Reset ADC error code field related to injected conversions only */
        CLEAR_BIT(hadc->ErrorCode, HAL_ADC_ERROR_JQOVF);            
      }
      else
      {
        /* Set ADC error code to none */
        ADC_CLEAR_ERRORCODE(hadc);
      }
      /* Clear HAL_ADC_STATE_READY and HAL_ADC_STATE_INJ_EOC bits, set HAL_ADC_STATE_INJ_BUSY bit */
      ADC_STATE_CLR_SET(hadc->State, (HAL_ADC_STATE_READY|HAL_ADC_STATE_INJ_EOC), HAL_ADC_STATE_INJ_BUSY);
      
      /* Reset HAL_ADC_STATE_MULTIMODE_SLAVE bit
        - by default if ADC is Master or Independent or if multimode feature is not available
        - if multimode setting is set to independent mode (no dual regular or injected conversions are configured) */
      if (ADC_NONMULTIMODE_OR_MULTIMODEMASTER(hadc))
      {
        CLEAR_BIT(hadc->State, HAL_ADC_STATE_MULTIMODE_SLAVE);
      }      
    
      /* Clear injected group conversion flag */
      /* (To ensure of no unknown state from potential previous ADC operations) */
      __HAL_ADC_CLEAR_FLAG(hadc, (ADC_FLAG_JEOC | ADC_FLAG_JEOS));
      
      /* Enable ADC Injected context queue overflow interrupt if this feature   */
      /* is enabled.                                                            */
      if ((hadc->Instance->CFGR & ADC_CFGR_JQM) != RESET)
      {
        __HAL_ADC_ENABLE_IT(hadc, ADC_FLAG_JQOVF);
      }
      
      /* Enable ADC end of conversion interrupt */
      switch(hadc->Init.EOCSelection)
      {
        case ADC_EOC_SEQ_CONV: 
          __HAL_ADC_DISABLE_IT(hadc, ADC_IT_JEOC);
          __HAL_ADC_ENABLE_IT(hadc, ADC_IT_JEOS);
          break;
        /* case ADC_EOC_SINGLE_CONV */
        default:
          __HAL_ADC_DISABLE_IT(hadc, ADC_IT_JEOS);      
          __HAL_ADC_ENABLE_IT(hadc, ADC_IT_JEOC);
          break;
      }
      
      /* Enable conversion of injected group, if automatic injected conversion  */
      /* is disabled.                                                           */
      /* If software start has been selected, conversion starts immediately.    */
      /* If external trigger has been selected, conversion will start at next   */
      /* trigger event.                                                         */
      /* Case of multimode enabled (when multimode feature is available):       */
      /* if ADC is slave,                                                       */
      /*    - ADC is enabled only (conversion is not started),                  */
      /*    - if multimode only concerns regular conversion, ADC is enabled     */
      /*     and conversion is started.                                         */
      /* If ADC is master or independent,                                       */
      /*    - ADC is enabled and conversion is started.                         */

      /* Are injected conversions that of a dual Slave ? */      
      if (ADC_INDEPENDENT_OR_NONMULTIMODEINJECTED_SLAVE(hadc))
      {
        /* hadc is not the handle of a Slave ADC with dual injected conversions enabled:
           set ADSTART only if JAUTO is cleared */
        /* Process unlocked */
        __HAL_UNLOCK(hadc);            
        if (HAL_IS_BIT_CLR(hadc->Instance->CFGR, ADC_CFGR_JAUTO))
        {
          SET_BIT(hadc->Instance->CR, ADC_CR_JADSTART) ;
        }
      }
      else
      {
        /* hadc is the handle of a Slave ADC with dual injected conversions enabled:
           ADSTART is not set */
          SET_BIT(hadc->State, HAL_ADC_STATE_MULTIMODE_SLAVE);
        /* Process unlocked */
        __HAL_UNLOCK(hadc);          
      }
    }
    else
    {
      /* Process unlocked */
      __HAL_UNLOCK(hadc);
    }
    
    /* Return function status */
    return tmp_status;
}
}



/**
  * @brief  Stop conversion of injected channels, disable interruption of end-of-conversion. 
  * @note   Disable ADC peripheral if no regular conversion
  *         is on going.
  * @note   If ADC must be disabled and if regular conversion
  *         is on going, function HAL_ADC_Stop must be used first.
  * @note   Case of multimode enabled (when multimode feature is available): 
  *         HAL_ADCEx_InjectedStop_IT() API must be called for ADC master first, 
  *         then for ADC slave.
  *         For ADC master, conversion is stopped and ADC is disabled. 
  *         For ADC slave, ADC is disabled only (conversion stop of ADC master
  *         has already stopped conversion of ADC slave).
  * @note   In case of auto-injection mode, HAL_ADC_Stop() must be used.
  * @param  hadc: ADC handle
  * @retval None
  */
HAL_StatusTypeDef HAL_ADCEx_InjectedStop_IT(ADC_HandleTypeDef* hadc)
{ 
  HAL_StatusTypeDef tmp_status = HAL_OK;
  
  /* Check the parameters */
  assert_param(IS_ADC_ALL_INSTANCE(hadc->Instance));
  
  /* Process locked */
  __HAL_LOCK(hadc);
  
  /* 1. Stop potential conversion on going on injected group only. */
  tmp_status = ADC_ConversionStop(hadc, ADC_INJECTED_GROUP);
  
  /* Disable ADC peripheral if injected conversions are effectively stopped   */
  /* and if no conversion on the other group (regular group) is intended to   */
  /* continue.                                                                */
  if (tmp_status == HAL_OK)
  {
    /* Disable ADC end of conversion interrupt for injected channels */
    __HAL_ADC_DISABLE_IT(hadc, (ADC_IT_JEOC | ADC_IT_JEOS | ADC_FLAG_JQOVF));
    
    if ((ADC_IS_CONVERSION_ONGOING_REGULAR(hadc) == RESET))
    {
      /* 2. Disable the ADC peripheral */
      tmp_status = ADC_Disable(hadc);
      
      /* Check if ADC is effectively disabled */
      if (tmp_status == HAL_OK)
      {
        /* Change ADC state */
        /* Clear HAL_ADC_STATE_REG_BUSY and HAL_ADC_STATE_INJ_BUSY bits, set HAL_ADC_STATE_READY bit */
        ADC_STATE_CLR_SET(hadc->State, (HAL_ADC_STATE_REG_BUSY|HAL_ADC_STATE_INJ_BUSY), HAL_ADC_STATE_READY);
      }
    }
    /* Conversion on injected group is stopped, but ADC not disabled since    */
    /* conversion on regular group is still running.                          */
    else
    {
      /* Clear HAL_ADC_STATE_INJ_BUSY bit */
      CLEAR_BIT(hadc->State, HAL_ADC_STATE_INJ_BUSY); 
    }
  }

  /* Process unlocked */
  __HAL_UNLOCK(hadc);
  
  /* Return function status */
  return tmp_status;
}


/**
  * @brief  Enable ADC, start MultiMode conversion and transfer regular results through DMA.
  * @note   Multimode must have been previously configured using 
  *         HAL_ADCEx_MultiModeConfigChannel() function.
  *         Interruptions enabled in this function:
  *          overrun, DMA half transfer, DMA transfer complete. 
  *         Each of these interruptions has its dedicated callback function.
  * @note   State field of Slave ADC handle is not updated in this configuration: 
  *          user should not rely on it for information related to Slave regular 
  *         conversions.                                                                          
  * @param  hadc: ADC handle of ADC master (handle of ADC slave must not be used)
  * @param  pData: Destination Buffer address.
  * @param  Length: Length of data to be transferred from ADC peripheral to memory (in bytes).
  * @retval None
  */
HAL_StatusTypeDef HAL_ADCEx_MultiModeStart_DMA(ADC_HandleTypeDef* hadc, uint32_t* pData, uint32_t Length)
{
  HAL_StatusTypeDef tmp_status = HAL_OK;
  ADC_HandleTypeDef tmphadcSlave;
  ADC_Common_TypeDef *tmpADC_Common;

  /* Check the parameters */
  assert_param(IS_ADC_MULTIMODE_MASTER_INSTANCE(hadc->Instance));
  assert_param(IS_FUNCTIONAL_STATE(hadc->Init.ContinuousConvMode));
  assert_param(IS_ADC_EXTTRIG_EDGE(hadc->Init.ExternalTrigConvEdge));
  assert_param(IS_FUNCTIONAL_STATE(hadc->Init.DMAContinuousRequests));
  
  if (ADC_IS_CONVERSION_ONGOING_REGULAR(hadc))
  {
    return HAL_BUSY;
  }
  else
  {
    /* Process locked */
    __HAL_LOCK(hadc);

    /* Set a temporary handle of the ADC slave associated to the ADC master   */
    ADC_MULTI_SLAVE(hadc, &tmphadcSlave);
  
    if (tmphadcSlave.Instance == NULL)
    {
      /* Update ADC state machine to error */
      SET_BIT(hadc->State, HAL_ADC_STATE_ERROR_CONFIG);
    
      /* Process unlocked */
      __HAL_UNLOCK(hadc);
    
      return HAL_ERROR;
    }
    
  
    /* Enable the ADC peripherals: master and slave (in case if not already   */
    /* enabled previously)                                                    */
    tmp_status = ADC_Enable(hadc);
    if (tmp_status == HAL_OK)
    {
      tmp_status = ADC_Enable(&tmphadcSlave);
    }
  
    /* Start multimode conversion of ADCs pair */
    if (tmp_status == HAL_OK)
    {
      /* Update Master State */
      /* Clear HAL_ADC_STATE_READY and regular conversion results bits, set HAL_ADC_STATE_REG_BUSY bit */
      ADC_STATE_CLR_SET(hadc->State, (HAL_ADC_STATE_READY|HAL_ADC_STATE_REG_EOC|HAL_ADC_STATE_REG_OVR|HAL_ADC_STATE_REG_EOSMP), HAL_ADC_STATE_REG_BUSY);

       
      /* Set ADC error code to none */
      ADC_CLEAR_ERRORCODE(hadc);
      
      
      /* Set the DMA transfer complete callback */
      hadc->DMA_Handle->XferCpltCallback = ADC_DMAConvCplt;
         
      /* Set the DMA half transfer complete callback */
      hadc->DMA_Handle->XferHalfCpltCallback = ADC_DMAHalfConvCplt;
      
      /* Set the DMA error callback */
      hadc->DMA_Handle->XferErrorCallback = ADC_DMAError ;
      
      /* Pointer to the common control register  */
      tmpADC_Common = ADC_COMMON_REGISTER(hadc);
      
      
      /* Manage ADC and DMA start: ADC overrun interruption, DMA start, ADC     */
      /* start (in case of SW start):                                           */
  
      /* Clear regular group conversion flag and overrun flag */
      /* (To ensure of no unknown state from potential previous ADC operations) */
      __HAL_ADC_CLEAR_FLAG(hadc, (ADC_FLAG_EOC | ADC_FLAG_EOS | ADC_FLAG_OVR));
      
      /* Enable ADC overrun interrupt */
      __HAL_ADC_ENABLE_IT(hadc, ADC_IT_OVR);
  
      /* Start the DMA channel */
      HAL_DMA_Start_IT(hadc->DMA_Handle, (uint32_t)&tmpADC_Common->CDR, (uint32_t)pData, Length);
          
      /* Enable conversion of regular group.                                    */
      /* Process unlocked */
      __HAL_UNLOCK(hadc);      
      /* If software start has been selected, conversion starts immediately.    */
      /* If external trigger has been selected, conversion will start at next   */
      /* trigger event.                                                         */
      SET_BIT(hadc->Instance->CR, ADC_CR_ADSTART);

    }
    else
    {
      /* Process unlocked */
      __HAL_UNLOCK(hadc);
    }
  
    /* Return function status */
    return tmp_status;
  }
}

/**
  * @brief  Stop multimode ADC conversion, disable ADC DMA transfer, disable ADC peripheral.
  * @note   Multimode is kept enabled after this function. MultiMode DMA bits
  *         (MDMA and DMACFG bits of common CCR register) are maintained. To disable 
  *         Multimode (set with HAL_ADCEx_MultiModeConfigChannel()), ADC must be 
  *         reinitialized using HAL_ADC_Init() or HAL_ADC_DeInit(), or the user can
  *         resort to HAL_ADCEx_DisableMultiMode() API.
  * @note   In case of DMA configured in circular mode, function 
  *         HAL_ADC_Stop_DMA() must be called after this function with handle of
  *         ADC slave, to properly disable the DMA channel.
  * @param  hadc: ADC handle of ADC master (handle of ADC slave must not be used)
  * @retval None
  */
HAL_StatusTypeDef HAL_ADCEx_MultiModeStop_DMA(ADC_HandleTypeDef* hadc)
{
  HAL_StatusTypeDef tmp_status = HAL_OK;
  uint32_t tickstart;
  ADC_HandleTypeDef tmphadcSlave;
  
  /* Check the parameters */
  assert_param(IS_ADC_MULTIMODE_MASTER_INSTANCE(hadc->Instance));
  
  /* Process locked */
  __HAL_LOCK(hadc);
  
 
  /* 1. Stop potential multimode conversion on going, on regular and injected groups */
  tmp_status = ADC_ConversionStop(hadc, ADC_REGULAR_INJECTED_GROUP);

  /* Disable ADC peripheral if conversions are effectively stopped */
  if (tmp_status == HAL_OK)
  {                                      
    /* Set a temporary handle of the ADC slave associated to the ADC master   */
    ADC_MULTI_SLAVE(hadc, &tmphadcSlave);
    
    if (tmphadcSlave.Instance == NULL)
    {
      /* Update ADC state machine to error */
      SET_BIT(hadc->State, HAL_ADC_STATE_ERROR_CONFIG);
      
      /* Process unlocked */
      __HAL_UNLOCK(hadc);
      
      return HAL_ERROR;
    }
    
    /* Procedure to disable the ADC peripheral: wait for conversions          */
    /* effectively stopped (ADC master and ADC slave), then disable ADC       */
    
    /* 1. Wait until ADSTP=0 for ADC master and ADC slave*/
    tickstart = HAL_GetTick();  

    while(ADC_IS_CONVERSION_ONGOING_REGULAR(hadc)          || 
          ADC_IS_CONVERSION_ONGOING_REGULAR(&tmphadcSlave)   )
    {
      if((HAL_GetTick()-tickstart) > ADC_STOP_CONVERSION_TIMEOUT)
      {
        /* Update ADC state machine to error */
        SET_BIT(hadc->State, HAL_ADC_STATE_ERROR_INTERNAL);
        
        /* Process unlocked */
        __HAL_UNLOCK(hadc);
        
        return HAL_ERROR;
      }
    }
    
    /* Disable the DMA channel (in case of DMA in circular mode or stop       */
    /* while DMA transfer is on going)                                        */
    /* Note: DMA channel of ADC slave should be stopped after this function   */
    /*       with HAL_ADC_Stop_DMA() API.                                     */
    tmp_status = HAL_DMA_Abort(hadc->DMA_Handle);
    
    /* Check if DMA channel effectively disabled */
    if (tmp_status == HAL_ERROR)
    {
      /* Update ADC state machine to error */
      SET_BIT(hadc->State, HAL_ADC_STATE_ERROR_DMA); 
    }
    
    /* Disable ADC overrun interrupt */
    __HAL_ADC_DISABLE_IT(hadc, ADC_IT_OVR);
    
    
    
    /* 2. Disable the ADC peripherals: master and slave */
    /* Update "tmp_status" only if DMA channel disabling passed, to keep in */
    /* memory a potential failing status.                                     */
    if (tmp_status == HAL_OK)
    {
      /* Check if ADC are effectively disabled */
      if ((ADC_Disable(hadc) == HAL_OK)         &&
          (ADC_Disable(&tmphadcSlave) == HAL_OK)   )
      {
        tmp_status = HAL_OK;
      }
    }
    else
    {
      ADC_Disable(hadc);
      ADC_Disable(&tmphadcSlave);
    }
    /* Change ADC state (ADC master) */
    /* Clear HAL_ADC_STATE_REG_BUSY and HAL_ADC_STATE_INJ_BUSY bits, set HAL_ADC_STATE_READY bit */
    ADC_STATE_CLR_SET(hadc->State, (HAL_ADC_STATE_REG_BUSY|HAL_ADC_STATE_INJ_BUSY), HAL_ADC_STATE_READY);
    
  }
  
  /* Process unlocked */
  __HAL_UNLOCK(hadc);
  
  /* Return function status */
  return tmp_status;
}


/**
  * @brief  Return the last ADC Master and Slave regular conversions results when in multimode configuration.
  * @param  hadc: ADC handle of ADC Master (handle of ADC Slave must not be used)
  * @retval The converted data values.
  */
uint32_t HAL_ADCEx_MultiModeGetValue(ADC_HandleTypeDef* hadc)
{
  ADC_Common_TypeDef *tmpADC_Common;
  
  /* Check the parameters */
  assert_param(IS_ADC_MULTIMODE_MASTER_INSTANCE(hadc->Instance));
  
  /* Pointer to the common control register  */
  tmpADC_Common = ADC_COMMON_REGISTER(hadc);
  
  /* Return the multi mode conversion value */
  return tmpADC_Common->CDR;
}


/**
  * @brief  Get ADC injected group conversion result.
  * @param  hadc: ADC handle
  * @param  InjectedRank: the converted ADC injected rank.
  *          This parameter can be one of the following values:
  *            @arg @ref ADC_INJECTED_RANK_1 Injected Channel1 selected
  *            @arg @ref ADC_INJECTED_RANK_2 Injected Channel2 selected
  *            @arg @ref ADC_INJECTED_RANK_3 Injected Channel3 selected
  *            @arg @ref ADC_INJECTED_RANK_4 Injected Channel4 selected
  * @note   Reading JDRy register automatically clears JEOC flag. To reset JEOS 
  *         flag the user must resort to the macro 
  *         __HAL_ADC_CLEAR_FLAG(hadc, ADC_FLAG_JEOS).    
  * @retval None
  */
uint32_t HAL_ADCEx_InjectedGetValue(ADC_HandleTypeDef* hadc, uint32_t InjectedRank)
{
  uint32_t tmp_jdr = 0;

  /* Check the parameters */
  assert_param(IS_ADC_ALL_INSTANCE(hadc->Instance));
  assert_param(IS_ADC_INJECTED_RANK(InjectedRank));
  
  
  /* Get ADC converted value */ 
  switch(InjectedRank)
  {  
    case ADC_INJECTED_RANK_4: 
      tmp_jdr = hadc->Instance->JDR4;
      break;
    case ADC_INJECTED_RANK_3: 
      tmp_jdr = hadc->Instance->JDR3;
      break;
    case ADC_INJECTED_RANK_2: 
      tmp_jdr = hadc->Instance->JDR2;
      break;
    case ADC_INJECTED_RANK_1:
    default:
      tmp_jdr = hadc->Instance->JDR1;
      break;
  }
  
  /* Return ADC converted value */ 
  return tmp_jdr;
}

/**
  * @brief  Injected conversion complete callback in non-blocking mode. 
  * @param  hadc: ADC handle
  * @retval None
  */
__weak void HAL_ADCEx_InjectedConvCpltCallback(ADC_HandleTypeDef* hadc)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hadc);

  /* NOTE : This function should not be modified. When the callback is needed,
            function HAL_ADCEx_InjectedConvCpltCallback must be implemented in the user file.
  */
}


/**
  * @brief  Injected context queue overflow callback. 
  * @note   This callback is called if injected context queue is enabled
            (parameter "QueueInjectedContext" in injected channel configuration)
            and if a new injected context is set when queue is full (maximum 2
            contexts).
  * @param  hadc: ADC handle
  * @retval None
  */
__weak void HAL_ADCEx_InjectedQueueOverflowCallback(ADC_HandleTypeDef* hadc)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hadc);

  /* NOTE : This function should not be modified. When the callback is needed,
            function HAL_ADCEx_InjectedQueueOverflowCallback must be implemented in the user file.
  */
}

/**
  * @brief  Analog watchdog 2 callback in non-blocking mode.        
  * @param  hadc: ADC handle
  * @retval None
  */
__weak void HAL_ADCEx_LevelOutOfWindow2Callback(ADC_HandleTypeDef* hadc)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hadc);

  /* NOTE : This function should not be modified. When the callback is needed,
            function HAL_ADCEx_LevelOutOfWindow2Callback must be implemented in the user file.
  */
}

/**
  * @brief  Analog watchdog 3 callback in non-blocking mode.        
  * @param  hadc: ADC handle
  * @retval None
  */
__weak void HAL_ADCEx_LevelOutOfWindow3Callback(ADC_HandleTypeDef* hadc)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hadc);

  /* NOTE : This function should not be modified. When the callback is needed,
            function HAL_ADCEx_LevelOutOfWindow3Callback must be implemented in the user file.
  */
}


/**
  * @brief  End Of Sampling callback in non-blocking mode.        
  * @param  hadc: ADC handle
  * @retval None
  */
__weak void HAL_ADCEx_EndOfSamplingCallback(ADC_HandleTypeDef* hadc)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hadc);

  /* NOTE : This function should not be modified. When the callback is needed,
            function HAL_ADCEx_EndOfSamplingCallback must be implemented in the user file.
  */
}



/**  
  * @brief  Stop ADC conversion of regular groups, disable ADC peripheral if no injected conversion is on-going.
  * @param  hadc: ADC handle
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_ADCEx_RegularStop(ADC_HandleTypeDef* hadc)
{ 
  HAL_StatusTypeDef tmp_status = HAL_OK;
  
  /* Check the parameters */
  assert_param(IS_ADC_ALL_INSTANCE(hadc->Instance));
  
  /* Process locked */
  __HAL_LOCK(hadc);
  
  /* 1. Stop potential regular conversion on going */
  tmp_status = ADC_ConversionStop(hadc, ADC_REGULAR_GROUP);
  
  /* Disable ADC peripheral if regular conversions are effectively stopped 
     and if no injected conversions are on-going */
  if (tmp_status == HAL_OK)
  {
    /* Clear HAL_ADC_STATE_REG_BUSY bit */ 
    CLEAR_BIT(hadc->State, HAL_ADC_STATE_REG_BUSY); 
     
    if (ADC_IS_CONVERSION_ONGOING_INJECTED(hadc) == RESET)
    {
      /* 2. Disable the ADC peripheral */
      tmp_status = ADC_Disable(hadc);
      
      /* Check if ADC is effectively disabled */
      if (tmp_status == HAL_OK)
      {
        /* Change ADC state */
        /* Clear HAL_ADC_STATE_INJ_BUSY bit, set HAL_ADC_STATE_READY bit */
        ADC_STATE_CLR_SET(hadc->State, HAL_ADC_STATE_INJ_BUSY, HAL_ADC_STATE_READY);
      }
    }
    /* Conversion on injected group is stopped, but ADC not disabled since    */
    /* conversion on regular group is still running.                          */
    else
    {
      SET_BIT(hadc->State, HAL_ADC_STATE_INJ_BUSY);
    }
  }

  /* Process unlocked */
  __HAL_UNLOCK(hadc);
  
  /* Return function status */
  return tmp_status;
}


/**  
  * @brief  Stop ADC conversion of regular groups when interruptions are enabled, disable ADC peripheral if no injected conversion is on-going.
  * @param  hadc: ADC handle
  * @retval HAL status.
  */  
HAL_StatusTypeDef HAL_ADCEx_RegularStop_IT(ADC_HandleTypeDef* hadc)
{
  HAL_StatusTypeDef tmp_status = HAL_OK;
  
  /* Check the parameters */
  assert_param(IS_ADC_ALL_INSTANCE(hadc->Instance));
  
  /* Process locked */
  __HAL_LOCK(hadc);
  
  /* 1. Stop potential regular conversion on going */
  tmp_status = ADC_ConversionStop(hadc, ADC_REGULAR_GROUP);
  
  /* Disable ADC peripheral if conversions are effectively stopped
    and if no injected conversion is on-going */
  if (tmp_status == HAL_OK)
  {
    /* Clear HAL_ADC_STATE_REG_BUSY bit */ 
    CLEAR_BIT(hadc->State, HAL_ADC_STATE_REG_BUSY);  
     
    /* Disable all regular-related interrupts */
    __HAL_ADC_DISABLE_IT(hadc, (ADC_IT_EOC | ADC_IT_EOS | ADC_IT_OVR));
    
    /* 2. Disable ADC peripheral if no injected conversions are on-going */
    if (ADC_IS_CONVERSION_ONGOING_INJECTED(hadc) == RESET)
    {     
      tmp_status = ADC_Disable(hadc);
      /* if no issue reported */
      if (tmp_status == HAL_OK)
      {
        /* Change ADC state */
        /* Clear HAL_ADC_STATE_INJ_BUSY bit, set HAL_ADC_STATE_READY bit */
        ADC_STATE_CLR_SET(hadc->State, HAL_ADC_STATE_INJ_BUSY, HAL_ADC_STATE_READY);
      }
    }
    else
    {
      SET_BIT(hadc->State, HAL_ADC_STATE_INJ_BUSY);
    }
  }

  /* Process unlocked */
  __HAL_UNLOCK(hadc);
  
  /* Return function status */
  return tmp_status;
}


/**
  * @brief  Stop ADC conversion of regular groups and disable ADC DMA transfer, disable ADC peripheral if no injected conversion is on-going.
  * @note   HAL_ADCEx_RegularStop_DMA() function is dedicated to single-ADC mode only. 
  *         For multimode (when multimode feature is available), 
  *         HAL_ADCEx_RegularMultiModeStop_DMA() API must be used.
  * @param  hadc: ADC handle
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_ADCEx_RegularStop_DMA(ADC_HandleTypeDef* hadc)
{  
  HAL_StatusTypeDef tmp_status = HAL_OK;
  
  /* Check the parameters */
  assert_param(IS_ADC_ALL_INSTANCE(hadc->Instance));

  /* Process locked */
  __HAL_LOCK(hadc);
  
  /* 1. Stop potential regular conversion on going */
  tmp_status = ADC_ConversionStop(hadc, ADC_REGULAR_GROUP);
  
  /* Disable ADC peripheral if conversions are effectively stopped 
     and if no injected conversion is on-going */
  if (tmp_status == HAL_OK)
  {
    /* Clear HAL_ADC_STATE_REG_BUSY bit */ 
    CLEAR_BIT(hadc->State, HAL_ADC_STATE_REG_BUSY);
    
    /* Disable ADC DMA (ADC DMA configuration ADC_CFGR_DMACFG is kept) */
    CLEAR_BIT(hadc->Instance->CFGR, ADC_CFGR_DMAEN); 
    
    /* Disable the DMA channel (in case of DMA in circular mode or stop while */
    /* while DMA transfer is on going)                                        */
    tmp_status = HAL_DMA_Abort(hadc->DMA_Handle);   
    
    /* Check if DMA channel effectively disabled */
    if (tmp_status != HAL_OK)
    {
      /* Update ADC state machine to error */
      SET_BIT(hadc->State, HAL_ADC_STATE_ERROR_DMA);   
    }
    
    /* Disable ADC overrun interrupt */
    __HAL_ADC_DISABLE_IT(hadc, ADC_IT_OVR);
    
    /* 2. Disable the ADC peripheral */
    /* Update "tmp_status" only if DMA channel disabling passed, to keep in */
    /* memory a potential failing status.                                     */
    if (ADC_IS_CONVERSION_ONGOING_INJECTED(hadc) == RESET)
    {    
      if (tmp_status == HAL_OK)
      {
        tmp_status = ADC_Disable(hadc);
      }
      else
      {
        ADC_Disable(hadc);
      }

      /* Check if ADC is effectively disabled */
      if (tmp_status == HAL_OK)
      {
        /* Change ADC state */
        /* Clear HAL_ADC_STATE_INJ_BUSY bit, set HAL_ADC_STATE_READY bit */
        ADC_STATE_CLR_SET(hadc->State, HAL_ADC_STATE_INJ_BUSY, HAL_ADC_STATE_READY);      
      }
    }
    else
    {
      SET_BIT(hadc->State, HAL_ADC_STATE_INJ_BUSY);  
    }
  }

  /* Process unlocked */
  __HAL_UNLOCK(hadc);
  
  /* Return function status */
  return tmp_status;
}


/**
  * @brief  Stop DMA-based multimode ADC conversion, disable ADC DMA transfer, disable ADC peripheral if no injected conversion is on-going.
  * @note   Multimode is kept enabled after this function. Multimode DMA bits
  *         (MDMA and DMACFG bits of common CCR register) are maintained. To disable 
  *         multimode (set with HAL_ADCEx_MultiModeConfigChannel()), ADC must be 
  *         reinitialized using HAL_ADC_Init() or HAL_ADC_DeInit(), or the user can
  *         resort to HAL_ADCEx_DisableMultiMode() API.  
  * @note   In case of DMA configured in circular mode, function 
  *         HAL_ADCEx_RegularStop_DMA() must be called after this function with handle of
  *         ADC slave, to properly disable the DMA channel.
  * @param  hadc: ADC handle of ADC master (handle of ADC slave must not be used)
  * @retval None
  */
HAL_StatusTypeDef HAL_ADCEx_RegularMultiModeStop_DMA(ADC_HandleTypeDef* hadc)
{
  HAL_StatusTypeDef tmp_status = HAL_OK;
  uint32_t tickstart;
  ADC_HandleTypeDef tmphadcSlave;
  
  /* Check the parameters */
  assert_param(IS_ADC_MULTIMODE_MASTER_INSTANCE(hadc->Instance));
  
  /* Process locked */
  __HAL_LOCK(hadc);
  
 
  /* 1. Stop potential multimode conversion on going, on regular groups */
  tmp_status = ADC_ConversionStop(hadc, ADC_REGULAR_GROUP);

  /* Disable ADC peripheral if conversions are effectively stopped */
  if (tmp_status == HAL_OK)
  { 
    /* Clear HAL_ADC_STATE_REG_BUSY bit */                                     
    CLEAR_BIT(hadc->State, HAL_ADC_STATE_REG_BUSY);
    
    /* Set a temporary handle of the ADC slave associated to the ADC master   */
    ADC_MULTI_SLAVE(hadc, &tmphadcSlave);
    
    if (tmphadcSlave.Instance == NULL)
    {
      /* Update ADC state machine to error */
      SET_BIT(hadc->State, HAL_ADC_STATE_ERROR_CONFIG);
      
      /* Process unlocked */
      __HAL_UNLOCK(hadc);
      
      return HAL_ERROR;
    }
    
    /* Procedure to disable the ADC peripheral: wait for conversions          */
    /* effectively stopped (ADC master and ADC slave), then disable ADC       */
    
    /* 1. Wait until ADSTP=0 for ADC master and ADC slave*/
    tickstart = HAL_GetTick();  

    while(ADC_IS_CONVERSION_ONGOING_REGULAR(hadc)          || 
          ADC_IS_CONVERSION_ONGOING_REGULAR(&tmphadcSlave)   )
    {
      if((HAL_GetTick()-tickstart) > ADC_STOP_CONVERSION_TIMEOUT)
      {
        /* Update ADC state machine to error */
        SET_BIT(hadc->State, HAL_ADC_STATE_ERROR_INTERNAL);
        
        /* Process unlocked */
        __HAL_UNLOCK(hadc);
        
        return HAL_ERROR;
      }
    }
    
    /* Disable the DMA channel (in case of DMA in circular mode or stop       */
    /* while DMA transfer is on going)                                        */
    /* Note: DMA channel of ADC slave should be stopped after this function   */
    /* with HAL_ADCEx_RegularStop_DMA() API.                                  */
    tmp_status = HAL_DMA_Abort(hadc->DMA_Handle);
    
    /* Check if DMA channel effectively disabled */
    if (tmp_status != HAL_OK)
    {
      /* Update ADC state machine to error */
      SET_BIT(hadc->State, HAL_ADC_STATE_ERROR_DMA);  
    }
    
    /* Disable ADC overrun interrupt */
    __HAL_ADC_DISABLE_IT(hadc, ADC_IT_OVR);
    
    
    
    /* 2. Disable the ADC peripherals: master and slave if no injected        */
    /*   conversion is on-going.                                              */
    /* Update "tmp_status" only if DMA channel disabling passed, to keep in */
    /* memory a potential failing status.                                     */
    if (tmp_status == HAL_OK)
    {
       if (ADC_IS_CONVERSION_ONGOING_INJECTED(hadc) == RESET)
       {  
         tmp_status =  ADC_Disable(hadc);
         if (tmp_status == HAL_OK)
         {
           if (ADC_IS_CONVERSION_ONGOING_INJECTED(&tmphadcSlave) == RESET)
           {
             tmp_status =  ADC_Disable(&tmphadcSlave);
           }
         }
       }
       
       if (tmp_status == HAL_OK)
       {
         /* Both Master and Slave ADC's could be disabled. Update Master State */
         /* Clear HAL_ADC_STATE_INJ_BUSY bit, set HAL_ADC_STATE_READY bit */
         ADC_STATE_CLR_SET(hadc->State, HAL_ADC_STATE_INJ_BUSY, HAL_ADC_STATE_READY);
       }
       else
       {
         /* injected (Master or Slave) conversions are still on-going,
            no Master State change */
       }
          

    }

    
  }
  
  /* Process unlocked */
  __HAL_UNLOCK(hadc);
  
  /* Return function status */
  return tmp_status;
}


/**
  * @}
  */

/** @defgroup ADCEx_Exported_Functions_Group2 Extended Peripheral Control functions
  * @brief    Extended Peripheral Control functions
  *
@verbatim   
 ===============================================================================
             ##### Peripheral Control functions #####
 ===============================================================================  
    [..]  This section provides functions allowing to:
      (+) Configure channels on injected group
      (+) Configure multimode when multimode feature is available
      (+) Enable or Disable Injected Queue
      (+) Disable ADC voltage regulator
      (+) Enter ADC deep-power-down mode      
      
      
@endverbatim
  * @{
  */

/**  
  * @brief  Configure the ADC injected group and the selected channel to be linked to the injected group.
  * @note   Possibility to update parameters on the fly:
  *         This function initializes injected group, consecutive calls to this 
  *         function can be used to reconfigure some parameters of structure
  *         "ADC_InjectionConfTypeDef" on the fly, without resetting the ADC.
  *         The setting of these parameters is conditioned to ADC state.
  *         For parameters constraints, see comments of structure 
  *         "ADC_InjectionConfTypeDef".
  * @note   In case of usage of internal measurement channels (Vbat/VrefInt/TempSensor),
  *         The internal paths can be disabled using function HAL_ADC_DeInit().
  * @note   To reset injected sequencer, function HAL_ADCEx_InjectedStop() can
  *         be used.
  * @note   Caution: For Injected Context Queue use, a context must be fully 
  *         defined before start of injected conversion. All channels are configured 
  *         consecutively for the same ADC instance. Therefore, the number of calls to 
  *         HAL_ADCEx_InjectedConfigChannel() must be equal to the value of parameter 
  *         InjectedNbrOfConversion for each context.
  *  - Example 1: If 1 context is intended to be used (or if there is no use of the 
  *    Injected Queue Context feature) and if the context contains 3 injected ranks 
  *    (InjectedNbrOfConversion = 3), HAL_ADCEx_InjectedConfigChannel() must be  
  *    called once for each channel (i.e. 3 times) before starting a conversion.   
  *    This function must not be called to configure a 4th injected channel:   
  *    it would start a new context into context queue.
  *  - Example 2: If 2 contexts are intended to be used and each of them contains 
  *    3 injected ranks (InjectedNbrOfConversion = 3),  
  *    HAL_ADCEx_InjectedConfigChannel() must be called once for each channel and  
  *    for each context (3 channels x 2 contexts = 6 calls). Conversion can  
  *    start once the 1st context is set, that is after the first three 
  *    HAL_ADCEx_InjectedConfigChannel() calls. The 2nd context can be set on the fly.
  * @param  hadc: ADC handle
  * @param  sConfigInjected: Structure of ADC injected group and ADC channel for
  *         injected group.
  * @retval None
  */
HAL_StatusTypeDef HAL_ADCEx_InjectedConfigChannel(ADC_HandleTypeDef* hadc, ADC_InjectionConfTypeDef* sConfigInjected)
{
  HAL_StatusTypeDef tmp_status = HAL_OK;
  ADC_Common_TypeDef *tmpADC_Common;
  uint32_t tmpOffsetShifted;
  uint32_t wait_loop_index = 0;
  

  uint32_t tmp_JSQR_ContextQueueBeingBuilt = 0;
  
  /* Check the parameters */
  assert_param(IS_ADC_ALL_INSTANCE(hadc->Instance));
  assert_param(IS_ADC_SAMPLE_TIME(sConfigInjected->InjectedSamplingTime));
  assert_param(IS_ADC_SINGLE_DIFFERENTIAL(sConfigInjected->InjectedSingleDiff));
  assert_param(IS_FUNCTIONAL_STATE(sConfigInjected->AutoInjectedConv));
  assert_param(IS_FUNCTIONAL_STATE(sConfigInjected->QueueInjectedContext));
  assert_param(IS_ADC_EXTTRIGINJEC_EDGE(sConfigInjected->ExternalTrigInjecConvEdge));
  assert_param(IS_ADC_EXTTRIGINJEC(sConfigInjected->ExternalTrigInjecConv));
  assert_param(IS_ADC_OFFSET_NUMBER(sConfigInjected->InjectedOffsetNumber));
  assert_param(IS_ADC_RANGE(ADC_GET_RESOLUTION(hadc), sConfigInjected->InjectedOffset));
  assert_param(IS_FUNCTIONAL_STATE(sConfigInjected->InjecOversamplingMode));
  
  if(hadc->Init.ScanConvMode != ADC_SCAN_DISABLE)
  {
    assert_param(IS_ADC_INJECTED_RANK(sConfigInjected->InjectedRank));
    assert_param(IS_ADC_INJECTED_NB_CONV(sConfigInjected->InjectedNbrOfConversion));
    assert_param(IS_FUNCTIONAL_STATE(sConfigInjected->InjectedDiscontinuousConvMode));
  }
  
  
  /* if JOVSE is set, the value of the OFFSETy_EN bit in ADCx_OFRy register is  
     ignored (considered as reset) */
  assert_param(!((sConfigInjected->InjectedOffsetNumber != ADC_OFFSET_NONE) && (sConfigInjected->InjecOversamplingMode == ENABLE))); 
    
  /* JDISCEN and JAUTO bits can't be set at the same time  */
  assert_param(!((sConfigInjected->InjectedDiscontinuousConvMode == ENABLE) && (sConfigInjected->AutoInjectedConv == ENABLE))); 

  /*  DISCEN and JAUTO bits can't be set at the same time */
  assert_param(!((hadc->Init.DiscontinuousConvMode == ENABLE) && (sConfigInjected->AutoInjectedConv == ENABLE)));  

  /* Verification of channel number */
  if (sConfigInjected->InjectedSingleDiff != ADC_DIFFERENTIAL_ENDED)
  {
     assert_param(IS_ADC_CHANNEL(hadc, sConfigInjected->InjectedChannel));
  }
  else
  {
    assert_param(IS_ADC_DIFF_CHANNEL(hadc, sConfigInjected->InjectedChannel));
  }                
 
    
  /* Process locked */
  __HAL_LOCK(hadc);



  /* Configuration of Injected group sequencer.                               */
  /* Hardware constraint: Must fully define injected context register JSQR    */
  /* before make it entering into injected sequencer queue.                   */
  /*                                                                          */
  /* - if scan mode is disabled:                                              */
  /*    * Injected channels sequence length is set to 0x00: 1 channel         */
  /*      converted (channel on injected rank 1)                              */
  /*      Parameter "InjectedNbrOfConversion" is discarded.                   */
  /*    * Injected context register JSQR setting is simple: register is fully */
  /*      defined on one call of this function (for injected rank 1) and can  */
  /*      be entered into queue directly.                                     */
  /* - if scan mode is enabled:                                               */
  /*    * Injected channels sequence length is set to parameter               */
  /*      "InjectedNbrOfConversion".                                          */
  /*    * Injected context register JSQR setting more complex: register is    */
  /*      fully defined over successive calls of this function, for each      */
  /*      injected channel rank. It is entered into queue only when all       */
  /*      injected ranks have been set.                                       */
  /*   Note: Scan mode is not present by hardware on this device, but used    */
  /*   by software for alignment over all STM32 devices.                      */

  if ((hadc->Init.ScanConvMode == ADC_SCAN_DISABLE)  ||
      (sConfigInjected->InjectedNbrOfConversion == 1)  )
  {
    /* Configuration of context register JSQR:                                */
    /*  - number of ranks in injected group sequencer: fixed to 1st rank      */
    /*    (scan mode disabled, only rank 1 used)                              */
    /*  - external trigger to start conversion                                */
    /*  - external trigger polarity                                           */
    /*  - channel set to rank 1 (scan mode disabled, only rank 1 can be used) */
    
    if (sConfigInjected->InjectedRank == ADC_INJECTED_RANK_1)
    {
      /* Enable external trigger if trigger selection is different of         */
      /* software start.                                                      */
      /* Note: This configuration keeps the hardware feature of parameter     */
      /*       ExternalTrigInjecConvEdge "trigger edge none" equivalent to    */
      /*       software start.                                                */
      if ((sConfigInjected->ExternalTrigInjecConv != ADC_INJECTED_SOFTWARE_START)
         && (sConfigInjected->ExternalTrigInjecConvEdge != ADC_EXTERNALTRIGINJECCONV_EDGE_NONE))
      {
        tmp_JSQR_ContextQueueBeingBuilt = ( ADC_JSQR_RK(sConfigInjected->InjectedChannel, ADC_INJECTED_RANK_1) |
                                             sConfigInjected->ExternalTrigInjecConv     |
                                             sConfigInjected->ExternalTrigInjecConvEdge                                );                                              
      }
      else
      {
        tmp_JSQR_ContextQueueBeingBuilt = ( ADC_JSQR_RK(sConfigInjected->InjectedChannel, ADC_INJECTED_RANK_1) );
      }
      

      MODIFY_REG(hadc->Instance->JSQR, ADC_JSQR_FIELDS, tmp_JSQR_ContextQueueBeingBuilt); 
      /* For debug and informative reasons, hadc handle saves JSQR setting */
      hadc->InjectionConfig.ContextQueue = tmp_JSQR_ContextQueueBeingBuilt;
    
    }
  }
  else
  {
    /* Case of scan mode enabled, several channels to set into injected group */
    /* sequencer.                                                             */
    /*                                                                        */
    /* Procedure to define injected context register JSQR over successive     */
    /* calls of this function, for each injected channel rank:                */
    /* 1. Start new context and set parameters related to all injected        */
    /*    channels: injected sequence length and trigger.                     */
    
    /* if hadc->InjectionConfig.ChannelCount is equal to 0, this is the first */
    /*   call of the context under setting                                    */
    if (hadc->InjectionConfig.ChannelCount == 0) 
    {
      /* Initialize number of channels that will be configured on the context */
      /*  being built                                                         */
      hadc->InjectionConfig.ChannelCount = sConfigInjected->InjectedNbrOfConversion;
      /* Handle hadc saves the context under build up over each HAL_ADCEx_InjectedConfigChannel()
         call, this context will be written in JSQR register at the last call.
         At this point, the context is merely reset  */
       hadc->InjectionConfig.ContextQueue = (uint32_t)0x00000000;

      /* Configuration of context register JSQR:                              */
      /*  - number of ranks in injected group sequencer                       */
      /*  - external trigger to start conversion                              */
      /*  - external trigger polarity                                         */
        
      /* Enable external trigger if trigger selection is different of         */
      /* software start.                                                      */
      /* Note: This configuration keeps the hardware feature of parameter     */
      /*       ExternalTrigInjecConvEdge "trigger edge none" equivalent to    */
      /*       software start.                                                */
      if ((sConfigInjected->ExternalTrigInjecConv != ADC_INJECTED_SOFTWARE_START)
         && (sConfigInjected->ExternalTrigInjecConvEdge != ADC_EXTERNALTRIGINJECCONV_EDGE_NONE))
      {      
        tmp_JSQR_ContextQueueBeingBuilt = ((sConfigInjected->InjectedNbrOfConversion - (uint32_t)1)              |
                                             sConfigInjected->ExternalTrigInjecConv |
                                             sConfigInjected->ExternalTrigInjecConvEdge                            );        
      }
      else
      {
        tmp_JSQR_ContextQueueBeingBuilt = ((sConfigInjected->InjectedNbrOfConversion - (uint32_t)1) );
      }
     
      
    } /* if (hadc->InjectionConfig.ChannelCount == 0) */  
    

    /* 2. Continue setting of context under definition with parameter       */
    /*    related to each channel: channel rank sequence                    */ 
    /* Clear the old JSQx bits for the selected rank */
    tmp_JSQR_ContextQueueBeingBuilt &= ~ADC_JSQR_RK(ADC_SQR3_SQ10, sConfigInjected->InjectedRank);
    
    /* Set the JSQx bits for the selected rank */
    tmp_JSQR_ContextQueueBeingBuilt |= ADC_JSQR_RK(sConfigInjected->InjectedChannel, sConfigInjected->InjectedRank);

    /* Decrease channel count  */
    hadc->InjectionConfig.ChannelCount--;
    
    
    /* 3. tmp_JSQR_ContextQueueBeingBuilt is fully built for this HAL_ADCEx_InjectedConfigChannel()
          call, aggregate the setting to those already built during the previous  
          HAL_ADCEx_InjectedConfigChannel() calls (for the same context of course)  */
    hadc->InjectionConfig.ContextQueue |= tmp_JSQR_ContextQueueBeingBuilt;
          
    /* 4. End of context setting: if this is the last channel set, then write context
        into register JSQR and make it enter into queue                   */
    if (hadc->InjectionConfig.ChannelCount == 0)
    {
      MODIFY_REG(hadc->Instance->JSQR, ADC_JSQR_FIELDS, hadc->InjectionConfig.ContextQueue); 
    }

    
  } 

  /* Parameters update conditioned to ADC state:                              */
  /* Parameters that can be updated when ADC is disabled or enabled without   */
  /* conversion on going on injected group:                                   */
  /*  - Injected context queue: Queue disable (active context is kept) or     */
  /*    enable (context decremented, up to 2 contexts queued)                 */
  /*  - Injected discontinuous mode: can be enabled only if auto-injected     */
  /*    mode is disabled.                                                     */
  if (ADC_IS_CONVERSION_ONGOING_INJECTED(hadc) == RESET)
  {
     
    /* If auto-injected mode is disabled: no constraint                       */
    if (sConfigInjected->AutoInjectedConv == DISABLE)
    {                               
      MODIFY_REG(hadc->Instance->CFGR, ADC_CFGR_JQM | ADC_CFGR_JDISCEN, 
                               ADC_CFGR_INJECT_CONTEXT_QUEUE(sConfigInjected->QueueInjectedContext)          | 
                               ADC_CFGR_INJECT_DISCCONTINUOUS(sConfigInjected->InjectedDiscontinuousConvMode) );                          
    }
    /* If auto-injected mode is enabled: Injected discontinuous setting is   */
    /* discarded.                                                             */
    else
    {
            MODIFY_REG(hadc->Instance->CFGR, ADC_CFGR_JQM | ADC_CFGR_JDISCEN, 
                               ADC_CFGR_INJECT_CONTEXT_QUEUE(sConfigInjected->QueueInjectedContext) );  
    }

  }
  
  
  /* Parameters update conditioned to ADC state:                              */
  /* Parameters that can be updated when ADC is disabled or enabled without   */
  /* conversion on going on regular and injected groups:                      */
  /*  - Automatic injected conversion: can be enabled if injected group       */
  /*    external triggers are disabled.                                       */
  /*  - Channel sampling time                                                 */
  /*  - Channel offset                                                        */
  if (ADC_IS_CONVERSION_ONGOING_REGULAR_INJECTED(hadc) == RESET)
  {    
    /* If injected group external triggers are disabled (set to injected      */
    /* software start): no constraint                                         */
    if ((sConfigInjected->ExternalTrigInjecConv == ADC_INJECTED_SOFTWARE_START)
       || (sConfigInjected->ExternalTrigInjecConvEdge == ADC_EXTERNALTRIGINJECCONV_EDGE_NONE))
    {
         if (sConfigInjected->AutoInjectedConv == ENABLE)
         {
           SET_BIT(hadc->Instance->CFGR, ADC_CFGR_JAUTO);    
         }
         else
         {
           CLEAR_BIT(hadc->Instance->CFGR, ADC_CFGR_JAUTO);    
         }                                                                                  
    }
    /* If Automatic injected conversion was intended to be set and could not  */
    /* due to injected group external triggers enabled, error is reported.    */
    else
    {
      if (sConfigInjected->AutoInjectedConv == ENABLE)
      {
        /* Update ADC state machine to error */
        SET_BIT(hadc->State, HAL_ADC_STATE_ERROR_CONFIG);
        
        tmp_status = HAL_ERROR;
      }
      else
      {
        CLEAR_BIT(hadc->Instance->CFGR, ADC_CFGR_JAUTO); 
      }
    }
    
    
    
    if (sConfigInjected->InjecOversamplingMode == ENABLE)
    {
      assert_param(IS_ADC_OVERSAMPLING_RATIO(sConfigInjected->InjecOversampling.Ratio));
      assert_param(IS_ADC_RIGHT_BIT_SHIFT(sConfigInjected->InjecOversampling.RightBitShift));
      
      /*  JOVSE must be reset in case of triggered regular mode  */
      assert_param(!(READ_BIT(hadc->Instance->CFGR2, ADC_CFGR2_ROVSE|ADC_CFGR2_TROVS) == (ADC_CFGR2_ROVSE|ADC_CFGR2_TROVS)));

      /* Configuration of Injected Oversampler:                                 */
      /*  - Oversampling Ratio                                                  */
      /*  - Right bit shift                                                     */
      
      /* Enable OverSampling mode */
  
       MODIFY_REG(hadc->Instance->CFGR2, ADC_CFGR2_INJ_FIELDS, 
                                 ADC_CFGR2_JOVSE                     | 
                                 sConfigInjected->InjecOversampling.Ratio         |
                                 sConfigInjected->InjecOversampling.RightBitShift );
    }  
    else
    {
      /* Disable Regular OverSampling */
       CLEAR_BIT( hadc->Instance->CFGR2, ADC_CFGR2_JOVSE);
    }    
        
  
    /* Sampling time configuration of the selected channel */
    /* if ADC_Channel_10 ... ADC_Channel_18 is selected */
    if (sConfigInjected->InjectedChannel >= ADC_CHANNEL_10)
    {
      /* Clear the old sample time and set the new one */
      ADC_SMPR2_SETTING(hadc, sConfigInjected->InjectedSamplingTime, sConfigInjected->InjectedChannel);                
    }
    else /* if ADC_Channel_0 ... ADC_Channel_9 is selected */
    {
     /* Clear the old sample time and set the new one */
      ADC_SMPR1_SETTING(hadc, sConfigInjected->InjectedSamplingTime, sConfigInjected->InjectedChannel);             
    }
    
        
    /* Configure the offset: offset enable/disable, channel, offset value */
  
    /* Shift the offset with respect to the selected ADC resolution. */
    /* Offset has to be left-aligned on bit 11, the LSB (right bits) are set to 0 */
    tmpOffsetShifted = ADC_OFFSET_SHIFT_RESOLUTION(hadc, sConfigInjected->InjectedOffset);
    
    switch (sConfigInjected->InjectedOffsetNumber)
    {
    case ADC_OFFSET_1:
      /* Configure offset register 1:                                         */
      /* - Enable offset                                                      */
      /* - Set channel number                                                 */
      /* - Set offset value                                                   */
      MODIFY_REG(hadc->Instance->OFR1, 
                ADC_OFR1_OFFSET1 | ADC_OFR1_OFFSET1_CH | ADC_OFR1_OFFSET1_EN, 
                ADC_OFR1_OFFSET1_EN | ADC_OFR_CHANNEL(sConfigInjected->InjectedChannel) | tmpOffsetShifted);                                    
      break;
    
    case ADC_OFFSET_2:
      /* Configure offset register 2:                                         */
      /* - Enable offset                                                      */
      /* - Set channel number                                                 */
      /* - Set offset value                                                   */
      MODIFY_REG(hadc->Instance->OFR2, 
                ADC_OFR2_OFFSET2 | ADC_OFR2_OFFSET2_CH | ADC_OFR2_OFFSET2_EN, 
                ADC_OFR2_OFFSET2_EN | ADC_OFR_CHANNEL(sConfigInjected->InjectedChannel) | tmpOffsetShifted);                                     
      break;
        
    case ADC_OFFSET_3:
      /* Configure offset register 3:                                         */
      /* - Enable offset                                                      */
      /* - Set channel number                                                 */
      /* - Set offset value                                                   */
      MODIFY_REG(hadc->Instance->OFR3, 
                ADC_OFR3_OFFSET3 | ADC_OFR3_OFFSET3_CH | ADC_OFR3_OFFSET3_EN, 
                ADC_OFR3_OFFSET3_EN | ADC_OFR_CHANNEL(sConfigInjected->InjectedChannel) | tmpOffsetShifted);                                   
      break;
    
    case ADC_OFFSET_4:
      /* Configure offset register 1:                                         */
      /* - Enable offset                                                      */
      /* - Set channel number                                                 */
      /* - Set offset value                                                   */
      MODIFY_REG(hadc->Instance->OFR4, 
                ADC_OFR4_OFFSET4 | ADC_OFR4_OFFSET4_CH | ADC_OFR4_OFFSET4_EN, 
                ADC_OFR4_OFFSET4_EN | ADC_OFR_CHANNEL(sConfigInjected->InjectedChannel) | tmpOffsetShifted);                                  
      break;
    
    /* Case ADC_OFFSET_NONE */
    default :
    /* Scan OFR1, OFR2, OFR3, OFR4 to check if the selected channel is enabled. If this is the case, offset OFRx is disabled. */
      if (((hadc->Instance->OFR1) & ADC_OFR1_OFFSET1_CH) == ADC_OFR_CHANNEL(sConfigInjected->InjectedChannel))
      {
        /* Disable offset OFR1*/
        CLEAR_BIT(hadc->Instance->OFR1, ADC_OFR1_OFFSET1_EN);
      }
      if (((hadc->Instance->OFR2) & ADC_OFR2_OFFSET2_CH) == ADC_OFR_CHANNEL(sConfigInjected->InjectedChannel))
      {
        /* Disable offset OFR2*/
        CLEAR_BIT(hadc->Instance->OFR2, ADC_OFR2_OFFSET2_EN); 
      }
      if (((hadc->Instance->OFR3) & ADC_OFR3_OFFSET3_CH) == ADC_OFR_CHANNEL(sConfigInjected->InjectedChannel))
      {
        /* Disable offset OFR3*/
        CLEAR_BIT(hadc->Instance->OFR3, ADC_OFR3_OFFSET3_EN); 
      }
      if (((hadc->Instance->OFR4) & ADC_OFR4_OFFSET4_CH) == ADC_OFR_CHANNEL(sConfigInjected->InjectedChannel))
      {
        /* Disable offset OFR4*/
        CLEAR_BIT(hadc->Instance->OFR4, ADC_OFR4_OFFSET4_EN); 
      }
      break;
    }

  } /* if (ADC_IS_CONVERSION_ONGOING_REGULAR_INJECTED(hadc) == RESET) */
  
  
  /* Parameters update conditioned to ADC state:                              */
  /* Parameters that can be updated only when ADC is disabled:                */
  /*  - Single or differential mode                                           */
  /*  - Internal measurement channels: Vbat/VrefInt/TempSensor                */
  if (ADC_IS_ENABLE(hadc) == RESET)
  {
    /* Configuration of differential mode */
    if (sConfigInjected->InjectedSingleDiff != ADC_DIFFERENTIAL_ENDED)
    {
      /* Disable differential mode (default mode: single-ended) */
      CLEAR_BIT(hadc->Instance->DIFSEL, ADC_DIFSEL_CHANNEL(sConfigInjected->InjectedChannel));
    }
    else
    {
      /* Enable differential mode */
      SET_BIT(hadc->Instance->DIFSEL, ADC_DIFSEL_CHANNEL(sConfigInjected->InjectedChannel));
      
      /* Sampling time configuration of channel ADC_IN+1 (negative input).
         Starting from channel 9, SMPR2 register must be configured.      */
      if (sConfigInjected->InjectedChannel >= ADC_CHANNEL_9)
      {
        /* Clear the old sample time and set the new one */
        ADC_SMPR2_SETTING(hadc, sConfigInjected->InjectedSamplingTime, sConfigInjected->InjectedChannel+1);                     
      }
      else /* For channels 0 to 8 */
      {
         /* Clear the old sample time and set the new one */
        ADC_SMPR1_SETTING(hadc, sConfigInjected->InjectedSamplingTime, sConfigInjected->InjectedChannel+1);                 
      }
    }
    

    /* Management of internal measurement channels: Vbat/VrefInt/TempSensor   */
    /* internal measurement paths enable: If internal channel selected,       */
    /* enable dedicated internal buffers and path.                            */
    /* Note: these internal measurement paths can be disabled using           */
    /* HAL_ADC_DeInit().                                                      */

    /* Configuration of common ADC parameters                                 */

    tmpADC_Common = ADC_COMMON_REGISTER(hadc);
  
    /* If the requested internal measurement path has already been enabled,   */
    /* bypass the configuration processing.                                   */
    if (( (sConfigInjected->InjectedChannel == ADC_CHANNEL_TEMPSENSOR) &&
          (HAL_IS_BIT_CLR(tmpADC_Common->CCR, ADC_CCR_TSEN))            ) ||
        ( (sConfigInjected->InjectedChannel == ADC_CHANNEL_VBAT)       &&
          (HAL_IS_BIT_CLR(tmpADC_Common->CCR, ADC_CCR_VBATEN))          ) ||
        ( (sConfigInjected->InjectedChannel == ADC_CHANNEL_VREFINT)    &&
          (HAL_IS_BIT_CLR(tmpADC_Common->CCR, ADC_CCR_VREFEN)))
       )
    {
      /* Configuration of common ADC parameters (continuation)                */
      /* Software is allowed to change common parameters only when all ADCs   */
      /* of the common group are disabled.                                    */
      if ((ADC_IS_ENABLE(hadc) == RESET)   &&
         (ADC_ANY_OTHER_ENABLED(hadc) == RESET) )              
      {
        if (sConfigInjected->InjectedChannel == ADC_CHANNEL_TEMPSENSOR)
        {
          if (ADC_TEMPERATURE_SENSOR_INSTANCE(hadc)) 
          {
            SET_BIT(tmpADC_Common->CCR, ADC_CCR_TSEN);
          
            /* Delay for temperature sensor stabilization time */
            /* Compute number of CPU cycles to wait for */
            wait_loop_index = (ADC_TEMPSENSOR_DELAY_US * (SystemCoreClock / 1000000));
            while(wait_loop_index != 0)
            {
              wait_loop_index--;
            }
          }
        }
        else if (sConfigInjected->InjectedChannel == ADC_CHANNEL_VBAT)
        { 
          if (ADC_BATTERY_VOLTAGE_INSTANCE(hadc))
          {
            SET_BIT(tmpADC_Common->CCR, ADC_CCR_VBATEN);
          }
        }
        else if (sConfigInjected->InjectedChannel == ADC_CHANNEL_VREFINT)
        { 
          if (ADC_VREFINT_INSTANCE(hadc))
          {
            SET_BIT(tmpADC_Common->CCR, ADC_CCR_VREFEN);
          }                       
        }       
      }
      /* If the requested internal measurement path has already been enabled  */
      /* and other ADC of the common group are enabled, internal              */
      /* measurement paths cannot be enabled.                                 */
      else  
      {
        /* Update ADC state machine to error */
        SET_BIT(hadc->State, HAL_ADC_STATE_ERROR_CONFIG);
        
        tmp_status = HAL_ERROR;
      }
    }
    
  } /* if (ADC_IS_ENABLE(hadc) == RESET) */
  
  /* Process unlocked */
  __HAL_UNLOCK(hadc);

  /* Return function status */
  return tmp_status;
}




/**
  * @brief  Enable ADC multimode and configure multimode parameters
  * @note   Possibility to update parameters on the fly:
  *         This function initializes multimode parameters, following  
  *         calls to this function can be used to reconfigure some parameters 
  *         of structure "ADC_MultiModeTypeDef" on the fly, without resetting 
  *         the ADCs.
  *         The setting of these parameters is conditioned to ADC state.
  *         For parameters constraints, see comments of structure 
  *         "ADC_MultiModeTypeDef".
  * @note   To move back configuration from multimode to single mode, ADC must
  *         be reset (using function HAL_ADC_Init() ).
  * @param  hadc: Master ADC handle
  * @param  multimode : Structure of ADC multimode configuration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ADCEx_MultiModeConfigChannel(ADC_HandleTypeDef* hadc, ADC_MultiModeTypeDef* multimode)
{
  HAL_StatusTypeDef  tmp_status = HAL_OK;
  ADC_Common_TypeDef *tmpADC_Common;
  ADC_HandleTypeDef  tmphadcSlave;
  
  /* Check the parameters */
  assert_param(IS_ADC_MULTIMODE_MASTER_INSTANCE(hadc->Instance));
  assert_param(IS_ADC_MULTIMODE(multimode->Mode));
  assert_param(IS_ADC_DMA_ACCESS_MULTIMODE(multimode->DMAAccessMode));
  assert_param(IS_ADC_SAMPLING_DELAY(multimode->TwoSamplingDelay));
  
  /* Process locked */
  __HAL_LOCK(hadc);

  ADC_MULTI_SLAVE(hadc, &tmphadcSlave);
  
  /* Parameters update conditioned to ADC state:                              */
  /* Parameters that can be updated when ADC is disabled or enabled without   */
  /* conversion on going on regular group:                                    */
  /*  - Multimode DMA configuration                                           */
  /*  - Multimode DMA mode                                                    */
  if ( (ADC_IS_CONVERSION_ONGOING_REGULAR(hadc) == RESET) 
    && (ADC_IS_CONVERSION_ONGOING_REGULAR(&tmphadcSlave) == RESET) )
  {
    
    /* Pointer to the common control register */
    tmpADC_Common = ADC_COMMON_REGISTER(hadc);
    
    MODIFY_REG(tmpADC_Common->CCR, ADC_CCR_MDMA | ADC_CCR_DMACFG, 
               multimode->DMAAccessMode |
               ADC_CCR_MULTI_DMACONTREQ(hadc->Init.DMAContinuousRequests));
               
    /* Parameters that can be updated only when ADC is disabled:                */
    /*  - Multimode mode selection                                              */
    /*  - Multimode delay                                                       */
    if ((ADC_IS_ENABLE(hadc) == RESET)                             &&
        (ADC_IS_ENABLE(&tmphadcSlave) == RESET)  )
    {
      /* Configuration of ADC common group ADC1&ADC2                            */
      /*  - set the selected multimode                                          */
      /*  - Set delay between two sampling phases                               */
      /*    Note: Delay range depends on selected resolution:                   */
      /*      from 1 to 12 clock cycles for 12 bits                             */
      /*      from 1 to 10 clock cycles for 10 bits,                            */
      /*      from 1 to 8 clock cycles for 8 bits                               */
      /*      from 1 to 6 clock cycles for 6 bits                               */
      /*    If a higher delay is selected, it will be clipped to maximum delay  */
      /*    range                                                               */
  
        MODIFY_REG(tmpADC_Common->CCR, ADC_CCR_DUAL | ADC_CCR_DELAY, 
               multimode->Mode | multimode->TwoSamplingDelay );
    }                              
                            
                            
  }
  /* If one of the ADC sharing the same common group is enabled, no update    */
  /* could be done on neither of the multimode structure parameters.          */
  else
  {
    /* Update ADC state machine to error */
    SET_BIT(hadc->State, HAL_ADC_STATE_ERROR_CONFIG);
    
    tmp_status = HAL_ERROR;
  }
    
    
  /* Process unlocked */
  __HAL_UNLOCK(hadc);
  
  /* Return function status */
  return tmp_status;
} 


/**
  * @brief  Enable Injected Queue
  * @note   This function resets CFGR register JQDIS bit in order to enable the
  *         Injected Queue. JQDIS can be written only when ADSTART and JDSTART
  *         are both equal to 0 to ensure that no regular nor injected 
  *         conversion is ongoing. 
  * @param  hadc: ADC handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ADCEx_EnableInjectedQueue(ADC_HandleTypeDef* hadc)
{

  /* Parameter can be set only if no conversion is on-going                   */
  if (ADC_IS_CONVERSION_ONGOING_REGULAR_INJECTED(hadc) == RESET)
  {
    CLEAR_BIT(hadc->Instance->CFGR, ADC_CFGR_JQDIS);
    
    /* Update state, clear previous result related to injected queue overflow */
    CLEAR_BIT(hadc->State, HAL_ADC_STATE_INJ_JQOVF);
    
    return HAL_OK;
  }
  else
  {
    return HAL_ERROR;
  }

}

/**
  * @brief  Disable Injected Queue
  * @note   This function sets CFGR register JQDIS bit in order to disable the
  *         Injected Queue. JQDIS can be written only when ADSTART and JDSTART
  *         are both equal to 0 to ensure that no regular nor injected 
  *         conversion is ongoing. 
  * @param  hadc: ADC handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ADCEx_DisableInjectedQueue(ADC_HandleTypeDef* hadc)
{

  /* Parameter can be set only if no conversion is on-going                   */
  if (ADC_IS_CONVERSION_ONGOING_REGULAR_INJECTED(hadc) == RESET)
  {
    SET_BIT(hadc->Instance->CFGR, ADC_CFGR_JQDIS);
    return HAL_OK;    
  }
  else
  {
    return HAL_ERROR;
  }

}


/**
  * @brief  Disable ADC voltage regulator.
  * @note   Disabling voltage regulator allows to save power. This operation can
  *         be carried out only when ADC is disabled.
  * @note   To enable again the voltage regulator, the user is expected to 
  *         resort to HAL_ADC_Init() API.           
  * @param  hadc: ADC handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ADCEx_DisableVoltageRegulator(ADC_HandleTypeDef* hadc)
{

  /* ADVREGEN can be written only when the ADC is disabled  */
  if (ADC_IS_ENABLE(hadc) == RESET)
  {
    CLEAR_BIT(hadc->Instance->CR, ADC_CR_ADVREGEN);
    return HAL_OK;    
  }
  else
  {
    return HAL_ERROR;
  }
}

/**
  * @brief  Enter ADC deep-power-down mode
  * @note   This mode is achieved in setting DEEPPWD bit and allows to save power 
  *         in reducing leakage currents. It is particularly interesting before 
  *         entering stop modes.
  * @note   Setting DEEPPWD automatically clears ADVREGEN bit and disables the
  *         ADC voltage regulator. This means that this API encompasses
  *         HAL_ADCEx_DisableVoltageRegulator(). Additionally, the internal
  *         calibration is lost.    
  * @note   To exit the ADC deep-power-down mode, the user is expected to 
  *         resort to HAL_ADC_Init() API as well as to relaunch a calibration
  *         with HAL_ADCEx_Calibration_Start() API or to re-apply a previously
  *         saved calibration factor.
  * @param  hadc: ADC handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ADCEx_EnterADCDeepPowerDownMode(ADC_HandleTypeDef* hadc)
{

  /* DEEPPWD can be written only when the ADC is disabled  */
  if (ADC_IS_ENABLE(hadc) == RESET)
  {
    SET_BIT(hadc->Instance->CR, ADC_CR_DEEPPWD);
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

/**
  * @}
  */
  

  
#endif /* HAL_ADC_MODULE_ENABLED */
/**
  * @}
  */

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
