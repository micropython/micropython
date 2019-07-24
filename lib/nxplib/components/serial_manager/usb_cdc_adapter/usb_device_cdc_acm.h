/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _USB_DEVICE_CDC_ACM_H_
#define _USB_DEVICE_CDC_ACM_H_

/*!
 * @addtogroup cdc_acm
 * @{
 */

/*******************************************************************************
* Definitions
******************************************************************************/
#define USB_DEVICE_CONFIG_CDC_ACM_MAX_INSTANCE (1)   /*!< The maximum number of CDC device instance. */
#define USB_DEVICE_CONFIG_CDC_COMM_CLASS_CODE (0x02) /*!< The CDC communication class code. */
#define USB_DEVICE_CONFIG_CDC_DATA_CLASS_CODE (0x0A) /*!< The CDC data class code. */

#define USB_DEVICE_CDC_REQUEST_SEND_ENCAPSULATED_COMMAND \
    (0x00) /*!< The CDC class request code for SEND_ENCAPSULATED_COMMAND. */
#define USB_DEVICE_CDC_REQUEST_GET_ENCAPSULATED_RESPONSE \
    (0x01)                                             /*!< The CDC class request code for GET_ENCAPSULATED_RESPONSE. */
#define USB_DEVICE_CDC_REQUEST_SET_COMM_FEATURE (0x02) /*!< The CDC class request code for SET_COMM_FEATURE. */
#define USB_DEVICE_CDC_REQUEST_GET_COMM_FEATURE (0x03) /*!< The CDC class request code for GET_COMM_FEATURE. */
#define USB_DEVICE_CDC_REQUEST_CLEAR_COMM_FEATURE (0x04) /*!< The CDC class request code for CLEAR_COMM_FEATURE. */
#define USB_DEVICE_CDC_REQUEST_SET_AUX_LINE_STATE (0x10) /*!< The CDC class request code for SET_AUX_LINE_STATE. */
#define USB_DEVICE_CDC_REQUEST_SET_HOOK_STATE (0x11)     /*!< The CDC class request code for SET_HOOK_STATE. */
#define USB_DEVICE_CDC_REQUEST_PULSE_SETUP (0x12)        /*!< The CDC class request code for PULSE_SETUP. */
#define USB_DEVICE_CDC_REQUEST_SEND_PULSE (0x13)         /*!< The CDC class request code for SEND_PULSE. */
#define USB_DEVICE_CDC_REQUEST_SET_PULSE_TIME (0x14)     /*!< The CDC class request code for SET_PULSE_TIME. */
#define USB_DEVICE_CDC_REQUEST_RING_AUX_JACK (0x15)      /*!< The CDC class request code for RING_AUX_JACK. */
#define USB_DEVICE_CDC_REQUEST_SET_LINE_CODING (0x20)    /*!< The CDC class request code for SET_LINE_CODING. */
#define USB_DEVICE_CDC_REQUEST_GET_LINE_CODING (0x21)    /*!< The CDC class request code for GET_LINE_CODING. */
#define USB_DEVICE_CDC_REQUEST_SET_CONTROL_LINE_STATE \
    (0x22)                                                /*!< The CDC class request code for SET_CONTROL_LINE_STATE. */
