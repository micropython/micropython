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

#include "shared-bindings/busio/SPI.h"
#include "py/mperrno.h"
#include "py/runtime.h"
#include "stm32f4xx_hal.h"

#include "shared-bindings/microcontroller/__init__.h"
#include "boards/board.h"
#include "supervisor/shared/translate.h"
#include "common-hal/microcontroller/Pin.h"

STATIC bool reserved_spi[6];
STATIC bool never_reset_spi[6];

void spi_reset(void) {
    #ifdef SPI1
    if(!never_reset_spi[0]) {
        reserved_spi[0] = false;
        __HAL_RCC_SPI1_CLK_DISABLE(); 
    }
    #endif
    #ifdef SPI2
    if(!never_reset_spi[1]) {
        reserved_spi[1] = false;
        __HAL_RCC_SPI2_CLK_DISABLE(); 
    }
    #endif
    #ifdef SPI3
    if(!never_reset_spi[2]) {
        reserved_spi[2] = false;
        __HAL_RCC_SPI3_CLK_DISABLE(); 
    }
    #endif
    #ifdef SPI4
    if(!never_reset_spi[3]) {
        reserved_spi[3] = false;
        __HAL_RCC_SPI4_CLK_DISABLE();
    } 
    #endif
    #ifdef SPI5
    if(!never_reset_spi[4]) {
        reserved_spi[4] = false;
        __HAL_RCC_SPI5_CLK_DISABLE();
    } 
    #endif
    #ifdef SPI6
    if(!never_reset_spi[5]) {
        reserved_spi[5] = false;
        __HAL_RCC_SPI6_CLK_DISABLE(); 
    }
    #endif
}

void common_hal_busio_spi_construct(busio_spi_obj_t *self,
         const mcu_pin_obj_t * sck, const mcu_pin_obj_t * mosi,
         const mcu_pin_obj_t * miso) {

    //match pins to SPI objects
    SPI_TypeDef * SPIx;

    uint8_t sck_len = sizeof(mcu_spi_sck_list)/sizeof(*mcu_spi_sck_list);
    uint8_t mosi_len = sizeof(mcu_spi_mosi_list)/sizeof(*mcu_spi_mosi_list);
    uint8_t miso_len = sizeof(mcu_spi_miso_list)/sizeof(*mcu_spi_miso_list);

    bool spi_taken = false;
    //sck
    for(uint i=0; i<sck_len;i++) {
        if (mcu_spi_sck_list[i].pin == sck) {
            //mosi
            for(uint j=0; j<mosi_len;j++) {
                if (mcu_spi_mosi_list[j].pin == mosi) {
                    //miso
                    for(uint k=0; k<miso_len;k++) {
                        if ((mcu_spi_miso_list[k].pin == miso) //everything needs the same index
                            && (mcu_spi_sck_list[i].spi_index == mcu_spi_mosi_list[j].spi_index)
                            && (mcu_spi_sck_list[i].spi_index == mcu_spi_miso_list[k].spi_index)) {
                            //keep looking if the SPI is taken, edge case
                            if(reserved_spi[mcu_spi_sck_list[i].spi_index-1]) {
                                spi_taken = true;
                                continue;
                            }
                            //store pins if not
                            self->sck = &mcu_spi_sck_list[i];
                            self->mosi = &mcu_spi_mosi_list[j];
                            self->miso = &mcu_spi_miso_list[k];
                            break;
                        }
                    }   
                }
            }
        }
    }

    //handle typedef selection, errors
    if(self->sck!=NULL && self->mosi!=NULL && self->miso!=NULL ) {
        SPIx = mcu_spi_banks[self->sck->spi_index-1];
    } else {
        if (spi_taken) {
            mp_raise_ValueError(translate("Hardware busy, try alternative pins"));
        } else {
            mp_raise_ValueError(translate("Invalid SPI pin selection"));
        }
    }

    //Start GPIO for each pin
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = pin_mask(sck->number);
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = self->sck->altfn_index; 
    HAL_GPIO_Init(pin_port(sck->port), &GPIO_InitStruct);

    GPIO_InitStruct.Pin = pin_mask(mosi->number);
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = self->mosi->altfn_index; 
    HAL_GPIO_Init(pin_port(mosi->port), &GPIO_InitStruct);

    GPIO_InitStruct.Pin = pin_mask(miso->number);
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = self->miso->altfn_index; 
    HAL_GPIO_Init(pin_port(miso->port), &GPIO_InitStruct);

    #ifdef SPI1
    if(SPIx==SPI1) { 
        reserved_spi[0] = true;
        __HAL_RCC_SPI1_CLK_ENABLE();
    } 
    #endif
    #ifdef SPI2
    if(SPIx==SPI2) { 
        reserved_spi[1] = true;
        __HAL_RCC_SPI2_CLK_ENABLE();
    } 
    #endif
    #ifdef SPI3
    if(SPIx==SPI3) { 
        reserved_spi[2] = true;
        __HAL_RCC_SPI3_CLK_ENABLE();
    } 
    #endif
    #ifdef SPI4
    if(SPIx==SPI4) { 
        reserved_spi[3] = true;
        __HAL_RCC_SPI4_CLK_ENABLE();
    } 
    #endif
    #ifdef SPI5
    if(SPIx==SPI5) { 
        reserved_spi[4] = true;
        __HAL_RCC_SPI5_CLK_ENABLE();
    } 
    #endif
    #ifdef SPI6
    if(SPIx==SPI6) { 
        reserved_spi[5] = true;
        __HAL_RCC_SPI6_CLK_ENABLE();
    } 
    #endif
    
    self->handle.Instance = SPIx; 
    self->handle.Init.Mode = SPI_MODE_MASTER;
    self->handle.Init.Direction = SPI_DIRECTION_2LINES;
    self->handle.Init.DataSize = SPI_DATASIZE_8BIT;
    self->handle.Init.CLKPolarity = SPI_POLARITY_LOW;
    self->handle.Init.CLKPhase = SPI_PHASE_1EDGE;
    self->handle.Init.NSS = SPI_NSS_SOFT;
    self->handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
    self->handle.Init.FirstBit = SPI_FIRSTBIT_MSB;
    self->handle.Init.TIMode = SPI_TIMODE_DISABLE;
    self->handle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    self->handle.Init.CRCPolynomial = 10;
    if (HAL_SPI_Init(&self->handle) != HAL_OK)
    {
        mp_raise_ValueError(translate("SPI Init Error"));
    }
    self->baudrate = (HAL_RCC_GetPCLK2Freq()/16);
    self->prescaler = 16;
    self->polarity = 0;
    self->phase = 1;
    self->bits = 8;

    claim_pin(sck);
    claim_pin(mosi);
    claim_pin(miso);
}

