/*
 * Copyright  2017-2019 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_codec_common.h"
#include "fsl_codec_adapter.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*! @brief codec play and record capability */
#define GET_PLAY_CHANNEL_CAPABILITY(capability) (capability & 0xFFU)
#define GET_PLAY_SOURCE_CAPABILITY(capability) (capability >> 8U)
#define GET_RECORD_SOURCE_CAPABILITY(capability) (capability & 0x3FU)
#define GET_RECORD_CHANNEL_CAPABILITY(capability) (capability >> 6U)
/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * brief Codec initilization.
 *
 * param handle codec handle.
 * param config codec configuration.
 * return kStatus_Success is success, else initial failed.
 */
status_t CODEC_Init(codec_handle_t *handle, codec_config_t *config)
{
    assert((config != NULL) && (handle != NULL));

    /* Set the handle information */
    handle->codecConfig = config;

    return HAL_CODEC_Init(handle, config);
}

/*!
 * brief Codec de-initilization.
 *
 * param handle codec handle.
 * return kStatus_Success is success, else de-initial failed.
 */
status_t CODEC_Deinit(codec_handle_t *handle)
{
    assert((handle != NULL) && (handle->codecConfig != NULL));

    return HAL_CODEC_Deinit(handle);
}

/*!
 * brief set audio data format.
 *
 * param handle codec handle.
 * param mclk master clock frequency in HZ.
 * param sampleRate sample rate in HZ.
 * param bitWidth bit width.
 * return kStatus_Success is success, else configure failed.
 */
status_t CODEC_SetFormat(codec_handle_t *handle, uint32_t mclk, uint32_t sampleRate, uint32_t bitWidth)
{
    assert((handle != NULL) && (handle->codecConfig != NULL));

    return HAL_CODEC_SetFormat(handle, mclk, sampleRate, bitWidth);
}

/*!
 * brief codec module control.
 *
 * This function is used for codec module control, support switch digital interface cmd, can be expand to support codec
 * module specific feature
 *
 * param handle codec handle.
 * param cmd module control cmd, reference _codec_module_ctrl_cmd.
 * param data value to write, when cmd is kCODEC_ModuleRecordSourceChannel, the data should be a value combine
 *  of channel and source, please reference macro CODEC_MODULE_RECORD_SOURCE_CHANNEL(source, LP, LN, RP, RN), reference
 *  codec specific driver for detail configurations.
 * return kStatus_Success is success, else configure failed.
 */
status_t CODEC_ModuleControl(codec_handle_t *handle, codec_module_ctrl_cmd_t cmd, uint32_t data)
{
    assert((handle != NULL) && (handle->codecConfig != NULL));

    switch (cmd)
    {
        case kCODEC_ModuleSwitchI2SInInterface:
            if ((handle->codecCapability.codecModuleCapability & kCODEC_SupportModuleI2SInSwitchInterface) == 0U)
            {
                return kStatus_CODEC_NotSupport;
            }
            break;

        default:
            return kStatus_CODEC_NotSupport;
    }

    return HAL_CODEC_ModuleControl(handle, cmd, data);
}

/*!
 * brief set audio codec module volume.
 *
 * param handle codec handle.
 * param channel audio codec play channel, can be a value or combine value of _codec_play_channel.
 * param volume volume value, support 0 ~ 100, 0 is mute, 100 is the maximum volume value.
 * return kStatus_Success is success, else configure failed.
 */
status_t CODEC_SetVolume(codec_handle_t *handle, uint32_t playChannel, uint32_t volume)
{
    assert((handle != NULL) && (handle->codecConfig != NULL));
    assert(volume <= CODEC_VOLUME_MAX_VALUE);

    /* check capability of set volume */
    if ((GET_PLAY_CHANNEL_CAPABILITY(handle->codecCapability.codecPlayCapability) & playChannel) == 0U)
    {
        return kStatus_CODEC_NotSupport;
    }

    return HAL_CODEC_SetVolume(handle, playChannel, volume);
}

