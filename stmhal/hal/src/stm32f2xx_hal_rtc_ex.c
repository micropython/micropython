/**
  ******************************************************************************
  * @file    stm32f2xx_hal_rtc_ex.c
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    25-March-2014
  * @brief   RTC HAL module driver.
  *          This file provides firmware functions to manage the following 
  *          functionalities of the Real Time Clock (RTC) Extension peripheral:
  *           + RTC Time Stamp functions
  *           + RTC Tamper functions 
  *           + RTC Wake-up functions
  *           + Extension Control functions
  *           + Extension RTC features functions    
  *         
  @verbatim
  ==============================================================================
                  ##### How to use this driver #####
  ==============================================================================
  [..] 
    (+) Enable the RTC domain access.
    (+) Configure the RTC Prescaler (Asynchronous and Synchronous) and RTC hour 
        format using the HAL_RTC_Init() function.
  
  *** RTC Wakeup configuration ***
  ================================
  [..] 
    (+) To configure the RTC Wakeup Clock source and Counter use the HAL_RTC_SetWakeUpTimer()
        function. You can also configure the RTC Wakeup timer with interrupt mode 
        using the HAL_RTC_SetWakeUpTimer_IT() function.
    (+) To read the RTC WakeUp Counter register, use the HAL_RTC_GetWakeUpTimer() 
        function.
  
  *** TimeStamp configuration ***
  ===============================
  [..]
    (+) Configure the RTC_AFx trigger and enables the RTC TimeStamp using the 
        HAL_RTC_SetTimeStamp() function. You can also configure the RTC TimeStamp with 
        interrupt mode using the HAL_RTC_SetTimeStamp_IT() function.
    (+) To read the RTC TimeStamp Time and Date register, use the HAL_RTC_GetTimeStamp()
        function.
    (+) The TIMESTAMP alternate function can be mapped either to RTC_AF1 (PC13)
        or RTC_AF2 (PI8) depending on the value of TSINSEL bit in 
        RTC_TAFCR register. The corresponding pin is also selected by HAL_RTC_SetTimeStamp()
        or HAL_RTC_SetTimeStamp_IT() function.
  
  *** Tamper configuration ***
  ============================
  [..]
    (+) Enable the RTC Tamper and Configure the Tamper filter count, trigger Edge 
        or Level according to the Tamper filter (if equal to 0 Edge else Level) 
        value, sampling frequency, precharge or discharge and Pull-UP using the 
        HAL_RTC_SetTamper() function. You can configure RTC Tamper with interrupt 
        mode using HAL_RTC_SetTamper_IT() function.
    (+) The TAMPER1 alternate function can be mapped either to RTC_AF1 (PC13)
        or RTC_AF2 (PI8) depending on the value of TAMP1INSEL bit in 
        RTC_TAFCR register. The corresponding pin is also selected by HAL_RTC_SetTamper()
        or HAL_RTC_SetTamper_IT() function.
  
  *** Backup Data Registers configuration ***
  ===========================================
  [..]
    (+) To write to the RTC Backup Data registers, use the HAL_RTC_BKUPWrite()
        function.  
    (+) To read the RTC Backup Data registers, use the HAL_RTC_BKUPRead()
        function.
     
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
#include "stm32f2xx_hal.h"

/** @addtogroup STM32F2xx_HAL_Driver
  * @{
  */

/** @defgroup RTCEx 
  * @brief RTC HAL module driver
  * @{
  */

#ifdef HAL_RTC_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup RTCEx_Private_Functions
  * @{
  */
  

/** @defgroup RTCEx_Group1 RTC TimeStamp and Tamper functions
 *  @brief   RTC TimeStamp and Tamper functions
 *
@verbatim   
 ===============================================================================
                 ##### RTC TimeStamp and Tamper functions #####
 ===============================================================================  
 
 [..] This section provide functions allowing to configure TimeStamp feature

@endverbatim
  * @{
  */

/**
  * @brief  Sets TimeStamp.
  * @note   This API must be called before enabling the TimeStamp feature. 
  * @param  hrtc: RTC handle
  * @param  TimeStampEdge: Specifies the pin edge on which the TimeStamp is 
  *         activated.
  *          This parameter can be one of the following:
  *             @arg TimeStampEdge_Rising: the Time stamp event occurs on the  
  *                                        rising edge of the related pin.
  *             @arg TimeStampEdge_Falling: the Time stamp event occurs on the 
  *                                         falling edge of the related pin.
  * @param  RTC_TimeStampPin: specifies the RTC TimeStamp Pin.
  *          This parameter can be one of the following values:
  *             @arg RTC_TIMESTAMPPIN_PC13: PC13 is selected as RTC TimeStamp Pin.
  *             @arg RTC_TIMESTAMPPIN_PI8: PI8 is selected as RTC TimeStamp Pin.  
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTCEx_SetTimeStamp(RTC_HandleTypeDef *hrtc, uint32_t TimeStampEdge, uint32_t RTC_TimeStampPin)
{
  uint32_t tmpreg = 0;
  
  /* Check the parameters */
  assert_param(IS_TIMESTAMP_EDGE(TimeStampEdge));
  assert_param(IS_RTC_TIMESTAMP_PIN(RTC_TimeStampPin));
  
  /* Process Locked */ 
  __HAL_LOCK(hrtc);
  
  hrtc->State = HAL_RTC_STATE_BUSY;
  
  /* Get the RTC_CR register and clear the bits to be configured */
  tmpreg = (uint32_t)(hrtc->Instance->CR & (uint32_t)~(RTC_CR_TSEDGE | RTC_CR_TSE));
    
  tmpreg|= TimeStampEdge;
  
  /* Disable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_DISABLE(hrtc);
  
  hrtc->Instance->TAFCR &= (uint32_t)~RTC_TAFCR_TSINSEL;
  hrtc->Instance->TAFCR |= (uint32_t)(RTC_TimeStampPin); 
  
  /* Configure the Time Stamp TSEDGE and Enable bits */
  hrtc->Instance->CR = (uint32_t)tmpreg;
  
  __HAL_RTC_TIMESTAMP_ENABLE(hrtc);
  
  /* Enable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);    
  
  /* Change RTC state */
  hrtc->State = HAL_RTC_STATE_READY; 
  
  /* Process Unlocked */ 
  __HAL_UNLOCK(hrtc);
  
  return HAL_OK;
}

