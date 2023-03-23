/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Matthew McGowan for Blues Inc.
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


#ifndef _MEMS_AUDIO_LL_STM32L4_H_
#define _MEMS_AUDIO_LL_STM32L4_H_

#include <stdbool.h>
#include <assert.h>
#include <stm32l4xx_hal.h>
#include "OpenPDMFilter.h"
#include "MEMS_Audio.h"
#include "MEMS_Audio_ll.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The SAI PDM interface captures 8 bits from the PDM signal.
 */
typedef uint8_t pdm_sample_t;

/**
 * @brief The PDM sample frequency in kHz. (Bit samples per millisecond.)
 */
#define PDM_IN_FREQUENCY_KHZ                1024

#define PDM_IN_FREQUENCY                    (PDM_IN_FREQUENCY_KHZ * 1000)

/**
 * @brief The number of channels of audio captured
 */
#define PDM_IN_CHANNELS                     1

/**
 * @brief The decimation of the PDM bit stream to produce PCM samples at the desired output rate.
 */
#define PDM_IN_DECIMATION_FACTOR            64

/**
 * @brief The number of pdm samples captured per millisecond from the PDM interface.
 */
#define MEMS_AUDIO_PDM_SAMPLES_PER_MS ((PDM_IN_FREQUENCY_KHZ / (sizeof(pdm_sample_t) * 8)) * PDM_IN_CHANNELS)

/**
 * @brief The size of the buffer used to hold PDM samples prior to conversion to PCM.
 * Each half of the buffer generates an interrupt.
 */
#define MEMS_AUDIO_PDM_BUFFER_LENGTH (MEMS_AUDIO_PDM_SAMPLES_PER_MS * MEMS_AUDIO_MS_BUFFER * 2)

/**
 * @brief The length of the PCM buffer required to hold converted samples.
 */
#define MEMS_AUDIO_PCM_BUFFER_LENGTH (PCM_OUT_SAMPLING_FREQUENCY * MEMS_AUDIO_MS_BUFFER / 1000)


/**
 * Presently the internal PDM parameters and output PCM parameters are fixed for the values given here.
 */


/**
 * @brief 128 point decimation did not work with the OpenPDMFilter and just produced PCM output
 * approaching a square wave.
 */
_Static_assert(PDM_IN_DECIMATION_FACTOR == 64 || PDM_IN_DECIMATION_FACTOR == 128, "A decomation factor of 64 or 128 is supported at present.");


/**
 * @brief The PDM bitstream frequency divided by the decimation factor should be the same as the desired output PCM frequency.
 */
_Static_assert(PDM_IN_FREQUENCY / PDM_IN_DECIMATION_FACTOR == PCM_OUT_SAMPLING_FREQUENCY, "PDM output frequency should equal the input frequency divided by the decimation factor.");

//
// SAI PDM interface clock configuration
//

#define MEMS_AUDIO_MSI_FREQUENCY        (48 * 1000 * 1000)
#define MEMS_AUDIO_CLOCK_PLLM           (15)
#define MEMS_AUDIO_CLOCK_PLLN           (16)
#define MEMS_AUDIO_CLOCK_PLLP           (RCC_PLLP_DIV25)

/**
 * @brief The SAI PDM clock should be twice the desired PDM bitstream frequency
 */
_Static_assert((MEMS_AUDIO_MSI_FREQUENCY / MEMS_AUDIO_CLOCK_PLLM * MEMS_AUDIO_CLOCK_PLLN / MEMS_AUDIO_CLOCK_PLLP) == (PDM_IN_FREQUENCY_KHZ * 1000 * 2), "PDM clock should be twice the PDM sample frequency.");

typedef struct MemsAudio_STM32L4SAIPDM_t MemsAudio_STM32L4SAIPDM;

/**
 * @brief Callback informing that PDM samples are available for processing.
 * @param audio The MemsAudio instance
 * @return `false` to skip conversion of PDM to PCM. `true` to convert the PDM samples to PCM.
 */
typedef bool (*pdm_data_available_t)(MemsAudio_STM32L4SAIPDM *audio, pdm_sample_t *pdmSamples, size_t pdmLength);

/**
 * @brief Implementation details for the STM32 SAI PDM implementation.
 *
 */
/**
 * @brief Audio capture from a MEMS microphone on the STM32L4 using the SAI PDM interface.
 */
typedef struct MemsAudio_STM32L4SAIPDM_t {

    MemsAudio *audio;

    /**
     * @brief The last error that happened in a void function (e.g. HAL callback)
     */
    mems_audio_err_t lastError;

    /**
     * @brief The buffer to store PDM audio samples
     */
    pdm_sample_t *pdmBuffer;

    /**
     * @brief The length of the PDM buffer. Should be at least MEMS_AUDIO_PDM_BUFFER_LENGTH
     */
    size_t pdmBufferLength;

    /**
     * @brief Optional callback for when PDM data is available.
     */
    pdm_data_available_t pdm_data_available;

    /**
     * @brief A count of the number of PDM clients in use.
     */
    uint32_t SAI1_client;

    /**
     * @brief The SAI peripheral handle being used for SAI A subclock 1.
     */
    SAI_HandleTypeDef hSAI_BlockA1;

    /**
     * @brief The DMA handle to transfer SAI data from the peripheral to memory.
     */
    DMA_HandleTypeDef hdma_sai1_a;

    /**
     * @brief An instance of the PDM filter that performs decimation, and high and low pass filtering.
     * Unlike the DFSDM peripheral, the SAI PDM interface doesn't perform these operations in hardware.
     */
    TPDMFilter_InitStruct filter;

    /**
     * @brief The number of DMA transfers to ignore after starting recording.
     */
    volatile uint16_t discard_dma;

} MemsAudio_STM32L4SAIPDM;

/**
 * @brief Creates a MemsAudio instance that retrieves PDM samples from SAI A block 1 via the PDM interface,
 * decimates and filters these in software to produce the PCM output stream.
 *
 * @param audio
 * @param implementation
 * @return meems_audio_error_t
 */
mems_audio_err_t mems_audio_init_stm32l4_sai_pdm(MemsAudio *audio, MemsAudio_STM32L4SAIPDM *implementation);

/**
 * @brief Implementation-specific error codes.
 *
 */
typedef enum mems_audio_err_stm32l4_t {
    MEMS_AUDIO_ERROR_SAI_DMA_INIT = 1,
    MEMS_AUDIO_ERROR_SAI_CLOCK = 2,
    MEMS_AUDIO_ERROR_SAI_INIT = 3,
    MEMS_AUDIO_ERROR_SAI_DEINIT = 4,
    MEMS_AUDIO_ERROR_DMA_START = 5,
    MEMS_AUDIO_ERROR_DMA_STOP = 6,
    MEMS_AUDIO_ERROR_DMA_PAUSE = 7,
    MEMS_AUDIO_ERROR_DMA_RESUME = 8
} mems_audio_err_stm32l4_t;


#ifdef __cplusplus
}
#endif


#endif // _MEMS_AUDIO_LL_STM32L4_H_
