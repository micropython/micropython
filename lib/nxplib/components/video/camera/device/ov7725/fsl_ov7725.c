/*
 * Copyright  2017-2018 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_video_common.h"
#include "fsl_camera.h"
#include "fsl_camera_device.h"
#include "fsl_ov7725.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define OV7725_SCCB_ADDR 0x21U
#define OV7725_REVISION 0x7721U

#define OV7725_WriteReg(handle, reg, val)                            \
    SCCB_WriteReg(OV7725_SCCB_ADDR, kSCCB_RegAddr8Bit, (reg), (val), \
                  ((ov7725_resource_t *)(handle->resource))->i2cSendFunc)

#define OV7725_ReadReg(handle, reg, val)                            \
    SCCB_ReadReg(OV7725_SCCB_ADDR, kSCCB_RegAddr8Bit, (reg), (val), \
                 ((ov7725_resource_t *)(handle->resource))->i2cReceiveFunc)

#define OV7725_ModifyReg(handle, reg, clrMask, val)                              \
    SCCB_ModifyReg(OV7725_SCCB_ADDR, kSCCB_RegAddr8Bit, (reg), (clrMask), (val), \
                   ((ov7725_resource_t *)(handle->resource))->i2cReceiveFunc,    \
                   ((ov7725_resource_t *)(handle->resource))->i2cSendFunc)

typedef struct _ov7725_clock_config
{
    uint32_t frameRate_Hz;
    uint32_t inputClk_Hz;
    uint8_t clkrc;  /*!< Register CLKRC. */
    uint8_t com4;   /*!< Register COM4. */
    uint8_t dm_lnl; /*!< Register DM_LNL. */
} ov7725_clock_config_t;

typedef struct _ov7725_light_mode
{
    uint8_t lightMode;
    uint8_t com8;
    uint8_t blue;
    uint8_t red;
    uint8_t com5;
} ov7725_light_mode_config_t;

typedef struct _ov7725_special_effect_config
{
    uint8_t effect;
    uint8_t sde;
    uint8_t ufix;
    uint8_t vfix;
} ov7725_special_effect_config_t;

typedef struct _ov7725_night_mode
{
    uint8_t nightMode;
    uint8_t com5;
} ov7725_night_mode_t;

typedef struct _ov7725_pixel_format_config
{
    video_pixel_format_t fmt;
    uint8_t com7;
} ov7725_pixel_format_config_t;

typedef status_t (*ov7725_cmd_func_t)(camera_device_handle_t *handle, int32_t arg);

typedef struct _ov7725_cmd_func_map
{
    camera_device_cmd_t cmd;
    ov7725_cmd_func_t func;
} ov7725_cmd_func_map_t;

typedef struct _ov7725_reg
{
    uint8_t reg;
    uint8_t val;
} ov7725_reg_t;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
status_t OV7725_Init(camera_device_handle_t *handle, const camera_config_t *config);

status_t OV7725_InitExt(camera_device_handle_t *handle, const camera_config_t *config, const void *specialConfig);

status_t OV7725_Deinit(camera_device_handle_t *handle);

status_t OV7725_Control(camera_device_handle_t *handle, camera_device_cmd_t cmd, int32_t arg);

status_t OV7725_Start(camera_device_handle_t *handle);

status_t OV7725_Stop(camera_device_handle_t *handle);

status_t OV7725_SetSpecialEffect(camera_device_handle_t *handle, int32_t effect);

status_t OV7725_SetLightMode(camera_device_handle_t *handle, int32_t lightMode);

status_t OV7725_SetNightMode(camera_device_handle_t *handle, int32_t nightMode);

status_t OV7725_SetSaturation(camera_device_handle_t *handle, int32_t saturation);

status_t OV7725_SetContrast(camera_device_handle_t *handle, int32_t contrast);

status_t OV7725_SetBrightness(camera_device_handle_t *handle, int32_t brightness);

/*******************************************************************************
 * Variables
 ******************************************************************************/
const camera_device_operations_t ov7725_ops = {
    .init     = OV7725_Init,
    .deinit   = OV7725_Deinit,
    .start    = OV7725_Start,
    .stop     = OV7725_Stop,
    .control  = OV7725_Control,
    .init_ext = OV7725_InitExt,
};

