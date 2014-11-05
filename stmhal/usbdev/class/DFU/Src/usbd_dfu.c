/**
  ******************************************************************************
  * @file    usbd_dfu.c
  * @author  MCD Application Team
  * @version V2.0.0
  * @date    18-February-2014
  * @brief   This file provides the HID core functions.
  *
  * @verbatim
  *      
  *          ===================================================================      
  *                                DFU Class  Description
  *          ===================================================================
  *          
  *
  *
  *
  *           
  *      
  * @note     In HS mode and when the DMA is used, all variables and data structures
  *           dealing with the DMA during the transaction process should be 32-bit aligned.
  *           
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
#include "usbd_dfu.h"
#include "usbd_desc.h"
#include "usbd_ctlreq.h"


/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @{
  */


/** @defgroup USBD_DFU 
  * @brief usbd core module
  * @{
  */ 

/** @defgroup USBD_DFU_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBD_DFU_Private_Defines
  * @{
  */ 

/**
  * @}
  */ 


/** @defgroup USBD_DFU_Private_Macros
  * @{
  */ 
#define DFU_SAMPLE_FREQ(frq)      (uint8_t)(frq), (uint8_t)((frq >> 8)), (uint8_t)((frq >> 16))

#define DFU_PACKET_SZE(frq)          (uint8_t)(((frq * 2 * 2)/1000) & 0xFF), \
                                       (uint8_t)((((frq * 2 * 2)/1000) >> 8) & 0xFF)
                                         
/**
  * @}
  */ 




/** @defgroup USBD_DFU_Private_FunctionPrototypes
  * @{
  */


static uint8_t  USBD_DFU_Init (USBD_HandleTypeDef *pdev, 
                               uint8_t cfgidx);

static uint8_t  USBD_DFU_DeInit (USBD_HandleTypeDef *pdev, 
                                 uint8_t cfgidx);

static uint8_t  USBD_DFU_Setup (USBD_HandleTypeDef *pdev, 
                                USBD_SetupReqTypedef *req);

static uint8_t  *USBD_DFU_GetCfgDesc (uint16_t *length);

static uint8_t  *USBD_DFU_GetDeviceQualifierDesc (uint16_t *length);

static uint8_t  USBD_DFU_DataIn (USBD_HandleTypeDef *pdev, uint8_t epnum);

static uint8_t  USBD_DFU_DataOut (USBD_HandleTypeDef *pdev, uint8_t epnum);

static uint8_t  USBD_DFU_EP0_RxReady (USBD_HandleTypeDef *pdev);

static uint8_t  USBD_DFU_EP0_TxReady (USBD_HandleTypeDef *pdev);

static uint8_t  USBD_DFU_SOF (USBD_HandleTypeDef *pdev);

static uint8_t  USBD_DFU_IsoINIncomplete (USBD_HandleTypeDef *pdev, uint8_t epnum);

static uint8_t  USBD_DFU_IsoOutIncomplete (USBD_HandleTypeDef *pdev, uint8_t epnum);

#if (USBD_SUPPORT_USER_STRING == 1)  
static uint8_t* USBD_DFU_GetUsrStringDesc ( USBD_HandleTypeDef *pdev, uint8_t index , uint16_t *length);
#endif

static void DFU_Detach    (USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);

static void DFU_Download  (USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);

static void DFU_Upload    (USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);

static void DFU_GetStatus (USBD_HandleTypeDef *pdev);

static void DFU_ClearStatus (USBD_HandleTypeDef *pdev);

static void DFU_GetState  (USBD_HandleTypeDef *pdev);

static void DFU_Abort     (USBD_HandleTypeDef *pdev);

static void DFU_Leave  (USBD_HandleTypeDef *pdev); 


/**
  * @}
  */ 

/** @defgroup USBD_DFU_Private_Variables
  * @{
  */ 

USBD_ClassTypeDef  USBD_DFU = 
{
  USBD_DFU_Init,
  USBD_DFU_DeInit,
  USBD_DFU_Setup,
  USBD_DFU_EP0_TxReady,  
  USBD_DFU_EP0_RxReady,
  USBD_DFU_DataIn,
  USBD_DFU_DataOut,
  USBD_DFU_SOF,
  USBD_DFU_IsoINIncomplete,
  USBD_DFU_IsoOutIncomplete,      
  USBD_DFU_GetCfgDesc,
  USBD_DFU_GetCfgDesc, 
  USBD_DFU_GetCfgDesc,
  USBD_DFU_GetDeviceQualifierDesc,
#if (USBD_SUPPORT_USER_STRING == 1)  
  USBD_DFU_GetUsrStringDesc
#endif
};

