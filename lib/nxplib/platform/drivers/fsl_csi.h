/*
 * Copyright  2017-2019 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_CSI_H_
#define _FSL_CSI_H_

#include "fsl_common.h"

/*!
 * @addtogroup csi_driver
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
/*@{*/
#define FSL_CSI_DRIVER_VERSION (MAKE_VERSION(2, 0, 3))
/*@}*/

/*! @brief Size of the frame buffer queue used in CSI transactional function. */
#ifndef CSI_DRIVER_QUEUE_SIZE
#define CSI_DRIVER_QUEUE_SIZE 4U
#endif

/*! @brief Enable fragment capture function or not. */
#ifndef CSI_DRIVER_FRAG_MODE
#define CSI_DRIVER_FRAG_MODE 0U
#endif

/*
 * There is one empty room in queue, used to distinguish whether the queue
 * is full or empty. When header equals tail, the queue is empty; when header
 * equals tail + 1, the queue is full.
 */
#define CSI_DRIVER_ACTUAL_QUEUE_SIZE (CSI_DRIVER_QUEUE_SIZE + 1U)

/*
 * The interrupt enable bits are in registers CSICR1[16:31], CSICR3[0:7],
 * and CSICR18[2:9]. So merge them into an uint32_t value, place CSICR18 control
 * bits to [8:15].
 */
#define CSI_CSICR1_INT_EN_MASK 0xFFFF0000U
#define CSI_CSICR3_INT_EN_MASK 0x000000FFU
#define CSI_CSICR18_INT_EN_MASK 0x0000FF00U

#if ((~CSI_CSICR1_INT_EN_MASK) &                                                                             \
     (CSI_CSICR1_EOF_INT_EN_MASK | CSI_CSICR1_COF_INT_EN_MASK | CSI_CSICR1_SF_OR_INTEN_MASK |                \
      CSI_CSICR1_RF_OR_INTEN_MASK | CSI_CSICR1_SFF_DMA_DONE_INTEN_MASK | CSI_CSICR1_STATFF_INTEN_MASK |      \
      CSI_CSICR1_FB2_DMA_DONE_INTEN_MASK | CSI_CSICR1_FB1_DMA_DONE_INTEN_MASK | CSI_CSICR1_RXFF_INTEN_MASK | \
      CSI_CSICR1_SOF_INTEN_MASK))
#error CSI_CSICR1_INT_EN_MASK could not cover all interrupt bits in CSICR1.
#endif

#if ((~CSI_CSICR3_INT_EN_MASK) & (CSI_CSICR3_ECC_INT_EN_MASK | CSI_CSICR3_HRESP_ERR_EN_MASK))
#error CSI_CSICR3_INT_EN_MASK could not cover all interrupt bits in CSICR3.
#endif

#if ((~CSI_CSICR18_INT_EN_MASK) & ((CSI_CSICR18_FIELD0_DONE_IE_MASK | CSI_CSICR18_DMA_FIELD1_DONE_IE_MASK | \
                                    CSI_CSICR18_BASEADDR_CHANGE_ERROR_IE_MASK)                              \
                                   << 6U))
#error CSI_CSICR18_INT_EN_MASK could not cover all interrupt bits in CSICR18.
#endif

/*! @brief Error codes for the CSI driver. */
enum _csi_status
{
    kStatus_CSI_NoEmptyBuffer = MAKE_STATUS(kStatusGroup_CSI, 0), /*!< No empty frame buffer in queue to load to CSI. */
    kStatus_CSI_NoFullBuffer  = MAKE_STATUS(kStatusGroup_CSI, 1), /*!< No full frame buffer in queue to read out. */
    kStatus_CSI_QueueFull = MAKE_STATUS(kStatusGroup_CSI, 2), /*!< Queue is full, no room to save new empty buffer. */
    kStatus_CSI_FrameDone = MAKE_STATUS(kStatusGroup_CSI, 3), /*!< New frame received and saved to queue. */
};

/*!
 * @brief CSI work mode.
 *
 * The CCIR656 interlace mode is not supported currently.
 */
