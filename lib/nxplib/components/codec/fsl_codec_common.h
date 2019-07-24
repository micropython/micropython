/*
 * Copyright  2017- 2019 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_CODEC_COMMON_H_
#define _FSL_CODEC_COMMON_H_

#include "fsl_common.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*! @name Driver version */
/*@{*/
/*! @brief CLOCK driver version 2.1.0. */
#define FSL_CODEC_DRIVER_VERSION (MAKE_VERSION(2, 1, 0))
/*@}*/

/*! @brief CODEC handle buffer size */
#ifndef CODEC_HANDLE_SIZE
#define CODEC_HANDLE_SIZE (128U)
#endif

/*! @brief codec maximum volume range */
#define CODEC_VOLUME_MAX_VALUE (100U)

/*! @brief CODEC status */
enum _codec_status
{
    kStatus_CODEC_NotSupport          = MAKE_STATUS(kStatusGroup_CODEC, 0U), /*!< CODEC not support status */
    kStatus_CODEC_DeviceNotRegistered = MAKE_STATUS(kStatusGroup_CODEC, 1U), /*!< CODEC device register failed status */
    kStatus_CODEC_I2CBusInitialFailed =
        MAKE_STATUS(kStatusGroup_CODEC, 2U), /*!< CODEC i2c bus initialization failed status */
    kStatus_CODEC_I2CCommandTransferFailed =
        MAKE_STATUS(kStatusGroup_CODEC, 3U), /*!< CODEC i2c bus command transfer failed status */
};

/*! @brief AUDIO format definition. */
typedef enum _codec_audio_protocol
{
    kCODEC_BusI2S            = 0U, /*!< I2S type */
    kCODEC_BusLeftJustified  = 1U, /*!< Left justified mode */
    kCODEC_BusRightJustified = 2U, /*!< Right justified mode */
    kCODEC_BusPCMA           = 3U, /*!< DSP/PCM A mode */
    kCODEC_BusPCMB           = 4U, /*!< DSP/PCM B mode */
    kCODEC_BusTDM            = 5U, /*!< TDM mode */
} codec_audio_protocol_t;

/*! @brief audio sample rate definition */
enum _codec_audio_sample_rate
{
    kCODEC_AudioSampleRate8KHz    = 8000U,   /*!< Sample rate 8000 Hz */
    kCODEC_AudioSampleRate11025Hz = 11025U,  /*!< Sample rate 11025 Hz */
    kCODEC_AudioSampleRate12KHz   = 12000U,  /*!< Sample rate 12000 Hz */
    kCODEC_AudioSampleRate16KHz   = 16000U,  /*!< Sample rate 16000 Hz */
    kCODEC_AudioSampleRate22050Hz = 22050U,  /*!< Sample rate 22050 Hz */
    kCODEC_AudioSampleRate24KHz   = 24000U,  /*!< Sample rate 24000 Hz */
    kCODEC_AudioSampleRate32KHz   = 32000U,  /*!< Sample rate 32000 Hz */
    kCODEC_AudioSampleRate44100Hz = 44100U,  /*!< Sample rate 44100 Hz */
    kCODEC_AudioSampleRate48KHz   = 48000U,  /*!< Sample rate 48000 Hz */
    kCODEC_AudioSampleRate96KHz   = 96000U,  /*!< Sample rate 96000 Hz */
    kCODEC_AudioSampleRate192KHz  = 192000U, /*!< Sample rate 192000 Hz */
    kCODEC_AudioSampleRate384KHz  = 384000U, /*!< Sample rate 384000 Hz */
};

/*! @brief audio bit width */
enum _codec_audio_bit_width
{
    kCODEC_AudioBitWidth16bit = 16U, /*!< audio bit width 16 */
    kCODEC_AudioBitWidth20bit = 20U, /*!< audio bit width 20 */
    kCODEC_AudioBitWidth24bit = 24U, /*!< audio bit width 24 */
    kCODEC_AudioBitWidth32bit = 32U, /*!< audio bit width 32 */
};

