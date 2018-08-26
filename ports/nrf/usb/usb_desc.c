/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 hathach for Adafruit Industries
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

#include "usb_desc.h"
#include "common-hal/usb_hid/Device.h"

//--------------------------------------------------------------------+
// MACRO TYPEDEF CONSTANT ENUM DECLARATION
//--------------------------------------------------------------------+
#define USB_VID     0x239A

/* Note: different class combination e.g CDC and (CDC + MSC) should have different
 * PID since Host OS will "remembered" device driver after the first plug */
#define USB_PID     0x802A

/*------------- Interface Numbering -------------*/
enum {
    ITF_NUM_CDC = 0  ,
    ITF_NUM_CDC_DATA ,
    ITF_NUM_MSC      ,
    ITF_NUM_HID_GEN  ,
    ITF_NUM_TOTAL
};

enum {
    ITF_STR_LANGUAGE = 0 ,
    ITF_STR_MANUFACTURER ,
    ITF_STR_PRODUCT      ,
    ITF_STR_SERIAL       ,
    ITF_STR_CDC          ,
    ITF_STR_MSC          ,
    ITF_STR_HID
};

/*------------- Endpoint Numbering & Size -------------*/
#define _EP_IN(x)               (0x80 | (x))
#define _EP_OUT(x)              (x)

// CDC
#define EP_CDC_NOTIF            _EP_IN ( ITF_NUM_CDC+1 )
#define EP_CDC_NOTIF_SIZE       8

#define EP_CDC_OUT              _EP_OUT( ITF_NUM_CDC+2 )
#define EP_CDC_IN               _EP_IN ( ITF_NUM_CDC+2 )

// Mass Storage
#define EP_MSC_OUT              _EP_OUT( ITF_NUM_MSC+1 )
#define EP_MSC_IN               _EP_IN ( ITF_NUM_MSC+1 )

// HID composite = keyboard + mouse + gamepad + etc ...
#define EP_HID_GEN              _EP_IN ( ITF_NUM_HID_GEN+1 )
#define EP_HID_GEN_SIZE         16

//--------------------------------------------------------------------+
// STRING DESCRIPTORS
//--------------------------------------------------------------------+

uint16_t usb_desc_str_serial[1+16] = { TUD_DESC_STR_HEADER(16) };

// array of pointer to string descriptors
uint16_t const * const string_desc_arr [] =
{
    // 0 index is supported language = English
    TUD_DESC_STRCONV(0x0409),

    // 1 Manufacturer
    TUD_DESC_STRCONV('A','d','a','f','r','u','i','t',' ','I','n','d','u','s','t','r','i','e','s'),

    // 2 Product
    TUD_DESC_STRCONV('C','i','r','c','u','i','t','P','y',' ','n','R','F','5','2'),

    // 3 Serials TODO use chip ID
    usb_desc_str_serial,

    // 4 CDC Interface
    TUD_DESC_STRCONV('C','i','r','c','u','i','t','P','y',' ','S','e','r','i','a','l'),

    // 5 MSC Interface
    TUD_DESC_STRCONV('C','i','r','c','u','i','t','P','y',' ','S','t','o','r','a','g','e'),

    // 6 HID Interface
    TUD_DESC_STRCONV('C','i','r','c','u','i','t','P','y',' ','H','I','D'),

    // Custom Interface
//    TUD_DESC_STRCONV('C','i','r','c','u','i','t','P','y',' ','C','u','s','t','o','m')
};


//--------------------------------------------------------------------+
// Device Descriptor
//--------------------------------------------------------------------+
tusb_desc_device_t const usb_desc_dev =
{
    .bLength            = sizeof(tusb_desc_device_t),
    .bDescriptorType    = TUSB_DESC_DEVICE,
    .bcdUSB             = 0x0200,

  #if CFG_TUD_CDC
    // Use Interface Association Descriptor (IAD) for CDC
    // As required by USB Specs IAD's subclass must be common class (2) and protocol must be IAD (1)
    .bDeviceClass       = TUSB_CLASS_MISC,
    .bDeviceSubClass    = MISC_SUBCLASS_COMMON,
    .bDeviceProtocol    = MISC_PROTOCOL_IAD,
  #else
    .bDeviceClass       = 0x00,
    .bDeviceSubClass    = 0x00,
    .bDeviceProtocol    = 0x00,
  #endif

    .bMaxPacketSize0    = CFG_TUD_ENDOINT0_SIZE,

    .idVendor           = USB_VID,
    .idProduct          = USB_PID,
    .bcdDevice          = 0x0100,

    .iManufacturer      = 0x01,
    .iProduct           = 0x02,
    .iSerialNumber      = 0x03,

    .bNumConfigurations = 0x01 // TODO multiple configurations
};

