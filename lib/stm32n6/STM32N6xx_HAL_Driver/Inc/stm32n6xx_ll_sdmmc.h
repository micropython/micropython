/**
  ******************************************************************************
  * @file    stm32n6xx_ll_sdmmc.h
  * @author  MCD Application Team
  * @brief   Header file of SDMMC HAL module.
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
#ifndef STM32N6xx_LL_SDMMC_H
#define STM32N6xx_LL_SDMMC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal_def.h"

/** @addtogroup STM32N6xx_Driver
  * @{
  */
#if defined (SDMMC1) || defined (SDMMC2)
/** @addtogroup SDMMC_LL
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup SDMMC_LL_Exported_Types SDMMC_LL Exported Types
  * @{
  */

/**
  * @brief  SDMMC Configuration Structure definition
  */
typedef struct
{
  uint32_t ClockEdge;            /*!< Specifies the SDMMC_CCK clock transition on which Data and Command change.
                                      This parameter can be a value of @ref SDMMC_LL_Clock_Edge                 */

  uint32_t ClockPowerSave;       /*!< Specifies whether SDMMC Clock output is enabled or
                                      disabled when the bus is idle.
                                      This parameter can be a value of @ref SDMMC_LL_Clock_Power_Save           */

  uint32_t BusWide;              /*!< Specifies the SDMMC bus width.
                                      This parameter can be a value of @ref SDMMC_LL_Bus_Wide                   */

  uint32_t HardwareFlowControl;  /*!< Specifies whether the SDMMC hardware flow control is enabled or disabled.
                                      This parameter can be a value of @ref SDMMC_LL_Hardware_Flow_Control      */

  uint32_t ClockDiv;             /*!< Specifies the clock frequency of the SDMMC controller.
                                      This parameter can be a value between Min_Data = 0 and Max_Data = 1023   */

#if (USE_SD_TRANSCEIVER != 0U) || (USE_SDIO_TRANSCEIVER != 0U)
  uint32_t TranceiverPresent;    /*!< Specifies if there is a 1V8 Transceiver/Switcher.
                                      This parameter can be a value of @ref SDMMC_LL_TRANSCEIVER_PRESENT       */
#endif /* USE_SD_TRANSCEIVER || USE_SDIO_TRANSCEIVER */
} SDMMC_InitTypeDef;


/**
  * @brief  SDMMC Command Control structure
  */
typedef struct
{
  uint32_t Argument;            /*!< Specifies the SDMMC command argument which is sent
                                     to a card as part of a command message. If a command
                                     contains an argument, it must be loaded into this register
                                     before writing the command to the command register.              */

  uint32_t CmdIndex;            /*!< Specifies the SDMMC command index. It must be Min_Data = 0 and
                                     Max_Data = 64                                                    */

  uint32_t Response;            /*!< Specifies the SDMMC response type.
                                     This parameter can be a value of @ref SDMMC_LL_Response_Type         */

  uint32_t WaitForInterrupt;    /*!< Specifies whether SDMMC wait for interrupt request is
                                     enabled or disabled.
                                     This parameter can be a value of @ref SDMMC_LL_Wait_Interrupt_State  */

  uint32_t CPSM;                /*!< Specifies whether SDMMC Command path state machine (CPSM)
                                     is enabled or disabled.
                                     This parameter can be a value of @ref SDMMC_LL_CPSM_State            */
} SDMMC_CmdInitTypeDef;


/**
  * @brief  SDMMC Data Control structure
  */
typedef struct
{
  uint32_t DataTimeOut;         /*!< Specifies the data timeout period in card bus clock periods.  */

  uint32_t DataLength;          /*!< Specifies the number of data bytes to be transferred.         */

  uint32_t DataBlockSize;       /*!< Specifies the data block size for block transfer.
                                     This parameter can be a value of @ref SDMMC_LL_Data_Block_Size    */

  uint32_t TransferDir;         /*!< Specifies the data transfer direction, whether the transfer
                                     is a read or write.
                                     This parameter can be a value of @ref SDMMC_LL_Transfer_Direction */

  uint32_t TransferMode;        /*!< Specifies whether data transfer is in stream or block mode.
                                     This parameter can be a value of @ref SDMMC_LL_Transfer_Type      */

  uint32_t DPSM;                /*!< Specifies whether SDMMC Data path state machine (DPSM)
                                     is enabled or disabled.
                                     This parameter can be a value of @ref SDMMC_LL_DPSM_State         */
} SDMMC_DataInitTypeDef;

/** @defgroup SDEx_Exported_Types_Group1 SD Card Internal DMA Buffer structure
  * @{
  */
typedef struct
{
  __IO uint32_t IDMALAR;              /*!< SDMMC DMA linked list configuration register  */
  __IO uint32_t IDMABASER;            /*!< SDMMC DMA buffer base address register        */
  __IO uint32_t IDMABSIZE;            /*!< SDMMC DMA buffer size register                */
} SDMMC_DMALinkNodeTypeDef;

typedef struct
{
  uint32_t BufferAddress;              /*!<  Node Buffer address                          */
  uint32_t BufferSize ;                /*!<  Node Buffer size                             */
} SDMMC_DMALinkNodeConfTypeDef;

typedef struct
{
  SDMMC_DMALinkNodeTypeDef *pHeadNode;  /*!<  Linked List Node Head                        */
  SDMMC_DMALinkNodeTypeDef *pTailNode;  /*!<  Linked List Node Head                        */
  uint32_t NodesCounter ;               /*!<  Node is ready for execution                  */
} SDMMC_DMALinkedListTypeDef;
/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup SDMMC_LL_Exported_Constants SDMMC_LL Exported Constants
  * @{
  */
#define SDMMC_ERROR_NONE                     ((uint32_t)0x00000000U)   /*!< No error                                                      */
#define SDMMC_ERROR_CMD_CRC_FAIL             ((uint32_t)0x00000001U)   /*!< Command response received (but CRC check failed)              */
#define SDMMC_ERROR_DATA_CRC_FAIL            ((uint32_t)0x00000002U)   /*!< Data block sent/received (CRC check failed)                   */
#define SDMMC_ERROR_CMD_RSP_TIMEOUT          ((uint32_t)0x00000004U)   /*!< Command response timeout                                      */
#define SDMMC_ERROR_DATA_TIMEOUT             ((uint32_t)0x00000008U)   /*!< Data timeout                                                  */
#define SDMMC_ERROR_TX_UNDERRUN              ((uint32_t)0x00000010U)   /*!< Transmit FIFO underrun                                        */
#define SDMMC_ERROR_RX_OVERRUN               ((uint32_t)0x00000020U)   /*!< Receive FIFO overrun                                          */
#define SDMMC_ERROR_ADDR_MISALIGNED          ((uint32_t)0x00000040U)   /*!< Misaligned address                                            */
#define SDMMC_ERROR_BLOCK_LEN_ERR            ((uint32_t)0x00000080U)   /*!< Transferred block length is not allowed for the card or the number of transferred bytes does not match the block length   */
#define SDMMC_ERROR_ERASE_SEQ_ERR            ((uint32_t)0x00000100U)   /*!< An error in the sequence of erase command occurs              */
#define SDMMC_ERROR_BAD_ERASE_PARAM          ((uint32_t)0x00000200U)   /*!< An invalid selection for erase groups                         */
#define SDMMC_ERROR_WRITE_PROT_VIOLATION     ((uint32_t)0x00000400U)   /*!< Attempt to program a write protect block                      */
#define SDMMC_ERROR_LOCK_UNLOCK_FAILED       ((uint32_t)0x00000800U)   /*!< Sequence or password error has been detected in unlock command or if there was an attempt to access a locked card    */
#define SDMMC_ERROR_COM_CRC_FAILED           ((uint32_t)0x00001000U)   /*!< CRC check of the previous command failed                      */
#define SDMMC_ERROR_ILLEGAL_CMD              ((uint32_t)0x00002000U)   /*!< Command is not legal for the card state                       */
#define SDMMC_ERROR_CARD_ECC_FAILED          ((uint32_t)0x00004000U)   /*!< Card internal ECC was applied but failed to correct the data  */
#define SDMMC_ERROR_CC_ERR                   ((uint32_t)0x00008000U)   /*!< Internal card controller error                                */
#define SDMMC_ERROR_GENERAL_UNKNOWN_ERR      ((uint32_t)0x00010000U)   /*!< General or unknown error                                      */
#define SDMMC_ERROR_STREAM_READ_UNDERRUN     ((uint32_t)0x00020000U)   /*!< The card could not sustain data reading in stream rmode       */
#define SDMMC_ERROR_STREAM_WRITE_OVERRUN     ((uint32_t)0x00040000U)   /*!< The card could not sustain data programming in stream mode    */
#define SDMMC_ERROR_CID_CSD_OVERWRITE        ((uint32_t)0x00080000U)   /*!< CID/CSD overwrite error                                       */
#define SDMMC_ERROR_WP_ERASE_SKIP            ((uint32_t)0x00100000U)   /*!< Only partial address space was erased                         */
#define SDMMC_ERROR_CARD_ECC_DISABLED        ((uint32_t)0x00200000U)   /*!< Command has been executed without using internal ECC          */
#define SDMMC_ERROR_ERASE_RESET              ((uint32_t)0x00400000U)   /*!< Erase sequence was cleared before executing because an out of erase sequence command was received                        */
#define SDMMC_ERROR_AKE_SEQ_ERR              ((uint32_t)0x00800000U)   /*!< Error in sequence of authentication                           */
#define SDMMC_ERROR_INVALID_VOLTRANGE        ((uint32_t)0x01000000U)   /*!< Error in case of invalid voltage range                        */
#define SDMMC_ERROR_ADDR_OUT_OF_RANGE        ((uint32_t)0x02000000U)   /*!< Error when addressed block is out of range                    */
#define SDMMC_ERROR_REQUEST_NOT_APPLICABLE   ((uint32_t)0x04000000U)   /*!< Error when command request is not applicable                  */
#define SDMMC_ERROR_INVALID_PARAMETER        ((uint32_t)0x08000000U)   /*!< the used parameter is not valid                               */
#define SDMMC_ERROR_UNSUPPORTED_FEATURE      ((uint32_t)0x10000000U)   /*!< Error when feature is not insupported                         */
#define SDMMC_ERROR_BUSY                     ((uint32_t)0x20000000U)   /*!< Error when transfer process is busy                           */
#define SDMMC_ERROR_DMA                      ((uint32_t)0x40000000U)   /*!< Error while DMA transfer                                      */
#define SDMMC_ERROR_TIMEOUT                  ((uint32_t)0x80000000U)   /*!< Timeout error                                                 */

/**
  * @brief  Masks for R5 Response
  */
/** this is the reserved for future use in spec RFU */
#define SDMMC_SDIO_R5_ERROR                          ((uint32_t)0x00000400U)
/** Out of range error */
#define SDMMC_SDIO_R5_OUT_OF_RANGE                   ((uint32_t)0x00000100U)
/** Invalid function number */
#define SDMMC_SDIO_R5_INVALID_FUNCTION_NUMBER        ((uint32_t)0x00000200U)
/** General or an unknown error */
#define SDMMC_SDIO_R5_GENERAL_UNKNOWN_ERROR          ((uint32_t)0x00000800U)
/** SDIO Card current state
  * 00=DIS (card not selected)
  * 01=CMD (data line free)
  * 10=TRN (transfer on data lines) */
#define SDMMC_SDIO_R5_IO_CURRENT_STATE               ((uint32_t)0x00003000U)
/** Illegal command error */
#define SDMMC_SDIO_R5_ILLEGAL_CMD                    ((uint32_t)0x00004000U)
/** CRC check of previous cmd failed */
#define SDMMC_SDIO_R5_COM_CRC_FAILED                 ((uint32_t)0x00008000U)

#define SDMMC_SDIO_R5_ERRORBITS                      (SDMMC_SDIO_R5_COM_CRC_FAILED          | \
                                                      SDMMC_SDIO_R5_ILLEGAL_CMD             | \
                                                      SDMMC_SDIO_R5_GENERAL_UNKNOWN_ERROR   | \
                                                      SDMMC_SDIO_R5_INVALID_FUNCTION_NUMBER | \
                                                      SDMMC_SDIO_R5_OUT_OF_RANGE)