static const ov7725_clock_config_t ov7725ClockConfigs[] = {
    /* FrameRate, inputClk, clkrc, com4, dm_lnl. */
    {.frameRate_Hz = 30, .inputClk_Hz = 24000000, .clkrc = 0x01, .com4 = 0x41, .dm_lnl = 0x00},
    {.frameRate_Hz = 15, .inputClk_Hz = 24000000, .clkrc = 0x03, .com4 = 0x41, .dm_lnl = 0x00},
    {.frameRate_Hz = 25, .inputClk_Hz = 24000000, .clkrc = 0x01, .com4 = 0x41, .dm_lnl = 0x66},
    {.frameRate_Hz = 14, .inputClk_Hz = 24000000, .clkrc = 0x03, .com4 = 0x41, .dm_lnl = 0x1a},
    {.frameRate_Hz = 30, .inputClk_Hz = 26000000, .clkrc = 0x01, .com4 = 0x41, .dm_lnl = 0x2b},
    {.frameRate_Hz = 15, .inputClk_Hz = 26000000, .clkrc = 0x03, .com4 = 0x41, .dm_lnl = 0x2b},
    {.frameRate_Hz = 25, .inputClk_Hz = 26000000, .clkrc = 0x01, .com4 = 0x41, .dm_lnl = 0x99},
    {.frameRate_Hz = 14, .inputClk_Hz = 26000000, .clkrc = 0x03, .com4 = 0x41, .dm_lnl = 0x46},
    {.frameRate_Hz = 30, .inputClk_Hz = 13000000, .clkrc = 0x00, .com4 = 0x41, .dm_lnl = 0x2b},
    {.frameRate_Hz = 15, .inputClk_Hz = 13000000, .clkrc = 0x01, .com4 = 0x41, .dm_lnl = 0x2b},
    {.frameRate_Hz = 25, .inputClk_Hz = 13000000, .clkrc = 0x00, .com4 = 0x41, .dm_lnl = 0x99},
    {.frameRate_Hz = 14, .inputClk_Hz = 13000000, .clkrc = 0x01, .com4 = 0x41, .dm_lnl = 0x46},
};

static const ov7725_special_effect_config_t ov7725SpecialEffectConfigs[] = {
    /* Normal. */
    {.effect = CAMERA_SPECIAL_EFFECT_NORMAL, .sde = 0x06, .ufix = 0x80, .vfix = 0x80},
    /* B & W */
    {.effect = CAMERA_SPECIAL_EFFECT_BW, .sde = 0x26, .ufix = 0x80, .vfix = 0x80},
    /* Sepia. */
    {.effect = CAMERA_SPECIAL_EFFECT_SEPIA, .sde = 0x1e, .ufix = 0x40, .vfix = 0xa0},
    /* Bluish. */
    {.effect = CAMERA_SPECIAL_EFFECT_BLUISH, .sde = 0x1e, .ufix = 0xa0, .vfix = 0x40},
    /* Redish. */
    {.effect = CAMERA_SPECIAL_EFFECT_REDISH, .sde = 0x1e, .ufix = 0x80, .vfix = 0x40},
    /* Greenish. */
    {.effect = CAMERA_SPECIAL_EFFECT_GREENISH, .sde = 0x1e, .ufix = 0x60, .vfix = 0x60},
    /* Negtive. */
    {.effect = CAMERA_SPECIAL_EFFECT_NEGTIVE, .sde = 0x46, .ufix = 0x00, .vfix = 0x00},
};

static const ov7725_light_mode_config_t ov7725LightModeConfigs[] = {
    /* Auto. */
    {.lightMode = CAMERA_LIGHT_MODE_AUTO, .com8 = 0xff, .blue = 0x80, .red = 0x80, .com5 = 0x65},
    /* Sunny. */
    {.lightMode = CAMERA_LIGHT_MODE_SUNNY, .com8 = 0xfd, .blue = 0x5a, .red = 0x5c, .com5 = 0x65},
    /* Cloudy. */
    {.lightMode = CAMERA_LIGHT_MODE_CLOUDY, .com8 = 0xfd, .blue = 0x58, .red = 0x60, .com5 = 0x65},
    /* Office. */
    {.lightMode = CAMERA_LIGHT_MODE_OFFICE, .com8 = 0xfd, .blue = 0x84, .red = 0x4c, .com5 = 0x65},
    /* Home. */
    {.lightMode = CAMERA_LIGHT_MODE_HOME, .com8 = 0xfd, .blue = 0x96, .red = 0x40, .com5 = 0x65},
    /* Night. */
    {.lightMode = CAMERA_LIGHT_MODE_NIGHT, .com8 = 0xff, .blue = 0x80, .red = 0x80, .com5 = 0xe5}};

