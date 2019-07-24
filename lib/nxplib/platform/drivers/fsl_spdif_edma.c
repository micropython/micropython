/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_spdif_edma.h"

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.spdif_edma"
#endif

/*******************************************************************************
 * Definitations
 ******************************************************************************/
/* Used for 32byte aligned */
#define STCD_ADDR(address) (edma_tcd_t *)(((uint32_t)address + 32) & ~0x1FU)

/*<! Structure definition for uart_edma_private_handle_t. The structure is private. */
typedef struct _spdif_edma_private_handle
{
    SPDIF_Type *base;
    spdif_edma_handle_t *handle;
} spdif_edma_private_handle_t;

enum _spdif_edma_transfer_state
{
    kSPDIF_Busy = 0x0U, /*!< SPDIF is busy */
    kSPDIF_Idle,        /*!< Transfer is done. */
};

/*<! Private handle only used for internally. */
static spdif_edma_private_handle_t s_edmaPrivateHandle[FSL_FEATURE_SOC_SPDIF_COUNT][2];
static uint8_t s_spdif_tx_watermark[4] = {16, 12, 8, 4};
static uint8_t s_spdif_rx_watermark[4] = {1, 4, 8, 16};

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief Get the instance number for SPDIF.
 *
 * @param base SPDIF base pointer.
 */
extern uint32_t SPDIF_GetInstance(SPDIF_Type *base);

/*!
 * @brief Submit SPDIF tcds to EDMA.
 *
 * @param base SPDIF base pointer.
 */
static status_t SPDIF_SubmitTransfer(edma_handle_t *handle,
                                     const edma_transfer_config_t *config,
                                     uint32_t rightChannel);

/*!
 * @brief SPDIF EDMA callback for send.
 *
 * @param handle pointer to spdif_edma_handle_t structure which stores the transfer state.
 * @param userData Parameter for user callback.
 * @param done If the DMA transfer finished.
 * @param tcds The TCD index.
 */
static void SPDIF_TxEDMACallback(edma_handle_t *handle, void *userData, bool done, uint32_t tcds);

/*!
 * @brief SPDIF EDMA callback for receive.
 *
 * @param handle pointer to spdif_edma_handle_t structure which stores the transfer state.
 * @param userData Parameter for user callback.
 * @param done If the DMA transfer finished.
 * @param tcds The TCD index.
 */
static void SPDIF_RxEDMACallback(edma_handle_t *handle, void *userData, bool done, uint32_t tcds);

/*******************************************************************************
 * Code
 ******************************************************************************/
static void SPDIF_TxEDMACallback(edma_handle_t *handle, void *userData, bool done, uint32_t tcds)
{
    spdif_edma_private_handle_t *privHandle = (spdif_edma_private_handle_t *)userData;
    spdif_edma_handle_t *spdifHandle        = privHandle->handle;

    /* If finished a block, call the callback function */
    memset(&spdifHandle->spdifQueue[spdifHandle->queueDriver], 0, sizeof(spdif_edma_transfer_t));
    spdifHandle->queueDriver = (spdifHandle->queueDriver + 1) % SPDIF_XFER_QUEUE_SIZE;
    if (spdifHandle->callback)
    {
        (spdifHandle->callback)(privHandle->base, spdifHandle, kStatus_SPDIF_TxIdle, spdifHandle->userData);
    }

    /* If all data finished, just stop the transfer */
    if (spdifHandle->spdifQueue[spdifHandle->queueDriver].rightData == NULL)
    {
        SPDIF_TransferAbortSendEDMA(privHandle->base, spdifHandle);
    }
}

static void SPDIF_RxEDMACallback(edma_handle_t *handle, void *userData, bool done, uint32_t tcds)
{
    spdif_edma_private_handle_t *privHandle = (spdif_edma_private_handle_t *)userData;
    spdif_edma_handle_t *spdifHandle        = privHandle->handle;

    /* If finished a block, call the callback function */
    memset(&spdifHandle->spdifQueue[spdifHandle->queueDriver], 0, sizeof(spdif_edma_transfer_t));
    spdifHandle->queueDriver = (spdifHandle->queueDriver + 1) % SPDIF_XFER_QUEUE_SIZE;
    if (spdifHandle->callback)
    {
        (spdifHandle->callback)(privHandle->base, spdifHandle, kStatus_SPDIF_RxIdle, spdifHandle->userData);
    }

    /* If all data finished, just stop the transfer */
    if (spdifHandle->spdifQueue[spdifHandle->queueDriver].rightData == NULL)
    {
        SPDIF_TransferAbortReceiveEDMA(privHandle->base, spdifHandle);
    }
}

