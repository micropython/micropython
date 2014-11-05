/**
  ******************************************************************************
  * @file    usbd_msc_bot.c
  * @author  MCD Application Team
  * @version V2.0.0
  * @date    18-February-2014
  * @brief   This file provides all the BOT protocol core functions.
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
#include "usbd_msc_bot.h"
#include "usbd_msc.h"
#include "usbd_msc_scsi.h"
#include "usbd_ioreq.h"

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @{
  */


/** @defgroup MSC_BOT 
  * @brief BOT protocol module
  * @{
  */ 

/** @defgroup MSC_BOT_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup MSC_BOT_Private_Defines
  * @{
  */ 

/**
  * @}
  */ 


/** @defgroup MSC_BOT_Private_Macros
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup MSC_BOT_Private_Variables
  * @{
  */ 

/**
  * @}
  */ 


/** @defgroup MSC_BOT_Private_FunctionPrototypes
  * @{
  */ 
static void MSC_BOT_CBW_Decode (USBD_HandleTypeDef  *pdev);

static void MSC_BOT_SendData (USBD_HandleTypeDef  *pdev, 
                              uint8_t* pbuf, 
                              uint16_t len);

static void MSC_BOT_Abort(USBD_HandleTypeDef  *pdev);
/**
  * @}
  */ 


/** @defgroup MSC_BOT_Private_Functions
  * @{
  */ 



/**
* @brief  MSC_BOT_Init
*         Initialize the BOT Process
* @param  pdev: device instance
* @retval None
*/
void MSC_BOT_Init (USBD_HandleTypeDef  *pdev)
{
  USBD_MSC_BOT_HandleTypeDef  *hmsc = pdev->pClassData;
    
  hmsc->bot_state  = USBD_BOT_IDLE;
  hmsc->bot_status = USBD_BOT_STATUS_NORMAL;
  
  hmsc->scsi_sense_tail = 0;
  hmsc->scsi_sense_head = 0;
  
  ((USBD_StorageTypeDef *)pdev->pUserData)->Init(0);
  
  USBD_LL_FlushEP(pdev, MSC_EPOUT_ADDR);
  USBD_LL_FlushEP(pdev, MSC_EPIN_ADDR);
  
  /* Prapare EP to Receive First BOT Cmd */
  USBD_LL_PrepareReceive (pdev,
                          MSC_EPOUT_ADDR,
                          (uint8_t *)&hmsc->cbw,
                          USBD_BOT_CBW_LENGTH);    
}

/**
* @brief  MSC_BOT_Reset
*         Reset the BOT Machine
* @param  pdev: device instance
* @retval  None
*/
void MSC_BOT_Reset (USBD_HandleTypeDef  *pdev)
{
  USBD_MSC_BOT_HandleTypeDef  *hmsc = pdev->pClassData;
    
  hmsc->bot_state  = USBD_BOT_IDLE;
  hmsc->bot_status = USBD_BOT_STATUS_RECOVERY;  
  
  /* Prapare EP to Receive First BOT Cmd */
  USBD_LL_PrepareReceive (pdev,
                          MSC_EPOUT_ADDR,
                          (uint8_t *)&hmsc->cbw,
                          USBD_BOT_CBW_LENGTH);   
}

/**
* @brief  MSC_BOT_DeInit
*         Uninitialize the BOT Machine
* @param  pdev: device instance
* @retval None
*/
void MSC_BOT_DeInit (USBD_HandleTypeDef  *pdev)
{
  USBD_MSC_BOT_HandleTypeDef  *hmsc = pdev->pClassData;  
  hmsc->bot_state  = USBD_BOT_IDLE;
}

