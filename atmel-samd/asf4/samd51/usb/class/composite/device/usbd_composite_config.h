/* Auto-generated config file usbd_composite_config.h */
#ifndef USBD_COMPOSITE_CONFIG_H
#define USBD_COMPOSITE_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h> Composite Device Descriptor

// <o> bcdUSB
// <0x0200=> USB 2.0 version
// <0x0210=> USB 2.1 version
// <id> usb_composite_bcdusb
#ifndef CONF_USB_COMPOSITE_BCDUSB
#define CONF_USB_COMPOSITE_BCDUSB 0x200
#endif

// <o> bMaxPackeSize0
// <0x0008=> 8 bytes
// <0x0010=> 16 bytes
// <0x0020=> 32 bytes
// <0x0040=> 64 bytes
// <id> usb_composite_bmaxpksz0
#ifndef CONF_USB_COMPOSITE_BMAXPKSZ0
#define CONF_USB_COMPOSITE_BMAXPKSZ0 0x40
#endif

// <o> idVender <0x0000-0xFFFF>
// <id> usb_composite_idvender
#ifndef CONF_USB_COMPOSITE_IDVENDER
#define CONF_USB_COMPOSITE_IDVENDER 0x3eb
#endif

// <o> idProduct <0x0000-0xFFFF>
// <id> usb_composite_idproduct
#ifndef CONF_USB_COMPOSITE_IDPRODUCT
#define CONF_USB_COMPOSITE_IDPRODUCT 0x2421
#endif

// <o> bcdDevice <0x0000-0xFFFF>
// <id> usb_composite_bcddevice
#ifndef CONF_USB_COMPOSITE_BCDDEVICE
#define CONF_USB_COMPOSITE_BCDDEVICE 0x100
#endif

// <o> iManufacturer <0x00-0xFF>
// <id> usb_composite_imanufact
#ifndef CONF_USB_COMPOSITE_IMANUFACT
#define CONF_USB_COMPOSITE_IMANUFACT 0x0
#endif

#ifndef CONF_USB_COMPOSITE_LANGUAGE_ID_STR_DESC
#define CONF_USB_COMPOSITE_LANGUAGE_ID_STR_DESC 0x04, 0x03, 0x09, 0x04
#endif

#ifndef CONF_USB_COMPOSITE_MANUFACTOR_STR_DESC
#define CONF_USB_COMPOSITE_MANUFACTOR_STR_DESC                                                                         \
	0x14, 0x03, 0x41, 0x00, 0x54, 0x00, 0x4D, 0x00, 0x45, 0x00, 0x4C, 0x00, 0x20, 0x00, 0x41, 0x00, 0x53, 0x00, 0x46,  \
	    0x00
#endif

// <o> iProduct <0x00-0xFF>
// <id> usb_composite_iproduct
#ifndef CONF_USB_COMPOSITE_IPRODUCT
#define CONF_USB_COMPOSITE_IPRODUCT 0x0
#endif

#ifndef CONF_USB_COMPOSITE_PRODUCT_STR_DESC
#define CONF_USB_COMPOSITE_PRODUCT_STR_DESC                                                                            \
	0x22, 0x03, 0x43, 0x00, 0x6f, 0x00, 0x6d, 0x00, 0x70, 0x00, 0x6f, 0x00, 0x73, 0x00, 0x69, 0x00, 0x74, 0x00, 0x65,  \
	    0x00, 0x20, 0x00, 0x44, 0x00, 0x65, 0x00, 0x76, 0x00, 0x69, 0x00, 0x63, 0x00, 0x65, 0x00
#endif

// <o> iSerialNumber <0x00-0xFF>
// <id> usb_composite_iserialnum
#ifndef CONF_USB_COMPOSITE_ISERIALNUM
#define CONF_USB_COMPOSITE_ISERIALNUM 0x0
#endif

// <o> bNumConfigurations <0x01-0xFF>
// <id> usb_composite_bnumconfig
#ifndef CONF_USB_COMPOSITE_BNUMCONFIG
#define CONF_USB_COMPOSITE_BNUMCONFIG 0x1
#endif
// </h>

// <h> Composite Configuration Descriptor

// <o> bConfigurationValue <0x01-0xFF>
// <id> usb_composite_bconfigval
#ifndef CONF_USB_COMPOSITE_BCONFIGVAL
#define CONF_USB_COMPOSITE_BCONFIGVAL 0x1
#endif

// <o> iConfiguration <0x00-0xFF>
// <id> usb_composite_iconfig
#ifndef CONF_USB_COMPOSITE_ICONFIG
#define CONF_USB_COMPOSITE_ICONFIG 0x0
#endif

