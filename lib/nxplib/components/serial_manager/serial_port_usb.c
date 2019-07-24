/*
 * Copyright (c) 2015 - 2016, Freescale Semiconductor, Inc.
 * Copyright 2016 - 2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_common.h"
#include "serial_manager.h"

#if (defined(SERIAL_PORT_TYPE_USBCDC) && (SERIAL_PORT_TYPE_USBCDC > 0U))
#include "usb_device_config.h"
#include "usb.h"
#include "usb_device.h"

#include "usb_device_class.h"
#include "usb_device_cdc_acm.h"
#include "usb_device_ch9.h"

#include "usb_device_descriptor.h"
#include "serial_port_usb.h"

#if defined(USB_DEVICE_CONFIG_EHCI) && (USB_DEVICE_CONFIG_EHCI > 0)
#include "usb_phy.h"
#endif
#if (defined(FSL_FEATURE_SOC_SYSMPU_COUNT) && (FSL_FEATURE_SOC_SYSMPU_COUNT > 0U))
#include "fsl_sysmpu.h"
#endif /* FSL_FEATURE_SOC_SYSMPU_COUNT */

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#ifndef NDEBUG
#if (defined(DEBUG_CONSOLE_ASSERT_DISABLE) && (DEBUG_CONSOLE_ASSERT_DISABLE > 0U))
#undef assert
#define assert(n)
#endif
#endif

#if defined(USB_DEVICE_CONFIG_EHCI) && (USB_DEVICE_CONFIG_EHCI > 0)
#define DATA_BUFF_SIZE HS_CDC_VCOM_BULK_OUT_PACKET_SIZE

#endif
#if defined(USB_DEVICE_CONFIG_KHCI) && (USB_DEVICE_CONFIG_KHCI > 0)
#define DATA_BUFF_SIZE FS_CDC_VCOM_BULK_OUT_PACKET_SIZE

#endif
#if defined(USB_DEVICE_CONFIG_LPCIP3511FS) && (USB_DEVICE_CONFIG_LPCIP3511FS > 0U)
#define DATA_BUFF_SIZE FS_CDC_VCOM_BULK_OUT_PACKET_SIZE

#endif

#if defined(USB_DEVICE_CONFIG_LPCIP3511HS) && (USB_DEVICE_CONFIG_LPCIP3511HS > 0U)
#define DATA_BUFF_SIZE HS_CDC_VCOM_BULK_OUT_PACKET_SIZE
#endif

/* Currently configured line coding */
#define LINE_CODING_SIZE (0x07)
#define LINE_CODING_DTERATE (115200)
#define LINE_CODING_CHARFORMAT (0x00)
#define LINE_CODING_PARITYTYPE (0x00)
#define LINE_CODING_DATABITS (0x08)

/* Communications feature */
#define COMM_FEATURE_DATA_SIZE (0x02)
#define STATUS_ABSTRACT_STATE (0x0000)
#define COUNTRY_SETTING (0x0000)

/* Notification of serial state */
#define NOTIF_PACKET_SIZE (0x08)
#define UART_BITMAP_SIZE (0x02)
#define NOTIF_REQUEST_TYPE (0xA1)

typedef struct _serial_usb_send_state
{
    serial_manager_callback_t callback;
    void *callbackParam;
    uint8_t *buffer;
    uint32_t length;
    volatile uint8_t busy;
    volatile uint8_t waiting4Prime;
} serial_usb_send_state_t;

typedef struct _serial_usb_recv_state
{
    serial_manager_callback_t callback;
    void *callbackParam;
    volatile uint8_t busy;
} serial_usb_recv_state_t;

/* Define the information relates to abstract control model */
typedef struct _usb_cdc_acm_info
{
    uint8_t serialStateBuf[NOTIF_PACKET_SIZE + UART_BITMAP_SIZE]; /* Serial state buffer of the CDC device to notify the
                                                                     serial state to host. */
    bool dtePresent;          /* A flag to indicate whether DTE is present.         */
    uint16_t breakDuration;   /* Length of time in milliseconds of the break signal */
    uint8_t dteStatus;        /* Status of data terminal equipment                  */
    uint8_t currentInterface; /* Current interface index.                           */
    uint16_t uartState;       /* UART state of the CDC device.                      */
} usb_cdc_acm_info_t;

/* Define the types for application */
typedef struct _serial_usb_cdc_state
{
    struct _serial_usb_cdc_state *next; /* Next pointer of the interface */
    usb_device_handle deviceHandle;     /* USB device handle. */
    class_handle_t cdcAcmHandle;        /* USB CDC ACM class handle. */
    serial_usb_send_state_t tx;
    serial_usb_recv_state_t rx;
    volatile uint8_t attach; /* A flag to indicate whether a usb device is attached. 1: attached, 0: not attached */
    uint8_t speed;           /* Speed of USB device. USB_SPEED_FULL/USB_SPEED_LOW/USB_SPEED_HIGH.                 */
    volatile uint8_t
        startTransactions; /* A flag to indicate whether a CDC device is ready to transmit and receive data.    */
    uint8_t currentConfiguration;                                           /* Current configuration value. */
    uint8_t currentInterfaceAlternateSetting[USB_CDC_VCOM_INTERFACE_COUNT]; /* Current alternate setting value for each
                                                                               interface. */
    uint8_t instance;                                                       /* The instance of the interface */
    uint8_t irqNumber;                                                      /* The IRQ number of the interface */
} serial_usb_cdc_state_t;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

