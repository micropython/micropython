/**
  ******************************************************************************
  * @file    usbd_dfu_core.h
  * @author  MCD Application Team
  * @version V2.0.0
  * @date    18-February-2014
  * @brief   header file for the usbd_dfu_core.c file.
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

#ifndef __USB_DFU_CORE_H_
#define __USB_DFU_CORE_H_

#include  "usbd_ioreq.h"

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @{
  */
  
/** @defgroup USBD_DFU
  * @brief This file is the Header file for USBD_msc.c
  * @{
  */ 


/** @defgroup USBD_DFU_Exported_Defines
  * @{
  */ 
    
#define USB_DFU_CONFIG_DESC_SIZ       (18 + (9 * USBD_DFU_MAX_ITF_NUM))
#define USB_DFU_DESC_SIZ              9
    
#define DFU_DESCRIPTOR_TYPE           0x21
    

/**************************************************/
/* DFU Requests  DFU states                       */
/**************************************************/
#define APP_STATE_IDLE                 0
#define APP_STATE_DETACH               1
#define DFU_STATE_IDLE                 2
#define DFU_STATE_DNLOAD_SYNC          3
#define DFU_STATE_DNLOAD_BUSY          4
#define DFU_STATE_DNLOAD_IDLE          5
#define DFU_STATE_MANIFEST_SYNC        6
#define DFU_STATE_MANIFEST             7
#define DFU_STATE_MANIFEST_WAIT_RESET  8
#define DFU_STATE_UPLOAD_IDLE          9
#define DFU_STATE_ERROR                10

/**************************************************/
/* DFU errors                                     */
/**************************************************/

#define DFU_ERROR_NONE              0x00
#define DFU_ERROR_TARGET            0x01
#define DFU_ERROR_FILE              0x02
#define DFU_ERROR_WRITE             0x03
#define DFU_ERROR_ERASE             0x04
#define DFU_ERROR_CHECK_ERASED      0x05
#define DFU_ERROR_PROG              0x06
#define DFU_ERROR_VERIFY            0x07
#define DFU_ERROR_ADDRESS           0x08
#define DFU_ERROR_NOTDONE           0x09
#define DFU_ERROR_FIRMWARE          0x0A
#define DFU_ERROR_VENDOR            0x0B
#define DFU_ERROR_USB               0x0C
#define DFU_ERROR_POR               0x0D
#define DFU_ERROR_UNKNOWN           0x0E
#define DFU_ERROR_STALLEDPKT        0x0F

/**************************************************/
/* DFU Manifestation State                        */
/**************************************************/

#define DFU_MANIFEST_COMPLETE       0x00
#define DFU_MANIFEST_IN_PROGRESS    0x01


/**************************************************/
/* Special Commands  with Download Request        */
/**************************************************/

#define DFU_CMD_GETCOMMANDS         0x00
#define DFU_CMD_SETADDRESSPOINTER   0x21
#define DFU_CMD_ERASE               0x41
    
#define DFU_MEDIA_ERASE             0x00
#define DFU_MEDIA_PROGRAM           0x01
/**************************************************/
/* Other defines                                  */
/**************************************************/
/* Bit Detach capable = bit 3 in bmAttributes field */
#define DFU_DETACH_MASK              (uint8_t)(1 << 4) 
#define DFU_STATUS_DEPTH             (6) 
    
typedef enum 
{
  DFU_DETACH = 0,
  DFU_DNLOAD ,
  DFU_UPLOAD,
  DFU_GETSTATUS,
  DFU_CLRSTATUS,
  DFU_GETSTATE,
  DFU_ABORT
} DFU_RequestTypeDef;
  
typedef  void (*pFunction)(void);


/**********  Descriptor of DFU interface 0 Alternate setting n ****************/  
#define USBD_DFU_IF_DESC(n)   0x09,   /* bLength: Interface Descriptor size */ \
                              USB_DESC_TYPE_INTERFACE,   /* bDescriptorType */ \
                              0x00,   /* bInterfaceNumber: Number of Interface */ \
                              (n),      /* bAlternateSetting: Alternate setting */ \
                              0x00,   /* bNumEndpoints*/ \
                              0xFE,   /* bInterfaceClass: Application Specific Class Code */ \
                              0x01,   /* bInterfaceSubClass : Device Firmware Upgrade Code */ \
                              0x02,   /* nInterfaceProtocol: DFU mode protocol */ \
                              USBD_IDX_INTERFACE_STR + (n) + 1 /* iInterface: Index of string descriptor */ \
 
                             /* 18 */
                                
#define TRANSFER_SIZE_BYTES(size)          ((uint8_t)(size)), /* XFERSIZEB0 */\
                                           ((uint8_t)(size >> 8)) /* XFERSIZEB1 */
                                             

#define IS_PROTECTED_AREA(add)    (uint8_t)(((add >= 0x08000000) && (add < (APP_DEFAULT_ADD)))? 1:0)                                             
                                            
/**
  * @}
  */ 


/** @defgroup USBD_CORE_Exported_TypesDefinitions
  * @{
  */

typedef struct
{
  union
  {
    uint32_t d32[USBD_DFU_XFER_SIZE/4];
    uint8_t  d8[USBD_DFU_XFER_SIZE];
  }buffer;
  
  uint8_t              dev_state; 
  uint8_t              dev_status[DFU_STATUS_DEPTH];
  uint8_t              manif_state;    
  
  uint32_t             wblock_num;
  uint32_t             wlength;
  uint32_t             data_ptr; 
  __IO uint32_t        alt_setting;
  
}
USBD_DFU_HandleTypeDef; 


typedef struct
{
  const uint8_t* pStrDesc;
  uint16_t (* Init)     (void);   
  uint16_t (* DeInit)   (void);   
  uint16_t (* Erase)    (uint32_t Add);
  uint16_t (* Write)    (uint8_t *src, uint8_t *dest, uint32_t Len);
  uint8_t* (* Read)     (uint8_t *src, uint8_t *dest, uint32_t Len);
  uint16_t (* GetStatus)(uint32_t Add, uint8_t cmd, uint8_t *buff);  
}
USBD_DFU_MediaTypeDef;
/**
  * @}
  */ 



/** @defgroup USBD_CORE_Exported_Macros
  * @{
  */ 

/**
  * @}
  */ 

/** @defgroup USBD_CORE_Exported_Variables
  * @{
  */ 

extern USBD_ClassTypeDef  USBD_DFU;
/**
  * @}
  */ 

/** @defgroup USB_CORE_Exported_Functions
  * @{
  */ 
uint8_t  USBD_DFU_RegisterMedia    (USBD_HandleTypeDef   *pdev, 
                                    USBD_DFU_MediaTypeDef *fops);
/**
  * @}
  */ 

#endif  // __USB_DFU_CORE_H_
/**
  * @}
  */ 

/**
  * @}
  */ 
  
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