/**
  * @brief  Sets TimeStamp with Interrupt. 
  * @param  hrtc: RTC handle
  * @note   This API must be called before enabling the TimeStamp feature.
  * @param  TimeStampEdge: Specifies the pin edge on which the TimeStamp is 
  *         activated.
  *          This parameter can be one of the following:
  *             @arg TimeStampEdge_Rising: the Time stamp event occurs on the  
  *                                        rising edge of the related pin.
  *             @arg TimeStampEdge_Falling: the Time stamp event occurs on the 
  *                                         falling edge of the related pin.
  * @param  RTC_TimeStampPin: Specifies the RTC TimeStamp Pin.
  *          This parameter can be one of the following values:
  *             @arg RTC_TIMESTAMPPIN_PC13: PC13 is selected as RTC TimeStamp Pin.
  *             @arg RTC_TIMESTAMPPIN_PI8: PI8 is selected as RTC TimeStamp Pin.   
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTCEx_SetTimeStamp_IT(RTC_HandleTypeDef *hrtc, uint32_t TimeStampEdge, uint32_t RTC_TimeStampPin)
{
  uint32_t tmpreg = 0;
  
  /* Check the parameters */
  assert_param(IS_TIMESTAMP_EDGE(TimeStampEdge));
  assert_param(IS_RTC_TIMESTAMP_PIN(RTC_TimeStampPin));
  
  /* Process Locked */ 
  __HAL_LOCK(hrtc);
  
  hrtc->State = HAL_RTC_STATE_BUSY;
  
  /* Get the RTC_CR register and clear the bits to be configured */
  tmpreg = (uint32_t)(hrtc->Instance->CR & (uint32_t)~(RTC_CR_TSEDGE | RTC_CR_TSE));
  
  tmpreg |= TimeStampEdge;
  
  /* Disable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_DISABLE(hrtc);
  
  /* Configure the Time Stamp TSEDGE and Enable bits */
  hrtc->Instance->CR = (uint32_t)tmpreg;
  
  hrtc->Instance->TAFCR &= (uint32_t)~RTC_TAFCR_TSINSEL;
  hrtc->Instance->TAFCR |= (uint32_t)(RTC_TimeStampPin); 
  
  __HAL_RTC_TIMESTAMP_ENABLE(hrtc);
  
  /* Enable IT timestamp */ 
  __HAL_RTC_TIMESTAMP_ENABLE_IT(hrtc,RTC_IT_TS);
  
  /* RTC timestamp Interrupt Configuration: EXTI configuration */
  __HAL_RTC_ENABLE_IT(RTC_EXTI_LINE_TAMPER_TIMESTAMP_EVENT);
  
  EXTI->RTSR |= RTC_EXTI_LINE_TAMPER_TIMESTAMP_EVENT;
  
  /* Enable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);  
  
  hrtc->State = HAL_RTC_STATE_READY;  
  
  /* Process Unlocked */ 
  __HAL_UNLOCK(hrtc);
  
  return HAL_OK;
}

/**
  * @brief  Deactivates TimeStamp. 
  * @param  hrtc: RTC handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTCEx_DeactivateTimeStamp(RTC_HandleTypeDef *hrtc)
{
  uint32_t tmpreg = 0;
  
  /* Process Locked */ 
  __HAL_LOCK(hrtc);
  
  hrtc->State = HAL_RTC_STATE_BUSY;
  
  /* Disable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_DISABLE(hrtc);
  
  /* In case of interrupt mode is used, the interrupt source must disabled */ 
  __HAL_RTC_TIMESTAMP_DISABLE_IT(hrtc, RTC_IT_TS);
  
  /* Get the RTC_CR register and clear the bits to be configured */
  tmpreg = (uint32_t)(hrtc->Instance->CR & (uint32_t)~(RTC_CR_TSEDGE | RTC_CR_TSE));
  
  /* Configure the Time Stamp TSEDGE and Enable bits */
  hrtc->Instance->CR = (uint32_t)tmpreg;
  
  /* Enable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);
 
  hrtc->State = HAL_RTC_STATE_READY;  
  
  /* Process Unlocked */ 
  __HAL_UNLOCK(hrtc);
  
  return HAL_OK;
}

