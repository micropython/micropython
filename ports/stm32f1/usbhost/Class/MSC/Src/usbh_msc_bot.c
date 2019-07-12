/**
  ******************************************************************************
  * @file    usbh_msc_bot.c 
  * @author  MCD Application Team
  * @version V3.0.0
  * @date    18-February-2014
  * @brief   This file includes the BOT protocol related functions
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
#include "usbh_msc_bot.h"
#include "usbh_msc.h"

/** @addtogroup USBH_LIB
* @{
*/

/** @addtogroup USBH_CLASS
* @{
*/

/** @addtogroup USBH_MSC_CLASS
* @{
*/

/** @defgroup USBH_MSC_BOT 
* @brief    This file includes the mass storage related functions
* @{
*/ 


/** @defgroup USBH_MSC_BOT_Private_TypesDefinitions
* @{
*/ 
/**
* @}
*/ 

/** @defgroup USBH_MSC_BOT_Private_Defines
* @{
*/ 
/**
* @}
*/ 

/** @defgroup USBH_MSC_BOT_Private_Macros
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBH_MSC_BOT_Private_Variables
* @{
*/ 

/**
* @}
*/ 


/** @defgroup USBH_MSC_BOT_Private_FunctionPrototypes
* @{
*/ 
static USBH_StatusTypeDef USBH_MSC_BOT_Abort(USBH_HandleTypeDef *phost, uint8_t lun, uint8_t dir);
static BOT_CSWStatusTypeDef USBH_MSC_DecodeCSW(USBH_HandleTypeDef *phost);
/**
* @}
*/ 


/** @defgroup USBH_MSC_BOT_Exported_Variables
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBH_MSC_BOT_Private_Functions
* @{
*/ 

/**
  * @brief  USBH_MSC_BOT_REQ_Reset 
  *         The function the MSC BOT Reset request.
  * @param  phost: Host handle
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_MSC_BOT_REQ_Reset(USBH_HandleTypeDef *phost)
{
  
  phost->Control.setup.b.bmRequestType = USB_H2D | USB_REQ_TYPE_CLASS | \
                              USB_REQ_RECIPIENT_INTERFACE;
  
  phost->Control.setup.b.bRequest = USB_REQ_BOT_RESET;
  phost->Control.setup.b.wValue.w = 0;
  phost->Control.setup.b.wIndex.w = 0;
  phost->Control.setup.b.wLength.w = 0;           
  
  return USBH_CtlReq(phost, 0 , 0 );  
}

/**
  * @brief  USBH_MSC_BOT_REQ_GetMaxLUN 
  *         The function the MSC BOT GetMaxLUN request.
  * @param  phost: Host handle
  * @param  Maxlun: pointer to Maxlun variable
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_MSC_BOT_REQ_GetMaxLUN(USBH_HandleTypeDef *phost, uint8_t *Maxlun)
{
  phost->Control.setup.b.bmRequestType = USB_D2H | USB_REQ_TYPE_CLASS | \
                              USB_REQ_RECIPIENT_INTERFACE;
  
  phost->Control.setup.b.bRequest = USB_REQ_GET_MAX_LUN;
  phost->Control.setup.b.wValue.w = 0;
  phost->Control.setup.b.wIndex.w = 0;
  phost->Control.setup.b.wLength.w = 1;           
  
  return USBH_CtlReq(phost, Maxlun , 1 ); 
}



/**
  * @brief  USBH_MSC_BOT_Init 
  *         The function Initializes the BOT protocol.
  * @param  phost: Host handle
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_MSC_BOT_Init(USBH_HandleTypeDef *phost)
{
  
  MSC_HandleTypeDef *MSC_Handle =  phost->pActiveClass->pData;
  
  MSC_Handle->hbot.cbw.field.Signature = BOT_CBW_SIGNATURE;
  MSC_Handle->hbot.cbw.field.Tag = BOT_CBW_TAG;
  MSC_Handle->hbot.state = BOT_SEND_CBW;    
  MSC_Handle->hbot.cmd_state = BOT_CMD_SEND;   
  
  return USBH_OK;
}



/**
  * @brief  USBH_MSC_BOT_Process 
  *         The function handle the BOT protocol.
  * @param  phost: Host handle
  * @param  lun: Logical Unit Number
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_MSC_BOT_Process (USBH_HandleTypeDef *phost, uint8_t lun)
{
  USBH_StatusTypeDef   status = USBH_BUSY;
  USBH_StatusTypeDef   error  = USBH_BUSY;  
  BOT_CSWStatusTypeDef CSW_Status = BOT_CSW_CMD_FAILED;
  USBH_URBStateTypeDef URB_Status = USBH_URB_IDLE;
  MSC_HandleTypeDef *MSC_Handle =  phost->pActiveClass->pData;
  uint8_t toggle = 0;
  
  switch (MSC_Handle->hbot.state)
  {
  case BOT_SEND_CBW:
    MSC_Handle->hbot.cbw.field.LUN = lun;
    MSC_Handle->hbot.state = BOT_SEND_CBW_WAIT;    
    USBH_BulkSendData (phost,
                       MSC_Handle->hbot.cbw.data, 
                       BOT_CBW_LENGTH, 
                       MSC_Handle->OutPipe,
                       1);
    
    break;
    
  case BOT_SEND_CBW_WAIT:
    
    URB_Status = USBH_LL_GetURBState(phost, MSC_Handle->OutPipe); 
    
    if(URB_Status == USBH_URB_DONE)
    { 
      if ( MSC_Handle->hbot.cbw.field.DataTransferLength != 0 )
      {
        /* If there is Data Transfer Stage */
        if (((MSC_Handle->hbot.cbw.field.Flags) & USB_REQ_DIR_MASK) == USB_D2H)
        {
          /* Data Direction is IN */
          MSC_Handle->hbot.state = BOT_DATA_IN;
        }
        else
        {
          /* Data Direction is OUT */
          MSC_Handle->hbot.state = BOT_DATA_OUT;
        } 
      }
      
      else
      {/* If there is NO Data Transfer Stage */
        MSC_Handle->hbot.state = BOT_RECEIVE_CSW;
      }