/* USB DFU device Configuration Descriptor */
__ALIGN_BEGIN static uint8_t USBD_DFU_CfgDesc[USB_DFU_CONFIG_DESC_SIZ] __ALIGN_END =
{
  0x09, /* bLength: Configuation Descriptor size */
  USB_DESC_TYPE_CONFIGURATION, /* bDescriptorType: Configuration */
  USB_DFU_CONFIG_DESC_SIZ,
  /* wTotalLength: Bytes returned */
  0x00,
  0x01,         /*bNumInterfaces: 1 interface*/
  0x01,         /*bConfigurationValue: Configuration value*/
  0x02,         /*iConfiguration: Index of string descriptor describing the configuration*/
  0xC0,         /*bmAttributes: bus powered and Supprts Remote Wakeup */
  0x32,         /*MaxPower 100 mA: this current is used for detecting Vbus*/
  /* 09 */
  
  /**********  Descriptor of DFU interface 0 Alternate setting 0 **************/  
  USBD_DFU_IF_DESC(0), /* This interface is mandatory for all devices */
  
#if (USBD_DFU_MAX_ITF_NUM > 1)
  /**********  Descriptor of DFU interface 0 Alternate setting 1 **************/ 
  USBD_DFU_IF_DESC(1),
#endif /* (USBD_DFU_MAX_ITF_NUM > 1) */

#if (USBD_DFU_MAX_ITF_NUM > 2)
  /**********  Descriptor of DFU interface 0 Alternate setting 2 **************/ 
  USBD_DFU_IF_DESC(2),
#endif /* (USBD_DFU_MAX_ITF_NUM > 2) */

#if (USBD_DFU_MAX_ITF_NUM > 3)
  /**********  Descriptor of DFU interface 0 Alternate setting 3 **************/ 
  USBD_DFU_IF_DESC(3),
#endif /* (USBD_DFU_MAX_ITF_NUM > 3) */

#if (USBD_DFU_MAX_ITF_NUM > 4)
  /**********  Descriptor of DFU interface 0 Alternate setting 4 **************/ 
  USBD_DFU_IF_DESC(4),
#endif /* (USBD_DFU_MAX_ITF_NUM > 4) */

#if (USBD_DFU_MAX_ITF_NUM > 5)
  /**********  Descriptor of DFU interface 0 Alternate setting 5 **************/ 
  USBD_DFU_IF_DESC(5),
#endif /* (USBD_DFU_MAX_ITF_NUM > 5) */

#if (USBD_DFU_MAX_ITF_NUM > 6)
#error "ERROR: usbd_dfu_core.c: Modify the file to support more descriptors!"
#endif /* (USBD_DFU_MAX_ITF_NUM > 6) */

  /******************** DFU Functional Descriptor********************/
  0x09,   /*blength = 9 Bytes*/
  DFU_DESCRIPTOR_TYPE,   /* DFU Functional Descriptor*/
  0x0B,   /*bmAttribute
                bitCanDnload             = 1      (bit 0)
                bitCanUpload             = 1      (bit 1)
                bitManifestationTolerant = 0      (bit 2)
                bitWillDetach            = 1      (bit 3)
                Reserved                          (bit4-6)
                bitAcceleratedST         = 0      (bit 7)*/
  0xFF,   /*DetachTimeOut= 255 ms*/
  0x00,
  /*WARNING: In DMA mode the multiple MPS packets feature is still not supported
   ==> In this case, when using DMA USBD_DFU_XFER_SIZE should be set to 64 in usbd_conf.h */
  TRANSFER_SIZE_BYTES(USBD_DFU_XFER_SIZE),       /* TransferSize = 1024 Byte*/         
  0x1A,                                /* bcdDFUVersion*/
  0x01
  /***********************************************************/
  /* 9*/
};
  
/* USB Standard Device Descriptor */
__ALIGN_BEGIN static uint8_t USBD_DFU_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC] __ALIGN_END =
{
  USB_LEN_DEV_QUALIFIER_DESC,
  USB_DESC_TYPE_DEVICE_QUALIFIER,
  0x00,
  0x02,
  0x00,
  0x00,
  0x00,
  0x40,
  0x01,
  0x00,
};

/**
  * @}
  */ 

/** @defgroup USBD_DFU_Private_Functions
  * @{
  */ 