/*! @brief audio codec module*/
typedef enum _codec_module
{
    kCODEC_ModuleADC       = 0U,  /*!< codec module ADC */
    kCODEC_ModuleDAC       = 1U,  /*!< codec module DAC */
    kCODEC_ModulePGA       = 2U,  /*!< codec module PGA */
    kCODEC_ModuleHeadphone = 3U,  /*!< codec module headphone */
    kCODEC_ModuleSpeaker   = 4U,  /*!< codec module speaker */
    kCODEC_ModuleLinein    = 5U,  /*!< codec module linein */
    kCODEC_ModuleLineout   = 6U,  /*!< codec module lineout */
    kCODEC_ModuleVref      = 7U,  /*!< codec module VREF */
    kCODEC_ModuleMicbias   = 8U,  /*!< codec module MIC BIAS */
    kCODEC_ModuleMic       = 9U,  /*!< codec module MIC */
    kCODEC_ModuleI2SIn     = 10U, /*!< codec module I2S in */
    kCODEC_ModuleI2SOut    = 11U, /*!< codec module I2S out */
    kCODEC_ModuleMxier     = 12U, /*!< codec module mixer */
} codec_module_t;

/*! @brief audio codec module control cmd */
typedef enum _codec_module_ctrl_cmd
{
    kCODEC_ModuleSwitchI2SInInterface = 0U, /*!< module digital interface siwtch. */
} codec_module_ctrl_cmd_t;

/*! @brief audio codec module digital interface */
enum _codec_module_ctrl_i2s_in_interface
{
    kCODEC_ModuleI2SInInterfacePCM = 0U, /*!< Pcm interface*/
    kCODEC_ModuleI2SInInterfaceDSD = 1U, /*!< DSD interface */
};

/*! @brief audio codec module record source value */
enum _codec_record_source
{
    kCODEC_RecordSourceDifferentialLine = 1U,  /*!< record source from differential line */
    kCODEC_RecordSourceLineInput        = 2U,  /*!< record source from line input */
    kCODEC_RecordSourceDifferentialMic  = 4U,  /*!< record source from differential mic */
    kCODEC_RecordSourceDigitalMic       = 8U,  /*!< record source from digital microphone */
    kCODEC_RecordSourceSingleEndMic     = 16U, /*!< record source from single microphone */
};

/*! @brief audio codec record channel */
enum _codec_reocrd_channel
{
    kCODEC_RecordChannelLeft1                 = 1U,  /*!< left record channel 1 */
    kCODEC_RecordChannelLeft2                 = 2U,  /*!< left record channel 2 */
    kCODEC_RecordChannelLeft3                 = 4U,  /*!< left record channel 3 */
    kCODEC_RecordChannelRight1                = 1U,  /*!< right record channel 1 */
    kCODEC_RecordChannelRight2                = 2U,  /*!< right record channel 2 */
    kCODEC_RecordChannelRight3                = 4U,  /*!< right record channel 3 */
    kCODEC_RecordChannelDifferentialPositive1 = 1U,  /*!< differential positive record channel 1 */
    kCODEC_RecordChannelDifferentialPositive2 = 2U,  /*!< differential positive record channel 2 */
    kCODEC_RecordChannelDifferentialPositive3 = 4U,  /*!< differential positive record channel 3 */
    kCODEC_RecordChannelDifferentialNegative1 = 8U,  /*!< differential negative record channel 1 */
    kCODEC_RecordChannelDifferentialNegative2 = 16U, /*!< differential negative record channel 2 */
    kCODEC_RecordChannelDifferentialNegative3 = 32U, /*!< differential negative record channel 3 */
};

