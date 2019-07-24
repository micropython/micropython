/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_SAI_H_
#define _FSL_SAI_H_

#include "fsl_common.h"

/*!
 * @addtogroup sai_driver
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
/*@{*/
#define FSL_SAI_DRIVER_VERSION (MAKE_VERSION(2, 2, 1)) /*!< Version 2.2.1 */
/*@}*/

/*! @brief SAI return status*/
enum _sai_status_t
{
    kStatus_SAI_TxBusy    = MAKE_STATUS(kStatusGroup_SAI, 0), /*!< SAI Tx is busy. */
    kStatus_SAI_RxBusy    = MAKE_STATUS(kStatusGroup_SAI, 1), /*!< SAI Rx is busy. */
    kStatus_SAI_TxError   = MAKE_STATUS(kStatusGroup_SAI, 2), /*!< SAI Tx FIFO error. */
    kStatus_SAI_RxError   = MAKE_STATUS(kStatusGroup_SAI, 3), /*!< SAI Rx FIFO error. */
    kStatus_SAI_QueueFull = MAKE_STATUS(kStatusGroup_SAI, 4), /*!< SAI transfer queue is full. */
    kStatus_SAI_TxIdle    = MAKE_STATUS(kStatusGroup_SAI, 5), /*!< SAI Tx is idle */
    kStatus_SAI_RxIdle    = MAKE_STATUS(kStatusGroup_SAI, 6)  /*!< SAI Rx is idle */
};

/*< sai channel mask value, actual channel numbers is depend soc specific */
enum _sai_channel_mask
{
    kSAI_Channel0Mask = 1 << 0U, /*!< channel 0 mask value */
    kSAI_Channel1Mask = 1 << 1U, /*!< channel 1 mask value */
    kSAI_Channel2Mask = 1 << 2U, /*!< channel 2 mask value */
    kSAI_Channel3Mask = 1 << 3U, /*!< channel 3 mask value */
    kSAI_Channel4Mask = 1 << 4U, /*!< channel 4 mask value */
    kSAI_Channel5Mask = 1 << 5U, /*!< channel 5 mask value */
    kSAI_Channel6Mask = 1 << 6U, /*!< channel 6 mask value */
    kSAI_Channel7Mask = 1 << 7U, /*!< channel 7 mask value */
};

/*! @brief Define the SAI bus type */
typedef enum _sai_protocol
{
    kSAI_BusLeftJustified = 0x0U, /*!< Uses left justified format.*/
    kSAI_BusRightJustified,       /*!< Uses right justified format. */
    kSAI_BusI2S,                  /*!< Uses I2S format. */
    kSAI_BusPCMA,                 /*!< Uses I2S PCM A format.*/
    kSAI_BusPCMB                  /*!< Uses I2S PCM B format. */
} sai_protocol_t;

/*! @brief Master or slave mode */
typedef enum _sai_master_slave
{
    kSAI_Master                      = 0x0U, /*!< Master mode include bclk and frame sync */
    kSAI_Slave                       = 0x1U, /*!< Slave mode  include bclk and frame sync */
    kSAI_Bclk_Master_FrameSync_Slave = 0x2U, /*!< bclk in master mode, frame sync in slave mode */
    kSAI_Bclk_Slave_FrameSync_Master = 0x3U, /*!< bclk in slave mode, frame sync in master mode */
} sai_master_slave_t;

/*! @brief Mono or stereo audio format */
typedef enum _sai_mono_stereo
{
    kSAI_Stereo = 0x0U, /*!< Stereo sound. */
    kSAI_MonoRight,     /*!< Only Right channel have sound. */
    kSAI_MonoLeft       /*!< Only left channel have sound. */
} sai_mono_stereo_t;

/*! @brief SAI data order, MSB or LSB */
typedef enum _sai_data_order
{
    kSAI_DataLSB = 0x0U, /*!< LSB bit transferred first */
    kSAI_DataMSB         /*!< MSB bit transferred first */
} sai_data_order_t;

/*! @brief SAI clock polarity, active high or low */
typedef enum _sai_clock_polarity
{
    kSAI_PolarityActiveHigh = 0x0U,  /*!< Drive outputs on rising edge */
    kSAI_PolarityActiveLow,          /*!< Drive outputs on falling edge */
    kSAI_SampleOnFallingEdge = 0x0U, /*!< Sample inputs on falling edge */
    kSAI_SampleOnRisingEdge          /*!< Sample inputs on rising edge */
} sai_clock_polarity_t;

/*! @brief Synchronous or asynchronous mode */
typedef enum _sai_sync_mode
{
    kSAI_ModeAsync = 0x0U, /*!< Asynchronous mode */
    kSAI_ModeSync,         /*!< Synchronous mode (with receiver or transmit) */
#if defined(FSL_FEATURE_SAI_HAS_SYNC_WITH_ANOTHER_SAI) && (FSL_FEATURE_SAI_HAS_SYNC_WITH_ANOTHER_SAI)
    kSAI_ModeSyncWithOtherTx, /*!< Synchronous with another SAI transmit */
    kSAI_ModeSyncWithOtherRx  /*!< Synchronous with another SAI receiver */
#endif                        /* FSL_FEATURE_SAI_HAS_SYNC_WITH_ANOTHER_SAI */
} sai_sync_mode_t;

#if !(defined(FSL_FEATURE_SAI_HAS_NO_MCR_MICS) && (FSL_FEATURE_SAI_HAS_NO_MCR_MICS))
/*! @brief Mater clock source */
typedef enum _sai_mclk_source
{
    kSAI_MclkSourceSysclk = 0x0U, /*!< Master clock from the system clock */
    kSAI_MclkSourceSelect1,       /*!< Master clock from source 1 */
    kSAI_MclkSourceSelect2,       /*!< Master clock from source 2 */
    kSAI_MclkSourceSelect3        /*!< Master clock from source 3 */
} sai_mclk_source_t;
#endif

/*! @brief Bit clock source */
typedef enum _sai_bclk_source
{
    kSAI_BclkSourceBusclk = 0x0U, /*!< Bit clock using bus clock */
    /* General device bit source definition */
    kSAI_BclkSourceMclkOption1 = 0x1U, /*!< Bit clock MCLK option 1 */
    kSAI_BclkSourceMclkOption2 = 0x2U, /*!< Bit clock MCLK option2  */
    kSAI_BclkSourceMclkOption3 = 0x3U, /*!< Bit clock MCLK option3 */
    /* Kinetis device bit clock source definition */
    kSAI_BclkSourceMclkDiv   = 0x1U, /*!< Bit clock using master clock divider */
    kSAI_BclkSourceOtherSai0 = 0x2U, /*!< Bit clock from other SAI device  */
    kSAI_BclkSourceOtherSai1 = 0x3U  /*!< Bit clock from other SAI device */
} sai_bclk_source_t;

/*! @brief The SAI interrupt enable flag */
enum _sai_interrupt_enable_t
{
    kSAI_WordStartInterruptEnable =
        I2S_TCSR_WSIE_MASK, /*!< Word start flag, means the first word in a frame detected */
    kSAI_SyncErrorInterruptEnable   = I2S_TCSR_SEIE_MASK, /*!< Sync error flag, means the sync error is detected */
    kSAI_FIFOWarningInterruptEnable = I2S_TCSR_FWIE_MASK, /*!< FIFO warning flag, means the FIFO is empty */
    kSAI_FIFOErrorInterruptEnable   = I2S_TCSR_FEIE_MASK, /*!< FIFO error flag */
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    kSAI_FIFORequestInterruptEnable = I2S_TCSR_FRIE_MASK, /*!< FIFO request, means reached watermark */
#endif                                                    /* FSL_FEATURE_SAI_FIFO_COUNT */
};

/*! @brief The DMA request sources */
enum _sai_dma_enable_t
{
    kSAI_FIFOWarningDMAEnable = I2S_TCSR_FWDE_MASK, /*!< FIFO warning caused by the DMA request */
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    kSAI_FIFORequestDMAEnable = I2S_TCSR_FRDE_MASK, /*!< FIFO request caused by the DMA request */
#endif                                              /* FSL_FEATURE_SAI_FIFO_COUNT */
};

