#include <stdio.h>

#include "usbd_ioreq.h"
#include "usbd_desc.h"
#include "usbd_req.h"
#include "usbd_pyb_core.h"

#define USB_PYB_CONFIG_DESC_SIZ (34)

#define HID_MOUSE_REPORT_DESC_SIZE (74)

#define HID_DESCRIPTOR_TYPE (0x21)
#define HID_REPORT_DESC     (0x22)

#define HID_IN_EP           (0x81)
#define HID_IN_PACKET       (4) /* maximum, and actual, packet size */

/*********************************************
   PYB Device library callbacks
 *********************************************/

static uint8_t usbd_pyb_Init   (void *pdev, uint8_t cfgidx);
static uint8_t usbd_pyb_DeInit (void *pdev, uint8_t cfgidx);
static uint8_t usbd_pyb_Setup  (void *pdev, USB_SETUP_REQ *req);
static uint8_t usbd_pyb_DataIn (void *pdev, uint8_t epnum);

/*********************************************
   PYB specific management functions
 *********************************************/

static uint8_t *usbd_pyb_GetCfgDesc(uint8_t speed, uint16_t *length);

__ALIGN_BEGIN static uint8_t USBD_HID_AltSet __ALIGN_END = 0;
__ALIGN_BEGIN static uint8_t USBD_HID_Protocol __ALIGN_END = 0;
__ALIGN_BEGIN static uint8_t USBD_HID_IdleState __ALIGN_END = 0;

/* PYB interface class callbacks structure */
USBD_Class_cb_TypeDef USBD_PYB_HID_cb =
{
    usbd_pyb_Init,
    usbd_pyb_DeInit,
    usbd_pyb_Setup,
    NULL, // EP0_TxSent
    NULL, // usbd_pyb_EP0_RxReady,
    usbd_pyb_DataIn,
    NULL, // usbd_pyb_DataOut,
    NULL, // usbd_pyb_SOF,
    NULL, // IsoINIncomplete
    NULL, // IsoOUTIncomplete
    usbd_pyb_GetCfgDesc,
    // for OTG_HS support need to add other cfg desc here
};

/* USB PYB device Configuration Descriptor */
__ALIGN_BEGIN static uint8_t usbd_pyb_CfgDesc[USB_PYB_CONFIG_DESC_SIZ] __ALIGN_END =
{
    //--------------------------------------------------------------------------
    // Configuration Descriptor
    0x09,   // bLength: Configuration Descriptor size
    USB_CONFIGURATION_DESCRIPTOR_TYPE,      // bDescriptorType: Configuration
    LOBYTE(USB_PYB_CONFIG_DESC_SIZ),        // wTotalLength: no of returned bytes
    HIBYTE(USB_PYB_CONFIG_DESC_SIZ),
    0x01,   // bNumInterfaces: 1 interfaces
    0x01,   // bConfigurationValue: Configuration value
    0x04,   // iConfiguration: Index of string descriptor describing the configuration
    0x80,   // bmAttributes: bus powered; 0xc0 for self powered
    0xfa,   // bMaxPower: in units of 2mA

    //--------------------------------------------------------------------------
    // Interface Descriptor
    0x09,   // bLength: Interface Descriptor size
    USB_INTERFACE_DESCRIPTOR_TYPE,      // bDescriptorType: interface descriptor
    0x00,   // bInterfaceNumber: Number of Interface
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
};

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

    // Open EP IN
    DCD_EP_Open(pdev,
                HID_IN_EP,
                HID_IN_PACKET,
                USB_OTG_EP_INT);

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
    // Close HID EP
    DCD_EP_Close(pdev, HID_IN_EP);

    return USBD_OK;
}

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
            break;

        // Standard Interface Request ------------------------------------------
        case (USB_REQ_TYPE_STANDARD | USB_REQ_RECIPIENT_INTERFACE):
            switch (req->bRequest) {
                case USB_REQ_GET_DESCRIPTOR: // needed for HID; SU 0x81 0x06 0x2200 0x00 request
                    // wIndex & 0xff is the interface
                    if ((req->wIndex & 0xff) == 0) {
                          uint16_t len = 0;
                          uint8_t  *pbuf = NULL;
                          if( req->wValue >> 8 == HID_REPORT_DESC) {
                            len = MIN(HID_MOUSE_REPORT_DESC_SIZE , req->wLength);
                            pbuf = HID_MOUSE_ReportDesc;
                              return USBD_CtlSendData (pdev, pbuf, len);
                          } else if( req->wValue >> 8 == HID_DESCRIPTOR_TYPE) {
                            pbuf = usbd_pyb_CfgDesc + 0x09 + 0x09;
                            len = MIN(0x09 , req->wLength);
                              return USBD_CtlSendData (pdev, pbuf, len);
                          }
                    }

                case USB_REQ_GET_INTERFACE:
                    // wIndex & 0xff is the interface
                    if ((req->wIndex & 0xff) == 0) {
                        return USBD_CtlSendData(pdev, &USBD_HID_AltSet, 1);
                    }

                case USB_REQ_SET_INTERFACE:
                    if ((uint8_t)(req->wValue) < USBD_ITF_MAX_NUM) { // TODO
                        if ((req->wIndex & 0xff) == 0) {
                            USBD_HID_AltSet = req->wValue;
                        }
                        return USBD_OK;
                    }
            }
            break;

        // Standard Endpoint Request -------------------------------------------
        case (USB_REQ_TYPE_STANDARD | USB_REQ_RECIPIENT_ENDPOINT):
            // req->wIndex is the endpoint number, including direction
            break;

        // CDC Class Requests ------------------------------
        case (USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE):
            // req->wIndex is the recipient interface number
            if (req->wIndex == 0) {
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
            }
            break;
    }

    printf("SU %x %x %x %x\n", req->bmRequest, req->bRequest, req->wValue, req->wIndex);

    // invalid command
    USBD_CtlError(pdev, req);
    return USBD_FAIL;
}

/**
  * @brief  usbd_pyb_DataIn
  *         Data sent on non-control IN endpoint
  * @param  pdev: device instance
  * @param  epnum: endpoint number
  * @retval status
  */
static uint8_t usbd_pyb_DataIn(void *pdev, uint8_t epnum) {
    switch (epnum) {
        case (HID_IN_EP & 0x7f):
            /* Ensure that the FIFO is empty before a new transfer, this condition could
            be caused by  a new transfer before the end of the previous transfer */
            DCD_EP_Flush(pdev, HID_IN_EP);
            return USBD_OK;
    }

    printf("DI %x\n", epnum);

    return USBD_OK;
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
uint8_t USBD_HID_SendReport(USB_OTG_CORE_HANDLE *pdev, uint8_t *report, uint16_t len) {
    if (pdev->dev.device_status == USB_OTG_CONFIGURED) {
        DCD_EP_Tx(pdev, HID_IN_EP, report, len);
    }
    return USBD_OK;
}
