// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Matthew McGowan for Blues Inc.
//
// SPDX-License-Identifier: MIT


#include <stm32l4xx_hal.h>
#include "MEMS_Audio_ll_stm32l4.h"
#include "MEMS_Audio.h"

/**
 * @brief The implementation is a singleton.
 *
 */
MemsAudio_STM32L4SAIPDM *volatile audioImpl;

static mems_audio_err_t MX_DMA_Init(void);
static mems_audio_err_t MX_DMA_Uninit(void);
static mems_audio_err_t MX_SAI1_Init(void);

#define CHECK_HAL_ERROR(x, e) \
    {                         \
        if ((x) != HAL_OK)    \
        return e;         \
    }

/**
 * @brief Checks the HAL return code and returns from a void function on error. The
 * error is saved to lastError.
 */
#define CHECK_HAL_ERROR_VOID(x, e)    \
    {                                 \
        if ((x) != HAL_OK) {          \
            audioImpl->lastError = e; \
            return;                   \
        }                             \
    }

#define CHECK_MEMS_AUDIO_ERROR_LAST()            \
    {                                            \
        if (audioImpl->lastError != MEMS_AUDIO_OK) \
        return audioImpl->lastError;    \
    }

static bool default_pdm_data_available(MemsAudio_STM32L4SAIPDM *audio, pdm_sample_t *pdmSamples, size_t count) {
    return true;
}

int filter_pdm(MemsAudio_STM32L4SAIPDM *impl, pdm_sample_t *input, pcm_sample_t *output) {
    if (impl->filter.Decimation == 64) {
        Open_PDM_Filter_64(input, output, 1, &impl->filter);
    } else {
        Open_PDM_Filter_128(input, output, 1, &impl->filter);
    }
    return impl->filter.nSamples;
}

static void mems_audio_init_filter(MemsAudio_STM32L4SAIPDM *impl) {
    TPDMFilter_InitStruct *filter = &impl->filter;
    filter->Fs = PCM_OUT_SAMPLING_FREQUENCY;
    filter->nSamples = MEMS_AUDIO_PCM_BUFFER_LENGTH;
    filter->LP_HZ = PCM_OUT_SAMPLING_FREQUENCY / 2; // The Nyquist frequency
    filter->HP_HZ = 10;                             // high pass to remove DC offset
    filter->In_MicChannels = 1;
    filter->Out_MicChannels = 1;
    filter->Decimation = PDM_IN_DECIMATION_FACTOR;
    Open_PDM_Filter_Init(filter);
}
volatile unsigned ignore_dma_count;

/**
 * @brief Converts PDM samples
 *
 * @param pdmBuffer The buffer holding the PDM samples
 * @param pdmBufferLength   The number of samples available
 */
void pdm2pcm(uint8_t *pdmBuffer, size_t pdmBufferLength) {
    MemsAudio_STM32L4SAIPDM *impl = audioImpl;
    if (impl) {
        bool convert = impl->discard_dma || impl->pdm_data_available(impl, pdmBuffer, pdmBufferLength);
        if (convert) {
            MemsAudio *audio = impl->audio;
            filter_pdm(impl, pdmBuffer, (pcm_sample_t *)audio->pcmOutputBuffer);
            if (!impl->discard_dma) {
                audio->pcm_data_available(audio, (pcm_sample_t *)audio->pcmOutputBuffer, impl->filter.nSamples);
            } else {
                impl->discard_dma--;
            }
        }
    }
}

/**
 *  @brief Initialize the PDM interface ready to begin capture.
 *  @retval
 */
mems_audio_err_t mems_audio_ll_init(MemsAudio *audio) {
    mems_audio_init_filter(audioImpl);
    if (!audioImpl->pdm_data_available) {
        audioImpl->pdm_data_available = &default_pdm_data_available;
    }

    CHECK_MEMS_AUDIO_ERROR(MX_DMA_Init());
    CHECK_MEMS_AUDIO_ERROR(MX_SAI1_Init());
    return MEMS_AUDIO_OK;
}

