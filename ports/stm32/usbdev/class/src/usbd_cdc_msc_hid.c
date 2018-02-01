/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include STM32_HAL_H
#include "usbd_ioreq.h"
#include "usbd_cdc_msc_hid.h"

#define CDC_TEMPLATE_CONFIG_DESC_SIZE (67)
#define CDC_MSC_TEMPLATE_CONFIG_DESC_SIZE (98)
#define CDC_HID_TEMPLATE_CONFIG_DESC_SIZE (107)
#define CDC_HID_TEMPLATE_HID_DESC_OFFSET (9)
#define HID_DESC_OFFSET_SUBCLASS (6)
#define HID_DESC_OFFSET_PROTOCOL (7)
#define HID_DESC_OFFSET_SUBDESC (9)
#define HID_DESC_OFFSET_REPORT_DESC_LEN (16)
#define HID_DESC_OFFSET_MAX_PACKET_LO (22)
#define HID_DESC_OFFSET_MAX_PACKET_HI (23)
#define HID_DESC_OFFSET_POLLING_INTERVAL (24)
#define HID_DESC_OFFSET_MAX_PACKET_OUT_LO (29)
#define HID_DESC_OFFSET_MAX_PACKET_OUT_HI (30)
#define HID_DESC_OFFSET_POLLING_INTERVAL_OUT (31)
#define HID_SUBDESC_LEN (9)

#define CDC_IFACE_NUM_ALONE (0)
#define CDC_IFACE_NUM_WITH_MSC (1)
#define CDC_IFACE_NUM_WITH_HID (1)
#define MSC_IFACE_NUM_WITH_CDC (0)
#define HID_IFACE_NUM_WITH_CDC (0)
#define HID_IFACE_NUM_WITH_MSC (1)
#define HID_IN_EP_WITH_CDC (0x81)
#define HID_OUT_EP_WITH_CDC (0x01)
#define HID_IN_EP_WITH_MSC (0x83)
#define HID_OUT_EP_WITH_MSC (0x03)

#define USB_DESC_TYPE_ASSOCIATION (0x0b)

#define CDC_CMD_PACKET_SIZE         (8)  // Control Endpoint Packet size
#define CDC_DATA_IN_PACKET_SIZE     CDC_DATA_FS_MAX_PACKET_SIZE
#define CDC_DATA_OUT_PACKET_SIZE    CDC_DATA_FS_MAX_PACKET_SIZE

#define MSC_MAX_PACKET          (0x40)
#define BOT_GET_MAX_LUN         (0xfe)
#define BOT_RESET               (0xff)

#define HID_DESCRIPTOR_TYPE     (0x21)
#define HID_REPORT_DESC         (0x22)
#define HID_REQ_SET_PROTOCOL    (0x0b)
#define HID_REQ_GET_PROTOCOL    (0x03)
#define HID_REQ_SET_IDLE        (0x0a)
#define HID_REQ_GET_IDLE        (0x02)

/*
// this is used only in high-speed mode, which we don't support
// USB Standard Device Descriptor
__ALIGN_BEGIN static uint8_t USBD_CDC_MSC_HID_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC] __ALIGN_END = {
    USB_LEN_DEV_QUALIFIER_DESC,
    USB_DESC_TYPE_DEVICE_QUALIFIER,
    0x00,
    0x02,
    0x00,
    0x00,
    0x00,
    0x40, // same for CDC and MSC (latter being MSC_MAX_PACKET), HID is 0x04
    0x01,
    0x00,
};
*/

