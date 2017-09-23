/**
 * \file
 *
 * \brief USB Human Interface Device (HID) protocol definitions.
 *
 * Copyright (c) 2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#ifndef _USB_PROTOCOL_HID_H_
#define _USB_PROTOCOL_HID_H_

#include "usb_includes.h"

/**
 * \ingroup usb_protocol_group
 * \defgroup usb_hid_protocol USB Human Interface Device (HID)
 * protocol definitions
 * \brief USB Human Interface Device (HID) protocol definitions
 *
 * @{
 */

//! \name Possible Class value
//@{
#define HID_CLASS 0x03
//@}

//! \name Possible SubClass value
//@{
//! Interface subclass NO support BOOT protocol
#define HID_SUB_CLASS_NOBOOT 0x00
//! Interface subclass support BOOT protocol
#define HID_SUB_CLASS_BOOT 0x01
//@}

//! \name Possible protocol value
//@{
//! Protocol generic standard
#define HID_PROTOCOL_GENERIC 0x00
//! Protocol keyboard standard
#define HID_PROTOCOL_KEYBOARD 0x01
//! Protocol mouse standard
#define HID_PROTOCOL_MOUSE 0x02
//@}

//! \brief Hid USB requests (bRequest)

#define USB_REQ_HID_GET_REPORT 0x01
#define USB_REQ_HID_GET_IDLE 0x02
#define USB_REQ_HID_GET_PROTOCOL 0x03
#define USB_REQ_HID_SET_REPORT 0x09
#define USB_REQ_HID_SET_IDLE 0x0A
#define USB_REQ_HID_SET_PROTOCOL 0x0B

//! \brief HID USB descriptor types
#define USB_DT_HID 0x21
#define USB_DT_HID_REPORT 0x22
#define USB_DT_HID_PHYSICAL 0x23

//! \brief HID Type for report descriptor
#define USB_HID_ITEM_REPORT_TYPE_MAIN 0
#define USB_HID_ITEM_REPORT_TYPE_GLOBAL 1
#define USB_HID_ITEM_REPORT_TYPE_LOCAL 2
#define USB_HID_ITEM_REPORT_TYPE_LONG 3

//! \brief HID report type
#define USB_HID_REPORT_TYPE_INPUT 1
#define USB_HID_REPORT_TYPE_OUTPUT 2
#define USB_HID_REPORT_TYPE_FEATURE 3

//! \brief HID protocol
#define USB_HID_PROCOTOL_BOOT 0
#define USB_HID_PROCOTOL_REPORT 1

#define USB_HID_DESC_BYTES(bLength, bCountryCode, bNumDescriptors, bDescriptorType, bDescriptorLength)                 \
	bLength, 0x21, 0x10, 0x01, bCountryCode, bNumDescriptors, bDescriptorType, LE_BYTE0(bDescriptorLength),            \
	    LE_BYTE1(bDescriptorLength)

COMPILER_PACK_SET(1)

//! \brief HID Descriptor
typedef struct usb_hid_descriptor {
	uint8_t bLength;           //!< Size of this descriptor in bytes
	uint8_t bDescriptorType;   //!< HID descriptor type
	le16_t  bcdHID;            //!< Binary Coded Decimal Spec. release
	uint8_t bCountryCode;      //!< Hardware target country
	uint8_t bNumDescriptors;   //!< Number of HID class descriptors to follow
	uint8_t bRDescriptorType;  //!< Report descriptor type
	le16_t  wDescriptorLength; //!< Total length of Report descriptor
} usb_hid_descriptor_t;

COMPILER_PACK_RESET()

//! \name HID Report type
//! Used by SETUP_HID_GET_REPORT & SETUP_HID_SET_REPORT
//! @{
#define REPORT_TYPE_INPUT 0x01
#define REPORT_TYPE_OUTPUT 0x02
#define REPORT_TYPE_FEATURE 0x03
//! @}

