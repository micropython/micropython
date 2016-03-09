/**
  ******************************************************************************
  * @file    stm32l4xx_hal_pcd.h
  * @author  MCD Application Team
  * @version V1.3.0
  * @date    29-January-2016
  * @brief   Header file of PCD HAL module.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32L4xx_HAL_PCD_H
#define __STM32L4xx_HAL_PCD_H

#ifdef __cplusplus
 extern "C" {
#endif

#if defined(STM32L475xx) || defined(STM32L476xx) || defined(STM32L485xx) || defined(STM32L486xx)

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_ll_usb.h"
   
/** @addtogroup STM32L4xx_HAL_Driver
  * @{
  */

/** @addtogroup PCD
  * @{
  */ 

/* Exported types ------------------------------------------------------------*/ 
/** @defgroup PCD_Exported_Types PCD Exported Types
  * @{
  */

   /** 
  * @brief  PCD State structure definition  
  */  
typedef enum 
{
  HAL_PCD_STATE_RESET   = 0x00,
  HAL_PCD_STATE_READY   = 0x01,
  HAL_PCD_STATE_ERROR   = 0x02,
  HAL_PCD_STATE_BUSY    = 0x03,
  HAL_PCD_STATE_TIMEOUT = 0x04
} PCD_StateTypeDef;

/* Device LPM suspend state */
typedef enum  
{
  LPM_L0 = 0x00, /* on */
  LPM_L1 = 0x01, /* LPM L1 sleep */
  LPM_L2 = 0x02, /* suspend */
  LPM_L3 = 0x03, /* off */
}PCD_LPM_StateTypeDef;

typedef USB_OTG_GlobalTypeDef  PCD_TypeDef;
typedef USB_OTG_CfgTypeDef     PCD_InitTypeDef;
typedef USB_OTG_EPTypeDef      PCD_EPTypeDef ;                          

/** 
  * @brief  PCD Handle Structure definition  
  */ 
typedef struct
{
  PCD_TypeDef             *Instance;   /*!< Register base address              */ 
  PCD_InitTypeDef         Init;        /*!< PCD required parameters            */
  PCD_EPTypeDef           IN_ep[15];    /*!< IN endpoint parameters             */
  PCD_EPTypeDef           OUT_ep[15];   /*!< OUT endpoint parameters            */ 
  HAL_LockTypeDef         Lock;        /*!< PCD peripheral status              */
  __IO PCD_StateTypeDef   State;       /*!< PCD communication state            */
  uint32_t                Setup[12];   /*!< Setup packet buffer                */
  PCD_LPM_StateTypeDef    LPM_State;    /*!< LPM State                          */
  uint32_t                BESL;
  
  
  uint32_t lpm_active;                  /*!< Enable or disable the Link Power Management .                                  
                                        This parameter can be set to ENABLE or DISABLE                      */

  uint32_t battery_charging_active;     /*!< Enable or disable Battery charging.                                  
                                        This parameter can be set to ENABLE or DISABLE                      */
  void                    *pData;      /*!< Pointer to upper stack Handler     */    
  
} PCD_HandleTypeDef;

/**
  * @}
  */
  
/* Exported constants --------------------------------------------------------*/
/** @defgroup PCD_Exported_Constants PCD Exported Constants
  * @{
  */

/** @defgroup PCD_Speed PCD Speed
  * @{
  */
#define PCD_SPEED_FULL               1
/**
  * @}
  */
  
/** @defgroup PCD_PHY_Module PCD PHY Module
  * @{
  */
#define PCD_PHY_EMBEDDED             1
/**
  * @}
  */

/** @defgroup PCD_Turnaround_Timeout Turnaround Timeout Value
  * @{
  */
#ifndef USBD_FS_TRDT_VALUE
 #define USBD_FS_TRDT_VALUE           5
#endif /* USBD_FS_TRDT_VALUE */
/**
  * @}
  */

/**
  * @}
  */ 
  
/* Exported macros -----------------------------------------------------------*/
/** @defgroup PCD_Exported_Macros PCD Exported Macros
 *  @brief macros to handle interrupts and specific clock configurations
 * @{
 */
#define __HAL_PCD_ENABLE(__HANDLE__)                       USB_EnableGlobalInt ((__HANDLE__)->Instance)
#define __HAL_PCD_DISABLE(__HANDLE__)                      USB_DisableGlobalInt ((__HANDLE__)->Instance)
   
#define __HAL_PCD_GET_FLAG(__HANDLE__, __INTERRUPT__)      ((USB_ReadInterrupts((__HANDLE__)->Instance) & (__INTERRUPT__)) == (__INTERRUPT__))
#define __HAL_PCD_CLEAR_FLAG(__HANDLE__, __INTERRUPT__)    (((__HANDLE__)->Instance->GINTSTS) &=  (__INTERRUPT__))
#define __HAL_PCD_IS_INVALID_INTERRUPT(__HANDLE__)         (USB_ReadInterrupts((__HANDLE__)->Instance) == 0)


#define __HAL_PCD_UNGATE_PHYCLOCK(__HANDLE__)             *(__IO uint32_t *)((uint32_t)((__HANDLE__)->Instance) + USB_OTG_PCGCCTL_BASE) &= \
                                                          ~(USB_OTG_PCGCCTL_STOPCLK)

#define __HAL_PCD_GATE_PHYCLOCK(__HANDLE__)               *(__IO uint32_t *)((uint32_t)((__HANDLE__)->Instance) + USB_OTG_PCGCCTL_BASE) |= USB_OTG_PCGCCTL_STOPCLK
                                                      
