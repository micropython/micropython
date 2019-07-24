/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_lpspi_edma.h"

/***********************************************************************************************************************
 * Definitions
 ***********************************************************************************************************************/

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.lpspi_edma"
#endif

/*!
 * @brief Structure definition for dspi_master_edma_private_handle_t. The structure is private.
 */
typedef struct _lpspi_master_edma_private_handle
{
    LPSPI_Type *base;                   /*!< LPSPI peripheral base address. */
    lpspi_master_edma_handle_t *handle; /*!< lpspi_master_edma_handle_t handle */
} lpspi_master_edma_private_handle_t;

/*!
 * @brief Structure definition for dspi_slave_edma_private_handle_t. The structure is private.
 */
typedef struct _lpspi_slave_edma_private_handle
{
    LPSPI_Type *base;                  /*!< LPSPI peripheral base address. */
    lpspi_slave_edma_handle_t *handle; /*!< lpspi_slave_edma_handle_t handle */
} lpspi_slave_edma_private_handle_t;

/***********************************************************************************************************************
 * Prototypes
 ***********************************************************************************************************************/

/*!
 * @brief Get instance number for LPSPI module.
 *
 * @param base LPSPI peripheral base address.
 * @return Return the value of LPSPI instance.
 */
static uint32_t LPSPI_GetInstance(LPSPI_Type *base);

/*!
 * @brief EDMA_LpspiMasterCallback after the LPSPI master transfer completed by using EDMA.
 * This is not a public API.
 */
static void EDMA_LpspiMasterCallback(edma_handle_t *edmaHandle,
                                     void *g_lpspiEdmaPrivateHandle,
                                     bool transferDone,
                                     uint32_t tcds);

/*!
 * @brief EDMA_LpspiSlaveCallback after the LPSPI slave transfer completed by using EDMA.
 * This is not a public API.
 */
static void EDMA_LpspiSlaveCallback(edma_handle_t *edmaHandle,
                                    void *g_lpspiEdmaPrivateHandle,
                                    bool transferDone,
                                    uint32_t tcds);

static void LPSPI_SeparateEdmaReadData(uint8_t *rxData, uint32_t readData, uint32_t bytesEachRead, bool isByteSwap);

/***********************************************************************************************************************
 * Variables
 ***********************************************************************************************************************/
/*! @brief Pointers to lpspi bases for each instance. */
static LPSPI_Type *const s_lpspiBases[] = LPSPI_BASE_PTRS;

/*! @brief Pointers to lpspi edma handles for each instance. */
static lpspi_master_edma_private_handle_t s_lpspiMasterEdmaPrivateHandle[ARRAY_SIZE(s_lpspiBases)];
static lpspi_slave_edma_private_handle_t s_lpspiSlaveEdmaPrivateHandle[ARRAY_SIZE(s_lpspiBases)];

/***********************************************************************************************************************
 * Code
 ***********************************************************************************************************************/

static uint32_t LPSPI_GetInstance(LPSPI_Type *base)
{
    uint8_t instance = 0;

    /* Find the instance index from base address mappings. */
    for (instance = 0; instance < ARRAY_SIZE(s_lpspiBases); instance++)
    {
        if (s_lpspiBases[instance] == base)
        {
            break;
        }
    }

    assert(instance < ARRAY_SIZE(s_lpspiBases));

    return instance;
}

static void LPSPI_SeparateEdmaReadData(uint8_t *rxData, uint32_t readData, uint32_t bytesEachRead, bool isByteSwap)
{
    assert(rxData);

    switch (bytesEachRead)
    {
        case 1:
            if (!isByteSwap)
            {
                *rxData = readData;
                ++rxData;
            }
            else
            {
                *rxData = readData >> 24;
                ++rxData;
            }
            break;

        case 2:
            if (!isByteSwap)
            {
                *rxData = readData;
                ++rxData;
                *rxData = readData >> 8;
                ++rxData;
            }
            else
            {
                *rxData = readData >> 16;
                ++rxData;
                *rxData = readData >> 24;
                ++rxData;
            }
            break;

        case 4:

            *rxData = readData;
            ++rxData;
            *rxData = readData >> 8;
            ++rxData;
            *rxData = readData >> 16;
            ++rxData;
            *rxData = readData >> 24;
            ++rxData;

            break;

        default:
            assert(false);
            break;
    }
}

/*!
 * brief Initializes the LPSPI master eDMA handle.
 *
 * This function initializes the LPSPI eDMA handle which can be used for other LPSPI transactional APIs.  Usually, for a
 * specified LPSPI instance, call this API once to get the initialized handle.
 *
 * Note that the LPSPI eDMA has a separated (Rx and Rx as two sources) or shared (Rx  and Tx are the same source) DMA
 * request source.
 * (1) For a separated DMA request source, enable and set the Rx DMAMUX source for edmaRxRegToRxDataHandle and
 * Tx DMAMUX source for edmaIntermediaryToTxRegHandle.
 * (2) For a shared DMA request source, enable and set the Rx/Rx DMAMUX source for edmaRxRegToRxDataHandle.
 *
 * param base LPSPI peripheral base address.
 * param handle LPSPI handle pointer to lpspi_master_edma_handle_t.
 * param callback LPSPI callback.
 * param userData callback function parameter.
 * param edmaRxRegToRxDataHandle edmaRxRegToRxDataHandle pointer to edma_handle_t.
 * param edmaTxDataToTxRegHandle edmaTxDataToTxRegHandle pointer to edma_handle_t.
 */