//! \name Constants of field DESCRIPTOR_HID
//! @{
//! Numeric expression identifying the HID Class
//! Specification release (here V1.11)
#define USB_HID_BDC_V1_11 0x0111
//! Numeric expression specifying the number of class descriptors
//! Note: Always at least one i.e. Report descriptor.
#define USB_HID_NUM_DESC 0x01

//! \name Country code
//! @{
#define USB_HID_NO_COUNTRY_CODE 0            // Not Supported
#define USB_HID_COUNTRY_ARABIC 1             // Arabic
#define USB_HID_COUNTRY_BELGIAN 2            // Belgian
#define USB_HID_COUNTRY_CANADIAN_BILINGUAL 3 // Canadian-Bilingual
#define USB_HID_COUNTRY_CANADIAN_FRENCH 4    // Canadian-French
#define USB_HID_COUNTRY_CZECH_REPUBLIC 5     // Czech Republic
#define USB_HID_COUNTRY_DANISH 6             // Danish
#define USB_HID_COUNTRY_FINNISH 7            // Finnish
#define USB_HID_COUNTRY_FRENCH 8             // French
#define USB_HID_COUNTRY_GERMAN 9             // German
#define USB_HID_COUNTRY_GREEK 10             // Greek
#define USB_HID_COUNTRY_HEBREW 11            // Hebrew
#define USB_HID_COUNTRY_HUNGARY 12           // Hungary
#define USB_HID_COUNTRY_INTERNATIONAL_ISO 13 // International (ISO)
#define USB_HID_COUNTRY_ITALIAN 14           // Italian
#define USB_HID_COUNTRY_JAPAN_KATAKANA 15    // Japan (Katakana)
#define USB_HID_COUNTRY_KOREAN 16            // Korean
#define USB_HID_COUNTRY_LATIN_AMERICAN 17    // Latin American
#define USB_HID_COUNTRY_NETHERLANDS_DUTCH 18 // Netherlands/Dutch
#define USB_HID_COUNTRY_NORWEGIAN 19         // Norwegian
#define USB_HID_COUNTRY_PERSIAN_FARSI 20     // Persian (Farsi)
#define USB_HID_COUNTRY_POLAND 21            // Poland
#define USB_HID_COUNTRY_PORTUGUESE 22        // Portuguese
#define USB_HID_COUNTRY_RUSSIA 23            // Russia
#define USB_HID_COUNTRY_SLOVAKIA 24          // Slovakia
#define USB_HID_COUNTRY_SPANISH 25           // Spanish
#define USB_HID_COUNTRY_SWEDISH 26           // Swedish
#define USB_HID_COUNTRY_SWISS_FRENCH 27      // Swiss/French
#define USB_HID_COUNTRY_SWISS_GERMAN 28      // Swiss/German
#define USB_HID_COUNTRY_SWITZERLAND 29       // Switzerland
#define USB_HID_COUNTRY_TAIWAN 30            // Taiwan
#define USB_HID_COUNTRY_TURKISH_Q 31         // Turkish-Q
#define USB_HID_COUNTRY_UK 32                // UK
#define USB_HID_COUNTRY_US 33                // US
#define USB_HID_COUNTRY_YUGOSLAVIA 34        // Yugoslavia
#define USB_HID_COUNTRY_TURKISH_F 35         // Turkish-F
                                             //! @}
                                             //! @}
//! @}

