/*
 * Copyright  2017-2018 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_csi.h"
#if CSI_DRIVER_FRAG_MODE
#include "fsl_cache.h"
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.csi"
#endif

/* Two frame buffer loaded to CSI register at most. */
#define CSI_MAX_ACTIVE_FRAME_NUM 2

/* CSI driver only support RGB565 and YUV422 in fragment mode, 2 bytes per pixel. */
#define CSI_FRAG_INPUT_BYTES_PER_PIXEL 2

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*!
 * @brief Get the instance from the base address
 *
 * @param base CSI peripheral base address
 *
 * @return The CSI module instance
 */
static uint32_t CSI_GetInstance(CSI_Type *base);

#if !CSI_DRIVER_FRAG_MODE
/*!
 * @brief Get the delta value of two index in queue.
 *
 * @param startIdx Start index.
 * @param endIdx End index.
 *
 * @return The delta between startIdx and endIdx in queue.
 */
static uint32_t CSI_TransferGetQueueDelta(uint32_t startIdx, uint32_t endIdx);

/*!
 * @brief Increase a index value in queue.
 *
 * This function increases the index value in the queue, if the index is out of
 * the queue range, it is reset to 0.
 *
 * @param idx The index value to increase.
 *
 * @return The index value after increase.
 */
static uint32_t CSI_TransferIncreaseQueueIdx(uint32_t idx);

/*!
 * @brief Get the empty frame buffer count in queue.
 *
 * @param base CSI peripheral base address
 * @param handle Pointer to CSI driver handle.
 *
 * @return Number of the empty frame buffer count in queue.
 */
static uint32_t CSI_TransferGetEmptyBufferCount(CSI_Type *base, csi_handle_t *handle);

/*!
 * @brief Load one empty frame buffer in queue to CSI module.
 *
 * Load one empty frame in queue to CSI module, this function could only be called
 * when there is empty frame buffer in queue.
 *
 * @param base CSI peripheral base address
 * @param handle Pointer to CSI driver handle.
 */
static void CSI_TransferLoadBufferToDevice(CSI_Type *base, csi_handle_t *handle);

/* Typedef for interrupt handler. */
typedef void (*csi_isr_t)(CSI_Type *base, csi_handle_t *handle);

#else

/* Typedef for interrupt handler to work in fragment mode. */
typedef void (*csi_isr_t)(CSI_Type *base, csi_frag_handle_t *handle);
#endif /* CSI_DRIVER_FRAG_MODE */

/*******************************************************************************
 * Variables
 ******************************************************************************/
/*! @brief Pointers to CSI bases for each instance. */
static CSI_Type *const s_csiBases[] = CSI_BASE_PTRS;

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
/*! @brief Pointers to CSI clocks for each CSI submodule. */
static const clock_ip_name_t s_csiClocks[] = CSI_CLOCKS;
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

/* Array for the CSI driver handle. */
#if !CSI_DRIVER_FRAG_MODE
static csi_handle_t *s_csiHandle[ARRAY_SIZE(s_csiBases)];
#else
static csi_frag_handle_t *s_csiHandle[ARRAY_SIZE(s_csiBases)];
#endif

/* Array of CSI IRQ number. */
static const IRQn_Type s_csiIRQ[] = CSI_IRQS;

/* CSI ISR for transactional APIs. */
static csi_isr_t s_csiIsr;

/*******************************************************************************
 * Code
 ******************************************************************************/
static uint32_t CSI_GetInstance(CSI_Type *base)
{
    uint32_t instance;

    /* Find the instance index from base address mappings. */
    for (instance = 0; instance < ARRAY_SIZE(s_csiBases); instance++)
    {
        if (s_csiBases[instance] == base)
        {
            break;
        }
    }

    assert(instance < ARRAY_SIZE(s_csiBases));

    return instance;
}

#if !CSI_DRIVER_FRAG_MODE
static uint32_t CSI_TransferGetQueueDelta(uint32_t startIdx, uint32_t endIdx)
{
    if (endIdx >= startIdx)
    {
        return endIdx - startIdx;
    }
    else
    {
        return endIdx + CSI_DRIVER_ACTUAL_QUEUE_SIZE - startIdx;
    }
}

static uint32_t CSI_TransferIncreaseQueueIdx(uint32_t idx)
{
    uint32_t ret;

    /*
     * Here not use the method:
     * ret = (idx+1) % CSI_DRIVER_ACTUAL_QUEUE_SIZE;
     *
     * Because the mod function might be slow.
     */

    ret = idx + 1;

    if (ret >= CSI_DRIVER_ACTUAL_QUEUE_SIZE)
    {
        ret = 0;
    }

    return ret;
}

static uint32_t CSI_TransferGetEmptyBufferCount(CSI_Type *base, csi_handle_t *handle)
{
    return CSI_TransferGetQueueDelta(handle->queueDrvReadIdx, handle->queueUserWriteIdx);
}

static void CSI_TransferLoadBufferToDevice(CSI_Type *base, csi_handle_t *handle)
{
    /* Load the frame buffer address to CSI register. */
    CSI_SetRxBufferAddr(base, handle->nextBufferIdx, handle->frameBufferQueue[handle->queueDrvReadIdx]);

    handle->queueDrvReadIdx = CSI_TransferIncreaseQueueIdx(handle->queueDrvReadIdx);
    handle->activeBufferNum++;

    /* There are two CSI buffers, so could use XOR to get the next index. */
    handle->nextBufferIdx ^= 1U;
}
#endif /* CSI_DRIVER_FRAG_MODE */

/*!
 * brief Initialize the CSI.
 *
 * This function enables the CSI peripheral clock, and resets the CSI registers.
 *
 * param base CSI peripheral base address.
 * param config Pointer to the configuration structure.
 *
 * retval kStatus_Success Initialize successfully.
 * retval kStatus_InvalidArgument Initialize failed because of invalid argument.
 */
