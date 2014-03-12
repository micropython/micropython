/**
  ******************************************************************************
  * @file    usbh_cdc.c
  * @author  MCD Application Team
  * @version V3.0.0
  * @date    18-February-2014
  * @brief   This file is the CDC Layer Handlers for USB Host CDC class.
  *
  * @verbatim
  *      
  *          ===================================================================      
  *                                CDC Class  Description
  *          =================================================================== 
  *           This module manages the MSC class V1.11 following the "Device Class Definition
  *           for Human Interface Devices (CDC) Version 1.11 Jun 27, 2001".
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
#include "usbh_cdc.h"

/** @addtogroup USBH_LIB
* @{
*/

/** @addtogroup USBH_CLASS
* @{
*/

/** @addtogroup USBH_CDC_CLASS
* @{
*/

/** @defgroup USBH_CDC_CORE 
* @brief    This file includes CDC Layer Handlers for USB Host CDC class.
* @{
*/ 

/** @defgroup USBH_CDC_CORE_Private_TypesDefinitions
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBH_CDC_CORE_Private_Defines
* @{
*/ 
#define USBH_CDC_BUFFER_SIZE                 1024
/**
* @}
*/ 


/** @defgroup USBH_CDC_CORE_Private_Macros
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBH_CDC_CORE_Private_Variables
* @{
*/
/**
* @}
*/ 


/** @defgroup USBH_CDC_CORE_Private_FunctionPrototypes
* @{
*/ 

static USBH_StatusTypeDef USBH_CDC_InterfaceInit  (USBH_HandleTypeDef *phost);

static USBH_StatusTypeDef USBH_CDC_InterfaceDeInit  (USBH_HandleTypeDef *phost);

static USBH_StatusTypeDef USBH_CDC_Process(USBH_HandleTypeDef *phost);

static USBH_StatusTypeDef USBH_CDC_SOFProcess(USBH_HandleTypeDef *phost);

static USBH_StatusTypeDef USBH_CDC_ClassRequest (USBH_HandleTypeDef *phost);

static USBH_StatusTypeDef GetLineCoding(USBH_HandleTypeDef *phost, 
                                                 CDC_LineCodingTypeDef *linecoding);

static USBH_StatusTypeDef SetLineCoding(USBH_HandleTypeDef *phost, 
                                                 CDC_LineCodingTypeDef *linecoding);

static void CDC_ProcessTransmission(USBH_HandleTypeDef *phost);

static void CDC_ProcessReception(USBH_HandleTypeDef *phost);

USBH_ClassTypeDef  CDC_Class = 
{
  "CDC",
  USB_CDC_CLASS,
  USBH_CDC_InterfaceInit,
  USBH_CDC_InterfaceDeInit,
  USBH_CDC_ClassRequest,
  USBH_CDC_Process,
  USBH_CDC_SOFProcess,
  NULL,
};
/**
* @}
*/ 


/** @defgroup USBH_CDC_CORE_Private_Functions
* @{
*/ 

/**
  * @brief  USBH_CDC_InterfaceInit 
  *         The function init the CDC class.
  * @param  phost: Host handle
  * @retval USBH Status
  */
