/**
  ******************************************************************************
  * @file    Templates/Src/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    26-February-2014
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
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
#include <stdio.h>

#include "stm32f4xx_it.h"
#include "stm32f4xx_hal.h"

#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "exti.h"
#include "timer.h"

/** @addtogroup STM32F4xx_HAL_Examples
  * @{
  */

/** @addtogroup Templates
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

extern void fatality();
extern PCD_HandleTypeDef hpcd;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
    fatality();
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
    fatality();
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
    fatality();
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
    fatality();
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
    extern void pendsv_isr_handler(void);
    pendsv_isr_handler();
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
    HAL_IncTick();
}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles USB-On-The-Go FS global interrupt request.
  * @param  None
  * @retval None
  */
#if defined(USE_USB_FS)
#define OTG_XX_IRQHandler      OTG_FS_IRQHandler
#define OTG_XX_WKUP_IRQHandler OTG_FS_WKUP_IRQHandler
#elif defined(USE_USB_HS)
#define OTG_XX_IRQHandler      OTG_HS_IRQHandler
#define OTG_XX_WKUP_IRQHandler OTG_HS_WKUP_IRQHandler
#endif

#if defined(OTG_XX_IRQHandler)
void OTG_XX_IRQHandler(void)
{
    HAL_PCD_IRQHandler(&hpcd);
}
#endif

/**
  * @brief  This function handles USB OTG FS or HS Wakeup IRQ Handler.
  * @param  None
  * @retval None
  */
#if defined(OTG_XX_WKUP_IRQHandler)
void OTG_XX_WKUP_IRQHandler(void)
{
 
  if((&hpcd)->Init.low_power_enable)
  {
    /* Reset SLEEPDEEP bit of Cortex System Control Register */
    SCB->SCR &= (uint32_t)~((uint32_t)(SCB_SCR_SLEEPDEEP_Msk | SCB_SCR_SLEEPONEXIT_Msk));  
    
    /* Configures system clock after wake-up from STOP: enable HSE, PLL and select 
    PLL as system clock source (HSE and PLL are disabled in STOP mode) */
    
    __HAL_RCC_HSE_CONFIG(RCC_HSE_ON);
    
    /* Wait till HSE is ready */  
    while(__HAL_RCC_GET_FLAG(RCC_FLAG_HSERDY) == RESET)
    {}
    
    /* Enable the main PLL. */
    __HAL_RCC_PLL_ENABLE();
    
    /* Wait till PLL is ready */  
    while(__HAL_RCC_GET_FLAG(RCC_FLAG_PLLRDY) == RESET)
    {}
    
    /* Select PLL as SYSCLK */
    MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, RCC_SYSCLKSOURCE_PLLCLK);
    
    while (__HAL_RCC_GET_SYSCLK_SOURCE() != RCC_CFGR_SWS_PLL)
    {}
    
    /* ungate PHY clock */
     __HAL_PCD_UNGATE_PHYCLOCK((&hpcd)); 
  }
#ifdef USE_USB_FS
  /* Clear EXTI pending Bit*/
  __HAL_USB_FS_EXTI_CLEAR_FLAG();
#elif defined(USE_USB_HS)
    /* Clear EXTI pending Bit*/
  __HAL_USB_HS_EXTI_CLEAR_FLAG();
#endif
  
}
#endif

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @brief  These functions handle the EXTI interrupt requests.
  * @param  None
  * @retval None
  */
void EXTI0_IRQHandler(void) {
    Handle_EXTI_Irq(0);
}

void EXTI1_IRQHandler(void) {
    Handle_EXTI_Irq(1);
}

void EXTI2_IRQHandler(void) {
    Handle_EXTI_Irq(2);
}

void EXTI3_IRQHandler(void) {
    Handle_EXTI_Irq(3);
}

void EXTI4_IRQHandler(void) {
    Handle_EXTI_Irq(4);
}

void EXTI9_5_IRQHandler(void) {
    Handle_EXTI_Irq(5);
    Handle_EXTI_Irq(6);
    Handle_EXTI_Irq(7);
    Handle_EXTI_Irq(8);
    Handle_EXTI_Irq(9);
}

void EXTI15_10_IRQHandler(void) {
    Handle_EXTI_Irq(10);
    Handle_EXTI_Irq(11);
    Handle_EXTI_Irq(12);
    Handle_EXTI_Irq(13);
    Handle_EXTI_Irq(14);
    Handle_EXTI_Irq(15);

#if 0
    // for CC3000 support, needs to be re-written to use new EXTI code 
    if (EXTI_GetITStatus(EXTI_Line14) != RESET) {
        led_toggle(PYB_LED_G2);
        /* these are needed for CC3000 support
        extern void SpiIntGPIOHandler(void);
        extern uint32_t exti14_enabled;
        extern uint32_t exti14_missed;
        //printf("-> EXTI14 en=%lu miss=%lu\n", exti14_enabled, exti14_missed);
        if (exti14_enabled) {
            exti14_missed = 0;
            SpiIntGPIOHandler(); // CC3000 interrupt
        } else {
            exti14_missed = 1;
        }
        */
        EXTI_ClearITPendingBit(EXTI_Line14);
        //printf("<- EXTI14 done\n");
    }
#endif
}

void PVD_IRQHandler(void) {
    Handle_EXTI_Irq(EXTI_PVD_OUTPUT);
}

void RTC_Alarm_IRQHandler(void) {
    Handle_EXTI_Irq(EXTI_RTC_ALARM);
}


#if defined(ETH)    // The 407 has ETH, the 405 doesn't
void ETH_WKUP_IRQHandler(void)  {
    Handle_EXTI_Irq(EXTI_ETH_WAKEUP);
}
#endif

void TAMP_STAMP_IRQHandler(void) {
    Handle_EXTI_Irq(EXTI_RTC_TIMESTAMP);
}

void RTC_WKUP_IRQHandler(void) {
    Handle_EXTI_Irq(EXTI_RTC_WAKEUP);
}

void TIM3_IRQHandler(void) {
    HAL_TIM_IRQHandler(&TIM3_Handle);
}

void TIM5_IRQHandler(void) {
    HAL_TIM_IRQHandler(&TIM5_Handle);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
