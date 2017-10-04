/**
  ******************************************************************************
  * @file    usbh_mtp_ptp.c 
  * @author  MCD Application Team
  * @version V3.0.0
  * @date    18-February-2014
  * @brief   This file includes the PTP operations layer
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
#include "usbh_mtp_ptp.h"
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

/** @defgroup USBH_MTP_PTP
* @brief    This file includes the mass storage related functions
* @{
*/ 


/** @defgroup USBH_MTP_PTP_Private_TypesDefinitions
* @{
*/ 
/**
* @}
*/ 

/** @defgroup USBH_MTP_PTP_Private_Defines
* @{
*/ 
/**
* @}
*/ 

/** @defgroup USBH_MTP_PTP_Private_Macros
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBH_MTP_PTP_Private_Variables
* @{
*/ 

/**
* @}
*/ 


/** @defgroup USBH_MTP_PTP_Private_FunctionPrototypes
* @{
*/ 
static void PTP_DecodeDeviceInfo (USBH_HandleTypeDef *phost, PTP_DeviceInfoTypedef *dev_info);
static void PTP_GetStorageIDs (USBH_HandleTypeDef *phost, PTP_StorageIDsTypedef *stor_ids);
static void PTP_GetStorageInfo (USBH_HandleTypeDef *phost, uint32_t storage_id, PTP_StorageInfoTypedef *stor_info);
static void PTP_GetObjectPropDesc (USBH_HandleTypeDef *phost, PTP_ObjectPropDescTypeDef *opd, uint32_t opdlen);
static void PTP_DecodeDeviceInfo (USBH_HandleTypeDef *phost, PTP_DeviceInfoTypedef *dev_info);
static void PTP_GetDevicePropValue(USBH_HandleTypeDef *phost, 
                                   uint32_t *offset, 
                                   uint32_t total, 
                                   PTP_PropertyValueTypedef* value, 
                                   uint16_t datatype);

static uint32_t PTP_GetObjectPropList (USBH_HandleTypeDef *phost, 
                                       MTP_PropertiesTypedef *props, 
                                       uint32_t len);


static void PTP_BufferFullCallback(USBH_HandleTypeDef *phost);
static void PTP_GetString(uint8_t *str, uint8_t* data, uint16_t *len);
static uint32_t PTP_GetArray16 (uint16_t *array, uint8_t *data, uint32_t offset);
static uint32_t PTP_GetArray32 (uint32_t *array, uint8_t *data, uint32_t offset);
/**
* @}
*/ 


/** @defgroup USBH_MTP_PTP_Exported_Variables
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBH_MTP_PTP_Private_Functions
* @{
*/ 
/**
  * @brief  USBH_PTP_Init 
  *         The function Initializes the BOT protocol.
  * @param  phost: Host handle
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_PTP_Init(USBH_HandleTypeDef *phost)
{
  MTP_HandleTypeDef *MTP_Handle =  phost->pActiveClass->pData;
  
  /* Set state to idle to be ready for operations */
  MTP_Handle->ptp.state = PTP_IDLE;
  MTP_Handle->ptp.req_state = PTP_REQ_SEND;
  
  return USBH_OK;
}

/**
  * @brief  USBH_PTP_Process 
  *         The function handle the BOT protocol.
  * @param  phost: Host handle
  * @param  lun: Logical Unit Number
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_PTP_Process (USBH_HandleTypeDef *phost)
{
  USBH_StatusTypeDef   status = USBH_BUSY;
  USBH_URBStateTypeDef URB_Status = USBH_URB_IDLE;  
  MTP_HandleTypeDef    *MTP_Handle =  phost->pActiveClass->pData; 
  PTP_ContainerTypedef  ptp_container;  
  uint32_t  len;
  
  switch (MTP_Handle->ptp.state)
  {
  case PTP_IDLE:
    /*Do Nothing */
    break;
    
  case PTP_OP_REQUEST_STATE:  
    USBH_BulkSendData (phost,
                       (uint8_t*)&(MTP_Handle->ptp.op_container),
                       MTP_Handle->ptp.op_container.length, 
                       MTP_Handle->DataOutPipe,
                       1);
    MTP_Handle->ptp.state = PTP_OP_REQUEST_WAIT_STATE;
    break;
    
  case PTP_OP_REQUEST_WAIT_STATE:
    URB_Status = USBH_LL_GetURBState(phost, MTP_Handle->DataOutPipe);
    
    if(URB_Status == USBH_URB_DONE)
    {
       if(MTP_Handle->ptp.flags == PTP_DP_NODATA)
       {
         MTP_Handle->ptp.state = PTP_RESPONSE_STATE;
       }
       else  if(MTP_Handle->ptp.flags == PTP_DP_SENDDATA)
       {
         MTP_Handle->ptp.state = PTP_DATA_OUT_PHASE_STATE;
       }
       else  if(MTP_Handle->ptp.flags == PTP_DP_GETDATA)
       {
         MTP_Handle->ptp.state = PTP_DATA_IN_PHASE_STATE;
       }
#if (USBH_USE_OS == 1)
    osMessagePut ( phost->os_event, USBH_URB_EVENT, 0);
#endif  
    }
    else if(URB_Status == USBH_URB_NOTREADY)
    {
      /* Re-send Request */
      MTP_Handle->ptp.state = PTP_OP_REQUEST_STATE;
#if (USBH_USE_OS == 1)
    osMessagePut ( phost->os_event, USBH_URB_EVENT, 0);
#endif       
    }     
    else if(URB_Status == USBH_URB_STALL)
    {
      MTP_Handle->ptp.state  = PTP_ERROR;
#if (USBH_USE_OS == 1)
     osMessagePut ( phost->os_event, USBH_URB_EVENT, 0);
#endif 
    }
    break;
    
  case PTP_DATA_OUT_PHASE_STATE:
    
    USBH_BulkSendData (phost,
                       MTP_Handle->ptp.data_ptr,
                       MTP_Handle->DataOutEpSize , 
                       MTP_Handle->DataOutPipe,
                       1);
    
    
    MTP_Handle->ptp.state  = PTP_DATA_OUT_PHASE_WAIT_STATE;    
    break;
    
  case PTP_DATA_OUT_PHASE_WAIT_STATE:
    URB_Status = USBH_LL_GetURBState(phost, MTP_Handle->DataOutPipe);     
    
    if(URB_Status == USBH_URB_DONE)
    {
      /* Adjudt Data pointer and data length */
      if(MTP_Handle->ptp.data_length > MTP_Handle->DataOutEpSize)
      {
        MTP_Handle->ptp.data_ptr += MTP_Handle->DataOutEpSize;
        MTP_Handle->ptp.data_length -= MTP_Handle->DataOutEpSize; 
        MTP_Handle->ptp.data_packet += MTP_Handle->DataOutEpSize;
        
        if(MTP_Handle->ptp.data_packet >= PTP_USB_BULK_PAYLOAD_LEN_READ)
        {
          PTP_BufferFullCallback (phost);
          MTP_Handle->ptp.data_packet = 0;
          MTP_Handle->ptp.iteration++;            
        }
        
      }
      else
      {
        MTP_Handle->ptp.data_length = 0;
      } 
      
      /* More Data To be Sent */
      if(MTP_Handle->ptp.data_length > 0)
      {
        USBH_BulkSendData (phost,
                           MTP_Handle->ptp.data_ptr,
                           MTP_Handle->DataOutEpSize , 
                           MTP_Handle->DataOutPipe,
                           1);
      }
      else
      {
        /* If value was 0, and successful transfer, then change the state */
        MTP_Handle->ptp.state  = PTP_RESPONSE_STATE;
      }  
#if (USBH_USE_OS == 1)
    osMessagePut ( phost->os_event, USBH_URB_EVENT, 0);
#endif       
    }
    
    else if(URB_Status == USBH_URB_NOTREADY)
    {
      /* Re-send same data */      
      MTP_Handle->ptp.state = PTP_DATA_OUT_PHASE_STATE;
#if (USBH_USE_OS == 1)
    osMessagePut ( phost->os_event, USBH_URB_EVENT, 0);
#endif       
    }
    
    else if(URB_Status == USBH_URB_STALL)
    {
      MTP_Handle->ptp.state  = PTP_ERROR;    
#if (USBH_USE_OS == 1)
    osMessagePut ( phost->os_event, USBH_URB_EVENT, 0);
#endif       
    }
    break;
    
  case PTP_DATA_IN_PHASE_STATE:
    /* Send first packet */        
    USBH_BulkReceiveData (phost,
                          MTP_Handle->ptp.data_ptr, 
                          MTP_Handle->DataInEpSize, 
                          MTP_Handle->DataInPipe);
    
    MTP_Handle->ptp.state  = PTP_DATA_IN_PHASE_WAIT_STATE;    
    break;
    
  case PTP_DATA_IN_PHASE_WAIT_STATE:
    URB_Status = USBH_LL_GetURBState(phost, MTP_Handle->DataInPipe);
    
    if(URB_Status == USBH_URB_DONE)
    {
      len = USBH_LL_GetLastXferSize (phost, MTP_Handle->DataInPipe);
      
      if( MTP_Handle->ptp.data_packet_counter++ == 0)
      {
        /* This is the first packet; so retrieve exact data length from payload */ 
        MTP_Handle->ptp.data_length = *(uint32_t*)(MTP_Handle->ptp.data_ptr);
        MTP_Handle->ptp.iteration = 0;
      }
      
      if((len >=  MTP_Handle->DataInEpSize) && (MTP_Handle->ptp.data_length > 0))
      {
        MTP_Handle->ptp.data_ptr += len;
        MTP_Handle->ptp.data_length -= len;
        MTP_Handle->ptp.data_packet += len;

        if(MTP_Handle->ptp.data_packet >= PTP_USB_BULK_PAYLOAD_LEN_READ)
        {
          PTP_BufferFullCallback (phost);
          MTP_Handle->ptp.data_packet = 0;
          MTP_Handle->ptp.iteration++;            
        }
        
        /* Continue receiving data*/        
        USBH_BulkReceiveData (phost,
                              MTP_Handle->ptp.data_ptr, 
                              MTP_Handle->DataInEpSize, 
                              MTP_Handle->DataInPipe);
      }
      else
      {
        MTP_Handle->ptp.data_length -= len;
        MTP_Handle->ptp.state = PTP_RESPONSE_STATE;
#if (USBH_USE_OS == 1)
    osMessagePut ( phost->os_event, USBH_URB_EVENT, 0);
#endif          
      }
    }
    else if(URB_Status == USBH_URB_STALL)     
    {
      MTP_Handle->ptp.state  = PTP_ERROR;
#if (USBH_USE_OS == 1)
    osMessagePut ( phost->os_event, USBH_URB_EVENT, 0);
#endif       
    }
    break;
    
  case PTP_RESPONSE_STATE:
    
    USBH_BulkReceiveData (phost,
                          (uint8_t*)&(MTP_Handle->ptp.resp_container), 
                          PTP_USB_BULK_REQ_RESP_MAX_LEN , 
                          MTP_Handle->DataInPipe);
    
    MTP_Handle->ptp.state  = PTP_RESPONSE_WAIT_STATE;
    break;
    
  case PTP_RESPONSE_WAIT_STATE:
    URB_Status = USBH_LL_GetURBState(phost, MTP_Handle->DataInPipe);
    
    if(URB_Status == USBH_URB_DONE)
    {
       USBH_PTP_GetResponse (phost, &ptp_container);
       
       if(ptp_container.Code == PTP_RC_OK)
       {
         status = USBH_OK;
       }
       else
       {
         status = USBH_FAIL;
       }
       MTP_Handle->ptp.req_state = PTP_REQ_SEND;
    }
    else if(URB_Status == USBH_URB_STALL)     
    {
      MTP_Handle->ptp.state  = PTP_ERROR;
#if (USBH_USE_OS == 1)
    osMessagePut ( phost->os_event, USBH_URB_EVENT, 0);
#endif       
    }
    break;
    
  case PTP_ERROR:
      MTP_Handle->ptp.req_state = PTP_REQ_SEND;
      break;
      
  default:
    break;
  }
  return status;
}