/**
  * @brief  Gets the RTC TimeStamp value.
  * @param  hrtc: RTC handle
  * @param  sTimeStamp: Pointer to Time structure
  * @param  sTimeStampDate: Pointer to Date structure  
  * @param  Format: specifies the format of the entered parameters.
  *          This parameter can be one of the following values:
  *             @arg Format_BIN: Binary data format 
  *             @arg Format_BCD: BCD data format
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTCEx_GetTimeStamp(RTC_HandleTypeDef *hrtc, RTC_TimeTypeDef* sTimeStamp, RTC_DateTypeDef* sTimeStampDate, uint32_t Format)
{
  uint32_t tmptime = 0, tmpdate = 0;

  /* Check the parameters */
  assert_param(IS_RTC_FORMAT(Format));

  /* Get the TimeStamp time and date registers values */
  tmptime = (uint32_t)(hrtc->Instance->TSTR & RTC_TR_RESERVED_MASK);
  tmpdate = (uint32_t)(hrtc->Instance->TSDR & RTC_DR_RESERVED_MASK);

  /* Fill the Time structure fields with the read parameters */
  sTimeStamp->Hours = (uint8_t)((tmptime & (RTC_TR_HT | RTC_TR_HU)) >> 16);
  sTimeStamp->Minutes = (uint8_t)((tmptime & (RTC_TR_MNT | RTC_TR_MNU)) >> 8);
  sTimeStamp->Seconds = (uint8_t)(tmptime & (RTC_TR_ST | RTC_TR_SU));
  sTimeStamp->TimeFormat = (uint8_t)((tmptime & (RTC_TR_PM)) >> 16);  
  
  /* Fill the Date structure fields with the read parameters */
  sTimeStampDate->Year = 0;
  sTimeStampDate->Month = (uint8_t)((tmpdate & (RTC_DR_MT | RTC_DR_MU)) >> 8);
  sTimeStampDate->Date = (uint8_t)(tmpdate & (RTC_DR_DT | RTC_DR_DU));
  sTimeStampDate->WeekDay = (uint8_t)((tmpdate & (RTC_DR_WDU)) >> 13);

  /* Check the input parameters format */
  if(Format == FORMAT_BIN)
  {
    /* Convert the TimeStamp structure parameters to Binary format */
    sTimeStamp->Hours = (uint8_t)RTC_Bcd2ToByte(sTimeStamp->Hours);
    sTimeStamp->Minutes = (uint8_t)RTC_Bcd2ToByte(sTimeStamp->Minutes);
    sTimeStamp->Seconds = (uint8_t)RTC_Bcd2ToByte(sTimeStamp->Seconds);
    
    /* Convert the DateTimeStamp structure parameters to Binary format */
    sTimeStampDate->Month = (uint8_t)RTC_Bcd2ToByte(sTimeStampDate->Month);
    sTimeStampDate->Date = (uint8_t)RTC_Bcd2ToByte(sTimeStampDate->Date);
    sTimeStampDate->WeekDay = (uint8_t)RTC_Bcd2ToByte(sTimeStampDate->WeekDay);
  }
  
  /* Clear the TIMESTAMP Flag */
  __HAL_RTC_TIMESTAMP_CLEAR_FLAG(hrtc, RTC_FLAG_TSF);
    
  return HAL_OK;
}

/**
  * @brief  Sets Tamper
  * @note   By calling this API we disable the tamper interrupt for all tampers. 
  * @param  hrtc: RTC handle
  * @param  sTamper: Pointer to Tamper Structure.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTCEx_SetTamper(RTC_HandleTypeDef *hrtc, RTC_TamperTypeDef* sTamper)
{
  uint32_t tmpreg = 0;
  
  /* Check the parameters */
  assert_param(IS_TAMPER(sTamper->Tamper)); 
  assert_param(IS_RTC_TAMPER_PIN(sTamper->PinSelection));
  assert_param(IS_TAMPER_TRIGGER(sTamper->Trigger));

  /* Process Locked */ 
  __HAL_LOCK(hrtc);
    
  hrtc->State = HAL_RTC_STATE_BUSY;

  tmpreg = ((uint32_t)sTamper->Tamper | (uint32_t)sTamper->PinSelection | (uint32_t)(sTamper->Trigger));

  hrtc->Instance->TAFCR &= (uint32_t)~((uint32_t)RTC_TAFCR_TAMP1E | (uint32_t)RTC_TAFCR_TAMP1TRG | (uint32_t)RTC_TAFCR_TAMPINSEL);

  hrtc->Instance->TAFCR |= tmpreg;
  
  hrtc->State = HAL_RTC_STATE_READY; 

  /* Process Unlocked */ 
  __HAL_UNLOCK(hrtc);
    
  return HAL_OK;
}

/**
  * @brief  Sets Tamper with interrupt.
  * @note   By calling this API we force the tamper interrupt for all tampers.
  * @param  hrtc: RTC handle
  * @param  sTamper: Pointer to RTC Tamper.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTCEx_SetTamper_IT(RTC_HandleTypeDef *hrtc, RTC_TamperTypeDef* sTamper)
{
  uint32_t tmpreg = 0;
  
  /* Check the parameters */
  assert_param(IS_TAMPER(sTamper->Tamper)); 
  assert_param(IS_RTC_TAMPER_PIN(sTamper->PinSelection));
  assert_param(IS_TAMPER_TRIGGER(sTamper->Trigger));

  /* Process Locked */ 
  __HAL_LOCK(hrtc);
      
  hrtc->State = HAL_RTC_STATE_BUSY;

  tmpreg = ((uint32_t)sTamper->Tamper | (uint32_t)sTamper->PinSelection | (uint32_t)sTamper->Trigger);

  hrtc->Instance->TAFCR &= (uint32_t)~((uint32_t)RTC_TAFCR_TAMP1E | (uint32_t)RTC_TAFCR_TAMP1TRG | (uint32_t)RTC_TAFCR_TAMPINSEL | (uint32_t)RTC_TAFCR_TAMPIE);

  hrtc->Instance->TAFCR |= tmpreg;
  
  /* Configure the Tamper Interrupt in the RTC_TAFCR */
  hrtc->Instance->TAFCR |= (uint32_t)RTC_TAFCR_TAMPIE;
  
  /* RTC Tamper Interrupt Configuration: EXTI configuration */
  __HAL_RTC_ENABLE_IT(RTC_EXTI_LINE_TAMPER_TIMESTAMP_EVENT);
  
  EXTI->RTSR |= RTC_EXTI_LINE_TAMPER_TIMESTAMP_EVENT;
  
  hrtc->State = HAL_RTC_STATE_READY;   
  
  /* Process Unlocked */ 
  __HAL_UNLOCK(hrtc);
  
  return HAL_OK;
}