typedef enum _csi_work_mode
{
    kCSI_GatedClockMode         = CSI_CSICR1_GCLK_MODE(1U), /*!< HSYNC, VSYNC, and PIXCLK signals are used. */
    kCSI_NonGatedClockMode      = 0U,                       /*!< VSYNC, and PIXCLK signals are used. */
    kCSI_CCIR656ProgressiveMode = CSI_CSICR1_CCIR_EN(1U),   /*!< CCIR656 progressive mode. */
} csi_work_mode_t;

/*!
 * @brief CSI data bus witdh.
 *
 * Currently only support 8-bit width.
 */
typedef enum _csi_data_bus
{
    kCSI_DataBus8Bit, /*!< 8-bit data bus. */
} csi_data_bus_t;

/*! @brief CSI signal polarity. */
enum _csi_polarity_flags
{
    kCSI_HsyncActiveLow         = 0U,                        /*!< HSYNC is active low. */
    kCSI_HsyncActiveHigh        = CSI_CSICR1_HSYNC_POL_MASK, /*!< HSYNC is active high. */
    kCSI_DataLatchOnRisingEdge  = CSI_CSICR1_REDGE_MASK,     /*!< Pixel data latched at rising edge of pixel clock. */
    kCSI_DataLatchOnFallingEdge = 0U,                        /*!< Pixel data latched at falling edge of pixel clock. */
    kCSI_VsyncActiveHigh        = 0U,                        /*!< VSYNC is active high. */
    kCSI_VsyncActiveLow         = CSI_CSICR1_SOF_POL_MASK,   /*!< VSYNC is active low. */
};

/*! @brief Configuration to initialize the CSI module. */
typedef struct _csi_config
{
    uint16_t width;           /*!< Pixels of the input frame. */
    uint16_t height;          /*!< Lines of the input frame.  */
    uint32_t polarityFlags;   /*!< Timing signal polarity flags, OR'ed value of @ref _csi_polarity_flags. */
    uint8_t bytesPerPixel;    /*!< Bytes per pixel, valid values are:
                                - 2: Used for RGB565, YUV422, and so on.
                                - 3: Used for packed RGB888, packed YUV444, and so on.
                                - 4: Used for XRGB8888, XYUV444, and so on.
                                */
    uint16_t linePitch_Bytes; /*!< Frame buffer line pitch, must be 8-byte aligned. */
    csi_work_mode_t workMode; /*!< CSI work mode. */
    csi_data_bus_t dataBus;   /*!< Data bus width. */
    bool useExtVsync;         /*!< In CCIR656 progressive mode, set true to use external VSYNC signal, set false
                                to use internal VSYNC signal decoded from SOF. */
} csi_config_t;

/*! @brief The CSI FIFO, used for FIFO operation. */
typedef enum _csi_fifo
{
    kCSI_RxFifo   = (1U << 0U),  /*!< RXFIFO. */
    kCSI_StatFifo = (1U << 1U),  /*!< STAT FIFO. */
    kCSI_AllFifo  = 0x01 | 0x02, /*!< Both RXFIFO and STAT FIFO. */
} csi_fifo_t;

