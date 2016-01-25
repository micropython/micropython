/***********************************************************************
* $Id: mw_usbd_hid.h.rca 1.2 Tue Nov  1 11:45:07 2011 nlv09221 Experimental $
*
* Project: USB device ROM Stack
*
* Description:
*     HID Definitions.
*
***********************************************************************
*   Copyright(C) 2011, NXP Semiconductor
*   All rights reserved.
*
* Software that is described herein is for illustrative purposes only
* which provides customers with programming information regarding the
* products. This software is supplied "AS IS" without any warranties.
* NXP Semiconductors assumes no responsibility or liability for the
* use of the software, conveys no license or title under any patent,
* copyright, or mask work right to the product. NXP Semiconductors
* reserves the right to make changes in the software without
* notification. NXP Semiconductors also make no representation or
* warranty that such application will be suitable for the specified
* use without further testing or modification.
**********************************************************************/
#ifndef __HID_H__
#define __HID_H__

#include "usbd.h"

/** \file
 *  \brief Common definitions and declarations for the library USB HID Class driver.
 *
 *  Common definitions and declarations for the library USB HID Class driver.
 *  \addtogroup USBD_HID 
 *  @{
 */


/** HID Subclass Codes  
 * @{ 
 */
/** Descriptor Subclass value indicating that the device or interface does not implement a HID boot protocol. */
#define HID_SUBCLASS_NONE               0x00
/** Descriptor Subclass value indicating that the device or interface implements a HID boot protocol. */
#define HID_SUBCLASS_BOOT               0x01
/** @} */

/** HID Protocol Codes 
 * @{ 
 */
/** Descriptor Protocol value indicating that the device or interface does not belong to a HID boot protocol. */
#define HID_PROTOCOL_NONE               0x00
/** Descriptor Protocol value indicating that the device or interface belongs to the Keyboard HID boot protocol. */
#define HID_PROTOCOL_KEYBOARD           0x01
/** Descriptor Protocol value indicating that the device or interface belongs to the Mouse HID boot protocol. */
#define HID_PROTOCOL_MOUSE              0x02
/** @} */



/** Descriptor Types  
 * @{ 
 */
/** Descriptor header type value, to indicate a HID class HID descriptor. */
#define HID_HID_DESCRIPTOR_TYPE         0x21
/** Descriptor header type value, to indicate a HID class HID report descriptor. */
#define HID_REPORT_DESCRIPTOR_TYPE      0x22
/** Descriptor header type value, to indicate a HID class HID Physical descriptor. */
#define HID_PHYSICAL_DESCRIPTOR_TYPE    0x23
/** @} */


/** \brief HID class-specific HID Descriptor.
 *
 *  Type define for the HID class-specific HID descriptor, to describe the HID device's specifications. Refer to the HID
 *  specification for details on the structure elements.
 *
 */
PRE_PACK struct POST_PACK _HID_DESCRIPTOR {
  uint8_t  bLength;	/**< Size of the descriptor, in bytes. */
  uint8_t  bDescriptorType;	/**< Type of HID descriptor. */
  uint16_t bcdHID; /**< BCD encoded version that the HID descriptor and device complies to. */
  uint8_t  bCountryCode; /**< Country code of the localized device, or zero if universal. */
  uint8_t  bNumDescriptors; /**< Total number of HID report descriptors for the interface. */
  
  PRE_PACK struct POST_PACK _HID_DESCRIPTOR_LIST {
    uint8_t  bDescriptorType; /**< Type of HID report. */
    uint16_t wDescriptorLength; /**< Length of the associated HID report descriptor, in bytes. */
  } DescriptorList[1]; /**< Array of one or more descriptors */
} ;
/** HID class-specific HID Descriptor. */
typedef struct _HID_DESCRIPTOR HID_DESCRIPTOR;

#define HID_DESC_SIZE   sizeof(HID_DESCRIPTOR)

/** HID Request Codes  
 * @{ 
 */
#define HID_REQUEST_GET_REPORT          0x01
#define HID_REQUEST_GET_IDLE            0x02
#define HID_REQUEST_GET_PROTOCOL        0x03
#define HID_REQUEST_SET_REPORT          0x09
#define HID_REQUEST_SET_IDLE            0x0A
#define HID_REQUEST_SET_PROTOCOL        0x0B
/** @} */

