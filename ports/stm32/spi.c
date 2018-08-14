/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2018 Damien P. George
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

#include <stdio.h>
#include <string.h>

#include "py/runtime.h"
#include "py/mphal.h"
#include "spi.h"

// Possible DMA configurations for SPI busses:
// SPI1_TX: DMA2_Stream3.CHANNEL_3 or DMA2_Stream5.CHANNEL_3
// SPI1_RX: DMA2_Stream0.CHANNEL_3 or DMA2_Stream2.CHANNEL_3
// SPI2_TX: DMA1_Stream4.CHANNEL_0
// SPI2_RX: DMA1_Stream3.CHANNEL_0
// SPI3_TX: DMA1_Stream5.CHANNEL_0 or DMA1_Stream7.CHANNEL_0
// SPI3_RX: DMA1_Stream0.CHANNEL_0 or DMA1_Stream2.CHANNEL_0
// SPI4_TX: DMA2_Stream4.CHANNEL_5 or DMA2_Stream1.CHANNEL_4
// SPI4_RX: DMA2_Stream3.CHANNEL_5 or DMA2_Stream0.CHANNEL_4
// SPI5_TX: DMA2_Stream4.CHANNEL_2 or DMA2_Stream6.CHANNEL_7
// SPI5_RX: DMA2_Stream3.CHANNEL_2 or DMA2_Stream5.CHANNEL_7
// SPI6_TX: DMA2_Stream5.CHANNEL_1
// SPI6_RX: DMA2_Stream6.CHANNEL_1

#if defined(MICROPY_HW_SPI1_SCK)
SPI_HandleTypeDef SPIHandle1 = {.Instance = NULL};
#endif
#if defined(MICROPY_HW_SPI2_SCK)
SPI_HandleTypeDef SPIHandle2 = {.Instance = NULL};
#endif
#if defined(MICROPY_HW_SPI3_SCK)
SPI_HandleTypeDef SPIHandle3 = {.Instance = NULL};
#endif
#if defined(MICROPY_HW_SPI4_SCK)
SPI_HandleTypeDef SPIHandle4 = {.Instance = NULL};
#endif
#if defined(MICROPY_HW_SPI5_SCK)
SPI_HandleTypeDef SPIHandle5 = {.Instance = NULL};
#endif
#if defined(MICROPY_HW_SPI6_SCK)
SPI_HandleTypeDef SPIHandle6 = {.Instance = NULL};
#endif

const spi_t spi_obj[6] = {
    #if defined(MICROPY_HW_SPI1_SCK)
    {&SPIHandle1, &dma_SPI_1_TX, &dma_SPI_1_RX},
    #else
    {NULL, NULL, NULL},
    #endif
    #if defined(MICROPY_HW_SPI2_SCK)
    {&SPIHandle2, &dma_SPI_2_TX, &dma_SPI_2_RX},
    #else
    {NULL, NULL, NULL},
    #endif
    #if defined(MICROPY_HW_SPI3_SCK)
    {&SPIHandle3, &dma_SPI_3_TX, &dma_SPI_3_RX},
    #else
    {NULL, NULL, NULL},
    #endif
    #if defined(MICROPY_HW_SPI4_SCK)
    {&SPIHandle4, &dma_SPI_4_TX, &dma_SPI_4_RX},
    #else
    {NULL, NULL, NULL},
    #endif
    #if defined(MICROPY_HW_SPI5_SCK)
    {&SPIHandle5, &dma_SPI_5_TX, &dma_SPI_5_RX},
    #else
    {NULL, NULL, NULL},
    #endif
    #if defined(MICROPY_HW_SPI6_SCK)
    {&SPIHandle6, &dma_SPI_6_TX, &dma_SPI_6_RX},
    #else
    {NULL, NULL, NULL},
    #endif
};

