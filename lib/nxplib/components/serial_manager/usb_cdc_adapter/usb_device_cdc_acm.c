/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016, 2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#include <stdio.h>
#include <stdlib.h>

#include "usb_device_config.h"
#include "usb.h"
#include "usb_device.h"

#include "usb_device_class.h"

#if USB_DEVICE_CONFIG_CDC_ACM
#include "usb_device_cdc_acm.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define USB_CDC_ACM_ENTER_CRITICAL() \
    USB_OSA_SR_ALLOC();              \
    USB_OSA_ENTER_CRITICAL()

#define USB_CDC_ACM_EXIT_CRITICAL() USB_OSA_EXIT_CRITICAL()

/*******************************************************************************
 * Variables
 ******************************************************************************/
/* CDC ACM device instance */

USB_GLOBAL USB_RAM_ADDRESS_ALIGNMENT(USB_DATA_ALIGN_SIZE)
    usb_device_cdc_acm_struct_t g_cdcAcmHandle[USB_DEVICE_CONFIG_CDC_ACM];

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief Allocates the CDC ACM device handle.
 *
 * This function allocates the CDC ACM device handle.
 *
 * @param handle The class handle of the CDC ACM class.
 * @return A USB error code or kStatus_USB_Success.
 */
static usb_status_t USB_DeviceCdcAcmAllocateHandle(usb_device_cdc_acm_struct_t **handle)
{
    uint32_t count;
    for (count = 0; count < USB_DEVICE_CONFIG_CDC_ACM; count++)
    {
        if (NULL == g_cdcAcmHandle[count].handle)
        {
            *handle = &g_cdcAcmHandle[count];
            return kStatus_USB_Success;
        }
    }

    return kStatus_USB_Busy;
}

/*!
 * @brief Frees the CDC ACM device handle.
 *
 * This function frees the CDC ACM device handle.
 *
 * @param handle The class handle of the CDC ACM class.
 * @return A USB error code or kStatus_USB_Success.
 */
static usb_status_t USB_DeviceCdcAcmFreeHandle(usb_device_cdc_acm_struct_t *handle)
{
    handle->handle        = NULL;
    handle->configStruct  = NULL;
    handle->configuration = 0;
    handle->alternate     = 0;
    return kStatus_USB_Success;
}

/*!
 * @brief Responds to the interrupt in endpoint event.
 *
 * This function responds to the interrupt in endpoint event.
 *
 * @param handle The device handle of the CDC ACM device.
 * @param message The pointer to the message of the endpoint callback.
 * @param callbackParam The pointer to the parameter of the callback.
 * @return A USB error code or kStatus_USB_Success.
 */
usb_status_t USB_DeviceCdcAcmInterruptIn(usb_device_handle handle,
                                         usb_device_endpoint_callback_message_struct_t *message,
                                         void *callbackParam)
{
    usb_device_cdc_acm_struct_t *cdcAcmHandle;
    usb_status_t error = kStatus_USB_Error;
    cdcAcmHandle       = (usb_device_cdc_acm_struct_t *)callbackParam;
    if (!cdcAcmHandle)
    {
        return kStatus_USB_InvalidHandle;
    }

    cdcAcmHandle->interruptIn.isBusy = 0;

    if ((NULL != cdcAcmHandle->configStruct) && (cdcAcmHandle->configStruct->classCallback))
    {
        /*classCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap,
        it is from the second parameter of classInit */
        error = cdcAcmHandle->configStruct->classCallback((class_handle_t)cdcAcmHandle,
                                                          kUSB_DeviceCdcEventSerialStateNotif, message);
    }
    return error;
}

/*!
 * @brief Responds to the bulk in endpoint event.
 *
 * This function responds to the bulk in endpoint event.
 *
 * @param handle The device handle of the CDC ACM device.
 * @param message The pointer to the message of the endpoint callback.
 * @param callbackParam The pointer to the parameter of the callback.
 * @return A USB error code or kStatus_USB_Success.
 */