static status_t SPDIF_SubmitTransfer(edma_handle_t *handle, const edma_transfer_config_t *config, uint32_t rightChannel)
{
    edma_tcd_t *tcdRegs = (edma_tcd_t *)&handle->base->TCD[handle->channel];
    uint32_t primask;
    uint32_t csr;
    int8_t currentTcd;
    int8_t previousTcd;
    int8_t nextTcd;

    /* Check if tcd pool is full. */
    primask = DisableGlobalIRQ();
    if (handle->tcdUsed >= handle->tcdSize)
    {
        EnableGlobalIRQ(primask);

        return kStatus_EDMA_QueueFull;
    }
    currentTcd = handle->tail;
    handle->tcdUsed++;
    /* Calculate index of next TCD */
    nextTcd = currentTcd + 1U;
    if (nextTcd == handle->tcdSize)
    {
        nextTcd = 0U;
    }
    /* Advance queue tail index */
    handle->tail = nextTcd;
    EnableGlobalIRQ(primask);
    /* Calculate index of previous TCD */
    previousTcd = currentTcd ? currentTcd - 1U : handle->tcdSize - 1U;
    /* Configure current TCD block. */
    EDMA_TcdReset(&handle->tcdPool[currentTcd]);
    EDMA_TcdSetTransferConfig(&handle->tcdPool[currentTcd], config, NULL);
    /* Set channel link */
    EDMA_TcdSetChannelLink(&handle->tcdPool[currentTcd], kEDMA_MinorLink, rightChannel);
    EDMA_TcdSetChannelLink(&handle->tcdPool[currentTcd], kEDMA_MajorLink, rightChannel);
    /* Enable major interrupt */
    handle->tcdPool[currentTcd].CSR |= DMA_CSR_INTMAJOR_MASK;
    /* Link current TCD with next TCD for identification of current TCD */
    handle->tcdPool[currentTcd].DLAST_SGA = (uint32_t)&handle->tcdPool[nextTcd];
    /* Chain from previous descriptor unless tcd pool size is 1(this descriptor is its own predecessor). */
    if (currentTcd != previousTcd)
    {
        /* Enable scatter/gather feature in the previous TCD block. */
        csr                              = (handle->tcdPool[previousTcd].CSR | DMA_CSR_ESG_MASK) & ~DMA_CSR_DREQ_MASK;
        handle->tcdPool[previousTcd].CSR = csr;
        /*
            Check if the TCD block in the registers is the previous one (points to current TCD block). It
            is used to check if the previous TCD linked has been loaded in TCD register. If so, it need to
            link the TCD register in case link the current TCD with the dead chain when TCD loading occurs
            before link the previous TCD block.
        */
        if (tcdRegs->DLAST_SGA == (uint32_t)&handle->tcdPool[currentTcd])
        {
            /* Enable scatter/gather also in the TCD registers. */
            csr = (tcdRegs->CSR | DMA_CSR_ESG_MASK) & ~DMA_CSR_DREQ_MASK;
            /* Must write the CSR register one-time, because the transfer maybe finished anytime. */
            tcdRegs->CSR = csr;
            /*
                It is very important to check the ESG bit!
                Because this hardware design: if DONE bit is set, the ESG bit can not be set. So it can
                be used to check if the dynamic TCD link operation is successful. If ESG bit is not set
                and the DLAST_SGA is not the next TCD address(it means the dynamic TCD link succeed and
                the current TCD block has been loaded into TCD registers), it means transfer finished
                and TCD link operation fail, so must install TCD content into TCD registers and enable
                transfer again. And if ESG is set, it means transfer has notfinished, so TCD dynamic
                link succeed.
            */
            if (tcdRegs->CSR & DMA_CSR_ESG_MASK)
            {
                return kStatus_Success;
            }
            /*
                Check whether the current TCD block is already loaded in the TCD registers. It is another
                condition when ESG bit is not set: it means the dynamic TCD link succeed and the current
                TCD block has been loaded into TCD registers.
            */
            if (tcdRegs->DLAST_SGA == (uint32_t)&handle->tcdPool[nextTcd])
            {
                return kStatus_Success;
            }
            /*
                If go to this, means the previous transfer finished, and the DONE bit is set.
                So shall configure TCD registers.
            */
        }
        else if (tcdRegs->DLAST_SGA != 0)
        {
            /* The current TCD block has been linked successfully. */
            return kStatus_Success;
        }
        else
        {
            /*
                DLAST_SGA is 0 and it means the first submit transfer, so shall configure
                TCD registers.
            */
        }
    }
    /* There is no live chain, TCD block need to be installed in TCD registers. */
    EDMA_InstallTCD(handle->base, handle->channel, &handle->tcdPool[currentTcd]);
    /* Enable channel request again. */
    if (handle->flags & 0x80)
    {
        handle->base->SERQ = DMA_SERQ_SERQ(handle->channel);
    }

    return kStatus_Success;
}

