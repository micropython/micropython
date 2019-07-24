/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_SDMMC_SPEC_H_
#define _FSL_SDMMC_SPEC_H_

#include <stdint.h>

/*!
 * @addtogroup CARD
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*! @brief SD/MMC card initialization clock frequency */
#define SDMMC_CLOCK_400KHZ (400000U)
/*! @brief SD card bus frequency 1 in high-speed mode */
#define SD_CLOCK_25MHZ (25000000U)
/*! @brief SD card bus frequency 2 in high-speed mode */
#define SD_CLOCK_50MHZ (50000000U)
/*! @brief SD card bus frequency in SDR50 mode */
#define SD_CLOCK_100MHZ (100000000U)
/*! @brief SD card bus frequency in SDR104 mode */
#define SD_CLOCK_208MHZ (208000000U)
/*! @brief MMC card bus frequency 1 in high-speed mode */
#define MMC_CLOCK_26MHZ (26000000U)
/*! @brief MMC card bus frequency 2 in high-speed mode */
#define MMC_CLOCK_52MHZ (52000000U)
/*! @brief MMC card bus frequency  in high-speed DDR52 mode */
#define MMC_CLOCK_DDR52 (52000000U)
/*! @brief MMC card bus frequency  in high-speed HS200 mode */
#define MMC_CLOCK_HS200 (200000000U)
/*! @brief MMC card bus frequency  in high-speed HS400 mode */
#define MMC_CLOCK_HS400 (400000000U)

/*!@brief mask convert  */
#define SDMMC_MASK(bit) (1U << (bit))

/*! @brief Card status bit in R1 */
enum _sdmmc_r1_card_status_flag
{
    kSDMMC_R1OutOfRangeFlag                  = 31, /*!< Out of range status bit */
    kSDMMC_R1AddressErrorFlag                = 30, /*!< Address error status bit */
    kSDMMC_R1BlockLengthErrorFlag            = 29, /*!< Block length error status bit */
    kSDMMC_R1EraseSequenceErrorFlag          = 28, /*!< Erase sequence error status bit */
    kSDMMC_R1EraseParameterErrorFlag         = 27, /*!< Erase parameter error status bit */
    kSDMMC_R1WriteProtectViolationFlag       = 26, /*!< Write protection violation status bit */
    kSDMMC_R1CardIsLockedFlag                = 25, /*!< Card locked status bit */
    kSDMMC_R1LockUnlockFailedFlag            = 24, /*!< lock/unlock error status bit */
    kSDMMC_R1CommandCrcErrorFlag             = 23, /*!< CRC error status bit */
    kSDMMC_R1IllegalCommandFlag              = 22, /*!< Illegal command status bit */
    kSDMMC_R1CardEccFailedFlag               = 21, /*!< Card ecc error status bit */
    kSDMMC_R1CardControllerErrorFlag         = 20, /*!< Internal card controller error status bit */
    kSDMMC_R1ErrorFlag                       = 19, /*!< A general or an unknown error status bit */
    kSDMMC_R1CidCsdOverwriteFlag             = 16, /*!< Cid/csd overwrite status bit */
    kSDMMC_R1WriteProtectEraseSkipFlag       = 15, /*!< Write protection erase skip status bit */
    kSDMMC_R1CardEccDisabledFlag             = 14, /*!< Card ecc disabled status bit */
    kSDMMC_R1EraseResetFlag                  = 13, /*!< Erase reset status bit */
    kSDMMC_R1ReadyForDataFlag                = 8,  /*!< Ready for data status bit */
    kSDMMC_R1SwitchErrorFlag                 = 7,  /*!< Switch error status bit */
    kSDMMC_R1ApplicationCommandFlag          = 5,  /*!< Application command enabled status bit */
    kSDMMC_R1AuthenticationSequenceErrorFlag = 3,  /*!< error in the sequence of authentication process */
};

/*! @brief R1 all the error flag */
#define SDMMC_R1_ALL_ERROR_FLAG                                                                      \
    (SDMMC_MASK(kSDMMC_R1OutOfRangeFlag) | SDMMC_MASK(kSDMMC_R1AddressErrorFlag) |                   \
     SDMMC_MASK(kSDMMC_R1BlockLengthErrorFlag) | SDMMC_MASK(kSDMMC_R1EraseSequenceErrorFlag) |       \
     SDMMC_MASK(kSDMMC_R1EraseParameterErrorFlag) | SDMMC_MASK(kSDMMC_R1WriteProtectViolationFlag) | \
     SDMMC_MASK(kSDMMC_R1CardIsLockedFlag) | SDMMC_MASK(kSDMMC_R1LockUnlockFailedFlag) |             \
     SDMMC_MASK(kSDMMC_R1CommandCrcErrorFlag) | SDMMC_MASK(kSDMMC_R1IllegalCommandFlag) |            \
     SDMMC_MASK(kSDMMC_R1CardEccFailedFlag) | SDMMC_MASK(kSDMMC_R1CardControllerErrorFlag) |         \
     SDMMC_MASK(kSDMMC_R1ErrorFlag) | SDMMC_MASK(kSDMMC_R1CidCsdOverwriteFlag) |                     \
     SDMMC_MASK(kSDMMC_R1AuthenticationSequenceErrorFlag))

/*! @brief R1: current state */
#define SDMMC_R1_CURRENT_STATE(x) (((x)&0x00001E00U) >> 9U)

/*! @brief CURRENT_STATE filed in R1 */
typedef enum _sdmmc_r1_current_state
{
    kSDMMC_R1StateIdle        = 0U, /*!< R1: current state: idle */
    kSDMMC_R1StateReady       = 1U, /*!< R1: current state: ready */
    kSDMMC_R1StateIdentify    = 2U, /*!< R1: current state: identification */
    kSDMMC_R1StateStandby     = 3U, /*!< R1: current state: standby */
    kSDMMC_R1StateTransfer    = 4U, /*!< R1: current state: transfer */
    kSDMMC_R1StateSendData    = 5U, /*!< R1: current state: sending data */
    kSDMMC_R1StateReceiveData = 6U, /*!< R1: current state: receiving data */
    kSDMMC_R1StateProgram     = 7U, /*!< R1: current state: programming */
    kSDMMC_R1StateDisconnect  = 8U, /*!< R1: current state: disconnect */
} sdmmc_r1_current_state_t;

/*! @brief Error bit in SPI mode R1 */
enum _sdspi_r1_error_status_flag
{
    kSDSPI_R1InIdleStateFlag        = (1U << 0U), /*!< In idle state */
    kSDSPI_R1EraseResetFlag         = (1U << 1U), /*!< Erase reset */
    kSDSPI_R1IllegalCommandFlag     = (1U << 2U), /*!< Illegal command */
    kSDSPI_R1CommandCrcErrorFlag    = (1U << 3U), /*!< Com crc error */
    kSDSPI_R1EraseSequenceErrorFlag = (1U << 4U), /*!< Erase sequence error */
    kSDSPI_R1AddressErrorFlag       = (1U << 5U), /*!< Address error */
    kSDSPI_R1ParameterErrorFlag     = (1U << 6U), /*!< Parameter error */
};

/*! @brief Error bit in SPI mode R2 */
enum _sdspi_r2_error_status_flag
{
    kSDSPI_R2CardLockedFlag            = (1U << 0U), /*!< Card is locked */
    kSDSPI_R2WriteProtectEraseSkip     = (1U << 1U), /*!< Write protect erase skip */
    kSDSPI_R2LockUnlockFailed          = (1U << 1U), /*!< Lock/unlock command failed */
    kSDSPI_R2ErrorFlag                 = (1U << 2U), /*!< Unknown error */
    kSDSPI_R2CardControllerErrorFlag   = (1U << 3U), /*!< Card controller error */
    kSDSPI_R2CardEccFailedFlag         = (1U << 4U), /*!< Card ecc failed */
    kSDSPI_R2WriteProtectViolationFlag = (1U << 5U), /*!< Write protect violation */
    kSDSPI_R2EraseParameterErrorFlag   = (1U << 6U), /*!< Erase parameter error */
    kSDSPI_R2OutOfRangeFlag            = (1U << 7U), /*!< Out of range */
    kSDSPI_R2CsdOverwriteFlag          = (1U << 7U), /*!< CSD overwrite */
};

/*! @brief The bit mask for COMMAND VERSION field in R7 */
#define SDSPI_R7_VERSION_SHIFT (28U)
/*! @brief The bit mask for COMMAND VERSION field in R7 */
#define SDSPI_R7_VERSION_MASK (0xFU)
/*! @brief The bit shift for VOLTAGE ACCEPTED field in R7 */
#define SDSPI_R7_VOLTAGE_SHIFT (8U)
/*! @brief The bit mask for VOLTAGE ACCEPTED field in R7 */
#define SDSPI_R7_VOLTAGE_MASK (0xFU)
/*! @brief The bit mask for VOLTAGE 2.7V to 3.6V field in R7 */
#define SDSPI_R7_VOLTAGE_27_36_MASK (0x1U << SDSPI_R7_VOLTAGE_SHIFT)
/*! @brief The bit shift for ECHO field in R7 */
#define SDSPI_R7_ECHO_SHIFT (0U)
/*! @brief The bit mask for ECHO field in R7 */
#define SDSPI_R7_ECHO_MASK (0xFFU)

/*! @brief Data error token mask */
#define SDSPI_DATA_ERROR_TOKEN_MASK (0xFU)
/*! @brief Data Error Token mask bit */
enum _sdspi_data_error_token
{
    kSDSPI_DataErrorTokenError               = (1U << 0U), /*!< Data error */
    kSDSPI_DataErrorTokenCardControllerError = (1U << 1U), /*!< Card controller error */
    kSDSPI_DataErrorTokenCardEccFailed       = (1U << 2U), /*!< Card ecc error */
    kSDSPI_DataErrorTokenOutOfRange          = (1U << 3U), /*!< Out of range */
};

/*! @brief Data Token */
typedef enum _sdspi_data_token
{
    kSDSPI_DataTokenBlockRead          = 0xFEU, /*!< Single block read, multiple block read */
    kSDSPI_DataTokenSingleBlockWrite   = 0xFEU, /*!< Single block write */
    kSDSPI_DataTokenMultipleBlockWrite = 0xFCU, /*!< Multiple block write */
    kSDSPI_DataTokenStopTransfer       = 0xFDU, /*!< Stop transmission */
} sdspi_data_token_t;

