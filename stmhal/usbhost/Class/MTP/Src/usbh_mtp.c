/**
  ******************************************************************************
  * @file    usbh_mtp.c
  * @author  MCD Application Team
  * @version V3.0.0
  * @date    18-February-2014
  * @brief   This file is the MTP Layer Handlers for USB Host MTP class.
  *
  * @verbatim
  *      
  *          ===================================================================      
  *                                MTP Class  Description
  *          =================================================================== 
  *           This module manages the MSC class V1.11 following the "Device Class Definition
  *           for Human Interface Devices (MTP) Version 1.11 Jun 27, 2001".
  *           This driver implements the following aspects of the specification:
  *             - The Boot Interface Subclass
  *             - The Mouse and Keyboard protocols
  *      
  *  @endverbatim
  *
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
#include "usbh_mtp.h"

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
* @brief    This file includes MTP Layer Handlers for USB Host MTP class.
* @{
*/ 

/** @defgroup USBH_MTP_CORE_Private_TypesDefinitions
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBH_MTP_CORE_Private_Defines
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBH_MTP_CORE_Private_Macros
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBH_MTP_CORE_Private_Variables
* @{
*/
/**
* @}
*/ 


/** @defgroup USBH_MTP_CORE_Private_FunctionPrototypes
* @{
*/ 

static USBH_StatusTypeDef USBH_MTP_InterfaceInit  (USBH_HandleTypeDef *phost);

static USBH_StatusTypeDef USBH_MTP_InterfaceDeInit  (USBH_HandleTypeDef *phost);

static USBH_StatusTypeDef USBH_MTP_Process(USBH_HandleTypeDef *phost);

static USBH_StatusTypeDef USBH_MTP_ClassRequest (USBH_HandleTypeDef *phost);

static uint8_t MTP_FindCtlEndpoint(USBH_HandleTypeDef *phost);

static uint8_t MTP_FindDataOutEndpoint(USBH_HandleTypeDef *phost);

static uint8_t MTP_FindDataInEndpoint(USBH_HandleTypeDef *phost);

static USBH_StatusTypeDef USBH_MTP_SOFProcess (USBH_HandleTypeDef *phost);

static USBH_StatusTypeDef USBH_MTP_Events (USBH_HandleTypeDef *phost);

static void MTP_DecodeEvent (USBH_HandleTypeDef *phost) ;

USBH_ClassTypeDef  MTP_Class = 
{
  "MTP",
  USB_MTP_CLASS,
  USBH_MTP_InterfaceInit,
  USBH_MTP_InterfaceDeInit,
  USBH_MTP_ClassRequest,
  USBH_MTP_Process,
  USBH_MTP_SOFProcess,
  NULL,
};
/**
* @}
*/ 


/** @defgroup USBH_MTP_CORE_Private_Functions
* @{
*/ 

/**
  * @brief  USBH_MTP_InterfaceInit 
  *         The function init the MTP class.
  * @param  phost: Host handle
  * @retval USBH Status
  */
