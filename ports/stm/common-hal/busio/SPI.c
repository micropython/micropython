/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft
 * Copyright (c) 2019 Lucian Copeland for Adafruit Industries
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
#include <stdbool.h>
#include <string.h>

#include "shared-bindings/busio/SPI.h"
#include "py/mperrno.h"
#include "py/runtime.h"

#include "shared-bindings/microcontroller/__init__.h"
#include "supervisor/board.h"
#include "supervisor/shared/translate.h"
#include "shared-bindings/microcontroller/Pin.h"

// Note that any bugs introduced in this file can cause crashes at startup
// for chips using external SPI flash.

// arrays use 0 based numbering: SPI1 is stored at index 0
#define MAX_SPI 6

STATIC bool reserved_spi[MAX_SPI];
STATIC bool never_reset_spi[MAX_SPI];

#define ALL_CLOCKS 0xFF
STATIC void spi_clock_enable(uint8_t mask);
STATIC void spi_clock_disable(uint8_t mask);

STATIC uint32_t get_busclock(SPI_TypeDef *instance) {
    #if (CPY_STM32H7)
    if (instance == SPI1 || instance == SPI2 || instance == SPI3) {
        return HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SPI123);
    } else if (instance == SPI4 || instance == SPI5) {
        return HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SPI45);
    } else {
        return HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SPI6);
    }
    #elif (CPY_STM32F4 || CPY_STM32F7)
    // SPI2 and 3 are on PCLK1, if they exist.
    #ifdef SPI2
    if (instance == SPI2) {
        return HAL_RCC_GetPCLK1Freq();
    }
    #endif
    #ifdef SPI3
    if (instance == SPI3) {
        return HAL_RCC_GetPCLK1Freq();
    }
    #endif
    return HAL_RCC_GetPCLK2Freq();
    #endif
}

STATIC uint32_t stm32_baud_to_spi_div(uint32_t baudrate, uint16_t *prescaler, uint32_t busclock) {
    static const uint32_t baud_map[8][2] = {
        {2,SPI_BAUDRATEPRESCALER_2},
        {4,SPI_BAUDRATEPRESCALER_4},
        {8,SPI_BAUDRATEPRESCALER_8},
        {16,SPI_BAUDRATEPRESCALER_16},
        {32,SPI_BAUDRATEPRESCALER_32},
        {64,SPI_BAUDRATEPRESCALER_64},
        {128,SPI_BAUDRATEPRESCALER_128},
        {256,SPI_BAUDRATEPRESCALER_256}
    };
    size_t i = 0;
    uint16_t divisor;
    do {
        divisor = baud_map[i][0];
        if (baudrate >= (busclock / divisor)) {
            *prescaler = divisor;
            return baud_map[i][1];
        }
        i++;
    } while (divisor != 256);
    // only gets here if requested baud is lower than minimum
    *prescaler = 256;
    return SPI_BAUDRATEPRESCALER_256;
}

void spi_reset(void) {
    uint16_t never_reset_mask = 0x00;
    for (int i = 0; i < MAX_SPI; i++) {
        if (!never_reset_spi[i]) {
            reserved_spi[i] = false;
        } else {
            never_reset_mask |= 1 << i;
        }
    }
    spi_clock_disable(ALL_CLOCKS & ~(never_reset_mask));
}

STATIC const mcu_periph_obj_t *find_pin_function(const mcu_periph_obj_t *table, size_t sz, const mcu_pin_obj_t *pin, int periph_index) {
    for (size_t i = 0; i < sz; i++, table++) {
        if (periph_index == table->periph_index && pin == table->pin) {
            return table;
        }
    }
    return NULL;
}