/**
  * @brief  USBH_PTP_OpenSession
  *         Open a new session
  * @param  phost: Host handle
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_PTP_SendRequest (USBH_HandleTypeDef *phost, PTP_ContainerTypedef  *req)
{
  USBH_StatusTypeDef            status = USBH_OK; 
  MTP_HandleTypeDef             *MTP_Handle =  phost->pActiveClass->pData; 
  
  /* Clear PTP Data container*/
  USBH_memset(&(MTP_Handle->ptp.op_container), 0, sizeof(PTP_OpContainerTypedef));
  
  /* build appropriate USB container */
  MTP_Handle->ptp.op_container.length = PTP_USB_BULK_REQ_LEN- (sizeof(uint32_t)*(5-req->Nparam));
  MTP_Handle->ptp.op_container.type = PTP_USB_CONTAINER_COMMAND;
  MTP_Handle->ptp.op_container.code = req->Code;
  MTP_Handle->ptp.op_container.trans_id = req->Transaction_ID;
  MTP_Handle->ptp.op_container.param1 = req->Param1;
  MTP_Handle->ptp.op_container.param2 = req->Param2;
  MTP_Handle->ptp.op_container.param3 = req->Param3;
  MTP_Handle->ptp.op_container.param4 = req->Param4;
  MTP_Handle->ptp.op_container.param5 = req->Param5;
  
  return status;
}

/**
  * @brief  USBH_PTP_OpenSession
  *         Open a new session
  * @param  phost: Host handle
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_PTP_GetResponse (USBH_HandleTypeDef *phost, PTP_ContainerTypedef  *resp)
{
  USBH_StatusTypeDef            status = USBH_OK; 
  MTP_HandleTypeDef             *MTP_Handle =  phost->pActiveClass->pData; 
      
  /* build an appropriate PTPContainer */
  resp->Code = MTP_Handle->ptp.resp_container.code;
  resp->SessionID = MTP_Handle->ptp.session_id;
  resp->Transaction_ID = MTP_Handle->ptp.resp_container.trans_id;
  resp->Param1 = MTP_Handle->ptp.resp_container.param1;
  resp->Param2 = MTP_Handle->ptp.resp_container.param2;
  resp->Param3 = MTP_Handle->ptp.resp_container.param3;
  resp->Param4 = MTP_Handle->ptp.resp_container.param4;
  resp->Param5 = MTP_Handle->ptp.resp_container.param5;
  
  return status;
}

/**
  * @brief  The function informs user that data buffer is full
  *  @param  phost: host handle
  * @retval None
  */
void PTP_BufferFullCallback(USBH_HandleTypeDef *phost)
{
  MTP_HandleTypeDef    *MTP_Handle =  phost->pActiveClass->pData; 
  
  switch (MTP_Handle->ptp.data_container.code)
  {
  case PTP_OC_GetDeviceInfo:
    PTP_DecodeDeviceInfo (phost, &(MTP_Handle->info.devinfo));
    break;
    
  case PTP_OC_GetPartialObject:    
  case PTP_OC_GetObject:

    /* first packet is in the PTP data payload buffer */
    if(MTP_Handle->ptp.iteration == 0)
    {
       /* copy it to object */
       USBH_memcpy(MTP_Handle->ptp.object_ptr, MTP_Handle->ptp.data_container.payload.data, PTP_USB_BULK_PAYLOAD_LEN_READ);
       
       /* next packet should be directly copied to object */
       MTP_Handle->ptp.data_ptr = (MTP_Handle->ptp.object_ptr + PTP_USB_BULK_PAYLOAD_LEN_READ); 
    }
    break;

  case PTP_OC_SendObject:    
    /* first packet is in the PTP data payload buffer */
    if(MTP_Handle->ptp.iteration == 0)
    {
       /* next packet should be directly copied to object */
       MTP_Handle->ptp.data_ptr = (MTP_Handle->ptp.object_ptr + PTP_USB_BULK_PAYLOAD_LEN_READ); 
    }
    break;    
    
  default:
    break;
    
    
  }
}

