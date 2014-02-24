/**
  ******************************************************************************
  * @file    usbd_cdc_core.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-March-2012
  * @brief   This file provides the high layer firmware functions to manage the 
  *          following functionalities of the USB CDC Class:
  *           - Initialization and Configuration of high and low layer
  *           - Enumeration as CDC Device (and enumeration for each implemented memory interface)
  *           - OUT/IN data transfer
  *           - Command IN transfer (class requests management)
  *           - Error management
  *           
  *  @verbatim
  *      
  *          ===================================================================      
  *                                CDC Class Driver Description
  *          =================================================================== 
  *           This driver manages the "Universal Serial Bus Class Definitions for Communications Devices
  *           Revision 1.2 November 16, 2007" and the sub-protocol specification of "Universal Serial Bus 
  *           Communications Class Subclass Specification for PSTN Devices Revision 1.2 February 9, 2007"
  *           This driver implements the following aspects of the specification:
  *             - Device descriptor management
  *             - Configuration descriptor management
  *             - Enumeration as CDC device with 2 data endpoints (IN and OUT) and 1 command endpoint (IN)
  *             - Requests management (as described in section 6.2 in specification)
  *             - Abstract Control Model compliant
  *             - Union Functional collection (using 1 IN endpoint for control)
  *             - Data interface class

  *           @note
  *             For the Abstract Control Model, this core allows only transmitting the requests to
  *             lower layer dispatcher (ie. usbd_cdc_vcp.c/.h) which should manage each request and
  *             perform relative actions.
  * 
  *           These aspects may be enriched or modified for a specific user application.
  *          
  *            This driver doesn't implement the following aspects of the specification 
  *            (but it is possible to manage these features with some modifications on this driver):
  *             - Any class-specific aspect relative to communication classes should be managed by user application.
  *             - All communication classes other than PSTN are not managed
  *      
  *  @endverbatim
  *                                  
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>

#include "usbd_cdc_core.h"
#include "usbd_desc.h"
#include "usbd_req.h"
#include "usbd_conf.h"
#include "usbd_msc_bot.h"
#include "usbd_msc_mem.h"

// CDC VCP is the first interface
// the option below is supposed to select between MSC (1) and HID (0) for the
// second USB interface, but it does not work (only VCP+MSC works)
// to use HID on its own (ie no VCP), the functions in usbd_pyb_core2.c can be
// used, and selected in the call to pyb_usb_dev_init().
#define USB_PYB_USE_MSC (1)

#if USB_PYB_USE_MSC
//#define USB_PYB_CONFIG_DESC_SIZ (67) // for only CDC VCP interfaces
#define USB_PYB_CONFIG_DESC_SIZ (98) // for both CDC VCP and MSC interfaces
#else // USE_HID
#define USB_PYB_CONFIG_DESC_SIZ (100) // for both CDC VCP and HID interfaces
#endif

#define MSC_EPIN_SIZE                MSC_MAX_PACKET
#define MSC_EPOUT_SIZE               MSC_MAX_PACKET

#define HID_MOUSE_REPORT_DESC_SIZE   (74)

#define HID_DESCRIPTOR_TYPE           0x21
#define HID_REPORT_DESC               0x22

// HID parameters
#define HID_IN_EP                   (0x83)
#define HID_IN_PACKET               (4) /* maximum, and actual, packet size */

/*********************************************
   PYB Device library callbacks
 *********************************************/
static uint8_t usbd_pyb_Init        (void *pdev, uint8_t cfgidx);
static uint8_t usbd_pyb_DeInit      (void *pdev, uint8_t cfgidx);
static uint8_t usbd_pyb_Setup       (void *pdev, USB_SETUP_REQ *req);
static uint8_t usbd_pyb_EP0_RxReady (void *pdev);
static uint8_t usbd_pyb_DataIn      (void *pdev, uint8_t epnum);
static uint8_t usbd_pyb_DataOut     (void *pdev, uint8_t epnum);
static uint8_t usbd_pyb_SOF         (void *pdev);

/*********************************************
   PYB specific management functions
 *********************************************/
static void    Handle_USBAsynchXfer (void *pdev);
static uint8_t *usbd_pyb_GetCfgDesc (uint8_t speed, uint16_t *length);

/** @defgroup usbd_cdc_Private_Variables
  * @{
  */ 
extern CDC_IF_Prop_TypeDef  VCP_fops;
extern uint8_t USBD_DeviceDesc   [USB_SIZ_DEVICE_DESC];

