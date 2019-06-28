/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Sandeep Mistry All right reserved.
 * Copyright (c) 2017 hathach
 * Copyright (c) 2018 Artur Pacholec
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

#include "shared-bindings/busio/I2C.h"
#include "py/mperrno.h"
#include "py/runtime.h"
#include "supervisor/shared/translate.h"

#include "nrfx_twim.h"
#include "nrf_gpio.h"

#include "nrfx_spim.h"
#include "nrf_gpio.h"

// all TWI instances have the same max size
// 16 bits for 840, 10 bits for 810, 8 bits for 832
#define I2C_MAX_XFER_LEN         ((1UL << TWIM0_EASYDMA_MAXCNT_SIZE) - 1)

STATIC twim_peripheral_t twim_peripherals[] = {
#if NRFX_CHECK(NRFX_TWIM0_ENABLED)
    // SPIM0 and TWIM0 share an address.
    { .twim = NRFX_TWIM_INSTANCE(0),
      .in_use = false
    },
#endif
#if NRFX_CHECK(NRFX_TWIM1_ENABLED)
    // SPIM1 and TWIM1 share an address.
    { .twim = NRFX_TWIM_INSTANCE(1),
      .in_use = false
    },
#endif
};

void i2c_reset(void) {
    for (size_t i = 0 ; i < MP_ARRAY_SIZE(twim_peripherals); i++) {
        nrf_twim_disable(twim_peripherals[i].twim.p_twim);
        twim_peripherals[i].in_use = false;
    }
}

static uint8_t twi_error_to_mp(const nrfx_err_t err) {
    switch (err) {
    case NRFX_ERROR_DRV_TWI_ERR_ANACK:
        return MP_ENODEV;
    case NRFX_ERROR_BUSY:
        return MP_EBUSY;
    case NRFX_ERROR_DRV_TWI_ERR_DNACK:
    case NRFX_ERROR_INVALID_ADDR:
        return MP_EIO;
    default:
        break;
    }

    return 0;
}

void common_hal_busio_i2c_construct(busio_i2c_obj_t *self, const mcu_pin_obj_t *scl, const mcu_pin_obj_t *sda, uint32_t frequency, uint32_t timeout) {
    if (scl->number == sda->number) {
        mp_raise_ValueError(translate("Invalid pins"));
    }

    // Find a free instance.
    self->twim_peripheral = NULL;
    for (size_t i = 0 ; i < MP_ARRAY_SIZE(twim_peripherals); i++) {
        if (!twim_peripherals[i].in_use) {
            self->twim_peripheral = &twim_peripherals[i];
            self->twim_peripheral->in_use = true;
            break;
        }
    }

    if (self->twim_peripheral == NULL) {
        mp_raise_ValueError(translate("All I2C peripherals are in use"));
    }

    nrfx_twim_config_t config = NRFX_TWIM_DEFAULT_CONFIG;
    config.scl = scl->number;
    config.sda = sda->number;

    // change freq. only if it's less than the default 400K
    if (frequency < 100000) {
        config.frequency = NRF_TWIM_FREQ_100K;
    } else if (frequency < 250000) {
      config.frequency = NRF_TWIM_FREQ_250K;
    }

    self->scl_pin_number = scl->number;
    self->sda_pin_number = sda->number;
    claim_pin(sda);
    claim_pin(scl);

    nrfx_err_t err = nrfx_twim_init(&self->twim_peripheral->twim, &config, NULL, NULL);

    // A soft reset doesn't uninit the driver so we might end up with a invalid state
    if (err == NRFX_ERROR_INVALID_STATE) {
        nrfx_twim_uninit(&self->twim_peripheral->twim);
        err = nrfx_twim_init(&self->twim_peripheral->twim, &config, NULL, NULL);
    }

    if (err != NRFX_SUCCESS) {
        common_hal_busio_i2c_deinit(self);
        mp_raise_OSError(MP_EIO);
    }

}

bool common_hal_busio_i2c_deinited(busio_i2c_obj_t *self) {
    return self->sda_pin_number == NO_PIN;
}