/**
  * @brief  PTP_GetDeviceInfo
  *         Gets device info dataset and fills deviceinfo structure.
  * @param  phost: Host handle
  * @param  dev_info: Device info structure
  * @retval None
  */
static void PTP_DecodeDeviceInfo (USBH_HandleTypeDef *phost, PTP_DeviceInfoTypedef *dev_info)
{
  MTP_HandleTypeDef    *MTP_Handle =  phost->pActiveClass->pData;   
  uint8_t *data = MTP_Handle->ptp.data_container.payload.data;
  uint32_t totallen; 
  uint16_t len;
  
  /* Max device info is PTP_USB_BULK_HS_MAX_PACKET_LEN_READ */
  USBH_DbgLog (" MTP device info size exceeds internal buffer size.\
               only available data are decoded.");
                   
  if(MTP_Handle->ptp.iteration == 0)
  {
    dev_info->StandardVersion = LE16(&data[PTP_di_StandardVersion]);
    dev_info->VendorExtensionID = LE32(&data[PTP_di_VendorExtensionID]);
    dev_info->VendorExtensionVersion = LE16(&data[PTP_di_VendorExtensionVersion]);
    PTP_GetString(dev_info->VendorExtensionDesc, &data[PTP_di_VendorExtensionDesc], &len);
    
    totallen=len*2+1;
    dev_info->FunctionalMode = LE16(&data[PTP_di_FunctionalMode+totallen]);
    dev_info->OperationsSupported_len = PTP_GetArray16 ((uint16_t *)&dev_info->OperationsSupported, 
                                                      data, 
                                                      PTP_di_OperationsSupported+totallen);
    
    totallen=totallen+dev_info->OperationsSupported_len*sizeof(uint16_t)+sizeof(uint32_t);
    dev_info->EventsSupported_len = PTP_GetArray16 ((uint16_t *)&dev_info->EventsSupported, 
                                                      data, 
                                                      PTP_di_OperationsSupported+totallen);
    
    totallen=totallen+dev_info->EventsSupported_len*sizeof(uint16_t)+sizeof(uint32_t);
    dev_info->DevicePropertiesSupported_len = PTP_GetArray16 ((uint16_t *)&dev_info->DevicePropertiesSupported, 
                                                      data, 
                                                      PTP_di_OperationsSupported+totallen);
      
    totallen=totallen+dev_info->DevicePropertiesSupported_len*sizeof(uint16_t)+sizeof(uint32_t);
      
    dev_info->CaptureFormats_len = PTP_GetArray16 ((uint16_t *)&dev_info->CaptureFormats, 
                                                      data, 
                                                      PTP_di_OperationsSupported+totallen);
      
    totallen=totallen+dev_info->CaptureFormats_len*sizeof(uint16_t)+sizeof(uint32_t);
    dev_info->ImageFormats_len =  PTP_GetArray16 ((uint16_t *)&dev_info->ImageFormats, 
                                                      data, 
                                                      PTP_di_OperationsSupported+totallen);
      
    totallen=totallen+dev_info->ImageFormats_len*sizeof(uint16_t)+sizeof(uint32_t); 
    PTP_GetString(dev_info->Manufacturer, &data[PTP_di_OperationsSupported+totallen], &len);

    totallen+=len*2+1;
    PTP_GetString(dev_info->Model, &data[PTP_di_OperationsSupported+totallen], &len);
    
    totallen+=len*2+1;
    PTP_GetString(dev_info->DeviceVersion, &data[PTP_di_OperationsSupported+totallen], &len);

    totallen+=len*2+1;
    PTP_GetString(dev_info->SerialNumber, &data[PTP_di_OperationsSupported+totallen], &len);
  }
}
                 
/**
  * @brief  PTP_GetStorageIDs
  *         Gets Storage Ids and fills stor_ids structure.
  * @param  phost: Host handle
  * @param  stor_ids: Storage IDsstructure
  * @retval None
  */
static void PTP_GetStorageIDs (USBH_HandleTypeDef *phost, PTP_StorageIDsTypedef *stor_ids)
{
  MTP_HandleTypeDef    *MTP_Handle =  phost->pActiveClass->pData;   
  uint8_t *data = MTP_Handle->ptp.data_container.payload.data;

  stor_ids->n = PTP_GetArray32 (stor_ids->Storage, data, 0); 
}

                                  
/**
  * @brief  PTP_GetStorageInfo
  *         Gets Storage Info and fills stor_info structure.
  * @param  phost: Host handle
  * @param  stor_ids: Storage IDsstructure
  * @retval None
  */
static void PTP_GetStorageInfo (USBH_HandleTypeDef *phost, uint32_t storage_id, PTP_StorageInfoTypedef *stor_info)
{
  MTP_HandleTypeDef    *MTP_Handle =  phost->pActiveClass->pData;   
  uint8_t *data = MTP_Handle->ptp.data_container.payload.data;

  uint16_t len;
  
  stor_info->StorageType=LE16(&data[PTP_si_StorageType]);
  stor_info->FilesystemType=LE16(&data[PTP_si_FilesystemType]);
  stor_info->AccessCapability=LE16(&data[PTP_si_AccessCapability]);
  stor_info->MaxCapability=LE64(&data[PTP_si_MaxCapability]);
  stor_info->FreeSpaceInBytes=LE64(&data[PTP_si_FreeSpaceInBytes]);
  stor_info->FreeSpaceInImages=LE32(&data[PTP_si_FreeSpaceInImages]);
  
  PTP_GetString(stor_info->StorageDescription, &data[PTP_si_StorageDescription], &len);
  PTP_GetString(stor_info->VolumeLabel, &data[PTP_si_StorageDescription+len*2+1], &len);   
}

/**
  * @brief  PTP_GetObjectInfo
  *         Gets objectInfo and fills object_info structure.
  * @param  phost: Host handle
  * @param  object_info: object info structure
  * @retval None
  */
static void PTP_GetObjectInfo (USBH_HandleTypeDef *phost, PTP_ObjectInfoTypedef *object_info)
{
  MTP_HandleTypeDef    *MTP_Handle =  phost->pActiveClass->pData;   
  uint8_t *data = MTP_Handle->ptp.data_container.payload.data;
  uint16_t filenamelen;
  
  object_info->StorageID=LE32(&data[PTP_oi_StorageID]);
  object_info->ObjectFormat=LE16(&data[PTP_oi_ObjectFormat]);
  object_info->ProtectionStatus=LE16(&data[PTP_oi_ProtectionStatus]);
  object_info->ObjectCompressedSize=LE32(&data[PTP_oi_ObjectCompressedSize]);
  
  /* For Samsung Galaxy */
  if ((data[PTP_oi_filenamelen] == 0) && (data[PTP_oi_filenamelen+4] != 0)) 
  {
    data += 4;
  }
  object_info->ThumbFormat=LE16(&data[PTP_oi_ThumbFormat]);
  object_info->ThumbCompressedSize=LE32(&data[PTP_oi_ThumbCompressedSize]);
  object_info->ThumbPixWidth=LE32(&data[PTP_oi_ThumbPixWidth]);
  object_info->ThumbPixHeight=LE32(&data[PTP_oi_ThumbPixHeight]);
  object_info->ImagePixWidth=LE32(&data[PTP_oi_ImagePixWidth]);
  object_info->ImagePixHeight=LE32(&data[PTP_oi_ImagePixHeight]);
  object_info->ImageBitDepth=LE32(&data[PTP_oi_ImageBitDepth]);
  object_info->ParentObject=LE32(&data[PTP_oi_ParentObject]);
  object_info->AssociationType=LE16(&data[PTP_oi_AssociationType]);
  object_info->AssociationDesc=LE32(&data[PTP_oi_AssociationDesc]);
  object_info->SequenceNumber=LE32(&data[PTP_oi_SequenceNumber]);
  PTP_GetString(object_info->Filename, &data[PTP_oi_filenamelen], &filenamelen);
}

                 
/**
  * @brief  PTP_GetObjectPropDesc
  *         Gets objectInfo and fills object_info structure.
  * @param  phost: Host handle
  * @param  opd: object prop descriptor structure
  * @retval None
  */