// <o> bmAttributes
// <0x80=> Bus power supply, not support for remote wakeup
// <0xA0=> Bus power supply, support for remote wakeup
// <0xC0=> Self powered, not support for remote wakeup
// <0xE0=> Self powered, support for remote wakeup
// <id> usb_composite_bmattri
#ifndef CONF_USB_COMPOSITE_BMATTRI
#define CONF_USB_COMPOSITE_BMATTRI 0x80
#endif

// <o> bMaxPower <0x00-0xFF>
// <id> usb_composite_bmaxpower
#ifndef CONF_USB_COMPOSITE_BMAXPOWER
#define CONF_USB_COMPOSITE_BMAXPOWER 0x32
#endif
// </h>

// <e> CDC ACM Support
// <id> usb_composite_cdc_acm_support
#ifndef CONF_USB_COMPOSITE_CDC_ACM_EN
#define CONF_USB_COMPOSITE_CDC_ACM_EN 1
#endif

// <o> CDC ACM Comm Interrupt IN Endpoint Address
// <0x81=> EndpointAddress = 0x81
// <0x82=> EndpointAddress = 0x82
// <0x83=> EndpointAddress = 0x83
// <0x84=> EndpointAddress = 0x84
// <0x85=> EndpointAddress = 0x85
// <0x86=> EndpointAddress = 0x86
// <0x87=> EndpointAddress = 0x87
// <id> usb_composite_cdc_acm_epaddr
#ifndef CONF_USB_COMPOSITE_CDC_ACM_COMM_INT_EPADDR
#define CONF_USB_COMPOSITE_CDC_ACM_COMM_INT_EPADDR 0x82
#endif

// <o> CDC ACM Comm Interrupt IN Endpoint wMaxPacketSize
// <0x0008=> 8 bytes
// <0x0010=> 16 bytes
// <0x0020=> 32 bytes
// <0x0040=> 64 bytes
// <id> usb_composite_cdc_acm_comm_int_maxpksz
#ifndef CONF_USB_COMPOSITE_CDC_ACM_COMM_INT_MAXPKSZ
#define CONF_USB_COMPOSITE_CDC_ACM_COMM_INT_MAXPKSZ 0x40
#endif

// <o> CDC ACM Data BULK IN Endpoint Address
// <0x81=> EndpointAddress = 0x81
// <0x82=> EndpointAddress = 0x82
// <0x83=> EndpointAddress = 0x83
// <0x84=> EndpointAddress = 0x84
// <0x85=> EndpointAddress = 0x85
// <0x86=> EndpointAddress = 0x86
// <0x87=> EndpointAddress = 0x87
// <id> usb_composite_cdc_acm_data_bulkin_epaddr
#ifndef CONF_USB_COMPOSITE_CDC_ACM_DATA_BULKIN_EPADDR
#define CONF_USB_COMPOSITE_CDC_ACM_DATA_BULKIN_EPADDR 0x81
#endif

// <o> CDC ACM Data BULK IN Endpoint wMaxPacketSize
// <0x0008=> 8 bytes
// <0x0010=> 16 bytes
// <0x0020=> 32 bytes
// <0x0040=> 64 bytes
// <id> usb_composite_cdc_acm_data_builin_maxpksz
#ifndef CONF_USB_COMPOSITE_CDC_ACM_DATA_BULKIN_MAXPKSZ
#define CONF_USB_COMPOSITE_CDC_ACM_DATA_BULKIN_MAXPKSZ 0x40
#endif

// <o> CDC ACM Data BULK OUT Endpoint Address
// <0x01=> EndpointAddress = 0x01
// <0x02=> EndpointAddress = 0x02
// <0x03=> EndpointAddress = 0x03
// <0x04=> EndpointAddress = 0x04
// <0x05=> EndpointAddress = 0x05
// <0x06=> EndpointAddress = 0x06
// <0x07=> EndpointAddress = 0x07
// <id> usb_composite_cdc_acm_data_bulkout_epaddr
#ifndef CONF_USB_COMPOSITE_CDC_ACM_DATA_BULKOUT_EPADDR
#define CONF_USB_COMPOSITE_CDC_ACM_DATA_BULKOUT_EPADDR 0x1
#endif

// <o> CDC ACM Data BULK OUT Endpoint wMaxPacketSize
// <0x0008=> 8 bytes
// <0x0010=> 16 bytes
// <0x0020=> 32 bytes
// <0x0040=> 64 bytes
// <id> usb_composite_cdc_acm_data_buckout_maxpksz
#ifndef CONF_USB_COMPOSITE_CDC_ACM_DATA_BULKOUT_MAXPKSZ
#define CONF_USB_COMPOSITE_CDC_ACM_DATA_BULKOUT_MAXPKSZ 0x40
#endif
// </e>