usb_status_t USB_DeviceCdcAcmBulkIn(usb_device_handle handle,
                                    usb_device_endpoint_callback_message_struct_t *message,
                                    void *callbackParam)
{
    usb_device_cdc_acm_struct_t *cdcAcmHandle;
    usb_status_t error = kStatus_USB_Error;
    cdcAcmHandle       = (usb_device_cdc_acm_struct_t *)callbackParam;

    if (!cdcAcmHandle)
    {
        return kStatus_USB_InvalidHandle;
    }

    cdcAcmHandle->bulkIn.isBusy = 0;

    if ((NULL != cdcAcmHandle->configStruct) && (cdcAcmHandle->configStruct->classCallback))
    {
        /*classCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap,
        it is from the second parameter of classInit */
        error = cdcAcmHandle->configStruct->classCallback((class_handle_t)cdcAcmHandle, kUSB_DeviceCdcEventSendResponse,
                                                          message);
    }
    return error;
}

/*!
 * @brief Responds to the bulk out endpoint event.
 *
 * This function responds to the bulk out endpoint event.
 *
 * @param handle The device handle of the CDC ACM device.
 * @param message The pointer to the message of the endpoint callback.
 * @param callbackParam The pointer to the parameter of the callback.
 * @return A USB error code or kStatus_USB_Success.
 */
usb_status_t USB_DeviceCdcAcmBulkOut(usb_device_handle handle,
                                     usb_device_endpoint_callback_message_struct_t *message,
                                     void *callbackParam)
{
    usb_device_cdc_acm_struct_t *cdcAcmHandle;
    usb_status_t error = kStatus_USB_Error;
    cdcAcmHandle       = (usb_device_cdc_acm_struct_t *)callbackParam;

    if (!cdcAcmHandle)
    {
        return kStatus_USB_InvalidHandle;
    }

    cdcAcmHandle->bulkOut.isBusy = 0;

    if ((NULL != cdcAcmHandle->configStruct) && (cdcAcmHandle->configStruct->classCallback))
    {
        /*classCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap,
        it is from the second parameter of classInit */
        error = cdcAcmHandle->configStruct->classCallback((class_handle_t)cdcAcmHandle, kUSB_DeviceCdcEventRecvResponse,
                                                          message);
    }
    return error;
}

/*!
 * @brief Initializes the endpoints in CDC ACM class.
 *
 * This function initializes the endpoints in CDC ACM class.
 *
 * @param cdcAcmHandle The class handle of the CDC ACM class.
 * @return A USB error code or kStatus_USB_Success.
 */
