/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
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

#include "shared/runtime/interrupt_char.h"
#include "py/mperrno.h"
#include "py/runtime.h"

#include "supervisor/board.h"
#include "common-hal/microcontroller/Pin.h"
#include "shared-bindings/microcontroller/Pin.h"

#include "peripherals/broadcom/cpu.h"
#include "peripherals/broadcom/defines.h"
#include "peripherals/broadcom/gpio.h"
#include "peripherals/broadcom/pins.h"
#include "peripherals/broadcom/vcmailbox.h"

#if BCM_VERSION == 2711
#define NUM_SPI (7)
STATIC SPI0_Type *spi[NUM_SPI] = {SPI0, NULL, NULL, SPI3, SPI4, SPI5, SPI6};
STATIC SPI1_Type *aux_spi[NUM_SPI] = {NULL, SPI1, SPI2, NULL, NULL, NULL, NULL};
#else
#define NUM_SPI (3)
STATIC SPI0_Type *spi[NUM_SPI] = {SPI0, NULL, NULL};
STATIC SPI1_Type *aux_spi[NUM_SPI] = {NULL, SPI1, SPI2};
#endif

STATIC bool never_reset_spi[NUM_SPI];
STATIC bool spi_in_use[NUM_SPI];

void reset_spi(void) {
    for (size_t i = 0; i < NUM_SPI; i++) {
        if (never_reset_spi[i]) {
            continue;
        }

        if (i == 1 || i == 2) {
            if (i == 1) {
                AUX->ENABLES_b.SPI_1 = false;
            } else {
                AUX->ENABLES_b.SPI_2 = false;
            }
            aux_spi[i]->CNTL0 = 0;
        } else {
            // Set CS back to default. All 0 except read enable.
            spi[i]->CS = SPI0_CS_REN_Msk;
        }

        spi_in_use[i] = false;
    }
}

void common_hal_busio_spi_construct(busio_spi_obj_t *self,
    const mcu_pin_obj_t *clock, const mcu_pin_obj_t *mosi,
    const mcu_pin_obj_t *miso, bool half_duplex) {
    size_t instance_index = NUM_SPI;
    BP_Function_Enum clock_alt = 0;
    BP_Function_Enum mosi_alt = 0;
    BP_Function_Enum miso_alt = 0;

    if (half_duplex) {
        mp_raise_NotImplementedError(MP_ERROR_TEXT("Half duplex SPI is not implemented"));
    }

    // BCM_VERSION != 2711 have 3 SPI but as listed in peripherals/gen/pins.c two are on
    // index 0, once one index 0 SPI is found the other will throw an invalid_pins error.
    for (size_t i = 0; i < NUM_SPI; i++) {
        if (spi_in_use[i]) {
            continue;
        }
        if (!pin_find_alt(clock, PIN_FUNCTION_SPI, i, SPI_FUNCTION_SCLK, &clock_alt)) {
            continue;
        }
        if (mosi != NULL && !pin_find_alt(mosi, PIN_FUNCTION_SPI, i, SPI_FUNCTION_MOSI, &mosi_alt)) {
            continue;
        }
        if (miso != NULL && !pin_find_alt(miso, PIN_FUNCTION_SPI, i, SPI_FUNCTION_MISO, &miso_alt)) {
            continue;
        }
        instance_index = i;
        break;
    }
    if (instance_index == NUM_SPI) {
        raise_ValueError_invalid_pins();
    }

    self->clock = clock;
    self->MOSI = mosi;
    self->MISO = miso;
    self->index = instance_index;
    spi_in_use[instance_index] = true;


    if (instance_index == 1) {
        AUX->ENABLES_b.SPI_1 = true;
    } else if (instance_index == 2) {
        AUX->ENABLES_b.SPI_2 = true;
    }

    common_hal_busio_spi_configure(self, 250000, 0, 0, 8);

    COMPLETE_MEMORY_READS;
    gpio_set_pull(clock->number, BP_PULL_NONE);
    gpio_set_function(clock->number, clock_alt);
    if (mosi != NULL) {
        gpio_set_pull(mosi->number, BP_PULL_NONE);
        gpio_set_function(mosi->number, mosi_alt);
    }
    if (miso != NULL) {
        gpio_set_pull(miso->number, BP_PULL_NONE);
        gpio_set_function(miso->number, miso_alt);
    }
}