/**
  * @brief  USBD_DFU_Init
  *         Initialize the DFU interface
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t  USBD_DFU_Init (USBD_HandleTypeDef *pdev, 
                               uint8_t cfgidx)
{
  USBD_DFU_HandleTypeDef   *hdfu;
  
 /* Allocate Audio structure */
  pdev->pClassData = USBD_malloc(sizeof (USBD_DFU_HandleTypeDef));
  
  if(pdev->pClassData == NULL)
  {
    return USBD_FAIL; 
  }
  else
  {
    hdfu = pdev->pClassData;
    
    hdfu->alt_setting = 0;
    hdfu->data_ptr = USBD_DFU_APP_DEFAULT_ADD;
    hdfu->wblock_num = 0;
    hdfu->wlength = 0;
    
    hdfu->manif_state = DFU_MANIFEST_COMPLETE;
    hdfu->dev_state = DFU_STATE_IDLE;
    
    hdfu->dev_status[0] = DFU_ERROR_NONE;
    hdfu->dev_status[1] = 0;
    hdfu->dev_status[2] = 0;   
    hdfu->dev_status[3] = 0;
    hdfu->dev_status[4] = DFU_STATE_IDLE;    
    hdfu->dev_status[5] = 0;    
    
    /* Initialize Hardware layer */
    if (((USBD_DFU_MediaTypeDef *)pdev->pUserData)->Init() != USBD_OK)
    {
      return USBD_FAIL;
    }   
  }
  return USBD_OK;
}

/**
  * @brief  USBD_DFU_Init
  *         DeInitialize the DFU layer
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t  USBD_DFU_DeInit (USBD_HandleTypeDef *pdev, 
                                 uint8_t cfgidx)
{
  USBD_DFU_HandleTypeDef   *hdfu;
  hdfu = pdev->pClassData;
  
  hdfu->wblock_num = 0;
  hdfu->wlength = 0;

  hdfu->dev_state = DFU_STATE_IDLE;
  hdfu->dev_status[0] = DFU_ERROR_NONE;
  hdfu->dev_status[4] = DFU_STATE_IDLE;
 
  /* DeInit  physical Interface components */
  if(pdev->pClassData != NULL)
  {
    /* De-Initialize Hardware layer */
    ((USBD_DFU_MediaTypeDef *)pdev->pUserData)->DeInit(); 
    USBD_free(pdev->pClassData);
    pdev->pClassData = NULL;
  } 

  return USBD_OK;
}

/**
  * @brief  USBD_DFU_Setup
  *         Handle the DFU specific requests
  * @param  pdev: instance
  * @param  req: usb requests
  * @retval status
  */
static uint8_t  USBD_DFU_Setup (USBD_HandleTypeDef *pdev, 
                                USBD_SetupReqTypedef *req)
{
  uint8_t *pbuf = 0;
  uint16_t len = 0;
  uint8_t ret = USBD_OK;
  USBD_DFU_HandleTypeDef   *hdfu;
  
  hdfu = pdev->pClassData;
  
  switch (req->bmRequest & USB_REQ_TYPE_MASK)
  {
  case USB_REQ_TYPE_CLASS :  
    switch (req->bRequest)
    {
    case DFU_DNLOAD:
      DFU_Download(pdev, req);
      break;
      
    case DFU_UPLOAD:
      DFU_Upload(pdev, req);   
      break;
      
    case DFU_GETSTATUS:
      DFU_GetStatus(pdev);
      break;
      
    case DFU_CLRSTATUS:
      DFU_ClearStatus(pdev);
      break;      
      
    case DFU_GETSTATE:
      DFU_GetState(pdev);
      break;  
      
    case DFU_ABORT:
      DFU_Abort(pdev);
      break;
      
    case DFU_DETACH:
      DFU_Detach(pdev, req);
      break;
      
      
    default:
      USBD_CtlError (pdev, req);
      ret = USBD_FAIL; 
    }
    break;
    
  case USB_REQ_TYPE_STANDARD:
    switch (req->bRequest)
    {
    case USB_REQ_GET_DESCRIPTOR: 
      if( (req->wValue >> 8) == DFU_DESCRIPTOR_TYPE)
      {
        pbuf = USBD_DFU_CfgDesc + (9 * (USBD_DFU_MAX_ITF_NUM + 1));
        len = MIN(USB_DFU_DESC_SIZ , req->wLength);
      }
      
      USBD_CtlSendData (pdev, 
                        pbuf,
                        len);
      break;
      
    case USB_REQ_GET_INTERFACE :
      USBD_CtlSendData (pdev,
                        (uint8_t *)&hdfu->alt_setting,
                        1);
      break;
      
    case USB_REQ_SET_INTERFACE :
      if ((uint8_t)(req->wValue) < USBD_DFU_MAX_ITF_NUM)
      {
        hdfu->alt_setting = (uint8_t)(req->wValue);
      }
      else
      {
        /* Call the error management function (command will be nacked */
        USBD_CtlError (pdev, req);
        ret = USBD_FAIL;  
      }
      break;
      
    default:
      USBD_CtlError (pdev, req);
      ret = USBD_FAIL;     
    }
  }
  return ret;
}