static void USB_DeviceIsrEnable(serial_usb_cdc_state_t *serialUsbCdc);
#if USB_DEVICE_CONFIG_USE_TASK
void USB_DeviceTaskFn(void *deviceHandle);
#endif

static usb_status_t USB_DeviceCdcVcomCallback(class_handle_t handle, uint32_t event, void *param);
static usb_status_t USB_DeviceCallback(usb_device_handle handle, uint32_t event, void *param);
serial_manager_status_t Serial_UsbCdcRead(serial_handle_t serialHandle, uint8_t *buffer, uint32_t length);

/*******************************************************************************
 * Variables
 ******************************************************************************/

static serial_usb_cdc_state_t *s_UsbCdcHead;
extern usb_device_endpoint_struct_t g_UsbDeviceCdcVcomDicEndpoints[];
extern usb_device_class_struct_t g_UsbDeviceCdcVcomConfig;

/* Line codinig of cdc device */
USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
static uint8_t s_lineCoding[LINE_CODING_SIZE] = {
    /* E.g. 0x00,0xC2,0x01,0x00 : 0x0001C200 is 115200 bits per second */
    (LINE_CODING_DTERATE >> 0U) & 0x000000FFU,
    (LINE_CODING_DTERATE >> 8U) & 0x000000FFU,
    (LINE_CODING_DTERATE >> 16U) & 0x000000FFU,
    (LINE_CODING_DTERATE >> 24U) & 0x000000FFU,
    LINE_CODING_CHARFORMAT,
    LINE_CODING_PARITYTYPE,
    LINE_CODING_DATABITS};

/* Abstract state of cdc device */
USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
static uint8_t s_abstractState[COMM_FEATURE_DATA_SIZE] = {(STATUS_ABSTRACT_STATE >> 0U) & 0x00FFU,
                                                          (STATUS_ABSTRACT_STATE >> 8U) & 0x00FFU};

/* Country code of cdc device */
USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
static uint8_t s_countryCode[COMM_FEATURE_DATA_SIZE] = {(COUNTRY_SETTING >> 0U) & 0x00FFU,
                                                        (COUNTRY_SETTING >> 8U) & 0x00FFU};

/* CDC ACM information */
USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) static usb_cdc_acm_info_t s_usbCdcAcmInfo;
/* Data buffer for receiving and sending*/
USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) static uint8_t s_currRecvBuf[DATA_BUFF_SIZE];

/* USB device class information */
static usb_device_class_config_struct_t s_cdcAcmConfig[1] = {{
    USB_DeviceCdcVcomCallback,
    0,
    &g_UsbDeviceCdcVcomConfig,
}};

/* USB device class configuration information */
static usb_device_class_config_list_struct_t s_cdcAcmConfigList = {
    s_cdcAcmConfig,
    USB_DeviceCallback,
    1,
};

/*******************************************************************************
 * Code
 ******************************************************************************/

static serial_usb_cdc_state_t *USB_DeviceGetInstanceFromDeviceHandle(serial_usb_cdc_state_t *head,
                                                                     usb_device_handle deviceHandle)
{
    while (head)
    {
        if (head->deviceHandle == deviceHandle)
        {
            return head;
        }
        head = head->next;
    }
    return NULL;
}

static serial_usb_cdc_state_t *USB_DeviceGetInstanceFromClassHandle(serial_usb_cdc_state_t *head,
                                                                    class_handle_t ClassHandle)
{
    while (head)
    {
        if (head->cdcAcmHandle == ClassHandle)
        {
            return head;
        }
        head = head->next;
    }
    return NULL;
}

static usb_status_t USB_DeviceAddItem(serial_usb_cdc_state_t **head, serial_usb_cdc_state_t *node)
{
    serial_usb_cdc_state_t *p = *head;
    uint32_t regPrimask;

    regPrimask = DisableGlobalIRQ();

    if (NULL == p)
    {
        *head = node;
    }
    else
    {
        while (p->next)
        {
            if (p == node)
            {
                EnableGlobalIRQ(regPrimask);
                return kStatus_USB_Error;
            }
            p = p->next;
        }

        p->next = node;
    }
    node->next = NULL;
    EnableGlobalIRQ(regPrimask);
    return kStatus_USB_Success;
}