static const ov7725_pixel_format_config_t ov7725PixelFormatConfigs[] = {
    {.fmt = kVIDEO_PixelFormatYUYV, .com7 = (0)},
    {.fmt = kVIDEO_PixelFormatRGB565, .com7 = (1 << 2) | (2)},
    {.fmt = kVIDEO_PixelFormatXRGB1555, .com7 = (2 << 2) | (2)},
    {.fmt = kVIDEO_PixelFormatXRGB4444, .com7 = (3 << 2) | (2)}};

static const ov7725_night_mode_t ov7725NightModeConfigs[] = {
    {.nightMode = CAMERA_NIGHT_MODE_DISABLED, .com5 = 0},
    {.nightMode = CAMERA_NIGHT_MODE_AUTO_FR_DIVBY2, .com5 = 8 | (1 << 4)},
    {.nightMode = CAMERA_NIGHT_MODE_AUTO_FR_DIVBY4, .com5 = 8 | (2 << 4)},
    {.nightMode = CAMERA_NIGHT_MODE_AUTO_FR_DIVBY8, .com5 = 8 | (3 << 4)}};

static const ov7725_cmd_func_map_t ov7725CmdFuncMap[] = {
    {
        kCAMERA_DeviceLightMode,
        OV7725_SetLightMode,
    },
    {
        kCAMERA_DeviceSaturation,
        OV7725_SetSaturation,
    },
    {
        kCAMERA_DeviceBrightness,
        OV7725_SetBrightness,
    },
    {
        kCAMERA_DeviceContrast,
        OV7725_SetContrast,
    },
    {
        kCAMERA_DeviceSpecialEffect,
        OV7725_SetSpecialEffect,
    },
    {
        kCAMERA_DeviceNightMode,
        OV7725_SetNightMode,
    },
};

static const ov7725_reg_t ov7725InitRegs[] = {
    {0x3d, 0x03},
    {0x42, 0x7f},
    {0x4d, 0x09},

    /* DSP */
    {0x64, 0xff},
    {0x65, 0x20},
    {0x66, 0x00},
    {0x67, 0x48},
    {0x0f, 0xc5},
    {0x13, 0xff},

    /* AEC/AGC/AWB */
    {0x63, 0xe0},
    {0x14, 0x11},
    {0x22, 0x3f},
    {0x23, 0x07},
    {0x24, 0x40},
    {0x25, 0x30},
    {0x26, 0xa1},
    {0x2b, 0x00},
    {0x6b, 0xaa},
    {0x0d, 0x41},

    /* Sharpness. */
    {0x90, 0x05},
    {0x91, 0x01},
    {0x92, 0x03},
    {0x93, 0x00},

    /* Matrix. */
    {0x94, 0x90},
    {0x95, 0x8a},
    {0x96, 0x06},
    {0x97, 0x0b},
    {0x98, 0x95},
    {0x99, 0xa0},
    {0x9a, 0x1e},

    /* Brightness. */
    {0x9b, 0x08},
    /* Contrast. */
    {0x9c, 0x20},
    /* UV */
    {0x9e, 0x81},
    /* DSE */
    {0xa6, 0x04},

    /* Gamma. */
    {0x7e, 0x0c},
    {0x7f, 0x16},
    {0x80, 0x2a},
    {0x81, 0x4e},
    {0x82, 0x61},
    {0x83, 0x6f},
    {0x84, 0x7b},
    {0x85, 0x86},
    {0x86, 0x8e},
    {0x87, 0x97},
    {0x88, 0xa4},
    {0x89, 0xaf},
    {0x8a, 0xc5},
    {0x8b, 0xd7},
    {0x8c, 0xe8},
};

/*******************************************************************************
 * Code
 ******************************************************************************/

