/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 NXP
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "board.h"

/* SDK drivers */
#include "fsl_lpspi.h"

#include "drv_spi.h"

int drv_spi_init(drv_spi_t *spi, uint8_t id, drv_spi_config_t *cfg) {
    spi->instance = MCX_BoardGetSPIInstance(id);
    if (spi->instance == NULL) {
        return -ENOENT;
    }

    spi->id = id;

    int spi_inputfreq = MCX_BoardConfigureSPIClock(id);
    if (spi_inputfreq <= 0) {
        return -EINVAL;
    }

    lpspi_master_config_t spi_cfg;
    LPSPI_MasterGetDefaultConfig(&spi_cfg);

    spi_cfg.baudRate = cfg->frequency;
    spi_cfg.bitsPerFrame = cfg->frame_size;
    spi_cfg.cpol = cfg->cpol ?  kLPSPI_ClockPolarityActiveLow : kLPSPI_ClockPolarityActiveHigh;
    spi_cfg.cpha = cfg->cpha ? kLPSPI_ClockPhaseSecondEdge : kLPSPI_ClockPhaseFirstEdge;
    spi_cfg.direction = cfg->lsbfirst ? kLPSPI_LsbFirst : kLPSPI_MsbFirst;

    LPSPI_Type *lpspi = spi->instance;
    LPSPI_MasterInit(lpspi, &spi_cfg, spi_inputfreq);

    return 0;
}

int drv_spi_transfer(drv_spi_t *spi, drv_spi_transfer_t *transfer) {
    LPSPI_Type *lpspi = spi->instance;

    /* TODO: Ditch SDK drivers, prepare DMA transfer. */

    lpspi_transfer_t xfer = {
        .txData = (uint8_t *)transfer->tx_data, /* To SDK: Please do not touch that. */
        .rxData = transfer->rx_data,
        .dataSize = transfer->len,
    };

    status_t status = LPSPI_MasterTransferBlocking(lpspi, &xfer);
    if (status != kStatus_Success) {
        return -EIO;
    }

    return 0;
}