__ALIGN_BEGIN static uint8_t usbd_cdc_AltSet __ALIGN_END = 0;
__ALIGN_BEGIN static uint8_t USB_Rx_Buffer[CDC_DATA_MAX_PACKET_SIZE] __ALIGN_END;
__ALIGN_BEGIN uint8_t APP_Rx_Buffer[APP_RX_DATA_SIZE] __ALIGN_END;

__ALIGN_BEGIN static uint8_t CmdBuff[CDC_CMD_PACKET_SZE] __ALIGN_END;

#if USB_PYB_USE_MSC
__ALIGN_BEGIN static uint8_t USBD_MSC_MaxLun __ALIGN_END = 0;
__ALIGN_BEGIN static uint8_t USBD_MSC_AltSet __ALIGN_END = 0;
#else
__ALIGN_BEGIN static uint8_t USBD_HID_AltSet __ALIGN_END = 0;
__ALIGN_BEGIN static uint8_t USBD_HID_Protocol __ALIGN_END = 0;
__ALIGN_BEGIN static uint8_t USBD_HID_IdleState __ALIGN_END = 0;
#endif

uint32_t APP_Rx_ptr_in  = 0;
uint32_t APP_Rx_ptr_out = 0;
uint32_t APP_Rx_length  = 0;

uint8_t  USB_Tx_State = 0;

static uint32_t cdcCmd = 0xFF;
static uint32_t cdcLen = 0;