//--------------------------------------------------------------------+
// HID Report Descriptor
//--------------------------------------------------------------------+
uint8_t const usb_desc_hid_generic_report[] =
{
#if USB_HID_DEVICE_KEYBOARD
    HID_REPORT_DESC_KEYBOARD( HID_REPORT_ID(USB_HID_REPORT_ID_KEYBOARD), ),
#endif

#if USB_HID_DEVICE_MOUSE
    HID_REPORT_DESC_MOUSE( HID_REPORT_ID(USB_HID_REPORT_ID_MOUSE), ),
#endif

#if USB_HID_DEVICE_CONSUMER
    HID_REPORT_DESC_CONSUMER( HID_REPORT_ID(USB_HID_REPORT_ID_CONSUMER), ),
#endif

#if USB_HID_DEVICE_SYS_CONTROL
    HID_REPORT_DESC_SYSTEM_CONTROL( HID_REPORT_ID(USB_HID_REPORT_ID_SYS_CONTROL ), ),
#endif

#if USB_HID_DEVICE_GAMEPAD
    HID_REPORT_DESC_GAMEPAD( HID_REPORT_ID(USB_HID_REPORT_ID_GAMEPAD ), )
#endif

};

//--------------------------------------------------------------------+
// Configuration Descriptor
//--------------------------------------------------------------------+
usb_desc_cfg_t const usb_desc_cfg =
{
    .config =
    {
        .bLength             = sizeof(tusb_desc_configuration_t),
        .bDescriptorType     = TUSB_DESC_CONFIGURATION,
        .wTotalLength        = sizeof(usb_desc_cfg_t),
        .bNumInterfaces      = ITF_NUM_TOTAL,
        .bConfigurationValue = 1,
        .iConfiguration      = 0x00,
        .bmAttributes        = TUSB_DESC_CONFIG_ATT_BUS_POWER,
        .bMaxPower           = TUSB_DESC_CONFIG_POWER_MA(100)
    },

    // IAD points to CDC Interfaces
    .cdc =
    {
      .iad =
      {
          .bLength           = sizeof(tusb_desc_interface_assoc_t),
          .bDescriptorType   = TUSB_DESC_INTERFACE_ASSOCIATION,

          .bFirstInterface   = ITF_NUM_CDC,
          .bInterfaceCount   = 2,

          .bFunctionClass    = TUSB_CLASS_CDC,
          .bFunctionSubClass = CDC_COMM_SUBCLASS_ABSTRACT_CONTROL_MODEL,
          .bFunctionProtocol = CDC_COMM_PROTOCOL_ATCOMMAND,
          .iFunction         = 0
      },

      //------------- CDC Communication Interface -------------//
      .comm_itf =
      {
          .bLength            = sizeof(tusb_desc_interface_t),
          .bDescriptorType    = TUSB_DESC_INTERFACE,
          .bInterfaceNumber   = ITF_NUM_CDC,
          .bAlternateSetting  = 0,
          .bNumEndpoints      = 1,
          .bInterfaceClass    = TUSB_CLASS_CDC,
          .bInterfaceSubClass = CDC_COMM_SUBCLASS_ABSTRACT_CONTROL_MODEL,
          .bInterfaceProtocol = CDC_COMM_PROTOCOL_ATCOMMAND,
          .iInterface         = ITF_STR_CDC
      },

      .header =
      {
          .bLength            = sizeof(cdc_desc_func_header_t),
          .bDescriptorType    = TUSB_DESC_CLASS_SPECIFIC,
          .bDescriptorSubType = CDC_FUNC_DESC_HEADER,
          .bcdCDC             = 0x0120
      },

      .call =
      {
          .bLength            = sizeof(cdc_desc_func_call_management_t),
          .bDescriptorType    = TUSB_DESC_CLASS_SPECIFIC,
          .bDescriptorSubType = CDC_FUNC_DESC_CALL_MANAGEMENT,
          .bmCapabilities     = { 0 },
          .bDataInterface     = ITF_NUM_CDC+1,
      },

      .acm =
      {
          .bLength            = sizeof(cdc_desc_func_acm_t),
          .bDescriptorType    = TUSB_DESC_CLASS_SPECIFIC,
          .bDescriptorSubType = CDC_FUNC_DESC_ABSTRACT_CONTROL_MANAGEMENT,
          .bmCapabilities     = { // 0x02
              .support_line_request = 1,
          }
      },

      .union_func =
      {
          .bLength                  = sizeof(cdc_desc_func_union_t), // plus number of
          .bDescriptorType          = TUSB_DESC_CLASS_SPECIFIC,
          .bDescriptorSubType       = CDC_FUNC_DESC_UNION,
          .bControlInterface        = ITF_NUM_CDC,
          .bSubordinateInterface    = ITF_NUM_CDC+1,
      },

      .ep_notif =
      {
          .bLength          = sizeof(tusb_desc_endpoint_t),
          .bDescriptorType  = TUSB_DESC_ENDPOINT,
          .bEndpointAddress = EP_CDC_NOTIF,
          .bmAttributes     = { .xfer = TUSB_XFER_INTERRUPT },
          .wMaxPacketSize   = { .size = EP_CDC_NOTIF_SIZE },
          .bInterval        = 0x10
      },

      //------------- CDC Data Interface -------------//
      .data_itf =
      {
          .bLength            = sizeof(tusb_desc_interface_t),
          .bDescriptorType    = TUSB_DESC_INTERFACE,
          .bInterfaceNumber   = ITF_NUM_CDC+1,
          .bAlternateSetting  = 0x00,
          .bNumEndpoints      = 2,
          .bInterfaceClass    = TUSB_CLASS_CDC_DATA,
          .bInterfaceSubClass = 0,
          .bInterfaceProtocol = 0,
          .iInterface         = 0x00
      },

      .ep_out =
      {
          .bLength          = sizeof(tusb_desc_endpoint_t),
          .bDescriptorType  = TUSB_DESC_ENDPOINT,
          .bEndpointAddress = EP_CDC_OUT,
          .bmAttributes     = { .xfer = TUSB_XFER_BULK },
          .wMaxPacketSize   = { .size = CFG_TUD_CDC_EPSIZE },
          .bInterval        = 0
      },

      .ep_in =
      {
          .bLength          = sizeof(tusb_desc_endpoint_t),
          .bDescriptorType  = TUSB_DESC_ENDPOINT,
          .bEndpointAddress = EP_CDC_IN,
          .bmAttributes     = { .xfer = TUSB_XFER_BULK },
          .wMaxPacketSize   = { .size = CFG_TUD_CDC_EPSIZE },
          .bInterval        = 0
      },
    },

    //------------- Mass Storage-------------//
    .msc =
    {
      .itf =
      {
          .bLength            = sizeof(tusb_desc_interface_t),
          .bDescriptorType    = TUSB_DESC_INTERFACE,
          .bInterfaceNumber   = ITF_NUM_MSC,
          .bAlternateSetting  = 0x00,
          .bNumEndpoints      = 2,
          .bInterfaceClass    = TUSB_CLASS_MSC,
          .bInterfaceSubClass = MSC_SUBCLASS_SCSI,
          .bInterfaceProtocol = MSC_PROTOCOL_BOT,
          .iInterface         = ITF_STR_MSC
      },

      .ep_out =
      {
          .bLength          = sizeof(tusb_desc_endpoint_t),
          .bDescriptorType  = TUSB_DESC_ENDPOINT,
          .bEndpointAddress = EP_MSC_OUT,
          .bmAttributes     = { .xfer = TUSB_XFER_BULK },
          .wMaxPacketSize   = { .size = CFG_TUD_MSC_EPSIZE},
          .bInterval        = 1
      },

      .ep_in =
      {
          .bLength          = sizeof(tusb_desc_endpoint_t),
          .bDescriptorType  = TUSB_DESC_ENDPOINT,
          .bEndpointAddress = EP_MSC_IN,
          .bmAttributes     = { .xfer = TUSB_XFER_BULK },
          .wMaxPacketSize   = { .size = CFG_TUD_MSC_EPSIZE},
          .bInterval        = 1
      }
    },

    //------------- HID Generic Multiple report -------------//
    .hid_generic =
    {
        .itf =
        {
            .bLength            = sizeof(tusb_desc_interface_t),
            .bDescriptorType    = TUSB_DESC_INTERFACE,
            .bInterfaceNumber   = ITF_NUM_HID_GEN,
            .bAlternateSetting  = 0x00,
            .bNumEndpoints      = 1,
            .bInterfaceClass    = TUSB_CLASS_HID,
            .bInterfaceSubClass = 0,
            .bInterfaceProtocol = 0,
            .iInterface         = ITF_STR_HID
        },

        .hid_desc =
        {
            .bLength           = sizeof(tusb_hid_descriptor_hid_t),
            .bDescriptorType   = HID_DESC_TYPE_HID,
            .bcdHID            = 0x0111,
            .bCountryCode      = HID_Local_NotSupported,
            .bNumDescriptors   = 1,
            .bReportType       = HID_DESC_TYPE_REPORT,
            .wReportLength     = sizeof(usb_desc_hid_generic_report)
        },

        .ep_in =
        {
            .bLength          = sizeof(tusb_desc_endpoint_t),
            .bDescriptorType  = TUSB_DESC_ENDPOINT,
            .bEndpointAddress = EP_HID_GEN,
            .bmAttributes     = { .xfer = TUSB_XFER_INTERRUPT },
            .wMaxPacketSize   = { .size = EP_HID_GEN_SIZE },
            .bInterval        = 0x0A
        }
    }
};


// tud_desc_set is required by tinyusb stack
tud_desc_set_t tud_desc_set =
{
    .device       = &usb_desc_dev,
    .config       = &usb_desc_cfg,
    .string_arr   = (uint8_t const **) string_desc_arr,
    .string_count = sizeof(string_desc_arr)/sizeof(string_desc_arr[0]),

    .hid_report =
    {
        .generic       = usb_desc_hid_generic_report,
        .boot_keyboard = NULL,
        .boot_mouse    = NULL
    }
};