#define USB_DEVICE_CDC_REQUEST_SEND_BREAK (0x23)          /*!< The CDC class request code for SEND_BREAK. */
#define USB_DEVICE_CDC_REQUEST_SET_RINGER_PARAMS (0x30)   /*!< The CDC class request code for SET_RINGER_PARAMS. */
#define USB_DEVICE_CDC_REQUEST_GET_RINGER_PARAMS (0x31)   /*!< The CDC class request code for GET_RINGER_PARAMS. */
#define USB_DEVICE_CDC_REQUEST_SET_OPERATION_PARAM (0x32) /*!< The CDC class request code for SET_OPERATION_PARAM. */
#define USB_DEVICE_CDC_REQUEST_GET_OPERATION_PARAM (0x33) /*!< The CDC class request code for GET_OPERATION_PARAM. */
#define USB_DEVICE_CDC_REQUEST_SET_LINE_PARAMS (0x34)     /*!< The CDC class request code for SET_LINE_PARAMS. */
#define USB_DEVICE_CDC_REQUEST_GET_LINE_PARAMS (0x35)     /*!< The CDC class request code for GET_LINE_PARAMS. */
#define USB_DEVICE_CDC_REQUEST_DIAL_DIGITS (0x36)         /*!< The CDC class request code for DIAL_DIGITS. */
#define USB_DEVICE_CDC_REQUEST_SET_UNIT_PARAMETER (0x37)  /*!< The CDC class request code for SET_UNIT_PARAMETER. */
#define USB_DEVICE_CDC_REQUEST_GET_UNIT_PARAMETER (0x38)  /*!< The CDC class request code for GET_UNIT_PARAMETER. */
#define USB_DEVICE_CDC_REQUEST_CLEAR_UNIT_PARAMETER \
    (0x39) /*!< The CDC class request code for CLEAR_UNIT_PARAMETER. */
#define USB_DEVICE_CDC_REQUEST_SET_ETHERNET_MULTICAST_FILTERS \
    (0x40) /*!< The CDC class request code for SET_ETHERNET_MULTICAST_FILTERS. */
#define USB_DEVICE_CDC_REQUEST_SET_ETHERNET_POW_PATTER_FILTER \
    (0x41) /*!< The CDC class request code for SET_ETHERNET_POW_PATTER_FILTER. */
#define USB_DEVICE_CDC_REQUEST_GET_ETHERNET_POW_PATTER_FILTER \
    (0x42) /*!< The CDC class request code for GET_ETHERNET_POW_PATTER_FILTER. */
#define USB_DEVICE_CDC_REQUEST_SET_ETHERNET_PACKET_FILTER \
    (0x43) /*!< The CDC class request code for SET_ETHERNET_PACKET_FILTER. */
#define USB_DEVICE_CDC_REQUEST_GET_ETHERNET_STATISTIC \
    (0x44)                                                /*!< The CDC class request code for GET_ETHERNET_STATISTIC. */
#define USB_DEVICE_CDC_REQUEST_SET_ATM_DATA_FORMAT (0x50) /*!< The CDC class request code for SET_ATM_DATA_FORMAT. */
#define USB_DEVICE_CDC_REQUEST_GET_ATM_DEVICE_STATISTICS \
    (0x51) /*!< The CDC class request code for GET_ATM_DEVICE_STATISTICS. */
#define USB_DEVICE_CDC_REQUEST_SET_ATM_DEFAULT_VC (0x52) /*!< The CDC class request code for SET_ATM_DEFAULT_VC. */
#define USB_DEVICE_CDC_REQUEST_GET_ATM_VC_STATISTICS \
    (0x53) /*!< The CDC class request code for GET_ATM_VC_STATISTICS. */
#define USB_DEVICE_CDC_REQUEST_MDLM_SPECIFIC_REQUESTS_MASK \
    (0x7F) /*!< The CDC class request code for MDLM_SPECIFIC_REQUESTS_MASK. */

#define USB_DEVICE_CDC_NOTIF_NETWORK_CONNECTION (0x00)  /*!< The CDC class notify code for NETWORK_CONNECTION. */
#define USB_DEVICE_CDC_NOTIF_RESPONSE_AVAIL (0x01)      /*!< The CDC class notify code for RESPONSE_AVAIL. */
#define USB_DEVICE_CDC_NOTIF_AUX_JACK_HOOK_STATE (0x08) /*!< The CDC class notify code for AUX_JACK_HOOK_STATE. */
#define USB_DEVICE_CDC_NOTIF_RING_DETECT (0x09)         /*!< The CDC class notify code for RING_DETECT. */
#define USB_DEVICE_CDC_NOTIF_SERIAL_STATE (0x20)        /*!< The CDC class notify code for SERIAL_STATE. */
#define USB_DEVICE_CDC_NOTIF_CALL_STATE_CHANGE (0x28)   /*!< The CDC class notify code for CALL_STATE_CHANGE. */
#define USB_DEVICE_CDC_NOTIF_LINE_STATE_CHANGE (0x29)   /*!< The CDC class notify code for LINE_STATE_CHANGE. */
#define USB_DEVICE_CDC_NOTIF_CONNECTION_SPEED_CHANGE \
    (0x2A) /*!< The CDC class notify code for CONNECTION_SPEED_CHANGE. */

