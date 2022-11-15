/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Renesas Electronics Corporation
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
#include <stdint.h>
#include <stdbool.h>
#include "hal_data.h"
#include "ra_config.h"
#include "ra_gpio.h"
#include "ra_utils.h"
#include "ra_adc.h"

static R_ADC0_Type *adc_reg = (R_ADC0_Type *)0x4005c000;
#if defined(RA4M1) | defined(RA4W1)
static R_TSN_Type *tsn_reg = (R_TSN_Type *)0x407ec000;
#endif
#if defined(RA6M1) | defined(RA6M2)
static R_TSN_CTRL_Type *tsn_ctrl_reg = (R_TSN_CTRL_Type *)0x4005D000;
static R_TSN_CAL_Type *tsn_cal_reg = (R_TSN_CAL_Type *)0x407FB17C;
#endif
static uint8_t resolution = RA_ADC_DEF_RESOLUTION;

typedef struct adc_pin_to_ch {
    uint32_t pin;
    uint8_t ch;
} adc_pin_to_ch_t;

static const adc_pin_to_ch_t pin_to_ch[] = {
    #if defined(RA4M1)

    { P000, AN000 },
    { P001, AN001 },
    { P002, AN002 },
    { P003, AN003 },
    { P004, AN004 },
    { P005, AN011 },
    { P006, AN012 },
    { P007, AN013 },
    { P008, AN014 },
    { P010, AN005 },
    { P011, AN006 },
    { P012, AN007 },
    { P013, AN008 },
    { P014, AN009 },
    { P015, AN010 },
    { P100, AN022 },
    { P101, AN021 },
    { P102, AN020 },
    { P103, AN019 },
    { P500, AN016 },
    { P501, AN017 },
    { P502, AN018 },
    { P503, AN023 },
    { P504, AN024 },
    { P505, AN025 },

    #elif defined(RA4W1)

    { P004, AN004 },
    { P010, AN005 },
    { P011, AN006 },
    { P014, AN009 },
    { P015, AN010 },
    { P102, AN020 },
    { P103, AN019 },
    { P501, AN017 },

    #elif defined(RA6M2)
    { P000, AN000 },
    { P001, AN001 },
    { P002, AN002 },
    { P003, AN007 },
    { P004, AN100 },
    { P005, AN101 },
    { P006, AN102 },
    { P007, AN107 },
    { P008, AN003 },
    { P009, AN004 },
    { P014, AN005 },
    { P014, AN105 },
    { P015, AN006 },
    { P015, AN106 },
    { P500, AN016 },
    { P501, AN116 },
    { P502, AN017 },
    { P503, AN117 },
    { P504, AN018 },
    { P505, AN118 },
    { P506, AN019 },
    { P509, AN020 },

    #elif defined(RA6M1)
    { P000, AN000 },
    { P001, AN001 },
    { P002, AN002 },
    { P003, AN007 },
    { P004, AN100 },
    { P005, AN101 },
    { P006, AN102 },
    { P007, AN107 },
    { P008, AN003 },
    { P014, AN005 },
    { P014, AN105 },
    { P015, AN006 },
    { P015, AN106 },
    { P500, AN016 },
    { P501, AN116 },
    { P502, AN017 },
    { P503, AN117 },
    { P504, AN018 },
    { P508, AN020 },

    #else
    #error "CMSIS MCU Series is not specified."
    #endif
};
#define ADC_PIN_TO_CH_SIZE (sizeof(pin_to_ch) / sizeof(adc_pin_to_ch_t))

#define ADC_CH_MAX         (32)
#define ADC_UNIT0_SCAN_MAX (28)
static uint16_t adc_values[ADC_CH_MAX];

