/*
 * Copyright (c) 2015 - 2016, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __USB_DEVICE_CLASS_H__
#define __USB_DEVICE_CLASS_H__

/*!
 * @addtogroup usb_device_class_driver
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief Macro to define class handle */
#define class_handle_t uint32_t

/*! @brief Available class types. */
typedef enum _usb_usb_device_class_type
{
    kUSB_DeviceClassTypeHid = 1U,
    kUSB_DeviceClassTypeCdc,
    kUSB_DeviceClassTypeMsc,
    kUSB_DeviceClassTypeAudio,
    kUSB_DeviceClassTypePhdc,
    kUSB_DeviceClassTypeVideo,
    kUSB_DeviceClassTypePrinter,
    kUSB_DeviceClassTypeDfu,
    kUSB_DeviceClassTypeCcid,
} usb_device_class_type_t;

/*! @brief Available common class events. */
typedef enum _usb_device_class_event
{
    kUSB_DeviceClassEventClassRequest = 1U,
    kUSB_DeviceClassEventDeviceReset,
    kUSB_DeviceClassEventSetConfiguration,
    kUSB_DeviceClassEventSetInterface,
    kUSB_DeviceClassEventSetEndpointHalt,
    kUSB_DeviceClassEventClearEndpointHalt,
} usb_device_class_event_t;

/*!
 * @brief Obtains the endpoint data structure.
 *
 * Define the endpoint data structure.
 *
 */
typedef struct _usb_device_endpoint_struct
{
    uint8_t endpointAddress; /*!< Endpoint address*/
    uint8_t transferType;    /*!< Endpoint transfer type*/
    uint16_t maxPacketSize;  /*!< Endpoint maximum packet size */
    uint8_t interval;        /*!< Endpoint interval*/
} usb_device_endpoint_struct_t;

/*!
* @brief Obtains the endpoint group.
*
* Structure representing endpoints and the number of endpoints that the user wants.
*
*/
typedef struct _usb_device_endpoint_list
{
    uint8_t count;                          /*!< How many endpoints in current interface*/
    usb_device_endpoint_struct_t *endpoint; /*!< Endpoint structure list*/
} usb_device_endpoint_list_t;

/*!
 * @brief Obtains the interface list data structure.
 *
 * Structure representing an interface.
 *
 */
typedef struct _usb_device_interface_struct
{
    uint8_t alternateSetting;                /*!< Alternate setting number*/
    usb_device_endpoint_list_t endpointList; /*!< Endpoints of the interface*/
    void *classSpecific;                     /*!< Class specific structure handle*/
} usb_device_interface_struct_t;

/*!
 * @brief Obtains the interface data structure.
 *
 * Structure representing interface.
 *
 */
typedef struct _usb_device_interfaces_struct
{
    uint8_t classCode;                        /*!< Class code of the interface*/
    uint8_t subclassCode;                     /*!< Subclass code of the interface*/
    uint8_t protocolCode;                     /*!< Protocol code of the interface*/
    uint8_t interfaceNumber;                  /*!< Interface number*/
    usb_device_interface_struct_t *interface; /*!< Interface structure list*/
    uint8_t count;                            /*!< Number of interfaces in the current interface*/
} usb_device_interfaces_struct_t;

/*!
 * @brief Obtains the interface group.
 *
 * Structure representing how many interfaces in one class type.
 *
 */
typedef struct _usb_device_interface_list
{
    uint8_t count;                              /*!< Number of interfaces of the class*/
    usb_device_interfaces_struct_t *interfaces; /*!< All interfaces*/
} usb_device_interface_list_t;

/*!
 * @brief Obtains the class data structure.
 *
 * Structure representing how many configurations in one class type.
 *
 */
typedef struct _usb_device_class_struct
{
    usb_device_interface_list_t *interfaceList; /*!< Interfaces of the class*/
    usb_device_class_type_t type;               /*!< Class type*/
    uint8_t configurations;                     /*!< Number of configurations of the class*/
} usb_device_class_struct_t;

/*callback function pointer structure for application to provide class parameters*/
typedef usb_status_t (*usb_device_class_callback_t)(class_handle_t classHandle,
                                                    uint32_t callbackEvent,
                                                    void *eventParam);

