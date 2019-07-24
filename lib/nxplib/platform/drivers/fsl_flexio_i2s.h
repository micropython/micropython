/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _FSL_FLEXIO_I2S_H_
#define _FSL_FLEXIO_I2S_H_

#include "fsl_common.h"
#include "fsl_flexio.h"

/*!
 * @addtogroup flexio_i2s
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
/*@{*/
/*! @brief FlexIO I2S driver version 2.1.6. */
#define FSL_FLEXIO_I2S_DRIVER_VERSION (MAKE_VERSION(2, 1, 6))
/*@}*/

/*! @brief FlexIO I2S transfer status */
enum _flexio_i2s_status
{
    kStatus_FLEXIO_I2S_Idle      = MAKE_STATUS(kStatusGroup_FLEXIO_I2S, 0), /*!< FlexIO I2S is in idle state */
    kStatus_FLEXIO_I2S_TxBusy    = MAKE_STATUS(kStatusGroup_FLEXIO_I2S, 1), /*!< FlexIO I2S Tx is busy */
    kStatus_FLEXIO_I2S_RxBusy    = MAKE_STATUS(kStatusGroup_FLEXIO_I2S, 2), /*!< FlexIO I2S Tx is busy */
    kStatus_FLEXIO_I2S_Error     = MAKE_STATUS(kStatusGroup_FLEXIO_I2S, 3), /*!< FlexIO I2S error occurred */
    kStatus_FLEXIO_I2S_QueueFull = MAKE_STATUS(kStatusGroup_FLEXIO_I2S, 4), /*!< FlexIO I2S transfer queue is full. */
};

/*! @brief Define FlexIO I2S access structure typedef */
typedef struct _flexio_i2s_type
{
    FLEXIO_Type *flexioBase; /*!< FlexIO base pointer */
    uint8_t txPinIndex;      /*!< Tx data pin index in FlexIO pins */
    uint8_t rxPinIndex;      /*!< Rx data pin index */
    uint8_t bclkPinIndex;    /*!< Bit clock pin index */
    uint8_t fsPinIndex;      /*!< Frame sync pin index */
    uint8_t txShifterIndex;  /*!< Tx data shifter index */
    uint8_t rxShifterIndex;  /*!< Rx data shifter index */
    uint8_t bclkTimerIndex;  /*!< Bit clock timer index */
    uint8_t fsTimerIndex;    /*!< Frame sync timer index */
} FLEXIO_I2S_Type;

/*! @brief Master or slave mode */
typedef enum _flexio_i2s_master_slave
{
    kFLEXIO_I2S_Master = 0x0U, /*!< Master mode */
    kFLEXIO_I2S_Slave  = 0x1U  /*!< Slave mode */
} flexio_i2s_master_slave_t;

/*! @brief Define FlexIO FlexIO I2S interrupt mask. */
enum _flexio_i2s_interrupt_enable
{
    kFLEXIO_I2S_TxDataRegEmptyInterruptEnable = 0x1U, /*!< Transmit buffer empty interrupt enable. */
    kFLEXIO_I2S_RxDataRegFullInterruptEnable  = 0x2U, /*!< Receive buffer full interrupt enable. */
};

/*! @brief Define FlexIO FlexIO I2S status mask. */
enum _flexio_i2s_status_flags
{
    kFLEXIO_I2S_TxDataRegEmptyFlag = 0x1U, /*!< Transmit buffer empty flag. */
    kFLEXIO_I2S_RxDataRegFullFlag  = 0x2U, /*!< Receive buffer full flag. */
};