static void PTP_GetObjectPropDesc (USBH_HandleTypeDef *phost, PTP_ObjectPropDescTypeDef *opd, uint32_t opdlen)
{
  MTP_HandleTypeDef    *MTP_Handle =  phost->pActiveClass->pData;   
  uint8_t *data = MTP_Handle->ptp.data_container.payload.data;
  uint32_t offset = 0, i;
  
  opd->ObjectPropertyCode=LE16(&data[PTP_opd_ObjectPropertyCode]);
  opd->DataType=LE16(&data[PTP_opd_DataType]);
  opd->GetSet=*(uint8_t *)(&data[PTP_opd_GetSet]);
  
  offset = PTP_opd_FactoryDefaultValue;
  PTP_GetDevicePropValue (phost, &offset, opdlen, &opd->FactoryDefaultValue, opd->DataType);
  
  opd->GroupCode=LE32(&data[offset]);
  offset+=sizeof(uint32_t);
  
  opd->FormFlag=*(uint8_t *)(&data[offset]);
  offset+=sizeof(uint8_t);
  
  switch (opd->FormFlag) 
  {
  case PTP_OPFF_Range:
    PTP_GetDevicePropValue(phost, &offset, opdlen, &opd->FORM.Range.MinimumValue, opd->DataType);
    PTP_GetDevicePropValue(phost, &offset, opdlen, &opd->FORM.Range.MaximumValue, opd->DataType);
    PTP_GetDevicePropValue(phost, &offset, opdlen, &opd->FORM.Range.StepSize, opd->DataType);
    break;
    
  case PTP_OPFF_Enumeration:
    
    opd->FORM.Enum.NumberOfValues = LE16(&data[offset]);
    offset+=sizeof(uint16_t);
    
    for (i=0 ; i < opd->FORM.Enum.NumberOfValues ; i++) 
    {
      PTP_GetDevicePropValue(phost, &offset, opdlen, &opd->FORM.Enum.SupportedValue[i], opd->DataType);
    }
    break;
  default:
    break;
  }
}
                 
/**
  * @brief  PTP_GetDevicePropValue
  *         Gets objectInfo and fills object_info structure.
  * @param  phost: Host handle
  * @param  opd: object prop descriptor structure
  * @retval None
  */             
static void PTP_GetDevicePropValue(USBH_HandleTypeDef *phost, 
                                   uint32_t *offset, 
                                   uint32_t total, 
                                   PTP_PropertyValueTypedef* value, 
                                   uint16_t datatype)
{
  MTP_HandleTypeDef    *MTP_Handle =  phost->pActiveClass->pData;   
  uint8_t *data = MTP_Handle->ptp.data_container.payload.data;
  uint16_t len;
  switch (datatype) 
  {
  case PTP_DTC_INT8:
    value->i8 = *(uint8_t *)&(data[*offset]);
    *offset += 1;
    break;
  case PTP_DTC_UINT8:
    value->u8 = *(uint8_t *)&(data[*offset]);
    *offset += 1;                
    break;
  case PTP_DTC_INT16:
    
    value->i16 = LE16(&(data[*offset]));
    *offset += 2;                
    break;
  case PTP_DTC_UINT16:
    value->u16 = LE16(&(data[*offset]));                
    *offset += 2;                
    break;
  case PTP_DTC_INT32:
    value->i32 = LE32(&(data[*offset]));              
    *offset += 4;                
    break;
  case PTP_DTC_UINT32:
    value->u32 = LE32(&(data[*offset]));             
    *offset += 4;                
    break;
  case PTP_DTC_INT64:
    value->i64 = LE64(&(data[*offset]));            
    *offset += 8;                
    break;
  case PTP_DTC_UINT64:
    value->u64 = LE64(&(data[*offset]));              
    *offset += 8;                
    break;
    
  case PTP_DTC_UINT128:
    *offset += 16;
    break;
  case PTP_DTC_INT128:
    *offset += 16;
    break;
 
  case PTP_DTC_STR:
    
    PTP_GetString((uint8_t *)value->str, (uint8_t *)&(data[*offset]), &len);
    *offset += len*2+1;
    break;
  default:
    break;
  }
}

                 
/**
  * @brief  PTP_GetDevicePropValue
  *         Gets objectInfo and fills object_info structure.
  * @param  phost: Host handle
  * @param  opd: object prop descriptor structure
  * @retval None
  */             
static uint32_t PTP_GetObjectPropList (USBH_HandleTypeDef *phost, 
                                   MTP_PropertiesTypedef *props, 
                                   uint32_t len)
{ 
  MTP_HandleTypeDef    *MTP_Handle =  phost->pActiveClass->pData;   
  uint8_t *data = MTP_Handle->ptp.data_container.payload.data;
  uint32_t prop_count;
  uint32_t offset = 0, i;
  
 prop_count = LE32(data);

 
 if (prop_count == 0) 
 {
   return 0;
 }

 data += sizeof(uint32_t);
 len -= sizeof(uint32_t);
 
 for (i = 0; i < prop_count; i++) 
 {
   if (len <= 0) 
   {
     return 0;
   }
   
   props[i].ObjectHandle = LE32(data);
   data += sizeof(uint32_t);
   len -= sizeof(uint32_t);
   
   props[i].property = LE16(data);
   data += sizeof(uint16_t);
   len -= sizeof(uint16_t);
   
   props[i].datatype = LE16(data);
   data += sizeof(uint16_t);
   len -= sizeof(uint16_t);
   
   offset = 0;
   
   PTP_GetDevicePropValue(phost, &offset, len, &props[i].propval, props[i].datatype);
   
   data += offset;
   len -= offset;
 }
 
 return prop_count;
}
                 
/**
  * @brief  PTP_GetString
  *         Gets SCII String from.
  * @param  str: ascii string
  * @param  data: Device info structure
  * @retval None
  */
static void PTP_GetString (uint8_t *str, uint8_t* data, uint16_t *len)
{
  uint16_t strlength;
  uint16_t idx;
  
  *len = data[0];
  strlength = 2 * data[0]; 
  data ++; /* Adjust the offset ignoring the String Len */
  
  for (idx = 0; idx < strlength; idx+=2 )
  {
    /* Copy Only the string and ignore the UNICODE ID, hence add the src */
    *str =  data[idx];
    str++;
  }  
  *str = 0; /* mark end of string */  
}
  
/**
  * @brief  PTP_GetString
  *         Gets SCII String from.
  * @param  str: ascii string
  * @param  data: Device info structure
  * @retval None
  */              

static uint32_t PTP_GetArray16 (uint16_t *array, uint8_t *data, uint32_t offset)
{
  uint32_t size, idx = 0;
  
  size=LE32(&data[offset]);
  while (size > idx) 
  {
    array[idx] = LE16(&data[offset+(sizeof(uint16_t)*(idx+2))]);
    idx++;
  }
  return size;
}

/**
  * @brief  PTP_GetString
  *         Gets SCII String from.
  * @param  str: ascii string
  * @param  data: Device info structure
  * @retval None
  */              

static uint32_t PTP_GetArray32 (uint32_t *array, uint8_t *data, uint32_t offset)
{
  uint32_t size, idx = 0;
  
  size=LE32(&data[offset]);
  while (size > idx) 
  {
    array[idx] = LE32(&data[offset+(sizeof(uint32_t)*(idx+1))]);
    idx++;
  }
  return size;
}

