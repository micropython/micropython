/**
  ******************************************************************************
  * @file    ff_gen_drv.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    18-February-2014
  * @brief   FatFs generic low level driver.
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
#include "ff_gen_drv.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
Disk_drvTypeDef  disk = {0};

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Links a compatible diskio driver and increments the number of active
  *         linked drivers.          
  * @note   The number of linked drivers (volumes) is up to 10 due to FatFs limits
  * @param  drv: pointer to the disk IO Driver structure
  * @param  path: pointer to the logical drive path 
  * @retval Returns 0 in case of success, otherwise 1.
  */
uint8_t FATFS_LinkDriver(Diskio_drvTypeDef *drv, char *path)
{
  uint8_t ret = 1;
  uint8_t DiskNum = 0;
  if(disk.nbr <= _VOLUMES)
  {
    disk.drv[disk.nbr] = drv;  
    DiskNum = disk.nbr++;
    path[0] = DiskNum + '0';
    path[1] = ':';
    path[2] = '/';
    path[3] = 0;
    ret = 0;
  }
  return ret;
}

/**
  * @brief  Unlinks a diskio driver and decrements the number of active linked
  *         drivers.
  * @param  path: pointer to the logical drive path  
  * @retval Returns 0 in case of success, otherwise 1.
  */
uint8_t FATFS_UnLinkDriver(char *path)
{ 
  uint8_t DiskNum = 0;
  uint8_t ret = 1;
  
  if(disk.nbr >= 1)
  {    
    DiskNum = path[0] - '0';
    if(DiskNum <= disk.nbr)
    {
      disk.drv[disk.nbr--] = 0;
      ret = 0;
    }
  }
  
  return ret;
}

/**
  * @brief  Gets number of linked drivers to the FatFs module.
  * @param  None
  * @retval Number of attached drivers.
  */
uint8_t FATFS_GetAttachedDriversNbr(void)
{
  return disk.nbr;
}
 
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