// <e> HID Mouse Support
// <id> usb_composite_hid_mouse_support
#ifndef CONF_USB_COMPOSITE_HID_MOUSE_EN
#define CONF_USB_COMPOSITE_HID_MOUSE_EN 0
#endif

// <o> HID Mouse INTERRUPT IN Endpoint Address
// <0x81=> EndpointAddress = 0x81
// <0x82=> EndpointAddress = 0x82
// <0x83=> EndpointAddress = 0x83
// <0x84=> EndpointAddress = 0x84
// <0x85=> EndpointAddress = 0x85
// <0x86=> EndpointAddress = 0x86
// <0x87=> EndpointAddress = 0x87
// <id> usb_composite_hid_mouse_intin_epaddr
// <i> Please make sure that the setting here is coincide with the endpoint setting in USB device driver.
#ifndef CONF_USB_COMPOSITE_HID_MOUSE_INTIN_EPADDR
#define CONF_USB_COMPOSITE_HID_MOUSE_INTIN_EPADDR 0x83
#endif

// <o> HID Mouse INTERRUPT IN Endpoint wMaxPacketSize
// <0x0008=> 8 bytes
// <0x0010=> 16 bytes
// <0x0020=> 32 bytes
// <0x0040=> 64 bytes
// <id> usb_composite_hid_mouse_intin_maxpksz
// <i> Please make sure that the setting here is coincide with the endpoint setting in USB device driver.
#ifndef CONF_USB_COMPOSITE_HID_MOUSE_INTIN_MAXPKSZ
#define CONF_USB_COMPOSITE_HID_MOUSE_INTIN_MAXPKSZ 0x8
#endif

// </e>

// <e> HID Keyboard Support
// <id> usb_composite_hid_keyboard_support
#ifndef CONF_USB_COMPOSITE_HID_KEYBOARD_EN
#define CONF_USB_COMPOSITE_HID_KEYBOARD_EN 0
#endif

// <o> HID Keyboard INTERRUPT IN Endpoint Address
// <0x81=> EndpointAddress = 0x81
// <0x82=> EndpointAddress = 0x82
// <0x83=> EndpointAddress = 0x83
// <0x84=> EndpointAddress = 0x84
// <0x85=> EndpointAddress = 0x85
// <0x86=> EndpointAddress = 0x86
// <0x87=> EndpointAddress = 0x87
// <id> usb_composite_hid_keyboard_intin_epaddr
// <i> Please make sure that the setting here is coincide with the endpoint setting in USB device driver.
#ifndef CONF_USB_COMPOSITE_HID_KEYBOARD_INTIN_EPADDR
#define CONF_USB_COMPOSITE_HID_KEYBOARD_INTIN_EPADDR 0x84
#endif

// <o> HID Keyboard INTERRUPT IN Endpoint wMaxPacketSize
// <0x0008=> 8 bytes
// <0x0010=> 16 bytes
// <0x0020=> 32 bytes
// <0x0040=> 64 bytes
// <id> usb_composite_hid_keyboard_intin_maxpksz
// <i> Please make sure that the setting here is coincide with the endpoint setting in USB device driver.
#ifndef CONF_USB_COMPOSITE_HID_KEYBOARD_INTIN_MAXPKSZ
#define CONF_USB_COMPOSITE_HID_KEYBOARD_INTIN_MAXPKSZ 0x8
#endif

// <o> HID Keyboard INTERRUPT OUT Endpoint Address
// <0x01=> EndpointAddress = 0x01
// <0x02=> EndpointAddress = 0x02
// <0x03=> EndpointAddress = 0x03
// <0x04=> EndpointAddress = 0x04
// <0x05=> EndpointAddress = 0x05
// <0x06=> EndpointAddress = 0x06
// <0x07=> EndpointAddress = 0x07
// <id> usb_composite_hid_keyboard_intout_epaddr
// <i> Please make sure that the setting here is coincide with the endpoint setting in USB device driver.
#ifndef CONF_USB_COMPOSITE_HID_KEYBOARD_INTOUT_EPADDR
#define CONF_USB_COMPOSITE_HID_KEYBOARD_INTOUT_EPADDR 0x2
#endif