/*! @brief CSI feature interrupt source. */
enum _csi_interrupt_enable
{
    kCSI_EndOfFrameInterruptEnable      = CSI_CSICR1_EOF_INT_EN_MASK,  /*!< End of frame interrupt enable. */
    kCSI_ChangeOfFieldInterruptEnable   = CSI_CSICR1_COF_INT_EN_MASK,  /*!< Change of field interrupt enable. */
    kCSI_StatFifoOverrunInterruptEnable = CSI_CSICR1_SF_OR_INTEN_MASK, /*!< STAT FIFO overrun interrupt enable. */
    kCSI_RxFifoOverrunInterruptEnable   = CSI_CSICR1_RF_OR_INTEN_MASK, /*!< RXFIFO overrun interrupt enable. */
    kCSI_StatFifoDmaDoneInterruptEnable =
        CSI_CSICR1_SFF_DMA_DONE_INTEN_MASK,                          /*!< STAT FIFO DMA done interrupt enable. */
    kCSI_StatFifoFullInterruptEnable = CSI_CSICR1_STATFF_INTEN_MASK, /*!< STAT FIFO full interrupt enable. */
    kCSI_RxBuffer1DmaDoneInterruptEnable =
        CSI_CSICR1_FB2_DMA_DONE_INTEN_MASK, /*!< RX frame buffer 1 DMA transfer done. */
    kCSI_RxBuffer0DmaDoneInterruptEnable =
        CSI_CSICR1_FB1_DMA_DONE_INTEN_MASK,                        /*!< RX frame buffer 0 DMA transfer done. */
    kCSI_RxFifoFullInterruptEnable   = CSI_CSICR1_RXFF_INTEN_MASK, /*!< RXFIFO full interrupt enable. */
    kCSI_StartOfFrameInterruptEnable = CSI_CSICR1_SOF_INTEN_MASK,  /*!< Start of frame (SOF) interrupt enable. */

    kCSI_EccErrorInterruptEnable    = CSI_CSICR3_ECC_INT_EN_MASK,   /*!< ECC error detection interrupt enable. */
    kCSI_AhbResErrorInterruptEnable = CSI_CSICR3_HRESP_ERR_EN_MASK, /*!< AHB response Error interrupt enable. */

    kCSI_BaseAddrChangeErrorInterruptEnable = CSI_CSICR18_BASEADDR_CHANGE_ERROR_IE_MASK
                                              << 6U,                            /*!< The DMA output buffer base address
                                                                changes before DMA completed. */
    kCSI_Field0DoneInterruptEnable = CSI_CSICR18_FIELD0_DONE_IE_MASK << 6U,     /*!< Field 0 done interrupt enable. */
    kCSI_Field1DoneInterruptEnable = CSI_CSICR18_DMA_FIELD1_DONE_IE_MASK << 6U, /*!< Field 1 done interrupt enable. */
};

/*!
 * @brief CSI status flags.
 *
 * The following status register flags can be cleared:
 * - kCSI_EccErrorFlag
 * - kCSI_AhbResErrorFlag
 * - kCSI_ChangeOfFieldFlag
 * - kCSI_StartOfFrameFlag
 * - kCSI_EndOfFrameFlag
 * - kCSI_RxBuffer1DmaDoneFlag
 * - kCSI_RxBuffer0DmaDoneFlag
 * - kCSI_StatFifoDmaDoneFlag
 * - kCSI_StatFifoOverrunFlag
 * - kCSI_RxFifoOverrunFlag
 * - kCSI_Field0DoneFlag
 * - kCSI_Field1DoneFlag
 * - kCSI_BaseAddrChangeErrorFlag
 */