void spi_init0(void) {
    // Initialise the SPI handles.
    // The structs live on the BSS so all other fields will be zero after a reset.
    #if defined(MICROPY_HW_SPI1_SCK)
    SPIHandle1.Instance = SPI1;
    #endif
    #if defined(MICROPY_HW_SPI2_SCK)
    SPIHandle2.Instance = SPI2;
    #endif
    #if defined(MICROPY_HW_SPI3_SCK)
    SPIHandle3.Instance = SPI3;
    #endif
    #if defined(MICROPY_HW_SPI4_SCK)
    SPIHandle4.Instance = SPI4;
    #endif
    #if defined(MICROPY_HW_SPI5_SCK)
    SPIHandle5.Instance = SPI5;
    #endif
    #if defined(MICROPY_HW_SPI6_SCK)
    SPIHandle6.Instance = SPI6;
    #endif
}

int spi_find_index(mp_obj_t id) {
    if (MP_OBJ_IS_STR(id)) {
        // given a string id
        const char *port = mp_obj_str_get_str(id);
        if (0) {
        #ifdef MICROPY_HW_SPI1_NAME
        } else if (strcmp(port, MICROPY_HW_SPI1_NAME) == 0) {
            return 1;
        #endif
        #ifdef MICROPY_HW_SPI2_NAME
        } else if (strcmp(port, MICROPY_HW_SPI2_NAME) == 0) {
            return 2;
        #endif
        #ifdef MICROPY_HW_SPI3_NAME
        } else if (strcmp(port, MICROPY_HW_SPI3_NAME) == 0) {
            return 3;
        #endif
        #ifdef MICROPY_HW_SPI4_NAME
        } else if (strcmp(port, MICROPY_HW_SPI4_NAME) == 0) {
            return 4;
        #endif
        #ifdef MICROPY_HW_SPI5_NAME
        } else if (strcmp(port, MICROPY_HW_SPI5_NAME) == 0) {
            return 5;
        #endif
        #ifdef MICROPY_HW_SPI6_NAME
        } else if (strcmp(port, MICROPY_HW_SPI6_NAME) == 0) {
            return 6;
        #endif
        }
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
            "SPI(%s) doesn't exist", port));
    } else {
        // given an integer id
        int spi_id = mp_obj_get_int(id);
        if (spi_id >= 1 && spi_id <= MP_ARRAY_SIZE(spi_obj)
            && spi_obj[spi_id - 1].spi != NULL) {
            return spi_id;
        }
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
            "SPI(%d) doesn't exist", spi_id));
    }
}

// sets the parameters in the SPI_InitTypeDef struct
// if an argument is -1 then the corresponding parameter is not changed
void spi_set_params(const spi_t *spi_obj, uint32_t prescale, int32_t baudrate,
    int32_t polarity, int32_t phase, int32_t bits, int32_t firstbit) {
    SPI_HandleTypeDef *spi = spi_obj->spi;
    SPI_InitTypeDef *init = &spi->Init;

    if (prescale != 0xffffffff || baudrate != -1) {
        if (prescale == 0xffffffff) {
            // prescaler not given, so select one that yields at most the requested baudrate
            mp_uint_t spi_clock;
            #if defined(STM32F0)
            spi_clock = HAL_RCC_GetPCLK1Freq();
            #else
            if (spi->Instance == SPI2 || spi->Instance == SPI3) {
                // SPI2 and SPI3 are on APB1
                spi_clock = HAL_RCC_GetPCLK1Freq();
            } else {
                // SPI1, SPI4, SPI5 and SPI6 are on APB2
                spi_clock = HAL_RCC_GetPCLK2Freq();
            }
            #endif
            prescale = (spi_clock + baudrate - 1) / baudrate;
        }
        if (prescale <= 2) { init->BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2; }
        else if (prescale <= 4) { init->BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4; }
        else if (prescale <= 8) { init->BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8; }
        else if (prescale <= 16) { init->BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16; }
        else if (prescale <= 32) { init->BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32; }
        else if (prescale <= 64) { init->BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64; }
        else if (prescale <= 128) { init->BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128; }
        else { init->BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256; }
    }

    if (polarity != -1) {
        init->CLKPolarity = polarity == 0 ? SPI_POLARITY_LOW : SPI_POLARITY_HIGH;
    }

    if (phase != -1) {
        init->CLKPhase = phase == 0 ? SPI_PHASE_1EDGE : SPI_PHASE_2EDGE;
    }

    if (bits != -1) {
        init->DataSize = (bits == 16) ? SPI_DATASIZE_16BIT : SPI_DATASIZE_8BIT;
    }

    if (firstbit != -1) {
        init->FirstBit = firstbit;
    }
}

