/**
  ******************************************************************************
  * @file    stm32n6xx_hal_mmc.h
  * @author  MCD Application Team
  * @brief   Header file of MMC HAL module.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32N6xx_HAL_MMC_H
#define STM32N6xx_HAL_MMC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_ll_sdmmc.h"

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */
#if defined (SDMMC1) || defined (SDMMC2)

/** @addtogroup MMC
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup MMC_Exported_Types MMC Exported Types
  * @{
  */

/** @defgroup MMC_Exported_Types_Group1 MMC State enumeration structure
  * @{
  */
typedef enum
{
  HAL_MMC_STATE_RESET                  = ((uint32_t)0x00000000U),  /*!< MMC not yet initialized or disabled  */
  HAL_MMC_STATE_READY                  = ((uint32_t)0x00000001U),  /*!< MMC initialized and ready for use    */
  HAL_MMC_STATE_TIMEOUT                = ((uint32_t)0x00000002U),  /*!< MMC Timeout state                    */
  HAL_MMC_STATE_BUSY                   = ((uint32_t)0x00000003U),  /*!< MMC process ongoing                  */
  HAL_MMC_STATE_PROGRAMMING            = ((uint32_t)0x00000004U),  /*!< MMC Programming State                */
  HAL_MMC_STATE_RECEIVING              = ((uint32_t)0x00000005U),  /*!< MMC Receinving State                 */
  HAL_MMC_STATE_TRANSFER               = ((uint32_t)0x00000006U),  /*!< MMC Transfer State                   */
  HAL_MMC_STATE_ERROR                  = ((uint32_t)0x0000000FU)   /*!< MMC is in error state                */
} HAL_MMC_StateTypeDef;
/**
  * @}
  */

/** @defgroup MMC_Exported_Types_Group2 MMC Card State enumeration structure
  * @{
  */
typedef uint32_t HAL_MMC_CardStateTypeDef;

#define HAL_MMC_CARD_IDLE           0x00000000U  /*!< Card is in idle state (can't be checked by CMD13)           */
#define HAL_MMC_CARD_READY          0x00000001U  /*!< Card state is ready (can't be checked by CMD13)             */
#define HAL_MMC_CARD_IDENTIFICATION 0x00000002U  /*!< Card is in identification state (can't be checked by CMD13) */
#define HAL_MMC_CARD_STANDBY        0x00000003U  /*!< Card is in standby state                                    */
#define HAL_MMC_CARD_TRANSFER       0x00000004U  /*!< Card is in transfer state                                   */
#define HAL_MMC_CARD_SENDING        0x00000005U  /*!< Card is sending an operation                                */
#define HAL_MMC_CARD_RECEIVING      0x00000006U  /*!< Card is receiving operation information                     */
#define HAL_MMC_CARD_PROGRAMMING    0x00000007U  /*!< Card is in programming state                                */
#define HAL_MMC_CARD_DISCONNECTED   0x00000008U  /*!< Card is disconnected                                        */
#define HAL_MMC_CARD_BUSTEST        0x00000009U  /*!< Card is in bus test state                                   */
#define HAL_MMC_CARD_SLEEP          0x0000000AU  /*!< Card is in sleep state (can't be checked by CMD13)          */
#define HAL_MMC_CARD_ERROR          0x000000FFU  /*!< Card response Error (can't be checked by CMD13)             */
/**
  * @}
  */

/** @defgroup MMC_Exported_Types_Group3 MMC Handle Structure definition
  * @{
  */
#define MMC_InitTypeDef      SDMMC_InitTypeDef
#define MMC_TypeDef          SDMMC_TypeDef

/**
  * @brief  MMC Card Information Structure definition
  */
typedef struct
{
  uint32_t CardType;                     /*!< Specifies the card Type                         */

  uint32_t Class;                        /*!< Specifies the class of the card class           */

  uint32_t RelCardAdd;                   /*!< Specifies the Relative Card Address             */

  uint32_t BlockNbr;                     /*!< Specifies the Card Capacity in blocks           */

  uint32_t BlockSize;                    /*!< Specifies one block size in bytes               */

  uint32_t LogBlockNbr;                  /*!< Specifies the Card logical Capacity in blocks   */

  uint32_t LogBlockSize;                 /*!< Specifies logical block size in bytes           */

} HAL_MMC_CardInfoTypeDef;

/**
  * @brief  MMC handle Structure definition
  */
#if defined (USE_HAL_MMC_REGISTER_CALLBACKS) && (USE_HAL_MMC_REGISTER_CALLBACKS == 1U)
typedef struct __MMC_HandleTypeDef
#else
typedef struct
#endif /* USE_HAL_MMC_REGISTER_CALLBACKS */
{
  MMC_TypeDef                  *Instance;        /*!< MMC registers base address           */

  MMC_InitTypeDef              Init;             /*!< MMC required parameters              */

  HAL_LockTypeDef              Lock;             /*!< MMC locking object                   */

  const uint8_t                *pTxBuffPtr;      /*!< Pointer to MMC Tx transfer Buffer    */

  uint32_t                     TxXferSize;       /*!< MMC Tx Transfer size                 */

  uint8_t                      *pRxBuffPtr;      /*!< Pointer to MMC Rx transfer Buffer    */

  uint32_t                     RxXferSize;       /*!< MMC Rx Transfer size                 */

  __IO uint32_t                Context;          /*!< MMC transfer context                 */

  __IO HAL_MMC_StateTypeDef    State;            /*!< MMC card State                       */

  __IO uint32_t                ErrorCode;        /*!< MMC Card Error codes                 */

  __IO uint16_t                RPMBErrorCode;    /*!< MMC RPMB Area Error codes            */

  HAL_MMC_CardInfoTypeDef      MmcCard;          /*!< MMC Card information                 */

  uint32_t                     CSD[4U];          /*!< MMC card specific data table         */

  uint32_t                     CID[4U];          /*!< MMC card identification number table */

  uint32_t                     Ext_CSD[128];

#if defined (USE_HAL_MMC_REGISTER_CALLBACKS) && (USE_HAL_MMC_REGISTER_CALLBACKS == 1U)
  void (* TxCpltCallback)(struct __MMC_HandleTypeDef *hmmc);
  void (* RxCpltCallback)(struct __MMC_HandleTypeDef *hmmc);
  void (* ErrorCallback)(struct __MMC_HandleTypeDef *hmmc);
  void (* AbortCpltCallback)(struct __MMC_HandleTypeDef *hmmc);
  void (* Read_DMALnkLstBufCpltCallback)(struct __MMC_HandleTypeDef *hmmc);
  void (* Write_DMALnkLstBufCpltCallback)(struct __MMC_HandleTypeDef *hmmc);

  void (* MspInitCallback)(struct __MMC_HandleTypeDef *hmmc);
  void (* MspDeInitCallback)(struct __MMC_HandleTypeDef *hmmc);
#endif /* USE_HAL_MMC_REGISTER_CALLBACKS */
} MMC_HandleTypeDef;