/* PYB interface class callbacks structure */
USBD_Class_cb_TypeDef USBD_PYB_cb =
{
    usbd_pyb_Init,
    usbd_pyb_DeInit,
    usbd_pyb_Setup,
    NULL, // EP0_TxSent
    usbd_pyb_EP0_RxReady,
    usbd_pyb_DataIn,
    usbd_pyb_DataOut,
    usbd_pyb_SOF,
    NULL, // IsoINIncomplete
    NULL, // IsoOUTIncomplete
    usbd_pyb_GetCfgDesc,
    // for OTG_HS support need to add other cfg desc here
};

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4   
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
/* USB PYB device Configuration Descriptor */
__ALIGN_BEGIN static uint8_t usbd_pyb_CfgDesc[USB_PYB_CONFIG_DESC_SIZ] __ALIGN_END =
{
    //--------------------------------------------------------------------------
    // Configuration Descriptor
    0x09,   // bLength: Configuration Descriptor size
    USB_CONFIGURATION_DESCRIPTOR_TYPE,      // bDescriptorType: Configuration
    LOBYTE(USB_PYB_CONFIG_DESC_SIZ),        // wTotalLength: no of returned bytes
    HIBYTE(USB_PYB_CONFIG_DESC_SIZ),
    0x03,   // bNumInterfaces: 3 interfaces
    0x01,   // bConfigurationValue: Configuration value
    0x04,   // iConfiguration: Index of string descriptor describing the configuration
    0x80,   // bmAttributes: bus powered; 0xc0 for self powered
    0xfa,   // bMaxPower: in units of 2mA

    //==========================================================================
    // Interface Association for CDC VCP
    0x08,   // bLength: 8 bytes
    USB_INTERFACE_ASSOCIATION_TYPE,     // bDescriptorType: IAD
    0x00,   // bFirstInterface: first interface for this association
    0x02,   // bInterfaceCount: nummber of interfaces for this association
    0x00,   // bFunctionClass: ?
    0x00,   // bFunctionSubClass: ?
    0x00,   // bFunctionProtocol: ?
    0x04,   // iFunction: index of string for this function

    //--------------------------------------------------------------------------
    // Interface Descriptor
    0x09,   // bLength: Interface Descriptor size
    USB_INTERFACE_DESCRIPTOR_TYPE,      // bDescriptorType: Interface
    0x00,   // bInterfaceNumber: Number of Interface
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
    0x01,   // bDataInterface: 1

    // ACM Functional Descriptor
    0x04,   // bFunctionLength
    0x24,   // bDescriptorType: CS_INTERFACE
    0x02,   // bDescriptorSubtype: Abstract Control Management desc
    0x02,   // bmCapabilities

    // Union Functional Descriptor
    0x05,   // bFunctionLength
    0x24,   // bDescriptorType: CS_INTERFACE
    0x06,   // bDescriptorSubtype: Union func desc
    0x00,   // bMasterInterface: Communication class interface
    0x01,   // bSlaveInterface0: Data Class Interface

    // Endpoint 2 Descriptor
    0x07,                           // bLength: Endpoint Descriptor size
    USB_ENDPOINT_DESCRIPTOR_TYPE,   // bDescriptorType: Endpoint
    CDC_CMD_EP,                     // bEndpointAddress
    0x03,                           // bmAttributes: Interrupt
    LOBYTE(CDC_CMD_PACKET_SZE),     // wMaxPacketSize:
    HIBYTE(CDC_CMD_PACKET_SZE),
    0x80,                           // bInterval: polling interval in frames of 1ms

    //--------------------------------------------------------------------------
    // Data class interface descriptor
    0x09,   // bLength: Endpoint Descriptor size
    USB_INTERFACE_DESCRIPTOR_TYPE,      // bDescriptorType: interface
    0x01,   // bInterfaceNumber: Number of Interface
    0x00,   // bAlternateSetting: Alternate setting
    0x02,   // bNumEndpoints: Two endpoints used
    0x0A,   // bInterfaceClass: CDC
    0x00,   // bInterfaceSubClass: ?
    0x00,   // bInterfaceProtocol: ?
    0x00,   // iInterface:

    // Endpoint OUT Descriptor
    0x07,                               // bLength: Endpoint Descriptor size
    USB_ENDPOINT_DESCRIPTOR_TYPE,       // bDescriptorType: Endpoint
    CDC_OUT_EP,                         // bEndpointAddress
    0x02,                               // bmAttributes: Bulk
    LOBYTE(CDC_DATA_MAX_PACKET_SIZE),   // wMaxPacketSize:
    HIBYTE(CDC_DATA_MAX_PACKET_SIZE),
    0x00,                               // bInterval: ignore for Bulk transfer

    // Endpoint IN Descriptor
    0x07,                               // bLength: Endpoint Descriptor size
    USB_ENDPOINT_DESCRIPTOR_TYPE,       // bDescriptorType: Endpoint
    CDC_IN_EP,                          // bEndpointAddress
    0x02,                               // bmAttributes: Bulk
    LOBYTE(CDC_DATA_MAX_PACKET_SIZE),   // wMaxPacketSize:
    HIBYTE(CDC_DATA_MAX_PACKET_SIZE),
    0x00,                               // bInterval: ignore for Bulk transfer

#if USB_PYB_USE_MSC
    //==========================================================================
    // MSC only has 1 interface so doesn't need an IAD

    //--------------------------------------------------------------------------
    // Interface Descriptor
    0x09,   // bLength: Interface Descriptor size
    USB_INTERFACE_DESCRIPTOR_TYPE,      // bDescriptorType: interface descriptor
    0x02,   // bInterfaceNumber: Number of Interface
    0x00,   // bAlternateSetting: Alternate setting
    0x02,   // bNumEndpoints
    0x08,   // bInterfaceClass: MSC Class
    0x06,   // bInterfaceSubClass : SCSI transparent
    0x50,   // nInterfaceProtocol
    0x00,   // iInterface:

    // Endpoint IN descriptor
    0x07,                           // bLength: Endpoint descriptor length
    USB_ENDPOINT_DESCRIPTOR_TYPE,   // bDescriptorType: Endpoint descriptor type
    MSC_IN_EP,                      // bEndpointAddress: IN, address 3
    0x02,                           // bmAttributes: Bulk endpoint type
    LOBYTE(MSC_MAX_PACKET),         // wMaxPacketSize
    HIBYTE(MSC_MAX_PACKET),
    0x00,                           // bInterval: ignore for Bulk transfer

    // Endpoint OUT descriptor
    0x07,                           // bLength: Endpoint descriptor length
    USB_ENDPOINT_DESCRIPTOR_TYPE,   // bDescriptorType: Endpoint descriptor type
    MSC_OUT_EP,                     // bEndpointAddress: OUT, address 3
    0x02,                           // bmAttributes: Bulk endpoint type
    LOBYTE(MSC_MAX_PACKET),         // wMaxPacketSize
    HIBYTE(MSC_MAX_PACKET),
    0x00,                           // bInterval: ignore for Bulk transfer

#else
    //==========================================================================
    // HID only has 1 interface so doesn't need an IAD

    //--------------------------------------------------------------------------
    // Interface Descriptor
    0x09,   // bLength: Interface Descriptor size
    USB_INTERFACE_DESCRIPTOR_TYPE,      // bDescriptorType: interface descriptor
    0x02,   // bInterfaceNumber: Number of Interface
    0x00,   // bAlternateSetting: Alternate setting
    0x01,   // bNumEndpoints
    0x03,   // bInterfaceClass: HID Class
    0x01,   // bInterfaceSubClass: 0=no boot, 1=BOOT
    0x02,   // nInterfaceProtocol: 0=none, 1=keyboard, 2=mouse
    0x00,   // iInterface:

    // Descriptor of Joystick Mouse HID
    0x09,   // bLength: HID Descriptor size
    HID_DESCRIPTOR_TYPE, // bDescriptorType: HID
    0x11,   // bcdHID: HID Class Spec release number, low byte
    0x01,   // bcdHID: high byte
    0x00,   // bCountryCode: Hardware target country (0=unsupported)
    0x01,   // bNumDescriptors: Number of HID class descriptors to follow
    HID_REPORT_DESC,            // bDescriptorType: report
    HID_MOUSE_REPORT_DESC_SIZE, // wItemLength: Total length of Report descriptor
    0x00,

    // Endpoint IN descriptor
    0x07,                           // bLength: Endpoint descriptor length
    USB_ENDPOINT_DESCRIPTOR_TYPE,   // bDescriptorType: Endpoint descriptor type
    HID_IN_EP,                      // bEndpointAddress: IN, address of HID
    0x03,                           // bmAttributes: Interrupt endpoint type
    LOBYTE(HID_IN_PACKET),          // wMaxPacketSize
    HIBYTE(HID_IN_PACKET),
    0x0a,                           // bInterval: polling interval, units of 1ms

#endif
};

