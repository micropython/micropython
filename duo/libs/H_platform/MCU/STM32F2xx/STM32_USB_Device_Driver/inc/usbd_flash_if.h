/**
  ******************************************************************************
  * @file    usbd_flash_if.h
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-March-2012
  * @brief   Header for usbd_flash_if.c file.
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
#ifndef __FLASH_IF_MAL_H
#define __FLASH_IF_MAL_H

/* Includes ------------------------------------------------------------------*/
#include "usbd_dfu_mal.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define FLASH_START_ADD                  0x08000000

#ifdef STM32F2XX
 #define FLASH_END_ADD                   0x08100000
#if PLATFORM_ID == PLATFORM_DUO_PRODUCTION
 #define FLASH_IF_STRING                 "@Internal Flash   /0x08000000/01*016Ka,02*016Kg,01*016Kg,01*064Kg,07*128Kg"
#else
 #define FLASH_IF_STRING                 "@Internal Flash   /0x08000000/03*016Ka,01*016Kg,01*064Kg,07*128Kg"
#endif
#elif defined(STM32F4XX)
 #define FLASH_END_ADD                   0x08100000
 #define FLASH_IF_STRING                 "@Internal Flash   /0x08000000/03*016Ka,01*016Kg,01*064Kg,07*128Kg"
#elif defined(STM32F10X_CL)
 #define FLASH_END_ADD                   0x08040000
 #define FLASH_IF_STRING                 "@Internal Flash   /0x08000000/06*002Ka,122*002Kg"
#endif /* STM32F2XX */


extern DFU_MAL_Prop_TypeDef DFU_Flash_cb;

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* __FLASH_IF_MAL_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
