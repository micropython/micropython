/* Auto-generated config file usbd_config.h */
#ifndef USBD_CONFIG_H
#define USBD_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// ---- USB Device Stack Core Options ----

// <q> High Speed Support
// <i> Enable high speed specific descriptors support, e.g., DeviceQualifierDescriptor and OtherSpeedConfiguration Descriptor.
// <i> High speed support require descriptors description array on start, for LS/FS and HS support in first and second place.
// <id> usbd_hs_sp
#ifndef CONF_USBD_HS_SP
#define CONF_USBD_HS_SP 0
#endif

// ---- USB Device Stack Composite Options ----

// <e> Enable String Descriptors
// <id> usb_composite_str_en
#ifndef CONF_USB_COMPOSITE_STR_EN
#define CONF_USB_COMPOSITE_STR_EN 0
#endif
// <s> Language IDs
// <i> Language IDs in c format, split by comma (E.g., 0x0409 ...)
// <id> usb_composite_langid
#ifndef CONF_USB_COMPOSITE_LANGID
#define CONF_USB_COMPOSITE_LANGID "0x0409"
#endif

#ifndef CONF_USB_COMPOSITE_LANGID_DESC
#define CONF_USB_COMPOSITE_LANGID_DESC
#endif
// </e>

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

// <e> Enable string descriptor of iManufact
// <id> usb_composite_imanufact_en
#ifndef CONF_USB_COMPOSITE_IMANUFACT_EN
#define CONF_USB_COMPOSITE_IMANUFACT_EN 0
#endif

#ifndef CONF_USB_COMPOSITE_IMANUFACT
#define CONF_USB_COMPOSITE_IMANUFACT (CONF_USB_COMPOSITE_IMANUFACT_EN * (CONF_USB_COMPOSITE_IMANUFACT_EN))
#endif

// <s> Unicode string of iManufact
// <id> usb_composite_imanufact_str
#ifndef CONF_USB_COMPOSITE_IMANUFACT_STR
#define CONF_USB_COMPOSITE_IMANUFACT_STR "Atmel"
#endif

#ifndef CONF_USB_COMPOSITE_IMANUFACT_STR_DESC
#define CONF_USB_COMPOSITE_IMANUFACT_STR_DESC
#endif

// </e>

// <e> Enable string descriptor of iProduct
// <id> usb_composite_iproduct_en
#ifndef CONF_USB_COMPOSITE_IPRODUCT_EN
#define CONF_USB_COMPOSITE_IPRODUCT_EN 0
#endif

#ifndef CONF_USB_COMPOSITE_IPRODUCT
#define CONF_USB_COMPOSITE_IPRODUCT                                                                                    \
	(CONF_USB_COMPOSITE_IPRODUCT_EN * (CONF_USB_COMPOSITE_IMANUFACT_EN + CONF_USB_COMPOSITE_IPRODUCT_EN))
#endif

// <s> Unicode string of iProduct
// <id> usb_composite_iproduct_str
#ifndef CONF_USB_COMPOSITE_IPRODUCT_STR
#define CONF_USB_COMPOSITE_IPRODUCT_STR "Composite Demo"
#endif

#ifndef CONF_USB_COMPOSITE_IPRODUCT_STR_DESC
#define CONF_USB_COMPOSITE_IPRODUCT_STR_DESC
#endif

// </e>

// <e> Enable string descriptor of iSerialNum
// <id> usb_composite_iserialnum_en
#ifndef CONF_USB_COMPOSITE_ISERIALNUM_EN
#define CONF_USB_COMPOSITE_ISERIALNUM_EN 0
#endif

#ifndef CONF_USB_COMPOSITE_ISERIALNUM
#define CONF_USB_COMPOSITE_ISERIALNUM                                                                                  \
	(CONF_USB_COMPOSITE_ISERIALNUM_EN                                                                                  \
	 * (CONF_USB_COMPOSITE_IMANUFACT_EN + CONF_USB_COMPOSITE_IPRODUCT_EN + CONF_USB_COMPOSITE_ISERIALNUM_EN))