/**
  * @brief SDIO_CMD53_MODE
  */
#define SDMMC_SDIO_MODE_BYTE                         0x00U  /*!< Byte Mode  */
#define SDMMC_SDIO_MODE_BLOCK                        0x01U  /*!< Block Mode */

/**
  * @brief SDIO_CMD53_OP_CODE
  */
#define SDMMC_SDIO_NO_INC                            0x00U  /*!< No auto indentation */
#define SDMMC_SDIO_AUTO_INC                          0x01U  /*!< Auto indentation    */

/**
  * @brief SDIO_CMD53_RAW
  */
#define SDMMC_SDIO_WO                                0x00U  /*!< Write only Flag       */
#define SDMMC_SDIO_RAW                               0x01U  /*!< Read after write Flag */

/**
  * @brief SDMMC Commands Index
  */
#define SDMMC_CMD_GO_IDLE_STATE                       0U   /*!< Resets the SD memory card.                                                               */
#define SDMMC_CMD_SEND_OP_COND                        1U   /*!< Sends host capacity support information and activates the card's initialization process. */
#define SDMMC_CMD_ALL_SEND_CID                        2U   /*!< Asks any card connected to the host to send the CID numbers on the CMD line.             */
#define SDMMC_CMD_SET_REL_ADDR                        3U   /*!< Asks the card to publish a new relative address (RCA).                                   */
#define SDMMC_CMD_SET_DSR                             4U   /*!< Programs the DSR of all cards.                                                           */
#define SDMMC_CMD_SDMMC_SEN_OP_COND                   5U   /*!< Sends host capacity support information (HCS) and asks the accessed card to send its operating condition register (OCR) content in the response on the CMD line.*/
#define SDMMC_CMD_HS_SWITCH                           6U   /*!< Checks switchable function (mode 0) and switch card function (mode 1).                   */
#define SDMMC_CMD_SEL_DESEL_CARD                      7U   /*!< Selects the card by its own relative address and gets deselected by any other address    */
#define SDMMC_CMD_HS_SEND_EXT_CSD                     8U   /*!< Sends SD Memory Card interface condition, which includes host supply voltage information  and asks the card whether card supports voltage.                      */
#define SDMMC_CMD_SEND_CSD                            9U   /*!< Addressed card sends its card specific data (CSD) on the CMD line.                       */
#define SDMMC_CMD_SEND_CID                            10U  /*!< Addressed card sends its card identification (CID) on the CMD line.                      */
#define SDMMC_CMD_VOLTAGE_SWITCH                      11U  /*!< SD card Voltage switch to 1.8V mode.                                                     */
#define SDMMC_CMD_STOP_TRANSMISSION                   12U  /*!< Forces the card to stop transmission.                                                    */
#define SDMMC_CMD_SEND_STATUS                         13U  /*!< Addressed card sends its status register.                                                */
#define SDMMC_CMD_HS_BUSTEST_READ                     14U  /*!< Reserved                                                                                 */
#define SDMMC_CMD_GO_INACTIVE_STATE                   15U  /*!< Sends an addressed card into the inactive state.                                         */
#define SDMMC_CMD_SET_BLOCKLEN                        16U  /*!< Sets the block length (in bytes for SDSC) for all following block commands (read, write, lock). Default block length is fixed to 512 Bytes. Not effective        */
/*!< for SDHS and SDXC.                                                                       */
#define SDMMC_CMD_READ_SINGLE_BLOCK                   17U  /*!< Reads single block of size selected by SET_BLOCKLEN in case of SDSC, and a block of fixed 512 bytes in case of SDHC and SDXC.                                    */
#define SDMMC_CMD_READ_MULT_BLOCK                     18U  /*!< Continuously transfers data blocks from card to host until interrupted by  STOP_TRANSMISSION command.                                                            */
#define SDMMC_CMD_HS_BUSTEST_WRITE                    19U  /*!< 64 bytes tuning pattern is sent for SDR50 and SDR104.                                    */
#define SDMMC_CMD_WRITE_DAT_UNTIL_STOP                20U  /*!< Speed class control command.                                                             */
#define SDMMC_CMD_SET_BLOCK_COUNT                     23U  /*!< Specify block count for CMD18 and CMD25.                                                 */
#define SDMMC_CMD_WRITE_SINGLE_BLOCK                  24U  /*!< Writes single block of size selected by SET_BLOCKLEN in case of SDSC, and a block of fixed 512 bytes in case of SDHC and SDXC.                                   */
#define SDMMC_CMD_WRITE_MULT_BLOCK                    25U  /*!< Continuously writes blocks of data until a STOP_TRANSMISSION follows.                    */
#define SDMMC_CMD_PROG_CID                            26U  /*!< Reserved for manufacturers.                                                              */
#define SDMMC_CMD_PROG_CSD                            27U  /*!< Programming of the programmable bits of the CSD.                                         */
#define SDMMC_CMD_SET_WRITE_PROT                      28U  /*!< Sets the write protection bit of the addressed group.                                    */
#define SDMMC_CMD_CLR_WRITE_PROT                      29U  /*!< Clears the write protection bit of the addressed group.                                  */
#define SDMMC_CMD_SEND_WRITE_PROT                     30U  /*!< Asks the card to send the status of the write protection bits.                           */
#define SDMMC_CMD_SD_ERASE_GRP_START                  32U  /*!< Sets the address of the first write block to be erased. (For SD card only).              */
#define SDMMC_CMD_SD_ERASE_GRP_END                    33U  /*!< Sets the address of the last write block of the continuous range to be erased.           */
#define SDMMC_CMD_ERASE_GRP_START                     35U  /*!< Sets the address of the first write block to be erased. Reserved for each command system set by switch function command (CMD6).                                  */
#define SDMMC_CMD_ERASE_GRP_END                       36U  /*!< Sets the address of the last write block of the continuous range to be erased. Reserved for each command system set by switch function command (CMD6).           */
#define SDMMC_CMD_ERASE                               38U  /*!< Reserved for SD security applications.                                                   */
#define SDMMC_CMD_FAST_IO                             39U  /*!< SD card doesn't support it (Reserved).                                                   */
#define SDMMC_CMD_GO_IRQ_STATE                        40U  /*!< SD card doesn't support it (Reserved).                                                   */
#define SDMMC_CMD_LOCK_UNLOCK                         42U  /*!< Sets/resets the password or lock/unlock the card. The size of the data block is set by the SET_BLOCK_LEN command.                                                */
#define SDMMC_CMD_APP_CMD                             55U  /*!< Indicates to the card that the next command is an application specific command rather than a standard command.                                                   */
#define SDMMC_CMD_GEN_CMD                             56U  /*!< Used either to transfer a data block to the card or to get a data block from the card for general purpose/application specific commands.                         */
#define SDMMC_CMD_NO_CMD                              64U  /*!< No command                                                                               */

/**
  * @brief Following commands are SD Card Specific commands.
  *        SDMMC_APP_CMD should be sent before sending these commands.
  */
#define SDMMC_CMD_APP_SD_SET_BUSWIDTH                 6U   /*!< (ACMD6) Defines the data bus width to be used for data transfer. The allowed data bus widths are given in SCR register.                                                   */
#define SDMMC_CMD_SD_APP_STATUS                       13U  /*!< (ACMD13) Sends the SD status.                                                            */
#define SDMMC_CMD_SD_APP_SEND_NUM_WRITE_BLOCKS        22U  /*!< (ACMD22) Sends the number of the written (without errors) write blocks. Responds with 32bit+CRC data block.                                                               */
#define SDMMC_CMD_SD_APP_OP_COND                      41U  /*!< (ACMD41) Sends host capacity support information (HCS) and asks the accessed card to send its operating condition register (OCR) content in the response on the CMD line. */
#define SDMMC_CMD_SD_APP_SET_CLR_CARD_DETECT          42U  /*!< (ACMD42) Connect/Disconnect the 50 KOhm pull-up resistor on CD/DAT3 (pin 1) of the card  */
#define SDMMC_CMD_SD_APP_SEND_SCR                     51U  /*!< Reads the SD Configuration Register (SCR).                                               */
#define SDMMC_CMD_SDMMC_RW_DIRECT                     52U  /*!< For SD I/O card only, reserved for security specification.                               */
#define SDMMC_CMD_SDMMC_RW_EXTENDED                   53U  /*!< For SD I/O card only, reserved for security specification.                               */