enum _csi_flags
{
    kCSI_RxFifoDataReadyFlag     = CSI_CSISR_DRDY_MASK,          /*!< RXFIFO data ready. */
    kCSI_EccErrorFlag            = CSI_CSISR_ECC_INT_MASK,       /*!< ECC error detected. */
    kCSI_AhbResErrorFlag         = CSI_CSISR_HRESP_ERR_INT_MASK, /*!< Hresponse (AHB bus response) Error. */
    kCSI_ChangeOfFieldFlag       = CSI_CSISR_COF_INT_MASK,       /*!< Change of field. */
    kCSI_Field0PresentFlag       = CSI_CSISR_F1_INT_MASK,        /*!< Field 0 present in CCIR mode. */
    kCSI_Field1PresentFlag       = CSI_CSISR_F2_INT_MASK,        /*!< Field 1 present in CCIR mode. */
    kCSI_StartOfFrameFlag        = CSI_CSISR_SOF_INT_MASK,       /*!< Start of frame (SOF) detected. */
    kCSI_EndOfFrameFlag          = CSI_CSISR_EOF_INT_MASK,       /*!< End of frame (EOF) detected. */
    kCSI_RxFifoFullFlag          = CSI_CSISR_RxFF_INT_MASK, /*!< RXFIFO full (Number of data reaches trigger level). */
    kCSI_RxBuffer1DmaDoneFlag    = CSI_CSISR_DMA_TSF_DONE_FB2_MASK,       /*!< RX frame buffer 1 DMA transfer done. */
    kCSI_RxBuffer0DmaDoneFlag    = CSI_CSISR_DMA_TSF_DONE_FB1_MASK,       /*!< RX frame buffer 0 DMA transfer done. */
    kCSI_StatFifoFullFlag        = CSI_CSISR_STATFF_INT_MASK,             /*!< STAT FIFO full (Reach trigger level). */
    kCSI_StatFifoDmaDoneFlag     = CSI_CSISR_DMA_TSF_DONE_SFF_MASK,       /*!< STAT FIFO DMA transfer done. */
    kCSI_StatFifoOverrunFlag     = CSI_CSISR_SF_OR_INT_MASK,              /*!< STAT FIFO overrun. */
    kCSI_RxFifoOverrunFlag       = CSI_CSISR_RF_OR_INT_MASK,              /*!< RXFIFO overrun. */
    kCSI_Field0DoneFlag          = CSI_CSISR_DMA_FIELD0_DONE_MASK,        /*!< Field 0 transfer done. */
    kCSI_Field1DoneFlag          = CSI_CSISR_DMA_FIELD1_DONE_MASK,        /*!< Field 1 transfer done. */
    kCSI_BaseAddrChangeErrorFlag = CSI_CSISR_BASEADDR_CHHANGE_ERROR_MASK, /*!< The DMA output buffer base address
                                                                               changes before DMA completed. */
};

/* Forward declaration of the handle typedef. */
typedef struct _csi_handle csi_handle_t;

/*!
 * @brief CSI transfer callback function.
 *
 * When a new frame is received and saved to the frame buffer queue, the callback
 * is called and the pass the status @ref kStatus_CSI_FrameDone to upper layer.
 */
typedef void (*csi_transfer_callback_t)(CSI_Type *base, csi_handle_t *handle, status_t status, void *userData);

/*!
 * @brief CSI handle structure.
 *
 * Please see the user guide for the details of the CSI driver queue mechanism.
 */
struct _csi_handle
{
    uint32_t frameBufferQueue[CSI_DRIVER_ACTUAL_QUEUE_SIZE]; /*!< Frame buffer queue. */

    volatile uint8_t queueUserReadIdx;  /*!< Application gets full-filled frame buffer from this index. */
    volatile uint8_t queueUserWriteIdx; /*!< Application puts empty frame buffer to this index. */
    volatile uint8_t queueDrvReadIdx;   /*!< Driver gets empty frame buffer from this index. */
    volatile uint8_t queueDrvWriteIdx;  /*!< Driver puts the full-filled frame buffer to this index. */

    volatile uint8_t activeBufferNum; /*!< How many frame buffers are in progres currently. */
    volatile uint8_t nextBufferIdx;   /*!< The CSI frame buffer index to use for next frame. */

    volatile bool transferStarted; /*!< User has called @ref CSI_TransferStart to start frame receiving. */
    volatile bool transferOnGoing; /*!< CSI is working and receiving incoming frames. */

    csi_transfer_callback_t callback; /*!< Callback function. */
    void *userData;                   /*!< CSI callback function parameter.*/
};

#if CSI_DRIVER_FRAG_MODE

/*! @brief Input pixel format when CSI works in fragment mode. */
typedef enum _csi_frag_input_pixel_format
{
    kCSI_FragInputRGB565 = 0, /*!< Input pixel format is RGB565. */
    kCSI_FragInputYUYV,       /*!< Input pixel format is YUV422 (Y-U-Y-V). */
    kCSI_FragInputUYVY,       /*!< Input pixel format is YUV422 (U-Y-V-Y). */
} csi_frag_input_pixel_format_t;

