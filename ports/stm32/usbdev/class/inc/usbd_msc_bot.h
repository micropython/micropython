/**
  ******************************************************************************
  * @file    usbd_msc_bot.h
  * @author  MCD Application Team
  * @version V2.0.0
  * @date    18-February-2014
  * @brief   header for the usbd_msc_bot.c file
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
#define USBD_BOT_IDLE                      0       /* Idle state */
#define USBD_BOT_DATA_OUT                  1       /* Data Out state */
#define USBD_BOT_DATA_IN                   2       /* Data In state */
#define USBD_BOT_LAST_DATA_IN              3       /* Last Data In Last */
#define USBD_BOT_SEND_DATA                 4       /* Send Immediate data */
#define USBD_BOT_NO_DATA                   5       /* No data Stage */

#define USBD_BOT_CBW_SIGNATURE             0x43425355
#define USBD_BOT_CSW_SIGNATURE             0x53425355
#define USBD_BOT_CBW_LENGTH                31
#define USBD_BOT_CSW_LENGTH                13
#define USBD_BOT_MAX_DATA                  256

/* CSW Status Definitions */
#define USBD_CSW_CMD_PASSED                0x00
#define USBD_CSW_CMD_FAILED                0x01
#define USBD_CSW_PHASE_ERROR               0x02

/* BOT Status */
#define USBD_BOT_STATUS_NORMAL             0
#define USBD_BOT_STATUS_RECOVERY           1
#define USBD_BOT_STATUS_ERROR              2


#define USBD_DIR_IN                        0
#define USBD_DIR_OUT                       1
#define USBD_BOTH_DIR                      2

/**
  * @}
  */

/** @defgroup MSC_CORE_Private_TypesDefinitions
  * @{
  */

typedef struct
{
  uint32_t dSignature;
  uint32_t dTag;
  uint32_t dDataLength;
  uint8_t  bmFlags;
  uint8_t  bLUN;
  uint8_t  bCBLength;
  uint8_t  CB[16];
  uint8_t  ReservedForAlign;
}
USBD_MSC_BOT_CBWTypeDef;


typedef struct
{
  uint32_t dSignature;
  uint32_t dTag;
  uint32_t dDataResidue;
  uint8_t  bStatus;
  uint8_t  ReservedForAlign[3];
}
USBD_MSC_BOT_CSWTypeDef;

/**
  * @}
  */


/** @defgroup USBD_CORE_Exported_Types
  * @{
  */

/**
  * @}
  */
/** @defgroup USBD_CORE_Exported_FunctionsPrototypes
  * @{
  */
void MSC_BOT_Init (USBD_HandleTypeDef  *pdev);
void MSC_BOT_Reset (USBD_HandleTypeDef  *pdev);
void MSC_BOT_DeInit (USBD_HandleTypeDef  *pdev);
void MSC_BOT_DataIn (USBD_HandleTypeDef  *pdev,
                     uint8_t epnum);

void MSC_BOT_DataOut (USBD_HandleTypeDef  *pdev,
                      uint8_t epnum);

void MSC_BOT_SendCSW (USBD_HandleTypeDef  *pdev,
                             uint8_t CSW_Status);

void  MSC_BOT_CplClrFeature (USBD_HandleTypeDef  *pdev,
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