static USBH_StatusTypeDef USBH_MTP_InterfaceInit (USBH_HandleTypeDef *phost)
{	
  USBH_StatusTypeDef status = USBH_OK ;
  uint8_t interface, endpoint;
  
  MTP_HandleTypeDef *MTP_Handle;
  
  interface = USBH_FindInterface(phost, 
                                 USB_MTP_CLASS, 
                                 1, 
                                 1);
  
  if(interface == 0xFF) /* No Valid Interface */
  {
    status = USBH_FAIL;  
    USBH_DbgLog ("Cannot Find the interface for Still Image Class.");         
  }
  else
  {
    USBH_SelectInterface (phost, interface);
    
    endpoint = MTP_FindCtlEndpoint(phost);

    phost->pActiveClass->pData = (MTP_HandleTypeDef *)USBH_malloc (sizeof(MTP_HandleTypeDef));
    MTP_Handle =  phost->pActiveClass->pData; 
    
    if( MTP_Handle == NULL)
    {
      status = USBH_FAIL;  
      USBH_DbgLog ("Cannot allocate RAM for MTP Handle");       
    }
    
    /*Collect the control endpoint address and length*/
    MTP_Handle->NotificationEp = phost->device.CfgDesc.Itf_Desc[interface].Ep_Desc[endpoint].bEndpointAddress;
    MTP_Handle->NotificationEpSize  = phost->device.CfgDesc.Itf_Desc[interface].Ep_Desc[endpoint].wMaxPacketSize;
    MTP_Handle->NotificationPipe = USBH_AllocPipe(phost, MTP_Handle->NotificationEp);
    MTP_Handle->events.poll = phost->device.CfgDesc.Itf_Desc[interface].Ep_Desc[endpoint].bInterval;
    
    /* Open pipe for Notification endpoint */
    USBH_OpenPipe  (phost,
                    MTP_Handle->NotificationPipe,
                    MTP_Handle->NotificationEp,                            
                    phost->device.address,
                    phost->device.speed,
                    USB_EP_TYPE_INTR,
                    MTP_Handle->NotificationEpSize); 
    
    USBH_LL_SetToggle (phost, MTP_Handle->NotificationPipe, 0);    
    
    
    endpoint = MTP_FindDataInEndpoint(phost);
    
    /*Collect the control endpoint address and length*/
    MTP_Handle->DataInEp = phost->device.CfgDesc.Itf_Desc[interface].Ep_Desc[endpoint].bEndpointAddress;
    MTP_Handle->DataInEpSize  = phost->device.CfgDesc.Itf_Desc[interface].Ep_Desc[endpoint].wMaxPacketSize;
    MTP_Handle->DataInPipe = USBH_AllocPipe(phost, MTP_Handle->DataInEp);
    
    /* Open pipe for DATA IN endpoint */
    USBH_OpenPipe  (phost,
                    MTP_Handle->DataInPipe,
                    MTP_Handle->DataInEp,                            
                    phost->device.address,
                    phost->device.speed,
                    USB_EP_TYPE_BULK,
                    MTP_Handle->DataInEpSize); 
    
    USBH_LL_SetToggle (phost, MTP_Handle->DataInPipe, 0);  

    endpoint = MTP_FindDataOutEndpoint(phost);
    
    /*Collect the DATA OUT endpoint address and length*/
    MTP_Handle->DataOutEp = phost->device.CfgDesc.Itf_Desc[interface].Ep_Desc[endpoint].bEndpointAddress;
    MTP_Handle->DataOutEpSize  = phost->device.CfgDesc.Itf_Desc[interface].Ep_Desc[endpoint].wMaxPacketSize;
    MTP_Handle->DataOutPipe = USBH_AllocPipe(phost, MTP_Handle->DataOutEp);

    /* Open pipe for DATA OUT endpoint */
    USBH_OpenPipe  (phost,
                    MTP_Handle->DataOutPipe,
                    MTP_Handle->DataOutEp,                            
                    phost->device.address,
                    phost->device.speed,
                    USB_EP_TYPE_BULK,
                    MTP_Handle->DataOutEpSize); 
    
    USBH_LL_SetToggle (phost, MTP_Handle->DataOutPipe, 0); 
    
      
  MTP_Handle->state = MTP_OPENSESSION;
  MTP_Handle->is_ready = 0;
  MTP_Handle->events.state = MTP_EVENTS_INIT;
  return USBH_PTP_Init(phost);
  
  }
  return status;
}

/**
  * @brief  Find MTP Ctl interface
  * @param  phost: Host handle
  * @retval USBH Status
  */
static uint8_t MTP_FindCtlEndpoint(USBH_HandleTypeDef *phost)
{
  uint8_t interface, endpoint;
  
  for (interface = 0;  interface < USBH_MAX_NUM_INTERFACES ; interface ++ )
  {
    if(phost->device.CfgDesc.Itf_Desc[interface].bInterfaceClass == USB_MTP_CLASS)
    {    
      for (endpoint = 0;  endpoint < USBH_MAX_NUM_ENDPOINTS ; endpoint ++ )
      {
        if((phost->device.CfgDesc.Itf_Desc[interface].Ep_Desc[endpoint].bEndpointAddress & 0x80)&&
           (phost->device.CfgDesc.Itf_Desc[interface].Ep_Desc[endpoint].wMaxPacketSize > 0)&&
             ((phost->device.CfgDesc.Itf_Desc[interface].Ep_Desc[endpoint].bmAttributes & USBH_EP_INTERRUPT) == USBH_EP_INTERRUPT))
        {
          return endpoint;
        }
      }
    }
  }
  
  return 0xFF; /* Invalid Endpoint */
}


/**
  * @brief  Find MTP DATA OUT interface
  * @param  phost: Host handle
  * @retval USBH Status
  */