// USB CDC MSC device Configuration Descriptor
static const uint8_t cdc_msc_template_config_desc[CDC_MSC_TEMPLATE_CONFIG_DESC_SIZE] = {
    //--------------------------------------------------------------------------
    // Configuration Descriptor
    0x09,   // bLength: Configuration Descriptor size
    USB_DESC_TYPE_CONFIGURATION, // bDescriptorType: Configuration
    LOBYTE(CDC_MSC_TEMPLATE_CONFIG_DESC_SIZE), // wTotalLength: no of returned bytes
    HIBYTE(CDC_MSC_TEMPLATE_CONFIG_DESC_SIZE),
    0x03,   // bNumInterfaces: 3 interfaces
    0x01,   // bConfigurationValue: Configuration value
    0x00,   // iConfiguration: Index of string descriptor describing the configuration
    0x80,   // bmAttributes: bus powered; 0xc0 for self powered
    0xfa,   // bMaxPower: in units of 2mA

    //==========================================================================
    // MSC only has 1 interface so doesn't need an IAD

    //--------------------------------------------------------------------------
    // Interface Descriptor
    0x09,   // bLength: Interface Descriptor size
    USB_DESC_TYPE_INTERFACE, // bDescriptorType: interface descriptor
    MSC_IFACE_NUM_WITH_CDC, // bInterfaceNumber: Number of Interface
    0x00,   // bAlternateSetting: Alternate setting
    0x02,   // bNumEndpoints
    0x08,   // bInterfaceClass: MSC Class
    0x06,   // bInterfaceSubClass : SCSI transparent
    0x50,   // nInterfaceProtocol
    0x00,   // iInterface:

    // Endpoint IN descriptor
    0x07,                           // bLength: Endpoint descriptor length
    USB_DESC_TYPE_ENDPOINT,         // bDescriptorType: Endpoint descriptor type
    MSC_IN_EP,                      // bEndpointAddress: IN, address 3
    0x02,                           // bmAttributes: Bulk endpoint type
    LOBYTE(MSC_MAX_PACKET),         // wMaxPacketSize
    HIBYTE(MSC_MAX_PACKET),
    0x00,                           // bInterval: ignore for Bulk transfer

    // Endpoint OUT descriptor
    0x07,                           // bLength: Endpoint descriptor length
    USB_DESC_TYPE_ENDPOINT,         // bDescriptorType: Endpoint descriptor type
    MSC_OUT_EP,                     // bEndpointAddress: OUT, address 3
    0x02,                           // bmAttributes: Bulk endpoint type
    LOBYTE(MSC_MAX_PACKET),         // wMaxPacketSize
    HIBYTE(MSC_MAX_PACKET),
    0x00,                           // bInterval: ignore for Bulk transfer

    //==========================================================================
    // Interface Association for CDC VCP
    0x08,   // bLength: 8 bytes
    USB_DESC_TYPE_ASSOCIATION, // bDescriptorType: IAD
    CDC_IFACE_NUM_WITH_MSC, // bFirstInterface: first interface for this association
    0x02,   // bInterfaceCount: nummber of interfaces for this association
    0x02,   // bFunctionClass: Communication Interface Class
    0x02,   // bFunctionSubClass: Abstract Control Model
    0x01,   // bFunctionProtocol: Common AT commands
    0x00,   // iFunction: index of string for this function

    //--------------------------------------------------------------------------
    // Interface Descriptor
    0x09,   // bLength: Interface Descriptor size
    USB_DESC_TYPE_INTERFACE, // bDescriptorType: Interface
    CDC_IFACE_NUM_WITH_MSC, // bInterfaceNumber: Number of Interface
    0x00,   // bAlternateSetting: Alternate setting
    0x01,   // bNumEndpoints: One endpoints used
    0x02,   // bInterfaceClass: Communication Interface Class
    0x02,   // bInterfaceSubClass: Abstract Control Model
    0x01,   // bInterfaceProtocol: Common AT commands
    0x00,   // iInterface:

    // Header Functional Descriptor
    0x05,   // bLength: Endpoint Descriptor size
    0x24,   // bDescriptorType: CS_INTERFACE
    0x00,   // bDescriptorSubtype: Header Func Desc
    0x10,   // bcdCDC: spec release number
    0x01,   // ?

    // Call Management Functional Descriptor
    0x05,   // bFunctionLength
    0x24,   // bDescriptorType: CS_INTERFACE
    0x01,   // bDescriptorSubtype: Call Management Func Desc
    0x00,   // bmCapabilities: D0+D1
    CDC_IFACE_NUM_WITH_MSC + 1,   // bDataInterface: 1

    // ACM Functional Descriptor
    0x04,   // bFunctionLength
    0x24,   // bDescriptorType: CS_INTERFACE
    0x02,   // bDescriptorSubtype: Abstract Control Management desc
    0x02,   // bmCapabilities

    // Union Functional Descriptor
    0x05,   // bFunctionLength
    0x24,   // bDescriptorType: CS_INTERFACE
    0x06,   // bDescriptorSubtype: Union func desc
    CDC_IFACE_NUM_WITH_MSC + 0,   // bMasterInterface: Communication class interface
    CDC_IFACE_NUM_WITH_MSC + 1,   // bSlaveInterface0: Data Class Interface

    // Endpoint 2 Descriptor
    0x07,                           // bLength: Endpoint Descriptor size
    USB_DESC_TYPE_ENDPOINT,         // bDescriptorType: Endpoint
    CDC_CMD_EP,                     // bEndpointAddress
    0x03,                           // bmAttributes: Interrupt
    LOBYTE(CDC_CMD_PACKET_SIZE),    // wMaxPacketSize:
    HIBYTE(CDC_CMD_PACKET_SIZE),
    0x20,                           // bInterval: polling interval in frames of 1ms

    //--------------------------------------------------------------------------
    // Data class interface descriptor
    0x09,   // bLength: Endpoint Descriptor size
    USB_DESC_TYPE_INTERFACE, // bDescriptorType: interface
    CDC_IFACE_NUM_WITH_MSC + 1,   // bInterfaceNumber: Number of Interface
    0x00,   // bAlternateSetting: Alternate setting
    0x02,   // bNumEndpoints: Two endpoints used
    0x0A,   // bInterfaceClass: CDC
    0x00,   // bInterfaceSubClass: ?
    0x00,   // bInterfaceProtocol: ?
    0x00,   // iInterface:

    // Endpoint OUT Descriptor
    0x07,                               // bLength: Endpoint Descriptor size
    USB_DESC_TYPE_ENDPOINT,             // bDescriptorType: Endpoint
    CDC_OUT_EP,                         // bEndpointAddress
    0x02,                               // bmAttributes: Bulk
    LOBYTE(CDC_DATA_FS_MAX_PACKET_SIZE),// wMaxPacketSize:
    HIBYTE(CDC_DATA_FS_MAX_PACKET_SIZE),
    0x00,                               // bInterval: ignore for Bulk transfer

    // Endpoint IN Descriptor
    0x07,                               // bLength: Endpoint Descriptor size
    USB_DESC_TYPE_ENDPOINT,             // bDescriptorType: Endpoint
    CDC_IN_EP,                          // bEndpointAddress
    0x02,                               // bmAttributes: Bulk
    LOBYTE(CDC_DATA_FS_MAX_PACKET_SIZE),// wMaxPacketSize:
    HIBYTE(CDC_DATA_FS_MAX_PACKET_SIZE),
    0x00,                               // bInterval: ignore for Bulk transfer
};

