/**
  ******************************************************************************
  * @file    usbh_hid.h
  * @author  MCD Application Team
  * @version V3.0.0
  * @date    18-February-2014
  * @brief   This file contains all the prototypes for the usbh_hid.c
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
#ifndef __USBH_HID_H
#define __USBH_HID_H

/* Includes ------------------------------------------------------------------*/
#include "usbh_core.h"
#include "usbh_hid_mouse.h"
#include "usbh_hid_keybd.h"
 
/** @addtogroup USBH_LIB
  * @{
  */

/** @addtogroup USBH_CLASS
  * @{
  */

/** @addtogroup USBH_HID_CLASS
  * @{
  */
  
/** @defgroup USBH_HID_CORE
  * @brief This file is the Header file for USBH_HID_CORE.c
  * @{
  */ 


/** @defgroup USBH_HID_CORE_Exported_Types
  * @{
  */ 

#define HID_MIN_POLL                10
#define HID_REPORT_SIZE             16    
#define HID_MAX_USAGE               10
#define HID_MAX_NBR_REPORT_FMT      10 
#define HID_QUEUE_SIZE              10    
    
#define  HID_ITEM_LONG                              0xFE
                                                                       
#define  HID_ITEM_TYPE_MAIN                         0x00
#define  HID_ITEM_TYPE_GLOBAL                       0x01
#define  HID_ITEM_TYPE_LOCAL                        0x02
#define  HID_ITEM_TYPE_RESERVED                     0x03

                                                                         
#define  HID_MAIN_ITEM_TAG_INPUT                    0x08
#define  HID_MAIN_ITEM_TAG_OUTPUT                   0x09
#define  HID_MAIN_ITEM_TAG_COLLECTION               0x0A
#define  HID_MAIN_ITEM_TAG_FEATURE                  0x0B
#define  HID_MAIN_ITEM_TAG_ENDCOLLECTION            0x0C

                                                                         
#define  HID_GLOBAL_ITEM_TAG_USAGE_PAGE             0x00
#define  HID_GLOBAL_ITEM_TAG_LOG_MIN                0x01
#define  HID_GLOBAL_ITEM_TAG_LOG_MAX                0x02
#define  HID_GLOBAL_ITEM_TAG_PHY_MIN                0x03
#define  HID_GLOBAL_ITEM_TAG_PHY_MAX                0x04
#define  HID_GLOBAL_ITEM_TAG_UNIT_EXPONENT          0x05
#define  HID_GLOBAL_ITEM_TAG_UNIT                   0x06
#define  HID_GLOBAL_ITEM_TAG_REPORT_SIZE            0x07
#define  HID_GLOBAL_ITEM_TAG_REPORT_ID              0x08
#define  HID_GLOBAL_ITEM_TAG_REPORT_COUNT           0x09
#define  HID_GLOBAL_ITEM_TAG_PUSH                   0x0A
#define  HID_GLOBAL_ITEM_TAG_POP                    0x0B

                                                                         
#define  HID_LOCAL_ITEM_TAG_USAGE                   0x00
#define  HID_LOCAL_ITEM_TAG_USAGE_MIN               0x01
#define  HID_LOCAL_ITEM_TAG_USAGE_MAX               0x02
#define  HID_LOCAL_ITEM_TAG_DESIGNATOR_INDEX        0x03
#define  HID_LOCAL_ITEM_TAG_DESIGNATOR_MIN          0x04
#define  HID_LOCAL_ITEM_TAG_DESIGNATOR_MAX          0x05
#define  HID_LOCAL_ITEM_TAG_STRING_INDEX            0x07
#define  HID_LOCAL_ITEM_TAG_STRING_MIN              0x08
#define  HID_LOCAL_ITEM_TAG_STRING_MAX              0x09
#define  HID_LOCAL_ITEM_TAG_DELIMITER               0x0A
    

/* States for HID State Machine */
typedef enum
{
  HID_INIT= 0,  
  HID_IDLE,
  HID_SEND_DATA,
  HID_BUSY,
  HID_GET_DATA,   
  HID_SYNC,     
  HID_POLL,
  HID_ERROR,
}
HID_StateTypeDef;

typedef enum
{
  HID_REQ_INIT = 0,
  HID_REQ_IDLE, 
  HID_REQ_GET_REPORT_DESC,
  HID_REQ_GET_HID_DESC,
  HID_REQ_SET_IDLE,
  HID_REQ_SET_PROTOCOL,
  HID_REQ_SET_REPORT,

}
HID_CtlStateTypeDef;

typedef enum
{
  HID_MOUSE    = 0x01,
  HID_KEYBOARD = 0x02,
  HID_UNKNOWN = 0xFF,
}
HID_TypeTypeDef;


typedef  struct  _HID_ReportData 
{
    uint8_t   ReportID;    
    uint8_t   ReportType;  
    uint16_t  UsagePage;   
    uint32_t  Usage[HID_MAX_USAGE]; 
    uint32_t  NbrUsage;                      
    uint32_t  UsageMin;                      
    uint32_t  UsageMax;                      
    int32_t   LogMin;                        
    int32_t   LogMax;                        
    int32_t   PhyMin;                        
    int32_t   PhyMax;                        
    int32_t   UnitExp;                       
    uint32_t  Unit;                          
    uint32_t  ReportSize;                    
    uint32_t  ReportCnt;                     
    uint32_t  Flag;                          
    uint32_t  PhyUsage;                      
    uint32_t  AppUsage;                      
    uint32_t  LogUsage;   
} 
HID_ReportDataTypeDef;