void common_hal_busio_i2c_deinit(busio_i2c_obj_t *self) {
    if (common_hal_busio_i2c_deinited(self))
        return;

    nrfx_twim_uninit(&self->twim_peripheral->twim);

    reset_pin_number(self->sda_pin_number);
    reset_pin_number(self->scl_pin_number);
    self->sda_pin_number = NO_PIN;
    self->scl_pin_number = NO_PIN;

    self->twim_peripheral->in_use = false;
}

// nrfx_twim_tx doesn't support 0-length data so we fall back to the hal API
bool common_hal_busio_i2c_probe(busio_i2c_obj_t *self, uint8_t addr) {
    NRF_TWIM_Type *reg = self->twim_peripheral->twim.p_twim;
    bool found = true;

    nrfx_twim_enable(&self->twim_peripheral->twim);

    nrf_twim_address_set(reg, addr);
    nrf_twim_tx_buffer_set(reg, NULL, 0);

    nrf_twim_task_trigger(reg, NRF_TWIM_TASK_RESUME);

    nrf_twim_task_trigger(reg, NRF_TWIM_TASK_STARTTX);
    while (nrf_twim_event_check(reg, NRF_TWIM_EVENT_TXSTARTED) == 0 &&
        nrf_twim_event_check(reg, NRF_TWIM_EVENT_ERROR) == 0);
    nrf_twim_event_clear(reg, NRF_TWIM_EVENT_TXSTARTED);

    nrf_twim_task_trigger(reg, NRF_TWIM_TASK_STOP);
    while (nrf_twim_event_check(reg, NRF_TWIM_EVENT_STOPPED) == 0);
    nrf_twim_event_clear(reg, NRF_TWIM_EVENT_STOPPED);

    if (nrf_twim_event_check(reg, NRF_TWIM_EVENT_ERROR)) {
        nrf_twim_event_clear(reg, NRF_TWIM_EVENT_ERROR);

        nrf_twim_errorsrc_get_and_clear(reg);
        found = false;
    }

    nrfx_twim_disable(&self->twim_peripheral->twim);

    return found;
}

bool common_hal_busio_i2c_try_lock(busio_i2c_obj_t *self) {
    bool grabbed_lock = false;
    // NRFX_CRITICAL_SECTION_ENTER();
    if (!self->has_lock) {
        grabbed_lock = true;
        self->has_lock = true;
    }
    // NRFX_CRITICAL_SECTION_EXIT();
    return grabbed_lock;
}

bool common_hal_busio_i2c_has_lock(busio_i2c_obj_t *self) {
    return self->has_lock;
}

void common_hal_busio_i2c_unlock(busio_i2c_obj_t *self) {
    self->has_lock = false;
}

uint8_t common_hal_busio_i2c_write(busio_i2c_obj_t *self, uint16_t addr, const uint8_t *data, size_t len, bool stopBit) {
    if(len == 0) {
        return common_hal_busio_i2c_probe(self, addr) ? 0 : MP_ENODEV;
    }

    nrfx_err_t err = NRFX_SUCCESS;

    nrfx_twim_enable(&self->twim_peripheral->twim);

    // break into MAX_XFER_LEN transaction
    while ( len ) {
        const size_t xact_len = MIN(len, I2C_MAX_XFER_LEN);

        if ( NRFX_SUCCESS != (err = nrfx_twim_tx(&self->twim_peripheral->twim, addr, data, xact_len, !stopBit)) ) {
            break;
        }

        len -= xact_len;
        data += xact_len;
    }

    nrfx_twim_disable(&self->twim_peripheral->twim);

    return twi_error_to_mp(err);
}

uint8_t common_hal_busio_i2c_read(busio_i2c_obj_t *self, uint16_t addr, uint8_t *data, size_t len) {
    if(len == 0) {
        return 0;
    }

    nrfx_err_t err = NRFX_SUCCESS;

    nrfx_twim_enable(&self->twim_peripheral->twim);

    // break into MAX_XFER_LEN transaction
    while ( len ) {
        const size_t xact_len = MIN(len, I2C_MAX_XFER_LEN);

        if ( NRFX_SUCCESS != (err = nrfx_twim_rx(&self->twim_peripheral->twim, addr, data, xact_len)) ) {
            break;
        }

        len -= xact_len;
        data += xact_len;
    }

    nrfx_twim_disable(&self->twim_peripheral->twim);

    return twi_error_to_mp(err);
}
