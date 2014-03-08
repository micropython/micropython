#include <stdint.h>
#include <string.h>

#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_dac.h"

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "parse.h"
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

/******************************************************************************/
// Micro Python bindings

typedef struct _pyb_audio_t {
    mp_obj_base_t base;
    int dac_id; // 1 or 2
} pyb_audio_t;

// direct access to DAC
mp_obj_t pyb_audio_dac(mp_obj_t self_in, mp_obj_t val) {
    pyb_audio_t *self = self_in;
    if (self->dac_id == 1) {
        DAC_SetChannel1Data(DAC_Align_8b_R, mp_obj_get_int(val));
    } else {
        DAC_SetChannel2Data(DAC_Align_8b_R, mp_obj_get_int(val));
    }
    return mp_const_none;
}

mp_obj_t pyb_audio_is_full(mp_obj_t self_in) {
    if (audio_is_full()) {
        return mp_const_true;
    } else {
        return mp_const_false;
    }
}

mp_obj_t pyb_audio_fill(mp_obj_t self_in, mp_obj_t val) {
    audio_fill(mp_obj_get_int(val));
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_audio_dac_obj, pyb_audio_dac);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_audio_is_full_obj, pyb_audio_is_full);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_audio_fill_obj, pyb_audio_fill);

STATIC const mp_method_t pyb_audio_methods[] = {
    { "dac", &pyb_audio_dac_obj },
    { "is_full", &pyb_audio_is_full_obj },
    { "fill", &pyb_audio_fill_obj },
    { NULL, NULL },
};

STATIC const mp_obj_type_t pyb_audio_type = {
    { &mp_type_type },
    .name = MP_QSTR_,
    .methods = pyb_audio_methods,
};

STATIC const pyb_audio_t pyb_audio_channel_1 = {{&pyb_audio_type}, 1};
STATIC const pyb_audio_t pyb_audio_channel_2 = {{&pyb_audio_type}, 2};

// create the audio object
// currently support either DAC1 on X5 (id = 1) or DAC2 on X6 (id = 2)

STATIC mp_obj_t pyb_Audio(mp_obj_t id) {
    // DAC peripheral clock
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

    int dac_id = mp_obj_get_int(id);
    uint pin;
    uint channel;
    mp_obj_t dac_obj;

    if (dac_id == 1) {
        pin = GPIO_Pin_4;
        channel = DAC_Channel_1;
        dac_obj = (mp_obj_t)&pyb_audio_channel_1;
    } else {
        pin = GPIO_Pin_5;
        channel = DAC_Channel_2;
        dac_obj = (mp_obj_t)&pyb_audio_channel_2;
    }

    // DAC channel configuration
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // DAC channel Configuration
    DAC_InitTypeDef DAC_InitStructure;
    DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;
    DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
    DAC_Init(channel, &DAC_InitStructure);

    // Enable DAC Channel
    DAC_Cmd(channel, ENABLE);

    // from now on use DAC_SetChannel[12]Data to trigger a conversion

    sample_buf_in = 0;
    sample_buf_out = 0;

    // return static object
    return dac_obj;
}

MP_DEFINE_CONST_FUN_OBJ_1(pyb_Audio_obj, pyb_Audio);