#endif

// <s> Unicode string of iSerialNum
// <id> usb_composite_iserialnum_str
#ifndef CONF_USB_COMPOSITE_ISERIALNUM_STR
#define CONF_USB_COMPOSITE_ISERIALNUM_STR "123456789ABCDEF"
#endif

#ifndef CONF_USB_COMPOSITE_ISERIALNUM_STR_DESC
#define CONF_USB_COMPOSITE_ISERIALNUM_STR_DESC
#endif

// </e>

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
// <e> Enable string descriptor of iConfig
// <id> usb_composite_iconfig_en
#ifndef CONF_USB_COMPOSITE_ICONFIG_EN
#define CONF_USB_COMPOSITE_ICONFIG_EN 0
#endif

#ifndef CONF_USB_COMPOSITE_ICONFIG
#define CONF_USB_COMPOSITE_ICONFIG                                                                                     \
	(CONF_USB_COMPOSITE_ICONFIG_EN                                                                                     \
	 * (CONF_USB_COMPOSITE_IMANUFACT_EN + CONF_USB_COMPOSITE_IPRODUCT_EN + CONF_USB_COMPOSITE_ISERIALNUM_EN            \
	    + CONF_USB_COMPOSITE_ICONFIG_EN))
#endif

// <s> Unicode string of iConfig
// <id> usb_composite_iconfig_str
#ifndef CONF_USB_COMPOSITE_ICONFIG_STR
#define CONF_USB_COMPOSITE_ICONFIG_STR ""
#endif

#ifndef CONF_USB_COMPOSITE_ICONFIG_STR_DESC
#define CONF_USB_COMPOSITE_ICONFIG_STR_DESC
#endif

// </e>

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
#define CONF_USB_COMPOSITE_CDC_ACM_EN 0
#endif

// <o> CDC ACM Comm Interrupt IN Endpoint Address
// <0x81=> EndpointAddress = 0x81
// <0x82=> EndpointAddress = 0x82
// <0x83=> EndpointAddress = 0x83
// <0x84=> EndpointAddress = 0x84
// <0x85=> EndpointAddress = 0x85
// <0x86=> EndpointAddress = 0x86
// <0x87=> EndpointAddress = 0x87
// <0x88=> EndpointAddress = 0x88
// <0x89=> EndpointAddress = 0x89

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
// <0x88=> EndpointAddress = 0x88
// <0x89=> EndpointAddress = 0x89

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

// <o> CDC ACM Data BULK IN Endpoint wMaxPacketSize for High Speed
// <0x0008=> 8 bytes
// <0x0010=> 16 bytes
// <0x0020=> 32 bytes
// <0x0040=> 64 bytes
// <0x0080=> 128 bytes
// <0x0100=> 256 bytes
// <0x0200=> 512 bytes

// <id> usb_composite_cdc_acm_data_builin_maxpksz_hs
#ifndef CONF_USB_COMPOSITE_CDC_ACM_DATA_BULKIN_MAXPKSZ_HS
#define CONF_USB_COMPOSITE_CDC_ACM_DATA_BULKIN_MAXPKSZ_HS 0x200
#endif

// <o> CDC ACM Data BULK OUT Endpoint Address
// <0x01=> EndpointAddress = 0x01
// <0x02=> EndpointAddress = 0x02
// <0x03=> EndpointAddress = 0x03
// <0x04=> EndpointAddress = 0x04
// <0x05=> EndpointAddress = 0x05
// <0x06=> EndpointAddress = 0x06
// <0x07=> EndpointAddress = 0x07
// <0x08=> EndpointAddress = 0x08
// <0x09=> EndpointAddress = 0x09

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

// <o> CDC ACM Data BULK OUT Endpoint wMaxPacketSize for High Speed
// <0x0008=> 8 bytes
// <0x0010=> 16 bytes
// <0x0020=> 32 bytes
// <0x0040=> 64 bytes
// <0x0080=> 128 bytes
// <0x0100=> 256 bytes
// <0x0200=> 512 bytes