static usb_status_t USB_DeviceRemoveItem(serial_usb_cdc_state_t **head, serial_usb_cdc_state_t *node)
{
    serial_usb_cdc_state_t *p = *head;
    serial_usb_cdc_state_t *q = NULL;
    uint32_t regPrimask;

    regPrimask = DisableGlobalIRQ();
    while (p)
    {
        if (p == node)
        {
            if (NULL == q)
            {
                *head = p->next;
            }
            else
            {
                q->next = p->next;
            }
            break;
        }
        else
        {
            q = p;
            p = p->next;
        }
    }
    EnableGlobalIRQ(regPrimask);
    return kStatus_USB_Success;
}

/*!
 * @brief CDC class specific callback function.
 *
 * This function handles the CDC class specific requests.
 *
 * @param handle          The CDC ACM class handle.
 * @param event           The CDC ACM class event type.
 * @param param           The parameter of the class specific request.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
static usb_status_t USB_DeviceCdcVcomCallback(class_handle_t handle, uint32_t event, void *param)
{
    serial_usb_cdc_state_t *serialUsbCdc;
    uint32_t len;
    uint8_t *uartBitmap;
    usb_device_cdc_acm_request_param_struct_t *acmReqParam;
    usb_device_endpoint_callback_message_struct_t *epCbParam;
    usb_cdc_acm_info_t *acmInfo = &s_usbCdcAcmInfo;
    serial_manager_callback_message_t msg;
    usb_status_t error = kStatus_USB_Error;

    serialUsbCdc = USB_DeviceGetInstanceFromClassHandle(s_UsbCdcHead, handle);
    if (NULL == serialUsbCdc)
    {
        return kStatus_USB_Error;
    }
    acmReqParam = (usb_device_cdc_acm_request_param_struct_t *)param;
    epCbParam   = (usb_device_endpoint_callback_message_struct_t *)param;
    switch (event)
    {
        case kUSB_DeviceCdcEventSendResponse:
        {
            if ((1 == serialUsbCdc->attach))
            {
                if ((epCbParam->length != 0) &&
                    (!(epCbParam->length % g_UsbDeviceCdcVcomDicEndpoints[0].maxPacketSize)))
                {
                    /* If the last packet is the size of endpoint, then send also zero-ended packet,
                     ** meaning that we want to inform the host that we do not have any additional
                     ** data, so it can flush the output.
                     */
                    USB_DeviceCdcAcmSend(handle, USB_CDC_VCOM_BULK_IN_ENDPOINT, NULL, 0);
                }
                else
                {
                    serialUsbCdc->tx.busy = 0;

                    if ((NULL != serialUsbCdc->tx.callback))
                    {
                        serial_manager_status_t serialManagerStatus = kStatus_SerialManager_Success;
                        msg.buffer                                  = serialUsbCdc->tx.buffer;
                        msg.length                                  = serialUsbCdc->tx.length;
                        if (USB_UNINITIALIZED_VAL_32 == epCbParam->length)
                        {
                            serialManagerStatus = kStatus_SerialManager_Canceled;
                        }
                        serialUsbCdc->tx.callback(serialUsbCdc->tx.callbackParam, &msg, serialManagerStatus);
                    }
                }
            }
        }
        break;
        case kUSB_DeviceCdcEventRecvResponse:
        {
            serialUsbCdc->rx.busy = 0U;
            if ((1 == serialUsbCdc->startTransactions))
            {
                serial_manager_status_t callbackStatus = kStatus_SerialManager_Success;

                if ((NULL != serialUsbCdc->rx.callback))
                {
                    msg.buffer = epCbParam->buffer;
                    msg.length = epCbParam->length;
                    if (USB_UNINITIALIZED_VAL_32 == msg.length)
                    {
                        msg.length     = 0U;
                        callbackStatus = kStatus_SerialManager_Canceled;
                    }
                    serialUsbCdc->rx.callback(serialUsbCdc->rx.callbackParam, &msg, callbackStatus);
                }
            }
        }
        break;
        case kUSB_DeviceCdcEventSerialStateNotif:
            ((usb_device_cdc_acm_struct_t *)handle)->hasSentState = 0;
            if ((serialUsbCdc->startTransactions) && (serialUsbCdc->tx.waiting4Prime))
            {
                serialUsbCdc->tx.waiting4Prime = 0;
                serialUsbCdc->tx.busy          = 1;
                if (kStatus_USB_Success != USB_DeviceCdcAcmSend(serialUsbCdc->cdcAcmHandle,
                                                                USB_CDC_VCOM_BULK_IN_ENDPOINT, serialUsbCdc->tx.buffer,
                                                                serialUsbCdc->tx.length))
                {
                    serialUsbCdc->tx.busy = 0;
                }
            }
            error = kStatus_USB_Success;
            break;
        case kUSB_DeviceCdcEventSendEncapsulatedCommand:
            break;
        case kUSB_DeviceCdcEventGetEncapsulatedResponse:
            break;
        case kUSB_DeviceCdcEventSetCommFeature:
            if (USB_DEVICE_CDC_FEATURE_ABSTRACT_STATE == acmReqParam->setupValue)
            {
                if (1 == acmReqParam->isSetup)
                {
                    *(acmReqParam->buffer) = s_abstractState;
                }
                else
                {
                    *(acmReqParam->length) = 0;
                }
            }
            else if (USB_DEVICE_CDC_FEATURE_COUNTRY_SETTING == acmReqParam->setupValue)
            {
                if (1 == acmReqParam->isSetup)
                {
                    *(acmReqParam->buffer) = s_countryCode;
                }
                else
                {
                    *(acmReqParam->length) = 0;
                }
            }
            else
            {
            }
            error = kStatus_USB_Success;
            break;
        case kUSB_DeviceCdcEventGetCommFeature:
            if (USB_DEVICE_CDC_FEATURE_ABSTRACT_STATE == acmReqParam->setupValue)
            {
                *(acmReqParam->buffer) = s_abstractState;
                *(acmReqParam->length) = COMM_FEATURE_DATA_SIZE;
            }
            else if (USB_DEVICE_CDC_FEATURE_COUNTRY_SETTING == acmReqParam->setupValue)
            {
                *(acmReqParam->buffer) = s_countryCode;
                *(acmReqParam->length) = COMM_FEATURE_DATA_SIZE;
            }
            else
            {
            }
            error = kStatus_USB_Success;
            break;
        case kUSB_DeviceCdcEventClearCommFeature:
            break;
        case kUSB_DeviceCdcEventGetLineCoding:
            *(acmReqParam->buffer) = s_lineCoding;
            *(acmReqParam->length) = LINE_CODING_SIZE;
            error                  = kStatus_USB_Success;
            break;
        case kUSB_DeviceCdcEventSetLineCoding:
        {
            if (1 == acmReqParam->isSetup)
            {
                *(acmReqParam->buffer) = s_lineCoding;
            }
            else
            {
                *(acmReqParam->length) = 0;
            }
        }
            error = kStatus_USB_Success;
            break;
        case kUSB_DeviceCdcEventSetControlLineState:
        {
            s_usbCdcAcmInfo.dteStatus = acmReqParam->setupValue;
            /* activate/deactivate Tx carrier */
            if (acmInfo->dteStatus & USB_DEVICE_CDC_CONTROL_SIG_BITMAP_CARRIER_ACTIVATION)
            {
                acmInfo->uartState |= USB_DEVICE_CDC_UART_STATE_TX_CARRIER;
            }
            else
            {
                acmInfo->uartState &= (uint16_t)~USB_DEVICE_CDC_UART_STATE_TX_CARRIER;
            }

            /* activate carrier and DTE */
            if (acmInfo->dteStatus & USB_DEVICE_CDC_CONTROL_SIG_BITMAP_DTE_PRESENCE)
            {
                acmInfo->uartState |= USB_DEVICE_CDC_UART_STATE_RX_CARRIER;
            }
            else
            {
                acmInfo->uartState &= (uint16_t)~USB_DEVICE_CDC_UART_STATE_RX_CARRIER;
            }

            /* Indicates to DCE if DTE is present or not */
            acmInfo->dtePresent = (acmInfo->dteStatus & USB_DEVICE_CDC_CONTROL_SIG_BITMAP_DTE_PRESENCE) ? true : false;

            /* Initialize the serial state buffer */
            acmInfo->serialStateBuf[0] = NOTIF_REQUEST_TYPE;                /* bmRequestType */
            acmInfo->serialStateBuf[1] = USB_DEVICE_CDC_NOTIF_SERIAL_STATE; /* bNotification */
            acmInfo->serialStateBuf[2] = 0x00;                              /* wValue */
            acmInfo->serialStateBuf[3] = 0x00;
            acmInfo->serialStateBuf[4] = 0x00; /* wIndex */
            acmInfo->serialStateBuf[5] = 0x00;
            acmInfo->serialStateBuf[6] = UART_BITMAP_SIZE; /* wLength */
            acmInfo->serialStateBuf[7] = 0x00;
            /* Notifiy to host the line state */
            acmInfo->serialStateBuf[4] = acmReqParam->interfaceIndex;
            /* Lower byte of UART BITMAP */
            uartBitmap    = (uint8_t *)&acmInfo->serialStateBuf[NOTIF_PACKET_SIZE + UART_BITMAP_SIZE - 2];
            uartBitmap[0] = acmInfo->uartState & 0xFFu;
            uartBitmap[1] = (acmInfo->uartState >> 8) & 0xFFu;
            len           = (uint32_t)(NOTIF_PACKET_SIZE + UART_BITMAP_SIZE);
            if (0 == ((usb_device_cdc_acm_struct_t *)handle)->hasSentState)
            {
                error = USB_DeviceCdcAcmSend(handle, USB_CDC_VCOM_INTERRUPT_IN_ENDPOINT, acmInfo->serialStateBuf, len);
                if (kStatus_USB_Success != error)
                {
                    usb_echo("kUSB_DeviceCdcEventSetControlLineState error!");
                }
                ((usb_device_cdc_acm_struct_t *)handle)->hasSentState = 1;
            }

            /* Update status */
            if (acmInfo->dteStatus & USB_DEVICE_CDC_CONTROL_SIG_BITMAP_CARRIER_ACTIVATION)
            {
                /*  To do: CARRIER_ACTIVATED */
            }
            else
            {
                /* To do: CARRIER_DEACTIVATED */
            }
            if (acmInfo->dteStatus & USB_DEVICE_CDC_CONTROL_SIG_BITMAP_DTE_PRESENCE)
            {
                /* DTE_ACTIVATED */
                if (1 == serialUsbCdc->attach)
                {
                    serialUsbCdc->startTransactions = 1;
                }
            }
            else
            {
                /* DTE_DEACTIVATED */
                if (1 == serialUsbCdc->attach)
                {
                    serialUsbCdc->startTransactions = 0;
                }
            }
        }
        break;
        case kUSB_DeviceCdcEventSendBreak:
            break;
        default:
            break;
    }

    return error;
}