// match pins to SPI objects
STATIC int check_pins(busio_spi_obj_t *self,
    const mcu_pin_obj_t *sck, const mcu_pin_obj_t *mosi,
    const mcu_pin_obj_t *miso) {
    bool spi_taken = false;

    uint8_t sck_len = MP_ARRAY_SIZE(mcu_spi_sck_list);
    uint8_t mosi_len = MP_ARRAY_SIZE(mcu_spi_mosi_list);
    uint8_t miso_len = MP_ARRAY_SIZE(mcu_spi_miso_list);

    // Loop over each possibility for SCK.  Check whether MISO and/or MOSI can be used on the same peripheral
    for (uint i = 0; i < sck_len; i++) {
        const mcu_periph_obj_t *mcu_spi_sck = &mcu_spi_sck_list[i];
        if (mcu_spi_sck->pin != sck) {
            continue;
        }

        int periph_index = mcu_spi_sck->periph_index;

        const mcu_periph_obj_t *mcu_spi_miso = NULL;
        if (miso && !(mcu_spi_miso = find_pin_function(mcu_spi_miso_list, miso_len, miso, periph_index))) {
            continue;
        }

        const mcu_periph_obj_t *mcu_spi_mosi = NULL;
        if (mosi && !(mcu_spi_mosi = find_pin_function(mcu_spi_mosi_list, mosi_len, mosi, periph_index))) {
            continue;
        }

        if (reserved_spi[periph_index - 1]) {
            spi_taken = true;
            continue;
        }

        self->sck = mcu_spi_sck;
        self->mosi = mcu_spi_mosi;
        self->miso = mcu_spi_miso;

        return periph_index;
    }

    if (spi_taken) {
        mp_raise_ValueError(translate("Hardware busy, try alternative pins"));
    } else {
        mp_raise_ValueError_varg(translate("Invalid %q pin selection"), MP_QSTR_SPI);
    }
}

void common_hal_busio_spi_construct(busio_spi_obj_t *self,
    const mcu_pin_obj_t *sck, const mcu_pin_obj_t *mosi,
    const mcu_pin_obj_t *miso) {

    int periph_index = check_pins(self, sck, mosi, miso);
    SPI_TypeDef *SPIx = mcu_spi_banks[periph_index - 1];

    // Start GPIO for each pin
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = pin_mask(sck->number);
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = self->sck->altfn_index;
    HAL_GPIO_Init(pin_port(sck->port), &GPIO_InitStruct);

    if (self->mosi != NULL) {
        GPIO_InitStruct.Pin = pin_mask(mosi->number);
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = self->mosi->altfn_index;
        HAL_GPIO_Init(pin_port(mosi->port), &GPIO_InitStruct);
    }

    if (self->miso != NULL) {
        GPIO_InitStruct.Pin = pin_mask(miso->number);
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = self->miso->altfn_index;
        HAL_GPIO_Init(pin_port(miso->port), &GPIO_InitStruct);
    }

    spi_clock_enable(1 << (self->sck->periph_index - 1));
    reserved_spi[self->sck->periph_index - 1] = true;

    self->handle.Instance = SPIx;
    self->handle.Init.Mode = SPI_MODE_MASTER;
    // Direction change only required for RX-only, see RefMan RM0090:884
    self->handle.Init.Direction = (self->mosi == NULL) ? SPI_DIRECTION_2LINES_RXONLY : SPI_DIRECTION_2LINES;
    self->handle.Init.DataSize = SPI_DATASIZE_8BIT;
    self->handle.Init.CLKPolarity = SPI_POLARITY_LOW;
    self->handle.Init.CLKPhase = SPI_PHASE_1EDGE;
    self->handle.Init.NSS = SPI_NSS_SOFT;
    self->handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
    self->handle.Init.FirstBit = SPI_FIRSTBIT_MSB;
    self->handle.Init.TIMode = SPI_TIMODE_DISABLE;
    self->handle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    self->handle.Init.CRCPolynomial = 10;
    if (HAL_SPI_Init(&self->handle) != HAL_OK) {
        mp_raise_ValueError(translate("SPI Init Error"));
    }
    self->baudrate = (get_busclock(SPIx) / 16);
    self->prescaler = 16;
    self->polarity = 0;
    self->phase = 0;
    self->bits = 8;

    common_hal_mcu_pin_claim(sck);
    if (self->mosi != NULL) {
        common_hal_mcu_pin_claim(mosi);
    }
    if (self->miso != NULL) {
        common_hal_mcu_pin_claim(miso);
    }
}

