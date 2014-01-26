/**
  ******************************************************************************
  * @file    usbh_hid_keybd.c 
  * @author  MCD Application Team
  * @version V2.1.0
  * @date    19-March-2012
  * @brief   This file is the application layer for USB Host HID Keyboard handling
  *          QWERTY and AZERTY Keyboard are supported as per the selection in 
  *          usbh_hid_keybd.h              
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
#include "usbh_hid_keybd.h"

/** @addtogroup USBH_LIB
* @{
*/

/** @addtogroup USBH_CLASS
* @{
*/

/** @addtogroup USBH_HID_CLASS
* @{
*/

/** @defgroup USBH_HID_KEYBD 
* @brief    This file includes HID Layer Handlers for USB Host HID class.
* @{
*/ 

/** @defgroup USBH_HID_KEYBD_Private_TypesDefinitions
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBH_HID_KEYBD_Private_Defines
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBH_HID_KEYBD_Private_Macros
* @{
*/ 
/**
* @}
*/ 

/** @defgroup USBH_HID_KEYBD_Private_FunctionPrototypes
* @{
*/ 
static void  KEYBRD_Init (void);
static void  KEYBRD_Decode(uint8_t *data);

/**
* @}
*/ 

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
 #if defined   (__CC_ARM) /*!< ARM Compiler */
  __align(4) 
 #elif defined ( __ICCARM__ ) /*!< IAR Compiler */
  #pragma data_alignment=4
 #elif defined (__GNUC__) /*!< GNU Compiler */
 #pragma pack(4) 
 #elif defined  (__TASKING__) /*!< TASKING Compiler */                           
  __align(4) 
 #endif /* __CC_ARM */
#endif
 
/** @defgroup USBH_HID_KEYBD_Private_Variables
* @{
*/
HID_cb_TypeDef HID_KEYBRD_cb= 
{
  KEYBRD_Init,
  KEYBRD_Decode
};

/*
*******************************************************************************
*                                             LOCAL CONSTANTS
*******************************************************************************
*/

static  const  uint8_t  HID_KEYBRD_Codes[] = {
  0,     0,    0,    0,   31,   50,   48,   33, 
  19,   34,   35,   36,   24,   37,   38,   39,       /* 0x00 - 0x0F */
  52,    51,   25,   26,   17,   20,   32,   21,
  23,   49,   18,   47,   22,   46,    2,    3,       /* 0x10 - 0x1F */
  4,    5,    6,    7,    8,    9,   10,   11, 
  43,  110,   15,   16,   61,   12,   13,   27,       /* 0x20 - 0x2F */
  28,   29,   42,   40,   41,    1,   53,   54,  
  55,   30,  112,  113,  114,  115,  116,  117,       /* 0x30 - 0x3F */
  118,  119,  120,  121,  122,  123,  124,  125,  
  126,   75,   80,   85,   76,   81,   86,   89,       /* 0x40 - 0x4F */
  79,   84,   83,   90,   95,  100,  105,  106,
  108,   93,   98,  103,   92,   97,  102,   91,       /* 0x50 - 0x5F */
  96,  101,   99,  104,   45,  129,    0,    0, 
  0,    0,    0,    0,    0,    0,    0,    0,       /* 0x60 - 0x6F */
  0,    0,    0,    0,    0,    0,    0,    0,
  0,    0,    0,    0,    0,    0,    0,    0,       /* 0x70 - 0x7F */
  0,    0,    0,    0,    0,  107,    0,   56,
  0,    0,    0,    0,    0,    0,    0,    0,       /* 0x80 - 0x8F */
  0,    0,    0,    0,    0,    0,    0,    0,
  0,    0,    0,    0,    0,    0,    0,    0,       /* 0x90 - 0x9F */
  0,    0,    0,    0,    0,    0,    0,    0,
  0,    0,    0,    0,    0,    0,    0,    0,       /* 0xA0 - 0xAF */
  0,    0,    0,    0,    0,    0,    0,    0, 
  0,    0,    0,    0,    0,    0,    0,    0,       /* 0xB0 - 0xBF */
  0,    0,    0,    0,    0,    0,    0,    0, 
  0,    0,    0,    0,    0,    0,    0,    0,       /* 0xC0 - 0xCF */
  0,    0,    0,    0,    0,    0,    0,    0, 
  0,    0,    0,    0,    0,    0,    0,    0,       /* 0xD0 - 0xDF */
  58,   44,   60,  127,   64,   57,   62,  128        /* 0xE0 - 0xE7 */
};