/* Data Response Token mask */
#define SDSPI_DATA_RESPONSE_TOKEN_MASK (0x1FU) /*!< Mask for data response bits */
/*! @brief Data Response Token */
typedef enum _sdspi_data_response_token
{
    kSDSPI_DataResponseTokenAccepted   = 0x05U, /*!< Data accepted */
    kSDSPI_DataResponseTokenCrcError   = 0x0BU, /*!< Data rejected due to CRC error */
    kSDSPI_DataResponseTokenWriteError = 0x0DU, /*!< Data rejected due to write error */
} sdspi_data_response_token_t;

/*! @brief SD card individual commands */
typedef enum _sd_command
{
    kSD_SendRelativeAddress    = 3U,  /*!< Send Relative Address */
    kSD_Switch                 = 6U,  /*!< Switch Function */
    kSD_SendInterfaceCondition = 8U,  /*!< Send Interface Condition */
    kSD_VoltageSwitch          = 11U, /*!< Voltage Switch */
    kSD_SpeedClassControl      = 20U, /*!< Speed Class control */
    kSD_EraseWriteBlockStart   = 32U, /*!< Write Block Start */
    kSD_EraseWriteBlockEnd     = 33U, /*!< Write Block End */
    kSD_SendTuningBlock        = 19U, /*!< Send Tuning Block */
} sd_command_t;

/*! @brief SDSPI individual commands */
typedef enum _sdspi_command
{
    kSDSPI_CommandCrc = 59U, /*!< Command crc protection on/off */
} sdspi_command_t;

/*! @brief SD card individual application commands */
typedef enum _sd_application_command
{
    kSD_ApplicationSetBusWdith             = 6U,  /*!< Set Bus Width */
    kSD_ApplicationStatus                  = 13U, /*!< Send SD status */
    kSD_ApplicationSendNumberWriteBlocks   = 22U, /*!< Send Number Of Written Blocks */
    kSD_ApplicationSetWriteBlockEraseCount = 23U, /*!< Set Write Block Erase Count */
    kSD_ApplicationSendOperationCondition  = 41U, /*!< Send Operation Condition */
    kSD_ApplicationSetClearCardDetect      = 42U, /*!< Set Connnect/Disconnect pull up on detect pin */
    kSD_ApplicationSendScr                 = 51U, /*!< Send Scr */
} sd_application_command_t;

/*! @brief SD card command class */
enum _sdmmc_command_class
{
    kSDMMC_CommandClassBasic               = (1U << 0U),  /*!< Card command class 0 */
    kSDMMC_CommandClassBlockRead           = (1U << 2U),  /*!< Card command class 2 */
    kSDMMC_CommandClassBlockWrite          = (1U << 4U),  /*!< Card command class 4 */
    kSDMMC_CommandClassErase               = (1U << 5U),  /*!< Card command class 5 */
    kSDMMC_CommandClassWriteProtect        = (1U << 6U),  /*!< Card command class 6 */
    kSDMMC_CommandClassLockCard            = (1U << 7U),  /*!< Card command class 7 */
    kSDMMC_CommandClassApplicationSpecific = (1U << 8U),  /*!< Card command class 8 */
    kSDMMC_CommandClassInputOutputMode     = (1U << 9U),  /*!< Card command class 9 */
    kSDMMC_CommandClassSwitch              = (1U << 10U), /*!< Card command class 10 */
};

/*! @brief OCR register in SD card */
enum _sd_ocr_flag
{
    kSD_OcrPowerUpBusyFlag         = 31,                             /*!< Power up busy status */
    kSD_OcrHostCapacitySupportFlag = 30,                             /*!< Card capacity status */
    kSD_OcrCardCapacitySupportFlag = kSD_OcrHostCapacitySupportFlag, /*!< Card capacity status */
    kSD_OcrSwitch18RequestFlag     = 24,                             /*!< Switch to 1.8V request */
    kSD_OcrSwitch18AcceptFlag      = kSD_OcrSwitch18RequestFlag,     /*!< Switch to 1.8V accepted */
    kSD_OcrVdd27_28Flag            = 15,                             /*!< VDD 2.7-2.8 */
    kSD_OcrVdd28_29Flag            = 16,                             /*!< VDD 2.8-2.9 */
    kSD_OcrVdd29_30Flag            = 17,                             /*!< VDD 2.9-3.0 */
    kSD_OcrVdd30_31Flag            = 18,                             /*!< VDD 2.9-3.0 */
    kSD_OcrVdd31_32Flag            = 19,                             /*!< VDD 3.0-3.1 */
    kSD_OcrVdd32_33Flag            = 20,                             /*!< VDD 3.1-3.2 */
    kSD_OcrVdd33_34Flag            = 21,                             /*!< VDD 3.2-3.3 */
    kSD_OcrVdd34_35Flag            = 22,                             /*!< VDD 3.3-3.4 */
    kSD_OcrVdd35_36Flag            = 23,                             /*!< VDD 3.4-3.5 */
};

/*! @brief SD card specification version number */
enum _sd_specification_version
{
    kSD_SpecificationVersion1_0 = (1U << 0U), /*!< SD card version 1.0-1.01 */
    kSD_SpecificationVersion1_1 = (1U << 1U), /*!< SD card version 1.10 */
    kSD_SpecificationVersion2_0 = (1U << 2U), /*!< SD card version 2.00 */
    kSD_SpecificationVersion3_0 = (1U << 3U), /*!< SD card version 3.0 */
};

/*! @brief SD card bus width */
typedef enum _sd_data_bus_width
{
    kSD_DataBusWidth1Bit = 0U, /*!< SD data bus width 1-bit mode */
    kSD_DataBusWidth4Bit = 1U, /*!< SD data bus width 4-bit mode */
} sd_data_bus_width_t;

/*! @brief SD card switch mode */
typedef enum _sd_switch_mode
{
    kSD_SwitchCheck = 0U, /*!< SD switch mode 0: check function */
    kSD_SwitchSet   = 1U, /*!< SD switch mode 1: set function */
} sd_switch_mode_t;

/*! @brief SD card CSD register flags */
enum _sd_csd_flag
{
    kSD_CsdReadBlockPartialFlag         = (1U << 0U),  /*!< Partial blocks for read allowed [79:79] */
    kSD_CsdWriteBlockMisalignFlag       = (1U << 1U),  /*!< Write block misalignment [78:78] */
    kSD_CsdReadBlockMisalignFlag        = (1U << 2U),  /*!< Read block misalignment [77:77] */
    kSD_CsdDsrImplementedFlag           = (1U << 3U),  /*!< DSR implemented [76:76] */
    kSD_CsdEraseBlockEnabledFlag        = (1U << 4U),  /*!< Erase single block enabled [46:46] */
    kSD_CsdWriteProtectGroupEnabledFlag = (1U << 5U),  /*!< Write protect group enabled [31:31] */
    kSD_CsdWriteBlockPartialFlag        = (1U << 6U),  /*!< Partial blocks for write allowed [21:21] */
    kSD_CsdFileFormatGroupFlag          = (1U << 7U),  /*!< File format group [15:15] */
    kSD_CsdCopyFlag                     = (1U << 8U),  /*!< Copy flag [14:14] */
    kSD_CsdPermanentWriteProtectFlag    = (1U << 9U),  /*!< Permanent write protection [13:13] */
    kSD_CsdTemporaryWriteProtectFlag    = (1U << 10U), /*!< Temporary write protection [12:12] */
};

/*! @brief SD card SCR register flags */
enum _sd_scr_flag
{
    kSD_ScrDataStatusAfterErase = (1U << 0U), /*!< Data status after erases [55:55] */
    kSD_ScrSdSpecification3     = (1U << 1U), /*!< Specification version 3.00 or higher [47:47]*/
};

/*! @brief SD timing function number */
enum _sd_timing_function
{
    kSD_FunctionSDR12Deafult   = 0U, /*!< SDR12 mode & default*/
    kSD_FunctionSDR25HighSpeed = 1U, /*!< SDR25 & high speed*/
    kSD_FunctionSDR50          = 2U, /*!< SDR50 mode*/
    kSD_FunctionSDR104         = 3U, /*!< SDR104 mode*/
    kSD_FunctionDDR50          = 4U, /*!< DDR50 mode*/
};

/*! @brief SD group number */
enum _sd_group_num
{
    kSD_GroupTimingMode     = 0U, /*!< acess mode group*/
    kSD_GroupCommandSystem  = 1U, /*!< command system group*/
    kSD_GroupDriverStrength = 2U, /*!< driver strength group*/
    kSD_GroupCurrentLimit   = 3U, /*!< current limit group*/
};

/*! @brief SD card timing mode flags */
typedef enum _sd_timing_mode
{
    kSD_TimingSDR12DefaultMode   = 0U, /*!< Identification mode & SDR12 */
    kSD_TimingSDR25HighSpeedMode = 1U, /*!< High speed mode & SDR25 */
    kSD_TimingSDR50Mode          = 2U, /*!< SDR50 mode*/
    kSD_TimingSDR104Mode         = 3U, /*!< SDR104 mode */
    kSD_TimingDDR50Mode          = 4U, /*!< DDR50 mode */
} sd_timing_mode_t;

/*! @brief SD card driver strength */
typedef enum _sd_driver_strength
{
    kSD_DriverStrengthTypeB = 0U, /*!< default driver strength*/
    kSD_DriverStrengthTypeA = 1U, /*!< driver strength TYPE A */
    kSD_DriverStrengthTypeC = 2U, /*!< driver strength TYPE C */
    kSD_DriverStrengthTypeD = 3U, /*!< driver strength TYPE D */
} sd_driver_strength_t;

/*! @brief SD card current limit */
typedef enum _sd_max_current
{
    kSD_CurrentLimit200MA = 0U, /*!< default current limit */
    kSD_CurrentLimit400MA = 1U, /*!< current limit to 400MA */
    kSD_CurrentLimit600MA = 2U, /*!< current limit to 600MA */
    kSD_CurrentLimit800MA = 3U, /*!< current limit to 800MA */
} sd_max_current_t;

