/*
 * This file is part of Adafruit for EFR32 project
 *
 * The MIT License (MIT)
 *
 * Copyright 2023 Silicon Laboratories Inc. www.silabs.com
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

#include "shared-bindings/busio/SPI.h"
#include "py/mperrno.h"
#include "py/runtime.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "supervisor/board.h"
#include "supervisor/shared/translate/translate.h"
#include "shared-bindings/microcontroller/Pin.h"

// Note that any bugs introduced in this file can cause crashes
// at startupfor chips using external SPI flash.

STATIC SPIDRV_HandleData_t spidrv_eusart_handle;
STATIC SPIDRV_Init_t spidrv_eusart_init = SPIDRV_MASTER_EUSART1;
STATIC bool in_used = false;
STATIC bool never_reset = false;

// Reset SPI when reload
void spi_reset(void) {
    if (!never_reset && in_used) {
        SPIDRV_DeInit(&spidrv_eusart_handle);
        in_used = false;
    }
    return;
}

// Construct SPI protocol, this function init SPI peripheral
void common_hal_busio_spi_construct(busio_spi_obj_t *self,
    const mcu_pin_obj_t *sck,
    const mcu_pin_obj_t *mosi,
    const mcu_pin_obj_t *miso,
    bool half_duplex) {
    Ecode_t sc = ECODE_OK;

    if (half_duplex) {
        mp_raise_NotImplementedError(
            translate("Half duplex SPI is not implemented"));
    }

    if ((sck != NULL) && (mosi != NULL) && (miso != NULL)) {
        if (sck->function_list[FN_EUSART1_SCLK] == 1
            && miso->function_list[FN_EUSART1_RX] == 1
            && mosi->function_list[FN_EUSART1_TX] == 1) {

            self->sck = sck;
            self->mosi = mosi;
            self->miso = miso;
            self->handle = &spidrv_eusart_handle;
            self->polarity = 0;
            self->phase = 0;
            self->bits = 8;

            spidrv_eusart_init.portTx = mosi->port;
            spidrv_eusart_init.portRx = miso->port;
            spidrv_eusart_init.portClk = sck->port;
            spidrv_eusart_init.pinTx = mosi->number;
            spidrv_eusart_init.pinRx = miso->number;
            spidrv_eusart_init.pinClk = sck->number;
            spidrv_eusart_init.bitRate = 1000000;
            spidrv_eusart_init.frameLength = 8;
            spidrv_eusart_init.dummyTxValue = 0;
            spidrv_eusart_init.type = spidrvMaster;
            spidrv_eusart_init.bitOrder = spidrvBitOrderMsbFirst;
            spidrv_eusart_init.clockMode = spidrvClockMode0;
            spidrv_eusart_init.csControl = spidrvCsControlApplication;
            spidrv_eusart_init.slaveStartMode = spidrvSlaveStartImmediate;

            sc = SPIDRV_Init(self->handle, &spidrv_eusart_init);
            if (sc != ECODE_EMDRV_SPIDRV_OK) {
                mp_raise_ValueError(translate("SPI init error"));
            }
        } else {
            mp_raise_ValueError(translate("Hardware in use, try alternative pins"));
        }
    } else {
        raise_ValueError_invalid_pins();
    }

    in_used = true;
    common_hal_mcu_pin_claim(sck);
    common_hal_mcu_pin_claim(mosi);
    common_hal_mcu_pin_claim(miso);
}

// Never reset SPI when reload
void common_hal_busio_spi_never_reset(busio_spi_obj_t *self) {
    never_reset = true;
    common_hal_never_reset_pin(self->mosi);
    common_hal_never_reset_pin(self->miso);
    common_hal_never_reset_pin(self->sck);
}

// Check SPI status, deinited or not
bool common_hal_busio_spi_deinited(busio_spi_obj_t *self) {
    return self->sck == NULL;
}

// Deinit SPI obj
void common_hal_busio_spi_deinit(busio_spi_obj_t *self) {

    if (common_hal_busio_spi_deinited(self)) {
        return;
    }

    Ecode_t sc = SPIDRV_DeInit(self->handle);
    if (sc != ECODE_EMDRV_SPIDRV_OK) {
        mp_raise_RuntimeError(translate("SPI re-init"));
    }

    in_used = false;
    self->sck = NULL;
    self->mosi = NULL;
    self->miso = NULL;
    self->handle = NULL;
    common_hal_reset_pin(self->mosi);
    common_hal_reset_pin(self->miso);
    common_hal_reset_pin(self->sck);
}

// Configures the SPI bus. The SPI object must be locked.
bool common_hal_busio_spi_configure(busio_spi_obj_t *self,
    uint32_t baudrate,
    uint8_t polarity,
    uint8_t phase,
    uint8_t bits) {
    Ecode_t sc;
    // This resets the SPI, so check before updating it redundantly
    if (baudrate == self->baudrate && polarity == self->polarity
        && phase == self->phase && bits == self->bits) {
        return true;
    }

    sc = SPIDRV_DeInit(self->handle);
    if (sc != ECODE_EMDRV_SPIDRV_OK) {
        mp_raise_RuntimeError(translate("SPI re-init"));
    }
    in_used = false;
    self->baudrate = baudrate;
    self->phase = phase;
    self->bits = bits;
    self->polarity = polarity;

    spidrv_eusart_init.bitRate = baudrate;
    spidrv_eusart_init.frameLength = 8;
    if (polarity == 0 && phase == 0) {
        spidrv_eusart_init.clockMode = spidrvClockMode0;
    } else if (polarity == 0 && phase == 1) {
        spidrv_eusart_init.clockMode = spidrvClockMode1;
    } else if (polarity == 1 && phase == 0) {
        spidrv_eusart_init.clockMode = spidrvClockMode2;
    } else if (polarity == 1 && phase == 1) {
        spidrv_eusart_init.clockMode = spidrvClockMode3;
    }

    sc = SPIDRV_Init(self->handle, &spidrv_eusart_init);
    if (sc != ECODE_EMDRV_SPIDRV_OK) {
        mp_raise_RuntimeError(translate("SPI re-init"));
    }
    in_used = true;
    return true;
}

// Lock SPI bus
bool common_hal_busio_spi_try_lock(busio_spi_obj_t *self) {
    bool grabbed_lock = false;
    if (!self->has_lock) {
        grabbed_lock = true;
        self->has_lock = true;
    }

    return grabbed_lock;
}

// Check SPI lock status
bool common_hal_busio_spi_has_lock(busio_spi_obj_t *self) {
    return self->has_lock;
}

// Unlock SPI bus
void common_hal_busio_spi_unlock(busio_spi_obj_t *self) {
    self->has_lock = false;
}

// Write the data contained in buffer
bool common_hal_busio_spi_write(busio_spi_obj_t *self,
    const uint8_t *data,
    size_t len) {

    Ecode_t result = SPIDRV_MTransmitB(self->handle, data, len);
    return result == ECODE_EMDRV_SPIDRV_OK;
}

// Read data into buffer
bool common_hal_busio_spi_read(busio_spi_obj_t *self,
    uint8_t *data, size_t len,
    uint8_t write_value) {

    self->handle->initData.dummyTxValue = write_value;
    Ecode_t result = SPIDRV_MReceiveB(self->handle, data, len);
    return result == ECODE_EMDRV_SPIDRV_OK;
}

// Write out the data in data_out
// while simultaneously reading data into data_in
bool common_hal_busio_spi_transfer(busio_spi_obj_t *self,
    const uint8_t *data_out,
    uint8_t *data_in,
    size_t len) {

    Ecode_t result = SPIDRV_MTransferB(self->handle, data_out, data_in, len);
    return result == ECODE_EMDRV_SPIDRV_OK;
}

// Get SPI baudrate
uint32_t common_hal_busio_spi_get_frequency(busio_spi_obj_t *self) {
    return self->baudrate;
}

// Get SPI phase
uint8_t common_hal_busio_spi_get_phase(busio_spi_obj_t *self) {
    return self->phase;
}

// Get SPI polarity
uint8_t common_hal_busio_spi_get_polarity(busio_spi_obj_t *self) {
    return self->polarity;
}
