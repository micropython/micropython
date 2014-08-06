/**
  ******************************************************************************
  * @file    stm32f4xx_hal_rtc.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-June-2014
  * @brief   RTC HAL module driver.
  *          This file provides firmware functions to manage the following 
  *          functionalities of the Real Time Clock (RTC) peripheral:
  *           + Initialization and de-initialization functions
  *           + RTC Time and Date functions
  *           + RTC Alarm functions
  *           + Peripheral Control functions   
  *           + Peripheral State functions
  *         
  @verbatim
  ==============================================================================
              ##### Backup Domain Operating Condition #####
  ==============================================================================
  [..] The real-time clock (RTC), the RTC backup registers, and the backup 
       SRAM (BKP SRAM) can be powered from the VBAT voltage when the main 
       VDD supply is powered off.
       To retain the content of the RTC backup registers, backup SRAM, and supply 
       the RTC when VDD is turned off, VBAT pin can be connected to an optional 
       standby voltage supplied by a battery or by another source.

  [..] To allow the RTC operating even when the main digital supply (VDD) is turned
       off, the VBAT pin powers the following blocks:
    (#) The RTC
    (#) The LSE oscillator
    (#) The backup SRAM when the low power backup regulator is enabled
    (#) PC13 to PC15 I/Os, plus PI8 I/O (when available)
  
  [..] When the backup domain is supplied by VDD (analog switch connected to VDD),
       the following pins are available:
    (#) PC14 and PC15 can be used as either GPIO or LSE pins
    (#) PC13 can be used as a GPIO or as the RTC_AF1 pin
    (#) PI8 can be used as a GPIO or as the RTC_AF2 pin
  
  [..] When the backup domain is supplied by VBAT (analog switch connected to VBAT 
       because VDD is not present), the following pins are available:
    (#) PC14 and PC15 can be used as LSE pins only
    (#) PC13 can be used as the RTC_AF1 pin 
    (#) PI8 can be used as the RTC_AF2 pin
             
                   ##### Backup Domain Reset #####
  ==================================================================
  [..] The backup domain reset sets all RTC registers and the RCC_BDCR register 
       to their reset values. The BKPSRAM is not affected by this reset. The only
       way to reset the BKPSRAM is through the Flash interface by requesting 
       a protection level change from 1 to 0.
  [..] A backup domain reset is generated when one of the following events occurs:
    (#) Software reset, triggered by setting the BDRST bit in the 
        RCC Backup domain control register (RCC_BDCR). 
    (#) VDD or VBAT power on, if both supplies have previously been powered off.  

                   ##### Backup Domain Access #####
  ==================================================================
  [..] After reset, the backup domain (RTC registers, RTC backup data 
       registers and backup SRAM) is protected against possible unwanted write 
       accesses. 
  [..] To enable access to the RTC Domain and RTC registers, proceed as follows:
    (+) Enable the Power Controller (PWR) APB1 interface clock using the
        __PWR_CLK_ENABLE() function.
    (+) Enable access to RTC domain using the HAL_PWR_EnableBkUpAccess() function.
    (+) Select the RTC clock source using the __HAL_RCC_RTC_CONFIG() function.
    (+) Enable RTC Clock using the __HAL_RCC_RTC_ENABLE() function.
  
  
                  ##### How to use this driver #####
  ==================================================================
  [..] 
    (+) Enable the RTC domain access (see description in the section above).
    (+) Configure the RTC Prescaler (Asynchronous and Synchronous) and RTC hour 
        format using the HAL_RTC_Init() function.
  
  *** Time and Date configuration ***
  ===================================
  [..] 
    (+) To configure the RTC Calendar (Time and Date) use the HAL_RTC_SetTime() 
        and HAL_RTC_SetDate() functions.
    (+) To read the RTC Calendar, use the HAL_RTC_GetTime() and HAL_RTC_GetDate() functions. 
  
  *** Alarm configuration ***
  ===========================
  [..]
    (+) To configure the RTC Alarm use the HAL_RTC_SetAlarm() function. 
        You can also configure the RTC Alarm with interrupt mode using the HAL_RTC_SetAlarm_IT() function.
    (+) To read the RTC Alarm, use the HAL_RTC_GetAlarm() function.
  
                  ##### RTC and low power modes #####
  ==================================================================
  [..] The MCU can be woken up from a low power mode by an RTC alternate 
       function.
  [..] The RTC alternate functions are the RTC alarms (Alarm A and Alarm B), 
       RTC wakeup, RTC tamper event detection and RTC time stamp event detection.
       These RTC alternate functions can wake up the system from the Stop and 
       Standby low power modes.
  [..] The system can also wake up from low power modes without depending 
       on an external interrupt (Auto-wakeup mode), by using the RTC alarm 
       or the RTC wakeup events.
  [..] The RTC provides a programmable time base for waking up from the 
       Stop or Standby mode at regular intervals.
       Wakeup from STOP and STANDBY modes is possible only when the RTC clock source
       is LSE or LSI.
     
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

/** @defgroup RTC 
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

/** @defgroup RTC_Private_Functions
  * @{
  */
  
/** @defgroup RTC_Group1 Initialization and de-initialization functions 
 *  @brief    Initialization and Configuration functions 
 *
@verbatim    
 ===============================================================================
              ##### Initialization and de-initialization functions #####
 ===============================================================================
   [..] This section provides functions allowing to initialize and configure the 
         RTC Prescaler (Synchronous and Asynchronous), RTC Hour format, disable 
         RTC registers Write protection, enter and exit the RTC initialization mode, 
         RTC registers synchronization check and reference clock detection enable.
         (#) The RTC Prescaler is programmed to generate the RTC 1Hz time base. 
             It is split into 2 programmable prescalers to minimize power consumption.
             (++) A 7-bit asynchronous prescaler and a 13-bit synchronous prescaler.
             (++) When both prescalers are used, it is recommended to configure the 
                 asynchronous prescaler to a high value to minimize power consumption.
         (#) All RTC registers are Write protected. Writing to the RTC registers
             is enabled by writing a key into the Write Protection register, RTC_WPR.
         (#) To configure the RTC Calendar, user application should enter 
             initialization mode. In this mode, the calendar counter is stopped 
             and its value can be updated. When the initialization sequence is 
             complete, the calendar restarts counting after 4 RTCCLK cycles.
         (#) To read the calendar through the shadow registers after Calendar 
             initialization, calendar update or after wakeup from low power modes 
             the software must first clear the RSF flag. The software must then 
             wait until it is set again before reading the calendar, which means 
             that the calendar registers have been correctly copied into the 
             RTC_TR and RTC_DR shadow registers.The HAL_RTC_WaitForSynchro() function 
             implements the above software sequence (RSF clear and RSF check).
 
@endverbatim
  * @{
  */

