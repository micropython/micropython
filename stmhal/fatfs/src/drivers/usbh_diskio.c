/**
  ******************************************************************************
  * @file    usbh_diskio.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    18-February-2014
  * @brief   USB Key Disk I/O driver.
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
/* Private variables ---------------------------------------------------------*/
extern USBH_HandleTypeDef  HOST_HANDLE;

/* Private function prototypes -----------------------------------------------*/
DSTATUS USBH_initialize (void);
DSTATUS USBH_status (void);
DRESULT USBH_read (BYTE*, DWORD, BYTE);

#if _USE_WRITE == 1
  DRESULT USBH_write (const BYTE*, DWORD, BYTE);
#endif /* _USE_WRITE == 1 */

#if _USE_IOCTL == 1
  DRESULT USBH_ioctl (BYTE, void*);
#endif /* _USE_IOCTL == 1 */
  
Diskio_drvTypeDef  USBH_Driver =
{
  USBH_initialize,
  USBH_status,
  USBH_read, 
#if  _USE_WRITE == 1
  USBH_write,
#endif /* _USE_WRITE == 1 */  
#if  _USE_IOCTL == 1
  USBH_ioctl,
#endif /* _USE_IOCTL == 1 */
};

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initializes a Drive
  * @param  None
  * @retval DSTATUS: Operation status
  */
DSTATUS USBH_initialize(void)
{
  return RES_OK;
}

/**
  * @brief  Gets Disk Status
  * @param  None
  * @retval DSTATUS: Operation status
  */
DSTATUS USBH_status(void)
{
  DRESULT res = RES_ERROR;
  
  if(USBH_MSC_UnitIsReady(&HOST_HANDLE, 0))
  {
    res = RES_OK;
  }
  else
  {
    res = RES_ERROR;
  }
  
  return res;
}

/**
  * @brief  Reads Sector(s) 
  * @param  *buff: Data buffer to store read data
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to read (1..128)
  * @retval DRESULT: Operation result
  */
DRESULT USBH_read(BYTE *buff, DWORD sector, BYTE count)
{
  DRESULT res = RES_ERROR;
  MSC_LUNTypeDef info;
  USBH_StatusTypeDef  status = USBH_OK;
  DWORD scratch [_MAX_SS / 4];
  
  if ((DWORD)buff & 3) /* DMA Alignment issue, do single up to aligned buffer */
  {
    while ((count--)&&(status == USBH_OK))
    {
      status = USBH_MSC_Read(&HOST_HANDLE, 0, sector + count, (uint8_t *)scratch, 1);
      if(status == USBH_OK)
      {
        memcpy (&buff[count * _MAX_SS] ,scratch, _MAX_SS);
      }
      else
      {
        break;
      }
    }
  }
  else
  {
    status = USBH_MSC_Read(&HOST_HANDLE, 0, sector, buff, count);
  }
  
  if(status == USBH_OK)
  {
    res = RES_OK;
  }
  else
  {
    USBH_MSC_GetLUNInfo(&HOST_HANDLE, 0, &info); 
    
    switch (info.sense.asc)
    {
    case SCSI_ASC_LOGICAL_UNIT_NOT_READY:
    case SCSI_ASC_MEDIUM_NOT_PRESENT:
    case SCSI_ASC_NOT_READY_TO_READY_CHANGE: 
      USBH_ErrLog ("USB Disk is not ready!");  
      res = RES_NOTRDY;
      break; 
      
    default:
      res = RES_ERROR;
      break;
    }
  }
  
  return res;
}

/**
  * @brief  Writes Sector(s)
  * @param  *buff: Data to be written
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to write (1..128)
  * @retval DRESULT: Operation result
  */
#if _USE_WRITE == 1
DRESULT USBH_write(const BYTE *buff, DWORD sector, BYTE count)
{
  DRESULT res = RES_ERROR; 
  MSC_LUNTypeDef info;
  USBH_StatusTypeDef  status = USBH_OK;  
  DWORD scratch [_MAX_SS / 4];  
  
  if ((DWORD)buff & 3) /* DMA Alignment issue, do single up to aligned buffer */
  {
    while (count--)
    {
      memcpy (scratch, &buff[count * _MAX_SS], _MAX_SS);
      
      status = USBH_MSC_Write(&HOST_HANDLE, 0, sector + count, (BYTE *)scratch, 1) ;
      if(status == USBH_FAIL)
      {
        break;
      }
    }
  }
  else
  {
    status = USBH_MSC_Write(&HOST_HANDLE, 0, sector, (BYTE *)buff, count);
  }
  
  if(status == USBH_OK)
  {
    res = RES_OK;
  }
  else
  {
    USBH_MSC_GetLUNInfo(&HOST_HANDLE, 0, &info); 
    
    switch (info.sense.asc)
    {
    case SCSI_ASC_WRITE_PROTECTED:
      USBH_ErrLog("USB Disk is Write protected!");
      res = RES_WRPRT;
      break;
      
    case SCSI_ASC_LOGICAL_UNIT_NOT_READY:
    case SCSI_ASC_MEDIUM_NOT_PRESENT:
    case SCSI_ASC_NOT_READY_TO_READY_CHANGE:
      USBH_ErrLog("USB Disk is not ready!");      
      res = RES_NOTRDY;
      break; 
      
    default:
      res = RES_ERROR;
      break;
    }
  }
  
  return res;   
}
#endif /* _USE_WRITE == 1 */

/**
  * @brief  I/O control operation
  * @param  cmd: Control code
  * @param  *buff: Buffer to send/receive control data
  * @retval DRESULT: Operation result
  */
#if _USE_IOCTL == 1
DRESULT USBH_ioctl(BYTE cmd, void *buff)
{
  DRESULT res = RES_ERROR;
  MSC_LUNTypeDef info;
  
  switch (cmd)
  {
  /* Make sure that no pending write process */
  case CTRL_SYNC: 
    res = RES_OK;
    break;
    
  /* Get number of sectors on the disk (DWORD) */  
  case GET_SECTOR_COUNT : 
    if(USBH_MSC_GetLUNInfo(&HOST_HANDLE, 0, &info) == USBH_OK)
    {
      *(DWORD*)buff = info.capacity.block_nbr;
      res = RES_OK;
    }
    else
    {
      res = RES_ERROR;
    }
    break;
    
  /* Get R/W sector size (WORD) */  
  case GET_SECTOR_SIZE :	
    if(USBH_MSC_GetLUNInfo(&HOST_HANDLE, 0, &info) == USBH_OK)
    {
      *(DWORD*)buff = info.capacity.block_size;
      res = RES_OK;
    }
    else
    {
      res = RES_ERROR;
    }
    break;
    
    /* Get erase block size in unit of sector (DWORD) */ 
  case GET_BLOCK_SIZE : 
    
    if(USBH_MSC_GetLUNInfo(&HOST_HANDLE, 0, &info) == USBH_OK)
    {
      *(DWORD*)buff = info.capacity.block_size;
      res = RES_OK;
    }
    else
    {
      res = RES_ERROR;
    }
    break;
    
  default:
    res = RES_PARERR;
  }
  
  return res;
}
#endif /* _USE_IOCTL == 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