static uint8_t MTP_FindDataOutEndpoint(USBH_HandleTypeDef *phost)
{
  uint8_t interface, endpoint;
  
  for (interface = 0;  interface < USBH_MAX_NUM_INTERFACES ; interface ++ )
  {
    if(phost->device.CfgDesc.Itf_Desc[interface].bInterfaceClass == USB_MTP_CLASS)
    {    
      for (endpoint = 0;  endpoint < USBH_MAX_NUM_ENDPOINTS ; endpoint ++ )
      {

        if(((phost->device.CfgDesc.Itf_Desc[interface].Ep_Desc[endpoint].bEndpointAddress & 0x80) == 0)&&
           (phost->device.CfgDesc.Itf_Desc[interface].Ep_Desc[endpoint].wMaxPacketSize > 0)&&
             ((phost->device.CfgDesc.Itf_Desc[interface].Ep_Desc[endpoint].bmAttributes & USBH_EP_BULK) == USBH_EP_BULK))
        {
          return endpoint;
        }
      }
    }
  }
  
  return 0xFF; /* Invalid Endpoint */
}

/**
  * @brief  Find MTP DATA IN interface
  * @param  phost: Host handle
  * @retval USBH Status
  */
static uint8_t MTP_FindDataInEndpoint(USBH_HandleTypeDef *phost)
{
  uint8_t interface, endpoint;
  
  for (interface = 0;  interface < USBH_MAX_NUM_INTERFACES ; interface ++ )
  {
    if(phost->device.CfgDesc.Itf_Desc[interface].bInterfaceClass == USB_MTP_CLASS)
    {    
      for (endpoint = 0;  endpoint < USBH_MAX_NUM_ENDPOINTS ; endpoint ++ )
      {
        
        if((phost->device.CfgDesc.Itf_Desc[interface].Ep_Desc[endpoint].bEndpointAddress & 0x80)&&
           (phost->device.CfgDesc.Itf_Desc[interface].Ep_Desc[endpoint].wMaxPacketSize > 0)&&
             ((phost->device.CfgDesc.Itf_Desc[interface].Ep_Desc[endpoint].bmAttributes & USBH_EP_BULK) == USBH_EP_BULK))
        {
          return endpoint;
        }
      }
    }
  }
  
  return 0xFF; /* Invalid Endpoint */
}


/**
  * @brief  USBH_MTP_InterfaceDeInit 
  *         The function DeInit the Pipes used for the MTP class.
  * @param  phost: Host handle
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_MTP_InterfaceDeInit (USBH_HandleTypeDef *phost)
{
  MTP_HandleTypeDef *MTP_Handle =  phost->pActiveClass->pData;
  
  if (MTP_Handle->DataOutPipe)
  {
    USBH_ClosePipe(phost, MTP_Handle->DataOutPipe);
    USBH_FreePipe  (phost, MTP_Handle->DataOutPipe);
    MTP_Handle->DataOutPipe = 0;     /* Reset the Channel as Free */
  }
  
  if (MTP_Handle->DataInPipe)
  {
    USBH_ClosePipe(phost, MTP_Handle->DataInPipe);
    USBH_FreePipe  (phost, MTP_Handle->DataInPipe);
    MTP_Handle->DataInPipe = 0;     /* Reset the Channel as Free */
  } 
  
  if (MTP_Handle->NotificationPipe)
  {
    USBH_ClosePipe(phost, MTP_Handle->NotificationPipe);
    USBH_FreePipe  (phost, MTP_Handle->NotificationPipe);
    MTP_Handle->NotificationPipe = 0;     /* Reset the Channel as Free */
  } 
  
  if(phost->pActiveClass->pData)
  {
    USBH_free (phost->pActiveClass->pData);
    phost->pActiveClass->pData = 0;
  }
  return USBH_OK;
}

/**
  * @brief  USBH_MTP_ClassRequest 
  *         The function is responsible for handling Standard requests
  *         for MTP class.
  * @param  phost: Host handle
  * @retval USBH Status
  */
static USBH_StatusTypeDef USBH_MTP_ClassRequest (USBH_HandleTypeDef *phost)
{   
  return USBH_OK;;
}


/**
  * @brief  USBH_MTP_Process 
  *         The function is for managing state machine for MTP data transfers 
  * @param  phost: Host handle
  * @retval USBH Status
  */