/**
  * @brief  Initializes the RTC peripheral 
  * @param  hrtc: pointer to a RTC_HandleTypeDef structure that contains
  *                the configuration information for RTC.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTC_Init(RTC_HandleTypeDef *hrtc)
{
  /* Check the RTC peripheral state */
  if(hrtc == NULL)
  {
     return HAL_ERROR;
  }
  
  /* Check the parameters */
  assert_param(IS_RTC_HOUR_FORMAT(hrtc->Init.HourFormat));
  assert_param(IS_RTC_ASYNCH_PREDIV(hrtc->Init.AsynchPrediv));
  assert_param(IS_RTC_SYNCH_PREDIV(hrtc->Init.SynchPrediv));
  assert_param (IS_RTC_OUTPUT(hrtc->Init.OutPut));
  assert_param (IS_RTC_OUTPUT_POL(hrtc->Init.OutPutPolarity));
  assert_param(IS_RTC_OUTPUT_TYPE(hrtc->Init.OutPutType));
    
  if(hrtc->State == HAL_RTC_STATE_RESET)
  {
    /* Initialize RTC MSP */
    HAL_RTC_MspInit(hrtc);
  }
  
  /* Set RTC state */  
  hrtc->State = HAL_RTC_STATE_BUSY;  
       
  /* Disable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_DISABLE(hrtc);

  /* Set Initialization mode */
  if(RTC_EnterInitMode(hrtc) != HAL_OK)
  {
    /* Enable the write protection for RTC registers */
    __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc); 
    
    /* Set RTC state */
    hrtc->State = HAL_RTC_STATE_ERROR;
    
    return HAL_ERROR;
  } 
  else
  { 
    /* Clear RTC_CR FMT, OSEL and POL Bits */
    hrtc->Instance->CR &= ((uint32_t)~(RTC_CR_FMT | RTC_CR_OSEL | RTC_CR_POL));
    /* Set RTC_CR register */
    hrtc->Instance->CR |= (uint32_t)(hrtc->Init.HourFormat | hrtc->Init.OutPut | hrtc->Init.OutPutPolarity);
    
    /* Configure the RTC PRER */
    hrtc->Instance->PRER = (uint32_t)(hrtc->Init.SynchPrediv);
    hrtc->Instance->PRER |= (uint32_t)(hrtc->Init.AsynchPrediv << 16);
    
    /* Exit Initialization mode */
    hrtc->Instance->ISR &= (uint32_t)~RTC_ISR_INIT; 
    
    hrtc->Instance->TAFCR &= (uint32_t)~RTC_TAFCR_ALARMOUTTYPE;
    hrtc->Instance->TAFCR |= (uint32_t)(hrtc->Init.OutPutType); 
    
    /* Enable the write protection for RTC registers */
    __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc); 
    
    /* Set RTC state */
    hrtc->State = HAL_RTC_STATE_READY;
    
    return HAL_OK;
  }
}

/**
  * @brief  DeInitializes the RTC peripheral 
  * @param  hrtc: pointer to a RTC_HandleTypeDef structure that contains
  *                the configuration information for RTC.
  * @note   This function doesn't reset the RTC Backup Data registers.   
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTC_DeInit(RTC_HandleTypeDef *hrtc)
{
  uint32_t tickstart = 0;

  /* Set RTC state */
  hrtc->State = HAL_RTC_STATE_BUSY; 
  
  /* Disable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_DISABLE(hrtc);
  
  /* Set Initialization mode */
  if(RTC_EnterInitMode(hrtc) != HAL_OK)
  {
    /* Enable the write protection for RTC registers */
    __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc); 
    
    /* Set RTC state */
    hrtc->State = HAL_RTC_STATE_ERROR;
    
    return HAL_ERROR;
  }  
  else
  {
    /* Reset TR, DR and CR registers */
    hrtc->Instance->TR = (uint32_t)0x00000000;
    hrtc->Instance->DR = (uint32_t)0x00002101;
    /* Reset All CR bits except CR[2:0] */
    hrtc->Instance->CR &= (uint32_t)0x00000007;

    /* Get tick */
    tickstart = HAL_GetTick();

    /* Wait till WUTWF flag is set and if Time out is reached exit */
    while(((hrtc->Instance->ISR) & RTC_ISR_WUTWF) == (uint32_t)RESET)
    {
      if((HAL_GetTick() - tickstart ) > RTC_TIMEOUT_VALUE)
      { 
        /* Enable the write protection for RTC registers */
        __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc); 
        
        /* Set RTC state */
        hrtc->State = HAL_RTC_STATE_TIMEOUT;
        
        return HAL_TIMEOUT;
      }      
    }
    
    /* Reset all RTC CR register bits */
    hrtc->Instance->CR &= (uint32_t)0x00000000;
    hrtc->Instance->WUTR = (uint32_t)0x0000FFFF;
    hrtc->Instance->PRER = (uint32_t)0x007F00FF;
    hrtc->Instance->CALIBR = (uint32_t)0x00000000;
    hrtc->Instance->ALRMAR = (uint32_t)0x00000000;
    hrtc->Instance->ALRMBR = (uint32_t)0x00000000;
    hrtc->Instance->SHIFTR = (uint32_t)0x00000000;
    hrtc->Instance->CALR = (uint32_t)0x00000000;
    hrtc->Instance->ALRMASSR = (uint32_t)0x00000000;
    hrtc->Instance->ALRMBSSR = (uint32_t)0x00000000;
    
    /* Reset ISR register and exit initialization mode */
    hrtc->Instance->ISR = (uint32_t)0x00000000;
    
    /* Reset Tamper and alternate functions configuration register */
    hrtc->Instance->TAFCR = 0x00000000;
    
    /* If  RTC_CR_BYPSHAD bit = 0, wait for synchro else this check is not needed */
    if((hrtc->Instance->CR & RTC_CR_BYPSHAD) == RESET)
    {
      if(HAL_RTC_WaitForSynchro(hrtc) != HAL_OK)
      {
        /* Enable the write protection for RTC registers */
        __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);  
        
        hrtc->State = HAL_RTC_STATE_ERROR;
        
        return HAL_ERROR;
      }
    }    
  }
  
  /* Enable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);
  
  /* De-Initialize RTC MSP */
  HAL_RTC_MspDeInit(hrtc);
  
  hrtc->State = HAL_RTC_STATE_RESET; 

  /* Release Lock */
  __HAL_UNLOCK(hrtc);

  return HAL_OK;
}

/**
  * @brief  Initializes the RTC MSP.
  * @param  hrtc: pointer to a RTC_HandleTypeDef structure that contains
  *                the configuration information for RTC.  
  * @retval None
  */
__weak void HAL_RTC_MspInit(RTC_HandleTypeDef* hrtc)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_RTC_MspInit could be implenetd in the user file
   */ 
}

/**
  * @brief  DeInitializes the RTC MSP.
  * @param  hrtc: pointer to a RTC_HandleTypeDef structure that contains
  *                the configuration information for RTC. 
  * @retval None
  */
__weak void HAL_RTC_MspDeInit(RTC_HandleTypeDef* hrtc)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_RTC_MspDeInit could be implenetd in the user file
   */ 
}

/**
  * @}
  */

/** @defgroup RTC_Group2 RTC Time and Date functions
 *  @brief   RTC Time and Date functions
 *
@verbatim   
 ===============================================================================
                 ##### RTC Time and Date functions #####
 ===============================================================================  
 
 [..] This section provides functions allowing to configure Time and Date features

@endverbatim
  * @{
  */