static USBH_StatusTypeDef USBH_CDC_InterfaceInit (USBH_HandleTypeDef *phost)
{	

  USBH_StatusTypeDef status = USBH_FAIL ;
  uint8_t interface;
  CDC_HandleTypeDef *CDC_Handle;
  
  interface = USBH_FindInterface(phost, 
                                 COMMUNICATION_INTERFACE_CLASS_CODE, 
                                 ABSTRACT_CONTROL_MODEL, 
                                 COMMON_AT_COMMAND);
  
  if(interface == 0xFF) /* No Valid Interface */
  {
    USBH_DbgLog ("Cannot Find the interface for Communication Interface Class.", phost->pActiveClass->Name);         
  }
  else
  {
    USBH_SelectInterface (phost, interface);
    phost->pActiveClass->pData = (CDC_HandleTypeDef *)USBH_malloc (sizeof(CDC_HandleTypeDef));
    CDC_Handle =  phost->pActiveClass->pData; 
    
    /*Collect the notification endpoint address and length*/
    if(phost->device.CfgDesc.Itf_Desc[interface].Ep_Desc[0].bEndpointAddress & 0x80)
    {
      CDC_Handle->CommItf.NotifEp = phost->device.CfgDesc.Itf_Desc[interface].Ep_Desc[0].bEndpointAddress;
      CDC_Handle->CommItf.NotifEpSize  = phost->device.CfgDesc.Itf_Desc[interface].Ep_Desc[0].wMaxPacketSize;
    }
    
    /*Allocate the length for host channel number in*/
    CDC_Handle->CommItf.NotifPipe = USBH_AllocPipe(phost, CDC_Handle->CommItf.NotifEp);
    
    /* Open pipe for Notification endpoint */
    USBH_OpenPipe  (phost,
                    CDC_Handle->CommItf.NotifPipe,
                    CDC_Handle->CommItf.NotifEp,                            
                    phost->device.address,
                    phost->device.speed,
                    USB_EP_TYPE_INTR,
                    CDC_Handle->CommItf.NotifEpSize); 
    
    USBH_LL_SetToggle (phost, CDC_Handle->CommItf.NotifPipe, 0);    
    
    interface = USBH_FindInterface(phost, 
                                   DATA_INTERFACE_CLASS_CODE, 
                                   RESERVED, 
                                   NO_CLASS_SPECIFIC_PROTOCOL_CODE);
    
    if(interface == 0xFF) /* No Valid Interface */
    {
      USBH_DbgLog ("Cannot Find the interface for Data Interface Class.", phost->pActiveClass->Name);         
    }
    else
    {    
      /*Collect the class specific endpoint address and length*/
      if(phost->device.CfgDesc.Itf_Desc[interface].Ep_Desc[0].bEndpointAddress & 0x80)
      {      
        CDC_Handle->DataItf.InEp = phost->device.CfgDesc.Itf_Desc[interface].Ep_Desc[0].bEndpointAddress;
        CDC_Handle->DataItf.InEpSize  = phost->device.CfgDesc.Itf_Desc[interface].Ep_Desc[0].wMaxPacketSize;
      }
      else
      {
        CDC_Handle->DataItf.OutEp = phost->device.CfgDesc.Itf_Desc[interface].Ep_Desc[0].bEndpointAddress;
        CDC_Handle->DataItf.OutEpSize  = phost->device.CfgDesc.Itf_Desc[interface].Ep_Desc[0].wMaxPacketSize;
      }
      
      if(phost->device.CfgDesc.Itf_Desc[interface].Ep_Desc[1].bEndpointAddress & 0x80)
      {      
        CDC_Handle->DataItf.InEp = phost->device.CfgDesc.Itf_Desc[interface].Ep_Desc[1].bEndpointAddress;
        CDC_Handle->DataItf.InEpSize  = phost->device.CfgDesc.Itf_Desc[interface].Ep_Desc[1].wMaxPacketSize;
      }
      else
      {
        CDC_Handle->DataItf.OutEp = phost->device.CfgDesc.Itf_Desc[interface].Ep_Desc[1].bEndpointAddress;
        CDC_Handle->DataItf.OutEpSize  = phost->device.CfgDesc.Itf_Desc[interface].Ep_Desc[1].wMaxPacketSize;
      }    
      
      /*Allocate the length for host channel number out*/
      CDC_Handle->DataItf.OutPipe = USBH_AllocPipe(phost, CDC_Handle->DataItf.OutEp);
      
      /*Allocate the length for host channel number in*/
      CDC_Handle->DataItf.InPipe = USBH_AllocPipe(phost, CDC_Handle->DataItf.InEp);  
      
      /* Open channel for OUT endpoint */
      USBH_OpenPipe  (phost,
                      CDC_Handle->DataItf.OutPipe,
                      CDC_Handle->DataItf.OutEp,
                      phost->device.address,
                      phost->device.speed,
                      USB_EP_TYPE_BULK,
                      CDC_Handle->DataItf.OutEpSize);  
      /* Open channel for IN endpoint */
      USBH_OpenPipe  (phost,
                      CDC_Handle->DataItf.InPipe,
                      CDC_Handle->DataItf.InEp,
                      phost->device.address,
                      phost->device.speed,
                      USB_EP_TYPE_BULK,
                      CDC_Handle->DataItf.InEpSize);
      
      CDC_Handle->state = CDC_IDLE_STATE;
      
      USBH_LL_SetToggle  (phost, CDC_Handle->DataItf.OutPipe,0);
      USBH_LL_SetToggle  (phost, CDC_Handle->DataItf.InPipe,0);
      status = USBH_OK; 
    }
  }
  return status;
}



