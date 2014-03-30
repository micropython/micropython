#include <stdio.h>
#include <stm32f4xx.h>

#include "misc.h"
#include "nlr.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "adc.h"

/* ADC defintions */
#define ADCx                (ADC1)
#define ADCx_CLK            (RCC_APB2Periph_ADC1)
#define ADC_NUM_CHANNELS    (16)

/* Internally connected ADC channels Temp/VBAT/VREF*/
#if   defined (STM32F40XX) || defined(STM32F41XX) || defined(STM32F40_41xxx)
#define ADC_TEMP_CHANNEL    (16)
#define ADC_VBAT_CHANNEL    (18)
#define ADC_VREF_CHANNEL    (17)
#elif defined (STM32F42XX) || defined(STM32F43XX)
#define ADC_TEMP_CHANNEL    (18)
#define ADC_VBAT_CHANNEL    (18) /* same channel as TEMP */
#define ADC_VREF_CHANNEL    (17)
#endif

/* Core temperature sensor definitions */
#define CORE_TEMP_V25          (943)  /* (0.76v/3.3v)*(2^ADC resoultion) */
#define CORE_TEMP_AVG_SLOPE    (3)    /* (2.5mv/3.3v)*(2^ADC resoultion) */

/* VBAT divider */
#if   defined (STM32F40XX) || defined(STM32F41XX) || defined(STM32F40_41xxx)
#define VBAT_DIV (2)
#elif defined (STM32F42XX) || defined(STM32F43XX)
#define VBAT_DIV (4)
#endif

/* GPIO struct */
typedef struct {
    GPIO_TypeDef* port;
    uint32_t pin;
} gpio_t;

/* ADC GPIOs */
static const gpio_t adc_gpio[] = {
    {GPIOA, GPIO_Pin_0},    /* ADC123_IN0 */
    {GPIOA, GPIO_Pin_1},    /* ADC123_IN1 */
    {GPIOA, GPIO_Pin_2},    /* ADC123_IN2 */
    {GPIOA, GPIO_Pin_3},    /* ADC123_IN3 */
    {GPIOA, GPIO_Pin_4},    /* ADC12_IN4 */
    {GPIOA, GPIO_Pin_5},    /* ADC12_IN5 */
    {GPIOA, GPIO_Pin_6},    /* ADC12_IN6 */
    {GPIOA, GPIO_Pin_7},    /* ADC12_IN7 */
    {GPIOB, GPIO_Pin_0},    /* ADC12_IN8 */
    {GPIOB, GPIO_Pin_1},    /* ADC12_IN9 */
    {GPIOC, GPIO_Pin_0},    /* ADC123_IN10 */
    {GPIOC, GPIO_Pin_1},    /* ADC123_IN11 */
    {GPIOC, GPIO_Pin_2},    /* ADC123_IN12 */
    {GPIOC, GPIO_Pin_3},    /* ADC123_IN13 */
    {GPIOC, GPIO_Pin_4},    /* ADC12_IN14 */
    {GPIOC, GPIO_Pin_5},    /* ADC12_IN15 */

};

void adc_init_all(uint32_t resolution) {
  ADC_InitTypeDef       ADC_InitStructure;
  GPIO_InitTypeDef      GPIO_InitStructure;
  ADC_CommonInitTypeDef ADC_CommonInitStructure;

  /* Enable ADCx, DMA and GPIO clocks */ 
#if 0
  /* GPIO clocks enabled in main */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA |
                         RCC_AHB1Periph_GPIOB |
                         RCC_AHB1Periph_GPIOC, ENABLE);
#endif
  RCC_APB2PeriphClockCmd(ADCx_CLK, ENABLE);

  /* ADC Common Init */
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
  ADC_CommonInit(&ADC_CommonInitStructure);

  /* Configure ADC GPIOs */
  for (int i=0; i<ADC_NUM_CHANNELS; i++) {
      GPIO_InitStructure.GPIO_Pin = adc_gpio[i].pin;
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
      GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
      GPIO_Init(adc_gpio[i].port, &GPIO_InitStructure);
  }

  /* ADCx Init */
