/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft
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
#include "py/nlr.h"
#include "py/runtime.h"
#include "rgb_led_status.h"
#include "samd21_pins.h"
#include "shared_dma.h"

// We use ENABLE registers below we don't want to treat as a macro.
#undef ENABLE

// Number of times to try to send packet if failed.
#define TIMEOUT 1

void common_hal_busio_spi_construct(busio_spi_obj_t *self,
        const mcu_pin_obj_t * clock, const mcu_pin_obj_t * mosi,
        const mcu_pin_obj_t * miso) {
    struct spi_config config_spi_master;
    spi_get_config_defaults(&config_spi_master);

    Sercom* sercom = NULL;
    uint32_t clock_pinmux = 0;
    bool mosi_none = mosi == mp_const_none;
    bool miso_none = miso == mp_const_none;
    uint32_t mosi_pinmux = 0;
    uint32_t miso_pinmux = 0;
    uint8_t clock_pad = 0;
    uint8_t mosi_pad = 0;
    uint8_t miso_pad = 0;
    for (int i = 0; i < NUM_SERCOMS_PER_PIN; i++) {
        Sercom* potential_sercom = clock->sercom[i].sercom;
        if (potential_sercom == NULL ||
        #if defined(MICROPY_HW_APA102_SCK) && defined(MICROPY_HW_APA102_MOSI) && !defined(CIRCUITPY_BITBANG_APA102)
            (potential_sercom->SPI.CTRLA.bit.ENABLE != 0 &&
             potential_sercom != status_apa102.spi_master_instance.hw &&
             !apa102_sck_in_use)) {
        #else
            potential_sercom->SPI.CTRLA.bit.ENABLE != 0) {
        #endif
            continue;
        }
        clock_pinmux = PINMUX(clock->pin, (i == 0) ? MUX_C : MUX_D);
        clock_pad = clock->sercom[i].pad;
        for (int j = 0; j < NUM_SERCOMS_PER_PIN; j++) {
            if (!mosi_none) {
                if(potential_sercom == mosi->sercom[j].sercom) {
                    mosi_pinmux = PINMUX(mosi->pin, (j == 0) ? MUX_C : MUX_D);
                    mosi_pad = mosi->sercom[j].pad;
                    if (miso_none) {
                        sercom = potential_sercom;
                        break;
                    }
                } else {
                    continue;
                }
            }
            if (!miso_none) {
                for (int k = 0; k < NUM_SERCOMS_PER_PIN; k++) {
                    if (potential_sercom == miso->sercom[k].sercom) {
                        miso_pinmux = PINMUX(miso->pin, (k == 0) ? MUX_C : MUX_D);
                        miso_pad = miso->sercom[k].pad;
                        sercom = potential_sercom;
                        break;
                    }
                }
            }
            if (sercom != NULL) {
                break;
            }
        }
        if (sercom != NULL) {
            break;
        }
    }
    if (sercom == NULL) {
        mp_raise_ValueError("Invalid pins");
    }

    // Depends on where MOSI and CLK are.
    uint8_t dopo = 8;
    if (clock_pad == 1) {
        if (mosi_pad == 0) {
            dopo = 0;
        } else if (mosi_pad == 3) {
            dopo = 2;
        }
    } else if (clock_pad == 3) {
        if (mosi_pad == 0) {
            dopo = 3;
        } else if (mosi_pad == 2) {
            dopo = 1;
        }
    }
    if (dopo == 8) {
        mp_raise_ValueError("MOSI and clock pins incompatible");
    }

    config_spi_master.mux_setting = (dopo << SERCOM_SPI_CTRLA_DOPO_Pos) |
        (miso_pad << SERCOM_SPI_CTRLA_DIPO_Pos);

    // Map pad to pinmux through a short array.
    uint32_t *pinmuxes[4] = {&config_spi_master.pinmux_pad0,
                             &config_spi_master.pinmux_pad1,
                             &config_spi_master.pinmux_pad2,
                             &config_spi_master.pinmux_pad3};
    // Set other pinmuxes to unused so we don't accidentally change other pin
    // state.
    for (uint8_t i = 0; i < 4; i++) {
        *pinmuxes[i] = PINMUX_UNUSED;
    }
    *pinmuxes[clock_pad] = clock_pinmux;
    self->clock_pin = clock->pin;
    claim_pin(clock);
    self->MOSI_pin = NO_PIN;
    if (!mosi_none) {
        *pinmuxes[mosi_pad] = mosi_pinmux;
        self->MOSI_pin = mosi->pin;
        claim_pin(mosi);
    }
    self->MISO_pin = NO_PIN;
    if (!miso_none) {
        *pinmuxes[miso_pad] = miso_pinmux;
        self->MISO_pin = miso->pin;
        claim_pin(miso);
    }

    // Always start at 250khz which is what SD cards need. They are sensitive to
    // SPI bus noise before they are put into SPI mode.
    self->current_baudrate = 250000;
    config_spi_master.mode_specific.master.baudrate = self->current_baudrate;

    spi_init(&self->spi_master_instance, sercom, &config_spi_master);

    spi_enable(&self->spi_master_instance);
}