void common_hal_busio_spi_never_reset(busio_spi_obj_t *self) {
    for(size_t i = 0 ; i < MP_ARRAY_SIZE(mcu_spi_banks); i++) {
        if (mcu_spi_banks[i] == self->handle.Instance) {
            never_reset_spi[i] = true;
            never_reset_pin_number(self->sck->pin->port, self->sck->pin->number);
            never_reset_pin_number(self->mosi->pin->port, self->mosi->pin->number);
            never_reset_pin_number(self->miso->pin->port, self->miso->pin->number);
            break;
        }
    }
}

bool common_hal_busio_spi_deinited(busio_spi_obj_t *self) {
    return self->sck->pin == mp_const_none;
}

void common_hal_busio_spi_deinit(busio_spi_obj_t *self) {
    #ifdef SPI1
    if(self->handle.Instance==SPI1) {
        reserved_spi[0] = false;
        __HAL_RCC_SPI1_CLK_DISABLE(); 
    }
    #endif
    #ifdef SPI2
    if(self->handle.Instance==SPI2) {
        reserved_spi[1] = false;
        __HAL_RCC_SPI2_CLK_DISABLE(); 
    }
    #endif
    #ifdef SPI3
    if(self->handle.Instance==SPI3) {
        reserved_spi[2] = false;
        __HAL_RCC_SPI3_CLK_DISABLE(); 
    }
    #endif
    #ifdef SPI4
    if(self->handle.Instance==SPI4) {
        reserved_spi[3] = false;
        __HAL_RCC_SPI4_CLK_DISABLE(); 
    }
    #endif
    #ifdef SPI5
    if(self->handle.Instance==SPI5) {
        reserved_spi[4] = false;
        __HAL_RCC_SPI5_CLK_DISABLE(); 
    }
    #endif
    #ifdef SPI6
    if(self->handle.Instance==SPI6) {
        reserved_spi[5] = false;
        __HAL_RCC_SPI6_CLK_DISABLE(); 
    }
    #endif
    reset_pin_number(self->sck->pin->port,self->sck->pin->number);
    reset_pin_number(self->mosi->pin->port,self->mosi->pin->number);
    reset_pin_number(self->miso->pin->port,self->miso->pin->number);
    self->sck = mp_const_none;
    self->mosi = mp_const_none;
    self->miso = mp_const_none;
}

