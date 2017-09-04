/**
  ******************************************************************************
  * @file    usbh_PIPES.h
  * @author  MCD Application Team
  * @version V3.0.0
  * @date    18-February-2014
  * @brief   Header file for usbh_pipes.c
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
#ifndef __USBH_PIPES_H
#define __USBH_PIPES_H

/* Includes ------------------------------------------------------------------*/
#include "usbh_core.h"

/** @addtogroup USBH_LIB
  * @{
  */

/** @addtogroup USBH_LIB_CORE
* @{
*/
  
/** @defgroup USBH_PIPES
  * @brief This file is the header file for usbh_PIPES.c
  * @{
  */ 

/** @defgroup USBH_PIPES_Exported_Defines
  * @{
  */
/**
  * @}
  */ 

/** @defgroup USBH_PIPES_Exported_Types
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBH_PIPES_Exported_Macros
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup USBH_PIPES_Exported_Variables
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup USBH_PIPES_Exported_FunctionsPrototype
  * @{
  */

USBH_StatusTypeDef USBH_OpenPipe  (USBH_HandleTypeDef *phost,
                            uint8_t ch_num,
                            uint8_t epnum,                            
                            uint8_t dev_address,
                            uint8_t speed,
                            uint8_t ep_type,
                            uint16_t mps);

USBH_StatusTypeDef USBH_ClosePipe  (USBH_HandleTypeDef *phost,
                            uint8_t pipe_num);

uint8_t USBH_AllocPipe  (USBH_HandleTypeDef *phost, 
                         uint8_t ep_addr);

USBH_StatusTypeDef USBH_FreePipe  (USBH_HandleTypeDef *phost, 
                                   uint8_t idx);




/**
  * @}
  */ 



#endif /* __USBH_PIPES_H */


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