/*! @brief FlexIO I2S configure structure */
typedef struct _flexio_i2s_config
{
    bool enableI2S;                                  /*!< Enable FlexIO I2S */
    flexio_i2s_master_slave_t masterSlave;           /*!< Master or slave */
    flexio_pin_polarity_t txPinPolarity;             /*!< Tx data pin polarity, active high or low */
    flexio_pin_polarity_t rxPinPolarity;             /*!< Rx data pin polarity */
    flexio_pin_polarity_t bclkPinPolarity;           /*!< Bit clock pin polarity */
    flexio_pin_polarity_t fsPinPolarity;             /*!< Frame sync pin polarity */
    flexio_shifter_timer_polarity_t txTimerPolarity; /*!< Tx data valid on bclk rising or falling edge */
    flexio_shifter_timer_polarity_t rxTimerPolarity; /*!< Rx data valid on bclk rising or falling edge */
} flexio_i2s_config_t;

/*! @brief FlexIO I2S audio format, FlexIO I2S only support the same format in Tx and Rx */
typedef struct _flexio_i2s_format
{
    uint8_t bitWidth;       /*!< Bit width of audio data, always 8/16/24/32 bits */
    uint32_t sampleRate_Hz; /*!< Sample rate of the audio data */
} flexio_i2s_format_t;

/*!@brief FlexIO I2S transfer queue size, user can refine it according to use case. */
#define FLEXIO_I2S_XFER_QUEUE_SIZE (4)

/*! @brief Audio sample rate */
typedef enum _flexio_i2s_sample_rate
{
    kFLEXIO_I2S_SampleRate8KHz    = 8000U,  /*!< Sample rate 8000Hz */
    kFLEXIO_I2S_SampleRate11025Hz = 11025U, /*!< Sample rate 11025Hz */
    kFLEXIO_I2S_SampleRate12KHz   = 12000U, /*!< Sample rate 12000Hz */
    kFLEXIO_I2S_SampleRate16KHz   = 16000U, /*!< Sample rate 16000Hz */
    kFLEXIO_I2S_SampleRate22050Hz = 22050U, /*!< Sample rate 22050Hz */
    kFLEXIO_I2S_SampleRate24KHz   = 24000U, /*!< Sample rate 24000Hz */
    kFLEXIO_I2S_SampleRate32KHz   = 32000U, /*!< Sample rate 32000Hz */
    kFLEXIO_I2S_SampleRate44100Hz = 44100U, /*!< Sample rate 44100Hz */
    kFLEXIO_I2S_SampleRate48KHz   = 48000U, /*!< Sample rate 48000Hz */
    kFLEXIO_I2S_SampleRate96KHz   = 96000U  /*!< Sample rate 96000Hz */
} flexio_i2s_sample_rate_t;

/*! @brief Audio word width */
typedef enum _flexio_i2s_word_width
{
    kFLEXIO_I2S_WordWidth8bits  = 8U,  /*!< Audio data width 8 bits */
    kFLEXIO_I2S_WordWidth16bits = 16U, /*!< Audio data width 16 bits */
    kFLEXIO_I2S_WordWidth24bits = 24U, /*!< Audio data width 24 bits */
    kFLEXIO_I2S_WordWidth32bits = 32U  /*!< Audio data width 32 bits */
} flexio_i2s_word_width_t;

/*! @brief Define FlexIO I2S transfer structure. */
typedef struct _flexio_i2s_transfer
{
    uint8_t *data;   /*!< Data buffer start pointer */
    size_t dataSize; /*!< Bytes to be transferred. */
} flexio_i2s_transfer_t;

typedef struct _flexio_i2s_handle flexio_i2s_handle_t;

/*! @brief FlexIO I2S xfer callback prototype */
typedef void (*flexio_i2s_callback_t)(FLEXIO_I2S_Type *base,
                                      flexio_i2s_handle_t *handle,
                                      status_t status,
                                      void *userData);

/*! @brief Define FlexIO I2S handle structure. */
struct _flexio_i2s_handle
{
    uint32_t state;                                          /*!< Internal state */
    flexio_i2s_callback_t callback;                          /*!< Callback function called at transfer event*/
    void *userData;                                          /*!< Callback parameter passed to callback function*/
    uint8_t bitWidth;                                        /*!< Bit width for transfer, 8/16/24/32bits */
    flexio_i2s_transfer_t queue[FLEXIO_I2S_XFER_QUEUE_SIZE]; /*!< Transfer queue storing queued transfer */
    size_t transferSize[FLEXIO_I2S_XFER_QUEUE_SIZE];         /*!< Data bytes need to transfer */
    volatile uint8_t queueUser;                              /*!< Index for user to queue transfer */
    volatile uint8_t queueDriver;                            /*!< Index for driver to get the transfer data and size */
};

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /*_cplusplus*/