void LPSPI_MasterTransferCreateHandleEDMA(LPSPI_Type *base,
                                          lpspi_master_edma_handle_t *handle,
                                          lpspi_master_edma_transfer_callback_t callback,
                                          void *userData,
                                          edma_handle_t *edmaRxRegToRxDataHandle,
                                          edma_handle_t *edmaTxDataToTxRegHandle)
{
    assert(handle);
    assert(edmaRxRegToRxDataHandle);
    assert(edmaTxDataToTxRegHandle);

    /* Zero the handle. */
    memset(handle, 0, sizeof(*handle));

    uint32_t instance = LPSPI_GetInstance(base);

    s_lpspiMasterEdmaPrivateHandle[instance].base   = base;
    s_lpspiMasterEdmaPrivateHandle[instance].handle = handle;

    handle->callback = callback;
    handle->userData = userData;

    handle->edmaRxRegToRxDataHandle = edmaRxRegToRxDataHandle;
    handle->edmaTxDataToTxRegHandle = edmaTxDataToTxRegHandle;
}

/*!
 * brief LPSPI master transfer data using eDMA.
 *
 * This function transfers data using eDMA. This is a non-blocking function, which returns right away. When all data
 * is transferred, the callback function is called.
 *
 * Note:
 * The transfer data size should be an integer multiple of bytesPerFrame if bytesPerFrame is less than or equal to 4.
 * For bytesPerFrame greater than 4:
 * The transfer data size should be equal to bytesPerFrame if the bytesPerFrame is not an integer multiple of 4.
 * Otherwise, the transfer data size can be an integer multiple of bytesPerFrame.
 *
 * param base LPSPI peripheral base address.
 * param handle pointer to lpspi_master_edma_handle_t structure which stores the transfer state.
 * param transfer pointer to lpspi_transfer_t structure.
 * return status of status_t.
 */
