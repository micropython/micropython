/**
  ******************************************************************************
  * @file    stm32f4xx_hal_pcd.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    18-February-2014
  * @brief   Header file of PCD HAL module.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F4xx_HAL_PCD_H
#define __STM32F4xx_HAL_PCD_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_ll_usb.h"
   
/** @addtogroup STM32F4xx_HAL_Driver
  * @{
  */

/** @addtogroup PCD
  * @{
  */ 

/* Exported types ------------------------------------------------------------*/ 

   /** 
  * @brief  PCD State structures definition  
  */  
typedef enum 
{
  PCD_READY    = 0x00,
  PCD_ERROR    = 0x01,
  PCD_BUSY     = 0x02,
  PCD_TIMEOUT  = 0x03
} PCD_StateTypeDef;


typedef USB_OTG_GlobalTypeDef  PCD_TypeDef;
typedef USB_OTG_CfgTypeDef     PCD_InitTypeDef;
typedef USB_OTG_EPTypeDef      PCD_EPTypeDef ;                          

/** 
  * @brief  PCD Handle Structure definition  
  */ 
typedef struct
{
  PCD_TypeDef             *Instance;   /*!< Register base address              */ 
  PCD_InitTypeDef         Init;       /*!< PCD required parameters            */
  PCD_EPTypeDef           IN_ep[15];  /*!< IN endpoint parameters             */
  PCD_EPTypeDef           OUT_ep[15]; /*!< OUT endpoint parameters            */ 
  HAL_LockTypeDef         Lock;       /*!< PCD peripheral status              */
  __IO PCD_StateTypeDef   State;      /*!< PCD communication state            */
  uint32_t                Setup[12];  /*!< Setup packet buffer                */
  void                    *pData;      /*!< Pointer to upper stack Handler     */    
  
} PCD_HandleTypeDef;
  
/* Exported constants --------------------------------------------------------*/
/** @defgroup PCD_Exported_Constants
  * @{
  */

/** @defgroup PCD_Speed
  * @{
  */
#define PCD_SPEED_HIGH               0
#define PCD_SPEED_HIGH_IN_FULL       1
#define PCD_SPEED_FULL               2
/**
  * @}
  */
  
  /** @defgroup PCD_PHY_Module
  * @{
  */
#define PCD_PHY_ULPI                 1
#define PCD_PHY_EMBEDDED             2
/**
  * @}
  */
  
/** @defgroup PCD_Instance_definition 
  * @{
  */ 
#if defined(STM32F405xx) || defined(STM32F415xx) || defined(STM32F407xx) || defined(STM32F417xx) || defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx)
 #define IS_PCD_ALL_INSTANCE(INSTANCE) (((INSTANCE) == USB_OTG_FS) || \
                                        ((INSTANCE) == USB_OTG_HS))
#elif defined(STM32F401xC) || defined(STM32F401xE)
 #define IS_PCD_ALL_INSTANCE(INSTANCE) (((INSTANCE) == USB_OTG_FS))
#endif

/**
  * @}
  */

/**
  * @}
  */ 
  
/* Exported macro ------------------------------------------------------------*/

/** @defgroup PCD_Interrupt_Clock
 *  @brief macros to handle interrupts and specific clock configurations
 * @{
 */
#define __HAL_PCD_ENABLE(__HANDLE__)                   USB_EnableGlobalInt ((__HANDLE__)->Instance)
#define __HAL_PCD_DISABLE(__HANDLE__)                  USB_DisableGlobalInt ((__HANDLE__)->Instance)
   
#define __HAL_GET_FLAG(__HANDLE__, __INTERRUPT__)      ((USB_ReadInterrupts((__HANDLE__)->Instance) & (__INTERRUPT__)) == (__INTERRUPT__))
#define __HAL_CLEAR_FLAG(__HANDLE__, __INTERRUPT__)    (((__HANDLE__)->Instance->GINTSTS) |= (__INTERRUPT__))
#define __HAL_IS_INVALID_INTERRUPT(__HANDLE__)         (USB_ReadInterrupts((__HANDLE__)->Instance) == 0)


#define __HAL_PCD_UNGATE_PHYCLOCK(__HANDLE__)             *(__IO uint32_t *)((uint32_t)((__HANDLE__)->Instance) + USB_OTG_PCGCCTL_BASE) &= \
                                                       ~(USB_OTG_PCGCCTL_STOPCLK)


