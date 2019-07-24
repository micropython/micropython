/*
 * Copyright  2017-2018 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_OV7725_H_
#define _FSL_OV7725_H_

#include "fsl_common.h"
#include "fsl_sccb.h"
#include "fsl_camera_device.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief Register definitions for the OV7725.*/

#define OV7725_GAIN_REG 0x00U       /*!< Gain control gain setting */
#define OV7725_BLUE_REG 0x01U       /*!< Blue channel gain setting */
#define OV7725_RED_REG 0x02U        /*!< Red channel gain setting. */
#define OV7725_GREEN_REG 0x03U      /*!< Green channel gain setting */
#define OV7725_BAVG_REG 0x05U       /*!< B Average Level */
#define OV7725_GAVG_REG 0x06U       /*!< G Average Level */
#define OV7725_RAVG_REG 0x07U       /*!< R Average Level */
#define OV7725_AECH_REG 0x08U       /*!< Exposure Value - AEC MSBs */
#define OV7725_COM2_REG 0x09U       /*!< Common Control 2 */
#define OV7725_PID_REG 0x0AU        /*!< Product ID Number MSB */
#define OV7725_VER_REG 0x0BU        /*!< Product ID Number LSB */
#define OV7725_COM3_REG 0x0CU       /*!< Common Control 3 */
#define OV7725_COM4_REG 0x0DU       /*!< Common Control 4 */
#define OV7725_COM5_REG 0x0EU       /*!< Common Control 5 */
#define OV7725_COM6_REG 0x0FU       /*!< Common Control 6 */
#define OV7725_AEC_REG 0x10U        /*!< Exposure Value */
#define OV7725_CLKRC_REG 0x11U      /*!< Internal Clock */
#define OV7725_COM7_REG 0x12U       /*!< Common Control 7 */
#define OV7725_COM8_REG 0x13U       /*!< Common Control 8 */
#define OV7725_COM9_REG 0x14U       /*!< Common Control 9 */
#define OV7725_COM10_REG 0x15U      /*!< Common Control 10 */
#define OV7725_REG16_REG 0x16U      /*!< Register 16 */
#define OV7725_HSTART_REG 0x17U     /*!< Horizontal Frame (HREF column) Start 8 MSBs */
#define OV7725_HSIZE_REG 0x18U      /*!< Horizontal Sensor Size */
#define OV7725_VSTART_REG 0x19U     /*!< Vertical Frame (row) Start 8 MSBs */
#define OV7725_VSIZE_REG 0x1AU      /*!< Vertical Sensor Size */
#define OV7725_PSHFT_REG 0x1BU      /*!< Data format */
#define OV7725_MIDH_REG 0x1CU       /*!< Manufacturer ID Byte - High */
#define OV7725_MIDL_REG 0x1DU       /*!< Manufacturer ID Byte - Low  */
#define OV7725_LAEC_REG 0x1FU       /*!< Fine AEC Value */
#define OV7725_COM11_REG 0x20U      /*!< Common Control 11 */
#define OV7725_BDBASE_REG 0x22U     /*!< Banding Filter Minimum AEC Value */
#define OV7725_BDMSTEP_REG 0x23U    /*!< Banding Filter Maximum Step */
#define OV7725_AEW_REG 0x24U        /*!< AGC/AEC Stable Operating Region (Upper Limit) */
#define OV7725_AEB_REG 0x25U        /*!< AGC/AEC Stable Operating Region (Lower Limit) */
#define OV7725_VPT_REG 0x26U        /*!< AGC/AEC Fast Mode Operating Region */
#define OV7725_REG28_REG 0x28U      /*!< Register 28 */
#define OV7725_HOUTSIZE_REG 0x29U   /*!< Horizontal Data Output Size 8 MSBs */
#define OV7725_EXHCH_REG 0x2AU      /*!< Dummy Pixel Insert MSB */
#define OV7725_EXHCL_REG 0x2BU      /*!< Dummy Pixel Insert LSB */
#define OV7725_VOUTSIZE_REG 0x2CU   /*!< Vertical Data Output Size MSBs */
#define OV7725_ADVFL_REG 0x2DU      /*!< LSB of Insert Dummy Rows in Vertical Sync (1 bit equals 1 row) */
#define OV7725_ADVFH_REG 0x2EU      /*!< MSB of Insert Dummy Rows in Vertical Sync */
#define OV7725_YAVE_REG 0x2FU       /*!< Y/G Channel Average Value */
#define OV7725_LUMHTH_REG 0x30U     /*!< Histogram AEC/AGC Luminance High Level Threshold */
#define OV7725_LUMLTH_REG 0x31U     /*!< Histogram AEC/AGC Luminance Low Level Threshold */
#define OV7725_HREF_REG 0x32U       /*!< Image Start and Size Control */
#define OV7725_DM_LNL_REG 0x33U     /*!< Low 8 Bits of the Number of Dummy Rows */
#define OV7725_DM_LNH_REG 0x34U     /*!< High 8 Bits of the Number of Dummy Rows */
#define OV7725_ADOFF_B_REG 0x35U    /*!< AD Offset Compensation Value for B Channel */
#define OV7725_ADOFF_R_REG 0x36U    /*!< AD Offset Compensation Value for R Channel */
#define OV7725_ADOFF_GB_REG 0x37U   /*!< AD Offset Compensation Value for Gb Channel */
#define OV7725_ADOFF_GR_REG 0x38U   /*!< AD Offset Compensation Value for Gr Channel */
#define OV7725_OFF_B_REG 0x39U      /*!< B Channel Offset Compensation Value */
#define OV7725_OFF_R_REG 0x3AU      /*!< R Channel Offset Compensation Value */
#define OV7725_OFF_GB_REG 0x3BU     /*!< Gb Channel Offset Compensation Value */
#define OV7725_OFF_GR_REG 0x3CU     /*!< Gr Channel Offset Compensation Value */
#define OV7725_COM12_REG 0x3DU      /*!< Common Control 12 */
#define OV7725_COM13_REG 0x3EU      /*!< Common Control 13 */
#define OV7725_COM14_REG 0x3FU      /*!< Common Control 14 */
#define OV7725_COM16_REG 0x41U      /*!< Common Control 16 */
#define OV7725_TGT_B_REG 0x42U      /*!< BLC Blue Channel Target Value */
#define OV7725_TGT_R_REG 0x43U      /*!< BLC Red Channel Target Value */
#define OV7725_TGT_GB_REG 0x44U     /*!< BLC Gb Channel Target Value */
#define OV7725_TGT_GR_REG 0x45U     /*!< BLC Gr Channel Target Value */
#define OV7725_LC_CTR_REG 0x46U     /*!< Lens Correction Control */
#define OV7725_LC_XC_REG 0x47U      /*!< X Coordinate of Lens Correction Center Relative to Array Center */
#define OV7725_LC_YC_REG 0x48U      /*!< Y Coordinate of Lens Correction Center Relative to Array Center */
#define OV7725_LC_COEF_REG 0x49U    /*!< Lens Correction Coefficient */
#define OV7725_LC_RADI_REG 0x4AU    /*!< Lens Correction Radius */
#define OV7725_LC_COEFB_REG 0x4BU   /*!< Lens Correction B Channel Compensation Coefficient */
#define OV7725_LC_COEFR_REG 0x4CU   /*!< Lens Correction R Channel Compensation Coefficient */
#define OV7725_FIXGAIN_REG 0x4DU    /*!< Analog Fix Gain Amplifier */
#define OV7725_AREF1_REG 0x4FU      /*!< Sensor Reference Current Control */
#define OV7725_AREF6_REG 0x54U      /*!< Analog Reference Control */
#define OV7725_UFIX_REG 0x60U       /*!< U Channel Fixed Value Output */
#define OV7725_VFIX_REG 0x61U       /*!< V Channel Fixed Value Output */
#define OV7725_AWBB_BLK_REG 0x62U   /*!< AWB Option for Advanced AWBA */
#define OV7725_AWB_CTRL0_REG 0x63U  /*!< AWB Control Byte 0 */
#define OV7725_DSP_CTRL1_REG 0x64U  /*!< DSP Control Byte 1 */
#define OV7725_DSP_CTRL2_REG 0x65U  /*!< DSP Control Byte 2 */
#define OV7725_DSP_CTRL3_REG 0x66U  /*!< DSP Control Byte 3 */
#define OV7725_DSP_CTRL4_REG 0x67U  /*!< DSP Control Byte 4 */
#define OV7725_AWB_BIAS_REG 0x68U   /*!< AWB BLC Level Clip */
#define OV7725_AWB_CTRL1_REG 0x69U  /*!< AWB Control 1 */
#define OV7725_AWB_CTRL2_REG 0x6AU  /*!< AWB Control 2 */
#define OV7725_AWB_CTRL3_REG 0x6BU  /*!< AWB Control 3 */
#define OV7725_AWB_CTRL4_REG 0x6CU  /*!< AWB Control 4 */
#define OV7725_AWB_CTRL5_REG 0x6DU  /*!< AWB Control 5 */
#define OV7725_AWB_CTRL6_REG 0x6EU  /*!< AWB Control 6 */
#define OV7725_AWB_CTRL7_REG 0x6FU  /*!< AWB Control 7 */
#define OV7725_AWB_CTRL8_REG 0x70U  /*!< AWB Control 8 */
#define OV7725_AWB_CTRL9_REG 0x71U  /*!< AWB Control 9 */
#define OV7725_AWB_CTRL10_REG 0x72U /*!< AWB Control 10 */
#define OV7725_AWB_CTRL11_REG 0x73U /*!< AWB Control 11 */
#define OV7725_AWB_CTRL12_REG 0x74U /*!< AWB Control 12 */
#define OV7725_AWB_CTRL13_REG 0x75U /*!< AWB Control 13 */
#define OV7725_AWB_CTRL14_REG 0x76U /*!< AWB Control 14 */
#define OV7725_AWB_CTRL15_REG 0x77U /*!< AWB Control 15 */
#define OV7725_AWB_CTRL16_REG 0x78U /*!< AWB Control 16 */
#define OV7725_AWB_CTRL17_REG 0x79U /*!< AWB Control 17 */
#define OV7725_AWB_CTRL18_REG 0x7AU /*!< AWB Control 18 */
#define OV7725_AWB_CTRL19_REG 0x7BU /*!< AWB R Gain Range */
#define OV7725_AWB_CTRL20_REG 0x7CU /*!< AWB G Gain Range */
#define OV7725_AWB_CTRL21_REG 0x7DU /*!< AWB B Gain Range */
#define OV7725_GAM1_REG 0x7EU       /*!< Gamma Curve 1st Segment Input End Point  0x04 Output Value */
#define OV7725_GAM2_REG 0x7FU       /*!< Gamma Curve 2nd Segment Input End Point  0x08 Output Value */
#define OV7725_GAM3_REG 0x80U       /*!< Gamma Curve 3rd Segment Input End Point  0x10 Output Value */
#define OV7725_GAM4_REG 0x81U       /*!< Gamma Curve 4th Segment Input End Point  0x20 Output Value */
#define OV7725_GAM5_REG 0x82U       /*!< Gamma Curve 5th Segment Input End Point  0x28 Output Value */
#define OV7725_GAM6_REG 0x83U       /*!< Gamma Curve 6th Segment Input End Point  0x30 Output Value */
#define OV7725_GAM7_REG 0x84U       /*!< Gamma Curve 7th Segment Input End Point  0x38 Output Value */
#define OV7725_GAM8_REG 0x85U       /*!< Gamma Curve 8th Segment Input End Point  0x40 Output Value */
#define OV7725_GAM9_REG 0x86U       /*!< Gamma Curve 9th Segment Input End Point  0x48 Output Value */
#define OV7725_GAM10_REG 0x87U      /*!< Gamma Curve 10th Segment Input End Point 0x50 Output Value */
#define OV7725_GAM11_REG 0x88U      /*!< Gamma Curve 11th Segment Input End Point 0x60 Output Value */
#define OV7725_GAM12_REG 0x89U      /*!< Gamma Curve 12th Segment Input End Point 0x70 Output Value */
#define OV7725_GAM13_REG 0x8AU      /*!< Gamma Curve 13th Segment Input End Point 0x90 Output Value */
#define OV7725_GAM14_REG 0x8BU      /*!< Gamma Curve 14th Segment Input End Point 0xB0 Output Value */
#define OV7725_GAM15_REG 0x8CU      /*!< Gamma Curve 15th Segment Input End Point 0xD0 Output Value */
#define OV7725_SLOP_REG 0x8DU       /*!< Gamma Curve Highest Segment Slope */
#define OV7725_DNSTH_REG 0x8EU      /*!< De-noise Threshold */
#define OV7725_EDGE0_REG 0x8FU      /*!< Sharpness (Edge Enhancement) Control 0 */
#define OV7725_EDGE1_REG 0x90U      /*!< Sharpness (Edge Enhancement) Control 1 */
#define OV7725_DNSOFF_REG 0x91U     /*!< Lower Limit of De-noise Threshold - effective in auto mode only */
#define OV7725_EDGE2_REG 0x92U      /*!< Sharpness (Edge Enhancement) Strength Upper Limit */
#define OV7725_EDGE3_REG 0x93U      /*!< Sharpness (Edge Enhancement) Strength Lower Limit */
#define OV7725_MTX1_REG 0x94U       /*!< Matrix Coefficient 1 */
#define OV7725_MTX2_REG 0x95U       /*!< Matrix Coefficient 2 */
#define OV7725_MTX3_REG 0x96U       /*!< Matrix Coefficient 3 */
#define OV7725_MTX4_REG 0x97U       /*!< Matrix Coefficient 4 */
#define OV7725_MTX5_REG 0x98U       /*!< Matrix Coefficient 5 */
#define OV7725_MTX6_REG 0x99U       /*!< Matrix Coefficient 6 */
#define OV7725_MTX_CTRL_REG 0x9AU   /*!< Matrix Control */
#define OV7725_BRIGHT_REG 0x9BU     /*!< Brightness */
#define OV7725_CNST_REG 0x9CU       /*!< Contrast */
#define OV7725_UVADJ0_REG 0x9EU     /*!< Auto UV Adjust Control 0 */
#define OV7725_UVADJ1_REG 0x9FU     /*!< Auto UV Adjust Control 1 */
#define OV7725_SCAL0_REG 0xA0U      /*!< DCW Ratio Control */
#define OV7725_SCAL1_REG 0xA1U      /*!< Horizontal Zoom Out Control */
#define OV7725_SCAL2_REG 0xA2U      /*!< Vertical Zoom Out Control */
#define OV7725_SDE_REG 0xA6U        /*!< Special Digital Effect (SDE) Control */
#define OV7725_USAT_REG 0xA7U       /*!< U Component Saturation Gain */
#define OV7725_VSAT_REG 0xA8U       /*!< V Component Saturation Gain */
#define OV7725_HUECOS_REG 0xA9U     /*!< Cosine value x 0x80 */
#define OV7725_HUESIN_REG 0xAAU     /*!< |Sine value| x 0x80 */
#define OV7725_SIGN_REG 0xABU       /*!< Sign Bit for Hue and Brightness */
#define OV7725_DSPAUTO_REG 0xACU    /*!< DSP Auto Function ON/OFF Control */