/**
  * @brief  USBD_DFU_GetCfgDesc 
  *         return configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t  *USBD_DFU_GetCfgDesc (uint16_t *length)
{
  *length = sizeof (USBD_DFU_CfgDesc);
  return USBD_DFU_CfgDesc;
}

/**
  * @brief  USBD_DFU_DataIn
  *         handle data IN Stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t  USBD_DFU_DataIn (USBD_HandleTypeDef *pdev, 
                              uint8_t epnum)
{

  return USBD_OK;
}

/**
  * @brief  USBD_DFU_EP0_RxReady
  *         handle EP0 Rx Ready event
  * @param  pdev: device instance
  * @retval status
  */
static uint8_t  USBD_DFU_EP0_RxReady (USBD_HandleTypeDef *pdev)
{

  return USBD_OK;
}
/**
  * @brief  USBD_DFU_EP0_TxReady
  *         handle EP0 TRx Ready event
  * @param  pdev: device instance
  * @retval status
  */
static uint8_t  USBD_DFU_EP0_TxReady (USBD_HandleTypeDef *pdev)
{
 uint32_t addr;
 USBD_SetupReqTypedef     req; 
 USBD_DFU_HandleTypeDef   *hdfu;
 
 hdfu = pdev->pClassData;
  
  if (hdfu->dev_state == DFU_STATE_DNLOAD_BUSY)
  {
    /* Decode the Special Command*/
    if (hdfu->wblock_num == 0)   
    {
      if ((hdfu->buffer.d8[0] ==  DFU_CMD_GETCOMMANDS) && (hdfu->wlength == 1))
      {
       
      }
      else if  (( hdfu->buffer.d8[0] ==  DFU_CMD_SETADDRESSPOINTER ) && (hdfu->wlength == 5))
      {
        hdfu->data_ptr  = hdfu->buffer.d8[1];
        hdfu->data_ptr += hdfu->buffer.d8[2] << 8;
        hdfu->data_ptr += hdfu->buffer.d8[3] << 16;
        hdfu->data_ptr += hdfu->buffer.d8[4] << 24;
      }
      else if (( hdfu->buffer.d8[0] ==  DFU_CMD_ERASE ) && (hdfu->wlength == 5))
      {
        hdfu->data_ptr  = hdfu->buffer.d8[1];
        hdfu->data_ptr += hdfu->buffer.d8[2] << 8;
        hdfu->data_ptr += hdfu->buffer.d8[3] << 16;
        hdfu->data_ptr += hdfu->buffer.d8[4] << 24;
       
        if (((USBD_DFU_MediaTypeDef *)pdev->pUserData)->Erase(hdfu->data_ptr) != USBD_OK)
        {
          return USBD_FAIL;
        }
      }
      else
      {
        /* Reset the global length and block number */
        hdfu->wlength = 0;
        hdfu->wblock_num = 0;     
        /* Call the error management function (command will be nacked) */
        req.bmRequest = 0;
        req.wLength = 1;
        USBD_CtlError (pdev, &req);
      }
    }
    /* Regular Download Command */
    else if (hdfu->wblock_num > 1)  
    {
      /* Decode the required address */
      addr = ((hdfu->wblock_num - 2) * USBD_DFU_XFER_SIZE) + hdfu->data_ptr;
      
      /* Preform the write operation */
      if (((USBD_DFU_MediaTypeDef *)pdev->pUserData)->Write(hdfu->buffer.d8, (uint8_t *)addr, hdfu->wlength) != USBD_OK)
      {
        return USBD_FAIL;
      }      
    }
    /* Reset the global lenght and block number */
    hdfu->wlength = 0;
    hdfu->wblock_num = 0;
    
    /* Update the state machine */
    hdfu->dev_state =  DFU_STATE_DNLOAD_SYNC;

    hdfu->dev_status[1] = 0;
    hdfu->dev_status[2] = 0;
    hdfu->dev_status[3] = 0;
    hdfu->dev_status[4] = hdfu->dev_state;    
    return USBD_OK;
  }
  else if (hdfu->dev_state == DFU_STATE_MANIFEST)/* Manifestation in progress*/
  {
    /* Start leaving DFU mode */
    DFU_Leave(pdev);
  }
  
  return USBD_OK;
}
/**
  * @brief  USBD_DFU_SOF
  *         handle SOF event
  * @param  pdev: device instance
  * @retval status
  */