bool ra_adc_pin_to_ch(uint32_t pin, uint8_t *ch) {
    uint32_t i;
    *ch = (uint8_t)ADC_NON;
    for (i = 0; i < ADC_PIN_TO_CH_SIZE; i++) {
        if (pin == pin_to_ch[i].pin) {
            *ch = pin_to_ch[i].ch;
            break;
        }
    }
    if (*ch == (uint8_t)ADC_NON) {
        return false;
    } else {
        return true;
    }
}

bool ra_adc_ch_to_pin(uint8_t ch, uint32_t *pin) {
    uint32_t i;
    *pin = (uint32_t)PIN_END;
    for (i = 0; i < ADC_PIN_TO_CH_SIZE; i++) {
        if (ch == pin_to_ch[i].ch) {
            *pin = pin_to_ch[i].pin;
            break;
        }
    }
    if (*pin == (uint32_t)PIN_END) {
        return false;
    } else {
        return true;
    }
}

uint8_t ra_adc_get_channel(uint32_t pin) {
    bool flag;
    uint8_t ch;
    flag = ra_adc_pin_to_ch(pin, &ch);
    if (!flag) {
        ch = 0;
    }
    return ch;
}

static void ra_adc0_module_start(void) {
    ra_mstpcrd_start(R_MSTP_MSTPCRD_MSTPD16_Msk);
}

static void ra_adc0_module_stop(void) {
    ra_mstpcrd_stop(R_MSTP_MSTPCRD_MSTPD16_Msk);
}

#if defined(RA6M1) | defined(RA6M2)
static void ra_adc1_module_start(void) {
    ra_mstpcrd_start(R_MSTP_MSTPCRD_MSTPD15_Msk);
}

static void ra_adc1_module_stop(void) {
    ra_mstpcrd_stop(R_MSTP_MSTPCRD_MSTPD15_Msk);
}
#endif

// For RA4M1 and RA4W1, there is no TSN configuration
#if defined(RA6M1) | defined(RA6M2)
static void ra_tsn_module_start(void) {
    ra_mstpcrd_start(R_MSTP_MSTPCRD_MSTPD22_Msk);
}

static void ra_tsn_module_stop(void) {
    ra_mstpcrd_stop(R_MSTP_MSTPCRD_MSTPD22_Msk);
}
#endif

void ra_adc_set_pin(uint32_t pin, bool adc_enable) {
    uint32_t port = GPIO_PORT(pin);
    uint32_t bit = GPIO_BIT(pin);
    uint32_t pfs = _PXXPFS(port, bit);
    pwpr_unprotect();
    if (adc_enable) {
        pfs &= ~PMR_MASK; /* GPIO */
        pfs &= ~PDR_MASK; /* input */
        pfs |= ASEL_MASK; /* set adc bit */
    } else {
        pfs |= PMR_MASK;   /* GPIO */
        pfs &= ~PDR_MASK;  /* input */
        pfs &= ~ASEL_MASK; /* clear adc bit */
    }
    _PXXPFS(port, bit) = pfs;
    pwpr_protect();
}

void ra_adc_enable(uint32_t pin) {
    ra_adc_set_pin(pin, true);
}

void ra_adc_disable(uint32_t pin) {
    ra_adc_set_pin(pin, false);
}

__attribute__((naked)) static void min_delay(__attribute__((unused)) uint32_t loop_cnt) {
    __asm volatile (
        "sw_delay_loop:         \n"
        #if defined(__ICCARM__) || defined(__ARMCC_VERSION)
        "   subs r0, #1         \n"  ///< 1 cycle
        #elif defined(__GNUC__)
        "   sub r0, r0, #1      \n"  ///< 1 cycle
        #endif
        "   cmp r0, #0          \n"  ///< 1 cycle
        /* CM0 and CM23 have a different instruction set */
        #if defined(__CORE_CM0PLUS_H_GENERIC) || defined(__CORE_CM23_H_GENERIC)
        "   bne sw_delay_loop   \n"  ///< 2 cycles
        #else
        "   bne.n sw_delay_loop \n"  ///< 2 cycles
        #endif
        "   bx lr               \n");  ///< 2 cycles
}

