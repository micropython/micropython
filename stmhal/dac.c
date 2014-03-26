#include <stdint.h>
#include <string.h>

#include "stm32f4xx_hal.h"

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "parse.h"
#include "obj.h"
#include "map.h"
#include "runtime.h"
#include "dac.h"

TIM_HandleTypeDef TIM6_Handle;
STATIC DAC_HandleTypeDef DAC_Handle;

void dac_init(void) {
    DAC_Handle.Instance = DAC;
    DAC_Handle.State = HAL_DAC_STATE_RESET;
    HAL_DAC_Init(&DAC_Handle);
}

STATIC void TIM6_Config(uint freq) {
    // TIM6 clock enable
    __TIM6_CLK_ENABLE();

    // Compute the prescaler value so TIM6 triggers at freq-Hz
    uint16_t period = (uint16_t) ((SystemCoreClock / 2) / freq) - 1;

    // time base clock configuration
    TIM6_Handle.Instance = TIM6;
    TIM6_Handle.Init.Period = period;
    TIM6_Handle.Init.Prescaler = 0; // timer runs at SystemCoreClock / 2
    TIM6_Handle.Init.ClockDivision = 0; // unused for TIM6
    TIM6_Handle.Init.CounterMode = TIM_COUNTERMODE_UP; // unused for TIM6
    HAL_TIM_Base_Init(&TIM6_Handle);

    // TIM6 TRGO selection
    TIM_MasterConfigTypeDef config;
    config.MasterOutputTrigger = TIM_TRGO_UPDATE;
    config.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    HAL_TIMEx_MasterConfigSynchronization(&TIM6_Handle, &config);

    // TIM6 start counter
    HAL_TIM_Base_Start(&TIM6_Handle);
}

/******************************************************************************/
// Micro Python bindings

typedef struct _pyb_dac_obj_t {
    mp_obj_base_t base;
    uint32_t dac_channel; // DAC_CHANNEL_1 or DAC_CHANNEL_2
    DMA_Stream_TypeDef *dma_stream; // DMA1_Stream5 or DMA1_Stream6
    machine_uint_t state;
} pyb_dac_obj_t;

STATIC pyb_dac_obj_t pyb_dac_channel_1 = {{&pyb_dac_type}, DAC_CHANNEL_1, DMA1_Stream5};
STATIC pyb_dac_obj_t pyb_dac_channel_2 = {{&pyb_dac_type}, DAC_CHANNEL_2, DMA1_Stream6};

// create the dac object
// currently support either DAC1 on X5 (id = 1) or DAC2 on X6 (id = 2)

STATIC mp_obj_t pyb_dac_make_new(mp_obj_t type_in, uint n_args, uint n_kw, const mp_obj_t *args) {
    // check arguments
    rt_check_nargs(n_args, 1, 1, n_kw, false);

    machine_int_t dac_id = mp_obj_get_int(args[0]);
    uint32_t pin;
    pyb_dac_obj_t *dac_obj;

    if (dac_id == 1) {
        pin = GPIO_PIN_4;
        dac_obj = &pyb_dac_channel_1;
    } else if (dac_id == 2) {
        pin = GPIO_PIN_5;
        dac_obj = &pyb_dac_channel_2;
    } else {
        nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "Dac %d does not exist", dac_id));
    }

    // GPIO configuration
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Pin = pin;
    GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

    // DAC peripheral clock
    __DAC_CLK_ENABLE();

    // stop anything already going on
    HAL_DAC_Stop(&DAC_Handle, dac_obj->dac_channel);
    HAL_DAC_Stop_DMA(&DAC_Handle, dac_obj->dac_channel);

    dac_obj->state = 0;

    // return object
    return dac_obj;
}

STATIC mp_obj_t pyb_dac_noise(mp_obj_t self_in, mp_obj_t freq) {
    pyb_dac_obj_t *self = self_in;

    // set TIM6 to trigger the DAC at the given frequency
    TIM6_Config(mp_obj_get_int(freq));

    if (self->state != 2) {
        // configure DAC to trigger via TIM6
        DAC_ChannelConfTypeDef config;
        config.DAC_Trigger = DAC_TRIGGER_T6_TRGO;
        config.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
        HAL_DAC_ConfigChannel(&DAC_Handle, &config, self->dac_channel);
        self->state = 2;
    }

    // set noise wave generation
    HAL_DACEx_NoiseWaveGenerate(&DAC_Handle, self->dac_channel, DAC_LFSRUNMASK_BITS10_0);
    HAL_DAC_SetValue(&DAC_Handle, self->dac_channel, DAC_ALIGN_12B_L, 0x7ff0);
    HAL_DAC_Start(&DAC_Handle, self->dac_channel);

    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_dac_noise_obj, pyb_dac_noise);

STATIC mp_obj_t pyb_dac_triangle(mp_obj_t self_in, mp_obj_t freq) {
    pyb_dac_obj_t *self = self_in;

    // set TIM6 to trigger the DAC at the given frequency
    TIM6_Config(mp_obj_get_int(freq));

    if (self->state != 2) {
        // configure DAC to trigger via TIM6
        DAC_ChannelConfTypeDef config;
        config.DAC_Trigger = DAC_TRIGGER_T6_TRGO;
        config.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
        HAL_DAC_ConfigChannel(&DAC_Handle, &config, self->dac_channel);
        self->state = 2;
    }

    // set triangle wave generation
    HAL_DACEx_TriangleWaveGenerate(&DAC_Handle, self->dac_channel, DAC_TRIANGLEAMPLITUDE_1023);
    HAL_DAC_SetValue(&DAC_Handle, self->dac_channel, DAC_ALIGN_12B_R, 0x100);
    HAL_DAC_Start(&DAC_Handle, self->dac_channel);

    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_dac_triangle_obj, pyb_dac_triangle);