/*!
 * @brief USB device callback function.
 *
 * This function handles the usb device specific requests.
 *
 * @param handle          The USB device handle.
 * @param event           The USB device event type.
 * @param param           The parameter of the device specific request.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
static usb_status_t USB_DeviceCallback(usb_device_handle handle, uint32_t event, void *param)
{
    serial_usb_cdc_state_t *serialUsbCdc;
    usb_status_t error = kStatus_USB_Error;
    uint16_t *temp16   = (uint16_t *)param;
    uint8_t *temp8     = (uint8_t *)param;

    serialUsbCdc = USB_DeviceGetInstanceFromDeviceHandle(s_UsbCdcHead, handle);
    if (NULL == serialUsbCdc)
    {
        return kStatus_USB_Error;
    }

    switch (event)
    {
        case kUSB_DeviceEventBusReset:
        {
            serialUsbCdc->attach            = 0;
            serialUsbCdc->startTransactions = 0;
#if (defined(USB_DEVICE_CONFIG_EHCI) && (USB_DEVICE_CONFIG_EHCI > 0U)) || \
    (defined(USB_DEVICE_CONFIG_LPCIP3511HS) && (USB_DEVICE_CONFIG_LPCIP3511HS > 0U))
            /* Get USB speed to configure the device, including max packet size and interval of the endpoints. */
            if (kStatus_USB_Success == USB_DeviceClassGetSpeed(serialUsbCdc->instance, &serialUsbCdc->speed))
            {
                USB_DeviceSetSpeed(handle, serialUsbCdc->speed);
            }
