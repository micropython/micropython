/*
 * Copyright  2017 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_CAMERA_DEVICE_H_
#define _FSL_CAMERA_DEVICE_H_

#include "fsl_common.h"
#include "fsl_camera.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
typedef struct _camera_device_handle camera_device_handle_t;

/*! @brief Camera device control command. */
typedef enum _camera_device_cmd
{
    kCAMERA_DeviceAutoFocus, /*!< Auto focus. */
#define CAMERA_AUTO_FOCUS_ON 1
#define CAMERA_AUTO_FOCUS_OFF 0

    kCAMERA_DeviceLightMode, /*!< Light mode. */
#define CAMERA_LIGHT_MODE_AUTO 0
#define CAMERA_LIGHT_MODE_SUNNY 1
#define CAMERA_LIGHT_MODE_CLOUDY 2
#define CAMERA_LIGHT_MODE_OFFICE 3
#define CAMERA_LIGHT_MODE_HOME 4
#define CAMERA_LIGHT_MODE_NIGHT 5

    kCAMERA_DeviceSaturation, /*!< Saturation, pass in adjust value, such as -2, -1, 0, 1, 2... */
    kCAMERA_DeviceBrightness, /*!< Brightness, pass in adjust value, such as -2, -1, 0, 1, 2... */
    kCAMERA_DeviceContrast,   /*!< Contrast, pass in adjust value, such as -2, -1, 0, 1, 2... */

    kCAMERA_DeviceSpecialEffect,              /*!< Special effect. */
#define CAMERA_SPECIAL_EFFECT_NORMAL 0        /* Normal. */
#define CAMERA_SPECIAL_EFFECT_BW 1            /* B & W */
#define CAMERA_SPECIAL_EFFECT_SEPIA 2         /* Sepia. */
#define CAMERA_SPECIAL_EFFECT_BLUISH 3        /* Bluish. */
#define CAMERA_SPECIAL_EFFECT_REDISH 4        /* Redish. */
#define CAMERA_SPECIAL_EFFECT_GREENISH 5      /* Greenish. */
#define CAMERA_SPECIAL_EFFECT_NEGTIVE 6       /* Negtive. */
#define CAMERA_SPECIAL_EFFECT_OVER_EXPOSURE 7 /* OverExposure. */
#define CAMERA_SPECIAL_EFFECT_SOLARIZE 8      /* Solarize. */

    kCAMERA_DeviceNightMode,               /*!< Night mode. */
#define CAMERA_NIGHT_MODE_DISABLED 0       /* Disable. */
#define CAMERA_NIGHT_MODE_AUTO_FR_DIVBY2 1 /* Use automatic frame rate, max reduction to 1/2 frame rate. */
#define CAMERA_NIGHT_MODE_AUTO_FR_DIVBY4 4 /* Use automatic frame rate, max reduction to 1/4 frame rate. */
#define CAMERA_NIGHT_MODE_AUTO_FR_DIVBY8 8 /* Use automatic frame rate, max reduction to 1/8 frame rate. */
} camera_device_cmd_t;

/*! @brief Camera device operations. */
typedef struct _camera_device_operations
{
    status_t (*init)(camera_device_handle_t *handle, const camera_config_t *config);
    status_t (*deinit)(camera_device_handle_t *handle);
    status_t (*start)(camera_device_handle_t *handle);
    status_t (*stop)(camera_device_handle_t *handle);
    status_t (*control)(camera_device_handle_t *handle, camera_device_cmd_t cmd, int32_t arg);
    status_t (*init_ext)(camera_device_handle_t *handle, const camera_config_t *config, const void *specialConfig);
} camera_device_operations_t;

/*! @brief Camera device handle. */
struct _camera_device_handle
{
    void *resource;                        /*!< The resource used by the camera device, it is device specific. */
    const camera_device_operations_t *ops; /*!< The device related operations. */
};

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief Initialize the camera device.
 *
 * Initialize the camera device with the user defined configuration.
 *
 * @param handle Camera device handle.
 * @param config Pointer to the configuration.
 * @return Returns @ref kStatus_Success if initialize success, otherwise returns
 * error code.
 */
static inline status_t CAMERA_DEVICE_Init(camera_device_handle_t *handle, const camera_config_t *config)
{
    return handle->ops->init(handle, config);
}

/*!
 * @brief Initialize the camera device with more specific configuration.
 *
 * This is an externed API of @ref CAMERA_DEVICE_Init, it allow upper layer to
 * pass in device specific configuration. In this case, the
 * @ref CAMERA_DEVICE_Init uses default specific configuration.
 *
 * @param handle Camera device handle.
 * @param config Pointer to the configuration.
 * @param specialConfig Pointer to the device specific configuration.
 * @return Returns @ref kStatus_Success if initialize success, otherwise returns
 * error code.
 */
static inline status_t CAMERA_DEVICE_InitExt(camera_device_handle_t *handle,
                                             const camera_config_t *config,
                                             const void *specialConfig)
{
    return handle->ops->init_ext(handle, config, specialConfig);
}

/*!
 * @brief Deinitialize the camera device.
 *
 * @param handle Camera device handle.
 * @return Returns @ref kStatus_Success if success, otherwise returns error code.
 */
static inline status_t CAMERA_DEVICE_Deinit(camera_device_handle_t *handle)
{
    return handle->ops->deinit(handle);
}

/*!
 * @brief Control the camera device.
 *
 * @param handle Camera device handle.
 * @param cmd Camera device command
 * @param arg Argument for the command.
 * @return Returns @ref kStatus_Success if success, otherwise returns error code.
 */
static inline status_t CAMERA_DEVICE_Control(camera_device_handle_t *handle, camera_device_cmd_t cmd, int32_t arg)
{
    return handle->ops->control(handle, cmd, arg);
}

/*!
 * @brief Start the camera device to output data.
 *
 * @param handle Camera device handle.
 * @return Returns @ref kStatus_Success if success, otherwise returns error code.
 */
static inline status_t CAMERA_DEVICE_Start(camera_device_handle_t *handle)
{
    return handle->ops->start(handle);
}

/*!
 * @brief Stop the camera device outputing data.
 *
 * @param handle Camera device handle.
 * @return Returns @ref kStatus_Success if success, otherwise returns error code.
 */
static inline status_t CAMERA_DEVICE_Stop(camera_device_handle_t *handle)
{
    return handle->ops->stop(handle);
}

#if defined(__cplusplus)
}
#endif

#endif /* _FSL_CAMERA_DEVICE_H_ */