// direct access to DAC (8 bit only at the moment)
STATIC mp_obj_t pyb_dac_write(mp_obj_t self_in, mp_obj_t val) {
    pyb_dac_obj_t *self = self_in;

    if (self->state != 1) {
        DAC_ChannelConfTypeDef config;
        config.DAC_Trigger = DAC_TRIGGER_NONE;
        config.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;
        HAL_DAC_ConfigChannel(&DAC_Handle, &config, self->dac_channel);
        self->state = 1;
    }

    HAL_DAC_SetValue(&DAC_Handle, self->dac_channel, DAC_ALIGN_8B_R, mp_obj_get_int(val));
    HAL_DAC_Start(&DAC_Handle, self->dac_channel);

    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_dac_write_obj, pyb_dac_write);

// initiates a burst of RAM->DAC using DMA
// input data is treated as an array of bytes (8 bit data)
// TIM6 is used to set the frequency of the transfer
// TODO still needs some attention to get it working properly
mp_obj_t pyb_dac_dma(uint n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    pyb_dac_obj_t *self = args[0];

    // set TIM6 to trigger the DAC at the given frequency
    TIM6_Config(mp_obj_get_int(args[2]));

    mp_obj_type_t *type = mp_obj_get_type(args[1]);
    if (type->buffer_p.get_buffer == NULL) {
        nlr_jump(mp_obj_new_exception_msg(&mp_type_TypeError, "buffer argument must support buffer protocol"));
    }
    buffer_info_t bufinfo;
    type->buffer_p.get_buffer(args[1], &bufinfo, BUFFER_READ);

    __DMA1_CLK_ENABLE();

    /*
    DMA_Cmd(self->dma_stream, DISABLE);
    while (DMA_GetCmdStatus(self->dma_stream) != DISABLE) {
    }

    DAC_Cmd(self->dac_channel, DISABLE);
    */

    /*
    // DAC channel configuration
    DAC_InitTypeDef DAC_InitStructure;
    DAC_InitStructure.DAC_Trigger = DAC_Trigger_T7_TRGO;
    DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
    DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_TriangleAmplitude_1; // unused, but need to set it to a valid value
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
    DAC_Init(self->dac_channel, &DAC_InitStructure);
    */

    if (self->state != 3) {
        DAC_ChannelConfTypeDef config;
        config.DAC_Trigger = DAC_TRIGGER_T6_TRGO;
        config.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
        HAL_DAC_ConfigChannel(&DAC_Handle, &config, self->dac_channel);
        self->state = 3;
    }

    // DMA1_Stream[67] channel7 configuration
    DMA_HandleTypeDef DMA_Handle;
    DMA_Handle.Instance = self->dma_stream;
    DMA_Handle.Init.Channel = DMA_CHANNEL_7;
    DMA_Handle.Init.Direction = DMA_MEMORY_TO_PERIPH;
    DMA_Handle.Init.PeriphInc = DMA_PINC_DISABLE;
    DMA_Handle.Init.MemInc = DMA_MINC_ENABLE;
    DMA_Handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    DMA_Handle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    mp_map_elem_t *kw_mode = mp_map_lookup(kw_args, MP_OBJ_NEW_QSTR(qstr_from_str("mode")), MP_MAP_LOOKUP);
    DMA_Handle.Init.Mode = kw_mode == NULL ? DMA_NORMAL : mp_obj_get_int(kw_mode->value); // normal = 0, circular = 0x100
    DMA_Handle.Init.Priority = DMA_PRIORITY_HIGH;
    DMA_Handle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    DMA_Handle.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL;
    DMA_Handle.Init.MemBurst = DMA_MBURST_SINGLE;
    DMA_Handle.Init.PeriphBurst = DMA_PBURST_SINGLE;
    HAL_DMA_Init(&DMA_Handle);

    __HAL_LINKDMA(&DAC_Handle, DMA_Handle1, DMA_Handle);

    HAL_DAC_Start_DMA(&DAC_Handle, self->dac_channel, (uint32_t*)bufinfo.buf, bufinfo.len, DAC_ALIGN_8B_R);

    /*
    // enable DMA stream
    DMA_Cmd(self->dma_stream, ENABLE);
    while (DMA_GetCmdStatus(self->dma_stream) == DISABLE) {
    }

    // enable DAC channel
    DAC_Cmd(self->dac_channel, ENABLE);

    // enable DMA for DAC channel
    DAC_DMACmd(self->dac_channel, ENABLE);
    */

    //printf("DMA: %p %lu\n", bufinfo.buf, bufinfo.len);

    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_dac_dma_obj, 3, pyb_dac_dma);

STATIC const mp_map_elem_t pyb_dac_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_noise), (mp_obj_t)&pyb_dac_noise_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_triangle), (mp_obj_t)&pyb_dac_triangle_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_write), (mp_obj_t)&pyb_dac_write_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_dma), (mp_obj_t)&pyb_dac_dma_obj },
    // TODO add function that does double buffering:
    //  dma2(freq, buf1, buf2, callback)
    //  where callback is called when the buffer has been drained
};

STATIC MP_DEFINE_CONST_DICT(pyb_dac_locals_dict, pyb_dac_locals_dict_table);

const mp_obj_type_t pyb_dac_type = {
    { &mp_type_type },
    .name = MP_QSTR_DAC,
    .make_new = pyb_dac_make_new,
    .locals_dict = (mp_obj_t)&pyb_dac_locals_dict,
};
