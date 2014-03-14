/**
  ******************************************************************************
  * @file    usbd_storage_template.c
  * @author  MCD Application Team
  * @version V2.0.0
  * @date    18-February-2014
  * @brief   Memory management layer
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
#include "usbd_storage.h"


/** @addtogroup STM32_USBD_DEVICE_LIBRARY
* @{
*/


/** @defgroup USBD_STORAGE 
* @brief usbd core module
* @{
*/ 

/** @defgroup USBD_STORAGE_Private_TypesDefinitions
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBD_STORAGE_Private_Defines
* @{
*/ 
#define STORAGE_LUN_NBR                  1  
#define STORAGE_BLK_NBR                  0x10000  
#define STORAGE_BLK_SIZ                  0x200
/**
* @}
*/ 


/** @defgroup USBD_STORAGE_Private_Macros
* @{
*/ 
/**
* @}
*/ 




/** @defgroup USBD_STORAGE_Private_FunctionPrototypes
* @{
*/ 

/**
* @}
*/ 


/** @defgroup USBD_STORAGE_Private_Functions
* @{
*/ 

int8_t STORAGE_Init (uint8_t lun);

int8_t STORAGE_GetCapacity (uint8_t lun, 
                           uint32_t *block_num, 
                           uint16_t *block_size);

int8_t  STORAGE_IsReady (uint8_t lun);

int8_t  STORAGE_IsWriteProtected (uint8_t lun);

int8_t STORAGE_Read (uint8_t lun, 
                        uint8_t *buf, 
                        uint32_t blk_addr,
                        uint16_t blk_len);

int8_t STORAGE_Write (uint8_t lun, 
                        uint8_t *buf, 
                        uint32_t blk_addr,
                        uint16_t blk_len);

int8_t STORAGE_GetMaxLun (void);

/** @defgroup USBD_STORAGE_Private_Variables
* @{
*/ 

/* USB Mass storage Standard Inquiry Data */
int8_t  STORAGE_Inquirydata[] = {//36
  
  /* LUN 0 */
  0x00,		
  0x80,		
  0x02,		
  0x02,
  (STANDARD_INQUIRY_DATA_LEN - 5),
  0x00,
  0x00,	
  0x00,
  'S', 'T', 'M', ' ', ' ', ' ', ' ', ' ', /* Manufacturer : 8 bytes */
  'P', 'r', 'o', 'd', 'u', 'c', 't', ' ', /* Product      : 16 Bytes */
  ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
  '0', '.', '0' ,'1',                     /* Version      : 4 Bytes */
}; 

USBD_StorageTypeDef USBD_MSD_fops =
{
  STORAGE_Init,
  STORAGE_GetCapacity,
  STORAGE_IsReady,
  STORAGE_IsWriteProtected,
  STORAGE_Read,
  STORAGE_Write,
  STORAGE_GetMaxLun,
  STORAGE_Inquirydata,
  
};
/**
  * @}
  */ 

/** @defgroup USBD_CORE_Private_Functions
  * @{
  */ 

/**
  * @brief  STORAGE_Init
  *         Initailizes the storage unit (medium)
  * @param  lun: logical unit number
  * @retval status (0 : Ok / -1 : Error)
  */

int8_t STORAGE_Init (uint8_t lun)
{
  return (0);
}

/**
  * @brief  STORAGE_GetCapacity
  *         return medium capacity
  * @param  lun: logical unit number
  * @param  block_num: number of total block number
  * @param  block_size: block size
  * @retval status (0 : Ok / -1 : Error)
  */
int8_t STORAGE_GetCapacity (uint8_t lun, uint32_t *block_num, uint16_t *block_size)
{
  
  *block_num  = STORAGE_BLK_NBR - 1;
  *block_size = STORAGE_BLK_SIZ;
  
  return (0);
}

/**
  * @brief  STORAGE_IsReady
  *         check whether the medium is ready
  * @param  lun: logical unit number
  * @retval status (0 : Ok / -1 : Error)
  */
int8_t  STORAGE_IsReady (uint8_t lun)
{

    return (0);
}

/**
  * @brief  STORAGE_IsWriteProtected
  *         check whether the medium is write protected
  * @param  lun: logical unit number
  * @retval status (0 : write enabled / -1 : otherwise)
  */
int8_t  STORAGE_IsWriteProtected (uint8_t lun)
{
  return  0;
}

/**
  * @brief  STORAGE_Read
  *         Read data from the medium
  * @param  lun: logical unit number
  * @param  blk_addr: logical block address
  * @param  blk_len: blocks number
  * @retval status (0 : Ok / -1 : Error)
  */
int8_t STORAGE_Read (uint8_t lun, 
                 uint8_t *buf, 
                 uint32_t blk_addr,                       
                 uint16_t blk_len)
{
  return (0);
}

/**
  * @brief  STORAGE_Write
  *         Write data into the medium
  * @param  lun: logical unit number
  * @param  blk_addr: logical block address
  * @param  blk_len: blocks number
  * @retval status (0 : Ok / -1 : Error)
  */
int8_t STORAGE_Write (uint8_t lun, 
                  uint8_t *buf, 
                  uint32_t blk_addr,
                  uint16_t blk_len)
{
  return (0);
}


/**
  * @brief  STORAGE_GetMaxLun
  *         return the Max Supported LUNs
  * @param  none
  * @retval lun(s) number
  */
int8_t STORAGE_GetMaxLun (void)
{
  return (STORAGE_LUN_NBR - 1);
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