#define USB_DEVICE_CDC_FEATURE_ABSTRACT_STATE (0x01)  /*!< The CDC class feature select code for ABSTRACT_STATE. */
#define USB_DEVICE_CDC_FEATURE_COUNTRY_SETTING (0x02) /*!< The CDC class feature select code for COUNTRY_SETTING. */

#define USB_DEVICE_CDC_CONTROL_SIG_BITMAP_CARRIER_ACTIVATION \
    (0x02) /*!< The CDC class control signal bitmap value for CARRIER_ACTIVATION. */
#define USB_DEVICE_CDC_CONTROL_SIG_BITMAP_DTE_PRESENCE \
    (0x01)                                           /*!< The CDC class control signal bitmap value for DTE_PRESENCE. */
#define USB_DEVICE_CDC_UART_STATE_RX_CARRIER (0x01)  /*!< The UART state bitmap value of RX_CARRIER. */
#define USB_DEVICE_CDC_UART_STATE_TX_CARRIER (0x02)  /*!< The UART state bitmap value of TX_CARRIER. */
#define USB_DEVICE_CDC_UART_STATE_BREAK (0x04)       /*!< The UART state bitmap value of BREAK. */
#define USB_DEVICE_CDC_UART_STATE_RING_SIGNAL (0x08) /*!< The UART state bitmap value of RING_SIGNAL. */
#define USB_DEVICE_CDC_UART_STATE_FRAMING (0x10)     /*!< The UART state bitmap value of FRAMING. */
#define USB_DEVICE_CDC_UART_STATE_PARITY (0x20)      /*!< The UART state bitmap value of PARITY. */
#define USB_DEVICE_CDC_UART_STATE_OVERRUN (0x40)     /*!< The UART state bitmap value of OVERRUN. */

/*! @brief Definition of CDC class event. */
typedef enum _usb_device_cdc_acm_event
{
    kUSB_DeviceCdcEventSendResponse = 0x01,     /*!< This event indicates the bulk send transfer is complete or cancelled etc. */
    kUSB_DeviceCdcEventRecvResponse,            /*!< This event indicates the bulk receive transfer is complete or cancelled etc.. */
    kUSB_DeviceCdcEventSerialStateNotif,        /*!< This event indicates the serial state has been sent to the host. */
    kUSB_DeviceCdcEventSendEncapsulatedCommand, /*!< This event indicates the device received the
                                                   SEND_ENCAPSULATED_COMMAND request. */
    kUSB_DeviceCdcEventGetEncapsulatedResponse, /*!< This event indicates the device received the
                                                   GET_ENCAPSULATED_RESPONSE request. */
    kUSB_DeviceCdcEventSetCommFeature,   /*!< This event indicates the device received the SET_COMM_FEATURE request. */
    kUSB_DeviceCdcEventGetCommFeature,   /*!< This event indicates the device received the GET_COMM_FEATURE request. */
    kUSB_DeviceCdcEventClearCommFeature, /*!< This event indicates the device received the CLEAR_COMM_FEATURE request.
                                            */
    kUSB_DeviceCdcEventGetLineCoding,    /*!< This event indicates the device received the GET_LINE_CODING request. */
    kUSB_DeviceCdcEventSetLineCoding,    /*!< This event indicates the device received the SET_LINE_CODING request. */
    kUSB_DeviceCdcEventSetControlLineState, /*!< This event indicates the device received the SET_CONTRL_LINE_STATE
                                               request. */
    kUSB_DeviceCdcEventSendBreak            /*!< This event indicates the device received the SEND_BREAK request. */
} usb_device_cdc_acm_event_t;

