/*
 * Copyright 2017-2018 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_video_common.h"
#include "fsl_camera.h"
#include "fsl_camera_device.h"
#include "fsl_mt9m114.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define MT9M114_DelayMs(ms) VIDEO_DelayMs(ms)
#define MT9M114_Write(handle, reg, size, value)                                                    \
    VIDEO_I2C_WriteReg(MT9M114_I2C_ADDR, kVIDEO_RegAddr16Bit, reg, (video_reg_width_t)size, value, \
                       ((mt9m114_resource_t *)(handle->resource))->i2cSendFunc)
#define MT9M114_Read(handle, reg, size, value)                                                    \
    VIDEO_I2C_ReadReg(MT9M114_I2C_ADDR, kVIDEO_RegAddr16Bit, reg, (video_reg_width_t)size, value, \
                      ((mt9m114_resource_t *)(handle->resource))->i2cReceiveFunc)
#define MT9M114_Modify(handle, reg, size, clrMask, value)                                                    \
    VIDEO_I2C_ModifyReg(MT9M114_I2C_ADDR, kVIDEO_RegAddr16Bit, reg, (video_reg_width_t)size, clrMask, value, \
                        ((mt9m114_resource_t *)(handle->resource))->i2cReceiveFunc,                          \
                        ((mt9m114_resource_t *)(handle->resource))->i2cSendFunc)

typedef struct _mt9m114_reg
{
    uint16_t reg;
    uint8_t size;
    uint32_t value;
} mt9m114_reg_t;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
status_t MT9M114_Init(camera_device_handle_t *handle, const camera_config_t *config);
status_t MT9M114_Deinit(camera_device_handle_t *handle);
status_t MT9M114_Start(camera_device_handle_t *handle);
status_t MT9M114_Stop(camera_device_handle_t *handle);
status_t MT9M114_Control(camera_device_handle_t *handle, camera_device_cmd_t cmd, int32_t arg);
status_t MT9M114_InitExt(camera_device_handle_t *handle, const camera_config_t *config, const void *specialConfig);

/*******************************************************************************
 * Variables
 ******************************************************************************/
const camera_device_operations_t mt9m114_ops = {
    .init     = MT9M114_Init,
    .deinit   = MT9M114_Deinit,
    .start    = MT9M114_Start,
    .stop     = MT9M114_Stop,
    .control  = MT9M114_Control,
    .init_ext = MT9M114_InitExt,
};

static const mt9m114_reg_t mt9m114_720p[] = {
    {MT9M114_VAR_CAM_SENSOR_CFG_Y_ADDR_START, 2, 0x007C},     /* cam_sensor_cfg_y_addr_start = 124 */
    {MT9M114_VAR_CAM_SENSOR_CFG_X_ADDR_START, 2, 0x0004},     /* cam_sensor_cfg_x_addr_start = 4 */
    {MT9M114_VAR_CAM_SENSOR_CFG_Y_ADDR_END, 2, 0x0353},       /* cam_sensor_cfg_y_addr_end = 851 */
    {MT9M114_VAR_CAM_SENSOR_CFG_X_ADDR_END, 2, 0x050B},       /* cam_sensor_cfg_x_addr_end = 1291 */
    {MT9M114_VAR_CAM_SENSOR_CFG_CPIPE_LAST_ROW, 2, 0x02D3},   /* cam_sensor_cfg_cpipe_last_row = 723 */
    {MT9M114_VAR_CAM_CROP_WINDOW_WIDTH, 2, 0x0500},           /* cam_crop_window_width = 1280 */
    {MT9M114_VAR_CAM_CROP_WINDOW_HEIGHT, 2, 0x02D0},          /* cam_crop_window_height = 720 */
    {MT9M114_VAR_CAM_OUTPUT_WIDTH, 2, 0x0500},                /* cam_output_width = 1280 */
    {MT9M114_VAR_CAM_OUTPUT_HEIGHT, 2, 0x02D0},               /* cam_output_height = 720 */
    {MT9M114_VAR_CAM_STAT_AWB_CLIP_WINDOW_XEND, 2, 0x04FF},   /* cam_stat_awb_clip_window_xend = 1279 */
    {MT9M114_VAR_CAM_STAT_AWB_CLIP_WINDOW_YEND, 2, 0x02CF},   /* cam_stat_awb_clip_window_yend = 719 */
    {MT9M114_VAR_CAM_STAT_AE_INITIAL_WINDOW_XEND, 2, 0x00FF}, /* cam_stat_ae_initial_window_xend = 255 */
    {MT9M114_VAR_CAM_STAT_AE_INITIAL_WINDOW_YEND, 2, 0x008F}, /* cam_stat_ae_initial_window_yend = 143 */
};