// USB CDC HID device Configuration Descriptor
static const uint8_t cdc_hid_template_config_desc[CDC_HID_TEMPLATE_CONFIG_DESC_SIZE] = {
    //--------------------------------------------------------------------------
    // Configuration Descriptor
    0x09,   // bLength: Configuration Descriptor size
    USB_DESC_TYPE_CONFIGURATION, // bDescriptorType: Configuration
    LOBYTE(CDC_HID_TEMPLATE_CONFIG_DESC_SIZE), // wTotalLength: no of returned bytes
    HIBYTE(CDC_HID_TEMPLATE_CONFIG_DESC_SIZE),
    0x03,   // bNumInterfaces: 3 interfaces
    0x01,   // bConfigurationValue: Configuration value
    0x00,   // iConfiguration: Index of string descriptor describing the configuration
    0x80,   // bmAttributes: bus powered; 0xc0 for self powered
    0xfa,   // bMaxPower: in units of 2mA

    //==========================================================================
    // HID only has 1 interface so doesn't need an IAD

    //--------------------------------------------------------------------------
    // Interface Descriptor
    0x09,   // bLength: Interface Descriptor size
    USB_DESC_TYPE_INTERFACE, // bDescriptorType: interface descriptor
    HID_IFACE_NUM_WITH_CDC, // bInterfaceNumber: Number of Interface
    0x00,   // bAlternateSetting: Alternate setting
    0x02,   // bNumEndpoints
    0x03,   // bInterfaceClass: HID Class
    0x01,   // bInterfaceSubClass: 0=no sub class, 1=boot
    0x02,   // nInterfaceProtocol: 0=none, 1=keyboard, 2=mouse
    0x00,   // iInterface:

    // HID descriptor
    0x09,                   // bLength: HID Descriptor size
    HID_DESCRIPTOR_TYPE,    // bDescriptorType: HID
    0x11,                   // bcdHID: HID Class Spec release number
    0x01,
    0x00,                   // bCountryCode: Hardware target country
    0x01,                   // bNumDescriptors: Number of HID class descriptors to follow
    0x22,                   // bDescriptorType
    USBD_HID_MOUSE_REPORT_DESC_SIZE, // wItemLength: Total length of Report descriptor
    0x00,

    // Endpoint IN descriptor
    0x07,                           // bLength: Endpoint descriptor length
    USB_DESC_TYPE_ENDPOINT,         // bDescriptorType: Endpoint descriptor type
    HID_IN_EP_WITH_CDC,             // bEndpointAddress: IN
    0x03,                           // bmAttributes: Interrupt endpoint type
    LOBYTE(USBD_HID_MOUSE_MAX_PACKET), // wMaxPacketSize
    HIBYTE(USBD_HID_MOUSE_MAX_PACKET),
    0x08,                           // bInterval: Polling interval

    // Endpoint OUT descriptor
    0x07,                           // bLength: Endpoint descriptor length
    USB_DESC_TYPE_ENDPOINT,         // bDescriptorType: Endpoint descriptor type
    HID_OUT_EP_WITH_CDC,             // bEndpointAddress: OUT
    0x03,                           // bmAttributes: Interrupt endpoint type
    LOBYTE(USBD_HID_MOUSE_MAX_PACKET), // wMaxPacketSize
    HIBYTE(USBD_HID_MOUSE_MAX_PACKET),
    0x08,                           // bInterval: Polling interval

    //==========================================================================
    // Interface Association for CDC VCP
    0x08,   // bLength: 8 bytes
    USB_DESC_TYPE_ASSOCIATION, // bDescriptorType: IAD
    CDC_IFACE_NUM_WITH_HID, // bFirstInterface: first interface for this association
    0x02,   // bInterfaceCount: nummber of interfaces for this association
    0x02,   // bFunctionClass: Communication Interface Class
    0x02,   // bFunctionSubClass: Abstract Control Model
    0x01,   // bFunctionProtocol: Common AT commands
    0x00,   // iFunction: index of string for this function

    //--------------------------------------------------------------------------
    // Interface Descriptor
    0x09,   // bLength: Interface Descriptor size
    USB_DESC_TYPE_INTERFACE, // bDescriptorType: Interface
    CDC_IFACE_NUM_WITH_HID, // bInterfaceNumber: Number of Interface
    0x00,   // bAlternateSetting: Alternate setting
    0x01,   // bNumEndpoints: One endpoints used
    0x02,   // bInterfaceClass: Communication Interface Class
    0x02,   // bInterfaceSubClass: Abstract Control Model
    0x01,   // bInterfaceProtocol: Common AT commands
    0x00,   // iInterface:

    // Header Functional Descriptor
    0x05,   // bLength: Endpoint Descriptor size
    0x24,   // bDescriptorType: CS_INTERFACE
    0x00,   // bDescriptorSubtype: Header Func Desc
    0x10,   // bcdCDC: spec release number
    0x01,   // ?

    // Call Management Functional Descriptor
    0x05,   // bFunctionLength
    0x24,   // bDescriptorType: CS_INTERFACE
    0x01,   // bDescriptorSubtype: Call Management Func Desc
    0x00,   // bmCapabilities: D0+D1
    CDC_IFACE_NUM_WITH_HID + 1,   // bDataInterface: 1

    // ACM Functional Descriptor
    0x04,   // bFunctionLength
    0x24,   // bDescriptorType: CS_INTERFACE
    0x02,   // bDescriptorSubtype: Abstract Control Management desc
    0x02,   // bmCapabilities

    // Union Functional Descriptor
    0x05,   // bFunctionLength
    0x24,   // bDescriptorType: CS_INTERFACE
    0x06,   // bDescriptorSubtype: Union func desc
    CDC_IFACE_NUM_WITH_HID + 0,   // bMasterInterface: Communication class interface
    CDC_IFACE_NUM_WITH_HID + 1,   // bSlaveInterface0: Data Class Interface

    // Endpoint 2 Descriptor
    0x07,                           // bLength: Endpoint Descriptor size
    USB_DESC_TYPE_ENDPOINT,         // bDescriptorType: Endpoint
    CDC_CMD_EP,                     // bEndpointAddress
    0x03,                           // bmAttributes: Interrupt
    LOBYTE(CDC_CMD_PACKET_SIZE),    // wMaxPacketSize:
    HIBYTE(CDC_CMD_PACKET_SIZE),
    0x20,                           // bInterval: polling interval in frames of 1ms

    //--------------------------------------------------------------------------
    // Data class interface descriptor
    0x09,   // bLength: Endpoint Descriptor size
    USB_DESC_TYPE_INTERFACE, // bDescriptorType: interface
    CDC_IFACE_NUM_WITH_HID + 1,   // bInterfaceNumber: Number of Interface
    0x00,   // bAlternateSetting: Alternate setting
    0x02,   // bNumEndpoints: Two endpoints used
    0x0A,   // bInterfaceClass: CDC
    0x00,   // bInterfaceSubClass: ?
    0x00,   // bInterfaceProtocol: ?
    0x00,   // iInterface:

    // Endpoint OUT Descriptor
    0x07,                               // bLength: Endpoint Descriptor size
    USB_DESC_TYPE_ENDPOINT,             // bDescriptorType: Endpoint
    CDC_OUT_EP,                         // bEndpointAddress
    0x02,                               // bmAttributes: Bulk
    LOBYTE(CDC_DATA_FS_MAX_PACKET_SIZE),// wMaxPacketSize:
    HIBYTE(CDC_DATA_FS_MAX_PACKET_SIZE),
    0x00,                               // bInterval: ignore for Bulk transfer

    // Endpoint IN Descriptor
    0x07,                               // bLength: Endpoint Descriptor size
    USB_DESC_TYPE_ENDPOINT,             // bDescriptorType: Endpoint
    CDC_IN_EP,                          // bEndpointAddress
    0x02,                               // bmAttributes: Bulk
    LOBYTE(CDC_DATA_FS_MAX_PACKET_SIZE),// wMaxPacketSize:
    HIBYTE(CDC_DATA_FS_MAX_PACKET_SIZE),
    0x00,                               // bInterval: ignore for Bulk transfer
};