static USBH_StatusTypeDef USBH_MTP_Process (USBH_HandleTypeDef *phost)
{
  USBH_StatusTypeDef status = USBH_BUSY;
  MTP_HandleTypeDef *MTP_Handle =  phost->pActiveClass->pData;
  uint32_t idx = 0;
  
  switch(MTP_Handle->state)
  {
  case  MTP_OPENSESSION:
    
    status = USBH_PTP_OpenSession (phost, 1); /* Session '0' is not valid */
    
    if(status == USBH_OK)
    {
      USBH_UsrLog("MTP Session #0 Opened");
      MTP_Handle->state = MTP_GETDEVICEINFO; 
    }
    break;
    
  case MTP_GETDEVICEINFO:
    status = USBH_PTP_GetDeviceInfo (phost, &(MTP_Handle->info.devinfo));
    
    if(status == USBH_OK)
    {
      USBH_DbgLog(">>>>> MTP Device Information");
      USBH_DbgLog("Standard version : %x", MTP_Handle->info.devinfo.StandardVersion);
      USBH_DbgLog("Vendor ExtID : %s", (MTP_Handle->info.devinfo.VendorExtensionID == 6)?"MTP": "NOT SUPPORTED");      
      USBH_DbgLog("Functional mode : %s", (MTP_Handle->info.devinfo.FunctionalMode == 0) ? "Standard" : "Vendor");
      USBH_DbgLog("Number of Supported Operation(s) : %d", MTP_Handle->info.devinfo.OperationsSupported_len);
      USBH_DbgLog("Number of Supported Events(s) : %d", MTP_Handle->info.devinfo.EventsSupported_len);  
      USBH_DbgLog("Number of Supported Proprieties : %d", MTP_Handle->info.devinfo.DevicePropertiesSupported_len); 
      USBH_DbgLog("Manufacturer : %s", MTP_Handle->info.devinfo.Manufacturer);
      USBH_DbgLog("Model : %s", MTP_Handle->info.devinfo.Model);       
      USBH_DbgLog("Device version : %s", MTP_Handle->info.devinfo.DeviceVersion); 
      USBH_DbgLog("Serial number : %s", MTP_Handle->info.devinfo.SerialNumber); 
      
      MTP_Handle->state = MTP_GETSTORAGEIDS; 
    }
    break;
    
  case MTP_GETSTORAGEIDS:
    status = USBH_PTP_GetStorageIds (phost, &(MTP_Handle->info.storids));
    
    if(status == USBH_OK)
    {
      USBH_DbgLog("Number of storage ID items : %d", MTP_Handle->info.storids.n);
      for (idx  = 0; idx < MTP_Handle->info.storids.n; idx ++)
      {
        USBH_DbgLog("storage#%d ID : %x", idx, MTP_Handle->info.storids.Storage[idx]);
      }
      
      MTP_Handle->current_storage_unit = 0;
      MTP_Handle->state = MTP_GETSTORAGEINFO; 
    }
    break;
    
  case MTP_GETSTORAGEINFO:
    status = USBH_PTP_GetStorageInfo (phost, 
                                      MTP_Handle->info.storids.Storage[MTP_Handle->current_storage_unit], 
                                      &((MTP_Handle->info.storinfo)[MTP_Handle->current_storage_unit]));
    
    if(status == USBH_OK)
    {
      USBH_UsrLog("Volume#%lu: %s   [%s]", MTP_Handle->current_storage_unit,
                  MTP_Handle->info.storinfo[MTP_Handle->current_storage_unit].StorageDescription,
                  MTP_Handle->info.storinfo[MTP_Handle->current_storage_unit].VolumeLabel);
      if(++MTP_Handle->current_storage_unit >= MTP_Handle->info.storids.n)
      {
        MTP_Handle->state = MTP_IDLE;
        MTP_Handle->is_ready = 1;
        MTP_Handle->current_storage_unit = 0;
        MTP_Handle->params.CurrentStorageId = MTP_Handle->info.storids.Storage[0];
        
        USBH_UsrLog( "MTP Class initialized.");
        USBH_UsrLog("%s is default storage unit", MTP_Handle->info.storinfo[0].StorageDescription);
        phost->pUser(phost, HOST_USER_CLASS_ACTIVE); 
      }
    }
    break;
  
  case  MTP_IDLE:
    USBH_MTP_Events(phost);
  default:
    status = USBH_OK;
    break;
  }
  return status;
}

/**
  * @brief  USBH_MTP_SOFProcess 
  *         The function is for managing SOF callback 
  * @param  phost: Host handle
  * @retval USBH Status
  */
static USBH_StatusTypeDef USBH_MTP_SOFProcess (USBH_HandleTypeDef *phost)
{
  USBH_StatusTypeDef status = USBH_OK;

  return status;
}

/**
  * @brief  USBH_MTP_IsReady 
  *         Select the storage Unit to be used
  * @param  phost: Host handle
  * @retval USBH Status
  */