/*! @brief SD/MMC card common commands */
typedef enum _sdmmc_command
{
    kSDMMC_GoIdleState        = 0U,  /*!< Go Idle State */
    kSDMMC_AllSendCid         = 2U,  /*!< All Send CID */
    kSDMMC_SetDsr             = 4U,  /*!< Set DSR */
    kSDMMC_SelectCard         = 7U,  /*!< Select Card */
    kSDMMC_SendCsd            = 9U,  /*!< Send CSD */
    kSDMMC_SendCid            = 10U, /*!< Send CID */
    kSDMMC_StopTransmission   = 12U, /*!< Stop Transmission */
    kSDMMC_SendStatus         = 13U, /*!< Send Status */
    kSDMMC_GoInactiveState    = 15U, /*!< Go Inactive State */
    kSDMMC_SetBlockLength     = 16U, /*!< Set Block Length */
    kSDMMC_ReadSingleBlock    = 17U, /*!< Read Single Block */
    kSDMMC_ReadMultipleBlock  = 18U, /*!< Read Multiple Block */
    kSDMMC_SetBlockCount      = 23U, /*!< Set Block Count */
    kSDMMC_WriteSingleBlock   = 24U, /*!< Write Single Block */
    kSDMMC_WriteMultipleBlock = 25U, /*!< Write Multiple Block */
    kSDMMC_ProgramCsd         = 27U, /*!< Program CSD */
    kSDMMC_SetWriteProtect    = 28U, /*!< Set Write Protect */
    kSDMMC_ClearWriteProtect  = 29U, /*!< Clear Write Protect */
    kSDMMC_SendWriteProtect   = 30U, /*!< Send Write Protect */
    kSDMMC_Erase              = 38U, /*!< Erase */
    kSDMMC_LockUnlock         = 42U, /*!< Lock Unlock */
    kSDMMC_ApplicationCommand = 55U, /*!< Send Application Command */
    kSDMMC_GeneralCommand     = 56U, /*!< General Purpose Command */
    kSDMMC_ReadOcr            = 58U, /*!< Read OCR */
} sdmmc_command_t;

/*! @brief sdio card cccr register number */
#define SDIO_CCCR_REG_NUMBER (0x16U)
/*! @brief sdio IO ready timeout steps */
#ifndef SDIO_IO_READY_TIMEOUT_UNIT
#define SDIO_IO_READY_TIMEOUT_UNIT (10U)
#endif
/*! @brief sdio card cccr register addr */
enum _sdio_cccr_reg
{
    kSDIO_RegCCCRSdioVer        = 0x00U, /*!< CCCR & SDIO version*/
    kSDIO_RegSDVersion          = 0x01U, /*!< SD version */
    kSDIO_RegIOEnable           = 0x02U, /*!< io enable register */
    kSDIO_RegIOReady            = 0x03U, /*!< io ready register */
    kSDIO_RegIOIntEnable        = 0x04U, /*!< io interrupt enable register */
    kSDIO_RegIOIntPending       = 0x05U, /*!< io interrupt pending register */
    kSDIO_RegIOAbort            = 0x06U, /*!< io abort register */
    kSDIO_RegBusInterface       = 0x07U, /*!< bus interface register */
    kSDIO_RegCardCapability     = 0x08U, /*!< card capability register */
    kSDIO_RegCommonCISPointer   = 0x09U, /*!< common CIS pointer register */
    kSDIO_RegBusSuspend         = 0x0C,  /*!< bus suspend register */
    kSDIO_RegFunctionSelect     = 0x0DU, /*!< function select register */
    kSDIO_RegExecutionFlag      = 0x0EU, /*!< execution flag register */
    kSDIO_RegReadyFlag          = 0x0FU, /*!< ready flag register */
    kSDIO_RegFN0BlockSizeLow    = 0x10U, /*!< FN0 block size register */
    kSDIO_RegFN0BlockSizeHigh   = 0x11U, /*!< FN0 block size register */
    kSDIO_RegPowerControl       = 0x12U, /*!< power control register */
    kSDIO_RegBusSpeed           = 0x13U, /*!< bus speed register */
    kSDIO_RegUHSITimingSupport  = 0x14U, /*!< UHS-I timing support register */
    kSDIO_RegDriverStrength     = 0x15U, /*!< Driver strength register */
    kSDIO_RegInterruptExtension = 0x16U, /*!< Interrupt extension register */
};

/*! @brief sdio card individual commands */
typedef enum _sdio_command
{
    kSDIO_SendRelativeAddress    = 3U,  /*!< send relative address */
    kSDIO_SendOperationCondition = 5U,  /*!< send operation condition */
    kSDIO_SendInterfaceCondition = 8U,  /*!< send interface condition */
    kSDIO_RWIODirect             = 52U, /*!< read/write IO direct command */
    kSDIO_RWIOExtended           = 53U, /*!< read/write IO extended command */
} sdio_command_t;

/*! @brief sdio card individual commands */
typedef enum _sdio_func_num
{
    kSDIO_FunctionNum0,   /*!< sdio function0*/
    kSDIO_FunctionNum1,   /*!< sdio function1*/
    kSDIO_FunctionNum2,   /*!< sdio function2*/
    kSDIO_FunctionNum3,   /*!< sdio function3*/
    kSDIO_FunctionNum4,   /*!< sdio function4*/
    kSDIO_FunctionNum5,   /*!< sdio function5*/
    kSDIO_FunctionNum6,   /*!< sdio function6*/
    kSDIO_FunctionNum7,   /*!< sdio function7*/
    kSDIO_FunctionMemory, /*!< for combo card*/
} sdio_func_num_t;

#define SDIO_CMD_ARGUMENT_RW_POS (31U)             /*!< read/write flag position */
#define SDIO_CMD_ARGUMENT_FUNC_NUM_POS (28U)       /*!< function number position */
#define SDIO_DIRECT_CMD_ARGUMENT_RAW_POS (27U)     /*!< direct raw flag position */
#define SDIO_CMD_ARGUMENT_REG_ADDR_POS (9U)        /*!< direct reg addr position */
#define SDIO_CMD_ARGUMENT_REG_ADDR_MASK (0x1FFFFU) /*!< direct reg addr mask */
#define SDIO_DIRECT_CMD_DATA_MASK (0xFFU)          /*!< data mask */

#define SDIO_EXTEND_CMD_ARGUMENT_BLOCK_MODE_POS (27U) /*!< extended command argument block mode bit position */
#define SDIO_EXTEND_CMD_ARGUMENT_OP_CODE_POS (26U)    /*!< extended command argument OP Code bit position */
#define SDIO_EXTEND_CMD_BLOCK_MODE_MASK (0x08000000U) /*!< block mode mask */
#define SDIO_EXTEND_CMD_OP_CODE_MASK (0x04000000U)    /*!< op code mask */
#define SDIO_EXTEND_CMD_COUNT_MASK (0x1FFU)           /*!< byte/block count mask */
#define SDIO_MAX_BLOCK_SIZE (2048U)                   /*!< max block size */
#define SDIO_FBR_BASE(x) (x * 0x100U)                 /*!< function basic register */
#define SDIO_TPL_CODE_END (0xFFU)                     /*!< tuple end */
#define SDIO_TPL_CODE_MANIFID (0x20U)                 /*!< manufacturer ID */
#define SDIO_TPL_CODE_FUNCID (0x21U)                  /*!< function ID */
#define SDIO_TPL_CODE_FUNCE (0x22U)                   /*!< function extension tuple*/
/*! @brief sdio command response flag */
enum _sdio_status_flag
{
    kSDIO_StatusCmdCRCError      = 0x8000U, /*!< the CRC check of the previous cmd fail*/
    kSDIO_StatusIllegalCmd       = 0x4000U, /*!< cmd illegal for the card state */
    kSDIO_StatusR6Error          = 0x2000U, /*!< special for R6 error status */
    kSDIO_StatusError            = 0x0800U, /*!< A general or an unknown error occurred */
    kSDIO_StatusFunctionNumError = 0x0200U, /*!< invail function error */
    kSDIO_StatusOutofRange       = 0x0100U, /*!<  cmd argument was out of the allowed range*/
};

/*! @brief sdio operation condition flag */
enum _sdio_ocr_flag
{
    kSDIO_OcrPowerUpBusyFlag = 31, /*!< Power up busy status */
    kSDIO_OcrIONumber        = 28, /*!< number of IO function */
    kSDIO_OcrMemPresent      = 27, /*!< memory present flag */

    kSDIO_OcrVdd20_21Flag = 8,  /*!< VDD 2.0-2.1 */
    kSDIO_OcrVdd21_22Flag = 9,  /*!< VDD 2.1-2.2 */
    kSDIO_OcrVdd22_23Flag = 10, /*!< VDD 2.2-2.3 */
    kSDIO_OcrVdd23_24Flag = 11, /*!< VDD 2.3-2.4 */
    kSDIO_OcrVdd24_25Flag = 12, /*!< VDD 2.4-2.5 */
    kSDIO_OcrVdd25_26Flag = 13, /*!< VDD 2.5-2.6 */
    kSDIO_OcrVdd26_27Flag = 14, /*!< VDD 2.6-2.7 */
    kSDIO_OcrVdd27_28Flag = 15, /*!< VDD 2.7-2.8 */
    kSDIO_OcrVdd28_29Flag = 16, /*!< VDD 2.8-2.9 */
    kSDIO_OcrVdd29_30Flag = 17, /*!< VDD 2.9-3.0 */
    kSDIO_OcrVdd30_31Flag = 18, /*!< VDD 2.9-3.0 */
    kSDIO_OcrVdd31_32Flag = 19, /*!< VDD 3.0-3.1 */
    kSDIO_OcrVdd32_33Flag = 20, /*!< VDD 3.1-3.2 */
    kSDIO_OcrVdd33_34Flag = 21, /*!< VDD 3.2-3.3 */
    kSDIO_OcrVdd34_35Flag = 22, /*!< VDD 3.3-3.4 */
    kSDIO_OcrVdd35_36Flag = 23, /*!< VDD 3.4-3.5 */
};
/*! @brief sdio ocr voltage window mask */
#define SDIO_OCR_VOLTAGE_WINDOW_MASK (0xFFFFU << 8U)

/*! @brief sdio ocr reigster IO NUMBER mask  */
#define SDIO_OCR_IO_NUM_MASK (7U << kSDIO_OcrIONumber)