/**
  * @brief  Sets RTC current time.
  * @param  hrtc: pointer to a RTC_HandleTypeDef structure that contains
  *                the configuration information for RTC.
  * @param  sTime: Pointer to Time structure
  * @param  Format: Specifies the format of the entered parameters.
  *          This parameter can be one of the following values:
  *            @arg FORMAT_BIN: Binary data format 
  *            @arg FORMAT_BCD: BCD data format
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTC_SetTime(RTC_HandleTypeDef *hrtc, RTC_TimeTypeDef *sTime, uint32_t Format)
{
  uint32_t tmpreg = 0;
  
 /* Check the parameters */
  assert_param(IS_RTC_FORMAT(Format));
  assert_param(IS_RTC_DAYLIGHT_SAVING(sTime->DayLightSaving));
  assert_param(IS_RTC_STORE_OPERATION(sTime->StoreOperation));
  
  /* Process Locked */ 
  __HAL_LOCK(hrtc);
  
  hrtc->State = HAL_RTC_STATE_BUSY;
  
  if(Format == FORMAT_BIN)
  {
    if((hrtc->Instance->CR & RTC_CR_FMT) != (uint32_t)RESET)
    {
      assert_param(IS_RTC_HOUR12(sTime->Hours));
      assert_param(IS_RTC_HOURFORMAT12(sTime->TimeFormat));
    } 
    else
    {
      sTime->TimeFormat = 0x00;
      assert_param(IS_RTC_HOUR24(sTime->Hours));
    }
    assert_param(IS_RTC_MINUTES(sTime->Minutes));
    assert_param(IS_RTC_SECONDS(sTime->Seconds));
    
    tmpreg = (uint32_t)(((uint32_t)RTC_ByteToBcd2(sTime->Hours) << 16) | \
                        ((uint32_t)RTC_ByteToBcd2(sTime->Minutes) << 8) | \
                        ((uint32_t)RTC_ByteToBcd2(sTime->Seconds)) | \
                        (((uint32_t)sTime->TimeFormat) << 16));  
  }
  else
  {
    if((hrtc->Instance->CR & RTC_CR_FMT) != (uint32_t)RESET)
    {
      tmpreg = RTC_Bcd2ToByte(sTime->Hours);
      assert_param(IS_RTC_HOUR12(tmpreg));
      assert_param(IS_RTC_HOURFORMAT12(sTime->TimeFormat)); 
    } 
    else
    {
      sTime->TimeFormat = 0x00;
      assert_param(IS_RTC_HOUR24(RTC_Bcd2ToByte(sTime->Hours)));
    }
    assert_param(IS_RTC_MINUTES(RTC_Bcd2ToByte(sTime->Minutes)));
    assert_param(IS_RTC_SECONDS(RTC_Bcd2ToByte(sTime->Seconds)));
    tmpreg = (((uint32_t)(sTime->Hours) << 16) | \
              ((uint32_t)(sTime->Minutes) << 8) | \
              ((uint32_t)sTime->Seconds) | \
              ((uint32_t)(sTime->TimeFormat) << 16));   
  }
  
  /* Disable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_DISABLE(hrtc);
  
  /* Set Initialization mode */
  if(RTC_EnterInitMode(hrtc) != HAL_OK)
  {
    /* Enable the write protection for RTC registers */
    __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc); 
    
    /* Set RTC state */
    hrtc->State = HAL_RTC_STATE_ERROR;
    
    /* Process Unlocked */ 
    __HAL_UNLOCK(hrtc);
    
    return HAL_ERROR;
  } 
  else
  {
    /* Set the RTC_TR register */
    hrtc->Instance->TR = (uint32_t)(tmpreg & RTC_TR_RESERVED_MASK);
     
    /* Clear the bits to be configured */
    hrtc->Instance->CR &= (uint32_t)~RTC_CR_BCK;
    
    /* Configure the RTC_CR register */
    hrtc->Instance->CR |= (uint32_t)(sTime->DayLightSaving | sTime->StoreOperation);
    
    /* Exit Initialization mode */
    hrtc->Instance->ISR &= (uint32_t)~RTC_ISR_INIT;  
    
    /* If  CR_BYPSHAD bit = 0, wait for synchro else this check is not needed */
    if((hrtc->Instance->CR & RTC_CR_BYPSHAD) == RESET)
    {
      if(HAL_RTC_WaitForSynchro(hrtc) != HAL_OK)
      {        
        /* Enable the write protection for RTC registers */
        __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);  
        
        hrtc->State = HAL_RTC_STATE_ERROR;
        
        /* Process Unlocked */ 
        __HAL_UNLOCK(hrtc);
        
        return HAL_ERROR;
      }
    }
    
    /* Enable the write protection for RTC registers */
    __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);
    
   hrtc->State = HAL_RTC_STATE_READY;
  
   __HAL_UNLOCK(hrtc); 
     
   return HAL_OK;
  }
}

/**
  * @brief  Gets RTC current time.
  * @param  hrtc: pointer to a RTC_HandleTypeDef structure that contains
  *                the configuration information for RTC.
  * @param  sTime: Pointer to Time structure
  * @param  Format: Specifies the format of the entered parameters.
  *          This parameter can be one of the following values:
  *            @arg FORMAT_BIN: Binary data format 
  *            @arg FORMAT_BCD: BCD data format
  * @note   Call HAL_RTC_GetDate() after HAL_RTC_GetTime() to unlock the values 
  *         in the higher-order calendar shadow registers.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTC_GetTime(RTC_HandleTypeDef *hrtc, RTC_TimeTypeDef *sTime, uint32_t Format)
{
  uint32_t tmpreg = 0;

  /* Check the parameters */
  assert_param(IS_RTC_FORMAT(Format));
  
  /* Get subseconds values from the correspondent registers*/
  sTime->SubSeconds = (uint32_t)(hrtc->Instance->SSR);

  /* Get the TR register */
  tmpreg = (uint32_t)(hrtc->Instance->TR & RTC_TR_RESERVED_MASK); 
  
  /* Fill the structure fields with the read parameters */
  sTime->Hours = (uint8_t)((tmpreg & (RTC_TR_HT | RTC_TR_HU)) >> 16);
  sTime->Minutes = (uint8_t)((tmpreg & (RTC_TR_MNT | RTC_TR_MNU)) >>8);
  sTime->Seconds = (uint8_t)(tmpreg & (RTC_TR_ST | RTC_TR_SU));
  sTime->TimeFormat = (uint8_t)((tmpreg & (RTC_TR_PM)) >> 16); 
  
  /* Check the input parameters format */
  if(Format == FORMAT_BIN)
  {
    /* Convert the time structure parameters to Binary format */
    sTime->Hours = (uint8_t)RTC_Bcd2ToByte(sTime->Hours);
    sTime->Minutes = (uint8_t)RTC_Bcd2ToByte(sTime->Minutes);
    sTime->Seconds = (uint8_t)RTC_Bcd2ToByte(sTime->Seconds);  
  }
  
  return HAL_OK;
}