static uint8_t  USBD_DFU_SOF (USBD_HandleTypeDef *pdev)
{

  return USBD_OK;
}
/**
  * @brief  USBD_DFU_IsoINIncomplete
  *         handle data ISO IN Incomplete event
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t  USBD_DFU_IsoINIncomplete (USBD_HandleTypeDef *pdev, uint8_t epnum)
{

  return USBD_OK;
}
/**
  * @brief  USBD_DFU_IsoOutIncomplete
  *         handle data ISO OUT Incomplete event
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t  USBD_DFU_IsoOutIncomplete (USBD_HandleTypeDef *pdev, uint8_t epnum)
{

  return USBD_OK;
}
/**
  * @brief  USBD_DFU_DataOut
  *         handle data OUT Stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t  USBD_DFU_DataOut (USBD_HandleTypeDef *pdev, 
                              uint8_t epnum)
{

  return USBD_OK;
}

/**
* @brief  DeviceQualifierDescriptor 
*         return Device Qualifier descriptor
* @param  length : pointer data length
* @retval pointer to descriptor buffer
*/
static uint8_t  *USBD_DFU_GetDeviceQualifierDesc (uint16_t *length)
{
  *length = sizeof (USBD_DFU_DeviceQualifierDesc);
  return USBD_DFU_DeviceQualifierDesc;
}

/**
  * @brief  USBD_DFU_GetUsrStringDesc
  *         Manages the transfer of memory interfaces string descriptors.
  * @param  speed : current device speed
  * @param  index: desciptor index
  * @param  length : pointer data length
  * @retval pointer to the descriptor table or NULL if the descriptor is not supported.
  */
#if (USBD_SUPPORT_USER_STRING == 1)  
static uint8_t* USBD_DFU_GetUsrStringDesc (USBD_HandleTypeDef *pdev, uint8_t index , uint16_t *length)
{
  static uint8_t USBD_StrDesc[255];
  /* Check if the requested string interface is supported */
  if (index <= (USBD_IDX_INTERFACE_STR + USBD_DFU_MAX_ITF_NUM))
  {
    USBD_GetString ((uint8_t *)((USBD_DFU_MediaTypeDef *)pdev->pUserData)->pStrDesc, USBD_StrDesc, length);
    return USBD_StrDesc;  
  }
  /* Not supported Interface Descriptor index */
  else
  {
    return NULL;
  }
}
#endif

/**
* @brief  USBD_MSC_RegisterStorage
* @param  fops: storage callback
* @retval status
*/
uint8_t  USBD_DFU_RegisterMedia    (USBD_HandleTypeDef   *pdev, 
                                    USBD_DFU_MediaTypeDef *fops)
{
  if(fops != NULL)
  {
    pdev->pUserData= fops;
  }
  return 0;
}

/******************************************************************************
     DFU Class requests management
******************************************************************************/
/**
  * @brief  DFU_Detach
  *         Handles the DFU DETACH request.
  * @param  pdev: device instance
  * @param  req: pointer to the request structure.
  * @retval None.
  */
static void DFU_Detach(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
 USBD_DFU_HandleTypeDef   *hdfu;
 
 hdfu = pdev->pClassData;
 
  if (hdfu->dev_state == DFU_STATE_IDLE || hdfu->dev_state == DFU_STATE_DNLOAD_SYNC
      || hdfu->dev_state == DFU_STATE_DNLOAD_IDLE || hdfu->dev_state == DFU_STATE_MANIFEST_SYNC
        || hdfu->dev_state == DFU_STATE_UPLOAD_IDLE )
  {
    /* Update the state machine */
    hdfu->dev_state = DFU_STATE_IDLE;
    hdfu->dev_status[0] = DFU_ERROR_NONE;
    hdfu->dev_status[1] = 0;
    hdfu->dev_status[2] = 0;
    hdfu->dev_status[3] = 0; /*bwPollTimeout=0ms*/
    hdfu->dev_status[4] = hdfu->dev_state;
    hdfu->dev_status[5] = 0; /*iString*/
    hdfu->wblock_num = 0;
    hdfu->wlength = 0;
  } 
  
  /* Check the detach capability in the DFU functional descriptor */
  if ((USBD_DFU_CfgDesc[12 + (9 * USBD_DFU_MAX_ITF_NUM)]) & DFU_DETACH_MASK)
  {
    /* Perform an Attach-Detach operation on USB bus */
    USBD_Stop (pdev);
    USBD_Start (pdev);  
  }
  else
  {
    /* Wait for the period of time specified in Detach request */
    USBD_Delay (req->wValue);  
  }
}