typedef  struct  _HID_ReportIDTypeDef {
    uint8_t  Size;         /* Report size return by the device id            */
    uint8_t  ReportID;     /* Report Id                                      */
    uint8_t  Type;         /* Report Type (INPUT/OUTPUT/FEATURE)             */
} HID_ReportIDTypeDef;

typedef struct  _HID_CollectionTypeDef 
{
    uint32_t                       Usage;                                               
    uint8_t                        Type;                                                
    struct _HID_CollectionTypeDef  *NextPtr;
} HID_CollectionTypeDef;

                                                                               
typedef  struct  _HID_AppCollectionTypeDef {
    uint32_t               Usage;                                                
    uint8_t                Type;                                                 
    uint8_t                NbrReportFmt;                                         
    HID_ReportDataTypeDef  ReportData[HID_MAX_NBR_REPORT_FMT];               
} HID_AppCollectionTypeDef;


typedef struct _HIDDescriptor
{
  uint8_t   bLength;
  uint8_t   bDescriptorType;
  uint16_t  bcdHID;               /* indicates what endpoint this descriptor is describing */
  uint8_t   bCountryCode;        /* specifies the transfer type. */
  uint8_t   bNumDescriptors;     /* specifies the transfer type. */
  uint8_t   bReportDescriptorType;    /* Maximum Packet Size this endpoint is capable of sending or receiving */  
  uint16_t  wItemLength;          /* is used to specify the polling interval of certain transfers. */
}
HID_DescTypeDef;


typedef struct 
{
     uint8_t  *buf;
     uint16_t  head;
     uint16_t tail;
     uint16_t size;
     uint8_t  lock;
} FIFO_TypeDef;


/* Structure for HID process */
typedef struct _HID_Process
{
  uint8_t              OutPipe; 
  uint8_t              InPipe; 
  HID_StateTypeDef     state; 
  uint8_t              OutEp;
  uint8_t              InEp;
  HID_CtlStateTypeDef  ctl_state;
  FIFO_TypeDef         fifo; 
  uint8_t              *pData;   
  uint16_t             length;
  uint8_t              ep_addr;
  uint16_t             poll; 
  uint16_t             timer;
  uint8_t              DataReady;
  HID_DescTypeDef      HID_Desc;  
  USBH_StatusTypeDef  ( * Init)(USBH_HandleTypeDef *phost);
}
HID_HandleTypeDef;

/**
  * @}
  */ 

/** @defgroup USBH_HID_CORE_Exported_Defines
  * @{
  */ 

#define USB_HID_GET_REPORT           0x01
#define USB_HID_GET_IDLE             0x02
#define USB_HID_GET_PROTOCOL         0x03
#define USB_HID_SET_REPORT           0x09
#define USB_HID_SET_IDLE             0x0A
#define USB_HID_SET_PROTOCOL         0x0B    




/* HID Class Codes */
#define USB_HID_CLASS                                   0x03

/* Interface Descriptor field values for HID Boot Protocol */
#define HID_BOOT_CODE                                  0x01    
#define HID_KEYBRD_BOOT_CODE                           0x01
#define HID_MOUSE_BOOT_CODE                            0x02


/**
  * @}
  */ 

/** @defgroup USBH_HID_CORE_Exported_Macros
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup USBH_HID_CORE_Exported_Variables
  * @{
  */ 
extern USBH_ClassTypeDef  HID_Class;
#define USBH_HID_CLASS    &HID_Class
/**
  * @}
  */ 

/** @defgroup USBH_HID_CORE_Exported_FunctionsPrototype
  * @{
  */ 

USBH_StatusTypeDef USBH_HID_SetReport (USBH_HandleTypeDef *phost,
                                  uint8_t reportType,
                                  uint8_t reportId,
                                  uint8_t* reportBuff,
                                  uint8_t reportLen);
  
USBH_StatusTypeDef USBH_HID_GetReport (USBH_HandleTypeDef *phost,
                                  uint8_t reportType,
                                  uint8_t reportId,
                                  uint8_t* reportBuff,
                                  uint8_t reportLen);  

USBH_StatusTypeDef USBH_HID_GetHIDReportDescriptor (USBH_HandleTypeDef *phost,  
                                            uint16_t length);

USBH_StatusTypeDef USBH_HID_GetHIDDescriptor (USBH_HandleTypeDef *phost,  
                                            uint16_t length);

USBH_StatusTypeDef USBH_HID_SetIdle (USBH_HandleTypeDef *phost,
                                  uint8_t duration,
                                  uint8_t reportId);

USBH_StatusTypeDef USBH_HID_SetProtocol (USBH_HandleTypeDef *phost,
                                      uint8_t protocol);

void USBH_HID_EventCallback(USBH_HandleTypeDef *phost);

HID_TypeTypeDef USBH_HID_GetDeviceType(USBH_HandleTypeDef *phost);

void fifo_init(FIFO_TypeDef * f, uint8_t * buf, uint16_t size);

uint16_t  fifo_read(FIFO_TypeDef * f, void * buf, uint16_t  nbytes);

uint16_t  fifo_write(FIFO_TypeDef * f, const void * buf, uint16_t  nbytes);

/**
  * @}
  */ 


#endif /* __USBH_HID_H */

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