/**
  * @brief  Sets RTC current date.
  * @param  hrtc: pointer to a RTC_HandleTypeDef structure that contains
  *                the configuration information for RTC.
  * @param  sDate: Pointer to date structure
  * @param  Format: specifies the format of the entered parameters.
  *          This parameter can be one of the following values:
  *            @arg FORMAT_BIN: Binary data format 
  *            @arg FORMAT_BCD: BCD data format
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTC_SetDate(RTC_HandleTypeDef *hrtc, RTC_DateTypeDef *sDate, uint32_t Format)
{
  uint32_t datetmpreg = 0;
  
 /* Check the parameters */
  assert_param(IS_RTC_FORMAT(Format));
  
 /* Process Locked */ 
 __HAL_LOCK(hrtc);
  
  hrtc->State = HAL_RTC_STATE_BUSY; 
  
  if((Format == FORMAT_BIN) && ((sDate->Month & 0x10) == 0x10))
  {
    sDate->Month = (uint8_t)((sDate->Month & (uint8_t)~(0x10)) + (uint8_t)0x0A);
  }
  
  assert_param(IS_RTC_WEEKDAY(sDate->WeekDay));
  
  if(Format == FORMAT_BIN)
  {   
    assert_param(IS_RTC_YEAR(sDate->Year));
    assert_param(IS_RTC_MONTH(sDate->Month));
    assert_param(IS_RTC_DATE(sDate->Date)); 
    
   datetmpreg = (((uint32_t)RTC_ByteToBcd2(sDate->Year) << 16) | \
                 ((uint32_t)RTC_ByteToBcd2(sDate->Month) << 8) | \
                 ((uint32_t)RTC_ByteToBcd2(sDate->Date)) | \
                 ((uint32_t)sDate->WeekDay << 13));   
  }
  else
  {   
    assert_param(IS_RTC_YEAR(RTC_Bcd2ToByte(sDate->Year)));
    datetmpreg = RTC_Bcd2ToByte(sDate->Month);
    assert_param(IS_RTC_MONTH(datetmpreg));
    datetmpreg = RTC_Bcd2ToByte(sDate->Date);
    assert_param(IS_RTC_DATE(datetmpreg));
    
    datetmpreg = ((((uint32_t)sDate->Year) << 16) | \
                  (((uint32_t)sDate->Month) << 8) | \
                  ((uint32_t)sDate->Date) | \
                  (((uint32_t)sDate->WeekDay) << 13));  
  }

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
    /* Set the RTC_DR register */
    hrtc->Instance->DR = (uint32_t)(datetmpreg & RTC_DR_RESERVED_MASK);
    
    /* Exit Initialization mode */
    hrtc->Instance->ISR &= (uint32_t)~RTC_ISR_INIT;  
    
    /* If  CR_BYPSHAD bit = 0, wait for synchro else this check is not needed */
    if((hrtc->Instance->CR & RTC_CR_BYPSHAD) == RESET)
    {
      if(HAL_RTC_WaitForSynchro(hrtc) != HAL_OK)
      { 
        /* Enable the write protection for RTC registers */
        __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);  
        
        hrtc->State = HAL_RTC_STATE_ERROR;
        
        /* Process Unlocked */ 
        __HAL_UNLOCK(hrtc);
        
        return HAL_ERROR;
      }
    }
    
    /* Enable the write protection for RTC registers */
    __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);  
    
    hrtc->State = HAL_RTC_STATE_READY ;
    
    /* Process Unlocked */ 
    __HAL_UNLOCK(hrtc);
    
    return HAL_OK;    
  }
}

/**
  * @brief  Gets RTC current date.
  * @param  hrtc: pointer to a RTC_HandleTypeDef structure that contains
  *                the configuration information for RTC.
  * @param  sDate: Pointer to Date structure
  * @param  Format: Specifies the format of the entered parameters.
  *          This parameter can be one of the following values:
  *            @arg FORMAT_BIN:  Binary data format 
  *            @arg FORMAT_BCD:  BCD data format
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTC_GetDate(RTC_HandleTypeDef *hrtc, RTC_DateTypeDef *sDate, uint32_t Format)
{
  uint32_t datetmpreg = 0;

  /* Check the parameters */
  assert_param(IS_RTC_FORMAT(Format));
          
  /* Get the DR register */
  datetmpreg = (uint32_t)(hrtc->Instance->DR & RTC_DR_RESERVED_MASK); 

  /* Fill the structure fields with the read parameters */
  sDate->Year = (uint8_t)((datetmpreg & (RTC_DR_YT | RTC_DR_YU)) >> 16);
  sDate->Month = (uint8_t)((datetmpreg & (RTC_DR_MT | RTC_DR_MU)) >> 8);
  sDate->Date = (uint8_t)(datetmpreg & (RTC_DR_DT | RTC_DR_DU));
  sDate->WeekDay = (uint8_t)((datetmpreg & (RTC_DR_WDU)) >> 13); 

  /* Check the input parameters format */
  if(Format == FORMAT_BIN)
  {    
    /* Convert the date structure parameters to Binary format */
    sDate->Year = (uint8_t)RTC_Bcd2ToByte(sDate->Year);
    sDate->Month = (uint8_t)RTC_Bcd2ToByte(sDate->Month);
    sDate->Date = (uint8_t)RTC_Bcd2ToByte(sDate->Date);  
  }
  return HAL_OK;
}

/**
  * @}
  */

/** @defgroup RTC_Group3 RTC Alarm functions
 *  @brief   RTC Alarm functions
 *
@verbatim   
 ===============================================================================
                 ##### RTC Alarm functions #####
 ===============================================================================  
 
 [..] This section provides functions allowing to configure Alarm feature

@endverbatim
  * @{
  */