void common_hal_busio_spi_deinit(busio_spi_obj_t *self) {
    spi_disable(&self->spi_master_instance);
    reset_pin(self->clock_pin);
    reset_pin(self->MOSI_pin);
    reset_pin(self->MISO_pin);
}

bool common_hal_busio_spi_configure(busio_spi_obj_t *self,
        uint32_t baudrate, uint8_t polarity, uint8_t phase, uint8_t bits) {
    // TODO(tannewt): Check baudrate first before changing it.
    if (baudrate != self->current_baudrate) {
        enum status_code status = spi_set_baudrate(&self->spi_master_instance, baudrate);
        if (status != STATUS_OK) {
            return false;
        }
    }

    SercomSpi *const spi_module = &(self->spi_master_instance.hw->SPI);
    // If the settings are already what we want then don't reset them.
    if (spi_module->CTRLA.bit.CPHA == phase &&
        spi_module->CTRLA.bit.CPOL == polarity &&
        spi_module->CTRLB.bit.CHSIZE == (bits - 8)) {
        return true;
    }

    spi_disable(&self->spi_master_instance);
    while (spi_is_syncing(&self->spi_master_instance)) {
        /* Wait until the synchronization is complete */
    }

    spi_module->CTRLA.bit.CPHA = phase;
    spi_module->CTRLA.bit.CPOL = polarity;
    spi_module->CTRLB.bit.CHSIZE = bits - 8;

    while (spi_is_syncing(&self->spi_master_instance)) {
        /* Wait until the synchronization is complete */
    }

    /* Enable the module */
    spi_enable(&self->spi_master_instance);

    while (spi_is_syncing(&self->spi_master_instance)) {
        /* Wait until the synchronization is complete */
    }

    return true;
}

bool common_hal_busio_spi_try_lock(busio_spi_obj_t *self) {
    self->has_lock = spi_lock(&self->spi_master_instance) == STATUS_OK;
    return self->has_lock;
}

bool common_hal_busio_spi_has_lock(busio_spi_obj_t *self) {
    return self->has_lock;
}

void common_hal_busio_spi_unlock(busio_spi_obj_t *self) {
    self->has_lock = false;
    spi_unlock(&self->spi_master_instance);
}

bool common_hal_busio_spi_write(busio_spi_obj_t *self,
        const uint8_t *data, size_t len) {
    if (len == 0) {
        return true;
    }
    enum status_code status;
    if (len >= 16) {
        status = shared_dma_write(self->spi_master_instance.hw, data, len);
    } else {
        status = spi_write_buffer_wait(&self->spi_master_instance, data, len);
    }
    return status == STATUS_OK;
}

bool common_hal_busio_spi_read(busio_spi_obj_t *self,
        uint8_t *data, size_t len, uint8_t write_value) {
    if (len == 0) {
        return true;
    }
    enum status_code status;
    if (len >= 16) {
        status = shared_dma_read(self->spi_master_instance.hw, data, len, write_value);
    } else {
        status = spi_read_buffer_wait(&self->spi_master_instance, data, len, write_value);
    }
    return status == STATUS_OK;
}