/**
  * @brief  Deactivates Tamper.
  * @param  hrtc: RTC handle
  * @param  Tamper: Selected tamper pin.
  *          This parameter can be RTC_Tamper_1 and/or RTC_TAMPER_2.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTCEx_DeactivateTamper(RTC_HandleTypeDef *hrtc, uint32_t Tamper)
{
  assert_param(IS_TAMPER(Tamper)); 
  
  /* Process Locked */ 
  __HAL_LOCK(hrtc);
      
  hrtc->State = HAL_RTC_STATE_BUSY;
  
  /* Disable the selected Tamper pin */
  hrtc->Instance->TAFCR &= (uint32_t)~Tamper;  
  
  hrtc->State = HAL_RTC_STATE_READY;   
  
  /* Process Unlocked */  
  __HAL_UNLOCK(hrtc);
  
  return HAL_OK; 
}

/**
  * @brief  This function handles TimeStamp interrupt request.
  * @param  hrtc: RTC handle
  * @retval None
  */
void HAL_RTCEx_TamperTimeStampIRQHandler(RTC_HandleTypeDef *hrtc)
{  
  if(__HAL_RTC_TIMESTAMP_GET_IT(hrtc, RTC_IT_TS))
  {
    /* Get the status of the Interrupt */
    if((uint32_t)(hrtc->Instance->CR & RTC_IT_TS) != (uint32_t)RESET)
    {
      /* TIMESTAMP callback */ 
      HAL_RTCEx_TimeStampEventCallback(hrtc);
  
      /* Clear the TIMESTAMP interrupt pending bit */
      __HAL_RTC_TIMESTAMP_CLEAR_FLAG(hrtc,RTC_FLAG_TSF);
    }
  }
  
  /* Get the status of the Interrupt */
  if(__HAL_RTC_TAMPER_GET_IT(hrtc,RTC_IT_TAMP1))
  {
    /* Get the TAMPER Interrupt enable bit and pending bit */
    if(((hrtc->Instance->TAFCR & (RTC_TAFCR_TAMPIE))) != (uint32_t)RESET) 
    {
      /* Tamper callback */ 
      HAL_RTCEx_Tamper1EventCallback(hrtc);
  
      /* Clear the Tamper interrupt pending bit */
      __HAL_RTC_TAMPER_CLEAR_FLAG(hrtc,RTC_FLAG_TAMP1F);
    }
  }
  
  /* Clear the EXTI's Flag for RTC TimeStamp and Tamper */
  __HAL_RTC_CLEAR_FLAG(RTC_EXTI_LINE_TAMPER_TIMESTAMP_EVENT);
  
  /* Change RTC state */
  hrtc->State = HAL_RTC_STATE_READY; 
}

/**
  * @brief  TimeStamp callback. 
  * @param  hrtc: RTC handle
  * @retval None
  */
__weak void HAL_RTCEx_TimeStampEventCallback(RTC_HandleTypeDef *hrtc)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_RTC_TimeStampEventCallback could be implemented in the user file
  */
}

/**
  * @brief  Tamper 1 callback. 
  * @param  hrtc: RTC handle
  * @retval None
  */
__weak void HAL_RTCEx_Tamper1EventCallback(RTC_HandleTypeDef *hrtc)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_RTC_Tamper1EventCallback could be implemented in the user file
   */
}

/**
  * @brief  This function handles TimeStamp polling request.
  * @param  hrtc: RTC handle
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTCEx_PollForTimeStampEvent(RTC_HandleTypeDef *hrtc, uint32_t Timeout)
{ 
  uint32_t timeout = 0; 

  /* Get Timeout value */
  timeout = HAL_GetTick() + Timeout;   

  while(__HAL_RTC_TIMESTAMP_GET_FLAG(hrtc, RTC_FLAG_TSF) == RESET)
  {
    if(__HAL_RTC_TIMESTAMP_GET_FLAG(hrtc, RTC_FLAG_TSOVF) != RESET)
    {
      /* Clear the TIMESTAMP OverRun Flag */
      __HAL_RTC_TIMESTAMP_CLEAR_FLAG(hrtc, RTC_FLAG_TSOVF);
      
      /* Change TIMESTAMP state */
      hrtc->State = HAL_RTC_STATE_ERROR; 
      
      return HAL_ERROR; 
    }
    
    if(Timeout != HAL_MAX_DELAY)
    {
      if(HAL_GetTick() >= timeout)
      {
        hrtc->State = HAL_RTC_STATE_TIMEOUT;
        return HAL_TIMEOUT;
      }
    }
  }
  
  /* Change RTC state */
  hrtc->State = HAL_RTC_STATE_READY; 
  
  return HAL_OK; 
}
  
/**
  * @brief  This function handles Tamper1 Polling.
  * @param  hrtc: RTC handle
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTCEx_PollForTamper1Event(RTC_HandleTypeDef *hrtc, uint32_t Timeout)
{  
  uint32_t timeout = 0; 

  /* Get Timeout value */
  timeout = HAL_GetTick() + Timeout;
  
  /* Get the status of the Interrupt */
  while(__HAL_RTC_TAMPER_GET_FLAG(hrtc, RTC_FLAG_TAMP1F)== RESET)
  {
    if(Timeout != HAL_MAX_DELAY)
    {
      if(HAL_GetTick() >= timeout)
      {
        hrtc->State = HAL_RTC_STATE_TIMEOUT;
        return HAL_TIMEOUT;
      }
    }
  }
  
  /* Clear the Tamper Flag */
  __HAL_RTC_TAMPER_CLEAR_FLAG(hrtc,RTC_FLAG_TAMP1F);
  
  /* Change RTC state */
  hrtc->State = HAL_RTC_STATE_READY;
  
  return HAL_OK; 
}

