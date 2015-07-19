/**
  ******************************************************************************
  * @file    stm32f4xx_hal_pwr.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-June-2014
  * @brief   PWR HAL module driver.
  *          This file provides firmware functions to manage the following 
  *          functionalities of the Power Controller (PWR) peripheral:
  *           + Initialization and de-initialization functions
  *           + Peripheral Control functions 
  *         
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

/** @defgroup PWR 
  * @brief PWR HAL module driver
  * @{
  */

#ifdef HAL_PWR_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup PWR_Private_Functions
  * @{
  */

/** @defgroup PWR_Group1 Initialization and de-initialization functions 
  *  @brief    Initialization and de-initialization functions
  *
@verbatim
 ===============================================================================
              ##### Initialization and de-initialization functions #####
 ===============================================================================
    [..]
      After reset, the backup domain (RTC registers, RTC backup data 
      registers and backup SRAM) is protected against possible unwanted 
      write accesses. 
      To enable access to the RTC Domain and RTC registers, proceed as follows:
        (+) Enable the Power Controller (PWR) APB1 interface clock using the
            __PWR_CLK_ENABLE() macro.
        (+) Enable access to RTC domain using the HAL_PWR_EnableBkUpAccess() function.
 
@endverbatim
  * @{
  */

/**
  * @brief Deinitializes the HAL PWR peripheral registers to their default reset values.
  * @param None
  * @retval None
  */
void HAL_PWR_DeInit(void)
{
  __PWR_FORCE_RESET();
  __PWR_RELEASE_RESET();
}

/**
  * @brief Enables access to the backup domain (RTC registers, RTC 
  *         backup data registers and backup SRAM).
  * @note If the HSE divided by 2, 3, ..31 is used as the RTC clock, the 
  *         Backup Domain Access should be kept enabled.
  * @param None
  * @retval None
  */
void HAL_PWR_EnableBkUpAccess(void)
{
  *(__IO uint32_t *) CR_DBP_BB = (uint32_t)ENABLE;
}

/**
  * @brief Disables access to the backup domain (RTC registers, RTC 
  *         backup data registers and backup SRAM).
  * @note If the HSE divided by 2, 3, ..31 is used as the RTC clock, the 
  *         Backup Domain Access should be kept enabled.
  * @param None
  * @retval None
  */
void HAL_PWR_DisableBkUpAccess(void)
{
  *(__IO uint32_t *) CR_DBP_BB = (uint32_t)DISABLE;
}

/**
  * @}
  */

/** @defgroup PWR_Group2 Peripheral Control functions 
  *  @brief Low Power modes configuration functions 
  *
@verbatim

 ===============================================================================
                 ##### Peripheral Control functions #####
 ===============================================================================
     
    *** PVD configuration ***
    =========================
    [..]
      (+) The PVD is used to monitor the VDD power supply by comparing it to a 
          threshold selected by the PVD Level (PLS[2:0] bits in the PWR_CR).
      (+) A PVDO flag is available to indicate if VDD/VDDA is higher or lower 
          than the PVD threshold. This event is internally connected to the EXTI 
          line16 and can generate an interrupt if enabled. This is done through
          __HAL_PVD_EXTI_ENABLE_IT() macro.
      (+) The PVD is stopped in Standby mode.

    *** WakeUp pin configuration ***
    ================================
    [..]
      (+) WakeUp pin is used to wake up the system from Standby mode. This pin is 
          forced in input pull-down configuration and is active on rising edges.
      (+) There is only one WakeUp pin: WakeUp Pin 1 on PA.00.

    *** Low Power modes configuration ***
    =====================================
    [..]
      The devices feature 3 low-power modes:
      (+) Sleep mode: Cortex-M4 core stopped, peripherals kept running.
      (+) Stop mode: all clocks are stopped, regulator running, regulator 
          in low power mode
      (+) Standby mode: 1.2V domain powered off.
   
   *** Sleep mode ***
   ==================
    [..]
      (+) Entry:
        The Sleep mode is entered by using the HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI)
              functions with
          (++) PWR_SLEEPENTRY_WFI: enter SLEEP mode with WFI instruction
          (++) PWR_SLEEPENTRY_WFE: enter SLEEP mode with WFE instruction
      
      -@@- The Regulator parameter is not used for the STM32F4 family 
              and is kept as parameter just to maintain compatibility with the 
              lower power families (STM32L).
      (+) Exit:
        Any peripheral interrupt acknowledged by the nested vectored interrupt 
              controller (NVIC) can wake up the device from Sleep mode.

   *** Stop mode ***
   =================
    [..]
      In Stop mode, all clocks in the 1.2V domain are stopped, the PLL, the HSI,
      and the HSE RC oscillators are disabled. Internal SRAM and register contents 
      are preserved.
      The voltage regulator can be configured either in normal or low-power mode.
      To minimize the consumption In Stop mode, FLASH can be powered off before 
      entering the Stop mode using the HAL_PWR_EnableFlashPowerDown() function.
      It can be switched on again by software after exiting the Stop mode using
      the HAL_PWR_DisableFlashPowerDown() function. 

      (+) Entry:
         The Stop mode is entered using the HAL_PWR_EnterSTOPMode(PWR_MAINREGULATOR_ON) 
             function with:
          (++) Main regulator ON.
          (++) Low Power regulator ON.
      (+) Exit:
        Any EXTI Line (Internal or External) configured in Interrupt/Event mode.

   *** Standby mode ***
   ====================
    [..]
    (+)
      The Standby mode allows to achieve the lowest power consumption. It is based 
      on the Cortex-M4 deep sleep mode, with the voltage regulator disabled. 
      The 1.2V domain is consequently powered off. The PLL, the HSI oscillator and 
      the HSE oscillator are also switched off. SRAM and register contents are lost 
      except for the RTC registers, RTC backup registers, backup SRAM and Standby 
      circuitry.
   
      The voltage regulator is OFF.
      
      (++) Entry:
        (+++) The Standby mode is entered using the HAL_PWR_EnterSTANDBYMode() function.
      (++) Exit:
        (+++) WKUP pin rising edge, RTC alarm (Alarm A and Alarm B), RTC wakeup,
             tamper event, time-stamp event, external reset in NRST pin, IWDG reset.

   *** Auto-wakeup (AWU) from low-power mode ***
   =============================================
    [..]
    
     (+) The MCU can be woken up from low-power mode by an RTC Alarm event, an RTC 
      Wakeup event, a tamper event or a time-stamp event, without depending on 
      an external interrupt (Auto-wakeup mode).

      (+) RTC auto-wakeup (AWU) from the Stop and Standby modes
       
        (++) To wake up from the Stop mode with an RTC alarm event, it is necessary to 
              configure the RTC to generate the RTC alarm using the HAL_RTC_SetAlarm_IT() function.

        (++) To wake up from the Stop mode with an RTC Tamper or time stamp event, it 
             is necessary to configure the RTC to detect the tamper or time stamp event using the
                HAL_RTCEx_SetTimeStamp_IT() or HAL_RTCEx_SetTamper_IT() functions.
                  
        (++) To wake up from the Stop mode with an RTC WakeUp event, it is necessary to
              configure the RTC to generate the RTC WakeUp event using the HAL_RTCEx_SetWakeUpTimer_IT() function.

@endverbatim
  * @{
  */

/**
  * @brief Configures the voltage threshold detected by the Power Voltage Detector(PVD).
  * @param sConfigPVD: pointer to an PWR_PVDTypeDef structure that contains the configuration
  *        information for the PVD.
  * @note Refer to the electrical characteristics of your device datasheet for
  *         more details about the voltage threshold corresponding to each 
  *         detection level.
  * @retval None
  */
void HAL_PWR_PVDConfig(PWR_PVDTypeDef *sConfigPVD)
{
  uint32_t tmpreg = 0;

  /* Check the parameters */
  assert_param(IS_PWR_PVD_LEVEL(sConfigPVD->PVDLevel));
  assert_param(IS_PWR_PVD_MODE(sConfigPVD->Mode));
  
  tmpreg = PWR->CR;
  
  /* Clear PLS[7:5] bits */
  tmpreg &= ~ (uint32_t)PWR_CR_PLS;
  
  /* Set PLS[7:5] bits according to PVDLevel value */
  tmpreg |= sConfigPVD->PVDLevel;
  
  /* Store the new value */
  PWR->CR = tmpreg;
  
  /* Configure the EXTI 16 interrupt */
  if((sConfigPVD->Mode == PWR_MODE_IT_RISING_FALLING) ||\
     (sConfigPVD->Mode == PWR_MODE_IT_FALLING) ||\
     (sConfigPVD->Mode == PWR_MODE_IT_RISING)) 
  {
    __HAL_PVD_EXTI_ENABLE_IT(PWR_EXTI_LINE_PVD);
  }
  /* Clear the edge trigger  for the EXTI Line 16 (PVD) */
  EXTI->RTSR &= ~EXTI_RTSR_TR16;
  EXTI->FTSR &= ~EXTI_FTSR_TR16;  
  /* Configure the rising edge */
  if((sConfigPVD->Mode == PWR_MODE_IT_RISING_FALLING) ||\
     (sConfigPVD->Mode == PWR_MODE_IT_RISING))
  {
    EXTI->RTSR |= PWR_EXTI_LINE_PVD;
  }
  /* Configure the falling edge */
  if((sConfigPVD->Mode == PWR_MODE_IT_RISING_FALLING) ||\
     (sConfigPVD->Mode == PWR_MODE_IT_FALLING))
  {
    EXTI->FTSR |= PWR_EXTI_LINE_PVD;
  }
}

/**
  * @brief Enables the Power Voltage Detector(PVD).
  * @param None
  * @retval None
  */
void HAL_PWR_EnablePVD(void)
{
  *(__IO uint32_t *) CR_PVDE_BB = (uint32_t)ENABLE;
}

/**
  * @brief Disables the Power Voltage Detector(PVD).
  * @param None
  * @retval None
  */
void HAL_PWR_DisablePVD(void)
{
  *(__IO uint32_t *) CR_PVDE_BB = (uint32_t)DISABLE;
}

/**
  * @brief Enables the WakeUp PINx functionality.
  * @param WakeUpPinx: Specifies the Power Wake-Up pin to enable.
  *         This parameter can be one of the following values:
  *           @arg PWR_WAKEUP_PIN1
  * @retval None
  */
void HAL_PWR_EnableWakeUpPin(uint32_t WakeUpPinx)
{
  /* Check the parameter */
  assert_param(IS_PWR_WAKEUP_PIN(WakeUpPinx));
  *(__IO uint32_t *) CSR_EWUP_BB = (uint32_t)ENABLE;
}

/**
  * @brief Disables the WakeUp PINx functionality.
  * @param WakeUpPinx: Specifies the Power Wake-Up pin to disable.
  *         This parameter can be one of the following values:
  *           @arg PWR_WAKEUP_PIN1
  * @retval None
  */
void HAL_PWR_DisableWakeUpPin(uint32_t WakeUpPinx)
{
  /* Check the parameter */
  assert_param(IS_PWR_WAKEUP_PIN(WakeUpPinx));  
  *(__IO uint32_t *) CSR_EWUP_BB = (uint32_t)DISABLE;
}
  
/**
  * @brief Enters Sleep mode.
  *   
  * @note In Sleep mode, all I/O pins keep the same state as in Run mode.
  * 
  * @note In Sleep mode, the systick is stopped to avoid exit from this mode with
  *       systick interrupt when used as time base for Timeout 
  *                
  * @param Regulator: Specifies the regulator state in SLEEP mode.
  *            This parameter can be one of the following values:
  *            @arg PWR_MAINREGULATOR_ON: SLEEP mode with regulator ON
  *            @arg PWR_LOWPOWERREGULATOR_ON: SLEEP mode with low power regulator ON
  * @note This parameter is not used for the STM32F4 family and is kept as parameter
  *       just to maintain compatibility with the lower power families.
  * @param SLEEPEntry: Specifies if SLEEP mode in entered with WFI or WFE instruction.
  *          This parameter can be one of the following values:
  *            @arg PWR_SLEEPENTRY_WFI: enter SLEEP mode with WFI instruction
  *            @arg PWR_SLEEPENTRY_WFE: enter SLEEP mode with WFE instruction
  * @retval None
  */
void HAL_PWR_EnterSLEEPMode(uint32_t Regulator, uint8_t SLEEPEntry)
{
  /* Check the parameters */
  assert_param(IS_PWR_REGULATOR(Regulator));
  assert_param(IS_PWR_SLEEP_ENTRY(SLEEPEntry));

  /* Select SLEEP mode entry -------------------------------------------------*/
  if(SLEEPEntry == PWR_SLEEPENTRY_WFI)
  {   
    /* Request Wait For Interrupt */
    __WFI();
  }
  else
  {
    /* Request Wait For Event */
    __SEV();
    __WFE();
    __WFE();
  }
}

/**
  * @brief Enters Stop mode. 
  * @note In Stop mode, all I/O pins keep the same state as in Run mode.
  * @note When exiting Stop mode by issuing an interrupt or a wakeup event, 
  *         the HSI RC oscillator is selected as system clock.
  * @note When the voltage regulator operates in low power mode, an additional 
  *         startup delay is incurred when waking up from Stop mode. 
  *         By keeping the internal regulator ON during Stop mode, the consumption 
  *         is higher although the startup time is reduced.    
  * @param Regulator: Specifies the regulator state in Stop mode.
  *          This parameter can be one of the following values:
  *            @arg PWR_MAINREGULATOR_ON: Stop mode with regulator ON
  *            @arg PWR_LOWPOWERREGULATOR_ON: Stop mode with low power regulator ON
  * @param STOPEntry: Specifies if Stop mode in entered with WFI or WFE instruction.
  *          This parameter can be one of the following values:
  *            @arg PWR_STOPENTRY_WFI: Enter Stop mode with WFI instruction
  *            @arg PWR_STOPENTRY_WFE: Enter Stop mode with WFE instruction
  * @retval None
  */
void HAL_PWR_EnterSTOPMode(uint32_t Regulator, uint8_t STOPEntry)
{
  uint32_t tmpreg = 0;
  
  /* Check the parameters */
  assert_param(IS_PWR_REGULATOR(Regulator));
  assert_param(IS_PWR_STOP_ENTRY(STOPEntry));
  
  /* Select the regulator state in Stop mode ---------------------------------*/
  tmpreg = PWR->CR;
  /* Clear PDDS and LPDS bits */
  tmpreg &= (uint32_t)~(PWR_CR_PDDS | PWR_CR_LPDS);
  
  /* Set LPDS, MRLVDS and LPLVDS bits according to Regulator value */
  tmpreg |= Regulator;
  
  /* Store the new value */
  PWR->CR = tmpreg;
  
  /* Set SLEEPDEEP bit of Cortex System Control Register */
  SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
  
  /* Select Stop mode entry --------------------------------------------------*/
  if(STOPEntry == PWR_STOPENTRY_WFI)
  {   
    /* Request Wait For Interrupt */
    __WFI();
  }
  else
  {
    /* Request Wait For Event */
    __WFE();
  }
  /* Reset SLEEPDEEP bit of Cortex System Control Register */
  SCB->SCR &= (uint32_t)~((uint32_t)SCB_SCR_SLEEPDEEP_Msk);  
}

/**
  * @brief Enters Standby mode.
  * @note In Standby mode, all I/O pins are high impedance except for:
  *          - Reset pad (still available) 
  *          - RTC_AF1 pin (PC13) if configured for tamper, time-stamp, RTC 
  *            Alarm out, or RTC clock calibration out.
  *          - RTC_AF2 pin (PI8) if configured for tamper or time-stamp.  
  *          - WKUP pin 1 (PA0) if enabled.       
  * @param None
  * @retval None
  */
void HAL_PWR_EnterSTANDBYMode(void)
{
  /* Select Standby mode */
  PWR->CR |= PWR_CR_PDDS;
  
  /* Set SLEEPDEEP bit of Cortex System Control Register */
  SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
  
  /* This option is used to ensure that store operations are completed */
#if defined ( __CC_ARM)
  __force_stores();
#endif
  /* Request Wait For Interrupt */
  __WFI();
}

/**
  * @brief This function handles the PWR PVD interrupt request.
  * @note This API should be called under the PVD_IRQHandler().
  * @param None
  * @retval None
  */
void HAL_PWR_PVD_IRQHandler(void)
{
  /* Check PWR exti flag */
  if(__HAL_PVD_EXTI_GET_FLAG(PWR_EXTI_LINE_PVD) != RESET)
  {
    /* PWR PVD interrupt user callback */
    HAL_PWR_PVDCallback();
    
    /* Clear PWR Exti pending bit */
    __HAL_PVD_EXTI_CLEAR_FLAG(PWR_EXTI_LINE_PVD);
  }
}

/**
  * @brief  PWR PVD interrupt callback
  * @param  None 
  * @retval None
  */
__weak void HAL_PWR_PVDCallback(void)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_PWR_PVDCallback could be implemented in the user file
   */ 
}

/**
  * @}
  */
  
/**
  * @}
  */

#endif /* HAL_PWR_MODULE_ENABLED */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