//  ADC_DeInit();
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion = 1;
  ADC_Init(ADCx, &ADC_InitStructure);

  /* Enable ADCx */
  ADC_Cmd(ADCx, ENABLE);

  /* Enable VBAT/VREF monitor */
  ADC_VBATCmd(ENABLE);

  /* Enable temperature sensor */
  ADC_TempSensorVrefintCmd(ENABLE);
}

void adc_init_single(uint32_t channel) {
  ADC_InitTypeDef       ADC_InitStructure;
  GPIO_InitTypeDef      GPIO_InitStructure;
  ADC_CommonInitTypeDef ADC_CommonInitStructure;

  /* Enable ADCx, DMA and GPIO clocks */ 
#if 0
  /* GPIO clocks enabled in main */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA |
                         RCC_AHB1Periph_GPIOB |
                         RCC_AHB1Periph_GPIOC, ENABLE);
#endif
  RCC_APB2PeriphClockCmd(ADCx_CLK, ENABLE);

  /* ADC Common Init */
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
  ADC_CommonInit(&ADC_CommonInitStructure);

  /* Configure ADC GPIO for the single channel */
  GPIO_InitStructure.GPIO_Pin = adc_gpio[channel].pin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(adc_gpio[channel].port, &GPIO_InitStructure);

  /* ADCx Init */
//  ADC_DeInit();
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion = 1;
  ADC_Init(ADCx, &ADC_InitStructure);

  /* Enable ADCx */
  ADC_Cmd(ADCx, ENABLE);

  /* Enable VBAT/VREF monitor */
  ADC_VBATCmd(ENABLE);

  /* Enable temperature sensor */
  ADC_TempSensorVrefintCmd(ENABLE);
}

uint32_t adc_read_channel(int channel)
{
    int timeout = 10000;

    if (channel > (ADC_NUM_CHANNELS-1)) {
        return 0;
    }

    /* ADC regular channel config ADC/Channel/SEQ Rank/Sample time */
    ADC_RegularChannelConfig(ADCx, channel, 1, ADC_SampleTime_15Cycles);

    /* Start ADC single conversion */
    ADC_SoftwareStartConv(ADCx);

    /* Wait for conversion to be complete*/
    while(!ADC_GetFlagStatus(ADCx, ADC_FLAG_EOC) && --timeout >0) {
    }

    /* ADC conversion timed out */
    if (timeout == 0) {
        return 0;
    }

    /* Return converted data */
    return ADC_GetConversionValue(ADCx); 
}

int adc_read_core_temp()
{
    int timeout = 10000;

    /* ADC temperature sensor channel config ADC/Channel/SEQ Rank/Sample time */
    /* Note: sample time must be higher than minimum sample time */
    ADC_RegularChannelConfig(ADCx, ADC_TEMP_CHANNEL, 1, ADC_SampleTime_480Cycles);

    /* Start ADC single conversion */
    ADC_SoftwareStartConv(ADCx);

    /* Wait for conversion to be complete*/
    while(!ADC_GetFlagStatus(ADCx, ADC_FLAG_EOC) && --timeout >0) {
    }

    /* ADC conversion timed out */
    if (timeout == 0) {
        return 0;
    }

    /* Convert ADC reading to temperature */
    /* Temperature formula from datasheet P.411 */
    return ((ADC_GetConversionValue(ADCx) - CORE_TEMP_V25) / CORE_TEMP_AVG_SLOPE) + 25;
}