#ifdef QWERTY_KEYBOARD
static  const  int8_t  HID_KEYBRD_Key[] = {
  '\0',  '`',  '1',  '2',  '3',  '4',  '5',  '6',
  '7',  '8',  '9',  '0',  '-',  '=',  '\0', 127, // dpgeorge 127 used to be \r (it's backspace)
  '\t',  'q',  'w',  'e',  'r',  't',  'y',  'u', 
  'i',  'o',  'p',  '[',  ']',  '\\',
  '\0',  'a',  's',  'd',  'f',  'g',  'h',  'j',  
  'k',  'l',  ';',  '\'', '\0', '\r', // dpgeorge \r used to be \n (it's enter)
  '\0',  '\0', 'z',  'x',  'c',  'v',  'b',  'n', 
  'm',  ',',  '.',  '/',  '\0', '\0',
  '\0',  '\0', '\0', ' ',  '\0', '\0', '\0', '\0', 
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0',  '\0', '\0', '\0', '\0', '\r', '\0', '\0', 
  '\0', '\0', '\0', '\0', '\0', '\0',
  '\0',  '\0', '7',  '4',  '1',
  '\0',  '/',  '8',  '5',  '2',
  '0',   '*',  '9',  '6',  '3',
  '.',   '-',  '+',  '\0', '\n', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0',  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', 
  '\0', '\0', '\0', '\0'
};

static  const  int8_t  HID_KEYBRD_ShiftKey[] = {
  '\0', '~',  '!',  '@',  '#',  '$',  '%',  '^',  '&',  '*',  '(',  ')',
  '_',  '+',  '\0', '\0', '\0', 'Q',  'W',  'E',  'R',  'T',  'Y',  'U', 
  'I',  'O',  'P',  '{',  '}',  '|',  '\0', 'A',  'S',  'D',  'F',  'G', 
  'H',  'J',  'K',  'L',  ':',  '"',  '\0', '\r', '\0', '\0', 'Z',  'X',   // dpgeorge \r used to be \n (it's enter)
  'C',  'V',  'B',  'N',  'M',  '<',  '>',  '?',  '\0', '\0',  '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', 
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', 
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0',    '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'
};

static  const  int8_t  HID_KEYBRD_CtrlKey[] = {
  '\0', '~',  '!',  '@',  '#',  '$',  '%',  '^',  '&',  '*',  '(',  ')',
  '_',  '+',  '\0', '\0', '\0', 'Q',  'W',  'E',  'R',  'T',  'Y',  'U', 
  'I',  'O',  'P',  '{',  '}',  '|',  '\0', 'A',  'S',  4,  'F',  'G',
  'H',  'J',  'K',  'L',  ':',  '"',  '\0', '\r', '\0', '\0', 'Z',  'X',
  'C',  'V',  'B',  'N',  'M',  '<',  '>',  '?',  '\0', '\0',  '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', 
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', 
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0',    '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'
};

#else

static  const  int8_t  HID_KEYBRD_Key[] = {
  '\0',  '`',  '1',  '2',  '3',  '4',  '5',  '6',  '7',  '8',  '9',  '0',
  '-',  '=',  '\0', '\r', '\t',  'a',  'z',  'e',  'r',  't',  'y',  'u', 
  'i',  'o',  'p',  '[',  ']', '\\', '\0',  'q',  's',  'd',  'f',  'g', 
  'h',  'j',  'k',  'l',  'm',  '\0', '\0', '\n', '\0',  '\0', 'w',  'x', 
  'c',  'v',  'b',  'n',  ',',  ';',  ':',  '!',  '\0', '\0', '\0',  '\0', 
  '\0', ' ',  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', 
  '\0', '\0', '\0', '\0',  '\0', '\0', '\0', '\0', '\r', '\0', '\0', '\0', 
  '\0', '\0', '\0', '\0', '\0', '\0',  '\0', '7',  '4',  '1','\0',  '/', 
  '8',  '5',  '2', '0',   '*',  '9',  '6',  '3', '.',   '-',  '+',  '\0', 
  '\n', '\0', '\0', '\0', '\0', '\0', '\0','\0',  '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'
};