/*! @brief Definition of parameters for CDC ACM request. */
typedef struct _usb_device_cdc_acm_request_param_struct
{
    uint8_t **buffer;        /*!< The pointer to the address of the buffer for CDC class request. */
    uint32_t *length;        /*!< The pointer to the length of the buffer for CDC class request. */
    uint16_t interfaceIndex; /*!< The interface index of the setup packet. */
    uint16_t setupValue;     /*!< The wValue field of the setup packet. */
    uint8_t isSetup;         /*!< The flag indicates if it is a setup packet, 1: yes, 0: no. */
} usb_device_cdc_acm_request_param_struct_t;

/*! @brief Definition of pipe structure. */
typedef struct _usb_device_cdc_acm_pipe
{
    usb_osa_mutex_handle mutex; /*!< The mutex of the pipe. */
    uint8_t *pipeDataBuffer;      /*!< pipe data buffer backup when stall */
    uint32_t pipeDataLen;         /*!< pipe data length backup when stall  */
    uint8_t pipeStall;            /*!< pipe is stall  */
    uint8_t ep;                 /*!< The endpoint number of the pipe. */
    uint8_t isBusy;             /*!< 1: The pipe is transferring packet, 0: The pipe is idle. */
} usb_device_cdc_acm_pipe_t;

/*! @brief Definition of structure for CDC ACM device. */
typedef struct _usb_device_cdc_acm_struct
{
    usb_device_handle handle;                           /*!< The handle of the USB device. */
    usb_device_class_config_struct_t *configStruct;     /*!< The class configure structure. */
    usb_device_interface_struct_t *commInterfaceHandle; /*!< The CDC communication interface handle. */
    usb_device_interface_struct_t *dataInterfaceHandle; /*!< The CDC data interface handle. */
    usb_device_cdc_acm_pipe_t bulkIn;                   /*!< The bulk in pipe for sending packet to host. */
    usb_device_cdc_acm_pipe_t bulkOut;                  /*!< The bulk out pipe for receiving packet from host. */
    usb_device_cdc_acm_pipe_t interruptIn; /*!< The interrupt in pipe for notifying the device state to host. */
    uint8_t configuration;                 /*!< The current configuration value. */
    uint8_t interfaceNumber;               /*!< The current interface number. */
    uint8_t alternate;                     /*!< The alternate setting value of the interface. */
    uint8_t hasSentState; /*!< 1: The device has primed the state in interrupt pipe, 0: Not primed the state. */
} usb_device_cdc_acm_struct_t;

/*******************************************************************************
* API
******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name USB CDC ACM Class Driver
 * @{
 */
/*!
 * @brief Initializes the USB CDC ACM class.
 *
 * This function obtains a USB device handle according to the controller ID, initializes the CDC ACM class
 * with the class configure parameters and creates the mutex for each pipe.
 *
 * @param controllerId The ID of the controller. The value can be chosen from the kUSB_ControllerKhci0,
 *  kUSB_ControllerKhci1, kUSB_ControllerEhci0, or kUSB_ControllerEhci1.
 * @param config The user configuration structure of type usb_device_class_config_struct_t. The user
 *  populates the members of this structure and passes the pointer of this structure
 *  into this function.
 * @param handle  It is out parameter. The class handle of the CDC ACM class.
 * @return A USB error code or kStatus_USB_Success.
 * @retval kStatus_USB_Success The CDC ACM class is initialized successfully.
 * @retval kStatus_USB_Busy No CDC ACM device handle available for allocation.
 * @retval kStatus_USB_InvalidHandle The CDC ACM device handle allocation failure.
 * @retval kStatus_USB_InvalidParameter The USB device handle allocation failure.
 */
extern usb_status_t USB_DeviceCdcAcmInit(uint8_t controllerId,
                                         usb_device_class_config_struct_t *config,
                                         class_handle_t *handle);
