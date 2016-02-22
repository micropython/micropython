/* mbed Microcontroller Library
 *******************************************************************************
 * Copyright (c) 2015, Realtek Semiconductor Corp.
 * All rights reserved.
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *******************************************************************************
 */

#ifndef MBED_EXT_I2S_API_EXT_H
#define MBED_EXT_I2S_API_EXT_H

#include "device.h"
#include "rtl8195a.h"
#include "hal_i2s.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
    SR_8KHZ = I2S_SR_8KHZ, 
    SR_16KHZ = I2S_SR_16KHZ,
    SR_24KHZ = I2S_SR_24KHZ,
	SR_32KHZ = I2S_SR_32KHZ,
    SR_48KHZ = I2S_SR_48KHZ,
	SR_96KHZ = I2S_SR_96KHZ,
	SR_7p35KHZ = I2S_SR_7p35KHZ,
	SR_11p02KHZ = I2S_SR_11p02KHZ,
	SR_22p05KHZ = I2S_SR_22p05KHZ,
	SR_29p4KHZ = I2S_SR_29p4KHZ,
	SR_44p1KHZ = I2S_SR_44p1KHZ,
	SR_88p2KHZ = I2S_SR_88p2KHZ
};

enum {
	CH_STEREO = I2S_CH_STEREO,
	CH_MONO = I2S_CH_MONO
};

enum {
	WL_16b = I2S_WL_16,
	WL_24b = I2S_WL_24
};

enum {
    I2S_DIR_RX   = I2S_ONLY_RX,    // Rx Only
    I2S_DIR_TX   = I2S_ONLY_TX,    // Tx Only
    I2S_DIR_TXRX = I2S_TXRX     // Tx & Rx (BiDirection)
};

typedef void (*i2s_irq_handler)(uint32_t id, char *pbuf);

typedef struct i2s_s i2s_t;
	
void i2s_init(i2s_t *obj, PinName sck, PinName ws, PinName sd);
void i2s_set_dma_buffer(i2s_t *obj, char *tx_buf, char *rx_buf, 
    uint32_t page_num, uint32_t page_size);
void i2s_tx_irq_handler(i2s_t *obj, i2s_irq_handler handler, uint32_t id);
void i2s_rx_irq_handler(i2s_t *obj, i2s_irq_handler handler, uint32_t id);
void i2s_set_direction(i2s_t *obj, int trx_type);
void i2s_set_param(i2s_t *obj, int channel_num, int rate, int word_len);
void i2s_deinit(i2s_t *obj);
int* i2s_get_tx_page(i2s_t *obj);
void i2s_send_page(i2s_t *obj, uint32_t *pbuf);
void i2s_recv_page(i2s_t *obj);
void i2s_enable(i2s_t *obj);
void i2s_disable(i2s_t *obj);

#ifdef __cplusplus
}
#endif


#endif