void common_hal_busio_spi_never_reset(busio_spi_obj_t *self) {

    never_reset_spi[self->sck->periph_index - 1] = true;
    never_reset_pin_number(self->sck->pin->port, self->sck->pin->number);
    if (self->mosi != NULL) {
        never_reset_pin_number(self->mosi->pin->port, self->mosi->pin->number);
    }
    if (self->miso != NULL) {
        never_reset_pin_number(self->miso->pin->port, self->miso->pin->number);
    }
}

bool common_hal_busio_spi_deinited(busio_spi_obj_t *self) {
    return self->sck->pin == NULL;
}

void common_hal_busio_spi_deinit(busio_spi_obj_t *self) {
    if (common_hal_busio_spi_deinited(self)) {
        return;
    }
    spi_clock_disable(1 << (self->sck->periph_index - 1));
    reserved_spi[self->sck->periph_index - 1] = false;
    never_reset_spi[self->sck->periph_index - 1] = false;

    reset_pin_number(self->sck->pin->port,self->sck->pin->number);
    if (self->mosi != NULL) {
        reset_pin_number(self->mosi->pin->port,self->mosi->pin->number);
    }
    if (self->miso != NULL) {
        reset_pin_number(self->miso->pin->port,self->miso->pin->number);
    }
    self->sck = NULL;
    self->mosi = NULL;
    self->miso = NULL;
}

bool common_hal_busio_spi_configure(busio_spi_obj_t *self,
    uint32_t baudrate, uint8_t polarity, uint8_t phase, uint8_t bits) {
    // This resets the SPI, so check before updating it redundantly
    if (baudrate == self->baudrate && polarity == self->polarity
        && phase == self->phase && bits == self->bits) {
        return true;
    }

    // Deinit SPI
    HAL_SPI_DeInit(&self->handle);

    self->handle.Init.DataSize = (bits == 16) ? SPI_DATASIZE_16BIT : SPI_DATASIZE_8BIT;
    self->handle.Init.CLKPolarity = (polarity) ? SPI_POLARITY_HIGH : SPI_POLARITY_LOW;
    self->handle.Init.CLKPhase = (phase) ? SPI_PHASE_2EDGE : SPI_PHASE_1EDGE;

    self->handle.Init.BaudRatePrescaler = stm32_baud_to_spi_div(baudrate, &self->prescaler,
        get_busclock(self->handle.Instance));

    if (HAL_SPI_Init(&self->handle) != HAL_OK) {
        mp_raise_ValueError(translate("SPI Re-initialization error"));
    }

    self->baudrate = baudrate;
    self->polarity = polarity;
    self->phase = phase;
    self->bits = bits;
    return true;
}

bool common_hal_busio_spi_try_lock(busio_spi_obj_t *self) {
    bool grabbed_lock = false;

    // Critical section code that may be required at some point.
    // uint32_t store_primask = __get_PRIMASK();
    // __disable_irq();
    // __DMB();

    if (!self->has_lock) {
        grabbed_lock = true;
        self->has_lock = true;
    }

    // __DMB();
    // __set_PRIMASK(store_primask);

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
    if (self->mosi == NULL) {
        mp_raise_ValueError(translate("No MOSI Pin"));
    }
    HAL_StatusTypeDef result = HAL_SPI_Transmit(&self->handle, (uint8_t *)data, (uint16_t)len, HAL_MAX_DELAY);
    return result == HAL_OK;
}

bool common_hal_busio_spi_read(busio_spi_obj_t *self,
    uint8_t *data, size_t len, uint8_t write_value) {
    if (self->miso == NULL) {
        mp_raise_ValueError(translate("No MISO Pin"));
    }
    HAL_StatusTypeDef result = HAL_OK;
    if (self->mosi == NULL) {
        result = HAL_SPI_Receive(&self->handle, data, (uint16_t)len, HAL_MAX_DELAY);
    } else {
        memset(data, write_value, len);
        result = HAL_SPI_TransmitReceive(&self->handle, data, data, (uint16_t)len, HAL_MAX_DELAY);
    }
    return result == HAL_OK;
}