status_t CSI_Init(CSI_Type *base, const csi_config_t *config)
{
    assert(config);
    uint32_t reg;
    uint32_t imgWidth_Bytes;

    imgWidth_Bytes = config->width * config->bytesPerPixel;

    /* The image width and frame buffer pitch should be multiple of 8-bytes. */
    if ((imgWidth_Bytes & 0x07) | ((uint32_t)config->linePitch_Bytes & 0x07))
    {
        return kStatus_InvalidArgument;
    }

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    uint32_t instance = CSI_GetInstance(base);
    CLOCK_EnableClock(s_csiClocks[instance]);
#endif

    CSI_Reset(base);

    /* Configure CSICR1. CSICR1 has been reset to the default value, so could write it directly. */
    reg = ((uint32_t)config->workMode) | config->polarityFlags | CSI_CSICR1_FCC_MASK;

    if (config->useExtVsync)
    {
        reg |= CSI_CSICR1_EXT_VSYNC_MASK;
    }

    base->CSICR1 = reg;

    /*
     * Generally, CSIIMAG_PARA[IMAGE_WIDTH] indicates how many data bus cycles per line.
     * One special case is when receiving 24-bit pixels through 8-bit data bus, and
     * CSICR3[ZERO_PACK_EN] is enabled, in this case, the CSIIMAG_PARA[IMAGE_WIDTH]
     * should be set to the pixel number per line.
     *
     * Currently the CSI driver only support 8-bit data bus, so generally the
     * CSIIMAG_PARA[IMAGE_WIDTH] is bytes number per line. When the CSICR3[ZERO_PACK_EN]
     * is enabled, CSIIMAG_PARA[IMAGE_WIDTH] is pixel number per line.
     *
     * NOTE: The CSIIMAG_PARA[IMAGE_WIDTH] setting code should be updated if the
     * driver is upgraded to support other data bus width.
     */
    if (4U == config->bytesPerPixel)
    {
        /* Enable zero pack. */
        base->CSICR3 |= CSI_CSICR3_ZERO_PACK_EN_MASK;
        /* Image parameter. */
        base->CSIIMAG_PARA = ((uint32_t)(config->width) << CSI_CSIIMAG_PARA_IMAGE_WIDTH_SHIFT) |
                             ((uint32_t)(config->height) << CSI_CSIIMAG_PARA_IMAGE_HEIGHT_SHIFT);
    }
    else
    {
        /* Image parameter. */
        base->CSIIMAG_PARA = ((uint32_t)(imgWidth_Bytes) << CSI_CSIIMAG_PARA_IMAGE_WIDTH_SHIFT) |
                             ((uint32_t)(config->height) << CSI_CSIIMAG_PARA_IMAGE_HEIGHT_SHIFT);
    }

    /* The CSI frame buffer bus is 8-byte width. */
    base->CSIFBUF_PARA = (uint32_t)((config->linePitch_Bytes - imgWidth_Bytes) / 8U)
                         << CSI_CSIFBUF_PARA_FBUF_STRIDE_SHIFT;

    /* Enable auto ECC. */
    base->CSICR3 |= CSI_CSICR3_ECC_AUTO_EN_MASK;

    /*
     * For better performance.
     * The DMA burst size could be set to 16 * 8 byte, 8 * 8 byte, or 4 * 8 byte,
     * choose the best burst size based on bytes per line.
     */
    if (!(imgWidth_Bytes % (8 * 16)))
    {
        base->CSICR2 = CSI_CSICR2_DMA_BURST_TYPE_RFF(3U);
        base->CSICR3 = (CSI->CSICR3 & ~CSI_CSICR3_RxFF_LEVEL_MASK) | ((2U << CSI_CSICR3_RxFF_LEVEL_SHIFT));
    }
    else if (!(imgWidth_Bytes % (8 * 8)))
    {
        base->CSICR2 = CSI_CSICR2_DMA_BURST_TYPE_RFF(2U);
        base->CSICR3 = (CSI->CSICR3 & ~CSI_CSICR3_RxFF_LEVEL_MASK) | ((1U << CSI_CSICR3_RxFF_LEVEL_SHIFT));
    }
    else
    {
        base->CSICR2 = CSI_CSICR2_DMA_BURST_TYPE_RFF(1U);
        base->CSICR3 = (CSI->CSICR3 & ~CSI_CSICR3_RxFF_LEVEL_MASK) | ((0U << CSI_CSICR3_RxFF_LEVEL_SHIFT));
    }

    CSI_ReflashFifoDma(base, kCSI_RxFifo);

    return kStatus_Success;
}

/*!
 * brief De-initialize the CSI.
 *
 * This function disables the CSI peripheral clock.
 *
 * param base CSI peripheral base address.
 */
void CSI_Deinit(CSI_Type *base)
{
    /* Disable transfer first. */
    CSI_Stop(base);
#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    uint32_t instance = CSI_GetInstance(base);
    CLOCK_DisableClock(s_csiClocks[instance]);
#endif
}

/*!
 * brief Reset the CSI.
 *
 * This function resets the CSI peripheral registers to default status.
 *
 * param base CSI peripheral base address.
 */
void CSI_Reset(CSI_Type *base)
{
    uint32_t csisr;

    /* Disable transfer first. */
    CSI_Stop(base);

    /* Disable DMA request. */
    base->CSICR3 = 0U;

    /* Reset the fame count. */
    base->CSICR3 |= CSI_CSICR3_FRMCNT_RST_MASK;
    while (base->CSICR3 & CSI_CSICR3_FRMCNT_RST_MASK)
    {
    }

    /* Clear the RX FIFO. */
    CSI_ClearFifo(base, kCSI_AllFifo);

    /* Reflash DMA. */
    CSI_ReflashFifoDma(base, kCSI_AllFifo);

    /* Clear the status. */
    csisr       = base->CSISR;
    base->CSISR = csisr;

    /* Set the control registers to default value. */
    base->CSICR1 = CSI_CSICR1_HSYNC_POL_MASK | CSI_CSICR1_EXT_VSYNC_MASK;
    base->CSICR2 = 0U;
    base->CSICR3 = 0U;
#if defined(CSI_CSICR18_CSI_LCDIF_BUFFER_LINES)
    base->CSICR18 = CSI_CSICR18_AHB_HPROT(0x0DU) | CSI_CSICR18_CSI_LCDIF_BUFFER_LINES(0x02U);
#else
    base->CSICR18 = CSI_CSICR18_AHB_HPROT(0x0DU);
#endif
    base->CSIFBUF_PARA = 0U;
    base->CSIIMAG_PARA = 0U;
}