status_t LPSPI_MasterTransferEDMA(LPSPI_Type *base, lpspi_master_edma_handle_t *handle, lpspi_transfer_t *transfer)
{
    assert(handle);
    assert(transfer);

    uint32_t bitsPerFrame  = ((base->TCR & LPSPI_TCR_FRAMESZ_MASK) >> LPSPI_TCR_FRAMESZ_SHIFT) + 1;
    uint32_t bytesPerFrame = (bitsPerFrame + 7) / 8;
    uint32_t temp          = 0U;

    if (!LPSPI_CheckTransferArgument(transfer, bitsPerFrame, bytesPerFrame))
    {
        return kStatus_InvalidArgument;
    }

    /*And since the dma transfer can not support 3 bytes .*/
    if ((bytesPerFrame % 4U) == 3)
    {
        return kStatus_InvalidArgument;
    }

    /* Check that we're not busy.*/
    if (handle->state == kLPSPI_Busy)
    {
        return kStatus_LPSPI_Busy;
    }

    handle->state = kLPSPI_Busy;

    uint32_t instance = LPSPI_GetInstance(base);
    uint32_t rxAddr   = LPSPI_GetRxRegisterAddress(base);
    uint32_t txAddr   = LPSPI_GetTxRegisterAddress(base);

    uint32_t whichPcs = (transfer->configFlags & LPSPI_MASTER_PCS_MASK) >> LPSPI_MASTER_PCS_SHIFT;

    /*Because DMA is fast enough , so set the RX and TX watermarks to 0 .*/
    uint8_t txWatermark = 0;
    uint8_t rxWatermark = 0;

    /*Used for byte swap*/
    uint32_t dif = 0;

    uint8_t bytesLastWrite = 0;

    bool isThereExtraTxBytes = false;

    uint8_t dummyData = g_lpspiDummyData[instance];

    edma_transfer_config_t transferConfigRx;
    edma_transfer_config_t transferConfigTx;

    edma_tcd_t *softwareTCD_extraBytes    = (edma_tcd_t *)((uint32_t)(&handle->lpspiSoftwareTCD[1]) & (~0x1FU));
    edma_tcd_t *softwareTCD_pcsContinuous = (edma_tcd_t *)((uint32_t)(&handle->lpspiSoftwareTCD[2]) & (~0x1FU));

    handle->txData               = transfer->txData;
    handle->rxData               = transfer->rxData;
    handle->txRemainingByteCount = transfer->dataSize;
    handle->rxRemainingByteCount = transfer->dataSize;
    handle->totalByteCount       = transfer->dataSize;

    handle->writeRegRemainingTimes = (transfer->dataSize / bytesPerFrame) * ((bytesPerFrame + 3) / 4);
    handle->readRegRemainingTimes  = handle->writeRegRemainingTimes;

    handle->txBuffIfNull =
        ((uint32_t)dummyData) | ((uint32_t)dummyData << 8) | ((uint32_t)dummyData << 16) | ((uint32_t)dummyData << 24);

    /*The TX and RX FIFO sizes are always the same*/
    handle->fifoSize = LPSPI_GetRxFifoSize(base);

    handle->isPcsContinuous = (bool)(transfer->configFlags & kLPSPI_MasterPcsContinuous);
    handle->isByteSwap      = (bool)(transfer->configFlags & kLPSPI_MasterByteSwap);

    LPSPI_SetFifoWatermarks(base, txWatermark, rxWatermark);

    /*Transfers will stall when transmit FIFO is empty or receive FIFO is full. */
    LPSPI_Enable(base, false);
    base->CFGR1 &= (~LPSPI_CFGR1_NOSTALL_MASK);
    /* Check if using 3-wire mode and the txData is NULL, set the output pin to tristated. */
    temp = base->CFGR1;
    temp &= LPSPI_CFGR1_PINCFG_MASK;
    if ((temp == LPSPI_CFGR1_PINCFG(kLPSPI_SdiInSdiOut)) || (temp == LPSPI_CFGR1_PINCFG(kLPSPI_SdoInSdoOut)))
    {
        if (!handle->txData)
        {
            base->CFGR1 |= LPSPI_CFGR1_OUTCFG_MASK;
        }
        /* The 3-wire mode can't send and receive data at the same time. */
        if ((handle->txData) && (handle->rxData))
        {
            return kStatus_InvalidArgument;
        }
    }

    LPSPI_Enable(base, true);

    /*Flush FIFO , clear status , disable all the inerrupts.*/
    LPSPI_FlushFifo(base, true, true);
    LPSPI_ClearStatusFlags(base, kLPSPI_AllStatusFlag);
    LPSPI_DisableInterrupts(base, kLPSPI_AllInterruptEnable);

    /* For DMA transfer , we'd better not masked the transmit data and receive data in TCR since the transfer flow is
     * hard to controlled by software.
     */
    base->TCR = (base->TCR & ~(LPSPI_TCR_CONT_MASK | LPSPI_TCR_CONTC_MASK | LPSPI_TCR_BYSW_MASK | LPSPI_TCR_PCS_MASK)) |
                LPSPI_TCR_CONT(handle->isPcsContinuous) | LPSPI_TCR_CONTC(0U) | LPSPI_TCR_BYSW(handle->isByteSwap) |
                LPSPI_TCR_PCS(whichPcs);

    isThereExtraTxBytes         = false;
    handle->isThereExtraRxBytes = false;

    /*Calculate the bytes for write/read the TX/RX register each time*/
    if (bytesPerFrame <= 4)
    {
        handle->bytesEachWrite = bytesPerFrame;
        handle->bytesEachRead  = bytesPerFrame;

        handle->bytesLastRead = bytesPerFrame;
    }
    else
    {
        handle->bytesEachWrite = 4;
        handle->bytesEachRead  = 4;

        handle->bytesLastRead = 4;

        if ((transfer->dataSize % 4) != 0)
        {
            bytesLastWrite        = transfer->dataSize % 4;
            handle->bytesLastRead = bytesLastWrite;

            isThereExtraTxBytes = true;

            --handle->writeRegRemainingTimes;

            --handle->readRegRemainingTimes;
            handle->isThereExtraRxBytes = true;
        }
    }

    LPSPI_DisableDMA(base, kLPSPI_RxDmaEnable | kLPSPI_TxDmaEnable);

    EDMA_SetCallback(handle->edmaRxRegToRxDataHandle, EDMA_LpspiMasterCallback,
                     &s_lpspiMasterEdmaPrivateHandle[instance]);

    /*Rx*/
    EDMA_ResetChannel(handle->edmaRxRegToRxDataHandle->base, handle->edmaRxRegToRxDataHandle->channel);

    if (handle->rxData)
    {
        transferConfigRx.destAddr   = (uint32_t) & (handle->rxData[0]);
        transferConfigRx.destOffset = 1;
    }
    else
    {
        transferConfigRx.destAddr   = (uint32_t) & (handle->rxBuffIfNull);
        transferConfigRx.destOffset = 0;
    }
    transferConfigRx.destTransferSize = kEDMA_TransferSize1Bytes;

    dif = 0;
    switch (handle->bytesEachRead)
    {
        case (1U):
            transferConfigRx.srcTransferSize = kEDMA_TransferSize1Bytes;
            transferConfigRx.minorLoopBytes  = 1;
            if (handle->isByteSwap)
            {
                dif = 3;
            }
            break;

        case (2U):
            transferConfigRx.srcTransferSize = kEDMA_TransferSize2Bytes;
            transferConfigRx.minorLoopBytes  = 2;
            if (handle->isByteSwap)
            {
                dif = 2;
            }
            break;

        case (4U):
            transferConfigRx.srcTransferSize = kEDMA_TransferSize4Bytes;
            transferConfigRx.minorLoopBytes  = 4;
            break;

        default:
            transferConfigRx.srcTransferSize = kEDMA_TransferSize1Bytes;
            transferConfigRx.minorLoopBytes  = 1;
            assert(false);
            break;
    }

    transferConfigRx.srcAddr   = (uint32_t)rxAddr + dif;
    transferConfigRx.srcOffset = 0;

    transferConfigRx.majorLoopCounts = handle->readRegRemainingTimes;

    /* Store the initially configured eDMA minor byte transfer count into the LPSPI handle */
    handle->nbytes = transferConfigRx.minorLoopBytes;

    EDMA_SetTransferConfig(handle->edmaRxRegToRxDataHandle->base, handle->edmaRxRegToRxDataHandle->channel,
                           &transferConfigRx, NULL);
    EDMA_EnableChannelInterrupts(handle->edmaRxRegToRxDataHandle->base, handle->edmaRxRegToRxDataHandle->channel,
                                 kEDMA_MajorInterruptEnable);

    /*Tx*/
    EDMA_ResetChannel(handle->edmaTxDataToTxRegHandle->base, handle->edmaTxDataToTxRegHandle->channel);

    if (isThereExtraTxBytes)
    {
        if (handle->txData)
        {
            transferConfigTx.srcAddr   = (uint32_t) & (transfer->txData[transfer->dataSize - bytesLastWrite]);
            transferConfigTx.srcOffset = 1;
        }
        else
        {
            transferConfigTx.srcAddr   = (uint32_t)(&handle->txBuffIfNull);
            transferConfigTx.srcOffset = 0;
        }

        transferConfigTx.destOffset = 0;

        transferConfigTx.srcTransferSize = kEDMA_TransferSize1Bytes;

        dif = 0;
        switch (bytesLastWrite)
        {
            case (1U):
                transferConfigTx.destTransferSize = kEDMA_TransferSize1Bytes;
                transferConfigTx.minorLoopBytes   = 1;
                if (handle->isByteSwap)
                {
                    dif = 3;
                }
                break;

            case (2U):
                transferConfigTx.destTransferSize = kEDMA_TransferSize2Bytes;
                transferConfigTx.minorLoopBytes   = 2;
                if (handle->isByteSwap)
                {
                    dif = 2;
                }
                break;

            default:
                transferConfigTx.destTransferSize = kEDMA_TransferSize1Bytes;
                transferConfigTx.minorLoopBytes   = 1;
                assert(false);
                break;
        }

        transferConfigTx.destAddr        = (uint32_t)txAddr + dif;
        transferConfigTx.majorLoopCounts = 1;

        EDMA_TcdReset(softwareTCD_extraBytes);

        if (handle->isPcsContinuous)
        {
            EDMA_TcdSetTransferConfig(softwareTCD_extraBytes, &transferConfigTx, softwareTCD_pcsContinuous);
        }
        else
        {
            EDMA_TcdSetTransferConfig(softwareTCD_extraBytes, &transferConfigTx, NULL);
        }
    }

    if (handle->isPcsContinuous)
    {
        handle->transmitCommand = base->TCR & ~(LPSPI_TCR_CONTC_MASK | LPSPI_TCR_CONT_MASK);

        transferConfigTx.srcAddr   = (uint32_t) & (handle->transmitCommand);
        transferConfigTx.srcOffset = 0;

        transferConfigTx.destAddr   = (uint32_t) & (base->TCR);
        transferConfigTx.destOffset = 0;

        transferConfigTx.srcTransferSize  = kEDMA_TransferSize4Bytes;
        transferConfigTx.destTransferSize = kEDMA_TransferSize4Bytes;
        transferConfigTx.minorLoopBytes   = 4;
        transferConfigTx.majorLoopCounts  = 1;

        EDMA_TcdReset(softwareTCD_pcsContinuous);
        EDMA_TcdSetTransferConfig(softwareTCD_pcsContinuous, &transferConfigTx, NULL);
    }

    if (handle->txData)
    {
        transferConfigTx.srcAddr   = (uint32_t)(handle->txData);
        transferConfigTx.srcOffset = 1;
    }
    else
    {
        transferConfigTx.srcAddr   = (uint32_t)(&handle->txBuffIfNull);
        transferConfigTx.srcOffset = 0;
    }

    transferConfigTx.destOffset = 0;

    transferConfigTx.srcTransferSize = kEDMA_TransferSize1Bytes;

    dif = 0;
    switch (handle->bytesEachRead)
    {
        case (1U):
            transferConfigTx.destTransferSize = kEDMA_TransferSize1Bytes;
            transferConfigTx.minorLoopBytes   = 1;
            if (handle->isByteSwap)
            {
                dif = 3;
            }
            break;

        case (2U):
            transferConfigTx.destTransferSize = kEDMA_TransferSize2Bytes;
            transferConfigTx.minorLoopBytes   = 2;

            if (handle->isByteSwap)
            {
                dif = 2;
            }
            break;

        case (4U):
            transferConfigTx.destTransferSize = kEDMA_TransferSize4Bytes;
            transferConfigTx.minorLoopBytes   = 4;
            break;

        default:
            transferConfigTx.destTransferSize = kEDMA_TransferSize1Bytes;
            transferConfigTx.minorLoopBytes   = 1;
            assert(false);
            break;
    }

    transferConfigTx.destAddr = (uint32_t)txAddr + dif;

    transferConfigTx.majorLoopCounts = handle->writeRegRemainingTimes;

    if (isThereExtraTxBytes)
    {
        EDMA_SetTransferConfig(handle->edmaTxDataToTxRegHandle->base, handle->edmaTxDataToTxRegHandle->channel,
                               &transferConfigTx, softwareTCD_extraBytes);
    }
    else if (handle->isPcsContinuous)
    {
        EDMA_SetTransferConfig(handle->edmaTxDataToTxRegHandle->base, handle->edmaTxDataToTxRegHandle->channel,
                               &transferConfigTx, softwareTCD_pcsContinuous);
    }
    else
    {
        EDMA_SetTransferConfig(handle->edmaTxDataToTxRegHandle->base, handle->edmaTxDataToTxRegHandle->channel,
                               &transferConfigTx, NULL);
    }

    EDMA_StartTransfer(handle->edmaTxDataToTxRegHandle);
    EDMA_StartTransfer(handle->edmaRxRegToRxDataHandle);

    LPSPI_EnableDMA(base, kLPSPI_RxDmaEnable | kLPSPI_TxDmaEnable);

    return kStatus_Success;
}