uint8_t USBH_MTP_IsReady (USBH_HandleTypeDef *phost)
{
  MTP_HandleTypeDef *MTP_Handle =  phost->pActiveClass->pData;
  
  return (MTP_Handle->is_ready);
}

/**
  * @brief  USBH_MTP_GetNumStorage 
  *         Select the storage Unit to be used
  * @param  phost: Host handle
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_MTP_GetNumStorage (USBH_HandleTypeDef *phost, uint8_t *storage_num)
{
  USBH_StatusTypeDef status = USBH_FAIL;
  MTP_HandleTypeDef *MTP_Handle =  phost->pActiveClass->pData;
  
  if(MTP_Handle->is_ready > 0)
  {
    *storage_num = MTP_Handle->info.storids.n;
    status  = USBH_OK;
  }
  
  return status;
}

/**
  * @brief  USBH_MTP_SelectStorage 
  *         Select the storage Unit to be used
  * @param  phost: Host handle
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_MTP_SelectStorage (USBH_HandleTypeDef *phost, uint8_t storage_idx)
{
  USBH_StatusTypeDef status = USBH_FAIL;
  MTP_HandleTypeDef *MTP_Handle =  phost->pActiveClass->pData;
  
  if((storage_idx < MTP_Handle->info.storids.n) && (MTP_Handle->is_ready))
  {
      MTP_Handle->params.CurrentStorageId = MTP_Handle->info.storids.Storage[storage_idx];
      status  = USBH_OK;
  }
  
  return status;
}

/**
  * @brief  USBH_MTP_GetStorageInfo 
  *         Get the storage Unit info
  * @param  phost: Host handle
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_MTP_GetStorageInfo (USBH_HandleTypeDef *phost, uint8_t storage_idx, MTP_StorageInfoTypedef *info)
{
  USBH_StatusTypeDef status = USBH_FAIL;
  MTP_HandleTypeDef *MTP_Handle =  phost->pActiveClass->pData;
  
  if((storage_idx < MTP_Handle->info.storids.n) && (MTP_Handle->is_ready))
  {
    *info = MTP_Handle->info.storinfo[storage_idx];
    status  = USBH_OK;
  }
  return status;
}

/**
  * @brief  USBH_MTP_GetStorageInfo 
  *         Get the storage Unit info
  * @param  phost: Host handle
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_MTP_GetNumObjects (USBH_HandleTypeDef *phost, 
                                           uint32_t storage_idx, 
                                           uint32_t objectformatcode, 
                                           uint32_t associationOH,
                                           uint32_t* numobs)
{
  USBH_StatusTypeDef status = USBH_FAIL;
  MTP_HandleTypeDef *MTP_Handle =  phost->pActiveClass->pData;
  uint32_t timeout = phost->Timer + 5000;
  if((storage_idx < MTP_Handle->info.storids.n) && (MTP_Handle->is_ready))
  {
    while ((status = USBH_PTP_GetNumObjects (phost, 
                                             MTP_Handle->info.storids.Storage[storage_idx], 
                                             objectformatcode, 
                                             associationOH,
                                             numobs)) == USBH_BUSY)
    {
      if((phost->Timer > timeout) || (phost->device.is_connected == 0)) 
      {
        return USBH_FAIL;
      }
    }
  }
  return status;
}


/**
  * @brief  USBH_MTP_GetStorageInfo 
  *         Get the storage Unit info
  * @param  phost: Host handle
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_MTP_GetObjectHandles (USBH_HandleTypeDef *phost, 
                                           uint32_t storage_idx, 
                                           uint32_t objectformatcode, 
                                           uint32_t associationOH,
                                           PTP_ObjectHandlesTypedef* objecthandles)
{
  USBH_StatusTypeDef status = USBH_FAIL;
  MTP_HandleTypeDef *MTP_Handle =  phost->pActiveClass->pData;
  uint32_t timeout = phost->Timer + 5000;
  
  if((storage_idx < MTP_Handle->info.storids.n) && (MTP_Handle->is_ready))
  {  
    while ((status = USBH_PTP_GetObjectHandles (phost, 
                                                MTP_Handle->info.storids.Storage[storage_idx], 
                                                objectformatcode, 
                                                associationOH,
                                                objecthandles)) == USBH_BUSY)
    {
      if((phost->Timer > timeout) || (phost->device.is_connected == 0)) 
      {
        return USBH_FAIL;
      }
    }
  }
  return status;
}

/**
  * @brief  USBH_PTP_GetObjectInfo
  *         Gets objert info
  * @param  phost: Host handle
  * @param  dev_info: Device info structure
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_MTP_GetObjectInfo (USBH_HandleTypeDef *phost, 
                                           uint32_t handle, 
                                           PTP_ObjectInfoTypedef* objectinfo)
{
  USBH_StatusTypeDef status = USBH_FAIL;
  MTP_HandleTypeDef *MTP_Handle =  phost->pActiveClass->pData;
  uint32_t timeout = phost->Timer + 5000;
  
  if(MTP_Handle->is_ready)
  {   
    while ((status = USBH_PTP_GetObjectInfo (phost, handle, objectinfo)) == USBH_BUSY)
    {
      if((phost->Timer > timeout) || (phost->device.is_connected == 0)) 
      {
        return USBH_FAIL;
      }
    }
  }
  return status;
}
/**
  * @brief  USBH_MTP_DeleteObject
  *         Delete an object.
  * @param  phost: Host handle
  * @param  handle : Object Handle
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_MTP_DeleteObject (USBH_HandleTypeDef *phost,
                                          uint32_t handle,
                                          uint32_t objectformatcode)
{
  USBH_StatusTypeDef status = USBH_FAIL;
  MTP_HandleTypeDef *MTP_Handle =  phost->pActiveClass->pData;
  uint32_t timeout = phost->Timer + 5000;
  
  if(MTP_Handle->is_ready)
  {   
    while ((status = USBH_PTP_DeleteObject (phost, handle, objectformatcode)) == USBH_BUSY)
    {
      if((phost->Timer > timeout) || (phost->device.is_connected == 0)) 
      {
        return USBH_FAIL;
      }
    }
  }
  return status;
}

/**
  * @brief  USBH_MTP_GetObject
  *         Gets object
  * @param  phost: Host handle
  * @param  dev_info: Device info structure
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_MTP_GetObject (USBH_HandleTypeDef *phost, 
                                           uint32_t handle, 
                                           uint8_t *object)
{
  USBH_StatusTypeDef status = USBH_FAIL;
  MTP_HandleTypeDef *MTP_Handle =  phost->pActiveClass->pData;
  uint32_t timeout = phost->Timer + 5000;
  
  if(MTP_Handle->is_ready)
  {   
    while ((status = USBH_PTP_GetObject (phost, handle, object)) == USBH_BUSY)
    {
      if((phost->Timer > timeout) || (phost->device.is_connected == 0)) 
      {
        return USBH_FAIL;
      }
    }
  }
  return status;
}

/**
  * @brief  USBH_MTP_GetPartialObject
  *         Gets object
  * @param  phost: Host handle
  * @param  dev_info: Device info structure
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_MTP_GetPartialObject(USBH_HandleTypeDef *phost, 
                                           uint32_t handle, 
                                           uint32_t offset,
                                           uint32_t maxbytes, 
                                           uint8_t *object,
                                           uint32_t *len)
{
  USBH_StatusTypeDef status = USBH_FAIL;
  MTP_HandleTypeDef *MTP_Handle =  phost->pActiveClass->pData;
  uint32_t timeout = phost->Timer + 5000;
  
  if(MTP_Handle->is_ready)
  {   
    while ((status = USBH_PTP_GetPartialObject(phost, 
                                           handle, 
                                           offset,
                                           maxbytes, 
                                           object,
                                           len)) == USBH_BUSY)
    {
      if((phost->Timer > timeout) || (phost->device.is_connected == 0)) 
      {
        return USBH_FAIL;
      }
    }
  }
  return status;
}

/**
  * @brief  USBH_MTP_GetObjectPropsSupported
  *         Gets object partially
  * @param  phost: Host handle
  * @param  dev_info: Device info structure
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_MTP_GetObjectPropsSupported (USBH_HandleTypeDef *phost,
                                                     uint16_t ofc,
                                                     uint32_t *propnum, 
                                                     uint16_t *props)
{
  USBH_StatusTypeDef status = USBH_FAIL;
  MTP_HandleTypeDef *MTP_Handle =  phost->pActiveClass->pData;
  uint32_t timeout = phost->Timer + 5000;
  
  if(MTP_Handle->is_ready)
  {   
    while ((status = USBH_PTP_GetObjectPropsSupported (phost,
                                                     ofc,
                                                     propnum, 
                                                     props)) == USBH_BUSY)
    {
      if((phost->Timer > timeout) || (phost->device.is_connected == 0)) 
      {
        return USBH_FAIL;
      }
    }
  }
  return status;
}

/**
  * @brief  USBH_MTP_GetObjectPropDesc
  *         Gets object partially
  * @param  phost: Host handle
  * @param  dev_info: Device info structure
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_MTP_GetObjectPropDesc (USBH_HandleTypeDef *phost,
                                                uint16_t opc, 
                                                uint16_t ofc, 
                                                PTP_ObjectPropDescTypeDef *opd)
{
  USBH_StatusTypeDef status = USBH_FAIL;
  MTP_HandleTypeDef *MTP_Handle =  phost->pActiveClass->pData;
  uint32_t timeout = phost->Timer + 5000;
  
  if(MTP_Handle->is_ready)
  {   
    while ((status = USBH_PTP_GetObjectPropDesc (phost,
                                                opc, 
                                                ofc, 
                                                opd)) == USBH_BUSY)
    {
      if((phost->Timer > timeout) || (phost->device.is_connected == 0)) 
      {
        return USBH_FAIL;
      }
    }
  }
  return status; 
}

/**
  * @brief  USBH_MTP_GetObjectPropList
  *         Gets object partially
  * @param  phost: Host handle
  * @param  dev_info: Device info structure
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_MTP_GetObjectPropList (USBH_HandleTypeDef *phost,
                                                uint32_t handle, 
                                                MTP_PropertiesTypedef *pprops, 
                                                uint32_t *nrofprops)
{
  USBH_StatusTypeDef status = USBH_FAIL;
  MTP_HandleTypeDef *MTP_Handle =  phost->pActiveClass->pData;
  uint32_t timeout = phost->Timer + 5000;
  
  if(MTP_Handle->is_ready)
  {   
    while ((status = USBH_PTP_GetObjectPropList (phost,
                                                handle, 
                                                pprops, 
                                                nrofprops)) == USBH_BUSY)
    {
      if((phost->Timer > timeout) || (phost->device.is_connected == 0)) 
      {
        return USBH_FAIL;
      }
    }
  }
  return status; 
}

/**
  * @brief  USBH_MTP_SendObject
  *         Send an object
  * @param  phost: Host handle
  * @param  dev_info: Device info structure
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_MTP_SendObject (USBH_HandleTypeDef *phost, 
                                           uint32_t handle, 
                                           uint8_t *object,
                                           uint32_t size)
{
  USBH_StatusTypeDef status = USBH_FAIL;
  MTP_HandleTypeDef *MTP_Handle =  phost->pActiveClass->pData;
  uint32_t timeout = phost->Timer + 5000;
  
  if(MTP_Handle->is_ready)
  {   
    while ((status = USBH_PTP_SendObject (phost, handle, object, size)) == USBH_BUSY)
    {
      if((phost->Timer > timeout) || (phost->device.is_connected == 0)) 
      {
        return USBH_FAIL;
      }
    }
  }
  return status;
}

 /**
  * @brief  Handle HID Control process
  * @param  phost: Host handle
  * @retval USBH Status
  */