// <id> usb_composite_cdc_acm_data_buckout_maxpksz_hs
#ifndef CONF_USB_COMPOSITE_CDC_ACM_DATA_BULKOUT_MAXPKSZ_HS
#define CONF_USB_COMPOSITE_CDC_ACM_DATA_BULKOUT_MAXPKSZ_HS 0x200
#endif

// <q> CDC ACM Echo Demo generation
// <id> conf_usb_composite_cdc_echo_demo
// <i> Invoke cdcdf_acm_demo_init(buf[wMaxPacketSize]) to enable the echo demo.
// <i> Buf is packet buffer for data receive and echo back.
// <i> The buffer is 4 byte aligned to support DMA.
#ifndef CONF_USB_COMPOSITE_CDC_ECHO_DEMO
#define CONF_USB_COMPOSITE_CDC_ECHO_DEMO 0
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
// <0x88=> EndpointAddress = 0x88
// <0x89=> EndpointAddress = 0x89

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

// <q> HID Mouse Move Demo generation
// <id> conf_usb_composite_hid_mouse_demo
// <i> Invoke hiddf_demo_init(button1, button2, button3) to enabled the move demo.
// <i> Button1 and button3 are the pins used for mouse moving left and right.
#ifndef CONF_USB_COMPOSITE_HID_MOUSE_DEMO
#define CONF_USB_COMPOSITE_HID_MOUSE_DEMO 0
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
// <0x88=> EndpointAddress = 0x88
// <0x89=> EndpointAddress = 0x89

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
// <0x08=> EndpointAddress = 0x08
// <0x09=> EndpointAddress = 0x09

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

// <q> HID Keyboard Caps Lock Demo generation
// <id> conf_usb_composite_hid_keyboard_demo
// <i> Invoke hiddf_demo_init(button1, button2, button3) to enabled the move demo.
// <i> Buffon2 is the pin used for keyboard CAPS LOCK simulation.
#ifndef CONF_USB_COMPOSITE_HID_KEYBOARD_DEMO
#define CONF_USB_COMPOSITE_HID_KEYBOARD_DEMO 0
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
// <0x88=> EndpointAddress = 0x88
// <0x89=> EndpointAddress = 0x89

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
// <0x08=> EndpointAddress = 0x08
// <0x09=> EndpointAddress = 0x09

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

// <e> MSC Support
// <id> usb_composite_msc_support
#ifndef CONF_USB_COMPOSITE_MSC_EN
#define CONF_USB_COMPOSITE_MSC_EN 0
#endif

// <o> MSC BULK Endpoints wMaxPacketSize
// <0x0008=> 8 bytes
// <0x0010=> 16 bytes
// <0x0020=> 32 bytes
// <0x0040=> 64 bytes

// <id> usb_composite_msc_bulk_maxpksz
#ifndef CONF_USB_COMPOSITE_MSC_BULK_MAXPKSZ
#define CONF_USB_COMPOSITE_MSC_BULK_MAXPKSZ 0x40
#endif

// <o> MSC BULK Endpoints wMaxPacketSize for High Speed
// <0x0008=> 8 bytes
// <0x0010=> 16 bytes
// <0x0020=> 32 bytes
// <0x0040=> 64 bytes
// <0x0080=> 128 bytes
// <0x0100=> 256 bytes
// <0x0200=> 512 bytes

// <id> usb_composite_msc_bulk_maxpksz_hs
#ifndef CONF_USB_COMPOSITE_MSC_BULK_MAXPKSZ_HS
#define CONF_USB_COMPOSITE_MSC_BULK_MAXPKSZ_HS 0x200
#endif

// <o> MSC BULK IN Endpoint Address
// <0x81=> EndpointAddress = 0x81
// <0x82=> EndpointAddress = 0x82
// <0x83=> EndpointAddress = 0x83
// <0x84=> EndpointAddress = 0x84
// <0x85=> EndpointAddress = 0x85
// <0x86=> EndpointAddress = 0x86
// <0x87=> EndpointAddress = 0x87
// <0x88=> EndpointAddress = 0x88
// <0x89=> EndpointAddress = 0x89

