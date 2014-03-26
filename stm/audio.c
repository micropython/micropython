#include <stdint.h>
#include <string.h>

#include "stm32f4xx_dac.h"

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "parse.h"
#include "obj.h"
#include "map.h"
#include "runtime.h"

#include "audio.h"

STATIC void TIM7_Config(uint freq) {
    // TIM7 clock enable
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);

    // reset TIM7
    TIM_DeInit(TIM7);

    // Compute the prescaler value so TIM7 triggers at freq-Hz
    uint16_t period = (uint16_t) ((SystemCoreClock / 2) / freq) - 1;

    // Time base configuration
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period = period; // timer triggers with this period
    TIM_TimeBaseStructure.TIM_Prescaler = 0; // timer runs at SystemCoreClock / 2
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; // unused for TIM7
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // unused for TIM7
    TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure);

    // TIM7 TRGO selection
    TIM_SelectOutputTrigger(TIM7, TIM_TRGOSource_Update);

    // TIM7 enable counter
    TIM_Cmd(TIM7, ENABLE);
}

/******************************************************************************/
// Micro Python bindings

typedef struct _pyb_audio_t {
    mp_obj_base_t base;
    uint dac_channel; // DAC_Channel_1 or DAC_Channel_2
    DMA_Stream_TypeDef *dma_stream; // DMA1_Stream6 or DMA1_Stream7
} pyb_audio_t;

mp_obj_t pyb_audio_noise(mp_obj_t self_in, mp_obj_t freq) {
    pyb_audio_t *self = self_in;

    // set TIM7 to trigger the DAC at the given frequency
    TIM7_Config(mp_obj_get_int(freq));

    DAC_Cmd(self->dac_channel, DISABLE);

    DAC_InitTypeDef DAC_InitStructure;
    DAC_InitStructure.DAC_Trigger = DAC_Trigger_T7_TRGO;
    DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_Noise;
    DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bits10_0;
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
    DAC_Init(self->dac_channel, &DAC_InitStructure);

    DAC_Cmd(self->dac_channel, ENABLE);

    if (self->dac_channel == DAC_Channel_1) {
        DAC_SetChannel1Data(DAC_Align_12b_L, 0x7ff0);
    } else {
        DAC_SetChannel2Data(DAC_Align_12b_L, 0x7ff0);
    }

    return mp_const_none;
}

mp_obj_t pyb_audio_triangle(mp_obj_t self_in, mp_obj_t freq) {
    pyb_audio_t *self = self_in;

    // set TIM7 to trigger the DAC at the given frequency
    TIM7_Config(mp_obj_get_int(freq));

    DAC_Cmd(self->dac_channel, DISABLE);

    DAC_InitTypeDef DAC_InitStructure;
    DAC_InitStructure.DAC_Trigger = DAC_Trigger_T7_TRGO;
    DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_Triangle;
    DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_TriangleAmplitude_1023;
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
    DAC_Init(self->dac_channel, &DAC_InitStructure);

    DAC_Cmd(self->dac_channel, ENABLE);

    // set base value of triangle wave
    if (self->dac_channel == DAC_Channel_1) {
        DAC_SetChannel1Data(DAC_Align_12b_R, 0x100);
    } else {
        DAC_SetChannel2Data(DAC_Align_12b_R, 0x100);
    }

    return mp_const_none;
}

// direct access to DAC
mp_obj_t pyb_audio_dac(mp_obj_t self_in, mp_obj_t val) {
    pyb_audio_t *self = self_in;
    if (self->dac_channel == DAC_Channel_1) {
        DAC_SetChannel1Data(DAC_Align_8b_R, mp_obj_get_int(val));
    } else {
        DAC_SetChannel2Data(DAC_Align_8b_R, mp_obj_get_int(val));
    }
    return mp_const_none;
}

#define DAC_DHR8R1_ADDRESS (DAC_BASE + 0x10)
#define DAC_DHR8R2_ADDRESS (DAC_BASE + 0x1c)