// <o> HID Keyboard INTERRUPT OUT Endpoint wMaxPacketSize
// <0x0008=> 8 bytes
// <0x0010=> 16 bytes
// <0x0020=> 32 bytes
// <0x0040=> 64 bytes
// <id> usb_composite_hid_keyboard_intout_maxpksz
// <i> Please make sure that the setting here is coincide with the endpoint setting in USB device driver.
#ifndef CONF_USB_COMPOSITE_HID_KEYBOARD_INTOUT_MAXPKSZ
#define CONF_USB_COMPOSITE_HID_KEYBOARD_INTOUT_MAXPKSZ 0x8
#endif

// </e>

// <e> HID Generic Support
// <id> usb_composite_hid_generic_support
#ifndef CONF_USB_COMPOSITE_HID_GENERIC_EN
#define CONF_USB_COMPOSITE_HID_GENERIC_EN 0
#endif

#ifndef CONF_USB_COMPOSITE_HID_GENERIC_REPORT_LEN
#define CONF_USB_COMPOSITE_HID_GENERIC_REPORT_LEN 53
#endif

#ifndef CONF_USB_COMPOSITE_HID_GENERIC_REPORT
#define CONF_USB_COMPOSITE_HID_GENERIC_REPORT                                                                          \
	0x06, 0xFF, 0xFF, 0x09, 0x01, 0xA1, 0x01, 0x09, 0x02, 0x09, 0x03, 0x15, 0x00, 0x26, 0xFF, 0x00, 0x75, 0x08, 0x95,  \
	    0x40, 0x81, 0x02, 0x09, 0x04, 0x09, 0x05, 0x15, 0x00, 0x26, 0xFF, 0x00, 0x75, 0x08, 0x95, 0x40, 0x91, 0x02,    \
	    0x09, 0x06, 0x09, 0x07, 0x15, 0x00, 0x26, 0xFF, 0x00, 0x75, 0x08, 0x95, 0x04, 0xB1, 0x02, 0xC0
#endif

// <o> HID Generic INTERRUPT IN Endpoint Address
// <0x81=> EndpointAddress = 0x81
// <0x82=> EndpointAddress = 0x82
// <0x83=> EndpointAddress = 0x83
// <0x84=> EndpointAddress = 0x84
// <0x85=> EndpointAddress = 0x85
// <0x86=> EndpointAddress = 0x86
// <0x87=> EndpointAddress = 0x87
// <id> usb_composite_hid_generic_intin_epaddr
// <i> Please make sure that the setting here is coincide with the endpoint setting in USB device driver.
#ifndef CONF_USB_COMPOSITE_HID_GENERIC_INTIN_EPADDR
#define CONF_USB_COMPOSITE_HID_GENERIC_INTIN_EPADDR 0x85
#endif

// <o> HID Generic INTERRUPT IN Endpoint wMaxPacketSize
// <0x0008=> 8 bytes
// <0x0010=> 16 bytes
// <0x0020=> 32 bytes
// <0x0040=> 64 bytes
// <id> usb_composite_hid_generic_intin_maxpksz
// <i> Please make sure that the setting here is coincide with the endpoint setting in USB device driver.
#ifndef CONF_USB_COMPOSITE_HID_GENERIC_INTIN_MAXPKSZ
#define CONF_USB_COMPOSITE_HID_GENERIC_INTIN_MAXPKSZ 0x40
#endif

// <o> HID Generic INTERRUPT OUT Endpoint Address
// <0x01=> EndpointAddress = 0x01
// <0x02=> EndpointAddress = 0x02
// <0x03=> EndpointAddress = 0x03
// <0x04=> EndpointAddress = 0x04
// <0x05=> EndpointAddress = 0x05
// <0x06=> EndpointAddress = 0x06
// <0x07=> EndpointAddress = 0x07
// <id> usb_composite_hid_generic_intout_epaddr
// <i> Please make sure that the setting here is coincide with the endpoint setting in USB device driver.
#ifndef CONF_USB_COMPOSITE_HID_GENERIC_INTOUT_EPADDR
#define CONF_USB_COMPOSITE_HID_GENERIC_INTOUT_EPADDR 0x3
#endif

// <o> HID Generic INTERRUPT OUT Endpoint wMaxPacketSize
// <0x0008=> 8 bytes
// <0x0010=> 16 bytes
// <0x0020=> 32 bytes
// <0x0040=> 64 bytes
// <id> usb_composite_hid_generic_intout_maxpksz
// <i> Please make sure that the setting here is coincide with the endpoint setting in USB device driver.
#ifndef CONF_USB_COMPOSITE_HID_GENERIC_INTOUT_MAXPKSZ
#define CONF_USB_COMPOSITE_HID_GENERIC_INTOUT_MAXPKSZ 0x40
#endif

// </e>

// <<< end of configuration section >>>

#endif // USBD_COMPOSITE_CONFIG_H