/** HID Report Types  
 * @{ 
 */
#define HID_REPORT_INPUT                0x01
#define HID_REPORT_OUTPUT               0x02
#define HID_REPORT_FEATURE              0x03
/** @} */


/** Usage Pages  
 * @{ 
 */
#define HID_USAGE_PAGE_UNDEFINED        0x00
#define HID_USAGE_PAGE_GENERIC          0x01
#define HID_USAGE_PAGE_SIMULATION       0x02
#define HID_USAGE_PAGE_VR               0x03
#define HID_USAGE_PAGE_SPORT            0x04
#define HID_USAGE_PAGE_GAME             0x05
#define HID_USAGE_PAGE_DEV_CONTROLS     0x06
#define HID_USAGE_PAGE_KEYBOARD         0x07
#define HID_USAGE_PAGE_LED              0x08
#define HID_USAGE_PAGE_BUTTON           0x09
#define HID_USAGE_PAGE_ORDINAL          0x0A
#define HID_USAGE_PAGE_TELEPHONY        0x0B
#define HID_USAGE_PAGE_CONSUMER         0x0C
#define HID_USAGE_PAGE_DIGITIZER        0x0D
#define HID_USAGE_PAGE_UNICODE          0x10
#define HID_USAGE_PAGE_ALPHANUMERIC     0x14
/** @} */


/** Generic Desktop Page (0x01)  
 * @{ 
 */
#define HID_USAGE_GENERIC_POINTER               0x01
#define HID_USAGE_GENERIC_MOUSE                 0x02
#define HID_USAGE_GENERIC_JOYSTICK              0x04
#define HID_USAGE_GENERIC_GAMEPAD               0x05
#define HID_USAGE_GENERIC_KEYBOARD              0x06
#define HID_USAGE_GENERIC_KEYPAD                0x07
#define HID_USAGE_GENERIC_X                     0x30
#define HID_USAGE_GENERIC_Y                     0x31
#define HID_USAGE_GENERIC_Z                     0x32
#define HID_USAGE_GENERIC_RX                    0x33
#define HID_USAGE_GENERIC_RY                    0x34
#define HID_USAGE_GENERIC_RZ                    0x35
#define HID_USAGE_GENERIC_SLIDER                0x36
#define HID_USAGE_GENERIC_DIAL                  0x37
#define HID_USAGE_GENERIC_WHEEL                 0x38
#define HID_USAGE_GENERIC_HATSWITCH             0x39
#define HID_USAGE_GENERIC_COUNTED_BUFFER        0x3A
#define HID_USAGE_GENERIC_BYTE_COUNT            0x3B
#define HID_USAGE_GENERIC_MOTION_WAKEUP         0x3C
#define HID_USAGE_GENERIC_VX                    0x40
#define HID_USAGE_GENERIC_VY                    0x41
#define HID_USAGE_GENERIC_VZ                    0x42
#define HID_USAGE_GENERIC_VBRX                  0x43
#define HID_USAGE_GENERIC_VBRY                  0x44
#define HID_USAGE_GENERIC_VBRZ                  0x45
#define HID_USAGE_GENERIC_VNO                   0x46
#define HID_USAGE_GENERIC_SYSTEM_CTL            0x80
#define HID_USAGE_GENERIC_SYSCTL_POWER          0x81
#define HID_USAGE_GENERIC_SYSCTL_SLEEP          0x82
#define HID_USAGE_GENERIC_SYSCTL_WAKE           0x83
#define HID_USAGE_GENERIC_SYSCTL_CONTEXT_MENU   0x84
#define HID_USAGE_GENERIC_SYSCTL_MAIN_MENU      0x85
#define HID_USAGE_GENERIC_SYSCTL_APP_MENU       0x86
#define HID_USAGE_GENERIC_SYSCTL_HELP_MENU      0x87
#define HID_USAGE_GENERIC_SYSCTL_MENU_EXIT      0x88
#define HID_USAGE_GENERIC_SYSCTL_MENU_SELECT    0x89
#define HID_USAGE_GENERIC_SYSCTL_MENU_RIGHT     0x8A
#define HID_USAGE_GENERIC_SYSCTL_MENU_LEFT      0x8B
#define HID_USAGE_GENERIC_SYSCTL_MENU_UP        0x8C
#define HID_USAGE_GENERIC_SYSCTL_MENU_DOWN      0x8D
/** @} */