/**
  * @brief  USBH_CDC_InterfaceDeInit 
  *         The function DeInit the Pipes used for the CDC class.
  * @param  phost: Host handle
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_CDC_InterfaceDeInit (USBH_HandleTypeDef *phost)
{
  CDC_HandleTypeDef *CDC_Handle =  phost->pActiveClass->pData;
  
  if ( CDC_Handle->CommItf.NotifPipe)
  {
    USBH_ClosePipe(phost, CDC_Handle->CommItf.NotifPipe);
    USBH_FreePipe  (phost, CDC_Handle->CommItf.NotifPipe);
    CDC_Handle->CommItf.NotifPipe = 0;     /* Reset the Channel as Free */
  }
  
  if ( CDC_Handle->DataItf.InPipe)
  {
    USBH_ClosePipe(phost, CDC_Handle->DataItf.InPipe);
    USBH_FreePipe  (phost, CDC_Handle->DataItf.InPipe);
    CDC_Handle->DataItf.InPipe = 0;     /* Reset the Channel as Free */
  }
  
  if ( CDC_Handle->DataItf.OutPipe)
  {
    USBH_ClosePipe(phost, CDC_Handle->DataItf.OutPipe);
    USBH_FreePipe  (phost, CDC_Handle->DataItf.OutPipe);
    CDC_Handle->DataItf.OutPipe = 0;     /* Reset the Channel as Free */
  } 
  
  if(phost->pActiveClass->pData)
  {
    USBH_free (phost->pActiveClass->pData);
    phost->pActiveClass->pData = 0;
  }
   
  return USBH_OK;
}

/**
  * @brief  USBH_CDC_ClassRequest 
  *         The function is responsible for handling Standard requests
  *         for CDC class.
  * @param  phost: Host handle
  * @retval USBH Status
  */
static USBH_StatusTypeDef USBH_CDC_ClassRequest (USBH_HandleTypeDef *phost)
{   
  USBH_StatusTypeDef status = USBH_FAIL ;  
  CDC_HandleTypeDef *CDC_Handle =  phost->pActiveClass->pData;  
  
  /*Issue the get line coding request*/
  status =   GetLineCoding(phost, &CDC_Handle->LineCoding);
  if(status == USBH_OK)
  {
    phost->pUser(phost, HOST_USER_CLASS_ACTIVE); 
  }
  return status;
}


/**
  * @brief  USBH_CDC_Process 
  *         The function is for managing state machine for CDC data transfers 
  * @param  phost: Host handle
  * @retval USBH Status
  */
static USBH_StatusTypeDef USBH_CDC_Process (USBH_HandleTypeDef *phost)
{
  USBH_StatusTypeDef status = USBH_BUSY;
  USBH_StatusTypeDef req_status = USBH_OK;
  CDC_HandleTypeDef *CDC_Handle =  phost->pActiveClass->pData; 
  
  switch(CDC_Handle->state)
  {
    
  case CDC_IDLE_STATE:
    status = USBH_OK;
    break;
    
  case CDC_SET_LINE_CODING_STATE:
    req_status = SetLineCoding(phost, CDC_Handle->pUserLineCoding);
    
    if(req_status == USBH_OK)
    {
      CDC_Handle->state = CDC_GET_LAST_LINE_CODING_STATE; 
    }
    
    else if(req_status != USBH_BUSY)
    {
      CDC_Handle->state = CDC_ERROR_STATE; 
    }
    break;
    
    
  case CDC_GET_LAST_LINE_CODING_STATE:
    req_status = GetLineCoding(phost, &(CDC_Handle->LineCoding));
    
    if(req_status == USBH_OK)
    {
      CDC_Handle->state = CDC_IDLE_STATE; 
      
      if((CDC_Handle->LineCoding.b.bCharFormat == CDC_Handle->pUserLineCoding->b.bCharFormat) && 
         (CDC_Handle->LineCoding.b.bDataBits == CDC_Handle->pUserLineCoding->b.bDataBits) &&
         (CDC_Handle->LineCoding.b.bParityType == CDC_Handle->pUserLineCoding->b.bParityType) &&
         (CDC_Handle->LineCoding.b.dwDTERate == CDC_Handle->pUserLineCoding->b.dwDTERate))
      {
        USBH_CDC_LineCodingChanged(phost);
      }
    }
    
    else if(req_status != USBH_BUSY)
    {
      CDC_Handle->state = CDC_ERROR_STATE; 
    }   

    break;
    
  case CDC_TRANSFER_DATA:
    CDC_ProcessTransmission(phost);
    CDC_ProcessReception(phost);
    break;   
    
  case CDC_ERROR_STATE:
    req_status = USBH_ClrFeature(phost, 0x00); 
    
    if(req_status == USBH_OK )
    {        
      /*Change the state to waiting*/
      CDC_Handle->state = CDC_IDLE_STATE ;
    }    
    break;
    
  default:
    break;
    
  }
  
  return status;
}