/**
  * @}
  */

/** @defgroup MMC_Exported_Types_Group4 Card Specific Data: CSD Register
  * @{
  */
typedef struct
{
  __IO uint8_t  CSDStruct;            /*!< CSD structure                         */
  __IO uint8_t  SysSpecVersion;       /*!< System specification version          */
  __IO uint8_t  Reserved1;            /*!< Reserved                              */
  __IO uint8_t  TAAC;                 /*!< Data read access time 1               */
  __IO uint8_t  NSAC;                 /*!< Data read access time 2 in CLK cycles */
  __IO uint8_t  MaxBusClkFrec;        /*!< Max. bus clock frequency              */
  __IO uint16_t CardComdClasses;      /*!< Card command classes                  */
  __IO uint8_t  RdBlockLen;           /*!< Max. read data block length           */
  __IO uint8_t  PartBlockRead;        /*!< Partial blocks for read allowed       */
  __IO uint8_t  WrBlockMisalign;      /*!< Write block misalignment              */
  __IO uint8_t  RdBlockMisalign;      /*!< Read block misalignment               */
  __IO uint8_t  DSRImpl;              /*!< DSR implemented                       */
  __IO uint8_t  Reserved2;            /*!< Reserved                              */
  __IO uint32_t DeviceSize;           /*!< Device Size                           */
  __IO uint8_t  MaxRdCurrentVDDMin;   /*!< Max. read current @ VDD min           */
  __IO uint8_t  MaxRdCurrentVDDMax;   /*!< Max. read current @ VDD max           */
  __IO uint8_t  MaxWrCurrentVDDMin;   /*!< Max. write current @ VDD min          */
  __IO uint8_t  MaxWrCurrentVDDMax;   /*!< Max. write current @ VDD max          */
  __IO uint8_t  DeviceSizeMul;        /*!< Device size multiplier                */
  __IO uint8_t  EraseGrSize;          /*!< Erase group size                      */
  __IO uint8_t  EraseGrMul;           /*!< Erase group size multiplier           */
  __IO uint8_t  WrProtectGrSize;      /*!< Write protect group size              */
  __IO uint8_t  WrProtectGrEnable;    /*!< Write protect group enable            */
  __IO uint8_t  ManDeflECC;           /*!< Manufacturer default ECC              */
  __IO uint8_t  WrSpeedFact;          /*!< Write speed factor                    */
  __IO uint8_t  MaxWrBlockLen;        /*!< Max. write data block length          */
  __IO uint8_t  WriteBlockPaPartial;  /*!< Partial blocks for write allowed      */
  __IO uint8_t  Reserved3;            /*!< Reserved                              */
  __IO uint8_t  ContentProtectAppli;  /*!< Content protection application        */
  __IO uint8_t  FileFormatGroup;      /*!< File format group                     */
  __IO uint8_t  CopyFlag;             /*!< Copy flag (OTP)                       */
  __IO uint8_t  PermWrProtect;        /*!< Permanent write protection            */
  __IO uint8_t  TempWrProtect;        /*!< Temporary write protection            */
  __IO uint8_t  FileFormat;           /*!< File format                           */
  __IO uint8_t  ECC;                  /*!< ECC code                              */
  __IO uint8_t  CSD_CRC;              /*!< CSD CRC                               */
  __IO uint8_t  Reserved4;            /*!< Always 1                              */

} HAL_MMC_CardCSDTypeDef;
/**
  * @}
  */

/** @defgroup MMC_Exported_Types_Group5 Card Identification Data: CID Register
  * @{
  */
typedef struct
{
  __IO uint8_t  ManufacturerID;  /*!< Manufacturer ID       */
  __IO uint16_t OEM_AppliID;     /*!< OEM/Application ID    */
  __IO uint32_t ProdName1;       /*!< Product Name part1    */
  __IO uint8_t  ProdName2;       /*!< Product Name part2    */
  __IO uint8_t  ProdRev;         /*!< Product Revision      */
  __IO uint32_t ProdSN;          /*!< Product Serial Number */
  __IO uint8_t  Reserved1;       /*!< Reserved1             */
  __IO uint16_t ManufactDate;    /*!< Manufacturing Date    */
  __IO uint8_t  CID_CRC;         /*!< CID CRC               */
  __IO uint8_t  Reserved2;       /*!< Always 1              */

} HAL_MMC_CardCIDTypeDef;
/**
  * @}
  */

#if defined (USE_HAL_MMC_REGISTER_CALLBACKS) && (USE_HAL_MMC_REGISTER_CALLBACKS == 1U)
/** @defgroup MMC_Exported_Types_Group6 MMC Callback ID enumeration definition
  * @{
  */
typedef enum
{
  HAL_MMC_TX_CPLT_CB_ID                 = 0x00U,  /*!< MMC Tx Complete Callback ID                     */
  HAL_MMC_RX_CPLT_CB_ID                 = 0x01U,  /*!< MMC Rx Complete Callback ID                     */
  HAL_MMC_ERROR_CB_ID                   = 0x02U,  /*!< MMC Error Callback ID                           */
  HAL_MMC_ABORT_CB_ID                   = 0x03U,  /*!< MMC Abort Callback ID                           */
  HAL_MMC_READ_DMA_LNKLST_BUF_CPLT_CB_ID  = 0x04U,  /*!< MMC DMA Rx Linked List Node buffer Callback ID */
  HAL_MMC_WRITE_DMA_LNKLST_BUF_CPLT_CB_ID = 0x05U,  /*!< MMC DMA Tx Linked List Node buffer Callback ID */

  HAL_MMC_MSP_INIT_CB_ID                = 0x10U,  /*!< MMC MspInit Callback ID                         */
  HAL_MMC_MSP_DEINIT_CB_ID              = 0x11U   /*!< MMC MspDeInit Callback ID                       */
} HAL_MMC_CallbackIDTypeDef;
/**
  * @}
  */

/** @defgroup MMC_Exported_Types_Group7 MMC Callback pointer definition
  * @{
  */
typedef void (*pMMC_CallbackTypeDef)(MMC_HandleTypeDef *hmmc);
/**
  * @}
  */