//! \name HID KEYS values
//! @{
#define HID_A 0x04
#define HID_B 0x05
#define HID_C 0x06
#define HID_D 0x07
#define HID_E 0x08
#define HID_F 0x09
#define HID_G 0x0A
#define HID_H 0x0B
#define HID_I 0x0C
#define HID_J 0x0D
#define HID_K 0x0E
#define HID_L 0x0F
#define HID_M 0x10
#define HID_N 0x11
#define HID_O 0x12
#define HID_P 0x13
#define HID_Q 0x14
#define HID_R 0x15
#define HID_S 0x16
#define HID_T 0x17
#define HID_U 0x18
#define HID_V 0x19
#define HID_W 0x1A
#define HID_X 0x1B
#define HID_Y 0x1C
#define HID_Z 0x1D
#define HID_1 30
#define HID_2 31
#define HID_3 32
#define HID_4 33
#define HID_5 34
#define HID_6 35
#define HID_7 36
#define HID_8 37
#define HID_9 38
#define HID_0 39
#define HID_ENTER 40
#define HID_ESCAPE 41
#define HID_BACKSPACE 42
#define HID_TAB 43
#define HID_SPACEBAR 44
#define HID_UNDERSCORE 45
#define HID_PLUS 46
#define HID_OPEN_BRACKET 47  // {
#define HID_CLOSE_BRACKET 48 // }
#define HID_BACKSLASH 49
#define HID_ASH 50   // # ~
#define HID_COLON 51 // ; :
#define HID_QUOTE 52 // ' "
#define HID_TILDE 53
#define HID_COMMA 54
#define HID_DOT 55
#define HID_SLASH 56
#define HID_CAPS_LOCK 57
#define HID_F1 58
#define HID_F2 59
#define HID_F3 60
#define HID_F4 61
#define HID_F5 62
#define HID_F6 63
#define HID_F7 64
#define HID_F8 65
#define HID_F9 66
#define HID_F10 67
#define HID_F11 68
#define HID_F12 69
#define HID_PRINTSCREEN 70
#define HID_SCROLL_LOCK 71
#define HID_PAUSE 72
#define HID_INSERT 73
#define HID_HOME 74
#define HID_PAGEUP 75
#define HID_DELETE 76
#define HID_END 77
#define HID_PAGEDOWN 78
#define HID_RIGHT 79
#define HID_LEFT 80
#define HID_DOWN 81
#define HID_UP 82
#define HID_KEYPAD_NUM_LOCK 83
#define HID_KEYPAD_DIVIDE 84
#define HID_KEYPAD_AT 85
#define HID_KEYPAD_MULTIPLY 85
#define HID_KEYPAD_MINUS 86
#define HID_KEYPAD_PLUS 87
#define HID_KEYPAD_ENTER 88
#define HID_KEYPAD_1 89
#define HID_KEYPAD_2 90
#define HID_KEYPAD_3 91
#define HID_KEYPAD_4 92
#define HID_KEYPAD_5 93
#define HID_KEYPAD_6 94
#define HID_KEYPAD_7 95
#define HID_KEYPAD_8 96
#define HID_KEYPAD_9 97
#define HID_KEYPAD_0 98

//! \name HID modifier values
//! @{
#define HID_MODIFIER_NONE 0x00
#define HID_MODIFIER_LEFT_CTRL 0x01
#define HID_MODIFIER_LEFT_SHIFT 0x02
#define HID_MODIFIER_LEFT_ALT 0x04
#define HID_MODIFIER_LEFT_UI 0x08
#define HID_MODIFIER_RIGHT_CTRL 0x10
#define HID_MODIFIER_RIGHT_SHIFT 0x20
#define HID_MODIFIER_RIGHT_ALT 0x40
#define HID_MODIFIER_RIGHT_UI 0x80
//! @}
//! @}

//! \name HID KEYS LED values
//! @{
#define HID_LED_NUM_LOCK (1 << 0)
#define HID_LED_CAPS_LOCK (1 << 1)
#define HID_LED_SCROLL_LOCK (1 << 2)
#define HID_LED_COMPOSE (1 << 3)
#define HID_LED_KANA (1 << 4)
//! @}

/** \name Standard HID mouse definitions from HID spec. example */
/*@{*/