float adc_read_core_vbat()
{
    int timeout = 10000;

    /* ADC VBAT channel config ADC/Channel/SEQ Rank/Sample time */
    /* Note: sample time must be higher than minimum sample time */
    ADC_RegularChannelConfig(ADCx, ADC_VBAT_CHANNEL, 1, ADC_SampleTime_144Cycles);

    /* Start ADC single conversion */
    ADC_SoftwareStartConv(ADCx);

    /* Wait for conversion to be complete */
    while(!ADC_GetFlagStatus(ADCx, ADC_FLAG_EOC) && --timeout >0) {
    }

    /* ADC conversion timed out */
    if (timeout == 0) {
        return 0;
    }

    /* Convert ADC reading to voltage, VBAT pin is
       internally connected to a bridge divider by VBAT_DIV */
    return ADC_GetConversionValue(ADCx)*VBAT_DIV/4096.0f*3.3f;
}

float adc_read_core_vref()
{
    int timeout = 10000;

    /* ADC VBAT channel config ADC/Channel/SEQ Rank/Sample time */
    /* Note: sample time must be higher than minimum sample time */
    ADC_RegularChannelConfig(ADCx, ADC_VREF_CHANNEL, 1, ADC_SampleTime_112Cycles);

    /* Start ADC single conversion */
    ADC_SoftwareStartConv(ADCx);

    /* Wait for conversion to be complete*/
    while(!ADC_GetFlagStatus(ADCx, ADC_FLAG_EOC) && --timeout >0) {
    }

    /* ADC conversion timed out */
    if (timeout == 0) {
        return 0;
    }

    /* Convert ADC reading to voltage */
    return ADC_GetConversionValue(ADCx)/4096.0f*3.3f;
}

/******************************************************************************/
/* Micro Python bindings : adc_all object                                     */

typedef struct _pyb_obj_adc_all_t {
    mp_obj_base_t base;
    bool is_enabled;
} pyb_obj_adc_all_t;

static void adc_all_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    print(env, "<ADC all>");
}

static mp_obj_t adc_all_read_channel(mp_obj_t self_in, mp_obj_t channel) {
    pyb_obj_adc_all_t *self = self_in;

    if (self->is_enabled) {
        uint32_t chan = mp_obj_get_int(channel);
        uint32_t data = adc_read_channel(chan);
        return mp_obj_new_int(data);
    } else {
        return mp_const_none;
    }
}

static mp_obj_t adc_all_read_core_temp(mp_obj_t self_in) {
    pyb_obj_adc_all_t *self = self_in;

    if (self->is_enabled) {
        int data  = adc_read_core_temp();
        return mp_obj_new_int(data);
    } else {
        return mp_const_none;
    }
}

static mp_obj_t adc_all_read_core_vbat(mp_obj_t self_in) {
    pyb_obj_adc_all_t *self = self_in;

    if (self->is_enabled) {
        float data = adc_read_core_vbat();
        return mp_obj_new_float(data);
    } else {
        return mp_const_none;
    }
}

static mp_obj_t adc_all_read_core_vref(mp_obj_t self_in) {
    pyb_obj_adc_all_t *self = self_in;

    if (self->is_enabled) {
        float data  = adc_read_core_vref();
        return mp_obj_new_float(data);
    } else {
        return mp_const_none;
    }
}

static MP_DEFINE_CONST_FUN_OBJ_2(adc_all_read_channel_obj, adc_all_read_channel);
static MP_DEFINE_CONST_FUN_OBJ_1(adc_all_read_core_temp_obj, adc_all_read_core_temp);
static MP_DEFINE_CONST_FUN_OBJ_1(adc_all_read_core_vbat_obj, adc_all_read_core_vbat);
static MP_DEFINE_CONST_FUN_OBJ_1(adc_all_read_core_vref_obj, adc_all_read_core_vref);

STATIC const mp_map_elem_t adc_all_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_read_channel), (mp_obj_t)  &adc_all_read_channel_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_read_core_temp), (mp_obj_t)&adc_all_read_core_temp_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_read_core_vbat), (mp_obj_t)&adc_all_read_core_vbat_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_read_core_vref), (mp_obj_t)&adc_all_read_core_vref_obj},
};