/*!
 * brief Get the default configuration for to initialize the CSI.
 *
 * The default configuration value is:
 *
 * code
    config->width = 320U;
    config->height = 240U;
    config->polarityFlags = kCSI_HsyncActiveHigh | kCSI_DataLatchOnRisingEdge;
    config->bytesPerPixel = 2U;
    config->linePitch_Bytes = 320U * 2U;
    config->workMode = kCSI_GatedClockMode;
    config->dataBus = kCSI_DataBus8Bit;
    config->useExtVsync = true;
   endcode
 *
 * param config Pointer to the CSI configuration.
 */
void CSI_GetDefaultConfig(csi_config_t *config)
{
    assert(config);

    /* Initializes the configure structure to zero. */
    memset(config, 0, sizeof(*config));

    config->width           = 320U;
    config->height          = 240U;
    config->polarityFlags   = kCSI_HsyncActiveHigh | kCSI_DataLatchOnRisingEdge;
    config->bytesPerPixel   = 2U;
    config->linePitch_Bytes = 320U * 2U;
    config->workMode        = kCSI_GatedClockMode;
    config->dataBus         = kCSI_DataBus8Bit;
    config->useExtVsync     = true;
}

/*!
 * brief Set the RX frame buffer address.
 *
 * param base CSI peripheral base address.
 * param index Buffer index.
 * param addr Frame buffer address to set.
 */
void CSI_SetRxBufferAddr(CSI_Type *base, uint8_t index, uint32_t addr)
{
    if (index)
    {
        base->CSIDMASA_FB2 = addr;
    }
    else
    {
        base->CSIDMASA_FB1 = addr;
    }
}

/*!
 * brief Clear the CSI FIFO.
 *
 * This function clears the CSI FIFO.
 *
 * param base CSI peripheral base address.
 * param fifo The FIFO to clear.
 */
void CSI_ClearFifo(CSI_Type *base, csi_fifo_t fifo)
{
    uint32_t cr1;
    uint32_t mask = 0U;

    /* The FIFO could only be cleared when CSICR1[FCC] = 0, so first clear the FCC. */
    cr1          = base->CSICR1;
    base->CSICR1 = (cr1 & ~CSI_CSICR1_FCC_MASK);

    if ((uint32_t)fifo & (uint32_t)kCSI_RxFifo)
    {
        mask |= CSI_CSICR1_CLR_RXFIFO_MASK;
    }

    if ((uint32_t)fifo & (uint32_t)kCSI_StatFifo)
    {
        mask |= CSI_CSICR1_CLR_STATFIFO_MASK;
    }

    base->CSICR1 = (cr1 & ~CSI_CSICR1_FCC_MASK) | mask;

    /* Wait clear completed. */
    while (base->CSICR1 & mask)
    {
    }

    /* Recover the FCC. */
    base->CSICR1 = cr1;
}

/*!
 * brief Reflash the CSI FIFO DMA.
 *
 * This function reflashes the CSI FIFO DMA.
 *
 * For RXFIFO, there are two frame buffers. When the CSI module started, it saves
 * the frames to frame buffer 0 then frame buffer 1, the two buffers will be
 * written by turns. After reflash DMA using this function, the CSI is reset to
 * save frame to buffer 0.
 *
 * param base CSI peripheral base address.
 * param fifo The FIFO DMA to reflash.
 */
void CSI_ReflashFifoDma(CSI_Type *base, csi_fifo_t fifo)
{
    uint32_t cr3 = 0U;

    if ((uint32_t)fifo & (uint32_t)kCSI_RxFifo)
    {
        cr3 |= CSI_CSICR3_DMA_REFLASH_RFF_MASK;
    }

    if ((uint32_t)fifo & (uint32_t)kCSI_StatFifo)
    {
        cr3 |= CSI_CSICR3_DMA_REFLASH_SFF_MASK;
    }

    base->CSICR3 |= cr3;

    /* Wait clear completed. */
    while (base->CSICR3 & cr3)
    {
    }
}

/*!
 * brief Enable or disable the CSI FIFO DMA request.
 *
 * param base CSI peripheral base address.
 * param fifo The FIFO DMA reques to enable or disable.
 * param enable True to enable, false to disable.
 */
void CSI_EnableFifoDmaRequest(CSI_Type *base, csi_fifo_t fifo, bool enable)
{
    uint32_t cr3 = 0U;

    if ((uint32_t)fifo & (uint32_t)kCSI_RxFifo)
    {
        cr3 |= CSI_CSICR3_DMA_REQ_EN_RFF_MASK;
    }

    if ((uint32_t)fifo & (uint32_t)kCSI_StatFifo)
    {
        cr3 |= CSI_CSICR3_DMA_REQ_EN_SFF_MASK;
    }

    if (enable)
    {
        base->CSICR3 |= cr3;
    }
    else
    {
        base->CSICR3 &= ~cr3;
    }
}

/*!
 * brief Enables CSI interrupt requests.
 *
 * param base CSI peripheral base address.
 * param mask The interrupts to enable, pass in as OR'ed value of ref _csi_interrupt_enable.
 */
void CSI_EnableInterrupts(CSI_Type *base, uint32_t mask)
{
    base->CSICR1 |= (mask & CSI_CSICR1_INT_EN_MASK);
    base->CSICR3 |= (mask & CSI_CSICR3_INT_EN_MASK);
    base->CSICR18 |= ((mask & CSI_CSICR18_INT_EN_MASK) >> 6U);
}

