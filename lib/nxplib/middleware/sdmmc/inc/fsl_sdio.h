/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _FSL_SDIO_H_
#define _FSL_SDIO_H_

#include "fsl_sdmmc_common.h"

/*!
 * @addtogroup SDIOCARD
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*! @brief Middleware version. */
#define FSL_SDIO_DRIVER_VERSION (MAKE_VERSION(2U, 2U, 11U)) /*2.2.11*/

/*!@brief sdio device support maximum IO number */
#ifndef FSL_SDIO_MAX_IO_NUMS
#define FSL_SDIO_MAX_IO_NUMS (7U)
#endif
/*!@brief sdio card descriptor */
typedef struct _sdio_card sdio_card_t;
/*!@brief sdio io handler */
typedef void (*sdio_io_irq_handler_t)(sdio_card_t *card, uint32_t func);
/*! @brief sdio io read/write direction */
typedef enum _sdio_io_direction
{
    kSDIO_IORead  = 0U, /*!< io read */
    kSDIO_IOWrite = 1U, /*!< io write */
} sdio_io_direction_t;

/*!
 * @brief SDIO card state
 *
 * Define the card structure including the necessary fields to identify and describe the card.
 */
struct _sdio_card
{
    SDMMCHOST_CONFIG host;         /*!< Host information */
    sdiocard_usr_param_t usrParam; /*!< user parameter */
    bool noInternalAlign;          /*!< use this flag to disable sdmmc align. If disable, sdmmc will not make sure the
                                  data buffer address is word align, otherwise all the transfer are align to low level driver */
    bool isHostReady;              /*!< use this flag to indicate if need host re-init or not*/
    bool memPresentFlag;           /*!< indicate if memory present */

    uint32_t busClock_Hz;                       /*!< SD bus clock frequency united in Hz */
    uint32_t relativeAddress;                   /*!< Relative address of the card */
    uint8_t sdVersion;                          /*!< SD version */
    sd_timing_mode_t currentTiming;             /*!< current timing mode */
    sd_driver_strength_t driverStrength;        /*!< driver strength */
    sd_max_current_t maxCurrent;                /*!< card current limit */
    sdmmc_operation_voltage_t operationVoltage; /*!< card operation voltage */

    uint8_t sdioVersion;         /*!< SDIO version */
    uint8_t cccrVersioin;        /*!< CCCR version */
    uint8_t ioTotalNumber;       /*!< total number of IO function */
    uint32_t cccrflags;          /*!< Flags in _sd_card_flag */
    uint32_t io0blockSize;       /*!< record the io0 block size*/
    uint32_t ocr;                /*!< Raw OCR content, only 24bit avalible for SDIO card */
    uint32_t commonCISPointer;   /*!< point to common CIS */
    sdio_common_cis_t commonCIS; /*!< CIS table */

    /* io registers/IRQ handler */
    sdio_fbr_t ioFBR[FSL_SDIO_MAX_IO_NUMS];                   /*!< FBR table */
    sdio_func_cis_t funcCIS[FSL_SDIO_MAX_IO_NUMS];            /*!< function CIS table*/
    sdio_io_irq_handler_t ioIRQHandler[FSL_SDIO_MAX_IO_NUMS]; /*!< io IRQ handler */
    uint8_t ioIntIndex;                                       /*!< used to record current enabled io interrupt index */
    uint8_t ioIntNums;                                        /*!< used to record total enabled io interrupt numbers  */
};

/*************************************************************************************************
 * API
 ************************************************************************************************/