void common_hal_busio_spi_never_reset(busio_spi_obj_t *self) {
    never_reset_spi[self->index] = true;

    common_hal_never_reset_pin(self->clock);
    common_hal_never_reset_pin(self->MOSI);
    common_hal_never_reset_pin(self->MISO);
}

bool common_hal_busio_spi_deinited(busio_spi_obj_t *self) {
    return self->clock == NULL;
}

void common_hal_busio_spi_deinit(busio_spi_obj_t *self) {
    if (common_hal_busio_spi_deinited(self)) {
        return;
    }
    never_reset_spi[self->index] = false;

    common_hal_reset_pin(self->clock);
    common_hal_reset_pin(self->MOSI);
    common_hal_reset_pin(self->MISO);
    self->clock = NULL;
    spi_in_use[self->index] = false;

    if (self->index == 1 ||
        self->index == 2) {
        aux_spi[self->index]->CNTL0_b.ENABLE = false;
        if (self->index == 1) {
            AUX->ENABLES_b.SPI_1 = false;
        } else if (self->index == 2) {
            AUX->ENABLES_b.SPI_2 = false;
        }
    }
}

bool common_hal_busio_spi_configure(busio_spi_obj_t *self,
    uint32_t baudrate, uint8_t polarity, uint8_t phase, uint8_t bits) {
    if (baudrate == self->target_frequency &&
        polarity == self->polarity &&
        phase == self->phase &&
        bits == self->bits) {
        return true;
    }

    if (self->index == 1 || self->index == 2) {
        SPI1_Type *p = aux_spi[self->index];
        uint32_t source_clock = vcmailbox_get_clock_rate(VCMAILBOX_CLOCK_CORE);
        uint16_t clock_divider = source_clock / baudrate;
        if (source_clock % baudrate > 0) {
            clock_divider += 2;
        }

        p->CNTL0 = (clock_divider / 2 - 1) << SPI1_CNTL0_SPEED_Pos |
            SPI1_CNTL0_ENABLE_Msk |
            SPI1_CNTL0_MSB_FIRST_Msk |
                    (polarity == 1? SPI1_CNTL0_INVERT_CLK_Msk : 0) |
                    (phase == polarity? SPI1_CNTL0_IN_RISING_Msk : SPI1_CNTL0_OUT_RISING_Msk) |
            8 << SPI1_CNTL0_SHIFT_LENGTH_Pos;
        p->CNTL1 = SPI1_CNTL1_MSB_FIRST_Msk;
        self->real_frequency = source_clock / clock_divider;
    } else {
        SPI0_Type *p = spi[self->index];
        p->CS = polarity << SPI0_CS_CPOL_Pos |
            phase << SPI0_CS_CPHA_Pos;
        uint32_t source_clock = vcmailbox_get_clock_rate(VCMAILBOX_CLOCK_CORE);
        uint16_t clock_divider = source_clock / baudrate;
        if (source_clock % baudrate > 0) {
            clock_divider += 2;
        }

        p->CLK = clock_divider;
        self->real_frequency = source_clock / clock_divider;
    }

    self->polarity = polarity;
    self->phase = phase;
    self->bits = bits;
    self->target_frequency = baudrate;

    return true;
}

bool common_hal_busio_spi_try_lock(busio_spi_obj_t *self) {
    bool grabbed_lock = false;
    if (!self->has_lock) {
        grabbed_lock = true;
        self->has_lock = true;
    }
    return grabbed_lock;
}

bool common_hal_busio_spi_has_lock(busio_spi_obj_t *self) {
    return self->has_lock;
}