/*******************************************************************************
                             
                          PTP Requests

*******************************************************************************/
/**
  * @brief  USBH_PTP_OpenSession
  *         Open a new session
  * @param  phost: Host handle
  * @param  session: Session ID (MUST BE > 0)
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_PTP_OpenSession (USBH_HandleTypeDef *phost, uint32_t session)
{
  USBH_StatusTypeDef   status = USBH_BUSY; 
  MTP_HandleTypeDef    *MTP_Handle =  phost->pActiveClass->pData; 
  PTP_ContainerTypedef  ptp_container;
  
  switch(MTP_Handle->ptp.req_state)
  {
  case PTP_REQ_SEND:

    /* Init session params */
    MTP_Handle->ptp.transaction_id = 0x00000000;
    MTP_Handle->ptp.session_id = session;
    MTP_Handle->ptp.flags = PTP_DP_NODATA;
    
    /* Fill operation request params */      
    ptp_container.Code = PTP_OC_OpenSession;
    ptp_container.SessionID = session;
    ptp_container.Transaction_ID = MTP_Handle->ptp.transaction_id ++;
    ptp_container.Param1 = session;
    ptp_container.Nparam = 1;
    
    /* convert request packet inti USB raw packet*/
    USBH_PTP_SendRequest (phost, &ptp_container); 
        
    /* Setup State machine and start transfer */
    MTP_Handle->ptp.state = PTP_OP_REQUEST_STATE;
    MTP_Handle->ptp.req_state = PTP_REQ_WAIT;
    status = USBH_BUSY; 
    break;
    
  case PTP_REQ_WAIT:
    status = USBH_PTP_Process(phost);
    break;
    
  default:
    break;
  }
  return status;
}

/**
  * @brief  USBH_PTP_GetDevicePropDesc
  *         Gets object partially
  * @param  phost: Host handle
  * @param  dev_info: Device info structure
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_PTP_GetDevicePropDesc (USBH_HandleTypeDef *phost,
                                                uint16_t propcode, 
			PTP_DevicePropDescTypdef* devicepropertydesc)
{
  USBH_StatusTypeDef   status = USBH_BUSY; 
  MTP_HandleTypeDef    *MTP_Handle =  phost->pActiveClass->pData; 
  PTP_ContainerTypedef  ptp_container;
  uint8_t               *data = MTP_Handle->ptp.data_container.payload.data;
  switch(MTP_Handle->ptp.req_state)
  {
  case PTP_REQ_SEND:

    /* Set operation request type */
    MTP_Handle->ptp.flags = PTP_DP_GETDATA;
    MTP_Handle->ptp.data_ptr = (uint8_t *)&(MTP_Handle->ptp.data_container);
    MTP_Handle->ptp.data_length = 0;
    MTP_Handle->ptp.data_packet_counter = 0;
    MTP_Handle->ptp.data_packet = 0;
   
    /* Fill operation request params */      
    ptp_container.Code = PTP_OC_GetDevicePropDesc;
    ptp_container.SessionID = MTP_Handle->ptp.session_id;
    ptp_container.Transaction_ID = MTP_Handle->ptp.transaction_id ++;
    ptp_container.Param1 = propcode;     
    ptp_container.Nparam = 1;
    
    /* convert request packet into USB raw packet*/
    USBH_PTP_SendRequest (phost, &ptp_container); 
        
    /* Setup State machine and start transfer */
    MTP_Handle->ptp.state = PTP_OP_REQUEST_STATE;
    MTP_Handle->ptp.req_state = PTP_REQ_WAIT;
    status = USBH_BUSY; 
    break;
    
  case PTP_REQ_WAIT:
    status = USBH_PTP_Process(phost);
    
    if(status == USBH_OK)
    {
	devicepropertydesc->DevicePropertyCode = LE16(&data[PTP_dpd_DevicePropertyCode]);
	devicepropertydesc->DataType = LE16(&data[PTP_dpd_DataType]);
	devicepropertydesc->GetSet = *(uint8_t *)(&data[PTP_dpd_GetSet]);
	devicepropertydesc->FormFlag =  PTP_DPFF_None;
    }
    break;
    
  default:
    break;
  }
  return status;
}
/**
  * @brief  USBH_PTP_GetDeviceInfo
  *         Gets device info dataset and fills deviceinfo structure.
  * @param  phost: Host handle
  * @param  dev_info: Device info structure
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_PTP_GetDeviceInfo (USBH_HandleTypeDef *phost, PTP_DeviceInfoTypedef *dev_info)
{
  USBH_StatusTypeDef   status = USBH_BUSY; 
  MTP_HandleTypeDef    *MTP_Handle =  phost->pActiveClass->pData; 
  PTP_ContainerTypedef  ptp_container;
  
  switch(MTP_Handle->ptp.req_state)
  {
  case PTP_REQ_SEND:

    /* Set operation request type */
    MTP_Handle->ptp.flags = PTP_DP_GETDATA;
    MTP_Handle->ptp.data_ptr = (uint8_t *)&(MTP_Handle->ptp.data_container);
    MTP_Handle->ptp.data_length = 0;
    MTP_Handle->ptp.data_packet_counter = 0;
    MTP_Handle->ptp.data_packet = 0;
   
    /* Fill operation request params */      
    ptp_container.Code = PTP_OC_GetDeviceInfo;
    ptp_container.SessionID = MTP_Handle->ptp.session_id;
    ptp_container.Transaction_ID = MTP_Handle->ptp.transaction_id ++;
    ptp_container.Nparam = 0;
    
    /* convert request packet inti USB raw packet*/
    USBH_PTP_SendRequest (phost, &ptp_container); 
        
    /* Setup State machine and start transfer */
    MTP_Handle->ptp.state = PTP_OP_REQUEST_STATE;
    MTP_Handle->ptp.req_state = PTP_REQ_WAIT;
    status = USBH_BUSY; 
    break;
    
  case PTP_REQ_WAIT:
    status = USBH_PTP_Process(phost);
    
    if(status == USBH_OK)
    {
       PTP_DecodeDeviceInfo (phost, dev_info);
    }
    break;
    
  default:
    break;
  }
  return status;
}

/**
  * @brief  USBH_PTP_GetStorageIds
  *         Gets device info dataset and fills deviceinfo structure.
  * @param  phost: Host handle
  * @param  dev_info: Device info structure
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_PTP_GetStorageIds (USBH_HandleTypeDef *phost, PTP_StorageIDsTypedef *storage_ids)
{
  USBH_StatusTypeDef   status = USBH_BUSY; 
  MTP_HandleTypeDef    *MTP_Handle =  phost->pActiveClass->pData; 
  PTP_ContainerTypedef  ptp_container;
  
  switch(MTP_Handle->ptp.req_state)
  {
  case PTP_REQ_SEND:

    /* Set operation request type */
    MTP_Handle->ptp.flags = PTP_DP_GETDATA;
    MTP_Handle->ptp.data_ptr = (uint8_t *)&(MTP_Handle->ptp.data_container);
    MTP_Handle->ptp.data_length = 0;
    MTP_Handle->ptp.data_packet_counter = 0;
    MTP_Handle->ptp.data_packet = 0;
   
    /* Fill operation request params */      
    ptp_container.Code = PTP_OC_GetStorageIDs;
    ptp_container.SessionID = MTP_Handle->ptp.session_id;
    ptp_container.Transaction_ID = MTP_Handle->ptp.transaction_id ++;
    ptp_container.Nparam = 0;
    
    /* convert request packet inti USB raw packet*/
    USBH_PTP_SendRequest (phost, &ptp_container); 
        
    /* Setup State machine and start transfer */
    MTP_Handle->ptp.state = PTP_OP_REQUEST_STATE;
    MTP_Handle->ptp.req_state = PTP_REQ_WAIT;
    status = USBH_BUSY; 
    break;
    
  case PTP_REQ_WAIT:
    status = USBH_PTP_Process(phost);
    
    if(status == USBH_OK)
    {
      PTP_GetStorageIDs (phost, storage_ids);
    }
    break;
    
  default:
    break;
  }
  return status;
}
                 