/**
  * @brief Following commands are MMC Specific commands.
  */
#define SDMMC_CMD_MMC_SLEEP_AWAKE                     5U   /*!< Toggle the device between Sleep state and Standby state.                                 */

/**
  * @brief Following commands are SD Card Specific security commands.
  *        SDMMC_CMD_APP_CMD should be sent before sending these commands.
  */
#define SDMMC_CMD_SD_APP_GET_MKB                      43U
#define SDMMC_CMD_SD_APP_GET_MID                      44U
#define SDMMC_CMD_SD_APP_SET_CER_RN1                  45U
#define SDMMC_CMD_SD_APP_GET_CER_RN2                  46U
#define SDMMC_CMD_SD_APP_SET_CER_RES2                 47U
#define SDMMC_CMD_SD_APP_GET_CER_RES1                 48U
#define SDMMC_CMD_SD_APP_SECURE_READ_MULTIPLE_BLOCK   18U
#define SDMMC_CMD_SD_APP_SECURE_WRITE_MULTIPLE_BLOCK  25U
#define SDMMC_CMD_SD_APP_SECURE_ERASE                 38U
#define SDMMC_CMD_SD_APP_CHANGE_SECURE_AREA           49U
#define SDMMC_CMD_SD_APP_SECURE_WRITE_MKB             48U

/**
  * @brief  Masks for errors Card Status R1 (OCR Register)
  */
#define SDMMC_OCR_ADDR_OUT_OF_RANGE        ((uint32_t)0x80000000U)
#define SDMMC_OCR_ADDR_MISALIGNED          ((uint32_t)0x40000000U)
#define SDMMC_OCR_BLOCK_LEN_ERR            ((uint32_t)0x20000000U)
#define SDMMC_OCR_ERASE_SEQ_ERR            ((uint32_t)0x10000000U)
#define SDMMC_OCR_BAD_ERASE_PARAM          ((uint32_t)0x08000000U)
#define SDMMC_OCR_WRITE_PROT_VIOLATION     ((uint32_t)0x04000000U)
#define SDMMC_OCR_LOCK_UNLOCK_FAILED       ((uint32_t)0x01000000U)
#define SDMMC_OCR_COM_CRC_FAILED           ((uint32_t)0x00800000U)
#define SDMMC_OCR_ILLEGAL_CMD              ((uint32_t)0x00400000U)
#define SDMMC_OCR_CARD_ECC_FAILED          ((uint32_t)0x00200000U)
#define SDMMC_OCR_CC_ERROR                 ((uint32_t)0x00100000U)
#define SDMMC_OCR_GENERAL_UNKNOWN_ERROR    ((uint32_t)0x00080000U)
#define SDMMC_OCR_STREAM_READ_UNDERRUN     ((uint32_t)0x00040000U)
#define SDMMC_OCR_STREAM_WRITE_OVERRUN     ((uint32_t)0x00020000U)
#define SDMMC_OCR_CID_CSD_OVERWRITE        ((uint32_t)0x00010000U)
#define SDMMC_OCR_WP_ERASE_SKIP            ((uint32_t)0x00008000U)
#define SDMMC_OCR_CARD_ECC_DISABLED        ((uint32_t)0x00004000U)
#define SDMMC_OCR_ERASE_RESET              ((uint32_t)0x00002000U)
#define SDMMC_OCR_AKE_SEQ_ERROR            ((uint32_t)0x00000008U)
#define SDMMC_OCR_ERRORBITS                ((uint32_t)0xFDFFE008U)

/**
  * @brief  Masks for R6 Response
  */
#define SDMMC_R6_GENERAL_UNKNOWN_ERROR     ((uint32_t)0x00002000U)
#define SDMMC_R6_ILLEGAL_CMD               ((uint32_t)0x00004000U)
#define SDMMC_R6_COM_CRC_FAILED            ((uint32_t)0x00008000U)

#define SDMMC_VOLTAGE_WINDOW_SD            ((uint32_t)0x80100000U)
#define SDMMC_HIGH_CAPACITY                ((uint32_t)0x40000000U)
#define SDMMC_STD_CAPACITY                 ((uint32_t)0x00000000U)
#define SDMMC_CHECK_PATTERN                ((uint32_t)0x000001AAU)
#define SD_SWITCH_1_8V_CAPACITY            ((uint32_t)0x01000000U)
#define SDMMC_DDR50_SWITCH_PATTERN         ((uint32_t)0x80FFFF04U)
#define SDMMC_SDR104_SWITCH_PATTERN        ((uint32_t)0x80FF1F03U)
#define SDMMC_SDR50_SWITCH_PATTERN         ((uint32_t)0x80FF1F02U)
#define SDMMC_SDR25_SWITCH_PATTERN         ((uint32_t)0x80FFFF01U)
#define SDMMC_SDR12_SWITCH_PATTERN         ((uint32_t)0x80FFFF00U)

#define SDMMC_MAX_VOLT_TRIAL               ((uint32_t)0x0000FFFFU)

#define SDMMC_MAX_TRIAL                    ((uint32_t)0x0000FFFFU)

#define SDMMC_ALLZERO                      ((uint32_t)0x00000000U)

#define SDMMC_WIDE_BUS_SUPPORT             ((uint32_t)0x00040000U)
#define SDMMC_SINGLE_BUS_SUPPORT           ((uint32_t)0x00010000U)
#define SDMMC_CARD_LOCKED                  ((uint32_t)0x02000000U)

#ifndef SDMMC_DATATIMEOUT /*Hardware Data Timeout (cycles) */
#define SDMMC_DATATIMEOUT                  ((uint32_t)0xFFFFFFFFU)
#endif /* SDMMC_DATATIMEOUT */

#ifndef SDMMC_SWDATATIMEOUT /*Software Data Timeout (ms) */
#define SDMMC_SWDATATIMEOUT                ((uint32_t)0xFFFFFFFFU)
#endif /* SDMMC_SWDATATIMEOUT */

#define SDMMC_0TO7BITS                     ((uint32_t)0x000000FFU)
#define SDMMC_8TO15BITS                    ((uint32_t)0x0000FF00U)
#define SDMMC_16TO23BITS                   ((uint32_t)0x00FF0000U)
#define SDMMC_24TO31BITS                   ((uint32_t)0xFF000000U)
#define SDMMC_MAX_DATA_LENGTH              ((uint32_t)0x01FFFFFFU)

#define SDMMC_HALFFIFO                     ((uint32_t)0x00000008U)
#define SDMMC_HALFFIFOBYTES                ((uint32_t)0x00000020U)

/* SDMMC FIFO Size */
#define SDMMC_FIFO_SIZE 512U
/**
  * @brief  Command Class supported
  */
#define SDMMC_CCCC_ERASE                   ((uint32_t)0x00000020U)

#define SDMMC_CMDTIMEOUT                   ((uint32_t)5000U)        /* Command send and response timeout     */
#define SDMMC_MAXERASETIMEOUT              ((uint32_t)63000U)       /* Max erase Timeout 63 s                */
#define SDMMC_STOPTRANSFERTIMEOUT          ((uint32_t)100000000U)   /* Timeout for STOP TRANSMISSION command */

/** @defgroup SDMMC_LL_Clock_Edge Clock Edge
  * @{
  */
#define SDMMC_CLOCK_EDGE_RISING               ((uint32_t)0x00000000U)
#define SDMMC_CLOCK_EDGE_FALLING              SDMMC_CLKCR_NEGEDGE

#define IS_SDMMC_CLOCK_EDGE(EDGE) (((EDGE) == SDMMC_CLOCK_EDGE_RISING) || \
                                   ((EDGE) == SDMMC_CLOCK_EDGE_FALLING))
/**
  * @}
  */

/** @defgroup SDMMC_LL_Clock_Power_Save Clock Power Saving
  * @{
  */
#define SDMMC_CLOCK_POWER_SAVE_DISABLE         ((uint32_t)0x00000000U)
#define SDMMC_CLOCK_POWER_SAVE_ENABLE          SDMMC_CLKCR_PWRSAV

#define IS_SDMMC_CLOCK_POWER_SAVE(SAVE) (((SAVE) == SDMMC_CLOCK_POWER_SAVE_DISABLE) || \
                                         ((SAVE) == SDMMC_CLOCK_POWER_SAVE_ENABLE))
/**
  * @}
  */

/** @defgroup SDMMC_LL_Bus_Wide Bus Width
  * @{
  */
#define SDMMC_BUS_WIDE_1B                      ((uint32_t)0x00000000U)
#define SDMMC_BUS_WIDE_4B                      SDMMC_CLKCR_WIDBUS_0
#define SDMMC_BUS_WIDE_8B                      SDMMC_CLKCR_WIDBUS_1

#define IS_SDMMC_BUS_WIDE(WIDE) (((WIDE) == SDMMC_BUS_WIDE_1B) || \
                                 ((WIDE) == SDMMC_BUS_WIDE_4B) || \
                                 ((WIDE) == SDMMC_BUS_WIDE_8B))
/**
  * @}
  */

/** @defgroup SDMMC_LL_Speed_Mode
  * @{
  */
#define SDMMC_SPEED_MODE_AUTO                  ((uint32_t)0x00000000U)
#define SDMMC_SPEED_MODE_DEFAULT               ((uint32_t)0x00000001U)
#define SDMMC_SPEED_MODE_HIGH                  ((uint32_t)0x00000002U)
#define SDMMC_SPEED_MODE_ULTRA                 ((uint32_t)0x00000003U)
#define SDMMC_SPEED_MODE_ULTRA_SDR104          SDMMC_SPEED_MODE_ULTRA
#define SDMMC_SPEED_MODE_DDR                   ((uint32_t)0x00000004U)
#define SDMMC_SPEED_MODE_ULTRA_SDR50           ((uint32_t)0x00000005U)