#endif /* USE_HAL_MMC_REGISTER_CALLBACKS */
/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup MMC_Exported_Constants Exported Constants
  * @{
  */

#define MMC_BLOCKSIZE   ((uint32_t)512U) /*!< Block size is 512 bytes */

/** @defgroup MMC_Exported_Constansts_Group1 MMC Error status enumeration Structure definition
  * @{
  */
#define HAL_MMC_ERROR_NONE                           SDMMC_ERROR_NONE                    /*!< No error                                                      */
#define HAL_MMC_ERROR_CMD_CRC_FAIL                   SDMMC_ERROR_CMD_CRC_FAIL            /*!< Command response received (but CRC check failed)              */
#define HAL_MMC_ERROR_DATA_CRC_FAIL                  SDMMC_ERROR_DATA_CRC_FAIL           /*!< Data block sent/received (CRC check failed)                   */
#define HAL_MMC_ERROR_CMD_RSP_TIMEOUT                SDMMC_ERROR_CMD_RSP_TIMEOUT         /*!< Command response timeout                                      */
#define HAL_MMC_ERROR_DATA_TIMEOUT                   SDMMC_ERROR_DATA_TIMEOUT            /*!< Data timeout                                                  */
#define HAL_MMC_ERROR_TX_UNDERRUN                    SDMMC_ERROR_TX_UNDERRUN             /*!< Transmit FIFO underrun                                        */
#define HAL_MMC_ERROR_RX_OVERRUN                     SDMMC_ERROR_RX_OVERRUN              /*!< Receive FIFO overrun                                          */
#define HAL_MMC_ERROR_ADDR_MISALIGNED                SDMMC_ERROR_ADDR_MISALIGNED         /*!< Misaligned address                                            */
#define HAL_MMC_ERROR_BLOCK_LEN_ERR                  SDMMC_ERROR_BLOCK_LEN_ERR           /*!< Transferred block length is not allowed for the card or the   */
/*!< number of transferred bytes does not match the block length   */
#define HAL_MMC_ERROR_ERASE_SEQ_ERR                  SDMMC_ERROR_ERASE_SEQ_ERR           /*!< An error in the sequence of erase command occurs              */
#define HAL_MMC_ERROR_BAD_ERASE_PARAM                SDMMC_ERROR_BAD_ERASE_PARAM         /*!< An invalid selection for erase groups                         */
#define HAL_MMC_ERROR_WRITE_PROT_VIOLATION           SDMMC_ERROR_WRITE_PROT_VIOLATION    /*!< Attempt to program a write protect block                      */
#define HAL_MMC_ERROR_LOCK_UNLOCK_FAILED             SDMMC_ERROR_LOCK_UNLOCK_FAILED      /*!< Sequence or password error has been detected in unlock        */
/*!< command or if there was an attempt to access a locked card    */
#define HAL_MMC_ERROR_COM_CRC_FAILED                 SDMMC_ERROR_COM_CRC_FAILED          /*!< CRC check of the previous command failed                      */
#define HAL_MMC_ERROR_ILLEGAL_CMD                    SDMMC_ERROR_ILLEGAL_CMD             /*!< Command is not legal for the card state                       */
#define HAL_MMC_ERROR_CARD_ECC_FAILED                SDMMC_ERROR_CARD_ECC_FAILED         /*!< Card internal ECC was applied but failed to correct the data  */
#define HAL_MMC_ERROR_CC_ERR                         SDMMC_ERROR_CC_ERR                  /*!< Internal card controller error                                */
#define HAL_MMC_ERROR_GENERAL_UNKNOWN_ERR            SDMMC_ERROR_GENERAL_UNKNOWN_ERR     /*!< General or unknown error                                      */
#define HAL_MMC_ERROR_STREAM_READ_UNDERRUN           SDMMC_ERROR_STREAM_READ_UNDERRUN    /*!< The card could not sustain data reading in stream rmode       */
#define HAL_MMC_ERROR_STREAM_WRITE_OVERRUN           SDMMC_ERROR_STREAM_WRITE_OVERRUN    /*!< The card could not sustain data programming in stream mode    */
#define HAL_MMC_ERROR_CID_CSD_OVERWRITE              SDMMC_ERROR_CID_CSD_OVERWRITE       /*!< CID/CSD overwrite error                                       */
#define HAL_MMC_ERROR_WP_ERASE_SKIP                  SDMMC_ERROR_WP_ERASE_SKIP           /*!< Only partial address space was erased                         */
#define HAL_MMC_ERROR_CARD_ECC_DISABLED              SDMMC_ERROR_CARD_ECC_DISABLED       /*!< Command has been executed without using internal ECC          */
#define HAL_MMC_ERROR_ERASE_RESET                    SDMMC_ERROR_ERASE_RESET             /*!< Erase sequence was cleared before executing because an out    */
/*!< of erase sequence command was received                        */
#define HAL_MMC_ERROR_AKE_SEQ_ERR                    SDMMC_ERROR_AKE_SEQ_ERR             /*!< Error in sequence of authentication                           */
#define HAL_MMC_ERROR_INVALID_VOLTRANGE              SDMMC_ERROR_INVALID_VOLTRANGE       /*!< Error in case of invalid voltage range                        */
#define HAL_MMC_ERROR_ADDR_OUT_OF_RANGE              SDMMC_ERROR_ADDR_OUT_OF_RANGE       /*!< Error when addressed block is out of range                    */
#define HAL_MMC_ERROR_REQUEST_NOT_APPLICABLE         SDMMC_ERROR_REQUEST_NOT_APPLICABLE  /*!< Error when command request is not applicable                  */
#define HAL_MMC_ERROR_PARAM                          SDMMC_ERROR_INVALID_PARAMETER       /*!< the used parameter is not valid                               */
#define HAL_MMC_ERROR_UNSUPPORTED_FEATURE            SDMMC_ERROR_UNSUPPORTED_FEATURE     /*!< Error when feature is not insupported                         */
#define HAL_MMC_ERROR_BUSY                           SDMMC_ERROR_BUSY                    /*!< Error when transfer process is busy                           */
#define HAL_MMC_ERROR_DMA                            SDMMC_ERROR_DMA                     /*!< Error while DMA transfer                                      */
#define HAL_MMC_ERROR_TIMEOUT                        SDMMC_ERROR_TIMEOUT                 /*!< Timeout error                                                 */
/*!< response results after operating with RPMB partition          */
#define HAL_MMC_ERROR_RPMB_OPERATION_OK              0x0000U                             /*!< Operation OK                                                  */
#define HAL_MMC_ERROR_RPMB_GENERAL_FAILURE           0x0001U                             /*!< General failure                                               */
#define HAL_MMC_ERROR_RPMB_AUTHENTICATION_FAILURE    0x0002U                             /*!< Authentication failure                                        */
#define HAL_MMC_ERROR_RPMB_COUNTER_FAILURE           0x0003U                             /*!< Counter failure                                               */
#define HAL_MMC_ERROR_RPMB_ADDRESS_FAILURE           0x0004U                             /*!< Address failure                                               */
#define HAL_MMC_ERROR_RPMB_WRITE_FAILURE             0x0005U                             /*!< Write failure                                                 */
#define HAL_MMC_ERROR_RPMB_READ_FAILURE              0x0006U                             /*!< Read failure                                                  */
#define HAL_MMC_ERROR_RPMB_KEY_NOT_YET_PROG          0x0007U                             /*!< Authentication Key not yet programmed                         */
#define HAL_MMC_ERROR_RPMB_COUNTER_EXPIRED           0x0080U                             /*!< Write Counter has expired i.e. reached its max value          */

#if defined (USE_HAL_MMC_REGISTER_CALLBACKS) && (USE_HAL_MMC_REGISTER_CALLBACKS == 1U)
#define HAL_MMC_ERROR_INVALID_CALLBACK              SDMMC_ERROR_INVALID_PARAMETER        /*!< Invalid callback error                                        */
#endif /* USE_HAL_MMC_REGISTER_CALLBACKS */
/**
  * @}
  */

/** @defgroup MMC_Exported_Constansts_Group2 MMC context enumeration
  * @{
  */
#define   MMC_CONTEXT_NONE                 ((uint32_t)0x00000000U)  /*!< None                             */
#define   MMC_CONTEXT_READ_SINGLE_BLOCK    ((uint32_t)0x00000001U)  /*!< Read single block operation      */
#define   MMC_CONTEXT_READ_MULTIPLE_BLOCK  ((uint32_t)0x00000002U)  /*!< Read multiple blocks operation   */
#define   MMC_CONTEXT_WRITE_SINGLE_BLOCK   ((uint32_t)0x00000010U)  /*!< Write single block operation     */
#define   MMC_CONTEXT_WRITE_MULTIPLE_BLOCK ((uint32_t)0x00000020U)  /*!< Write multiple blocks operation  */
#define   MMC_CONTEXT_IT                   ((uint32_t)0x00000008U)  /*!< Process in Interrupt mode        */
#define   MMC_CONTEXT_DMA                  ((uint32_t)0x00000080U)  /*!< Process in DMA mode              */

/**
  * @}
  */

/** @defgroup MMC_Exported_Constansts_Group3 MMC Voltage mode
  * @{
  */
/**
  * @brief
  */
#define MMC_HIGH_VOLTAGE_RANGE         0x80FF8000U  /*!< High voltage in byte mode    */
#define MMC_DUAL_VOLTAGE_RANGE         0x80FF8080U  /*!< Dual voltage in byte mode    */
#define MMC_LOW_VOLTAGE_RANGE          0x80000080U  /*!< Low voltage in byte mode     */
#define EMMC_HIGH_VOLTAGE_RANGE        0xC0FF8000U  /*!< High voltage in sector mode  */
#define EMMC_DUAL_VOLTAGE_RANGE        0xC0FF8080U  /*!< Dual voltage in sector mode  */
#define EMMC_LOW_VOLTAGE_RANGE         0xC0000080U  /*!< Low voltage in sector mode   */
#define MMC_INVALID_VOLTAGE_RANGE      0x0001FF01U
/**
  * @}
  */

/** @defgroup MMC_Exported_Constansts_Group4 MMC Memory Cards
  * @{
  */
#define  MMC_LOW_CAPACITY_CARD     ((uint32_t)0x00000000U)   /*!< MMC Card Capacity <=2Gbytes   */
#define  MMC_HIGH_CAPACITY_CARD    ((uint32_t)0x00000001U)   /*!< MMC Card Capacity >2Gbytes and <2Tbytes   */

/**
  * @}
  */

/** @defgroup MMC_Exported_Constansts_Group5 MMC Erase Type
  * @{
  */
#define HAL_MMC_ERASE             0x00000000U  /*!< Erase the erase groups identified by CMD35 & 36                                   */
#define HAL_MMC_TRIM              0x00000001U  /*!< Erase the write blocks identified by CMD35 & 36                                   */
#define HAL_MMC_DISCARD           0x00000003U  /*!< Discard the write blocks identified by CMD35 & 36                                 */
#define HAL_MMC_SECURE_ERASE      0x80000000U  /*!< Perform a secure purge according SRT on the erase groups identified by CMD35 & 36 */
#define HAL_MMC_SECURE_TRIM_STEP1 0x80000001U  /*!< Mark the write blocks identified by CMD35 & 36 for secure erase                   */
#define HAL_MMC_SECURE_TRIM_STEP2 0x80008000U  /*!< Perform a secure purge according SRT on the write blocks previously identified    */

#define IS_MMC_ERASE_TYPE(TYPE) (((TYPE) == HAL_MMC_ERASE)             || \
                                 ((TYPE) == HAL_MMC_TRIM)              || \
                                 ((TYPE) == HAL_MMC_DISCARD)           || \
                                 ((TYPE) == HAL_MMC_SECURE_ERASE)      || \
                                 ((TYPE) == HAL_MMC_SECURE_TRIM_STEP1) || \
                                 ((TYPE) == HAL_MMC_SECURE_TRIM_STEP2))
