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
#include "py/mperrno.h"
#include "py/runtime.h"

#include "boards/board.h"
#include "common-hal/microcontroller/Pin.h"
#include "supervisor/shared/rgb_led_status.h"

void spi_reset(void) {

}

void common_hal_busio_spi_construct(busio_spi_obj_t *self,
        const mcu_pin_obj_t * clock, const mcu_pin_obj_t * mosi,
        const mcu_pin_obj_t * miso) {
    // uint8_t sercom_index;
    // uint32_t clock_pinmux = 0;
    // bool mosi_none = mosi == NULL;
    // bool miso_none = miso == NULL;
    // uint32_t mosi_pinmux = 0;
    // uint32_t miso_pinmux = 0;
    // uint8_t clock_pad = 0;
    // uint8_t mosi_pad = 0;
    // uint8_t miso_pad = 0;
    // uint8_t dopo = 255;

    // if (sercom == NULL) {
    //     mp_raise_ValueError(translate("Invalid pins"));
    // }

    // // Set up SPI clocks on SERCOM.
    // samd_peripherals_sercom_clock_init(sercom, sercom_index);

    #if defined(MICROPY_HW_APA102_SCK) && defined(MICROPY_HW_APA102_MOSI) && !CIRCUITPY_BITBANG_APA102
    // // if we're re-using the dotstar sercom, make sure it is disabled or the init will fail out
    // hri_sercomspi_clear_CTRLA_ENABLE_bit(sercom);
    #endif
    // if (spi_m_sync_init(&self->spi_desc, sercom) != ERR_NONE) {
    //     mp_raise_OSError(MP_EIO);
    // }

    // Pads must be set after spi_m_sync_init(), which uses default values from
    // the prototypical SERCOM.
    // hri_sercomspi_write_CTRLA_DOPO_bf(sercom, dopo);
    // hri_sercomspi_write_CTRLA_DIPO_bf(sercom, miso_pad);

    // Always start at 250khz which is what SD cards need. They are sensitive to
    // SPI bus noise before they are put into SPI mode.
    // uint8_t baud_value = samd_peripherals_spi_baudrate_to_baud_reg_value(250000);
    // if (spi_m_sync_set_baudrate(&self->spi_desc, baud_value) != ERR_NONE) {
    //     // spi_m_sync_set_baudrate does not check for validity, just whether the device is
    //     // busy or not
    //     mp_raise_OSError(MP_EIO);
    // }

    // gpio_set_pin_direction(clock->number, GPIO_DIRECTION_OUT);
    // gpio_set_pin_pull_mode(clock->number, GPIO_PULL_OFF);
    // gpio_set_pin_function(clock->number, clock_pinmux);
    // claim_pin(clock);
    // self->clock_pin = clock->number;

    // if (mosi_none) {
    //     self->MOSI_pin = NO_PIN;
    // } else {
    //     gpio_set_pin_direction(mosi->number, GPIO_DIRECTION_OUT);
    //     gpio_set_pin_pull_mode(mosi->number, GPIO_PULL_OFF);
    //     gpio_set_pin_function(mosi->number, mosi_pinmux);
    //     self->MOSI_pin = mosi->number;
    //     claim_pin(mosi);
    // }

    // if (miso_none) {
    //     self->MISO_pin = NO_PIN;
    // } else {
    //     gpio_set_pin_direction(miso->number, GPIO_DIRECTION_IN);
    //     gpio_set_pin_pull_mode(miso->number, GPIO_PULL_OFF);
    //     gpio_set_pin_function(miso->number, miso_pinmux);
    //     self->MISO_pin = miso->number;
    //     claim_pin(miso);
    // }

    // spi_m_sync_enable(&self->spi_desc);
}

void common_hal_busio_spi_never_reset(busio_spi_obj_t *self) {
    // never_reset_sercom(self->spi_desc.dev.prvt);

    never_reset_pin(self->clock_pin);
    never_reset_pin(self->MOSI_pin);
    never_reset_pin(self->MISO_pin);
}

bool common_hal_busio_spi_deinited(busio_spi_obj_t *self) {
    return self->clock_pin == NULL;
}

void common_hal_busio_spi_deinit(busio_spi_obj_t *self) {
    if (common_hal_busio_spi_deinited(self)) {
        return;
    }
    // allow_reset_sercom(self->spi_desc.dev.prvt);

    // spi_m_sync_disable(&self->spi_desc);
    // spi_m_sync_deinit(&self->spi_desc);
    reset_pin(self->clock_pin);
    reset_pin(self->MOSI_pin);
    reset_pin(self->MISO_pin);
    self->clock_pin = NULL;
}

