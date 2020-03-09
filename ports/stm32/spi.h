/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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
#ifndef MICROPY_INCLUDED_STM32_SPI_H
#define MICROPY_INCLUDED_STM32_SPI_H

#include "drivers/bus/spi.h"
#include "dma.h"

#ifdef SPIDMA_MODES
// XXXTODO how to get these to match the values in extmod/machine_spi.h
#define SPI_CFG_MODE                0x00000000  // SPI DMA MODE
#define SPI_CFG_MODE_NORMAL         0x00000001  // SPI DMA MODE NORMAL
#define SPI_CFG_MODE_NONBLOCKING    0x00000002  // SPI DMA MODE NON-BLOCKING
#define SPI_CFG_MODE_CIRCULAR       0x00000003  // SPI DMA MODE CIRCULAR

/**
  * @brief  SPI DMA handle Structure definition
  */
//  typedef void  (* pf_hdma_void)(struct __DMA_HandleTypeDef *);
typedef struct __SPI_DMAHandleTypeDef
{
    uint32_t mode; /*!< Specifies the operation mode of the SPI DMAy Channelx. This parameter can be a value of CIRCULAR.*/

    mp_obj_t callback;     /*!< DMA transfer complete callback       */

    mp_obj_t callbackhalf;  /*!< DMA Half transfer complete callback  */

    mp_obj_t callbackerror; /*!< DMA transfer error callback          */

    mp_obj_t callbackabort; /*!< DMA transfer aborted by user callback*/

    // Since DMA is happening in the background, these two structures need a place they can be stored (not on the stack frame)
    DMA_HandleTypeDef tx_dma; /*!< Since DMA is happening in the background, this needs to be static and not on the stack frame */
    DMA_HandleTypeDef rx_dma;  /*!< Since DMA is happening in the background, this needs to be static and not on the stack frame */
} SPI_DMAHandleTypeDef;
#endif

typedef SPI_DMAHandleTypeDef  machine_hard_spi_dmamode_t;
 
typedef struct _spi_t {
    SPI_HandleTypeDef *spi;
    const dma_descr_t *tx_dma_descr;
    const dma_descr_t *rx_dma_descr;
#ifdef SPIDMA_MODES
    SPI_DMAHandleTypeDef        *dma_modes; // Null if normal blocking behavior
#endif
} spi_t;

typedef struct _spi_proto_cfg_t {
    const spi_t *spi;
    uint32_t baudrate;
    uint8_t polarity;
    uint8_t phase;
    uint8_t bits;
    uint8_t firstbit;
} spi_proto_cfg_t;

typedef struct _pyb_spi_obj_t {
    mp_obj_base_t base;
    const spi_t *spi;
} pyb_spi_obj_t;

typedef struct _machine_hard_spi_obj_t {
    mp_obj_base_t base;
    const spi_t *spi;
} machine_hard_spi_obj_t;

extern SPI_HandleTypeDef SPIHandle1;
extern SPI_HandleTypeDef SPIHandle2;
extern SPI_HandleTypeDef SPIHandle3;
extern SPI_HandleTypeDef SPIHandle4;
extern SPI_HandleTypeDef SPIHandle5;
extern SPI_HandleTypeDef SPIHandle6;

extern const spi_t spi_obj[6];

extern const mp_spi_proto_t spi_proto;
extern const mp_obj_type_t pyb_spi_type;
extern const mp_obj_type_t machine_soft_spi_type;
extern const mp_obj_type_t machine_hard_spi_type;

// A transfer of "len" bytes should take len*8*1000/baudrate milliseconds.
// To simplify the calculation we assume the baudrate is never less than 8kHz
// and use that value for the baudrate in the formula, plus a small constant.
#define SPI_TRANSFER_TIMEOUT(len) ((len) + 100)

void spi_init0(void);
void spi_init(const spi_t *spi, bool enable_nss_pin);
void spi_deinit(const spi_t *spi_obj);
int spi_find_index(mp_obj_t id);
#ifdef SPIDMA_MODES
void spi_set_params(const spi_t *spi_obj, uint32_t prescale, int32_t baudrate,
    int32_t polarity, int32_t phase, int32_t bits, int32_t firstbit,
    uint32_t mode, mp_obj_t callback, mp_obj_t callbackhalf, mp_obj_t callbackerror, mp_obj_t callbackabort);
#else
void spi_set_params(const spi_t *spi_obj, uint32_t prescale, int32_t baudrate,
    int32_t polarity, int32_t phase, int32_t bits, int32_t firstbit);
#endif
void spi_transfer(const spi_t *self, size_t len, const uint8_t *src, uint8_t *dest, uint32_t timeout);
void spi_print(const mp_print_t *print, const spi_t *spi_obj, bool legacy);
const spi_t *spi_from_mp_obj(mp_obj_t o);

#endif // MICROPY_INCLUDED_STM32_SPI_H