/**
  * @brief  Sets the specified RTC Alarm.
  * @param  hrtc: pointer to a RTC_HandleTypeDef structure that contains
  *                the configuration information for RTC.
  * @param  sAlarm: Pointer to Alarm structure
  * @param  Format: Specifies the format of the entered parameters.
  *          This parameter can be one of the following values:
  *             @arg FORMAT_BIN: Binary data format 
  *             @arg FORMAT_BCD: BCD data format
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTC_SetAlarm(RTC_HandleTypeDef *hrtc, RTC_AlarmTypeDef *sAlarm, uint32_t Format)
{
  uint32_t tickstart = 0;
  uint32_t tmpreg = 0, subsecondtmpreg = 0;
  
  /* Check the parameters */
  assert_param(IS_RTC_FORMAT(Format));
  assert_param(IS_ALARM(sAlarm->Alarm));
  assert_param(IS_ALARM_MASK(sAlarm->AlarmMask));
  assert_param(IS_RTC_ALARM_DATE_WEEKDAY_SEL(sAlarm->AlarmDateWeekDaySel));
  assert_param(IS_RTC_ALARM_SUB_SECOND_VALUE(sAlarm->AlarmTime.SubSeconds));
  assert_param(IS_RTC_ALARM_SUB_SECOND_MASK(sAlarm->AlarmSubSecondMask));
  
  /* Process Locked */ 
  __HAL_LOCK(hrtc);
  
  hrtc->State = HAL_RTC_STATE_BUSY;
  
  if(Format == FORMAT_BIN)
  {
    if((hrtc->Instance->CR & RTC_CR_FMT) != (uint32_t)RESET)
    {
      assert_param(IS_RTC_HOUR12(sAlarm->AlarmTime.Hours));
      assert_param(IS_RTC_HOURFORMAT12(sAlarm->AlarmTime.TimeFormat));
    } 
    else
    {
      sAlarm->AlarmTime.TimeFormat = 0x00;
      assert_param(IS_RTC_HOUR24(sAlarm->AlarmTime.Hours));
    }
    assert_param(IS_RTC_MINUTES(sAlarm->AlarmTime.Minutes));
    assert_param(IS_RTC_SECONDS(sAlarm->AlarmTime.Seconds));
    
    if(sAlarm->AlarmDateWeekDaySel == RTC_ALARMDATEWEEKDAYSEL_DATE)
    {
      assert_param(IS_RTC_ALARM_DATE_WEEKDAY_DATE(sAlarm->AlarmDateWeekDay));
    }
    else
    {
      assert_param(IS_RTC_ALARM_DATE_WEEKDAY_WEEKDAY(sAlarm->AlarmDateWeekDay));
    }
    
    tmpreg = (((uint32_t)RTC_ByteToBcd2(sAlarm->AlarmTime.Hours) << 16) | \
              ((uint32_t)RTC_ByteToBcd2(sAlarm->AlarmTime.Minutes) << 8) | \
              ((uint32_t)RTC_ByteToBcd2(sAlarm->AlarmTime.Seconds)) | \
              ((uint32_t)(sAlarm->AlarmTime.TimeFormat) << 16) | \
              ((uint32_t)RTC_ByteToBcd2(sAlarm->AlarmDateWeekDay) << 24) | \
              ((uint32_t)sAlarm->AlarmDateWeekDaySel) | \
              ((uint32_t)sAlarm->AlarmMask)); 
  }
  else
  {
    if((hrtc->Instance->CR & RTC_CR_FMT) != (uint32_t)RESET)
    {
      tmpreg = RTC_Bcd2ToByte(sAlarm->AlarmTime.Hours);
      assert_param(IS_RTC_HOUR12(tmpreg));
      assert_param(IS_RTC_HOURFORMAT12(sAlarm->AlarmTime.TimeFormat));
    } 
    else
    {
      sAlarm->AlarmTime.TimeFormat = 0x00;
      assert_param(IS_RTC_HOUR24(RTC_Bcd2ToByte(sAlarm->AlarmTime.Hours)));
    }
    
    assert_param(IS_RTC_MINUTES(RTC_Bcd2ToByte(sAlarm->AlarmTime.Minutes)));
    assert_param(IS_RTC_SECONDS(RTC_Bcd2ToByte(sAlarm->AlarmTime.Seconds)));
    
    if(sAlarm->AlarmDateWeekDaySel == RTC_ALARMDATEWEEKDAYSEL_DATE)
    {
      tmpreg = RTC_Bcd2ToByte(sAlarm->AlarmDateWeekDay);
      assert_param(IS_RTC_ALARM_DATE_WEEKDAY_DATE(tmpreg));    
    }
    else
    {
      tmpreg = RTC_Bcd2ToByte(sAlarm->AlarmDateWeekDay);
      assert_param(IS_RTC_ALARM_DATE_WEEKDAY_WEEKDAY(tmpreg));      
    }  
    
    tmpreg = (((uint32_t)(sAlarm->AlarmTime.Hours) << 16) | \
              ((uint32_t)(sAlarm->AlarmTime.Minutes) << 8) | \
              ((uint32_t) sAlarm->AlarmTime.Seconds) | \
              ((uint32_t)(sAlarm->AlarmTime.TimeFormat) << 16) | \
              ((uint32_t)(sAlarm->AlarmDateWeekDay) << 24) | \
              ((uint32_t)sAlarm->AlarmDateWeekDaySel) | \
              ((uint32_t)sAlarm->AlarmMask));   
  }
  
  /* Configure the Alarm A or Alarm B Sub Second registers */
  subsecondtmpreg = (uint32_t)((uint32_t)(sAlarm->AlarmTime.SubSeconds) | (uint32_t)(sAlarm->AlarmSubSecondMask));
  
  /* Disable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_DISABLE(hrtc);

  /* Configure the Alarm register */
  if(sAlarm->Alarm == RTC_ALARM_A)
  {
    /* Disable the Alarm A interrupt */
    __HAL_RTC_ALARMA_DISABLE(hrtc);
    
    /* In case of interrupt mode is used, the interrupt source must disabled */ 
    __HAL_RTC_ALARM_DISABLE_IT(hrtc, RTC_IT_ALRA);

    /* Get tick */
    tickstart = HAL_GetTick();

    /* Wait till RTC ALRAWF flag is set and if Time out is reached exit */
    while(__HAL_RTC_ALARM_GET_FLAG(hrtc, RTC_FLAG_ALRAWF) == RESET)
    {
      if((HAL_GetTick() - tickstart ) > RTC_TIMEOUT_VALUE)
      {
        /* Enable the write protection for RTC registers */
        __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);
        
        hrtc->State = HAL_RTC_STATE_TIMEOUT; 
        
        /* Process Unlocked */ 
        __HAL_UNLOCK(hrtc);
        
        return HAL_TIMEOUT;
      }   
    }
    
    hrtc->Instance->ALRMAR = (uint32_t)tmpreg;
    /* Configure the Alarm A Sub Second register */
    hrtc->Instance->ALRMASSR = subsecondtmpreg;
    /* Configure the Alarm state: Enable Alarm */
    __HAL_RTC_ALARMA_ENABLE(hrtc);
  }
  else
  {
    /* Disable the Alarm B interrupt */
    __HAL_RTC_ALARMB_DISABLE(hrtc);
    
    /* In case of interrupt mode is used, the interrupt source must disabled */ 
    __HAL_RTC_ALARM_DISABLE_IT(hrtc, RTC_IT_ALRB);

    /* Get tick */
    tickstart = HAL_GetTick();

    /* Wait till RTC ALRBWF flag is set and if Time out is reached exit */
    while(__HAL_RTC_ALARM_GET_FLAG(hrtc, RTC_FLAG_ALRBWF) == RESET)
    {
      if((HAL_GetTick() - tickstart ) > RTC_TIMEOUT_VALUE)
      {
        /* Enable the write protection for RTC registers */
        __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);
        
        hrtc->State = HAL_RTC_STATE_TIMEOUT; 
        
        /* Process Unlocked */ 
        __HAL_UNLOCK(hrtc);
        
        return HAL_TIMEOUT;
      }  
    }    
    
    hrtc->Instance->ALRMBR = (uint32_t)tmpreg;
    /* Configure the Alarm B Sub Second register */
    hrtc->Instance->ALRMBSSR = subsecondtmpreg;
    /* Configure the Alarm state: Enable Alarm */
    __HAL_RTC_ALARMB_ENABLE(hrtc); 
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
  * @brief  Sets the specified RTC Alarm with Interrupt 
  * @param  hrtc: pointer to a RTC_HandleTypeDef structure that contains
  *                the configuration information for RTC.
  * @param  sAlarm: Pointer to Alarm structure
  * @param  Format: Specifies the format of the entered parameters.
  *          This parameter can be one of the following values:
  *             @arg FORMAT_BIN: Binary data format 
  *             @arg FORMAT_BCD: BCD data format
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTC_SetAlarm_IT(RTC_HandleTypeDef *hrtc, RTC_AlarmTypeDef *sAlarm, uint32_t Format)
{
  uint32_t tickstart = 0;
  uint32_t tmpreg = 0, subsecondtmpreg = 0;
  
  /* Check the parameters */
  assert_param(IS_RTC_FORMAT(Format));
  assert_param(IS_ALARM(sAlarm->Alarm));
  assert_param(IS_ALARM_MASK(sAlarm->AlarmMask));
  assert_param(IS_RTC_ALARM_DATE_WEEKDAY_SEL(sAlarm->AlarmDateWeekDaySel));
  assert_param(IS_RTC_ALARM_SUB_SECOND_VALUE(sAlarm->AlarmTime.SubSeconds));
  assert_param(IS_RTC_ALARM_SUB_SECOND_MASK(sAlarm->AlarmSubSecondMask));
      
  /* Process Locked */ 
  __HAL_LOCK(hrtc);
  
  hrtc->State = HAL_RTC_STATE_BUSY;
  
  if(Format == FORMAT_BIN)
  {
    if((hrtc->Instance->CR & RTC_CR_FMT) != (uint32_t)RESET)
    {
      assert_param(IS_RTC_HOUR12(sAlarm->AlarmTime.Hours));
      assert_param(IS_RTC_HOURFORMAT12(sAlarm->AlarmTime.TimeFormat));
    } 
    else
    {
      sAlarm->AlarmTime.TimeFormat = 0x00;
      assert_param(IS_RTC_HOUR24(sAlarm->AlarmTime.Hours));
    }
    assert_param(IS_RTC_MINUTES(sAlarm->AlarmTime.Minutes));
    assert_param(IS_RTC_SECONDS(sAlarm->AlarmTime.Seconds));
    
    if(sAlarm->AlarmDateWeekDaySel == RTC_ALARMDATEWEEKDAYSEL_DATE)
    {
      assert_param(IS_RTC_ALARM_DATE_WEEKDAY_DATE(sAlarm->AlarmDateWeekDay));
    }
    else
    {
      assert_param(IS_RTC_ALARM_DATE_WEEKDAY_WEEKDAY(sAlarm->AlarmDateWeekDay));
    }
    tmpreg = (((uint32_t)RTC_ByteToBcd2(sAlarm->AlarmTime.Hours) << 16) | \
              ((uint32_t)RTC_ByteToBcd2(sAlarm->AlarmTime.Minutes) << 8) | \
              ((uint32_t)RTC_ByteToBcd2(sAlarm->AlarmTime.Seconds)) | \
              ((uint32_t)(sAlarm->AlarmTime.TimeFormat) << 16) | \
              ((uint32_t)RTC_ByteToBcd2(sAlarm->AlarmDateWeekDay) << 24) | \
              ((uint32_t)sAlarm->AlarmDateWeekDaySel) | \
              ((uint32_t)sAlarm->AlarmMask)); 
  }
  else
  {
    if((hrtc->Instance->CR & RTC_CR_FMT) != (uint32_t)RESET)
    {
      tmpreg = RTC_Bcd2ToByte(sAlarm->AlarmTime.Hours);
      assert_param(IS_RTC_HOUR12(tmpreg));
      assert_param(IS_RTC_HOURFORMAT12(sAlarm->AlarmTime.TimeFormat));
    } 
    else
    {
      sAlarm->AlarmTime.TimeFormat = 0x00;
      assert_param(IS_RTC_HOUR24(RTC_Bcd2ToByte(sAlarm->AlarmTime.Hours)));
    }
    
    assert_param(IS_RTC_MINUTES(RTC_Bcd2ToByte(sAlarm->AlarmTime.Minutes)));
    assert_param(IS_RTC_SECONDS(RTC_Bcd2ToByte(sAlarm->AlarmTime.Seconds)));
    
    if(sAlarm->AlarmDateWeekDaySel == RTC_ALARMDATEWEEKDAYSEL_DATE)
    {
      tmpreg = RTC_Bcd2ToByte(sAlarm->AlarmDateWeekDay);
      assert_param(IS_RTC_ALARM_DATE_WEEKDAY_DATE(tmpreg));    
    }
    else
    {
      tmpreg = RTC_Bcd2ToByte(sAlarm->AlarmDateWeekDay);
      assert_param(IS_RTC_ALARM_DATE_WEEKDAY_WEEKDAY(tmpreg));      
    }
    tmpreg = (((uint32_t)(sAlarm->AlarmTime.Hours) << 16) | \
              ((uint32_t)(sAlarm->AlarmTime.Minutes) << 8) | \
              ((uint32_t) sAlarm->AlarmTime.Seconds) | \
              ((uint32_t)(sAlarm->AlarmTime.TimeFormat) << 16) | \
              ((uint32_t)(sAlarm->AlarmDateWeekDay) << 24) | \
              ((uint32_t)sAlarm->AlarmDateWeekDaySel) | \
              ((uint32_t)sAlarm->AlarmMask));     
  }
  /* Configure the Alarm A or Alarm B Sub Second registers */
  subsecondtmpreg = (uint32_t)((uint32_t)(sAlarm->AlarmTime.SubSeconds) | (uint32_t)(sAlarm->AlarmSubSecondMask));
  
  /* Disable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_DISABLE(hrtc);
  
  /* Configure the Alarm register */
  if(sAlarm->Alarm == RTC_ALARM_A)
  {
    /* Disable the Alarm A interrupt */
    __HAL_RTC_ALARMA_DISABLE(hrtc);

    /* Clear flag alarm A */
    __HAL_RTC_ALARM_CLEAR_FLAG(hrtc, RTC_FLAG_ALRAF);

    /* Get tick */
    tickstart = HAL_GetTick();

    /* Wait till RTC ALRAWF flag is set and if Time out is reached exit */
    while(__HAL_RTC_ALARM_GET_FLAG(hrtc, RTC_FLAG_ALRAWF) == RESET)
    {
      if((HAL_GetTick() - tickstart ) > RTC_TIMEOUT_VALUE)
      {
        /* Enable the write protection for RTC registers */
        __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);
        
        hrtc->State = HAL_RTC_STATE_TIMEOUT; 
        
        /* Process Unlocked */ 
        __HAL_UNLOCK(hrtc);
        
        return HAL_TIMEOUT;
      }  
    }
    
    hrtc->Instance->ALRMAR = (uint32_t)tmpreg;
    /* Configure the Alarm A Sub Second register */
    hrtc->Instance->ALRMASSR = subsecondtmpreg;
    /* Configure the Alarm state: Enable Alarm */
    __HAL_RTC_ALARMA_ENABLE(hrtc);
    /* Configure the Alarm interrupt */
    __HAL_RTC_ALARM_ENABLE_IT(hrtc,RTC_IT_ALRA);
  }
  else
  {
    /* Disable the Alarm B interrupt */
    __HAL_RTC_ALARMB_DISABLE(hrtc);

    /* Clear flag alarm B */
    __HAL_RTC_ALARM_CLEAR_FLAG(hrtc, RTC_FLAG_ALRBF);

    /* Get tick */
    tickstart = HAL_GetTick();

    /* Wait till RTC ALRBWF flag is set and if Time out is reached exit */
    while(__HAL_RTC_ALARM_GET_FLAG(hrtc, RTC_FLAG_ALRBWF) == RESET)
    {
      if((HAL_GetTick() - tickstart ) > RTC_TIMEOUT_VALUE)
      {
        /* Enable the write protection for RTC registers */
        __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);
        
        hrtc->State = HAL_RTC_STATE_TIMEOUT; 
        
        /* Process Unlocked */ 
        __HAL_UNLOCK(hrtc);
        
        return HAL_TIMEOUT;
      }  
    }

    hrtc->Instance->ALRMBR = (uint32_t)tmpreg;
    /* Configure the Alarm B Sub Second register */
    hrtc->Instance->ALRMBSSR = subsecondtmpreg;
    /* Configure the Alarm state: Enable Alarm */
    __HAL_RTC_ALARMB_ENABLE(hrtc);
    /* Configure the Alarm interrupt */
    __HAL_RTC_ALARM_ENABLE_IT(hrtc, RTC_IT_ALRB);
  }

  /* RTC Alarm Interrupt Configuration: EXTI configuration */
  __HAL_RTC_EXTI_ENABLE_IT(RTC_EXTI_LINE_ALARM_EVENT);
  
  EXTI->RTSR |= RTC_EXTI_LINE_ALARM_EVENT;
  
  /* Enable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);  
  
  hrtc->State = HAL_RTC_STATE_READY; 
  
  /* Process Unlocked */ 
  __HAL_UNLOCK(hrtc);  
  
  return HAL_OK;
}