#define IS_SDMMC_SPEED_MODE(MODE) (((MODE) == SDMMC_SPEED_MODE_AUTO)         || \
                                   ((MODE) == SDMMC_SPEED_MODE_DEFAULT)      || \
                                   ((MODE) == SDMMC_SPEED_MODE_HIGH)         || \
                                   ((MODE) == SDMMC_SPEED_MODE_ULTRA)        || \
                                   ((MODE) == SDMMC_SPEED_MODE_ULTRA_SDR50)  || \
                                   ((MODE) == SDMMC_SPEED_MODE_DDR))

/**
  * @}
  */

/** @defgroup SDMMC_LL_Hardware_Flow_Control Hardware Flow Control
  * @{
  */
#define SDMMC_HARDWARE_FLOW_CONTROL_DISABLE    ((uint32_t)0x00000000U)
#define SDMMC_HARDWARE_FLOW_CONTROL_ENABLE     SDMMC_CLKCR_HWFC_EN

#define IS_SDMMC_HARDWARE_FLOW_CONTROL(CONTROL) (((CONTROL) == SDMMC_HARDWARE_FLOW_CONTROL_DISABLE) || \
                                                 ((CONTROL) == SDMMC_HARDWARE_FLOW_CONTROL_ENABLE))
/**
  * @}
  */

/** @defgroup SDMMC_LL_Clock_Division Clock Division
  * @{
  */
/* SDMMC_CK frequency = SDMMCCLK / [2 * CLKDIV] */
#define IS_SDMMC_CLKDIV(DIV)   ((DIV) < 0x400U)
/**
  * @}
  */

/** @defgroup SDMMC_LL_TRANSCEIVER_PRESENT Transceiver Present
  * @{
  */
#define SDMMC_TRANSCEIVER_UNKNOWN             ((uint32_t)0x00000000U)
#define SDMMC_TRANSCEIVER_NOT_PRESENT         ((uint32_t)0x00000001U)
#define SDMMC_TRANSCEIVER_PRESENT             ((uint32_t)0x00000002U)

/**
  * @}
  */

/** @defgroup SDMMC_LL_Command_Index Command Index
  * @{
  */
#define IS_SDMMC_CMD_INDEX(INDEX)            ((INDEX) < 0x40U)
/**
  * @}
  */

/** @defgroup SDMMC_LL_Response_Type Response Type
  * @{
  */
#define SDMMC_RESPONSE_NO                    ((uint32_t)0x00000000U)
#define SDMMC_RESPONSE_SHORT                 SDMMC_CMD_WAITRESP_0
#define SDMMC_RESPONSE_LONG                  SDMMC_CMD_WAITRESP

#define IS_SDMMC_RESPONSE(RESPONSE) (((RESPONSE) == SDMMC_RESPONSE_NO)    || \
                                     ((RESPONSE) == SDMMC_RESPONSE_SHORT) || \
                                     ((RESPONSE) == SDMMC_RESPONSE_LONG))
/**
  * @}
  */

/** @defgroup SDMMC_LL_Wait_Interrupt_State Wait Interrupt
  * @{
  */
#define SDMMC_WAIT_NO                        ((uint32_t)0x00000000U)
#define SDMMC_WAIT_IT                        SDMMC_CMD_WAITINT
#define SDMMC_WAIT_PEND                      SDMMC_CMD_WAITPEND

#define IS_SDMMC_WAIT(WAIT) (((WAIT) == SDMMC_WAIT_NO) || \
                             ((WAIT) == SDMMC_WAIT_IT) || \
                             ((WAIT) == SDMMC_WAIT_PEND))
/**
  * @}
  */

/** @defgroup SDMMC_LL_CPSM_State CPSM State
  * @{
  */
#define SDMMC_CPSM_DISABLE                   ((uint32_t)0x00000000U)
#define SDMMC_CPSM_ENABLE                    SDMMC_CMD_CPSMEN

#define IS_SDMMC_CPSM(CPSM) (((CPSM) == SDMMC_CPSM_DISABLE) || \
                             ((CPSM) == SDMMC_CPSM_ENABLE))
/**
  * @}
  */

/** @defgroup SDMMC_LL_Response_Registers Response Register
  * @{
  */
#define SDMMC_RESP1                          ((uint32_t)0x00000000U)
#define SDMMC_RESP2                          ((uint32_t)0x00000004U)
#define SDMMC_RESP3                          ((uint32_t)0x00000008U)
#define SDMMC_RESP4                          ((uint32_t)0x0000000CU)

#define IS_SDMMC_RESP(RESP) (((RESP) == SDMMC_RESP1) || \
                             ((RESP) == SDMMC_RESP2) || \
                             ((RESP) == SDMMC_RESP3) || \
                             ((RESP) == SDMMC_RESP4))

/** @defgroup SDMMC_Internal_DMA_Mode  SDMMC Internal DMA Mode
  * @{
  */
#define SDMMC_DISABLE_IDMA              ((uint32_t)0x00000000)
#define SDMMC_ENABLE_IDMA_SINGLE_BUFF   (SDMMC_IDMA_IDMAEN)
#define SDMMC_ENABLE_IDMA_DOUBLE_BUFF0  (SDMMC_IDMA_IDMAEN | SDMMC_IDMA_IDMABMODE)
#define SDMMC_ENABLE_IDMA_DOUBLE_BUFF1  (SDMMC_IDMA_IDMAEN | SDMMC_IDMA_IDMABMODE | SDMMC_IDMA_IDMABACT)

/**
  * @}
  */

/** @defgroup SDMMC_LL_Data_Length Data Length
  * @{
  */
#define IS_SDMMC_DATA_LENGTH(LENGTH) ((LENGTH) <= 0x01FFFFFFU)
/**
  * @}
  */

/** @defgroup SDMMC_LL_Data_Block_Size  Data Block Size
  * @{
  */
#define SDMMC_DATABLOCK_SIZE_1B               ((uint32_t)0x00000000U)
#define SDMMC_DATABLOCK_SIZE_2B               SDMMC_DCTRL_DBLOCKSIZE_0
#define SDMMC_DATABLOCK_SIZE_4B               SDMMC_DCTRL_DBLOCKSIZE_1
#define SDMMC_DATABLOCK_SIZE_8B               (SDMMC_DCTRL_DBLOCKSIZE_0|SDMMC_DCTRL_DBLOCKSIZE_1)
#define SDMMC_DATABLOCK_SIZE_16B              SDMMC_DCTRL_DBLOCKSIZE_2
#define SDMMC_DATABLOCK_SIZE_32B              (SDMMC_DCTRL_DBLOCKSIZE_0|SDMMC_DCTRL_DBLOCKSIZE_2)
#define SDMMC_DATABLOCK_SIZE_64B              (SDMMC_DCTRL_DBLOCKSIZE_1|SDMMC_DCTRL_DBLOCKSIZE_2)
#define SDMMC_DATABLOCK_SIZE_128B             (SDMMC_DCTRL_DBLOCKSIZE_0| \
                                               SDMMC_DCTRL_DBLOCKSIZE_1|SDMMC_DCTRL_DBLOCKSIZE_2)
#define SDMMC_DATABLOCK_SIZE_256B             SDMMC_DCTRL_DBLOCKSIZE_3
#define SDMMC_DATABLOCK_SIZE_512B             (SDMMC_DCTRL_DBLOCKSIZE_0|SDMMC_DCTRL_DBLOCKSIZE_3)
#define SDMMC_DATABLOCK_SIZE_1024B            (SDMMC_DCTRL_DBLOCKSIZE_1|SDMMC_DCTRL_DBLOCKSIZE_3)
#define SDMMC_DATABLOCK_SIZE_2048B            (SDMMC_DCTRL_DBLOCKSIZE_0| \
                                               SDMMC_DCTRL_DBLOCKSIZE_1|SDMMC_DCTRL_DBLOCKSIZE_3)
#define SDMMC_DATABLOCK_SIZE_4096B            (SDMMC_DCTRL_DBLOCKSIZE_2|SDMMC_DCTRL_DBLOCKSIZE_3)
#define SDMMC_DATABLOCK_SIZE_8192B            (SDMMC_DCTRL_DBLOCKSIZE_0| \
                                               SDMMC_DCTRL_DBLOCKSIZE_2|SDMMC_DCTRL_DBLOCKSIZE_3)
#define SDMMC_DATABLOCK_SIZE_16384B           (SDMMC_DCTRL_DBLOCKSIZE_1| \
                                               SDMMC_DCTRL_DBLOCKSIZE_2|SDMMC_DCTRL_DBLOCKSIZE_3)

#define IS_SDMMC_BLOCK_SIZE(SIZE) (((SIZE) == SDMMC_DATABLOCK_SIZE_1B)    || \
                                   ((SIZE) == SDMMC_DATABLOCK_SIZE_2B)    || \
                                   ((SIZE) == SDMMC_DATABLOCK_SIZE_4B)    || \
                                   ((SIZE) == SDMMC_DATABLOCK_SIZE_8B)    || \
                                   ((SIZE) == SDMMC_DATABLOCK_SIZE_16B)   || \
                                   ((SIZE) == SDMMC_DATABLOCK_SIZE_32B)   || \
                                   ((SIZE) == SDMMC_DATABLOCK_SIZE_64B)   || \
                                   ((SIZE) == SDMMC_DATABLOCK_SIZE_128B)  || \
                                   ((SIZE) == SDMMC_DATABLOCK_SIZE_256B)  || \
                                   ((SIZE) == SDMMC_DATABLOCK_SIZE_512B)  || \
                                   ((SIZE) == SDMMC_DATABLOCK_SIZE_1024B) || \
                                   ((SIZE) == SDMMC_DATABLOCK_SIZE_2048B) || \
                                   ((SIZE) == SDMMC_DATABLOCK_SIZE_4096B) || \
                                   ((SIZE) == SDMMC_DATABLOCK_SIZE_8192B) || \
                                   ((SIZE) == SDMMC_DATABLOCK_SIZE_16384B))