static void EDMA_LpspiMasterCallback(edma_handle_t *edmaHandle,
                                     void *g_lpspiEdmaPrivateHandle,
                                     bool transferDone,
                                     uint32_t tcds)
{
    assert(edmaHandle);
    assert(g_lpspiEdmaPrivateHandle);

    uint32_t readData;

    lpspi_master_edma_private_handle_t *lpspiEdmaPrivateHandle;

    lpspiEdmaPrivateHandle = (lpspi_master_edma_private_handle_t *)g_lpspiEdmaPrivateHandle;

    LPSPI_DisableDMA(lpspiEdmaPrivateHandle->base, kLPSPI_TxDmaEnable | kLPSPI_RxDmaEnable);

    if (lpspiEdmaPrivateHandle->handle->isThereExtraRxBytes)
    {
        while (LPSPI_GetRxFifoCount(lpspiEdmaPrivateHandle->base) == 0)
        {
        }
        readData = LPSPI_ReadData(lpspiEdmaPrivateHandle->base);

        if (lpspiEdmaPrivateHandle->handle->rxData)
        {
            LPSPI_SeparateEdmaReadData(
                &(lpspiEdmaPrivateHandle->handle->rxData[lpspiEdmaPrivateHandle->handle->rxRemainingByteCount -
                                                         lpspiEdmaPrivateHandle->handle->bytesLastRead]),
                readData, lpspiEdmaPrivateHandle->handle->bytesLastRead, lpspiEdmaPrivateHandle->handle->isByteSwap);
        }
    }

    lpspiEdmaPrivateHandle->handle->state = kLPSPI_Idle;

    if (lpspiEdmaPrivateHandle->handle->callback)
    {
        lpspiEdmaPrivateHandle->handle->callback(lpspiEdmaPrivateHandle->base, lpspiEdmaPrivateHandle->handle,
                                                 kStatus_Success, lpspiEdmaPrivateHandle->handle->userData);
    }
}