/**
  * @}
  */

/** @defgroup MMC_Exported_Constansts_Group6 MMC Secure Removal Type
  * @{
  */
#define HAL_MMC_SRT_ERASE                   0x00000001U  /*!< Information removed by an erase                                                                */
#define HAL_MMC_SRT_WRITE_CHAR_ERASE        0x00000002U  /*!< Information removed by an overwriting with a character followed by an erase                    */
#define HAL_MMC_SRT_WRITE_CHAR_COMPL_RANDOM 0x00000004U  /*!< Information removed by an overwriting with a character, its complement then a random character */
#define HAL_MMC_SRT_VENDOR_DEFINED          0x00000008U  /*!< Information removed using a vendor defined                                                     */


#define IS_MMC_SRT_TYPE(TYPE) (((TYPE) == HAL_MMC_SRT_ERASE)                   || \
                               ((TYPE) == HAL_MMC_SRT_WRITE_CHAR_ERASE)        || \
                               ((TYPE) == HAL_MMC_SRT_WRITE_CHAR_COMPL_RANDOM) || \
                               ((TYPE) == HAL_MMC_SRT_VENDOR_DEFINED))
/**
  * @}
  */

/** @defgroup MMC_Exported_Constansts_Group7 MMC Partitions types
  * @{
  */
typedef uint32_t HAL_MMC_PartitionTypeDef;

#define HAL_MMC_USER_AREA_PARTITION              0x00000000U   /*!< User area partition */
#define HAL_MMC_BOOT_PARTITION1                  0x00000100U   /*!< Boot partition 1    */
#define HAL_MMC_BOOT_PARTITION2                  0x00000200U   /*!< Boot partition 2    */
#define HAL_MMC_RPMB_PARTITION                   0x00000300U   /*!< RPMB partition      */
/**
  * @}
  */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
/** @defgroup MMC_Exported_macros MMC Exported Macros
  *  @brief macros to handle interrupts and specific clock configurations
  * @{
  */
/** @brief Reset MMC handle state.
  * @param  __HANDLE__ MMC Handle.
  * @retval None
  */
#if defined (USE_HAL_MMC_REGISTER_CALLBACKS) && (USE_HAL_MMC_REGISTER_CALLBACKS == 1U)
#define __HAL_MMC_RESET_HANDLE_STATE(__HANDLE__)           do {                                              \
                                                                (__HANDLE__)->State = HAL_MMC_STATE_RESET; \
                                                                (__HANDLE__)->MspInitCallback = NULL;       \
                                                                (__HANDLE__)->MspDeInitCallback = NULL;     \
                                                              } while(0)