#endif
        }
        break;
        case kUSB_DeviceEventSetConfiguration:
            if (param)
            {
                serialUsbCdc->attach               = 1;
                serialUsbCdc->currentConfiguration = *temp8;
                if (USB_CDC_VCOM_CONFIGURE_INDEX == (*temp8))
                {
                    /* Schedule buffer for receive */
                    Serial_UsbCdcRead(serialUsbCdc, s_currRecvBuf, g_UsbDeviceCdcVcomDicEndpoints[0].maxPacketSize);
                }
            }
            break;
        case kUSB_DeviceEventSetInterface:
            if (serialUsbCdc->attach)
            {
                uint8_t interface        = (uint8_t)((*temp16 & 0xFF00U) >> 0x08U);
                uint8_t alternateSetting = (uint8_t)(*temp16 & 0x00FFU);
                if (interface < USB_CDC_VCOM_INTERFACE_COUNT)
                {
                    serialUsbCdc->currentInterfaceAlternateSetting[interface] = alternateSetting;
                }
            }
            break;
        case kUSB_DeviceEventGetConfiguration:
            break;
        case kUSB_DeviceEventGetInterface:
            break;
        case kUSB_DeviceEventGetDeviceDescriptor:
            if (param)
            {
                error = USB_DeviceGetDeviceDescriptor(handle, (usb_device_get_device_descriptor_struct_t *)param);
            }
            break;
        case kUSB_DeviceEventGetConfigurationDescriptor:
            if (param)
            {
                error = USB_DeviceGetConfigurationDescriptor(handle,
                                                             (usb_device_get_configuration_descriptor_struct_t *)param);
            }
            break;
        case kUSB_DeviceEventGetStringDescriptor:
            if (param)
            {
                /* Get device string descriptor request */
                error = USB_DeviceGetStringDescriptor(handle, (usb_device_get_string_descriptor_struct_t *)param);
            }
            break;
        default:
            break;
    }

    return error;
}

#if (defined(USB_DEVICE_CONFIG_EHCI) && (USB_DEVICE_CONFIG_EHCI > 0U))
void USB_OTG2_IRQHandler(void)
{
    serial_usb_cdc_state_t *serialUsbCdc = s_UsbCdcHead;

    while (serialUsbCdc)
    {
        if ((kSerialManager_UsbControllerEhci1 == serialUsbCdc->instance))
        {
            USB_DeviceEhciIsrFunction(serialUsbCdc->deviceHandle);
        }
        serialUsbCdc = serialUsbCdc->next;
    }
}
#endif