#if defined(__cplusplus)
extern "C"
{
#endif
/*!
 * @name Initialization and deinitialization
 * @{
 */

/*!
 * @brief SDIO card init function
 *
 * @param card Card descriptor.
 * @retval kStatus_SDMMC_GoIdleFailed
 * @retval kStatus_SDMMC_HandShakeOperationConditionFailed
 * @retval kStatus_SDMMC_SDIO_InvalidCard
 * @retval kStatus_SDMMC_SDIO_InvalidVoltage
 * @retval kStatus_SDMMC_SendRelativeAddressFailed
 * @retval kStatus_SDMMC_SelectCardFailed
 * @retval kStatus_SDMMC_SDIO_SwitchHighSpeedFail
 * @retval kStatus_SDMMC_SDIO_ReadCISFail
 * @retval kStatus_SDMMC_TransferFailed
 * @retval kStatus_Success
 */
status_t SDIO_Init(sdio_card_t *card);

/*!
 * @brief SDIO card deinit, include card and host deinit.
 *
 * @param card Card descriptor.
 */
void SDIO_Deinit(sdio_card_t *card);

/*!
 * @brief Initializes the card.
 *
 * This function initializes the card only, make sure the host is ready when call this function,
 * otherwise it will return kStatus_SDMMC_HostNotReady.
 *
 * @param card Card descriptor.
 * @retval kStatus_SDMMC_HostNotReady host is not ready.
 * @retval kStatus_SDMMC_GoIdleFailed Go idle failed.
 * @retval kStatus_SDMMC_NotSupportYet Card not support.
 * @retval kStatus_SDMMC_SendOperationConditionFailed Send operation condition failed.
 * @retval kStatus_SDMMC_AllSendCidFailed Send CID failed.
 * @retval kStatus_SDMMC_SendRelativeAddressFailed Send relative address failed.
 * @retval kStatus_SDMMC_SendCsdFailed Send CSD failed.
 * @retval kStatus_SDMMC_SelectCardFailed Send SELECT_CARD command failed.
 * @retval kStatus_SDMMC_SendScrFailed Send SCR failed.
 * @retval kStatus_SDMMC_SetBusWidthFailed Set bus width failed.
 * @retval kStatus_SDMMC_SwitchHighSpeedFailed Switch high speed failed.
 * @retval kStatus_SDMMC_SetCardBlockSizeFailed Set card block size failed.
 * @retval kStatus_Success Operate successfully.
 */
status_t SDIO_CardInit(sdio_card_t *card);

/*!
 * @brief Deinitializes the card.
 *
 * This function deinitializes the specific card.
 *
 * @param card Card descriptor.
 */
void SDIO_CardDeinit(sdio_card_t *card);

/*!
 * @brief initialize the host.
 *
 * This function deinitializes the specific host.
 *
 * @param card Card descriptor.
 */
status_t SDIO_HostInit(sdio_card_t *card);

/*!
 * @brief Deinitializes the host.
 *
 * This function deinitializes the host.
 *
 * @param card Card descriptor.
 */
void SDIO_HostDeinit(sdio_card_t *card);

/*!
 * @brief reset the host.
 *
 * This function reset the specific host.
 *
 * @param host host descriptor.
 */
void SDIO_HostReset(SDMMCHOST_CONFIG *host);

/*!
 * @brief power on card.
 *
 * The power on operation depend on host or the user define power on function.
 * @param base host base address.
 * @param pwr user define power control configuration
 */
void SDIO_PowerOnCard(SDMMCHOST_TYPE *base, const sdmmchost_pwr_card_t *pwr);

/*!
 * @brief power on card.
 *
 * The power off operation depend on host or the user define power on function.
 * @param base host base address.
 * @param pwr user define power control configuration
 */
void SDIO_PowerOffCard(SDMMCHOST_TYPE *base, const sdmmchost_pwr_card_t *pwr);

/*!
 * @brief set SDIO card to inactive state
 *
 * @param card Card descriptor.
 * @retval kStatus_SDMMC_TransferFailed
 * @retval kStatus_Success
 */
status_t SDIO_CardInActive(sdio_card_t *card);

/*!
 * @brief get SDIO card capability
 *
 * @param card Card descriptor.
 * @param function IO number
 * @retval kStatus_SDMMC_TransferFailed
 * @retval kStatus_Success
 */
status_t SDIO_GetCardCapability(sdio_card_t *card, sdio_func_num_t func);

/*!
 * @brief set SDIO card block size
 *
 * @param card Card descriptor.
 * @param function io number
 * @param block size
 * @retval kStatus_SDMMC_SetCardBlockSizeFailed
 * @retval kStatus_SDMMC_SDIO_InvalidArgument
 * @retval kStatus_Success
 */
status_t SDIO_SetBlockSize(sdio_card_t *card, sdio_func_num_t func, uint32_t blockSize);

/*!
 * @brief set SDIO card reset
 *
 * @param card Card descriptor.
 * @retval kStatus_SDMMC_TransferFailed
 * @retval kStatus_Success
 */
status_t SDIO_CardReset(sdio_card_t *card);

/*!
 * @brief set SDIO card data bus width
 *
 * @param card Card descriptor.
 * @param data bus width
 * @retval kStatus_SDMMC_TransferFailed
 * @retval kStatus_Success
 */
status_t SDIO_SetDataBusWidth(sdio_card_t *card, sdio_bus_width_t busWidth);

/*!
 * @brief switch the card to high speed
 *
 * @param card Card descriptor.
 * @retval kStatus_SDMMC_TransferFailed
 * @retval kStatus_SDMMC_SDIO_SwitchHighSpeedFail
 * @retval kStatus_Success
 */
status_t SDIO_SwitchToHighSpeed(sdio_card_t *card);

/*!
 * @brief read SDIO card CIS for each function
 *
 * @param card Card descriptor.
 * @param function io number
 * @param tuple code list
 * @param tuple code number
 * @retval kStatus_SDMMC_SDIO_ReadCISFail
 * @retval kStatus_SDMMC_TransferFailed
 * @retval kStatus_Success
 */
status_t SDIO_ReadCIS(sdio_card_t *card, sdio_func_num_t func, const uint32_t *tupleList, uint32_t tupleNum);

/*!
 * @brief sdio wait card detect function.
 *
 * Detect card through GPIO, CD, DATA3.
 *
 * @param card card descriptor.
 * @param card detect configuration
 * @param waitCardStatus wait card detect status
 */
status_t SDIO_WaitCardDetectStatus(SDMMCHOST_TYPE *hostBase,
                                   const sdmmchost_detect_card_t *cd,
                                   bool waitCardStatus);

/*!
 * @brief sdio card present check function.
 *
 * @param card card descriptor.
 */
bool SDIO_IsCardPresent(sdio_card_t *card);

/* @} */

/*!
 * @name IO operations
 * @{
 */

/*!
 * @brief IO direct write transfer function
 *
 * @param card Card descriptor.
 * @param function IO numner
 * @param register address
 * @param the data pinter to write
 * @param raw flag, indicate read after write or write only
 * @retval kStatus_SDMMC_TransferFailed
 * @retval kStatus_Success
 */
status_t SDIO_IO_Write_Direct(sdio_card_t *card, sdio_func_num_t func, uint32_t regAddr, uint8_t *data, bool raw);

/*!
 * @brief IO direct read transfer function
 *
 * @param card Card descriptor.
 * @param function IO number
 * @param register address
 * @param data pointer to read
 * @retval kStatus_SDMMC_TransferFailed
 * @retval kStatus_Success
 */
status_t SDIO_IO_Read_Direct(sdio_card_t *card, sdio_func_num_t func, uint32_t regAddr, uint8_t *data);

/*!
 * @brief IO direct read/write transfer function
 *
 * @param card Card descriptor.
 * @param direction io access direction, please reference sdio_io_direction_t.
 * @param function IO number
 * @param register address
 * @param dataIn data to write
 * @param dataOut data pointer for readback data, support both for read and write, when application want readback
 * the data after write command, dataOut should not be NULL.
 * @retval kStatus_SDMMC_TransferFailed
 * @retval kStatus_Success
 */

status_t SDIO_IO_RW_Direct(sdio_card_t *card,
                           sdio_io_direction_t direction,
                           sdio_func_num_t func,
                           uint32_t regAddr,
                           uint8_t dataIn,
                           uint8_t *dataOut);

/*!
 * @brief IO extended write transfer function
 *
 * @param card Card descriptor.
 * @param function IO number
 * @param register address
 * @param data buffer to write
 * @param data count
 * @param write flags
 * @retval kStatus_SDMMC_TransferFailed
 * @retval kStatus_SDMMC_SDIO_InvalidArgument
 * @retval kStatus_Success
 */
status_t SDIO_IO_Write_Extended(
    sdio_card_t *card, sdio_func_num_t func, uint32_t regAddr, uint8_t *buffer, uint32_t count, uint32_t flags);
/*!
 * @brief IO extended read transfer function
 *
 * @param card Card descriptor.
 * @param function IO number
 * @param register address
 * @param data buffer to read
 * @param data count
 * @param write flags
 * @retval kStatus_SDMMC_TransferFailed
 * @retval kStatus_SDMMC_SDIO_InvalidArgument
 * @retval kStatus_Success
 */
status_t SDIO_IO_Read_Extended(
    sdio_card_t *card, sdio_func_num_t func, uint32_t regAddr, uint8_t *buffer, uint32_t count, uint32_t flags);
/*!
 * @brief enable IO interrupt
 *
 * @param card Card descriptor.
 * @param function IO number
 * @param enable/disable flag
 * @retval kStatus_SDMMC_TransferFailed
 * @retval kStatus_Success
 */
status_t SDIO_EnableIOInterrupt(sdio_card_t *card, sdio_func_num_t func, bool enable);

/*!
 * @brief enable IO and wait IO ready
 *
 * @param card Card descriptor.
 * @param function IO number
 * @param enable/disable flag
 * @retval kStatus_SDMMC_TransferFailed
 * @retval kStatus_Success
 */
status_t SDIO_EnableIO(sdio_card_t *card, sdio_func_num_t func, bool enable);

/*!
 * @brief select IO
 *
 * @param card Card descriptor.
 * @param function IO number
 * @retval kStatus_SDMMC_TransferFailed
 * @retval kStatus_Success
 */
status_t SDIO_SelectIO(sdio_card_t *card, sdio_func_num_t func);

/*!
 * @brief Abort IO transfer
 *
 * @param card Card descriptor.
 * @param function IO number
 * @retval kStatus_SDMMC_TransferFailed
 * @retval kStatus_Success
 */
status_t SDIO_AbortIO(sdio_card_t *card, sdio_func_num_t func);

/*!
 * @brief Set driver strength.
 *
 * @param card Card descriptor.
 * @param driverStrength target driver strength.
 * @retval kStatus_SDMMC_TransferFailed
 * @retval kStatus_Success
 */
status_t SDIO_SetDriverStrength(sdio_card_t *card, sd_driver_strength_t driverStrength);

/*!
 * @brief Enable/Disable Async interrupt.
 *
 * @param card Card descriptor.
 * @param func function io number.
 * @param enable true is enable, false is disable.
 * @retval kStatus_SDMMC_TransferFailed
 * @retval kStatus_Success
 */
status_t SDIO_EnableAsyncInterrupt(sdio_card_t *card, bool enable);

/*!
 * @brief Get pending interrupt.
 *
 * @param card Card descriptor.
 * @param pendingInt pointer store pending interrupt
 * @retval kStatus_SDMMC_TransferFailed
 * @retval kStatus_Success
 */
status_t SDIO_GetPendingInterrupt(sdio_card_t *card, uint8_t *pendingInt);

/*!
 * @brief sdio card io transfer function.
 * This function can be used for trnansfer direct/extend command.
 * Please pay attention to the non-align data buffer address transfer,
 * if data buffer address can not meet host controller internal DMA requirement, sdio driver will try to use
 internal align buffer if data size is not bigger than internal buffer size,
 * Align address transfer always can get a better performance, so if application want sdio driver make sure buffer
 address align,
 * please redefine the SDMMC_GLOBAL_BUFFER_SIZE macro to a value which is big enough for your application.
 *
 * @param card card descriptor.
 * @param cmd command to transfer
 * @param argument argument to transfer
 * @param blockSize used for block mode.
 * @param txData tx buffer pointer or NULL
 * @param rxData rx buffer pointer or NULL
 * @param dataSize transfer data size
 * @param response reponse pointer, if application want read response back, please set it to a NON-NULL pointer.

 */
status_t SDIO_IO_Transfer(sdio_card_t *card,
                          sdio_command_t cmd,
                          uint32_t argument,
                          uint32_t blockSize,
                          uint8_t *txData,
                          uint8_t *rxData,
                          uint16_t dataSize,
                          uint32_t *response);

/*!
 * @brief sdio set io IRQ handler.
 *
 * @param card card descriptor.
 * @param func function io number.
 * @param handler,  io IRQ handler.
 */
void SDIO_SetIOIRQHandler(sdio_card_t *card, sdio_func_num_t func, sdio_io_irq_handler_t handler);

/*!
 * @brief sdio card io pending interrupt handle function.
 * This function is used to handle the pending io interrupt.
 * To reigster a IO IRQ handler,
 * @code
 * //initialization
 * SDIO_EnableIOInterrupt(card, 0, true);
 * SDIO_SetIOIRQHandler(card, 0, func0_handler);
 * //call it in interrupt callback
 * SDIO_HandlePendingIOInterrupt(card);
 * @code
 * To releae a IO IRQ handler,
 * @code
 * SDIO_EnableIOInterrupt(card, 0, false);
 * SDIO_SetIOIRQHandler(card, 0, NULL);
 * @code
 * @param card card descriptor.
 *
 * @retval kStatus_SDMMC_TransferFailed
 * @retval kStatus_Success
 */
status_t SDIO_HandlePendingIOInterrupt(sdio_card_t *card);

/* @} */

#if defined(__cplusplus)
}
#endif
/*! @} */
#endif /* _FSL_SDIO_H_*/
