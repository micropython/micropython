/********************************** (C) COPYRIGHT  *******************************
* File Name          : ch32v30x_flash.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : This file contains all the functions prototypes for the FLASH  
*                      firmware library.
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/ 
#ifndef __CH32V30x_FLASH_H
#define __CH32V30x_FLASH_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "ch32v30x.h"

/* FLASH Status */
typedef enum
{ 
  FLASH_BUSY = 1,
  FLASH_ERROR_PG,
  FLASH_ERROR_WRP,
  FLASH_COMPLETE,
  FLASH_TIMEOUT
}FLASH_Status;


/* Write Protect */
#define FLASH_WRProt_Sectors0        ((uint32_t)0x00000001) /* Write protection of setor 0  */
#define FLASH_WRProt_Sectors1        ((uint32_t)0x00000002) /* Write protection of setor 0 */
#define FLASH_WRProt_Sectors2        ((uint32_t)0x00000004) /* Write protection of setor 0 */
#define FLASH_WRProt_Sectors3        ((uint32_t)0x00000008) /* Write protection of setor 0 */
#define FLASH_WRProt_Sectors4        ((uint32_t)0x00000010) /* Write protection of setor 0 */
#define FLASH_WRProt_Sectors5        ((uint32_t)0x00000020) /* Write protection of setor 0 */
#define FLASH_WRProt_Sectors6        ((uint32_t)0x00000040) /* Write protection of setor 0 */
#define FLASH_WRProt_Sectors7        ((uint32_t)0x00000080) /* Write protection of setor 0 */
#define FLASH_WRProt_Sectors8        ((uint32_t)0x00000100) /* Write protection of setor 0 */
#define FLASH_WRProt_Sectors9        ((uint32_t)0x00000200) /* Write protection of setor 0 */
#define FLASH_WRProt_Sectors10       ((uint32_t)0x00000400) /* Write protection of setor 0 */
#define FLASH_WRProt_Sectors11       ((uint32_t)0x00000800) /* Write protection of setor 0 */
#define FLASH_WRProt_Sectors12       ((uint32_t)0x00001000) /* Write protection of setor 0 */
#define FLASH_WRProt_Sectors13       ((uint32_t)0x00002000) /* Write protection of setor 0 */
#define FLASH_WRProt_Sectors14       ((uint32_t)0x00004000) /* Write protection of setor 0 */
#define FLASH_WRProt_Sectors15       ((uint32_t)0x00008000) /* Write protection of setor 0 */
#define FLASH_WRProt_Sectors16       ((uint32_t)0x00010000) /* Write protection of setor 0 */
#define FLASH_WRProt_Sectors17       ((uint32_t)0x00020000) /* Write protection of setor 0 */
#define FLASH_WRProt_Sectors18       ((uint32_t)0x00040000) /* Write protection of setor 0 */
#define FLASH_WRProt_Sectors19       ((uint32_t)0x00080000) /* Write protection of setor 0 */
#define FLASH_WRProt_Sectors20       ((uint32_t)0x00100000) /* Write protection of setor 0 */
#define FLASH_WRProt_Sectors21       ((uint32_t)0x00200000) /* Write protection of setor 0 */
#define FLASH_WRProt_Sectors22       ((uint32_t)0x00400000) /* Write protection of setor 0 */
#define FLASH_WRProt_Sectors23       ((uint32_t)0x00800000) /* Write protection of setor 0 */
#define FLASH_WRProt_Sectors24       ((uint32_t)0x01000000) /* Write protection of setor 0 */
#define FLASH_WRProt_Sectors25       ((uint32_t)0x02000000) /* Write protection of setor 0 */
#define FLASH_WRProt_Sectors26       ((uint32_t)0x04000000) /* Write protection of setor 0 */
#define FLASH_WRProt_Sectors27       ((uint32_t)0x08000000) /* Write protection of setor 0 */
#define FLASH_WRProt_Sectors28       ((uint32_t)0x10000000) /* Write protection of setor 0 */
#define FLASH_WRProt_Sectors29       ((uint32_t)0x20000000) /* Write protection of setor 0 */
#define FLASH_WRProt_Sectors30       ((uint32_t)0x40000000) /* Write protection of setor 0 */
#define FLASH_WRProt_Sectors31to127  ((uint32_t)0x80000000) /* Write protection of page 62 to 255 */

#define FLASH_WRProt_AllSectors      ((uint32_t)0xFFFFFFFF) /* Write protection of all Sectors */

/* Option_Bytes_IWatchdog */
#define OB_IWDG_SW                     ((uint16_t)0x0001)  /* Software IWDG selected */
#define OB_IWDG_HW                     ((uint16_t)0x0000)  /* Hardware IWDG selected */

/* Option_Bytes_nRST_STOP */
#define OB_STOP_NoRST                  ((uint16_t)0x0002) /* No reset generated when entering in STOP */
#define OB_STOP_RST                    ((uint16_t)0x0000) /* Reset generated when entering in STOP */