#if !USB_PYB_USE_MSC
__ALIGN_BEGIN static uint8_t HID_MOUSE_ReportDesc[HID_MOUSE_REPORT_DESC_SIZE] __ALIGN_END =
{
  0x05,   0x01,
  0x09,   0x02,
  0xA1,   0x01,
  0x09,   0x01,
  
  0xA1,   0x00,
  0x05,   0x09,
  0x19,   0x01,
  0x29,   0x03,
  
  0x15,   0x00,
  0x25,   0x01,
  0x95,   0x03,
  0x75,   0x01,
  
  0x81,   0x02,
  0x95,   0x01,
  0x75,   0x05,
  0x81,   0x01,
  
  0x05,   0x01,
  0x09,   0x30,
  0x09,   0x31,
  0x09,   0x38,
  
  0x15,   0x81,
  0x25,   0x7F,
  0x75,   0x08,
  0x95,   0x03,
  
  0x81,   0x06,
  0xC0,   0x09,
  0x3c,   0x05,
  0xff,   0x09,
  
  0x01,   0x15,
  0x00,   0x25,
  0x01,   0x75,
  0x01,   0x95,
  
  0x02,   0xb1,
  0x22,   0x75,
  0x06,   0x95,
  0x01,   0xb1,
  
  0x01,   0xc0
}; 
#endif

/** @defgroup usbd_pyb_Private_Functions
  * @{
  */ 

/**
  * @brief  usbd_pyb_Init
  *         Initilaize the PYB interface
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t usbd_pyb_Init(void *pdev, uint8_t cfgidx) {
    // deinit first to reset
    usbd_pyb_DeInit(pdev, cfgidx);

    //----------------------------------
    // CDC VCP component

    // Open EP IN
    DCD_EP_Open(pdev,
                CDC_IN_EP,
                CDC_DATA_IN_PACKET_SIZE,
                USB_OTG_EP_BULK);

    // Open EP OUT
    DCD_EP_Open(pdev,
                CDC_OUT_EP,
                CDC_DATA_OUT_PACKET_SIZE,
                USB_OTG_EP_BULK);

    // Open Command IN EP
    DCD_EP_Open(pdev,
                CDC_CMD_EP,
                CDC_CMD_PACKET_SZE,
                USB_OTG_EP_INT);

    /*
    // can use this to dynamically set the device class
    uint8_t *pbuf = USBD_DeviceDesc;
    pbuf[4] = DEVICE_CLASS_CDC;
    pbuf[5] = DEVICE_SUBCLASS_CDC;
    */

    // Initialize the Interface physical components
    VCP_fops.pIf_Init();

    // Prepare Out endpoint to receive next packet */
    DCD_EP_PrepareRx(pdev,
                     CDC_OUT_EP,
                     (uint8_t*)(USB_Rx_Buffer),
                     CDC_DATA_OUT_PACKET_SIZE);