/**
  * @}
  */
  
/** @defgroup RTCEx_Group2 RTC Wake-up functions
 *  @brief   RTC Wake-up functions
 *
@verbatim   
 ===============================================================================
                        ##### RTC Wake-up functions #####
 ===============================================================================  
 
 [..] This section provide functions allowing to configure Wake-up feature

@endverbatim
  * @{
  */

/**
  * @brief  Sets wake up timer. 
  * @param  hrtc: RTC handle
  * @param  WakeUpCounter: Wake up counter
  * @param  WakeUpClock: Wake up clock  
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTCEx_SetWakeUpTimer(RTC_HandleTypeDef *hrtc, uint32_t WakeUpCounter, uint32_t WakeUpClock)
{
  uint32_t timeout = 0;

  /* Check the parameters */
  assert_param(IS_WAKEUP_CLOCK(WakeUpClock));
  assert_param(IS_WAKEUP_COUNTER(WakeUpCounter));
 
  /* Process Locked */ 
  __HAL_LOCK(hrtc);
    
  hrtc->State = HAL_RTC_STATE_BUSY;
  
  /* Disable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_DISABLE(hrtc);
  
  __HAL_RTC_WAKEUPTIMER_DISABLE(hrtc);
     
  timeout = HAL_GetTick() + RTC_TIMEOUT_VALUE;

  /* Wait till RTC WUTWF flag is set and if Time out is reached exit */
  while(__HAL_RTC_WAKEUPTIMER_GET_FLAG(hrtc, RTC_FLAG_WUTWF) == RESET)
  {
    if(HAL_GetTick() >= timeout)
    {
      /* Enable the write protection for RTC registers */
      __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);
      
      hrtc->State = HAL_RTC_STATE_TIMEOUT; 
      
      /* Process Unlocked */ 
      __HAL_UNLOCK(hrtc);
      
      return HAL_TIMEOUT;
    }  
  }
  
  /* Clear the Wakeup Timer clock source bits in CR register */
  hrtc->Instance->CR &= (uint32_t)~RTC_CR_WUCKSEL;
  
  /* Configure the clock source */
  hrtc->Instance->CR |= (uint32_t)WakeUpClock;
  
  /* Configure the Wakeup Timer counter */
  hrtc->Instance->WUTR = (uint32_t)WakeUpCounter;
  
   /* Enable the Wakeup Timer */
  __HAL_RTC_WAKEUPTIMER_ENABLE(hrtc);   
  
  /* Enable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc); 
  
  hrtc->State = HAL_RTC_STATE_READY;   
  
  /* Process Unlocked */ 
  __HAL_UNLOCK(hrtc);
  
  return HAL_OK;
}

/**
  * @brief  Sets wake up timer with interrupt
  * @param  hrtc: RTC handle
  * @param  WakeUpCounter: wake up counter
  * @param  WakeUpClock: wake up clock  
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTCEx_SetWakeUpTimer_IT(RTC_HandleTypeDef *hrtc, uint32_t WakeUpCounter, uint32_t WakeUpClock)
{
  uint32_t timeout = 0;
  
  /* Check the parameters */
  assert_param(IS_WAKEUP_CLOCK(WakeUpClock));
  assert_param(IS_WAKEUP_COUNTER(WakeUpCounter));
  
  /* Process Locked */ 
  __HAL_LOCK(hrtc);
  
  hrtc->State = HAL_RTC_STATE_BUSY;
  
  /* Disable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_DISABLE(hrtc);
  
  __HAL_RTC_WAKEUPTIMER_DISABLE(hrtc);
       
  timeout = HAL_GetTick() + RTC_TIMEOUT_VALUE;
  
  /* Wait till RTC WUTWF flag is set and if Time out is reached exit */
  while(__HAL_RTC_WAKEUPTIMER_GET_FLAG(hrtc, RTC_FLAG_WUTWF) == RESET)
  {
    if(HAL_GetTick() >= timeout)
    {
      /* Enable the write protection for RTC registers */
      __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);
      
      hrtc->State = HAL_RTC_STATE_TIMEOUT; 
      
      /* Process Unlocked */ 
      __HAL_UNLOCK(hrtc);
      
      return HAL_TIMEOUT;
    }  
  }
  
  /* Configure the Wakeup Timer counter */
  hrtc->Instance->WUTR = (uint32_t)WakeUpCounter;

  /* Clear the Wakeup Timer clock source bits in CR register */
  hrtc->Instance->CR &= (uint32_t)~RTC_CR_WUCKSEL;

  /* Configure the clock source */
  hrtc->Instance->CR |= (uint32_t)WakeUpClock;
  
  /* RTC WakeUpTimer Interrupt Configuration: EXTI configuration */
  __HAL_RTC_ENABLE_IT(RTC_EXTI_LINE_WAKEUPTIMER_EVENT);
  
  EXTI->RTSR |= RTC_EXTI_LINE_WAKEUPTIMER_EVENT;
  
  /* Configure the Interrupt in the RTC_CR register */
  __HAL_RTC_WAKEUPTIMER_ENABLE_IT(hrtc,RTC_IT_WUT);
  
  /* Enable the Wakeup Timer */
  __HAL_RTC_WAKEUPTIMER_ENABLE(hrtc);
    
  /* Enable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc); 
  
  hrtc->State = HAL_RTC_STATE_READY;   
  
  /* Process Unlocked */ 
  __HAL_UNLOCK(hrtc);
 
  return HAL_OK;
}