/*!
 * brief Disable CSI interrupt requests.
 *
 * param base CSI peripheral base address.
 * param mask The interrupts to disable, pass in as OR'ed value of ref _csi_interrupt_enable.
 */
void CSI_DisableInterrupts(CSI_Type *base, uint32_t mask)
{
    base->CSICR1 &= ~(mask & CSI_CSICR1_INT_EN_MASK);
    base->CSICR3 &= ~(mask & CSI_CSICR3_INT_EN_MASK);
    base->CSICR18 &= ~((mask & CSI_CSICR18_INT_EN_MASK) >> 6U);
}

#if !CSI_DRIVER_FRAG_MODE
/*!
 * brief Initializes the CSI handle.
 *
 * This function initializes CSI handle, it should be called before any other
 * CSI transactional functions.
 *
 * param base CSI peripheral base address.
 * param handle Pointer to the handle structure.
 * param callback Callback function for CSI transfer.
 * param userData Callback function parameter.
 *
 * retval kStatus_Success Handle created successfully.
 */
status_t CSI_TransferCreateHandle(CSI_Type *base,
                                  csi_handle_t *handle,
                                  csi_transfer_callback_t callback,
                                  void *userData)
{
    assert(handle);
    uint32_t instance;

    memset(handle, 0, sizeof(*handle));

    /* Set the callback and user data. */
    handle->callback = callback;
    handle->userData = userData;

    /* Get instance from peripheral base address. */
    instance = CSI_GetInstance(base);

    /* Save the handle in global variables to support the double weak mechanism. */
    s_csiHandle[instance] = handle;

    s_csiIsr = CSI_TransferHandleIRQ;

    /* Enable interrupt. */
    EnableIRQ(s_csiIRQ[instance]);

    return kStatus_Success;
}

/*!
 * brief Start the transfer using transactional functions.
 *
 * When the empty frame buffers have been submit to CSI driver using function
 * ref CSI_TransferSubmitEmptyBuffer, user could call this function to start
 * the transfer. The incoming frame will be saved to the empty frame buffer,
 * and user could be optionally notified through callback function.
 *
 * param base CSI peripheral base address.
 * param handle Pointer to the handle structure.
 *
 * retval kStatus_Success Started successfully.
 * retval kStatus_CSI_NoEmptyBuffer Could not start because no empty frame buffer in queue.
 */
status_t CSI_TransferStart(CSI_Type *base, csi_handle_t *handle)
{
    assert(handle);

    uint32_t emptyBufferCount;

    emptyBufferCount = CSI_TransferGetEmptyBufferCount(base, handle);

    if (emptyBufferCount < 2U)
    {
        return kStatus_CSI_NoEmptyBuffer;
    }

    handle->nextBufferIdx   = 0U;
    handle->activeBufferNum = 0U;

    /*
     * Write to memory from first completed frame.
     * DMA base addr switch at the edge of the first data of each frame, thus
     * if one frame is broken, it could be reset at the next frame.
     */
    base->CSICR18 = (base->CSICR18 & ~CSI_CSICR18_MASK_OPTION_MASK) | CSI_CSICR18_MASK_OPTION(0) |
                    CSI_CSICR18_BASEADDR_SWITCH_SEL_MASK | CSI_CSICR18_BASEADDR_SWITCH_EN_MASK;

    /* Load the frame buffer to CSI register, there are at least two empty buffers. */
    CSI_TransferLoadBufferToDevice(base, handle);
    CSI_TransferLoadBufferToDevice(base, handle);

    /* After reflash DMA, the CSI saves frame to frame buffer 0. */
    CSI_ReflashFifoDma(base, kCSI_RxFifo);

    handle->transferStarted = true;
    handle->transferOnGoing = true;

    CSI_EnableInterrupts(base, kCSI_RxBuffer1DmaDoneInterruptEnable | kCSI_RxBuffer0DmaDoneInterruptEnable);

    CSI_Start(base);

    return kStatus_Success;
}

/*!
 * brief Stop the transfer using transactional functions.
 *
 * The driver does not clean the full frame buffers in queue. In other words, after
 * calling this function, user still could get the full frame buffers in queue
 * using function ref CSI_TransferGetFullBuffer.
 *
 * param base CSI peripheral base address.
 * param handle Pointer to the handle structure.
 *
 * retval kStatus_Success Stoped successfully.
 */
status_t CSI_TransferStop(CSI_Type *base, csi_handle_t *handle)
{
    assert(handle);

    CSI_Stop(base);
    CSI_DisableInterrupts(base, kCSI_RxBuffer1DmaDoneInterruptEnable | kCSI_RxBuffer0DmaDoneInterruptEnable);

    handle->transferStarted = false;
    handle->transferOnGoing = false;

    /* Stoped, reset the state flags. */
    handle->queueDrvReadIdx = handle->queueDrvWriteIdx;
    handle->activeBufferNum = 0U;

    return kStatus_Success;
}

/*!
 * brief Submit empty frame buffer to queue.
 *
 * This function could be called before ref CSI_TransferStart or after ref
 * CSI_TransferStart. If there is no room in queue to store the empty frame
 * buffer, this function returns error.
 *
 * param base CSI peripheral base address.
 * param handle Pointer to the handle structure.
 * param frameBuffer Empty frame buffer to submit.
 *
 * retval kStatus_Success Started successfully.
 * retval kStatus_CSI_QueueFull Could not submit because there is no room in queue.
 */
