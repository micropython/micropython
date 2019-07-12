/**
  ******************************************************************************
  * @file    usbh_hid_keybd.c 
  * @author  MCD Application Team
  * @version V3.0.0
  * @date    18-February-2014
  * @brief   This file contain the USAGE page codes              
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
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
#ifndef _HID_USAGE_H_
#define _HID_USAGE_H_

/** @addtogroup USBH_LIB
  * @{
  */

/** @addtogroup USBH_CLASS
  * @{
  */

/** @addtogroup USBH_HID_CLASS
  * @{
  */
  
/** @defgroup USBH_HID_USAGE
  * @brief This file is the Header file for USBH_HID_USAGE.c
  * @{
  */ 


/** @defgroup USBH_HID_USAGE_Exported_Types
  * @{
  */

/****************************************************/
/* HID 1.11 usage pages                             */
/****************************************************/

#define HID_USAGE_PAGE_UNDEFINED  uint16_t (0x00)   /* Undefined */
/**** Top level pages */
#define HID_USAGE_PAGE_GEN_DES    uint16_t (0x01)   /* Generic Desktop Controls*/
#define HID_USAGE_PAGE_SIM_CTR    uint16_t (0x02)   /* Simulation Controls */
#define HID_USAGE_PAGE_VR_CTR     uint16_t (0x03)   /* VR Controls */
#define HID_USAGE_PAGE_SPORT_CTR  uint16_t (0x04)   /* Sport Controls */
#define HID_USAGE_PAGE_GAME_CTR   uint16_t (0x05)   /* Game Controls */
#define HID_USAGE_PAGE_GEN_DEV    uint16_t (0x06)   /* Generic Device Controls */
#define HID_USAGE_PAGE_KEYB       uint16_t (0x07)   /* Keyboard/Keypad */
#define HID_USAGE_PAGE_LED        uint16_t (0x08)   /* LEDs */
#define HID_USAGE_PAGE_BUTTON     uint16_t (0x09)   /* Button */
#define HID_USAGE_PAGE_ORDINAL    uint16_t (0x0A)   /* Ordinal */
#define HID_USAGE_PAGE_PHONE      uint16_t (0x0B)   /* Telephony */
#define HID_USAGE_PAGE_CONSUMER   uint16_t (0x0C)   /* Consumer */
#define HID_USAGE_PAGE_DIGITIZER  uint16_t (0x0D)   /* Digitizer*/
/* 0E    Reserved */
#define HID_USAGE_PAGE_PID        uint16_t (0x0F)   /* PID Page (force feedback and related devices) */
#define HID_USAGE_PAGE_UNICODE    uint16_t (0x10)   /* Unicode */
/* 11-13 Reserved */
#define HID_USAGE_PAGE_ALNUM_DISP uint16_t (0x14)   /* Alphanumeric Display */
/* 15-1f Reserved */
/**** END of top level pages */
/* 25-3f Reserved */
#define HID_USAGE_PAGE_MEDICAL    uint16_t (0x40)   /* Medical Instruments */
/* 41-7F Reserved */
/*80-83 Monitor pages USB Device Class Definition for Monitor Devices
  84-87 Power pages USB Device Class Definition for Power Devices */
/* 88-8B Reserved */
#define HID_USAGE_PAGE_BARCODE    uint16_t (0x8C)   /* Bar Code Scanner page */
#define HID_USAGE_PAGE_SCALE      uint16_t (0x8D)   /* Scale page */
#define HID_USAGE_PAGE_MSR        uint16_t (0x8E)   /* Magnetic Stripe Reading (MSR) Devices */
#define HID_USAGE_PAGE_POS        uint16_t (0x8F)   /* Reserved Point of Sale pages */
#define HID_USAGE_PAGE_CAMERA_CTR uint16_t (0x90)   /* Camera Control Page */
#define HID_USAGE_PAGE_ARCADE     uint16_t (0x91)   /* Arcade Page */