#else
#define __HAL_MMC_RESET_HANDLE_STATE(__HANDLE__)           ((__HANDLE__)->State = HAL_MMC_STATE_RESET)
#endif /* USE_HAL_MMC_REGISTER_CALLBACKS */

/**
  * @brief  Enable the MMC device interrupt.
  * @param  __HANDLE__ MMC Handle.
  * @param  __INTERRUPT__ specifies the SDMMC interrupt sources to be enabled.
  *         This parameter can be one or a combination of the following values:
  *            @arg SDMMC_IT_CCRCFAIL:   Command response received (CRC check failed) interrupt
  *            @arg SDMMC_IT_DCRCFAIL:   Data block sent/received (CRC check failed) interrupt
  *            @arg SDMMC_IT_CTIMEOUT:   Command response timeout interrupt
  *            @arg SDMMC_IT_DTIMEOUT:   Data timeout interrupt
  *            @arg SDMMC_IT_TXUNDERR:   Transmit FIFO underrun error interrupt
  *            @arg SDMMC_IT_RXOVERR:    Received FIFO overrun error interrupt
  *            @arg SDMMC_IT_CMDREND:    Command response received (CRC check passed) interrupt
  *            @arg SDMMC_IT_CMDSENT:    Command sent (no response required) interrupt
  *            @arg SDMMC_IT_DATAEND:    Data end (data counter, DATACOUNT, is zero) interrupt
  *            @arg SDMMC_IT_DHOLD:      Data transfer Hold interrupt
  *            @arg SDMMC_IT_DBCKEND:    Data block sent/received (CRC check passed) interrupt
  *            @arg SDMMC_IT_DABORT:     Data transfer aborted by CMD12 interrupt
  *            @arg SDMMC_IT_TXFIFOHE:   Transmit FIFO Half Empty interrupt
  *            @arg SDMMC_IT_RXFIFOHF:   Receive FIFO Half Full interrupt
  *            @arg SDMMC_IT_RXFIFOF:    Receive FIFO full interrupt
  *            @arg SDMMC_IT_TXFIFOE:    Transmit FIFO empty interrupt
  *            @arg SDMMC_IT_BUSYD0END:  End of SDMMC_D0 Busy following a CMD response detected interrupt
  *            @arg SDMMC_IT_SDIOIT:     SD I/O interrupt received interrupt
  *            @arg SDMMC_IT_ACKFAIL:    Boot Acknowledgment received interrupt
  *            @arg SDMMC_IT_ACKTIMEOUT: Boot Acknowledgment timeout interrupt
  *            @arg SDMMC_IT_VSWEND:     Voltage switch critical timing section completion interrupt
  *            @arg SDMMC_IT_CKSTOP:     SDMMC_CK stopped in Voltage switch procedure interrupt
  *            @arg SDMMC_IT_IDMABTC:    IDMA buffer transfer complete interrupt
  * @retval None
  */
#define __HAL_MMC_ENABLE_IT(__HANDLE__, __INTERRUPT__) __SDMMC_ENABLE_IT((__HANDLE__)->Instance, (__INTERRUPT__))

/**
  * @brief  Disable the MMC device interrupt.
  * @param  __HANDLE__ MMC Handle.
  * @param  __INTERRUPT__ specifies the SDMMC interrupt sources to be disabled.
  *          This parameter can be one or a combination of the following values:
  *            @arg SDMMC_IT_CCRCFAIL:   Command response received (CRC check failed) interrupt
  *            @arg SDMMC_IT_DCRCFAIL:   Data block sent/received (CRC check failed) interrupt
  *            @arg SDMMC_IT_CTIMEOUT:   Command response timeout interrupt
  *            @arg SDMMC_IT_DTIMEOUT:   Data timeout interrupt
  *            @arg SDMMC_IT_TXUNDERR:   Transmit FIFO underrun error interrupt
  *            @arg SDMMC_IT_RXOVERR:    Received FIFO overrun error interrupt
  *            @arg SDMMC_IT_CMDREND:    Command response received (CRC check passed) interrupt
  *            @arg SDMMC_IT_CMDSENT:    Command sent (no response required) interrupt
  *            @arg SDMMC_IT_DATAEND:    Data end (data counter, DATACOUNT, is zero) interrupt
  *            @arg SDMMC_IT_DHOLD:      Data transfer Hold interrupt
  *            @arg SDMMC_IT_DBCKEND:    Data block sent/received (CRC check passed) interrupt
  *            @arg SDMMC_IT_DABORT:     Data transfer aborted by CMD12 interrupt
  *            @arg SDMMC_IT_TXFIFOHE:   Transmit FIFO Half Empty interrupt
  *            @arg SDMMC_IT_RXFIFOHF:   Receive FIFO Half Full interrupt
  *            @arg SDMMC_IT_RXFIFOF:    Receive FIFO full interrupt
  *            @arg SDMMC_IT_TXFIFOE:    Transmit FIFO empty interrupt
  *            @arg SDMMC_IT_BUSYD0END:  End of SDMMC_D0 Busy following a CMD response detected interrupt
  *            @arg SDMMC_IT_SDIOIT:     SD I/O interrupt received interrupt
  *            @arg SDMMC_IT_ACKFAIL:    Boot Acknowledgment received interrupt
  *            @arg SDMMC_IT_ACKTIMEOUT: Boot Acknowledgment timeout interrupt
  *            @arg SDMMC_IT_VSWEND:     Voltage switch critical timing section completion interrupt
  *            @arg SDMMC_IT_CKSTOP:     SDMMC_CK stopped in Voltage switch procedure interrupt
  *            @arg SDMMC_IT_IDMABTC:    IDMA buffer transfer complete interrupt
  * @retval None
  */
#define __HAL_MMC_DISABLE_IT(__HANDLE__, __INTERRUPT__) __SDMMC_DISABLE_IT((__HANDLE__)->Instance, (__INTERRUPT__))

