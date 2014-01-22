/**
  ******************************************************************************
  * @file    stm32fxxx_it.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-March-2012
  * @brief   Main Interrupt Service Routines.
  *          This file provides all exceptions handler and peripherals interrupt
  *          service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32fxxx_it.h"
#include "stm32f4xx_exti.h"
#include "usb_core.h"
//#include "usb_hcd_int.h" // for usb host mode only
//#include "usbd_core.h"

//#include "usbd_cdc_core.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern USB_OTG_CORE_HANDLE USB_OTG_dev;

/* Private function prototypes -----------------------------------------------*/
extern uint32_t USBD_OTG_ISR_Handler (USB_OTG_CORE_HANDLE *pdev);
extern uint32_t USBH_OTG_ISR_Handler (USB_OTG_CORE_HANDLE *pdev);
//extern uint32_t STM32_USBO_OTG_ISR_Handler (USB_OTG_CORE_HANDLE *pdev);

#ifdef USB_OTG_HS_DEDICATED_EP1_ENABLED 
extern uint32_t USBD_OTG_EP1IN_ISR_Handler (USB_OTG_CORE_HANDLE *pdev);
extern uint32_t USBD_OTG_EP1OUT_ISR_Handler (USB_OTG_CORE_HANDLE *pdev);
#endif

/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*             Cortex-M Processor Exceptions Handlers                         */
/******************************************************************************/

extern void fatality();

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
  fatality();
  while (1)
  {
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
  fatality();
  while (1)
  {
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
  fatality();
  while (1)
  {
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
  fatality();
  while (1)
  {
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
}

/**
  * @brief  This function handles EXTI15_10_IRQ Handler.
  * @param  None
  * @retval None
  */
#ifdef USE_USB_OTG_FS  
void OTG_FS_WKUP_IRQHandler(void)
{
  if(USB_OTG_dev.cfg.low_power)
  {
    *(uint32_t *)(0xE000ED10) &= 0xFFFFFFF9 ; 
    SystemInit();
#ifdef USE_DEVICE_MODE
    USB_OTG_UngateClock(&USB_OTG_dev);
#endif
  }
  EXTI_ClearITPendingBit(EXTI_Line18);
}
#endif

/**
  * @brief  This function handles EXTI15_10_IRQ Handler.
  * @param  None
  * @retval None
  */
#ifdef USE_USB_OTG_HS  
void OTG_HS_WKUP_IRQHandler(void)
{
  if(USB_OTG_dev.cfg.low_power)
  {
    *(uint32_t *)(0xE000ED10) &= 0xFFFFFFF9 ; 
    SystemInit();
    USB_OTG_UngateClock(&USB_OTG_dev);
  }
  EXTI_ClearITPendingBit(EXTI_Line20);
}
#endif

/**
  * @brief  This function handles OTG_HS Handler.
  * @param  None
  * @retval None
  */
#ifdef USE_USB_OTG_HS  
void OTG_HS_IRQHandler(void)
#else
void OTG_FS_IRQHandler(void)
#endif
{
    if (USB_OTG_IsHostMode(&USB_OTG_dev)) {
        // host mode
#ifdef USE_HOST_MODE
        USBH_OTG_ISR_Handler(&USB_OTG_dev);
#endif
        //STM32_USBO_OTG_ISR_Handler(&USB_OTG_dev); // USE_OTG_MODE
    } else {
        // device mode
#ifdef USE_DEVICE_MODE
        USBD_OTG_ISR_Handler(&USB_OTG_dev);
#endif
    }
}

#ifdef USB_OTG_HS_DEDICATED_EP1_ENABLED 
/**
  * @brief  This function handles EP1_IN Handler.
  * @param  None
  * @retval None
  */
void OTG_HS_EP1_IN_IRQHandler(void)
{
  USBD_OTG_EP1IN_ISR_Handler (&USB_OTG_dev);
}

/**
  * @brief  This function handles EP1_OUT Handler.
  * @param  None
  * @retval None
  */
void OTG_HS_EP1_OUT_IRQHandler(void)
{
  USBD_OTG_EP1OUT_ISR_Handler (&USB_OTG_dev);
}
#endif

/**
  * @brief  This function handles SDIO global interrupt request.
  * @param  None
  * @retval None
  */
void SDIO_IRQHandler(void)
{
  /* Process All SDIO Interrupt Sources */
  // dpgeorge: i don't think this is used at the moment...
  extern void SD_ProcessIRQSrc(void);
  SD_ProcessIRQSrc();
}

/******************************************************************************/
/*                 STM32Fxxx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32fxxx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

// TIM6 Update event
#include "stm32f4xx_tim.h"
void TIM6_DAC_IRQHandler(void) {
    // work out if it's TIM6 that had the interrupt
    if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET) {
        extern void timer_interrupt(void);
        timer_interrupt();
        TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
    } else {
        // it seems we get 2 calls to this interrupt handler, and only 1 is the TIM_IT_Update...
        // TODO work out what the other one is, and if we can disable it
        //printf("unhandled TIM6_DAC\n");
    }
}

#include "std.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "led.h"
// EXTI
// for USRSW on A13
// for cc3000 on A14
void EXTI15_10_IRQHandler(void) {
    // work out if it's A13 that had the interrupt
    if (EXTI_GetITStatus(EXTI_Line13) != RESET) {
        // this is used just to wake the device
        EXTI_ClearITPendingBit(EXTI_Line13);
    }
    // work out if it's A14 that had the interrupt
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
}

#if defined(STM32F4DISC)
void EXTI0_IRQHandler(void) {
    // clear pending interrupt bit
    EXTI_ClearITPendingBit(EXTI_Line0);
}
#endif
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