/*!
 * @brief Obtains the device class information structure.
 *
 * Structure representing the device class information. This structure only can be stored in RAM space.
 *
 */
typedef struct _usb_device_class_config_struct
{
    usb_device_class_callback_t classCallback;  /*!< Class callback function to handle the device status-related event
                                                   for the specified type of class*/
    class_handle_t classHandle;                 /*!< The class handle of the class, filled by the common driver.*/
    usb_device_class_struct_t *classInfomation; /*!< Detailed information of the class*/
} usb_device_class_config_struct_t;

/*!
 * @brief Obtains the device class configuration structure.
 *
 * Structure representing the device class configuration information.
 *
 */
typedef struct _usb_device_class_config_list_struct
{
    usb_device_class_config_struct_t *config; /*!< Array of class configuration structures */
    usb_device_callback_t deviceCallback;     /*!< Device callback function */
    uint8_t count;                            /*!< Number of class supported */
} usb_device_class_config_list_struct_t;

/*!
 * @brief Obtains the control request structure.
 *
 * This structure is used to pass the control request information.
 * The structure is used in following two cases.
 * 1. Case one, the host wants to send data to the device in the control data stage: @n
 *         a. If a setup packet is received, the structure is used to pass the setup packet data and wants to get the
 * buffer to receive data sent from the host.
 *            The field isSetup is 1.
 *            The length is the requested buffer length.
 *            The buffer is filled by the class or application by using the valid buffer address.
 *            The setup is the setup packet address.
 *         b. If the data received is sent by the host, the structure is used to pass the data buffer address and the
 * data
 * length sent by the host.
 *            In this way, the field isSetup is 0.
 *            The buffer is the address of the data sent from the host.
 *            The length is the received data length.
 *            The setup is the setup packet address. @n
 * 2. Case two, the host wants to get data from the device in control data stage: @n
 *            If the setup packet is received, the structure is used to pass the setup packet data and wants to get the
 * data buffer address to send data to the host.
 *            The field isSetup is 1.
 *            The length is the requested data length.
 *            The buffer is filled by the class or application by using the valid buffer address.
 *            The setup is the setup packet address.
 *
 */
typedef struct _usb_device_control_request_struct
{
    usb_setup_struct_t *setup; /*!< The pointer of the setup packet data. */
    uint8_t *buffer;           /*!< Pass the buffer address. */
    uint32_t length;           /*!< Pass the buffer length or requested length. */
    uint8_t isSetup;           /*!< Indicates whether a setup packet is received. */
} usb_device_control_request_struct_t;

/*! @brief Obtains the control get descriptor request common structure. */
typedef struct _usb_device_get_descriptor_common_struct
{
    uint8_t *buffer; /*!< Pass the buffer address. */
    uint32_t length; /*!< Pass the buffer length. */
} usb_device_get_descriptor_common_struct_t;

/*! @brief Obtains the control get device descriptor request structure. */
typedef struct _usb_device_get_device_descriptor_struct
{
    uint8_t *buffer; /*!< Pass the buffer address. */
    uint32_t length; /*!< Pass the buffer length. */
} usb_device_get_device_descriptor_struct_t;

/*! @brief Obtains the control get device qualifier descriptor request structure. */
typedef struct _usb_device_get_device_qualifier_descriptor_struct
{
    uint8_t *buffer; /*!< Pass the buffer address. */
    uint32_t length; /*!< Pass the buffer length. */
} usb_device_get_device_qualifier_descriptor_struct_t;

/*! @brief Obtains the control get configuration descriptor request structure. */
typedef struct _usb_device_get_configuration_descriptor_struct
{
    uint8_t *buffer;       /*!< Pass the buffer address. */
    uint32_t length;       /*!< Pass the buffer length. */
    uint8_t configuration; /*!< The configuration number. */
} usb_device_get_configuration_descriptor_struct_t;

/*! @brief Obtains the control get bos descriptor request structure. */
typedef struct _usb_device_get_bos_descriptor_struct
{
    uint8_t *buffer; /*!< Pass the buffer address. */
    uint32_t length; /*!< Pass the buffer length. */
} usb_device_get_bos_descriptor_struct_t;