/*! @brief audio codec module play source value */
enum _codec_play_source
{
    kCODEC_PlaySourcePGA          = 1U, /*!< play source PGA, bypass ADC */
    kCODEC_PlaySourceInput        = 2U, /*!< play source Input3 */
    kCODEC_PlaySourceDAC          = 4U, /*!< play source DAC */
    kCODEC_PlaySourceMixerIn      = 1U, /*!< play source mixer in */
    kCODEC_PlaySourceMixerInLeft  = 2U, /*!< play source mixer in left */
    kCODEC_PlaySourceMixerInRight = 4U, /*!< play source mixer in right */
    kCODEC_PlaySourceAux          = 8U, /*!< play source mixer in AUx */
};

/*! @brief codec play channel */
enum _codec_play_channel
{
    kCODEC_PlayChannelHeadphoneLeft  = 1U,  /*!< play channel headphone left */
    kCODEC_PlayChannelHeadphoneRight = 2U,  /*!< play channel headphone right */
    kCODEC_PlayChannelSpeakerLeft    = 4U,  /*!< play channel speaker left */
    kCODEC_PlayChannelSpeakerRight   = 8U,  /*!< play channel speaker right */
    kCODEC_PlayChannelLineOutLeft    = 16U, /*!< play channel lineout left */
    kCODEC_PlayChannelLineOutRight   = 32U, /*!< play channel lineout right */

    kCODEC_PlayChannelLeft0  = 1U,   /*!< play channel left0 */
    kCODEC_PlayChannelRight0 = 2U,   /*!< play channel right0 */
    kCODEC_PlayChannelLeft1  = 4U,   /*!< play channel left1 */
    kCODEC_PlayChannelRight1 = 8U,   /*!< play channel right1 */
    kCODEC_PlayChannelLeft2  = 16U,  /*!< play channel left2 */
    kCODEC_PlayChannelRight2 = 32U,  /*!< play channel right2 */
    kCODEC_PlayChannelLeft3  = 64U,  /*!< play channel left3 */
    kCODEC_PlayChannelRight3 = 128U, /*!< play channel right3 */
};

/*! @brief audio codec capability */
enum _codec_capability_flag
{
    kCODEC_SupportModuleADC                  = 1U << 0U,  /*!< codec capability of module ADC */
    kCODEC_SupportModuleDAC                  = 1U << 1U,  /*!< codec capability of module DAC */
    kCODEC_SupportModulePGA                  = 1U << 2U,  /*!< codec capability of module PGA */
    kCODEC_SupportModuleHeadphone            = 1U << 3U,  /*!< codec capability of module headphone */
    kCODEC_SupportModuleSpeaker              = 1U << 4U,  /*!< codec capability of module speaker */
    kCODEC_SupportModuleLinein               = 1U << 5U,  /*!< codec capability of module linein */
    kCODEC_SupportModuleLineout              = 1U << 6U,  /*!< codec capability of module lineout */
    kCODEC_SupportModuleVref                 = 1U << 7U,  /*!< codec capability of module vref */
    kCODEC_SupportModuleMicbias              = 1U << 8U,  /*!< codec capability of module mic bias */
    kCODEC_SupportModuleMic                  = 1U << 9U,  /*!< codec capability of module mic bias */
    kCODEC_SupportModuleI2SIn                = 1U << 10U, /*!< codec capability of module I2S in */
    kCODEC_SupportModuleI2SOut               = 1U << 11U, /*!< codec capability of module I2S out */
    kCODEC_SupportModuleMixer                = 1U << 12U, /*!< codec capability of module mixer */
    kCODEC_SupportModuleI2SInSwitchInterface = 1U << 13U, /*!< codec capability of module I2S in switch interface */

