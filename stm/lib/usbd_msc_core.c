/**
  ******************************************************************************
  * @file    usbd_msc_core.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-March-2012
  * @brief   This file provides all the MSC core functions.
  *
  * @verbatim
  *      
  *          ===================================================================      
  *                                MSC Class  Description
  *          =================================================================== 
  *           This module manages the MSC class V1.0 following the "Universal 
  *           Serial Bus Mass Storage Class (MSC) Bulk-Only Transport (BOT) Version 1.0
  *           Sep. 31, 1999".
  *           This driver implements the following aspects of the specification:
  *             - Bulk-Only Transport protocol
  *             - Subclass : SCSI transparent command set (ref. SCSI Primary Commands - 3 (SPC-3))
  *      
  *  @endverbatim
  *
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
#include "usbd_msc_mem.h"
#include "usbd_msc_core.h"
#include "usbd_msc_bot.h"
#include "usbd_msc_conf.h"
#include "usbd_req.h"


/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @{
  */


/** @defgroup MSC_CORE 
  * @brief Mass storage core module
  * @{
  */ 

/** @defgroup MSC_CORE_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup MSC_CORE_Private_Defines
  * @{
  */ 

/**
  * @}
  */ 


/** @defgroup MSC_CORE_Private_Macros
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup MSC_CORE_Private_FunctionPrototypes
  * @{
  */ 
uint8_t  USBD_MSC_Init (void  *pdev, 
                            uint8_t cfgidx);

uint8_t  USBD_MSC_DeInit (void  *pdev, 
                              uint8_t cfgidx);

uint8_t  USBD_MSC_Setup (void  *pdev, 
                             USB_SETUP_REQ *req);

uint8_t  USBD_MSC_DataIn (void  *pdev, 
                              uint8_t epnum);


uint8_t  USBD_MSC_DataOut (void  *pdev, 
                               uint8_t epnum);

uint8_t  *USBD_MSC_GetCfgDesc (uint8_t speed, 
                                      uint16_t *length);

#ifdef USB_OTG_HS_CORE  
uint8_t  *USBD_MSC_GetOtherCfgDesc (uint8_t speed, 
                                      uint16_t *length);
#endif


uint8_t USBD_MSC_CfgDesc[USB_MSC_CONFIG_DESC_SIZ];




/**
  * @}
  */ 


/** @defgroup MSC_CORE_Private_Variables
  * @{
  */ 


