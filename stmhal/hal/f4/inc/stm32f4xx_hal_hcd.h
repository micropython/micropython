/**
  ******************************************************************************
  * @file    stm32f4xx_hal_hcd.h
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-June-2014
  * @brief   Header file of HCD HAL module.
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
#ifndef __STM32F4xx_HAL_HCD_H
#define __STM32F4xx_HAL_HCD_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_ll_usb.h"
   
/** @addtogroup STM32F4xx_HAL_Driver
  * @{
  */

/** @addtogroup HCD
  * @{
  */ 

/* Exported types ------------------------------------------------------------*/ 

   /** 
  * @brief  HCD Status structures structure definition  
  */  
typedef enum 
{
  HAL_HCD_STATE_RESET    = 0x00,
  HAL_HCD_STATE_READY    = 0x01,
  HAL_HCD_STATE_ERROR    = 0x02,
  HAL_HCD_STATE_BUSY     = 0x03,
  HAL_HCD_STATE_TIMEOUT  = 0x04
} HCD_StateTypeDef;

typedef USB_OTG_GlobalTypeDef   HCD_TypeDef;
typedef USB_OTG_CfgTypeDef      HCD_InitTypeDef;
typedef USB_OTG_HCTypeDef       HCD_HCTypeDef ;   
typedef USB_OTG_URBStateTypeDef HCD_URBStateTypeDef ;
typedef USB_OTG_HCStateTypeDef  HCD_HCStateTypeDef ;

/** 
  * @brief  HCD Handle Structure definition  
  */ 
typedef struct
{
  HCD_TypeDef               *Instance;  /*!< Register base address    */ 
  HCD_InitTypeDef           Init;       /*!< HCD required parameters  */
  HCD_HCTypeDef             hc[15];     /*!< Host channels parameters */
  HAL_LockTypeDef           Lock;       /*!< HCD peripheral status    */
  __IO HCD_StateTypeDef     State;      /*!< HCD communication state  */
  void                      *pData;     /*!< Pointer Stack Handler    */    
  
} HCD_HandleTypeDef;
  
/* Exported constants --------------------------------------------------------*/
/** @defgroup HCD_Exported_Constants
  * @{
  */

/** @defgroup HCD_Instance_definition 
  * @{
  */ 

#if defined(STM32F405xx) || defined(STM32F415xx) || defined(STM32F407xx) || defined(STM32F417xx) || defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx)
 #define IS_HCD_ALL_INSTANCE(INSTANCE) (((INSTANCE) == USB_OTG_FS) || \
                                        ((INSTANCE) == USB_OTG_HS))
#elif defined(STM32F401xC) || defined(STM32F401xE) || defined(STM32F411xE)
 #define IS_HCD_ALL_INSTANCE(INSTANCE) (((INSTANCE) == USB_OTG_FS))
#endif

/**
  * @}
  */

/** @defgroup HCD_Speed
  * @{
  */
#define HCD_SPEED_HIGH               0
#define HCD_SPEED_LOW                2  
#define HCD_SPEED_FULL               3

/**
  * @}
  */
  
  /** @defgroup HCD_PHY_Module
  * @{
  */
#define HCD_PHY_ULPI                 1
#define HCD_PHY_EMBEDDED             2
/**
  * @}
  */
/**
  * @}
  */ 
  
/* Exported macro ------------------------------------------------------------*/

/** @defgroup HCD_Interrupt_Clock
 *  @brief macros to handle interrupts and specific clock configurations
 * @{
 */