/**
* @brief  MSC_BOT_DataIn
*         Handle BOT IN data stage
* @param  pdev: device instance
* @param  epnum: endpoint index
* @retval None
*/
void MSC_BOT_DataIn (USBD_HandleTypeDef  *pdev, 
                     uint8_t epnum)
{
  USBD_MSC_BOT_HandleTypeDef  *hmsc = pdev->pClassData;  
  
  switch (hmsc->bot_state)
  {
  case USBD_BOT_DATA_IN:
    if(SCSI_ProcessCmd(pdev,
                        hmsc->cbw.bLUN,
                        &hmsc->cbw.CB[0]) < 0)
    {
      MSC_BOT_SendCSW (pdev, USBD_CSW_CMD_FAILED);
    }
    break;
    
  case USBD_BOT_SEND_DATA:
  case USBD_BOT_LAST_DATA_IN:
    MSC_BOT_SendCSW (pdev, USBD_CSW_CMD_PASSED);
    
    break;
    
  default:
    break;
  }
}
/**
* @brief  MSC_BOT_DataOut
*         Proccess MSC OUT data
* @param  pdev: device instance
* @param  epnum: endpoint index
* @retval None
*/
void MSC_BOT_DataOut (USBD_HandleTypeDef  *pdev, 
                      uint8_t epnum)
{
  USBD_MSC_BOT_HandleTypeDef  *hmsc = pdev->pClassData;
  
  switch (hmsc->bot_state)
  {
  case USBD_BOT_IDLE:
    MSC_BOT_CBW_Decode(pdev);
    break;
    
  case USBD_BOT_DATA_OUT:
    
    if(SCSI_ProcessCmd(pdev,
                        hmsc->cbw.bLUN,
                        &hmsc->cbw.CB[0]) < 0)
    {
      MSC_BOT_SendCSW (pdev, USBD_CSW_CMD_FAILED);
    }

    break;
    
  default:
    break;
  }
}

/**
* @brief  MSC_BOT_CBW_Decode
*         Decode the CBW command and set the BOT state machine accordingtly  
* @param  pdev: device instance
* @retval None
*/
static void  MSC_BOT_CBW_Decode (USBD_HandleTypeDef  *pdev)
{
  USBD_MSC_BOT_HandleTypeDef  *hmsc = pdev->pClassData;  
  
  hmsc->csw.dTag = hmsc->cbw.dTag;
  hmsc->csw.dDataResidue = hmsc->cbw.dDataLength;
  
  if ((USBD_LL_GetRxDataSize (pdev ,MSC_EPOUT_ADDR) != USBD_BOT_CBW_LENGTH) ||
      (hmsc->cbw.dSignature != USBD_BOT_CBW_SIGNATURE)||
        (hmsc->cbw.bLUN > 1) || 
          (hmsc->cbw.bCBLength < 1) || 
            (hmsc->cbw.bCBLength > 16))
  {
    
    SCSI_SenseCode(pdev,
                   hmsc->cbw.bLUN, 
                   ILLEGAL_REQUEST, 
                   INVALID_CDB);
    
    hmsc->bot_status = USBD_BOT_STATUS_ERROR;   
    MSC_BOT_Abort(pdev);
 
  }
  else
  {
    if(SCSI_ProcessCmd(pdev,
                       hmsc->cbw.bLUN,
                       &hmsc->cbw.CB[0]) < 0)
    {
      if(hmsc->bot_state == USBD_BOT_NO_DATA)
      {
       MSC_BOT_SendCSW (pdev,
                         USBD_CSW_CMD_FAILED); 
      }
      else
      {
        MSC_BOT_Abort(pdev);
      }
    }
    /*Burst xfer handled internally*/
    else if ((hmsc->bot_state != USBD_BOT_DATA_IN) && 
             (hmsc->bot_state != USBD_BOT_DATA_OUT) &&
             (hmsc->bot_state != USBD_BOT_LAST_DATA_IN)) 
    {
      if (hmsc->bot_data_length > 0)
      {
        MSC_BOT_SendData(pdev,
                         hmsc->bot_data, 
                         hmsc->bot_data_length);
      }
      else if (hmsc->bot_data_length == 0) 
      {
        MSC_BOT_SendCSW (pdev,
                         USBD_CSW_CMD_PASSED);
      }
    }
  }
}