    kCODEC_SupportPlayChannelLeft0  = 1U << 0U, /*!< codec capability of play channel left 0 */
    kCODEC_SupportPlayChannelRight0 = 1U << 1U, /*!< codec capability of play channel right 0 */
    kCODEC_SupportPlayChannelLeft1  = 1U << 2U, /*!< codec capability of play channel left 1 */
    kCODEC_SupportPlayChannelRight1 = 1U << 3U, /*!< codec capability of play channel right 1 */
    kCODEC_SupportPlayChannelLeft2  = 1U << 4U, /*!< codec capability of play channel left 2 */
    kCODEC_SupportPlayChannelRight2 = 1U << 5U, /*!< codec capability of play channel right 2 */
    kCODEC_SupportPlayChannelLeft3  = 1U << 6U, /*!< codec capability of play channel left 3 */
    kCODEC_SupportPlayChannelRight3 = 1U << 7U, /*!< codec capability of play channel right 3 */

    kCODEC_SupportPlaySourcePGA          = 1U << 8U,  /*!< codec capability of set playback source PGA */
    kCODEC_SupportPlaySourceInput        = 1U << 9U,  /*!< codec capability of set playback source INPUT */
    kCODEC_SupportPlaySourceDAC          = 1U << 10U, /*!< codec capability of set playback source DAC */
    kCODEC_SupportPlaySourceMixerIn      = 1U << 11U, /*!< codec capability of set play source Mixer in */
    kCODEC_SupportPlaySourceMixerInLeft  = 1U << 12U, /*!< codec capability of set play source Mixer in left */
    kCODEC_SupportPlaySourceMixerInRight = 1U << 13U, /*!< codec capability of set play source Mixer in right */
    kCODEC_SupportPlaySourceAux          = 1U << 14U, /*!< codec capability of set play source aux */

    kCODEC_SupportRecordSourceDifferentialLine = 1U << 0U,  /*!< codec capability of record source differential line */
    kCODEC_SupportRecordSourceLineInput        = 1U << 1U,  /*!< codec capability of record source line input */
    kCODEC_SupportRecordSourceDifferentialMic  = 1U << 2U,  /*!< codec capability of record source differential mic */
    kCODEC_SupportRecordSourceDigitalMic       = 1U << 3U,  /*!< codec capability of record digital mic */
    kCODEC_SupportRecordSourceSingleEndMic     = 1U << 4U,  /*!< codec capability of single end mic */
    kCODEC_SupportRecordChannelLeft1           = 1U << 6U,  /*!< left record channel 1 */
    kCODEC_SupportRecordChannelLeft2           = 1U << 7U,  /*!< left record channel 2 */
    kCODEC_SupportRecordChannelLeft3           = 1U << 8U,  /*!< left record channel 3 */
    kCODEC_SupportRecordChannelRight1          = 1U << 9U,  /*!< right record channel 1 */
    kCODEC_SupportRecordChannelRight2          = 1U << 10U, /*!< right record channel 2 */
    kCODEC_SupportRecordChannelRight3          = 1U << 11U, /*!< right record channel 3 */
};

/*!@brief codec handle declaration */
typedef struct codec_handle codec_handle_t;

/*! @brief Initialize structure of the codec */
typedef struct _codec_config
{
    uint32_t codecDevType; /*!< codec type */
    void *codecDevConfig;  /*!< Codec device specific configuration */
} codec_config_t;

/*! @brief codec capability */
typedef struct _codec_capability
{
    uint32_t codecModuleCapability; /*!< codec module capability */
    uint32_t codecPlayCapability;   /*!< codec play capability */
    uint32_t codecRecordCapability; /*!< codec record capability */
} codec_capability_t;

/*! @brief Codec handle definition.
 * * Application should allocate a buffer with CODEC_HANDLE_SIZE for handle definition, such as
 * uint8_t codecHandleBuffer[CODEC_HANDLE_SIZE];
 * codec_handle_t *codecHandle = codecHandleBuffer;
 */
struct codec_handle
{
    codec_config_t *codecConfig;        /*!< codec configuration function pointer */
    codec_capability_t codecCapability; /*!< codec capability */
    void *codecDevHandle;               /*!< codec device handle */
};

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif
/*!
 * @brief Codec initilization.
 *
 * @param handle codec handle.
 * @param config codec configurations.
 * @return kStatus_Success is success, else de-initial failed.
 */