static  const  int8_t  HID_KEYBRD_ShiftKey[] = {
  '\0', '~',  '!',  '@',  '#',  '$',  '%',  '^',  '&',  '*',  '(',  ')',  '_',
  '+',  '\0', '\0', '\0', 'A',  'Z',  'E',  'R',  'T',  'Y',  'U',  'I',  'O',
  'P',  '{',  '}',  '*', '\0', 'Q',  'S',  'D',  'F',  'G',  'H',  'J',  'K', 
  'L',  'M',  '%',  '\0', '\n', '\0', '\0', 'W',  'X',  'C',  'V',  'B',  'N',
  '?',  '.',  '/',  '\0',  '\0', '\0','\0', '\0', '\0', '\0', '\0', '\0', '\0', 
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', 
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'
};
#endif

/**
* @}
*/ 


/** @defgroup USBH_HID_KEYBD_Private_Functions
* @{
*/ 


/**
* @brief  KEYBRD_Init.
*         Initialize the keyboard function.
* @param  None
* @retval None
*/
static void  KEYBRD_Init (void)
{
  /* Call User Init*/
  USR_KEYBRD_Init();
}

/**
* @brief  KEYBRD_ProcessData.
*         The function is to decode the pressed keys.
* @param  pbuf : Pointer to the HID IN report data buffer
* @retval None
*/

static void KEYBRD_Decode(uint8_t *pbuf)
{
  static  uint8_t   shift;
  static  uint8_t   ctrl;
  static  uint8_t   keys[KBR_MAX_NBR_PRESSED];
  static  uint8_t   keys_new[KBR_MAX_NBR_PRESSED];
  static  uint8_t   keys_last[KBR_MAX_NBR_PRESSED];
  static  uint8_t   key_newest;
  static  uint8_t   nbr_keys;
  static  uint8_t   nbr_keys_new;
  static  uint8_t   nbr_keys_last;
  uint8_t   ix;
  uint8_t   jx;
  uint8_t   error;
  uint8_t   output;            
  
  nbr_keys      = 0;
  nbr_keys_new  = 0;
  nbr_keys_last = 0;
  key_newest    = 0x00;
  
  
  /* Check if Shift key is pressed */                                                                         
  if ((pbuf[0] == KBD_LEFT_SHIFT) || (pbuf[0] == KBD_RIGHT_SHIFT)) {
    shift = TRUE;
  } else {
    shift = FALSE;
  }
  /* Check if Ctrl key is pressed */                                                                         
  if ((pbuf[0] == KBD_LEFT_CTRL) || (pbuf[0] == KBD_RIGHT_CTRL)) {
    ctrl = TRUE;
  } else {
    ctrl = FALSE;
  }
  
  error = FALSE;
  
  /* Check for the value of pressed key */
  for (ix = 2; ix < 2 + KBR_MAX_NBR_PRESSED; ix++) {                       
    if ((pbuf[ix] == 0x01) ||
        (pbuf[ix] == 0x02) ||
          (pbuf[ix] == 0x03)) {
            error = TRUE;
          }
  }
  
  if (error == TRUE) {
    return;
  }
  
  nbr_keys     = 0;
  nbr_keys_new = 0;
  for (ix = 2; ix < 2 + KBR_MAX_NBR_PRESSED; ix++) {
    if (pbuf[ix] != 0) {
      keys[nbr_keys] = pbuf[ix];                                       
      nbr_keys++;
      for (jx = 0; jx < nbr_keys_last; jx++) {                         
        if (pbuf[ix] == keys_last[jx]) {
          break;
        }
      }
      
      if (jx == nbr_keys_last) {
        keys_new[nbr_keys_new] = pbuf[ix];
        nbr_keys_new++;
      }
    }
  }
  
  if (nbr_keys_new == 1) {
    key_newest = keys_new[0];
    
    if (ctrl == TRUE) {
      output =  HID_KEYBRD_CtrlKey[HID_KEYBRD_Codes[key_newest]];
    } else if (shift == TRUE) {
      output =  HID_KEYBRD_ShiftKey[HID_KEYBRD_Codes[key_newest]];
    } else {
      output =  HID_KEYBRD_Key[HID_KEYBRD_Codes[key_newest]];
    }
    
    /* call user process handle */
    USR_KEYBRD_ProcessData(output);
  } else {
    key_newest = 0x00;
  }
  
  
  nbr_keys_last  = nbr_keys;
  for (ix = 0; ix < KBR_MAX_NBR_PRESSED; ix++) {
    keys_last[ix] = keys[ix];
  }
}

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


/**
* @}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