usb_status_t USB_DeviceCdcAcmEndpointsInit(usb_device_cdc_acm_struct_t *cdcAcmHandle)
{
    usb_device_interface_list_t *interfaceList;
    usb_device_interface_struct_t *interface = NULL;
    usb_status_t error                       = kStatus_USB_Error;

    if (!cdcAcmHandle)
    {
        return error;
    }

    /* return error when configuration is invalid (0 or more than the configuration number) */
    if ((cdcAcmHandle->configuration == 0U) ||
        (cdcAcmHandle->configuration > cdcAcmHandle->configStruct->classInfomation->configurations))
    {
        return error;
    }

    interfaceList = &cdcAcmHandle->configStruct->classInfomation->interfaceList[cdcAcmHandle->configuration - 1];

    for (uint32_t count = 0; count < interfaceList->count; count++)
    {
        if (USB_DEVICE_CONFIG_CDC_COMM_CLASS_CODE == interfaceList->interfaces[count].classCode)
        {
            for (uint32_t index = 0; index < interfaceList->interfaces[count].count; index++)
            {
                if (interfaceList->interfaces[count].interface[index].alternateSetting == cdcAcmHandle->alternate)
                {
                    interface = &interfaceList->interfaces[count].interface[index];
                    break;
                }
            }
            cdcAcmHandle->interfaceNumber = interfaceList->interfaces[count].interfaceNumber;
            break;
        }
    }
    if (!interface)
    {
        return error;
    }
    cdcAcmHandle->commInterfaceHandle = interface;
    for (uint32_t count = 0; count < interface->endpointList.count; count++)
    {
        usb_device_endpoint_init_struct_t epInitStruct;
        usb_device_endpoint_callback_struct_t epCallback;
        epInitStruct.zlt             = 0;
        epInitStruct.interval        = interface->endpointList.endpoint[count].interval;
        epInitStruct.endpointAddress = interface->endpointList.endpoint[count].endpointAddress;
        epInitStruct.maxPacketSize   = interface->endpointList.endpoint[count].maxPacketSize;
        epInitStruct.transferType    = interface->endpointList.endpoint[count].transferType;

        if ((USB_IN == ((epInitStruct.endpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) >>
                        USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT)) &&
            (USB_ENDPOINT_INTERRUPT == epInitStruct.transferType))
        {
            cdcAcmHandle->interruptIn.ep = (epInitStruct.endpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_NUMBER_MASK);
            cdcAcmHandle->interruptIn.isBusy         = 0;
            cdcAcmHandle->interruptIn.pipeDataBuffer = (uint8_t *)USB_UNINITIALIZED_VAL_32;
            cdcAcmHandle->interruptIn.pipeStall      = 0U;
            cdcAcmHandle->interruptIn.pipeDataLen    = 0U;
            epCallback.callbackFn                    = USB_DeviceCdcAcmInterruptIn;
        }

        epCallback.callbackParam = cdcAcmHandle;

        error = USB_DeviceInitEndpoint(cdcAcmHandle->handle, &epInitStruct, &epCallback);
    }

    for (uint32_t count = 0; count < interfaceList->count; count++)
    {
        if (USB_DEVICE_CONFIG_CDC_DATA_CLASS_CODE == interfaceList->interfaces[count].classCode)
        {
            for (uint32_t index = 0; index < interfaceList->interfaces[count].count; index++)
            {
                if (interfaceList->interfaces[count].interface[index].alternateSetting == cdcAcmHandle->alternate)
                {
                    interface = &interfaceList->interfaces[count].interface[index];
                    break;
                }
            }
            break;
        }
    }

    cdcAcmHandle->dataInterfaceHandle = interface;

    for (uint32_t count = 0; count < interface->endpointList.count; count++)
    {
        usb_device_endpoint_init_struct_t epInitStruct;
        usb_device_endpoint_callback_struct_t epCallback;
        epInitStruct.zlt             = 0;
        epInitStruct.interval        = interface->endpointList.endpoint[count].interval;
        epInitStruct.endpointAddress = interface->endpointList.endpoint[count].endpointAddress;
        epInitStruct.maxPacketSize   = interface->endpointList.endpoint[count].maxPacketSize;
        epInitStruct.transferType    = interface->endpointList.endpoint[count].transferType;

        if ((USB_IN == ((epInitStruct.endpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) >>
                        USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT)) &&
            (USB_ENDPOINT_BULK == epInitStruct.transferType))
        {
            cdcAcmHandle->bulkIn.ep     = (epInitStruct.endpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_NUMBER_MASK);
            cdcAcmHandle->bulkIn.isBusy = 0;
            cdcAcmHandle->bulkIn.pipeDataBuffer = (uint8_t *)USB_UNINITIALIZED_VAL_32;
            cdcAcmHandle->bulkIn.pipeStall      = 0U;
            cdcAcmHandle->bulkIn.pipeDataLen    = 0U;
            epCallback.callbackFn               = USB_DeviceCdcAcmBulkIn;
        }
        else if ((USB_OUT == ((epInitStruct.endpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) >>
                              USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT)) &&
                 (USB_ENDPOINT_BULK == epInitStruct.transferType))
        {
            cdcAcmHandle->bulkOut.ep     = (epInitStruct.endpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_NUMBER_MASK);
            cdcAcmHandle->bulkOut.isBusy = 0;
            cdcAcmHandle->bulkOut.pipeDataBuffer = (uint8_t *)USB_UNINITIALIZED_VAL_32;
            cdcAcmHandle->bulkOut.pipeStall      = 0U;
            cdcAcmHandle->bulkOut.pipeDataLen    = 0U;
            epCallback.callbackFn                = USB_DeviceCdcAcmBulkOut;
        }
        else
        {
        }
        epCallback.callbackParam = cdcAcmHandle;

        error = USB_DeviceInitEndpoint(cdcAcmHandle->handle, &epInitStruct, &epCallback);
    }
    return error;
}

/*!
 * @brief De-initializes the endpoints in CDC ACM class.
 *
 * This function de-initializes the endpoints in CDC ACM class.
 *
 * @param cdcAcmHandle The class handle of the CDC ACM class.
 * @return A USB error code or kStatus_USB_Success.
 */
