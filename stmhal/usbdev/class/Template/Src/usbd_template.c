/**
  ******************************************************************************
  * @file    usbd_template.c
  * @author  MCD Application Team
  * @version V2.0.0
  * @date    18-February-2014
  * @brief   This file provides the HID core functions.
  *
  * @verbatim
  *      
  *          ===================================================================      
  *                                TEMPLATE Class  Description
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
#include "usbd_template.h"
#include "usbd_desc.h"
#include "usbd_ctlreq.h"


/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @{
  */


/** @defgroup USBD_TEMPLATE 
  * @brief usbd core module
  * @{
  */ 

/** @defgroup USBD_TEMPLATE_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBD_TEMPLATE_Private_Defines
  * @{
  */ 

/**
  * @}
  */ 


/** @defgroup USBD_TEMPLATE_Private_Macros
  * @{
  */ 
                                         
/**
  * @}
  */ 




/** @defgroup USBD_TEMPLATE_Private_FunctionPrototypes
  * @{
  */


static uint8_t  USBD_TEMPLATE_Init (USBD_HandleTypeDef *pdev, 
                               uint8_t cfgidx);

static uint8_t  USBD_TEMPLATE_DeInit (USBD_HandleTypeDef *pdev, 
                                 uint8_t cfgidx);

static uint8_t  USBD_TEMPLATE_Setup (USBD_HandleTypeDef *pdev, 
                                USBD_SetupReqTypedef *req);

static uint8_t  *USBD_TEMPLATE_GetCfgDesc (uint16_t *length);

static uint8_t  *USBD_TEMPLATE_GetDeviceQualifierDesc (uint16_t *length);

static uint8_t  USBD_TEMPLATE_DataIn (USBD_HandleTypeDef *pdev, uint8_t epnum);

static uint8_t  USBD_TEMPLATE_DataOut (USBD_HandleTypeDef *pdev, uint8_t epnum);

static uint8_t  USBD_TEMPLATE_EP0_RxReady (USBD_HandleTypeDef *pdev);

static uint8_t  USBD_TEMPLATE_EP0_TxReady (USBD_HandleTypeDef *pdev);

static uint8_t  USBD_TEMPLATE_SOF (USBD_HandleTypeDef *pdev);

static uint8_t  USBD_TEMPLATE_IsoINIncomplete (USBD_HandleTypeDef *pdev, uint8_t epnum);

static uint8_t  USBD_TEMPLATE_IsoOutIncomplete (USBD_HandleTypeDef *pdev, uint8_t epnum);

/**
  * @}
  */ 

/** @defgroup USBD_TEMPLATE_Private_Variables
  * @{
  */ 

USBD_ClassTypeDef  USBD_TEMPLATE_ClassDriver = 
{
  USBD_TEMPLATE_Init,
  USBD_TEMPLATE_DeInit,
  USBD_TEMPLATE_Setup,
  USBD_TEMPLATE_EP0_TxReady,  
  USBD_TEMPLATE_EP0_RxReady,
  USBD_TEMPLATE_DataIn,
  USBD_TEMPLATE_DataOut,
  USBD_TEMPLATE_SOF,
  USBD_TEMPLATE_IsoINIncomplete,
  USBD_TEMPLATE_IsoOutIncomplete,      
  USBD_TEMPLATE_GetCfgDesc,
  USBD_TEMPLATE_GetCfgDesc, 
  USBD_TEMPLATE_GetCfgDesc,
  USBD_TEMPLATE_GetDeviceQualifierDesc,
};

#if defined ( __ICCARM__ ) /*!< IAR Compiler */
  #pragma data_alignment=4   
#endif
/* USB TEMPLATE device Configuration Descriptor */
static uint8_t USBD_TEMPLATE_CfgDesc[USB_TEMPLATE_CONFIG_DESC_SIZ] =
{
  0x09, /* bLength: Configuation Descriptor size */
  USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION, /* bDescriptorType: Configuration */
  USB_TEMPLATE_CONFIG_DESC_SIZ,
  /* wTotalLength: Bytes returned */
  0x00,
  0x01,         /*bNumInterfaces: 1 interface*/
  0x01,         /*bConfigurationValue: Configuration value*/
  0x02,         /*iConfiguration: Index of string descriptor describing the configuration*/
  0xC0,         /*bmAttributes: bus powered and Supprts Remote Wakeup */
  0x32,         /*MaxPower 100 mA: this current is used for detecting Vbus*/
  /* 09 */
  
  /**********  Descriptor of TEMPLATE interface 0 Alternate setting 0 **************/  
 
};
  
#if defined ( __ICCARM__ ) /*!< IAR Compiler */
  #pragma data_alignment=4   