#if USB_PYB_USE_MSC
    //----------------------------------
    // MSC component

    // Open EP IN
    DCD_EP_Open(pdev,
                MSC_IN_EP,
                MSC_EPIN_SIZE,
                USB_OTG_EP_BULK);

    // Open EP OUT
    DCD_EP_Open(pdev,
                MSC_OUT_EP,
                MSC_EPOUT_SIZE,
                USB_OTG_EP_BULK);

    // Init the BOT layer
    MSC_BOT_Init(pdev);

#else
    //----------------------------------
    // HID component

    // Open EP IN
    DCD_EP_Open(pdev,
                HID_IN_EP,
                HID_IN_PACKET,
                USB_OTG_EP_INT);
#endif

    return USBD_OK;
}

/**
  * @brief  usbd_pyb_Init
  *         DeInitialize the CDC layer
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t usbd_pyb_DeInit(void *pdev, uint8_t cfgidx) {
    //----------------------------------
    // CDC VCP component
    // close CDC EPs
    DCD_EP_Close(pdev, CDC_IN_EP);
    DCD_EP_Close(pdev, CDC_OUT_EP);
    DCD_EP_Close(pdev, CDC_CMD_EP);

    // Restore default state of the Interface physical components
    VCP_fops.pIf_DeInit();

#if USB_PYB_USE_MSC
    //----------------------------------
    // MSC component

    // Close MSC EPs
    DCD_EP_Close(pdev, MSC_IN_EP);
    DCD_EP_Close(pdev, MSC_OUT_EP);

    // Un Init the BOT layer
    MSC_BOT_DeInit(pdev);

#else
    //----------------------------------
    // HID component

    // Close HID EP
    DCD_EP_Close(pdev, HID_IN_EP);
#endif

    return USBD_OK;
}

#define BOT_GET_MAX_LUN              0xFE
#define BOT_RESET                    0xFF

#define HID_REQ_SET_PROTOCOL         (0x0B)
#define HID_REQ_GET_PROTOCOL         (0x03)
#define HID_REQ_SET_IDLE             (0x0A)
#define HID_REQ_GET_IDLE             (0x02)
#define HID_REQ_SET_REPORT           (0x09) // used?
#define HID_REQ_GET_REPORT           (0x01) // used?

/**
  * @brief  usbd_pyb_Setup
  *         Handle the CDC specific requests
  * @param  pdev: instance
  * @param  req: usb requests
  * @retval status
  */