/** \brief Structure for standard HID keyboard modified keys */
typedef union hid_kbd_modifier {
	uint8_t byte;
	struct {
		uint8_t lctrl : 1;  /**< Left Ctrl */
		uint8_t lshift : 1; /**< Left Shift */
		uint8_t lalt : 1;   /**< Left Alt */
		uint8_t lgui : 1;   /**< Left Gui */
		uint8_t rctrl : 1;  /**< Right Ctrl */
		uint8_t rshift : 1; /**< Right Shift */
		uint8_t ralt : 1;   /**< Right Alt */
		uint8_t rgui : 1;   /**< Right Gui */
	} bm;
} hid_kbd_modifier_t;
#define HID_KBD_L_CTRL (1 << 0)  /**< Left Ctrl */
#define HID_KBD_L_SHIFT (1 << 1) /**< Left Shift */
#define HID_KBD_L_ALT (1 << 2)   /**< Left Alt */
#define HID_KBD_L_GUI (1 << 3)   /**< Left Gui */
#define HID_KBD_R_CTRL (1 << 4)  /**< Right Ctrl */
#define HID_KBD_R_SHIFT (1 << 5) /**< Right Shift */
#define HID_KBD_R_ALT (1 << 6)   /**< Right Alt */
#define HID_KBD_R_GUI (1 << 7)   /**< Right Gui */

/** \brief Structure for standard HID keyboard input report */
typedef union hid_kbd_input_report {
	uint8_t byte[8];
	struct {
		union hid_kbd_modifier modifier;
		uint8_t                reserved_oem;
		uint8_t                key[6]; /** < Key array */
	} field;
} hid_kbd_input_report_t;
/** Access to the modifier keys in HID keyboard report
 *  \param rpt Pointer to report data of bytes
 */
#define hid_kbd_modifier_keys(rpt) (rpt[0])
/** Access to the OEM reserved in HID keyboard report
 *  \param rpt Pointer to report data of bytes
 */
#define hid_kbd_oem(rpt) (rpt[1])
/** Access to the Key code in HID keyboard report
 *  \param rpt Pointer to report data of bytes
 *  \param i   Index in the report key array (0~n)
 */
#define hid_kbd_keycode(rpt, i) (rpt[2 + i])

/** \brief Structure for standard HID keyboard output report */
typedef union hid_kbd_output_report {
	uint8_t byte;
	struct {
		uint8_t num_lock : 1;    /**< LED of Num Lock */
		uint8_t caps_lock : 1;   /**< LED of Caps Lock */
		uint8_t scroll_lock : 1; /**< LED of Scroll Lock */
		uint8_t compose : 1;     /**< LED of COMPOSE LOCK */
		uint8_t kana : 1;        /**< LED of KANA */
		uint8_t constant : 3;
	} field;
} hid_kbd_output_report_t;
#define HID_KBD_NUM_LOCK (1 << 0)    /**< LED of Num Lock */
#define HID_KBD_CAPS_LOCK (1 << 1)   /**< LED of Caps Lock */
#define HID_KBD_SCROLL_LOCK (1 << 2) /**< LED of Scroll Lock */
#define HID_KBD_COMPOSE (1 << 3)     /**< LED of COMPOSE LOCK */
#define HID_KBD_KANA (1 << 4)        /**< LED of KANA */

/*@}*/

/** \name Standard HID mouse definitions from HID spec. example */
/*@{*/
/** \brief Structure for standard HID mouse report */
typedef union hid_mouse_report {
	uint8_t byte[3];
	struct {
		uint8_t btn1 : 1; /**< Button 1 */
		uint8_t btn2 : 2; /**< Button 2 */
		uint8_t btn3 : 3; /**< Button 3 */
		uint8_t device_spec : 5;
		int8_t  x; /**< X displacement */
		int8_t  y; /**< Y displacement */
	} field;
	/* More bytes are optional */
} hid_mouse_report_t;
#define HID_MOUSE_BTN1 (1 << 0) /**< Button 1 */
#define HID_MOUSE_BTN2 (1 << 1) /**< Button 2 */
#define HID_MOUSE_BTN3 (1 << 2) /**< Button 3 */