/**
  * @brief  Check whether the specified MMC flag is set or not.
  * @param  __HANDLE__ MMC Handle.
  * @param  __FLAG__ specifies the flag to check.
  *          This parameter can be one of the following values:
  *            @arg SDMMC_FLAG_CCRCFAIL:   Command response received (CRC check failed)
  *            @arg SDMMC_FLAG_DCRCFAIL:   Data block sent/received (CRC check failed)
  *            @arg SDMMC_FLAG_CTIMEOUT:   Command response timeout
  *            @arg SDMMC_FLAG_DTIMEOUT:   Data timeout
  *            @arg SDMMC_FLAG_TXUNDERR:   Transmit FIFO underrun error
  *            @arg SDMMC_FLAG_RXOVERR:    Received FIFO overrun error
  *            @arg SDMMC_FLAG_CMDREND:    Command response received (CRC check passed)
  *            @arg SDMMC_FLAG_CMDSENT:    Command sent (no response required)
  *            @arg SDMMC_FLAG_DATAEND:    Data end (data counter, DATACOUNT, is zero)
  *            @arg SDMMC_FLAG_DHOLD:      Data transfer Hold
  *            @arg SDMMC_FLAG_DBCKEND:    Data block sent/received (CRC check passed)
  *            @arg SDMMC_FLAG_DABORT:     Data transfer aborted by CMD12
  *            @arg SDMMC_FLAG_DPSMACT:    Data path state machine active
  *            @arg SDMMC_FLAG_CPSMACT:    Command path state machine active
  *            @arg SDMMC_FLAG_TXFIFOHE:   Transmit FIFO Half Empty
  *            @arg SDMMC_FLAG_RXFIFOHF:   Receive FIFO Half Full
  *            @arg SDMMC_FLAG_TXFIFOF:    Transmit FIFO full
  *            @arg SDMMC_FLAG_RXFIFOF:    Receive FIFO full
  *            @arg SDMMC_FLAG_TXFIFOE:    Transmit FIFO empty
  *            @arg SDMMC_FLAG_RXFIFOE:    Receive FIFO empty
  *            @arg SDMMC_FLAG_BUSYD0:     Inverted value of SDMMC_D0 line (Busy)
  *            @arg SDMMC_FLAG_BUSYD0END:  End of SDMMC_D0 Busy following a CMD response detected
  *            @arg SDMMC_FLAG_SDIOIT:     SD I/O interrupt received
  *            @arg SDMMC_FLAG_ACKFAIL:    Boot Acknowledgment received
  *            @arg SDMMC_FLAG_ACKTIMEOUT: Boot Acknowledgment timeout
  *            @arg SDMMC_FLAG_VSWEND:     Voltage switch critical timing section completion
  *            @arg SDMMC_FLAG_CKSTOP:     SDMMC_CK stopped in Voltage switch procedure
  *            @arg SDMMC_FLAG_IDMATE:     IDMA transfer error
  *            @arg SDMMC_FLAG_IDMABTC:    IDMA buffer transfer complete
  * @retval The new state of MMC FLAG (SET or RESET).
  */
#define __HAL_MMC_GET_FLAG(__HANDLE__, __FLAG__) __SDMMC_GET_FLAG((__HANDLE__)->Instance, (__FLAG__))

/**
  * @brief  Clear the MMC's pending flags.
  * @param  __HANDLE__ MMC Handle.
  * @param  __FLAG__ specifies the flag to clear.
  *          This parameter can be one or a combination of the following values:
  *            @arg SDMMC_FLAG_CCRCFAIL:   Command response received (CRC check failed)
  *            @arg SDMMC_FLAG_DCRCFAIL:   Data block sent/received (CRC check failed)
  *            @arg SDMMC_FLAG_CTIMEOUT:   Command response timeout
  *            @arg SDMMC_FLAG_DTIMEOUT:   Data timeout
  *            @arg SDMMC_FLAG_TXUNDERR:   Transmit FIFO underrun error
  *            @arg SDMMC_FLAG_RXOVERR:    Received FIFO overrun error
  *            @arg SDMMC_FLAG_CMDREND:    Command response received (CRC check passed)
  *            @arg SDMMC_FLAG_CMDSENT:    Command sent (no response required)
  *            @arg SDMMC_FLAG_DATAEND:    Data end (data counter, DATACOUNT, is zero)
  *            @arg SDMMC_FLAG_DHOLD:      Data transfer Hold
  *            @arg SDMMC_FLAG_DBCKEND:    Data block sent/received (CRC check passed)
  *            @arg SDMMC_FLAG_DABORT:     Data transfer aborted by CMD12
  *            @arg SDMMC_FLAG_BUSYD0END:  End of SDMMC_D0 Busy following a CMD response detected
  *            @arg SDMMC_FLAG_SDIOIT:     SD I/O interrupt received
  *            @arg SDMMC_FLAG_ACKFAIL:    Boot Acknowledgment received
  *            @arg SDMMC_FLAG_ACKTIMEOUT: Boot Acknowledgment timeout
  *            @arg SDMMC_FLAG_VSWEND:     Voltage switch critical timing section completion
  *            @arg SDMMC_FLAG_CKSTOP:     SDMMC_CK stopped in Voltage switch procedure
  *            @arg SDMMC_FLAG_IDMATE:     IDMA transfer error
  *            @arg SDMMC_FLAG_IDMABTC:    IDMA buffer transfer complete
  * @retval None
  */
#define __HAL_MMC_CLEAR_FLAG(__HANDLE__, __FLAG__) __SDMMC_CLEAR_FLAG((__HANDLE__)->Instance, (__FLAG__))

/**
  * @brief  Check whether the specified MMC interrupt has occurred or not.
  * @param  __HANDLE__ MMC Handle.
  * @param  __INTERRUPT__ specifies the SDMMC interrupt source to check.
  *          This parameter can be one of the following values:
  *            @arg SDMMC_IT_CCRCFAIL:   Command response received (CRC check failed) interrupt
  *            @arg SDMMC_IT_DCRCFAIL:   Data block sent/received (CRC check failed) interrupt
  *            @arg SDMMC_IT_CTIMEOUT:   Command response timeout interrupt
  *            @arg SDMMC_IT_DTIMEOUT:   Data timeout interrupt
  *            @arg SDMMC_IT_TXUNDERR:   Transmit FIFO underrun error interrupt
  *            @arg SDMMC_IT_RXOVERR:    Received FIFO overrun error interrupt
  *            @arg SDMMC_IT_CMDREND:    Command response received (CRC check passed) interrupt
  *            @arg SDMMC_IT_CMDSENT:    Command sent (no response required) interrupt
  *            @arg SDMMC_IT_DATAEND:    Data end (data counter, DATACOUNT, is zero) interrupt
  *            @arg SDMMC_IT_DHOLD:      Data transfer Hold interrupt
  *            @arg SDMMC_IT_DBCKEND:    Data block sent/received (CRC check passed) interrupt
  *            @arg SDMMC_IT_DABORT:     Data transfer aborted by CMD12 interrupt
  *            @arg SDMMC_IT_TXFIFOHE:   Transmit FIFO Half Empty interrupt
  *            @arg SDMMC_IT_RXFIFOHF:   Receive FIFO Half Full interrupt
  *            @arg SDMMC_IT_RXFIFOF:    Receive FIFO full interrupt
  *            @arg SDMMC_IT_TXFIFOE:    Transmit FIFO empty interrupt
  *            @arg SDMMC_IT_BUSYD0END:  End of SDMMC_D0 Busy following a CMD response detected interrupt
  *            @arg SDMMC_IT_SDIOIT:     SD I/O interrupt received interrupt
  *            @arg SDMMC_IT_ACKFAIL:    Boot Acknowledgment received interrupt
  *            @arg SDMMC_IT_ACKTIMEOUT: Boot Acknowledgment timeout interrupt
  *            @arg SDMMC_IT_VSWEND:     Voltage switch critical timing section completion interrupt
  *            @arg SDMMC_IT_CKSTOP:     SDMMC_CK stopped in Voltage switch procedure interrupt
  *            @arg SDMMC_IT_IDMABTC:    IDMA buffer transfer complete interrupt
  * @retval The new state of MMC IT (SET or RESET).
  */
