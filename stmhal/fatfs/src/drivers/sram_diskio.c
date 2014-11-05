/**
  ******************************************************************************
  * @file    sram_diskio.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    18-February-2014
  * @brief   SRAM Disk I/O driver
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
DSTATUS SRAMDISK_initialize (void);
DSTATUS SRAMDISK_status (void);
DRESULT SRAMDISK_read (BYTE*, DWORD, BYTE);
#if _USE_WRITE == 1
  DRESULT SRAMDISK_write (const BYTE*, DWORD, BYTE);
#endif /* _USE_WRITE == 1 */
#if _USE_IOCTL == 1
  DRESULT SRAMDISK_ioctl (BYTE, void*);
#endif /* _USE_IOCTL == 1 */
  
Diskio_drvTypeDef  SRAMDISK_Driver =
{
  SRAMDISK_initialize,
  SRAMDISK_status,
  SRAMDISK_read, 
#if  _USE_WRITE == 1
  SRAMDISK_write,
#endif /* _USE_WRITE == 1 */  
#if  _USE_IOCTL == 1
  SRAMDISK_ioctl,
#endif /* _USE_IOCTL == 1 */
};

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initializes a Drive
  * @param  None
  * @retval DSTATUS: Operation status
  */
DSTATUS SRAMDISK_initialize(void)
{
  Stat = STA_NOINIT;
  
  /* Configure the SRAM device */
  BSP_SRAM_Init();
  
  Stat &= ~STA_NOINIT;
  return Stat;
}

/**
  * @brief  Gets Disk Status
  * @param  None
  * @retval DSTATUS: Operation status
  */
DSTATUS SRAMDISK_status(void)
{
  Stat = STA_NOINIT;
  
  Stat &= ~STA_NOINIT;

  return Stat;
}

/**
  * @brief  Reads Sector(s) 
  * @param  *buff: Data buffer to store read data
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to read (1..128)
  * @retval DRESULT: Operation result
  */
DRESULT SRAMDISK_read(BYTE *buff, DWORD sector, BYTE count)
{
  uint32_t BufferSize = (BLOCK_SIZE * count); 
  uint8_t *pSramAddress = (uint8_t *) (SRAM_DEVICE_ADDR + (sector * BLOCK_SIZE)); 
  
  for(; BufferSize != 0; BufferSize--)
  {
    *buff++ = *(__IO uint8_t *)pSramAddress++;  
  } 
  
  return RES_OK;
}

/**
  * @brief  Writes Sector(s)
  * @param  *buff: Data to be written
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to write (1..128)
  * @retval DRESULT: Operation result
  */
#if _USE_WRITE == 1
DRESULT SRAMDISK_write(const BYTE *buff, DWORD sector, BYTE count)
{
  uint32_t BufferSize = (BLOCK_SIZE * count) + count; 
  uint8_t *pSramAddress = (uint8_t *) (SRAM_DEVICE_ADDR + (sector * BLOCK_SIZE)); 
  
  for(; BufferSize != 0; BufferSize--)
  {
    *(__IO uint8_t *)pSramAddress++ = *buff++;    
  } 
  
  return RES_OK;
}
#endif /* _USE_WRITE == 1 */

/**
  * @brief  I/O control operation
  * @param  cmd: Control code
  * @param  *buff: Buffer to send/receive control data
  * @retval DRESULT: Operation result
  */
#if _USE_IOCTL == 1
DRESULT SRAMDISK_ioctl(BYTE cmd, void *buff)
{
  DRESULT res = RES_ERROR;
  
  if (Stat & STA_NOINIT) return RES_NOTRDY;
  
  switch (cmd)
  {
  /* Make sure that no pending write process */
  case CTRL_SYNC :
    res = RES_OK;
    break;
  
  /* Get number of sectors on the disk (DWORD) */
  case GET_SECTOR_COUNT :
    *(DWORD*)buff = SRAM_DEVICE_SIZE / BLOCK_SIZE;
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

