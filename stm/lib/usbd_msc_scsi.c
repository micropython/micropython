/**
  ******************************************************************************
  * @file    usbd_msc_scsi.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-March-2012
  * @brief   This file provides all the USBD SCSI layer functions.
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

/* Includes ------------------------------------------------------------------*/
#include "usbd_msc_bot.h"
#include "usbd_msc_scsi.h"
#include "usbd_msc_mem.h"
#include "usbd_msc_data.h"
#include "usbd_msc_conf.h"



/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @{
  */


/** @defgroup MSC_SCSI 
  * @brief Mass storage SCSI layer module
  * @{
  */ 

/** @defgroup MSC_SCSI_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup MSC_SCSI_Private_Defines
  * @{
  */ 

/**
  * @}
  */ 


/** @defgroup MSC_SCSI_Private_Macros
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup MSC_SCSI_Private_Variables
  * @{
  */ 

SCSI_Sense_TypeDef     SCSI_Sense [SENSE_LIST_DEEPTH];
uint8_t   SCSI_Sense_Head;
uint8_t   SCSI_Sense_Tail;

uint32_t  SCSI_blk_size;
uint32_t  SCSI_blk_nbr;

uint32_t  SCSI_blk_addr;
uint32_t  SCSI_blk_len;

USB_OTG_CORE_HANDLE  *cdev;
/**
  * @}
  */ 


/** @defgroup MSC_SCSI_Private_FunctionPrototypes
  * @{
  */ 
static int8_t SCSI_TestUnitReady(uint8_t lun, uint8_t *params);
static int8_t SCSI_Inquiry(uint8_t lun, uint8_t *params);
static int8_t SCSI_ReadFormatCapacity(uint8_t lun, uint8_t *params);
static int8_t SCSI_ReadCapacity10(uint8_t lun, uint8_t *params);
static int8_t SCSI_RequestSense (uint8_t lun, uint8_t *params);
static int8_t SCSI_StartStopUnit(uint8_t lun, uint8_t *params);
static int8_t SCSI_ModeSense6 (uint8_t lun, uint8_t *params);
static int8_t SCSI_ModeSense10 (uint8_t lun, uint8_t *params);
static int8_t SCSI_Write10(uint8_t lun , uint8_t *params);
static int8_t SCSI_Read10(uint8_t lun , uint8_t *params);
static int8_t SCSI_Verify10(uint8_t lun, uint8_t *params);
static int8_t SCSI_CheckAddressRange (uint8_t lun , 
                                      uint32_t blk_offset , 
                                      uint16_t blk_nbr);
static int8_t SCSI_ProcessRead (uint8_t lun);

static int8_t SCSI_ProcessWrite (uint8_t lun);
/**
  * @}
  */ 


/** @defgroup MSC_SCSI_Private_Functions
  * @{
  */ 


/**
* @brief  SCSI_ProcessCmd
*         Process SCSI commands
* @param  pdev: device instance
* @param  lun: Logical unit number
* @param  params: Command parameters
* @retval status
*/
int8_t SCSI_ProcessCmd(USB_OTG_CORE_HANDLE  *pdev,
                           uint8_t lun, 
                           uint8_t *params)
{
  cdev = pdev;
  
  switch (params[0])
  {
  case SCSI_TEST_UNIT_READY:
    return SCSI_TestUnitReady(lun, params);
    
  case SCSI_REQUEST_SENSE:
    return SCSI_RequestSense (lun, params);
  case SCSI_INQUIRY:
    return SCSI_Inquiry(lun, params);
    
  case SCSI_START_STOP_UNIT:
    return SCSI_StartStopUnit(lun, params);
    
  case SCSI_ALLOW_MEDIUM_REMOVAL:
    return SCSI_StartStopUnit(lun, params);
    
  case SCSI_MODE_SENSE6:
    return SCSI_ModeSense6 (lun, params);
    
  case SCSI_MODE_SENSE10:
    return SCSI_ModeSense10 (lun, params);
    
  case SCSI_READ_FORMAT_CAPACITIES:
    return SCSI_ReadFormatCapacity(lun, params);
    
  case SCSI_READ_CAPACITY10:
    return SCSI_ReadCapacity10(lun, params);
    
  case SCSI_READ10:
    return SCSI_Read10(lun, params); 
    
  case SCSI_WRITE10:
    return SCSI_Write10(lun, params);
    
  case SCSI_VERIFY10:
    return SCSI_Verify10(lun, params);
    
  default:
    SCSI_SenseCode(lun,
                   ILLEGAL_REQUEST, 
                   INVALID_CDB);    
    return -1;
  }
}


/**
* @brief  SCSI_TestUnitReady
*         Process SCSI Test Unit Ready Command
* @param  lun: Logical unit number
* @param  params: Command parameters
* @retval status
*/
static int8_t SCSI_TestUnitReady(uint8_t lun, uint8_t *params)
{
  
  /* case 9 : Hi > D0 */
  if (MSC_BOT_cbw.dDataLength != 0)
  {
    SCSI_SenseCode(MSC_BOT_cbw.bLUN, 
                   ILLEGAL_REQUEST, 
                   INVALID_CDB);
    return -1;
  }  
  
  if(USBD_STORAGE_fops->IsReady(lun) !=0 )
  {
    SCSI_SenseCode(lun,
                   NOT_READY, 
                   MEDIUM_NOT_PRESENT);
    return -1;
  } 
  MSC_BOT_DataLen = 0;
  return 0;
}

/**
* @brief  SCSI_Inquiry
*         Process Inquiry command
* @param  lun: Logical unit number
* @param  params: Command parameters
* @retval status
*/
static int8_t  SCSI_Inquiry(uint8_t lun, uint8_t *params)
{
  uint8_t* pPage;
  uint16_t len;
  
  if (params[1] & 0x01)/*Evpd is set*/
  {
    pPage = (uint8_t *)MSC_Page00_Inquiry_Data;
    len = LENGTH_INQUIRY_PAGE00;
  }
  else
  {
    
    pPage = (uint8_t *)&USBD_STORAGE_fops->pInquiry[lun * USBD_STD_INQUIRY_LENGTH];
    len = pPage[4] + 5;
    
    if (params[4] <= len)
    {
      len = params[4];
    }
  }
  MSC_BOT_DataLen = len;
  
  while (len) 
  {
    len--;
    MSC_BOT_Data[len] = pPage[len];
  }
  return 0;
}

/**
* @brief  SCSI_ReadCapacity10
*         Process Read Capacity 10 command
* @param  lun: Logical unit number
* @param  params: Command parameters
* @retval status
*/
static int8_t SCSI_ReadCapacity10(uint8_t lun, uint8_t *params)
{
  
  if(USBD_STORAGE_fops->GetCapacity(lun, &SCSI_blk_nbr, &SCSI_blk_size) != 0)
  {
    SCSI_SenseCode(lun,
                   NOT_READY, 
                   MEDIUM_NOT_PRESENT);
    return -1;
  } 
  else
  {
    
    MSC_BOT_Data[0] = (uint8_t)((SCSI_blk_nbr - 1) >> 24); // dpgeorge added paren
    MSC_BOT_Data[1] = (uint8_t)((SCSI_blk_nbr - 1) >> 16); // dpgeorge added paren
    MSC_BOT_Data[2] = (uint8_t)((SCSI_blk_nbr - 1) >>  8); // dpgeorge added paren
    MSC_BOT_Data[3] = (uint8_t)(SCSI_blk_nbr - 1);
    
    MSC_BOT_Data[4] = (uint8_t)(SCSI_blk_size >>  24);
    MSC_BOT_Data[5] = (uint8_t)(SCSI_blk_size >>  16);
    MSC_BOT_Data[6] = (uint8_t)(SCSI_blk_size >>  8);
    MSC_BOT_Data[7] = (uint8_t)(SCSI_blk_size);
    
    MSC_BOT_DataLen = 8;
    return 0;
  }
}
/**
* @brief  SCSI_ReadFormatCapacity
*         Process Read Format Capacity command
* @param  lun: Logical unit number
* @param  params: Command parameters
* @retval status
*/
static int8_t SCSI_ReadFormatCapacity(uint8_t lun, uint8_t *params)
{
  
  uint32_t blk_size;
  uint32_t blk_nbr;
  uint16_t i;
  
  for(i=0 ; i < 12 ; i++) 
  {
    MSC_BOT_Data[i] = 0;
  }
  
  if(USBD_STORAGE_fops->GetCapacity(lun, &blk_nbr, &blk_size) != 0)
  {
    SCSI_SenseCode(lun,
                   NOT_READY, 
                   MEDIUM_NOT_PRESENT);
    return -1;
  } 
  else
  {
    MSC_BOT_Data[3] = 0x08;
    MSC_BOT_Data[4] = (uint8_t)((blk_nbr - 1) >> 24); // dpgeorge added paren
    MSC_BOT_Data[5] = (uint8_t)((blk_nbr - 1) >> 16); // dpgeorge added paren
    MSC_BOT_Data[6] = (uint8_t)((blk_nbr - 1) >>  8); // dpgeorge added paren
    MSC_BOT_Data[7] = (uint8_t)(blk_nbr - 1);
    
    MSC_BOT_Data[8] = 0x02;
    MSC_BOT_Data[9] = (uint8_t)(blk_size >>  16);
    MSC_BOT_Data[10] = (uint8_t)(blk_size >>  8);
    MSC_BOT_Data[11] = (uint8_t)(blk_size);
    
    MSC_BOT_DataLen = 12;
    return 0;
  }
}
/**
* @brief  SCSI_ModeSense6
*         Process Mode Sense6 command
* @param  lun: Logical unit number
* @param  params: Command parameters
* @retval status
*/
static int8_t SCSI_ModeSense6 (uint8_t lun, uint8_t *params)
{
  
  uint16_t len = 8 ;
  MSC_BOT_DataLen = len;
  
  while (len) 
  {
    len--;
    MSC_BOT_Data[len] = MSC_Mode_Sense6_data[len];
  }
  return 0;
}

/**
* @brief  SCSI_ModeSense10
*         Process Mode Sense10 command
* @param  lun: Logical unit number
* @param  params: Command parameters
* @retval status
*/
static int8_t SCSI_ModeSense10 (uint8_t lun, uint8_t *params)
{
 uint16_t len = 8;

 MSC_BOT_DataLen = len;

 while (len) 
  {
    len--;
    MSC_BOT_Data[len] = MSC_Mode_Sense10_data[len];
  }
  return 0;
}

/**
* @brief  SCSI_RequestSense
*         Process Request Sense command
* @param  lun: Logical unit number
* @param  params: Command parameters
* @retval status
*/

static int8_t SCSI_RequestSense (uint8_t lun, uint8_t *params)
{
  uint8_t i;
  
  for(i=0 ; i < REQUEST_SENSE_DATA_LEN ; i++) 
  {
    MSC_BOT_Data[i] = 0;
  }
  
  MSC_BOT_Data[0]	= 0x70;		
  MSC_BOT_Data[7]	= REQUEST_SENSE_DATA_LEN - 6;	
  
  if((SCSI_Sense_Head != SCSI_Sense_Tail)) {
    
    MSC_BOT_Data[2]     = SCSI_Sense[SCSI_Sense_Head].Skey;		
    MSC_BOT_Data[12]    = SCSI_Sense[SCSI_Sense_Head].w.b.ASCQ;	
    MSC_BOT_Data[13]    = SCSI_Sense[SCSI_Sense_Head].w.b.ASC;	
    SCSI_Sense_Head++;
    
    if (SCSI_Sense_Head == SENSE_LIST_DEEPTH)
    {
      SCSI_Sense_Head = 0;
    }
  }
  MSC_BOT_DataLen = REQUEST_SENSE_DATA_LEN;  
  
  if (params[4] <= REQUEST_SENSE_DATA_LEN)
  {
    MSC_BOT_DataLen = params[4];
  }
  return 0;
}

/**
* @brief  SCSI_SenseCode
*         Load the last error code in the error list
* @param  lun: Logical unit number
* @param  sKey: Sense Key
* @param  ASC: Additional Sense Key
* @retval none

*/
void SCSI_SenseCode(uint8_t lun, uint8_t sKey, uint8_t ASC)
{
  SCSI_Sense[SCSI_Sense_Tail].Skey  = sKey;
  SCSI_Sense[SCSI_Sense_Tail].w.ASC = ASC << 8;
  SCSI_Sense_Tail++;
  if (SCSI_Sense_Tail == SENSE_LIST_DEEPTH)
  {
    SCSI_Sense_Tail = 0;
  }
}
/**
* @brief  SCSI_StartStopUnit
*         Process Start Stop Unit command
* @param  lun: Logical unit number
* @param  params: Command parameters
* @retval status
*/
static int8_t SCSI_StartStopUnit(uint8_t lun, uint8_t *params)
{
  MSC_BOT_DataLen = 0;
  return 0;
}

/**
* @brief  SCSI_Read10
*         Process Read10 command
* @param  lun: Logical unit number
* @param  params: Command parameters
* @retval status
*/
static int8_t SCSI_Read10(uint8_t lun , uint8_t *params)
{
  if(MSC_BOT_State == BOT_IDLE)  /* Idle */
  {
    
    /* case 10 : Ho <> Di */
    
    if ((MSC_BOT_cbw.bmFlags & 0x80) != 0x80)
    {
      SCSI_SenseCode(MSC_BOT_cbw.bLUN, 
                     ILLEGAL_REQUEST, 
                     INVALID_CDB);
      return -1;
    }    
    
    if(USBD_STORAGE_fops->IsReady(lun) !=0 )
    {
      SCSI_SenseCode(lun,
                     NOT_READY, 
                     MEDIUM_NOT_PRESENT);
      return -1;
    } 
    
    SCSI_blk_addr = (params[2] << 24) | \
      (params[3] << 16) | \
        (params[4] <<  8) | \
          params[5];
    
    SCSI_blk_len =  (params[7] <<  8) | \
      params[8];  
    
    
    
    if( SCSI_CheckAddressRange(lun, SCSI_blk_addr, SCSI_blk_len) < 0)
    {
      return -1; /* error */
    }
    
    MSC_BOT_State = BOT_DATA_IN;
    SCSI_blk_addr *= SCSI_blk_size;
    SCSI_blk_len  *= SCSI_blk_size;
    
    /* cases 4,5 : Hi <> Dn */
    if (MSC_BOT_cbw.dDataLength != SCSI_blk_len)
    {
      SCSI_SenseCode(MSC_BOT_cbw.bLUN, 
                     ILLEGAL_REQUEST, 
                     INVALID_CDB);
      return -1;
    }
  }
  MSC_BOT_DataLen = MSC_MEDIA_PACKET;  
  
  return SCSI_ProcessRead(lun);
}

/**
* @brief  SCSI_Write10
*         Process Write10 command
* @param  lun: Logical unit number
* @param  params: Command parameters
* @retval status
*/

static int8_t SCSI_Write10 (uint8_t lun , uint8_t *params)
{
  if (MSC_BOT_State == BOT_IDLE) /* Idle */
  {
    
    /* case 8 : Hi <> Do */
    
    if ((MSC_BOT_cbw.bmFlags & 0x80) == 0x80)
    {
      SCSI_SenseCode(MSC_BOT_cbw.bLUN, 
                     ILLEGAL_REQUEST, 
                     INVALID_CDB);
      return -1;
    }
    
    /* Check whether Media is ready */
    if(USBD_STORAGE_fops->IsReady(lun) !=0 )
    {
      SCSI_SenseCode(lun,
                     NOT_READY, 
                     MEDIUM_NOT_PRESENT);
      return -1;
    } 
    
    /* Check If media is write-protected */
    if(USBD_STORAGE_fops->IsWriteProtected(lun) !=0 )
    {
      SCSI_SenseCode(lun,
                     NOT_READY, 
                     WRITE_PROTECTED);
      return -1;
    } 
    
    
    SCSI_blk_addr = (params[2] << 24) | \
      (params[3] << 16) | \
        (params[4] <<  8) | \
          params[5];
    SCSI_blk_len = (params[7] <<  8) | \
      params[8];  
    
    /* check if LBA address is in the right range */
    if(SCSI_CheckAddressRange(lun, SCSI_blk_addr, SCSI_blk_len) < 0)
    {
      return -1; /* error */      
    }
    
    SCSI_blk_addr *= SCSI_blk_size;
    SCSI_blk_len  *= SCSI_blk_size;
    
    /* cases 3,11,13 : Hn,Ho <> D0 */
    if (MSC_BOT_cbw.dDataLength != SCSI_blk_len)
    {
      SCSI_SenseCode(MSC_BOT_cbw.bLUN, 
                     ILLEGAL_REQUEST, 
                     INVALID_CDB);
      return -1;
    }
    
    /* Prepare EP to receive first data packet */
    MSC_BOT_State = BOT_DATA_OUT;  
    DCD_EP_PrepareRx (cdev,
                      MSC_OUT_EP,
                      MSC_BOT_Data, 
                      MIN (SCSI_blk_len, MSC_MEDIA_PACKET));  
  }
  else /* Write Process ongoing */
  {
    return SCSI_ProcessWrite(lun);
  }
  return 0;
}


/**
* @brief  SCSI_Verify10
*         Process Verify10 command
* @param  lun: Logical unit number
* @param  params: Command parameters
* @retval status
*/

static int8_t SCSI_Verify10(uint8_t lun , uint8_t *params){
  if ((params[1]& 0x02) == 0x02) 
  {
    SCSI_SenseCode (lun, ILLEGAL_REQUEST, INVALID_FIELED_IN_COMMAND);
    return -1; /* Error, Verify Mode Not supported*/
  }
  
  if(SCSI_CheckAddressRange(lun, SCSI_blk_addr, SCSI_blk_len) < 0)
  {
    return -1; /* error */      
  }
  MSC_BOT_DataLen = 0;
  return 0;
}

/**
* @brief  SCSI_CheckAddressRange
*         Check address range
* @param  lun: Logical unit number
* @param  blk_offset: first block address
* @param  blk_nbr: number of block to be processed
* @retval status
*/
static int8_t SCSI_CheckAddressRange (uint8_t lun , uint32_t blk_offset , uint16_t blk_nbr)
{
  
  if ((blk_offset + blk_nbr) > SCSI_blk_nbr )
  {
    SCSI_SenseCode(lun, ILLEGAL_REQUEST, ADDRESS_OUT_OF_RANGE);
    return -1;
  }
  return 0;
}

/**
* @brief  SCSI_ProcessRead
*         Handle Read Process
* @param  lun: Logical unit number
* @retval status
*/
static int8_t SCSI_ProcessRead (uint8_t lun)
{
  uint32_t len;
  
  len = MIN(SCSI_blk_len , MSC_MEDIA_PACKET); 
  
  if( USBD_STORAGE_fops->Read(lun ,
                              MSC_BOT_Data, 
                              SCSI_blk_addr / SCSI_blk_size, 
                              len / SCSI_blk_size) < 0)
  {
    
    SCSI_SenseCode(lun, HARDWARE_ERROR, UNRECOVERED_READ_ERROR);
    return -1; 
  }
  
  
  DCD_EP_Tx (cdev, 
             MSC_IN_EP,
             MSC_BOT_Data,
             len);
  
  
  SCSI_blk_addr   += len; 
  SCSI_blk_len    -= len;  
  
  /* case 6 : Hi = Di */
  MSC_BOT_csw.dDataResidue -= len;
  
  if (SCSI_blk_len == 0)
  {
    MSC_BOT_State = BOT_LAST_DATA_IN;
  }
  return 0;
}

/**
* @brief  SCSI_ProcessWrite
*         Handle Write Process
* @param  lun: Logical unit number
* @retval status
*/

static int8_t SCSI_ProcessWrite (uint8_t lun)
{
  uint32_t len;
  
  len = MIN(SCSI_blk_len , MSC_MEDIA_PACKET); 
  
  if(USBD_STORAGE_fops->Write(lun ,
                              MSC_BOT_Data, 
                              SCSI_blk_addr / SCSI_blk_size, 
                              len / SCSI_blk_size) < 0)
  {
    SCSI_SenseCode(lun, HARDWARE_ERROR, WRITE_FAULT);     
    return -1; 
  }
  
  
  SCSI_blk_addr  += len; 
  SCSI_blk_len   -= len; 
  
  /* case 12 : Ho = Do */
  MSC_BOT_csw.dDataResidue -= len;
  
  if (SCSI_blk_len == 0)
  {
    MSC_BOT_SendCSW (cdev, CSW_CMD_PASSED);
  }
  else
  {
    /* Prapare EP to Receive next packet */
    DCD_EP_PrepareRx (cdev,
                      MSC_OUT_EP,
                      MSC_BOT_Data, 
                      MIN (SCSI_blk_len, MSC_MEDIA_PACKET)); 
  }
  
  return 0;
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
