/**
  ******************************************************************************
  * @file    usbh_mtp.h
  * @author  MCD Application Team
  * @version V3.0.0
  * @date    18-February-2014
  * @brief   This file contains all the prototypes for the usbh_mtp.c
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
#ifndef __USBH_MTP_CORE_H
#define __USBH_MTP_CORE_H

/* Includes ------------------------------------------------------------------*/
#include "usbh_mtp_ptp.h"
#include "usbh_core.h"


/** @addtogroup USBH_LIB
* @{
*/

/** @addtogroup USBH_CLASS
* @{
*/

/** @addtogroup USBH_MTP_CLASS
* @{
*/

/** @defgroup USBH_MTP_CORE
* @brief This file is the Header file for USBH_MTP_CORE.c
* @{
*/ 




/*Communication Class codes*/
#define USB_MTP_CLASS                                           0x06 /* Still Image Class)*/
#define MTP_MAX_STORAGE_UNITS_NBR                               PTP_MAX_STORAGE_UNITS_NBR

/**
  * @}
  */ 

/** @defgroup USBH_MTP_CORE_Exported_Types
* @{
*/ 
typedef enum
{   
  MTP_IDLE = 0,
  MTP_GETDEVICEINFO ,           
  MTP_OPENSESSION ,             
  MTP_CLOSESESSION ,            
  MTP_GETSTORAGEIDS ,           
  MTP_GETSTORAGEINFO ,          
}
MTP_StateTypeDef;


typedef enum
{   
  MTP_EVENTS_INIT = 0,
  MTP_EVENTS_GETDATA ,                   
}
MTP_EventsStateTypeDef;


typedef struct
{
   MTP_EventsStateTypeDef   state;
   uint32_t timer;
   uint16_t poll;
   PTP_EventContainerTypedef  container;       
} 
MTP_EventHandleTypedef;

typedef struct
{
    
  uint32_t        CurrentStorageId;
  uint32_t        ObjectFormatCode;
  uint32_t        CurrentObjectHandler;
  uint8_t         ObjectHandlerNbr;
  uint32_t        Objdepth;
} 
MTP_ParamsTypedef;


typedef struct
{
  PTP_DeviceInfoTypedef     devinfo;
  PTP_StorageIDsTypedef     storids;
  PTP_StorageInfoTypedef    storinfo[MTP_MAX_STORAGE_UNITS_NBR]; 
  PTP_ObjectHandlesTypedef  Handles;
} 
MTP_InfoTypedef;

/* Structure for MTP process */
typedef struct _MTP_Process
{
  MTP_InfoTypedef       info;
  MTP_ParamsTypedef     params;
  
  uint8_t               DataInPipe; 
  uint8_t               DataOutPipe;
  uint8_t               NotificationPipe;  
  
  uint8_t               DataOutEp;
  uint8_t               DataInEp;
  uint8_t               NotificationEp;  

  uint16_t              DataOutEpSize;
  uint16_t              DataInEpSize;
  uint16_t              NotificationEpSize; 
  MTP_StateTypeDef      state;
  MTP_EventHandleTypedef events;
  PTP_HandleTypeDef     ptp;
  uint32_t              current_storage_unit;
  uint32_t              is_ready;  
}
MTP_HandleTypeDef;

#define MTP_StorageInfoTypedef      PTP_StorageInfoTypedef
#define MTP_ObjectHandlesTypedef    PTP_ObjectHandlesTypedef
#define MTP_ObjectInfoTypedef       PTP_ObjectInfoTypedef
/**
* @}
*/ 

/** @defgroup USBH_MTP_CORE_Exported_Defines
* @{
*/ 

/**
* @}
*/ 

/** @defgroup USBH_MTP_CORE_Exported_Macros
* @{
*/ 
/**
* @}
*/ 

/** @defgroup USBH_MTP_CORE_Exported_Variables
* @{
*/ 
extern USBH_ClassTypeDef  MTP_Class;
#define USBH_MTP_CLASS    &MTP_Class

/**
* @}
*/ 

/** @defgroup USBH_MTP_CORE_Exported_FunctionsPrototype
* @{
*/ 
uint8_t            USBH_MTP_IsReady (USBH_HandleTypeDef *phost);
USBH_StatusTypeDef USBH_MTP_SelectStorage (USBH_HandleTypeDef *phost, uint8_t storage_idx);
USBH_StatusTypeDef USBH_MTP_GetNumStorage (USBH_HandleTypeDef *phost, uint8_t *storage_num);
USBH_StatusTypeDef USBH_MTP_GetNumObjects (USBH_HandleTypeDef *phost, 
                                           uint32_t storage_id, 
                                           uint32_t objectformatcode, 
                                           uint32_t associationOH,
                                           uint32_t* numobs);
USBH_StatusTypeDef USBH_MTP_GetStorageInfo (USBH_HandleTypeDef *phost, 
                                            uint8_t storage_idx, 
                                            MTP_StorageInfoTypedef *info);

USBH_StatusTypeDef USBH_MTP_GetObjectHandles (USBH_HandleTypeDef *phost, 
                                           uint32_t storage_id, 
                                           uint32_t objectformatcode, 
                                           uint32_t associationOH,
                                           PTP_ObjectHandlesTypedef* objecthandles);

USBH_StatusTypeDef USBH_MTP_GetObjectInfo (USBH_HandleTypeDef *phost, 
                                           uint32_t handle, 
                                           PTP_ObjectInfoTypedef* objectinfo);

USBH_StatusTypeDef USBH_MTP_DeleteObject (USBH_HandleTypeDef *phost,
                                          uint32_t handle,
                                          uint32_t objectformatcode);

USBH_StatusTypeDef USBH_MTP_GetObject (USBH_HandleTypeDef *phost, 
                                           uint32_t handle, 
                                           uint8_t *object);

USBH_StatusTypeDef USBH_MTP_GetPartialObject(USBH_HandleTypeDef *phost, 
                                           uint32_t handle, 
                                           uint32_t offset,
                                           uint32_t maxbytes, 
                                           uint8_t *object,
                                           uint32_t *len);

USBH_StatusTypeDef USBH_MTP_GetObjectPropsSupported (USBH_HandleTypeDef *phost,
                                                     uint16_t ofc,
                                                     uint32_t *propnum, 
                                                     uint16_t *props);

USBH_StatusTypeDef USBH_MTP_GetObjectPropDesc (USBH_HandleTypeDef *phost,
                                                uint16_t opc, 
                                                uint16_t ofc, 
                                                PTP_ObjectPropDescTypeDef *opd);

USBH_StatusTypeDef USBH_MTP_GetObjectPropList (USBH_HandleTypeDef *phost,
                                                uint32_t handle, 
                                                MTP_PropertiesTypedef *pprops, 
                                                uint32_t *nrofprops);

USBH_StatusTypeDef USBH_MTP_SendObject (USBH_HandleTypeDef *phost, 
                                           uint32_t handle, 
                                           uint8_t *object,
                                           uint32_t size);

USBH_StatusTypeDef USBH_MTP_GetDevicePropDesc (USBH_HandleTypeDef *phost,
                                                uint16_t propcode, 
			                        PTP_DevicePropDescTypdef* devicepropertydesc);

void USBH_MTP_EventsCallback(USBH_HandleTypeDef *phost, uint32_t event, uint32_t param);
/**
* @}
*/ 


#endif /* __USBH_MTP_CORE_H */

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