/**
  * @}
  */

/** @defgroup SDMMC_LL_Transfer_Direction Transfer Direction
  * @{
  */
#define SDMMC_TRANSFER_DIR_TO_CARD            ((uint32_t)0x00000000U)
#define SDMMC_TRANSFER_DIR_TO_SDMMC            SDMMC_DCTRL_DTDIR

#define IS_SDMMC_TRANSFER_DIR(DIR) (((DIR) == SDMMC_TRANSFER_DIR_TO_CARD) || \
                                    ((DIR) == SDMMC_TRANSFER_DIR_TO_SDMMC))
/**
  * @}
  */

/** @defgroup SDMMC_LL_Transfer_Type Transfer Type
  * @{
  */
#define SDMMC_TRANSFER_MODE_BLOCK             ((uint32_t)0x00000000U)
#define SDMMC_TRANSFER_MODE_SDIO              SDMMC_DCTRL_DTMODE_0
#define SDMMC_TRANSFER_MODE_STREAM            SDMMC_DCTRL_DTMODE_1

#define IS_SDMMC_TRANSFER_MODE(MODE) (((MODE) == SDMMC_TRANSFER_MODE_BLOCK) || \
                                      ((MODE) == SDMMC_TRANSFER_MODE_SDIO)  || \
                                      ((MODE) == SDMMC_TRANSFER_MODE_STREAM))
/**
  * @}
  */

/** @defgroup SDMMC_LL_DPSM_State DPSM State
  * @{
  */
#define SDMMC_DPSM_DISABLE                    ((uint32_t)0x00000000U)
#define SDMMC_DPSM_ENABLE                     SDMMC_DCTRL_DTEN

#define IS_SDMMC_DPSM(DPSM) (((DPSM) == SDMMC_DPSM_DISABLE) ||\
                             ((DPSM) == SDMMC_DPSM_ENABLE))
/**
  * @}
  */

/** @defgroup SDMMC_LL_Read_Wait_Mode Read Wait Mode
  * @{
  */
#define SDMMC_READ_WAIT_MODE_DATA2                ((uint32_t)0x00000000U)
#define SDMMC_READ_WAIT_MODE_CLK                  (SDMMC_DCTRL_RWMOD)

#define IS_SDMMC_READWAIT_MODE(MODE) (((MODE) == SDMMC_READ_WAIT_MODE_CLK) || \
                                      ((MODE) == SDMMC_READ_WAIT_MODE_DATA2))
/**
  * @}
  */

/** @defgroup SDMMC_LL_Interrupt_sources Interrupt Sources
  * @{
  */
#define SDMMC_IT_CCRCFAIL                  SDMMC_MASK_CCRCFAILIE
#define SDMMC_IT_DCRCFAIL                  SDMMC_MASK_DCRCFAILIE
#define SDMMC_IT_CTIMEOUT                  SDMMC_MASK_CTIMEOUTIE
#define SDMMC_IT_DTIMEOUT                  SDMMC_MASK_DTIMEOUTIE
#define SDMMC_IT_TXUNDERR                  SDMMC_MASK_TXUNDERRIE
#define SDMMC_IT_RXOVERR                   SDMMC_MASK_RXOVERRIE
#define SDMMC_IT_CMDREND                   SDMMC_MASK_CMDRENDIE
#define SDMMC_IT_CMDSENT                   SDMMC_MASK_CMDSENTIE
#define SDMMC_IT_DATAEND                   SDMMC_MASK_DATAENDIE
#define SDMMC_IT_DHOLD                     SDMMC_MASK_DHOLDIE
#define SDMMC_IT_DBCKEND                   SDMMC_MASK_DBCKENDIE
#define SDMMC_IT_DABORT                    SDMMC_MASK_DABORTIE
#define SDMMC_IT_TXFIFOHE                  SDMMC_MASK_TXFIFOHEIE
#define SDMMC_IT_RXFIFOHF                  SDMMC_MASK_RXFIFOHFIE
#define SDMMC_IT_RXFIFOF                   SDMMC_MASK_RXFIFOFIE
#define SDMMC_IT_TXFIFOE                   SDMMC_MASK_TXFIFOEIE
#define SDMMC_IT_BUSYD0END                 SDMMC_MASK_BUSYD0ENDIE
#define SDMMC_IT_SDIOIT                    SDMMC_MASK_SDIOITIE
#define SDMMC_IT_ACKFAIL                   SDMMC_MASK_ACKFAILIE
#define SDMMC_IT_ACKTIMEOUT                SDMMC_MASK_ACKTIMEOUTIE
#define SDMMC_IT_VSWEND                    SDMMC_MASK_VSWENDIE
#define SDMMC_IT_CKSTOP                    SDMMC_MASK_CKSTOPIE
#define SDMMC_IT_IDMABTC                   SDMMC_MASK_IDMABTCIE
/**
  * @}
  */

/** @defgroup SDMMC_LL_Flags Flags
  * @{
  */
#define SDMMC_FLAG_CCRCFAIL                  SDMMC_STA_CCRCFAIL
#define SDMMC_FLAG_DCRCFAIL                  SDMMC_STA_DCRCFAIL
#define SDMMC_FLAG_CTIMEOUT                  SDMMC_STA_CTIMEOUT
#define SDMMC_FLAG_DTIMEOUT                  SDMMC_STA_DTIMEOUT
#define SDMMC_FLAG_TXUNDERR                  SDMMC_STA_TXUNDERR
#define SDMMC_FLAG_RXOVERR                   SDMMC_STA_RXOVERR
#define SDMMC_FLAG_CMDREND                   SDMMC_STA_CMDREND
#define SDMMC_FLAG_CMDSENT                   SDMMC_STA_CMDSENT
#define SDMMC_FLAG_DATAEND                   SDMMC_STA_DATAEND
#define SDMMC_FLAG_DHOLD                     SDMMC_STA_DHOLD
#define SDMMC_FLAG_DBCKEND                   SDMMC_STA_DBCKEND
#define SDMMC_FLAG_DABORT                    SDMMC_STA_DABORT
#define SDMMC_FLAG_DPSMACT                   SDMMC_STA_DPSMACT
#define SDMMC_FLAG_CMDACT                    SDMMC_STA_CPSMACT
#define SDMMC_FLAG_TXFIFOHE                  SDMMC_STA_TXFIFOHE
#define SDMMC_FLAG_RXFIFOHF                  SDMMC_STA_RXFIFOHF
#define SDMMC_FLAG_TXFIFOF                   SDMMC_STA_TXFIFOF
#define SDMMC_FLAG_RXFIFOF                   SDMMC_STA_RXFIFOF
#define SDMMC_FLAG_TXFIFOE                   SDMMC_STA_TXFIFOE
#define SDMMC_FLAG_RXFIFOE                   SDMMC_STA_RXFIFOE
#define SDMMC_FLAG_BUSYD0                    SDMMC_STA_BUSYD0
#define SDMMC_FLAG_BUSYD0END                 SDMMC_STA_BUSYD0END
#define SDMMC_FLAG_SDIOIT                    SDMMC_STA_SDIOIT
#define SDMMC_FLAG_ACKFAIL                   SDMMC_STA_ACKFAIL
#define SDMMC_FLAG_ACKTIMEOUT                SDMMC_STA_ACKTIMEOUT
#define SDMMC_FLAG_VSWEND                    SDMMC_STA_VSWEND
#define SDMMC_FLAG_CKSTOP                    SDMMC_STA_CKSTOP
#define SDMMC_FLAG_IDMATE                    SDMMC_STA_IDMATE
#define SDMMC_FLAG_IDMABTC                   SDMMC_STA_IDMABTC

#define SDMMC_STATIC_FLAGS             ((uint32_t)(SDMMC_FLAG_CCRCFAIL   | SDMMC_FLAG_DCRCFAIL | SDMMC_FLAG_CTIMEOUT |\
                                                   SDMMC_FLAG_DTIMEOUT   | SDMMC_FLAG_TXUNDERR | SDMMC_FLAG_RXOVERR  |\
                                                   SDMMC_FLAG_CMDREND    | SDMMC_FLAG_CMDSENT  | SDMMC_FLAG_DATAEND  |\
                                                   SDMMC_FLAG_DHOLD      | SDMMC_FLAG_DBCKEND  | SDMMC_FLAG_DABORT   |\
                                                   SDMMC_FLAG_BUSYD0END  | SDMMC_FLAG_SDIOIT   | SDMMC_FLAG_ACKFAIL  |\
                                                   SDMMC_FLAG_ACKTIMEOUT | SDMMC_FLAG_VSWEND   | SDMMC_FLAG_CKSTOP   |\
                                                   SDMMC_FLAG_IDMATE     | SDMMC_FLAG_IDMABTC))

#define SDMMC_STATIC_CMD_FLAGS         ((uint32_t)(SDMMC_FLAG_CCRCFAIL | SDMMC_FLAG_CTIMEOUT  | SDMMC_FLAG_CMDREND   |\
                                                   SDMMC_FLAG_CMDSENT  | SDMMC_FLAG_BUSYD0END))

#define SDMMC_STATIC_DATA_FLAGS        ((uint32_t)(SDMMC_FLAG_DCRCFAIL | SDMMC_FLAG_DTIMEOUT | SDMMC_FLAG_TXUNDERR   |\
                                                   SDMMC_FLAG_RXOVERR  | SDMMC_FLAG_DATAEND  | SDMMC_FLAG_DHOLD      |\
                                                   SDMMC_FLAG_DBCKEND  | SDMMC_FLAG_DABORT   | SDMMC_FLAG_IDMATE     |\
                                                   SDMMC_FLAG_IDMABTC))
/**
  * @}
  */

/** @defgroup SDMMC_SDIO_CCCR_Registers
  * @{
  */