#define __HAL_MMC_GET_IT(__HANDLE__, __INTERRUPT__) __SDMMC_GET_IT((__HANDLE__)->Instance, (__INTERRUPT__))

/**
  * @brief  Clear the MMC's interrupt pending bits.
  * @param  __HANDLE__ MMC Handle.
  * @param  __INTERRUPT__ specifies the interrupt pending bit to clear.
  *          This parameter can be one or a combination of the following values:
  *            @arg SDMMC_IT_CCRCFAIL:   Command response received (CRC check failed) interrupt
  *            @arg SDMMC_IT_DCRCFAIL:   Data block sent/received (CRC check failed) interrupt
  *            @arg SDMMC_IT_CTIMEOUT:   Command response timeout interrupt
  *            @arg SDMMC_IT_DTIMEOUT:   Data timeout interrupt
  *            @arg SDMMC_IT_TXUNDERR:   Transmit FIFO underrun error interrupt
  *            @arg SDMMC_IT_RXOVERR:    Received FIFO overrun error interrupt
  *            @arg SDMMC_IT_CMDREND:    Command response received (CRC check passed) interrupt
  *            @arg SDMMC_IT_CMDSENT:    Command sent (no response required) interrupt
  *            @arg SDMMC_IT_DATAEND:    Data end (data counter, DATACOUNT, is zero) interrupt
  *            @arg SDMMC_IT_DHOLD:      Data transfer Hold interrupt
  *            @arg SDMMC_IT_DBCKEND:    Data block sent/received (CRC check passed) interrupt
  *            @arg SDMMC_IT_DABORT:     Data transfer aborted by CMD12 interrupt
  *            @arg SDMMC_IT_TXFIFOHE:   Transmit FIFO Half Empty interrupt
  *            @arg SDMMC_IT_RXFIFOHF:   Receive FIFO Half Full interrupt
  *            @arg SDMMC_IT_RXFIFOF:    Receive FIFO full interrupt
  *            @arg SDMMC_IT_TXFIFOE:    Transmit FIFO empty interrupt
  *            @arg SDMMC_IT_BUSYD0END:  End of SDMMC_D0 Busy following a CMD response detected interrupt
  *            @arg SDMMC_IT_SDIOIT:     SD I/O interrupt received interrupt
  *            @arg SDMMC_IT_ACKFAIL:    Boot Acknowledgment received interrupt
  *            @arg SDMMC_IT_ACKTIMEOUT: Boot Acknowledgment timeout interrupt
  *            @arg SDMMC_IT_VSWEND:     Voltage switch critical timing section completion interrupt
  *            @arg SDMMC_IT_CKSTOP:     SDMMC_CK stopped in Voltage switch procedure interrupt
  *            @arg SDMMC_IT_IDMABTC:    IDMA buffer transfer complete interrupt
  * @retval None
  */
#define __HAL_MMC_CLEAR_IT(__HANDLE__, __INTERRUPT__) __SDMMC_CLEAR_IT((__HANDLE__)->Instance, (__INTERRUPT__))

/**
  * @}
  */

/* Include MMC HAL Extension module */
#include "stm32n6xx_hal_mmc_ex.h"

/* Exported functions --------------------------------------------------------*/
/** @defgroup MMC_Exported_Functions MMC Exported Functions
  * @{
  */

/** @defgroup MMC_Exported_Functions_Group1 Initialization and de-initialization functions
  * @{
  */
HAL_StatusTypeDef HAL_MMC_Init(MMC_HandleTypeDef *hmmc);
HAL_StatusTypeDef HAL_MMC_InitCard(MMC_HandleTypeDef *hmmc);
HAL_StatusTypeDef HAL_MMC_DeInit(MMC_HandleTypeDef *hmmc);
void HAL_MMC_MspInit(MMC_HandleTypeDef *hmmc);
void HAL_MMC_MspDeInit(MMC_HandleTypeDef *hmmc);

/**
  * @}
  */

/** @defgroup MMC_Exported_Functions_Group2 Input and Output operation functions
  * @{
  */
/* Blocking mode: Polling */
HAL_StatusTypeDef HAL_MMC_ReadBlocks(MMC_HandleTypeDef *hmmc, uint8_t *pData, uint32_t BlockAdd,
                                     uint32_t NumberOfBlocks,
                                     uint32_t Timeout);
HAL_StatusTypeDef HAL_MMC_WriteBlocks(MMC_HandleTypeDef *hmmc, const uint8_t *pData, uint32_t BlockAdd,
                                      uint32_t NumberOfBlocks, uint32_t Timeout);
HAL_StatusTypeDef HAL_MMC_Erase(MMC_HandleTypeDef *hmmc, uint32_t BlockStartAdd, uint32_t BlockEndAdd);
/* Non-Blocking mode: IT */
HAL_StatusTypeDef HAL_MMC_ReadBlocks_IT(MMC_HandleTypeDef *hmmc, uint8_t *pData, uint32_t BlockAdd,
                                        uint32_t NumberOfBlocks);
HAL_StatusTypeDef HAL_MMC_WriteBlocks_IT(MMC_HandleTypeDef *hmmc, const uint8_t *pData, uint32_t BlockAdd,
                                         uint32_t NumberOfBlocks);
/* Non-Blocking mode: DMA */
HAL_StatusTypeDef HAL_MMC_ReadBlocks_DMA(MMC_HandleTypeDef *hmmc, uint8_t *pData, uint32_t BlockAdd,
                                         uint32_t NumberOfBlocks);
HAL_StatusTypeDef HAL_MMC_WriteBlocks_DMA(MMC_HandleTypeDef *hmmc, const uint8_t *pData, uint32_t BlockAdd,
                                          uint32_t NumberOfBlocks);

void HAL_MMC_IRQHandler(MMC_HandleTypeDef *hmmc);

/* Callback in non blocking modes (DMA) */
void HAL_MMC_TxCpltCallback(MMC_HandleTypeDef *hmmc);
void HAL_MMC_RxCpltCallback(MMC_HandleTypeDef *hmmc);
void HAL_MMC_ErrorCallback(MMC_HandleTypeDef *hmmc);
void HAL_MMC_AbortCallback(MMC_HandleTypeDef *hmmc);

#if defined (USE_HAL_MMC_REGISTER_CALLBACKS) && (USE_HAL_MMC_REGISTER_CALLBACKS == 1U)
/* MMC callback registering/unregistering */
HAL_StatusTypeDef HAL_MMC_RegisterCallback(MMC_HandleTypeDef *hmmc, HAL_MMC_CallbackIDTypeDef CallbackId,
                                           pMMC_CallbackTypeDef pCallback);
HAL_StatusTypeDef HAL_MMC_UnRegisterCallback(MMC_HandleTypeDef *hmmc, HAL_MMC_CallbackIDTypeDef CallbackId);
#endif /* USE_HAL_MMC_REGISTER_CALLBACKS */
/**
  * @}
  */

