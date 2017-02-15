/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef MBED_OBJECTS_H
#define MBED_OBJECTS_H

#include "cmsis.h"
#include "PortNames.h"
#include "PeripheralNames.h"
#include "PinNames.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIG_GPIO_EN
struct gpio_irq_s {
    PinName pin;
    uint32_t event;
    HAL_GPIO_PIN hal_pin;
};

typedef struct gpio_irq_s gpio_irq_t;

struct gpio_s {
    PinName pin;
    PinMode mode;
    PinDirection direction;
    HAL_GPIO_PIN hal_pin;
};

typedef struct gpio_s gpio_t;

struct port_s {
    PortName port;
    uint32_t mask;
    PinDirection direction;
    uint8_t *pin_def;
};
#endif  // end of "#ifdef CONFIG_GPIO_EN"

#ifdef CONFIG_UART_EN
struct serial_s {
    HAL_RUART_OP hal_uart_op;
    HAL_RUART_ADAPTER hal_uart_adp;
#ifdef CONFIG_GDMA_EN    
    UART_DMA_CONFIG   uart_gdma_cfg;
    HAL_GDMA_ADAPTER uart_gdma_adp_tx;
    HAL_GDMA_ADAPTER uart_gdma_adp_rx;
#endif    
    uint32_t    tx_len;
    uint32_t    rx_len;
};
#endif  // end of "#ifdef CONFIG_UART_EN"

struct log_uart_s {
    HAL_LOG_UART_ADAPTER log_hal_uart;
};

#ifdef CONFIG_SPI_COM_EN

#endif

#ifdef CONFIG_PWM_EN
struct pwmout_s {
    uint8_t pwm_idx;
    uint8_t pin_sel;
    uint32_t period;
    uint32_t pulse;
    
};
#endif

#ifdef CONFIG_I2C_EN
struct i2c_s {
    SAL_I2C_MNGT_ADPT   SalI2CMngtAdpt;        
    SAL_I2C_HND_PRIV    SalI2CHndPriv;    
    HAL_I2C_INIT_DAT    HalI2CInitData;
    HAL_I2C_OP          HalI2COp;   
    IRQ_HANDLE          I2CIrqHandleDat;    
    HAL_GDMA_ADAPTER    HalI2CTxGdmaAdpt;   
    HAL_GDMA_ADAPTER    HalI2CRxGdmaAdpt;     
    HAL_GDMA_OP         HalI2CGdmaOp;
    IRQ_HANDLE          I2CTxGdmaIrqHandleDat;    
    IRQ_HANDLE          I2CRxGdmaIrqHandleDat;        
    SAL_I2C_USER_CB     SalI2CUserCB;   
    SAL_I2C_USERCB_ADPT SalI2CUserCBAdpt[SAL_USER_CB_NUM];    
    SAL_I2C_DMA_USER_DEF    SalI2CDmaUserDef;
};
#endif


struct flash_s
{
    SPIC_INIT_PARA SpicInitPara;
    u32 Length;
};



#ifdef CONFIG_ADC_EN
struct analogin_s {
    SAL_ADC_MNGT_ADPT   SalADCMngtAdpt;        
    SAL_ADC_HND_PRIV    SalADCHndPriv;    
    HAL_ADC_INIT_DAT    HalADCInitData;
    HAL_ADC_OP          HalADCOp;   
    IRQ_HANDLE          ADCIrqHandleDat;    
    HAL_GDMA_ADAPTER    HalADCGdmaAdpt;     
    HAL_GDMA_OP         HalADCGdmaOp;
    IRQ_HANDLE          ADCGdmaIrqHandleDat;        
    SAL_ADC_USER_CB     SalADCUserCB;   
    SAL_ADC_USERCB_ADPT SalADCUserCBAdpt[SAL_ADC_USER_CB_NUM];    
};
#endif

#if 0
struct i2c_s {
    I2C_Type *i2c;
};

struct spi_s {
    SPI_Type *spi;
};

#endif

#ifdef CONFIG_NFC_EN
struct nfctag_s {
    NFC_ADAPTER NFCAdapter;
    void *nfc_rd_cb;    // read callback function
    void *rd_cb_arg;
    void *nfc_wr_cb;    // write callback function
    void *wr_cb_arg;
    void *nfc_ev_cb;    // event callback function
    void *ev_cb_arg;
    void *nfc_cache_rd_cb;    // cache read callback function
    void *cache_read_cb_arg;
    unsigned int event_mask;
    int pwr_status;
};
#endif

#ifdef CONFIG_TIMER_EN
struct gtimer_s {
    TIMER_ADAPTER hal_gtimer_adp;
    void *handler;
    u32 hid;
    u8 timer_id;
    u8 is_periodcal;    
};
#endif

#ifdef CONFIG_I2S_EN
struct i2s_s {
    HAL_I2S_ADAPTER I2SAdapter;
	HAL_I2S_INIT_DAT InitDat;
	u8 sampling_rate;
	u8 channel_num;
	u8 word_length;
    u8 direction;
};

#endif

#ifdef CONFIG_DAC_EN
/** \file objects.h
  *  \brief A Documented file.
  *
  *  A documented file.
*/

/** \struct dac_s objects.h "rtl8195a/objects.h"
 *   \brief This is a dac_s structure.
 *
 *  For analogout APIs, a pointer to dac_s is used as an input paras.
 *  A DAC initial data structure is the major element of dac_s.
 */
struct dac_s {
   HAL_DAC_INIT_DAT DACpara;
};
#endif


#ifdef __cplusplus
}
#endif

#endif