mems_audio_err_t uninit(void) {
    if (audioImpl) {
        MemsAudio_STM32L4SAIPDM *impl = audioImpl;
        audioImpl = NULL;
        mems_audio_ll_stop(impl->audio);
        CHECK_HAL_ERROR(HAL_SAI_DeInit(&impl->hSAI_BlockA1), MEMS_AUDIO_ERROR_SAI_DEINIT);
        CHECK_MEMS_AUDIO_ERROR(MX_DMA_Uninit());
    }
    return MEMS_AUDIO_OK;
}

/**
 * @brief Uninitialize low level PDM capture
 */
mems_audio_err_t mems_audio_ll_uninit(MemsAudio *audio) {
    if (audioImpl->audio == audio) {
        uninit();
    }
    return MEMS_AUDIO_OK;
}

mems_audio_err_t mems_audio_ll_record(MemsAudio *audio) {
    audioImpl->discard_dma = (100 / MEMS_AUDIO_MS_BUFFER) + 1;
    CHECK_HAL_ERROR(HAL_SAI_Receive_DMA(&audioImpl->hSAI_BlockA1, audioImpl->pdmBuffer, audioImpl->pdmBufferLength),
        MEMS_AUDIO_ERROR_DMA_START);
    return MEMS_AUDIO_OK;
}

mems_audio_err_t mems_audio_ll_stop(MemsAudio *audio) {
    CHECK_HAL_ERROR(HAL_SAI_DMAStop(&audioImpl->hSAI_BlockA1), MEMS_AUDIO_ERROR_DMA_STOP);
    return MEMS_AUDIO_OK;
}

mems_audio_err_t mems_audio_ll_pause(MemsAudio *audio) {
    CHECK_HAL_ERROR(HAL_SAI_DMAPause(&audioImpl->hSAI_BlockA1), MEMS_AUDIO_ERROR_DMA_PAUSE);
    return MEMS_AUDIO_OK;
}

mems_audio_err_t mems_audio_ll_resume(MemsAudio *audio) {
    CHECK_HAL_ERROR(HAL_SAI_DMAResume(&audioImpl->hSAI_BlockA1), MEMS_AUDIO_ERROR_DMA_RESUME);
    return MEMS_AUDIO_OK;
}

/**
 * @brief SAI1 Initialization Function
 * @param None
 * @retval None
 */
