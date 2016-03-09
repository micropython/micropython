/**
  ******************************************************************************
  * @file    stm32l4xx_ll_lptim.c
  * @author  MCD Application Team
  * @version V1.3.0
  * @date    29-January-2016
  * @brief   LPTIM LL module driver.
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
#if defined(USE_FULL_LL_DRIVER)

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_ll_lptim.h"
#include "stm32l4xx_ll_bus.h"

#ifdef  USE_FULL_ASSERT
  #include "stm32_assert.h"
#else
  #define assert_param(expr) ((void)0)
#endif

/** @addtogroup STM32L4xx_LL_Driver
  * @{
  */

#if defined (LPTIM1) || defined (LPTIM2)

/** @addtogroup LPTIM_LL
  * @{
  */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/** @addtogroup LPTIM_LL_Private_Macros
  * @{
  */
#define IS_LPTIM_CLOCK_SOURCE(__VALUE__) (((__VALUE__) == LL_LPTIM_CLK_SOURCE_INTERNAL) \
                                       || ((__VALUE__) == LL_LPTIM_CLK_SOURCE_EXTERNAL))

#define IS_LPTIM_CLOCK_PRESCALER(__VALUE__) (((__VALUE__) == LL_LPTIM_PRESCALER_DIV1)   \
                                          || ((__VALUE__) == LL_LPTIM_PRESCALER_DIV2)   \
                                          || ((__VALUE__) == LL_LPTIM_PRESCALER_DIV4)   \
                                          || ((__VALUE__) == LL_LPTIM_PRESCALER_DIV8)   \
                                          || ((__VALUE__) == LL_LPTIM_PRESCALER_DIV16)  \
                                          || ((__VALUE__) == LL_LPTIM_PRESCALER_DIV32)  \
                                          || ((__VALUE__) == LL_LPTIM_PRESCALER_DIV64)  \
                                          || ((__VALUE__) == LL_LPTIM_PRESCALER_DIV128))

#define IS_LPTIM_WAVEFORM(__VALUE__) (((__VALUE__) == LL_LPTIM_OUTPUT_WAVEFORM_PWM) \
                                   || ((__VALUE__) == LL_LPTIM_OUTPUT_WAVEFORM_SETONCE))

#define IS_LPTIM_OUTPUT_POLARITY(__VALUE__) (((__VALUE__) == LL_LPTIM_OUTPUT_POLARITY_REGULAR) \
                                          || ((__VALUE__) == LL_LPTIM_OUTPUT_POLARITY_INVERSE))
/**
  * @}
  */


/* Private function prototypes -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/** @addtogroup LPTIM_LL_Exported_Functions
  * @{
  */

/** @addtogroup LPTIM_LL_EF_Init
  * @{
  */

/**
  * @brief  Set LPTIMx registers to their reset values.
  * @param  LPTIMx LP Timer instance
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: LPTIMx registers are de-initialized
  *          - ERROR: invalid LPTIMx instance
  */
ErrorStatus LL_LPTIM_DeInit(LPTIM_TypeDef* LPTIMx)
{
  ErrorStatus result = SUCCESS;

  /* Check the parameters */
  assert_param(IS_LPTIM_INSTANCE(LPTIMx)); 
 
  if (LPTIMx == LPTIM1)
  {
    LL_APB1_GRP1_ForceReset(LL_APB1_GRP1_PERIPH_LPTIM1);
    LL_APB1_GRP1_ReleaseReset(LL_APB1_GRP1_PERIPH_LPTIM1);  
  } 
#if defined(LPTIM2)  
  else if (LPTIMx == LPTIM2)
  { 
    LL_APB1_GRP2_ForceReset(LL_APB1_GRP2_PERIPH_LPTIM2);
    LL_APB1_GRP2_ReleaseReset(LL_APB1_GRP2_PERIPH_LPTIM2);
  }
#endif
  else
  {
    result = ERROR;
  }
  
  return result;
}

/**
  * @brief  Set each fields of the LPTIM_InitStruct structure to its default
  *         value.
  * @param  LPTIM_InitStruct pointer to a @ref LL_LPTIM_InitTypeDef structure
  * @retval None
  */
void LL_LPTIM_StructInit(LL_LPTIM_InitTypeDef* LPTIM_InitStruct)
{
  /* Set the default configuration */
  LPTIM_InitStruct->ClockSource = LL_LPTIM_CLK_SOURCE_INTERNAL;
  LPTIM_InitStruct->Prescaler   = LL_LPTIM_PRESCALER_DIV1;
  LPTIM_InitStruct->Waveform    = LL_LPTIM_OUTPUT_WAVEFORM_PWM;
  LPTIM_InitStruct->Polarity    = LL_LPTIM_OUTPUT_POLARITY_REGULAR;
}

/**
  * @brief  Configure the LPTIMx peripheral according to the specified parameters.
  * @note LL_LPTIM_Init can only be called when the LPTIM instance is disabled.
  * @note LPTIMx can be disabled using unitary function @ref LL_LPTIM_Disable().
  * @param  LPTIMx LP Timer Instance
  * @param  LPTIM_InitStruct pointer to a @ref LL_LPTIM_InitTypeDef structure
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: LPTIMx instance has been initialized
  *          - ERROR: LPTIMx instance hasn't been initialized
  */
ErrorStatus LL_LPTIM_Init(LPTIM_TypeDef * LPTIMx, LL_LPTIM_InitTypeDef* LPTIM_InitStruct)
{
  ErrorStatus result = SUCCESS;
  
  /* The LPTIMx_CFGR register must only be modified when the LPTIM is disabled 
     (ENABLE bit is reset to ‘0’).
  */
  if (LL_LPTIM_IsEnabled(LPTIMx))
  {
    result = ERROR;
  }
  else
  {
  /* Check the parameters */
  assert_param(IS_LPTIM_INSTANCE(LPTIMx)); 
  assert_param(IS_LPTIM_CLOCK_SOURCE(LPTIM_InitStruct->ClockSource));
  assert_param(IS_LPTIM_CLOCK_PRESCALER(LPTIM_InitStruct->Prescaler));
  assert_param(IS_LPTIM_WAVEFORM(LPTIM_InitStruct->Waveform));
  assert_param(IS_LPTIM_OUTPUT_POLARITY(LPTIM_InitStruct->Polarity));
  
  /* Set CKSEL bitfield according to ClockSource value */
  /* Set PRESC bitfield according to Prescaler value */
  /* Set WAVE bitfield according to Waveform value */
  /* Set WAVEPOL bitfield according to Polarity value */
  MODIFY_REG(LPTIMx->CFGR, 
             (LPTIM_CFGR_CKSEL | LPTIM_CFGR_PRESC | LPTIM_CFGR_WAVE| LPTIM_CFGR_WAVPOL), 
             LPTIM_InitStruct->ClockSource | \
             LPTIM_InitStruct->Prescaler | \
             LPTIM_InitStruct->Waveform | \
             LPTIM_InitStruct->Polarity);
  }

  return result;
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

#endif /* defined (LPTIM1) || defined (LPTIM2) */

/**
  * @}
  */
  
#endif /* USE_FULL_LL_DRIVER */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