/*!
 * @name Initialization and deinitialization
 * @{
 */

/*!
 * @brief Initializes the FlexIO I2S.
 *
 * This API configures FlexIO pins and shifter to I2S and configures the FlexIO I2S with a configuration structure.
 * The configuration structure can be filled by the user, or be set with default values by
 * FLEXIO_I2S_GetDefaultConfig().
 *
 * @note  This API should be called at the beginning of the application to use
 * the FlexIO I2S driver. Otherwise, any access to the FlexIO I2S module can cause hard fault
 * because the clock is not enabled.
 *
 * @param base FlexIO I2S base pointer
 * @param config FlexIO I2S configure structure.
 */
void FLEXIO_I2S_Init(FLEXIO_I2S_Type *base, const flexio_i2s_config_t *config);

/*!
 * @brief  Sets the FlexIO I2S configuration structure to default values.
 *
 * The purpose of this API is to get the configuration structure initialized for use in FLEXIO_I2S_Init().
 * Users may use the initialized structure unchanged in FLEXIO_I2S_Init() or modify
 * some fields of the structure before calling FLEXIO_I2S_Init().
 *
 * @param config pointer to master configuration structure
 */
void FLEXIO_I2S_GetDefaultConfig(flexio_i2s_config_t *config);

/*!
 * @brief De-initializes the FlexIO I2S.
 *
 * Calling this API resets the FlexIO I2S shifter and timer config. After calling this API,
 * call the FLEXO_I2S_Init to use the FlexIO I2S module.
 *
 * @param base FlexIO I2S base pointer
 */
void FLEXIO_I2S_Deinit(FLEXIO_I2S_Type *base);

/*!
 * @brief Enables/disables the FlexIO I2S module operation.
 *
 * @param base Pointer to FLEXIO_I2S_Type
 * @param enable True to enable, false dose not have any effect.
 */
static inline void FLEXIO_I2S_Enable(FLEXIO_I2S_Type *base, bool enable)
{
    if (enable)
    {
        base->flexioBase->CTRL |= FLEXIO_CTRL_FLEXEN_MASK;
    }
}

/*! @} */

/*!
 * @name Status
 * @{
 */

/*!
 * @brief Gets the FlexIO I2S status flags.
 *
 * @param base Pointer to FLEXIO_I2S_Type structure
 * @return Status flag, which are ORed by the enumerators in the _flexio_i2s_status_flags.
 */
uint32_t FLEXIO_I2S_GetStatusFlags(FLEXIO_I2S_Type *base);

/*! @} */

/*!
 * @name Interrupts
 * @{
 */

/*!
 * @brief Enables the FlexIO I2S interrupt.
 *
 * This function enables the FlexIO UART interrupt.
 *
 * @param base Pointer to FLEXIO_I2S_Type structure
 * @param mask interrupt source
 */
void FLEXIO_I2S_EnableInterrupts(FLEXIO_I2S_Type *base, uint32_t mask);

/*!
 * @brief Disables the FlexIO I2S interrupt.
 *
 * This function enables the FlexIO UART interrupt.
 *
 * @param base pointer to FLEXIO_I2S_Type structure
 * @param mask interrupt source
 */
void FLEXIO_I2S_DisableInterrupts(FLEXIO_I2S_Type *base, uint32_t mask);

/*! @} */

/*!
 * @name DMA Control
 * @{
 */

/*!
 * @brief Enables/disables the FlexIO I2S Tx DMA requests.
 *
 * @param base FlexIO I2S base pointer
 * @param enable True means enable DMA, false means disable DMA.
 */