/** Simulation Controls Page (0x02)  
 * @{ 
 */
#define HID_USAGE_SIMULATION_RUDDER             0xBA
#define HID_USAGE_SIMULATION_THROTTLE           0xBB
/** @} */

/* Virtual Reality Controls Page (0x03) */
/* ... */

/* Sport Controls Page (0x04) */
/* ... */

/* Game Controls Page (0x05) */
/* ... */

/* Generic Device Controls Page (0x06) */
/* ... */

/** Keyboard/Keypad Page (0x07)  
 * @{ 
 */
/** Error "keys" */
#define HID_USAGE_KEYBOARD_NOEVENT              0x00
#define HID_USAGE_KEYBOARD_ROLLOVER             0x01
#define HID_USAGE_KEYBOARD_POSTFAIL             0x02
#define HID_USAGE_KEYBOARD_UNDEFINED            0x03

/** Letters */
#define HID_USAGE_KEYBOARD_aA                   0x04
#define HID_USAGE_KEYBOARD_zZ                   0x1D

/** Numbers */
#define HID_USAGE_KEYBOARD_ONE                  0x1E
#define HID_USAGE_KEYBOARD_ZERO                 0x27

#define HID_USAGE_KEYBOARD_RETURN               0x28
#define HID_USAGE_KEYBOARD_ESCAPE               0x29
#define HID_USAGE_KEYBOARD_DELETE               0x2A

/** Funtion keys */
#define HID_USAGE_KEYBOARD_F1                   0x3A
#define HID_USAGE_KEYBOARD_F12                  0x45

#define HID_USAGE_KEYBOARD_PRINT_SCREEN         0x46

/** Modifier Keys */
#define HID_USAGE_KEYBOARD_LCTRL                0xE0
#define HID_USAGE_KEYBOARD_LSHFT                0xE1
#define HID_USAGE_KEYBOARD_LALT                 0xE2
#define HID_USAGE_KEYBOARD_LGUI                 0xE3
#define HID_USAGE_KEYBOARD_RCTRL                0xE4
#define HID_USAGE_KEYBOARD_RSHFT                0xE5
#define HID_USAGE_KEYBOARD_RALT                 0xE6
#define HID_USAGE_KEYBOARD_RGUI                 0xE7
#define HID_USAGE_KEYBOARD_SCROLL_LOCK          0x47
#define HID_USAGE_KEYBOARD_NUM_LOCK             0x53
#define HID_USAGE_KEYBOARD_CAPS_LOCK            0x39
/** @} */

/* ... */

/** LED Page (0x08)  
 * @{ 
 */