static const mt9m114_reg_t mt9m114_480_272[] = {
    {MT9M114_VAR_CAM_SENSOR_CFG_Y_ADDR_START, 2, 0x00D4},     /* cam_sensor_cfg_y_addr_start = 212 */
    {MT9M114_VAR_CAM_SENSOR_CFG_X_ADDR_START, 2, 0x00A4},     /* cam_sensor_cfg_x_addr_start = 164 */
    {MT9M114_VAR_CAM_SENSOR_CFG_Y_ADDR_END, 2, 0x02FB},       /* cam_sensor_cfg_y_addr_end = 763 */
    {MT9M114_VAR_CAM_SENSOR_CFG_X_ADDR_END, 2, 0x046B},       /* cam_sensor_cfg_x_addr_end = 1131 */
    {MT9M114_VAR_CAM_SENSOR_CFG_CPIPE_LAST_ROW, 2, 0x0223},   /* cam_sensor_cfg_cpipe_last_row = 547 */
    {MT9M114_VAR_CAM_CROP_WINDOW_WIDTH, 2, 0x03C0},           /* cam_crop_window_width = 960 */
    {MT9M114_VAR_CAM_CROP_WINDOW_HEIGHT, 2, 0x0220},          /* cam_crop_window_height = 544 */
    {MT9M114_VAR_CAM_OUTPUT_WIDTH, 2, 0x01E0},                /* cam_output_width = 480 */
    {MT9M114_VAR_CAM_OUTPUT_HEIGHT, 2, 0x0110},               /* cam_output_height = 272 */
    {MT9M114_VAR_CAM_STAT_AWB_CLIP_WINDOW_XEND, 2, 0x01DF},   /* cam_stat_awb_clip_window_xend = 479 */
    {MT9M114_VAR_CAM_STAT_AWB_CLIP_WINDOW_YEND, 2, 0x010F},   /* cam_stat_awb_clip_window_yend = 271 */
    {MT9M114_VAR_CAM_STAT_AE_INITIAL_WINDOW_XEND, 2, 0x005F}, /* cam_stat_ae_initial_window_xend = 95 */
    {MT9M114_VAR_CAM_STAT_AE_INITIAL_WINDOW_YEND, 2, 0x0035}, /* cam_stat_ae_initial_window_yend = 53 */
};