#define __HAL_PCD_IS_PHY_SUSPENDED(__HANDLE__)            ((*(__IO uint32_t *)((uint32_t)((__HANDLE__)->Instance) + USB_OTG_PCGCCTL_BASE))&0x10)
                                                         
#define USB_OTG_FS_WAKEUP_EXTI_RISING_EDGE                ((uint32_t)0x08) 
#define USB_OTG_FS_WAKEUP_EXTI_FALLING_EDGE               ((uint32_t)0x0C) 
#define USB_OTG_FS_WAKEUP_EXTI_RISING_FALLING_EDGE        ((uint32_t)0x10) 

#define USB_OTG_FS_WAKEUP_EXTI_LINE                       ((uint32_t)0x00020000)  /*!< External interrupt line 17 Connected to the USB FS EXTI Line */

                                                      
#define __HAL_USB_OTG_FS_WAKEUP_EXTI_ENABLE_IT()    EXTI->IMR1 |= USB_OTG_FS_WAKEUP_EXTI_LINE
#define __HAL_USB_OTG_FS_WAKEUP_EXTI_DISABLE_IT()   EXTI->IMR1 &= ~(USB_OTG_FS_WAKEUP_EXTI_LINE)
#define __HAL_USB_OTG_FS_WAKEUP_EXTI_GET_FLAG()     EXTI->PR1 & (USB_OTG_FS_WAKEUP_EXTI_LINE)
#define __HAL_USB_OTG_FS_WAKEUP_EXTI_CLEAR_FLAG()   EXTI->PR1 = USB_OTG_FS_WAKEUP_EXTI_LINE

#define __HAL_USB_OTG_FS_WAKEUP_EXTI_ENABLE_RISING_EDGE()  do {\
                                                             EXTI->FTSR1 &= ~(USB_OTG_FS_WAKEUP_EXTI_LINE);\
                                                             EXTI->RTSR1 |= USB_OTG_FS_WAKEUP_EXTI_LINE;\
                                                           } while(0)

#define __HAL_USB_OTG_FS_WAKEUP_EXTI_ENABLE_FALLING_EDGE() do {\
                                                             EXTI->FTSR1 |= (USB_OTG_FS_WAKEUP_EXTI_LINE);\
                                                             EXTI->RTSR1 &= ~(USB_OTG_FS_WAKEUP_EXTI_LINE);\
                                                           } while(0)

#define __HAL_USB_OTG_FS_WAKEUP_EXTI_ENABLE_RISING_FALLING_EDGE() do {\
                                                                   EXTI->RTSR1 &= ~(USB_OTG_FS_WAKEUP_EXTI_LINE);\
                                                                   EXTI->FTSR1 &= ~(USB_OTG_FS_WAKEUP_EXTI_LINE);\
                                                                   EXTI->RTSR1 |= USB_OTG_FS_WAKEUP_EXTI_LINE;\
                                                                   EXTI->FTSR1 |= USB_OTG_FS_WAKEUP_EXTI_LINE;\
                                                                  } while(0)                                                                   
                                                         
#define __HAL_USB_OTG_FS_WAKEUP_EXTI_GENERATE_SWIT()   (EXTI->SWIER1 |= USB_OTG_FS_WAKEUP_EXTI_LINE)                                                          

/**
  * @}
  */

/* Include PCD HAL Extended module */
#include "stm32l4xx_hal_pcd_ex.h"

/** @addtogroup PCD_Exported_Functions PCD Exported Functions
  * @{
  */

/* Initialization/de-initialization functions  ********************************/
/** @addtogroup PCD_Exported_Functions_Group1 Initialization and de-initialization functions
  * @{
  */
HAL_StatusTypeDef HAL_PCD_Init(PCD_HandleTypeDef *hpcd);
HAL_StatusTypeDef HAL_PCD_DeInit (PCD_HandleTypeDef *hpcd);
void HAL_PCD_MspInit(PCD_HandleTypeDef *hpcd);
void HAL_PCD_MspDeInit(PCD_HandleTypeDef *hpcd);
/**
  * @}
  */

/* I/O operation functions  ***************************************************/
/* Non-Blocking mode: Interrupt */
/** @addtogroup PCD_Exported_Functions_Group2 Input and Output operation functions
  * @{
  */
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
/**
  * @}
  */

/* Peripheral Control functions  **********************************************/
/** @addtogroup PCD_Exported_Functions_Group3 Peripheral Control functions
  * @{
  */
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
HAL_StatusTypeDef HAL_PCD_ActivateRemoteWakeup(PCD_HandleTypeDef *hpcd);
HAL_StatusTypeDef HAL_PCD_DeActivateRemoteWakeup(PCD_HandleTypeDef *hpcd);
/**
  * @}
  */

/* Peripheral State functions  ************************************************/
/** @addtogroup PCD_Exported_Functions_Group4 Peripheral State functions
  * @{
  */
PCD_StateTypeDef HAL_PCD_GetState(PCD_HandleTypeDef *hpcd);
/**
  * @}
  */

/**
  * @}
  */ 

/* Private macros ------------------------------------------------------------*/
/** @defgroup PCD_Private_Macros PCD Private Macros
 * @{
 */

/**
  * @}
  */

/**
  * @}
  */ 

/**
  * @}
  */

#endif /* STM32L475xx || STM32L476xx || STM32L485xx || STM32L486xx */

#ifdef __cplusplus
}
#endif


#endif /* __STM32L4xx_HAL_PCD_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