// <id> usb_composite_msc_bulkin_epaddr
#ifndef CONF_USB_COMPOSITE_MSC_BULKIN_EPADDR
#define CONF_USB_COMPOSITE_MSC_BULKIN_EPADDR 0x86
#endif

// <o> MSC BULK OUT Endpoint Address
// <0x01=> EndpointAddress = 0x01
// <0x02=> EndpointAddress = 0x02
// <0x03=> EndpointAddress = 0x03
// <0x04=> EndpointAddress = 0x04
// <0x05=> EndpointAddress = 0x05
// <0x06=> EndpointAddress = 0x06
// <0x07=> EndpointAddress = 0x07
// <0x08=> EndpointAddress = 0x08
// <0x09=> EndpointAddress = 0x09

// <id> usb_composite_msc_bulkout_epaddr
#ifndef CONF_USB_COMPOSITE_MSC_BULKOUT_EPADDR
#define CONF_USB_COMPOSITE_MSC_BULKOUT_EPADDR 0x4
#endif

// <e> Enable Demo code for Disk LUN handling
// <id> usb_composite_msc_demo_en
#ifndef CONF_USB_COMPOSITE_MSC_LUN_DEMO
#define CONF_USB_COMPOSITE_MSC_LUN_DEMO 1
#endif

// <o> Disk access cache/buffer of sectors if non-RAM disk (e.g., SD/MMC) enabled <1-64>
// <id> conf_usb_msc_lun_buf_sectors
#ifndef CONF_USB_MSC_LUN_BUF_SECTORS
#define CONF_USB_MSC_LUN_BUF_SECTORS 4
#endif

// <e> Enable Demo for RAM Disk
// <id> conf_usb_msc_lun0_enable
#ifndef CONF_USB_MSC_LUN0_ENABLE
#define CONF_USB_MSC_LUN0_ENABLE 1
#endif

#ifndef CONF_USB_MSC_LUN0_TYPE
#define CONF_USB_MSC_LUN0_TYPE 0x00
#endif

// <q> The disk is removable
// <id> conf_usb_msc_lun0_rmb
#ifndef CONF_USB_MSC_LUN0_RMB
#define CONF_USB_MSC_LUN0_RMB 0x1
#endif

#ifndef CONF_USB_MSC_LUN0_ISO
#define CONF_USB_MSC_LUN0_ISO 0x00
#endif

#ifndef CONF_USB_MSC_LUN0_ECMA
#define CONF_USB_MSC_LUN0_ECMA 0x00
#endif

#ifndef CONF_USB_MSC_LUN0_ANSI
#define CONF_USB_MSC_LUN0_ANSI 0x00
#endif

#ifndef CONF_USB_MSC_LUN0_REPO
#define CONF_USB_MSC_LUN0_REPO 0x01
#endif

#ifndef CONF_USB_MSC_LUN0_FACTORY
#define CONF_USB_MSC_LUN0_FACTORY 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
#endif

#ifndef CONF_USB_MSC_LUN0_PRODUCT
#define CONF_USB_MSC_LUN0_PRODUCT 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
#endif

#ifndef CONF_USB_MSC_LUN0_PRODUCT_VERSION
#define CONF_USB_MSC_LUN0_PRODUCT_VERSION 0x00, 0x00, 0x00, 0x00
#endif

// <o> Disk Size (in KB) <0x1-0xFFFFFFFF>
// <i> Windows will not show disk less than 20K, so 22K is used to reserve more RAM for APP
// <id> conf_usb_msc_lun0_capacity

#ifndef CONF_USB_MSC_LUN0_CAPACITY
#define CONF_USB_MSC_LUN0_CAPACITY 22
#endif

#ifndef CONF_USB_MSC_LUN0_BLOCK_SIZE
#define CONF_USB_MSC_LUN0_BLOCK_SIZE 512
#endif