#define HID_USAGE_LED_NUM_LOCK                  0x01
#define HID_USAGE_LED_CAPS_LOCK                 0x02
#define HID_USAGE_LED_SCROLL_LOCK               0x03
#define HID_USAGE_LED_COMPOSE                   0x04
#define HID_USAGE_LED_KANA                      0x05
#define HID_USAGE_LED_POWER                     0x06
#define HID_USAGE_LED_SHIFT                     0x07
#define HID_USAGE_LED_DO_NOT_DISTURB            0x08
#define HID_USAGE_LED_MUTE                      0x09
#define HID_USAGE_LED_TONE_ENABLE               0x0A
#define HID_USAGE_LED_HIGH_CUT_FILTER           0x0B
#define HID_USAGE_LED_LOW_CUT_FILTER            0x0C
#define HID_USAGE_LED_EQUALIZER_ENABLE          0x0D
#define HID_USAGE_LED_SOUND_FIELD_ON            0x0E
#define HID_USAGE_LED_SURROUND_FIELD_ON         0x0F
#define HID_USAGE_LED_REPEAT                    0x10
#define HID_USAGE_LED_STEREO                    0x11
#define HID_USAGE_LED_SAMPLING_RATE_DETECT      0x12
#define HID_USAGE_LED_SPINNING                  0x13
#define HID_USAGE_LED_CAV                       0x14
#define HID_USAGE_LED_CLV                       0x15
#define HID_USAGE_LED_RECORDING_FORMAT_DET      0x16
#define HID_USAGE_LED_OFF_HOOK                  0x17
#define HID_USAGE_LED_RING                      0x18
#define HID_USAGE_LED_MESSAGE_WAITING           0x19
#define HID_USAGE_LED_DATA_MODE                 0x1A
#define HID_USAGE_LED_BATTERY_OPERATION         0x1B
#define HID_USAGE_LED_BATTERY_OK                0x1C
#define HID_USAGE_LED_BATTERY_LOW               0x1D
#define HID_USAGE_LED_SPEAKER                   0x1E
#define HID_USAGE_LED_HEAD_SET                  0x1F
#define HID_USAGE_LED_HOLD                      0x20
#define HID_USAGE_LED_MICROPHONE                0x21
#define HID_USAGE_LED_COVERAGE                  0x22
#define HID_USAGE_LED_NIGHT_MODE                0x23
#define HID_USAGE_LED_SEND_CALLS                0x24
#define HID_USAGE_LED_CALL_PICKUP               0x25
#define HID_USAGE_LED_CONFERENCE                0x26
#define HID_USAGE_LED_STAND_BY                  0x27
#define HID_USAGE_LED_CAMERA_ON                 0x28
#define HID_USAGE_LED_CAMERA_OFF                0x29
#define HID_USAGE_LED_ON_LINE                   0x2A
#define HID_USAGE_LED_OFF_LINE                  0x2B
#define HID_USAGE_LED_BUSY                      0x2C
#define HID_USAGE_LED_READY                     0x2D
#define HID_USAGE_LED_PAPER_OUT                 0x2E
#define HID_USAGE_LED_PAPER_JAM                 0x2F
#define HID_USAGE_LED_REMOTE                    0x30
#define HID_USAGE_LED_FORWARD                   0x31
#define HID_USAGE_LED_REVERSE                   0x32
#define HID_USAGE_LED_STOP                      0x33
#define HID_USAGE_LED_REWIND                    0x34
#define HID_USAGE_LED_FAST_FORWARD              0x35
#define HID_USAGE_LED_PLAY                      0x36
#define HID_USAGE_LED_PAUSE                     0x37
#define HID_USAGE_LED_RECORD                    0x38
#define HID_USAGE_LED_ERROR                     0x39
#define HID_USAGE_LED_SELECTED_INDICATOR        0x3A
#define HID_USAGE_LED_IN_USE_INDICATOR          0x3B
#define HID_USAGE_LED_MULTI_MODE_INDICATOR      0x3C
#define HID_USAGE_LED_INDICATOR_ON              0x3D
#define HID_USAGE_LED_INDICATOR_FLASH           0x3E
#define HID_USAGE_LED_INDICATOR_SLOW_BLINK      0x3F
#define HID_USAGE_LED_INDICATOR_FAST_BLINK      0x40
#define HID_USAGE_LED_INDICATOR_OFF             0x41
#define HID_USAGE_LED_FLASH_ON_TIME             0x42
#define HID_USAGE_LED_SLOW_BLINK_ON_TIME        0x43
#define HID_USAGE_LED_SLOW_BLINK_OFF_TIME       0x44
#define HID_USAGE_LED_FAST_BLINK_ON_TIME        0x45
#define HID_USAGE_LED_FAST_BLINK_OFF_TIME       0x46
#define HID_USAGE_LED_INDICATOR_COLOR           0x47
#define HID_USAGE_LED_RED                       0x48
#define HID_USAGE_LED_GREEN                     0x49
#define HID_USAGE_LED_AMBER                     0x4A
#define HID_USAGE_LED_GENERIC_INDICATOR         0x4B
/** @} */

/*  Button Page (0x09)  
 */
/*   There is no need to label these usages. */

/*  Ordinal Page (0x0A)  
 */
/*   There is no need to label these usages. */

/** Telephony Device Page (0x0B)  
 * @{ 
 */
#define HID_USAGE_TELEPHONY_PHONE               0x01
#define HID_USAGE_TELEPHONY_ANSWERING_MACHINE   0x02
#define HID_USAGE_TELEPHONY_MESSAGE_CONTROLS    0x03
#define HID_USAGE_TELEPHONY_HANDSET             0x04
#define HID_USAGE_TELEPHONY_HEADSET             0x05
#define HID_USAGE_TELEPHONY_KEYPAD              0x06
#define HID_USAGE_TELEPHONY_PROGRAMMABLE_BUTTON 0x07
/** @} */
/* ... */