static const uint8_t cdc_template_config_desc[CDC_TEMPLATE_CONFIG_DESC_SIZE] = {
    //--------------------------------------------------------------------------
    // Configuration Descriptor
    0x09,   // bLength: Configuration Descriptor size
    USB_DESC_TYPE_CONFIGURATION, // bDescriptorType: Configuration
    LOBYTE(CDC_TEMPLATE_CONFIG_DESC_SIZE), // wTotalLength:no of returned bytes
    HIBYTE(CDC_TEMPLATE_CONFIG_DESC_SIZE),
    0x02,   // bNumInterfaces: 2 interface
    0x01,   // bConfigurationValue: Configuration value
    0x00,   // iConfiguration: Index of string descriptor describing the configuration
    0x80,   // bmAttributes: bus powered; 0xc0 for self powered
    0xfa,   // bMaxPower: in units of 2mA

    //--------------------------------------------------------------------------
    // Interface Descriptor
    0x09,   // bLength: Interface Descriptor size
    USB_DESC_TYPE_INTERFACE,  // bDescriptorType: Interface
    CDC_IFACE_NUM_ALONE,   // bInterfaceNumber: Number of Interface
    0x00,   // bAlternateSetting: Alternate setting
    0x01,   // bNumEndpoints: One endpoints used
    0x02,   // bInterfaceClass: Communication Interface Class
    0x02,   // bInterfaceSubClass: Abstract Control Model
    0x01,   // bInterfaceProtocol: Common AT commands
    0x00,   // iInterface:

    // Header Functional Descriptor
    0x05,   // bLength: Endpoint Descriptor size
    0x24,   // bDescriptorType: CS_INTERFACE
    0x00,   // bDescriptorSubtype: Header Func Desc
    0x10,   // bcdCDC: spec release number
    0x01,   // ?

    // Call Management Functional Descriptor
    0x05,   // bFunctionLength
    0x24,   // bDescriptorType: CS_INTERFACE
    0x01,   // bDescriptorSubtype: Call Management Func Desc
    0x00,   // bmCapabilities: D0+D1
    CDC_IFACE_NUM_ALONE + 1,   // bDataInterface: 1

    // ACM Functional Descriptor
    0x04,   // bFunctionLength
    0x24,   // bDescriptorType: CS_INTERFACE
    0x02,   // bDescriptorSubtype: Abstract Control Management desc
    0x02,   // bmCapabilities

    // Union Functional Descriptor
    0x05,   // bFunctionLength
    0x24,   // bDescriptorType: CS_INTERFACE
    0x06,   // bDescriptorSubtype: Union func desc
    CDC_IFACE_NUM_ALONE + 0,   // bMasterInterface: Communication class interface
    CDC_IFACE_NUM_ALONE + 1,   // bSlaveInterface0: Data Class Interface

    // Endpoint 2 Descriptor
    0x07,                           // bLength: Endpoint Descriptor size
    USB_DESC_TYPE_ENDPOINT,         // bDescriptorType: Endpoint
    CDC_CMD_EP,                     // bEndpointAddress
    0x03,                           // bmAttributes: Interrupt
    LOBYTE(CDC_CMD_PACKET_SIZE),    // wMaxPacketSize:
    HIBYTE(CDC_CMD_PACKET_SIZE),
    0x20,                           // bInterval: polling interval in frames of 1ms

    //--------------------------------------------------------------------------
    // Data class interface descriptor
    0x09,   // bLength: Endpoint Descriptor size
    USB_DESC_TYPE_INTERFACE,  // bDescriptorType:
    CDC_IFACE_NUM_ALONE + 1,   // bInterfaceNumber: Number of Interface
    0x00,   // bAlternateSetting: Alternate setting
    0x02,   // bNumEndpoints: Two endpoints used
    0x0a,   // bInterfaceClass: CDC
    0x00,   // bInterfaceSubClass: ?
    0x00,   // bInterfaceProtocol: ?
    0x00,   // iInterface:

    // Endpoint OUT Descriptor
    0x07,                               // bLength: Endpoint Descriptor size
    USB_DESC_TYPE_ENDPOINT,             // bDescriptorType: Endpoint
    CDC_OUT_EP,                         // bEndpointAddress
    0x02,                               // bmAttributes: Bulk
    LOBYTE(CDC_DATA_FS_MAX_PACKET_SIZE),// wMaxPacketSize:
    HIBYTE(CDC_DATA_FS_MAX_PACKET_SIZE),
    0x00,                               // bInterval: ignore for Bulk transfer

    // Endpoint IN Descriptor
    0x07,                               // bLength: Endpoint Descriptor size
    USB_DESC_TYPE_ENDPOINT,             // bDescriptorType: Endpoint
    CDC_IN_EP,                          // bEndpointAddress
    0x02,                               // bmAttributes: Bulk
    LOBYTE(CDC_DATA_FS_MAX_PACKET_SIZE),// wMaxPacketSize:
    HIBYTE(CDC_DATA_FS_MAX_PACKET_SIZE),
    0x00                                // bInterval: ignore for Bulk transfer
};

__ALIGN_BEGIN const uint8_t USBD_HID_MOUSE_ReportDesc[USBD_HID_MOUSE_REPORT_DESC_SIZE] __ALIGN_END = {
    0x05, 0x01,     // Usage Page (Generic Desktop),
    0x09, 0x02,     // Usage (Mouse),
    0xA1, 0x01,     // Collection (Application),
    0x09, 0x01,         // Usage (Pointer),
    0xA1, 0x00,         // Collection (Physical),
    0x05, 0x09,         // Usage Page (Buttons),
    0x19, 0x01,         // Usage Minimum (01),
    0x29, 0x03,         // Usage Maximum (03),
    0x15, 0x00,         // Logical Minimum (0),
    0x25, 0x01,         // Logical Maximum (1),
    0x95, 0x03,         // Report Count (3),
    0x75, 0x01,         // Report Size (1),
    0x81, 0x02,         // Input(Data, Variable, Absolute), -- 3 button bits
    0x95, 0x01,         // Report Count(1),
    0x75, 0x05,         // Report Size(5),
    0x81, 0x01,         // Input(Constant),                 -- 5 bit padding
    0x05, 0x01,         // Usage Page (Generic Desktop),
    0x09, 0x30,         // Usage (X),
    0x09, 0x31,         // Usage (Y),
    0x09, 0x38,         // Usage (Wheel),
    0x15, 0x81,         // Logical Minimum (-127),
    0x25, 0x7F,         // Logical Maximum (127),
    0x75, 0x08,         // Report Size (8),
    0x95, 0x03,         // Report Count (3),
    0x81, 0x06,         // Input(Data, Variable, Relative), -- 3 position bytes (X,Y,Wheel)
    0xC0,           // End Collection,
    0x09, 0x3c,     // Usage (Motion Wakeup),
    0x05, 0xff,     // Usage Page (?),
    0x09, 0x01,     // Usage (?),
    0x15, 0x00,     // Logical Minimum (0),
    0x25, 0x01,     // Logical Maximum (1),
    0x75, 0x01,     // Report Size(1),
    0x95, 0x02,     // Report Count(2),
    0xb1, 0x22,     // ?
    0x75, 0x06,     // Report Size(6),
    0x95, 0x01,     // Report Count(1),
    0xb1, 0x01,     // ?
    0xc0            // End Collection
};

