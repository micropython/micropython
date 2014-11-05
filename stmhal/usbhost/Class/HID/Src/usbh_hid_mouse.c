/**
  ******************************************************************************
  * @file    usbh_hid_mouse.c 
  * @author  MCD Application Team
  * @version V3.0.0
  * @date    18-February-2014
  * @brief   This file is the application layer for USB Host HID Mouse Handling.                  
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
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
#include "usbh_hid_mouse.h"
#include "usbh_hid_parser.h"


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
  * @brief    This file includes HID Layer Handlers for USB Host HID class.
  * @{
  */ 

/** @defgroup USBH_HID_MOUSE_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBH_HID_MOUSE_Private_Defines
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBH_HID_MOUSE_Private_Macros
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup USBH_HID_MOUSE_Private_FunctionPrototypes
  * @{
  */ 
static USBH_StatusTypeDef USBH_HID_MouseDecode(USBH_HandleTypeDef *phost);

/**
  * @}
  */ 


/** @defgroup USBH_HID_MOUSE_Private_Variables
  * @{
  */
HID_MOUSE_Info_TypeDef    mouse_info;
uint32_t                  mouse_report_data[1];

/* Structures defining how to access items in a HID mouse report */
/* Access button 1 state. */
static const HID_Report_ItemTypedef prop_b1={
  (uint8_t *)mouse_report_data+0, /*data*/
  1,     /*size*/
  0,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  1,     /*max value read can return*/
  0,     /*min value device can report*/
  1,     /*max value device can report*/
  1      /*resolution*/
};

/* Access button 2 state. */
static const HID_Report_ItemTypedef prop_b2={
  (uint8_t *)mouse_report_data+0, /*data*/
  1,     /*size*/
  1,     /*shift*/
  0,     /*count (only for array items)*/  
  0,     /*signed?*/
  0,     /*min value read can return*/
  1,     /*max value read can return*/
  0,     /*min value device can report*/
  1,     /*max value device can report*/
  1      /*resolution*/
};

/* Access button 3 state. */   
static const HID_Report_ItemTypedef prop_b3={
  (uint8_t *)mouse_report_data+0, /*data*/
  1,     /*size*/
  2,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  1,     /*max value read can return*/
  0,     /*min vale device can report*/
  1,     /*max value device can report*/
  1      /*resolution*/
};

/* Access x coordinate change. */
static const HID_Report_ItemTypedef prop_x={
  (uint8_t *)mouse_report_data+1, /*data*/
  8,     /*size*/
  0,     /*shift*/
  0,     /*count (only for array items)*/  
  1,     /*signed?*/
  0,     /*min value read can return*/
  0xFFFF,/*max value read can return*/
  0,     /*min vale device can report*/
  0xFFFF,/*max value device can report*/
  1      /*resolution*/
};

/* Access y coordinate change. */
static const HID_Report_ItemTypedef prop_y={
  (uint8_t *)mouse_report_data+2, /*data*/
  8,     /*size*/
  0,     /*shift*/
  0,     /*count (only for array items)*/  
  1,     /*signed?*/
  0,     /*min value read can return*/
  0xFFFF,/*max value read can return*/
  0,     /*min vale device can report*/
  0xFFFF,/*max value device can report*/
  1      /*resolution*/
};


/**
  * @}
  */ 


/** @defgroup USBH_HID_MOUSE_Private_Functions
  * @{
  */ 

/**
  * @brief  USBH_HID_MouseInit 
  *         The function init the HID mouse.
  * @param  phost: Host handle
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_HID_MouseInit(USBH_HandleTypeDef *phost)
{
  HID_HandleTypeDef *HID_Handle =  phost->pActiveClass->pData;

  mouse_info.x=0;
  mouse_info.y=0;
  mouse_info.buttons[0]=0;
  mouse_info.buttons[1]=0;
  mouse_info.buttons[2]=0;
  
  mouse_report_data[0]=0;
  
  if(HID_Handle->length > sizeof(mouse_report_data))
  {
    HID_Handle->length = sizeof(mouse_report_data);
  }
  HID_Handle->pData = (uint8_t *)mouse_report_data;
  fifo_init(&HID_Handle->fifo, phost->device.Data, HID_QUEUE_SIZE * sizeof(mouse_report_data));

  return USBH_OK;  
}

/**
  * @brief  USBH_HID_GetMouseInfo 
  *         The function return mouse information.
  * @param  phost: Host handle
  * @retval mouse information
  */
HID_MOUSE_Info_TypeDef *USBH_HID_GetMouseInfo(USBH_HandleTypeDef *phost)
{
 if(USBH_HID_MouseDecode(phost)== USBH_OK)
 {
  return &mouse_info;
 }
 else
 {
  return NULL; 
 }
}

/**
  * @brief  USBH_HID_MouseDecode 
  *         The function decode mouse data.
  * @param  phost: Host handle
  * @retval USBH Status
  */
static USBH_StatusTypeDef USBH_HID_MouseDecode(USBH_HandleTypeDef *phost)
{
  HID_HandleTypeDef *HID_Handle =  phost->pActiveClass->pData;
  
  if(HID_Handle->length == 0)
  {
    return USBH_FAIL;
  }
  /*Fill report */
  if(fifo_read(&HID_Handle->fifo, &mouse_report_data, HID_Handle->length) ==  HID_Handle->length)
  {
    
    /*Decode report */
    mouse_info.x = (int16_t )HID_ReadItem((HID_Report_ItemTypedef *) &prop_x, 0);
    mouse_info.y = (int16_t )HID_ReadItem((HID_Report_ItemTypedef *) &prop_y, 0);
    
    mouse_info.buttons[0]=(uint8_t)HID_ReadItem((HID_Report_ItemTypedef *) &prop_b1, 0);
    mouse_info.buttons[1]=(uint8_t)HID_ReadItem((HID_Report_ItemTypedef *) &prop_b2, 0);
    mouse_info.buttons[2]=(uint8_t)HID_ReadItem((HID_Report_ItemTypedef *) &prop_b3, 0);
    
    return USBH_OK;  
  }
  return   USBH_FAIL;
}

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


/**
  * @}
  */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
