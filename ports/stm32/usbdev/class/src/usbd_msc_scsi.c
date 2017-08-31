/**
  ******************************************************************************
  * @file    usbd_msc_scsi.c
  * @author  MCD Application Team
  * @version V2.0.0
  * @date    18-February-2014
  * @brief   This file provides all the USBD SCSI layer functions.
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
#include "usbd_msc_bot.h"
#include "usbd_msc_scsi.h"
#include "usbd_msc_data.h"
#include "usbd_cdc_msc_hid.h"



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

/**
  * @}
  */ 


/** @defgroup MSC_SCSI_Private_FunctionPrototypes
  * @{
  */ 
static int8_t SCSI_TestUnitReady(USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params);
static int8_t SCSI_Inquiry(USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params);
static int8_t SCSI_ReadFormatCapacity(USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params);
static int8_t SCSI_ReadCapacity10(USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params);
static int8_t SCSI_RequestSense (USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params);
static int8_t SCSI_StartStopUnit(USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params);
static int8_t SCSI_AllowMediumRemoval(USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params);
static int8_t SCSI_ModeSense6 (USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params);
static int8_t SCSI_ModeSense10 (USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params);
static int8_t SCSI_SynchronizeCache(USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params);
static int8_t SCSI_Write10(USBD_HandleTypeDef  *pdev, uint8_t lun , uint8_t *params);
static int8_t SCSI_Read10(USBD_HandleTypeDef  *pdev, uint8_t lun , uint8_t *params);
static int8_t SCSI_Verify10(USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params);
static int8_t SCSI_CheckAddressRange (USBD_HandleTypeDef  *pdev, 
                                      uint8_t lun , 
                                      uint32_t blk_offset , 
                                      uint16_t blk_nbr);
static int8_t SCSI_ProcessRead (USBD_HandleTypeDef  *pdev,
                                uint8_t lun);