static USBH_StatusTypeDef USBH_MTP_Events (USBH_HandleTypeDef *phost)
{
  USBH_StatusTypeDef status = USBH_BUSY ;
  MTP_HandleTypeDef *MTP_Handle =  phost->pActiveClass->pData; 
 
  
  switch(MTP_Handle->events.state)
  {
  case MTP_EVENTS_INIT:
    if((phost->Timer & 1) == 0)
    {
      MTP_Handle->events.timer = phost->Timer;
      USBH_InterruptReceiveData(phost, 
                                (uint8_t *)&(MTP_Handle->events.container),
                                MTP_Handle->NotificationEpSize,
                                MTP_Handle->NotificationPipe);
      
      
     MTP_Handle->events.state = MTP_EVENTS_GETDATA ;
    }
    break;
  case MTP_EVENTS_GETDATA:
    if(USBH_LL_GetURBState(phost , MTP_Handle->NotificationPipe) == USBH_URB_DONE)
    {
      MTP_DecodeEvent(phost);
    }
    
    if(( phost->Timer - MTP_Handle->events.timer) >= MTP_Handle->events.poll)
    {
     MTP_Handle->events.timer = phost->Timer;
      
      USBH_InterruptReceiveData(phost, 
                                (uint8_t *)&(MTP_Handle->events.container),
                                MTP_Handle->NotificationEpSize,
                                MTP_Handle->NotificationPipe);     
      
    }
    break;

  default:  
    break; 
  }
    
  return status;  
}