/*! @brief The SAI status flag */
enum _sai_flags
{
    kSAI_WordStartFlag = I2S_TCSR_WSF_MASK, /*!< Word start flag, means the first word in a frame detected */
    kSAI_SyncErrorFlag = I2S_TCSR_SEF_MASK, /*!< Sync error flag, means the sync error is detected */
    kSAI_FIFOErrorFlag = I2S_TCSR_FEF_MASK, /*!< FIFO error flag */
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    kSAI_FIFORequestFlag = I2S_TCSR_FRF_MASK, /*!< FIFO request flag. */
#endif                                        /* FSL_FEATURE_SAI_FIFO_COUNT */
    kSAI_FIFOWarningFlag = I2S_TCSR_FWF_MASK, /*!< FIFO warning flag */
};

/*! @brief The reset type */
typedef enum _sai_reset_type
{
    kSAI_ResetTypeSoftware = I2S_TCSR_SR_MASK, /*!< Software reset, reset the logic state */
    kSAI_ResetTypeFIFO     = I2S_TCSR_FR_MASK, /*!< FIFO reset, reset the FIFO read and write pointer */
    kSAI_ResetAll          = I2S_TCSR_SR_MASK | I2S_TCSR_FR_MASK /*!< All reset. */
} sai_reset_type_t;

#if defined(FSL_FEATURE_SAI_HAS_FIFO_PACKING) && FSL_FEATURE_SAI_HAS_FIFO_PACKING
/*!
 * @brief The SAI packing mode
 * The mode includes 8 bit and 16 bit packing.
 */
typedef enum _sai_fifo_packing
{
    kSAI_FifoPackingDisabled = 0x0U, /*!< Packing disabled */
    kSAI_FifoPacking8bit     = 0x2U, /*!< 8 bit packing enabled */
    kSAI_FifoPacking16bit    = 0x3U  /*!< 16bit packing enabled */
} sai_fifo_packing_t;
#endif /* FSL_FEATURE_SAI_HAS_FIFO_PACKING */

/*! @brief SAI user configuration structure */
typedef struct _sai_config
{
    sai_protocol_t protocol;  /*!< Audio bus protocol in SAI */
    sai_sync_mode_t syncMode; /*!< SAI sync mode, control Tx/Rx clock sync */
#if defined(FSL_FEATURE_SAI_HAS_MCR) && (FSL_FEATURE_SAI_HAS_MCR)
    bool mclkOutputEnable; /*!< Master clock output enable, true means master clock divider enabled */
#if !(defined(FSL_FEATURE_SAI_HAS_NO_MCR_MICS) && (FSL_FEATURE_SAI_HAS_NO_MCR_MICS))
    sai_mclk_source_t mclkSource; /*!< Master Clock source */
#endif                            /* FSL_FEATURE_SAI_HAS_MCR */
#endif
    sai_bclk_source_t bclkSource;   /*!< Bit Clock source */
    sai_master_slave_t masterSlave; /*!< Master or slave */
} sai_config_t;

#ifndef SAI_XFER_QUEUE_SIZE
/*!@brief SAI transfer queue size, user can refine it according to use case. */
#define SAI_XFER_QUEUE_SIZE (4)
#endif

/*! @brief Audio sample rate */
typedef enum _sai_sample_rate
{
    kSAI_SampleRate8KHz    = 8000U,   /*!< Sample rate 8000 Hz */
    kSAI_SampleRate11025Hz = 11025U,  /*!< Sample rate 11025 Hz */
    kSAI_SampleRate12KHz   = 12000U,  /*!< Sample rate 12000 Hz */
    kSAI_SampleRate16KHz   = 16000U,  /*!< Sample rate 16000 Hz */
    kSAI_SampleRate22050Hz = 22050U,  /*!< Sample rate 22050 Hz */
    kSAI_SampleRate24KHz   = 24000U,  /*!< Sample rate 24000 Hz */
    kSAI_SampleRate32KHz   = 32000U,  /*!< Sample rate 32000 Hz */
    kSAI_SampleRate44100Hz = 44100U,  /*!< Sample rate 44100 Hz */
    kSAI_SampleRate48KHz   = 48000U,  /*!< Sample rate 48000 Hz */
    kSAI_SampleRate96KHz   = 96000U,  /*!< Sample rate 96000 Hz */
    kSAI_SampleRate192KHz  = 192000U, /*!< Sample rate 192000 Hz */
    kSAI_SampleRate384KHz  = 384000U, /*!< Sample rate 384000 Hz */
} sai_sample_rate_t;

/*! @brief Audio word width */
typedef enum _sai_word_width
{
    kSAI_WordWidth8bits  = 8U,  /*!< Audio data width 8 bits */
    kSAI_WordWidth16bits = 16U, /*!< Audio data width 16 bits */
    kSAI_WordWidth24bits = 24U, /*!< Audio data width 24 bits */
    kSAI_WordWidth32bits = 32U  /*!< Audio data width 32 bits */
} sai_word_width_t;

#if defined(FSL_FEATURE_SAI_HAS_CHANNEL_MODE) && FSL_FEATURE_SAI_HAS_CHANNEL_MODE
/*! @brief sai data pin state definition */
typedef enum _sai_data_pin_state
{
    kSAI_DataPinStateTriState =
        0U, /*!< transmit data pins are tri-stated when slots are masked or channels are disabled */
    kSAI_DataPinStateOutputZero = 1U, /*!< transmit data pins are never tri-stated and will output zero when slots
                                             are masked or channel disabled */
} sai_data_pin_state_t;
#endif

#if defined(FSL_FEATURE_SAI_HAS_FIFO_FUNCTION_COMBINE) && FSL_FEATURE_SAI_HAS_FIFO_FUNCTION_COMBINE
/*! @brief sai fifo combine mode definition */
typedef enum _sai_fifo_combine
{
    kSAI_FifoCombineDisabled = 0U,          /*!< sai fifo combine mode disabled */
    kSAI_FifoCombineModeEnabledOnRead,      /*!< sai fifo combine mode enabled on FIFO reads */
    kSAI_FifoCombineModeEnabledOnWrite,     /*!< sai fifo combine mode enabled on FIFO write */
    kSAI_FifoCombineModeEnabledOnReadWrite, /*!< sai fifo combined mode enabled on FIFO read/writes */
} sai_fifo_combine_t;
#endif

/*! @brief sai transceiver type */
typedef enum _sai_transceiver_type
{
    kSAI_Transmitter = 0U, /*!< sai transmitter */
    kSAI_Receiver    = 1U, /*!< sai receiver */
} sai_transceiver_type_t;

/*! @brief sai frame sync len */
typedef enum _sai_frame_sync_len
{
    kSAI_FrameSyncLenOneBitClk    = 0U, /*!< 1 bit clock frame sync len for DSP mode */
    kSAI_FrameSyncLenPerWordWidth = 1U, /*!< Frame sync length decided by word width */
} sai_frame_sync_len_t;