/**
  * @brief  USBH_CDC_SOFProcess 
  *         The function is for managing SOF callback 
  * @param  phost: Host handle
  * @retval USBH Status
  */
static USBH_StatusTypeDef USBH_CDC_SOFProcess (USBH_HandleTypeDef *phost)
{
  return USBH_OK;  
}
                                   
  
  /**
  * @brief  USBH_CDC_Stop 
  *         Stop current CDC Transmission 
  * @param  phost: Host handle
  * @retval USBH Status
  */
USBH_StatusTypeDef  USBH_CDC_Stop(USBH_HandleTypeDef *phost)
{
  CDC_HandleTypeDef *CDC_Handle =  phost->pActiveClass->pData; 
  
  if(phost->gState == HOST_CLASS)
  {
    CDC_Handle->state = CDC_IDLE_STATE;
    
    USBH_ClosePipe(phost, CDC_Handle->CommItf.NotifPipe);
    USBH_ClosePipe(phost, CDC_Handle->DataItf.InPipe);
    USBH_ClosePipe(phost, CDC_Handle->DataItf.OutPipe);
  }
  return USBH_OK;  
}
/**
  * @brief  This request allows the host to find out the currently 
  *         configured line coding.
  * @param  pdev: Selected device
  * @retval USBH_StatusTypeDef : USB ctl xfer status
  */
static USBH_StatusTypeDef GetLineCoding(USBH_HandleTypeDef *phost, CDC_LineCodingTypeDef *linecoding)
{
 
  phost->Control.setup.b.bmRequestType = USB_D2H | USB_REQ_TYPE_CLASS | \
                              USB_REQ_RECIPIENT_INTERFACE;
  
  phost->Control.setup.b.bRequest = CDC_GET_LINE_CODING;
  phost->Control.setup.b.wValue.w = 0;
  phost->Control.setup.b.wIndex.w = 0;
  phost->Control.setup.b.wLength.w = LINE_CODING_STRUCTURE_SIZE;           
 
  return USBH_CtlReq(phost, linecoding->Array, LINE_CODING_STRUCTURE_SIZE);
}


/**
  * @brief  This request allows the host to specify typical asynchronous 
  * line-character formatting properties 
  * This request applies to asynchronous byte stream data class interfaces 
  * and endpoints
  * @param  pdev: Selected device
  * @retval USBH_StatusTypeDef : USB ctl xfer status
  */
static USBH_StatusTypeDef SetLineCoding(USBH_HandleTypeDef *phost, CDC_LineCodingTypeDef *linecodin)
{
  phost->Control.setup.b.bmRequestType = USB_H2D | USB_REQ_TYPE_CLASS | \
                              USB_REQ_RECIPIENT_INTERFACE;
  
  phost->Control.setup.b.bRequest = CDC_SET_LINE_CODING;
  phost->Control.setup.b.wValue.w = 0;

  phost->Control.setup.b.wIndex.w = 0;

  phost->Control.setup.b.wLength.w = LINE_CODING_STRUCTURE_SIZE;           
  
  return USBH_CtlReq(phost, linecodin->Array , LINE_CODING_STRUCTURE_SIZE );  
}

