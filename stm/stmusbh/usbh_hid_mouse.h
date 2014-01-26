/**
  ******************************************************************************
  * @file    usbh_hid_mouse.h 
  * @author  MCD Application Team
  * @version V2.1.0
  * @date    19-March-2012
  * @brief   This file contains all the prototypes for the usbh_hid_mouse.c
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


/* Define to prevent recursive  ----------------------------------------------*/
#ifndef __USBH_HID_MOUSE_H
#define __USBH_HID_MOUSE_H

/* Includes ------------------------------------------------------------------*/
#include "usbh_hid_core.h"

/** @addtogroup USBH_LIB
  * @{
  */

/** @addtogroup USBH_CLASS
  * @{
  */

/** @addtogroup USBH_HID_CLASS
  * @{
  */

/** @defgroup USBH_HID_MOUSE
  * @brief This file is the Header file for USBH_HID_MOUSE.c
  * @{
  */ 


/** @defgroup USBH_HID_MOUSE_Exported_Types
  * @{
  */ 
typedef struct _HID_MOUSE_Data
{
  uint8_t              x; 
  uint8_t              y;
  uint8_t              z;               /* Not Supported */ 
  uint8_t              button; 
}
HID_MOUSE_Data_TypeDef;

/**
  * @}
  */ 

/** @defgroup USBH_HID_MOUSE_Exported_Defines
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup USBH_HID_MOUSE_Exported_Macros
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup USBH_HID_MOUSE_Exported_Variables
  * @{
  */ 

extern HID_cb_TypeDef HID_MOUSE_cb;
extern HID_MOUSE_Data_TypeDef	 HID_MOUSE_Data;
/**
  * @}
  */ 

/** @defgroup USBH_HID_MOUSE_Exported_FunctionsPrototype
  * @{
  */ 
void  USR_MOUSE_Init (void);
void  USR_MOUSE_ProcessData (HID_MOUSE_Data_TypeDef *data);
/**
  * @}
  */ 

#endif /* __USBH_HID_MOUSE_H */

/**
  * @}
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
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
