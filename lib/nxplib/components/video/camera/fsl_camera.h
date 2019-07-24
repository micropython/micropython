/*
 * Copyright  2017 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_CAMERA_H_
#define _FSL_CAMERA_H_

#include "fsl_video_common.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief Camera control flags. */
enum _camera_flags
{
    kCAMERA_HrefActiveLow          = 0U,         /*!< HREF is active low. */
    kCAMERA_HrefActiveHigh         = (1U << 0U), /*!< HREF is active high. */
    kCAMERA_DataLatchOnFallingEdge = 0U,         /*!< Pixel data latched at falling edge of pixel clock. */
    kCAMERA_DataLatchOnRisingEdge  = (1U << 1U), /*!< Pixel data latched at rising edge of pixel clock. */
    kCAMERA_VsyncActiveLow         = 0U,         /*!< VSYNC is active low. */
    kCAMERA_VsyncActiveHigh        = (1U << 2U), /*!< VSYNC is active high. */
};

/*! @brief Camera interface type. */
typedef enum _camera_interface
{
    kCAMERA_InterfaceGatedClock,    /*!< HSYNC/HREF, VSYNC, and PIXCLK signals are used. */
    kCAMERA_InterfaceNonGatedClock, /*!< VSYNC, and PIXCLK signals are used. */
    kCAMERA_InterfaceCCIR656,       /*!< CCIR656 interface. */
    kCAMERA_InterfaceMIPI,          /*!< MIPI CSI2 interface. */
} camera_interface_t;

/*! @brief Camera configuration. */
typedef struct _camera_config
{
    video_pixel_format_t pixelFormat;    /*!< Pixel format. */
    uint8_t bytesPerPixel;               /*!< Byte per pixel. */
    uint32_t resolution;                 /*!< Resolution, see @ref video_resolution_t and @ref FSL_VIDEO_RESOLUTION. */
    uint16_t frameBufferLinePitch_Bytes; /*!< Frame buffer line pitch in bytes. */
    camera_interface_t interface;        /*!< Interface type. */
    uint32_t controlFlags;               /*!< Control flags, OR'ed value of @ref _camera_flags. */
    uint8_t framePerSec;                 /*!< Frame per second. */
    uint8_t mipiChannel;                 /*!< MIPI virtual channel. */
    uint8_t csiLanes;                    /*!< MIPI CSI data lanes number. */
} camera_config_t;

#endif /* _FSL_CAMERA_H_ */