static uint8_t usbd_pyb_Setup(void *pdev, USB_SETUP_REQ *req) {
    switch (req->bmRequest & (USB_REQ_TYPE_MASK | USB_REQ_RECIPIENT_MASK)) {

        // Standard Device Request ---------------------------------------------
        case (USB_REQ_TYPE_STANDARD | USB_REQ_RECIPIENT_DEVICE):
            /*
            switch (req->bRequest) {
                case USB_REQ_GET_DESCRIPTOR:
                {
                    uint16_t len = USB_CDC_DESC_SIZ; // XXX WRONG!
                    uint8_t *pbuf = usbd_pyb_CfgDesc + 9;
                    if ((req->wValue >> 8) == CDC_DESCRIPTOR_TYPE) {
                        pbuf = usbd_pyb_CfgDesc + 9 + (9 * USBD_ITF_MAX_NUM); // TODO
                        len = MIN(USB_CDC_DESC_SIZ, req->wLength); // TODO
                    }
                    return USBD_CtlSendData(pdev, pbuf, len);
                    // TODO stuff here for HID, using HID_MOUSE_ReportDesc
                }
            }
            */
            break;

        // Standard Interface Request ------------------------------------------
        case (USB_REQ_TYPE_STANDARD | USB_REQ_RECIPIENT_INTERFACE):
            switch (req->bRequest) {
                case USB_REQ_GET_DESCRIPTOR: // needed for HID; SU 0x81 0x06 0x2200 0x00 request
                    // wIndex & 0xff is the interface
                    if ((req->wIndex & 0xff) <= 1) {
                        // CDC VCP
                    } else {
#if USB_PYB_USE_MSC
#else
                        uint16_t len = 0;
                        uint8_t *pbuf = NULL;
                        if (req->wValue >> 8 == HID_REPORT_DESC) {
                            len = MIN(HID_MOUSE_REPORT_DESC_SIZE , req->wLength);
                            pbuf = HID_MOUSE_ReportDesc;
                            return USBD_CtlSendData(pdev, pbuf, len);
                        } else if( req->wValue >> 8 == HID_DESCRIPTOR_TYPE) {
                            pbuf = usbd_pyb_CfgDesc + /* skip VCP */ 0x09 + 0x08 + 0x09 + 0x05 + 0x05 + 0x04 + 0x05 + 0x07 + 0x09 + 0x07 + 0x07 + /* skip iface descr */ 0x09;
                            len = MIN(0x09, req->wLength);
                            return USBD_CtlSendData(pdev, pbuf, len);
                        }
#endif
                    }
                    break;

                case USB_REQ_GET_INTERFACE:
                    // wIndex & 0xff is the interface
                    if ((req->wIndex & 0xff) <= 1) {
                        return USBD_CtlSendData(pdev, &usbd_cdc_AltSet, 1);
                    } else {
#if USB_PYB_USE_MSC
                        return USBD_CtlSendData(pdev, &USBD_MSC_AltSet, 1);
#else
                        return USBD_CtlSendData(pdev, &USBD_HID_AltSet, 1);
#endif
                    }

                case USB_REQ_SET_INTERFACE:
                    if ((uint8_t)(req->wValue) < USBD_ITF_MAX_NUM) { // TODO
                        if ((req->wIndex & 0xff) <= 1) {
                            usbd_cdc_AltSet = req->wValue;
                        } else {
#if USB_PYB_USE_MSC
                            USBD_MSC_AltSet = req->wValue;
#else
                            USBD_HID_AltSet = req->wValue;
#endif
                        }
                        return USBD_OK;
                    }
            }
            break;

        // Standard Endpoint Request -------------------------------------------
        case (USB_REQ_TYPE_STANDARD | USB_REQ_RECIPIENT_ENDPOINT):
            // req->wIndex is the endpoint number, including direction
#if USB_PYB_USE_MSC
            if (req->wIndex == MSC_IN_EP || req->wIndex == MSC_OUT_EP) {
                // MSC component
                switch (req->bRequest) {
                    case USB_REQ_CLEAR_FEATURE:

                        // Flush the FIFO and Clear the stall status
                        DCD_EP_Flush(pdev, (uint8_t)req->wIndex);

                        // Re-activate the EP
                        DCD_EP_Close(pdev, (uint8_t)req->wIndex);

                        if ((((uint8_t)req->wIndex) & 0x80) == 0x80) {
                            DCD_EP_Open(pdev, ((uint8_t)req->wIndex), MSC_EPIN_SIZE, USB_OTG_EP_BULK);
                        } else {
                            DCD_EP_Open(pdev, ((uint8_t)req->wIndex), MSC_EPOUT_SIZE, USB_OTG_EP_BULK);
                        }

                        // Handle BOT error
                        MSC_BOT_CplClrFeature(pdev, (uint8_t)req->wIndex);
                        return USBD_OK;
                }
            }
#endif
            break;

        // CDC Class Requests ------------------------------
        case (USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE):
            // req->wIndex is the recipient interface number
            if (req->wIndex == 0) {
                // CDC component, communications interface (TODO do we need to handle if#1?)

                // Check if the request is a data setup packet
                if (req->wLength) {
                    if (req->bmRequest & 0x80) {
                        // Device-to-Host request

                        // Get the data to be sent to Host from interface layer
                        VCP_fops.pIf_Ctrl(req->bRequest, CmdBuff, req->wLength);

                        // Send the data to the host
                        return USBD_CtlSendData(pdev, CmdBuff, req->wLength);

                    } else {
                        // Host-to-Device requeset

                        // Set the value of the current command to be processed */
                        cdcCmd = req->bRequest;
                        cdcLen = req->wLength;

                        // Prepare the reception of the buffer over EP0
                        // Next step: the received data will be managed in usbd_cdc_EP0_TxSent() function.
                        return USBD_CtlPrepareRx(pdev, CmdBuff, req->wLength);
                    }
                } else {
                    // Not a Data request

                    // Transfer the command to the interface layer */
                    return VCP_fops.pIf_Ctrl(req->bRequest, NULL, req->wValue);
                }

            } else if (req->wIndex == 2) {
#if USB_PYB_USE_MSC
                // MSC component
                switch (req->bRequest) {
                    case BOT_GET_MAX_LUN:
                        if ((req->wValue  == 0) && (req->wLength == 1) && ((req->bmRequest & 0x80) == 0x80)) {
                            USBD_MSC_MaxLun = USBD_STORAGE_fops->GetMaxLun();
                            if (USBD_MSC_MaxLun > 0) {
                                return USBD_CtlSendData(pdev, &USBD_MSC_MaxLun, 1);
                            }
                        }
                        break;

                    case BOT_RESET:
                        if ((req->wValue  == 0) && (req->wLength == 0) && ((req->bmRequest & 0x80) != 0x80)) {
                            MSC_BOT_Reset(pdev);
                            return USBD_OK;
                        }
                        break;
                }
#else
                // HID component
                switch (req->bRequest) {
                    case HID_REQ_SET_PROTOCOL:
                        USBD_HID_Protocol = req->wValue;
                        return USBD_OK;

                    case HID_REQ_GET_PROTOCOL:
                        return USBD_CtlSendData(pdev, &USBD_HID_Protocol, 1);

                    case HID_REQ_SET_IDLE:
                        USBD_HID_IdleState = (req->wValue >> 8);
                        return USBD_OK;

                    case HID_REQ_GET_IDLE:
                        return USBD_CtlSendData(pdev, &USBD_HID_IdleState, 1);
                }
#endif
            }
            break;
    }

    printf("SU %x %x %x %x\n", req->bmRequest, req->bRequest, req->wValue, req->wIndex);

    // invalid command
    USBD_CtlError(pdev, req);
    return USBD_FAIL;
}