USBD_Class_cb_TypeDef  USBD_MSC_cb = 
{
  USBD_MSC_Init,
  USBD_MSC_DeInit,
  USBD_MSC_Setup,
  NULL, /*EP0_TxSent*/  
  NULL, /*EP0_RxReady*/
  USBD_MSC_DataIn,
  USBD_MSC_DataOut,
  NULL, /*SOF */ 
  NULL,  
  NULL,     
  USBD_MSC_GetCfgDesc,
#ifdef USB_OTG_HS_CORE  
  USBD_MSC_GetOtherCfgDesc,
#endif
};

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4   
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
/* USB Mass storage device Configuration Descriptor */
/*   All Descriptors (Configuration, Interface, Endpoint, Class, Vendor */
__ALIGN_BEGIN uint8_t USBD_MSC_CfgDesc[USB_MSC_CONFIG_DESC_SIZ] __ALIGN_END =
{
  
  0x09,   /* bLength: Configuation Descriptor size */
  USB_DESC_TYPE_CONFIGURATION,   /* bDescriptorType: Configuration */
  USB_MSC_CONFIG_DESC_SIZ,
  
  0x00,
  0x01,   /* bNumInterfaces: 1 interface */
  0x01,   /* bConfigurationValue: */
  0x04,   /* iConfiguration: */
  0xC0,   /* bmAttributes: */
  0x32,   /* MaxPower 100 mA */
  
  /********************  Mass Storage interface ********************/
  0x09,   /* bLength: Interface Descriptor size */
  0x04,   /* bDescriptorType: */
  0x00,   /* bInterfaceNumber: Number of Interface */
  0x00,   /* bAlternateSetting: Alternate setting */
  0x02,   /* bNumEndpoints*/
  0x08,   /* bInterfaceClass: MSC Class */
  0x06,   /* bInterfaceSubClass : SCSI transparent*/
  0x50,   /* nInterfaceProtocol */
  0x05,          /* iInterface: */
  /********************  Mass Storage Endpoints ********************/
  0x07,   /*Endpoint descriptor length = 7*/
  0x05,   /*Endpoint descriptor type */
  MSC_IN_EP,   /*Endpoint address (IN, address 1) */
  0x02,   /*Bulk endpoint type */
  LOBYTE(MSC_MAX_PACKET),
  HIBYTE(MSC_MAX_PACKET),
  0x00,   /*Polling interval in milliseconds */
  
  0x07,   /*Endpoint descriptor length = 7 */
  0x05,   /*Endpoint descriptor type */
  MSC_OUT_EP,   /*Endpoint address (OUT, address 1) */
  0x02,   /*Bulk endpoint type */
  LOBYTE(MSC_MAX_PACKET),
  HIBYTE(MSC_MAX_PACKET),
  0x00     /*Polling interval in milliseconds*/
};
#ifdef USB_OTG_HS_CORE 
 #ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
   #if defined ( __ICCARM__ ) /*!< IAR Compiler */
     #pragma data_alignment=4   
   #endif
 #endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN uint8_t USBD_MSC_OtherCfgDesc[USB_MSC_CONFIG_DESC_SIZ] __ALIGN_END =
{
  
  0x09,   /* bLength: Configuation Descriptor size */
  USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION,   
  USB_MSC_CONFIG_DESC_SIZ,
  
  0x00,
  0x01,   /* bNumInterfaces: 1 interface */
  0x01,   /* bConfigurationValue: */
  0x04,   /* iConfiguration: */
  0xC0,   /* bmAttributes: */
  0x32,   /* MaxPower 100 mA */
  
  /********************  Mass Storage interface ********************/
  0x09,   /* bLength: Interface Descriptor size */
  0x04,   /* bDescriptorType: */
  0x00,   /* bInterfaceNumber: Number of Interface */
  0x00,   /* bAlternateSetting: Alternate setting */
  0x02,   /* bNumEndpoints*/
  0x08,   /* bInterfaceClass: MSC Class */
  0x06,   /* bInterfaceSubClass : SCSI transparent command set*/
  0x50,   /* nInterfaceProtocol */
  0x05,          /* iInterface: */
  /********************  Mass Storage Endpoints ********************/
  0x07,   /*Endpoint descriptor length = 7*/
  0x05,   /*Endpoint descriptor type */
  MSC_IN_EP,   /*Endpoint address (IN, address 1) */
  0x02,   /*Bulk endpoint type */
  0x40,
  0x00,
  0x00,   /*Polling interval in milliseconds */
  
  0x07,   /*Endpoint descriptor length = 7 */
  0x05,   /*Endpoint descriptor type */
  MSC_OUT_EP,   /*Endpoint address (OUT, address 1) */
  0x02,   /*Bulk endpoint type */
  0x40,
  0x00,
  0x00     /*Polling interval in milliseconds*/
};
#endif 

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4   
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN static uint8_t  USBD_MSC_MaxLun  __ALIGN_END = 0;

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4   
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN static uint8_t  USBD_MSC_AltSet  __ALIGN_END = 0;

/**
  * @}
  */ 


/** @defgroup MSC_CORE_Private_Functions
  * @{
  */ 

/**
* @brief  USBD_MSC_Init
*         Initialize  the mass storage configuration
* @param  pdev: device instance
* @param  cfgidx: configuration index
* @retval status
*/
uint8_t  USBD_MSC_Init (void  *pdev, 
                            uint8_t cfgidx)
{
  USBD_MSC_DeInit(pdev , cfgidx );
  
  /* Open EP IN */
  DCD_EP_Open(pdev,
              MSC_IN_EP,
              MSC_EPIN_SIZE,
              USB_OTG_EP_BULK);
  
  /* Open EP OUT */
  DCD_EP_Open(pdev,
              MSC_OUT_EP,
              MSC_EPOUT_SIZE,
              USB_OTG_EP_BULK);
 
  /* Init the BOT  layer */
  MSC_BOT_Init(pdev); 
  
  return USBD_OK;
}