/**
  * @brief  DFU_Download
  *         Handles the DFU DNLOAD request.
  * @param  pdev: device instance
  * @param  req: pointer to the request structure
  * @retval None
  */
static void DFU_Download(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
 USBD_DFU_HandleTypeDef   *hdfu;
 
 hdfu = pdev->pClassData;
 
  /* Data setup request */
  if (req->wLength > 0)
  {
    if ((hdfu->dev_state == DFU_STATE_IDLE) || (hdfu->dev_state == DFU_STATE_DNLOAD_IDLE))
    {
      /* Update the global length and block number */
      hdfu->wblock_num = req->wValue;
      hdfu->wlength = req->wLength;
      
      /* Update the state machine */
      hdfu->dev_state = DFU_STATE_DNLOAD_SYNC;
      hdfu->dev_status[4] = hdfu->dev_state;
      
      /* Prepare the reception of the buffer over EP0 */
      USBD_CtlPrepareRx (pdev,
                         (uint8_t*)hdfu->buffer.d8,                                  
                         hdfu->wlength);
    }
    /* Unsupported state */
    else
    {
      /* Call the error management function (command will be nacked */
      USBD_CtlError (pdev, req);
    }
  }
  /* 0 Data DNLOAD request */
  else
  {
    /* End of DNLOAD operation*/
    if (hdfu->dev_state == DFU_STATE_DNLOAD_IDLE || hdfu->dev_state == DFU_STATE_IDLE )
    {
      hdfu->manif_state = DFU_MANIFEST_IN_PROGRESS;
      hdfu->dev_state = DFU_STATE_MANIFEST_SYNC;
      hdfu->dev_status[1] = 0;
      hdfu->dev_status[2] = 0;
      hdfu->dev_status[3] = 0;
      hdfu->dev_status[4] = hdfu->dev_state;        
    }
    else
    {
      /* Call the error management function (command will be nacked */
      USBD_CtlError (pdev, req);
    }
  }  
}

/**
  * @brief  DFU_Upload
  *         Handles the DFU UPLOAD request.
  * @param  pdev: instance
  * @param  req: pointer to the request structure
  * @retval status
  */
static void DFU_Upload(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
 USBD_DFU_HandleTypeDef   *hdfu;
 
 hdfu = pdev->pClassData;
 
  uint8_t *phaddr = NULL;
  uint32_t addr = 0;
  
  /* Data setup request */
  if (req->wLength > 0)
  {
    if ((hdfu->dev_state == DFU_STATE_IDLE) || (hdfu->dev_state == DFU_STATE_UPLOAD_IDLE))
    {
      /* Update the global langth and block number */
      hdfu->wblock_num = req->wValue;
      hdfu->wlength = req->wLength;
      
      /* DFU Get Command */
      if (hdfu->wblock_num == 0)  
      {
        /* Update the state machine */
        hdfu->dev_state = (hdfu->wlength > 3)? DFU_STATE_IDLE:DFU_STATE_UPLOAD_IDLE;        
    
        hdfu->dev_status[1] = 0;
        hdfu->dev_status[2] = 0;
        hdfu->dev_status[3] = 0;
        hdfu->dev_status[4] = hdfu->dev_state;       
        
        /* Store the values of all supported commands */
        hdfu->buffer.d8[0] = DFU_CMD_GETCOMMANDS;
        hdfu->buffer.d8[1] = DFU_CMD_SETADDRESSPOINTER;
        hdfu->buffer.d8[2] = DFU_CMD_ERASE;
        
        /* Send the status data over EP0 */
        USBD_CtlSendData (pdev,
                          (uint8_t *)(&(hdfu->buffer.d8[0])),
                          3);
      }
      else if (hdfu->wblock_num > 1)
      {
        hdfu->dev_state = DFU_STATE_UPLOAD_IDLE ;
        
        hdfu->dev_status[1] = 0;
        hdfu->dev_status[2] = 0;
        hdfu->dev_status[3] = 0;
        hdfu->dev_status[4] = hdfu->dev_state;
        
        addr = ((hdfu->wblock_num - 2) * USBD_DFU_XFER_SIZE) + hdfu->data_ptr;  /* Change is Accelerated*/
        
        /* Return the physical address where data are stored */
        phaddr =  ((USBD_DFU_MediaTypeDef *)pdev->pUserData)->Read((uint8_t *)addr, hdfu->buffer.d8, hdfu->wlength);  
        
        /* Send the status data over EP0 */
        USBD_CtlSendData (pdev,
                          phaddr,
                          hdfu->wlength);
      }
      else  /* unsupported hdfu->wblock_num */
      {
        hdfu->dev_state = DFU_ERROR_STALLEDPKT;
        
        hdfu->dev_status[1] = 0;
        hdfu->dev_status[2] = 0;
        hdfu->dev_status[3] = 0;
        hdfu->dev_status[4] = hdfu->dev_state;        
        
        /* Call the error management function (command will be nacked */
        USBD_CtlError (pdev, req); 
      }
    }
    /* Unsupported state */
    else
    {
      hdfu->wlength = 0;
      hdfu->wblock_num = 0;   
      /* Call the error management function (command will be nacked */
      USBD_CtlError (pdev, req);
    }
  }
  /* No Data setup request */
  else
  {
    hdfu->dev_state = DFU_STATE_IDLE;
     
    hdfu->dev_status[1] = 0;
    hdfu->dev_status[2] = 0;
    hdfu->dev_status[3] = 0;
    hdfu->dev_status[4] = hdfu->dev_state;
  }
}