static inline void FLEXIO_I2S_TxEnableDMA(FLEXIO_I2S_Type *base, bool enable)
{
    FLEXIO_EnableShifterStatusDMA(base->flexioBase, 1 << base->txShifterIndex, enable);
}

/*!
 * @brief Enables/disables the FlexIO I2S Rx DMA requests.
 *
 * @param base FlexIO I2S base pointer
 * @param enable True means enable DMA, false means disable DMA.
 */
static inline void FLEXIO_I2S_RxEnableDMA(FLEXIO_I2S_Type *base, bool enable)
{
    FLEXIO_EnableShifterStatusDMA(base->flexioBase, 1 << base->rxShifterIndex, enable);
}

/*!
 * @brief Gets the FlexIO I2S send data register address.
 *
 * This function returns the I2S data register address, mainly used by DMA/eDMA.
 *
 * @param base Pointer to FLEXIO_I2S_Type structure
 * @return FlexIO i2s send data register address.
 */
static inline uint32_t FLEXIO_I2S_TxGetDataRegisterAddress(FLEXIO_I2S_Type *base)
{
    return FLEXIO_GetShifterBufferAddress(base->flexioBase, kFLEXIO_ShifterBufferBitSwapped, base->txShifterIndex);
}

/*!
 * @brief Gets the FlexIO I2S receive data register address.
 *
 * This function returns the I2S data register address, mainly used by DMA/eDMA.
 *
 * @param base Pointer to FLEXIO_I2S_Type structure
 * @return FlexIO i2s receive data register address.
 */
static inline uint32_t FLEXIO_I2S_RxGetDataRegisterAddress(FLEXIO_I2S_Type *base)
{
    return FLEXIO_GetShifterBufferAddress(base->flexioBase, kFLEXIO_ShifterBufferBitSwapped, base->rxShifterIndex);
}

/*! @} */

/*!
 * @name Bus Operations
 * @{
 */

/*!
 * @brief Configures the FlexIO I2S audio format in master mode.
 *
 * Audio format can be changed in run-time of FlexIO I2S. This function configures the sample rate and audio data
 * format to be transferred.
 *
 * @param base Pointer to FLEXIO_I2S_Type structure
 * @param format Pointer to FlexIO I2S audio data format structure.
 * @param srcClock_Hz I2S master clock source frequency in Hz.
 */
void FLEXIO_I2S_MasterSetFormat(FLEXIO_I2S_Type *base, flexio_i2s_format_t *format, uint32_t srcClock_Hz);

/*!
 * @brief Configures the FlexIO I2S audio format in slave mode.
 *
 * Audio format can be changed in run-time of FlexIO I2S. This function configures the sample rate and audio data
 * format to be transferred.
 *
 * @param base Pointer to FLEXIO_I2S_Type structure
 * @param format Pointer to FlexIO I2S audio data format structure.
 */
void FLEXIO_I2S_SlaveSetFormat(FLEXIO_I2S_Type *base, flexio_i2s_format_t *format);

/*!
 * @brief Sends data using a blocking method.
 *
 * @note This function blocks via polling until data is ready to be sent.
 *
 * @param base FlexIO I2S base pointer.
 * @param bitWidth How many bits in a audio word, usually 8/16/24/32 bits.
 * @param txData Pointer to the data to be written.
 * @param size Bytes to be written.
 */
void FLEXIO_I2S_WriteBlocking(FLEXIO_I2S_Type *base, uint8_t bitWidth, uint8_t *txData, size_t size);

/*!
 * @brief Writes data into a data register.
 *
 * @param base FlexIO I2S base pointer.
 * @param bitWidth How many bits in a audio word, usually 8/16/24/32 bits.
 * @param data Data to be written.
 */
static inline void FLEXIO_I2S_WriteData(FLEXIO_I2S_Type *base, uint8_t bitWidth, uint32_t data)
{
    base->flexioBase->SHIFTBUFBIS[base->txShifterIndex] = (data << (32U - bitWidth));
}