#if defined(USB_DEVICE_CONFIG_EHCI) && (USB_DEVICE_CONFIG_EHCI > 0U)
void USBHS_IRQHandler(void)
{
    serial_usb_cdc_state_t *serialUsbCdc = s_UsbCdcHead;

    while (serialUsbCdc)
    {
        if ((kSerialManager_UsbControllerEhci0 == serialUsbCdc->instance))
        {
            USB_DeviceEhciIsrFunction(serialUsbCdc->deviceHandle);
        }
        serialUsbCdc = serialUsbCdc->next;
    }
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#if defined(USB_DEVICE_CONFIG_EHCI) && (USB_DEVICE_CONFIG_EHCI > 1U)
#if defined(FSL_FEATURE_USBHS_EHCI_COUNT) && (FSL_FEATURE_USBHS_EHCI_COUNT > 1U)
void USB1_IRQHandler(void)
{
    serial_usb_cdc_state_t *serialUsbCdc = s_UsbCdcHead;

    while (serialUsbCdc)
    {
        if ((kSerialManager_UsbControllerEhci1 == serialUsbCdc->instance))
        {
            USB_DeviceEhciIsrFunction(serialUsbCdc->deviceHandle);
        }
        serialUsbCdc = serialUsbCdc->next;
    }
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif
#endif
#endif
#if defined(USB_DEVICE_CONFIG_KHCI) && (USB_DEVICE_CONFIG_KHCI > 0U)
void USB0_IRQHandler(void)
{
    serial_usb_cdc_state_t *serialUsbCdc = s_UsbCdcHead;

    while (serialUsbCdc)
    {
        if ((kSerialManager_UsbControllerKhci0 == serialUsbCdc->instance))
        {
            USB_DeviceKhciIsrFunction(serialUsbCdc->deviceHandle);
        }
        serialUsbCdc = serialUsbCdc->next;
    }
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif
#if defined(USB_DEVICE_CONFIG_LPCIP3511FS) && (USB_DEVICE_CONFIG_LPCIP3511FS > 0U)
void USB0_IRQHandler(void)
{
    serial_usb_cdc_state_t *serialUsbCdc = s_UsbCdcHead;

    while (serialUsbCdc)
    {
        if ((kSerialManager_UsbControllerLpcIp3511Fs0 == serialUsbCdc->instance))
        {
            USB_DeviceLpcIp3511IsrFunction(serialUsbCdc->deviceHandle);
        }
        serialUsbCdc = serialUsbCdc->next;
    }
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif
#if defined(USB_DEVICE_CONFIG_LPCIP3511HS) && (USB_DEVICE_CONFIG_LPCIP3511HS > 0U)
void USB1_IRQHandler(void)
{
    serial_usb_cdc_state_t *serialUsbCdc = s_UsbCdcHead;

    while (serialUsbCdc)
    {
        if ((kSerialManager_UsbControllerLpcIp3511Hs0 == serialUsbCdc->instance))
        {
            USB_DeviceLpcIp3511IsrFunction(serialUsbCdc->deviceHandle);
        }
        serialUsbCdc = serialUsbCdc->next;
    }
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif

static void USB_DeviceIsrEnable(serial_usb_cdc_state_t *serialUsbCdc)
{
#if defined(USB_DEVICE_CONFIG_EHCI) && (USB_DEVICE_CONFIG_EHCI > 0U)
    uint8_t usbDeviceEhciIrq[] = USBHS_IRQS;
    serialUsbCdc->irqNumber    = usbDeviceEhciIrq[serialUsbCdc->instance - kSerialManager_UsbControllerEhci0];
#endif

#if defined(USB_DEVICE_CONFIG_KHCI) && (USB_DEVICE_CONFIG_KHCI > 0U)
    uint8_t usbDeviceKhciIrq[] = USB_IRQS;
    serialUsbCdc->irqNumber    = usbDeviceKhciIrq[serialUsbCdc->instance - kSerialManager_UsbControllerKhci0];
#endif

#if defined(USB_DEVICE_CONFIG_LPCIP3511FS) && (USB_DEVICE_CONFIG_LPCIP3511FS > 0U)
    uint8_t usbDeviceIP3511Irq[] = USB_IRQS;
    serialUsbCdc->irqNumber = usbDeviceIP3511Irq[serialUsbCdc->instance - kSerialManager_UsbControllerLpcIp3511Fs0];
#endif

#if defined(USB_DEVICE_CONFIG_LPCIP3511HS) && (USB_DEVICE_CONFIG_LPCIP3511HS > 0U)
    uint8_t usbDeviceIP3511Irq[] = USBHSD_IRQS;
    serialUsbCdc->irqNumber = usbDeviceIP3511Irq[serialUsbCdc->instance - kSerialManager_UsbControllerLpcIp3511Hs0];
#endif

/* Install isr, set priority, and enable IRQ. */
#if defined(__GIC_PRIO_BITS)
    GIC_SetPriority((IRQn_Type)serialUsbCdc->irqNumber, USB_DEVICE_INTERRUPT_PRIORITY);
#else
    NVIC_SetPriority((IRQn_Type)serialUsbCdc->irqNumber, USB_DEVICE_INTERRUPT_PRIORITY);
#endif
    EnableIRQ((IRQn_Type)serialUsbCdc->irqNumber);
}

serial_manager_status_t Serial_UsbCdcInit(serial_handle_t serialHandle, void *serialConfig)
{
    serial_usb_cdc_state_t *serialUsbCdc;
    serial_port_usb_cdc_config_t *usbCdcConfig;

    assert(serialConfig);
    assert(serialHandle);
    if (SERIAL_PORT_USB_CDC_HANDLE_SIZE < sizeof(serial_usb_cdc_state_t))
    {
        return kStatus_SerialManager_Error;
    }

#if (defined(FSL_FEATURE_SOC_SYSMPU_COUNT) && (FSL_FEATURE_SOC_SYSMPU_COUNT > 0U))
    SYSMPU_Enable(SYSMPU, 0);
#endif /* FSL_FEATURE_SOC_SYSMPU_COUNT */

    usbCdcConfig = (serial_port_usb_cdc_config_t *)serialConfig;
    serialUsbCdc = (serial_usb_cdc_state_t *)serialHandle;

    serialUsbCdc->speed    = USB_SPEED_FULL;
    serialUsbCdc->instance = usbCdcConfig->controllerIndex;

    if (kStatus_USB_Success !=
        USB_DeviceClassInit(serialUsbCdc->instance, &s_cdcAcmConfigList, &serialUsbCdc->deviceHandle))
    {
        return kStatus_SerialManager_Error;
    }
    else
    {
        serialUsbCdc->cdcAcmHandle = s_cdcAcmConfigList.config->classHandle;
    }

    USB_DeviceAddItem(&s_UsbCdcHead, serialUsbCdc);

    USB_DeviceIsrEnable(serialUsbCdc);

    USB_DeviceRun(serialUsbCdc->deviceHandle);

    return kStatus_SerialManager_Success;
}

serial_manager_status_t Serial_UsbCdcDeinit(serial_handle_t serialHandle)
{
    serial_usb_cdc_state_t *serialUsbCdc;

    assert(serialHandle);

    serialUsbCdc = (serial_usb_cdc_state_t *)serialHandle;

    if (kStatus_USB_Success != USB_DeviceClassDeinit(serialUsbCdc->instance))
    {
        return kStatus_SerialManager_Error;
    }
    else
    {
        USB_DeviceRemoveItem(&s_UsbCdcHead, serialUsbCdc);
    }

    return kStatus_SerialManager_Success;
}

serial_manager_status_t Serial_UsbCdcWrite(serial_handle_t serialHandle, uint8_t *buffer, uint32_t length)
{
    serial_usb_cdc_state_t *serialUsbCdc;
    uint32_t needToPrime = 0;

    assert(serialHandle);

    serialUsbCdc = (serial_usb_cdc_state_t *)serialHandle;

    if (serialUsbCdc->tx.busy)
    {
        return kStatus_SerialManager_Busy;
    }
    serialUsbCdc->tx.busy          = 1;
    serialUsbCdc->tx.waiting4Prime = 0;

    serialUsbCdc->tx.buffer = buffer;
    serialUsbCdc->tx.length = length;

    if ((serialUsbCdc->attach))
    {
        needToPrime = 1;
    }

    if (needToPrime)
    {
        if (kStatus_USB_Success !=
            USB_DeviceCdcAcmSend(serialUsbCdc->cdcAcmHandle, USB_CDC_VCOM_BULK_IN_ENDPOINT, buffer, length))
        {
            serialUsbCdc->tx.busy = 0;
            return kStatus_SerialManager_Error;
        }
    }
    else
    {
        serialUsbCdc->tx.waiting4Prime = 1;
    }

    return kStatus_SerialManager_Success;
}

serial_manager_status_t Serial_UsbCdcRead(serial_handle_t serialHandle, uint8_t *buffer, uint32_t length)
{
    serial_usb_cdc_state_t *serialUsbCdc;
    uint8_t *primeBuffer;
    uint32_t primeLength;
    uint32_t regPrimask;

    assert(serialHandle);

    serialUsbCdc = (serial_usb_cdc_state_t *)serialHandle;

    if (!(serialUsbCdc->attach))
    {
        return kStatus_SerialManager_Error;
    }

    regPrimask = DisableGlobalIRQ();
    if (serialUsbCdc->rx.busy)
    {
        EnableGlobalIRQ(regPrimask);
        return kStatus_SerialManager_Busy;
    }
    serialUsbCdc->rx.busy = 1U;
    EnableGlobalIRQ(regPrimask);

    if (length < g_UsbDeviceCdcVcomDicEndpoints[0].maxPacketSize)
    {
        serialUsbCdc->rx.busy = 0U;
        return kStatus_SerialManager_Error;
    }

    if (NULL == buffer)
    {
        primeBuffer = s_currRecvBuf;
        primeLength = g_UsbDeviceCdcVcomDicEndpoints[0].maxPacketSize;
    }
    else
    {
        primeBuffer = buffer;
        primeLength = length;
    }
    /* Schedule buffer for next receive event */

    if (kStatus_USB_Success !=
        USB_DeviceCdcAcmRecv(serialUsbCdc->cdcAcmHandle, USB_CDC_VCOM_BULK_OUT_ENDPOINT, primeBuffer, primeLength))
    {
        serialUsbCdc->rx.busy = 0U;
        return kStatus_SerialManager_Error;
    }

    return kStatus_SerialManager_Success;
}

serial_manager_status_t Serial_UsbCdcCancelWrite(serial_handle_t serialHandle)
{
    serial_usb_cdc_state_t *serialUsbCdc;

    assert(serialHandle);

    serialUsbCdc = (serial_usb_cdc_state_t *)serialHandle;
    USB_DeviceCancel(serialUsbCdc->deviceHandle,
                     (USB_IN << USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT) | USB_CDC_VCOM_BULK_IN_ENDPOINT);
    return kStatus_SerialManager_Success;
}

serial_manager_status_t Serial_UsbCdcInstallTxCallback(serial_handle_t serialHandle,
                                                       serial_manager_callback_t callback,
                                                       void *callbackParam)
{
    serial_usb_cdc_state_t *serialUsbCdc;

    assert(serialHandle);

    serialUsbCdc = (serial_usb_cdc_state_t *)serialHandle;

    serialUsbCdc->tx.callback      = callback;
    serialUsbCdc->tx.callbackParam = callbackParam;

    return kStatus_SerialManager_Success;
}

serial_manager_status_t Serial_UsbCdcInstallRxCallback(serial_handle_t serialHandle,
                                                       serial_manager_callback_t callback,
                                                       void *callbackParam)
{
    serial_usb_cdc_state_t *serialUsbCdc;

    assert(serialHandle);

    serialUsbCdc = (serial_usb_cdc_state_t *)serialHandle;

    serialUsbCdc->rx.callback      = callback;
    serialUsbCdc->rx.callbackParam = callbackParam;

    return kStatus_SerialManager_Success;
}

void Serial_UsbCdcIsrFunction(serial_handle_t serialHandle)
{
    serial_usb_cdc_state_t *serialUsbCdc;

    assert(serialHandle);

    serialUsbCdc = (serial_usb_cdc_state_t *)serialHandle;

#if 0
    DisableIRQ((IRQn_Type)serialUsbCdc->irqNumber);
#endif
#if defined(USB_DEVICE_CONFIG_EHCI) && (USB_DEVICE_CONFIG_EHCI > 0)
    if ((kSerialManager_UsbControllerEhci0 == serialUsbCdc->instance) ||
        (kSerialManager_UsbControllerEhci1 == serialUsbCdc->instance))
    {
        USB_DeviceEhciIsrFunction(serialUsbCdc->deviceHandle);
    }
#endif
#if defined(USB_DEVICE_CONFIG_KHCI) && (USB_DEVICE_CONFIG_KHCI > 0)
    if ((kSerialManager_UsbControllerKhci0 == serialUsbCdc->instance) ||
        (kSerialManager_UsbControllerKhci1 == serialUsbCdc->instance))
    {
        USB_DeviceKhciIsrFunction(serialUsbCdc->deviceHandle);
    }
#endif
#if defined(USB_DEVICE_CONFIG_LPCIP3511FS) && (USB_DEVICE_CONFIG_LPCIP3511FS > 0U)
    if ((kSerialManager_UsbControllerLpcIp3511Fs0 == serialUsbCdc->instance) ||
        (kSerialManager_UsbControllerLpcIp3511Fs1 == serialUsbCdc->instance))
    {
        USB_DeviceLpcIp3511IsrFunction(serialUsbCdc->deviceHandle);
    }
#endif

#if defined(USB_DEVICE_CONFIG_LPCIP3511HS) && (USB_DEVICE_CONFIG_LPCIP3511HS > 0U)
    if ((kSerialManager_UsbControllerLpcIp3511Hs0 == serialUsbCdc->instance) ||
        (kSerialManager_UsbControllerLpcIp3511Hs1 == serialUsbCdc->instance))
    {
        USB_DeviceLpcIp3511IsrFunction(serialUsbCdc->deviceHandle);
    }
#endif
#if 0
    EnableIRQ((IRQn_Type)serialUsbCdc->irqNumber);
#endif
}
#endif