/**
* @brief  MSC_BOT_SendData
*         Send the requested data
* @param  pdev: device instance
* @param  buf: pointer to data buffer
* @param  len: Data Length
* @retval None
*/
static void  MSC_BOT_SendData(USBD_HandleTypeDef  *pdev,
                              uint8_t* buf, 
                              uint16_t len)
{
  USBD_MSC_BOT_HandleTypeDef  *hmsc = pdev->pClassData; 
  
  len = MIN (hmsc->cbw.dDataLength, len);
  hmsc->csw.dDataResidue -= len;
  hmsc->csw.bStatus = USBD_CSW_CMD_PASSED;
  hmsc->bot_state = USBD_BOT_SEND_DATA;
  
  USBD_LL_Transmit (pdev, MSC_EPIN_ADDR, buf, len);  
}

/**
* @brief  MSC_BOT_SendCSW
*         Send the Command Status Wrapper
* @param  pdev: device instance
* @param  status : CSW status
* @retval None
*/
void  MSC_BOT_SendCSW (USBD_HandleTypeDef  *pdev,
                              uint8_t CSW_Status)
{
  USBD_MSC_BOT_HandleTypeDef  *hmsc = pdev->pClassData; 
  
  hmsc->csw.dSignature = USBD_BOT_CSW_SIGNATURE;
  hmsc->csw.bStatus = CSW_Status;
  hmsc->bot_state = USBD_BOT_IDLE;
  
  USBD_LL_Transmit (pdev, 
             MSC_EPIN_ADDR, 
             (uint8_t *)&hmsc->csw, 
             USBD_BOT_CSW_LENGTH);
  
  /* Prapare EP to Receive next Cmd */
  USBD_LL_PrepareReceive (pdev,
                    MSC_EPOUT_ADDR,
                    (uint8_t *)&hmsc->cbw, 
                    USBD_BOT_CBW_LENGTH);  
  
}

/**
* @brief  MSC_BOT_Abort
*         Abort the current transfer
* @param  pdev: device instance
* @retval status
*/

static void  MSC_BOT_Abort (USBD_HandleTypeDef  *pdev)
{
  USBD_MSC_BOT_HandleTypeDef  *hmsc = pdev->pClassData; 
  
  if ((hmsc->cbw.bmFlags == 0) && 
      (hmsc->cbw.dDataLength != 0) &&
      (hmsc->bot_status == USBD_BOT_STATUS_NORMAL) )
  {
    USBD_LL_StallEP(pdev, MSC_EPOUT_ADDR );
  }
  USBD_LL_StallEP(pdev, MSC_EPIN_ADDR);
  
  if(hmsc->bot_status == USBD_BOT_STATUS_ERROR)
  {
    USBD_LL_PrepareReceive (pdev,
                      MSC_EPOUT_ADDR,
                      (uint8_t *)&hmsc->cbw, 
                      USBD_BOT_CBW_LENGTH);    
  }
}

/**
* @brief  MSC_BOT_CplClrFeature
*         Complete the clear feature request
* @param  pdev: device instance
* @param  epnum: endpoint index
* @retval None
*/

void  MSC_BOT_CplClrFeature (USBD_HandleTypeDef  *pdev, uint8_t epnum)
{
  USBD_MSC_BOT_HandleTypeDef  *hmsc = pdev->pClassData; 
  
  if(hmsc->bot_status == USBD_BOT_STATUS_ERROR )/* Bad CBW Signature */
  {
    USBD_LL_StallEP(pdev, MSC_EPIN_ADDR);
    hmsc->bot_status = USBD_BOT_STATUS_NORMAL;    
  }
  else if(((epnum & 0x80) == 0x80) && ( hmsc->bot_status != USBD_BOT_STATUS_RECOVERY))
  {
    MSC_BOT_SendCSW (pdev, USBD_CSW_CMD_FAILED);
  }
  
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