#define OV7725_COM10_VSYNC_NEG_MASK (1U << 1U)
#define OV7725_COM10_HREF_REVERSE_MASK (1U << 3U)
#define OV7725_COM10_PCLK_REVERSE_MASK (1U << 4U)
#define OV7725_COM10_PCLK_OUT_MASK (1U << 5U)
#define OV7725_COM10_DATA_NEG_MASK (1U << 7U)

/*!
 * @brief OV7725 resource.
 *
 * Before initialize the OV7725, the resource must be initialized that the SCCB
 * I2C could start to work.
 */
typedef struct _ov7725_resource
{
    sccb_i2c_send_func_t i2cSendFunc;       /*!< I2C send function. */
    sccb_i2c_receive_func_t i2cReceiveFunc; /*!< I2C receive function. */
    void (*pullResetPin)(bool pullUp);      /*!< Function to pull reset pin high or low. */
    void (*pullPowerDownPin)(bool pullUp);  /*!< Function to pull the power down pin high or low. */
    uint32_t inputClockFreq_Hz;             /*!< Input clock frequency. */
} ov7725_resource_t;

/*! @brief OV7725 operation functions. */
extern const camera_device_operations_t ov7725_ops;

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

#if defined(__cplusplus)
}
#endif

#endif /* _FSL_OV7725_H_ */