/**
  * @brief  USBH_PTP_GetDeviceInfo
  *         Gets device info dataset and fills deviceinfo structure.
  * @param  phost: Host handle
  * @param  dev_info: Device info structure
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_PTP_GetStorageInfo (USBH_HandleTypeDef *phost, uint32_t storage_id, PTP_StorageInfoTypedef *storage_info)
{
  USBH_StatusTypeDef   status = USBH_BUSY; 
  MTP_HandleTypeDef    *MTP_Handle =  phost->pActiveClass->pData; 
  PTP_ContainerTypedef  ptp_container;
  
  switch(MTP_Handle->ptp.req_state)
  {
  case PTP_REQ_SEND:

    /* Set operation request type */
    MTP_Handle->ptp.flags = PTP_DP_GETDATA;
    MTP_Handle->ptp.data_ptr = (uint8_t *)&(MTP_Handle->ptp.data_container);
    MTP_Handle->ptp.data_length = 0;
    MTP_Handle->ptp.data_packet_counter = 0;
    MTP_Handle->ptp.data_packet = 0;
   
    /* Fill operation request params */      
    ptp_container.Code = PTP_OC_GetStorageInfo;
    ptp_container.SessionID = MTP_Handle->ptp.session_id;
    ptp_container.Transaction_ID = MTP_Handle->ptp.transaction_id ++;
    ptp_container.Param1 = storage_id;    
    ptp_container.Nparam = 1;
    
    /* convert request packet inti USB raw packet*/
    USBH_PTP_SendRequest (phost, &ptp_container); 
        
    /* Setup State machine and start transfer */
    MTP_Handle->ptp.state = PTP_OP_REQUEST_STATE;
    MTP_Handle->ptp.req_state = PTP_REQ_WAIT;
    status = USBH_BUSY; 
    break;
    
  case PTP_REQ_WAIT:
    status = USBH_PTP_Process(phost);
    
    if(status == USBH_OK)
    {
      PTP_GetStorageInfo (phost, storage_id, storage_info);
    }
    break;
    
  default:
    break;
  }
  return status;
}
                 
/**
  * @brief  USBH_PTP_GetNumObjects
  *         Gets device info dataset and fills deviceinfo structure.
  * @param  phost: Host handle
  * @param  dev_info: Device info structure
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_PTP_GetNumObjects (USBH_HandleTypeDef *phost, 
                                           uint32_t storage_id, 
                                           uint32_t objectformatcode, 
                                           uint32_t associationOH,
                                           uint32_t* numobs)
{
  USBH_StatusTypeDef   status = USBH_BUSY; 
  MTP_HandleTypeDef    *MTP_Handle =  phost->pActiveClass->pData; 
  PTP_ContainerTypedef  ptp_container;
  
  switch(MTP_Handle->ptp.req_state)
  {
  case PTP_REQ_SEND:

    /* Set operation request type */
    MTP_Handle->ptp.flags = PTP_DP_NODATA;
   
    /* Fill operation request params */      
    ptp_container.Code = PTP_OC_GetNumObjects;
    ptp_container.SessionID = MTP_Handle->ptp.session_id;
    ptp_container.Transaction_ID = MTP_Handle->ptp.transaction_id ++;
    ptp_container.Param1 = storage_id;  
    ptp_container.Param2 = objectformatcode;
    ptp_container.Param3 = associationOH;    
    ptp_container.Nparam = 3;
    
    /* convert request packet into USB raw packet*/
    USBH_PTP_SendRequest (phost, &ptp_container); 
        
    /* Setup State machine and start transfer */
    MTP_Handle->ptp.state = PTP_OP_REQUEST_STATE;
    MTP_Handle->ptp.req_state = PTP_REQ_WAIT;
    status = USBH_BUSY; 
    break;
    
  case PTP_REQ_WAIT:
    status = USBH_PTP_Process(phost);
    
    if(status == USBH_OK)
    {
      *numobs = MTP_Handle->ptp.resp_container.param1;
    }
    break;
    
  default:
    break;
  }
  return status;
}

/**
  * @brief  USBH_PTP_GetObjectHandles
  *         Gets device info dataset and fills deviceinfo structure.
  * @param  phost: Host handle
  * @param  dev_info: Device info structure
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_PTP_GetObjectHandles (USBH_HandleTypeDef *phost, 
                                           uint32_t storage_id, 
                                           uint32_t objectformatcode, 
                                           uint32_t associationOH,
                                           PTP_ObjectHandlesTypedef* objecthandles)
{
  USBH_StatusTypeDef   status = USBH_BUSY; 
  MTP_HandleTypeDef    *MTP_Handle =  phost->pActiveClass->pData; 
  PTP_ContainerTypedef  ptp_container;
  
  switch(MTP_Handle->ptp.req_state)
  {
  case PTP_REQ_SEND:

    /* Set operation request type */
    MTP_Handle->ptp.flags = PTP_DP_GETDATA;
    MTP_Handle->ptp.data_ptr = (uint8_t *)&(MTP_Handle->ptp.data_container);
    MTP_Handle->ptp.data_length = 0;
    MTP_Handle->ptp.data_packet_counter = 0;
    MTP_Handle->ptp.data_packet = 0;
   
    /* Fill operation request params */      
    ptp_container.Code = PTP_OC_GetObjectHandles;
    ptp_container.SessionID = MTP_Handle->ptp.session_id;
    ptp_container.Transaction_ID = MTP_Handle->ptp.transaction_id ++;
    ptp_container.Param1 = storage_id;  
    ptp_container.Param2 = objectformatcode;
    ptp_container.Param3 = associationOH;    
    ptp_container.Nparam = 3;
    
    /* convert request packet into USB raw packet*/
    USBH_PTP_SendRequest (phost, &ptp_container); 
        
    /* Setup State machine and start transfer */
    MTP_Handle->ptp.state = PTP_OP_REQUEST_STATE;
    MTP_Handle->ptp.req_state = PTP_REQ_WAIT;
    status = USBH_BUSY; 
    break;
    
  case PTP_REQ_WAIT:
    status = USBH_PTP_Process(phost);
    
    if(status == USBH_OK)
    {
       objecthandles->n = PTP_GetArray32 (objecthandles->Handler, 
                                          MTP_Handle->ptp.data_container.payload.data, 
                                          0); 
    }
    break;
    
  default:
    break;
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
USBH_StatusTypeDef USBH_PTP_GetObjectInfo (USBH_HandleTypeDef *phost, 
                                           uint32_t handle, 
                                           PTP_ObjectInfoTypedef* object_info)
{
  USBH_StatusTypeDef   status = USBH_BUSY; 
  MTP_HandleTypeDef    *MTP_Handle =  phost->pActiveClass->pData; 
  PTP_ContainerTypedef  ptp_container;
  
  switch(MTP_Handle->ptp.req_state)
  {
  case PTP_REQ_SEND:

    /* Set operation request type */
    MTP_Handle->ptp.flags = PTP_DP_GETDATA;
    MTP_Handle->ptp.data_ptr = (uint8_t *)&(MTP_Handle->ptp.data_container);
    MTP_Handle->ptp.data_length = 0;
    MTP_Handle->ptp.data_packet_counter = 0;
    MTP_Handle->ptp.data_packet = 0;
   
    /* Fill operation request params */      
    ptp_container.Code = PTP_OC_GetObjectInfo;
    ptp_container.SessionID = MTP_Handle->ptp.session_id;
    ptp_container.Transaction_ID = MTP_Handle->ptp.transaction_id ++;
    ptp_container.Param1 = handle;  
    ptp_container.Nparam = 1;
    
    /* convert request packet into USB raw packet*/
    USBH_PTP_SendRequest (phost, &ptp_container); 
        
    /* Setup State machine and start transfer */
    MTP_Handle->ptp.state = PTP_OP_REQUEST_STATE;
    MTP_Handle->ptp.req_state = PTP_REQ_WAIT;
    status = USBH_BUSY; 
    break;
    
  case PTP_REQ_WAIT:
    status = USBH_PTP_Process(phost);
    
    if(status == USBH_OK)
    {
       PTP_GetObjectInfo (phost, object_info);
    }
    break;
    
  default:
    break;
  }
  return status;
}
                 