/*!
 * @brief Receives a piece of data using a blocking method.
 *
 * @note This function blocks via polling until data is ready to be sent.
 *
 * @param base FlexIO I2S base pointer
 * @param bitWidth How many bits in a audio word, usually 8/16/24/32 bits.
 * @param rxData Pointer to the data to be read.
 * @param size Bytes to be read.
 */
void FLEXIO_I2S_ReadBlocking(FLEXIO_I2S_Type *base, uint8_t bitWidth, uint8_t *rxData, size_t size);

/*!
 * @brief Reads a data from the data register.
 *
 * @param base FlexIO I2S base pointer
 * @return Data read from data register.
 */
static inline uint32_t FLEXIO_I2S_ReadData(FLEXIO_I2S_Type *base)
{
    return base->flexioBase->SHIFTBUFBIS[base->rxShifterIndex];
}

/*! @} */

/*!
 * @name Transactional
 * @{
 */

/*!
 * @brief Initializes the FlexIO I2S handle.
 *
 * This function initializes the FlexIO I2S handle which can be used for other
 * FlexIO I2S transactional APIs. Call this API once to get the
 * initialized handle.
 *
 * @param base Pointer to FLEXIO_I2S_Type structure
 * @param handle Pointer to flexio_i2s_handle_t structure to store the transfer state.
 * @param callback FlexIO I2S callback function, which is called while finished a block.
 * @param userData User parameter for the FlexIO I2S callback.
 */
void FLEXIO_I2S_TransferTxCreateHandle(FLEXIO_I2S_Type *base,
                                       flexio_i2s_handle_t *handle,
                                       flexio_i2s_callback_t callback,
                                       void *userData);

/*!
 * @brief Configures the FlexIO I2S audio format.
 *
 * Audio format can be changed at run-time of FlexIO I2S. This function configures the sample rate and audio data
 * format to be transferred.
 *
 * @param base Pointer to FLEXIO_I2S_Type structure.
 * @param handle FlexIO I2S handle pointer.
 * @param format Pointer to audio data format structure.
 * @param srcClock_Hz FlexIO I2S bit clock source frequency in Hz. This parameter should be 0 while in slave mode.
 */
void FLEXIO_I2S_TransferSetFormat(FLEXIO_I2S_Type *base,
                                  flexio_i2s_handle_t *handle,
                                  flexio_i2s_format_t *format,
                                  uint32_t srcClock_Hz);

/*!
 * @brief Initializes the FlexIO I2S receive handle.
 *
 * This function initializes the FlexIO I2S handle which can be used for other
 * FlexIO I2S transactional APIs. Call this API once to get the
 * initialized handle.
 *
 * @param base Pointer to FLEXIO_I2S_Type structure.
 * @param handle Pointer to flexio_i2s_handle_t structure to store the transfer state.
 * @param callback FlexIO I2S callback function, which is called while finished a block.
 * @param userData User parameter for the FlexIO I2S callback.
 */
void FLEXIO_I2S_TransferRxCreateHandle(FLEXIO_I2S_Type *base,
                                       flexio_i2s_handle_t *handle,
                                       flexio_i2s_callback_t callback,
                                       void *userData);

/*!
 * @brief Performs an interrupt non-blocking send transfer on FlexIO I2S.
 *
 * @note The API returns immediately after transfer initiates.
 * Call FLEXIO_I2S_GetRemainingBytes to poll the transfer status and check whether
 * the transfer is finished. If the return status is 0, the transfer is finished.
 *
 * @param base Pointer to FLEXIO_I2S_Type structure.
 * @param handle Pointer to flexio_i2s_handle_t structure which stores the transfer state
 * @param xfer Pointer to flexio_i2s_transfer_t structure
 * @retval kStatus_Success Successfully start the data transmission.
 * @retval kStatus_FLEXIO_I2S_TxBusy Previous transmission still not finished, data not all written to TX register yet.
 * @retval kStatus_InvalidArgument The input parameter is invalid.
 */
status_t FLEXIO_I2S_TransferSendNonBlocking(FLEXIO_I2S_Type *base,
                                            flexio_i2s_handle_t *handle,
                                            flexio_i2s_transfer_t *xfer);