/**
  * @brief  MTP_DecodeEvent
  *         Decode device event sent by responder
  * @param  phost: Host handle
  * @retval None
  */
static void MTP_DecodeEvent (USBH_HandleTypeDef *phost) 
{
  MTP_HandleTypeDef *MTP_Handle =  phost->pActiveClass->pData; 
  
  uint16_t code;
  uint32_t param1;

  /* Process the event */
  code = MTP_Handle->events.container.code;
  param1 = MTP_Handle->events.container.param1;
  
 switch(code) 
 {
    case PTP_EC_Undefined:
      USBH_DbgLog("EVT: PTP_EC_Undefined in session %u", MTP_Handle->ptp.session_id);
      break;
    case PTP_EC_CancelTransaction:
      USBH_DbgLog("EVT: PTP_EC_CancelTransaction in session %u", MTP_Handle->ptp.session_id);
      break;
    case PTP_EC_ObjectAdded:
      USBH_DbgLog("EVT: PTP_EC_ObjectAdded in session %u", MTP_Handle->ptp.session_id);
      break;
      
    case PTP_EC_ObjectRemoved:
      USBH_DbgLog("EVT: PTP_EC_ObjectRemoved in session %u", MTP_Handle->ptp.session_id);
      break;
      
    case PTP_EC_StoreAdded:
      USBH_DbgLog("EVT: PTP_EC_StoreAdded in session %u", MTP_Handle->ptp.session_id);
      break;
      
    case PTP_EC_StoreRemoved:
      USBH_DbgLog("EVT: PTP_EC_StoreRemoved in session %u", MTP_Handle->ptp.session_id);
      break;
      
    case PTP_EC_DevicePropChanged:
      USBH_DbgLog("EVT: PTP_EC_DevicePropChanged in session %u", MTP_Handle->ptp.session_id);
      break;
      
    case PTP_EC_ObjectInfoChanged:
      USBH_DbgLog("EVT: PTP_EC_ObjectInfoChanged in session %u", MTP_Handle->ptp.session_id);
      break;
      
    case PTP_EC_DeviceInfoChanged:
      USBH_DbgLog("EVT: PTP_EC_DeviceInfoChanged in session %u", MTP_Handle->ptp.session_id);
      break;
      
    case PTP_EC_RequestObjectTransfer:
      USBH_DbgLog("EVT: PTP_EC_RequestObjectTransfer in session %u", MTP_Handle->ptp.session_id);
      break;
      
    case PTP_EC_StoreFull:
      USBH_DbgLog("EVT: PTP_EC_StoreFull in session %u", MTP_Handle->ptp.session_id);
      break;
      
    case PTP_EC_DeviceReset:
      USBH_DbgLog("EVT: PTP_EC_DeviceReset in session %u", MTP_Handle->ptp.session_id);
      break;
      
    case PTP_EC_StorageInfoChanged :
      USBH_DbgLog( "EVT: PTP_EC_StorageInfoChanged in session %u", MTP_Handle->ptp.session_id);
      break;
      
    case PTP_EC_CaptureComplete :
      USBH_DbgLog( "EVT: PTP_EC_CaptureComplete in session %u", MTP_Handle->ptp.session_id);
      break;
      
    case PTP_EC_UnreportedStatus :
      USBH_DbgLog( "EVT: PTP_EC_UnreportedStatus in session %u", MTP_Handle->ptp.session_id);
      break;
      
    default :
      USBH_DbgLog( "Received unknown event in session %u", MTP_Handle->ptp.session_id);
      break;
  }
 
 USBH_MTP_EventsCallback(phost, code, param1);
}

/**
  * @brief  USBH_MTP_GetDevicePropDesc
  *         Gets object partially
  * @param  phost: Host handle
  * @param  dev_info: Device info structure
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_MTP_GetDevicePropDesc (USBH_HandleTypeDef *phost,
                                                uint16_t propcode, 
			                        PTP_DevicePropDescTypdef* devicepropertydesc)

{
  USBH_StatusTypeDef status = USBH_FAIL;
  MTP_HandleTypeDef *MTP_Handle =  phost->pActiveClass->pData;
  uint32_t timeout = phost->Timer + 5000;
  
  if(MTP_Handle->is_ready)
  {   
    while ((status = USBH_PTP_GetDevicePropDesc (phost, propcode, devicepropertydesc)) == USBH_BUSY)
    {
      if((phost->Timer > timeout) || (phost->device.is_connected == 0)) 
      {
        return USBH_FAIL;
      }
    }
  }
  return status;
}
/**
  * @brief  The function informs that host has rceived an event
  *  @param  pdev: Selected device
  * @retval None
  */
__weak void USBH_MTP_EventsCallback(USBH_HandleTypeDef *phost, uint32_t event, uint32_t param)
{
  
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
