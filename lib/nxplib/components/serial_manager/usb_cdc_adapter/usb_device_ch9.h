/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __USB_DEVICE_CH9_H__
#define __USB_DEVICE_CH9_H__

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*!
 * @addtogroup usb_device_ch9
 * @{
 */
/*! @brief Defines USB device status size when the host request to get device status */
#define USB_DEVICE_STATUS_SIZE (0x02U)

/*! @brief Defines USB device interface status size when the host request to get interface status */
#define USB_INTERFACE_STATUS_SIZE (0x02U)

/*! @brief Defines USB device endpoint status size when the host request to get endpoint status */
#define USB_ENDPOINT_STATUS_SIZE (0x02U)

/*! @brief Defines USB device configuration size when the host request to get current configuration */
#define USB_CONFIGURE_SIZE (0X01U)

/*! @brief Defines USB device interface alternate setting size when the host request to get interface alternate setting
 */
#define USB_INTERFACE_SIZE (0X01U)

/*! @brief Defines USB device status mask */
#define USB_GET_STATUS_DEVICE_MASK (0x03U)

/*! @brief Defines USB device interface status mask */
#define USB_GET_STATUS_INTERFACE_MASK (0x03U)

/*! @brief Defines USB device endpoint status mask */
#define USB_GET_STATUS_ENDPOINT_MASK (0x03U)

/*! @brief Control read and write sequence */
typedef enum _usb_device_control_read_write_sequence
{
    kUSB_DeviceControlPipeSetupStage = 0U, /*!< Setup stage */
    kUSB_DeviceControlPipeDataStage,       /*!< Data stage */
    kUSB_DeviceControlPipeStatusStage,     /*!< status stage */
} usb_device_control_read_write_sequence_t;

#if defined(__cplusplus)
extern "C" {
#endif

/*******************************************************************************
 * API
 ******************************************************************************/

/*!
 * @brief Initializes the control pipes.
 *
 * The function is used to initialize the control pipes. This function should be called when event
 * kUSB_DeviceEventBusReset is received.
 *
 * @param[in] handle      The device handle.
 * @param[in] param       The event parameter.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
extern usb_status_t USB_DeviceControlPipeInit(usb_device_handle handle, void *param);

#if defined(__cplusplus)
}
#endif

/*! @}*/

#endif /* __USB_DEVICE_CH9_H__ */
