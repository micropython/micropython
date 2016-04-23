/**
  ******************************************************************************
  * @file    usbd_req.h
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-March-2012
  * @brief   header file for the usbd_req.c file
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

#ifndef __USB_REQUEST_H_
#define __USB_REQUEST_H_

/* Includes ------------------------------------------------------------------*/
#include  "usbd_def.h"
#include  "usbd_core.h"
#include  "usbd_conf.h"


/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @{
  */

/** @defgroup USBD_REQ
  * @brief header file for the usbd_ioreq.c file
  * @{
  */

/** @defgroup USBD_REQ_Exported_Defines
  * @{
  */
/**
  * @}
  */


/** @defgroup USBD_REQ_Exported_Types
  * @{
  */
/**
  * @}
  */



/** @defgroup USBD_REQ_Exported_Macros
  * @{
  */
/**
  * @}
  */

/** @defgroup USBD_REQ_Exported_Variables
  * @{
  */
/**
  * @}
  */

/** @defgroup USBD_REQ_Exported_FunctionsPrototype
  * @{
  */

USBD_Status  USBD_StdDevReq (USB_OTG_CORE_HANDLE  *pdev, USB_SETUP_REQ  *req);
USBD_Status  USBD_StdItfReq (USB_OTG_CORE_HANDLE  *pdev, USB_SETUP_REQ  *req);
USBD_Status  USBD_StdEPReq (USB_OTG_CORE_HANDLE  *pdev, USB_SETUP_REQ  *req);
void USBD_ParseSetupRequest( USB_OTG_CORE_HANDLE  *pdev,
                                    USB_SETUP_REQ *req);

void USBD_CtlError( USB_OTG_CORE_HANDLE  *pdev,
                            USB_SETUP_REQ *req);

void USBD_GetString(uint8_t *desc, uint8_t *unicode, uint16_t *len);
/**
  * @}
  */

#endif /* __USB_REQUEST_H_ */

/**
  * @}
  */

/**
* @}
*/


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