/*! @brief Configuration for CSI module to work in fragment mode. */
typedef struct _csi_frag_config
{
    uint16_t width;           /*!< Pixels of the input frame. */
    uint16_t height;          /*!< Lines of the input frame.  */
    uint32_t polarityFlags;   /*!< Timing signal polarity flags, OR'ed value of @ref _csi_polarity_flags. */
    csi_work_mode_t workMode; /*!< CSI work mode. */
    csi_data_bus_t dataBus;   /*!< Data bus width. */
    bool useExtVsync;         /*!< In CCIR656 progressive mode, set true to use external VSYNC signal, set false
                                to use internal VSYNC signal decoded from SOF. */
    csi_frag_input_pixel_format_t inputFormat; /*!< Input pixel format. */

    uint32_t dmaBufferAddr0;  /*!< Buffer 0 used for CSI DMA, must be double word aligned. */
    uint32_t dmaBufferAddr1;  /*!< Buffer 1 used for CSI DMA, must be double word aligned. */
    uint16_t dmaBufferLine;   /*!< Lines of each DMA buffer. The size of DMA buffer 0 and
                                   buffer 1 must be the same. Camera frame height must be
                                   dividable by this value. */
    bool isDmaBufferCachable; /*!< Is DMA buffer cachable or not. */
} csi_frag_config_t;

/* Forward declaration of the handle typedef. */
typedef struct _csi_frag_handle csi_frag_handle_t;

/*!
 * @brief CSI fragment transfer callback function.
 *
 * When a new frame is received and saved to the frame buffer queue, the callback
 * is called and the pass the status @ref kStatus_CSI_FrameDone to upper layer.
 */
typedef void (*csi_frag_transfer_callback_t)(CSI_Type *base,
                                             csi_frag_handle_t *handle,
                                             status_t status,
                                             void *userData);

/*!
 * @brief Function to copy data from CSI DMA buffer to user buffer.
 */
typedef void (*csi_frag_copy_func_t)(void *pDest, const void *pSrc, size_t cnt);

/*! @brief Handle for CSI module to work in fragment mode. */
struct _csi_frag_handle
{
    uint16_t width;                            /*!< Pixels of the input frame. */
    uint16_t height;                           /*!< Lines of the input frame.  */
    uint16_t maxLinePerFrag;                   /*!< Max line saved per fragment. */
    uint16_t linePerFrag;                      /*!< Actual line saved per fragment. */
    uint16_t dmaBytePerLine;                   /*!< How many bytes DMA transfered each line. */
    uint16_t datBytePerLine;                   /*!< How many bytes copied to user buffer each line. */
    uint16_t dmaCurLine;                       /*!< Current line index in whole frame. */
    uint16_t windowULX;                        /*!< X of windows upper left corner. */
    uint16_t windowULY;                        /*!< Y of windows upper left corner. */
    uint16_t windowLRX;                        /*!< X of windows lower right corner. */
    uint16_t windowLRY;                        /*!< Y of windows lower right corner. */
    uint32_t outputBuffer;                     /*!< Address of buffer to save the captured image. */
    uint32_t datCurWriteAddr;                  /*!< Current write address to the user buffer. */
    csi_frag_input_pixel_format_t inputFormat; /*!< Input pixel format. */

    csi_frag_transfer_callback_t callback; /*!< Callback function. */
    void *userData;                        /*!< CSI callback function parameter.*/
    csi_frag_copy_func_t copyFunc;         /*!< Function to copy data from CSI DMA buffer to user buffer. */
    bool isDmaBufferCachable;              /*!< Is DMA buffer cachable or not. */
};

/*! @brief Handle for CSI module to work in fragment mode. */
typedef struct _csi_frag_window
{
    uint16_t windowULX; /*!< X of windows upper left corner. */
    uint16_t windowULY; /*!< Y of windows upper left corner. */
    uint16_t windowLRX; /*!< X of windows lower right corner. */
    uint16_t windowLRY; /*!< Y of windows lower right corner. */
} csi_frag_window_t;

/*! @brief Handle for CSI module to work in fragment mode. */
typedef struct _csi_frag_capture_config
{
    bool outputGrayScale;      /*!< Output gray scale image or not, could only enable when input format is YUV. */
    uint32_t buffer;           /*!< Buffer to save the captured image. */
    csi_frag_window_t *window; /*!< Capture window. Capture full frame if set this to NULL. When output format is gray,
                                    the window width must be multiple value of 8. */
} csi_frag_capture_config_t;