#if (USBH_USE_OS == 1)
    osMessagePut ( phost->os_event, USBH_URB_EVENT, 0);
#endif   
    
    }   
    else if(URB_Status == USBH_URB_NOTREADY)
    {
      /* Re-send CBW */
      MSC_Handle->hbot.state = BOT_SEND_CBW;
#if (USBH_USE_OS == 1)
    osMessagePut ( phost->os_event, USBH_URB_EVENT, 0);
#endif       
    }     
    else if(URB_Status == USBH_URB_STALL)
    {
      MSC_Handle->hbot.state  = BOT_ERROR_OUT;
#if (USBH_USE_OS == 1)
    osMessagePut ( phost->os_event, USBH_URB_EVENT, 0);
#endif       
    }
    break;
    
  case BOT_DATA_IN:   
    /* Send first packet */        
    USBH_BulkReceiveData (phost,
                          MSC_Handle->hbot.pbuf, 
                          MSC_Handle->InEpSize , 
                          MSC_Handle->InPipe);
    
    MSC_Handle->hbot.state  = BOT_DATA_IN_WAIT;
    
    break;   
    
  case BOT_DATA_IN_WAIT:  
    
    URB_Status = USBH_LL_GetURBState(phost, MSC_Handle->InPipe); 
    
    if(URB_Status == USBH_URB_DONE) 
    {
      /* Adjudt Data pointer and data length */
      if(MSC_Handle->hbot.cbw.field.DataTransferLength > MSC_Handle->InEpSize)
      {
          MSC_Handle->hbot.pbuf += MSC_Handle->InEpSize;
          MSC_Handle->hbot.cbw.field.DataTransferLength -= MSC_Handle->InEpSize;  
      }
      else
      {
        MSC_Handle->hbot.cbw.field.DataTransferLength = 0;
      }
        
      /* More Data To be Received */
      if(MSC_Handle->hbot.cbw.field.DataTransferLength > 0)
      {
        /* Send next packet */        
        USBH_BulkReceiveData (phost,
                              MSC_Handle->hbot.pbuf, 
                              MSC_Handle->InEpSize , 
                              MSC_Handle->InPipe);
        
      }
      else
      {
        /* If value was 0, and successful transfer, then change the state */
        MSC_Handle->hbot.state  = BOT_RECEIVE_CSW;
#if (USBH_USE_OS == 1)
        osMessagePut ( phost->os_event, USBH_URB_EVENT, 0);
#endif 
      }
    }
    else if(URB_Status == USBH_URB_STALL)
    {
      /* This is Data IN Stage STALL Condition */
      MSC_Handle->hbot.state  = BOT_ERROR_IN;
      
      /* Refer to USB Mass-Storage Class : BOT (www.usb.org) 
      6.7.2 Host expects to receive data from the device
      3. On a STALL condition receiving data, then:
      The host shall accept the data received.
      The host shall clear the Bulk-In pipe.
      4. The host shall attempt to receive a CSW.*/
      
#if (USBH_USE_OS == 1)
    osMessagePut ( phost->os_event, USBH_URB_EVENT, 0);
#endif       
    }     
    break;  
    
  case BOT_DATA_OUT:
    
    USBH_BulkSendData (phost,
                       MSC_Handle->hbot.pbuf, 
                       MSC_Handle->OutEpSize , 
                       MSC_Handle->OutPipe,
                       1);
    
    
    MSC_Handle->hbot.state  = BOT_DATA_OUT_WAIT;
    break;
    
  case BOT_DATA_OUT_WAIT:
    URB_Status = USBH_LL_GetURBState(phost, MSC_Handle->OutPipe);     
    
    if(URB_Status == USBH_URB_DONE)
    {
      /* Adjudt Data pointer and data length */
      if(MSC_Handle->hbot.cbw.field.DataTransferLength > MSC_Handle->OutEpSize)
      {
          MSC_Handle->hbot.pbuf += MSC_Handle->OutEpSize;
          MSC_Handle->hbot.cbw.field.DataTransferLength -= MSC_Handle->OutEpSize; 
      }
      else
      {
        MSC_Handle->hbot.cbw.field.DataTransferLength = 0;
      } 
      
      /* More Data To be Sent */
      if(MSC_Handle->hbot.cbw.field.DataTransferLength > 0)
      {
        USBH_BulkSendData (phost,
                           MSC_Handle->hbot.pbuf, 
                           MSC_Handle->OutEpSize , 
                           MSC_Handle->OutPipe,
                           1);
      }
      else
      {
        /* If value was 0, and successful transfer, then change the state */
        MSC_Handle->hbot.state  = BOT_RECEIVE_CSW;
      }  
#if (USBH_USE_OS == 1)
    osMessagePut ( phost->os_event, USBH_URB_EVENT, 0);
#endif       
    }
    
    else if(URB_Status == USBH_URB_NOTREADY)
    {
      /* Re-send same data */      
      MSC_Handle->hbot.state  = BOT_DATA_OUT;
#if (USBH_USE_OS == 1)
    osMessagePut ( phost->os_event, USBH_URB_EVENT, 0);
#endif       
    }
    
    else if(URB_Status == USBH_URB_STALL)
    {
      MSC_Handle->hbot.state  = BOT_ERROR_OUT;
      
      /* Refer to USB Mass-Storage Class : BOT (www.usb.org) 
      6.7.3 Ho - Host expects to send data to the device
      3. On a STALL condition sending data, then:
      " The host shall clear the Bulk-Out pipe.
      4. The host shall attempt to receive a CSW.
      */      
#if (USBH_USE_OS == 1)
      osMessagePut ( phost->os_event, USBH_URB_EVENT, 0);
#endif       
    }
    break;
    
  case BOT_RECEIVE_CSW:
    
    USBH_BulkReceiveData (phost,
                          MSC_Handle->hbot.csw.data, 
                          BOT_CSW_LENGTH , 
                          MSC_Handle->InPipe);
    
    MSC_Handle->hbot.state  = BOT_RECEIVE_CSW_WAIT;
    break;
    
  case BOT_RECEIVE_CSW_WAIT:
    
    URB_Status = USBH_LL_GetURBState(phost, MSC_Handle->InPipe); 
    
    /* Decode CSW */
    if(URB_Status == USBH_URB_DONE)
    {
      MSC_Handle->hbot.state = BOT_SEND_CBW;    
      MSC_Handle->hbot.cmd_state = BOT_CMD_SEND;        
      CSW_Status = USBH_MSC_DecodeCSW(phost);
      
      if(CSW_Status == BOT_CSW_CMD_PASSED)
      {
        status = USBH_OK;
      }
      else
      {
        status = USBH_FAIL;
      }
#if (USBH_USE_OS == 1)
      osMessagePut ( phost->os_event, USBH_URB_EVENT, 0);
#endif       
    }
    else if(URB_Status == USBH_URB_STALL)     
    {
      MSC_Handle->hbot.state  = BOT_ERROR_IN;
#if (USBH_USE_OS == 1)
      osMessagePut ( phost->os_event, USBH_URB_EVENT, 0);
#endif       
    }
    break;
    
  case BOT_ERROR_IN: 
    error = USBH_MSC_BOT_Abort(phost, lun, BOT_DIR_IN);
    
    if (error == USBH_OK)
    {
      MSC_Handle->hbot.state = BOT_RECEIVE_CSW;
    }
    else if (error == USBH_UNRECOVERED_ERROR)
    {
      /* This means that there is a STALL Error limit, Do Reset Recovery */
      MSC_Handle->hbot.state = BOT_UNRECOVERED_ERROR;
    }
    break;
    
  case BOT_ERROR_OUT: 
    error = USBH_MSC_BOT_Abort(phost, lun, BOT_DIR_OUT);
    
    if ( error == USBH_OK)
    { 
      
      toggle = USBH_LL_GetToggle(phost, MSC_Handle->OutPipe); 
      USBH_LL_SetToggle(phost, MSC_Handle->OutPipe, 1- toggle);   
      USBH_LL_SetToggle(phost, MSC_Handle->InPipe, 0);  
      MSC_Handle->hbot.state = BOT_ERROR_IN;        
    }
    else if (error == USBH_UNRECOVERED_ERROR)
    {
      MSC_Handle->hbot.state = BOT_UNRECOVERED_ERROR;
    }
    break;
    
    
  case BOT_UNRECOVERED_ERROR: 
    status = USBH_MSC_BOT_REQ_Reset(phost);
    if ( status == USBH_OK)
    {
      MSC_Handle->hbot.state = BOT_SEND_CBW; 
    }
    break;
    
  default:      
    break;
  }
  return status;
}