/*! @brief sdio capability flag */
enum _sdio_capability_flag
{
    kSDIO_CCCRSupportDirectCmdDuringDataTrans = (1U << 0U),  /*!< support direct cmd during data transfer */
    kSDIO_CCCRSupportMultiBlock               = (1U << 1U),  /*!< support multi block mode */
    kSDIO_CCCRSupportReadWait                 = (1U << 2U),  /*!< support read wait */
    kSDIO_CCCRSupportSuspendResume            = (1U << 3U),  /*!< support suspend resume */
    kSDIO_CCCRSupportIntDuring4BitDataTrans   = (1U << 4U),  /*!< support interrupt during 4-bit data transfer */
    kSDIO_CCCRSupportLowSpeed1Bit             = (1U << 6U),  /*!< support low speed 1bit mode */
    kSDIO_CCCRSupportLowSpeed4Bit             = (1U << 7U),  /*!< support low speed 4bit mode */
    kSDIO_CCCRSupportMasterPowerControl       = (1U << 8U),  /*!< support master power control */
    kSDIO_CCCRSupportHighSpeed                = (1U << 9U),  /*!< support high speed */
    kSDIO_CCCRSupportContinuousSPIInt         = (1U << 10U), /*!< support continuous SPI interrupt */
};
/*! @brief UHS timing mode flag */
#define SDIO_CCCR_SUPPORT_HIGHSPEED (1u << 9U)
#define SDIO_CCCR_SUPPORT_SDR50 (1U << 11U)
#define SDIO_CCCR_SUPPORT_SDR104 (1U << 12U)
#define SDIO_CCCR_SUPPORT_DDR50 (1U << 13U)
#define SDIO_CCCR_SUPPORT_DRIVER_TYPE_A (1U << 14U)
#define SDIO_CCCR_SUPPORT_DRIVER_TYPE_C (1U << 15U)
#define SDIO_CCCR_SUPPORT_DRIVER_TYPE_D (1U << 16U)
#define SDIO_CCCR_SUPPORT_ASYNC_INT (1U << 17U)

#define SDIO_CCCR_BUS_SPEED_MASK (7U << 1U)
#define SDIO_CCCR_ENABLE_HIGHSPEED_MODE (1U << 1U)
#define SDIO_CCCR_ENABLE_SDR50_MODE (2U << 1U)
#define SDIO_CCCR_ENABLE_SDR104_MODE (3U << 1U)
#define SDIO_CCCR_ENABLE_DDR50_MODE (4U << 1U)

/*! @brief Driver type flag */
#define SDIO_CCCR_DRIVER_TYPE_MASK (3U << 4U)
#define SDIO_CCCR_ENABLE_DRIVER_TYPE_B (0U << 4U)
#define SDIO_CCCR_ENABLE_DRIVER_TYPE_A (1U << 4U)
#define SDIO_CCCR_ENABLE_DRIVER_TYPE_C (2U << 4U)
#define SDIO_CCCR_ENABLE_DRIVER_TYPE_D (3U << 4U)

/*! @brief aync interrupt flag*/
#define SDIO_CCCR_ASYNC_INT_MASK (1U)
#define SDIO_CCCR_ENABLE_AYNC_INT (1U << 1U)

/*! @brief 8 bit data bus flag*/
#define SDIO_CCCR_SUPPORT_8BIT_BUS (1U << 18U)
#define SDIO_CCCR_SUPPORT_LOW_SPEED_4BIT_BUS (1U << 7U)
/*! @brief sdio fbr flag */
enum _sdio_fbr_flag
{
    kSDIO_FBRSupportCSA            = (1U << 0U), /*!< function support CSA */
    kSDIO_FBRSupportPowerSelection = (1U << 1U), /*!< function support power selection */
};

/*! @brief sdio bus width */
typedef enum _sdio_bus_width
{
    kSDIO_DataBus1Bit = 0x00U, /*!< 1 bit bus mode */
    kSDIO_DataBus4Bit = 0X02U, /*!< 4 bit bus mode*/
    kSDIO_DataBus8Bit = 0X03U, /*!< 8 bit bus mode*/
} sdio_bus_width_t;

/*! @brief MMC card individual commands */
typedef enum _mmc_command
{
    kMMC_SendOperationCondition = 1U,  /*!< Send Operation Condition */
    kMMC_SetRelativeAddress     = 3U,  /*!< Set Relative Address */
    kMMC_SleepAwake             = 5U,  /*!< Sleep Awake */
    kMMC_Switch                 = 6U,  /*!< Switch */
    kMMC_SendExtendedCsd        = 8U,  /*!< Send EXT_CSD */
    kMMC_ReadDataUntilStop      = 11U, /*!< Read Data Until Stop */
    kMMC_BusTestRead            = 14U, /*!< Test Read */
    kMMC_SendingBusTest         = 19U, /*!< test bus width cmd*/
    kMMC_WriteDataUntilStop     = 20U, /*!< Write Data Until Stop */
    kMMC_SendTuningBlock        = 21U, /*!< MMC sending tuning block */
    kMMC_ProgramCid             = 26U, /*!< Program CID */
    kMMC_EraseGroupStart        = 35U, /*!< Erase Group Start */
    kMMC_EraseGroupEnd          = 36U, /*!< Erase Group End */
    kMMC_FastInputOutput        = 39U, /*!< Fast IO */
    kMMC_GoInterruptState       = 40U, /*!< Go interrupt State */
} mmc_command_t;

/*! @brief MMC card classified as voltage range */
typedef enum _mmc_classified_voltage
{
    kMMC_ClassifiedVoltageHigh = 0U, /*!< High-voltage MMC card */
    kMMC_ClassifiedVoltageDual = 1U, /*!< Dual-voltage MMC card */
} mmc_classified_voltage_t;

/*! @brief MMC card classified as density level */
typedef enum _mmc_classified_density
{
    kMMC_ClassifiedDensityWithin2GB = 0U, /*!< Density byte is less than or equal 2GB */
    kMMC_ClassifiedDensityHigher2GB = 1U, /* Density byte is higher than 2GB */
} mmc_classified_density_t;

/*! @brief The bit mask for VOLTAGE WINDOW 1.70V to 1.95V field in OCR */
#define MMC_OCR_V170TO195_SHIFT (7U)
/*! @brief The bit mask for VOLTAGE WINDOW 1.70V to 1.95V field in OCR */
#define MMC_OCR_V170TO195_MASK (0x00000080U)
/*! @brief The bit shift for VOLTAGE WINDOW 2.00V to 2.60V field in OCR */
#define MMC_OCR_V200TO260_SHIFT (8U)
/*! @brief The bit mask for VOLTAGE WINDOW 2.00V to 2.60V field in OCR */
#define MMC_OCR_V200TO260_MASK (0x00007F00U)
/*! @brief The bit shift for VOLTAGE WINDOW 2.70V to 3.60V field in OCR */
#define MMC_OCR_V270TO360_SHIFT (15U)
/*! @brief The bit mask for VOLTAGE WINDOW 2.70V to 3.60V field in OCR */
#define MMC_OCR_V270TO360_MASK (0x00FF8000U)
/*! @brief The bit shift for ACCESS MODE field in OCR */
#define MMC_OCR_ACCESS_MODE_SHIFT (29U)
/*! @brief The bit mask for ACCESS MODE field in OCR */
#define MMC_OCR_ACCESS_MODE_MASK (0x60000000U)
/*! @brief The bit shift for BUSY field in OCR */
#define MMC_OCR_BUSY_SHIFT (31U)
/*! @brief The bit mask for BUSY field in OCR */
#define MMC_OCR_BUSY_MASK (1U << MMC_OCR_BUSY_SHIFT)

/*! @brief MMC card access mode(Access mode in OCR). */
typedef enum _mmc_access_mode
{
    kMMC_AccessModeByte   = 0U, /*!< The card should be accessed as byte */
    kMMC_AccessModeSector = 2U, /*!< The card should be accessed as sector */
} mmc_access_mode_t;

/*! @brief MMC card voltage window(VDD voltage window in OCR). */
typedef enum _mmc_voltage_window
{
    kMMC_VoltageWindowNone      = 0U,     /*!< voltage window is not define by user*/
    kMMC_VoltageWindow120       = 0x01U,  /*!< Voltage window is 1.20V */
    kMMC_VoltageWindow170to195  = 0x02U,  /*!< Voltage window is 1.70V to 1.95V */
    kMMC_VoltageWindows270to360 = 0x1FFU, /*!< Voltage window is 2.70V to 3.60V */
} mmc_voltage_window_t;

/*! @brief CSD structure version(CSD_STRUCTURE in CSD). */
typedef enum _mmc_csd_structure_version
{
    kMMC_CsdStrucureVersion10       = 0U, /*!< CSD version No. 1.0 */
    kMMC_CsdStrucureVersion11       = 1U, /*!< CSD version No. 1.1 */
    kMMC_CsdStrucureVersion12       = 2U, /*!< CSD version No. 1.2 */
    kMMC_CsdStrucureVersionInExtcsd = 3U, /*!< Version coded in Extended CSD */
} mmc_csd_structure_version_t;

/*! @brief MMC card specification version(SPEC_VERS in CSD). */
typedef enum _mmc_specification_version
{
    kMMC_SpecificationVersion0 = 0U, /*!< Allocated by MMCA */
    kMMC_SpecificationVersion1 = 1U, /*!< Allocated by MMCA */
    kMMC_SpecificationVersion2 = 2U, /*!< Allocated by MMCA */
    kMMC_SpecificationVersion3 = 3U, /*!< Allocated by MMCA */
    kMMC_SpecificationVersion4 = 4U, /*!< Version 4.1/4.2/4.3/4.41-4.5-4.51-5.0 */
} mmc_specification_version_t;

/*! @brief The bit shift for FREQUENCY UNIT field in TRANSFER SPEED(TRAN-SPEED in Extended CSD) */
#define MMC_TRANSFER_SPEED_FREQUENCY_UNIT_SHIFT (0U)
/*! @brief The bit mask for FRQEUENCY UNIT in TRANSFER SPEED */
#define MMC_TRANSFER_SPEED_FREQUENCY_UNIT_MASK (0x07U)
/*! @brief The bit shift for MULTIPLIER field in TRANSFER SPEED */
#define MMC_TRANSFER_SPEED_MULTIPLIER_SHIFT (3U)
/*! @brief The bit mask for MULTIPLIER field in TRANSFER SPEED  */
#define MMC_TRANSFER_SPEED_MULTIPLIER_MASK (0x78U)