#ifndef CONF_USB_MSC_LUN0_LAST_BLOCK_ADDR
#define CONF_USB_MSC_LUN0_LAST_BLOCK_ADDR                                                                              \
	((uint32_t)CONF_USB_MSC_LUN0_CAPACITY * 1024 / CONF_USB_MSC_LUN0_BLOCK_SIZE - 1)
#endif

// </e>

// <e> Enable Demo for SD/MMC Disk
// <i> SD/MMC stack must be added before enable SD/MMC demo
// <i> SD/MMC insert/eject not supported by this simple demo
// <id> conf_usb_msc_lun1_enable
#ifndef CONF_USB_MSC_LUN1_ENABLE
#define CONF_USB_MSC_LUN1_ENABLE 0
#endif

#ifndef CONF_USB_MSC_LUN1_TYPE
#define CONF_USB_MSC_LUN1_TYPE 0x00
#endif

// <q> The disk is removable
// <i> SD/MMC stack must be added before enable SD/MMC demo
// <i> SD/MMC insert/eject not supported by this simple demo
// <id> conf_usb_msc_lun1_rmb
#ifndef CONF_USB_MSC_LUN1_RMB
#define CONF_USB_MSC_LUN1_RMB 0x1
#endif

#ifndef CONF_USB_MSC_LUN1_ISO
#define CONF_USB_MSC_LUN1_ISO 0x00
#endif

#ifndef CONF_USB_MSC_LUN1_ECMA
#define CONF_USB_MSC_LUN1_ECMA 0x00
#endif

#ifndef CONF_USB_MSC_LUN1_ANSI
#define CONF_USB_MSC_LUN1_ANSI 0x00
#endif

#ifndef CONF_USB_MSC_LUN1_REPO
#define CONF_USB_MSC_LUN1_REPO 0x01
#endif

#ifndef CONF_USB_MSC_LUN1_FACTORY
#define CONF_USB_MSC_LUN1_FACTORY 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
#endif

#ifndef CONF_USB_MSC_LUN1_PRODUCT
#define CONF_USB_MSC_LUN1_PRODUCT 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
#endif

#ifndef CONF_USB_MSC_LUN1_PRODUCT_VERSION
#define CONF_USB_MSC_LUN1_PRODUCT_VERSION 0x00, 0x00, 0x00, 0x00
#endif

#ifndef CONF_USB_MSC_LUN1_CAPACITY
#define CONF_USB_MSC_LUN1_CAPACITY 22
#endif

#ifndef CONF_USB_MSC_LUN1_BLOCK_SIZE
#define CONF_USB_MSC_LUN1_BLOCK_SIZE 512
#endif

#ifndef CONF_USB_MSC_LUN1_LAST_BLOCK_ADDR
#define CONF_USB_MSC_LUN1_LAST_BLOCK_ADDR                                                                              \
	((uint32_t)CONF_USB_MSC_LUN1_CAPACITY * 1024 / CONF_USB_MSC_LUN1_BLOCK_SIZE - 1)
#endif

// </e>

// <e> Enable Demo for LUN 2
// <id> conf_usb_msc_lun2_enable
#ifndef CONF_USB_MSC_LUN2_ENABLE
#define CONF_USB_MSC_LUN2_ENABLE 0
#endif

#ifndef CONF_USB_MSC_LUN2_TYPE
#define CONF_USB_MSC_LUN2_TYPE 0x00
#endif

// <q> The disk is removable
// <id> conf_usb_msc_lun2_rmb
#ifndef CONF_USB_MSC_LUN2_RMB
#define CONF_USB_MSC_LUN2_RMB 0x1
#endif

#ifndef CONF_USB_MSC_LUN2_ISO
#define CONF_USB_MSC_LUN2_ISO 0x00
#endif

#ifndef CONF_USB_MSC_LUN2_ECMA
#define CONF_USB_MSC_LUN2_ECMA 0x00
#endif

#ifndef CONF_USB_MSC_LUN2_ANSI
#define CONF_USB_MSC_LUN2_ANSI 0x00
#endif