#endif /* CSI_DRIVER_FRAG_MODE */

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name Initialization and deinitialization
 * @{
 */

/*!
 * @brief Initialize the CSI.
 *
 * This function enables the CSI peripheral clock, and resets the CSI registers.
 *
 * @param base CSI peripheral base address.
 * @param config Pointer to the configuration structure.
 *
 * @retval kStatus_Success Initialize successfully.
 * @retval kStatus_InvalidArgument Initialize failed because of invalid argument.
 */
status_t CSI_Init(CSI_Type *base, const csi_config_t *config);

/*!
 * @brief De-initialize the CSI.
 *
 * This function disables the CSI peripheral clock.
 *
 * @param base CSI peripheral base address.
 */
void CSI_Deinit(CSI_Type *base);

/*!
 * @brief Reset the CSI.
 *
 * This function resets the CSI peripheral registers to default status.
 *
 * @param base CSI peripheral base address.
 */
void CSI_Reset(CSI_Type *base);

/*!
 * @brief Get the default configuration for to initialize the CSI.
 *
 * The default configuration value is:
 *
 * @code
    config->width = 320U;
    config->height = 240U;
    config->polarityFlags = kCSI_HsyncActiveHigh | kCSI_DataLatchOnRisingEdge;
    config->bytesPerPixel = 2U;
    config->linePitch_Bytes = 320U * 2U;
    config->workMode = kCSI_GatedClockMode;
    config->dataBus = kCSI_DataBus8Bit;
    config->useExtVsync = true;
   @endcode
 *
 * @param config Pointer to the CSI configuration.
 */
void CSI_GetDefaultConfig(csi_config_t *config);

/* @} */

/*!
 * @name Module operation
 * @{
 */

/*!
 * @brief Clear the CSI FIFO.
 *
 * This function clears the CSI FIFO.
 *
 * @param base CSI peripheral base address.
 * @param fifo The FIFO to clear.
 */
void CSI_ClearFifo(CSI_Type *base, csi_fifo_t fifo);

/*!
 * @brief Reflash the CSI FIFO DMA.
 *
 * This function reflashes the CSI FIFO DMA.
 *
 * For RXFIFO, there are two frame buffers. When the CSI module started, it saves
 * the frames to frame buffer 0 then frame buffer 1, the two buffers will be
 * written by turns. After reflash DMA using this function, the CSI is reset to
 * save frame to buffer 0.
 *
 * @param base CSI peripheral base address.
 * @param fifo The FIFO DMA to reflash.
 */
void CSI_ReflashFifoDma(CSI_Type *base, csi_fifo_t fifo);

/*!
 * @brief Enable or disable the CSI FIFO DMA request.
 *
 * @param base CSI peripheral base address.
 * @param fifo The FIFO DMA reques to enable or disable.
 * @param enable True to enable, false to disable.
 */
void CSI_EnableFifoDmaRequest(CSI_Type *base, csi_fifo_t fifo, bool enable);

/*!
 * @brief Start to receive data.
 *
 * @param base CSI peripheral base address.
 */
static inline void CSI_Start(CSI_Type *base)
{
    CSI_EnableFifoDmaRequest(base, kCSI_RxFifo, true);
    base->CSICR18 |= CSI_CSICR18_CSI_ENABLE_MASK;
}

/*!
 * @brief Stop to receiving data.
 *
 * @param base CSI peripheral base address.
 */
static inline void CSI_Stop(CSI_Type *base)
{
    base->CSICR18 &= ~CSI_CSICR18_CSI_ENABLE_MASK;
    CSI_EnableFifoDmaRequest(base, kCSI_RxFifo, false);
}

/*!
 * @brief Set the RX frame buffer address.
 *
 * @param base CSI peripheral base address.
 * @param index Buffer index.
 * @param addr Frame buffer address to set.
 */
void CSI_SetRxBufferAddr(CSI_Type *base, uint8_t index, uint32_t addr);
/* @} */

/*!
 * @name Interrupts
 * @{
 */

/*!
 * @brief Enables CSI interrupt requests.
 *
 * @param base CSI peripheral base address.
 * @param mask The interrupts to enable, pass in as OR'ed value of @ref _csi_interrupt_enable.
 */
void CSI_EnableInterrupts(CSI_Type *base, uint32_t mask);

/*!
 * @brief Disable CSI interrupt requests.
 *
 * @param base CSI peripheral base address.
 * @param mask The interrupts to disable, pass in as OR'ed value of @ref _csi_interrupt_enable.
 */
void CSI_DisableInterrupts(CSI_Type *base, uint32_t mask);

/* @} */

/*!
 * @name Status
 * @{
 */

/*!
 * @brief Gets the CSI status flags.
 *
 * @param base CSI peripheral base address.
 * @return status flag, it is OR'ed value of @ref _csi_flags.
 */
static inline uint32_t CSI_GetStatusFlags(CSI_Type *base)
{
    return base->CSISR;
}

/*!
 * @brief Clears the CSI status flag.
 *
 * The flags to clear are passed in as OR'ed value of @ref _csi_flags. The following
 * flags are cleared automatically by hardware:
 *
 * - @ref kCSI_RxFifoFullFlag,
 * - @ref kCSI_StatFifoFullFlag,
 * - @ref kCSI_Field0PresentFlag,
 * - @ref kCSI_Field1PresentFlag,
 * - @ref kCSI_RxFifoDataReadyFlag,
 *
 * @param base CSI peripheral base address.
 * @param statusMask The status flags mask, OR'ed value of @ref _csi_flags.
 */
static inline void CSI_ClearStatusFlags(CSI_Type *base, uint32_t statusMask)
{
    base->CSISR = statusMask;
}
/* @} */

#if !CSI_DRIVER_FRAG_MODE
/*!
 * @name Transactional
 * @{
 */

/*!
 * @brief Initializes the CSI handle.
 *
 * This function initializes CSI handle, it should be called before any other
 * CSI transactional functions.
 *
 * @param base CSI peripheral base address.
 * @param handle Pointer to the handle structure.
 * @param callback Callback function for CSI transfer.
 * @param userData Callback function parameter.
 *
 * @retval kStatus_Success Handle created successfully.
 */
status_t CSI_TransferCreateHandle(CSI_Type *base,
                                  csi_handle_t *handle,
                                  csi_transfer_callback_t callback,
                                  void *userData);

/*!
 * @brief Start the transfer using transactional functions.
 *
 * When the empty frame buffers have been submit to CSI driver using function
 * @ref CSI_TransferSubmitEmptyBuffer, user could call this function to start
 * the transfer. The incoming frame will be saved to the empty frame buffer,
 * and user could be optionally notified through callback function.
 *
 * @param base CSI peripheral base address.
 * @param handle Pointer to the handle structure.
 *
 * @retval kStatus_Success Started successfully.
 * @retval kStatus_CSI_NoEmptyBuffer Could not start because no empty frame buffer in queue.
 */
status_t CSI_TransferStart(CSI_Type *base, csi_handle_t *handle);

/*!
 * @brief Stop the transfer using transactional functions.
 *
 * The driver does not clean the full frame buffers in queue. In other words, after
 * calling this function, user still could get the full frame buffers in queue
 * using function @ref CSI_TransferGetFullBuffer.
 *
 * @param base CSI peripheral base address.
 * @param handle Pointer to the handle structure.
 *
 * @retval kStatus_Success Stoped successfully.
 */
status_t CSI_TransferStop(CSI_Type *base, csi_handle_t *handle);

/*!
 * @brief Submit empty frame buffer to queue.
 *
 * This function could be called before @ref CSI_TransferStart or after @ref
 * CSI_TransferStart. If there is no room in queue to store the empty frame
 * buffer, this function returns error.
 *
 * @param base CSI peripheral base address.
 * @param handle Pointer to the handle structure.
 * @param frameBuffer Empty frame buffer to submit.
 *
 * @retval kStatus_Success Started successfully.
 * @retval kStatus_CSI_QueueFull Could not submit because there is no room in queue.
 */
status_t CSI_TransferSubmitEmptyBuffer(CSI_Type *base, csi_handle_t *handle, uint32_t frameBuffer);

/*!
 * @brief Get one full frame buffer from queue.
 *
 * After the transfer started using function @ref CSI_TransferStart, the incoming
 * frames will be saved to the empty frame buffers in queue. This function gets
 * the full-filled frame buffer from the queue. If there is no full frame buffer
 * in queue, this function returns error.
 *
 * @param base CSI peripheral base address.
 * @param handle Pointer to the handle structure.
 * @param frameBuffer Full frame buffer.
 *
 * @retval kStatus_Success Started successfully.
 * @retval kStatus_CSI_NoFullBuffer There is no full frame buffer in queue.
 */
status_t CSI_TransferGetFullBuffer(CSI_Type *base, csi_handle_t *handle, uint32_t *frameBuffer);

/*!
 * @brief CSI IRQ handle function.
 *
 * This function handles the CSI IRQ request to work with CSI driver transactional
 * APIs.
 *
 * @param base CSI peripheral base address.
 * @param handle CSI handle pointer.
 */
void CSI_TransferHandleIRQ(CSI_Type *base, csi_handle_t *handle);
/* @} */

#else

/*!
 * @name Fragment mode
 * @{
 */

/*!
 * @brief Initialize the CSI to work in fragment mode.
 *
 * This function enables the CSI peripheral clock, and resets the CSI registers.
 *
 * @param base CSI peripheral base address.
 */
void CSI_FragModeInit(CSI_Type *base);

/*!
 * @brief De-initialize the CSI.
 *
 * This function disables the CSI peripheral clock.
 *
 * @param base CSI peripheral base address.
 */
void CSI_FragModeDeinit(CSI_Type *base);

/*!
 * @brief Create handle for CSI work in fragment mode.
 *
 * @param base CSI peripheral base address.
 * @param handle Pointer to the transactional handle.
 * @param config Pointer to the configuration structure.
 * @param callback Callback function for CSI transfer.
 * @param userData Callback function parameter.
 *
 * @retval kStatus_Success Initialize successfully.
 * @retval kStatus_InvalidArgument Initialize failed because of invalid argument.
 */
status_t CSI_FragModeCreateHandle(CSI_Type *base,
                                  csi_frag_handle_t *handle,
                                  const csi_frag_config_t *config,
                                  csi_frag_transfer_callback_t callback,
                                  void *userData);

/*!
 * @brief Start to capture a image.
 *
 * @param base CSI peripheral base address.
 * @param handle Pointer to the transactional handle.
 * @param config Pointer to the capture configuration.
 *
 * @retval kStatus_Success Initialize successfully.
 * @retval kStatus_InvalidArgument Initialize failed because of invalid argument.
 */
status_t CSI_FragModeTransferCaptureImage(CSI_Type *base,
                                          csi_frag_handle_t *handle,
                                          const csi_frag_capture_config_t *config);

/*!
 * @brief Abort image capture.
 *
 * Abort image capture initialized by @ref CSI_FragModeTransferCaptureImage.
 *
 * @param base CSI peripheral base address.
 * @param handle Pointer to the transactional handle.
 */
void CSI_FragModeTransferAbortCaptureImage(CSI_Type *base, csi_frag_handle_t *handle);

/*!
 * @brief CSI IRQ handle function.
 *
 * This function handles the CSI IRQ request to work with CSI driver fragment mode
 * APIs.
 *
 * @param base CSI peripheral base address.
 * @param handle CSI handle pointer.
 */
void CSI_FragModeTransferHandleIRQ(CSI_Type *base, csi_frag_handle_t *handle);

/* @} */

#endif /* CSI_DRIVER_FRAG_MODE */

#if defined(__cplusplus)
}
#endif

/*! @}*/

#endif /* _FSL_CSI_H_ */