// TODO allow to take a list of pins to use
void spi_init(const spi_t *self, bool enable_nss_pin) {
    SPI_HandleTypeDef *spi = self->spi;
    const pin_obj_t *pins[4] = { NULL, NULL, NULL, NULL };

    if (0) {
    #if defined(MICROPY_HW_SPI1_SCK)
    } else if (spi->Instance == SPI1) {
        #if defined(MICROPY_HW_SPI1_NSS)
        pins[0] = MICROPY_HW_SPI1_NSS;
        #endif
        pins[1] = MICROPY_HW_SPI1_SCK;
        #if defined(MICROPY_HW_SPI1_MISO)
        pins[2] = MICROPY_HW_SPI1_MISO;
        #endif
        pins[3] = MICROPY_HW_SPI1_MOSI;
        // enable the SPI clock
        __HAL_RCC_SPI1_CLK_ENABLE();
    #endif
    #if defined(MICROPY_HW_SPI2_SCK)
    } else if (spi->Instance == SPI2) {
        #if defined(MICROPY_HW_SPI2_NSS)
        pins[0] = MICROPY_HW_SPI2_NSS;
        #endif
        pins[1] = MICROPY_HW_SPI2_SCK;
        #if defined(MICROPY_HW_SPI2_MISO)
        pins[2] = MICROPY_HW_SPI2_MISO;
        #endif
        pins[3] = MICROPY_HW_SPI2_MOSI;
        // enable the SPI clock
        __HAL_RCC_SPI2_CLK_ENABLE();
    #endif
    #if defined(MICROPY_HW_SPI3_SCK)
    } else if (spi->Instance == SPI3) {
        #if defined(MICROPY_HW_SPI3_NSS)
        pins[0] = MICROPY_HW_SPI3_NSS;
        #endif
        pins[1] = MICROPY_HW_SPI3_SCK;
        #if defined(MICROPY_HW_SPI3_MISO)
        pins[2] = MICROPY_HW_SPI3_MISO;
        #endif
        pins[3] = MICROPY_HW_SPI3_MOSI;
        // enable the SPI clock
        __HAL_RCC_SPI3_CLK_ENABLE();
    #endif
    #if defined(MICROPY_HW_SPI4_SCK)
    } else if (spi->Instance == SPI4) {
        #if defined(MICROPY_HW_SPI4_NSS)
        pins[0] = MICROPY_HW_SPI4_NSS;
        #endif
        pins[1] = MICROPY_HW_SPI4_SCK;
        #if defined(MICROPY_HW_SPI4_MISO)
        pins[2] = MICROPY_HW_SPI4_MISO;
        #endif
        pins[3] = MICROPY_HW_SPI4_MOSI;
        // enable the SPI clock
        __HAL_RCC_SPI4_CLK_ENABLE();
    #endif
    #if defined(MICROPY_HW_SPI5_SCK)
    } else if (spi->Instance == SPI5) {
        #if defined(MICROPY_HW_SPI5_NSS)
        pins[0] = MICROPY_HW_SPI5_NSS;
        #endif
        pins[1] = MICROPY_HW_SPI5_SCK;
        #if defined(MICROPY_HW_SPI5_MISO)
        pins[2] = MICROPY_HW_SPI5_MISO;
        #endif
        pins[3] = MICROPY_HW_SPI5_MOSI;
        // enable the SPI clock
        __HAL_RCC_SPI5_CLK_ENABLE();
    #endif
    #if defined(MICROPY_HW_SPI6_SCK)
    } else if (spi->Instance == SPI6) {
        #if defined(MICROPY_HW_SPI6_NSS)
        pins[0] = MICROPY_HW_SPI6_NSS;
        #endif
        pins[1] = MICROPY_HW_SPI6_SCK;
        #if defined(MICROPY_HW_SPI6_MISO)
        pins[2] = MICROPY_HW_SPI6_MISO;
        #endif
        pins[3] = MICROPY_HW_SPI6_MOSI;
        // enable the SPI clock
        __HAL_RCC_SPI6_CLK_ENABLE();
    #endif
    } else {
        // SPI does not exist for this board (shouldn't get here, should be checked by caller)
        return;
    }

    // init the GPIO lines
    uint32_t mode = MP_HAL_PIN_MODE_ALT;
    uint32_t pull = spi->Init.CLKPolarity == SPI_POLARITY_LOW ? MP_HAL_PIN_PULL_DOWN : MP_HAL_PIN_PULL_UP;
    for (uint i = (enable_nss_pin ? 0 : 1); i < 4; i++) {
        if (pins[i] == NULL) {
            continue;
        }
        mp_hal_pin_config_alt(pins[i], mode, pull, AF_FN_SPI, (self - &spi_obj[0]) + 1);
    }

    // init the SPI device
    if (HAL_SPI_Init(spi) != HAL_OK) {
        // init error
        // TODO should raise an exception, but this function is not necessarily going to be
        // called via Python, so may not be properly wrapped in an NLR handler
        printf("OSError: HAL_SPI_Init failed\n");
        return;
    }

    // After calling HAL_SPI_Init() it seems that the DMA gets disconnected if
    // it was previously configured.  So we invalidate the DMA channel to force
    // an initialisation the next time we use it.
    dma_invalidate_channel(self->tx_dma_descr);
    dma_invalidate_channel(self->rx_dma_descr);
}