/* Option_Bytes_nRST_STDBY  */
#define OB_STDBY_NoRST                 ((uint16_t)0x0004) /* No reset generated when entering in STANDBY */
#define OB_STDBY_RST                   ((uint16_t)0x0000) /* Reset generated when entering in STANDBY */

/* FLASH_Interrupts */	
#define FLASH_IT_ERROR                 ((uint32_t)0x00000400)  /* FPEC error interrupt source */
#define FLASH_IT_EOP                   ((uint32_t)0x00001000)  /* End of FLASH Operation Interrupt source */
#define FLASH_IT_BANK1_ERROR           FLASH_IT_ERROR          /* FPEC BANK1 error interrupt source */
#define FLASH_IT_BANK1_EOP             FLASH_IT_EOP            /* End of FLASH BANK1 Operation Interrupt source */

/* FLASH_Flags */	
#define FLASH_FLAG_BSY                 ((uint32_t)0x00000001)  /* FLASH Busy flag */
#define FLASH_FLAG_EOP                 ((uint32_t)0x00000020)  /* FLASH End of Operation flag */
#define FLASH_FLAG_PGERR               ((uint32_t)0x00000004)  /* FLASH Program error flag */
#define FLASH_FLAG_WRPRTERR            ((uint32_t)0x00000010)  /* FLASH Write protected error flag */
#define FLASH_FLAG_OPTERR              ((uint32_t)0x00000001)  /* FLASH Option Byte error flag */

#define FLASH_FLAG_BANK1_BSY                 FLASH_FLAG_BSY       /* FLASH BANK1 Busy flag*/
#define FLASH_FLAG_BANK1_EOP                 FLASH_FLAG_EOP       /* FLASH BANK1 End of Operation flag */
#define FLASH_FLAG_BANK1_PGERR               FLASH_FLAG_PGERR     /* FLASH BANK1 Program error flag */
#define FLASH_FLAG_BANK1_WRPRTERR            FLASH_FLAG_WRPRTERR  /* FLASH BANK1 Write protected error flag */

/* FLASH_Enhance_CLK */
#define FLASH_Enhance_SYSTEM_HALF      ((uint32_t)0x00000000)   /* FLASH Enhance Clock = SYSTEM */
#define FLASH_Enhance_SYSTEM           ((uint32_t)0x02000000)   /* Enhance_CLK = SYSTEM/2 */ 
 
 
/*Functions used for all devices*/
void FLASH_Unlock(void);
void FLASH_Lock(void);
FLASH_Status FLASH_ErasePage(uint32_t Page_Address);
FLASH_Status FLASH_EraseAllPages(void);
FLASH_Status FLASH_EraseOptionBytes(void);
FLASH_Status FLASH_ProgramWord(uint32_t Address, uint32_t Data);
FLASH_Status FLASH_ProgramHalfWord(uint32_t Address, uint16_t Data);
FLASH_Status FLASH_ProgramOptionByteData(uint32_t Address, uint8_t Data);
FLASH_Status FLASH_EnableWriteProtection(uint32_t FLASH_Sectors);
FLASH_Status FLASH_ReadOutProtection(FunctionalState NewState);
FLASH_Status FLASH_UserOptionByteConfig(uint16_t OB_IWDG, uint16_t OB_STOP, uint16_t OB_STDBY);
uint32_t FLASH_GetUserOptionByte(void);
uint32_t FLASH_GetWriteProtectionOptionByte(void);
FlagStatus FLASH_GetReadOutProtectionStatus(void);
void FLASH_ITConfig(uint32_t FLASH_IT, FunctionalState NewState);
FlagStatus FLASH_GetFlagStatus(uint32_t FLASH_FLAG);
void FLASH_ClearFlag(uint32_t FLASH_FLAG);
FLASH_Status FLASH_GetStatus(void);
FLASH_Status FLASH_WaitForLastOperation(uint32_t Timeout);
void FLASH_Unlock_Fast(void);
void FLASH_Lock_Fast(void);
void FLASH_ErasePage_Fast(uint32_t Page_Address);
void FLASH_EraseBlock_32K_Fast(uint32_t Block_Address);
void FLASH_EraseBlock_64K_Fast(uint32_t Block_Address);
void FLASH_ProgramPage_Fast(uint32_t Page_Address, uint32_t* pbuf);
void FLASH_Enhance_Mode(uint32_t FLASH_Enhance_CLK, FunctionalState NewState);

/* New function used for all devices */
void FLASH_UnlockBank1(void);
void FLASH_LockBank1(void);
FLASH_Status FLASH_EraseAllBank1Pages(void);
FLASH_Status FLASH_GetBank1Status(void);
FLASH_Status FLASH_WaitForLastBank1Operation(uint32_t Timeout);

#ifdef __cplusplus
}
#endif


#endif 

