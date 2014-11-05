/**
  ******************************************************************************
  * @file    usbd_dfu_media_template.c
  * @author  MCD Application Team
  * @version V2.0.0
  * @date    18-February-2014
  * @brief   Memory management layer
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
#include "usbd_dfu_media_template.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Extern function prototypes ------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
uint16_t MEM_If_Init(void);
uint16_t MEM_If_Erase (uint32_t Add);
uint16_t MEM_If_Write (uint8_t *src, uint8_t *dest, uint32_t Len);
uint8_t *MEM_If_Read  (uint8_t *src, uint8_t *dest, uint32_t Len);
uint16_t MEM_If_DeInit(void);
uint16_t MEM_If_GetStatus (uint32_t Add, uint8_t Cmd, uint8_t *buffer);

USBD_DFU_MediaTypeDef USBD_DFU_MEDIA_Template_fops =
{
    (uint8_t *)"DFU MEDIA",
    MEM_If_Init,
    MEM_If_DeInit,
    MEM_If_Erase,
    MEM_If_Write,
    MEM_If_Read,
    MEM_If_GetStatus,
  
};
/**
  * @brief  MEM_If_Init
  *         Memory initialization routine.
  * @param  None
  * @retval 0 if operation is successeful, MAL_FAIL else.
  */
uint16_t MEM_If_Init(void)
{ 
  return 0;
}

/**
  * @brief  MEM_If_DeInit
  *         Memory deinitialization routine.
  * @param  None
  * @retval 0 if operation is successeful, MAL_FAIL else.
  */
uint16_t MEM_If_DeInit(void)
{ 
  return 0;
}

/**
  * @brief  MEM_If_Erase
  *         Erase sector.
  * @param  Add: Address of sector to be erased.
  * @retval 0 if operation is successeful, MAL_FAIL else.
  */
uint16_t MEM_If_Erase(uint32_t Add)
{
  return 0;
}

/**
  * @brief  MEM_If_Write
  *         Memory write routine.
  * @param  Add: Address to be written to.
  * @param  Len: Number of data to be written (in bytes).
  * @retval 0 if operation is successeful, MAL_FAIL else.
  */
uint16_t MEM_If_Write(uint8_t *src, uint8_t *dest, uint32_t Len)
{
  return 0;
}

/**
  * @brief  MEM_If_Read
  *         Memory read routine.
  * @param  Add: Address to be read from.
  * @param  Len: Number of data to be read (in bytes).
  * @retval Pointer to the phyisical address where data should be read.
  */
uint8_t *MEM_If_Read (uint8_t *src, uint8_t *dest, uint32_t Len)
{
  /* Return a valid address to avoid HardFault */
  return  (uint8_t*)(0); 
}

/**
  * @brief  Flash_If_GetStatus
  *         Memory read routine.
  * @param  Add: Address to be read from.
  * @param  cmd: Number of data to be read (in bytes).
  * @retval Pointer to the phyisical address where data should be read.
  */
uint16_t MEM_If_GetStatus (uint32_t Add, uint8_t Cmd, uint8_t *buffer)
{
  switch (Cmd)
  {
  case DFU_MEDIA_PROGRAM:

    break;
    
  case DFU_MEDIA_ERASE:
  default:

    break;
  }                             
  return  (0); 
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