void spi_deinit(const spi_t *spi_obj) {
    SPI_HandleTypeDef *spi = spi_obj->spi;
    HAL_SPI_DeInit(spi);
    if (0) {
    #if defined(MICROPY_HW_SPI1_SCK)
    } else if (spi->Instance == SPI1) {
        __HAL_RCC_SPI1_FORCE_RESET();
        __HAL_RCC_SPI1_RELEASE_RESET();
        __HAL_RCC_SPI1_CLK_DISABLE();
    #endif
    #if defined(MICROPY_HW_SPI2_SCK)
    } else if (spi->Instance == SPI2) {
        __HAL_RCC_SPI2_FORCE_RESET();
        __HAL_RCC_SPI2_RELEASE_RESET();
        __HAL_RCC_SPI2_CLK_DISABLE();
    #endif
    #if defined(MICROPY_HW_SPI3_SCK)
    } else if (spi->Instance == SPI3) {
        __HAL_RCC_SPI3_FORCE_RESET();
        __HAL_RCC_SPI3_RELEASE_RESET();
        __HAL_RCC_SPI3_CLK_DISABLE();
    #endif
    #if defined(MICROPY_HW_SPI4_SCK)
    } else if (spi->Instance == SPI4) {
        __HAL_RCC_SPI4_FORCE_RESET();
        __HAL_RCC_SPI4_RELEASE_RESET();
        __HAL_RCC_SPI4_CLK_DISABLE();
    #endif
    #if defined(MICROPY_HW_SPI5_SCK)
    } else if (spi->Instance == SPI5) {
        __HAL_RCC_SPI5_FORCE_RESET();
        __HAL_RCC_SPI5_RELEASE_RESET();
        __HAL_RCC_SPI5_CLK_DISABLE();
    #endif
    #if defined(MICROPY_HW_SPI6_SCK)
    } else if (spi->Instance == SPI6) {
        __HAL_RCC_SPI6_FORCE_RESET();
        __HAL_RCC_SPI6_RELEASE_RESET();
        __HAL_RCC_SPI6_CLK_DISABLE();
    #endif
    }
}