/**
* @brief  USBD_MSC_DeInit
*         DeInitilaize  the mass storage configuration
* @param  pdev: device instance
* @param  cfgidx: configuration index
* @retval status
*/
uint8_t  USBD_MSC_DeInit (void  *pdev, 
                              uint8_t cfgidx)
{
  /* Close MSC EPs */
  DCD_EP_Close (pdev , MSC_IN_EP);
  DCD_EP_Close (pdev , MSC_OUT_EP);
  
  /* Un Init the BOT layer */
  MSC_BOT_DeInit(pdev);   
  return USBD_OK;
}
/**
* @brief  USBD_MSC_Setup
*         Handle the MSC specific requests
* @param  pdev: device instance
* @param  req: USB request
* @retval status
*/
uint8_t  USBD_MSC_Setup (void  *pdev, USB_SETUP_REQ *req)
{
  
  switch (req->bmRequest & USB_REQ_TYPE_MASK)
  {

  /* Class request */
  case USB_REQ_TYPE_CLASS :
    switch (req->bRequest)
    {
    case BOT_GET_MAX_LUN :

      if((req->wValue  == 0) && 
         (req->wLength == 1) &&
         ((req->bmRequest & 0x80) == 0x80))
      {
        USBD_MSC_MaxLun = USBD_STORAGE_fops->GetMaxLun();
        if(USBD_MSC_MaxLun > 0)
        {
           USBD_CtlSendData (pdev,
                             &USBD_MSC_MaxLun,
                              1);
        }
        else
        {
          USBD_CtlError(pdev , req);
          return USBD_FAIL; 
          
        }
      }
      else
      {
         USBD_CtlError(pdev , req);
         return USBD_FAIL; 
      }
      break;
      
    case BOT_RESET :
      if((req->wValue  == 0) && 
         (req->wLength == 0) &&
        ((req->bmRequest & 0x80) != 0x80))
      {      
         MSC_BOT_Reset(pdev);
      }
      else
      {
         USBD_CtlError(pdev , req);
         return USBD_FAIL; 
      }
      break;

    default:
       USBD_CtlError(pdev , req);
       return USBD_FAIL; 
    }
    break;
  /* Interface & Endpoint request */
  case USB_REQ_TYPE_STANDARD:
    switch (req->bRequest)
    {
    case USB_REQ_GET_INTERFACE :
      USBD_CtlSendData (pdev,
                        &USBD_MSC_AltSet,
                        1);
      break;
      
    case USB_REQ_SET_INTERFACE :
      USBD_MSC_AltSet = (uint8_t)(req->wValue);
      break;
    
    case USB_REQ_CLEAR_FEATURE:  
      
      /* Flush the FIFO and Clear the stall status */    
      DCD_EP_Flush(pdev, (uint8_t)req->wIndex);
      
      /* Re-activate the EP */      
      DCD_EP_Close (pdev , (uint8_t)req->wIndex);
      if((((uint8_t)req->wIndex) & 0x80) == 0x80)
      {
        DCD_EP_Open(pdev,
                    ((uint8_t)req->wIndex),
                    MSC_EPIN_SIZE,
                    USB_OTG_EP_BULK);
      }
      else
      {
        DCD_EP_Open(pdev,
                    ((uint8_t)req->wIndex),
                    MSC_EPOUT_SIZE,
                    USB_OTG_EP_BULK);
      }
      
      /* Handle BOT error */
      MSC_BOT_CplClrFeature(pdev, (uint8_t)req->wIndex);
      break;
      
    }  
    break;
   
  default:
    break;
  }
  return USBD_OK;
}

/**
* @brief  USBD_MSC_DataIn
*         handle data IN Stage
* @param  pdev: device instance
* @param  epnum: endpoint index
* @retval status
*/
uint8_t  USBD_MSC_DataIn (void  *pdev, 
                              uint8_t epnum)
{
  MSC_BOT_DataIn(pdev , epnum);
  return USBD_OK;
}

/**
* @brief  USBD_MSC_DataOut
*         handle data OUT Stage
* @param  pdev: device instance
* @param  epnum: endpoint index
* @retval status
*/
uint8_t  USBD_MSC_DataOut (void  *pdev, 
                               uint8_t epnum)
{
  MSC_BOT_DataOut(pdev , epnum);
  return USBD_OK;
}

/**
* @brief  USBD_MSC_GetCfgDesc 
*         return configuration descriptor
* @param  speed : current device speed
* @param  length : pointer data length
* @retval pointer to descriptor buffer
*/
uint8_t  *USBD_MSC_GetCfgDesc (uint8_t speed, uint16_t *length)
{
  *length = sizeof (USBD_MSC_CfgDesc);
  return USBD_MSC_CfgDesc;
}

/**
* @brief  USBD_MSC_GetOtherCfgDesc 
*         return other speed configuration descriptor
* @param  speed : current device speed
* @param  length : pointer data length
* @retval pointer to descriptor buffer
*/
#ifdef USB_OTG_HS_CORE  
uint8_t  *USBD_MSC_GetOtherCfgDesc (uint8_t speed, 
                                      uint16_t *length)
{
  *length = sizeof (USBD_MSC_OtherCfgDesc);
  return USBD_MSC_OtherCfgDesc;
}
#endif
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