/** Access to the button state in HID mouse report
 *  \param rpt Pointer to report data of bytes
 */
#define hid_mouse_btn_state(rpt) (rpt[0])
/** Check if the button is done in HID mouse report
 *  \param rpt Pointer to report data of bytes
 *  \param btn Button bitmap to check
 */
#define hid_mouse_is_btn_down(rpt, btn) (rpt[0] & (btn))
/** Access to the relative X displacement in HID mouse report
 *  \param rpt Pointer to report data of bytes
 */
#define hid_mouse_x(rpt) ((int8_t)rpt[1])
/** Access to the relative Y displacement in HID mouse report
 *  \param rpt Pointer to report data of bytes
 */
#define hid_mouse_y(rpt) ((int8_t)rpt[2])

/*@}*/

/** \name Standard HID joystick definitions from HID spec. example */
/*@{*/

/** \brief Structure for standard HID joystick report */
typedef union hid_joystick_report {
	uint8_t byte[4];
	struct {
		int8_t  x;
		int8_t  y;
		uint8_t hat_sw : 4; /**< Hat switch */
		uint8_t btn1 : 1;   /**< Button 1 */
		uint8_t btn2 : 1;   /**< Button 2 */
		uint8_t btn3 : 1;   /**< Button 3 */
		uint8_t btn4 : 1;   /**< Button 4 */
		uint8_t throttle;
	} field;
} hid_joystick_report_t;
#define HID_JOYSTICK_BTN1 (1 << 4) /**< Button 1 */
#define HID_JOYSTICK_BTN2 (1 << 5) /**< Button 2 */
#define HID_JOYSTICK_BTN3 (1 << 6) /**< Button 3 */
#define HID_JOYSTICK_BTN4 (1 << 7) /**< Button 4 */

/** Access to the absolute X position in HID joystick report
 *  \param rpt Pointer to report data of bytes
 */
#define hid_joystick_x(rpt) ((int8_t)rpt[0])
/** Access to the absolute Y position in HID joystick report
 *  \param rpt Pointer to report data of bytes
 */
#define hid_joystick_y(rpt) ((int8_t)rpt[1])
/** Access to the button state in HID joystick report
 *  \param rpt Pointer to report data of bytes
 */
#define hid_joystick_btn_state(rpt) (rpt[2])
/** Check if the button is done in HID joystick report
 *  \param rpt Pointer to report data of bytes
 *  \param btn Button bitmap to check
 */
#define hid_joystick_is_btn_down(rpt, btn) (rpt[2] & (btn))
/** Get the Hat switch in HID joystick report
 *  \param rpt Pointer to report data of bytes
 */
#define hid_joystick_hat_sw(rpt) (rpt[2] & 0xF)
/** Access to the throttle in HID joystick report
 *  \param rpt Pointer to report data of bytes
 */
#define hid_joystick_throttle(rpt) (rpt[3])

/*@}*/

/**
 * \brief Fill a GetHIDDescriptor request
 * \param[out] req   Pointer to the request to fill
 * \param[in]  type  Descriptor type
 * \param[in]  index Descriptor index
 * \param[in]  iface Interface Number
 * \param[in]  len   Descriptor Length
 */
static inline void usb_fill_GetHIDDesc_req(struct usb_req *req, uint8_t type, uint8_t index, uint8_t iface,
                                           uint16_t len)
{
	req->bmRequestType = 0x81;
	req->bRequest      = USB_REQ_GET_DESC;
	req->wValue        = (type << 8) | index;
	req->wIndex        = iface;
	req->wLength       = len;
}