/**
  * @brief  Deactive the specified RTC Alarm 
  * @param  hrtc: pointer to a RTC_HandleTypeDef structure that contains
  *                the configuration information for RTC.
  * @param  Alarm: Specifies the Alarm.
  *          This parameter can be one of the following values:
  *            @arg RTC_ALARM_A:  AlarmA
  *            @arg RTC_ALARM_B:  AlarmB
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTC_DeactivateAlarm(RTC_HandleTypeDef *hrtc, uint32_t Alarm)
{
  uint32_t tickstart = 0;
  
  /* Check the parameters */
  assert_param(IS_ALARM(Alarm));
  
  /* Process Locked */ 
  __HAL_LOCK(hrtc);
  
  hrtc->State = HAL_RTC_STATE_BUSY;
  
  /* Disable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_DISABLE(hrtc);
  
  if(Alarm == RTC_ALARM_A)
  {
    /* AlarmA */
    __HAL_RTC_ALARMA_DISABLE(hrtc);
    
    /* In case of interrupt mode is used, the interrupt source must disabled */ 
    __HAL_RTC_ALARM_DISABLE_IT(hrtc, RTC_IT_ALRA);

    /* Get tick */
    tickstart = HAL_GetTick();

    /* Wait till RTC ALRxWF flag is set and if Time out is reached exit */
    while(__HAL_RTC_ALARM_GET_FLAG(hrtc, RTC_FLAG_ALRAWF) == RESET)
    {
      if((HAL_GetTick() - tickstart ) > RTC_TIMEOUT_VALUE)
      { 
        /* Enable the write protection for RTC registers */
        __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);
        
        hrtc->State = HAL_RTC_STATE_TIMEOUT; 
        
        /* Process Unlocked */ 
        __HAL_UNLOCK(hrtc);
        
        return HAL_TIMEOUT;
      }      
    }
  }
  else
  {
    /* AlarmB */
    __HAL_RTC_ALARMB_DISABLE(hrtc);
    
    /* In case of interrupt mode is used, the interrupt source must disabled */ 
    __HAL_RTC_ALARM_DISABLE_IT(hrtc,RTC_IT_ALRB);

    /* Get tick */
    tickstart = HAL_GetTick();

    /* Wait till RTC ALRxWF flag is set and if Time out is reached exit */
    while(__HAL_RTC_ALARM_GET_FLAG(hrtc, RTC_FLAG_ALRBWF) == RESET)
    {
      if((HAL_GetTick() - tickstart ) > RTC_TIMEOUT_VALUE)
      {
        /* Enable the write protection for RTC registers */
        __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);
        
        hrtc->State = HAL_RTC_STATE_TIMEOUT; 
        
        /* Process Unlocked */ 
        __HAL_UNLOCK(hrtc);
        
        return HAL_TIMEOUT;
      }    
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
  * @brief  Gets the RTC Alarm value and masks.
  * @param  hrtc: pointer to a RTC_HandleTypeDef structure that contains
  *                the configuration information for RTC.
  * @param  sAlarm: Pointer to Date structure
  * @param  Alarm: Specifies the Alarm.
  *          This parameter can be one of the following values:
  *             @arg RTC_ALARM_A: AlarmA
  *             @arg RTC_ALARM_B: AlarmB  
  * @param  Format: Specifies the format of the entered parameters.
  *          This parameter can be one of the following values:
  *             @arg FORMAT_BIN: Binary data format 
  *             @arg FORMAT_BCD: BCD data format
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTC_GetAlarm(RTC_HandleTypeDef *hrtc, RTC_AlarmTypeDef *sAlarm, uint32_t Alarm, uint32_t Format)
{
  uint32_t tmpreg = 0, subsecondtmpreg = 0;
  
  /* Check the parameters */
  assert_param(IS_RTC_FORMAT(Format));
  assert_param(IS_ALARM(Alarm));
  
  if(Alarm == RTC_ALARM_A)
  {
    /* AlarmA */
    sAlarm->Alarm = RTC_ALARM_A;
    
    tmpreg = (uint32_t)(hrtc->Instance->ALRMAR);
    subsecondtmpreg = (uint32_t)((hrtc->Instance->ALRMASSR ) & RTC_ALRMASSR_SS);
  }
  else
  {
    sAlarm->Alarm = RTC_ALARM_B;
    
    tmpreg = (uint32_t)(hrtc->Instance->ALRMBR);
    subsecondtmpreg = (uint32_t)((hrtc->Instance->ALRMBSSR) & RTC_ALRMBSSR_SS);
  }
    
  /* Fill the structure with the read parameters */
  sAlarm->AlarmTime.Hours = (uint32_t)((tmpreg & (RTC_ALRMAR_HT | RTC_ALRMAR_HU)) >> 16);
  sAlarm->AlarmTime.Minutes = (uint32_t)((tmpreg & (RTC_ALRMAR_MNT | RTC_ALRMAR_MNU)) >> 8);
  sAlarm->AlarmTime.Seconds = (uint32_t)(tmpreg & (RTC_ALRMAR_ST | RTC_ALRMAR_SU));
  sAlarm->AlarmTime.TimeFormat = (uint32_t)((tmpreg & RTC_ALRMAR_PM) >> 16);
  sAlarm->AlarmTime.SubSeconds = (uint32_t) subsecondtmpreg;
  sAlarm->AlarmDateWeekDay = (uint32_t)((tmpreg & (RTC_ALRMAR_DT | RTC_ALRMAR_DU)) >> 24);
  sAlarm->AlarmDateWeekDaySel = (uint32_t)(tmpreg & RTC_ALRMAR_WDSEL);
  sAlarm->AlarmMask = (uint32_t)(tmpreg & RTC_ALARMMASK_ALL);
    
  if(Format == FORMAT_BIN)
  {
    sAlarm->AlarmTime.Hours = RTC_Bcd2ToByte(sAlarm->AlarmTime.Hours);
    sAlarm->AlarmTime.Minutes = RTC_Bcd2ToByte(sAlarm->AlarmTime.Minutes);
    sAlarm->AlarmTime.Seconds = RTC_Bcd2ToByte(sAlarm->AlarmTime.Seconds);
    sAlarm->AlarmDateWeekDay = RTC_Bcd2ToByte(sAlarm->AlarmDateWeekDay);
  }  
    
  return HAL_OK;
}

/**
  * @brief  This function handles Alarm interrupt request.
  * @param  hrtc: pointer to a RTC_HandleTypeDef structure that contains
  *                the configuration information for RTC.
  * @retval None
  */
void HAL_RTC_AlarmIRQHandler(RTC_HandleTypeDef* hrtc)
{  
  if(__HAL_RTC_ALARM_GET_IT(hrtc, RTC_IT_ALRA))
  {
    /* Get the status of the Interrupt */
    if((uint32_t)(hrtc->Instance->CR & RTC_IT_ALRA) != (uint32_t)RESET)
    {
      /* AlarmA callback */ 
      HAL_RTC_AlarmAEventCallback(hrtc);
      
      /* Clear the Alarm interrupt pending bit */
      __HAL_RTC_ALARM_CLEAR_FLAG(hrtc,RTC_FLAG_ALRAF);
    }
  }
  
  if(__HAL_RTC_ALARM_GET_IT(hrtc, RTC_IT_ALRB))
  {
    /* Get the status of the Interrupt */
    if((uint32_t)(hrtc->Instance->CR & RTC_IT_ALRB) != (uint32_t)RESET)
    {
      /* AlarmB callback */ 
      HAL_RTCEx_AlarmBEventCallback(hrtc);
      
      /* Clear the Alarm interrupt pending bit */
      __HAL_RTC_ALARM_CLEAR_FLAG(hrtc,RTC_FLAG_ALRBF);
    }
  }
  
  /* Clear the EXTI's line Flag for RTC Alarm */
  __HAL_RTC_EXTI_CLEAR_FLAG(RTC_EXTI_LINE_ALARM_EVENT);
  
  /* Change RTC state */
  hrtc->State = HAL_RTC_STATE_READY; 
}

/**
  * @brief  Alarm A callback.
  * @param  hrtc: pointer to a RTC_HandleTypeDef structure that contains
  *                the configuration information for RTC.
  * @retval None
  */
__weak void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_RTC_AlarmAEventCallback could be implemented in the user file
   */
}

