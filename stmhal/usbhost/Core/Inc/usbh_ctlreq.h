/**
  ******************************************************************************
  * @file    usbh_ctlreq.h
  * @author  MCD Application Team
  * @version V3.0.0
  * @date    18-February-2014
  * @brief   Header file for usbh_ctlreq.c
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

/* Define to prevent recursive  ----------------------------------------------*/
#ifndef __USBH_CTLREQ_H
#define __USBH_CTLREQ_H

/* Includes ------------------------------------------------------------------*/
#include "usbh_core.h"

/** @addtogroup USBH_LIB
  * @{
  */

/** @addtogroup USBH_LIB_CORE
* @{
*/
  
/** @defgroup USBH_CTLREQ
  * @brief This file is the 
  * @{
  */ 


/** @defgroup USBH_CTLREQ_Exported_Defines
  * @{
  */
/*Standard Feature Selector for clear feature command*/
#define FEATURE_SELECTOR_ENDPOINT         0X00
#define FEATURE_SELECTOR_DEVICE           0X01


#define INTERFACE_DESC_TYPE               0x04
#define ENDPOINT_DESC_TYPE                0x05
#define INTERFACE_DESC_SIZE               0x09

/**
  * @}
  */ 


/** @defgroup USBH_CTLREQ_Exported_Types
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBH_CTLREQ_Exported_Macros
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup USBH_CTLREQ_Exported_Variables
  * @{
  */ 
extern uint8_t USBH_CfgDesc[512];
/**
  * @}
  */ 

/** @defgroup USBH_CTLREQ_Exported_FunctionsPrototype
  * @{
  */
USBH_StatusTypeDef USBH_CtlReq     (USBH_HandleTypeDef *phost, 
                             uint8_t             *buff,
                             uint16_t            length);

USBH_StatusTypeDef USBH_GetDescriptor(USBH_HandleTypeDef *phost,                                
                               uint8_t  req_type,
                               uint16_t value_idx, 
                               uint8_t* buff, 
                               uint16_t length );

USBH_StatusTypeDef USBH_Get_DevDesc(USBH_HandleTypeDef *phost,
                             uint8_t length);

USBH_StatusTypeDef USBH_Get_StringDesc(USBH_HandleTypeDef *phost,                              
                                uint8_t string_index, 
                                uint8_t *buff, 
                                uint16_t length);

USBH_StatusTypeDef USBH_SetCfg(USBH_HandleTypeDef *phost, 
                        uint16_t configuration_value);

USBH_StatusTypeDef USBH_Get_CfgDesc(USBH_HandleTypeDef *phost,                              
                             uint16_t length);

USBH_StatusTypeDef USBH_SetAddress(USBH_HandleTypeDef *phost,                          
                            uint8_t DeviceAddress);

USBH_StatusTypeDef USBH_SetInterface(USBH_HandleTypeDef *phost, 
                        uint8_t ep_num, uint8_t altSetting);

USBH_StatusTypeDef USBH_ClrFeature(USBH_HandleTypeDef *phost, 
                                   uint8_t ep_num);

USBH_DescHeader_t      *USBH_GetNextDesc (uint8_t   *pbuf, 
                                                  uint16_t  *ptr);
/**
  * @}
  */ 

#endif /* __USBH_CTLREQ_H */

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