/**
  * @brief  Deactivates wake up timer counter.
  * @param  hrtc: RTC handle 
  * @retval HAL status
  */
uint32_t HAL_RTCEx_DeactivateWakeUpTimer(RTC_HandleTypeDef *hrtc)
{
  uint32_t timeout = 0;
  
  /* Process Locked */ 
  __HAL_LOCK(hrtc);
  
  hrtc->State = HAL_RTC_STATE_BUSY;
  
  /* Disable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_DISABLE(hrtc);
  
  /* Disable the Wakeup Timer */
  __HAL_RTC_WAKEUPTIMER_DISABLE(hrtc);
  
  /* In case of interrupt mode is used, the interrupt source must disabled */ 
  __HAL_RTC_WAKEUPTIMER_DISABLE_IT(hrtc,RTC_IT_WUT);
      
  timeout = HAL_GetTick() + RTC_TIMEOUT_VALUE;
  /* Wait till RTC WUTWF flag is set and if Time out is reached exit */
  while(__HAL_RTC_WAKEUPTIMER_GET_FLAG(hrtc, RTC_FLAG_WUTWF) == RESET)
  {
    if(HAL_GetTick() >= timeout)
    {
      /* Enable the write protection for RTC registers */
      __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);
      
      hrtc->State = HAL_RTC_STATE_TIMEOUT; 
      
      /* Process Unlocked */ 
      __HAL_UNLOCK(hrtc);
      
      return HAL_TIMEOUT;
    }   
  }
  
  /* Enable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);
  
  hrtc->State = HAL_RTC_STATE_READY;   
  
  /* Process Unlocked */ 
  __HAL_UNLOCK(hrtc);
  
  return HAL_OK;
}

/**
  * @brief  Gets wake up timer counter.
  * @param  hrtc: RTC handle 
  * @retval Counter value
  */
uint32_t HAL_RTCEx_GetWakeUpTimer(RTC_HandleTypeDef *hrtc)
{
  /* Get the counter value */
  return ((uint32_t)(hrtc->Instance->WUTR & RTC_WUTR_WUT)); 
}

/**
  * @brief  This function handles Wake Up Timer interrupt request.
  * @param  hrtc: RTC handle
  * @retval None
  */
void HAL_RTCEx_WakeUpTimerIRQHandler(RTC_HandleTypeDef *hrtc)
{  
  if(__HAL_RTC_WAKEUPTIMER_GET_IT(hrtc, RTC_IT_WUT))
  {
    /* Get the status of the Interrupt */
    if((uint32_t)(hrtc->Instance->CR & RTC_IT_WUT) != (uint32_t)RESET)
    {
      /* WAKEUPTIMER callback */ 
      HAL_RTCEx_WakeUpTimerEventCallback(hrtc);
      
      /* Clear the WAKEUPTIMER interrupt pending bit */
      __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(hrtc, RTC_FLAG_WUTF);
    }
  }
  
  /* Clear the EXTI's line Flag for RTC WakeUpTimer */
  __HAL_RTC_CLEAR_FLAG(RTC_EXTI_LINE_WAKEUPTIMER_EVENT);
  
  /* Change RTC state */
  hrtc->State = HAL_RTC_STATE_READY; 
}

/**
  * @brief  Wake Up Timer callback.
  * @param  hrtc: RTC handle
  * @retval None
  */
__weak void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_RTC_WakeUpTimerEventCallback could be implemented in the user file
   */
}

/**
  * @brief  This function handles Wake Up Timer Polling.
  * @param  hrtc: RTC handle
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTCEx_PollForWakeUpTimerEvent(RTC_HandleTypeDef *hrtc, uint32_t Timeout)
{  
  uint32_t timeout = 0; 

  /* Get Timeout value */
  timeout = HAL_GetTick() + Timeout;
  
  while(__HAL_RTC_WAKEUPTIMER_GET_FLAG(hrtc, RTC_FLAG_WUTF) == RESET)
  {
    if(Timeout != HAL_MAX_DELAY)
    {
      if(HAL_GetTick() >= timeout)
      {
        hrtc->State = HAL_RTC_STATE_TIMEOUT;
      
        return HAL_TIMEOUT;
      }
    }
  }
  
  /* Clear the WAKEUPTIMER Flag */
  __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(hrtc, RTC_FLAG_WUTF);
  
  /* Change RTC state */
  hrtc->State = HAL_RTC_STATE_READY;
  
  return HAL_OK; 
}

/**
  * @}
  */


/** @defgroup RTCEx_Group3 Extension Peripheral Control functions 
 *  @brief   Extension Peripheral Control functions 
 *
@verbatim   
 ===============================================================================
              ##### Extension Peripheral Control functions #####
 ===============================================================================  
    [..]
    This subsection provides functions allowing to
      (+) Writes a data in a specified RTC Backup data register
      (+) Read a data in a specified RTC Backup data register
      (+) Sets the Coarse calibration parameters.
      (+) Deactivates the Coarse calibration parameters
      (+) Sets the Smooth calibration parameters.
      (+) Configures the Synchronization Shift Control Settings.
      (+) Configures the Calibration Pinout (RTC_CALIB) Selection (1Hz or 512Hz).
      (+) Deactivates the Calibration Pinout (RTC_CALIB) Selection (1Hz or 512Hz).
      (+) Enables the RTC reference clock detection.
      (+) Disable the RTC reference clock detection.
      (+) Enables the Bypass Shadow feature.
      (+) Disables the Bypass Shadow feature.

@endverbatim
  * @{
  */