usb_status_t USB_DeviceCdcAcmEndpointsDeinit(usb_device_cdc_acm_struct_t *cdcAcmHandle)
{
    usb_status_t error = kStatus_USB_Error;

    if ((!cdcAcmHandle->commInterfaceHandle) || (!cdcAcmHandle->dataInterfaceHandle))
    {
        return error;
    }
    for (uint32_t count = 0; count < cdcAcmHandle->commInterfaceHandle->endpointList.count; count++)
    {
        error = USB_DeviceDeinitEndpoint(
            cdcAcmHandle->handle, cdcAcmHandle->commInterfaceHandle->endpointList.endpoint[count].endpointAddress);
    }
    for (uint32_t count = 0; count < cdcAcmHandle->dataInterfaceHandle->endpointList.count; count++)
    {
        error = USB_DeviceDeinitEndpoint(
            cdcAcmHandle->handle, cdcAcmHandle->dataInterfaceHandle->endpointList.endpoint[count].endpointAddress);
    }
    cdcAcmHandle->commInterfaceHandle = NULL;
    cdcAcmHandle->dataInterfaceHandle = NULL;

    return error;
}

/*!
 * @brief Handles the CDC ACM class event.
 *
 * This function responses to various events including the common device events and the class specific events.
 * For class specific events, it calls the class callback defined in the application to deal with the class specific
 * event.
 *
 * @param handle The class handle of the CDC ACM class.
 * @param event The event type.
 * @param param The class handle of the CDC ACM class.
 * @return A USB error code or kStatus_USB_Success.
 */