// initiates a burst of RAM->DAC using DMA
// input data is treated as an array of bytes (8 bit data)
// TIM7 is used to set the frequency of the transfer
mp_obj_t pyb_audio_dma(uint n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    pyb_audio_t *self = args[0];

    // set TIM7 to trigger the DAC at the given frequency
    TIM7_Config(mp_obj_get_int(args[2]));

    mp_obj_type_t *type = mp_obj_get_type(args[1]);
    if (type->buffer_p.get_buffer == NULL) {
        nlr_jump(mp_obj_new_exception_msg(&mp_type_TypeError, "buffer argument must support buffer protocol"));
    }
    buffer_info_t bufinfo;
    type->buffer_p.get_buffer(args[1], &bufinfo, BUFFER_READ);

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);

    DMA_Cmd(self->dma_stream, DISABLE);
    while (DMA_GetCmdStatus(self->dma_stream) != DISABLE) {
    }

    DAC_Cmd(self->dac_channel, DISABLE);

    // DAC channel configuration
    DAC_InitTypeDef DAC_InitStructure;
    DAC_InitStructure.DAC_Trigger = DAC_Trigger_T7_TRGO;
    DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
    DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_TriangleAmplitude_1; // unused, but need to set it to a valid value
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
    DAC_Init(self->dac_channel, &DAC_InitStructure);

    // DMA1_Stream[67] channel7 configuration
    DMA_DeInit(self->dma_stream);
    DMA_InitTypeDef DMA_InitStructure;
    DMA_InitStructure.DMA_Channel = DMA_Channel_7;
    if (self->dac_channel == DAC_Channel_1) {
        DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR8R1_ADDRESS;
    } else {
        DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR8R2_ADDRESS;
    }
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)bufinfo.buf;
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
    DMA_InitStructure.DMA_BufferSize = bufinfo.len;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    mp_map_elem_t *kw_mode = mp_map_lookup(kw_args, MP_OBJ_NEW_QSTR(qstr_from_str("mode")), MP_MAP_LOOKUP);
    DMA_InitStructure.DMA_Mode = kw_mode == NULL ? DMA_Mode_Normal : mp_obj_get_int(kw_mode->value); // normal = 0, circular = 0x100
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(self->dma_stream, &DMA_InitStructure);

    // enable DMA stream
    DMA_Cmd(self->dma_stream, ENABLE);
    while (DMA_GetCmdStatus(self->dma_stream) == DISABLE) {
    }

    // enable DAC channel
    DAC_Cmd(self->dac_channel, ENABLE);

    // enable DMA for DAC channel
    DAC_DMACmd(self->dac_channel, ENABLE);

    //printf("DMA: %p %lu\n", bufinfo.buf, bufinfo.len);

    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_audio_noise_obj, pyb_audio_noise);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_audio_triangle_obj, pyb_audio_triangle);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_audio_dac_obj, pyb_audio_dac);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_audio_dma_obj, 3, pyb_audio_dma);

STATIC const mp_map_elem_t pyb_audio_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_noise), (mp_obj_t)&pyb_audio_noise_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_triangle), (mp_obj_t)&pyb_audio_triangle_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_dac), (mp_obj_t)&pyb_audio_dac_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_dma), (mp_obj_t)&pyb_audio_dma_obj },
};

STATIC MP_DEFINE_CONST_DICT(pyb_audio_locals_dict, pyb_audio_locals_dict_table);

STATIC const mp_obj_type_t pyb_audio_type = {
    { &mp_type_type },
    .name = MP_QSTR_,
    .locals_dict = (mp_obj_t)&pyb_audio_locals_dict,
};

STATIC const pyb_audio_t pyb_audio_channel_1 = {{&pyb_audio_type}, DAC_Channel_1, DMA1_Stream5};
STATIC const pyb_audio_t pyb_audio_channel_2 = {{&pyb_audio_type}, DAC_Channel_2, DMA1_Stream6};

// create the audio object
// currently support either DAC1 on X5 (id = 1) or DAC2 on X6 (id = 2)

STATIC mp_obj_t pyb_Audio(mp_obj_t id) {
    // DAC peripheral clock
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

    int dac_id = mp_obj_get_int(id);
    uint pin;
    const pyb_audio_t *dac_obj;

    if (dac_id == 1) {
        pin = GPIO_Pin_4;
        dac_obj = &pyb_audio_channel_1;
    } else {
        pin = GPIO_Pin_5;
        dac_obj = &pyb_audio_channel_2;
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
    DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_TriangleAmplitude_1023;
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
    DAC_Init(dac_obj->dac_channel, &DAC_InitStructure);

    // Enable DAC Channel
    DAC_Cmd(dac_obj->dac_channel, ENABLE);

    // from now on use DAC_SetChannel[12]Data to trigger a conversion

    // return static object
    return (mp_obj_t)dac_obj;
}

MP_DEFINE_CONST_FUN_OBJ_1(pyb_Audio_obj, pyb_Audio);
