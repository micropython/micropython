/**
  ******************************************************************************
  * @file    usbh_msc_scsi.h
  * @author  MCD Application Team
  * @version V3.0.0
  * @date    18-February-2014
  * @brief   Header file for usbh_msc_scsi.c
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
#ifndef __USBH_MSC_SCSI_H__
#define __USBH_MSC_SCSI_H__

/* Includes ------------------------------------------------------------------*/
#include "usbh_core.h"


/** @addtogroup USBH_LIB
  * @{
  */

/** @addtogroup USBH_CLASS
  * @{
  */

/** @addtogroup USBH_MSC_CLASS
  * @{
  */
  
/** @defgroup USBH_MSC_SCSI
  * @brief This file is the Header file for usbh_msc_scsi.c
  * @{
  */ 
    
    
// Capacity data.
typedef struct
{
  uint32_t block_nbr;   
  uint16_t block_size;   
} SCSI_CapacityTypeDef;


// Sense data.
typedef struct
{
  uint8_t key;   
  uint8_t asc;   
  uint8_t ascq;  
} SCSI_SenseTypeDef;

// INQUIRY data.
typedef struct
{
  uint8_t PeripheralQualifier;
  uint8_t DeviceType;
  uint8_t RemovableMedia;
  uint8_t vendor_id[9];
  uint8_t product_id[17];
  uint8_t revision_id[5];
}SCSI_StdInquiryDataTypeDef;

/** @defgroup USBH_MSC_SCSI_Exported_Defines
  * @{
  */ 
#define OPCODE_TEST_UNIT_READY            0x00
#define OPCODE_READ_CAPACITY10            0x25
#define OPCODE_READ10                     0x28
#define OPCODE_WRITE10                    0x2A
#define OPCODE_REQUEST_SENSE              0x03
#define OPCODE_INQUIRY                    0x12    

#define DATA_LEN_MODE_TEST_UNIT_READY        0
#define DATA_LEN_READ_CAPACITY10             8
#define DATA_LEN_INQUIRY                    36  
#define DATA_LEN_REQUEST_SENSE              14       

#define CBW_CB_LENGTH                       16
#define CBW_LENGTH                          10    

/** @defgroup USBH_MSC_SCSI_Exported_Defines
  * @{
  */ 
#define SCSI_SENSE_KEY_NO_SENSE                          0x00
#define SCSI_SENSE_KEY_RECOVERED_ERROR                   0x01
#define SCSI_SENSE_KEY_NOT_READY                         0x02
#define SCSI_SENSE_KEY_MEDIUM_ERROR                      0x03
#define SCSI_SENSE_KEY_HARDWARE_ERROR                    0x04
#define SCSI_SENSE_KEY_ILLEGAL_REQUEST                   0x05
#define SCSI_SENSE_KEY_UNIT_ATTENTION                    0x06
#define SCSI_SENSE_KEY_DATA_PROTECT                      0x07
#define SCSI_SENSE_KEY_BLANK_CHECK                       0x08
#define SCSI_SENSE_KEY_VENDOR_SPECIFIC                   0x09
#define SCSI_SENSE_KEY_COPY_ABORTED                      0x0A
#define SCSI_SENSE_KEY_ABORTED_COMMAND                   0x0B
#define SCSI_SENSE_KEY_VOLUME_OVERFLOW                   0x0D
#define SCSI_SENSE_KEY_MISCOMPARE                        0x0E
/**
  * @}
  */ 
    
    
/** @defgroup USBH_MSC_SCSI_Exported_Defines
  * @{
  */     
#define SCSI_ASC_NO_ADDITIONAL_SENSE_INFORMATION         0x00
#define SCSI_ASC_LOGICAL_UNIT_NOT_READY                  0x04
#define SCSI_ASC_INVALID_FIELD_IN_CDB                    0x24
#define SCSI_ASC_WRITE_PROTECTED                         0x27
#define SCSI_ASC_FORMAT_ERROR                            0x31
#define SCSI_ASC_INVALID_COMMAND_OPERATION_CODE          0x20
#define SCSI_ASC_NOT_READY_TO_READY_CHANGE               0x28
#define SCSI_ASC_MEDIUM_NOT_PRESENT                      0x3A
/**
  * @}
  */ 
    
    
/** @defgroup USBH_MSC_SCSI_Exported_Defines
  * @{
  */     
#define SCSI_ASCQ_FORMAT_COMMAND_FAILED                  0x01
#define SCSI_ASCQ_INITIALIZING_COMMAND_REQUIRED          0x02
#define SCSI_ASCQ_OPERATION_IN_PROGRESS                  0x07
    
/**
  * @}
  */ 

/** @defgroup USBH_MSC_SCSI_Exported_Macros
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup _Exported_Variables
  * @{
  */ 

/**
  * @}
  */ 

/** @defgroup USBH_MSC_SCSI_Exported_FunctionsPrototype
  * @{
  */ 
USBH_StatusTypeDef USBH_MSC_SCSI_TestUnitReady (USBH_HandleTypeDef *phost, 
                                                uint8_t lun);

USBH_StatusTypeDef USBH_MSC_SCSI_ReadCapacity (USBH_HandleTypeDef *phost, 
                                               uint8_t lun, 
                                               SCSI_CapacityTypeDef *capacity);

USBH_StatusTypeDef USBH_MSC_SCSI_Inquiry (USBH_HandleTypeDef *phost, 
                                               uint8_t lun, 
                                               SCSI_StdInquiryDataTypeDef *inquiry);

USBH_StatusTypeDef USBH_MSC_SCSI_RequestSense (USBH_HandleTypeDef *phost, 
                                               uint8_t lun, 
                                               SCSI_SenseTypeDef *sense_data);

USBH_StatusTypeDef USBH_MSC_SCSI_Write(USBH_HandleTypeDef *phost,
                                     uint8_t lun,
                                     uint32_t address,
                                     uint8_t *pbuf,
                                     uint32_t length);

USBH_StatusTypeDef USBH_MSC_SCSI_Read(USBH_HandleTypeDef *phost,
                                     uint8_t lun,
                                     uint32_t address,
                                     uint8_t *pbuf,
                                     uint32_t length);


/**
  * @}
  */ 

#endif  //__USBH_MSC_SCSI_H__


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