#define __HAL_PCD_GATE_PHYCLOCK(__HANDLE__)               *(__IO uint32_t *)((uint32_t)((__HANDLE__)->Instance) + USB_OTG_PCGCCTL_BASE) |= USB_OTG_PCGCCTL_STOPCLK
                                                      
#define __HAL_PCD_IS_PHY_SUSPENDED(__HANDLE__)            ((*(__IO uint32_t *)((uint32_t)((__HANDLE__)->Instance) + USB_OTG_PCGCCTL_BASE))&0x10)
                                                         
#define USB_FS_EXTI_TRIGGER_RISING_EDGE      ((uint32_t)0x08) 
#define USB_FS_EXTI_TRIGGER_FALLING_EDGE     ((uint32_t)0x0C) 
#define USB_FS_EXTI_TRIGGER_BOTH_EDGE        ((uint32_t)0x10) 

#define USB_HS_EXTI_TRIGGER_RISING_EDGE      ((uint32_t)0x08) 
#define USB_HS_EXTI_TRIGGER_FALLING_EDGE     ((uint32_t)0x0C) 
#define USB_HS_EXTI_TRIGGER_BOTH_EDGE        ((uint32_t)0x10) 


#define USB_HS_EXTI_LINE_WAKEUP              ((uint32_t)0x00100000)  /*!< External interrupt line 20 Connected to the USB HS EXTI Line */
#define USB_FS_EXTI_LINE_WAKEUP              ((uint32_t)0x00040000)  /*!< External interrupt line 18 Connected to the USB FS EXTI Line */



#define __HAL_USB_HS_EXTI_ENABLE_IT()    EXTI->IMR |= (USB_HS_EXTI_LINE_WAKEUP)
#define __HAL_USB_HS_EXTI_DISABLE_IT()   EXTI->IMR &= ~(USB_HS_EXTI_LINE_WAKEUP)
#define __HAL_USB_HS_EXTI_GET_FLAG()     EXTI->PR & (USB_HS_EXTI_LINE_WAKEUP)
#define __HAL_USB_HS_EXTI_CLEAR_FLAG()   EXTI->PR = (USB_HS_EXTI_LINE_WAKEUP)

#define __HAL_USB_HS_EXTI_SET_RISING_EGDE_TRIGGER() EXTI->FTSR &= ~(USB_HS_EXTI_LINE_WAKEUP);\
                                                    EXTI->RTSR |= USB_HS_EXTI_LINE_WAKEUP

                                                      
#define __HAL_USB_HS_EXTI_SET_FALLING_EGDE_TRIGGER()  EXTI->FTSR |= (USB_HS_EXTI_LINE_WAKEUP);\
                                                      EXTI->RTSR &= ~(USB_HS_EXTI_LINE_WAKEUP)


#define __HAL_USB_HS_EXTI_SET_FALLINGRISING_TRIGGER()   EXTI->RTSR &= ~(USB_HS_EXTI_LINE_WAKEUP);\
                                                        EXTI->FTSR &= ~(USB_HS_EXTI_LINE_WAKEUP;)\
                                                        EXTI->RTSR |= USB_HS_EXTI_LINE_WAKEUP;\
                                                        EXTI->FTSR |= USB_HS_EXTI_LINE_WAKEUP


#define __HAL_USB_FS_EXTI_ENABLE_IT()    EXTI->IMR |= USB_FS_EXTI_LINE_WAKEUP
#define __HAL_USB_FS_EXTI_DISABLE_IT()   EXTI->IMR &= ~(USB_FS_EXTI_LINE_WAKEUP)
#define __HAL_USB_FS_EXTI_GET_FLAG()     EXTI->PR & (USB_FS_EXTI_LINE_WAKEUP)
#define __HAL_USB_FS_EXTI_CLEAR_FLAG()   EXTI->PR = USB_FS_EXTI_LINE_WAKEUP

#define __HAL_USB_FS_EXTI_SET_RISING_EGDE_TRIGGER() EXTI->FTSR &= ~(USB_FS_EXTI_LINE_WAKEUP);\
                                                    EXTI->RTSR |= USB_FS_EXTI_LINE_WAKEUP

                                                      
#define __HAL_USB_FS_EXTI_SET_FALLING_EGDE_TRIGGER()  EXTI->FTSR |= (USB_FS_EXTI_LINE_WAKEUP);\
                                                      EXTI->RTSR &= ~(USB_FS_EXTI_LINE_WAKEUP)