/*! @brief sai transfer format */
typedef struct _sai_transfer_format
{
    uint32_t sampleRate_Hz;   /*!< Sample rate of audio data */
    uint32_t bitWidth;        /*!< Data length of audio data, usually 8/16/24/32 bits */
    sai_mono_stereo_t stereo; /*!< Mono or stereo */
#if defined(FSL_FEATURE_SAI_HAS_MCLKDIV_REGISTER) && (FSL_FEATURE_SAI_HAS_MCLKDIV_REGISTER)
    uint32_t masterClockHz; /*!< Master clock frequency in Hz */
#endif                      /* FSL_FEATURE_SAI_HAS_MCLKDIV_REGISTER */
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    uint8_t watermark; /*!< Watermark value */
#endif                 /* FSL_FEATURE_SAI_FIFO_COUNT */

    /* for the multi channel usage, user can provide channelMask Oonly, then sai driver will handle
     * other parameter carefully, such as
     * channelMask = kSAI_Channel0Mask | kSAI_Channel1Mask | kSAI_Channel4Mask
     * then in SAI_RxSetFormat/SAI_TxSetFormat function, channel/endChannel/channelNums will be calculated.
     * for the single channel usage, user can provide channel or channel mask only, such as,
     * channel = 0 or channelMask = kSAI_Channel0Mask.
     */
    uint8_t channel;     /*!< Transfer start channel */
    uint8_t channelMask; /*!< enabled channel mask value, reference _sai_channel_mask */
    uint8_t endChannel;  /*!< end channel number */
    uint8_t channelNums; /*!< Total enabled channel numbers */

    sai_protocol_t protocol; /*!< Which audio protocol used */
    bool isFrameSyncCompact; /*!< True means Frame sync length is configurable according to bitWidth, false means frame
                                sync length is 64 times of bit clock. */
} sai_transfer_format_t;

#if (defined(FSL_FEATURE_SAI_HAS_MCR) && (FSL_FEATURE_SAI_HAS_MCR)) || \
    (defined(FSL_FEATURE_SAI_HAS_MCLKDIV_REGISTER) && (FSL_FEATURE_SAI_HAS_MCLKDIV_REGISTER))
/*! @brief master clock configurations */
typedef struct _sai_master_clock
{
#if defined(FSL_FEATURE_SAI_HAS_MCR) && (FSL_FEATURE_SAI_HAS_MCR)
    bool mclkOutputEnable; /*!< master clock output enable */
#if !(defined(FSL_FEATURE_SAI_HAS_NO_MCR_MICS) && (FSL_FEATURE_SAI_HAS_NO_MCR_MICS))
    sai_mclk_source_t mclkSource; /*!< Master Clock source */
#endif
#endif

#if (defined(FSL_FEATURE_SAI_HAS_MCR) && (FSL_FEATURE_SAI_HAS_MCR)) || \
    (defined(FSL_FEATURE_SAI_HAS_MCLKDIV_REGISTER) && (FSL_FEATURE_SAI_HAS_MCLKDIV_REGISTER))
    uint32_t mclkHz;          /*!< target mclk frequency */
    uint32_t mclkSourceClkHz; /*!< mclk source frequency*/
#endif
} sai_master_clock_t;
#endif

/*! @brief sai fifo configurations */
typedef struct _sai_fifo
{
#if defined(FSL_FEATURE_SAI_HAS_FIFO_FUNCTION_AFTER_ERROR) && FSL_FEATURE_SAI_HAS_FIFO_FUNCTION_AFTER_ERROR
    bool fifoContinueOneError; /*!< fifo continues when error occur */
#endif

#if defined(FSL_FEATURE_SAI_HAS_FIFO_FUNCTION_COMBINE) && FSL_FEATURE_SAI_HAS_FIFO_FUNCTION_COMBINE
    sai_fifo_combine_t fifoCombine; /*!< fifo combine mode */
#endif

#if defined(FSL_FEATURE_SAI_HAS_FIFO_PACKING) && FSL_FEATURE_SAI_HAS_FIFO_PACKING
    sai_fifo_packing_t fifoPacking; /*!< fifo packing mode */
#endif
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    uint8_t fifoWatermark; /*!< fifo watermark */
#endif
} sai_fifo_t;

/*! @brief sai bit clock configurations */
typedef struct _sai_bit_clock
{
    bool bclkSrcSwap;    /*!< bit clock source swap */
    bool bclkInputDelay; /*!< bit clock actually used by the transmitter is delayed by the pad output delay,
                           this has effect of decreasing the data input setup time, but increasing the data output valid
                           time .*/
    sai_clock_polarity_t bclkPolarity; /*!< bit clock polarity */
    sai_bclk_source_t bclkSource;      /*!< bit Clock source */
} sai_bit_clock_t;

/*! @brief sai frame sync configurations */
typedef struct _sai_frame_sync
{
    uint8_t frameSyncWidth; /*!< frame sync width in number of bit clocks */
    bool frameSyncEarly;    /*!< TRUE is frame sync assert one bit before the first bit of frame
                                FALSE is frame sync assert with the first bit of the frame */

#if defined(FSL_FEATURE_SAI_HAS_FRAME_SYNC_ON_DEMAND) && FSL_FEATURE_SAI_HAS_FRAME_SYNC_ON_DEMAND
    bool frameSyncGenerateOnDemand; /*!< internal frame sync is generated when FIFO waring flag is clear */
#endif

    sai_clock_polarity_t frameSyncPolarity; /*!< frame sync polarity */

} sai_frame_sync_t;

/*! @brief sai serial data configurations */
typedef struct _sai_serial_data
{
#if defined(FSL_FEATURE_SAI_HAS_CHANNEL_MODE) && FSL_FEATURE_SAI_HAS_CHANNEL_MODE
    sai_data_pin_state_t dataMode; /*!< sai data pin state when slots masked or channel disabled */
#endif

    sai_data_order_t dataOrder; /*!< configure whether the LSB or MSB is transmitted first */
    uint8_t dataWord0Length;    /*!< configure the number of bits in the first word in each frame */
    uint8_t dataWordNLength; /*!< configure the number of bits in the each word in each frame, except the first word */
    uint8_t dataWordLength;  /*!< used to record the data length for dma transfer */
    uint8_t
        dataFirstBitShifted; /*!< Configure the bit index for the first bit transmitted for each word in the frame */
    uint8_t dataWordNum;     /*!< configure the number of words in each frame */
    uint32_t dataMaskedWord; /*!< configure whether the transmit word is masked */
} sai_serial_data_t;

/*! @brief sai transceiver configurations */
typedef struct _sai_transceiver
{
    sai_serial_data_t serialData; /*!< serial data configurations */
    sai_frame_sync_t frameSync;   /*!< ws configurations */
    sai_bit_clock_t bitClock;     /*!< bit clock configurations */
    sai_fifo_t fifo;              /*!< fifo configurations */

    sai_master_slave_t masterSlave; /*!< transceiver is master or slave */

    sai_sync_mode_t syncMode; /*!< transceiver sync mode */

    uint8_t startChannel; /*!< Transfer start channel */
    uint8_t channelMask;  /*!< enabled channel mask value, reference _sai_channel_mask */
    uint8_t endChannel;   /*!< end channel number */
    uint8_t channelNums;  /*!< Total enabled channel numbers */

} sai_transceiver_t;

/*! @brief SAI transfer structure */
typedef struct _sai_transfer
{
    uint8_t *data;   /*!< Data start address to transfer. */
    size_t dataSize; /*!< Transfer size. */
} sai_transfer_t;

typedef struct _sai_handle sai_handle_t;

/*! @brief SAI transfer callback prototype */
typedef void (*sai_transfer_callback_t)(I2S_Type *base, sai_handle_t *handle, status_t status, void *userData);

/*! @brief SAI handle structure */
struct _sai_handle
{
    I2S_Type *base; /*!< base address */

    uint32_t state;                   /*!< Transfer status */
    sai_transfer_callback_t callback; /*!< Callback function called at transfer event*/
    void *userData;                   /*!< Callback parameter passed to callback function*/
    uint8_t bitWidth;                 /*!< Bit width for transfer, 8/16/24/32 bits */

    /* for the multi channel usage, user can provide channelMask Oonly, then sai driver will handle
     * other parameter carefully, such as
     * channelMask = kSAI_Channel0Mask | kSAI_Channel1Mask | kSAI_Channel4Mask
     * then in SAI_RxSetFormat/SAI_TxSetFormat function, channel/endChannel/channelNums will be calculated.
     * for the single channel usage, user can provide channel or channel mask only, such as,
     * channel = 0 or channelMask = kSAI_Channel0Mask.
     */
    uint8_t channel;     /*!< Transfer start channel */
    uint8_t channelMask; /*!< enabled channel mask value, refernece _sai_channel_mask */
    uint8_t endChannel;  /*!< end channel number */
    uint8_t channelNums; /*!< Total enabled channel numbers */