static const mt9m114_reg_t mt9m114InitConfig[] = {
    {MT9M114_REG_LOGICAL_ADDRESS_ACCESS, 2u, 0x1000},
    /* PLL Fout = (Fin * 2 * m) / ((n + 1) * (p + 1)) */
    {MT9M114_VAR_CAM_SYSCTL_PLL_ENABLE, 1u, 0x01},        /*  cam_sysctl_pll_enable = 1 */
    {MT9M114_VAR_CAM_SYSCTL_PLL_DIVIDER_M_N, 2u, 0x0120}, /*  cam_sysctl_pll_divider_m_n = 288 */
    {MT9M114_VAR_CAM_SYSCTL_PLL_DIVIDER_P, 2u, 0x0700},   /*  cam_sysctl_pll_divider_p = 1792 */
    {MT9M114_VAR_CAM_SENSOR_CFG_PIXCLK, 4u, 0x2DC6C00},   /*  cam_sensor_cfg_pixclk = 48000000 */
    {0x316A, 2, 0x8270}, /*  auto txlo_row for hot pixel and linear full well optimization */
    {0x316C, 2, 0x8270}, /*  auto txlo for hot pixel and linear full well optimization */
    {0x3ED0, 2, 0x2305}, /*  eclipse setting, ecl range=1, ecl value=2, ivln=3 */
    {0x3ED2, 2, 0x77CF}, /*  TX_hi=12 */
    {0x316E, 2, 0x8202}, /*  auto ecl , threshold 2x, ecl=0 at high gain, ecl=2 for low gain */
    {0x3180, 2, 0x87FF}, /*  enable delta dark */
    {0x30D4, 2, 0x6080}, /*  disable column correction due to AE oscillation problem */
    {0xA802, 2, 0x0008}, /*  RESERVED_AE_TRACK_02 */
    {0x3E14, 2, 0xFF39}, /*  Enabling pixout clamping to VAA during ADC streaming to solve column band issue */
    {MT9M114_VAR_CAM_SENSOR_CFG_ROW_SPEED, 2u, 0x0001},           /*  cam_sensor_cfg_row_speed = 1 */
    {MT9M114_VAR_CAM_SENSOR_CFG_FINE_INTEG_TIME_MIN, 2u, 0x00DB}, /*  cam_sensor_cfg_fine_integ_time_min = 219 */
    {MT9M114_VAR_CAM_SENSOR_CFG_FINE_INTEG_TIME_MAX, 2u, 0x07C2}, /*  cam_sensor_cfg_fine_integ_time_max = 1986 */
    {MT9M114_VAR_CAM_SENSOR_CFG_FRAME_LENGTH_LINES, 2u, 0x02FE},  /*  cam_sensor_cfg_frame_length_lines = 766 */
    {MT9M114_VAR_CAM_SENSOR_CFG_LINE_LENGTH_PCK, 2u, 0x0845},     /*  cam_sensor_cfg_line_length_pck = 2117 */
    {MT9M114_VAR_CAM_SENSOR_CFG_FINE_CORRECTION, 2u, 0x0060},     /*  cam_sensor_cfg_fine_correction = 96 */
    {MT9M114_VAR_CAM_SENSOR_CFG_REG_0_DATA, 2u, 0x0020},          /*  cam_sensor_cfg_reg_0_data = 32 */
    {MT9M114_VAR_CAM_SENSOR_CONTROL_READ_MODE, 2u, 0x0000},       /*  cam_sensor_control_read_mode = 0 */
    {MT9M114_VAR_CAM_CROP_WINDOW_XOFFSET, 2u, 0x0000},            /*  cam_crop_window_xoffset = 0 */
    {MT9M114_VAR_CAM_CROP_WINDOW_YOFFSET, 2u, 0x0000},            /*  cam_crop_window_yoffset = 0 */
    {MT9M114_VAR_CAM_CROP_CROPMODE, 1u, 0x03},                    /*  cam_crop_cropmode = 3 */
    {MT9M114_VAR_CAM_AET_AEMODE, 1u, 0x00},                       /*  cam_aet_aemode = 0 */
    {MT9M114_VAR_CAM_AET_MAX_FRAME_RATE, 2u, 0x1D9A},             /*  cam_aet_max_frame_rate = 7578 */
    {MT9M114_VAR_CAM_AET_MIN_FRAME_RATE, 2u, 0x1D9A},             /*  cam_aet_min_frame_rate = 7578 */
    {MT9M114_VAR_CAM_STAT_AWB_CLIP_WINDOW_XSTART, 2u, 0x0000},    /*  cam_stat_awb_clip_window_xstart = 0 */
    {MT9M114_VAR_CAM_STAT_AWB_CLIP_WINDOW_YSTART, 2u, 0x0000},    /*  cam_stat_awb_clip_window_ystart = 0 */
    {MT9M114_VAR_CAM_STAT_AE_INITIAL_WINDOW_XSTART, 2u, 0x0000},  /*  cam_stat_ae_initial_window_xstart = 0 */
    {MT9M114_VAR_CAM_STAT_AE_INITIAL_WINDOW_YSTART, 2u, 0x0000},  /*  cam_stat_ae_initial_window_ystart = 0 */
    {MT9M114_REG_PAD_SLEW, 2u, 0x0777},                           /*  Pad slew rate */
    {MT9M114_VAR_CAM_OUTPUT_FORMAT_YUV, 2u, 0x0038},              /*  Must set cam_output_format_yuv_clip for CSI */
};