/*! @brief Obtains the control get string descriptor request structure. */
typedef struct _usb_device_get_string_descriptor_struct
{
    uint8_t *buffer;     /*!< Pass the buffer address. */
    uint32_t length;     /*!< Pass the buffer length. */
    uint16_t languageId; /*!< Language ID. */
    uint8_t stringIndex; /*!< String index. */
} usb_device_get_string_descriptor_struct_t;

/*! @brief Obtains the control get HID descriptor request structure. */
typedef struct _usb_device_get_hid_descriptor_struct
{
    uint8_t *buffer;         /*!< Pass the buffer address. */
    uint32_t length;         /*!< Pass the buffer length. */
    uint8_t interfaceNumber; /*!< The interface number. */
} usb_device_get_hid_descriptor_struct_t;

/*! @brief Obtains the control get HID report descriptor request structure. */
typedef struct _usb_device_get_hid_report_descriptor_struct
{
    uint8_t *buffer;         /*!< Pass the buffer address. */
    uint32_t length;         /*!< Pass the buffer length. */
    uint8_t interfaceNumber; /*!< The interface number. */
} usb_device_get_hid_report_descriptor_struct_t;

/*! @brief Obtains the control get HID physical descriptor request structure. */
typedef struct _usb_device_get_hid_physical_descriptor_struct
{
    uint8_t *buffer;         /*!< Pass the buffer address. */
    uint32_t length;         /*!< Pass the buffer length. */
    uint8_t index;           /*!< Physical index */
    uint8_t interfaceNumber; /*!< The interface number. */
} usb_device_get_hid_physical_descriptor_struct_t;

/*! @brief Obtains the control get descriptor request common union. */
typedef union _usb_device_get_descriptor_common_union
{
    usb_device_get_descriptor_common_struct_t commonDescriptor; /*!< Common structure. */
    usb_device_get_device_descriptor_struct_t deviceDescriptor; /*!< The structure to get device descriptor. */
    usb_device_get_device_qualifier_descriptor_struct_t
        deviceQualifierDescriptor; /*!< The structure to get device qualifier descriptor. */
    usb_device_get_configuration_descriptor_struct_t
        configurationDescriptor;                                /*!< The structure to get configuration descriptor. */
    usb_device_get_string_descriptor_struct_t stringDescriptor; /*!< The structure to get string descriptor. */
    usb_device_get_hid_descriptor_struct_t hidDescriptor;       /*!< The structure to get HID descriptor. */
    usb_device_get_hid_report_descriptor_struct_t
        hidReportDescriptor; /*!< The structure to get HID report descriptor. */
    usb_device_get_hid_physical_descriptor_struct_t
        hidPhysicalDescriptor; /*!< The structure to get HID physical descriptor. */
} usb_device_get_descriptor_common_union_t;

/*! @brief Define function type for class device instance initialization */
typedef usb_status_t (*usb_device_class_init_call_t)(uint8_t controllerId,
                                                     usb_device_class_config_struct_t *classConfig,
                                                     class_handle_t *classHandle);
/*! @brief Define function type for class device instance deinitialization, internal */
typedef usb_status_t (*usb_device_class_deinit_call_t)(class_handle_t handle);
/*! @brief Define function type for class device instance Event change */
typedef usb_status_t (*usb_device_class_event_callback_t)(void *classHandle, uint32_t event, void *param);

/*! @brief Define class driver interface structure. */
typedef struct _usb_device_class_map
{
    usb_device_class_init_call_t classInit;     /*!< Class driver initialization- entry  of the class driver */
    usb_device_class_deinit_call_t classDeinit; /*!< Class driver de-initialization*/
    usb_device_class_event_callback_t classEventCallback; /*!< Class driver event callback*/
    usb_device_class_type_t type;                         /*!< Class type*/
} usb_device_class_map_t;