#ifndef CONF_USB_MSC_LUN2_REPO
#define CONF_USB_MSC_LUN2_REPO 0x01
#endif

#ifndef CONF_USB_MSC_LUN2_FACTORY
#define CONF_USB_MSC_LUN2_FACTORY 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
#endif

#ifndef CONF_USB_MSC_LUN2_PRODUCT
#define CONF_USB_MSC_LUN2_PRODUCT 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
#endif

#ifndef CONF_USB_MSC_LUN2_PRODUCT_VERSION
#define CONF_USB_MSC_LUN2_PRODUCT_VERSION 0x00, 0x00, 0x00, 0x00
#endif

// <o> Disk Size (in KB) <0x1-0xFFFFFFFF>
// <id> conf_usb_msc_lun2_capacity

#ifndef CONF_USB_MSC_LUN2_CAPACITY
#define CONF_USB_MSC_LUN2_CAPACITY 22
#endif

#ifndef CONF_USB_MSC_LUN2_BLOCK_SIZE
#define CONF_USB_MSC_LUN2_BLOCK_SIZE 512
#endif

#ifndef CONF_USB_MSC_LUN2_LAST_BLOCK_ADDR
#define CONF_USB_MSC_LUN2_LAST_BLOCK_ADDR                                                                              \
	((uint32_t)CONF_USB_MSC_LUN2_CAPACITY * 1024 / CONF_USB_MSC_LUN2_BLOCK_SIZE - 1)
#endif

// </e>

// <e> Enable Demo for LUN 3
// <id> conf_usb_msc_lun3_enable
#ifndef CONF_USB_MSC_LUN3_ENABLE
#define CONF_USB_MSC_LUN3_ENABLE 0
#endif

#ifndef CONF_USB_MSC_LUN3_TYPE
#define CONF_USB_MSC_LUN3_TYPE 0x00
#endif

// <q> The disk is removable
// <id> conf_usb_msc_lun3_rmb
#ifndef CONF_USB_MSC_LUN3_RMB
#define CONF_USB_MSC_LUN3_RMB 0x1
#endif

#ifndef CONF_USB_MSC_LUN3_ISO
#define CONF_USB_MSC_LUN3_ISO 0x00
#endif

#ifndef CONF_USB_MSC_LUN3_ECMA
#define CONF_USB_MSC_LUN3_ECMA 0x00
#endif

#ifndef CONF_USB_MSC_LUN3_ANSI
#define CONF_USB_MSC_LUN3_ANSI 0x00
#endif

#ifndef CONF_USB_MSC_LUN3_REPO
#define CONF_USB_MSC_LUN3_REPO 0x01
#endif

#ifndef CONF_USB_MSC_LUN3_FACTORY
#define CONF_USB_MSC_LUN3_FACTORY 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
#endif

#ifndef CONF_USB_MSC_LUN3_PRODUCT
#define CONF_USB_MSC_LUN3_PRODUCT 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
#endif

#ifndef CONF_USB_MSC_LUN3_PRODUCT_VERSION
#define CONF_USB_MSC_LUN3_PRODUCT_VERSION 0x00, 0x00, 0x00, 0x00
#endif

// <o> Disk Size (in KB) <0x1-0xFFFFFFFF>
// <id> conf_usb_msc_lun3_capacity

#ifndef CONF_USB_MSC_LUN3_CAPACITY
#define CONF_USB_MSC_LUN3_CAPACITY 22
#endif

#ifndef CONF_USB_MSC_LUN3_BLOCK_SIZE
#define CONF_USB_MSC_LUN3_BLOCK_SIZE 512
#endif

#ifndef CONF_USB_MSC_LUN3_LAST_BLOCK_ADDR
#define CONF_USB_MSC_LUN3_LAST_BLOCK_ADDR                                                                              \
	((uint32_t)CONF_USB_MSC_LUN3_CAPACITY * 1024 / CONF_USB_MSC_LUN3_BLOCK_SIZE - 1)
#endif

// </e>

// </e>
// </e>

// <<< end of configuration section >>>

#endif // USBD_CONFIG_H