/*******************************************************************************
 * Code
 ******************************************************************************/

static status_t MT9M114_MultiWrite(camera_device_handle_t *handle, const mt9m114_reg_t regs[], uint32_t num)
{
    status_t status = kStatus_Success;

    for (uint32_t i = 0; i < num; i++)
    {
        status = MT9M114_Write(handle, regs[i].reg, regs[i].size, regs[i].value);

        if (kStatus_Success != status)
        {
            break;
        }
    }

    return status;
}

static status_t MT9M114_SoftwareReset(camera_device_handle_t *handle)
{
    MT9M114_Modify(handle, MT9M114_REG_RESET_AND_MISC_CONTROL, 2u, 0x01, 0x01);
    MT9M114_DelayMs(1);
    MT9M114_Modify(handle, MT9M114_REG_RESET_AND_MISC_CONTROL, 2u, 0x01, 0x00);
    MT9M114_DelayMs(45);

    return kStatus_Success;
}

/*
 * Set state, the parameter nextState could be
 *
 * MT9M114_SYS_STATE_ENTER_CONFIG_CHANGE
 * MT9M114_SYS_STATE_START_STREAMING
 * MT9M114_SYS_STATE_ENTER_SUSPEND
 * MT9M114_SYS_STATE_ENTER_STANDBY
 * MT9M114_SYS_STATE_LEAVE_STANDBY
 */
static status_t MT9M114_SetState(camera_device_handle_t *handle, uint16_t nextState)
{
    uint16_t value;
    /* Set the desired next state. */
    MT9M114_Write(handle, MT9M114_VAR_SYSMGR_NEXT_STATE, 1u, nextState);

    /* Check that the FW is ready to accept a new command. */
    while (1)
    {
        MT9M114_Read(handle, MT9M114_REG_COMMAND_REGISTER, 2u, &value);
        if (!(value & MT9M114_COMMAND_SET_STATE))
        {
            break;
        }
    }

    /* Issue the Set State command. */
    MT9M114_Write(handle, MT9M114_REG_COMMAND_REGISTER, 2u, MT9M114_COMMAND_SET_STATE | MT9M114_COMMAND_OK);

    /* Wait for the FW to complete the command. */
    while (1)
    {
        MT9M114_DelayMs(1);
        MT9M114_Read(handle, MT9M114_REG_COMMAND_REGISTER, 2u, &value);
        if (!(value & MT9M114_COMMAND_SET_STATE))
        {
            break;
        }
    }

    /* Check the 'OK' bit to see if the command was successful. */
    MT9M114_Read(handle, MT9M114_REG_COMMAND_REGISTER, 2u, &value);
    if (!(value & MT9M114_COMMAND_OK))
    {
        return kStatus_Fail;
    }

    return kStatus_Success;
}