/*! @brief Structure holding common class state information */
typedef struct _usb_device_common_class_struct
{
    usb_device_handle handle;                          /*!< USB device handle*/
    usb_device_class_config_list_struct_t *configList; /*!< USB device configure list*/
    uint8_t *setupBuffer;                              /*!< Setup packet data buffer*/
    uint16_t standardTranscationBuffer;                /*!<
                                                        * This variable is used in:
                                                        *           get status request
                                                        *           get configuration request
                                                        *           get interface request
                                                        *           set interface request
                                                        *           get sync frame request
                                                        */
    uint8_t controllerId;                              /*!< Controller ID*/
} usb_device_common_class_struct_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief Initializes the common class and the supported classes.
 *
 * This function is used to initialize the common class and the supported classes.
 *
 * @param[in] controllerId   The controller ID of the USB IP. See the enumeration #usb_controller_index_t.
 * @param[in] configList     The class configurations. The pointer must point to the global variable.
 *                           See the structure #usb_device_class_config_list_struct_t.
 * @param[out] handle        A parameter used to return pointer of the device handle to the caller.
 *                           The value of the parameter is a pointer to the device handle. This design is used to
 *                           make a simple device align with the composite device. For the composite device, there are
 * many
 *                           kinds of class handles. However, there is only one device handle. Therefore, the handle
 * points to
 *                           a device instead of a class. The class handle can be received from the
 *                           #usb_device_class_config_struct_t::classHandle after the function successfully.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
usb_status_t USB_DeviceClassInit(uint8_t controllerId,
                                 usb_device_class_config_list_struct_t *configList,
                                 usb_device_handle *handle);

/*!
 * @brief Deinitializes the common class and the supported classes.
 *
 * This function is used to deinitialize the common class and the supported classes.
 *
 * @param[in] controllerId   The controller ID of the USB IP. See the enumeration #usb_controller_index_t.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
usb_status_t USB_DeviceClassDeinit(uint8_t controllerId);

/*!
 * @brief Gets the USB bus speed.
 *
 * This function is used to get the USB bus speed.
 *
 * @param[in] controllerId   The controller ID of the USB IP. See the enumeration #usb_controller_index_t.
 * @param[out] speed           It is an OUT parameter, which returns the current speed of the controller.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
usb_status_t USB_DeviceClassGetSpeed(uint8_t controllerId, uint8_t *speed);

/*!
 * @brief Handles the event passed to the class drivers.
 *
 * This function handles the event passed to the class drivers.
 *
 * @param[in] handle          The device handle received from the #USB_DeviceInit.
 * @param[in] event           The event codes. See the enumeration #usb_device_class_event_t.
 * @param[in,out] param           The parameter type is determined by the event code.
 *
 * @return A USB error code or kStatus_USB_Success.
 * @retval kStatus_USB_Success              A valid request has been handled.
 * @retval kStatus_USB_InvalidParameter     The device handle not be found.
 * @retval kStatus_USB_InvalidRequest       The request is invalid, and the control pipe is stalled by the caller.
 */
usb_status_t USB_DeviceClassEvent(usb_device_handle handle, usb_device_class_event_t event, void *param);

/*!
 * @brief Handles the common class callback.
 *
 * This function handles the common class callback.
 *
 * @param[in] handle          The device handle received from the #USB_DeviceInit.
 * @param[in] event           The event codes. See the enumeration #usb_device_event_t.
 * @param[in,out] param           The parameter type is determined by the event code.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
usb_status_t USB_DeviceClassCallback(usb_device_handle handle, uint32_t event, void *param);

/*!
 * @brief Gets the device handle according to the controller ID.
 *
 * This function gets the device handle according to the controller ID.
 *
 * @param[in] controllerId   The controller ID of the USB IP. See the enumeration #usb_controller_index_t.
 * @param[out] handle          An out parameter used to return the pointer of the device handle to the caller.
 *
 * @retval kStatus_USB_Success              Get device handle successfully.
 * @retval kStatus_USB_InvalidParameter     The device handle can't be found.
 */
usb_status_t USB_DeviceClassGetDeviceHandle(uint8_t controllerId, usb_device_handle *handle);

#if defined(__cplusplus)
}
#endif

/*! @}*/

#endif /* __USB_DEVICE_CLASS_H__ */