bool common_hal_busio_spi_transfer(busio_spi_obj_t *self,
    const uint8_t *data_out, uint8_t *data_in, size_t len) {
    if (self->miso == NULL || self->mosi == NULL) {
        mp_raise_ValueError(translate("Missing MISO or MOSI Pin"));
    }
    HAL_StatusTypeDef result = HAL_SPI_TransmitReceive(&self->handle,
        (uint8_t *)data_out, data_in, (uint16_t)len,HAL_MAX_DELAY);
    return result == HAL_OK;
}

uint32_t common_hal_busio_spi_get_frequency(busio_spi_obj_t *self) {
    // returns actual frequency
    uint32_t result = HAL_RCC_GetPCLK2Freq() / self->prescaler;
    return result;
}

uint8_t common_hal_busio_spi_get_phase(busio_spi_obj_t *self) {
    return self->phase;
}

uint8_t common_hal_busio_spi_get_polarity(busio_spi_obj_t *self) {
    return self->polarity;
}

STATIC void spi_clock_enable(uint8_t mask) {
    #ifdef SPI1
    if (mask & (1 << 0)) {
        __HAL_RCC_SPI1_CLK_ENABLE();
    }
    #endif
    #ifdef SPI2
    if (mask & (1 << 1)) {
        __HAL_RCC_SPI2_CLK_ENABLE();
    }
    #endif
    #ifdef SPI3
    if (mask & (1 << 2)) {
        __HAL_RCC_SPI3_CLK_ENABLE();
    }
    #endif
    #ifdef SPI4
    if (mask & (1 << 3)) {
        __HAL_RCC_SPI4_CLK_ENABLE();
    }
    #endif
    #ifdef SPI5
    if (mask & (1 << 4)) {
        __HAL_RCC_SPI5_CLK_ENABLE();
    }
    #endif
    #ifdef SPI6
    if (mask & (1 << 5)) {
        __HAL_RCC_SPI6_CLK_ENABLE();
    }
    #endif
}

STATIC void spi_clock_disable(uint8_t mask) {
    #ifdef SPI1
    if (mask & (1 << 0)) {
        __HAL_RCC_SPI1_CLK_DISABLE();
        __HAL_RCC_SPI1_FORCE_RESET();
        __HAL_RCC_SPI1_RELEASE_RESET();
    }
    #endif
    #ifdef SPI2
    if (mask & (1 << 1)) {
        __HAL_RCC_SPI2_CLK_DISABLE();
        __HAL_RCC_SPI2_FORCE_RESET();
        __HAL_RCC_SPI2_RELEASE_RESET();
    }
    #endif
    #ifdef SPI3
    if (mask & (1 << 2)) {
        __HAL_RCC_SPI3_CLK_DISABLE();
        __HAL_RCC_SPI3_FORCE_RESET();
        __HAL_RCC_SPI3_RELEASE_RESET();
    }
    #endif
    #ifdef SPI4
    if (mask & (1 << 3)) {
        __HAL_RCC_SPI4_CLK_DISABLE();
        __HAL_RCC_SPI4_FORCE_RESET();
        __HAL_RCC_SPI4_RELEASE_RESET();
    }
    #endif
    #ifdef SPI5
    if (mask & (1 << 4)) {
        __HAL_RCC_SPI5_CLK_DISABLE();
        __HAL_RCC_SPI5_FORCE_RESET();
        __HAL_RCC_SPI5_RELEASE_RESET();
    }
    #endif
    #ifdef SPI6
    if (mask & (1 << 5)) {
        __HAL_RCC_SPI6_CLK_DISABLE();
        __HAL_RCC_SPI6_FORCE_RESET();
        __HAL_RCC_SPI6_RELEASE_RESET();
    }
    #endif
}