/*-------------------------------- CCCR0 ----------------------------------*/
#define SDMMC_SDIO_CCCR0                     0x000U  /*!< SDIOS Card Common Control Register 0        */
#define SDMMC_SDIO_CCCR0_SD_BYTE0            0x000U  /*!< SDIOS Card Common Control Register 0 Byte 0 */
#define SDMMC_SDIO_CCCR0_SD_BYTE1            0x001U  /*!< SDIOS Card Common Control Register 0 Byte 1 */
#define SDMMC_SDIO_CCCR0_SD_BYTE2            0x002U  /*!< SDIOS Card Common Control Register 0 Byte 2 */
#define SDMMC_SDIO_CCCR0_SD_BYTE3            0x003U  /*!< SDIOS Card Common Control Register 0 Byte 3 */

/*-------------------------------- CCCR4 ----------------------------------*/
#define SDMMC_SDIO_CCCR4                     0x004U  /*!< SDIOS Card Common Control Register 4        */
#define SDMMC_SDIO_CCCR4_SD_BYTE0            0x004U  /*!< SDIOS Card Common Control Register 4 Byte 0 */
#define SDMMC_SDIO_CCCR4_SD_BYTE1            0x005U  /*!< SDIOS Card Common Control Register 4 Byte 1 */
#define SDMMC_SDIO_CCCR4_SD_BYTE2            0x006U  /*!< SDIOS Card Common Control Register 4 Byte 2 */
#define SDMMC_SDIO_CCCR4_SD_BYTE3            0x007U  /*!< SDIOS Card Common Control Register 4 Byte 3 */

/*-------------------------------- CCCR8 ----------------------------------*/
#define SDMMC_SDIO_CCCR8                     0x008U  /*!< SDIOS Card Common Control Register 8        */
#define SDMMC_SDIO_CCCR8_SD_BYTE0            0x008U  /*!< SDIOS Card Common Control Register 8 Byte 0 */
#define SDMMC_SDIO_CCCR8_SD_BYTE1            0x009U  /*!< SDIOS Card Common Control Register 8 Byte 1 */
#define SDMMC_SDIO_CCCR8_SD_BYTE2            0x00AU  /*!< SDIOS Card Common Control Register 8 Byte 2 */
#define SDMMC_SDIO_CCCR8_SD_BYTE3            0x00BU  /*!< SDIOS Card Common Control Register 8 Byte 3 */

/*-------------------------------- CCCR12 ---------------------------------*/
#define SDMMC_SDIO_CCCR12                    0x00CU  /*!< SDIOS Card Common Control Register 12        */
#define SDMMC_SDIO_CCCR12_SD_BYTE0           0x00CU  /*!< SDIOS Card Common Control Register 12 Byte 0 */
#define SDMMC_SDIO_CCCR12_SD_BYTE1           0x00DU  /*!< SDIOS Card Common Control Register 12 Byte 1 */
#define SDMMC_SDIO_CCCR12_SD_BYTE2           0x00EU  /*!< SDIOS Card Common Control Register 12 Byte 2 */
#define SDMMC_SDIO_CCCR12_SD_BYTE3           0x00FU  /*!< SDIOS Card Common Control Register 12 Byte 3 */

/*-------------------------------- CCCR16 ---------------------------------*/
#define SDMMC_SDIO_CCCR16                    0x010U  /*!< SDIOS Card Common Control Register 16        */
#define SDMMC_SDIO_CCCR16_SD_BYTE0           0x010U  /*!< SDIOS Card Common Control Register 16 Byte 0 */
#define SDMMC_SDIO_CCCR16_SD_BYTE1           0x011U  /*!< SDIOS Card Common Control Register 16 Byte 1 */
#define SDMMC_SDIO_CCCR16_SD_BYTE2           0x012U  /*!< SDIOS Card Common Control Register 16 Byte 2 */
#define SDMMC_SDIO_CCCR16_SD_BYTE3           0x013U  /*!< SDIOS Card Common Control Register 16 Byte 3 */

/*-------------------------------- CCCR20 ---------------------------------*/
#define SDMMC_SDIO_CCCR20                    0x014U  /*!< SDIOS Card Common Control Register 20        */
#define SDMMC_SDIO_CCCR20_SD_BYTE0           0x014U  /*!< SDIOS Card Common Control Register 20 Byte 0 */
#define SDMMC_SDIO_CCCR20_SD_BYTE1           0x015U  /*!< SDIOS Card Common Control Register 20 Byte 1 */
#define SDMMC_SDIO_CCCR20_SD_BYTE2           0x016U  /*!< SDIOS Card Common Control Register 20 Byte 2 */
#define SDMMC_SDIO_CCCR20_SD_BYTE3           0x017U  /*!< SDIOS Card Common Control Register 20 Byte 3 */

/*-------------------------------- F1BR0 ----------------------------------*/
#define SDMMC_SDIO_F1BR0                     0x100U  /*!< SDIOS Function 1 Basic Register 0        */
#define SDMMC_SDIO_F1BR0_SD_BYTE0            0x100U  /*!< SDIOS Function 1 Basic Register 0 Byte 0 */
#define SDMMC_SDIO_F1BR0_SD_BYTE1            0x101U  /*!< SDIOS Function 1 Basic Register 0 Byte 1 */
#define SDMMC_SDIO_F1BR0_SD_BYTE2            0x102U  /*!< SDIOS Function 1 Basic Register 0 Byte 2 */
#define SDMMC_SDIO_F1BR0_SD_BYTE3            0x103U  /*!< SDIOS Function 1 Basic Register 0 Byte 3 */

/*-------------------------------- F1BR8 ----------------------------------*/
#define SDMMC_SDIO_F1BR8                     0x108U  /*!< SDIOS Function 1 Basic Register 8        */
#define SDMMC_SDIO_F1BR8_SD_BYTE0            0x108U  /*!< SDIOS Function 1 Basic Register 8 Byte 0 */
#define SDMMC_SDIO_F1BR8_SD_BYTE1            0x109U  /*!< SDIOS Function 1 Basic Register 8 Byte 1 */
#define SDMMC_SDIO_F1BR8_SD_BYTE2            0x10AU  /*!< SDIOS Function 1 Basic Register 8 Byte 2 */
#define SDMMC_SDIO_F1BR8_SD_BYTE3            0x10BU  /*!< SDIOS Function 1 Basic Register 8 Byte 3 */

/*-------------------------------- F1BR12 ---------------------------------*/
#define SDMMC_SDIO_F1BR12                    0x10CU  /*!< SDIOS Function 1 Basic Register 12        */
#define SDMMC_SDIO_F1BR12_SD_BYTE0           0x10CU  /*!< SDIOS Function 1 Basic Register 12 Byte 0 */
#define SDMMC_SDIO_F1BR12_SD_BYTE1           0x10DU  /*!< SDIOS Function 1 Basic Register 12 Byte 1 */
#define SDMMC_SDIO_F1BR12_SD_BYTE2           0x10EU  /*!< SDIOS Function 1 Basic Register 12 Byte 2 */
#define SDMMC_SDIO_F1BR12_SD_BYTE3           0x10FU  /*!< SDIOS Function 1 Basic Register 12 Byte 3 */

/*-------------------------------- F1BR16 ---------------------------------*/
#define SDMMC_SDIO_F1BR16                    0x110U  /*!< SDIOS Function 1 Basic Register 16        */
#define SDMMC_SDIO_F1BR16_SD_BYTE0           0x110U  /*!< SDIOS Function 1 Basic Register 16 Byte 0 */
#define SDMMC_SDIO_F1BR16_SD_BYTE1           0x111U  /*!< SDIOS Function 1 Basic Register 16 Byte 1 */
#define SDMMC_SDIO_F1BR16_SD_BYTE2           0x112U  /*!< SDIOS Function 1 Basic Register 16 Byte 2 */
#define SDMMC_SDIO_F1BR16_SD_BYTE3           0x113U  /*!< SDIOS Function 1 Basic Register 16 Byte 3 */
/**
  * @}
  */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
/** @defgroup SDMMC_LL_Exported_macros SDMMC_LL Exported Macros
  * @{
  */

/** @defgroup SDMMC_LL_Register Bits And Addresses Definitions
  * @brief SDMMC_LL registers bit address in the alias region
  * @{
  */
/* ---------------------- SDMMC registers bit mask --------------------------- */
/* --- CLKCR Register ---*/
/* CLKCR register clear mask */
#define CLKCR_CLEAR_MASK         ((uint32_t)(SDMMC_CLKCR_CLKDIV  | SDMMC_CLKCR_PWRSAV |\
                                             SDMMC_CLKCR_WIDBUS |\
                                             SDMMC_CLKCR_NEGEDGE | SDMMC_CLKCR_HWFC_EN |\
                                             SDMMC_CLKCR_DDR | SDMMC_CLKCR_BUSSPEED |\
                                             SDMMC_CLKCR_SELCLKRX))

/* --- DCTRL Register ---*/
/* SDMMC DCTRL Clear Mask */
#define DCTRL_CLEAR_MASK         ((uint32_t)(SDMMC_DCTRL_DTEN    | SDMMC_DCTRL_DTDIR |\
                                             SDMMC_DCTRL_DTMODE  | SDMMC_DCTRL_DBLOCKSIZE))

/* --- CMD Register ---*/
/* CMD Register clear mask */
#define CMD_CLEAR_MASK           ((uint32_t)(SDMMC_CMD_CMDINDEX | SDMMC_CMD_WAITRESP |\
                                             SDMMC_CMD_WAITINT  | SDMMC_CMD_WAITPEND |\
                                             SDMMC_CMD_CPSMEN   | SDMMC_CMD_CMDSUSPEND))

/* SDMMC Initialization Frequency (400KHz max) for Peripheral CLK 200MHz*/
#define SDMMC_INIT_CLK_DIV ((uint8_t)0xFA)

/* SDMMC Default Speed Frequency (25Mhz max) for Peripheral CLK 200MHz*/
#define SDMMC_NSPEED_CLK_DIV ((uint8_t)0x4)

/* SDMMC High Speed Frequency (50Mhz max) for Peripheral CLK 200MHz*/
#define SDMMC_HSPEED_CLK_DIV ((uint8_t)0x2)
/**
  * @}
  */

/** @defgroup SDMMC_LL_Interrupt_Clock Interrupt And Clock Configuration
  *  @brief macros to handle interrupts and specific clock configurations
  * @{
  */