static int8_t SCSI_ProcessWrite (USBD_HandleTypeDef  *pdev,
                                 uint8_t lun);
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
int8_t SCSI_ProcessCmd(USBD_HandleTypeDef  *pdev,
                           uint8_t lun, 
                           uint8_t *params)
{
    /*
    if (params[0] != SCSI_READ10 && params[0] != SCSI_WRITE10) {
        printf("SCSI_ProcessCmd(lun=%d, params=%x, %x)\n", lun, params[0], params[1]);
    }
    */
  
  switch (params[0])
  {
  case SCSI_TEST_UNIT_READY:
    return SCSI_TestUnitReady(pdev, lun, params);
    
  case SCSI_REQUEST_SENSE:
    return SCSI_RequestSense (pdev, lun, params);
  case SCSI_INQUIRY:
    return SCSI_Inquiry(pdev, lun, params);
    
  case SCSI_START_STOP_UNIT:
    return SCSI_StartStopUnit(pdev, lun, params);
    
  case SCSI_ALLOW_MEDIUM_REMOVAL:
    return SCSI_AllowMediumRemoval(pdev, lun, params);
    
  case SCSI_MODE_SENSE6:
    return SCSI_ModeSense6 (pdev, lun, params);
    
  case SCSI_MODE_SENSE10:
    return SCSI_ModeSense10 (pdev, lun, params);
    
  case SCSI_SYNCHRONIZE_CACHE10:
  case SCSI_SYNCHRONIZE_CACHE16:
    return SCSI_SynchronizeCache(pdev, lun, params);

  case SCSI_READ_FORMAT_CAPACITIES:
    return SCSI_ReadFormatCapacity(pdev, lun, params);
    
  case SCSI_READ_CAPACITY10:
    return SCSI_ReadCapacity10(pdev, lun, params);
    
  case SCSI_READ10:
    return SCSI_Read10(pdev, lun, params); 
    
  case SCSI_WRITE10:
    return SCSI_Write10(pdev, lun, params);
    
  case SCSI_VERIFY10:
    return SCSI_Verify10(pdev, lun, params);
    
  default:
    SCSI_SenseCode(pdev, 
                   lun,
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
static int8_t SCSI_TestUnitReady(USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params)
{
  USBD_MSC_BOT_HandleTypeDef  *hmsc = ((usbd_cdc_msc_hid_state_t*)pdev->pClassData)->msc;
    
  /* case 9 : Hi > D0 */
  if (hmsc->cbw.dDataLength != 0)
  {
    SCSI_SenseCode(pdev,
                   hmsc->cbw.bLUN, 
                   ILLEGAL_REQUEST, 
                   INVALID_CDB);
    return -1;
  }  
  
  if(((USBD_StorageTypeDef *)pdev->pUserData)->IsReady(lun) !=0 )
  {
    SCSI_SenseCode(pdev,
                   lun,
                   NOT_READY, 
                   MEDIUM_NOT_PRESENT);
    
    hmsc->bot_state = USBD_BOT_NO_DATA;
    return -1;
  } 
  hmsc->bot_data_length = 0;
  return 0;
}

/**
* @brief  SCSI_Inquiry
*         Process Inquiry command
* @param  lun: Logical unit number
* @param  params: Command parameters
* @retval status
*/
static int8_t  SCSI_Inquiry(USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params)
{
  uint8_t* pPage;
  uint16_t len;
  USBD_MSC_BOT_HandleTypeDef  *hmsc = ((usbd_cdc_msc_hid_state_t*)pdev->pClassData)->msc;
  
  if (params[1] & 0x01)/*Evpd is set*/
  {
    pPage = (uint8_t *)MSC_Page00_Inquiry_Data;
    len = LENGTH_INQUIRY_PAGE00;
  }
  else
  {
    
    pPage = (uint8_t *)&((USBD_StorageTypeDef *)pdev->pUserData)->pInquiry[lun * STANDARD_INQUIRY_DATA_LEN];
    len = pPage[4] + 5;
    
    if (params[4] <= len)
    {
      len = params[4];
    }
  }
  hmsc->bot_data_length = len;
  
  while (len) 
  {
    len--;
    hmsc->bot_data[len] = pPage[len];
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
static int8_t SCSI_ReadCapacity10(USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params)
{
  USBD_MSC_BOT_HandleTypeDef  *hmsc = ((usbd_cdc_msc_hid_state_t*)pdev->pClassData)->msc;
  
  if(((USBD_StorageTypeDef *)pdev->pUserData)->GetCapacity(lun, &hmsc->scsi_blk_nbr, &hmsc->scsi_blk_size) != 0)
  {
    SCSI_SenseCode(pdev,
                   lun,
                   NOT_READY, 
                   MEDIUM_NOT_PRESENT);
    return -1;
  } 
  else
  {
    
    hmsc->bot_data[0] = (uint8_t)((hmsc->scsi_blk_nbr - 1) >> 24);
    hmsc->bot_data[1] = (uint8_t)((hmsc->scsi_blk_nbr - 1) >> 16);
    hmsc->bot_data[2] = (uint8_t)((hmsc->scsi_blk_nbr - 1) >>  8);
    hmsc->bot_data[3] = (uint8_t)(hmsc->scsi_blk_nbr - 1);
    
    hmsc->bot_data[4] = (uint8_t)(hmsc->scsi_blk_size >>  24);
    hmsc->bot_data[5] = (uint8_t)(hmsc->scsi_blk_size >>  16);
    hmsc->bot_data[6] = (uint8_t)(hmsc->scsi_blk_size >>  8);
    hmsc->bot_data[7] = (uint8_t)(hmsc->scsi_blk_size);
    
    hmsc->bot_data_length = 8;
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
static int8_t SCSI_ReadFormatCapacity(USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params)
{
  USBD_MSC_BOT_HandleTypeDef  *hmsc = ((usbd_cdc_msc_hid_state_t*)pdev->pClassData)->msc;
  
  uint16_t blk_size;
  uint32_t blk_nbr;
  uint16_t i;
  
  for(i=0 ; i < 12 ; i++) 
  {
    hmsc->bot_data[i] = 0;
  }
  
  if(((USBD_StorageTypeDef *)pdev->pUserData)->GetCapacity(lun, &blk_nbr, &blk_size) != 0)
  {
    SCSI_SenseCode(pdev,
                   lun,
                   NOT_READY, 
                   MEDIUM_NOT_PRESENT);
    return -1;
  } 
  else
  {
    hmsc->bot_data[3] = 0x08;
    hmsc->bot_data[4] = (uint8_t)((blk_nbr - 1) >> 24);
    hmsc->bot_data[5] = (uint8_t)((blk_nbr - 1) >> 16);
    hmsc->bot_data[6] = (uint8_t)((blk_nbr - 1) >>  8);
    hmsc->bot_data[7] = (uint8_t)(blk_nbr - 1);
    
    hmsc->bot_data[8] = 0x02;
    hmsc->bot_data[9] = (uint8_t)(blk_size >>  16);
    hmsc->bot_data[10] = (uint8_t)(blk_size >>  8);
    hmsc->bot_data[11] = (uint8_t)(blk_size);
    
    hmsc->bot_data_length = 12;
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
static int8_t SCSI_ModeSense6 (USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params)
{
  USBD_MSC_BOT_HandleTypeDef  *hmsc = ((usbd_cdc_msc_hid_state_t*)pdev->pClassData)->msc;
  uint16_t len = 8 ;
  hmsc->bot_data_length = len;
  
  while (len) 
  {
    len--;
    hmsc->bot_data[len] = MSC_Mode_Sense6_data[len];
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
static int8_t SCSI_ModeSense10 (USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params)
{
  uint16_t len = 8;
  USBD_MSC_BOT_HandleTypeDef  *hmsc = ((usbd_cdc_msc_hid_state_t*)pdev->pClassData)->msc;
  
  hmsc->bot_data_length = len;

  while (len) 
  {
    len--;
    hmsc->bot_data[len] = MSC_Mode_Sense10_data[len];
  }
  return 0;
}

static int8_t SCSI_SynchronizeCache(USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params) {
    // nothing to synchronize, so just return "success"
    USBD_MSC_BOT_HandleTypeDef  *hmsc = ((usbd_cdc_msc_hid_state_t*)pdev->pClassData)->msc;
    hmsc->bot_data_length = 0;
    return 0;
}

/**
* @brief  SCSI_RequestSense
*         Process Request Sense command
* @param  lun: Logical unit number
* @param  params: Command parameters
* @retval status
*/

static int8_t SCSI_RequestSense (USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params)
{
  uint8_t i;
  USBD_MSC_BOT_HandleTypeDef  *hmsc = ((usbd_cdc_msc_hid_state_t*)pdev->pClassData)->msc;
  
  for(i=0 ; i < REQUEST_SENSE_DATA_LEN ; i++) 
  {
    hmsc->bot_data[i] = 0;
  }
  
  hmsc->bot_data[0]	= 0x70;		
  hmsc->bot_data[7]	= REQUEST_SENSE_DATA_LEN - 6;	
  
  if((hmsc->scsi_sense_head != hmsc->scsi_sense_tail)) {
    
    hmsc->bot_data[2]     = hmsc->scsi_sense[hmsc->scsi_sense_head].Skey;		
    hmsc->bot_data[12]    = hmsc->scsi_sense[hmsc->scsi_sense_head].w.b.ASCQ;	
    hmsc->bot_data[13]    = hmsc->scsi_sense[hmsc->scsi_sense_head].w.b.ASC;	
    hmsc->scsi_sense_head++;
    
    if (hmsc->scsi_sense_head == SENSE_LIST_DEEPTH)
    {
      hmsc->scsi_sense_head = 0;
    }
  }
  hmsc->bot_data_length = REQUEST_SENSE_DATA_LEN;  
  
  if (params[4] <= REQUEST_SENSE_DATA_LEN)
  {
    hmsc->bot_data_length = params[4];
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
void SCSI_SenseCode(USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t sKey, uint8_t ASC)
{
  USBD_MSC_BOT_HandleTypeDef  *hmsc = ((usbd_cdc_msc_hid_state_t*)pdev->pClassData)->msc;
  
  hmsc->scsi_sense[hmsc->scsi_sense_tail].Skey  = sKey;
  hmsc->scsi_sense[hmsc->scsi_sense_tail].w.ASC = ASC << 8;
  hmsc->scsi_sense_tail++;
  if (hmsc->scsi_sense_tail == SENSE_LIST_DEEPTH)
  {
    hmsc->scsi_sense_tail = 0;
  }
}
/**
* @brief  SCSI_StartStopUnit
*         Process Start Stop Unit command
* @param  lun: Logical unit number
* @param  params: Command parameters
* @retval status
*/
static int8_t SCSI_StartStopUnit(USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params)
{
  USBD_MSC_BOT_HandleTypeDef  *hmsc = ((usbd_cdc_msc_hid_state_t*)pdev->pClassData)->msc;
  hmsc->bot_data_length = 0;

  // On Mac OS X, when the device is ejected a SCSI_START_STOP_UNIT command is sent.
  // Bit 0 of params[4] is the START bit.
  // If we get a stop, we must really stop the device so that the Mac does not
  // automatically remount it.
  ((USBD_StorageTypeDef *)pdev->pUserData)->StartStopUnit(lun, params[4] & 1);

  return 0;
}

/**
* @brief  SCSI_AllowMediumRemoval
*         Process Allow Medium Removal command
* @param  lun: Logical unit number
* @param  params: Command parameters
* @retval status
*/
static int8_t SCSI_AllowMediumRemoval(USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params)
{
  USBD_MSC_BOT_HandleTypeDef  *hmsc = ((usbd_cdc_msc_hid_state_t*)pdev->pClassData)->msc;
  hmsc->bot_data_length = 0;
  ((USBD_StorageTypeDef *)pdev->pUserData)->PreventAllowMediumRemoval(lun, params[0]);
  return 0;
}

/**
* @brief  SCSI_Read10
*         Process Read10 command
* @param  lun: Logical unit number
* @param  params: Command parameters
* @retval status
*/
static int8_t SCSI_Read10(USBD_HandleTypeDef  *pdev, uint8_t lun , uint8_t *params)
{
  USBD_MSC_BOT_HandleTypeDef  *hmsc = ((usbd_cdc_msc_hid_state_t*)pdev->pClassData)->msc;
  
  if(hmsc->bot_state == USBD_BOT_IDLE)  /* Idle */
  {
    
    /* case 10 : Ho <> Di */
    
    if ((hmsc->cbw.bmFlags & 0x80) != 0x80)
    {
      SCSI_SenseCode(pdev,
                     hmsc->cbw.bLUN, 
                     ILLEGAL_REQUEST, 
                     INVALID_CDB);
      return -1;
    }    
    
    if(((USBD_StorageTypeDef *)pdev->pUserData)->IsReady(lun) !=0 )
    {
      SCSI_SenseCode(pdev,
                     lun,
                     NOT_READY, 
                     MEDIUM_NOT_PRESENT);
      return -1;
    } 
    
    hmsc->scsi_blk_addr_in_blks = (params[2] << 24) | \
      (params[3] << 16) | \
        (params[4] <<  8) | \
          params[5];
    
    hmsc->scsi_blk_len =  (params[7] <<  8) | \
      params[8];  
    
    
    
    if( SCSI_CheckAddressRange(pdev, lun, hmsc->scsi_blk_addr_in_blks, hmsc->scsi_blk_len) < 0)
    {
      return -1; /* error */
    }
    
    hmsc->bot_state = USBD_BOT_DATA_IN;
    hmsc->scsi_blk_len  *= hmsc->scsi_blk_size;
    
    /* cases 4,5 : Hi <> Dn */
    if (hmsc->cbw.dDataLength != hmsc->scsi_blk_len)
    {
      SCSI_SenseCode(pdev,
                     hmsc->cbw.bLUN, 
                     ILLEGAL_REQUEST, 
                     INVALID_CDB);
      return -1;
    }
  }
  hmsc->bot_data_length = MSC_MEDIA_PACKET;  
  
  return SCSI_ProcessRead(pdev, lun);
}

/**
* @brief  SCSI_Write10
*         Process Write10 command
* @param  lun: Logical unit number
* @param  params: Command parameters
* @retval status
*/

static int8_t SCSI_Write10 (USBD_HandleTypeDef  *pdev, uint8_t lun , uint8_t *params)
{
  USBD_MSC_BOT_HandleTypeDef  *hmsc = ((usbd_cdc_msc_hid_state_t*)pdev->pClassData)->msc;
  
  if (hmsc->bot_state == USBD_BOT_IDLE) /* Idle */
  {
    
    /* case 8 : Hi <> Do */
    
    if ((hmsc->cbw.bmFlags & 0x80) == 0x80)
    {
      SCSI_SenseCode(pdev,
                     hmsc->cbw.bLUN, 
                     ILLEGAL_REQUEST, 
                     INVALID_CDB);
      return -1;
    }
    
    /* Check whether Media is ready */
    if(((USBD_StorageTypeDef *)pdev->pUserData)->IsReady(lun) !=0 )
    {
      SCSI_SenseCode(pdev,
                     lun,
                     NOT_READY, 
                     MEDIUM_NOT_PRESENT);
      return -1;
    } 
    
    /* Check If media is write-protected */
    if(((USBD_StorageTypeDef *)pdev->pUserData)->IsWriteProtected(lun) !=0 )
    {
      SCSI_SenseCode(pdev,
                     lun,
                     NOT_READY, 
                     WRITE_PROTECTED);
      return -1;
    } 
    
    
    hmsc->scsi_blk_addr_in_blks = (params[2] << 24) | \
      (params[3] << 16) | \
        (params[4] <<  8) | \
          params[5];
    hmsc->scsi_blk_len = (params[7] <<  8) | \
      params[8];  
    
    /* check if LBA address is in the right range */
    if(SCSI_CheckAddressRange(pdev,
                              lun,
                              hmsc->scsi_blk_addr_in_blks,
                              hmsc->scsi_blk_len) < 0)
    {
      return -1; /* error */      
    }
    
    hmsc->scsi_blk_len  *= hmsc->scsi_blk_size;
    
    /* cases 3,11,13 : Hn,Ho <> D0 */
    if (hmsc->cbw.dDataLength != hmsc->scsi_blk_len)
    {
      SCSI_SenseCode(pdev,
                     hmsc->cbw.bLUN, 
                     ILLEGAL_REQUEST, 
                     INVALID_CDB);
      return -1;
    }
    
    /* Prepare EP to receive first data packet */
    hmsc->bot_state = USBD_BOT_DATA_OUT;  
    USBD_LL_PrepareReceive (pdev,
                      MSC_OUT_EP,
                      hmsc->bot_data, 
                      MIN (hmsc->scsi_blk_len, MSC_MEDIA_PACKET));  
  }
  else /* Write Process ongoing */
  {
    return SCSI_ProcessWrite(pdev, lun);
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

static int8_t SCSI_Verify10(USBD_HandleTypeDef  *pdev, uint8_t lun , uint8_t *params)
{
  USBD_MSC_BOT_HandleTypeDef  *hmsc = ((usbd_cdc_msc_hid_state_t*)pdev->pClassData)->msc;
  
  if ((params[1]& 0x02) == 0x02) 
  {
    SCSI_SenseCode (pdev,
                    lun, 
                    ILLEGAL_REQUEST, 
                    INVALID_FIELED_IN_COMMAND);
    return -1; /* Error, Verify Mode Not supported*/
  }
  
  hmsc->scsi_blk_addr_in_blks = (params[2] << 24) | (params[3] << 16) | (params[4] << 8) | params[5];
  hmsc->scsi_blk_len = (params[7] << 8) | params[8];

  if(SCSI_CheckAddressRange(pdev,
                            lun, 
                            hmsc->scsi_blk_addr_in_blks,
                            hmsc->scsi_blk_len) < 0)
  {
    return -1; /* error */      
  }
  hmsc->bot_data_length = 0;
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
static int8_t SCSI_CheckAddressRange (USBD_HandleTypeDef  *pdev, uint8_t lun , uint32_t blk_offset , uint16_t blk_nbr)
{
  USBD_MSC_BOT_HandleTypeDef  *hmsc = ((usbd_cdc_msc_hid_state_t*)pdev->pClassData)->msc;
  
  if ((blk_offset + blk_nbr) > hmsc->scsi_blk_nbr )
  {
    SCSI_SenseCode(pdev,
                   lun, 
                   ILLEGAL_REQUEST, 
                   ADDRESS_OUT_OF_RANGE);
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
static int8_t SCSI_ProcessRead (USBD_HandleTypeDef  *pdev, uint8_t lun)
{
  USBD_MSC_BOT_HandleTypeDef  *hmsc = ((usbd_cdc_msc_hid_state_t*)pdev->pClassData)->msc;
  uint32_t len;
  
  len = MIN(hmsc->scsi_blk_len , MSC_MEDIA_PACKET); 
  
  if( ((USBD_StorageTypeDef *)pdev->pUserData)->Read(lun ,
                              hmsc->bot_data, 
                              hmsc->scsi_blk_addr_in_blks,
                              len / hmsc->scsi_blk_size) < 0)
  {
    
    SCSI_SenseCode(pdev,
                   lun, 
                   HARDWARE_ERROR, 
                   UNRECOVERED_READ_ERROR);
    return -1; 
  }
  
  
  USBD_LL_Transmit (pdev, 
             MSC_IN_EP,
             hmsc->bot_data,
             len);
  
  
  hmsc->scsi_blk_addr_in_blks += len / hmsc->scsi_blk_size;
  hmsc->scsi_blk_len    -= len;  
  
  /* case 6 : Hi = Di */
  hmsc->csw.dDataResidue -= len;
  
  if (hmsc->scsi_blk_len == 0)
  {
    hmsc->bot_state = USBD_BOT_LAST_DATA_IN;
  }
  return 0;
}

/**
* @brief  SCSI_ProcessWrite
*         Handle Write Process
* @param  lun: Logical unit number
* @retval status
*/

static int8_t SCSI_ProcessWrite (USBD_HandleTypeDef  *pdev, uint8_t lun)
{
  uint32_t len;
  USBD_MSC_BOT_HandleTypeDef  *hmsc = ((usbd_cdc_msc_hid_state_t*)pdev->pClassData)->msc;
  
  len = MIN(hmsc->scsi_blk_len , MSC_MEDIA_PACKET); 
  
  if(((USBD_StorageTypeDef *)pdev->pUserData)->Write(lun ,
                              hmsc->bot_data, 
                              hmsc->scsi_blk_addr_in_blks,
                              len / hmsc->scsi_blk_size) < 0)
  {
    SCSI_SenseCode(pdev,
                   lun, 
                   HARDWARE_ERROR, 
                   WRITE_FAULT);     
    return -1; 
  }
  
  
  hmsc->scsi_blk_addr_in_blks += len / hmsc->scsi_blk_size;
  hmsc->scsi_blk_len   -= len; 
  
  /* case 12 : Ho = Do */
  hmsc->csw.dDataResidue -= len;
  
  if (hmsc->scsi_blk_len == 0)
  {
    MSC_BOT_SendCSW (pdev, USBD_CSW_CMD_PASSED);
  }
  else
  {
    /* Prapare EP to Receive next packet */
    USBD_LL_PrepareReceive (pdev,
                            MSC_OUT_EP,
                            hmsc->bot_data, 
                            MIN (hmsc->scsi_blk_len, MSC_MEDIA_PACKET)); 
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
