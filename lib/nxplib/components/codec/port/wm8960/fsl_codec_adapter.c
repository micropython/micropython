/*
 * Copyright  2019 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_wm8960.h"
#include "fsl_codec_adapter.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*! @brief module capability definition */
#define HAL_WM8960_MODULE_CAPABILITY                                                                               \
    (kCODEC_SupportModuleADC | kCODEC_SupportModuleDAC | kCODEC_SupportModulePGA | kCODEC_SupportModuleHeadphone | \
     kCODEC_SupportModuleLineout | kCODEC_SupportModuleSpeaker)

#define HAL_WM8960_PLAY_CAPABILITY                                                                       \
    (kCODEC_SupportPlayChannelLeft0 | kCODEC_SupportPlayChannelRight0 | kCODEC_SupportPlayChannelLeft1 | \
     kCODEC_SupportPlayChannelRight1 | kCODEC_SupportPlaySourcePGA | kCODEC_SupportPlaySourceDAC |       \
     kCODEC_SupportPlaySourceInput)

#define HAL_WM8960_RECORD_CAPABILITY                                                                    \
    (kCODEC_SupportPlayChannelLeft0 | kCODEC_SupportPlayChannelLeft1 | kCODEC_SupportPlayChannelLeft2 | \
     kCODEC_SupportPlayChannelRight0 | kCODEC_SupportPlayChannelRight1 | kCODEC_SupportPlayChannelRight2)

/*! @brief wm8960 map protocol */
#define HAL_WM8960_MAP_PROTOCOL(protocol)       \
    (protocol == kCODEC_BusI2S ?                \
         kWM8960_BusI2S :                       \
         protocol == kCODEC_BusLeftJustified ?  \
         kWM8960_BusLeftJustified :             \
         protocol == kCODEC_BusRightJustified ? \
         kWM8960_BusRightJustified :            \
         protocol == kCODEC_BusPCMA ? kWM8960_BusPCMA : protocol == kCODEC_BusPCMB ? kWM8960_BusPCMB : kWM8960_BusI2S)

/*! @brief wm8960 map module */
#define HAL_WM8960_MAP_MODULE(module)                        \
    (module == kCODEC_ModuleADC ?                            \
         kWM8960_ModuleADC :                                 \
         module == kCODEC_ModuleDAC ?                        \
         kWM8960_ModuleDAC :                                 \
         module == kCODEC_ModuleVref ?                       \
         kWM8960_ModuleVREF :                                \
         module == kCODEC_ModuleHeadphone ?                  \
         kWM8960_ModuleHP :                                  \
         module == kCODEC_ModuleMicbias ?                    \
         kWM8960_ModuleMICB :                                \
         module == kCODEC_ModuleMic ?                        \
         kWM8960_ModuleMIC :                                 \
         module == kCODEC_ModuleLinein ?                     \
         kWM8960_ModuleLineIn :                              \
         module == kCODEC_ModuleSpeaker ?                    \
         kWM8960_ModuleSpeaker :                             \
         module == kCODEC_ModuleMxier ? kWM8960_ModuleOMIX : \
                                        module == kCODEC_ModuleLineout ? kWM8960_ModuleLineOut : kWM8960_ModuleADC)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

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
status_t HAL_CODEC_Init(codec_handle_t *handle, void *config)
{
    assert((config != NULL) && (handle != NULL));
    assert(CODEC_HANDLE_SIZE >= (sizeof(codec_handle_t) + sizeof(wm8960_handle_t)) + HAL_I2C_MASTER_HANDLE_SIZE);

    codec_config_t *codecConfig = (codec_config_t *)config;

    wm8960_config_t *wm8960Config = (wm8960_config_t *)(codecConfig->codecDevConfig);
    wm8960_handle_t *wm8960Handle = (wm8960_handle_t *)((uint32_t) & (handle->codecDevHandle));

    /* load codec capability */
    handle->codecCapability.codecModuleCapability = HAL_WM8960_MODULE_CAPABILITY;
    /* add nop to aovid alignment fault, since that the compiler may generate 'strd' instruction to store 64 bit
    with one instruction, but the address may not word-aligned
    Will remove the __NOP in next release and use a word align address.
    */
    __NOP();
    handle->codecCapability.codecPlayCapability = HAL_WM8960_PLAY_CAPABILITY;
    /* codec device initialization */
    return WM8960_Init(wm8960Handle, wm8960Config);
}

/*!
 * brief Codec de-initilization.
 *
 * param handle codec handle.
 * return kStatus_Success is success, else de-initial failed.
 */