/*!
 * brief LPSPI master aborts a transfer which is using eDMA.
 *
 * This function aborts a transfer which is using eDMA.
 *
 * param base LPSPI peripheral base address.
 * param handle pointer to lpspi_master_edma_handle_t structure which stores the transfer state.
 */
void LPSPI_MasterTransferAbortEDMA(LPSPI_Type *base, lpspi_master_edma_handle_t *handle)
{
    assert(handle);

    LPSPI_DisableDMA(base, kLPSPI_RxDmaEnable | kLPSPI_TxDmaEnable);

    EDMA_AbortTransfer(handle->edmaRxRegToRxDataHandle);
    EDMA_AbortTransfer(handle->edmaTxDataToTxRegHandle);

    handle->state = kLPSPI_Idle;
}

/*!
 * brief Gets the master eDMA transfer remaining bytes.
 *
 * This function gets the master eDMA transfer remaining bytes.
 *
 * param base LPSPI peripheral base address.
 * param handle pointer to lpspi_master_edma_handle_t structure which stores the transfer state.
 * param count Number of bytes transferred so far by the EDMA transaction.
 * return status of status_t.
 */
status_t LPSPI_MasterTransferGetCountEDMA(LPSPI_Type *base, lpspi_master_edma_handle_t *handle, size_t *count)
{
    assert(handle);

    if (!count)
    {
        return kStatus_InvalidArgument;
    }

    /* Catch when there is not an active transfer. */
    if (handle->state != kLPSPI_Busy)
    {
        *count = 0;
        return kStatus_NoTransferInProgress;
    }

    size_t remainingByte;

    remainingByte =
        (uint32_t)handle->nbytes * EDMA_GetRemainingMajorLoopCount(handle->edmaRxRegToRxDataHandle->base,
                                                                   handle->edmaRxRegToRxDataHandle->channel);

    *count = handle->totalByteCount - remainingByte;

    return kStatus_Success;
}

/*!
 * brief Initializes the LPSPI slave eDMA handle.
 *
 * This function initializes the LPSPI eDMA handle which can be used for other LPSPI transactional APIs.  Usually, for a
 * specified LPSPI instance, call this API once to get the initialized handle.
 *
 * Note that LPSPI eDMA has a separated (Rx and Tx as two sources) or shared (Rx  and Tx as the same source) DMA request
 * source.
 *
 * (1) For a separated DMA request source, enable and set the Rx DMAMUX source for edmaRxRegToRxDataHandle and
 * Tx DMAMUX source for edmaTxDataToTxRegHandle.
 * (2) For a shared DMA request source, enable and set the Rx/Rx DMAMUX source for edmaRxRegToRxDataHandle .
 *
 * param base LPSPI peripheral base address.
 * param handle LPSPI handle pointer to lpspi_slave_edma_handle_t.
 * param callback LPSPI callback.
 * param userData callback function parameter.
 * param edmaRxRegToRxDataHandle edmaRxRegToRxDataHandle pointer to edma_handle_t.
 * param edmaTxDataToTxRegHandle edmaTxDataToTxRegHandle pointer to edma_handle_t.
 */
void LPSPI_SlaveTransferCreateHandleEDMA(LPSPI_Type *base,
                                         lpspi_slave_edma_handle_t *handle,
                                         lpspi_slave_edma_transfer_callback_t callback,
                                         void *userData,
                                         edma_handle_t *edmaRxRegToRxDataHandle,
                                         edma_handle_t *edmaTxDataToTxRegHandle)
{
    assert(handle);
    assert(edmaRxRegToRxDataHandle);
    assert(edmaTxDataToTxRegHandle);

    /* Zero the handle. */
    memset(handle, 0, sizeof(*handle));

    uint32_t instance = LPSPI_GetInstance(base);

    s_lpspiSlaveEdmaPrivateHandle[instance].base   = base;
    s_lpspiSlaveEdmaPrivateHandle[instance].handle = handle;

    handle->callback = callback;
    handle->userData = userData;

    handle->edmaRxRegToRxDataHandle = edmaRxRegToRxDataHandle;
    handle->edmaTxDataToTxRegHandle = edmaTxDataToTxRegHandle;
}