/**
  * @brief  Writes a data in a specified RTC Backup data register.
  * @param  hrtc: RTC handle 
  * @param  BackupRegister: RTC Backup data Register number.
  *          This parameter can be: RTC_BKP_DRx where x can be from 0 to 19 to 
  *                                 specify the register.
  * @param  Data: Data to be written in the specified RTC Backup data register.                     
  * @retval None
  */
void HAL_RTCEx_BKUPWrite(RTC_HandleTypeDef *hrtc, uint32_t BackupRegister, uint32_t Data)
{
  uint32_t tmp = 0;
  
  /* Check the parameters */
  assert_param(IS_RTC_BKP(BackupRegister));
  
  tmp = (uint32_t)&(hrtc->Instance->BKP0R);
  tmp += (BackupRegister * 4);
  
  /* Write the specified register */
  *(__IO uint32_t *)tmp = (uint32_t)Data;
}

/**
  * @brief  Reads data from the specified RTC Backup data Register.
  * @param  hrtc: RTC handle 
  * @param  BackupRegister: RTC Backup data Register number.
  *          This parameter can be: RTC_BKP_DRx where x can be from 0 to 19 to 
  *                                 specify the register.                   
  * @retval Read value
  */
uint32_t HAL_RTCEx_BKUPRead(RTC_HandleTypeDef *hrtc, uint32_t BackupRegister)
{
  uint32_t tmp = 0;
  
  /* Check the parameters */
  assert_param(IS_RTC_BKP(BackupRegister));

  tmp = (uint32_t)&(hrtc->Instance->BKP0R);
  tmp += (BackupRegister * 4);
  
  /* Read the specified register */
  return (*(__IO uint32_t *)tmp);
}
      
/**
  * @brief  Sets the Coarse calibration parameters.
  * @param  hrtc: RTC handle  
  * @param  CalibSign: Specifies the sign of the coarse calibration value.
  *          This parameter can be  one of the following values :
  *             @arg RTC_CALIBSIGN_POSITIVE: The value sign is positive 
  *             @arg RTC_CALIBSIGN_NEGATIVE: The value sign is negative
  * @param  Value: value of coarse calibration expressed in ppm (coded on 5 bits).
  *    
  * @note   This Calibration value should be between 0 and 63 when using negative
  *         sign with a 2-ppm step.
  *           
  * @note   This Calibration value should be between 0 and 126 when using positive
  *         sign with a 4-ppm step.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTCEx_SetCoarseCalib(RTC_HandleTypeDef* hrtc, uint32_t CalibSign, uint32_t Value)
{
  /* Check the parameters */
  assert_param(IS_RTC_CALIB_SIGN(CalibSign));
  assert_param(IS_RTC_CALIB_VALUE(Value)); 
  
  /* Process Locked */ 
  __HAL_LOCK(hrtc);
  
  hrtc->State = HAL_RTC_STATE_BUSY;

  /* Disable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_DISABLE(hrtc);

  /* Set Initialization mode */
  if(RTC_EnterInitMode(hrtc) != HAL_OK)
  {
    /* Enable the write protection for RTC registers */
    __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc); 
    
    /* Set RTC state*/
    hrtc->State = HAL_RTC_STATE_ERROR;
    
    /* Process Unlocked */ 
    __HAL_UNLOCK(hrtc);
    
    return HAL_ERROR;
  } 
  else
  { 
    /* Enable the Coarse Calibration */
    __HAL_RTC_COARSE_CALIB_ENABLE(hrtc);
    
    /* Set the coarse calibration value */
    hrtc->Instance->CALIBR = (uint32_t)(CalibSign|Value);
    
    /* Exit Initialization mode */
    hrtc->Instance->ISR &= (uint32_t)~RTC_ISR_INIT; 
  } 

  /* Enable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);
  
  /* Change state */
  hrtc->State = HAL_RTC_STATE_READY; 
  
  /* Process Unlocked */ 
  __HAL_UNLOCK(hrtc);
  
  return HAL_OK;
}

/**
  * @brief  Deactivates the Coarse calibration parameters.
  * @param  hrtc: RTC handle  
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTCEx_DeactivateCoarseCalib(RTC_HandleTypeDef* hrtc)
{ 
  /* Process Locked */ 
  __HAL_LOCK(hrtc);
  
  hrtc->State = HAL_RTC_STATE_BUSY;
  
  /* Disable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_DISABLE(hrtc);

  /* Set Initialization mode */
  if(RTC_EnterInitMode(hrtc) != HAL_OK)
  {
    /* Enable the write protection for RTC registers */
    __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc); 
    
    /* Set RTC state*/
    hrtc->State = HAL_RTC_STATE_ERROR;
    
    /* Process Unlocked */ 
    __HAL_UNLOCK(hrtc);
    
    return HAL_ERROR;
  } 
  else
  { 
    /* Enable the Coarse Calibration */
    __HAL_RTC_COARSE_CALIB_DISABLE(hrtc);
    
    /* Exit Initialization mode */
    hrtc->Instance->ISR &= (uint32_t)~RTC_ISR_INIT; 
  } 

  /* Enable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);
  
  /* Change state */
  hrtc->State = HAL_RTC_STATE_READY; 
  
  /* Process Unlocked */ 
  __HAL_UNLOCK(hrtc);
  
  return HAL_OK;
}