status_t CSI_TransferSubmitEmptyBuffer(CSI_Type *base, csi_handle_t *handle, uint32_t frameBuffer)
{
    uint32_t csicr1;

    if (CSI_DRIVER_QUEUE_SIZE == CSI_TransferGetQueueDelta(handle->queueUserReadIdx, handle->queueUserWriteIdx))
    {
        return kStatus_CSI_QueueFull;
    }

    /* Disable the interrupt to protect the index information in handle. */
    csicr1 = base->CSICR1;

    base->CSICR1 = (csicr1 & ~(CSI_CSICR1_FB2_DMA_DONE_INTEN_MASK | CSI_CSICR1_FB1_DMA_DONE_INTEN_MASK));

    /* Save the empty frame buffer address to queue. */
    handle->frameBufferQueue[handle->queueUserWriteIdx] = frameBuffer;
    handle->queueUserWriteIdx                           = CSI_TransferIncreaseQueueIdx(handle->queueUserWriteIdx);

    /*
     * If transfer is ongoing and an active slot is available, Load the buffer
     * now to prevent buffer starvation during next TransferHandleIRQ event.
     */
    if (handle->transferOnGoing && handle->activeBufferNum < CSI_MAX_ACTIVE_FRAME_NUM)
    {
        CSI_TransferLoadBufferToDevice(base, handle);
    }

    base->CSICR1 = csicr1;

    if (handle->transferStarted)
    {
        /*
         * If user has started transfer using @ref CSI_TransferStart, and the CSI is
         * stopped due to no empty frame buffer in queue, then start the CSI.
         */
        if ((!handle->transferOnGoing) && (CSI_TransferGetEmptyBufferCount(base, handle) >= 2U))
        {
            handle->transferOnGoing = true;
            handle->nextBufferIdx   = 0U;

            /* Load the frame buffers to CSI module. */
            CSI_TransferLoadBufferToDevice(base, handle);
            CSI_TransferLoadBufferToDevice(base, handle);
            CSI_ReflashFifoDma(base, kCSI_RxFifo);
            CSI_Start(base);
        }
    }

    return kStatus_Success;
}

/*!
 * brief Get one full frame buffer from queue.
 *
 * After the transfer started using function ref CSI_TransferStart, the incoming
 * frames will be saved to the empty frame buffers in queue. This function gets
 * the full-filled frame buffer from the queue. If there is no full frame buffer
 * in queue, this function returns error.
 *
 * param base CSI peripheral base address.
 * param handle Pointer to the handle structure.
 * param frameBuffer Full frame buffer.
 *
 * retval kStatus_Success Started successfully.
 * retval kStatus_CSI_NoFullBuffer There is no full frame buffer in queue.
 */
status_t CSI_TransferGetFullBuffer(CSI_Type *base, csi_handle_t *handle, uint32_t *frameBuffer)
{
    uint32_t csicr1;

    /* No full frame buffer. */
    if (handle->queueUserReadIdx == handle->queueDrvWriteIdx)
    {
        return kStatus_CSI_NoFullBuffer;
    }

    /* Disable the interrupt to protect the index information in handle. */
    csicr1 = base->CSICR1;

    base->CSICR1 = (csicr1 & ~(CSI_CSICR1_FB2_DMA_DONE_INTEN_MASK | CSI_CSICR1_FB1_DMA_DONE_INTEN_MASK));

    *frameBuffer = handle->frameBufferQueue[handle->queueUserReadIdx];

    handle->queueUserReadIdx = CSI_TransferIncreaseQueueIdx(handle->queueUserReadIdx);

    base->CSICR1 = csicr1;

    return kStatus_Success;
}

/*!
 * brief CSI IRQ handle function.
 *
 * This function handles the CSI IRQ request to work with CSI driver transactional
 * APIs.
 *
 * param base CSI peripheral base address.
 * param handle CSI handle pointer.
 */
void CSI_TransferHandleIRQ(CSI_Type *base, csi_handle_t *handle)
{
    uint32_t queueDrvWriteIdx;
    uint32_t csisr = base->CSISR;

    /* Clear the error flags. */
    base->CSISR = csisr;

    /*
     * If both frame buffer 0 and frame buffer 1 flags assert, driver does not
     * know which frame buffer ready just now, so reset the CSI transfer to
     * start from frame buffer 0.
     */
    if ((csisr & (CSI_CSISR_DMA_TSF_DONE_FB2_MASK | CSI_CSISR_DMA_TSF_DONE_FB1_MASK)) ==
        (CSI_CSISR_DMA_TSF_DONE_FB2_MASK | CSI_CSISR_DMA_TSF_DONE_FB1_MASK))
    {
        CSI_Stop(base);

        /* Reset the active buffers. */
        if (1 <= handle->activeBufferNum)
        {
            queueDrvWriteIdx = handle->queueDrvWriteIdx;

            base->CSIDMASA_FB1 = handle->frameBufferQueue[queueDrvWriteIdx];

            if (2U == handle->activeBufferNum)
            {
                queueDrvWriteIdx      = CSI_TransferIncreaseQueueIdx(queueDrvWriteIdx);
                base->CSIDMASA_FB2    = handle->frameBufferQueue[queueDrvWriteIdx];
                handle->nextBufferIdx = 0U;
            }
            else
            {
                handle->nextBufferIdx = 1U;
            }
        }
        CSI_ReflashFifoDma(base, kCSI_RxFifo);
        CSI_Start(base);
    }
    else if (csisr & (CSI_CSISR_DMA_TSF_DONE_FB2_MASK | CSI_CSISR_DMA_TSF_DONE_FB1_MASK))
    {
        handle->queueDrvWriteIdx = CSI_TransferIncreaseQueueIdx(handle->queueDrvWriteIdx);

        handle->activeBufferNum--;

        if (handle->callback)
        {
            handle->callback(base, handle, kStatus_CSI_FrameDone, handle->userData);
        }

        /* No frame buffer to save incoming data, then stop the CSI module. */
        if (!(handle->activeBufferNum))
        {
            CSI_Stop(base);
            handle->transferOnGoing = false;
        }
        else
        {
            if (CSI_TransferGetEmptyBufferCount(base, handle))
            {
                CSI_TransferLoadBufferToDevice(base, handle);
            }
        }
    }
    else
    {
    }
}