STATIC HAL_StatusTypeDef spi_wait_dma_finished(const spi_t *spi, uint32_t t_start, uint32_t timeout) {
    volatile HAL_SPI_StateTypeDef *state = &spi->spi->State;
    for (;;) {
        // Do an atomic check of the state; WFI will exit even if IRQs are disabled
        uint32_t irq_state = disable_irq();
        if (*state == HAL_SPI_STATE_READY) {
            enable_irq(irq_state);
            return HAL_OK;
        }
        __WFI();
        enable_irq(irq_state);
        if (HAL_GetTick() - t_start >= timeout) {
            return HAL_TIMEOUT;
        }
    }
    return HAL_OK;
}

void spi_transfer(const spi_t *self, size_t len, const uint8_t *src, uint8_t *dest, uint32_t timeout) {
    // Note: there seems to be a problem sending 1 byte using DMA the first
    // time directly after the SPI/DMA is initialised.  The cause of this is
    // unknown but we sidestep the issue by using polling for 1 byte transfer.

    // Note: DMA transfers are limited to 65535 bytes at a time.

    HAL_StatusTypeDef status;

    if (dest == NULL) {
        // send only
        if (len == 1 || query_irq() == IRQ_STATE_DISABLED) {
            status = HAL_SPI_Transmit(self->spi, (uint8_t*)src, len, timeout);
        } else {
            DMA_HandleTypeDef tx_dma;
            dma_init(&tx_dma, self->tx_dma_descr, self->spi);
            self->spi->hdmatx = &tx_dma;
            self->spi->hdmarx = NULL;
            MP_HAL_CLEAN_DCACHE(src, len);
            uint32_t t_start = HAL_GetTick();
            do {
                uint32_t l = MIN(len, 65535);
                status = HAL_SPI_Transmit_DMA(self->spi, (uint8_t*)src, l);
                if (status != HAL_OK) {
                    break;
                }
                status = spi_wait_dma_finished(self, t_start, timeout);
                if (status != HAL_OK) {
                    break;
                }
                len -= l;
                src += l;
            } while (len);
            dma_deinit(self->tx_dma_descr);
        }
    } else if (src == NULL) {
        // receive only
        if (len == 1 || query_irq() == IRQ_STATE_DISABLED) {
            status = HAL_SPI_Receive(self->spi, dest, len, timeout);
        } else {
            DMA_HandleTypeDef tx_dma, rx_dma;
            if (self->spi->Init.Mode == SPI_MODE_MASTER) {
                // in master mode the HAL actually does a TransmitReceive call
                dma_init(&tx_dma, self->tx_dma_descr, self->spi);
                self->spi->hdmatx = &tx_dma;
            } else {
                self->spi->hdmatx = NULL;
            }
            dma_init(&rx_dma, self->rx_dma_descr, self->spi);
            self->spi->hdmarx = &rx_dma;
            MP_HAL_CLEANINVALIDATE_DCACHE(dest, len);
            uint32_t t_start = HAL_GetTick();
            do {
                uint32_t l = MIN(len, 65535);
                status = HAL_SPI_Receive_DMA(self->spi, dest, l);
                if (status != HAL_OK) {
                    break;
                }
                status = spi_wait_dma_finished(self, t_start, timeout);
                if (status != HAL_OK) {
                    break;
                }
                len -= l;
                dest += l;
            } while (len);
            if (self->spi->hdmatx != NULL) {
                dma_deinit(self->tx_dma_descr);
            }
            dma_deinit(self->rx_dma_descr);
        }
    } else {
        // send and receive
        if (len == 1 || query_irq() == IRQ_STATE_DISABLED) {
            status = HAL_SPI_TransmitReceive(self->spi, (uint8_t*)src, dest, len, timeout);
        } else {
            DMA_HandleTypeDef tx_dma, rx_dma;
            dma_init(&tx_dma, self->tx_dma_descr, self->spi);
            self->spi->hdmatx = &tx_dma;
            dma_init(&rx_dma, self->rx_dma_descr, self->spi);
            self->spi->hdmarx = &rx_dma;
            MP_HAL_CLEAN_DCACHE(src, len);
            MP_HAL_CLEANINVALIDATE_DCACHE(dest, len);
            uint32_t t_start = HAL_GetTick();
            do {
                uint32_t l = MIN(len, 65535);
                status = HAL_SPI_TransmitReceive_DMA(self->spi, (uint8_t*)src, dest, l);
                if (status != HAL_OK) {
                    break;
                }
                status = spi_wait_dma_finished(self, t_start, timeout);
                if (status != HAL_OK) {
                    break;
                }
                len -= l;
                src += l;
                dest += l;
            } while (len);
            dma_deinit(self->tx_dma_descr);
            dma_deinit(self->rx_dma_descr);
        }
    }

    if (status != HAL_OK) {
        mp_hal_raise(status);
    }
}