/**
  * @brief  This function handles AlarmA Polling request.
  * @param  hrtc: pointer to a RTC_HandleTypeDef structure that contains
  *                the configuration information for RTC.
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTC_PollForAlarmAEvent(RTC_HandleTypeDef *hrtc, uint32_t Timeout)
{
  uint32_t tickstart = 0; 

    /* Get tick */
    tickstart = HAL_GetTick();

  while(__HAL_RTC_ALARM_GET_FLAG(hrtc, RTC_FLAG_ALRAF) == RESET)
  {
    if(Timeout != HAL_MAX_DELAY)
    {
      if((Timeout == 0)||((HAL_GetTick() - tickstart ) > Timeout))
      {
        hrtc->State = HAL_RTC_STATE_TIMEOUT;
        return HAL_TIMEOUT;
      }
    }
  }
  
  /* Clear the Alarm interrupt pending bit */
  __HAL_RTC_ALARM_CLEAR_FLAG(hrtc, RTC_FLAG_ALRAF);
  
  /* Change RTC state */
  hrtc->State = HAL_RTC_STATE_READY; 
  
  return HAL_OK;  
}

/**
  * @}
  */

/** @defgroup RTC_Group4 Peripheral Control functions 
 *  @brief   Peripheral Control functions 
 *
@verbatim   
 ===============================================================================
                     ##### Peripheral Control functions #####
 ===============================================================================  
    [..]
    This subsection provides functions allowing to
      (+) Wait for RTC Time and Date Synchronization

@endverbatim
  * @{
  */