static void OV7725_DelayMs(uint32_t ms)
{
    volatile uint32_t i;
    uint32_t loopPerMs = SystemCoreClock / 3000;

    while (ms--)
    {
        i = loopPerMs;
        while (i--)
        {
        }
    }
}

static status_t OV7725_WriteRegs(camera_device_handle_t *handle, const ov7725_reg_t regs[], uint32_t num)
{
    status_t status = kStatus_Success;

    for (uint32_t i = 0; i < num; i++)
    {
        status = OV7725_WriteReg(handle, regs[i].reg, regs[i].val);

        if (kStatus_Success != status)
        {
            break;
        }
    }

    return status;
}

static status_t OV7725_SoftwareReset(camera_device_handle_t *handle)
{
    return OV7725_WriteReg(handle, OV7725_COM7_REG, 0x80);
}

static status_t OV7725_SetClockConfig(camera_device_handle_t *handle, uint32_t frameRate_Hz, uint32_t inputClk_Hz)
{
    for (uint32_t i = 0; i < ARRAY_SIZE(ov7725ClockConfigs); i++)
    {
        if ((ov7725ClockConfigs[i].frameRate_Hz == frameRate_Hz) && (ov7725ClockConfigs[i].inputClk_Hz == inputClk_Hz))
        {
            OV7725_WriteReg(handle, OV7725_CLKRC_REG, ov7725ClockConfigs[i].clkrc);
            OV7725_ModifyReg(handle, OV7725_COM4_REG, 0xC0, ov7725ClockConfigs[i].com4);
            OV7725_WriteReg(handle, OV7725_EXHCL_REG, 0x00);
            OV7725_WriteReg(handle, OV7725_DM_LNL_REG, ov7725ClockConfigs[i].dm_lnl);
            OV7725_WriteReg(handle, OV7725_DM_LNH_REG, 0x00);
            OV7725_WriteReg(handle, OV7725_ADVFL_REG, 0x00);
            OV7725_WriteReg(handle, OV7725_ADVFH_REG, 0x00);
            return OV7725_WriteReg(handle, OV7725_COM5_REG, 0x65);
        }
    }

    return kStatus_InvalidArgument;
}

static status_t OV7725_SetPixelFormat(camera_device_handle_t *handle, video_pixel_format_t fmt)
{
    for (uint8_t i = 0; i < ARRAY_SIZE(ov7725PixelFormatConfigs); i++)
    {
        if (ov7725PixelFormatConfigs[i].fmt == fmt)
        {
            return OV7725_ModifyReg(handle, OV7725_COM7_REG, 0x1FU, ov7725PixelFormatConfigs[i].com7);
        }
    }

    return kStatus_InvalidArgument;
}