bool common_hal_busio_spi_configure(busio_spi_obj_t *self,
        uint32_t baudrate, uint8_t polarity, uint8_t phase, uint8_t bits) {
    // If the settings are already what we want then don't reset them.
    // if (hri_sercomspi_get_CTRLA_CPHA_bit(hw) == phase &&
    //     hri_sercomspi_get_CTRLA_CPOL_bit(hw) == polarity &&
    //     hri_sercomspi_read_CTRLB_CHSIZE_bf(hw) == ((uint32_t)bits - 8) &&
    //     hri_sercomspi_read_BAUD_BAUD_bf(hw) == baud_reg_value) {
    //     return true;
    // }

    // Disable, set values (most or all are enable-protected), and re-enable.
    // spi_m_sync_disable(&self->spi_desc);
    // hri_sercomspi_wait_for_sync(hw, SERCOM_SPI_SYNCBUSY_MASK);

    // hri_sercomspi_write_CTRLA_CPHA_bit(hw, phase);
    // hri_sercomspi_write_CTRLA_CPOL_bit(hw, polarity);
    // hri_sercomspi_write_CTRLB_CHSIZE_bf(hw, bits - 8);
    // hri_sercomspi_write_BAUD_BAUD_bf(hw, baud_reg_value);
    // hri_sercomspi_wait_for_sync(hw, SERCOM_SPI_SYNCBUSY_MASK);

    // spi_m_sync_enable(&self->spi_desc);
    // hri_sercomspi_wait_for_sync(hw, SERCOM_SPI_SYNCBUSY_MASK);

    return true;
}

bool common_hal_busio_spi_try_lock(busio_spi_obj_t *self) {
    bool grabbed_lock = false;
    // CRITICAL_SECTION_ENTER()
        if (!self->has_lock) {
            grabbed_lock = true;
            self->has_lock = true;
        }
    // CRITICAL_SECTION_LEAVE();
    return grabbed_lock;
}

bool common_hal_busio_spi_has_lock(busio_spi_obj_t *self) {
    return self->has_lock;
}

void common_hal_busio_spi_unlock(busio_spi_obj_t *self) {
    self->has_lock = false;
}

bool common_hal_busio_spi_write(busio_spi_obj_t *self,
        const uint8_t *data, size_t len) {
    if (len == 0) {
        return true;
    }
    // int32_t status;
    if (len >= 16) {
        // status = sercom_dma_write(self->spi_desc.dev.prvt, data, len);
    } else {
        // struct io_descriptor *spi_io;
        // spi_m_sync_get_io_descriptor(&self->spi_desc, &spi_io);
        // status = spi_io->write(spi_io, data, len);
    }
    return false; // Status is number of chars read or an error code < 0.
}

bool common_hal_busio_spi_read(busio_spi_obj_t *self,
        uint8_t *data, size_t len, uint8_t write_value) {
    if (len == 0) {
        return true;
    }
    // int32_t status;
    if (len >= 16) {
        // status = sercom_dma_read(self->spi_desc.dev.prvt, data, len, write_value);
    } else {
        // self->spi_desc.dev.dummy_byte = write_value;

        // struct io_descriptor *spi_io;
        // spi_m_sync_get_io_descriptor(&self->spi_desc, &spi_io);

        // status = spi_io->read(spi_io, data, len);
    }
    return false; // Status is number of chars read or an error code < 0.
}

bool common_hal_busio_spi_transfer(busio_spi_obj_t *self, uint8_t *data_out, uint8_t *data_in, size_t len) {
    if (len == 0) {
        return true;
    }
    // int32_t status;
    if (len >= 16) {
        // status = sercom_dma_transfer(self->spi_desc.dev.prvt, data_out, data_in, len);
    } else {
        // struct spi_xfer xfer;
        // xfer.txbuf = data_out;
        // xfer.rxbuf = data_in;
        // xfer.size = len;
        // status = spi_m_sync_transfer(&self->spi_desc, &xfer);
    }
    return false; // Status is number of chars read or an error code < 0.
}

uint32_t common_hal_busio_spi_get_frequency(busio_spi_obj_t* self) {
    // return samd_peripherals_spi_baud_reg_value_to_baudrate(hri_sercomspi_read_BAUD_reg(self->spi_desc.dev.prvt));
    return 0;
}

uint8_t common_hal_busio_spi_get_phase(busio_spi_obj_t* self) {
    // void * hw = self->spi_desc.dev.prvt;
    // return hri_sercomspi_get_CTRLA_CPHA_bit(hw);
    return 0;
}

uint8_t common_hal_busio_spi_get_polarity(busio_spi_obj_t* self) {
    // void * hw = self->spi_desc.dev.prvt;
    // return hri_sercomspi_get_CTRLA_CPOL_bit(hw);
    return 0;
}