    sai_transfer_t saiQueue[SAI_XFER_QUEUE_SIZE]; /*!< Transfer queue storing queued transfer */
    size_t transferSize[SAI_XFER_QUEUE_SIZE];     /*!< Data bytes need to transfer */
    volatile uint8_t queueUser;                   /*!< Index for user to queue transfer */
    volatile uint8_t queueDriver;                 /*!< Index for driver to get the transfer data and size */
#if defined(FSL_FEATURE_SAI_FIFO_COUNT) && (FSL_FEATURE_SAI_FIFO_COUNT > 1)
    uint8_t watermark; /*!< Watermark value */
#endif
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
 * @brief Initializes the SAI Tx peripheral.
 * @deprecated Do not use this function.  It has been superceded by @ref SAI_Init
 *
 * Ungates the SAI clock, resets the module, and configures SAI Tx with a configuration structure.
 * The configuration structure can be custom filled or set with default values by
 * SAI_TxGetDefaultConfig().
 *
 * @note  This API should be called at the beginning of the application to use
 * the SAI driver. Otherwise, accessing the SAIM module can cause a hard fault
 * because the clock is not enabled.
 *
 * @param base SAI base pointer
 * @param config SAI configuration structure.
 */
void SAI_TxInit(I2S_Type *base, const sai_config_t *config);

/*!
 * @brief Initializes the SAI Rx peripheral.
 * @deprecated Do not use this function.  It has been superceded by @ref SAI_Init
 *
 * Ungates the SAI clock, resets the module, and configures the SAI Rx with a configuration structure.
 * The configuration structure can be custom filled or set with default values by
 * SAI_RxGetDefaultConfig().
 *
 * @note  This API should be called at the beginning of the application to use
 * the SAI driver. Otherwise, accessing the SAI module can cause a hard fault
 * because the clock is not enabled.
 *
 * @param base SAI base pointer
 * @param config SAI configuration structure.
 */
void SAI_RxInit(I2S_Type *base, const sai_config_t *config);

/*!
 * @brief  Sets the SAI Tx configuration structure to default values.
 * @deprecated Do not use this function.  It has been superceded by @ref
 * SAI_GetClassicI2SConfig/SAI_GetLeftJustifiedConfig/SAI_GetRightJustifiedConfig/SAI_GetDSPConfig/SAI_GetTDMConfig
 *
 * This API initializes the configuration structure for use in SAI_TxConfig().
 * The initialized structure can remain unchanged in SAI_TxConfig(), or it can be modified
 *  before calling SAI_TxConfig().
 * This is an example.
   @code
   sai_config_t config;
   SAI_TxGetDefaultConfig(&config);
   @endcode
 *
 * @param config pointer to master configuration structure
 */
void SAI_TxGetDefaultConfig(sai_config_t *config);

/*!
 * @brief  Sets the SAI Rx configuration structure to default values.
 * @deprecated Do not use this function.  It has been superceded by @ref
 * SAI_GetClassicI2SConfig/SAI_GetLeftJustifiedConfig/SAI_GetRightJustifiedConfig/SAI_GetDSPConfig/SAI_GetTDMConfig
 *
 * This API initializes the configuration structure for use in SAI_RxConfig().
 * The initialized structure can remain unchanged in SAI_RxConfig() or it can be modified
 *  before calling SAI_RxConfig().
 * This is an example.
   @code
   sai_config_t config;
   SAI_RxGetDefaultConfig(&config);
   @endcode
 *
 * @param config pointer to master configuration structure
 */
void SAI_RxGetDefaultConfig(sai_config_t *config);

/*!
 * @brief Initializes the SAI peripheral.
 *
 * This API gates the SAI clock. The SAI module can't operate unless SAI_Init is called to enable the clock.
 *
 * @param base SAI base pointer.
 */
void SAI_Init(I2S_Type *base);

/*!
 * @brief De-initializes the SAI peripheral.
 *
 * This API gates the SAI clock. The SAI module can't operate unless SAI_TxInit
 * or SAI_RxInit is called to enable the clock.
 *
 * @param base SAI base pointer.
 */
void SAI_Deinit(I2S_Type *base);

/*!
 * @brief Resets the SAI Tx.
 *
 * This function enables the software reset and FIFO reset of SAI Tx. After reset, clear the reset bit.
 *
 * @param base SAI base pointer
 */
void SAI_TxReset(I2S_Type *base);

/*!
 * @brief Resets the SAI Rx.
 *
 * This function enables the software reset and FIFO reset of SAI Rx. After reset, clear the reset bit.
 *
 * @param base SAI base pointer
 */
void SAI_RxReset(I2S_Type *base);

/*!
 * @brief Enables/disables the SAI Tx.
 *
 * @param base SAI base pointer.
 * @param enable True means enable SAI Tx, false means disable.
 */
void SAI_TxEnable(I2S_Type *base, bool enable);

/*!
 * @brief Enables/disables the SAI Rx.
 *
 * @param base SAI base pointer.
 * @param enable True means enable SAI Rx, false means disable.
 */
void SAI_RxEnable(I2S_Type *base, bool enable);

/*!
 * @brief Set Rx bit clock direction.
 *
 * Select bit clock direction, master or slave.
 *
 * @param base SAI base pointer.
 * @param masterSlave reference sai_master_slave_t.
 */
static inline void SAI_TxSetBitClockDirection(I2S_Type *base, sai_master_slave_t masterSlave)
{
    if (masterSlave == kSAI_Master)
    {
        base->TCR2 |= I2S_TCR2_BCD_MASK;
    }
    else
    {
        base->TCR2 &= ~I2S_TCR2_BCD_MASK;
    }
}

/*!
 * @brief Set Rx bit clock direction.
 *
 * Select bit clock direction, master or slave.
 *
 * @param base SAI base pointer.
 * @param masterSlave reference sai_master_slave_t.
 */
static inline void SAI_RxSetBitClockDirection(I2S_Type *base, sai_master_slave_t masterSlave)
{
    if (masterSlave == kSAI_Master)
    {
        base->RCR2 |= I2S_RCR2_BCD_MASK;
    }
    else
    {
        base->RCR2 &= ~I2S_RCR2_BCD_MASK;
    }
}

/*!
 * @brief Set Rx frame sync direction.
 *
 * Select frame sync direction, master or slave.
 *
 * @param base SAI base pointer.
 * @param masterSlave reference sai_master_slave_t.
 */
static inline void SAI_RxSetFrameSyncDirection(I2S_Type *base, sai_master_slave_t masterSlave)
{
    if (masterSlave == kSAI_Master)
    {
        base->RCR4 |= I2S_RCR4_FSD_MASK;
    }
    else
    {
        base->RCR4 &= ~I2S_RCR4_FSD_MASK;
    }
}

/*!
 * @brief Set Tx frame sync direction.
 *
 * Select frame sync direction, master or slave.
 *
 * @param base SAI base pointer.
 * @param masterSlave reference sai_master_slave_t.
 */
static inline void SAI_TxSetFrameSyncDirection(I2S_Type *base, sai_master_slave_t masterSlave)
{
    if (masterSlave == kSAI_Master)
    {
        base->TCR4 |= I2S_TCR4_FSD_MASK;
    }
    else
    {
        base->TCR4 &= ~I2S_TCR4_FSD_MASK;
    }
}

/*!
 * @brief Transmitter bit clock rate configurations.
 *
 * @param base SAI base pointer.
 * @param sourceClockHz, bit clock source frequency.
 * @param sampleRate audio data sample rate.
 * @param bitWidth, audio data bitWidth.
 * @param channelNumbers, audio channel numbers.
 */
void SAI_TxSetBitClockRate(
    I2S_Type *base, uint32_t sourceClockHz, uint32_t sampleRate, uint32_t bitWidth, uint32_t channelNumbers);

/*!
 * @brief Receiver bit clock rate configurations.
 *
 * @param base SAI base pointer.
 * @param sourceClockHz, bit clock source frequency.
 * @param sampleRate audio data sample rate.
 * @param bitWidth, audio data bitWidth.
 * @param channelNumbers, audio channel numbers.
 */
void SAI_RxSetBitClockRate(
    I2S_Type *base, uint32_t sourceClockHz, uint32_t sampleRate, uint32_t bitWidth, uint32_t channelNumbers);

/*!
 * @brief Transmitter Bit clock configurations.
 *
 * @param base SAI base pointer.
 * @param masterSlave master or slave.
 * @param config bit clock other configurations, can be NULL in slave mode.
 */
void SAI_TxSetBitclockConfig(I2S_Type *base, sai_master_slave_t masterSlave, sai_bit_clock_t *config);

/*!
 * @brief Receiver Bit clock configurations.
 *
 * @param base SAI base pointer.
 * @param masterSlave master or slave.
 * @param config bit clock other configurations, can be NULL in slave mode.
 */
void SAI_RxSetBitclockConfig(I2S_Type *base, sai_master_slave_t masterSlave, sai_bit_clock_t *config);

#if (defined(FSL_FEATURE_SAI_HAS_MCR) && (FSL_FEATURE_SAI_HAS_MCR)) || \
    (defined(FSL_FEATURE_SAI_HAS_MCLKDIV_REGISTER) && (FSL_FEATURE_SAI_HAS_MCLKDIV_REGISTER))
/*!
 * @brief Master clock configurations.
 *
 * @param base SAI base pointer.
 * @param config master clock configurations.
 */
void SAI_SetMasterClockConfig(I2S_Type *base, sai_master_clock_t *config);
#endif

/*!
 * @brief SAI transmitter fifo configurations.
 *
 * @param base SAI base pointer.
 * @param config fifo configurations.
 */
void SAI_TxSetFifoConfig(I2S_Type *base, sai_fifo_t *config);

/*!
 * @brief SAI receiver fifo configurations.
 *
 * @param base SAI base pointer.
 * @param config fifo configurations.
 */
void SAI_RxSetFifoConfig(I2S_Type *base, sai_fifo_t *config);

/*!
 * @brief SAI transmitter Frame sync configurations.
 *
 * @param base SAI base pointer.
 * @param masterSlave master or slave.
 * @param config frame sync configurations, can be NULL in slave mode.
 */
void SAI_TxSetFrameSyncConfig(I2S_Type *base, sai_master_slave_t masterSlave, sai_frame_sync_t *config);

/*!
 * @brief SAI receiver Frame sync configurations.
 *
 * @param base SAI base pointer.
 * @param masterSlave master or slave.
 * @param config frame sync configurations, can be NULL in slave mode.
 */
void SAI_RxSetFrameSyncConfig(I2S_Type *base, sai_master_slave_t masterSlave, sai_frame_sync_t *config);

/*!
 * @brief SAI transmitter Serial data configurations.
 *
 * @param base SAI base pointer.
 * @param config serial data configurations.
 */
void SAI_TxSetSerialDataConfig(I2S_Type *base, sai_serial_data_t *config);

/*!
 * @brief SAI receiver Serial data configurations.
 *
 * @param base SAI base pointer.
 * @param config serial data configurations.
 */
void SAI_RxSetSerialDataConfig(I2S_Type *base, sai_serial_data_t *config);

/*!
 * @brief SAI transmitter configurations.
 *
 * @param base SAI base pointer.
 * @param config transmitter configurations.
 */
void SAI_TxSetConfig(I2S_Type *base, sai_transceiver_t *config);

/*!
 * @brief SAI receiver configurations.
 *
 * @param base SAI base pointer.
 * @param config receiver configurations.
 */
void SAI_RxSetConfig(I2S_Type *base, sai_transceiver_t *config);

/*!
 * @brief Get classic I2S mode configurations.
 *
 * @param config transceiver configurations.
 * @param bitWidth audio data bitWidth.
 * @param mode audio data channel.
 * @param saiChannelMask mask value of the channel to be enable.
 */
void SAI_GetClassicI2SConfig(sai_transceiver_t *config,
                             sai_word_width_t bitWidth,
                             sai_mono_stereo_t mode,
                             uint32_t saiChannelMask);

/*!
 * @brief Get left justified mode configurations.
 *
 * @param config transceiver configurations.
 * @param bitWidth audio data bitWidth.
 * @param mode audio data channel.
 * @param saiChannelMask mask value of the channel to be enable.
 */
void SAI_GetLeftJustifiedConfig(sai_transceiver_t *config,
                                sai_word_width_t bitWidth,
                                sai_mono_stereo_t mode,
                                uint32_t saiChannelMask);

/*!
 * @brief Get right justified mode configurations.
 *
 * @param config transceiver configurations.
 * @param bitWidth audio data bitWidth.
 * @param mode audio data channel.
 * @param saiChannelMask mask value of the channel to be enable.
 */
void SAI_GetRightJustifiedConfig(sai_transceiver_t *config,
                                 sai_word_width_t bitWidth,
                                 sai_mono_stereo_t mode,
                                 uint32_t saiChannelMask);

/*!
 * @brief Get TDM mode configurations.
 *
 * @param config transceiver configurations.
 * @param frameSyncWidth length of frame sync.
 * @param bitWidth audio data word width.
 * @param dataWordNum word number in one frame.
 * @param saiChannelMask mask value of the channel to be enable.
 */
void SAI_GetTDMConfig(sai_transceiver_t *config,
                      sai_frame_sync_len_t frameSyncWidth,
                      sai_word_width_t bitWidth,
                      uint32_t dataWordNum,
                      uint32_t saiChannelMask);

/*!
 * @brief Get DSP mode configurations.
 *
 * @param config transceiver configurations.
 * @param frameSyncWidth length of frame sync.
 * @param bitWidth audio data bitWidth.
 * @param mode audio data channel.
 * @param saiChannelMask mask value of the channel to enable.
 */
void SAI_GetDSPConfig(sai_transceiver_t *config,
                      sai_frame_sync_len_t frameSyncWidth,
                      sai_word_width_t bitWidth,
                      sai_mono_stereo_t mode,
                      uint32_t saiChannelMask);
/*! @} */

/*!
 * @name Status
 * @{
 */

/*!
 * @brief Gets the SAI Tx status flag state.
 *
 * @param base SAI base pointer
 * @return SAI Tx status flag value. Use the Status Mask to get the status value needed.
 */
static inline uint32_t SAI_TxGetStatusFlag(I2S_Type *base)
{
    return base->TCSR;
}

/*!
 * @brief Clears the SAI Tx status flag state.
 *
 * @param base SAI base pointer
 * @param mask State mask. It can be a combination of the following source if defined:
 *        @arg kSAI_WordStartFlag
 *        @arg kSAI_SyncErrorFlag
 *        @arg kSAI_FIFOErrorFlag
 */
static inline void SAI_TxClearStatusFlags(I2S_Type *base, uint32_t mask)
{
    base->TCSR = ((base->TCSR & 0xFFE3FFFFU) | mask);
}

/*!
 * @brief Gets the SAI Tx status flag state.
 *
 * @param base SAI base pointer
 * @return SAI Rx status flag value. Use the Status Mask to get the status value needed.
 */
static inline uint32_t SAI_RxGetStatusFlag(I2S_Type *base)
{
    return base->RCSR;
}

/*!
 * @brief Clears the SAI Rx status flag state.
 *
 * @param base SAI base pointer
 * @param mask State mask. It can be a combination of the following sources if defined.
 *        @arg kSAI_WordStartFlag
 *        @arg kSAI_SyncErrorFlag
 *        @arg kSAI_FIFOErrorFlag
 */
static inline void SAI_RxClearStatusFlags(I2S_Type *base, uint32_t mask)
{
    base->RCSR = ((base->RCSR & 0xFFE3FFFFU) | mask);
}

/*!
 * @brief Do software reset or FIFO reset .
 *
 * FIFO reset means clear all the data in the FIFO, and make the FIFO pointer both to 0.
 * Software reset means clear the Tx internal logic, including the bit clock, frame count etc. But software
 * reset will not clear any configuration registers like TCR1~TCR5.
 * This function will also clear all the error flags such as FIFO error, sync error etc.
 *
 * @param base SAI base pointer
 * @param type Reset type, FIFO reset or software reset
 */
void SAI_TxSoftwareReset(I2S_Type *base, sai_reset_type_t type);

/*!
 * @brief Do software reset or FIFO reset .
 *
 * FIFO reset means clear all the data in the FIFO, and make the FIFO pointer both to 0.
 * Software reset means clear the Rx internal logic, including the bit clock, frame count etc. But software
 * reset will not clear any configuration registers like RCR1~RCR5.
 * This function will also clear all the error flags such as FIFO error, sync error etc.
 *
 * @param base SAI base pointer
 * @param type Reset type, FIFO reset or software reset
 */
void SAI_RxSoftwareReset(I2S_Type *base, sai_reset_type_t type);

/*!
 * @brief Set the Tx channel FIFO enable mask.
 *
 * @param base SAI base pointer
 * @param mask Channel enable mask, 0 means all channel FIFO disabled, 1 means channel 0 enabled,
 * 3 means both channel 0 and channel 1 enabled.
 */
void SAI_TxSetChannelFIFOMask(I2S_Type *base, uint8_t mask);

/*!
 * @brief Set the Rx channel FIFO enable mask.
 *
 * @param base SAI base pointer
 * @param mask Channel enable mask, 0 means all channel FIFO disabled, 1 means channel 0 enabled,
 * 3 means both channel 0 and channel 1 enabled.
 */
void SAI_RxSetChannelFIFOMask(I2S_Type *base, uint8_t mask);

/*!
 * @brief Set the Tx data order.
 *
 * @param base SAI base pointer
 * @param order Data order MSB or LSB
 */
void SAI_TxSetDataOrder(I2S_Type *base, sai_data_order_t order);

/*!
 * @brief Set the Rx data order.
 *
 * @param base SAI base pointer
 * @param order Data order MSB or LSB
 */
void SAI_RxSetDataOrder(I2S_Type *base, sai_data_order_t order);

/*!
 * @brief Set the Tx data order.
 *
 * @param base SAI base pointer
 * @param order Data order MSB or LSB
 */
void SAI_TxSetBitClockPolarity(I2S_Type *base, sai_clock_polarity_t polarity);

/*!
 * @brief Set the Rx data order.
 *
 * @param base SAI base pointer
 * @param order Data order MSB or LSB
 */
void SAI_RxSetBitClockPolarity(I2S_Type *base, sai_clock_polarity_t polarity);

/*!
 * @brief Set the Tx data order.
 *
 * @param base SAI base pointer
 * @param order Data order MSB or LSB
 */
void SAI_TxSetFrameSyncPolarity(I2S_Type *base, sai_clock_polarity_t polarity);

/*!
 * @brief Set the Rx data order.
 *
 * @param base SAI base pointer
 * @param order Data order MSB or LSB
 */
void SAI_RxSetFrameSyncPolarity(I2S_Type *base, sai_clock_polarity_t polarity);

#if defined(FSL_FEATURE_SAI_HAS_FIFO_PACKING) && FSL_FEATURE_SAI_HAS_FIFO_PACKING
/*!
 * @brief Set Tx FIFO packing feature.
 *
 * @param base SAI base pointer.
 * @param pack FIFO pack type. It is element of sai_fifo_packing_t.
 */
void SAI_TxSetFIFOPacking(I2S_Type *base, sai_fifo_packing_t pack);

/*!
 * @brief Set Rx FIFO packing feature.
 *
 * @param base SAI base pointer.
 * @param pack FIFO pack type. It is element of sai_fifo_packing_t.
 */
void SAI_RxSetFIFOPacking(I2S_Type *base, sai_fifo_packing_t pack);
#endif /* FSL_FEATURE_SAI_HAS_FIFO_PACKING */

#if defined(FSL_FEATURE_SAI_HAS_FIFO_FUNCTION_AFTER_ERROR) && FSL_FEATURE_SAI_HAS_FIFO_FUNCTION_AFTER_ERROR
/*!
 * @brief Set Tx FIFO error continue.
 *
 * FIFO error continue mode means SAI will keep running while FIFO error occurred. If this feature
 * not enabled, SAI will hang and users need to clear FEF flag in TCSR register.
 *
 * @param base SAI base pointer.
 * @param isEnabled Is FIFO error continue enabled, true means enable, false means disable.
 */
static inline void SAI_TxSetFIFOErrorContinue(I2S_Type *base, bool isEnabled)
{
    if (isEnabled)
    {
        base->TCR4 |= I2S_TCR4_FCONT_MASK;
    }
    else
    {
        base->TCR4 &= ~I2S_TCR4_FCONT_MASK;
    }
}

/*!
 * @brief Set Rx FIFO error continue.
 *
 * FIFO error continue mode means SAI will keep running while FIFO error occurred. If this feature
 * not enabled, SAI will hang and users need to clear FEF flag in RCSR register.
 *
 * @param base SAI base pointer.
 * @param isEnabled Is FIFO error continue enabled, true means enable, false means disable.
 */
static inline void SAI_RxSetFIFOErrorContinue(I2S_Type *base, bool isEnabled)
{
    if (isEnabled)
    {
        base->RCR4 |= I2S_RCR4_FCONT_MASK;
    }
    else
    {
        base->RCR4 &= ~I2S_RCR4_FCONT_MASK;
    }
}
#endif

/*! @} */

/*!
 * @name Interrupts
 * @{
 */

/*!
 * @brief Enables the SAI Tx interrupt requests.
 *
 * @param base SAI base pointer
 * @param mask interrupt source
 *     The parameter can be a combination of the following sources if defined.
 *     @arg kSAI_WordStartInterruptEnable
 *     @arg kSAI_SyncErrorInterruptEnable
 *     @arg kSAI_FIFOWarningInterruptEnable
 *     @arg kSAI_FIFORequestInterruptEnable
 *     @arg kSAI_FIFOErrorInterruptEnable
 */
static inline void SAI_TxEnableInterrupts(I2S_Type *base, uint32_t mask)
{
    base->TCSR = ((base->TCSR & 0xFFE3FFFFU) | mask);
}

/*!
 * @brief Enables the SAI Rx interrupt requests.
 *
 * @param base SAI base pointer
 * @param mask interrupt source
 *     The parameter can be a combination of the following sources if defined.
 *     @arg kSAI_WordStartInterruptEnable
 *     @arg kSAI_SyncErrorInterruptEnable
 *     @arg kSAI_FIFOWarningInterruptEnable
 *     @arg kSAI_FIFORequestInterruptEnable
 *     @arg kSAI_FIFOErrorInterruptEnable
 */
static inline void SAI_RxEnableInterrupts(I2S_Type *base, uint32_t mask)
{
    base->RCSR = ((base->RCSR & 0xFFE3FFFFU) | mask);
}

/*!
 * @brief Disables the SAI Tx interrupt requests.
 *
 * @param base SAI base pointer
 * @param mask interrupt source
 *     The parameter can be a combination of the following sources if defined.
 *     @arg kSAI_WordStartInterruptEnable
 *     @arg kSAI_SyncErrorInterruptEnable
 *     @arg kSAI_FIFOWarningInterruptEnable
 *     @arg kSAI_FIFORequestInterruptEnable
 *     @arg kSAI_FIFOErrorInterruptEnable
 */
static inline void SAI_TxDisableInterrupts(I2S_Type *base, uint32_t mask)
{
    base->TCSR = ((base->TCSR & 0xFFE3FFFFU) & (~mask));
}

/*!
 * @brief Disables the SAI Rx interrupt requests.
 *
 * @param base SAI base pointer
 * @param mask interrupt source
 *     The parameter can be a combination of the following sources if defined.
 *     @arg kSAI_WordStartInterruptEnable
 *     @arg kSAI_SyncErrorInterruptEnable
 *     @arg kSAI_FIFOWarningInterruptEnable
 *     @arg kSAI_FIFORequestInterruptEnable
 *     @arg kSAI_FIFOErrorInterruptEnable
 */
static inline void SAI_RxDisableInterrupts(I2S_Type *base, uint32_t mask)
{
    base->RCSR = ((base->RCSR & 0xFFE3FFFFU) & (~mask));
}

/*! @} */

/*!
 * @name DMA Control
 * @{
 */

/*!
 * @brief Enables/disables the SAI Tx DMA requests.
 * @param base SAI base pointer
 * @param mask DMA source
 *     The parameter can be combination of the following sources if defined.
 *     @arg kSAI_FIFOWarningDMAEnable
 *     @arg kSAI_FIFORequestDMAEnable
 * @param enable True means enable DMA, false means disable DMA.
 */
static inline void SAI_TxEnableDMA(I2S_Type *base, uint32_t mask, bool enable)
{
    if (enable)
    {
        base->TCSR = ((base->TCSR & 0xFFE3FFFFU) | mask);
    }
    else
    {
        base->TCSR = ((base->TCSR & 0xFFE3FFFFU) & (~mask));
    }
}

/*!
 * @brief Enables/disables the SAI Rx DMA requests.
 * @param base SAI base pointer
 * @param mask DMA source
 *     The parameter can be a combination of the following sources if defined.
 *     @arg kSAI_FIFOWarningDMAEnable
 *     @arg kSAI_FIFORequestDMAEnable
 * @param enable True means enable DMA, false means disable DMA.
 */
static inline void SAI_RxEnableDMA(I2S_Type *base, uint32_t mask, bool enable)
{
    if (enable)
    {
        base->RCSR = ((base->RCSR & 0xFFE3FFFFU) | mask);
    }
    else
    {
        base->RCSR = ((base->RCSR & 0xFFE3FFFFU) & (~mask));
    }
}

/*!
 * @brief  Gets the SAI Tx data register address.
 *
 * This API is used to provide a transfer address for the SAI DMA transfer configuration.
 *
 * @param base SAI base pointer.
 * @param channel Which data channel used.
 * @return data register address.
 */
static inline uint32_t SAI_TxGetDataRegisterAddress(I2S_Type *base, uint32_t channel)
{
    return (uint32_t)(&(base->TDR)[channel]);
}

/*!
 * @brief  Gets the SAI Rx data register address.
 *
 * This API is used to provide a transfer address for the SAI DMA transfer configuration.
 *
 * @param base SAI base pointer.
 * @param channel Which data channel used.
 * @return data register address.
 */
static inline uint32_t SAI_RxGetDataRegisterAddress(I2S_Type *base, uint32_t channel)
{
    return (uint32_t)(&(base->RDR)[channel]);
}

/*! @} */

/*!
 * @name Bus Operations
 * @{
 */

/*!
 * @brief Configures the SAI Tx audio format.
 * @deprecated Do not use this function.  It has been superceded by @ref SAI_TxSetConfig
 *
 * The audio format can be changed at run-time. This function configures the sample rate and audio data
 * format to be transferred.
 *
 * @param base SAI base pointer.
 * @param format Pointer to the SAI audio data format structure.
 * @param mclkSourceClockHz SAI master clock source frequency in Hz.
 * @param bclkSourceClockHz SAI bit clock source frequency in Hz. If the bit clock source is a master
 * clock, this value should equal the masterClockHz.
 */
void SAI_TxSetFormat(I2S_Type *base,
                     sai_transfer_format_t *format,
                     uint32_t mclkSourceClockHz,
                     uint32_t bclkSourceClockHz);

/*!
 * @brief Configures the SAI Rx audio format.
 * @deprecated Do not use this function.  It has been superceded by @ref SAI_RxSetConfig
 *
 * The audio format can be changed at run-time. This function configures the sample rate and audio data
 * format to be transferred.
 *
 * @param base SAI base pointer.
 * @param format Pointer to the SAI audio data format structure.
 * @param mclkSourceClockHz SAI master clock source frequency in Hz.
 * @param bclkSourceClockHz SAI bit clock source frequency in Hz. If the bit clock source is a master
 * clock, this value should equal the masterClockHz.
 */
void SAI_RxSetFormat(I2S_Type *base,
                     sai_transfer_format_t *format,
                     uint32_t mclkSourceClockHz,
                     uint32_t bclkSourceClockHz);

/*!
 * @brief Sends data using a blocking method.
 *
 * @note This function blocks by polling until data is ready to be sent.
 *
 * @param base SAI base pointer.
 * @param channel Data channel used.
 * @param bitWidth How many bits in an audio word; usually 8/16/24/32 bits.
 * @param buffer Pointer to the data to be written.
 * @param size Bytes to be written.
 */
void SAI_WriteBlocking(I2S_Type *base, uint32_t channel, uint32_t bitWidth, uint8_t *buffer, uint32_t size);

/*!
 * @brief Sends data to multi channel using a blocking method.
 *
 * @note This function blocks by polling until data is ready to be sent.
 *
 * @param base SAI base pointer.
 * @param channel Data channel used.
 * @param channelMask channel mask.
 * @param bitWidth How many bits in an audio word; usually 8/16/24/32 bits.
 * @param buffer Pointer to the data to be written.
 * @param size Bytes to be written.
 */
void SAI_WriteMultiChannelBlocking(
    I2S_Type *base, uint32_t channel, uint32_t channelMask, uint32_t bitWidth, uint8_t *buffer, uint32_t size);

/*!
 * @brief Writes data into SAI FIFO.
 *
 * @param base SAI base pointer.
 * @param channel Data channel used.
 * @param data Data needs to be written.
 */
static inline void SAI_WriteData(I2S_Type *base, uint32_t channel, uint32_t data)
{
    base->TDR[channel] = data;
}

/*!
 * @brief Receives data using a blocking method.
 *
 * @note This function blocks by polling until data is ready to be sent.
 *
 * @param base SAI base pointer.
 * @param channel Data channel used.
 * @param bitWidth How many bits in an audio word; usually 8/16/24/32 bits.
 * @param buffer Pointer to the data to be read.
 * @param size Bytes to be read.
 */
void SAI_ReadBlocking(I2S_Type *base, uint32_t channel, uint32_t bitWidth, uint8_t *buffer, uint32_t size);

/*!
 * @brief Receives multi channel data using a blocking method.
 *
 * @note This function blocks by polling until data is ready to be sent.
 *
 * @param base SAI base pointer.
 * @param channel Data channel used.
 * @param channelMask channel mask.
 * @param bitWidth How many bits in an audio word; usually 8/16/24/32 bits.
 * @param buffer Pointer to the data to be read.
 * @param size Bytes to be read.
 */
void SAI_ReadMultiChannelBlocking(
    I2S_Type *base, uint32_t channel, uint32_t channelMask, uint32_t bitWidth, uint8_t *buffer, uint32_t size);

/*!
 * @brief Reads data from the SAI FIFO.
 *
 * @param base SAI base pointer.
 * @param channel Data channel used.
 * @return Data in SAI FIFO.
 */
static inline uint32_t SAI_ReadData(I2S_Type *base, uint32_t channel)
{
    return base->RDR[channel];
}

/*! @} */

/*!
 * @name Transactional
 * @{
 */

/*!
 * @brief Initializes the SAI Tx handle.
 *
 * This function initializes the Tx handle for the SAI Tx transactional APIs. Call
 * this function once to get the handle initialized.
 *
 * @param base SAI base pointer
 * @param handle SAI handle pointer.
 * @param callback Pointer to the user callback function.
 * @param userData User parameter passed to the callback function
 */
void SAI_TransferTxCreateHandle(I2S_Type *base, sai_handle_t *handle, sai_transfer_callback_t callback, void *userData);

/*!
 * @brief Initializes the SAI Rx handle.
 *
 * This function initializes the Rx handle for the SAI Rx transactional APIs. Call
 * this function once to get the handle initialized.
 *
 * @param base SAI base pointer.
 * @param handle SAI handle pointer.
 * @param callback Pointer to the user callback function.
 * @param userData User parameter passed to the callback function.
 */
void SAI_TransferRxCreateHandle(I2S_Type *base, sai_handle_t *handle, sai_transfer_callback_t callback, void *userData);

/*!
 * @brief SAI transmitter transfer configurations.
 *
 * This function initializes the Tx, include bit clock, frame sync, master clock, serial data and fifo
 * configurations.
 *
 * @param base SAI base pointer.
 * @param handle SAI handle pointer.
 * @param config tranmitter configurations.
 */
void SAI_TransferTxSetConfig(I2S_Type *base, sai_handle_t *handle, sai_transceiver_t *config);

/*!
 * @brief SAI receiver transfer configurations.
 *
 * This function initializes the Rx, include bit clock, frame sync, master clock, serial data and fifo
 * configurations.
 *
 * @param base SAI base pointer.
 * @param handle SAI handle pointer.
 * @param config receiver configurations.
 */
void SAI_TransferRxSetConfig(I2S_Type *base, sai_handle_t *handle, sai_transceiver_t *config);

/*!
 * @brief Configures the SAI Tx audio format.
 * @deprecated Do not use this function.  It has been superceded by @ref SAI_TxSetTransferConfig
 *
 * The audio format can be changed at run-time. This function configures the sample rate and audio data
 * format to be transferred.
 *
 * @param base SAI base pointer.
 * @param handle SAI handle pointer.
 * @param format Pointer to the SAI audio data format structure.
 * @param mclkSourceClockHz SAI master clock source frequency in Hz.
 * @param bclkSourceClockHz SAI bit clock source frequency in Hz. If a bit clock source is a master
 * clock, this value should equal the masterClockHz in format.
 * @return Status of this function. Return value is the status_t.
 */
status_t SAI_TransferTxSetFormat(I2S_Type *base,
                                 sai_handle_t *handle,
                                 sai_transfer_format_t *format,
                                 uint32_t mclkSourceClockHz,
                                 uint32_t bclkSourceClockHz);

/*!
 * @brief Configures the SAI Rx audio format.
 * @deprecated Do not use this function.  It has been superceded by @ref SAI_RxSetTransferConfig
 *
 * The audio format can be changed at run-time. This function configures the sample rate and audio data
 * format to be transferred.
 *
 * @param base SAI base pointer.
 * @param handle SAI handle pointer.
 * @param format Pointer to the SAI audio data format structure.
 * @param mclkSourceClockHz SAI master clock source frequency in Hz.
 * @param bclkSourceClockHz SAI bit clock source frequency in Hz. If a bit clock source is a master
 * clock, this value should equal the masterClockHz in format.
 * @return Status of this function. Return value is one of status_t.
 */
status_t SAI_TransferRxSetFormat(I2S_Type *base,
                                 sai_handle_t *handle,
                                 sai_transfer_format_t *format,
                                 uint32_t mclkSourceClockHz,
                                 uint32_t bclkSourceClockHz);

/*!
 * @brief Performs an interrupt non-blocking send transfer on SAI.
 *
 * @note This API returns immediately after the transfer initiates.
 * Call the SAI_TxGetTransferStatusIRQ to poll the transfer status and check whether
 * the transfer is finished. If the return status is not kStatus_SAI_Busy, the transfer
 * is finished.
 *
 * @param base SAI base pointer.
 * @param handle Pointer to the sai_handle_t structure which stores the transfer state.
 * @param xfer Pointer to the sai_transfer_t structure.
 * @retval kStatus_Success Successfully started the data receive.
 * @retval kStatus_SAI_TxBusy Previous receive still not finished.
 * @retval kStatus_InvalidArgument The input parameter is invalid.
 */
status_t SAI_TransferSendNonBlocking(I2S_Type *base, sai_handle_t *handle, sai_transfer_t *xfer);

/*!
 * @brief Performs an interrupt non-blocking receive transfer on SAI.
 *
 * @note This API returns immediately after the transfer initiates.
 * Call the SAI_RxGetTransferStatusIRQ to poll the transfer status and check whether
 * the transfer is finished. If the return status is not kStatus_SAI_Busy, the transfer
 * is finished.
 *
 * @param base SAI base pointer
 * @param handle Pointer to the sai_handle_t structure which stores the transfer state.
 * @param xfer Pointer to the sai_transfer_t structure.
 * @retval kStatus_Success Successfully started the data receive.
 * @retval kStatus_SAI_RxBusy Previous receive still not finished.
 * @retval kStatus_InvalidArgument The input parameter is invalid.
 */
status_t SAI_TransferReceiveNonBlocking(I2S_Type *base, sai_handle_t *handle, sai_transfer_t *xfer);

/*!
 * @brief Gets a set byte count.
 *
 * @param base SAI base pointer.
 * @param handle Pointer to the sai_handle_t structure which stores the transfer state.
 * @param count Bytes count sent.
 * @retval kStatus_Success Succeed get the transfer count.
 * @retval kStatus_NoTransferInProgress There is not a non-blocking transaction currently in progress.
 */
status_t SAI_TransferGetSendCount(I2S_Type *base, sai_handle_t *handle, size_t *count);

/*!
 * @brief Gets a received byte count.
 *
 * @param base SAI base pointer.
 * @param handle Pointer to the sai_handle_t structure which stores the transfer state.
 * @param count Bytes count received.
 * @retval kStatus_Success Succeed get the transfer count.
 * @retval kStatus_NoTransferInProgress There is not a non-blocking transaction currently in progress.
 */
status_t SAI_TransferGetReceiveCount(I2S_Type *base, sai_handle_t *handle, size_t *count);

/*!
 * @brief Aborts the current send.
 *
 * @note This API can be called any time when an interrupt non-blocking transfer initiates
 * to abort the transfer early.
 *
 * @param base SAI base pointer.
 * @param handle Pointer to the sai_handle_t structure which stores the transfer state.
 */
void SAI_TransferAbortSend(I2S_Type *base, sai_handle_t *handle);

/*!
 * @brief Aborts the current IRQ receive.
 *
 * @note This API can be called when an interrupt non-blocking transfer initiates
 * to abort the transfer early.
 *
 * @param base SAI base pointer
 * @param handle Pointer to the sai_handle_t structure which stores the transfer state.
 */
void SAI_TransferAbortReceive(I2S_Type *base, sai_handle_t *handle);

/*!
 * @brief Terminate all SAI send.
 *
 * This function will clear all transfer slots buffered in the sai queue. If users only want to abort the
 * current transfer slot, please call SAI_TransferAbortSend.
 *
 * @param base SAI base pointer.
 * @param handle SAI eDMA handle pointer.
 */
void SAI_TransferTerminateSend(I2S_Type *base, sai_handle_t *handle);

/*!
 * @brief Terminate all SAI receive.
 *
 * This function will clear all transfer slots buffered in the sai queue. If users only want to abort the
 * current transfer slot, please call SAI_TransferAbortReceive.
 *
 * @param base SAI base pointer.
 * @param handle SAI eDMA handle pointer.
 */
void SAI_TransferTerminateReceive(I2S_Type *base, sai_handle_t *handle);

/*!
 * @brief Tx interrupt handler.
 *
 * @param base SAI base pointer.
 * @param handle Pointer to the sai_handle_t structure.
 */
void SAI_TransferTxHandleIRQ(I2S_Type *base, sai_handle_t *handle);

/*!
 * @brief Tx interrupt handler.
 *
 * @param base SAI base pointer.
 * @param handle Pointer to the sai_handle_t structure.
 */
void SAI_TransferRxHandleIRQ(I2S_Type *base, sai_handle_t *handle);

/*! @} */

#if defined(__cplusplus)
}
#endif /*_cplusplus*/

/*! @} */

#endif /* _FSL_SAI_H_ */