/*!
 * brief Initializes the SPDIF eDMA handle.
 *
 * This function initializes the SPDIF master DMA handle, which can be used for other SPDIF master transactional APIs.
 * Usually, for a specified SPDIF instance, call this API once to get the initialized handle.
 *
 * param base SPDIF base pointer.
 * param handle SPDIF eDMA handle pointer.
 * param base SPDIF peripheral base address.
 * param callback Pointer to user callback function.
 * param userData User parameter passed to the callback function.
 * param dmaLeftHandle eDMA handle pointer for left channel, this handle shall be static allocated by users.
 * param dmaRightHandle eDMA handle pointer for right channel, this handle shall be static allocated by users.
 */
void SPDIF_TransferTxCreateHandleEDMA(SPDIF_Type *base,
                                      spdif_edma_handle_t *handle,
                                      spdif_edma_callback_t callback,
                                      void *userData,
                                      edma_handle_t *dmaLeftHandle,
                                      edma_handle_t *dmaRightHandle)
{
    assert(handle && dmaLeftHandle && dmaRightHandle);

    uint32_t instance = SPDIF_GetInstance(base);

    /* Zero the handle */
    memset(handle, 0, sizeof(*handle));

    /* Set spdif base to handle */
    handle->dmaLeftHandle  = dmaLeftHandle;
    handle->dmaRightHandle = dmaRightHandle;
    handle->callback       = callback;
    handle->userData       = userData;
    handle->count =
        s_spdif_tx_watermark[(base->SCR & SPDIF_SCR_TXFIFOEMPTY_SEL_MASK) >> SPDIF_SCR_TXFIFOEMPTY_SEL_SHIFT];

    /* Set SPDIF state to idle */
    handle->state = kSPDIF_Idle;

    s_edmaPrivateHandle[instance][0].base   = base;
    s_edmaPrivateHandle[instance][0].handle = handle;

    /* Need to use scatter gather */
    EDMA_InstallTCDMemory(dmaLeftHandle, STCD_ADDR(handle->leftTcd), SPDIF_XFER_QUEUE_SIZE);
    EDMA_InstallTCDMemory(dmaRightHandle, STCD_ADDR(handle->rightTcd), SPDIF_XFER_QUEUE_SIZE);

    /* Install callback for Tx dma channel, only right channel finished, a transfer finished */
    EDMA_SetCallback(dmaRightHandle, SPDIF_TxEDMACallback, &s_edmaPrivateHandle[instance][0]);
}