/****************************************************/
/* Usage definitions for the "Generic Decktop" page */
/****************************************************/
#define HID_USAGE_UNDEFINED     uint16_t (0x00)   /* Undefined */
#define HID_USAGE_POINTER       uint16_t (0x01)   /* Pointer (Physical Collection) */
#define HID_USAGE_MOUSE         uint16_t (0x02)   /* Mouse (Application Collection) */
/* 03 Reserved */
#define HID_USAGE_JOYSTICK      uint16_t (0x04)   /* Joystick (Application Collection) */
#define HID_USAGE_GAMEPAD       uint16_t (0x05)   /* Game Pad (Application Collection) */
#define HID_USAGE_KBD           uint16_t (0x06)   /* Keyboard (Application Collection) */
#define HID_USAGE_KEYPAD        uint16_t (0x07)   /* Keypad (Application Collection) */
#define HID_USAGE_MAX_CTR       uint16_t (0x08)   /* Multi-axis Controller (Application Collection) */
/* 09-2F Reserved */
#define HID_USAGE_X             uint16_t (0x30)   /* X (Dynamic Value) */
#define HID_USAGE_Y             uint16_t (0x31)   /* Y (Dynamic Value) */
#define HID_USAGE_Z             uint16_t (0x32)   /* Z (Dynamic Value) */
#define HID_USAGE_RX            uint16_t (0x33)   /* Rx (Dynamic Value) */
#define HID_USAGE_RY            uint16_t (0x34)   /* Ry (Dynamic Value) */
#define HID_USAGE_RZ            uint16_t (0x35)   /* Rz (Dynamic Value) */
#define HID_USAGE_SLIDER        uint16_t (0x36)   /* Slider (Dynamic Value) */
#define HID_USAGE_DIAL          uint16_t (0x37)   /* Dial (Dynamic Value) */
#define HID_USAGE_WHEEL         uint16_t (0x38)   /* Wheel (Dynamic Value) */
#define HID_USAGE_HATSW         uint16_t (0x39)   /* Hat switch (Dynamic Value) */
#define HID_USAGE_COUNTEDBUF    uint16_t (0x3A)   /* Counted Buffer (Logical Collection) */
#define HID_USAGE_BYTECOUNT     uint16_t (0x3B)   /* Byte Count (Dynamic Value) */
#define HID_USAGE_MOTIONWAKE    uint16_t (0x3C)   /* Motion Wakeup (One Shot Control) */
#define HID_USAGE_START         uint16_t (0x3D)   /* Start (On/Off Control) */
#define HID_USAGE_SELECT        uint16_t (0x3E)   /* Select (On/Off Control) */
/* 3F Reserved */
#define HID_USAGE_VX            uint16_t (0x40)   /* Vx (Dynamic Value) */
#define HID_USAGE_VY            uint16_t (0x41)   /* Vy (Dynamic Value) */
#define HID_USAGE_VZ            uint16_t (0x42)   /* Vz (Dynamic Value) */
#define HID_USAGE_VBRX          uint16_t (0x43)   /* Vbrx (Dynamic Value) */
#define HID_USAGE_VBRY          uint16_t (0x44)   /* Vbry (Dynamic Value) */
#define HID_USAGE_VBRZ          uint16_t (0x45)   /* Vbrz (Dynamic Value) */
#define HID_USAGE_VNO           uint16_t (0x46)   /* Vno (Dynamic Value) */
#define HID_USAGE_FEATNOTIF     uint16_t (0x47)   /* Feature Notification (Dynamic Value),(Dynamic Flag) */
/* 48-7F Reserved */
#define HID_USAGE_SYSCTL        uint16_t (0x80)   /* System Control (Application Collection) */
#define HID_USAGE_PWDOWN        uint16_t (0x81)   /* System Power Down (One Shot Control) */
#define HID_USAGE_SLEEP         uint16_t (0x82)   /* System Sleep (One Shot Control) */
#define HID_USAGE_WAKEUP        uint16_t (0x83)   /* System Wake Up (One Shot Control)  */
#define HID_USAGE_CONTEXTM      uint16_t (0x84)   /* System Context Menu (One Shot Control) */
#define HID_USAGE_MAINM         uint16_t (0x85)   /* System Main Menu (One Shot Control) */
#define HID_USAGE_APPM          uint16_t (0x86)   /* System App Menu (One Shot Control) */
#define HID_USAGE_MENUHELP      uint16_t (0x87)   /* System Menu Help (One Shot Control) */
#define HID_USAGE_MENUEXIT      uint16_t (0x88)   /* System Menu Exit (One Shot Control) */
#define HID_USAGE_MENUSELECT    uint16_t (0x89)   /* System Menu Select (One Shot Control) */
#define HID_USAGE_SYSM_RIGHT    uint16_t (0x8A)   /* System Menu Right (Re-Trigger Control) */
#define HID_USAGE_SYSM_LEFT     uint16_t (0x8B)   /* System Menu Left (Re-Trigger Control) */
#define HID_USAGE_SYSM_UP       uint16_t (0x8C)   /* System Menu Up (Re-Trigger Control) */
#define HID_USAGE_SYSM_DOWN     uint16_t (0x8D)   /* System Menu Down (Re-Trigger Control) */
#define HID_USAGE_COLDRESET     uint16_t (0x8E)   /* System Cold Restart (One Shot Control) */
#define HID_USAGE_WARMRESET     uint16_t (0x8F)   /* System Warm Restart (One Shot Control) */
#define HID_USAGE_DUP           uint16_t (0x90)   /* D-pad Up (On/Off Control) */
#define HID_USAGE_DDOWN         uint16_t (0x91)   /* D-pad Down (On/Off Control) */
#define HID_USAGE_DRIGHT        uint16_t (0x92)   /* D-pad Right (On/Off Control) */
#define HID_USAGE_DLEFT         uint16_t (0x93)   /* D-pad Left (On/Off Control) */
/* 94-9F Reserved */
#define HID_USAGE_SYS_DOCK      uint16_t (0xA0)   /* System Dock (One Shot Control) */
#define HID_USAGE_SYS_UNDOCK    uint16_t (0xA1)   /* System Undock (One Shot Control) */
#define HID_USAGE_SYS_SETUP     uint16_t (0xA2)   /* System Setup (One Shot Control) */
#define HID_USAGE_SYS_BREAK     uint16_t (0xA3)   /* System Break (One Shot Control) */
#define HID_USAGE_SYS_DBGBRK    uint16_t (0xA4)   /* System Debugger Break (One Shot Control) */
#define HID_USAGE_APP_BRK       uint16_t (0xA5)   /* Application Break (One Shot Control) */
#define HID_USAGE_APP_DBGBRK    uint16_t (0xA6)   /* Application Debugger Break (One Shot Control) */
#define HID_USAGE_SYS_SPKMUTE   uint16_t (0xA7)   /* System Speaker Mute (One Shot Control) */
#define HID_USAGE_SYS_HIBERN    uint16_t (0xA8)   /* System Hibernate (One Shot Control) */
/* A9-AF Reserved */
#define HID_USAGE_SYS_SIDPINV   uint16_t (0xB0)   /* System Display Invert (One Shot Control) */
#define HID_USAGE_SYS_DISPINT   uint16_t (0xB1)   /* System Display Internal (One Shot Control) */
#define HID_USAGE_SYS_DISPEXT   uint16_t (0xB2)   /* System Display External (One Shot Control) */
#define HID_USAGE_SYS_DISPBOTH  uint16_t (0xB3)   /* System Display Both (One Shot Control) */
#define HID_USAGE_SYS_DISPDUAL  uint16_t (0xB4)   /* System Display Dual (One Shot Control) */
#define HID_USAGE_SYS_DISPTGLIE uint16_t (0xB5)   /* System Display Toggle Int/Ext (One Shot Control) */
#define HID_USAGE_SYS_DISP_SWAP uint16_t (0xB6)   /* System Display Swap Primary/Secondary (One Shot Control) */
#define HID_USAGE_SYS_DIPS_LCDA uint16_t (0xB7)   /* System Display LCD Autoscale (One Shot Control) */
/* B8-FFFF Reserved */

/**
  * @}
  */ 

#endif /* _HID_USAGE_H_ */

/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */ 
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