/** @defgroup MMC_Exported_Functions_Group3 Peripheral Control functions
  * @{
  */
HAL_StatusTypeDef HAL_MMC_ConfigWideBusOperation(MMC_HandleTypeDef *hmmc, uint32_t WideMode);
HAL_StatusTypeDef HAL_MMC_ConfigSpeedBusOperation(MMC_HandleTypeDef *hmmc, uint32_t SpeedMode);
HAL_StatusTypeDef HAL_MMC_SwitchPartition(MMC_HandleTypeDef *hmmc, HAL_MMC_PartitionTypeDef Partition);
/**
  * @}
  */

/** @defgroup MMC_Exported_Functions_Group4 MMC card related functions
  * @{
  */
HAL_MMC_CardStateTypeDef HAL_MMC_GetCardState(MMC_HandleTypeDef *hmmc);
HAL_StatusTypeDef HAL_MMC_GetCardCID(const MMC_HandleTypeDef *hmmc, HAL_MMC_CardCIDTypeDef *pCID);
HAL_StatusTypeDef HAL_MMC_GetCardCSD(MMC_HandleTypeDef *hmmc, HAL_MMC_CardCSDTypeDef *pCSD);
HAL_StatusTypeDef HAL_MMC_GetCardInfo(const MMC_HandleTypeDef *hmmc, HAL_MMC_CardInfoTypeDef *pCardInfo);
HAL_StatusTypeDef HAL_MMC_GetCardExtCSD(MMC_HandleTypeDef *hmmc, uint32_t *pExtCSD, uint32_t Timeout);
/**
  * @}
  */

/** @defgroup MMC_Exported_Functions_Group5 Peripheral State and Errors functions
  * @{
  */
HAL_MMC_StateTypeDef HAL_MMC_GetState(const MMC_HandleTypeDef *hmmc);
uint32_t HAL_MMC_GetError(const MMC_HandleTypeDef *hmmc);
uint32_t HAL_MMC_GetRPMBError(const MMC_HandleTypeDef *hmmc);
/**
  * @}
  */

/** @defgroup MMC_Exported_Functions_Group6 Peripheral Abort management
  * @{
  */
HAL_StatusTypeDef HAL_MMC_Abort(MMC_HandleTypeDef *hmmc);
HAL_StatusTypeDef HAL_MMC_Abort_IT(MMC_HandleTypeDef *hmmc);
/**
  * @}
  */

/** @defgroup MMC_Exported_Functions_Group7 Peripheral Erase management
  * @{
  */
HAL_StatusTypeDef HAL_MMC_EraseSequence(MMC_HandleTypeDef *hmmc, uint32_t EraseType, uint32_t BlockStartAdd,
                                        uint32_t BlockEndAdd);
HAL_StatusTypeDef HAL_MMC_Sanitize(MMC_HandleTypeDef *hmmc);
HAL_StatusTypeDef HAL_MMC_ConfigSecRemovalType(MMC_HandleTypeDef *hmmc, uint32_t SRTMode);
HAL_StatusTypeDef HAL_MMC_GetSupportedSecRemovalType(MMC_HandleTypeDef *hmmc, uint32_t *SupportedSRT);
/**
  * @}
  */

/** @defgroup MMC_Exported_Functions_Group8 Peripheral Sleep management
  * @{
  */
HAL_StatusTypeDef HAL_MMC_SleepDevice(MMC_HandleTypeDef *hmmc);
HAL_StatusTypeDef HAL_MMC_AwakeDevice(MMC_HandleTypeDef *hmmc);
/**
  * @}
  */

/** @defgroup MMC_Exported_Functions_Group9 Replay Protected Memory Block management
  * @{
  */
HAL_StatusTypeDef HAL_MMC_RPMB_ProgramAuthenticationKey(MMC_HandleTypeDef *hmmc, const uint8_t *pKey, uint32_t Timeout);
HAL_StatusTypeDef HAL_MMC_RPMB_ProgramAuthenticationKey_IT(MMC_HandleTypeDef *hmmc, const uint8_t *pKey,
                                                           uint32_t Timeout);
uint32_t HAL_MMC_RPMB_GetWriteCounter(MMC_HandleTypeDef *hmmc, uint8_t *pNonce, uint32_t Timeout);
uint32_t HAL_MMC_RPMB_GetWriteCounter_IT(MMC_HandleTypeDef *hmmc, uint8_t *pNonce);
HAL_StatusTypeDef HAL_MMC_RPMB_WriteBlocks(MMC_HandleTypeDef *hmmc, const uint8_t *pData, uint16_t BlockAdd,
                                           uint16_t NumberOfBlocks, const uint8_t *pMAC, uint32_t Timeout);
HAL_StatusTypeDef HAL_MMC_RPMB_WriteBlocks_IT(MMC_HandleTypeDef *hmmc, const uint8_t *pData, uint16_t BlockAdd,
                                              uint16_t NumberOfBlocks, const uint8_t *pMAC);
HAL_StatusTypeDef HAL_MMC_RPMB_ReadBlocks(MMC_HandleTypeDef *hmmc, uint8_t *pData, uint16_t BlockAdd,
                                          uint16_t NumberOfBlocks, const uint8_t *pNonce, uint8_t *pMAC,
                                          uint32_t Timeout);
HAL_StatusTypeDef HAL_MMC_RPMB_ReadBlocks_IT(MMC_HandleTypeDef *hmmc, uint8_t *pData, uint16_t BlockAdd,
                                             uint16_t NumberOfBlocks, const uint8_t *pNonce, uint8_t *pMAC);

/**
  * @}
  */

/* Private types -------------------------------------------------------------*/
/** @defgroup MMC_Private_Types MMC Private Types
  * @{
  */

/**
  * @}
  */

/* Private defines -----------------------------------------------------------*/
/** @defgroup MMC_Private_Defines MMC Private Defines
  * @{
  */
#define MMC_EXT_CSD_DATA_SEC_SIZE_INDEX 61
#define MMC_EXT_CSD_DATA_SEC_SIZE_POS   8
/**
  * @}
  */

/* Private variables ---------------------------------------------------------*/
/** @defgroup MMC_Private_Variables MMC Private Variables
  * @{
  */

/**
  * @}
  */

/* Private constants ---------------------------------------------------------*/
/** @defgroup MMC_Private_Constants MMC Private Constants
  * @{
  */

/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/** @defgroup MMC_Private_Macros MMC Private Macros
  * @{
  */

/**
  * @}
  */

/* Private functions prototypes ----------------------------------------------*/
/** @defgroup MMC_Private_Functions_Prototypes MMC Private Functions Prototypes
  * @{
  */

/**
  * @}
  */

/* Private functions ---------------------------------------------------------*/
/** @defgroup MMC_Private_Functions MMC Private Functions
  * @{
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
#endif /* SDMMC1 || SDMMC2 */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif


#endif /* STM32N6xx_HAL_MMC_H */