void common_hal_busio_spi_unlock(busio_spi_obj_t *self) {
    self->has_lock = false;
}

STATIC void _spi_transfer(SPI0_Type *p,
    const uint8_t *data_out, size_t out_len,
    uint8_t *data_in, size_t in_len) {
    size_t len = MAX(out_len, in_len);
    COMPLETE_MEMORY_READS;
    p->DLEN = len;
    p->CS |= SPI0_CS_TA_Msk | SPI0_CS_CLEAR_Msk;
    size_t in = 0;
    size_t out = 0;
    while (in < len) {
        while (out < len && p->CS_b.TXD == 1) {
            if (out_len == 1) {
                p->FIFO = data_out[0];
            } else {
                p->FIFO = data_out[out];
            }
            out++;
        }
        // Wait for data to read (also means data has been sent.)
        while (p->CS_b.RXD == 0) {
            RUN_BACKGROUND_TASKS;
        }
        while (p->CS_b.RXD == 1) {
            uint8_t data = p->FIFO;
            if (data_in != NULL) {
                data_in[in] = data;
            } else {
                (void)data;
            }
            in++;
        }
    }
    p->CS_b.TA = false;
    COMPLETE_MEMORY_READS;
}

STATIC void _aux_spi_transfer(SPI1_Type *p,
    const uint8_t *data_out, size_t out_len,
    uint8_t *data_in, size_t in_len) {
    size_t len = MAX(out_len, in_len);
    p->CNTL0 |= SPI1_CNTL0_CLEAR_FIFOS_Msk;
    p->CNTL0 &= ~SPI1_CNTL0_CLEAR_FIFOS_Msk;
    size_t in = 0;
    size_t out = 0;
    while (in < len) {
        while (out < len && p->STAT_b.TX_FULL == 0) {
            if (out_len == 1) {
                p->TXHOLD0 = data_out[0] << 24;
            } else {
                p->TXHOLD0 = data_out[out] << 24;
            }
            out++;
        }
        // Wait for data to read (also means data has been sent.)
        while (p->STAT_b.RX_EMPTY == 1) {
            RUN_BACKGROUND_TASKS;
        }
        while (p->STAT_b.RX_EMPTY == 0) {
            uint8_t data = p->TXHOLD0;
            if (data_in != NULL) {
                data_in[in] = data;
            } else {
                (void)data;
            }
            in++;
        }
    }
}

bool common_hal_busio_spi_write(busio_spi_obj_t *self,
    const uint8_t *data, size_t len) {
    if (self->index == 1 || self->index == 2) {
        _aux_spi_transfer(aux_spi[self->index], data, len, NULL, 0);
    } else {
        _spi_transfer(spi[self->index], data, len, NULL, 0);
    }
    return true;
}

bool common_hal_busio_spi_read(busio_spi_obj_t *self,
    uint8_t *data, size_t len, uint8_t write_value) {
    if (self->index == 1 || self->index == 2) {
        _aux_spi_transfer(aux_spi[self->index], &write_value, 1, data, len);
    } else {
        _spi_transfer(spi[self->index], &write_value, 1, data, len);
    }
    return true;
}

bool common_hal_busio_spi_transfer(busio_spi_obj_t *self,
    const uint8_t *data_out, uint8_t *data_in, size_t len) {
    if (self->index == 1 || self->index == 2) {
        _aux_spi_transfer(aux_spi[self->index], data_out, len, data_in, len);
    } else {
        _spi_transfer(spi[self->index], data_out, len, data_in, len);
    }
    return true;
}

uint32_t common_hal_busio_spi_get_frequency(busio_spi_obj_t *self) {
    return self->real_frequency;
}

uint8_t common_hal_busio_spi_get_phase(busio_spi_obj_t *self) {
    return self->phase;
}

uint8_t common_hal_busio_spi_get_polarity(busio_spi_obj_t *self) {
    return self->polarity;
}
