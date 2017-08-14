/**
  ******************************************************************************
  * @file    stm32l4xx_hal_sd.h
  * @author  MCD Application Team
  * @version V1.3.0
  * @date    29-January-2016
  * @brief   Header file of SD HAL module.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32L4xx_HAL_SD_H
#define __STM32L4xx_HAL_SD_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_ll_sdmmc.h"

/** @addtogroup STM32L4xx_HAL_Driver
  * @{
  */

/** @defgroup SD SD
  * @brief SD HAL module driver
  * @{
  */ 

/* Exported types ------------------------------------------------------------*/ 
/** @defgroup SD_Exported_Types SD Exported Types
  * @{
  */

/** @defgroup SD_Exported_Types_Group1 SD Handle Structure definition   
  * @{
  */
#define SD_InitTypeDef      SDMMC_InitTypeDef 
#define SD_TypeDef          SDMMC_TypeDef

typedef struct
{
  SD_TypeDef                   *Instance;        /*!< SDMMC register base address                     */
  
  SD_InitTypeDef               Init;             /*!< SD required parameters                         */
  
  HAL_LockTypeDef              Lock;             /*!< SD locking object                              */
  
  uint32_t                     CardType;         /*!< SD card type                                   */
  
  uint32_t                     RCA;              /*!< SD relative card address                       */
  
  uint32_t                     CSD[4];           /*!< SD card specific data table                    */
  
  uint32_t                     CID[4];           /*!< SD card identification number table            */
  
  __IO uint32_t                SdTransferCplt;   /*!< SD transfer complete flag in non blocking mode */
  
  __IO uint32_t                SdTransferErr;    /*!< SD transfer error flag in non blocking mode    */
  
  __IO uint32_t                DmaTransferCplt;  /*!< SD DMA transfer complete flag                  */
  
  __IO uint32_t                SdOperation;      /*!< SD transfer operation (read/write)             */
  
  DMA_HandleTypeDef            *hdmarx;          /*!< SD Rx DMA handle parameters                    */
  
  DMA_HandleTypeDef            *hdmatx;          /*!< SD Tx DMA handle parameters                    */
  
}SD_HandleTypeDef;
/** 
  * @}
  */

/** @defgroup SD_Exported_Types_Group2 Card Specific Data: CSD Register 
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
  __IO uint8_t  FileFormatGrouop;     /*!< File format group                     */
  __IO uint8_t  CopyFlag;             /*!< Copy flag (OTP)                       */
  __IO uint8_t  PermWrProtect;        /*!< Permanent write protection            */
  __IO uint8_t  TempWrProtect;        /*!< Temporary write protection            */
  __IO uint8_t  FileFormat;           /*!< File format                           */
  __IO uint8_t  ECC;                  /*!< ECC code                              */
  __IO uint8_t  CSD_CRC;              /*!< CSD CRC                               */
  __IO uint8_t  Reserved4;            /*!< Always 1                              */

}HAL_SD_CSDTypedef;
/** 
  * @}
  */

/** @defgroup SD_Exported_Types_Group3 Card Identification Data: CID Register
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

}HAL_SD_CIDTypedef;
/** 
  * @}
  */

/** @defgroup SD_Exported_Types_Group4 SD Card Status returned by ACMD13 
  * @{
  */
typedef struct
{
  __IO uint8_t  DAT_BUS_WIDTH;           /*!< Shows the currently defined data bus width                 */
  __IO uint8_t  SECURED_MODE;            /*!< Card is in secured mode of operation                       */
  __IO uint16_t SD_CARD_TYPE;            /*!< Carries information about card type                        */
  __IO uint32_t SIZE_OF_PROTECTED_AREA;  /*!< Carries information about the capacity of protected area   */
  __IO uint8_t  SPEED_CLASS;             /*!< Carries information about the speed class of the card      */
  __IO uint8_t  PERFORMANCE_MOVE;        /*!< Carries information about the card's performance move      */
  __IO uint8_t  AU_SIZE;                 /*!< Carries information about the card's allocation unit size  */
  __IO uint16_t ERASE_SIZE;              /*!< Determines the number of AUs to be erased in one operation */
  __IO uint8_t  ERASE_TIMEOUT;           /*!< Determines the timeout for any number of AU erase          */
  __IO uint8_t  ERASE_OFFSET;            /*!< Carries information about the erase offset                 */

}HAL_SD_CardStatusTypedef;
/** 
  * @}
  */

/** @defgroup SD_Exported_Types_Group5 SD Card information structure 
  * @{
  */
typedef struct
{
  HAL_SD_CSDTypedef   SD_csd;         /*!< SD card specific data register         */
  HAL_SD_CIDTypedef   SD_cid;         /*!< SD card identification number register */
  uint64_t            CardCapacity;   /*!< Card capacity                          */
  uint32_t            CardBlockSize;  /*!< Card block size                        */
  uint16_t            RCA;            /*!< SD relative card address               */
  uint8_t             CardType;       /*!< SD card type                           */

}HAL_SD_CardInfoTypedef;
/** 
  * @}
  */

/** @defgroup SD_Exported_Types_Group6 SD Error status enumeration Structure definition 
  * @{
  */