/**
* @brief  This function prepares the state before issuing the class specific commands
* @param  None
* @retval None
*/
USBH_StatusTypeDef USBH_CDC_SetLineCoding(USBH_HandleTypeDef *phost, CDC_LineCodingTypeDef *linecodin)
{
  CDC_HandleTypeDef *CDC_Handle =  phost->pActiveClass->pData;
  if(phost->gState == HOST_CLASS)
  {
    CDC_Handle->state = CDC_SET_LINE_CODING_STATE;
    CDC_Handle->pUserLineCoding = linecodin;    
    
#if (USBH_USE_OS == 1)
    osMessagePut ( phost->os_event, USBH_CLASS_EVENT, 0);
#endif  
  }    
  return USBH_OK;
}

/**
* @brief  This function prepares the state before issuing the class specific commands
* @param  None
* @retval None
*/
USBH_StatusTypeDef  USBH_CDC_GetLineCoding(USBH_HandleTypeDef *phost, CDC_LineCodingTypeDef *linecodin)
{
  CDC_HandleTypeDef *CDC_Handle =  phost->pActiveClass->pData; 
  
  if((phost->gState == HOST_CLASS) ||(phost->gState == HOST_CLASS_REQUEST))
  {
    *linecodin = CDC_Handle->LineCoding;
    return USBH_OK;
  }
  else
  {
    return USBH_FAIL;
  }
}

/**
  * @brief  This function return last recieved data size
  * @param  None
  * @retval None
  */
uint16_t USBH_CDC_GetLastReceivedDataSize(USBH_HandleTypeDef *phost)
{
  CDC_HandleTypeDef *CDC_Handle =  phost->pActiveClass->pData; 
  
  if(phost->gState == HOST_CLASS)
  {
    return USBH_LL_GetLastXferSize(phost, CDC_Handle->DataItf.InPipe);;
  }
  else
  {
    return 0;
  }
}

/**
  * @brief  This function prepares the state before issuing the class specific commands
  * @param  None
  * @retval None
  */
USBH_StatusTypeDef  USBH_CDC_Transmit(USBH_HandleTypeDef *phost, uint8_t *pbuff, uint32_t length)
{
  USBH_StatusTypeDef Status = USBH_BUSY;
  CDC_HandleTypeDef *CDC_Handle =  phost->pActiveClass->pData;
  
  if((CDC_Handle->state == CDC_IDLE_STATE) || (CDC_Handle->state == CDC_TRANSFER_DATA))
  {
    CDC_Handle->pTxData = pbuff;
    CDC_Handle->TxDataLength = length;  
    CDC_Handle->state = CDC_TRANSFER_DATA;
    CDC_Handle->data_tx_state = CDC_SEND_DATA; 
    Status = USBH_OK;
  }
  return Status;    
}
  
  
/**
* @brief  This function prepares the state before issuing the class specific commands
* @param  None
* @retval None
*/
USBH_StatusTypeDef  USBH_CDC_Receive(USBH_HandleTypeDef *phost, uint8_t *pbuff, uint32_t length)
{
  USBH_StatusTypeDef Status = USBH_BUSY;
  CDC_HandleTypeDef *CDC_Handle =  phost->pActiveClass->pData;
  
  if((CDC_Handle->state == CDC_IDLE_STATE) || (CDC_Handle->state == CDC_TRANSFER_DATA))
  {
    CDC_Handle->pRxData = pbuff;
    CDC_Handle->RxDataLength = length;  
    CDC_Handle->state = CDC_TRANSFER_DATA;
    CDC_Handle->data_rx_state = CDC_RECEIVE_DATA;     
    Status = USBH_OK;
  }
  return Status;    
} 