status_t OV7725_Init(camera_device_handle_t *handle, const camera_config_t *config)
{
    status_t status;
    uint8_t pid, ver;
    uint8_t com10 = 0;
    uint16_t width, height;
    uint16_t hstart, vstart, hsize;
    ov7725_resource_t *resource = (ov7725_resource_t *)(handle->resource);

    if ((kCAMERA_InterfaceNonGatedClock != config->interface) && (kCAMERA_InterfaceGatedClock != config->interface) &&
        (kCAMERA_InterfaceCCIR656 != config->interface))
    {
        return kStatus_InvalidArgument;
    }

    width  = FSL_VIDEO_EXTRACT_WIDTH(config->resolution);
    height = FSL_VIDEO_EXTRACT_HEIGHT(config->resolution);

    if ((width > 640) || (height > 480))
    {
        return kStatus_InvalidArgument;
    }

    resource->pullPowerDownPin(true);

    /* Delay 1ms. */
    OV7725_DelayMs(1);

    resource->pullPowerDownPin(false);

    /* Delay 1ms. */
    OV7725_DelayMs(1);

    resource->pullResetPin(false);

    /* Delay 1ms. */
    OV7725_DelayMs(1);

    resource->pullResetPin(true);

    /* Delay 1ms. */
    OV7725_DelayMs(1);

    /* Identify the device. */
    status = OV7725_ReadReg(handle, OV7725_PID_REG, &pid);
    if (kStatus_Success != status)
    {
        return status;
    }

    status = OV7725_ReadReg(handle, OV7725_VER_REG, &ver);
    if (kStatus_Success != status)
    {
        return status;
    }

    if (OV7725_REVISION != (((uint32_t)pid << 8U) | (uint32_t)ver))
    {
        return kStatus_Fail;
    }

    /* Device identify OK, perform software reset. */
    OV7725_SoftwareReset(handle);

    /* Delay 2ms. */
    OV7725_DelayMs(2);

    /* Start configuration */
    status = OV7725_WriteRegs(handle, ov7725InitRegs, ARRAY_SIZE(ov7725InitRegs));
    if (kStatus_Success != status)
    {
        return status;
    }

    /* Clock setting. */
    status = OV7725_SetClockConfig(handle, config->framePerSec, resource->inputClockFreq_Hz);
    if (kStatus_Success != status)
    {
        return status;
    }

    /* Pixel format setting. */
    status = OV7725_SetPixelFormat(handle, config->pixelFormat);
    if (kStatus_Success != status)
    {
        return status;
    }

    if (kCAMERA_InterfaceCCIR656 == config->interface)
    {
        OV7725_ModifyReg(handle, OV7725_COM7_REG, (1 << 5), (1 << 5));
        width += 2;
    }
    else
    {
        OV7725_ModifyReg(handle, OV7725_COM7_REG, (1 << 5), (0 << 5));
    }

    if (kCAMERA_HrefActiveHigh != (config->controlFlags & kCAMERA_HrefActiveHigh))
    {
        com10 |= OV7725_COM10_HREF_REVERSE_MASK;
    }

    if (kCAMERA_VsyncActiveHigh != (config->controlFlags & kCAMERA_VsyncActiveHigh))
    {
        com10 |= OV7725_COM10_VSYNC_NEG_MASK;
    }

    if (kCAMERA_DataLatchOnRisingEdge != (config->controlFlags & kCAMERA_DataLatchOnRisingEdge))
    {
        com10 |= OV7725_COM10_PCLK_REVERSE_MASK;
    }

    if (kCAMERA_InterfaceNonGatedClock == config->interface)
    {
        com10 |= OV7725_COM10_PCLK_OUT_MASK;
    }

    OV7725_WriteReg(handle, OV7725_COM10_REG, com10);

    /* Don't swap output MSB/LSB. */
    OV7725_WriteReg(handle, OV7725_COM3_REG, 0x00);

    /*
     * Output drive capability
     * 0: 1X
     * 1: 2X
     * 2: 3X
     * 3: 4X
     */
    OV7725_ModifyReg(handle, OV7725_COM2_REG, 0x03, 0x03);

    /* Resolution and timing. */
    hstart = 0x22U << 2U;
    vstart = 0x07U << 1U;
    hsize  = width + 16U;

    /* Set the window size. */
    OV7725_WriteReg(handle, OV7725_HSTART_REG, hstart >> 2U);
    OV7725_WriteReg(handle, OV7725_HSIZE_REG, hsize >> 2U);
    OV7725_WriteReg(handle, OV7725_VSTART_REG, vstart >> 1U);
    OV7725_WriteReg(handle, OV7725_VSIZE_REG, height >> 1U);
    OV7725_WriteReg(handle, OV7725_HOUTSIZE_REG, width >> 2U);
    OV7725_WriteReg(handle, OV7725_VOUTSIZE_REG, height >> 1U);
    OV7725_WriteReg(handle, OV7725_HREF_REG,
                    ((vstart & 1U) << 6U) | ((hstart & 3U) << 4U) | ((height & 1U) << 2U) | ((hsize & 3U) << 0U));
    return OV7725_WriteReg(handle, OV7725_EXHCH_REG, ((height & 1U) << 2U) | ((width & 3U) << 0U));
}

status_t OV7725_SetSpecialEffect(camera_device_handle_t *handle, int32_t effect)
{
    uint8_t i;

    for (i = 0; i < ARRAY_SIZE(ov7725SpecialEffectConfigs); i++)
    {
        if (effect == ov7725SpecialEffectConfigs[i].effect)
        {
            OV7725_WriteReg(handle, OV7725_SDE_REG, ov7725SpecialEffectConfigs[i].sde);
            OV7725_WriteReg(handle, OV7725_UFIX_REG, ov7725SpecialEffectConfigs[i].ufix);
            OV7725_WriteReg(handle, OV7725_VFIX_REG, ov7725SpecialEffectConfigs[i].vfix);

            return OV7725_ModifyReg(handle, OV7725_DSP_CTRL1_REG, 1 << 5, 1 << 5);
        }
    }

    /* No configuration found. */
    return kStatus_InvalidArgument;
}

