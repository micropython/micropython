/**
  ******************************************************************************
  * @file    usbd_cdc_if_template.c
  * @author  MCD Application Team
  * @version V2.0.0
  * @date    18-February-2014
  * @brief   Generic media access Layer.
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
#include "usbd_audio_if_template.h"

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @{
  */


/** @defgroup USBD_AUDIO 
  * @brief usbd core module
  * @{
  */ 

/** @defgroup USBD_AUDIO_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBD_AUDIO_Private_Defines
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBD_AUDIO_Private_Macros
  * @{
  */ 

/**
  * @}
  */ 


/** @defgroup USBD_AUDIO_Private_FunctionPrototypes
  * @{
  */

static int8_t  TEMPLATE_Init         (uint32_t  AudioFreq, uint32_t Volume, uint32_t options);
static int8_t  TEMPLATE_DeInit       (uint32_t options);
static int8_t  TEMPLATE_AudioCmd     (uint8_t* pbuf, uint32_t size, uint8_t cmd);
static int8_t  TEMPLATE_VolumeCtl    (uint8_t vol);
static int8_t  TEMPLATE_MuteCtl      (uint8_t cmd);
static int8_t  TEMPLATE_PeriodicTC   (uint8_t cmd);
static int8_t  TEMPLATE_GetState     (void);

USBD_AUDIO_ItfTypeDef USBD_AUDIO_Template_fops = 
{
  TEMPLATE_Init,
  TEMPLATE_DeInit,
  TEMPLATE_AudioCmd,
  TEMPLATE_VolumeCtl,
  TEMPLATE_MuteCtl,
  TEMPLATE_PeriodicTC,
  TEMPLATE_GetState,
};

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  TEMPLATE_Init
  *         Initializes the AUDIO media low layer
  * @param  None
  * @retval Result of the opeartion: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t TEMPLATE_Init(uint32_t  AudioFreq, uint32_t Volume, uint32_t options)
{
  /*
     Add your initialization code here 
  */  
  return (0);
}

/**
  * @brief  TEMPLATE_DeInit
  *         DeInitializes the AUDIO media low layer
  * @param  None
  * @retval Result of the opeartion: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t TEMPLATE_DeInit(uint32_t options)
{
  /*
     Add your deinitialization code here 
  */  
  return (0);
}


/**
  * @brief  TEMPLATE_AudioCmd
  *         AUDIO command handler 
  * @param  Buf: Buffer of data to be sent
  * @param  size: Number of data to be sent (in bytes)
  * @param  cmd: command opcode
  * @retval Result of the opeartion: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t TEMPLATE_AudioCmd (uint8_t* pbuf, uint32_t size, uint8_t cmd)
{

  return (0);
}

/**
  * @brief  TEMPLATE_VolumeCtl              
  * @param  vol: volume level (0..100)
  * @retval Result of the opeartion: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t TEMPLATE_VolumeCtl (uint8_t vol)
{
 
  return (0);
}

/**
  * @brief  TEMPLATE_MuteCtl              
  * @param  cmd: vmute command
  * @retval Result of the opeartion: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t TEMPLATE_MuteCtl (uint8_t cmd)
{
 
  return (0);
}

/**
  * @brief  TEMPLATE_PeriodicTC              
  * @param  cmd
  * @retval Result of the opeartion: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t TEMPLATE_PeriodicTC (uint8_t cmd)
{
 
  return (0);
}

/**
  * @brief  TEMPLATE_GetState              
  * @param  None
  * @retval Result of the opeartion: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t TEMPLATE_GetState (void)
{
 
  return (0);
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