/*! @brief Read the value of FREQUENCY UNIT in TRANSFER SPEED. */
#define READ_MMC_TRANSFER_SPEED_FREQUENCY_UNIT(CSD) \
    (((CSD.transferSpeed) & MMC_TRANSFER_SPEED_FREQUENCY_UNIT_MASK) >> MMC_TRANSFER_SPEED_FREQUENCY_UNIT_SHIFT)
/*! @brief Read the value of MULTIPLER filed in TRANSFER SPEED. */
#define READ_MMC_TRANSFER_SPEED_MULTIPLIER(CSD) \
    (((CSD.transferSpeed) & MMC_TRANSFER_SPEED_MULTIPLIER_MASK) >> MMC_TRANSFER_SPEED_MULTIPLIER_SHIFT)

/*! @brief MMC card Extended CSD fix version(EXT_CSD_REV in Extended CSD) */
enum _mmc_extended_csd_revision
{
    kMMC_ExtendedCsdRevision10 = 0U, /*!< Revision 1.0 */
    kMMC_ExtendedCsdRevision11 = 1U, /*!< Revision 1.1 */
    kMMC_ExtendedCsdRevision12 = 2U, /*!< Revision 1.2 */
    kMMC_ExtendedCsdRevision13 = 3U, /*!< Revision 1.3 MMC4.3*/
    kMMC_ExtendedCsdRevision14 = 4U, /*!< Revision 1.4 obsolete*/
    kMMC_ExtendedCsdRevision15 = 5U, /*!< Revision 1.5 MMC4.41*/
    kMMC_ExtendedCsdRevision16 = 6U, /*!< Revision 1.6 MMC4.5*/
    kMMC_ExtendedCsdRevision17 = 7U, /*!< Revision 1.7 MMC5.0 */
};

/*! @brief MMC card command set(COMMAND_SET in Extended CSD) */
typedef enum _mmc_command_set
{
    kMMC_CommandSetStandard = 0U, /*!< Standard MMC */
    kMMC_CommandSet1        = 1U, /*!< Command set 1 */
    kMMC_CommandSet2        = 2U, /*!< Command set 2 */
    kMMC_CommandSet3        = 3U, /*!< Command set 3 */
    kMMC_CommandSet4        = 4U, /*!< Command set 4 */
} mmc_command_set_t;

/*! @brief boot support(BOOT_INFO in Extended CSD) */
enum _mmc_support_boot_mode
{
    kMMC_SupportAlternateBoot = 1U, /*!< support alternative boot mode*/
    kMMC_SupportDDRBoot       = 2U, /*!< support DDR boot mode*/
    kMMC_SupportHighSpeedBoot = 4U, /*!< support high speed boot mode*/
};
/*! @brief The power class value bit mask when bus in 4 bit mode */
#define MMC_POWER_CLASS_4BIT_MASK (0x0FU)
/*! @brief The power class current value bit mask when bus in 8 bit mode */
#define MMC_POWER_CLASS_8BIT_MASK (0xF0U)

/*! @brief MMC card high-speed timing(HS_TIMING in Extended CSD) */
typedef enum _mmc_high_speed_timing
{
    kMMC_HighSpeedTimingNone = 0U, /*!< MMC card using none high-speed timing */
    kMMC_HighSpeedTiming     = 1U, /*!< MMC card using high-speed timing */
    kMMC_HighSpeed200Timing  = 2U, /*!< MMC card high speed 200 timing*/
    kMMC_HighSpeed400Timing  = 3U, /*!< MMC card high speed 400 timing*/
} mmc_high_speed_timing_t;

/*! @brief The number of data bus width type */
#define MMC_DATA_BUS_WIDTH_TYPE_NUMBER (3U)
/*! @brief MMC card data bus width(BUS_WIDTH in Extended CSD) */
typedef enum _mmc_data_bus_width
{
    kMMC_DataBusWidth1bit    = 0U, /*!< MMC data bus width is 1 bit */
    kMMC_DataBusWidth4bit    = 1U, /*!< MMC data bus width is 4 bits */
    kMMC_DataBusWidth8bit    = 2U, /*!< MMC data bus width is 8 bits */
    kMMC_DataBusWidth4bitDDR = 5U, /*!< MMC data bus width is 4 bits ddr */
    kMMC_DataBusWidth8bitDDR = 6U, /*!< MMC data bus width is 8 bits ddr */
} mmc_data_bus_width_t;

/*! @brief MMC card boot partition enabled(BOOT_PARTITION_ENABLE in Extended CSD) */
typedef enum _mmc_boot_partition_enable
{
    kMMC_BootPartitionEnableNot        = 0U, /*!< Device not boot enabled (default) */
    kMMC_BootPartitionEnablePartition1 = 1U, /*!< Boot partition 1 enabled for boot */
    kMMC_BootPartitionEnablePartition2 = 2U, /*!< Boot partition 2 enabled for boot */
    kMMC_BootPartitionEnableUserAera   = 7U, /*!< User area enabled for boot */
} mmc_boot_partition_enable_t;

/*! @brief boot mode configuration
 * Note: HS200 & HS400 is not support during BOOT operation.
 */
typedef enum _mmc_boot_timing_mode
{
    kMMC_BootModeSDRWithDefaultTiming   = 0U << 3U, /*!< boot mode single data rate with backward compatiable timings */
    kMMC_BootModeSDRWithHighSpeedTiming = 1U << 3U, /*!< boot mode single data rate with high speed timing */
    kMMC_BootModeDDRTiming              = 2U << 3U, /*!< boot mode dual date rate */
} mmc_boot_timing_mode_t;

/*! @brief MMC card boot partition write protect configurations
 * All the bits in BOOT_WP register, except the two R/W bits B_PERM_WP_DIS
 * and B_PERM_WP_EN, shall only be written once per power cycle.The protection
 * mdde intended for both boot areas will be set with a single write.
 */
typedef enum _mmc_boot_partition_wp
{
    kMMC_BootPartitionWPDisable = 0x50U, /*!< boot partition write protection disable */
    kMMC_BootPartitionPwrWPToBothPartition =
        0x01U, /*!< power on period write protection apply to both boot partitions */
    kMMC_BootPartitionPermWPToBothPartition = 0x04U, /*!< permanent write protection apply to both boot partitions */

    kMMC_BootPartitionPwrWPToPartition1 = (1U << 7U) | 1U, /*!< power on period write protection apply to partition1 */
    kMMC_BootPartitionPwrWPToPartition2 = (1U << 7U) | 3U, /*!< power on period write protection apply to partition2 */

    kMMC_BootPartitionPermWPToPartition1 =
        (1U << 7U) | (1U << 2U), /*!< permanent write protection apply to partition1 */
    kMMC_BootPartitionPermWPToPartition2 =
        (1U << 7U) | (3U << 2U), /*!< permanent write protection apply to partition2 */

    kMMC_BootPartitionPermWPToPartition1PwrWPToPartition2 =
        (1U << 7U) | (1U << 2U) |
        3U, /*!< permanent write protection apply to partition1, power on period write protection apply to partition2 */
    kMMC_BootPartitionPermWPToPartition2PwrWPToPartition1 =
        (1U << 7U) | (3U << 2U) |
        1U, /*!< permanent write protection apply to partition2, power on period write protection apply to partition1 */
} mmc_boot_partition_wp_t;

/*! @brief MMC card boot partition write protect status */
enum _mmc_boot_partition_wp_status
{
    kMMC_BootPartitionNotProtected  = 0U, /*!< boot partition not protected */
    kMMC_BootPartitionPwrProtected  = 1U, /*!< boot partition is power on period write protected */
    kMMC_BootPartitionPermProtected = 2U, /*!< boot partition is permanently protected */
};

/*! @brief MMC card partition to be accessed(BOOT_PARTITION_ACCESS in Extended CSD) */
typedef enum _mmc_access_partition
{
    kMMC_AccessPartitionUserAera        = 0U, /*!< No access to boot partition (default), normal partition */
    kMMC_AccessPartitionBoot1           = 1U, /*!< Read/Write boot partition 1 */
    kMMC_AccessPartitionBoot2           = 2U, /*!< Read/Write boot partition 2*/
    kMMC_AccessRPMB                     = 3U, /*!< Replay protected mem block */
    kMMC_AccessGeneralPurposePartition1 = 4U, /*!< access to general purpose partition 1 */
    kMMC_AccessGeneralPurposePartition2 = 5U, /*!< access to general purpose partition 2 */
    kMMC_AccessGeneralPurposePartition3 = 6U, /*!< access to general purpose partition 3 */
    kMMC_AccessGeneralPurposePartition4 = 7U, /*!< access to general purpose partition 4 */
} mmc_access_partition_t;

/*! @brief The bit shift for PARTITION ACCESS filed in BOOT CONFIG (BOOT_CONFIG in Extend CSD) */
#define MMC_PARTITION_CONFIG_PARTITION_ACCESS_SHIFT (0U)
/*! @brief The bit mask for PARTITION ACCESS field in BOOT CONFIG */
#define MMC_PARTITION_CONFIG_PARTITION_ACCESS_MASK (0x00000007U)
/*! @brief The bit shift for PARTITION ENABLE field in BOOT CONFIG */
#define MMC_PARTITION_CONFIG_PARTITION_ENABLE_SHIFT (3U)
/*! @brief The bit mask for PARTITION ENABLE field in BOOT CONFIG */
#define MMC_PARTITION_CONFIG_PARTITION_ENABLE_MASK (0x00000038U)
/*! @brief The bit shift for ACK field in BOOT CONFIG */
#define MMC_PARTITION_CONFIG_BOOT_ACK_SHIFT (6U)
/*! @brief The bit mask for ACK field in BOOT CONFIG */
#define MMC_PARTITION_CONFIG_BOOT_ACK_MASK (0x00000040U)
/*! @brief The bit shift for BOOT BUS WIDTH field in BOOT CONFIG */
#define MMC_BOOT_BUS_CONDITION_BUS_WIDTH_SHIFT (0U)
/*! @brief The bit mask for BOOT BUS WIDTH field in BOOT CONFIG */
#define MMC_BOOT_BUS_CONDITION_BUS_WIDTH_MASK (3U)
/*! @brief The bit shift for BOOT BUS WIDTH RESET field in BOOT CONFIG */
#define MMC_BOOT_BUS_CONDITION_RESET_BUS_CONDITION_SHIFT (2U)
/*! @brief The bit mask for BOOT BUS WIDTH RESET field in BOOT CONFIG */
#define MMC_BOOT_BUS_CONDITION_RESET_BUS_CONDITION_MASK (4U)
/*! @brief The bit mask for BOOT BUS WIDTH RESET field in BOOT CONFIG */
#define MMC_BOOT_BUS_CONDITION_BOOT_MODE_MASK (0x18U)