usb_status_t USB_DeviceCdcAcmEvent(void *handle, uint32_t event, void *param)
{
    usb_device_cdc_acm_struct_t *cdcAcmHandle;
    usb_device_cdc_acm_request_param_struct_t reqParam;
    usb_status_t error = kStatus_USB_Error;
    uint16_t interfaceAlternate;
    uint8_t *temp8;
    uint8_t alternate;

    if ((!param) || (!handle))
    {
        return kStatus_USB_InvalidHandle;
    }

    cdcAcmHandle = (usb_device_cdc_acm_struct_t *)handle;

    switch (event)
    {
        case kUSB_DeviceClassEventDeviceReset:
            /* Bus reset, clear the configuration. */
            cdcAcmHandle->configuration = 0;
            break;
        case kUSB_DeviceClassEventSetConfiguration:
            temp8 = ((uint8_t *)param);
            if (!cdcAcmHandle->configStruct)
            {
                break;
            }
            if (*temp8 == cdcAcmHandle->configuration)
            {
                break;
            }

            error                       = USB_DeviceCdcAcmEndpointsDeinit(cdcAcmHandle);
            cdcAcmHandle->configuration = *temp8;
            cdcAcmHandle->alternate     = 0;
            error                       = USB_DeviceCdcAcmEndpointsInit(cdcAcmHandle);
            if (kStatus_USB_Success != error)
            {
#ifdef DEBUG
                usb_echo("kUSB_DeviceClassEventSetConfiguration, USB_DeviceInitEndpoint fail\r\n");
#endif
            }
            break;
        case kUSB_DeviceClassEventSetInterface:
            if (!cdcAcmHandle->configStruct)
            {
                break;
            }

            interfaceAlternate = *((uint16_t *)param);
            alternate          = (uint8_t)(interfaceAlternate & 0xFFU);

            if (cdcAcmHandle->interfaceNumber != ((uint8_t)(interfaceAlternate >> 8U)))
            {
                break;
            }
            if (alternate == cdcAcmHandle->alternate)
            {
                break;
            }
            error                   = USB_DeviceCdcAcmEndpointsDeinit(cdcAcmHandle);
            cdcAcmHandle->alternate = alternate;
            error                   = USB_DeviceCdcAcmEndpointsInit(cdcAcmHandle);
            if (kStatus_USB_Success != error)
            {
#ifdef DEBUG
                usb_echo("kUSB_DeviceClassEventSetInterface, USB_DeviceInitEndpoint fail\r\n");
#endif
            }
            break;
        case kUSB_DeviceClassEventSetEndpointHalt:
            if ((!cdcAcmHandle->configStruct) || (!cdcAcmHandle->commInterfaceHandle) ||
                (!cdcAcmHandle->dataInterfaceHandle))
            {
                break;
            }
            temp8 = ((uint8_t *)param);
            for (uint32_t count = 0; count < cdcAcmHandle->commInterfaceHandle->endpointList.count; count++)
            {
                if (*temp8 == cdcAcmHandle->commInterfaceHandle->endpointList.endpoint[count].endpointAddress)
                {
                    cdcAcmHandle->interruptIn.pipeStall = 1U;
                    error                               = USB_DeviceStallEndpoint(cdcAcmHandle->handle, *temp8);
                }
            }
            for (uint32_t count = 0; count < cdcAcmHandle->dataInterfaceHandle->endpointList.count; count++)
            {
                if (*temp8 == cdcAcmHandle->dataInterfaceHandle->endpointList.endpoint[count].endpointAddress)
                {
                    if (USB_IN == (((*temp8) & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) >>
                                   USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT))
                    {
                        cdcAcmHandle->bulkIn.pipeStall = 1U;
                    }
                    else
                    {
                        cdcAcmHandle->bulkOut.pipeStall = 1U;
                    }
                    error = USB_DeviceStallEndpoint(cdcAcmHandle->handle, *temp8);
                }
            }
            break;
        case kUSB_DeviceClassEventClearEndpointHalt:
            if ((!cdcAcmHandle->configStruct) || (!cdcAcmHandle->commInterfaceHandle) ||
                (!cdcAcmHandle->dataInterfaceHandle))
            {
                break;
            }
            temp8 = ((uint8_t *)param);
            for (uint32_t count = 0; count < cdcAcmHandle->commInterfaceHandle->endpointList.count; count++)
            {
                if (*temp8 == cdcAcmHandle->commInterfaceHandle->endpointList.endpoint[count].endpointAddress)
                {
                    error = USB_DeviceUnstallEndpoint(cdcAcmHandle->handle, *temp8);
                    if (USB_IN == (((*temp8) & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) >>
                                   USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT))
                    {
                        if (cdcAcmHandle->interruptIn.pipeStall)
                        {
                            cdcAcmHandle->interruptIn.pipeStall = 0U;
                            if ((uint8_t *)USB_UNINITIALIZED_VAL_32 != cdcAcmHandle->interruptIn.pipeDataBuffer)
                            {
                                error = USB_DeviceSendRequest(cdcAcmHandle->handle, (cdcAcmHandle->interruptIn.ep),
                                                              cdcAcmHandle->interruptIn.pipeDataBuffer,
                                                              cdcAcmHandle->interruptIn.pipeDataLen);
                                if (kStatus_USB_Success != error)
                                {
                                    usb_device_endpoint_callback_message_struct_t endpointCallbackMessage;
                                    endpointCallbackMessage.buffer  = cdcAcmHandle->interruptIn.pipeDataBuffer;
                                    endpointCallbackMessage.length  = cdcAcmHandle->interruptIn.pipeDataLen;
                                    endpointCallbackMessage.isSetup = 0U;
                                    USB_DeviceCdcAcmBulkIn(cdcAcmHandle->handle, (void *)&endpointCallbackMessage,
                                                           handle);
                                }
                                cdcAcmHandle->interruptIn.pipeDataBuffer = (uint8_t *)USB_UNINITIALIZED_VAL_32;
                                cdcAcmHandle->interruptIn.pipeDataLen    = 0U;
                            }
                        }
                    }
                }
            }
            for (uint32_t count = 0; count < cdcAcmHandle->dataInterfaceHandle->endpointList.count; count++)
            {
                if (*temp8 == cdcAcmHandle->dataInterfaceHandle->endpointList.endpoint[count].endpointAddress)
                {
                    error = USB_DeviceUnstallEndpoint(cdcAcmHandle->handle, *temp8);
                    if (USB_IN == (((*temp8) & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) >>
                                   USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT))
                    {
                        if (cdcAcmHandle->bulkIn.pipeStall)
                        {
                            cdcAcmHandle->bulkIn.pipeStall = 0U;
                            if ((uint8_t *)USB_UNINITIALIZED_VAL_32 != cdcAcmHandle->bulkIn.pipeDataBuffer)
                            {
                                error = USB_DeviceSendRequest(cdcAcmHandle->handle, (cdcAcmHandle->bulkIn.ep),
                                                              cdcAcmHandle->bulkIn.pipeDataBuffer,
                                                              cdcAcmHandle->bulkIn.pipeDataLen);
                                if (kStatus_USB_Success != error)
                                {
                                    usb_device_endpoint_callback_message_struct_t endpointCallbackMessage;
                                    endpointCallbackMessage.buffer  = cdcAcmHandle->bulkIn.pipeDataBuffer;
                                    endpointCallbackMessage.length  = cdcAcmHandle->bulkIn.pipeDataLen;
                                    endpointCallbackMessage.isSetup = 0U;
                                    USB_DeviceCdcAcmBulkIn(cdcAcmHandle->handle, (void *)&endpointCallbackMessage,
                                                           handle);
                                }
                                cdcAcmHandle->bulkIn.pipeDataBuffer = (uint8_t *)USB_UNINITIALIZED_VAL_32;
                                cdcAcmHandle->bulkIn.pipeDataLen    = 0U;
                            }
                        }
                    }
                    else
                    {
                        if (cdcAcmHandle->bulkOut.pipeStall)
                        {
                            cdcAcmHandle->bulkOut.pipeStall = 0U;
                            if ((uint8_t *)USB_UNINITIALIZED_VAL_32 != cdcAcmHandle->bulkOut.pipeDataBuffer)
                            {
                                error = USB_DeviceRecvRequest(cdcAcmHandle->handle, (cdcAcmHandle->bulkOut.ep),
                                                              cdcAcmHandle->bulkOut.pipeDataBuffer,
                                                              cdcAcmHandle->bulkOut.pipeDataLen);
                                if (kStatus_USB_Success != error)
                                {
                                    usb_device_endpoint_callback_message_struct_t endpointCallbackMessage;
                                    endpointCallbackMessage.buffer  = cdcAcmHandle->bulkOut.pipeDataBuffer;
                                    endpointCallbackMessage.length  = cdcAcmHandle->bulkOut.pipeDataLen;
                                    endpointCallbackMessage.isSetup = 0U;
                                    USB_DeviceCdcAcmBulkOut(cdcAcmHandle->handle, (void *)&endpointCallbackMessage,
                                                            handle);
                                }
                                cdcAcmHandle->bulkOut.pipeDataBuffer = (uint8_t *)USB_UNINITIALIZED_VAL_32;
                                cdcAcmHandle->bulkOut.pipeDataLen    = 0U;
                            }
                        }
                    }
                }
            }
            break;
        case kUSB_DeviceClassEventClassRequest:
            if (param)
            {
                usb_device_control_request_struct_t *controlRequest = (usb_device_control_request_struct_t *)param;

                if ((controlRequest->setup->wIndex & 0xFFU) != cdcAcmHandle->interfaceNumber)
                {
                    break;
                }
                /* Standard CDC request */
                if (USB_REQUEST_TYPE_TYPE_CLASS == (controlRequest->setup->bmRequestType & USB_REQUEST_TYPE_TYPE_MASK))
                {
                    reqParam.buffer         = &(controlRequest->buffer);
                    reqParam.length         = &(controlRequest->length);
                    reqParam.interfaceIndex = controlRequest->setup->wIndex;
                    reqParam.setupValue     = controlRequest->setup->wValue;
                    reqParam.isSetup        = controlRequest->isSetup;
                    switch (controlRequest->setup->bRequest)
                    {
                        case USB_DEVICE_CDC_REQUEST_SEND_ENCAPSULATED_COMMAND:
                            /* classCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap,
                            it is from the second parameter of classInit */
                            error = cdcAcmHandle->configStruct->classCallback(
                                (class_handle_t)cdcAcmHandle, kUSB_DeviceCdcEventSendEncapsulatedCommand, &reqParam);
                            break;
                        case USB_DEVICE_CDC_REQUEST_GET_ENCAPSULATED_RESPONSE:
                            /* classCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap,
                            it is from the second parameter of classInit */
                            error = cdcAcmHandle->configStruct->classCallback(
                                (class_handle_t)cdcAcmHandle, kUSB_DeviceCdcEventGetEncapsulatedResponse, &reqParam);
                            break;
                        case USB_DEVICE_CDC_REQUEST_SET_COMM_FEATURE:
                            /* classCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap,
                            it is from the second parameter of classInit */
                            error = cdcAcmHandle->configStruct->classCallback(
                                (class_handle_t)cdcAcmHandle, kUSB_DeviceCdcEventSetCommFeature, &reqParam);
                            break;
                        case USB_DEVICE_CDC_REQUEST_GET_COMM_FEATURE:
                            /* classCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap,
                            it is from the second parameter of classInit */
                            error = cdcAcmHandle->configStruct->classCallback(
                                (class_handle_t)cdcAcmHandle, kUSB_DeviceCdcEventGetCommFeature, &reqParam);
                            break;
                        case USB_DEVICE_CDC_REQUEST_CLEAR_COMM_FEATURE:
                            /* classCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap,
                            it is from the second parameter of classInit */
                            error = cdcAcmHandle->configStruct->classCallback(
                                (class_handle_t)cdcAcmHandle, kUSB_DeviceCdcEventClearCommFeature, &reqParam);
                            break;
                        case USB_DEVICE_CDC_REQUEST_GET_LINE_CODING:
                            /* classCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap,
                            it is from the second parameter of classInit */
                            error = cdcAcmHandle->configStruct->classCallback(
                                (class_handle_t)cdcAcmHandle, kUSB_DeviceCdcEventGetLineCoding, &reqParam);
                            break;
                        case USB_DEVICE_CDC_REQUEST_SET_LINE_CODING:
                            /* classCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap,
                            it is from the second parameter of classInit */
                            error = cdcAcmHandle->configStruct->classCallback(
                                (class_handle_t)cdcAcmHandle, kUSB_DeviceCdcEventSetLineCoding, &reqParam);
                            break;
                        case USB_DEVICE_CDC_REQUEST_SET_CONTROL_LINE_STATE:
                            /* classCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap,
                            it is from the second parameter of classInit */
                            error = cdcAcmHandle->configStruct->classCallback(
                                (class_handle_t)cdcAcmHandle, kUSB_DeviceCdcEventSetControlLineState, &reqParam);
                            break;
                        case USB_DEVICE_CDC_REQUEST_SEND_BREAK:
                            /* classCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap,
                            it is from the second parameter of classInit */
                            error = cdcAcmHandle->configStruct->classCallback((class_handle_t)cdcAcmHandle,
                                                                              kUSB_DeviceCdcEventSendBreak, &reqParam);
                            break;
                        default:
                            error = kStatus_USB_InvalidRequest;
                            break;
                    }
                }
            }
            break;
        default:
            break;
    }
    return error;
}