/*!
 * brief LPSPI slave transfers data using eDMA.
 *
 * This function transfers data using eDMA. This is a non-blocking function, which return right away. When all data
 * is transferred, the callback function is called.
 *
 * Note:
 * The transfer data size should be an integer multiple of bytesPerFrame if bytesPerFrame is less than or equal to 4.
 * For bytesPerFrame greater than 4:
 * The transfer data size should be equal to bytesPerFrame if the bytesPerFrame is not an integer multiple of 4.
 * Otherwise, the transfer data size can be an integer multiple of bytesPerFrame.
 *
 * param base LPSPI peripheral base address.
 * param handle pointer to lpspi_slave_edma_handle_t structure which stores the transfer state.
 * param transfer pointer to lpspi_transfer_t structure.
 * return status of status_t.
 */
status_t LPSPI_SlaveTransferEDMA(LPSPI_Type *base, lpspi_slave_edma_handle_t *handle, lpspi_transfer_t *transfer)
{
    assert(handle);
    assert(transfer);

    uint32_t bitsPerFrame  = ((base->TCR & LPSPI_TCR_FRAMESZ_MASK) >> LPSPI_TCR_FRAMESZ_SHIFT) + 1;
    uint32_t bytesPerFrame = (bitsPerFrame + 7) / 8;
    uint32_t temp          = 0U;

    uint8_t dummyData = g_lpspiDummyData[LPSPI_GetInstance(base)];

    if (!LPSPI_CheckTransferArgument(transfer, bitsPerFrame, bytesPerFrame))
    {
        return kStatus_InvalidArgument;
    }

    /*And since the dma transfer can not support 3 bytes .*/
    if ((bytesPerFrame % 4U) == 3)
    {
        return kStatus_InvalidArgument;
    }

    /* Check that we're not busy.*/
    if (handle->state == kLPSPI_Busy)
    {
        return kStatus_LPSPI_Busy;
    }

    handle->state = kLPSPI_Busy;

    uint32_t rxAddr = LPSPI_GetRxRegisterAddress(base);
    uint32_t txAddr = LPSPI_GetTxRegisterAddress(base);

    edma_tcd_t *softwareTCD_extraBytes = (edma_tcd_t *)((uint32_t)(&handle->lpspiSoftwareTCD[1]) & (~0x1FU));

    uint32_t whichPcs = (transfer->configFlags & LPSPI_MASTER_PCS_MASK) >> LPSPI_MASTER_PCS_SHIFT;

    /*Because DMA is fast enough , so set the RX and TX watermarks to 0 .*/
    uint8_t txWatermark = 0;
    uint8_t rxWatermark = 0;

    /*Used for byte swap*/
    uint32_t dif = 0;

    uint8_t bytesLastWrite = 0;

    uint32_t instance = LPSPI_GetInstance(base);

    edma_transfer_config_t transferConfigRx;
    edma_transfer_config_t transferConfigTx;

    bool isThereExtraTxBytes = false;

    handle->txData               = transfer->txData;
    handle->rxData               = transfer->rxData;
    handle->txRemainingByteCount = transfer->dataSize;
    handle->rxRemainingByteCount = transfer->dataSize;
    handle->totalByteCount       = transfer->dataSize;

    handle->writeRegRemainingTimes = (transfer->dataSize / bytesPerFrame) * ((bytesPerFrame + 3) / 4);
    handle->readRegRemainingTimes  = handle->writeRegRemainingTimes;

    handle->txBuffIfNull =
        ((uint32_t)dummyData) | ((uint32_t)dummyData << 8) | ((uint32_t)dummyData << 16) | ((uint32_t)dummyData << 24);

    /*The TX and RX FIFO sizes are always the same*/
    handle->fifoSize = LPSPI_GetRxFifoSize(base);

    handle->isByteSwap = (bool)(transfer->configFlags & kLPSPI_MasterByteSwap);

    LPSPI_SetFifoWatermarks(base, txWatermark, rxWatermark);

    /*Transfers will stall when transmit FIFO is empty or receive FIFO is full. */
    LPSPI_Enable(base, false);
    base->CFGR1 &= (~LPSPI_CFGR1_NOSTALL_MASK);
    /* Check if using 3-wire mode and the txData is NULL, set the output pin to tristated. */
    temp = base->CFGR1;
    temp &= LPSPI_CFGR1_PINCFG_MASK;
    if ((temp == LPSPI_CFGR1_PINCFG(kLPSPI_SdiInSdiOut)) || (temp == LPSPI_CFGR1_PINCFG(kLPSPI_SdoInSdoOut)))
    {
        if (!handle->txData)
        {
            base->CFGR1 |= LPSPI_CFGR1_OUTCFG_MASK;
        }
        /* The 3-wire mode can't send and receive data at the same time. */
        if ((handle->txData) && (handle->rxData))
        {
            return kStatus_InvalidArgument;
        }
    }

    LPSPI_Enable(base, true);

    /*Flush FIFO , clear status , disable all the inerrupts.*/
    LPSPI_FlushFifo(base, true, true);
    LPSPI_ClearStatusFlags(base, kLPSPI_AllStatusFlag);
    LPSPI_DisableInterrupts(base, kLPSPI_AllInterruptEnable);

    /* For DMA transfer , we'd better not masked the transmit data and receive data in TCR since the transfer flow is
     * hard to controlled by software.
     */
    base->TCR = (base->TCR & ~(LPSPI_TCR_CONT_MASK | LPSPI_TCR_CONTC_MASK | LPSPI_TCR_BYSW_MASK)) |
                LPSPI_TCR_CONTC(0U) | LPSPI_TCR_BYSW(handle->isByteSwap) | LPSPI_TCR_PCS(whichPcs);

    isThereExtraTxBytes         = false;
    handle->isThereExtraRxBytes = false;

    /*Calculate the bytes for write/read the TX/RX register each time*/
    if (bytesPerFrame <= 4)
    {
        handle->bytesEachWrite = bytesPerFrame;
        handle->bytesEachRead  = bytesPerFrame;

        handle->bytesLastRead = bytesPerFrame;
    }
    else
    {
        handle->bytesEachWrite = 4;
        handle->bytesEachRead  = 4;

        handle->bytesLastRead = 4;

        if ((transfer->dataSize % 4) != 0)
        {
            bytesLastWrite        = transfer->dataSize % 4;
            handle->bytesLastRead = bytesLastWrite;

            isThereExtraTxBytes = true;
            --handle->writeRegRemainingTimes;

            handle->isThereExtraRxBytes = true;
            --handle->readRegRemainingTimes;
        }
    }

    LPSPI_DisableDMA(base, kLPSPI_RxDmaEnable | kLPSPI_TxDmaEnable);

    EDMA_SetCallback(handle->edmaRxRegToRxDataHandle, EDMA_LpspiSlaveCallback,
                     &s_lpspiSlaveEdmaPrivateHandle[instance]);

    /*Rx*/
    if (handle->readRegRemainingTimes > 0)
    {
        EDMA_ResetChannel(handle->edmaRxRegToRxDataHandle->base, handle->edmaRxRegToRxDataHandle->channel);

        if (handle->rxData)
        {
            transferConfigRx.destAddr   = (uint32_t) & (handle->rxData[0]);
            transferConfigRx.destOffset = 1;
        }
        else
        {
            transferConfigRx.destAddr   = (uint32_t) & (handle->rxBuffIfNull);
            transferConfigRx.destOffset = 0;
        }
        transferConfigRx.destTransferSize = kEDMA_TransferSize1Bytes;

        dif = 0;
        switch (handle->bytesEachRead)
        {
            case (1U):
                transferConfigRx.srcTransferSize = kEDMA_TransferSize1Bytes;
                transferConfigRx.minorLoopBytes  = 1;
                if (handle->isByteSwap)
                {
                    dif = 3;
                }
                break;

            case (2U):
                transferConfigRx.srcTransferSize = kEDMA_TransferSize2Bytes;
                transferConfigRx.minorLoopBytes  = 2;
                if (handle->isByteSwap)
                {
                    dif = 2;
                }
                break;

            case (4U):
                transferConfigRx.srcTransferSize = kEDMA_TransferSize4Bytes;
                transferConfigRx.minorLoopBytes  = 4;
                break;

            default:
                transferConfigRx.srcTransferSize = kEDMA_TransferSize1Bytes;
                transferConfigRx.minorLoopBytes  = 1;
                assert(false);
                break;
        }

        transferConfigRx.srcAddr   = (uint32_t)rxAddr + dif;
        transferConfigRx.srcOffset = 0;

        transferConfigRx.majorLoopCounts = handle->readRegRemainingTimes;

        /* Store the initially configured eDMA minor byte transfer count into the DSPI handle */
        handle->nbytes = transferConfigRx.minorLoopBytes;

        EDMA_SetTransferConfig(handle->edmaRxRegToRxDataHandle->base, handle->edmaRxRegToRxDataHandle->channel,
                               &transferConfigRx, NULL);
        EDMA_EnableChannelInterrupts(handle->edmaRxRegToRxDataHandle->base, handle->edmaRxRegToRxDataHandle->channel,
                                     kEDMA_MajorInterruptEnable);
    }

    /*Tx*/
    EDMA_ResetChannel(handle->edmaTxDataToTxRegHandle->base, handle->edmaTxDataToTxRegHandle->channel);

    if (isThereExtraTxBytes)
    {
        if (handle->txData)
        {
            transferConfigTx.srcAddr   = (uint32_t) & (transfer->txData[transfer->dataSize - bytesLastWrite]);
            transferConfigTx.srcOffset = 1;
        }
        else
        {
            transferConfigTx.srcAddr   = (uint32_t)(&handle->txBuffIfNull);
            transferConfigTx.srcOffset = 0;
        }

        transferConfigTx.destOffset = 0;

        transferConfigTx.srcTransferSize = kEDMA_TransferSize1Bytes;

        dif = 0;
        switch (bytesLastWrite)
        {
            case (1U):
                transferConfigTx.destTransferSize = kEDMA_TransferSize1Bytes;
                transferConfigTx.minorLoopBytes   = 1;
                if (handle->isByteSwap)
                {
                    dif = 3;
                }
                break;

            case (2U):
                transferConfigTx.destTransferSize = kEDMA_TransferSize2Bytes;
                transferConfigTx.minorLoopBytes   = 2;
                if (handle->isByteSwap)
                {
                    dif = 2;
                }
                break;

            default:
                transferConfigTx.destTransferSize = kEDMA_TransferSize1Bytes;
                transferConfigTx.minorLoopBytes   = 1;
                assert(false);
                break;
        }

        transferConfigTx.destAddr        = (uint32_t)txAddr + dif;
        transferConfigTx.majorLoopCounts = 1;

        EDMA_TcdReset(softwareTCD_extraBytes);

        EDMA_TcdSetTransferConfig(softwareTCD_extraBytes, &transferConfigTx, NULL);
    }

    if (handle->txData)
    {
        transferConfigTx.srcAddr   = (uint32_t)(handle->txData);
        transferConfigTx.srcOffset = 1;
    }
    else
    {
        transferConfigTx.srcAddr   = (uint32_t)(&handle->txBuffIfNull);
        transferConfigTx.srcOffset = 0;
    }

    transferConfigTx.destOffset = 0;

    transferConfigTx.srcTransferSize = kEDMA_TransferSize1Bytes;

    dif = 0;
    switch (handle->bytesEachRead)
    {
        case (1U):
            transferConfigTx.destTransferSize = kEDMA_TransferSize1Bytes;
            transferConfigTx.minorLoopBytes   = 1;
            if (handle->isByteSwap)
            {
                dif = 3;
            }
            break;

        case (2U):
            transferConfigTx.destTransferSize = kEDMA_TransferSize2Bytes;
            transferConfigTx.minorLoopBytes   = 2;

            if (handle->isByteSwap)
            {
                dif = 2;
            }
            break;

        case (4U):
            transferConfigTx.destTransferSize = kEDMA_TransferSize4Bytes;
            transferConfigTx.minorLoopBytes   = 4;
            break;

        default:
            transferConfigTx.destTransferSize = kEDMA_TransferSize1Bytes;
            transferConfigTx.minorLoopBytes   = 1;
            assert(false);
            break;
    }

    transferConfigTx.destAddr = (uint32_t)txAddr + dif;

    transferConfigTx.majorLoopCounts = handle->writeRegRemainingTimes;

    if (isThereExtraTxBytes)
    {
        EDMA_SetTransferConfig(handle->edmaTxDataToTxRegHandle->base, handle->edmaTxDataToTxRegHandle->channel,
                               &transferConfigTx, softwareTCD_extraBytes);
    }
    else
    {
        EDMA_SetTransferConfig(handle->edmaTxDataToTxRegHandle->base, handle->edmaTxDataToTxRegHandle->channel,
                               &transferConfigTx, NULL);
    }

    EDMA_StartTransfer(handle->edmaTxDataToTxRegHandle);
    EDMA_StartTransfer(handle->edmaRxRegToRxDataHandle);

    LPSPI_EnableDMA(base, kLPSPI_RxDmaEnable | kLPSPI_TxDmaEnable);

    return kStatus_Success;
}