static uint32_t stm32_baud_to_spi_div(uint32_t baudrate, uint16_t * prescaler) {
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
        if (baudrate >= (HAL_RCC_GetPCLK2Freq()/divisor)) {
            *prescaler = divisor;
            return baud_map[i][1];
        }
        i++;
    } while (divisor != 256);
    //only gets here if requested baud is lower than minimum
    *prescaler = 256;
    return SPI_BAUDRATEPRESCALER_256;
}

bool common_hal_busio_spi_configure(busio_spi_obj_t *self,
        uint32_t baudrate, uint8_t polarity, uint8_t phase, uint8_t bits) {
    //This resets the SPI, so check before updating it redundantly
    if (baudrate == self->baudrate && polarity== self->polarity 
        && phase == self->phase && bits == self->bits) return true;

    //Deinit SPI
    HAL_SPI_DeInit(&self->handle);

    if (bits == 8) {
        self->handle.Init.DataSize = SPI_DATASIZE_8BIT;
    } else if (bits == 16) {
        self->handle.Init.DataSize = SPI_DATASIZE_16BIT;
    } else {
        return false;
    }

    if (polarity) {
        self->handle.Init.CLKPolarity = SPI_POLARITY_HIGH;
    } else {
        self->handle.Init.CLKPolarity = SPI_POLARITY_LOW;
    }

    if (phase) {
        self->handle.Init.CLKPhase = SPI_PHASE_2EDGE;
    } else {
        self->handle.Init.CLKPhase = SPI_PHASE_1EDGE;
    }

    self->handle.Init.BaudRatePrescaler = stm32_baud_to_spi_div(baudrate, &self->prescaler);

    self->handle.Init.Mode = SPI_MODE_MASTER;
    self->handle.Init.Direction = SPI_DIRECTION_2LINES;
    self->handle.Init.NSS = SPI_NSS_SOFT;
    self->handle.Init.FirstBit = SPI_FIRSTBIT_MSB;
    self->handle.Init.TIMode = SPI_TIMODE_DISABLE;
    self->handle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    self->handle.Init.CRCPolynomial = 10;

    if (HAL_SPI_Init(&self->handle) != HAL_OK)
    {
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

    //Critical section code that may be required at some point. 
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
    HAL_StatusTypeDef result = HAL_SPI_Transmit (&self->handle, (uint8_t *)data, (uint16_t)len, HAL_MAX_DELAY);
    return result == HAL_OK;
}

bool common_hal_busio_spi_read(busio_spi_obj_t *self,
        uint8_t *data, size_t len, uint8_t write_value) {
    HAL_StatusTypeDef result = HAL_SPI_Receive (&self->handle, data, (uint16_t)len, HAL_MAX_DELAY);
    return result == HAL_OK;
}

bool common_hal_busio_spi_transfer(busio_spi_obj_t *self, 
        uint8_t *data_out, uint8_t *data_in, size_t len) {
    HAL_StatusTypeDef result = HAL_SPI_TransmitReceive (&self->handle,
        data_out, data_in, (uint16_t)len,HAL_MAX_DELAY);
    return result == HAL_OK;
}

uint32_t common_hal_busio_spi_get_frequency(busio_spi_obj_t* self) {
    //returns actual frequency
    uint32_t result = HAL_RCC_GetPCLK2Freq()/self->prescaler;
    return result;
}

uint8_t common_hal_busio_spi_get_phase(busio_spi_obj_t* self) {
    return self->phase;
}

uint8_t common_hal_busio_spi_get_polarity(busio_spi_obj_t* self) {
    return self->polarity;
}