/**
  * @brief  Enable the SDMMC device interrupt.
  * @param  __INSTANCE__ Pointer to SDMMC register base
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
  *            @arg SDMMC_IT_SDIOIT:     SDIO interrupt received interrupt
  *            @arg SDMMC_IT_ACKFAIL:    Boot Acknowledgment received interrupt
  *            @arg SDMMC_IT_ACKTIMEOUT: Boot Acknowledgment timeout interrupt
  *            @arg SDMMC_IT_VSWEND:     Voltage switch critical timing section completion interrupt
  *            @arg SDMMC_IT_CKSTOP:     SDMMC_CK stopped in Voltage switch procedure interrupt
  *            @arg SDMMC_IT_IDMABTC:    IDMA buffer transfer complete interrupt
  * @retval None
  */
#define __SDMMC_ENABLE_IT(__INSTANCE__, __INTERRUPT__)  ((__INSTANCE__)->MASK |= (__INTERRUPT__))

/**
  * @brief  Disable the SDMMC device interrupt.
  * @param  __INSTANCE__ Pointer to SDMMC register base
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
  *            @arg SDMMC_IT_SDIOIT:     SDIO interrupt received interrupt
  *            @arg SDMMC_IT_ACKFAIL:    Boot Acknowledgment received interrupt
  *            @arg SDMMC_IT_ACKTIMEOUT: Boot Acknowledgment timeout interrupt
  *            @arg SDMMC_IT_VSWEND:     Voltage switch critical timing section completion interrupt
  *            @arg SDMMC_IT_CKSTOP:     SDMMC_CK stopped in Voltage switch procedure interrupt
  *            @arg SDMMC_IT_IDMABTC:    IDMA buffer transfer complete interrupt
  * @retval None
  */
#define __SDMMC_DISABLE_IT(__INSTANCE__, __INTERRUPT__)  ((__INSTANCE__)->MASK &= ~(__INTERRUPT__))

/**
  * @brief  Checks whether the specified SDMMC flag is set or not.
  * @param  __INSTANCE__ Pointer to SDMMC register base
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
  *            @arg SDMMC_FLAG_SDIOIT:     SDIO interrupt received
  *            @arg SDMMC_FLAG_ACKFAIL:    Boot Acknowledgment received
  *            @arg SDMMC_FLAG_ACKTIMEOUT: Boot Acknowledgment timeout
  *            @arg SDMMC_FLAG_VSWEND:     Voltage switch critical timing section completion
  *            @arg SDMMC_FLAG_CKSTOP:     SDMMC_CK stopped in Voltage switch procedure
  *            @arg SDMMC_FLAG_IDMATE:     IDMA transfer error
  *            @arg SDMMC_FLAG_IDMABTC:    IDMA buffer transfer complete
  * @retval The new state of SDMMC_FLAG (SET or RESET).
  */
#define __SDMMC_GET_FLAG(__INSTANCE__, __FLAG__)  (((__INSTANCE__)->STA &(__FLAG__)) != 0U)


/**
  * @brief  Clears the SDMMC pending flags.
  * @param  __INSTANCE__ Pointer to SDMMC register base
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
  *            @arg SDMMC_FLAG_SDIOIT:     SDIO interrupt received
  *            @arg SDMMC_FLAG_ACKFAIL:    Boot Acknowledgment received
  *            @arg SDMMC_FLAG_ACKTIMEOUT: Boot Acknowledgment timeout
  *            @arg SDMMC_FLAG_VSWEND:     Voltage switch critical timing section completion
  *            @arg SDMMC_FLAG_CKSTOP:     SDMMC_CK stopped in Voltage switch procedure
  *            @arg SDMMC_FLAG_IDMATE:     IDMA transfer error
  *            @arg SDMMC_FLAG_IDMABTC:    IDMA buffer transfer complete
  * @retval None
  */
#define __SDMMC_CLEAR_FLAG(__INSTANCE__, __FLAG__)  ((__INSTANCE__)->ICR = (__FLAG__))

/**
  * @brief  Checks whether the specified SDMMC interrupt has occurred or not.
  * @param  __INSTANCE__ Pointer to SDMMC register base
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
  *            @arg SDMMC_IT_SDIOIT:     SDIO interrupt received interrupt
  *            @arg SDMMC_IT_ACKFAIL:    Boot Acknowledgment received interrupt
  *            @arg SDMMC_IT_ACKTIMEOUT: Boot Acknowledgment timeout interrupt
  *            @arg SDMMC_IT_VSWEND:     Voltage switch critical timing section completion interrupt
  *            @arg SDMMC_IT_CKSTOP:     SDMMC_CK stopped in Voltage switch procedure interrupt
  *            @arg SDMMC_IT_IDMABTC:    IDMA buffer transfer complete interrupt
  * @retval The new state of SDMMC_IT (SET or RESET).
  */
#define __SDMMC_GET_IT(__INSTANCE__, __INTERRUPT__)  (((__INSTANCE__)->STA &(__INTERRUPT__)) == (__INTERRUPT__))

/**
  * @brief  Checks the source of specified interrupt.
  * @param  __INSTANCE__ Pointer to SDMMC register base
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
  *            @arg SDMMC_IT_SDIOIT:     SDIO interrupt received interrupt
  *            @arg SDMMC_IT_ACKFAIL:    Boot Acknowledgment received interrupt
  *            @arg SDMMC_IT_ACKTIMEOUT: Boot Acknowledgment timeout interrupt
  *            @arg SDMMC_IT_VSWEND:     Voltage switch critical timing section completion interrupt
  *            @arg SDMMC_IT_CKSTOP:     SDMMC_CK stopped in Voltage switch procedure interrupt
  *            @arg SDMMC_IT_IDMABTC:    IDMA buffer transfer complete interrupt
  * @retval The new state of SDMMC_IT (SET or RESET).
  */
#define __SDMMC_GET_IT_SOURCE(__INSTANCE__, __INTERRUPT__) (((__HANDLE__)->Instance->STA & (__INTERRUPT__)))

/**
  * @brief  Clears the SDMMC's interrupt pending bits.
  * @param  __INSTANCE__ Pointer to SDMMC register base
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
  *            @arg SDMMC_IT_BUSYD0END:  End of SDMMC_D0 Busy following a CMD response detected interrupt
  *            @arg SDMMC_IT_SDIOIT:     SDIO interrupt received interrupt
  *            @arg SDMMC_IT_ACKFAIL:    Boot Acknowledgment received interrupt
  *            @arg SDMMC_IT_ACKTIMEOUT: Boot Acknowledgment timeout interrupt
  *            @arg SDMMC_IT_VSWEND:     Voltage switch critical timing section completion interrupt
  *            @arg SDMMC_IT_CKSTOP:     SDMMC_CK stopped in Voltage switch procedure interrupt
  *            @arg SDMMC_IT_IDMABTC:    IDMA buffer transfer complete interrupt
  * @retval None
  */
#define __SDMMC_CLEAR_IT(__INSTANCE__, __INTERRUPT__)  ((__INSTANCE__)->ICR = (__INTERRUPT__))

/**
  * @brief  Enable Start the SD I/O Read Wait operation.
  * @param  __INSTANCE__ Pointer to SDMMC register base
  * @retval None
  */
#define __SDMMC_START_READWAIT_ENABLE(__INSTANCE__)  ((__INSTANCE__)->DCTRL |= SDMMC_DCTRL_RWSTART)

/**
  * @brief  Disable Start the SD I/O Read Wait operations.
  * @param  __INSTANCE__ Pointer to SDMMC register base
  * @retval None
  */
#define __SDMMC_START_READWAIT_DISABLE(__INSTANCE__)  ((__INSTANCE__)->DCTRL &= ~SDMMC_DCTRL_RWSTART)

/**
  * @brief  Enable Start the SD I/O Read Wait operation.
  * @param  __INSTANCE__ Pointer to SDMMC register base
  * @retval None
  */
#define __SDMMC_STOP_READWAIT_ENABLE(__INSTANCE__)  ((__INSTANCE__)->DCTRL |= SDMMC_DCTRL_RWSTOP)

/**
  * @brief  Disable Stop the SD I/O Read Wait operations.
  * @param  __INSTANCE__ Pointer to SDMMC register base
  * @retval None
  */
#define __SDMMC_STOP_READWAIT_DISABLE(__INSTANCE__)  ((__INSTANCE__)->DCTRL &= ~SDMMC_DCTRL_RWSTOP)

/**
  * @brief  Enable the SD I/O Mode Operation.
  * @param  __INSTANCE__ Pointer to SDMMC register base
  * @retval None
  */
#define __SDMMC_OPERATION_ENABLE(__INSTANCE__)  ((__INSTANCE__)->DCTRL |= SDMMC_DCTRL_SDIOEN)

/**
  * @brief  Disable the SD I/O Mode Operation.
  * @param  __INSTANCE__ Pointer to SDMMC register base
  * @retval None
  */
#define __SDMMC_OPERATION_DISABLE(__INSTANCE__)  ((__INSTANCE__)->DCTRL &= ~SDMMC_DCTRL_SDIOEN)

/**
  * @brief  Enable the SD I/O Suspend command sending.
  * @param  __INSTANCE__ Pointer to SDMMC register base
  * @retval None
  */
#define __SDMMC_SUSPEND_CMD_ENABLE(__INSTANCE__)  ((__INSTANCE__)->CMD |= SDMMC_CMD_CMDSUSPEND)

/**
  * @brief  Disable the SD I/O Suspend command sending.
  * @param  __INSTANCE__ Pointer to SDMMC register base
  * @retval None
  */
#define __SDMMC_SUSPEND_CMD_DISABLE(__INSTANCE__)  ((__INSTANCE__)->CMD &= ~SDMMC_CMD_CMDSUSPEND)

/**
  * @brief  Enable the CMDTRANS mode.
  * @param  __INSTANCE__ Pointer to SDMMC register base
  * @retval None
  */
#define __SDMMC_CMDTRANS_ENABLE(__INSTANCE__)  ((__INSTANCE__)->CMD |= SDMMC_CMD_CMDTRANS)