static void udelay(uint32_t us) {
    while (us-- > 0) {
        min_delay(PCLK / 1000000 / 4);
    }
}

void ra_adc_set_resolution(uint8_t res) {
    uint16_t adcer;
    uint16_t adprc;
    #if defined(RA4M1) | defined(RA4W1)
    if ((res == 14) | (res == 12)) {
        if (res == 14) {
            adprc = 0x0006;
        } else {
            adprc = 0x0000;
        }
        adcer = adc_reg->ADCER;
        adcer &= (uint16_t) ~0x0006;
        adcer |= (uint16_t)adprc;
        adc_reg->ADCER;
        resolution = res;
    }
    #else
    if ((res == 12) | (res == 10) | (res == 8)) {
        if (res == 12) {
            adprc = 0x0000;
        } else if (res == 10) {
            adprc = 0x0002;
        } else {
            adprc = 0x0004;
        }
        adcer = adc_reg->ADCER;
        adcer &= (uint16_t) ~0x0006;
        adcer |= (uint16_t)adprc;
        adc_reg->ADCER;
        resolution = res;
    }
    #endif
}

uint8_t ra_adc_get_resolution(void) {
    uint8_t res = 0;
    uint16_t adcer;
    adcer = adc_reg->ADCER;
    adcer &= 0x0006;
    #if defined(RA4M1) | defined(RA4W1)
    if (adcer == 0x0006) {
        res = 14;
    } else if (adcer == 0x0000) {
        res = 12;
    }
    #else
    if (adcer == 0x0000) {
        res = 12;
    } else if (adcer == 0x0002) {
        res = 10;
    } else if (adcer == 0x0004) {
        res = 8;
    }
    #endif
    return res;
}

// assumption
// AVCC0 is used. Neither VREFH0 nor internal reference voltage is not used.
uint16_t ra_adc_read_ch(uint8_t ch) {
    uint16_t value16 = 0;
    if ((ch == ADC_TEMP) | (ch == ADC_REF)) {
        #if defined(RA6M1) | defined(RA6M2)
        if (ch == ADC_TEMP) {
            adc_reg->ADEXICR_b.TSSA = 1;
            tsn_ctrl_reg->TSCR_b.TSEN = 1;
            while (!tsn_ctrl_reg->TSCR_b.TSEN) {
                ;
            }
            tsn_ctrl_reg->TSCR_b.TSOE = 1;
            while (!tsn_ctrl_reg->TSCR_b.TSOE) {
                ;
            }
            udelay(300);
        } else if (ch == ADC_REF) {
            adc_reg->ADEXICR_b.OCSA = 1;
            udelay(300);
        }
        #endif
        adc_reg->ADANSA[0] = 0;
        adc_reg->ADANSA[1] = 0;
    } else if (ch < 16) {
        adc_reg->ADANSA[0] |= (uint16_t)(1 << ch);
    } else {
        adc_reg->ADANSA[1] |= (uint16_t)(1 << (ch - 16));
    }
    adc_reg->ADCSR_b.ADCS = 0; /* single scan mode */
    adc_reg->ADCSR_b.ADST = 1; /* start a/d conversion */
    while (adc_reg->ADCSR_b.ADST) {
        ; /* ADC in progress*/
    }
    if (ch == ADC_TEMP) {
        value16 = (uint16_t)adc_reg->ADTSDR;
    } else if (ch == ADC_REF) {
        value16 = (uint16_t)adc_reg->ADOCDR;
    } else {
        value16 = (uint16_t)adc_reg->ADDR[ch];
    }
    #if defined(RA6M1) | defined(RA6M2)
    if (ch == ADC_TEMP) {
        tsn_ctrl_reg->TSCR_b.TSOE = 0;
        while (tsn_ctrl_reg->TSCR_b.TSOE) {
            ;
        }
        tsn_ctrl_reg->TSCR_b.TSEN = 0;
        while (tsn_ctrl_reg->TSCR_b.TSEN) {
            ;
        }
    } else if (ch == ADC_REF) {
        adc_reg->ADEXICR_b.OCSA = 0;
    }
    #endif
    return value16;
}

