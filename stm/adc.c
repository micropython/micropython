#include <stdio.h>
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_adc.h>

#include "misc.h"
#include "mpconfig.h"
#include "obj.h"
#include "adc.h"

/* ADC defintions */
#define ADCx                (ADC1)
#define ADCx_CLK            (RCC_APB2Periph_ADC1)
#define ADC_NUM_CHANNELS    (16)

/* GPIO struct */
typedef struct {
    GPIO_TypeDef* port;
    uint32_t pin;
} gpio_t;

/* ADC GPIOs */
static gpio_t adc_gpio[] = {
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

void adc_init(uint32_t resolution) {
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
}

uint32_t adc_read_channel(int channel)
{
    int timeout = 10000;

    if (channel > (ADC_NUM_CHANNELS-1)) {
        return 0;
    }

    /* ADC regular channel config ADC/Channel/SEQ Rank/Sample time */
    ADC_RegularChannelConfig(ADCx, channel, 1, ADC_SampleTime_3Cycles);

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

/******************************************************************************/
/* Micro Python bindings                                                      */

typedef struct _pyb_adc_obj_t {
    mp_obj_base_t base;
    int adc_id;
    bool is_enabled;
} pyb_adc_obj_t;

static mp_obj_t adc_obj_read_channel(mp_obj_t self_in, mp_obj_t channel) {
    mp_obj_t ret = mp_const_none;
    pyb_adc_obj_t *self = self_in;

    if (self->is_enabled) {
        uint32_t chan = mp_obj_get_int(channel);
        uint32_t data = adc_read_channel(chan);
        ret = mp_obj_new_int(data);
    }
    return ret;
}

static void adc_obj_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in) {
    pyb_adc_obj_t *self = self_in;
    print(env, "<ADC %lu>", self->adc_id);
}

static MP_DEFINE_CONST_FUN_OBJ_2(adc_obj_read_channel_obj, adc_obj_read_channel);

static const mp_method_t adc_methods[] = {
    { "read_channel",   &adc_obj_read_channel_obj},
    { NULL, NULL },
};

static const mp_obj_type_t adc_obj_type = {
    { &mp_const_type },
    "ADC",
    .print = adc_obj_print,
    .methods = adc_methods,
};

mp_obj_t pyb_ADC(mp_obj_t resolution) {
    /* init ADC */
    adc_init(mp_obj_get_int(resolution));

    pyb_adc_obj_t *o = m_new_obj(pyb_adc_obj_t);
    o->base.type = &adc_obj_type;
    o->adc_id = 1;
    o->is_enabled = true;
    return o;
}