/**
  * @brief  USBH_MSC_BOT_Abort 
  *         The function handle the BOT Abort process.
  * @param  phost: Host handle
  * @param  lun: Logical Unit Number
  * @param  dir: direction (0: out / 1 : in)
  * @retval USBH Status
  */
static USBH_StatusTypeDef USBH_MSC_BOT_Abort(USBH_HandleTypeDef *phost, uint8_t lun, uint8_t dir)
{
  USBH_StatusTypeDef status = USBH_FAIL;
  MSC_HandleTypeDef *MSC_Handle =  phost->pActiveClass->pData;
  
  switch (dir)
  {
  case BOT_DIR_IN :
    /* send ClrFeture on Bulk IN endpoint */
    status = USBH_ClrFeature(phost, MSC_Handle->InEp);
    
    break;
    
  case BOT_DIR_OUT :
    /*send ClrFeature on Bulk OUT endpoint */
    status = USBH_ClrFeature(phost, MSC_Handle->OutEp);
    break;
    
  default:
    break;
  }
  return status;
}

/**
  * @brief  USBH_MSC_BOT_DecodeCSW
  *         This function decodes the CSW received by the device and updates the
  *         same to upper layer.
  * @param  phost: Host handle
  * @retval USBH Status
  * @notes
  *     Refer to USB Mass-Storage Class : BOT (www.usb.org)
  *    6.3.1 Valid CSW Conditions :
  *     The host shall consider the CSW valid when:
  *     1. dCSWSignature is equal to 53425355h
  *     2. the CSW is 13 (Dh) bytes in length,
  *     3. dCSWTag matches the dCBWTag from the corresponding CBW.
  */