/*!
 * @brief Deinitializes the USB CDC ACM class.
 *
 * This function destroys the mutex for each pipe, deinitializes each endpoint of the CDC ACM class and frees
 * the CDC ACM class handle.
 *
 * @param handle The class handle of the CDC ACM class.
 * @return A USB error code or kStatus_USB_Success.
 * @retval kStatus_USB_Success The CDC ACM class is de-initialized successfully.
 * @retval kStatus_USB_Error The endpoint deinitialization failure.
 * @retval kStatus_USB_InvalidHandle The CDC ACM device handle or the CDC ACM class handle is invalid.
 * @retval kStatus_USB_InvalidParameter The endpoint number of the CDC ACM class handle is invalid.
 */
extern usb_status_t USB_DeviceCdcAcmDeinit(class_handle_t handle);
/*!
 * @brief Handles the CDC ACM class event.
 *
 * This function responds to various events including the common device events and the class-specific events.
 * For class-specific events, it calls the class callback defined in the application to deal with the class-specific
 * event.
 *
 * @param handle The class handle of the CDC ACM class.
 * @param event The event type.
 * @param param The class handle of the CDC ACM class.
 * @return A USB error code or kStatus_USB_Success.
 * @retval kStatus_USB_Success The CDC ACM class is de-initialized successfully.
 * @retval kStatus_USB_Error The configure structure of the CDC ACM class handle is invalid.
 * @retval kStatus_USB_InvalidHandle The CDC ACM device handle or the CDC ACM class handle is invalid.
 * @retval kStatus_USB_InvalidParameter The endpoint number of the CDC ACM class handle is invalid.
 * @retval Others The error code returned by class callback in application.
 */
extern usb_status_t USB_DeviceCdcAcmEvent(void *handle, uint32_t event, void *param);

/*!
 * @brief Primes the endpoint to send packet to host.
 *
 * This function checks whether the endpoint is sending packet, then it primes the endpoint
 * with the buffer address and the buffer length if the pipe is not busy. Otherwise, it ignores this transfer by
 * returning an error code.
 *
 * @param handle The class handle of the CDC ACM class.
 * @param ep The endpoint number of the transfer.
 * @param buffer The pointer to the buffer to be transferred.
 * @param length The length of the buffer to be transferred.
 * @return A USB error code or kStatus_USB_Success.
 * @retval kStatus_USB_Success Prime to send packet successfully.
 * @retval kStatus_USB_Busy The endpoint is busy in transferring.
 * @retval kStatus_USB_InvalidHandle The CDC ACM device handle or the CDC ACM class handle is invalid.
 * @retval kStatus_USB_ControllerNotFound The controller interface is invalid.
 *
 * @note The function can only be called in the same context. 
 */
extern usb_status_t USB_DeviceCdcAcmSend(class_handle_t handle, uint8_t ep, uint8_t *buffer, uint32_t length);
/*!
 * @brief Primes the endpoint to receive packet from host.
 *
 * This function checks whether the endpoint is receiving packet, then it primes the endpoint
 * with the buffer address and the buffer length if the pipe is not busy. Otherwise, it ignores this transfer by
 * returning an error code.
 *
 * @param handle The class handle of the CDC ACM class.
 * @param ep The endpoint number of the transfer.
 * @param buffer The pointer to the buffer to be transferred.
 * @param length The length of the buffer to be transferred.
 * @return A USB error code or kStatus_USB_Success.
 * @retval kStatus_USB_Success Prime to receive packet successfully.
 * @retval kStatus_USB_Busy The endpoint is busy in transferring.
 * @retval kStatus_USB_InvalidHandle The CDC ACM device handle or the CDC ACM class handle is invalid.
 * @retval kStatus_USB_ControllerNotFound The controller interface is invalid.
 *
 * @note The function can only be called in the same context. 
 */
extern usb_status_t USB_DeviceCdcAcmRecv(class_handle_t handle, uint8_t ep, uint8_t *buffer, uint32_t length);

/*! @}*/

#if defined(__cplusplus)
}
#endif

/*! @}*/

#endif /* _USB_DEVICE_CDC_ACM_H_ */