STATIC MP_DEFINE_CONST_DICT(adc_all_locals_dict, adc_all_locals_dict_table);

static const mp_obj_type_t adc_all_type = {
    { &mp_type_type },
    .name = MP_QSTR_ADC,
    .print = adc_all_print,
    .locals_dict = (mp_obj_t)&adc_all_locals_dict,
};

mp_obj_t pyb_ADC_all(mp_obj_t resolution) {
    /* init ADC */
    adc_init_all(mp_obj_get_int(resolution));

    pyb_obj_adc_all_t *o = m_new_obj(pyb_obj_adc_all_t);
    o->base.type = &adc_all_type;
    o->is_enabled = true;
    return o;
}

MP_DEFINE_CONST_FUN_OBJ_1(pyb_ADC_all_obj, pyb_ADC_all);

/******************************************************************************/
/* Micro Python bindings : adc object (single channel)                        */

typedef struct _pyb_obj_adc_t {
    mp_obj_base_t base;
    mp_obj_t pin_name;
    int channel;
    bool is_enabled;
} pyb_obj_adc_t;

static void adc_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_obj_adc_t *self = self_in;
    print(env, "<ADC on ");
    mp_obj_print_helper(print, env, self->pin_name, PRINT_STR);
    print(env, " channel=%lu>", self->channel);
}

static mp_obj_t adc_read(mp_obj_t self_in) {
    pyb_obj_adc_t *self = self_in;

    if (self->is_enabled) {
        uint32_t data = adc_read_channel(self->channel);
        return mp_obj_new_int(data);
    } else {
        return mp_const_none;
    }
}

static MP_DEFINE_CONST_FUN_OBJ_1(adc_read_obj, adc_read);

STATIC const mp_map_elem_t adc_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_read), (mp_obj_t)&adc_read_obj},
};

STATIC MP_DEFINE_CONST_DICT(adc_locals_dict, adc_locals_dict_table);

static const mp_obj_type_t adc_type = {
    { &mp_type_type },
    .name = MP_QSTR_ADC,
    .print = adc_print,
    .locals_dict = (mp_obj_t)&adc_locals_dict,
};

mp_obj_t pyb_ADC(mp_obj_t pin_name_obj) {

    pyb_obj_adc_t *o = m_new_obj(pyb_obj_adc_t);
    o->base.type = &adc_type;
    o->pin_name = pin_name_obj;

    // work out the channel from the pin name
    const char *pin_name = mp_obj_str_get_str(pin_name_obj);
    GPIO_TypeDef *port;
    switch (pin_name[0]) {
        case 'A': case 'a': port = GPIOA; break;
        case 'B': case 'b': port = GPIOB; break;
        case 'C': case 'c': port = GPIOC; break;
        default: goto pin_error;
    }
    uint pin_num = 0;
    for (const char *s = pin_name + 1; *s; s++) {
        if (!('0' <= *s && *s <= '9')) {
            goto pin_error;
        }
        pin_num = 10 * pin_num + *s - '0';
    }
    if (!(0 <= pin_num && pin_num <= 15)) {
        goto pin_error;
    }

    int i;
    for (i = 0; i < ADC_NUM_CHANNELS; i++) {
        if (adc_gpio[i].port == port && adc_gpio[i].pin == (1 << pin_num)) {
            o->channel = i;
            break;
        }
    }

    if (i == ADC_NUM_CHANNELS) {
        nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "pin %s does not have ADC capabilities", pin_name));
    }

    // init ADC just for this channel
    adc_init_single(o->channel);

    o->is_enabled = true;

    return o;

pin_error:
    nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "pin %s does not exist", pin_name));
}

MP_DEFINE_CONST_FUN_OBJ_1(pyb_ADC_obj, pyb_ADC);