/**
  * @brief  usbd_pyb_EP0_RxReady
  *         Data received on control endpoint
  * @param  pdev: device device instance
  * @retval status
  */
static uint8_t usbd_pyb_EP0_RxReady(void *pdev) {
    if (cdcCmd != NO_CMD) {
        // Process the data
        VCP_fops.pIf_Ctrl(cdcCmd, CmdBuff, cdcLen);

        // Reset the command variable to default value
        cdcCmd = NO_CMD;
    }

    return USBD_OK;
}

/**
  * @brief  usbd_pyb_DataIn
  *         Data sent on non-control IN endpoint
  * @param  pdev: device instance
  * @param  epnum: endpoint number
  * @retval status
  */
static uint8_t usbd_pyb_DataIn(void *pdev, uint8_t epnum) {
    uint16_t USB_Tx_ptr;
    uint16_t USB_Tx_length;

    switch (epnum) {
        case (CDC_IN_EP & 0x7f): // TODO is this correct?
        case (CDC_CMD_EP & 0x7f): // TODO is this correct?
            if (USB_Tx_State == 1) {
                if (APP_Rx_length == 0) {
                    USB_Tx_State = 0;
                } else {
                    if (APP_Rx_length > CDC_DATA_IN_PACKET_SIZE) {
                        USB_Tx_ptr = APP_Rx_ptr_out;
                        USB_Tx_length = CDC_DATA_IN_PACKET_SIZE;

                        APP_Rx_ptr_out += CDC_DATA_IN_PACKET_SIZE;
                        APP_Rx_length -= CDC_DATA_IN_PACKET_SIZE;
                    } else {
                        USB_Tx_ptr = APP_Rx_ptr_out;
                        USB_Tx_length = APP_Rx_length;

                        APP_Rx_ptr_out += APP_Rx_length;
                        APP_Rx_length = 0;
                    }

                    // Prepare the available data buffer to be sent on IN endpoint
                    DCD_EP_Tx(pdev,
                              CDC_IN_EP,
                              (uint8_t*)&APP_Rx_Buffer[USB_Tx_ptr],
                              USB_Tx_length);
                }
            }
            return USBD_OK;

#if USB_PYB_USE_MSC
        case (MSC_IN_EP & 0x7f): // TODO?
            MSC_BOT_DataIn(pdev, epnum);
            return USBD_OK;

#else
        case (HID_IN_EP & 0x7f):
            /* Ensure that the FIFO is empty before a new transfer, this condition could
            be caused by  a new transfer before the end of the previous transfer */
            DCD_EP_Flush(pdev, HID_IN_EP);
            return USBD_OK;
#endif
    }

    printf("DI %x\n", epnum);

    return USBD_OK;
}

/**
  * @brief  usbd_pyb_DataOut
  *         Data received on non-control Out endpoint
  * @param  pdev: device instance
  * @param  epnum: endpoint number
  * @retval status
  */
