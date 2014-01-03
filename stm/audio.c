#include <stdint.h>

#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_dac.h"

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "parse.h"
#include "compile.h"
#include "obj.h"
#include "runtime.h"

#include "audio.h"

#define SAMPLE_BUF_SIZE (32)

// sample_buf_in is always the same or ahead of sample_buf_out
// when they are the same, there are no more samples left to process
// in this scheme, there is always 1 unusable byte in the buffer, just before sample_buf_out
int sample_buf_in;
int sample_buf_out;
byte sample_buf[SAMPLE_BUF_SIZE];

bool audio_is_full(void) {
    return ((sample_buf_in + 1) % SAMPLE_BUF_SIZE) == sample_buf_out;
}

void audio_fill(byte sample) {
    sample_buf[sample_buf_in] = sample;
    sample_buf_in = (sample_buf_in + 1) % SAMPLE_BUF_SIZE;
    // enable interrupt
}

void audio_drain(void) {
    if (sample_buf_in == sample_buf_out) {
        // buffer is empty; disable interrupt
    } else {
        // buffer has a sample; output it
        byte sample = sample_buf[sample_buf_out];
        DAC_SetChannel2Data(DAC_Align_8b_R, sample);
        sample_buf_out = (sample_buf_out + 1) % SAMPLE_BUF_SIZE;
    }
}

// direct access to DAC
mp_obj_t pyb_audio_dac(mp_obj_t val) {
    DAC_SetChannel2Data(DAC_Align_8b_R, mp_obj_get_int(val));
    return mp_const_none;
}

mp_obj_t pyb_audio_is_full(void) {
    if (audio_is_full()) {
        return mp_const_true;
    } else {
        return mp_const_false;
    }
}

mp_obj_t pyb_audio_fill(mp_obj_t val) {
    audio_fill(mp_obj_get_int(val));
    return mp_const_none;
}

void audio_init(void) {
    // DAC peripheral clock
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

    // DAC channel 2 (DAC_OUT2 = PA.5) configuration
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // DAC channel2 Configuration
    DAC_InitTypeDef DAC_InitStructure;
    DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;
    DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
    DAC_Init(DAC_Channel_2, &DAC_InitStructure);

    // Enable DAC Channel2
    DAC_Cmd(DAC_Channel_2, ENABLE);

    // from now on use DAC_SetChannel2Data to trigger a conversion

    sample_buf_in = 0;
    sample_buf_out = 0;
    // enable interrupt

    // Python interface
    mp_obj_t m = mp_obj_new_module(qstr_from_str_static("audio"));
    rt_store_attr(m, qstr_from_str_static("dac"), rt_make_function_1(pyb_audio_dac));
    rt_store_attr(m, qstr_from_str_static("is_full"), rt_make_function_0(pyb_audio_is_full));
    rt_store_attr(m, qstr_from_str_static("fill"), rt_make_function_1(pyb_audio_fill));
    rt_store_name(qstr_from_str_static("audio"), m);
}
