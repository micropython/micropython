/**
  ******************************************************************************
  * @file    usbd_msc_bot.h
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-March-2012
  * @brief   header for the usbd_msc_bot.c file
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

/* Define to prevent recursive inclusion -------------------------------------*/

#include "usbd_core.h"

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBD_MSC_BOT_H
#define __USBD_MSC_BOT_H

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @{
  */
  
/** @defgroup MSC_BOT
  * @brief This file is the Header file for usbd_bot.c
  * @{
  */ 


/** @defgroup USBD_CORE_Exported_Defines
  * @{
  */ 
#define BOT_IDLE                      0       /* Idle state */
#define BOT_DATA_OUT                  1       /* Data Out state */
#define BOT_DATA_IN                   2       /* Data In state */
#define BOT_LAST_DATA_IN              3       /* Last Data In Last */
#define BOT_SEND_DATA                 4       /* Send Immediate data */

#define BOT_CBW_SIGNATURE             0x43425355
#define BOT_CSW_SIGNATURE             0x53425355
#define BOT_CBW_LENGTH                31
#define BOT_CSW_LENGTH                13

/* CSW Status Definitions */
#define CSW_CMD_PASSED                0x00
#define CSW_CMD_FAILED                0x01
#define CSW_PHASE_ERROR               0x02

/* BOT Status */
#define BOT_STATE_NORMAL              0
#define BOT_STATE_RECOVERY            1
#define BOT_STATE_ERROR               2


#define DIR_IN                        0
#define DIR_OUT                       1
#define BOTH_DIR                      2

/**
  * @}
  */ 

/** @defgroup MSC_CORE_Private_TypesDefinitions
  * @{
  */ 

typedef struct _MSC_BOT_CBW
{
  uint32_t dSignature;
  uint32_t dTag;
  uint32_t dDataLength;
  uint8_t  bmFlags;
  uint8_t  bLUN;
  uint8_t  bCBLength;
  uint8_t  CB[16];
}
MSC_BOT_CBW_TypeDef;


typedef struct _MSC_BOT_CSW
{
  uint32_t dSignature;
  uint32_t dTag;
  uint32_t dDataResidue;
  uint8_t  bStatus;
}
MSC_BOT_CSW_TypeDef;

/**
  * @}
  */ 


/** @defgroup USBD_CORE_Exported_Types
  * @{
  */

extern uint8_t              MSC_BOT_Data[];
extern uint16_t             MSC_BOT_DataLen;
extern uint8_t              MSC_BOT_State;
extern uint8_t              MSC_BOT_BurstMode;
extern MSC_BOT_CBW_TypeDef  MSC_BOT_cbw;
extern MSC_BOT_CSW_TypeDef  MSC_BOT_csw;
/**
  * @}
  */ 
/** @defgroup USBD_CORE_Exported_FunctionsPrototypes
  * @{
  */ 
void MSC_BOT_Init (USB_OTG_CORE_HANDLE  *pdev);
void MSC_BOT_Reset (USB_OTG_CORE_HANDLE  *pdev);
void MSC_BOT_DeInit (USB_OTG_CORE_HANDLE  *pdev);
void MSC_BOT_DataIn (USB_OTG_CORE_HANDLE  *pdev, 
                     uint8_t epnum);

void MSC_BOT_DataOut (USB_OTG_CORE_HANDLE  *pdev, 
                      uint8_t epnum);

void MSC_BOT_SendCSW (USB_OTG_CORE_HANDLE  *pdev,
                             uint8_t CSW_Status);

void  MSC_BOT_CplClrFeature (USB_OTG_CORE_HANDLE  *pdev, 
                             uint8_t epnum);
/**
  * @}
  */ 

#endif /* __USBD_MSC_BOT_H */
/**
  * @}
  */ 

/**
* @}
*/ 
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