/*!
 * brief Initializes the SPDIF Rx eDMA handle.
 *
 * This function initializes the SPDIF slave DMA handle, which can be used for other SPDIF master transactional APIs.
 * Usually, for a specified SPDIF instance, call this API once to get the initialized handle.
 *
 * param base SPDIF base pointer.
 * param handle SPDIF eDMA handle pointer.
 * param base SPDIF peripheral base address.
 * param callback Pointer to user callback function.
 * param userData User parameter passed to the callback function.
 * param dmaLeftHandle eDMA handle pointer for left channel, this handle shall be static allocated by users.
 * param dmaRightHandle eDMA handle pointer for right channel, this handle shall be static allocated by users.
 */
void SPDIF_TransferRxCreateHandleEDMA(SPDIF_Type *base,
                                      spdif_edma_handle_t *handle,
                                      spdif_edma_callback_t callback,
                                      void *userData,
                                      edma_handle_t *dmaLeftHandle,
                                      edma_handle_t *dmaRightHandle)
{
    assert(handle && dmaLeftHandle && dmaRightHandle);

    uint32_t instance = SPDIF_GetInstance(base);

    /* Zero the handle */
    memset(handle, 0, sizeof(*handle));

    /* Set spdif base to handle */
    handle->dmaLeftHandle  = dmaLeftHandle;
    handle->dmaRightHandle = dmaRightHandle;
    handle->callback       = callback;
    handle->userData       = userData;
    handle->count = s_spdif_rx_watermark[(base->SCR & SPDIF_SCR_RXFIFOFULL_SEL_MASK) >> SPDIF_SCR_RXFIFOFULL_SEL_SHIFT];

    /* Set SPDIF state to idle */
    handle->state = kSPDIF_Idle;

    s_edmaPrivateHandle[instance][1].base   = base;
    s_edmaPrivateHandle[instance][1].handle = handle;

    /* Need to use scatter gather */
    EDMA_InstallTCDMemory(dmaLeftHandle, STCD_ADDR(handle->leftTcd), SPDIF_XFER_QUEUE_SIZE);
    EDMA_InstallTCDMemory(dmaRightHandle, STCD_ADDR(handle->rightTcd), SPDIF_XFER_QUEUE_SIZE);

    /* Install callback for Tx dma channel */
    EDMA_SetCallback(dmaRightHandle, SPDIF_RxEDMACallback, &s_edmaPrivateHandle[instance][1]);
}

/*!
 * brief Performs a non-blocking SPDIF transfer using DMA.
 *
 * note This interface returns immediately after the transfer initiates. Call
 * SPDIF_GetTransferStatus to poll the transfer status and check whether the SPDIF transfer is finished.
 *
 * param base SPDIF base pointer.
 * param handle SPDIF eDMA handle pointer.
 * param xfer Pointer to the DMA transfer structure.
 * retval kStatus_Success Start a SPDIF eDMA send successfully.
 * retval kStatus_InvalidArgument The input argument is invalid.
 * retval kStatus_TxBusy SPDIF is busy sending data.
 */
status_t SPDIF_TransferSendEDMA(SPDIF_Type *base, spdif_edma_handle_t *handle, spdif_edma_transfer_t *xfer)
{
    assert(handle && xfer);

    edma_transfer_config_t config = {0};
    uint32_t destAddr             = SPDIF_TxGetLeftDataRegisterAddress(base);

    /* Check if input parameter invalid */
    if ((xfer->leftData == NULL) || (xfer->dataSize == 0U) || (xfer->rightData == NULL))
    {
        return kStatus_InvalidArgument;
    }

    if ((handle->spdifQueue[handle->queueUser].leftData) || (handle->spdifQueue[handle->queueUser].rightData))
    {
        return kStatus_SPDIF_QueueFull;
    }

    /* Change the state of handle */
    handle->state = kSPDIF_Busy;

    /* Update the queue state */
    handle->transferSize[handle->queueUser]         = xfer->dataSize;
    handle->spdifQueue[handle->queueUser].leftData  = xfer->leftData;
    handle->spdifQueue[handle->queueUser].dataSize  = xfer->dataSize;
    handle->spdifQueue[handle->queueUser].rightData = xfer->rightData;
    handle->queueUser                               = (handle->queueUser + 1) % SPDIF_XFER_QUEUE_SIZE;

    /* Store the initially configured eDMA minor byte transfer count into the SPDIF handle */
    handle->nbytes = handle->count * 8U;

    /* Prepare edma configure */
    EDMA_PrepareTransfer(&config, xfer->leftData, 4U, (void *)destAddr, 4U, handle->count * 4U, xfer->dataSize,
                         kEDMA_MemoryToPeripheral);
    SPDIF_SubmitTransfer(handle->dmaLeftHandle, &config, handle->dmaRightHandle->channel);

    /* Prepare right channel */
    destAddr = SPDIF_TxGetRightDataRegisterAddress(base);
    EDMA_PrepareTransfer(&config, xfer->rightData, 4U, (void *)destAddr, 4U, handle->count * 4U, xfer->dataSize,
                         kEDMA_MemoryToPeripheral);
    EDMA_SubmitTransfer(handle->dmaRightHandle, &config);

    /* Start DMA transfer */
    EDMA_StartTransfer(handle->dmaLeftHandle);
    EDMA_StartTransfer(handle->dmaRightHandle);

    /* Enable DMA enable bit */
    SPDIF_EnableDMA(base, kSPDIF_TxDMAEnable, true);

    /* Enable SPDIF Tx clock */
    SPDIF_TxEnable(base, true);

    return kStatus_Success;
}