typedef enum
{
/** 
  * @brief  SD specific error defines  
  */   
  SD_CMD_CRC_FAIL                    = (1),   /*!< Command response received (but CRC check failed)              */
  SD_DATA_CRC_FAIL                   = (2),   /*!< Data block sent/received (CRC check failed)                   */
  SD_CMD_RSP_TIMEOUT                 = (3),   /*!< Command response timeout                                      */
  SD_DATA_TIMEOUT                    = (4),   /*!< Data timeout                                                  */
  SD_TX_UNDERRUN                     = (5),   /*!< Transmit FIFO underrun                                        */
  SD_RX_OVERRUN                      = (6),   /*!< Receive FIFO overrun                                          */
  SD_START_BIT_ERR                   = (7),   /*!< Start bit not detected on all data signals in wide bus mode   */
  SD_CMD_OUT_OF_RANGE                = (8),   /*!< Command's argument was out of range.                          */
  SD_ADDR_MISALIGNED                 = (9),   /*!< Misaligned address                                            */
  SD_BLOCK_LEN_ERR                   = (10),  /*!< Transferred block length is not allowed for the card or the number of transferred bytes does not match the block length */
  SD_ERASE_SEQ_ERR                   = (11),  /*!< An error in the sequence of erase command occurs.            */
  SD_BAD_ERASE_PARAM                 = (12),  /*!< An invalid selection for erase groups                        */
  SD_WRITE_PROT_VIOLATION            = (13),  /*!< Attempt to program a write protect block                     */
  SD_LOCK_UNLOCK_FAILED              = (14),  /*!< Sequence or password error has been detected in unlock command or if there was an attempt to access a locked card */
  SD_COM_CRC_FAILED                  = (15),  /*!< CRC check of the previous command failed                     */
  SD_ILLEGAL_CMD                     = (16),  /*!< Command is not legal for the card state                      */
  SD_CARD_ECC_FAILED                 = (17),  /*!< Card internal ECC was applied but failed to correct the data */
  SD_CC_ERROR                        = (18),  /*!< Internal card controller error                               */
  SD_GENERAL_UNKNOWN_ERROR           = (19),  /*!< General or unknown error                                     */
  SD_STREAM_READ_UNDERRUN            = (20),  /*!< The card could not sustain data transfer in stream read operation. */
  SD_STREAM_WRITE_OVERRUN            = (21),  /*!< The card could not sustain data programming in stream mode   */
  SD_CID_CSD_OVERWRITE               = (22),  /*!< CID/CSD overwrite error                                      */
  SD_WP_ERASE_SKIP                   = (23),  /*!< Only partial address space was erased                        */
  SD_CARD_ECC_DISABLED               = (24),  /*!< Command has been executed without using internal ECC         */
  SD_ERASE_RESET                     = (25),  /*!< Erase sequence was cleared before executing because an out of erase sequence command was received */
  SD_AKE_SEQ_ERROR                   = (26),  /*!< Error in sequence of authentication.                         */
  SD_INVALID_VOLTRANGE               = (27),
  SD_ADDR_OUT_OF_RANGE               = (28),
  SD_SWITCH_ERROR                    = (29),
  SD_SDMMC_DISABLED                  = (30),
  SD_SDMMC_FUNCTION_BUSY             = (31),
  SD_SDMMC_FUNCTION_FAILED           = (32),
  SD_SDMMC_UNKNOWN_FUNCTION          = (33),

/** 
  * @brief  Standard error defines   
  */ 
  SD_INTERNAL_ERROR                  = (34),
  SD_NOT_CONFIGURED                  = (35),
  SD_REQUEST_PENDING                 = (36),
  SD_REQUEST_NOT_APPLICABLE          = (37),
  SD_INVALID_PARAMETER               = (38),
  SD_UNSUPPORTED_FEATURE             = (39),
  SD_UNSUPPORTED_HW                  = (40),
  SD_ERROR                           = (41),
  SD_OK                              = (0) 

}HAL_SD_ErrorTypedef;
/** 
  * @}
  */

/** @defgroup SD_Exported_Types_Group7 SD Transfer state enumeration structure
  * @{
  */   
typedef enum
{
  SD_TRANSFER_OK    = 0,  /*!< Transfer success      */
  SD_TRANSFER_BUSY  = 1,  /*!< Transfer is occurring */
  SD_TRANSFER_ERROR = 2   /*!< Transfer failed       */

}HAL_SD_TransferStateTypedef;
/** 
  * @}
  */

/** @defgroup SD_Exported_Types_Group8 SD Card State enumeration structure
  * @{
  */   
typedef enum
{
  SD_CARD_READY                  = ((uint32_t)0x00000001),  /*!< Card state is ready                     */
  SD_CARD_IDENTIFICATION         = ((uint32_t)0x00000002),  /*!< Card is in identification state         */
  SD_CARD_STANDBY                = ((uint32_t)0x00000003),  /*!< Card is in standby state                */
  SD_CARD_TRANSFER               = ((uint32_t)0x00000004),  /*!< Card is in transfer state               */  
  SD_CARD_SENDING                = ((uint32_t)0x00000005),  /*!< Card is sending an operation            */
  SD_CARD_RECEIVING              = ((uint32_t)0x00000006),  /*!< Card is receiving operation information */
  SD_CARD_PROGRAMMING            = ((uint32_t)0x00000007),  /*!< Card is in programming state            */
  SD_CARD_DISCONNECTED           = ((uint32_t)0x00000008),  /*!< Card is disconnected                    */
  SD_CARD_ERROR                  = ((uint32_t)0x000000FF)   /*!< Card is in error state                  */

}HAL_SD_CardStateTypedef;
/** 
  * @}
  */