__ALIGN_BEGIN const uint8_t USBD_HID_KEYBOARD_ReportDesc[USBD_HID_KEYBOARD_REPORT_DESC_SIZE] __ALIGN_END = {
    // From p69 of http://www.usb.org/developers/devclass_docs/HID1_11.pdf
    0x05, 0x01,     // Usage Page (Generic Desktop),
    0x09, 0x06,     // Usage (Keyboard),
    0xA1, 0x01,     // Collection (Application),
    0x05, 0x07,         // Usage Page (Key Codes);
    0x19, 0xE0,         // Usage Minimum (224),
    0x29, 0xE7,         // Usage Maximum (231),
    0x15, 0x00,         // Logical Minimum (0),
    0x25, 0x01,         // Logical Maximum (1),
    0x75, 0x01,         // Report Size (1),
    0x95, 0x08,         // Report Count (8),
    0x81, 0x02,         // Input (Data, Variable, Absolute), ;Modifier byte
    0x95, 0x01,         // Report Count (1),
    0x75, 0x08,         // Report Size (8),
    0x81, 0x01,         // Input (Constant), ;Reserved byte
    0x95, 0x05,         // Report Count (5),
    0x75, 0x01,         // Report Size (1),
    0x05, 0x08,         // Usage Page (Page# for LEDs),
    0x19, 0x01,         // Usage Minimum (1),
    0x29, 0x05,         // Usage Maximum (5),
    0x91, 0x02,         // Output (Data, Variable, Absolute), ;LED report
    0x95, 0x01,         // Report Count (1),
    0x75, 0x03,         // Report Size (3),
    0x91, 0x01,         // Output (Constant), ;LED report padding
    0x95, 0x06,         // Report Count (6),
    0x75, 0x08,         // Report Size (8),
    0x15, 0x00,         // Logical Minimum (0),
    0x25, 0x65,         // Logical Maximum(101),
    0x05, 0x07,         // Usage Page (Key Codes),
    0x19, 0x00,         // Usage Minimum (0),
    0x29, 0x65,         // Usage Maximum (101),
    0x81, 0x00,         // Input (Data, Array), ;Key arrays (6 bytes)
    0xC0            // End Collection
};

// return the saved usb mode
uint8_t USBD_GetMode(usbd_cdc_msc_hid_state_t *usbd) {
    return usbd->usbd_mode;
}

int USBD_SelectMode(usbd_cdc_msc_hid_state_t *usbd, uint32_t mode, USBD_HID_ModeInfoTypeDef *hid_info) {
    // save mode
    usbd->usbd_mode = mode;

    // construct config desc
    switch (usbd->usbd_mode) {
        case USBD_MODE_CDC_MSC:
            usbd->usbd_config_desc_size = sizeof(cdc_msc_template_config_desc);
            memcpy(usbd->usbd_config_desc, cdc_msc_template_config_desc, sizeof(cdc_msc_template_config_desc));
            usbd->cdc_iface_num = CDC_IFACE_NUM_WITH_MSC;
            break;

        case USBD_MODE_CDC_HID:
            usbd->usbd_config_desc_size = sizeof(cdc_hid_template_config_desc);
            memcpy(usbd->usbd_config_desc, cdc_hid_template_config_desc, sizeof(cdc_hid_template_config_desc));
            usbd->cdc_iface_num = CDC_IFACE_NUM_WITH_HID;
            usbd->hid_in_ep = HID_IN_EP_WITH_CDC;
            usbd->hid_out_ep = HID_OUT_EP_WITH_CDC;
            usbd->hid_iface_num = HID_IFACE_NUM_WITH_CDC;
            usbd->hid_desc = usbd->usbd_config_desc + CDC_HID_TEMPLATE_HID_DESC_OFFSET;
            break;

        case USBD_MODE_CDC:
            usbd->usbd_config_desc_size = sizeof(cdc_template_config_desc);
            memcpy(usbd->usbd_config_desc, cdc_template_config_desc, sizeof(cdc_template_config_desc));
            usbd->cdc_iface_num = CDC_IFACE_NUM_ALONE;
            break;

            /*
            // not implemented
        case USBD_MODE_MSC_HID:
            hid_in_ep = HID_IN_EP_WITH_MSC;
            hid_out_ep = HID_OUT_EP_WITH_MSC;
            hid_iface_num = HID_IFACE_NUM_WITH_MSC;
            break;
            */

        default:
            // mode not supported
            return -1;
    }

    // configure the HID descriptor, if needed
    if (usbd->usbd_mode & USBD_MODE_HID) {
        uint8_t *hid_desc = usbd->hid_desc;
        hid_desc[HID_DESC_OFFSET_SUBCLASS] = hid_info->subclass;
        hid_desc[HID_DESC_OFFSET_PROTOCOL] = hid_info->protocol;
        hid_desc[HID_DESC_OFFSET_REPORT_DESC_LEN] = hid_info->report_desc_len;
        hid_desc[HID_DESC_OFFSET_MAX_PACKET_LO] = hid_info->max_packet_len;
        hid_desc[HID_DESC_OFFSET_MAX_PACKET_HI] = 0;
        hid_desc[HID_DESC_OFFSET_POLLING_INTERVAL] = hid_info->polling_interval;
        hid_desc[HID_DESC_OFFSET_MAX_PACKET_OUT_LO] = hid_info->max_packet_len;
        hid_desc[HID_DESC_OFFSET_MAX_PACKET_OUT_HI] = 0;
        hid_desc[HID_DESC_OFFSET_POLLING_INTERVAL_OUT] = hid_info->polling_interval;
        usbd->hid_report_desc = hid_info->report_desc;
    }

    return 0;
}