status_t OV7725_SetContrast(camera_device_handle_t *handle, int32_t contrast)
{
    if ((contrast < -4) || (contrast > 4))
    {
        return kStatus_InvalidArgument;
    }

    return OV7725_WriteReg(handle, OV7725_CNST_REG, 0x20 + (0x04 * contrast));
}

status_t OV7725_SetBrightness(camera_device_handle_t *handle, int32_t brightness)
{
    if ((brightness < -4) || (brightness > 4))
    {
        return kStatus_InvalidArgument;
    }

    if (brightness >= 0)
    {
        OV7725_WriteReg(handle, OV7725_BRIGHT_REG, 0x08 + (0x10 * brightness));
        return OV7725_WriteReg(handle, OV7725_SIGN_REG, 0x06);
    }
    else
    {
        brightness = -brightness - 1;
        OV7725_WriteReg(handle, OV7725_BRIGHT_REG, 0x08 + (0x10 * brightness));
        return OV7725_WriteReg(handle, OV7725_SIGN_REG, 0x0e);
    }
}

status_t OV7725_SetSaturation(camera_device_handle_t *handle, int32_t saturation)
{
    if ((saturation < -4) || (saturation > 4))
    {
        return kStatus_InvalidArgument;
    }

    uint8_t val = (saturation + 4) * 0x10;

    OV7725_WriteReg(handle, OV7725_USAT_REG, val);
    return OV7725_WriteReg(handle, OV7725_VSAT_REG, val);
}

status_t OV7725_SetLightMode(camera_device_handle_t *handle, int32_t lightMode)
{
    uint8_t i;

    for (i = 0; i < ARRAY_SIZE(ov7725LightModeConfigs); i++)
    {
        if (lightMode == ov7725LightModeConfigs[i].lightMode)
        {
            OV7725_WriteReg(handle, OV7725_COM8_REG, ov7725LightModeConfigs[i].com8);
            OV7725_WriteReg(handle, OV7725_BLUE_REG, ov7725LightModeConfigs[i].blue);
            OV7725_WriteReg(handle, OV7725_RED_REG, ov7725LightModeConfigs[i].red);
            OV7725_WriteReg(handle, OV7725_COM5_REG, ov7725LightModeConfigs[i].com5);
            OV7725_WriteReg(handle, OV7725_ADVFL_REG, 0);
            return OV7725_WriteReg(handle, OV7725_ADVFH_REG, 0);
        }
    }

    /* No configuration found. */
    return kStatus_InvalidArgument;
}

status_t OV7725_SetNightMode(camera_device_handle_t *handle, int32_t nightMode)
{
    uint8_t i;

    for (i = 0; i < ARRAY_SIZE(ov7725NightModeConfigs); i++)
    {
        if (nightMode == ov7725NightModeConfigs[i].nightMode)
        {
            return OV7725_ModifyReg(handle, OV7725_COM5_REG, 0xF0, ov7725NightModeConfigs[i].com5);
        }
    }

    /* No configuration found. */
    return kStatus_InvalidArgument;
}

status_t OV7725_Deinit(camera_device_handle_t *handle)
{
    ((ov7725_resource_t *)(handle->resource))->pullPowerDownPin(true);

    return kStatus_Success;
}

status_t OV7725_Control(camera_device_handle_t *handle, camera_device_cmd_t cmd, int32_t arg)
{
    for (uint8_t i = 0; i < ARRAY_SIZE(ov7725CmdFuncMap); i++)
    {
        if (ov7725CmdFuncMap[i].cmd == cmd)
        {
            return ov7725CmdFuncMap[i].func(handle, arg);
        }
    }

    return kStatus_InvalidArgument;
}

status_t OV7725_Start(camera_device_handle_t *handle)
{
    return kStatus_Success;
}

status_t OV7725_Stop(camera_device_handle_t *handle)
{
    return kStatus_Success;
}

status_t OV7725_InitExt(camera_device_handle_t *handle, const camera_config_t *config, const void *specialConfig)
{
    return OV7725_Init(handle, config);
}