#else /* CSI_DRIVER_FRAG_MODE */

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
__asm void CSI_ExtractYFromYUYV(void *datBase, const void *dmaBase, size_t count)
{
    /* clang-format off */
    push    {r4-r7, lr}
10
    LDMIA    R1!, {r3-r6}
    bfi      r7, r3, #0, #8  /* Y0 */
    bfi      ip, r5, #0, #8  /* Y4 */
    lsr      r3, r3, #16
    lsr      r5, r5, #16
    bfi      r7, r3, #8, #8  /* Y1 */
    bfi      ip, r5, #8, #8  /* Y5 */
    bfi      r7, r4, #16, #8 /* Y2 */
    bfi      ip, r6, #16, #8 /* Y6 */
    lsr      r4, r4, #16
    lsr      r6, r6, #16
    bfi      r7, r4, #24, #8 /* Y3 */
    bfi      ip, r6, #24, #8 /* Y7 */
    STMIA    r0!, {r7, ip}
    subs     r2, #8
    bne      %b10
    pop      {r4-r7, pc}
    /* clang-format on */
}

__asm void CSI_ExtractYFromUYVY(void *datBase, const void *dmaBase, size_t count)
{
    /* clang-format off */
    push    {r4-r7, lr}
10
    LDMIA    R1!, {r3-r6}
    lsr      r3, r3, #8
    lsr      r5, r5, #8
    bfi      r7, r3, #0, #8  /* Y0 */
    bfi      ip, r5, #0, #8  /* Y4 */
    lsr      r3, r3, #16
    lsr      r5, r5, #16
    bfi      r7, r3, #8, #8  /* Y1 */
    bfi      ip, r5, #8, #8  /* Y5 */
    lsr      r4, r4, #8
    lsr      r6, r6, #8
    bfi      r7, r4, #16, #8 /* Y2 */
    bfi      ip, r6, #16, #8 /* Y6 */
    lsr      r4, r4, #16
    lsr      r6, r6, #16
    bfi      r7, r4, #24, #8 /* Y3 */
    bfi      ip, r6, #24, #8 /* Y7 */
    STMIA    r0!, {r7, ip}
    subs     r2, #8
    bne      %b10
    pop      {r4-r7, pc}
    /* clang-format on */
}

#elif (defined(__GNUC__) || defined(__ICCARM__))
#if defined(__ICCARM__)
#pragma diag_suppress = Pe940
#endif
__attribute__((naked)) void CSI_ExtractYFromYUYV(void *datBase, const void *dmaBase, size_t count)
{
    /* clang-format off */
    __asm volatile(
        "    push    {r1-r7, r12, lr}  \n"
        "loop0:                        \n"
        "    ldmia   r1!, {r3-r6}      \n"
        "    bfi     r7, r3, #0, #8    \n" /* Y0 */
        "    bfi     r12, r5, #0, #8   \n" /* Y4 */
        "    lsr     r3, r3, #16       \n"
        "    lsr     r5, r5, #16       \n"
        "    bfi     r7, r3, #8, #8    \n" /* Y1 */
        "    bfi     r12, r5, #8, #8   \n" /* Y5 */
        "    bfi     r7, r4, #16, #8   \n" /* Y2 */
        "    bfi     r12, r6, #16, #8  \n" /* Y6 */
        "    lsr     r4, r4, #16       \n"
        "    lsr     r6, r6, #16       \n"
        "    bfi     r7, r4, #24, #8   \n" /* Y3 */
        "    bfi     r12, r6, #24, #8  \n" /* Y7 */
        "    stmia   r0!, {r7, r12}    \n"
        "    subs    r2, #8            \n"
        "    bne     loop0             \n"
        "    pop     {r1-r7, r12, pc}  \n");
    /* clang-format on */
}

__attribute__((naked)) void CSI_ExtractYFromUYVY(void *datBase, const void *dmaBase, size_t count)
{
    /* clang-format off */
    __asm volatile(
        "    push    {r1-r7, r12, lr}  \n"
        "loop1:                        \n"
        "    ldmia   r1!, {r3-r6}      \n"
        "    lsr     r3, r3, #8        \n"
        "    lsr     r5, r5, #8        \n"
        "    bfi     r7, r3, #0, #8    \n" /* Y0 */
        "    bfi     r12, r5, #0, #8   \n" /* Y4 */
        "    lsr     r3, r3, #16       \n"
        "    lsr     r5, r5, #16       \n"
        "    bfi     r7, r3, #8, #8    \n" /* Y1 */
        "    bfi     r12, r5, #8, #8   \n" /* Y5 */
        "    lsr     r4, r4, #8        \n"
        "    lsr     r6, r6, #8        \n"
        "    bfi     r7, r4, #16, #8   \n" /* Y2 */
        "    bfi     r12, r6, #16, #8  \n" /* Y6 */
        "    lsr     r4, r4, #16       \n"
        "    lsr     r6, r6, #16       \n"
        "    bfi     r7, r4, #24, #8   \n" /* Y3 */
        "    bfi     r12, r6, #24, #8  \n" /* Y7 */
        "    stmia   r0!, {r7, r12}    \n"
        "    subs    r2, #8            \n"
        "    bne     loop1             \n"
        "    pop     {r1-r7, r12, pc}  \n");
    /* clang-format on */
}
#if defined(__ICCARM__)
#pragma diag_default = Pe940
#endif
#else
#error Toolchain not supported.
#endif

static void CSI_MemCopy(void *pDest, const void *pSrc, size_t cnt)
{
    memcpy(pDest, pSrc, cnt);
}

/*!
 * brief Initialize the CSI to work in fragment mode.
 *
 * This function enables the CSI peripheral clock, and resets the CSI registers.
 *
 * param base CSI peripheral base address.
 */
void CSI_FragModeInit(CSI_Type *base)
{
#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    uint32_t instance = CSI_GetInstance(base);
    CLOCK_EnableClock(s_csiClocks[instance]);
#endif

    CSI_Reset(base);
}