uint16_t ra_adc_read(uint32_t pin) {
    uint8_t ch = ra_adc_get_channel(pin);
    if (ch == ADC_NON) {
        return 0;
    }
    return ra_adc_read_ch(ch);
}

int16_t ra_adc_read_itemp(void) {
    int16_t temp = 0;
    int16_t vmax = (int16_t)(1 << resolution);
    #if defined(RA4M1) | defined(RA4W1)
    uint16_t cal125 = ((uint16_t)tsn_reg->TSCDRH << 8) + (uint16_t)tsn_reg->TSCDRL;
    uint16_t val = ra_adc_read_ch(ADC_TEMP);
    int16_t v125 = (int16_t)(33 * cal125 / vmax / 10);
    int16_t vtemp = (int16_t)(33 * val / vmax / 10);
    temp = (int16_t)(125 + ((vtemp - v125) * 1000000 / (int16_t)BSP_FEATURE_ADC_TSN_SLOPE));
    #elif defined(RA6M1) | defined(RA6M2)
    uint16_t cal127 = (uint16_t)tsn_cal_reg->TSCDR;
    uint16_t val = ra_adc_read_ch(ADC_TEMP);
    int16_t v127 = (int16_t)(33 * cal127 / vmax / 10);
    int16_t vtemp = (int16_t)(33 * val / vmax / 10);
    temp = (int16_t)(127 + ((vtemp - v127) * 1000000 / (int16_t)BSP_FEATURE_ADC_TSN_SLOPE));
    #else
    #error "CMSIS MCU Series is not specified."
    #endif
    return temp;
}

float ra_adc_read_ftemp(void) {
    float temp;
    float vmax = (float)(1 << resolution);
    #if defined(RA4M1) | defined(RA4W1)
    uint16_t cal125 = ((uint16_t)tsn_reg->TSCDRH << 8) + (uint16_t)tsn_reg->TSCDRL;
    uint16_t val = ra_adc_read_ch(ADC_TEMP);
    float v125 = (float)(3.3f * (float)cal125 / vmax);
    float vtemp = (float)(3.3f * (float)val / vmax);
    temp = (float)(125.0f + ((vtemp - v125) * 1000000.0f / (float)BSP_FEATURE_ADC_TSN_SLOPE));
    #endif
    #if defined(RA6M1) | defined(RA6M2)
    uint16_t cal127 = (uint16_t)tsn_cal_reg->TSCDR;
    uint16_t val = ra_adc_read_ch(ADC_TEMP);
    float v127 = (float)(3.3f * (float)cal127 / vmax);
    float vtemp = (float)(3.3f * (float)val / vmax);
    temp = (float)(127.0f + ((vtemp - v127) * 1000000.0f / (float)BSP_FEATURE_ADC_TSN_SLOPE));
    #endif
    return temp;
}

float ra_adc_read_fref(void) {
    uint16_t val = ra_adc_read_ch(ADC_REF);
    float vmax = (float)(1 << resolution);
    float vref = (float)(3.3f * (float)val / vmax);
    return vref;
}

uint16_t ra_adc_all_read_ch(uint32_t ch) {
    if (ch < ADC_CH_MAX) {
        return adc_values[ch];
    } else {
        return 0;
    }
}