static uint8_t USBD_CDC_MSC_HID_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx) {
    if (pdev->dev_speed == USBD_SPEED_HIGH) {
        // can't handle high speed
        return 1;
    }

    usbd_cdc_msc_hid_state_t *usbd = pdev->pClassData;

    if (usbd->usbd_mode & USBD_MODE_CDC) {
        // CDC VCP component

        // Open EP IN
        USBD_LL_OpenEP(pdev,
                       CDC_IN_EP,
                       USBD_EP_TYPE_BULK,
                       CDC_DATA_IN_PACKET_SIZE);

        // Open EP OUT
        USBD_LL_OpenEP(pdev,
                       CDC_OUT_EP,
                       USBD_EP_TYPE_BULK,
                       CDC_DATA_OUT_PACKET_SIZE);

        // Open Command IN EP
        USBD_LL_OpenEP(pdev,
                       CDC_CMD_EP,
                       USBD_EP_TYPE_INTR,
                       CDC_CMD_PACKET_SIZE);

        // Init physical Interface components
        uint8_t *buf = usbd_cdc_init(usbd->cdc, usbd);

        // Init Xfer states
        usbd->CDC_ClassData.TxState = 0;
        usbd->CDC_ClassData.RxState = 0;

        // Prepare Out endpoint to receive next packet
        USBD_LL_PrepareReceive(pdev, CDC_OUT_EP, buf, CDC_DATA_OUT_PACKET_SIZE);
    }

    if (usbd->usbd_mode & USBD_MODE_MSC) {
        // MSC component

        // Open EP OUT
        USBD_LL_OpenEP(pdev,
                       MSC_OUT_EP,
                       USBD_EP_TYPE_BULK,
                       MSC_MAX_PACKET);

        // Open EP IN
        USBD_LL_OpenEP(pdev,
                       MSC_IN_EP,
                       USBD_EP_TYPE_BULK,
                       MSC_MAX_PACKET);

        // Init the BOT layer
        MSC_BOT_Init(pdev);
    }

    if (usbd->usbd_mode & USBD_MODE_HID) {
        // HID component

        // get max packet lengths from descriptor
        uint16_t mps_in =
            usbd->hid_desc[HID_DESC_OFFSET_MAX_PACKET_LO]
            | (usbd->hid_desc[HID_DESC_OFFSET_MAX_PACKET_HI] << 8);
        uint16_t mps_out =
            usbd->hid_desc[HID_DESC_OFFSET_MAX_PACKET_OUT_LO]
            | (usbd->hid_desc[HID_DESC_OFFSET_MAX_PACKET_OUT_HI] << 8);

        // Open EP IN
        USBD_LL_OpenEP(pdev,
                       usbd->hid_in_ep,
                       USBD_EP_TYPE_INTR,
                       mps_in);

        // Open EP OUT
        USBD_LL_OpenEP(pdev,
                       usbd->hid_out_ep,
                       USBD_EP_TYPE_INTR,
                       mps_out);

        uint8_t *buf = usbd_hid_init(usbd->hid, usbd);

        // Prepare Out endpoint to receive next packet
        USBD_LL_PrepareReceive(pdev, usbd->hid_out_ep, buf, mps_out);

        usbd->HID_ClassData.state = HID_IDLE;
    }

    return 0;
}

static uint8_t USBD_CDC_MSC_HID_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx) {
    usbd_cdc_msc_hid_state_t *usbd = pdev->pClassData;

    if ((usbd->usbd_mode & USBD_MODE_CDC) && usbd->cdc) {
        // CDC VCP component

        // close endpoints
        USBD_LL_CloseEP(pdev, CDC_IN_EP);
        USBD_LL_CloseEP(pdev, CDC_OUT_EP);
        USBD_LL_CloseEP(pdev, CDC_CMD_EP);
    }

    if (usbd->usbd_mode & USBD_MODE_MSC) {
        // MSC component

        // close endpoints
        USBD_LL_CloseEP(pdev, MSC_OUT_EP);
        USBD_LL_CloseEP(pdev, MSC_IN_EP);

        // DeInit the BOT layer
        MSC_BOT_DeInit(pdev);
    }

    if (usbd->usbd_mode & USBD_MODE_HID) {
        // HID component

        // close endpoints
        USBD_LL_CloseEP(pdev, usbd->hid_in_ep);
        USBD_LL_CloseEP(pdev, usbd->hid_out_ep);
    }

    return 0;
}