/*!
 * brief De-initialize the CSI.
 *
 * This function disables the CSI peripheral clock.
 *
 * param base CSI peripheral base address.
 */
void CSI_FragModeDeinit(CSI_Type *base)
{
    CSI_Deinit(base);
}

/*!
 * brief Create handle for CSI work in fragment mode.
 *
 * param base CSI peripheral base address.
 * param handle Pointer to the transactional handle.
 * param config Pointer to the configuration structure.
 * param callback Callback function for CSI transfer.
 * param userData Callback function parameter.
 *
 * retval kStatus_Success Initialize successfully.
 * retval kStatus_InvalidArgument Initialize failed because of invalid argument.
 */
status_t CSI_FragModeCreateHandle(CSI_Type *base,
                                  csi_frag_handle_t *handle,
                                  const csi_frag_config_t *config,
                                  csi_frag_transfer_callback_t callback,
                                  void *userData)
{
    assert(config);
    uint32_t reg;
    uint32_t instance;
    uint32_t imgWidth_Bytes;

    imgWidth_Bytes = config->width * CSI_FRAG_INPUT_BYTES_PER_PIXEL;

    /* The image buffer line width should be multiple of 8-bytes. */
    if ((imgWidth_Bytes & 0x07) != 0)
    {
        return kStatus_InvalidArgument;
    }

    /* Camera frame height must be dividable by DMA buffer line. */
    if (config->height % config->dmaBufferLine != 0)
    {
        return kStatus_InvalidArgument;
    }

    memset(handle, 0, sizeof(*handle));
    handle->callback            = callback;
    handle->userData            = userData;
    handle->height              = config->height;
    handle->width               = config->width;
    handle->maxLinePerFrag      = config->dmaBufferLine;
    handle->dmaBytePerLine      = config->width * CSI_FRAG_INPUT_BYTES_PER_PIXEL;
    handle->isDmaBufferCachable = config->isDmaBufferCachable;

    /* Get instance from peripheral base address. */
    instance = CSI_GetInstance(base);
    /* Save the handle in global variables to support the double weak mechanism. */
    s_csiHandle[instance] = handle;

    s_csiIsr = CSI_FragModeTransferHandleIRQ;

    EnableIRQ(s_csiIRQ[instance]);

    /* Configure CSICR1. CSICR1 has been reset to the default value, so could write it directly. */
    reg = ((uint32_t)config->workMode) | config->polarityFlags | CSI_CSICR1_FCC_MASK;

    if (config->useExtVsync)
    {
        reg |= CSI_CSICR1_EXT_VSYNC_MASK;
    }

    base->CSICR1 = reg;

    /* No stride. */
    base->CSIFBUF_PARA = 0;

    /* Enable auto ECC. */
    base->CSICR3 |= CSI_CSICR3_ECC_AUTO_EN_MASK;

    /*
     * For better performance.
     * The DMA burst size could be set to 16 * 8 byte, 8 * 8 byte, or 4 * 8 byte,
     * choose the best burst size based on bytes per line.
     */
    if (!(imgWidth_Bytes % (8 * 16)))
    {
        base->CSICR2 = CSI_CSICR2_DMA_BURST_TYPE_RFF(3U);
        base->CSICR3 = (CSI->CSICR3 & ~CSI_CSICR3_RxFF_LEVEL_MASK) | ((2U << CSI_CSICR3_RxFF_LEVEL_SHIFT));
    }
    else if (!(imgWidth_Bytes % (8 * 8)))
    {
        base->CSICR2 = CSI_CSICR2_DMA_BURST_TYPE_RFF(2U);
        base->CSICR3 = (CSI->CSICR3 & ~CSI_CSICR3_RxFF_LEVEL_MASK) | ((1U << CSI_CSICR3_RxFF_LEVEL_SHIFT));
    }
    else
    {
        base->CSICR2 = CSI_CSICR2_DMA_BURST_TYPE_RFF(1U);
        base->CSICR3 = (CSI->CSICR3 & ~CSI_CSICR3_RxFF_LEVEL_MASK) | ((0U << CSI_CSICR3_RxFF_LEVEL_SHIFT));
    }

    base->CSIDMASA_FB1 = config->dmaBufferAddr0;
    base->CSIDMASA_FB2 = config->dmaBufferAddr1;

    if (handle->isDmaBufferCachable)
    {
        DCACHE_CleanInvalidateByRange(config->dmaBufferAddr0,
                                      config->dmaBufferLine * config->width * CSI_FRAG_INPUT_BYTES_PER_PIXEL);
        DCACHE_CleanInvalidateByRange(config->dmaBufferAddr1,
                                      config->dmaBufferLine * config->width * CSI_FRAG_INPUT_BYTES_PER_PIXEL);
    }

    return kStatus_Success;
}

/*!
 * brief Start to capture a image.
 *
 * param base CSI peripheral base address.
 * param handle Pointer to the transactional handle.
 * param config Pointer to the capture configuration.
 *
 * retval kStatus_Success Initialize successfully.
 * retval kStatus_InvalidArgument Initialize failed because of invalid argument.
 */