/*!
 * brief Performs a non-blocking SPDIF receive using eDMA.
 *
 * note This interface returns immediately after the transfer initiates. Call
 * the SPDIF_GetReceiveRemainingBytes to poll the transfer status and check whether the SPDIF transfer is finished.
 *
 * param base SPDIF base pointer
 * param handle SPDIF eDMA handle pointer.
 * param xfer Pointer to DMA transfer structure.
 * retval kStatus_Success Start a SPDIF eDMA receive successfully.
 * retval kStatus_InvalidArgument The input argument is invalid.
 * retval kStatus_RxBusy SPDIF is busy receiving data.
 */
status_t SPDIF_TransferReceiveEDMA(SPDIF_Type *base, spdif_edma_handle_t *handle, spdif_edma_transfer_t *xfer)
{
    assert(handle && xfer);

    edma_transfer_config_t config = {0};
    uint32_t srcAddr              = SPDIF_RxGetLeftDataRegisterAddress(base);

    /* Check if input parameter invalid */
    if ((xfer->leftData == NULL) || (xfer->dataSize == 0U) || (xfer->rightData == NULL))
    {
        return kStatus_InvalidArgument;
    }

    if ((handle->spdifQueue[handle->queueUser].leftData) || (handle->spdifQueue[handle->queueUser].rightData))
    {
        return kStatus_SPDIF_QueueFull;
    }

    /* Change the state of handle */
    handle->state = kSPDIF_Busy;

    /* Update the queue state */
    handle->transferSize[handle->queueUser]         = xfer->dataSize;
    handle->spdifQueue[handle->queueUser].leftData  = xfer->leftData;
    handle->spdifQueue[handle->queueUser].dataSize  = xfer->dataSize;
    handle->spdifQueue[handle->queueUser].rightData = xfer->rightData;
    handle->queueUser                               = (handle->queueUser + 1) % SPDIF_XFER_QUEUE_SIZE;

    /* Store the initially configured eDMA minor byte transfer count into the SPDIF handle */
    handle->nbytes = handle->count * 8U;

    /* Prepare edma configure */
    EDMA_PrepareTransfer(&config, (void *)srcAddr, 4U, xfer->leftData, 4U, handle->count * 4U, xfer->dataSize,
                         kEDMA_PeripheralToMemory);
    /* Use specific submit function to enable channel link */
    SPDIF_SubmitTransfer(handle->dmaLeftHandle, &config, handle->dmaRightHandle->channel);

    /* Prepare right channel */
    srcAddr = SPDIF_RxGetRightDataRegisterAddress(base);
    EDMA_PrepareTransfer(&config, (void *)srcAddr, 4U, xfer->rightData, 4U, handle->count * 4U, xfer->dataSize,
                         kEDMA_PeripheralToMemory);
    EDMA_SubmitTransfer(handle->dmaRightHandle, &config);

    /* Start DMA transfer */
    EDMA_StartTransfer(handle->dmaLeftHandle);
    EDMA_StartTransfer(handle->dmaRightHandle);

    /* Enable DMA enable bit */
    SPDIF_EnableDMA(base, kSPDIF_RxDMAEnable, true);

    /* Enable SPDIF Rx clock */
    SPDIF_RxEnable(base, true);

    return kStatus_Success;
}

