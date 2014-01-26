/** 
  ******************************************************************************
  * @file    usbh_ioreq.c 
  * @author  MCD Application Team
  * @version V2.1.0
  * @date    19-March-2012
  * @brief   This file handles the issuing of the USB transactions
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
/* Includes ------------------------------------------------------------------*/

#include "usbh_ioreq.h"

/** @addtogroup USBH_LIB
  * @{
  */

/** @addtogroup USBH_LIB_CORE
* @{
*/
  
/** @defgroup USBH_IOREQ 
  * @brief This file handles the standard protocol processing (USB v2.0)
  * @{
  */


/** @defgroup USBH_IOREQ_Private_Defines
  * @{
  */ 
/**
  * @}
  */ 
 

/** @defgroup USBH_IOREQ_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 



/** @defgroup USBH_IOREQ_Private_Macros
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBH_IOREQ_Private_Variables
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBH_IOREQ_Private_FunctionPrototypes
  * @{
  */ 
static USBH_Status USBH_SubmitSetupRequest(USBH_HOST *phost,
                                           uint8_t* buff, 
                                           uint16_t length);

/**
  * @}
  */ 


/** @defgroup USBH_IOREQ_Private_Functions
  * @{
  */ 


/**
  * @brief  USBH_CtlReq
  *         USBH_CtlReq sends a control request and provide the status after 
  *            completion of the request
  * @param  pdev: Selected device
  * @param  req: Setup Request Structure
  * @param  buff: data buffer address to store the response
  * @param  length: length of the response
  * @retval Status
  */
USBH_Status USBH_CtlReq     (USB_OTG_CORE_HANDLE *pdev, 
                             USBH_HOST           *phost, 
                             uint8_t             *buff,
                             uint16_t            length)
{
  USBH_Status status;
  status = USBH_BUSY;
  
  switch (phost->RequestState)
  {
  case CMD_SEND:
    /* Start a SETUP transfer */
    USBH_SubmitSetupRequest(phost, buff, length);
    phost->RequestState = CMD_WAIT;
    status = USBH_BUSY;
    break;
    
  case CMD_WAIT:
     if (phost->Control.state == CTRL_COMPLETE ) 
    {
      /* Commands successfully sent and Response Received  */       
      phost->RequestState = CMD_SEND;
      phost->Control.state =CTRL_IDLE;  
      status = USBH_OK;      
    }
    else if  (phost->Control.state == CTRL_ERROR)
    {
      /* Failure Mode */
      phost->RequestState = CMD_SEND;
      status = USBH_FAIL;
    }   
     else if  (phost->Control.state == CTRL_STALLED )
    {
      /* Commands successfully sent and Response Received  */       
      phost->RequestState = CMD_SEND;
      status = USBH_NOT_SUPPORTED;
    }
    break;
    
  default:
    break; 
  }
  return status;
}

/**
  * @brief  USBH_CtlSendSetup
  *         Sends the Setup Packet to the Device
  * @param  pdev: Selected device
  * @param  buff: Buffer pointer from which the Data will be send to Device
  * @param  hc_num: Host channel Number
  * @retval Status
  */
USBH_Status USBH_CtlSendSetup ( USB_OTG_CORE_HANDLE *pdev, 
                                uint8_t *buff, 
                                uint8_t hc_num){
  pdev->host.hc[hc_num].ep_is_in = 0;
  pdev->host.hc[hc_num].data_pid = HC_PID_SETUP;   
  pdev->host.hc[hc_num].xfer_buff = buff;
  pdev->host.hc[hc_num].xfer_len = USBH_SETUP_PKT_SIZE;   

  return (USBH_Status)HCD_SubmitRequest (pdev , hc_num);   
}


/**
  * @brief  USBH_CtlSendData
  *         Sends a data Packet to the Device
  * @param  pdev: Selected device
  * @param  buff: Buffer pointer from which the Data will be sent to Device
  * @param  length: Length of the data to be sent
  * @param  hc_num: Host channel Number
  * @retval Status
  */