static void EDMA_LpspiSlaveCallback(edma_handle_t *edmaHandle,
                                    void *g_lpspiEdmaPrivateHandle,
                                    bool transferDone,
                                    uint32_t tcds)
{
    assert(edmaHandle);
    assert(g_lpspiEdmaPrivateHandle);

    uint32_t readData;

    lpspi_slave_edma_private_handle_t *lpspiEdmaPrivateHandle;

    lpspiEdmaPrivateHandle = (lpspi_slave_edma_private_handle_t *)g_lpspiEdmaPrivateHandle;

    LPSPI_DisableDMA(lpspiEdmaPrivateHandle->base, kLPSPI_TxDmaEnable | kLPSPI_RxDmaEnable);

    if (lpspiEdmaPrivateHandle->handle->isThereExtraRxBytes)
    {
        while (LPSPI_GetRxFifoCount(lpspiEdmaPrivateHandle->base) == 0)
        {
        }
        readData = LPSPI_ReadData(lpspiEdmaPrivateHandle->base);

        if (lpspiEdmaPrivateHandle->handle->rxData)
        {
            LPSPI_SeparateEdmaReadData(
                &(lpspiEdmaPrivateHandle->handle->rxData[lpspiEdmaPrivateHandle->handle->rxRemainingByteCount -
                                                         lpspiEdmaPrivateHandle->handle->bytesLastRead]),
                readData, lpspiEdmaPrivateHandle->handle->bytesLastRead, lpspiEdmaPrivateHandle->handle->isByteSwap);
        }
    }

    lpspiEdmaPrivateHandle->handle->state = kLPSPI_Idle;

    if (lpspiEdmaPrivateHandle->handle->callback)
    {
        lpspiEdmaPrivateHandle->handle->callback(lpspiEdmaPrivateHandle->base, lpspiEdmaPrivateHandle->handle,
                                                 kStatus_Success, lpspiEdmaPrivateHandle->handle->userData);
    }
}