/**
  * @brief  USBH_PTP_DeleteObject
  *         Delete an object.
  * @param  phost: Host handle
  * @param  handle : Object Handle
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_PTP_DeleteObject (USBH_HandleTypeDef *phost,
                                          uint32_t handle,
                                          uint32_t objectformatcode)
{
  USBH_StatusTypeDef   status = USBH_BUSY; 
  MTP_HandleTypeDef    *MTP_Handle =  phost->pActiveClass->pData; 
  PTP_ContainerTypedef  ptp_container;
  
  switch(MTP_Handle->ptp.req_state)
  {
  case PTP_REQ_SEND:

    /* Set operation request type */
    MTP_Handle->ptp.flags = PTP_DP_NODATA;
   
    /* Fill operation request params */      
    ptp_container.Code = PTP_OC_DeleteObject;
    ptp_container.SessionID = MTP_Handle->ptp.session_id;
    ptp_container.Transaction_ID = MTP_Handle->ptp.transaction_id ++;
    ptp_container.Param1 = handle;  
    ptp_container.Param2 = objectformatcode;
    ptp_container.Nparam = 2;
    
    /* convert request packet into USB raw packet*/
    USBH_PTP_SendRequest (phost, &ptp_container); 
        
    /* Setup State machine and start transfer */
    MTP_Handle->ptp.state = PTP_OP_REQUEST_STATE;
    MTP_Handle->ptp.req_state = PTP_REQ_WAIT;
    status = USBH_BUSY; 
    break;
    
  case PTP_REQ_WAIT:
    status = USBH_PTP_Process(phost);
    break;
    
  default:
    break;
  }
  return status;
}
                 
/**
  * @brief  USBH_PTP_GetObject
  *         Gets object
  * @param  phost: Host handle
  * @param  dev_info: Device info structure
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_PTP_GetObject (USBH_HandleTypeDef *phost, 
                                           uint32_t handle, 
                                           uint8_t *object)
{
  USBH_StatusTypeDef   status = USBH_BUSY; 
  MTP_HandleTypeDef    *MTP_Handle =  phost->pActiveClass->pData; 
  PTP_ContainerTypedef  ptp_container;
  
  switch(MTP_Handle->ptp.req_state)
  {
  case PTP_REQ_SEND:

    /* Set operation request type */
    MTP_Handle->ptp.flags = PTP_DP_GETDATA;
    MTP_Handle->ptp.data_ptr = (uint8_t *)&(MTP_Handle->ptp.data_container);
    MTP_Handle->ptp.data_length = 0;
    MTP_Handle->ptp.data_packet_counter = 0;
    MTP_Handle->ptp.data_packet = 0;
    
    /* set object control params */
    MTP_Handle->ptp.object_ptr = object;
    
    /* Fill operation request params */      
    ptp_container.Code = PTP_OC_GetObject;
    ptp_container.SessionID = MTP_Handle->ptp.session_id;
    ptp_container.Transaction_ID = MTP_Handle->ptp.transaction_id ++;
    ptp_container.Param1 = handle;  
    ptp_container.Nparam = 1;

    
    /* convert request packet into USB raw packet*/
    USBH_PTP_SendRequest (phost, &ptp_container); 
        
    /* Setup State machine and start transfer */
    MTP_Handle->ptp.state = PTP_OP_REQUEST_STATE;
    MTP_Handle->ptp.req_state = PTP_REQ_WAIT;
    status = USBH_BUSY; 
    break;
    
  case PTP_REQ_WAIT:
    status = USBH_PTP_Process(phost);
    
    if(status == USBH_OK)
    {
      /* first packet is in the PTP data payload buffer */
      if(MTP_Handle->ptp.iteration == 0)
      {
        /* copy it to object */
        USBH_memcpy(MTP_Handle->ptp.object_ptr, MTP_Handle->ptp.data_container.payload.data, PTP_USB_BULK_PAYLOAD_LEN_READ);
      }
    }
    break;
    
  default:
    break;
  }
  return status;
}
                 
/**
  * @brief  USBH_PTP_GetPartialObject
  *         Gets object partially
  * @param  phost: Host handle
  * @param  dev_info: Device info structure
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_PTP_GetPartialObject(USBH_HandleTypeDef *phost, 
                                           uint32_t handle, 
                                           uint32_t offset,
                                           uint32_t maxbytes, 
                                           uint8_t *object,
                                           uint32_t *len)
{
  USBH_StatusTypeDef   status = USBH_BUSY; 
  MTP_HandleTypeDef    *MTP_Handle =  phost->pActiveClass->pData; 
  PTP_ContainerTypedef  ptp_container;
  
  switch(MTP_Handle->ptp.req_state)
  {
  case PTP_REQ_SEND:

    /* Set operation request type */
    MTP_Handle->ptp.flags = PTP_DP_GETDATA;
    MTP_Handle->ptp.data_ptr = (uint8_t *)&(MTP_Handle->ptp.data_container);
    MTP_Handle->ptp.data_length = 0;
    MTP_Handle->ptp.data_packet_counter = 0;
    MTP_Handle->ptp.data_packet = 0;

    /* set object control params */
    MTP_Handle->ptp.object_ptr = object;
    
    /* Fill operation request params */      
    ptp_container.Code = PTP_OC_GetPartialObject;
    ptp_container.SessionID = MTP_Handle->ptp.session_id;
    ptp_container.Transaction_ID = MTP_Handle->ptp.transaction_id ++;
    ptp_container.Param1 = handle;  
    ptp_container.Param2 = offset;
    ptp_container.Param3 = maxbytes;   
    ptp_container.Nparam = 3;
    
    /* convert request packet into USB raw packet*/
    USBH_PTP_SendRequest (phost, &ptp_container); 
        
    /* Setup State machine and start transfer */
    MTP_Handle->ptp.state = PTP_OP_REQUEST_STATE;
    MTP_Handle->ptp.req_state = PTP_REQ_WAIT;
    status = USBH_BUSY; 
    break;
    
  case PTP_REQ_WAIT:
    status = USBH_PTP_Process(phost);
    
    if(status == USBH_OK)
    {
      *len = MTP_Handle->ptp.resp_container.param1;
      /* first packet is in the PTP data payload buffer */
      if(MTP_Handle->ptp.iteration == 0)
      {
        /* copy it to object */
        USBH_memcpy(MTP_Handle->ptp.object_ptr, MTP_Handle->ptp.data_container.payload.data, *len);
      }      
    }
    break;
    
  default:
    break;
  }
  return status;
}
                 