/**
  * @brief  Configure the Calibration Pinout (RTC_CALIB).
  * @param  hrtc : RTC handle    
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTCEx_SetCalibrationOutPut(RTC_HandleTypeDef* hrtc)
{
  /* Process Locked */ 
  __HAL_LOCK(hrtc);
  
  hrtc->State = HAL_RTC_STATE_BUSY;

  /* Disable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_DISABLE(hrtc);
  
  __HAL_RTC_CALIBRATION_OUTPUT_ENABLE(hrtc);
  
  /* Enable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);
  
  /* Change RTC state */
  hrtc->State = HAL_RTC_STATE_READY; 
  
  /* Process Unlocked */ 
  __HAL_UNLOCK(hrtc);
  
  return HAL_OK;
}

/**
  * @brief  Deactivates the Calibration Pinout (RTC_CALIB) Selection (1Hz or 512Hz).
  * @param  hrtc: RTC handle    
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTCEx_DeactivateCalibrationOutPut(RTC_HandleTypeDef* hrtc)
{
  /* Process Locked */ 
  __HAL_LOCK(hrtc);
  
  hrtc->State = HAL_RTC_STATE_BUSY;
  
  /* Disable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_DISABLE(hrtc);
  
  __HAL_RTC_CALIBRATION_OUTPUT_DISABLE(hrtc);
    
  /* Enable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);
  
  /* Change RTC state */
  hrtc->State = HAL_RTC_STATE_READY; 
  
  /* Process Unlocked */ 
  __HAL_UNLOCK(hrtc);
  
  return HAL_OK;
}

/**
  * @brief  Enables the RTC reference clock detection.
  * @param  hrtc: RTC handle    
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTCEx_SetRefClock(RTC_HandleTypeDef* hrtc)
{
  /* Process Locked */ 
  __HAL_LOCK(hrtc);
  
  hrtc->State = HAL_RTC_STATE_BUSY;
  
  /* Disable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_DISABLE(hrtc);
  
  /* Set Initialization mode */
  if(RTC_EnterInitMode(hrtc) != HAL_OK)
  {
    /* Enable the write protection for RTC registers */
    __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc); 
    
    /* Set RTC state*/
    hrtc->State = HAL_RTC_STATE_ERROR;
    
    /* Process Unlocked */ 
    __HAL_UNLOCK(hrtc);
    
    return HAL_ERROR;
  } 
  else
  {
    __HAL_RTC_CLOCKREF_DETECTION_ENABLE(hrtc);

    /* Exit Initialization mode */
    hrtc->Instance->ISR &= (uint32_t)~RTC_ISR_INIT; 
  }
  
  /* Enable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);
  
   /* Change RTC state */
  hrtc->State = HAL_RTC_STATE_READY; 
  
  /* Process Unlocked */ 
  __HAL_UNLOCK(hrtc);
  
  return HAL_OK;
}

/**
  * @brief  Disable the RTC reference clock detection.
  * @param  hrtc: RTC handle    
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTCEx_DeactivateRefClock(RTC_HandleTypeDef* hrtc)
{ 
  /* Process Locked */ 
  __HAL_LOCK(hrtc);
  
  hrtc->State = HAL_RTC_STATE_BUSY;
  
  /* Disable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_DISABLE(hrtc);
  
  /* Set Initialization mode */
  if(RTC_EnterInitMode(hrtc) != HAL_OK)
  {
    /* Enable the write protection for RTC registers */
    __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc); 
    
    /* Set RTC state*/
    hrtc->State = HAL_RTC_STATE_ERROR;
    
    /* Process Unlocked */ 
    __HAL_UNLOCK(hrtc);
    
    return HAL_ERROR;
  } 
  else
  {
    __HAL_RTC_CLOCKREF_DETECTION_DISABLE(hrtc);
    
    /* Exit Initialization mode */
    hrtc->Instance->ISR &= (uint32_t)~RTC_ISR_INIT; 
  }
  
  /* Enable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);
  
  /* Change RTC state */
  hrtc->State = HAL_RTC_STATE_READY; 
  
  /* Process Unlocked */ 
  __HAL_UNLOCK(hrtc);
  
  return HAL_OK;
}

/**
  * @}
  */

  /** @defgroup RTCEx_Group4 Extended features functions 
 *  @brief    Extended features functions  
 *
@verbatim   
 ===============================================================================
                 ##### Extended features functions #####
 ===============================================================================  
    [..]  This section provides functions allowing to:
      (+) RTC Alram B callback
      (+) RTC Poll for Alarm B request
               
@endverbatim
  * @{
  */

/**
  * @brief  Alarm B callback.
  * @param  hrtc: RTC handle
  * @retval None
  */
__weak void HAL_RTCEx_AlarmBEventCallback(RTC_HandleTypeDef *hrtc)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_RTC_AlarmBEventCallback could be implemented in the user file
   */
}

/**
  * @brief  This function handles AlarmB Polling request.
  * @param  hrtc: RTC handle
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTCEx_PollForAlarmBEvent(RTC_HandleTypeDef *hrtc, uint32_t Timeout)
{  
  uint32_t timeout = 0; 

  /* Get Timeout value */
  timeout = HAL_GetTick() + Timeout;   
  
  while(__HAL_RTC_ALARM_GET_FLAG(hrtc, RTC_FLAG_ALRBF) == RESET)
  {
    if(Timeout != HAL_MAX_DELAY)
    {
      if(HAL_GetTick() >= timeout)
      {
        hrtc->State = HAL_RTC_STATE_TIMEOUT;
        return HAL_TIMEOUT;
      }
    }
  }
  
  /* Clear the Alarm Flag */
  __HAL_RTC_ALARM_CLEAR_FLAG(hrtc, RTC_FLAG_ALRBF);
  
  /* Change RTC state */
  hrtc->State = HAL_RTC_STATE_READY; 
  
  return HAL_OK; 
}

/**
  * @}
  */

/**
  * @}
  */

#endif /* HAL_RTC_MODULE_ENABLED */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