/*!
 * brief Aborts a SPDIF transfer using eDMA.
 *
 * param base SPDIF base pointer.
 * param handle SPDIF eDMA handle pointer.
 */
void SPDIF_TransferAbortSendEDMA(SPDIF_Type *base, spdif_edma_handle_t *handle)
{
    assert(handle);

    /* Disable dma */
    EDMA_AbortTransfer(handle->dmaLeftHandle);
    EDMA_AbortTransfer(handle->dmaRightHandle);

    /* Disable DMA enable bit */
    SPDIF_EnableDMA(base, kSPDIF_TxDMAEnable, false);

    /* Set internal state */
    memset(handle->spdifQueue, 0U, sizeof(handle->spdifQueue));
    memset(handle->transferSize, 0U, sizeof(handle->transferSize));
    handle->queueUser   = 0U;
    handle->queueDriver = 0U;

    /* Set the handle state */
    handle->state = kSPDIF_Idle;
}

/*!
 * brief Aborts a SPDIF receive using eDMA.
 *
 * param base SPDIF base pointer
 * param handle SPDIF eDMA handle pointer.
 */
void SPDIF_TransferAbortReceiveEDMA(SPDIF_Type *base, spdif_edma_handle_t *handle)
{
    assert(handle);

    /* Disable dma */
    EDMA_AbortTransfer(handle->dmaLeftHandle);
    EDMA_AbortTransfer(handle->dmaRightHandle);

    /* Disable DMA enable bit */
    SPDIF_EnableDMA(base, kSPDIF_RxDMAEnable, false);

    /* Set internal state */
    memset(handle->spdifQueue, 0U, sizeof(handle->spdifQueue));
    memset(handle->transferSize, 0U, sizeof(handle->transferSize));
    handle->queueUser   = 0U;
    handle->queueDriver = 0U;

    /* Set the handle state */
    handle->state = kSPDIF_Idle;
}

/*!
 * brief Gets byte count sent by SPDIF.
 *
 * param base SPDIF base pointer.
 * param handle SPDIF eDMA handle pointer.
 * param count Bytes count sent by SPDIF.
 * retval kStatus_Success Succeed get the transfer count.
 * retval kStatus_NoTransferInProgress There is no non-blocking transaction in progress.
 */
status_t SPDIF_TransferGetSendCountEDMA(SPDIF_Type *base, spdif_edma_handle_t *handle, size_t *count)
{
    assert(handle);

    status_t status = kStatus_Success;

    if (handle->state != kSPDIF_Busy)
    {
        status = kStatus_NoTransferInProgress;
    }
    else
    {
        *count = (handle->transferSize[handle->queueDriver] -
                  (uint32_t)handle->nbytes *
                      EDMA_GetRemainingMajorLoopCount(handle->dmaRightHandle->base, handle->dmaRightHandle->channel));
    }

    return status;
}

/*!
 * brief Gets byte count received by SPDIF.
 *
 * param base SPDIF base pointer
 * param handle SPDIF eDMA handle pointer.
 * param count Bytes count received by SPDIF.
 * retval kStatus_Success Succeed get the transfer count.
 * retval kStatus_NoTransferInProgress There is no non-blocking transaction in progress.
 */
status_t SPDIF_TransferGetReceiveCountEDMA(SPDIF_Type *base, spdif_edma_handle_t *handle, size_t *count)
{
    assert(handle);

    status_t status = kStatus_Success;

    if (handle->state != kSPDIF_Busy)
    {
        status = kStatus_NoTransferInProgress;
    }
    else
    {
        *count = (handle->transferSize[handle->queueDriver] -
                  (uint32_t)handle->nbytes *
                      EDMA_GetRemainingMajorLoopCount(handle->dmaRightHandle->base, handle->dmaRightHandle->channel));
    }

    return status;
}