status_t CODEC_Init(codec_handle_t *handle, codec_config_t *config);

/*!
 * @brief Codec de-initilization.
 *
 * @param handle codec handle.
 * @return kStatus_Success is success, else de-initial failed.
 */
status_t CODEC_Deinit(codec_handle_t *handle);

/*!
 * @brief set audio data format.
 *
 * @param handle codec handle.
 * @param mclk master clock frequency in HZ.
 * @param sampleRate sample rate in HZ.
 * @param bitWidth bit width.
 * @return kStatus_Success is success, else configure failed.
 */
status_t CODEC_SetFormat(codec_handle_t *handle, uint32_t mclk, uint32_t sampleRate, uint32_t bitWidth);

/*!
 * @brief codec module control.
 *
 * This function is used for codec module control, support switch digital interface cmd, can be expand to support codec
 * module specific feature.
 *
 * @param handle codec handle.
 * @param cmd module control cmd, reference _codec_module_ctrl_cmd.
 * @param data value to write, when cmd is kCODEC_ModuleRecordSourceChannel, the data should be a value combine
 *  of channel and source, please reference macro CODEC_MODULE_RECORD_SOURCE_CHANNEL(source, LP, LN, RP, RN), reference
 *  codec specific driver for detail configurations.
 * @return kStatus_Success is success, else configure failed.
 */
status_t CODEC_ModuleControl(codec_handle_t *handle, codec_module_ctrl_cmd_t cmd, uint32_t data);

/*!
 * @brief set audio codec pl volume.
 *
 * @param handle codec handle.
 * @param channel audio codec play channel, can be a value or combine value of _codec_play_channel.
 * @param volume volume value, support 0 ~ 100, 0 is mute, 100 is the maximum volume value.
 * @return kStatus_Success is success, else configure failed.
 */
status_t CODEC_SetVolume(codec_handle_t *handle, uint32_t channel, uint32_t volume);

/*!
 * @brief set audio codec module mute.
 *
 * @param handle codec handle.
 * @param channel audio codec play channel, can be a value or combine value of _codec_play_channel.
 * @param mute true is mute, false is unmute.
 * @return kStatus_Success is success, else configure failed.
 */
status_t CODEC_SetMute(codec_handle_t *handle, uint32_t channel, bool mute);

/*!
 * @brief set audio codec power.
 *
 * @param handle codec handle.
 * @param module audio codec module.
 * @param powerOn true is power on, false is power down.
 * @return kStatus_Success is success, else configure failed.
 */
status_t CODEC_SetPower(codec_handle_t *handle, codec_module_t module, bool powerOn);

/*!
 * @brief codec set record source.
 *
 * @param handle codec handle.
 * @param source audio codec record source, can be a value or combine value of _codec_record_source.
 *
 * @return kStatus_Success is success, else configure failed.
 */
status_t CODEC_SetRecord(codec_handle_t *handle, uint32_t recordRource);

/*!
 * @brief codec set record channel.
 *
 * @param handle codec handle.
 * @param leftRecordChannel audio codec record channel, reference _codec_record_channel, can be a value combine of
 member in _codec_record_channel.
 * @param rightRecordChannel audio codec record channel, reference _codec_record_channel, can be a value combine of
 member in _codec_record_channel.

 * @return kStatus_Success is success, else configure failed.
 */
status_t CODEC_SetRecordChannel(codec_handle_t *handle, uint32_t leftRecordChannel, uint32_t rightRecordChannel);

/*!
 * @brief codec set play source.
 *
 * @param handle codec handle.
 * @param playSource audio codec play source, can be a value or combine value of _codec_play_source.
 *
 * @return kStatus_Success is success, else configure failed.
 */
status_t CODEC_SetPlay(codec_handle_t *handle, uint32_t playSource);

#if defined(__cplusplus)
}
#endif

#endif /* _FSL_CODEC_COMMON_H_ */