#endif
/* USB Standard Device Descriptor */
static uint8_t USBD_TEMPLATE_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC] =
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

/** @defgroup USBD_TEMPLATE_Private_Functions
  * @{
  */ 

/**
  * @brief  USBD_TEMPLATE_Init
  *         Initialize the TEMPLATE interface
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t  USBD_TEMPLATE_Init (USBD_HandleTypeDef *pdev, 
                               uint8_t cfgidx)
{
  uint8_t ret = 0;
  

  return ret;
}

/**
  * @brief  USBD_TEMPLATE_Init
  *         DeInitialize the TEMPLATE layer
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t  USBD_TEMPLATE_DeInit (USBD_HandleTypeDef *pdev, 
                                 uint8_t cfgidx)
{

  return USBD_OK;
}

/**
  * @brief  USBD_TEMPLATE_Setup
  *         Handle the TEMPLATE specific requests
  * @param  pdev: instance
  * @param  req: usb requests
  * @retval status
  */
static uint8_t  USBD_TEMPLATE_Setup (USBD_HandleTypeDef *pdev, 
                                USBD_SetupReqTypedef *req)
{
 
  switch (req->bmRequest & USB_REQ_TYPE_MASK)
  {
  case USB_REQ_TYPE_CLASS :  
    switch (req->bRequest)
    {
      
    default:
      USBD_CtlError (pdev, req);
      return USBD_FAIL; 
    }
    break;
    
  case USB_REQ_TYPE_STANDARD:
    switch (req->bRequest)
    {
    
    default:
      USBD_CtlError (pdev, req);
      return USBD_FAIL;     
    }
  }
  return USBD_OK;
}


/**
  * @brief  USBD_TEMPLATE_GetCfgDesc 
  *         return configuration descriptor
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t  *USBD_TEMPLATE_GetCfgDesc (uint16_t *length)
{
  *length = sizeof (USBD_TEMPLATE_CfgDesc);
  return USBD_TEMPLATE_CfgDesc;
}

/**
* @brief  DeviceQualifierDescriptor 
*         return Device Qualifier descriptor
* @param  length : pointer data length
* @retval pointer to descriptor buffer
*/
uint8_t  *USBD_TEMPLATE_DeviceQualifierDescriptor (uint16_t *length)
{
  *length = sizeof (USBD_TEMPLATE_DeviceQualifierDesc);
  return USBD_TEMPLATE_DeviceQualifierDesc;
}


/**
  * @brief  USBD_TEMPLATE_DataIn
  *         handle data IN Stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t  USBD_TEMPLATE_DataIn (USBD_HandleTypeDef *pdev, 
                              uint8_t epnum)
{

  return USBD_OK;
}

/**
  * @brief  USBD_TEMPLATE_EP0_RxReady
  *         handle EP0 Rx Ready event
  * @param  pdev: device instance
  * @retval status
  */
static uint8_t  USBD_TEMPLATE_EP0_RxReady (USBD_HandleTypeDef *pdev)
{

  return USBD_OK;
}
/**
  * @brief  USBD_TEMPLATE_EP0_TxReady
  *         handle EP0 TRx Ready event
  * @param  pdev: device instance
  * @retval status
  */
static uint8_t  USBD_TEMPLATE_EP0_TxReady (USBD_HandleTypeDef *pdev)
{

  return USBD_OK;
}
/**
  * @brief  USBD_TEMPLATE_SOF
  *         handle SOF event
  * @param  pdev: device instance
  * @retval status
  */
static uint8_t  USBD_TEMPLATE_SOF (USBD_HandleTypeDef *pdev)
{

  return USBD_OK;
}
/**
  * @brief  USBD_TEMPLATE_IsoINIncomplete
  *         handle data ISO IN Incomplete event
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t  USBD_TEMPLATE_IsoINIncomplete (USBD_HandleTypeDef *pdev, uint8_t epnum)
{

  return USBD_OK;
}
/**
  * @brief  USBD_TEMPLATE_IsoOutIncomplete
  *         handle data ISO OUT Incomplete event
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t  USBD_TEMPLATE_IsoOutIncomplete (USBD_HandleTypeDef *pdev, uint8_t epnum)
{

  return USBD_OK;
}
/**
  * @brief  USBD_TEMPLATE_DataOut
  *         handle data OUT Stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t  USBD_TEMPLATE_DataOut (USBD_HandleTypeDef *pdev, 
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
uint8_t  *USBD_TEMPLATE_GetDeviceQualifierDesc (uint16_t *length)
{
  *length = sizeof (USBD_TEMPLATE_DeviceQualifierDesc);
  return USBD_TEMPLATE_DeviceQualifierDesc;
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