static BOT_CSWStatusTypeDef USBH_MSC_DecodeCSW(USBH_HandleTypeDef *phost)
{
  MSC_HandleTypeDef *MSC_Handle =  phost->pActiveClass->pData;
  BOT_CSWStatusTypeDef status = BOT_CSW_CMD_FAILED;
  
    /*Checking if the transfer length is diffrent than 13*/    
    if(USBH_LL_GetLastXferSize(phost, MSC_Handle->InPipe) != BOT_CSW_LENGTH)
    {
      /*(4) Hi > Dn (Host expects to receive data from the device,
      Device intends to transfer no data)
      (5) Hi > Di (Host expects to receive data from the device,
      Device intends to send data to the host)
      (9) Ho > Dn (Host expects to send data to the device,
      Device intends to transfer no data)
      (11) Ho > Do  (Host expects to send data to the device,
      Device intends to receive data from the host)*/
      
      
      status = BOT_CSW_PHASE_ERROR;
    }
    else
    { /* CSW length is Correct */
      
      /* Check validity of the CSW Signature and CSWStatus */
      if(MSC_Handle->hbot.csw.field.Signature == BOT_CSW_SIGNATURE)
      {/* Check Condition 1. dCSWSignature is equal to 53425355h */
        
        if(MSC_Handle->hbot.csw.field.Tag == MSC_Handle->hbot.cbw.field.Tag)
        {
          /* Check Condition 3. dCSWTag matches the dCBWTag from the 
          corresponding CBW */

          if(MSC_Handle->hbot.csw.field.Status == 0) 
          {
            /* Refer to USB Mass-Storage Class : BOT (www.usb.org) 
            
            Hn Host expects no data transfers
            Hi Host expects to receive data from the device
            Ho Host expects to send data to the device
            
            Dn Device intends to transfer no data
            Di Device intends to send data to the host
            Do Device intends to receive data from the host
            
            Section 6.7 
            (1) Hn = Dn (Host expects no data transfers,
            Device intends to transfer no data)
            (6) Hi = Di (Host expects to receive data from the device,
            Device intends to send data to the host)
            (12) Ho = Do (Host expects to send data to the device, 
            Device intends to receive data from the host)
            
            */
            
            status = BOT_CSW_CMD_PASSED;
          }
          else if(MSC_Handle->hbot.csw.field.Status == 1)
          {
            status = BOT_CSW_CMD_FAILED;
          }
          
          else if(MSC_Handle->hbot.csw.field.Status == 2)
          { 
            /* Refer to USB Mass-Storage Class : BOT (www.usb.org) 
            Section 6.7 
            (2) Hn < Di ( Host expects no data transfers, 
            Device intends to send data to the host)
            (3) Hn < Do ( Host expects no data transfers, 
            Device intends to receive data from the host)
            (7) Hi < Di ( Host expects to receive data from the device, 
            Device intends to send data to the host)
            (8) Hi <> Do ( Host expects to receive data from the device, 
            Device intends to receive data from the host)
            (10) Ho <> Di (Host expects to send data to the device,
            Di Device intends to send data to the host)
            (13) Ho < Do (Host expects to send data to the device, 
            Device intends to receive data from the host)
            */
            
            status = BOT_CSW_PHASE_ERROR;
          }
        } /* CSW Tag Matching is Checked  */
      } /* CSW Signature Correct Checking */
      else
      {
        /* If the CSW Signature is not valid, We sall return the Phase Error to
        Upper Layers for Reset Recovery */
        
        status = BOT_CSW_PHASE_ERROR;
      }
    } /* CSW Length Check*/
    
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