/**
  * @brief  USBH_PTP_GetObjectPropsSupported
  *         Gets object partially
  * @param  phost: Host handle
  * @param  dev_info: Device info structure
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_PTP_GetObjectPropsSupported (USBH_HandleTypeDef *phost,
                                                     uint16_t ofc,
                                                     uint32_t *propnum, 
                                                     uint16_t *props)
{
  USBH_StatusTypeDef   status = USBH_BUSY; 
  MTP_HandleTypeDef    *MTP_Handle =  phost->pActiveClass->pData; 
  PTP_ContainerTypedef  ptp_container;
  
  switch(MTP_Handle->ptp.req_state)
  {
  case PTP_REQ_SEND:

    /* Set operation request type */
    MTP_Handle->ptp.flags = PTP_DP_GETDATA;
    MTP_Handle->ptp.data_ptr = (uint8_t *)&(MTP_Handle->ptp.data_container);
    MTP_Handle->ptp.data_length = 0;
    MTP_Handle->ptp.data_packet_counter = 0;
    MTP_Handle->ptp.data_packet = 0;
   
    /* Fill operation request params */      
    ptp_container.Code = PTP_OC_GetObjectPropsSupported;
    ptp_container.SessionID = MTP_Handle->ptp.session_id;
    ptp_container.Transaction_ID = MTP_Handle->ptp.transaction_id ++;
    ptp_container.Param1 = ofc;  
    ptp_container.Nparam = 1;
    
    /* convert request packet into USB raw packet*/
    USBH_PTP_SendRequest (phost, &ptp_container); 
        
    /* Setup State machine and start transfer */
    MTP_Handle->ptp.state = PTP_OP_REQUEST_STATE;
    MTP_Handle->ptp.req_state = PTP_REQ_WAIT;
    status = USBH_BUSY; 
    break;
    
  case PTP_REQ_WAIT:
    status = USBH_PTP_Process(phost);
    
    if(status == USBH_OK)
    {
      *propnum = PTP_GetArray16 (props, MTP_Handle->ptp.data_container.payload.data, 0);      
    }
    break;
    
  default:
    break;
  }
  return status;
}
                 
/**
  * @brief  USBH_PTP_GetObjectPropDesc
  *         Gets object partially
  * @param  phost: Host handle
  * @param  dev_info: Device info structure
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_PTP_GetObjectPropDesc (USBH_HandleTypeDef *phost,
                                                uint16_t opc, 
                                                uint16_t ofc, 
                                                PTP_ObjectPropDescTypeDef *opd)
{
  USBH_StatusTypeDef   status = USBH_BUSY; 
  MTP_HandleTypeDef    *MTP_Handle =  phost->pActiveClass->pData; 
  PTP_ContainerTypedef  ptp_container;
  
  switch(MTP_Handle->ptp.req_state)
  {
  case PTP_REQ_SEND:

    /* Set operation request type */
    MTP_Handle->ptp.flags = PTP_DP_GETDATA;
    MTP_Handle->ptp.data_ptr = (uint8_t *)&(MTP_Handle->ptp.data_container);
    MTP_Handle->ptp.data_length = 0;
    MTP_Handle->ptp.data_packet_counter = 0;
    MTP_Handle->ptp.data_packet = 0;
   
    /* Fill operation request params */      
    ptp_container.Code = PTP_OC_GetObjectPropDesc;
    ptp_container.SessionID = MTP_Handle->ptp.session_id;
    ptp_container.Transaction_ID = MTP_Handle->ptp.transaction_id ++;
    ptp_container.Param1 = opc;  
    ptp_container.Param2 = ofc;      
    ptp_container.Nparam = 2;
    
    /* convert request packet into USB raw packet*/
    USBH_PTP_SendRequest (phost, &ptp_container); 
        
    /* Setup State machine and start transfer */
    MTP_Handle->ptp.state = PTP_OP_REQUEST_STATE;
    MTP_Handle->ptp.req_state = PTP_REQ_WAIT;
    status = USBH_BUSY; 
    break;
    
  case PTP_REQ_WAIT:
    status = USBH_PTP_Process(phost);
    
    if(status == USBH_OK)
    {
     PTP_GetObjectPropDesc(phost, opd, MTP_Handle->ptp.data_length);
    }
    break;
    
  default:
    break;
  }
  return status;
}
                 
/**
  * @brief  USBH_PTP_GetObjectPropList
  *         Gets object partially
  * @param  phost: Host handle
  * @param  dev_info: Device info structure
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_PTP_GetObjectPropList (USBH_HandleTypeDef *phost,
                                                uint32_t handle, 
                                                MTP_PropertiesTypedef *pprops, 
                                                uint32_t *nrofprops)
{
  USBH_StatusTypeDef   status = USBH_BUSY; 
  MTP_HandleTypeDef    *MTP_Handle =  phost->pActiveClass->pData; 
  PTP_ContainerTypedef  ptp_container;
  
  switch(MTP_Handle->ptp.req_state)
  {
  case PTP_REQ_SEND:

    /* Set operation request type */
    MTP_Handle->ptp.flags = PTP_DP_GETDATA;
    MTP_Handle->ptp.data_ptr = (uint8_t *)&(MTP_Handle->ptp.data_container);
    MTP_Handle->ptp.data_length = 0;
    MTP_Handle->ptp.data_packet_counter = 0;
    MTP_Handle->ptp.data_packet = 0;
   
    /* copy first packet of the object into data container */
    USBH_memcpy(MTP_Handle->ptp.data_container.payload.data, MTP_Handle->ptp.object_ptr, PTP_USB_BULK_PAYLOAD_LEN_READ);
       
    /* Fill operation request params */      
    ptp_container.Code = PTP_OC_GetObjPropList;
    ptp_container.SessionID = MTP_Handle->ptp.session_id;
    ptp_container.Transaction_ID = MTP_Handle->ptp.transaction_id ++;
    ptp_container.Param1 = handle;  
    ptp_container.Param2 = 0x00000000U;  /* 0x00000000U should be "all formats" */
    ptp_container.Param3 = 0xFFFFFFFFU;  /* 0xFFFFFFFFU should be "all properties" */
    ptp_container.Param4 = 0x00000000U;
    ptp_container.Param5 = 0xFFFFFFFFU;  /* Return full tree below the Param1 handle */
    ptp_container.Nparam = 5;  
    
    /* convert request packet into USB raw packet*/
    USBH_PTP_SendRequest (phost, &ptp_container); 
        
    /* Setup State machine and start transfer */
    MTP_Handle->ptp.state = PTP_OP_REQUEST_STATE;
    MTP_Handle->ptp.req_state = PTP_REQ_WAIT;
    status = USBH_BUSY; 
    break;
    
  case PTP_REQ_WAIT:
    status = USBH_PTP_Process(phost);
    
    if(status == USBH_OK)
    {
      PTP_GetObjectPropList (phost, pprops,  MTP_Handle->ptp.data_length);
    }
    break;
    
  default:
    break;
  }
  return status;
}
               
/**
  * @brief  USBH_PTP_SendObject
  *         Send an object
  * @param  phost: Host handle
  * @param  dev_info: Device info structure
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_PTP_SendObject (USBH_HandleTypeDef *phost, 
                                           uint32_t handle, 
                                           uint8_t *object,
                                           uint32_t size)
{
  USBH_StatusTypeDef   status = USBH_BUSY; 
  MTP_HandleTypeDef    *MTP_Handle =  phost->pActiveClass->pData; 
  PTP_ContainerTypedef  ptp_container;
  
  switch(MTP_Handle->ptp.req_state)
  {
  case PTP_REQ_SEND:

    /* Set operation request type */
    MTP_Handle->ptp.flags = PTP_DP_SENDDATA;
    MTP_Handle->ptp.data_ptr = (uint8_t *)&(MTP_Handle->ptp.data_container);
    MTP_Handle->ptp.data_packet_counter = 0;
    MTP_Handle->ptp.data_packet = 0;
    MTP_Handle->ptp.iteration = 0;
    
    /* set object control params */
    MTP_Handle->ptp.object_ptr = object;
    MTP_Handle->ptp.data_length = size;    
    
    /* Fill operation request params */      
    ptp_container.Code = PTP_OC_SendObject;
    ptp_container.SessionID = MTP_Handle->ptp.session_id;
    ptp_container.Transaction_ID = MTP_Handle->ptp.transaction_id ++;
    ptp_container.Nparam = 0;

    
    /* convert request packet into USB raw packet*/
    USBH_PTP_SendRequest (phost, &ptp_container); 
        
    /* Setup State machine and start transfer */
    MTP_Handle->ptp.state = PTP_OP_REQUEST_STATE;
    MTP_Handle->ptp.req_state = PTP_REQ_WAIT;
    status = USBH_BUSY; 
    break;
    
  case PTP_REQ_WAIT:
    status = USBH_PTP_Process(phost);
    break;
    
  default:
    break;
  }
  return status;
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



