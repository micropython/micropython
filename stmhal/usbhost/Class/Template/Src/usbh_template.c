/**
  ******************************************************************************
  * @file    usbh_mtp.c
  * @author  MCD Application Team
  * @version V3.0.0
  * @date    18-February-2014
  * @brief   This file is the MTP Layer Handlers for USB Host MTP class.
  *
  *
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
#include "usbh_template.h"

/** @addtogroup USBH_LIB
* @{
*/

/** @addtogroup USBH_CLASS
* @{
*/

/** @addtogroup USBH_TEMPLATE_CLASS
* @{
*/

/** @defgroup USBH_TEMPLATE_CORE 
* @brief    This file includes TEMPLATE Layer Handlers for USB Host TEMPLATE class.
* @{
*/ 

/** @defgroup USBH_TEMPLATE_CORE_Private_TypesDefinitions
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBH_TEMPLATE_CORE_Private_Defines
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBH_TEMPLATE_CORE_Private_Macros
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBH_TEMPLATE_CORE_Private_Variables
* @{
*/
/**
* @}
*/ 


/** @defgroup USBH_TEMPLATE_CORE_Private_FunctionPrototypes
* @{
*/ 

static USBH_StatusTypeDef USBH_TEMPLATE_InterfaceInit  (USBH_HandleTypeDef *phost);

static USBH_StatusTypeDef USBH_TEMPLATE_InterfaceDeInit  (USBH_HandleTypeDef *phost);

static USBH_StatusTypeDef USBH_TEMPLATE_Process(USBH_HandleTypeDef *phost);

static USBH_StatusTypeDef USBH_TEMPLATE_ClassRequest (USBH_HandleTypeDef *phost);


USBH_ClassTypeDef  TEMPLATE_Class = 
{
  "TEMPLATE",
  USB_TEMPLATE_CLASS,
  USBH_TEMPLATE_InterfaceInit,
  USBH_TEMPLATE_InterfaceDeInit,
  USBH_TEMPLATE_ClassRequest,
  USBH_TEMPLATE_Process
};
/**
* @}
*/ 


/** @defgroup USBH_TEMPLATE_CORE_Private_Functions
* @{
*/ 

/**
  * @brief  USBH_TEMPLATE_InterfaceInit 
  *         The function init the TEMPLATE class.
  * @param  phost: Host handle
  * @retval USBH Status
  */
static USBH_StatusTypeDef USBH_TEMPLATE_InterfaceInit (USBH_HandleTypeDef *phost)
{	

  return USBH_OK;
}



/**
  * @brief  USBH_TEMPLATE_InterfaceDeInit 
  *         The function DeInit the Pipes used for the TEMPLATE class.
  * @param  phost: Host handle
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_TEMPLATE_InterfaceDeInit (USBH_HandleTypeDef *phost)
{

  return USBH_OK;
}

/**
  * @brief  USBH_TEMPLATE_ClassRequest 
  *         The function is responsible for handling Standard requests
  *         for TEMPLATE class.
  * @param  phost: Host handle
  * @retval USBH Status
  */
static USBH_StatusTypeDef USBH_TEMPLATE_ClassRequest (USBH_HandleTypeDef *phost)
{   

  return USBH_OK; 
}


/**
  * @brief  USBH_TEMPLATE_Process 
  *         The function is for managing state machine for TEMPLATE data transfers 
  * @param  phost: Host handle
  * @retval USBH Status
  */
static USBH_StatusTypeDef USBH_TEMPLATE_Process (USBH_HandleTypeDef *phost)
{
 
  return USBH_OK;
}


/**
  * @brief  USBH_TEMPLATE_Init 
  *         The function Initialize the TEMPLATE function
  * @param  phost: Host handle
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_TEMPLATE_Init (USBH_HandleTypeDef *phost)
{
  USBH_StatusTypeDef Status = USBH_BUSY;
#if (USBH_USE_OS == 1)
  osEvent event;
  
  event = osMessageGet( phost->class_ready_event, osWaitForever );
  
  if( event.status == osEventMessage )      
  {
    if(event.value.v == USBH_CLASS_EVENT)
    {
#else 
      
  while ((Status == USBH_BUSY) || (Status == USBH_FAIL))
  {
    /* Host background process */
    USBH_Process(phost);
    if(phost->gState == HOST_CLASS)
    {
#endif        
      Status = USBH_OK;
    }
  }
  return Status;   
}

/**
  * @brief  USBH_TEMPLATE_IOProcess 
  *         TEMPLATE TEMPLATE process
  * @param  phost: Host handle
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_TEMPLATE_IOProcess (USBH_HandleTypeDef *phost)
{
  if (phost->device.is_connected == 1)
  {
    if(phost->gState == HOST_CLASS)
    {
      USBH_TEMPLATE_Process(phost);
    }
  }
  
  return USBH_OK;
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


/**
* @}
*/


/**
* @}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