static uint8_t USBD_CDC_MSC_HID_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req) {

    /*
    printf("SU: %x %x %x %x\n", req->bmRequest, req->bRequest, req->wValue, req->wIndex);

    This is what we get when MSC is IFACE=0 and CDC is IFACE=1,2:
        SU: 21 22 0 1 -- USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE; CDC_SET_CONTROL_LINE_STATE
        SU: 21 20 0 1 -- USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE; CDC_SET_LINE_CODING
        SU: a1 fe 0 0 -- 0x80 | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE; BOT_GET_MAX_LUN; 0; 0
        SU: 21 22 3 1 -- USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE; CDC_SET_CONTROL_LINE_STATE

    On a Mac OS X, with MSC then CDC:
        SU: a1 fe 0 0
        SU: 21 22 2 1
        SU: 21 22 3 1
        SU: 21 20 0 1
    */

    usbd_cdc_msc_hid_state_t *usbd = pdev->pClassData;

    switch (req->bmRequest & USB_REQ_TYPE_MASK) {

        // Class request
        case USB_REQ_TYPE_CLASS:
            // req->wIndex is the recipient interface number
            if ((usbd->usbd_mode & USBD_MODE_CDC) && req->wIndex == usbd->cdc_iface_num) {
                // CDC component
                if (req->wLength) {
                    if (req->bmRequest & 0x80) {
                        // device-to-host request
                        usbd_cdc_control(usbd->cdc, req->bRequest, (uint8_t*)usbd->CDC_ClassData.data, req->wLength);
                        USBD_CtlSendData(pdev, (uint8_t*)usbd->CDC_ClassData.data, req->wLength);
                    } else {
                        // host-to-device request
                        usbd->CDC_ClassData.CmdOpCode = req->bRequest;
                        usbd->CDC_ClassData.CmdLength = req->wLength;
                        USBD_CtlPrepareRx(pdev, (uint8_t*)usbd->CDC_ClassData.data, req->wLength);
                    }
                } else {
                    // Not a Data request
                    // Transfer the command to the interface layer
                    return usbd_cdc_control(usbd->cdc, req->bRequest, NULL, req->wValue);
                }
            } else if ((usbd->usbd_mode & USBD_MODE_MSC) && req->wIndex == MSC_IFACE_NUM_WITH_CDC) {
                // MSC component
                switch (req->bRequest) {
                    case BOT_GET_MAX_LUN:
                        if ((req->wValue  == 0) && (req->wLength == 1) && ((req->bmRequest & 0x80) == 0x80)) {
                            usbd->MSC_BOT_ClassData.max_lun = usbd->MSC_BOT_ClassData.bdev_ops->GetMaxLun();
                            USBD_CtlSendData(pdev, (uint8_t *)&usbd->MSC_BOT_ClassData.max_lun, 1);
                        } else {
                            USBD_CtlError(pdev, req);
                            return USBD_FAIL;
                        }
                        break;

                    case BOT_RESET:
                        if ((req->wValue == 0) && (req->wLength == 0) && ((req->bmRequest & 0x80) != 0x80)) {
                            MSC_BOT_Reset(pdev);
                        } else {
                            USBD_CtlError(pdev, req);
                            return USBD_FAIL;
                        }
                        break;

                    default:
                        USBD_CtlError(pdev, req);
                        return USBD_FAIL;
                }
            } else if ((usbd->usbd_mode & USBD_MODE_HID) && req->wIndex == usbd->hid_iface_num) {
                switch (req->bRequest) {
                    case HID_REQ_SET_PROTOCOL:
                        usbd->HID_ClassData.Protocol = (uint8_t)(req->wValue);
                        break;

                    case HID_REQ_GET_PROTOCOL:
                        USBD_CtlSendData(pdev, (uint8_t *)&usbd->HID_ClassData.Protocol, 1);
                        break;

                    case HID_REQ_SET_IDLE:
                        usbd->HID_ClassData.IdleState = (uint8_t)(req->wValue >> 8);
                        break;

                    case HID_REQ_GET_IDLE:
                        USBD_CtlSendData(pdev, (uint8_t *)&usbd->HID_ClassData.IdleState, 1);
                        break;

                    default:
                        USBD_CtlError(pdev, req);
                        return USBD_FAIL;
                }
            }
            break;

        // Interface & Endpoint request
        case USB_REQ_TYPE_STANDARD:
            if ((usbd->usbd_mode & USBD_MODE_MSC) && req->wIndex == MSC_IFACE_NUM_WITH_CDC) {
                switch (req->bRequest) {
                    case USB_REQ_GET_INTERFACE :
                        USBD_CtlSendData(pdev, (uint8_t *)&usbd->MSC_BOT_ClassData.interface, 1);
                        break;

                    case USB_REQ_SET_INTERFACE :
                        usbd->MSC_BOT_ClassData.interface = (uint8_t)(req->wValue);
                        break;

                    case USB_REQ_CLEAR_FEATURE:
                        // Flush the FIFO and Clear the stall status
                        USBD_LL_FlushEP(pdev, (uint8_t)req->wIndex);

                        // Re-activate the EP
                        USBD_LL_CloseEP(pdev, (uint8_t)req->wIndex);
                        if((((uint8_t)req->wIndex) & 0x80) == 0x80) {
                            // Open EP IN
                            USBD_LL_OpenEP(pdev, MSC_IN_EP, USBD_EP_TYPE_BULK, MSC_MAX_PACKET);
                        } else {
                            // Open EP OUT
                            USBD_LL_OpenEP(pdev, MSC_OUT_EP, USBD_EP_TYPE_BULK, MSC_MAX_PACKET);
                        }
                        // Handle BOT error
                        MSC_BOT_CplClrFeature(pdev, (uint8_t)req->wIndex);
                        break;
                }
            } else if ((usbd->usbd_mode & USBD_MODE_HID) && req->wIndex == usbd->hid_iface_num) {
                switch (req->bRequest) {
                    case USB_REQ_GET_DESCRIPTOR: {
                        uint16_t len = 0;
                        const uint8_t *pbuf = NULL;
                        if (req->wValue >> 8 == HID_REPORT_DESC) {
                            len = usbd->hid_desc[HID_DESC_OFFSET_REPORT_DESC_LEN];
                            len = MIN(len, req->wLength);
                            pbuf = usbd->hid_report_desc;
                        } else if (req->wValue >> 8 == HID_DESCRIPTOR_TYPE) {
                            len = MIN(HID_SUBDESC_LEN, req->wLength);
                            pbuf = usbd->hid_desc + HID_DESC_OFFSET_SUBDESC;
                        }
                        USBD_CtlSendData(pdev, (uint8_t*)pbuf, len);
                        break;
                    }

                    case USB_REQ_GET_INTERFACE:
                        USBD_CtlSendData(pdev, (uint8_t *)&usbd->HID_ClassData.AltSetting, 1);
                        break;

                    case USB_REQ_SET_INTERFACE:
                        usbd->HID_ClassData.AltSetting = (uint8_t)(req->wValue);
                        break;
                }
            }
            break;
    }
    return USBD_OK;
}

/* unused
static uint8_t EP0_TxSent(USBD_HandleTypeDef *pdev) {
}
*/

static uint8_t USBD_CDC_MSC_HID_EP0_RxReady(USBD_HandleTypeDef *pdev) {
    usbd_cdc_msc_hid_state_t *usbd = pdev->pClassData;
    if (usbd->cdc != NULL && usbd->CDC_ClassData.CmdOpCode != 0xff) {
        usbd_cdc_control(usbd->cdc, usbd->CDC_ClassData.CmdOpCode, (uint8_t*)usbd->CDC_ClassData.data, usbd->CDC_ClassData.CmdLength);
        usbd->CDC_ClassData.CmdOpCode = 0xff;
    }

    return USBD_OK;
}

static uint8_t USBD_CDC_MSC_HID_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum) {
    usbd_cdc_msc_hid_state_t *usbd = pdev->pClassData;
    if ((usbd->usbd_mode & USBD_MODE_CDC) && (epnum == (CDC_IN_EP & 0x7f) || epnum == (CDC_CMD_EP & 0x7f))) {
        usbd->CDC_ClassData.TxState = 0;
        return USBD_OK;
    } else if ((usbd->usbd_mode & USBD_MODE_MSC) && epnum == (MSC_IN_EP & 0x7f)) {
        MSC_BOT_DataIn(pdev, epnum);
        return USBD_OK;
    } else if ((usbd->usbd_mode & USBD_MODE_HID) && epnum == (usbd->hid_in_ep & 0x7f)) {
        /* Ensure that the FIFO is empty before a new transfer, this condition could
        be caused by  a new transfer before the end of the previous transfer */
        usbd->HID_ClassData.state = HID_IDLE;
        return USBD_OK;
    }

    return USBD_OK;
}