status_t HAL_CODEC_Deinit(codec_handle_t *handle)
{
    assert(handle != NULL);

    return WM8960_Deinit((wm8960_handle_t *)((uint32_t) & (handle->codecDevHandle)));
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
status_t HAL_CODEC_SetFormat(codec_handle_t *handle, uint32_t mclk, uint32_t sampleRate, uint32_t bitWidth)
{
    assert(handle != NULL);

    return WM8960_ConfigDataFormat((wm8960_handle_t *)((uint32_t) & (handle->codecDevHandle)), mclk, sampleRate,
                                   bitWidth);
}

/*!
 * brief set audio codec module volume.
 *
 * param handle codec handle.
 * param channel audio codec play channel, can be a value or combine value of _codec_play_channel.
 * param volume volume value, support 0 ~ 100, 0 is mute, 100 is the maximum volume value.
 * return kStatus_Success is success, else configure failed.
 */
status_t HAL_CODEC_SetVolume(codec_handle_t *handle, uint32_t playChannel, uint32_t volume)
{
    assert(handle != NULL);

    status_t retVal = kStatus_Success;

    if ((playChannel & kWM8960_HeadphoneLeft) || (playChannel & kWM8960_HeadphoneRight))
    {
        retVal = WM8960_SetVolume((wm8960_handle_t *)((uint32_t) & (handle->codecDevHandle)), kWM8960_ModuleHP, volume);
    }

    if ((playChannel & kWM8960_SpeakerLeft) || (playChannel & kWM8960_SpeakerRight))
    {
        retVal =
            WM8960_SetVolume((wm8960_handle_t *)((uint32_t) & (handle->codecDevHandle)), kWM8960_ModuleSpeaker, volume);
    }

    return retVal;
}

/*!
 * brief set audio codec module mute.
 *
 * param handle codec handle.
 * param channel audio codec play channel, can be a value or combine value of _codec_play_channel.
 * param isMute true is mute, false is unmute.
 * return kStatus_Success is success, else configure failed.
 */
status_t HAL_CODEC_SetMute(codec_handle_t *handle, uint32_t playChannel, bool isMute)
{
    assert(handle != NULL);

    status_t retVal = kStatus_Success;

    if ((playChannel & kWM8960_HeadphoneLeft) || (playChannel & kWM8960_HeadphoneRight))
    {
        retVal = WM8960_SetMute((wm8960_handle_t *)((uint32_t) & (handle->codecDevHandle)), kWM8960_ModuleHP, isMute);
    }

    if ((playChannel & kWM8960_SpeakerLeft) || (playChannel & kWM8960_SpeakerRight))
    {
        retVal =
            WM8960_SetMute((wm8960_handle_t *)((uint32_t) & (handle->codecDevHandle)), kWM8960_ModuleSpeaker, isMute);
    }

    return retVal;
}

/*!
 * brief set audio codec module power.
 *
 * param handle codec handle.
 * param module audio codec module.
 * param powerOn true is power on, false is power down.
 * return kStatus_Success is success, else configure failed.
 */
status_t HAL_CODEC_SetPower(codec_handle_t *handle, codec_module_t module, bool powerOn)
{
    assert(handle != NULL);

    return WM8960_SetModule((wm8960_handle_t *)((uint32_t) & (handle->codecDevHandle)), HAL_WM8960_MAP_MODULE(module),
                            powerOn);
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
status_t HAL_CODEC_SetRecordChannel(codec_handle_t *handle, uint32_t leftRecordChannel, uint32_t rightRecordChannel)
{
    return kStatus_CODEC_NotSupport;
}

/*!
 * brief codec set record source.
 *
 * param handle codec handle.
 * param source audio codec record source, can be a value or combine value of _codec_record_source.
 *
 * @return kStatus_Success is success, else configure failed.
 */
status_t HAL_CODEC_SetRecord(codec_handle_t *handle, uint32_t recordSource)
{
    return kStatus_CODEC_NotSupport;
}

/*!
 * brief codec set play source.
 *
 * param handle codec handle.
 * param playSource audio codec play source, can be a value or combine value of _codec_play_source.
 *
 * return kStatus_Success is success, else configure failed.
 */
status_t HAL_CODEC_SetPlay(codec_handle_t *handle, uint32_t playSource)
{
    assert(handle != NULL);

    return WM8960_SetPlay((wm8960_handle_t *)((uint32_t) & (handle->codecDevHandle)), playSource);
}

/*!
 * brief codec module control.
 *
 * param handle codec handle.
 * param cmd module control cmd, reference _codec_module_ctrl_cmd.
 * param data value to write, when cmd is kCODEC_ModuleRecordSourceChannel, the data should be a value combine
 *  of channel and source, please reference macro CODEC_MODULE_RECORD_SOURCE_CHANNEL(source, LP, LN, RP, RN), reference
 *  codec specific driver for detail configurations.
 * return kStatus_Success is success, else configure failed.
 */
status_t HAL_CODEC_ModuleControl(codec_handle_t *handle, codec_module_ctrl_cmd_t cmd, uint32_t data)
{
    return kStatus_CODEC_NotSupport;
}