USBH_Status USBH_CtlSendData ( USB_OTG_CORE_HANDLE *pdev, 
                                uint8_t *buff, 
                                uint16_t length,
                                uint8_t hc_num)
{
  pdev->host.hc[hc_num].ep_is_in = 0;
  pdev->host.hc[hc_num].xfer_buff = buff;
  pdev->host.hc[hc_num].xfer_len = length;
 
  if ( length == 0 )
  { /* For Status OUT stage, Length==0, Status Out PID = 1 */
    pdev->host.hc[hc_num].toggle_out = 1;   
  }
 
 /* Set the Data Toggle bit as per the Flag */
  if ( pdev->host.hc[hc_num].toggle_out == 0)
  { /* Put the PID 0 */
      pdev->host.hc[hc_num].data_pid = HC_PID_DATA0;    
  }
 else
 { /* Put the PID 1 */
      pdev->host.hc[hc_num].data_pid = HC_PID_DATA1 ;
 }

  HCD_SubmitRequest (pdev , hc_num);   
   
  return USBH_OK;
}


/**
  * @brief  USBH_CtlReceiveData
  *         Receives the Device Response to the Setup Packet
  * @param  pdev: Selected device
  * @param  buff: Buffer pointer in which the response needs to be copied
  * @param  length: Length of the data to be received
  * @param  hc_num: Host channel Number
  * @retval Status. 
  */
USBH_Status USBH_CtlReceiveData(USB_OTG_CORE_HANDLE *pdev, 
                                uint8_t* buff, 
                                uint16_t length,
                                uint8_t hc_num)
{

  pdev->host.hc[hc_num].ep_is_in = 1;
  pdev->host.hc[hc_num].data_pid = HC_PID_DATA1;
  pdev->host.hc[hc_num].xfer_buff = buff;
  pdev->host.hc[hc_num].xfer_len = length;  

  HCD_SubmitRequest (pdev , hc_num);   
  
  return USBH_OK;
  
}


/**
  * @brief  USBH_BulkSendData
  *         Sends the Bulk Packet to the device
  * @param  pdev: Selected device
  * @param  buff: Buffer pointer from which the Data will be sent to Device
  * @param  length: Length of the data to be sent
  * @param  hc_num: Host channel Number
  * @retval Status
  */
USBH_Status USBH_BulkSendData ( USB_OTG_CORE_HANDLE *pdev, 
                                uint8_t *buff, 
                                uint16_t length,
                                uint8_t hc_num)
{ 
  pdev->host.hc[hc_num].ep_is_in = 0;
  pdev->host.hc[hc_num].xfer_buff = buff;
  pdev->host.hc[hc_num].xfer_len = length;  

 /* Set the Data Toggle bit as per the Flag */
  if ( pdev->host.hc[hc_num].toggle_out == 0)
  { /* Put the PID 0 */
      pdev->host.hc[hc_num].data_pid = HC_PID_DATA0;    
  }
 else
 { /* Put the PID 1 */
      pdev->host.hc[hc_num].data_pid = HC_PID_DATA1 ;
 }

  HCD_SubmitRequest (pdev , hc_num);   
  return USBH_OK;
}


/**
  * @brief  USBH_BulkReceiveData
  *         Receives IN bulk packet from device
  * @param  pdev: Selected device
  * @param  buff: Buffer pointer in which the received data packet to be copied
  * @param  length: Length of the data to be received
  * @param  hc_num: Host channel Number
  * @retval Status. 
  */
USBH_Status USBH_BulkReceiveData( USB_OTG_CORE_HANDLE *pdev, 
                                uint8_t *buff, 
                                uint16_t length,
                                uint8_t hc_num)
{
  pdev->host.hc[hc_num].ep_is_in = 1;   
  pdev->host.hc[hc_num].xfer_buff = buff;
  pdev->host.hc[hc_num].xfer_len = length;
  

  if( pdev->host.hc[hc_num].toggle_in == 0)
  {
    pdev->host.hc[hc_num].data_pid = HC_PID_DATA0;
  }
  else
  {
    pdev->host.hc[hc_num].data_pid = HC_PID_DATA1;
  }

  HCD_SubmitRequest (pdev , hc_num);  
  return USBH_OK;
}


/**
  * @brief  USBH_InterruptReceiveData
  *         Receives the Device Response to the Interrupt IN token
  * @param  pdev: Selected device
  * @param  buff: Buffer pointer in which the response needs to be copied
  * @param  length: Length of the data to be received
  * @param  hc_num: Host channel Number
  * @retval Status. 
  */
