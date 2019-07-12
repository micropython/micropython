/**
  ******************************************************************************
  * @file    usbh_hid_parser.c
  * @author  MCD Application Team
  * @version V3.0.0
  * @date    18-February-2014
  * @brief   This file is the HID Layer Handlers for USB Host HID class.
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
#include "usbh_hid_parser.h"


/** @addtogroup USBH_LIB
  * @{
  */

/** @addtogroup USBH_CLASS
  * @{
  */

/** @addtogroup USBH_HID_CLASS
  * @{
  */
  
/** @defgroup USBH_HID_PARSER 
  * @brief    This file includes HID parsers for USB Host HID class.
  * @{
  */ 

/** @defgroup USBH_HID_PARSER_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBH_HID_PARSER_Private_Defines
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBH_HID_PARSER_Private_Macros
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup USBH_HID_PARSER_Private_FunctionPrototypes
  * @{
  */ 

/**
  * @}
  */ 


/** @defgroup USBH_HID_PARSER_Private_Variables
  * @{
  */

/**
  * @}
  */ 


/** @defgroup USBH_HID_PARSER_Private_Functions
  * @{
  */ 

/**
  * @brief  HID_ReadItem 
  *         The function read a report item.
  * @param  ri: report item
  * @param  ndx: report index
* @retval status (0 : fail / otherwise: item value)
  */
uint32_t HID_ReadItem(HID_Report_ItemTypedef *ri, uint8_t ndx)
{
  uint32_t val=0;
  uint32_t x=0;
  uint32_t bofs;
  uint8_t *data=ri->data;
  uint8_t shift=ri->shift;
  
  /* get the logical value of the item */
  
  /* if this is an array, wee may need to offset ri->data.*/
  if (ri->count > 0)
  { 
    /* If app tries to read outside of the array. */
    if (ri->count <= ndx)
    {
      return(0);
    }
    
    /* calculate bit offset */
    bofs = ndx*ri->size;
    bofs += shift;
    /* calculate byte offset + shift pair from bit offset. */    
    data+=bofs/8;
    shift=(uint8_t)(bofs%8);
  }
  /* read data bytes in little endian order */
  for(x=0; x < ((ri->size & 0x7) ? (ri->size/8)+1 : (ri->size/8)); x++)
  {
    val=(uint32_t)(*data << (x*8));
  }    
  val=(val >> shift) & ((1<<ri->size)-1);
  
  if (val < ri->logical_min || val > ri->logical_max)
  {
    return(0);
  }
  
  /* convert logical value to physical value */
  /* See if the number is negative or not. */
  if ((ri->sign) && (val & (1<<(ri->size-1))))
  {
    /* yes, so sign extend value to 32 bits. */
    int vs=(int)((-1 & ~((1<<(ri->size))-1)) | val);
    
    if(ri->resolution == 1)
    {
      return((uint32_t)vs);
    }
    return((uint32_t)(vs*ri->resolution));
  }
  else
  {
    if(ri->resolution == 1)
    {
      return(val);
    }
    return(val*ri->resolution);    
  }  
}

/**
  * @brief  HID_WriteItem 
  *         The function write a report item.
  * @param  ri: report item
  * @param  ndx: report index
  * @retval status (1: fail/ 0 : Ok)
  */
uint32_t HID_WriteItem(HID_Report_ItemTypedef *ri, uint32_t value, uint8_t ndx)
{
  uint32_t x;
  uint32_t mask;
  uint32_t bofs;
  uint8_t *data=ri->data;
  uint8_t shift=ri->shift;
  
  if (value < ri->physical_min || value > ri->physical_max)  
  {
    return(1);
  }
  
    /* if this is an array, wee may need to offset ri->data.*/
  if (ri->count > 0)
  { 
    /* If app tries to read outside of the array. */
    if (ri->count >= ndx)
    {
      return(0);
    }
    /* calculate bit offset */
    bofs = ndx*ri->size;
    bofs += shift;
    /* calculate byte offset + shift pair from bit offset. */    
    data+=bofs/8;
    shift=(uint8_t)(bofs%8);

  }

  /* Convert physical value to logical value. */  
  if (ri->resolution != 1)
  {
    value=value/ri->resolution;
  }

  /* Write logical value to report in little endian order. */
  mask=(uint32_t)((1<<ri->size)-1);
  value = (value & mask) << shift;
  
  for(x=0; x < ((ri->size & 0x7) ? (ri->size/8)+1 : (ri->size/8)); x++)
  {
    *(ri->data+x)=(uint8_t)((*(ri->data+x) & ~(mask>>(x*8))) | ((value>>(x*8)) & (mask>>(x*8))));
  }
  return(0);
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