/**
  * @brief  Waits until the RTC Time and Date registers (RTC_TR and RTC_DR) are 
  *         synchronized with RTC APB clock.
  * @note   The RTC Resynchronization mode is write protected, use the 
  *         __HAL_RTC_WRITEPROTECTION_DISABLE() before calling this function. 
  * @note   To read the calendar through the shadow registers after Calendar 
  *         initialization, calendar update or after wakeup from low power modes 
  *         the software must first clear the RSF flag. 
  *         The software must then wait until it is set again before reading 
  *         the calendar, which means that the calendar registers have been 
  *         correctly copied into the RTC_TR and RTC_DR shadow registers.   
  * @param  hrtc: pointer to a RTC_HandleTypeDef structure that contains
  *                the configuration information for RTC.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RTC_WaitForSynchro(RTC_HandleTypeDef* hrtc)
{
  uint32_t tickstart = 0;

  /* Clear RSF flag */
  hrtc->Instance->ISR &= (uint32_t)RTC_RSF_MASK;

    /* Get tick */
    tickstart = HAL_GetTick();

  /* Wait the registers to be synchronised */
  while((hrtc->Instance->ISR & RTC_ISR_RSF) == (uint32_t)RESET)
  {
    if((HAL_GetTick() - tickstart ) > RTC_TIMEOUT_VALUE)
    {       
      return HAL_TIMEOUT;
    } 
  }

  return HAL_OK;
}

/** @defgroup RTC_Group5 Peripheral State functions 
 *  @brief   Peripheral State functions 
 *
@verbatim   
 ===============================================================================
                     ##### Peripheral State functions #####
 ===============================================================================  
    [..]
    This subsection provides functions allowing to
      (+) Get RTC state

@endverbatim
  * @{
  */
/**
  * @brief  Returns the RTC state.
  * @param  hrtc: pointer to a RTC_HandleTypeDef structure that contains
  *                the configuration information for RTC.
  * @retval HAL state
  */
HAL_RTCStateTypeDef HAL_RTC_GetState(RTC_HandleTypeDef* hrtc)
{
  return hrtc->State;
}

/**
  * @}
  */

/**
  * @brief  Enters the RTC Initialization mode.
  * @note   The RTC Initialization mode is write protected, use the
  *         __HAL_RTC_WRITEPROTECTION_DISABLE() before calling this function.
  * @param  hrtc: pointer to a RTC_HandleTypeDef structure that contains
  *                the configuration information for RTC.
  * @retval HAL status
  */
HAL_StatusTypeDef RTC_EnterInitMode(RTC_HandleTypeDef* hrtc)
{
  uint32_t tickstart = 0; 
  
  /* Check if the Initialization mode is set */
  if((hrtc->Instance->ISR & RTC_ISR_INITF) == (uint32_t)RESET)
  {
    /* Set the Initialization mode */
    hrtc->Instance->ISR = (uint32_t)RTC_INIT_MASK;

    /* Get tick */
    tickstart = HAL_GetTick();

    /* Wait till RTC is in INIT state and if Time out is reached exit */
    while((hrtc->Instance->ISR & RTC_ISR_INITF) == (uint32_t)RESET)
    {
      if((HAL_GetTick() - tickstart ) > RTC_TIMEOUT_VALUE)
      {       
        return HAL_TIMEOUT;
      } 
    }
  }
  
  return HAL_OK;  
}


/**
  * @brief  Converts a 2 digit decimal to BCD format.
  * @param  Value: Byte to be converted
  * @retval Converted byte
  */
uint8_t RTC_ByteToBcd2(uint8_t Value)
{
  uint32_t bcdhigh = 0;
  
  while(Value >= 10)
  {
    bcdhigh++;
    Value -= 10;
  }
  
  return  ((uint8_t)(bcdhigh << 4) | Value);
}

/**
  * @brief  Converts from 2 digit BCD to Binary.
  * @param  Value: BCD value to be converted
  * @retval Converted word
  */
uint8_t RTC_Bcd2ToByte(uint8_t Value)
{
  uint32_t tmp = 0;
  tmp = ((uint8_t)(Value & (uint8_t)0xF0) >> (uint8_t)0x4) * 10;
  return (tmp + (Value & (uint8_t)0x0F));
}

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