/**
  * @brief  DFU_GetStatus
  *         Handles the DFU GETSTATUS request.
  * @param  pdev: instance
  * @retval status
  */
static void DFU_GetStatus(USBD_HandleTypeDef *pdev)
{
 USBD_DFU_HandleTypeDef   *hdfu;
 
 hdfu = pdev->pClassData;
 
  switch (hdfu->dev_state)
  {
  case   DFU_STATE_DNLOAD_SYNC:
    if (hdfu->wlength != 0)
    {
      hdfu->dev_state = DFU_STATE_DNLOAD_BUSY;
       
      hdfu->dev_status[1] = 0;
      hdfu->dev_status[2] = 0;
      hdfu->dev_status[3] = 0;
      hdfu->dev_status[4] = hdfu->dev_state;
      
      if ((hdfu->wblock_num == 0) && (hdfu->buffer.d8[0] == DFU_CMD_ERASE))
      {
        ((USBD_DFU_MediaTypeDef *)pdev->pUserData)->GetStatus(hdfu->data_ptr, DFU_MEDIA_ERASE, hdfu->dev_status);
      }
      else
      {
        ((USBD_DFU_MediaTypeDef *)pdev->pUserData)->GetStatus(hdfu->data_ptr, DFU_MEDIA_PROGRAM, hdfu->dev_status);
      }
    }
    else  /* (hdfu->wlength==0)*/
    {
      hdfu->dev_state = DFU_STATE_DNLOAD_IDLE;

      hdfu->dev_status[1] = 0;
      hdfu->dev_status[2] = 0;
      hdfu->dev_status[3] = 0;
      hdfu->dev_status[4] = hdfu->dev_state;     
    }
    break;
    
  case   DFU_STATE_MANIFEST_SYNC :
    if (hdfu->manif_state == DFU_MANIFEST_IN_PROGRESS)
    {
      hdfu->dev_state = DFU_STATE_MANIFEST;
      
      hdfu->dev_status[1] = 1;             /*bwPollTimeout = 1ms*/
      hdfu->dev_status[2] = 0;
      hdfu->dev_status[3] = 0;
      hdfu->dev_status[4] = hdfu->dev_state;   
    }
    else if ((hdfu->manif_state == DFU_MANIFEST_COMPLETE) && \
      ((USBD_DFU_CfgDesc[(11 + (9 * USBD_DFU_MAX_ITF_NUM))]) & 0x04))
    {
      hdfu->dev_state = DFU_STATE_IDLE;
      
      hdfu->dev_status[1] = 0;
      hdfu->dev_status[2] = 0;
      hdfu->dev_status[3] = 0;
      hdfu->dev_status[4] = hdfu->dev_state;      
    }
    break;
    
  default :
    break;
  }
  
  /* Send the status data over EP0 */
  USBD_CtlSendData (pdev,
                    (uint8_t *)(&(hdfu->dev_status[0])),
                    6);
}

/**
  * @brief  DFU_ClearStatus 
  *         Handles the DFU CLRSTATUS request.
  * @param  pdev: device instance
  * @retval status
  */