void ra_adc_all(__attribute__((unused)) uint32_t resolution, uint32_t mask) {
    uint32_t i;
    uint32_t pin;
    uint32_t bit;
    uint16_t value16 = 0;
    bool badc = ((mask & 0x1fffffff) != 0);
    bool btemp = ((mask & (1 << ADC_TEMP)) != 0);
    bool bref = ((mask & (1 << ADC_REF)) != 0);
    for (i = 0; i < ADC_CH_MAX; i++) {
        adc_values[i] = 0;
    }
    if (badc) {
        bit = 1;
        adc_reg->ADANSA[0] = 0;
        adc_reg->ADANSA[1] = 0;
        for (i = 0; i < ADC_UNIT0_SCAN_MAX; i++) {
            if (mask & bit) {
                if (ra_adc_ch_to_pin((uint8_t)i, &pin)) {
                    ra_adc_enable(pin);
                    if (i < 16) {
                        adc_reg->ADANSA[0] |= (uint16_t)(1 << i);
                    } else {
                        adc_reg->ADANSA[1] |= (uint16_t)(1 << (i - 16));
                    }
                }
            }
            bit <<= 1;
        }
        adc_reg->ADCSR_b.ADCS = 0; /* single scan mode */
        adc_reg->ADCSR_b.ADST = 1; /* start a/d conversion */
        while (adc_reg->ADCSR_b.ADST) {
            ; /* ADC in progress*/
        }
        bit = 1;
        for (i = 0; i < ADC_UNIT0_SCAN_MAX; i++) {
            if (mask & bit) {
                value16 = (uint16_t)adc_reg->ADDR[i];
                adc_values[i] = value16;
            }
            bit <<= 1;
        }
    }
    if (btemp) {
        adc_reg->ADANSA[0] = 0;
        adc_reg->ADANSA[1] = 0;
        adc_reg->ADEXICR_b.TSSA = 1;
        #if defined(RA6M1) | defined(RA6M2)
        tsn_ctrl_reg->TSCR_b.TSEN = 1;
        while (!tsn_ctrl_reg->TSCR_b.TSEN) {
            ;
        }
        tsn_ctrl_reg->TSCR_b.TSOE = 1;
        while (!tsn_ctrl_reg->TSCR_b.TSOE) {
            ;
        }
        udelay(30);
        #endif
        adc_reg->ADCSR_b.ADCS = 0; /* single scan mode */
        adc_reg->ADCSR_b.ADST = 1; /* start a/d conversion */
        while (adc_reg->ADCSR_b.ADST) {
            ; /* ADC in progress*/
        }
        value16 = (uint16_t)adc_reg->ADTSDR;
        adc_values[ADC_TEMP] = value16;
        #if defined(RA6M1) | defined(RA6M2)
        tsn_ctrl_reg->TSCR_b.TSOE = 0;
        while (tsn_ctrl_reg->TSCR_b.TSOE) {
            ;
        }
        tsn_ctrl_reg->TSCR_b.TSEN = 0;
        while (tsn_ctrl_reg->TSCR_b.TSEN) {
            ;
        }
        #endif
    }
    if (bref) {
        adc_reg->ADANSA[0] = 0;
        adc_reg->ADANSA[1] = 0;
        adc_reg->ADEXICR_b.OCSA = 1;
        udelay(30);
        adc_reg->ADCSR_b.ADCS = 0; /* single scan mode */
        adc_reg->ADCSR_b.ADST = 1; /* start a/d conversion */
        while (adc_reg->ADCSR_b.ADST) {
            ; /* ADC in progress*/
        }
        value16 = (uint16_t)adc_reg->ADOCDR;
        adc_values[ADC_REF] = value16;
    }
}

bool ra_adc_init(void) {
    ra_adc0_module_start();
    #if defined(RA6M1) | defined(RA6M2)
    ra_adc1_module_start();
    ra_tsn_module_start();
    #endif
    resolution = RA_ADC_DEF_RESOLUTION;
    ra_adc_set_resolution(resolution);
    return true;
}

bool ra_adc_deinit(void) {
    ra_adc0_module_stop();
    #if defined(RA6M1) | defined(RA6M2)
    ra_adc1_module_stop();
    ra_tsn_module_stop();
    #endif
    return true;
}

__WEAK void adc_scan_end_isr(void) {
    // dummy
}