#define __HAL_HCD_ENABLE(__HANDLE__)                   USB_EnableGlobalInt ((__HANDLE__)->Instance)
#define __HAL_HCD_DISABLE(__HANDLE__)                  USB_DisableGlobalInt ((__HANDLE__)->Instance)
   
   
#define __HAL_HCD_GET_FLAG(__HANDLE__, __INTERRUPT__)      ((USB_ReadInterrupts((__HANDLE__)->Instance) & (__INTERRUPT__)) == (__INTERRUPT__))
#define __HAL_HCD_CLEAR_FLAG(__HANDLE__, __INTERRUPT__)    (((__HANDLE__)->Instance->GINTSTS) = (__INTERRUPT__))
#define __HAL_HCD_IS_INVALID_INTERRUPT(__HANDLE__)         (USB_ReadInterrupts((__HANDLE__)->Instance) == 0)    
   
   
#define __HAL_HCD_CLEAR_HC_INT(chnum, __INTERRUPT__)  (USBx_HC(chnum)->HCINT = (__INTERRUPT__)) 
#define __HAL_HCD_MASK_HALT_HC_INT(chnum)             (USBx_HC(chnum)->HCINTMSK &= ~USB_OTG_HCINTMSK_CHHM) 
#define __HAL_HCD_UNMASK_HALT_HC_INT(chnum)           (USBx_HC(chnum)->HCINTMSK |= USB_OTG_HCINTMSK_CHHM) 
#define __HAL_HCD_MASK_ACK_HC_INT(chnum)              (USBx_HC(chnum)->HCINTMSK &= ~USB_OTG_HCINTMSK_ACKM) 
#define __HAL_HCD_UNMASK_ACK_HC_INT(chnum)            (USBx_HC(chnum)->HCINTMSK |= USB_OTG_HCINTMSK_ACKM) 

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/

/* Initialization/de-initialization functions  **********************************/
HAL_StatusTypeDef      HAL_HCD_Init(HCD_HandleTypeDef *hhcd);
HAL_StatusTypeDef      HAL_HCD_DeInit (HCD_HandleTypeDef *hhcd);
HAL_StatusTypeDef      HAL_HCD_HC_Init(HCD_HandleTypeDef *hhcd,  
                                  uint8_t ch_num,
                                  uint8_t epnum,
                                  uint8_t dev_address,
                                  uint8_t speed,
                                  uint8_t ep_type,
                                  uint16_t mps);

HAL_StatusTypeDef       HAL_HCD_HC_Halt(HCD_HandleTypeDef *hhcd,  
                                  uint8_t ch_num);

void            HAL_HCD_MspInit(HCD_HandleTypeDef *hhcd);
void            HAL_HCD_MspDeInit(HCD_HandleTypeDef *hhcd);

/* I/O operation functions  *****************************************************/
HAL_StatusTypeDef       HAL_HCD_HC_SubmitRequest(HCD_HandleTypeDef *hhcd,
                                                 uint8_t pipe, 
                                                 uint8_t direction ,
                                                 uint8_t ep_type,  
                                                 uint8_t token, 
                                                 uint8_t* pbuff, 
                                                 uint16_t length,
                                                 uint8_t do_ping);

 /* Non-Blocking mode: Interrupt */
void                    HAL_HCD_IRQHandler(HCD_HandleTypeDef *hhcd);
void             HAL_HCD_SOF_Callback(HCD_HandleTypeDef *hhcd);
void             HAL_HCD_Connect_Callback(HCD_HandleTypeDef *hhcd);
void             HAL_HCD_Disconnect_Callback(HCD_HandleTypeDef *hhcd);
void             HAL_HCD_HC_NotifyURBChange_Callback(HCD_HandleTypeDef *hhcd, 
                                                            uint8_t chnum, 
                                                            HCD_URBStateTypeDef urb_state);

/* Peripheral Control functions  ************************************************/
HAL_StatusTypeDef       HAL_HCD_ResetPort(HCD_HandleTypeDef *hhcd);
HAL_StatusTypeDef       HAL_HCD_Start(HCD_HandleTypeDef *hhcd);
HAL_StatusTypeDef       HAL_HCD_Stop(HCD_HandleTypeDef *hhcd);

/* Peripheral State functions  **************************************************/
HCD_StateTypeDef        HAL_HCD_GetState(HCD_HandleTypeDef *hhcd);
HCD_URBStateTypeDef     HAL_HCD_HC_GetURBState(HCD_HandleTypeDef *hhcd, uint8_t chnum);
uint32_t                HAL_HCD_HC_GetXferCount(HCD_HandleTypeDef *hhcd, uint8_t chnum);
HCD_HCStateTypeDef      HAL_HCD_HC_GetState(HCD_HandleTypeDef *hhcd, uint8_t chnum);
uint32_t                HAL_HCD_GetCurrentFrame(HCD_HandleTypeDef *hhcd);
uint32_t                HAL_HCD_GetCurrentSpeed(HCD_HandleTypeDef *hhcd);

/**
  * @}
  */ 

/**
  * @}
  */
  
#ifdef __cplusplus
}
#endif

#endif /* __STM32F4xx_HAL_HCD_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