static uint8_t USBD_CDC_MSC_HID_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum) {
    usbd_cdc_msc_hid_state_t *usbd = pdev->pClassData;
    if ((usbd->usbd_mode & USBD_MODE_CDC) && epnum == (CDC_OUT_EP & 0x7f)) {
        /* Get the received data length */
        size_t len = USBD_LL_GetRxDataSize (pdev, epnum);

        /* USB data will be immediately processed, this allow next USB traffic being
        NAKed till the end of the application Xfer */
        usbd_cdc_receive(usbd->cdc, len);

        return USBD_OK;
    } else if ((usbd->usbd_mode & USBD_MODE_MSC) && epnum == (MSC_OUT_EP & 0x7f)) {
        MSC_BOT_DataOut(pdev, epnum);
        return USBD_OK;
    } else if ((usbd->usbd_mode & USBD_MODE_HID) && epnum == (usbd->hid_out_ep & 0x7f)) {
        size_t len = USBD_LL_GetRxDataSize(pdev, epnum);
        usbd_hid_receive(usbd->hid, len);
    }

    return USBD_OK;
}

static uint8_t *USBD_CDC_MSC_HID_GetCfgDesc(USBD_HandleTypeDef *pdev, uint16_t *length) {
    usbd_cdc_msc_hid_state_t *usbd = pdev->pClassData;
    *length = usbd->usbd_config_desc_size;
    return usbd->usbd_config_desc;
}

// this is used only in high-speed mode, which we don't support
uint8_t *USBD_CDC_MSC_HID_GetDeviceQualifierDescriptor(USBD_HandleTypeDef *pdev, uint16_t *length) {
    /*
    *length = sizeof(USBD_CDC_MSC_HID_DeviceQualifierDesc);
    return USBD_CDC_MSC_HID_DeviceQualifierDesc;
    */
    *length = 0;
    return NULL;
}

// data received on non-control OUT endpoint
uint8_t USBD_CDC_TransmitPacket(usbd_cdc_msc_hid_state_t *usbd, size_t len, const uint8_t *buf) {
    if (usbd->CDC_ClassData.TxState == 0) {
        // transmit next packet
        USBD_LL_Transmit(usbd->pdev, CDC_IN_EP, (uint8_t*)buf, len);

        // Tx transfer in progress
        usbd->CDC_ClassData.TxState = 1;
        return USBD_OK;
    } else {
        return USBD_BUSY;
    }
}

// prepare OUT endpoint for reception
uint8_t USBD_CDC_ReceivePacket(usbd_cdc_msc_hid_state_t *usbd, uint8_t *buf) {
    // Suspend or Resume USB Out process
    if (usbd->pdev->dev_speed == USBD_SPEED_HIGH) {
        return USBD_FAIL;
    }

    // Prepare Out endpoint to receive next packet
    USBD_LL_PrepareReceive(usbd->pdev, CDC_OUT_EP, buf, CDC_DATA_OUT_PACKET_SIZE);

    return USBD_OK;
}

// prepare OUT endpoint for reception
uint8_t USBD_HID_ReceivePacket(usbd_cdc_msc_hid_state_t *usbd, uint8_t *buf) {
    // Suspend or Resume USB Out process
    if (usbd->pdev->dev_speed == USBD_SPEED_HIGH) {
        return USBD_FAIL;
    }

    // Prepare Out endpoint to receive next packet
    uint16_t mps_out =
        usbd->hid_desc[HID_DESC_OFFSET_MAX_PACKET_OUT_LO]
        | (usbd->hid_desc[HID_DESC_OFFSET_MAX_PACKET_OUT_HI] << 8);
    USBD_LL_PrepareReceive(usbd->pdev, usbd->hid_out_ep, buf, mps_out);

    return USBD_OK;
}

int USBD_HID_CanSendReport(usbd_cdc_msc_hid_state_t *usbd) {
    return usbd->pdev->dev_state == USBD_STATE_CONFIGURED && usbd->HID_ClassData.state == HID_IDLE;
}

uint8_t USBD_HID_SendReport(usbd_cdc_msc_hid_state_t *usbd, uint8_t *report, uint16_t len) {
    if (usbd->pdev->dev_state == USBD_STATE_CONFIGURED) {
        if (usbd->HID_ClassData.state == HID_IDLE) {
            usbd->HID_ClassData.state = HID_BUSY;
            USBD_LL_Transmit(usbd->pdev, usbd->hid_in_ep, report, len);
        }
    }
    return USBD_OK;
}

uint8_t USBD_HID_SetNAK(usbd_cdc_msc_hid_state_t *usbd) {
    // get USBx object from pdev (needed for USBx_OUTEP macro below)
    PCD_HandleTypeDef *hpcd = usbd->pdev->pData;
    USB_OTG_GlobalTypeDef *USBx = hpcd->Instance;
    // set NAK on HID OUT endpoint
    USBx_OUTEP(HID_OUT_EP_WITH_CDC)->DOEPCTL |= USB_OTG_DOEPCTL_SNAK;
    return USBD_OK;
}

uint8_t USBD_HID_ClearNAK(usbd_cdc_msc_hid_state_t *usbd) {
    // get USBx object from pdev (needed for USBx_OUTEP macro below)
    PCD_HandleTypeDef *hpcd = usbd->pdev->pData;
    USB_OTG_GlobalTypeDef *USBx = hpcd->Instance;
    // clear NAK on HID OUT endpoint
    USBx_OUTEP(HID_OUT_EP_WITH_CDC)->DOEPCTL |= USB_OTG_DOEPCTL_CNAK;
    return USBD_OK;
}

// CDC/MSC/HID interface class callback structure
const USBD_ClassTypeDef USBD_CDC_MSC_HID = {
    USBD_CDC_MSC_HID_Init,
    USBD_CDC_MSC_HID_DeInit,
    USBD_CDC_MSC_HID_Setup,
    NULL, // EP0_TxSent
    USBD_CDC_MSC_HID_EP0_RxReady,
    USBD_CDC_MSC_HID_DataIn,
    USBD_CDC_MSC_HID_DataOut,
    NULL, // SOF
    NULL, // IsoINIncomplete
    NULL, // IsoOUTIncomplete
    USBD_CDC_MSC_HID_GetCfgDesc,
    USBD_CDC_MSC_HID_GetCfgDesc,
    USBD_CDC_MSC_HID_GetCfgDesc,
    USBD_CDC_MSC_HID_GetDeviceQualifierDescriptor,
};