/**
 * \brief Fill a SetHIDDescriptor request
 * \param[out] req   Pointer to the request to fill
 * \param[in]  type  Descriptor type
 * \param[in]  index Descriptor index
 * \param[in]  iface Interface Number
 * \param[in]  len   Descriptor Length
 */
static inline void usb_fill_SetHIDDesc_req(struct usb_req *req, uint8_t type, uint8_t index, uint8_t iface,
                                           uint16_t len)
{
	req->bmRequestType = 0x01;
	req->bRequest      = USB_REQ_SET_DESC;
	req->wValue        = (type << 8) | index;
	req->wIndex        = iface;
	req->wLength       = len;
}

/**
 * \brief Fill a GetReport request
 * \param[out] req   Pointer to the request to fill
 * \param[in]  type  Report type
 * \param[in]  id    Report ID
 * \param[in]  iface Interface Number
 * \param[in]  len   Report Length
 */
static inline void usb_fill_GetReport_req(struct usb_req *req, uint8_t type, uint8_t id, uint8_t iface, uint16_t len)
{
	req->bmRequestType = 0xA1;
	req->bRequest      = USB_REQ_HID_GET_REPORT;
	req->wValue        = (type << 8) | id;
	req->wIndex        = iface;
	req->wLength       = len;
}

/**
 * \brief Fill a SetReport request
 * \param[out] req   Pointer to the request to fill
 * \param[in]  type  Report type
 * \param[in]  id    Report ID
 * \param[in]  iface Interface Number
 * \param[in]  len   Report Length
 */
static inline void usb_fill_SetReport_req(struct usb_req *req, uint8_t type, uint8_t id, uint8_t iface, uint16_t len)
{
	req->bmRequestType = 0x21;
	req->bRequest      = USB_REQ_HID_GET_REPORT;
	req->wValue        = (type << 8) | id;
	req->wIndex        = iface;
	req->wLength       = len;
}

/**
 * \brief Fill a GetIdle request
 * \param[out] req   Pointer to the request to fill
 * \param[in]  id    Report ID
 * \param[in]  iface Interface Number
 */
static inline void usb_fill_GetIdle_req(struct usb_req *req, uint8_t id, uint8_t iface)
{
	req->bmRequestType = 0xA1;
	req->bRequest      = USB_REQ_HID_GET_IDLE;
	req->wValue        = id;
	req->wIndex        = iface;
	req->wLength       = 1;
}

/**
 * \brief Fill a SetIdle request
 * \param[out] req      Pointer to the request to fill
 * \param[in]  duration Duration value
 * \param[in]  id       Report ID
 * \param[in]  iface    Interface Number
 */
static inline void usb_fill_SetIdle_req(struct usb_req *req, uint8_t duration, uint8_t id, uint8_t iface)
{
	req->bmRequestType = 0x21;
	req->bRequest      = USB_REQ_HID_SET_IDLE;
	req->wValue        = (duration << 8) | id;
	req->wIndex        = iface;
	req->wLength       = 0;
}

/**
 * \brief Fill a GetProtocol request
 * \param[out] req   Pointer to the request to fill
 * \param[in]  iface Interface Number
 */
static inline void usb_fill_GetProtocol_req(struct usb_req *req, uint8_t iface)
{
	req->bmRequestType = 0xA1;
	req->bRequest      = USB_REQ_HID_GET_PROTOCOL;
	req->wValue        = 0;
	req->wIndex        = iface;
	req->wLength       = 1;
}

/**
 * \brief Fill a SetProtocol request
 * \param[out] req   Pointer to the request to fill
 * \param[in]  iface Interface Number
 */
static inline void usb_fill_SetProtocol_req(struct usb_req *req, uint8_t protocol, uint8_t iface)
{
	req->bmRequestType = 0x21;
	req->bRequest      = USB_REQ_HID_SET_PROTOCOL;
	req->wValue        = protocol;
	req->wIndex        = iface;
	req->wLength       = 0;
}

#endif // _USB_PROTOCOL_HID_H_
