/**
  ******************************************************************************
  * @file    usb_otg.h
  * @author  MCD Application Team
  * @version V2.1.0
  * @date    19-March-2012
  * @brief   OTG Core Header
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
#ifndef __USB_OTG__
#define __USB_OTG__


/** @addtogroup USB_OTG_DRIVER
  * @{
  */

/** @defgroup USB_OTG
  * @brief This file is the
  * @{
  */


/** @defgroup USB_OTG_Exported_Defines
  * @{
  */


void USB_OTG_InitiateSRP(void);
void USB_OTG_InitiateHNP(uint8_t state , uint8_t mode);
void USB_OTG_Switchback (USB_OTG_CORE_DEVICE *pdev);
uint32_t  USB_OTG_GetCurrentState (USB_OTG_CORE_DEVICE *pdev);

/**
  * @}
  */


/** @defgroup USB_OTG_Exported_Types
  * @{
  */
/**
  * @}
  */


/** @defgroup USB_OTG_Exported_Macros
  * @{
  */
/**
  * @}
  */

/** @defgroup USB_OTG_Exported_Variables
  * @{
  */
/**
  * @}
  */

/** @defgroup USB_OTG_Exported_FunctionsPrototype
  * @{
  */
/**
  * @}
  */


#endif //__USB_OTG__


/**
  * @}
  */

/**
  * @}
  */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