status_t MT9M114_Init(camera_device_handle_t *handle, const camera_config_t *config)
{
    status_t status;
    uint16_t chip_id;
    uint16_t outputFormat = 0;

    mt9m114_resource_t *resource = (mt9m114_resource_t *)(handle->resource);

    if ((kCAMERA_InterfaceGatedClock != config->interface) && (kCAMERA_InterfaceNonGatedClock != config->interface) &&
        (kCAMERA_InterfaceCCIR656 != config->interface))
    {
        return kStatus_InvalidArgument;
    }

    /* Only support 480 * 272 and 720P. */
    if ((kVIDEO_Resolution720P != config->resolution) && (FSL_VIDEO_RESOLUTION(480, 272) != config->resolution))
    {
        return kStatus_InvalidArgument;
    }

    /* Only support 30 fps now. */
    if (30 != config->framePerSec)
    {
        return kStatus_InvalidArgument;
    }

    /* Only support RGB565 and YUV422 */
    if ((kVIDEO_PixelFormatRGB565 != config->pixelFormat) && (kVIDEO_PixelFormatYUYV != config->pixelFormat))
    {
        return kStatus_InvalidArgument;
    }

    /* The input clock (EXTCLK) must be 24MHz. */
    if (24000000 != resource->inputClockFreq_Hz)
    {
        return kStatus_InvalidArgument;
    }

    /* Polarity check */
    if (config->controlFlags != (kCAMERA_HrefActiveHigh | kCAMERA_DataLatchOnRisingEdge | kCAMERA_VsyncActiveLow))
    {
        return kStatus_InvalidArgument;
    }

    resource->pullResetPin(false); /* Reset pin low. */
    MT9M114_DelayMs(50);           /* Delay at least 50 ms. */
    resource->pullResetPin(true);  /* Reset pin high. */
    MT9M114_DelayMs(45);           /* Delay typically 44.5 ms. */

    /* Identify the device. */
    status = MT9M114_Read(handle, MT9M114_REG_CHIP_ID, 2u, &chip_id);
    if (kStatus_Success != status)
    {
        return status;
    }
    if (MT9M114_CHIP_ID != chip_id)
    {
        return kStatus_Fail;
    }

    /* SW reset. */
    MT9M114_SoftwareReset(handle);

    MT9M114_MultiWrite(handle, mt9m114InitConfig, ARRAY_SIZE(mt9m114InitConfig));

    /* Pixel format. */
    if (kVIDEO_PixelFormatRGB565 == config->pixelFormat)
    {
        outputFormat |= ((1U << 8U) | (1U << 1U));
    }

    if (kCAMERA_InterfaceCCIR656 == config->interface)
    {
        outputFormat |= (1U << 3U);
    }

    MT9M114_Write(handle, MT9M114_VAR_CAM_OUTPUT_FORMAT, 2, outputFormat);

    if (kCAMERA_InterfaceNonGatedClock == config->interface)
    {
        MT9M114_Write(handle, MT9M114_VAR_CAM_PORT_OUTPUT_CONTROL, 2, 0x8020);
    }
    else
    {
        MT9M114_Write(handle, MT9M114_VAR_CAM_PORT_OUTPUT_CONTROL, 2, 0x8000);
    }

    /* Resolution */
    if (config->resolution == FSL_VIDEO_RESOLUTION(480, 272))
    {
        MT9M114_MultiWrite(handle, mt9m114_480_272, ARRAY_SIZE(mt9m114_480_272));
    }
    else
    {
        MT9M114_MultiWrite(handle, mt9m114_720p, ARRAY_SIZE(mt9m114_720p));
    }

    /* Execute Change-Config command. */
    return MT9M114_SetState(handle, MT9M114_SYS_STATE_ENTER_CONFIG_CHANGE);
}

status_t MT9M114_Deinit(camera_device_handle_t *handle)
{
    ((mt9m114_resource_t *)(handle->resource))->pullResetPin(false);

    return kStatus_Success;
}

status_t MT9M114_Start(camera_device_handle_t *handle)
{
    return MT9M114_SetState(handle, MT9M114_SYS_STATE_START_STREAMING);
}

status_t MT9M114_Stop(camera_device_handle_t *handle)
{
    return MT9M114_SetState(handle, MT9M114_SYS_STATE_ENTER_SUSPEND);
}

status_t MT9M114_Control(camera_device_handle_t *handle, camera_device_cmd_t cmd, int32_t arg)
{
    return kStatus_InvalidArgument;
}

status_t MT9M114_InitExt(camera_device_handle_t *handle, const camera_config_t *config, const void *specialConfig)
{
    return MT9M114_Init(handle, config);
}