void spi_print(const mp_print_t *print, const spi_t *spi_obj, bool legacy) {
    SPI_HandleTypeDef *spi = spi_obj->spi;

    uint spi_num = 1; // default to SPI1
    if (spi->Instance == SPI2) { spi_num = 2; }
    #if defined(SPI3)
    else if (spi->Instance == SPI3) { spi_num = 3; }
    #endif
    #if defined(SPI4)
    else if (spi->Instance == SPI4) { spi_num = 4; }
    #endif
    #if defined(SPI5)
    else if (spi->Instance == SPI5) { spi_num = 5; }
    #endif
    #if defined(SPI6)
    else if (spi->Instance == SPI6) { spi_num = 6; }
    #endif

    mp_printf(print, "SPI(%u", spi_num);
    if (spi->State != HAL_SPI_STATE_RESET) {
        if (spi->Init.Mode == SPI_MODE_MASTER) {
            // compute baudrate
            uint spi_clock;
            #if defined(STM32F0)
            spi_clock = HAL_RCC_GetPCLK1Freq();
            #else
            if (spi->Instance == SPI2 || spi->Instance == SPI3) {
                // SPI2 and SPI3 are on APB1
                spi_clock = HAL_RCC_GetPCLK1Freq();
            } else {
                // SPI1, SPI4, SPI5 and SPI6 are on APB2
                spi_clock = HAL_RCC_GetPCLK2Freq();
            }
            #endif
            uint log_prescaler = (spi->Init.BaudRatePrescaler >> 3) + 1;
            uint baudrate = spi_clock >> log_prescaler;
            if (legacy) {
                mp_printf(print, ", SPI.MASTER");
            }
            mp_printf(print, ", baudrate=%u", baudrate);
            if (legacy) {
                mp_printf(print, ", prescaler=%u", 1 << log_prescaler);
            }
        } else {
            mp_printf(print, ", SPI.SLAVE");
        }
        mp_printf(print, ", polarity=%u, phase=%u, bits=%u", spi->Init.CLKPolarity == SPI_POLARITY_LOW ? 0 : 1, spi->Init.CLKPhase == SPI_PHASE_1EDGE ? 0 : 1, spi->Init.DataSize == SPI_DATASIZE_8BIT ? 8 : 16);
        if (spi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE) {
            mp_printf(print, ", crc=0x%x", spi->Init.CRCPolynomial);
        }
    }
    mp_print_str(print, ")");
}

const spi_t *spi_from_mp_obj(mp_obj_t o) {
    if (MP_OBJ_IS_TYPE(o, &pyb_spi_type)) {
        pyb_spi_obj_t *self = MP_OBJ_TO_PTR(o);
        return self->spi;
    } else if (MP_OBJ_IS_TYPE(o, &machine_hard_spi_type)) {
        machine_hard_spi_obj_t *self = MP_OBJ_TO_PTR(o);
        return self->spi;
    } else {
        mp_raise_TypeError("expecting an SPI object");
    }
}