static mems_audio_err_t MX_SAI1_Init(void) {
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    SAI_HandleTypeDef hSAI_BlockA1 = {0};
    MemsAudio_STM32L4SAIPDM *impl = audioImpl;
    CHECK_MEMS_AUDIO_INITIALIZED(impl);
    hSAI_BlockA1.Instance = SAI1_Block_A;
    hSAI_BlockA1.Init.Protocol = SAI_FREE_PROTOCOL;
    hSAI_BlockA1.Init.AudioMode = SAI_MODEMASTER_RX;
    /* The PDM interface provides 8 1-bit samples at a time */
    hSAI_BlockA1.Init.DataSize = SAI_DATASIZE_8;
    hSAI_BlockA1.Init.FirstBit = SAI_FIRSTBIT_MSB;

    hSAI_BlockA1.Init.ClockStrobing = SAI_CLOCKSTROBING_FALLINGEDGE;
    hSAI_BlockA1.Init.Synchro = SAI_ASYNCHRONOUS;            /* asynchronous - not chained to other SAI blocks */
    hSAI_BlockA1.Init.OutputDrive = SAI_OUTPUTDRIVE_DISABLE; /* Not driving the primary SAI clock */
    hSAI_BlockA1.Init.NoDivider = SAI_MASTERDIVIDER_DISABLE;
    hSAI_BlockA1.Init.MckOverSampling = SAI_MCK_OVERSAMPLING_DISABLE;
    hSAI_BlockA1.Init.FIFOThreshold = SAI_FIFOTHRESHOLD_FULL;
    hSAI_BlockA1.Init.MonoStereoMode = SAI_MONOMODE; /* PDM is intrinsicly stereo, sampling on */
    hSAI_BlockA1.Init.CompandingMode = SAI_NOCOMPANDING;
    hSAI_BlockA1.Init.PdmInit.Activation = ENABLE; /* Enable PDM interface in the SAI */
    hSAI_BlockA1.Init.PdmInit.MicPairsNbr = 1;     /* 1 pair - 2 mics */
    hSAI_BlockA1.Init.PdmInit.ClockEnable = SAI_PDM_CLOCK1_ENABLE;
    hSAI_BlockA1.FrameInit.FrameLength = 16;
    hSAI_BlockA1.FrameInit.ActiveFrameLength = 1;
    hSAI_BlockA1.FrameInit.FSDefinition = SAI_FS_STARTFRAME; /* FS is not really used */
    hSAI_BlockA1.FrameInit.FSPolarity = SAI_FS_ACTIVE_HIGH;
    hSAI_BlockA1.FrameInit.FSOffset = SAI_FS_FIRSTBIT;
    hSAI_BlockA1.SlotInit.FirstBitOffset = 0;
    hSAI_BlockA1.SlotInit.SlotSize = SAI_SLOTSIZE_DATASIZE;
    hSAI_BlockA1.SlotInit.SlotNumber = 2;
    hSAI_BlockA1.SlotInit.SlotActive = 0x0001;
    impl->hSAI_BlockA1 = hSAI_BlockA1;
    CHECK_HAL_ERROR(HAL_SAI_Init(&impl->hSAI_BlockA1), MEMS_AUDIO_ERROR_SAI_INIT);
    CHECK_MEMS_AUDIO_ERROR_LAST();
    return MEMS_AUDIO_OK;
}

#define MEMS_AUDIO_DMA_IRQn DMA1_Channel6_IRQn
#define MEMS_AUDIO_DMA_CHANNEL DMA1_Channel6
#define MEMS_AUDIO_DMA_PRIORITY 6
#define DMA_HANDLER DMA1_Channel6_IRQHandler

void HAL_SAI_MspInit(SAI_HandleTypeDef *hsai) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
    /* SAI1 */
    MemsAudio_STM32L4SAIPDM *impl = audioImpl;
    if (hsai->Instance == SAI1_Block_A && impl) {
        PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_SAI1;
        PeriphClkInit.Sai1ClockSelection = RCC_SAI1CLKSOURCE_PLLSAI1;
        PeriphClkInit.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_MSI;
        PeriphClkInit.PLLSAI1.PLLSAI1M = MEMS_AUDIO_CLOCK_PLLM;
        PeriphClkInit.PLLSAI1.PLLSAI1N = MEMS_AUDIO_CLOCK_PLLN;
        PeriphClkInit.PLLSAI1.PLLSAI1P = MEMS_AUDIO_CLOCK_PLLP;
        PeriphClkInit.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV2;
        PeriphClkInit.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV2;
        PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_SAI1CLK;
        CHECK_HAL_ERROR_VOID(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit), MEMS_AUDIO_ERROR_SAI_CLOCK);

        if (impl->SAI1_client == 0) {
            __HAL_RCC_SAI1_CLK_ENABLE();
        }
        impl->SAI1_client++;

        /**SAI1_A_Block_A GPIO Configuration
        PC3     ------> SAI1_D1
        PA3     ------> SAI1_CK1
        */
        GPIO_InitStruct.Pin = GPIO_PIN_3;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF3_SAI1;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_3;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF3_SAI1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* Peripheral DMA init*/
        DMA_HandleTypeDef hdma_sai1_a = {0};
        hdma_sai1_a.Instance = MEMS_AUDIO_DMA_CHANNEL;
        hdma_sai1_a.Init.Request = DMA_REQUEST_SAI1_A;
        hdma_sai1_a.Init.Direction = DMA_PERIPH_TO_MEMORY;
        hdma_sai1_a.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_sai1_a.Init.MemInc = DMA_MINC_ENABLE;
        hdma_sai1_a.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hdma_sai1_a.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        hdma_sai1_a.Init.Mode = DMA_CIRCULAR;
        hdma_sai1_a.Init.Priority = DMA_PRIORITY_HIGH;
        impl->hdma_sai1_a = hdma_sai1_a;
        CHECK_HAL_ERROR_VOID(HAL_DMA_Init(&impl->hdma_sai1_a), MEMS_AUDIO_ERROR_SAI_DMA_INIT);

        /* Several peripheral DMA handle pointers point to the same DMA handle.
         Be aware that there is only one channel to perform all the requested DMAs. */
        __HAL_LINKDMA(hsai, hdmarx, impl->hdma_sai1_a);

        __HAL_LINKDMA(hsai, hdmatx, impl->hdma_sai1_a);
    }
}