/*! @brief MMC card CSD register flags */
enum _mmc_csd_flag
{
    kMMC_CsdReadBlockPartialFlag         = (1U << 0U),  /*!< Partial blocks for read allowed */
    kMMC_CsdWriteBlockMisalignFlag       = (1U << 1U),  /*!< Write block misalignment */
    kMMC_CsdReadBlockMisalignFlag        = (1U << 2U),  /*!< Read block misalignment */
    kMMC_CsdDsrImplementedFlag           = (1U << 3U),  /*!< DSR implemented */
    kMMC_CsdWriteProtectGroupEnabledFlag = (1U << 4U),  /*!< Write protect group enabled */
    kMMC_CsdWriteBlockPartialFlag        = (1U << 5U),  /*!< Partial blocks for write allowed */
    kMMC_ContentProtectApplicationFlag   = (1U << 6U),  /*!< Content protect application */
    kMMC_CsdFileFormatGroupFlag          = (1U << 7U),  /*!< File format group */
    kMMC_CsdCopyFlag                     = (1U << 8U),  /*!< Copy flag */
    kMMC_CsdPermanentWriteProtectFlag    = (1U << 9U),  /*!< Permanent write protection */
    kMMC_CsdTemporaryWriteProtectFlag    = (1U << 10U), /*!< Temporary write protection */
};

/*! @brief Extended CSD register access mode(Access mode in CMD6). */
typedef enum _mmc_extended_csd_access_mode
{
    kMMC_ExtendedCsdAccessModeCommandSet = 0U, /*!< Command set related setting */
    kMMC_ExtendedCsdAccessModeSetBits    = 1U, /*!< Set bits in specific byte in Extended CSD  */
    kMMC_ExtendedCsdAccessModeClearBits  = 2U, /*!< Clear bits in specific byte in Extended CSD */
    kMMC_ExtendedCsdAccessModeWriteBits  = 3U, /*!< Write a value to specific byte in Extended CSD */
} mmc_extended_csd_access_mode_t;

/*! @brief EXT CSD byte index */
typedef enum _mmc_extended_csd_index
{
    kMMC_ExtendedCsdIndexBootPartitionWP      = 173U, /*!< Boot partition write protect */
    kMMC_ExtendedCsdIndexEraseGroupDefinition = 175U, /*!< Erase Group Def */
    kMMC_ExtendedCsdIndexBootBusConditions    = 177U, /*!< Boot Bus conditions */
    kMMC_ExtendedCsdIndexBootConfigWP         = 178U, /*!< Boot config write protect */
    kMMC_ExtendedCsdIndexPartitionConfig      = 179U, /*!< Partition Config, before BOOT_CONFIG */
    kMMC_ExtendedCsdIndexBusWidth             = 183U, /*!< Bus Width */
    kMMC_ExtendedCsdIndexHighSpeedTiming      = 185U, /*!< High-speed Timing */
    kMMC_ExtendedCsdIndexPowerClass           = 187U, /*!< Power Class */
    kMMC_ExtendedCsdIndexCommandSet           = 191U, /*!< Command Set */
} mmc_extended_csd_index_t;

/*! @brief mmc driver strength */
enum _mmc_driver_strength
{
    kMMC_DriverStrength0 = 0U, /*!< Driver type0 ,nominal impedance 50ohm */
    kMMC_DriverStrength1 = 1U, /*!< Driver type1 ,nominal impedance 33ohm */
    kMMC_DriverStrength2 = 2U, /*!< Driver type2 ,nominal impedance 66ohm */
    kMMC_DriverStrength3 = 3U, /*!< Driver type3 ,nominal impedance 100ohm */
    kMMC_DriverStrength4 = 4U, /*!< Driver type4 ,nominal impedance 40ohm */
};

/*! @brief mmc extended csd flags*/
typedef enum _mmc_extended_csd_flags
{
    kMMC_ExtCsdExtPartitionSupport       = (1 << 0U), /*!< partitioning support[160] */
    kMMC_ExtCsdEnhancePartitionSupport   = (1 << 1U), /*!< partitioning support[160] */
    kMMC_ExtCsdPartitioningSupport       = (1 << 2U), /*!< partitioning support[160] */
    kMMC_ExtCsdPrgCIDCSDInDDRModeSupport = (1 << 3U), /*!< CMD26 and CMD27 are support dual data rate [130]*/
    kMMC_ExtCsdBKOpsSupport              = (1 << 4U), /*!< background operation feature support [502]*/
    kMMC_ExtCsdDataTagSupport            = (1 << 5U), /*!< data tag support[499]*/
    kMMC_ExtCsdModeOperationCodeSupport  = (1 << 6U), /*!< mode operation code support[493]*/
} mmc_extended_csd_flags_t;

/*! @brief MMC card boot mode */
enum _mmc_boot_mode
{
    kMMC_BootModeNormal      = 0U, /*!< Normal boot */
    kMMC_BootModeAlternative = 1U, /*!< Alternative boot */
};

/*! @brief The length of Extended CSD register, unit as bytes. */
#define MMC_EXTENDED_CSD_BYTES (512U)

/*! @brief MMC card default relative address */
#define MMC_DEFAULT_RELATIVE_ADDRESS (2U)

/*! @brief SD card product name length united as bytes. */
#define SD_PRODUCT_NAME_BYTES (5U)

/*! @brief sdio card FBR register */
typedef struct _sdio_fbr
{
    uint8_t flags;             /*!< current io flags */
    uint8_t ioStdFunctionCode; /*!< current io standard function code */
    uint8_t ioExtFunctionCode; /*!< current io extended function code*/
    uint32_t ioPointerToCIS;   /*!< current io pointer to CIS */
    uint32_t ioPointerToCSA;   /*!< current io pointer to CSA*/
    uint16_t ioBlockSize;      /*!< current io block size */
} sdio_fbr_t;

/*! @brief sdio card common CIS  */
typedef struct _sdio_common_cis
{
    /* manufacturer identification string tuple */
    uint16_t mID;   /*!< manufacturer code */
    uint16_t mInfo; /*!< manufacturer information */

    /*function identification tuple */
    uint8_t funcID; /*!< function ID */

    /* function extension tuple */
    uint16_t fn0MaxBlkSize; /*!< function 0 max block size */
    uint8_t maxTransSpeed;  /*!< max data transfer speed for all function */

} sdio_common_cis_t;

/*! @brief sdio card function CIS  */
typedef struct _sdio_func_cis
{
    /*function identification tuple */
    uint8_t funcID; /*!< function ID */

    /* function extension tuple */
    uint8_t funcInfo;        /*!< function info */
    uint8_t ioVersion;       /*!< level of application specification this io support */
    uint32_t cardPSN;        /*!< product serial number */
    uint32_t ioCSASize;      /*!< avaliable CSA size for io  */
    uint8_t ioCSAProperty;   /*!< CSA property */
    uint16_t ioMaxBlockSize; /*!< io max transfer data size */
    uint32_t ioOCR;          /*!< io ioeration condition */
    uint8_t ioOPMinPwr;      /*!< min current in operation mode */
    uint8_t ioOPAvgPwr;      /*!< average current in operation mode */
    uint8_t ioOPMaxPwr;      /*!< max current in operation mode */
    uint8_t ioSBMinPwr;      /*!< min current in standby mode */
    uint8_t ioSBAvgPwr;      /*!< average current in standby mode */
    uint8_t ioSBMaxPwr;      /*!< max current in standby mode */

    uint16_t ioMinBandWidth;          /*!< io min transfer bandwidth */
    uint16_t ioOptimumBandWidth;      /*!< io optimum transfer bandwidth */
    uint16_t ioReadyTimeout;          /*!< timeout value from enalbe to ready */
    uint16_t ioHighCurrentAvgCurrent; /*!< the average peak current (mA)
                                      when IO operating in high current mode */
    uint16_t ioHighCurrentMaxCurrent; /*!< the max peak current (mA)
                                      when IO operating in high current mode */
    uint16_t ioLowCurrentAvgCurrent;  /*!< the average peak current (mA)
                                      when IO operating in lower current mode */
    uint16_t ioLowCurrentMaxCurrent;  /*!< the max peak current (mA)
                                      when IO operating in lower current mode */
} sdio_func_cis_t;

/*! @brief SD AU start value */
#define SD_AU_START_VALUE (1U)
/*! @brief SD UHS AU start value */
#define SD_UHS_AU_START_VALUE (7U)

/*! @brief SD card status */
typedef struct _sd_status
{
    uint8_t busWidth;        /*!< current buswidth */
    uint8_t secureMode;      /*!< secured mode */
    uint16_t cardType;       /*!< sdcard type */
    uint32_t protectedSize;  /*!< size of protected area */
    uint8_t speedClass;      /*!< speed class of card */
    uint8_t performanceMove; /*!< Performance of move indicated by 1[MB/S]step */
    uint8_t auSize;          /*!< size of AU */
    uint16_t eraseSize;      /*!< number of AUs to be erased at a time */
    uint8_t eraseTimeout;    /*!< timeout value for erasing areas specified by UNIT OF ERASE AU */
    uint8_t eraseOffset;     /*!< fixed offset value added to erase time */
    uint8_t uhsSpeedGrade;   /*!< speed grade for UHS mode */
    uint8_t uhsAuSize;       /*!< size of AU for UHS mode */
} sd_status_t;

