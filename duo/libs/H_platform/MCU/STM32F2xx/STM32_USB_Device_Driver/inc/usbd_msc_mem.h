/**
  ******************************************************************************
  * @file    usbd_msc_mem.h
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-March-2012
  * @brief   header for the STORAGE DISK file file
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

#ifndef __USBD_MEM_H
#define __USBD_MEM_H
/* Includes ------------------------------------------------------------------*/
#include "usbd_def.h"


/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @{
  */

/** @defgroup USBD_MEM
  * @brief header file for the storage disk file
  * @{
  */

/** @defgroup USBD_MEM_Exported_Defines
  * @{
  */
#define USBD_STD_INQUIRY_LENGTH		36
/**
  * @}
  */


/** @defgroup USBD_MEM_Exported_TypesDefinitions
  * @{
  */

typedef struct _USBD_STORAGE
{
  int8_t (* Init) (uint8_t lun);
  int8_t (* GetCapacity) (uint8_t lun, uint32_t *block_num, uint32_t *block_size);
  int8_t (* IsReady) (uint8_t lun);
  int8_t (* IsWriteProtected) (uint8_t lun);
  int8_t (* Read) (uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);
  int8_t (* Write)(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);
  int8_t (* GetMaxLun)(void);
  int8_t *pInquiry;

}USBD_STORAGE_cb_TypeDef;
/**
  * @}
  */



/** @defgroup USBD_MEM_Exported_Macros
  * @{
  */

/**
  * @}
  */

/** @defgroup USBD_MEM_Exported_Variables
  * @{
  */

/**
  * @}
  */

/** @defgroup USBD_MEM_Exported_FunctionsPrototype
  * @{
  */
extern USBD_STORAGE_cb_TypeDef *USBD_STORAGE_fops;
/**
  * @}
  */

#endif /* __USBD_MEM_H */
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