/*!
 * brief set audio codec module mute.
 *
 * param handle codec handle.
 * param channel audio codec play channel, can be a value or combine value of _codec_play_channel.
 * param mute true is mute, false is unmute.
 * return kStatus_Success is success, else configure failed.
 */
status_t CODEC_SetMute(codec_handle_t *handle, uint32_t playChannel, bool mute)
{
    assert((handle != NULL) && (handle->codecConfig != NULL));

    /* check capability of mute */
    if ((GET_PLAY_CHANNEL_CAPABILITY(handle->codecCapability.codecPlayCapability) & playChannel) == 0U)
    {
        return kStatus_CODEC_NotSupport;
    }

    return HAL_CODEC_SetMute(handle, playChannel, mute);
}

/*!
 * brief set audio codec module power.
 *
 * param handle codec handle.
 * param module audio codec module.
 * param powerOn true is power on, false is power down.
 * return kStatus_Success is success, else configure failed.
 */
status_t CODEC_SetPower(codec_handle_t *handle, codec_module_t module, bool powerOn)
{
    assert((handle != NULL) && (handle->codecConfig != NULL));

    /* check capability of power switch */
    if ((handle->codecCapability.codecModuleCapability & (1U << module)) == 0U)
    {
        return kStatus_CODEC_NotSupport;
    }

    return HAL_CODEC_SetPower(handle, module, powerOn);
}

/*!
 * brief codec set record source.
 *
 * param handle codec handle.
 * param source audio codec record source, can be a value or combine value of _codec_record_source.
 *
 * return kStatus_Success is success, else configure failed.
 */
status_t CODEC_SetRecord(codec_handle_t *handle, uint32_t recordSource)
{
    assert((handle != NULL) && (handle->codecConfig != NULL));

    /* check capability of record capability */
    if ((GET_RECORD_SOURCE_CAPABILITY(handle->codecCapability.codecRecordCapability) & recordSource) == 0U)
    {
        return kStatus_CODEC_NotSupport;
    }

    return HAL_CODEC_SetRecord(handle, recordSource);
}

/*!
 * brief codec set record channel.
 *
 * param handle codec handle.
 * param leftRecordChannel audio codec record channel, reference _codec_record_channel, can be a value or combine value
 of member in _codec_record_channel.
 * param rightRecordChannel audio codec record channel, reference _codec_record_channel, can be a value combine of
 member in _codec_record_channel.

 * return kStatus_Success is success, else configure failed.
 */
status_t CODEC_SetRecordChannel(codec_handle_t *handle, uint32_t leftRecordChannel, uint32_t rightRecordChannel)
{
    assert((handle != NULL) && (handle->codecConfig != NULL));

    /* check capability of record capability */
    if ((GET_RECORD_CHANNEL_CAPABILITY(handle->codecCapability.codecRecordCapability) & leftRecordChannel) == 0U)
    {
        return kStatus_CODEC_NotSupport;
    }

    if ((GET_RECORD_CHANNEL_CAPABILITY(handle->codecCapability.codecRecordCapability) & rightRecordChannel) == 0U)
    {
        return kStatus_CODEC_NotSupport;
    }

    return HAL_CODEC_SetRecordChannel(handle, leftRecordChannel, rightRecordChannel);
}

/*!
 * brief codec set play source.
 *
 * param handle codec handle.
 * param playSource audio codec play source, can be a value or combine value of _codec_play_source.
 *
 * return kStatus_Success is success, else configure failed.
 */
status_t CODEC_SetPlay(codec_handle_t *handle, uint32_t playSource)
{
    assert((handle != NULL) && (handle->codecConfig != NULL));

    /* check capability of record capability */
    if ((GET_PLAY_SOURCE_CAPABILITY(handle->codecCapability.codecPlayCapability) & playSource) == 0U)
    {
        return kStatus_CODEC_NotSupport;
    }

    return HAL_CODEC_SetPlay(handle, playSource);
}