/*!
 * @brief Performs an interrupt non-blocking receive transfer on FlexIO I2S.
 *
 * @note The API returns immediately after transfer initiates.
 * Call FLEXIO_I2S_GetRemainingBytes to poll the transfer status to check whether
 * the transfer is finished. If the return status is 0, the transfer is finished.
 *
 * @param base Pointer to FLEXIO_I2S_Type structure.
 * @param handle Pointer to flexio_i2s_handle_t structure which stores the transfer state
 * @param xfer Pointer to flexio_i2s_transfer_t structure
 * @retval kStatus_Success Successfully start the data receive.
 * @retval kStatus_FLEXIO_I2S_RxBusy Previous receive still not finished.
 * @retval kStatus_InvalidArgument The input parameter is invalid.
 */
status_t FLEXIO_I2S_TransferReceiveNonBlocking(FLEXIO_I2S_Type *base,
                                               flexio_i2s_handle_t *handle,
                                               flexio_i2s_transfer_t *xfer);

/*!
 * @brief Aborts the current send.
 *
 * @note This API can be called at any time when interrupt non-blocking transfer initiates
 * to abort the transfer in a early time.
 *
 * @param base Pointer to FLEXIO_I2S_Type structure.
 * @param handle Pointer to flexio_i2s_handle_t structure which stores the transfer state
 */
void FLEXIO_I2S_TransferAbortSend(FLEXIO_I2S_Type *base, flexio_i2s_handle_t *handle);

/*!
 * @brief Aborts the current receive.
 *
 * @note This API can be called at any time when interrupt non-blocking transfer initiates
 * to abort the transfer in a early time.
 *
 * @param base Pointer to FLEXIO_I2S_Type structure.
 * @param handle Pointer to flexio_i2s_handle_t structure which stores the transfer state
 */
void FLEXIO_I2S_TransferAbortReceive(FLEXIO_I2S_Type *base, flexio_i2s_handle_t *handle);

/*!
 * @brief Gets the remaining bytes to be sent.
 *
 * @param base Pointer to FLEXIO_I2S_Type structure.
 * @param handle Pointer to flexio_i2s_handle_t structure which stores the transfer state
 * @param count Bytes sent.
 * @retval kStatus_Success Succeed get the transfer count.
 * @retval kStatus_NoTransferInProgress There is not a non-blocking transaction currently in progress.
 */
status_t FLEXIO_I2S_TransferGetSendCount(FLEXIO_I2S_Type *base, flexio_i2s_handle_t *handle, size_t *count);

/*!
 * @brief Gets the remaining bytes to be received.
 *
 * @param base Pointer to FLEXIO_I2S_Type structure.
 * @param handle Pointer to flexio_i2s_handle_t structure which stores the transfer state
 * @return count Bytes received.
 * @retval kStatus_Success Succeed get the transfer count.
 * @retval kStatus_NoTransferInProgress There is not a non-blocking transaction currently in progress.
 */
status_t FLEXIO_I2S_TransferGetReceiveCount(FLEXIO_I2S_Type *base, flexio_i2s_handle_t *handle, size_t *count);

/*!
 * @brief Tx interrupt handler.
 *
 * @param i2sBase Pointer to FLEXIO_I2S_Type structure.
 * @param i2sHandle Pointer to flexio_i2s_handle_t structure
 */
void FLEXIO_I2S_TransferTxHandleIRQ(void *i2sBase, void *i2sHandle);

/*!
 * @brief Rx interrupt handler.
 *
 * @param i2sBase Pointer to FLEXIO_I2S_Type structure.
 * @param i2sHandle Pointer to flexio_i2s_handle_t structure.
 */
void FLEXIO_I2S_TransferRxHandleIRQ(void *i2sBase, void *i2sHandle);

/*! @} */

#if defined(__cplusplus)
}
#endif /*_cplusplus*/

/*! @} */

#endif /* _FSL_FLEXIO_I2S_H_ */