/*!
 * @brief Initializes the USB CDC ACM class.
 *
 * This function obtains a usb device handle according to the controller id, initializes the CDC ACM class
 * with the class configure parameters and creates the mutex for each pipe.
 *
 * @param controllerId The id of the controller. The value can be choosen from kUSB_ControllerKhci0,
 *  kUSB_ControllerKhci1, kUSB_ControllerEhci0 or kUSB_ControllerEhci1.
 * @param config The user configuration structure of type usb_device_class_config_struct_t. The user
 *  populates the members of this structure and passes the pointer of this structure
 *  into this function.
 * @param handle  It is out parameter. The class handle of the CDC ACM class.
 * @return A USB error code or kStatus_USB_Success.
 */
usb_status_t USB_DeviceCdcAcmInit(uint8_t controllerId,
                                  usb_device_class_config_struct_t *config,
                                  class_handle_t *handle)
{
    usb_device_cdc_acm_struct_t *cdcAcmHandle;
    usb_status_t error = kStatus_USB_Error;

    error = USB_DeviceCdcAcmAllocateHandle(&cdcAcmHandle);

    if (kStatus_USB_Success != error)
    {
        return error;
    }

    error = USB_DeviceClassGetDeviceHandle(controllerId, &cdcAcmHandle->handle);

    if (kStatus_USB_Success != error)
    {
        return error;
    }

    if (!cdcAcmHandle->handle)
    {
        return kStatus_USB_InvalidHandle;
    }
    cdcAcmHandle->configStruct  = config;
    cdcAcmHandle->configuration = 0;
    cdcAcmHandle->alternate     = 0xFF;

    if (kStatus_USB_OSA_Success != USB_OsaMutexCreate(&(cdcAcmHandle->bulkIn.mutex)))
    {
#ifdef DEBUG
        usb_echo("mutex create error!");
#endif
    }
    if (kStatus_USB_OSA_Success != USB_OsaMutexCreate(&(cdcAcmHandle->bulkOut.mutex)))
    {
#ifdef DEBUG
        usb_echo("mutex create error!");
#endif
    }
    if (kStatus_USB_OSA_Success != USB_OsaMutexCreate(&(cdcAcmHandle->interruptIn.mutex)))
    {
#ifdef DEBUG
        usb_echo("mutex create error!");
#endif
    }
    *handle = (class_handle_t)cdcAcmHandle;
    return error;
}