/*! @brief SD card CID register */
typedef struct _sd_cid
{
    uint8_t manufacturerID;                     /*!< Manufacturer ID [127:120] */
    uint16_t applicationID;                     /*!< OEM/Application ID [119:104] */
    uint8_t productName[SD_PRODUCT_NAME_BYTES]; /*!< Product name [103:64] */
    uint8_t productVersion;                     /*!< Product revision [63:56] */
    uint32_t productSerialNumber;               /*!< Product serial number [55:24] */
    uint16_t manufacturerData;                  /*!< Manufacturing date [19:8] */
} sd_cid_t;

/*! @brief SD card CSD register */
typedef struct _sd_csd
{
    uint8_t csdStructure;        /*!< CSD structure [127:126] */
    uint8_t dataReadAccessTime1; /*!< Data read access-time-1 [119:112] */
    uint8_t dataReadAccessTime2; /*!< Data read access-time-2 in clock cycles (NSAC*100) [111:104] */
    uint8_t transferSpeed;       /*!< Maximum data transfer rate [103:96] */
    uint16_t cardCommandClass;   /*!< Card command classes [95:84] */
    uint8_t readBlockLength;     /*!< Maximum read data block length [83:80] */
    uint16_t flags;              /*!< Flags in _sd_csd_flag */
    uint32_t deviceSize;         /*!< Device size [73:62] */
    /* Following fields from 'readCurrentVddMin' to 'deviceSizeMultiplier' exist in CSD version 1 */
    uint8_t readCurrentVddMin;    /*!< Maximum read current at VDD min [61:59] */
    uint8_t readCurrentVddMax;    /*!< Maximum read current at VDD max [58:56] */
    uint8_t writeCurrentVddMin;   /*!< Maximum write current at VDD min [55:53] */
    uint8_t writeCurrentVddMax;   /*!< Maximum write current at VDD max [52:50] */
    uint8_t deviceSizeMultiplier; /*!< Device size multiplier [49:47] */

    uint8_t eraseSectorSize;       /*!< Erase sector size [45:39] */
    uint8_t writeProtectGroupSize; /*!< Write protect group size [38:32] */
    uint8_t writeSpeedFactor;      /*!< Write speed factor [28:26] */
    uint8_t writeBlockLength;      /*!< Maximum write data block length [25:22] */
    uint8_t fileFormat;            /*!< File format [11:10] */
} sd_csd_t;

/*! @brief The bit shift for RATE UNIT field in TRANSFER SPEED */
#define SD_TRANSFER_SPEED_RATE_UNIT_SHIFT (0U)
/*! @brief The bit mask for RATE UNIT field in TRANSFER SPEED */
#define SD_TRANSFER_SPEED_RATE_UNIT_MASK (0x07U)
/*! @brief The bit shift for TIME VALUE field in TRANSFER SPEED */
#define SD_TRANSFER_SPEED_TIME_VALUE_SHIFT (2U)
/*! @brief The bit mask for TIME VALUE field in TRANSFER SPEED */
#define SD_TRANSFER_SPEED_TIME_VALUE_MASK (0x78U)
/*! @brief Read the value of FREQUENCY UNIT in TRANSFER SPEED field */
#define SD_RD_TRANSFER_SPEED_RATE_UNIT(x) \
    (((x.transferSpeed) & SD_TRANSFER_SPEED_RATE_UNIT_MASK) >> SD_TRANSFER_SPEED_RATE_UNIT_SHIFT)
/*! @brief Read the value of TIME VALUE in TRANSFER SPEED field */
#define SD_RD_TRANSFER_SPEED_TIME_VALUE(x) \
    (((x.transferSpeed) & SD_TRANSFER_SPEED_TIME_VALUE_MASK) >> SD_TRANSFER_SPEED_TIME_VALUE_SHIFT)

/*! @brief SD card SCR register */
typedef struct _sd_scr
{
    uint8_t scrStructure;             /*!< SCR Structure [63:60] */
    uint8_t sdSpecification;          /*!< SD memory card specification version [59:56] */
    uint16_t flags;                   /*!< SCR flags in _sd_scr_flag */
    uint8_t sdSecurity;               /*!< Security specification supported [54:52] */
    uint8_t sdBusWidths;              /*!< Data bus widths supported [51:48] */
    uint8_t extendedSecurity;         /*!< Extended security support [46:43] */
    uint8_t commandSupport;           /*!< Command support bits [33:32] 33-support CMD23, 32-support cmd20*/
    uint32_t reservedForManufacturer; /*!< reserved for manufacturer usage [31:0] */
} sd_scr_t;

/*! @brief MMC card product name length united as bytes. */
#define MMC_PRODUCT_NAME_BYTES (6U)
/*! @brief MMC card CID register. */
typedef struct _mmc_cid
{
    uint8_t manufacturerID;                      /*!< Manufacturer ID */
    uint16_t applicationID;                      /*!< OEM/Application ID */
    uint8_t productName[MMC_PRODUCT_NAME_BYTES]; /*!< Product name */
    uint8_t productVersion;                      /*!< Product revision */
    uint32_t productSerialNumber;                /*!< Product serial number */
    uint8_t manufacturerData;                    /*!< Manufacturing date */
} mmc_cid_t;

/*! @brief MMC card CSD register. */
typedef struct _mmc_csd
{
    uint8_t csdStructureVersion;        /*!< CSD structure [127:126] */
    uint8_t systemSpecificationVersion; /*!< System specification version [125:122] */
    uint8_t dataReadAccessTime1;        /*!< Data read access-time 1 [119:112] */
    uint8_t dataReadAccessTime2;        /*!< Data read access-time 2 in CLOCK cycles (NSAC*100) [111:104] */
    uint8_t transferSpeed;              /*!< Max. bus clock frequency [103:96] */
    uint16_t cardCommandClass;          /*!< card command classes [95:84] */
    uint8_t readBlockLength;            /*!< Max. read data block length [83:80] */
    uint16_t flags;                     /*!< Contain flags in _mmc_csd_flag */
    uint16_t deviceSize;                /*!< Device size [73:62] */
    uint8_t readCurrentVddMin;          /*!< Max. read current @ VDD min [61:59] */
    uint8_t readCurrentVddMax;          /*!< Max. read current @ VDD max [58:56] */
    uint8_t writeCurrentVddMin;         /*!< Max. write current @ VDD min [55:53] */
    uint8_t writeCurrentVddMax;         /*!< Max. write current @ VDD max [52:50] */
    uint8_t deviceSizeMultiplier;       /*!< Device size multiplier [49:47] */
    uint8_t eraseGroupSize;             /*!< Erase group size [46:42] */
    uint8_t eraseGroupSizeMultiplier;   /*!< Erase group size multiplier [41:37] */
    uint8_t writeProtectGroupSize;      /*!< Write protect group size [36:32] */
    uint8_t defaultEcc;                 /*!< Manufacturer default ECC [30:29] */
    uint8_t writeSpeedFactor;           /*!< Write speed factor [28:26] */
    uint8_t maxWriteBlockLength;        /*!< Max. write data block length [25:22] */
    uint8_t fileFormat;                 /*!< File format [11:10] */
    uint8_t eccCode;                    /*!< ECC code [9:8] */
} mmc_csd_t;

