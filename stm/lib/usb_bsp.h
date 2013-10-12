/**
  ******************************************************************************
  * @file    usb_bsp.h
  * @author  MCD Application Team
  * @version V2.1.0
  * @date    19-March-2012
  * @brief   Specific api's relative to the used hardware platform
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_BSP__H__
#define __USB_BSP__H__

/* Includes ------------------------------------------------------------------*/
#include "usb_core.h"
#include "usb_conf.h"

/** @addtogroup USB_OTG_DRIVER
  * @{
  */
  
/** @defgroup USB_BSP
  * @brief This file is the 
  * @{
  */ 


/** @defgroup USB_BSP_Exported_Defines
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USB_BSP_Exported_Types
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USB_BSP_Exported_Macros
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup USB_BSP_Exported_Variables
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup USB_BSP_Exported_FunctionsPrototype
  * @{
  */ 
void BSP_Init(void);

void USB_OTG_BSP_Init (USB_OTG_CORE_HANDLE *pdev);
void USB_OTG_BSP_uDelay (const uint32_t usec);
void USB_OTG_BSP_mDelay (const uint32_t msec);
void USB_OTG_BSP_EnableInterrupt (USB_OTG_CORE_HANDLE *pdev);
#ifdef USE_HOST_MODE
void USB_OTG_BSP_ConfigVBUS(USB_OTG_CORE_HANDLE *pdev);
void USB_OTG_BSP_DriveVBUS(USB_OTG_CORE_HANDLE *pdev,uint8_t state);
#endif
/**
  * @}
  */ 

#endif //__USB_BSP__H__

/**
  * @}
  */ 

/**
  * @}
  */ 
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