static void DFU_ClearStatus(USBD_HandleTypeDef *pdev)
{
 USBD_DFU_HandleTypeDef   *hdfu;
 
 hdfu = pdev->pClassData;
 
  if (hdfu->dev_state == DFU_STATE_ERROR)
  {
    hdfu->dev_state = DFU_STATE_IDLE;
    hdfu->dev_status[0] = DFU_ERROR_NONE;/*bStatus*/
    hdfu->dev_status[1] = 0;
    hdfu->dev_status[2] = 0;
    hdfu->dev_status[3] = 0; /*bwPollTimeout=0ms*/
    hdfu->dev_status[4] = hdfu->dev_state;/*bState*/
    hdfu->dev_status[5] = 0;/*iString*/
  }
  else
  {   /*State Error*/
    hdfu->dev_state = DFU_STATE_ERROR;
    hdfu->dev_status[0] = DFU_ERROR_UNKNOWN;/*bStatus*/
    hdfu->dev_status[1] = 0;
    hdfu->dev_status[2] = 0;
    hdfu->dev_status[3] = 0; /*bwPollTimeout=0ms*/
    hdfu->dev_status[4] = hdfu->dev_state;/*bState*/
    hdfu->dev_status[5] = 0;/*iString*/
  }
}

/**
  * @brief  DFU_GetState
  *         Handles the DFU GETSTATE request.
  * @param  pdev: device instance
  * @retval None
  */
static void DFU_GetState(USBD_HandleTypeDef *pdev)
{
 USBD_DFU_HandleTypeDef   *hdfu;
 
 hdfu = pdev->pClassData;
 
  /* Return the current state of the DFU interface */
  USBD_CtlSendData (pdev, 
                    &hdfu->dev_state,
                    1);  
}

/**
  * @brief  DFU_Abort
  *         Handles the DFU ABORT request.
  * @param  pdev: device instance
  * @retval None
  */
static void DFU_Abort(USBD_HandleTypeDef *pdev)
{
 USBD_DFU_HandleTypeDef   *hdfu;
 
 hdfu = pdev->pClassData;
 
  if (hdfu->dev_state == DFU_STATE_IDLE || hdfu->dev_state == DFU_STATE_DNLOAD_SYNC
      || hdfu->dev_state == DFU_STATE_DNLOAD_IDLE || hdfu->dev_state == DFU_STATE_MANIFEST_SYNC
        || hdfu->dev_state == DFU_STATE_UPLOAD_IDLE )
  {
    hdfu->dev_state = DFU_STATE_IDLE;
    hdfu->dev_status[0] = DFU_ERROR_NONE;
    hdfu->dev_status[1] = 0;
    hdfu->dev_status[2] = 0;
    hdfu->dev_status[3] = 0; /*bwPollTimeout=0ms*/
    hdfu->dev_status[4] = hdfu->dev_state;
    hdfu->dev_status[5] = 0; /*iString*/
    hdfu->wblock_num = 0;
    hdfu->wlength = 0;
  }  
}

/**
  * @brief  DFU_Leave
  *         Handles the sub-protocol DFU leave DFU mode request (leaves DFU mode
  *         and resets device to jump to user loaded code).
  * @param  pdev: device instance
  * @retval None
  */
void DFU_Leave(USBD_HandleTypeDef *pdev)
{
 USBD_DFU_HandleTypeDef   *hdfu;
 
 hdfu = pdev->pClassData;
 
 hdfu->manif_state = DFU_MANIFEST_COMPLETE;

  if ((USBD_DFU_CfgDesc[(11 + (9 * USBD_DFU_MAX_ITF_NUM))]) & 0x04)
  {
    hdfu->dev_state = DFU_STATE_MANIFEST_SYNC;

    hdfu->dev_status[1] = 0;
    hdfu->dev_status[2] = 0;
    hdfu->dev_status[3] = 0;
    hdfu->dev_status[4] = hdfu->dev_state;       
    return;
  }
  else
  {
    
    hdfu->dev_state = DFU_STATE_MANIFEST_WAIT_RESET;
    
    hdfu->dev_status[1] = 0;
    hdfu->dev_status[2] = 0;
    hdfu->dev_status[3] = 0;
    hdfu->dev_status[4] = hdfu->dev_state;     
    
    /* Disconnect the USB device */
    USBD_Stop (pdev);

    /* DeInitilialize the MAL(Media Access Layer) */
    ((USBD_DFU_MediaTypeDef *)pdev->pUserData)->DeInit();
    
    /* Generate system reset to allow jumping to the user code */
    NVIC_SystemReset();
   
    /* This instruction will not be reached (system reset) */
    for(;;);
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