USBH_Status USBH_InterruptReceiveData( USB_OTG_CORE_HANDLE *pdev, 
                                uint8_t *buff, 
                                uint8_t length,
                                uint8_t hc_num)
{

  pdev->host.hc[hc_num].ep_is_in = 1;  
  pdev->host.hc[hc_num].xfer_buff = buff;
  pdev->host.hc[hc_num].xfer_len = length;
  

  
  if(pdev->host.hc[hc_num].toggle_in == 0)
  {
    pdev->host.hc[hc_num].data_pid = HC_PID_DATA0;
  }
  else
  {
    pdev->host.hc[hc_num].data_pid = HC_PID_DATA1;
  }

  /* toggle DATA PID */
  pdev->host.hc[hc_num].toggle_in ^= 1;  
  
  HCD_SubmitRequest (pdev , hc_num);  
  
  return USBH_OK;
}

/**
  * @brief  USBH_InterruptSendData
  *         Sends the data on Interrupt OUT Endpoint
  * @param  pdev: Selected device
  * @param  buff: Buffer pointer from where the data needs to be copied
  * @param  length: Length of the data to be sent
  * @param  hc_num: Host channel Number
  * @retval Status. 
  */
USBH_Status USBH_InterruptSendData( USB_OTG_CORE_HANDLE *pdev, 
                                uint8_t *buff, 
                                uint8_t length,
                                uint8_t hc_num)
{

  pdev->host.hc[hc_num].ep_is_in = 0;  
  pdev->host.hc[hc_num].xfer_buff = buff;
  pdev->host.hc[hc_num].xfer_len = length;
  
  if(pdev->host.hc[hc_num].toggle_in == 0)
  {
    pdev->host.hc[hc_num].data_pid = HC_PID_DATA0;
  }
  else
  {
    pdev->host.hc[hc_num].data_pid = HC_PID_DATA1;
  }

  pdev->host.hc[hc_num].toggle_in ^= 1;  
  
  HCD_SubmitRequest (pdev , hc_num);  
  
  return USBH_OK;
}


/**
  * @brief  USBH_SubmitSetupRequest
  *         Start a setup transfer by changing the state-machine and 
  *         initializing  the required variables needed for the Control Transfer
  * @param  pdev: Selected device
  * @param  setup: Setup Request Structure
  * @param  buff: Buffer used for setup request
  * @param  length: Length of the data
  * @retval Status. 
*/
static USBH_Status USBH_SubmitSetupRequest(USBH_HOST *phost,
                                           uint8_t* buff, 
                                           uint16_t length)
{
  
  /* Save Global State */
  phost->gStateBkp =   phost->gState; 
  
  /* Prepare the Transactions */
  phost->gState = HOST_CTRL_XFER;
  phost->Control.buff = buff; 
  phost->Control.length = length;
  phost->Control.state = CTRL_SETUP;  

  return USBH_OK;  
}


/**
  * @brief  USBH_IsocReceiveData
  *         Receives the Device Response to the Isochronous IN token
  * @param  pdev: Selected device
  * @param  buff: Buffer pointer in which the response needs to be copied
  * @param  length: Length of the data to be received
  * @param  hc_num: Host channel Number
  * @retval Status. 
  */
USBH_Status USBH_IsocReceiveData( USB_OTG_CORE_HANDLE *pdev, 
                                uint8_t *buff, 
                                uint32_t length,
                                uint8_t hc_num)
{    
  
  pdev->host.hc[hc_num].ep_is_in = 1;  
  pdev->host.hc[hc_num].xfer_buff = buff;
  pdev->host.hc[hc_num].xfer_len = length;
  pdev->host.hc[hc_num].data_pid = HC_PID_DATA0;
  

  HCD_SubmitRequest (pdev , hc_num);  
  
  return USBH_OK;
}

/**
  * @brief  USBH_IsocSendData
  *         Sends the data on Isochronous OUT Endpoint
  * @param  pdev: Selected device
  * @param  buff: Buffer pointer from where the data needs to be copied
  * @param  length: Length of the data to be sent
  * @param  hc_num: Host channel Number
  * @retval Status. 
  */
USBH_Status USBH_IsocSendData( USB_OTG_CORE_HANDLE *pdev, 
                                uint8_t *buff, 
                                uint32_t length,
                                uint8_t hc_num)
{
  
  pdev->host.hc[hc_num].ep_is_in = 0;  
  pdev->host.hc[hc_num].xfer_buff = buff;
  pdev->host.hc[hc_num].xfer_len = length;
  pdev->host.hc[hc_num].data_pid = HC_PID_DATA0;
  
  HCD_SubmitRequest (pdev , hc_num);  
  
  return USBH_OK;
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/