/**
* @brief  The function is responsible for sending data to the device
*  @param  pdev: Selected device
* @retval None
*/
static void CDC_ProcessTransmission(USBH_HandleTypeDef *phost)
{
  CDC_HandleTypeDef *CDC_Handle =  phost->pActiveClass->pData;
  USBH_URBStateTypeDef URB_Status = USBH_URB_IDLE;
  
  switch(CDC_Handle->data_tx_state)
  {
 
  case CDC_SEND_DATA:
    if(CDC_Handle->TxDataLength > CDC_Handle->DataItf.OutEpSize)
    {
      USBH_BulkSendData (phost,
                         CDC_Handle->pTxData, 
                         CDC_Handle->DataItf.OutEpSize, 
                         CDC_Handle->DataItf.OutPipe,
                         1);
    }
    else
    {
      USBH_BulkSendData (phost,
                         CDC_Handle->pTxData, 
                         CDC_Handle->TxDataLength, 
                         CDC_Handle->DataItf.OutPipe,
                         1);
    }
    
    CDC_Handle->data_tx_state = CDC_SEND_DATA_WAIT;
    
    break;
    
  case CDC_SEND_DATA_WAIT:
    
    URB_Status = USBH_LL_GetURBState(phost, CDC_Handle->DataItf.OutPipe); 
    
    /*Check the status done for transmssion*/
    if(URB_Status == USBH_URB_DONE )
    {         
      if(CDC_Handle->TxDataLength > CDC_Handle->DataItf.OutEpSize)
      {
        CDC_Handle->TxDataLength -= CDC_Handle->DataItf.OutEpSize ;
        CDC_Handle->pTxData += CDC_Handle->DataItf.OutEpSize;
      }
      else
      {
        CDC_Handle->TxDataLength = 0;
      }
      
      if( CDC_Handle->TxDataLength > 0)
      {
       CDC_Handle->data_tx_state = CDC_SEND_DATA; 
      }
      else
      {
        CDC_Handle->data_tx_state = CDC_IDLE;    
        USBH_CDC_TransmitCallback(phost);

      }
    }
    else if( URB_Status == USBH_URB_NOTREADY )
    {
      CDC_Handle->data_tx_state = CDC_SEND_DATA; 
    }
    break;
  default:
    break;
  }
}
/**
* @brief  This function responsible for reception of data from the device
*  @param  pdev: Selected device
* @retval None
*/

static void CDC_ProcessReception(USBH_HandleTypeDef *phost)
{
  CDC_HandleTypeDef *CDC_Handle =  phost->pActiveClass->pData;
  USBH_URBStateTypeDef URB_Status = USBH_URB_IDLE;
  uint16_t length;

  switch(CDC_Handle->data_rx_state)
  {
    
  case CDC_RECEIVE_DATA:

    USBH_BulkReceiveData (phost,
                          CDC_Handle->pRxData, 
                          CDC_Handle->DataItf.InEpSize, 
                          CDC_Handle->DataItf.InPipe);
    
    CDC_Handle->data_rx_state = CDC_RECEIVE_DATA_WAIT;
    
    break;
    
  case CDC_RECEIVE_DATA_WAIT:
    
    URB_Status = USBH_LL_GetURBState(phost, CDC_Handle->DataItf.InPipe); 
    
    /*Check the status done for reception*/
    if(URB_Status == USBH_URB_DONE )
    {  
      length = USBH_LL_GetLastXferSize(phost, CDC_Handle->DataItf.InPipe);
        
      if(((CDC_Handle->RxDataLength - length) > 0) && (length > CDC_Handle->DataItf.InEpSize))
      {
        CDC_Handle->RxDataLength -= length ;
        CDC_Handle->pRxData += length;
        CDC_Handle->data_rx_state = CDC_RECEIVE_DATA; 
      }
      else
      {
        CDC_Handle->data_rx_state = CDC_IDLE;
        USBH_CDC_ReceiveCallback(phost);
      }
    }
    break;
    
  default:
    break;
  }
}

/**
* @brief  The function informs user that data have been received
*  @param  pdev: Selected device
* @retval None
*/
__weak void USBH_CDC_TransmitCallback(USBH_HandleTypeDef *phost)
{
  
}
  
  /**
* @brief  The function informs user that data have been sent
*  @param  pdev: Selected device
* @retval None
*/
__weak void USBH_CDC_ReceiveCallback(USBH_HandleTypeDef *phost)
{
  
}

  /**
* @brief  The function informs user that Settings have been changed
*  @param  pdev: Selected device
* @retval None
*/
__weak void USBH_CDC_LineCodingChanged(USBH_HandleTypeDef *phost)
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
