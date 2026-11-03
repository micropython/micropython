/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022-2026 Infineon Technologies AG
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

// mpy includes
#include "py/runtime.h"
#include "py/mphal.h"

// MTB includes
#include "cy_scb_spi.h"

// port-specific includes
#include "clk.h"
#include "spi.h"

#if MICROPY_PY_MACHINE_SPI

#define SPI_CONTROLLER_OVERSAMPLE   (4U)
#define SPI_CLK_DIV_BASE            (4U)

uint8_t spi_get_id(spi_obj_t *self) {
    return self->id;
}

// Claims the SCB unit for the parent object. is_new is false if the parent already owns it.
void spi_alloc(spi_obj_t *self, uint8_t id, mp_obj_t parent, void (*scb_isr)(mp_obj_t), bool *is_new) {
    if (self->scb_obj == NULL) {
        *is_new = true;
        self->pclk_div = NULL;
        self->scb_obj = scb_obj_alloc(id, parent, scb_isr);
        self->id = id;
    } else if (self->scb_obj->parent == parent) {
        *is_new = false;
    } else {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("SPI object is already associated with another parent."));
    }
}

void spi_free(spi_obj_t *self) {
    if (self->scb_obj != NULL) {
        scb_obj_free(self->scb_obj);
        self->scb_obj = NULL;
    }
}

void spi_hw_irq(spi_obj_t *self) {
    Cy_SCB_SPI_Interrupt(self->scb_obj->scb, &self->ctx);
}

static cy_en_scb_spi_status_t spi_hw_init(spi_obj_t *self, cy_en_scb_spi_mode_t mode) {
    cy_en_scb_spi_sclk_mode_t sclk_mode;
    if (self->polarity == 0 && self->phase == 0) {
        sclk_mode = CY_SCB_SPI_CPHA0_CPOL0;
    } else if (self->polarity == 0 && self->phase == 1) {
        sclk_mode = CY_SCB_SPI_CPHA1_CPOL0;
    } else if (self->polarity == 1 && self->phase == 0) {
        sclk_mode = CY_SCB_SPI_CPHA0_CPOL1;
    } else {
        sclk_mode = CY_SCB_SPI_CPHA1_CPOL1;
    }

    bool is_controller = (mode == CY_SCB_SPI_MASTER);
    cy_stc_scb_spi_config_t cfg = {
        .spiMode = mode,
        .subMode = CY_SCB_SPI_MOTOROLA,
        .sclkMode = sclk_mode,
        .parity = CY_SCB_SPI_PARITY_NONE,
        .dropOnParityError = false,
        .oversample = is_controller ? SPI_CONTROLLER_OVERSAMPLE : 0U,
        .rxDataWidth = self->bits,
        .txDataWidth = self->bits,
        .enableMsbFirst = (self->firstbit == MICROPY_PY_MACHINE_SPI_MSB),
        .enableInputFilter = false,
        .enableFreeRunSclk = false,
        .enableMisoLateSample = is_controller,
        .enableTransferSeparation = false,
        .ssPolarity =
            ((CY_SCB_SPI_ACTIVE_LOW << CY_SCB_SPI_SLAVE_SELECT0) |
                (CY_SCB_SPI_ACTIVE_LOW << CY_SCB_SPI_SLAVE_SELECT1) |
                (CY_SCB_SPI_ACTIVE_LOW << CY_SCB_SPI_SLAVE_SELECT2) |
                (CY_SCB_SPI_ACTIVE_LOW << CY_SCB_SPI_SLAVE_SELECT3)),
        .ssSetupDelay = false,
        .ssHoldDelay = false,
        .ssInterFrameDelay = false,
        .enableWakeFromSleep = false,
        .rxFifoTriggerLevel = 0UL,
        .rxFifoIntEnableMask = 0UL,
        .txFifoTriggerLevel = 0UL,
        .txFifoIntEnableMask = 0UL,
        .masterSlaveIntEnableMask = 0UL,
    };

    cy_en_scb_spi_status_t result = Cy_SCB_SPI_Init(self->scb_obj->scb, &cfg, &self->ctx);
    if (result == CY_SCB_SPI_SUCCESS) {
        sys_int_init(&self->scb_obj->irq);
        Cy_SCB_SPI_Enable(self->scb_obj->scb);
    }
    return result;
}

// Requires the SCB to be claimed and polarity, phase, bits and firstbit to be set.
// Raises on failure, cleanup (spi_hw_deinit) is done by the caller.
void spi_hw_controller_init(spi_obj_t *self, uint32_t baudrate) {
    uint32_t clk_hf_freq = pclk_div_get_input_freq(self->scb_obj->clk);
    if (clk_hf_freq == 0U) {
        mp_raise_ValueError(MP_ERROR_TEXT("failed to get SPI input clock"));
    }

    uint32_t div_val = clk_hf_freq / ((uint64_t)baudrate * SPI_CONTROLLER_OVERSAMPLE);
    if (div_val > 0U) {
        div_val--;
    }
    if (div_val < SPI_CLK_DIV_BASE) {
        div_val = SPI_CLK_DIV_BASE;
    }

    self->pclk_div = pclk_div_init(self->scb_obj->clk, div_val, 0);
    if (self->pclk_div == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("SPI clock dividers exhausted"));
    }

    cy_en_scb_spi_status_t result = spi_hw_init(self, CY_SCB_SPI_MASTER);
    if (result != CY_SCB_SPI_SUCCESS) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("SPI init failed: 0x%lx"), (uint32_t)result);
    }
}

cy_en_scb_spi_status_t spi_hw_controller_transfer(spi_obj_t *self, const uint8_t *tx_buf, uint8_t *rx_buf, size_t len) {
    return Cy_SCB_SPI_Transfer(self->scb_obj->scb, (void *)tx_buf, rx_buf, len, &self->ctx);
}

bool spi_hw_controller_is_transfer_active(spi_obj_t *self) {
    return (Cy_SCB_SPI_GetTransferStatus(self->scb_obj->scb, &self->ctx) & CY_SCB_SPI_TRANSFER_ACTIVE) != 0U;
}

uint32_t spi_hw_controller_get_num_transferred(spi_obj_t *self) {
    return Cy_SCB_SPI_GetNumTransferred(self->scb_obj->scb, &self->ctx);
}

void spi_hw_controller_abort_transfer(spi_obj_t *self) {
    Cy_SCB_SPI_AbortTransfer(self->scb_obj->scb, &self->ctx);
}

// Stops the SPI hardware and releases the clock divider. The SCB stays claimed (see spi_free).
void spi_hw_deinit(spi_obj_t *self) {
    if (self->scb_obj != NULL) {
        Cy_SCB_SPI_Disable(self->scb_obj->scb, &self->ctx);
        Cy_SCB_SPI_DeInit(self->scb_obj->scb);
        sys_int_deinit(&self->scb_obj->irq);
    }
    if (self->pclk_div != NULL) {
        pclk_div_deinit(self->pclk_div);
        self->pclk_div = NULL;
    }
}

#endif // MICROPY_PY_MACHINE_SPI