/*!
 * brief LPSPI slave aborts a transfer which is using eDMA.
 *
 * This function aborts a transfer which is using eDMA.
 *
 * param base LPSPI peripheral base address.
 * param handle pointer to lpspi_slave_edma_handle_t structure which stores the transfer state.
 */
void LPSPI_SlaveTransferAbortEDMA(LPSPI_Type *base, lpspi_slave_edma_handle_t *handle)
{
    assert(handle);

    LPSPI_DisableDMA(base, kLPSPI_RxDmaEnable | kLPSPI_TxDmaEnable);

    EDMA_AbortTransfer(handle->edmaRxRegToRxDataHandle);
    EDMA_AbortTransfer(handle->edmaTxDataToTxRegHandle);

    handle->state = kLPSPI_Idle;
}

/*!
 * brief Gets the slave eDMA transfer remaining bytes.
 *
 * This function gets the slave eDMA transfer remaining bytes.
 *
 * param base LPSPI peripheral base address.
 * param handle pointer to lpspi_slave_edma_handle_t structure which stores the transfer state.
 * param count Number of bytes transferred so far by the eDMA transaction.
 * return status of status_t.
 */
status_t LPSPI_SlaveTransferGetCountEDMA(LPSPI_Type *base, lpspi_slave_edma_handle_t *handle, size_t *count)
{
    assert(handle);

    if (!count)
    {
        return kStatus_InvalidArgument;
    }

    /* Catch when there is not an active transfer. */
    if (handle->state != kLPSPI_Busy)
    {
        *count = 0;
        return kStatus_NoTransferInProgress;
    }

    size_t remainingByte;

    remainingByte =
        (uint32_t)handle->nbytes * EDMA_GetRemainingMajorLoopCount(handle->edmaRxRegToRxDataHandle->base,
                                                                   handle->edmaRxRegToRxDataHandle->channel);

    *count = handle->totalByteCount - remainingByte;

    return kStatus_Success;
}