/** @defgroup SD_Exported_Types_Group9 SD Operation enumeration structure
  * @{
  */   
typedef enum
{
  SD_READ_SINGLE_BLOCK    = 0,  /*!< Read single block operation      */
  SD_READ_MULTIPLE_BLOCK  = 1,  /*!< Read multiple blocks operation   */
  SD_WRITE_SINGLE_BLOCK   = 2,  /*!< Write single block operation     */
  SD_WRITE_MULTIPLE_BLOCK = 3   /*!< Write multiple blocks operation  */

}HAL_SD_OperationTypedef;
/**
  * @}
  */

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup SD_Exported_Constants SD Exported Constants
  * @{
  */

/** 
  * @brief SD Commands Index 
  */
#define SD_CMD_GO_IDLE_STATE                       ((uint8_t)0)   /*!< Resets the SD memory card.                                                               */
#define SD_CMD_SEND_OP_COND                        ((uint8_t)1)   /*!< Sends host capacity support information and activates the card's initialization process. */
#define SD_CMD_ALL_SEND_CID                        ((uint8_t)2)   /*!< Asks any card connected to the host to send the CID numbers on the CMD line.             */
#define SD_CMD_SET_REL_ADDR                        ((uint8_t)3)   /*!< Asks the card to publish a new relative address (RCA).                                   */
#define SD_CMD_SET_DSR                             ((uint8_t)4)   /*!< Programs the DSR of all cards.                                                           */
#define SD_CMD_SDMMC_SEN_OP_COND                   ((uint8_t)5)   /*!< Sends host capacity support information (HCS) and asks the accessed card to send its 
                                                                       operating condition register (OCR) content in the response on the CMD line.              */
#define SD_CMD_HS_SWITCH                           ((uint8_t)6)   /*!< Checks switchable function (mode 0) and switch card function (mode 1).                   */
#define SD_CMD_SEL_DESEL_CARD                      ((uint8_t)7)   /*!< Selects the card by its own relative address and gets deselected by any other address    */
#define SD_CMD_HS_SEND_EXT_CSD                     ((uint8_t)8)   /*!< Sends SD Memory Card interface condition, which includes host supply voltage information 
                                                                       and asks the card whether card supports voltage.                                         */
#define SD_CMD_SEND_CSD                            ((uint8_t)9)   /*!< Addressed card sends its card specific data (CSD) on the CMD line.                       */
#define SD_CMD_SEND_CID                            ((uint8_t)10)  /*!< Addressed card sends its card identification (CID) on the CMD line.                      */
#define SD_CMD_READ_DAT_UNTIL_STOP                 ((uint8_t)11)  /*!< SD card doesn't support it.                                                              */
#define SD_CMD_STOP_TRANSMISSION                   ((uint8_t)12)  /*!< Forces the card to stop transmission.                                                    */
#define SD_CMD_SEND_STATUS                         ((uint8_t)13)  /*!< Addressed card sends its status register.                                                */
#define SD_CMD_HS_BUSTEST_READ                     ((uint8_t)14) 
#define SD_CMD_GO_INACTIVE_STATE                   ((uint8_t)15)  /*!< Sends an addressed card into the inactive state.                                         */
#define SD_CMD_SET_BLOCKLEN                        ((uint8_t)16)  /*!< Sets the block length (in bytes for SDSC) for all following block commands 
                                                                       (read, write, lock). Default block length is fixed to 512 Bytes. Not effective 
                                                                       for SDHS and SDXC.                                                                       */
#define SD_CMD_READ_SINGLE_BLOCK                   ((uint8_t)17)  /*!< Reads single block of size selected by SET_BLOCKLEN in case of SDSC, and a block of 
                                                                       fixed 512 bytes in case of SDHC and SDXC.                                                */
#define SD_CMD_READ_MULT_BLOCK                     ((uint8_t)18)  /*!< Continuously transfers data blocks from card to host until interrupted by 
                                                                       STOP_TRANSMISSION command.                                                               */
#define SD_CMD_HS_BUSTEST_WRITE                    ((uint8_t)19)  /*!< 64 bytes tuning pattern is sent for SDR50 and SDR104.                                    */
#define SD_CMD_WRITE_DAT_UNTIL_STOP                ((uint8_t)20)  /*!< Speed class control command.                                                             */
#define SD_CMD_SET_BLOCK_COUNT                     ((uint8_t)23)  /*!< Specify block count for CMD18 and CMD25.                                                 */
#define SD_CMD_WRITE_SINGLE_BLOCK                  ((uint8_t)24)  /*!< Writes single block of size selected by SET_BLOCKLEN in case of SDSC, and a block of 
                                                                       fixed 512 bytes in case of SDHC and SDXC.                                                */
#define SD_CMD_WRITE_MULT_BLOCK                    ((uint8_t)25)  /*!< Continuously writes blocks of data until a STOP_TRANSMISSION follows.                    */
#define SD_CMD_PROG_CID                            ((uint8_t)26)  /*!< Reserved for manufacturers.                                                              */
#define SD_CMD_PROG_CSD                            ((uint8_t)27)  /*!< Programming of the programmable bits of the CSD.                                         */
#define SD_CMD_SET_WRITE_PROT                      ((uint8_t)28)  /*!< Sets the write protection bit of the addressed group.                                    */
#define SD_CMD_CLR_WRITE_PROT                      ((uint8_t)29)  /*!< Clears the write protection bit of the addressed group.                                  */
#define SD_CMD_SEND_WRITE_PROT                     ((uint8_t)30)  /*!< Asks the card to send the status of the write protection bits.                           */
#define SD_CMD_SD_ERASE_GRP_START                  ((uint8_t)32)  /*!< Sets the address of the first write block to be erased. (For SD card only).              */
#define SD_CMD_SD_ERASE_GRP_END                    ((uint8_t)33)  /*!< Sets the address of the last write block of the continuous range to be erased.           */
#define SD_CMD_ERASE_GRP_START                     ((uint8_t)35)  /*!< Sets the address of the first write block to be erased. Reserved for each command 
                                                                       system set by switch function command (CMD6).                                            */
#define SD_CMD_ERASE_GRP_END                       ((uint8_t)36)  /*!< Sets the address of the last write block of the continuous range to be erased. 
                                                                       Reserved for each command system set by switch function command (CMD6).                  */
#define SD_CMD_ERASE                               ((uint8_t)38)  /*!< Reserved for SD security applications.                                                   */
#define SD_CMD_FAST_IO                             ((uint8_t)39)  /*!< SD card doesn't support it (Reserved).                                                   */
#define SD_CMD_GO_IRQ_STATE                        ((uint8_t)40)  /*!< SD card doesn't support it (Reserved).                                                   */
#define SD_CMD_LOCK_UNLOCK                         ((uint8_t)42)  /*!< Sets/resets the password or lock/unlock the card. The size of the data block is set by 
                                                                       the SET_BLOCK_LEN command.                                                               */
#define SD_CMD_APP_CMD                             ((uint8_t)55)  /*!< Indicates to the card that the next command is an application specific command rather 
                                                                       than a standard command.                                                                 */
#define SD_CMD_GEN_CMD                             ((uint8_t)56)  /*!< Used either to transfer a data block to the card or to get a data block from the card 
                                                                       for general purpose/application specific commands.                                       */
#define SD_CMD_NO_CMD                              ((uint8_t)64) 

/** 
  * @brief Following commands are SD Card Specific commands.
  *        SDMMC_APP_CMD should be sent before sending these commands. 
  */
#define SD_CMD_APP_SD_SET_BUSWIDTH                 ((uint8_t)6)   /*!< (ACMD6) Defines the data bus width to be used for data transfer. The allowed data bus 
                                                                       widths are given in SCR register.                                                          */
#define SD_CMD_SD_APP_STATUS                       ((uint8_t)13)  /*!< (ACMD13) Sends the SD status.                                                              */
#define SD_CMD_SD_APP_SEND_NUM_WRITE_BLOCKS        ((uint8_t)22)  /*!< (ACMD22) Sends the number of the written (without errors) write blocks. Responds with 
                                                                       32bit+CRC data block.                                                                      */
#define SD_CMD_SD_APP_OP_COND                      ((uint8_t)41)  /*!< (ACMD41) Sends host capacity support information (HCS) and asks the accessed card to 
                                                                       send its operating condition register (OCR) content in the response on the CMD line.       */
#define SD_CMD_SD_APP_SET_CLR_CARD_DETECT          ((uint8_t)42)  /*!< (ACMD42) Connects/Disconnects the 50 KOhm pull-up resistor on CD/DAT3 (pin 1) of the card. */
#define SD_CMD_SD_APP_SEND_SCR                     ((uint8_t)51)  /*!< Reads the SD Configuration Register (SCR).                                                 */
#define SD_CMD_SDMMC_RW_DIRECT                     ((uint8_t)52)  /*!< For SD I/O card only, reserved for security specification.                                 */
#define SD_CMD_SDMMC_RW_EXTENDED                   ((uint8_t)53)  /*!< For SD I/O card only, reserved for security specification.                                 */

/** 
  * @brief Following commands are SD Card Specific security commands.
  *        SD_CMD_APP_CMD should be sent before sending these commands. 
  */
#define SD_CMD_SD_APP_GET_MKB                      ((uint8_t)43)  /*!< For SD card only */
#define SD_CMD_SD_APP_GET_MID                      ((uint8_t)44)  /*!< For SD card only */
#define SD_CMD_SD_APP_SET_CER_RN1                  ((uint8_t)45)  /*!< For SD card only */
#define SD_CMD_SD_APP_GET_CER_RN2                  ((uint8_t)46)  /*!< For SD card only */
#define SD_CMD_SD_APP_SET_CER_RES2                 ((uint8_t)47)  /*!< For SD card only */
#define SD_CMD_SD_APP_GET_CER_RES1                 ((uint8_t)48)  /*!< For SD card only */
#define SD_CMD_SD_APP_SECURE_READ_MULTIPLE_BLOCK   ((uint8_t)18)  /*!< For SD card only */
#define SD_CMD_SD_APP_SECURE_WRITE_MULTIPLE_BLOCK  ((uint8_t)25)  /*!< For SD card only */
#define SD_CMD_SD_APP_SECURE_ERASE                 ((uint8_t)38)  /*!< For SD card only */
#define SD_CMD_SD_APP_CHANGE_SECURE_AREA           ((uint8_t)49)  /*!< For SD card only */
#define SD_CMD_SD_APP_SECURE_WRITE_MKB             ((uint8_t)48)  /*!< For SD card only */

/** 
  * @brief Supported SD Memory Cards 
  */
#define STD_CAPACITY_SD_CARD_V1_1             ((uint32_t)0x00000000)
#define STD_CAPACITY_SD_CARD_V2_0             ((uint32_t)0x00000001)
#define HIGH_CAPACITY_SD_CARD                 ((uint32_t)0x00000002)
#define MULTIMEDIA_CARD                       ((uint32_t)0x00000003)
#define SECURE_DIGITAL_IO_CARD                ((uint32_t)0x00000004)
#define HIGH_SPEED_MULTIMEDIA_CARD            ((uint32_t)0x00000005)
#define SECURE_DIGITAL_IO_COMBO_CARD          ((uint32_t)0x00000006)
#define HIGH_CAPACITY_MMC_CARD                ((uint32_t)0x00000007)
/**
  * @}
  */
  
/* Exported macro ------------------------------------------------------------*/
/** @defgroup SD_Exported_macros SD Exported Macros
 *  @brief macros to handle interrupts and specific clock configurations
 * @{
 */
 
/**
  * @brief  Enable the SD device.
  * @retval None
  */ 
#define __HAL_SD_SDMMC_ENABLE(__HANDLE__) __SDMMC_ENABLE((__HANDLE__)->Instance)

/**
  * @brief  Disable the SD device.
  * @retval None
  */
#define __HAL_SD_SDMMC_DISABLE(__HANDLE__) __SDMMC_DISABLE((__HANDLE__)->Instance)

/**
  * @brief  Enable the SDMMC DMA transfer.
  * @retval None
  */ 
#define __HAL_SD_SDMMC_DMA_ENABLE(__HANDLE__) __SDMMC_DMA_ENABLE((__HANDLE__)->Instance)

/**
  * @brief  Disable the SDMMC DMA transfer.
  * @retval None
  */
#define __HAL_SD_SDMMC_DMA_DISABLE(__HANDLE__)  __SDMMC_DMA_DISABLE((__HANDLE__)->Instance)
 
/**
  * @brief  Enable the SD device interrupt.
  * @param  __HANDLE__: SD Handle  
  * @param  __INTERRUPT__: specifies the SDMMC interrupt sources to be enabled.
  *         This parameter can be one or a combination of the following values:
  *            @arg SDMMC_IT_CCRCFAIL: Command response received (CRC check failed) interrupt
  *            @arg SDMMC_IT_DCRCFAIL: Data block sent/received (CRC check failed) interrupt
  *            @arg SDMMC_IT_CTIMEOUT: Command response timeout interrupt
  *            @arg SDMMC_IT_DTIMEOUT: Data timeout interrupt
  *            @arg SDMMC_IT_TXUNDERR: Transmit FIFO underrun error interrupt
  *            @arg SDMMC_IT_RXOVERR:  Received FIFO overrun error interrupt
  *            @arg SDMMC_IT_CMDREND:  Command response received (CRC check passed) interrupt
  *            @arg SDMMC_IT_CMDSENT:  Command sent (no response required) interrupt
  *            @arg SDMMC_IT_DATAEND:  Data end (data counter, SDIDCOUNT, is zero) interrupt
  *            @arg SDMMC_IT_DBCKEND:  Data block sent/received (CRC check passed) interrupt
  *            @arg SDMMC_IT_CMDACT:   Command transfer in progress interrupt
  *            @arg SDMMC_IT_TXACT:    Data transmit in progress interrupt
  *            @arg SDMMC_IT_RXACT:    Data receive in progress interrupt
  *            @arg SDMMC_IT_TXFIFOHE: Transmit FIFO Half Empty interrupt
  *            @arg SDMMC_IT_RXFIFOHF: Receive FIFO Half Full interrupt
  *            @arg SDMMC_IT_TXFIFOF:  Transmit FIFO full interrupt
  *            @arg SDMMC_IT_RXFIFOF:  Receive FIFO full interrupt
  *            @arg SDMMC_IT_TXFIFOE:  Transmit FIFO empty interrupt
  *            @arg SDMMC_IT_RXFIFOE:  Receive FIFO empty interrupt
  *            @arg SDMMC_IT_TXDAVL:   Data available in transmit FIFO interrupt
  *            @arg SDMMC_IT_RXDAVL:   Data available in receive FIFO interrupt
  *            @arg SDMMC_IT_SDIOIT:   SD I/O interrupt received interrupt
  * @retval None
  */
#define __HAL_SD_SDMMC_ENABLE_IT(__HANDLE__, __INTERRUPT__) __SDMMC_ENABLE_IT((__HANDLE__)->Instance, (__INTERRUPT__))

/**
  * @brief  Disable the SD device interrupt.
  * @param  __HANDLE__: SD Handle   
  * @param  __INTERRUPT__: specifies the SDMMC interrupt sources to be disabled.
  *          This parameter can be one or a combination of the following values:
  *            @arg SDMMC_IT_CCRCFAIL: Command response received (CRC check failed) interrupt
  *            @arg SDMMC_IT_DCRCFAIL: Data block sent/received (CRC check failed) interrupt
  *            @arg SDMMC_IT_CTIMEOUT: Command response timeout interrupt
  *            @arg SDMMC_IT_DTIMEOUT: Data timeout interrupt
  *            @arg SDMMC_IT_TXUNDERR: Transmit FIFO underrun error interrupt
  *            @arg SDMMC_IT_RXOVERR:  Received FIFO overrun error interrupt
  *            @arg SDMMC_IT_CMDREND:  Command response received (CRC check passed) interrupt
  *            @arg SDMMC_IT_CMDSENT:  Command sent (no response required) interrupt
  *            @arg SDMMC_IT_DATAEND:  Data end (data counter, SDIDCOUNT, is zero) interrupt
  *            @arg SDMMC_IT_DBCKEND:  Data block sent/received (CRC check passed) interrupt
  *            @arg SDMMC_IT_CMDACT:   Command transfer in progress interrupt
  *            @arg SDMMC_IT_TXACT:    Data transmit in progress interrupt
  *            @arg SDMMC_IT_RXACT:    Data receive in progress interrupt
  *            @arg SDMMC_IT_TXFIFOHE: Transmit FIFO Half Empty interrupt
  *            @arg SDMMC_IT_RXFIFOHF: Receive FIFO Half Full interrupt
  *            @arg SDMMC_IT_TXFIFOF:  Transmit FIFO full interrupt
  *            @arg SDMMC_IT_RXFIFOF:  Receive FIFO full interrupt
  *            @arg SDMMC_IT_TXFIFOE:  Transmit FIFO empty interrupt
  *            @arg SDMMC_IT_RXFIFOE:  Receive FIFO empty interrupt
  *            @arg SDMMC_IT_TXDAVL:   Data available in transmit FIFO interrupt
  *            @arg SDMMC_IT_RXDAVL:   Data available in receive FIFO interrupt
  *            @arg SDMMC_IT_SDIOIT:   SD I/O interrupt received interrupt   
  * @retval None
  */
#define __HAL_SD_SDMMC_DISABLE_IT(__HANDLE__, __INTERRUPT__) __SDMMC_DISABLE_IT((__HANDLE__)->Instance, (__INTERRUPT__))

/**
  * @brief  Check whether the specified SD flag is set or not. 
  * @param  __HANDLE__: SD Handle   
  * @param  __FLAG__: specifies the flag to check. 
  *          This parameter can be one of the following values:
  *            @arg SDMMC_FLAG_CCRCFAIL: Command response received (CRC check failed)
  *            @arg SDMMC_FLAG_DCRCFAIL: Data block sent/received (CRC check failed)
  *            @arg SDMMC_FLAG_CTIMEOUT: Command response timeout
  *            @arg SDMMC_FLAG_DTIMEOUT: Data timeout
  *            @arg SDMMC_FLAG_TXUNDERR: Transmit FIFO underrun error
  *            @arg SDMMC_FLAG_RXOVERR:  Received FIFO overrun error
  *            @arg SDMMC_FLAG_CMDREND:  Command response received (CRC check passed)
  *            @arg SDMMC_FLAG_CMDSENT:  Command sent (no response required)
  *            @arg SDMMC_FLAG_DATAEND:  Data end (data counter, SDIDCOUNT, is zero)
  *            @arg SDMMC_FLAG_DBCKEND:  Data block sent/received (CRC check passed)
  *            @arg SDMMC_FLAG_CMDACT:   Command transfer in progress
  *            @arg SDMMC_FLAG_TXACT:    Data transmit in progress
  *            @arg SDMMC_FLAG_RXACT:    Data receive in progress
  *            @arg SDMMC_FLAG_TXFIFOHE: Transmit FIFO Half Empty
  *            @arg SDMMC_FLAG_RXFIFOHF: Receive FIFO Half Full
  *            @arg SDMMC_FLAG_TXFIFOF:  Transmit FIFO full
  *            @arg SDMMC_FLAG_RXFIFOF:  Receive FIFO full
  *            @arg SDMMC_FLAG_TXFIFOE:  Transmit FIFO empty
  *            @arg SDMMC_FLAG_RXFIFOE:  Receive FIFO empty
  *            @arg SDMMC_FLAG_TXDAVL:   Data available in transmit FIFO
  *            @arg SDMMC_FLAG_RXDAVL:   Data available in receive FIFO
  *            @arg SDMMC_FLAG_SDIOIT:   SD I/O interrupt received
  * @retval The new state of SD FLAG (SET or RESET).
  */
#define __HAL_SD_SDMMC_GET_FLAG(__HANDLE__, __FLAG__) __SDMMC_GET_FLAG((__HANDLE__)->Instance, (__FLAG__))

/**
  * @brief  Clear the SD's pending flags.
  * @param  __HANDLE__: SD Handle  
  * @param  __FLAG__: specifies the flag to clear.  
  *          This parameter can be one or a combination of the following values:
  *            @arg SDMMC_FLAG_CCRCFAIL: Command response received (CRC check failed)
  *            @arg SDMMC_FLAG_DCRCFAIL: Data block sent/received (CRC check failed)
  *            @arg SDMMC_FLAG_CTIMEOUT: Command response timeout
  *            @arg SDMMC_FLAG_DTIMEOUT: Data timeout
  *            @arg SDMMC_FLAG_TXUNDERR: Transmit FIFO underrun error
  *            @arg SDMMC_FLAG_RXOVERR:  Received FIFO overrun error
  *            @arg SDMMC_FLAG_CMDREND:  Command response received (CRC check passed)
  *            @arg SDMMC_FLAG_CMDSENT:  Command sent (no response required)
  *            @arg SDMMC_FLAG_DATAEND:  Data end (data counter, SDIDCOUNT, is zero)
  *            @arg SDMMC_FLAG_DBCKEND:  Data block sent/received (CRC check passed)
  *            @arg SDMMC_FLAG_SDIOIT:   SD I/O interrupt received
  * @retval None
  */
#define __HAL_SD_SDMMC_CLEAR_FLAG(__HANDLE__, __FLAG__) __SDMMC_CLEAR_FLAG((__HANDLE__)->Instance, (__FLAG__))

/**
  * @brief  Check whether the specified SD interrupt has occurred or not.
  * @param  __HANDLE__: SD Handle   
  * @param  __INTERRUPT__: specifies the SDMMC interrupt source to check. 
  *          This parameter can be one of the following values:
  *            @arg SDMMC_IT_CCRCFAIL: Command response received (CRC check failed) interrupt
  *            @arg SDMMC_IT_DCRCFAIL: Data block sent/received (CRC check failed) interrupt
  *            @arg SDMMC_IT_CTIMEOUT: Command response timeout interrupt
  *            @arg SDMMC_IT_DTIMEOUT: Data timeout interrupt
  *            @arg SDMMC_IT_TXUNDERR: Transmit FIFO underrun error interrupt
  *            @arg SDMMC_IT_RXOVERR:  Received FIFO overrun error interrupt
  *            @arg SDMMC_IT_CMDREND:  Command response received (CRC check passed) interrupt
  *            @arg SDMMC_IT_CMDSENT:  Command sent (no response required) interrupt
  *            @arg SDMMC_IT_DATAEND:  Data end (data counter, SDIDCOUNT, is zero) interrupt
  *            @arg SDMMC_IT_DBCKEND:  Data block sent/received (CRC check passed) interrupt
  *            @arg SDMMC_IT_CMDACT:   Command transfer in progress interrupt
  *            @arg SDMMC_IT_TXACT:    Data transmit in progress interrupt
  *            @arg SDMMC_IT_RXACT:    Data receive in progress interrupt
  *            @arg SDMMC_IT_TXFIFOHE: Transmit FIFO Half Empty interrupt
  *            @arg SDMMC_IT_RXFIFOHF: Receive FIFO Half Full interrupt
  *            @arg SDMMC_IT_TXFIFOF:  Transmit FIFO full interrupt
  *            @arg SDMMC_IT_RXFIFOF:  Receive FIFO full interrupt
  *            @arg SDMMC_IT_TXFIFOE:  Transmit FIFO empty interrupt
  *            @arg SDMMC_IT_RXFIFOE:  Receive FIFO empty interrupt
  *            @arg SDMMC_IT_TXDAVL:   Data available in transmit FIFO interrupt
  *            @arg SDMMC_IT_RXDAVL:   Data available in receive FIFO interrupt
  *            @arg SDMMC_IT_SDIOIT:   SD I/O interrupt received interrupt
  * @retval The new state of SD IT (SET or RESET).
  */
#define __HAL_SD_SDMMC_GET_IT(__HANDLE__, __INTERRUPT__) __SDMMC_GET_IT((__HANDLE__)->Instance, (__INTERRUPT__))

/**
  * @brief  Clear the SD's interrupt pending bits.
  * @param  __HANDLE__: SD Handle
  * @param  __INTERRUPT__: specifies the interrupt pending bit to clear. 
  *          This parameter can be one or a combination of the following values:
  *            @arg SDMMC_IT_CCRCFAIL: Command response received (CRC check failed) interrupt
  *            @arg SDMMC_IT_DCRCFAIL: Data block sent/received (CRC check failed) interrupt
  *            @arg SDMMC_IT_CTIMEOUT: Command response timeout interrupt
  *            @arg SDMMC_IT_DTIMEOUT: Data timeout interrupt
  *            @arg SDMMC_IT_TXUNDERR: Transmit FIFO underrun error interrupt
  *            @arg SDMMC_IT_RXOVERR:  Received FIFO overrun error interrupt
  *            @arg SDMMC_IT_CMDREND:  Command response received (CRC check passed) interrupt
  *            @arg SDMMC_IT_CMDSENT:  Command sent (no response required) interrupt
  *            @arg SDMMC_IT_DATAEND:  Data end (data counter, SDMMC_DCOUNT, is zero) interrupt
  *            @arg SDMMC_IT_SDIOIT:   SD I/O interrupt received interrupt
  * @retval None
  */
#define __HAL_SD_SDMMC_CLEAR_IT(__HANDLE__, __INTERRUPT__) __SDMMC_CLEAR_IT((__HANDLE__)->Instance, (__INTERRUPT__))
/**
  * @}
  */
  
/* Exported functions --------------------------------------------------------*/
/** @defgroup SD_Exported_Functions SD Exported Functions
  * @{
  */
  
/** @defgroup SD_Exported_Functions_Group1 Initialization and de-initialization functions
  * @{
  */
HAL_SD_ErrorTypedef HAL_SD_Init(SD_HandleTypeDef *hsd, HAL_SD_CardInfoTypedef *SDCardInfo);
HAL_StatusTypeDef   HAL_SD_DeInit (SD_HandleTypeDef *hsd);
void HAL_SD_MspInit(SD_HandleTypeDef *hsd);
void HAL_SD_MspDeInit(SD_HandleTypeDef *hsd);
/**
  * @}
  */
  
/** @defgroup SD_Exported_Functions_Group2 Input and Output operation functions
  * @{
  */
/* Blocking mode: Polling */
// dpgeorge: read/write functions renamed to emphasise that address is given by block number
HAL_SD_ErrorTypedef HAL_SD_ReadBlocks_BlockNumber(SD_HandleTypeDef *hsd, uint32_t *pReadBuffer, uint32_t BlockNumber, uint32_t BlockSize, uint32_t NumberOfBlocks);
HAL_SD_ErrorTypedef HAL_SD_WriteBlocks_BlockNumber(SD_HandleTypeDef *hsd, uint32_t *pWriteBuffer, uint32_t BlockNumber, uint32_t BlockSize, uint32_t NumberOfBlocks);
HAL_SD_ErrorTypedef HAL_SD_Erase(SD_HandleTypeDef *hsd, uint64_t startaddr, uint64_t endaddr);

/* Non-Blocking mode: Interrupt */
void HAL_SD_IRQHandler(SD_HandleTypeDef *hsd);

/* Callback in non blocking modes (DMA) */
void HAL_SD_DMA_RxCpltCallback(DMA_HandleTypeDef *hdma);
void HAL_SD_DMA_RxErrorCallback(DMA_HandleTypeDef *hdma);
void HAL_SD_DMA_TxCpltCallback(DMA_HandleTypeDef *hdma);
void HAL_SD_DMA_TxErrorCallback(DMA_HandleTypeDef *hdma);
void HAL_SD_XferCpltCallback(SD_HandleTypeDef *hsd);
void HAL_SD_XferErrorCallback(SD_HandleTypeDef *hsd);

/* Non-Blocking mode: DMA */
// dpgeorge: read/write functions renamed to emphasise that address is given by block number
HAL_SD_ErrorTypedef HAL_SD_ReadBlocks_BlockNumber_DMA(SD_HandleTypeDef *hsd, uint32_t *pReadBuffer, uint32_t BlockNumber, uint32_t BlockSize, uint32_t NumberOfBlocks);
HAL_SD_ErrorTypedef HAL_SD_WriteBlocks_BlockNumber_DMA(SD_HandleTypeDef *hsd, uint32_t *pWriteBuffer, uint32_t BlockNumber, uint32_t BlockSize, uint32_t NumberOfBlocks);
HAL_SD_ErrorTypedef HAL_SD_CheckWriteOperation(SD_HandleTypeDef *hsd, uint32_t Timeout);
HAL_SD_ErrorTypedef HAL_SD_CheckReadOperation(SD_HandleTypeDef *hsd, uint32_t Timeout);
/**
  * @}
  */
  
/** @defgroup SD_Exported_Functions_Group3 Peripheral Control functions
  * @{
  */
HAL_SD_ErrorTypedef HAL_SD_Get_CardInfo(SD_HandleTypeDef *hsd, HAL_SD_CardInfoTypedef *pCardInfo);
HAL_SD_ErrorTypedef HAL_SD_WideBusOperation_Config(SD_HandleTypeDef *hsd, uint32_t WideMode);
HAL_SD_ErrorTypedef HAL_SD_StopTransfer(SD_HandleTypeDef *hsd);
HAL_SD_ErrorTypedef HAL_SD_HighSpeed (SD_HandleTypeDef *hsd);
/**
  * @}
  */
  
/* Peripheral State functions  ************************************************/
/** @defgroup SD_Exported_Functions_Group4 Peripheral State functions
  * @{
  */
HAL_SD_ErrorTypedef HAL_SD_SendSDStatus(SD_HandleTypeDef *hsd, uint32_t *pSDstatus);
HAL_SD_ErrorTypedef HAL_SD_GetCardStatus(SD_HandleTypeDef *hsd, HAL_SD_CardStatusTypedef *pCardStatus);
HAL_SD_TransferStateTypedef HAL_SD_GetStatus(SD_HandleTypeDef *hsd);
/**
  * @}
  */
  
/**
  * @}
  */
    
/* Private types -------------------------------------------------------------*/
/** @defgroup SD_Private_Types SD Private Types
  * @{
  */

/**
  * @}
  */ 

/* Private defines -----------------------------------------------------------*/
/** @defgroup SD_Private_Defines SD Private Defines
  * @{
  */

/**
  * @}
  */ 
          
/* Private variables ---------------------------------------------------------*/
/** @defgroup SD_Private_Variables SD Private Variables
  * @{
  */

/**
  * @}
  */ 

/* Private constants ---------------------------------------------------------*/
/** @defgroup SD_Private_Constants SD Private Constants
  * @{
  */

/**
  * @}
  */ 

/* Private macros ------------------------------------------------------------*/
/** @defgroup SD_Private_Macros SD Private Macros
  * @{
  */

/**
  * @}
  */

/* Private functions prototypes ----------------------------------------------*/
/** @defgroup SD_Private_Functions_Prototypes SD Private Functions Prototypes
  * @{
  */

/**
  * @}
  */

/* Private functions ---------------------------------------------------------*/
/** @defgroup SD_Private_Functions SD Private Functions
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

#ifdef __cplusplus
}
#endif


#endif /* __STM32L4xx_HAL_SD_H */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