/*!
 * @brief De-Initializes the USB CDC ACM class.
 *
 * This function destroys the mutex for each pipe, deinit each endpoint of the CDC ACM class and free
 * the CDC ACM class handle.
 *
 * @param handle The class handle of the CDC ACM class.
 * @return A USB error code or kStatus_USB_Success.
 */
usb_status_t USB_DeviceCdcAcmDeinit(class_handle_t handle)
{
    usb_device_cdc_acm_struct_t *cdcAcmHandle;
    usb_status_t error = kStatus_USB_Error;

    cdcAcmHandle = (usb_device_cdc_acm_struct_t *)handle;

    if (!cdcAcmHandle)
    {
        return kStatus_USB_InvalidHandle;
    }
    if (kStatus_USB_OSA_Success != USB_OsaMutexDestroy((cdcAcmHandle->bulkIn.mutex)))
    {
#ifdef DEBUG
        usb_echo("mutex destroy error!");
#endif
    }
    if (kStatus_USB_OSA_Success != USB_OsaMutexDestroy((cdcAcmHandle->bulkOut.mutex)))
    {
#ifdef DEBUG
        usb_echo("mutex destroy error!");
#endif
    }
    if (kStatus_USB_OSA_Success != USB_OsaMutexDestroy((cdcAcmHandle->interruptIn.mutex)))
    {
#ifdef DEBUG
        usb_echo("mutex destroy error!");
#endif
    }
    error = USB_DeviceCdcAcmEndpointsDeinit(cdcAcmHandle);
    USB_DeviceCdcAcmFreeHandle(cdcAcmHandle);
    return error;
}

