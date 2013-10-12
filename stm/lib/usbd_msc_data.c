/**
  ******************************************************************************
  * @file    usbd_msc_data.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-March-2012
  * @brief   This file provides all the vital inquiry pages and sense data.
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
#include "usbd_msc_data.h"


/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @{
  */


/** @defgroup MSC_DATA 
  * @brief Mass storage info/data module
  * @{
  */ 

/** @defgroup MSC_DATA_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup MSC_DATA_Private_Defines
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup MSC_DATA_Private_Macros
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup MSC_DATA_Private_Variables
  * @{
  */ 


/* USB Mass storage Page 0 Inquiry Data */
const uint8_t  MSC_Page00_Inquiry_Data[] = {//7						
	0x00,		
	0x00, 
	0x00, 
	(LENGTH_INQUIRY_PAGE00 - 4),
	0x00, 
	0x80, 
	0x83 
};  
/* USB Mass storage sense 6  Data */
const uint8_t  MSC_Mode_Sense6_data[] = {
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00, 
	0x00,
	0x00
};	
/* USB Mass storage sense 10  Data */
const uint8_t  MSC_Mode_Sense10_data[] = {
	0x00,
	0x06, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00
};
/**
  * @}
  */ 


/** @defgroup MSC_DATA_Private_FunctionPrototypes
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup MSC_DATA_Private_Functions
  * @{
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