#define __HAL_USB_FS_EXTI_SET_FALLINGRISING_TRIGGER()  EXTI->RTSR &= ~(USB_FS_EXTI_LINE_WAKEUP);\
                                                       EXTI->FTSR &= ~(USB_FS_EXTI_LINE_WAKEUP);\
                                                       EXTI->RTSR |= USB_FS_EXTI_LINE_WAKEUP;\
                                                       EXTI->FTSR |= USB_FS_EXTI_LINE_WAKEUP  

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/

/* Initialization/de-initialization functions  **********************************/
HAL_StatusTypeDef HAL_PCD_Init(PCD_HandleTypeDef *hpcd);
HAL_StatusTypeDef HAL_PCD_DeInit (PCD_HandleTypeDef *hpcd);
void HAL_PCD_MspInit(PCD_HandleTypeDef *hpcd);
void HAL_PCD_MspDeInit(PCD_HandleTypeDef *hpcd);

/* I/O operation functions  *****************************************************/
 /* Non-Blocking mode: Interrupt */
HAL_StatusTypeDef HAL_PCD_Start(PCD_HandleTypeDef *hpcd);
HAL_StatusTypeDef HAL_PCD_Stop(PCD_HandleTypeDef *hpcd);
void HAL_PCD_IRQHandler(PCD_HandleTypeDef *hpcd);

void HAL_PCD_DataOutStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum);
void HAL_PCD_DataInStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum);
void HAL_PCD_SetupStageCallback(PCD_HandleTypeDef *hpcd);
void HAL_PCD_SOFCallback(PCD_HandleTypeDef *hpcd);
void HAL_PCD_ResetCallback(PCD_HandleTypeDef *hpcd);
void HAL_PCD_SuspendCallback(PCD_HandleTypeDef *hpcd);
void HAL_PCD_ResumeCallback(PCD_HandleTypeDef *hpcd);
void HAL_PCD_ISOOUTIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum);
void HAL_PCD_ISOINIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum);
void HAL_PCD_ConnectCallback(PCD_HandleTypeDef *hpcd);
void HAL_PCD_DisconnectCallback(PCD_HandleTypeDef *hpcd);



/* Peripheral Control functions  ************************************************/
HAL_StatusTypeDef HAL_PCD_DevConnect(PCD_HandleTypeDef *hpcd);
HAL_StatusTypeDef HAL_PCD_DevDisconnect(PCD_HandleTypeDef *hpcd);
HAL_StatusTypeDef HAL_PCD_SetAddress(PCD_HandleTypeDef *hpcd, uint8_t address);
HAL_StatusTypeDef HAL_PCD_EP_Open(PCD_HandleTypeDef *hpcd, uint8_t ep_addr, uint16_t ep_mps, uint8_t ep_type);
HAL_StatusTypeDef HAL_PCD_EP_Close(PCD_HandleTypeDef *hpcd, uint8_t ep_addr);
HAL_StatusTypeDef HAL_PCD_EP_Receive(PCD_HandleTypeDef *hpcd, uint8_t ep_addr, uint8_t *pBuf, uint32_t len);
HAL_StatusTypeDef HAL_PCD_EP_Transmit(PCD_HandleTypeDef *hpcd, uint8_t ep_addr, uint8_t *pBuf, uint32_t len);
uint16_t          HAL_PCD_EP_GetRxCount(PCD_HandleTypeDef *hpcd, uint8_t ep_addr);
HAL_StatusTypeDef HAL_PCD_EP_SetStall(PCD_HandleTypeDef *hpcd, uint8_t ep_addr);
HAL_StatusTypeDef HAL_PCD_EP_ClrStall(PCD_HandleTypeDef *hpcd, uint8_t ep_addr);
HAL_StatusTypeDef HAL_PCD_EP_Flush(PCD_HandleTypeDef *hpcd, uint8_t ep_addr);
HAL_StatusTypeDef HAL_PCD_SetTxFiFo(PCD_HandleTypeDef *hpcd, uint8_t fifo, uint16_t size);
HAL_StatusTypeDef HAL_PCD_SetRxFiFo(PCD_HandleTypeDef *hpcd, uint16_t size);
HAL_StatusTypeDef HAL_PCD_ActiveRemoteWakeup(PCD_HandleTypeDef *hpcd);
HAL_StatusTypeDef HAL_PCD_DeActiveRemoteWakeup(PCD_HandleTypeDef *hpcd);
/* Peripheral State functions  **************************************************/
PCD_StateTypeDef HAL_PCD_GetState(PCD_HandleTypeDef *hpcd);

/**
  * @}
  */ 

/**
  * @}
  */ 

#ifdef __cplusplus
}
#endif


#endif /* __STM32F4xx_HAL_PCD_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