/*!
 * @brief Prime the endpoint to send packet to host.
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
 */
usb_status_t USB_DeviceCdcAcmSend(class_handle_t handle, uint8_t ep, uint8_t *buffer, uint32_t length)
{
    usb_device_cdc_acm_struct_t *cdcAcmHandle;
    usb_status_t error                    = kStatus_USB_Error;
    usb_device_cdc_acm_pipe_t *cdcAcmPipe = NULL;

    if (!handle)
    {
        return kStatus_USB_InvalidHandle;
    }
    cdcAcmHandle = (usb_device_cdc_acm_struct_t *)handle;

    if (cdcAcmHandle->bulkIn.ep == ep)
    {
        cdcAcmPipe = &(cdcAcmHandle->bulkIn);
    }
    else if (cdcAcmHandle->interruptIn.ep == ep)
    {
        cdcAcmPipe = &(cdcAcmHandle->interruptIn);
    }
    else
    {
    }

    if (NULL != cdcAcmPipe)
    {
        if (1U == cdcAcmPipe->isBusy)
        {
            return kStatus_USB_Busy;
        }
        cdcAcmPipe->isBusy = 1U;

        if (cdcAcmPipe->pipeStall)
        {
            cdcAcmPipe->pipeDataBuffer = buffer;
            cdcAcmPipe->pipeDataLen    = length;
            return kStatus_USB_Success;
        }

        error = USB_DeviceSendRequest(cdcAcmHandle->handle, ep, buffer, length);
        if (kStatus_USB_Success != error)
        {
            cdcAcmPipe->isBusy = 0U;
        }
    }
    return error;
}

/*!
 * @brief Prime the endpoint to receive packet from host.
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
 */
usb_status_t USB_DeviceCdcAcmRecv(class_handle_t handle, uint8_t ep, uint8_t *buffer, uint32_t length)
{
    usb_device_cdc_acm_struct_t *cdcAcmHandle;
    usb_status_t error = kStatus_USB_Error;
    if (!handle)
    {
        return kStatus_USB_InvalidHandle;
    }
    cdcAcmHandle = (usb_device_cdc_acm_struct_t *)handle;

    if (1U == cdcAcmHandle->bulkOut.isBusy)
    {
        return kStatus_USB_Busy;
    }
    cdcAcmHandle->bulkOut.isBusy = 1U;

    if (cdcAcmHandle->bulkOut.pipeStall)
    {
        cdcAcmHandle->bulkOut.pipeDataBuffer = buffer;
        cdcAcmHandle->bulkOut.pipeDataLen    = length;
        return kStatus_USB_Success;
    }

    error = USB_DeviceRecvRequest(cdcAcmHandle->handle, ep, buffer, length);
    if (kStatus_USB_Success != error)
    {
        cdcAcmHandle->bulkOut.isBusy = 0U;
    }
    return error;
}

#endif /* USB_DEVICE_CONFIG_CDC_ACM */
