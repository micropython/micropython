/**
  ******************************************************************************
  * @file    sd_diskio.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    18-February-2014
  * @brief   SD Disk I/O driver
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
#include <string.h>
#include "ff_gen_drv.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Block Size in Bytes */
#define BLOCK_SIZE                512

/* Private variables ---------------------------------------------------------*/
/* Disk status */
static volatile DSTATUS Stat = STA_NOINIT;

/* Private function prototypes -----------------------------------------------*/
DSTATUS SD_initialize (void);
DSTATUS SD_status (void);
DRESULT SD_read (BYTE*, DWORD, BYTE);
#if _USE_WRITE == 1
  DRESULT SD_write (const BYTE*, DWORD, BYTE);
#endif /* _USE_WRITE == 1 */
#if _USE_IOCTL == 1
  DRESULT SD_ioctl (BYTE, void*);
#endif  /* _USE_IOCTL == 1 */
  
Diskio_drvTypeDef  SD_Driver =
{
  SD_initialize,
  SD_status,
  SD_read, 
#if  _USE_WRITE == 1
  SD_write,
#endif /* _USE_WRITE == 1 */
  
#if  _USE_IOCTL == 1
  SD_ioctl,
#endif /* _USE_IOCTL == 1 */
};

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initializes a Drive
  * @param  None
  * @retval DSTATUS: Operation status
  */
DSTATUS SD_initialize(void)
{
  Stat = STA_NOINIT;
  
  /* Configure the uSD device */
  if(BSP_SD_Init() == MSD_OK)
  {
    Stat &= ~STA_NOINIT;
  }

  return Stat;
}

/**
  * @brief  Gets Disk Status
  * @param  None
  * @retval DSTATUS: Operation status
  */
DSTATUS SD_status(void)
{
  Stat = STA_NOINIT;

  if(BSP_SD_GetStatus() == 0)
  {
    Stat &= ~STA_NOINIT;
  }
  
  return Stat;
}

/**
  * @brief  Reads Sector(s)
  * @param  *buff: Data buffer to store read data
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to read (1..128)
  * @retval DRESULT: Operation result
  */
DRESULT SD_read(BYTE *buff, DWORD sector, BYTE count)
{
  uint32_t timeout = 100000;
  DWORD scratch [BLOCK_SIZE / 4];  /* Alignment ensured, need enough stack */
  uint8_t SD_state = MSD_OK;
    
  if ((DWORD)buff & 3) /* DMA Alignment issue, do single up to aligned buffer */
  {
    while (count--)
    {
      SD_state = BSP_SD_ReadBlocks_DMA((uint32_t*)scratch, (uint64_t) ((sector + count) * BLOCK_SIZE), BLOCK_SIZE, 1);
      
      while(BSP_SD_GetStatus() != SD_TRANSFER_OK)
      {
        if (timeout-- == 0)
        {
          return RES_ERROR;
        }
      }
      memcpy (&buff[count * BLOCK_SIZE] ,scratch, BLOCK_SIZE);
    }
  }
  else
  {
    SD_state = BSP_SD_ReadBlocks_DMA((uint32_t*)buff, (uint64_t) (sector * BLOCK_SIZE), BLOCK_SIZE, count);
    
    while(BSP_SD_GetStatus() != SD_TRANSFER_OK)
    {  
      if (timeout-- == 0)
      {
        return RES_ERROR;
      }
    }
  }
  if (SD_state == MSD_OK)
  {
    return RES_OK;
  }
  
  return RES_ERROR;
}

/**
  * @brief  Writes Sector(s)
  * @param  *buff: Data to be written
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to write (1..128)
  * @retval DRESULT: Operation result
  */
#if _USE_WRITE == 1
DRESULT SD_write(const BYTE *buff, DWORD sector, BYTE count)
{
  uint32_t timeout = 100000;
  DWORD scratch [BLOCK_SIZE / 4];  /* Alignment ensured, need enough stack */
  uint8_t SD_state = MSD_OK;
  
  if ((DWORD)buff & 3) /* DMA Alignment issue, do single up to aligned buffer */
  {
    while (count--)
    {
      memcpy (scratch, &buff[count * BLOCK_SIZE], BLOCK_SIZE);  
      
      SD_state = BSP_SD_WriteBlocks_DMA((uint32_t*)scratch, (uint64_t)((sector + count) * BLOCK_SIZE), BLOCK_SIZE, 1);
      
      while(BSP_SD_GetStatus() != SD_TRANSFER_OK)
      {
        if (timeout-- == 0)
        {
          return RES_ERROR;
        }
      }
    }
  }
  else
  {
    SD_state = BSP_SD_WriteBlocks_DMA((uint32_t*)buff, (uint64_t)(sector * BLOCK_SIZE), BLOCK_SIZE, count);
    
    while(BSP_SD_GetStatus() != SD_TRANSFER_OK)
    {
      if (timeout-- == 0)
      {
        return RES_ERROR;
      }
    }
  }
  if (SD_state == MSD_OK)
  {
    return RES_OK;
  }
  
  return RES_ERROR;
}
#endif /* _USE_WRITE == 1 */

/**
  * @brief  I/O control operation
  * @param  cmd: Control code
  * @param  *buff: Buffer to send/receive control data
  * @retval DRESULT: Operation result
  */
#if _USE_IOCTL == 1
DRESULT SD_ioctl(BYTE cmd, void *buff)
{
  DRESULT res = RES_ERROR;
  HAL_SD_CardInfoTypedef CardInfo;
  
  if (Stat & STA_NOINIT) return RES_NOTRDY;
  
  switch (cmd)
  {
  /* Make sure that no pending write process */
  case CTRL_SYNC :
    res = RES_OK;
    break;
  
  /* Get number of sectors on the disk (DWORD) */
  case GET_SECTOR_COUNT :
    BSP_SD_GetCardInfo(&CardInfo);
    *(DWORD*)buff = CardInfo.CardCapacity / BLOCK_SIZE;
    res = RES_OK;
    break;
  
  /* Get R/W sector size (WORD) */
  case GET_SECTOR_SIZE :
    *(WORD*)buff = BLOCK_SIZE;
    res = RES_OK;
    break;
  
  /* Get erase block size in unit of sector (DWORD) */
  case GET_BLOCK_SIZE :
    *(DWORD*)buff = BLOCK_SIZE;
    break;
  
  default:
    res = RES_PARERR;
  }
  
  return res;
}
#endif /* _USE_IOCTL == 1 */
  
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