/**
  * @brief  Disable the CMDTRANS mode.
  * @param  __INSTANCE__ Pointer to SDMMC register base
  * @retval None
  */
#define __SDMMC_CMDTRANS_DISABLE(__INSTANCE__)  ((__INSTANCE__)->CMD &= ~SDMMC_CMD_CMDTRANS)

/**
  * @brief  Enable the CMDSTOP mode.
  * @param  __INSTANCE__ Pointer to SDMMC register base
  * @retval None
  */
#define __SDMMC_CMDSTOP_ENABLE(__INSTANCE__)  ((__INSTANCE__)->CMD |= SDMMC_CMD_CMDSTOP)

/**
  * @brief  Disable the CMDSTOP mode.
  * @param  __INSTANCE__ Pointer to SDMMC register base
  * @retval None
  */
#define __SDMMC_CMDSTOP_DISABLE(__INSTANCE__)  ((__INSTANCE__)->CMD &= ~SDMMC_CMD_CMDSTOP)

/**
  * @}
  */

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @addtogroup SDMMC_LL_Exported_Functions
  * @{
  */

/* Initialization/de-initialization functions  **********************************/
/** @addtogroup HAL_SDMMC_LL_Group1
  * @{
  */
HAL_StatusTypeDef SDMMC_Init(SDMMC_TypeDef *SDMMCx, SDMMC_InitTypeDef Init);
/**
  * @}
  */

/* I/O operation functions  *****************************************************/
/** @addtogroup HAL_SDMMC_LL_Group2
  * @{
  */
uint32_t          SDMMC_ReadFIFO(const SDMMC_TypeDef *SDMMCx);
HAL_StatusTypeDef SDMMC_WriteFIFO(SDMMC_TypeDef *SDMMCx, uint32_t *pWriteData);
/**
  * @}
  */

/* Peripheral Control functions  ************************************************/
/** @addtogroup HAL_SDMMC_LL_Group3
  * @{
  */
HAL_StatusTypeDef SDMMC_PowerState_ON(SDMMC_TypeDef *SDMMCx);
HAL_StatusTypeDef SDMMC_PowerState_Cycle(SDMMC_TypeDef *SDMMCx);
HAL_StatusTypeDef SDMMC_PowerState_OFF(SDMMC_TypeDef *SDMMCx);
uint32_t          SDMMC_GetPowerState(const SDMMC_TypeDef *SDMMCx);

/* Command path state machine (CPSM) management functions */
HAL_StatusTypeDef SDMMC_SendCommand(SDMMC_TypeDef *SDMMCx, const SDMMC_CmdInitTypeDef *Command);
uint8_t           SDMMC_GetCommandResponse(const SDMMC_TypeDef *SDMMCx);
uint32_t          SDMMC_GetResponse(const SDMMC_TypeDef *SDMMCx, uint32_t Response);

/* Data path state machine (DPSM) management functions */
HAL_StatusTypeDef SDMMC_ConfigData(SDMMC_TypeDef *SDMMCx, const SDMMC_DataInitTypeDef *Data);
uint32_t          SDMMC_GetDataCounter(const SDMMC_TypeDef *SDMMCx);
uint32_t          SDMMC_GetFIFOCount(const SDMMC_TypeDef *SDMMCx);

/* SDMMC Cards mode management functions */
HAL_StatusTypeDef SDMMC_SetSDMMCReadWaitMode(SDMMC_TypeDef *SDMMCx, uint32_t SDMMC_ReadWaitMode);
/**
  * @}
  */

/* SDMMC Commands management functions ******************************************/
/** @addtogroup HAL_SDMMC_LL_Group4
  * @{
  */
uint32_t SDMMC_CmdBlockLength(SDMMC_TypeDef *SDMMCx, uint32_t BlockSize);
uint32_t SDMMC_CmdReadSingleBlock(SDMMC_TypeDef *SDMMCx, uint32_t ReadAdd);
uint32_t SDMMC_CmdReadMultiBlock(SDMMC_TypeDef *SDMMCx, uint32_t ReadAdd);
uint32_t SDMMC_CmdWriteSingleBlock(SDMMC_TypeDef *SDMMCx, uint32_t WriteAdd);
uint32_t SDMMC_CmdWriteMultiBlock(SDMMC_TypeDef *SDMMCx, uint32_t WriteAdd);
uint32_t SDMMC_CmdEraseStartAdd(SDMMC_TypeDef *SDMMCx, uint32_t StartAdd);
uint32_t SDMMC_CmdSDEraseStartAdd(SDMMC_TypeDef *SDMMCx, uint32_t StartAdd);
uint32_t SDMMC_CmdEraseEndAdd(SDMMC_TypeDef *SDMMCx, uint32_t EndAdd);
uint32_t SDMMC_CmdSDEraseEndAdd(SDMMC_TypeDef *SDMMCx, uint32_t EndAdd);
uint32_t SDMMC_CmdErase(SDMMC_TypeDef *SDMMCx, uint32_t EraseType);
uint32_t SDMMC_CmdStopTransfer(SDMMC_TypeDef *SDMMCx);
uint32_t SDMMC_CmdSelDesel(SDMMC_TypeDef *SDMMCx, uint32_t Addr);
uint32_t SDMMC_CmdGoIdleState(SDMMC_TypeDef *SDMMCx);
uint32_t SDMMC_CmdOperCond(SDMMC_TypeDef *SDMMCx);
uint32_t SDMMC_CmdAppCommand(SDMMC_TypeDef *SDMMCx, uint32_t Argument);
uint32_t SDMMC_CmdAppOperCommand(SDMMC_TypeDef *SDMMCx, uint32_t Argument);
uint32_t SDMMC_CmdBusWidth(SDMMC_TypeDef *SDMMCx, uint32_t BusWidth);
uint32_t SDMMC_CmdSendSCR(SDMMC_TypeDef *SDMMCx);
uint32_t SDMMC_CmdSendCID(SDMMC_TypeDef *SDMMCx);
uint32_t SDMMC_CmdSendCSD(SDMMC_TypeDef *SDMMCx, uint32_t Argument);
uint32_t SDMMC_CmdSetRelAdd(SDMMC_TypeDef *SDMMCx, uint16_t *pRCA);
uint32_t SDMMC_CmdSetRelAddMmc(SDMMC_TypeDef *SDMMCx, uint16_t RCA);
uint32_t SDMMC_CmdSleepMmc(SDMMC_TypeDef *SDMMCx, uint32_t Argument);
uint32_t SDMMC_CmdSendStatus(SDMMC_TypeDef *SDMMCx, uint32_t Argument);
uint32_t SDMMC_CmdStatusRegister(SDMMC_TypeDef *SDMMCx);
uint32_t SDMMC_CmdVoltageSwitch(SDMMC_TypeDef *SDMMCx);
uint32_t SDMMC_CmdOpCondition(SDMMC_TypeDef *SDMMCx, uint32_t Argument);
uint32_t SDMMC_CmdSwitch(SDMMC_TypeDef *SDMMCx, uint32_t Argument);
uint32_t SDMMC_CmdSendEXTCSD(SDMMC_TypeDef *SDMMCx, uint32_t Argument);
uint32_t SDMMC_CmdBlockCount(SDMMC_TypeDef *SDMMCx, uint32_t BlockCount);
uint32_t SDMMC_SDIO_CmdReadWriteDirect(SDMMC_TypeDef *SDMMCx, uint32_t Argument, uint8_t *pResponse);
uint32_t SDMMC_SDIO_CmdReadWriteExtended(SDMMC_TypeDef *SDMMCx, uint32_t Argument);
uint32_t SDMMC_CmdSendOperationcondition(SDMMC_TypeDef *SDMMCx, uint32_t Argument, uint32_t *pResp);
/**
  * @}
  */

/* SDMMC Responses management functions *****************************************/
/** @addtogroup HAL_SDMMC_LL_Group5
  * @{
  */
uint32_t SDMMC_GetCmdResp1(SDMMC_TypeDef *SDMMCx, uint8_t SD_CMD, uint32_t Timeout);
uint32_t SDMMC_GetCmdResp2(SDMMC_TypeDef *SDMMCx);
uint32_t SDMMC_GetCmdResp3(SDMMC_TypeDef *SDMMCx);
uint32_t SDMMC_GetCmdResp4(SDMMC_TypeDef *SDMMCx, uint32_t *pResp);
uint32_t SDMMC_GetCmdResp5(SDMMC_TypeDef *SDMMCx, uint8_t SDIO_CMD, uint8_t *pData);
uint32_t SDMMC_GetCmdResp6(SDMMC_TypeDef *SDMMCx, uint8_t SD_CMD, uint16_t *pRCA);
uint32_t SDMMC_GetCmdResp7(SDMMC_TypeDef *SDMMCx);
/**
  * @}
  */

/* Linked List functions  *******************************************************/
/** @addtogroup HAL_SDMMC_LL_Group6
  * @{
  */
uint32_t SDMMC_DMALinkedList_BuildNode(SDMMC_DMALinkNodeTypeDef *pNode, const SDMMC_DMALinkNodeConfTypeDef *pNodeConf);
uint32_t SDMMC_DMALinkedList_InsertNode(SDMMC_DMALinkedListTypeDef *pLinkedList, SDMMC_DMALinkNodeTypeDef *pPrevNode,
                                        SDMMC_DMALinkNodeTypeDef *pNode);
uint32_t SDMMC_DMALinkedList_RemoveNode(SDMMC_DMALinkedListTypeDef *pLinkedList, SDMMC_DMALinkNodeTypeDef *pNode);
uint32_t SDMMC_DMALinkedList_LockNode(SDMMC_DMALinkNodeTypeDef *pNode);
uint32_t SDMMC_DMALinkedList_UnlockNode(SDMMC_DMALinkNodeTypeDef *pNode);
uint32_t SDMMC_DMALinkedList_EnableCircularMode(SDMMC_DMALinkedListTypeDef *pLinkedList);
uint32_t SDMMC_DMALinkedList_DisableCircularMode(SDMMC_DMALinkedListTypeDef *pLinkedList);
/**
  * @}
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

#endif /* STM32N6xx_LL_SDMMC_H */