/** Consumer Page (0x0C)  
 * @{ 
 */
#define HID_USAGE_CONSUMER_CONTROL              0x01
#define HID_USAGE_CONSUMER_FAST_FORWARD       0xB3
#define HID_USAGE_CONSUMER_REWIND             0xB4
#define HID_USAGE_CONSUMER_PLAY_PAUSE			    0xCD
#define HID_USAGE_CONSUMER_VOLUME_INCREMENT		0xE9
#define HID_USAGE_CONSUMER_VOLUME_DECREMENT		0xEA
/** @} */
/* ... */

/* and others ... */


/** HID Report Item Macros  
 * @{ 
 */
/** Main Items */
#define HID_Input(x)           0x81,x
#define HID_Output(x)          0x91,x
#define HID_Feature(x)         0xB1,x
#define HID_Collection(x)      0xA1,x
#define HID_EndCollection      0xC0

/** Data (Input, Output, Feature) */
#define HID_Data               0<<0
#define HID_Constant           1<<0
#define HID_Array              0<<1
#define HID_Variable           1<<1
#define HID_Absolute           0<<2
#define HID_Relative           1<<2
#define HID_NoWrap             0<<3
#define HID_Wrap               1<<3
#define HID_Linear             0<<4
#define HID_NonLinear          1<<4
#define HID_PreferredState     0<<5
#define HID_NoPreferred        1<<5
#define HID_NoNullPosition     0<<6
#define HID_NullState          1<<6
#define HID_NonVolatile        0<<7
#define HID_Volatile           1<<7

/** Collection Data */
#define HID_Physical           0x00
#define HID_Application        0x01
#define HID_Logical            0x02
#define HID_Report             0x03
#define HID_NamedArray         0x04
#define HID_UsageSwitch        0x05
#define HID_UsageModifier      0x06

/** Global Items */
#define HID_UsagePage(x)       0x05,x
#define HID_UsagePageVendor(x) 0x06,x,0xFF
#define HID_LogicalMin(x)      0x15,x
#define HID_LogicalMinS(x)     0x16,(x&0xFF),((x>>8)&0xFF)
#define HID_LogicalMinL(x)     0x17,(x&0xFF),((x>>8)&0xFF),((x>>16)&0xFF),((x>>24)&0xFF)
#define HID_LogicalMax(x)      0x25,x
#define HID_LogicalMaxS(x)     0x26,(x&0xFF),((x>>8)&0xFF)
#define HID_LogicalMaxL(x)     0x27,(x&0xFF),((x>>8)&0xFF),((x>>16)&0xFF),((x>>24)&0xFF)
#define HID_PhysicalMin(x)     0x35,x
#define HID_PhysicalMinS(x)    0x36,(x&0xFF),((x>>8)&0xFF)
#define HID_PhysicalMinL(x)    0x37,(x&0xFF),((x>>8)&0xFF),((x>>16)&0xFF),((x>>24)&0xFF)
#define HID_PhysicalMax(x)     0x45,x
#define HID_PhysicalMaxS(x)    0x46,(x&0xFF),((x>>8)&0xFF)
#define HID_PhysicalMaxL(x)    0x47,(x&0xFF),((x>>8)&0xFF),((x>>16)&0xFF),((x>>24)&0xFF)
#define HID_UnitExponent(x)    0x55,x
#define HID_Unit(x)            0x65,x
#define HID_UnitS(x)           0x66,(x&0xFF),((x>>8)&0xFF)
#define HID_UnitL(x)           0x67,(x&0xFF),((x>>8)&0xFF),((x>>16)&0xFF),((x>>24)&0xFF)
#define HID_ReportSize(x)      0x75,x
#define HID_ReportID(x)        0x85,x
#define HID_ReportCount(x)     0x95,x
#define HID_ReportCount16(x)   0x96,(x&0xFF),((x>>8)&0xFF)
#define HID_Push               0xA0
#define HID_Pop                0xB0

/** Local Items */
#define HID_Usage(x)           0x09,x
#define HID_UsageMin(x)        0x19,x
#define HID_UsageMax(x)        0x29,x
/** @} */

/** @} */

#endif  /* __HID_H__ */
