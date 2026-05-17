/**
  ******************************************************************************
  * @file    usbh_core.h
  * @author  MCD Application Team
  * @version V3.0.0
  * @date    18-February-2014
  * @brief   Header file for usbh_core.c
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
#ifndef __USBH_CORE_H
#define __USBH_CORE_H

/* Includes ------------------------------------------------------------------*/
#include "usbh_conf.h"
#include "usbh_def.h"
#include "usbh_ioreq.h"
#include "usbh_pipes.h"
#include "usbh_ctlreq.h"

/** @addtogroup USBH_LIB
  * @{
  */

/** @addtogroup USBH_LIB_CORE
* @{
*/
  
/** @defgroup USBH_CORE
  * @brief This file is the Header file for usbh_core.c
  * @{
  */ 


/** @defgroup USBH_CORE_Exported_Defines
  * @{
  */ 

/**
  * @}
  */ 
#define HOST_USER_SELECT_CONFIGURATION          1
#define HOST_USER_CLASS_ACTIVE                  2
#define HOST_USER_CLASS_SELECTED                3
#define HOST_USER_CONNECTION                    4
#define HOST_USER_DISCONNECTION                 5



/**
  * @}
  */ 



/** @defgroup USBH_CORE_Exported_Macros
  * @{
  */ 

/**
  * @}
  */ 

/** @defgroup USBH_CORE_Exported_Variables
  * @{
  */ 

/**
  * @}
  */ 

/** @defgroup USBH_CORE_Exported_FunctionsPrototype
  * @{
  */ 

  
USBH_StatusTypeDef  USBH_Init(USBH_HandleTypeDef *phost, void (*pUsrFunc)(USBH_HandleTypeDef *phost, uint8_t ), uint8_t id);
USBH_StatusTypeDef  USBH_DeInit(USBH_HandleTypeDef *phost);
USBH_StatusTypeDef  USBH_RegisterClass(USBH_HandleTypeDef *phost, USBH_ClassTypeDef *pclass);
USBH_StatusTypeDef  USBH_SelectInterface(USBH_HandleTypeDef *phost, uint8_t interface);
uint8_t             USBH_FindInterface(USBH_HandleTypeDef *phost, 
                                            uint8_t Class, 
                                            uint8_t SubClass, 
                                            uint8_t Protocol);
uint8_t             USBH_GetActiveClass(USBH_HandleTypeDef *phost);

uint8_t             USBH_FindInterfaceIndex(USBH_HandleTypeDef *phost, 
                                            uint8_t interface_number, 
                                            uint8_t alt_settings);

USBH_StatusTypeDef  USBH_Start            (USBH_HandleTypeDef *phost); 
USBH_StatusTypeDef  USBH_Stop             (USBH_HandleTypeDef *phost); 
USBH_StatusTypeDef  USBH_Process          (USBH_HandleTypeDef *phost);
USBH_StatusTypeDef  USBH_ReEnumerate      (USBH_HandleTypeDef *phost);

/* USBH Low Level Driver */
USBH_StatusTypeDef   USBH_LL_Init         (USBH_HandleTypeDef *phost);
USBH_StatusTypeDef   USBH_LL_DeInit       (USBH_HandleTypeDef *phost);
USBH_StatusTypeDef   USBH_LL_Start        (USBH_HandleTypeDef *phost);
USBH_StatusTypeDef   USBH_LL_Stop         (USBH_HandleTypeDef *phost);

USBH_StatusTypeDef   USBH_LL_Connect      (USBH_HandleTypeDef *phost);
USBH_StatusTypeDef   USBH_LL_Disconnect   (USBH_HandleTypeDef *phost);
USBH_SpeedTypeDef    USBH_LL_GetSpeed     (USBH_HandleTypeDef *phost);
USBH_StatusTypeDef   USBH_LL_ResetPort    (USBH_HandleTypeDef *phost);
uint32_t             USBH_LL_GetLastXferSize   (USBH_HandleTypeDef *phost, uint8_t ); 
USBH_StatusTypeDef   USBH_LL_DriverVBUS   (USBH_HandleTypeDef *phost, uint8_t );

USBH_StatusTypeDef   USBH_LL_OpenPipe     (USBH_HandleTypeDef *phost, uint8_t, uint8_t, uint8_t, uint8_t , uint8_t, uint16_t ); 
USBH_StatusTypeDef   USBH_LL_ClosePipe    (USBH_HandleTypeDef *phost, uint8_t );   
USBH_StatusTypeDef   USBH_LL_SubmitURB    (USBH_HandleTypeDef *phost, uint8_t, uint8_t,uint8_t,  uint8_t, uint8_t*, uint16_t, uint8_t ); 
USBH_URBStateTypeDef USBH_LL_GetURBState  (USBH_HandleTypeDef *phost, uint8_t ); 
#if (USBH_USE_OS == 1)
USBH_StatusTypeDef  USBH_LL_NotifyURBChange (USBH_HandleTypeDef *phost);
#endif
USBH_StatusTypeDef   USBH_LL_SetToggle    (USBH_HandleTypeDef *phost, uint8_t , uint8_t );
uint8_t              USBH_LL_GetToggle    (USBH_HandleTypeDef *phost, uint8_t );

/* USBH Time base */
void                 USBH_Delay (uint32_t Delay);
void                 USBH_LL_SetTimer     (USBH_HandleTypeDef *phost, uint32_t );  
void                 USBH_LL_IncTimer     (USBH_HandleTypeDef *phost);  
/**
  * @}
  */ 

#endif /* __CORE_H */
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