void HAL_SAI_MspDeInit(SAI_HandleTypeDef *hsai) {
    /* SAI1 */
    MemsAudio_STM32L4SAIPDM *impl = audioImpl;
    if (hsai->Instance == SAI1_Block_A && impl) {
        impl->SAI1_client--;
        if (impl->SAI1_client == 0) {
            /* Peripheral clock disable */
            __HAL_RCC_SAI1_CLK_DISABLE();
        }

        /**SAI1_A_Block_A GPIO Configuration
        PC3     ------> SAI1_D1
        PA3     ------> SAI1_CK1
        */
        HAL_GPIO_DeInit(GPIOC, GPIO_PIN_3);

        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_3);

        /* SAI1 DMA Deinit */
        HAL_DMA_DeInit(hsai->hdmarx);
        HAL_DMA_DeInit(hsai->hdmatx);
    }
}

/**
 * @brief Initialize the DMA peripheral
 *
 */
static mems_audio_err_t MX_DMA_Init(void) {

    /* DMA controller clock enable */
    __HAL_RCC_DMAMUX1_CLK_ENABLE();
    __HAL_RCC_DMA1_CLK_ENABLE();

    /* DMA interrupt init */
    /* DMA1_Channel1_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(MEMS_AUDIO_DMA_IRQn, MEMS_AUDIO_DMA_PRIORITY, 0);
    HAL_NVIC_EnableIRQ(MEMS_AUDIO_DMA_IRQn);
    return MEMS_AUDIO_OK;
}

static mems_audio_err_t MX_DMA_Uninit(void) {
    HAL_NVIC_DisableIRQ(MEMS_AUDIO_DMA_IRQn);
    return MEMS_AUDIO_OK;
}

/**
 * @brief Global handler for the DMA interrupt. Forwards to the HAL for further processing.
 *
 */
void DMA_HANDLER(void) {
    HAL_DMA_IRQHandler(&audioImpl->hdma_sai1_a);
}

/**
 * @brief Converts PDM samples in the upper half of the PDM buffer.
 *
 * @param hSai
 */
void HAL_SAI_RxHalfCpltCallback(SAI_HandleTypeDef *hSai) {
    (void)hSai;
    pdm2pcm(audioImpl->pdmBuffer, audioImpl->pdmBufferLength >> 1);
}

/**
 * @brief Converts PDM samples in the upper half of the PDM buffer.
 *
 * @param hSai
 */
void HAL_SAI_RxCpltCallback(SAI_HandleTypeDef *hSai) {
    (void)hSai;
    pdm2pcm(audioImpl->pdmBuffer + (audioImpl->pdmBufferLength >> 1), audioImpl->pdmBufferLength >> 1);
}

mems_audio_err_t mems_audio_init_stm32l4_sai_pdm(MemsAudio *audio, MemsAudio_STM32L4SAIPDM *impl) {
    uninit();
    audioImpl = impl;
    impl->audio = audio;
    return mems_audio_init(audio);
}