static uint8_t usbd_pyb_DataOut(void *pdev, uint8_t epnum) {
    uint16_t USB_Rx_Cnt;

    switch (epnum) {
        case (CDC_OUT_EP & 0x7f): // TODO is this correct?
            // Get the received data buffer and update the counter */
            USB_Rx_Cnt = ((USB_OTG_CORE_HANDLE*)pdev)->dev.out_ep[epnum].xfer_count;

            /* USB data will be immediately processed, this allow next USB traffic being 
               NAKed till the end of the application Xfer */
            VCP_fops.pIf_DataRx(USB_Rx_Buffer, USB_Rx_Cnt);

            // Prepare Out endpoint to receive next packet */
            DCD_EP_PrepareRx(pdev,
                             CDC_OUT_EP,
                             (uint8_t*)(USB_Rx_Buffer),
                             CDC_DATA_OUT_PACKET_SIZE);
            return USBD_OK;

#if USB_PYB_USE_MSC
        case (MSC_OUT_EP & 0x7f): // TODO is this correct?
            MSC_BOT_DataOut(pdev, epnum);
            return USBD_OK;
#endif
    }

    printf("DO %x\n", epnum);

    return USBD_OK;
}

/**
  * @brief  usbd_pyb_SOF
  *         Start Of Frame event management
  * @param  pdev: instance
  * @retval status
  */
static uint8_t usbd_pyb_SOF(void *pdev) {
    static uint32_t FrameCount = 0;

    // TODO do we need to check that this is for CDC/VCP? can we even do that?

    if (FrameCount++ == CDC_IN_FRAME_INTERVAL) {
        // Reset the frame counter */
        FrameCount = 0;

        // Check the data to be sent through IN pipe */
        Handle_USBAsynchXfer(pdev);
    }

    return USBD_OK;
}

/**
  * @brief  Handle_USBAsynchXfer
  *         Send data to USB
  * @param  pdev: instance
  * @retval None
  */
static void Handle_USBAsynchXfer (void *pdev)
{
  uint16_t USB_Tx_ptr;
  uint16_t USB_Tx_length;
  
  if(USB_Tx_State != 1)
  {
    if (APP_Rx_ptr_out == APP_RX_DATA_SIZE)
    {
      APP_Rx_ptr_out = 0;
    }
    
    if(APP_Rx_ptr_out == APP_Rx_ptr_in) 
    {
      USB_Tx_State = 0; 
      return;
    }
    
    if(APP_Rx_ptr_out > APP_Rx_ptr_in) // rollback */
    { 
      APP_Rx_length = APP_RX_DATA_SIZE - APP_Rx_ptr_out;
    
    }
    else 
    {
      APP_Rx_length = APP_Rx_ptr_in - APP_Rx_ptr_out;
     
    }
#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
     APP_Rx_length &= ~0x03;
#endif // USB_OTG_HS_INTERNAL_DMA_ENABLED */
    
    if (APP_Rx_length > CDC_DATA_IN_PACKET_SIZE)
    {
      USB_Tx_ptr = APP_Rx_ptr_out;
      USB_Tx_length = CDC_DATA_IN_PACKET_SIZE;
      
      APP_Rx_ptr_out += CDC_DATA_IN_PACKET_SIZE;	
      APP_Rx_length -= CDC_DATA_IN_PACKET_SIZE;
    }
    else
    {
      USB_Tx_ptr = APP_Rx_ptr_out;
      USB_Tx_length = APP_Rx_length;
      
      APP_Rx_ptr_out += APP_Rx_length;
      APP_Rx_length = 0;
    }
    USB_Tx_State = 1; 

    DCD_EP_Tx (pdev,
               CDC_IN_EP,
               (uint8_t*)&APP_Rx_Buffer[USB_Tx_ptr],
               USB_Tx_length);
  }  
  
}

/**
  * @brief  usbd_pyb_GetCfgDesc 
  *         Return configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t *usbd_pyb_GetCfgDesc(uint8_t speed, uint16_t *length) {
    *length = sizeof(usbd_pyb_CfgDesc);
    return usbd_pyb_CfgDesc;
}

/**
  * @brief  USBD_HID_SendReport
  *         Send HID Report
  * @param  pdev: device instance
  * @param  buff: pointer to report (4 bytes: ?, x, y, ?)
  * @retval status
  */
/*
uint8_t USBD_HID_SendReport(USB_OTG_CORE_HANDLE *pdev, uint8_t *report, uint16_t len) {
    if (pdev->dev.device_status == USB_OTG_CONFIGURED) {
        DCD_EP_Tx(pdev, HID_IN_EP, report, len);
    }
    return USBD_OK;
}
*/