/*! @brief MMC card Extended CSD register (unit: byte). */
typedef struct _mmc_extended_csd
{
    /*uint8_t SecureRemoveType;*/             /*!< secure removal type[16]*/
    /*uint8_t enProductStateAware;*/          /*!< product state awareness enablement[17]*/
    /*uint32_t maxPreLoadDataSize;*/          /*!< max preload data size[21-18]*/
    /*uint32_t preLoadDataSize;*/             /*!< pre-load data size[25-22]*/
    /*uint8_t ffuStatus;*/                    /*!< FFU status [26]*/
    /*uint8_t modeOperationCode;*/            /*!< mode operation code[29]*/
    /*uint8_t modeConfig;*/                   /*!< mode config [30]*/
    /*uint8_t cacheCtrl;*/                    /*!< control to turn on/off cache[33]*/
    /*uint8_t pwroffNotify;*/                 /*!< power off notification[34]*/
    /*uint8_t packedCmdFailIndex;*/           /*!< packed cmd fail index [35]*/
    /*uint8_t packedCmdStatus;*/              /*!< packed cmd status[36]*/
    /*uint32_t contextConfig[4U];*/           /*!< context configuration[51-37]*/
    /*uint16_t extPartitionAttr;*/            /*!< extended partitions attribut[53-52]*/
    /*uint16_t exceptEventStatus;*/           /*!< exception events status[55-54]*/
    /*uint16_t exceptEventControl;*/          /*!< exception events control[57-56]*/
    /*uint8_t toReleaseAddressedGroup;*/      /*!< number of group to be released[58]*/
    /*uint8_t class6CmdCtrl;*/                /*!< class 6 command control[59]*/
    /*uint8_t intTimeoutEmu;*/                /*!< 1st initiallization after disabling sector size emu[60]*/
    /*uint8_t sectorSize;*/                   /*!< sector size[61] */
    /*uint8_t sectorSizeEmu;*/                /*!< sector size emulation[62]*/
    /*uint8_t nativeSectorSize;*/             /*!< native sector size[63]*/
    /*uint8_t periodWakeup;*/                 /*!< period wakeup [131]*/
    /*uint8_t tCASESupport;*/                 /*!< package case temperature is controlled[132]*/
    /*uint8_t productionStateAware;*/         /*!< production state awareness[133]*/
    /*uint32_t enhanceUsrDataStartAddr;*/     /*!< enhanced user data start addr [139-136]*/
    /*uint32_t enhanceUsrDataSize;*/          /*!< enhanced user data area size[142-140]*/
    /*uint32_t generalPartitionSize[3];*/     /*!< general purpose partition size[154-143]*/
    uint8_t partitionAttribute;               /*!< partition attribute [156]*/
    /*uint32_t maxEnhanceAreaSize;*/          /*!< max enhance area size [159-157]*/
    /*uint8_t hpiManagementEn;*/              /*!< HPI management [161]*/
    /*uint8_t writeReliabilityParameter;*/    /*!< write reliability parameter register[166] */
    /*uint8_t writeReliabilitySet;*/          /*!< write reliability setting register[167] */
    /*uint8_t rpmbSizeMult;*/                 /*!< RPMB size multi [168]*/
    /*uint8_t fwConfig;*/                     /*!< FW configuration[169]*/
    uint8_t userWP;                           /*!< user write protect register[171] */
    uint8_t bootPartitionWP;                  /*!< boot write protect register[173]*/
    uint8_t bootWPStatus;                     /*!< boot write protect status register[174]*/
    uint8_t highDensityEraseGroupDefinition;  /*!< High-density erase group definition [175] */
    uint8_t bootDataBusConditions;            /*!< Boot bus conditions [177] */
    uint8_t bootConfigProtect;                /*!< Boot config protection [178]*/
    uint8_t partitionConfig;                  /*!< Boot configuration [179] */
    uint8_t eraseMemoryContent;               /*!< Erased memory content [181] */
    uint8_t dataBusWidth;                     /*!< Data bus width mode [183] */
    uint8_t highSpeedTiming;                  /*!< High-speed interface timing [185] */
    uint8_t powerClass;                       /*!< Power class [187] */
    uint8_t commandSetRevision;               /*!< Command set revision [189] */
    uint8_t commandSet;                       /*!< Command set [191] */
    uint8_t extendecCsdVersion;               /*!< Extended CSD revision [192] */
    uint8_t csdStructureVersion;              /*!< CSD structure version [194] */
    uint8_t cardType;                         /*!< Card Type [196] */
    uint8_t ioDriverStrength;                 /*!< IO driver strength [197] */
    /*uint8_t OutofInterruptBusyTiming;*/     /*!< out of interrupt busy timing [198] */
    /*uint8_t partitionSwitchTiming;*/        /*!< partition switch timing [199] */
    uint8_t powerClass52MHz195V;              /*!< Power Class for 52MHz @ 1.95V [200] */
    uint8_t powerClass26MHz195V;              /*!< Power Class for 26MHz @ 1.95V [201] */
    uint8_t powerClass52MHz360V;              /*!< Power Class for 52MHz @ 3.6V [202] */
    uint8_t powerClass26MHz360V;              /*!< Power Class for 26MHz @ 3.6V [203] */
    uint8_t minimumReadPerformance4Bit26MHz;  /*!< Minimum Read Performance for 4bit at 26MHz [205] */
    uint8_t minimumWritePerformance4Bit26MHz; /*!< Minimum Write Performance for 4bit at 26MHz [206] */
    uint8_t minimumReadPerformance8Bit26MHz4Bit52MHz;
    /*!< Minimum read Performance for 8bit at 26MHz/4bit @52MHz [207] */
    uint8_t minimumWritePerformance8Bit26MHz4Bit52MHz;
    /*!< Minimum Write Performance for 8bit at 26MHz/4bit @52MHz [208] */
    uint8_t minimumReadPerformance8Bit52MHz;   /*!< Minimum Read Performance for 8bit at 52MHz [209] */
    uint8_t minimumWritePerformance8Bit52MHz;  /*!< Minimum Write Performance for 8bit at 52MHz [210] */
    uint32_t sectorCount;                      /*!< Sector Count [215:212] */
    /*uint8_t sleepNotificationTimeout;*/      /*!< sleep notification timeout [216]*/
    uint8_t sleepAwakeTimeout;                 /*!< Sleep/awake timeout [217] */
    /*uint8_t productionStateAwareTimeout;*/   /*!< Production state awareness timeout [218]*/
    uint8_t sleepCurrentVCCQ;                  /*!< Sleep current (VCCQ) [219] */
    uint8_t sleepCurrentVCC;                   /*!< Sleep current (VCC) [220] */
    uint8_t highCapacityWriteProtectGroupSize; /*!< High-capacity write protect group size [221] */
    uint8_t reliableWriteSectorCount;          /*!< Reliable write sector count [222] */
    uint8_t highCapacityEraseTimeout;          /*!< High-capacity erase timeout [223] */
    uint8_t highCapacityEraseUnitSize;         /*!< High-capacity erase unit size [224] */
    uint8_t accessSize;                        /*!< Access size [225] */
    /*uint8_t secureTrimMultiplier;*/          /*!< secure trim multiplier[229]*/
    /*uint8_t secureEraseMultiplier;*/         /*!< secure erase multiplier[230]*/
    /*uint8_t secureFeatureSupport;*/          /*!< secure feature support[231]*/
    /*uint32_t trimMultiplier;*/               /*!< trim multiplier[232]*/
    uint8_t minReadPerformance8bitAt52MHZDDR;  /*!< Minimum read performance for 8bit at DDR 52MHZ[234]*/
    uint8_t minWritePerformance8bitAt52MHZDDR; /*!< Minimum write performance for 8bit at DDR 52MHZ[235]*/
    uint8_t powerClass200MHZVCCQ130VVCC360V;   /*!< power class for 200MHZ, at VCCQ= 1.3V,VCC=3.6V[236]*/
    uint8_t powerClass200MHZVCCQ195VVCC360V;   /*!< power class for 200MHZ, at VCCQ= 1.95V,VCC=3.6V[237]*/
    uint8_t powerClass52MHZDDR195V;            /*!< power class for 52MHZ,DDR at Vcc 1.95V[238]*/
    uint8_t powerClass52MHZDDR360V;            /*!< power class for 52MHZ,DDR at Vcc 3.6V[239]*/
    /*uint8_t iniTimeoutAP;*/                  /*!< 1st initialization time after partitioning[241]*/
    /*uint32_t correctPrgSectorNum;*/          /*!< correct prg sectors number[245-242]*/
    /*uint8_t bkOpsStatus;*/                   /*!< background operations status[246]*/
    /*uint8_t powerOffNotifyTimeout;*/         /*!< power off notification timeout[247]*/
    /*uint8_t genericCMD6Timeout;*/            /*!< generic CMD6 timeout[248]*/
    uint32_t cacheSize;                        /*!< cache size[252-249]*/
    uint8_t powerClass200MHZDDR360V;           /*!< power class for 200MHZ, DDR at VCC=2.6V[253]*/
    /*uint32_t fwVer[2U];*/                    /*!< fw VERSION [261-254]*/
    /*uint16_t deviceVer;*/                    /*!< device version[263-262]*/
    /*uint8_t optimalTrimSize;*/               /*!< optimal trim size[264]*/
    /*uint8_t optimalWriteSize;*/              /*!< optimal write size[265]*/
    /*uint8_t optimalReadSize;*/               /*!< optimal read size[266]*/
    /*uint8_t preEolInfo;*/                    /*!< pre EOL information[267]*/
    /*uint8_t deviceLifeTimeEstimationA;*/     /*!< device life time estimation typeA[268]*/
    /*uint8_t deviceLifeTimeEstimationB;*/     /*!< device life time estimation typeB[269]*/
    /*uint32_t correctPrgFWSectorNum;*/        /*!< number of FW sectors correctly programmed[305-302]*/
    /*uint32_t ffuArg;*/                       /*!< FFU argument[490-487]*/
    /*uint8_t operationCodeTimeout;*/          /*!< operation code timeout[491]*/
    /*uint8_t supportMode;*/                   /*!< support mode [493]*/
    uint8_t extPartitionSupport;               /*!< extended partition attribute support[494]*/
    /*uint8_t largeUnitSize;*/                 /*!< large unit size[495]*/
    /*uint8_t contextManageCap;*/              /*!< context management capability[496]*/
    /*uint8_t tagResourceSize;*/               /*!< tag resource size[497]*/
    /*uint8_t tagUnitSize;*/                   /*!< tag unit size[498]*/
    /*uint8_t maxPackedWriteCmd;*/             /*!< max packed write cmd[500]*/
    /*uint8_t maxPackedReadCmd;*/              /*!< max packed read cmd[501]*/
    /*uint8_t hpiFeature;*/                    /*!< HPI feature[503]*/
    uint8_t supportedCommandSet;               /*!< Supported Command Sets [504] */
    /*uint8_t extSecurityCmdError;*/           /*!< extended security commands error[505]*/
} mmc_extended_csd_t;

/*! @brief The bit shift for COMMAND SET field in SWITCH command. */
#define MMC_SWITCH_COMMAND_SET_SHIFT (0U)
/*! @brief The bit mask for COMMAND set field in SWITCH command. */
#define MMC_SWITCH_COMMAND_SET_MASK (0x00000007U)
/*! @brief The bit shift for VALUE field in SWITCH command */
#define MMC_SWITCH_VALUE_SHIFT (8U)
/*! @brief The bit mask for VALUE field in SWITCH command */
#define MMC_SWITCH_VALUE_MASK (0x0000FF00U)
/*! @brief The bit shift for BYTE INDEX field in SWITCH command */
#define MMC_SWITCH_BYTE_INDEX_SHIFT (16U)
/*! @brief The bit mask for BYTE INDEX field in SWITCH command */
#define MMC_SWITCH_BYTE_INDEX_MASK (0x00FF0000U)
/*! @brief The bit shift for ACCESS MODE field in SWITCH command */
#define MMC_SWITCH_ACCESS_MODE_SHIFT (24U)
/*! @brief The bit mask for ACCESS MODE field in SWITCH command */
#define MMC_SWTICH_ACCESS_MODE_MASK (0x03000000U)

/*! @brief MMC Extended CSD configuration. */
typedef struct _mmc_extended_csd_config
{
    mmc_command_set_t commandSet;              /*!< Command set */
    uint8_t ByteValue;                         /*!< The value to set */
    uint8_t ByteIndex;                         /*!< The byte index in Extended CSD(mmc_extended_csd_index_t) */
    mmc_extended_csd_access_mode_t accessMode; /*!< Access mode */
} mmc_extended_csd_config_t;

/*! @brief MMC card boot configuration definition. */
typedef struct _mmc_boot_config
{
    bool enableBootAck;                        /*!< Enable boot ACK */
    mmc_boot_partition_enable_t bootPartition; /*!< Boot partition */

    mmc_boot_timing_mode_t bootTimingMode; /*!< boot mode */
    mmc_data_bus_width_t bootDataBusWidth; /*!< Boot data bus width */
    bool retainBootbusCondition;           /*!< If retain boot bus width and boot mode conditions */

    bool pwrBootConfigProtection;  /*!< Disable the change of boot configuration register bits from at this point
                                   until next power cycle or next H/W reset operation */
    bool premBootConfigProtection; /*!< Disable the change of boot configuration register bits permanently */

    mmc_boot_partition_wp_t bootPartitionWP; /*!< boot partition write protect configurations */

} mmc_boot_config_t;

/* @} */

#endif /* _FSL_SDMMC_SPEC_H_ */