status_t CSI_FragModeTransferCaptureImage(CSI_Type *base,
                                          csi_frag_handle_t *handle,
                                          const csi_frag_capture_config_t *config)
{
    assert(config);

    uint16_t windowWidth;

    /*
     * If no special window setting, capture full frame.
     * If capture window, then capture 1 one each fragment.
     */
    if (config->window != NULL)
    {
        handle->windowULX   = config->window->windowULX;
        handle->windowULY   = config->window->windowULY;
        handle->windowLRX   = config->window->windowLRX;
        handle->windowLRY   = config->window->windowLRY;
        handle->linePerFrag = 1;
    }
    else
    {
        handle->windowULX   = 0;
        handle->windowULY   = 0;
        handle->windowLRX   = handle->width - 1;
        handle->windowLRY   = handle->height - 1;
        handle->linePerFrag = handle->maxLinePerFrag;
    }

    windowWidth = handle->windowLRX - handle->windowULX + 1;

    if (config->outputGrayScale)
    {
        /* When output format is gray, the window width must be multiple value of 8. */
        if (windowWidth % 8 != 0)
        {
            return kStatus_InvalidArgument;
        }

        handle->datBytePerLine = windowWidth;
        if (handle->inputFormat == kCSI_FragInputYUYV)
        {
            handle->copyFunc = CSI_ExtractYFromYUYV;
        }
        else
        {
            handle->copyFunc = CSI_ExtractYFromUYVY;
        }
    }
    else
    {
        handle->datBytePerLine = windowWidth * CSI_FRAG_INPUT_BYTES_PER_PIXEL;
        handle->copyFunc       = CSI_MemCopy;
    }

    handle->dmaCurLine      = 0;
    handle->outputBuffer    = (uint32_t)config->buffer;
    handle->datCurWriteAddr = (uint32_t)config->buffer;

    /* Image parameter. */
    base->CSIIMAG_PARA =
        ((uint32_t)(handle->width * CSI_FRAG_INPUT_BYTES_PER_PIXEL) << CSI_CSIIMAG_PARA_IMAGE_WIDTH_SHIFT) |
        ((uint32_t)(handle->linePerFrag) << CSI_CSIIMAG_PARA_IMAGE_HEIGHT_SHIFT);

    /*
     * Write to memory from first completed frame.
     * DMA base addr switch at dma transfer done.
     */
    base->CSICR18 = (base->CSICR18 & ~CSI_CSICR18_MASK_OPTION_MASK) | CSI_CSICR18_MASK_OPTION(0);

    CSI_EnableInterrupts(base, kCSI_StartOfFrameInterruptEnable | kCSI_RxBuffer1DmaDoneInterruptEnable |
                                   kCSI_RxBuffer0DmaDoneInterruptEnable);

    return kStatus_Success;
}

/*!
 * brief Abort image capture.
 *
 * Abort image capture initialized by ref CSI_FragModeTransferCaptureImage.
 *
 * param base CSI peripheral base address.
 * param handle Pointer to the transactional handle.
 */
void CSI_FragModeTransferAbortCaptureImage(CSI_Type *base, csi_frag_handle_t *handle)
{
    CSI_Stop(base);
    CSI_DisableInterrupts(base, kCSI_StartOfFrameInterruptEnable | kCSI_RxBuffer1DmaDoneInterruptEnable |
                                    kCSI_RxBuffer0DmaDoneInterruptEnable);
}

/*!
 * brief CSI IRQ handle function.
 *
 * This function handles the CSI IRQ request to work with CSI driver fragment mode
 * APIs.
 *
 * param base CSI peripheral base address.
 * param handle CSI handle pointer.
 */
void CSI_FragModeTransferHandleIRQ(CSI_Type *base, csi_frag_handle_t *handle)
{
    uint32_t csisr = base->CSISR;
    uint32_t dmaBufAddr;
    uint16_t line;

    /* Clear the error flags. */
    base->CSISR = csisr;

    /* Start of frame, clear the FIFO and start receiving. */
    if (csisr & kCSI_StartOfFrameFlag)
    {
        /* Reflash the DMA and enable RX DMA request. */
        base->CSICR3 |= (CSI_CSICR3_DMA_REFLASH_RFF_MASK | CSI_CSICR3_DMA_REQ_EN_RFF_MASK);
        CSI_Start(base);
        handle->dmaCurLine      = 0;
        handle->datCurWriteAddr = handle->outputBuffer;
    }
    else if ((csisr & (CSI_CSISR_DMA_TSF_DONE_FB2_MASK | CSI_CSISR_DMA_TSF_DONE_FB1_MASK)) != 0)
    {
        if ((csisr & CSI_CSISR_DMA_TSF_DONE_FB1_MASK) == CSI_CSISR_DMA_TSF_DONE_FB1_MASK)
        {
            dmaBufAddr = base->CSIDMASA_FB1;
        }
        else
        {
            dmaBufAddr = base->CSIDMASA_FB2;
        }

        if (handle->isDmaBufferCachable)
        {
            DCACHE_InvalidateByRange(dmaBufAddr, handle->dmaBytePerLine * handle->linePerFrag);
        }

        /* Copy from DMA buffer to user data buffer. */
        dmaBufAddr += (handle->windowULX * CSI_FRAG_INPUT_BYTES_PER_PIXEL);

        for (line = 0; line < handle->linePerFrag; line++)
        {
            if (handle->dmaCurLine + line > handle->windowLRY)
            {
                /* out of window range */
                break;
            }
            else if (handle->dmaCurLine + line >= handle->windowULY)
            {
                handle->copyFunc((void *)(handle->datCurWriteAddr), (void const *)dmaBufAddr, handle->datBytePerLine);
                handle->datCurWriteAddr += handle->datBytePerLine;
                dmaBufAddr += handle->dmaBytePerLine;
            }
            else
            {
            }
        }

        handle->dmaCurLine += handle->linePerFrag;

        if (handle->dmaCurLine >= handle->height)
        {
            CSI_Stop(base);
            CSI_DisableInterrupts(base, kCSI_StartOfFrameInterruptEnable | kCSI_RxBuffer1DmaDoneInterruptEnable |
                                            kCSI_RxBuffer0DmaDoneInterruptEnable);

            /* Image captured. Stop the CSI. */
            if (handle->callback)
            {
                handle->callback(base, handle, kStatus_CSI_FrameDone, handle->userData);
            }
        }
    }
    else
    {
    }
}
#endif /* CSI_DRIVER_FRAG_MODE */

#if defined(CSI)
void CSI_DriverIRQHandler(void)
{
    s_csiIsr(CSI, s_csiHandle[0]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif

#if defined(CSI0)
void CSI0_DriverIRQHandler(void)
{
    s_csiIsr(CSI, s_csiHandle[0]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif
